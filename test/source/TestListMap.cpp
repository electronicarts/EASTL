/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/utility.h>
#include <EASTL/bonus/list_map.h>


#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stdio.h>

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif



// We would like to use the generic EASTLTest VerifySequence function, but it's not currently ready to deal
// with non-POD types. That can probably be solved, but in the meantime we implement a custom function here.
template <typename T1, typename T2>
bool VerifyListMapSequence(const char* pName, 
							eastl::list_map<T1, T2>& listMap, 
							T1 t1End,   T2, 
							T1 t10 = 0, T2 t20 = 0, 
							T1 t11 = 0, T2 t21 = 0, 
							T1 t12 = 0, T2 t22 = 0, 
							T1 t13 = 0, T2 t23 = 0, 
							T1 t14 = 0, T2 t24 = 0, 
							T1 t15 = 0, T2 t25 = 0)
{
	typename eastl::list_map<T1, T2>::iterator it = listMap.begin();

	if(t10 == t1End)
		return (it == listMap.end());
	if(it->first != t10 || it->second != t20)
		{ EASTLTest_Printf("[%s] Mismatch at index %d\n", pName, 0); return false; }
	++it;

	if(t11 == t1End)
		return (it == listMap.end());
	if(it->first != t11 || it->second != t21)
		{ EASTLTest_Printf("[%s] Mismatch at index %d\n", pName, 1); return false; }
	++it;

	if(t12 == t1End)
		return (it == listMap.end());
	if(it->first != t12 || it->second != t22)
		{ EASTLTest_Printf("[%s] Mismatch at index %d\n", pName, 2); return false; }
	++it;

	if(t13 == t1End)
		return (it == listMap.end());
	if(it->first != t13 || it->second != t23)
		{ EASTLTest_Printf("[%s] Mismatch at index %d\n", pName, 3); return false; }
	++it;

	if(t14 == t1End)
		return (it == listMap.end());
	if(it->first != t14 || it->second != t24)
		{ EASTLTest_Printf("[%s] Mismatch at index %d\n", pName, 4); return false; }
	++it;

	if(t15 == t1End)
		return (it == listMap.end());
	if(it->first != t15 || it->second != t25)
		{ EASTLTest_Printf("[%s] Mismatch at index %d\n", pName, 5); return false; }
	++it;

	return true;
}


