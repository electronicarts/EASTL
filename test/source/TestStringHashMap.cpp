/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/string_hash_map.h>
#include <EAStdC/EAString.h>

using namespace eastl;


// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::string_hash_map<int>;
template class eastl::string_hash_map<Align32>;

static const char* strings[] = { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t"};
static const size_t kStringCount = 10; // This is intentionally half the length of strings, so that we can test with strings that are not inserted to the map. 


int TestStringHashMap()
{   
	int nErrorCount = 0;

	{  // Test declarations
		string_hash_map<int>	stringHashMap;
		
		string_hash_map<int> stringHashMap2(stringHashMap);
		EATEST_VERIFY(stringHashMap2.size() == stringHashMap.size());
		EATEST_VERIFY(stringHashMap2 == stringHashMap);


		// allocator_type& get_allocator();
		// void            set_allocator(const allocator_type& allocator);
		string_hash_map<int>::allocator_type& allocator = stringHashMap.get_allocator();
		stringHashMap.set_allocator(EASTLAllocatorType());
		stringHashMap.set_allocator(allocator);
		// To do: Try to find something better to test here.


		// const key_equal& key_eq() const;
		// key_equal&       key_eq();
		string_hash_map<int>       hs;
		const string_hash_map<int> hsc;

		const string_hash_map<int>::key_equal& ke = hsc.key_eq();
		hs.key_eq() = ke;


		// const char*     get_name() const;
		// void            set_name(const char* pName);
		#if EASTL_NAME_ENABLED
			stringHashMap.get_allocator().set_name("test");
			const char* pName = stringHashMap.get_allocator().get_name();
			EATEST_VERIFY(equal(pName, pName + 5, "test"));
		#endif
	}


	{
		string_hash_map<int> stringHashMap;

		// Clear a newly constructed, already empty container.
		stringHashMap.clear(true);
		EATEST_VERIFY(stringHashMap.validate());
		EATEST_VERIFY(stringHashMap.size() == 0);
		EATEST_VERIFY(stringHashMap.bucket_count() == 1);

		for (int i = 0; i < (int)kStringCount; i++)
			stringHashMap.insert(strings[i], i);

		EATEST_VERIFY(stringHashMap.validate());
		EATEST_VERIFY(stringHashMap.size() == kStringCount);

		stringHashMap.clear(true);
		EATEST_VERIFY(stringHashMap.validate());
		EATEST_VERIFY(stringHashMap.size() == 0);
		EATEST_VERIFY(stringHashMap.bucket_count() == 1);

		for (int i = 0; i < (int)kStringCount; i++)
			stringHashMap.insert(strings[i], i);
		EATEST_VERIFY(stringHashMap.validate());
		EATEST_VERIFY(stringHashMap.size() == kStringCount);

		stringHashMap.clear(true);
		EATEST_VERIFY(stringHashMap.validate());
		EATEST_VERIFY(stringHashMap.size() == 0);
		EATEST_VERIFY(stringHashMap.bucket_count() == 1);
	}


	{   // Test string_hash_map

		// size_type          size() const
		// bool               empty() const
		// insert_return_type insert(const value_type& value);
		// insert_return_type insert(const value_type& value, hash_code_t c, node_type* pNodeNew = NULL);
		// iterator           insert(const_iterator, const value_type& value);
		// iterator           find(const key_type& k);
		// const_iterator     find(const key_type& k) const;
		// size_type          count(const key_type& k) const;

		typedef string_hash_map<int> StringHashMapInt;

		StringHashMapInt stringHashMap;

		EATEST_VERIFY(stringHashMap.empty());
		EATEST_VERIFY(stringHashMap.size() == 0);
		EATEST_VERIFY(stringHashMap.count(strings[0]) == 0);

		for (int i = 0; i < (int)kStringCount; i++)
			stringHashMap.insert(strings[i], i);

		EATEST_VERIFY(!stringHashMap.empty());
		EATEST_VERIFY(stringHashMap.size() == kStringCount);
		EATEST_VERIFY(stringHashMap.count(strings[0]) == 1);

		int j = 0;
		for (StringHashMapInt::iterator it = stringHashMap.begin(); it != stringHashMap.end(); ++it, ++j)
		{
			int value = (*it).second;
			EATEST_VERIFY(value < (int)kStringCount);
		}

		for(int i = 0; i < (int)kStringCount * 2; i++)
		{
			StringHashMapInt::iterator it = stringHashMap.find(strings[i]);

			if (i < (int)kStringCount)
			{
				EATEST_VERIFY(it != stringHashMap.end());
				const char* k = it->first;
				int v = it->second;
				EATEST_VERIFY(EA::StdC::Strcmp(k, strings[i]) == 0);
				EATEST_VERIFY(v == i);
			}
			else
				EATEST_VERIFY(it == stringHashMap.end());
		}

		StringHashMapInt::insert_return_type result = stringHashMap.insert("EASTLTEST");
		EATEST_VERIFY(result.second == true);
		result = stringHashMap.insert("EASTLTEST");
		EATEST_VERIFY(result.second == false);
		result.first->second = 0;

		// iterator erase(const_iterator);
		size_t nExpectedSize = stringHashMap.size();

		StringHashMapInt::iterator itD = stringHashMap.find("d");
		EATEST_VERIFY(itD != stringHashMap.end());

		// erase the element and verify that the size has decreased
		stringHashMap.erase(itD);
		nExpectedSize--;
		EATEST_VERIFY(stringHashMap.size() == nExpectedSize);

		// verify that erased element is gone
		itD = stringHashMap.find(strings[3]);
		EATEST_VERIFY(itD == stringHashMap.end());

		// iterator erase(const char*)
		StringHashMapInt::size_type n = stringHashMap.erase(strings[4]);
		nExpectedSize--;
		EATEST_VERIFY(n == 1);
		EATEST_VERIFY(stringHashMap.size() == nExpectedSize);
		

		// mapped_type& operator[](const key_type& key)
		stringHashMap.clear();

		int x = stringHashMap["A"]; // A default-constructed int (i.e. 0) should be returned.
		EATEST_VERIFY(x == 0);

		stringHashMap["B"] = 1;
		x = stringHashMap["B"];
		EATEST_VERIFY(x == 1);     // Verify that the value we assigned is returned and a default-constructed value is not returned.

		stringHashMap["A"] = 10;    // Overwrite our previous 0 with 10.
		stringHashMap["B"] = 11;
		x = stringHashMap["A"];
		EATEST_VERIFY(x == 10);    // Verify the value is as expected.
		x = stringHashMap["B"];
		EATEST_VERIFY(x == 11);

	}

	
	{
		// string_hash_map(const allocator_type& allocator);
		// string_hash_map& operator=(const this_type& x);
		// bool validate() const;

		string_hash_map<int> stringHashMap1(EASTLAllocatorType("TestStringHashMap"));
		string_hash_map<int> stringHashMap2(stringHashMap1);

		for (int i = 0; i < (int)kStringCount; i++)
		{
			stringHashMap1.insert(strings[i], i);
		}

		stringHashMap2 = stringHashMap1;
		string_hash_map<int> stringHashMap3(stringHashMap1);

		EATEST_VERIFY(stringHashMap1.validate());
		EATEST_VERIFY(stringHashMap2.validate());
		EATEST_VERIFY(stringHashMap3.validate());

		for (int i = 0; i < (int)kStringCount; i++)
		{
			EATEST_VERIFY(stringHashMap1[strings[i]] == stringHashMap2[strings[i]]);
			EATEST_VERIFY(stringHashMap1[strings[i]] == stringHashMap3[strings[i]]);
		}

	}

	// pair<iterator, bool> insert_or_assign(const char* key, const T& value);
	{
		{
			string_hash_map<int> m;

			m.insert_or_assign("hello", 0);
			EATEST_VERIFY(m["hello"] == 0);

			m.insert_or_assign("hello", 42);
			EATEST_VERIFY(m["hello"] == 42);

			m.insert_or_assign("hello", 43);
			EATEST_VERIFY(m["hello"] == 43);

			m.insert_or_assign("hello", 1143);
			EATEST_VERIFY(m["hello"] == 1143);

			EATEST_VERIFY(m.size() == 1);
			m.clear(); 
			EATEST_VERIFY(m.size() == 0);
		}

		{
			string_hash_map<int> m;
			m.insert_or_assign("hello", 0);
			m.insert_or_assign("hello2", 0);

			EATEST_VERIFY(m.size() == 2);
			m.clear(); 
			EATEST_VERIFY(m.size() == 0);
		}

		{
			string_hash_map<int> m;
			m.insert_or_assign("hello", 0);
			m.insert_or_assign("hello2", 0);

			EATEST_VERIFY(m.size() == 2);
			m.clear(true); 
			EATEST_VERIFY(m.size() == 0);
		}

		{
			string_hash_map<int> m;
			m.insert_or_assign("hello", 0);
			m.insert_or_assign("hello2", 0);

			EATEST_VERIFY(m.size() == 2);
			m.clear(false); 
			EATEST_VERIFY(m.size() == 0);
		}

		{
			string_hash_map<TestObject> m;

			m.insert_or_assign("hello", TestObject(42));
			EATEST_VERIFY(m["hello"].mX == 42);

			m.insert_or_assign("hello", TestObject(43));
			EATEST_VERIFY(m["hello"].mX == 43);

			EATEST_VERIFY(m.size() == 1);
		}

		{
			typedef string_hash_map<TestObject, hash<const char*>, str_equal_to<const char*>, CountingAllocator> counting_string_hash_map;
			counting_string_hash_map m;
			EATEST_VERIFY(CountingAllocator::getActiveAllocationCount() == 0);

			m.insert_or_assign("hello", TestObject(42));
			EATEST_VERIFY(CountingAllocator::getActiveAllocationCount() == 3);
			EATEST_VERIFY(m["hello"].mX == 42);
			EATEST_VERIFY(CountingAllocator::getActiveAllocationCount() == 3);

			m.insert_or_assign("hello", TestObject(43));
			EATEST_VERIFY(CountingAllocator::getActiveAllocationCount() == 3);
			EATEST_VERIFY(m["hello"].mX == 43);
			EATEST_VERIFY(CountingAllocator::getActiveAllocationCount() == 3);

			EATEST_VERIFY(m.size() == 1);
		}
		EATEST_VERIFY(CountingAllocator::getActiveAllocationCount() == 0);
	}

	return nErrorCount;
}
