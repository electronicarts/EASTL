/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <EASTL/atomic.h>

// This header provides extension functions for atomically operating on non-atomic types with
// specific memory order semantics. These functions should be used as a last resort when using
// the standard atomic types is impossible (for example due to having to cross established API
// boundaries which can't change).
//
// The reason the atomic<T> template should be preferred is that all operations on that type
// are atomic by design, using the functions provided by this header and mixing atomic and
// non-atomic accesses leads to potential data races unless done with extreme care. However, as
// mentioned above sometimes there are circumstances where using atomic<T> is
// unfeasible. Whenever this API is used, it is recommended to verify that your data access
// assumptions are consistent and correct by using tools like TSAN (thread sanitizer).
//
// For every member function of the form atomic<T>::foo(...) where T is bool, an integral type,
// or a pointer type, we provide an equivalent free function atomic_raw_foo(T* ptr, ...) which
// operates on the T pointed to by ptr. Specifically:
//
// For bool we provide:
//    - bool atomic_raw_load(bool*, mem_order)
//    - void atomic_raw_store(bool*, bool, mem_order)
//    - bool atomic_raw_exchange(bool*, bool, mem_order)
//    - bool atomic_raw_compare_exchange_weak/strong(bool*, bool&, bool, mem_ord(, mem_ord))
//
// For integral T we provide:
//    - T atomic_raw_load(T*, mem_order)
//    - void atomic_raw_store(T*, T, mem_order)
//    - T atomic_raw_exchange(T*, T, mem_order)
//    - bool atomic_raw_compare_exchange_weak/strong(T*, T&, T, mem_ord(, mem_ord))
//    - T atomic_raw_fetch_add/atomic_raw_add_fetch(T*, T, mem_ord)
//    - T atomic_raw_fetch_sub/atomic_raw_sub_fetch(T*, T, mem_ord)
//    - T atomic_raw_fetch_and/atomic_raw_and_fetch(T*, T, mem_ord)
//    - T atomic_raw_fetch_or / atomic_raw_or_fetch(T*, T, mem_ord)
//    - T atomic_raw_fetch_xor/atomic_raw_xor_fetch(T*, T, mem_ord)
//
// For any T we provide:
//    - T* atomic_raw_load(T**, mem_order)
//    - void atomic_raw_store(T**, T*, mem_order)
//    - T* atomic_raw_exchange(T**, T*, mem_order)
//    - bool atomic_raw_compare_exchange_weak/strong(T**, T*&, T*, mem_ord(, mem_ord))
//    - T* atomic_raw_fetch_add/atomic_raw_add_fetch(T**, T*, mem_ord)
//    - T* atomic_raw_fetch_sub/atomic_raw_sub_fetch(T**, T*, mem_ord)

// For pointer types, the fetch operations which look like:
// T* fetch_add(T** p, ptrdiff_t arg, memory_order)
// T* add_fetch(T** p, ptrdiff_t arg, memory_order)
// The supported operations for pointer types are: add, sub.

#define EASTL_ATOMIC_RAW_OPERATION_IMPL(Operation, OpAlias, p, MemOrderMacro, BitCount)      \
	EA_PREPROCESSOR_JOIN(EA_PREPROCESSOR_JOIN(EASTL_ATOMIC_, Operation), FUNC_WITH_PTR_IMPL) \
	(EASTL_ATOMIC_BASE_OP_JOIN(OpAlias, MemOrderMacro), BitCount, ptr);

#define EASTL_ATOMIC_RAW_SIZE_DEPENDANT_OPERATION_WITH_ALIAS_IMPL(Operation, OpAlias, MemOrderMacro) \
	static_assert(sizeof(T) <= 8, "Atomic functions only support up to 64bit types");                \
	static_assert(eastl::is_integral_v<T> || eastl::is_pointer_v<T>,                                 \
	              "We only support these for integral and pointer types");                           \
	if constexpr (sizeof(T) == 1)                                                                    \
	{                                                                                                \
		EASTL_ATOMIC_RAW_OPERATION_IMPL(Operation, OpAlias, ptr, MemOrderMacro, 8)                   \
	}                                                                                                \
	else if constexpr (sizeof(T) == 2)                                                               \
	{                                                                                                \
		EASTL_ATOMIC_RAW_OPERATION_IMPL(Operation, OpAlias, ptr, MemOrderMacro, 16)                  \
	}                                                                                                \
	else if constexpr (sizeof(T) == 4)                                                               \
	{                                                                                                \
		EASTL_ATOMIC_RAW_OPERATION_IMPL(Operation, OpAlias, ptr, MemOrderMacro, 32)                  \
	}                                                                                                \
	else if constexpr (sizeof(T) == 8)                                                               \
	{                                                                                                \
		EASTL_ATOMIC_RAW_OPERATION_IMPL(Operation, OpAlias, ptr, MemOrderMacro, 64)                  \
	}

