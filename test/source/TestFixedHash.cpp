/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include "EASTLTest.h"
#include "TestMap.h"
#include "TestSet.h"
#include <EASTL/fixed_hash_set.h>
#include <EASTL/fixed_hash_map.h>
#include <EASTL/fixed_vector.h>




using namespace eastl;


struct A
{
	int mX;
	A(int x = 999) : mX(x) {}
};

inline bool operator==(const A& a1, const A& a2)
	{ return a1.mX == a2.mX; }



namespace eastl
{
	template <> 
	struct hash<A>
	{
		size_t operator()(const A& a) const 
			{ return static_cast<size_t>(a.mX); }
	};
}


///////////////////////////////////////////////////////////////////////////////
// For test of user-reported crash.
//
struct MemoryEntry
{
	  size_t mSize;
	  void*  mGroup;
};
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// For test of bug reported by Dave Wall, May 14, 2008.
//
struct InstanceRenderData
{
	static const uint32_t kDataCount = 10; // Bug only occurs with this value.

	uint32_t mPad[kDataCount];

	InstanceRenderData()
	{
		memset(mPad, 0, sizeof(mPad));
	}

	bool operator==(const InstanceRenderData &rhs) const
	{
		for(uint32_t index = 0; index < kDataCount; index++)
		{
			if(mPad[index] != rhs.mPad[index])
			{
				return false;
			}
		}

		return true;
	}
};

namespace eastl
{
	template <> 
	struct hash<const InstanceRenderData>
	{
		size_t operator()(InstanceRenderData val) const 
		{
			return val.mPad[0];
		}
	};

	template <> 
	struct hash<InstanceRenderData>
	{
		size_t operator()(InstanceRenderData val) const 
		{
			return val.mPad[0];
		}
	};
}
///////////////////////////////////////////////////////////////////////////////



// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::fixed_hash_set<int, 1, 2>;
template class eastl::fixed_hash_map<int, int, 1, 2>;
template class eastl::fixed_hash_multiset<int, 1, 2>;
template class eastl::fixed_hash_multimap<int, int, 1, 2>;

template class eastl::fixed_hash_set<A, 1, 2>;
template class eastl::fixed_hash_map<A, A, 1, 2>;
template class eastl::fixed_hash_multiset<A, 1, 2>;
template class eastl::fixed_hash_multimap<A, A, 1, 2>;

template class eastl::fixed_hash_set<int, 1, 2, true, eastl::hash<int>, eastl::equal_to<int>, true>;
template class eastl::fixed_hash_map<int, int, 1, 2, true, eastl::hash<int>, eastl::equal_to<int>, true>;
template class eastl::fixed_hash_multiset<int, 1, 2, true, eastl::hash<int>, eastl::equal_to<int>, true>;
template class eastl::fixed_hash_multimap<int, int, 1, 2, true, eastl::hash<int>, eastl::equal_to<int>, true>;

template class eastl::fixed_hash_set<A, 1, 2, true, eastl::hash<A>, eastl::equal_to<A>, true>;
template class eastl::fixed_hash_map<A, A, 1, 2, true, eastl::hash<A>, eastl::equal_to<A>, true>;
template class eastl::fixed_hash_multiset<A, 1, 2, true, eastl::hash<A>, eastl::equal_to<A>, true>;
template class eastl::fixed_hash_multimap<A, A, 1, 2, true, eastl::hash<A>, eastl::equal_to<A>, true>;

// Custom allocator
template class eastl::fixed_hash_set<int, 1, 2, true, eastl::hash<int>, eastl::equal_to<int>, false, MallocAllocator>;
template class eastl::fixed_hash_map<int, int, 1, 2, true, eastl::hash<int>, eastl::equal_to<int>, false, MallocAllocator>;
template class eastl::fixed_hash_multiset<int, 1, 2, true, eastl::hash<int>, eastl::equal_to<int>, false, MallocAllocator>;
template class eastl::fixed_hash_multimap<int, int, 1, 2, true, eastl::hash<int>, eastl::equal_to<int>, false, MallocAllocator>;

