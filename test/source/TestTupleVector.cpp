/////////////////////////////////////////////////////////////////////////////
// TestTupleVector.cpp
//
// Copyright (c) 2018, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"

#include <EASTL/bonus/tuple_vector.h>

#include <EASTL/sort.h>

using namespace eastl;

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

			// check that return types of const-version of begin and cbegin (etc) match
			static_assert(eastl::is_same<decltype(constVec.begin()), decltype(constVec.cbegin())>::value, "error");
			static_assert(eastl::is_same<decltype(constVec.end()), decltype(constVec.cend())>::value, "error");
			static_assert(eastl::is_same<decltype(constVec.rbegin()), decltype(constVec.crbegin())>::value, "error");
			static_assert(eastl::is_same<decltype(constVec.rend()), decltype(constVec.crend())>::value, "error");

			// check that return type of non-const version of begin and cbegin (etc) do _not_ match
			static_assert(!eastl::is_same<decltype(complexVec.begin()), decltype(complexVec.cbegin())>::value, "error");
			static_assert(!eastl::is_same<decltype(complexVec.end()), decltype(complexVec.cend())>::value, "error");
			static_assert(!eastl::is_same<decltype(complexVec.rbegin()), decltype(complexVec.crbegin())>::value, "error");
			static_assert(!eastl::is_same<decltype(complexVec.rend()), decltype(complexVec.crend())>::value, "error");

		}
	}

	// test the memory layouts work for aligned structures
	{
		struct EA_ALIGN(16) AlignTestVec4
		{
			float a[4];
			AlignTestVec4() : a{1.0f, 2.0f, 3.0f, 4.0f} {}
		};

		struct AlignTestByte3
		{
			char a[3];
			AlignTestByte3() : a{1, 2, 3} {}
		};

		struct EA_ALIGN(8) AlignTestFourByte
		{
			int a[5];
			AlignTestFourByte() : a{-1, -2, -3, -4, -5} {}
		};

		tuple_vector<bool, AlignTestVec4, AlignTestByte3, AlignTestFourByte> alignElementVec;
		alignElementVec.push_back();
		alignElementVec.push_back();
		alignElementVec.push_back();
		alignElementVec.push_back();
		alignElementVec.push_back();

		EATEST_VERIFY((uintptr_t)alignElementVec.get<AlignTestVec4>() % 16 == 0);
		EATEST_VERIFY((uintptr_t)alignElementVec.get<AlignTestFourByte>() % 8 == 0);
	}

	// Test resize and various modifications
	{
		TestObject::Reset();

		tuple_vector<bool, TestObject, float> testVec;
		typedef tuple_vector<bool, TestObject, float>::size_type tuple_vector_size_type;
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
		EATEST_VERIFY(static_cast<tuple_vector_size_type>(TestObject::sTOCount) == newTestVecSize);
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
			typedef tuple_vector<bool, TestObject, float>::size_type tuple_vector_size_type;

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
			EATEST_VERIFY(static_cast<tuple_vector_size_type>(TestObject::sTOCount) == newTestVecSize + 1);
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
		EATEST_VERIFY(static_cast<decltype(testVec)::size_type>(TestObject::sTOCount) == newTestVecSize - 1); // down 2 from last sTOCount check - resizeTup dtor and pop_back
		
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

		// test insert with initList
		{
			tuple_vector<bool, TestObject, float> testVec;
			tuple<bool, TestObject, float> testTup;
			testVec.reserve(10);

			// test insert on empty vector that doesn't cause growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(3), 3.0f);
			testVec.insert(testVec.begin(), { 
				{true, TestObject(3), 3.0f},
				testTup,
				{true, TestObject(3), 3.0f}
				});
			EATEST_VERIFY(testVec.size() == 3);

			// test insert to end of vector that doesn't cause growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(5), 5.0f);
			testVec.insert(testVec.end(), { 
				{true, TestObject(5), 5.0f}, 
				testTup, 
				{true, TestObject(5), 5.0f} 
				});
			EATEST_VERIFY(testVec.size() == 6);

			// test insert to middle of vector that doesn't cause growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(4), 4.0f);
			testVec.insert(testVec.begin() + 3, {
				{true, TestObject(4), 4.0f}, 
				testTup, 
				{true, TestObject(4), 4.0f}
				});
			EATEST_VERIFY(testVec.size() == 9);
			EATEST_VERIFY(testVec.capacity() == 10);

			// test insert to end of vector that causes growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(6), 6.0f);
			testVec.insert(testVec.end(), { 
				{true, TestObject(6), 6.0f},
				testTup,
				{true, TestObject(6), 6.0f}
				});
			EATEST_VERIFY(testVec.size() == 12);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 12);

			// test insert to beginning of vector that causes growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(1), 1.0f);
			testVec.insert(testVec.begin(), { 
				{true, TestObject(1), 1.0f}, 
				testTup, 
				{true, TestObject(1), 1.0f}
				});
			EATEST_VERIFY(testVec.size() == 15);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 15);

			// test insert to middle of vector that causes growth
			testTup = tuple<bool, TestObject, float>(true, TestObject(2), 2.0f);
			testVec.insert(testVec.begin() + 3, { 
				{true, TestObject(2), 2.0f},
				testTup,
				{true, TestObject(2), 2.0f
				} });
			EATEST_VERIFY(testVec.size() == 18);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 18);

			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec.get<1>()[i] == TestObject(i / 3 + 1));
			}
			EATEST_VERIFY(testVec.validate());
		}

		// test insert with rvalue args
		{
			tuple_vector<int, MoveOnlyType, TestObject> testVec;
			testVec.reserve(3);

			// test insert on empty vector that doesn't cause growth
			testVec.insert(testVec.begin(), 3, MoveOnlyType(3), TestObject(3));
			EATEST_VERIFY(testVec.size() == 1);

			// test insert to end of vector that doesn't cause growth
			testVec.insert(testVec.end(), 5, MoveOnlyType(5), TestObject(5));
			EATEST_VERIFY(testVec.size() == 2);

			// test insert to middle of vector that doesn't cause growth
			testVec.insert(testVec.begin() + 1, 4, MoveOnlyType(4), TestObject(4));
			EATEST_VERIFY(testVec.size() == 3);
			EATEST_VERIFY(testVec.capacity() == 3);

			// test insert to end of vector that causes growth
			testVec.insert(testVec.end(), 6, MoveOnlyType(6), TestObject(6));
			EATEST_VERIFY(testVec.size() == 4);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 4);

			// test insert to beginning of vector that causes growth
			testVec.insert(testVec.begin(), 1, MoveOnlyType(1), TestObject(1));
			EATEST_VERIFY(testVec.size() == 5);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 5);

			// test insert to middle of vector that causes growth
			testVec.insert(testVec.begin() + 1, 2, MoveOnlyType(2), TestObject(2));
			EATEST_VERIFY(testVec.size() == 6);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 6);

			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec.get<2>()[i] == TestObject(i + 1));
			}
			EATEST_VERIFY(testVec.validate());
		}

		// test insert with rvalue tuple
		{
			tuple_vector<int, MoveOnlyType, TestObject> testVec;
			testVec.reserve(3);

			// test insert on empty vector that doesn't cause growth
			testVec.insert(testVec.begin(), forward_as_tuple(3, MoveOnlyType(3), TestObject(3)));
			EATEST_VERIFY(testVec.size() == 1);

			// test insert to end of vector that doesn't cause growth
			testVec.insert(testVec.end(), forward_as_tuple(5, MoveOnlyType(5), TestObject(5)));
			EATEST_VERIFY(testVec.size() == 2);

			// test insert to middle of vector that doesn't cause growth
			testVec.insert(testVec.begin() + 1, forward_as_tuple(4, MoveOnlyType(4), TestObject(4)));
			EATEST_VERIFY(testVec.size() == 3);
			EATEST_VERIFY(testVec.capacity() == 3);

			// test insert to end of vector that causes growth
			testVec.insert(testVec.end(), forward_as_tuple(6, MoveOnlyType(6), TestObject(6)));
			EATEST_VERIFY(testVec.size() == 4);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 4);

			// test insert to beginning of vector that causes growth
			testVec.insert(testVec.begin(), forward_as_tuple(1, MoveOnlyType(1), TestObject(1)));
			EATEST_VERIFY(testVec.size() == 5);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 5);

			// test insert to middle of vector that causes growth
			testVec.insert(testVec.begin() + 1, forward_as_tuple(2, MoveOnlyType(2), TestObject(2)));
			EATEST_VERIFY(testVec.size() == 6);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 6);

			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec.get<2>()[i] == TestObject(i + 1));
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

		{
			tuple_vector<bool, TestObject, float> testVec;

			// test assign from initList that grows the capacity
			testVec.assign({
				{ true, TestObject(1), 1.0f }, 
				{ true, TestObject(2), 2.0f },
				{ true, TestObject(3), 3.0f }
				});
			EATEST_VERIFY(testVec.size() == 3);
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(i + 1), (float)i + 1.0f));
			}
			EATEST_VERIFY(TestObject::sTOCount == 3);

			// test assign from initList that shrinks the vector
			testVec.assign({
				{ true, TestObject(4), 4.0f }
				});
			EATEST_VERIFY(testVec.size() == 1);
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(i + 4), (float)i + 4.0f));
			}
			EATEST_VERIFY(TestObject::sTOCount == 1);

			// test assign from initList for when there's enough capacity
			testVec.assign({
				{ true, TestObject(5), 5.0f },
				{ true, TestObject(6), 6.0f }
				});
			EATEST_VERIFY(testVec.size() == 2);
			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec[i] == make_tuple(true, TestObject(i + 5), (float)i + 5.0f));
			}
			EATEST_VERIFY(TestObject::sTOCount == 2);
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
		MallocAllocator ma;
		TestObject::Reset();

		// test ctor via initlist to prime srcVec
		tuple_vector<bool, TestObject, float> srcVec({
			{ true,  TestObject(0), 0.0f},
			{ false, TestObject(1), 1.0f},
			{ false, TestObject(2), 2.0f},
			{ true,  TestObject(3), 3.0f},
			{ false, TestObject(4), 4.0f},
			{ false, TestObject(5), 5.0f},
			{ true,  TestObject(6), 6.0f},
			{ false, TestObject(7), 7.0f},
			{ false, TestObject(8), 8.0f},
			{ true,  TestObject(9), 9.0f}
			});

		// copy entire tuple_vector in ctor
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

		// copy entire tuple_vector via assignment
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

		// copy entire tuple_vector via assignment of init-list
		{
			tuple_vector<bool, TestObject, float> ctorFromAssignment;
			ctorFromAssignment = { 
				{ true,  TestObject(0), 0.0f},
				{ false, TestObject(1), 1.0f},
				{ false, TestObject(2), 2.0f},
				{ true,  TestObject(3), 3.0f},
				{ false, TestObject(4), 4.0f},
				{ false, TestObject(5), 5.0f},
				{ true,  TestObject(6), 6.0f},
				{ false, TestObject(7), 7.0f},
				{ false, TestObject(8), 8.0f},
				{ true,  TestObject(9), 9.0f}
			};
			EATEST_VERIFY(ctorFromAssignment.size() == 10);
			EATEST_VERIFY(ctorFromAssignment.validate());
			for (int i = 0; i < 10; ++i)
			{
				EATEST_VERIFY(ctorFromAssignment.get<0>()[i] == (i % 3 == 0));
				EATEST_VERIFY(ctorFromAssignment.get<1>()[i] == TestObject(i));
				EATEST_VERIFY(ctorFromAssignment.get<2>()[i] == (float)i);
			}
		}

		// ctor tuple_vector with iterator range
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

		// ctor tuple_vector with initial size
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

		// ctor tuple_vector with initial size and args
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

		// ctor tuple_vector with initial size and tuple
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

		// ctor tuple_Vector with custom mallocator
		{
			tuple_vector_alloc<MallocAllocator, bool, TestObject, float> ctorWithAlloc(ma);
			tuple_vector<bool, TestObject, float> ctorDefault;

			ctorWithAlloc.push_back();
			ctorDefault.push_back();

			EATEST_VERIFY(ctorWithAlloc == ctorDefault);
			EATEST_VERIFY(ctorWithAlloc.validate());
		}

		// ctor tuple_vector_alloc with copy (from diff. allocator)
		{
			tuple_vector_alloc<MallocAllocator, bool, TestObject, float> ctorFromConstRef(srcVec, ma);
			EATEST_VERIFY(ctorFromConstRef.size() == 10);
			EATEST_VERIFY(ctorFromConstRef.validate());
			for (int i = 0; i < 10; ++i)
			{
				EATEST_VERIFY(ctorFromConstRef.get<0>()[i] == (i % 3 == 0));
				EATEST_VERIFY(ctorFromConstRef.get<1>()[i] == TestObject(i));
				EATEST_VERIFY(ctorFromConstRef.get<2>()[i] == (float)i);
			}
			EATEST_VERIFY(ctorFromConstRef.validate());
		}

		// ctor tuple_vector with initial size and args
		{
			tuple_vector_alloc<MallocAllocator, bool, TestObject, float> ctorFromFillArgs(10, true, TestObject(5), 5.0f, ma);
			EATEST_VERIFY(ctorFromFillArgs.size() == 10);
			EATEST_VERIFY(ctorFromFillArgs.validate());
			for (int i = 0; i < 10; ++i)
			{
				EATEST_VERIFY(ctorFromFillArgs.get<0>()[i] == true);
				EATEST_VERIFY(ctorFromFillArgs.get<1>()[i] == TestObject(5));
				EATEST_VERIFY(ctorFromFillArgs.get<2>()[i] == 5.0f);
			}
		}

		// ctor tuple_vector via move
		{
			tuple_vector<int, MoveOnlyType, TestObject> srcMoveVec;
			for (int i = 0; i < 10; ++i)
			{
				srcMoveVec.emplace_back(move(i), MoveOnlyType(i), TestObject(i));
			}

			tuple_vector<int, MoveOnlyType, TestObject> ctorFromMove(move(srcMoveVec));

			EATEST_VERIFY(ctorFromMove.size() == 10);
			EATEST_VERIFY(ctorFromMove.validate());
			for (int i = 0; i < 10; ++i)
			{
				EATEST_VERIFY(ctorFromMove.get<0>()[i] == i);
				EATEST_VERIFY(ctorFromMove.get<1>()[i] == MoveOnlyType(i));
				EATEST_VERIFY(ctorFromMove.get<2>()[i] == TestObject(i));
			}
			EATEST_VERIFY(srcMoveVec.size() == 0);
			EATEST_VERIFY(srcMoveVec.validate());
		}

		// ctor tuple_vector via move (from diff. allocator)
		{
			tuple_vector_alloc<MallocAllocator, int, MoveOnlyType, TestObject> srcMoveVec;
			for (int i = 0; i < 10; ++i)
			{
				srcMoveVec.emplace_back(move(i), MoveOnlyType(i), TestObject(i));
			}

			MallocAllocator otherMa;
			tuple_vector_alloc<MallocAllocator, int, MoveOnlyType, TestObject> ctorFromMove(move(srcMoveVec), otherMa);

			EATEST_VERIFY(ctorFromMove.size() == 10);
			EATEST_VERIFY(ctorFromMove.validate());
			for (int i = 0; i < 10; ++i)
			{
				EATEST_VERIFY(ctorFromMove.get<0>()[i] == i);
				EATEST_VERIFY(ctorFromMove.get<1>()[i] == MoveOnlyType(i));
				EATEST_VERIFY(ctorFromMove.get<2>()[i] == TestObject(i));
			}
			EATEST_VERIFY(srcMoveVec.size() == 0);
			EATEST_VERIFY(srcMoveVec.validate());
			
			// bonus test for specifying a custom allocator, but using the same one as above
			tuple_vector_alloc<MallocAllocator, int, MoveOnlyType, TestObject> ctorFromMoveSameAlloc(move(ctorFromMove), otherMa);
			EATEST_VERIFY(ctorFromMoveSameAlloc.size() == 10);
			EATEST_VERIFY(ctorFromMoveSameAlloc.validate());
			for (int i = 0; i < 10; ++i)
			{
				EATEST_VERIFY(ctorFromMoveSameAlloc.get<0>()[i] == i);
				EATEST_VERIFY(ctorFromMoveSameAlloc.get<1>()[i] == MoveOnlyType(i));
				EATEST_VERIFY(ctorFromMoveSameAlloc.get<2>()[i] == TestObject(i));
			}
			EATEST_VERIFY(ctorFromMove.size() == 0);
			EATEST_VERIFY(ctorFromMove.validate());
		}

		// ctor tuple_vector via move-iters
		{
			tuple_vector<int, MoveOnlyType, TestObject> srcMoveVec;
			for (int i = 0; i < 10; ++i)
			{
				srcMoveVec.emplace_back(move(i), MoveOnlyType(i), TestObject(i));
			}

			tuple_vector<int, MoveOnlyType, TestObject> ctorFromMove(make_move_iterator(srcMoveVec.begin() + 2), make_move_iterator(srcMoveVec.begin() + 7));

			EATEST_VERIFY(ctorFromMove.size() == 5);
			EATEST_VERIFY(ctorFromMove.validate());
			for (int i = 2; i < 7; ++i)
			{
				EATEST_VERIFY(ctorFromMove.get<0>()[i-2] == i);
				EATEST_VERIFY(ctorFromMove.get<1>()[i-2] == MoveOnlyType(i));
				EATEST_VERIFY(ctorFromMove.get<2>()[i-2] == TestObject(i));
			}
			EATEST_VERIFY(srcMoveVec.size() == 10);
			EATEST_VERIFY(srcMoveVec.validate());
			for (int i = 0; i < 2; ++i)
			{
				EATEST_VERIFY(srcMoveVec.get<0>()[i] == i);
				EATEST_VERIFY(srcMoveVec.get<1>()[i] == MoveOnlyType(i));
				EATEST_VERIFY(srcMoveVec.get<2>()[i] == TestObject(i));
			}
			for (int i = 2; i < 7; ++i)
			{
				EATEST_VERIFY(srcMoveVec.get<0>()[i] == i); // int's just get copied because they're POD
				EATEST_VERIFY(srcMoveVec.get<1>()[i] == MoveOnlyType(0));
				EATEST_VERIFY(srcMoveVec.get<2>()[i] == TestObject(0));
			}
			for (int i = 7; i < 10; ++i)
			{
				EATEST_VERIFY(srcMoveVec.get<0>()[i] == i);
				EATEST_VERIFY(srcMoveVec.get<1>()[i] == MoveOnlyType(i));
				EATEST_VERIFY(srcMoveVec.get<2>()[i] == TestObject(i));
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
			for (auto iter : tripleElementVec)
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

	// Test move operations
	{
		TestObject::Reset();

		// test emplace 
		{
			tuple_vector<int, MoveOnlyType, TestObject> testVec;
			testVec.reserve(3);

			// test emplace on empty vector that doesn't cause growth
			testVec.emplace(testVec.begin(), 3, MoveOnlyType(3), TestObject(3));
			EATEST_VERIFY(testVec.size() == 1);

			// test emplace to end of vector that doesn't cause growth
			testVec.emplace(testVec.end(), 5, MoveOnlyType(5), TestObject(5));
			EATEST_VERIFY(testVec.size() == 2);

			// test emplace to middle of vector that doesn't cause growth
			testVec.emplace(testVec.begin() + 1, 4, MoveOnlyType(4), TestObject(4));
			EATEST_VERIFY(testVec.size() == 3);
			EATEST_VERIFY(testVec.capacity() == 3);

			// test emplace to end of vector that causes growth
			testVec.emplace(testVec.end(), 6, MoveOnlyType(6), TestObject(6));
			EATEST_VERIFY(testVec.size() == 4);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 4);

			// test emplace to beginning of vector that causes growth
			testVec.emplace(testVec.begin(), 1, MoveOnlyType(1), TestObject(1));
			EATEST_VERIFY(testVec.size() == 5);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 5);

			// test emplace to middle of vector that causes growth
			testVec.emplace(testVec.begin() + 1, 2, MoveOnlyType(2), TestObject(2));
			EATEST_VERIFY(testVec.size() == 6);
			testVec.shrink_to_fit();
			EATEST_VERIFY(testVec.capacity() == 6);

			for (unsigned int i = 0; i < testVec.size(); ++i)
			{
				EATEST_VERIFY(testVec.get<2>()[i] == TestObject(i + 1));
			}
			EATEST_VERIFY(testVec.validate());
		}
	
		// test some other miscellania around rvalues, including...
		// push_back with rvalue args, push_back with rvalue tuple,
		// emplace_back with args, and emplace_back with tup
		{
			tuple_vector<int, MoveOnlyType, TestObject> v1;
			tuple_vector<int, MoveOnlyType, TestObject> v2;
			// add some data in the vector so we can move it to the other vector.
			v1.reserve(5);
			auto emplacedTup = v1.emplace_back(1, MoveOnlyType(1), TestObject(1));
			EATEST_VERIFY(emplacedTup == v1.back());
			v1.push_back(3, MoveOnlyType(3), TestObject(3));
			v1.emplace_back(forward_as_tuple(5, MoveOnlyType(5), TestObject(5)));
			v1.push_back(forward_as_tuple(6, MoveOnlyType(6), TestObject(6)));
			v1.emplace(v1.begin() + 1, 2, MoveOnlyType(2), TestObject(2));
			v1.emplace(v1.begin() + 3, make_tuple(4, MoveOnlyType(4), TestObject(4)));

			tuple<int&, MoveOnlyType&, TestObject&> movedTup = v1.at(0);
			EATEST_VERIFY(v1.validate());
			EATEST_VERIFY(get<0>(movedTup) == 1);
			EATEST_VERIFY(get<0>(*v1.begin()) == 1);

			for (int i = 0; i < static_cast<int>(v1.size()); ++i)
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
		}
		EATEST_VERIFY(TestObject::IsClear());
		TestObject::Reset();
	}

	// Test comparisons
	{
		MallocAllocator ma;
		tuple_vector<bool, TestObject, float> equalsVec1, equalsVec2;
		for (int i = 0; i < 10; ++i)
		{
			equalsVec1.push_back(i % 3 == 0, TestObject(i), (float)i);
			equalsVec2.push_back(i % 3 == 0, TestObject(i), (float)i);
		}
		EATEST_VERIFY(equalsVec1 == equalsVec2);

		tuple_vector<bool, TestObject, float> smallSizeVec(5);
		tuple_vector<bool, TestObject, float> lessThanVec(10);
		tuple_vector_alloc<MallocAllocator, bool, TestObject, float> greaterThanVec(10, ma);
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

	// Test partition
	{
		{
			tuple_vector<bool, TestObject, float, MoveOnlyType> vec;
			for (int i = 0; i < 10; ++i)
			{
				vec.push_back(i % 3 == 0, TestObject(i), (float)i, MoveOnlyType(i));
			}

			eastl::partition(vec.begin(), vec.end(), [](tuple<bool&, TestObject&, float&, MoveOnlyType&> a)
			{ return get<0>(a) == true; });

			// partition will split the array into 4 elements where the bool property is true, and 6 where it's false
			for (int i = 0; i < 4; ++i)
				EATEST_VERIFY(vec.get<0>()[i] == true);
			for (int i = 4; i < 10; ++i)
				EATEST_VERIFY(vec.get<0>()[i] == false);

			EATEST_VERIFY(vec.validate());
			EATEST_VERIFY(TestObject::sTOCount == 10);
		}
		EATEST_VERIFY(TestObject::IsClear());
		TestObject::Reset();
	}

	// Test allocator manipulation
	{
		InstanceAllocator ia0((uint8_t)0), ia1((uint8_t)1);
		tuple_vector_alloc<InstanceAllocator, int> vec(ia0);

		// private vector allocator was copied from ia0 and should have matching id
		EATEST_VERIFY(vec.get_allocator() == ia0);

		// Assigning allocator
		vec.set_allocator(ia1);
		EATEST_VERIFY(vec.get_allocator() == ia1);
	}

	return nErrorCount;
}


