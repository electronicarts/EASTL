/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include "TestSet.h"
#include <EASTL/fixed_set.h>

EA_DISABLE_ALL_VC_WARNINGS()
#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	#include <set>
#endif
EA_RESTORE_ALL_VC_WARNINGS()

using namespace eastl;


// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::fixed_set     <int,        1>;
template class eastl::fixed_multiset<float,      1>;
template class eastl::fixed_set     <Align64,    1>;
template class eastl::fixed_multiset<TestObject, 1>;


template class eastl::fixed_set     <int,        1, true, eastl::less<int>,        MallocAllocator>;
template class eastl::fixed_multiset<float,      1, true, eastl::less<float>,      MallocAllocator>;
template class eastl::fixed_set     <Align64,    1, true, eastl::less<Align64>,    MallocAllocator>;
template class eastl::fixed_multiset<TestObject, 1, true, eastl::less<TestObject>, MallocAllocator>;


///////////////////////////////////////////////////////////////////////////////
// typedefs
//
const eastl_size_t kContainerSize = 1000;

typedef eastl::fixed_set<int, kContainerSize> VS1;
typedef eastl::fixed_set<TestObject, kContainerSize> VS4;
typedef eastl::fixed_multiset<int, kContainerSize> VMS1;
typedef eastl::fixed_multiset<TestObject, kContainerSize> VMS4;

#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	typedef std::set<int> VS3;
	typedef std::set<TestObject> VS6;
	typedef std::multiset<int> VMS3;
	typedef std::multiset<TestObject> VMS6;
#endif

///////////////////////////////////////////////////////////////////////////////


EA_DISABLE_VC_WARNING(6262)
int TestFixedSet()
{
	int nErrorCount = 0;

	#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
		{   // Test construction
			nErrorCount += TestSetConstruction<VS1, VS3, false>();
			nErrorCount += TestSetConstruction<VS4, VS6, false>();

			nErrorCount += TestSetConstruction<VMS1, VMS3, true>();
			nErrorCount += TestSetConstruction<VMS4, VMS6, true>();
		}


		{   // Test mutating functionality.
			nErrorCount += TestSetMutation<VS1, VS3, false>();
			nErrorCount += TestSetMutation<VS4, VS6, false>();

			nErrorCount += TestSetMutation<VMS1, VMS3, true>();
			nErrorCount += TestSetMutation<VMS4, VMS6, true>();
		}
	#endif // EA_COMPILER_NO_STANDARD_CPP_LIBRARY


	{   // Test searching functionality.
		nErrorCount += TestSetSearch<VS1, false>();
		nErrorCount += TestSetSearch<VS4, false>();

		nErrorCount += TestSetSearch<VMS1, true>();
		nErrorCount += TestSetSearch<VMS4, true>();
	}


	{
		// C++11 emplace and related functionality
		nErrorCount += TestSetCpp11<eastl::fixed_set<TestObject, 32> >();

		nErrorCount += TestMultisetCpp11<eastl::fixed_multiset<TestObject, 32> >();
	}


	{ // Test functionality specific to fixed size containers.

		VS1  vs1;
		VMS1 vms1;

		VERIFY(vs1.max_size() == kContainerSize);
		VERIFY(vms1.max_size() == kContainerSize);
	}


	{
		// Test version *without* pool overflow.
		typedef eastl::fixed_set<int, 100, false> FixedSetFalse;
		FixedSetFalse fixedSet;

		fixedSet.insert(FixedSetFalse::value_type(0));
		VERIFY(fixedSet.size() == 1);

		fixedSet.clear();
		VERIFY(fixedSet.size() == 0);

		for(int i = 0; fixedSet.size() < 100; i++)
			fixedSet.insert(FixedSetFalse::value_type(i));
		VERIFY(fixedSet.size() == 100);

		// Verify that we allocated enough space for exactly N items. 
		// It's possible that due to alignments, there might be room for N + 1.
		FixedSetFalse::allocator_type& allocator = fixedSet.get_allocator();
		void* pResult = allocator.allocate(sizeof(FixedSetFalse::node_type));
		if(pResult)
		{
			pResult = allocator.allocate(sizeof(FixedSetFalse::node_type));
			VERIFY(pResult == NULL); 
		}
	}


	{
		// Test version *with* pool overflow.
		typedef eastl::fixed_set<int, 100, true> FixedSetTrue;
		FixedSetTrue fixedSet;

		fixedSet.insert(FixedSetTrue::value_type(0));
		VERIFY(fixedSet.size() == 1);

		fixedSet.clear();
		VERIFY(fixedSet.size() == 0);

		for(int i = 0; fixedSet.size() < 100; i++)
			fixedSet.insert(FixedSetTrue::value_type(i));
		VERIFY(fixedSet.size() == 100);

		FixedSetTrue::allocator_type& allocator = fixedSet.get_allocator();
		void* pResult = allocator.allocate(sizeof(FixedSetTrue::node_type));
		VERIFY(pResult != NULL);
		allocator.deallocate(pResult, sizeof(FixedSetTrue::node_type));

		// get_overflow_allocator / set_overflow_allocator
		// This is a weak test which should be improved.
		EASTLAllocatorType a = fixedSet.get_allocator().get_overflow_allocator();
		fixedSet.get_allocator().set_overflow_allocator(a);
	}

	{
		// Test fixed set with overflow and alignment requirements.
		typedef fixed_set<Align64, 1, true> FixedSetWithAlignment;
		typedef fixed_multiset<Align64, 1, true> FixedMultiSetWithAlignment;

		FixedSetWithAlignment fs;
		FixedMultiSetWithAlignment fms;

		Align64 a; a.mX = 1;
		Align64 b; b.mX = 2;
		Align64 c; c.mX = 3;
		Align64 d; d.mX = 4;
		Align64 e; e.mX = 5;

		fs.insert(a);
		fs.insert(b);
		fs.insert(c);
		fs.insert(d);
		fs.insert(e);
		for (FixedSetWithAlignment::const_iterator it = fs.begin(); it != fs.end(); ++it)
		{
			const Align64* ptr = &(*it);
			EATEST_VERIFY((uint64_t)ptr % EASTL_ALIGN_OF(Align64) == 0);
		}
		fms.insert(a);
		fms.insert(b);
		fms.insert(c);
		fms.insert(d);
		fms.insert(e);
		for (FixedMultiSetWithAlignment::const_iterator it = fms.begin(); it != fms.end(); ++it)
		{
			const Align64* ptr = &(*it);
			EATEST_VERIFY((uint64_t)ptr % EASTL_ALIGN_OF(Align64) == 0);
		}
	}
	return nErrorCount;
}
EA_RESTORE_VC_WARNING()