template class eastl::fixed_hash_set<A, 1, 2, true, eastl::hash<A>, eastl::equal_to<A>, false, MallocAllocator>;
template class eastl::fixed_hash_map<A, A, 1, 2, true, eastl::hash<A>, eastl::equal_to<A>, false, MallocAllocator>;
template class eastl::fixed_hash_multiset<A, 1, 2, true, eastl::hash<A>, eastl::equal_to<A>, false, MallocAllocator>;
template class eastl::fixed_hash_multimap<A, A, 1, 2, true, eastl::hash<A>, eastl::equal_to<A>, false, MallocAllocator>;



template<typename FixedHashMap, int ELEMENT_MAX, int ITERATION_MAX>
int TestFixedHashMapClearBuckets()
{
	int nErrorCount = 0;

	FixedHashMap fixedHashMap;
	const auto nPreClearBucketCount = fixedHashMap.bucket_count();

	for (int j = 0; j < ITERATION_MAX; j++)
	{
		// add elements and ensure container is valid
		for (int i = 0; i < int(nPreClearBucketCount); i++)
			fixedHashMap.emplace(i, i);
		VERIFY(fixedHashMap.validate());
		
		// ensure contents are expected values
		for (int i = 0; i < int(nPreClearBucketCount); i++)
		{
			auto iter = fixedHashMap.find(i);

			VERIFY(iter != fixedHashMap.end());
			VERIFY(iter->second == i);
		}

		// validate container after its cleared its nodes and buckets
		fixedHashMap.clear(true);
		VERIFY(fixedHashMap.validate());
		VERIFY(fixedHashMap.size() == 0);
		VERIFY(fixedHashMap.bucket_count() == nPreClearBucketCount);
	}

	return nErrorCount;
}


