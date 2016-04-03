/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLBenchmark.h"
#include "EASTLTest.h"
#include <EAStdC/EAStopwatch.h>
#include <EASTL/vector.h>
#include <EASTL/hash_map.h>
#include <EASTL/string.h>
#include <EASTL/algorithm.h>


// STD_HASH_SUPPORTED
//
// Defined as 0 or 1. identifies if hash_map (pre-C++11 container type) is supported.
// 
#ifndef STD_HASH_SUPPORTED
	// Apache STL doesn't have any kind of hash containers.
	#if defined(_RWSTD_VER_STR)
		#define STD_HASH_SUPPORTED 0
	#elif defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4006) // To consider: Find a better way to identify the switchover from pre-C++11 hash containers to C++11 hash containers.
		#define STD_HASH_SUPPORTED 0 // Need to use std::unordered_map, etc. instead of hash_map, etc.
	#elif defined(EA_COMPILER_CLANG) && (EA_COMPILER_CPP11_ENABLED) // Bad way to determine if we have C++11 hash containers
		#define STD_HASH_SUPPORTED 0
	#else
		#define STD_HASH_SUPPORTED 1
	#endif
#endif


#ifdef _MSC_VER
	#pragma warning(push, 0)
	#pragma warning(disable: 4555) // expression has no effect; expected expression with side-effect
	#pragma warning(disable: 4350) // behavior change: X called instead of Y
#endif
#if STD_HASH_SUPPORTED
	#if (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)) && (defined(_CXXCONFIG) || defined(EA_PLATFORM_UNIX) || defined(EA_PLATFORM_MINGW))
		#include <ext/hash_map>
	#else
		#include <hash_map>
	#endif
#endif
#include <string>
#include <algorithm>
#include <stdio.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif



using namespace EA;


// HashString8
//
// We define a string
//
template <typename String>
struct HashString8
{
	// Defined for EASTL, STLPort, SGI, etc. and Metrowerks-related hash tables:
	size_t operator()(const String& s) const 
	{ 
		const uint8_t* p = (const uint8_t*) s.c_str();
		uint32_t c, stringHash = UINT32_C(2166136261);
		while((c = *p++) != 0)
			stringHash = (stringHash * 16777619) ^ c;
		return stringHash;
	}

	// Defined for Dinkumware-related (e.g. MS STL) hash tables:
	bool operator()(const String& s1, const String& s2) const
	{
		return s1 < s2;
	}

	// Defined for Dinkumware-related (e.g. MS STL) hash tables:
	enum {
		bucket_size = 7,
		min_buckets = 8
	};
};


#if STD_HASH_SUPPORTED
	#if defined(_STLPORT_VERSION) // STLPort is an modular add-on that can override any other STL.
		typedef std::hash_map<uint32_t, TestObject>     StdMapUint32TO;
		typedef eastl::hash_map<uint32_t, TestObject>   EaMapUint32TO;

		typedef std::hash_map<std::string, uint32_t, HashString8<std::string> >       StdMapStrUint32;
		typedef eastl::hash_map<eastl::string, uint32_t, HashString8<eastl::string> > EaMapStrUint32;

	#elif defined(_MSC_VER) 
		typedef stdext::hash_map<uint32_t, TestObject>  StdMapUint32TO;
		typedef eastl::hash_map<uint32_t, TestObject>   EaMapUint32TO;

		typedef stdext::hash_map<std::string, uint32_t, HashString8<std::string> >    StdMapStrUint32;
		typedef eastl::hash_map<eastl::string, uint32_t, HashString8<eastl::string> > EaMapStrUint32;

	#elif defined(_YVALS) // Dinkumware STL.
		typedef std::hash_map<uint32_t, TestObject>     StdMapUint32TO;
		typedef eastl::hash_map<uint32_t, TestObject>   EaMapUint32TO;

		typedef std::hash_map<std::string, uint32_t, HashString8<std::string> >       StdMapStrUint32;
		typedef eastl::hash_map<eastl::string, uint32_t, HashString8<eastl::string> > EaMapStrUint32;

	#elif defined(__GNUC__) && (defined(_CXXCONFIG) || defined(_GLIBCXX_STD) || defined(EA_PLATFORM_UNIX)) // GCC using libstdc++
		typedef __gnu_cxx::hash_map<uint32_t, TestObject> StdMapUint32TO;
		typedef eastl::hash_map<uint32_t, TestObject>     EaMapUint32TO;

		typedef __gnu_cxx::hash_map<std::string, uint32_t, HashString8<std::string> > StdMapStrUint32;
		typedef eastl::hash_map<eastl::string, uint32_t, HashString8<eastl::string> > EaMapStrUint32;

	#else // Else generic hash map similar to that proposed for the C++ standard. 
		typedef std::hash_map<uint32_t, TestObject>     StdMapUint32TO;
		typedef eastl::hash_map<uint32_t, TestObject>   EaMapUint32TO;

		typedef std::hash_map<std::string, uint32_t, HashString8<std::string> >       StdMapStrUint32;
		typedef eastl::hash_map<eastl::string, uint32_t, HashString8<eastl::string> > EaMapStrUint32;

	#endif
