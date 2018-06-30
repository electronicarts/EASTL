///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_TUPLEVECTOR_H
#define EASTL_TUPLEVECTOR_H

#include <EASTL/internal/config.h>
#include <EASTL/iterator.h>
#include <EASTL/memory.h>
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

namespace TupleVecInternal
{

// forward declarations
template <size_t I, typename... Ts>
struct tuplevec_element;

template <size_t I, typename... Ts>
using tuplevec_element_t = typename tuplevec_element<I, Ts...>::type;

template <typename... Ts>
struct TupleTypes {};

template <typename Allocator, typename Indices, typename... Ts>
class TupleVecImpl;

template <typename... Ts>
struct TupleRecurser;

template <size_t I, typename... Ts>
struct TupleIndexRecurser;

template <typename Indices, typename... Ts>
struct TupleVecIter;

// tuplevec_element helper to be able to isolate a type given an index
template <size_t I>
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

template <size_t I, typename T, typename... Ts>
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
		
	static constexpr size_type GetTotalAlignment()
	{
		return 0;
	}

	static constexpr size_type GetTotalAllocationSize(size_type capacity, size_type offset)
	{
		return offset;
	}

	template<typename Allocator, size_type I, typename Indices, typename... VecTypes>
	static pair<void*, size_type> DoAllocate(TupleVecImpl<Allocator, Indices, VecTypes...> &vec, void** ppNewLeaf, size_type capacity, size_type offset)
	{
		// If n is zero, then we allocate no memory and just return NULL. 
		// This is fine, as our default ctor initializes with NULL pointers. 
		size_type alignment = TupleRecurser<VecTypes...>::GetTotalAlignment();
		void* ptr = capacity ? allocate_memory(vec.mAllocator, offset, alignment, 0) : nullptr;
		return make_pair(ptr, offset);
	}

	template<typename TupleVecImplType, size_type I>
	static void SetNewData(TupleVecImplType &vec, void* pData, size_type capacity, size_type offset) 
	{ }
};

template <typename T, typename... Ts>
struct TupleRecurser<T, Ts...> : TupleRecurser<Ts...>
{
	typedef eastl_size_t size_type;
	
	static constexpr size_type GetTotalAlignment()
	{
		return max(alignof(T), TupleRecurser<Ts...>::GetTotalAlignment());
	}

	static constexpr size_type GetTotalAllocationSize(size_type capacity, size_type offset)
	{
		pair<size_type, size_type> offsetRange = CalculateAllocationOffsetRange(offset, capacity);
		return TupleRecurser<Ts...>::GetTotalAllocationSize(capacity, offsetRange.second);
	}

	template<typename Allocator, size_type I, typename Indices, typename... VecTypes>
	static pair<void*, size_type> DoAllocate(TupleVecImpl<Allocator, Indices, VecTypes...> &vec, void** ppNewLeaf, size_type capacity, size_type offset)
	{
		pair<size_type, size_type> offsetRange = CalculateAllocationOffsetRange(offset, capacity);
		pair<void*, size_type> allocation = TupleRecurser<Ts...>::DoAllocate<Allocator, I+1, Indices, VecTypes...>(vec, ppNewLeaf, capacity, offsetRange.second);
		ppNewLeaf[I] = (void*)((uintptr_t)(allocation.first) + offsetRange.first);
		return allocation;
	}

	template<typename TupleVecImplType, size_type I>
	static void SetNewData(TupleVecImplType &vec, void* pData, size_type capacity, size_type offset)
	{
		pair<size_type, size_type> offsetRange = CalculateAllocationOffsetRange(offset, capacity);
		vec.TupleVecLeaf<I, T>::mpData = (T*)((uintptr_t)pData + offsetRange.first);
		TupleRecurser<Ts...>::SetNewData<TupleVecImplType, I + 1>(vec, pData, capacity, offsetRange.second);
	}

private:
	static constexpr pair<size_type, size_type> CalculateAllocationOffsetRange(size_type offset, size_type capacity)
	{
		size_type alignment = alignof(T);
		size_type offsetBegin = (offset + alignment - 1) & (~alignment + 1);
		size_type offsetEnd = offsetBegin + sizeof(T) * capacity;
		return pair<size_type, size_type>(offsetBegin, offsetEnd);
	}
};

template <size_t I, typename T>
struct TupleVecLeaf
{
	typedef eastl_size_t size_type;

	// functions that get piped through swallow need to return some kind of value, hence why these are not void
	int DoUninitializedMoveAndDestruct(const size_type begin, const size_type end, T* pDest)
	{
		T* pBegin = mpData + begin;
		T* pEnd = mpData + end;
		eastl::uninitialized_move_ptr_if_noexcept(pBegin, pEnd, pDest);
		eastl::destruct(pBegin, pEnd);
		return 0;
	}

