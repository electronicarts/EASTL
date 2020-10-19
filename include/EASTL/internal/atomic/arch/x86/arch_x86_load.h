/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_ARCH_X86_LOAD_H
#define EASTL_ATOMIC_INTERNAL_ARCH_X86_LOAD_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ARCH_ATOMIC_LOAD_*_N(type, type ret, type * ptr)
//
#if defined(EA_COMPILER_MSVC)


	#if defined(EA_COMPILER_MSVC) && (EA_COMPILER_VERSION >= 1920) // >= VS2019

		#define EASTL_ARCH_ATOMIC_X86_LOAD_N(integralType, bits, type, ret, ptr) \
			{																	\
				integralType retIntegral;										\
				retIntegral = EA_PREPROCESSOR_JOIN(__iso_volatile_load, bits)(EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(integralType, (ptr))); \
																				\
				ret = EASTL_ATOMIC_TYPE_PUN_CAST(type, retIntegral);			\
			}

	#else

		#define EASTL_ARCH_ATOMIC_X86_LOAD_N(integralType, bits, type, ret, ptr) \
			{																	\
				integralType retIntegral;										\
				retIntegral = (*(EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(integralType, (ptr)))); \
																				\
				ret = EASTL_ATOMIC_TYPE_PUN_CAST(type, retIntegral);			\
			}

	#endif


	#define EASTL_ARCH_ATOMIC_X86_LOAD_128(type, ret, ptr, MemoryOrder)		\
		{																	\
			EASTL_ATOMIC_FIXED_WIDTH_TYPE_128 expected{0, 0};				\
			ret = EASTL_ATOMIC_TYPE_PUN_CAST(type, expected);				\
																			\
			bool cmpxchgRetBool; EA_UNUSED(cmpxchgRetBool);					\
			EA_PREPROCESSOR_JOIN(EA_PREPROCESSOR_JOIN(EASTL_ATOMIC_CMPXCHG_STRONG_, MemoryOrder), _128)(type, cmpxchgRetBool, ptr, &(ret), ret); \
		}


	#define EASTL_ARCH_ATOMIC_X86_LOAD_8(type, ret, ptr)		\
		EASTL_ARCH_ATOMIC_X86_LOAD_N(__int8, 8, type, ret, ptr)

	#define EASTL_ARCH_ATOMIC_X86_LOAD_16(type, ret, ptr)		\
		EASTL_ARCH_ATOMIC_X86_LOAD_N(__int16, 16, type, ret, ptr)

	#define EASTL_ARCH_ATOMIC_X86_LOAD_32(type, ret, ptr)		\
		EASTL_ARCH_ATOMIC_X86_LOAD_N(__int32, 32, type, ret, ptr)

	#define EASTL_ARCH_ATOMIC_X86_LOAD_64(type, ret, ptr)		\
		EASTL_ARCH_ATOMIC_X86_LOAD_N(__int64, 64, type, ret, ptr)


	#define EASTL_ARCH_ATOMIC_LOAD_RELAXED_8(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_8(type, ret, ptr)

	#define EASTL_ARCH_ATOMIC_LOAD_RELAXED_16(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_16(type, ret, ptr)

	#define EASTL_ARCH_ATOMIC_LOAD_RELAXED_32(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_32(type, ret, ptr)

	#define EASTL_ARCH_ATOMIC_LOAD_RELAXED_64(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_64(type, ret, ptr)

	#define EASTL_ARCH_ATOMIC_LOAD_RELAXED_128(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_128(type, ret, ptr, RELAXED)


	#define EASTL_ARCH_ATOMIC_LOAD_ACQUIRE_8(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_8(type, ret, ptr);			\
		EASTL_ATOMIC_COMPILER_BARRIER()

	#define EASTL_ARCH_ATOMIC_LOAD_ACQUIRE_16(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_16(type, ret, ptr);			\
		EASTL_ATOMIC_COMPILER_BARRIER()

	#define EASTL_ARCH_ATOMIC_LOAD_ACQUIRE_32(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_32(type, ret, ptr);			\
		EASTL_ATOMIC_COMPILER_BARRIER()

	#define EASTL_ARCH_ATOMIC_LOAD_ACQUIRE_64(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_64(type, ret, ptr);			\
		EASTL_ATOMIC_COMPILER_BARRIER()

	#define EASTL_ARCH_ATOMIC_LOAD_ACQUIRE_128(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_128(type, ret, ptr, ACQUIRE)


	#define EASTL_ARCH_ATOMIC_LOAD_SEQ_CST_8(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_8(type, ret, ptr);			\
		EASTL_ATOMIC_COMPILER_BARRIER()

	#define EASTL_ARCH_ATOMIC_LOAD_SEQ_CST_16(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_16(type, ret, ptr);			\
		EASTL_ATOMIC_COMPILER_BARRIER()

	#define EASTL_ARCH_ATOMIC_LOAD_SEQ_CST_32(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_32(type, ret, ptr);			\
		EASTL_ATOMIC_COMPILER_BARRIER()

	#define EASTL_ARCH_ATOMIC_LOAD_SEQ_CST_64(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_64(type, ret, ptr);			\
		EASTL_ATOMIC_COMPILER_BARRIER()

	#define EASTL_ARCH_ATOMIC_LOAD_SEQ_CST_128(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_128(type, ret, ptr, SEQ_CST)


