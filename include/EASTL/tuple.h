///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_TUPLE_H
#define EASTL_TUPLE_H

#include <EASTL/internal/config.h>
#include <EASTL/functional.h>
#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EASTL/internal/tuple_fwd_decls.h>

EA_DISABLE_VC_WARNING(4623) // warning C4623: default constructor was implicitly defined as deleted
EA_DISABLE_VC_WARNING(4625) // warning C4625: copy constructor was implicitly defined as deleted
EA_DISABLE_VC_WARNING(4510) // warning C4510: default constructor could not be generated

#if EASTL_TUPLE_ENABLED

namespace eastl
{
// non-recursive tuple implementation based on libc++ tuple implementation and description at
// http://mitchnull.blogspot.ca/2012/06/c11-tuple-implementation-details-part-1.html

// TupleTypes helper
template <typename... Ts> struct TupleTypes {};

// tuple_size helper
template <typename T> class tuple_size {};
template <typename T> class tuple_size<const T>          : public tuple_size<T> {};
template <typename T> class tuple_size<volatile T>       : public tuple_size<T> {};
template <typename T> class tuple_size<const volatile T> : public tuple_size<T> {};

template <typename... Ts> class tuple_size<TupleTypes<Ts...>> : public integral_constant<size_t, sizeof...(Ts)> {};
template <typename... Ts> class tuple_size<tuple<Ts...>>      : public integral_constant<size_t, sizeof...(Ts)> {};

#if EASTL_VARIABLE_TEMPLATES_ENABLED
	template <class T>
	EA_CONSTEXPR size_t tuple_size_v = tuple_size<T>::value;
#endif

namespace Internal
{
	template <typename TupleIndices, typename... Ts>
	struct TupleImpl;
} // namespace Internal

template <typename Indices, typename... Ts>
class tuple_size<Internal::TupleImpl<Indices, Ts...>> : public integral_constant<size_t, sizeof...(Ts)>
{
};

// tuple_element helper to be able to isolate a type given an index
template <size_t I, typename T>
class tuple_element
{
};

template <size_t I>
class tuple_element<I, TupleTypes<>>
{
public:
	static_assert(I != I, "tuple_element index out of range");
};

template <typename H, typename... Ts>
class tuple_element<0, TupleTypes<H, Ts...>>
{
public:
	typedef H type;
};

template <size_t I, typename H, typename... Ts>
class tuple_element<I, TupleTypes<H, Ts...>>
{
public:
	typedef tuple_element_t<I - 1, TupleTypes<Ts...>> type;
};

// specialization for tuple
template <size_t I, typename... Ts>
class tuple_element<I, tuple<Ts...>>
{
public:
	typedef tuple_element_t<I, TupleTypes<Ts...>> type;
};

template <size_t I, typename... Ts>
class tuple_element<I, const tuple<Ts...>>
{
public:
	typedef typename add_const<tuple_element_t<I, TupleTypes<Ts...>>>::type type;
};

template <size_t I, typename... Ts>
class tuple_element<I, volatile tuple<Ts...>>
{
public:
	typedef typename add_volatile<tuple_element_t<I, TupleTypes<Ts...>>>::type type;
};

template <size_t I, typename... Ts>
class tuple_element<I, const volatile tuple<Ts...>>
{
public:
	typedef typename add_cv<tuple_element_t<I, TupleTypes<Ts...>>>::type type;
};

// specialization for TupleImpl
template <size_t I, typename Indices, typename... Ts>
class tuple_element<I, Internal::TupleImpl<Indices, Ts...>> : public tuple_element<I, tuple<Ts...>>
{
};

template <size_t I, typename Indices, typename... Ts>
class tuple_element<I, const Internal::TupleImpl<Indices, Ts...>> : public tuple_element<I, const tuple<Ts...>>
{
};

template <size_t I, typename Indices, typename... Ts>
class tuple_element<I, volatile Internal::TupleImpl<Indices, Ts...>> : public tuple_element<I, volatile tuple<Ts...>>
{
};

template <size_t I, typename Indices, typename... Ts>
class tuple_element<I, const volatile Internal::TupleImpl<Indices, Ts...>> : public tuple_element<
																				 I, const volatile tuple<Ts...>>
{
};

// attempt to isolate index given a type
template <typename T, typename Tuple>
struct tuple_index
{
};

template <typename T>
struct tuple_index<T, TupleTypes<>>
{
	typedef void DuplicateTypeCheck;
	tuple_index() = delete; // tuple_index should only be used for compile-time assistance, and never be instantiated
	static const size_t index = 0;
};

template <typename T, typename... TsRest>
struct tuple_index<T, TupleTypes<T, TsRest...>>
{
	typedef int DuplicateTypeCheck;
	// after finding type T in the list of types, try to find type T in TsRest.
	// If we stumble back into this version of tuple_index, i.e. type T appears twice in the list of types, then DuplicateTypeCheck will be of type int, and the static_assert will fail.
	// If we don't, then we'll go through the version of tuple_index above, where all of the types have been exhausted, and DuplicateTypeCheck will be void.
	static_assert(is_void<typename tuple_index<T, TupleTypes<TsRest...>>::DuplicateTypeCheck>::value, "duplicate type T in tuple_vector::get<T>(); unique types must be provided in declaration, or only use get<size_t>()");

