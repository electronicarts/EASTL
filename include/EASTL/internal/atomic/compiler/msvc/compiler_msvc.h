/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_H
#define EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


EA_DISABLE_ALL_VC_WARNINGS();
#include <Windows.h>
#include <intrin.h>
EA_RESTORE_ALL_VC_WARNINGS();


/////////////////////////////////////////////////////////////////////////////////


#define EASTL_COMPILER_ATOMIC_HAS_8BIT
#define EASTL_COMPILER_ATOMIC_HAS_16BIT
#define EASTL_COMPILER_ATOMIC_HAS_32BIT
#define EASTL_COMPILER_ATOMIC_HAS_64BIT

#if EA_PLATFORM_PTR_SIZE == 8
	#define EASTL_COMPILER_ATOMIC_HAS_128BIT
#endif


/////////////////////////////////////////////////////////////////////////////////


#define EASTL_COMPILER_ATOMIC_FIXED_WIDTH_TYPE_8 char
#define EASTL_COMPILER_ATOMIC_FIXED_WIDTH_TYPE_16 short
#define EASTL_COMPILER_ATOMIC_FIXED_WIDTH_TYPE_32 long
#define EASTL_COMPILER_ATOMIC_FIXED_WIDTH_TYPE_64 __int64

namespace eastl
{

namespace internal
{

struct FixedWidth128
{
	__int64 value[2];
};

} // namespace internal

} // namespace eastl

#define EASTL_COMPILER_ATOMIC_FIXED_WIDTH_TYPE_128 eastl::internal::FixedWidth128


/////////////////////////////////////////////////////////////////////////////////


/**
 * NOTE:
 *
 * Unfortunately MSVC Intrinsics depend on the architecture
 * that we are compiling for.
 * These are some indirection macros to make our lives easier and
 * ensure the least possible amount of copy-paste to reduce programmer errors.
 *
 * All compiler implementations end up deferring to the below macros.
 */
#if defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64)


	#define EASTL_MSVC_ATOMIC_FETCH_OP(ret, ptr, val, MemoryOrder, Intrinsic) \
		ret = Intrinsic(ptr, val)

	#define EASTL_MSVC_ATOMIC_EXCHANGE_OP(ret, ptr, val, MemoryOrder, Intrinsic) \
		ret = Intrinsic(ptr, val)

	#define EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_OP(ret, ptr, comparand, exchange, MemoryOrder, Intrinsic) \
		ret = Intrinsic(ptr, exchange, comparand)

	#define EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_128_OP(ret, ptr, comparandResult, exchangeHigh, exchangeLow, MemoryOrder) \
		ret = _InterlockedCompareExchange128_np(ptr, exchangeHigh, exchangeLow, comparandResult)


#elif defined(EA_PROCESSOR_ARM32) || defined(EA_PROCESSOR_ARM64)


	#define EASTL_MSVC_INTRINSIC_RELAXED(Intrinsic) \
		EA_PREPROCESSOR_JOIN(Intrinsic, _nf)

	#define EASTL_MSVC_INTRINSIC_ACQUIRE(Intrinsic)	\
		EA_PREPROCESSOR_JOIN(Intrinsic, _acq)

	#define EASTL_MSVC_INTRINSIC_RELEASE(Intrinsic)	\
		EA_PREPROCESSOR_JOIN(Intrinsic, _rel)

	#define EASTL_MSVC_INTRINSIC_ACQ_REL(Intrinsic)	\
		Intrinsic

	#define EASTL_MSVC_INTRINSIC_SEQ_CST(Intrinsic)	\
		Intrinsic


	#define EASTL_MSVC_ATOMIC_FETCH_OP(ret, ptr, val, MemoryOrder, Intrinsic) \
		ret = EA_PREPROCESSOR_JOIN(EASTL_MSVC_INTRINSIC_, MemoryOrder)(Intrinsic)(ptr, val)

	#define EASTL_MSVC_ATOMIC_EXCHANGE_OP(ret, ptr, val, MemoryOrder, Intrinsic) \
		ret = EA_PREPROCESSOR_JOIN(EASTL_MSVC_INTRINSIC_, MemoryOrder)(Intrinsic)(ptr, val)

	#define EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_OP(ret, ptr, comparand, exchange, MemoryOrder, Intrinsic) \
		ret = EA_PREPROCESSOR_JOIN(EASTL_MSVC_INTRINSIC_, MemoryOrder)(Intrinsic)(ptr, exchange, comparand)

	#define EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_128_OP(ret, ptr, comparandResult, exchangeHigh, exchangeLow, MemoryOrder) \
		ret = EA_PREPROCESSOR_JOIN(EASTL_MSVC_INTRINSIC_, MemoryOrder)(_InterlockedCompareExchange128)(ptr, exchangeHigh, exchangeLow, comparandResult)


