/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_ARCH_ARM_MEMORY_BARRIER_H
#define EASTL_ATOMIC_INTERNAL_ARCH_ARM_MEMORY_BARRIER_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


#if defined(EA_COMPILER_MSVC) && !defined(EA_COMPILER_CLANG_CL)

	#if defined(EA_PROCESSOR_ARM32)

		#define EASTL_ARM_DMB_ISH _ARM_BARRIER_ISH

		#define EASTL_ARM_DMB_ISHST _ARM_BARRIER_ISHST

		#define EASTL_ARM_DMB_ISHLD _ARM_BARRIER_ISH

	#elif defined(EA_PROCESSOR_ARM64)

		#define EASTL_ARM_DMB_ISH _ARM64_BARRIER_ISH

		#define EASTL_ARM_DMB_ISHST _ARM64_BARRIER_ISHST

		#define EASTL_ARM_DMB_ISHLD _ARM64_BARRIER_ISHLD

	#endif


	/**
	 * NOTE:
	 *
	 * While it makes no sense for a hardware memory barrier to not imply a compiler barrier.
	 * MSVC docs do not explicitly state that, so better to be safe than sorry chasing down
	 * hard to find bugs due to the compiler deciding to reorder things.
	 */

	#define EASTL_ARCH_ATOMIC_ARM_EMIT_DMB(option)	\
		EASTL_ATOMIC_COMPILER_BARRIER();			\
		__dmb(option);								\
		EASTL_ATOMIC_COMPILER_BARRIER()


#elif defined(EA_COMPILER_GNUC) || defined(__clang__)

	#define EASTL_ARM_DMB_ISH ish

	#define EASTL_ARM_DMB_ISHST ishst

	#if defined(EA_PROCESSOR_ARM32)

		#define EASTL_ARM_DMB_ISHLD ish

	#elif defined(EA_PROCESSOR_ARM64)

		#define EASTL_ARM_DMB_ISHLD ishld

	#endif


	#define EASTL_ARCH_ATOMIC_ARM_EMIT_DMB(option)										\
		__asm__ __volatile__ ("dmb " EA_STRINGIFY(option) ::: "memory")


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ARCH_ATOMIC_CPU_MB()
//
#define EASTL_ARCH_ATOMIC_CPU_MB()				\
	EASTL_ARCH_ATOMIC_ARM_EMIT_DMB(EASTL_ARM_DMB_ISH)


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ARCH_ATOMIC_CPU_WMB()
//
#define EASTL_ARCH_ATOMIC_CPU_WMB()				\
	EASTL_ARCH_ATOMIC_ARM_EMIT_DMB(EASTL_ARM_DMB_ISHST)


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ARCH_ATOMIC_CPU_RMB()
//
#define EASTL_ARCH_ATOMIC_CPU_RMB()				\
	EASTL_ARCH_ATOMIC_ARM_EMIT_DMB(EASTL_ARM_DMB_ISHLD)


#endif /* EASTL_ATOMIC_INTERNAL_ARCH_ARM_MEMORY_BARRIER_H */
