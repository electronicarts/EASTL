/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_TYPE_PROPERTIES_H
#define EASTL_INTERNAL_TYPE_PROPERTIES_H


#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#include <limits.h>


namespace eastl
{


	///////////////////////////////////////////////////////////////////////
	// underlying_type
	//
	// Defines a member typedef type of type that is the underlying type for the enumeration T. 
	// Requires explicit compiler support to implement.
	//
	///////////////////////////////////////////////////////////////////////

	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && ((defined(_MSC_VER) && (_MSC_VER >= 1700)) || (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4007)) || defined(EA_COMPILER_CLANG)) // VS2012+
		#define EASTL_TYPE_TRAIT_underlying_type_CONFORMANCE 1    // underlying_type is conforming.

		template <typename T> 
		struct underlying_type{ typedef __underlying_type(T) type; };

	#else
		#define EASTL_TYPE_TRAIT_underlying_type_CONFORMANCE 0

		template <typename T> 
		struct underlying_type{ typedef int type; };    // This is of course wrong, but we emulate libstdc++ and typedef it as int.
	#endif




	///////////////////////////////////////////////////////////////////////
	// is_signed
	//
	// is_signed<T>::value == true if and only if T is one of the following types:
	//    [const] [volatile] char (maybe)
	//    [const] [volatile] signed char
	//    [const] [volatile] short
	//    [const] [volatile] int
	//    [const] [volatile] long
	//    [const] [volatile] long long
	//    [const] [volatile] float
	//    [const] [volatile] double
	//    [const] [volatile] long double
	// 
	// Used to determine if a integral type is signed or unsigned.
	// Given that there are some user-made classes which emulate integral
	// types, we provide the EASTL_DECLARE_SIGNED macro to allow you to 
	// set a given class to be identified as a signed type.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_signed_CONFORMANCE 1    // is_signed is conforming.

	template <typename T> struct is_signed_helper : public false_type{};

	template <> struct is_signed_helper<signed char>      : public true_type{};
	template <> struct is_signed_helper<signed short>     : public true_type{};
	template <> struct is_signed_helper<signed int>       : public true_type{};
	template <> struct is_signed_helper<signed long>      : public true_type{};
	template <> struct is_signed_helper<signed long long> : public true_type{};
	template <> struct is_signed_helper<float>            : public true_type{};
	template <> struct is_signed_helper<double>           : public true_type{};
	template <> struct is_signed_helper<long double>      : public true_type{};

	#if (CHAR_MAX == SCHAR_MAX)
		template <> struct is_signed_helper<char>         : public true_type{};
	#endif
	#ifndef EA_WCHAR_T_NON_NATIVE // If wchar_t is a native type instead of simply a define to an existing type...
		#if defined(__WCHAR_MAX__) && ((__WCHAR_MAX__ == 2147483647) || (__WCHAR_MAX__ == 32767)) // GCC defines __WCHAR_MAX__ for most platforms.
			template <> struct is_signed_helper<wchar_t>  : public true_type{};
		#endif
	#endif

	template <typename T>
	struct is_signed : public eastl::is_signed_helper<typename eastl::remove_cv<T>::type>{};


	#define EASTL_DECLARE_SIGNED(T)                                             \
	namespace eastl{                                                            \
		template <> struct is_signed<T>                : public true_type{};    \
		template <> struct is_signed<const T>          : public true_type{};    \
		template <> struct is_signed<volatile T>       : public true_type{};    \
		template <> struct is_signed<const volatile T> : public true_type{};    \
	}



	///////////////////////////////////////////////////////////////////////
	// is_unsigned
	//
	// is_unsigned<T>::value == true if and only if T is one of the following types:
	//    [const] [volatile] char (maybe)
	//    [const] [volatile] unsigned char
	//    [const] [volatile] unsigned short
	//    [const] [volatile] unsigned int
	//    [const] [volatile] unsigned long
	//    [const] [volatile] unsigned long long
	//
	// Used to determine if a integral type is signed or unsigned. 
	// Given that there are some user-made classes which emulate integral
	// types, we provide the EASTL_DECLARE_UNSIGNED macro to allow you to 
	// set a given class to be identified as an unsigned type.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_unsigned_CONFORMANCE 1    // is_unsigned is conforming.

	template <typename T> struct is_unsigned_helper : public false_type{};

	template <> struct is_unsigned_helper<unsigned char>      : public true_type{};
	template <> struct is_unsigned_helper<unsigned short>     : public true_type{};
	template <> struct is_unsigned_helper<unsigned int>       : public true_type{};
	template <> struct is_unsigned_helper<unsigned long>      : public true_type{};
	template <> struct is_unsigned_helper<unsigned long long> : public true_type{};

	#if (CHAR_MAX == UCHAR_MAX)
		template <> struct is_unsigned_helper<char>           : public true_type{};
	#endif
	#ifndef EA_WCHAR_T_NON_NATIVE // If wchar_t is a native type instead of simply a define to an existing type...
		#if defined(_MSC_VER) || (defined(__WCHAR_MAX__) && ((__WCHAR_MAX__ == 4294967295U) || (__WCHAR_MAX__ == 65535))) // GCC defines __WCHAR_MAX__ for most platforms.
			template <> struct is_unsigned_helper<wchar_t>    : public true_type{};
		#endif
	#endif

	template <typename T>
	struct is_unsigned : public eastl::is_unsigned_helper<typename eastl::remove_cv<T>::type>{};

	#define EASTL_DECLARE_UNSIGNED(T)                                             \
	namespace eastl{                                                              \
		template <> struct is_unsigned<T>                : public true_type{};    \
		template <> struct is_unsigned<const T>          : public true_type{};    \
		template <> struct is_unsigned<volatile T>       : public true_type{};    \
		template <> struct is_unsigned<const volatile T> : public true_type{};    \
	}



	///////////////////////////////////////////////////////////////////////
	// alignment_of
	//
	// alignment_of<T>::value is an integral value representing, in bytes, 
	// the memory alignment of objects of type T.
	//
	// alignment_of may only be applied to complete types.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_alignment_of_CONFORMANCE 1    // alignment_of is conforming.

	template <typename T>
	struct alignment_of_value{ static const size_t value = EASTL_ALIGN_OF(T); };

	template <typename T>
	struct alignment_of : public integral_constant<size_t, alignment_of_value<T>::value>{};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR size_t alignment_of_v = alignment_of<T>::value;
	#endif


    ///////////////////////////////////////////////////////////////////////
	// is_aligned
	//
	// Defined as true if the type has alignment requirements greater 
	// than default alignment, which is taken to be 8. This allows for 
	// doing specialized object allocation and placement for such types.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_aligned_CONFORMANCE 1    // is_aligned is conforming.

	template <typename T>
	struct is_aligned_value{ static const bool value = (EASTL_ALIGN_OF(T) > 8); };

	template <typename T> 
	struct is_aligned : public integral_constant<bool, is_aligned_value<T>::value>{};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR size_t is_aligned_v = is_aligned<T>::value;
	#endif



	///////////////////////////////////////////////////////////////////////
	// rank
	//
	// rank<T>::value is an integral value representing the number of 
	// dimensions possessed by an array type. For example, given a 
	// multi-dimensional array type T[M][N], std::tr1::rank<T[M][N]>::value == 2. 
	// For a given non-array type T, std::tr1::rank<T>::value == 0.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_rank_CONFORMANCE 1    // rank is conforming.

	template<typename T>
	struct rank : public eastl::integral_constant<size_t, 0> {};
 
	template<typename T>
	struct rank<T[]> : public eastl::integral_constant<size_t, rank<T>::value + 1> {};
 
	template<typename T, size_t N>
	struct rank<T[N]> : public eastl::integral_constant<size_t, rank<T>::value + 1> {};



	///////////////////////////////////////////////////////////////////////
	// extent
	//
	// extent<T, I>::value is an integral type representing the number of 
	// elements in the Ith dimension of array type T.
	// 
	// For a given array type T[N], extent<T[N]>::value == N.
	// For a given multi-dimensional array type T[M][N], extent<T[M][N], 0>::value == N.
	// For a given multi-dimensional array type T[M][N], extent<T[M][N], 1>::value == M.
	// For a given array type T and a given dimension I where I >= rank<T>::value, extent<T, I>::value == 0.
	// For a given array type of unknown extent T[], extent<T[], 0>::value == 0.
	// For a given non-array type T and an arbitrary dimension I, extent<T, I>::value == 0.
	// 
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_extent_CONFORMANCE 1    // extent is conforming.

	template<typename T, unsigned N> 
	struct extent_help : public eastl::integral_constant<size_t, 0> {};

	template<typename T, unsigned I>
	struct extent_help<T[I], 0> : public eastl::integral_constant<size_t, I> {};

	template<typename T, unsigned N, unsigned I>
	struct extent_help<T[I], N> : public eastl::extent_help<T, N - 1> { };

	template<typename T, unsigned N>
	struct extent_help<T[], N> : public eastl::extent_help<T, N - 1> {};

	template<typename T, unsigned N = 0> // extent uses unsigned instead of size_t.
	struct extent : public eastl::extent_help<T, N> { };



	///////////////////////////////////////////////////////////////////////
	// is_base_of
	//
	// Given two (possibly identical) types Base and Derived, is_base_of<Base, Derived>::value == true 
	// if and only if Base is a direct or indirect base class of Derived, 
	// or Base and Derived are the same type.
	//
	// is_base_of may only be applied to complete types.
	//
	///////////////////////////////////////////////////////////////////////

	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(_MSC_VER) || defined(EA_COMPILER_GNUC) || (defined(EA_COMPILER_CLANG) && EA_COMPILER_HAS_FEATURE(is_base_of)))
		#define EASTL_TYPE_TRAIT_is_base_of_CONFORMANCE 1    // is_base_of is conforming.

		template <typename Base, typename Derived> 
		struct is_base_of : public eastl::integral_constant<bool, __is_base_of(Base, Derived) || eastl::is_same<Base, Derived>::value>{};
	#else
		// Not implemented yet.
		// This appears to be implementable. 
		#define EASTL_TYPE_TRAIT_is_base_of_CONFORMANCE 0
	#endif



	///////////////////////////////////////////////////////////////////////
	// is_lvalue_reference
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_lvalue_reference_CONFORMANCE 1    // is_lvalue_reference is conforming.

	template<typename T> struct is_lvalue_reference     : public eastl::false_type {};
	template<typename T> struct is_lvalue_reference<T&> : public eastl::true_type {};


	///////////////////////////////////////////////////////////////////////
	// is_rvalue_reference
	//
	///////////////////////////////////////////////////////////////////////

	#if EASTL_NO_RVALUE_REFERENCES
		#define EASTL_TYPE_TRAIT_is_rvalue_reference_CONFORMANCE 0    // Given that the compiler doesn't support rvalue references, maybe the conformance here should be 1, since the result of this is always correct.

		template <typename T> struct is_rvalue_reference      : public eastl::false_type {};
	#else
		#define EASTL_TYPE_TRAIT_is_rvalue_reference_CONFORMANCE 1    // is_rvalue_reference is conforming.

		template <typename T> struct is_rvalue_reference      : public eastl::false_type {};
		template <typename T> struct is_rvalue_reference<T&&> : public eastl::true_type {};
	#endif


	///////////////////////////////////////////////////////////////////////
	// result_of
	//
	///////////////////////////////////////////////////////////////////////

	#if defined(EA_COMPILER_NO_VARIADIC_TEMPLATES) || defined(EA_COMPILER_NO_DECLTYPE)
		//  To do: come up with the best possible alternative.
		#define EASTL_TYPE_TRAIT_result_of_CONFORMANCE 0
	#else
		#define EASTL_TYPE_TRAIT_result_of_CONFORMANCE 1    // result_of is conforming.

		template<typename> struct result_of;
 
		template<typename F, typename... ArgTypes>
		struct result_of<F(ArgTypes...)>
			{ typedef decltype(eastl::declval<F>()(eastl::declval<ArgTypes>()...)) type; };


		// result_of_t is the C++14 using typedef for typename result_of<T>::type.
		// We provide a backwards-compatible means to access it through a macro for pre-C++11 compilers.
	   #if defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
			#define EASTL_RESULT_OF_T(T) typename result_of<T>::type
		#else
			template <typename T>
			using result_of_t = typename result_of<T>::type;
			#define EASTL_RESULT_OF_T(T) result_of_t<T>
		#endif

	#endif


	///////////////////////////////////////////////////////////////////////
	// common_type
	// 
	// Determines the common type among all types T..., that is the type all T... 
	// can be implicitly converted to.
	//
	// It is intended that this be specialized by the user for cases where it
	// is useful to do so. Example specialization:
	//     template <typename Class1, typename Class2>
	//     struct common_type<MyClass1, MyClass2>{ typedef MyBaseClassB type; };
	//
	// The member typedef type shall be defined as set out in 20.9.7.6,p3. All types in
	// the parameter pack T shall be complete or (possibly cv) void. A program may 
	// specialize this trait if at least one template parameter in the specialization 
	// is a user-defined type. Note: Such specializations are needed when only  
	// explicit conversions are desired among the template arguments.
	///////////////////////////////////////////////////////////////////////

	#if defined(EA_COMPILER_NO_DECLTYPE)
		#define EASTL_TYPE_TRAIT_common_type_CONFORMANCE 0

		// Perhaps we can do better. On the other hand, compilers that don't support variadic 
		// templates and decltype probably won't need the common_type trait anyway.
		template <typename T, typename U = void, typename V = void>
		struct common_type
			{ typedef void type; };

		template <typename T, typename U>
		struct common_type<T, U, void>
			{ typedef void type; };

		template <typename T>
		struct common_type<T, T, void>
			{ typedef T type; };

		template <typename T>
		struct common_type<T, void, void>
			{ typedef T type; };

	#elif defined(EA_COMPILER_NO_VARIADIC_TEMPLATES)
		#define EASTL_TYPE_TRAIT_common_type_CONFORMANCE 0

		template <typename T, typename U = void, typename V = void>
		struct common_type
			{ typedef typename eastl::common_type<typename eastl::common_type<T, U>::type, V>::type type; };

		template <typename T>
		struct common_type<T, void, void>
			{ typedef T type; };

		template <typename T, typename U>
		class common_type<T, U, void>
		{
			#if EASTL_NO_RVALUE_REFERENCES
				static T t();
				static U u();
			#else
				static T&& t();
				static U&& u();
			#endif

		public:
			typedef typename eastl::remove_reference<decltype(true ? t() : u())>::type type;
		};

	#else
		#define EASTL_TYPE_TRAIT_common_type_CONFORMANCE 1    // common_type is conforming.

		template<typename... T>
		struct common_type;

		template<typename T>
		struct common_type<T>
			{ typedef T type; }; // Question: Should we use T or decay_t<T> here? The C++11 Standard specifically (20.9.7.6,p3) specifies that it be without decay, but libc++ uses decay.

		template<typename T, typename U>
		struct common_type<T, U>
		{
			static T&& t();
			static U&& u();
			typedef decltype(true ? t() : u()) type; // The type of a tertiary expression is set by the compiler to be the common type of the two result types.
		};

		template<typename T, typename U, typename... V>
		struct common_type<T, U, V...>
			{ typedef typename common_type<typename common_type<T, U>::type, V...>::type type; };


		// common_type_t is the C++14 using typedef for typename common_type<T...>::type.
		// We provide a backwards-compatible means to access it through a macro for pre-C++11 compilers.
		#if defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
			#define EASTL_COMMON_TYPE_T(...) typename common_type<__VA_ARGS__>::type
		#else
			template <typename... T>
			using common_type_t = typename common_type<T...>::type;
			#define EASTL_COMMON_TYPE_T(...) common_type_t<__VA_ARGS__>
		#endif

	#endif


} // namespace eastl


#endif // Header include guard




















