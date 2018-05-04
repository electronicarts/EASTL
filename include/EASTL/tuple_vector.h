///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_TUPLEVECTOR_H
#define EASTL_TUPLEVECTOR_H

#include <EASTL/internal/config.h>
#include <EASTL/tuple.h>
#include <EASTL/utility.h>

namespace eastl
{


	/// EASTL_TUPLE_VECTOR_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_TUPLE_VECTOR_DEFAULT_NAME
	#define EASTL_TUPLE_VECTOR_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " tuple-vector" // Unless the user overrides something, this is "EASTL tuple-vector".
	#endif


	/// EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR
	#define EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR allocator_type(EASTL_TUPLE_VECTOR_DEFAULT_NAME)
	#endif



// forward declarations
template <typename... Ts>
class tuple_vector;

template <size_t I, typename TupleVector>
struct tuplevec_element;

template <size_t I, typename TupleVector>
using tuplevec_element_t = typename tuplevec_element<I, TupleVector>::type;

namespace TupleVecInternal
{
	template <typename Allocator, typename Indices, typename... Ts>
	struct TupleVecImpl;

	template <typename... Ts>
	struct TupleRecurser;

	template <size_t I, typename... Ts>
	struct TupleIndexRecurser;

	template < typename... Ts>
	struct TupleVecIter;
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

template <typename Allocator, size_t I, typename Indices, typename... Ts>
struct tuplevec_element<I, TupleVecInternal::TupleVecImpl<Allocator, Indices, Ts...>> : public tuplevec_element<I, tuple_vector<Ts...>>
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
	static const size_t index = 0;
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
	typedef typename tuplevec_index<T, tuple_vector<TsRest...>>::DuplicateTypeCheck DuplicateTypeCheck;
	static const size_t index = tuplevec_index<T, tuple_vector<TsRest...>>::index + 1;
};

template <typename Allocator, typename T, typename Indices, typename... Ts>
struct tuplevec_index<T, TupleVecInternal::TupleVecImpl<Allocator, Indices, Ts...>> : public tuplevec_index<T, tuple_vector<Ts...>>
{
};

namespace TupleVecInternal
{

// helper to calculate the sizeof the full tuple
template <>
struct TupleRecurser<>
{
	// This class should never be instantiated. This is just a helper for working with static functions when anonymous functions don't work
	// and provide some other utilities
	TupleRecurser() = delete;
		
	static constexpr size_t GetTotalAlignment()
	{
		return 0;
	}

	template<typename Allocator, size_t I, typename Indices, typename... VecTypes>
	static pair<void*, size_t> DoAllocate(TupleVecImpl<Allocator, Indices, VecTypes...> &vec, size_t capacity, size_t offset)
	{
		// If n is zero, then we allocate no memory and just return NULL. 
		// This is fine, as our default ctor initializes with NULL pointers. 
		size_t alignment = TupleRecurser<VecTypes...>::GetTotalAlignment();
		void* ptr = capacity ? allocate_memory(vec.mAllocator, offset, alignment, 0) : nullptr;
		return make_pair(ptr, offset);
	}
};

template <typename T, typename... Ts>
struct TupleRecurser<T, Ts...> : TupleRecurser<Ts...>
{
	static constexpr size_t GetTotalAlignment()
	{
		return max(alignof(T), TupleRecurser<Ts...>::GetTotalAlignment());
	}

	template<typename Allocator, size_t I, typename Indices, typename... VecTypes>
	static pair<void*, size_t> DoAllocate(TupleVecImpl<Allocator, Indices, VecTypes...> &vec, size_t capacity, size_t offset)
	{
		size_t alignment = alignof(T);
		size_t offsetBegin = (offset + alignment - 1) & (~alignment + 1);
		size_t offsetEnd = offsetBegin + sizeof(T) * capacity;
		auto allocation = TupleRecurser<Ts...>::DoAllocate<Allocator, I+1, Indices, VecTypes...>(vec, capacity, offsetEnd);
		void* pDest = (char*)(allocation.first)+offsetBegin;
		vec.TupleVecLeaf<I, T>::DoMove(pDest, vec.mNumElements);
		return allocation;
	}
};


template <typename... Ts>
struct TupleIndexRecurser<0, Ts...>
{
	static void DoPtrMove(tuple<Ts*...>& tuple, ptrdiff_t offset)
	{
		get<0>(tuple) += offset;
	}
};


template <size_t I, typename... Ts>
struct TupleIndexRecurser<I, Ts...>
{
	static void DoPtrMove(tuple<Ts*...>& tuple, ptrdiff_t offset)
	{
		get<I>(tuple) += offset;
		TupleIndexRecurser<I - 1, Ts...>::DoPtrMove(tuple, offset);
	}
};

template <typename... Ts>
static void DoPtrMove(tuple<Ts*...>& tuple, ptrdiff_t offset)
{
	TupleIndexRecurser<sizeof...(Ts)-1, Ts... >::DoPtrMove(tuple, offset);
}


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

