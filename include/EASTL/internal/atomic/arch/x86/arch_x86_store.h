/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_ARCH_X86_STORE_H
#define EASTL_ATOMIC_INTERNAL_ARCH_X86_STORE_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ARCH_ATOMIC_STORE_*_N(type, type * ptr, type val)
//
#if defined(EA_COMPILER_MSVC)


	#if defined(EA_COMPILER_MSVC) && (EA_COMPILER_VERSION >= 1920) // >= VS2019

		#define EASTL_ARCH_ATOMIC_X86_STORE_N(integralType, bits, type, ptr, val) \
			EA_PREPROCESSOR_JOIN(__iso_volatile_store, bits)(EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(integralType, (ptr)), EASTL_ATOMIC_TYPE_PUN_CAST(integralType, (val)))

	#else

		#define EASTL_ARCH_ATOMIC_X86_STORE_N(integralType, bits, type, ptr, val) \
			{																	\
				integralType valIntegral = EASTL_ATOMIC_TYPE_PUN_CAST(integralType, (val)); \
																				\
				(*(EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(integralType, (ptr)))) = valIntegral; \
			}

	#endif


	#define EASTL_ARCH_ATOMIC_X86_STORE_128(type, ptr, val, MemoryOrder)	\
		{																	\
			type exchange128; EA_UNUSED(exchange128);						\
			EA_PREPROCESSOR_JOIN(EA_PREPROCESSOR_JOIN(EASTL_ATOMIC_EXCHANGE_, MemoryOrder), _128)(type, exchange128, ptr, val); \
		}


	#define EASTL_ARCH_ATOMIC_X86_STORE_8(type, ptr, val)			\
		EASTL_ARCH_ATOMIC_X86_STORE_N(__int8, 8, type, ptr, val)

	#define EASTL_ARCH_ATOMIC_X86_STORE_16(type, ptr, val)			\
		EASTL_ARCH_ATOMIC_X86_STORE_N(__int16, 16, type, ptr, val)

	#define EASTL_ARCH_ATOMIC_X86_STORE_32(type, ptr, val)			\
		EASTL_ARCH_ATOMIC_X86_STORE_N(__int32, 32, type, ptr, val)

	#define EASTL_ARCH_ATOMIC_X86_STORE_64(type, ptr, val)			\
		EASTL_ARCH_ATOMIC_X86_STORE_N(__int64, 64, type, ptr, val)


	#define EASTL_ARCH_ATOMIC_STORE_RELAXED_8(type, ptr, val)	\
		EASTL_ARCH_ATOMIC_X86_STORE_8(type, ptr, val)

	#define EASTL_ARCH_ATOMIC_STORE_RELAXED_16(type, ptr, val)	\
		EASTL_ARCH_ATOMIC_X86_STORE_16(type, ptr, val)

	#define EASTL_ARCH_ATOMIC_STORE_RELAXED_32(type, ptr, val)	\
		EASTL_ARCH_ATOMIC_X86_STORE_32(type, ptr, val)

	#define EASTL_ARCH_ATOMIC_STORE_RELAXED_64(type, ptr, val)	\
		EASTL_ARCH_ATOMIC_X86_STORE_64(type, ptr, val)

	#define EASTL_ARCH_ATOMIC_STORE_RELAXED_128(type, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_STORE_128(type, ptr, val, RELAXED)


	#define EASTL_ARCH_ATOMIC_STORE_RELEASE_8(type, ptr, val)	\
		EASTL_ATOMIC_COMPILER_BARRIER();						\
		EASTL_ARCH_ATOMIC_X86_STORE_8(type, ptr, val)

	#define EASTL_ARCH_ATOMIC_STORE_RELEASE_16(type, ptr, val)	\
		EASTL_ATOMIC_COMPILER_BARRIER();						\
		EASTL_ARCH_ATOMIC_X86_STORE_16(type, ptr, val)

	#define EASTL_ARCH_ATOMIC_STORE_RELEASE_32(type, ptr, val)	\
		EASTL_ATOMIC_COMPILER_BARRIER();						\
		EASTL_ARCH_ATOMIC_X86_STORE_32(type, ptr, val)

	#define EASTL_ARCH_ATOMIC_STORE_RELEASE_64(type, ptr, val)	\
		EASTL_ATOMIC_COMPILER_BARRIER();						\
		EASTL_ARCH_ATOMIC_X86_STORE_64(type, ptr, val)

	#define EASTL_ARCH_ATOMIC_STORE_RELEASE_128(type, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_STORE_128(type, ptr, val, RELEASE)


	#define EASTL_ARCH_ATOMIC_STORE_SEQ_CST_8(type, ptr, val)		\
		{															\
			type exchange8; EA_UNUSED(exchange8);					\
			EASTL_ATOMIC_EXCHANGE_SEQ_CST_8(type, exchange8, ptr, val);	\
		}

	#define EASTL_ARCH_ATOMIC_STORE_SEQ_CST_16(type, ptr, val)		\
		{															\
			type exchange16; EA_UNUSED(exchange16);					\
			EASTL_ATOMIC_EXCHANGE_SEQ_CST_16(type, exchange16, ptr, val); \
		}

	#define EASTL_ARCH_ATOMIC_STORE_SEQ_CST_32(type, ptr, val)		\
		{															\
			type exchange32; EA_UNUSED(exchange32);					\
			EASTL_ATOMIC_EXCHANGE_SEQ_CST_32(type, exchange32, ptr, val); \
		}


	/**
	 * NOTE:
	 *
	 * Since 64-bit exchange is wrapped around a cmpxchg8b on 32-bit x86, it is
	 * faster to just do a mov; mfence.
	 */
	#if defined(EA_PROCESSOR_X86)


		#define EASTL_ARCH_ATOMIC_STORE_SEQ_CST_64(type, ptr, val)		\
					EASTL_ATOMIC_COMPILER_BARRIER();					\
					EASTL_ARCH_ATOMIC_X86_STORE_64(type, ptr, val);		\
					EASTL_ATOMIC_CPU_MB()


	#elif defined(EA_PROCESSOR_X86_64)


		#define EASTL_ARCH_ATOMIC_STORE_SEQ_CST_64(type, ptr, val)		\
			{															\
				type exchange64; EA_UNUSED(exchange64);					\
				EASTL_ATOMIC_EXCHANGE_SEQ_CST_64(type, exchange64, ptr, val); \
			}


	#endif


	#define EASTL_ARCH_ATOMIC_STORE_SEQ_CST_128(type, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_STORE_128(type, ptr, val, SEQ_CST)


#endif


#if ((defined(__clang__) || defined(EA_COMPILER_GNUC)) && defined(EA_PROCESSOR_X86_64))


	#define EASTL_ARCH_ATOMIC_X86_STORE_128(type, ptr, val, MemoryOrder)	\
		{																	\
			type exchange128; EA_UNUSED(exchange128);						\
			EA_PREPROCESSOR_JOIN(EA_PREPROCESSOR_JOIN(EASTL_ATOMIC_EXCHANGE_, MemoryOrder), _128)(type, exchange128, ptr, val); \
		}


	#define EASTL_ARCH_ATOMIC_STORE_RELAXED_128(type, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_STORE_128(type, ptr, val, RELAXED)

	#define EASTL_ARCH_ATOMIC_STORE_RELEASE_128(type, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_STORE_128(type, ptr, val, RELEASE)

	#define EASTL_ARCH_ATOMIC_STORE_SEQ_CST_128(type, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_STORE_128(type, ptr, val, SEQ_CST)


#endif


#endif /* EASTL_ATOMIC_INTERNAL_ARCH_X86_STORE_H */
