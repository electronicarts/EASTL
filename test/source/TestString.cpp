/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EABase/eabase.h>
#include <EAStdC/EAMemory.h>
#include <EASTL/string.h>
#include <EASTL/algorithm.h>
#include <EASTL/allocator_malloc.h>

using namespace eastl;

// todo:  comment on this mess being required inorder to inject string literal string conversion macros into the unit
// test code.
#define TEST_STRING_NAME TestBasicString
#define LITERAL(x) x
#include "TestString.inl"

#define TEST_STRING_NAME TestBasicStringW
#define LITERAL(x) EA_WCHAR(x) 
#include "TestString.inl"

#define TEST_STRING_NAME TestBasicString16
#define LITERAL(x) EA_CHAR16(x) 
#include "TestString.inl"

#define TEST_STRING_NAME TestBasicString32
#define LITERAL(x) EA_CHAR32(x) 
#include "TestString.inl"

int TestString()
{
	EASTLTest_Printf("TestString\n");

	int nErrorCount = 0;

	nErrorCount += TestBasicString<eastl::basic_string<char>>();
	nErrorCount += TestBasicString<eastl::string>();

	nErrorCount += TestBasicStringW<eastl::basic_string<wchar_t>>();
	nErrorCount += TestBasicStringW<eastl::wstring>();

	nErrorCount += TestBasicString16<eastl::basic_string<char16_t>>();
	nErrorCount += TestBasicString16<eastl::u16string>();

#if EA_CHAR32_NATIVE
	nErrorCount += TestBasicString32<eastl::basic_string<char32_t>>();
	nErrorCount += TestBasicString32<eastl::u32string>();
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
	#ifndef EA_COMPILER_GNUC
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
	#ifndef EA_COMPILER_GNUC
		// todo:  long double sprintf functionality is unrealiable on unix-gcc, requires further debugging.  
		VERIFY(eastl::to_wstring(42.0l) == L"42.000000");
	#endif
	}

	return nErrorCount;
}


