///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Specification
//
// This file implements C++ type traits as proposed by the emerging C++ update
// as of May, 2005. This update is known as "Proposed Draft Technical Report
// on C++ Library Extensions" and is document number n1745. It can be found
// on the Internet as n1745.pdf and as of this writing it is updated every
// couple months to reflect current thinking.
//////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Description
//
// EASTL includes a fairly serious type traits library that is on par with the
// one found in Boost but offers some additional performance-enhancing help as well.
// The type_traits library provides information about class types, as opposed to
// class instances. For example, the is_integral type trait tells if a type is
// one of int, short, long, char, uint64_t, etc.
//
// There are three primary uses of type traits:
//     * Allowing for optimized operations on some data types.
//     * Allowing for different logic pathways based on data types.
//     * Allowing for compile-type assertions about data type expectations.
//
// Most of the type traits are automatically detected and implemented by the compiler.
// However, EASTL allows for the user to explicitly give the compiler hints about
// type traits that the compiler cannot know, via the EASTL_DECLARE declarations.
// If the user has a class that is relocatable (i.e. can safely use memcpy to copy values),
// the user can use the EASTL_DECLARE_TRIVIAL_RELOCATE declaration to tell the compiler
// that the class can be copied via memcpy. This will automatically significantly speed
// up some containers and algorithms that use that class.
//
// Here is an example of using type traits to tell if a value is a floating point
// value or not:
//
//    template <typename T>
//    DoSomething(T t) {
//        assert(is_floating_point<T>::value);
//    }
//
// Here is an example of declaring a class as relocatable and using it in a vector.
//
//    EASTL_DECLARE_TRIVIAL_RELOCATE(Widget); // Usually you put this at the Widget class declaration.
//    vector<Widget> wVector;
//    wVector.erase(wVector.begin());         // This operation will be optimized via using memcpy.
//
// The following is a full list of the currently recognized type traits. Most of these
// are implemented as of this writing, but if there is one that is missing, feel free
// to contact the maintainer of this library and request that it be completed.
// As of this writing all C++11 type traits are supported, as well as some additional ones.
// http://en.cppreference.com/w/cpp/types
//
//    Trait                                 Description
// ------------------------------------------------------------------------------
//    is_void                               T is void or a cv-qualified (const/void-qualified) void.
//    is_null_pointer
//    is_integral                           T is an integral type.
//    is_floating_point                     T is a floating point type.
//    is_array                              T is an array type. The templated array container is not an array type.
//    is_enum                               T is an enumeration type.
//    is_union                              T is a union type.
//    is_class                              T is a class type but not a union type.
//    is_function                           T is a function type.
//    is_pointer                            T is a pointer type. Includes function pointers, but not pointers to (data or function) members.
//    is_rvalue_reference
//    is_lvalue_reference
//    is_member_object_pointer              T is a pointer to data member.
//    is_member_function_pointer            T is a pointer to member function.
//
//    is_fundamental                        T is a fundamental type (void, integral, or floating point).
//    is_arithmetic                         T is an arithmetic type (integral or floating point).
//    is_scalar                             T is a scalar type (arithmetic, enum, pointer, member_pointer)
//    is_object                             T is an object type.
//    is_compound                           T is a compound type (anything but fundamental).
//    is_reference                          T is a reference type. Includes references to functions.
//    is_member_pointer                     T is a pointer to a member or member function.
//
//    is_const                              T is const-qualified.
//    is_volatile                           T is volatile-qualified.
//    is_trivial
//    is_trivially_copyable
//    is_standard_layout
//    is_pod                                T is a POD type.
//    is_literal_type
//    is_empty                              T is an empty class.
//    is_polymorphic                        T is a polymorphic class.
//    is_abstract                           T is an abstract class.
//    is_signed                             T is a signed integral type.
//    is_unsigned                           T is an unsigned integral type.
//
//    is_constructible
//    is_trivially_constructible
//    is_nothrow_constructible
//    is_default_constructible
//    is_trivially_default_constructible
//    is_nothrow_default_constructible
//    is_copy_constructible
//    is_trivially_copy_constructible
//    is_nothrow_copy_constructible
//    is_move_constructible
//    is_trivially_move_constructible
//    is_nothrow_move_constructible
//    is_assignable
//    is_trivially_assignable
//    is_nothrow_assignable
//    is_copy_assignable
//    is_trivially_copy_assignable
//    is_nothrow_copy_assignable
//    is_move_assignable
//    is_trivially_move_assignable
//    is_nothrow_move_assignable
//    is_destructible
//    is_trivially_destructible
//    is_nothrow_destructible
//    has_virtual_destructor                T has a virtual destructor.
//
//    alignment_of                          An integer value representing the number of bytes of the alignment of objects of type T; an object of type T may be allocated at an address that is a multiple of its alignment.
//    rank                                  An integer value representing the rank of objects of type T. The term 'rank' here is used to describe the number of dimensions of an array type.
//    extent                                An integer value representing the extent (dimension) of the I'th bound of objects of type T. If the type T is not an array type, has rank of less than I, or if I == 0 and T is of type 'array of unknown bound of U,' then value shall evaluate to zero; otherwise value shall evaluate to the number of elements in the I'th array bound of T. The term 'extent' here is used to describe the number of elements in an array type.
//
//    is_same                               T and U name the same type.
//    is_base_of                            Base is a base class of Derived or Base and Derived name the same type.
//    is_convertible                        An imaginary lvalue of type From is implicitly convertible to type To. Special conversions involving string-literals and null-pointer constants are not considered. No function-parameter adjustments are made to type To when determining whether From is convertible to To; this implies that if type To is a function type or an array type, then the condition is false.
//
//    remove_cv
//    remove_const                          The member typedef type shall be the same as T except that any top level const-qualifier has been removed. remove_const<const volatile int>::type evaluates to volatile int, whereas remove_const<const int*> is const int*.
//    remove_volatile
//    add_cv
//    add_const
//    add_volatile
//
//    remove_reference
//    add_lvalue_reference
//    add_rvalue_reference
//
//    remove_pointer
//    add_pointer
//
//    make_signed
//    make_unsigned
//
//    remove_extent
//    remove_all_extents
//
//    aligned_storage
//    aligned_union
//    decay
//    enable_if
//    conditional
//    common_type
//    underlying_type
//    result_of
//
//    integral_constant
//    bool_constant
//    true_type
//    false_type
//
// EASTL extension type traits
//    identity                              Simply sets T as type.
//    is_aligned                            Defined as true if the type has alignment requirements greater than default alignment, which is taken to be 8. is_aligned is not found in Boost nor C++11, though alignment_of is.
//    union_cast                            Allows for easy-to-read casting between types that are unrelated but have binary equivalence. The classic use case is converting between float and int32_t bit representations.
//    is_array_of_known_bounds
//    is_array_of_unknown_bounds
//    add_signed                            Deprecated in favor of make_signed.
//    add_unsigned                          Deprecated in favor of make_unsigned.
//    add_reference
//    yes_type
//    no_type
//    is_swappable                          Found in <EASTL/utility.h>
//    is_nothrow_swappable                  "
//    is_reference_wrapper                  Found in <EASTL/functional.h>
//    remove_reference_wrapper              "
//
// Deprecated pre-C++11 type traits
//    has_trivial_constructor               The default constructor for T is trivial.
//    has_trivial_copy                      The copy constructor for T is trivial.
//    has_trivial_assign                    The assignment operator for T is trivial.
//    has_trivial_destructor                The destructor for T is trivial.
//    has_nothrow_constructor               The default constructor for T has an empty exception specification or can otherwise be deduced never to throw an exception.
//    has_nothrow_copy                      The copy constructor for T has an empty exception specification or can otherwise be deduced never to throw an exception.
//    has_nothrow_assign                    The assignment operator for T has an empty exception specification or can otherwise be deduced never to throw an exception.
//   *has_trivial_relocate                  T can be moved to a new location via bitwise copy. Note that C++11 rvalue/move functionality supercedes this.
//
// * has_trivial_relocate is not found in Boost nor the pre-C++ standard update proposal.
//   However, it is somewhat useful in pre-C++11 environments (prior to move semantics)
//   for allowing the generation of optimized object moving operations. It is similar to
//   the is_pod type trait, but goes further and allows non-pod classes to be categorized
//   as relocatable. Such categorization is something that no compiler can do, as only
//   the user can know if it is such. Thus EASTL_DECLARE_TRIVIAL_RELOCATE is provided to
//   allow the user to give the compiler a hint. However, C++11 rvalue/move functionality
//   supercedes this and will eventually fully displace it.
//
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// Requirements
//
// As of this writing (5/2005), type_traits here requires a well-conforming
// C++ compiler with respect to template metaprogramming. To use this library
// you need to have at least one of the following:
//     MSVC++ 7.1       (includes Win32, Win64, and WinCE platforms)
//     GCC 3.2          (includes MacOSX, and Linux platforms)
//     Metrowerks 8.0   (incluees MacOSX, Windows, and other platforms)
//     EDG              (includes any compiler with EDG as a back-end, such as the Intel compiler)
//     Comeau           (this is a C++ to C generator)
//
// It may be useful to list the compilers/platforms the current version of
// type_traits doesn't support:
//     Borland C++      (it simply has too many bugs with respect to templates).
//     GCC 2.96         We used to have a separate set of type traits for this compiler, but removed it due to lack of necessity.
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Implementation
//
// The implementation here is almost entirely based on template metaprogramming.
// This is whereby you use the compiler's template functionality to define types
// and values and make compilation decisions based on template declarations.
// Many of the algorithms here are similar to those found in books such as
// "Modern C++ Design" and C++ libraries such as Boost. The implementations here
// are simpler and more straightforward than those found in some libraries, due
// largely to our assumption that the compiler is good at doing template programming.
///////////////////////////////////////////////////////////////////////////////



