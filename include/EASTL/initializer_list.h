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


#if defined(EA_HAVE_CPP11_INITIALIZER_LIST) // If the compiler can generate calls to std::initializer_list...

	// The initializer_list type must be declared in the std namespace, as that's the 
	// namespace the compiler uses when generating code to use it.
	EA_DISABLE_ALL_VC_WARNINGS()
	#include <initializer_list>
	EA_RESTORE_ALL_VC_WARNINGS()

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















