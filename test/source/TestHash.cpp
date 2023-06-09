/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include "TestMap.h"
#include "TestSet.h"
#include <EASTL/hash_set.h>
#include <EASTL/hash_map.h>
#include <EASTL/unordered_set.h>
#include <EASTL/unordered_map.h>
#include <EASTL/map.h>
#include <EASTL/string.h>
#include <EASTL/algorithm.h>
#include <EASTL/vector.h>
#include <EASTL/unique_ptr.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <string.h>
EA_RESTORE_ALL_VC_WARNINGS()


using namespace eastl;

namespace eastl
{
	template <> 
	struct hash<Align32>
	{
		size_t operator()(const Align32& a32) const 
			{ return static_cast<size_t>(a32.mX); }
	};

	// extension to hash an eastl::pair
	template <typename T1, typename T2>
	struct hash<pair<T1, T2>>
	{
		size_t operator()(const pair<T1, T2>& c) const
		{
			return static_cast<size_t>(hash<T1>()(c.first) ^ hash<T2>()(c.second));
		}
	};
}

// For regression code below.
class HashRegressionA { public: int x; };
class HashRegressionB { public: int y; };


// For regression code below.
struct Struct {
	char8_t name[128];
};


// For regression code below.
template<class HashType>
struct HashTest
{
	template<typename... Args>
	auto operator()(Args&&... args)
	{
		return eastl::hash<HashType>{}(eastl::forward<Args>(args)...);
	}
};



// What we are doing here is creating a special case of a hashtable where the key compare
// function is not the same as the value operator==. 99% of the time when you create a 
// hashtable the key compare (predicate) is simply key_equal or something else that's
// identical to operator== for the hashtable value type. But for some tests we want
// to exercise the case that these aren't different. A result of this difference is that
// you can lookup an element in a hash table and the returned value is not == to the 
// value you looked up, because it succeeds the key compare but not operator==.
struct HashtableValue
{
	HashtableValue(eastl_size_t d = 0, eastl_size_t e = 0) : mData(d), mExtra(e){}
	void Set(eastl_size_t d, eastl_size_t e = 0) { mData = d; mExtra = e; }

	eastl_size_t mData;
	eastl_size_t mExtra;
};

bool operator==(const HashtableValue& htv1, const HashtableValue& htv2)
{
	return (htv1.mData == htv2.mData) && (htv1.mExtra == htv2.mExtra); // Fully compare the HashTableValue.
}

struct HashtableValuePredicate
{
	bool operator()(const HashtableValue& htv1, const HashtableValue& htv2) const
		{ return (htv1.mData == htv2.mData); } // Compare just the mData portion of HashTableValue.
};

struct HashtableValueHash
{
	size_t operator()(const HashtableValue& htv) const 
		{ return static_cast<size_t>(htv.mData); }
};




// Explicit Template instantiations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::hashtable<int,
                                eastl::pair<const int, int>,
                                eastl::allocator,
                                eastl::use_first<eastl::pair<const int, int>>,
                                eastl::equal_to<int>,
                                eastl::hash<int>,
                                mod_range_hashing,
                                default_ranged_hash,
                                prime_rehash_policy,
                                true, // bCacheHashCode
                                true, // bMutableIterators
                                true  // bUniqueKeys
                                >;
template class eastl::hashtable<int,
								eastl::pair<const int, int>,
								eastl::allocator,
								eastl::use_first<eastl::pair<const int, int>>,
								eastl::equal_to<int>,
								eastl::hash<int>,
								mod_range_hashing,
								default_ranged_hash,
								prime_rehash_policy,
								false, // bCacheHashCode
								true,  // bMutableIterators
								true   // bUniqueKeys
								>;
// TODO(rparolin): known compiler error, we should fix this.
// template class eastl::hashtable<int,
//                                 eastl::pair<const int, int>,
//                                 eastl::allocator,
//                                 eastl::use_first<eastl::pair<const int, int>>,
//                                 eastl::equal_to<int>,
//                                 eastl::hash<int>,
//                                 mod_range_hashing,
//                                 default_ranged_hash,
//                                 prime_rehash_policy,
//                                 false, // bCacheHashCode
//                                 true,  // bMutableIterators
//                                 false  // bUniqueKeys
//                                 >;

// Note these will only compile non-inherited functions.  We provide explicit
// template instantiations for the hashtable base class above to get compiler
// coverage of those inherited hashtable functions.
template class eastl::hash_set<int>;
template class eastl::hash_multiset<int>;
template class eastl::hash_map<int, int>;
template class eastl::hash_multimap<int, int>;
template class eastl::hash_set<Align32>;
template class eastl::hash_multiset<Align32>;
template class eastl::hash_map<Align32, Align32>;
template class eastl::hash_multimap<Align32, Align32>;

// validate static assumptions about hashtable core types
typedef eastl::hash_node<int, false> HashNode1;
typedef eastl::hash_node<int, true> HashNode2;
static_assert(eastl::is_default_constructible<HashNode1>::value, "hash_node static error");
static_assert(eastl::is_default_constructible<HashNode2>::value, "hash_node static error");
static_assert(eastl::is_copy_constructible<HashNode1>::value, "hash_node static error");
static_assert(eastl::is_copy_constructible<HashNode2>::value, "hash_node static error");
static_assert(eastl::is_move_constructible<HashNode1>::value, "hash_node static error");
static_assert(eastl::is_move_constructible<HashNode2>::value, "hash_node static error");

// A custom hash function that has a high number of collisions is used to ensure many keys share the same hash value.
struct colliding_hash
{
	size_t operator()(const int& val) const 
		{ return static_cast<size_t>(val % 3); }
};



