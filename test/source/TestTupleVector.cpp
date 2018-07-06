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

	// Test resize and various modifications
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

	// Test insert
	{
		TestObject::Reset();

		// test insert with n values and lvalue args
		{
			tuple_vector<bool, TestObject, float> testVec;
			bool boolArg = true;
			TestObject toArg = TestObject(0);
			float floatArg = 0.0f;
			testVec.reserve(10);

			// test insert on empty vector that doesn't cause growth
			toArg = TestObject(3);
			floatArg = 3.0f;
			auto insertIter = testVec.insert(testVec.begin(), 3, boolArg, toArg, floatArg);
			EATEST_VERIFY(testVec.size() == 3);
			EATEST_VERIFY(insertIter == testVec.begin());

			// test insert to end of vector that doesn't cause growth
			toArg = TestObject(5);
			floatArg = 5.0f;
			insertIter = testVec.insert(testVec.end(), 3, boolArg, toArg, floatArg);
			EATEST_VERIFY(testVec.size() == 6);
			EATEST_VERIFY(insertIter == testVec.begin() + 3);

			// test insert to middle of vector that doesn't cause growth
			toArg = TestObject(4);
			floatArg = 4.0f;
			testVec.insert(testVec.begin() + 3, 3, boolArg, toArg, floatArg);
			EATEST_VERIFY(testVec.size() == 9);
			EATEST_VERIFY(testVec.capacity() == 10);

			// test insert to end of vector that causes growth
			toArg = TestObject(6);
			floatArg = 6.0f;
			testVec.insert(testVec.end(), 3, boolArg, toArg, floatArg);
			EATEST_VERIFY(testVec.size() == 12);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 12);

			// test insert to beginning of vector that causes growth
			toArg = TestObject(1);
			floatArg = 1.0f;
			testVec.insert(testVec.begin(), 3, boolArg, toArg, floatArg);
			EATEST_VERIFY(testVec.size() == 15);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 15);

			// test insert to middle of vector that causes growth
			toArg = TestObject(2);
			floatArg = 2.0f;
			testVec.insert(testVec.begin() + 3, 3, boolArg, toArg, floatArg);
			EATEST_VERIFY(testVec.size() == 18);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 18);

			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec.get<1>()[i] == TestObject(i / 3 + 1));
			}
			EATEST_VERIFY(testVec.validate());
		}

		// test insert with lvalue args
		{
			tuple_vector<bool, TestObject, float> testVec;
			bool boolArg = true;
			TestObject toArg = TestObject(0);
			float floatArg = 0.0f;
			testVec.reserve(3);

			// test insert on empty vector that doesn't cause growth
			toArg = TestObject(3);
			floatArg = 3.0f;
			testVec.insert(testVec.begin(), boolArg, toArg, floatArg);
			EATEST_VERIFY(testVec.size() == 1);

			// test insert to end of vector that doesn't cause growth
			toArg = TestObject(5);
			floatArg = 5.0f;
			testVec.insert(testVec.end(), boolArg, toArg, floatArg);
			EATEST_VERIFY(testVec.size() == 2);

			// test insert to middle of vector that doesn't cause growth
			toArg = TestObject(4);
			floatArg = 4.0f;
			testVec.insert(testVec.begin() + 1, boolArg, toArg, floatArg);
			EATEST_VERIFY(testVec.size() == 3);
			EATEST_VERIFY(testVec.capacity() == 3);

			// test insert to end of vector that causes growth
			toArg = TestObject(6);
			floatArg = 6.0f;
			testVec.insert(testVec.end(), boolArg, toArg, floatArg);
			EATEST_VERIFY(testVec.size() == 4);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 4);

			// test insert to beginning of vector that causes growth
			toArg = TestObject(1);
			floatArg = 1.0f;
			testVec.insert(testVec.begin(), boolArg, toArg, floatArg);
			EATEST_VERIFY(testVec.size() == 5);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 5);

			// test insert to middle of vector that causes growth
			toArg = TestObject(2);
			floatArg = 2.0f;
			testVec.insert(testVec.begin() + 1, boolArg, toArg, floatArg);
			EATEST_VERIFY(testVec.size() == 6);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 6);

			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec.get<1>()[i] == TestObject(i + 1));
			}
			EATEST_VERIFY(testVec.validate());
		}

		// test insert with n and tuple
		{
			tuple_vector<bool, TestObject, float> testVec;
			tuple<bool, TestObject, float> testTup;
			testVec.reserve(10);

			// test insert on empty vector that doesn't cause growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(3), 3.0f);
			testVec.insert(testVec.begin(), 3, testTup);
			EATEST_VERIFY(testVec.size() == 3);

			// test insert to end of vector that doesn't cause growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(5), 5.0f);
			testVec.insert(testVec.end(), 3, testTup);
			EATEST_VERIFY(testVec.size() == 6);

			// test insert to middle of vector that doesn't cause growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(4), 4.0f);
			testVec.insert(testVec.begin() + 3, 3, testTup);
			EATEST_VERIFY(testVec.size() == 9);
			EATEST_VERIFY(testVec.capacity() == 10);

			// test insert to end of vector that causes growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(6), 6.0f);
			testVec.insert(testVec.end(), 3, testTup);
			EATEST_VERIFY(testVec.size() == 12);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 12);

			// test insert to beginning of vector that causes growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(1), 1.0f);
			testVec.insert(testVec.begin(), 3, testTup);
			EATEST_VERIFY(testVec.size() == 15);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 15);

			// test insert to middle of vector that causes growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(2), 2.0f);
			testVec.insert(testVec.begin() + 3, 3, testTup);
			EATEST_VERIFY(testVec.size() == 18);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 18);

			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec.get<1>()[i] == TestObject(i / 3 + 1));
			}
			EATEST_VERIFY(testVec.validate());
		}

		// test insert with tuple
		{
			tuple_vector<bool, TestObject, float> testVec;
			tuple<bool, TestObject, float> testTup;
			testVec.reserve(3);

			// test insert on empty vector that doesn't cause growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(3), 3.0f);
			testVec.insert(testVec.begin(), testTup);
			EATEST_VERIFY(testVec.size() == 1);

			// test insert to end of vector that doesn't cause growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(5), 5.0f);
			testVec.insert(testVec.end(), testTup);
			EATEST_VERIFY(testVec.size() == 2);

			// test insert to middle of vector that doesn't cause growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(4), 4.0f);
			testVec.insert(testVec.begin() + 1, testTup);
			EATEST_VERIFY(testVec.size() == 3);
			EATEST_VERIFY(testVec.capacity() == 3);

			// test insert to end of vector that causes growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(6), 6.0f);
			testVec.insert(testVec.end(), 1, testTup);
			EATEST_VERIFY(testVec.size() == 4);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 4);

			// test insert to beginning of vector that causes growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(1), 1.0f);
			testVec.insert(testVec.begin(), 1, testTup);
			EATEST_VERIFY(testVec.size() == 5);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 5);

			// test insert to middle of vector that causes growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(2), 2.0f);
			testVec.insert(testVec.begin() + 1, 1, testTup);
			EATEST_VERIFY(testVec.size() == 6);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 6);

			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec.get<1>()[i] == TestObject(i + 1));
			}
			EATEST_VERIFY(testVec.validate());
		}

		// test insert with iterator range 
		{
			tuple_vector<bool, TestObject, float> srcVec;
			for (unsigned int i = 0; i < 20; ++i)
			{
				srcVec.push_back(true, TestObject(i), (float)i);
			}

			tuple_vector<bool, TestObject, float> testVec;
			testVec.reserve(10);

			// test insert on empty vector that doesn't cause growth
			testVec.insert(testVec.begin(), srcVec.begin() + 6, srcVec.begin() + 9);
			EATEST_VERIFY(testVec.size() == 3);

			// test insert to end of vector that doesn't cause growth
			testVec.insert(testVec.end(), srcVec.begin() + 12, srcVec.begin() + 15);
			EATEST_VERIFY(testVec.size() == 6);

			// test insert to middle of vector that doesn't cause growth
			testVec.insert(testVec.begin() + 3, srcVec.begin() + 9, srcVec.begin() + 12);
			EATEST_VERIFY(testVec.size() == 9);
			EATEST_VERIFY(testVec.capacity() == 10);

			// test insert to end of vector that causes growth
			testVec.insert(testVec.end(), srcVec.begin() + 15, srcVec.begin() + 18);
			EATEST_VERIFY(testVec.size() == 12);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 12);

			// test insert to beginning of vector that causes growth
			testVec.insert(testVec.begin(), srcVec.begin(), srcVec.begin() + 3);
			EATEST_VERIFY(testVec.size() == 15);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 15);

			// test insert to middle of vector that causes growth
			testVec.insert(testVec.begin() + 3, srcVec.begin() + 3, srcVec.begin() + 6);
			EATEST_VERIFY(testVec.size() == 18);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 18);

			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(i), (float)i));
			}
			EATEST_VERIFY(testVec.validate());
		}
		EATEST_VERIFY(TestObject::IsClear());
		TestObject::Reset();
	}

	// Test assign
	{
		{
			tuple_vector<bool, TestObject, float> testVec;

			// test assign that grows the capacity
			testVec.assign(20, true, TestObject(1), 1.0f);
			EATEST_VERIFY(testVec.size() == 20);
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(1), 1.0f));
			}
			EATEST_VERIFY(TestObject::sTOCount == 20);

			// test assign that shrinks the vector
			testVec.assign(10, true, TestObject(2), 2.0f);
			EATEST_VERIFY(testVec.size() == 10);
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(2), 2.0f));
			}
			EATEST_VERIFY(TestObject::sTOCount == 10);

			// test assign for when there's enough capacity
			testVec.assign(15, true, TestObject(3), 3.0f);
			EATEST_VERIFY(testVec.size() == 15);
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(3), 3.0f));
			}
			EATEST_VERIFY(TestObject::sTOCount == 15);
		}

		{
			tuple<bool, TestObject, float> srcTup;
			tuple_vector<bool, TestObject, float> testVec;

			// test assign from tuple that grows the capacity
			srcTup = make_tuple(true, TestObject(1), 1.0f);
			testVec.assign(20, srcTup);
			EATEST_VERIFY(testVec.size() == 20);
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec[i] == srcTup);
			}
			EATEST_VERIFY(TestObject::sTOCount == 20 + 1);

			// test assign from tuple that shrinks the vector
			srcTup = make_tuple(true, TestObject(2), 2.0f);
			testVec.assign(10, srcTup);
			EATEST_VERIFY(testVec.size() == 10);
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec[i] == srcTup);
			}
			EATEST_VERIFY(TestObject::sTOCount == 10 + 1);

			// test assign from tuple for when there's enough capacity
			srcTup = make_tuple(true, TestObject(3), 3.0f);
			testVec.assign(15, srcTup);
			EATEST_VERIFY(testVec.size() == 15);
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec[i] == srcTup);
			}
			EATEST_VERIFY(TestObject::sTOCount == 15 + 1);
		}

		{
			tuple_vector<bool, TestObject, float> srcVec;
			for (unsigned int i = 0; i < 20; ++i)
			{
				srcVec.push_back(true, TestObject(i), (float)i);
			}
			tuple_vector<bool, TestObject, float> testVec;

			// test assign from iter range that grows the capacity
			testVec.assign(srcVec.begin() + 5, srcVec.begin() + 15);
			EATEST_VERIFY(testVec.size() == 10);
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec[i] == srcVec[i+5]);
			}
			EATEST_VERIFY(TestObject::sTOCount == 10 + 20);

			// test assign from iter range that shrinks the vector
			testVec.assign(srcVec.begin() + 2, srcVec.begin() + 7);
			EATEST_VERIFY(testVec.size() == 5);
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec[i] == srcVec[i + 2]);
			}
			EATEST_VERIFY(TestObject::sTOCount == 5 + 20);

			// test assign from iter range for when there's enough capacity
			testVec.assign(srcVec.begin() + 5, srcVec.begin() + 15);
			EATEST_VERIFY(testVec.size() == 10);
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec[i] == srcVec[i + 5]);
			}
			EATEST_VERIFY(TestObject::sTOCount == 10 + 20);
		}
		EATEST_VERIFY(TestObject::IsClear());
		TestObject::Reset();
	}

	// Test erase functions
	{
		{
			tuple_vector<bool, TestObject, float> srcVec;
			for (unsigned int i = 0; i < 20; ++i)
			{
				srcVec.push_back(true, TestObject(i), (float)i);
			}
			tuple_vector<bool, TestObject, float> testVec;

			// test erase on an iter range
			testVec.assign(srcVec.begin(), srcVec.end());
			auto eraseIter = testVec.erase(testVec.begin() + 5, testVec.begin() + 10);
			EATEST_VERIFY(eraseIter == testVec.begin() + 5);
			EATEST_VERIFY(testVec.size() == 15);
			EATEST_VERIFY(testVec.validate());
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				if (i < 5)
					EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(i), (float)i));
				else
					EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(i + 5), (float)(i + 5)));
			}
			EATEST_VERIFY(TestObject::sTOCount == 15 + 20);

			// test erase on one position
			testVec.assign(srcVec.begin(), srcVec.end());
			eraseIter = testVec.erase(testVec.begin() + 5);
			EATEST_VERIFY(eraseIter == testVec.begin() + 5);
			EATEST_VERIFY(testVec.size() == 19);
			EATEST_VERIFY(testVec.validate());
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				if (i < 5)
					EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(i), (float)i));
				else
					EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(i + 1), (float)(i + 1)));
			}
			EATEST_VERIFY(TestObject::sTOCount == 19 + 20);

			// test erase_unsorted
			testVec.assign(srcVec.begin(), srcVec.end());
			eraseIter = testVec.erase_unsorted(testVec.begin() + 5);
			EATEST_VERIFY(eraseIter == testVec.begin() + 5);
			EATEST_VERIFY(testVec.size() == 19);
			EATEST_VERIFY(testVec.validate());
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				if (i != 5)
					EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(i), (float)i));
				else
					EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(19), (float)(19)));
			}
			EATEST_VERIFY(TestObject::sTOCount == 19 + 20);
		}

		// test erase again but with reverse iterators everywhere
		{
			tuple_vector<bool, TestObject, float> srcVec;
			for (unsigned int i = 0; i < 20; ++i)
			{
				srcVec.push_back(true, TestObject(i), (float)i);
			}
			tuple_vector<bool, TestObject, float> testVec;

			// test erase on an iter range
			testVec.assign(srcVec.begin(), srcVec.end());
			auto eraseIter = testVec.erase(testVec.rbegin() + 5, testVec.rbegin() + 10);
			EATEST_VERIFY(eraseIter == testVec.rbegin() + 5);
			EATEST_VERIFY(testVec.size() == 15);
			EATEST_VERIFY(testVec.validate());
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				if (i < 10)
					EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(i), (float)i));
				else
					EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(i + 5), (float)(i + 5)));
			}
			EATEST_VERIFY(TestObject::sTOCount == 15 + 20);

			// test erase on one position
			testVec.assign(srcVec.begin(), srcVec.end());
			eraseIter = testVec.erase(testVec.rbegin() + 5);
			EATEST_VERIFY(eraseIter == testVec.rbegin() + 5);
			EATEST_VERIFY(testVec.size() == 19);
			EATEST_VERIFY(testVec.validate());
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				if (i < 14)
					EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(i), (float)i));
				else
					EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(i + 1), (float)(i + 1)));
			}
			EATEST_VERIFY(TestObject::sTOCount == 19 + 20);

			// test erase_unsorted
			testVec.assign(srcVec.begin(), srcVec.end());
			eraseIter = testVec.erase_unsorted(testVec.rbegin() + 5);
			EATEST_VERIFY(eraseIter == testVec.rbegin() + 5);
			EATEST_VERIFY(testVec.size() == 19);
			EATEST_VERIFY(testVec.validate());
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				if (i != 14)
					EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(i), (float)i));
				else
					EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(19), (float)(19)));
			}
			EATEST_VERIFY(TestObject::sTOCount == 19 + 20);
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


