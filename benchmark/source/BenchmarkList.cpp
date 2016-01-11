/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLBenchmark.h"
#include "EASTLTest.h"
#include <EAStdC/EAStopwatch.h>
#include <EASTL/list.h>
#include <EASTL/vector.h>
#include <EASTL/algorithm.h>
#include <EASTL/random.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
	#pragma warning(disable: 4555) // expression has no effect; expected expression with side-effect
	#pragma warning(disable: 4350) // behavior change: X called instead of Y
#endif
#include <list>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif


using namespace EA;
using namespace eastl;



typedef std::list<TestObject>   StdListTO;
typedef eastl::list<TestObject> EaListTO;



namespace
{
	void DoNothing(void*)
	{
		// Empty
	}


	template <typename ContainerSource, typename Container>
	void TestCtorIterator(EA::StdC::Stopwatch& stopwatch, const ContainerSource& cs, Container*) // Dummy Container argument because of GCC 2.X limitations.
	{
		stopwatch.Restart();
		Container c(cs.begin(), cs.end());
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c.back().mX);
	}


	template <typename Container>
	void TestCtorN(EA::StdC::Stopwatch& stopwatch, Container*) // Dummy Container argument because of GCC 2.X limitations.
	{
		stopwatch.Restart();
		Container c(10000);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c.back().mX);
	}


	template <typename Container>
	void TestPushBack(EA::StdC::Stopwatch& stopwatch, Container& c, const TestObject* pTOBegin, const TestObject* const pTOEnd)
	{
		stopwatch.Restart();
		while(pTOBegin != pTOEnd)
			c.push_back(*pTOBegin++);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c.back().mX);
	}


	template <typename Container>
	void TestInsert(EA::StdC::Stopwatch& stopwatch, Container& c, const TestObject* pTOBegin, const TestObject* const pTOEnd)
	{
		typename Container::iterator it = c.begin();
		stopwatch.Restart();
		while(pTOBegin != pTOEnd)
		{
			it = c.insert(it, *pTOBegin++);

			if(++it == c.end()) // Try to safely increment the iterator a couple times
				it = c.begin();
			if(++it == c.end())
				it = c.begin();
		}
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c.back().mX);
	}


	template <typename Container>
	void TestSize(EA::StdC::Stopwatch& stopwatch, Container& c, void (*pFunction)(...))
	{
		stopwatch.Restart();
		for(int i = 0; (i < 10000) && c.size(); i++)
			(*pFunction)(&c);
		stopwatch.Stop();
	}


	template <typename Container>
	void TestFind(EA::StdC::Stopwatch& stopwatch, Container& c, const TestObject& to)
	{
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c.size());
		stopwatch.Restart();
		typename Container::iterator it = eastl::find(c.begin(), c.end(), to);
		stopwatch.Stop();
		if(it != c.end())
			sprintf(Benchmark::gScratchBuffer, "%d", (*it).mX);
	}


	template <typename Container>
	void TestReverse(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c.size());
		stopwatch.Restart();
		c.reverse();
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c.back().mX);
	}


	template <typename Container>
	void TestRemove(EA::StdC::Stopwatch& stopwatch, Container& c, const TestObject* pTOBegin, const TestObject* const pTOEnd)
	{
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c.size());
		stopwatch.Restart();
		while(pTOBegin != pTOEnd)
			c.remove(*pTOBegin++);
		stopwatch.Stop();
		if(!c.empty())
			sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c.back().mX);
	}


	template <typename Container>
	void TestSplice(EA::StdC::Stopwatch& stopwatch, Container& c, Container& cSource)
	{
		typename Container::iterator it = c.begin();
		int i = 0, iEnd = (int)cSource.size() - 5;
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c.size());
		stopwatch.Restart();
		while(i++ != iEnd)
			c.splice(it, cSource, cSource.begin());
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c.back().mX);
	}


	template <typename Container>
	void TestErase(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		typename Container::iterator it = c.begin();
		int i = 0, iEnd = (int)c.size() - 5;
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c.size());
		stopwatch.Restart();
		while(i++ != iEnd)
		{
			it = c.erase(it);

			if(it == c.end()) // Try to safely increment the iterator a couple times
				it = c.begin();
			if(++it == c.end())
				it = c.begin();
		}
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)c.back().mX);
	}

} // namespace




