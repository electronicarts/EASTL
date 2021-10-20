/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_TYPE_COMPOUND_H
#define EASTL_INTERNAL_TYPE_COMPOUND_H


#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


// Until we revise the code below to handle EDG warnings, we don't have much choice but to disable them.
#if defined(__EDG_VERSION__)
	#pragma diag_suppress=1931 // operand of sizeof is not a type, variable, or dereferenced pointer expression
#endif


namespace eastl
{

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

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T, unsigned N = 0> 
		EA_CONSTEXPR auto extent_v = extent<T, N>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_array
	//
	// is_array<T>::value == true if and only if T is an array type, 
	// including unbounded array types.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_array_CONFORMANCE 1    // is_array is conforming; doesn't make mistakes.

	template<typename T>
	struct is_array : public eastl::false_type {};

	template<typename T>
	struct is_array<T[]> : public eastl::true_type {};

	template<typename T, size_t N>
	struct is_array<T[N]> : public eastl::true_type {};

	#if !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		template<typename T>
		EA_CONSTEXPR bool is_array_v = is_array<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_array_of_known_bounds
	//
	// Not part of the C++11 Standard.
	// is_array_of_known_bounds<T>::value is true if T is an array and is 
	// of known bounds. is_array_of_unknown_bounds<int[3]>::value == true,
	// while is_array_of_unknown_bounds<int[]>::value = false.
	// 
	///////////////////////////////////////////////////////////////////////

	template<typename T>
	struct is_array_of_known_bounds
		: public eastl::integral_constant<bool, eastl::extent<T>::value != 0> {};


	///////////////////////////////////////////////////////////////////////
	// is_array_of_unknown_bounds
	//
	// Not part of the C++11 Standard.
	// is_array_of_unknown_bounds<T>::value is true if T is an array but is 
	// of unknown bounds. is_array_of_unknown_bounds<int[3]>::value == false,
	// while is_array_of_unknown_bounds<int[]>::value = true.
	// 
	///////////////////////////////////////////////////////////////////////

	template<typename T>
	struct is_array_of_unknown_bounds
		: public eastl::integral_constant<bool, eastl::is_array<T>::value && (eastl::extent<T>::value == 0)> {};


	///////////////////////////////////////////////////////////////////////
	// is_member_function_pointer
	//
	// is_member_function_pointer<T>::value == true if and only if T is a 
	// pointer to member function type.
	//
	///////////////////////////////////////////////////////////////////////
	// We detect member functions with 0 to N arguments. We can extend this
	// for additional arguments if necessary.
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_member_function_pointer_CONFORMANCE 1    // is_member_function_pointer is conforming; doesn't make mistakes.

	// To do: Revise this to support C++11 variadic templates when possible.
	// To do: We can probably also use remove_cv to simply the multitude of types below.

	template <typename T> struct is_mem_fun_pointer_value : public false_type{};

	template <typename R, typename T> struct is_mem_fun_pointer_value<R (T::*)()> : public true_type{};
	template <typename R, typename T> struct is_mem_fun_pointer_value<R (T::*)() const> : public true_type{};
	template <typename R, typename T> struct is_mem_fun_pointer_value<R (T::*)() volatile> : public true_type{};
	template <typename R, typename T> struct is_mem_fun_pointer_value<R (T::*)() const volatile> : public true_type{};

	template <typename R, typename T, typename Arg0> struct is_mem_fun_pointer_value<R (T::*)(Arg0)> : public true_type{};
	template <typename R, typename T, typename Arg0> struct is_mem_fun_pointer_value<R (T::*)(Arg0) const> : public true_type{};
	template <typename R, typename T, typename Arg0> struct is_mem_fun_pointer_value<R (T::*)(Arg0) volatile> : public true_type{};
	template <typename R, typename T, typename Arg0> struct is_mem_fun_pointer_value<R (T::*)(Arg0) const volatile> : public true_type{};

