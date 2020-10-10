/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_MACROS_CPU_PAUSE_H
#define EASTL_ATOMIC_INTERNAL_MACROS_CPU_PAUSE_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ATOMIC_CPU_PAUSE()
//
#define EASTL_ATOMIC_CPU_PAUSE()					\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CPU_PAUSE)()


#endif /* EASTL_ATOMIC_INTERNAL_MACROS_CPU_PAUSE_H */
