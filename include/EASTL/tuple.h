///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_TUPLE_H
#define EASTL_TUPLE_H

#include <EASTL/internal/config.h>
#include <EASTL/compare.h>
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
template <typename T> struct tuple_size {};
template <typename T> struct tuple_size<const T>          : public tuple_size<T> {};
template <typename T> struct tuple_size<volatile T>       : public tuple_size<T> {};
template <typename T> struct tuple_size<const volatile T> : public tuple_size<T> {};

template <typename... Ts> struct tuple_size<TupleTypes<Ts...>> : public integral_constant<size_t, sizeof...(Ts)> {};
template <typename... Ts> struct tuple_size<tuple<Ts...>>      : public integral_constant<size_t, sizeof...(Ts)> {};

namespace Internal
{
	template <typename TupleIndices, typename... Ts>
	struct TupleImpl;
} // namespace Internal

template <typename Indices, typename... Ts>
struct tuple_size<Internal::TupleImpl<Indices, Ts...>> : public integral_constant<size_t, sizeof...(Ts)>
{
};

// tuple_element helper to be able to isolate a type given an index
template <size_t I, typename T>
struct tuple_element
{
};

template <size_t I>
struct tuple_element<I, TupleTypes<>>
{
public:
	static_assert(I != I, "tuple_element index out of range");
};

template <typename H, typename... Ts>
struct tuple_element<0, TupleTypes<H, Ts...>>
{
public:
	typedef H type;
};

template <size_t I, typename H, typename... Ts>
struct tuple_element<I, TupleTypes<H, Ts...>>
{
public:
	typedef tuple_element_t<I - 1, TupleTypes<Ts...>> type;
};

// specialization for tuple
template <size_t I, typename... Ts>
struct tuple_element<I, tuple<Ts...>>
{
public:
	typedef tuple_element_t<I, TupleTypes<Ts...>> type;
};

template <size_t I, typename... Ts>
struct tuple_element<I, const tuple<Ts...>>
{
public:
	typedef typename add_const<tuple_element_t<I, TupleTypes<Ts...>>>::type type;
};

template <size_t I, typename... Ts>
struct tuple_element<I, volatile tuple<Ts...>>
{
public:
	typedef typename add_volatile<tuple_element_t<I, TupleTypes<Ts...>>>::type type;
};

template <size_t I, typename... Ts>
struct tuple_element<I, const volatile tuple<Ts...>>
{
public:
	typedef typename add_cv<tuple_element_t<I, TupleTypes<Ts...>>>::type type;
};

// specialization for TupleImpl
template <size_t I, typename Indices, typename... Ts>
struct tuple_element<I, Internal::TupleImpl<Indices, Ts...>> : public tuple_element<I, tuple<Ts...>>
{
};

template <size_t I, typename Indices, typename... Ts>
struct tuple_element<I, const Internal::TupleImpl<Indices, Ts...>> : public tuple_element<I, const tuple<Ts...>>
{
};

template <size_t I, typename Indices, typename... Ts>
struct tuple_element<I, volatile Internal::TupleImpl<Indices, Ts...>> : public tuple_element<I, volatile tuple<Ts...>>
{
};

