/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_TYPE_VOID_T_H
#define EASTL_INTERNAL_TYPE_VOID_T_H


#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

namespace eastl
{

	///////////////////////////////////////////////////////////////////////
	// void_t 
	//
	// Maps a sequence of any types to void.  This utility class is used in
	// template meta programming to simplify compile time reflection mechanisms
	// required by the standard library.
	//
	// http://en.cppreference.com/w/cpp/types/void_t
	//
	// Example:
	//    template <typename T, typename = void>
	//    struct is_iterable : false_type {};
	//
	//    template <typename T>
	//    struct is_iterable<T, void_t<decltype(declval<T>().begin()), 
	//                                 decltype(declval<T>().end())>> : true_type {};
	//
	///////////////////////////////////////////////////////////////////////
	template <class...>
	using void_t = void;


} // namespace eastl


#endif // Header include guard
