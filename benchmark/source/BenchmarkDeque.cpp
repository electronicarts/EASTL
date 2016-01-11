/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLBenchmark.h"
#include "EASTLTest.h"
#include <EAStdC/EAStopwatch.h>
#include <EASTL/algorithm.h>
#include <EASTL/deque.h>
#include <EASTL/vector.h>
#include <EASTL/sort.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
	#pragma warning(disable: 4350) // behavior change: X called instead of Y
#endif
#include <algorithm>
#include <vector>
#include <deque>
#include <stdio.h>
#include <stdlib.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif


using namespace EA;


namespace
{
	struct ValuePair
	{
		uint32_t key;
		uint32_t v;
	};

	struct VPCompare
	{
		bool operator()(const ValuePair& vp1, const ValuePair& vp2) const
		{
			return (vp1.key == vp2.key) ? (vp1.v < vp2.v) : (vp1.key < vp2.key);
		}
	};

	bool operator<(const ValuePair& vp1, const ValuePair& vp2)
	{
		return (vp1.key == vp2.key) ? (vp1.v < vp2.v) : (vp1.key < vp2.key);
	}

	bool operator==(const ValuePair& vp1, const ValuePair& vp2)
	{
		return (vp1.key == vp2.key) && (vp1.v == vp2.v);
	}
}


EASTL_DECLARE_POD(ValuePair)
EASTL_DECLARE_TRIVIAL_CONSTRUCTOR(ValuePair)
EASTL_DECLARE_TRIVIAL_COPY(ValuePair)
EASTL_DECLARE_TRIVIAL_ASSIGN(ValuePair)
EASTL_DECLARE_TRIVIAL_DESTRUCTOR(ValuePair)
EASTL_DECLARE_TRIVIAL_RELOCATE(ValuePair)



typedef std::deque<ValuePair>   StdDeque;
typedef eastl::deque<ValuePair, EASTLAllocatorType, 128> EaDeque;  // What value do we pick for the subarray size to make the comparison fair? Using the default isn't ideal because it results in this test measuring speed efficiency and ignoring memory efficiency. 



namespace
{
	template <typename Container>
	void TestPushBack(EA::StdC::Stopwatch& stopwatch, Container& c, eastl::vector<uint32_t>& intVector)
	{
		stopwatch.Restart();
		for(eastl_size_t j = 0, jEnd = intVector.size(); j < jEnd; j++)
		{
			const ValuePair vp = { intVector[j], intVector[j] };
			c.push_back(vp);
		}
		stopwatch.Stop();
	}


	template <typename Container>
	void TestPushFront(EA::StdC::Stopwatch& stopwatch, Container& c, eastl::vector<uint32_t>& intVector)
	{
		stopwatch.Restart();
		for(eastl_size_t j = 0, jEnd = intVector.size(); j < jEnd; j++)
		{
			const ValuePair vp = { intVector[j], intVector[j] };
			c.push_front(vp);
		}
		stopwatch.Stop();
	}


	template <typename Container>
	void TestBracket(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		uint64_t temp = 0;
		stopwatch.Restart();
		for(typename Container::size_type j = 0, jEnd = c.size(); j < jEnd; j++)
			temp += c[j].key;
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)(temp & 0xffffffff));
	}


	template <typename Container>
	void TestIteration(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		typename Container::iterator it = c.begin(), itEnd = c.end();
		stopwatch.Restart();
		while(it != itEnd)
			++it;
		stopwatch.Stop();
		if(it != c.end())
			sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)(*it).key);

		/* Alternative way to measure:
		const eastl_size_t n = c.size();
		stopwatch.Restart();
		for(eastl_size_t i = 0; i < n; ++i)
			++it;
		stopwatch.Stop();
		if(it != c.end())
			sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)(*it).key);
		*/
	}


	template <typename Container>
	void TestFind(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		// Intentionally use eastl find in order to measure just  
		// vector access speed and not be polluted by sort speed.
		const ValuePair vp = { 0xffffffff, 0 };
		stopwatch.Restart();
		typename Container::iterator it = eastl::find(c.begin(), c.end(), vp);
		stopwatch.Stop();
		if(it != c.end())
			sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)(*it).key);
	}


	template <typename Container>
	void TestSort(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		// Intentionally use eastl sort in order to measure just  
		// vector access speed and not be polluted by sort speed.
		VPCompare vpCompare;
		stopwatch.Restart();
		eastl::quick_sort(c.begin(), c.end(), vpCompare);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c[0].key);
	}


	template <typename Container>
	void TestInsert(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		const ValuePair               vp = { 0xffffffff, 0 };
		typename Container::size_type j, jEnd;
		typename Container::iterator  it;

		stopwatch.Restart();
		for(j = 0, jEnd = 2000, it = c.begin(); j < jEnd; ++j)
		{
			it = c.insert(it, vp);

			if(it == c.end()) // Try to safely increment the iterator three times.
				it = c.begin();
			if(++it == c.end())
				it = c.begin();
			if(++it == c.end())
				it = c.begin();
		}
		stopwatch.Stop();
	}


	template <typename Container>
	void TestErase(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		typename Container::size_type j, jEnd;
		typename Container::iterator  it;

		stopwatch.Restart();
		for(j = 0, jEnd = 2000, it = c.begin(); j < jEnd; ++j)
		{
			it = c.erase(it);

			if(it == c.end()) // Try to safely increment the iterator three times.
				it = c.begin();
			if(++it == c.end())
				it = c.begin();
			if(++it == c.end())
				it = c.begin();
		}
		stopwatch.Stop();
	}

} // namespace



