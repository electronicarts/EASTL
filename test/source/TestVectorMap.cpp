/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "TestMap.h"
#include "EASTLTest.h"
#include <EASTL/vector_map.h>
#include <EASTL/vector_multimap.h>
#include <EASTL/vector.h>
#include <EASTL/deque.h>
#include <EASTL/string.h>
#include <EASTL/fixed_string.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/utility.h>

EA_DISABLE_ALL_VC_WARNINGS()
#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	#include <map>
#endif
EA_RESTORE_ALL_VC_WARNINGS()

using namespace eastl;


// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::vector_map<int, int>;
template class eastl::vector_multimap<float, int>;
template class eastl::vector_map<TestObject, TestObject>;
template class eastl::vector_multimap<TestObject, TestObject>;


///////////////////////////////////////////////////////////////////////////////
// typedefs
//
typedef eastl::vector_map<int, int> VM1;
typedef eastl::vector_map<int, int, eastl::less<int>, EASTLAllocatorType, eastl::deque<eastl::pair<int, int> > > VM2;

typedef eastl::vector_map<TestObject, TestObject> VM4;
typedef eastl::vector_map<TestObject, TestObject, eastl::less<TestObject>, EASTLAllocatorType, eastl::deque<eastl::pair<TestObject, TestObject> > > VM5;

static_assert(sizeof(eastl::vector_map<int, int>) == sizeof(eastl::vector<int>), "if is_empty_v<Compare>, sizeof(vector_map) == sizeof(RandomAccessContainer)");
static_assert(sizeof(eastl::vector_map<double, double>) == sizeof(eastl::vector<double>), "if is_empty_v<Compare>, sizeof(vector_map) == sizeof(RandomAccessContainer)");

typedef eastl::vector_multimap<int, int> VMM1;
typedef eastl::vector_multimap<int, int, eastl::less<int>, EASTLAllocatorType, eastl::deque<eastl::pair<int, int> > > VMM2;

typedef eastl::vector_multimap<TestObject, TestObject> VMM4;
typedef eastl::vector_multimap<TestObject, TestObject, eastl::less<TestObject>, EASTLAllocatorType, eastl::deque<eastl::pair<TestObject, TestObject> > > VMM5;

static_assert(sizeof(eastl::vector_multimap<int, int>) == sizeof(eastl::vector<eastl::pair<int, int>>), "if is_empty_v<Compare>, sizeof(vector_multimap) == sizeof(RandomAccessContainer)");
static_assert(sizeof(eastl::vector_multimap<TestObject, TestObject>) == sizeof(eastl::vector<eastl::pair<TestObject, TestObject>>), "if is_empty_v<Compare>, sizeof(vector_multimap) == sizeof(RandomAccessContainer)");

#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	typedef std::map<int, int> VM3;
	typedef std::map<TestObject, TestObject> VM6;
	typedef std::multimap<int, int> VMM3;
	typedef std::multimap<TestObject, TestObject> VMM6;
#endif
///////////////////////////////////////////////////////////////////////////////

template <typename T1>
int TestVectorMapAtKey()
{
	int nErrorCount = 0;

	typedef T1 TOMap;
	typedef typename TOMap::key_type key_type;
	typedef typename TOMap::mapped_type mapped_type;

	TOMap map1;
	map1[key_type(1)] = mapped_type(1);
	map1[key_type(3)] = mapped_type(3);
	map1[key_type(0)] = mapped_type(1);
	EATEST_VERIFY(map1.at_key(key_type(0)) == mapped_type(1));
	EATEST_VERIFY(map1.at_key(key_type(1)) == mapped_type(1));
	EATEST_VERIFY(map1.at_key(key_type(3)) == mapped_type(3));

	return nErrorCount;
}

