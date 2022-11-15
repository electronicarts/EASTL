/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_BASE_WIDTH_H
#define EASTL_ATOMIC_INTERNAL_BASE_WIDTH_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


#include "atomic_push_compiler_options.h"


namespace eastl
{


namespace internal
{


	template <typename T, unsigned width = sizeof(T)>
	struct atomic_base_width;

	/**
	 * NOTE:
	 *
	 * T does not have to be trivially default constructible but it still
	 * has to be a trivially copyable type for the primary atomic template.
	 * Thus we must type pun into whatever storage type of the given fixed width
	 * the platform designates. This ensures T does not have to be trivially constructible.
	 */

#define EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits)				\
	EA_PREPROCESSOR_JOIN(EASTL_ATOMIC_FIXED_WIDTH_TYPE_, bits)


#define EASTL_ATOMIC_STORE_FUNC_IMPL(op, bits)							\
	EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits) fixedWidthDesired = EASTL_ATOMIC_TYPE_PUN_CAST(EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits), desired); \
	EA_PREPROCESSOR_JOIN(op, bits)(EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits), \
								   EASTL_ATOMIC_TYPE_CAST(EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits), this->GetAtomicAddress()), \
								   fixedWidthDesired)


#define EASTL_ATOMIC_LOAD_FUNC_IMPL(op, bits)							\
	EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits) retVal;					\
	EA_PREPROCESSOR_JOIN(op, bits)(EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits), \
								   retVal,								\
								   EASTL_ATOMIC_TYPE_CAST(EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits), this->GetAtomicAddress())); \
	return EASTL_ATOMIC_TYPE_PUN_CAST(T, retVal);


#define EASTL_ATOMIC_EXCHANGE_FUNC_IMPL(op, bits)						\
	EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits) retVal;					\
	EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits) fixedWidthDesired = EASTL_ATOMIC_TYPE_PUN_CAST(EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits), desired); \
	EA_PREPROCESSOR_JOIN(op, bits)(EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits), \
								   retVal,								\
								   EASTL_ATOMIC_TYPE_CAST(EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits), this->GetAtomicAddress()), \
								   fixedWidthDesired);					\
	return EASTL_ATOMIC_TYPE_PUN_CAST(T, retVal);


#define EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(op, bits)						\
	EASTL_ATOMIC_DEFAULT_INIT(bool, retVal);					        \
	EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits) fixedWidthDesired = EASTL_ATOMIC_TYPE_PUN_CAST(EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits), desired); \
	EA_PREPROCESSOR_JOIN(op, bits)(EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits), \
								   retVal,								\
								   EASTL_ATOMIC_TYPE_CAST(EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits), this->GetAtomicAddress()), \
								   EASTL_ATOMIC_TYPE_CAST(EASTL_ATOMIC_BASE_FIXED_WIDTH_TYPE(bits), &expected), \
								   fixedWidthDesired);					\
	return retVal;


#define EASTL_ATOMIC_BASE_OP_JOIN(op, Order)						\
	EA_PREPROCESSOR_JOIN(EA_PREPROCESSOR_JOIN(EASTL_ATOMIC_, op), Order)