#endif




namespace
{
	template <typename Container, typename Value>
	void TestInsert(EA::StdC::Stopwatch& stopwatch, Container& c, const Value* pArrayBegin, const Value* pArrayEnd)
	{
		stopwatch.Restart();
		c.insert(pArrayBegin, pArrayEnd);
		stopwatch.Stop();
	}


	template <typename Container, typename Value>
	void TestIteration(EA::StdC::Stopwatch& stopwatch, const Container& c, const Value& findValue)
	{
		stopwatch.Restart();
		typename Container::const_iterator it = eastl::find(c.begin(), c.end(), findValue); // It shouldn't matter what find implementation we use here, as it merely iterates values.
		stopwatch.Stop();
		if(it != c.end())
			sprintf(Benchmark::gScratchBuffer, "%p", &*it);
	}


	template <typename Container, typename Value>
	void TestBracket(EA::StdC::Stopwatch& stopwatch, Container& c, const Value* pArrayBegin, const Value* pArrayEnd)
	{
		stopwatch.Restart();
		while(pArrayBegin != pArrayEnd)
		{
			Benchmark::DoNothing(&c[pArrayBegin->first]);
			++pArrayBegin;
		}
		stopwatch.Stop();
	}


	template <typename Container, typename Value>
	void TestFind(EA::StdC::Stopwatch& stopwatch, Container& c, const Value* pArrayBegin, const Value* pArrayEnd)
	{
		stopwatch.Restart();
		while(pArrayBegin != pArrayEnd)
		{
			typename Container::iterator it = c.find(pArrayBegin->first);
			Benchmark::DoNothing(&it);
			++pArrayBegin;
		}
		stopwatch.Stop();
	}


	template <typename Container, typename Value>
	void TestFindAsStd(EA::StdC::Stopwatch& stopwatch, Container& c, const Value* pArrayBegin, const Value* pArrayEnd)
	{
		stopwatch.Restart();
		while(pArrayBegin != pArrayEnd)
		{
			typename Container::iterator it = c.find(pArrayBegin->first.c_str());
			Benchmark::DoNothing(&it);
			++pArrayBegin;
		}
		stopwatch.Stop();
	}


	template <typename Container, typename Value>
	void TestFindAsEa(EA::StdC::Stopwatch& stopwatch, Container& c, const Value* pArrayBegin, const Value* pArrayEnd)
	{
		stopwatch.Restart();
		while(pArrayBegin != pArrayEnd)
		{
			typename Container::iterator it = c.find_as(pArrayBegin->first.c_str());
			Benchmark::DoNothing(&it);
			++pArrayBegin;
		}
		stopwatch.Stop();
	}


