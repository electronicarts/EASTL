/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "TestSet.h"
#include "EASTLTest.h"
#include <EASTL/map.h>
#include <EASTL/set.h>
#include <EASTL/functional.h>
#include <EASTL/internal/config.h>
#include <EABase/eabase.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <stdio.h>

#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	#include <set>
	#include <map>
	#include <algorithm>
#endif
EA_RESTORE_ALL_VC_WARNINGS()

using namespace eastl;


// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::set<int>;
template class eastl::multiset<float>;
template class eastl::set<TestObject>;
template class eastl::multiset<TestObject>;


///////////////////////////////////////////////////////////////////////////////
// typedefs
//
typedef eastl::set<int> VS1;
typedef eastl::set<TestObject> VS4;
typedef eastl::multiset<int> VMS1;
typedef eastl::multiset<TestObject> VMS4;

#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	typedef std::set<int> VS3;
	typedef std::set<TestObject> VS6;
	typedef std::multiset<int> VMS3;
	typedef std::multiset<TestObject> VMS6;
#endif

///////////////////////////////////////////////////////////////////////////////



int TestSet()
{
	EASTLTest_Printf("TestSet\n");

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
		nErrorCount += TestSetCpp11<eastl::set<TestObject> >();

		nErrorCount += TestMultisetCpp11<eastl::multiset<TestObject> >();
	}


	{ // Misc tests

		// const key_compare& key_comp() const;
		// key_compare&       key_comp();
		VS1       vs;
		const VS1 vsc;

		const VS1::key_compare& kc = vsc.key_comp();
		vs.key_comp() = kc;
	}


	return nErrorCount;
}












