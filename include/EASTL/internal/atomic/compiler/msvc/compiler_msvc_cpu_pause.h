/////////////////////////////////////////////////////////////////////////////////
// copyright (c) electronic arts inc. all rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_CPU_PAUSE_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_CPU_PAUSE_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#if defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64)
	#define EASTL_COMPILER_ATOMIC_CPU_PAUSE() _mm_pause()
#elif defined(EA_PROCESSOR_ARM32) || defined(EA_PROCESSOR_ARM64)
	#define EASTL_COMPILER_ATOMIC_CPU_PAUSE() __yield()
#else 
	#error Unsupported CPU architecture for EASTL_COMPILER_ATOMIC_CPU_PAUSE
#endif


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_CPU_PAUSE_H */
