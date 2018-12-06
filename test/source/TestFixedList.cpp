/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/fixed_list.h>


using namespace eastl;


// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::fixed_list<int, 1, true,  EASTLAllocatorType>;
template class eastl::fixed_list<int, 1, false, EASTLAllocatorType>;


/*
// This does not compile, since the fixed_list allocator is templated on sizeof(T), 
// not just T. Thus, the full type is required at the time of instantiation, but it
// is not available.
// See EATech Core JIRA issue ETCR-1608 for more information.
struct StructWithContainerOfStructs
{
	eastl::fixed_list<StructWithContainerOfStructs,4> children;
};
*/


namespace FixedListTest
{
	struct Item
	{
	   char mName[5];
	};
}


EA_DISABLE_VC_WARNING(6262)
int TestFixedList()
{
	int nErrorCount = 0;

	{
		// Test version *without* pool overflow.
		typedef fixed_list<int, 64, false> FixedListInt64False;

		FixedListInt64False listInt64;
		VERIFY(listInt64.empty());
		VERIFY(listInt64.size() == 0);
		VERIFY(listInt64.max_size() == 64);

		listInt64.push_back(1);
		VERIFY(!listInt64.empty());
		VERIFY(listInt64.size() == 1);

		listInt64.resize(3, 2);
		VERIFY(!listInt64.empty());
		VERIFY(listInt64.size() == 3);

		FixedListInt64False::iterator i = listInt64.begin();
		VERIFY(*i == 1); ++i;
		VERIFY(*i == 2); ++i;
		VERIFY(*i == 2); ++i;
		VERIFY(i == listInt64.end());

		listInt64.resize(0);
		VERIFY(listInt64.empty());
		VERIFY(listInt64.size() == 0);

		while(listInt64.size() < 64)
			listInt64.push_back(0);

		// Verify that we allocated enough space for exactly N items. 
		// It's possible that due to alignments, there might be room for N + 1.
		FixedListInt64False::allocator_type& allocator = listInt64.get_allocator();
		void* pResult = allocator.allocate(sizeof(FixedListInt64False::node_type));
		if(pResult)
		{
			pResult = allocator.allocate(sizeof(FixedListInt64False::node_type));
			VERIFY(pResult == NULL); 
		}
	}


	{
		// Test version *with* pool overflow.
		typedef fixed_list<int, 64, true> FixedListInt64True;

		FixedListInt64True listInt64;
		VERIFY(listInt64.empty());
		VERIFY(listInt64.size() == 0);

		listInt64.push_back(1);
		VERIFY(!listInt64.empty());
		VERIFY(listInt64.size() == 1);

		listInt64.resize(3, 2);
		VERIFY(!listInt64.empty());
		VERIFY(listInt64.size() == 3);

		FixedListInt64True::iterator i = listInt64.begin();
		VERIFY(*i == 1); ++i;
		VERIFY(*i == 2); ++i;
		VERIFY(*i == 2); ++i;
		VERIFY(i == listInt64.end());

		listInt64.resize(0);
		VERIFY(listInt64.empty());
		VERIFY(listInt64.size() == 0);

		while(listInt64.size() < 64 + 16)
			listInt64.push_back(0);

		FixedListInt64True::allocator_type& allocator = listInt64.get_allocator();
		void* pResult = allocator.allocate(sizeof(FixedListInt64True::node_type));
		VERIFY(pResult != NULL);
		allocator.deallocate(pResult, sizeof(FixedListInt64True::node_type));

		// get_overflow_allocator / set_overflow_allocator
		// This is a weak test which should be improved.
		EASTLAllocatorType a = listInt64.get_allocator().get_overflow_allocator();
		listInt64.get_allocator().set_overflow_allocator(a);
	}


	{
		// Test version *with* pool overflow with a custom overlow allocator specification.
		typedef fixed_list<int, 64, true, MallocAllocator> FixedListInt64TrueMalloc;

		FixedListInt64TrueMalloc listInt64;
		VERIFY(listInt64.empty());
		VERIFY(listInt64.size() == 0);

		listInt64.push_back(1);
		VERIFY(!listInt64.empty());
		VERIFY(listInt64.size() == 1);

		listInt64.resize(3, 2);
		VERIFY(!listInt64.empty());
		VERIFY(listInt64.size() == 3);

		FixedListInt64TrueMalloc::iterator i = listInt64.begin();
		VERIFY(*i == 1); ++i;
		VERIFY(*i == 2); ++i;
		VERIFY(*i == 2); ++i;
		VERIFY(i == listInt64.end());

		listInt64.resize(0);
		VERIFY(listInt64.empty());
		VERIFY(listInt64.size() == 0);

		while(listInt64.size() < 64 + 16)
			listInt64.push_back(0);

		FixedListInt64TrueMalloc::allocator_type& allocator = listInt64.get_allocator();
		void* pResult = allocator.allocate(sizeof(FixedListInt64TrueMalloc::node_type));
		VERIFY(pResult != NULL);
		allocator.deallocate(pResult, sizeof(FixedListInt64TrueMalloc::node_type));
	}

	{
		// Test fixed list with overflow and alignment requirements.
		typedef fixed_list<Align64, 1, true, CustomAllocator> FixedListWithAlignment;

		FixedListWithAlignment fl;

		Align64 a;

		fl.push_back(a);
		fl.push_back(a);
		fl.push_back(a);
		fl.push_back(a);
		fl.push_back(a);
		for (FixedListWithAlignment::const_iterator it = fl.begin(); it != fl.end(); ++it)
		{
			const Align64* ptr = &(*it);
			EATEST_VERIFY((uint64_t)ptr % EASTL_ALIGN_OF(Align64) == 0);
		}
	}

	{
		// swap

		fixed_list<int, 64>* pListInt64A = new fixed_list<int, 64>;
		fixed_list<int, 64>* pListInt64B = new fixed_list<int, 64>;

		pListInt64A->push_back(0);
		pListInt64B->push_back(0);

		swap(*pListInt64A, *pListInt64B);

		delete pListInt64A;
		delete pListInt64B;
	}


	{
		// operator=

		fixed_list<int, 64>* pListInt64A = new fixed_list<int, 64>;
		fixed_list<int, 64>* pListInt64B = new fixed_list<int, 64>;

		pListInt64A->push_back(0);
		pListInt64B->push_back(0);

		*pListInt64A = *pListInt64B;

		delete pListInt64A;
		delete pListInt64B;
	}


	{
		// bool empty() const
		// bool has_overflowed() const
		// size_type size() const;
		// size_type max_size() const

		// Test a list that has overflow disabled.
		fixed_list<int, 5, false> listInt5;

		VERIFY(listInt5.max_size() == 5);
		VERIFY(listInt5.size() == 0);
		VERIFY(listInt5.empty());
		VERIFY(!listInt5.has_overflowed());

		listInt5.push_back(37);
		listInt5.push_back(37);
		listInt5.push_back(37);

		VERIFY(listInt5.size() == 3);
		VERIFY(!listInt5.empty());
		VERIFY(!listInt5.has_overflowed());

		listInt5.push_back(37);
		listInt5.push_back(37);

		VERIFY(listInt5.size() == 5);
		VERIFY(!listInt5.empty());
		VERIFY(!listInt5.has_overflowed());

		listInt5.pop_back();

		VERIFY(listInt5.size() == 4);
		VERIFY(!listInt5.empty());
		VERIFY(!listInt5.has_overflowed());
	}


	{
		// bool empty() const
		// bool has_overflowed() const
		// size_type size() const;
		// size_type max_size() const

		// Test a list that has overflow enabled.
		fixed_list<int, 5, true> listInt5;

		VERIFY(listInt5.max_size() == 5);
		VERIFY(listInt5.size() == 0);
		VERIFY(listInt5.empty());
		VERIFY(!listInt5.has_overflowed());

		listInt5.push_back(37);
		listInt5.push_back(37);
		listInt5.push_back(37);

		VERIFY(listInt5.size() == 3);
		VERIFY(!listInt5.empty());
		VERIFY(!listInt5.has_overflowed());

		listInt5.push_back(37);
		listInt5.push_back(37);

		VERIFY(listInt5.size() == 5);
		VERIFY(!listInt5.empty());
		VERIFY(!listInt5.has_overflowed());

		listInt5.push_back(37);

		VERIFY(listInt5.size() == 6);
		VERIFY(!listInt5.empty());
		VERIFY(listInt5.has_overflowed());

		listInt5.pop_back();

		VERIFY(listInt5.size() == 5);
		VERIFY(!listInt5.empty());
	  //VERIFY(listInt5.has_overflowed());  Disabled because currently has_overflowed can't detect this situation in non-debug builds.
	}

	{
		//template <typename Compare>
		//void merge(this_type& x, Compare compare);
		//void unique();
		//template <typename BinaryPredicate>
		//void unique(BinaryPredicate);
		//void sort();
		//template<typename Compare>
		//void sort(Compare compare);

		const int A[] = {1, 2, 3, 4, 5, 6};
		const int B[] = {12, 15, 13, 14, 11};
		const int C[] = {11, 12, 13, 14, 15};
		const int D[] = {1, 11, 2, 12, 3, 13, 4, 14, 5, 15, 6};
		const int N = sizeof(A) / sizeof(A[0]);
		const int M = sizeof(B) / sizeof(B[0]);
		const int Q = sizeof(D) / sizeof(D[0]);

		fixed_list<int, 32, true> list0401(A, A + N);
		fixed_list<int, 32, true> list0402(B, B + M);
		fixed_list<int, 32, true> list0403(C, C + M);
		fixed_list<int, 32, true> list0404(D, D + Q);
		fixed_list<int, 32, true> list0405(A, A + N);

		list0402.sort(eastl::less<int>());
		VERIFY(list0402 == list0403);

		list0401.merge(list0402, eastl::less<int>());
		list0404.sort();

		//merge and isn't yet working for fixed_list.
		//VERIFY(list0401 == list0404);

		VERIFY(list0401.validate());
		VERIFY(list0402.validate());
		VERIFY(list0403.validate());
		VERIFY(list0404.validate());
		VERIFY(list0405.validate());
	}


	{
		// void sort()
		// void sort(Compare compare)

		const int kSize = 10;
		const int A[kSize] = { 1, 9, 2, 3, 5, 7, 4, 6, 8, 0 };

		fixed_list<int, 32, true> listEmpty;
		VERIFY(VerifySequence(listEmpty.begin(), listEmpty.end(), int(), "fixed_list::sort", -1));
		listEmpty.sort();
		VERIFY(VerifySequence(listEmpty.begin(), listEmpty.end(), int(), "fixed_list::sort", -1));

		fixed_list<int, 32, true> list1(A, A + 1);
		VERIFY(VerifySequence(list1.begin(), list1.end(), int(), "fixed_list::sort", 1, -1));
		list1.sort();
		VERIFY(VerifySequence(list1.begin(), list1.end(), int(), "fixed_list::sort", 1, -1));

		fixed_list<int, 32, true> list4(A, A + 4);
		VERIFY(VerifySequence(list4.begin(), list4.end(), int(), "fixed_list::sort", 1, 9, 2, 3, -1));
		list4.sort();
		VERIFY(VerifySequence(list4.begin(), list4.end(), int(), "fixed_list::sort", 1, 2, 3, 9, -1));

		fixed_list<int, 32, true> listA(A, A + kSize);
		VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "fixed_list::sort", 1, 9, 2, 3, 5, 7, 4, 6, 8, 0, -1));
		listA.sort();
		VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "fixed_list::sort", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1));

		listA.assign(A, A + kSize);
		VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "fixed_list::sort", 1, 9, 2, 3, 5, 7, 4, 6, 8, 0, -1));
		listA.sort(eastl::less<int>());
		VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "fixed_list::sort", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1));
	}


	{
		// void merge(this_type& x);
		// void merge(this_type& x, Compare compare);

		const int kSize = 8;
		const int A[kSize] = { 1, 2, 3, 4, 4, 5, 9, 9 };
		const int B[kSize] = { 1, 2, 3, 4, 4, 5, 9, 9 };

		fixed_list<int, 32, true> listA(A, A + kSize);
		fixed_list<int, 32, true> listB(B, B + kSize);

		listA.merge(listB);

		//merge and isn't yet working for fixed_list.
		//VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "fixed_list::merge", 1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 5, 5, 9, 9, 9, 9, -1));
		//VERIFY(VerifySequence(listB.begin(), listB.end(), int(), "fixed_list::merge", -1));
	}


	{
		// void splice(iterator position, this_type& x);
		// void splice(iterator position, this_type& x, iterator i);
		// void splice(iterator position, this_type& x, iterator first, iterator last);

		const int kSize = 8;
		const int A[kSize] = { 1, 2, 3, 4, 4, 5, 9, 9 };
		const int B[kSize] = { 1, 2, 3, 4, 4, 5, 9, 9 };

		fixed_list<int, 32, true> listA(A, A + kSize);
		fixed_list<int, 32, true> listB(B, B + kSize);
		fixed_list<int, 32, true>::iterator it;

		// void splice(iterator position, this_type& x);
		it = listA.begin(); eastl::advance(it, 2);
		listA.splice(it, listB); // move listB into listA at position it.
		VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "fixed_list::splice", 1, 2, 1, 2, 3, 4, 4, 5, 9, 9, 3, 4, 4, 5, 9, 9, -1));
		VERIFY(VerifySequence(listB.begin(), listB.end(), int(), "fixed_list::splice", -1));

		// void splice(iterator position, this_type& x, iterator i);
		it = listA.begin(); eastl::advance(it, 6);
		listB.splice(listB.begin(), listA, it);     // move listA's it (6th element) into the front of listB.
		VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "fixed_list::splice", 1, 2, 1, 2, 3, 4, 5, 9, 9, 3, 4, 4, 5, 9, 9, -1));
		VERIFY(VerifySequence(listB.begin(), listB.end(), int(), "fixed_list::splice", 4, -1));

		// void splice(iterator position, this_type& x, iterator first, iterator last);
		listA.splice(listA.end(), listB, listB.begin(), listB.end()); // move listB into listA at the end of listA.
		VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "fixed_list::splice", 1, 2, 1, 2, 3, 4, 5, 9, 9, 3, 4, 4, 5, 9, 9, 4, -1));
		VERIFY(VerifySequence(listB.begin(), listB.end(), int(), "fixed_list::splice", -1));
	}


	{
		// void unique();
		// void unique(BinaryPredicate);

		const int kSize = 8;
		const int A[kSize] = { 1, 2, 3, 4, 4, 5, 9, 9 };
		const int B[kSize] = { 1, 2, 3, 4, 4, 5, 9, 9 };

		fixed_list<int, 32, true> listA(A, A + kSize);
		listA.unique();
		VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "fixed_list::unique", 1, 2, 3, 4, 5, 9, -1));

		fixed_list<int, 32, true> listB(B, B + kSize);
		listB.unique(eastl::equal_to<int>());
		VERIFY(VerifySequence(listA.begin(), listA.end(), int(), "fixed_list::unique", 1, 2, 3, 4, 5, 9, -1));
	}


	{
		// fixed_list(this_type&& x);
		// fixed_list(this_type&&, const allocator_type&);
		// this_type& operator=(this_type&& x);
		fixed_list<TestObject, 16> list3TO33(3, TestObject(33));
		fixed_list<TestObject, 16> toListA(eastl::move(list3TO33));
		EATEST_VERIFY((toListA.size() == 3) && (toListA.front().mX == 33) /* && (list3TO33.size() == 0) fixed_list usually can't honor the move request. */);

		// The following is not as strong a test of this ctor as it could be. A stronger test would be to use IntanceAllocator with different instances.
		fixed_list<TestObject, 16, true, MallocAllocator> list4TO44(4, TestObject(44));
		fixed_list<TestObject, 16, true, MallocAllocator> toListB(eastl::move(list4TO44), MallocAllocator());
		EATEST_VERIFY((toListB.size() == 4) && (toListB.front().mX == 44) /* && (list4TO44.size() == 0) fixed_list usually can't honor the move request. */);

		fixed_list<TestObject, 16, true, MallocAllocator> list5TO55(5, TestObject(55));
		toListB = eastl::move(list5TO55);
		EATEST_VERIFY((toListB.size() == 5) && (toListB.front().mX == 55) /* && (list5TO55.size() == 0) fixed_list usually can't honor the move request. */);
	}


	{
		// template <class... Args>
		// void emplace_front(Args&&... args);

		// template <class... Args>
		// void emplace_back(Args&&... args);

		// template <class... Args>
		// iterator emplace(const_iterator position, Args&&... args);

		TestObject::Reset();

		fixed_list<TestObject, 16> toListA;

		toListA.emplace_front(1, 2, 3); // This uses the TestObject(int x0, int x1, int x2, bool bThrowOnCopy) constructor.
		EATEST_VERIFY((toListA.size() == 1) && (toListA.front().mX == (1+2+3)) && (TestObject::sTOCtorCount == 1));

		toListA.emplace_back(2, 3, 4);
		EATEST_VERIFY((toListA.size() == 2) && (toListA.back().mX == (2+3+4)) && (TestObject::sTOCtorCount == 2));

		toListA.emplace(toListA.begin(), 3, 4, 5);
		EATEST_VERIFY((toListA.size() == 3) && (toListA.front().mX == (3+4+5)) && (TestObject::sTOCtorCount == 3));


		// This test is similar to the emplace pathway above. 
		TestObject::Reset();

		// void push_front(T&& x);
		// void push_back(T&& x);
		// iterator insert(const_iterator position, T&& x);

		fixed_list<TestObject, 16> toListC;

		toListC.push_front(TestObject(1, 2, 3));
		EATEST_VERIFY((toListC.size() == 1) && (toListC.front().mX == (1+2+3)) && (TestObject::sTOMoveCtorCount == 1));

		toListC.push_back(TestObject(2, 3, 4));
		EATEST_VERIFY((toListC.size() == 2) && (toListC.back().mX == (2+3+4)) && (TestObject::sTOMoveCtorCount == 2));

		toListC.insert(toListC.begin(), TestObject(3, 4, 5));
		EATEST_VERIFY((toListC.size() == 3) && (toListC.front().mX == (3+4+5)) && (TestObject::sTOMoveCtorCount == 3));
	}


	{
		// list(std::initializer_list<value_type> ilist, const allocator_type& allocator = EASTL_LIST_DEFAULT_ALLOCATOR);
		// this_type& operator=(std::initializer_list<value_type> ilist);
		// void assign(std::initializer_list<value_type> ilist);
		// iterator insert(iterator position, std::initializer_list<value_type> ilist);
		list<int> intList = { 0, 1, 2 };
		EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "list std::initializer_list", 0, 1, 2, -1));

		intList = { 13, 14, 15 };
		EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "list std::initializer_list", 13, 14, 15, -1));

		intList.assign({ 16, 17, 18 });
		EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "list std::initializer_list", 16, 17, 18, -1));

		intList.insert(intList.begin(), { 14, 15 });
		EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "list std::initializer_list", 14, 15, 16, 17, 18, -1));
	}


	{   // Regression of user test
		struct Dummy
		{
		   typedef eastl::fixed_list<FixedListTest::Item, 10, false> TCollection;

		   TCollection mCollection1;
		   TCollection mCollection2;
		};

		Dummy d;
		VERIFY(d.mCollection1.size() == d.mCollection2.size());
	}


	{
		// Test construction of a container with an overflow allocator constructor argument.
		MallocAllocator overflowAllocator;
		void* p = overflowAllocator.allocate(1);
		fixed_list<int, 64, true, MallocAllocator> c(overflowAllocator);
		c.resize(65);
		VERIFY(c.get_overflow_allocator().mAllocCount == 2); // 1 for above, and 1 for overflowing from 64 to 65.
		overflowAllocator.deallocate(p, 1);
	}


	// We can't do this, due to how Reset is used above:
	//   EATEST_VERIFY(TestObject::IsClear());
	EATEST_VERIFY(TestObject::sMagicErrorCount == 0);
	TestObject::Reset();


	return nErrorCount;
}
EA_RESTORE_VC_WARNING()










