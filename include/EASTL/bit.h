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

} // namespace eastl

#endif // EASTL_BIT_H
