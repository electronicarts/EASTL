/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EABase/eabase.h>
#include <EAStdC/EAMemory.h>
#include <EAStdC/EAString.h>
#include <EASTL/string.h>
#include <EASTL/algorithm.h>
#include <EASTL/allocator_malloc.h>

using namespace eastl;

// Verify char8_t support is present if the test build requested it.
#if defined(EASTL_EXPECT_CHAR8T_SUPPORT) && !EA_CHAR8_UNIQUE
static_assert(false, "Building with char8_t tests enabled, but EA_CHAR8_UNIQUE evaluates to false.");
#endif

// inject string literal string conversion macros into the unit tests
#define TEST_STRING_NAME TestBasicString
#define LITERAL(x) x
#include "TestString.inl"

#define TEST_STRING_NAME TestBasicStringW
#define LITERAL(x) EA_WCHAR(x)
#include "TestString.inl"

#define TEST_STRING_NAME TestBasicString8
#define LITERAL(x) EA_CHAR8(x)
#include "TestString.inl"

#define TEST_STRING_NAME TestBasicString16
#define LITERAL(x) EA_CHAR16(x)
#include "TestString.inl"

#define TEST_STRING_NAME TestBasicString32
#define LITERAL(x) EA_CHAR32(x)
#include "TestString.inl"

int TestString()
{
	int nErrorCount = 0;

	nErrorCount += TestBasicString<eastl::basic_string<char, StompDetectAllocator>>();
	nErrorCount += TestBasicString<eastl::string>();

	nErrorCount += TestBasicStringW<eastl::basic_string<wchar_t, StompDetectAllocator>>();
	nErrorCount += TestBasicStringW<eastl::wstring>();

#if EA_CHAR8_UNIQUE
	nErrorCount += TestBasicString8<eastl::basic_string<char8_t, StompDetectAllocator>>();
	nErrorCount += TestBasicString8<eastl::u8string>();
#endif

	nErrorCount += TestBasicString16<eastl::basic_string<char16_t, StompDetectAllocator>>();
	nErrorCount += TestBasicString16<eastl::u16string>();

#if EA_CHAR32_NATIVE
	nErrorCount += TestBasicString32<eastl::basic_string<char32_t, StompDetectAllocator>>();
	nErrorCount += TestBasicString32<eastl::u32string>();
#endif

	// Check for memory leaks by using the 'CountingAllocator' to ensure no active allocation after tests have completed.
	CountingAllocator::resetCount();
	nErrorCount += TestBasicString<eastl::basic_string<char, CountingAllocator>>();
	VERIFY(CountingAllocator::getActiveAllocationCount() == 0); 

	nErrorCount += TestBasicStringW<eastl::basic_string<wchar_t, CountingAllocator>>();
	VERIFY(CountingAllocator::getActiveAllocationCount() == 0); 

#if EA_CHAR8_UNIQUE
	nErrorCount += TestBasicString8<eastl::basic_string<char8_t, CountingAllocator>>();
	VERIFY(CountingAllocator::getActiveAllocationCount() == 0);
#endif

	nErrorCount += TestBasicString16<eastl::basic_string<char16_t, CountingAllocator>>();
	VERIFY(CountingAllocator::getActiveAllocationCount() == 0); 

#if EA_CHAR32_NATIVE
	nErrorCount += TestBasicString32<eastl::basic_string<char32_t, CountingAllocator>>();
	VERIFY(CountingAllocator::getActiveAllocationCount() == 0); 
#endif

	// to_string
	{
		VERIFY(eastl::to_string(42)    == "42");
		VERIFY(eastl::to_string(42l)   == "42");
		VERIFY(eastl::to_string(42ll)  == "42");
		VERIFY(eastl::to_string(42u)   == "42");
		VERIFY(eastl::to_string(42ul)  == "42");
		VERIFY(eastl::to_string(42ull) == "42");
		VERIFY(eastl::to_string(42.f)  == "42.000000");
		VERIFY(eastl::to_string(42.0)  == "42.000000");
	#if !defined(EA_COMPILER_GNUC) && !defined(EA_PLATFORM_MINGW)
		// todo:  long double sprintf functionality is unrealiable on unix-gcc, requires further debugging.  
		VERIFY(eastl::to_string(42.0l) == "42.000000");
	#endif
	}

	// to_wstring
	{
		VERIFY(eastl::to_wstring(42)    == L"42");
		VERIFY(eastl::to_wstring(42l)   == L"42");
		VERIFY(eastl::to_wstring(42ll)  == L"42");
		VERIFY(eastl::to_wstring(42u)   == L"42");
		VERIFY(eastl::to_wstring(42ul)  == L"42");
		VERIFY(eastl::to_wstring(42ull) == L"42");
		VERIFY(eastl::to_wstring(42.f)  == L"42.000000");
		VERIFY(eastl::to_wstring(42.0)  == L"42.000000");
	#if !defined(EA_COMPILER_GNUC) && !defined(EA_PLATFORM_MINGW)
		// todo:  long double sprintf functionality is unrealiable on unix-gcc, requires further debugging.  
		VERIFY(eastl::to_wstring(42.0l) == L"42.000000");
	#endif
	}

	#if EASTL_USER_LITERALS_ENABLED 
	{
		VERIFY("cplusplus"s == "cplusplus");
		VERIFY(L"cplusplus"s == L"cplusplus");
		VERIFY(u"cplusplus"s == u"cplusplus");
		VERIFY(U"cplusplus"s == U"cplusplus");
		VERIFY(u8"cplusplus"s == u8"cplusplus");
	}
	#endif


	{
		// CustomAllocator has no data members which reduces the size of an eastl::basic_string via the empty base class optimization.
		typedef eastl::basic_string<char, CustomAllocator> EboString;

		// this must match the eastl::basic_string heap memory layout struct which is a pointer and 2 eastl_size_t.
		const int expectedSize = sizeof(EboString::pointer) + (2 * sizeof(EboString::size_type));

		static_assert(sizeof(EboString) == expectedSize, "unexpected layout size of basic_string");
	}

	return nErrorCount;
}


