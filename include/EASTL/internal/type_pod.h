/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// Implementation Notes:
// The compiler builtins (Clang terminology) and intrinsics (MSVC terminology) are documented here (as of 2023):
//	Clang:
//		https://clang.llvm.org/docs/LanguageExtensions.html#type-trait-primitives
//	GCC:
//		https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/Type-Traits.html#Type-Traits
//	MSVC (no actual documentation, but searchable here):
//		https://github.com/microsoft/STL/blob/main/stl/inc/type_traits

#ifndef EASTL_INTERNAL_TYPE_POD_H
#define EASTL_INTERNAL_TYPE_POD_H


#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#include <limits.h>
#include <EASTL/type_traits.h>

namespace eastl
{
	///////////////////////////////////////////////////////////////////////
	// is_empty
	// 
	// is_empty<T>::value == true if and only if T is an empty class or struct. 
	// is_empty may only be applied to complete types.
	//
	// is_empty cannot be used with union types until is_union can be made to work.
	///////////////////////////////////////////////////////////////////////
	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(_MSC_VER) || defined(EA_COMPILER_GNUC) || (defined(__clang__) && EA_COMPILER_HAS_FEATURE(is_empty)))
		#define EASTL_TYPE_TRAIT_is_empty_CONFORMANCE 1    // is_empty is conforming. 

		template <typename T> 
		struct is_empty : public integral_constant<bool, __is_empty(T)>{};
	#else
		#define EASTL_TYPE_TRAIT_is_empty_CONFORMANCE 1    // is_empty is fully conforming.

		template <typename T>
		struct is_empty_helper_t1 : public T { char m[64]; };
		struct is_empty_helper_t2            { char m[64]; };

		// The inheritance in empty_helper_t1 will not work with non-class types
		template <typename T, bool is_a_class = false>
		struct is_empty_helper : public eastl::false_type{};

		template <typename T>
		struct is_empty_helper<T, true> : public eastl::integral_constant<bool,
			sizeof(is_empty_helper_t1<T>) == sizeof(is_empty_helper_t2)
		>{};

		template <typename T>
		struct is_empty_helper2
		{
			typedef typename eastl::remove_cv<T>::type _T;
			typedef eastl::is_empty_helper<_T, eastl::is_class<_T>::value> type;
		};

		template <typename T> 
		struct is_empty : public eastl::is_empty_helper2<T>::type {};
	#endif


	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_empty_v = is_empty<T>::value;
    #endif


	///////////////////////////////////////////////////////////////////////
	// is_pod
	// 
	// is_pod<T>::value == true if and only if, for a given type T:
	//    - is_scalar<T>::value == true, or
	//    - T is a class or struct that has no user-defined copy assignment 
	//      operator or destructor, and T has no non-static data members M for 
	//      which is_pod<M>::value == false, and no members of reference type, or
	//    - T is the type of an array of objects E for which is_pod<E>::value == true
	//
	// is_pod may only be applied to complete types.
	///////////////////////////////////////////////////////////////////////

	#if defined(EA_COMPILER_MSVC) && !defined(EA_COMPILER_CLANG_CL)
		#define EASTL_TYPE_TRAIT_is_pod_CONFORMANCE 1    // is_pod is conforming. Actually as of VS2008 it is apparently not fully conforming, as it flags the following as a non-pod: struct Pod{ Pod(){} };

		EA_DISABLE_VC_WARNING(4647)
		template <typename T> // We check for has_trivial_constructor only because the VC++ is_pod does. Is it due to some compiler bug?
		struct is_pod : public eastl::integral_constant<bool, (__has_trivial_constructor(T) && __is_pod(T) && !eastl::is_hat_type<T>::value) || eastl::is_void<T>::value || eastl::is_scalar<T>::value>{};
		EA_RESTORE_VC_WARNING()
	
	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(EA_COMPILER_GNUC) || (defined(__clang__) && EA_COMPILER_HAS_FEATURE(is_pod)))
		#define EASTL_TYPE_TRAIT_is_pod_CONFORMANCE 1    // is_pod is conforming.

		template <typename T> 
		struct is_pod : public eastl::integral_constant<bool, __is_pod(T) || eastl::is_void<T>::value || eastl::is_scalar<T>::value>{};
	#else
		#define EASTL_TYPE_TRAIT_is_pod_CONFORMANCE 0    // is_pod is not conforming. Can return false negatives.

		template <typename T> // There's not much we can do here without some compiler extension.
		struct is_pod : public eastl::integral_constant<bool, eastl::is_void<T>::value || eastl::is_scalar<typename eastl::remove_all_extents<T>::type>::value>{};
	#endif

	template <typename T, size_t N>
	struct is_pod<T[N]> : public is_pod<T>{};

	template <typename T>
	struct is_POD : public is_pod<T>{};  // Backwards compatibility.

	#define EASTL_DECLARE_IS_POD(T, isPod)																									   \
		namespace eastl {																													   \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_pod<T>                : public eastl::integral_constant<bool, isPod>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_pod<const T>          : public eastl::integral_constant<bool, isPod>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_pod<volatile T>       : public eastl::integral_constant<bool, isPod>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_pod<const volatile T> : public eastl::integral_constant<bool, isPod>  { }; \
		}

	// Old style macro, for bacwards compatibility:
	#define EASTL_DECLARE_POD(T) namespace eastl{ template <> struct EASTL_REMOVE_AT_2024_APRIL is_pod<T> : public true_type{}; template <> struct EASTL_REMOVE_AT_2024_APRIL is_pod<const T> : public true_type{}; }

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_pod_v = is_pod<T>::value;
    #endif


	///////////////////////////////////////////////////////////////////////
	// is_standard_layout
	//
	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && ((defined(EA_COMPILER_MSVC) && (_MSC_VER >= 1700)) || (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4006)) || (defined(__clang__) && EA_COMPILER_HAS_FEATURE(is_standard_layout)))
		#define EASTL_TYPE_TRAIT_is_standard_layout_CONFORMANCE 1    // is_standard_layout is conforming.

		template <typename T> 
		struct is_standard_layout : public eastl::integral_constant<bool, __is_standard_layout(T) || eastl::is_void<T>::value || eastl::is_scalar<T>::value>{};
	#else
		#define EASTL_TYPE_TRAIT_is_standard_layout_CONFORMANCE 0    // is_standard_layout is not conforming. Can return false negatives.

		template <typename T> // There's not much we can do here without some compiler extension.
		struct is_standard_layout : public eastl::integral_constant<bool, is_void<T>::value || is_scalar<T>::value>{};
	#endif

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_standard_layout_v = is_standard_layout<T>::value;
    #endif

	#define EASTL_DECLARE_IS_STANDARD_LAYOUT(T, isStandardLayout)                                                    \
		namespace eastl {                                                                                            \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_standard_layout<T>                : public eastl::integral_constant<bool, isStandardLayout>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_standard_layout<const T>          : public eastl::integral_constant<bool, isStandardLayout>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_standard_layout<volatile T>       : public eastl::integral_constant<bool, isStandardLayout>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_standard_layout<const volatile T> : public eastl::integral_constant<bool, isStandardLayout>  { }; \
		}

	// Old style macro, for bacwards compatibility:
	#define EASTL_DECLARE_STANDARD_LAYOUT(T) namespace eastl{ template <> struct EASTL_REMOVE_AT_2024_APRIL is_standard_layout<T> : public true_type{}; template <> struct EASTL_REMOVE_AT_2024_APRIL is_standard_layout<const T> : public true_type{}; }



	///////////////////////////////////////////////////////////////////////
	// has_trivial_constructor
	//
	// Deprecated. Use is_trivially_default_constructible for the std conforming alternative.
	//
	// has_trivial_constructor<T>::value == true if and only if T is a class 
	// or struct that has a trivial constructor. A constructor is trivial if
	//    - it is implicitly defined by the compiler, and
	//    - is_polymorphic<T>::value == false, and
	//    - T has no virtual base classes, and
	//    - for every direct base class of T, has_trivial_constructor<B>::value == true, 
	//      where B is the type of the base class, and
	//    - for every nonstatic data member of T that has class type or array 
	//      of class type, has_trivial_constructor<M>::value == true, 
	//      where M is the type of the data member
	//
	// has_trivial_constructor may only be applied to complete types.
	//
	// Without from the compiler or user, has_trivial_constructor will not 
	// report that a class or struct has a trivial constructor. 
	// The user can use EASTL_DECLARE_TRIVIAL_CONSTRUCTOR to help the compiler.
	//
	// A default constructor for a class X is a constructor of class X that 
	// can be called without an argument.
	///////////////////////////////////////////////////////////////////////

	#if defined(_MSC_VER) && (_MSC_VER >= 1600) && !defined(EA_COMPILER_CLANG_CL) // VS2010+
		#define EASTL_TYPE_TRAIT_has_trivial_constructor_CONFORMANCE 1    // has_trivial_constructor is conforming.

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_constructor : public eastl::integral_constant<bool, (__has_trivial_constructor(T) || eastl::is_pod<T>::value) && !eastl::is_hat_type<T>::value>{};
	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && defined(__clang__) && EASTL_HAS_INTRINSIC(is_trivially_constructible)
		#define EASTL_TYPE_TRAIT_has_trivial_constructor_CONFORMANCE 1    // has_trivial_constructor is conforming.

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_constructor : public eastl::integral_constant<bool, __is_trivially_constructible(T) || eastl::is_pod<T>::value>{};
	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(_MSC_VER) || defined(EA_COMPILER_GNUC) || defined(__clang__))
		#define EASTL_TYPE_TRAIT_has_trivial_constructor_CONFORMANCE 1    // has_trivial_constructor is conforming.

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_constructor : public eastl::integral_constant<bool, __has_trivial_constructor(T) || eastl::is_pod<T>::value>{};
	#else
		#define EASTL_TYPE_TRAIT_has_trivial_constructor_CONFORMANCE 0    // has_trivial_constructor is not fully conforming. Can return false negatives.

		// With current compilers, this is all we can do.
		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_constructor : public eastl::is_pod<T> {};
	#endif

	#define EASTL_DECLARE_HAS_TRIVIAL_CONSTRUCTOR(T, hasTrivialConstructor)                                                     \
		namespace eastl {                                                                                                       \
			template <> struct EASTL_REMOVE_AT_2024_APRIL has_trivial_constructor<T>  : public eastl::integral_constant<bool, hasTrivialConstructor>  { }; \
		}

	// Old style macro, for bacwards compatibility:
	#define EASTL_DECLARE_TRIVIAL_CONSTRUCTOR(T) namespace eastl{ template <> struct EASTL_REMOVE_AT_2024_APRIL has_trivial_constructor<T> : public true_type{}; template <> struct EASTL_REMOVE_AT_2024_APRIL has_trivial_constructor<const T> : public true_type{}; }




	///////////////////////////////////////////////////////////////////////
	// has_trivial_copy
	//
	// Deprecated. Use is_trivially_copy_constructible for the std conforming alternative.
	//
	// has_trivial_copy<T>::value == true if and only if T is a class or 
	// struct that has a trivial copy constructor. A copy constructor is 
	// trivial if
	//   - it is implicitly defined by the compiler, and
	//   - is_polymorphic<T>::value == false, and
	//   - T has no virtual base classes, and
	//   - for every direct base class of T, has_trivial_copy<B>::value == true, 
	//     where B is the type of the base class, and
	//   - for every nonstatic data member of T that has class type or array 
	//     of class type, has_trivial_copy<M>::value == true, where M is the 
	//     type of the data member
	//
	// has_trivial_copy may only be applied to complete types.
	//
	// Another way of looking at this is:
	// A copy constructor for class X is trivial if it is implicitly 
	// declared and if all the following are true:
	//    - Class X has no virtual functions (10.3) and no virtual base classes (10.1).
	//    - Each direct base class of X has a trivial copy constructor.
	//    - For all the nonstatic data members of X that are of class type 
	//      (or array thereof), each such class type has a trivial copy constructor;
	//      otherwise the copy constructor is nontrivial.
	//
	// Without help from the compiler or user, has_trivial_copy will not report 
	// that a class or struct has a trivial copy constructor. The user can 
	// use EASTL_DECLARE_TRIVIAL_COPY to help the compiler.
	///////////////////////////////////////////////////////////////////////

	#if defined(_MSC_VER) && !defined(EA_COMPILER_CLANG_CL)
		#define EASTL_TYPE_TRAIT_has_trivial_copy_CONFORMANCE 1    // has_trivial_copy is conforming.

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_copy : public eastl::integral_constant<bool, (__has_trivial_copy(T) || eastl::is_pod<T>::value) && !eastl::is_volatile<T>::value && !eastl::is_hat_type<T>::value>{};
	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && defined(__clang__) && EASTL_HAS_INTRINSIC(is_trivially_copyable)
		#define EASTL_TYPE_TRAIT_has_trivial_copy_CONFORMANCE 1    // has_trivial_copy is conforming.

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_copy : public eastl::integral_constant<bool, (__is_trivially_copyable(T) || eastl::is_pod<T>::value) && (!eastl::is_volatile<T>::value && !eastl::is_reference<T>::value)>{};
	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(EA_COMPILER_GNUC) || defined(__clang__))
		#define EASTL_TYPE_TRAIT_has_trivial_copy_CONFORMANCE 1    // has_trivial_copy is conforming.

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_copy : public eastl::integral_constant<bool, (__has_trivial_copy(T) || eastl::is_pod<T>::value) && (!eastl::is_volatile<T>::value && !eastl::is_reference<T>::value)>{};
	#else
		#define EASTL_TYPE_TRAIT_has_trivial_copy_CONFORMANCE 0   // has_trivial_copy is not fully conforming. Can return false negatives.

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_copy : public eastl::integral_constant<bool, eastl::is_pod<T>::value && !eastl::is_volatile<T>::value>{};
	#endif

	#define EASTL_DECLARE_HAS_TRIVIAL_COPY(T, hasTrivialCopy)                                                    \
		namespace eastl {                                                                                        \
			template <> struct EASTL_REMOVE_AT_2024_APRIL has_trivial_copy<T> : public eastl::integral_constant<bool, hasTrivialCopy>  { }; \
		}

	// Old style macro, for bacwards compatibility:
	#define EASTL_DECLARE_TRIVIAL_COPY(T) namespace eastl{ template <> struct EASTL_REMOVE_AT_2024_APRIL has_trivial_copy<T> : public true_type{}; template <> struct EASTL_REMOVE_AT_2024_APRIL has_trivial_copy<const T> : public true_type{}; }




	///////////////////////////////////////////////////////////////////////
	// has_trivial_assign
	//
	// Deprecated. Use is_trivially_assignable<T, U>, is_trivially_move_assignable<T> or is_trivially_copy_assignable<T> as a std conforming alternative.
	//
	// has_trivial_assign<T>::value == true if and only if T is a class or 
	// struct that has a trivial copy assignment operator. A copy assignment 
	// operator is trivial if:
	//    - it is implicitly defined by the compiler, and
	//    - is_polymorphic<T>::value == false, and
	//    - T has no virtual base classes, and
	//    - for every direct base class of T, has_trivial_assign<B>::value == true, 
	//      where B is the type of the base class, and
	//    - for every nonstatic data member of T that has class type or array 
	//      of class type, has_trivial_assign<M>::value == true, where M is 
	//      the type of the data member.
	//
	// has_trivial_assign may only be applied to complete types.
	//
	// Without  from the compiler or user, has_trivial_assign will not 
	// report that a class or struct has trivial assignment. The user 
	// can use EASTL_DECLARE_TRIVIAL_ASSIGN to help the compiler.
	///////////////////////////////////////////////////////////////////////

	#if defined(_MSC_VER) && (_MSC_VER >= 1600) && !defined(EA_COMPILER_CLANG_CL)
		#define EASTL_TYPE_TRAIT_has_trivial_assign_CONFORMANCE 1    // has_trivial_assign is conforming.

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_assign : public integral_constant<bool, (__has_trivial_assign(T) || eastl::is_pod<T>::value) && !eastl::is_const<T>::value && !eastl::is_volatile<T>::value && !eastl::is_hat_type<T>::value>{};
	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && defined(__clang__) && EASTL_HAS_INTRINSIC(is_trivially_assignable)
		#define EASTL_TYPE_TRAIT_has_trivial_assign_CONFORMANCE 1    // has_trivial_assign is conforming.

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_assign : public integral_constant<bool, (__is_trivially_assignable(typename add_lvalue_reference<T>::type, typename add_lvalue_reference<const T>::type) || eastl::is_pod<T>::value) && !eastl::is_const<T>::value && !eastl::is_volatile<T>::value>{};
	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(_MSC_VER) || defined(EA_COMPILER_GNUC) || defined(__clang__))
		#define EASTL_TYPE_TRAIT_has_trivial_assign_CONFORMANCE 1    // has_trivial_assign is conforming.

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_assign : public integral_constant<bool, (__has_trivial_assign(T) || eastl::is_pod<T>::value) && !eastl::is_const<T>::value && !eastl::is_volatile<T>::value>{};
	#else
		#define EASTL_TYPE_TRAIT_has_trivial_assign_CONFORMANCE 0  // is_pod is not fully conforming. Can return false negatives.

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_assign : public integral_constant<bool,
			is_pod<T>::value && !is_const<T>::value && !is_volatile<T>::value
		>{};
	#endif

	#define EASTL_DECLARE_HAS_TRIVIAL_ASSIGN(T, hasTrivialAssign)                                                    \
		namespace eastl {                                                                                            \
			template <> struct EASTL_REMOVE_AT_2024_APRIL has_trivial_assign<T> : public eastl::integral_constant<bool, hasTrivialAssign>  { }; \
		}

	// Old style macro, for bacwards compatibility:
	#define EASTL_DECLARE_TRIVIAL_ASSIGN(T) namespace eastl{ template <> struct EASTL_REMOVE_AT_2024_APRIL has_trivial_assign<T> : public true_type{}; template <> struct EASTL_REMOVE_AT_2024_APRIL has_trivial_assign<const T> : public true_type{}; }




	///////////////////////////////////////////////////////////////////////
	// has_trivial_destructor
	//
	// Deprecated. Use is_trivially_destructible for the std conforming alternative.
	//
	// has_trivial_destructor<T>::value == true if and only if T is a class 
	// or struct that has a trivial destructor. A destructor is trivial if
	//    - it is implicitly defined by the compiler, and
	//    - for every direct base class of T, has_trivial_destructor<B>::value == true, 
	//      where B is the type of the base class, and
	//    - for every nonstatic data member of T that has class type or 
	//      array of class type, has_trivial_destructor<M>::value == true, 
	//      where M is the type of the data member
	//
	// has_trivial_destructor may only be applied to complete types.
	//
	// Without from the compiler or user, has_trivial_destructor will not 
	// report that a class or struct has a trivial destructor. 
	// The user can use EASTL_DECLARE_TRIVIAL_DESTRUCTOR to help the compiler.
	///////////////////////////////////////////////////////////////////////
	
	EASTL_INTERNAL_DISABLE_DEPRECATED()
	#if defined(_MSC_VER) && (_MSC_VER >= 1600) && !defined(EA_COMPILER_CLANG_CL)
		#define EASTL_TYPE_TRAIT_has_trivial_destructor_CONFORMANCE 1    // has_trivial_destructor is conforming.

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_destructor : public eastl::integral_constant<bool, (__has_trivial_destructor(T) || eastl::is_pod<T>::value) && !eastl::is_hat_type<T>::value>{};
	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && defined(__clang__) && EASTL_HAS_INTRINSIC(is_trivially_destructible)
		#define EASTL_TYPE_TRAIT_has_trivial_destructor_CONFORMANCE 1    // has_trivial_destructor is conforming.

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_destructor : public eastl::integral_constant<bool, __is_trivially_destructible(T) || eastl::is_pod<T>::value>{};
	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(_MSC_VER) || defined(EA_COMPILER_GNUC) || defined(__clang__))
		#define EASTL_TYPE_TRAIT_has_trivial_destructor_CONFORMANCE 1    // has_trivial_destructor is conforming.

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_destructor : public eastl::integral_constant<bool, __has_trivial_destructor(T) || eastl::is_pod<T>::value>{};
	#else
		#define EASTL_TYPE_TRAIT_has_trivial_destructor_CONFORMANCE 0  // is_pod is not fully conforming. Can return false negatives.

		// With current compilers, this is all we can do.
		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_trivial_destructor : public eastl::is_pod<T>{};
	#endif
	EASTL_INTERNAL_RESTORE_DEPRECATED()

	#define EASTL_DECLARE_HAS_TRIVIAL_DESTRUCTOR(T, hasTrivialDestructor)                                                    \
		namespace eastl {                                                                                                    \
			template <> struct EASTL_REMOVE_AT_2024_APRIL has_trivial_destructor<T> : public eastl::integral_constant<bool, hasTrivialDestructor>  { }; \
		}

	// Old style macro, for bacwards compatibility:
	#define EASTL_DECLARE_TRIVIAL_DESTRUCTOR(T) namespace eastl{ template <> struct EASTL_REMOVE_AT_2024_APRIL has_trivial_destructor<T> : public true_type{}; template <> struct EASTL_REMOVE_AT_2024_APRIL has_trivial_destructor<const T> : public true_type{}; }

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
	// Note: some compilers (notably GCC) trigger deprecation warnings in template variable
	// declarations even if the variable is not insantiated here, so turn the warning off
	// here. If this varialbe is used, the warning will still trigger in the user code, this
	// just disables the warning in this declaration.
