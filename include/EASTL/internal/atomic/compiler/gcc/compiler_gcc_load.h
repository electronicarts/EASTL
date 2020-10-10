/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_GCC_LOAD_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_GCC_LOAD_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


#define EASTL_GCC_ATOMIC_LOAD_N(integralType, type, ret, ptr, gccMemoryOrder) \
	{																	\
		integralType retIntegral;										\
		__atomic_load(EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(integralType, (ptr)), &retIntegral, gccMemoryOrder); \
																		\
		ret = EASTL_ATOMIC_TYPE_PUN_CAST(type, retIntegral);			\
	}

#define EASTL_GCC_ATOMIC_LOAD_8(type, ret, ptr, gccMemoryOrder)			\
	EASTL_GCC_ATOMIC_LOAD_N(uint8_t, type, ret, ptr, gccMemoryOrder)

#define EASTL_GCC_ATOMIC_LOAD_16(type, ret, ptr, gccMemoryOrder)		\
	EASTL_GCC_ATOMIC_LOAD_N(uint16_t, type, ret, ptr, gccMemoryOrder)

#define EASTL_GCC_ATOMIC_LOAD_32(type, ret, ptr, gccMemoryOrder)		\
	EASTL_GCC_ATOMIC_LOAD_N(uint32_t, type, ret, ptr, gccMemoryOrder)

#define EASTL_GCC_ATOMIC_LOAD_64(type, ret, ptr, gccMemoryOrder)		\
	EASTL_GCC_ATOMIC_LOAD_N(uint64_t, type, ret, ptr, gccMemoryOrder)

#define EASTL_GCC_ATOMIC_LOAD_128(type, ret, ptr, gccMemoryOrder)		\
	EASTL_GCC_ATOMIC_LOAD_N(__uint128_t, type, ret, ptr, gccMemoryOrder)


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_COMPILER_ATOMIC_LOAD_*_N(type, type ret, type * ptr)
//
#define EASTL_COMPILER_ATOMIC_LOAD_RELAXED_8(type, ret, ptr)	\
	EASTL_GCC_ATOMIC_LOAD_8(type, ret, ptr, __ATOMIC_RELAXED)

#define EASTL_COMPILER_ATOMIC_LOAD_RELAXED_16(type, ret, ptr)	\
	EASTL_GCC_ATOMIC_LOAD_16(type, ret, ptr, __ATOMIC_RELAXED)

#define EASTL_COMPILER_ATOMIC_LOAD_RELAXED_32(type, ret, ptr)	\
	EASTL_GCC_ATOMIC_LOAD_32(type, ret, ptr, __ATOMIC_RELAXED)

#define EASTL_COMPILER_ATOMIC_LOAD_RELAXED_64(type, ret, ptr)	\
	EASTL_GCC_ATOMIC_LOAD_64(type, ret, ptr, __ATOMIC_RELAXED)

#define EASTL_COMPILER_ATOMIC_LOAD_RELAXED_128(type, ret, ptr)	\
	EASTL_GCC_ATOMIC_LOAD_128(type, ret, ptr, __ATOMIC_RELAXED)


#define EASTL_COMPILER_ATOMIC_LOAD_ACQUIRE_8(type, ret, ptr)	\
	EASTL_GCC_ATOMIC_LOAD_8(type, ret, ptr, __ATOMIC_ACQUIRE)

#define EASTL_COMPILER_ATOMIC_LOAD_ACQUIRE_16(type, ret, ptr)	\
	EASTL_GCC_ATOMIC_LOAD_16(type, ret, ptr, __ATOMIC_ACQUIRE)

#define EASTL_COMPILER_ATOMIC_LOAD_ACQUIRE_32(type, ret, ptr)	\
	EASTL_GCC_ATOMIC_LOAD_32(type, ret, ptr, __ATOMIC_ACQUIRE)

#define EASTL_COMPILER_ATOMIC_LOAD_ACQUIRE_64(type, ret, ptr)	\
	EASTL_GCC_ATOMIC_LOAD_64(type, ret, ptr, __ATOMIC_ACQUIRE)

#define EASTL_COMPILER_ATOMIC_LOAD_ACQUIRE_128(type, ret, ptr)	\
	EASTL_GCC_ATOMIC_LOAD_128(type, ret, ptr, __ATOMIC_ACQUIRE)


#define EASTL_COMPILER_ATOMIC_LOAD_SEQ_CST_8(type, ret, ptr)	\
	EASTL_GCC_ATOMIC_LOAD_8(type, ret, ptr, __ATOMIC_SEQ_CST)

#define EASTL_COMPILER_ATOMIC_LOAD_SEQ_CST_16(type, ret, ptr)	\
	EASTL_GCC_ATOMIC_LOAD_16(type, ret, ptr, __ATOMIC_SEQ_CST)

#define EASTL_COMPILER_ATOMIC_LOAD_SEQ_CST_32(type, ret, ptr)	\
	EASTL_GCC_ATOMIC_LOAD_32(type, ret, ptr, __ATOMIC_SEQ_CST)

#define EASTL_COMPILER_ATOMIC_LOAD_SEQ_CST_64(type, ret, ptr)	\
	EASTL_GCC_ATOMIC_LOAD_64(type, ret, ptr, __ATOMIC_SEQ_CST)

#define EASTL_COMPILER_ATOMIC_LOAD_SEQ_CST_128(type, ret, ptr)	\
	EASTL_GCC_ATOMIC_LOAD_128(type, ret, ptr, __ATOMIC_SEQ_CST)


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_GCC_LOAD_H */
