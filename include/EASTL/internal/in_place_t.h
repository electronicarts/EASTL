/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_IN_PLACE_T_H
#define EASTL_INTERNAL_IN_PLACE_T_H


#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

namespace eastl
{
	namespace Internal
	{
		struct in_place_tag {};
		template <class> struct in_place_type_tag {};
		template <size_t> struct in_place_index_tag {};
	}

	///////////////////////////////////////////////////////////////////////////////
	/// in_place_tag
	///
	/// http://en.cppreference.com/w/cpp/utility/in_place_tag
	///
	struct in_place_tag
	{
		in_place_tag() = delete;

	private:
		explicit in_place_tag(Internal::in_place_tag) {}
		friend inline in_place_tag Internal_ConstructInPlaceTag();
	};

	// internal factory function for in_place_tag
	inline in_place_tag Internal_ConstructInPlaceTag() { return in_place_tag(Internal::in_place_tag{}); }


	///////////////////////////////////////////////////////////////////////////////
	/// in_place_t / in_place_type_t / in_place_index_t
	///
	/// used to disambiguate overloads that take arguments (possibly a parameter
	/// pack) for in-place construction of some value.
	/// 
	/// http://en.cppreference.com/w/cpp/utility/optional/in_place_t
	///
	using in_place_t = in_place_tag(&)(Internal::in_place_tag);

	template <class T>
	using in_place_type_t = in_place_tag(&)(Internal::in_place_type_tag<T>);

	template <size_t N>
	using in_place_index_t = in_place_tag(&)(Internal::in_place_index_tag<N>);


	///////////////////////////////////////////////////////////////////////////////
	/// in_place / in_place<T> / in_place<size_t>
	/// 
	/// http://en.cppreference.com/w/cpp/utility/in_place
	///
	inline in_place_tag in_place(Internal::in_place_tag) { return Internal_ConstructInPlaceTag(); }

	template <class T>
	inline in_place_tag in_place(Internal::in_place_type_tag<T>) { return Internal_ConstructInPlaceTag(); }

	template <std::size_t I>
	inline in_place_tag in_place(Internal::in_place_index_tag<I>) { return Internal_ConstructInPlaceTag(); }


} // namespace eastl


#endif // Header include guard






