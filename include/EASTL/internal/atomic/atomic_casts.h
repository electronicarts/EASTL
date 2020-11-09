/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_INTERNAL_CASTS_H
#define EASTL_ATOMIC_INTERNAL_CASTS_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


#include <EASTL/internal/type_transformations.h>


#include <string.h>


namespace eastl
{


namespace internal
{


template <typename T>
EASTL_FORCE_INLINE volatile T* AtomicVolatileCast(T* ptr) EA_NOEXCEPT
{
	static_assert(!eastl::is_volatile<volatile T*>::value, "eastl::atomic<T> : pointer must not be volatile, the pointed to type must be volatile!");
	static_assert(eastl::is_volatile<volatile T>::value, "eastl::atomic<T> : the pointed to type must be volatile!");

	return reinterpret_cast<volatile T*>(ptr);
}


/**
 * NOTE:
 *
 * Some compiler intrinsics do not operate on pointer types thus
 * doing atomic operations on pointers must be casted to the suitable
 * sized unsigned integral type.
 *
 * Some compiler intrinsics aren't generics and thus structs must also
 * be casted to the appropriate sized unsigned integral type.
 *
 * Atomic operations on an int* might have to be casted to a uint64_t on
 * a platform with 8-byte pointers as an example.
 *
 * Also doing an atomic operation on a struct, we must ensure that we observe
 * the whole struct as one atomic unit with no shearing between the members.
 * A load of a struct with two uint32_t members must be one uint64_t load,
 * not two separate uint32_t loads, thus casted to the suitable sized
 * unsigned integral type.
 */
template <typename Integral, typename T>
EASTL_FORCE_INLINE volatile Integral* AtomicVolatileIntegralCast(T* ptr) EA_NOEXCEPT
{
	static_assert(!eastl::is_volatile<volatile Integral*>::value, "eastl::atomic<T> : pointer must not be volatile, the pointed to type must be volatile!");
	static_assert(eastl::is_volatile<volatile Integral>::value, "eastl::atomic<T> : the pointed to type must be volatile!");
	static_assert(eastl::is_integral<Integral>::value, "eastl::atomic<T> : Integral cast must cast to an Integral type!");
	static_assert(sizeof(Integral) == sizeof(T), "eastl::atomic<T> : Integral and T must be same size for casting!");

	return reinterpret_cast<volatile Integral*>(ptr);
}

template <typename Integral, typename T>
EASTL_FORCE_INLINE Integral* AtomicIntegralCast(T* ptr) EA_NOEXCEPT
{
	static_assert(eastl::is_integral<Integral>::value, "eastl::atomic<T> : Integral cast must cast to an Integral type!");
	static_assert(sizeof(Integral) == sizeof(T), "eastl::atomic<T> : Integral and T must be same size for casting!");

	return reinterpret_cast<Integral*>(ptr);
}


/**
 * NOTE:
 *
 * These casts are meant to be used with unions or structs of larger types that must be casted
 * down to the smaller integral types. Like with 128-bit atomics and msvc intrinsics.
 *
 * struct Foo128 { __int64 array[2]; }; can be casted to a __int64*
 * since a poiter to Foo128 is a pointer to the first member.
 */
template <typename ToType, typename FromType>
EASTL_FORCE_INLINE volatile ToType* AtomicVolatileTypeCast(FromType* ptr) EA_NOEXCEPT
{
	static_assert(!eastl::is_volatile<volatile ToType*>::value, "eastl::atomic<T> : pointer must not be volatile, the pointed to type must be volatile!");
	static_assert(eastl::is_volatile<volatile ToType>::value, "eastl::atomic<T> : the pointed to type must be volatile!");

	return reinterpret_cast<volatile ToType*>(ptr);
}

template <typename ToType, typename FromType>
EASTL_FORCE_INLINE ToType* AtomicTypeCast(FromType* ptr) EA_NOEXCEPT
{
	return reinterpret_cast<ToType*>(ptr);
}


/**
 * NOTE:
 *
 * This is a compiler guaranteed safe type punning.
 * This is useful when dealing with user defined structs.
 * struct Test { uint32_t; unint32_t; };
 *
 * Example:
 * uint64_t atomicLoad = *((volatile uint64_t*)&Test);
 * Test load = AtomicTypePunCast<Test, uint64_t>(atomicLoad);
 *
 * uint64_t comparand = AtomicTypePunCast<uint64_t, Test>(Test);
 * cmpxchg(&Test, comparand, desired);
 *
 * This can be implemented in many different ways depending on the compiler such
 * as thru a union, memcpy, reinterpret_cast<Test&>(atomicLoad), etc.
 */
template <typename Pun, typename T, eastl::enable_if_t<!eastl::is_same_v<Pun, T>, int> = 0>
EASTL_FORCE_INLINE Pun AtomicTypePunCast(const T& fromType) EA_NOEXCEPT
{
	static_assert(sizeof(Pun) == sizeof(T), "eastl::atomic<T> : Pun and T must be the same size for type punning!");

	/**
	 * aligned_storage ensures we can TypePun objects that aren't trivially default constructible
	 * but still trivially copyable.
	 */
	typename eastl::aligned_storage<sizeof(Pun), alignof(Pun)>::type ret;
	memcpy(eastl::addressof(ret), eastl::addressof(fromType), sizeof(Pun));
	return reinterpret_cast<Pun&>(ret);
}

template <typename Pun, typename T, eastl::enable_if_t<eastl::is_same_v<Pun, T>, int> = 0>
EASTL_FORCE_INLINE Pun AtomicTypePunCast(const T& fromType) EA_NOEXCEPT
{
	return fromType;
}


template <typename T>
EASTL_FORCE_INLINE T AtomicNegateOperand(T val) EA_NOEXCEPT
{
	static_assert(eastl::is_integral<T>::value, "eastl::atomic<T> : Integral Negation must be an Integral type!");
	static_assert(!eastl::is_volatile<T>::value, "eastl::atomic<T> : T must not be volatile!");

	return static_cast<T>(0U - static_cast<eastl::make_unsigned_t<T>>(val));
}

EASTL_FORCE_INLINE ptrdiff_t AtomicNegateOperand(ptrdiff_t val) EA_NOEXCEPT
{
	return -val;
}


} // namespace internal


} // namespace eastl


