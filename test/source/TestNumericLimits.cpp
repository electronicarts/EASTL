/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/numeric_limits.h>


struct NonNumericType
{
	NonNumericType(int value) : mValue(value){}
	bool operator==(int value) const { return mValue == value; }
	int mValue; // This exists for the purpose of allowing the type to act like a number and allow the test logic below to work.
};


///////////////////////////////////////////////////////////////////////////////
// TestNumericLimits
//
int TestNumericLimits()
{
	int nErrorCount = 0;

	// To consider: Some day when we get more time, make a big table-driven set of 
	// expected results to all member variables and function calls.

	// Test a type that is not numeric,.
	EATEST_VERIFY(!eastl::numeric_limits<NonNumericType>::is_bounded);
	EATEST_VERIFY( eastl::numeric_limits<NonNumericType>::max() == 0);

	EATEST_VERIFY(!eastl::numeric_limits<const NonNumericType>::is_bounded);
	EATEST_VERIFY( eastl::numeric_limits<const NonNumericType>::max() == 0);

	EATEST_VERIFY(!eastl::numeric_limits<volatile NonNumericType>::is_bounded);
	EATEST_VERIFY( eastl::numeric_limits<volatile NonNumericType>::max() == 0);

	EATEST_VERIFY(!eastl::numeric_limits<const volatile NonNumericType>::is_bounded);
	EATEST_VERIFY( eastl::numeric_limits<const volatile NonNumericType>::max() == 0);

	// Test bool in all const-volatile variants.
	EATEST_VERIFY(eastl::numeric_limits<bool>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<bool>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<const bool>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<const bool>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<volatile bool>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<volatile bool>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<const volatile bool>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<const volatile bool>::max() != 0);

	// Do basic tests of the remaining types.
	EATEST_VERIFY(eastl::numeric_limits<char>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<char>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<unsigned char>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<unsigned char>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<signed char>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<signed char>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<wchar_t>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<wchar_t>::max() != 0);

	#if defined(EA_CHAR8_UNIQUE) && EA_CHAR8_UNIQUE
	EATEST_VERIFY(eastl::numeric_limits<char8_t>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<char8_t>::max() != 0);
	#endif

	EATEST_VERIFY(eastl::numeric_limits<char16_t>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<char16_t>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<char32_t>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<char32_t>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<unsigned short>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<unsigned short>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<signed short>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<signed short>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<unsigned int>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<unsigned int>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<signed int>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<signed int>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<unsigned long>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<unsigned long>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<signed long>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<signed long>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<unsigned long long>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<unsigned long long>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<signed long long>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<signed long long>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<float>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<float>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<double>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<double>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<long double>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<long double>::max() != 0);

	// We don't yet have a generic global way to identify what the name of the supported 128 bit type is. 
	// We just happen to know that for gcc/clang it is __int128.
	#if (EA_COMPILER_INTMAX_SIZE >= 16) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)) // If __int128_t/__uint128_t is supported...
		EATEST_VERIFY(eastl::numeric_limits<__uint128_t>::is_bounded);
		EATEST_VERIFY(eastl::numeric_limits<__uint128_t>::max() != 0);

		EATEST_VERIFY(eastl::numeric_limits<__int128_t>::is_bounded);
		EATEST_VERIFY(eastl::numeric_limits<__int128_t>::max() != 0);
	#endif

	// Test sized types.
	EATEST_VERIFY(eastl::numeric_limits<uint8_t>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<uint8_t>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<int8_t>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<int8_t>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<uint16_t>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<uint16_t>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<int16_t>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<int16_t>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<uint32_t>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<uint32_t>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<int32_t>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<int32_t>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<uint64_t>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<uint64_t>::max() != 0);

	EATEST_VERIFY(eastl::numeric_limits<int64_t>::is_bounded);
	EATEST_VERIFY(eastl::numeric_limits<int64_t>::max() != 0);

	return nErrorCount;
}












