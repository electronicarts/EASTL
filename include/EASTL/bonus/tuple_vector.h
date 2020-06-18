///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// tuple_vector is a data container that is designed to abstract and simplify
// the handling of a "structure of arrays" layout of data in memory. In
// particular, it mimics the interface of vector, including functionality to do
// inserts, erases, push_backs, and random-access. It also provides a
// RandomAccessIterator and corresponding functionality, making it compatible
// with most STL (and STL-esque) algorithms such as ranged-for loops, find_if,
// remove_if, or sort.

// When used or applied properly, this container can improve performance of
// some algorithms through cache-coherent data accesses or allowing for
// sensible SIMD programming, while keeping the structure of a single
// container, to permit a developer to continue to use existing algorithms in
// STL and the like.
//
// Consult doc/Bonus/tuple_vector_readme.md for more information.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_TUPLEVECTOR_H
#define EASTL_TUPLEVECTOR_H

#include <EASTL/bonus/compressed_pair.h>
#include <EASTL/internal/config.h>
#include <EASTL/iterator.h>
#include <EASTL/memory.h>
#include <EASTL/tuple.h>
#include <EASTL/utility.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif

EA_DISABLE_VC_WARNING(4244) // warning C4244: 'conversion from '___' to '___', possible loss of data
EA_DISABLE_VC_WARNING(4623) // warning C4623: default constructor was implicitly defined as deleted
EA_DISABLE_VC_WARNING(4625) // warning C4625: copy constructor was implicitly defined as deleted
EA_DISABLE_VC_WARNING(4510) // warning C4510: default constructor could not be generated

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

namespace TupleVecInternal
{

// forward declarations
template <eastl_size_t I, typename... Ts>
struct tuplevec_element;

template <eastl_size_t I, typename... Ts>
using tuplevec_element_t = typename tuplevec_element<I, Ts...>::type;

template <typename... Ts>
struct TupleTypes {};

template <typename Allocator, typename Indices, typename... Ts>
class TupleVecImpl;

template <typename... Ts>
struct TupleRecurser;

template <eastl_size_t I, typename... Ts>
struct TupleIndexRecurser;

template <eastl_size_t I, typename T>
struct TupleVecLeaf;

template <typename Indices, typename... Ts>
struct TupleVecIter;

// tuplevec_element helper to be able to isolate a type given an index
template <eastl_size_t I>
struct tuplevec_element<I>
{
	static_assert(I != I, "tuplevec_element index out of range");
};

template <typename T, typename... Ts>
struct tuplevec_element<0, T, Ts...>
{
	tuplevec_element() = delete; // tuplevec_element should only be used for compile-time assistance, and never be instantiated
	typedef T type;
};

template <eastl_size_t I, typename T, typename... Ts>
struct tuplevec_element<I, T, Ts...>
{
	typedef tuplevec_element_t<I - 1, Ts...> type;
};

// attempt to isolate index given a type
template <typename T, typename TupleVector>
struct tuplevec_index
{
};

template <typename T>
struct tuplevec_index<T, TupleTypes<>>
{
	typedef void DuplicateTypeCheck;
	tuplevec_index() = delete; // tuplevec_index should only be used for compile-time assistance, and never be instantiated
	static const eastl_size_t index = 0;
};

template <typename T, typename... TsRest>
struct tuplevec_index<T, TupleTypes<T, TsRest...>>
{
	typedef int DuplicateTypeCheck;
	static_assert(is_void<typename tuplevec_index<T, TupleTypes<TsRest...>>::DuplicateTypeCheck>::value, "duplicate type T in tuple_vector::get<T>(); unique types must be provided in declaration, or only use get<eastl_size_t>()");

	static const eastl_size_t index = 0;
};

template <typename T, typename Ts, typename... TsRest>
struct tuplevec_index<T, TupleTypes<Ts, TsRest...>>
{
	typedef typename tuplevec_index<T, TupleTypes<TsRest...>>::DuplicateTypeCheck DuplicateTypeCheck;
	static const eastl_size_t index = tuplevec_index<T, TupleTypes<TsRest...>>::index + 1;
};

template <typename Allocator, typename T, typename Indices, typename... Ts>
struct tuplevec_index<T, TupleVecImpl<Allocator, Indices, Ts...>> : public tuplevec_index<T, TupleTypes<Ts...>>
{
};


// helper to calculate the layout of the allocations for the tuple of types (esp. to take alignment into account)
template <>
struct TupleRecurser<>
{
	typedef eastl_size_t size_type;

	// This class should never be instantiated. This is just a helper for working with static functions when anonymous functions don't work
	// and provide some other utilities
	TupleRecurser() = delete;
		
	static EA_CONSTEXPR size_type GetTotalAlignment()
	{
		return 0;
	}

	static EA_CONSTEXPR size_type GetTotalAllocationSize(size_type capacity, size_type offset)
	{
		EA_UNUSED(capacity);
		return offset;
	}

	template<typename Allocator, size_type I, typename Indices, typename... VecTypes>
	static pair<void*, size_type> DoAllocate(TupleVecImpl<Allocator, Indices, VecTypes...> &vec, void** ppNewLeaf, size_type capacity, size_type offset)
	{
		EA_UNUSED(ppNewLeaf);

		// If n is zero, then we allocate no memory and just return NULL. 
		// This is fine, as our default ctor initializes with NULL pointers. 
		size_type alignment = TupleRecurser<VecTypes...>::GetTotalAlignment();
		void* ptr = capacity ? allocate_memory(vec.get_allocator(), offset, alignment, 0) : nullptr;

	#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY((size_t)ptr & (alignment - 1)) != 0)
		{
			EASTL_FAIL_MSG("tuple_vector::DoAllocate -- memory not alignment at requested alignment");
		}
	#endif

		return make_pair(ptr, offset);
	}

	template<typename TupleVecImplType, size_type I>
	static void SetNewData(TupleVecImplType &vec, void* pData, size_type capacity, size_type offset) 
	{ 
		EA_UNUSED(vec);
		EA_UNUSED(pData);
		EA_UNUSED(capacity);
		EA_UNUSED(offset);
	}
};

template <typename T, typename... Ts>
struct TupleRecurser<T, Ts...> : TupleRecurser<Ts...>
{
	typedef eastl_size_t size_type;
	
	static EA_CONSTEXPR size_type GetTotalAlignment()
	{
		return max(static_cast<size_type>(alignof(T)), TupleRecurser<Ts...>::GetTotalAlignment());
	}

	static EA_CONSTEXPR size_type GetTotalAllocationSize(size_type capacity, size_type offset)
	{
		return TupleRecurser<Ts...>::GetTotalAllocationSize(capacity, CalculateAllocationSize(offset, capacity));
	}

	template<typename Allocator, size_type I, typename Indices, typename... VecTypes>
	static pair<void*, size_type> DoAllocate(TupleVecImpl<Allocator, Indices, VecTypes...> &vec, void** ppNewLeaf, size_type capacity, size_type offset)
	{
		size_type allocationOffset = CalculatAllocationOffset(offset);
		size_type allocationSize = CalculateAllocationSize(offset, capacity);
		pair<void*, size_type> allocation = TupleRecurser<Ts...>::template DoAllocate<Allocator, I + 1, Indices, VecTypes...>(
			vec, ppNewLeaf, capacity, allocationSize);
		ppNewLeaf[I] = (void*)((uintptr_t)(allocation.first) + allocationOffset);
		return allocation;
	}

	template<typename TupleVecImplType, size_type I>
	static void SetNewData(TupleVecImplType &vec, void* pData, size_type capacity, size_type offset)
	{
		size_type allocationOffset = CalculatAllocationOffset(offset);
		size_type allocationSize = CalculateAllocationSize(offset, capacity);
		vec.TupleVecLeaf<I, T>::mpData = (T*)((uintptr_t)pData + allocationOffset);
		TupleRecurser<Ts...>::template SetNewData<TupleVecImplType, I + 1>(vec, pData, capacity, allocationSize);
	}

private:
	static EA_CONSTEXPR size_type CalculateAllocationSize(size_type offset, size_type capacity)
	{
		return CalculatAllocationOffset(offset) + sizeof(T) * capacity;
	}

