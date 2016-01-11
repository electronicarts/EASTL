/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLBenchmark.h"
#include "EASTLTest.h"
#include <EAStdC/EAStopwatch.h>
#include <EASTL/heap.h>
#include <EASTL/vector.h>
#include <EASTL/algorithm.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
	#pragma warning(disable: 4350) // behavior change: X called instead of Y
#endif
#include <algorithm>
#include <vector>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif


using namespace EA;


namespace
{
	template <typename Iterator>
	void TestMakeHeapStd(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator last)
	{
		stopwatch.Restart();
		std::make_heap(first, last);
		stopwatch.Stop();
	}

	template <typename Iterator>
	void TestMakeHeapEa(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator last)
	{
		stopwatch.Restart();
		eastl::make_heap(first, last);
		stopwatch.Stop();
	}



	template <typename Iterator1, typename Iterator2>
	void TestPushHeapStd(EA::StdC::Stopwatch& stopwatch, Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2)
	{
		stopwatch.Restart();
		while(first2 != last2)
		{
			*last1++ = *first2++;
			std::push_heap(first1, last1);
		}
		stopwatch.Stop();
	}

	template <typename Iterator1, typename Iterator2>
	void TestPushHeapEa(EA::StdC::Stopwatch& stopwatch, Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2)
	{
		stopwatch.Restart();
		while(first2 != last2)
		{
			*last1++ = *first2++;
			eastl::push_heap(first1, last1);
		}
		stopwatch.Stop();
	}



	template <typename Iterator>
	void TestPopHeapStd(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator last, Iterator popEnd)
	{
		stopwatch.Restart();
		while(last != popEnd)
			std::pop_heap(first, last--);
		stopwatch.Stop();
	}

	template <typename Iterator>
	void TestPopHeapEa(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator last, Iterator popEnd)
	{
		stopwatch.Restart();
		while(last != popEnd)
			eastl::pop_heap(first, last--);
		stopwatch.Stop();
	}



	template <typename Iterator>
	void TestSortHeapStd(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator last)
	{
		stopwatch.Restart();
		std::sort_heap(first, last);
		stopwatch.Stop();
	}

	template <typename Iterator>
	void TestSortHeapEa(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator last)
	{
		stopwatch.Restart();
		eastl::sort_heap(first, last);
		stopwatch.Stop();
	}

} // namespace



