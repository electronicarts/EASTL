/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_CONCEPTS_H
#define EASTL_INTERNAL_CONCEPTS_H


#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
#pragma once
#endif

#include <EASTL/internal/type_detected.h>

namespace eastl
{
	namespace internal
	{
		template <typename From, typename To>
		using detect_explicitely_convertible = decltype(static_cast<To>(declval<From>()));

		namespace concepts
		{
			template <typename T>
			constexpr bool destructible = is_nothrow_destructible_v<T>;

			template <typename T, typename... Args>
			constexpr bool constructible_from = destructible<T> && is_constructible_v<T, Args...>;

			template <typename From, typename To>
			constexpr bool convertible_to =
				  is_convertible_v<From, To> && is_detected_v<detect_explicitely_convertible, From, To>;

			template <typename T>
			constexpr bool move_constructible = constructible_from<T, T> && convertible_to<T, T>;

			template <typename T>
			constexpr bool copy_constructible =
				move_constructible<T> && constructible_from<T, T&> && convertible_to<T&, T> &&
				constructible_from<T, const T&> && convertible_to<const T&, T> && constructible_from<T, const T> &&
				convertible_to<const T, T>;
		} // namespace concepts
	} // namespace internal
} // namespace eastl

#endif