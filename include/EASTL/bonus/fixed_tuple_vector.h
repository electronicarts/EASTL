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

	typedef TupleVecInternal::TupleVecImpl<fixed_allocator_type, make_index_sequence<sizeof...(Ts)>, Ts...> base_type;
	typedef aligned_buffer<fixed_allocator_type::kNodesSize, fixed_allocator_type::kNodeAlignment> aligned_buffer_type;

	aligned_buffer_type mBuffer;

	using base_type::base_type;

	typedef typename base_type::size_type size_type;

public:
	fixed_tuple_vector()
		:base_type(fixed_allocator_type(mBuffer.buffer), mBuffer.buffer, nodeCount)
	{ }

	// Returns the max fixed size, which is the user-supplied nodeCount parameter.
	size_type max_size() const { return nodeCount; }
	// Returns true if the fixed space has been fully allocated. Note that if overflow is enabled,
	// the container size can be greater than nodeCount but full() could return true because the
	// fixed space may have a recently freed slot.
	bool full() const { return (mNumElements >= nodeCount) || ((void*)mpData != (void*)mBuffer.buffer);	}
	// Returns true if the allocations spilled over into the overflow allocator. Meaningful
	// only if overflow is enabled.
	bool has_overflowed() const { return ((void*)mpData != (void*)mBuffer.buffer); }
	// Returns the value of the bEnableOverflow template parameter.
	bool can_overflow() const { return bEnableOverflow; }
};

}  // namespace eastl

#endif  // EASTL_TUPLEVECTOR_H