int TestListMap()
{
	int nErrorCount = 0;

	{
		typedef eastl::list_map<uint32_t, uint64_t> TestMapType;
		typedef eastl::pair<uint32_t, uint64_t>     ValueType;      // We currently can't use TestMapType::value_type because its 'first' is const.

		TestMapType                         testMap;
		TestMapType::iterator               iter;
		TestMapType::const_iterator         c_iter;
		TestMapType::reverse_iterator       rIter;
		TestMapType::const_reverse_iterator c_rIter;
		TestMapType::iterator               tempIter;

		EATEST_VERIFY(testMap.empty());
		EATEST_VERIFY(testMap.validate());

		testMap.push_front(ValueType(3, 1003));
		EATEST_VERIFY(testMap.validate());

		testMap.push_back(ValueType(4, 1004));
		EATEST_VERIFY(testMap.validate());

		testMap.push_back(ValueType(2, 1002));
		EATEST_VERIFY(testMap.validate());

		testMap.push_front(ValueType(6, 1006));
		EATEST_VERIFY(testMap.validate());

		EATEST_VERIFY(!testMap.empty());
		EATEST_VERIFY(testMap.size() == 4);
	
		EATEST_VERIFY(testMap.find(3) != testMap.end());
		EATEST_VERIFY(testMap.find(5) == testMap.end());
		EATEST_VERIFY((VerifyListMapSequence<uint32_t, uint64_t>("list_map::push_back", testMap, UINT32_MAX, 0,  6, 1006,  3, 1003,  4, 1004,  2, 1002,  UINT32_MAX, 0)));

		iter = testMap.find(3);
		EATEST_VERIFY((iter->first == 3) && ((++iter)->first == 4) && ((++iter)->first == 2));

		rIter = testMap.rbegin();
		EATEST_VERIFY((rIter->first == 2) && ((++rIter)->first == 4) && ((++rIter)->first == 3) && ((++rIter)->first == 6));

		TestMapType::const_reference rFront = testMap.front();
		EATEST_VERIFY(rFront.first == 6);

		TestMapType::reference rBack = testMap.back();
		EATEST_VERIFY(rBack.first == 2);

		testMap.clear();
		EATEST_VERIFY(testMap.empty());
		EATEST_VERIFY(testMap.validate());

		iter = testMap.begin();
		EATEST_VERIFY(iter == testMap.end());

		testMap.push_back(ValueType(10, 1010));
		EATEST_VERIFY(testMap.validate());

		testMap.push_front(ValueType(8, 1008));
		EATEST_VERIFY(testMap.validate());

		testMap.push_back(7, 1007);
		EATEST_VERIFY(testMap.validate());

		testMap.push_front(9, 1009);
		EATEST_VERIFY(testMap.validate());

		testMap.push_back(11, 1011LL);
		EATEST_VERIFY(testMap.validate());

		EATEST_VERIFY((VerifyListMapSequence<uint32_t, uint64_t>("list_map::push_back", testMap, UINT32_MAX, 0,  9, 1009,  8, 1008,  10, 1010,  7, 1007,  11, 1011,  UINT32_MAX, 0)));

		testMap.pop_front();
		EATEST_VERIFY(testMap.validate());
		EATEST_VERIFY((VerifyListMapSequence<uint32_t, uint64_t>("list_map::push_back", testMap, UINT32_MAX, 0,  8, 1008,  10, 1010,  7, 1007,  11, 1011,  UINT32_MAX, 0)));

		rIter = testMap.rbegin();
		EATEST_VERIFY((rIter->first == 11 && ((++rIter)->first == 7) && ((++rIter)->first == 10) && ((++rIter)->first == 8)));

		testMap.pop_back();
		EATEST_VERIFY(testMap.validate());
		EATEST_VERIFY((VerifyListMapSequence<uint32_t, uint64_t>("list_map::push_back", testMap, UINT32_MAX, 0,  8, 1008,  10, 1010,  7, 1007,  UINT32_MAX, 0)));

		rIter = testMap.rbegin();
		EATEST_VERIFY(((rIter)->first == 7) && ((++rIter)->first == 10) && ((++rIter)->first == 8));

		tempIter = testMap.find(10);
		EATEST_VERIFY(tempIter != testMap.end());

		testMap.erase(10);
		EATEST_VERIFY(testMap.validate());
		EATEST_VERIFY((VerifyListMapSequence<uint32_t, uint64_t>("list_map::push_back", testMap, UINT32_MAX, 0,  8, 1008,  7, 1007,  UINT32_MAX, 0)));

		EATEST_VERIFY(testMap.validate_iterator(testMap.find(8)) == (eastl::isf_valid | eastl::isf_current | eastl::isf_can_dereference));
		EATEST_VERIFY(testMap.validate_iterator(testMap.find(30)) == (eastl::isf_valid | eastl::isf_current));
		EATEST_VERIFY(testMap.validate_iterator(tempIter) == eastl::isf_none);
		EATEST_VERIFY(testMap.validate());

		testMap.erase(20);  // erasing an index not in use should still be safe
		EATEST_VERIFY(testMap.validate());
		EATEST_VERIFY((VerifyListMapSequence<uint32_t, uint64_t>("list_map::push_back", testMap, UINT32_MAX, 0,  8, 1008,  7, 1007,  UINT32_MAX, 0)));

		EATEST_VERIFY(testMap.count(7) == 1);
		EATEST_VERIFY(testMap.count(10) == 0);
		EATEST_VERIFY(testMap.validate());

		testMap.erase(testMap.find(8));
		EATEST_VERIFY(testMap.validate());
		EATEST_VERIFY((VerifyListMapSequence<uint32_t, uint64_t>("list_map::push_back", testMap, UINT32_MAX, 0,  7, 1007,  UINT32_MAX, 0)));

		testMap.erase(testMap.rbegin());
		EATEST_VERIFY(testMap.empty());
		EATEST_VERIFY(testMap.validate());
	}

	{
		typedef eastl::list_map<eastl::string, uint32_t> TestStringMapType;
		TestStringMapType           testStringMap;
		TestStringMapType::iterator strIter;

		testStringMap.push_back(eastl::string("hello"), 750);
		EATEST_VERIFY(testStringMap.size() == 1);

		strIter = testStringMap.find_as("hello", eastl::less<>());
		EATEST_VERIFY(strIter != testStringMap.end());
		EATEST_VERIFY(strIter->first == "hello");
		EATEST_VERIFY(strIter->second == 750);

		strIter = testStringMap.find_as("fake_string", eastl::less<>());
		EATEST_VERIFY(strIter == testStringMap.end());
		EATEST_VERIFY(testStringMap.validate());
	}

	return nErrorCount;
}









