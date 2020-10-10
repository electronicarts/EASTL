/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_SIGNAL_FENCE_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_SIGNAL_FENCE_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_COMPILER_ATOMIC_SIGNAL_FENCE_*()
//
#define EASTL_COMPILER_ATOMIC_SIGNAL_FENCE_RELAXED()	\
	EASTL_ATOMIC_COMPILER_BARRIER()

#define EASTL_COMPILER_ATOMIC_SIGNAL_FENCE_ACQUIRE()	\
	EASTL_ATOMIC_COMPILER_BARRIER()

#define EASTL_COMPILER_ATOMIC_SIGNAL_FENCE_RELEASE()	\
	EASTL_ATOMIC_COMPILER_BARRIER()

#define EASTL_COMPILER_ATOMIC_SIGNAL_FENCE_ACQ_REL()	\
	EASTL_ATOMIC_COMPILER_BARRIER()

#define EASTL_COMPILER_ATOMIC_SIGNAL_FENCE_SEQ_CST()	\
	EASTL_ATOMIC_COMPILER_BARRIER()


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_SIGNAL_FENCE_H */
