/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EASTL/bit.h>

#if defined(EA_COMPILER_CPP20_ENABLED)
template <typename T>
int TestHasSingleBit()
{
	int nErrorCount = 0;

	VERIFY(eastl::has_single_bit(T(0)) == false);
	VERIFY(eastl::has_single_bit(T(1)) == true);
	VERIFY(eastl::has_single_bit(T(2)) == true);
	VERIFY(eastl::has_single_bit(T(3)) == false);

	VERIFY(eastl::has_single_bit(eastl::numeric_limits<T>::min()) == false);
	VERIFY(eastl::has_single_bit(eastl::numeric_limits<T>::max()) == false);

	for (int i = 4; i < eastl::numeric_limits<T>::digits; i++)
	{
		T power_of_two = static_cast<T>(T(1U) << i);
		VERIFY(eastl::has_single_bit(power_of_two));
		VERIFY(eastl::has_single_bit(static_cast<T>(power_of_two - 1)) == false);
	}

	return nErrorCount;
}

template <typename T>
static int TestBitCeil()
{
	int nErrorCount = 0;

	VERIFY(eastl::bit_ceil(T(0)) == T(1));
	VERIFY(eastl::bit_ceil(T(1)) == T(1));
	VERIFY(eastl::bit_ceil(T(2)) == T(2));
	VERIFY(eastl::bit_ceil(T(3)) == T(4));

	EA_CONSTEXPR auto DIGITS = eastl::numeric_limits<T>::digits;
	EA_CONSTEXPR auto MIN = eastl::numeric_limits<T>::min();
	EA_CONSTEXPR auto MAX = static_cast<T>(T(1) << (DIGITS - 1));

	VERIFY(eastl::bit_ceil(MAX) == MAX);
	VERIFY(eastl::bit_ceil(static_cast<T>(MAX - 1)) == MAX);
	VERIFY(eastl::bit_ceil(MIN) == T(1));

	for (int i = 4; i < eastl::numeric_limits<T>::digits; i++)
	{
		T power_of_two = static_cast<T>(T(1U) << i);
		VERIFY(eastl::bit_ceil(power_of_two) == power_of_two);
		VERIFY(eastl::bit_ceil(static_cast<T>(power_of_two - 1)) == power_of_two);
	}

	return nErrorCount;
}

template <typename T>
static int TestBitFloor()
{
	int nErrorCount = 0;
	VERIFY(eastl::bit_floor(T(0)) == T(0));
	VERIFY(eastl::bit_floor(T(1)) == T(1));
	VERIFY(eastl::bit_floor(T(2)) == T(2));
	VERIFY(eastl::bit_floor(T(3)) == T(2));

	EA_CONSTEXPR auto DIGITS = eastl::numeric_limits<T>::digits;
	EA_CONSTEXPR auto MIN = eastl::numeric_limits<T>::min();
	EA_CONSTEXPR auto MAX = eastl::numeric_limits<T>::max();

	VERIFY(eastl::bit_floor(MAX) == T(1) << (DIGITS - 1));
	VERIFY(eastl::bit_floor(MIN) == T(0));

	for (int i = 4; i < eastl::numeric_limits<T>::digits; i++)
	{
		T power_of_two = static_cast<T>(T(1U) << i);
		VERIFY(eastl::bit_floor(power_of_two) == power_of_two);
		VERIFY(eastl::bit_floor(static_cast<T>(power_of_two + 1)) == power_of_two);
	}
	return nErrorCount;
}

template <typename T>
static int TestBitWidth()
{
	int nErrorCount = 0;

	VERIFY(eastl::bit_width(T(0)) == T(0));
	VERIFY(eastl::bit_width(T(1)) == T(1));
	VERIFY(eastl::bit_width(T(2)) == T(2));
	VERIFY(eastl::bit_width(T(3)) == T(2));

	EA_CONSTEXPR auto DIGITS = eastl::numeric_limits<T>::digits;
	EA_CONSTEXPR auto MIN = eastl::numeric_limits<T>::min();
	EA_CONSTEXPR auto MAX = eastl::numeric_limits<T>::max();

	VERIFY(eastl::bit_width(MIN) == 0);
	VERIFY(eastl::bit_width(MAX) == DIGITS);

	for (int i = 4; i < eastl::numeric_limits<T>::digits; i++)
	{
		T power_of_two = static_cast<T>(T(1U) << i);
		VERIFY(eastl::bit_width(power_of_two) == static_cast<T>(i + 1));
	}

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestBit
//
int TestBit()
{
	int nErrorCount = 0;

	nErrorCount += TestHasSingleBit<unsigned int>();
	nErrorCount += TestHasSingleBit<unsigned char>();
	nErrorCount += TestHasSingleBit<unsigned short>();
	nErrorCount += TestHasSingleBit<unsigned long>();
	nErrorCount += TestHasSingleBit<unsigned long long>();

	nErrorCount += TestBitCeil<unsigned int>();
	nErrorCount += TestBitCeil<unsigned char>();
	nErrorCount += TestBitCeil<unsigned short>();
	nErrorCount += TestBitCeil<unsigned long>();
	nErrorCount += TestBitCeil<unsigned long long>();

	nErrorCount += TestBitFloor<unsigned int>();
	nErrorCount += TestBitFloor<unsigned char>();
	nErrorCount += TestBitFloor<unsigned short>();
	nErrorCount += TestBitFloor<unsigned long>();
	nErrorCount += TestBitFloor<unsigned long long>();

	nErrorCount += TestBitWidth<unsigned int>();
	nErrorCount += TestBitWidth<unsigned char>();
	nErrorCount += TestBitWidth<unsigned short>();
	nErrorCount += TestBitWidth<unsigned long>();
	nErrorCount += TestBitWidth<unsigned long long>();

	return nErrorCount;
}
#endif
