///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_FIXEDTUPLEVECTOR_H
#define EASTL_FIXEDTUPLEVECTOR_H

#include <EASTL/tuple_vector.h>
#include <EASTL/internal/fixed_pool.h>

namespace eastl
{

// External interface of fixed_tuple_vector
template <size_t nodeCount, bool bEnableOverflow, typename... Ts>
class fixed_tuple_vector
{
private:
	typedef fixed_vector_allocator<
		TupleVecInternal::TupleRecurser<Ts...>::GetTotalAllocationSize(nodeCount, 0), 1,
		TupleVecInternal::TupleRecurser<Ts...>::GetTotalAlignment(), 0,
		bEnableOverflow, EASTLAllocatorType> fixed_allocator_type;

	typedef TupleVecInternal::TupleVecImpl<fixed_allocator_type, make_index_sequence<sizeof...(Ts)>, Ts...> Impl;
	typedef aligned_buffer<fixed_allocator_type::kNodesSize, fixed_allocator_type::kNodeAlignment> aligned_buffer_type;

public:
	typedef TupleVecInternal::TupleVecIter<Ts...> iterator;
	typedef typename Impl::size_type size_type;

	fixed_tuple_vector();
	
	void push_back();
	void push_back(const Ts&... args);
	void push_back_uninitialized();

	size_type size();
	size_type capacity();

	iterator begin();
	iterator end();

	void reserve(size_t n);

	template<size_t I>
	tuplevec_element_t<I, Ts...>* get();

	template<typename T>
	T* get();

private:
	Impl mImpl;
	aligned_buffer_type mBuffer;

};

template <size_t nodeCount, bool bEnableOverflow, typename... Ts>
fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::fixed_tuple_vector()
	:mImpl(fixed_allocator_type(mBuffer.buffer), mBuffer.buffer, nodeCount)
{
	
}

template <size_t nodeCount, bool bEnableOverflow, typename... Ts>
void fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::push_back()
{
	mImpl.push_back();
}


template <size_t nodeCount, bool bEnableOverflow, typename... Ts>
void fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::push_back(const Ts&... args)
{
	mImpl.push_back(args...);
}

template <size_t nodeCount, bool bEnableOverflow, typename... Ts>
void fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::push_back_uninitialized()
{
	mImpl.push_back_uninitialized();
}

template <size_t nodeCount, bool bEnableOverflow, typename... Ts>
typename fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::size_type fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::size()
{
	return mImpl.size();
}

template <size_t nodeCount, bool bEnableOverflow, typename... Ts>
typename fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::size_type fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::capacity()
{
	return mImpl.capacity();
}

template <size_t nodeCount, bool bEnableOverflow, typename... Ts>
typename fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::iterator fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::begin()
{
	return fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::iterator(*this, 0);
}

template <size_t nodeCount, bool bEnableOverflow, typename... Ts>
typename fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::iterator fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::end()
{
	return fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::iterator(*this, size());
}

template <size_t nodeCount, bool bEnableOverflow, typename... Ts>
void fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::reserve(size_t n)
{
	mImpl.reserve(n);
}

template <size_t nodeCount, bool bEnableOverflow, typename... Ts>
template <size_t I>
tuplevec_element_t<I, Ts...>* fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::get()
{
	return TupleVecInternal::get<I, Impl, Ts...>(mImpl);
}

template <size_t nodeCount, bool bEnableOverflow, typename... Ts>
template<typename T>
T* fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>::get()
{
	return TupleVecInternal::get<T>(mImpl);
}

}  // namespace eastl

#endif  // EASTL_TUPLEVECTOR_H
