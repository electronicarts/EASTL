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
struct TupleVecImpl;

template <typename... Ts>
struct TupleRecurser;

template <size_t I, typename... Ts>
struct TupleIndexRecurser;

template <typename AllocatorType, typename... Ts>
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
	static const size_t index = 0;
};

template <typename T, typename... TsRest>
struct tuplevec_index<T, TupleTypes<T, TsRest...>>
{
	typedef int DuplicateTypeCheck;
	static_assert(is_void<typename tuplevec_index<T, TupleTypes<TsRest...>>::DuplicateTypeCheck>::value, "duplicate type T in tuple_vector::get<T>(); unique types must be provided in declaration, or only use get<size_t>()");

	static const size_t index = 0;
};

template <typename T, typename Ts, typename... TsRest>
struct tuplevec_index<T, TupleTypes<Ts, TsRest...>>
{
	typedef typename tuplevec_index<T, TupleTypes<TsRest...>>::DuplicateTypeCheck DuplicateTypeCheck;
	static const size_t index = tuplevec_index<T, TupleTypes<TsRest...>>::index + 1;
};

template <typename Allocator, typename T, typename Indices, typename... Ts>
struct tuplevec_index<T, TupleVecInternal::TupleVecImpl<Allocator, Indices, Ts...>> : public tuplevec_index<T, TupleTypes<Ts...>>
{
};


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

	static constexpr size_t GetTotalAllocationSize(size_t capacity, size_t offset)
	{
		return offset;
	}

	template<typename Allocator, size_t I, typename Indices, typename... VecTypes>
	static pair<void*, size_t> DoAllocate(TupleVecImpl<Allocator, Indices, VecTypes...> &vec, void** ppNewLeaf, size_t capacity, size_t offset)
	{
		// If n is zero, then we allocate no memory and just return NULL. 
		// This is fine, as our default ctor initializes with NULL pointers. 
		size_t alignment = TupleRecurser<VecTypes...>::GetTotalAlignment();
		void* ptr = capacity ? allocate_memory(vec.mAllocator, offset, alignment, 0) : nullptr;
		return make_pair(ptr, offset);
	}

	template<typename TupleVecImplType, size_t I>
	static void SetNewData(TupleVecImplType &vec, void* pData, size_t capacity, size_t offset) 
	{ }
};

template <typename T, typename... Ts>
struct TupleRecurser<T, Ts...> : TupleRecurser<Ts...>
{
	static constexpr size_t GetTotalAlignment()
	{
		return max(alignof(T), TupleRecurser<Ts...>::GetTotalAlignment());
	}

	static constexpr size_t GetTotalAllocationSize(size_t capacity, size_t offset)
	{
		auto offsetRange = CalculateAllocationOffsetRange(offset, capacity);
		return TupleRecurser<Ts...>::GetTotalAllocationSize(capacity, offsetRange.second);
	}

	template<typename Allocator, size_t I, typename Indices, typename... VecTypes>
	static pair<void*, size_t> DoAllocate(TupleVecImpl<Allocator, Indices, VecTypes...> &vec, void** ppNewLeaf, size_t capacity, size_t offset)
	{
		auto offsetRange = CalculateAllocationOffsetRange(offset, capacity);
		auto allocation = TupleRecurser<Ts...>::DoAllocate<Allocator, I+1, Indices, VecTypes...>(vec, ppNewLeaf, capacity, offsetRange.second);
		ppNewLeaf[I] = (void*)((uintptr_t)(allocation.first) + offsetRange.first);
		return allocation;
	}

