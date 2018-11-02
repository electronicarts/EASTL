/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/fixed_slist.h>
#include <EABase/eabase.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stdio.h>

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


using namespace eastl;


// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::fixed_slist<int, 1, true,  EASTLAllocatorType>;
template class eastl::fixed_slist<int, 1, false, EASTLAllocatorType>;


/*
// This does not compile, since the fixed_slist allocator is templated on sizeof(T), 
// not just T. Thus, the full type is required at the time of instantiation, but it
// is not available.
// See EATech Core JIRA issue ETCR-1608 for more information.
struct StructWithContainerOfStructs
{
	eastl::fixed_slist<StructWithContainerOfStructs,4> children;
};
*/


int TestFixedSList()
{
	int nErrorCount = 0;

	{
		fixed_slist<int, 64> list0101;
		VERIFY(list0101.empty());
		VERIFY(list0101.size() == 0);
		VERIFY(list0101.max_size() == 64);

		list0101.push_front(1);
		VERIFY(!list0101.empty());
		VERIFY(list0101.size() == 1);

		list0101.resize(3, 2);
		VERIFY(!list0101.empty());
		VERIFY(list0101.size() == 3);

		fixed_slist<int, 64>::iterator i = list0101.begin();
		VERIFY(*i == 1); ++i;
		VERIFY(*i == 2); ++i;
		VERIFY(*i == 2); ++i;
		VERIFY(i == list0101.end());

		list0101.resize(0);
		VERIFY(list0101.empty());
		VERIFY(list0101.size() == 0);
	}

	{
		fixed_slist<int, 64, true, MallocAllocator> list0101;
		VERIFY(list0101.empty());
		VERIFY(list0101.size() == 0);
		VERIFY(list0101.max_size() == 64);

		list0101.push_front(1);
		VERIFY(!list0101.empty());
		VERIFY(list0101.size() == 1);

		list0101.resize(3, 2);
		VERIFY(!list0101.empty());
		VERIFY(list0101.size() == 3);

		fixed_slist<int, 64>::iterator i = list0101.begin();
		VERIFY(*i == 1); ++i;
		VERIFY(*i == 2); ++i;
		VERIFY(*i == 2); ++i;
		VERIFY(i == list0101.end());

		while(list0101.size() < 64 + 16)
			list0101.push_front(0);

		list0101.resize(0);
		VERIFY(list0101.empty());
		VERIFY(list0101.size() == 0);
	}

	{
		// Test fixed slist with overflow and alignment requirements.
		typedef fixed_slist<Align64, 1, true, CustomAllocator> FixedSListWithAlignment;

		FixedSListWithAlignment fsl;

		Align64 a;

		fsl.push_front(a);
		fsl.push_front(a);
		fsl.push_front(a);
		fsl.push_front(a);
		fsl.push_front(a);
		for (FixedSListWithAlignment::const_iterator it = fsl.begin(); it != fsl.end(); ++it)
		{
			const Align64* ptr = &(*it);
			EATEST_VERIFY((uint64_t)ptr % EASTL_ALIGN_OF(Align64) == 0);
		}
	}

	{
		// bool empty() const
		// bool has_overflowed() const
		// size_type size() const;
		// size_type max_size() const

		// Test a list that has overflow disabled.
		fixed_slist<int, 5, false> listInt5;

		VERIFY(listInt5.max_size() == 5);
		VERIFY(listInt5.size() == 0);
		VERIFY(listInt5.empty());
		VERIFY(!listInt5.has_overflowed());

		listInt5.push_front(37);
		listInt5.push_front(37);
		listInt5.push_front(37);

		VERIFY(listInt5.size() == 3);
		VERIFY(!listInt5.empty());
		VERIFY(!listInt5.has_overflowed());

		listInt5.push_front(37);
		listInt5.push_front(37);

		VERIFY(listInt5.size() == 5);
		VERIFY(!listInt5.empty());
		VERIFY(!listInt5.has_overflowed());

		listInt5.pop_front();

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
		fixed_slist<int, 5, true> listInt5;

		VERIFY(listInt5.max_size() == 5);
		VERIFY(listInt5.size() == 0);
		VERIFY(listInt5.empty());
		VERIFY(!listInt5.has_overflowed());

		listInt5.push_front(37);
		listInt5.push_front(37);
		listInt5.push_front(37);

		VERIFY(listInt5.size() == 3);
		VERIFY(!listInt5.empty());
		VERIFY(!listInt5.has_overflowed());

		listInt5.push_front(37);
		listInt5.push_front(37);

		VERIFY(listInt5.size() == 5);
		VERIFY(!listInt5.empty());
		VERIFY(!listInt5.has_overflowed());

		listInt5.push_front(37);

		VERIFY(listInt5.size() == 6);
		VERIFY(!listInt5.empty());
		VERIFY(listInt5.has_overflowed());

		listInt5.pop_front();

		VERIFY(listInt5.size() == 5);
		VERIFY(!listInt5.empty());
	  //VERIFY(listInt5.has_overflowed());  Disabled because currently has_overflowed can't detect this situation in non-debug builds.
	}


	{
		// fixed_slist(this_type&& x);
		// fixed_slist(this_type&&, const allocator_type&);
		// this_type& operator=(this_type&& x);

		fixed_slist<TestObject, 16> slist3TO33(3, TestObject(33));
		fixed_slist<TestObject, 16> toListA(eastl::move(slist3TO33));
		EATEST_VERIFY((toListA.size() == 3) && (toListA.front().mX == 33) /* && (slist3TO33.size() == 0) fixed_list usually can't honor the move request. */);

		// The following is not as strong a test of this ctor as it could be. A stronger test would be to use IntanceAllocator with different instances.
		fixed_slist<TestObject, 16, true, MallocAllocator> slist4TO44(4, TestObject(44));
		fixed_slist<TestObject, 16, true, MallocAllocator> toListB(eastl::move(slist4TO44), MallocAllocator());
		EATEST_VERIFY((toListB.size() == 4) && (toListB.front().mX == 44) /* && (slist4TO44.size() == 0) fixed_list usually can't honor the move request. */);

		fixed_slist<TestObject, 16, true, MallocAllocator> slist5TO55(5, TestObject(55));
		toListB = eastl::move(slist5TO55);
		EATEST_VERIFY((toListB.size() == 5) && (toListB.front().mX == 55) /* && (slist5TO55.size() == 0) fixed_list usually can't honor the move request. */);
	}


	{
		// template <class... Args>
		// void emplace_front(Args&&... args);

		// template <class... Args>
		// iterator emplace_after(const_iterator position, Args&&... args);

		TestObject::Reset();

		fixed_slist<TestObject, 16> toListA;

		toListA.emplace_front(1, 2, 3); // This uses the TestObject(int x0, int x1, int x2, bool bThrowOnCopy) constructor.
		EATEST_VERIFY((toListA.size() == 1) && (toListA.front().mX == (1+2+3)) && (TestObject::sTOCtorCount == 1));

		toListA.emplace_after(toListA.before_begin(), 3, 4, 5);
		EATEST_VERIFY((toListA.size() == 2) && (toListA.front().mX == (3+4+5)) && (TestObject::sTOCtorCount == 2));


		// This test is similar to the emplace pathway above. 
		TestObject::Reset();

		// void push_front(T&& x);
		// iterator insert(const_iterator position, T&& x);

		fixed_slist<TestObject, 16> toListC;

		toListC.push_front(TestObject(1, 2, 3));
		EATEST_VERIFY((toListC.size() == 1) && (toListC.front().mX == (1+2+3)) && (TestObject::sTOMoveCtorCount == 1));

		toListC.insert_after(toListC.before_begin(), TestObject(3, 4, 5));
		EATEST_VERIFY((toListC.size() == 2) && (toListC.front().mX == (3+4+5)) && (TestObject::sTOMoveCtorCount == 2));
	}


	{
		// slist(std::initializer_list<value_type> ilist, const allocator_type& allocator = EASTL_SLIST_DEFAULT_ALLOCATOR);
		// this_type& operator=(std::initializer_list<value_type>);
		// void assign(std::initializer_list<value_type> ilist);
		// iterator insert_after(iterator position, std::initializer_list<value_type> ilist);
		#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
			fixed_slist<int, 8> intList = { 0, 1, 2 };
			EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "fixed_slist std::initializer_list", 0, 1, 2, -1));

			intList = { 13, 14, 15 };
			EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "fixed_slist std::initializer_list", 13, 14, 15, -1));

			intList.assign({ 16, 17, 18 });
			EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "fixed_slist std::initializer_list", 16, 17, 18, -1));

			fixed_slist<int, 8>::iterator it = intList.insert_after(intList.before_begin(), { 14, 15 });
			EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "fixed_slist std::initializer_list", 14, 15, 16, 17, 18, -1));
			EATEST_VERIFY(*it == 15); // Note that slist::insert_after returns the last inserted element, not the first as with list::insert.
		#endif
	}


	{
		// Test construction of a container with an overflow allocator constructor argument.
		//
		// GCC 4.4 has a hard time compiling this code correctly in optimized builds as it 
		// omits the increment of the mAllocCount field when calling overflowAllocator.allocate.
		#if defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION == 4004)
			MallocAllocator overflowAllocator;
			fixed_slist<int, 64, true, MallocAllocator> c(overflowAllocator);
			c.resize(65);
			VERIFY(c.get_overflow_allocator().mAllocCount == 1); // 1 for overflowing from 64 to 65.
		#else
			MallocAllocator overflowAllocator;
			void* p = overflowAllocator.allocate(1);
			fixed_slist<int, 64, true, MallocAllocator> c(overflowAllocator);
			c.resize(65);
			VERIFY(c.get_overflow_allocator().mAllocCount == 2); // 1 for above, and 1 for overflowing from 64 to 65.
			overflowAllocator.deallocate(p, 1);
		#endif
	}


	// We can't do this, due to how Reset is used above:
	//   EATEST_VERIFY(TestObject::IsClear());
	EATEST_VERIFY(TestObject::sMagicErrorCount == 0);
	TestObject::Reset();


	return nErrorCount;
}










