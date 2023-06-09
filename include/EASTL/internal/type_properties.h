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
#include <EASTL/internal/type_compound.h>


namespace eastl
{


	///////////////////////////////////////////////////////////////////////
	// underlying_type
	//
	// Defines a member typedef type of type that is the underlying type for the enumeration T.
	// Requires explicit compiler support to implement.
	//
	///////////////////////////////////////////////////////////////////////

	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && ((defined(_MSC_VER) && (_MSC_VER >= 1700)) || (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4007)) || defined(__clang__)) // VS2012+
		#define EASTL_TYPE_TRAIT_underlying_type_CONFORMANCE 1    // underlying_type is conforming.

		template <typename T>
		struct underlying_type{ typedef __underlying_type(T) type; };

	#else
		#define EASTL_TYPE_TRAIT_underlying_type_CONFORMANCE 0

		template <typename T>
		struct underlying_type{ typedef int type; };    // This is of course wrong, but we emulate libstdc++ and typedef it as int.
	#endif

	#if !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		template <typename T>
		using underlying_type_t = typename underlying_type<T>::type;
	#endif

	///////////////////////////////////////////////////////////////////////
	// to_underlying 
	//
	// Cast a enum value to its underlying type.
	// For example:
	// 
	// enum class MyEnum : uint8_t { Value = 0; }
	// auto x = MyEnum::Value; 
	// std::cout << to_underlying(x); // equivalent to  sts::cout << static_cast<uint8_t>(x);
	///////////////////////////////////////////////////////////////////////

	#if EASTL_VARIABLE_TEMPLATES_ENABLED && !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		template<class T>
		constexpr underlying_type_t<T> to_underlying(T value) noexcept
		{
			return static_cast<underlying_type_t<T>>(value);
		}
	#endif


	///////////////////////////////////////////////////////////////////////
	// has_unique_object_representations
	//
	// If T is TriviallyCopyable and if any two objects of type T with the same
	// value have the same object representation, value is true. For any other
	// type, value is false.
	//
	// http://en.cppreference.com/w/cpp/types/has_unique_object_representations
	///////////////////////////////////////////////////////////////////////
	#if EASTL_HAS_UNIQUE_OBJECT_REPRESENTATIONS_AVAILABLE
		#define EASTL_TYPE_TRAIT_has_unique_object_representations_CONFORMANCE 1

		template <typename T>
		struct has_unique_object_representations
			: public integral_constant<bool, __has_unique_object_representations(remove_cv_t<remove_all_extents_t<T>>)>
		{
		};

	#else
		#define EASTL_TYPE_TRAIT_has_unique_object_representations_CONFORMANCE 0

		template <typename T>
		struct has_unique_object_representations
			: public integral_constant<bool, is_integral_v<remove_cv_t<remove_all_extents_t<T>>>> // only integral types (floating point types excluded).
		{
		};

	#endif

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR auto has_unique_object_representations_v = has_unique_object_representations<T>::value;
	#endif


    ///////////////////////////////////////////////////////////////////////
	// is_signed
	//
	// is_signed<T>::value == true if T is a (possibly cv-qualified) floating-point or signed integer type.
	//
	// Used to determine if a type is signed.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_signed_CONFORMANCE 1    // is_signed is conforming.
		