int TestVectorMap()
{
	int nErrorCount = 0;

	#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
		{   // Test construction
			nErrorCount += TestMapConstruction<VM1, VM3, false>();
			nErrorCount += TestMapConstruction<VM2, VM3, false>();
			nErrorCount += TestMapConstruction<VM4, VM6, false>();
			nErrorCount += TestMapConstruction<VM5, VM6, false>();

			nErrorCount += TestMapConstruction<VMM1, VMM3, true>();
			nErrorCount += TestMapConstruction<VMM2, VMM3, true>();
			nErrorCount += TestMapConstruction<VMM4, VMM6, true>();
			nErrorCount += TestMapConstruction<VMM5, VMM6, true>();
		}


		{   // Test mutating functionality.
			nErrorCount += TestMapMutation<VM1, VM3, false>();
			nErrorCount += TestMapMutation<VM2, VM3, false>();
			nErrorCount += TestMapMutation<VM4, VM6, false>();
			nErrorCount += TestMapMutation<VM5, VM6, false>();

			nErrorCount += TestMapMutation<VMM1, VMM3, true>();
			nErrorCount += TestMapMutation<VMM2, VMM3, true>();
			nErrorCount += TestMapMutation<VMM4, VMM6, true>();
			nErrorCount += TestMapMutation<VMM5, VMM6, true>();
		}
	#endif // EA_COMPILER_NO_STANDARD_CPP_LIBRARY


	{   // Test search functionality.
		nErrorCount += TestMapSearch<VM1, false>();
		nErrorCount += TestMapSearch<VM2, false>();
		nErrorCount += TestMapSearch<VM4, false>();
		nErrorCount += TestMapSearch<VM5, false>();

		nErrorCount += TestMapSearch<VMM1, true>();
		nErrorCount += TestMapSearch<VMM2, true>();
		nErrorCount += TestMapSearch<VMM4, true>();
		nErrorCount += TestMapSearch<VMM5, true>();
	}


	{
		// C++11 emplace and related functionality
		nErrorCount += TestMapCpp11<eastl::vector_map<int, TestObject> >();
		nErrorCount += TestMapCpp11<eastl::vector_map<int, TestObject, eastl::less<int>, EASTLAllocatorType, eastl::deque<eastl::pair<int, TestObject> > > >();

		nErrorCount += TestMultimapCpp11<eastl::vector_multimap<int, TestObject> >();
		nErrorCount += TestMultimapCpp11<eastl::vector_multimap<int, TestObject, eastl::less<int>, EASTLAllocatorType, eastl::deque<eastl::pair<int, TestObject> > > >();
	}

	{
		// Tests for element access: operator[] and at()
		
		// todo: can't enable these tests until the current at() function is removed (which gets by index rather than by key) and we can replace with a semanticly correct one.
		
		//nErrorCount += TestMapAccess<VM1>();
		//nErrorCount += TestMapAccess<VM2>();
		//nErrorCount += TestMapAccess<VM4>();
		//nErrorCount += TestMapAccess<VM5>();
		 
		nErrorCount += TestVectorMapAtKey<VM1>();
		nErrorCount += TestVectorMapAtKey<VM2>();
		nErrorCount += TestVectorMapAtKey<VM4>();
		nErrorCount += TestVectorMapAtKey<VM5>();
	}

    {
        // insert at the upper bound of a range
        VMM1 vmm = {{0, 0}};
        VERIFY(vmm.emplace(0, 0) != vmm.begin());
    }


	{ // Misc tests

		// const key_compare& key_comp() const;
		// key_compare&       key_comp();
		VM2       vm;
		const VM2 vmc;

		const VM2::key_compare& kc = vmc.key_comp();
		vm.key_comp() = kc;

		// ensure count can be called from a const object
		vmc.count(0);
	}

	{
		const VMM1 vmm;

		// ensure count can be called from a const object
		vmm.count(0);
	}

	{
		// Misc testing
		typedef eastl::fixed_vector<eastl::pair<int, float>, 8> FV;
		typedef eastl::vector_map<int, float, eastl::less<int>, FV::allocator_type, FV> FixedVectorMap;

		FixedVectorMap fvm;

		for(int i = FV::kMaxSize - 1; i >= 0; i--)
			fvm.insert(eastl::pair<int, float>(i, (float)i));

		FixedVectorMap::iterator it = fvm.find(3);
		EATEST_VERIFY(it != fvm.end());
	}

	{
		// Misc testing
		typedef eastl::fixed_string<char, 16>   KeyStringType;
		typedef eastl::fixed_string<char, 24>   ValueStringType;
		typedef eastl::pair<ValueStringType, bool> StringMapValueType;

		typedef eastl::vector_map<KeyStringType, StringMapValueType> StringMapType;
		StringMapType stringMap;

		stringMap.reserve(20);
		EATEST_VERIFY(stringMap.capacity() == 20);

		StringMapValueType& v1 = stringMap["abc"];
		EATEST_VERIFY(strlen(v1.first.c_str()) == 0);
		v1.first.clear();
		EATEST_VERIFY(strlen(v1.first.c_str()) == 0);

		StringMapValueType& v2 = stringMap["def"];
		EATEST_VERIFY(strlen(v2.first.c_str()) == 0);
		v2.first = "def";
		EATEST_VERIFY(strlen(v2.first.c_str()) == 3);
	}

	{
		// Regression for problem observed in EAWebKit
		typedef eastl::vector_map<eastl::string, void*> TestVectorMap;

		TestVectorMap tvm;

		tvm["Parameters"]      = NULL;
		tvm["ThemeParameters"] = NULL; 
		tvm["CookieInfo"]      = NULL; 
		tvm["DiskCacheInfo"]   = NULL; 
		tvm["RamCacheInfo"]    = NULL;
		tvm["SSLCert"]         = NULL;
		tvm["AllowedDomain"]   = NULL;
	}

	{     // find / find_as / lower_bound / upper_bound
		{ // vector_map
			eastl::vector_map<string, int> vss = {{"abc", 11},   {"def", 22}, {"ghi", 33}, {"jklmnop", 44},
												  {"qrstu", 55}, {"vw", 66},  {"x", 77},   {"yz", 88}};
			VERIFY(vss.find("ghi") != vss.end());
			VERIFY(vss.find_as("GHI", TestStrCmpI_2()) != vss.end());
			VERIFY(vss.lower_bound("ghi") != vss.end());
			VERIFY(vss.upper_bound("ghi") != vss.end());
		}

		{ // const vector_map
			const eastl::vector_map<string, int> vss = {{"abc", 11},   {"def", 22}, {"ghi", 33}, {"jklmnop", 44},
														{"qrstu", 55}, {"vw", 66},  {"x", 77},   {"yz", 88}};
			VERIFY(vss.find("ghi") != vss.end());
			VERIFY(vss.find_as("GHI", TestStrCmpI_2()) != vss.end());
			VERIFY(vss.lower_bound("ghi") != vss.end());
			VERIFY(vss.upper_bound("ghi") != vss.end());
		}

		// vector_multimap
		{
			eastl::vector_multimap<string, int> vss = {{"abc", 11},   {"def", 22}, {"ghi", 33}, {"jklmnop", 44},
													   {"qrstu", 55}, {"vw", 66},  {"x", 77},   {"yz", 88}};
			VERIFY(vss.find_as("GHI", TestStrCmpI_2()) != vss.end());
		}

		// const vector_multimap
		{
			const eastl::vector_multimap<string, int> vss = {{"abc", 11},   {"def", 22}, {"ghi", 33}, {"jklmnop", 44},
															 {"qrstu", 55}, {"vw", 66},  {"x", 77},   {"yz", 88}};
			VERIFY(vss.find_as("GHI", TestStrCmpI_2()) != vss.end());
		}
	}

	return nErrorCount;
}












