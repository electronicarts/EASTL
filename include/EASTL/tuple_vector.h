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
struct tuplevec_element;

template <size_t I, typename TupleVector>
using tuplevec_element_t = typename tuplevec_element<I, TupleVector>::type;

namespace Internal
{
	template <typename Indices, typename... Ts>
	struct TupleVecImpl;
}

// tuplevec_element helper to be able to isolate a type given an index
template <size_t I, typename T>
struct tuplevec_element
{
};

// attempt to isolate type given an index
template <size_t I>
struct tuplevec_element<I, tuple_vector<>>
{
	static_assert(I != I, "tuplevec_element index out of range");
};

template <typename T, typename... Ts>
struct tuplevec_element<0, tuple_vector<T, Ts...>>
{
	tuplevec_element() = delete; // tuplevec_element should only be used for compile-time assistance, and never be instantiated
	typedef T type;
};

template <size_t I, typename T, typename... Ts>
struct tuplevec_element<I, tuple_vector<T, Ts...>>
{
	typedef tuplevec_element_t<I - 1, tuple_vector<Ts...>> type;
};

template <size_t I, typename Indices, typename... Ts>
struct tuplevec_element<I, Internal::TupleVecImpl<Indices, Ts...>> : public tuplevec_element<I, tuple_vector<Ts...>>
{
};

// attempt to isolate index given a type
template <typename T, typename TupleVector>
struct tuplevec_index
{
};

template <typename T>
struct tuplevec_index<T, tuple_vector<>>
{
	typedef void DuplicateTypeCheck;
	tuplevec_index() = delete; // tuplevec_index should only be used for compile-time assistance, and never be instantiated
};

template <typename T, typename... TsRest>
struct tuplevec_index<T, tuple_vector<T, TsRest...>>
{
	typedef int DuplicateTypeCheck;
	static_assert(is_void<typename tuplevec_index<T, tuple_vector<TsRest...>>::DuplicateTypeCheck>::value, "duplicate type T in tuple_vector::get<T>(); unique types must be provided in declaration, or only use get<size_t>()");

	static const size_t index = 0;
};

template <typename T, typename Ts, typename... TsRest>
struct tuplevec_index<T, tuple_vector<Ts, TsRest...>>
{
	static const size_t index = tuplevec_index<T, tuple_vector<TsRest...>>::index + 1;
};

template <typename T, typename Indices, typename... Ts>
struct tuplevec_index<T, Internal::TupleVecImpl<Indices, Ts...>> : public tuplevec_index<T, tuple_vector<Ts...>>
{
};

namespace Internal
{

template <size_t I, typename T>
struct TupleVecLeaf
{
	// functions that get piped through swallow need to return some kind of value, hence why these are not void
	int DoConstruction(const size_t index)
	{
		::new(mpData + index) T();
		return 0;
	}

	int DoConstruction(const size_t index, const T& arg)
	{
		::new(mpData + index) T(arg);
		return 0;
	}

	int DoMove(T* pDest, const size_t srcElements)
	{
		eastl::uninitialized_move_ptr_if_noexcept(mpData, mpData + srcElements, pDest);
		eastl::destruct(mpData, mpData + srcElements);
		mpData = pDest;
		return 0;
	}

	int SetData(T* pData)
	{
		mpData = pData;
		return 0;
	}
	
	T* mpData = nullptr;
};

// swallow allows for parameter pack expansion of arguments as means of expanding operations performed
template <typename... Ts>
void swallow(Ts&&...)
{
}

// TupleVecImpl
template <size_t... Indices, typename... Ts>
class TupleVecImpl<integer_sequence<size_t, Indices...>, Ts...> : public TupleVecLeaf<Indices, Ts>...
{
public:
	typedef eastl_size_t size_type;
	
	EA_CONSTEXPR TupleVecImpl() = default;

	size_type push_back()
	{
		if (mNumElements == mNumCapacity)
		{
			DoGrow(mNumElements + 1);
		}
		swallow(TupleVecLeaf<Indices, Ts>::DoConstruction(mNumElements)...);
		++mNumElements;
		return mNumElements;
	}

	void push_back(const Ts&... args)
	{
		if (mNumElements == mNumCapacity)
		{
			DoGrow(mNumElements + 1);
		}
		swallow(TupleVecLeaf<Indices, Ts>::DoConstruction(mNumElements, args)...);
		++mNumElements;
	}

	void push_back_uninitialized()
	{
		if (mNumElements == mNumCapacity)
		{
			DoGrow(mNumElements + 1);
		}
		++mNumElements;
	}

	size_type size() const
	{
		return mNumElements;
	}

	size_type capacity() const
	{
		return mNumCapacity;
	}

	template<typename T>
	T* data() const
	{
		return TupleVecLeaf<tuplevec_index<T, tuple_vector<Ts...>>::index, T>::mpData;
	}

	template<size_t I, typename T>
	T* data() const
	{
		return TupleVecLeaf<I, tuplevec_element_t<I, tuple_vector<Ts...>>::mpData;
	}

private:
	void* mpBegin = nullptr;
	size_type mNumElements = 0;
	size_type mNumCapacity = 0;

	EASTLAllocatorType mAllocator = EASTLAllocatorType(EASTL_VECTOR_DEFAULT_NAME);

private:

	void DoGrow(size_type n)
	{
		void* pNewData = DoAllocate(n);
		if (mpBegin)
		{
			size_type oldCapacity = capacity();
			swallow(TupleVecLeaf<Indices, Ts>::DoMove(static_cast<int*>(pNewData), mNumElements)...);
			// dcrooks-todo really need to calculate proper sizeof here
			EASTLFree(mAllocator, mpBegin, oldCapacity * sizeof(int));
		}
		else
		{
			swallow(TupleVecLeaf<Indices, Ts>::SetData(static_cast<int*>(pNewData))...);
		}
		mpBegin = pNewData;
		mNumCapacity = n;
	}

