/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include "TestMap.h"
#include <EASTL/fixed_map.h>

EA_DISABLE_ALL_VC_WARNINGS()
#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	#include <map>
#endif
EA_RESTORE_ALL_VC_WARNINGS()

using namespace eastl;


// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::fixed_map     <int,        float,      1>;
template class eastl::fixed_multimap<float,      int,        1>;
template class eastl::fixed_map     <int,        TestObject, 1>;
template class eastl::fixed_multimap<TestObject, int,        1>;

template class eastl::fixed_map     <int,        float,      1, true, eastl::less<int>,        MallocAllocator>;
template class eastl::fixed_multimap<float,      int,        1, true, eastl::less<float>,      MallocAllocator>;
template class eastl::fixed_map     <int,        TestObject, 1, true, eastl::less<int>,        MallocAllocator>;
template class eastl::fixed_multimap<TestObject, int,        1, true, eastl::less<TestObject>, MallocAllocator>;


///////////////////////////////////////////////////////////////////////////////
// typedefs
//
	const eastl_size_t kContainerSize = 1000;

typedef eastl::fixed_map<int, int, kContainerSize> VM1;
typedef eastl::fixed_map<TestObject, TestObject, kContainerSize> VM4;
typedef eastl::fixed_multimap<int, int, kContainerSize> VMM1;
typedef eastl::fixed_multimap<TestObject, TestObject, kContainerSize> VMM4;

#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	typedef std::map<int, int> VM3;
	typedef std::map<TestObject, TestObject> VM6;
	typedef std::multimap<int, int> VMM3;
	typedef std::multimap<TestObject, TestObject> VMM6;
#endif

///////////////////////////////////////////////////////////////////////////////


EA_DISABLE_VC_WARNING(6262)
int TestFixedMap()
{
	int nErrorCount = 0;

	#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
		{   // Test construction
			nErrorCount += TestMapConstruction<VM1, VM3, false>();
			nErrorCount += TestMapConstruction<VM4, VM6, false>();

			nErrorCount += TestMapConstruction<VMM1, VMM3, true>();
			nErrorCount += TestMapConstruction<VMM4, VMM6, true>();
		}


		{   // Test mutating functionality.
			nErrorCount += TestMapMutation<VM1, VM3, false>();
			nErrorCount += TestMapMutation<VM4, VM6, false>();

			nErrorCount += TestMapMutation<VMM1, VMM3, true>();
			nErrorCount += TestMapMutation<VMM4, VMM6, true>();
		}
	#endif // EA_COMPILER_NO_STANDARD_CPP_LIBRARY


	{   // Test searching functionality.
		nErrorCount += TestMapSearch<VM1, false>();
		nErrorCount += TestMapSearch<VM4, false>();

		nErrorCount += TestMapSearch<VMM1, true>();
		nErrorCount += TestMapSearch<VMM4, true>();
	}


	{
		// C++11 emplace and related functionality
		nErrorCount += TestMapCpp11<eastl::fixed_map<int, TestObject, 32> >();

		nErrorCount += TestMultimapCpp11<eastl::fixed_multimap<int, TestObject, 32> >();

		nErrorCount += TestMapCpp11NonCopyable<eastl::fixed_map<int, NonCopyable, 32>>();
	}

	{
		// C++17 try_emplace and related functionality
		nErrorCount += TestMapCpp17<eastl::fixed_map<int, TestObject, 32>>();
	}


	{ // Test functionality specific to fixed size containers.

		VM1  vm1;
		VMM1 vmm1;

		VERIFY(vm1.max_size() == kContainerSize);
		VERIFY(vmm1.max_size() == kContainerSize);
	}


	{   // Regression of bug report by Eric Turmel, May 20, 2008
		typedef eastl::fixed_map<int, TestObject, 37, false> FixedMap;
		VERIFY(FixedMap::kMaxSize == 37);

		FixedMap fixedMap;
		FixedMap::fixed_allocator_type& a = fixedMap.get_allocator();

		for(int i = 0; i < FixedMap::kMaxSize; i++)
		{
			VERIFY(a.can_allocate());

			fixedMap.insert(FixedMap::value_type(i, TestObject(i)));

			#if EASTL_FIXED_SIZE_TRACKING_ENABLED
				// Disabled because mPool is (mistakenly) inaccessible.
				// VERIFY((a.mPool.mnCurrentSize == a.mPool.mnPeakSize) && (a.mPool.mnCurrentSize == i));
			#endif
		}

		VERIFY(!a.can_allocate());
	}

	{
		// Test fixed set with overflow and alignment requirements.
		typedef fixed_map<Align64, int, 1, true> FixedMapWithAlignment;
		typedef fixed_multimap<Align64, int, 1, true> FixedMultiMapWithAlignment;

		FixedMapWithAlignment fm;
		FixedMultiMapWithAlignment fmm;

		Align64 a; a.mX = 1;
		Align64 b; b.mX = 2;
		Align64 c; c.mX = 3;
		Align64 d; d.mX = 4;
		Align64 e; e.mX = 5;

		fm.insert(a);
		fm.insert(b);
		fm.insert(c);
		fm.insert(d);
		fm.insert(e);
		for (FixedMapWithAlignment::const_iterator it = fm.begin(); it != fm.end(); ++it)
		{
			const Align64* ptr = &((*it).first);
			EATEST_VERIFY((uint64_t)ptr % EASTL_ALIGN_OF(Align64) == 0);
		}

		fmm.insert(a);
		fmm.insert(b);
		fmm.insert(c);
		fmm.insert(d);
		fmm.insert(e);
		for (FixedMultiMapWithAlignment::const_iterator it = fmm.begin(); it != fmm.end(); ++it)
		{
			const Align64* ptr = &((*it).first);
			EATEST_VERIFY((uint64_t)ptr % EASTL_ALIGN_OF(Align64) == 0);
		}
	}


	return nErrorCount;
}
EA_RESTORE_VC_WARNING()












