/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_ARCH_H
#define EASTL_ATOMIC_INTERNAL_ARCH_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// Include the architecture specific implementations
//
#if defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64)

	#include "x86/arch_x86.h"

#elif defined(EA_PROCESSOR_ARM32) || defined(EA_PROCESSOR_ARM64)

	#include "arm/arch_arm.h"

#endif


/////////////////////////////////////////////////////////////////////////////////


#include "arch_fetch_add.h"
#include "arch_fetch_sub.h"

#include "arch_fetch_and.h"
#include "arch_fetch_xor.h"
#include "arch_fetch_or.h"

#include "arch_add_fetch.h"
#include "arch_sub_fetch.h"

#include "arch_and_fetch.h"
#include "arch_xor_fetch.h"
#include "arch_or_fetch.h"

#include "arch_exchange.h"

#include "arch_cmpxchg_weak.h"
#include "arch_cmpxchg_strong.h"

#include "arch_load.h"
#include "arch_store.h"

#include "arch_compiler_barrier.h"

#include "arch_cpu_pause.h"

#include "arch_memory_barrier.h"

#include "arch_signal_fence.h"

#include "arch_thread_fence.h"


#endif /* EASTL_ATOMIC_INTERNAL_ARCH_H */
