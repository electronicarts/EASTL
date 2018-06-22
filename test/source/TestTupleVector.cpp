/////////////////////////////////////////////////////////////////////////////
// TestTupleVector.cpp
//
// Copyright (c) 2016, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"

#include <EASTL/array.h>
#include <EASTL/tuple_vector.h>
#include <EASTL/sort.h>

#include <random>
#include <windows.h>

using namespace eastl;

void flushCache(int* cacheFlushSrc, const int cacheFlushSize, int* cacheFlushDest)
{
	// try and evict all of the cache
	memset(cacheFlushSrc, 0, cacheFlushSize);
	memcpy(cacheFlushDest, cacheFlushSrc, cacheFlushSize);
}

int TestTupleVector()
{
	int nErrorCount = 0;

	// Test push-backs and simple accessors
	{
		tuple_vector<int> singleElementVec;
		EATEST_VERIFY(singleElementVec.size() == 0);
		EATEST_VERIFY(singleElementVec.capacity() == 0);
		EATEST_VERIFY(singleElementVec.empty() == true);
		singleElementVec.push_back_uninitialized();
		singleElementVec.push_back(5);
		EATEST_VERIFY(singleElementVec.size() == 2);
		EATEST_VERIFY(singleElementVec.get<0>()[1] == 5);
		EATEST_VERIFY(singleElementVec.get<int>()[1] == 5);
		EATEST_VERIFY(singleElementVec.empty() == false);
		
		tuple_vector<int, float, bool> complexVec;
		complexVec.push_back(3, 2.0f, true);
		complexVec.push_back(1, 4.0f, false);
		complexVec.push_back(2, 1.0f, true);
		complexVec.push_back(4, 3.0f, false);
		EATEST_VERIFY(*(complexVec.get<0>()) == 3);
		EATEST_VERIFY(complexVec.get<float>()[1] == 4.0f);
		EATEST_VERIFY(complexVec.get<2>()[2] == complexVec.get<bool>()[2]);
		
		tuple<int*, float*, bool*> complexPtrTuple = complexVec.data();
		EATEST_VERIFY(get<0>(complexPtrTuple) != nullptr);
		EATEST_VERIFY(get<2>(complexPtrTuple)[2] == complexVec.get<2>()[2]);

		tuple<int&, float&, bool&> complexRefTuple = complexVec.at(2);
		EATEST_VERIFY(get<2>(complexRefTuple) == complexVec.get<2>()[2]);
		EATEST_VERIFY(get<1>(complexVec[2]) == 1.0f);
		EATEST_VERIFY(get<1>(complexVec.front()) == 2.0f);
		EATEST_VERIFY(get<1>(complexVec.back()) == 3.0f);

		// verify the equivalent accessors for the const container exist/compile
		{
			const tuple_vector<int, float, bool>& constVec = complexVec;

			EATEST_VERIFY(constVec.size() == 4);
			EATEST_VERIFY(constVec.capacity() >= constVec.size());
			EATEST_VERIFY(constVec.empty() == false);
			EATEST_VERIFY(constVec.get<1>() == constVec.get<float>());
			
			tuple<const int*, const float*, const bool*> constPtrTuple = constVec.data();
			EATEST_VERIFY(get<0>(constPtrTuple) != nullptr);
			EATEST_VERIFY(get<2>(constPtrTuple)[2] == constVec.get<2>()[2]);

			tuple<const int&, const float&, const bool&> constRefTuple = constVec.at(2);
			EATEST_VERIFY(get<2>(constRefTuple) == constVec.get<2>()[2]);
			EATEST_VERIFY(get<1>(constVec[2]) == 1.0f);
			EATEST_VERIFY(get<1>(constVec.front()) == 2.0f);
			EATEST_VERIFY(get<1>(constVec.back()) == 3.0f);
		}

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

	// Test various modifications
	{
		TestObject::Reset();

		tuple_vector<bool, TestObject, float> testVec;
		for (int i = 0; i < 10; ++i)
		{
			testVec.push_back(i % 3 == 0, TestObject(i), (float)i);
		}
		testVec.pop_back();
		EATEST_VERIFY(testVec.size() == 9);
		testVec.resize(5);
		EATEST_VERIFY(testVec.size() == 5);
		testVec.resize(10);
		EATEST_VERIFY(testVec.size() == 10);

		testVec.resize(15, true, TestObject(5), 5.0f);
		EATEST_VERIFY(testVec.size() == 15);
		for (unsigned int i = 10; i < 15; ++i)
		{
			EATEST_VERIFY(testVec.get<0>()[i] == true);
			EATEST_VERIFY(testVec.get<1>()[i] == TestObject(5));
			EATEST_VERIFY(testVec.get<2>()[i] == 5.0f);
		}

		{
			tuple<bool, TestObject, float> resizeTup(true, TestObject(10), 10.0f);
			testVec.resize(20, resizeTup);
		}
		EATEST_VERIFY(testVec.size() == 20);
		for (unsigned int i = 10; i < 15; ++i)
		{
			EATEST_VERIFY(testVec.get<0>()[i] == true);
			EATEST_VERIFY(testVec.get<1>()[i] == TestObject(5));
			EATEST_VERIFY(testVec.get<2>()[i] == 5.0f);
		}
		for (unsigned int i = 15; i < 20; ++i)
		{
			EATEST_VERIFY(testVec.get<0>()[i] == true);
			EATEST_VERIFY(testVec.get<1>()[i] == TestObject(10));
			EATEST_VERIFY(testVec.get<2>()[i] == 10.0f);
		}

		testVec.push_back();
		testVec.pop_back();
		EATEST_VERIFY(testVec.capacity() != 20);
		testVec.shrink_to_fit();
		EATEST_VERIFY(testVec.capacity() == 20);

		testVec.clear();
		EATEST_VERIFY(testVec.empty());
		EATEST_VERIFY(TestObject::IsClear());
		TestObject::Reset();

		testVec.shrink_to_fit();
		EATEST_VERIFY(testVec.capacity() == 0);

		// convoluted inserts to get "0, 1, 2, 3, 4, 5, 6" on the floats/testobject's
		auto testVecIter = testVec.insert(testVec.begin(), true, TestObject(5), 5.0f);
		testVec.insert(testVecIter, false, TestObject(4), 4.0f);
		testVec.insert(testVecIter, true, TestObject(1), 1.0f);
		testVecIter = testVec.insert(testVecIter + 1, false, TestObject(3), 3.0f);
		testVec.insert(testVecIter, true, TestObject(2), 2.0f);
		testVec.insert(testVec.begin(), false, TestObject(0), 0.0f);
		testVec.insert(testVec.end(), true, TestObject(6), 6.0f);
		EATEST_VERIFY(testVec.size() == 7);
		for (unsigned int i = 0; i < testVec.size(); ++i)
		{
			EATEST_VERIFY(testVec.get<1>()[i] == TestObject(i));
		}
		
		// test for large inserts that don't resize capacity, and clean out the added range
		testVec.reserve(20);
		testVec.insert(testVec.begin() + 5, 5, false, TestObject(10), 10.0f);
		testVec.insert(testVec.begin() + 5, 5, false, TestObject(10), 10.0f);
		for (unsigned int i = 5; i < 15; ++i)
		{
			EATEST_VERIFY(testVec.get<1>()[i] == TestObject(10));
		}
		testVec.erase(eastl::remove_if(testVec.begin(), testVec.end(),
			[](auto tup)
			{
				return get<2>(tup) == 10.0f;  
			}),
			testVec.end());
		EATEST_VERIFY(testVec.size() == 7);
		for (unsigned int i = 0; i < testVec.size(); ++i)
		{
			EATEST_VERIFY(testVec.get<2>()[i] != 10.0f);
		}

		// eliminate 0, 2, 4, 6 from the above list to get 1, 3, 5
		testVecIter = testVec.erase(testVec.begin());
		testVecIter = testVec.erase(testVecIter + 1); 
		testVec.erase(testVecIter + 1);
		testVec.erase(testVec.end() - 1);
		for (unsigned int i = 0; i < testVec.size(); ++i)
		{
			EATEST_VERIFY(testVec.get<1>()[i] == TestObject(i * 2 + 1));
		}
		EATEST_VERIFY(TestObject::sTOCount == testVec.size());
		
		// remove 1, 3 from the list and make sure 5 is present, then remove the rest of the list
		testVec.erase(testVec.begin(), testVec.begin() + 2);
		EATEST_VERIFY(testVec.size() == 1);
		EATEST_VERIFY(testVec.get<1>()[0] == TestObject(5));
		testVec.erase(testVec.begin(), testVec.end());
		EATEST_VERIFY(testVec.empty());
		
		EATEST_VERIFY(TestObject::IsClear());

		// erase_unsorted test
		for (int i = 0; i < 10; ++i)
		{
			testVec.push_back(i % 3 == 0, TestObject(i), (float)i);
		}

		testVec.erase_unsorted(testVec.begin() + 0);
		EATEST_VERIFY(testVec.size() == 9);
		EATEST_VERIFY(testVec.get<1>()[0] == TestObject(9));
		EATEST_VERIFY(testVec.get<1>()[1] == TestObject(1));
		EATEST_VERIFY(testVec.get<1>()[8] == TestObject(8));

		testVec.erase_unsorted(testVec.begin() + 5);
		EATEST_VERIFY(testVec.size() == 8);
		EATEST_VERIFY(testVec.get<1>()[0] == TestObject(9));
		EATEST_VERIFY(testVec.get<1>()[5] == TestObject(8));
		EATEST_VERIFY(testVec.get<1>()[7] == TestObject(7));

		testVec.erase_unsorted(testVec.begin() + 7);
		EATEST_VERIFY(testVec.size() == 7);
		EATEST_VERIFY(testVec.get<1>()[0] == TestObject(9));
		EATEST_VERIFY(testVec.get<1>()[5] == TestObject(8));
		EATEST_VERIFY(testVec.get<1>()[6] == TestObject(6));

		testVec.erase(testVec.begin(), testVec.end());
		EATEST_VERIFY(TestObject::IsClear());

		// test tuple_vector dtor
		{
			tuple_vector<bool, TestObject, float> dtorCheck;
			for (int i = 0; i < 10; ++i)
			{
				dtorCheck.push_back(i % 3 == 0, TestObject(i), (float)i);
			}
		}
		EATEST_VERIFY(TestObject::IsClear());

		TestObject::Reset();
	}

	// Test multitude of constructors
	{
		TestObject::Reset();
		tuple_vector<bool, TestObject, float> srcVec;
		for (int i = 0; i < 10; ++i)
		{
			srcVec.push_back(i % 3 == 0, TestObject(i), (float)i);
		}

		{
			tuple_vector<bool, TestObject, float> ctorFromConstRef(srcVec);
			EATEST_VERIFY(ctorFromConstRef.size() == 10);
			for (int i = 0; i < 10; ++i)
			{
				EATEST_VERIFY(ctorFromConstRef.get<0>()[i] == (i % 3 == 0));
				EATEST_VERIFY(ctorFromConstRef.get<1>()[i] == TestObject(i));
				EATEST_VERIFY(ctorFromConstRef.get<2>()[i] == (float)i);
			}
		}

		{
			tuple_vector<bool, TestObject, float> ctorFromIters(srcVec.begin() + 2, srcVec.begin() + 7);
			EATEST_VERIFY(ctorFromIters.size() == 5);
			for (int i = 2; i < 7; ++i)
			{
				EATEST_VERIFY(ctorFromIters.get<0>()[i - 2] == (i % 3 == 0));
				EATEST_VERIFY(ctorFromIters.get<1>()[i - 2] == TestObject(i));
				EATEST_VERIFY(ctorFromIters.get<2>()[i - 2] == (float)i);
			}
		}

		{
			tuple_vector<bool, TestObject, float> ctorFromFill(10);
			EATEST_VERIFY(ctorFromFill.size() == 10);
			for (int i = 0; i < 10; ++i)
			{
				EATEST_VERIFY(ctorFromFill.get<0>()[i] == false);
				EATEST_VERIFY(ctorFromFill.get<1>()[i] == TestObject());
				EATEST_VERIFY(ctorFromFill.get<2>()[i] == 0.0f);
			}
		}

		{
			tuple_vector<bool, TestObject, float> ctorFromFillArgs(10, true, TestObject(5), 5.0f);
			EATEST_VERIFY(ctorFromFillArgs.size() == 10);
			for (int i = 0; i < 10; ++i)
			{
				EATEST_VERIFY(ctorFromFillArgs.get<0>()[i] == true);
				EATEST_VERIFY(ctorFromFillArgs.get<1>()[i] == TestObject(5));
				EATEST_VERIFY(ctorFromFillArgs.get<2>()[i] == 5.0f);
			}
		}

		{
			tuple<bool, TestObject, float> tup(true, TestObject(5), 5.0f);
			tuple_vector<bool, TestObject, float> ctorFromFillTup(10, tup);
			EATEST_VERIFY(ctorFromFillTup.size() == 10);
			for (int i = 0; i < 10; ++i)
			{
				EATEST_VERIFY(ctorFromFillTup.get<0>()[i] == true);
				EATEST_VERIFY(ctorFromFillTup.get<1>()[i] == TestObject(5));
				EATEST_VERIFY(ctorFromFillTup.get<2>()[i] == 5.0f);
			}
		}
		srcVec.clear();
		EATEST_VERIFY(TestObject::IsClear());

		TestObject::Reset();
	}

	// Test erase with reverse iterators
	{
		TestObject::Reset();

		tuple_vector<bool, TestObject, float> testVec;
		for (int i = 0; i < 7; ++i)
		{
			testVec.push_back(i % 3 == 0, TestObject(i), (float)i);
		}

		// eliminate 0, 2, 4, 6 from the above list to get 1, 3, 5
		auto testVecIter = testVec.erase(testVec.rbegin());
		testVecIter = testVec.erase(testVecIter + 1);
		testVec.erase(testVecIter + 1);
		testVec.erase(testVec.rend() - 1);
		for (unsigned int i = 0; i < testVec.size(); ++i)
		{
			EATEST_VERIFY(testVec.get<1>()[i] == TestObject(i * 2 + 1));
		}
		EATEST_VERIFY(TestObject::sTOCount == testVec.size());

		// remove 1, 3 from the list and make sure 5 is present, then remove the rest of the list
		testVec.erase(testVec.rbegin() + 1, testVec.rend());
		EATEST_VERIFY(testVec.size() == 1);
		EATEST_VERIFY(testVec.get<1>()[0] == TestObject(5));
		testVec.erase(testVec.rbegin(), testVec.rend());
		EATEST_VERIFY(testVec.empty());

		EATEST_VERIFY(TestObject::IsClear());

		// erase_unsorted test
		for (int i = 0; i < 10; ++i)
		{
			testVec.push_back(i % 3 == 0, TestObject(i), (float)i);
		}

		testVec.erase_unsorted(testVec.rbegin() + 9);
		EATEST_VERIFY(testVec.size() == 9);
		EATEST_VERIFY(testVec.get<1>()[0] == TestObject(9));
		EATEST_VERIFY(testVec.get<1>()[1] == TestObject(1));
		EATEST_VERIFY(testVec.get<1>()[8] == TestObject(8));

		testVec.erase_unsorted(testVec.rbegin() + 3);
		EATEST_VERIFY(testVec.size() == 8);
		EATEST_VERIFY(testVec.get<1>()[0] == TestObject(9));
		EATEST_VERIFY(testVec.get<1>()[5] == TestObject(8));
		EATEST_VERIFY(testVec.get<1>()[7] == TestObject(7));

		testVec.erase_unsorted(testVec.rbegin() + 0);
		EATEST_VERIFY(testVec.size() == 7);
		EATEST_VERIFY(testVec.get<1>()[0] == TestObject(9));
		EATEST_VERIFY(testVec.get<1>()[5] == TestObject(8));
		EATEST_VERIFY(testVec.get<1>()[6] == TestObject(6));

		testVec.erase(testVec.begin(), testVec.end());
		EATEST_VERIFY(TestObject::IsClear());

		TestObject::Reset();
	}

	// Test swap
	{
		tuple_vector<int, float, bool> complexVec;
		complexVec.push_back(3, 2.0f, true);
		complexVec.push_back(1, 4.0f, false);
		complexVec.push_back(2, 1.0f, true);
		complexVec.push_back(4, 3.0f, false);

		tuple_vector<int, float, bool> otherComplexVec;
		complexVec.swap(otherComplexVec);
		
		EATEST_VERIFY(complexVec.size() == 0);
		EATEST_VERIFY(otherComplexVec.get<0>()[0] == 3);
		EATEST_VERIFY(otherComplexVec.get<float>()[1] == 4.0f);

		complexVec.push_back(10, 10.0f, true);
		swap(complexVec, otherComplexVec);

		EATEST_VERIFY(*(complexVec.get<0>()) == 3);
		EATEST_VERIFY(complexVec.get<float>()[1] == 4.0f);

		EATEST_VERIFY(otherComplexVec.get<float>()[0] == 10.0f);
		EATEST_VERIFY(otherComplexVec.size() == 1);

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

			tuple<const int&, const float&, const int&> ref(*iter);
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

		// test simple iteration, and reverse iteration
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

			float reverse_i = 0;
			int reverse_j = 0;

			eastl::for_each(tripleElementVec.rbegin(), tripleElementVec.rend(),
				[&](const tuple<int, float, int> tup)
				{
					reverse_i += get<1>(tup);
					reverse_j += get<2>(tup);
				});
			EATEST_VERIFY(i == reverse_i);
			EATEST_VERIFY(j == reverse_j);
			EATEST_VERIFY(get<0>(*tripleElementVec.rbegin()) == 5);
		}
	}

	// Test move operations (esp. with unique_ptr)
	{
		TestObject::Reset();
		{
			tuple_vector<int, eastl::unique_ptr<int>, TestObject> v1;
			tuple_vector<int, eastl::unique_ptr<int>, TestObject> v2;
			int testValues[] = {42, 43, 44, 45, 46, 47};

			// add some data in the vector so we can move it to the other vector.
			v1.reserve(5);
			for (int i = 0; i < 3; ++i)
			{
				auto emplacedTup = v1.emplace_back(i * 2 + 1, eastl::make_unique<int>(testValues[i * 2]), TestObject(i * 2 + 1));
			}
			v1.emplace(v1.end(), 6, eastl::make_unique<int>(testValues[5]), TestObject(6));
			v1.emplace(v1.begin() + 1, 2, eastl::make_unique<int>(testValues[1]), TestObject(2));
			v1.emplace(v1.begin() + 3, 4, eastl::make_unique<int>(testValues[3]), TestObject(4));

 			tuple<int&, eastl::unique_ptr<int>&, TestObject&> movedTup = v1.at(0);
 			EATEST_VERIFY(get<0>(movedTup) == 1);
 			EATEST_VERIFY(get<0>(*v1.begin()) == 1);

			for (unsigned int i = 0; i < v1.size(); ++i)
			{
				EATEST_VERIFY(v1.get<0>()[i] == i + 1);
			}
			EATEST_VERIFY(!v1.empty() && v2.empty());
			v2 = eastl::move(v1);
			EATEST_VERIFY(v1.empty() && !v2.empty());
			v1.swap(v2);
			EATEST_VERIFY(!v1.empty() && v2.empty());

			v2.insert(v2.begin(), *make_move_iterator(v1.begin()));
			EATEST_VERIFY(v2.size() == 1);
			EATEST_VERIFY(v1.size() == 6);
			EATEST_VERIFY(v1.get<2>()[0] == TestObject(0));
			EATEST_VERIFY(v2.get<2>()[0] == TestObject(1));
			EATEST_VERIFY(v1.get<1>()[0].get() == nullptr);
			EATEST_VERIFY(*v2.get<1>()[0].get() == testValues[0]);

			v1.shrink_to_fit();
			v2.shrink_to_fit();
			EATEST_VERIFY(v2.size() == 1);
			EATEST_VERIFY(v1.size() == 6);
			EATEST_VERIFY(v1.get<2>()[0] == TestObject(0));
			EATEST_VERIFY(v2.get<2>()[0] == TestObject(1));
			EATEST_VERIFY(v1.get<1>()[0].get() == nullptr);
			EATEST_VERIFY(*v2.get<1>()[0].get() == testValues[0]);

			tuple_vector<int, eastl::unique_ptr<int>, TestObject> v3(eastl::move(v2));
			EATEST_VERIFY(v2.size() == 0);
			EATEST_VERIFY(v3.size() == 1);
			EATEST_VERIFY(v3.get<2>()[0] == TestObject(1));
			EATEST_VERIFY(*v3.get<1>()[0].get() == testValues[0]);
		}
		EATEST_VERIFY(TestObject::IsClear());
	}

	// test sort.h
	{
		LARGE_INTEGER  tupleLoopCounter; tupleLoopCounter.QuadPart = 0;
		LARGE_INTEGER  vectorLoopCounter; vectorLoopCounter.QuadPart = 0;
		LARGE_INTEGER  tupleSortCounter; tupleSortCounter.QuadPart = 0;
		LARGE_INTEGER  vectorSortCounter; vectorSortCounter.QuadPart = 0;
		const int cacheFlushSize = 10 * 1024 * 1024;
		int* cacheFlushSrc = new int[cacheFlushSize];
		int* cacheFlushDest = new int[cacheFlushSize];
		LARGE_INTEGER frequency;
		const int NumData = 64;
		struct LargeData
		{
			LargeData(float f)
			{
				data.fill(f);
			}
			eastl::array<float, NumData> data;
		};
		QueryPerformanceFrequency(&frequency);
		do
		{
			LARGE_INTEGER startTime, endTime;

			// create+populate the two vectors with some junk
			tuple_vector<bool, LargeData, int> tripleElementVec;
			const int ElementCount = 1 * 1024 * 1024;
			tripleElementVec.reserve(ElementCount);

			struct TripleElement
			{
				bool a;
				LargeData b;
				int c;
			};
			vector<TripleElement> aosTripleElement;
			aosTripleElement.reserve(ElementCount);

			std::default_random_engine e1(0);
			std::uniform_int_distribution<int> bool_picker(0, 1);
			std::uniform_real_distribution<float> float_picker(0, 32768);
			std::uniform_int_distribution<int> int_picker(0, 32768);

			for (int i = 0; i < ElementCount; ++i)
			{
				bool randomBool = bool_picker(e1) < 1 ? false : true;
				float randomFloat = float_picker(e1);
				int randomInt = int_picker(e1);
				tripleElementVec.push_back(randomBool, {randomFloat}, randomInt);
				aosTripleElement.push_back({ randomBool, {randomFloat}, randomInt });
			}

			flushCache(cacheFlushSrc, cacheFlushSize, cacheFlushDest);

			// measure tuplevec in a loop
			QueryPerformanceCounter(&startTime);
			volatile int numTupleBools = 0;
			for (auto& iter : tripleElementVec)
			{
				numTupleBools += get<0>(iter) ? 1 : 0;
			}
			QueryPerformanceCounter(&endTime);
			tupleLoopCounter.QuadPart += (endTime.QuadPart - startTime.QuadPart);

			flushCache(cacheFlushSrc, cacheFlushSize, cacheFlushDest);

			// measure tuplevec in a sort
			QueryPerformanceCounter(&startTime);
			sort(tripleElementVec.begin(), tripleElementVec.end(),
				[](auto& a, auto& b)
			{
				return get<2>(a) > get<2>(b);
			});
			QueryPerformanceCounter(&endTime);
			tupleSortCounter.QuadPart += (endTime.QuadPart - startTime.QuadPart);

			flushCache(cacheFlushSrc, cacheFlushSize, cacheFlushDest);

			// measure vector in a loop
			QueryPerformanceCounter(&startTime);
			volatile int numVecBools = 0;
			for (auto& iter : aosTripleElement)
			{
				numVecBools += iter.a ? 1 : 0;
			}
			QueryPerformanceCounter(&endTime);
			vectorLoopCounter.QuadPart += (endTime.QuadPart - startTime.QuadPart);

			flushCache(cacheFlushSrc, cacheFlushSize, cacheFlushDest);

			// measure vector in a sort
			QueryPerformanceCounter(&startTime);
			sort(aosTripleElement.begin(), aosTripleElement.end(), [](const TripleElement& a, const TripleElement& b)
			{
				return a.c > b.c;
			});
			QueryPerformanceCounter(&endTime);
			vectorSortCounter.QuadPart += (endTime.QuadPart - startTime.QuadPart);

		} while (((tupleSortCounter.QuadPart * 1000.0) / frequency.QuadPart) < 5.0);

		EATEST_VERIFY(nErrorCount == 0);

		double tupleLoopTime = (tupleLoopCounter.QuadPart * 1000.0) / frequency.QuadPart;
		double vectorLoopTime = (vectorLoopCounter.QuadPart * 1000.0) / frequency.QuadPart;
		double tupleSortTime = (tupleSortCounter.QuadPart * 1000.0) / frequency.QuadPart;
		double vectorSortTime = (vectorSortCounter.QuadPart * 1000.0) / frequency.QuadPart;

		EA::EAMain::ReportVerbosity(0, "Tuple loop time (ms): %f\n", tupleLoopTime);
		EA::EAMain::ReportVerbosity(0, "Vector loop time (ms): %f\n", vectorLoopTime);
		EA::EAMain::ReportVerbosity(0, "Tuple sort time (ms): %f\n", tupleSortTime);
		EA::EAMain::ReportVerbosity(0, "Vector sort time (ms): %f\n", vectorSortTime);

		EATEST_VERIFY(nErrorCount == 0);

	}


	return nErrorCount;
}