	int DoInsertAndFill(size_type pos, size_type n, size_type numElements, const T& arg)
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
		return 0;
	}

	int DoInsertRange(T* pSrcBegin, T* pSrcEnd, T* pDestBegin, size_type numDataElements)
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
			eastl::copy(pSrcBegin, pSrcEnd, pDestBegin);
		}
		return 0;
	}

	int DoInsertValue(size_type pos, size_type numElements, T&& arg)
	{
		T* pDest = mpData + pos;
		T* pDataEnd = mpData + numElements;

		eastl::uninitialized_move_ptr(pDataEnd - 1, pDataEnd, pDataEnd);
		eastl::move_backward(pDest, pDataEnd - 1, pDataEnd); // We need move_backward because of potential overlap issues.
		eastl::destruct(pDest);
		::new (pDest) T(eastl::move(arg));

		return 0;
	}

	T* mpData = nullptr;
};

// specializations of various memory/utility functions w/ return 0 to allow for compatibility with swallow(...)
template <typename T>
inline int DoConstruction(T* ptr) { ::new (ptr) T(); return 0; }

template <typename T>
inline int DoConstruction(T* ptr, const T& arg) { ::new (ptr) T(arg); return 0; }

template <typename T>
inline int DoConstruction(T* ptr, T&& arg) { ::new (ptr) T(eastl::move(arg)); return 0; }

template <typename InputIterator, typename OutputIterator>
inline int DoCopy(InputIterator first, InputIterator last, OutputIterator result) { eastl::copy(first, last, result); return 0; }

template <typename ForwardIterator>
inline int DoDestruct(ForwardIterator first, ForwardIterator last) { eastl::destruct(first, last); return 0; }

template <typename ForwardIterator, typename T>
inline int DoFill(ForwardIterator first, ForwardIterator last, const T& value) { eastl::fill(first, last, value); return 0; }

template <typename InputIterator, typename OutputIterator>
inline int DoMove(InputIterator first, InputIterator last, OutputIterator result) { eastl::move(first, last, result); return 0; }

template <typename T>
inline int DoSwap(T& a, T& b) { eastl::swap(a, b); return 0; }

template <typename First, typename Last, typename Result>
inline int DoUninitializedCopyPtr(First first, Last last, Result result) { eastl::uninitialized_copy_ptr(first, last, result); return 0; }

template <typename ForwardIterator, typename Count>
inline int DoUninitializedDefaultFillN(ForwardIterator first, Count n) { eastl::uninitialized_default_fill_n(first, n); return 0; }

template <typename T>
inline int DoUninitializedFillPtr(T* first, T* last, const T& value) { eastl::uninitialized_fill_ptr(first, last, value); return 0; }

// swallow allows for parameter pack expansion of arguments as means of expanding operations performed
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
template <size_t... Indices, typename... Ts>
struct TupleVecIter<integer_sequence<size_t, Indices...>, Ts...> : public iterator<random_access_iterator_tag, tuple<Ts...>, eastl_size_t, tuple<Ts*...>, tuple<Ts&...>>
{
private:
	typedef TupleVecIter<integer_sequence<size_t, Indices...>, Ts...> this_type;
	typedef eastl_size_t size_type;

	template<typename U, typename... Us> 
	friend struct TupleVecIter;

	template<typename U, typename V, typename... Ts>
	friend class TupleVecImpl;

	template<typename U>
	friend class move_iterator;
public:
	TupleVecIter() = default;

	template<typename VecImplType>
	TupleVecIter(VecImplType* tupleVec, size_type index)
		: mIndex(index)
		, mpData{(void*)tupleVec->TupleVecLeaf<Indices, Ts>::mpData...}
	{ }

	template <typename OtherIndicesType,
			  typename... Us,
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

// Move_iterator specialization for TupleVecIter.
// An rvalue reference of a move_iterator would normaly be "tuple<Ts...> &&" whereas
// what we actually want is "tuple<Ts&&...>". This specialization gives us that.
template <size_t... Indices, typename... Ts>
class move_iterator<TupleVecIter<integer_sequence<size_t, Indices...>, Ts...>>
{
public:
	typedef TupleVecInternal::TupleVecIter<integer_sequence<size_t, Indices...>, Ts...> iterator_type;
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