	static EA_CONSTEXPR size_type CalculatAllocationOffset(size_type offset) { return (offset + alignof(T) - 1) & (~alignof(T) + 1); }
};

template <eastl_size_t I, typename T>
struct TupleVecLeaf
{
	typedef eastl_size_t size_type;

	void DoUninitializedMoveAndDestruct(const size_type begin, const size_type end, T* pDest)
	{
		T* pBegin = mpData + begin;
		T* pEnd = mpData + end;
		eastl::uninitialized_move_ptr_if_noexcept(pBegin, pEnd, pDest);
		eastl::destruct(pBegin, pEnd);
	}

	void DoInsertAndFill(size_type pos, size_type n, size_type numElements, const T& arg)
	{
		T* pDest = mpData + pos;
		T* pDataEnd = mpData + numElements;
		const T temp = arg;
		const size_type nExtra = (numElements - pos);
		if (n < nExtra) // If the inserted values are entirely within initialized memory (i.e. are before mpEnd)...
		{
			eastl::uninitialized_move_ptr(pDataEnd - n, pDataEnd, pDataEnd);
			eastl::move_backward(pDest, pDataEnd - n, pDataEnd); // We need move_backward because of potential overlap issues.
			eastl::fill(pDest, pDest + n, temp);
		}
		else
		{
			eastl::uninitialized_fill_n_ptr(pDataEnd, n - nExtra, temp);
			eastl::uninitialized_move_ptr(pDest, pDataEnd, pDataEnd + n - nExtra);
			eastl::fill(pDest, pDataEnd, temp);
		}
	}

	void DoInsertRange(T* pSrcBegin, T* pSrcEnd, T* pDestBegin, size_type numDataElements)
	{
		size_type pos = pDestBegin - mpData;
		size_type n = pSrcEnd - pSrcBegin;
		T* pDataEnd = mpData + numDataElements;
		const size_type nExtra = numDataElements - pos;
		if (n < nExtra) // If the inserted values are entirely within initialized memory (i.e. are before mpEnd)...
		{
			eastl::uninitialized_move_ptr(pDataEnd - n, pDataEnd, pDataEnd);
			eastl::move_backward(pDestBegin, pDataEnd - n, pDataEnd); // We need move_backward because of potential overlap issues.
			eastl::copy(pSrcBegin, pSrcEnd, pDestBegin);
		}
		else
		{
			eastl::uninitialized_copy(pSrcEnd - (n - nExtra), pSrcEnd, pDataEnd);
			eastl::uninitialized_move_ptr(pDestBegin, pDataEnd, pDataEnd + n - nExtra);
			eastl::copy(pSrcBegin, pSrcEnd - (n - nExtra), pDestBegin);
		}
	}

	void DoInsertValue(size_type pos, size_type numElements, T&& arg)
	{
		T* pDest = mpData + pos;
		T* pDataEnd = mpData + numElements;

		eastl::uninitialized_move_ptr(pDataEnd - 1, pDataEnd, pDataEnd);
		eastl::move_backward(pDest, pDataEnd - 1, pDataEnd); // We need move_backward because of potential overlap issues.
		eastl::destruct(pDest);
		::new (pDest) T(eastl::forward<T>(arg));
	}

	T* mpData = nullptr;
};

// swallow allows for parameter pack expansion of arguments as means of expanding operations performed
// if a void function is used for operation expansion, it should be wrapped in (..., 0) so that the compiler
// thinks it has a parameter to pass into the function
template <typename... Ts>
void swallow(Ts&&...) { }

inline bool variadicAnd(bool cond) { return cond; }

inline bool variadicAnd(bool cond, bool conds...) { return cond && variadicAnd(conds); }

// Helper struct to check for strict compatibility between two iterators, whilst still allowing for
// conversion between TupleVecImpl<Ts...>::iterator and TupleVecImpl<Ts...>::const_iterator. 
template <bool IsSameSize, typename From, typename To>
struct TupleVecIterCompatibleImpl : public false_type { };
	
template<>
struct TupleVecIterCompatibleImpl<true, TupleTypes<>, TupleTypes<>> : public true_type { };

template <typename From, typename... FromRest, typename To, typename... ToRest>
struct TupleVecIterCompatibleImpl<true, TupleTypes<From, FromRest...>, TupleTypes<To, ToRest...>> : public integral_constant<bool,
		TupleVecIterCompatibleImpl<true, TupleTypes<FromRest...>, TupleTypes<ToRest...>>::value &&
		is_same<typename remove_const<From>::type, typename remove_const<To>::type>::value >
{ };

template <typename From, typename To>
struct TupleVecIterCompatible;

template<typename... Us, typename... Ts>
struct TupleVecIterCompatible<TupleTypes<Us...>, TupleTypes<Ts...>> :
	public TupleVecIterCompatibleImpl<sizeof...(Us) == sizeof...(Ts), TupleTypes<Us...>, TupleTypes<Ts...>>
{ };

// The Iterator operates by storing a persistent index internally,
// and resolving the tuple of pointers to the various parts of the original tupleVec when dereferenced.
// While resolving the tuple is a non-zero operation, it consistently generated better code than the alternative of
// storing - and harmoniously updating on each modification - a full tuple of pointers to the tupleVec's data
template <eastl_size_t... Indices, typename... Ts>
struct TupleVecIter<index_sequence<Indices...>, Ts...>
	: public iterator<random_access_iterator_tag, tuple<Ts...>, eastl_size_t, tuple<Ts*...>, tuple<Ts&...>>
{
private:
	typedef TupleVecIter<index_sequence<Indices...>, Ts...> this_type;
	typedef eastl_size_t size_type;

	typedef iterator<random_access_iterator_tag, tuple<Ts...>, eastl_size_t, tuple<Ts*...>, tuple<Ts&...>> iter_type;

	template<typename U, typename... Us> 
	friend struct TupleVecIter;

	template<typename U, typename V, typename... Us>
	friend class TupleVecImpl;

	template<typename U>
	friend class move_iterator;
public:
	typedef typename iter_type::iterator_category iterator_category;
	typedef typename iter_type::value_type value_type;
	typedef typename iter_type::difference_type difference_type;
	typedef typename iter_type::pointer pointer;
	typedef typename iter_type::reference reference;

	TupleVecIter() = default;

	template<typename VecImplType>
	TupleVecIter(VecImplType* tupleVec, size_type index)
		: mIndex(index)
		, mpData{(void*)tupleVec->TupleVecLeaf<Indices, Ts>::mpData...}
	{ }

	template <typename OtherIndicesType, typename... Us,
			  typename = typename enable_if<TupleVecIterCompatible<TupleTypes<Us...>, TupleTypes<Ts...>>::value, bool>::type>
	TupleVecIter(const TupleVecIter<OtherIndicesType, Us...>& other)
		: mIndex(other.mIndex)
		, mpData{other.mpData[Indices]...}
	{
	}

	bool operator==(const TupleVecIter& other) const { return mIndex == other.mIndex && mpData[0] == other.mpData[0]; }
	bool operator!=(const TupleVecIter& other) const { return mIndex != other.mIndex || mpData[0] != other.mpData[0]; }
	reference operator*() const { return MakeReference(); }

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
	this_type operator+(difference_type n) const
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
	this_type operator-(difference_type n) const
	{
		this_type temp = *this;
		return temp -= n;
	}
	friend this_type operator-(difference_type n, const this_type& rhs)
	{
		this_type temp = rhs;
		return temp -= n;
	}

	difference_type operator-(const this_type& rhs) const { return mIndex - rhs.mIndex; }
	bool operator<(const this_type& rhs) const { return mIndex < rhs.mIndex; }
	bool operator>(const this_type& rhs) const { return mIndex > rhs.mIndex; }
	bool operator>=(const this_type& rhs) const { return mIndex >= rhs.mIndex; }
	bool operator<=(const this_type& rhs) const { return mIndex <= rhs.mIndex; }

	reference operator[](const size_type n) const
	{
		return *(*this + n);
	}

private:

	value_type MakeValue() const
	{
		return value_type(((Ts*)mpData[Indices])[mIndex]...);
	}

	reference MakeReference() const
	{
		return reference(((Ts*)mpData[Indices])[mIndex]...);
	}

	pointer MakePointer() const
	{
		return pointer(&((Ts*)mpData[Indices])[mIndex]...);
	}

	size_type mIndex = 0;
	const void* mpData[sizeof...(Ts)];
};

// TupleVecImpl
template <typename Allocator, eastl_size_t... Indices, typename... Ts>
class TupleVecImpl<Allocator, index_sequence<Indices...>, Ts...> : public TupleVecLeaf<Indices, Ts>...
{
	typedef Allocator	allocator_type;
	typedef index_sequence<Indices...> index_sequence_type;
	typedef TupleVecImpl<Allocator, index_sequence_type, Ts...> this_type;
	typedef TupleVecImpl<Allocator, index_sequence_type, const Ts...> const_this_type;

public:
	typedef TupleVecInternal::TupleVecIter<index_sequence_type, Ts...> iterator;
	typedef TupleVecInternal::TupleVecIter<index_sequence_type, const Ts...> const_iterator;
	typedef eastl::reverse_iterator<iterator> reverse_iterator;
	typedef eastl::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef eastl_size_t size_type;
	typedef eastl::tuple<Ts...> value_tuple;
	typedef eastl::tuple<Ts&...> reference_tuple;
	typedef eastl::tuple<const Ts&...> const_reference_tuple;
	typedef eastl::tuple<Ts*...> ptr_tuple;
	typedef eastl::tuple<const Ts*...> const_ptr_tuple;
	typedef eastl::tuple<Ts&&...> rvalue_tuple;