	template<typename TupleVecImplType, size_t I>
	static void SetNewData(TupleVecImplType &vec, void* pData, size_t capacity, size_t offset)
	{
		auto offsetRange = CalculateAllocationOffsetRange(offset, capacity);
		vec.TupleVecLeaf<I, T>::mpData = (T*)((uintptr_t)pData + offsetRange.first);
		TupleRecurser<Ts...>::SetNewData<TupleVecImplType, I + 1>(vec, pData, capacity, offsetRange.second);
	}

private:
	static constexpr pair<size_t, size_t> CalculateAllocationOffsetRange(size_t offset, size_t capacity)
	{
		size_t alignment = alignof(T);
		size_t offsetBegin = (offset + alignment - 1) & (~alignment + 1);
		size_t offsetEnd = offsetBegin + sizeof(T) * capacity;
		return pair<size_t, size_t>(offsetBegin, offsetEnd);
	}
};

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

	int DoConstruction(const size_t begin, const size_t end)
	{
		eastl::uninitialized_default_fill(mpData + begin, mpData + end);
		return 0;
	}

	int DoConstruction(const size_t begin, const size_t end, const T& arg)
	{
		eastl::uninitialized_fill_ptr(mpData + begin, mpData + end, arg);
		return 0;
	}

	int DoUninitializedMove(void* pDest, const size_t begin, const size_t end)
	{
		eastl::uninitialized_move_ptr_if_noexcept(mpData + begin, mpData + end, (T*)pDest);
		eastl::destruct(mpData + begin, mpData + end);
		return 0;
	}

	int DoMove(const size_t srcBegin, const size_t srcEnd, const size_t destBegin)
	{
		eastl::move(mpData + srcBegin, mpData + srcEnd, mpData + destBegin);
		return 0;
	}

	int DoInsertValues(size_t pos, size_t n, size_t numElements, const T& arg)
	{
		T* pDest = mpData + pos;
		T* pEnd = mpData + numElements;
		
		const T temp = arg;
		const size_t nExtra = (numElements - pos);
		if (n < nExtra) // If the inserted values are entirely within initialized memory (i.e. are before mpEnd)...
		{
			eastl::uninitialized_move_ptr(pEnd - n, pEnd, pEnd);
			eastl::move_backward(pDest, pEnd - n, pEnd); // We need move_backward because of potential overlap issues.
			eastl::fill(pDest, pDest + n, temp);
		}
		else
		{
			eastl::uninitialized_fill_n_ptr(pEnd, n - nExtra, temp);
			eastl::uninitialized_move_ptr(pDest, pEnd, pEnd + n - nExtra);
			eastl::fill(pDest, pEnd, temp);
		}
		return 0;
	}

	int DoDestruct(size_t begin, size_t end)
	{
		eastl::destruct(mpData + begin, mpData + end);
		return 0;
	}

	int DoSwap(TupleVecLeaf<I, T>& x)
	{
		eastl::swap(mpData, x.mpData);
		return 0;
	}

	int SetData(void* pData)
	{
		mpData = (T*)pData;
		return 0;
	}

	T* mpData = nullptr;
};

// swallow allows for parameter pack expansion of arguments as means of expanding operations performed
template <typename... Ts>
void swallow(Ts&&...)
{
}

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
template <typename AllocatorType, typename... Ts>
struct TupleVecIter : public iterator<random_access_iterator_tag, tuple<Ts...>, ptrdiff_t, tuple<Ts*...>, tuple<Ts&...>>
{
private:
	typedef TupleVecIter<AllocatorType, Ts...> this_type;
	typedef TupleVecImpl<AllocatorType, make_index_sequence<sizeof...(Ts)>, Ts...> vec_impl_type;

	template<typename U, typename... Us> 
	friend struct TupleVecIter;

public:
	TupleVecIter() = default;
	TupleVecIter(vec_impl_type* tupleVec, size_t index)
		: mTupleVec(tupleVec), mIndex(index) { }

	template <typename OtherAllocType,
			  typename... Us,
			  typename = typename enable_if<TupleVecIterCompatible<TupleTypes<Us...>, TupleTypes<Ts...>>::value, bool>::type>
	TupleVecIter(const TupleVecIter<OtherAllocType, Us...>& other)
		: mTupleVec((vec_impl_type*)(other.mTupleVec))
		, mIndex(other.mIndex)
	{
	}

	bool operator==(const TupleVecIter& other) const { return mIndex == other.mIndex && mTupleVec->get<0>() == other.mTupleVec->get<0>(); }
	bool operator!=(const TupleVecIter& other) const { return mIndex != other.mIndex || mTupleVec->get<0>() != other.mTupleVec->get<0>(); }
	reference operator*() const { return MakeReference(make_index_sequence<sizeof...(Ts)>()); }

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

	reference operator[](const size_t n) const
	{
		return *(*this + n);
	}

private:

	template <size_t... Indices>
	value_type MakeValue(integer_sequence<size_t, Indices...> indices) const
	{
		return value_type(mTupleVec->get<Indices>()[mIndex]...);
	}

	template <size_t... Indices>
	reference MakeReference(integer_sequence<size_t, Indices...> indices) const
	{
		return reference(mTupleVec->get<Indices>()[mIndex]...);
	}

