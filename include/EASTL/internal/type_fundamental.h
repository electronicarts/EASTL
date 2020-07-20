/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_TYPE_FUNDAMENTAL_H
#define EASTL_INTERNAL_TYPE_FUNDAMENTAL_H


#include <EABase/eabase.h>
#include <EABase/nullptr.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

namespace eastl
{


	///////////////////////////////////////////////////////////////////////
	// is_void
	//
	// is_void<T>::value == true if and only if T is one of the following types:
	//    [const][volatile] void
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_void_CONFORMANCE 1    // is_void is conforming.

	template <typename T> struct is_void : public false_type{};

	template <> struct is_void<void>                : public true_type{};
	template <> struct is_void<void const>          : public true_type{};
	template <> struct is_void<void volatile>       : public true_type{};
	template <> struct is_void<void const volatile> : public true_type{};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_void_v = is_void<T>::value;
	#endif



	///////////////////////////////////////////////////////////////////////
	// has_void_arg
	//
	// utility which identifies if any of the given template arguments is void.
	//
	// TODO(rparolin):  refactor with fold expressions when C++17 compilers are widely available.
	///////////////////////////////////////////////////////////////////////

	template <typename ...Args> 
	struct has_void_arg;

	template <> 
	struct has_void_arg<> 
		: public eastl::false_type {};

	template <typename A0, typename ...Args>
	struct has_void_arg<A0, Args...>
		{ static const bool value = (eastl::is_void<A0>::value || eastl::has_void_arg<Args...>::value); };


	///////////////////////////////////////////////////////////////////////
	// is_null_pointer
	//
	// C++14 type trait. Refers only to nullptr_t and not NULL (0).
	// eastl::is_null_pointer<nullptr>::value == true
	// eastl::is_null_pointer<std::nullptr_t>::value == true
	// eastl::is_null_pointer<void*>::value == false
	// eastl::is_null_pointer<NULL>::value == [cannot compile]
	//
	///////////////////////////////////////////////////////////////////////

	#if defined(EA_COMPILER_CPP11_ENABLED) && !defined(EA_COMPILER_NO_DECLTYPE) && !defined(_MSC_VER) // VC++'s handling of decltype(nullptr) is broken.
		#define EASTL_TYPE_TRAIT_is_null_pointer_CONFORMANCE 1

		template <typename T> 
		struct is_null_pointer : public eastl::is_same<typename eastl::remove_cv<T>::type, decltype(nullptr)> {}; // A C++11 compiler defines nullptr, but you need a C++11 standard library to declare std::nullptr_t. So it's safer to compare against decltype(nullptr) than to use std::nullptr_t, because we may have a C++11 compiler but C++98 library (happens with Apple frequently).
	#else
		#define EASTL_TYPE_TRAIT_is_null_pointer_CONFORMANCE 1

		template <typename T> 
		struct is_null_pointer : public eastl::is_same<typename eastl::remove_cv<T>::type, std::nullptr_t> {};
	#endif

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_null_pointer_v = is_null_pointer<T>::value;
	#endif



	///////////////////////////////////////////////////////////////////////
	// is_integral
	//
	// is_integral<T>::value == true if and only if T  is one of the following types:
	//    [const] [volatile] bool
	//    [const] [volatile] char
	//    [const] [volatile] signed char
	//    [const] [volatile] unsigned char
	//    [const] [volatile] wchar_t
	//    [const] [volatile] short
	//    [const] [volatile] int
	//    [const] [volatile] long
	//    [const] [volatile] long long
	//    [const] [volatile] unsigned short
	//    [const] [volatile] unsigned int
	//    [const] [volatile] unsigned long
	//    [const] [volatile] unsigned long long
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_integral_CONFORMANCE 1    // is_integral is conforming.

	template <typename T> struct is_integral_helper           : public false_type{};

	template <> struct is_integral_helper<unsigned char>      : public true_type{};
	template <> struct is_integral_helper<unsigned short>     : public true_type{};
	template <> struct is_integral_helper<unsigned int>       : public true_type{};
	template <> struct is_integral_helper<unsigned long>      : public true_type{};
	template <> struct is_integral_helper<unsigned long long> : public true_type{};

	template <> struct is_integral_helper<signed char>        : public true_type{};
	template <> struct is_integral_helper<signed short>       : public true_type{};
	template <> struct is_integral_helper<signed int>         : public true_type{};
	template <> struct is_integral_helper<signed long>        : public true_type{};
	template <> struct is_integral_helper<signed long long>   : public true_type{};