	TupleVecImpl()
		: mDataSizeAndAllocator(0, EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
	{}

	TupleVecImpl(const allocator_type& allocator)
		: mDataSizeAndAllocator(0, allocator)
	{}

	TupleVecImpl(this_type&& x)
		: mDataSizeAndAllocator(0, eastl::move(x.get_allocator()))
	{
		swap(x);
	}

	TupleVecImpl(this_type&& x, const Allocator& allocator) 
		: mDataSizeAndAllocator(0, allocator)
	{
		if (get_allocator() == x.get_allocator()) // If allocators are equivalent, then we can safely swap member-by-member
		{
			swap(x);
		}
		else
		{
			this_type temp(eastl::move(*this));
			temp.swap(x);
		}
	}

	TupleVecImpl(const this_type& x) 
		: mDataSizeAndAllocator(0, x.get_allocator())
	{
		DoInitFromIterator(x.begin(), x.end());
	}

	template<typename OtherAllocator>
	TupleVecImpl(const TupleVecImpl<OtherAllocator, index_sequence_type, Ts...>& x, const Allocator& allocator)  
		: mDataSizeAndAllocator(0, allocator)
	{
		DoInitFromIterator(x.begin(), x.end());
	}

	template<typename MoveIterBase>
	TupleVecImpl(move_iterator<MoveIterBase> begin, move_iterator<MoveIterBase> end, const allocator_type& allocator = EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
		: mDataSizeAndAllocator(0, allocator)
	{
		DoInitFromIterator(begin, end);
	}

	TupleVecImpl(const_iterator begin, const_iterator end, const allocator_type& allocator = EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
		: mDataSizeAndAllocator(0, allocator )
	{
		DoInitFromIterator(begin, end);
	}

	TupleVecImpl(size_type n, const allocator_type& allocator = EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
		: mDataSizeAndAllocator(0, allocator)
	{
		DoInitDefaultFill(n);
	}

	TupleVecImpl(size_type n, const Ts&... args) 
		: mDataSizeAndAllocator(0, EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
	{
		DoInitFillArgs(n, args...);
	}

	TupleVecImpl(size_type n, const Ts&... args, const allocator_type& allocator) 
		: mDataSizeAndAllocator(0, allocator)
	{
		DoInitFillArgs(n, args...);
	}

	TupleVecImpl(size_type n, const_reference_tuple tup, const allocator_type& allocator = EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
		: mDataSizeAndAllocator(0, allocator)
	{
		DoInitFillTuple(n, tup);
	}

	TupleVecImpl(const value_tuple* first, const value_tuple* last, const allocator_type& allocator = EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
		: mDataSizeAndAllocator(0, allocator)
	{
		DoInitFromTupleArray(first, last);
	}

	TupleVecImpl(std::initializer_list<value_tuple> iList, const allocator_type& allocator = EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
		: mDataSizeAndAllocator(0, allocator)
	{
		DoInitFromTupleArray(iList.begin(), iList.end());
	}

protected:
	// ctor to provide a pre-allocated field of data that the container will own, specifically for fixed_tuple_vector
	TupleVecImpl(const allocator_type& allocator, void* pData, size_type capacity, size_type dataSize)
		: mpData(pData), mNumCapacity(capacity), mDataSizeAndAllocator(dataSize, allocator)
	{
		TupleRecurser<Ts...>::template SetNewData<this_type, 0>(*this, mpData, mNumCapacity, 0);
	}

public:
	~TupleVecImpl()
	{ 
		swallow((eastl::destruct(TupleVecLeaf<Indices, Ts>::mpData, TupleVecLeaf<Indices, Ts>::mpData + mNumElements), 0)...);
		if (mpData)
			EASTLFree(get_allocator(), mpData, internalDataSize()); 
	}

	void assign(size_type n, const Ts&... args)
	{
		if (n > mNumCapacity)
		{
			this_type temp(n, args..., get_allocator()); // We have little choice but to reallocate with new memory.
			swap(temp);
		}
		else if (n > mNumElements) // If n > mNumElements ...
		{
			size_type oldNumElements = mNumElements;
			swallow((eastl::fill(TupleVecLeaf<Indices, Ts>::mpData, TupleVecLeaf<Indices, Ts>::mpData + oldNumElements, args), 0)...);
			swallow((eastl::uninitialized_fill_ptr(TupleVecLeaf<Indices, Ts>::mpData + oldNumElements,
					                       TupleVecLeaf<Indices, Ts>::mpData + n, args), 0)...);
			mNumElements = n;
		}
		else // else 0 <= n <= mNumElements
		{
			swallow((eastl::fill(TupleVecLeaf<Indices, Ts>::mpData, TupleVecLeaf<Indices, Ts>::mpData + n, args), 0)...);
			erase(begin() + n, end());
		}
	}

	void assign(const_iterator first, const_iterator last)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(!validate_iterator_pair(first, last)))
			EASTL_FAIL_MSG("tuple_vector::assign -- invalid iterator pair");
#endif
		size_type newNumElements = last - first;
		if (newNumElements > mNumCapacity)
		{
			this_type temp(first, last, get_allocator());
			swap(temp);
		}
		else
		{
			const void* ppOtherData[sizeof...(Ts)] = {first.mpData[Indices]...};
			size_type firstIdx = first.mIndex;
			size_type lastIdx = last.mIndex;
			if (newNumElements > mNumElements) // If n > mNumElements ...
			{
				size_type oldNumElements = mNumElements;
				swallow((eastl::copy((Ts*)(ppOtherData[Indices]) + firstIdx,
						       (Ts*)(ppOtherData[Indices]) + firstIdx + oldNumElements,
						       TupleVecLeaf<Indices, Ts>::mpData), 0)...);
				swallow((eastl::uninitialized_copy_ptr((Ts*)(ppOtherData[Indices]) + firstIdx + oldNumElements,
						                       (Ts*)(ppOtherData[Indices]) + lastIdx,
						                       TupleVecLeaf<Indices, Ts>::mpData + oldNumElements), 0)...);
				mNumElements = newNumElements;
			}
			else // else 0 <= n <= mNumElements
			{
				swallow((eastl::copy((Ts*)(ppOtherData[Indices]) + firstIdx, (Ts*)(ppOtherData[Indices]) + lastIdx,
						       TupleVecLeaf<Indices, Ts>::mpData), 0)...);
				erase(begin() + newNumElements, end());
			}
		}
	}

	void assign(const value_tuple* first, const value_tuple* last)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(first > last || first == nullptr || last == nullptr))
			EASTL_FAIL_MSG("tuple_vector::assign from tuple array -- invalid ptrs");
#endif
		size_type newNumElements = last - first;
		if (newNumElements > mNumCapacity)
		{
			this_type temp(first, last, get_allocator());
			swap(temp);
		}
		else
		{
			if (newNumElements > mNumElements) // If n > mNumElements ...
			{
				size_type oldNumElements = mNumElements;
				
				DoCopyFromTupleArray(begin(), begin() + oldNumElements, first);
				DoUninitializedCopyFromTupleArray(begin() + oldNumElements, begin() + newNumElements, first + oldNumElements);
				mNumElements = newNumElements;
			}
			else // else 0 <= n <= mNumElements
			{
				DoCopyFromTupleArray(begin(), begin() + newNumElements, first);
				erase(begin() + newNumElements, end());
			}
		}
	}

	reference_tuple push_back()
	{
		size_type oldNumElements = mNumElements;
		size_type newNumElements = oldNumElements + 1;
		size_type oldNumCapacity = mNumCapacity;
		mNumElements = newNumElements;
		DoGrow(oldNumElements, oldNumCapacity, newNumElements);
		swallow(::new(TupleVecLeaf<Indices, Ts>::mpData + oldNumElements) Ts()...);
		return back();
	}

	void push_back(const Ts&... args)
	{
		size_type oldNumElements = mNumElements;
		size_type newNumElements = oldNumElements + 1;
		size_type oldNumCapacity = mNumCapacity;
		mNumElements = newNumElements;
		DoGrow(oldNumElements, oldNumCapacity, newNumElements);
		swallow(::new(TupleVecLeaf<Indices, Ts>::mpData + oldNumElements) Ts(args)...);
	}

	void push_back_uninitialized()
	{
		size_type oldNumElements = mNumElements;
		size_type newNumElements = oldNumElements + 1;
		size_type oldNumCapacity = mNumCapacity;
		mNumElements = newNumElements;
		DoGrow(oldNumElements, oldNumCapacity, newNumElements);
	}
	
	reference_tuple emplace_back(Ts&&... args)
	{
		size_type oldNumElements = mNumElements;
		size_type newNumElements = oldNumElements + 1;
		size_type oldNumCapacity = mNumCapacity;
		mNumElements = newNumElements;
		DoGrow(oldNumElements, oldNumCapacity, newNumElements);
		swallow(::new(TupleVecLeaf<Indices, Ts>::mpData + oldNumElements) Ts(eastl::forward<Ts>(args))...);
		return back();
	}

	iterator emplace(const_iterator pos, Ts&&... args)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(validate_iterator(pos) == isf_none))
			EASTL_FAIL_MSG("tuple_vector::emplace -- invalid iterator");
#endif
		size_type firstIdx = pos - cbegin();
		size_type oldNumElements = mNumElements;
		size_type newNumElements = mNumElements + 1;
		size_type oldNumCapacity = mNumCapacity;
		mNumElements = newNumElements;
		if (newNumElements > oldNumCapacity || firstIdx != oldNumElements)
		{
			if (newNumElements > oldNumCapacity)
			{
				const size_type newCapacity = max(GetNewCapacity(oldNumCapacity), newNumElements);

				void* ppNewLeaf[sizeof...(Ts)];
				pair<void*, size_type> allocation =	TupleRecurser<Ts...>::template DoAllocate<allocator_type, 0, index_sequence_type, Ts...>(
					*this, ppNewLeaf, newCapacity, 0);

				swallow((TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(
					0, firstIdx, (Ts*)ppNewLeaf[Indices]), 0)...);
				swallow((TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(
					firstIdx, oldNumElements, (Ts*)ppNewLeaf[Indices] + firstIdx + 1), 0)...);
				swallow(::new ((Ts*)ppNewLeaf[Indices] + firstIdx) Ts(eastl::forward<Ts>(args))...);
				swallow(TupleVecLeaf<Indices, Ts>::mpData = (Ts*)ppNewLeaf[Indices]...);

				EASTLFree(get_allocator(), mpData, internalDataSize());
				mpData = allocation.first;
				mNumCapacity = newCapacity;
				internalDataSize() = allocation.second;
			}
			else
			{
				swallow((TupleVecLeaf<Indices, Ts>::DoInsertValue(firstIdx, oldNumElements, eastl::forward<Ts>(args)), 0)...);
			}
		}
		else
		{
			swallow(::new (TupleVecLeaf<Indices, Ts>::mpData + oldNumElements) Ts(eastl::forward<Ts>(args))...);
		}
		return begin() + firstIdx;
	}

	iterator insert(const_iterator pos, size_type n, const Ts&... args)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(validate_iterator(pos) == isf_none))
			EASTL_FAIL_MSG("tuple_vector::insert -- invalid iterator");
#endif
		size_type firstIdx = pos - cbegin();
		size_type lastIdx = firstIdx + n;
		size_type oldNumElements = mNumElements;
		size_type newNumElements = mNumElements + n;
		size_type oldNumCapacity = mNumCapacity;
		mNumElements = newNumElements;
		if (newNumElements > oldNumCapacity || firstIdx != oldNumElements)
		{
			if (newNumElements > oldNumCapacity)
			{
				const size_type newCapacity = max(GetNewCapacity(oldNumCapacity), newNumElements);

				void* ppNewLeaf[sizeof...(Ts)];
				pair<void*, size_type> allocation = TupleRecurser<Ts...>::template DoAllocate<allocator_type, 0, index_sequence_type, Ts...>(
						*this, ppNewLeaf, newCapacity, 0);

				swallow((TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(
					0, firstIdx, (Ts*)ppNewLeaf[Indices]), 0)...);
				swallow((TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(
					firstIdx, oldNumElements, (Ts*)ppNewLeaf[Indices] + lastIdx), 0)...);
				swallow((eastl::uninitialized_fill_ptr((Ts*)ppNewLeaf[Indices] + firstIdx, (Ts*)ppNewLeaf[Indices] + lastIdx, args), 0)...);
				swallow(TupleVecLeaf<Indices, Ts>::mpData = (Ts*)ppNewLeaf[Indices]...);
		
				EASTLFree(get_allocator(), mpData, internalDataSize());
				mpData = allocation.first;
				mNumCapacity = newCapacity;
				internalDataSize() = allocation.second;
			}
			else
			{
				swallow((TupleVecLeaf<Indices, Ts>::DoInsertAndFill(firstIdx, n, oldNumElements, args), 0)...);
			}
		}
		else
		{
			swallow((eastl::uninitialized_fill_ptr(TupleVecLeaf<Indices, Ts>::mpData + oldNumElements,
					                       TupleVecLeaf<Indices, Ts>::mpData + newNumElements, args), 0)...);
		}
		return begin() + firstIdx;
	}