	reference operator[](difference_type n) const { return *(*this + n); }

private:
	reference MakeReference() const 
	{
		return reference(eastl::move(((Ts*)mIterator.mpData[Indices])[mIterator.mIndex])...);
	}
};

// TupleVecImpl
template <typename Allocator, size_t... Indices, typename... Ts>
class TupleVecImpl<Allocator, integer_sequence<size_t, Indices...>, Ts...> : public TupleVecLeaf<Indices, Ts>...
{
	typedef Allocator	allocator_type;
	typedef integer_sequence<size_t, Indices...> index_sequence_type;
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
		: mAllocator(EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
	{}

	TupleVecImpl(const allocator_type& allocator)
		: mAllocator(allocator)
	{}

	TupleVecImpl(this_type&& x)
		: mAllocator(eastl::move(x.mAllocator))
	{
		swap(x);
	}

	TupleVecImpl(this_type&& x, const Allocator& allocator)
		: mAllocator(allocator)
	{
		swap(x);
	}

	TupleVecImpl(const this_type& x)
		: mAllocator(x.mAllocator)
	{
		DoInitFromIterator(x.begin(), x.end());
	}

	TupleVecImpl(const this_type& x, const Allocator& allocator) 
		: mAllocator(allocator)
	{
		DoInitFromIterator(x.begin(), x.end());
	}

	template<typename MoveIterBase>
	TupleVecImpl(move_iterator<MoveIterBase> begin, move_iterator<MoveIterBase> end, const allocator_type& allocator = EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
		: mAllocator(allocator)
	{
		DoInitFromIterator(begin, end);
	}

	template<typename Iterator>
 	TupleVecImpl(Iterator begin, Iterator end, const allocator_type& allocator = EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
 		: mAllocator(allocator)
 	{
		DoInitFromIterator(begin, end);
 	}

	TupleVecImpl(size_type n, const allocator_type& allocator = EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
		: mAllocator(allocator)
	{
		DoInitDefaultFill(n);

	}

	TupleVecImpl(size_type n, const Ts&... args)
		: mAllocator(EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
	{
		DoInitFillArgs(n, args...);
	}

	TupleVecImpl(size_type n, const Ts&... args, const allocator_type& allocator)
		: mAllocator(allocator)
	{
		DoInitFillArgs(n, args...);
	}

	TupleVecImpl(size_type n, const_reference_tuple tup, const allocator_type& allocator = EASTL_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
		: mAllocator(allocator)
	{
		DoInitFillTuple(n, tup);
	}

protected:
	// ctor to provide a pre-allocated field of data that the container will own, specifically for fixed_tuple_vector
	TupleVecImpl(const allocator_type& allocator, void* pData, size_type capacity, size_type dataSize)
		: mAllocator(allocator), mpData(pData), mNumCapacity(capacity), mDataSize(dataSize)
	{
		TupleRecurser<Ts...>::SetNewData<this_type, 0>(*this, mpData, mNumCapacity, 0);
	}

public:
	~TupleVecImpl()
	{ 
		swallow(DoDestruct(TupleVecLeaf<Indices, Ts>::mpData, TupleVecLeaf<Indices, Ts>::mpData + mNumElements)...);
		if (mpData)
			EASTLFree(mAllocator, mpData, mDataSize); 
	}

	void assign(size_type n, const Ts&... args)
	{
		if (n > mNumCapacity)
		{
			this_type temp(n, args..., mAllocator); // We have little choice but to reallocate with new memory.
			swap(temp);
		}
		else if (n > mNumElements) // If n > mNumElements ...
		{
			size_type oldNumElements = mNumElements;
			swallow(DoFill(
				TupleVecLeaf<Indices, Ts>::mpData,
				TupleVecLeaf<Indices, Ts>::mpData + oldNumElements,
				args
			)...);
			swallow(DoUninitializedFillPtr(
				TupleVecLeaf<Indices, Ts>::mpData + oldNumElements,
				TupleVecLeaf<Indices, Ts>::mpData + n,
				args
			)...);
			mNumElements = n;
		}
		else // else 0 <= n <= mNumElements
		{
			swallow(DoFill(
				TupleVecLeaf<Indices, Ts>::mpData,
				TupleVecLeaf<Indices, Ts>::mpData + n,
				args
			)...);
			erase(begin() + n, end());
		}
	}

	void assign(const_iterator first, const_iterator last)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(!validateIteratorPair(first, last)))
			EASTL_FAIL_MSG("tuple_vector::assign -- invalid iterator pair");
#endif

		size_type newNumElements = last - first;
		if (newNumElements > mNumCapacity)
		{
			this_type temp(first, last, mAllocator);
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
				swallow(DoCopy(
						(Ts*)(ppOtherData[Indices]) + firstIdx,
						(Ts*)(ppOtherData[Indices]) + firstIdx + oldNumElements,
						TupleVecLeaf<Indices, Ts>::mpData
					)...);
				swallow(DoUninitializedCopyPtr(
						(Ts*)(ppOtherData[Indices]) + firstIdx + oldNumElements,
						(Ts*)(ppOtherData[Indices]) + lastIdx,
						TupleVecLeaf<Indices, Ts>::mpData + oldNumElements
					)...);
				mNumElements = newNumElements;
			}
			else // else 0 <= n <= mNumElements
			{
				swallow(DoCopy(
						(Ts*)(ppOtherData[Indices]) + firstIdx,
						(Ts*)(ppOtherData[Indices]) + lastIdx,
						TupleVecLeaf<Indices, Ts>::mpData
					)...);
				erase(begin() + newNumElements, end());
			}
		}
	}

	reference_tuple push_back()
	{
		size_type oldNumElements = mNumElements++;
		if (oldNumElements >= mNumCapacity)
		{
			DoReallocate(oldNumElements, GetNewCapacity(oldNumElements));
		}
		swallow(DoConstruction(TupleVecLeaf<Indices, Ts>::mpData + oldNumElements)...);
		return back();
	}

	void push_back(const Ts&... args)
	{
		size_type oldNumElements = mNumElements++;
		if (oldNumElements >= mNumCapacity)
		{
			DoReallocate(oldNumElements, GetNewCapacity(oldNumElements));
		}
		swallow(DoConstruction(TupleVecLeaf<Indices, Ts>::mpData + oldNumElements, args)...);
	}

	void push_back_uninitialized()
	{
		size_type oldNumElements = mNumElements++;
		if (oldNumElements >= mNumCapacity)
		{
			DoReallocate(oldNumElements, GetNewCapacity(oldNumElements));
		}
	}
	
	reference_tuple emplace_back(Ts&&... args)
	{
		size_type oldNumElements = mNumElements++;
		if (oldNumElements >= mNumCapacity)
		{
			DoReallocate(oldNumElements, GetNewCapacity(oldNumElements));
		}
		swallow(DoConstruction(TupleVecLeaf<Indices, Ts>::mpData + oldNumElements, eastl::move(args))...);
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
		mNumElements = newNumElements;
		if (newNumElements >= mNumCapacity || firstIdx != oldNumElements)
		{
			if (newNumElements >= mNumCapacity)
			{
				const size_type newCapacity = max(GetNewCapacity(mNumCapacity), newNumElements);

				void* ppNewLeaf[sizeof...(Ts)];
				pair<void*, size_type> allocation =
					TupleRecurser<Ts...>::DoAllocate<allocator_type, 0, index_sequence_type, Ts...>(
						*this, ppNewLeaf, newCapacity, 0);

				swallow(TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(0, firstIdx, (Ts*)ppNewLeaf[Indices])...);
				swallow(TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(firstIdx, oldNumElements, (Ts*)ppNewLeaf[Indices] + firstIdx + 1)...);
				swallow(DoConstruction((Ts*)ppNewLeaf[Indices] + firstIdx, eastl::move(args))...);
				swallow(TupleVecLeaf<Indices, Ts>::mpData = (Ts*)ppNewLeaf[Indices]...);

				EASTLFree(mAllocator, mpData, mDataSize);
				mpData = allocation.first;
				mDataSize = allocation.second;
				mNumCapacity = newCapacity;
			}
			else
			{
				swallow(TupleVecLeaf<Indices, Ts>::DoInsertValue(firstIdx, oldNumElements, eastl::move(args))...);
			}
		}
		else
		{
			swallow(DoConstruction(TupleVecLeaf<Indices, Ts>::mpData + oldNumElements, eastl::move(args))...);
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
		mNumElements = newNumElements;
		if (newNumElements >= mNumCapacity || firstIdx != oldNumElements)
		{
			if (newNumElements >= mNumCapacity)
			{
				const size_type newCapacity = max(GetNewCapacity(mNumCapacity), newNumElements);

				void* ppNewLeaf[sizeof...(Ts)];
				pair<void*, size_type> allocation =
					TupleRecurser<Ts...>::DoAllocate<allocator_type, 0, index_sequence_type, Ts...>(
						*this, ppNewLeaf, newCapacity, 0);

				swallow(TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(0, firstIdx, (Ts*)ppNewLeaf[Indices])...);
				swallow(TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(firstIdx, oldNumElements, (Ts*)ppNewLeaf[Indices] + lastIdx)...);
				swallow(DoUninitializedFillPtr((Ts*)ppNewLeaf[Indices] + firstIdx, (Ts*)ppNewLeaf[Indices] + lastIdx, args)...);
				swallow(TupleVecLeaf<Indices, Ts>::mpData = (Ts*)ppNewLeaf[Indices]...);
		
				EASTLFree(mAllocator, mpData, mDataSize);
				mpData = allocation.first;
				mDataSize = allocation.second;
				mNumCapacity = newCapacity;
			}
			else
			{
				swallow(TupleVecLeaf<Indices, Ts>::DoInsertAndFill(firstIdx, n, oldNumElements, args)...);
			}
		}
		else
		{
			swallow(DoUninitializedFillPtr(
				TupleVecLeaf<Indices, Ts>::mpData + oldNumElements,
				TupleVecLeaf<Indices, Ts>::mpData + newNumElements,
				args
			)...);
		}
		return begin() + firstIdx;
	}

	iterator insert(const_iterator pos, const_iterator first, const_iterator last)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(validate_iterator(pos) == isf_none))
			EASTL_FAIL_MSG("tuple_vector::insert -- invalid iterator");
		if (EASTL_UNLIKELY(!validateIteratorPair(first, last)))
			EASTL_FAIL_MSG("tuple_vector::insert -- invalid iterator pair");
#endif
		size_type posIdx = pos - cbegin();
		size_type firstIdx = first.mIndex;
		size_type lastIdx = last.mIndex;
		size_type numToInsert = last - first;
		size_type oldNumElements = mNumElements;
		size_type newNumElements = oldNumElements + numToInsert;
		mNumElements = newNumElements;
		const void* ppOtherData[sizeof...(Ts)] = {first.mpData[Indices]...};
		if (newNumElements >= mNumCapacity || posIdx != oldNumElements)
		{
			if (newNumElements >= mNumCapacity)
			{
 				const size_type newCapacity = max(GetNewCapacity(mNumCapacity), newNumElements);
 
 				void* ppNewLeaf[sizeof...(Ts)];
				pair<void*, size_type> allocation =
 					TupleRecurser<Ts...>::DoAllocate<allocator_type, 0, index_sequence_type, Ts...>(
 						*this, ppNewLeaf, newCapacity, 0);
 
 				swallow(TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(0, posIdx, (Ts*)ppNewLeaf[Indices])...);
 				swallow(TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(posIdx, oldNumElements, (Ts*)ppNewLeaf[Indices] + posIdx + numToInsert)...);
				swallow(DoUninitializedCopyPtr((Ts*)(ppOtherData[Indices]) + firstIdx, (Ts*)(ppOtherData[Indices]) + lastIdx, (Ts*)ppNewLeaf[Indices] + posIdx)...);
				swallow(TupleVecLeaf<Indices, Ts>::mpData = (Ts*)ppNewLeaf[Indices]...);
				
				EASTLFree(mAllocator, mpData, mDataSize);
 				mpData = allocation.first;
 				mDataSize = allocation.second;
 				mNumCapacity = newCapacity;
			}
			else
			{
				swallow(TupleVecLeaf<Indices, Ts>::DoInsertRange(
						(Ts*)(ppOtherData[Indices]) + firstIdx,
						(Ts*)(ppOtherData[Indices]) + lastIdx,
						TupleVecLeaf<Indices, Ts>::mpData + posIdx,
					oldNumElements
					)...);
			}
		}
		else
		{
			swallow(DoUninitializedCopyPtr(
					(Ts*)(ppOtherData[Indices]) + firstIdx,
					(Ts*)(ppOtherData[Indices]) + lastIdx,
					TupleVecLeaf<Indices, Ts>::mpData + posIdx
				)...);
		}
		return begin() + posIdx;
	}

	iterator erase(const_iterator first, const_iterator last)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(validate_iterator(first) == isf_none || validate_iterator(last) == isf_none))
			EASTL_FAIL_MSG("tuple_vector::erase -- invalid iterator");
		if (EASTL_UNLIKELY(!validateIteratorPair(first, last)))
			EASTL_FAIL_MSG("tuple_vector::erase -- invalid iterator pair");
#endif
		if (first != last)
		{
			size_type firstIdx = first - cbegin();
			size_type lastIdx = last - cbegin();
			size_type oldNumElements = mNumElements;
			size_type newNumElements = mNumElements - (lastIdx - firstIdx);
			mNumElements = newNumElements;
			swallow(DoMove(
				TupleVecLeaf<Indices, Ts>::mpData + lastIdx, 
				TupleVecLeaf<Indices, Ts>::mpData + oldNumElements,
				TupleVecLeaf<Indices, Ts>::mpData + firstIdx
			)...);
			swallow(DoDestruct(TupleVecLeaf<Indices, Ts>::mpData + newNumElements, TupleVecLeaf<Indices, Ts>::mpData + oldNumElements)...);
		}
		return first;
	}
	
