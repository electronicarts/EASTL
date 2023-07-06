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

EA_DISABLE_ALL_VC_WARNINGS()
#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	#include <set>
#endif
EA_RESTORE_ALL_VC_WARNINGS()

using namespace eastl;


// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::vector_set<int>;
template class eastl::vector_multiset<float>;
template class eastl::vector_set<TestObject>;
template class eastl::vector_multiset<TestObject>;

static_assert(sizeof(eastl::vector_set<int>) == sizeof(eastl::vector<int>), "if is_empty_v<Compare>, sizeof(vector_set) == sizeof(RandomAccessContainer)");
static_assert(sizeof(eastl::vector_set<TestObject>) == sizeof(eastl::vector<TestObject>), "if is_empty_v<Compare>, sizeof(vector_set) == sizeof(RandomAccessContainer)");
static_assert(sizeof(eastl::vector_multiset<int>) == sizeof(eastl::vector<int>), "if is_empty_v<Compare>, sizeof(vector_multiset) == sizeof(RandomAccessContainer)");
static_assert(sizeof(eastl::vector_multiset<TestObject>) == sizeof(eastl::vector<TestObject>), "if is_empty_v<Compare>, sizeof(vector_multiset) == sizeof(RandomAccessContainer)");


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


    {
        // insert at the upper bound of a range
        VMS1 vms = {0};
        VERIFY(vms.insert(0) != vms.begin());
    }


	{ // Misc tests
		{
			// const key_compare& key_comp() const;
			// key_compare&	   key_comp();
			VS2	   vs;
			const VS2 vsc;

			// ensure count can be called from a const object
			const VS2::key_compare& kc = vsc.key_comp();
			vs.key_comp() = kc;
			vsc.count(0);
		}

		{
			// ensure count can be called from a const object
			const VMS1 vms;
			vms.count(0);
		}
	}

	{ // find / find_as / lower_bound / upper_bound
		{ // vector_set
			eastl::vector_set<string> vss = {"abc", "def", "ghi", "jklmnop", "qrstu", "vw", "x", "yz"};
			VERIFY(vss.find("ghi") != vss.end());
			VERIFY(vss.find("GHI") == vss.end());
			VERIFY(vss.find_as("GHI", TestStrCmpI_2()) != vss.end());
			VERIFY(vss.lower_bound("ghi") != vss.end());
			VERIFY(vss.upper_bound("ghi") != vss.end());
		}

		{ // const vector_set
			const eastl::vector_set<string> vss = {"abc", "def", "ghi", "jklmnop", "qrstu", "vw", "x", "yz"};
			VERIFY(vss.find("ghi") != vss.end());
			VERIFY(vss.find("GHI") == vss.end());
			VERIFY(vss.find_as("GHI", TestStrCmpI_2()) != vss.end());
			VERIFY(vss.lower_bound("ghi") != vss.end());
			VERIFY(vss.upper_bound("ghi") != vss.end());
		}

		{ // vector_multiset
			eastl::vector_multiset<string> vss = {"abc", "def", "ghi", "jklmnop", "qrstu", "vw", "x", "yz"};
			VERIFY(vss.find("ghi") != vss.end());
			VERIFY(vss.find("GHI") == vss.end());
			VERIFY(vss.find_as("GHI", TestStrCmpI_2()) != vss.end());
			VERIFY(vss.lower_bound("ghi") != vss.end());
			VERIFY(vss.upper_bound("ghi") != vss.end());
		}

		{ // const vector_multiset
			const eastl::vector_multiset<string> vss = {"abc", "def", "ghi", "jklmnop", "qrstu", "vw", "x", "yz"};
			VERIFY(vss.find("ghi") != vss.end());
			VERIFY(vss.find("GHI") == vss.end());
			VERIFY(vss.find_as("GHI", TestStrCmpI_2()) != vss.end());
			VERIFY(vss.lower_bound("ghi") != vss.end());
			VERIFY(vss.upper_bound("ghi") != vss.end());
		}
	}

	return nErrorCount;
}












