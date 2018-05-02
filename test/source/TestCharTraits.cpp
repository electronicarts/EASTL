/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EABase/eabase.h>
#include <EASTL/internal/char_traits.h>


template<typename CharT>
int TestCharTraits()
{
	int nErrorCount = 0;
	return nErrorCount;
}


int TestCharTraits()
{
	using namespace eastl;

	int nErrorCount = 0;

	nErrorCount += TestCharTraits<char>();
	nErrorCount += TestCharTraits<wchar_t>();
	nErrorCount += TestCharTraits<char16_t>();
	nErrorCount += TestCharTraits<char32_t>();

	return nErrorCount;
}









