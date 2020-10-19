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


///////////////////////////////////////////////////////////////////////////////
// xvalue_test
//
// Test utility type that sets the class data to known value when its data has
// has been moved out.  This enables us to write tests that verify that the
// destruction action taken on container elements occured during move operations.
//
struct xvalue_test
{
	static const int MOVED_FROM = -1;

	int data = 42;

	xvalue_test(int in) : data(in) {}
	~xvalue_test() = default;

	xvalue_test(const xvalue_test& other) 
		: data(other.data) {}

	xvalue_test& operator=(const xvalue_test& other)
	{
		data = other.data;
		return *this;
	}

	xvalue_test(xvalue_test&& other)
	{
		data = other.data;
		other.data = MOVED_FROM;
	}

	xvalue_test& operator=(xvalue_test&& other)
	{
		data = other.data;
		other.data = MOVED_FROM;
		return *this;
	}

	friend bool operator<(const xvalue_test& rhs, const xvalue_test& lhs) 
		{ return rhs.data < lhs.data; }
};



int TestSet()
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

	{ // non-const comparator test
		struct my_less
		{
			bool operator()(int a, int b) { return a < b; }
		};

		{
			set<int, my_less> a = {0, 1, 2, 3, 4};
			auto i = a.find(42);
			VERIFY(i == a.end());
		}
	}

	{ // set erase_if tests
		set<int> s = {0, 1, 2, 3, 4};
		eastl::erase_if(s, [](auto i) { return i % 2 == 0;});
		VERIFY((s == set<int>{1,3}));
	}

	{ // multiset erase_if tests
		multiset<int> s = {0, 0, 0, 0, 0, 1, 1, 1, 2, 3, 3, 3, 4};
		eastl::erase_if(s, [](auto i) { return i % 2 == 0;});
		VERIFY((s == multiset<int>{1, 1, 1, 3, 3, 3}));
	}

	{
		// user reported regression: ensure container elements are NOT 
		// moved from during the eastl::set construction process.
		eastl::vector<xvalue_test> m1 = {{0}, {1}, {2}, {3}, {4}, {5}};
		eastl::set<xvalue_test> m2{m1.begin(), m1.end()};

		bool result = eastl::all_of(m1.begin(), m1.end(), 
				[&](auto& e) { return e.data != xvalue_test::MOVED_FROM; });

		VERIFY(result);
	}

	{
		// user reported regression: ensure container elements are moved from during the
		// eastl::set construction process when using an eastl::move_iterator.
		eastl::vector<xvalue_test> m1 = {{0}, {1}, {2}, {3}, {4}, {5}};
		eastl::set<xvalue_test> m2{eastl::make_move_iterator(m1.begin()), eastl::make_move_iterator(m1.end())};

		bool result = eastl::all_of(m1.begin(), m1.end(), 
				[&](auto& e) { return e.data == xvalue_test::MOVED_FROM; });

		VERIFY(result);
	}

	return nErrorCount;
}