	static const size_t index = 0;
};

template <typename T, typename TsHead, typename... TsRest>
struct tuple_index<T, TupleTypes<TsHead, TsRest...>>
{
	typedef typename tuple_index<T, TupleTypes<TsRest...>>::DuplicateTypeCheck DuplicateTypeCheck;
	static const size_t index = tuple_index<T, TupleTypes<TsRest...>>::index + 1;
};

template <typename T, typename Indices, typename... Ts>
struct tuple_index<T, Internal::TupleImpl<Indices, Ts...>> : public tuple_index<T, TupleTypes<Ts...>>
{
};

namespace Internal
{

// TupleLeaf
template <size_t I, typename ValueType, bool IsEmpty = is_empty<ValueType>::value>
class TupleLeaf;

template <size_t I, typename ValueType, bool IsEmpty>
inline void swap(TupleLeaf<I, ValueType, IsEmpty>& a, TupleLeaf<I, ValueType, IsEmpty>& b)
{
	eastl::swap(a.getInternal(), b.getInternal());
}

template <size_t I, typename ValueType, bool IsEmpty>
class TupleLeaf
{
public:
	TupleLeaf() : mValue() {}
	TupleLeaf(const TupleLeaf&) = default;
	TupleLeaf& operator=(const TupleLeaf&) = delete;

	// We shouldn't need this explicit constructor as it should be handled by the template below but OSX clang
	// is_constructible type trait incorrectly gives false for is_constructible<T&&, T&&>::value
	explicit TupleLeaf(ValueType&& v) : mValue(move(v)) {}

	template <typename T, typename = typename enable_if<is_constructible<ValueType, T&&>::value>::type>
	explicit TupleLeaf(T&& t)
		: mValue(forward<T>(t))
	{
	}

	template <typename T>
	explicit TupleLeaf(const TupleLeaf<I, T>& t)
		: mValue(t.getInternal())
	{
	}

	template <typename T>
	TupleLeaf& operator=(T&& t)
	{
		mValue = forward<T>(t);
		return *this;
	}

	int swap(TupleLeaf& t)
	{
		eastl::Internal::swap(*this, t);
		return 0;
	}

	ValueType& getInternal() { return mValue; }
	const ValueType& getInternal() const { return mValue; }

private:
	ValueType mValue;  
};

// Specialize for when ValueType is a reference 
template <size_t I, typename ValueType, bool IsEmpty>
class TupleLeaf<I, ValueType&, IsEmpty>
{
public:
	TupleLeaf(const TupleLeaf&) = default;
	TupleLeaf& operator=(const TupleLeaf&) = delete;

	template <typename T, typename = typename enable_if<is_constructible<ValueType, T&&>::value>::type>
	explicit TupleLeaf(T&& t)
		: mValue(forward<T>(t))
	{
	}

	explicit TupleLeaf(ValueType& t) : mValue(t)
	{
	}

	template <typename T>
	explicit TupleLeaf(const TupleLeaf<I, T>& t)
		: mValue(t.getInternal())
	{
	}

	template <typename T>
	TupleLeaf& operator=(T&& t)
	{
		mValue = forward<T>(t);
		return *this;
	}

	int swap(TupleLeaf& t)
	{
		eastl::Internal::swap(*this, t);
		return 0;
	}