	iterator insert(const_iterator pos, const_iterator first, const_iterator last)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(validate_iterator(pos) == isf_none))
			EASTL_FAIL_MSG("tuple_vector::insert -- invalid iterator");
		if (EASTL_UNLIKELY(!validate_iterator_pair(first, last)))
			EASTL_FAIL_MSG("tuple_vector::insert -- invalid iterator pair");
#endif
		size_type posIdx = pos - cbegin();
		size_type firstIdx = first.mIndex;
		size_type lastIdx = last.mIndex;
		size_type numToInsert = last - first;
		size_type oldNumElements = mNumElements;
		size_type newNumElements = oldNumElements + numToInsert;
		size_type oldNumCapacity = mNumCapacity;
		mNumElements = newNumElements;
		const void* ppOtherData[sizeof...(Ts)] = {first.mpData[Indices]...};
		if (newNumElements > oldNumCapacity || posIdx != oldNumElements)
		{
			if (newNumElements > oldNumCapacity)
			{
				const size_type newCapacity = max(GetNewCapacity(oldNumCapacity), newNumElements);

				void* ppNewLeaf[sizeof...(Ts)];
				pair<void*, size_type> allocation = TupleRecurser<Ts...>::template DoAllocate<allocator_type, 0, index_sequence_type, Ts...>(
						*this, ppNewLeaf, newCapacity, 0);

				swallow((TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(
					0, posIdx, (Ts*)ppNewLeaf[Indices]), 0)...);
				swallow((TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(
					posIdx, oldNumElements, (Ts*)ppNewLeaf[Indices] + posIdx + numToInsert), 0)...);
				swallow((eastl::uninitialized_copy_ptr((Ts*)(ppOtherData[Indices]) + firstIdx,
						                       (Ts*)(ppOtherData[Indices]) + lastIdx,
						                       (Ts*)ppNewLeaf[Indices] + posIdx), 0)...);
				swallow(TupleVecLeaf<Indices, Ts>::mpData = (Ts*)ppNewLeaf[Indices]...);
				
				EASTLFree(get_allocator(), mpData, internalDataSize());
				mpData = allocation.first;
				mNumCapacity = newCapacity;
				internalDataSize() = allocation.second;
			}
			else
			{
				swallow((TupleVecLeaf<Indices, Ts>::DoInsertRange(
					(Ts*)(ppOtherData[Indices]) + firstIdx, (Ts*)(ppOtherData[Indices]) + lastIdx,
					TupleVecLeaf<Indices, Ts>::mpData + posIdx, oldNumElements), 0)...);
			}
		}
		else
		{
			swallow((eastl::uninitialized_copy_ptr((Ts*)(ppOtherData[Indices]) + firstIdx,
					                       (Ts*)(ppOtherData[Indices]) + lastIdx,
					                       TupleVecLeaf<Indices, Ts>::mpData + posIdx), 0)...);
		}
		return begin() + posIdx;
	}

	iterator insert(const_iterator pos, const value_tuple* first, const value_tuple* last)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(validate_iterator(pos) == isf_none))
			EASTL_FAIL_MSG("tuple_vector::insert -- invalid iterator");
		if (EASTL_UNLIKELY(first > last || first == nullptr || last == nullptr))
			EASTL_FAIL_MSG("tuple_vector::insert -- invalid source pointers");
