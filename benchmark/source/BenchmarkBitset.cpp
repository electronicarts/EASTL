/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifdef _MSC_VER
	// Microsoft STL generates warnings.
	#pragma warning(disable: 4267) // 'initializing' : conversion from 'size_t' to 'const int', possible loss of data
#endif

#include "EASTLBenchmark.h"
#include "EASTLTest.h"
#include <EAStdC/EAStopwatch.h>
#include <EASTL/bitset.h>


EA_DISABLE_ALL_VC_WARNINGS()
#include <bitset>
EA_RESTORE_ALL_VC_WARNINGS()


using namespace EA;


namespace
{
	template <typename Bitset>
	void TestSet(EA::StdC::Stopwatch& stopwatch, Bitset& b)
	{
		stopwatch.Restart();
		for(int i = 0; i < 100000; i++)
		{
			b.set();
			Benchmark::DoNothing(&b);
		}
		stopwatch.Stop();
	}


	template <typename Bitset>
	void TestSetIndex(EA::StdC::Stopwatch& stopwatch, Bitset& b, size_t index)
	{
		stopwatch.Restart();
		for(int i = 0; i < 100000; i++)
		{
			b.set(index);
			Benchmark::DoNothing(&b);
		}
		stopwatch.Stop();
	}


	template <typename Bitset>
	void TestReset(EA::StdC::Stopwatch& stopwatch, Bitset& b)
	{
		stopwatch.Restart();
		for(int i = 0; i < 100000; i++)
		{
			b.reset();
			Benchmark::DoNothing(&b);
		}
		stopwatch.Stop();
	}


	template <typename Bitset>
	void TestFlip(EA::StdC::Stopwatch& stopwatch, Bitset& b)
	{
		stopwatch.Restart();
		for(int i = 0; i < 100000; i++)
		{
			b.flip();
			Benchmark::DoNothing(&b);
		}
		stopwatch.Stop();
	}


	template <typename Bitset>
	void TestTest(EA::StdC::Stopwatch& stopwatch, Bitset& b, unsigned nANDValue)
	{
		stopwatch.Restart();
		for(unsigned i = 0; i < 100000; i++)
			Benchmark::DoNothing(b.test(i & nANDValue)); // We use & instead of % because the former is always fast due to forced power of 2.
		stopwatch.Stop();
	}


	template <typename Bitset>
	void TestCount(EA::StdC::Stopwatch& stopwatch, Bitset& b)
	{
		size_t temp = 0;
		stopwatch.Restart();
		for(int i = 0; i < 100000; i++)
		{
			temp += b.count();
			Benchmark::DoNothing(&temp);
		}
		stopwatch.Stop();
	}


	template <typename Bitset>
	void TestRightShift(EA::StdC::Stopwatch& stopwatch, Bitset& b, size_t n)
	{
		size_t temp = 0;
		stopwatch.Restart();
		for(int i = 0; i < 100000; i++)
		{
			b >>= n;
			Benchmark::DoNothing(&temp);
		}
		stopwatch.Stop();
	}

} // namespace



