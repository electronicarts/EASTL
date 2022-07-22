/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_TYPE_DETECTED_H
#define EASTL_INTERNAL_TYPE_DETECTED_H


#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
#pragma once
#endif

#include <EASTL/type_traits.h>

namespace eastl
{
	///////////////////////////////////////////////////////////////////////
	// nonesuch
	//
	// Type given as a result from detected_t if the supplied arguments does not respect the constraint.
	//
	// https://en.cppreference.com/w/cpp/experimental/nonesuch
	//
	///////////////////////////////////////////////////////////////////////
	struct nonesuch
	{
		~nonesuch() = delete;
		nonesuch(nonesuch const&) = delete;
		void operator=(nonesuch const&) = delete;
	};

	namespace internal
	{
		template <class Default, class AlwaysVoid, template <class...> class Op, class... Args>
		struct detector
		{
			using type = Default;
			using value_t = false_type;
		};

		template <class Default, template <class...> class Op, class... Args>
		struct detector<Default, void_t<Op<Args...>>, Op, Args...>
		{
			using type = Op<Args...>;
			using value_t = true_type;
		};
	} // namespace internal

	///////////////////////////////////////////////////////////////////////
	// is_detected
	//
	// Checks if some supplied arguments (Args) respect a constraint (Op).
	// is_detected expands to true_type if the arguments respect the constraint, false_type otherwise.
	// This helper is convenient to use for compile time introspection.
	//
	// https://en.cppreference.com/w/cpp/experimental/is_detected
	//
	// Example:
	// template <class T, class U>
	// using detect_can_use_addition_operator = decltype(declval<T>() + declval<U>());
	//
	// template <class T, class U>
	// void sum(const T& t, const U& u)
	// {
	// 	    static_assert(is_detected<detect_can_use_addition_operator, T, U>::value, "Supplied types cannot be summedtogether.");
	//      // or...
	// 	    static_assert(is_detected_v<detect_can_use_addition_operator, T, U>, "Supplied types cannot be summedtogether.");
	//      return t + u;
	// }
	//
	///////////////////////////////////////////////////////////////////////
	template <template <class...> class Op, class... Args>
	using is_detected = typename internal::detector<nonesuch, void, Op, Args...>::value_t;

#if EASTL_VARIABLE_TEMPLATES_ENABLED
	template <template <class...> class Op, class... Args>
	EA_CONSTEXPR bool is_detected_v = is_detected<Op, Args...>::value;
#endif

	///////////////////////////////////////////////////////////////////////
	// detected_t
	//
	// Check which type we obtain after expanding some arguments (Args) over a constraint (Op).
	// If the constraint cannot be applied, the result type will be nonesuch.
	//
	// https://en.cppreference.com/w/cpp/experimental/is_detected
	//
	// Example:
	// template <class T, class U>
	// using detect_can_use_addition_operator = decltype(declval<T>() + declval<U>());
	//
	// using result_type = detected_t<detect_can_use_addition_operator, int, int>;
	// // result_type == int
	// using failed_result_type = detected_t<detect_can_use_addition_operator, int, string>;
	// // failed_result_type == nonesuch
	//
	///////////////////////////////////////////////////////////////////////
	template <template <class...> class Op, class... Args>
	using detected_t = typename internal::detector<nonesuch, void, Op, Args...>::type;

	///////////////////////////////////////////////////////////////////////
	// detected_or
	//
	// Checks if some supplied arguments (Args) respect a constraint (Op).
	// Expand to a struct that contains two type aliases:
	// - type: the type we obtain after expanding some arguments (Args) over a constraint (Op).
	// If the constraint cannot be applied, the result type will be the suplied Default type.
	// - value_t: true_type if the arguments respect the constraint, false_type otherwise.
	//
	// https://en.cppreference.com/w/cpp/experimental/is_detected
	//
	// Example:
	// template <class T, class U>
	// using detected_calling_foo = decltype(declval<T>().foo());
	//
	// using result = detected_or<bool, detected_calling_foo, std::string>; // std::string doesn't have foo member.
	// function.
	// // result::type == bool
	// // result::value_t == false_type
	//
	///////////////////////////////////////////////////////////////////////
	template <class Default, template <class...> class Op, class... Args>
	using detected_or = internal::detector<Default, void, Op, Args...>;

	///////////////////////////////////////////////////////////////////////
	// detected_or_t
	//
	// Equivalent to detected_or<Default, Op, Args...>::type.
	//
	///////////////////////////////////////////////////////////////////////
	template <class Default, template <class...> class Op, class... Args>
	using detected_or_t = typename detected_or<Default, Op, Args...>::type;

	///////////////////////////////////////////////////////////////////////
	// is_detected_exact
	//
	// Check that the type we obtain after expanding some arguments (Args) over a constraint (Op) is equivalent to
	// Expected.
	//
	// template <class T, class U>
	// using detected_calling_size = decltype(declval<T>().size());
	//
	// using result = is_detected_exact<int, detected_calling_size, std::string>;
	// result == false_type // std::string::size returns eastl_size_t which is not the same as int.
	//
	///////////////////////////////////////////////////////////////////////
	template <class Expected, template <class...> class Op, class... Args>
	using is_detected_exact = is_same<Expected, detected_t<Op, Args...>>;

#if EASTL_VARIABLE_TEMPLATES_ENABLED
	template <class Expected, template <class...> class Op, class... Args>
	EA_CONSTEXPR bool is_detected_exact_v = is_detected_exact<Expected, Op, Args...>::value;
#endif

	///////////////////////////////////////////////////////////////////////
	// is_detected_convertible
	//
	// Check that the type we obtain after expanding some arguments (Args) over a constraint (Op) is convertible to
	// Expected.
	//
	// template <class T, class U>
	// using detected_calling_size = decltype(declval<T>().size());
	//
	// using result = is_detected_convertible<int, detected_calling_size, std::string>;
	// result == true_type // std::string::size returns eastl_size_t which is convertible to int.
	//
	///////////////////////////////////////////////////////////////////////
	template <class To, template <class...> class Op, class... Args>
	using is_detected_convertible = is_convertible<detected_t<Op, Args...>, To>;

#if EASTL_VARIABLE_TEMPLATES_ENABLED
	template <class To, template <class...> class Op, class... Args>
	EA_CONSTEXPR bool is_detected_convertible_v = is_detected_convertible<To, Op, Args...>::value;
#endif

} // namespace eastl

#endif // EASTL_INTERNAL_TYPE_DETECTED_H