#endif
		size_type posIdx = pos - cbegin();
		size_type numToInsert = last - first;
		size_type oldNumElements = mNumElements;
		size_type newNumElements = oldNumElements + numToInsert;
		size_type oldNumCapacity = mNumCapacity;
		mNumElements = newNumElements;
		if (newNumElements > oldNumCapacity || posIdx != oldNumElements)
		{
			if (newNumElements > oldNumCapacity)
			{
				const size_type newCapacity = max(GetNewCapacity(oldNumCapacity), newNumElements);

				void* ppNewLeaf[sizeof...(Ts)];
				pair<void*, size_type> allocation = TupleRecurser<Ts...>::template DoAllocate<allocator_type, 0, index_sequence_type, Ts...>(
					*this, ppNewLeaf, newCapacity, 0);

				swallow((TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(
					0, posIdx, (Ts*)ppNewLeaf[Indices]), 0)...);
				swallow((TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(
					posIdx, oldNumElements, (Ts*)ppNewLeaf[Indices] + posIdx + numToInsert), 0)...);
				
				swallow(TupleVecLeaf<Indices, Ts>::mpData = (Ts*)ppNewLeaf[Indices]...);

				// Do this after mpData is updated so that we can use new iterators
				DoUninitializedCopyFromTupleArray(begin() + posIdx, begin() + posIdx + numToInsert, first);

				EASTLFree(get_allocator(), mpData, internalDataSize());
				mpData = allocation.first;
				mNumCapacity = newCapacity;
				internalDataSize() = allocation.second;
			}
			else
			{
				const size_type nExtra = oldNumElements - posIdx;
				void* ppDataEnd[sizeof...(Ts)] = { (void*)(TupleVecLeaf<Indices, Ts>::mpData + oldNumElements)... };
				void* ppDataBegin[sizeof...(Ts)] = { (void*)(TupleVecLeaf<Indices, Ts>::mpData + posIdx)... };
				if (numToInsert < nExtra) // If the inserted values are entirely within initialized memory (i.e. are before mpEnd)...
				{
					swallow((eastl::uninitialized_move_ptr((Ts*)ppDataEnd[Indices] - numToInsert,
						(Ts*)ppDataEnd[Indices], (Ts*)ppDataEnd[Indices]), 0)...);
					// We need move_backward because of potential overlap issues.
					swallow((eastl::move_backward((Ts*)ppDataBegin[Indices],
						(Ts*)ppDataEnd[Indices] - numToInsert, (Ts*)ppDataEnd[Indices]), 0)...); 
					
					DoCopyFromTupleArray(pos, pos + numToInsert, first);
				}
				else
				{
					size_type numToInitialize = numToInsert - nExtra;
					swallow((eastl::uninitialized_move_ptr((Ts*)ppDataBegin[Indices],
						(Ts*)ppDataEnd[Indices], (Ts*)ppDataEnd[Indices] + numToInitialize), 0)...);
					
					DoCopyFromTupleArray(pos, begin() + oldNumElements, first);
					DoUninitializedCopyFromTupleArray(begin() + oldNumElements, pos + numToInsert, first + nExtra);
				}
			}
		}
		else
		{
			DoUninitializedCopyFromTupleArray(pos, pos + numToInsert, first);
		}
		return begin() + posIdx;
	}

	iterator erase(const_iterator first, const_iterator last)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(validate_iterator(first) == isf_none || validate_iterator(last) == isf_none))
			EASTL_FAIL_MSG("tuple_vector::erase -- invalid iterator");
		if (EASTL_UNLIKELY(!validate_iterator_pair(first, last)))
			EASTL_FAIL_MSG("tuple_vector::erase -- invalid iterator pair");
#endif
		if (first != last)
		{
			size_type firstIdx = first - cbegin();
			size_type lastIdx = last - cbegin();
			size_type oldNumElements = mNumElements;
			size_type newNumElements = oldNumElements - (lastIdx - firstIdx);
			mNumElements = newNumElements;
			swallow((eastl::move(TupleVecLeaf<Indices, Ts>::mpData + lastIdx,
					       TupleVecLeaf<Indices, Ts>::mpData + oldNumElements,
					       TupleVecLeaf<Indices, Ts>::mpData + firstIdx), 0)...);
			swallow((eastl::destruct(TupleVecLeaf<Indices, Ts>::mpData + newNumElements,
					           TupleVecLeaf<Indices, Ts>::mpData + oldNumElements), 0)...);
		}
		return begin() + first.mIndex;
	}
	
	iterator erase_unsorted(const_iterator pos)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(validate_iterator(pos) == isf_none))
			EASTL_FAIL_MSG("tuple_vector::erase_unsorted -- invalid iterator");
#endif
		size_type oldNumElements = mNumElements;
		size_type newNumElements = oldNumElements - 1;
		mNumElements = newNumElements;
		swallow((eastl::move(TupleVecLeaf<Indices, Ts>::mpData + newNumElements,
				       TupleVecLeaf<Indices, Ts>::mpData + oldNumElements,
				       TupleVecLeaf<Indices, Ts>::mpData + (pos - begin())), 0)...);
		swallow((eastl::destruct(TupleVecLeaf<Indices, Ts>::mpData + newNumElements,
				           TupleVecLeaf<Indices, Ts>::mpData + oldNumElements), 0)...);
		return begin() + pos.mIndex;
	}

	void resize(size_type n)
	{
		size_type oldNumElements = mNumElements;
		size_type oldNumCapacity = mNumCapacity;
		mNumElements = n;
		if (n > oldNumElements)
		{
			if (n > oldNumCapacity)
			{
				DoReallocate(oldNumElements, eastl::max<size_type>(GetNewCapacity(oldNumCapacity), n));
			}
			swallow((eastl::uninitialized_default_fill_n(TupleVecLeaf<Indices, Ts>::mpData + oldNumElements, n - oldNumElements), 0)...);
		}
		else
		{
			swallow((eastl::destruct(TupleVecLeaf<Indices, Ts>::mpData + n,
					           TupleVecLeaf<Indices, Ts>::mpData + oldNumElements), 0)...);
		}
	}

	void resize(size_type n, const Ts&... args)
	{
		size_type oldNumElements = mNumElements;
		size_type oldNumCapacity = mNumCapacity;
		mNumElements = n;
		if (n > oldNumElements)
		{
			if (n > oldNumCapacity)
			{
				DoReallocate(oldNumElements, eastl::max<size_type>(GetNewCapacity(oldNumCapacity), n));
			} 
			swallow((eastl::uninitialized_fill_ptr(TupleVecLeaf<Indices, Ts>::mpData + oldNumElements,
					                       TupleVecLeaf<Indices, Ts>::mpData + n, args), 0)...);
		}
		else
		{
			swallow((eastl::destruct(TupleVecLeaf<Indices, Ts>::mpData + n,
					           TupleVecLeaf<Indices, Ts>::mpData + oldNumElements), 0)...);
		}
	}

	void reserve(size_type n)
	{
		DoConditionalReallocate(mNumElements, mNumCapacity, n);
	}

	void shrink_to_fit()
	{
		this_type temp(move_iterator<iterator>(begin()), move_iterator<iterator>(end()), get_allocator());
		swap(temp);
	}

	void clear() EA_NOEXCEPT
	{
		size_type oldNumElements = mNumElements;
		mNumElements = 0;
		swallow((eastl::destruct(TupleVecLeaf<Indices, Ts>::mpData, TupleVecLeaf<Indices, Ts>::mpData + oldNumElements), 0)...);
	}

	void pop_back()
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(mNumElements <= 0))
			EASTL_FAIL_MSG("tuple_vector::pop_back -- container is empty");