EASTL_INTERNAL_DISABLE_DEPRECATED()
		template <class T>
		EASTL_REMOVE_AT_2024_APRIL EA_CONSTEXPR bool has_trivial_destructor_v = has_trivial_destructor<T>::value;
EASTL_INTERNAL_RESTORE_DEPRECATED()
    #endif


	///////////////////////////////////////////////////////////////////////
	// has_trivial_relocate
	//
	// This is an EA extension to the type traits standard.
	// This trait is deprecated under conforming C++11 compilers, as C++11 
	// move functionality supercedes this functionality and we want to 
	// migrate away from it in the future.
	//
	// Deprecated. Use is_trivially_copyable for the std conforming alternative.
	//
	// A trivially relocatable object is one that can be safely memmove'd 
	// to uninitialized memory. construction, assignment, and destruction 
	// properties are not addressed by this trait. A type that has the 
	// is_fundamental trait would always have the has_trivial_relocate trait. 
	// A type that has the has_trivial_constructor, has_trivial_copy or 
	// has_trivial_assign traits would usally have the has_trivial_relocate 
	// trait, but this is not strictly guaranteed.
	//
	// The user can use EASTL_DECLARE_TRIVIAL_RELOCATE to help the compiler.
	// 
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_has_trivial_relocate_CONFORMANCE 0  // is_pod is not fully conforming. Can return false negatives.

	EASTL_INTERNAL_DISABLE_DEPRECATED()
	template <typename T>
	struct EASTL_REMOVE_AT_2024_APRIL has_trivial_relocate : public eastl::bool_constant<eastl::is_pod_v<T> && !eastl::is_volatile_v<T>> {};
	EASTL_INTERNAL_RESTORE_DEPRECATED()

    #define EASTL_DECLARE_TRIVIAL_RELOCATE(T) namespace eastl{ template <> struct EASTL_REMOVE_AT_2024_APRIL has_trivial_relocate<T> : public true_type{}; template <> struct EASTL_REMOVE_AT_2024_APRIL has_trivial_relocate<const T> : public true_type{}; }




	///////////////////////////////////////////////////////////////////////
	// has_nothrow_constructor
	//
	// Deprecated. Use is_nothrow_constructible<T>::value for the std conforming alternative.
	//
	// has_nothrow_constructor<T>::value == true if and only if T is a 
	// class or struct whose default constructor has an empty throw specification.
	// 
	// has_nothrow_constructor may only be applied to complete types.
	//
	///////////////////////////////////////////////////////////////////////

	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && defined(__clang__) && EASTL_HAS_INTRINSIC(is_nothrow_constructible)
		#define EASTL_TYPE_TRAIT_has_nothrow_constructor_CONFORMANCE 1

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_nothrow_constructor
			: public eastl::integral_constant<bool, __is_nothrow_constructible(T)>{};

	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(EA_COMPILER_GNUC) || defined(__clang__))
		#define EASTL_TYPE_TRAIT_has_nothrow_constructor_CONFORMANCE 1

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_nothrow_constructor
			: public eastl::integral_constant<bool, __has_nothrow_constructor(T)>{};

	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && defined(_MSC_VER)
		// Microsoft's implementation of __has_nothrow_constructor is crippled and returns true only if T is a class that has an explicit constructor.
		// "Returns true if the default constructor has an empty exception specification."
		#define EASTL_TYPE_TRAIT_has_nothrow_constructor_CONFORMANCE 0

		template <typename T> // This is mistakenly returning true for an unbounded array of scalar type. 
		struct EASTL_REMOVE_AT_2024_APRIL has_nothrow_constructor : public eastl::integral_constant<bool, __has_nothrow_constructor(T) || eastl::is_scalar<typename eastl::remove_all_extents<T>::type>::value || eastl::is_reference<T>::value>{};

	#else
		#define EASTL_TYPE_TRAIT_has_nothrow_constructor_CONFORMANCE 0  // has_nothrow_constructor is not fully conforming. Can return false negatives.

		template <typename T>
		struct EASTL_REMOVE_AT_2024_APRIL has_nothrow_constructor // To do: Improve this to include other types that can work.
			{ static const bool value = eastl::is_scalar<typename eastl::remove_all_extents<T>::type>::value || eastl::is_reference<T>::value; };
	#endif

	#define EASTL_DECLARE_HAS_NOTHROW_CONSTRUCTOR(T, hasNothrowConstructor)                                                    \
		namespace eastl {                                                                                                      \
			template <> struct EASTL_REMOVE_AT_2024_APRIL has_nothrow_constructor<T> : public eastl::integral_constant<bool, hasNothrowConstructor>  { }; \
		}



	///////////////////////////////////////////////////////////////////////
	// has_nothrow_copy
	//
	// Deprecated. Use is_nothrow_copy_constructible for the std conforming alternative.
	//
	// has_nothrow_copy<T>::value == true if and only if T is a class or 
	// struct whose copy constructor has an empty throw specification.
	//
	// has_nothrow_copy may only be applied to complete types.
	//
	///////////////////////////////////////////////////////////////////////

	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && defined(__clang__) && EASTL_HAS_INTRINSIC(is_nothrow_constructible)
		#define EASTL_TYPE_TRAIT_has_nothrow_copy_CONFORMANCE 1

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_nothrow_copy : public eastl::integral_constant<bool, __is_nothrow_constructible(T, typename eastl::add_lvalue_reference<const T>::type)>{};

	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(EA_COMPILER_GNUC) || defined(__clang__))
		#define EASTL_TYPE_TRAIT_has_nothrow_copy_CONFORMANCE 1

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_nothrow_copy : public eastl::integral_constant<bool, __has_nothrow_copy(T)>{};

	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && defined(_MSC_VER)
		// Microsoft's implementation of __has_nothrow_copy is crippled and returns true only if T is a class that has a copy constructor.
		// "Returns true if the copy constructor has an empty exception specification."
		#define EASTL_TYPE_TRAIT_has_nothrow_copy_CONFORMANCE 0

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_nothrow_copy : public eastl::integral_constant<bool, __has_nothrow_copy(T) || eastl::is_scalar<typename eastl::remove_all_extents<T>::type>::value || eastl::is_reference<T>::value>{};

	#else
		#define EASTL_TYPE_TRAIT_has_nothrow_copy_CONFORMANCE 0  // has_nothrow_copy is not fully conforming. Can return false negatives.

		template <typename T>
		struct EASTL_REMOVE_AT_2024_APRIL has_nothrow_copy // To do: Improve this to include other types that can work.
			{ static const bool value = eastl::is_scalar<typename eastl::remove_all_extents<T>::type>::value || eastl::is_reference<T>::value; };
	#endif

	#define EASTL_DECLARE_HAS_NOTHROW_COPY(T, hasNothrowCopy)                                                    \
		namespace eastl {                                                                                        \
			template <> struct EASTL_REMOVE_AT_2024_APRIL has_nothrow_copy<T> : public eastl::integral_constant<bool, hasNothrowCopy>  { }; \
		}



	///////////////////////////////////////////////////////////////////////
	// has_nothrow_assign
	//
	// Deprecated. Use is_nothrow_assignable<T, const T&>::value for the std conforming alternative.
	//
	// has_nothrow_assign<T>::value == true if and only if T is a class or 
	// struct whose copy assignment operator has an empty throw specification.
	// 
	// has_nothrow_assign may only be applied to complete types.
	//
	///////////////////////////////////////////////////////////////////////

	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && defined(__clang__) && EASTL_HAS_INTRINSIC(is_nothrow_assignable)
		#define EASTL_TYPE_TRAIT_has_nothrow_assign_CONFORMANCE 1

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_nothrow_assign : public eastl::integral_constant<bool, __is_nothrow_assignable(typename eastl::add_lvalue_reference<T>::type, typename eastl::add_lvalue_reference<const T>::type)>{};

	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(EA_COMPILER_GNUC) || defined(__clang__))
		#define EASTL_TYPE_TRAIT_has_nothrow_assign_CONFORMANCE 1

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL has_nothrow_assign : public eastl::integral_constant<bool, __has_nothrow_assign(T)>{};

	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && defined(_MSC_VER)
		// Microsoft's implementation of __has_nothrow_assign is crippled and returns true only if T is a class that has an assignment operator.
		// "Returns true if a copy assignment operator has an empty exception specification."
		#define EASTL_TYPE_TRAIT_has_nothrow_assign_CONFORMANCE 0

		template <typename T> // This is mistakenly returning true for an unbounded array of scalar type. 
		struct EASTL_REMOVE_AT_2024_APRIL has_nothrow_assign : public eastl::integral_constant<bool, __has_nothrow_assign(T) || eastl::is_scalar<typename eastl::remove_all_extents<T>::type>::value || eastl::is_reference<T>::value>{};
	#else
		#define EASTL_TYPE_TRAIT_has_nothrow_assign_CONFORMANCE 0  // has_nothrow_assign is not fully conforming. Can return false negatives.

		template <typename T>
		struct EASTL_REMOVE_AT_2024_APRIL has_nothrow_assign // To do: Improve this to include other types that can work.
			{ static const bool value = eastl::is_scalar<typename eastl::remove_all_extents<T>::type>::value || eastl::is_reference<T>::value; } ;
	#endif

	#define EASTL_DECLARE_HAS_NOTHROW_ASSIGN(T, hasNothrowAssign)                                                    \
		namespace eastl {                                                                                            \
			template <> struct EASTL_REMOVE_AT_2024_APRIL has_nothrow_assign<T> : public eastl::integral_constant<bool, hasNothrowAssign>  { }; \
		}



	///////////////////////////////////////////////////////////////////////
	// has_virtual_destructor
	//
	// has_virtual_destructor<T>::value == true if and only if T is a class 
	// or struct with a virtual destructor.
	//
	// has_virtual_destructor may only be applied to complete types.
	//
	///////////////////////////////////////////////////////////////////////

	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(_MSC_VER) || defined(EA_COMPILER_GNUC) || defined(__clang__))
		#define EASTL_TYPE_TRAIT_has_virtual_destructor_CONFORMANCE 1

		template <typename T> 
		struct has_virtual_destructor : public eastl::integral_constant<bool, __has_virtual_destructor(T)>{};
	#else
		#define EASTL_TYPE_TRAIT_has_virtual_destructor_CONFORMANCE 0   // has_virtual_destructor is not fully conforming. Can return false negatives.

		template <typename T>
		struct has_virtual_destructor : public eastl::false_type{};
	#endif

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool has_virtual_destructor_v = has_virtual_destructor<T>::value;
    #endif

	#define EASTL_DECLARE_HAS_VIRTUAL_DESTRUCTOR(T, hasVirtualDestructor)                                                                   \
		namespace eastl {                                                                                                                   \
			template <> struct EASTL_REMOVE_AT_2024_APRIL has_virtual_destructor<T>                : public eastl::integral_constant<bool, hasVirtualDestructor>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL has_virtual_destructor<const T>          : public eastl::integral_constant<bool, hasVirtualDestructor>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL has_virtual_destructor<volatile T>       : public eastl::integral_constant<bool, hasVirtualDestructor>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL has_virtual_destructor<const volatile T> : public eastl::integral_constant<bool, hasVirtualDestructor>  { }; \
		}


	///////////////////////////////////////////////////////////////////////
	// is_literal_type
	//
	// Deprecated in C++17. Removed from the standard in C++20.
	// 
	// From https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0174r2.html:
	// The is_literal type trait offers negligible value to generic code, as what is really needed is the ability to know
	// that a specific construction would produce constant initialization. The core term of a literal type having at least
	// one constexpr constructor is too weak to be used meaningfully.
	// 
	// See the C++11 Standard, section 2.9,p10.
	// A type is a literal type if it is:
	//     - a scalar type; or
	//     - a reference type referring to a literal type; or
	//     - an array of literal type; or
	//     - a class type (Clause 9) that has all of the following properties:
	//         - it has a trivial destructor,
	//         - every constructor call and full-expression in the brace-or-equal-initializer s for non-static data members (if any) is a constant expression (5.19),
	//         - it is an aggregate type (8.5.1) or has at least one constexpr constructor or constructor template that is not a copy or move constructor, and
	//         - all of its non-static data members and base classes are of literal types.
	//
	///////////////////////////////////////////////////////////////////////

	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(__clang__) && EA_COMPILER_HAS_FEATURE(is_literal))
		#define EASTL_TYPE_TRAIT_is_literal_type_CONFORMANCE 1

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL is_literal_type : public eastl::integral_constant<bool, __is_literal(T)>{};

	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && ((defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4006)) || (defined(_MSC_VER) && (_MSC_VER >= 1700))) // VS2012+
		#if defined(EA_COMPILER_GNUC) && (!defined(EA_COMPILER_CPP11_ENABLED) || (EA_COMPILER_VERSION < 4007))
			#define EASTL_TYPE_TRAIT_is_literal_type_CONFORMANCE 0 // It seems that in this case GCC supports the compiler intrinsic but reports it as false when it's true.
		#else
			#define EASTL_TYPE_TRAIT_is_literal_type_CONFORMANCE 1
		#endif

		template <typename T> 
		struct EASTL_REMOVE_AT_2024_APRIL is_literal_type : public eastl::integral_constant<bool, __is_literal_type(T)>{};

	#else
		#define EASTL_TYPE_TRAIT_is_literal_type_CONFORMANCE 0

		// It's not clear if this trait can be fully implemented without explicit compiler support.
		// For now we assume that it can't be but implement something that gets it right at least 
		// some of the time. Recall that partial positives and false negatives are OK (though not ideal),
		// while false positives are not OK for us to generate.

		template <typename T> // This is not a complete implementation and will be true for only some literal types (the basic ones).
		struct EASTL_REMOVE_AT_2024_APRIL is_literal_type : public eastl::integral_constant<bool, eastl::is_scalar<typename eastl::remove_reference<typename eastl::remove_all_extents<T>::type>::type>::value>{};
	#endif

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		EASTL_INTERNAL_DISABLE_DEPRECATED() // '*': was declared deprecated
		template <class T>
		EASTL_REMOVE_AT_2024_APRIL EA_CONSTEXPR bool is_literal_type_v = is_literal_type<T>::value;
		EASTL_INTERNAL_RESTORE_DEPRECATED()
    #endif



	///////////////////////////////////////////////////////////////////////
	// is_abstract
	//
	// is_abstract<T>::value == true if and only if T is a class or struct 
	// that has at least one pure virtual function. is_abstract may only 
	// be applied to complete types.
	//
	///////////////////////////////////////////////////////////////////////

	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(_MSC_VER) || defined(EA_COMPILER_GNUC) || (defined(__clang__) && EA_COMPILER_HAS_FEATURE(is_abstract)))
		#define EASTL_TYPE_TRAIT_is_abstract_CONFORMANCE 1    // is_abstract is conforming.

		template <typename T> 
		struct is_abstract : public integral_constant<bool, __is_abstract(T)>{};
	#else
		#define EASTL_TYPE_TRAIT_is_abstract_CONFORMANCE 0

		template<typename T, bool = !eastl::is_object<T>::value>
		class is_abstract_helper
		{
			template<typename>
			static eastl::yes_type test(...);

			template<typename T1>
			static eastl::no_type test(T1(*)[1]);  // The following: 'typedef SomeAbstractClass (*SomeFunctionType)[1];' is invalid (can't have an array of abstract types) and thus doesn't choose this path.
	
		public:
			static const bool value = (sizeof(test<T>(NULL)) == sizeof(eastl::yes_type));
		};
  
		template <typename T>
		struct is_abstract_helper<T, true>
			{ static const bool value = false; };

		template <typename T>
		struct is_abstract
			: public integral_constant<bool, is_abstract_helper<T>::value> { };

	#endif

	#define EASTL_DECLARE_IS_ABSTRACT(T, isAbstract)                                                                   \
		namespace eastl {                                                                                              \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_abstract<T>                : public eastl::integral_constant<bool, isAbstract>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_abstract<const T>          : public eastl::integral_constant<bool, isAbstract>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_abstract<volatile T>       : public eastl::integral_constant<bool, isAbstract>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_abstract<const volatile T> : public eastl::integral_constant<bool, isAbstract>  { }; \
		}

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_abstract_v = is_abstract<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_trivially_copyable
	//
	// T is a trivially copyable type (3.9) T shall be a complete type,
	// (possibly cv-qualified) void, or an array of unknown bound.
	//
	// 3.9,p3: For any trivially copyable type T, if two pointers to T 
	// point to distinct T objects obj1 and obj2, where neither obj1 nor 
	// obj2 is a base-class subobject, if the underlying bytes making 
	// up obj1 are copied into obj2, obj2 shall subsequently hold the 
	// same value as obj1. In other words, you can memcpy/memmove it.
	///////////////////////////////////////////////////////////////////////
	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && ((defined(_MSC_VER) && (_MSC_VER >= 1700)) || (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 5003)) || (defined(__clang__) && EA_COMPILER_HAS_FEATURE(is_trivially_copyable)))
		#define EASTL_TYPE_TRAIT_is_trivially_copyable_CONFORMANCE 1

		// https://connect.microsoft.com/VisualStudio/feedback/details/808827/c-std-is-trivially-copyable-produces-wrong-result-for-arrays
		// 
		// From Microsoft:
		//   We're working on fixing this. When overhauling <type_traits> in VC 2013, I incorrectly believed that is_trivially_copyable was a synonym
		//   for is_trivially_copy_constructible. I've asked the compiler team to provide a compiler hook with 100% accurate answers. (Currently, the
		//   compiler hook has incorrect answers for volatile scalars, volatile data members, and various scenarios for defaulted/deleted/private
		//   special member functions - I wrote an exhaustive test case to exercise the complicated Standardese.) When the compiler hook is fixed,
		//   I'll change <type_traits> to invoke it.
		//
		// Microsoft broken VS2013 STL implementation:
		//   template<class _Ty>
		//   struct is_trivially_copyable
		//       : is_trivially_copy_constructible<_Ty>::type
		//   {   // determine whether _Ty has a trivial copy constructor
		//   };
		//

		template <typename T>
		struct is_trivially_copyable : public bool_constant<__is_trivially_copyable(T)> {};

	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(EA_COMPILER_MSVC) || defined(EA_COMPILER_GNUC))
		#define EASTL_TYPE_TRAIT_is_trivially_copyable_CONFORMANCE 1

		// Micrsoft (prior to VS2012) and GCC have __has_trivial_copy, but it may not be identical with the goals of this type trait.
		template <typename T> 
		struct is_trivially_copyable : public integral_constant<bool, (__has_trivial_copy(T) || eastl::is_pod<typename eastl::remove_all_extents<T>::type>::value) && (!eastl::is_void<T>::value && !eastl::is_volatile<T>::value && !eastl::is_reference<T>::value)>{};
	#else
		#define EASTL_TYPE_TRAIT_is_trivially_copyable_CONFORMANCE 0  // Generates false negatives.

		template <typename T>
		struct is_trivially_copyable { static const bool value = eastl::is_scalar<typename eastl::remove_all_extents<T>::type>::value; };
	#endif

	#define EASTL_DECLARE_IS_TRIVIALLY_COPYABLE(T, isTriviallyCopyable)                                                                   \
		namespace eastl {                                                                                                                 \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_trivially_copyable<T>                : public eastl::integral_constant<bool, isTriviallyCopyable>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_trivially_copyable<const T>          : public eastl::integral_constant<bool, isTriviallyCopyable>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_trivially_copyable<volatile T>       : public eastl::integral_constant<bool, isTriviallyCopyable>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_trivially_copyable<const volatile T> : public eastl::integral_constant<bool, isTriviallyCopyable>  { }; \
		}

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_trivially_copyable_v = is_trivially_copyable<T>::value;
    #endif


	///////////////////////////////////////////////////////////////////////
	// is_constructible
	//
	// See the C++11 Standard, section 20.9.4.3,p6.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_constructible_CONFORMANCE 1

	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(_MSC_VER) || (defined(__clang__) && EA_COMPILER_HAS_FEATURE(is_constructible)))
		template<typename T, typename... Args>
		struct is_constructible : public bool_constant<__is_constructible(T, Args...) > {};
	#else
		// We implement a copy of move here has move_internal. We are currently stuck doing this because our move
		// implementation is in <utility.h> and <utility.h> currently #includes us, and so we have a header 
		// chicken-and-egg problem. To do: Resolve this, probably by putting eastl::move somewhere else.
		template <typename T>
		inline typename eastl::remove_reference<T>::type&& move_internal(T&& x) EA_NOEXCEPT
		{ return ((typename eastl::remove_reference<T>::type&&)x); }

		template <typename T, class ...Args>
		typename first_type_select<eastl::true_type, decltype(eastl::move_internal(T(eastl::declval<Args>()...)))>::type is(T&&, Args&& ...);

		template <typename T>
		struct can_construct_scalar_helper
		{
			static eastl::true_type can(T);
			static eastl::false_type can(...);
		};

		template <typename ...Args>
		eastl::false_type is(argument_sink, Args&& ...);

		EASTL_INTERNAL_DISABLE_DEPRECATED()
		// Except for scalars and references (handled below), check for constructibility via decltype.
		template <bool, typename T, typename... Args>
		struct is_constructible_helper_2    // argument_sink will catch all T that is not constructible from the Args and denote false_type
			: public eastl::identity<decltype(is(eastl::declval<T>(), eastl::declval<Args>()...))>::type {};
		EASTL_INTERNAL_RESTORE_DEPRECATED()

		template <typename T>
		struct is_constructible_helper_2<true, T>
			: public eastl::is_scalar<T> {};

		EASTL_INTERNAL_DISABLE_DEPRECATED()
		template <typename T, typename Arg0> // We handle the case of multiple arguments below (by disallowing them).
		struct is_constructible_helper_2<true, T, Arg0>
			: public eastl::identity<decltype(can_construct_scalar_helper<T>::can(eastl::declval<Arg0>()))>::type {};
		EASTL_INTERNAL_RESTORE_DEPRECATED()

		// Scalars and references can be constructed only with 0 or 1 argument. e.g the following is an invalid expression: int(17, 23)
		template <typename T, typename Arg0, typename ...Args>
		struct is_constructible_helper_2<true, T, Arg0, Args...>
			: public eastl::false_type {};

		template <bool, typename T, typename... Args>
		struct is_constructible_helper_1
			: public is_constructible_helper_2<eastl::is_scalar<T>::value || eastl::is_reference<T>::value, T, Args...> {};

		// Unilaterally dismiss void, abstract, unknown bound arrays, and function types as not constructible.
		template <typename T, typename... Args>
		struct is_constructible_helper_1<true, T, Args...>
			: public false_type {};

		EASTL_INTERNAL_DISABLE_DEPRECATED()
		// is_constructible
		template <typename T, typename... Args>
		struct is_constructible
			: public is_constructible_helper_1<(eastl::is_abstract<typename eastl::remove_all_extents<T>::type>::value || 
												eastl::is_array_of_unknown_bounds<T>::value                            ||
												eastl::is_function<typename eastl::remove_all_extents<T>::type>::value || 
												eastl::has_void_arg<T, Args...>::value), 
												T, Args...> {};
		EASTL_INTERNAL_RESTORE_DEPRECATED()

		// Array types are constructible if constructed with no arguments and if their element type is default-constructible
		template <typename Array, size_t N>
		struct is_constructible_helper_2<false, Array[N]>
			: public eastl::is_constructible<typename eastl::remove_all_extents<Array>::type> {};

		// Arrays with arguments are not constructible. e.g. the following is an invalid expression: int[3](37, 34, 12)
		template <typename Array, size_t N, typename ...Args>
		struct is_constructible_helper_2<false, Array[N], Args...>
			: public eastl::false_type {};

	#endif


	// You need to manually declare const/volatile variants individually if you want them.
	#define EASTL_DECLARE_IS_CONSTRUCTIBLE(T, U, isConstructible)                                                     \
		namespace eastl {                                                                                             \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_constructible<T, U>  : public eastl::integral_constant<bool, isConstructible>  { }; \
		}

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T, class... Args>
		EA_CONSTEXPR bool is_constructible_v = is_constructible<T, Args...>::value;
    #endif


	///////////////////////////////////////////////////////////////////////
	// is_trivially_constructible
	//
	// is_constructible<T, Args...>::value is true and the variable definition
	// for is_constructible, as defined below, is known to call no operation 
	// that is not trivial (3.9, 12). T and all types in the parameter pack 
	// Args shall be complete types, (possibly cv-qualified) void, or arrays 
	// of unknown bound.
	//
	// Note:
	// C++11's is_trivially_constructible sounds the same as the pre-standard
	// has_trivial_constructor type trait (which we also support here). However,
	// the definition of has_trivial_constructor has never been formally standardized
	// and so we can't just blindly equate the two to each other. Since we are 
	// moving forward with C++11 and deprecating the old type traits, we leave
	// the old ones as-is, though we defer to them in cases where we don't seem
	// to have a good alternative.
	//
	///////////////////////////////////////////////////////////////////////

	#if defined(EA_COMPILER_NO_VARIADIC_TEMPLATES)

		#define EASTL_TYPE_TRAIT_is_trivially_constructible_CONFORMANCE 0

		// In this version we allow only zero or one argument (Arg). We can add more arguments
		// by creating a number of extra specializations. It's probably not possible to 
		// simplify the implementation with recursive templates because ctor argument 
		// presence is specific.
		//
		// To consider: we can fold the two implementations below by making a macro that's defined
		// has __is_trivially_constructible(T) or eastl::has_trivial_copy<T>::value, depending on
		// whether the __is_trivially_constructible compiler intrinsic is available.

		// If the compiler has this trait built-in (which ideally all compilers would have since it's necessary for full conformance) use it.
		#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && ((defined(__clang__) && EA_COMPILER_HAS_FEATURE(is_trivially_constructible)) || defined(EA_COMPILER_MSVC))

			template <typename T, typename Arg0 = eastl::unused>
			struct is_trivially_constructible 
				: public eastl::false_type {};

			template <typename T>
			struct is_trivially_constructible<T, eastl::unused>
				: public eastl::integral_constant<bool, __is_trivially_constructible(T)> {};

			template <typename T>
			struct is_trivially_constructible<T, T>
				: public eastl::integral_constant<bool, __is_trivially_constructible(T)> {};

			template <typename T>
			struct is_trivially_constructible<T, T&>
				: public eastl::integral_constant<bool, __is_trivially_constructible(T)> {};

			template <typename T>
			struct is_trivially_constructible<T, const T&>
				: public eastl::integral_constant<bool, __is_trivially_constructible(T)> {};

			template <typename T>
			struct is_trivially_constructible<T, volatile T&>
				: public eastl::integral_constant<bool, __is_trivially_constructible(T)> {};

			template <typename T>
			struct is_trivially_constructible<T, const volatile T&>
				: public eastl::integral_constant<bool, __is_trivially_constructible(T)> {};

		#else

			template <typename T, typename Arg0 = eastl::unused>
			struct is_trivially_constructible 
				: public eastl::false_type {};

			template <typename T>
			struct is_trivially_constructible<T, eastl::unused>
				: public eastl::integral_constant<bool, eastl::is_constructible<T>::value && eastl::has_trivial_constructor<typename eastl::remove_all_extents<T>::type>::value> {};

			template <typename T>
			struct is_trivially_constructible<T, T>
				: public eastl::integral_constant<bool, eastl::is_constructible<T>::value && eastl::has_trivial_copy<T>::value> {};

			template <typename T>
			struct is_trivially_constructible<T, T&>
				: public eastl::integral_constant<bool, eastl::is_constructible<T>::value && eastl::has_trivial_copy<T>::value> {};

			template <typename T>
			struct is_trivially_constructible<T, const T&>
				: public eastl::integral_constant<bool, eastl::is_constructible<T>::value && eastl::has_trivial_copy<T>::value> {};

			template <typename T>
			struct is_trivially_constructible<T, volatile T&>
				: public eastl::integral_constant<bool, eastl::is_constructible<T>::value && eastl::has_trivial_copy<T>::value> {};

			template <typename T>
			struct is_trivially_constructible<T, const volatile T&>
				: public eastl::integral_constant<bool, eastl::is_constructible<T>::value && eastl::has_trivial_copy<T>::value> {};

		#endif

	#else

		// If the compiler has this trait built-in (which ideally all compilers would have since it's necessary for full conformance) use it.
		#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && ((EASTL_HAS_INTRINSIC(is_trivially_constructible)) || defined(EA_COMPILER_MSVC))
			#define EASTL_TYPE_TRAIT_is_trivially_constructible_CONFORMANCE 1

			// We have a problem with clang here as of clang 3.4: __is_trivially_constructible(int[]) is false, yet I believe it should be true.
			// Until it gets resolved, what we do is check for is_constructible along with __is_trivially_constructible().
			template <typename T, typename... Args>
			struct is_trivially_constructible
				: public eastl::integral_constant<bool, eastl::is_constructible<T, Args...>::value && __is_trivially_constructible(T, Args...)> {};

		#else

			#define EASTL_TYPE_TRAIT_is_trivially_constructible_CONFORMANCE 0   // This is 0 but in fact it will work for most real-world cases due to the has_trivial_constructor specialization below.

			template <typename T, typename... Args>
			struct is_trivially_constructible
				: public eastl::false_type {};

			template <typename T>
			struct is_trivially_constructible<T>
				: public eastl::integral_constant<bool, eastl::is_constructible<T>::value && eastl::has_trivial_constructor<typename eastl::remove_all_extents<T>::type>::value> {};

			// It's questionable whether we can use has_trivial_copy here, as it could theoretically create a false-positive.
			template <typename T>
			struct is_trivially_constructible<T, T>
				: public eastl::integral_constant<bool, eastl::is_constructible<T>::value && eastl::has_trivial_copy<T>::value> {};

			template <typename T>
			struct is_trivially_constructible<T, T&&>
				: public eastl::integral_constant<bool, eastl::is_constructible<T>::value && eastl::has_trivial_copy<T>::value> {};

			template <typename T>
			struct is_trivially_constructible<T, T&>
				: public eastl::integral_constant<bool, eastl::is_constructible<T>::value && eastl::has_trivial_copy<T>::value> {};

			template <typename T>
			struct is_trivially_constructible<T, const T&>
				: public eastl::integral_constant<bool, eastl::is_constructible<T>::value && eastl::has_trivial_copy<T>::value> {};

			template <typename T>
			struct is_trivially_constructible<T, volatile T&>
				: public eastl::integral_constant<bool, eastl::is_constructible<T>::value && eastl::has_trivial_copy<T>::value> {};

			template <typename T>
			struct is_trivially_constructible<T, const volatile T&>
				: public eastl::integral_constant<bool, eastl::is_constructible<T>::value && eastl::has_trivial_copy<T>::value> {};

		#endif

	#endif


	#define EASTL_DECLARE_IS_TRIVIALLY_CONSTRUCTIBLE(T, isTriviallyConstructible)                                                                   \
		namespace eastl {                                                                                                                           \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_trivially_constructible<T>                : public eastl::integral_constant<bool, isTriviallyConstructible>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_trivially_constructible<const T>          : public eastl::integral_constant<bool, isTriviallyConstructible>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_trivially_constructible<volatile T>       : public eastl::integral_constant<bool, isTriviallyConstructible>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_trivially_constructible<const volatile T> : public eastl::integral_constant<bool, isTriviallyConstructible>  { }; \
		}

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_trivially_constructible_v = is_trivially_constructible<T>::value;
    #endif


	///////////////////////////////////////////////////////////////////////
	// is_trivially_default_constructible
	//
	// is_trivially_constructible<T>::value is true.
	// This is thus identical to is_trivially_constructible<T>::value.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_trivially_default_constructible_CONFORMANCE EASTL_TYPE_TRAIT_is_trivially_constructible_CONFORMANCE

	template <typename T>
	struct is_trivially_default_constructible 
		: public eastl::is_trivially_constructible<T> {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_trivially_default_constructible_v = is_trivially_default_constructible<T>::value;
    #endif


	///////////////////////////////////////////////////////////////////////
	// is_trivial
	//
	// is_trivial<T>::value == true if T is a scalar type, a trivially copyable 
	// class with a trivial default constructor, or array of such type/class, 
	// possibly cv-qualified), provides the member constant value equal true. 
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_trivial_CONFORMANCE ((EASTL_TYPE_TRAIT_is_trivially_default_constructible_CONFORMANCE && EASTL_TYPE_TRAIT_is_trivially_copyable_CONFORMANCE) ? 1 : 0)

	#if defined(_MSC_VER) && _MSC_VER == 1800
		template<bool, typename T>
		struct is_trivial_helper
			: public eastl::integral_constant<bool, eastl::is_trivially_copyable<T>::value && eastl::is_trivially_default_constructible<T>::value>{};

		template<typename T>
		struct is_trivial_helper<true, T>
			: public false_type{};

		template <typename T>
		struct is_trivial
			: public is_trivial_helper<(EA_ALIGN_OF(T) > EA_PLATFORM_MIN_MALLOC_ALIGNMENT), T>::type{};
	#else
		// All other compilers seem to be able to handle aligned types passed as value
		template <typename T>
		struct is_trivial 
			: public eastl::integral_constant<bool, eastl::is_trivially_copyable<T>::value && eastl::is_trivially_default_constructible<T>::value> {};
	#endif

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_trivial_v = is_trivial<T>::value;
    #endif

	

	///////////////////////////////////////////////////////////////////////
	// is_nothrow_constructible
	//
	// is_constructible<T, Args...>::value is true and the variable definition
	// for is_constructible, as defined below, is known not to throw any
	// exceptions (5.3.7). T and all types in the parameter pack Args shall
	// be complete types, (possibly cv-qualified) void, or arrays of unknown bound.
	//
	///////////////////////////////////////////////////////////////////////
	#if defined(EA_COMPILER_NO_NOEXCEPT)

		#define EASTL_TYPE_TRAIT_is_nothrow_constructible_CONFORMANCE 0

		template <typename T, typename... Args>
		struct is_nothrow_constructible
			: public eastl::false_type {};

		template <typename T>
		struct is_nothrow_constructible<T>
			: public eastl::integral_constant<bool, eastl::has_nothrow_constructor<T>::value> {};

		template <typename T>
		struct is_nothrow_constructible<T, T>
			: public eastl::integral_constant<bool, eastl::has_nothrow_copy<T>::value> {};

		template <typename T>
		struct is_nothrow_constructible<T, const T&>
			: public eastl::integral_constant<bool, eastl::has_nothrow_copy<T>::value> {};

		template <typename T>
		struct is_nothrow_constructible<T, T&>
			: public eastl::integral_constant<bool, eastl::has_nothrow_copy<T>::value> {};

		template <typename T>
		struct is_nothrow_constructible<T, T&&>
			: public eastl::integral_constant<bool, eastl::has_nothrow_copy<T>::value> {};

	#else
		// could use __is_nothrow_constructible(T, Args...), if available, instead of this implementation.

		#if defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION < 4008)
			#define EASTL_TYPE_TRAIT_is_nothrow_constructible_CONFORMANCE 0 // GCC up to v4.7's noexcept is broken and fails to generate true for the case of compiler-generated constructors.
		#else
			#define EASTL_TYPE_TRAIT_is_nothrow_constructible_CONFORMANCE EASTL_TYPE_TRAIT_is_constructible_CONFORMANCE
		#endif
	   
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// *_noexcept_wrapper implements a workaround for VS2015 preview.  A standards conforming noexcept operator allows variadic template expansion.
		// There appears to be an issue with VS2015 preview that prevents variadic template expansion into a noexcept operator that is passed directly 
		// to a template parameter.
		// 
		// The fix hoists the noexcept expression into a separate struct and caches the result of the expression.  This result is then passed to integral_constant.
		//
		// Example code from Clang libc++
		// template <class _Tp, class... _Args>
		// struct __libcpp_is_nothrow_constructible<[>is constructible*/true, /*is reference<]false, _Tp, _Args...>
		//     : public integral_constant<bool, noexcept(_Tp(declval<_Args>()...))> { };
		//
		
		template <typename T, typename... Args>
		struct is_nothrow_constructible_helper_noexcept_wrapper
			{ static const bool value = noexcept(T(eastl::declval<Args>()...)); };

		template <bool, typename T, typename... Args>
		struct is_nothrow_constructible_helper;

		template <typename T, typename... Args>
		struct is_nothrow_constructible_helper<true, T, Args...>
			: public eastl::integral_constant<bool, is_nothrow_constructible_helper_noexcept_wrapper<T, Args...>::value> {};

		template<typename T, typename Arg>
		struct is_nothrow_constructible_helper<true, T, Arg>
			: public eastl::integral_constant<bool, noexcept(T(eastl::declval<Arg>()))> {};

		template<typename T>
		struct is_nothrow_constructible_helper<true, T>
			: public eastl::integral_constant<bool, noexcept(T())> {};

		template <typename T, typename... Args>
		struct is_nothrow_constructible_helper<false, T, Args...>
			: public eastl::false_type {};

		template <typename T, typename... Args>
		struct is_nothrow_constructible
			: public eastl::is_nothrow_constructible_helper<eastl::is_constructible<T, Args...>::value, T, Args...> {};

		template <typename T, size_t N>
		struct is_nothrow_constructible<T[N]>
			: public eastl::is_nothrow_constructible_helper<eastl::is_constructible<T>::value, T> {};
	#endif

	#define EASTL_DECLARE_IS_NOTHROW_CONSTRUCTIBLE(T, isNothrowConstructible)                                                    \
		namespace eastl{                                                                                                         \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_nothrow_constructible<T> : public eastl::integral_constant<bool, isNothrowConstructible>  { }; \
		}

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T, typename... Args>
		EA_CONSTEXPR bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;
    #endif


	///////////////////////////////////////////////////////////////////////
	// is_default_constructible
	//
	// is_constructible<T>::value is true.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_default_constructible_CONFORMANCE EASTL_TYPE_TRAIT_is_constructible_CONFORMANCE

	template <typename T>
	struct is_default_constructible
		: public eastl::is_constructible<T> {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_default_constructible_v = is_default_constructible<T>::value;
    #endif


	///////////////////////////////////////////////////////////////////////
	// is_copy_constructible
	//
	// is_constructible<T, const T&>::value is true.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_copy_constructible_CONFORMANCE EASTL_TYPE_TRAIT_is_constructible_CONFORMANCE

	template <typename T>
	struct is_copy_constructible
		: public eastl::is_constructible<T, typename eastl::add_lvalue_reference<typename eastl::add_const<T>::type>::type> {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_copy_constructible_v = is_copy_constructible<T>::value;
    #endif


	///////////////////////////////////////////////////////////////////////
	// is_trivially_copy_constructible
	//
	// is_trivially_constructible<T, const T&>::value is true.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_trivially_copy_constructible_CONFORMANCE EASTL_TYPE_TRAIT_is_trivially_constructible_CONFORMANCE

	template <typename T> 
	struct is_trivially_copy_constructible
		: public eastl::is_trivially_constructible<T, typename eastl::add_lvalue_reference<typename eastl::add_const<T>::type>::type> {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_trivially_copy_constructible_v = is_trivially_copy_constructible<T>::value;
    #endif


	///////////////////////////////////////////////////////////////////////
	// is_nothrow_copy_constructible
	//
	// is_nothrow_-constructible<T, const T&>::value is true.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_nothrow_copy_constructible_CONFORMANCE EASTL_TYPE_TRAIT_is_nothrow_constructible_CONFORMANCE

	template <typename T>
	struct is_nothrow_copy_constructible
		: public is_nothrow_constructible<T, typename eastl::add_lvalue_reference<typename eastl::add_const<T>::type>::type> {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<T>::value;
    #endif


	///////////////////////////////////////////////////////////////////////
	// is_move_constructible
	//
	// is_constructible<T, T&&>::value is true.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_move_constructible_CONFORMANCE EASTL_TYPE_TRAIT_is_constructible_CONFORMANCE

	template <typename T>
	struct is_move_constructible
		: public eastl::is_constructible<T, typename eastl::add_rvalue_reference<T>::type> {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_move_constructible_v = is_move_constructible<T>::value;
    #endif


	///////////////////////////////////////////////////////////////////////
	// is_trivially_move_constructible
	//
	// is_trivially_constructible<T, T&&>::value is true.
	// T shall be a complete type, (possibly cv-qualified) void, or an 
	// array of unknown bound.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_trivially_move_constructible_CONFORMANCE EASTL_TYPE_TRAIT_is_trivially_constructible_CONFORMANCE

	template <typename T> 
	struct is_trivially_move_constructible
		: public eastl::is_trivially_constructible<T, typename eastl::add_rvalue_reference<T>::type> {};

	#define EASTL_DECLARE_IS_TRIVIALLY_MOVE_CONSTRUCTIBLE(T, isTrivallyMoveConstructible)                                                      \
		namespace eastl{                                                                                                                       \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_trivially_move_constructible<T>  : public eastl::integral_constant<bool, isTriviallyMoveConstructible>  { }; \
		}
	
	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_trivially_move_constructible_v = is_trivially_move_constructible<T>::value;
    #endif



	///////////////////////////////////////////////////////////////////////
	// is_assignable
	//
	// The expression declval<T>() = declval<U>() is well-formed when treated as an unevaluated operand.
	// Access checking is performed as if in a context unrelated to T and U. Only the validity of
	// the immediate context of the assignment expression is considered. The compilation of the expression 
	// can result in side effects such as the instantiation of class template specializations and function 
	// template specializations, the generation of implicitly-defined functions, and so on. Such side 
	// effects are not in the "immediate context" and can result in the program being ill-formed.
	//
	// Note: 
	// This type trait has a misleading and counter-intuitive name. It does not indicate whether an instance
	// of U can be assigned to an instance of T (e.g. t = u). Instead it indicates whether the assignment can be 
	// done after adding rvalue references to both, as in add_rvalue_reference<T>::type = add_rvalue_reference<U>::type.
	// A counterintuitive result of this is that is_assignable<int, int>::value == false. The is_copy_assignable
	// trait indicates if a type can be assigned to its own type, though there isn't a standard C++ way to tell
	// if an arbitrary type is assignable to another type.
	// http://stackoverflow.com/questions/19920213/why-is-stdis-assignable-counter-intuitive
	//
	// Note:
	// A true is_assignable value doesn't guarantee that the expression is compile-able, the compiler checks 
	// only that the assignment matches before compilation. In particular, if you have templated operator=
	// for a class, the compiler will always say is_assignable is true, regardless of what's being tested
	// on the right hand side of the expression. It may actually turn out during compilation that the 
	// templated operator= fails to compile because in practice it doesn't accept every possible type for
	// the right hand side of the expression.
	//
	// Expected results:                                        
	//     is_assignable<void, void>::value             == false
	//     is_assignable<int&, int>::value              == true 
	//     is_assignable<int, int>::value               == false
	//     is_assignable<int, int&>::value              == false
	//     is_assignable<bool, bool>::value             == false
	//     is_assignable<int, float>::value             == false
	//     is_assignable<int[], int[]>::value           == false
	//     is_assignable<char*, int*>::value            == false
	//     is_assignable<char*, const char*>::value     == false
	//     is_assignable<const char*, char*>::value     == false
	//     is_assignable<PodA, PodB*>::value            == false
	//     is_assignable<Assignable, Assignable>::value == true 
	//     is_assignable<Assignable, Unrelated>::value  == false
	//
	// Note: 
	// Our implementation here yields different results than does the std::is_assignable from Dinkumware-based Standard
	// Libraries, but yields similar results to the std::is_assignable from GCC's libstdc++ and clang's libc++. It may
	// possibly be that the Dinkumware results are intentionally different for some practical purpose or because they
	// represent the spirit or the Standard but not the letter of the Standard. 
	//
	///////////////////////////////////////////////////////////////////////
	#define EASTL_TYPE_TRAIT_is_assignable_CONFORMANCE 1

	template<typename T, typename U>
	struct is_assignable_helper
	{
		template<typename, typename>
		static eastl::no_type is(...);

		template<typename T1, typename U1>
		static decltype(eastl::declval<T1>() = eastl::declval<U1>(), eastl::yes_type()) is(int);

		static const bool value = (sizeof(is<T, U>(0)) == sizeof(eastl::yes_type));
	};

	template<typename T, typename U>
	struct is_assignable : 
		public eastl::integral_constant<bool, eastl::is_assignable_helper<T, U>::value> {};

	// The main purpose of this function is to help the non-conforming case above.
	// Note: We don't handle const/volatile variations here, as we expect the user to 
	// manually specify any such variations via this macro.
	// Example usage: 
	//     EASTL_DECLARE_IS_ASSIGNABLE(int, int, false)
	//
	#define EASTL_DECLARE_IS_ASSIGNABLE(T, U, isAssignable)                                                    \
		namespace eastl {                                                                                      \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_assignable<T, U> : public eastl::integral_constant<bool, isAssignable>  { }; \
		}

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T, class U>
		EA_CONSTEXPR bool is_assignable_v = is_assignable<T, U>::value;
    #endif



	///////////////////////////////////////////////////////////////////////
	// is_lvalue_assignable
	// 
	// Deprecated. Use is_copy_assignable instead.
	//
	// This is an EASTL extension function which is like is_assignable but
	// works for arbitrary assignments and not just rvalue assignments.
	// This function provides an intuitive assignability test, as opposed
	// to is_assignable.
	//
	// Note: is_lvalue_assignable<T, T> === is_copy_assignable<T>
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_lvalue_assignable_CONFORMANCE EASTL_TYPE_TRAIT_is_assignable_CONFORMANCE

	template <typename T, typename U> 
	struct EASTL_REMOVE_AT_2024_APRIL is_lvalue_assignable
		: public eastl::is_assignable<typename eastl::add_lvalue_reference<T>::type,
									  typename eastl::add_lvalue_reference<typename eastl::add_const<U>::type>::type> {};

	#define EASTL_DECLARE_IS_LVALUE_ASSIGNABLE(T, U, isLvalueAssignable)                                                    \
		namespace eastl {                                                                                                   \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_lvalue_assignable<T, U> : public eastl::integral_constant<bool, isLvalueAssignable>  { }; \
		}



	///////////////////////////////////////////////////////////////////////
	// is_trivially_assignable
	//
	// is_assignable<T, U>::value is true and the assignment, as defined by
	// is_assignable, is known to call no operation that is not trivial (3.9, 12).
	// T and U shall be complete types, (possibly cv-qualified) void, or
	// arrays of unknown bound
	///////////////////////////////////////////////////////////////////////

	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(__clang__) && EA_COMPILER_HAS_FEATURE(is_trivially_assignable))
		#define EASTL_TYPE_TRAIT_is_trivially_assignable_CONFORMANCE 1

		template <typename T, typename U>
		struct is_trivially_assignable
			: eastl::integral_constant<bool, __is_trivially_assignable(T, U)> {};

	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(_MSC_VER) && (_MSC_VER >= 1800))
		#define EASTL_TYPE_TRAIT_is_trivially_assignable_CONFORMANCE EASTL_TYPE_TRAIT_is_assignable_CONFORMANCE

		// This code path is attempting to work around the issue with VS2013 __is_trivially_assignable compiler intrinsic documented in the link
		// below.  todo: Re-evaluate in VS2014.  
		//
		// https://connect.microsoft.com/VisualStudio/feedback/details/806233/std-is-trivially-copyable-const-int-n-and-std-is-trivially-copyable-int-n-incorrect

		template <bool A, typename T, typename U>
		struct is_trivially_assignable_helper;

		template <typename T, typename U>
		struct is_trivially_assignable_helper<true, T, U> : eastl::integral_constant<bool, __is_trivially_assignable(T, U)>{};

		template <typename T, typename U>
		struct is_trivially_assignable_helper<false, T, U> : false_type{};

		template <typename T, typename U>
		struct is_trivially_assignable
			: eastl::integral_constant<bool, is_trivially_assignable_helper< eastl::is_assignable<T, U>::value, T, U >::value> {};

	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(EA_COMPILER_MSVC) || defined(EA_COMPILER_GNUC))
		#define EASTL_TYPE_TRAIT_is_trivially_assignable_CONFORMANCE EASTL_TYPE_TRAIT_is_assignable_CONFORMANCE

		// Micrsoft (up till at least VS2012) and GCC have __has_trivial_assign, but it may not be identical with the goals of this type trait.
		// The Microsoft type trait headers suggest that a future version of VS will have a __is_trivially_assignable intrinsic, but we
		// need to come up with something in the meantime. To do: Re-evalulate this for VS2013+ when it becomes available.
		template <typename T, typename U>
		struct is_trivially_assignable
			: eastl::integral_constant<bool, eastl::is_assignable<T, U>::value && 
									   (eastl::is_pod<typename eastl::remove_reference<T>::type>::value || __has_trivial_assign(typename eastl::remove_reference<T>::type))> {};
	#else

		#define EASTL_TYPE_TRAIT_is_trivially_assignable_CONFORMANCE 0  // Generates false negatives.

		template <typename T, typename U>
		struct is_trivially_assignable
			: public eastl::false_type {};

		template <typename T>
		struct is_trivially_assignable<T&, T>
			: public eastl::integral_constant<bool, eastl::is_scalar<T>::value> {};

		template <typename T>
		struct is_trivially_assignable<T&, T&>
			: public eastl::integral_constant<bool, eastl::is_scalar<T>::value> {};

		template <typename T>
		struct is_trivially_assignable<T&, const T&>
			: public eastl::integral_constant<bool, eastl::is_scalar<T>::value> {};

		template <typename T>
		struct is_trivially_assignable<T&, T&&>
			: public eastl::integral_constant<bool, eastl::is_scalar<T>::value> {};

	#endif

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T, class U>
		EA_CONSTEXPR bool is_trivially_assignable_v = is_trivially_assignable<T, U>::value;
    #endif

	// The main purpose of this function is to help the non-conforming case above.
	// Note: We don't handle const/volatile variations here, as we expect the user to 
	// manually specify any such variations via this macro.
	// Example usage: 
	//     EASTL_DECLARE_IS_TRIVIALLY_ASSIGNABLE(int, int, false)
	//
	#define EASTL_DECLARE_IS_TRIVIALLY_ASSIGNABLE(T, U, isTriviallyAssignable)                                                   \
		namespace eastl {                                                                                                        \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_trivially_assignable<T, U> : public eastl::integral_constant<bool, isTriviallyAssignable> { }; \
		}



	///////////////////////////////////////////////////////////////////////
	// is_nothrow_assignable
	//
	// is_assignable<T, U>::value is true and the assignment is known
	// not to throw any exceptions (5.3.7). T and U shall be complete
	// types, (possibly cv-qualified) void, or arrays of unknown bound.
	//
	///////////////////////////////////////////////////////////////////////
		
	#if defined(_MSC_VER) && (_MSC_VER >= 1800) // VS2013+
		#define EASTL_TYPE_TRAIT_is_nothrow_assignable_CONFORMANCE 1

		template <typename T, typename U>
		struct is_nothrow_assignable
			: eastl::integral_constant<bool, __is_nothrow_assignable(T, U)> {};

	#elif defined(EA_COMPILER_NO_NOEXCEPT) || defined(__EDG_VERSION__) // EDG mis-compiles the conforming code below and so must be placed here.
		#define EASTL_TYPE_TRAIT_is_nothrow_assignable_CONFORMANCE 0

		template <typename T, typename U>
		struct is_nothrow_assignable
			: public false_type {};

		// Note that the following are crippled in that they support only assignment of T types to other T types.
		template <typename T>
		struct is_nothrow_assignable<T&, T>
			: public eastl::integral_constant<bool, eastl::has_nothrow_assign<T>::value> {};

		template <typename T>
		struct is_nothrow_assignable<T&, T&>
			: public eastl::integral_constant<bool, eastl::has_nothrow_assign<T>::value> {};

		template <typename T>
		struct is_nothrow_assignable<T&, const T&>
			: public eastl::integral_constant<bool, eastl::has_nothrow_assign<T>::value> {};

	#else
		#define EASTL_TYPE_TRAIT_is_nothrow_assignable_CONFORMANCE 1

		template <bool, typename T, typename U>
		struct is_nothrow_assignable_helper;

		template <typename T, typename U>
		struct is_nothrow_assignable_helper<false, T, U> 
			: public false_type {};

		template <typename T, typename U>
		struct is_nothrow_assignable_helper<true, T, U> // Set to true if the assignment (same as is_assignable) cannot generate an exception.
			: public eastl::integral_constant<bool, noexcept(eastl::declval<T>() = eastl::declval<U>()) >
		{
		};

		template <typename T, typename U>
		struct is_nothrow_assignable
			: public eastl::is_nothrow_assignable_helper<eastl::is_assignable<T, U>::value, T, U>
		{
		};
	#endif

	#define EASTL_DECLARE_IS_NOTHROW_ASSIGNABLE(T, isNothrowAssignable)                                                                   \
		namespace eastl{                                                                                                                  \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_nothrow_assignable<T>                : public eastl::integral_constant<bool, isNothrowAssignable>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_nothrow_assignable<const T>          : public eastl::integral_constant<bool, isNothrowAssignable>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_nothrow_assignable<volatile T>       : public eastl::integral_constant<bool, isNothrowAssignable>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_nothrow_assignable<const volatile T> : public eastl::integral_constant<bool, isNothrowAssignable>  { }; \
		}

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T, class U>
		EA_CONSTEXPR bool is_nothrow_assignable_v = is_nothrow_assignable<T, U>::value;
	#endif



	///////////////////////////////////////////////////////////////////////
	// is_copy_assignable
	//
	// is_assignable<T&, const T&>::value is true. T shall be a complete type,
	// (possibly cv -qualified) void, or an array of unknown bound.
	//
	// This (and not is_assignable) is the type trait you use to tell if you 
	// can do an arbitrary assignment. is_assignable tells if you can do an 
	// assignment specifically to an rvalue and not in general. 
	// http://stackoverflow.com/a/19921030/725009
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_copy_assignable_CONFORMANCE EASTL_TYPE_TRAIT_is_assignable_CONFORMANCE

	template <typename T> 
	struct is_copy_assignable
		: public eastl::is_assignable<typename eastl::add_lvalue_reference<T>::type,
									  typename eastl::add_lvalue_reference<typename eastl::add_const<T>::type>::type> {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_copy_assignable_v = is_copy_assignable<T>::value;
    #endif


	///////////////////////////////////////////////////////////////////////
	// is_trivially_copy_assignable
	//
	// is_trivially_assignable<T&, const T&>::value is true. T shall be a 
	// complete type, (possibly cv-qualified) void, or an array of unknown bound.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_trivially_copy_assignable_CONFORMANCE EASTL_TYPE_TRAIT_is_trivially_assignable_CONFORMANCE

#if EASTL_TYPE_TRAIT_is_trivially_copy_assignable_CONFORMANCE
	template <typename T>
	struct is_trivially_copy_assignable
		: public eastl::is_trivially_assignable<typename eastl::add_lvalue_reference<T>::type,
												typename eastl::add_lvalue_reference<typename eastl::add_const<T>::type>::type> {};
#else
	template <typename T>
	struct is_trivially_copy_assignable
		: public integral_constant<bool,
			eastl::is_scalar<T>::value || eastl::is_pod<T>::value || eastl::is_trivially_assignable<typename eastl::add_lvalue_reference<T>::type, typename eastl::add_lvalue_reference<typename eastl::add_const<T>::type>::type>::value
		> {};
#endif

	#define EASTL_DECLARE_IS_TRIVIALLY_COPY_ASSIGNABLE(T, isTriviallyCopyAssignable)                                                    \
		namespace eastl {                                                                                                               \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_trivially_copy_assignable<T> : public eastl::integral_constant<bool, isTriviallyCopyAssignable>  { }; \
		}

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_trivially_copy_assignable_v = is_trivially_copy_assignable<T>::value;
    #endif

	///////////////////////////////////////////////////////////////////////
	// is_nothrow_copy_assignable
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_nothrow_copy_assignable_CONFORMANCE EASTL_TYPE_TRAIT_is_nothrow_assignable_CONFORMANCE

	template <typename T>
	struct is_nothrow_copy_assignable
		: public eastl::is_nothrow_assignable<typename eastl::add_lvalue_reference<T>::type,
											  typename eastl::add_lvalue_reference<typename eastl::add_const<T>::type>::type> {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<T>::value;
    #endif


	///////////////////////////////////////////////////////////////////////
	// is_move_assignable
	//
	// is_assignable<T&, T&&>::value is true. T shall be a complete type,
	// (possibly cv -qualified) void, or an array of unknown bound.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_move_assignable_CONFORMANCE EASTL_TYPE_TRAIT_is_assignable_CONFORMANCE

	template <typename T>
	struct is_move_assignable
		: public eastl::is_assignable<typename eastl::add_lvalue_reference<T>::type,
									  typename eastl::add_rvalue_reference<T>::type> {};

	#define EASTL_DECLARE_IS_MOVE_ASSIGNABLE(T, isMoveAssignable)                                                                   \
		namespace eastl{                                                                                                            \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_move_assignable<T>                : public eastl::integral_constant<bool, isMoveAssignable>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_move_assignable<const T>          : public eastl::integral_constant<bool, isMoveAssignable>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_move_assignable<volatile T>       : public eastl::integral_constant<bool, isMoveAssignable>  { }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_move_assignable<const volatile T> : public eastl::integral_constant<bool, isMoveAssignable>  { }; \
		}

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_move_assignable_v = is_move_assignable<T>::value;
    #endif


 
	///////////////////////////////////////////////////////////////////////
	// is_trivially_move_assignable
	//
	// is_trivially_-assignable<T&, T&&>::value is true. T shall be a complete type,
	// (possibly cv-qualified) void, or an array of unknown bound.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_trivially_move_assignable_CONFORMANCE EASTL_TYPE_TRAIT_is_trivially_assignable_CONFORMANCE

	template <typename T>
	struct is_trivially_move_assignable
		: public eastl::is_trivially_assignable<typename eastl::add_lvalue_reference<T>::type,
												typename eastl::add_rvalue_reference<T>::type> {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_trivially_move_assignable_v = is_trivially_move_assignable<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_nothrow_move_assignable
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_nothrow_move_assignable_CONFORMANCE EASTL_TYPE_TRAIT_is_nothrow_assignable_CONFORMANCE

	template <typename T>
	struct is_nothrow_move_assignable
		: public eastl::is_nothrow_assignable<typename eastl::add_lvalue_reference<T>::type,
											  typename eastl::add_rvalue_reference<T>::type> {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<T>::value;
	#endif

	///////////////////////////////////////////////////////////////////////
	// is_destructible
	//
	// For a complete type T and given 
	//     template <class U>
	//     struct test { U u; };
	// test<T>::~test() is not deleted (C++11 "= delete").
	// T shall be a complete type, (possibly cv-qualified) void, or an array of unknown bound.
	//
	///////////////////////////////////////////////////////////////////////

	#if defined(_MSC_VER) && (_MSC_VER >= 1920)
		#define EASTL_TYPE_TRAIT_is_destructible_CONFORMANCE 1

		template <typename T>
		struct is_destructible
			: integral_constant<bool, __is_destructible(T)> {};

	#elif defined(EA_COMPILER_NO_DECLTYPE) || defined(EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS) || defined(_MSC_VER) || defined(__EDG_VERSION__) // VS2012 and EDG mis-compile the conforming code below and so must be placed here.
		#define EASTL_TYPE_TRAIT_is_destructible_CONFORMANCE 0

		// This implementation works for almost all cases, with the primary exception being the 
		// case that the user declared the destructor as deleted. To deal with that case the 
		// user needs to use EASTL_DECLARE_IS_NOT_DESTRUCTIBLE to cause is_destructible<T>::value
		// to be false.

		EASTL_INTERNAL_DISABLE_DEPRECATED()
		template <typename T>
		struct is_destructible
			: public eastl::integral_constant<bool, !eastl::is_array_of_unknown_bounds<T>::value && 
													!eastl::is_void<T>::value                    && 
													!eastl::is_function<T>::value> {};
		EASTL_INTERNAL_RESTORE_DEPRECATED()
	#else
		#define EASTL_TYPE_TRAIT_is_destructible_CONFORMANCE 1

		template <typename>
		eastl::false_type destructible_test_function(...);
		
		template <typename T, typename U = typename eastl::remove_all_extents<T>::type, typename V = decltype(eastl::declval<U&>().~U())>
		eastl::true_type destructible_test_function(int);

		EASTL_INTERNAL_DISABLE_DEPRECATED()
		template <typename T, bool = eastl::is_array_of_unknown_bounds<T>::value || // Exclude these types from being considered destructible.
									 eastl::is_void<T>::value                    || 
									 eastl::is_function<T>::value>
		struct is_destructible_helper
			: public eastl::identity<decltype(eastl::destructible_test_function<T>(0))>::type {}; // Need to wrap decltype with identity because some compilers otherwise don't like the bare decltype usage.
		EASTL_INTERNAL_RESTORE_DEPRECATED()

		template <typename T>
		struct is_destructible_helper<T, true>
			: public eastl::false_type {};

		template <typename T, bool Whatever>
		struct is_destructible_helper<T&, Whatever> // Reference are trivially destructible.
			: public eastl::true_type {};

		template <typename T, bool Whatever>
		struct is_destructible_helper<T&&, Whatever> // Reference are trivially destructible.
			: public eastl::true_type {};

		template <typename T>
		struct is_destructible
			: public is_destructible_helper<T> {};

	#endif

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_destructible_v = is_destructible<T>::value;
    #endif

	#define EASTL_DECLARE_IS_DESTRUCTIBLE(T, isDestructible)												                \
		namespace eastl{                                                                                                    \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_destructible<T>                : public eastl::integral_constant<bool, isDestructible>{}; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_destructible<const T>          : public eastl::integral_constant<bool, isDestructible>{}; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_destructible<volatile T>       : public eastl::integral_constant<bool, isDestructible>{}; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_destructible<const volatile T> : public eastl::integral_constant<bool, isDestructible>{}; \
		}



	///////////////////////////////////////////////////////////////////////
	// is_trivially_destructible
	//
	// is_destructible<T>::value is true and the indicated destructor is 
	// known to be trivial. T shall be a complete type, (possibly cv-qualified)
	// void, or an array of unknown bound.
	//
	// A destructor is trivial if it is not user-provided and if:
	//    - the destructor is not virtual,
	//    - all of the direct base classes of its class have trivial destructors, and
	//    - for all of the non-static data members of its class that are of 
	//      class type (or array thereof), each such class has a trivial destructor.
	//
	///////////////////////////////////////////////////////////////////////

	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && ((defined(_MSC_VER) && (_MSC_VER >= 1920)) || (defined(__clang__) && EASTL_HAS_INTRINSIC(is_trivially_destructible)))
		#define EASTL_TYPE_TRAIT_is_trivially_destructible_CONFORMANCE 1

		template <typename T>
		struct is_trivially_destructible
			: integral_constant<bool, __is_trivially_destructible(T)> {};

	#elif EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(_MSC_VER) || defined(EA_COMPILER_GNUC) || defined(__clang__))
		#define EASTL_TYPE_TRAIT_is_trivially_destructible_CONFORMANCE EASTL_TYPE_TRAIT_is_destructible_CONFORMANCE

		template <typename T>
		struct is_trivially_destructible // Can't use just __has_trivial_destructor(T) because some compilers give it slightly different meaning, and are just plain broken, such as VC++'s __has_trivial_destructor, which says false for fundamental types.
			: public integral_constant<bool, eastl::is_destructible<T>::value && ((__has_trivial_destructor(T) && !eastl::is_hat_type<T>::value)|| eastl::is_scalar<typename eastl::remove_all_extents<T>::type>::value)> {};

	#else
		#define EASTL_TYPE_TRAIT_is_trivially_destructible_CONFORMANCE 0

		template <typename T>
		struct is_trivially_destructible_helper
			: public integral_constant<bool, (eastl::is_pod<T>::value || eastl::is_scalar<T>::value || eastl::is_reference<T>::value) && !eastl::is_void<T>::value> {};

		template <typename T> 
		struct is_trivially_destructible
			: public eastl::is_trivially_destructible_helper<typename eastl::remove_all_extents<T>::type> {};
	#endif

	#define EASTL_DECLARE_IS_TRIVIALLY_DESTRUCTIBLE(T, isTriviallyDestructible)                                                       \
		namespace eastl{                                                                                                              \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_trivially_destructible<T>                : public eastl::integral_constant<bool, isTriviallyDestructible>{}; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_trivially_destructible<const T>          : public eastl::integral_constant<bool, isTriviallyDestructible>{}; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_trivially_destructible<volatile T>       : public eastl::integral_constant<bool, isTriviallyDestructible>{}; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_trivially_destructible<const volatile T> : public eastl::integral_constant<bool, isTriviallyDestructible>{}; \
		}

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_trivially_destructible_v = is_trivially_destructible<T>::value;
    #endif




	///////////////////////////////////////////////////////////////////////
	// is_nothrow_destructible
	//
	// is_destructible<T>::value is true and the indicated destructor is 
	// known not to throw any exceptions (5.3.7). T shall be a complete type, 
	// (possibly cv-qualified) void, or an array of unknown bound.
	//
	///////////////////////////////////////////////////////////////////////

	#if defined(_MSC_VER) && (_MSC_VER >= 1920) 
		#define EASTL_TYPE_TRAIT_is_nothrow_destructible_CONFORMANCE ((_MSC_VER >= 1900) ? 1 : 0) // VS2013 (1800) doesn't support noexcept and so can't support all usage of this properly (in particular default exception specifications defined in [C++11 Standard, 15.4 paragraph 14].

		template <typename T>
		struct is_nothrow_destructible
			: integral_constant<bool, __is_nothrow_destructible(T)> {};

	#elif defined(EA_COMPILER_NO_NOEXCEPT) 
		#define EASTL_TYPE_TRAIT_is_nothrow_destructible_CONFORMANCE 0

		template <typename T>
		struct is_nothrow_destructible_helper
			: public eastl::integral_constant<bool, eastl::is_scalar<T>::value || eastl::is_reference<T>::value> {};

		template <typename T> 
		struct is_nothrow_destructible
			: public eastl::is_nothrow_destructible_helper<typename eastl::remove_all_extents<T>::type> {};

	#else
		#if defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION < 4008)
			#define EASTL_TYPE_TRAIT_is_nothrow_destructible_CONFORMANCE 0 // GCC up to v4.7's noexcept is broken and fails to generate true for the case of compiler-generated destructors.
		#else
			#define EASTL_TYPE_TRAIT_is_nothrow_destructible_CONFORMANCE EASTL_TYPE_TRAIT_is_destructible_CONFORMANCE
		#endif
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// *_noexcept_wrapper implements a workaround for VS2015.  A standards conforming noexcept operator allows variadic template expansion.
		// There appears to be an issue with VS2015 that prevents variadic template expansion into a noexcept operator that is passed directly 
		// to a template parameter.
		// 
		// The fix hoists the noexcept expression into a separate struct and caches the result of the expression.  This result is then passed to integral_constant.
		//
		// Example code from Clang libc++
		// template <class _Tp, class... _Args>
		// struct __libcpp_is_nothrow_constructible<[>is constructible*/true, /*is reference<]false, _Tp, _Args...>
		//     : public integral_constant<bool, noexcept(_Tp(declval<_Args>()...))> { };
		//
			
		template <typename T>
		struct is_nothrow_destructible_helper_noexcept_wrapper
			{ static const bool value = noexcept(eastl::declval<T&>().~T()); };

		template <typename T, bool>
		struct is_nothrow_destructible_helper;

		template <typename T>
		struct is_nothrow_destructible_helper<T, false>
			: public eastl::false_type {};

		template <typename T>
		struct is_nothrow_destructible_helper<T, true>     // If the expression T::~T is a noexcept expression then it's nothrow.
			: public eastl::integral_constant<bool, is_nothrow_destructible_helper_noexcept_wrapper<T>::value > {};

		template <typename T>
		struct is_nothrow_destructible                      // A type needs to at least be destructible before it could be nothrow destructible.
			: public eastl::is_nothrow_destructible_helper<T, eastl::is_destructible<T>::value> {};

		template <typename T, size_t N>                     // An array is nothrow destructible if its element type is nothrow destructible.
		struct is_nothrow_destructible<T[N]>                // To consider: Replace this with a remove_all_extents pathway.
			: public eastl::is_nothrow_destructible<T> {};

		template <typename T>
		struct is_nothrow_destructible<T&>                  // A reference type cannot throw while being destructed. It's just a reference.
			: public eastl::true_type {};

		template <typename T>
		struct is_nothrow_destructible<T&&>                 // An rvalue reference type cannot throw while being destructed.
			: public eastl::true_type {};

	#endif

	#define EASTL_DECLARE_IS_NOTHROW_DESTRUCTIBLE(T, isNoThrowDestructible)                                                    \
		namespace eastl{                                                                                                       \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_nothrow_destructible<T>                { static const bool value = isNoThrowDestructible; }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_nothrow_destructible<const T>          { static const bool value = isNoThrowDestructible; }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_nothrow_destructible<volatile T>       { static const bool value = isNoThrowDestructible; }; \
			template <> struct EASTL_REMOVE_AT_2024_APRIL is_nothrow_destructible<const volatile T> { static const bool value = isNoThrowDestructible; }; \
		}

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_nothrow_destructible_v = is_nothrow_destructible<T>::value;
	#endif



	///////////////////////////////////////////////////////////////////////
	// is_nothrow_default_constructible
	//
	///////////////////////////////////////////////////////////////////////
	#define EASTL_TYPE_TRAIT_is_nothrow_default_constructible_CONFORMANCE EASTL_TYPE_TRAIT_is_nothrow_constructible_CONFORMANCE

	template <typename T>
	struct is_nothrow_default_constructible
		: public eastl::is_nothrow_constructible<T> {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_nothrow_move_constructible
	//
	///////////////////////////////////////////////////////////////////////
	#define EASTL_TYPE_TRAIT_is_nothrow_move_constructible_CONFORMANCE EASTL_TYPE_TRAIT_is_nothrow_constructible_CONFORMANCE

	template <typename T>
	struct is_nothrow_move_constructible
	    : public eastl::is_nothrow_constructible<T, typename eastl::add_rvalue_reference<T>::type> {};

    #if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <class T>
		EA_CONSTEXPR bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;
	#endif


} // namespace eastl


#endif // Header include guard