	ValueType& getInternal() { return mValue; }
	const ValueType& getInternal() const { return mValue; }

private:
	ValueType& mValue;
};

// TupleLeaf partial specialization for when we can use the Empty Base Class Optimization
template <size_t I, typename ValueType>
class TupleLeaf<I, ValueType, true> : private ValueType
{
public:
	// true_type / false_type constructors for case where ValueType is default constructible and should be value
	// initialized and case where it is not
	TupleLeaf(const TupleLeaf&) = default;

	template <typename T, typename = typename enable_if<is_constructible<ValueType, T&&>::value>::type>
	explicit TupleLeaf(T&& t)
		: ValueType(forward<T>(t))
	{
	}

	template <typename T>
	explicit TupleLeaf(const TupleLeaf<I, T>& t)
		: ValueType(t.getInternal())
	{
	}

	template <typename T>
	TupleLeaf& operator=(T&& t)
	{
		ValueType::operator=(forward<T>(t));
		return *this;
	}

	int swap(TupleLeaf& t)
	{
		eastl::Internal::swap(*this, t);
		return 0;
	}

	ValueType& getInternal() { return static_cast<ValueType&>(*this); }
	const ValueType& getInternal() const { return static_cast<const ValueType&>(*this); }

private:
	TupleLeaf& operator=(const TupleLeaf&) = delete;
};

// MakeTupleTypes

template <typename TupleTypes, typename Tuple, size_t Start, size_t End>
struct MakeTupleTypesImpl;

template <typename... Types, typename Tuple, size_t Start, size_t End>
struct MakeTupleTypesImpl<TupleTypes<Types...>, Tuple, Start, End>
{
	typedef typename remove_reference<Tuple>::type TupleType;
	typedef typename MakeTupleTypesImpl<
		TupleTypes<Types..., typename conditional<is_lvalue_reference<Tuple>::value,
												  // append ref if Tuple is ref
												  tuple_element_t<Start, TupleType>&,
												  // append non-ref otherwise
												  tuple_element_t<Start, TupleType>>::type>,
		Tuple, Start + 1, End>::type type;
};

template <typename... Types, typename Tuple, size_t End>
struct MakeTupleTypesImpl<TupleTypes<Types...>, Tuple, End, End>
{
	typedef TupleTypes<Types...> type;
};

template <typename Tuple>
using MakeTupleTypes_t = typename MakeTupleTypesImpl<TupleTypes<>, Tuple, 0,
													 tuple_size<typename remove_reference<Tuple>::type>::value>::type;

// TupleImpl

template <typename... Ts>
void swallow(Ts&&...)
{
}

template <size_t I, typename Indices, typename... Ts>
tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(TupleImpl<Indices, Ts...>& t);

template <size_t I, typename Indices, typename... Ts>
const_tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(const TupleImpl<Indices, Ts...>& t);

template <size_t I, typename Indices, typename... Ts>
tuple_element_t<I, TupleImpl<Indices, Ts...>>&& get(TupleImpl<Indices, Ts...>&& t);

template <typename T, typename Indices, typename... Ts>
T& get(TupleImpl<Indices, Ts...>& t);

template <typename T, typename Indices, typename... Ts>
const T& get(const TupleImpl<Indices, Ts...>& t);

template <typename T, typename Indices, typename... Ts>
T&& get(TupleImpl<Indices, Ts...>&& t);

template <size_t... Indices, typename... Ts>
struct TupleImpl<integer_sequence<size_t, Indices...>, Ts...> : public TupleLeaf<Indices, Ts>...
{
	EA_CONSTEXPR TupleImpl() = default;

	// index_sequence changed to integer_sequence due to issues described below in VS2015 CTP 6. 
	// https://connect.microsoft.com/VisualStudio/feedback/details/1126958/error-in-template-parameter-pack-expansion-of-std-index-sequence
	// 
	template <typename... Us, typename... ValueTypes>
	explicit TupleImpl(integer_sequence<size_t, Indices...>, TupleTypes<Us...>, ValueTypes&&... values)
		: TupleLeaf<Indices, Ts>(forward<ValueTypes>(values))...
	{
	}

