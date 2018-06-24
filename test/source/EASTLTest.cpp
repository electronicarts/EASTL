/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <EASTL/version.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <new>
#if !defined(EA_COMPILER_NO_STANDARD_CPP_LIBRARY)
	#include <vector> // Used to detect the C++ Standard Library version.
#endif

#ifndef EA_PLATFORM_PLAYSTSATION2
	#include <wchar.h>
#endif
#if defined(EA_PLATFORM_WINDOWS)
	#include <Windows.h>
#elif defined(EA_PLATFORM_ANDROID)
	#include <android/log.h>
#endif
#if defined(_MSC_VER) && defined(EA_PLATFORM_MICROSOFT)
	#include <crtdbg.h>
#endif


#ifdef _MSC_VER
	#pragma warning(pop)
#endif


#include "EASTLTestAllocator.h"
#include "EASTLTest.h"  // Include this last, as it enables compiler warnings.

///////////////////////////////////////////////////////////////////////////////
// init_seg
//
#ifdef _MSC_VER
	// Set initialization order between init_seg(compiler) (.CRT$XCC) and
	// init_seg(lib) (.CRT$XCL). The linker sorts the .CRT sections
	// alphabetically so we simply need to pick a name that is between
	// XCC and XCL.
	#pragma warning(disable: 4075) // warning C4075: initializers put in unrecognized initialization area
	#pragma init_seg(".CRT$XCF")
#endif


///////////////////////////////////////////////////////////////////////////////
// EA_INIT_PRIORITY
//
// This is simply a wrapper for the GCC init_priority attribute that allows 
// multiplatform code to be easier to read. 
//
// Example usage:
//     SomeClass gSomeClass EA_INIT_PRIORITY(2000);
//
#if !defined(EA_INIT_PRIORITY)
	#if defined(__GNUC__)
		#define EA_INIT_PRIORITY(x)  __attribute__ ((init_priority (x)))
	#else
		#define EA_INIT_PRIORITY(x)
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// TestObject
//
int64_t TestObject::sTOCount            = 0;
int64_t TestObject::sTOCtorCount        = 0;
int64_t TestObject::sTODtorCount        = 0;
int64_t TestObject::sTODefaultCtorCount = 0;
int64_t TestObject::sTOArgCtorCount     = 0;
int64_t TestObject::sTOCopyCtorCount    = 0;
int64_t TestObject::sTOMoveCtorCount    = 0;
int64_t TestObject::sTOCopyAssignCount  = 0;
int64_t TestObject::sTOMoveAssignCount  = 0;
int     TestObject::sMagicErrorCount    = 0;


///////////////////////////////////////////////////////////////////////////////
// MallocAllocator
//
int    MallocAllocator::mAllocCountAll   = 0;
int    MallocAllocator::mFreeCountAll    = 0;
size_t MallocAllocator::mAllocVolumeAll  = 0;
void*  MallocAllocator::mpLastAllocation = NULL;


///////////////////////////////////////////////////////////////////////////////
// InstanceAllocator
//
int InstanceAllocator::mMismatchCount = 0;


///////////////////////////////////////////////////////////////////////////////
// CountingAllocator
//
uint64_t CountingAllocator::activeAllocCount      = 0;
uint64_t CountingAllocator::totalAllocCount       = 0;
uint64_t CountingAllocator::totalDeallocCount     = 0;
uint64_t CountingAllocator::totalCtorCount        = 0;
uint64_t CountingAllocator::defaultCtorCount      = 0;
uint64_t CountingAllocator::copyCtorCount         = 0;
uint64_t CountingAllocator::assignOpCount         = 0;
uint64_t CountingAllocator::totalAllocatedMemory  = 0;
uint64_t CountingAllocator::activeAllocatedMemory = 0;


///////////////////////////////////////////////////////////////////////////////
// gEASTL_TestLevel
//
int gEASTL_TestLevel = 0;


