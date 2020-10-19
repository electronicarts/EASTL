/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_ADD_FETCH_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_ADD_FETCH_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


#define EASTL_MSVC_ADD_FETCH_POST_INTRIN_COMPUTE(ret, val, addend)	\
	ret = (val) + (addend)

#define EASTL_MSVC_ATOMIC_ADD_FETCH_N(integralType, addIntrinsic, type, ret, ptr, val, MemoryOrder) \
	EASTL_MSVC_ATOMIC_OP_FETCH_N(integralType, addIntrinsic, type, ret, ptr, val, MemoryOrder, \
								 EASTL_MSVC_NOP_PRE_INTRIN_COMPUTE, EASTL_MSVC_ADD_FETCH_POST_INTRIN_COMPUTE)


#define EASTL_MSVC_ATOMIC_ADD_FETCH_8(type, ret, ptr, val, MemoryOrder)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_N(char, _InterlockedExchangeAdd8, type, ret, ptr, val, MemoryOrder)

#define EASTL_MSVC_ATOMIC_ADD_FETCH_16(type, ret, ptr, val, MemoryOrder) \
	EASTL_MSVC_ATOMIC_ADD_FETCH_N(short, _InterlockedExchangeAdd16, type, ret, ptr, val, MemoryOrder)

#define EASTL_MSVC_ATOMIC_ADD_FETCH_32(type, ret, ptr, val, MemoryOrder) \
	EASTL_MSVC_ATOMIC_ADD_FETCH_N(long, _InterlockedExchangeAdd, type, ret, ptr, val, MemoryOrder)

#define EASTL_MSVC_ATOMIC_ADD_FETCH_64(type, ret, ptr, val, MemoryOrder) \
	EASTL_MSVC_ATOMIC_ADD_FETCH_N(__int64, _InterlockedExchangeAdd64, type, ret, ptr, val, MemoryOrder)


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_COMPILER_ATOMIC_ADD_FETCH_*_N(type, type ret, type * ptr, type val)
//
#define EASTL_COMPILER_ATOMIC_ADD_FETCH_RELAXED_8(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_8(type, ret, ptr, val, RELAXED)

#define EASTL_COMPILER_ATOMIC_ADD_FETCH_RELAXED_16(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_16(type, ret, ptr, val, RELAXED)

#define EASTL_COMPILER_ATOMIC_ADD_FETCH_RELAXED_32(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_32(type, ret, ptr, val, RELAXED)

#define EASTL_COMPILER_ATOMIC_ADD_FETCH_RELAXED_64(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_64(type, ret, ptr, val, RELAXED)


#define EASTL_COMPILER_ATOMIC_ADD_FETCH_ACQUIRE_8(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_8(type, ret, ptr, val, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_ADD_FETCH_ACQUIRE_16(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_16(type, ret, ptr, val, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_ADD_FETCH_ACQUIRE_32(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_32(type, ret, ptr, val, ACQUIRE)

#define EASTL_COMPILER_ATOMIC_ADD_FETCH_ACQUIRE_64(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_64(type, ret, ptr, val, ACQUIRE)


#define EASTL_COMPILER_ATOMIC_ADD_FETCH_RELEASE_8(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_8(type, ret, ptr, val, RELEASE)

#define EASTL_COMPILER_ATOMIC_ADD_FETCH_RELEASE_16(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_16(type, ret, ptr, val, RELEASE)

#define EASTL_COMPILER_ATOMIC_ADD_FETCH_RELEASE_32(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_32(type, ret, ptr, val, RELEASE)

#define EASTL_COMPILER_ATOMIC_ADD_FETCH_RELEASE_64(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_64(type, ret, ptr, val, RELEASE)


#define EASTL_COMPILER_ATOMIC_ADD_FETCH_ACQ_REL_8(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_8(type, ret, ptr, val, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_ADD_FETCH_ACQ_REL_16(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_16(type, ret, ptr, val, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_ADD_FETCH_ACQ_REL_32(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_32(type, ret, ptr, val, ACQ_REL)

#define EASTL_COMPILER_ATOMIC_ADD_FETCH_ACQ_REL_64(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_64(type, ret, ptr, val, ACQ_REL)


#define EASTL_COMPILER_ATOMIC_ADD_FETCH_SEQ_CST_8(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_8(type, ret, ptr, val, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_ADD_FETCH_SEQ_CST_16(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_16(type, ret, ptr, val, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_ADD_FETCH_SEQ_CST_32(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_32(type, ret, ptr, val, SEQ_CST)

#define EASTL_COMPILER_ATOMIC_ADD_FETCH_SEQ_CST_64(type, ret, ptr, val)	\
	EASTL_MSVC_ATOMIC_ADD_FETCH_64(type, ret, ptr, val, SEQ_CST)


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_ADD_FETCH_H */