	iterator erase_unsorted(const_iterator pos)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(validate_iterator(pos) == isf_none))
			EASTL_FAIL_MSG("tuple_vector::erase_unsorted -- invalid iterator");
#endif
		size_type oldNumElements = mNumElements;
		size_type newNumElements = mNumElements - 1;
		mNumElements = newNumElements;
		swallow(DoMove(
			TupleVecLeaf<Indices, Ts>::mpData + newNumElements,
			TupleVecLeaf<Indices, Ts>::mpData + oldNumElements,
			TupleVecLeaf<Indices, Ts>::mpData + (pos - begin())
		)...);
		swallow(DoDestruct(TupleVecLeaf<Indices, Ts>::mpData + newNumElements, TupleVecLeaf<Indices, Ts>::mpData + oldNumElements)...);
		return pos;
	}

	void resize(size_type n)
	{
		size_type oldNumElements = mNumElements;
		mNumElements = n;
		if (n > oldNumElements)
		{
			if (n > mNumCapacity)
			{
				DoReallocate(oldNumElements, eastl::max<size_type>(GetNewCapacity(oldNumElements), n));
			}
			swallow(DoUninitializedDefaultFillN(TupleVecLeaf<Indices, Ts>::mpData + oldNumElements, n - oldNumElements)...);
		}
		else
		{
			swallow(DoDestruct(TupleVecLeaf<Indices, Ts>::mpData + n, TupleVecLeaf<Indices, Ts>::mpData + oldNumElements)...);
		}
	}

	void resize(size_type n, const Ts&... args)
	{
		size_type oldNumElements = mNumElements;
		mNumElements = n;
		if (n > oldNumElements)
		{
			if (n > mNumCapacity)
			{
				DoReallocate(oldNumElements, eastl::max<size_type>(GetNewCapacity(oldNumElements), n));
			}
			swallow(DoUninitializedFillPtr(
				TupleVecLeaf<Indices, Ts>::mpData + oldNumElements,
				TupleVecLeaf<Indices, Ts>::mpData + n,
				args
			)...);
		}
		else
		{
			swallow(DoDestruct(TupleVecLeaf<Indices, Ts>::mpData + n, TupleVecLeaf<Indices, Ts>::mpData + oldNumElements)...);

		}
	}

	void reserve(size_type n)
	{
		if (n > mNumCapacity)
		{
			DoReallocate(mNumElements, n);
		}
	}

	void shrink_to_fit()
	{
		this_type temp(move_iterator<iterator>(begin()), move_iterator<iterator>(end()), mAllocator);
		swap(temp);
	}

	void clear() EA_NOEXCEPT
	{
		size_type oldNumElements = mNumElements;
		mNumElements = 0;
		swallow(DoDestruct(TupleVecLeaf<Indices, Ts>::mpData, TupleVecLeaf<Indices, Ts>::mpData + oldNumElements)...);
	}

	void pop_back()
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(mNumElements <= 0))
			EASTL_FAIL_MSG("tuple_vector::pop_back -- container is empty");
