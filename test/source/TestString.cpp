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

	return nErrorCount;
}


