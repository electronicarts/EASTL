/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EASTL/bit.h>

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
		VERIFY(eastl::bit_width(power_of_two) == i + 1);
	}

	return nErrorCount;
}

template <typename T>
int TestCountlZero()
{
	int nErrorCount = 0;

	VERIFY(eastl::countl_zero(T(0)) == (sizeof(T) * CHAR_BIT));
	VERIFY(eastl::countl_zero(T(0b1)) == (sizeof(T) * CHAR_BIT - 1));
	VERIFY(eastl::countl_zero(T(0b11)) == (sizeof(T) * CHAR_BIT - 2));
	VERIFY(eastl::countl_zero(T(0b111)) == (sizeof(T) * CHAR_BIT - 3));
	VERIFY(eastl::countl_zero(T(0b1111)) == (sizeof(T) * CHAR_BIT - 4));
	VERIFY(eastl::countl_zero(T(0b1010)) == (sizeof(T) * CHAR_BIT - 4));
	VERIFY(eastl::countl_zero(T(0b1100)) == (sizeof(T) * CHAR_BIT - 4));

	EA::UnitTest::Rand rng(EA::UnitTest::GetRandSeed());

	for (int r = 0; r < 100; ++r)
	{
		T num = 0;
		int min_index = sizeof(T) * CHAR_BIT;
		for (unsigned int i = 0; i < 4; ++i)
		{
			int index = rng.RandRange(0, eastl::numeric_limits<T>::digits);
			num = num | (T(1) << (sizeof(T) * CHAR_BIT - 1 - index));
			min_index = eastl::min(min_index, index);
		}

		VERIFY(eastl::countl_zero(num) == min_index);
	}

	return nErrorCount;
}

template <typename T>
int TestPopCount()
{
	int nErrorCount = 0;

	VERIFY(eastl::popcount(T(0)) == 0);
	VERIFY(eastl::popcount(T(0b1)) == 1);
	VERIFY(eastl::popcount(T(0b11)) == 2);
	VERIFY(eastl::popcount(T(0b111)) == 3);
	VERIFY(eastl::popcount(T(0b1111)) == 4);
	VERIFY(eastl::popcount(T(0b1010)) == 2);
	VERIFY(eastl::popcount(T(0b1100)) == 2);

	EA_CONSTEXPR auto digits = eastl::numeric_limits<T>::digits;

	EA::UnitTest::Rand rng(EA::UnitTest::GetRandSeed());

	for (int r = 0; r < 100; ++r)
	{
		T num = 0;
		int count = 0;
		for (unsigned int i = 0; i < digits; ++i)
		{
			if (rng.RandRange(0, 4) == 0)
			{
				++count;
				num = num | (T(1) << i);
			}
		}

		VERIFY(eastl::popcount(num) == count);
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
#if EASTL_INT128_SUPPORTED
	nErrorCount += TestHasSingleBit<eastl_uint128_t>();
#endif

	nErrorCount += TestCountlZero<unsigned int>();
	nErrorCount += TestCountlZero<unsigned char>();
	nErrorCount += TestCountlZero<unsigned short>();
	nErrorCount += TestCountlZero<unsigned long>();
	nErrorCount += TestCountlZero<unsigned long long>();
#if EASTL_INT128_SUPPORTED
	nErrorCount += TestCountlZero<eastl_uint128_t>();
#endif

	nErrorCount += TestBitWidth<unsigned int>();
	nErrorCount += TestBitWidth<unsigned char>();
	nErrorCount += TestBitWidth<unsigned short>();
	nErrorCount += TestBitWidth<unsigned long>();
	nErrorCount += TestBitWidth<unsigned long long>();
#if EASTL_INT128_SUPPORTED
	nErrorCount += TestBitWidth<eastl_uint128_t>();
#endif

	nErrorCount += TestBitCeil<unsigned int>();
	nErrorCount += TestBitCeil<unsigned char>();
	nErrorCount += TestBitCeil<unsigned short>();
	nErrorCount += TestBitCeil<unsigned long>();
	nErrorCount += TestBitCeil<unsigned long long>();
#if EASTL_INT128_SUPPORTED
	nErrorCount += TestBitCeil<eastl_uint128_t>();
#endif

	nErrorCount += TestBitFloor<unsigned int>();
	nErrorCount += TestBitFloor<unsigned char>();
	nErrorCount += TestBitFloor<unsigned short>();
	nErrorCount += TestBitFloor<unsigned long>();
	nErrorCount += TestBitFloor<unsigned long long>();
#if EASTL_INT128_SUPPORTED
	nErrorCount += TestBitFloor<eastl_uint128_t>();
#endif

	nErrorCount += TestPopCount<unsigned int>();
	nErrorCount += TestPopCount<unsigned char>();
	nErrorCount += TestPopCount<unsigned short>();
	nErrorCount += TestPopCount<unsigned long>();
	nErrorCount += TestPopCount<unsigned long long>();
#if EASTL_INT128_SUPPORTED
	nErrorCount += TestPopCount<eastl_uint128_t>();
#endif

	return nErrorCount;
}