#endif
		size_type oldNumElements = mNumElements--;
		swallow(DoDestruct(TupleVecLeaf<Indices, Ts>::mpData + oldNumElements - 1, TupleVecLeaf<Indices, Ts>::mpData + oldNumElements)...);
	}

	void swap(this_type& x)
	{
		swallow(DoSwap(TupleVecLeaf<Indices, Ts>::mpData, x.TupleVecLeaf<Indices, Ts>::mpData)...);
		eastl::swap(mAllocator, x.mAllocator);
		eastl::swap(mpData, x.mpData);
		eastl::swap(mDataSize, x.mDataSize);
		eastl::swap(mNumElements, x.mNumElements);
		eastl::swap(mNumCapacity, x.mNumCapacity);
	}

	void assign(size_type n, const_reference_tuple tup) { assign(n, eastl::get<Indices>(tup)...); }

	void push_back(Ts&&... args) { emplace_back(eastl::move(args)...); }
	void push_back(const_reference_tuple tup) { push_back(eastl::get<Indices>(tup)...); }
	void push_back(rvalue_tuple tup) { emplace_back(eastl::move(eastl::get<Indices>(tup))...); }

	void emplace_back(rvalue_tuple tup) { emplace_back(eastl::move(eastl::get<Indices>(tup))..); }
	void emplace(const_iterator pos, rvalue_tuple tup) { emplace(pos, eastl::move(eastl::get<Indices>(tup))..); }

	iterator insert(const_iterator pos, const Ts&... args) { return insert(pos, 1, args...); }
	iterator insert(const_iterator pos, Ts&&... args) { return emplace(pos, eastl::move(args)...); }
	iterator insert(const_iterator pos, rvalue_tuple tup) { return emplace(pos, eastl::move(eastl::get<Indices>(tup))...); }
	iterator insert(const_iterator pos, const_reference_tuple tup) { return insert(pos, eastl::get<Indices>(tup)...); }
	iterator insert(const_iterator pos, size_type n, const_reference_tuple tup) { return insert(pos, n, eastl::get<Indices>(tup)...); }

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
#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
	// We allow the user to reference an empty container.