void BenchmarkList()
{
	EASTLTest_Printf("List\n");

	EASTLTest_Rand      rng(EA::UnitTest::GetRandSeed());
	EA::StdC::Stopwatch stopwatch1(EA::StdC::Stopwatch::kUnitsCPUCycles);
	EA::StdC::Stopwatch stopwatch2(EA::StdC::Stopwatch::kUnitsCPUCycles);

	EaListTO  eaListTO_1(1);
	EaListTO  eaListTO_10(10);
	EaListTO  eaListTO_100(100);
	StdListTO stdListTO_1(1);
	StdListTO stdListTO_10(10);
	StdListTO stdListTO_100(100);

	{
		char buffer[32];
		sprintf(buffer, "%p", &DoNothing);
	}

	{
		eastl::vector<TestObject> toVector(100000);
		for(eastl_size_t i = 0, iEnd = toVector.size(); i < iEnd; ++i)
			toVector[i] = TestObject((int)i);
		random_shuffle(toVector.begin(), toVector.end(), rng);


		for(int i = 0; i < 2; i++)
		{
			StdListTO stdListTO;
			EaListTO  eaListTO;


			///////////////////////////////
			// Test list(InputIterator first, InputIterator last)
			///////////////////////////////

			TestCtorIterator(stopwatch1, toVector, &stdListTO);
			TestCtorIterator(stopwatch2, toVector, &eaListTO);

			if(i == 1)
				Benchmark::AddResult("list<TestObject>/ctor(it)", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());



			///////////////////////////////
			// Test list(size_type n)
			///////////////////////////////

			TestCtorN(stopwatch1, &stdListTO);
			TestCtorN(stopwatch2, &eaListTO);

			if(i == 1)
				Benchmark::AddResult("list<TestObject>/ctor(n)", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());




			///////////////////////////////
			// Test push_back()
			///////////////////////////////

			TestPushBack(stopwatch1, stdListTO, toVector.data(), toVector.data() + toVector.size());
			TestPushBack(stopwatch2,  eaListTO, toVector.data(), toVector.data() + toVector.size());

			if(i == 1)
				Benchmark::AddResult("list<TestObject>/push_back", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());




			///////////////////////////////
			// Test insert()
			///////////////////////////////

			TestInsert(stopwatch1, stdListTO, toVector.data(), toVector.data() + toVector.size());
			TestInsert(stopwatch2,  eaListTO, toVector.data(), toVector.data() + toVector.size());

			if(i == 1)
				Benchmark::AddResult("list<TestObject>/insert", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());




			///////////////////////////////
			// Test size()
			///////////////////////////////

			TestSize(stopwatch1, stdListTO_1, Benchmark::DoNothing);
			TestSize(stopwatch2,  eaListTO_1, Benchmark::DoNothing);

			if(i == 1)
				Benchmark::AddResult("list<TestObject>/size/1", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestSize(stopwatch1, stdListTO_10, Benchmark::DoNothing);
			TestSize(stopwatch2,  eaListTO_10, Benchmark::DoNothing);

			if(i == 1)
				Benchmark::AddResult("list<TestObject>/size/10", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime()
									#if !EASTL_LIST_SIZE_CACHE
										, "EASTL is configured to not cache the list size."
									#endif
									);

			TestSize(stopwatch1, stdListTO_100, Benchmark::DoNothing);
			TestSize(stopwatch2,  eaListTO_100, Benchmark::DoNothing);

			if(i == 1)
				Benchmark::AddResult("list<TestObject>/size/100", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime()
									#if !EASTL_LIST_SIZE_CACHE
										, "EASTL is configured to not cache the list size."
									#endif
									);



			///////////////////////////////
			// Test find()
			///////////////////////////////

			TestFind(stopwatch1, stdListTO, TestObject(99999999));
			TestFind(stopwatch2,  eaListTO, TestObject(99999999));

			if(i == 1)
				Benchmark::AddResult("list<TestObject>/find", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());




			///////////////////////////////
			// Test reverse()
			///////////////////////////////

			TestReverse(stopwatch1, stdListTO);
			TestReverse(stopwatch2,  eaListTO);

			if(i == 1)
				Benchmark::AddResult("list<TestObject>/reverse", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());




			///////////////////////////////
			// Test remove()
			///////////////////////////////

			random_shuffle(toVector.begin(), toVector.end(), rng);
			TestRemove(stopwatch1, stdListTO, &toVector[0], &toVector[20]);
			TestRemove(stopwatch2,  eaListTO, &toVector[0], &toVector[20]);

			if(i == 1)
				Benchmark::AddResult("list<TestObject>/remove", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());




			///////////////////////////////
			// Test splice()
			///////////////////////////////
			StdListTO listCopyStd(stdListTO);
			EaListTO  listCopyEa(eaListTO);

			TestSplice(stopwatch1, stdListTO, listCopyStd);
			TestSplice(stopwatch2,  eaListTO, listCopyEa);

			if(i == 1)
				Benchmark::AddResult("list<TestObject>/splice", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());




			///////////////////////////////
			// Test erase()
			///////////////////////////////

			TestErase(stopwatch1, stdListTO);
			TestErase(stopwatch2,  eaListTO);

			if(i == 1)
				Benchmark::AddResult("list<TestObject>/erase", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
		}
	}
}






