#endif


/////////////////////////////////////////////////////////////////////////////////


#define EASTL_MSVC_NOP_POST_INTRIN_COMPUTE(ret, lhs, rhs)

#define EASTL_MSVC_NOP_PRE_INTRIN_COMPUTE(ret, val) \
	ret = (val)


#define EASTL_MSVC_ATOMIC_FETCH_INTRIN_N(integralType, fetchIntrinsic, type, ret, ptr, val, MemoryOrder, PRE_INTRIN_COMPUTE, POST_INTRIN_COMPUTE) \
	{																	\
		integralType retIntegral;										\
		type valCompute;												\
																		\
		PRE_INTRIN_COMPUTE(valCompute, (val));							\
		const integralType valIntegral = EASTL_ATOMIC_TYPE_PUN_CAST(integralType, valCompute); \
																		\
		EASTL_MSVC_ATOMIC_FETCH_OP(retIntegral, EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(integralType, (ptr)), \
								   valIntegral, MemoryOrder, fetchIntrinsic); \
																		\
		ret = EASTL_ATOMIC_TYPE_PUN_CAST(type, retIntegral);			\
		POST_INTRIN_COMPUTE(ret, ret, (val));							\
	}

#define EASTL_MSVC_ATOMIC_EXCHANGE_INTRIN_N(integralType, exchangeIntrinsic, type, ret, ptr, val, MemoryOrder) \
	{																	\
		integralType retIntegral;										\
		EASTL_MSVC_ATOMIC_EXCHANGE_OP(retIntegral, EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(integralType, (ptr)), \
									  EASTL_ATOMIC_TYPE_PUN_CAST(integralType, (val)), MemoryOrder, \
									  exchangeIntrinsic);				\
																		\
		ret = EASTL_ATOMIC_TYPE_PUN_CAST(type, retIntegral);			\
	}

#define EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_INTRIN_N(integralType, cmpxchgStrongIntrinsic, type, ret, ptr, expected, desired, MemoryOrder) \
	{																	\
		integralType comparandIntegral = EASTL_ATOMIC_TYPE_PUN_CAST(integralType, *(expected)); \
		integralType oldIntegral;										\
		EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_OP(oldIntegral, EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(integralType, (ptr)), \
											comparandIntegral, EASTL_ATOMIC_TYPE_PUN_CAST(integralType, (desired)), \
											MemoryOrder, cmpxchgStrongIntrinsic); \
																		\
		if (oldIntegral == comparandIntegral)							\
		{																\
			ret = true;													\
		}																\
		else															\
		{																\
			*(expected) = EASTL_ATOMIC_TYPE_PUN_CAST(type, oldIntegral); \
			ret = false;												\
		}																\
	}

/**
 * In my own opinion, I found the wording on Microsoft docs a little confusing.
 * ExchangeHigh means the top 8 bytes so (ptr + 8).
 * ExchangeLow means the low 8 butes so (ptr).
 * Endianness does not matter since we are just loading data and comparing data.
 * Thought of as memcpy() and memcmp() function calls whereby the layout of the
 * data itself is irrelevant.
 * Only after we type pun back to the original type, and load from memory does
 * the layout of the data matter again.
 */