	template <size_t... Indices>
	pointer MakePointer(integer_sequence<size_t, Indices...> indices) const
	{
		return pointer(&mTupleVec->get<Indices>()[mIndex]...);
	}

	size_t mIndex = 0;
	vec_impl_type* mTupleVec = nullptr;
};


// TupleVecImpl
template <typename Allocator, size_t... Indices, typename... Ts>
class TupleVecImpl<Allocator, integer_sequence<size_t, Indices...>, Ts...> : public TupleVecLeaf<Indices, Ts>...
{
	typedef Allocator    allocator_type;
	typedef TupleVecImpl<Allocator, integer_sequence<size_t, Indices...>, Ts...> this_type;
	typedef TupleVecImpl<Allocator, integer_sequence<size_t, Indices...>, const Ts...> const_this_type;

public:
	typedef TupleVecInternal::TupleVecIter<Allocator, Ts...> iterator;
	typedef TupleVecInternal::TupleVecIter<Allocator, const Ts...> const_iterator;
	typedef eastl::reverse_iterator<iterator> reverse_iterator;
	typedef eastl::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef eastl_size_t size_type;
	typedef eastl::tuple<Ts...> value_tuple;
	typedef eastl::tuple<Ts&...> reference_tuple;
	typedef eastl::tuple<const Ts&...> const_reference_tuple;
	typedef eastl::tuple<Ts*...> ptr_tuple;
	typedef eastl::tuple<const Ts*...> const_ptr_tuple;

	TupleVecImpl()
		: mAllocator(allocator_type(EASTL_TUPLE_VECTOR_DEFAULT_NAME))
	{}
	TupleVecImpl(const allocator_type& allocator)
		: mAllocator(allocator)
	{}

protected:
	// ctor to provide a pre-allocated field of data that the container will own, specifically for fixed_tuple_vector
	TupleVecImpl(const allocator_type& allocator, void* pData, size_type capacity)
		: mAllocator(allocator), mpData(pData), mNumCapacity(capacity)
	{
		TupleRecurser<Ts...>::SetNewData<this_type, 0>(*this, mpData, mNumCapacity, 0);
	}

public:
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
	
	iterator insert(const_iterator pos, const Ts&... args)
	{
		return insert(pos, 1, args...);
	}

	iterator insert(const_iterator pos, size_t n, const Ts&... args)
	{
		size_t firstIdx = pos - cbegin();
		size_t lastIdx = firstIdx + n;
		size_t newNumElements = mNumElements + n;
		if (newNumElements >= mNumCapacity || firstIdx != mNumElements)
		{
			if (newNumElements >= mNumCapacity)
			{
				const auto newCapacity = max(GetNewCapacity(mNumCapacity), newNumElements);

				void* ppNewLeaf[sizeof...(Ts)];
				auto allocation =
					TupleRecurser<Ts...>::DoAllocate<allocator_type, 0, integer_sequence<size_t, Indices...>, Ts...>(
						*this, ppNewLeaf, newCapacity, 0);

				swallow(TupleVecLeaf<Indices, Ts>::DoUninitializedMove(ppNewLeaf[Indices], 0, firstIdx)...);
				swallow(TupleVecLeaf<Indices, Ts>::DoUninitializedMove((void*)((Ts*)ppNewLeaf[Indices] + lastIdx),
						                                               firstIdx, mNumElements)...);
				swallow(TupleVecLeaf<Indices, Ts>::SetData(ppNewLeaf[Indices])...);
				swallow(TupleVecLeaf<Indices, Ts>::DoConstruction(firstIdx, lastIdx, args)...);

				EASTLFree(mAllocator, mpData, mDataSize);
				mpData = allocation.first;
				mDataSize = allocation.second;
				mNumCapacity = newCapacity;
			}
			else
			{
				swallow(TupleVecLeaf<Indices, Ts>::DoInsertValues(firstIdx, n, mNumElements, args)...);
			}
		}
		else
		{
			swallow(TupleVecLeaf<Indices, Ts>::DoConstruction(mNumElements, newNumElements, args)...);
		}
		mNumElements = newNumElements;
		return begin() + firstIdx;

	}

	iterator erase(const_iterator pos)
	{
		return erase(pos, pos + 1);
	}

