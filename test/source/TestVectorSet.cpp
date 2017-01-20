/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////



#include "TestSet.h"
#include "EASTLTest.h"
#include <EASTL/vector_set.h>
#include <EASTL/vector_multiset.h>
#include <EASTL/vector.h>
#include <EASTL/deque.h>
#include <EABase/eabase.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
	#pragma warning(disable: 4702) // VC++ STL headers generate this. warning C4702: unreachable code
	#pragma warning(disable:4350) // for whatever reason, the push,0 above does not turn this warning off with vs2012.
								  // VC++ 2012 STL headers generate this. warning C4350: behavior change: 'std::_Wrap_alloc<_Alloc>::_Wrap_alloc(const std::_Wrap_alloc<_Alloc> &) throw()' called instead of 'std::_Wrap_alloc<_Alloc>::_Wrap_alloc<std::_Wrap_alloc<_Alloc>>(_Other &) throw()'
#endif

#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	#include <set>
#endif

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


using namespace eastl;


// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::vector_set<int>;
template class eastl::vector_multiset<float>;
template class eastl::vector_set<TestObject>;
template class eastl::vector_multiset<TestObject>;


///////////////////////////////////////////////////////////////////////////////
// typedefs
//
typedef eastl::vector_set<int> VS1;
typedef eastl::vector_set<int, eastl::less<int>, EASTLAllocatorType, eastl::deque<int> > VS2;
typedef eastl::vector_set<TestObject> VS4;
typedef eastl::vector_set<TestObject, eastl::less<TestObject>, EASTLAllocatorType, eastl::deque<TestObject> > VS5;
typedef eastl::vector_multiset<int> VMS1;
typedef eastl::vector_multiset<int, eastl::less<int>, EASTLAllocatorType, eastl::deque<int> > VMS2;
typedef eastl::vector_multiset<TestObject> VMS4;
typedef eastl::vector_multiset<TestObject, eastl::less<TestObject>, EASTLAllocatorType, eastl::deque<TestObject> > VMS5;

#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	typedef std::set<int> VS3;
	typedef std::set<TestObject> VS6;
	typedef std::multiset<int> VMS3;
	typedef std::multiset<TestObject> VMS6;
#endif
///////////////////////////////////////////////////////////////////////////////



int TestVectorSet()
{
	EASTLTest_Printf("TestVectorSet\n");

	int nErrorCount = 0;

	#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
		{   // Test construction
			nErrorCount += TestSetConstruction<VS1, VS3, false>();
			nErrorCount += TestSetConstruction<VS2, VS3, false>();
			nErrorCount += TestSetConstruction<VS4, VS6, false>();
			nErrorCount += TestSetConstruction<VS5, VS6, false>();

			nErrorCount += TestSetConstruction<VMS1, VMS3, true>();
			nErrorCount += TestSetConstruction<VMS2, VMS3, true>();
			nErrorCount += TestSetConstruction<VMS4, VMS6, true>();
			nErrorCount += TestSetConstruction<VMS5, VMS6, true>();
		}


		{   // Test mutating functionality.
			nErrorCount += TestSetMutation<VS1, VS3, false>();
			nErrorCount += TestSetMutation<VS2, VS3, false>();
			nErrorCount += TestSetMutation<VS4, VS6, false>();
			nErrorCount += TestSetMutation<VS5, VS6, false>();

			nErrorCount += TestSetMutation<VMS1, VMS3, true>();
			nErrorCount += TestSetMutation<VMS2, VMS3, true>();
			nErrorCount += TestSetMutation<VMS4, VMS6, true>();
			nErrorCount += TestSetMutation<VMS5, VMS6, true>();
		}
	#endif // EA_COMPILER_NO_STANDARD_CPP_LIBRARY


	{   // Test search functionality.
		nErrorCount += TestSetSearch<VS1, false>();
		nErrorCount += TestSetSearch<VS2, false>();
		nErrorCount += TestSetSearch<VS4, false>();
		nErrorCount += TestSetSearch<VS5, false>();

		nErrorCount += TestSetSearch<VMS1, true>();
		nErrorCount += TestSetSearch<VMS2, true>();
		nErrorCount += TestSetSearch<VMS4, true>();
		nErrorCount += TestSetSearch<VMS5, true>();
	}


	{
		// C++11 emplace and related functionality
		nErrorCount += TestSetCpp11<VS4>();
		nErrorCount += TestSetCpp11<VS5>();

		nErrorCount += TestMultisetCpp11<VMS4>();
		nErrorCount += TestMultisetCpp11<VMS5>();
	}


	{ // Misc tests

		// const key_compare& key_comp() const;
		// key_compare&       key_comp();
		VS2       vs;
		const VS2 vsc;

		const VS2::key_compare& kc = vsc.key_comp();
		vs.key_comp() = kc;

		// verify count method is const qualified
		vsc.count(0);
	}


	return nErrorCount;
}












