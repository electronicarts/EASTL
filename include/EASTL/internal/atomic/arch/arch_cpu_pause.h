/////////////////////////////////////////////////////////////////////////////////
// copyright (c) electronic arts inc. all rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_ARCH_CPU_PAUSE_H
#define EASTL_ATOMIC_INTERNAL_ARCH_CPU_PAUSE_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ARCH_ATOMIC_CPU_PAUSE()
//
#if defined(EASTL_ARCH_ATOMIC_CPU_PAUSE)
	#define EASTL_ARCH_ATOMIC_CPU_PAUSE_AVAILABLE 1
#else
	#define EASTL_ARCH_ATOMIC_CPU_PAUSE_AVAILABLE 0
#endif


#endif /* EASTL_ATOMIC_INTERNAL_ARCH_CPU_PAUSE_H */