void BenchmarkDeque()
{
	EASTLTest_Printf("Deque\n");

	EA::UnitTest::RandGenT<uint32_t> rng(EA::UnitTest::GetRandSeed());
	EA::StdC::Stopwatch              stopwatch1(EA::StdC::Stopwatch::kUnitsCPUCycles);
	EA::StdC::Stopwatch              stopwatch2(EA::StdC::Stopwatch::kUnitsCPUCycles);

	{ // Exercise some declarations
		int nErrorCount = 0;
		ValuePair vp1 = { 0, 0 }, vp2 = { 0, 0 };
		VPCompare c1, c2;

		VERIFY(c1.operator()(vp1, vp2) == c2.operator()(vp1, vp2));
		VERIFY((vp1 < vp2) || (vp1 == vp2) || !(vp1 == vp2));
	}

	{
		eastl::vector<uint32_t> intVector(100000);
		eastl::generate(intVector.begin(), intVector.end(), rng);

		for(int i = 0; i < 2; i++)
		{
			StdDeque stdDeque;
			EaDeque  eaDeque;


			///////////////////////////////
			// Test push_back
			///////////////////////////////

			TestPushBack(stopwatch1, stdDeque, intVector);
			TestPushBack(stopwatch2,  eaDeque, intVector);

			if(i == 1)
				Benchmark::AddResult("deque<ValuePair>/push_back", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test push_front
			///////////////////////////////

			TestPushFront(stopwatch1, stdDeque, intVector);
			TestPushFront(stopwatch2,  eaDeque, intVector);

			if(i == 1)
				Benchmark::AddResult("deque<ValuePair>/push_front", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test operator[]
			///////////////////////////////

			TestBracket(stopwatch1, stdDeque);
			TestBracket(stopwatch2,  eaDeque);

			if(i == 1)
				Benchmark::AddResult("deque<ValuePair>/operator[]", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test iteration
			///////////////////////////////

			TestIteration(stopwatch1, stdDeque);
			TestIteration(stopwatch2,  eaDeque);

			if(i == 1)
				Benchmark::AddResult("deque<ValuePair>/iteration", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test find()
			///////////////////////////////

			TestFind(stopwatch1, stdDeque);
			TestFind(stopwatch2,  eaDeque);

			if(i == 1)
				Benchmark::AddResult("deque<ValuePair>/find", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test sort
			///////////////////////////////

			// Currently VC++ complains about our sort function decrementing std::iterator that is already at begin(). In the strictest sense,
			// that's a valid complaint, but we aren't testing std STL here. We will want to revise our sort function eventually.
			#if !defined(_MSC_VER) || !defined(_ITERATOR_DEBUG_LEVEL) || (_ITERATOR_DEBUG_LEVEL < 2)
				TestSort(stopwatch1, stdDeque);
				TestSort(stopwatch2,  eaDeque);

				if(i == 1)
					Benchmark::AddResult("deque<ValuePair>/sort", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
			#endif


			///////////////////////////////
			// Test insert
			///////////////////////////////

			TestInsert(stopwatch1, stdDeque);
			TestInsert(stopwatch2,  eaDeque);

			if(i == 1)
				Benchmark::AddResult("deque<ValuePair>/insert", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test erase
			///////////////////////////////

			TestErase(stopwatch1, stdDeque);
			TestErase(stopwatch2,  eaDeque);

			if(i == 1)
				Benchmark::AddResult("deque<ValuePair>/erase", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
		}
	}
}












