/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_GCC_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_GCC_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/**
 * NOTE:
 *
 * gcc __atomic builtins may defer to function calls in libatomic.so for architectures that do not
 * support atomic instructions of a given size. These functions will be implemented with pthread_mutex_t.
 * It also requires the explicit linking against the compiler runtime libatomic.so.
 * On architectures that do not support atomics, like armv6 the builtins may defer to kernel helpers
 * or on classic uniprocessor systems just disable interrupts.
 *
 * We do not want to have to link against libatomic.so or fall into the trap of our atomics degrading
 * into locks. We would rather have user-code explicitly use locking primitives if their code cannot
 * be satisfied with atomic instructions on the given platform.
 */
static_assert(__atomic_always_lock_free(1, 0), "eastl::atomic<T> where sizeof(T) == 1 must be lock-free!");
static_assert(__atomic_always_lock_free(2, 0), "eastl::atomic<T> where sizeof(T) == 2 must be lock-free!");
static_assert(__atomic_always_lock_free(4, 0), "eastl::atomic<T> where sizeof(T) == 4 must be lock-free!");
#if EA_PLATFORM_PTR_SIZE == 8
	static_assert(__atomic_always_lock_free(8, 0), "eastl::atomic<T> where sizeof(T) == 8 must be lock-free!");
#endif

/**
 * NOTE:
 *
 * The following can fail on gcc/clang on 64-bit systems.
 * Firstly, it depends on the -march setting on clang whether or not it calls out to libatomic for 128-bit operations.
 * Second, gcc always calls out to libatomic for 128-bit atomics. It is unclear if it uses locks
 * or tries to look at the cpuid and use cmpxchg16b if its available.
 * gcc mailing lists argue that since load must be implemented with cmpxchg16b, then the __atomic bultin
 * cannot be used in read-only memory which is why they always call out to libatomic.
 * There is no way to tell gcc to not do that, unfortunately.
 * We don't care about the read-only restriction because our eastl::atomic<T> object is mutable
 * and also msvc doesn't enforce this restriction thus to be fully platform agnostic we cannot either.
 *
 * Therefore, the follow static_assert is commented out for the time being, as it always fails on these compilers.
 * We still guarantee 128-bit atomics are lock-free by handrolling the inline assembly ourselves.
 *
 * static_assert(__atomic_always_lock_free(16, 0), "eastl::atomic<T> where sizeof(T) == 16 must be lock-free!");
 */

/**
 * NOTE:
 *
 * Why do we do the cast to the unsigned fixed width types for every operation even though gcc/clang builtins are generics?
 * Well gcc/clang correctly-incorrectly call out to libatomic and do locking on user types that may be potentially misaligned.
 * struct UserType { uint8_t a,b; }; This given struct is 2 bytes in size but has only 1 byte alignment.
 * gcc/clang cannot and doesn't know that we always guarantee every type T is size aligned within eastl::atomic<T>.
 * Therefore it always emits calls into libatomic and does locking for structs like these which we do not want.
 * Therefore you'll notice we always cast each atomic ptr type to the equivalent unsigned fixed width type when doing the atomic operations.
 * This ensures all user types are size aligned and thus are lock free.
 */


/////////////////////////////////////////////////////////////////////////////////


#define EASTL_COMPILER_ATOMIC_HAS_8BIT
#define EASTL_COMPILER_ATOMIC_HAS_16BIT
#define EASTL_COMPILER_ATOMIC_HAS_32BIT
#define EASTL_COMPILER_ATOMIC_HAS_64BIT

#if EA_PLATFORM_PTR_SIZE == 8
	#define EASTL_COMPILER_ATOMIC_HAS_128BIT
#endif


/////////////////////////////////////////////////////////////////////////////////


#define EASTL_COMPILER_ATOMIC_FIXED_WIDTH_TYPE_8 uint8_t
#define EASTL_COMPILER_ATOMIC_FIXED_WIDTH_TYPE_16 uint16_t
#define EASTL_COMPILER_ATOMIC_FIXED_WIDTH_TYPE_32 uint32_t
#define EASTL_COMPILER_ATOMIC_FIXED_WIDTH_TYPE_64 uint64_t
#define EASTL_COMPILER_ATOMIC_FIXED_WIDTH_TYPE_128 __uint128_t


/////////////////////////////////////////////////////////////////////////////////


#define EASTL_GCC_ATOMIC_FETCH_INTRIN_N(integralType, fetchIntrinsic, type, ret, ptr, val, gccMemoryOrder) \
	{																	\
		integralType retIntegral;										\
		integralType valIntegral = EASTL_ATOMIC_TYPE_PUN_CAST(integralType, (val)); \
																		\
		retIntegral = fetchIntrinsic(EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(integralType, (ptr)), valIntegral, gccMemoryOrder); \
																		\
		ret = EASTL_ATOMIC_TYPE_PUN_CAST(type, retIntegral);			\
	}

#define EASTL_GCC_ATOMIC_CMPXCHG_INTRIN_N(integralType, type, ret, ptr, expected, desired, weak, successOrder, failOrder) \
	ret = __atomic_compare_exchange(EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(integralType, (ptr)),							  \
									EASTL_ATOMIC_INTEGRAL_CAST(integralType, (expected)), 								  \
									EASTL_ATOMIC_INTEGRAL_CAST(integralType, &(desired)), 								  \
									weak, successOrder, failOrder)

#define EASTL_GCC_ATOMIC_EXCHANGE_INTRIN_N(integralType, type, ret, ptr, val, gccMemoryOrder) \
	{																	\
		integralType retIntegral;										\
		integralType valIntegral = EASTL_ATOMIC_TYPE_PUN_CAST(integralType, (val)); \
																		\
		__atomic_exchange(EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(integralType, (ptr)), \
						  &valIntegral, &retIntegral, gccMemoryOrder);	\
																		\
		ret = EASTL_ATOMIC_TYPE_PUN_CAST(type, retIntegral);			\
	}


/////////////////////////////////////////////////////////////////////////////////


#include "compiler_gcc_fetch_add.h"
#include "compiler_gcc_fetch_sub.h"

#include "compiler_gcc_fetch_and.h"
#include "compiler_gcc_fetch_xor.h"
#include "compiler_gcc_fetch_or.h"

#include "compiler_gcc_add_fetch.h"
#include "compiler_gcc_sub_fetch.h"

#include "compiler_gcc_and_fetch.h"
#include "compiler_gcc_xor_fetch.h"
#include "compiler_gcc_or_fetch.h"

#include "compiler_gcc_exchange.h"

#include "compiler_gcc_cmpxchg_weak.h"
#include "compiler_gcc_cmpxchg_strong.h"

#include "compiler_gcc_load.h"
#include "compiler_gcc_store.h"

#include "compiler_gcc_barrier.h"

#include "compiler_gcc_cpu_pause.h"

#include "compiler_gcc_signal_fence.h"

#include "compiler_gcc_thread_fence.h"


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_GCC_H */
