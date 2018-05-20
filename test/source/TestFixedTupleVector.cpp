/////////////////////////////////////////////////////////////////////////////
// TestFixedTupleVector.cpp
//
// Copyright (c) 2016, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"

#include <EASTL/fixed_tuple_vector.h>

using namespace eastl;

template <size_t nodeCount, bool bEnableOverflow>
int TestTupleVectorIterator()
{
	int nErrorCount = 0;

	fixed_tuple_vector<nodeCount, bEnableOverflow, int, float, int> tripleElementVec;
	tripleElementVec.push_back(1, 2.0f, 6);
	tripleElementVec.push_back(2, 3.0f, 7);
	tripleElementVec.push_back(3, 4.0f, 8);
	tripleElementVec.push_back(4, 5.0f, 9);
	tripleElementVec.push_back(5, 6.0f, 10);


	// test copyConstructible, copyAssignable, swappable, prefix inc, !=, reference convertible to value_type (InputIterator!)
	{
		fixed_tuple_vector<nodeCount, bEnableOverflow, int, float, int>::iterator iter = tripleElementVec.begin();
		++iter;
		auto copiedIter(iter);
		EATEST_VERIFY(get<2>(*copiedIter) == 7);
		EATEST_VERIFY(copiedIter == iter);

		++iter;
		copiedIter = iter;
		EATEST_VERIFY(get<2>(*copiedIter) == 8);

		++iter;
		swap(iter, copiedIter);
		EATEST_VERIFY(get<2>(*iter) == 8);
		EATEST_VERIFY(get<2>(*copiedIter) == 9);

		EATEST_VERIFY(copiedIter != iter);

		tuple<int&, float&, int&> ref(*iter);
		tuple<int, float, int> value(*iter);
		EATEST_VERIFY(get<2>(ref) == get<2>(value));
	}

	// test postfix increment, default constructible (ForwardIterator)
	{
		fixed_tuple_vector<nodeCount, bEnableOverflow, int, float, int>::iterator iter = tripleElementVec.begin();
		auto prefixIter = ++iter;

		fixed_tuple_vector<nodeCount, bEnableOverflow, int, float, int>::iterator postfixIter;
		postfixIter = iter++;
		EATEST_VERIFY(prefixIter == postfixIter);
		EATEST_VERIFY(get<2>(*prefixIter) == 7);
		EATEST_VERIFY(get<2>(*iter) == 8);
	}

	// test prefix decrement and postfix decrement (BidirectionalIterator)
	{
		fixed_tuple_vector<nodeCount, bEnableOverflow, int, float, int>::iterator iter = tripleElementVec.end();
		auto prefixIter = --iter;

		fixed_tuple_vector<nodeCount, bEnableOverflow, int, float, int>::iterator postfixIter;
		postfixIter = iter--;
		EATEST_VERIFY(prefixIter == postfixIter);
		EATEST_VERIFY(get<2>(*prefixIter) == 10);
		EATEST_VERIFY(get<2>(*iter) == 9);
	}

	// test many arithmetic operations (RandomAccessIterator)
	{
		fixed_tuple_vector<nodeCount, bEnableOverflow, int, float, int>::iterator iter = tripleElementVec.begin();
		auto symmetryOne = iter + 2;
		auto symmetryTwo = 2 + iter;
		iter += 2;
		EATEST_VERIFY(symmetryOne == symmetryTwo);
		EATEST_VERIFY(symmetryOne == iter);

		symmetryOne = iter - 2;
		symmetryTwo = 2 - iter;
		iter -= 2;
		EATEST_VERIFY(symmetryOne == symmetryTwo);
		EATEST_VERIFY(symmetryOne == iter);

		iter += 2;
		EATEST_VERIFY(iter - symmetryOne == 2);

		tuple<int&, float&, int&> symmetryRef = symmetryOne[2];
		EATEST_VERIFY(get<2>(symmetryRef) == get<2>(*iter));

		EATEST_VERIFY(symmetryOne < iter);
		EATEST_VERIFY(iter > symmetryOne);
		EATEST_VERIFY(symmetryOne >= symmetryTwo && iter >= symmetryOne);
		EATEST_VERIFY(symmetryOne <= symmetryTwo && symmetryOne <= iter);
	}

	{
		float i = 0;
		int j = 0;
		EATEST_VERIFY(&get<0>(*tripleElementVec.begin()) == tripleElementVec.get<0>());
		EATEST_VERIFY(&get<1>(*tripleElementVec.begin()) == tripleElementVec.get<1>());
		for (auto& iter : tripleElementVec)
		{
			i += get<1>(iter);
			j += get<2>(iter);
		}
		EATEST_VERIFY(i == 20.0f);
		EATEST_VERIFY(j == 40);
	}
	return nErrorCount;
}

int TestFixedTupleVector()
{
	int nErrorCount = 0;

	// Test uninit'ed push-backs
	{
		fixed_tuple_vector<4, false, int> singleElementVec;
		EATEST_VERIFY(singleElementVec.size() == 0);
		EATEST_VERIFY(singleElementVec.capacity() == 4);
		singleElementVec.push_back_uninitialized();
		singleElementVec.push_back(5);
		EATEST_VERIFY(singleElementVec.size() == 2);
		EATEST_VERIFY(singleElementVec.get<0>()[1] == 5);
		EATEST_VERIFY(singleElementVec.get<int>()[1] == 5);


		fixed_tuple_vector<4, true, int, float, bool> complexVec;
		complexVec.push_back(3, 2.0f, true);
		complexVec.push_back(1, 4.0f, false);
		complexVec.push_back(2, 1.0f, true);
		complexVec.push_back(4, 3.0f, false);
		EATEST_VERIFY(*(complexVec.get<0>()) == 3);
		EATEST_VERIFY(complexVec.get<float>()[1] == 4.0f);
		EATEST_VERIFY(complexVec.get<2>()[2] == complexVec.get<bool>()[2]);

		complexVec.push_back(2, 1.0f, true);
		complexVec.push_back(4, 3.0f, false);
		EATEST_VERIFY(*(complexVec.get<0>()) == 3);
		EATEST_VERIFY(complexVec.get<float>()[1] == 4.0f);
		EATEST_VERIFY(complexVec.get<2>()[2] == complexVec.get<bool>()[2]);

		__declspec(align(16)) struct AlignTestVec4
		{
			float a[4];
			AlignTestVec4() :a{ 1.0f, 2.0f, 3.0f, 4.0f } {}
		};

		struct AlignTestByte3
		{
			char a[3];
			AlignTestByte3() :a{ 1, 2, 3 } {}
		};

		__declspec(align(8)) struct AlignTestFourByte
		{
			int a[5];
			AlignTestFourByte() :a{ -1, -2, -3, -4, -5 } {}
		};

		fixed_tuple_vector<16, false, bool, AlignTestVec4, AlignTestByte3, AlignTestFourByte> alignElementVec;
		alignElementVec.push_back();
		alignElementVec.push_back();
		alignElementVec.push_back();
		alignElementVec.push_back();
	}

	// Test fixed_tuple_Vector in a ranged for, and other large-scale iterator testing
	nErrorCount += TestTupleVectorIterator<4, true>();
	nErrorCount += TestTupleVectorIterator<16, false>();
	nErrorCount += TestTupleVectorIterator<16, true>();


	return nErrorCount;
}