template <size_t I, typename Indices, typename... Ts>
struct tuple_element<I, const volatile Internal::TupleImpl<Indices, Ts...>> : public tuple_element<
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
	// swallow
	//
	// Provides a vessel to expand variadic packs.
	//
	template <typename... Ts>
	void swallow(Ts&&...) {}


	// TupleLeaf
	//
	template <size_t I, typename ValueType, bool IsEmpty = is_empty_v<ValueType>>
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
		TupleLeaf& operator=(const TupleLeaf&) = delete;

		// We shouldn't need this explicit constructor as it should be handled by the template below but OSX clang
		// is_constructible type trait incorrectly gives false for is_constructible<T&&, T&&>::value
		explicit TupleLeaf(ValueType&& v) : mValue(eastl::forward<ValueType>(v)) {}

		template <typename T, typename = typename enable_if<is_constructible<ValueType, T&&>::value>::type>
		explicit TupleLeaf(T&& t)
			: mValue(eastl::forward<T>(t))
		{
		}

		template <typename T>
		explicit TupleLeaf(const TupleLeaf& t)
			: mValue(t.getInternal())
		{
		}

		template <typename T>
		TupleLeaf& operator=(T&& t)
		{
			mValue = eastl::forward<T>(t);
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

	// TupleLeaf: partial specialization for when we can use the Empty Base Class Optimization
	template <size_t I, typename ValueType>
	class TupleLeaf<I, ValueType, true> : private ValueType
	{
	public:
		TupleLeaf() = default;
		TupleLeaf& operator=(const TupleLeaf&) = delete;

		// We shouldn't need this explicit constructor as it should be handled by the template below but OSX clang
		// is_constructible type trait incorrectly gives false for is_constructible<T&&, T&&>::value
		explicit TupleLeaf(ValueType&& v) : ValueType(eastl::forward<ValueType>(v)) {}

		template <typename T, typename = typename enable_if<is_constructible<ValueType, T&&>::value>::type>
		explicit TupleLeaf(T&& t)
			: ValueType(eastl::forward<T>(t))
		{
		}

		template <typename T>
		explicit TupleLeaf(const TupleLeaf& t)
			: ValueType(t.getInternal())
		{
		}

		template <typename T>
		TupleLeaf& operator=(T&& t)
		{
			ValueType::operator=(eastl::forward<T>(t));
			return *this;
		}

		int swap(TupleLeaf& t)
		{
			eastl::Internal::swap(*this, t);
			return 0;
		}

		ValueType& getInternal() { return static_cast<ValueType&>(*this); }
		const ValueType& getInternal() const { return static_cast<const ValueType&>(*this); }
	};



	// MakeTupleTypes
	//
	//
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
	//
	//
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
			: TupleLeaf<Indices, Ts>(eastl::forward<ValueTypes>(values))...
		{
		}

		template <typename OtherTuple>
		TupleImpl(OtherTuple&& t)
			: TupleLeaf<Indices, Ts>(eastl::forward<tuple_element_t<Indices, MakeTupleTypes_t<OtherTuple>>>(get<Indices>(t)))...
		{
		}

		template <typename OtherTuple>
		TupleImpl& operator=(OtherTuple&& t)
		{
			swallow(TupleLeaf<Indices, Ts>::operator=(
						eastl::forward<tuple_element_t<Indices, MakeTupleTypes_t<OtherTuple>>>(get<Indices>(t)))...);
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
	//
	// type-trait that determines if a type is an eastl::tuple or an eastl::pair.
	//
	template <typename T> struct TupleLike                   : public false_type {};
	template <typename T> struct TupleLike<const T>          : public TupleLike<T> {};
	template <typename T> struct TupleLike<volatile T>       : public TupleLike<T> {};
	template <typename T> struct TupleLike<const volatile T> : public TupleLike<T> {};

	template <typename... Ts>
	struct TupleLike<tuple<Ts...>> : public true_type {};

	template <typename First, typename Second>
	struct TupleLike<eastl::pair<First, Second>> : public true_type {};


	// TupleConvertible
	//
	//
	//
	template <bool IsSameSize, typename From, typename To>
	struct TupleConvertibleImpl : public false_type
	{
	};

	template <typename... FromTypes, typename... ToTypes>
	struct TupleConvertibleImpl<true, TupleTypes<FromTypes...>,	TupleTypes<ToTypes...>>
		: public integral_constant<bool, conjunction<is_convertible<FromTypes, ToTypes>...>::value>
	{
	};

	template <typename From, typename To,
			  bool = TupleLike<typename remove_reference<From>::type>::value,
			  bool = TupleLike<typename remove_reference<To>::type>::value>
	struct TupleConvertible : public false_type
	{
	};

	template <typename From, typename To>
	struct TupleConvertible<From, To, true, true>
		: public TupleConvertibleImpl<tuple_size<typename remove_reference<From>::type>::value ==
				tuple_size<typename remove_reference<To>::type>::value,
				MakeTupleTypes_t<From>, MakeTupleTypes_t<To>>
	{
	};


	// TupleAssignable
	//
	//
	//
	template <bool IsSameSize, typename Target, typename From>
	struct TupleAssignableImpl : public false_type
	{
	};

	template <typename... TargetTypes, typename... FromTypes>
	struct TupleAssignableImpl<true, TupleTypes<TargetTypes...>, TupleTypes<FromTypes...>>
		: public bool_constant<conjunction<is_assignable<TargetTypes, FromTypes>...>::value>
	{
	};

	template <typename Target, typename From,
			  bool = TupleLike<typename remove_reference<Target>::type>::value,
			  bool = TupleLike<typename remove_reference<From>::type>::value>
	struct TupleAssignable : public false_type
	{
	};

	template <typename Target, typename From>
	struct TupleAssignable<Target, From, true, true>
		: public TupleAssignableImpl<
			tuple_size<typename remove_reference<Target>::type>::value ==
			tuple_size<typename remove_reference<From>::type>::value,
			MakeTupleTypes_t<Target>, MakeTupleTypes_t<From>>
	{
	};


	// TupleImplicitlyConvertible and TupleExplicitlyConvertible 
	//
	// helpers for constraining conditionally-explicit ctors
	//
	template <bool IsSameSize, typename TargetType, typename... FromTypes>
	struct TupleImplicitlyConvertibleImpl : public false_type
	{
	};


	template <typename... TargetTypes, typename... FromTypes>
	struct TupleImplicitlyConvertibleImpl<true, TupleTypes<TargetTypes...>, FromTypes...>
		: public conjunction<
		is_constructible<TargetTypes, FromTypes>...,
		is_convertible<FromTypes, TargetTypes>...>
	{
	};

	template <typename TargetTupleType, typename... FromTypes>
	struct TupleImplicitlyConvertible
		: public TupleImplicitlyConvertibleImpl<
		tuple_size<TargetTupleType>::value == sizeof...(FromTypes),
		MakeTupleTypes_t<TargetTupleType>, FromTypes...>::type
	{
	};

	template<typename TargetTupleType, typename... FromTypes>
	using TupleImplicitlyConvertible_t = enable_if_t<TupleImplicitlyConvertible<TargetTupleType, FromTypes...>::value, bool>;

	template <bool IsSameSize, typename TargetType, typename... FromTypes>
	struct TupleExplicitlyConvertibleImpl : public false_type
	{
	};

	template <typename... TargetTypes, typename... FromTypes>
	struct TupleExplicitlyConvertibleImpl<true, TupleTypes<TargetTypes...>, FromTypes...>
		: public conjunction<
			is_constructible<TargetTypes, FromTypes>...,
			negation<conjunction<is_convertible<FromTypes, TargetTypes>...>>>
	{
	};

	template <typename TargetTupleType, typename... FromTypes>
	struct TupleExplicitlyConvertible
		: public TupleExplicitlyConvertibleImpl<
		tuple_size<TargetTupleType>::value == sizeof...(FromTypes),
		MakeTupleTypes_t<TargetTupleType>, FromTypes...>::type
	{
	};

	template<typename TargetTupleType, typename... FromTypes>
	using TupleExplicitlyConvertible_t = enable_if_t<TupleExplicitlyConvertible<TargetTupleType, FromTypes...>::value, bool>;


	// TupleEqual
	//
	//
	//
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
	//
	//
	//
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


	// MakeTupleReturnImpl
	//
	//
	//
	template <typename T> struct MakeTupleReturnImpl                       { typedef T type; };
	template <typename T> struct MakeTupleReturnImpl<reference_wrapper<T>> { typedef T& type; };

	template <typename T>
	using MakeTupleReturn_t = typename MakeTupleReturnImpl<decay_t<T>>::type;

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	template <typename... T1s, typename... T2s, size_t... Is>
	constexpr auto TupleThreeWay(const tuple<T1s...>& t1, const tuple<T2s...>& t2, index_sequence<Is...> is)
	{
		std::common_comparison_category_t<synth_three_way_result<T1s, T2s>...> result = std::strong_ordering::equal;
		((result = synth_three_way{}(get<Is>(t1), get<Is>(t2)), result != 0) || ...);
		return result;
	}
#endif
}  // namespace Internal



// tuple
//
// eastl::tuple is a fixed-size container of heterogeneous values. It is a
// generalization of eastl::pair which hold only two heterogeneous values.
//
// https://en.cppreference.com/w/cpp/utility/tuple
//
template <typename... Ts>
class tuple;

template <typename T, typename... Ts>
class tuple<T, Ts...>
{
public:
	EA_CONSTEXPR tuple() = default;
	
	template <typename T2 = T, 
		Internal::TupleImplicitlyConvertible_t<tuple, const T2&, const Ts&...> = 0>
	EA_CONSTEXPR tuple(const T& t, const Ts&... ts)
		: mImpl(make_index_sequence<sizeof...(Ts) + 1>{}, Internal::MakeTupleTypes_t<tuple>{}, t, ts...)
	{
	}

	template <typename T2 = T, 
		Internal::TupleExplicitlyConvertible_t<tuple, const T2&, const Ts&...> = 0>
	explicit EA_CONSTEXPR tuple(const T& t, const Ts&... ts)
		: mImpl(make_index_sequence<sizeof...(Ts) + 1>{}, Internal::MakeTupleTypes_t<tuple>{}, t, ts...)
	{
	}

	template <typename U, typename... Us,
		Internal::TupleImplicitlyConvertible_t<tuple, U, Us...> = 0>
		EA_CONSTEXPR tuple(U&& u, Us&&... us)
			: mImpl(make_index_sequence<sizeof...(Us) + 1>{}, Internal::MakeTupleTypes_t<tuple>{}, eastl::forward<U>(u),
					eastl::forward<Us>(us)...)
	{
	}

	template <typename U, typename... Us,
		Internal::TupleExplicitlyConvertible_t<tuple, U, Us...> = 0>
		explicit EA_CONSTEXPR tuple(U&& u, Us&&... us)
			: mImpl(make_index_sequence<sizeof...(Us) + 1>{}, Internal::MakeTupleTypes_t<tuple>{}, eastl::forward<U>(u),
					eastl::forward<Us>(us)...)
	{
	}

	template <typename OtherTuple,
			  typename enable_if<Internal::TupleConvertible<OtherTuple, tuple>::value, bool>::type = false>
	tuple(OtherTuple&& t)
		: mImpl(eastl::forward<OtherTuple>(t))
	{
	}

	template <typename OtherTuple,
			  typename enable_if<Internal::TupleAssignable<tuple, OtherTuple>::value, bool>::type = false>
	tuple& operator=(OtherTuple&& t)
	{
		mImpl.operator=(eastl::forward<OtherTuple>(t));
		return *this;
	}

	void swap(tuple& t) { mImpl.swap(t.mImpl); }

private:
	typedef Internal::TupleImpl<make_index_sequence<sizeof...(Ts) + 1>, T, Ts...> Impl;
	Impl mImpl;

	template <size_t I, typename... Ts_>
	friend tuple_element_t<I, tuple<Ts_...>>& get(tuple<Ts_...>& t);

	template <size_t I, typename... Ts_>
	friend const_tuple_element_t<I, tuple<Ts_...>>& get(const tuple<Ts_...>& t);

	template <size_t I, typename... Ts_>
	friend tuple_element_t<I, tuple<Ts_...>>&& get(tuple<Ts_...>&& t);

	template <typename T_, typename... ts_>
	friend T_& get(tuple<ts_...>& t);

	template <typename T_, typename... ts_>
	friend const T_& get(const tuple<ts_...>& t);

	template <typename T_, typename... ts_>
	friend T_&& get(tuple<ts_...>&& t);
};

// template specialization for an empty tuple
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
	return get<I>(eastl::move(t.mImpl));
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
	return get<T>(eastl::move(t.mImpl));
}

