///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_FIXEDTUPLEVECTOR_H
#define EASTL_FIXEDTUPLEVECTOR_H

#include <EASTL/bonus/tuple_vector.h>
#include <EASTL/internal/fixed_pool.h>

namespace eastl
{

// External interface of fixed_tuple_vector
template <size_t nodeCount, bool bEnableOverflow, typename... Ts>
class fixed_tuple_vector : public TupleVecInternal::TupleVecImpl<fixed_vector_allocator<
	TupleVecInternal::TupleRecurser<Ts...>::GetTotalAllocationSize(nodeCount, 0), 1,
	TupleVecInternal::TupleRecurser<Ts...>::GetTotalAlignment(), 0,
	bEnableOverflow, EASTLAllocatorType>, make_index_sequence<sizeof...(Ts)>, Ts...>
{
private:
	typedef fixed_vector_allocator<
		TupleVecInternal::TupleRecurser<Ts...>::GetTotalAllocationSize(nodeCount, 0), 1,
		TupleVecInternal::TupleRecurser<Ts...>::GetTotalAlignment(), 0,
		bEnableOverflow, EASTLAllocatorType> fixed_allocator_type;

	typedef TupleVecInternal::TupleVecImpl<fixed_allocator_type, make_index_sequence<sizeof...(Ts)>, Ts...> Impl;
	typedef aligned_buffer<fixed_allocator_type::kNodesSize, fixed_allocator_type::kNodeAlignment> aligned_buffer_type;

	aligned_buffer_type mBuffer;

public:
	fixed_tuple_vector()
		:Impl(fixed_allocator_type(mBuffer.buffer), mBuffer.buffer, nodeCount)
	{ }
};

}  // namespace eastl

#endif  // EASTL_TUPLEVECTOR_H