int TestHash()
{   
	int nErrorCount = 0;

	{  // Test declarations
		hash_set<int>           hashSet;
		hash_multiset<int>      hashMultiSet;
		hash_map<int, int>      hashMap;
		hash_multimap<int, int> hashMultiMap;

		hash_set<int> hashSet2(hashSet);
		EATEST_VERIFY(hashSet2.size() == hashSet.size());
		EATEST_VERIFY(hashSet2 == hashSet);

		hash_multiset<int> hashMultiSet2(hashMultiSet);
		EATEST_VERIFY(hashMultiSet2.size() == hashMultiSet.size());
		EATEST_VERIFY(hashMultiSet2 == hashMultiSet);

		hash_map<int, int> hashMap2(hashMap);
		EATEST_VERIFY(hashMap2.size() == hashMap.size());
		EATEST_VERIFY(hashMap2 == hashMap);

		hash_multimap<int, int> hashMultiMap2(hashMultiMap);
		EATEST_VERIFY(hashMultiMap2.size() == hashMultiMap.size());
		EATEST_VERIFY(hashMultiMap2 == hashMultiMap);


		// allocator_type& get_allocator();
		// void            set_allocator(const allocator_type& allocator);
		hash_set<int>::allocator_type& allocator = hashSet.get_allocator();
		hashSet.set_allocator(EASTLAllocatorType());
		hashSet.set_allocator(allocator);
		// To do: Try to find something better to test here.


		// const key_equal& key_eq() const;
		// key_equal&       key_eq();
		hash_set<int>       hs;
		const hash_set<int> hsc;

		const hash_set<int>::key_equal& ke = hsc.key_eq();
		hs.key_eq() = ke;


		// const char*     get_name() const;
		// void            set_name(const char* pName);
		#if EASTL_NAME_ENABLED
			hashMap.get_allocator().set_name("test");
			const char* pName = hashMap.get_allocator().get_name();
			EATEST_VERIFY(equal(pName, pName + 5, "test"));
		#endif
	}


	{
		hash_set<int> hashSet;

		// Clear a newly constructed, already empty container.
		hashSet.clear(true);
		EATEST_VERIFY(hashSet.validate());
		EATEST_VERIFY(hashSet.size() == 0);
		EATEST_VERIFY(hashSet.bucket_count() == 1);

		for(int i = 0; i < 100; ++i)
			hashSet.insert(i);
		EATEST_VERIFY(hashSet.validate());
		EATEST_VERIFY(hashSet.size() == 100);

		hashSet.clear(true);
		EATEST_VERIFY(hashSet.validate());
		EATEST_VERIFY(hashSet.size() == 0);
		EATEST_VERIFY(hashSet.bucket_count() == 1);

		for(int i = 0; i < 100; ++i)
			hashSet.insert(i);
		EATEST_VERIFY(hashSet.validate());
		EATEST_VERIFY(hashSet.size() == 100);

		hashSet.clear(true);
		EATEST_VERIFY(hashSet.validate());
		EATEST_VERIFY(hashSet.size() == 0);
		EATEST_VERIFY(hashSet.bucket_count() == 1);
	}


	{   // Test hash_set

		// size_type          size() const
		// bool               empty() const
		// insert_return_type insert(const value_type& value);
		// insert_return_type insert(const value_type& value, hash_code_t c, node_type* pNodeNew = NULL);
		// iterator           insert(const_iterator, const value_type& value);
		// iterator           find(const key_type& k);
		// const_iterator     find(const key_type& k) const;
		// size_type          count(const key_type& k) const;

		typedef hash_set<int> HashSetInt;

		HashSetInt hashSet;
		const HashSetInt::size_type kCount = 10000;

		EATEST_VERIFY(hashSet.empty());
		EATEST_VERIFY(hashSet.size() == 0);
		EATEST_VERIFY(hashSet.count(0) == 0);

		for(int i = 0; i < (int)kCount; i++)
			hashSet.insert(i);

		EATEST_VERIFY(!hashSet.empty());
		EATEST_VERIFY(hashSet.size() == kCount);
		EATEST_VERIFY(hashSet.count(0) == 1);

		for(HashSetInt::iterator it = hashSet.begin(); it != hashSet.end(); ++it)
		{
			int value = *it;
			EATEST_VERIFY(value < (int)kCount);
		}

		for(int i = 0; i < (int)kCount * 2; i++)
		{
			HashSetInt::iterator it = hashSet.find(i);

			if(i < (int)kCount)
				EATEST_VERIFY(it != hashSet.end());
			else
				EATEST_VERIFY(it == hashSet.end());
		}

		// insert_return_type insert(const value_type& value, hash_code_t c, node_type* pNodeNew = NULL);
		HashSetInt::node_type* pNode = hashSet.allocate_uninitialized_node();
		HashSetInt::insert_return_type r = hashSet.insert(eastl::hash<int>()(999999), pNode, 999999);
		EATEST_VERIFY(r.second == true);
		pNode = hashSet.allocate_uninitialized_node();
		r = hashSet.insert(eastl::hash<int>()(999999), pNode, 999999);
		EATEST_VERIFY(r.second == false);
		hashSet.free_uninitialized_node(pNode);
		hashSet.erase(999999);


		// iterator       begin();
		// const_iterator begin() const;
		// iterator       end();
		// const_iterator end() const;

		int* const pIntArray = new int[kCount];
		memset(pIntArray, 0, kCount * sizeof(int)); // We want to make sure each element is present only once.
		int nCount = 0;

		for(HashSetInt::iterator it = hashSet.begin(); it != hashSet.end(); ++it, ++nCount)
		{
			int i = *it;

			EATEST_VERIFY((i >= 0) && (i < (int)kCount) && (pIntArray[i] == 0));
			pIntArray[i] = 1;
		}

		EATEST_VERIFY(nCount == (int)kCount);
		delete[] pIntArray;
	}


	{
		// size_type bucket_count() const
		// size_type bucket_size(size_type n) const
		// float load_factor() const
		// float get_max_load_factor() const;
		// void  set_max_load_factor(float fMaxLoadFactor);
		// void rehash(size_type n);
		// const RehashPolicy& rehash_policy() const
		// void  rehash_policy(const RehashPolicy& rehashPolicy);

		typedef hash_set<int> HashSetInt;

		HashSetInt hashSet;

		float fLoadFactor = hashSet.load_factor();
		EATEST_VERIFY(fLoadFactor == 0.f);

		hashSet.set_max_load_factor(65536.f * 512.f);
		float fMaxLoadFactor = hashSet.get_max_load_factor();
		EATEST_VERIFY(fMaxLoadFactor == (65536.f * 512.f));

		hashSet.rehash(20);
		HashSetInt::size_type n = hashSet.bucket_count();
		EATEST_VERIFY((n >= 20) && (n < 25));
		
		for(int i = 0; i < 100000; i++)
			hashSet.insert(i); // This also tests for high loading.

		HashSetInt::size_type n2 = hashSet.bucket_count();
		EATEST_VERIFY(n2 == n); // Verify no rehashing has occured, due to our high load factor.

		n = hashSet.bucket_size(0);
		EATEST_VERIFY(n >= ((hashSet.size() / hashSet.bucket_count()) / 2)); // It will be some high value. We divide by 2 to give it some slop.
		EATEST_VERIFY(hashSet.validate());

		hash_set<int>::rehash_policy_type rp = hashSet.rehash_policy();
		rp.mfGrowthFactor = 1.5f;
		hashSet.rehash_policy(rp);
		EATEST_VERIFY(hashSet.validate());


		// local_iterator       begin(size_type n);
		// local_iterator       end(size_type n);
		// const_local_iterator begin(size_type n) const;
		// const_local_iterator end(size_type n) const;

		HashSetInt::size_type b = hashSet.bucket_count() - 1;
		hash<int> IntHash;
		for(HashSetInt::const_local_iterator cli = hashSet.begin(b); cli != hashSet.end(b); ++cli)
		{
			int v = *cli;
			EATEST_VERIFY((IntHash(v) % hashSet.bucket_count()) == b);
		}


		// clear();

		hashSet.clear();
		EATEST_VERIFY(hashSet.validate());
		EATEST_VERIFY(hashSet.empty());
		EATEST_VERIFY(hashSet.size() == 0);
		EATEST_VERIFY(hashSet.count(0) == 0);

		hashSet.clear(true);
		EATEST_VERIFY(hashSet.validate());
		EATEST_VERIFY(hashSet.bucket_count() == 1);
	}


	{
		// void reserve(size_type nElementCount);
		nErrorCount += HashContainerReserveTest<hash_set<int>>()();
		nErrorCount += HashContainerReserveTest<hash_multiset<int>>()();
		nErrorCount += HashContainerReserveTest<hash_map<int, int>>()();
		nErrorCount += HashContainerReserveTest<hash_multimap<int, int>>()();
	}


	{   // Test hash_set with cached hash code.

		// insert_return_type insert(const value_type& value) ;
		// iterator       find(const key_type& k);
		// const_iterator find(const key_type& k) const;

		typedef hash_set<int, hash<int>, equal_to<int>, EASTLAllocatorType, true> HashSetIntC;

		HashSetIntC hashSet;
		const int kCount = 10000;

		for(int i = 0; i < kCount; i++)
			hashSet.insert(i);

		for(HashSetIntC::iterator it = hashSet.begin(); it != hashSet.end(); ++it)
		{
			int value = *it;
			EATEST_VERIFY(value < kCount);
		}

		for(int i = 0; i < kCount * 2; i++)
		{
			HashSetIntC::iterator it = hashSet.find(i);
			if(i < kCount)
				EATEST_VERIFY(it != hashSet.end());
			else
				EATEST_VERIFY(it == hashSet.end());
		}
	}

	{
		// ENABLE_IF_HASHCODE_U32(HashCodeT, iterator)       find_by_hash(HashCodeT c)
		// ENABLE_IF_HASHCODE_U32(HashCodeT, const_iterator) find_by_hash(HashCodeT c) const
		{
			// NOTE(rparolin):
			// these overloads of find_by_hash contains a static assert that forces a compiler error in the event it is
			// used with a hashtable configured to not cache the hash value in the node.
		}

		// iterator                                          find_by_hash(const key_type& k, hash_code_t c)
		// const_iterator                                    find_by_hash(const key_type& k, hash_code_t c) const
		#ifdef EA_COMPILER_CPP14_ENABLED 
		{
			auto FindByHashTest = [&nErrorCount](auto& hashSet)
			{
				const int kCount = 10000;
				for(int i = 0; i < kCount; i++)
					hashSet.insert(i);

				for(int i = 0; i < kCount * 2; i++)
				{
					auto it = hashSet.find_by_hash(i, i);

					if(i < kCount)
						EATEST_VERIFY(it != hashSet.end());
					else
						EATEST_VERIFY(it == hashSet.end());
				}
			};

			{
				typedef hash_set<int, hash<int>, equal_to<int>, EASTLAllocatorType, true> HashSetIntC;
				HashSetIntC hashSetC;
				FindByHashTest(hashSetC);

				typedef hash_set<int, hash<int>, equal_to<int>, EASTLAllocatorType, false> HashSetInt;
				HashSetInt hashSet;
				FindByHashTest(hashSet);
			}
		}
		#endif
	}


	{
		// hash_set(const allocator_type& allocator);
		// hashtable& operator=(const this_type& x);
		// bool validate() const;

		hash_set<int> hashSet1(EASTLAllocatorType("hash_set name"));
		hash_set<int> hashSet2(hashSet1);

		for(int i = 0; i < 10; i++)
		{
			hashSet1.insert(i);
			hashSet2.insert(i);
		}

		hashSet1 = hashSet2;

		EATEST_VERIFY(hashSet1.validate());
		EATEST_VERIFY(hashSet2.validate());
	}


	{
		// hash_set(size_type nBucketCount, const Hash& hashFunction = Hash(), const Predicate& predicate = Predicate(), const allocator_type& allocator);
		// hashtable(const hashtable& x);
		// hashtable& operator=(const this_type& x);
		// void swap(this_type& x);
		// bool validate() const;
		{
			hash_set<int> hashSet3(0);
			hash_set<int> hashSet4(1);
			hash_set<int> hashSet5(2);
			hash_set<int> hashSet6(3);
			hash_set<int> hashSet7(4);

			hashSet4 = hashSet3;
			hashSet6 = hashSet5;
			hashSet3 = hashSet7;

			for(int i = 0; i < 10; i++)
			{
				hashSet3.insert(i);
				hashSet4.insert(i);
				hashSet5.insert(i);
				hashSet6.insert(i);
				hashSet7.insert(i);
			}

			hashSet4 = hashSet3;
			hashSet6 = hashSet5;
			hashSet3 = hashSet7;

			EATEST_VERIFY(hashSet3.validate());
			EATEST_VERIFY(hashSet4.validate());
			EATEST_VERIFY(hashSet5.validate());
			EATEST_VERIFY(hashSet6.validate());
			EATEST_VERIFY(hashSet7.validate());

			swap(hashSet4, hashSet3);
			swap(hashSet6, hashSet5);
			swap(hashSet3, hashSet7);

			EATEST_VERIFY(hashSet3.validate());
			EATEST_VERIFY(hashSet4.validate());
			EATEST_VERIFY(hashSet5.validate());
			EATEST_VERIFY(hashSet6.validate());
			EATEST_VERIFY(hashSet7.validate());

			hash_set<int> hashSet8(hashSet6);
			hash_set<int> hashSet9(hashSet7);
			hash_set<int> hashSet10(hashSet8);

			EATEST_VERIFY(hashSet8.validate());
			EATEST_VERIFY(hashSet9.validate());
			EATEST_VERIFY(hashSet10.validate());
		}
		
		// test hashtable::swap using different allocator instances
		{
			typedef hash_set<int, eastl::hash<int>, eastl::equal_to<int>, InstanceAllocator> HS;
			HS hashSet1(InstanceAllocator("hash_set1 name", 111));
			HS hashSet2(InstanceAllocator("hash_set2 name", 222));

			for(int i = 0; i < 10; i++)
			{
				hashSet1.insert(i);
				hashSet2.insert(i+10);
			}

			hashSet2.swap(hashSet1);

			EATEST_VERIFY(hashSet1.validate());
			EATEST_VERIFY(hashSet2.validate());

			EATEST_VERIFY(hashSet1.get_allocator().mInstanceId == 222);
			EATEST_VERIFY(hashSet2.get_allocator().mInstanceId == 111);

			EATEST_VERIFY(eastl::all_of(eastl::begin(hashSet2), eastl::end(hashSet2), [](int i) { return i < 10; }));
			EATEST_VERIFY(eastl::all_of(eastl::begin(hashSet1), eastl::end(hashSet1), [](int i) { return i >= 10; }));
		}
	}


	{
		// hash_set(InputIterator first, InputIterator last, size_type nBucketCount = 8, const Hash& hashFunction = Hash(), const Predicate& predicate = Predicate(), const allocator_type& allocator);
		// bool validate() const;

		vector<int> intArray;
		for(int i = 0; i < 1000; i++)
			intArray.push_back(i);

		hash_set<int> hashSet1(intArray.begin(), intArray.end(), 0);
		hash_set<int> hashSet2(intArray.begin(), intArray.end(), 1);
		hash_set<int> hashSet3(intArray.begin(), intArray.end(), 2);
		hash_set<int> hashSet4(intArray.begin(), intArray.end(), 3);

		EATEST_VERIFY(hashSet1.validate());
		EATEST_VERIFY(hashSet2.validate());
		EATEST_VERIFY(hashSet3.validate());
		EATEST_VERIFY(hashSet4.validate());


		// bool validate_iterator(const_iterator i) const;
		hash_set<int>::iterator it;
		int result = hashSet1.validate_iterator(it);
		EATEST_VERIFY(result == isf_none);

		it = hashSet1.begin();
		result = hashSet2.validate_iterator(it);
		EATEST_VERIFY(result == isf_none);
		result = hashSet1.validate_iterator(it);
		EATEST_VERIFY(result == (isf_valid | isf_current | isf_can_dereference));

		it = hashSet1.end();
		result = hashSet1.validate_iterator(it);
		EATEST_VERIFY(result == (isf_valid | isf_current));


		// void reset_lose_memory();
		hashSet1.reset_lose_memory();
		hashSet1 = hashSet2;

		EATEST_VERIFY(hashSet1.validate());
		EATEST_VERIFY(hashSet2.validate());

		hashSet3.reset_lose_memory();
		hashSet4 = hashSet3;

		EATEST_VERIFY(hashSet3.validate());
		EATEST_VERIFY(hashSet4.validate());

		hashSet2.reset_lose_memory();
		hashSet3.reset_lose_memory();
		swap(hashSet2, hashSet3);

		EATEST_VERIFY(hashSet3.validate());
		EATEST_VERIFY(hashSet4.validate());

		hashSet2 = hashSet3;
		EATEST_VERIFY(hashSet2.validate());
	}


	{
		// void insert(InputIterator first, InputIterator last);
		vector<int> intArray1;
		vector<int> intArray2;

		for(int i = 0; i < 1000; i++)
		{
			intArray1.push_back(i + 0);
			intArray2.push_back(i + 500);
		}

		hash_set<int> hashSet1(intArray1.begin(), intArray1.end());
		hashSet1.insert(intArray2.begin(), intArray2.end());
		EATEST_VERIFY(hashSet1.validate());

		hash_set<int> hashSet2;
		hashSet2.insert(intArray1.begin(), intArray1.end());
		hashSet2.insert(intArray2.begin(), intArray2.end());
		EATEST_VERIFY(hashSet2.validate());

		EATEST_VERIFY(hashSet1 == hashSet2);


		// insert_return_type insert(const_iterator, const value_type& value)
		for(int j = 0; j < 1000; j++)
			hashSet1.insert(hashSet1.begin(), j);

		insert_iterator< hash_set<int> > ii(hashSet1, hashSet1.begin());
		for(int j = 0; j < 1000; j++)
			*ii++ = j;
	}


	{
		// C++11 emplace and related functionality
		nErrorCount += TestMapCpp11<eastl::hash_map<int, TestObject>>();
		nErrorCount += TestMapCpp11<eastl::unordered_map<int, TestObject>>();

		nErrorCount += TestSetCpp11<eastl::hash_set<TestObject>>();
		nErrorCount += TestSetCpp11<eastl::unordered_set<TestObject>>();

		nErrorCount += TestMultimapCpp11<eastl::hash_multimap<int, TestObject>>();
		nErrorCount += TestMultimapCpp11<eastl::unordered_multimap<int, TestObject>>();

		nErrorCount += TestMultisetCpp11<eastl::hash_multiset<TestObject>>();
		nErrorCount += TestMultisetCpp11<eastl::unordered_multiset<TestObject>>();

		nErrorCount += TestMapCpp11NonCopyable<eastl::hash_map<int, NonCopyable>>();
		nErrorCount += TestMapCpp11NonCopyable<eastl::unordered_map<int, NonCopyable>>();
	}

	{
		// C++17 try_emplace and related functionality
		nErrorCount += TestMapCpp17<eastl::hash_map<int, TestObject>>();
		nErrorCount += TestMapCpp17<eastl::unordered_map<int, TestObject>>();
	}


	{
		// initializer_list support.
		// hash_set(std::initializer_list<value_type> ilist, size_type nBucketCount = 0, const Hash& hashFunction = Hash(), 
		//            const Predicate& predicate = Predicate(), const allocator_type& allocator = EASTL_HASH_SET_DEFAULT_ALLOCATOR)
		// this_type& operator=(std::initializer_list<value_type> ilist);
		// void insert(std::initializer_list<value_type> ilist);
		hash_set<int> intHashSet = { 12, 13, 14 };
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
	}

	{
		// eastl::pair<iterator, iterator>             equal_range(const key_type& k);
		// eastl::pair<const_iterator, const_iterator> equal_range(const key_type& k) const;
		// const_iterator  erase(const_iterator, const_iterator);
		// size_type       erase(const key_type&);
		// To do.
	}


	{ // hash_set erase_if
		hash_set<int> m = {0, 1, 2, 3, 4};
		auto numErased = eastl::erase_if(m, [](auto i) { return i % 2 == 0; });
		VERIFY((m == hash_set<int>{1, 3}));
	    VERIFY(numErased == 3);
	}

	{ // hash_multiset erase_if
		hash_multiset<int> m = {0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 4};
		auto numErased = eastl::erase_if(m, [](auto i) { return i % 2 == 0; });
		VERIFY((m == hash_multiset<int>{1, 1, 1, 3}));
	    VERIFY(numErased == 12);
	}






	{   // Test hash_map

		// insert_return_type insert(const value_type& value);
		// insert_return_type insert(const key_type& key);
		// iterator       find(const key_type& k);
		// const_iterator find(const key_type& k) const;

		typedef hash_map<int, int> HashMapIntInt;
		HashMapIntInt hashMap;
		const int kCount = 10000;

		for(int i = 0; i < kCount; i++)
		{
			HashMapIntInt::value_type vt(i, i);
			hashMap.insert(vt);
		}

		const HashMapIntInt const_hashMap = hashMap; // creating a const version to test for const correctness

		for(auto& e : hashMap)
		{
			int k = e.first;
			int v = e.second;
			EATEST_VERIFY(k < kCount);
			EATEST_VERIFY(v == k);
			EATEST_VERIFY(hashMap.at(k) == k);
			EATEST_VERIFY(const_hashMap.at(k) == k);
			hashMap.at(k) = k << 4;
		}

		for(auto& e : hashMap)
		{
			int k = e.first;
			int v = e.second;
			EATEST_VERIFY(k < kCount);
			EATEST_VERIFY(v == (k << 4));
		}

		for(int i = 0; i < kCount * 2; i++)
		{
			HashMapIntInt::iterator it = hashMap.find(i);

			if(i < kCount)
			{
				EATEST_VERIFY(it != hashMap.end());

				int k = (*it).first;
				int v = (*it).second;
				EATEST_VERIFY(v == (k << 4));
			}
			else
				EATEST_VERIFY(it == hashMap.end());
		}

		for(int i = 0; i < kCount; i++)
		{
			int v = hashMap.at(i);
			EATEST_VERIFY(v == (i << 4));
		}

		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
				hashMap.at(kCount);
				EASTL_ASSERT_MSG(false, "at accessor did not throw out_of_range exception");
			}
			catch(const std::out_of_range) { }
			catch(const std::exception& e)
			{
				string e_msg(e.what());
				string msg = "wrong exception with message \"" + e_msg + "\" thrown";
				EASTL_ASSERT_MSG(false, msg.c_str());
			}
		#endif
		HashMapIntInt::insert_return_type result = hashMap.insert(88888);
		EATEST_VERIFY(result.second == true);
		result = hashMap.insert(88888);
		EATEST_VERIFY(result.second == false);
		result.first->second = 0;

		// const_iterator erase(const_iterator);
		size_t nExpectedSize = hashMap.size();

		HashMapIntInt::iterator it50 = hashMap.find(50);
		EATEST_VERIFY(it50 != hashMap.end());

		HashMapIntInt::iterator itNext = hashMap.erase(it50);
		nExpectedSize--;
		EATEST_VERIFY(itNext != hashMap.end()); // Strictly speaking, this isn't guaranteed to be so. But statistically it is very likely. We'll fix this if it becomes a problem.
		EATEST_VERIFY(hashMap.size() == nExpectedSize);

		HashMapIntInt::size_type n = hashMap.erase(10);
		nExpectedSize--;
		EATEST_VERIFY(n == 1);
		EATEST_VERIFY(hashMap.size() == nExpectedSize);

		HashMapIntInt::iterator it60 = hashMap.find(60);
		EATEST_VERIFY(itNext != hashMap.end());

		HashMapIntInt::iterator it60Incremented(it60);
		for(int i = 0; (i < 5) && (it60Incremented != hashMap.end()); ++i)
		{
			++it60Incremented;
			--nExpectedSize;
		}

		hashMap.erase(it60, it60Incremented);
		EATEST_VERIFY(hashMap.size() == nExpectedSize);


		// insert_return_type insert(const value_type& value, hash_code_t c, node_type* pNodeNew = NULL);
		HashMapIntInt::node_type* pNode = hashMap.allocate_uninitialized_node();
		HashMapIntInt::insert_return_type r = hashMap.insert(eastl::hash<int>()(999999), pNode, HashMapIntInt::value_type(999999, 999999));
		EATEST_VERIFY(r.second == true);
		pNode = hashMap.allocate_uninitialized_node();
		r = hashMap.insert(eastl::hash<int>()(999999), pNode, HashMapIntInt::value_type(999999, 999999));
		EATEST_VERIFY(r.second == false);
		hashMap.free_uninitialized_node(pNode);
		hashMap.erase(999999);


		// mapped_type& operator[](const key_type& key)
		// hash_map is unique among the map/set containers in having this function.
		hashMap.clear();

		int x = hashMap[0]; // A default-constructed int (i.e. 0) should be returned.
		EATEST_VERIFY(x == 0); 
   
		hashMap[1] = 1;
		x = hashMap[1];
		EATEST_VERIFY(x == 1);     // Verify that the value we assigned is returned and a default-constructed value is not returned.
	 
		hashMap[0] = 10;    // Overwrite our previous 0 with 10.
		hashMap[1] = 11;
		x = hashMap[0];
		EATEST_VERIFY(x == 10);    // Verify the value is as expected.
		x = hashMap[1];
		EATEST_VERIFY(x == 11);
	}


	{   // Test hash_map

		// Aligned objects should be CustomAllocator instead of the default, because the 
		// EASTL default might be unable to do aligned allocations, but CustomAllocator always can.
		hash_map<Align32, int, eastl::hash<Align32>, eastl::equal_to<Align32>, CustomAllocator> hashMap;
		const int kCount = 10000;

		for(int i = 0; i < kCount; i++)
		{
			Align32 a32(i); // GCC 2.x doesn't like the Align32 object being created in the ctor below.
			hash_map<Align32, int>::value_type vt(a32, i);
			hashMap.insert(vt);
		}

		for(hash_map<Align32, int>::iterator it = hashMap.begin(); it != hashMap.end(); ++it)
		{
			const Align32& k = (*it).first;
			int            v = (*it).second;
			EATEST_VERIFY(k.mX < 10000);
			EATEST_VERIFY(v == k.mX);
		}

		for(int i = 0; i < kCount * 2; i++)
		{
			hash_map<Align32, int>::iterator it = hashMap.find(Align32(i));

			if(i < kCount)
			{
				EATEST_VERIFY(it != hashMap.end());

				const Align32& k = (*it).first;
				int            v = (*it).second;
				EATEST_VERIFY(v == k.mX);
			}
			else
				EATEST_VERIFY(it == hashMap.end());
		}
	}

	{ // hash_map erase_if
		hash_map<int, int> m = {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}};
		auto numErased = eastl::erase_if(m, [](auto p) { return p.first % 2 == 0; });
		VERIFY((m == hash_map<int, int>{{1, 1}, {3, 3}}));
	    VERIFY(numErased == 3);
	}

	{ // hash_multimap erase_if
		hash_multimap<int, int> m = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {1, 1}, {2, 2},
		                             {2, 2}, {2, 2}, {2, 2}, {3, 3}, {3, 3}, {4, 4}};
		auto numErased = eastl::erase_if(m, [](auto p) { return p.first % 2 == 0; });
		VERIFY((m == hash_multimap<int, int>{{1, 1}, {3, 3}, {3, 3}}));
	    VERIFY(numErased == 9);
	}



	{   
		// template <typename U, typename UHash, typename BinaryPredicate>
		// iterator find_as(const U& u, UHash uhash, BinaryPredicate predicate);
		// template <typename U, typename UHash, typename BinaryPredicate>
		// const_iterator find_as(const U& u, UHash uhash, BinaryPredicate predicate) const;
		// template <typename U>
		// iterator find_as(const U& u);
		// template <typename U>
		// const_iterator find_as(const U& u) const;

		typedef hash_set<string> HashSetString;

		HashSetString hashSet;
		const int kCount = 100;

		for(int i = 0; i < kCount; i++)
		{
			string::CtorSprintf cs; // GCC 2.x doesn't like this value being created in the ctor below.
			string s(cs, "%d", i);
			hashSet.insert(s);
		}

		for(int i = 0; i < kCount * 2; i++)
		{
			char pString[32];
			sprintf(pString, "%d", i);

			HashSetString::iterator it = hashSet.find_as(pString);
			if(i < kCount)
				EATEST_VERIFY(it != hashSet.end());
			else
				EATEST_VERIFY(it == hashSet.end());

			it = hashSet.find_as(pString, hash<const char*>(), equal_to<>());
			if(i < kCount)
				EATEST_VERIFY(it != hashSet.end());
			else
				EATEST_VERIFY(it == hashSet.end());

			string::CtorSprintf cs;
		    string s(cs, "%d", i);

			it = hashSet.find_as(s);
		    if (i < kCount)
			    EATEST_VERIFY(it != hashSet.end());
		    else
			    EATEST_VERIFY(it == hashSet.end());
		}
	}


	{
		// Test const containers.
		const hash_set<int> constHashSet;

		hash_set<int>::const_iterator i = constHashSet.begin();
		hash_set<int>::const_iterator i3 = i;
		hash_set<int>::iterator i2;
		i3 = i2;

		EATEST_VERIFY(i3 == i2);

		//const std::tr1::unordered_set<int> constUSet;
		//std::tr1::unordered_set<int>::const_iterator i = constUSet.begin();
		//*i = 0;
	}

	{
		// global operator ==, !=
		EASTLTest_Rand rng(EA::UnitTest::GetRandSeed());
		const eastl_size_t kIterationCount = 100;
		const eastl_size_t kDataRange = 50;

		{
			typedef hash_set<HashtableValue, HashtableValueHash, HashtableValuePredicate> HashSet;
			HashtableValue value;

			HashSet h1;
			HashSet h2;
			EATEST_VERIFY(h1 == h2);

			for(eastl_size_t i = 0; i < kIterationCount; i++)
			{
				value.mData = rng.RandLimit(kDataRange);
				h1.insert(value);  // Leave value.mExtra as 0.
			}

			EATEST_VERIFY(h1 != h2);
			h2 = h1;
			EATEST_VERIFY(h1 == h2);

			// Test the case of the containers being the same size but having a single different value, despite that it's key compare yields equal.
			HashSet h2Saved(h2);
			HashSet::iterator it = h2.find(value);
			HashtableValue valueModified(value.mData, 1);
			h2.erase(it);
			h2.insert(valueModified);
			EATEST_VERIFY(h1 != h2);
			h2 = h2Saved;

			// Test the case of the containers being the same size but having a single different key.
			h2Saved = h2;
			h2.erase(h2.find(value));
			h2.insert(kDataRange); // Insert something that could not have been in h2.
			EATEST_VERIFY(h1 != h2);
			h2 = h2Saved;

			h1.erase(h1.find(value)); // Erase from h1 whatever the last value was.
			EATEST_VERIFY(h1 != h2);
		}

		{
			typedef hash_multiset<HashtableValue, HashtableValueHash, HashtableValuePredicate> HashSet;
			HashtableValue value;

			HashSet h1;
			HashSet h2;
			EATEST_VERIFY(h1 == h2);

			for(eastl_size_t i = 0; i < kIterationCount; i++)
			{
				value.mData = rng.RandLimit(kDataRange);
				h1.insert(value);  // Leave value.mExtra as 0.
			}

			EATEST_VERIFY(h1 != h2);
			h2 = h1;
			EATEST_VERIFY(h1 == h2);

			// Test the case of the containers being the same size but having a single different value, despite that it's key compare yields equal.
			HashSet h2Saved(h2);
			HashSet::iterator it = h2.find(value);
			HashtableValue valueModified(value.mData, 1);
			h2.erase(it);
			h2.insert(valueModified);
			EATEST_VERIFY(h1 != h2);
			h2 = h2Saved;

			// Test the case of the containers being the same size but having a single different key.
			h2Saved = h2;
			h2.erase(h2.find(value));
			h2.insert(kDataRange); // Insert something that could not have been in h2.
			EATEST_VERIFY(h1 != h2);
			h2 = h2Saved;

			h1.erase(h1.find(value)); // Erase from h1 whatever the last value was.
			EATEST_VERIFY(h1 != h2);
		}

		{
			// For simplicity we duplicate the HashtableValue::mData member as the hash map key.
			typedef hash_map<eastl_size_t, HashtableValue, HashtableValueHash, HashtableValuePredicate> HashMap;
			HashtableValue value;

			HashMap h1;
			HashMap h2;
			EATEST_VERIFY(h1 == h2);

			for(eastl_size_t i = 0; i < kIterationCount; i++)
			{
				value.mData = rng.RandLimit(kDataRange);
				h1.insert(HashMap::value_type(value.mData, value));  // Leave value.mExtra as 0.
			}

			EATEST_VERIFY(h1 != h2);
			h2 = h1;
			EATEST_VERIFY(h1 == h2);

			// Test the case of the containers being the same size but having a single different value, despite that it's key compare yields equal.
			HashMap h2Saved(h2);
			HashMap::iterator it = h2.find(value.mData); // We are using value.mData as the key as well, so we can do a find via it.
			HashtableValue valueModified(value.mData, 1);
			h2.erase(it);
			h2.insert(HashMap::value_type(valueModified.mData, valueModified));
			EATEST_VERIFY(h1 != h2);
			h2 = h2Saved;

			// Test the case of the containers being the same size but having a single different key.
			h2Saved = h2;
			h2.erase(h2.find(value.mData));
			h2.insert(HashMap::value_type(kDataRange, HashtableValue(kDataRange))); // Insert something that could not have been in h2.
			EATEST_VERIFY(h1 != h2);
			h2 = h2Saved;

			h1.erase(h1.find(value.mData)); // Erase from h1 whatever the last value was.
			EATEST_VERIFY(h1 != h2);
		}

		{
			// For simplicity we duplicate the HashtableValue::mData member as the hash map key.
			typedef hash_multimap<eastl_size_t, HashtableValue, HashtableValueHash, HashtableValuePredicate> HashMap;
			HashtableValue value;

			HashMap h1;
			HashMap h2;
			EATEST_VERIFY(h1 == h2);

			for(eastl_size_t i = 0; i < kIterationCount; i++)
			{
				value.mData = rng.RandLimit(kDataRange);
				h1.insert(HashMap::value_type(value.mData, value));  // Leave value.mExtra as 0.
			}

			EATEST_VERIFY(h1 != h2);
			h2 = h1;
			EATEST_VERIFY(h1 == h2);

			// Test the case of the containers being the same size but having a single different value, despite that it's key compare yields equal.
			HashMap h2Saved(h2);
			HashMap::iterator it = h2.find(value.mData); // We are using value.mData as the key as well, so we can do a find via it.
			HashtableValue valueModified(value.mData, 1);
			h2.erase(it);
			h2.insert(HashMap::value_type(valueModified.mData, valueModified));
			EATEST_VERIFY(h1 != h2);
			h2 = h2Saved;

			// Test the case of the containers being the same size but having a single different key.
			h2Saved = h2;
			h2.erase(h2.find(value.mData));
			h2.insert(HashMap::value_type(kDataRange, HashtableValue(kDataRange))); // Insert something that could not have been in h2.
			EATEST_VERIFY(h1 != h2);
			h2 = h2Saved;

			h1.erase(h1.find(value.mData)); // Erase from h1 whatever the last value was.
			EATEST_VERIFY(h1 != h2);
		}
	}

	{
		typedef eastl::hash_multiset<int> HashMultisetInt;

		HashMultisetInt hashMultiSet;

		// insert_return_type insert(const value_type& value, hash_code_t c, node_type* pNodeNew = NULL);
		HashMultisetInt::node_type* pNode = hashMultiSet.allocate_uninitialized_node();
		HashMultisetInt::iterator it1 = hashMultiSet.insert(eastl::hash<int>()(999999), pNode, 999999);
		EATEST_VERIFY(it1 != hashMultiSet.end());
		pNode = hashMultiSet.allocate_uninitialized_node();
		HashMultisetInt::iterator it2 = hashMultiSet.insert(eastl::hash<int>()(999999), pNode, 999999);
		EATEST_VERIFY(it2 != hashMultiSet.end() && it2 != it1);
	}

	{ 
		// Regression of compiler warning reported by Jeff Litz/Godfather regarding 
		// strict aliasing (EASTL 1.09.01) December 2007).
		typedef eastl::hash_multimap<uint32_t, uint32_t*> Map;
		Map* pMap = new Map;
		delete pMap;
	}

	{ 
		// Regression of user-reported crash.
		eastl::hash_map<int, eastl::string*>* _hmTextureList;
		_hmTextureList = new eastl::hash_map<int, eastl::string*>();
		eastl::string* a = NULL;
		(*_hmTextureList)[0] = a;
		delete _hmTextureList;
	}

	{
		// Regression of user-reported Android compiler error.
		typedef eastl::hash_multimap<HashRegressionA*, HashRegressionB> HMM;
		HMM m_hash;

		// Section 1
		for (HMM::iterator it = m_hash.begin(); it != m_hash.end(); it++)
			it->second.y = 1;

		// Section 2
		HashRegressionA* pA = NULL;
		eastl::pair<HMM::iterator, HMM::iterator> pair = m_hash.equal_range(pA);
		(void)pair;
	}

	{
		// Regression of user-reported GCC 4.8 compile failure.
		typedef eastl::hash_map<int64_t, Struct> AuditByBlazeIdMap;

		AuditByBlazeIdMap auditBlazeIds;
		AuditByBlazeIdMap tempAuditBlazeIds;

		auditBlazeIds.swap(tempAuditBlazeIds); // This line was generating an unexpected compiler failure.
		EATEST_VERIFY(auditBlazeIds.empty() && tempAuditBlazeIds.empty());
	}

	{
		// This test is designed to designed to use the find_range_by_hash method to walk over all keys in a hash bucket (located by a hash value).
		
		// Use the 'colliding_hash' hash function to intentionally create lots of collisions in a predictable way.
		typedef hash_map<int, int, colliding_hash> HM;
		HM hashMap;

		// Add some numbers to the hashMap.
		for(int i=0; i<90; i++)
		{
			hashMap[i] = i;
		}

		// Try to find a hash value that doesn't exist
		{
			eastl::pair<HM::iterator, HM::iterator> i = hashMap.find_range_by_hash(1000);
			EATEST_VERIFY(i.first == hashMap.end());
			EATEST_VERIFY(i.second == hashMap.end());
		}

		{
			int iterations = 0;
			for(eastl::pair<HM::iterator, HM::iterator> i = hashMap.find_range_by_hash(1); i.first != i.second; i.first++)
			{
				int nodeValue = i.first.get_node()->mValue.first;
				EATEST_VERIFY(nodeValue % 3 == 1);   // Verify the hash of the node matches the expected value
				iterations++;
			}
			EATEST_VERIFY(iterations == 30);
		}

		{
			const HM &constHashMap = hashMap;
			int iterations = 0;
			for(eastl::pair<HM::const_iterator, HM::const_iterator> i = constHashMap.find_range_by_hash(1); i.first != i.second; i.first++)
			{
				int nodeValue = i.first.get_node()->mValue.first;
				EATEST_VERIFY(nodeValue % 3 == 1);   // Verify the hash of the node matches the expected value
				iterations++;
			}
			EATEST_VERIFY(iterations == 30);
		}
	}

	// test hashtable holding move-only types
	#if !defined(EA_COMPILER_MSVC_2013)
	{
		struct Movable
		{
			Movable() {}
			Movable(Movable&&) = default;
			Movable& operator=(Movable&&) = default;
			Movable(const Movable&) = delete;
			Movable& operator=(const Movable&) = delete;

			bool operator==(Movable) const { return true; }

			struct Hash
			{
				size_t operator()(Movable) const { return 0; }
			};
		};

		eastl::unordered_set<Movable, Movable::Hash> a, b;
		swap(a,b);
	}
	#endif

	{
		// hashtable(this_type&& x);
		// hashtable(this_type&& x, const allocator_type& allocator);
		// this_type& operator=(this_type&& x);

		// template <class... Args>
		// insert_return_type emplace(Args&&... args);

		// template <class... Args>
		// iterator emplace_hint(const_iterator position, Args&&... args);

		// template <class P> // Requires that "value_type is constructible from forward<P>(otherValue)."
		// insert_return_type insert(P&& otherValue);

		// iterator insert(const_iterator hint, value_type&& value);

		// Regression of user reported compiler error in hashtable sfinae mechanism 
		{
			TestObject::Reset();
			eastl::hash_set<TestObject> toSet;
			toSet.emplace(3, 4, 5);
		}
	}



	{
		// initializer_list support.
		// hash_map(std::initializer_list<value_type> ilist, size_type nBucketCount = 0, const Hash& hashFunction = Hash(), 
		//            const Predicate& predicate = Predicate(), const allocator_type& allocator = EASTL_HASH_MAP_DEFAULT_ALLOCATOR)
		// this_type& operator=(std::initializer_list<value_type> ilist);
		// void insert(std::initializer_list<value_type> ilist);
		
		// VS2013 has a known issue when dealing with std::initializer_lists
		// https://connect.microsoft.com/VisualStudio/feedback/details/792355/compiler-confused-about-whether-to-use-a-initializer-list-assignment-operator
		#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS) && !(defined(_MSC_VER) && _MSC_VER == 1800)
			hash_map<int, double> intHashMap = { {12,12.0}, {13,13.0}, {14,14.0} };
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
		#endif
	}

	// Can't use move semantics with hash_map::operator[]
	//
	// GCC has a bug with overloading rvalue and lvalue function templates.
	// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=54425
	// 
	// error: 'eastl::pair<T1, T2>::pair(T1&&) [with T1 = const int&; T2 = const int&]' cannot be overloaded
	// error: with 'eastl::pair<T1, T2>::pair(const T1&) [with T1 = const int&; T2 = const int&]'
	#if !defined(EA_COMPILER_GNUC)
	{
		EA_DISABLE_VC_WARNING(4626)
		struct Key
		{
			Key() {}
			Key(Key&&) {}
			Key(const Key&) {}
			bool operator==(const Key&) const { return true; }
		};
		EA_RESTORE_VC_WARNING()

		struct Hash
		{
			std::size_t operator()(const Key&) const { return 0; }
		};

		Key key1, key2;
		eastl::hash_map<Key, int, Hash> hm;
		hm[eastl::move(key1)] = 12345;

		EATEST_VERIFY(hm[eastl::move(key2)] == 12345);
	}
	#endif

	{
		using AllocatorType = CountingAllocator;
		using String = eastl::basic_string<char8_t, AllocatorType>;
		using StringStringMap = eastl::map<String, String, eastl::equal_to<String>, AllocatorType>;
		using StringStringHashMap = eastl::hash_map<String, String, eastl::string_hash<String>, eastl::equal_to<String>, AllocatorType>;
		AllocatorType::resetCount();

		{
			StringStringHashMap myMap(5); // construct map with 5 buckets, so we don't rehash on insert
			String key("mykey01234567890000000000000000000000000000");
			String value("myvalue01234567890000000000000000000000000000");
			AllocatorType::resetCount();

			myMap.insert(eastl::make_pair(eastl::move(key), eastl::move(value)));
			EATEST_VERIFY(AllocatorType::getTotalAllocationCount() == 1);
		}
		{
			StringStringHashMap myMap(5); // construct map with 5 buckets, so we don't rehash on insert
			String key("mykey01234567890000000000000000000000000000");
			String value("myvalue01234567890000000000000000000000000000");
			AllocatorType::resetCount();

			myMap.emplace(eastl::move(key), eastl::move(value));
			EATEST_VERIFY(AllocatorType::getTotalAllocationCount() == 1);
		}
		{
			StringStringMap myMap;
			String key("mykey01234567890000000000000000000000000000");
			String value("myvalue01234567890000000000000000000000000000");
			AllocatorType::resetCount();

			myMap.insert(eastl::make_pair(eastl::move(key), eastl::move(value)));
			EATEST_VERIFY(AllocatorType::getTotalAllocationCount() == 1);
		}
		{
			StringStringMap myMap;
			String key("mykey01234567890000000000000000000000000000");
			String value("myvalue01234567890000000000000000000000000000");
			AllocatorType::resetCount();

			myMap.emplace(eastl::move(key), eastl::move(value));
			EATEST_VERIFY(AllocatorType::getTotalAllocationCount() == 1);
		}
	}

	
	{
		struct name_equals
		{
			bool operator()(const eastl::pair<int, const char*>& a, const eastl::pair<int, const char*>& b) const
			{
				if (a.first != b.first)
					return false;

				return strcmp(a.second, b.second) == 0;
			}
		};

		{
			int n = 42;
			const char* pCStrName = "electronic arts";
			eastl::hash_map<eastl::pair<int, const char*>, bool, eastl::hash<eastl::pair<int, const char*>>, name_equals, eastl::allocator> m_TempNames;
			m_TempNames[eastl::make_pair(n, pCStrName)] = true;

			auto isFound = (m_TempNames.find(eastl::make_pair(n, pCStrName)) != m_TempNames.end());
			VERIFY(isFound);
		}
	}

	{ // User reported regression for code changes limiting hash code generated for non-arithmetic types.
	    { VERIFY(HashTest<char>{}('a') == size_t('a')); }
	    { VERIFY(HashTest<int>{}(42) == 42); }
	    { VERIFY(HashTest<unsigned>{}(42) == 42); }
	    { VERIFY(HashTest<signed>{}(42) == 42); }
	    { VERIFY(HashTest<short>{}(short(42)) == 42); }
	    { VERIFY(HashTest<unsigned short>{}((unsigned short)42) == 42); }
	    { VERIFY(HashTest<int>{}(42) == 42); }
	    { VERIFY(HashTest<unsigned int>{}(42) == 42); }
	    { VERIFY(HashTest<long int>{}(42) == 42); }
	    { VERIFY(HashTest<unsigned long int>{}(42) == 42); }
	    { VERIFY(HashTest<long long int>{}(42) == 42); }
	    { VERIFY(HashTest<unsigned long long int>{}(42) == 42); }

	#if defined(EA_HAVE_INT128) && EA_HAVE_INT128
	    { VERIFY(HashTest<uint128_t>{}(UINT128_C(0, 42)) == 42); }
	#endif
    }

	return nErrorCount;
}