template <typename... Ts>
inline void swap(tuple<Ts...>& a, tuple<Ts...>& b)
{
	a.swap(b);
}


// tuple operators
//
//
template <typename... T1s, typename... T2s>
inline bool operator==(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return Internal::TupleEqual<sizeof...(T1s)>()(t1, t2);
}

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
template <typename... T1s, typename... T2s>
inline constexpr std::common_comparison_category_t<synth_three_way_result<T1s, T2s>...> operator<=>(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return Internal::TupleThreeWay(t1, t2, make_index_sequence<sizeof...(T1s)>{});
}
#else
template <typename... T1s, typename... T2s>
inline bool operator<(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return Internal::TupleLess<sizeof...(T1s)>()(t1, t2);
}

template <typename... T1s, typename... T2s> inline bool operator!=(const tuple<T1s...>& t1, const tuple<T2s...>& t2) { return !(t1 == t2); }
template <typename... T1s, typename... T2s> inline bool operator> (const tuple<T1s...>& t1, const tuple<T2s...>& t2) { return t2 < t1; }
template <typename... T1s, typename... T2s> inline bool operator<=(const tuple<T1s...>& t1, const tuple<T2s...>& t2) { return !(t2 < t1); }
template <typename... T1s, typename... T2s> inline bool operator>=(const tuple<T1s...>& t1, const tuple<T2s...>& t2) { return !(t1 < t2); }
#endif