#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4296)  // '<': expression is always false
#endif
	template<typename T, bool = is_arithmetic<T>::value>
	struct is_signed_helper : bool_constant<T(-1) < T(0)> {};
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

	template<typename T>
	struct is_signed_helper<T, false> : false_type {};

	template <typename T>
	struct is_signed : public eastl::is_signed_helper<T>::type {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_signed_v = is_signed<T>::value;
	#endif

	#define EASTL_DECLARE_SIGNED(T)                                             \
	namespace eastl{                                                            \
		template <> struct EASTL_REMOVE_AT_2024_APRIL is_signed<T>                : public true_type{};    \
		template <> struct EASTL_REMOVE_AT_2024_APRIL is_signed<const T>          : public true_type{};    \
		template <> struct EASTL_REMOVE_AT_2024_APRIL is_signed<volatile T>       : public true_type{};    \
		template <> struct EASTL_REMOVE_AT_2024_APRIL is_signed<const volatile T> : public true_type{};    \
	}



	///////////////////////////////////////////////////////////////////////
	// is_unsigned
	//
	// is_unsigned<T>::value == true if T is a (possibly cv-qualified) bool or unsigned integer type.
	//
	// Used to determine if a type is unsigned.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_unsigned_CONFORMANCE 1    // is_unsigned is conforming.
		
#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4296)  // '<': expression is always false
#endif
	template<typename T, bool = is_arithmetic<T>::value>
	struct is_unsigned_helper : integral_constant<bool, T(0) < T(-1)> {};
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

	template<typename T>
	struct is_unsigned_helper<T, false> : false_type {};

	template <typename T>
	struct is_unsigned : public eastl::is_unsigned_helper<T>::type {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_unsigned_v = is_unsigned<T>::value;
	#endif

	#define EASTL_DECLARE_UNSIGNED(T)                                             \
	namespace eastl{                                                              \
		template <> struct EASTL_REMOVE_AT_2024_APRIL is_unsigned<T>                : public true_type{};    \
		template <> struct EASTL_REMOVE_AT_2024_APRIL is_unsigned<const T>          : public true_type{};    \
		template <> struct EASTL_REMOVE_AT_2024_APRIL is_unsigned<volatile T>       : public true_type{};    \
		template <> struct EASTL_REMOVE_AT_2024_APRIL is_unsigned<const volatile T> : public true_type{};    \
	}

	///////////////////////////////////////////////////////////////////////
	// is_bounded_array
	//
	// is_bounded_array<T>::value == true if T is an array type of known bound.
	//
	// is_bounded_array<int>::value is false.
	// is_bounded_array<int[5]>::value is true.
	// is_bounded_array<int[]>::value is false.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_bounded_array_CONFORMANCE 1    // is_bounded_array is conforming.

	template<class T>
	struct is_bounded_array: eastl::false_type {};

	template<class T, size_t N>
	struct is_bounded_array<T[N]> : eastl::true_type {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_bounded_array_v = is_bounded_array<T>::value;
	#endif

	///////////////////////////////////////////////////////////////////////
	// is_unbounded_array
	//
	// is_unbounded_array<T>::value == true if T is an array type of known bound.
	//
	// is_unbounded_array<int>::value is false.
	// is_unbounded_array<int[5]>::value is false.
	// is_unbounded_array<int[]>::value is true.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_unbounded_array_CONFORMANCE 1    // is_unbounded_array is conforming.

	template<class T>
	struct is_unbounded_array: eastl::false_type {};

	template<class T>
	struct is_unbounded_array<T[]> : eastl::true_type {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_unbounded_array_v = is_unbounded_array<T>::value;
	#endif

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

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR auto rank_v = rank<T>::value;
	#endif


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

	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(_MSC_VER) || defined(EA_COMPILER_GNUC) || ((defined(__clang__)) && EA_COMPILER_HAS_FEATURE(is_base_of)))
		#define EASTL_TYPE_TRAIT_is_base_of_CONFORMANCE 1    // is_base_of is conforming.

		template <typename Base, typename Derived>
		struct is_base_of : public eastl::integral_constant<bool, __is_base_of(Base, Derived) || eastl::is_same<Base, Derived>::value>{};

		#if EASTL_VARIABLE_TEMPLATES_ENABLED
			template <typename Base, typename Derived>
			EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR bool is_base_of_v = is_base_of<Base, Derived>::value;
		#endif
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

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_lvalue_reference_v = is_lvalue_reference<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_rvalue_reference
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_rvalue_reference_CONFORMANCE 1    // is_rvalue_reference is conforming.

	template <typename T> struct is_rvalue_reference      : public eastl::false_type {};
	template <typename T> struct is_rvalue_reference<T&&> : public eastl::true_type {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_rvalue_reference_v = is_rvalue_reference<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// result_of
	//
	// Deprecated in C++17.
	// 
	// Use invoke_result instead.
	// See https://en.cppreference.com/w/cpp/types/result_of#Notes for an explanation of issues with result_of.
	///////////////////////////////////////////////////////////////////////
	#define EASTL_TYPE_TRAIT_result_of_CONFORMANCE 1    // result_of is conforming.

	template<typename> struct EASTL_REMOVE_AT_2024_APRIL result_of;

	template<typename F, typename... ArgTypes>
	struct EASTL_REMOVE_AT_2024_APRIL result_of<F(ArgTypes...)>
		{ typedef decltype(eastl::declval<F>()(eastl::declval<ArgTypes>()...)) type; };


	// result_of_t is the C++14 using typedef for typename result_of<T>::type.
	// We provide a backwards-compatible means to access it through a macro for pre-C++11 compilers.
	#if defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		#define EASTL_RESULT_OF_T(T) typename result_of<T>::type
	#else
		template <typename T>
		using result_of_t EASTL_REMOVE_AT_2024_APRIL = typename result_of<T>::type;
		#define EASTL_RESULT_OF_T(T) result_of_t<T>
	#endif


	///////////////////////////////////////////////////////////////////////
	// has_equality
	//
	// Determines if the specified type can be tested for equality.
	//
	///////////////////////////////////////////////////////////////////////
	template <typename, typename = eastl::void_t<>>
	struct has_equality : eastl::false_type {};

	template <typename T>
	struct has_equality<T, eastl::void_t<decltype(eastl::declval<T>() == eastl::declval<T>())>> : eastl::true_type
	{
	};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR auto has_equality_v = has_equality<T>::value;
	#endif

	namespace internal
	{
		///////////////////////////////////////////////////////////////////////
		// is_complete_type
		//
		// Determines if the specified type is complete
		//
		// Warning: Be careful when using is_complete_type since the value is fixed at first instantiation.
		// Consider the following:
		//
		// struct Foo;
		// is_complete_type_v<Foo> // false
		// struct Foo {};
		// is_complete_type_v<Foo> // still false
		///////////////////////////////////////////////////////////////////////

		template<typename T, typename = void>
		struct is_complete_type : public false_type {};

		template<typename T>
		struct is_complete_type<T, eastl::void_t<decltype(sizeof(T) != 0)>> : public true_type {};

		template<>
		struct is_complete_type<const volatile void> : public false_type {};
		template<>
		struct is_complete_type<const void> : public false_type {};
		template<>
		struct is_complete_type<volatile void> : public false_type {};
		template<>
		struct is_complete_type<void> : public false_type {};

		template<typename T>
		struct is_complete_type<T, eastl::enable_if_t<eastl::is_function_v<T>>> : public true_type {};

		template <typename T>
		EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR bool is_complete_type_v = is_complete_type<T, void>::value;
	}

} // namespace eastl


#endif // Header include guard
