///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_FIXED_RING_BUFFER_H
#define EASTL_FIXED_RING_BUFFER_H

#include <EASTL/internal/config.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/bonus/ring_buffer.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif

namespace eastl
{

	/// fixed_ring_buffer
	///
	/// This is a convenience template alias for creating a fixed-sized
	/// ring_buffer using eastl::fixed_vector as its storage container. This has
	/// been tricky for users to get correct due to the constructor requirements
	/// of eastl::ring_buffer leaking the implementation detail of the sentinel
	/// value being used internally.  In addition, it was not obvious what the
	/// correct allocator_type template parameter should be used for containers
	/// providing both a default allocator type and an overflow allocator type.
	///
	/// We are over-allocating the fixed_vector container to accommodate the
	/// ring_buffer sentinel to prevent that implementation detail leaking into
	/// user code.
	///
	/// Example usage:
	///
	/// 	fixed_ring_buffer<int, 8> rb = {0, 1, 2, 3, 4, 5, 6, 7};
	///  or
	/// 	fixed_ring_buffer<int, 8> rb(8); // capacity doesn't need to respect sentinel
	/// 	rb.push_back(0);
	///
	///
#if !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
	template <typename T, size_t N>
	using fixed_ring_buffer =
	    ring_buffer<T, fixed_vector<T, N + 1, false>, typename fixed_vector<T, N + 1, false>::overflow_allocator_type>;
#endif

} // namespace eastl

#endif // Header include guard

