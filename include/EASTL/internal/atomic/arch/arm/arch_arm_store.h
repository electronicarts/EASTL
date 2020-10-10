/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_ARCH_ARM_STORE_H
#define EASTL_ATOMIC_INTERNAL_ARCH_ARM_STORE_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ARCH_ATOMIC_STORE_*_N(type, type * ptr, type val)
//
#if defined(EA_COMPILER_MSVC)


	#define EASTL_ARCH_ATOMIC_ARM_STORE_N(integralType, bits, type, ptr, val) \
		EA_PREPROCESSOR_JOIN(__iso_volatile_store, bits)(EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(integralType, (ptr)), EASTL_ATOMIC_TYPE_PUN_CAST(integralType, (val)))


	#define EASTL_ARCH_ATOMIC_ARM_STORE_8(type, ptr, val)			\
		EASTL_ARCH_ATOMIC_ARM_STORE_N(__int8, 8, type, ptr, val)

	#define EASTL_ARCH_ATOMIC_ARM_STORE_16(type, ptr, val)			\
		EASTL_ARCH_ATOMIC_ARM_STORE_N(__int16, 16, type, ptr, val)

	#define EASTL_ARCH_ATOMIC_ARM_STORE_32(type, ptr, val)			\
		EASTL_ARCH_ATOMIC_ARM_STORE_N(__int32, 32, type, ptr, val)


	#if defined(EA_PROCESSOR_ARM64)

		#define EASTL_ARCH_ATOMIC_ARM_STORE_64(type, ptr, val)			\
			EASTL_ARCH_ATOMIC_ARM_STORE_N(__int64, 64, type, ptr, val)

	#endif


	#define EASTL_ARCH_ATOMIC_ARM_STORE_128(type, ptr, val, MemoryOrder)	\
		{																	\
			type exchange128; EA_UNUSED(exchange128);						\
			EA_PREPROCESSOR_JOIN(EA_PREPROCESSOR_JOIN(EASTL_ATOMIC_EXCHANGE_, MemoryOrder), _128)(type, exchange128, ptr, val); \
		}


	#define EASTL_ARCH_ATOMIC_STORE_RELAXED_8(type, ptr, val)	\
		EASTL_ARCH_ATOMIC_ARM_STORE_8(type, ptr, val)

	#define EASTL_ARCH_ATOMIC_STORE_RELAXED_16(type, ptr, val)	\
		EASTL_ARCH_ATOMIC_ARM_STORE_16(type, ptr, val)

	#define EASTL_ARCH_ATOMIC_STORE_RELAXED_32(type, ptr, val)	\
		EASTL_ARCH_ATOMIC_ARM_STORE_32(type, ptr, val)

	#define EASTL_ARCH_ATOMIC_STORE_RELAXED_128(type, ptr, val)		\
		EASTL_ARCH_ATOMIC_ARM_STORE_128(type, ptr, val, RELAXED)


	#define EASTL_ARCH_ATOMIC_STORE_RELEASE_8(type, ptr, val)	\
		EASTL_ATOMIC_CPU_MB();									\
		EASTL_ARCH_ATOMIC_ARM_STORE_8(type, ptr, val)

	#define EASTL_ARCH_ATOMIC_STORE_RELEASE_16(type, ptr, val)	\
		EASTL_ATOMIC_CPU_MB();									\
		EASTL_ARCH_ATOMIC_ARM_STORE_16(type, ptr, val)

	#define EASTL_ARCH_ATOMIC_STORE_RELEASE_32(type, ptr, val)	\
		EASTL_ATOMIC_CPU_MB();									\
		EASTL_ARCH_ATOMIC_ARM_STORE_32(type, ptr, val)

	#define EASTL_ARCH_ATOMIC_STORE_RELEASE_128(type, ptr, val)		\
		EASTL_ARCH_ATOMIC_ARM_STORE_128(type, ptr, val, RELEASE)


	#define EASTL_ARCH_ATOMIC_STORE_SEQ_CST_8(type, ptr, val)	\
		EASTL_ATOMIC_CPU_MB();									\
		EASTL_ARCH_ATOMIC_ARM_STORE_8(type, ptr, val) ;			\
		EASTL_ATOMIC_CPU_MB()

	#define EASTL_ARCH_ATOMIC_STORE_SEQ_CST_16(type, ptr, val)	\
		EASTL_ATOMIC_CPU_MB();									\
		EASTL_ARCH_ATOMIC_ARM_STORE_16(type, ptr, val);			\
		EASTL_ATOMIC_CPU_MB()

	#define EASTL_ARCH_ATOMIC_STORE_SEQ_CST_32(type, ptr, val)	\
		EASTL_ATOMIC_CPU_MB();									\
		EASTL_ARCH_ATOMIC_ARM_STORE_32(type, ptr, val);			\
		EASTL_ATOMIC_CPU_MB()

	#define EASTL_ARCH_ATOMIC_STORE_SEQ_CST_128(type, ptr, val)		\
		EASTL_ARCH_ATOMIC_ARM_STORE_128(type, ptr, val, SEQ_CST)


	#if defined(EA_PROCESSOR_ARM32)


		#define EASTL_ARCH_ATOMIC_STORE_RELAXED_64(type, ptr, val)			\
			{																\
				type retExchange64; EA_UNUSED(retExchange64);				\
				EASTL_ATOMIC_EXCHANGE_RELAXED_64(type, retExchange64, ptr, val); \
			}

		#define EASTL_ARCH_ATOMIC_STORE_RELEASE_64(type, ptr, val)			\
			{																\
				type retExchange64; EA_UNUSED(retExchange64);				\
				EASTL_ATOMIC_EXCHANGE_RELEASE_64(type, retExchange64, ptr, val); \
			}

		#define EASTL_ARCH_ATOMIC_STORE_SEQ_CST_64(type, ptr, val)			\
			{																\
				type retExchange64; EA_UNUSED(retExchange64);				\
				EASTL_ATOMIC_EXCHANGE_SEQ_CST_64(type, retExchange64, ptr, val); \
			}


	#elif defined(EA_PROCESSOR_ARM64)


		#define EASTL_ARCH_ATOMIC_STORE_RELAXED_64(type, ptr, val)	\
			EASTL_ARCH_ATOMIC_ARM_STORE_64(type, ptr, val)

		#define EASTL_ARCH_ATOMIC_STORE_RELEASE_64(type, ptr, val)	\
			EASTL_ATOMIC_CPU_MB();									\
			EASTL_ARCH_ATOMIC_ARM_STORE_64(type, ptr, val)

		#define EASTL_ARCH_ATOMIC_STORE_SEQ_CST_64(type, ptr, val)	\
			EASTL_ATOMIC_CPU_MB();									\
			EASTL_ARCH_ATOMIC_ARM_STORE_64(type, ptr, val);			\
			EASTL_ATOMIC_CPU_MB()


	#endif


#endif


#endif /* EASTL_ATOMIC_INTERNAL_ARCH_ARM_STORE_H */
