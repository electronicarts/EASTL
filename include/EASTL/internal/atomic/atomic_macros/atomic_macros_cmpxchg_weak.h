/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_MACROS_CMPXCHG_WEAK_H
#define EASTL_ATOMIC_INTERNAL_MACROS_CMPXCHG_WEAK_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ATOMIC_CMPXCHG_WEAK_*_*_N(type, bool ret, type * ptr, type * expected, type desired)
//
#define EASTL_ATOMIC_CMPXCHG_WEAK_RELAXED_RELAXED_8(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELAXED_RELAXED_8)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQUIRE_RELAXED_8(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQUIRE_RELAXED_8)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQUIRE_ACQUIRE_8(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQUIRE_ACQUIRE_8)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_RELEASE_RELAXED_8(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELEASE_RELAXED_8)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQ_REL_RELAXED_8(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQ_REL_RELAXED_8)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQ_REL_ACQUIRE_8(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQ_REL_ACQUIRE_8)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_RELAXED_8(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_RELAXED_8)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_ACQUIRE_8(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_ACQUIRE_8)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_SEQ_CST_8(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_SEQ_CST_8)(type, ret, ptr, expected, desired)


#define EASTL_ATOMIC_CMPXCHG_WEAK_RELAXED_RELAXED_16(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELAXED_RELAXED_16)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQUIRE_RELAXED_16(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQUIRE_RELAXED_16)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQUIRE_ACQUIRE_16(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQUIRE_ACQUIRE_16)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_RELEASE_RELAXED_16(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELEASE_RELAXED_16)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQ_REL_RELAXED_16(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQ_REL_RELAXED_16)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQ_REL_ACQUIRE_16(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQ_REL_ACQUIRE_16)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_RELAXED_16(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_RELAXED_16)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_ACQUIRE_16(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_ACQUIRE_16)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_SEQ_CST_16(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_SEQ_CST_16)(type, ret, ptr, expected, desired)


#define EASTL_ATOMIC_CMPXCHG_WEAK_RELAXED_RELAXED_32(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELAXED_RELAXED_32)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQUIRE_RELAXED_32(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQUIRE_RELAXED_32)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQUIRE_ACQUIRE_32(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQUIRE_ACQUIRE_32)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_RELEASE_RELAXED_32(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELEASE_RELAXED_32)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQ_REL_RELAXED_32(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQ_REL_RELAXED_32)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQ_REL_ACQUIRE_32(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQ_REL_ACQUIRE_32)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_RELAXED_32(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_RELAXED_32)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_ACQUIRE_32(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_ACQUIRE_32)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_SEQ_CST_32(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_SEQ_CST_32)(type, ret, ptr, expected, desired)


#define EASTL_ATOMIC_CMPXCHG_WEAK_RELAXED_RELAXED_64(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELAXED_RELAXED_64)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQUIRE_RELAXED_64(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQUIRE_RELAXED_64)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQUIRE_ACQUIRE_64(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQUIRE_ACQUIRE_64)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_RELEASE_RELAXED_64(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELEASE_RELAXED_64)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQ_REL_RELAXED_64(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQ_REL_RELAXED_64)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQ_REL_ACQUIRE_64(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQ_REL_ACQUIRE_64)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_RELAXED_64(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_RELAXED_64)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_ACQUIRE_64(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_ACQUIRE_64)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_SEQ_CST_64(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_SEQ_CST_64)(type, ret, ptr, expected, desired)


#define EASTL_ATOMIC_CMPXCHG_WEAK_RELAXED_RELAXED_128(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELAXED_RELAXED_128)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQUIRE_RELAXED_128(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQUIRE_RELAXED_128)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQUIRE_ACQUIRE_128(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQUIRE_ACQUIRE_128)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_RELEASE_RELAXED_128(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELEASE_RELAXED_128)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQ_REL_RELAXED_128(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQ_REL_RELAXED_128)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQ_REL_ACQUIRE_128(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQ_REL_ACQUIRE_128)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_RELAXED_128(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_RELAXED_128)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_ACQUIRE_128(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_ACQUIRE_128)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_SEQ_CST_128(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_SEQ_CST_128)(type, ret, ptr, expected, desired)


/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ATOMIC_CMPXCHG_WEAK_*(bool ret, type * ptr, type * expected, type desired)
//
#define EASTL_ATOMIC_CMPXCHG_WEAK_RELAXED_8(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELAXED_8)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQUIRE_8(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQUIRE_8)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_RELEASE_8(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELEASE_8)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQ_REL_8(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQ_REL_8)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_8(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_8)(type, ret, ptr, expected, desired)


#define EASTL_ATOMIC_CMPXCHG_WEAK_RELAXED_16(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELAXED_16)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQUIRE_16(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQUIRE_16)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_RELEASE_16(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELEASE_16)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQ_REL_16(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQ_REL_16)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_16(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_16)(type, ret, ptr, expected, desired)


#define EASTL_ATOMIC_CMPXCHG_WEAK_RELAXED_32(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELAXED_32)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQUIRE_32(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQUIRE_32)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_RELEASE_32(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELEASE_32)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQ_REL_32(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQ_REL_32)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_32(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_32)(type, ret, ptr, expected, desired)


#define EASTL_ATOMIC_CMPXCHG_WEAK_RELAXED_64(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELAXED_64)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQUIRE_64(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQUIRE_64)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_RELEASE_64(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELEASE_64)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQ_REL_64(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQ_REL_64)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_64(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_64)(type, ret, ptr, expected, desired)


#define EASTL_ATOMIC_CMPXCHG_WEAK_RELAXED_128(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELAXED_128)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQUIRE_128(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQUIRE_128)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_RELEASE_128(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_RELEASE_128)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_ACQ_REL_128(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_ACQ_REL_128)(type, ret, ptr, expected, desired)

#define EASTL_ATOMIC_CMPXCHG_WEAK_SEQ_CST_128(type, ret, ptr, expected, desired) \
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_CMPXCHG_WEAK_SEQ_CST_128)(type, ret, ptr, expected, desired)


#endif /* EASTL_ATOMIC_INTERNAL_MACROS_CMPXCHG_WEAK_H */