#endif
		size_type oldNumElements = mNumElements--;
		swallow((eastl::destruct(TupleVecLeaf<Indices, Ts>::mpData + oldNumElements - 1,
				           TupleVecLeaf<Indices, Ts>::mpData + oldNumElements), 0)...);
	}

	void swap(this_type& x)
	{
		swallow((eastl::swap(TupleVecLeaf<Indices, Ts>::mpData, x.TupleVecLeaf<Indices, Ts>::mpData), 0)...);
		eastl::swap(mpData, x.mpData);
		eastl::swap(mNumElements, x.mNumElements);
		eastl::swap(mNumCapacity, x.mNumCapacity);
		eastl::swap(get_allocator(), x.get_allocator());
		eastl::swap(internalDataSize(), x.internalDataSize());
	}

	void assign(size_type n, const_reference_tuple tup) { assign(n, eastl::get<Indices>(tup)...); }
	void assign(std::initializer_list<value_tuple> iList) { assign(iList.begin(), iList.end()); }

	void push_back(Ts&&... args) { emplace_back(eastl::forward<Ts>(args)...); }
	void push_back(const_reference_tuple tup) { push_back(eastl::get<Indices>(tup)...); }
	void push_back(rvalue_tuple tup) { emplace_back(eastl::forward<Ts>(eastl::get<Indices>(tup))...); }

	void emplace_back(rvalue_tuple tup) { emplace_back(eastl::forward<Ts>(eastl::get<Indices>(tup))...); }
	void emplace(const_iterator pos, rvalue_tuple tup) { emplace(pos, eastl::forward<Ts>(eastl::get<Indices>(tup))...); }

	iterator insert(const_iterator pos, const Ts&... args) { return insert(pos, 1, args...); }
	iterator insert(const_iterator pos, Ts&&... args) { return emplace(pos, eastl::forward<Ts>(args)...); }
	iterator insert(const_iterator pos, rvalue_tuple tup) { return emplace(pos, eastl::forward<Ts>(eastl::get<Indices>(tup))...); }
	iterator insert(const_iterator pos, const_reference_tuple tup) { return insert(pos, eastl::get<Indices>(tup)...); }
	iterator insert(const_iterator pos, size_type n, const_reference_tuple tup) { return insert(pos, n, eastl::get<Indices>(tup)...); }
	iterator insert(const_iterator pos, std::initializer_list<value_tuple> iList) { return insert(pos, iList.begin(), iList.end()); }

	iterator erase(const_iterator pos) { return erase(pos, pos + 1); }
	reverse_iterator erase(const_reverse_iterator pos) { return reverse_iterator(erase((pos + 1).base(), (pos).base())); }
	reverse_iterator erase(const_reverse_iterator first, const_reverse_iterator last) { return reverse_iterator(erase((last).base(), (first).base())); }
	reverse_iterator erase_unsorted(const_reverse_iterator pos) { return reverse_iterator(erase_unsorted((pos + 1).base())); }

	void resize(size_type n, const_reference_tuple tup) { resize(n, eastl::get<Indices>(tup)...); }

	bool empty() const EA_NOEXCEPT { return mNumElements == 0; }
	size_type size() const EA_NOEXCEPT { return mNumElements; }
	size_type capacity() const EA_NOEXCEPT { return mNumCapacity; }

	iterator begin() EA_NOEXCEPT { return iterator(this, 0); }
	const_iterator begin() const EA_NOEXCEPT { return const_iterator((const_this_type*)(this), 0); }
	const_iterator cbegin() const EA_NOEXCEPT { return const_iterator((const_this_type*)(this), 0); }

	iterator end() EA_NOEXCEPT { return iterator(this, size()); }
	const_iterator end() const EA_NOEXCEPT { return const_iterator((const_this_type*)(this), size()); }
	const_iterator cend() const EA_NOEXCEPT { return const_iterator((const_this_type*)(this), size()); }

	reverse_iterator rbegin() EA_NOEXCEPT { return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const  EA_NOEXCEPT { return const_reverse_iterator(end()); }
	const_reverse_iterator crbegin() const EA_NOEXCEPT { return const_reverse_iterator(end()); }
	
	reverse_iterator rend() EA_NOEXCEPT { return reverse_iterator(begin()); }
	const_reverse_iterator rend() const EA_NOEXCEPT { return const_reverse_iterator(begin()); }
	const_reverse_iterator crend() const EA_NOEXCEPT { return const_reverse_iterator(begin()); }

	ptr_tuple data() EA_NOEXCEPT { return ptr_tuple(TupleVecLeaf<Indices, Ts>::mpData...); }
	const_ptr_tuple data() const EA_NOEXCEPT { return const_ptr_tuple(TupleVecLeaf<Indices, Ts>::mpData...); }

	reference_tuple at(size_type n) 
	{ 
#if EASTL_EXCEPTIONS_ENABLED
		if (EASTL_UNLIKELY(n >= mNumElements))
			throw std::out_of_range("tuple_vector::at -- out of range");
#elif EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(n >= mNumElements))
			EASTL_FAIL_MSG("tuple_vector::at -- out of range");
#endif
		return reference_tuple(*(TupleVecLeaf<Indices, Ts>::mpData + n)...); 
	}

	const_reference_tuple at(size_type n) const
	{
#if EASTL_EXCEPTIONS_ENABLED
		if (EASTL_UNLIKELY(n >= mNumElements))
			throw std::out_of_range("tuple_vector::at -- out of range");
#elif EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(n >= mNumElements))
			EASTL_FAIL_MSG("tuple_vector::at -- out of range");
#endif
		return const_reference_tuple(*(TupleVecLeaf<Indices, Ts>::mpData + n)...); 
	}
	
	reference_tuple operator[](size_type n) { return at(n); }
	const_reference_tuple operator[](size_type n) const { return at(n); }
	
	reference_tuple front() 
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY(mNumElements == 0)) // We don't allow the user to reference an empty container.
				EASTL_FAIL_MSG("tuple_vector::front -- empty vector");
		#else
			// We allow the user to reference an empty container.
		#endif

		return at(0); 
	}

	const_reference_tuple front() const
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY(mNumElements == 0)) // We don't allow the user to reference an empty container.
				EASTL_FAIL_MSG("tuple_vector::front -- empty vector");
		#else
			// We allow the user to reference an empty container.
		#endif

		return at(0); 
	}
	
	reference_tuple back() 
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY(mNumElements == 0)) // We don't allow the user to reference an empty container.
				EASTL_FAIL_MSG("tuple_vector::back -- empty vector");
		#else
			// We allow the user to reference an empty container.
		#endif

		return at(size() - 1); 
	}

	const_reference_tuple back() const 
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY(mNumElements == 0)) // We don't allow the user to reference an empty container.
				EASTL_FAIL_MSG("tuple_vector::back -- empty vector");
		#else
			// We allow the user to reference an empty container.
		#endif

		return at(size() - 1); 
	}

	template <size_type I>
	tuplevec_element_t<I, Ts...>* get() 
	{
		typedef tuplevec_element_t<I, Ts...> Element;
		return TupleVecLeaf<I, Element>::mpData;
	}
	template <size_type I>
	const tuplevec_element_t<I, Ts...>* get() const
	{
		typedef tuplevec_element_t<I, Ts...> Element;
		return TupleVecLeaf<I, Element>::mpData;
	}

	template <typename T>
	T* get() 
	{ 
		typedef tuplevec_index<T, TupleTypes<Ts...>> Index;
		return TupleVecLeaf<Index::index, T>::mpData;
	}
	template <typename T>
	const T* get() const
	{
		typedef tuplevec_index<T, TupleTypes<Ts...>> Index;
		return TupleVecLeaf<Index::index, T>::mpData;
	}

	this_type& operator=(const this_type& other)
	{
		if (this != &other)
		{
			clear();
			assign(other.begin(), other.end());
		}
		return *this;
	}

	this_type& operator=(this_type&& other)
	{
		if (this != &other)
		{
			swap(other);
		}
		return *this;
	}

	this_type& operator=(std::initializer_list<value_tuple> iList) 
	{
		assign(iList.begin(), iList.end());
		return *this; 
	}

	bool validate() const EA_NOEXCEPT
	{
		if (mNumElements > mNumCapacity)
			return false;
		if (!(variadicAnd(mpData <= TupleVecLeaf<Indices, Ts>::mpData...)))
			return false;
		void* pDataEnd = (void*)((uintptr_t)mpData + internalDataSize());
		if (!(variadicAnd(pDataEnd >= TupleVecLeaf<Indices, Ts>::mpData...)))
			return false;
		return true;
	}

	int validate_iterator(const_iterator iter) const EA_NOEXCEPT
	{
		if (!(variadicAnd(iter.mpData[Indices] == TupleVecLeaf<Indices, Ts>::mpData...)))
			return isf_none;
		if (iter.mIndex < mNumElements)
			return (isf_valid | isf_current | isf_can_dereference);
		if (iter.mIndex <= mNumElements)
			return (isf_valid | isf_current);
		return isf_none;
	}

	static bool validate_iterator_pair(const_iterator first, const_iterator last) EA_NOEXCEPT
	{
		return (first.mIndex <= last.mIndex) && variadicAnd(first.mpData[Indices] == last.mpData[Indices]...);
	}

	template <typename Iterator, typename = typename enable_if<is_iterator_wrapper<Iterator>::value, bool>::type>
	int validate_iterator(Iterator iter) const EA_NOEXCEPT { return validate_iterator(unwrap_iterator(iter)); }

	template <typename Iterator, typename = typename enable_if<is_iterator_wrapper<Iterator>::value, bool>::type>
	static bool validate_iterator_pair(Iterator first, Iterator last) EA_NOEXCEPT { return validate_iterator_pair(unwrap_iterator(first), unwrap_iterator(last)); }

	allocator_type& get_allocator() EA_NOEXCEPT { return mDataSizeAndAllocator.second(); }
	const allocator_type& get_allocator() const EA_NOEXCEPT { return mDataSizeAndAllocator.second(); }

	void set_allocator(const allocator_type& alloc) { mDataSizeAndAllocator.second() = alloc; }