///////////////////////////////////////////////////////////////////////////////
// EASTLTest_CheckMemory_Imp
//
int EASTLTest_CheckMemory_Imp(const char* pFile, int nLine)
{
	int  nErrorCount(0);
	bool bMemoryOK(true);

	#if defined(_DEBUG) && (defined(EA_COMPILER_MSVC) && defined(EA_PLATFORM_MICROSOFT))
		if(!_CrtCheckMemory())
			bMemoryOK = false;
	#endif

	#ifdef EA_DEBUG
		if(!EASTLTest_ValidateHeap())
			bMemoryOK = false;
	#endif

	if(!bMemoryOK)
	{
		nErrorCount++;
		EASTLTest_Printf("Memory check failure:\n%s: line %d\n\n", pFile, nLine);
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// GetStdSTLType
//
StdSTLType GetStdSTLType()
{
	#if defined(_STLPORT_VERSION)
		return kSTLPort;                    // Descendent of the old HP / SGI STL.
	#elif defined(_RWSTD_VER_STR)
		return kSTLApache;                  // a.k.a. Rogue Wave, which is a descendent of the old HP / SGI STL.
	#elif defined(_CPPLIB_VER)
		return kSTLDinkumware;              // Indicated by the presence of the central yvals.h header.
	#elif defined(_LIBCPP_VECTOR)
		return kSTLClang;
	#elif defined(_GLIBCXX_VECTOR)
		return kSTLGCC;                     // a.k.a. libstdc++
	#elif defined(_MSC_VER)
		return kSTLMS;                      // This is a tweaked version of Dinkumware.
	#else
		return kSTLUnknown;
	#endif
}



///////////////////////////////////////////////////////////////////////////////
// GetStdSTLName
//
const char* GetStdSTLName()
{
	// We may need to tweak this function over time. 
	// Theoretically it is possible to have multiple standard
	// STL versions active, as some of them have options to 
	// put themselves in different namespaces.

	// Tests for specific STL versions directly.
	#if defined(_STLPORT_VERSION)
		return "STLPort";
	#elif defined(__SGI_STL_VECTOR)
		return "SGI";
	#elif defined(_RWSTD_VER_STR)
		return "Apache";

	// Tests for specific platforms that have specific STL versions.
	#elif defined(EA_PLATFORM_SONY)
		return "Sony Dinkumware";

	// Special case for Dinkumware.
	#elif defined(_CPPLIB_VER)
		#if defined(_MSC_VER)
			return "VC++ Dinkumware";
		#else
			return "Dinkumware";
		#endif

	// Tests for specific compilers as a fallback.
	#elif defined(_MSC_VER)
		return "VC++ ???";
	#elif defined(_LIBCPP_VECTOR)
		return "clang libc++";
	#elif defined(__GNUC__) || defined(_GLIBCXX_VECTOR)
		return "GCC (or emulated GCC) libstdc++";
	#else
		#error Need to be able to identify the standard STL version.
	#endif
}



///////////////////////////////////////////////////////////////////////////////
// MallocAllocator
///////////////////////////////////////////////////////////////////////////////

// The following function is defined here instead of in the header because GCC  
// generates a bogus warning about freeing a non-heap pointer when this function
// is declared inline.

void MallocAllocator::deallocate(void *p, size_t n)
{ 
	++mFreeCount;
	mAllocVolume -= n;
	++mFreeCountAll;
	mAllocVolumeAll -= n;

	return free(p);
}

void* MallocAllocator::allocate(size_t n, int)
{
	++mAllocCount; mAllocVolume += n; ++mAllocCountAll; mAllocVolumeAll += n; mpLastAllocation = malloc(n); return mpLastAllocation;
}

void* MallocAllocator::allocate(size_t n, size_t, size_t, int)
{
	++mAllocCount; mAllocVolume += n; ++mAllocCountAll; mAllocVolumeAll += n; mpLastAllocation = malloc(n); return mpLastAllocation;
}



///////////////////////////////////////////////////////////////////////////////
// CustomAllocator
///////////////////////////////////////////////////////////////////////////////

void* CustomAllocator::allocate(size_t n, int flags)
{
	return ::operator new[](n, get_name(), flags, 0, __FILE__, __LINE__);
}

void* CustomAllocator::allocate(size_t n, size_t alignment, size_t offset, int flags)
{
	return ::operator new[](n, alignment, offset, get_name(), flags, 0, __FILE__, __LINE__);
}

void  CustomAllocator::deallocate(void* p, size_t /*n*/)
{
	::operator delete((char*)p);
}






