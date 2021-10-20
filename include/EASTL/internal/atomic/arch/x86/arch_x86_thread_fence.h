/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_ARCH_X86_THREAD_FENCE_H
#define EASTL_ATOMIC_INTERNAL_ARCH_X86_THREAD_FENCE_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ARCH_ATOMIC_THREAD_FENCE_*()
//
#if defined(EA_COMPILER_MSVC)

	#define EASTL_ARCH_ATOMIC_THREAD_FENCE_RELAXED()

	#define EASTL_ARCH_ATOMIC_THREAD_FENCE_ACQUIRE()	\
		EASTL_ATOMIC_COMPILER_BARRIER()

	#define EASTL_ARCH_ATOMIC_THREAD_FENCE_RELEASE()	\
		EASTL_ATOMIC_COMPILER_BARRIER()

	#define EASTL_ARCH_ATOMIC_THREAD_FENCE_ACQ_REL()	\
		EASTL_ATOMIC_COMPILER_BARRIER()

#endif


#if defined(EA_COMPILER_MSVC) || defined(__clang__) || defined(EA_COMPILER_GNUC)

	#define EASTL_ARCH_ATOMIC_THREAD_FENCE_SEQ_CST()	\
		EASTL_ATOMIC_CPU_MB()

#endif


#endif /* EASTL_ATOMIC_INTERNAL_ARCH_X86_THREAD_FENCE_H */
