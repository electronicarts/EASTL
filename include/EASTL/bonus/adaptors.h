/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ADAPTORS_H
#define EASTL_ADAPTORS_H


#include <EASTL/internal/config.h>
#include <EASTL/internal/move_help.h>
#include <EASTL/type_traits.h>
#include <EASTL/iterator.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif

EA_DISABLE_VC_WARNING(4512 4626)
#if defined(_MSC_VER) && (_MSC_VER >= 1900) // VS2015+
	EA_DISABLE_VC_WARNING(5027) // move assignment operator was implicitly defined as deleted
#endif


namespace eastl
{
	/// reverse
	///
	/// This adaptor allows reverse iteration of a container in ranged base for-loops.
	///
	/// for (auto& i : reverse(c)) { ... }
	///
	template <typename Container>
	struct reverse_wrapper
	{
		template <typename C>
		reverse_wrapper(C&& c)
			: mContainer(eastl::forward<C>(c))
		{
			/**
			 * NOTE:
			 *
			 * Due to reference collapsing rules of universal references Container type is either
			 *
			 * const C&  if the input is a const lvalue
			 * C&        if the input is a non-const lvalue
			 * C         if the input is an rvalue
			 * const C   if the input is a const rvalue thus the object will have to be copied and the copy-ctor will be called
			 *
			 *
			 * Thus we either move the whole container into this object or take a reference to the lvalue avoiding the copy.
			 * The static_assert below ensures this.
			 */
			static_assert(eastl::is_same_v<C, Container>, "Reference collapsed deduced type must be the same as the deduced Container type!");
		}

		Container mContainer;
	};

	template <typename Container>
	auto begin(const reverse_wrapper<Container>& w) -> decltype(eastl::rbegin(w.mContainer))
	{
		return eastl::rbegin(w.mContainer);
	}

	template <typename Container>
	auto end(const reverse_wrapper<Container>& w) -> decltype(eastl::rend(w.mContainer))
	{
		return eastl::rend(w.mContainer);
	}

	template <typename Container>
	reverse_wrapper<Container> reverse(Container&& c)
	{
		return reverse_wrapper<Container>(eastl::forward<Container>(c));
	}

} // namespace eastl

#if defined(_MSC_VER) && (_MSC_VER >= 1900) // VS2015+
	EA_RESTORE_VC_WARNING()
#endif
EA_RESTORE_VC_WARNING()

#endif // Header include guard
