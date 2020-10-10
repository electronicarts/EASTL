/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_GCC_SIGNAL_FENCE_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_GCC_SIGNAL_FENCE_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


#define EASTL_GCC_ATOMIC_SIGNAL_FENCE(gccMemoryOrder)	\
	__atomic_signal_fence(gccMemoryOrder)


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_COMPILER_ATOMIC_SIGNAL_FENCE_*()
//
#define EASTL_COMPILER_ATOMIC_SIGNAL_FENCE_RELAXED()	\
	EASTL_GCC_ATOMIC_SIGNAL_FENCE(__ATOMIC_RELAXED)

#define EASTL_COMPILER_ATOMIC_SIGNAL_FENCE_ACQUIRE()	\
	EASTL_GCC_ATOMIC_SIGNAL_FENCE(__ATOMIC_ACQUIRE)

#define EASTL_COMPILER_ATOMIC_SIGNAL_FENCE_RELEASE()	\
	EASTL_GCC_ATOMIC_SIGNAL_FENCE(__ATOMIC_RELEASE)

#define EASTL_COMPILER_ATOMIC_SIGNAL_FENCE_ACQ_REL()	\
	EASTL_GCC_ATOMIC_SIGNAL_FENCE(__ATOMIC_ACQ_REL)

#define EASTL_COMPILER_ATOMIC_SIGNAL_FENCE_SEQ_CST()	\
	EASTL_GCC_ATOMIC_SIGNAL_FENCE(__ATOMIC_SEQ_CST)


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_GCC_SIGNAL_FENCE_H */
