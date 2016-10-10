/////////////////////////////////////////////////////////////////////////////
// TestTupleVector.cpp
//
// Copyright (c) 2014, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"

#include <EASTL/tuple_vector.h>
#include <EASTL/sort.h>

#include <random>
#include <windows.h>
using namespace eastl;

int TestTupleVector()
{
	int nErrorCount = 0;

	// Test uninit'ed push-backs
	{
		tuple_vector<int> singleElementVec;
		EATEST_VERIFY(singleElementVec.size() == 0);
		EATEST_VERIFY(singleElementVec.capacity() == 0);
		singleElementVec.push_back_uninitialized();
		singleElementVec.push_back(5);
		EATEST_VERIFY(singleElementVec.size() == 2);
		EATEST_VERIFY(singleElementVec.get<0>()[1] == 5);
		EATEST_VERIFY(singleElementVec.get<int>()[1] == 5);

		
		
		tuple_vector<int, float, bool> complexVec;
		complexVec.push_back(3, 2.0f, true);
		complexVec.push_back(1, 4.0f, false);
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

		tuple_vector<bool, AlignTestVec4, AlignTestByte3, AlignTestFourByte> alignElementVec;
		alignElementVec.push_back();
		alignElementVec.push_back();
		alignElementVec.push_back();
		alignElementVec.push_back();
		alignElementVec.push_back();
	}

	// Test tuple_Vector in a ranged for, and other large-scale iterator testing
	{
		tuple_vector<int, float, int> tripleElementVec;
		tripleElementVec.push_back(1, 2.0f, 6);
		tripleElementVec.push_back(2, 3.0f, 7);
		tripleElementVec.push_back(3, 4.0f, 8);
		tripleElementVec.push_back(4, 5.0f, 9);
		tripleElementVec.push_back(5, 6.0f, 10);


		// test copyConstructible, copyAssignable, swappable, prefix inc, !=, reference convertible to value_type (InputIterator!)
		{
			tuple_vector<int, float, int>::iterator iter = tripleElementVec.begin();
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
			tuple_vector<int, float, int>::iterator iter = tripleElementVec.begin();
			auto prefixIter = ++iter;
			
			tuple_vector<int, float, int>::iterator postfixIter;
			postfixIter = iter++;
			EATEST_VERIFY(prefixIter == postfixIter);
			EATEST_VERIFY(get<2>(*prefixIter) == 7);
			EATEST_VERIFY(get<2>(*iter) == 8);
		}

		// test prefix decrement and postfix decrement (BidirectionalIterator)
		{
			tuple_vector<int, float, int>::iterator iter = tripleElementVec.end();
			auto prefixIter = --iter;

			tuple_vector<int, float, int>::iterator postfixIter;
			postfixIter = iter--;
			EATEST_VERIFY(prefixIter == postfixIter);
			EATEST_VERIFY(get<2>(*prefixIter) == 10);
			EATEST_VERIFY(get<2>(*iter) == 9);
		}

		// test many arithmetic operations (RandomAccessIterator)
		{
			tuple_vector<int, float, int>::iterator iter = tripleElementVec.begin();
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
	}

	// test sort.h
	{
		LARGE_INTEGER  tupleSortCounter; tupleSortCounter.QuadPart = 0;
		LARGE_INTEGER  vectorSortCounter; vectorSortCounter.QuadPart = 0;
		const int cacheFlushSize = 10 * 1024 * 1024;
		int* cacheFlushSrc = new int[cacheFlushSize];
		int* cacheFlushDest = new int[cacheFlushSize];
		LARGE_INTEGER frequency;
		const int NumData = 64;
		struct LargeData
		{
			/*LargeData(float f)
			{
				memset(&data, f, NumData);
			}*/
			float data[NumData];
		};
		QueryPerformanceFrequency(&frequency);
		do
		{
			LARGE_INTEGER startTime, endTime;

			tuple_vector<bool, LargeData, int> tripleElementVec;
			const int ElementCount = 1 * 1024;
			tripleElementVec.reserve(ElementCount);

			struct TripleElement
			{
				bool a;
				LargeData b;
				int c;
			};
			vector<TripleElement> aosTripleElement;
			aosTripleElement.reserve(ElementCount);

			// Choose a random mean between 1 and 6
			std::default_random_engine e1(0);
			std::uniform_int_distribution<int> bool_picker(0, 0);
			std::uniform_real_distribution<float> float_picker(0, 32768);
			std::uniform_int_distribution<int> int_picker(0, 32768);

			for (int i = 0; i < ElementCount; ++i)
			{
				bool randomBool = (bool)bool_picker(e1);
				float randomFloat = float_picker(e1);
				int randomInt = int_picker(e1);
				tripleElementVec.push_back(randomBool, {randomFloat}, randomInt);
				aosTripleElement.push_back({ randomBool, {randomFloat}, randomInt });
			}

			{
				memset(cacheFlushSrc, 0, cacheFlushSize);
				memcpy(cacheFlushDest, cacheFlushSrc, cacheFlushSize);
			}

			QueryPerformanceCounter(&startTime);
			//EATEST_VERIFY(tripleElementVec.get<0>() != nullptr);
			//volatile int numTupleBools = 0;
			//for (auto& iter : tripleElementVec)
			//{
			//	numTupleBools += get<0>(iter) ? 1 : 0;
			//}
			sort(tripleElementVec.begin(), tripleElementVec.end(),
				[](tuple<const bool&, const LargeData&, const int&> a, tuple<const bool&, const LargeData&, const int&> b)
			{
				return get<2>(a) > get<2>(b);
			});
			QueryPerformanceCounter(&endTime);
			tupleSortCounter.QuadPart += (endTime.QuadPart - startTime.QuadPart);

			//EATEST_VERIFY(tripleElementVec.get<0>() != nullptr);

			{
				memset(cacheFlushSrc, 0, cacheFlushSize);
				memcpy(cacheFlushDest, cacheFlushSrc, cacheFlushSize);
			}
			//EATEST_VERIFY(aosTripleElement.data() != nullptr);

			QueryPerformanceCounter(&startTime);
			//volatile int numVecBools = 0;
			//for (auto& iter : aosTripleElement)
			//{
			//	numVecBools += iter.a ? 1 : 0;
			//}
			sort(aosTripleElement.begin(), aosTripleElement.end(), [](const TripleElement& a, const TripleElement& b)
			{
				return a.c > b.c;
			});
			QueryPerformanceCounter(&endTime);
			vectorSortCounter.QuadPart += (endTime.QuadPart - startTime.QuadPart);
			//EATEST_VERIFY(aosTripleElement.data() != nullptr);
		} while ((tupleSortCounter.QuadPart * 1000.0) / frequency.QuadPart < 10.0);

		EATEST_VERIFY(nErrorCount == 0);

		double tupleSortTime = (tupleSortCounter.QuadPart * 1000.0) / frequency.QuadPart;
		double vectorSortTime = (vectorSortCounter.QuadPart * 1000.0) / frequency.QuadPart;

		EA::EAMain::ReportVerbosity(0, "Tuple loop time (ms): %f\n", tupleSortTime);
		EA::EAMain::ReportVerbosity(0, "Vector loop time (ms): %f\n", vectorSortTime);

		EATEST_VERIFY(nErrorCount == 0);

	}


	return nErrorCount;
}