	template <> struct is_integral_helper<bool>               : public true_type{};
	template <> struct is_integral_helper<char>               : public true_type{};
	#if defined(EA_CHAR16_NATIVE) && EA_CHAR16_NATIVE
		template <> struct is_integral_helper<char16_t>       : public true_type{};
	#endif
	#if defined(EA_CHAR32_NATIVE) && EA_CHAR32_NATIVE
		template <> struct is_integral_helper<char32_t>       : public true_type{};
	#endif
	#ifndef EA_WCHAR_T_NON_NATIVE // If wchar_t is a native type instead of simply a define to an existing type which is already handled above...
		template <> struct is_integral_helper<wchar_t>        : public true_type{};
	#endif
	#if EASTL_INT128_SUPPORTED && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		template <> struct is_integral_helper<__int128_t>     : public true_type{};
		template <> struct is_integral_helper<__uint128_t>    : public true_type{};
	#endif

	template <typename T>
	struct is_integral : public eastl::is_integral_helper<typename eastl::remove_cv<T>::type>{};

	#define EASTL_DECLARE_INTEGRAL(T)                                             \
	namespace eastl{                                                              \
		template <> struct is_integral<T>                : public true_type{};    \
		template <> struct is_integral<const T>          : public true_type{};    \
		template <> struct is_integral<volatile T>       : public true_type{};    \
		template <> struct is_integral<const volatile T> : public true_type{};    \
	}

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_integral_v = is_integral<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_floating_point
	//
	// is_floating_point<T>::value == true if and only if T is one of the following types:
	//    [const] [volatile] float
	//    [const] [volatile] double
	//    [const] [volatile] long double
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_floating_point_CONFORMANCE 1    // is_floating_point is conforming.

	template <typename T> struct is_floating_point_helper    : public false_type{};

	template <> struct is_floating_point_helper<float>       : public true_type{};
	template <> struct is_floating_point_helper<double>      : public true_type{};
	template <> struct is_floating_point_helper<long double> : public true_type{};

	template <typename T>
	struct is_floating_point : public eastl::is_floating_point_helper<typename eastl::remove_cv<T>::type>{};

	#define EASTL_DECLARE_FLOATING_POINT(T)                                             \
	namespace eastl{                                                                    \
		template <> struct is_floating_point<T>                : public true_type{};    \
		template <> struct is_floating_point<const T>          : public true_type{};    \
		template <> struct is_floating_point<volatile T>       : public true_type{};    \
		template <> struct is_floating_point<const volatile T> : public true_type{};    \
	}

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_floating_point_v = is_floating_point<T>::value;
	#endif



	///////////////////////////////////////////////////////////////////////
	// is_arithmetic
	//
	// is_arithmetic<T>::value == true if and only if:
	//    is_floating_point<T>::value == true, or
	//    is_integral<T>::value == true
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_arithmetic_CONFORMANCE 1    // is_arithmetic is conforming.

	template <typename T>
	struct is_arithmetic 
		: public integral_constant<bool, is_integral<T>::value || is_floating_point<T>::value> {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_arithmetic_v = is_arithmetic<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_fundamental
	//
	// is_fundamental<T>::value == true if and only if:
	//    is_floating_point<T>::value == true, or
	//    is_integral<T>::value == true, or
	//    is_void<T>::value == true
	//    is_null_pointer<T>::value == true
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_fundamental_CONFORMANCE 1    // is_fundamental is conforming.

	template <typename T>
	struct is_fundamental
		: public bool_constant<is_void_v<T> || is_integral_v<T> || is_floating_point_v<T> || is_null_pointer_v<T>> {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_fundamental_v = is_fundamental<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_hat_type
	//
	// is_hat_type<T>::value == true if and only if:
	//    underlying type is a C++/CX '^' type such as: Foo^
	//	  meaning the type is heap allocated and ref-counted
	///////////////////////////////////////////////////////////////////////

	template <typename T> struct is_hat_type_helper : public false_type {};

	#if (EABASE_VERSION_N > 20607 && defined(EA_COMPILER_WINRTCX_ENABLED)) || defined(__cplusplus_winrt)
		template <typename T> struct is_hat_type_helper<T^> : public true_type{};
	#endif 

	template <typename T>
	struct is_hat_type : public eastl::is_hat_type_helper<T> {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_hat_type_v = is_hat_type<T>::value;
	#endif

} // namespace eastl


#endif // Header include guard





















