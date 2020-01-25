/////////////////////////////////////////////////////////////////////////////
// BenchmarkAlgorithm.cpp
//
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLBenchmark.h"
#include "EASTLTest.h"
#include <EAStdC/EAStopwatch.h>
#include <EAStdC/EAMemory.h>
#include <EASTL/algorithm.h>
#include <EASTL/sort.h>
#include <EASTL/vector.h>
#include <EASTL/slist.h>
#include <EASTL/list.h>
#include <EASTL/string.h>
#include <EASTL/random.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
EA_RESTORE_ALL_VC_WARNINGS()

#ifdef _MSC_VER
	#pragma warning(disable: 4996) // Function call with parameters that may be unsafe
#endif


using namespace EA;


typedef std::vector<unsigned char>   StdVectorUChar;
typedef eastl::vector<unsigned char> EaVectorUChar;

typedef std::vector<signed char>     StdVectorSChar;
typedef eastl::vector<signed char>   EaVectorSChar;

typedef std::vector<uint32_t>        StdVectorUint32;
typedef eastl::vector<uint32_t>      EaVectorUint32;

typedef std::vector<uint64_t>        StdVectorUint64;
typedef eastl::vector<uint64_t>      EaVectorUint64;

typedef std::vector<TestObject>      StdVectorTO;
typedef eastl::vector<TestObject>    EaVectorTO;


// We make a fake version of C++11 std::next, as some C++ compilers don't currently 
// provide the C++11 next algorithm in their standard libraries.
namespace std__
{
	template<typename InputIterator>
	inline InputIterator 
	next(InputIterator it, typename std::iterator_traits<InputIterator>::difference_type n = 1)
	{
		std::advance(it, n);
		return it;
	}
}


namespace
{
	void TestFindEndStd(EA::StdC::Stopwatch& stopwatch, const std::string& sTest, const char* pSearchStringBegin, const char* pSearchStringEnd)
	{
		stopwatch.Restart();
		std::string::const_iterator it = std::find_end(sTest.begin(), sTest.end(), pSearchStringBegin, pSearchStringEnd);
		stopwatch.Stop();
		if(it != sTest.end())
			sprintf(Benchmark::gScratchBuffer, "%c", *it);
	}

	void TestFindEndEa(EA::StdC::Stopwatch& stopwatch, const eastl::string& sTest, const char* pSearchStringBegin, const char* pSearchStringEnd)
	{
		stopwatch.Restart();
		eastl::string::const_iterator it = eastl::find_end(sTest.begin(), sTest.end(), pSearchStringBegin, pSearchStringEnd);
		stopwatch.Stop();
		if(it != sTest.end())
			sprintf(Benchmark::gScratchBuffer, "%c", *it);
	}



	void TestSearchStd(EA::StdC::Stopwatch& stopwatch, const std::string& sTest, const char* pSearchStringBegin, const char* pSearchStringEnd)
	{
		stopwatch.Restart();
		std::string::const_iterator it = std::search(sTest.begin(), sTest.end(), pSearchStringBegin, pSearchStringEnd);
		stopwatch.Stop();
		if(it != sTest.end())
			sprintf(Benchmark::gScratchBuffer, "%c", *it);
	}

	void TestSearchEa(EA::StdC::Stopwatch& stopwatch, const eastl::string& sTest, const char* pSearchStringBegin, const char* pSearchStringEnd)
	{
		stopwatch.Restart();
		eastl::string::const_iterator it = eastl::search(sTest.begin(), sTest.end(), pSearchStringBegin, pSearchStringEnd);
		stopwatch.Stop();
		if(it != sTest.end())
			sprintf(Benchmark::gScratchBuffer, "%c", *it);
	}



	void TestSearchNStd(EA::StdC::Stopwatch& stopwatch, const std::string& sTest, int n, char c)
	{
		stopwatch.Restart();
		std::string::const_iterator it = std::search_n(sTest.begin(), sTest.end(), n, c);
		stopwatch.Stop();
		if(it != sTest.end())
			sprintf(Benchmark::gScratchBuffer, "%c", *it);
	}

	void TestSearchNEa(EA::StdC::Stopwatch& stopwatch, const eastl::string& sTest, int n, char c)
	{
		stopwatch.Restart();
		eastl::string::const_iterator it = eastl::search_n(sTest.begin(), sTest.end(), n, c);
		stopwatch.Stop();
		if(it != sTest.end())
			sprintf(Benchmark::gScratchBuffer, "%c", *it);
	}



	template <typename Container>
	void TestUniqueStd(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		stopwatch.Restart();
		typename Container::iterator it = std::unique(c.begin(), c.end());
		stopwatch.Stop();
		c.erase(it, c.end());
	}

	template <typename Container>
	void TestUniqueEa(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		stopwatch.Restart();
		typename Container::iterator it = eastl::unique(c.begin(), c.end());
		stopwatch.Stop();        
		c.erase(it, c.end());
	}



	template <typename Container>
	void TestMinElementStd(EA::StdC::Stopwatch& stopwatch, const Container& c)
	{
		stopwatch.Restart();
		const typename Container::const_iterator it = std::min_element(c.begin(), c.end());
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%p", &it);
	}

	template <typename Container>
	void TestMinElementEa(EA::StdC::Stopwatch& stopwatch, const Container& c)
	{
		stopwatch.Restart();
		const typename Container::const_iterator it = eastl::min_element(c.begin(), c.end());
		stopwatch.Stop();        
		sprintf(Benchmark::gScratchBuffer, "%p", &it);
	}



	template <typename Container>
	void TestCountStd(EA::StdC::Stopwatch& stopwatch, const Container& c)
	{
		stopwatch.Restart();
		const typename Container::difference_type n = std::count(c.begin(), c.end(), (typename Container::value_type)999999);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%d", (int)n);
	}

	template <typename Container>
	void TestCountEa(EA::StdC::Stopwatch& stopwatch, const Container& c)
	{
		stopwatch.Restart();
		const typename Container::difference_type n = eastl::count(c.begin(), c.end(), (typename Container::value_type)999999);
		stopwatch.Stop();        
		sprintf(Benchmark::gScratchBuffer, "%d", (int)n);
	}



	template <typename Container>
	void TestAdjacentFindStd(EA::StdC::Stopwatch& stopwatch, const Container& c)
	{
		stopwatch.Restart();
		const typename Container::const_iterator it = std::adjacent_find(c.begin(), c.end());
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%p", &it);
	}

	template <typename Container>
	void TestAdjacentFindEa(EA::StdC::Stopwatch& stopwatch, const Container& c)
	{
		stopwatch.Restart();
		const typename Container::const_iterator it = eastl::adjacent_find(c.begin(), c.end());
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%p", &it);
	}