	void* DoAllocate(size_type n)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(n >= 0x80000000))
			EASTL_FAIL_MSG("tuple_vector::DoAllocate -- improbably large request.");
#endif

		// If n is zero, then we allocate no memory and just return NULL. 
		// This is fine, as our default ctor initializes with NULL pointers. 
		// dcrooks-todo really need to calculate proper sizeof here
		return n ? allocate_memory(mAllocator, n * sizeof(int), EASTL_ALIGN_OF(int), 0) : nullptr;
	}
};

template <size_t I, typename Indices, typename... Ts>
tuplevec_element_t<I, TupleVecImpl<Indices, Ts...>>* get(TupleVecImpl<Indices, Ts...>& t)
{
	typedef tuplevec_element_t<I, TupleVecImpl<Indices, Ts...>> Element;
	return t.data<Element>();
}

template <typename T, typename Indices, typename... Ts>
T* get(TupleVecImpl<Indices, Ts...>& t)
{
	return t.data<T>();
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
	//template <typename... Ts>
	//struct element // dcrooks-todo maybe this'll just be iterator?
	//{
	//public:
	//	element(tuple_vector<Ts...>& tupleVector, size_t index = 0)
	//	: mIndex(index)
	//	, mTupleVector(tupleVector)
	//	{	}

	//	template<size_t I>
	//	tuplevec_element_t<I, tuple_vector<Ts...>>& get() { return mTupleVector.get<I>()[mIndex];	}

	//	template<typename T> 
	//	T& get() { return mTupleVector.get<T>()[mIndex]; }

	//	element& operator++()
	//	{
	//		++mIndex;
	//		return *this;
	//	}

	//	bool operator==(const element& other) const	{ return mIndex == other.mIndex; }
	//	bool operator!=(const element& other) const { return mIndex != other.mIndex; }
	//	element& operator*() { return *this; }

	//private:
	//	size_t mIndex;
	//	tuple_vector<Ts...> &mTupleVector;
	//};

	//typedef element<Ts...> element_type;
	typedef typename Impl::size_type size_type;


	EA_CONSTEXPR tuple_vector() = default;
	
	void push_back();
	void push_back(const Ts&... args);
	void push_back_uninitialized();

	size_type size();
	size_type capacity();

	//element_type begin();
	//element_type end();
	
	template<size_t I>
	tuplevec_element_t<I, tuple_vector<Ts...>>* get();

	template<typename T>
	T* get();

private:
	Impl mImpl;

};

template <typename... Ts>
void tuple_vector<Ts...>::push_back()
{
	mImpl.push_back();
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
typename tuple_vector<Ts...>::size_type tuple_vector<Ts...>::size()
{
	return mImpl.size();
}

template <typename... Ts>
typename tuple_vector<Ts...>::size_type tuple_vector<Ts...>::capacity()
{
	return mImpl.capacity();
}

template<typename... Ts>
template<size_t I>
eastl::tuplevec_element_t<I, tuple_vector<Ts...>>* tuple_vector<Ts...>::get()
{
	return Internal::get<I>(mImpl);
}

template<typename... Ts>
template<typename T>
T* tuple_vector<Ts...>::get()
{
	return Internal::get<T>(mImpl);
}

// Vector_Decl macros
//#pragma region 
//#define TUPLE_VECTOR_DECL_START(className, ... ) \
//	class className : public tuple_vector<##__VA_ARGS__> \
//	{\
//		public: \
//
//#define TUPLE_VECTOR_DECL_TYPENAME(type, name, iter) vector<type>& name() { return get<iter>(); } \
//
//#define TUPLE_VECTOR_DECL_END() };
//
//#define TUPLE_VECTOR_DECL_2(className, type0, name0, type1, name1) \
//	TUPLE_VECTOR_DECL_START(className, type0, type1) \
//	TUPLE_VECTOR_DECL_TYPENAME(type0, name0, 0) \
//	TUPLE_VECTOR_DECL_TYPENAME(type1, name1, 1) \
//	TUPLE_VECTOR_DECL_END()
//
//#define TUPLE_VECTOR_DECL_3(className, type0, name0, type1, name1, type2, name2) \
//	TUPLE_VECTOR_DECL_START(className, type0, type1, type2) \
//	TUPLE_VECTOR_DECL_TYPENAME(type0, name0, 0) \
//	TUPLE_VECTOR_DECL_TYPENAME(type1, name1, 1) \
//	TUPLE_VECTOR_DECL_TYPENAME(type2, name2, 2) \
//	TUPLE_VECTOR_DECL_END()
//
//#define TUPLE_VECTOR_DECL_4(className, type0, name0, type1, name1, type2, name2, type3, name3) \
//	TUPLE_VECTOR_DECL_START(className, type0, type1, type2, type3) \
//	TUPLE_VECTOR_DECL_TYPENAME(type0, name0, 0) \
//	TUPLE_VECTOR_DECL_TYPENAME(type1, name1, 1) \
//	TUPLE_VECTOR_DECL_TYPENAME(type2, name2, 2) \
//	TUPLE_VECTOR_DECL_TYPENAME(type3, name3, 3) \
//	TUPLE_VECTOR_DECL_END()
//#pragma endregion

}  // namespace eastl

#endif  // EASTL_TUPLEVECTOR_H