	template <typename R, typename T, typename Arg0, typename Arg1> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1)> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1) const> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1) volatile> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1) const volatile> : public true_type{};

	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2)> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2) const> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2) volatile> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2) const volatile> : public true_type{};

	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3)> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3) const> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3) volatile> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3) const volatile> : public true_type{};

	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4)> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4) const> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4) volatile> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4) const volatile> : public true_type{};

	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5)> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5) const> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5) volatile> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5) const volatile> : public true_type{};

	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) volatile> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const volatile> : public true_type{};

	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) volatile> : public true_type{};
	template <typename R, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7> struct is_mem_fun_pointer_value<R (T::*)(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const volatile> : public true_type{};

	template <typename T> 
	struct is_member_function_pointer : public integral_constant<bool, is_mem_fun_pointer_value<T>::value>{};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_member_function_pointer_v = is_member_function_pointer<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_member_pointer
	//
	// is_member_pointer<T>::value == true if and only if:
	//    is_member_object_pointer<T>::value == true, or
	//    is_member_function_pointer<T>::value == true
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_member_pointer_CONFORMANCE 1    // is_member_pointer is conforming; doesn't make mistakes.

	template <typename T> 
	struct is_member_pointer 
		: public eastl::integral_constant<bool, eastl::is_member_function_pointer<T>::value>{};

	template <typename T, typename U>
	struct is_member_pointer<U T::*> 
		: public eastl::true_type{};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_member_pointer_v = is_member_pointer<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_member_object_pointer
	//
	// is_member_object_pointer<T>::value == true if and only if T is a 
	// pointer to data member type.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_member_object_pointer_CONFORMANCE 1    // is_member_object_pointer is conforming; doesn't make mistakes.

	template<typename T>
	struct is_member_object_pointer : public eastl::integral_constant<bool,
																	  eastl::is_member_pointer<T>::value &&
																	 !eastl::is_member_function_pointer<T>::value
																	 > {};
	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_member_object_pointer_v = is_member_object_pointer<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_pointer
	//
	// is_pointer<T>::value == true if and only if T is a pointer type. 
	// This category includes function pointer types, but not pointer to 
	// member types.
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_pointer_CONFORMANCE 1    // is_pointer is conforming; doesn't make mistakes.

	template <typename T> struct is_pointer_helper : public false_type{};

	template <typename T> struct is_pointer_helper<T*>                : public true_type{};
	template <typename T> struct is_pointer_helper<T* const>          : public true_type{};
	template <typename T> struct is_pointer_helper<T* volatile>       : public true_type{};
	template <typename T> struct is_pointer_helper<T* const volatile> : public true_type{};

	template <typename T>
	struct is_pointer_value : public type_and<is_pointer_helper<T>::value, type_not<is_member_pointer<T>::value>::value> {};

	template <typename T> 
	struct is_pointer : public integral_constant<bool, is_pointer_value<T>::value>{};

	#if !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		template<typename T>
		EA_CONSTEXPR bool is_pointer_v = is_pointer<T>::value;
	#endif



	///////////////////////////////////////////////////////////////////////
	// is_convertible
	//
	// Given two (possible identical) types From and To, is_convertible<From, To>::value == true 
	// if and only if an lvalue of type From can be implicitly converted to type To, 
	// or is_void<To>::value == true
	// 
	// An instance of the type predicate holds true if the expression To to = from;, where from is an object of type From, is well-formed.
	//
	// is_convertible may only be applied to complete types.
	// Type To may not be an abstract type. 
	// If the conversion is ambiguous, the program is ill-formed. 
	// If either or both of From and To are class types, and the conversion would invoke 
	// non-public member functions of either From or To (such as a private constructor of To, 
	// or a private conversion operator of From), the program is ill-formed.
	//
	// Note that without compiler help, both is_convertible and is_base 
	// can produce compiler errors if the conversion is ambiguous. 
	// Example:
	//    struct A {};
	//    struct B : A {};
	//    struct C : A {};
	//    struct D : B, C {};
	//    is_convertible<D*, A*>::value; // Generates compiler error.
	///////////////////////////////////////////////////////////////////////

	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(_MSC_VER) || (defined(__clang__) && EA_COMPILER_HAS_FEATURE(is_convertible_to)))
		#define EASTL_TYPE_TRAIT_is_convertible_CONFORMANCE 1    // is_convertible is conforming.

		// Problem: VC++ reports that int is convertible to short, yet if you construct a short from an int then VC++ generates a warning:
		//          warning C4242: 'initializing' : conversion from 'int' to 'short', possible loss of data. We can deal with this by making
		//          is_convertible be false for conversions that could result in loss of data. Or we could make another trait called is_lossless_convertible
		//          and use that appropriately in our code. Or we could put the onus on the user to work around such warnings.
		template <typename From, typename To>
		struct is_convertible : public integral_constant<bool, __is_convertible_to(From, To)>{};

	#else
		#define EASTL_TYPE_TRAIT_is_convertible_CONFORMANCE 1

		template<typename From, typename To, bool = eastl::is_void<From>::value || eastl::is_function<To>::value || eastl::is_array<To>::value >
		struct is_convertible_helper // Anything is convertible to void. Nothing is convertible to a function or an array.
			{ static const bool value = eastl::is_void<To>::value; };

		template<typename From, typename To>
		class is_convertible_helper<From, To, false>
		{
			template<typename To1>
			static void ToFunction(To1);    // We try to call this function with an instance of From. It is valid if From can be converted to To.

			template<typename /*From1*/, typename /*To1*/>
			static eastl::no_type is(...);

			template<typename From1, typename To1>
			static decltype(ToFunction<To1>(eastl::declval<From1>()), eastl::yes_type()) is(int);

		public:
			static const bool value = sizeof(is<From, To>(0)) == 1;
		};

		template<typename From, typename To>
		struct is_convertible
			: public integral_constant<bool, is_convertible_helper<From, To>::value> {};

	#endif

	#if !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		template<typename From, typename To>
		EA_CONSTEXPR bool is_convertible_v = is_convertible<From, To>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_nothrow_convertible
	// 
	// https://en.cppreference.com/w/cpp/types/is_convertible
	//
	// template<typename From, typename To>
	// struct is_explicitly_convertible
	//     : public is_constructible<To, From> {};
	///////////////////////////////////////////////////////////////////////
	// TODO(rparolin):  implement type-trait



	///////////////////////////////////////////////////////////////////////
	// is_explicitly_convertible
	// 
	// This sometime-seen extension trait is the same as is_constructible
	// and so we don't define it.
	//
	// template<typename From, typename To>
	// struct is_explicitly_convertible
	//     : public is_constructible<To, From> {};
	///////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////
	// is_union
	//
	// is_union<T>::value == true if and only if T is a union type.
	//
	// There is no way to tell if a type is a union without compiler help.
	// As of this writing, only Metrowerks v8+ supports such functionality
	// via 'msl::is_union<T>::value'. The user can force something to be 
	// evaluated as a union via EASTL_DECLARE_UNION.
	///////////////////////////////////////////////////////////////////////
	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(_MSC_VER) || defined(EA_COMPILER_GNUC) || (defined(__clang__) && EA_COMPILER_HAS_FEATURE(is_union)))
		#define EASTL_TYPE_TRAIT_is_union_CONFORMANCE 1    // is_union is conforming.

		template <typename T> 
		struct is_union : public integral_constant<bool, __is_union(T)>{};
	#else
		#define EASTL_TYPE_TRAIT_is_union_CONFORMANCE 0    // is_union is not fully conforming.

		template <typename T> struct is_union : public false_type{};
	#endif

	#define EASTL_DECLARE_UNION(T) namespace eastl{ template <> struct is_union<T> : public true_type{}; template <> struct is_union<const T> : public true_type{}; }

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_union_v = is_union<T>::value;
	#endif



	///////////////////////////////////////////////////////////////////////
	// is_class
	//
	// is_class<T>::value == true if and only if T is a class or struct 
	// type (and not a union type).
	//
	// Without specific compiler help, it is not possible to 
	// distinguish between unions and classes. As a result, is_class
	// will erroneously evaluate to true for union types.
	///////////////////////////////////////////////////////////////////////
	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(_MSC_VER) || defined(EA_COMPILER_GNUC) || (defined(__clang__) && EA_COMPILER_HAS_FEATURE(is_class)))
		#define EASTL_TYPE_TRAIT_is_class_CONFORMANCE 1    // is_class is conforming.

		template <typename T> 
		struct is_class : public integral_constant<bool, __is_class(T)>{};
	#elif defined(__EDG__)
		#define EASTL_TYPE_TRAIT_is_class_CONFORMANCE   EASTL_TYPE_TRAIT_is_union_CONFORMANCE
	   
		typedef char yes_array_type[1];
		typedef char no_array_type[2];
		template <typename U> static yes_array_type& is_class_helper(void (U::*)());
		template <typename U> static no_array_type& is_class_helper(...);

		template <typename T> 
		struct is_class : public integral_constant<bool,
			sizeof(is_class_helper<T>(0)) == sizeof(yes_array_type) && !is_union<T>::value
		>{};
	#elif !defined(__GNUC__) || (((__GNUC__ * 100) + __GNUC_MINOR__) >= 304) // Not GCC or GCC 3.4+
		#define EASTL_TYPE_TRAIT_is_class_CONFORMANCE   EASTL_TYPE_TRAIT_is_union_CONFORMANCE

		template <typename U> static yes_type is_class_helper(void (U::*)());
		template <typename U> static no_type  is_class_helper(...);

		template <typename T> 
		struct is_class : public integral_constant<bool,
			sizeof(is_class_helper<T>(0)) == sizeof(yes_type) && !is_union<T>::value
		>{};
	#else
		#define EASTL_TYPE_TRAIT_is_class_CONFORMANCE 0    // is_class is not fully conforming.

		// GCC 2.x version, due to GCC being broken.
		template <typename T> 
		struct is_class : public false_type{};
	#endif

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_class_v = is_class<T>::value;
	#endif



	///////////////////////////////////////////////////////////////////////
	// is_polymorphic
	// 
	// is_polymorphic<T>::value == true if and only if T is a class or struct 
	// that declares or inherits a virtual function. is_polymorphic may only 
	// be applied to complete types.
	//
	///////////////////////////////////////////////////////////////////////

	#if EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE && (defined(_MSC_VER) || defined(EA_COMPILER_GNUC) || (defined(__clang__) && EA_COMPILER_HAS_FEATURE(is_polymorphic)))
		#define EASTL_TYPE_TRAIT_is_polymorphic_CONFORMANCE 1    // is_polymorphic is conforming. 

		template <typename T> 
		struct is_polymorphic : public integral_constant<bool, __is_polymorphic(T)>{};
	#else
		#define EASTL_TYPE_TRAIT_is_polymorphic_CONFORMANCE 1    // is_polymorphic is conforming.

		template <typename T>
		struct is_polymorphic_imp1
		{
			typedef typename remove_cv<T>::type t;

			struct helper_1 : public t
			{
				helper_1();
				~helper_1() throw();
				char pad[64];
			};

			struct helper_2 : public t
			{
				helper_2();
				virtual ~helper_2() throw();
				#ifndef _MSC_VER
					virtual void foo();
				#endif
				char pad[64];
			};

			static const bool value = (sizeof(helper_1) == sizeof(helper_2));
		};

		template <typename T>
		struct is_polymorphic_imp2{ static const bool value = false; };

		template <bool is_class>
		struct is_polymorphic_selector{ template <typename T> struct rebind{ typedef is_polymorphic_imp2<T> type; }; };

		template <>
		struct is_polymorphic_selector<true>{ template <typename T> struct rebind{ typedef is_polymorphic_imp1<T> type; }; };

		template <typename T>
		struct is_polymorphic_value{
			typedef is_polymorphic_selector<is_class<T>::value> selector;
			typedef typename selector::template rebind<T> binder;
			typedef typename binder::type imp_type;
			static const bool value = imp_type::value;
		};

		template <typename T> 
		struct is_polymorphic : public integral_constant<bool, is_polymorphic_value<T>::value>{};
	#endif

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_polymorphic_v = is_polymorphic<T>::value;
	#endif




	///////////////////////////////////////////////////////////////////////
	// is_object
	//
	// is_object<T>::value == true if and only if:
	//    is_reference<T>::value == false, and
	//    is_function<T>::value == false, and
	//    is_void<T>::value == false
	//
	// The C++ standard, section 3.9p9, states: "An object type is a
	// (possibly cv-qualified) type that is not a function type, not a 
	// reference type, and not incomplete (except for an incompletely
	// defined object type).
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_object_CONFORMANCE  (EASTL_TYPE_TRAIT_is_reference_CONFORMANCE && EASTL_TYPE_TRAIT_is_void_CONFORMANCE && EASTL_TYPE_TRAIT_is_function_CONFORMANCE)

	template <typename T> 
	struct is_object : public integral_constant<bool,
		!is_reference<T>::value && !is_void<T>::value && !is_function<T>::value
	>{};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_object_v = is_object<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_scalar
	//
	// is_scalar<T>::value == true if and only if:
	//    is_arithmetic<T>::value == true, or
	//    is_enum<T>::value == true, or
	//    is_pointer<T>::value == true, or
	//    is_member_pointer<T>::value == true, or
	//    is_null_pointer<T>::value == true
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_scalar_CONFORMANCE 1    // is_scalar is conforming.

	template <typename T>
	struct is_scalar : public integral_constant<bool,
	                                            is_arithmetic<T>::value || is_enum<T>::value || is_pointer<T>::value ||
	                                                is_member_pointer<T>::value ||
	                                                is_null_pointer<T>::value> {};

	template <typename T> struct is_scalar<T*>                : public true_type {};
	template <typename T> struct is_scalar<T* const>          : public true_type {};
	template <typename T> struct is_scalar<T* volatile>       : public true_type {};
	template <typename T> struct is_scalar<T* const volatile> : public true_type {};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_scalar_v = is_scalar<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_compound
	//
	// Compound means anything but fundamental. See C++ standard, section 3.9.2.
	//
	// is_compound<T>::value == true if and only if:
	//    is_fundamental<T>::value == false
	//
	// Thus, is_compound<T>::value == true if and only if:
	//    is_floating_point<T>::value == false, and
	//    is_integral<T>::value == false, and
	//    is_void<T>::value == false
	//
	///////////////////////////////////////////////////////////////////////

	#define EASTL_TYPE_TRAIT_is_compound_CONFORMANCE  EASTL_TYPE_TRAIT_is_fundamental_CONFORMANCE

	template <typename T> 
	struct is_compound : public integral_constant<bool, !is_fundamental<T>::value>{};

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_compound_v = is_compound<T>::value;
	#endif



	///////////////////////////////////////////////////////////////////////
	// decay
	//
	// Converts the type T to its decayed equivalent. That means doing 
	// lvalue to rvalue, array to pointer, function to pointer conversions,
	// and removal of const and volatile.
	// This is the type conversion silently applied by the compiler to 
	// all function arguments when passed by value. 

	#define EASTL_TYPE_TRAIT_decay_CONFORMANCE 1    // decay is conforming.

	template<typename T>
	struct decay
	{
		typedef typename eastl::remove_reference<T>::type U;

		typedef typename eastl::conditional< 
			eastl::is_array<U>::value,
			typename eastl::remove_extent<U>::type*,
			typename eastl::conditional< 
				eastl::is_function<U>::value,
				typename eastl::add_pointer<U>::type,
				typename eastl::remove_cv<U>::type
			>::type
		>::type type;
	};


	// decay_t is the C++14 using typedef for typename decay<T>::type, though
	// it requires only C++11 compiler functionality to implement.
	// We provide a backwards-compatible means to access it through a macro for pre-C++11 compilers.
	#if defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		#define EASTL_DECAY_T(T) typename decay<T>::type
	#else
		template<typename T>
		using decay_t = typename decay<T>::type;
		#define EASTL_DECAY_T(T) decay_t<T>
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

	#define EASTL_TYPE_TRAIT_common_type_CONFORMANCE 1    // common_type is conforming.

	template<typename... T>
	struct common_type;

	template<typename T>
	struct common_type<T>
		{ typedef decay_t<T> type; }; // Question: Should we use T or decay_t<T> here? The C++11 Standard specifically (20.9.7.6,p3) specifies that it be without decay, but libc++ uses decay.

	template<typename T, typename U>
	struct common_type<T, U>
	{
		typedef decay_t<decltype(true ? declval<T>() : declval<U>())> type; // The type of a tertiary expression is set by the compiler to be the common type of the two result types.
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

	///////////////////////////////////////////////////////////////////////
	// is_final
	///////////////////////////////////////////////////////////////////////
	#if EA_COMPILER_HAS_FEATURE(is_final)
		template <typename T>
		struct is_final : public integral_constant<bool, __is_final(T)> {};
	#else
		// no compiler support so we always return false
		template <typename T>
		struct is_final : public false_type {};
	#endif

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template<typename T>
		EA_CONSTEXPR bool is_final_v = is_final<T>::value;
	#endif


	///////////////////////////////////////////////////////////////////////
	// is_aggregate
	//
	// https://en.cppreference.com/w/cpp/language/aggregate_initialization
	//
	// An aggregate is one of the following types:
	// * array type
	// * class type (typically, struct or union), that has
	//     * no private or protected non-static data members
	//     * no user-provided constructors (explicitly defaulted or deleted constructors are allowed)
	//     * no user-provided, inherited, or explicit constructors 
	//         * (explicitly defaulted or deleted constructors are allowed)
	//     * no virtual, private, or protected (since C++17) base classes
	//     * no virtual member functions
	//     * no default member initializers
	//
	///////////////////////////////////////////////////////////////////////
	#if EA_COMPILER_HAS_FEATURE(is_aggregate) || defined(_MSC_VER) && (_MSC_VER >= 1916)  // VS2017 15.9+
		#define EASTL_TYPE_TRAIT_is_aggregate_CONFORMANCE 1  

		template <typename T>
		struct is_aggregate : public integral_constant<bool, __is_aggregate(T)> {};
	#else
		#define EASTL_TYPE_TRAIT_is_aggregate_CONFORMANCE 0 

		// no compiler support so we always return false
		template <typename T>
		struct is_aggregate : public false_type {};
	#endif

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		template <typename T>
		EA_CONSTEXPR bool is_aggregate_v = is_aggregate<T>::value;
	#endif
} // namespace eastl


#endif // Header include guard




