/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#if defined(_MSC_VER)
	//#pragma warning(disable: 4267)  // 'argument' : conversion from 'size_t' to 'uint32_t', possible loss of data.
#endif


#include "EASTLTest.h"
#include <EASTL/numeric_limits.h>
#include <EASTL/set.h>
#include <EASTL/random.h>


struct GeneratorUint8
{
	uint8_t mValue;
	GeneratorUint8() : mValue(0) {}
	uint8_t operator()(){ return mValue++; } // This is a pretty bad random number generator, but works for our tests.
};

struct GeneratorUint16
{
	uint16_t mValue;
	GeneratorUint16() : mValue(0) {}
	uint16_t operator()(){ return mValue++; }
};

struct GeneratorUint32
{
	uint32_t mValue;
	GeneratorUint32() : mValue(0) {}
	uint32_t operator()(){ return mValue++; }
};




///////////////////////////////////////////////////////////////////////////////
// TestRandom
//
int TestRandom()
{
	int nErrorCount = 0;

	{
		// template<class IntType = int>
		// class uniform_int_distribution

		// The C++11 Standard defines a number of formal Generators, such as std::mersenne_twister_engine, 
		// linear_congruential_engine, discard_block_engine, etc.

		using namespace eastl;

		{
			eastl::uniform_int_distribution<uint8_t> uid(1, 6);
			GeneratorUint8 g;
 
			for(uint32_t i = 0; i < UINT8_MAX; i += 1)
			{
				uint8_t value = uid(g);
				EATEST_VERIFY((value >= 1) && (value <= 6));
				// To do: Validate the randomness of the value.
			}

			eastl::uniform_int_distribution<uint8_t> uid2(1, 6);
			EATEST_VERIFY(uid == uid2);
		}

		{
			eastl::uniform_int_distribution<uint16_t> uid(1, 6);
			GeneratorUint16 g;
 
			for(uint32_t i = 0; i < (UINT16_MAX - (UINT16_MAX / 50)); i += (UINT16_MAX / 50))
			{
				uint16_t value = uid(g);
				EATEST_VERIFY((value >= 1) && (value <= 6));
				// To do: Validate the randomness of the value.
			}

			eastl::uniform_int_distribution<uint16_t> uid2(1, 6);
			EATEST_VERIFY(uid == uid2);
		}

		{
			eastl::uniform_int_distribution<uint32_t> uid(1, 6);
			GeneratorUint32 g;
 
			for(uint32_t i = 0; i < (UINT32_MAX - (UINT32_MAX / 500)); i += (UINT32_MAX / 500))
			{
				uint32_t value = uid(g);
				EATEST_VERIFY((value >= 1) && (value <= 6));
				// To do: Validate the randomness of the value.
			}

			eastl::uniform_int_distribution<uint32_t> uid2(1, 6);
			EATEST_VERIFY(uid == uid2);
		}
	}



	/// Example usage:
	///     eastl_size_t Rand(eastl_size_t n) { return (eastl_size_t)(rand() % n); } // Note: The C rand function is poor and slow.
	///     pointer_to_unary_function<eastl_size_t, eastl_size_t> randInstance(Rand);
	///     random_shuffle(pArrayBegin, pArrayEnd, randInstance);
	///
	/// Example usage:
	///     struct Rand{ eastl_size_t operator()(eastl_size_t n) { return (eastl_size_t)(rand() % n); } }; // Note: The C rand function is poor and slow.
	///     Rand randInstance;
	///     random_shuffle(pArrayBegin, pArrayEnd, randInstance);


	{
		// void random_shuffle(RandomAccessIterator first, RandomAccessIterator last, RandomNumberGenerator& rng)
		using namespace eastl;

		EASTLTest_Rand rng(EA::UnitTest::GetRandSeed());
		int intArray[] = { 3, 2, 6, 5, 4, 1 };

		random_shuffle(intArray, intArray + 0, rng);
		EATEST_VERIFY(VerifySequence(intArray, intArray + 6, int(), "random_shuffle", 3, 2, 6, 5, 4, 1, -1));

		random_shuffle(intArray, intArray + (sizeof(intArray) / sizeof(intArray[0])), rng);
		bool changed = false;
		for(int i = 0; (i < 5) && !changed; i++)
		{
			changed = (intArray[0] != 3) || (intArray[1] != 2) || (intArray[2] != 6) || 
					  (intArray[3] != 5) || (intArray[4] != 4) || (intArray[5] != 1);
		}
		EATEST_VERIFY(changed);

		// Test of possible bug report by user John Chin.
		// The report is that shuffling an ordered array 0, 1, 2, 3, 4 ... results in duplicates, such as 5, 2, 2, 4 ...
		eastl::vector<eastl_size_t> rngArray;

		for(eastl_size_t i = 0; i < 200; ++i)
			rngArray.push_back(i);

		random_shuffle(rngArray.begin(), rngArray.end(), rng);
		EATEST_VERIFY(rngArray.validate());

		eastl::set<eastl_size_t> intSet;

		for(eastl_size_t s = 0, sEnd = rngArray.size(); s < sEnd; ++s)
			intSet.insert(rngArray[s]);

		// If the shuffled array is unique, then a set of its values should be the same size as the array.
		EATEST_VERIFY(intSet.size() == rngArray.size());
	}


	return nErrorCount;
}












