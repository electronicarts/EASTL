/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_IN_PLACE_T_H
#define EASTL_INTERNAL_IN_PLACE_T_H

#include <cstddef> // for std::size_t

#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

namespace eastl
{
	///////////////////////////////////////////////////////////////////////////////
	/// in_place, in_place_type<T>, in_place_index<size_t>
	/// in_place_t, in_place_type_t<T>, in_place_index_t<size_t>
	///
	/// http://en.cppreference.com/w/cpp/utility/in_place
	///
	struct in_place_t
	{
		explicit in_place_t() = default;
	};

	EASTL_CPP17_INLINE_VARIABLE constexpr in_place_t in_place{};

	template<typename>
	struct in_place_type_t
	{
		explicit in_place_type_t() = default;
	};

	template<typename T>
	constexpr in_place_type_t<T> in_place_type{};

	template<size_t>
	struct in_place_index_t
	{
		explicit in_place_index_t() = default;
	};

	template<size_t Idx>
	constexpr in_place_index_t<Idx> in_place_index{};

} // namespace eastl


#endif // Header include guard






