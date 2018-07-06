/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_INTERNAL_FUNCTION_HELP_H
#define EASTL_INTERNAL_FUNCTION_HELP_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
#pragma once
#endif

#include <EASTL/internal/config.h>

namespace eastl
{
	template<typename>
	class function;

	namespace internal
	{
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// to_functor
		//
		template <typename T>
		T to_functor(T&& func)
		{
			return EASTL_FORWARD(T, func);
		}

		template <typename Result, typename Class, typename... Arguments>
		auto to_functor(Result (Class::*func)(Arguments...)) -> decltype(eastl::mem_fn(func))
		{
			return eastl::mem_fn(func);
		}

		template <typename Result, typename Class, typename... Arguments>
		auto to_functor(Result (Class::*func)(Arguments...) const) -> decltype(eastl::mem_fn(func))
		{
			return eastl::mem_fn(func);
		}


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// is_null
		//
		template <typename T>
		bool is_null(const T&)
		{
			return false;
		}

		template <typename Result, typename... Arguments>
		bool is_null(Result (*const& function_pointer)(Arguments...))
		{
			return function_pointer == nullptr;
		}

		template <typename Result, typename Class, typename... Arguments>
		bool is_null(Result (Class::*const& function_pointer)(Arguments...))
		{
			return function_pointer == nullptr;
		}

		template <typename Result, typename Class, typename... Arguments>
		bool is_null(Result (Class::*const& function_pointer)(Arguments...) const)
		{
			return function_pointer == nullptr;
		}


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// is_valid_function_argument
		//
		template <typename, typename>
		struct is_valid_function_argument
		{
			static const bool value = false;
		};

		template <typename Result, typename... Arguments>
		struct is_valid_function_argument<function<Result(Arguments...)>, Result(Arguments...)>
		{
			static const bool value = false;
		};

		template <typename T, typename Result, typename... Arguments>
		struct is_valid_function_argument<T, Result(Arguments...)>
		{
			struct local_empty_struct {}; // a type that no user function will use as a return type

			template <typename U>
			static decltype(to_functor(eastl::declval<U>())(eastl::declval<Arguments>()...)) local_check(U*);

			template <typename>
			static local_empty_struct local_check(...);

			static const bool value = eastl::is_convertible<decltype(local_check<T>(0)), Result>::value;
		};
	} // namespace internal
} // namespace eastl

#endif // Header include guard

