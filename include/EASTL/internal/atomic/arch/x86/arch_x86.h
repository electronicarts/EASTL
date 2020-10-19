/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_ARCH_X86_H
#define EASTL_ATOMIC_INTERNAL_ARCH_X86_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/**
 * x86 && x64 Mappings
 *
 * Load Relaxed       : MOV
 * Load Acquire       : MOV; COMPILER_BARRIER;
 * Load Seq_Cst       : MOV; COMPILER_BARRIER;
 *
 * Store Relaxed      : MOV
 * Store Release      : COMPILER_BARRIER; MOV;
 * Store Seq_Cst      : LOCK XCHG : MOV; MFENCE;
 *
 * Relaxed Fence      :
 * Acquire Fence      : COMPILER_BARRIER
 * Release Fence      : COMPILER_BARRIER
 * Acq_Rel Fence      : COMPILER_BARRIER
 * Seq_Cst FENCE      : MFENCE
 */


/////////////////////////////////////////////////////////////////////////////////


#if defined(EA_COMPILER_MSVC)

	#if EA_PLATFORM_PTR_SIZE == 8
		#define EASTL_ARCH_ATOMIC_HAS_128BIT
	#endif

#endif


#if ((defined(EA_COMPILER_CLANG) || defined(EA_COMPILER_GNUC)) && defined(EA_PROCESSOR_X86_64))

	#define EASTL_ARCH_ATOMIC_HAS_128BIT

#endif


/////////////////////////////////////////////////////////////////////////////////


/**
 * NOTE:
 *
 * On 32-bit x86 CPUs Intel Pentium and newer, AMD K5 and newer
 * and any i586 class of x86 CPUs support only 64-bit cmpxchg
 * known as cmpxchg8b.
 *
 * On these class of cpus we can guarantee that 64-bit loads/stores are
 * also atomic by using the SSE2 movq, SSE1 movlps, or x87 fild/fstp instructions.
 *
 * We support all other atomic operations
 * on compilers that only provide this 64-bit cmpxchg instruction
 * by wrapping them around the 64-bit cmpxchg8b instruction.
 */
#if defined(EA_COMPILER_MSVC) && defined(EA_PROCESSOR_X86)


	#define EASTL_ARCH_ATOMIC_X86_NOP_PRE_COMPUTE_DESIRED(ret, observed, val) \
		static_assert(false, "EASTL_ARCH_ATOMIC_X86_NOP_PRE_COMPUTE_DESIRED() must be implmented!");

	#define EASTL_ARCH_ATOMIC_X86_NOP_POST_COMPUTE_RET(ret, prevObserved, val)


	#define EASTL_ARCH_ATOMIC_X86_OP_64_IMPL(type, ret, ptr, val, MemoryOrder, PRE_COMPUTE_DESIRED, POST_COMPUTE_RET) \
		{																	\
			bool cmpxchgRet;												\
			EASTL_ATOMIC_LOAD_RELAXED_64(type, ret, ptr);					\
			do																\
			{																\
				type computedDesired;										\
				PRE_COMPUTE_DESIRED(computedDesired, ret, (val));			\
				EA_PREPROCESSOR_JOIN(EA_PREPROCESSOR_JOIN(EASTL_ATOMIC_CMPXCHG_STRONG_, MemoryOrder), _64)(type, cmpxchgRet, ptr, &(ret), computedDesired); \
			} while (!cmpxchgRet);											\
			POST_COMPUTE_RET(ret, ret, (val));								\
		}


#endif


/**
 * NOTE:
 *
 * 64-bit x64 CPUs support only 128-bit cmpxchg known as cmpxchg16b.
 *
 * We support all other atomic operations by wrapping them around
 * the 128-bit cmpxchg16b instruction.
 *
 * 128-bit loads are only atomic by using the cmpxchg16b instruction.
 * SSE 128-bit loads are not guaranteed to be atomic even though some CPUs
 * make them atomic such as AMD Ryzen or Intel SandyBridge.
 */
#if ((defined(EA_COMPILER_CLANG) || defined(EA_COMPILER_GNUC)) && defined(EA_PROCESSOR_X86_64))


	#define EASTL_ARCH_ATOMIC_X86_NOP_PRE_COMPUTE_DESIRED(ret, observed, val) \
		static_assert(false, "EASTL_ARCH_ATOMIC_X86_NOP_PRE_COMPUTE_DESIRED() must be implmented!");

	#define EASTL_ARCH_ATOMIC_X86_NOP_POST_COMPUTE_RET(ret, prevObserved, val)


	#define EASTL_ARCH_ATOMIC_X86_OP_128_IMPL(type, ret, ptr, val, MemoryOrder, PRE_COMPUTE_DESIRED, POST_COMPUTE_RET) \
		{																	\
			bool cmpxchgRet;												\
			/* This is intentionally a non-atomic 128-bit load which may observe shearing. */ \
			/* Either we do not observe *(ptr) but then the cmpxchg will fail and the observed */ \
			/* atomic load will be returned. Or the non-atomic load got lucky and the cmpxchg succeeds */ \
			/* because the observed value equals the value in *(ptr) thus we optimistically do a non-atomic load. */ \
			ret = *(ptr);													\
			do																\
			{																\
				type computedDesired;										\
				PRE_COMPUTE_DESIRED(computedDesired, ret, (val));			\
				EA_PREPROCESSOR_JOIN(EA_PREPROCESSOR_JOIN(EASTL_ATOMIC_CMPXCHG_STRONG_, MemoryOrder), _128)(type, cmpxchgRet, ptr, &(ret), computedDesired); \
			} while (!cmpxchgRet);											\
			POST_COMPUTE_RET(ret, ret, (val));								\
		}


#endif


/////////////////////////////////////////////////////////////////////////////////


#include "arch_x86_fetch_add.h"
#include "arch_x86_fetch_sub.h"

#include "arch_x86_fetch_and.h"
#include "arch_x86_fetch_xor.h"
#include "arch_x86_fetch_or.h"

#include "arch_x86_add_fetch.h"
#include "arch_x86_sub_fetch.h"

#include "arch_x86_and_fetch.h"
#include "arch_x86_xor_fetch.h"
#include "arch_x86_or_fetch.h"

#include "arch_x86_exchange.h"

#include "arch_x86_cmpxchg_weak.h"
#include "arch_x86_cmpxchg_strong.h"

#include "arch_x86_memory_barrier.h"

#include "arch_x86_thread_fence.h"

#include "arch_x86_load.h"
#include "arch_x86_store.h"


#endif /* EASTL_ATOMIC_INTERNAL_ARCH_X86_H */
