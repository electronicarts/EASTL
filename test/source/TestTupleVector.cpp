/////////////////////////////////////////////////////////////////////////////
// TestTupleVector.cpp
//
// Copyright (c) 2016, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"

#include <EASTL/bonus/tuple_vector.h>

using namespace eastl;

struct MoveOnlyType
{
	MoveOnlyType() = delete;
	MoveOnlyType(int val) : mVal(val) {}
	MoveOnlyType(const MoveOnlyType&) = delete;
	MoveOnlyType(MoveOnlyType&& x) : mVal(x.mVal) { x.mVal = 0; }
	MoveOnlyType& operator=(const MoveOnlyType&) = delete;
	MoveOnlyType& operator=(MoveOnlyType&& x)
	{
		mVal = x.mVal;
		x.mVal = 0;
		return *this;
	}

	int mVal;
};

int TestTupleVector()
{
	int nErrorCount = 0;

	// Test push-backs and accessors
	{
		tuple_vector<int> singleElementVec;
		EATEST_VERIFY(singleElementVec.size() == 0);
		EATEST_VERIFY(singleElementVec.capacity() == 0);
		EATEST_VERIFY(singleElementVec.empty() == true);
		EATEST_VERIFY(singleElementVec.validate());
		singleElementVec.push_back_uninitialized();
		singleElementVec.push_back(5);
		EATEST_VERIFY(singleElementVec.size() == 2);
		EATEST_VERIFY(singleElementVec.capacity() > 0);
		EATEST_VERIFY(singleElementVec.get<0>()[1] == 5);
		EATEST_VERIFY(singleElementVec.get<int>()[1] == 5);
		EATEST_VERIFY(singleElementVec.empty() == false);
		EATEST_VERIFY(singleElementVec.validate());

		tuple_vector<int, float, bool> complexVec;
		complexVec.reserve(5);
		{
			// need to call an overload of push_back that specifically grabs lvalue candidates - providing constants tend to prefer rvalue path
			int intArg = 3;
			float floatArg = 2.0f;
			bool boolArg = true;
			complexVec.push_back(intArg, floatArg, boolArg);
		}
		complexVec.push_back(1, 4.0f, false);
		complexVec.push_back(2, 1.0f, true);
		{
			tuple<int, float, bool> complexTup(4, 3.0f, false);
			complexVec.push_back(complexTup);
		}
		complexVec.push_back();
		EATEST_VERIFY(complexVec.capacity() == 5);
		EATEST_VERIFY(*(complexVec.get<0>()) == 3);
		EATEST_VERIFY(complexVec.get<float>()[1] == 4.0f);
		EATEST_VERIFY(complexVec.get<2>()[2] == complexVec.get<bool>()[2]);
		EATEST_VERIFY(complexVec.validate());

		tuple<int, float, bool> defaultComplexTup;
		EATEST_VERIFY(complexVec.at(4) == defaultComplexTup);
		
		tuple<int*, float*, bool*> complexPtrTuple = complexVec.data();
		EATEST_VERIFY(get<0>(complexPtrTuple) != nullptr);
		EATEST_VERIFY(get<2>(complexPtrTuple)[2] == complexVec.get<2>()[2]);

		tuple<int&, float&, bool&> complexRefTuple = complexVec.at(2);
		tuple<int&, float&, bool&> complexRefTupleBracket = complexVec[2];
		tuple<int&, float&, bool&> complexRefTupleFront = complexVec.front();
		tuple<int&, float&, bool&> complexRefTupleBack = complexVec.back();
		EATEST_VERIFY(get<2>(complexRefTuple) == complexVec.get<2>()[2]);
		EATEST_VERIFY(get<1>(complexRefTupleBracket) == 1.0f);
		EATEST_VERIFY(get<1>(complexRefTupleFront) == 2.0f);
		EATEST_VERIFY(get<1>(complexRefTupleBack) == 0.0f);

		// verify the equivalent accessors for the const container exist/compile
		{
			const tuple_vector<int, float, bool>& constVec = complexVec;

			EATEST_VERIFY(constVec.size() == 5);
			EATEST_VERIFY(constVec.capacity() >= constVec.size());
			EATEST_VERIFY(constVec.empty() == false);
			EATEST_VERIFY(constVec.get<1>() == constVec.get<float>());

			tuple<const int*, const float*, const bool*> constPtrTuple = constVec.data();
			EATEST_VERIFY(get<0>(constPtrTuple) != nullptr);
			EATEST_VERIFY(get<2>(constPtrTuple)[2] == constVec.get<2>()[2]);

			tuple<const int&, const float&, const bool&> constRefTuple = constVec.at(2);
			tuple<const int&, const float&, const bool&> constRefTupleBracket = constVec[2];
			tuple<const int&, const float&, const bool&> constRefTupleFront = constVec.front();
			tuple<const int&, const float&, const bool&> constRefTupleBack = constVec.back();
			EATEST_VERIFY(get<2>(constRefTuple) == constVec.get<2>()[2]);
			EATEST_VERIFY(get<1>(constRefTupleBracket) == 1.0f);
			EATEST_VERIFY(get<1>(constRefTupleFront) == 2.0f);
			EATEST_VERIFY(get<1>(constRefTupleBack) == 0.0f);
		}
	}

	// test the memory layouts work for aligned structures
	{
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

		auto alignDataPtrs = alignElementVec.data();
		EATEST_VERIFY((uintptr_t)alignElementVec.get<AlignTestVec4>() % 16 == 0);
		EATEST_VERIFY((uintptr_t)alignElementVec.get<AlignTestFourByte>() % 8 == 0);
	}

	// Test various modifications
	{
		TestObject::Reset();

		tuple_vector<bool, TestObject, float> testVec;
		testVec.reserve(10);
		for (int i = 0; i < 10; ++i)
		{
			testVec.push_back(i % 3 == 0, TestObject(i), (float)i);
		}
		testVec.pop_back();
		EATEST_VERIFY(testVec.size() == 9);

		// test resize that does destruction of objects
		testVec.resize(5);
		EATEST_VERIFY(testVec.size() == 5);
		EATEST_VERIFY(TestObject::sTOCount == 5);
		EATEST_VERIFY(testVec.capacity() == 10);

		// test resize that does default construction of objects
		testVec.resize(10);
		EATEST_VERIFY(testVec.size() == 10);
		EATEST_VERIFY(TestObject::sTOCount == 10);
		EATEST_VERIFY(testVec.capacity() == 10);

		// test resize that does default construction of objects and grows the vector
		testVec.resize(15);
		EATEST_VERIFY(testVec.size() == 15);
		EATEST_VERIFY(TestObject::sTOCount == 15);
		EATEST_VERIFY(testVec.capacity() > 10);
		EATEST_VERIFY(testVec.validate());

		// test resize with args that does destruction of objects
		auto testVecCapacity = testVec.capacity();
		testVec.resize(5, true, TestObject(5), 5.0f);
		EATEST_VERIFY(testVec.size() == 5);
		EATEST_VERIFY(TestObject::sTOCount == 5);
		EATEST_VERIFY(testVec.capacity() == testVecCapacity);

		// test resize with args that does construction of objects
		testVec.resize(15, true, TestObject(5), 5.0f);
		EATEST_VERIFY(testVec.size() == 15);
		EATEST_VERIFY(TestObject::sTOCount == 15);
		EATEST_VERIFY(testVec.capacity() == testVecCapacity);

		// test resize with args that does construction of objects and grows the vector
		auto newTestVecSize = testVecCapacity + 5;
		testVec.resize(newTestVecSize, true, TestObject(5), 5.0f);
		EATEST_VERIFY(testVec.size() == newTestVecSize);
		EATEST_VERIFY(TestObject::sTOCount == newTestVecSize);
		EATEST_VERIFY(testVec.capacity() > newTestVecSize);
		EATEST_VERIFY(testVec.validate());
		for (unsigned int i = 5; i < newTestVecSize; ++i)
		{
			EATEST_VERIFY(testVec.get<0>()[i] == true);
			EATEST_VERIFY(testVec.get<1>()[i] == TestObject(5));
			EATEST_VERIFY(testVec.get<2>()[i] == 5.0f);
		}

		{
			tuple<bool, TestObject, float> resizeTup(true, TestObject(10), 10.0f);
			// test resize with tuple that does destruction of objects
			testVecCapacity = testVec.capacity();
			EATEST_VERIFY(testVecCapacity >= 15); // check for next two resizes to make sure we don't grow vec

			testVec.resize(20, resizeTup);
			EATEST_VERIFY(testVec.size() == 20);
			EATEST_VERIFY(TestObject::sTOCount == 20 + 1);
			EATEST_VERIFY(testVec.capacity() == testVecCapacity);

			// test resize with tuple that does construction of objects
			testVec.resize(25, resizeTup);
			EATEST_VERIFY(testVec.size() == 25);
			EATEST_VERIFY(TestObject::sTOCount == 25 + 1);
			EATEST_VERIFY(testVec.capacity() == testVecCapacity);

			// test resize with tuple that does construction of objects and grows the vector
			newTestVecSize = testVecCapacity + 5;
			testVec.resize(newTestVecSize, resizeTup);
			EATEST_VERIFY(testVec.size() == newTestVecSize);
			EATEST_VERIFY(TestObject::sTOCount == newTestVecSize + 1);
			EATEST_VERIFY(testVec.capacity() > newTestVecSize);
			EATEST_VERIFY(testVec.validate());
			for (unsigned int i = 5; i < 20; ++i)
			{
				EATEST_VERIFY(testVec.get<0>()[i] == true);
				EATEST_VERIFY(testVec.get<1>()[i] == TestObject(5));
				EATEST_VERIFY(testVec.get<2>()[i] == 5.0f);
			}
			for (unsigned int i = 20; i < testVecCapacity; ++i)
			{
				EATEST_VERIFY(testVec.get<0>()[i] == get<0>(resizeTup));
				EATEST_VERIFY(testVec.get<1>()[i] == get<1>(resizeTup));
				EATEST_VERIFY(testVec.get<2>()[i] == get<2>(resizeTup));
			}
		}

		// test other modifiers
		testVec.pop_back();
		EATEST_VERIFY(testVec.size() == newTestVecSize - 1);
		EATEST_VERIFY(TestObject::sTOCount == newTestVecSize - 1); // down 2 from last sTOCount check - resizeTup dtor and pop_back
		
		EATEST_VERIFY(testVec.capacity() > newTestVecSize);
		testVec.shrink_to_fit();
		EATEST_VERIFY(testVec.capacity() == testVec.size());
		EATEST_VERIFY(testVec.validate());

		testVec.clear();
		EATEST_VERIFY(testVec.empty());
		EATEST_VERIFY(testVec.validate());
		EATEST_VERIFY(TestObject::IsClear());

		testVec.shrink_to_fit();
		EATEST_VERIFY(testVec.capacity() == 0);
		EATEST_VERIFY(testVec.validate());
		TestObject::Reset();
	}

	{
		tuple_vector<bool, TestObject, float> testVec;

		// convoluted inserts to get "0, 1, 2, 3, 4, 5, 6" on the floats/testobject's
		auto testVecIter = testVec.insert(testVec.begin(), true, TestObject(5), 5.0f);
		testVec.insert(testVec.begin(), false, TestObject(4), 4.0f);
		testVec.insert(testVec.begin(), true, TestObject(1), 1.0f);
		testVecIter = testVec.insert(testVec.begin() + 1, false, TestObject(3), 3.0f);
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
		EATEST_VERIFY(testVec.validate());

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
		EATEST_VERIFY(testVec.validate());

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
			EATEST_VERIFY(ctorFromConstRef.validate());
			for (int i = 0; i < 10; ++i)
			{
				EATEST_VERIFY(ctorFromConstRef.get<0>()[i] == (i % 3 == 0));
				EATEST_VERIFY(ctorFromConstRef.get<1>()[i] == TestObject(i));
				EATEST_VERIFY(ctorFromConstRef.get<2>()[i] == (float)i);
			}
		}

		{
			tuple_vector<bool, TestObject, float> ctorFromAssignment;
			ctorFromAssignment = srcVec;
			EATEST_VERIFY(ctorFromAssignment.size() == 10);
			EATEST_VERIFY(ctorFromAssignment.validate());
			for (int i = 0; i < 10; ++i)
			{
				EATEST_VERIFY(ctorFromAssignment.get<0>()[i] == (i % 3 == 0));
				EATEST_VERIFY(ctorFromAssignment.get<1>()[i] == TestObject(i));
				EATEST_VERIFY(ctorFromAssignment.get<2>()[i] == (float)i);
			}
		}

		{
			tuple_vector<bool, TestObject, float> ctorFromIters(srcVec.begin() + 2, srcVec.begin() + 7);
			EATEST_VERIFY(ctorFromIters.size() == 5);
			EATEST_VERIFY(ctorFromIters.validate());
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
			EATEST_VERIFY(ctorFromFill.validate());
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
			EATEST_VERIFY(ctorFromFillArgs.validate());
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
			EATEST_VERIFY(ctorFromFillTup.validate());
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
		EATEST_VERIFY(testVec.validate_iterator(testVecIter) != isf_none);
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
		EATEST_VERIFY(complexVec.validate());
		EATEST_VERIFY(otherComplexVec.validate());
		EATEST_VERIFY(otherComplexVec.get<0>()[0] == 3);
		EATEST_VERIFY(otherComplexVec.get<float>()[1] == 4.0f);

		complexVec.push_back(10, 10.0f, true);
		swap(complexVec, otherComplexVec);

		EATEST_VERIFY(complexVec.validate());
		EATEST_VERIFY(*(complexVec.get<0>()) == 3);
		EATEST_VERIFY(complexVec.get<float>()[1] == 4.0f);

		EATEST_VERIFY(otherComplexVec.validate());
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
			EATEST_VERIFY(tripleElementVec.validate_iterator(iter) != isf_none);
			EATEST_VERIFY(tripleElementVec.validate_iterator(copiedIter) != isf_none);

			++iter;
			copiedIter = iter;
			EATEST_VERIFY(get<2>(*copiedIter) == 8);
			EATEST_VERIFY(tripleElementVec.validate_iterator(iter) != isf_none);
			EATEST_VERIFY(tripleElementVec.validate_iterator(copiedIter) != isf_none);

			++iter;
			swap(iter, copiedIter);
			EATEST_VERIFY(get<2>(*iter) == 8);
			EATEST_VERIFY(get<2>(*copiedIter) == 9);
			EATEST_VERIFY(tripleElementVec.validate_iterator(iter) != isf_none);
			EATEST_VERIFY(tripleElementVec.validate_iterator(copiedIter) != isf_none);

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
			EATEST_VERIFY(tripleElementVec.validate_iterator(iter) != isf_none);
			EATEST_VERIFY(tripleElementVec.validate_iterator(prefixIter) != isf_none);
			EATEST_VERIFY(tripleElementVec.validate_iterator(postfixIter) != isf_none);
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
			EATEST_VERIFY(tripleElementVec.validate_iterator(iter) != isf_none);
			EATEST_VERIFY(tripleElementVec.validate_iterator(prefixIter) != isf_none);
			EATEST_VERIFY(tripleElementVec.validate_iterator(postfixIter) != isf_none);
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
			EATEST_VERIFY(tripleElementVec.validate_iterator(iter) != isf_none);
			EATEST_VERIFY(tripleElementVec.validate_iterator(symmetryOne) != isf_none);
			EATEST_VERIFY(tripleElementVec.validate_iterator(symmetryTwo) != isf_none);
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
			tuple_vector<int, MoveOnlyType, TestObject> v1;
			tuple_vector<int, MoveOnlyType, TestObject> v2;

			// add some data in the vector so we can move it to the other vector.
			v1.reserve(5);
			for (int i = 0; i < 3; ++i)
			{
				auto emplacedTup = v1.emplace_back(i * 2 + 1, MoveOnlyType(i * 2 + 1), TestObject(i * 2 + 1));
			}
			v1.emplace(v1.end(), 6, MoveOnlyType(6), TestObject(6));
			v1.emplace(v1.begin() + 1, 2, MoveOnlyType(2), TestObject(2));
			v1.emplace(v1.begin() + 3, 4, MoveOnlyType(4), TestObject(4));

			tuple<int&, MoveOnlyType&, TestObject&> movedTup = v1.at(0);
			EATEST_VERIFY(v1.validate());
			EATEST_VERIFY(get<0>(movedTup) == 1);
			EATEST_VERIFY(get<0>(*v1.begin()) == 1);

			for (unsigned int i = 0; i < v1.size(); ++i)
			{
				EATEST_VERIFY(v1.get<0>()[i] == i + 1);
			}
			EATEST_VERIFY(!v1.empty() && v2.empty());
			v2 = eastl::move(v1);
			EATEST_VERIFY(v2.validate());
			EATEST_VERIFY(v1.empty() && !v2.empty());
			v1.swap(v2);
			EATEST_VERIFY(v1.validate());
			EATEST_VERIFY(v2.validate());
			EATEST_VERIFY(!v1.empty() && v2.empty());

			v2.insert(v2.begin(), *make_move_iterator(v1.begin()));
			EATEST_VERIFY(v1.validate());
			EATEST_VERIFY(v2.validate());
			EATEST_VERIFY(v2.size() == 1);
			EATEST_VERIFY(v1.size() == 6);
			EATEST_VERIFY(v1.get<2>()[0] == TestObject(0));
			EATEST_VERIFY(v2.get<2>()[0] == TestObject(1));
			EATEST_VERIFY(v1.get<1>()[0].mVal == 0);
			EATEST_VERIFY(v2.get<1>()[0].mVal == 1);

			v1.shrink_to_fit();
			v2.shrink_to_fit();
			EATEST_VERIFY(v1.validate());
			EATEST_VERIFY(v2.validate());
			EATEST_VERIFY(v2.size() == 1);
			EATEST_VERIFY(v1.size() == 6);
			EATEST_VERIFY(v1.get<2>()[0] == TestObject(0));
			EATEST_VERIFY(v2.get<2>()[0] == TestObject(1));
			EATEST_VERIFY(v1.get<1>()[0].mVal == 0);
			EATEST_VERIFY(v2.get<1>()[0].mVal == 1);

			tuple_vector<int, MoveOnlyType, TestObject> v3(eastl::move(v2));
			EATEST_VERIFY(v2.validate());
			EATEST_VERIFY(v3.validate());
			EATEST_VERIFY(v2.size() == 0);
			EATEST_VERIFY(v3.size() == 1);
			EATEST_VERIFY(v3.get<2>()[0] == TestObject(1));
			EATEST_VERIFY(v3.get<1>()[0].mVal == 1);
		}
		EATEST_VERIFY(TestObject::IsClear());
		TestObject::Reset();
	}

	// Test comparisons
	{
		tuple_vector<bool, TestObject, float> equalsVec1, equalsVec2;
		for (int i = 0; i < 10; ++i)
		{
			equalsVec1.push_back(i % 3 == 0, TestObject(i), (float)i);
			equalsVec2.push_back(i % 3 == 0, TestObject(i), (float)i);
		}
		EATEST_VERIFY(equalsVec1 == equalsVec2);

		tuple_vector<bool, TestObject, float> smallSizeVec(5);
		tuple_vector<bool, TestObject, float> lessThanVec(10);
		tuple_vector<bool, TestObject, float> greaterThanVec(10);
		for (int i = 0; i < 10; ++i)
		{
			lessThanVec.push_back(i % 3 == 0, TestObject(i), (float)i);
			greaterThanVec.push_back(i % 3 == 0, TestObject(i * 2), (float)i * 2);
		}
		EATEST_VERIFY(equalsVec1 != smallSizeVec);
		EATEST_VERIFY(equalsVec1 != lessThanVec);
		EATEST_VERIFY(equalsVec1 != greaterThanVec);
		EATEST_VERIFY(lessThanVec < greaterThanVec);
		EATEST_VERIFY(greaterThanVec > lessThanVec);
		EATEST_VERIFY(lessThanVec <= greaterThanVec);
		EATEST_VERIFY(equalsVec1 <= equalsVec2);
		EATEST_VERIFY(equalsVec1 >= equalsVec2);
	}

	// Test assign()
	{
		TestObject::Reset();

		tuple_vector<bool, TestObject, float> assignTest(10);

		// test the case where we shrink/erase elements using args
		{
			tuple<bool, TestObject, float> assignTup(true, TestObject(100), 100.0f);
			assignTest.assign(3, assignTup);
			EATEST_VERIFY(assignTest.validate());
			EATEST_VERIFY(assignTest.size() == 3);
			for (unsigned int i = 0; i < assignTest.size(); ++i)
			{
				EATEST_VERIFY(assignTest[i] == assignTup);
			}
		}

		// test expansion using args (w/o increasing capacity)
		{
			tuple<bool, TestObject, float> assignTup(false, TestObject(150), 150.0f);
			assignTest.assign(8, assignTup);
			EATEST_VERIFY(assignTest.validate());
			EATEST_VERIFY(assignTest.size() == 8);
			for (unsigned int i = 0; i < assignTest.size(); ++i)
			{
				EATEST_VERIFY(assignTest[i] == assignTup);
			}
		}
		
		// test expansion using args (w/ increasing capacity)
		{
			tuple<bool, TestObject, float> assignTup(true, TestObject(200), 200.0f);
			assignTest.assign(15, assignTup);
			EATEST_VERIFY(assignTest.validate());
			EATEST_VERIFY(assignTest.size() == 15);
			for (unsigned int i = 0; i < assignTest.size(); ++i)
			{
				EATEST_VERIFY(assignTest[i] == assignTup);
			}
		}
		assignTest.clear();
		
		assignTest.shrink_to_fit();
		EATEST_VERIFY(assignTest.validate());
		assignTest.resize(7);
		EATEST_VERIFY(assignTest.validate());
		EATEST_VERIFY(assignTest.capacity() == 7);

		tuple_vector<bool, TestObject, float> assignSrc;
		for (int i = 0; i < 10; ++i)
		{
			assignSrc.push_back(i % 3 == 0, TestObject(i), (float)i);
		}

		// test the case where we shrink/erase elements using iterators
		{
			assignTest.assign(assignSrc.begin() + 2, assignSrc.begin() + 5);
			EATEST_VERIFY(assignTest.validate());
			EATEST_VERIFY(assignTest.size() == 3);
			for (unsigned int i = 0; i < assignTest.size(); ++i)
			{
				EATEST_VERIFY(assignTest[i] == assignSrc[ i+ 2]);
			}
		}

		// test expansion using iterators (w/o increasing capacity)
		{
			assignTest.assign(assignSrc.begin() + 1, assignSrc.begin()+7);
			EATEST_VERIFY(assignTest.validate());
			EATEST_VERIFY(assignTest.size() == 6);
			for (unsigned int i = 0; i < assignTest.size(); ++i)
			{
				EATEST_VERIFY(assignTest[i] == assignSrc[i + 1]);
			}
		}

		// test expansion using iterators (w/ increasing capacity)
		{
			assignTest.assign(assignSrc.begin() + 2, assignSrc.end());
			EATEST_VERIFY(assignTest.validate());
			EATEST_VERIFY(assignTest.size() == 8);
			for (unsigned int i = 0; i < assignTest.size(); ++i)
			{
				EATEST_VERIFY(assignTest[i] == assignSrc[i + 2]);
			}
		}

		assignTest.clear();
		assignSrc.clear();

		EATEST_VERIFY(TestObject::IsClear());
		TestObject::Reset();
	}

	// Test insert() via iterators
	{
		TestObject::Reset();

		tuple_vector<bool, TestObject, float> insertTest(10);
		tuple_vector<bool, TestObject, float> insertSrc;
		for (int i = 0; i < 10; ++i)
		{
			insertSrc.push_back(i % 3 == 0, TestObject(i), (float)(i));
		}

		// test the case where we insert elements anywhere, with a realloc
		{
			tuple<bool, TestObject, float> defaultTup;
			EATEST_VERIFY(insertTest.capacity() == 10);
			insertTest.insert(insertTest.begin() + 3, insertSrc.begin() + 6, insertSrc.begin() + 9);
			EATEST_VERIFY(insertTest.validate());
			EATEST_VERIFY(insertTest.capacity() != 10);
			EATEST_VERIFY(insertTest.size() == 13);
			for (unsigned int i = 0; i < insertTest.size(); ++i)
			{
				if (i >= 3 && i < 6)
				{
					EATEST_VERIFY(insertTest[i] == insertSrc[i + 3]);
				}
				else
				{
					EATEST_VERIFY(insertTest[i] == defaultTup);
				}
			}
		}

		insertTest.clear();
		insertTest.reserve(30);
		insertTest.resize(5);
		{
			auto originalInsertTestPtrs = insertTest.data();
			// test the case where we insert elements to the end, without realloc
			tuple<bool, TestObject, float> defaultTup;
			insertTest.insert(insertTest.end(), insertSrc.begin() + 4, insertSrc.begin() + 9);
			EATEST_VERIFY(insertTest.validate());
			EATEST_VERIFY(insertTest.size() == 10);
			for (unsigned int i = 0; i < insertTest.size(); ++i)
			{
				if (i < 5)
				{
					EATEST_VERIFY(insertTest[i] == defaultTup);
				}
				else
				{
					EATEST_VERIFY(insertTest[i] == insertSrc[i - 1]);
				}
			}

			// test the case where we insert a couple elements to the middle, without a realloc
			insertTest.insert(insertTest.begin() + 5, insertSrc.begin() + 3, insertSrc.begin() + 5);
			EATEST_VERIFY(insertTest.validate());
			EATEST_VERIFY(insertTest.size() == 12);
			for (unsigned int i = 0; i < insertTest.size(); ++i)
			{
				if (i < 5)
				{
					EATEST_VERIFY(insertTest[i] == defaultTup);
				}
				else if (i < 7)
				{
					EATEST_VERIFY(insertTest[i] == insertSrc[i - 2]);
				}
				else
				{
					EATEST_VERIFY(insertTest[i] == insertSrc[i - 3]);
				}
			}

			// test the case where we insert a lot of elements near the end, without a realloc
			insertTest.insert(insertTest.begin() + 9, insertSrc.begin() + 2, insertSrc.begin() + 7);
			EATEST_VERIFY(insertTest.validate());
			EATEST_VERIFY(insertTest.size() == 17);
			for (unsigned int i = 0; i < insertTest.size(); ++i)
			{
				if (i < 5)
				{
					EATEST_VERIFY(insertTest[i] == defaultTup);
				}
				else if (i < 7)
				{
					EATEST_VERIFY(insertTest[i] == insertSrc[i - 2]);
				}
				else if (i < 9)
				{
					EATEST_VERIFY(insertTest[i] == insertSrc[i - 3]);
				}
				else if (i < 14)
				{
					EATEST_VERIFY(insertTest[i] == insertSrc[i - 7]);
				}
				else
				{
					EATEST_VERIFY(insertTest[i] == insertSrc[i - 8]);
				}
			}
			EATEST_VERIFY(originalInsertTestPtrs == insertTest.data());
		}

		insertTest.clear();
		insertSrc.clear();

		EATEST_VERIFY(TestObject::IsClear());
		TestObject::Reset();
	}

	return nErrorCount;
}