namespace Internal
{
	// tuple_cat helpers
	//
	//
	//

	// TupleCat2Impl
	template <typename Tuple1, typename Is1, typename Tuple2, typename Is2>
	struct TupleCat2Impl;

	template <typename... T1s, size_t... I1s, typename... T2s, size_t... I2s>
	struct TupleCat2Impl<tuple<T1s...>, index_sequence<I1s...>, tuple<T2s...>, index_sequence<I2s...>>
	{
		using ResultType = tuple<T1s..., T2s...>;

		template <typename Tuple1, typename Tuple2>
		static inline ResultType DoCat2(Tuple1&& t1, Tuple2&& t2)
		{
			return ResultType(get<I1s>(eastl::forward<Tuple1>(t1))..., get<I2s>(eastl::forward<Tuple2>(t2))...);
		}
	};

	// TupleCat2
	template <typename Tuple1, typename Tuple2>
	struct TupleCat2;

	template <typename... T1s, typename... T2s>
	struct TupleCat2<tuple<T1s...>, tuple<T2s...>>
	{
		using Is1        = make_index_sequence<sizeof...(T1s)>;
		using Is2        = make_index_sequence<sizeof...(T2s)>;
		using TCI        = TupleCat2Impl<tuple<T1s...>, Is1, tuple<T2s...>, Is2>;
		using ResultType = typename TCI::ResultType;

