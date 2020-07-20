/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_GCC_FETCH_OR_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_GCC_FETCH_OR_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


#define EASTL_GCC_ATOMIC_FETCH_OR_N(integralType, type, ret, ptr, val, gccMemoryOrder) \
	EASTL_GCC_ATOMIC_FETCH_INTRIN_N(integralType, __atomic_fetch_or, type, ret, ptr, val, gccMemoryOrder)


#define EASTL_GCC_ATOMIC_FETCH_OR_8(type, ret, ptr, val, gccMemoryOrder) \
	EASTL_GCC_ATOMIC_FETCH_OR_N(uint8_t, type, ret, ptr, val, gccMemoryOrder)

#define EASTL_GCC_ATOMIC_FETCH_OR_16(type, ret, ptr, val, gccMemoryOrder) \
	EASTL_GCC_ATOMIC_FETCH_OR_N(uint16_t, type, ret, ptr, val, gccMemoryOrder)

#define EASTL_GCC_ATOMIC_FETCH_OR_32(type, ret, ptr, val, gccMemoryOrder) \
	EASTL_GCC_ATOMIC_FETCH_OR_N(uint32_t, type, ret, ptr, val, gccMemoryOrder)

#define EASTL_GCC_ATOMIC_FETCH_OR_64(type, ret, ptr, val, gccMemoryOrder) \
	EASTL_GCC_ATOMIC_FETCH_OR_N(uint64_t, type, ret, ptr, val, gccMemoryOrder)

#define EASTL_GCC_ATOMIC_FETCH_OR_128(type, ret, ptr, val, gccMemoryOrder) \
	EASTL_GCC_ATOMIC_FETCH_OR_N(__uint128_t, type, ret, ptr, val, gccMemoryOrder)


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_COMPILER_ATOMIC_FETCH_OR_*_N(type, type ret, type * ptr, type val)
//
#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELAXED_8(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_8(type, ret, ptr, val, __ATOMIC_RELAXED)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELAXED_16(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_16(type, ret, ptr, val, __ATOMIC_RELAXED)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELAXED_32(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_32(type, ret, ptr, val, __ATOMIC_RELAXED)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELAXED_64(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_64(type, ret, ptr, val, __ATOMIC_RELAXED)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELAXED_128(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_128(type, ret, ptr, val, __ATOMIC_RELAXED)


#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQUIRE_8(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_8(type, ret, ptr, val, __ATOMIC_ACQUIRE)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQUIRE_16(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_16(type, ret, ptr, val, __ATOMIC_ACQUIRE)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQUIRE_32(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_32(type, ret, ptr, val, __ATOMIC_ACQUIRE)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQUIRE_64(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_64(type, ret, ptr, val, __ATOMIC_ACQUIRE)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQUIRE_128(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_128(type, ret, ptr, val, __ATOMIC_ACQUIRE)


#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELEASE_8(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_8(type, ret, ptr, val, __ATOMIC_RELEASE)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELEASE_16(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_16(type, ret, ptr, val, __ATOMIC_RELEASE)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELEASE_32(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_32(type, ret, ptr, val, __ATOMIC_RELEASE)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELEASE_64(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_64(type, ret, ptr, val, __ATOMIC_RELEASE)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_RELEASE_128(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_128(type, ret, ptr, val, __ATOMIC_RELEASE)


#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQ_REL_8(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_8(type, ret, ptr, val, __ATOMIC_ACQ_REL)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQ_REL_16(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_16(type, ret, ptr, val, __ATOMIC_ACQ_REL)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQ_REL_32(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_32(type, ret, ptr, val, __ATOMIC_ACQ_REL)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQ_REL_64(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_64(type, ret, ptr, val, __ATOMIC_ACQ_REL)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_ACQ_REL_128(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_128(type, ret, ptr, val, __ATOMIC_ACQ_REL)


#define EASTL_COMPILER_ATOMIC_FETCH_OR_SEQ_CST_8(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_8(type, ret, ptr, val, __ATOMIC_SEQ_CST)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_SEQ_CST_16(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_16(type, ret, ptr, val, __ATOMIC_SEQ_CST)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_SEQ_CST_32(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_32(type, ret, ptr, val, __ATOMIC_SEQ_CST)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_SEQ_CST_64(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_64(type, ret, ptr, val, __ATOMIC_SEQ_CST)

#define EASTL_COMPILER_ATOMIC_FETCH_OR_SEQ_CST_128(type, ret, ptr, val)	\
	EASTL_GCC_ATOMIC_FETCH_OR_128(type, ret, ptr, val, __ATOMIC_SEQ_CST)


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_GCC_FETCH_OR_H */