	template <typename Container, typename Value>
	void TestCount(EA::StdC::Stopwatch& stopwatch, Container& c, const Value* pArrayBegin, const Value* pArrayEnd)
	{
		typename Container::size_type temp = 0;
		stopwatch.Restart();
		while(pArrayBegin != pArrayEnd)
		{
			temp += c.count(pArrayBegin->first);
			++pArrayBegin;
		}
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)temp);
	}


	template <typename Container, typename Value>
	void TestEraseValue(EA::StdC::Stopwatch& stopwatch, Container& c, const Value* pArrayBegin, const Value* pArrayEnd)
	{
		stopwatch.Restart();
		while(pArrayBegin != pArrayEnd)
		{
			c.erase(pArrayBegin->first);
			++pArrayBegin;
		}
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c.size());
	}


	template <typename Container>
	void TestErasePosition(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		typename Container::size_type j, jEnd;
		typename Container::iterator it;

		stopwatch.Restart();
		for(j = 0, jEnd = c.size() / 3, it = c.begin(); j < jEnd; ++j)
		{
			// The erase fucntion is supposed to return an iterator, but the C++ standard was 
			// not initially clear about it and some STL implementations don't do it correctly.
			#if (defined(_MSC_VER) || defined(_YVALS)) // yvals is something defined by Dinkumware STL.
				it = c.erase(it);
			#else
				// This pathway may execute at a slightly different speed than the 
				// standard behaviour, but that's fine for the benchmark because the
				// benchmark is measuring the speed of erasing while iterating, and 
				// however it needs to get done by the given STL is how it is measured.
				const typename Container::iterator itErase(it++);
				c.erase(itErase);
			#endif

			++it;
			++it;
		}

		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%p %p", &c, &it);
	}


	template <typename Container>
	void TestEraseRange(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		typename Container::size_type j, jEnd;
		typename Container::iterator  it1 = c.begin();
		typename Container::iterator  it2 = c.begin();

		for(j = 0, jEnd = c.size() / 3; j < jEnd; ++j)
			++it2;

		stopwatch.Restart();
		c.erase(it1, it2);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%p %p %p", &c, &it1, &it2);
	}


	template <typename Container>
	void TestClear(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		stopwatch.Restart();
		c.clear();
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c.size());
	}


} // namespace



