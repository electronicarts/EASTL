/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_MACROS_FETCH_XOR_H
#define EASTL_ATOMIC_INTERNAL_MACROS_FETCH_XOR_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ATOMIC_FETCH_XOR_*_N(type, type ret, type * ptr, type val)
//
#define EASTL_ATOMIC_FETCH_XOR_RELAXED_8(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_RELAXED_8)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_ACQUIRE_8(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_ACQUIRE_8)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_RELEASE_8(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_RELEASE_8)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_ACQ_REL_8(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_ACQ_REL_8)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_SEQ_CST_8(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_SEQ_CST_8)(type, ret, ptr, val)


#define EASTL_ATOMIC_FETCH_XOR_RELAXED_16(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_RELAXED_16)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_ACQUIRE_16(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_ACQUIRE_16)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_RELEASE_16(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_RELEASE_16)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_ACQ_REL_16(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_ACQ_REL_16)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_SEQ_CST_16(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_SEQ_CST_16)(type, ret, ptr, val)


#define EASTL_ATOMIC_FETCH_XOR_RELAXED_32(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_RELAXED_32)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_ACQUIRE_32(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_ACQUIRE_32)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_RELEASE_32(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_RELEASE_32)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_ACQ_REL_32(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_ACQ_REL_32)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_SEQ_CST_32(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_SEQ_CST_32)(type, ret, ptr, val)


#define EASTL_ATOMIC_FETCH_XOR_RELAXED_64(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_RELAXED_64)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_ACQUIRE_64(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_ACQUIRE_64)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_RELEASE_64(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_RELEASE_64)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_ACQ_REL_64(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_ACQ_REL_64)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_SEQ_CST_64(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_SEQ_CST_64)(type, ret, ptr, val)


#define EASTL_ATOMIC_FETCH_XOR_RELAXED_128(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_RELAXED_128)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_ACQUIRE_128(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_ACQUIRE_128)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_RELEASE_128(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_RELEASE_128)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_ACQ_REL_128(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_ACQ_REL_128)(type, ret, ptr, val)

#define EASTL_ATOMIC_FETCH_XOR_SEQ_CST_128(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_FETCH_XOR_SEQ_CST_128)(type, ret, ptr, val)


#endif /* EASTL_ATOMIC_INTERNAL_MACROS_FETCH_XOR_H */
