/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_BIT_H
#define EASTL_BIT_H

#include <EASTL/internal/config.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#include <EASTL/internal/memory_base.h>
#include <EASTL/type_traits.h>
#include <EASTL/numeric_limits.h>
#include <string.h> // memcpy

#if defined(EA_COMPILER_MSVC) && !defined(__clang__)
#include <intrin.h>
#endif

namespace eastl
{
	// eastl::bit_cast
	// Obtains a value of type To by reinterpreting the object representation of 'from'.
	// Every bit in the value representation of the returned To object is equal to the
	// corresponding bit in the object representation of 'from'.
	//
	// In order for bit_cast to be constexpr, the compiler needs to explicitly support
	// it by providing the __builtin_bit_cast builtin. If that builtin is not available,
	// then we memcpy into aligned storage at runtime and return that instead.
	//
	// Both types To and From must be equal in size, and must be trivially copyable.

	#if defined(EASTL_CONSTEXPR_BIT_CAST_SUPPORTED) && EASTL_CONSTEXPR_BIT_CAST_SUPPORTED

		template<typename To, typename From,
			typename = eastl::enable_if_t<
				sizeof(To) == sizeof(From)
				&& eastl::is_trivially_copyable<To>::value
				&& eastl::is_trivially_copyable<From>::value
			>
		>
		constexpr To bit_cast(const From& from) noexcept
		{
			return __builtin_bit_cast(To, from);
		}

	#else

		template<typename To, typename From,
			typename = eastl::enable_if_t<
				sizeof(To) == sizeof(From)
				&& eastl::is_trivially_copyable<To>::value
				&& eastl::is_trivially_copyable<From>::value
			>
		>
		inline To bit_cast(const From& from) noexcept
		{
			typename eastl::aligned_storage<sizeof(To), alignof(To)>::type to;
			::memcpy(eastl::addressof(to), eastl::addressof(from), sizeof(To));
			return reinterpret_cast<To&>(to);
		}

