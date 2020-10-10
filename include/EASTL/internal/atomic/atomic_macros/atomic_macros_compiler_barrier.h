/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_MACROS_COMPILER_BARRIER_H
#define EASTL_ATOMIC_INTERNAL_MACROS_COMPILER_BARRIER_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ATOMIC_COMPILER_BARRIER()
//
#define EASTL_ATOMIC_COMPILER_BARRIER()						\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_COMPILER_BARRIER)()


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ATOMIC_COMPILER_BARRIER_DATA_DEPENDENCY(const T&, type)
//
#define EASTL_ATOMIC_COMPILER_BARRIER_DATA_DEPENDENCY(val, type)		\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_COMPILER_BARRIER_DATA_DEPENDENCY)(val, type)


#endif /* EASTL_ATOMIC_INTERNAL_MACROS_COMPILER_BARRIER_H */