	template <typename OtherTuple>
	TupleImpl(OtherTuple&& t)
		: TupleLeaf<Indices, Ts>(forward<tuple_element_t<Indices, MakeTupleTypes_t<OtherTuple>>>(get<Indices>(t)))...
	{
	}

	template <typename OtherTuple>
	TupleImpl& operator=(OtherTuple&& t)
	{
		swallow(TupleLeaf<Indices, Ts>::operator=(
			forward<tuple_element_t<Indices, MakeTupleTypes_t<OtherTuple>>>(get<Indices>(t)))...);
		return *this;
	}

	TupleImpl& operator=(const TupleImpl& t)
	{
		swallow(TupleLeaf<Indices, Ts>::operator=(static_cast<const TupleLeaf<Indices, Ts>&>(t).getInternal())...);
		return *this;
	}

	void swap(TupleImpl& t) { swallow(TupleLeaf<Indices, Ts>::swap(static_cast<TupleLeaf<Indices, Ts>&>(t))...); }
};

template <size_t I, typename Indices, typename... Ts>
inline tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(TupleImpl<Indices, Ts...>& t)
{
	typedef tuple_element_t<I, TupleImpl<Indices, Ts...>> Type;
	return static_cast<Internal::TupleLeaf<I, Type>&>(t).getInternal();
}

template <size_t I, typename Indices, typename... Ts>
inline const_tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(const TupleImpl<Indices, Ts...>& t)
{
	typedef tuple_element_t<I, TupleImpl<Indices, Ts...>> Type;
	return static_cast<const Internal::TupleLeaf<I, Type>&>(t).getInternal();
}

template <size_t I, typename Indices, typename... Ts>
inline tuple_element_t<I, TupleImpl<Indices, Ts...>>&& get(TupleImpl<Indices, Ts...>&& t)
{
	typedef tuple_element_t<I, TupleImpl<Indices, Ts...>> Type;
	return static_cast<Type&&>(static_cast<Internal::TupleLeaf<I, Type>&>(t).getInternal());
}

template <typename T, typename Indices, typename... Ts>
inline T& get(TupleImpl<Indices, Ts...>& t)
{
	typedef tuple_index<T, TupleImpl<Indices, Ts...>> Index;
	return static_cast<Internal::TupleLeaf<Index::index, T>&>(t).getInternal();
}

template <typename T, typename Indices, typename... Ts>
inline const T& get(const TupleImpl<Indices, Ts...>& t)
{
	typedef tuple_index<T, TupleImpl<Indices, Ts...>> Index;
	return static_cast<const Internal::TupleLeaf<Index::index, T>&>(t).getInternal();
}

template <typename T, typename Indices, typename... Ts>
inline T&& get(TupleImpl<Indices, Ts...>&& t)
{
	typedef tuple_index<T, TupleImpl<Indices, Ts...>> Index;
	return static_cast<T&&>(static_cast<Internal::TupleLeaf<Index::index, T>&>(t).getInternal());
}

// TupleLike

template <typename T>
struct TupleLike : public false_type
{
};

template <typename T>
struct TupleLike<const T> : public TupleLike<T>
{
};

template <typename T>
struct TupleLike<volatile T> : public TupleLike<T>
{
};

template <typename T>
struct TupleLike<const volatile T> : public TupleLike<T>
{
};

template <typename... Ts>
struct TupleLike<tuple<Ts...>> : public true_type
{
};

template <typename First, typename Second>
struct TupleLike<eastl::pair<First, Second>> : public true_type
{
};

// TupleConvertible

template <bool IsSameSize, typename From, typename To>
struct TupleConvertibleImpl : public false_type
{
};

template <typename FromFirst, typename... FromRest, typename ToFirst, typename... ToRest>
struct TupleConvertibleImpl<
	true, TupleTypes<FromFirst, FromRest...>,
	TupleTypes<ToFirst, ToRest...>> : public integral_constant<bool,
															   is_convertible<FromFirst, ToFirst>::value&&
																   TupleConvertibleImpl<true, TupleTypes<FromRest...>,
																						TupleTypes<ToRest...>>::value>
{
};

template <>
struct TupleConvertibleImpl<true, TupleTypes<>, TupleTypes<>> : public true_type
{
};

template <typename From,
		  typename To,
		  bool = TupleLike<typename remove_reference<From>::type>::value,
		  bool = TupleLike<typename remove_reference<To>::type>::value>
struct TupleConvertible : public false_type
{
};

template <typename From, typename To>
struct TupleConvertible<From, To, true, true>
	: public TupleConvertibleImpl<tuple_size<typename remove_reference<From>::type>::value ==
		                              tuple_size<typename remove_reference<To>::type>::value,
		                          MakeTupleTypes_t<From>,
		                          MakeTupleTypes_t<To>>
{
};

// TupleAssignable

template <bool IsSameSize, typename Target, typename From>
struct TupleAssignableImpl : public false_type
{
};

template <typename TargetFirst, typename... TargetRest, typename FromFirst, typename... FromRest>
struct TupleAssignableImpl<true, TupleTypes<TargetFirst, TargetRest...>, TupleTypes<FromFirst, FromRest...>>
	: public bool_constant<is_assignable<TargetFirst, FromFirst>::value &&
		                   TupleAssignableImpl<true, TupleTypes<TargetRest...>, TupleTypes<FromRest...>>::value>
{
};

template <>
struct TupleAssignableImpl<true, TupleTypes<>, TupleTypes<>> : public true_type
{
};

template <typename Target,
		  typename From,
		  bool = TupleLike<typename remove_reference<Target>::type>::value,
		  bool = TupleLike<typename remove_reference<From>::type>::value>
struct TupleAssignable : public false_type
{
};

template <typename Target, typename From>
struct TupleAssignable<Target, From, true, true>
	: public TupleAssignableImpl<tuple_size<typename remove_reference<Target>::type>::value == tuple_size<typename remove_reference<From>::type>::value,
		                         MakeTupleTypes_t<Target>,
		                         MakeTupleTypes_t<From>>
{
};

// TupleEqual

template <size_t I>
struct TupleEqual
{
	template <typename Tuple1, typename Tuple2>
	bool operator()(const Tuple1& t1, const Tuple2& t2)
	{
		static_assert(tuple_size<Tuple1>::value == tuple_size<Tuple2>::value, "comparing tuples of different sizes.");
		return TupleEqual<I - 1>()(t1, t2) && get<I - 1>(t1) == get<I - 1>(t2);
	}
};

template <>
struct TupleEqual<0>
{
	template <typename Tuple1, typename Tuple2>
	bool operator()(const Tuple1&, const Tuple2&)
	{
		return true;
	}
};

// TupleLess

template <size_t I>
struct TupleLess
{
	template <typename Tuple1, typename Tuple2>
	bool operator()(const Tuple1& t1, const Tuple2& t2)
	{
		static_assert(tuple_size<Tuple1>::value == tuple_size<Tuple2>::value, "comparing tuples of different sizes.");
		return TupleLess<I - 1>()(t1, t2) || (!TupleLess<I - 1>()(t2, t1) && get<I - 1>(t1) < get<I - 1>(t2));
	}
};

template <>
struct TupleLess<0>
{
	template <typename Tuple1, typename Tuple2>
	bool operator()(const Tuple1&, const Tuple2&)
	{
		return false;
	}
};

template <typename T>
struct MakeTupleReturnImpl
{
	typedef T type;
};

template <typename T>
struct MakeTupleReturnImpl<reference_wrapper<T>>
{
	typedef T& type;
};

template <typename T>
using MakeTupleReturn_t = typename MakeTupleReturnImpl<typename decay<T>::type>::type;

struct ignore_t
{
	ignore_t() {}

