/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_ARCH_X86_CMPXCHG_STRONG_H
#define EASTL_ATOMIC_INTERNAL_ARCH_X86_CMPXCHG_STRONG_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ARCH_ATOMIC_CMPXCHG_STRONG_*_*_N(type, bool ret, type * ptr, type * expected, type desired)
//
#if ((defined(__clang__) || defined(EA_COMPILER_GNUC)) && defined(EA_PROCESSOR_X86_64))


	#define EASTL_ARCH_ATOMIC_X86_CMPXCHG_STRONG_128_IMPL(type, ret, ptr, expected, desired) \
		{																	\
			/* Compare RDX:RAX with m128. If equal, set ZF and load RCX:RBX into m128. Else, clear ZF and load m128 into RDX:RAX. */ \
			__asm__ __volatile__ ("lock; cmpxchg16b %2\n" /* cmpxchg16b sets/clears ZF */ \
								  "sete %3" /* If ZF == 1, set the return value to 1 */ \
								  /* Output  Operands */					\
								  : "=a"((EASTL_ATOMIC_TYPE_CAST(uint64_t, (expected)))[0]), "=d"((EASTL_ATOMIC_TYPE_CAST(uint64_t, (expected)))[1]), \
									"+m"(*(EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(__uint128_t, (ptr)))), \
									"=rm"((ret))							\
								  /* Input Operands */						\
								  : "b"((EASTL_ATOMIC_TYPE_CAST(uint64_t, &(desired)))[0]), "c"((EASTL_ATOMIC_TYPE_CAST(uint64_t, &(desired)))[1]), \
									"a"((EASTL_ATOMIC_TYPE_CAST(uint64_t, (expected)))[0]), "d"((EASTL_ATOMIC_TYPE_CAST(uint64_t, (expected)))[1]) \
								  /* Clobbers */							\
								  : "memory", "cc");						\
		}


	#define EASTL_ARCH_ATOMIC_CMPXCHG_STRONG_RELAXED_RELAXED_128(type, ret, ptr, expected, desired) \
		EASTL_ARCH_ATOMIC_X86_CMPXCHG_STRONG_128_IMPL(type, ret, ptr, expected, desired)

	#define EASTL_ARCH_ATOMIC_CMPXCHG_STRONG_ACQUIRE_RELAXED_128(type, ret, ptr, expected, desired) \
		EASTL_ARCH_ATOMIC_X86_CMPXCHG_STRONG_128_IMPL(type, ret, ptr, expected, desired)

	#define EASTL_ARCH_ATOMIC_CMPXCHG_STRONG_ACQUIRE_ACQUIRE_128(type, ret, ptr, expected, desired) \
		EASTL_ARCH_ATOMIC_X86_CMPXCHG_STRONG_128_IMPL(type, ret, ptr, expected, desired)

	#define EASTL_ARCH_ATOMIC_CMPXCHG_STRONG_RELEASE_RELAXED_128(type, ret, ptr, expected, desired) \
		EASTL_ARCH_ATOMIC_X86_CMPXCHG_STRONG_128_IMPL(type, ret, ptr, expected, desired)

	#define EASTL_ARCH_ATOMIC_CMPXCHG_STRONG_ACQ_REL_RELAXED_128(type, ret, ptr, expected, desired) \
		EASTL_ARCH_ATOMIC_X86_CMPXCHG_STRONG_128_IMPL(type, ret, ptr, expected, desired)

	#define EASTL_ARCH_ATOMIC_CMPXCHG_STRONG_ACQ_REL_ACQUIRE_128(type, ret, ptr, expected, desired) \
		EASTL_ARCH_ATOMIC_X86_CMPXCHG_STRONG_128_IMPL(type, ret, ptr, expected, desired)

	#define EASTL_ARCH_ATOMIC_CMPXCHG_STRONG_SEQ_CST_RELAXED_128(type, ret, ptr, expected, desired) \
		EASTL_ARCH_ATOMIC_X86_CMPXCHG_STRONG_128_IMPL(type, ret, ptr, expected, desired)

	#define EASTL_ARCH_ATOMIC_CMPXCHG_STRONG_SEQ_CST_ACQUIRE_128(type, ret, ptr, expected, desired) \
		EASTL_ARCH_ATOMIC_X86_CMPXCHG_STRONG_128_IMPL(type, ret, ptr, expected, desired)

	#define EASTL_ARCH_ATOMIC_CMPXCHG_STRONG_SEQ_CST_SEQ_CST_128(type, ret, ptr, expected, desired) \
		EASTL_ARCH_ATOMIC_X86_CMPXCHG_STRONG_128_IMPL(type, ret, ptr, expected, desired)


#endif


#endif /* EASTL_ATOMIC_INTERNAL_ARCH_X86_CMPXCHG_STRONG_H */
