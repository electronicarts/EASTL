/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_FETCH_OR_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_FETCH_OR_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


#if defined(EA_PROCESSOR_X86_64)

	#define EASTL_MSVC_ATOMIC_FETCH_OR_INTRIN_8 _InterlockedOr8_np
	#define EASTL_MSVC_ATOMIC_FETCH_OR_INTRIN_16 _InterlockedOr16_np
	#define EASTL_MSVC_ATOMIC_FETCH_OR_INTRIN_32 _InterlockedOr_np
	#define EASTL_MSVC_ATOMIC_FETCH_OR_INTRIN_64 _InterlockedOr64_np

#else

	#define EASTL_MSVC_ATOMIC_FETCH_OR_INTRIN_8 _InterlockedOr8
	#define EASTL_MSVC_ATOMIC_FETCH_OR_INTRIN_16 _InterlockedOr16
	#define EASTL_MSVC_ATOMIC_FETCH_OR_INTRIN_32 _InterlockedOr
	#define EASTL_MSVC_ATOMIC_FETCH_OR_INTRIN_64 _InterlockedOr64

#endif


#define EASTL_MSVC_ATOMIC_FETCH_OR_N(integralType, orIntrinsic, type, ret, ptr, val, MemoryOrder) \
	EASTL_MSVC_ATOMIC_FETCH_OP_N(integralType, orIntrinsic, type, ret, ptr, val, MemoryOrder, \
								 EASTL_MSVC_NOP_PRE_INTRIN_COMPUTE)


#define EASTL_MSVC_ATOMIC_FETCH_OR_8(type, ret, ptr, val, MemoryOrder)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_N(char, EASTL_MSVC_ATOMIC_FETCH_OR_INTRIN_8, type, ret, ptr, val, MemoryOrder)

#define EASTL_MSVC_ATOMIC_FETCH_OR_16(type, ret, ptr, val, MemoryOrder)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_N(short, EASTL_MSVC_ATOMIC_FETCH_OR_INTRIN_16, type, ret, ptr, val, MemoryOrder)

#define EASTL_MSVC_ATOMIC_FETCH_OR_32(type, ret, ptr, val, MemoryOrder)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_N(long, EASTL_MSVC_ATOMIC_FETCH_OR_INTRIN_32, type, ret, ptr, val, MemoryOrder)

#define EASTL_MSVC_ATOMIC_FETCH_OR_64(type, ret, ptr, val, MemoryOrder)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_N(long long, EASTL_MSVC_ATOMIC_FETCH_OR_INTRIN_64, type, ret, ptr, val, MemoryOrder)


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_COMPILER_ATOMIC_FETCH_OR_*_N(type, type ret, type * ptr, type val)
//
#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELAXED_8(type, ret, ptr, val) \
	EASTL_MSVC_ATOMIC_FETCH_OR_8(type, ret, ptr, val, RELAXED)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELAXED_16(type, ret, ptr, val) \
	EASTL_MSVC_ATOMIC_FETCH_OR_16(type, ret, ptr, val, RELAXED)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELAXED_32(type, ret, ptr, val) \
	EASTL_MSVC_ATOMIC_FETCH_OR_32(type, ret, ptr, val, RELAXED)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELAXED_64(type, ret, ptr, val) \
	EASTL_MSVC_ATOMIC_FETCH_OR_64(type, ret, ptr, val, RELAXED)


#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQUIRE_8(type, ret, ptr, val) \
	EASTL_MSVC_ATOMIC_FETCH_OR_8(type, ret, ptr, val, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQUIRE_16(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_16(type, ret, ptr, val, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQUIRE_32(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_32(type, ret, ptr, val, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQUIRE_64(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_64(type, ret, ptr, val, ACQUIRE)


#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELEASE_8(type, ret, ptr, val) \
	EASTL_MSVC_ATOMIC_FETCH_OR_8(type, ret, ptr, val, RELEASE)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELEASE_16(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_16(type, ret, ptr, val, RELEASE)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELEASE_32(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_32(type, ret, ptr, val, RELEASE)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELEASE_64(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_64(type, ret, ptr, val, RELEASE)


#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQ_REL_8(type, ret, ptr, val) \
	EASTL_MSVC_ATOMIC_FETCH_OR_8(type, ret, ptr, val, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQ_REL_16(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_16(type, ret, ptr, val, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQ_REL_32(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_32(type, ret, ptr, val, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQ_REL_64(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_64(type, ret, ptr, val, ACQ_REL)


#define EASTL_COMPILER_ATOMIC_FETCH_OR_SEQ_CST_8(type, ret, ptr, val) \
	EASTL_MSVC_ATOMIC_FETCH_OR_8(type, ret, ptr, val, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_SEQ_CST_16(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_16(type, ret, ptr, val, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_SEQ_CST_32(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_32(type, ret, ptr, val, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_SEQ_CST_64(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_FETCH_OR_64(type, ret, ptr, val, SEQ_CST)


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_FETCH_OR_H */
