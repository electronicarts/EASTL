/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_XOR_FETCH_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_XOR_FETCH_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


#if defined(EA_PROCESSOR_X86_64)

	#define EASTL_MSVC_ATOMIC_XOR_FETCH_INTRIN_8 _InterlockedXor8_np
	#define EASTL_MSVC_ATOMIC_XOR_FETCH_INTRIN_16 _InterlockedXor16_np
	#define EASTL_MSVC_ATOMIC_XOR_FETCH_INTRIN_32 _InterlockedXor_np
	#define EASTL_MSVC_ATOMIC_XOR_FETCH_INTRIN_64 _InterlockedXor64_np

#else

	#define EASTL_MSVC_ATOMIC_XOR_FETCH_INTRIN_8 _InterlockedXor8
	#define EASTL_MSVC_ATOMIC_XOR_FETCH_INTRIN_16 _InterlockedXor16
	#define EASTL_MSVC_ATOMIC_XOR_FETCH_INTRIN_32 _InterlockedXor
	#define EASTL_MSVC_ATOMIC_XOR_FETCH_INTRIN_64 _InterlockedXor64

#endif


#define EASTL_MSVC_XOR_FETCH_POST_INTRIN_COMPUTE(ret, val, xorend)	\
	ret = (val) ^ (xorend)

#define EASTL_MSVC_ATOMIC_XOR_FETCH_N(integralType, xorIntrinsic, type, ret, ptr, val, MemoryOrder) \
	EASTL_MSVC_ATOMIC_OP_FETCH_N(integralType, xorIntrinsic, type, ret, ptr, val, MemoryOrder, \
								 EASTL_MSVC_NOP_PRE_INTRIN_COMPUTE, EASTL_MSVC_XOR_FETCH_POST_INTRIN_COMPUTE)


#define EASTL_MSVC_ATOMIC_XOR_FETCH_8(type, ret, ptr, val, MemoryOrder)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_N(char, EASTL_MSVC_ATOMIC_XOR_FETCH_INTRIN_8, type, ret, ptr, val, MemoryOrder)

#define EASTL_MSVC_ATOMIC_XOR_FETCH_16(type, ret, ptr, val, MemoryOrder) \
	EASTL_MSVC_ATOMIC_XOR_FETCH_N(short, EASTL_MSVC_ATOMIC_XOR_FETCH_INTRIN_16, type, ret, ptr, val, MemoryOrder)

#define EASTL_MSVC_ATOMIC_XOR_FETCH_32(type, ret, ptr, val, MemoryOrder) \
	EASTL_MSVC_ATOMIC_XOR_FETCH_N(long, EASTL_MSVC_ATOMIC_XOR_FETCH_INTRIN_32, type, ret, ptr, val, MemoryOrder)

#define EASTL_MSVC_ATOMIC_XOR_FETCH_64(type, ret, ptr, val, MemoryOrder) \
	EASTL_MSVC_ATOMIC_XOR_FETCH_N(__int64, EASTL_MSVC_ATOMIC_XOR_FETCH_INTRIN_64, type, ret, ptr, val, MemoryOrder)


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_COMPILER_ATOMIC_XOR_FETCH_*_N(type, type ret, type * ptr, type val)
//
#define EASTL_COMPILER_ATOMIC_XOR_FETCH_RELAXED_8(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_8(type, ret, ptr, val, RELAXED)

#define EASTL_COMPILER_ATOMIC_XOR_FETCH_RELAXED_16(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_16(type, ret, ptr, val, RELAXED)

#define EASTL_COMPILER_ATOMIC_XOR_FETCH_RELAXED_32(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_32(type, ret, ptr, val, RELAXED)

#define EASTL_COMPILER_ATOMIC_XOR_FETCH_RELAXED_64(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_64(type, ret, ptr, val, RELAXED)


#define EASTL_COMPILER_ATOMIC_XOR_FETCH_ACQUIRE_8(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_8(type, ret, ptr, val, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_XOR_FETCH_ACQUIRE_16(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_16(type, ret, ptr, val, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_XOR_FETCH_ACQUIRE_32(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_32(type, ret, ptr, val, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_XOR_FETCH_ACQUIRE_64(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_64(type, ret, ptr, val, ACQUIRE)


#define EASTL_COMPILER_ATOMIC_XOR_FETCH_RELEASE_8(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_8(type, ret, ptr, val, RELEASE)

#define EASTL_COMPILER_ATOMIC_XOR_FETCH_RELEASE_16(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_16(type, ret, ptr, val, RELEASE)

#define EASTL_COMPILER_ATOMIC_XOR_FETCH_RELEASE_32(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_32(type, ret, ptr, val, RELEASE)

#define EASTL_COMPILER_ATOMIC_XOR_FETCH_RELEASE_64(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_64(type, ret, ptr, val, RELEASE)


#define EASTL_COMPILER_ATOMIC_XOR_FETCH_ACQ_REL_8(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_8(type, ret, ptr, val, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_XOR_FETCH_ACQ_REL_16(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_16(type, ret, ptr, val, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_XOR_FETCH_ACQ_REL_32(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_32(type, ret, ptr, val, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_XOR_FETCH_ACQ_REL_64(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_64(type, ret, ptr, val, ACQ_REL)


#define EASTL_COMPILER_ATOMIC_XOR_FETCH_SEQ_CST_8(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_8(type, ret, ptr, val, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_XOR_FETCH_SEQ_CST_16(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_16(type, ret, ptr, val, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_XOR_FETCH_SEQ_CST_32(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_32(type, ret, ptr, val, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_XOR_FETCH_SEQ_CST_64(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_XOR_FETCH_64(type, ret, ptr, val, SEQ_CST)


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_XOR_FETCH_H */
