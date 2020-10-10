/////////////////////////////////////////////////////////////////////////////////
// copyright (c) electronic arts inc. all rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_CPU_PAUSE_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_CPU_PAUSE_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_COMPILER_ATOMIC_CPU_PAUSE()
//
// NOTE:
// Rather obscure macro in Windows.h that expands to pause or rep; nop on
// compatible x86 cpus or the arm yield on compatible arm processors.
// This is nicer than switching on platform specific intrinsics.
//
#define EASTL_COMPILER_ATOMIC_CPU_PAUSE()		\
	YieldProcessor()


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_CPU_PAUSE_H */
