/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EABase/eabase.h>
#include <EASTL/numeric_limits.h>
#include <EASTL/string_view.h>

// Verify char8_t support is present if the test build requested it.
#if defined(EASTL_EXPECT_CHAR8T_SUPPORT) && !EA_CHAR8_UNIQUE
static_assert(false, "Building with char8_t tests enabled, but EA_CHAR8_UNIQUE evaluates to false.");
#endif

// this mess is required inorder to inject string literal string conversion macros into the unit tests
#define TEST_STRING_NAME TestBasicStringView
#define LITERAL(x) x
#include "TestStringView.inl"

#define TEST_STRING_NAME TestBasicStringViewW
#define LITERAL(x) EA_WCHAR(x)
#include "TestStringView.inl"

#define TEST_STRING_NAME TestBasicStringView8
#define LITERAL(x) EA_CHAR8(x)
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

#if EA_CHAR8_UNIQUE
	nErrorCount += TestBasicStringView8<eastl::basic_string_view<char8_t>>();
	nErrorCount += TestBasicStringView8<eastl::u8string_view>();
#endif

	nErrorCount += TestBasicStringView16<eastl::basic_string_view<char16_t>>();
	nErrorCount += TestBasicStringView16<eastl::u16string_view>();

#if EA_CHAR32_NATIVE
	nErrorCount += TestBasicStringView32<eastl::basic_string_view<char32_t>>();
	nErrorCount += TestBasicStringView32<eastl::u32string_view>();
#endif


	// constexpr string_view operator "" sv(const char* str, size_t len) noexcept;
	// constexpr u8string_view operator "" sv(const char8_t* str, size_t len) noexcept;
	// constexpr u16string_view operator "" sv(const char16_t* str, size_t len) noexcept;
	// constexpr u32string_view operator "" sv(const char32_t* str, size_t len) noexcept;
	// constexpr wstring_view   operator "" sv(const wchar_t* str, size_t len) noexcept;
	#if EASTL_USER_LITERALS_ENABLED 
	{
		VERIFY("cplusplus"_sv.compare("cplusplus") == 0);
		VERIFY(L"cplusplus"_sv.compare(L"cplusplus") == 0);
		VERIFY(u"cplusplus"_sv.compare(u"cplusplus") == 0);
		VERIFY(U"cplusplus"_sv.compare(U"cplusplus") == 0);
		VERIFY(u8"cplusplus"_sv.compare(u8"cplusplus") == 0);

		static_assert(eastl::is_same_v<decltype("abcdef"_sv), eastl::string_view>, "string_view literal type mismatch");
		static_assert(eastl::is_same_v<decltype(u8"abcdef"_sv), eastl::u8string_view>, "string_view literal type mismatch");
		static_assert(eastl::is_same_v<decltype(u"abcdef"_sv), eastl::u16string_view>, "string_view literal type mismatch");
		static_assert(eastl::is_same_v<decltype(U"abcdef"_sv), eastl::u32string_view>, "string_view literal type mismatch");
		static_assert(eastl::is_same_v<decltype(L"abcdef"_sv), eastl::wstring_view>, "string_view literal type mismatch");

		// TODO:  Need to resolve this.  Not sure why on Clang the user literal 'operator ""sv' can't be found.
		// VERIFY("cplusplus"sv.compare("cplusplus") == 0);
		// VERIFY(L"cplusplus"sv.compare(L"cplusplus") == 0);
		// VERIFY(u"cplusplus"sv.compare(u"cplusplus") == 0);
		// VERIFY(U"cplusplus"sv.compare(U"cplusplus") == 0);
		// VERIFY(u8"cplusplus"sv.compare(u8"cplusplus") == 0);
	}
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

