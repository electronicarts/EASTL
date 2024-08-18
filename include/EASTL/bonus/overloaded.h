/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_OVERLOADED_H
#define EASTL_OVERLOADED_H

#include <EASTL/internal/move_help.h>
#include <EASTL/type_traits.h>


#if defined(EA_PRAGMA_ONCE_SUPPORTED)
#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed
			 // improvements in apps as a result.
#endif

// 4512/4626 - 'class' : assignment operator could not be generated.  // This disabling would best be put elsewhere.
EA_DISABLE_VC_WARNING(4512 4626);

namespace eastl
{
	///////////////////////////////////////////////////////////////////////////
	/// overloaded
	/// 
	/// A helper class that permits you to combine multiple function objects into one.
	/// Typically, this helper is really handy when visiting an eastl::variant with multiple lambdas.
	/// Example:
	///
	/// eastl::variant<int, string> v{42};
	/// 
	/// eastl::visit(
	///	 eastl::overloaded{
	///		  [](const int& x) { std::cout << "Visited an integer: " << x  << "\n"; }, // Will reach that lambda with x == 42.
	///		  [](const string& s) { std::cout << "Visited an string: " << s  << "\n"; }
	///	 },
	///	 v
	/// );
	///////////////////////////////////////////////////////////////////////////
	template <class... T>
	struct overloaded;

	template <class T>
	struct overloaded<T> : T
	{
		template <class U>
		EA_CPP14_CONSTEXPR overloaded(U&& u) : T(eastl::forward<U>(u))
		{
		}

		using T::operator();
	};

	template <class T, class... R>
	struct overloaded<T, R...> : T, overloaded<R...>
	{
		template <class U, class... V>
		EA_CPP14_CONSTEXPR overloaded(U&& u, V&&... v) : T(eastl::forward<U>(u)), overloaded<R...>(eastl::forward<V>(v)...)
		{
		}

		using T::operator();
		using overloaded<R...>::operator();
	};

	#ifdef __cpp_deduction_guides
	template <class... T>
	overloaded(T...) -> overloaded<T...>;
	#endif

	///////////////////////////////////////////////////////////////////////////
	/// make_overloaded
	///
	/// Helper function to create an overloaded instance when lacking deduction guides.
	/// make_overloaded(f1, f2, f3) == overloaded{f1, f2, f3}
	///////////////////////////////////////////////////////////////////////////
	template <class... T>
	EA_CPP14_CONSTEXPR overloaded<typename eastl::remove_cvref<T>::type...> make_overloaded(T&&... t)
	{
		return overloaded<typename eastl::remove_cvref<T>::type...>{eastl::forward<T>(t)...};
	}

} // namespace eastl

EA_RESTORE_VC_WARNING();

#endif // EASTL_OVERLOADED_H
