/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_ARCH_X86_EXCHANGE_H
#define EASTL_ATOMIC_INTERNAL_ARCH_X86_EXCHANGE_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ARCH_ATOMIC_EXCHANGE_*_N(type, type ret, type * ptr, type val)
//
#if defined(EA_COMPILER_MSVC) && defined(EA_PROCESSOR_X86)


	#define EASTL_ARCH_ATOMIC_X86_EXCHANGE_PRE_COMPUTE_DESIRED(ret, observed, val) \
		ret = (val)


	#define EASTL_ARCH_ATOMIC_EXCHANGE_RELAXED_64(type, ret, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_OP_64_IMPL(type, ret, ptr, val, RELAXED,		\
										 EASTL_ARCH_ATOMIC_X86_EXCHANGE_PRE_COMPUTE_DESIRED, \
										 EASTL_ARCH_ATOMIC_X86_NOP_POST_COMPUTE_RET)

	#define EASTL_ARCH_ATOMIC_EXCHANGE_ACQUIRE_64(type, ret, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_OP_64_IMPL(type, ret, ptr, val, ACQUIRE,		\
										 EASTL_ARCH_ATOMIC_X86_EXCHANGE_PRE_COMPUTE_DESIRED, \
										 EASTL_ARCH_ATOMIC_X86_NOP_POST_COMPUTE_RET)

	#define EASTL_ARCH_ATOMIC_EXCHANGE_RELEASE_64(type, ret, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_OP_64_IMPL(type, ret, ptr, val, RELEASE,		\
										 EASTL_ARCH_ATOMIC_X86_EXCHANGE_PRE_COMPUTE_DESIRED, \
										 EASTL_ARCH_ATOMIC_X86_NOP_POST_COMPUTE_RET)

	#define EASTL_ARCH_ATOMIC_EXCHANGE_ACQ_REL_64(type, ret, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_OP_64_IMPL(type, ret, ptr, val, ACQ_REL,		\
										 EASTL_ARCH_ATOMIC_X86_EXCHANGE_PRE_COMPUTE_DESIRED, \
										 EASTL_ARCH_ATOMIC_X86_NOP_POST_COMPUTE_RET)

	#define EASTL_ARCH_ATOMIC_EXCHANGE_SEQ_CST_64(type, ret, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_OP_64_IMPL(type, ret, ptr, val, SEQ_CST,		\
										 EASTL_ARCH_ATOMIC_X86_EXCHANGE_PRE_COMPUTE_DESIRED, \
										 EASTL_ARCH_ATOMIC_X86_NOP_POST_COMPUTE_RET)


#endif


#if ((defined(__clang__) || defined(EA_COMPILER_GNUC)) && defined(EA_PROCESSOR_X86_64))


	#define EASTL_ARCH_ATOMIC_X86_EXCHANGE_128(type, ret, ptr, val, MemoryOrder) \
		{																	\
			bool cmpxchgRet;												\
			/* This is intentionally a non-atomic 128-bit load which may observe shearing. */ \
			/* Either we do not observe *(ptr) but then the cmpxchg will fail and the observed */ \
			/* atomic load will be returned. Or the non-atomic load got lucky and the cmpxchg succeeds */ \
			/* because the observed value equals the value in *(ptr) thus we optimistically do a non-atomic load. */ \
			ret = *(ptr);													\
			do																\
			{																\
				EA_PREPROCESSOR_JOIN(EA_PREPROCESSOR_JOIN(EASTL_ATOMIC_CMPXCHG_STRONG_, MemoryOrder), _128)(type, cmpxchgRet, ptr, &(ret), val); \
			} while (!cmpxchgRet);											\
		}


	#define EASTL_ARCH_ATOMIC_EXCHANGE_RELAXED_128(type, ret, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_EXCHANGE_128(type, ret, ptr, val, RELAXED)

	#define EASTL_ARCH_ATOMIC_EXCHANGE_ACQUIRE_128(type, ret, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_EXCHANGE_128(type, ret, ptr, val, ACQUIRE)

	#define EASTL_ARCH_ATOMIC_EXCHANGE_RELEASE_128(type, ret, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_EXCHANGE_128(type, ret, ptr, val, RELEASE)

	#define EASTL_ARCH_ATOMIC_EXCHANGE_ACQ_REL_128(type, ret, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_EXCHANGE_128(type, ret, ptr, val, ACQ_REL)

	#define EASTL_ARCH_ATOMIC_EXCHANGE_SEQ_CST_128(type, ret, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_EXCHANGE_128(type, ret, ptr, val, SEQ_CST)


#endif


#endif /* EASTL_ATOMIC_INTERNAL_ARCH_X86_EXCHANGE_H */
