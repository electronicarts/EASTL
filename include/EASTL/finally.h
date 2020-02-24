/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// eastl::finally is an implementation of the popular cpp idiom RAII - Resource
// Acquisition Is Initialization. eastl::finally guarantees that the user
// provided callable will be executed upon whatever mechanism is used to leave
// the current scope. This can guard against user errors but this is a popular
// technique to write robust code in execution environments that have exceptions
// enabled.
//
// Example:
//     void foo()
//     {
//         void* p = malloc(128);
//         auto _ = eastl::make_finally([&] { free(p); });
//
//         // Code that may throw an exception...
//         
//     }  // eastl::finally guaranteed to call 'free' at scope exit.
//
// References:
// * https://www.bfilipek.com/2017/04/finalact.html
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_FINALLY_H
#define EASTL_FINALLY_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#include <EASTL/internal/config.h>
#include <EASTL/internal/move_help.h>
#include <EASTL/type_traits.h>

namespace eastl
{
	///////////////////////////////////////////////////////////////////////////
	// finally
	//
	// finally is the type that calls the users callback on scope exit.
	//
	template <typename Functor>
	class finally
	{
		static_assert(!eastl::is_lvalue_reference_v<Functor>, "eastl::finally requires the callable is passed as an rvalue reference.");

		Functor m_functor;
		bool m_engaged = false;

	public:
		finally(Functor f) : m_functor(eastl::move(f)), m_engaged(true) {}

		finally(finally&& other) : m_functor(eastl::move(other.m_functor)), m_engaged(other.m_engaged)
		{
			other.dismiss();
		}

		~finally() { execute(); }

		finally(const finally&) = delete;
		finally& operator=(const finally&) = delete;
		finally& operator=(finally&&) = delete;

		inline void dismiss() { m_engaged = false; }

		inline void execute()
		{
			if (m_engaged)
				m_functor();

			dismiss();
		}
	};


	///////////////////////////////////////////////////////////////////////////
	// make_finally
	//
	// this utility function is the standard mechansim to perform the required
	// type deduction on the users provided callback inorder to create a
	// 'finally' object.
	//
	template <typename F>
	auto make_finally(F&& f)
	{
		return finally<F>(eastl::forward<F>(f));
	}
}

#endif // EASTL_FINALLY_H
