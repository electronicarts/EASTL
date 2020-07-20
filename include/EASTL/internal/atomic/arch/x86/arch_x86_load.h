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
				retIntegral = *(EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(integralType, (ptr))); \
																				\
				ret = EASTL_ATOMIC_TYPE_PUN_CAST(type, retIntegral);			\
			}

	#endif


	#define EASTL_ARCH_ATOMIC_X64_LOAD_128(type, ret, ptr, MemoryOrder)		\
		{																	\
			struct BitfieldPun128											\
			{																\
				__int64 value[2];											\
			};																\
																			\
			struct BitfieldPun128 expectedPun{0, 0};						\
			ret = EASTL_ATOMIC_TYPE_PUN_CAST(type, expectedPun);			\
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
		EASTL_ARCH_ATOMIC_X64_LOAD_128(type, ret, ptr, RELAXED)


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

	#define EASTL_ARCH_ATOMIC_LOAD_ACQUIRE_128(type, ret, ptr)		\
		EASTL_ARCH_ATOMIC_X64_LOAD_128(type, ret, ptr, ACQUIRE);	\
		EASTL_ATOMIC_COMPILER_BARRIER()


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

	#define EASTL_ARCH_ATOMIC_LOAD_SEQ_CST_128(type, ret, ptr)		\
		EASTL_ARCH_ATOMIC_X64_LOAD_128(type, ret, ptr, SEQ_CST);	\
		EASTL_ATOMIC_COMPILER_BARRIER()


#endif


#if ((defined(EA_COMPILER_CLANG) || defined(EA_COMPILER_GNUC)) && defined(EA_PROCESSOR_X86_64))


	#define EASTL_ARCH_ATOMIC_X64_LOAD_128(type, ret, ptr, MemoryOrder)		\
		{																	\
			__uint128_t expected = 0;										\
			ret = EASTL_ATOMIC_TYPE_PUN_CAST(type, expected);				\
																			\
			bool cmpxchgRetBool; EA_UNUSED(cmpxchgRetBool);					\
			EA_PREPROCESSOR_JOIN(EA_PREPROCESSOR_JOIN(EASTL_ATOMIC_CMPXCHG_STRONG_, MemoryOrder), _128)(type, cmpxchgRetBool, ptr, &(ret), ret); \
		}


	#define EASTL_ARCH_ATOMIC_LOAD_RELAXED_128(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X64_LOAD_128(type, ret, ptr, RELAXED)

	#define EASTL_ARCH_ATOMIC_LOAD_ACQUIRE_128(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X64_LOAD_128(type, ret, ptr, ACQUIRE)

	#define EASTL_ARCH_ATOMIC_LOAD_SEQ_CST_128(type, ret, ptr)	\
		EASTL_ARCH_ATOMIC_X64_LOAD_128(type, ret, ptr, SEQ_CST)


#endif


#endif /* EASTL_ATOMIC_INTERNAL_ARCH_X86_LOAD_H */
