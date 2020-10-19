/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_ARCH_ARM_LOAD_H
#define EASTL_ATOMIC_INTERNAL_ARCH_ARM_LOAD_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ARCH_ATOMIC_LOAD_*_N(type, type ret, type * ptr)
//
#if defined(EA_COMPILER_MSVC)


	/**
	 * NOTE:
	 *
	 * Even 8-byte aligned 64-bit memory accesses on ARM32 are not
	 * guaranteed to be atomic on all ARM32 cpus. Only guaranteed on
	 * cpus with the LPAE extension. We need to use a
	 * ldrexd instruction in order to ensure no shearing is observed
	 * for all ARM32 processors.
	 */
	#if defined(EA_PROCESSOR_ARM32)

		#define EASTL_ARCH_ATOMIC_ARM32_LDREXD(ret, ptr)	\
			ret = __ldrexd((ptr))

	#endif


	#define EASTL_ARCH_ATOMIC_ARM_LOAD_N(integralType, bits, type, ret, ptr) \
		{																	\
			integralType retIntegral;										\
			retIntegral = EA_PREPROCESSOR_JOIN(__iso_volatile_load, bits)(EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(integralType, (ptr))); \
																			\
			ret = EASTL_ATOMIC_TYPE_PUN_CAST(type, retIntegral);			\
		}


	#define EASTL_ARCH_ATOMIC_ARM_LOAD_8(type, ret, ptr)		\
		EASTL_ARCH_ATOMIC_ARM_LOAD_N(__int8, 8, type, ret, ptr)

	#define EASTL_ARCH_ATOMIC_ARM_LOAD_16(type, ret, ptr)		\
		EASTL_ARCH_ATOMIC_ARM_LOAD_N(__int16, 16, type, ret, ptr)

	#define EASTL_ARCH_ATOMIC_ARM_LOAD_32(type, ret, ptr)		\
		EASTL_ARCH_ATOMIC_ARM_LOAD_N(__int32, 32, type, ret, ptr)


	#if defined(EA_PROCESSOR_ARM32)


		#define EASTL_ARCH_ATOMIC_LOAD_64(type, ret, ptr)						\
			{																	\
				__int64 loadRet64;												\
				EASTL_ARCH_ATOMIC_ARM32_LDREXD(loadRet64, EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(__int64, (ptr))); \
																				\
				ret = EASTL_ATOMIC_TYPE_PUN_CAST(type, loadRet64);				\
			}

	#else

		#define EASTL_ARCH_ATOMIC_ARM_LOAD_64(type, ret, ptr)		\
			EASTL_ARCH_ATOMIC_ARM_LOAD_N(__int64, 64, type, ret, ptr)

	#endif


	/**
	 * NOTE:
	 *
	 * The ARM documentation states the following:
	 * A 64-bit pair requires the address to be quadword aligned and is single-copy atomic for each doubleword at doubleword granularity
	 *
	 * Thus we must ensure the store succeeds inorder for the load to be observed as atomic.
	 * Thus we must use the full cmpxchg in order to do a proper atomic load.
	 */
	#define EASTL_ARCH_ATOMIC_ARM_LOAD_128(type, ret, ptr, MemoryOrder)		\
		{																	\
			bool cmpxchgRetBool;											\
			ret = *(ptr);													\
			do																\
			{																\
				EA_PREPROCESSOR_JOIN(EA_PREPROCESSOR_JOIN(EASTL_ATOMIC_CMPXCHG_STRONG_, MemoryOrder), _128)(type, cmpxchgRetBool, \
																											ptr, &(ret), ret); \
			} while (!cmpxchgRetBool);										\
		}


	#define EASTL_ARCH_ATOMIC_LOAD_RELAXED_8(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_ARM_LOAD_8(type, ret, ptr)

	#define EASTL_ARCH_ATOMIC_LOAD_RELAXED_16(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_ARM_LOAD_16(type, ret, ptr)

	#define EASTL_ARCH_ATOMIC_LOAD_RELAXED_32(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_ARM_LOAD_32(type, ret, ptr)

	#define EASTL_ARCH_ATOMIC_LOAD_RELAXED_64(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_ARM_LOAD_64(type, ret, ptr)

	#define EASTL_ARCH_ATOMIC_LOAD_RELAXED_128(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_ARM_LOAD_128(type, ret, ptr, RELAXED)


	#define EASTL_ARCH_ATOMIC_LOAD_ACQUIRE_8(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_ARM_LOAD_8(type, ret, ptr);			\
		EASTL_ATOMIC_CPU_MB()

	#define EASTL_ARCH_ATOMIC_LOAD_ACQUIRE_16(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_ARM_LOAD_16(type, ret, ptr);			\
		EASTL_ATOMIC_CPU_MB()

	#define EASTL_ARCH_ATOMIC_LOAD_ACQUIRE_32(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_ARM_LOAD_32(type, ret, ptr);			\
		EASTL_ATOMIC_CPU_MB()

	#define EASTL_ARCH_ATOMIC_LOAD_ACQUIRE_64(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_ARM_LOAD_64(type, ret, ptr);			\
		EASTL_ATOMIC_CPU_MB()

	#define EASTL_ARCH_ATOMIC_LOAD_ACQUIRE_128(type, ret, ptr)		\
		EASTL_ARCH_ATOMIC_ARM_LOAD_128(type, ret, ptr, ACQUIRE)


	#define EASTL_ARCH_ATOMIC_LOAD_SEQ_CST_8(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_ARM_LOAD_8(type, ret, ptr);			\
		EASTL_ATOMIC_CPU_MB()

	#define EASTL_ARCH_ATOMIC_LOAD_SEQ_CST_16(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_ARM_LOAD_16(type, ret, ptr);			\
		EASTL_ATOMIC_CPU_MB()

	#define EASTL_ARCH_ATOMIC_LOAD_SEQ_CST_32(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_ARM_LOAD_32(type, ret, ptr);			\
		EASTL_ATOMIC_CPU_MB()

	#define EASTL_ARCH_ATOMIC_LOAD_SEQ_CST_64(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_ARM_LOAD_64(type, ret, ptr);			\
		EASTL_ATOMIC_CPU_MB()

	#define EASTL_ARCH_ATOMIC_LOAD_SEQ_CST_128(type, ret, ptr)		\
		EASTL_ARCH_ATOMIC_ARM_LOAD_128(type, ret, ptr, SEQ_CST)


#endif


#endif /* EASTL_ATOMIC_INTERNAL_ARCH_ARM_LOAD_H */