	iterator erase(const_iterator first, const_iterator last)
	{
		if (first != last)
		{
			size_t firstIdx = first - cbegin();
			size_t lastIdx = last - cbegin();
			swallow(TupleVecLeaf<Indices, Ts>::DoMove(lastIdx, mNumElements, firstIdx)...);
			size_t newNumElements = mNumElements - (lastIdx - firstIdx);
			swallow(TupleVecLeaf<Indices, Ts>::DoDestruct(newNumElements, mNumElements)...);
			mNumElements = newNumElements;
		}

		return first;
	}
	
	void resize(size_type n)
	{
		if (n > mNumElements)
		{
			if (n > mNumCapacity)
			{
				DoGrow(n);
			}
			swallow(TupleVecLeaf<Indices, Ts>::DoConstruction(mNumElements, n)...);
		}
		else
		{
			swallow(TupleVecLeaf<Indices, Ts>::DoDestruct(n, mNumElements)...);
		}
		mNumElements = n;
	}

	void reserve(size_type n)
	{
		if (n > mNumCapacity)
		{
			DoGrow(n);
		}
	}

	void shrink_to_fit()
	{
		if (mNumElements < mNumCapacity)
		{
			DoGrow(mNumElements);
		}
	}

	void clear()
	{
		swallow(TupleVecLeaf<Indices, Ts>::DoDestruct(0, mNumElements)...);
		mNumElements = 0;
	}

	void pop_back()
	{
		swallow(TupleVecLeaf<Indices, Ts>::DoDestruct(mNumElements-1, mNumElements)...);
		mNumElements--;
	}

	void swap(this_type& x)
	{
		swallow(TupleVecLeaf<Indices, Ts>::DoSwap(x)...);
		eastl::swap(mAllocator, x.mAllocator);
		eastl::swap(mpData, x.mpData);
		eastl::swap(mDataSize, x.mDataSize);
		eastl::swap(mNumElements, x.mNumElements);
		eastl::swap(mNumCapacity, x.mNumCapacity);
	}

	bool empty() const { return mNumElements == 0; }
	size_type size() const { return mNumElements; }
	size_type capacity() const { return mNumCapacity; }

	iterator begin() { return iterator(this, 0); }
	const_iterator begin() const { return const_iterator((const_this_type*)(this), 0); }
	const_iterator cbegin() const { return const_iterator((const_this_type*)(this), 0); }

	iterator end() { return iterator(this, size()); }
	const_iterator end() const { return const_iterator((const_this_type*)(this), size()); }
	const_iterator cend() const { return const_iterator((const_this_type*)(this), size()); }

	reverse_iterator rbegin() { return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const  { return const_reverse_iterator(end()); }
	const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
	
	reverse_iterator rend() { return reverse_iterator(begin()); }
	const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
	const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

	ptr_tuple data() { return ptr_tuple(TupleVecLeaf<Indices, Ts>::mpData...); }
	const_ptr_tuple data() const { return ptr_tuple(TupleVecLeaf<Indices, Ts>::mpData...); }

	reference_tuple at(size_type n) { return reference_tuple(*(TupleVecLeaf<Indices, Ts>::mpData + n)...); }
	const_reference_tuple at(size_type n) const { return reference_tuple(*(TupleVecLeaf<Indices, Ts>::mpData + n)...); }
	
	reference_tuple operator[](size_type n) { return at(n); }
	const_reference_tuple operator[](size_type n) const { return at(n); }
	
	reference_tuple front() { return at(0); }
	const_reference_tuple front() const { return at(0); }
	
	reference_tuple back() { return at(size() - 1); }
	const_reference_tuple back() const { return at(size() - 1); }

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
		void* ppNewLeaf[sizeof...(Ts)];
		auto allocation = TupleRecurser<Ts...>::DoAllocate<allocator_type, 0, integer_sequence<size_t, Indices...>, Ts...>(*this, ppNewLeaf, n, 0);
		swallow(TupleVecLeaf<Indices, Ts>::DoUninitializedMove(ppNewLeaf[Indices], 0, mNumElements)...);
		swallow(TupleVecLeaf<Indices, Ts>::SetData(ppNewLeaf[Indices])...);
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

}  // namespace TupleVecInternal

// External interface of tuple_vector
template <typename... Ts>
class tuple_vector : public TupleVecInternal::TupleVecImpl<EASTLAllocatorType, make_index_sequence<sizeof...(Ts)>, Ts...>
{
};

}  // namespace eastl

#endif  // EASTL_TUPLEVECTOR_H