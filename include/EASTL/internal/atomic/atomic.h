/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_H
#define EASTL_ATOMIC_INTERNAL_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


#include <EASTL/internal/config.h>
#include <EASTL/internal/move_help.h>
#include <EASTL/internal/memory_base.h>
#include <EASTL/type_traits.h>

#include "atomic_macros.h"
#include "atomic_casts.h"

#include "atomic_memory_order.h"
#include "atomic_asserts.h"

#include "atomic_size_aligned.h"
#include "atomic_base_width.h"

#include "atomic_integral.h"

#include "atomic_pointer.h"


/////////////////////////////////////////////////////////////////////////////////


/**
 * NOTE:
 *
 * All of the actual implementation is done via the ATOMIC_MACROS in the compiler or arch sub folders.
 * The C++ code is merely boilerplate around these macros that actually implement the atomic operations.
 * The C++ boilerplate is also hidden behind macros.
 * This may seem more complicated but this is all meant to reduce copy-pasting and to ensure all operations
 * all end up going down to one macro that does the actual implementation.
 * The reduced code duplication makes it easier to verify the implementation and reason about it.
 * Ensures we do not have to re-implement the same code for compilers that do not support generic builtins such as MSVC.
 * Ensures for compilers that have separate intrinsics for different widths, that C++ boilerplate isn't copy-pasted leading to programmer errors.
 * Ensures if we ever have to implement a new platform, only the low-level leaf macros have to be implemented, everything else will be generated for you.
 */


namespace eastl
{


namespace internal
{


// 'class' : multiple assignment operators specified
EA_DISABLE_VC_WARNING(4522);

// misaligned atomic operation may incur significant performance penalty
// The above warning is emitted in earlier versions of clang incorrectly.
// All eastl::atomic<T> objects are size aligned.
// This is static and runtime asserted.
// Thus we disable this warning.
EA_DISABLE_CLANG_WARNING(-Watomic-alignment);


	template <typename T>
	struct is_atomic_lockfree_size
	{
		static EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR_OR_CONST bool value = false ||
		#if defined(EASTL_ATOMIC_HAS_8BIT)
			sizeof(T) == 1 ||
		#endif
		#if defined(EASTL_ATOMIC_HAS_16BIT)
			sizeof(T) == 2 ||
		#endif
		#if defined(EASTL_ATOMIC_HAS_32BIT)
			sizeof(T) == 4 ||
		#endif
		#if defined(EASTL_ATOMIC_HAS_64BIT)
			sizeof(T) == 8 ||
		#endif
		#if defined(EASTL_ATOMIC_HAS_128BIT)
			sizeof(T) == 16 ||
		#endif
		false;
	};


	template <typename T>
	struct is_user_type_suitable_for_primary_template
	{
		static EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR_OR_CONST bool value = eastl::internal::is_atomic_lockfree_size<T>::value;
	};


	template <typename T>
	using select_atomic_inherit_0 = typename eastl::conditional<eastl::is_same_v<bool, T> || eastl::internal::is_user_type_suitable_for_primary_template<T>::value,
																eastl::internal::atomic_base_width<T>, /* True */
																eastl::internal::atomic_invalid_type<T> /* False */
																>::type;

