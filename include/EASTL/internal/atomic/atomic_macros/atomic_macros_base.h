/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_MACROS_BASE_H
#define EASTL_ATOMIC_INTERNAL_MACROS_BASE_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


#define EASTL_ATOMIC_INTERNAL_COMPILER_AVAILABLE(op)					\
	EA_PREPROCESSOR_JOIN(EA_PREPROCESSOR_JOIN(EASTL_COMPILER_, op), _AVAILABLE)

#define EASTL_ATOMIC_INTERNAL_ARCH_AVAILABLE(op)						\
	EA_PREPROCESSOR_JOIN(EA_PREPROCESSOR_JOIN(EASTL_ARCH_, op), _AVAILABLE)

#define EASTL_ATOMIC_INTERNAL_NOT_IMPLEMENTED_ERROR(...)				\
	static_assert(false, "eastl::atomic<T> atomic macro not implemented!")


/* Compiler && Arch Not Implemented */
#define EASTL_ATOMIC_INTERNAL_OP_PATTERN_00(op) \
	EASTL_ATOMIC_INTERNAL_NOT_IMPLEMENTED_ERROR

/* Arch Implemented */
#define EASTL_ATOMIC_INTERNAL_OP_PATTERN_01(op) \
	EA_PREPROCESSOR_JOIN(EASTL_ARCH_, op)

/* Compiler Implmented */
#define EASTL_ATOMIC_INTERNAL_OP_PATTERN_10(op) \
	EA_PREPROCESSOR_JOIN(EASTL_COMPILER_, op)

/* Compiler && Arch Implemented */
#define EASTL_ATOMIC_INTERNAL_OP_PATTERN_11(op) \
	EA_PREPROCESSOR_JOIN(EASTL_ARCH_, op)


/* This macro creates the pattern macros above for the 2x2 True-False truth table */
#define EASTL_ATOMIC_INTERNAL_OP_HELPER1(compiler, arch, op)			\
	EA_PREPROCESSOR_JOIN(EASTL_ATOMIC_INTERNAL_OP_PATTERN_, EA_PREPROCESSOR_JOIN(compiler, arch))(op)


/////////////////////////////////////////////////////////////////////////////////
//
// EASTL_ATOMIC_CHOOSE_OP_IMPL
//
// This macro chooses between the compiler or architecture implementation for a
// given atomic operation.
//
// USAGE:
//
// EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_ADD_RELAXED_8)(ret, ptr, val)
//
#define EASTL_ATOMIC_CHOOSE_OP_IMPL(op)					\
	EASTL_ATOMIC_INTERNAL_OP_HELPER1(					\
		EASTL_ATOMIC_INTERNAL_COMPILER_AVAILABLE(op),	\
		EASTL_ATOMIC_INTERNAL_ARCH_AVAILABLE(op),		\
		op												\
		)


#endif /* EASTL_ATOMIC_INTERNAL_MACROS_BASE_H */