#define EASTL_ATOMIC_RAW_SIZE_DEPENDANT_OPERATION_IMPL(Operation, MemOrderMacro) \
	EASTL_ATOMIC_RAW_SIZE_DEPENDANT_OPERATION_WITH_ALIAS_IMPL(Operation, Operation, MemOrderMacro)

#define EASTL_ATOMIC_RAW_LOAD_TMPL(MemOrderType, MemOrderMacro)              \
	template <typename T>                                                    \
	T atomic_raw_load(T* ptr, MemOrderType)                                  \
	{                                                                        \
		EASTL_ATOMIC_RAW_SIZE_DEPENDANT_OPERATION_IMPL(LOAD_, MemOrderMacro) \
	}

// Special memory order only provided for pointers
#define EASTL_ATOMIC_RAW_POINTER_READ_DEPENDS_LOAD_TMPL()                          \
	template <typename T>                                                          \
	T* atomic_raw_load(T** p, internal::memory_order_read_depends_s)               \
	{                                                                              \
		static_assert(sizeof(T*) == 8 || sizeof(T*) == 4, "Invalid pointer size"); \
		if constexpr (sizeof(T*) == 4)                                             \
		{                                                                          \
			EASTL_ATOMIC_POINTER_READ_DEPENDS_LOAD_IMPL(32, p)                     \
		}                                                                          \
		if constexpr (sizeof(T*) == 8)                                             \
		{                                                                          \
			EASTL_ATOMIC_POINTER_READ_DEPENDS_LOAD_IMPL(64, p)                     \
		}                                                                          \
	}

#define EASTL_ATOMIC_RAW_STORE_TMPL(MemOrderType, MemOrderMacro)              \
	template <typename T>                                                     \
	void atomic_raw_store(T* ptr, T desired, MemOrderType)                    \
	{                                                                         \
		EASTL_ATOMIC_RAW_SIZE_DEPENDANT_OPERATION_IMPL(STORE_, MemOrderMacro) \
	}

#define EASTL_ATOMIC_RAW_EXCHANGE_TMPL(MemOrderType, MemOrderMacro)              \
	template <typename T>                                                        \
	T atomic_raw_exchange(T* ptr, T desired, MemOrderType)                       \
	{                                                                            \
		EASTL_ATOMIC_RAW_SIZE_DEPENDANT_OPERATION_IMPL(EXCHANGE_, MemOrderMacro) \
	}

#define EASTL_ATOMIC_RAW_CMPXCHG1_TMPL(MemOrderType, MemOrderMacro)                                         \
	template <typename T>                                                                                   \
	bool atomic_raw_compare_exchange_weak(T* ptr, T& expected, T desired, MemOrderType)                     \
	{                                                                                                       \
		EASTL_ATOMIC_RAW_SIZE_DEPENDANT_OPERATION_WITH_ALIAS_IMPL(CMPXCHG_, CMPXCHG_WEAK_, MemOrderMacro)   \
	}                                                                                                       \
	template <typename T>                                                                                   \
	bool atomic_raw_compare_exchange_strong(T* ptr, T& expected, T desired, MemOrderType)                   \
	{                                                                                                       \
		EASTL_ATOMIC_RAW_SIZE_DEPENDANT_OPERATION_WITH_ALIAS_IMPL(CMPXCHG_, CMPXCHG_STRONG_, MemOrderMacro) \
	}

