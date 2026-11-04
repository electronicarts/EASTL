/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "TestMap.h"
#include "EASTLTest.h"
#include <EASTL/map.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include "TestAssociativeContainers.h"

EA_DISABLE_ALL_VC_WARNINGS()
#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	#include <map>
#endif
EA_RESTORE_ALL_VC_WARNINGS()

using namespace eastl;


// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::map<int, int>;
template class eastl::multimap<int, int>;
template class eastl::map<TestObject, TestObject>;
template class eastl::multimap<TestObject, TestObject>;


///////////////////////////////////////////////////////////////////////////////
// typedefs
//
typedef eastl::map<int, int> VM1;
typedef eastl::map<TestObject, TestObject> VM4;
typedef eastl::map<Align64, Align64> VM7;
typedef eastl::multimap<int, int> VMM1;
typedef eastl::multimap<TestObject, TestObject> VMM4;
typedef eastl::multimap<Align64, Align64> VMM7;

#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	typedef std::map<int, int> VM3;
	typedef std::map<TestObject, TestObject> VM6;
	typedef std::map<Align64, Align64> VM9;
	typedef std::multimap<int, int> VMM3;
	typedef std::multimap<TestObject, TestObject> VMM6;
	typedef std::multimap<Align64, Align64> VMM9;
#endif

///////////////////////////////////////////////////////////////////////////////

namespace
{
	// Comparator that counts how many times it is called, so that tests can tell
	// a hinted (constant time) insertion apart from a second full tree search.
	struct CountingLess
	{
		static int sCount;
		bool operator()(int a, int b) const { ++sCount; return a < b; }
	};
	int CountingLess::sCount = 0;

	typedef eastl::map<int, int, CountingLess> CountingMap;

	// Calls operator[] with a key that is not in the map yet and returns how many more
	// comparisons it used than a plain lower_bound() on the same key. Once the key has
	// been located, inserting it should only need a few comparisons, not another search.
	int OperatorBracketExtraCompares(CountingMap& m, int key, bool bUseRValue)
	{
		CountingLess::sCount = 0;
		m.lower_bound(key);
		const int nLowerBoundCompares = CountingLess::sCount;

		CountingLess::sCount = 0;
		int keyCopy = key;
		int& value = bUseRValue ? m[eastl::move(keyCopy)] : m[key];
		const int nOperatorBracketCompares = CountingLess::sCount;

		value = key;
		return nOperatorBracketCompares - nLowerBoundCompares;
	}
}

