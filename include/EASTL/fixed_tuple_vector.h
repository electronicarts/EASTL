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
	typedef TupleVecInternal::TupleVecIter<fixed_allocator_type, Ts...> iterator;
	typedef typename Impl::size_type size_type;

	fixed_tuple_vector()
		:mImpl(fixed_allocator_type(mBuffer.buffer), mBuffer.buffer, nodeCount)
	{ }
	
	void push_back() { mImpl.push_back(); }
	void push_back(const Ts&... args) { mImpl.push_back(args...); }
	void push_back_uninitialized() { mImpl.push_back_uninitialized(); }

	bool empty() { return mImpl.empty(); }
	size_type size() { return mImpl.size(); }
	size_type capacity() { return mImpl.capacity(); }

	iterator begin() { return iterator(mImpl, 0); }
	iterator end() { return iterator(mImpl, size()); }

	void reserve(size_t n) { mImpl.reserve(n); }

	template<size_t I>
	tuplevec_element_t<I, Ts...>* get() { return TupleVecInternal::get<I, Impl, Ts...>(mImpl); }

	template<typename T>
	T* get() { return TupleVecInternal::get<T>(mImpl); }

private:
	Impl mImpl;
	aligned_buffer_type mBuffer;

};

}  // namespace eastl

#endif  // EASTL_TUPLEVECTOR_H
