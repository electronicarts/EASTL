/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLBenchmark.h"
#include "EASTLTest.h"
#include <EAStdC/EAStopwatch.h>
#include <EASTL/algorithm.h>
#include <EASTL/string.h>
#include <EASTL/sort.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <algorithm>
#include <string>
#include <stdio.h>
#include <stdlib.h>
EA_RESTORE_ALL_VC_WARNINGS()


using namespace EA;


namespace
{
	template <typename Container> 
	void TestPushBack(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		stopwatch.Restart();
		for(int i = 0; i < 100000; i++)
			c.push_back((typename Container::value_type)(i & ((typename Container::value_type)~0)));
		stopwatch.Stop();
	}


	template <typename Container, typename T> 
	void TestInsert1(EA::StdC::Stopwatch& stopwatch, Container& c, T* p)
	{
		const typename Container::size_type s = c.size();
		stopwatch.Restart();
		for(int i = 0; i < 100; i++)
			c.insert(s - (typename Container::size_type)(i * 317), p);
		stopwatch.Stop();
	}


	template <typename Container> 
	void TestErase1(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		const typename Container::size_type s = c.size();
		stopwatch.Restart();
		for(int i = 0; i < 100; i++)
			c.erase(s - (typename Container::size_type)(i * 339), 7);
		stopwatch.Stop();
	}


	template <typename Container, typename T> 
	void TestReplace1(EA::StdC::Stopwatch& stopwatch, Container& c, T* p, int n)
	{
		const typename Container::size_type s = c.size();
		stopwatch.Restart();
		for(int i = 0; i < 1000; i++)
			c.replace(s - (typename Container::size_type)(i * 5), ((n - 2) + (i & 3)), p, n); // The second argument rotates through n-2, n-1, n, n+1, n-2, etc.
		stopwatch.Stop();
	}


	template <typename Container> 
	void TestReserve(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		const typename Container::size_type s = c.capacity();
		stopwatch.Restart();
		for(int i = 0; i < 1000; i++)
			c.reserve((s - 2) + (i & 3)); // The second argument rotates through n-2, n-1, n, n+1, n-2, etc.
		stopwatch.Stop();
	}


	template <typename Container> 
	void TestSize(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		stopwatch.Restart();
		for(int i = 0; i < 1000; i++)
			Benchmark::DoNothing(&c, c.size()); 
		stopwatch.Stop();
	}


	template <typename Container>
	void TestBracket(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		int32_t temp = 0;
		stopwatch.Restart();
		for(typename Container::size_type j = 0, jEnd = c.size(); j < jEnd; j++)
			temp += c[j];
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)temp);
	}


	template <typename Container>
	void TestFind(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		stopwatch.Restart();
		for(int i = 0; i < 1000; i++)
			Benchmark::DoNothing(&c, *eastl::find(c.begin(), c.end(), (typename Container::value_type)~0));
		stopwatch.Stop();
	}


	template <typename Container, typename T> 
	void TestFind1(EA::StdC::Stopwatch& stopwatch, Container& c, T* p, int pos, int n)
	{
		stopwatch.Restart();
		for(int i = 0; i < 1000; i++)
			Benchmark::DoNothing(&c, c.find(p, (typename Container::size_type)pos, (typename Container::size_type)n));
		stopwatch.Stop();
	}


	template <typename Container, typename T> 
	void TestRfind1(EA::StdC::Stopwatch& stopwatch, Container& c, T* p, int pos, int n)
	{
		stopwatch.Restart();
		for(int i = 0; i < 1000; i++)
			Benchmark::DoNothing(&c, c.rfind(p, (typename Container::size_type)pos, (typename Container::size_type)n));
		stopwatch.Stop();
	}

	template <typename Container, typename T> 
	void TestFirstOf1(EA::StdC::Stopwatch& stopwatch, Container& c, T* p, int pos, int n)
	{
		stopwatch.Restart();
		for(int i = 0; i < 1000; i++)
			Benchmark::DoNothing(&c, c.find_first_of(p, (typename Container::size_type)pos, (typename Container::size_type)n));
		stopwatch.Stop();
	}

	template <typename Container, typename T> 
	void TestLastOf1(EA::StdC::Stopwatch& stopwatch, Container& c, T* p, int pos, int n)
	{
		stopwatch.Restart();
		for(int i = 0; i < 1000; i++)
			Benchmark::DoNothing(&c, c.find_last_of(p, (typename Container::size_type)pos, (typename Container::size_type)n));
		stopwatch.Stop();
	}

	template <typename Container, typename T> 
	void TestFirstNotOf1(EA::StdC::Stopwatch& stopwatch, Container& c, T* p, int pos, int n)
	{
		stopwatch.Restart();
		for(int i = 0; i < 1000; i++)
			Benchmark::DoNothing(&c, c.find_first_not_of(p, (typename Container::size_type)pos, (typename Container::size_type)n));
		stopwatch.Stop();
	}

	template <typename Container, typename T> 
	void TestLastNotOf1(EA::StdC::Stopwatch& stopwatch, Container& c, T* p, int pos, int n)
	{
		stopwatch.Restart();
		for(int i = 0; i < 1000; i++)
			Benchmark::DoNothing(&c, c.find_last_not_of(p, (typename Container::size_type)pos, (typename Container::size_type)n));
		stopwatch.Stop();
	}


	template <typename Container> 
	void TestCompare(EA::StdC::Stopwatch& stopwatch, Container& c1, Container& c2) // size()
	{
		stopwatch.Restart();
		for(int i = 0; i < 500; i++)
			Benchmark::DoNothing(&c1, c1.compare(c2));
		stopwatch.Stop();
	}


	template <typename Container> 
	void TestSwap(EA::StdC::Stopwatch& stopwatch, Container& c1, Container& c2) // size()
	{
		stopwatch.Restart();
		for(int i = 0; i < 10000; i++) // Make sure this is an even count so that when done things haven't changed.
		{
			c1.swap(c2);
			Benchmark::DoNothing(&c1);
		} 
		stopwatch.Stop();
	}

} // namespace