#define EASTL_ATOMIC_RAW_CMPXCHG2_TMPL(MemOrderType1, MemOrderType2, MemOrderMacro)                         \
	template <typename T>                                                                                   \
	bool atomic_raw_compare_exchange_weak(T* ptr, T& expected, T desired, MemOrderType1, MemOrderType2)     \
	{                                                                                                       \
		EASTL_ATOMIC_RAW_SIZE_DEPENDANT_OPERATION_WITH_ALIAS_IMPL(CMPXCHG_, CMPXCHG_WEAK_, MemOrderMacro)   \
	}                                                                                                       \
	template <typename T>                                                                                   \
	bool atomic_raw_compare_exchange_strong(T* ptr, T& expected, T desired, MemOrderType1, MemOrderType2)   \
	{                                                                                                       \
		EASTL_ATOMIC_RAW_SIZE_DEPENDANT_OPERATION_WITH_ALIAS_IMPL(CMPXCHG_, CMPXCHG_STRONG_, MemOrderMacro) \
	}

#define EASTL_ATOMIC_RAW_INTEGRAL_FETCH_ORDER_TMPL(FuncName, MacroFnName, MemOrderType, MemOrderMacro)                 \
	template <typename T>                                                                                              \
	T EA_PREPROCESSOR_JOIN(atomic_raw_, FuncName)(T * p, T arg, MemOrderType)                                          \
	{                                                                                                                  \
		static_assert(eastl::is_integral_v<T> && !eastl::is_same_v<bool, T>,                                           \
		              "This API is only enabled for integral non boolean types.");                                     \
		if constexpr (sizeof(T) == 1)                                                                                  \
		{                                                                                                              \
			EASTL_ATOMIC_INTEGRAL_FUNC_IMPL(EASTL_ATOMIC_INTEGRAL_FETCH_OP_JOIN(MacroFnName, MemOrderMacro), 8, p, T); \
		}                                                                                                              \
		else if constexpr (sizeof(T) == 2)                                                                             \
		{                                                                                                              \
			EASTL_ATOMIC_INTEGRAL_FUNC_IMPL(EASTL_ATOMIC_INTEGRAL_FETCH_OP_JOIN(MacroFnName, MemOrderMacro), 16, p,    \
			                                T);                                                                        \
		}                                                                                                              \
		else if constexpr (sizeof(T) == 4)                                                                             \
		{                                                                                                              \
			EASTL_ATOMIC_INTEGRAL_FUNC_IMPL(EASTL_ATOMIC_INTEGRAL_FETCH_OP_JOIN(MacroFnName, MemOrderMacro), 32, p,    \
			                                T);                                                                        \
		}                                                                                                              \
		else if constexpr (sizeof(T) == 8)                                                                             \
		{                                                                                                              \
			EASTL_ATOMIC_INTEGRAL_FUNC_IMPL(EASTL_ATOMIC_INTEGRAL_FETCH_OP_JOIN(MacroFnName, MemOrderMacro), 64, p,    \
			                                T);                                                                        \
		}                                                                                                              \
	}

#define EASTL_ATOMIC_RAW_INTEGRAL_FETCH_FUNC_TMPL(FuncName, MacroFnName)                                          \
	EASTL_ATOMIC_RAW_INTEGRAL_FETCH_ORDER_TMPL(FuncName, MacroFnName, internal::memory_order_relaxed_s, RELAXED_) \
	EASTL_ATOMIC_RAW_INTEGRAL_FETCH_ORDER_TMPL(FuncName, MacroFnName, internal::memory_order_acquire_s, ACQUIRE_) \
	EASTL_ATOMIC_RAW_INTEGRAL_FETCH_ORDER_TMPL(FuncName, MacroFnName, internal::memory_order_release_s, RELEASE_) \
	EASTL_ATOMIC_RAW_INTEGRAL_FETCH_ORDER_TMPL(FuncName, MacroFnName, internal::memory_order_acq_rel_s, ACQ_REL_) \
	EASTL_ATOMIC_RAW_INTEGRAL_FETCH_ORDER_TMPL(FuncName, MacroFnName, internal::memory_order_seq_cst_s, SEQ_CST_)

// Ptr version of the fetch functions.
#define EASTL_ATOMIC_RAW_POINTER_FETCH_FUNC_IMPL(FetchOp, BitCount, ptr)                     \
	using ptr_integral_type = EA_PREPROCESSOR_JOIN(EA_PREPROCESSOR_JOIN(int, BitCount), _t); \
	EASTL_ATOMIC_STATIC_ASSERT_TYPE_IS_OBJECT(T);                                            \
	EASTL_ATOMIC_POINTER_FUNC_IMPL(FetchOp, BitCount, ptr);

