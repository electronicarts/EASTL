/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_INTERNAL_MEMORY_BASE_H
#define EASTL_INTERNAL_MEMORY_BASE_H

#include <EASTL/internal/config.h>

EA_ONCE()


////////////////////////////////////////////////////////////////////////////////////////////
// This file contains basic functionality found in the standard library 'memory' header that
// have limited or no dependencies.  This allows us to utilize these utilize these functions
// in other EASTL code while avoid circular dependencies.
////////////////////////////////////////////////////////////////////////////////////////////

namespace eastl
{
	/// addressof
	///
	/// From the C++11 Standard, section 20.6.12.1
	/// Returns the actual address of the object or function referenced by r, even in the presence of an overloaded operator&.
	///
	template<typename T>
	T* addressof(T& value) EA_NOEXCEPT
	{
		return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(value)));
	}

} // namespace eastl

#endif // EASTL_INTERNAL_MEMORY_BASE_H

