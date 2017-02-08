/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EABase/eabase.h>
#include <EASTL/numeric_limits.h>
#include <EASTL/string_view.h>


// this mess is required inorder to inject string literal string conversion macros into the unit tests
#define TEST_STRING_NAME TestBasicStringView
#define LITERAL(x) x
#include "TestStringView.inl"

#define TEST_STRING_NAME TestBasicStringViewW
#define LITERAL(x) EA_WCHAR(x) 
#include "TestStringView.inl"

#define TEST_STRING_NAME TestBasicStringView16
#define LITERAL(x) EA_CHAR16(x) 
#include "TestStringView.inl"

#define TEST_STRING_NAME TestBasicStringView32
#define LITERAL(x) EA_CHAR32(x) 
#include "TestStringView.inl"


int TestStringView()
{
	using namespace eastl;
	int nErrorCount = 0;

	nErrorCount += TestBasicStringView<eastl::basic_string_view<char>>();
	nErrorCount += TestBasicStringView<eastl::string_view>();

	nErrorCount += TestBasicStringViewW<eastl::basic_string_view<wchar_t>>();
	nErrorCount += TestBasicStringViewW<eastl::wstring_view>();

	nErrorCount += TestBasicStringView16<eastl::basic_string_view<char16_t>>();
	nErrorCount += TestBasicStringView16<eastl::u16string_view>();

#if EA_CHAR32_NATIVE
	nErrorCount += TestBasicStringView32<eastl::basic_string_view<char32_t>>();
	nErrorCount += TestBasicStringView32<eastl::u32string_view>();
#endif

	// strlen(char_t) compatibility
	{
		auto* pStr = "Hello, World";
		string_view sw(pStr, strlen(pStr));
		VERIFY(sw.size() == strlen(pStr));
	}

	// strlen(wchar_t) compatibility
	{
		auto* pStr = L"Hello, World";
		wstring_view sw(pStr, wcslen(pStr));
		VERIFY(sw.size() == wcslen(pStr));
	}


	return nErrorCount;
}

