/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_MEMORY_ORDER_H
#define EASTL_ATOMIC_INTERNAL_MEMORY_ORDER_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


namespace eastl
{


namespace internal
{


struct memory_order_relaxed_s {};
struct memory_order_read_depends_s {};
struct memory_order_acquire_s {};
struct memory_order_release_s {};
struct memory_order_acq_rel_s {};
struct memory_order_seq_cst_s {};


} // namespace internal


EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR auto memory_order_relaxed = internal::memory_order_relaxed_s{};
EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR auto memory_order_read_depends = internal::memory_order_read_depends_s{};
EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR auto memory_order_acquire = internal::memory_order_acquire_s{};
EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR auto memory_order_release = internal::memory_order_release_s{};
EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR auto memory_order_acq_rel = internal::memory_order_acq_rel_s{};
EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR auto memory_order_seq_cst = internal::memory_order_seq_cst_s{};


} // namespace eastl


#endif /* EASTL_ATOMIC_INTERNAL_MEMORY_ORDER_H */
