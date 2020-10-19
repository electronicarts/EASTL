/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// Include the compiler specific implementations
//
#if defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)

	#include "gcc/compiler_gcc.h"

#elif defined(EA_COMPILER_MSVC)

	#include "msvc/compiler_msvc.h"

#endif


/////////////////////////////////////////////////////////////////////////////////


namespace eastl
{


namespace internal
{


/**
 * NOTE:
 *
 * This can be used by specific compiler implementations to implement a data dependency compiler barrier.
 * Some compiler barriers do not take in input dependencies as is possible with the gcc asm syntax.
 * Thus we need a way to create a false dependency on the input variable so the compiler does not dead-store
 * remove it.
 * A volatile function pointer ensures the compiler must always load the function pointer and call thru it
 * since the compiler cannot reason about any side effects. Thus the compiler must always assume the
 * input variable may be accessed and thus cannot be dead-stored. This technique works even in the presence
 * of Link-Time Optimization. A compiler barrier with a data dependency is useful in these situations.
 *
 * void foo()
 * {
 *    eastl::vector<int> v;
 *    while (Benchmark.ContinueRunning())
 *    {
 *      v.push_back(0);
 *      eastl::compiler_barrier(); OR eastl::compiler_barrier_data_dependency(v);
 *    }
 * }
 *
 * We are trying to benchmark the push_back function of a vector. The vector v has only local scope.
 * The compiler is well within its writes to remove all accesses to v even with the compiler barrier
 * because there are no observable uses of the vector v.
 * The compiler barrier data dependency ensures there is an input dependency on the variable so that
 * it isn't removed. This is also useful when writing test code that the compiler may remove.
 */

typedef void (*CompilerBarrierDataDependencyFuncPtr)(void*);

extern EASTL_API volatile CompilerBarrierDataDependencyFuncPtr gCompilerBarrierDataDependencyFunc;


#define EASTL_COMPILER_ATOMIC_COMPILER_BARRIER_DATA_DEPENDENCY_FUNC(ptr) \
	eastl::internal::gCompilerBarrierDataDependencyFunc(ptr)


} // namespace internal


} // namespace eastl


/////////////////////////////////////////////////////////////////////////////////


#include "compiler_fetch_add.h"
#include "compiler_fetch_sub.h"

#include "compiler_fetch_and.h"
#include "compiler_fetch_xor.h"
#include "compiler_fetch_or.h"

#include "compiler_add_fetch.h"
#include "compiler_sub_fetch.h"

#include "compiler_and_fetch.h"
#include "compiler_xor_fetch.h"
#include "compiler_or_fetch.h"

#include "compiler_exchange.h"

#include "compiler_cmpxchg_weak.h"
#include "compiler_cmpxchg_strong.h"

#include "compiler_load.h"
#include "compiler_store.h"

#include "compiler_barrier.h"

#include "compiler_cpu_pause.h"

#include "compiler_memory_barrier.h"

#include "compiler_signal_fence.h"

#include "compiler_thread_fence.h"


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_H */
