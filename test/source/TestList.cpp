/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Portions of this unit test:
//
// Copyright (C) 2001, 2003, 2004, 2005 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without Pred the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/list.h>
#include <EASTL/sort.h>
#include <EAStdC/EAStopwatch.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stdio.h>

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


using namespace eastl;



namespace
{
	// A nontrivial type.
	template <typename TestObject>
	struct A { };


	// Another nontrivial type
	struct B { };

	inline bool operator==(const B&, const B&)
		{ return true; }

	inline bool operator<(const B& b1, const B& b2)
		{ return &b1 < &b2; }


	// A nontrivial type convertible from an int
	struct C
	{
		C(int c = 0) : mC(c) { }
		bool operator==(const C& rhs) { return mC == rhs.mC; }
		int mC;
	};

	struct D
	{
		D() = delete;
	};
}




// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::list<int>;
template class eastl::list<B>;
template class eastl::list<Align64>;


// Test compiler issue that appeared in VS2012 relating to kAlignment
struct StructWithContainerOfStructs
{
	eastl::list<StructWithContainerOfStructs> children;
};


int TestList()
{
	EASTLTest_Printf("TestList\n");

	int nErrorCount = 0;

	{
		// test T with a deleted default-ctor
		eastl::list<D> dlist;

	}

	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();

	{
		list<int> list0101;
		EATEST_VERIFY(list0101.empty());
		EATEST_VERIFY(list0101.size() == 0);

		list0101.push_back(1);
		EATEST_VERIFY(!list0101.empty());
		EATEST_VERIFY(list0101.size() == 1);

		list0101.resize(3, 2);
		EATEST_VERIFY(!list0101.empty());
		EATEST_VERIFY(list0101.size() == 3);

		list<int>::iterator i = list0101.begin();
		EATEST_VERIFY(*i == 1); ++i;
		EATEST_VERIFY(*i == 2); ++i;
		EATEST_VERIFY(*i == 2); ++i;
		EATEST_VERIFY(i == list0101.end());

		list0101.resize(0);
		EATEST_VERIFY(list0101.empty());
		EATEST_VERIFY(list0101.size() == 0);

		EATEST_VERIFY(list0101.validate());
	}

	{
		// Aligned objects should be CustomAllocator instead of the default, because the 
		// EASTL default might be unable to do aligned allocations, but CustomAllocator always can.
		list<Align64, CustomAllocator> listAlign64;

		listAlign64.push_back(Align64());
		EATEST_VERIFY(listAlign64.size() == 1);

		Align64& Align64_1 = listAlign64.front();
		EATEST_VERIFY(((uintptr_t)&Align64_1 %  kEASTLTestAlign64) == 0);

		Align64& Align64_2 = listAlign64.back();
		EATEST_VERIFY(((uintptr_t)&Align64_2 % kEASTLTestAlign64) == 0);

		EATEST_VERIFY(listAlign64.validate());
	}

	{
		list<int> list1;
		list<int> list2;
		list<int> list3;

		list1.push_back(1);
		list3 = list1;

		EATEST_VERIFY(list1 == list3);
		EATEST_VERIFY(list1 != list2);
		EATEST_VERIFY((list1 < list2) || (list1 > list2));
		EATEST_VERIFY((list1 <= list2) || (list1 >= list2));

		EATEST_VERIFY(list1.validate());
		EATEST_VERIFY(list2.validate());
		EATEST_VERIFY(list3.validate());
	}

	{
		list<int> list1;

		int& refF = list1.push_front();
		int& refB = list1.push_back();

		EATEST_VERIFY(list1.size() == 2);
		EATEST_VERIFY(list1.validate());
		EATEST_VERIFY(&refF == &list1.front());
		EATEST_VERIFY(&refB == &list1.back());

		void* pInt = list1.push_back_uninitialized();
		new(pInt) int(17);
		EATEST_VERIFY(list1.back() == 17);

		pInt = list1.push_front_uninitialized();
		new(pInt) int(18);
		EATEST_VERIFY(list1.front() == 18);
	}

	{
		const int A[] = { 701, 702, 703, 704, 705 };
		const size_t N = sizeof(A) / sizeof(A[0]);
		size_t count;
		list<int>::iterator i;

		list<int> list0701(A, A + N);
		EATEST_VERIFY(list0701.size() == N);

		list<int> list0702;
		EATEST_VERIFY(list0702.size() == 0);

		list0702 = list0701;
		EATEST_VERIFY(list0702.size() == N);
		for(i = list0702.begin(), count = 0; i != list0702.end(); ++i, ++count)
			EATEST_VERIFY(*i == A[count]);
		EATEST_VERIFY(count == N);
		EATEST_VERIFY(list0702 == list0701);

		EATEST_VERIFY(list0701.validate());
		EATEST_VERIFY(list0702.validate());
	}

	{
		const size_t kListSize = 5;
		const int kValue = 7;
		size_t count;
		list<int>::const_iterator i;

		// nontrivial value_type
		list< A<B> > list0201(kListSize);

		// default value
		list<int> list0202(kListSize);
		for(i = list0202.begin(), count = 0; i != list0202.end(); ++i, ++count)
			EATEST_VERIFY(*i == 0);
		EATEST_VERIFY(count == kListSize);
		EATEST_VERIFY(list0202.size() == kListSize);

		// explicit value
		list<int> list0203(kListSize, kValue);
		for(i = list0203.begin(), count = 0; i != list0203.end(); ++i, ++count)
			EATEST_VERIFY(*i == kValue);
		EATEST_VERIFY(count == kListSize);
		EATEST_VERIFY(list0203.size() == kListSize);

		EATEST_VERIFY(list0201.validate());
		EATEST_VERIFY(list0202.validate());
		EATEST_VERIFY(list0203.validate());
	}

	{
		const int A[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
		const int B[] = { 101, 102, 103, 104, 105 };
		const size_t N = sizeof(A) / sizeof(A[0]);
		const size_t M = sizeof(B) / sizeof(B[0]);
		size_t count;
		list<int>::const_iterator i;

		list<int> list0501;

		// make it bigger
		list0501.assign(A, A + N);
		for(i = list0501.begin(), count = 0; i != list0501.end(); ++i, ++count)
			EATEST_VERIFY(*i == A[count]);
		EATEST_VERIFY(count == N);
		EATEST_VERIFY(list0501.size() == N);

		// make it smaller
		list0501.assign(B, B + M);
		for(i = list0501.begin(), count = 0; i != list0501.end(); ++i, ++count)
			EATEST_VERIFY(*i == B[count]);
		EATEST_VERIFY(count == M);
		EATEST_VERIFY(list0501.size() == M);

		EATEST_VERIFY(list0501.validate());
	}

	{
		const int A[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
		const size_t N = sizeof(A) / sizeof(A[0]);
		int count;

		list<int>::reverse_iterator i;
		list<int> list0401(A, A + N);
		list<int> list0402(list0401);

		for(i = list0401.rbegin(), count = N - 1; i != list0401.rend(); ++i, --count)
			EATEST_VERIFY(*i == A[count]);
		EATEST_VERIFY(count == -1);
		EATEST_VERIFY(list0401.size() == N);

		EATEST_VERIFY(list0401.validate());
		EATEST_VERIFY(list0402.validate());
	}

	{
		const size_t BIG_LIST_SIZE = 11;
		const int    BIG_INIT_VALUE = 7;
		const size_t SMALL_LIST_SIZE = 5;
		const int    SMALL_INIT_VALUE = 17;

		size_t count;
		list<int>::const_iterator i;
		list<int> list0601;
		EATEST_VERIFY(list0601.size() == 0);

		// make it bigger
		list0601.assign(BIG_LIST_SIZE, BIG_INIT_VALUE);
		for(i = list0601.begin(), count = 0; i != list0601.end(); ++i, ++count)
			EATEST_VERIFY(*i == BIG_INIT_VALUE);
		EATEST_VERIFY(count == BIG_LIST_SIZE);
		EATEST_VERIFY(list0601.size() == BIG_LIST_SIZE);

		// make it shrink
		list0601.assign(SMALL_LIST_SIZE, SMALL_INIT_VALUE);
		for(i = list0601.begin(), count = 0; i != list0601.end(); ++i, ++count)
			EATEST_VERIFY(*i == SMALL_INIT_VALUE);
		EATEST_VERIFY(count == SMALL_LIST_SIZE);
		EATEST_VERIFY(list0601.size() == SMALL_LIST_SIZE);

		EATEST_VERIFY(list0601.validate());
	}

	{
		const size_t LIST_SIZE = 5;
		const int    INIT_VALUE = 7;

		size_t count = 0;
		list<C> list0604;
		EATEST_VERIFY(list0604.size() == 0);
		  
		list0604.assign(LIST_SIZE, INIT_VALUE);
		list<C>::iterator i = list0604.begin();
		for(; i != list0604.end(); ++i, ++count)
			EATEST_VERIFY(*i == INIT_VALUE);
		EATEST_VERIFY(count == LIST_SIZE);
		EATEST_VERIFY(list0604.size() == LIST_SIZE);
		EATEST_VERIFY(list0604.validate());
	}

	{   // void push_back_unintialized()
		int64_t ctorCount0 = TestObject::sTOCtorCount;

		list<TestObject> listTO;
		EATEST_VERIFY(TestObject::sTOCtorCount == ctorCount0);

		void* pTO = listTO.push_back_uninitialized();
		EATEST_VERIFY(TestObject::sTOCtorCount == ctorCount0);

		new(pTO) TestObject(17);
		EATEST_VERIFY(TestObject::sTOCtorCount == (ctorCount0 + 1));
		EATEST_VERIFY(listTO.back().mX == 17);
		EATEST_VERIFY(listTO.validate());
	}

	{
		const size_t LIST_SIZE = 5;
		const int    INIT_VALUE = 7;

		size_t count = 0;
		list<C> list0204(LIST_SIZE, INIT_VALUE);
		list<C>::iterator i = list0204.begin();

		for(; i != list0204.end(); ++i, ++count)
			EATEST_VERIFY(*i == INIT_VALUE);
		EATEST_VERIFY(count == LIST_SIZE);
		EATEST_VERIFY(list0204.size() == LIST_SIZE);

		EATEST_VERIFY(list0204.validate());
	}

	{
		const int A[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
		const size_t N = sizeof(A) / sizeof(A[0]);
		size_t count;
		list<int>::const_iterator i;

		// construct from a dissimilar range
		list<int> list0301(A, A + N);
		for(i = list0301.begin(), count = 0; i != list0301.end(); ++i, ++count)
			EATEST_VERIFY(*i == A[count]);
		EATEST_VERIFY(count == N);
		EATEST_VERIFY(list0301.size() == N);

		// construct from a similar range
		list<int> list0302(list0301.begin(), list0301.end());
		for(i = list0302.begin(), count = 0; i != list0302.end(); ++i, ++count)
			EATEST_VERIFY(*i == A[count]);
		EATEST_VERIFY(count == N);
		EATEST_VERIFY(list0302.size() == N);

		EATEST_VERIFY(list0301.validate());
		EATEST_VERIFY(list0302.validate());
	}

	{
		list<TestObject> list0201;
		TestObject::Reset();

		list0201.insert(list0201.begin());   // list should be [0], using the default values for TestObject()
		EATEST_VERIFY(list0201.size() == 1);
		EATEST_VERIFY(TestObject::sTOCount == 1);
		list<TestObject>::iterator i0 = list0201.begin();
		EATEST_VERIFY(i0->mX == 0);
		list0201.clear();
		TestObject::Reset();

		list0201.insert(list0201.begin(), TestObject(1));   // list should be [1]
		EATEST_VERIFY(list0201.size() == 1);
		EATEST_VERIFY(TestObject::sTOCount == 1);

		list0201.insert(list0201.end(), TestObject(2));     // list should be [1 2]
		EATEST_VERIFY(list0201.size() == 2);
		EATEST_VERIFY(TestObject::sTOCount == 2);

		list<TestObject>::iterator       i = list0201.begin();
		list<TestObject>::const_iterator j = i;
		EATEST_VERIFY(i->mX == 1);
		++i;
		EATEST_VERIFY(i->mX == 2);

		list0201.insert(i, TestObject(3));     // list should be [1 3 2]
		EATEST_VERIFY(list0201.size() == 3);
		EATEST_VERIFY(TestObject::sTOCount == 3);

		list<TestObject>::const_iterator k = i;
		EATEST_VERIFY(i->mX == 2);
		--i;
		EATEST_VERIFY(i->mX == 3);
		--i;
		EATEST_VERIFY(i->mX == 1); 

		TestObject::Reset(); // Note that this Reset call will currently result in zeroing some values which will go negative in the next couple lines.
		++i; // will point to '3'
		list0201.erase(i); // should be [1 2]
		EATEST_VERIFY(list0201.size() == 2);
		EATEST_VERIFY(TestObject::sTODtorCount == 1);
		EATEST_VERIFY(k->mX == 2);
		EATEST_VERIFY(j->mX == 1); 

		TestObject::Reset();
		list<TestObject> list0202;
		EATEST_VERIFY(list0202.size() == 0);
		EATEST_VERIFY(TestObject::sTOCount == 0);
		EATEST_VERIFY(TestObject::sTODtorCount == 0);

		// member swap
		list0202.swap(list0201);
		EATEST_VERIFY(list0201.size() == 0);
		EATEST_VERIFY(list0202.size() == 2);
		EATEST_VERIFY(TestObject::sTOCount == 0);
		EATEST_VERIFY(TestObject::sTODtorCount == 0);

		// global swap
		swap(list0201, list0202);
		EATEST_VERIFY(list0201.size() == 2);
		EATEST_VERIFY(list0202.size() == 0);
		EATEST_VERIFY(TestObject::sTOCount == 0);
		EATEST_VERIFY(TestObject::sTODtorCount == 0);

		EATEST_VERIFY(list0201.validate());
		EATEST_VERIFY(list0202.validate());
	}

	{
		// void splice(iterator position, this_type& x, iterator first, iterator last);

		list<int> listA(10, 17);    // Insert 10 entries of value 17.
		list<int> listB(10, 42);    // Insert 10 entries of value 42.

		list<int>::iterator startB = listB.begin();
		list<int>::iterator endB   = startB;
		advance(endB, 5);
		list<int>::iterator afterB = endB;
		advance(afterB, 2);

		listA.splice(listA.begin(), listB, startB, endB);
		EATEST_VERIFY(listA.validate());
		EATEST_VERIFY(listB.validate());

		// Verify that the size of listA and listB are 15 and 5, respectively.
		EATEST_VERIFY(listA.size() == 15);
		EATEST_VERIFY(listB.size() ==  5);

		// Verify that startB was moved from listB to listA.
		EATEST_VERIFY((listA.validate_iterator(startB) & isf_can_dereference) != 0);
		EATEST_VERIFY((listB.validate_iterator(startB) & isf_can_dereference) == 0);

		// Verify that endB was not moved from listB to listA.
		EATEST_VERIFY((listA.validate_iterator(endB) & isf_can_dereference) == 0);
		EATEST_VERIFY((listB.validate_iterator(endB) & isf_can_dereference) != 0);

		// Verify that afterB was not moved from listB to listA.
		EATEST_VERIFY((listA.validate_iterator(afterB) & isf_can_dereference) == 0);
		EATEST_VERIFY((listB.validate_iterator(afterB) & isf_can_dereference) != 0);
	}

	{
		list<int> v(20, 42);

		// Single element erase (middle)
		list<int>::iterator before = v.begin();
		list<int>::iterator at = before;
		advance(at, 3);
		list<int>::iterator after = at;
		at = v.erase(at);
		// EATEST_VERIFY(before.dereferenceable());
		// EATEST_VERIFY(at.dereferenceable());
		// EATEST_VERIFY(after.singular());

		// Single element erase (end)
		before = v.begin();
		at = before;
		after = at;
		++after;
		at = v.erase(at);
		// EATEST_VERIFY(before.singular());
		// EATEST_VERIFY(at.dereferenceable());
		// EATEST_VERIFY(after.dereferenceable());

		// Multiple element erase
		before = v.begin();
		at = before;
		advance(at, 3);
		after = at;
		advance(after, 3);
		v.erase(at, after);
		// EATEST_VERIFY(before.dereferenceable());
		// EATEST_VERIFY(at.singular());

		// clear()
		before = v.begin();
		// EATEST_VERIFY(before.dereferenceable());
		//list<int>::iterator finish = v.end();
		v.clear();
		// EATEST_VERIFY(before.singular());
		// EATEST_VERIFY(!finish.singular() && !finish.dereferenceable());

		EATEST_VERIFY(v.validate());
	}

	{
		list<TestObject> list0301;

		// fill insert at beginning of list / empty list
		TestObject::Reset();
		list0301.insert(list0301.begin(), 3, TestObject(11)); // should be [11 11 11]
		EATEST_VERIFY(list0301.size() == 3);
		EATEST_VERIFY(TestObject::sTOCount == 3);

		// save iterators to verify post-insert validity
		list<TestObject>::iterator b = list0301.begin();          
		list<TestObject>::iterator m = list0301.end();
		--m;          
		list<TestObject>::iterator e = list0301.end();

		// fill insert at end of list
		TestObject::Reset();
		list0301.insert(list0301.end(), 3, TestObject(13)); // should be [11 11 11 13 13 13]
		EATEST_VERIFY(list0301.size() == 6);
		EATEST_VERIFY(TestObject::sTOCount == 3);
		EATEST_VERIFY(b == list0301.begin() && b->mX == 11);
		EATEST_VERIFY(e == list0301.end());
		EATEST_VERIFY(m->mX == 11);

		// fill insert in the middle of list
		TestObject::Reset();
		++m;
		list0301.insert(m, 3, TestObject(12)); // should be [11 11 11 12 12 12 13 13 13]
		EATEST_VERIFY(list0301.size() == 9);
		EATEST_VERIFY(TestObject::sTOCount == 3);
		EATEST_VERIFY(b == list0301.begin() && b->mX == 11);
		EATEST_VERIFY(e == list0301.end());
		EATEST_VERIFY(m->mX == 13);

		// single erase
		TestObject::Reset();
		m = list0301.erase(m); // should be [11 11 11 12 12 12 13 13]
		EATEST_VERIFY(list0301.size() == 8);
		EATEST_VERIFY(TestObject::sTODtorCount == 1);
		EATEST_VERIFY(b == list0301.begin() && b->mX == 11);
		EATEST_VERIFY(e == list0301.end());
		EATEST_VERIFY(m->mX == 13);

		// range erase
		TestObject::Reset();
		m = list0301.erase(list0301.begin(), m); // should be [13 13]
		EATEST_VERIFY(list0301.size() == 2);
		EATEST_VERIFY(TestObject::sTODtorCount == 6);
		EATEST_VERIFY(m->mX == 13);

		// range fill at beginning
		TestObject::Reset();
		const TestObject A[3] = { TestObject(321), TestObject(322), TestObject(333) };
		const int N = sizeof(A) / sizeof(A[0]);
		TestObject::Reset();
		b = list0301.begin();          
		list0301.insert(b, A, A + N); // should be [321 322 333 13 13]
		EATEST_VERIFY(list0301.size() == 5);
		EATEST_VERIFY(TestObject::sTOCount == 3);
		EATEST_VERIFY(m->mX == 13);
		  
		// range fill at end
		TestObject::Reset();
		list0301.insert(e, A, A + N); // should be [321 322 333 13 13 321 322 333]
		EATEST_VERIFY(list0301.size() == 8);
		EATEST_VERIFY(TestObject::sTOCount == 3);
		EATEST_VERIFY(e == list0301.end());
		EATEST_VERIFY(m->mX == 13);
		  
		// range fill in middle
		TestObject::Reset();
		list0301.insert(m, A, A + N); 
		EATEST_VERIFY(list0301.size() == 11);
		EATEST_VERIFY(TestObject::sTOCount == 3);
		EATEST_VERIFY(e == list0301.end());
		EATEST_VERIFY(m->mX == 13);

		TestObject::Reset();
		list0301.clear();
		EATEST_VERIFY(list0301.size() == 0);
		EATEST_VERIFY(TestObject::sTODtorCount == 11);
		EATEST_VERIFY(e == list0301.end());

		EATEST_VERIFY(list0301.validate());
	}

	{
		list<int> v(10, 17);
		list<int>::iterator before = v.begin();
		advance(before, 6);
		list<int>::iterator at = before;
		advance(at, 1);
		list<int>::iterator after = at;
		advance(after, 1);
		//list<int>::iterator finish = v.end();

		// Shrink
		v.resize(7);
		// EATEST_VERIFY(before.dereferenceable());
		// EATEST_VERIFY(at.singular());
		// EATEST_VERIFY(after.singular());
		// EATEST_VERIFY(!finish.singular() && !finish.dereferenceable());

		EATEST_VERIFY(v.validate());
	}

	{
		const int K = 417;
		const int A[] = { 1, 2, 3, 4, 5 };
		const int B[] = { K, K, K, K, K };
		const size_t N = sizeof(A) / sizeof(A[0]);
		const size_t M = sizeof(B) / sizeof(B[0]);

		list<int> list0101(A, A + N);
		list<int> list0102(B, B + M);
		list<int>::iterator p = list0101.begin();

		EATEST_VERIFY(list0101.size() == N);
		EATEST_VERIFY(list0102.size() == M);

		++p;
		list0101.splice(p, list0102); // [1 K K K K K 2 3 4 5]
		EATEST_VERIFY(list0101.size() == N + M);
		EATEST_VERIFY(list0102.size() == 0);

		// remove range from middle
		list0101.remove(K);
		EATEST_VERIFY(list0101.size() == N);

		// remove first element
		list0101.remove(1);
		EATEST_VERIFY(list0101.size() == N - 1);

		// remove last element
		list0101.remove(5);
		EATEST_VERIFY(list0101.size() == N - 2);

		// reverse
		list0101.reverse();
		p = list0101.begin();
		EATEST_VERIFY(*p == 4); ++p;
		EATEST_VERIFY(*p == 3); ++p;
		EATEST_VERIFY(*p == 2); ++p;
		EATEST_VERIFY(p == list0101.end());

		EATEST_VERIFY(list0101.validate());
	}

	{
		list<int> v1;
		list<int> v2;

		v1.push_front(17);

		//list<int>::iterator start = v1.begin();
		list<int>::iterator finish; // = v1.end();
		// EATEST_VERIFY(start.dereferenceable());
		// EATEST_VERIFY(!finish.dereferenceable() && !finish.singular());

		v1 = v2;
		// EATEST_VERIFY(start.singular());
		// EATEST_VERIFY(!finish.dereferenceable() && !finish.singular());

		finish = v1.end();
		v1.assign(v2.begin(), v2.end());
		// EATEST_VERIFY(!finish.dereferenceable() && !finish.singular());

		finish = v1.end();
		v1.assign(17, 42);
		// EATEST_VERIFY(!finish.dereferenceable() && !finish.singular());

		EATEST_VERIFY(v1.validate());
		EATEST_VERIFY(v2.validate());
	}

	{
		const int A[] = { 1, 2, 3, 4, 5 };
		const int B[] = { 2, 1, 3, 4, 5 };
		const int C[] = { 1, 3, 4, 5, 2 };
		const int N = sizeof(A) / sizeof(A[0]);
		list<int> list0201(A, A + N);
		list<int> list0202(A, A + N);
		list<int> list0203(B, B + N);
		list<int> list0204(C, C + N);
		list<int>::iterator i = list0201.begin();

		// result should be unchanged
		list0201.splice(list0201.begin(), list0201, i);
		EATEST_VERIFY(list0201 == list0202);

		// result should be [2 1 3 4 5]
		++i;
		list0201.splice(list0201.begin(), list0201, i);
		EATEST_VERIFY(list0201 != list0202);
		EATEST_VERIFY(list0201 == list0203);

		// result should be [1 3 4 5 2]
		list0201.splice(list0201.end(), list0201, i);
		EATEST_VERIFY(list0201 == list0204);

		EATEST_VERIFY(list0201.validate());
		EATEST_VERIFY(list0204.validate());
	}

	{
		const int A[] = {103, 203, 603, 303, 403, 503};
		const int B[] = {417, 417, 417, 417, 417};
		const int E[] = {103, 417, 417, 203, 603, 303, 403, 503};
		const int F[] = {103, 203, 303, 403, 417, 417, 503, 603};
		const int C[] = {103, 203, 303, 403, 417, 417, 417, 417, 417, 503, 603};
		const int D[] = {103, 203, 303, 403, 417, 503, 603};
		const int N = sizeof(A) / sizeof(A[0]);
		const int M = sizeof(B) / sizeof(B[0]);
		const int P = sizeof(C) / sizeof(C[0]);
		const int Q = sizeof(D) / sizeof(D[0]);
		const int R = sizeof(E) / sizeof(E[0]);

		list<int> list0301(A, A + N);
		list<int> list0302(B, B + M);
		list<int> list0303(C, C + P);
		list<int> list0304(D, D + Q);
		list<int> list0305(E, E + R);
		list<int> list0306(F, F + R);
		list<int>::iterator p = list0301.begin();
		list<int>::iterator q = list0302.begin();

		++p; ++q; ++q;
		list0301.splice(p, list0302, list0302.begin(), q);
		EATEST_VERIFY(list0301 == list0305);
		EATEST_VERIFY(list0301.size() == N + 2);
		EATEST_VERIFY(list0302.size() == M - 2);

		//list0301.sort();
		// EATEST_VERIFY(list0301 == list0306);

		//list0301.merge(list0302);
		// EATEST_VERIFY(list0301.size() == N + M);
		// EATEST_VERIFY(list0302.size() == 0);
		// EATEST_VERIFY(list0301 == list0303);

		//list0301.unique();
		// EATEST_VERIFY(list0301 == list0304);

		EATEST_VERIFY(list0301.validate());
		EATEST_VERIFY(list0302.validate());
		EATEST_VERIFY(list0303.validate());
		EATEST_VERIFY(list0304.validate());
		EATEST_VERIFY(list0305.validate());
		EATEST_VERIFY(list0306.validate());
	}

	{
		// void sort();
		// void merge(this_type& x);

		const int A[] = {1, 2, 3, 4, 5, 6};
		const int B[] = {12, 15, 13, 14, 11};
		const int C[] = {11, 12, 13, 14, 15};
		const int D[] = {1, 11, 2, 12, 3, 13, 4, 14, 5, 15, 6};
		const int N = sizeof(A) / sizeof(A[0]);
		const int M = sizeof(B) / sizeof(B[0]);
		const int Q = sizeof(D) / sizeof(D[0]);

		list<int> list0401(A, A + N);
		list<int> list0402(B, B + M);
		list<int> list0403(C, C + M);
		list<int> list0404(D, D + Q);
		list<int> list0405(A, A + N);

		list0402.sort(eastl::less<int>());
		EATEST_VERIFY(list0402 == list0403);

		list0401.merge(list0402, eastl::less<int>());
		list0404.sort();
		EATEST_VERIFY(list0401 == list0404);

		EATEST_VERIFY(list0401.validate());
		EATEST_VERIFY(list0402.validate());
		EATEST_VERIFY(list0403.validate());
		EATEST_VERIFY(list0404.validate());
		EATEST_VERIFY(list0405.validate());
	}


	{
		// void list<T, Allocator>::remove_if(Predicate predicate)

		const int kSize = 8;
		const int A[kSize] = { 1, 2, 3, 4, 4, 5, 9, 9 };

		list<int> listA(A, A + kSize);

		listA.remove_if(eastl::bind2nd(eastl::equal_to<int>(), 4));
		EATEST_VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "list::remove_if", 1, 2, 3, 5, 9, 9, -1));

		listA.remove_if(eastl::bind2nd(eastl::equal_to<int>(), 99));
		EATEST_VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "list::remove_if", 1, 2, 3, 5, 9, 9, -1));
	}


	{
		// void sort()
		// void sort(Compare compare)

		const int kSize = 10;
		const int A[kSize] = { 1, 9, 2, 3, 5, 7, 4, 6, 8, 0 };

		list<int> listEmpty;
		EATEST_VERIFY(VerifySequence(listEmpty.begin(), listEmpty.end(), int(), "list::sort", -1));
		listEmpty.sort();
		EATEST_VERIFY(VerifySequence(listEmpty.begin(), listEmpty.end(), int(), "list::sort", -1));

		list<int> list1(A, A + 1);
		EATEST_VERIFY(VerifySequence(list1.begin(), list1.end(), int(), "list::sort", 1, -1));
		list1.sort();
		EATEST_VERIFY(VerifySequence(list1.begin(), list1.end(), int(), "list::sort", 1, -1));

		list<int> list4(A, A + 4);
		EATEST_VERIFY(VerifySequence(list4.begin(), list4.end(), int(), "list::sort", 1, 9, 2, 3, -1));
		list4.sort();
		EATEST_VERIFY(VerifySequence(list4.begin(), list4.end(), int(), "list::sort", 1, 2, 3, 9, -1));

		list<int> listA(A, A + kSize);
		EATEST_VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "list::sort", 1, 9, 2, 3, 5, 7, 4, 6, 8, 0, -1));
		listA.sort();
		EATEST_VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "list::sort", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1));

		listA.assign(A, A + kSize);
		EATEST_VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "list::sort", 1, 9, 2, 3, 5, 7, 4, 6, 8, 0, -1));
		listA.sort(eastl::less<int>());
		EATEST_VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "list::sort", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1));


		// Test many randomized sequences
		EA::StdC::LimitStopwatch timer(EA::StdC::Stopwatch::kUnitsMilliseconds, 3000, true);
		EA::UnitTest::Rand       rng(EA::UnitTest::GetRandSeed());
		list<uint32_t>           intList(200, 0);
		EA::StdC::Stopwatch      stopwatch(EA::StdC::Stopwatch::kUnitsCPUCycles, false);

		for(int i = 0; ((i < 100) || !timer.IsTimeUp()) && (nErrorCount == 0); i++) // Run the tests at least 100 times, but after at least that many tests are done, continue extra tests until the time is up.
		{
			// list::sort can sort only an entire list and not a subset of it, so we make a list for 
			// sorting which is spliced from the main intList. Doing this is faster than creating a
			// new list each time through this loop.
			list<uint32_t> intListToSort;

			uint32_t sortSize = rng.RandLimit(static_cast<uint32_t>(intList.size()));
			intListToSort.splice(intListToSort.begin(), intList, intList.begin(), eastl::next(intList.begin(), sortSize));

			for(list<uint32_t>::iterator it = intListToSort.begin(); it != intListToSort.end(); ++it)
				*it = rng.RandLimit(sortSize);

			stopwatch.Start();
			intListToSort.sort(eastl::less<uint32_t>());
			stopwatch.Stop();

			EATEST_VERIFY(intListToSort.validate() && eastl::is_sorted(intListToSort.begin(), intListToSort.end()) && (intListToSort.size() == sortSize));

			intList.splice(intList.begin(), intListToSort); // put the elements back into intList.
		}

		EA::UnitTest::ReportVerbosity(2, "list sort time: %I64u ticks.\n", stopwatch.GetElapsedTime());
	}


	{
		// void merge(this_type& x);
		// void merge(this_type& x, Compare compare);

		const int kSize = 8;
		const int A[kSize] = { 1, 2, 3, 4, 4, 5, 9, 9 };
		const int B[kSize] = { 1, 2, 3, 4, 4, 5, 9, 9 };

		list<int> listA(A, A + kSize);
		list<int> listB(B, B + kSize);

		listA.merge(listB);
		EATEST_VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "list::merge", 1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 5, 5, 9, 9, 9, 9, -1));
		EATEST_VERIFY(VerifySequence(listB.begin(), listB.end(), int(), "list::merge", -1));
	}


	{
		// void unique();
		// void unique(BinaryPredicate);

		const int kSize = 8;
		const int A[kSize] = { 1, 2, 3, 4, 4, 5, 9, 9 };
		const int B[kSize] = { 1, 2, 3, 4, 4, 5, 9, 9 };

		list<int> listA(A, A + kSize);
		listA.unique();
		EATEST_VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "list::unique", 1, 2, 3, 4, 5, 9, -1));

		list<int> listB(B, B + kSize);
		listB.unique(eastl::equal_to<int>());
		EATEST_VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "list::unique", 1, 2, 3, 4, 5, 9, -1));
	}


	{
		// list(this_type&& x);
		// list(this_type&&, const allocator_type&);
		// this_type& operator=(this_type&& x);

		#if EASTL_MOVE_SEMANTICS_ENABLED
			list<TestObject> list3TO33(3, TestObject(33));
			list<TestObject> toListA(eastl::move(list3TO33));
			EATEST_VERIFY((toListA.size() == 3) && (toListA.front().mX == 33) && (list3TO33.size() == 0));

			// The following is not as strong a test of this ctor as it could be. A stronger test would be to use IntanceAllocator with different instances.
			list<TestObject, MallocAllocator> list4TO44(4, TestObject(44));
			list<TestObject, MallocAllocator> toListB(eastl::move(list4TO44), MallocAllocator());
			EATEST_VERIFY((toListB.size() == 4) && (toListB.front().mX == 44) && (list4TO44.size() == 0));

			list<TestObject, MallocAllocator> list5TO55(5, TestObject(55));
			toListB = eastl::move(list5TO55);
			EATEST_VERIFY((toListB.size() == 5) && (toListB.front().mX == 55) && (list5TO55.size() == 0));
		#endif
	}


	{
		#if EASTL_MOVE_SEMANTICS_ENABLED && EASTL_VARIADIC_TEMPLATES_ENABLED
			// template <class... Args>
			// void emplace_front(Args&&... args);

			// template <class... Args>
			// void emplace_back(Args&&... args);

			// template <class... Args>
			// iterator emplace(const_iterator position, Args&&... args);
		#else
			#if EASTL_MOVE_SEMANTICS_ENABLED
				// void emplace_front(value_type&& value);
				// void emplace_back(value_type&& value);
				// iterator emplace(const_iterator position, value_type&& value);
			#endif
			// void emplace_front(const value_type& value);
			// void emplace_back(const value_type& value);
			// iterator emplace(const_iterator position, const value_type& value);
		#endif

		#if EASTL_MOVE_SEMANTICS_ENABLED && EASTL_VARIADIC_TEMPLATES_ENABLED
			TestObject::Reset();

			list<TestObject> toListA;

			toListA.emplace_front(1, 2, 3); // This uses the TestObject(int x0, int x1, int x2, bool bThrowOnCopy) constructor.
			EATEST_VERIFY((toListA.size() == 1) && (toListA.front().mX == (1+2+3)) && (TestObject::sTOCtorCount == 1));

			toListA.emplace_back(2, 3, 4);
			EATEST_VERIFY((toListA.size() == 2) && (toListA.back().mX == (2+3+4)) && (TestObject::sTOCtorCount == 2));

			auto it = toListA.emplace(toListA.begin(), 3, 4, 5);
			EATEST_VERIFY((toListA.size() == 3) && (toListA.front().mX == (3+4+5)) && (it->mX == (3+4+5)) && (TestObject::sTOCtorCount == 3));
		#else
			#if EASTL_MOVE_SEMANTICS_ENABLED
				TestObject::Reset();

				// We have a potential problem here in that the compiler is not required to use move construction below.
				// It is allowed to use standard copy construction if it wants. We could force it with eastl::move() usage.
				list<TestObject> toListA;

				toListA.emplace_front(TestObject(1, 2, 3));
				EATEST_VERIFY((toListA.size() == 1) && (toListA.front().mX == (1+2+3)) && (TestObject::sTOMoveCtorCount == 1));

				toListA.emplace_back(TestObject(2, 3, 4));
				EATEST_VERIFY((toListA.size() == 2) && (toListA.back().mX == (2+3+4)) && (TestObject::sTOMoveCtorCount == 2));

				toListA.emplace(toListA.begin(), TestObject(3, 4, 5));
				EATEST_VERIFY((toListA.size() == 3) && (toListA.front().mX == (3+4+5)) && (TestObject::sTOMoveCtorCount == 3));
			#endif
			
			TestObject::Reset();

			list<TestObject> toListB;
			TestObject to123(1, 2, 3);
			TestObject to234(2, 3, 4);
			TestObject to345(3, 4, 5);

			toListB.emplace_front(to123);   // This should use the const value_type& version and not the value_type&& version of emplace_front.
			EATEST_VERIFY((toListB.size() == 1) && (toListB.front().mX == (1+2+3)) && (TestObject::sTOCopyCtorCount == 1));

			toListB.emplace_back(to234);
			EATEST_VERIFY((toListB.size() == 2) && (toListB.back().mX == (2+3+4)) && (TestObject::sTOCopyCtorCount == 2));

			toListB.emplace(toListB.begin(), to345);
			EATEST_VERIFY((toListB.size() == 3) && (toListB.front().mX == (3+4+5)) && (TestObject::sTOCopyCtorCount == 3));

			EATEST_VERIFY(to123.mX == (1+2+3));  // Verify that the object was copied and not moved. If it was moved then mX would be 0 and not 1+2+3.
			EATEST_VERIFY(to234.mX == (2+3+4));
			EATEST_VERIFY(to345.mX == (3+4+5));
		#endif


		#if EASTL_MOVE_SEMANTICS_ENABLED
			// This test is similar to the emplace EASTL_MOVE_SEMANTICS_ENABLED pathway above. 
			TestObject::Reset();

			// void push_front(T&& x);
			// void push_back(T&& x);
			// iterator insert(const_iterator position, T&& x);

			list<TestObject> toListC;

			toListC.push_front(TestObject(1, 2, 3));
			EATEST_VERIFY((toListC.size() == 1) && (toListC.front().mX == (1+2+3)) && (TestObject::sTOMoveCtorCount == 1));

			toListC.push_back(TestObject(2, 3, 4));
			EATEST_VERIFY((toListC.size() == 2) && (toListC.back().mX == (2+3+4)) && (TestObject::sTOMoveCtorCount == 2));

			toListC.insert(toListC.begin(), TestObject(3, 4, 5));
			EATEST_VERIFY((toListC.size() == 3) && (toListC.front().mX == (3+4+5)) && (TestObject::sTOMoveCtorCount == 3));
		#endif
	}


	{
		// list(std::initializer_list<value_type> ilist, const allocator_type& allocator = EASTL_LIST_DEFAULT_ALLOCATOR);
		// this_type& operator=(std::initializer_list<value_type> ilist);
		// void assign(std::initializer_list<value_type> ilist);
		// iterator insert(iterator position, std::initializer_list<value_type> ilist);
		#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
			list<int> intList = { 0, 1, 2 };
			EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "list std::initializer_list", 0, 1, 2, -1));

			intList = { 13, 14, 15 };
			EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "list std::initializer_list", 13, 14, 15, -1));

			intList.assign({ 16, 17, 18 });
			EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "list std::initializer_list", 16, 17, 18, -1));

			intList.insert(intList.begin(), { 14, 15 });
			EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "list std::initializer_list", 14, 15, 16, 17, 18, -1));
		#endif
	}


	{
		// Regression of list::operator= for the case of EASTL_ALLOCATOR_COPY_ENABLED=1
		// For this test we need to use InstanceAllocator to create two containers of the same
		// type but with different and unequal allocator instances. The bug was that when 
		// EASTL_ALLOCATOR_COPY_ENABLED was enabled operator=(this_type& x) assigned x.mAllocator
		// to this and then proceeded to assign member elements from x to this. That's invalid 
		// because the existing elements of this were allocated by a different allocator and 
		// will be freed in the future with the allocator copied from x. 
		// The test below should work for the case of EASTL_ALLOCATOR_COPY_ENABLED == 0 or 1.
		InstanceAllocator::reset_all();

		InstanceAllocator ia0((uint8_t)0);
		InstanceAllocator ia1((uint8_t)1);

		eastl::list<int, InstanceAllocator> list0((eastl_size_t)1, (int)0, ia0);
		eastl::list<int, InstanceAllocator> list1((eastl_size_t)1, (int)1, ia1);

		EATEST_VERIFY((list0.front() == 0) && (list1.front() == 1));
		#if EASTL_ALLOCATOR_COPY_ENABLED
			EATEST_VERIFY(list0.get_allocator() != list1.get_allocator());
		#endif
		list0 = list1;
		EATEST_VERIFY((list0.front() == 1) && (list1.front() == 1));
		EATEST_VERIFY(InstanceAllocator::mMismatchCount == 0);
		EATEST_VERIFY(list0.validate());
		EATEST_VERIFY(list1.validate());
		#if EASTL_ALLOCATOR_COPY_ENABLED
			EATEST_VERIFY(list0.get_allocator() == list1.get_allocator());
		#endif
	}


	/* This is disabled because it cannot be compiled under conforming C++ compilers (e.g. clang).
	{ // Regression of user error report for the case of list<const type>.
		eastl::list<int> ctorValues;

		for(int v = 0; v < 10; v++)
			ctorValues.push_back(v);

		eastl::list<const ConstType> testStruct(ctorValues.begin(), ctorValues.end());
		eastl::list<const int>       testInt(ctorValues.begin(), ctorValues.end());
	}
	*/
	
	

	// We can't do this, due to how Reset is used above:
	//   EATEST_VERIFY(TestObject::IsClear());
	EATEST_VERIFY(TestObject::sMagicErrorCount == 0);
	TestObject::Reset();

	return nErrorCount;
}









