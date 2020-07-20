/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_BARRIER_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_BARRIER_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_COMPILER_ATOMIC_COMPILER_BARRIER()
//
#if defined(EASTL_COMPILER_ATOMIC_COMPILER_BARRIER)
	#define EASTL_COMPILER_ATOMIC_COMPILER_BARRIER_AVAILABLE 1
#else
	#define EASTL_COMPILER_ATOMIC_COMPILER_BARRIER_AVAILABLE 0
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_COMPILER_ATOMIC_COMPILER_BARRIER_DATA_DEPENDENCY(const T&, type)
//
#if defined(EASTL_COMPILER_ATOMIC_COMPILER_BARRIER_DATA_DEPENDENCY)
	#define EASTL_COMPILER_ATOMIC_COMPILER_BARRIER_DATA_DEPENDENCY_AVAILABLE 1
#else
	#define EASTL_COMPILER_ATOMIC_COMPILER_BARRIER_DATA_DEPENDENCY_AVAILABLE 0
#endif


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_BARRIER_H */
