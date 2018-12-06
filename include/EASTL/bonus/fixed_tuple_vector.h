///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_FIXEDTUPLEVECTOR_H
#define EASTL_FIXEDTUPLEVECTOR_H

#include <EASTL/bonus/tuple_vector.h>
#include <EASTL/internal/fixed_pool.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif

namespace eastl
{

	/// EASTL_FIXED_TUPLE_VECTOR_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	/// In the case of fixed-size containers, the allocator name always refers
	/// to overflow allocations. 
	///
	#ifndef EASTL_FIXED_TUPLE_VECTOR_DEFAULT_NAME
		#define EASTL_FIXED_TUPLE_VECTOR_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " fixed_tuple_vector" // Unless the user overrides something, this is "EASTL fixed_vector".
	#endif


	/// EASTL_FIXED_TUPLE_VECTOR_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_FIXED_TUPLE_VECTOR_DEFAULT_ALLOCATOR
		#define EASTL_FIXED_TUPLE_VECTOR_DEFAULT_ALLOCATOR overflow_allocator_type(EASTL_FIXED_TUPLE_VECTOR_DEFAULT_NAME)
	#endif

// External interface of fixed_tuple_vector
template <size_t nodeCount, bool bEnableOverflow, typename... Ts>
class fixed_tuple_vector : public TupleVecInternal::TupleVecImpl<fixed_vector_allocator<
	TupleVecInternal::TupleRecurser<Ts...>::GetTotalAllocationSize(nodeCount, 0), 1,
	TupleVecInternal::TupleRecurser<Ts...>::GetTotalAlignment(), 0,
	bEnableOverflow, EASTLAllocatorType>, make_index_sequence<sizeof...(Ts)>, Ts...>
{
public:
	typedef fixed_vector_allocator<
		TupleVecInternal::TupleRecurser<Ts...>::GetTotalAllocationSize(nodeCount, 0), 1,
		TupleVecInternal::TupleRecurser<Ts...>::GetTotalAlignment(), 0,
		bEnableOverflow, EASTLAllocatorType> fixed_allocator_type;
	typedef aligned_buffer<fixed_allocator_type::kNodesSize, fixed_allocator_type::kNodeAlignment> aligned_buffer_type;
	typedef fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...> this_type;
	typedef EASTLAllocatorType overflow_allocator_type;

	typedef TupleVecInternal::TupleVecImpl<fixed_allocator_type, make_index_sequence<sizeof...(Ts)>, Ts...> base_type;
	typedef typename base_type::size_type size_type;

private:
	aligned_buffer_type mBuffer;

public:
	fixed_tuple_vector()
		: base_type(fixed_allocator_type(mBuffer.buffer), mBuffer.buffer, nodeCount, fixed_allocator_type::kNodeSize)
	{ }

	fixed_tuple_vector(const overflow_allocator_type& allocator)
		: base_type(fixed_allocator_type(mBuffer.buffer, allocator), mBuffer.buffer, nodeCount, fixed_allocator_type::kNodeSize)
	{ }

	fixed_tuple_vector(this_type&& x)
		: base_type(fixed_allocator_type(mBuffer.buffer), mBuffer.buffer, nodeCount, fixed_allocator_type::kNodeSize)
	{ 
		base_type::get_allocator().copy_overflow_allocator(x.get_allocator());
		base_type::DoInitFromIterator(make_move_iterator(x.begin()), make_move_iterator(x.end()));
		x.clear();
	}

	fixed_tuple_vector(this_type&& x, const overflow_allocator_type& allocator)
		: base_type(fixed_allocator_type(mBuffer.buffer, allocator), mBuffer.buffer, nodeCount, fixed_allocator_type::kNodeSize)
	{
		base_type::DoInitFromIterator(make_move_iterator(x.begin()), make_move_iterator(x.end()));
		x.clear();
	}

	fixed_tuple_vector(const this_type& x)
		: base_type(fixed_allocator_type(mBuffer.buffer), mBuffer.buffer, nodeCount, fixed_allocator_type::kNodeSize)
	{ 
		base_type::get_allocator().copy_overflow_allocator(x.get_allocator());
		base_type::DoInitFromIterator(x.begin(), x.end());
	}

	fixed_tuple_vector(const this_type& x, const overflow_allocator_type& allocator)
		: base_type(fixed_allocator_type(mBuffer.buffer, allocator), mBuffer.buffer, nodeCount, fixed_allocator_type::kNodeSize)
	{
		base_type::DoInitFromIterator(x.begin(), x.end());
	}

	template <typename MoveIterBase>
	fixed_tuple_vector(move_iterator<MoveIterBase> begin, move_iterator<MoveIterBase> end, const overflow_allocator_type& allocator = EASTL_FIXED_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
		: base_type(fixed_allocator_type(mBuffer.buffer, allocator), mBuffer.buffer, nodeCount, fixed_allocator_type::kNodeSize)
	{
		base_type::DoInitFromIterator(begin, end);
	}

	template <typename Iterator>
	fixed_tuple_vector(Iterator begin, Iterator end, const overflow_allocator_type& allocator = EASTL_FIXED_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
		: base_type(fixed_allocator_type(mBuffer.buffer, allocator), mBuffer.buffer, nodeCount, fixed_allocator_type::kNodeSize)
	{
		base_type::DoInitFromIterator(begin, end);
	}

	fixed_tuple_vector(size_type n, const overflow_allocator_type& allocator = EASTL_FIXED_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
		: base_type(fixed_allocator_type(mBuffer.buffer, allocator), mBuffer.buffer, nodeCount, fixed_allocator_type::kNodeSize)
	{
		base_type::DoInitDefaultFill(n);
	}

	fixed_tuple_vector(size_type n, const Ts&... args)
		: base_type(fixed_allocator_type(mBuffer.buffer), mBuffer.buffer, nodeCount, fixed_allocator_type::kNodeSize)
	{
		base_type::DoInitFillArgs(n, args...);
	}

	fixed_tuple_vector(size_type n, const Ts&... args, const overflow_allocator_type& allocator)
		: base_type(fixed_allocator_type(mBuffer.buffer, allocator), mBuffer.buffer, nodeCount, fixed_allocator_type::kNodeSize)
	{
		base_type::DoInitFillArgs(n, args...);
	}

	fixed_tuple_vector(size_type n,
				typename base_type::const_reference_tuple tup,
				const overflow_allocator_type& allocator = EASTL_FIXED_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
		: base_type(fixed_allocator_type(mBuffer.buffer, allocator), mBuffer.buffer, nodeCount, fixed_allocator_type::kNodeSize)
	{
		base_type::DoInitFillTuple(n, tup);
	}

	fixed_tuple_vector(const typename base_type::value_tuple* first, const typename base_type::value_tuple* last,
		const overflow_allocator_type& allocator = EASTL_FIXED_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
		: base_type(fixed_allocator_type(mBuffer.buffer, allocator), mBuffer.buffer, nodeCount, fixed_allocator_type::kNodeSize)
	{
		base_type::DoInitFromTupleArray(first, last);
	}

	fixed_tuple_vector(std::initializer_list<typename base_type::value_tuple> iList,
		const overflow_allocator_type& allocator = EASTL_FIXED_TUPLE_VECTOR_DEFAULT_ALLOCATOR)
		: base_type(fixed_allocator_type(mBuffer.buffer, allocator), mBuffer.buffer, nodeCount, fixed_allocator_type::kNodeSize)
	{
		base_type::DoInitFromTupleArray(iList.begin(), iList.end());
	}

	this_type& operator=(const this_type& other)
	{
		base_type::operator=(other);
		return *this;
	}

	this_type& operator=(this_type&& other)
	{
		base_type::clear();
		// OK to call DoInitFromIterator in a non-ctor scenario because clear() reset everything, more-or-less
		base_type::DoInitFromIterator(make_move_iterator(other.begin()), make_move_iterator(other.end()));
		other.clear();
		return *this;
	}

	this_type& operator=(std::initializer_list<typename base_type::value_tuple> iList)
	{
		base_type::operator=(iList);
		return *this;
	}

	void swap(this_type& x)
	{
		// If both containers are using the heap instead of local memory 
		// then we can do a fast pointer swap instead of content swap.
		if ((has_overflowed() && x.has_overflowed()) && (get_overflow_allocator() == x.get_overflow_allocator()))
		{
			base_type::swap(x);
		}
		else
		{
			// Fixed containers use a special swap that can deal with excessively large buffers.
			eastl::fixed_swap(*this, x);
		}
	}

	// Returns the max fixed size, which is the user-supplied nodeCount parameter.
	size_type max_size() const { return nodeCount; }
	// Returns true if the fixed space has been fully allocated. Note that if overflow is enabled,
	// the container size can be greater than nodeCount but full() could return true because the
	// fixed space may have a recently freed slot.
	bool full() const { return (base_type::mNumElements >= nodeCount) || ((void*)base_type::mpData != (void*)mBuffer.buffer);	}
	// Returns true if the allocations spilled over into the overflow allocator. Meaningful
	// only if overflow is enabled.
	bool has_overflowed() const { return ((void*)base_type::mpData != (void*)mBuffer.buffer); }
	// Returns the value of the bEnableOverflow template parameter.
	bool can_overflow() const { return bEnableOverflow; }

	const overflow_allocator_type& get_overflow_allocator() const { return base_type::get_allocator().get_overflow_allocator(); }
};


template <size_t nodeCount, bool bEnableOverflow, typename... Ts>
inline void swap(fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>& a,
				fixed_tuple_vector<nodeCount, bEnableOverflow, Ts...>& b)
{
	a.swap(b);
}


}  // namespace eastl

#endif  // EASTL_TUPLEVECTOR_H