#define EASTL_ATOMIC_BASE_CMPXCHG_FUNCS_IMPL(funcName, cmpxchgOp, bits)	\
	using Base::funcName;												\
																		\
	bool funcName(T& expected, T desired) EA_NOEXCEPT					\
	{																	\
		EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(EASTL_ATOMIC_BASE_OP_JOIN(cmpxchgOp, _SEQ_CST_), bits); \
	}																	\
																		\
	bool funcName(T& expected, T desired,								\
				  eastl::internal::memory_order_relaxed_s) EA_NOEXCEPT	\
	{																	\
		EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(EASTL_ATOMIC_BASE_OP_JOIN(cmpxchgOp, _RELAXED_), bits); \
	}																	\
																		\
	bool funcName(T& expected, T desired,								\
				  eastl::internal::memory_order_acquire_s) EA_NOEXCEPT	\
	{																	\
		EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(EASTL_ATOMIC_BASE_OP_JOIN(cmpxchgOp, _ACQUIRE_), bits); \
	}																	\
																		\
	bool funcName(T& expected, T desired,								\
				  eastl::internal::memory_order_release_s) EA_NOEXCEPT	\
	{																	\
		EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(EASTL_ATOMIC_BASE_OP_JOIN(cmpxchgOp, _RELEASE_), bits); \
	}																	\
																		\
	bool funcName(T& expected, T desired,								\
				  eastl::internal::memory_order_acq_rel_s) EA_NOEXCEPT	\
	{																	\
		EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(EASTL_ATOMIC_BASE_OP_JOIN(cmpxchgOp, _ACQ_REL_), bits); \
	}																	\
																		\
	bool funcName(T& expected, T desired,								\
				  eastl::internal::memory_order_seq_cst_s) EA_NOEXCEPT	\
	{																	\
		EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(EASTL_ATOMIC_BASE_OP_JOIN(cmpxchgOp, _SEQ_CST_), bits); \
	}																	\
																		\
	bool funcName(T& expected, T desired,								\
				  eastl::internal::memory_order_relaxed_s,				\
				  eastl::internal::memory_order_relaxed_s) EA_NOEXCEPT	\
	{																	\
		EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(EASTL_ATOMIC_BASE_OP_JOIN(cmpxchgOp, _RELAXED_RELAXED_), bits); \
	}																	\
																		\
	bool funcName(T& expected, T desired,								\
				  eastl::internal::memory_order_acquire_s,				\
				  eastl::internal::memory_order_relaxed_s) EA_NOEXCEPT	\
	{																	\
		EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(EASTL_ATOMIC_BASE_OP_JOIN(cmpxchgOp, _ACQUIRE_RELAXED_), bits); \
	}																	\
																		\
	bool funcName(T& expected, T desired,								\
				  eastl::internal::memory_order_acquire_s,				\
				  eastl::internal::memory_order_acquire_s) EA_NOEXCEPT	\
	{																	\
		EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(EASTL_ATOMIC_BASE_OP_JOIN(cmpxchgOp, _ACQUIRE_ACQUIRE_), bits); \
	}																	\
																		\
	bool funcName(T& expected, T desired,								\
				  eastl::internal::memory_order_release_s,				\
				  eastl::internal::memory_order_relaxed_s) EA_NOEXCEPT	\
	{																	\
		EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(EASTL_ATOMIC_BASE_OP_JOIN(cmpxchgOp, _RELEASE_RELAXED_), bits); \
	}																	\
																		\
	bool funcName(T& expected, T desired,								\
				  eastl::internal::memory_order_acq_rel_s,				\
				  eastl::internal::memory_order_relaxed_s) EA_NOEXCEPT	\
	{																	\
		EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(EASTL_ATOMIC_BASE_OP_JOIN(cmpxchgOp, _ACQ_REL_RELAXED_), bits); \
	}																	\
																		\
	bool funcName(T& expected, T desired,								\
				  eastl::internal::memory_order_acq_rel_s,				\
				  eastl::internal::memory_order_acquire_s) EA_NOEXCEPT	\
	{																	\
		EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(EASTL_ATOMIC_BASE_OP_JOIN(cmpxchgOp, _ACQ_REL_ACQUIRE_), bits); \
	}																	\
																		\
	bool funcName(T& expected, T desired,								\
				  eastl::internal::memory_order_seq_cst_s,				\
				  eastl::internal::memory_order_relaxed_s) EA_NOEXCEPT	\
	{																	\
		EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(EASTL_ATOMIC_BASE_OP_JOIN(cmpxchgOp, _SEQ_CST_RELAXED_), bits); \
	}																	\
																		\
	bool funcName(T& expected, T desired,								\
				  eastl::internal::memory_order_seq_cst_s,				\
				  eastl::internal::memory_order_acquire_s) EA_NOEXCEPT	\
	{																	\
		EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(EASTL_ATOMIC_BASE_OP_JOIN(cmpxchgOp, _SEQ_CST_ACQUIRE_), bits); \
	}																	\
																		\
	bool funcName(T& expected, T desired,								\
				  eastl::internal::memory_order_seq_cst_s,				\
				  eastl::internal::memory_order_seq_cst_s) EA_NOEXCEPT	\
	{																	\
		EASTL_ATOMIC_CMPXCHG_FUNC_IMPL(EASTL_ATOMIC_BASE_OP_JOIN(cmpxchgOp, _SEQ_CST_SEQ_CST_), bits); \
	}

