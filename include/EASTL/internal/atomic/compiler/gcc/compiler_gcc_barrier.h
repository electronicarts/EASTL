/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_GCC_BARRIER_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_GCC_BARRIER_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_COMPILER_ATOMIC_COMPILER_BARRIER()
//
#define EASTL_COMPILER_ATOMIC_COMPILER_BARRIER()	\
	__asm__ __volatile__ ("" ::: "memory")


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_COMPILER_ATOMIC_COMPILER_BARRIER_DATA_DEPENDENCY(const T&, type)
//
#define EASTL_COMPILER_ATOMIC_COMPILER_BARRIER_DATA_DEPENDENCY(val, type) \
	__asm__ __volatile__ ("" : /* Output Operands */ : "r"(&(val)) : "memory")


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_GCC_BARRIER_H */