#endif


#if ((defined(EA_COMPILER_CLANG) || defined(EA_COMPILER_GNUC)) && defined(EA_PROCESSOR_X86_64))


	/**
	 * NOTE:
	 *
	 * Since the cmpxchg 128-bit inline assembly does a sete in the asm to set the return boolean,
	 * it doesn't get dead-store removed even though we don't care about the success of the
	 * cmpxchg since the compiler cannot reason about what is inside asm blocks.
	 * Thus this variant just does the minimum required to do an atomic load.
	 */
	#define EASTL_ARCH_ATOMIC_X86_LOAD_128(type, ret, ptr, MemoryOrder)		\
		{																	\
			EASTL_ATOMIC_FIXED_WIDTH_TYPE_128 expected = 0;					\
			ret = EASTL_ATOMIC_TYPE_PUN_CAST(type, expected);				\
																			\
			/* Compare RDX:RAX with m128. If equal, set ZF and load RCX:RBX into m128. Else, clear ZF and load m128 into RDX:RAX. */ \
			__asm__ __volatile__ ("lock; cmpxchg16b %2" /* cmpxchg16b sets/clears ZF */ \
								  /* Output  Operands */					\
								  : "=a"((EASTL_ATOMIC_TYPE_CAST(uint64_t, &(ret)))[0]), "=d"((EASTL_ATOMIC_TYPE_CAST(uint64_t, &(ret)))[1]), \
									"+m"(*(EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(__uint128_t, (ptr)))) \
								  /* Input Operands */						\
								  : "b"((EASTL_ATOMIC_TYPE_CAST(uint64_t, &(ret)))[0]), "c"((EASTL_ATOMIC_TYPE_CAST(uint64_t, &(ret)))[1]), \
									"a"((EASTL_ATOMIC_TYPE_CAST(uint64_t, &(ret)))[0]), "d"((EASTL_ATOMIC_TYPE_CAST(uint64_t, &(ret)))[1]) \
								  /* Clobbers */							\
								  : "memory", "cc");						\
		}


	#define EASTL_ARCH_ATOMIC_LOAD_RELAXED_128(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_128(type, ret, ptr, RELAXED)

	#define EASTL_ARCH_ATOMIC_LOAD_ACQUIRE_128(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_128(type, ret, ptr, ACQUIRE)

	#define EASTL_ARCH_ATOMIC_LOAD_SEQ_CST_128(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X86_LOAD_128(type, ret, ptr, SEQ_CST)


#endif


#endif /* EASTL_ATOMIC_INTERNAL_ARCH_X86_LOAD_H */
