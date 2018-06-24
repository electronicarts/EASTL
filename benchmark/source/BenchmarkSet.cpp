/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLBenchmark.h"
#include "EASTLTest.h"
#include <EAStdC/EAStopwatch.h>
#include <EASTL/set.h>
#include <EASTL/vector.h>
#include <EASTL/algorithm.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <set>
#include <algorithm>
EA_RESTORE_ALL_VC_WARNINGS()


using namespace EA;


typedef std::set<uint32_t>     StdSetUint32;
typedef eastl::set<uint32_t>   EaSetUint32;


namespace
{
	template <typename Container>
	void TestInsert(EA::StdC::Stopwatch& stopwatch, Container& c, const uint32_t* pArrayBegin, const uint32_t* pArrayEnd)
	{
		stopwatch.Restart();
		c.insert(pArrayBegin, pArrayEnd);
		stopwatch.Stop();

		// Intentionally push back a high uint32_t value. We do this so that 
		// later upper_bound, lower_bound and equal_range never return end().
		c.insert(0xffffffff);
	}


	template <typename Container>
	void TestIteration(EA::StdC::Stopwatch& stopwatch, const Container& c)
	{
		stopwatch.Restart();
		typename Container::const_iterator it = eastl::find(c.begin(), c.end(), uint32_t(9999999));
		stopwatch.Stop();
		if(it != c.end())
			sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)*it);
	}


	template <typename Container>
	void TestFind(EA::StdC::Stopwatch& stopwatch, Container& c, const uint32_t* pArrayBegin, const uint32_t* pArrayEnd)
	{
		uint32_t temp = 0;
		typename Container::iterator it;
		stopwatch.Restart();
		while(pArrayBegin != pArrayEnd)
		{
			it = c.find(*pArrayBegin++);
			temp += *it;
		}
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)temp);
	}


	template <typename Container>
	void TestCount(EA::StdC::Stopwatch& stopwatch, Container& c, const uint32_t* pArrayBegin, const uint32_t* pArrayEnd)
	{
		typename Container::size_type temp = 0;
		stopwatch.Restart();
		while(pArrayBegin != pArrayEnd)
			temp += c.count(*pArrayBegin++);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)temp);
	}


	template <typename Container>
	void TestLowerBound(EA::StdC::Stopwatch& stopwatch, Container& c, const uint32_t* pArrayBegin, const uint32_t* pArrayEnd)
	{
		uint32_t temp = 0;
		typename Container::iterator it;
		stopwatch.Restart();
		while(pArrayBegin != pArrayEnd)
		{
			it = c.lower_bound(*pArrayBegin++);
			temp += *it; // We know that it != end because earlier we inserted 0xffffffff.
		}
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)temp);
	}


	template <typename Container>
	void TestUpperBound(EA::StdC::Stopwatch& stopwatch, Container& c, const uint32_t* pArrayBegin, const uint32_t* pArrayEnd)
	{
		uint32_t temp = 0;
		typename Container::iterator it;
		stopwatch.Restart();
		while(pArrayBegin != pArrayEnd)
		{
			it = c.upper_bound(*pArrayBegin++);
			temp += *it; // We know that it != end because earlier we inserted 0xffffffff.
		}
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)temp);
	}


	template <typename Container>
	void TestEqualRange(EA::StdC::Stopwatch& stopwatch, Container& c, const uint32_t* pArrayBegin, const uint32_t* pArrayEnd)
	{
		uint32_t temp = 0;
		stopwatch.Restart();
		while(pArrayBegin != pArrayEnd)
		{
			temp += *(c.equal_range(*pArrayBegin++).first); // We know that it != end because earlier we inserted 0xffffffff.
		}
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)temp);
	}


	template <typename Container>
	void TestEraseValue(EA::StdC::Stopwatch& stopwatch, Container& c, const uint32_t* pArrayBegin, const uint32_t* pArrayEnd)
	{
		stopwatch.Restart();
		while(pArrayBegin != pArrayEnd)
			c.erase(*pArrayBegin++);
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



void BenchmarkSet()
{
	EASTLTest_Printf("Set\n");

	EA::UnitTest::Rand  rng(EA::UnitTest::GetRandSeed());
	EA::StdC::Stopwatch stopwatch1(EA::StdC::Stopwatch::kUnitsCPUCycles);
	EA::StdC::Stopwatch stopwatch2(EA::StdC::Stopwatch::kUnitsCPUCycles);

	{
		eastl::vector<uint32_t> intVector(10000);
		for(eastl_size_t i = 0, iEnd = intVector.size(); i < iEnd; i++)
			intVector[i] = (uint32_t)rng.RandLimit(((uint32_t)iEnd / 2));  // This will result in duplicates and even a few triplicates.

		for(int i = 0; i < 2; i++)
		{
			StdSetUint32 stdSetUint32;
			EaSetUint32  eaSetUint32;


			///////////////////////////////
			// Test insert(const value_type&)
			///////////////////////////////

			TestInsert(stopwatch1, stdSetUint32, intVector.data(), intVector.data() + intVector.size());
			TestInsert(stopwatch2, eaSetUint32,  intVector.data(), intVector.data() + intVector.size());

			if(i == 1)
				Benchmark::AddResult("set<uint32_t>/insert", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test iteration
			///////////////////////////////

			TestIteration(stopwatch1, stdSetUint32);
			TestIteration(stopwatch2, eaSetUint32);

			if(i == 1)
				Benchmark::AddResult("set<uint32_t>/iteration", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test find
			///////////////////////////////

			TestFind(stopwatch1, stdSetUint32, intVector.data(), intVector.data() + intVector.size());
			TestFind(stopwatch2, eaSetUint32,  intVector.data(), intVector.data() + intVector.size());

			if(i == 1)
				Benchmark::AddResult("set<uint32_t>/find", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test count
			///////////////////////////////

			TestCount(stopwatch1, stdSetUint32, intVector.data(), intVector.data() + intVector.size());
			TestCount(stopwatch2, eaSetUint32,  intVector.data(), intVector.data() + intVector.size());

			if(i == 1)
				Benchmark::AddResult("set<uint32_t>/count", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test lower_bound
			///////////////////////////////

			TestLowerBound(stopwatch1, stdSetUint32, intVector.data(), intVector.data() + intVector.size());
			TestLowerBound(stopwatch2, eaSetUint32,  intVector.data(), intVector.data() + intVector.size());

			if(i == 1)
				Benchmark::AddResult("set<uint32_t>/lower_bound", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test upper_bound
			///////////////////////////////

			TestUpperBound(stopwatch1, stdSetUint32, intVector.data(), intVector.data() + intVector.size());
			TestUpperBound(stopwatch2, eaSetUint32,  intVector.data(), intVector.data() + intVector.size());

			if(i == 1)
				Benchmark::AddResult("set<uint32_t>/upper_bound", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test equal_range
			///////////////////////////////

			TestEqualRange(stopwatch1, stdSetUint32, intVector.data(), intVector.data() + intVector.size());
			TestEqualRange(stopwatch2, eaSetUint32,  intVector.data(), intVector.data() + intVector.size());

			if(i == 1)
				Benchmark::AddResult("set<uint32_t>/equal_range", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test erase(const key_type& key)
			///////////////////////////////

			TestEraseValue(stopwatch1, stdSetUint32, &intVector[0], &intVector[intVector.size() / 2]);
			TestEraseValue(stopwatch2, eaSetUint32,  &intVector[0], &intVector[intVector.size() / 2]);

			if(i == 1)
				Benchmark::AddResult("set<uint32_t>/erase/val", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test erase(iterator position)
			///////////////////////////////

			TestErasePosition(stopwatch1, stdSetUint32);
			TestErasePosition(stopwatch2, eaSetUint32);

			if(i == 1)
				Benchmark::AddResult("set<uint32_t>/erase/pos", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime(),
										GetStdSTLType() == kSTLMS ? "MS uses a code bloating implementation of erase." : NULL);


			///////////////////////////////
			// Test erase(iterator first, iterator last)
			///////////////////////////////

			TestEraseRange(stopwatch1, stdSetUint32);
			TestEraseRange(stopwatch2, eaSetUint32);

			if(i == 1)
				Benchmark::AddResult("set<uint32_t>/erase range", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test clear()
			///////////////////////////////

			TestClear(stopwatch1, stdSetUint32);
			TestClear(stopwatch2, eaSetUint32);

			if(i == 1)
				Benchmark::AddResult("set<uint32_t>/clear", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

		}
	}
}