int TestMap()
{
	int nErrorCount = 0;

	#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
		{   // Test construction
			nErrorCount += TestMapConstruction<VM1, VM3, false>();
			nErrorCount += TestMapConstruction<VM4, VM6, false>();
			nErrorCount += TestMapConstruction<eastl::map<int, int, eastl::less<void>>, std::map<int, int, std::less<void>>, false>();

			nErrorCount += TestMapConstruction<VMM1, VMM3, true>();
			nErrorCount += TestMapConstruction<VMM4, VMM6, true>();
			nErrorCount += TestMapConstruction<eastl::multimap<int, int, eastl::less<void>>, std::multimap<int, int, std::less<void>>, true>();
		}


		{   // Test mutating functionality.
			nErrorCount += TestMapMutation<VM1, VM3, false>();
			nErrorCount += TestMapMutation<VM4, VM6, false>();
			nErrorCount += TestMapMutation<eastl::map<int, int, eastl::less<void>>, std::map<int, int, std::less<void>>, false>();

			nErrorCount += TestMapMutation<VMM1, VMM3, true>();
			nErrorCount += TestMapMutation<VMM4, VMM6, true>();
			nErrorCount += TestMapMutation<eastl::multimap<int, int, eastl::less<void>>, std::multimap<int, int, std::less<void>>, true>();
		}

		// Note: some std:: libraries throw UBSAN errors with Align64.
		// So we only run these tests when UBSAN is not enabled to
		// keep our UBSAN builds clean.
		#if !EA_UBSAN_ENABLED
			{
				// Construction
				nErrorCount += TestMapConstruction<VM7, VM9, false>();
				nErrorCount += TestMapConstruction<VMM7, VMM9, true>();
				nErrorCount += TestMapConstruction<eastl::map<int, int, eastl::less<void>>, std::map<int, int, std::less<void>>, false>();
				nErrorCount += TestMapConstruction<eastl::multimap<int, int, eastl::less<void>>, std::multimap<int, int, std::less<void>>, true>();

				// Mutation
				nErrorCount += TestMapMutation<VMM7, VMM9, true>();
				nErrorCount += TestMapMutation<VM7, VM9, false>();
				nErrorCount += TestMapMutation<eastl::map<int, int, eastl::less<void>>, std::map<int, int, std::less<void>>, false>();
				nErrorCount += TestMapMutation<eastl::multimap<int, int, eastl::less<void>>, std::multimap<int, int, std::less<void>>, true>();
			}
		#endif // !EA_UBSAN_ENABLED

	#endif // EA_COMPILER_NO_STANDARD_CPP_LIBRARY


	{   // Test searching functionality.
		nErrorCount += TestMapSearch<VM1, false>();
		nErrorCount += TestMapSearch<VM4, false>();
		nErrorCount += TestMapSearch<VM7, false>();
		nErrorCount += TestMapSearch<eastl::map<int, int, eastl::less<void>>, false>();

		nErrorCount += TestMapSearch<VMM1, true>();
		nErrorCount += TestMapSearch<VMM4, true>();
		nErrorCount += TestMapSearch<VMM7, true>();
		nErrorCount += TestMapSearch<eastl::multimap<int, int, eastl::less<void>>, true>();
	}


	{
		// C++11 emplace and related functionality
		nErrorCount += TestMapCpp11<eastl::map<int, TestObject>>();
		nErrorCount += TestMapCpp11<eastl::map<int, TestObject, eastl::less<void>>>();
		nErrorCount += TestMultimapCpp11<eastl::multimap<int, TestObject>>();
		nErrorCount += TestMultimapCpp11<eastl::multimap<int, TestObject, eastl::less<void>>>();
		nErrorCount += TestMapCpp11NonCopyable<eastl::map<int, NonCopyable>>();
		nErrorCount += TestMapCpp11NonCopyable<eastl::map<int, NonCopyable, eastl::less<void>>>();
	}

	{
		// C++17 try_emplace and related functionality
		nErrorCount += TestMapCpp17<eastl::map<int, TestObject>>();
		nErrorCount += TestMapCpp17<eastl::map<int, TestObject, eastl::less<void>>>();
	}

	{
		// Tests for element access: operator[] and at()
		nErrorCount += TestMapAccess<VM1>();
		nErrorCount += TestMapAccess<VM4>();
		nErrorCount += TestMapAccess<VM7>();
		nErrorCount += TestMapAccess<eastl::map<int, int, eastl::less<void>>>();
	}


	{ // Misc tests

		// const key_compare& key_comp() const;
		// key_compare&       key_comp();
		VM1       vm;
		const VM1 vmc;

		const VM1::key_compare& kc = vmc.key_comp();
		vm.key_comp() = kc;
	}


	// Regressions against user bug reports.
	{
		// User reports that the following doesn't compile on GCC 4.1.1 due to unrecognized lower_bound.
		eastl::map<int, int> m;
		m[1] = 1;
		EATEST_VERIFY(m.size() == 1);
		m.erase(1);
		EATEST_VERIFY(m.empty());
	}

	{
		// User reports that EASTL_VALIDATE_COMPARE_ENABLED / EASTL_COMPARE_VALIDATE isn't compiling for this case.
		eastl::map<eastl::u8string, int> m; 
		m.find_as(EA_CHAR8("some string"), eastl::equal_to<>()); 
	}

	{
		eastl::map<int*, int> m;
		int* ip = (int*)(uintptr_t)0xDEADC0DE;

		m[ip] = 0;

		auto it = m.find_as(ip, eastl::less<>{});
		EATEST_VERIFY(it != m.end());

		it = m.find_as((int*)(uintptr_t)0xDEADC0DE, eastl::less<>{});
		EATEST_VERIFY(it != m.end());
	}

	{
		// User reports that vector<map<enum,enum>> is crashing after the recent changes to add rvalue move and emplace support to rbtree.
		typedef eastl::map<int, int>     IntIntMap;
		typedef eastl::vector<IntIntMap> IntIntMapArray;

		IntIntMapArray v;
		v.push_back(IntIntMap());           // This was calling the rbtree move constructor, which had a bug.
		v[0][16] = 0;                       // The rbtree was in a bad internal state and so this line resulted in a crash.
		EATEST_VERIFY(v[0].validate());
		EATEST_VERIFY(v.validate());
	}

	// User regression test
	{
	#if !EASTL_RBTREE_LEGACY_SWAP_BEHAVIOUR_REQUIRES_COPY_CTOR
		typedef eastl::map<int, MoveOnlyTypeDefaultCtor> IntMOMap;

		IntMOMap m1, m2;
		m2[0] = MoveOnlyTypeDefaultCtor(0);
		m2[1] = MoveOnlyTypeDefaultCtor(1);

		EATEST_VERIFY( m1.empty());
		EATEST_VERIFY(!m2.empty());

		m1.swap(m2);

		EATEST_VERIFY(!m1.empty());
		EATEST_VERIFY( m2.empty());
	#endif
	}

//    todo:  create a test case for this.
//    {	
//      // User reports that an incorrectly wrapped pair key used to insert into an eastl map compiles when it should fire a compiler error about unconvertible types.
//		typedef eastl::pair<eastl::string, eastl::string> PairStringKey;
//        typedef eastl::map<PairStringKey, eastl::string> PairStringMap;
//
//        PairStringMap p1, p2;		
//
//        p1.insert(PairStringMap::value_type(PairStringKey("key1", "key2"), "data")).first->second = "other_data";
//
//        PairStringKey key("key1", "key2");
//		PairStringMap::value_type insert_me(key, "data");
//        p2.insert(insert_me).first->second = "other_data";
//
//		for(auto& e : p1)
//			printf("%s,%s = %s\n", e.first.first.c_str(), e.first.second.c_str(), e.second.c_str());
//
//		for(auto& e : p2)
//			printf("%s,%s = %s\n", e.first.first.c_str(), e.first.second.c_str(), e.second.c_str());
//
//        EATEST_VERIFY(p1 == p2); 
//    }

	{
		// map::operator[] used to pass lower_bound(key) as the insertion hint, but the rbtree hint
		// must be the element *before* the insertion point. The hint was rejected and every miss
		// that wasn't at the end of the map searched the tree a second time.
		// https://github.com/electronicarts/EASTL/issues/592
		const int kCount = 1024;
		const int kMaxExtraCompares = 6; // 1 to detect the miss, up to 3 to use the hint, up to 2 for EASTL_VALIDATE_COMPARE.

		for (int pass = 0; pass < 2; ++pass)
		{
			const bool bUseRValue = (pass == 1);
			CountingMap m;
			int nSlowInserts = 0;

			for (int i = 0; i < kCount; ++i)
				m.insert(CountingMap::value_type(i * 2, i * 2)); // Even keys only; odd keys are inserted below.

			// Odd keys in scrambled order, so the insertion point is all over the tree.
			for (int i = 0; i < kCount; ++i)
			{
				const int key = ((i * 389) % kCount) * 2 + 1; // 389 is coprime with kCount, so this visits every odd key once.
				if (OperatorBracketExtraCompares(m, key, bUseRValue) > kMaxExtraCompares)
					++nSlowInserts;
			}

			// New smallest key (insert before begin()) and new largest key (insert at end()).
			if (OperatorBracketExtraCompares(m, -1, bUseRValue) > kMaxExtraCompares)
				++nSlowInserts;
			if (OperatorBracketExtraCompares(m, kCount * 2, bUseRValue) > kMaxExtraCompares)
				++nSlowInserts;

			EATEST_VERIFY(nSlowInserts == 0);
			EATEST_VERIFY(m.validate());
			EATEST_VERIFY(m.size() == (eastl_size_t)(kCount * 2 + 2));

			int expected = -1;
			for (CountingMap::iterator it = m.begin(); it != m.end(); ++it, ++expected)
				EATEST_VERIFY((it->first == expected) && (it->second == expected));
		}

		{
			// Filling an empty map in descending order inserts before begin() every time.
			CountingMap m;
			int nSlowInserts = 0;

			for (int key = kCount; key > 0; --key)
			{
				if (OperatorBracketExtraCompares(m, key, false) > kMaxExtraCompares)
					++nSlowInserts;
			}

			EATEST_VERIFY(nSlowInserts == 0);
			EATEST_VERIFY(m.validate());
			EATEST_VERIFY(m.size() == (eastl_size_t)kCount);
			EATEST_VERIFY((m.begin()->first == 1) && (m.rbegin()->first == kCount));
		}

		{
			// Existing keys must not be inserted or modified.
			CountingMap m;
			m[1] = 10;
			m[3] = 30;
			EATEST_VERIFY(m[1] == 10);
			EATEST_VERIFY(m[3] == 30);
			EATEST_VERIFY(m[2] == 0);
			EATEST_VERIFY(m.size() == 3);
			EATEST_VERIFY(m.validate());
		}
	}

	{ // Test empty base-class optimization
		struct UnemptyLess : eastl::less<int>
		{
			int foo;
		};

		typedef eastl::map<int, int, eastl::less<int>> VM1;
		typedef eastl::map<int, int, UnemptyLess> VM2;

		EATEST_VERIFY(sizeof(VM1) < sizeof(VM2));
	}

	{ // Test erase_if
		eastl::map<int, int> m = {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}};
		auto numErased = eastl::erase_if(m, [](auto p) { return p.first % 2 == 0; });
		VERIFY((m == eastl::map<int, int>{{1, 1},{3, 3}}));
		VERIFY(numErased == 3);
	}

	{ // Test erase_if
		eastl::multimap<int, int> m = {{0, 0}, {0, 0}, {0, 0}, {1, 1}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {4, 4}, {4, 4}};
		auto numErased = eastl::erase_if(m, [](auto p) { return p.first % 2 == 0; });
		VERIFY((m == eastl::multimap<int, int>{{1, 1}, {1, 1}, {3, 3}}));;
		VERIFY(numErased == 7);
	}

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	{ // Test map <=>
		eastl::map<int, int> m1 = {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}};
		eastl::map<int, int> m2 = {{4, 4}, {3, 3}, {2, 2}, {1, 1}, {0, 0}};
		eastl::map<int, int> m3 = {{0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}};
		eastl::map<int, int> m4 = {{1, 0}, {3, 2}, {5, 4}, {7, 6}, {9, 8}};
		eastl::map<int, int> m5 = {{0, 1}, {2, 3}, {4, 5}};

		VERIFY(m1 == m2);
		VERIFY(m1 != m3);
		VERIFY(m3 != m4);
		VERIFY(m3 < m4);
		VERIFY(m5 < m4);
		VERIFY(m5 < m3);


		VERIFY((m1 <=> m2) == 0);
		VERIFY((m1 <=> m3) != 0);
		VERIFY((m3 <=> m4) != 0);
		VERIFY((m3 <=> m4) < 0);
		VERIFY((m5 <=> m4) < 0);
		VERIFY((m5 <=> m3) < 0);
	}

	{ // Test multimap <=>
		eastl::multimap<int, int> m1 = {{0, 0}, {0, 0}, {1, 1}, {1, 1}, {2, 2}, {2, 2}, {3, 3}, {3, 3}, {4, 4}, {4, 4}};
		eastl::multimap<int, int> m2 = {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {4, 4}, {3, 3}, {2, 2}, {1, 1}, {0, 0}};
		eastl::multimap<int, int> m3 = {{0, 1}, {2, 3}, {4, 5}, {0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}};
		eastl::multimap<int, int> m4 = {{1, 0}, {3, 2}, {5, 4}, {1, 0}, {3, 2}, {5, 4}, {7, 6}, {9, 8}};
		eastl::multimap<int, int> m5 = {{10, 11}, {10, 11}};

		VERIFY(m1 == m2);
		VERIFY(m1 != m3);
		VERIFY(m3 != m4);
		VERIFY(m3 < m4);
		VERIFY(m5 > m4);
		VERIFY(m5 > m3);

		VERIFY((m1 <=> m2) == 0);
		VERIFY((m1 <=> m3) != 0);
		VERIFY((m3 <=> m4) != 0);
		VERIFY((m3 <=> m4) < 0);
		VERIFY((m5 <=> m4) > 0);
		VERIFY((m5 <=> m3) > 0);
	}
#endif

	{ // heterogenous functions - map
		eastl::map<ExplicitString, int, eastl::less<void>> m{ { ExplicitString::Create("found"), 1 } };
		nErrorCount += TestAssociativeContainerHeterogeneousLookup(m);
		nErrorCount += TestOrderedAssociativeContainerHeterogeneousLookup(m);
		nErrorCount += TestMapHeterogeneousInsertion<decltype(m)>();
		nErrorCount += TestAssociativeContainerHeterogeneousErasure(m);
	}

	{ // heterogenous functions - multimap
		eastl::multimap<ExplicitString, int, eastl::less<void>> m{ { ExplicitString::Create("found"), 1 } };
		nErrorCount += TestAssociativeContainerHeterogeneousLookup(m);
		nErrorCount += TestOrderedAssociativeContainerHeterogeneousLookup(m);

		VERIFY(m.equal_range_small("not found") == eastl::make_pair(m.lower_bound("not found"), m.upper_bound("not found")));
		VERIFY(m.equal_range_small("found") == eastl::make_pair(m.lower_bound("found"), m.upper_bound("found")));

		nErrorCount += TestAssociativeContainerHeterogeneousErasure(m);
	}

	return nErrorCount;
}











