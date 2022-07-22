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
#include <string.h> // memcpy

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
		EA_CONSTEXPR To bit_cast(const From& from) EA_NOEXCEPT
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
		inline To bit_cast(const From& from) EA_NOEXCEPT
		{
			typename eastl::aligned_storage<sizeof(To), alignof(To)>::type to;
			::memcpy(eastl::addressof(to), eastl::addressof(from), sizeof(To));
			return reinterpret_cast<To&>(to);
		}

	#endif // EASTL_CONSTEXPR_BIT_CAST_SUPPORTED

	#if defined(EA_COMPILER_CPP20_ENABLED)
	#ifndef EASTL_COUNT_LEADING_ZEROES
		#if   defined(__GNUC__)
			#if (EA_PLATFORM_PTR_SIZE == 8)
				#define EASTL_COUNT_LEADING_ZEROES __builtin_clzll
			#else
				#define EASTL_COUNT_LEADING_ZEROES __builtin_clz
			#endif
		#endif

		#ifndef EASTL_COUNT_LEADING_ZEROES
			static inline int eastl_count_leading_zeroes(uint64_t x)
			{
				if(x)
				{
					int n = 0;
					if(x & UINT64_C(0xFFFFFFFF00000000)) { n += 32; x >>= 32; }
					if(x & 0xFFFF0000)                   { n += 16; x >>= 16; }
					if(x & 0xFFFFFF00)                   { n +=  8; x >>=  8; }
					if(x & 0xFFFFFFF0)                   { n +=  4; x >>=  4; }
					if(x & 0xFFFFFFFC)                   { n +=  2; x >>=  2; }
					if(x & 0xFFFFFFFE)                   { n +=  1;           }
					return 63 - n;
				}
				return 64;
			}

			static inline int eastl_count_leading_zeroes(uint32_t x)
			{
				if(x)
				{
					int n = 0;
					if(x <= 0x0000FFFF) { n += 16; x <<= 16; }
					if(x <= 0x00FFFFFF) { n +=  8; x <<=  8; }
					if(x <= 0x0FFFFFFF) { n +=  4; x <<=  4; }
					if(x <= 0x3FFFFFFF) { n +=  2; x <<=  2; }
					if(x <= 0x7FFFFFFF) { n +=  1;           }
					return n;
				}
				return 32;
			}

			#define EASTL_COUNT_LEADING_ZEROES eastl_count_leading_zeroes
		#endif
	#endif

	template <typename T, typename = eastl::enable_if_t<eastl::is_unsigned_v<T>>>
	EA_CONSTEXPR int countl_zero(const T num) EA_NOEXCEPT
	{
		EA_CONSTEXPR auto DIGITS = eastl::numeric_limits<T>::digits;
		EA_CONSTEXPR auto DIGITS_U = eastl::numeric_limits<unsigned>::digits;
		EA_CONSTEXPR auto DIGITS_ULL = eastl::numeric_limits<unsigned long long>::digits;

		if (num == 0)
		{
			return DIGITS;
		}

		if constexpr (DIGITS <= DIGITS_U)
		{
			EA_CONSTEXPR auto DIFF = DIGITS_U - DIGITS;
			return EASTL_COUNT_LEADING_ZEROES(static_cast<uint32_t>(num)) - DIFF;
		}
		else
		{
			EA_CONSTEXPR auto DIFF = DIGITS_ULL - DIGITS;
			return EASTL_COUNT_LEADING_ZEROES(static_cast<uint64_t>(num)) - DIFF;
		}
	}

	template <typename T, typename = eastl::enable_if_t<eastl::is_unsigned_v<T>>>
	EA_CONSTEXPR bool has_single_bit(const T num) EA_NOEXCEPT
	{
		return num != 0 && (num & (num - 1)) == 0;
	}

	template <typename T, typename = eastl::enable_if_t<eastl::is_unsigned_v<T>>>
	EA_CONSTEXPR T bit_ceil(const T num) EA_NOEXCEPT
	{
		if (num <= 1U)
		{
			return T(1);
		}

		const auto shift = eastl::numeric_limits<T>::digits - eastl::countl_zero(static_cast<T>(num - 1));
		return static_cast<T>(T(1) << shift);
	}

	template <typename T, typename = eastl::enable_if_t<eastl::is_unsigned_v<T>>>
	EA_CONSTEXPR T bit_floor(const T num) EA_NOEXCEPT
	{
		if (num == 0)
		{
			return T(0);
		}

		const auto shift = eastl::numeric_limits<T>::digits - eastl::countl_zero(num) - 1;
		return static_cast<T>(T(1) << shift);
	}

	template <typename T, typename = eastl::enable_if_t<eastl::is_unsigned_v<T>>>
	EA_CONSTEXPR T bit_width(const T num) EA_NOEXCEPT
	{
		return static_cast<T>(eastl::numeric_limits<T>::digits - eastl::countl_zero(num));
	}
	#endif

} // namespace eastl

#endif // EASTL_BIT_H
