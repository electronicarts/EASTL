///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_TUPLEVECTOR_H
#define EASTL_TUPLEVECTOR_H

#include <EASTL/internal/config.h>
#include <EASTL/utility.h>

namespace eastl
{

// forward declarations
template <typename... Ts>
class tuple_vector;

template <size_t I, typename TupleVector>
class tuplevec_element;

template <size_t I, typename TupleVector>
using tuplevec_element_t = typename tuplevec_element<I, TupleVector>::type;

namespace Internal
{
	template <typename Indices, typename... Ts>
	struct TupleVecImpl;
}

// tuplevec_element helper to be able to isolate a type given an index
template <typename... Ts>
struct TupleVecTypes
{
};

template <size_t I, typename T>
class tuplevec_element
{
};

// attempt to isolate type given an index
template <size_t I>
class tuplevec_element<I, tuple_vector<>>
{
public:
	static_assert(I != I, "tuplevec_element index out of range");
};

template <typename H, typename... Ts>
class tuplevec_element<0, tuple_vector<H, Ts...>>
{
public:
	typedef H type;
};

template <size_t I, typename H, typename... Ts>
class tuplevec_element<I, tuple_vector<H, Ts...>>
{
public:
	typedef tuplevec_element_t<I - 1, tuple_vector<Ts...>> type;
};

template <size_t I, typename Indices, typename... Ts>
class tuplevec_element<I, Internal::TupleVecImpl<Indices, Ts...>> : public tuplevec_element<I, tuple_vector<Ts...>>
{
};

// attempt to isolate index given a type
template <typename T, typename TupleVector>
class tuplevec_index
{
};

template <typename T>
class tuplevec_index<T, tuple_vector<>>
{
public:
	typedef void DuplicateTypeCheck;
};

template <typename T, typename... TsRest>
class tuplevec_index<T, tuple_vector<T, TsRest...>>
{
public:
	typedef int DuplicateTypeCheck;
	static_assert(is_void<typename tuplevec_index<T, tuple_vector<TsRest...>>::DuplicateTypeCheck>::value, "duplicate type T in tuple_vector::get<T>(); unique types must be provided in declaration, or only use get<size_t>()");

	static const size_t index = 0;
};

template <typename T, typename Ts, typename... TsRest>
class tuplevec_index<T, tuple_vector<Ts, TsRest...>>
{
public:
	static const size_t index = tuplevec_index<T, tuple_vector<TsRest...>>::index + 1;
};

template <typename T, typename Indices, typename... Ts>
class tuplevec_index<T, Internal::TupleVecImpl<Indices, Ts...>> : public tuplevec_index<T, tuple_vector<Ts...>>
{
};

namespace Internal
{

// TupleVecLeaf
template <size_t I, typename ValueType>
class TupleVecLeaf
{
public:
	TupleVecLeaf() : mVec() {}

	// swallowed functions requires non-void return types
	int push_back()
	{
		mVec.push_back();
		return 0;
	}

	int push_back(const ValueType& value)
	{
		mVec.push_back(value);
		return 0;
	}

	int push_back_uninitialized()
	{
		mVec.push_back_uninitialized();
		return 0;
	}

	size_t size() const
	{
		return mVec.size();
	}


	vector<ValueType>& getInternal() { return mVec; }
private:
	vector<ValueType> mVec;
};

// TupleVecImpl

// swallow allows for parameter pack expansion of arguments as means of expanding operations performed
template <typename... Ts>
void swallow(Ts&&...)
{
}


template <size_t... Indices, typename... Ts>
class TupleVecImpl<integer_sequence<size_t, Indices...>, Ts...> : public TupleVecLeaf<Indices, Ts>...
{
public:
	typedef tuplevec_element_t<0, tuple_vector<Ts...>> BaseType;
	typedef TupleVecLeaf<0, BaseType> BaseLeaf;
	EA_CONSTEXPR TupleVecImpl() = default;

	size_t push_back()
	{
		swallow(TupleVecLeaf<Indices, Ts>::push_back()...);
		return BaseLeaf::size() - 1;
	}
	void push_back(const Ts&... args)
	{
		swallow(TupleVecLeaf<Indices, Ts>::push_back(args)...);
	}
	void push_back_uninitialized()
	{
		swallow(TupleVecLeaf<Indices, Ts>::push_back_uninitialized()...);
	}

	size_t size() const
	{
		return BaseLeaf::size();
	}

};

template <size_t I, typename Indices, typename... Ts>
vector<tuplevec_element_t<I, TupleVecImpl<Indices, Ts...>>>& get(TupleVecImpl<Indices, Ts...>& t)
{
	typedef tuplevec_element_t<I, TupleVecImpl<Indices, Ts...>> Type;
	return static_cast<Internal::TupleVecLeaf<I, Type>&>(t).getInternal();
}

template <typename T, typename Indices, typename... Ts>
vector<T>& get(TupleVecImpl<Indices, Ts...>& t)
{
	typedef tuplevec_index<T, TupleVecImpl<Indices, Ts...>> Index;
	return static_cast<Internal::TupleVecLeaf<Index::index, T>&>(t).getInternal();
}


}  // namespace Internal


// External interface of tuple_vector
template <typename... Ts>
class tuple_vector
{
private:
	typedef Internal::TupleVecImpl<make_index_sequence<sizeof...(Ts)>, Ts...> Impl;

public:
	