	template <typename Container>
	void TestLowerBoundStd(EA::StdC::Stopwatch& stopwatch, const Container& c, const typename Container::value_type* pBegin,  const typename Container::value_type* pEnd)
	{

		stopwatch.Restart();
		while(pBegin != pEnd)
		{
			typename Container::const_iterator it = std::lower_bound(c.begin(), c.end(), *pBegin++);
			Benchmark::DoNothing(&it);
		}
		stopwatch.Stop();
	}

	template <typename Container>
	void TestLowerBoundEa(EA::StdC::Stopwatch& stopwatch, Container& c, const typename Container::value_type* pBegin,  const typename Container::value_type* pEnd)
	{
		stopwatch.Restart();
		while(pBegin != pEnd)
		{
			typename Container::const_iterator it = eastl::lower_bound(c.begin(), c.end(), *pBegin++);
			Benchmark::DoNothing(&it);
		}
		stopwatch.Stop();        
	}



	template <typename Container>
	void TestUpperBoundStd(EA::StdC::Stopwatch& stopwatch, const Container& c, const typename Container::value_type* pBegin,  const typename Container::value_type* pEnd)
	{
		stopwatch.Restart();
		while(pBegin != pEnd)
		{
			typename Container::const_iterator it = std::upper_bound(c.begin(), c.end(), *pBegin++);
			Benchmark::DoNothing(&it);
		}
		stopwatch.Stop();
	}

	template <typename Container>
	void TestUpperBoundEa(EA::StdC::Stopwatch& stopwatch, Container& c, const typename Container::value_type* pBegin,  const typename Container::value_type* pEnd)
	{
		stopwatch.Restart();
		while(pBegin != pEnd)
		{
			typename Container::const_iterator it = eastl::upper_bound(c.begin(), c.end(), *pBegin++);
			Benchmark::DoNothing(&it);
		}
		stopwatch.Stop();
	}



	template <typename Container>
	void TestEqualRangeStd(EA::StdC::Stopwatch& stopwatch, const Container& c, const typename Container::value_type* pBegin,  const typename Container::value_type* pEnd)
	{
		stopwatch.Restart();
		while(pBegin != pEnd)
		{
			std::pair<const typename Container::const_iterator, 
					  const typename Container::const_iterator> itPair = std::equal_range(c.begin(), c.end(), *pBegin++);

			Benchmark::DoNothing(&itPair);
		}
		stopwatch.Stop();
	}

	template <typename Container>
	void TestEqualRangeEa(EA::StdC::Stopwatch& stopwatch, Container& c, const typename Container::value_type* pBegin,  const typename Container::value_type* pEnd)
	{
		stopwatch.Restart();
		while(pBegin != pEnd)
		{
			eastl::pair<const typename Container::const_iterator, 
						const typename Container::const_iterator> itPair = eastl::equal_range(c.begin(), c.end(), *pBegin++);
			Benchmark::DoNothing(&itPair);
		}
		stopwatch.Stop();
	}



	template <typename Iterator1, typename Iterator2>
	void TestLexicographicalCompareStd(EA::StdC::Stopwatch& stopwatch, Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2)
	{
		stopwatch.Restart();
		const bool bResult = std::lexicographical_compare(first1, last1, first2, last2);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%d", bResult ? (int)1 : (int)0);
	}

	template <typename Iterator1, typename Iterator2>
	void TestLexicographicalCompareEa(EA::StdC::Stopwatch& stopwatch, Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2)
	{
		stopwatch.Restart();
		const bool bResult = eastl::lexicographical_compare(first1, last1, first2, last2);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%d", bResult ? (int)1 : (int)0);
	}



	template <typename Iterator, typename OutputIterator>
	void TestCopyStd(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator last, OutputIterator result)
	{
		stopwatch.Restart();
		std::copy(first, last, result);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%d", (int)*first);
	}

	template <typename Iterator, typename OutputIterator>
	void TestCopyEa(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator last, OutputIterator result)
	{
		stopwatch.Restart();
		eastl::copy(first, last, result);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%d", (int)*first);
	}



	template <typename Iterator, typename OutputIterator>
	void TestCopyBackwardStd(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator last, OutputIterator result)
	{
		stopwatch.Restart();
		std::copy_backward(first, last, result);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%d", (int)*first);
	}

	template <typename Iterator, typename OutputIterator>
	void TestCopyBackwardEa(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator last, OutputIterator result)
	{
		stopwatch.Restart();
		eastl::copy_backward(first, last, result);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%d", (int)*first);
	}



	template <typename Iterator, typename Value>
	void TestFillStd(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator last, const Value& v)
	{
		stopwatch.Restart();
		std::fill(first, last, v);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%p", &*first);
	}

	template <typename Iterator, typename Value>
	void TestFillEa(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator last, const Value& v)
	{
		stopwatch.Restart();
		eastl::fill(first, last, v);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%p", &*first);
	}



	template <typename Iterator, typename Value>
	void TestFillNStd(EA::StdC::Stopwatch& stopwatch, Iterator first, int n, const Value& v)
	{
		stopwatch.Restart();
		std::fill_n(first, n, v);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%p", &*first);
	}

	template <typename Iterator, typename Value>
	void TestFillNEa(EA::StdC::Stopwatch& stopwatch, Iterator first, int n, const Value& v)
	{
		stopwatch.Restart();
		eastl::fill_n(first, n, v);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%p", &*first);
	}



	template <typename Iterator>
	void TestReverseStd(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator last)
	{
		stopwatch.Restart();
		std::reverse(first, last);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%p", &*first);
	}

	template <typename Iterator>
	void TestReverseEa(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator last)
	{
		stopwatch.Restart();
		eastl::reverse(first, last);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%p", &*first);
	}



	template <typename Iterator>
	void TestRotateStd(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator middle, Iterator last)
	{
		stopwatch.Restart();
		std::rotate(first, middle, last); // C++11 specifies that rotate has a return value, but not all std implementations return it.
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%p", &*first);
	}

	template <typename Iterator>
	void TestRotateEa(EA::StdC::Stopwatch& stopwatch, Iterator first, Iterator middle, Iterator last)
	{
		stopwatch.Restart();
		eastl::rotate(first, middle, last);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%p", &*first);
	}

	template <typename Iterator>
	void TestMergeStd(EA::StdC::Stopwatch& stopwatch, Iterator firstIn1, Iterator lastIn1, Iterator firstIn2, Iterator lastIn2, Iterator out)
	{
		stopwatch.Restart();
		std::merge(firstIn1, lastIn1, firstIn2, lastIn2, out);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%p", &*out);
	}

