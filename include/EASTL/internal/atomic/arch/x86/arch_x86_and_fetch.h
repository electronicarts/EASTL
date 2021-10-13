/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_ARCH_X86_AND_FETCH_H
#define EASTL_ATOMIC_INTERNAL_ARCH_X86_AND_FETCH_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ARCH_ATOMIC_AND_FETCH_*_N(type, type ret, type * ptr, type val)
//
#if defined(EA_COMPILER_MSVC) && defined(EA_PROCESSOR_X86)


	#define EASTL_ARCH_ATOMIC_X86_AND_FETCH_PRE_COMPUTE_DESIRED(ret, observed, val) \
		ret = ((observed) & (val))

	#define EASTL_ARCH_ATOMIC_X86_AND_FETCH_POST_COMPUTE_RET(ret, prevObserved, val) \
		ret = ((prevObserved) & (val))


	#define EASTL_ARCH_ATOMIC_AND_FETCH_RELAXED_64(type, ret, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_OP_64_IMPL(type, ret, ptr, val, RELAXED,		\
										 EASTL_ARCH_ATOMIC_X86_AND_FETCH_PRE_COMPUTE_DESIRED, \
										 EASTL_ARCH_ATOMIC_X86_AND_FETCH_POST_COMPUTE_RET)

	#define EASTL_ARCH_ATOMIC_AND_FETCH_ACQUIRE_64(type, ret, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_OP_64_IMPL(type, ret, ptr, val, ACQUIRE,		\
										 EASTL_ARCH_ATOMIC_X86_AND_FETCH_PRE_COMPUTE_DESIRED, \
										 EASTL_ARCH_ATOMIC_X86_AND_FETCH_POST_COMPUTE_RET)

	#define EASTL_ARCH_ATOMIC_AND_FETCH_RELEASE_64(type, ret, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_OP_64_IMPL(type, ret, ptr, val, RELEASE,		\
										 EASTL_ARCH_ATOMIC_X86_AND_FETCH_PRE_COMPUTE_DESIRED, \
										 EASTL_ARCH_ATOMIC_X86_AND_FETCH_POST_COMPUTE_RET)

	#define EASTL_ARCH_ATOMIC_AND_FETCH_ACQ_REL_64(type, ret, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_OP_64_IMPL(type, ret, ptr, val, ACQ_REL,		\
										 EASTL_ARCH_ATOMIC_X86_AND_FETCH_PRE_COMPUTE_DESIRED, \
										 EASTL_ARCH_ATOMIC_X86_AND_FETCH_POST_COMPUTE_RET)

	#define EASTL_ARCH_ATOMIC_AND_FETCH_SEQ_CST_64(type, ret, ptr, val)		\
		EASTL_ARCH_ATOMIC_X86_OP_64_IMPL(type, ret, ptr, val, SEQ_CST,		\
										 EASTL_ARCH_ATOMIC_X86_AND_FETCH_PRE_COMPUTE_DESIRED, \
										 EASTL_ARCH_ATOMIC_X86_AND_FETCH_POST_COMPUTE_RET)


#endif


#if ((defined(__clang__) || defined(EA_COMPILER_GNUC)) && defined(EA_PROCESSOR_X86_64))


	#define EASTL_ARCH_ATOMIC_X86_AND_FETCH_PRE_COMPUTE_DESIRED(ret, observed, val) \
		ret = ((observed) & (val))

	#define EASTL_ARCH_ATOMIC_X86_AND_FETCH_POST_COMPUTE_RET(ret, prevObserved, val) \
		ret = ((prevObserved) & (val))


	#define EASTL_ARCH_ATOMIC_AND_FETCH_RELAXED_128(type, ret, ptr, val)	\
		EASTL_ARCH_ATOMIC_X86_OP_128_IMPL(type, ret, ptr, val, RELAXED,		\
										  EASTL_ARCH_ATOMIC_X86_AND_FETCH_PRE_COMPUTE_DESIRED, \
										  EASTL_ARCH_ATOMIC_X86_AND_FETCH_POST_COMPUTE_RET)

	#define EASTL_ARCH_ATOMIC_AND_FETCH_ACQUIRE_128(type, ret, ptr, val)	\
		EASTL_ARCH_ATOMIC_X86_OP_128_IMPL(type, ret, ptr, val, ACQUIRE,		\
										  EASTL_ARCH_ATOMIC_X86_AND_FETCH_PRE_COMPUTE_DESIRED, \
										  EASTL_ARCH_ATOMIC_X86_AND_FETCH_POST_COMPUTE_RET)

	#define EASTL_ARCH_ATOMIC_AND_FETCH_RELEASE_128(type, ret, ptr, val)	\
		EASTL_ARCH_ATOMIC_X86_OP_128_IMPL(type, ret, ptr, val, RELEASE,		\
										  EASTL_ARCH_ATOMIC_X86_AND_FETCH_PRE_COMPUTE_DESIRED, \
										  EASTL_ARCH_ATOMIC_X86_AND_FETCH_POST_COMPUTE_RET)

	#define EASTL_ARCH_ATOMIC_AND_FETCH_ACQ_REL_128(type, ret, ptr, val)	\
		EASTL_ARCH_ATOMIC_X86_OP_128_IMPL(type, ret, ptr, val, ACQ_REL,		\
										  EASTL_ARCH_ATOMIC_X86_AND_FETCH_PRE_COMPUTE_DESIRED, \
										  EASTL_ARCH_ATOMIC_X86_AND_FETCH_POST_COMPUTE_RET)

	#define EASTL_ARCH_ATOMIC_AND_FETCH_SEQ_CST_128(type, ret, ptr, val)	\
		EASTL_ARCH_ATOMIC_X86_OP_128_IMPL(type, ret, ptr, val, SEQ_CST,		\
										  EASTL_ARCH_ATOMIC_X86_AND_FETCH_PRE_COMPUTE_DESIRED, \
										  EASTL_ARCH_ATOMIC_X86_AND_FETCH_POST_COMPUTE_RET)


#endif


#endif /* EASTL_ATOMIC_INTERNAL_ARCH_X86_AND_FETCH_H */
