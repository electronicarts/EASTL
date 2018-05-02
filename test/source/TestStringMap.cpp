/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/string_map.h>
#include <EAStdC/EAString.h>

using namespace eastl;


// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::string_map<int>;
template class eastl::string_map<Align32>;

static const char* strings[] = { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t" };
static const size_t kStringCount = 10; // This is intentionally half the length of strings, so that we can test with strings that are not inserted to the map. 


int TestStringMap()
{
	int nErrorCount = 0;

	{  // Test declarations
		string_map<int>	stringMap;

		string_map<int> stringMap2(stringMap);
		EATEST_VERIFY(stringMap2.size() == stringMap.size());
		EATEST_VERIFY(stringMap2 == stringMap);


		// allocator_type& get_allocator();
		// void            set_allocator(const allocator_type& allocator);
		string_map<int>::allocator_type& allocator = stringMap.get_allocator();
		stringMap.set_allocator(EASTLAllocatorType());
		stringMap.set_allocator(allocator);
		// To do: Try to find something better to test here.

		
		// const char*     get_name() const;
		// void            set_name(const char* pName);
#if EASTL_NAME_ENABLED
		stringMap.get_allocator().set_name("test");
		const char* pName = stringMap.get_allocator().get_name();
		EATEST_VERIFY(equal(pName, pName + 5, "test"));
#endif
	}


	{
		string_map<int> stringMap;

		// Clear a newly constructed, already empty container.
		stringMap.clear();
		EATEST_VERIFY(stringMap.validate());
		EATEST_VERIFY(stringMap.size() == 0);

		for (int i = 0; i < (int)kStringCount; i++)
			stringMap.insert(strings[i], i);

		EATEST_VERIFY(stringMap.validate());
		EATEST_VERIFY(stringMap.size() == kStringCount);

		stringMap.clear();
		EATEST_VERIFY(stringMap.validate());
		EATEST_VERIFY(stringMap.size() == 0);

		for (int i = 0; i < (int)kStringCount; i++)
			stringMap.insert(strings[i], i);
		EATEST_VERIFY(stringMap.validate());
		EATEST_VERIFY(stringMap.size() == kStringCount);

		stringMap.clear();
		EATEST_VERIFY(stringMap.validate());
		EATEST_VERIFY(stringMap.size() == 0);
	}


	{   // Test string_map

		// size_type          size() const
		// bool               empty() const
		// insert_return_type insert(const value_type& value);
		// insert_return_type insert(const value_type& value, hash_code_t c, node_type* pNodeNew = NULL);
		// iterator           insert(const_iterator, const value_type& value);
		// iterator           find(const key_type& k);
		// const_iterator     find(const key_type& k) const;
		// size_type          count(const key_type& k) const;

		typedef string_map<int> StringMapInt;

		StringMapInt stringMap;

		EATEST_VERIFY(stringMap.empty());
		EATEST_VERIFY(stringMap.size() == 0);
		EATEST_VERIFY(stringMap.count(strings[0]) == 0);

		for (int i = 0; i < (int)kStringCount; i++)
			stringMap.insert(strings[i], i);

		EATEST_VERIFY(!stringMap.empty());
		EATEST_VERIFY(stringMap.size() == kStringCount);
		EATEST_VERIFY(stringMap.count(strings[0]) == 1);

		int j = 0;
		for (StringMapInt::iterator it = stringMap.begin(); it != stringMap.end(); ++it, ++j)
		{
			int value = (*it).second;
			EATEST_VERIFY(value < (int)kStringCount);
		}

		for (int i = 0; i < (int)kStringCount * 2; i++)
		{
			StringMapInt::iterator it = stringMap.find(strings[i]);

			if (i < (int)kStringCount)
			{
				EATEST_VERIFY(it != stringMap.end());
				const char* k = (*it).first;
				int v = (*it).second;
				EATEST_VERIFY(EA::StdC::Strcmp(k, strings[i]) == 0);
				EATEST_VERIFY(v == i);
			}
			else
				EATEST_VERIFY(it == stringMap.end());
		}

		StringMapInt::insert_return_type result = stringMap.insert("EASTLTEST");
		EATEST_VERIFY(result.second == true);
		result = stringMap.insert("EASTLTEST");
		EATEST_VERIFY(result.second == false);
		result.first->second = 0;

		// iterator erase(const_iterator);
		size_t nExpectedSize = stringMap.size();

		StringMapInt::iterator itD = stringMap.find("d");
		EATEST_VERIFY(itD != stringMap.end());

		// erase the element and verify that the size has decreased
		stringMap.erase(itD);
		nExpectedSize--;
		EATEST_VERIFY(stringMap.size() == nExpectedSize);

		// verify that erased element is gone
		itD = stringMap.find(strings[3]);
		EATEST_VERIFY(itD == stringMap.end());

		// iterator erase(const char*)
		StringMapInt::size_type n = stringMap.erase(strings[4]);
		nExpectedSize--;
		EATEST_VERIFY(n == 1);
		EATEST_VERIFY(stringMap.size() == nExpectedSize);


		// mapped_type& operator[](const key_type& key)
		stringMap.clear();

		int x = stringMap["A"]; // A default-constructed int (i.e. 0) should be returned.
		EATEST_VERIFY(x == 0);

		stringMap["B"] = 1;
		x = stringMap["B"];
		EATEST_VERIFY(x == 1);     // Verify that the value we assigned is returned and a default-constructed value is not returned.

		stringMap["A"] = 10;    // Overwrite our previous 0 with 10.
		stringMap["B"] = 11;
		x = stringMap["A"];
		EATEST_VERIFY(x == 10);    // Verify the value is as expected.
		x = stringMap["B"];
		EATEST_VERIFY(x == 11);

	}


	{
		// string_map(const allocator_type& allocator);
		// string_map& operator=(const this_type& x);
		// bool validate() const;

		string_map<int> stringMap1(EASTLAllocatorType("TestStringMap"));
		string_map<int> stringMap2(stringMap1);

		for (int i = 0; i < (int)kStringCount; i++)
		{
			stringMap1.insert(strings[i], i);
		}

		stringMap2 = stringMap1;
		string_map<int> stringMap3(stringMap1);

		EATEST_VERIFY(stringMap1.validate());
		EATEST_VERIFY(stringMap2.validate());
		EATEST_VERIFY(stringMap3.validate());

		for (int i = 0; i < (int)kStringCount; i++)
		{
			EATEST_VERIFY(stringMap1[strings[i]] == stringMap2[strings[i]]);
			EATEST_VERIFY(stringMap1[strings[i]] == stringMap3[strings[i]]);
		}

	}

	return nErrorCount;
}