void BenchmarkString()
{
	EASTLTest_Printf("String\n");

	EA::StdC::Stopwatch stopwatch1(EA::StdC::Stopwatch::kUnitsCPUCycles);
	EA::StdC::Stopwatch stopwatch2(EA::StdC::Stopwatch::kUnitsCPUCycles);

	{
		for(int i = 0; i < 2; i++)
		{
			std::basic_string<char8_t>    ss8(16, 0);   // We initialize to size of 16 because different implementations may make
			eastl::basic_string<char8_t>  es8(16, 0);   // different tradeoffs related to startup size. Initial operations are faster
														// when strings start with a higher reserve, but they use more memory. 
			std::basic_string<char16_t>   ss16(16, 0);  // We try to nullify this tradeoff for the tests below by starting all at 
			eastl::basic_string<char16_t> es16(16, 0);  // the same baseline allocation.


			///////////////////////////////
			// Test push_back
			///////////////////////////////

			TestPushBack(stopwatch1, ss8);
			TestPushBack(stopwatch2, es8);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/push_back", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestPushBack(stopwatch1, ss16);
			TestPushBack(stopwatch2, es16);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/push_back", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test insert(size_type position, const value_type* p)
			///////////////////////////////

			const char8_t pInsert1_8[] = { 'a', 0 };
			TestInsert1(stopwatch1, ss8, pInsert1_8);
			TestInsert1(stopwatch2, es8, pInsert1_8);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/insert/pos,p", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			const char16_t pInsert1_16[] = { 'a', 0 };
			TestInsert1(stopwatch1, ss16, pInsert1_16);
			TestInsert1(stopwatch2, es16, pInsert1_16);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/insert/pos,p", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test erase(size_type position, size_type n)
			///////////////////////////////

			TestErase1(stopwatch1, ss8);
			TestErase1(stopwatch2, es8);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/erase/pos,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestErase1(stopwatch1, ss16);
			TestErase1(stopwatch2, es16);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/erase/pos,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////            
			// Test replace(size_type position, size_type n1, const value_type* p, size_type n2)
			///////////////////////////////

			const int kReplace1Size = 8;
			const char8_t pReplace1_8[kReplace1Size] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };

			TestReplace1(stopwatch1, ss8, pReplace1_8, kReplace1Size);
			TestReplace1(stopwatch2, es8, pReplace1_8, kReplace1Size);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/replace/pos,n,p,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			const char16_t pReplace1_16[kReplace1Size] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };

			TestReplace1(stopwatch1, ss16, pReplace1_16, kReplace1Size);
			TestReplace1(stopwatch2, es16, pReplace1_16, kReplace1Size);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/replace/pos,n,p,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test reserve(size_type)
			///////////////////////////////

			TestReserve(stopwatch1, ss8);
			TestReserve(stopwatch2, es8);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/reserve", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestReserve(stopwatch1, ss16);
			TestReserve(stopwatch2, es16);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/reserve", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test size()
			///////////////////////////////

			TestSize(stopwatch1, ss8);
			TestSize(stopwatch2, es8);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/size", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestSize(stopwatch1, ss16);
			TestSize(stopwatch2, es16);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/size", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test operator[].
			///////////////////////////////

			TestBracket(stopwatch1, ss8);
			TestBracket(stopwatch2, es8);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/operator[]", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestBracket(stopwatch1, ss16);
			TestBracket(stopwatch2, es16);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/operator[]", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test iteration via find().
			///////////////////////////////

			TestFind(stopwatch1, ss8);
			TestFind(stopwatch2, es8);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/iteration", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestFind(stopwatch1, ss16);
			TestFind(stopwatch2, es16);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/iteration", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test find(const value_type* p, size_type position, size_type n)
			///////////////////////////////

			const int kFind1Size = 7;
			const char8_t pFind1_8[kFind1Size] = { 'p', 'a', 't', 't', 'e', 'r', 'n' };

			ss8.insert(ss8.size() / 2, pFind1_8);
			es8.insert(es8.size() / 2, pFind1_8);

			TestFind1(stopwatch1, ss8, pFind1_8, 15, kFind1Size);
			TestFind1(stopwatch2, es8, pFind1_8, 15, kFind1Size);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/find/p,pos,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			const char16_t pFind1_16[kFind1Size] = { 'p', 'a', 't', 't', 'e', 'r', 'n' };

			#if !defined(EA_PLATFORM_IPHONE) && (!defined(EA_COMPILER_CLANG) && defined(EA_PLATFORM_MINGW)) // Crashes on iPhone.
			  ss16.insert(ss8.size() / 2, pFind1_16);
			#endif
			es16.insert(es8.size() / 2, pFind1_16);

			TestFind1(stopwatch1, ss16, pFind1_16, 15, kFind1Size);
			TestFind1(stopwatch2, es16, pFind1_16, 15, kFind1Size);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/find/p,pos,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test rfind(const value_type* p, size_type position, size_type n)
			///////////////////////////////

			TestRfind1(stopwatch1, ss8, pFind1_8, 15, kFind1Size);
			TestRfind1(stopwatch2, es8, pFind1_8, 15, kFind1Size);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/rfind/p,pos,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestRfind1(stopwatch1, ss16, pFind1_16, 15, kFind1Size);
			TestRfind1(stopwatch2, es16, pFind1_16, 15, kFind1Size);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/rfind/p,pos,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			//NOTICE (RASHIN):
			//FindFirstOf variants are incredibly slow on palm pixi debug builds.
			//Disabling for now...
		#if !defined(EA_DEBUG)
			///////////////////////////////
			// Test find_first_of(const value_type* p, size_type position, size_type n
			///////////////////////////////

			const int kFindOf1Size = 7;
			const char8_t pFindOf1_8[kFindOf1Size] = { '~', '~', '~', '~', '~', '~', '~' };

			TestFirstOf1(stopwatch1, ss8, pFindOf1_8, 15, kFindOf1Size);
			TestFirstOf1(stopwatch2, es8, pFindOf1_8, 15, kFindOf1Size);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/find_first_of/p,pos,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			const char16_t pFindOf1_16[kFindOf1Size] = { '~', '~', '~', '~', '~', '~', '~' };

			TestFirstOf1(stopwatch1, ss16, pFindOf1_16, 15, kFindOf1Size);
			TestFirstOf1(stopwatch2, es16, pFindOf1_16, 15, kFindOf1Size);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/find_first_of/p,pos,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test find_last_of(const value_type* p, size_type position, size_type n
			///////////////////////////////

			TestLastOf1(stopwatch1, ss8, pFindOf1_8, 15, kFindOf1Size);
			TestLastOf1(stopwatch2, es8, pFindOf1_8, 15, kFindOf1Size);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/find_last_of/p,pos,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestLastOf1(stopwatch1, ss16, pFindOf1_16, 15, kFindOf1Size);
			TestLastOf1(stopwatch2, es16, pFindOf1_16, 15, kFindOf1Size);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/find_last_of/p,pos,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test find_first_not_of(const value_type* p, size_type position, size_type n
			///////////////////////////////

			TestFirstNotOf1(stopwatch1, ss8, pFind1_8, 15, kFind1Size);
			TestFirstNotOf1(stopwatch2, es8, pFind1_8, 15, kFind1Size);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/find_first_not_of/p,pos,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestFirstNotOf1(stopwatch1, ss16, pFind1_16, 15, kFind1Size);
			TestFirstNotOf1(stopwatch2, es16, pFind1_16, 15, kFind1Size);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/find_first_not_of/p,pos,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test find_last_of(const value_type* p, size_type position, size_type n
			///////////////////////////////

			TestLastNotOf1(stopwatch1, ss8, pFind1_8, 15, kFind1Size);
			TestLastNotOf1(stopwatch2, es8, pFind1_8, 15, kFind1Size);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/find_last_of/p,pos,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestLastNotOf1(stopwatch1, ss16, pFind1_16, 15, kFind1Size);
			TestLastNotOf1(stopwatch2, es16, pFind1_16, 15, kFind1Size);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/find_last_of/p,pos,n", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

		#endif

			///////////////////////////////
			// Test compare()
			///////////////////////////////

			std::basic_string<char8_t>    ss8X(ss8);
			eastl::basic_string<char8_t>  es8X(es8);
			std::basic_string<char16_t>   ss16X(ss16);
			eastl::basic_string<char16_t> es16X(es16);

			TestCompare(stopwatch1, ss8, ss8X);
			TestCompare(stopwatch2, es8, es8X);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/compare", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestCompare(stopwatch1, ss16, ss16X);
			TestCompare(stopwatch2, es16, es16X);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/compare", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());



			///////////////////////////////
			// Test swap()
			///////////////////////////////

			TestSwap(stopwatch1, ss8, ss8X);
			TestSwap(stopwatch2, es8, es8X);

			if(i == 1)
				Benchmark::AddResult("string<char8_t>/swap", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestSwap(stopwatch1, ss16, ss16X);
			TestSwap(stopwatch2, es16, es16X);

			if(i == 1)
				Benchmark::AddResult("string<char16_t>/swap", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

		}
	}

}