#elif EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(mNumElements == 0)) // We don't allow the user to reference an empty container.
			EASTL_FAIL_MSG("tuple_vector::front -- empty vector");
#endif
		return at(0); 
	}

	const_reference_tuple front() const
	{
#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
	// We allow the user to reference an empty container.
#elif EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(mNumElements == 0)) // We don't allow the user to reference an empty container.
			EASTL_FAIL_MSG("tuple_vector::front -- empty vector");
#endif
		return at(0); 
	}
	
	reference_tuple back() 
	{
#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
	// We allow the user to reference an empty container.
#elif EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(mNumElements == 0)) // We don't allow the user to reference an empty container.
			EASTL_FAIL_MSG("tuple_vector::back -- empty vector");
#endif
		return at(size() - 1); 
	}

	const_reference_tuple back() const 
	{
#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
	// We allow the user to reference an empty container.
#elif EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(mNumElements == 0)) // We don't allow the user to reference an empty container.
			EASTL_FAIL_MSG("tuple_vector::back -- empty vector");
#endif
		return at(size() - 1); 
	}

	template<size_t I>
	tuplevec_element_t<I, Ts...>* get() 
	{
		typedef tuplevec_element_t<I, Ts...> Element;
		return TupleVecLeaf<I, Element>::mpData;
	}
	template <size_t I>
	const tuplevec_element_t<I, Ts...>* get() const
	{
		typedef tuplevec_element_t<I, Ts...> Element;
		return TupleVecLeaf<I, Element>::mpData;
	}

	template<typename T>
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

	bool validate() const EA_NOEXCEPT
	{
		if (mNumElements > mNumCapacity)
			return false;
		if (!(variadicAnd(mpData <= TupleVecLeaf<Indices, Ts>::mpData...)))
			return false;
		void* pDataEnd = (void*)((uintptr_t)mpData + mDataSize);
		if (!(variadicAnd(pDataEnd >= TupleVecLeaf<Indices, Ts>::mpData...)))
			return false;
		return true;
	}

	template<typename Iterator>
	int validate_iterator(Iterator iter) const EA_NOEXCEPT
	{
		return validate_iterator_nongeneric(unwrap_iterator(iter));
	}

	int validate_iterator_nongeneric(const_iterator iter) const EA_NOEXCEPT
	{
		if (!(variadicAnd(iter.mpData[Indices] == TupleVecLeaf<Indices, Ts>::mpData...)))
			return isf_none;
		if (iter.mIndex < mNumElements)
			return (isf_valid | isf_current | isf_can_dereference);
		if (iter.mIndex <= mNumElements)
			return (isf_valid | isf_current);
		return isf_none;
	}

	static bool validateIteratorPair(const_iterator first, const_iterator last) EA_NOEXCEPT
	{
		return (first.mIndex <= last.mIndex) && variadicAnd(first.mpData[Indices] == last.mpData[Indices]...);
	}

