/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// The design for call_traits here is very similar to that found in template
// metaprogramming libraries such as Boost, GCC, and Metrowerks, given that  
// these libraries have established this interface as a defacto standard for 
// solving this problem. Also, these are described in various books on the 
// topic of template metaprogramming, such as "Modern C++ Design".
//
// See http://www.boost.org/libs/utility/call_traits.htm or search for 
// call_traits in Google for a description of call_traits.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_CALL_TRAITS_H
#define EASTL_CALL_TRAITS_H


#include <EASTL/internal/config.h>
#include <EASTL/type_traits.h>     

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{


	template <typename T, bool small_>
	struct ct_imp2 { typedef const T& param_type; };

	template <typename T>
	struct ct_imp2<T, true> { typedef const T param_type; };

	template <typename T, bool isp, bool b1>
	struct ct_imp { typedef const T& param_type; };

	template <typename T, bool isp>
	struct ct_imp<T, isp, true> { typedef typename ct_imp2<T, sizeof(T) <= sizeof(void*)>::param_type param_type; };

	template <typename T, bool b1>
	struct ct_imp<T, true, b1> { typedef T const param_type; };



	template <typename T>
	struct call_traits
	{
	public:
		typedef T        value_type;
		typedef T&       reference;
		typedef const T& const_reference;
		typedef typename ct_imp<T, is_pointer<T>::value, is_arithmetic<T>::value>::param_type param_type;
	};


	template <typename T>
	struct call_traits<T&>
	{
		typedef T&       value_type;
		typedef T&       reference;
		typedef const T& const_reference;
		typedef T&       param_type;
	};


	template <typename T, size_t N>
	struct call_traits<T [N]>
	{
	private:
		typedef T array_type[N];

	public:
		typedef const T*          value_type;
		typedef array_type&       reference;
		typedef const array_type& const_reference;
		typedef const T* const    param_type;
	};


	template <typename T, size_t N>
	struct call_traits<const T [N]>
	{
	private:
		typedef const T array_type[N];

	public:
		typedef const T*          value_type;
		typedef array_type&       reference;
		typedef const array_type& const_reference;
		typedef const T* const    param_type;
	};


} // namespace eastl


#endif // Header include guard