	template <typename T>
	using select_atomic_inherit  = select_atomic_inherit_0<T>;


} // namespace internal


#define EASTL_ATOMIC_CLASS_IMPL(type, base, valueType, differenceType)	\
	private:															\
																		\
		EASTL_ATOMIC_STATIC_ASSERT_TYPE(type);							\
																		\
		using Base = base;												\
																		\
	public:																\
																		\
		typedef valueType value_type;									\
		typedef differenceType difference_type;							\
																		\
	public:																\
																		\
		static EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR_OR_CONST bool is_always_lock_free = eastl::internal::is_atomic_lockfree_size<type>::value; \
																		\
	public: /* deleted ctors && assignment operators */					\
																		\
		atomic(const atomic&) EA_NOEXCEPT = delete;						\
																		\
		atomic& operator=(const atomic&)          EA_NOEXCEPT = delete; \
		atomic& operator=(const atomic&) volatile EA_NOEXCEPT = delete; \
																		\
	public: /* ctors */													\
																		\
		EA_CONSTEXPR atomic(type desired) EA_NOEXCEPT					\
			: Base{ desired }											\
		{																\
		}																\
																		\
		EA_CONSTEXPR atomic() EA_NOEXCEPT_IF(eastl::is_nothrow_default_constructible_v<type>) = default; \
																		\
	public:																\
																		\
		bool is_lock_free() const EA_NOEXCEPT							\
		{																\
			return eastl::internal::is_atomic_lockfree_size<type>::value; \
		}																\
																		\
		bool is_lock_free() const volatile EA_NOEXCEPT					\
		{																\
			EASTL_ATOMIC_STATIC_ASSERT_VOLATILE_MEM_FN(type);			\
			return false;												\
		}


#define EASTL_ATOMIC_USING_ATOMIC_BASE(type)		\
	public:											\
													\
		using Base::operator=;						\
		using Base::store;							\
		using Base::load;							\
		using Base::exchange;						\
		using Base::compare_exchange_weak;			\
		using Base::compare_exchange_strong;		\
													\
	public:											\
													\
		operator type() const volatile EA_NOEXCEPT	\
		{											\
			EASTL_ATOMIC_STATIC_ASSERT_VOLATILE_MEM_FN(T); \
		}											\
													\
		operator type() const EA_NOEXCEPT			\
		{											\
			return load(eastl::memory_order_seq_cst); \
		}


#define EASTL_ATOMIC_USING_ATOMIC_INTEGRAL()	\
	public:										\
												\
		using Base::fetch_add;					\
		using Base::add_fetch;					\
												\
		using Base::fetch_sub;					\
		using Base::sub_fetch;					\
												\
		using Base::fetch_and;					\
		using Base::and_fetch;					\
												\
		using Base::fetch_or;					\
		using Base::or_fetch;					\
												\
		using Base::fetch_xor;					\
		using Base::xor_fetch;					\
												\
		using Base::operator++;					\
		using Base::operator--;					\
		using Base::operator+=;					\
		using Base::operator-=;					\
		using Base::operator&=;					\
		using Base::operator|=;					\
		using Base::operator^=;


#define EASTL_ATOMIC_USING_ATOMIC_POINTER()		\
	public:										\
												\
		using Base::fetch_add;					\
		using Base::add_fetch;					\
		using Base::fetch_sub;					\
		using Base::sub_fetch;					\
												\
		using Base::operator++;					\
		using Base::operator--;					\
		using Base::operator+=;					\
		using Base::operator-=;


template <typename T, typename = void>
struct atomic : protected eastl::internal::select_atomic_inherit<T>
{
	EASTL_ATOMIC_CLASS_IMPL(T, eastl::internal::select_atomic_inherit<T>, T, T)

	EASTL_ATOMIC_USING_ATOMIC_BASE(T)
};


template <typename T>
struct atomic<T, eastl::enable_if_t<eastl::is_integral_v<T> && !eastl::is_same_v<bool, T>>> : protected eastl::internal::atomic_integral_width<T>
{
	EASTL_ATOMIC_CLASS_IMPL(T, eastl::internal::atomic_integral_width<T>, T, T)

	EASTL_ATOMIC_USING_ATOMIC_BASE(T)

	EASTL_ATOMIC_USING_ATOMIC_INTEGRAL()
};


template <typename T>
struct atomic<T*> : protected eastl::internal::atomic_pointer_width<T*>
{
	EASTL_ATOMIC_CLASS_IMPL(T*, eastl::internal::atomic_pointer_width<T*>, T*, ptrdiff_t)

	EASTL_ATOMIC_USING_ATOMIC_BASE(T*)

	EASTL_ATOMIC_USING_ATOMIC_POINTER()
};


EA_RESTORE_VC_WARNING();

EA_RESTORE_CLANG_WARNING();

} // namespace eastl

#endif /* EASTL_ATOMIC_INTERNAL_H */