#define EASTL_ATOMIC_RAW_PTR_FETCH_ORDER_TMPL(FuncName, MacroFnName, MemOrderType, MemOrderMacro)                    \
	template <typename T>                                                                                            \
	T* EA_PREPROCESSOR_JOIN(atomic_raw_, FuncName)(T * *p, ptrdiff_t arg, MemOrderType)                              \
	{                                                                                                                \
		if constexpr (sizeof(T*) == 4)                                                                               \
		{                                                                                                            \
			EASTL_ATOMIC_RAW_POINTER_FETCH_FUNC_IMPL(EASTL_ATOMIC_POINTER_FETCH_OP_JOIN(MacroFnName, MemOrderMacro), \
			                                         32, p)                                                          \
		}                                                                                                            \
		else if constexpr (sizeof(T*) == 8)                                                                          \
		{                                                                                                            \
			EASTL_ATOMIC_RAW_POINTER_FETCH_FUNC_IMPL(EASTL_ATOMIC_POINTER_FETCH_OP_JOIN(MacroFnName, MemOrderMacro), \
			                                         64, p)                                                          \
		}                                                                                                            \
	}

#define EASTL_ATOMIC_RAW_PTR_FETCH_FUNC_TMPL(FuncName, MacroFnName)                                          \
	EASTL_ATOMIC_RAW_PTR_FETCH_ORDER_TMPL(FuncName, MacroFnName, internal::memory_order_relaxed_s, RELAXED_) \
	EASTL_ATOMIC_RAW_PTR_FETCH_ORDER_TMPL(FuncName, MacroFnName, internal::memory_order_acquire_s, ACQUIRE_) \
	EASTL_ATOMIC_RAW_PTR_FETCH_ORDER_TMPL(FuncName, MacroFnName, internal::memory_order_release_s, RELEASE_) \
	EASTL_ATOMIC_RAW_PTR_FETCH_ORDER_TMPL(FuncName, MacroFnName, internal::memory_order_acq_rel_s, ACQ_REL_) \
	EASTL_ATOMIC_RAW_PTR_FETCH_ORDER_TMPL(FuncName, MacroFnName, internal::memory_order_seq_cst_s, SEQ_CST_)

namespace eastl
{
	EASTL_ATOMIC_RAW_LOAD_TMPL(internal::memory_order_relaxed_s, RELAXED_)
	EASTL_ATOMIC_RAW_LOAD_TMPL(internal::memory_order_acquire_s, ACQUIRE_)
	EASTL_ATOMIC_RAW_LOAD_TMPL(internal::memory_order_seq_cst_s, SEQ_CST_)

	// Special memory order for pointers
	EASTL_ATOMIC_RAW_POINTER_READ_DEPENDS_LOAD_TMPL()

	EASTL_ATOMIC_RAW_STORE_TMPL(internal::memory_order_relaxed_s, RELAXED_)
	EASTL_ATOMIC_RAW_STORE_TMPL(internal::memory_order_release_s, RELEASE_)
	EASTL_ATOMIC_RAW_STORE_TMPL(internal::memory_order_seq_cst_s, SEQ_CST_)

	EASTL_ATOMIC_RAW_EXCHANGE_TMPL(internal::memory_order_relaxed_s, RELAXED_)
	EASTL_ATOMIC_RAW_EXCHANGE_TMPL(internal::memory_order_acquire_s, ACQUIRE_)
	EASTL_ATOMIC_RAW_EXCHANGE_TMPL(internal::memory_order_release_s, RELEASE_)
	EASTL_ATOMIC_RAW_EXCHANGE_TMPL(internal::memory_order_acq_rel_s, ACQ_REL_)
	EASTL_ATOMIC_RAW_EXCHANGE_TMPL(internal::memory_order_seq_cst_s, SEQ_CST_)

