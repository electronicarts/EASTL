/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_MACROS_LOAD_H
#define EASTL_ATOMIC_INTERNAL_MACROS_LOAD_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// void EASTL_ATOMIC_LOAD_*_N(type, type ret, type * ptr)
//
#define EASTL_ATOMIC_LOAD_RELAXED_8(type, ret, ptr)						\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_RELAXED_8)(type, ret, ptr)

#define EASTL_ATOMIC_LOAD_ACQUIRE_8(type, ret, ptr)						\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_ACQUIRE_8)(type, ret, ptr)

#define EASTL_ATOMIC_LOAD_SEQ_CST_8(type, ret, ptr)						\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_SEQ_CST_8)(type, ret, ptr)


#define EASTL_ATOMIC_LOAD_RELAXED_16(type, ret, ptr)					\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_RELAXED_16)(type, ret, ptr)

#define EASTL_ATOMIC_LOAD_ACQUIRE_16(type, ret, ptr)					\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_ACQUIRE_16)(type, ret, ptr)

#define EASTL_ATOMIC_LOAD_SEQ_CST_16(type, ret, ptr)					\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_SEQ_CST_16)(type, ret, ptr)


#define EASTL_ATOMIC_LOAD_RELAXED_32(type, ret, ptr)					\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_RELAXED_32)(type, ret, ptr)

#define EASTL_ATOMIC_LOAD_ACQUIRE_32(type, ret, ptr)					\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_ACQUIRE_32)(type, ret, ptr)

#define EASTL_ATOMIC_LOAD_SEQ_CST_32(type, ret, ptr)					\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_SEQ_CST_32)(type, ret, ptr)


#define EASTL_ATOMIC_LOAD_RELAXED_64(type, ret, ptr)					\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_RELAXED_64)(type, ret, ptr)

#define EASTL_ATOMIC_LOAD_ACQUIRE_64(type, ret, ptr)					\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_ACQUIRE_64)(type, ret, ptr)

#define EASTL_ATOMIC_LOAD_SEQ_CST_64(type, ret, ptr)					\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_SEQ_CST_64)(type, ret, ptr)


#define EASTL_ATOMIC_LOAD_RELAXED_128(type, ret, ptr)					\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_RELAXED_128)(type, ret, ptr)

#define EASTL_ATOMIC_LOAD_ACQUIRE_128(type, ret, ptr)					\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_ACQUIRE_128)(type, ret, ptr)

#define EASTL_ATOMIC_LOAD_SEQ_CST_128(type, ret, ptr)					\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_SEQ_CST_128)(type, ret, ptr)


#define EASTL_ATOMIC_LOAD_READ_DEPENDS_32(type, ret, ptr)				\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_READ_DEPENDS_32)(type, ret, ptr)

#define EASTL_ATOMIC_LOAD_READ_DEPENDS_64(type, ret, ptr)				\
	EASTL_ATOMIC_CHOOSE_OP_IMPL(ATOMIC_LOAD_READ_DEPENDS_64)(type, ret, ptr)


#endif /* EASTL_ATOMIC_INTERNAL_MACROS_LOAD_H */