void BenchmarkHash()
{
	#if STD_HASH_SUPPORTED
		EASTLTest_Printf("HashMap\n");

		EA::UnitTest::Rand  rng(EA::UnitTest::GetRandSeed());
		EA::StdC::Stopwatch stopwatch1(EA::StdC::Stopwatch::kUnitsCPUCycles);
		EA::StdC::Stopwatch stopwatch2(EA::StdC::Stopwatch::kUnitsCPUCycles);

		{
			eastl::vector<   std::pair<uint32_t, TestObject> > stdVectorUT(10000);
			eastl::vector< eastl::pair<uint32_t, TestObject> >  eaVectorUT(10000);

			eastl::vector<   std::pair<  std::string, uint32_t> > stdVectorSU(10000);
			eastl::vector< eastl::pair<eastl::string, uint32_t> >  eaVectorSU(10000);

			for(eastl_size_t i = 0, iEnd = stdVectorUT.size(); i < iEnd; i++)
			{
				const uint32_t n1 = rng.RandLimit((uint32_t)(iEnd / 2));
				const uint32_t n2 = rng.RandValue();

				stdVectorUT[i] =   std::pair<uint32_t, TestObject>(n1, TestObject(n2));
				eaVectorUT[i]  = eastl::pair<uint32_t, TestObject>(n1, TestObject(n2));

				char str_n1[32];
				sprintf(str_n1, "%u", (unsigned)n1);

				stdVectorSU[i] =   std::pair<  std::string, uint32_t>(  std::string(str_n1), n2);
				eaVectorSU[i]  = eastl::pair<eastl::string, uint32_t>(eastl::string(str_n1), n2);
			}

			for(int i = 0; i < 2; i++)
			{
				StdMapUint32TO  stdMapUint32TO;
				EaMapUint32TO   eaMapUint32TO;

				StdMapStrUint32 stdMapStrUint32;
				EaMapStrUint32  eaMapStrUint32;


				///////////////////////////////
				// Test insert(const value_type&)
				///////////////////////////////

				TestInsert(stopwatch1, stdMapUint32TO, stdVectorUT.data(), stdVectorUT.data() + stdVectorUT.size());
				TestInsert(stopwatch2, eaMapUint32TO,   eaVectorUT.data(),  eaVectorUT.data() +  eaVectorUT.size());

				if(i == 1)
					Benchmark::AddResult("hash_map<uint32_t, TestObject>/insert", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

				TestInsert(stopwatch1, stdMapStrUint32, stdVectorSU.data(), stdVectorSU.data() + stdVectorSU.size());
				TestInsert(stopwatch2, eaMapStrUint32,    eaVectorSU.data(),  eaVectorSU.data() +  eaVectorSU.size());

				if(i == 1)
					Benchmark::AddResult("hash_map<string, uint32_t>/insert", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


				///////////////////////////////
				// Test iteration
				///////////////////////////////

				TestIteration(stopwatch1, stdMapUint32TO, StdMapUint32TO::value_type(9999999, TestObject(9999999)));
				TestIteration(stopwatch2,  eaMapUint32TO,  EaMapUint32TO::value_type(9999999, TestObject(9999999)));

				if(i == 1)
					Benchmark::AddResult("hash_map<uint32_t, TestObject>/iteration", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

				TestIteration(stopwatch1, stdMapStrUint32, StdMapStrUint32::value_type(  std::string("9999999"), 9999999));
				TestIteration(stopwatch2,  eaMapStrUint32,  EaMapStrUint32::value_type(eastl::string("9999999"), 9999999));

				if(i == 1)
					Benchmark::AddResult("hash_map<string, uint32_t>/iteration", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


				///////////////////////////////
				// Test operator[]
				///////////////////////////////

				TestBracket(stopwatch1, stdMapUint32TO, stdVectorUT.data(), stdVectorUT.data() + stdVectorUT.size());
				TestBracket(stopwatch2, eaMapUint32TO,   eaVectorUT.data(),  eaVectorUT.data() +  eaVectorUT.size());

				if(i == 1)
					Benchmark::AddResult("hash_map<uint32_t, TestObject>/operator[]", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

				TestBracket(stopwatch1, stdMapStrUint32, stdVectorSU.data(), stdVectorSU.data() + stdVectorSU.size());
				TestBracket(stopwatch2, eaMapStrUint32,    eaVectorSU.data(),  eaVectorSU.data() +  eaVectorSU.size());

				if(i == 1)
					Benchmark::AddResult("hash_map<string, uint32_t>/operator[]", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


				///////////////////////////////
				// Test find
				///////////////////////////////

				TestFind(stopwatch1, stdMapUint32TO, stdVectorUT.data(), stdVectorUT.data() + stdVectorUT.size());
				TestFind(stopwatch2, eaMapUint32TO,   eaVectorUT.data(),  eaVectorUT.data() +  eaVectorUT.size());

				if(i == 1)
					Benchmark::AddResult("hash_map<uint32_t, TestObject>/find", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

				TestFind(stopwatch1, stdMapStrUint32, stdVectorSU.data(), stdVectorSU.data() + stdVectorSU.size());
				TestFind(stopwatch2, eaMapStrUint32,    eaVectorSU.data(),  eaVectorSU.data() +  eaVectorSU.size());

				if(i == 1)
					Benchmark::AddResult("hash_map<string, uint32_t>/find", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


				///////////////////////////////
				// Test find_as
				///////////////////////////////

				TestFindAsStd(stopwatch1, stdMapStrUint32, stdVectorSU.data(), stdVectorSU.data() + stdVectorSU.size());
				TestFindAsEa(stopwatch2, eaMapStrUint32,    eaVectorSU.data(),  eaVectorSU.data() +  eaVectorSU.size());

				if(i == 1)
					Benchmark::AddResult("hash_map<string, uint32_t>/find_as/char*", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


				///////////////////////////////
				// Test count
				///////////////////////////////

				TestCount(stopwatch1, stdMapUint32TO, stdVectorUT.data(), stdVectorUT.data() + stdVectorUT.size());
				TestCount(stopwatch2, eaMapUint32TO,   eaVectorUT.data(),  eaVectorUT.data() +  eaVectorUT.size());

				if(i == 1)
					Benchmark::AddResult("hash_map<uint32_t, TestObject>/count", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

				TestCount(stopwatch1, stdMapStrUint32, stdVectorSU.data(), stdVectorSU.data() + stdVectorSU.size());
				TestCount(stopwatch2, eaMapStrUint32,   eaVectorSU.data(),  eaVectorSU.data() +  eaVectorSU.size());

				if(i == 1)
					Benchmark::AddResult("hash_map<string, uint32_t>/count", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


				///////////////////////////////
				// Test erase(const key_type& key)
				///////////////////////////////

				TestEraseValue(stopwatch1, stdMapUint32TO, stdVectorUT.data(), stdVectorUT.data() + (stdVectorUT.size() / 2));
				TestEraseValue(stopwatch2, eaMapUint32TO,   eaVectorUT.data(),  eaVectorUT.data() +  (eaVectorUT.size() / 2));

				if(i == 1)
					Benchmark::AddResult("hash_map<uint32_t, TestObject>/erase val", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

				TestEraseValue(stopwatch1, stdMapStrUint32, stdVectorSU.data(), stdVectorSU.data() + (stdVectorSU.size() / 2));
				TestEraseValue(stopwatch2, eaMapStrUint32,   eaVectorSU.data(),  eaVectorSU.data() +  (eaVectorSU.size() / 2));

				if(i == 1)
					Benchmark::AddResult("hash_map<string, uint32_t>/erase val", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


				///////////////////////////////
				// Test erase(iterator position)
				///////////////////////////////

				TestErasePosition(stopwatch1, stdMapUint32TO);
				TestErasePosition(stopwatch2, eaMapUint32TO);

				if(i == 1)
					Benchmark::AddResult("hash_map<uint32_t, TestObject>/erase pos", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

				TestErasePosition(stopwatch1, stdMapStrUint32);
				TestErasePosition(stopwatch2, eaMapStrUint32);

				if(i == 1)
					Benchmark::AddResult("hash_map<string, uint32_t>/erase pos", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


				///////////////////////////////
				// Test erase(iterator first, iterator last)
				///////////////////////////////

				TestEraseRange(stopwatch1, stdMapUint32TO);
				TestEraseRange(stopwatch2, eaMapUint32TO);

				if(i == 1)
					Benchmark::AddResult("hash_map<uint32_t, TestObject>/erase range", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

				TestEraseRange(stopwatch1, stdMapStrUint32);
				TestEraseRange(stopwatch2, eaMapStrUint32);

				if(i == 1)
					Benchmark::AddResult("hash_map<string, uint32_t>/erase range", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


				///////////////////////////////
				// Test clear()
				///////////////////////////////

				// Clear the containers of whatever they happen to have. We want the containers to have full data.
				TestClear(stopwatch1, stdMapUint32TO);
				TestClear(stopwatch2, eaMapUint32TO);
				TestClear(stopwatch1, stdMapStrUint32);
				TestClear(stopwatch2, eaMapStrUint32);

				// Re-set the containers with full data.
				TestInsert(stopwatch1, stdMapUint32TO, stdVectorUT.data(), stdVectorUT.data() + stdVectorUT.size());
				TestInsert(stopwatch2, eaMapUint32TO,   eaVectorUT.data(),  eaVectorUT.data() +  eaVectorUT.size());
				TestInsert(stopwatch1, stdMapStrUint32, stdVectorSU.data(), stdVectorSU.data() + stdVectorSU.size());
				TestInsert(stopwatch2, eaMapStrUint32,   eaVectorSU.data(),  eaVectorSU.data() +  eaVectorSU.size());

				// Now clear the data again, this time measuring it.
				TestClear(stopwatch1, stdMapUint32TO);
				TestClear(stopwatch2, eaMapUint32TO);

				if(i == 1)
					Benchmark::AddResult("hash_map<uint32_t, TestObject>/clear", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

				TestClear(stopwatch1, stdMapStrUint32);
				TestClear(stopwatch2, eaMapStrUint32);

				if(i == 1)
					Benchmark::AddResult("hash_map<string, uint32_t>/clear", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			}
		}
	#else
		EASTLTest_Printf("HashMap...Unsupported by the tested std STL.\n");
	#endif
}


