	template <typename T>
	const ignore_t& operator=(const T&) const
	{
		return *this;
	}
};

// tuple_cat helpers
template <typename Tuple1, typename Is1, typename Tuple2, typename Is2>
struct TupleCat2Impl;

template <typename... T1s, size_t... I1s, typename... T2s, size_t... I2s>
struct TupleCat2Impl<tuple<T1s...>, index_sequence<I1s...>, tuple<T2s...>, index_sequence<I2s...>>
{
	typedef tuple<T1s..., T2s...> ResultType;

	template <typename Tuple1, typename Tuple2>
	static inline ResultType DoCat2(Tuple1&& t1, Tuple2&& t2)
	{
		return ResultType(get<I1s>(forward<Tuple1>(t1))..., get<I2s>(forward<Tuple2>(t2))...);
	}
};

template <typename Tuple1, typename Tuple2>
struct TupleCat2;

template <typename... T1s, typename... T2s>
struct TupleCat2<tuple<T1s...>, tuple<T2s...>>
{
	typedef make_index_sequence<sizeof...(T1s)> Is1;
	typedef make_index_sequence<sizeof...(T2s)> Is2;
	typedef TupleCat2Impl<tuple<T1s...>, Is1, tuple<T2s...>, Is2> TCI;
	typedef typename TCI::ResultType ResultType;

