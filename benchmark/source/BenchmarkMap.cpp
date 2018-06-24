/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLBenchmark.h"
#include "EASTLTest.h"
#include <EAStdC/EAStopwatch.h>
#include <EASTL/map.h>
#include <EASTL/vector.h>
#include <EASTL/algorithm.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <map>
#include <algorithm>
EA_RESTORE_ALL_VC_WARNINGS()


using namespace EA;


typedef std::map<TestObject, uint32_t>     StdMapTOUint32;
typedef eastl::map<TestObject, uint32_t>   EaMapTOUint32;


namespace
{
	template <typename Container, typename Value>
	void TestInsert(EA::StdC::Stopwatch& stopwatch, Container& c, const Value* pArrayBegin, const Value* pArrayEnd, const Value& highValue)
	{
		stopwatch.Restart();
		c.insert(pArrayBegin, pArrayEnd);
		stopwatch.Stop();
		c.insert(highValue);
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
			Benchmark::DoNothing(c[pArrayBegin->first]);
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
			Benchmark::DoNothing(c.find(pArrayBegin->first)->second);
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
	void TestLowerBound(EA::StdC::Stopwatch& stopwatch, Container& c, const Value* pArrayBegin, const Value* pArrayEnd)
	{
		stopwatch.Restart();
		while(pArrayBegin != pArrayEnd)
		{
			Benchmark::DoNothing(c.lower_bound(pArrayBegin->first)->second);
			++pArrayBegin;
		}
		stopwatch.Stop();
	}


	template <typename Container, typename Value>
	void TestUpperBound(EA::StdC::Stopwatch& stopwatch, Container& c, const Value* pArrayBegin, const Value* pArrayEnd)
	{
		stopwatch.Restart();
		while(pArrayBegin != pArrayEnd)
		{
			Benchmark::DoNothing(c.upper_bound(pArrayBegin->first)->second);
			++pArrayBegin;
		}
		stopwatch.Stop();
	}


	template <typename Container, typename Value>
	void TestEqualRange(EA::StdC::Stopwatch& stopwatch, Container& c, const Value* pArrayBegin, const Value* pArrayEnd)
	{
		stopwatch.Restart();
		while(pArrayBegin != pArrayEnd)
		{
			Benchmark::DoNothing(c.equal_range(pArrayBegin->first).second->second);
			++pArrayBegin;
		}
		stopwatch.Stop();
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
			#if (((defined(_MSC_VER) || defined(_CPPLIB_VER)) && !defined(_HAS_STRICT_CONFORMANCE))) // _CPPLIB_VER is something defined by Dinkumware STL.
				it = c.erase(it);  // Standard behavior.
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



void BenchmarkMap()
{
	EASTLTest_Printf("Map\n");

	EA::UnitTest::Rand  rng(EA::UnitTest::GetRandSeed());
	EA::StdC::Stopwatch stopwatch1(EA::StdC::Stopwatch::kUnitsCPUCycles);
	EA::StdC::Stopwatch stopwatch2(EA::StdC::Stopwatch::kUnitsCPUCycles);

	{
		eastl::vector< std::pair<TestObject, uint32_t> >   stdVector(10000);
		eastl::vector< eastl::pair<TestObject, uint32_t> > eaVector(10000);

		for(eastl_size_t i = 0, iEnd = stdVector.size(); i < iEnd; i++)
		{
			const uint32_t n1 = rng.RandLimit(((uint32_t)iEnd / 2));
			const uint32_t n2 = rng.RandValue();

			stdVector[i] = std::pair<TestObject, uint32_t>(TestObject(n1), n2);
			eaVector[i]  = eastl::pair<TestObject, uint32_t>(TestObject(n1), n2);
		}

		for(int i = 0; i < 2; i++)
		{
			StdMapTOUint32 stdMapTOUint32;
			EaMapTOUint32  eaMapTOUint32;


			///////////////////////////////
			// Test insert(const value_type&)
			///////////////////////////////
			const std::pair<TestObject, uint32_t>   stdHighValue(TestObject(0x7fffffff), 0x7fffffff);
			const eastl::pair<TestObject, uint32_t> eaHighValue(TestObject(0x7fffffff), 0x7fffffff);

			TestInsert(stopwatch1, stdMapTOUint32, stdVector.data(), stdVector.data() + stdVector.size(), stdHighValue);
			TestInsert(stopwatch2,  eaMapTOUint32,  eaVector.data(),  eaVector.data() +  eaVector.size(),  eaHighValue);

			if(i == 1)
				Benchmark::AddResult("map<TestObject, uint32_t>/insert", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test iteration
			///////////////////////////////

			TestIteration(stopwatch1, stdMapTOUint32, StdMapTOUint32::value_type(TestObject(9999999), 9999999));
			TestIteration(stopwatch2,  eaMapTOUint32,  EaMapTOUint32::value_type(TestObject(9999999), 9999999));

			if(i == 1)
				Benchmark::AddResult("map<TestObject, uint32_t>/iteration", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test operator[]
			///////////////////////////////

			TestBracket(stopwatch1, stdMapTOUint32, stdVector.data(), stdVector.data() + stdVector.size());
			TestBracket(stopwatch2,  eaMapTOUint32,  eaVector.data(),  eaVector.data() +  eaVector.size());

			if(i == 1)
				Benchmark::AddResult("map<TestObject, uint32_t>/operator[]", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test find
			///////////////////////////////

			TestFind(stopwatch1, stdMapTOUint32, stdVector.data(), stdVector.data() + stdVector.size());
			TestFind(stopwatch2,  eaMapTOUint32,  eaVector.data(),  eaVector.data() +  eaVector.size());

			if(i == 1)
				Benchmark::AddResult("map<TestObject, uint32_t>/find", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test count
			///////////////////////////////

			TestCount(stopwatch1, stdMapTOUint32, stdVector.data(), stdVector.data() + stdVector.size());
			TestCount(stopwatch2,  eaMapTOUint32,  eaVector.data(),  eaVector.data() +  eaVector.size());

			if(i == 1)
				Benchmark::AddResult("map<TestObject, uint32_t>/count", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test lower_bound
			///////////////////////////////

			TestLowerBound(stopwatch1, stdMapTOUint32, stdVector.data(), stdVector.data() + stdVector.size());
			TestLowerBound(stopwatch2,  eaMapTOUint32,  eaVector.data(),  eaVector.data() +  eaVector.size());

			if(i == 1)
				Benchmark::AddResult("map<TestObject, uint32_t>/lower_bound", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test upper_bound
			///////////////////////////////

			TestUpperBound(stopwatch1, stdMapTOUint32, stdVector.data(), stdVector.data() + stdVector.size());
			TestUpperBound(stopwatch2,  eaMapTOUint32,  eaVector.data(),  eaVector.data() +  eaVector.size());

			if(i == 1)
				Benchmark::AddResult("map<TestObject, uint32_t>/upper_bound", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test equal_range
			///////////////////////////////

			TestEqualRange(stopwatch1, stdMapTOUint32, stdVector.data(), stdVector.data() + stdVector.size());
			TestEqualRange(stopwatch2, eaMapTOUint32,   eaVector.data(),  eaVector.data() +  eaVector.size());

			if(i == 1)
				Benchmark::AddResult("map<TestObject, uint32_t>/equal_range", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test erase(const key_type& key)
			///////////////////////////////

			TestEraseValue(stopwatch1, stdMapTOUint32, stdVector.data(), stdVector.data() + (stdVector.size() / 2));
			TestEraseValue(stopwatch2,  eaMapTOUint32,  eaVector.data(),  eaVector.data() +  (eaVector.size() / 2));

			if(i == 1)
				Benchmark::AddResult("map<TestObject, uint32_t>/erase/key", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test erase(iterator position)
			///////////////////////////////

			TestErasePosition(stopwatch1, stdMapTOUint32);
			TestErasePosition(stopwatch2, eaMapTOUint32);

			if(i == 1)
				Benchmark::AddResult("map<TestObject, uint32_t>/erase/pos", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime(),
										GetStdSTLType() == kSTLMS ? "MS uses a code bloating implementation of erase." : NULL);


			///////////////////////////////
			// Test erase(iterator first, iterator last)
			///////////////////////////////

			TestEraseRange(stopwatch1, stdMapTOUint32);
			TestEraseRange(stopwatch2, eaMapTOUint32);

			if(i == 1)
				Benchmark::AddResult("map<TestObject, uint32_t>/erase/range", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test clear()
			///////////////////////////////

			TestClear(stopwatch1, stdMapTOUint32);
			TestClear(stopwatch2, eaMapTOUint32);

			if(i == 1)
				Benchmark::AddResult("map<TestObject, uint32_t>/clear", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

		}
	}
}