protected:

	void* mpData = nullptr;
	size_type mNumElements = 0;
	size_type mNumCapacity = 0;

	compressed_pair<size_type, allocator_type> mDataSizeAndAllocator;

	size_type& internalDataSize() EA_NOEXCEPT { return mDataSizeAndAllocator.first(); }
	size_type const& internalDataSize() const EA_NOEXCEPT { return mDataSizeAndAllocator.first(); }

	friend struct TupleRecurser<>;
	template<typename... Us>
	friend struct TupleRecurser;

	template <typename MoveIterBase>
	void DoInitFromIterator(move_iterator<MoveIterBase> begin, move_iterator<MoveIterBase> end)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(!validate_iterator_pair(begin, end)))
			EASTL_FAIL_MSG("tuple_vector::erase -- invalid iterator pair");
#endif
		size_type newNumElements = (size_type)(end - begin);
		const void* ppOtherData[sizeof...(Ts)] = { begin.base().mpData[Indices]... };
		size_type beginIdx = begin.base().mIndex;
		size_type endIdx = end.base().mIndex;
		DoConditionalReallocate(0, mNumCapacity, newNumElements);
		mNumElements = newNumElements;
		swallow((eastl::uninitialized_move_ptr(eastl::move_iterator<Ts*>((Ts*)(ppOtherData[Indices]) + beginIdx),
				                       eastl::move_iterator<Ts*>((Ts*)(ppOtherData[Indices]) + endIdx),
				                       TupleVecLeaf<Indices, Ts>::mpData), 0)...);
	}

	void DoInitFromIterator(const_iterator begin, const_iterator end)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(!validate_iterator_pair(begin, end)))
			EASTL_FAIL_MSG("tuple_vector::erase -- invalid iterator pair");
#endif
		size_type newNumElements = (size_type)(end - begin);
		const void* ppOtherData[sizeof...(Ts)] = { begin.mpData[Indices]... };
		size_type beginIdx = begin.mIndex;
		size_type endIdx = end.mIndex;
		DoConditionalReallocate(0, mNumCapacity, newNumElements);
		mNumElements = newNumElements;
		swallow((eastl::uninitialized_copy_ptr((Ts*)(ppOtherData[Indices]) + beginIdx,
				                       (Ts*)(ppOtherData[Indices]) + endIdx,
				                       TupleVecLeaf<Indices, Ts>::mpData), 0)...);
	}

	void DoInitFillTuple(size_type n, const_reference_tuple tup) { DoInitFillArgs(n, eastl::get<Indices>(tup)...); }

	void DoInitFillArgs(size_type n, const Ts&... args)
	{
		DoConditionalReallocate(0, mNumCapacity, n);
		mNumElements = n;
		swallow((eastl::uninitialized_fill_ptr(TupleVecLeaf<Indices, Ts>::mpData, TupleVecLeaf<Indices, Ts>::mpData + n, args), 0)...);
	}

	void DoInitDefaultFill(size_type n)
	{
		DoConditionalReallocate(0, mNumCapacity, n);
		mNumElements = n;
		swallow((eastl::uninitialized_default_fill_n(TupleVecLeaf<Indices, Ts>::mpData, n), 0)...);
	}

	void DoInitFromTupleArray(const value_tuple* first, const value_tuple* last)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(first > last || first == nullptr || last == nullptr))
			EASTL_FAIL_MSG("tuple_vector::ctor from tuple array -- invalid ptrs");
#endif
		size_type newNumElements = last - first;
		DoConditionalReallocate(0, mNumCapacity, newNumElements);
		mNumElements = newNumElements;
		DoUninitializedCopyFromTupleArray(begin(), end(), first);
	}

	void DoCopyFromTupleArray(iterator destPos, iterator destEnd, const value_tuple* srcTuple)
	{
		// assign to constructed region
		while (destPos < destEnd)
		{
			*destPos = *srcTuple;
			++destPos;
			++srcTuple;
		}
	}

	void DoUninitializedCopyFromTupleArray(iterator destPos, iterator destEnd, const value_tuple* srcTuple)
	{
		// placement-new/copy-ctor to unconstructed regions
		while (destPos < destEnd)
		{
			swallow(::new(eastl::get<Indices>(destPos.MakePointer())) Ts(eastl::get<Indices>(*srcTuple))...);
			++destPos;
			++srcTuple;
		}
	}

	// Try to grow the size of the container "naturally" given the number of elements being used
	void DoGrow(size_type oldNumElements, size_type oldNumCapacity, size_type requiredCapacity)
	{
		if (requiredCapacity > oldNumCapacity)
			DoReallocate(oldNumElements, GetNewCapacity(requiredCapacity));
	}

	// Reallocate to the newCapacity (IFF it's actually larger, though)
	void DoConditionalReallocate(size_type oldNumElements, size_type oldNumCapacity, size_type requiredCapacity)
	{
		if (requiredCapacity > oldNumCapacity)
			DoReallocate(oldNumElements, requiredCapacity);
	}

	void DoReallocate(size_type oldNumElements, size_type requiredCapacity)
	{
		void* ppNewLeaf[sizeof...(Ts)];
		pair<void*, size_type> allocation = TupleRecurser<Ts...>::template DoAllocate<allocator_type, 0, index_sequence_type, Ts...>(
			*this, ppNewLeaf, requiredCapacity, 0);
		swallow((TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(0, oldNumElements, (Ts*)ppNewLeaf[Indices]), 0)...);
		swallow(TupleVecLeaf<Indices, Ts>::mpData = (Ts*)ppNewLeaf[Indices]...);

		EASTLFree(get_allocator(), mpData, internalDataSize());
		mpData = allocation.first;
		mNumCapacity = requiredCapacity;
		internalDataSize() = allocation.second;
	}

	size_type GetNewCapacity(size_type oldNumCapacity)
	{
		return (oldNumCapacity > 0) ? (2 * oldNumCapacity) : 1;
	}
};

}  // namespace TupleVecInternal

