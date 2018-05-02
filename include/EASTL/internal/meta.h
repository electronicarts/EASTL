/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_INTERNAL_META_H
#define EASTL_INTERNAL_META_H

#include <EASTL/internal/config.h>
#include <EASTL/type_traits.h>

EA_ONCE()

////////////////////////////////////////////////////////////////////////////////////////////
// This file contains meta programming utilities that are internal to EASTL. We reserve
// the right to change this file at any time as it is only intended to be used internally.
////////////////////////////////////////////////////////////////////////////////////////////

namespace eastl
{
	namespace meta
	{
		////////////////////////////////////////////////////////////////////////////////////////////
		// get_type_index_v
		//
		// Linearly searches a typelist using compile-time recursion to inspect each T in
		// the typelist and returns its index, if the type is found.  If the T isn't found
		// in the typelist -1 is returned.
		//
		namespace Internal
		{
			template <int I, typename T, typename... Types>
			struct get_type_index;

			template <int I, typename T, typename Head, typename... Types>
			struct get_type_index<I, T, Head, Types...>
			{
				static const int value = is_same_v<T, Head> ? I : get_type_index<I + 1, T, Types...>::value;
			};

			template <int I, typename T>
			struct get_type_index<I, T>
			{
				static const int value = -1;
			};
		}

		template<typename T, typename... Types>
		struct get_type_index
		{
			static const int value = Internal::get_type_index<0, T, Types...>::value;
		};

		template <typename T, typename... Ts>
		constexpr int get_type_index_v = get_type_index<T, Ts...>::value;


		////////////////////////////////////////////////////////////////////////////////////////////
		// get_type_at
		//
		// This traverses the variadic type list and retrieves the type at the user provided index.
		//
		template <size_t I, typename... Ts>
			struct get_type_at_helper;

		template <size_t I, typename Head, typename... Tail>
			struct get_type_at_helper<I, Head, Tail...>
			{ typedef typename get_type_at_helper<I - 1, Tail...>::type type; };

		template <typename Head, typename... Tail>
			struct get_type_at_helper<0, Head, Tail...>
			{ typedef Head type; };

		template <int I, typename... Ts>
		using get_type_at_t = typename get_type_at_helper<I, Ts...>::type;


		////////////////////////////////////////////////////////////////////////////////////////////
		// type_count_v 
		//
		// Returns the number of occurrences of type T in a typelist.
		//
		template <typename T, typename... Types>
		struct type_count;

		template <typename T, typename H, typename... Types>
		struct type_count<T, H, Types...>
		{
			static const int value = (is_same_v<T, H> ? 1 : 0) + type_count<T, Types...>::value;
		};

		template <typename T> struct type_count<T> { static const int value = 0; };

		template <typename T, typename... Ts>
		constexpr int type_count_v = type_count<T, Ts...>::value;



		////////////////////////////////////////////////////////////////////////////////////////////
		// duplicate_type_check_v
		// 
		// Checks if a type T occurs in a typelist more than once.
		//
		template <typename T, typename... Types>
		struct duplicate_type_check
		{
			static const bool value = (type_count<T, Types...>::value == 1);
		};

		template <typename... Ts>
		constexpr bool duplicate_type_check_v = duplicate_type_check<Ts...>::value;

		//////////////////////////////////////////////////////////////////////////////////
		// type_list, unique_type_list
		//
		// type_list is a simple struct that allows us to pass template parameter packs
		// around in a single struct, and deduce parameter packs from function arguments
		// like so:
		//
		//     template <typename... Ts> void foo(type_list<Ts...>);
		//     foo(type_list<A, B, C>); // deduces Ts... as A, B, C
		//
		// unique_type_list is a meta-function which takes a parameter pack as its
		// argument, and returns a type_list with duplicate types removed, like so:
		//
		//    unique_type_list<int, int, string>::type; // type_list<int, string>
		//    unique_type_list<int, string, string>::type; // type_list<int, string>
		//
		// To use unique_type_list, specialize a variadic class template for a single
		// type parameter, which is type_list<Ts...>:
		//
		//    template <typename... Ts> struct foo {};
		//    template <typename... Ts> struct foo<type_list<Ts...>> {};
		//
		// Then instantiate the template with unique_type_list_t<Ts...> as its parameter:
		//
		//    template <typename... Ts> struct bar : public foo<unique_type_list_t<Ts...>> {}
		//
		// See overload_set below for examples.
		template <typename...> struct type_list {};

		template <typename T, typename... Ts>
		struct unique_type_list : public unique_type_list<Ts...>
		{
			template <typename... Args>
			static enable_if_t<!disjunction_v<is_same<T, Args>...>, type_list<T, Args...>>
			types(type_list<Args...>);

			template <typename... Args>
			static enable_if_t<disjunction_v<is_same<T, Args>...>, type_list<Args...>>
			types(type_list<Args...>);

			typedef decltype(types(declval<typename unique_type_list<Ts...>::type>())) type;
		};

		template <typename T>
		struct unique_type_list<T>
		{
			using type = type_list<T>;
		};

		template <typename... Ts>
		using unique_type_list_t = typename unique_type_list<Ts...>::type;

		////////////////////////////////////////////////////////////////////////////////////////////
		// overload_resolution_t 
		// 
		// Given an input type and a typelist (which is a stand-in for alternative
		// function overloads) this traits will return the same type chosen as if
		// overload_resolution has selected a function to run.
		//

		// a single overload of an individual type
		template <typename T>
		struct overload
		{
			// Overload is implicitly convertible to the surrogated function
			// call for pointer to member functions (pmf). This gets around
			// variadic pack expansion in a class using statement being a C++17
			// language feature. It is the core mechanism of aggregating all the
			// individual overloads into the overload_set structure.
			using F = T (*)(T);
			operator F() const { return nullptr; }
		};

		template <typename...> struct overload_set_impl;

		template <typename... Ts>
		struct overload_set_impl<type_list<Ts...>> : public overload<Ts>... {};

		template <typename... Ts>
		struct overload_set : public overload_set_impl<unique_type_list_t<Ts...>>
		{
			// encapsulates the overloads matching the types of the variadic pack
		};

		EA_DISABLE_VC_WARNING(4242 4244) // conversion from 'T' to 'U', possible loss of data.
		template <typename T, typename OverloadSet, typename ResultT = decltype(declval<OverloadSet>()(declval<T>()))>
		struct overload_resolution
		{
			// capture the return type of the function the compiler selected by
			// performing overload resolution on the overload set parameter
			using type = ResultT;
		};

		EA_RESTORE_VC_WARNING()

		template <typename T, typename OverloadSet>
		using overload_resolution_t = typename overload_resolution<decay_t<T>, OverloadSet>::type;

	} // namespace meta 
} // namespace eastl

#endif // EASTL_INTERNAL_META_H

