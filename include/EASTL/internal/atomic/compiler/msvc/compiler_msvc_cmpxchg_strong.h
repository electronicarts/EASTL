/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_CMPXCHG_STRONG_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_CMPXCHG_STRONG_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


#define EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_8(type, ret, ptr, expected, desired, MemoryOrder) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_OP_N(char, _InterlockedCompareExchange8, type, ret, ptr, expected, desired, MemoryOrder)

#define EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_16(type, ret, ptr, expected, desired, MemoryOrder) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_OP_N(short, _InterlockedCompareExchange16, type, ret, ptr, expected, desired, MemoryOrder)

#define EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_32(type, ret, ptr, expected, desired, MemoryOrder) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_OP_N(long, _InterlockedCompareExchange, type, ret, ptr, expected, desired, MemoryOrder)

#define EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_64(type, ret, ptr, expected, desired, MemoryOrder) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_OP_N(long long, _InterlockedCompareExchange64, type, ret, ptr, expected, desired, MemoryOrder)

#define EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_128(type, ret, ptr, expected, desired, MemoryOrder) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_OP_128(type, ret, ptr, expected, desired, MemoryOrder)


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_*_*_N(type, bool ret, type * ptr, type * expected, type desired)
//
#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_RELAXED_RELAXED_8(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_8(type, ret, ptr, expected, desired, RELAXED)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_RELAXED_RELAXED_16(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_16(type, ret, ptr, expected, desired, RELAXED)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_RELAXED_RELAXED_32(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_32(type, ret, ptr, expected, desired, RELAXED)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_RELAXED_RELAXED_64(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_64(type, ret, ptr, expected, desired, RELAXED)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_RELAXED_RELAXED_128(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_128(type, ret, ptr, expected, desired, RELAXED)


#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQUIRE_RELAXED_8(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_8(type, ret, ptr, expected, desired, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQUIRE_RELAXED_16(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_16(type, ret, ptr, expected, desired, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQUIRE_RELAXED_32(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_32(type, ret, ptr, expected, desired, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQUIRE_RELAXED_64(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_64(type, ret, ptr, expected, desired, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQUIRE_RELAXED_128(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_128(type, ret, ptr, expected, desired, ACQUIRE)


#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQUIRE_ACQUIRE_8(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_8(type, ret, ptr, expected, desired, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQUIRE_ACQUIRE_16(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_16(type, ret, ptr, expected, desired, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQUIRE_ACQUIRE_32(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_32(type, ret, ptr, expected, desired, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQUIRE_ACQUIRE_64(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_64(type, ret, ptr, expected, desired, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQUIRE_ACQUIRE_128(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_128(type, ret, ptr, expected, desired, ACQUIRE)


#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_RELEASE_RELAXED_8(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_8(type, ret, ptr, expected, desired, RELEASE)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_RELEASE_RELAXED_16(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_16(type, ret, ptr, expected, desired, RELEASE)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_RELEASE_RELAXED_32(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_32(type, ret, ptr, expected, desired, RELEASE)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_RELEASE_RELAXED_64(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_64(type, ret, ptr, expected, desired, RELEASE)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_RELEASE_RELAXED_128(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_128(type, ret, ptr, expected, desired, RELEASE)


#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQ_REL_RELAXED_8(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_8(type, ret, ptr, expected, desired, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQ_REL_RELAXED_16(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_16(type, ret, ptr, expected, desired, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQ_REL_RELAXED_32(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_32(type, ret, ptr, expected, desired, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQ_REL_RELAXED_64(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_64(type, ret, ptr, expected, desired, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQ_REL_RELAXED_128(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_128(type, ret, ptr, expected, desired, ACQ_REL)


#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQ_REL_ACQUIRE_8(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_8(type, ret, ptr, expected, desired, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQ_REL_ACQUIRE_16(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_16(type, ret, ptr, expected, desired, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQ_REL_ACQUIRE_32(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_32(type, ret, ptr, expected, desired, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQ_REL_ACQUIRE_64(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_64(type, ret, ptr, expected, desired, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_ACQ_REL_ACQUIRE_128(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_128(type, ret, ptr, expected, desired, ACQ_REL)


#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_SEQ_CST_RELAXED_8(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_8(type, ret, ptr, expected, desired, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_SEQ_CST_RELAXED_16(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_16(type, ret, ptr, expected, desired, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_SEQ_CST_RELAXED_32(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_32(type, ret, ptr, expected, desired, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_SEQ_CST_RELAXED_64(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_64(type, ret, ptr, expected, desired, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_SEQ_CST_RELAXED_128(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_128(type, ret, ptr, expected, desired, SEQ_CST)


#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_SEQ_CST_ACQUIRE_8(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_8(type, ret, ptr, expected, desired, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_SEQ_CST_ACQUIRE_16(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_16(type, ret, ptr, expected, desired, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_SEQ_CST_ACQUIRE_32(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_32(type, ret, ptr, expected, desired, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_SEQ_CST_ACQUIRE_64(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_64(type, ret, ptr, expected, desired, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_SEQ_CST_ACQUIRE_128(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_128(type, ret, ptr, expected, desired, SEQ_CST)


#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_SEQ_CST_SEQ_CST_8(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_8(type, ret, ptr, expected, desired, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_SEQ_CST_SEQ_CST_16(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_16(type, ret, ptr, expected, desired, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_SEQ_CST_SEQ_CST_32(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_32(type, ret, ptr, expected, desired, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_SEQ_CST_SEQ_CST_64(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_64(type, ret, ptr, expected, desired, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_CMPXCHG_STRONG_SEQ_CST_SEQ_CST_128(type, ret, ptr, expected, desired) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_128(type, ret, ptr, expected, desired, SEQ_CST)


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_CMPXCHG_STRONG_H */