void BenchmarkBitset()
{
	EASTLTest_Printf("Bitset\n");

	EA::StdC::Stopwatch stopwatch1(EA::StdC::Stopwatch::kUnitsCPUCycles);
	EA::StdC::Stopwatch stopwatch2(EA::StdC::Stopwatch::kUnitsCPUCycles);

	{
		std::bitset<15>     stdBitset15;
		eastl::bitset<15>   eaBitset15;

		std::bitset<35>     stdBitset35;
		eastl::bitset<35>   eaBitset35;

		std::bitset<75>     stdBitset75;
		eastl::bitset<75>   eaBitset75;

		std::bitset<1500>   stdBitset1500;
		eastl::bitset<1500> eaBitset1500;


		for(int i = 0; i < 2; i++)
		{
			///////////////////////////////
			// Test set()
			///////////////////////////////

			TestSet(stopwatch1, stdBitset15);
			TestSet(stopwatch2, eaBitset15);

			if(i == 1)
				Benchmark::AddResult("bitset<15>/set()", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestSet(stopwatch1, stdBitset35);
			TestSet(stopwatch2, eaBitset35);

			if(i == 1)
				Benchmark::AddResult("bitset<35>/set()", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestSet(stopwatch1, stdBitset75);
			TestSet(stopwatch2, eaBitset75);

			if(i == 1)
				Benchmark::AddResult("bitset<75>/set()", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestSet(stopwatch1, stdBitset1500);
			TestSet(stopwatch2, eaBitset1500);

			if(i == 1)
				Benchmark::AddResult("bitset<1500>/set()", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test set(index)
			///////////////////////////////

			TestSetIndex(stopwatch1, stdBitset15, 13);
			TestSetIndex(stopwatch2, eaBitset15, 13);

			if(i == 1)
				Benchmark::AddResult("bitset<15>/set(i)", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestSetIndex(stopwatch1, stdBitset35, 33);
			TestSetIndex(stopwatch2, eaBitset35, 33);

			if(i == 1)
				Benchmark::AddResult("bitset<35>/set(i)", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestSetIndex(stopwatch1, stdBitset75, 73);
			TestSetIndex(stopwatch2, eaBitset75, 73);

			if(i == 1)
				Benchmark::AddResult("bitset<75>/set(i)", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestSetIndex(stopwatch1, stdBitset1500, 730);
			TestSetIndex(stopwatch2, eaBitset1500, 730);

			if(i == 1)
				Benchmark::AddResult("bitset<1500>/set(i)", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test reset()
			///////////////////////////////

			TestReset(stopwatch1, stdBitset15);
			TestReset(stopwatch2, eaBitset15);

			if(i == 1)
				Benchmark::AddResult("bitset<15>/reset", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestReset(stopwatch1, stdBitset35);
			TestReset(stopwatch2, eaBitset35);

			if(i == 1)
				Benchmark::AddResult("bitset<35>/reset", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestReset(stopwatch1, stdBitset75);
			TestReset(stopwatch2, eaBitset75);

			if(i == 1)
				Benchmark::AddResult("bitset<75>/reset", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestReset(stopwatch1, stdBitset1500);
			TestReset(stopwatch2, eaBitset1500);

			if(i == 1)
				Benchmark::AddResult("bitset<1500>/reset", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test flip
			///////////////////////////////

			TestFlip(stopwatch1, stdBitset15);
			TestFlip(stopwatch2, eaBitset15);

			if(i == 1)
				Benchmark::AddResult("bitset<15>/flip", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestFlip(stopwatch1, stdBitset35);
			TestFlip(stopwatch2, eaBitset35);

			if(i == 1)
				Benchmark::AddResult("bitset<35>/flip", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestFlip(stopwatch1, stdBitset75);
			TestFlip(stopwatch2, eaBitset75);

			if(i == 1)
				Benchmark::AddResult("bitset<75>/flip", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestFlip(stopwatch1, stdBitset1500);
			TestFlip(stopwatch2, eaBitset1500);

			if(i == 1)
				Benchmark::AddResult("bitset<1500>/flip", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test test
			///////////////////////////////

			TestTest(stopwatch1, stdBitset15, 7);
			TestTest(stopwatch2, eaBitset15, 7);

			if(i == 1)
				Benchmark::AddResult("bitset<15>/test", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestTest(stopwatch1, stdBitset35, 31);
			TestTest(stopwatch2, eaBitset35, 31);

			if(i == 1)
				Benchmark::AddResult("bitset<35>/test", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestTest(stopwatch1, stdBitset75, 63);
			TestTest(stopwatch2, eaBitset75, 63);

			if(i == 1)
				Benchmark::AddResult("bitset<75>/test", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestTest(stopwatch1, stdBitset1500, 1023);
			TestTest(stopwatch2, eaBitset1500, 1023);

			if(i == 1)
				Benchmark::AddResult("bitset<1500>/test", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test count
			///////////////////////////////

			TestCount(stopwatch1, stdBitset15);
			TestCount(stopwatch2, eaBitset15);

			if(i == 1)
				Benchmark::AddResult("bitset<15>/count", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestCount(stopwatch1, stdBitset35);
			TestCount(stopwatch2, eaBitset35);

			if(i == 1)
				Benchmark::AddResult("bitset<35>/count", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestCount(stopwatch1, stdBitset75);
			TestCount(stopwatch2, eaBitset75);

			if(i == 1)
				Benchmark::AddResult("bitset<75>/count", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestCount(stopwatch1, stdBitset1500);
			TestCount(stopwatch2, eaBitset1500);

			if(i == 1)
				Benchmark::AddResult("bitset<1500>/count", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test >>=
			///////////////////////////////

			TestRightShift(stopwatch1, stdBitset15, 1);
			TestRightShift(stopwatch2, eaBitset15, 1);

			if(i == 1)
				Benchmark::AddResult("bitset<15>/>>=/1", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime(),
										GetStdSTLType() == kSTLPort ? "STLPort is broken, neglects wraparound check." : NULL);

			TestRightShift(stopwatch1, stdBitset35, 1);
			TestRightShift(stopwatch2, eaBitset35, 1);

			if(i == 1)
				Benchmark::AddResult("bitset<35>/>>=/1", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime(),
										GetStdSTLType() == kSTLPort ? "STLPort is broken, neglects wraparound check." : NULL);

			TestRightShift(stopwatch1, stdBitset75, 1);
			TestRightShift(stopwatch2, eaBitset75, 1);

			if(i == 1)
				Benchmark::AddResult("bitset<75>/>>=/1", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime(),
										GetStdSTLType() == kSTLPort ? "STLPort is broken, neglects wraparound check." : NULL);

			TestRightShift(stopwatch1, stdBitset1500, 1);
			TestRightShift(stopwatch2, eaBitset1500, 1);

			if(i == 1)
				Benchmark::AddResult("bitset<1500>/>>=/1", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime(),
										GetStdSTLType() == kSTLPort ? "STLPort is broken, neglects wraparound check." : NULL);
		}
	}
}
