	// tuple_vector_element interface:
	// - created by fetching push_back, dereferencing iterator
	// - get<>() function which returns value&
	// internals:
	// - reference to tuple_vector
	// - index [or store iterator to first vec and derive after the fact? eh, probably just index...]
	template <typename... Ts>
	struct element // dcrooks-todo maybe this'll just be iterator?
	{
	public:
		element(tuple_vector<Ts...>& tupleVector, size_t index = 0)
		: mIndex(index)
		, mTupleVector(tupleVector)
		{	}

		template<size_t I>
		tuplevec_element_t<I, tuple_vector<Ts...>>& get() { return mTupleVector.get<I>()[mIndex];	}

		template<typename T> 
		T& get() { return mTupleVector.get<T>()[mIndex]; }

		element& operator++()
		{
			++mIndex;
			return *this;
		}

		bool operator==(const element& other) const	{ return mIndex == other.mIndex; }
		bool operator!=(const element& other) const { return mIndex != other.mIndex; }
		element& operator*() { return *this; }

	private:
		size_t mIndex;
		tuple_vector<Ts...> &mTupleVector;
	};

	typedef element<Ts...> element_type;
public:

	EA_CONSTEXPR tuple_vector() = default;
	
	element_type push_back();
	void push_back(const Ts&... args);
	void push_back_uninitialized();

	size_t size();

	element_type begin();
	element_type end();
	
	template<size_t I>
	vector<tuplevec_element_t<I, tuple_vector<Ts...>>>& get();

	template<typename T>
	vector<T>& get();

private:
	Impl mImpl;

};

template <typename... Ts>
typename tuple_vector<Ts...>::element_type tuple_vector<Ts...>::push_back()
{
	size_t newIndex = mImpl.push_back();
	return element<Ts...>((*this), newIndex);
}

template <typename... Ts>
void tuple_vector<Ts...>::push_back(const Ts&... args)
{
	mImpl.push_back(args...);
}

template <typename... Ts>
void tuple_vector<Ts...>::push_back_uninitialized()
{
	mImpl.push_back_uninitialized();
}

template <typename... Ts>
size_t tuple_vector<Ts...>::size()
{
	return mImpl.size();
}

template <typename... Ts>
typename tuple_vector<Ts...>::element_type tuple_vector<Ts...>::begin()
{
	return element<Ts...>(*this, 0);
}

template <typename... Ts>
typename tuple_vector<Ts...>::element_type tuple_vector<Ts...>::end()
{
	return element<Ts...>(*this, size() - 1);
}

template <typename... Ts>
template<size_t I>
vector<eastl::tuplevec_element_t<I, tuple_vector<Ts...>>>&
tuple_vector<Ts...>::get()
{
	return Internal::get<I>(mImpl);
}


template <typename... Ts>
template<typename T>
eastl::vector<T>&
eastl::tuple_vector<Ts...>::get()
{
	return Internal::get<T>(mImpl);
}



// Vector_Decl macros
#pragma region 
#define TUPLE_VECTOR_DECL_START(className, ... ) \
	class className : public tuple_vector<##__VA_ARGS__> \
	{\
		public: \

#define TUPLE_VECTOR_DECL_TYPENAME(type, name, iter) vector<type>& name() { return get<iter>(); } \

#define TUPLE_VECTOR_DECL_END() };

#define TUPLE_VECTOR_DECL_2(className, type0, name0, type1, name1) \
	TUPLE_VECTOR_DECL_START(className, type0, type1) \
	TUPLE_VECTOR_DECL_TYPENAME(type0, name0, 0) \
	TUPLE_VECTOR_DECL_TYPENAME(type1, name1, 1) \
	TUPLE_VECTOR_DECL_END()

#define TUPLE_VECTOR_DECL_3(className, type0, name0, type1, name1, type2, name2) \
	TUPLE_VECTOR_DECL_START(className, type0, type1, type2) \
	TUPLE_VECTOR_DECL_TYPENAME(type0, name0, 0) \
	TUPLE_VECTOR_DECL_TYPENAME(type1, name1, 1) \
	TUPLE_VECTOR_DECL_TYPENAME(type2, name2, 2) \
	TUPLE_VECTOR_DECL_END()

#define TUPLE_VECTOR_DECL_4(className, type0, name0, type1, name1, type2, name2, type3, name3) \
	TUPLE_VECTOR_DECL_START(className, type0, type1, type2, type3) \
	TUPLE_VECTOR_DECL_TYPENAME(type0, name0, 0) \
	TUPLE_VECTOR_DECL_TYPENAME(type1, name1, 1) \
	TUPLE_VECTOR_DECL_TYPENAME(type2, name2, 2) \
	TUPLE_VECTOR_DECL_TYPENAME(type3, name3, 3) \
	TUPLE_VECTOR_DECL_END()
#pragma endregion

}  // namespace eastl

#endif  // EASTL_TUPLEVECTOR_H