/**
 *  NOTE:
 *
 *  These macros are meant to prevent inclusion hell.
 *  Also so that it fits with the style of the rest of the atomic macro implementation.
 */
#define EASTL_ATOMIC_VOLATILE_CAST(ptr)			\
	eastl::internal::AtomicVolatileCast((ptr))

#define EASTL_ATOMIC_VOLATILE_INTEGRAL_CAST(IntegralType, ptr)		\
	eastl::internal::AtomicVolatileIntegralCast<IntegralType>((ptr))

#define EASTL_ATOMIC_INTEGRAL_CAST(IntegralType, ptr)		\
	eastl::internal::AtomicIntegralCast<IntegralType>((ptr))

#define EASTL_ATOMIC_VOLATILE_TYPE_CAST(ToType, ptr)		\
	eastl::internal::AtomicVolatileTypeCast<ToType>((ptr))

#define EASTL_ATOMIC_TYPE_CAST(ToType, ptr)			\
	eastl::internal::AtomicTypeCast<ToType>((ptr))

#define EASTL_ATOMIC_TYPE_PUN_CAST(PunType, fromType)		\
	eastl::internal::AtomicTypePunCast<PunType>((fromType))

#define EASTL_ATOMIC_NEGATE_OPERAND(val)		\
	eastl::internal::AtomicNegateOperand((val))


#endif /* EASTL_ATOMIC_INTERNAL_CASTS_H */