void BenchmarkHeap()
{
	EASTLTest_Printf("Heap (Priority Queue)\n");

	EA::UnitTest::RandGenT<uint32_t> rng(EA::UnitTest::GetRandSeed());
	EA::StdC::Stopwatch              stopwatch1(EA::StdC::Stopwatch::kUnitsCPUCycles);
	EA::StdC::Stopwatch              stopwatch2(EA::StdC::Stopwatch::kUnitsCPUCycles);

	{
		const int kArraySize = 100000;

		// uint32[]
		uint32_t* const pIntArrayS = new uint32_t[kArraySize * 2];  // * 2 because we will be adding more items via push_heap.
		uint32_t* const pIntArrayE = new uint32_t[kArraySize * 2];  // S means Std; E means EA.
		uint32_t* const pIntArray2 = new uint32_t[kArraySize];      // This will be used for pop_heap.

		eastl::generate(pIntArrayS, pIntArrayS + kArraySize, rng);
		eastl::copy(pIntArrayS, pIntArrayS + kArraySize, pIntArrayE);
		eastl::copy(pIntArrayS, pIntArrayS + kArraySize, pIntArray2);


		// vector<TestObject>
		std::vector<TestObject>   stdVectorTO(kArraySize * 2);
		std::vector<TestObject>   stdVectorTO2(kArraySize);
		eastl::vector<TestObject> eaVectorTO(kArraySize * 2);
		eastl::vector<TestObject> eaVectorTO2(kArraySize);

		for(int k = 0; k < kArraySize; k++)
		{
			stdVectorTO[k]  = TestObject(pIntArrayS[k]);
			stdVectorTO2[k] = TestObject(pIntArrayS[k]);
			eaVectorTO[k]   = TestObject(pIntArrayS[k]);
			eaVectorTO2[k]  = TestObject(pIntArrayS[k]);
		}


		for(int i = 0; i < 2; i++)
		{
			///////////////////////////////
			// Test make_heap
			///////////////////////////////

			TestMakeHeapStd(stopwatch1, pIntArrayS, pIntArrayS + kArraySize);
			TestMakeHeapEa (stopwatch2, pIntArrayE, pIntArrayE + kArraySize);

			if(i == 1)
				Benchmark::AddResult("heap (uint32_t[])/make_heap", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestMakeHeapStd(stopwatch1, stdVectorTO.begin(), stdVectorTO.begin() + kArraySize);
			TestMakeHeapEa (stopwatch2, eaVectorTO.begin(),  eaVectorTO.begin()  + kArraySize);

			if(i == 1)
				Benchmark::AddResult("heap (vector<TestObject>)/make_heap", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());



			///////////////////////////////
			// Test push_heap
			///////////////////////////////

			TestPushHeapStd(stopwatch1, pIntArrayS, pIntArrayS + kArraySize, pIntArray2, pIntArray2 + kArraySize);
			TestPushHeapEa (stopwatch2, pIntArrayE, pIntArrayE + kArraySize, pIntArray2, pIntArray2 + kArraySize);

			if(i == 1)
				Benchmark::AddResult("heap (uint32_t[])/push_heap", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestPushHeapStd(stopwatch1, stdVectorTO.begin(), stdVectorTO.begin() + kArraySize, stdVectorTO2.begin(), stdVectorTO2.begin() + kArraySize);
			TestPushHeapEa (stopwatch2, eaVectorTO.begin(),  eaVectorTO.begin()  + kArraySize, eaVectorTO2.begin(),  eaVectorTO2.begin()  + kArraySize);

			if(i == 1)
				Benchmark::AddResult("heap (vector<TestObject>)/push_heap", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());



			///////////////////////////////
			// Test pop_heap
			///////////////////////////////

			TestPopHeapStd(stopwatch1, pIntArrayS, pIntArrayS + (kArraySize * 2), pIntArrayS + kArraySize); // * 2 because we used push_heap above to add more items.
			TestPopHeapEa (stopwatch2, pIntArrayE, pIntArrayE + (kArraySize * 2), pIntArrayE + kArraySize);

			if(i == 1)
				Benchmark::AddResult("heap (uint32_t[])/pop_heap", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestPopHeapStd(stopwatch1, stdVectorTO.begin(), stdVectorTO.begin() + (kArraySize * 2), stdVectorTO.begin() + kArraySize); // * 2 because we used push_heap above to add more items.
			TestPopHeapEa (stopwatch2, eaVectorTO.begin(),  eaVectorTO.begin()  + (kArraySize * 2), eaVectorTO.begin()  + kArraySize);

			if(i == 1)
				Benchmark::AddResult("heap (vector<TestObject>)/pop_heap", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());



			///////////////////////////////
			// Test sort_heap
			///////////////////////////////

			TestSortHeapStd(stopwatch1, pIntArrayS, pIntArrayS + kArraySize);
			TestSortHeapEa (stopwatch2, pIntArrayE, pIntArrayE + kArraySize);

			if(i == 1)
				Benchmark::AddResult("heap (uint32_t[])/sort_heap", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestSortHeapStd(stopwatch1, stdVectorTO.begin(), stdVectorTO.begin() + kArraySize);
			TestSortHeapEa (stopwatch2, eaVectorTO.begin(),  eaVectorTO.begin()  + kArraySize);

			if(i == 1)
				Benchmark::AddResult("heap (vector<TestObject>)/sort_heap", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
		}

		delete[] pIntArrayS;
		delete[] pIntArrayE;
		delete[] pIntArray2;
	}
}












