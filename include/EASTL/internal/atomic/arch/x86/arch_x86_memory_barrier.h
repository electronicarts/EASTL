/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_ARCH_X86_MEMORY_BARRIER_H
#define EASTL_ATOMIC_INTERNAL_ARCH_X86_MEMORY_BARRIER_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif



/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ARCH_ATOMIC_CPU_MB()
//
#if defined(EA_COMPILER_MSVC)

	/**
	 * NOTE:
	 * While it makes no sense for a hardware memory barrier to not imply a compiler barrier.
	 * MSVC docs do not explicitly state that, so better to be safe than sorry chasing down
	 * hard to find bugs due to the compiler deciding to reorder things.
	 */

	#if 1

		// 4459 : declaration of 'identifier' hides global declaration
		// 4456 : declaration of 'identifier' hides previous local declaration
		#define EASTL_ARCH_ATOMIC_CPU_MB()				\
			{											\
				EA_DISABLE_VC_WARNING(4459 4456);		\
				volatile long _;						\
				_InterlockedExchangeAdd(&_, 0);			\
				EA_RESTORE_VC_WARNING();				\
			}

	#else

		#define EASTL_ARCH_ATOMIC_CPU_MB()				\
			EASTL_ATOMIC_COMPILER_BARRIER();			\
			_mm_mfence();								\
			EASTL_ATOMIC_COMPILER_BARRIER()

	#endif

#elif defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)

	/**
	* NOTE:
	*
	* mfence orders all loads/stores to/from all memory types.
	* We only care about ordinary cacheable memory so lighter weight locked instruction
	* is far faster than a mfence to get a full memory barrier.
	* lock; addl against the top of the stack is good because:
	*     distinct for every thread so prevents false sharing
	*     that cacheline is most likely cache hot
	*
	* We intentionally do it below the stack pointer to avoid false RAW register dependencies,
	* in cases where the compiler reads from the stack pointer after the lock; addl instruction
	*
	* Accounting for Red Zones or Cachelines doesn't provide extra benefit.
	*/

	#if defined(EA_PROCESSOR_X86)

		#define EASTL_ARCH_ATOMIC_CPU_MB()				\
			__asm__ __volatile__ ("lock; addl $0, -4(%%esp)" ::: "memory", "cc")

	#elif defined(EA_PROCESSOR_X86_64)

		#define EASTL_ARCH_ATOMIC_CPU_MB()				\
			__asm__ __volatile__ ("lock; addl $0, -8(%%rsp)" ::: "memory", "cc")

	#else

		#define EASTL_ARCH_ATOMIC_CPU_MB()				\
			__asm__ __volatile__ ("mfence" ::: "memory")

	#endif


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ARCH_ATOMIC_CPU_WMB()
//
#define EASTL_ARCH_ATOMIC_CPU_WMB()				\
	EASTL_ATOMIC_COMPILER_BARRIER()


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ARCH_ATOMIC_CPU_RMB()
//
#define EASTL_ARCH_ATOMIC_CPU_RMB()				\
	EASTL_ATOMIC_COMPILER_BARRIER()


#endif /* EASTL_ATOMIC_INTERNAL_ARCH_X86_MEMORY_BARRIER_H */
