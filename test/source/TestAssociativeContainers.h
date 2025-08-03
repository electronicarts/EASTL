// (c) 2024 Electronic Arts Inc.

#include "EASTLTest.h"


///////////////////////////////////////////////////////////////////////////////
// TestAssociativeContainerHeterogeneousLookup
//
// This function is designed to work with map, multimap, set, multiset, fixed_*, hash_map (unordered_map), hash_multimap, hash_set, hash_multihash, fixed_hash_*.
//
// Requires container has a key_type of ExplicitString and has a single element with key "found".
// 
// Tests for heterogeneous element lookup: find(), count(), contains(), equal_range()
template <typename T>
int TestAssociativeContainerHeterogeneousLookup(T&& container)
{
	int nErrorCount = 0;

	ExplicitString::Reset();

	VERIFY(container.find("not found") == container.end());
	VERIFY(container.find("found") == container.begin());

	VERIFY(container.count("not found") == 0);
	VERIFY(container.count("found") == 1);

	VERIFY(container.contains("found"));
	VERIFY(!container.contains("not found"));

	VERIFY(container.equal_range("not found") == eastl::make_pair(container.end(), container.end()));

	auto it_found = container.find("found");
	auto it_found_next = it_found;
	++it_found_next;
	VERIFY(container.equal_range("found") == eastl::make_pair(it_found, it_found_next));

	VERIFY(ExplicitString::sCtorFromStrCount == 0);

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestOrderedAssociativeContainerHeterogeneousLookup
//
// This function is designed to work with map, fixed_map, vector_map.
//
// Requires container has a key_type of ExplicitString and has a single element with key "found".
//
// Tests for heterogeneous element lookup: lower_bound(), upper_bound()
template <typename T>
int TestOrderedAssociativeContainerHeterogeneousLookup(T&& container)
{
	int nErrorCount = 0;

	ExplicitString::Reset();

	VERIFY(container.lower_bound("not found") == container.upper_bound("not found"));
	VERIFY(container.lower_bound("found") != container.upper_bound("found"));

	VERIFY(ExplicitString::sCtorFromStrCount == 0);

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestMapHeterogeneousInsertion
//
// This function is designed to work with map, fixed_map, hash_map (unordered_map), fixed_hash_map, vector_map.
//
// Requires container has a key_type of ExplicitString and mapped_type of int.
// 
// Tests for heterogeneous element insertion: try_emplace(), insert_or_assign(), at(), operator[]
template <typename T>
int TestMapHeterogeneousInsertion()
{
	int nErrorCount = 0;

	ExplicitString::Reset();

	T container;

	// all of these statements cause the container to construct key_type, due to the insertion.
	EATEST_VERIFY(container.try_emplace("0", 1).second);
	EATEST_VERIFY(container.insert_or_assign("1", 1).second);
	container["3"] = 3;

	EATEST_VERIFY(ExplicitString::sCtorFromStrCount == 3);

	ExplicitString::Reset();

	// no construction of key_type:

	EATEST_VERIFY(!container.try_emplace("0").second);
	EATEST_VERIFY(!container.try_emplace("1").second);
	EATEST_VERIFY(!container.try_emplace("3").second);

	EATEST_VERIFY(container["0"] == 1);
	EATEST_VERIFY(container["1"] == 1);
	EATEST_VERIFY(container["3"] == 3);

	EATEST_VERIFY(!container.insert_or_assign("1", 2).second);

	EATEST_VERIFY(container.at("0") == 1);
	EATEST_VERIFY(container.at("1") == 2);
	EATEST_VERIFY(container.at("3") == 3);

	VERIFY(ExplicitString::sCtorFromStrCount == 0);

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestSetHeterogeneousInsertion
//
// This function is designed to work with set, fixed_set, hash_set (unordered_set), fixed_hash_set, vector_set.
//
// Requires container has a key_type of ExplicitString.
//
// Tests for heterogeneous element insertion: insert()
template <typename T>
int TestSetHeterogeneousInsertion()
{
	int nErrorCount = 0;

	ExplicitString::Reset();

	T container;

	// all of these statements cause the container to construct key_type, due to the insertion.
	EATEST_VERIFY(container.insert("0").second);
	EATEST_VERIFY(container.insert("1").second);
	EATEST_VERIFY(container.insert("3").second);

	EATEST_VERIFY(ExplicitString::sCtorFromStrCount == 3);

	ExplicitString::Reset();

	// no construction of key_type:

	auto insert_result0 = container.insert("0");
	EATEST_VERIFY(insert_result0.first == container.find("0"));
	EATEST_VERIFY(!insert_result0.second);

	auto insert_result1 = container.insert("1");
	EATEST_VERIFY(insert_result1.first == container.find("1"));
	EATEST_VERIFY(!insert_result1.second);

	auto insert_result3 = container.insert("3");
	EATEST_VERIFY(insert_result3.first == container.find("3"));
	EATEST_VERIFY(!insert_result3.second);

	VERIFY(ExplicitString::sCtorFromStrCount == 0);

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestAssociativeContainerHeterogeneousErasure
//
// This function is designed to work with map, multimap, set, multiset, fixed_*, hash_map (unordered_map), hash_multimap, hash_set, hash_multihash, fixed_hash_*.
//
// Requires container has a key_type of ExplicitString and has a single element with key "found".
//
// Tests for heterogeneous element insertion: erase()
template <typename T>
int TestAssociativeContainerHeterogeneousErasure(T&& container)
{
	int nErrorCount = 0;

	ExplicitString::Reset();

	VERIFY(container.erase("found") == 1);
	VERIFY(container.erase("not found") == 0);

	VERIFY(ExplicitString::sCtorFromStrCount == 0);

	return nErrorCount;
}
