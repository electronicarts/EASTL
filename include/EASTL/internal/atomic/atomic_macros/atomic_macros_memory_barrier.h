/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_MACROS_MEMORY_BARRIER_H
#define EASTL_ATOMIC_INTERNAL_MACROS_MEMORY_BARRIER_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ATOMIC_CPU_MB()
//
#define EASTL_ATOMIC_CPU_MB()						\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CPU_MB)()


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ATOMIC_CPU_WMB()
//
#define EASTL_ATOMIC_CPU_WMB()						\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CPU_WMB)()


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ATOMIC_CPU_RMB()
//
#define EASTL_ATOMIC_CPU_RMB()						\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CPU_RMB)()


#endif /* EASTL_ATOMIC_INTERNAL_MACROS_MEMORY_BARRIER_H */
