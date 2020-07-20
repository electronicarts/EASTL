/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_GCC_CPU_PAUSE_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_GCC_CPU_PAUSE_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_COMPILER_ATOMIC_CPU_PAUSE()
//
#if defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64)

	#define EASTL_COMPILER_ATOMIC_CPU_PAUSE()		\
		__asm__ __volatile__ ("pause")

#elif defined(EA_PROCESSOR_ARM32) || defined(EA_PROCESSOR_ARM64)

	#define EASTL_COMPILER_ATOMIC_CPU_PAUSE()		\
		__asm__ __volatile__ ("yield")

#endif


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_GCC_CPU_PAUSE_H */