	template <typename Tuple1, typename Tuple2>
	static inline ResultType DoCat2(Tuple1&& t1, Tuple2&& t2)
	{
		return TCI::DoCat2(forward<Tuple1>(t1), forward<Tuple2>(t2));
	}
};

template <typename... Tuples>
struct TupleCat;

template <typename Tuple1, typename Tuple2, typename... TuplesRest>
struct TupleCat<Tuple1, Tuple2, TuplesRest...>
{
	typedef typename TupleCat2<Tuple1, Tuple2>::ResultType FirstResultType;
	typedef typename TupleCat<FirstResultType, TuplesRest...>::ResultType ResultType;

	template <typename TupleArg1, typename TupleArg2, typename... TupleArgsRest>
	static inline ResultType DoCat(TupleArg1&& t1, TupleArg2&& t2, TupleArgsRest&&... ts)
	{
		return TupleCat<FirstResultType, TuplesRest...>::DoCat(
			TupleCat2<TupleArg1, TupleArg2>::DoCat2(forward<TupleArg1>(t1), forward<TupleArg2>(t2)),
			forward<TupleArgsRest>(ts)...);
	}
};

template <typename Tuple1, typename Tuple2>
struct TupleCat<Tuple1, Tuple2>
{
	typedef typename TupleCat2<Tuple1, Tuple2>::ResultType ResultType;

	template <typename TupleArg1, typename TupleArg2>
	static inline ResultType DoCat(TupleArg1&& t1, TupleArg2&& t2)
	{
		return TupleCat2<TupleArg1, TupleArg2>::DoCat2(forward<TupleArg1>(t1), forward<TupleArg2>(t2));
	}
};

}  // namespace Internal

template <typename... Ts>
class tuple
{
public:
	EA_CONSTEXPR tuple() = default;

	explicit EA_CONSTEXPR tuple(const Ts&... t)
		: mImpl(make_index_sequence<sizeof...(Ts)>{}, Internal::MakeTupleTypes_t<tuple>{}, t...)
	{
	}

	template <typename U, typename... Us,
			  typename = typename enable_if<
				  sizeof...(Us) + 1 == sizeof...(Ts) && Internal::TupleConvertible<tuple<U, Us...>, tuple>::value,
				  bool>::type>
	explicit EA_CONSTEXPR tuple(U&& u, Us&&... us)
		: mImpl(make_index_sequence<sizeof...(Us) + 1>{}, Internal::MakeTupleTypes_t<tuple>{}, forward<U>(u),
				forward<Us>(us)...)
	{
	}

	template <typename OtherTuple,
			  typename enable_if<Internal::TupleConvertible<OtherTuple, tuple>::value, bool>::type = false>
	tuple(OtherTuple&& t)
		: mImpl(forward<OtherTuple>(t))
	{
	}

	template <typename OtherTuple,
			  typename enable_if<Internal::TupleAssignable<tuple, OtherTuple>::value, bool>::type = false>
	tuple& operator=(OtherTuple&& t)
	{
		mImpl.operator=(forward<OtherTuple>(t));
		return *this;
	}

	void swap(tuple& t) { mImpl.swap(t.mImpl); }

private:
	typedef Internal::TupleImpl<make_index_sequence<sizeof...(Ts)>, Ts...> Impl;
	Impl mImpl;

	template <size_t I, typename... Ts_>
	friend tuple_element_t<I, tuple<Ts_...>>& get(tuple<Ts_...>& t);

	template <size_t I, typename... Ts_>
	friend const_tuple_element_t<I, tuple<Ts_...>>& get(const tuple<Ts_...>& t);

	template <size_t I, typename... Ts_>
	friend tuple_element_t<I, tuple<Ts_...>>&& get(tuple<Ts_...>&& t);

	template <typename T, typename... ts_>
	friend T& get(tuple<ts_...>& t);