protected:
	allocator_type mAllocator;

	void* mpData = nullptr;
	size_type mDataSize = 0;
	size_type mNumElements = 0;
	size_type mNumCapacity = 0;

	friend struct TupleRecurser<>;
	template<typename... Ts>
	friend struct TupleRecurser;

	template <typename MoveIterBase>
	void DoInitFromIterator(move_iterator<MoveIterBase> begin, move_iterator<MoveIterBase> end)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(!validateIteratorPair(begin.base(), end.base())))
			EASTL_FAIL_MSG("tuple_vector::erase -- invalid iterator pair");
#endif
		size_type newNumElements = (size_type)(end - begin);
		const void* ppOtherData[sizeof...(Ts)] = { begin.base().mpData[Indices]... };
		size_type beginIdx = begin.base().mIndex;
		size_type endIdx = end.base().mIndex;
		if (newNumElements > mNumCapacity)
		{
			DoReallocate(0, newNumElements);
		}
		mNumElements = newNumElements;
		swallow(DoUninitializedCopyPtr(
				eastl::move_iterator<Ts*>((Ts*)(ppOtherData[Indices]) + beginIdx),
				eastl::move_iterator<Ts*>((Ts*)(ppOtherData[Indices]) + endIdx),
				TupleVecLeaf<Indices, Ts>::mpData
			)...);
	}

	template <typename Iterator>
	void DoInitFromIterator(Iterator begin, Iterator end)
	{
#if EASTL_ASSERT_ENABLED
		if (EASTL_UNLIKELY(!validateIteratorPair(begin, end)))
			EASTL_FAIL_MSG("tuple_vector::erase -- invalid iterator pair");
#endif
		size_type newNumElements = (size_type)(end - begin);
		const void* ppOtherData[sizeof...(Ts)] = { begin.mpData[Indices]... };
		size_type beginIdx = begin.mIndex;
		size_type endIdx = end.mIndex;
		if (newNumElements > mNumCapacity)
		{
			DoReallocate(0, newNumElements);
		}
		mNumElements = newNumElements;
		swallow(DoUninitializedCopyPtr(
				(Ts*)(ppOtherData[Indices]) + beginIdx,
				(Ts*)(ppOtherData[Indices]) + endIdx,
				TupleVecLeaf<Indices, Ts>::mpData
			)...);
	}

	void DoInitFillTuple(size_type n, const_reference_tuple tup) { DoInitFillArgs(n, eastl::get<Indices>(tup)...); }

	void DoInitFillArgs(size_type n, const Ts&... args)
	{
		if (n > mNumCapacity)
		{
			DoReallocate(0, n);
		}
		mNumElements = n;
		swallow(DoUninitializedFillPtr(TupleVecLeaf<Indices, Ts>::mpData, TupleVecLeaf<Indices, Ts>::mpData + n, args)...);
	}

	void DoInitDefaultFill(size_type n)
	{
		if (n > mNumCapacity)
		{
			DoReallocate(0, n);
		}
		mNumElements = n;
		swallow(DoUninitializedDefaultFillN(TupleVecLeaf<Indices, Ts>::mpData, n)...);
	}

	void DoReallocate(size_type oldNumElements, size_type newCapacity)
	{
		void* ppNewLeaf[sizeof...(Ts)];
		pair<void*, size_type> allocation = TupleRecurser<Ts...>::DoAllocate<allocator_type, 0, index_sequence_type, Ts...>(*this, ppNewLeaf, newCapacity, 0);
		swallow(TupleVecLeaf<Indices, Ts>::DoUninitializedMoveAndDestruct(0, oldNumElements, (Ts*)ppNewLeaf[Indices])...);
		swallow(TupleVecLeaf<Indices, Ts>::mpData = (Ts*)ppNewLeaf[Indices]...);

		EASTLFree(mAllocator, mpData, mDataSize);
		mpData = allocation.first;
		mDataSize = allocation.second;
		mNumCapacity = newCapacity;
	}

	size_type GetNewCapacity(size_type currentCapacity)
	{
		return (currentCapacity > 0) ? (2 * currentCapacity) : 1;
	}
};

}  // namespace TupleVecInternal

