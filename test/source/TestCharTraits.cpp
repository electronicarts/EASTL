/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EABase/eabase.h>
#include <EASTL/internal/char_traits.h>
#include <EASTL/string.h>


template<typename CharT>
int TestCharTraits()
{
	int nErrorCount = 0;

	// Compare(const T* p1, const T* p2, size_t n)
	// Compare(const char* p1, const char* p2, size_t n)
	{
		auto create = [](const char* cstr, eastl_size_t sz)
		{
			eastl::basic_string<CharT> result;
			result.assign_convert(cstr, sz - 1);
			return result;
		};

		eastl::basic_string<CharT> empty;
		eastl::basic_string<CharT> abc = create("abc", 4);
		eastl::basic_string<CharT> abd = create("abd", 4);
		eastl::basic_string<CharT> ABC = create("ABC", 4);
		EATEST_VERIFY(eastl::Compare(empty.data(), empty.data(), 0) == 0);
		EATEST_VERIFY(eastl::Compare(abc.data(), abc.data(), 3) == 0);
		EATEST_VERIFY(eastl::Compare(abc.data(), abd.data(), 3) < 0);
		EATEST_VERIFY(eastl::Compare(abd.data(), abc.data(), 3) > 0);
		EATEST_VERIFY(eastl::Compare(ABC.data(), abc.data(), 3) < 0);
		EATEST_VERIFY(eastl::Compare(empty.data(), abc.data(), 0) == 0);
	}

	return nErrorCount;
}


int TestCharTraits()
{
	using namespace eastl;

	int nErrorCount = 0;

	nErrorCount += TestCharTraits<char>();
	// eastl::DecodePart() is not implemented for (un)signed char.
	//nErrorCount += TestCharTraits<unsigned char>();
	//nErrorCount += TestCharTraits<signed char>();
	nErrorCount += TestCharTraits<wchar_t>();
	nErrorCount += TestCharTraits<char8_t>();
	nErrorCount += TestCharTraits<char16_t>();
	nErrorCount += TestCharTraits<char32_t>();

	return nErrorCount;
}