	template <typename T, typename... ts_>
	friend const T& get(const tuple<ts_...>& t);

	template <typename T, typename... ts_>
	friend T&& get(tuple<ts_...>&& t);
};

template <>
class tuple<>
{
public:
	void swap(tuple&) {}
};

template <size_t I, typename... Ts>
inline tuple_element_t<I, tuple<Ts...>>& get(tuple<Ts...>& t)
{
	return get<I>(t.mImpl);
}

template <size_t I, typename... Ts>
inline const_tuple_element_t<I, tuple<Ts...>>& get(const tuple<Ts...>& t)
{
	return get<I>(t.mImpl);
}

template <size_t I, typename... Ts>
inline tuple_element_t<I, tuple<Ts...>>&& get(tuple<Ts...>&& t)
{
	return get<I>(move(t.mImpl));
}

template <typename T, typename... Ts>
inline T& get(tuple<Ts...>& t)
{
	return get<T>(t.mImpl);
}

template <typename T, typename... Ts>
inline const T& get(const tuple<Ts...>& t)
{
	return get<T>(t.mImpl);
}

template <typename T, typename... Ts>
inline T&& get(tuple<Ts...>&& t)
{
	return get<T>(move(t.mImpl));
}

template <typename... Ts>
inline void swap(tuple<Ts...>& a, tuple<Ts...>& b)
{
	a.swap(b);
}

template <typename... T1s, typename... T2s>
inline bool operator==(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return Internal::TupleEqual<sizeof...(T1s)>()(t1, t2);
}

template <typename... T1s, typename... T2s>
inline bool operator!=(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return !(t1 == t2);
}

template <typename... T1s, typename... T2s>
inline bool operator<(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return Internal::TupleLess<sizeof...(T1s)>()(t1, t2);
}

template <typename... T1s, typename... T2s>
inline bool operator>(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return t2 < t1;
}

template <typename... T1s, typename... T2s>
inline bool operator<=(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return !(t2 < t1);
}

template <typename... T1s, typename... T2s>
inline bool operator>=(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return !(t1 < t2);
}

// Tuple helper functions

template <typename... Ts>
inline EA_CONSTEXPR tuple<Internal::MakeTupleReturn_t<Ts>...> make_tuple(Ts&&... values)
{
	return tuple<Internal::MakeTupleReturn_t<Ts>...>(forward<Ts>(values)...);
}

template <typename... Ts>
inline EA_CONSTEXPR tuple<Ts&&...> forward_as_tuple(Ts&&... ts) EA_NOEXCEPT
{
	return tuple<Ts&&...>(forward<Ts&&>(ts)...);
}

// Specialize ignore_t is_assignable type trait due to yet another VS2013 type traits bug
template <typename U>
struct is_assignable<const Internal::ignore_t&, U> : public true_type
{
};

static const Internal::ignore_t ignore;

template <typename... Ts>
inline EA_CONSTEXPR tuple<Ts&...> tie(Ts&... ts) EA_NOEXCEPT
{
	return tuple<Ts&...>(ts...);
}

template <typename... Tuples>
inline typename Internal::TupleCat<Tuples...>::ResultType tuple_cat(Tuples&&... ts)
{
	return Internal::TupleCat<Tuples...>::DoCat(forward<Tuples>(ts)...);
}


// apply
//
// Invoke a callable object using a tuple to supply the arguments. 
//
// http://en.cppreference.com/w/cpp/utility/apply
//
namespace detail
{
	template <class F, class Tuple, size_t... I>
	EA_CONSTEXPR decltype(auto) apply_impl(F&& f, Tuple&& t, index_sequence<I...>)
	{
		return invoke(forward<F>(f), get<I>(forward<Tuple>(t))...);
	}
} // namespace detail

template <class F, class Tuple>
EA_CONSTEXPR decltype(auto) apply(F&& f, Tuple&& t)
{
	return detail::apply_impl(forward<F>(f), forward<Tuple>(t),
		                      make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{});
}


}  // namespace eastl

#endif  // EASTL_TUPLE_ENABLED
EA_RESTORE_VC_WARNING()
EA_RESTORE_VC_WARNING()
EA_RESTORE_VC_WARNING()
#endif  // EASTL_TUPLE_H