#ifndef EASTL_TYPE_TRAITS_H
#define EASTL_TYPE_TRAITS_H



#include <EASTL/internal/config.h>
#include <stddef.h>                 // Is needed for size_t usage by some traits.

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	///////////////////////////////////////////////////////////////////////
	// integral_constant
	//
	// This is the base class for various type traits, as defined by C++11.
	// This is essentially a utility base class for defining properties
	// as both class constants (value) and as types (type).
	//
	template <typename T, T v>
	struct integral_constant
	{
		static EA_CONSTEXPR T value = v;
		typedef T value_type;
		typedef integral_constant<T, v> type;

		EA_CONSTEXPR operator value_type() const EA_NOEXCEPT { return value; }
		EA_CONSTEXPR value_type operator()() const EA_NOEXCEPT { return value; }
	};


	///////////////////////////////////////////////////////////////////////
	// true_type / false_type
	//
	// These are commonly used types in the implementation of type_traits.
	// Other integral constant types can be defined, such as those based on int.
	//
	typedef integral_constant<bool, true>  true_type;
	typedef integral_constant<bool, false> false_type;


	///////////////////////////////////////////////////////////////////////
	// bool_constant
	//
	// This is a convenience helper for the often used integral_constant<bool, value>.
	//
	#if defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		template <bool B>
		struct bool_constant : public integral_constant<bool, B> {};
	#else
		template <bool B>
		using bool_constant = integral_constant<bool, B>;
	#endif


	///////////////////////////////////////////////////////////////////////
	// yes_type / no_type
	//
	// These are used as a utility to differentiate between two things.
	//
	typedef char yes_type;                      // sizeof(yes_type) == 1
	struct       no_type { char padding[8]; };  // sizeof(no_type)  != 1



	///////////////////////////////////////////////////////////////////////
	// unused
	//
	// Used internally to denote a special template argument that means
	// it's an unused argument.
	//
	struct unused { };


	///////////////////////////////////////////////////////////////////////
	// argument_sink
	//
	// Used as a type which constructs from anything.
	//
	// For compilers that support variadic templates we provide an
	// alternative argument_sink which provides a constructor overload of
	// the variadic pack of arguments by reference.  This avoids issues of
	// object alignment not being respected in Microsoft compilers.  Seen
	// in VS2015 preview.  In general, since arguments are consumed and
	// ignored its cheaper to consume references than passing by value
	// which incurs a construction cost.
	struct argument_sink
	{
		template<typename... Args>
		argument_sink(Args&&...) {}
	};


	///////////////////////////////////////////////////////////////////////
	// type_select
	//
	// This is used to declare a type from one of two type options.
	// The result is based on the condition type. This has certain uses
	// in template metaprogramming.
	//
	// Example usage:
	//    typedef ChosenType = typename type_select<is_integral<SomeType>::value, ChoiceAType, ChoiceBType>::type;
	//        or
	//    using ChosenType = type_select_t<is_integral_v<SomeType>, ChoiceAType, ChoiceBType>;
	//
	template <bool bCondition, class ConditionIsTrueType, class ConditionIsFalseType>
	struct type_select { typedef ConditionIsTrueType type; };

	template <typename ConditionIsTrueType, class ConditionIsFalseType>
	struct type_select<false, ConditionIsTrueType, ConditionIsFalseType> { typedef ConditionIsFalseType type; };

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <bool bCondition, class ConditionIsTrueType, class ConditionIsFalseType>
		using type_select_t = typename type_select<bCondition, ConditionIsTrueType, ConditionIsFalseType>::type;
	#endif



	///////////////////////////////////////////////////////////////////////
	// first_type_select
	//
	//  Similar to type_select but unilaterally selects the first type.
	//
	template <typename T, typename = eastl::unused, typename = eastl::unused>
	struct first_type_select { typedef T type; };



	///////////////////////////////////////////////////////////////////////
	// type_or
	//
	// This is a utility class for creating composite type traits.
	//
	template <bool b1, bool b2, bool b3 = false, bool b4 = false, bool b5 = false>
	struct type_or;

	template <bool b1, bool b2, bool b3, bool b4, bool b5>
	struct type_or { static const bool value = true; };

	template <>
	struct type_or<false, false, false, false, false> { static const bool value = false; };



	///////////////////////////////////////////////////////////////////////
	// type_and
	//
	// This is a utility class for creating composite type traits.
	//
	template <bool b1, bool b2, bool b3 = true, bool b4 = true, bool b5 = true>
	struct type_and;

	template <bool b1, bool b2, bool b3, bool b4, bool b5>
	struct type_and{ static const bool value = false; };

	template <>
	struct type_and<true, true, true, true, true>{ static const bool value = true; };



	///////////////////////////////////////////////////////////////////////
	// type_equal
	//
	// This is a utility class for creating composite type traits.
	//
	template <int b1, int b2>
	struct type_equal{ static const bool value = (b1 == b2); };



	///////////////////////////////////////////////////////////////////////
	// type_not_equal
	//
	// This is a utility class for creating composite type traits.
	//
	template <int b1, int b2>
	struct type_not_equal{ static const bool value = (b1 != b2); };



	///////////////////////////////////////////////////////////////////////
	// type_not
	//
	// This is a utility class for creating composite type traits.
	//
	template <bool b>
	struct type_not{ static const bool value = true; };

	template <>
	struct type_not<true>{ static const bool value = false; };



	///////////////////////////////////////////////////////////////////////
	// enable_if, disable_if
	//
	// template <bool B, typename T = void> struct enable_if;
	// template <bool B, typename T = void> struct disable_if;

	template<bool B, typename T = void>
	struct enable_if {};

	template <typename T>
	struct enable_if<true, T> { typedef T type; };

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <bool B, class T = void>
		using enable_if_t = typename enable_if<B, T>::type;
	#endif


	template<bool B, typename T = void>
	struct disable_if {};

	template <typename T>
	struct disable_if<false, T> { typedef T type; };

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <bool B, class T = void>
		using disable_if_t = typename disable_if<B, T>::type;
	#endif



	///////////////////////////////////////////////////////////////////////
	// conditional
	//
	// Provides member typedef type which is defined as T if B is true at
	// compile time, or as F if B is false.
	//
	template<bool B, typename T, typename F>
	struct conditional { typedef T type; };

	template <typename T, typename F>
	struct conditional<false, T, F> { typedef F type; };

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <bool B, class T, class F>
		using conditional_t = typename conditional<B, T, F>::type;
	#endif



	///////////////////////////////////////////////////////////////////////
	// conjunction
	//
	// This is a C++17 standard utility class that performs a short-circuiting
	// logical AND on a sequence of type traits.
	//
	// http://en.cppreference.com/w/cpp/types/conjunction
	//
	template <class...>
	struct conjunction : eastl::true_type {};

	template <class B>
	struct conjunction<B> : B {};

	template <class B, class... Bn>
	struct conjunction<B, Bn...> : conditional<bool(B::value), conjunction<Bn...>, B>::type {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <typename... Bn>
		EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR bool conjunction_v = conjunction<Bn...>::value;
	#endif



	///////////////////////////////////////////////////////////////////////
	// disjunction
	//
	// This is a C++17 standard utility class that performs a short-circuiting
	// logical OR on a sequence of type traits.
	//
	// http://en.cppreference.com/w/cpp/types/disjunction
	//
	template <class...>
	struct disjunction : eastl::false_type {};

	template <class B>
	struct disjunction<B> : B {};

	template <class B, class... Bn>
	struct disjunction<B, Bn...> : conditional<bool(B::value), B, disjunction<Bn...>>::type {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <typename... B>
		EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR bool disjunction_v = disjunction<B...>::value;
	#endif



	///////////////////////////////////////////////////////////////////////
	// negation
	//
	// This is a C++17 standard utility class that performs a logical NOT on a
	// single type trait.
	//
	// http://en.cppreference.com/w/cpp/types/negation
	//
	template <class B>
	struct negation : eastl::bool_constant<!bool(B::value)> {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <typename B>
		EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR bool negation_v = negation<B>::value;
	#endif



	///////////////////////////////////////////////////////////////////////
	// identity
	//
	// The purpose of this is typically to deal with non-deduced template
	// contexts. See the C++11 Standard, 14.8.2.5 p5.
	// Also: http://cppquiz.org/quiz/question/109?result=CE&answer=&did_answer=Answer
	//
	// Dinkumware has an identity, but adds a member function to it:
	//     const T& operator()(const T& t) const{ return t; }
	//
	// NOTE(rparolin): Use 'eastl::type_identity' it was included in the C++20
	// standard. This is a legacy EASTL type we continue to support for
	// backwards compatibility. 
	//
	template <typename T>
	struct identity { using type = T; };

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <typename T>
		using identity_t = typename identity<T>::type;
	#endif


	///////////////////////////////////////////////////////////////////////
	// type_identity
	//
	// The purpose of this is typically to deal with non-deduced template
	// contexts. See the C++11 Standard, 14.8.2.5 p5.
	// Also: http://cppquiz.org/quiz/question/109?result=CE&answer=&did_answer=Answer
	//
	// https://en.cppreference.com/w/cpp/types/type_identity
	//
	template <typename T>
	struct type_identity { using type = T; };

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <typename T>
		using type_identity_t = typename type_identity<T>::type;
	#endif



	///////////////////////////////////////////////////////////////////////
	// is_same
	//
	// Given two (possibly identical) types T and U, is_same<T, U>::value == true
	// if and only if T and U are the same type.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_same_CONFORMANCE 1    // is_same is conforming; doesn't make mistakes.

	template <typename T, typename U>
	struct is_same : public eastl::false_type { };

	template <typename T>
	struct is_same<T, T> : public eastl::true_type { };

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T, class U>
		EA_CONSTEXPR bool is_same_v = is_same<T, U>::value;
	#endif


    ///////////////////////////////////////////////////////////////////////
	// is_const
	//
	// is_const<T>::value == true if and only if T has const-qualification.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_const_CONFORMANCE 1    // is_const is conforming.

	template <typename T> struct is_const_value                    : public eastl::false_type{};
	template <typename T> struct is_const_value<const T*>          : public eastl::true_type{};
	template <typename T> struct is_const_value<const volatile T*> : public eastl::true_type{};

	template <typename T> struct is_const : public eastl::is_const_value<T*>{};
	template <typename T> struct is_const<T&> : public eastl::false_type{}; // Note here that T is const, not the reference to T. So is_const is false. See section 8.3.2p1 of the C++ standard.

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_const_v = is_const<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_volatile
	//
	// is_volatile<T>::value == true  if and only if T has volatile-qualification.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_volatile_CONFORMANCE 1    // is_volatile is conforming.

	template <typename T> struct is_volatile_value                    : public eastl::false_type{};
	template <typename T> struct is_volatile_value<volatile T*>       : public eastl::true_type{};
	template <typename T> struct is_volatile_value<const volatile T*> : public eastl::true_type{};

	template <typename T> struct is_volatile : public eastl::is_volatile_value<T*>{};
	template <typename T> struct is_volatile<T&> : public eastl::false_type{}; // Note here that T is volatile, not the reference to T. So is_const is false. See section 8.3.2p1 of the C++ standard.

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_volatile_v = is_volatile<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_reference
	//
	// is_reference<T>::value == true if and only if T is a reference type (l-value reference or r-value reference).
	// This category includes reference to function types.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_reference_CONFORMANCE 1    // is_reference is conforming; doesn't make mistakes.

	template <typename T> struct is_reference      : public eastl::false_type{};
	template <typename T> struct is_reference<T&>  : public eastl::true_type{};
	template <typename T> struct is_reference<T&&> : public eastl::true_type{};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_reference_v = is_reference<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_function
	//
	// is_function<T>::value == true  if and only if T is a function type.
	// A function type here does not include a member function type.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_function_CONFORMANCE 1    // is_function is conforming.

	template <typename>
	struct is_function
		: public eastl::false_type {};

	#if EA_PLATFORM_PTR_SIZE == 4 && defined(EA_PLATFORM_MICROSOFT) && defined(_MSC_EXTENSIONS)
		// __cdecl specialization
		template <typename ReturnValue, typename... ArgPack>
		struct is_function<ReturnValue __cdecl (ArgPack...)>
			: public eastl::true_type {};

		template <typename ReturnValue, typename... ArgPack>
		struct is_function<ReturnValue __cdecl (ArgPack..., ...)>    // The second ellipsis handles the case of a function that takes ellipsis, like printf.
			: public eastl::true_type {};

		// __stdcall specialization
		template <typename ReturnValue, typename... ArgPack>
		struct is_function<ReturnValue __stdcall (ArgPack...)>
			: public eastl::true_type {};

		// When functions use a variable number of arguments, it is the caller that cleans the stack (cf. cdecl).
		//
		// template <typename ReturnValue, typename... ArgPack>
		// struct is_function<ReturnValue __stdcall (ArgPack..., ...)>    // The second ellipsis handles the case of a function that takes ellipsis, like printf.
		//     : public eastl::true_type {};
	#else
		template <typename ReturnValue, typename... ArgPack>
		struct is_function<ReturnValue (ArgPack...)>
			: public eastl::true_type {};

		template <typename ReturnValue, typename... ArgPack>
		struct is_function<ReturnValue (ArgPack..., ...)>    // The second ellipsis handles the case of a function that takes ellipsis, like printf.
			: public eastl::true_type {};
	#endif

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_function_v = is_function<T>::value;
	#endif


	// The following remove utilities are defined here instead of in the headers
	// below because they are core utilits that many other type traits need.

	///////////////////////////////////////////////////////////////////////
	// remove_const
	//
	// Remove const from a type.
	//
	// The remove_const transformation trait removes top-level const
	// qualification (if any) from the type to which it is applied. For a
	// given type T, remove_const<T const>::type is equivalent to the type T.
	// For example, remove_const<char*>::type is equivalent to char* while
	// remove_const<const char*>::type is equivalent to const char*.
	// In the latter case, the const qualifier modifies char, not *, and is
	// therefore not at the top level.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_remove_const_CONFORMANCE 1    // remove_const is conforming.

	template <typename T>           struct remove_const             { typedef T type;    };
	template <typename T>           struct remove_const<const T>    { typedef T type;    };
	template <typename T>           struct remove_const<const T[]>  { typedef T type[];  };
	template <typename T, size_t N> struct remove_const<const T[N]> { typedef T type[N]; };

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		using remove_const_t = typename remove_const<T>::type;
	#endif

	///////////////////////////////////////////////////////////////////////
	// remove_volatile
	//
	// Remove volatile from a type.
	//
	// The remove_volatile transformation trait removes top-level volatile
	// qualification (if any) from the type to which it is applied.
	// For a given type T, the type remove_volatile <T volatile>::T is equivalent
	// to the type T. For example, remove_volatile <char* volatile>::type is
	// equivalent to char* while remove_volatile <volatile char*>::type is
	// equivalent to volatile char*. In the latter case, the volatile qualifier
	// modifies char, not *, and is therefore not at the top level.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_remove_volatile_CONFORMANCE 1    // remove_volatile is conforming.

	template <typename T>           struct remove_volatile                { typedef T type;    };
	template <typename T>           struct remove_volatile<volatile T>    { typedef T type;    };
	template <typename T>           struct remove_volatile<volatile T[]>  { typedef T type[];  };
	template <typename T, size_t N> struct remove_volatile<volatile T[N]> { typedef T type[N]; };

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		using remove_volatile_t = typename remove_volatile<T>::type;
	#endif


	///////////////////////////////////////////////////////////////////////
	// remove_cv
	//
	// Remove const and volatile from a type.
	//
	// The remove_cv transformation trait removes top-level const and/or volatile
	// qualification (if any) from the type to which it is applied. For a given type T,
	// remove_cv<T const volatile>::type is equivalent to T. For example,
	// remove_cv<char* volatile>::type is equivalent to char*, while remove_cv<const char*>::type
	// is equivalent to const char*. In the latter case, the const qualifier modifies
	// char, not *, and is therefore not at the top level.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_remove_cv_CONFORMANCE 1    // remove_cv is conforming.

	template <typename T>
	struct remove_cv { typedef typename eastl::remove_volatile<typename eastl::remove_const<T>::type>::type type; };

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		using remove_cv_t = typename remove_cv<T>::type;
	#endif



	///////////////////////////////////////////////////////////////////////
	// add_reference
	//
	// Add reference to a type.
	//
	// The add_reference transformation trait adds a level of indirection
	// by reference to the type to which it is applied. For a given type T,
	// add_reference<T>::type is equivalent to T& if is_lvalue_reference<T>::value == false,
	// and T otherwise.
	//
	// Note: due to the reference collapsing rules, if you supply an r-value reference such as T&&, it will collapse to T&. 
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_add_reference_CONFORMANCE 1    // add_reference is conforming.

	template <typename T> struct add_reference_impl      { typedef T&   type; };
	template <typename T> struct add_reference_impl<T&>  { typedef T&   type; };
	template <>           struct add_reference_impl<void>{ typedef void type; };
	#if defined(_MSC_VER) && (_MSC_VER <= 1600) // VS2010 and earlier mistakenly report: "cannot add a reference to a zero-sized array." Actually they are allowed, but there's nothing we can do about it under VS2010 and earlier.
	template <typename T> struct add_reference_impl<T[0]>{ typedef T    type; };
	#endif

	template <typename T> struct add_reference { typedef typename add_reference_impl<T>::type type; };



	///////////////////////////////////////////////////////////////////////
	// remove_reference
	//
	// Remove reference from a type.
	//
	// The remove_reference transformation trait removes top-level of
	// indirection by reference (if any) from the type to which it is applied.
	// For a given type T, remove_reference<T&>::type is equivalent to T.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_remove_reference_CONFORMANCE 1

	template <typename T> struct remove_reference     { typedef T type; };
	template <typename T> struct remove_reference<T&> { typedef T type; };
	template <typename T> struct remove_reference<T&&>{ typedef T type; };

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		using remove_reference_t = typename remove_reference<T>::type;
	#endif


	///////////////////////////////////////////////////////////////////////
	// remove_cvref
	//
	// Remove const and volatile from a reference type.
	//
	// The remove_cvref transformation trait removes top-level const and/or volatile
	// qualification (if any) from the reference type to which it is applied. For a given type T&,
	// remove_cvref<T& const volatile>::type is equivalent to T. For example,
	// remove_cv<int& volatile>::type is equivalent to int.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_remove_cvref_CONFORMANCE 1    // remove_cvref is conforming.

	template <typename T>
	struct remove_cvref { typedef typename eastl::remove_volatile<typename eastl::remove_const<typename eastl::remove_reference<T>::type>::type>::type type; };

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		using remove_cvref_t = typename remove_cvref<T>::type;
	#endif


	///////////////////////////////////////////////////////////////////////
	// add_lvalue_reference
	//
	// C++11 Standard, section 20.9.7.2
	// If T names an object or function type then the member typedef type
	// shall name T&; otherwise, if T names a type 'rvalue reference to T1' then
	// the member typedef type shall name T1&; otherwise, type shall name T.
	//
	// Rules (8.3.2 p6):
	//      void + &  -> void
	//      T    + &  -> T&
	//      T&   + &  -> T&
	//      T&&  + &  -> T&
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_add_lvalue_reference_CONFORMANCE 1    // add_lvalue_reference is conforming.

	template <typename T> struct add_lvalue_reference                      { typedef T& type;                  };   // If T is an && type then T&& & will be equivalent to T&.
	template <typename T> struct add_lvalue_reference<T&>                  { typedef T& type;                  };   // This shouldn't be required for modern compilers, as they recognize that a reference to a reference is still a reference.
	template <>           struct add_lvalue_reference<void>                { typedef void type;                };
	template <>           struct add_lvalue_reference<const void>          { typedef const void type;          };
	template <>           struct add_lvalue_reference<volatile void>       { typedef volatile void type;       };
	template <>           struct add_lvalue_reference<const volatile void> { typedef const volatile void type; };

	#if defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		// To do: define macro.
	#else
		template <typename T>
		using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
	#endif



	///////////////////////////////////////////////////////////////////////
	// add_rvalue_reference
	//
	// C++11 Standard, section 20.9.7.2
	// If T names an object or function type then the member typedef type
	// shall name T&&; otherwise, type shall name T. [ Note: This rule reflects
	// the semantics of reference collapsing (8.3.2). For example, when a type T
	// names a type T1&, the type add_rvalue_reference<T>::type is not an
	// rvalue reference. end note ]
	//
	// Rules (8.3.2 p6):
	//      void + &&  -> void
	//      T    + &&  -> T&&
	//      T&   + &&  -> T&
	//      T&&  + &&  -> T&&
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_add_rvalue_reference_CONFORMANCE 1

	template <typename T> struct add_rvalue_reference                      { typedef T&& type;                 }; // Dinkumware has this as { typedef typename eastl::remove_reference<T>::type&& type; }, but that doesn't seem right to me.
	template <typename T> struct add_rvalue_reference<T&>                  { typedef T& type;                  }; // The Standard section 20.7.9.2 specifies that we do this, though it seems like the compiler ought to not require this, as C++11 stipulates that & + && -> &.
	template <>           struct add_rvalue_reference<void>                { typedef void type;                };
	template <>           struct add_rvalue_reference<const void>          { typedef const void type;          };
	template <>           struct add_rvalue_reference<volatile void>       { typedef volatile void type;       };
	template <>           struct add_rvalue_reference<const volatile void> { typedef const volatile void type; };

	#if defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		// To do: define macro.
	#else
		template <typename T>
		using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;
	#endif



	///////////////////////////////////////////////////////////////////////
	// declval
	//
	// declval is normally found in <utility.h> but type traits need it and utility #includes this.
	//
	// Converts any type T to a reference type, making it possible to use member functions in
	// decltype expressions without specifying constructors. It has no use outside decltype expressions.
	// By design there is no implementation, as it's never executed but rather is used only in decltype expressions.
	// The C++11 Standard section 20.2.4 states that we must declare this.
	// http://en.cppreference.com/w/cpp/utility/declval
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_declval_CONFORMANCE 1

	template <typename T>
	typename eastl::add_rvalue_reference<T>::type declval() EA_NOEXCEPT;

	#if !defined(EA_COMPILER_NO_DECLTYPE) && !EASTL_TYPE_TRAIT_declval_CONFORMANCE
		#error decltype is supported by the compiler but declval is not. A lot of our type trait code assumes that if the compiler supports decltype then it supports rvalue references.
	#endif



	///////////////////////////////////////////////////////////////////////
	// static_min / static_max
	//
	// These are primarily useful in templated code for meta programming.
	// Currently we are limited to size_t, as C++ doesn't allow integral
	// template parameters to be generic. We can expand the supported types
	// to include additional integers if needed.
	//
	// These are not in the C++ Standard.
	//
	// Example usage:
	//     Printf("%zu", static_max<3, 7, 1, 5>::value); // prints "7"
	//
	///////////////////////////////////////////////////////////////////////
	#define EASTL_TYPE_TRAIT_static_min_CONFORMANCE 1
	#define EASTL_TYPE_TRAIT_static_max_CONFORMANCE 1

	template <size_t I0, size_t ...in>
	struct static_min;

	template <size_t I0>
	struct static_min<I0>
		{ static const size_t value = I0; };

	template <size_t I0, size_t I1, size_t ...in>
	struct static_min<I0, I1, in...>
		{ static const size_t value = ((I0 <= I1) ? static_min<I0, in...>::value : static_min<I1, in...>::value); };

	template <size_t I0, size_t ...in>
	struct static_max;

	template <size_t I0>
	struct static_max<I0>
		{ static const size_t value = I0; };

	template <size_t I0, size_t I1, size_t ...in>
	struct static_max<I0, I1, in...>
		{ static const size_t value = ((I0 >= I1) ? static_max<I0, in...>::value : static_max<I1, in...>::value); };

	///////////////////////////////////////////////////////////////////////
	/// This enum class is useful for detecting whether a system is little
	/// or big endian. Mixed or middle endian is not modeled here as described
	/// by the C++20 spec.
	///////////////////////////////////////////////////////////////////////
	EA_DISABLE_VC_WARNING(4472) // 'endian' is a native enum: add an access specifier (private/public) to declare a managed enum
	enum class endian
	{
		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			little = 1,
			big = 0,
			native = little
		#else
			little = 0,
			big = 1,
			native = big
		#endif
	};
	EA_RESTORE_VC_WARNING();

} // namespace eastl


// The following files implement the type traits themselves.
#include <EASTL/internal/type_fundamental.h>
#include <EASTL/internal/type_transformations.h>
#include <EASTL/internal/type_properties.h>
#include <EASTL/internal/type_compound.h>
#include <EASTL/internal/type_pod.h>


#endif // Header include guard