template <typename Allocator, typename... Ts>
inline bool operator==(const TupleVecInternal::TupleVecImpl<Allocator, make_index_sequence<sizeof...(Ts)>, Ts...> a,
					   const TupleVecInternal::TupleVecImpl<Allocator, make_index_sequence<sizeof...(Ts)>, Ts...> b)
{
	return ((a.size() == b.size()) && equal(a.begin(), a.end(), b.begin()));
}

template <typename Allocator, typename... Ts>
inline bool operator!=(const TupleVecInternal::TupleVecImpl<Allocator, make_index_sequence<sizeof...(Ts)>, Ts...> a,
					   const TupleVecInternal::TupleVecImpl<Allocator, make_index_sequence<sizeof...(Ts)>, Ts...> b)
{
	return ((a.size() != b.size()) || !equal(a.begin(), a.end(), b.begin()));
}

template <typename Allocator, typename... Ts>
inline bool operator<(const TupleVecInternal::TupleVecImpl<Allocator, make_index_sequence<sizeof...(Ts)>, Ts...> a,
					  const TupleVecInternal::TupleVecImpl<Allocator, make_index_sequence<sizeof...(Ts)>, Ts...> b)
{
	return lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template <typename Allocator, typename... Ts>
inline bool operator>(const TupleVecInternal::TupleVecImpl<Allocator, make_index_sequence<sizeof...(Ts)>, Ts...> a,
					  const TupleVecInternal::TupleVecImpl<Allocator, make_index_sequence<sizeof...(Ts)>, Ts...> b)
{
	return b < a;
}

template <typename Allocator, typename... Ts>
inline bool operator<=(const TupleVecInternal::TupleVecImpl<Allocator, make_index_sequence<sizeof...(Ts)>, Ts...> a,
					   const TupleVecInternal::TupleVecImpl<Allocator, make_index_sequence<sizeof...(Ts)>, Ts...> b)
{
	return !(b < a);
}

template <typename Allocator, typename... Ts>
inline bool operator>=(const TupleVecInternal::TupleVecImpl<Allocator, make_index_sequence<sizeof...(Ts)>, Ts...> a,
					   const TupleVecInternal::TupleVecImpl<Allocator, make_index_sequence<sizeof...(Ts)>, Ts...> b)
{
	return !(a < b);
}

template <typename Allocator, typename... Ts>
inline void swap(TupleVecInternal::TupleVecImpl<Allocator, make_index_sequence<sizeof...(Ts)>, Ts...>& a,
				TupleVecInternal::TupleVecImpl<Allocator, make_index_sequence<sizeof...(Ts)>, Ts...>& b)
{
	a.swap(b);
}

// External interface of tuple_vector
template <typename... Ts>
class tuple_vector : public TupleVecInternal::TupleVecImpl<EASTLAllocatorType, make_index_sequence<sizeof...(Ts)>, Ts...>
{
	typedef TupleVecInternal::TupleVecImpl<EASTLAllocatorType, make_index_sequence<sizeof...(Ts)>, Ts...> base_type;
	using base_type::base_type;
};

}  // namespace eastl

#endif  // EASTL_TUPLEVECTOR_H