#define EASTL_ATOMIC_BASE_CMPXCHG_WEAK_FUNCS_IMPL(bits)					\
	EASTL_ATOMIC_BASE_CMPXCHG_FUNCS_IMPL(compare_exchange_weak, CMPXCHG_WEAK, bits)

#define EASTL_ATOMIC_BASE_CMPXCHG_STRONG_FUNCS_IMPL(bits)				\
	EASTL_ATOMIC_BASE_CMPXCHG_FUNCS_IMPL(compare_exchange_strong, CMPXCHG_STRONG, bits)


#define EASTL_ATOMIC_BASE_WIDTH_SPECIALIZE(bytes, bits)					\
	template <typename T>												\
	struct atomic_base_width<T, bytes> : public atomic_size_aligned<T>	\
	{																	\
	private:															\
																		\
		static_assert(EA_ALIGN_OF(atomic_size_aligned<T>) == bytes, "eastl::atomic<T> must be sizeof(T) aligned!"); \
		static_assert(EA_ALIGN_OF(atomic_size_aligned<T>) == sizeof(T), "eastl::atomic<T> must be sizeof(T) aligned!"); \
		using Base = atomic_size_aligned<T>;							\
																		\
	public: /* ctors */													\
																		\
		EA_CONSTEXPR atomic_base_width(T desired) EA_NOEXCEPT			\
			: Base{ desired }											\
		{																\
		}																\
																		\
		EA_CONSTEXPR atomic_base_width() EA_NOEXCEPT_IF(eastl::is_nothrow_default_constructible_v<T>) = default; \
																		\
		atomic_base_width(const atomic_base_width&) EA_NOEXCEPT = delete; \
																		\
	public: /* store */													\
																		\
		using Base::store;												\
																		\
		void store(T desired) EA_NOEXCEPT								\
		{																\
			EASTL_ATOMIC_STORE_FUNC_IMPL(EASTL_ATOMIC_STORE_SEQ_CST_, bits); \
		}																\
																		\
		void store(T desired, eastl::internal::memory_order_relaxed_s) EA_NOEXCEPT	\
		{																\
			EASTL_ATOMIC_STORE_FUNC_IMPL(EASTL_ATOMIC_STORE_RELAXED_, bits); \
		}																\
																		\
		void store(T desired, eastl::internal::memory_order_release_s) EA_NOEXCEPT	\
		{																\
			EASTL_ATOMIC_STORE_FUNC_IMPL(EASTL_ATOMIC_STORE_RELEASE_, bits); \
		}																\
																		\
		void store(T desired, eastl::internal::memory_order_seq_cst_s) EA_NOEXCEPT	\
		{																\
			EASTL_ATOMIC_STORE_FUNC_IMPL(EASTL_ATOMIC_STORE_SEQ_CST_, bits); \
		}																\
																		\
	public: /* load */													\
																		\
		using Base::load;												\
																		\
		T load() const EA_NOEXCEPT										\
		{																\
			EASTL_ATOMIC_LOAD_FUNC_IMPL(EASTL_ATOMIC_LOAD_SEQ_CST_, bits); \
		}																\
																		\
		T load(eastl::internal::memory_order_relaxed_s) const EA_NOEXCEPT \
		{																\
			EASTL_ATOMIC_LOAD_FUNC_IMPL(EASTL_ATOMIC_LOAD_RELAXED_, bits); \
		}																\
																		\
		T load(eastl::internal::memory_order_acquire_s) const EA_NOEXCEPT \
		{																\
			EASTL_ATOMIC_LOAD_FUNC_IMPL(EASTL_ATOMIC_LOAD_ACQUIRE_, bits); \
		}																\
																		\
		T load(eastl::internal::memory_order_seq_cst_s) const EA_NOEXCEPT \
		{																\
			EASTL_ATOMIC_LOAD_FUNC_IMPL(EASTL_ATOMIC_LOAD_SEQ_CST_, bits); \
		}																\
																		\
	public: /* exchange */												\
																		\
		using Base::exchange;											\
																		\
		T exchange(T desired) EA_NOEXCEPT								\
		{																\
			EASTL_ATOMIC_EXCHANGE_FUNC_IMPL(EASTL_ATOMIC_EXCHANGE_SEQ_CST_, bits); \
		}																\
																		\
		T exchange(T desired, eastl::internal::memory_order_relaxed_s) EA_NOEXCEPT \
		{																\
			EASTL_ATOMIC_EXCHANGE_FUNC_IMPL(EASTL_ATOMIC_EXCHANGE_RELAXED_, bits); \
		}																\
																		\
		T exchange(T desired, eastl::internal::memory_order_acquire_s) EA_NOEXCEPT	\
		{																\
			EASTL_ATOMIC_EXCHANGE_FUNC_IMPL(EASTL_ATOMIC_EXCHANGE_ACQUIRE_, bits); \
		}																\
																		\
		T exchange(T desired, eastl::internal::memory_order_release_s) EA_NOEXCEPT	\
		{																\
			EASTL_ATOMIC_EXCHANGE_FUNC_IMPL(EASTL_ATOMIC_EXCHANGE_RELEASE_, bits); \
		}																\
																		\
		T exchange(T desired, eastl::internal::memory_order_acq_rel_s) EA_NOEXCEPT	\
		{																\
			EASTL_ATOMIC_EXCHANGE_FUNC_IMPL(EASTL_ATOMIC_EXCHANGE_ACQ_REL_, bits); \
		}																\
																		\
		T exchange(T desired, eastl::internal::memory_order_seq_cst_s) EA_NOEXCEPT	\
		{																\
			EASTL_ATOMIC_EXCHANGE_FUNC_IMPL(EASTL_ATOMIC_EXCHANGE_SEQ_CST_, bits); \
		}																\
																		\
	public: /* compare_exchange_weak */									\
																		\
		EASTL_ATOMIC_BASE_CMPXCHG_WEAK_FUNCS_IMPL(bits)					\
																		\
	public: /* compare_exchange_strong */								\
																		\
		EASTL_ATOMIC_BASE_CMPXCHG_STRONG_FUNCS_IMPL(bits)				\
																		\
	public: /* assignment operator */									\
																		\
		using Base::operator=;											\
																		\
		T operator=(T desired) EA_NOEXCEPT								\
		{																\
			store(desired, eastl::memory_order_seq_cst);				\
			return desired;												\
		}																\
																		\
		atomic_base_width& operator=(const atomic_base_width&)          EA_NOEXCEPT = delete; \
		atomic_base_width& operator=(const atomic_base_width&) volatile EA_NOEXCEPT = delete; \
																		\
	};


#if defined(EASTL_ATOMIC_HAS_8BIT)
	EASTL_ATOMIC_BASE_WIDTH_SPECIALIZE(1, 8)
#endif

#if defined(EASTL_ATOMIC_HAS_16BIT)
	EASTL_ATOMIC_BASE_WIDTH_SPECIALIZE(2, 16)
#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)
	EASTL_ATOMIC_BASE_WIDTH_SPECIALIZE(4, 32)
#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)
	EASTL_ATOMIC_BASE_WIDTH_SPECIALIZE(8, 64)
#endif

#if defined(EASTL_ATOMIC_HAS_128BIT)
	EASTL_ATOMIC_BASE_WIDTH_SPECIALIZE(16, 128)
#endif


} // namespace internal


} // namespace eastl


#include "atomic_pop_compiler_options.h"


#endif /* EASTL_ATOMIC_INTERNAL_BASE_WIDTH_H */