		template <typename Tuple1, typename Tuple2>
		static inline ResultType DoCat2(Tuple1&& t1, Tuple2&& t2)
		{
			return TCI::DoCat2(eastl::forward<Tuple1>(t1), eastl::forward<Tuple2>(t2));
		}
	};

	// TupleCat
	template <typename... Tuples>
	struct TupleCat;

	template <typename Tuple1, typename Tuple2, typename... TuplesRest>
	struct TupleCat<Tuple1, Tuple2, TuplesRest...>
	{
		using TC2             = TupleCat2<Tuple1, Tuple2>;
		using FirstResultType = typename TupleCat2<Tuple1, Tuple2>::ResultType;
		using ResultType      = typename TupleCat<FirstResultType, TuplesRest...>::ResultType;

		template <typename TupleArg1, typename TupleArg2, typename... TupleArgsRest>
		static inline ResultType DoCat(TupleArg1&& t1, TupleArg2&& t2, TupleArgsRest&&... ts)
		{
			return TupleCat<FirstResultType, TuplesRest...>::DoCat(
				TC2::DoCat2(eastl::forward<TupleArg1>(t1), eastl::forward<TupleArg2>(t2)),
				eastl::forward<TupleArgsRest>(ts)...);
		}
	};

	template <typename Tuple1, typename Tuple2>
	struct TupleCat<Tuple1, Tuple2>
	{
		using TC2 = TupleCat2<Tuple1, Tuple2>;
		using ResultType = typename TC2::ResultType;

