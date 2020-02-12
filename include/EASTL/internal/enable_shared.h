/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_ENABLE_SHARED_H
#define EASTL_INTERNAL_ENABLE_SHARED_H


#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

namespace eastl
{

	/// enable_shared_from_this
	///
	/// This is a helper mixin class that allows you to make any class
	/// export a shared_ptr instance that is associated with the class 
	/// instance. Any class that inherits from this class gets two functions:
	///    shared_ptr<T> shared_from_this();
	///    shared_ptr<T> shared_from_this() const;
	/// If you call shared_from_this, you get back a shared_ptr that 
	/// refers to the class. A second call to shared_from_this returns
	/// another shared_ptr that is shared with the first one. 
	///
	/// The trick that happens which is not so obvious here (and which is
	/// not mentioned at all in the Boost documentation of their version
	/// of this) is that the shared_ptr constructor detects that the 
	/// class has an enable_shared_from_this mixin and sets up this system
	/// automatically for the user. This is done with template tricks.
	///
	/// For some additional explanation, see the Boost documentation for 
	/// their description of their version of enable_shared_from_this.
	///
	template <typename T>
	class enable_shared_from_this
	{
	public:
		shared_ptr<T> shared_from_this()
			{ return shared_ptr<T>(mWeakPtr); }

		shared_ptr<const T> shared_from_this() const
			{ return shared_ptr<const T>(mWeakPtr); }

		weak_ptr<T> weak_from_this()
			{ return mWeakPtr; }

		weak_ptr<const T> weak_from_this() const
			{ return mWeakPtr; }

	public: // This is public because the alternative fails on some compilers that we need to support.
		mutable weak_ptr<T> mWeakPtr;

	protected:
		template <typename U> friend class shared_ptr;

		EA_CONSTEXPR enable_shared_from_this() EA_NOEXCEPT
			{ }

		enable_shared_from_this(const enable_shared_from_this&) EA_NOEXCEPT
			{ }

		enable_shared_from_this& operator=(const enable_shared_from_this&) EA_NOEXCEPT
			{ return *this; }

		~enable_shared_from_this()
			{ }

	}; // enable_shared_from_this

} // namespace eastl


#endif // Header include guard






