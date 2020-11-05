/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_STATIC_ASSERTS_H
#define EASTL_ATOMIC_INTERNAL_STATIC_ASSERTS_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


#define EASTL_ATOMIC_STATIC_ASSERT_VOLATILE_MEM_FN(type)				\
	static_assert(!eastl::is_same<type, type>::value, "eastl::atomic<T> : volatile eastl::atomic<T> is not what you expect! Read the docs in EASTL/atomic.h! Use the memory orders to access the atomic object!");

#define EASTL_ATOMIC_STATIC_ASSERT_INVALID_MEMORY_ORDER(type)			\
	static_assert(!eastl::is_same<type, type>::value, "eastl::atomic<T> : invalid memory order for the given operation!");

#define EASTL_ATOMIC_STATIC_ASSERT_TYPE(type)							\
	/* User Provided T must not be cv qualified */						\
	static_assert(!eastl::is_const<type>::value, "eastl::atomic<T> : Template Typename T cannot be const!"); \
	static_assert(!eastl::is_volatile<type>::value, "eastl::atomic<T> : Template Typename T cannot be volatile! Use the memory orders to access the underlying type for the guarantees you need."); \
	/* T must satisfy StandardLayoutType */								\
	static_assert(eastl::is_standard_layout<type>::value, "eastl::atomic<T> : Must have standard layout!"); \
	/* T must be TriviallyCopyable but it does not have to be TriviallyConstructible */ \
	static_assert(eastl::is_trivially_copyable<type>::value, "eastl::atomci<T> : Template Typename T must be trivially copyable!"); \
	static_assert(eastl::is_copy_constructible<type>::value, "eastl::atomic<T> : Template Typename T must be copy constructible!"); \
	static_assert(eastl::is_move_constructible<type>::value, "eastl::atomic<T> : Template Typename T must be move constructible!"); \
	static_assert(eastl::is_copy_assignable<type>::value, "eastl::atomic<T> : Template Typename T must be copy assignable!"); \
	static_assert(eastl::is_move_assignable<type>::value, "eastl::atomic<T> : Template Typename T must be move assignable!"); \
	static_assert(eastl::is_trivially_destructible<type>::value, "eastl::atomic<T> : Must be trivially destructible!"); \
	static_assert(eastl::internal::is_atomic_lockfree_size<type>::value, "eastl::atomic<T> : Template Typename T must be a lockfree size!");

#define EASTL_ATOMIC_STATIC_ASSERT_TYPE_IS_OBJECT(type) \
	static_assert(eastl::is_object<type>::value, "eastl::atomic<T> : Template Typename T must be an object type!");

#define EASTL_ATOMIC_ASSERT_ALIGNED(alignment)							\
	EASTL_ASSERT((alignment & (alignment - 1)) == 0);					\
	EASTL_ASSERT((reinterpret_cast<uintptr_t>(this) & (alignment - 1)) == 0)


namespace eastl
{


namespace internal
{


	template <typename T>
	struct atomic_invalid_type
	{
		/**
		 * class Test { int i; int j; int k; }; sizeof(Test) == 96 bits
		 *
		 * std::atomic allows non-primitive types to be used for the template type.
		 * This causes the api to degrade to locking for types that cannot fit into the lockfree size
		 * of the target platform such as std::atomic<Test> leading to performance traps.
		 *
		 * If this static_assert() fired, this means your template type T is larger than any atomic instruction
		 * supported on the given platform.
		 */

		static_assert(!eastl::is_same<T, T>::value, "eastl::atomic<T> : invalid template type T!");
	};


} // namespace internal


} // namespace eastl


#endif /* EASTL_ATOMIC_INTERNAL_STATIC_ASSERTS_H */