#define EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_INTRIN_128(type, ret, ptr, expected, desired, MemoryOrder) \
	{																	\
		union TypePun													\
		{																\
			type templateType;											\
																		\
			struct exchange128											\
			{															\
				__int64 value[2];										\
			};															\
																		\
			struct exchange128 exchangePun;								\
		};																\
																		\
		union TypePun typePun = { (desired) };							\
																		\
		unsigned char cmpxchgRetChar;									\
		cmpxchgRetChar = EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_128_OP(cmpxchgRetChar, EASTL_ATOMIC_VOLATILE_TYPE_CAST(__int64, (ptr)), \
																 EASTL_ATOMIC_TYPE_CAST(__int64, (expected)), \
																 typePun.exchangePun.value[1], typePun.exchangePun.value[0], \
																 MemoryOrder); \
																		\
		ret = static_cast<bool>(cmpxchgRetChar);						\
	}


/////////////////////////////////////////////////////////////////////////////////


#define EASTL_MSVC_ATOMIC_FETCH_OP_N(integralType, fetchIntrinsic, type, ret, ptr, val, MemoryOrder, PRE_INTRIN_COMPUTE) \
	EASTL_MSVC_ATOMIC_FETCH_INTRIN_N(integralType, fetchIntrinsic, type, ret, ptr, val, MemoryOrder, PRE_INTRIN_COMPUTE, EASTL_MSVC_NOP_POST_INTRIN_COMPUTE)

#define EASTL_MSVC_ATOMIC_OP_FETCH_N(integralType, fetchIntrinsic, type, ret, ptr, val, MemoryOrder, PRE_INTRIN_COMPUTE, POST_INTRIN_COMPUTE) \
	EASTL_MSVC_ATOMIC_FETCH_INTRIN_N(integralType, fetchIntrinsic, type, ret, ptr, val, MemoryOrder, PRE_INTRIN_COMPUTE, POST_INTRIN_COMPUTE)

#define EASTL_MSVC_ATOMIC_EXCHANGE_OP_N(integralType, exchangeIntrinsic, type, ret, ptr, val, MemoryOrder) \
	EASTL_MSVC_ATOMIC_EXCHANGE_INTRIN_N(integralType, exchangeIntrinsic, type, ret, ptr, val, MemoryOrder)

#define EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_OP_N(integralType, cmpxchgStrongIntrinsic, type, ret, ptr, expected, desired, MemoryOrder) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_INTRIN_N(integralType, cmpxchgStrongIntrinsic, type, ret, ptr, expected, desired, MemoryOrder)

#define EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_OP_128(type, ret, ptr, expected, desired, MemoryOrder) \
	EASTL_MSVC_ATOMIC_CMPXCHG_STRONG_INTRIN_128(type, ret, ptr, expected, desired, MemoryOrder)


/////////////////////////////////////////////////////////////////////////////////


#include "compiler_msvc_fetch_add.h"
#include "compiler_msvc_fetch_sub.h"

#include "compiler_msvc_fetch_and.h"
#include "compiler_msvc_fetch_xor.h"
#include "compiler_msvc_fetch_or.h"

#include "compiler_msvc_add_fetch.h"
#include "compiler_msvc_sub_fetch.h"

#include "compiler_msvc_and_fetch.h"
#include "compiler_msvc_xor_fetch.h"
#include "compiler_msvc_or_fetch.h"

#include "compiler_msvc_exchange.h"

#include "compiler_msvc_cmpxchg_weak.h"
#include "compiler_msvc_cmpxchg_strong.h"

#include "compiler_msvc_barrier.h"

#include "compiler_msvc_cpu_pause.h"

#include "compiler_msvc_signal_fence.h"


#endif /* EASTL_ATOMIC_INTERNAL_COMPILER_MSVC_H */
