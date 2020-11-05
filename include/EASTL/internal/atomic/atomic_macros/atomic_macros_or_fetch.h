/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_MACROS_OR_FETCH_H
#define EASTL_ATOMIC_INTERNAL_MACROS_OR_FETCH_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ATOMIC_OR_FETCH_*_N(type, type ret, type * ptr, type val)
//
#define EASTL_ATOMIC_OR_FETCH_RELAXED_8(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_RELAXED_8)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_ACQUIRE_8(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_ACQUIRE_8)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_RELEASE_8(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_RELEASE_8)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_ACQ_REL_8(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_ACQ_REL_8)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_SEQ_CST_8(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_SEQ_CST_8)(type, ret, ptr, val)


#define EASTL_ATOMIC_OR_FETCH_RELAXED_16(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_RELAXED_16)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_ACQUIRE_16(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_ACQUIRE_16)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_RELEASE_16(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_RELEASE_16)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_ACQ_REL_16(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_ACQ_REL_16)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_SEQ_CST_16(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_SEQ_CST_16)(type, ret, ptr, val)


#define EASTL_ATOMIC_OR_FETCH_RELAXED_32(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_RELAXED_32)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_ACQUIRE_32(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_ACQUIRE_32)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_RELEASE_32(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_RELEASE_32)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_ACQ_REL_32(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_ACQ_REL_32)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_SEQ_CST_32(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_SEQ_CST_32)(type, ret, ptr, val)


#define EASTL_ATOMIC_OR_FETCH_RELAXED_64(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_RELAXED_64)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_ACQUIRE_64(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_ACQUIRE_64)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_RELEASE_64(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_RELEASE_64)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_ACQ_REL_64(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_ACQ_REL_64)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_SEQ_CST_64(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_SEQ_CST_64)(type, ret, ptr, val)


#define EASTL_ATOMIC_OR_FETCH_RELAXED_128(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_RELAXED_128)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_ACQUIRE_128(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_ACQUIRE_128)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_RELEASE_128(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_RELEASE_128)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_ACQ_REL_128(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_ACQ_REL_128)(type, ret, ptr, val)

#define EASTL_ATOMIC_OR_FETCH_SEQ_CST_128(type, ret, ptr, val)			\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_OR_FETCH_SEQ_CST_128)(type, ret, ptr, val)


#endif /* EASTL_ATOMIC_INTERNAL_MACROS_OR_FETCH_H */