	#endif // EASTL_CONSTEXPR_BIT_CAST_SUPPORTED

namespace internal
{

constexpr int countl_zero64(uint64_t x) noexcept
{
	if (x)
	{
		int n = 0;
		if (x & UINT64_C(0xFFFFFFFF00000000))
		{
			n += 32;
			x >>= 32;
		}
		if (x & 0xFFFF0000)
		{
			n += 16;
			x >>= 16;
		}
		if (x & 0xFFFFFF00)
		{
			n += 8;
			x >>= 8;
		}
		if (x & 0xFFFFFFF0)
		{
			n += 4;
			x >>= 4;
		}
		if (x & 0xFFFFFFFC)
		{
			n += 2;
			x >>= 2;
		}
		if (x & 0xFFFFFFFE)
		{
			n += 1;
		}
		return 63 - n;
	}
	return 64;
}

// Count leading zeroes in an integer.
//
// todo: consolidate with EA::StdC::CountLeading0Bits() and bitset's GetLastBit() + 1.
// this implementation has these improvements:
//  - has overloads for all unsigned integral types. CountLeading0Bits() may not compile with ambiguous overloads because it doesn't match all unsigned integral types.
//  - supports 128 bit types.
//  - is noexcept.
//  - is constexpr, if available.
#if defined(EA_COMPILER_MSVC) && !defined(__clang__)

// MSVC overloads are not constexpr because _BitScanReverse is not constexpr.
inline int countl_zero(unsigned char x) noexcept
{
	unsigned long index;
	return _BitScanReverse(&index, static_cast<unsigned long>(x)) ? (sizeof(unsigned char) * CHAR_BIT - 1 - index) : (sizeof(unsigned char) * CHAR_BIT);
}

inline int countl_zero(unsigned short x) noexcept
{
	unsigned long index;
	return _BitScanReverse(&index, static_cast<unsigned long>(x)) ? (sizeof(unsigned short) * CHAR_BIT - 1 - index) : (sizeof(unsigned short) * CHAR_BIT);
}

inline int countl_zero(unsigned int x) noexcept
{
	unsigned long index;
	return _BitScanReverse(&index, static_cast<unsigned long>(x)) ? (sizeof(unsigned int) * CHAR_BIT - 1 - index) : (sizeof(unsigned int) * CHAR_BIT);
}

inline int countl_zero(unsigned long x) noexcept
{
	unsigned long index;
	return _BitScanReverse(&index, x) ? (sizeof(unsigned long) * CHAR_BIT - 1 - index) : (sizeof(unsigned long) * CHAR_BIT);
}

#if (EA_PLATFORM_PTR_SIZE == 8)
inline int countl_zero(unsigned long long x) noexcept
{
	unsigned long index;
	return _BitScanReverse64(&index, x) ? (sizeof(unsigned long long) * CHAR_BIT - 1 - index) : (sizeof(unsigned long long) * CHAR_BIT);
}
#else
inline int countl_zero(unsigned long long x) noexcept
{
	return countl_zero64(static_cast<uint64_t>(x));
}
#endif

#elif defined(__GNUC__) || defined(__clang__)
// __builtin_clz
constexpr inline int countl_zero(unsigned char x) noexcept
{
	constexpr auto diff = eastl::numeric_limits<unsigned int>::digits - eastl::numeric_limits<unsigned char>::digits;
	return x ? (__builtin_clz(static_cast<unsigned int>(x)) - diff) : (sizeof(unsigned char) * CHAR_BIT);
}
constexpr inline int countl_zero(unsigned short x) noexcept
{
	constexpr auto diff = eastl::numeric_limits<unsigned int>::digits - eastl::numeric_limits<unsigned short>::digits;
	return x ? (__builtin_clz(static_cast<unsigned int>(x)) - diff) : (sizeof(unsigned short) * CHAR_BIT);
}
constexpr inline int countl_zero(unsigned int x) noexcept
{
	return x ? __builtin_clz(x) : (sizeof(unsigned int) * CHAR_BIT);
}

// __builtin_clzl
constexpr inline int countl_zero(unsigned long x) noexcept
{
	return x ? __builtin_clzl(x) : (sizeof(unsigned long) * CHAR_BIT);
}

// __builtin_clzll
#if (EA_PLATFORM_PTR_SIZE == 8)
constexpr inline int countl_zero(unsigned long long x) noexcept
{
	return x ? __builtin_clzll(x) : (sizeof(unsigned long long) * CHAR_BIT);
}
#else
constexpr inline int countl_zero(unsigned long long x) noexcept
{
	return countl_zero64(static_cast<uint64_t>(x));
}
#endif

#if EASTL_INT128_SUPPORTED
// todo: once we are using Clang 19.1.0 and GCC ??? use __builtin_clzg(x)
constexpr inline int countl_zero(eastl_uint128_t x) noexcept
{
	const int first64bits = countl_zero(static_cast<uint64_t>(x >> 64));
	return first64bits == 64 ? (64 + countl_zero(static_cast<uint64_t>(x))) : first64bits;
}
#endif

#else // not MSVC, clang or GCC

template <typename T, eastl::enable_if_t<eastl::is_unsigned_v<T> && sizeof(T) < 4, bool > = true>
constexpr int countl_zero(const T num) noexcept
{
	constexpr auto diff = 32 - eastl::numeric_limits<T>::digits;
	return countl_zero(static_cast<uint32_t>(num)) - diff;
}

template <typename T, eastl::enable_if_t<eastl::is_unsigned_v<T> && sizeof(T) == 4, bool> = true>
constexpr int countl_zero(T x) noexcept
{
	if (x)
	{
		int n = 0;
		if (x <= 0x0000FFFF)
		{
			n += 16;
			x <<= 16;
		}
		if (x <= 0x00FFFFFF)
		{
			n += 8;
			x <<= 8;
		}
		if (x <= 0x0FFFFFFF)
		{
			n += 4;
			x <<= 4;
		}
		if (x <= 0x3FFFFFFF)
		{
			n += 2;
			x <<= 2;
		}
		if (x <= 0x7FFFFFFF)
		{
			n += 1;
		}
		return n;
	}
	return 32;
}

template <typename T, eastl::enable_if_t<eastl::is_unsigned_v<T> && sizeof(T) == 8, bool> = true>
constexpr int countl_zero(T x) noexcept
{
	return countl_zero64(static_cast<uint64_t>(x));
}

#if EASTL_INT128_SUPPORTED
constexpr inline int countl_zero(eastl_uint128_t x) noexcept
{
	if (x)
	{
		int n = 0;
		if (x & (~eastl_uint128_t(0) << 64))
		{
			n += 64;
			x >>= 64;
		}
		if (x & UINT64_C(0xFFFFFFFF00000000))
		{
			n += 32;
			x >>= 32;
		}
		if (x & 0xFFFF0000)
		{
			n += 16;
			x >>= 16;
		}
		if (x & 0xFFFFFF00)
		{
			n += 8;
			x >>= 8;
		}
		if (x & 0xFFFFFFF0)
		{
			n += 4;
			x >>= 4;
		}
		if (x & 0xFFFFFFFC)
		{
			n += 2;
			x >>= 2;
		}
		if (x & 0xFFFFFFFE)
		{
			n += 1;
		}
		return 127 - n;
	}
	return 128;
}
#endif

#endif

}  // namespace internal

