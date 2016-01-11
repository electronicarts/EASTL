///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
//
// This file #includes <initializer_list> if it's available, else it defines 
// its own version of std::initializer_list. It does not define eastl::initializer_list
// because that would not provide any use, due to how the C++11 Standard works.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INITIALIZER_LIST_H
#define EASTL_INITIALIZER_LIST_H


#include <EASTL/internal/config.h>
#include <EABase/eahave.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif

// Older EABase versions have a bug in EA_HAVE_CPP11_INITIALIZER_LIST for the 
// combination of clang/libstdc++, so we implement a fix for that here.
#if (EABASE_VERSION_N < 20042) // If using a version of EABase that has a bug or doesn't have support at all...
	#if defined(EA_HAVE_CPP11_INITIALIZER_LIST)
		#undef EA_HAVE_CPP11_INITIALIZER_LIST
	#endif
	#if defined(EA_NO_HAVE_CPP11_INITIALIZER_LIST)
		#undef EA_NO_HAVE_CPP11_INITIALIZER_LIST
	#endif

	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 520) && !defined(EA_COMPILER_NO_INITIALIZER_LISTS) // Dinkumware. VS2010+
		#define EA_HAVE_CPP11_INITIALIZER_LIST 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_CLANG) && (EA_COMPILER_VERSION >= 301) && !defined(EA_COMPILER_NO_INITIALIZER_LISTS) && !defined(EA_PLATFORM_APPLE)
		#define EA_HAVE_CPP11_INITIALIZER_LIST 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4004) && !defined(EA_COMPILER_NO_INITIALIZER_LISTS) && !defined(EA_PLATFORM_APPLE)
		#define EA_HAVE_CPP11_INITIALIZER_LIST 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1) && !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
		#define EA_HAVE_CPP11_INITIALIZER_LIST 1
	#else
		#define EA_NO_HAVE_CPP11_INITIALIZER_LIST 1
	#endif

#endif


#if defined(EA_HAVE_CPP11_INITIALIZER_LIST) // If the compiler can generate calls to std::initializer_list...

	// The initializer_list type must be declared in the std namespace, as that's the 
	// namespace the compiler uses when generating code to use it.
	#ifdef _MSC_VER
		#pragma warning(push, 0)
	#endif
	#include <initializer_list>
	#ifdef _MSC_VER
		#pragma warning(pop)
	#endif

#else

	// If you get an error here about initializer_list being already defined, then the EA_HAVE_CPP11_INITIALIZER_LIST define from <EABase/eahave.h> needs to be updated.
	namespace std
	{
		// See the C++11 Standard, section 18.9.
		template<class E> 
		class initializer_list
		{
		public:
			typedef E         value_type;
			typedef const E&  reference;
			typedef const E&  const_reference;
			typedef size_t    size_type;
			typedef const E*  iterator;             // Must be const, as initializer_list (and its mpArray) is an immutable temp object.
			typedef const E*  const_iterator;

		private:
			iterator  mpArray;
			size_type mArraySize;

			// This constructor is private, but the C++ compiler has the ability to call it, as per the C++11 Standard.
			initializer_list(const_iterator pArray, size_type arraySize)
			  : mpArray(pArray), mArraySize(arraySize) { }

		public:
			initializer_list() EA_NOEXCEPT  // EA_NOEXCEPT requires a recent version of EABase.  
			  : mpArray(NULL), mArraySize(0) { }

			size_type      size()  const EA_NOEXCEPT { return mArraySize; }
			const_iterator begin() const EA_NOEXCEPT { return mpArray; }            // Must be const_iterator, as initializer_list (and its mpArray) is an immutable temp object.
			const_iterator end()   const EA_NOEXCEPT { return mpArray + mArraySize; }
		};


		template<class T>
		const T* begin(std::initializer_list<T> ilist) EA_NOEXCEPT
		{
			return ilist.begin();
		}

		template<class T>
		const T* end(std::initializer_list<T> ilist) EA_NOEXCEPT
		{
			return ilist.end();
		}
	}

#endif


#endif // Header include guard