	template <typename Iterator>
	void TestMergeEa(EA::StdC::Stopwatch& stopwatch, Iterator firstIn1, Iterator lastIn1, Iterator firstIn2, Iterator lastIn2, Iterator out)
	{
		stopwatch.Restart();
		eastl::merge(firstIn1, lastIn1, firstIn2, lastIn2, out);
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%p", &*out);
	}
} // namespace




void BenchmarkAlgorithm1(EASTLTest_Rand& /*rng*/, EA::StdC::Stopwatch& stopwatch1, EA::StdC::Stopwatch& stopwatch2)
{
	{
		std::string   sTestStd;
		eastl::string sTestEa;
		const char*   pSearchString1Begin = "AAA";
		const char*   pSearchString1End   = pSearchString1Begin + strlen(pSearchString1Begin);
		const char*   pSearchString2Begin = "BBB"; // This is something that doesn't exist searched string.
		const char*   pSearchString2End   = pSearchString2Begin + strlen(pSearchString2Begin);
		const char*   pSearchString3Begin = "CCC";
		const char*   pSearchString3End   = pSearchString3Begin + strlen(pSearchString3Begin);

		for(int i = 0; i < 10000; i++)
			sTestStd += "This is a test of the find_end algorithm. ";
		sTestEa.assign(sTestStd.data(), (eastl_size_t)sTestStd.length());

		for(int i = 0; i < 2; i++)
		{
			///////////////////////////////
			// Test find_end
			///////////////////////////////

			sTestStd.insert(sTestStd.size() * 15 / 16, pSearchString1Begin);
			sTestEa.insert (sTestEa.size()  * 15 / 16, pSearchString1Begin);
			TestFindEndStd(stopwatch1, sTestStd, pSearchString1Begin, pSearchString1End);
			TestFindEndEa (stopwatch2, sTestEa,  pSearchString1Begin, pSearchString1End);

			if(i == 1)
				Benchmark::AddResult("algorithm/find_end/string/end", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			sTestStd.insert(sTestStd.size() / 2, pSearchString2Begin);
			sTestEa.insert (sTestEa.size()  / 2, pSearchString2Begin);
			TestFindEndStd(stopwatch1, sTestStd, pSearchString2Begin, pSearchString2End);
			TestFindEndEa (stopwatch2, sTestEa,  pSearchString2Begin, pSearchString2End);

			if(i == 1)
				Benchmark::AddResult("algorithm/find_end/string/middle", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestFindEndStd(stopwatch1, sTestStd, pSearchString3Begin, pSearchString3End);
			TestFindEndEa (stopwatch2, sTestEa,  pSearchString3Begin, pSearchString3End);

			if(i == 1)
				Benchmark::AddResult("algorithm/find_end/string/none", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());



			///////////////////////////////
			// Test search
			///////////////////////////////
			TestSearchStd(stopwatch1, sTestStd, pSearchString1Begin, pSearchString1End);
			TestSearchEa (stopwatch2, sTestEa,  pSearchString1Begin, pSearchString1End);

			if(i == 1)
				Benchmark::AddResult("algorithm/search/string<char>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test search_n
			///////////////////////////////
			TestSearchNStd(stopwatch1, sTestStd, 3, 'A');
			TestSearchNEa (stopwatch2, sTestEa, 3, 'A');

			if(i == 1)
				Benchmark::AddResult("algorithm/search_n/string<char>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test adjacent_find
			///////////////////////////////

		}
	}
}


void BenchmarkAlgorithm2(EASTLTest_Rand& rng, EA::StdC::Stopwatch& stopwatch1, EA::StdC::Stopwatch& stopwatch2)
{
	{
		StdVectorUint32 stdVectorUint32;
		EaVectorUint32  eaVectorUint32;

		StdVectorUint64 stdVectorUint64;
		EaVectorUint64  eaVectorUint64;

		StdVectorTO     stdVectorTO;
		EaVectorTO      eaVectorTO;

		for(int i = 0; i < 2; i++)
		{
			stdVectorUint32.clear();
			eaVectorUint32.clear();

			for(int j = 0; j < 100000; j++)
			{
				stdVectorUint32.push_back(j);
				eaVectorUint32.push_back(j);
				stdVectorUint64.push_back(j);
				eaVectorUint64.push_back(j);
				stdVectorTO.push_back(TestObject(j));
				eaVectorTO.push_back(TestObject(j));

				if((rng() % 16) == 0)
				{
					stdVectorUint32.push_back(i);
					eaVectorUint32.push_back(i);
					stdVectorUint64.push_back(j);
					eaVectorUint64.push_back(j);
					stdVectorTO.push_back(TestObject(j));
					eaVectorTO.push_back(TestObject(j));

					if((rng() % 16) == 0)
					{
						stdVectorUint32.push_back(i);
						eaVectorUint32.push_back(i);
						stdVectorUint64.push_back(j);
						eaVectorUint64.push_back(j);
						stdVectorTO.push_back(TestObject(j));
						eaVectorTO.push_back(TestObject(j));
					}
				}
			}


			///////////////////////////////
			// Test unique
			///////////////////////////////

			TestUniqueStd(stopwatch1, stdVectorUint32);
			TestUniqueEa (stopwatch2, eaVectorUint32);

			if(i == 1)
				Benchmark::AddResult("algorithm/unique/vector<uint32_t>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestUniqueStd(stopwatch1, stdVectorUint64);
			TestUniqueEa (stopwatch2, eaVectorUint64);

			if(i == 1)
				Benchmark::AddResult("algorithm/unique/vector<uint64_t>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestUniqueStd(stopwatch1, stdVectorTO);
			TestUniqueEa (stopwatch2, eaVectorTO);

			if(i == 1)
				Benchmark::AddResult("algorithm/unique/vector<TestObject>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());



			///////////////////////////////
			// Test min_element
			///////////////////////////////

			TestMinElementStd(stopwatch1, stdVectorTO);
			TestMinElementEa (stopwatch2, eaVectorTO);

			if(i == 1)
				Benchmark::AddResult("algorithm/min_element/vector<TestObject>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());



			///////////////////////////////
			// Test count
			///////////////////////////////

			TestCountStd(stopwatch1, stdVectorUint64);
			TestCountEa (stopwatch2, eaVectorUint64);

			if(i == 1)
				Benchmark::AddResult("algorithm/count/vector<uint64_t>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			
			///////////////////////////////
			// Test adjacent_find
			///////////////////////////////

			// Due to the above unique testing, the container should container unique elements. Let's change that.
			stdVectorTO[stdVectorTO.size() - 2] = stdVectorTO[stdVectorTO.size() - 1];
			eaVectorTO[eaVectorTO.size() - 2] = eaVectorTO[eaVectorTO.size() - 1];
			TestAdjacentFindStd(stopwatch1, stdVectorTO);
			TestAdjacentFindEa (stopwatch2, eaVectorTO);

			if(i == 1)
				Benchmark::AddResult("algorithm/adj_find/vector<TestObject>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());



			///////////////////////////////
			// Test lower_bound
			///////////////////////////////

			// Sort the containers for the following tests.
			std::sort(stdVectorTO.begin(), stdVectorTO.end());
			eaVectorTO.assign(&stdVectorTO[0], &stdVectorTO[0] + stdVectorTO.size());

			TestLowerBoundStd(stopwatch1, stdVectorTO, &stdVectorTO[0], &stdVectorTO[0] + stdVectorTO.size());
			TestLowerBoundEa (stopwatch2, eaVectorTO,   &eaVectorTO[0],  &eaVectorTO[0] + eaVectorTO.size());

			if(i == 1)
				Benchmark::AddResult("algorithm/lower_bound/vector<TestObject>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test upper_bound
			///////////////////////////////

			std::sort(stdVectorUint32.begin(), stdVectorUint32.end());
			eaVectorUint32.assign(&stdVectorUint32[0], &stdVectorUint32[0] + stdVectorUint32.size());

			TestUpperBoundStd(stopwatch1, stdVectorUint32, &stdVectorUint32[0], &stdVectorUint32[0] + stdVectorUint32.size());
			TestUpperBoundEa (stopwatch2,  eaVectorUint32,  &eaVectorUint32[0],  &eaVectorUint32[0] + eaVectorUint32.size());

			if(i == 1)
				Benchmark::AddResult("algorithm/upper_bound/vector<uint32_t>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test equal_range
			///////////////////////////////

			// VS2010 (and later versions?) is extremely slow executing this in debug builds. It can take minutes for a 
			// single TestEqualRangeStd call to complete. It's so slow that it's nearly pointless to execute.
			#if !defined(_MSC_VER) || (_MSC_VER < 1600) || !defined(_ITERATOR_DEBUG_LEVEL) || (_ITERATOR_DEBUG_LEVEL < 2) 
				std::sort(stdVectorUint64.begin(), stdVectorUint64.end());
				eaVectorUint64.assign(&stdVectorUint64[0], &stdVectorUint64[0] + stdVectorUint64.size());

				TestEqualRangeStd(stopwatch1, stdVectorUint64, &stdVectorUint64[0], &stdVectorUint64[0] + stdVectorUint64.size());
				TestEqualRangeEa (stopwatch2,  eaVectorUint64,  &eaVectorUint64[0],  &eaVectorUint64[0] +  eaVectorUint64.size());

				if(i == 1)
					Benchmark::AddResult("algorithm/equal_range/vector<uint64_t>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
			#endif
		}
	}
}


void BenchmarkAlgorithm3(EASTLTest_Rand& /*rng*/, EA::StdC::Stopwatch& stopwatch1, EA::StdC::Stopwatch& stopwatch2)
{
	{
		StdVectorUChar  stdVectorUChar1(100000);
		StdVectorUChar  stdVectorUChar2(100000);
		EaVectorUChar   eaVectorUChar1(100000);
		EaVectorUChar   eaVectorUChar2(100000);

		StdVectorSChar  stdVectorSChar1(100000);
		StdVectorSChar  stdVectorSChar2(100000);
		EaVectorSChar   eaVectorSChar1(100000);
		EaVectorSChar   eaVectorSChar2(100000);

		StdVectorTO     stdVectorTO1(100000);
		StdVectorTO     stdVectorTO2(100000);
		EaVectorTO      eaVectorTO1(100000);
		EaVectorTO      eaVectorTO2(100000);

		// All these containers should have values of zero in them.

		for(int i = 0; i < 2; i++)
		{
			///////////////////////////////
			// Test lexicographical_compare
			///////////////////////////////

			TestLexicographicalCompareStd(stopwatch1, stdVectorUChar1.begin(), stdVectorUChar1.end(), stdVectorUChar2.begin(), stdVectorUChar2.end());
			TestLexicographicalCompareEa (stopwatch2,  eaVectorUChar1.begin(),  eaVectorUChar2.end(),  eaVectorUChar2.begin(),  eaVectorUChar2.end());

			if(i == 1)
				Benchmark::AddResult("algorithm/lex_cmp/vector<uchar>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestLexicographicalCompareStd(stopwatch1, &stdVectorSChar1[0], &stdVectorSChar1[0] + stdVectorSChar1.size(), &stdVectorSChar2[0], &stdVectorSChar2[0] + stdVectorSChar2.size());
			TestLexicographicalCompareEa (stopwatch2,  &eaVectorSChar1[0],  &eaVectorSChar1[0] +  eaVectorSChar1.size(),  &eaVectorSChar2[0],  &eaVectorSChar2[0] +  eaVectorSChar2.size());

			if(i == 1)
				Benchmark::AddResult("algorithm/lex_cmp/schar[]", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestLexicographicalCompareStd(stopwatch1, stdVectorTO1.begin(), stdVectorTO1.end(), stdVectorTO2.begin(), stdVectorTO2.end());
			TestLexicographicalCompareEa (stopwatch2,  eaVectorTO1.begin(),  eaVectorTO1.end(),  eaVectorTO2.begin(),  eaVectorTO2.end());

			if(i == 1)
				Benchmark::AddResult("algorithm/lex_cmp/vector<TestObject>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
		}
	}

}


void BenchmarkAlgorithm4(EASTLTest_Rand& /*rng*/, EA::StdC::Stopwatch& stopwatch1, EA::StdC::Stopwatch& stopwatch2)
{
	{
		std::vector<uint32_t>   stdVectorUint321(10000);
		std::vector<uint32_t>   stdVectorUint322(10000);
		eastl::vector<uint32_t> eaVectorUint321(10000);
		eastl::vector<uint32_t> eaVectorUint322(10000);

		std::vector<uint64_t>   stdVectorUint64(100000);
		eastl::vector<uint64_t> eaVectorUint64(100000);


		for(int i = 0; i < 2; i++)
		{
			///////////////////////////////
			// Test copy
			///////////////////////////////

			TestCopyStd(stopwatch1, stdVectorUint321.begin(), stdVectorUint321.end(), stdVectorUint322.begin());
			TestCopyEa (stopwatch2,  eaVectorUint321.begin(),  eaVectorUint321.end(),  eaVectorUint322.begin());

			if(i == 1)
				Benchmark::AddResult("algorithm/copy/vector<uint32_t>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test copy_backward
			///////////////////////////////

			TestCopyBackwardStd(stopwatch1, stdVectorUint321.begin(), stdVectorUint321.end(), stdVectorUint322.end());
			TestCopyBackwardEa (stopwatch2,  eaVectorUint321.begin(),  eaVectorUint321.end(),  eaVectorUint322.end());

			if(i == 1)
				Benchmark::AddResult("algorithm/copy_backward/vector<uint32_t>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test fill
			///////////////////////////////

			TestFillStd(stopwatch1, stdVectorUint64.begin(), stdVectorUint64.end(), UINT64_C(37));
			TestFillEa (stopwatch2,  eaVectorUint64.begin(),  eaVectorUint64.end(), UINT64_C(37));
			TestFillStd(stopwatch1, stdVectorUint64.begin(), stdVectorUint64.end(), UINT64_C(37)); // Intentionally do this a second time, as we are finding 
			TestFillEa (stopwatch2,  eaVectorUint64.begin(),  eaVectorUint64.end(), UINT64_C(37)); // the results are inconsistent otherwise.
			if(EA::StdC::Memcheck64(&eaVectorUint64[0], UINT64_C(37), eaVectorUint64.size()))
				EA::UnitTest::Report("eastl algorithm 64 bit fill failure.");

			if(i == 1)
				Benchmark::AddResult("algorithm/fill/vector<uint64_t>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test fill_n
			///////////////////////////////

			TestFillNStd(stopwatch1, stdVectorUint64.begin(), (int)stdVectorUint64.size(), UINT64_C(37));
			TestFillNEa (stopwatch2,  eaVectorUint64.begin(), (int) eaVectorUint64.size(), UINT64_C(37));
			TestFillNStd(stopwatch1, stdVectorUint64.begin(), (int)stdVectorUint64.size(), UINT64_C(37)); // Intentionally do this a second time, as we are finding 
			TestFillNEa (stopwatch2,  eaVectorUint64.begin(), (int) eaVectorUint64.size(), UINT64_C(37));  // the results are inconsistent otherwise.

			if(i == 1)
				Benchmark::AddResult("algorithm/fill_n/vector<uint64_t>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
		}
	}
}


void BenchmarkAlgorithm5(EASTLTest_Rand& /*rng*/, EA::StdC::Stopwatch& stopwatch1, EA::StdC::Stopwatch& stopwatch2)
{
	{
		std::vector<void*>      stdVectorVoid(100000);
		eastl::vector<void*>    eaVectorVoid(100000);

		std::vector<char>       stdVectorChar(100000);
		eastl::vector<char>     eaVectorChar(100000);

		std::vector<bool>       stdVectorBool(100000);
		eastl::vector<bool>     eaVectorBool(100000);

		for(int i = 0; i < 2; i++)
		{
			TestFillStd(stopwatch1, stdVectorVoid.begin(), stdVectorVoid.end(), (void*)NULL);
			TestFillEa (stopwatch2, eaVectorVoid.begin(),  eaVectorVoid.end(),  (void*)NULL);

			if(i == 1)
				Benchmark::AddResult("algorithm/fill/vector<void*>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestFillStd(stopwatch1,    &stdVectorChar[0],     &stdVectorChar[0] + stdVectorChar.size(), 'd'); // Intentionally use ' ' and not casted to any type.
			TestFillEa (stopwatch2,  eaVectorChar.data(),   eaVectorChar.data() +  eaVectorChar.size(), 'd');
			TestFillStd(stopwatch1,    &stdVectorChar[0],     &stdVectorChar[0] + stdVectorChar.size(), 'd'); // Intentionally do this a second time, as we are finding 
			TestFillEa (stopwatch2,  eaVectorChar.data(),   eaVectorChar.data() +  eaVectorChar.size(), 'd'); // the results are inconsistent otherwise.

			if(i == 1)
				Benchmark::AddResult("algorithm/fill/char[]/'d'", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestFillStd(stopwatch1, stdVectorChar.begin(), stdVectorChar.end(), (char)'d'); 
			TestFillEa (stopwatch2,  eaVectorChar.begin(),  eaVectorChar.end(), (char)'d');
			TestFillStd(stopwatch1, stdVectorChar.begin(), stdVectorChar.end(), (char)'d'); // Intentionally do this a second time, as we are finding 
			TestFillEa (stopwatch2,  eaVectorChar.begin(),  eaVectorChar.end(), (char)'d'); // the results are inconsistent otherwise.

			if(i == 1)
				Benchmark::AddResult("algorithm/fill/vector<char>/'d'", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestFillStd(stopwatch1, stdVectorChar.begin(), stdVectorChar.end(), (char)0); 
			TestFillEa (stopwatch2,  eaVectorChar.begin(),  eaVectorChar.end(), (char)0);
			TestFillStd(stopwatch1, stdVectorChar.begin(), stdVectorChar.end(), (char)0); // Intentionally do this a second time, as we are finding 
			TestFillEa (stopwatch2,  eaVectorChar.begin(),  eaVectorChar.end(), (char)0); // the results are inconsistent otherwise.

			if(i == 1)
				Benchmark::AddResult("algorithm/fill/vector<char>/0", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestFillStd(stopwatch1,  eaVectorBool.data(),   eaVectorBool.data() +  eaVectorBool.size(), false); // Intentionally use eaVectorBool for the array. 
			TestFillEa (stopwatch2,  eaVectorBool.data(),   eaVectorBool.data() +  eaVectorBool.size(), false);
			TestFillStd(stopwatch1,  eaVectorBool.data(),   eaVectorBool.data() +  eaVectorBool.size(), false);
			TestFillEa (stopwatch2,  eaVectorBool.data(),   eaVectorBool.data() +  eaVectorBool.size(), false);

			if(i == 1)
				Benchmark::AddResult("algorithm/fill/bool[]", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test fill_n
			///////////////////////////////

			TestFillNStd(stopwatch1,  eaVectorChar.data(), (int) eaVectorChar.size(), 'd'); // Intentionally use eaVectorBool for the array.
			TestFillNEa (stopwatch2,  eaVectorChar.data(), (int) eaVectorChar.size(), 'd');
			TestFillNStd(stopwatch1,  eaVectorChar.data(), (int) eaVectorChar.size(), 'd'); // Intentionally do this a second time, as we are finding 
			TestFillNEa (stopwatch2,  eaVectorChar.data(), (int) eaVectorChar.size(), 'd'); // the results are inconsistent otherwise.

			if(i == 1)
				Benchmark::AddResult("algorithm/fill_n/char[]", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestFillNStd(stopwatch1,  eaVectorBool.data(), (int) eaVectorBool.size(), false); // Intentionally use eaVectorBool for the array.
			TestFillNEa (stopwatch2,  eaVectorBool.data(), (int) eaVectorBool.size(), false);
			TestFillNStd(stopwatch1,  eaVectorBool.data(), (int) eaVectorBool.size(), false); // Intentionally do this a second time, as we are finding 
			TestFillNEa (stopwatch2,  eaVectorBool.data(), (int) eaVectorBool.size(), false); // the results are inconsistent otherwise.

			if(i == 1)
				Benchmark::AddResult("algorithm/fill_n/bool[]", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
		}
	}
}


void BenchmarkAlgorithm6(EASTLTest_Rand& /*rng*/, EA::StdC::Stopwatch& stopwatch1, EA::StdC::Stopwatch& stopwatch2)
{
	// We allocate this on the heap because some platforms don't always have enough stack space for this.
	std::vector<LargePOD>*   pstdVectorLP1 = new std::vector<LargePOD>(100);
	std::vector<LargePOD>*   pstdVectorLP2 = new std::vector<LargePOD>(100);
	eastl::vector<LargePOD>* peaVectorLP1  = new eastl::vector<LargePOD>(100);
	eastl::vector<LargePOD>* peaVectorLP2  = new eastl::vector<LargePOD>(100);

	// Aliases.
	std::vector<LargePOD>&   stdVectorLP1 = *pstdVectorLP1;
	std::vector<LargePOD>&   stdVectorLP2 = *pstdVectorLP2;
	eastl::vector<LargePOD>& eaVectorLP1  = *peaVectorLP1;
	eastl::vector<LargePOD>& eaVectorLP2  = *peaVectorLP2;

	for(int i = 0; i < 2; i++)
	{
		///////////////////////////////
		// Test copy
		///////////////////////////////

		TestCopyStd(stopwatch1, stdVectorLP1.begin(), stdVectorLP1.end(), stdVectorLP2.begin());
		TestCopyEa (stopwatch2,  eaVectorLP1.begin(),  eaVectorLP1.end(),  eaVectorLP2.begin());

		if(i == 1)
			Benchmark::AddResult("algorithm/copy/vector<LargePOD>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


		///////////////////////////////
		// Test copy_backward
		///////////////////////////////

		TestCopyBackwardStd(stopwatch1, stdVectorLP1.begin(), stdVectorLP1.end(), stdVectorLP2.end());
		TestCopyBackwardEa (stopwatch2,  eaVectorLP1.begin(),  eaVectorLP1.end(),  eaVectorLP2.end());

		if(i == 1)
			Benchmark::AddResult("algorithm/copy_backward/vector<LargePOD>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
	}

	delete pstdVectorLP1;
	delete pstdVectorLP2;
	delete peaVectorLP1;
	delete peaVectorLP2;
}


void BenchmarkAlgorithm7(EASTLTest_Rand& /*rng*/, EA::StdC::Stopwatch& stopwatch1, EA::StdC::Stopwatch& stopwatch2)
{
	{
		std::list<TestObject>     stdListTO(10000);
		eastl::list<TestObject>   eaListTO(10000);

		std::vector<TestObject>   stdVectorTO(10000);
		eastl::vector<TestObject> eaVectorTO(10000);

		for(int i = 0; i < 2; i++)
		{
			///////////////////////////////
			// Test reverse
			///////////////////////////////

			TestReverseStd(stopwatch1, stdListTO.begin(), stdListTO.end());
			TestReverseEa (stopwatch2,  eaListTO.begin(),  eaListTO.end());

			if(i == 1)
				Benchmark::AddResult("algorithm/reverse/list<TestObject>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestReverseStd(stopwatch1, stdVectorTO.begin(), stdVectorTO.end());
			TestReverseEa (stopwatch2,  eaVectorTO.begin(),  eaVectorTO.end());

			if(i == 1)
				Benchmark::AddResult("algorithm/reverse/vector<TestObject>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
		}
	}

	{
		// Create some containers and seed them with incremental values (i.e. 0, 1, 2, 3...).
		eastl::slist<int32_t> eaSlistIntLarge(10000);
		eastl::generate(eaSlistIntLarge.begin(), eaSlistIntLarge.end(), GenerateIncrementalIntegers<int32_t>());


		std::vector< SizedPOD<32> > stdVectorLargePod32(10000);
		for(std::vector< SizedPOD<32> >::iterator it = stdVectorLargePod32.begin(); it != stdVectorLargePod32.end(); ++it)
			memset(&*it, 0, sizeof(SizedPOD<32>));
		eastl::vector< SizedPOD<32> > eaVectorLargePod32(10000);
		for(eastl::vector< SizedPOD<32> >::iterator it = eaVectorLargePod32.begin(); it != eaVectorLargePod32.end(); ++it)
			memset(&*it, 0, sizeof(SizedPOD<32>));

		std::list<int32_t> stdListIntLarge(10000);
		eastl::generate(stdListIntLarge.begin(), stdListIntLarge.end(), GenerateIncrementalIntegers<int32_t>());

		eastl::list<int32_t> eaListIntLarge(10000);
		eastl::generate(eaListIntLarge.begin(), eaListIntLarge.end(), GenerateIncrementalIntegers<int32_t>());


		std::vector<int32_t> stdVectorIntLarge(10000);
		eastl::generate(stdVectorIntLarge.begin(), stdVectorIntLarge.end(), GenerateIncrementalIntegers<int32_t>());

		eastl::vector<int32_t> eaVectorIntLarge(10000);
		eastl::generate(eaVectorIntLarge.begin(), eaVectorIntLarge.end(), GenerateIncrementalIntegers<int32_t>());


		std::list<int32_t> stdListIntSmall(10);
		eastl::generate(stdListIntLarge.begin(), stdListIntLarge.end(), GenerateIncrementalIntegers<int32_t>());

		eastl::list<int32_t> eaListIntSmall(10);
		eastl::generate(eaListIntLarge.begin(), eaListIntLarge.end(), GenerateIncrementalIntegers<int32_t>());


		std::vector<int32_t> stdVectorIntSmall(10);
		eastl::generate(stdVectorIntLarge.begin(), stdVectorIntLarge.end(), GenerateIncrementalIntegers<int32_t>());

		eastl::vector<int32_t> eaVectorIntSmall(10);
		eastl::generate(eaVectorIntLarge.begin(), eaVectorIntLarge.end(), GenerateIncrementalIntegers<int32_t>());



		std::list<TestObject> stdListTOLarge(10000);
		eastl::generate(stdListTOLarge.begin(), stdListTOLarge.end(), GenerateIncrementalIntegers<TestObject>());

		eastl::list<TestObject> eaListTOLarge(10000);
		eastl::generate(eaListTOLarge.begin(), eaListTOLarge.end(), GenerateIncrementalIntegers<TestObject>());


		std::vector<TestObject> stdVectorTOLarge(10000);
		eastl::generate(stdVectorTOLarge.begin(), stdVectorTOLarge.end(), GenerateIncrementalIntegers<TestObject>());

		eastl::vector<TestObject> eaVectorTOLarge(10000);
		eastl::generate(eaVectorTOLarge.begin(), eaVectorTOLarge.end(), GenerateIncrementalIntegers<TestObject>());


		std::list<TestObject> stdListTOSmall(10);
		eastl::generate(stdListTOSmall.begin(), stdListTOSmall.end(), GenerateIncrementalIntegers<TestObject>());

		eastl::list<TestObject> eaListTOSmall(10);
		eastl::generate(eaListTOSmall.begin(), eaListTOSmall.end(), GenerateIncrementalIntegers<TestObject>());


		std::vector<TestObject> stdVectorTOSmall(10);
		eastl::generate(stdVectorTOSmall.begin(), stdVectorTOSmall.end(), GenerateIncrementalIntegers<TestObject>());

		eastl::vector<TestObject> eaVectorTOSmall(10);
		eastl::generate(eaVectorTOSmall.begin(), eaVectorTOSmall.end(), GenerateIncrementalIntegers<TestObject>());
		

		for(int i = 0; i < 2; i++)
		{
			///////////////////////////////
			// Test reverse
			///////////////////////////////

			// There is no guaranteed Standard Library forward_list or slist.
			TestRotateEa (stopwatch2,  eaSlistIntLarge.begin(), eastl::next( eaSlistIntLarge.begin(),  (eaSlistIntLarge.size() / 2) - 1),  eaSlistIntLarge.end());
			if(i == 1)
				Benchmark::AddResult("algorithm/rotate/slist<int32_t> large", stopwatch1.GetUnits(), 0 /* untested */, stopwatch2.GetElapsedTime());



			TestRotateStd(stopwatch1, stdVectorLargePod32.begin(), std__::next(stdVectorLargePod32.begin(), (stdVectorLargePod32.size() / 2) - 1), stdVectorLargePod32.end());
			TestRotateEa (stopwatch2,  eaVectorLargePod32.begin(), eastl::next( eaVectorLargePod32.begin(),  (eaVectorLargePod32.size() / 2) - 1),  eaVectorLargePod32.end());
			if(i == 1)
				Benchmark::AddResult("algorithm/rotate/vector<SizedPOD<32>> large", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());




			TestRotateStd(stopwatch1, stdListIntLarge.begin(), std__::next(stdListIntLarge.begin(), (stdListIntLarge.size() / 2) - 1), stdListIntLarge.end());
			TestRotateEa (stopwatch2,  eaListIntLarge.begin(), eastl::next( eaListIntLarge.begin(),  (eaListIntLarge.size() / 2) - 1),  eaListIntLarge.end());
			if(i == 1)
				Benchmark::AddResult("algorithm/rotate/list<int32_t> large", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestRotateStd(stopwatch1, stdVectorIntLarge.begin(), std__::next(stdVectorIntLarge.begin(), (stdVectorIntLarge.size() / 2) - 1), stdVectorIntLarge.end());
			TestRotateEa (stopwatch2,  eaVectorIntLarge.begin(), eastl::next( eaVectorIntLarge.begin(),  (eaVectorIntLarge.size() / 2) - 1),  eaVectorIntLarge.end());
			if(i == 1)
				Benchmark::AddResult("algorithm/rotate/vector<int32_t large>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());



			TestRotateStd(stopwatch1, stdListIntSmall.begin(), std__::next(stdListIntSmall.begin(), (stdListIntSmall.size() / 2) - 1), stdListIntSmall.end());
			TestRotateEa (stopwatch2,  eaListIntSmall.begin(), eastl::next( eaListIntSmall.begin(),  (eaListIntSmall.size() / 2) - 1),  eaListIntSmall.end());
			if(i == 1)
				Benchmark::AddResult("algorithm/rotate/list<int32_t> small", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestRotateStd(stopwatch1, stdVectorIntSmall.begin(), std__::next(stdVectorIntSmall.begin(), (stdVectorIntSmall.size() / 2) - 1), stdVectorIntSmall.end());
			TestRotateEa (stopwatch2,  eaVectorIntSmall.begin(), eastl::next( eaVectorIntSmall.begin(),  (eaVectorIntSmall.size() / 2) - 1),  eaVectorIntSmall.end());
			if(i == 1)
				Benchmark::AddResult("algorithm/rotate/vector<int32_t small>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());



			TestRotateStd(stopwatch1, stdListTOLarge.begin(), std__::next(stdListTOLarge.begin(), (stdListTOLarge.size() / 2) - 1), stdListTOLarge.end());
			TestRotateEa (stopwatch2,  eaListTOLarge.begin(), eastl::next( eaListTOLarge.begin(),  (eaListTOLarge.size() / 2) - 1),  eaListTOLarge.end());
			if(i == 1)
				Benchmark::AddResult("algorithm/rotate/list<TestObject large>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestRotateStd(stopwatch1, stdVectorTOLarge.begin(), std__::next(stdVectorTOLarge.begin(), (stdVectorTOLarge.size() / 2) - 1), stdVectorTOLarge.end());
			TestRotateEa (stopwatch2,  eaVectorTOLarge.begin(), eastl::next( eaVectorTOLarge.begin(),  (eaVectorTOLarge.size() / 2) - 1),  eaVectorTOLarge.end());
			if(i == 1)
				Benchmark::AddResult("algorithm/rotate/vector<TestObject large>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());



			TestRotateStd(stopwatch1, stdListTOSmall.begin(), std__::next(stdListTOSmall.begin(), (stdListTOSmall.size() / 2) - 1), stdListTOSmall.end());
			TestRotateEa (stopwatch2,  eaListTOSmall.begin(), eastl::next( eaListTOSmall.begin(),  (eaListTOSmall.size() / 2) - 1),  eaListTOSmall.end());
			if(i == 1)
				Benchmark::AddResult("algorithm/rotate/list<TestObject small>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			TestRotateStd(stopwatch1, stdVectorTOSmall.begin(), std__::next(stdVectorTOSmall.begin(), (stdVectorTOSmall.size() / 2) - 1), stdVectorTOSmall.end());
			TestRotateEa (stopwatch2,  eaVectorTOSmall.begin(), eastl::next( eaVectorTOSmall.begin(),  (eaVectorTOSmall.size() / 2) - 1),  eaVectorTOSmall.end());
			if(i == 1)
				Benchmark::AddResult("algorithm/rotate/vector<TestObject small>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
		}
	}
}

void BenchmarkAlgorithm8(EASTLTest_Rand& rng, EA::StdC::Stopwatch& stopwatch1, EA::StdC::Stopwatch& stopwatch2)
{
	const uint32_t ElementCount = 10000;

	eastl::vector<int> srcVecA(ElementCount);
	eastl::vector<int> srcVecB(ElementCount);

	std::vector<int> stdVecAInt(ElementCount);
	std::vector<int> stdVecBInt(ElementCount);
	std::vector<int> stdVecOutInt(2 * ElementCount);
	std::vector<TestObject> stdVecATestObject(ElementCount);
	std::vector<TestObject> stdVecBTestObject(ElementCount);
	std::vector<TestObject> stdVecOutTestObject(2 * ElementCount);

	eastl::vector<int> eaVecAInt(ElementCount);
	eastl::vector<int> eaVecBInt(ElementCount);
	eastl::vector<int> eaVecOutInt(2 * ElementCount);
	eastl::vector<TestObject> eaVecATestObject(ElementCount);
	eastl::vector<TestObject> eaVecBTestObject(ElementCount);
	eastl::vector<TestObject> eaVecOutTestObject(2 * ElementCount);

	// Note:
	//   In some cases the compiler may generate branch free code for the loop body of merge.
	//   In this situation the performance of merging data that has a random merge selection (i.e. the chance that the smallest
	//   element is taken from the first or second list is essentially random) is the same as merging data where the choice of
	//   which list has the smallest element is predictable.
	//   However, if the compiler doesn't generate branch free code, then the performance of merge will suffer from branch
	//   misprediction when merging random data and will benefit greatly when misprediction is rare.
	//   This benchmark is aimed at highlighting what sort of code is being generated, and also showing the impact of
	//   predictability of the comparisons performed during merge.  The branch predictablity /can/ have a large impact
	//   on merge sort performance.

	// 'unpred' is the case where the comparison is unpredictable
	// 'pred' is the case where the comparison is mostly predictable
	const char* patternDescriptions[][2] =
	{
		{
			"algorithm/merge/vector<int> (unpred)",
			"algorithm/merge/vector<int> (pred)",
		},
		{
			"algorithm/merge/vector<TestObject> (unpred)",
			"algorithm/merge/vector<TestObject> (pred)",
		},
	};

	enum Pattern
	{
		P_Random,
		P_Predictable,
		P_Count
	};

	for (int pattern = 0; pattern < P_Count; pattern++)
	{
		if (pattern == P_Random)
		{
			eastl::generate(srcVecA.begin(), srcVecA.end(), [&]{ return int(rng()); });
			eastl::sort(srcVecA.begin(), srcVecA.end());
			eastl::generate(srcVecB.begin(), srcVecB.end(), [&] { return int(rng()); });
			eastl::sort(srcVecB.begin(), srcVecB.end());
		}
		else if (pattern == P_Predictable)
		{
			// The data pattern means that a simple/naive algorithm will select 'runLen' values
			// from one list, and then 'runLen' values from the other list (alternating back and forth).
			// Of course, a merge algorithm that is more complicated might have a different order of
			// comparison.
			const int runLen = 32;
			for (int i = 0; i < ElementCount; i++)
			{
				int baseValue = ((i / runLen) * 2 * runLen) + (i % (runLen));
				srcVecA[i] = baseValue;
				srcVecB[i] = baseValue + runLen;
			}
		}

		///////////////////////////////
		// Test merge
		///////////////////////////////
		for (int i = 0; i < 2; i++)
		{
			eastl::copy(srcVecA.begin(), srcVecA.end(), stdVecAInt.begin());
			eastl::copy(srcVecB.begin(), srcVecB.end(), stdVecBInt.begin());
			eastl::copy(srcVecA.begin(), srcVecA.end(), eaVecAInt.begin());
			eastl::copy(srcVecB.begin(), srcVecB.end(), eaVecBInt.begin());
			TestMergeStd(stopwatch1, stdVecAInt.begin(), stdVecAInt.end(), stdVecBInt.begin(), stdVecBInt.end(), stdVecOutInt.begin());
			TestMergeEa(stopwatch2, eaVecAInt.begin(), eaVecAInt.end(), eaVecBInt.begin(), eaVecBInt.end(), eaVecOutInt.begin());

			if (i == 1)
			{
				Benchmark::AddResult(patternDescriptions[0][pattern], stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
			}

			for (int j = 0; j < ElementCount; j++)
			{
				stdVecATestObject[j] = TestObject(srcVecA[j]);
				stdVecBTestObject[j] = TestObject(srcVecB[j]);
				eaVecATestObject[j] = TestObject(srcVecA[j]);
				eaVecBTestObject[j] = TestObject(srcVecB[j]);
			}
			TestMergeStd(stopwatch1, stdVecATestObject.begin(), stdVecATestObject.end(), stdVecBTestObject.begin(), stdVecBTestObject.end(), stdVecOutTestObject.begin());
			TestMergeEa(stopwatch2, eaVecATestObject.begin(), eaVecATestObject.end(), eaVecBTestObject.begin(), eaVecBTestObject.end(), eaVecOutTestObject.begin());

			if (i == 1)
			{
				Benchmark::AddResult(patternDescriptions[1][pattern], stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
			}
		}
	}

}



void BenchmarkAlgorithm()
{
	EASTLTest_Printf("Algorithm\n");

	EASTLTest_Rand rng(EA::UnitTest::GetRandSeed());
	EA::StdC::Stopwatch  stopwatch1(EA::StdC::Stopwatch::kUnitsCPUCycles);
	EA::StdC::Stopwatch  stopwatch2(EA::StdC::Stopwatch::kUnitsCPUCycles);

	BenchmarkAlgorithm1(rng, stopwatch1, stopwatch2);
	BenchmarkAlgorithm2(rng, stopwatch1, stopwatch2);
	BenchmarkAlgorithm3(rng, stopwatch1, stopwatch2);
	BenchmarkAlgorithm4(rng, stopwatch1, stopwatch2);
	BenchmarkAlgorithm5(rng, stopwatch1, stopwatch2);
	BenchmarkAlgorithm6(rng, stopwatch1, stopwatch2);
	BenchmarkAlgorithm7(rng, stopwatch1, stopwatch2);
	BenchmarkAlgorithm8(rng, stopwatch1, stopwatch2);
}
