EA_DISABLE_VC_WARNING(6262)
int TestFixedHash()
{
	int nErrorCount = 0;

	{ // fixed_hash_map
		{
			// Test version *without* pool overflow.
			typedef eastl::fixed_hash_map<int, int, 100, 100, false> FixedHashMapFalse;
			FixedHashMapFalse fixedHashMap;

			fixedHashMap[0] = 0;
			fixedHashMap.insert(FixedHashMapFalse::value_type(0, 0));

			VERIFY(fixedHashMap.max_size() == 100);
			VERIFY(fixedHashMap.size() == 1);

			fixedHashMap.clear();
			VERIFY(fixedHashMap.size() == 0);

			for(int i = 0; i < 100; i++)
				fixedHashMap.insert(FixedHashMapFalse::value_type(i, i));
			VERIFY(fixedHashMap.size() == 100);

			// Verify that we allocated enough space for exactly N items. 
			// It's possible that due to alignments, there might be room for N + 1.
			FixedHashMapFalse::allocator_type& allocator = fixedHashMap.get_allocator();
			void* pResult = allocator.allocate(sizeof(FixedHashMapFalse::node_type));
			if(pResult)
			{
				pResult = allocator.allocate(sizeof(FixedHashMapFalse::node_type));
				VERIFY(pResult == NULL); 
			}

			fixedHashMap.clear(true);
			VERIFY(fixedHashMap.validate());
			VERIFY(fixedHashMap.size() == 0);
			VERIFY(fixedHashMap.bucket_count() == fixedHashMap.rehash_policy().GetPrevBucketCount(100));
		}

		{
			// Test version *with* pool overflow.
			typedef eastl::fixed_hash_map<int, int, 100, 100, true> FixedHashMapTrue;
			FixedHashMapTrue fixedHashMap;

			fixedHashMap[0] = 0;
			fixedHashMap.insert(FixedHashMapTrue::value_type(0, 0));

			VERIFY(fixedHashMap.max_size() == 100);
			VERIFY(fixedHashMap.size() == 1);

			fixedHashMap.clear();
			VERIFY(fixedHashMap.size() == 0);

			for(int i = 0; i < 100; i++)
				fixedHashMap.insert(FixedHashMapTrue::value_type(i, i));
			VERIFY(fixedHashMap.size() == 100);

			FixedHashMapTrue::allocator_type& allocator = fixedHashMap.get_allocator();
			void* pResult = allocator.allocate(sizeof(FixedHashMapTrue::node_type));
			VERIFY(pResult != NULL);
			allocator.deallocate(pResult, sizeof(FixedHashMapTrue::node_type));

			fixedHashMap.clear(true);
			VERIFY(fixedHashMap.validate());
			VERIFY(fixedHashMap.size() == 0);
			VERIFY(fixedHashMap.bucket_count() == fixedHashMap.rehash_policy().GetPrevBucketCount(100));

			// get_overflow_allocator / set_overflow_allocator
			// This is a weak test which should be improved.
			EASTLAllocatorType a = fixedHashMap.get_allocator().get_overflow_allocator();
			fixedHashMap.get_allocator().set_overflow_allocator(a);
		}

		// Test that fixed_hash_map (with and without overflow enabled) is usable after the node and bucket array has
		// been cleared.
		{
			constexpr const int ITERATION_MAX = 5;
			constexpr const int ELEMENT_MAX = 100;
			constexpr const int ELEMENT_OVERFLOW_MAX = ELEMENT_MAX * 2;

			TestFixedHashMapClearBuckets<eastl::fixed_hash_map<int, int, ELEMENT_MAX, ELEMENT_MAX, false>,      ELEMENT_MAX,          ITERATION_MAX>();
			TestFixedHashMapClearBuckets<eastl::fixed_hash_map<int, int, ELEMENT_MAX, ELEMENT_MAX, true>,       ELEMENT_OVERFLOW_MAX, ITERATION_MAX>();
			TestFixedHashMapClearBuckets<eastl::fixed_hash_multimap<int, int, ELEMENT_MAX, ELEMENT_MAX, false>, ELEMENT_MAX,          ITERATION_MAX>();
			TestFixedHashMapClearBuckets<eastl::fixed_hash_multimap<int, int, ELEMENT_MAX, ELEMENT_MAX, true>,  ELEMENT_OVERFLOW_MAX, ITERATION_MAX>();
		}
		
		{
			// Test fixed_hash_map *with* overflow and ensure the underlying hashtable rehashes.
			typedef eastl::fixed_hash_map<unsigned int, unsigned int, 512, 513, true,  eastl::hash<unsigned int>, eastl::equal_to<unsigned int>, false, MallocAllocator> FixedHashMap;

			FixedHashMap fixedHashMap;
			auto old_bucket_count = fixedHashMap.bucket_count();
			auto old_load_factor = fixedHashMap.load_factor();
						
			for (int i = 0; i < 1000; i++)
				fixedHashMap.insert(i);

			auto new_bucket_count = fixedHashMap.bucket_count();
			auto new_load_factor = fixedHashMap.load_factor();

			VERIFY(new_bucket_count != old_bucket_count);
			VERIFY(new_bucket_count > old_bucket_count);
			VERIFY(new_load_factor != old_load_factor);
			VERIFY(fixedHashMap.get_overflow_allocator().mAllocCountAll != 0);			
		}

		{
			// Test version with overflow and alignment requirements.
			typedef fixed_hash_map<Align64, int, 1, 2, true> FixedHashMapWithAlignment;
			typedef fixed_hash_multimap<Align64, int, 1, 2, true> FixedHashMultiMapWithAlignment;
			typedef fixed_hash_set<Align64, 1, 2, true> FixedHashSetWithAlignment;
			typedef fixed_hash_multiset<Align64, 1, 2, true> FixedHashMultiSetWithAlignment;

			FixedHashMapWithAlignment fhm;
			FixedHashMultiMapWithAlignment fhmm;
			FixedHashSetWithAlignment fhs;
			FixedHashMultiSetWithAlignment fhms;

			Align64 a; a.mX = 1;
			Align64 b; b.mX = 2;
			Align64 c; c.mX = 3;
			Align64 d; d.mX = 4;
			Align64 e; e.mX = 5;

			fhm.insert(a);
			fhm.insert(b);
			fhm.insert(c);
			fhm.insert(d);
			fhm.insert(e);
			for (FixedHashMapWithAlignment::const_iterator it = fhm.begin(); it != fhm.end(); ++it)
			{
				const Align64* ptr = &((*it).first);
				EATEST_VERIFY((uint64_t)ptr % EASTL_ALIGN_OF(Align64) == 0);
			}
			fhmm.insert(a);
			fhmm.insert(b);
			fhmm.insert(c);
			fhmm.insert(d);
			fhmm.insert(e);
			for (FixedHashMultiMapWithAlignment::const_iterator it = fhmm.begin(); it != fhmm.end(); ++it)
			{
				const Align64* ptr = &((*it).first);
				EATEST_VERIFY((uint64_t)ptr % EASTL_ALIGN_OF(Align64) == 0);
			}
			fhs.insert(a);
			fhs.insert(b);
			fhs.insert(c);
			fhs.insert(d);
			fhs.insert(e);
			for (FixedHashSetWithAlignment::const_iterator it = fhs.begin(); it != fhs.end(); ++it)
			{
				const Align64* ptr = &(*it);
				EATEST_VERIFY((uint64_t)ptr % EASTL_ALIGN_OF(Align64) == 0);
			}
			fhms.insert(a);
			fhms.insert(b);
			fhms.insert(c);
			fhms.insert(d);
			fhms.insert(e);
			for (FixedHashMultiSetWithAlignment::const_iterator it = fhms.begin(); it != fhms.end(); ++it)
			{
				const Align64* ptr = &(*it);
				EATEST_VERIFY((uint64_t)ptr % EASTL_ALIGN_OF(Align64) == 0);
			}
		}

		{
			typedef eastl::fixed_hash_map<int, A, 100, 100> FixedHashMap;
			FixedHashMap fixedHashMap;

			fixedHashMap[0] = A();
			fixedHashMap.insert(FixedHashMap::value_type(0, A()));

			VERIFY(fixedHashMap.size() == 1);
		}

		{
			typedef eastl::fixed_hash_map<A, int, 100, 100> FixedHashMap;
			FixedHashMap fixedHashMap;

			fixedHashMap[A()] = 0;
			fixedHashMap.insert(FixedHashMap::value_type(A(), 0));

			VERIFY(fixedHashMap.size() == 1);
		}

		// explicitly instantiate some templated member functions
		{			
			typedef eastl::fixed_hash_map<int, int, 100, 100, true> FixedHashMapTrue;
			FixedHashMapTrue::value_type testValues[] = { eastl::make_pair(0, 0), eastl::make_pair(1,1) };
			FixedHashMapTrue fixedHashMap(testValues, testValues + EAArrayCount(testValues));
			VERIFY(fixedHashMap.size() == 2);
		}
	}


	{ // fixed_hash_multimap
		{
			typedef eastl::fixed_hash_multimap<int, int, 100, 100> FixedHashMultiMap;
			FixedHashMultiMap fixedHashMultiMap;

			fixedHashMultiMap.insert(FixedHashMultiMap::value_type(0, 0));
			fixedHashMultiMap.insert(FixedHashMultiMap::value_type(0, 0));

			VERIFY(fixedHashMultiMap.max_size() == 100);
			VERIFY(fixedHashMultiMap.size() == 2);
		}

		// explicitly instantiate some templated member functions
		{			
			typedef eastl::fixed_hash_multimap<int, int, 100, 100, true> FixedHashMultiMap;
			FixedHashMultiMap::value_type testValues[] = { eastl::make_pair(0, 0), eastl::make_pair(1,1) };
			FixedHashMultiMap fixedHashMultiMap(testValues, testValues + EAArrayCount(testValues));
			VERIFY(fixedHashMultiMap.size() == 2);
		}
	}


	{ // fixed_hash_set
		{
			typedef eastl::fixed_hash_set<int, 100, 100> FixedHashSet;
			FixedHashSet fixedHashSet;

			fixedHashSet.insert(0);
			fixedHashSet.insert(0);
			VERIFY(fixedHashSet.size() == 1);

			fixedHashSet.clear();
			VERIFY(fixedHashSet.size() == 0);

			for(int i = 0; i < 100; i++)
				fixedHashSet.insert(i);

			VERIFY(fixedHashSet.max_size() == 100);
			VERIFY(fixedHashSet.size() == 100);
 
			fixedHashSet.clear(true);
			VERIFY(fixedHashSet.validate());
			VERIFY(fixedHashSet.size() == 0);
			VERIFY(fixedHashSet.bucket_count() == 1);
	   }

		{
			typedef eastl::fixed_hash_set<A, 100, 100> FixedHashSet;
			FixedHashSet fixedHashSet;

			fixedHashSet.insert(A());
			fixedHashSet.insert(A());

			VERIFY(fixedHashSet.max_size() == 100);
			VERIFY(fixedHashSet.size() == 1);
		}

		// explicitly instantiate some templated member functions
		{			
			typedef eastl::fixed_hash_set<A, 100, 100> FixedHashSet;
			FixedHashSet::value_type testValues[] = { 0, 1 };
			FixedHashSet fixedHashSet(testValues, testValues + EAArrayCount(testValues));
			VERIFY(fixedHashSet.size() == 2);
		}
	}


	{ // fixed_hash_multiset
		{
			typedef eastl::fixed_hash_multiset<int, 100, 100> FixedHashMultiSet;
			FixedHashMultiSet fixedHashMultiSet;

			fixedHashMultiSet.insert(0);
			fixedHashMultiSet.insert(0);

			VERIFY(fixedHashMultiSet.size() == 2);
		}
		

		// explicitly instantiate some templated member functions
		{			
			typedef eastl::fixed_hash_multiset<A, 100, 100> FixedHashMultiSet;
			FixedHashMultiSet::value_type testValues[] = { 0, 1 };
			FixedHashMultiSet fixedHashMultiSet(testValues, testValues + EAArrayCount(testValues));
			VERIFY(fixedHashMultiSet.size() == 2);
		}
	}


	{ // Tests of various bucketCount values.
		{
			typedef eastl::fixed_hash_set<int, 1, 2> FixedHashSet;
			FixedHashSet fixedHashSet;

			fixedHashSet.insert(0);

			VERIFY(fixedHashSet.size() == 1);
		}

		{
			typedef eastl::fixed_hash_set<int, 2, 2> FixedHashSet;
			FixedHashSet fixedHashSet;

			fixedHashSet.insert(0);
			fixedHashSet.insert(1);

			VERIFY(fixedHashSet.size() == 2);
		}

		{
			typedef eastl::fixed_hash_set<int, 11, 11> FixedHashSet; // 11 is one of the hashtable prime numbers.
			FixedHashSet fixedHashSet;

			for(int i = 0; i < 11; i++)
				fixedHashSet.insert(i);

			VERIFY(fixedHashSet.size() == 11);
		}


		{
			typedef eastl::fixed_hash_set<int, 11, 11> FixedHashSet; // 11 is one of the hashtable prime numbers.
			FixedHashSet fixedHashSet;

			VERIFY(fixedHashSet.validate());
			VERIFY(fixedHashSet.size() == 0);

			// Clear a newly constructed, already empty container.
			fixedHashSet.clear(true);
			VERIFY(fixedHashSet.validate());
			VERIFY(fixedHashSet.size() == 0);
			VERIFY(fixedHashSet.bucket_count() == 1);

			for(int i = 0; i < 11; i++)
				fixedHashSet.insert(i);
			VERIFY(fixedHashSet.size() == 11);
			VERIFY(fixedHashSet.bucket_count() > 1);

			fixedHashSet.clear(true);
			VERIFY(fixedHashSet.validate());
			VERIFY(fixedHashSet.size() == 0);
			VERIFY(fixedHashSet.bucket_count() == 1);

			for(int i = 0; i < 11; i++)
				fixedHashSet.insert(i);
			VERIFY(fixedHashSet.size() == 11);
		}
	}

	{ // Test of user-reported crash.

		// MemoryAddressToGroupMap is a container used by one team to associate debug 
		// information with memory allocations. A crash due to corruption of the 
		// fixed size node pool was reported on consoles (no crash on PC platform). 
		const eastl_size_t kMemoryAddressMapNodeCount = 500000;

		typedef eastl::fixed_hash_map< 
			const void*,                             // Key
			MemoryEntry,                             // Value
			kMemoryAddressMapNodeCount,              // Node Count
			kMemoryAddressMapNodeCount + 1,          // Bucket Count
			true,                                    // Enable Overflow
			eastl::hash<const void*>,                // Hash
			eastl::equal_to<const void*>,            // Predicate
			false,                                   // Cache Hash Code
			eastl::allocator                         // Allocator
		> MemoryAddressToGroupMap;

		MemoryAddressToGroupMap* pMap = new MemoryAddressToGroupMap;
		EA::UnitTest::Rand       rng(EA::UnitTest::GetRandSeed());

		// We simulate the usage of MemoryAddressToGroupMap via simulated alloc/free actions.
		for(eastl_size_t i = 0; i < kMemoryAddressMapNodeCount * 2; i++)
		{
			void* const p = (void*)(uintptr_t)rng.RandLimit(kMemoryAddressMapNodeCount);

			if(pMap->find(p) == pMap->end())
				(*pMap)[p] = MemoryEntry();
			else
				pMap->erase(p);
		}

		delete pMap;
	}


	{  // Test of bug reported by Dave Wall, May 14, 2008.
		const size_t kNumBuckets = 10;  // Bug only occurred with kNumBuckets == 10 or 11.

		typedef eastl::fixed_hash_map<const InstanceRenderData, uint32_t, kNumBuckets, kNumBuckets + 1, false> Map;

		Map map;
		InstanceRenderData renderData;

		uint32_t count = (uint32_t)kNumBuckets;

		while(count--)
		{
			renderData.mPad[0] = count;
			map.insert(Map::value_type(renderData, count));
		}

	}

	{
		// Test construction of a container with an overflow allocator constructor argument.
		MallocAllocator overflowAllocator;
		void* p = overflowAllocator.allocate(1);

		typedef eastl::fixed_hash_map<int, int, 64, 100, true, eastl::hash<int>, eastl::equal_to<int>, false, MallocAllocator> Container;
		Container c(overflowAllocator);

		for(int i = 0; i < 65; i++)
			c.insert(Container::value_type(i, i));

		VERIFY(c.get_overflow_allocator().mAllocCount == 2); // 1 for above, and 1 for overflowing from 64 to 65.
		overflowAllocator.deallocate(p, 1);
	}


	{
		// C++11 emplace and related functionality
		nErrorCount += TestMapCpp11<eastl::fixed_hash_map<int, TestObject,  2, 7, true> >();  // Exercize a low-capacity fixed-size container.
		nErrorCount += TestMapCpp11<eastl::fixed_hash_map<int, TestObject, 32, 7, true> >();

		nErrorCount += TestMapCpp11NonCopyable<eastl::fixed_hash_map<int, NonCopyable, 2, 7, true>>();

		nErrorCount += TestSetCpp11<eastl::fixed_hash_set<TestObject,  2, 7, true> >();
		nErrorCount += TestSetCpp11<eastl::fixed_hash_set<TestObject, 32, 7, true> >();

		nErrorCount += TestMultimapCpp11<eastl::fixed_hash_multimap<int, TestObject,  2, 7, true> >();
		nErrorCount += TestMultimapCpp11<eastl::fixed_hash_multimap<int, TestObject, 32, 7, true> >();

		nErrorCount += TestMultisetCpp11<eastl::fixed_hash_multiset<TestObject,  2, 7, true> >();
		nErrorCount += TestMultisetCpp11<eastl::fixed_hash_multiset<TestObject, 32, 7, true> >();
	}

	{
		// C++17 try_emplace and related functionality
		nErrorCount += TestMapCpp17<eastl::fixed_hash_map<int, TestObject,  2, 7, true>>();
		nErrorCount += TestMapCpp17<eastl::fixed_hash_map<int, TestObject, 32, 7, true> >();
	}

	{
		// void reserve(size_type nElementCount);

		// test with overflow enabled.
		nErrorCount += HashContainerReserveTest<fixed_hash_set<int, 16>>()();
		nErrorCount += HashContainerReserveTest<fixed_hash_multiset<int, 16>>()();
		nErrorCount += HashContainerReserveTest<fixed_hash_map<int, int, 16>>()();
		nErrorCount += HashContainerReserveTest<fixed_hash_multimap<int, int, 16>>()();

		// API prevents testing fixed size hash container reservation without overflow enabled. 
		//
		// nErrorCount += HashContainerReserveTest<fixed_hash_set<int, 400, 401, false>>()();
		// nErrorCount += HashContainerReserveTest<fixed_hash_multiset<int, 400, 401, false>>()();
		// nErrorCount += HashContainerReserveTest<fixed_hash_map<int, int, 400, 401, false>>()();
		// nErrorCount += HashContainerReserveTest<fixed_hash_multimap<int, int, 9000, 9001, false>>()();
	}

	{
		// initializer_list support.
		// fixed_hash_set(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator = EASTL_FIXED_HASH_SET_DEFAULT_ALLOCATOR)
		// this_type& operator=(std::initializer_list<value_type> ilist);
		// void insert(std::initializer_list<value_type> ilist);
		fixed_hash_set<int, 11> intHashSet = { 12, 13, 14 };
		EATEST_VERIFY(intHashSet.size() == 3);
		EATEST_VERIFY(intHashSet.find(12) != intHashSet.end());
		EATEST_VERIFY(intHashSet.find(13) != intHashSet.end());
		EATEST_VERIFY(intHashSet.find(14) != intHashSet.end());

		intHashSet = { 22, 23, 24 };
		EATEST_VERIFY(intHashSet.size() == 3);
		EATEST_VERIFY(intHashSet.find(22) != intHashSet.end());
		EATEST_VERIFY(intHashSet.find(23) != intHashSet.end());
		EATEST_VERIFY(intHashSet.find(24) != intHashSet.end());

		intHashSet.insert({ 42, 43, 44 });
		EATEST_VERIFY(intHashSet.size() == 6);
		EATEST_VERIFY(intHashSet.find(42) != intHashSet.end());
		EATEST_VERIFY(intHashSet.find(43) != intHashSet.end());
		EATEST_VERIFY(intHashSet.find(44) != intHashSet.end());

		// hash_map(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator = EASTL_FIXED_HASH_SET_DEFAULT_ALLOCATOR)
		// this_type& operator=(std::initializer_list<value_type> ilist);
		// void insert(std::initializer_list<value_type> ilist);
		fixed_hash_map<int, double, 11> intHashMap = { {12,12.0}, {13,13.0}, {14,14.0} };
		EATEST_VERIFY(intHashMap.size() == 3);
		EATEST_VERIFY(intHashMap.find(12) != intHashMap.end());
		EATEST_VERIFY(intHashMap.find(13) != intHashMap.end());
		EATEST_VERIFY(intHashMap.find(14) != intHashMap.end());

		intHashMap = { {22,22.0}, {23,23.0}, {24,24.0} };
		EATEST_VERIFY(intHashMap.size() == 3);
		EATEST_VERIFY(intHashMap.find(22) != intHashMap.end());
		EATEST_VERIFY(intHashMap.find(23) != intHashMap.end());
		EATEST_VERIFY(intHashMap.find(24) != intHashMap.end());

		intHashMap.insert({ {42,42.0}, {43,43.0}, {44,44.0} });
		EATEST_VERIFY(intHashMap.size() == 6);
		EATEST_VERIFY(intHashMap.find(42) != intHashMap.end());
		EATEST_VERIFY(intHashMap.find(43) != intHashMap.end());
		EATEST_VERIFY(intHashMap.find(44) != intHashMap.end());
	}

	{
		constexpr int ELEM_MAX = 10;
		typedef eastl::fixed_hash_map<int, int, ELEM_MAX, ELEM_MAX, false> FixedHashMapFalse;
		FixedHashMapFalse fixedHashMap;
		VERIFY(fixedHashMap.size() == 0);

		for (int i = 0; i < ELEM_MAX; i++)
			fixedHashMap.insert(FixedHashMapFalse::value_type(i, i));

		VERIFY(fixedHashMap.validate());
		VERIFY(fixedHashMap.size() == ELEM_MAX);

		// Verify insert requests of nodes already in the container don't attempt to allocate memory.
		// Because the fixed_hash_map is full any attempt to allocate memory will generate an OOM error.
		{
			auto result = fixedHashMap.insert(FixedHashMapFalse::value_type(0, 0));
			VERIFY(result.second == false);
		}

		{
			auto result = fixedHashMap.insert(fixedHashMap.begin(), FixedHashMapFalse::value_type(0, 0));
			VERIFY(result->first == 0);
			VERIFY(result->second == 0);
		}

		{
			FixedHashMapFalse::value_type value(0, 0);
			auto result = fixedHashMap.insert(eastl::move(value));
			VERIFY(result.second == false);
		}
		{
			FixedHashMapFalse::value_type value(0, 0);
			auto result = fixedHashMap.insert(fixedHashMap.begin(), eastl::move(value));
			VERIFY(result->first == 0);
			VERIFY(result->second == 0);
		}

		{
			FixedHashMapFalse::value_type value(0, 0);
			auto result = fixedHashMap.insert(value);
			VERIFY(result.second == false);
		}

		{
			auto result = fixedHashMap.insert(eastl::make_pair(0, 0));
			VERIFY(result.second == false);
		}

		{
			// OOM, fixed allocator memory is exhausted so it can't create a node for insertation testing
			// auto result = fixedHashMap.emplace(0, 0);  
			// VERIFY(result.second == false);
		}
	}

	return nErrorCount;
}
EA_RESTORE_VC_WARNING()