// Move_iterator specialization for TupleVecIter.
// An rvalue reference of a move_iterator would normaly be "tuple<Ts...> &&" whereas
// what we actually want is "tuple<Ts&&...>". This specialization gives us that.
template <eastl_size_t... Indices, typename... Ts>
class move_iterator<TupleVecInternal::TupleVecIter<index_sequence<Indices...>, Ts...>>
{
public:
	typedef TupleVecInternal::TupleVecIter<index_sequence<Indices...>, Ts...> iterator_type;
	typedef iterator_type wrapped_iterator_type; // This is not in the C++ Standard; it's used by use to identify it as
												 // a wrapping iterator type.
	typedef iterator_traits<iterator_type> traits_type;
	typedef typename traits_type::iterator_category iterator_category;
	typedef typename traits_type::value_type value_type;
	typedef typename traits_type::difference_type difference_type;
	typedef typename traits_type::pointer pointer;
	typedef tuple<Ts&&...> reference;
	typedef move_iterator<iterator_type> this_type;

protected:
	iterator_type mIterator;

public:
	move_iterator() : mIterator() {}
	explicit move_iterator(iterator_type mi) : mIterator(mi) {}

	template <typename U>
	move_iterator(const move_iterator<U>& mi) : mIterator(mi.base()) {}

	iterator_type base() const { return mIterator; }
	reference operator*() const { return eastl::move(MakeReference()); }
	pointer operator->() const { return mIterator; }

	this_type& operator++() { ++mIterator; return *this; }
	this_type operator++(int) {
		this_type tempMoveIterator = *this;
		++mIterator;
		return tempMoveIterator;
	}

	this_type& operator--() { --mIterator; return *this; }
	this_type operator--(int)
	{
		this_type tempMoveIterator = *this;
		--mIterator;
		return tempMoveIterator;
	}

	this_type operator+(difference_type n) const { return move_iterator(mIterator + n); }
	this_type& operator+=(difference_type n)
	{
		mIterator += n;
		return *this;
	}

	this_type operator-(difference_type n) const { return move_iterator(mIterator - n); }
	this_type& operator-=(difference_type n)
	{
		mIterator -= n;
		return *this;
	}

	difference_type operator-(const this_type& rhs) const { return mIterator - rhs.mIterator; }
	bool operator<(const this_type& rhs) const { return mIterator < rhs.mIterator; }
	bool operator>(const this_type& rhs) const { return mIterator > rhs.mIterator; }
	bool operator>=(const this_type& rhs) const { return mIterator >= rhs.mIterator; }
	bool operator<=(const this_type& rhs) const { return mIterator <= rhs.mIterator; }

	reference operator[](difference_type n) const { return *(*this + n); }

private:
	reference MakeReference() const
	{
		return reference(eastl::move(((Ts*)mIterator.mpData[Indices])[mIterator.mIndex])...);
	}
};

template <typename AllocatorA, typename AllocatorB, typename Indices, typename... Ts>
inline bool operator==(const TupleVecInternal::TupleVecImpl<AllocatorA, Indices, Ts...>& a,
					   const TupleVecInternal::TupleVecImpl<AllocatorB, Indices, Ts...>& b)
{
	return ((a.size() == b.size()) && eastl::equal(a.begin(), a.end(), b.begin()));
}

template <typename AllocatorA, typename AllocatorB, typename Indices, typename... Ts>
inline bool operator!=(const TupleVecInternal::TupleVecImpl<AllocatorA, Indices, Ts...>& a,
					   const TupleVecInternal::TupleVecImpl<AllocatorB, Indices, Ts...>& b)
{
	return ((a.size() != b.size()) || !eastl::equal(a.begin(), a.end(), b.begin()));
}

template <typename AllocatorA, typename AllocatorB, typename Indices, typename... Ts>
inline bool operator<(const TupleVecInternal::TupleVecImpl<AllocatorA, Indices, Ts...>& a,
					  const TupleVecInternal::TupleVecImpl<AllocatorB, Indices, Ts...>& b)
{
	return eastl::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template <typename AllocatorA, typename AllocatorB, typename Indices, typename... Ts>
inline bool operator>(const TupleVecInternal::TupleVecImpl<AllocatorA, Indices, Ts...>& a,
					  const TupleVecInternal::TupleVecImpl<AllocatorB, Indices, Ts...>& b)
{
	return b < a;
}

template <typename AllocatorA, typename AllocatorB, typename Indices, typename... Ts>
inline bool operator<=(const TupleVecInternal::TupleVecImpl<AllocatorA, Indices, Ts...>& a,
					   const TupleVecInternal::TupleVecImpl<AllocatorB, Indices, Ts...>& b)
{
	return !(b < a);
}

template <typename AllocatorA, typename AllocatorB, typename Indices, typename... Ts>
inline bool operator>=(const TupleVecInternal::TupleVecImpl<AllocatorA, Indices, Ts...>& a,
					   const TupleVecInternal::TupleVecImpl<AllocatorB, Indices, Ts...>& b)
{
	return !(a < b);
}

template <typename AllocatorA, typename AllocatorB, typename Indices, typename... Ts>
inline void swap(TupleVecInternal::TupleVecImpl<AllocatorA, Indices, Ts...>& a,
				TupleVecInternal::TupleVecImpl<AllocatorB, Indices, Ts...>& b)
{
	a.swap(b);
}

// A customization of swap is made for r-values of tuples-of-references - 
// normally, swapping rvalues doesn't make sense, but in this case, we do want to 
// swap the contents of what the tuple-of-references are referring to
//
// This is required due to TupleVecIter returning a value-type for its dereferencing,
// as opposed to an actual real reference of some sort
template<typename... Ts>
inline
typename enable_if<conjunction<is_swappable<Ts>...>::value>::type
swap(tuple<Ts&...>&& a, tuple<Ts&...>&& b)
{
	a.swap(b);
}

template<typename... Ts>
inline
typename enable_if<!conjunction<is_swappable<Ts>...>::value>::type
swap(tuple<Ts&...>&& a, tuple<Ts&...>&& b) = delete;


// External interface of tuple_vector
template <typename... Ts>
class tuple_vector : public TupleVecInternal::TupleVecImpl<EASTLAllocatorType, make_index_sequence<sizeof...(Ts)>, Ts...>
{
	typedef tuple_vector<Ts...> this_type;
	typedef TupleVecInternal::TupleVecImpl<EASTLAllocatorType, make_index_sequence<sizeof...(Ts)>, Ts...> base_type;
	using base_type::base_type;

public:
	this_type& operator=(std::initializer_list<typename base_type::value_tuple> iList) 
	{
		base_type::operator=(iList);
		return *this;
	}
};

// Variant of tuple_vector that allows a user-defined allocator type (can't mix default template params with variadics)
template <typename AllocatorType, typename... Ts>
class tuple_vector_alloc
	: public TupleVecInternal::TupleVecImpl<AllocatorType, make_index_sequence<sizeof...(Ts)>, Ts...>
{
	typedef tuple_vector_alloc<AllocatorType, Ts...> this_type;
	typedef TupleVecInternal::TupleVecImpl<AllocatorType, make_index_sequence<sizeof...(Ts)>, Ts...> base_type;
	using base_type::base_type;

public:

	this_type& operator=(std::initializer_list<typename base_type::value_tuple> iList)
	{
		base_type::operator=(iList);
		return *this;
	}
};

}  // namespace eastl

EA_RESTORE_VC_WARNING()
EA_RESTORE_VC_WARNING()
EA_RESTORE_VC_WARNING()
EA_RESTORE_VC_WARNING()

#endif  // EASTL_TUPLEVECTOR_H