		template <typename TupleArg1, typename TupleArg2>
		static inline ResultType DoCat(TupleArg1&& t1, TupleArg2&& t2)
		{
			return TC2::DoCat2(eastl::forward<TupleArg1>(t1), eastl::forward<TupleArg2>(t2));
		}
	};

	template <typename... Ts>
	struct TupleCat<tuple<Ts...>>
	{
		using ResultType = tuple<Ts...>;
		
		template <typename TupleArg>
		static inline tuple<Ts...> DoCat(TupleArg&& t)
		{
			return eastl::forward<TupleArg>(t);
		}
	};

	template <>
	struct TupleCat<>
	{
		using ResultType = tuple<>;
		
		static inline tuple<> DoCat()
		{
			return {};
		}
	};
}
	
// tuple_cat 
//
//
template <typename... Tuples>
inline typename Internal::TupleCat<eastl::remove_cvref_t<Tuples>...>::ResultType tuple_cat(Tuples&&... ts)
{
	return Internal::TupleCat<eastl::remove_cvref_t<Tuples>...>::DoCat(eastl::forward<Tuples>(ts)...);
}


// make_tuple 
//
//
template <typename... Ts>
inline EA_CONSTEXPR tuple<Internal::MakeTupleReturn_t<Ts>...> make_tuple(Ts&&... values)
{
	return tuple<Internal::MakeTupleReturn_t<Ts>...>(eastl::forward<Ts>(values)...);
}


// forward_as_tuple 
//
//
template <typename... Ts>
inline EA_CONSTEXPR tuple<Ts&&...> forward_as_tuple(Ts&&... ts) EA_NOEXCEPT
{
	return tuple<Ts&&...>(eastl::forward<Ts&&>(ts)...);
}


// ignore 
//
// An object of unspecified type such that any value can be assigned to it with no effect.
//
// https://en.cppreference.com/w/cpp/utility/tuple/ignore
//
namespace Internal 
{
	struct ignore_t
	{
		ignore_t() = default;

		template <typename T>
		const ignore_t& operator=(const T&) const
		{
			return *this;
		}
	};
}// namespace Internal

static const Internal::ignore_t ignore;


// tie 
//
// Creates a tuple of lvalue references to its arguments or instances of eastl::ignore.
//
// https://en.cppreference.com/w/cpp/utility/tuple/tie
//
template <typename... Ts>
inline EA_CONSTEXPR tuple<Ts&...> tie(Ts&... ts) EA_NOEXCEPT
{
	return tuple<Ts&...>(ts...);
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
		return eastl::invoke(eastl::forward<F>(f), get<I>(eastl::forward<Tuple>(t))...);
	}
} // namespace detail

template <class F, class Tuple>
EA_CONSTEXPR decltype(auto) apply(F&& f, Tuple&& t)
{
	return detail::apply_impl(eastl::forward<F>(f), eastl::forward<Tuple>(t),
		                      make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{});
}

}  // namespace eastl


///////////////////////////////////////////////////////////////
// C++17 structured bindings support for eastl::tuple
//
#ifndef EA_COMPILER_NO_STRUCTURED_BINDING
// we can't forward declare tuple_size and tuple_element because some std implementations
// don't declare it in the std namespace, but instead alias it.
#include <tuple>

	namespace std
	{
		template <class... Ts>
		struct tuple_size<::eastl::tuple<Ts...>> : public ::eastl::integral_constant<size_t, sizeof...(Ts)>
		{
		};

		template <size_t I, class... Ts>
		struct tuple_element<I, ::eastl::tuple<Ts...>> : public ::eastl::tuple_element<I, ::eastl::tuple<Ts...>>
		{
		};
	}
#endif


#endif  // EASTL_TUPLE_ENABLED
EA_RESTORE_VC_WARNING()
EA_RESTORE_VC_WARNING()
EA_RESTORE_VC_WARNING()
#endif  // EASTL_TUPLE_H