	template <typename T, typename = eastl::enable_if_t<eastl::is_unsigned_v<T>>>
	constexpr int countl_zero(T x) noexcept { return internal::countl_zero(x); }

	template <typename T, typename = eastl::enable_if_t<eastl::is_unsigned_v<T>>>
	constexpr bool has_single_bit(const T num) noexcept
	{
		return num != 0 && (num & (num - 1)) == 0;
	}

	template <typename T, typename = eastl::enable_if_t<eastl::is_unsigned_v<T>>>
	constexpr T bit_ceil(const T num) noexcept
	{
		if (num <= 1U)
		{
			return T(1);
		}

		const auto shift = eastl::numeric_limits<T>::digits - eastl::countl_zero(static_cast<T>(num - 1));
		return static_cast<T>(T(1) << shift);
	}

	template <typename T, typename = eastl::enable_if_t<eastl::is_unsigned_v<T>>>
	constexpr T bit_floor(const T num) noexcept
	{
		if (num == 0)
		{
			return T(0);
		}

		const auto shift = eastl::numeric_limits<T>::digits - eastl::countl_zero(num) - 1;
		return static_cast<T>(T(1) << shift);
	}

	template <typename T, typename = eastl::enable_if_t<eastl::is_unsigned_v<T>>>
	constexpr int bit_width(const T num) noexcept
	{
		return eastl::numeric_limits<T>::digits - eastl::countl_zero(num);
	}

namespace internal
{

const static char kBitsPerUint16[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

template <typename T>
constexpr int popcount_non_intrinsic(T num) noexcept
{
	int n = 0;
	for (T w = num; w; w >>= 4)
		n += kBitsPerUint16[w & 0xF];
	return n;
}

#if defined(EA_COMPILER_MSVC) && !defined(__clang__)

// __popcnt is not constexpr

// __popcnt16
inline int popcount(unsigned char num) noexcept { return __popcnt16(static_cast<unsigned short>(num)); }
inline int popcount(unsigned short num) noexcept { return __popcnt16(num); }

// __popcnt
inline int popcount(unsigned int num) noexcept { return __popcnt(num); }

#if defined(EA_PROCESSOR_X86_64)
// __popcnt64
inline int popcount(unsigned long num) noexcept { return static_cast<int>(__popcnt64(num)); }
inline int popcount(unsigned long long num) noexcept { return static_cast<int>(__popcnt64(num)); }
#else
// todo: is it better to use __popcnt() or the fallback implementation?
inline int popcount(unsigned long num) noexcept { return popcount_non_intrinsic(num); }
inline int popcount(unsigned long long num) noexcept { return popcount_non_intrinsic(num); }
#endif
#elif defined(__GNUC__) || defined(__clang__)
// __builtin_popcount
constexpr inline int popcount(unsigned char num) noexcept { return __builtin_popcount(num); }
constexpr inline int popcount(unsigned short num) noexcept { return __builtin_popcount(num); }
constexpr inline int popcount(unsigned int num) noexcept { return __builtin_popcount(num); }

// __builtin_popcountl
constexpr inline int popcount(unsigned long num) noexcept { return __builtin_popcountl(num); }

// __builtin_popcountll
constexpr inline int popcount(unsigned long long num) noexcept { return __builtin_popcountll(num); }
#endif

#if EASTL_INT128_SUPPORTED
// todo: once we are using Clang 19.1.0 and GCC ??? use __builtin_popcountg(num)
constexpr inline int popcount(eastl_uint128_t num) noexcept
{
	return popcount(static_cast<uint64_t>(num >> 64)) + popcount(static_cast<uint64_t>(num));
}
#endif

} // namespace internal

	template <typename T, typename = eastl::enable_if_t<eastl::is_unsigned_v<T>>>
	constexpr int popcount(T x) noexcept { return internal::popcount(x); }

} // namespace eastl

#endif // EASTL_BIT_H