	EASTL_ATOMIC_RAW_CMPXCHG1_TMPL(internal::memory_order_relaxed_s, RELAXED_)
	EASTL_ATOMIC_RAW_CMPXCHG1_TMPL(internal::memory_order_acquire_s, ACQUIRE_)
	EASTL_ATOMIC_RAW_CMPXCHG1_TMPL(internal::memory_order_release_s, RELEASE_)
	EASTL_ATOMIC_RAW_CMPXCHG1_TMPL(internal::memory_order_acq_rel_s, ACQ_REL_)
	EASTL_ATOMIC_RAW_CMPXCHG1_TMPL(internal::memory_order_seq_cst_s, SEQ_CST_)

	EASTL_ATOMIC_RAW_CMPXCHG2_TMPL(internal::memory_order_relaxed_s, internal::memory_order_relaxed_s, RELAXED_RELAXED_)
	EASTL_ATOMIC_RAW_CMPXCHG2_TMPL(internal::memory_order_acquire_s, internal::memory_order_relaxed_s, ACQUIRE_RELAXED_)
	EASTL_ATOMIC_RAW_CMPXCHG2_TMPL(internal::memory_order_acquire_s, internal::memory_order_acquire_s, ACQUIRE_ACQUIRE_)
	EASTL_ATOMIC_RAW_CMPXCHG2_TMPL(internal::memory_order_release_s, internal::memory_order_relaxed_s, RELEASE_RELAXED_)
	EASTL_ATOMIC_RAW_CMPXCHG2_TMPL(internal::memory_order_acq_rel_s, internal::memory_order_relaxed_s, ACQ_REL_RELAXED_)
	EASTL_ATOMIC_RAW_CMPXCHG2_TMPL(internal::memory_order_acq_rel_s, internal::memory_order_acquire_s, ACQ_REL_ACQUIRE_)
	EASTL_ATOMIC_RAW_CMPXCHG2_TMPL(internal::memory_order_seq_cst_s, internal::memory_order_relaxed_s, SEQ_CST_RELAXED_)
	EASTL_ATOMIC_RAW_CMPXCHG2_TMPL(internal::memory_order_seq_cst_s, internal::memory_order_acquire_s, SEQ_CST_ACQUIRE_)
	EASTL_ATOMIC_RAW_CMPXCHG2_TMPL(internal::memory_order_seq_cst_s, internal::memory_order_seq_cst_s, SEQ_CST_SEQ_CST_)

	EASTL_ATOMIC_RAW_INTEGRAL_FETCH_FUNC_TMPL(fetch_add, FETCH_ADD_)
	EASTL_ATOMIC_RAW_INTEGRAL_FETCH_FUNC_TMPL(add_fetch, ADD_FETCH_)
	EASTL_ATOMIC_RAW_INTEGRAL_FETCH_FUNC_TMPL(fetch_sub, FETCH_SUB_)
	EASTL_ATOMIC_RAW_INTEGRAL_FETCH_FUNC_TMPL(sub_fetch, SUB_FETCH_)
	EASTL_ATOMIC_RAW_INTEGRAL_FETCH_FUNC_TMPL(fetch_and, FETCH_AND_)
	EASTL_ATOMIC_RAW_INTEGRAL_FETCH_FUNC_TMPL(and_fetch, AND_FETCH_)
	EASTL_ATOMIC_RAW_INTEGRAL_FETCH_FUNC_TMPL(fetch_or, FETCH_OR_)
	EASTL_ATOMIC_RAW_INTEGRAL_FETCH_FUNC_TMPL(or_fetch, OR_FETCH_)
	EASTL_ATOMIC_RAW_INTEGRAL_FETCH_FUNC_TMPL(fetch_xor, FETCH_XOR_)
	EASTL_ATOMIC_RAW_INTEGRAL_FETCH_FUNC_TMPL(xor_fetch, XOR_FETCH_)

	EASTL_ATOMIC_RAW_PTR_FETCH_FUNC_TMPL(fetch_add, FETCH_ADD_)
	EASTL_ATOMIC_RAW_PTR_FETCH_FUNC_TMPL(add_fetch, ADD_FETCH_)
	EASTL_ATOMIC_RAW_PTR_FETCH_FUNC_TMPL(fetch_sub, FETCH_SUB_)
	EASTL_ATOMIC_RAW_PTR_FETCH_FUNC_TMPL(sub_fetch, SUB_FETCH_)
} // namespace eastl