	int DoMove(void* pDest, const size_t srcNumElements)
	{
		eastl::uninitialized_move_ptr_if_noexcept(mpData, mpData + srcNumElements, (T*)pDest);
		eastl::destruct(mpData, mpData + srcNumElements);
		mpData = (T*)pDest;
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
template <typename Allocator, size_t... Indices, typename... Ts>
class TupleVecImpl<Allocator, integer_sequence<size_t, Indices...>, Ts...> : public TupleVecLeaf<Indices, Ts>...
{
	typedef Allocator    allocator_type;

public:
	typedef eastl_size_t size_type;

	TupleVecImpl()
		: mAllocator(allocator_type(EASTL_VECTOR_DEFAULT_NAME))
	{}
	TupleVecImpl(const allocator_type& allocator)
		: mAllocator(allocator)
	{}
	~TupleVecImpl()
	{ 
		if (mpData)
			EASTLFree(mAllocator, mpData, mDataSize); 
	}

	size_type push_back()
	{
		if (mNumElements >= mNumCapacity)
		{
			DoGrow(GetNewCapacity(mNumCapacity));
		}
		swallow(TupleVecLeaf<Indices, Ts>::DoConstruction(mNumElements)...);
		++mNumElements;
		return mNumElements;
	}

	void push_back(const Ts&... args)
	{
		if (mNumElements >= mNumCapacity)
		{
			DoGrow(GetNewCapacity(mNumCapacity));
		}
		swallow(TupleVecLeaf<Indices, Ts>::DoConstruction(mNumElements, args)...);
		++mNumElements;
	}

	void push_back_uninitialized()
	{
		if (mNumElements >= mNumCapacity)
		{
			DoGrow(GetNewCapacity(mNumCapacity));
		}
		++mNumElements;
	}

	void reserve(size_t n)
	{
		if (n > mNumCapacity)
		{
			DoGrow(n);
		}
	}

	size_type size() const
	{
		return mNumElements;
	}

	size_type capacity() const
	{
		return mNumCapacity;
	}

protected:
	allocator_type mAllocator;

private:
	void* mpData = nullptr;
	size_type mDataSize = 0;
	size_type mNumElements = 0;
	size_type mNumCapacity = 0;


	friend struct TupleRecurser<>;
	template<typename... Ts>
	friend struct TupleRecurser;

	void DoGrow(size_type n)
	{
		DoAllocate(n);
	}

	void DoAllocate(size_type n)
	{
		auto allocation = TupleRecurser<Ts...>::DoAllocate<allocator_type, 0, integer_sequence<size_t, Indices...>, Ts...>(*this, n, 0);
		EASTLFree(mAllocator, mpData, mDataSize);
		mpData = allocation.first;
		mDataSize = allocation.second;
		mNumCapacity = n;
	}

	size_type GetNewCapacity(size_type currentCapacity)
	{
		return (currentCapacity > 0) ? (2 * currentCapacity) : 1;
	}

};

template <size_t I, typename TupleVecType>
tuplevec_element_t<I, TupleVecType>* get(TupleVecType& t)
{
	typedef tuplevec_element_t<I, TupleVecType> Element;
	return t.TupleVecLeaf<I, Element>::mpData;
}

template <typename T, typename TupleVecType>
T* get(TupleVecType& t)
{
	typedef tuplevec_index<T, TupleVecType> Index;
	return t.TupleVecLeaf<Index::index, T>::mpData;
}

// The Iterator operates by storing a persistent index internally,
// and resolving the tuple of pointers to the various parts of the original tupleVec when dereferenced.
// While resolving the tuple is a non-zero operation, it consistently generated better code than the alternative of
// storing - and harmoniously updating on each modification - a full tuple of pointers to the tupleVec's data

template <typename... Ts>
struct TupleVecIter : public iterator<random_access_iterator_tag, tuple<Ts...>, ptrdiff_t, tuple<Ts*...>, tuple<Ts&...>>
{
private:
	typedef TupleVecIter<Ts...> this_type;

public:
	TupleVecIter() = default;
	TupleVecIter(tuple_vector<Ts...>& tupleVec, size_t index)
		: mTupleVec(&tupleVec), mIndex(index) { }

	bool operator==(const TupleVecIter& other) const { return mIndex == other.mIndex && mTupleVec->get<0>() == other.mTupleVec->get<0>(); }
	bool operator!=(const TupleVecIter& other) const { return mIndex != other.mIndex || mTupleVec->get<0>() != other.mTupleVec->get<0>(); }
	reference operator*() { return MakeReference(make_index_sequence<sizeof...(Ts)>()); }

	this_type& operator++() { ++mIndex; return *this; }
	this_type operator++(int)
	{
		this_type temp = *this;
		++mIndex;
		return temp;
	}

	this_type& operator--() { --mIndex; return *this; }
	this_type operator--(int)
	{
		this_type temp = *this;
		--mIndex;
		return temp;
	}

	this_type& operator+=(difference_type n) { mIndex += n; return *this; }
	this_type operator+(difference_type n)
	{
		this_type temp = *this;
		return temp += n;
	}
	friend this_type operator+(difference_type n, const this_type& rhs)
	{
		this_type temp = rhs;
		return temp += n;
	}

	this_type& operator-=(difference_type n) { mIndex -= n; return *this; }
	this_type operator-(difference_type n)
	{
		this_type temp = *this;
		return temp -= n;
	}
	friend this_type operator-(difference_type n, const this_type& rhs)
	{
		this_type temp = rhs;
		return temp -= n;
	}

	difference_type operator-(const this_type& rhs) { return mIndex - rhs.mIndex; }
	bool operator<(const this_type& rhs) { return mIndex < rhs.mIndex; }
	bool operator>(const this_type& rhs) { return mIndex > rhs.mIndex; }
	bool operator>=(const this_type& rhs) { return mIndex >= rhs.mIndex; }
	bool operator<=(const this_type& rhs) { return mIndex <= rhs.mIndex; }

	reference operator[](size_t n)
	{
		return *(*this + n);
	}

private:

	template <size_t... Indices>
	value_type MakeValue(integer_sequence<size_t, Indices...> indices)
	{
		return value_type(mTupleVec->get<Indices>()[mIndex]...);
	}

	template <size_t... Indices>
	reference MakeReference(integer_sequence<size_t, Indices...> indices)
	{
		return reference(mTupleVec->get<Indices>()[mIndex]...);
	}

	template <size_t... Indices>
	pointer MakePointer(integer_sequence<size_t, Indices...> indices)
	{
		return pointer(&mTupleVec->get<Indices>()[mIndex]...);
	}

	size_t mIndex = 0;
	tuple_vector<Ts...> *mTupleVec = nullptr;
};

}  // namespace TupleVecInternal

//template <typename... Ts>
//void iter_swap(TupleVecInternal::TupleVecIter<Ts...>& a, TupleVecInternal::TupleVecIter<Ts...>& b)
//{
//	TupleVecInternal::TupleIndexRecurser<sizeof...(Ts)-1, Ts... >::DoSwap(a.mTupleVec->mImpl, b.mTupleVec->mImpl, a.mIndex, b.mIndex);
//}

// External interface of tuple_vector
template <typename... Ts>
class tuple_vector
{
private:
	typedef TupleVecInternal::TupleVecImpl<EASTLAllocatorType, make_index_sequence<sizeof...(Ts)>, Ts...> Impl;

public:
	typedef TupleVecInternal::TupleVecIter<Ts...> iterator;
	typedef typename Impl::size_type size_type;

	EA_CONSTEXPR tuple_vector() = default;
	
	void push_back();
	void push_back(const Ts&... args);
	void push_back_uninitialized();

	size_type size();
	size_type capacity();

	iterator begin();
	iterator end();

	void reserve(size_t n);

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

template <typename... Ts>
typename tuple_vector<Ts...>::iterator tuple_vector<Ts...>::begin()
{
	return tuple_vector<Ts...>::iterator(*this, 0);
}

template <typename... Ts>
typename tuple_vector<Ts...>::iterator tuple_vector<Ts...>::end()
{
	return tuple_vector<Ts...>::iterator(*this, size());
}

template <typename...Ts>
void tuple_vector<Ts...>::reserve(size_t n)
{
	mImpl.reserve(n);
}

template<typename... Ts>
template<size_t I>
tuplevec_element_t<I, tuple_vector<Ts...>>* tuple_vector<Ts...>::get()
{
	return TupleVecInternal::get<I>(mImpl);
}

template<typename... Ts>
template<typename T>
T* tuple_vector<Ts...>::get()
{
	return TupleVecInternal::get<T>(mImpl);
}

}  // namespace eastl

#endif  // EASTL_TUPLEVECTOR_H
