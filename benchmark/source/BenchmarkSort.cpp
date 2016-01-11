/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLBenchmark.h"
#include "EASTLTest.h"
#include <EAStdC/EAStopwatch.h>
#include <EASTL/sort.h>
#include <EASTL/bonus/sort_extra.h>
#include <EASTL/vector.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
	#pragma warning(disable: 4350)
#endif
#include <algorithm>
#include <functional>
#include <vector>
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
			//return *(const uint64_t*)&vp1 < *(const uint64_t*)&vp2;
			return (vp1.key == vp2.key) ? (vp1.v < vp2.v) : (vp1.key < vp2.key);
		}
	};

	bool operator<(const ValuePair& vp1, const ValuePair& vp2)
	{
		//return *(const uint64_t*)&vp1 < *(const uint64_t*)&vp2;
		return (vp1.key == vp2.key) ? (vp1.v < vp2.v) : (vp1.key < vp2.key);
	}

	bool operator==(const ValuePair& vp1, const ValuePair& vp2)
	{
		//return *(const uint64_t*)&vp1 == *(const uint64_t*)&vp2;
		return (vp1.key == vp2.key) && (vp1.v == vp2.v);
	}

}

// VPCompareC
// Useful for testing the the C qsort function.
int VPCompareC(const void* elem1, const void* elem2)
	{ return (int)(*(const uint64_t*)elem1 - *(const uint64_t*)elem2); }





typedef std::vector<ValuePair>    StdVectorVP;
typedef eastl::vector<ValuePair>  EaVectorVP;

typedef std::vector<uint32_t>     StdVectorInt;
typedef eastl::vector<uint32_t>   EaVectorInt;

typedef std::vector<TestObject>   StdVectorTO;
typedef eastl::vector<TestObject> EaVectorTO;



namespace
{
	#ifndef EA_PREFIX_NO_INLINE
		#ifdef _MSC_VER
			#define EA_PREFIX_NO_INLINE EA_NO_INLINE
			#define EA_POSTFIX_NO_INLINE
		#else
			#define EA_PREFIX_NO_INLINE
			#define EA_POSTFIX_NO_INLINE EA_NO_INLINE
		#endif
	#endif

	EA_PREFIX_NO_INLINE void TestQuickSortStdVP (EA::StdC::Stopwatch& stopwatch, StdVectorVP&  stdVectorVP)  EA_POSTFIX_NO_INLINE;
	EA_PREFIX_NO_INLINE void TestQuickSortEaVP  (EA::StdC::Stopwatch& stopwatch, EaVectorVP&   eaVectorVP)   EA_POSTFIX_NO_INLINE;
	EA_PREFIX_NO_INLINE void TestQuickSortStdInt(EA::StdC::Stopwatch& stopwatch, StdVectorInt& stdVectorInt) EA_POSTFIX_NO_INLINE;
	EA_PREFIX_NO_INLINE void TestQuickSortEaInt (EA::StdC::Stopwatch& stopwatch, EaVectorInt&  eaVectorInt)  EA_POSTFIX_NO_INLINE;
	EA_PREFIX_NO_INLINE void TestQuickSortStdTO (EA::StdC::Stopwatch& stopwatch, StdVectorTO&  stdVectorTO)  EA_POSTFIX_NO_INLINE;
	EA_PREFIX_NO_INLINE void TestQuickSortEaTO  (EA::StdC::Stopwatch& stopwatch, EaVectorTO&   eaVectorTO)   EA_POSTFIX_NO_INLINE;



	void TestQuickSortStdVP(EA::StdC::Stopwatch& stopwatch, StdVectorVP& stdVectorVP)
	{
		stopwatch.Restart();
		std::sort(stdVectorVP.begin(), stdVectorVP.end());
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)stdVectorVP[0].key);
	}


	void TestQuickSortEaVP(EA::StdC::Stopwatch& stopwatch, EaVectorVP& eaVectorVP)
	{
		stopwatch.Restart();
		eastl::quick_sort(eaVectorVP.begin(), eaVectorVP.end());
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)eaVectorVP[0].key);
	}




	void TestQuickSortStdInt(EA::StdC::Stopwatch& stopwatch, StdVectorInt& stdVectorInt)
	{
		stopwatch.Restart();
		std::sort(stdVectorInt.begin(), stdVectorInt.end());
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)stdVectorInt[0]);
	}


	void TestQuickSortEaInt(EA::StdC::Stopwatch& stopwatch, EaVectorInt& eaVectorInt)
	{
		stopwatch.Restart();
		eastl::quick_sort(eaVectorInt.begin(), eaVectorInt.end());
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)eaVectorInt[0]);
	}




	void TestQuickSortStdTO(EA::StdC::Stopwatch& stopwatch, StdVectorTO& stdVectorTO)
	{
		stopwatch.Restart();
		std::sort(stdVectorTO.begin(), stdVectorTO.end());
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)stdVectorTO[0].mX);
	}


	void TestQuickSortEaTO(EA::StdC::Stopwatch& stopwatch, EaVectorTO& eaVectorTO)
	{
		stopwatch.Restart();
		eastl::quick_sort(eaVectorTO.begin(), eaVectorTO.end());
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)eaVectorTO[0].mX);
	}

} // namespace


namespace 
{
	enum SortFunctionType
	{
		sf_qsort,                   // C qsort
		sf_shell_sort,              // eastl::shell_sort.
		sf_heap_sort,               // eastl::heap_sort
		sf_merge_sort,              // eastl::merge_sort
		sf_merge_sort_buffer,       // eastl::merge_sort_buffer
		sf_comb_sort,               // eastl::comb_sort
		sf_bubble_sort,             // eastl::bubble_sort
		sf_selection_sort,          // eastl::selection_sort
		sf_shaker_sort,             // eastl::shaker_sort
		sf_quick_sort,              // eastl::quick_sort
		sf_tim_sort,                // eastl::tim_sort
		sf_insertion_sort,          // eastl::insertion_sort
		sf_std_sort,                // std::sort
		sf_radix_sort,              // eastl::radix_sort (unconventional sort)
		sf_count                    // 
	};

	const char* GetSortFunctionName(int sortFunctionType)
	{
		switch(sortFunctionType)
		{
			case sf_quick_sort:
				return "eastl::sort";

			case sf_tim_sort:
				return "eastl::tim_sort";

			case sf_insertion_sort:
				return "eastl::insertion_sort";

			case sf_shell_sort:
				return "eastl::shell_sort";

			case sf_heap_sort:
				return "eastl::heap_sort";

			case sf_merge_sort:
				return "eastl::merge_sort";

			case sf_merge_sort_buffer:
				return "eastl::merge_sort_buffer";

			case sf_comb_sort:
				return "eastl::comb_sort";

			case sf_bubble_sort:
				return "eastl::bubble_sort";

			case sf_selection_sort:
				return "eastl::selection_sort";

			case sf_shaker_sort:
				return "eastl::shaker_sort";

			case sf_radix_sort:
				return "eastl::radix_sort";

			case sf_qsort:
				return "qsort";

			case sf_std_sort:
				return "std::sort";

			default:
				return "unknown";
		}
	}


	enum RandomizationType
	{
		kRandom,                    // Completely random data.
		kOrdered,                   // Already sorted.
		kMostlyOrdered,             // Partly sorted already.
		kRandomizationTypeCount
	};

	const char* GetRandomizationTypeName(int randomizationType)
	{
		switch(randomizationType)
		{
			case kRandom:
				return "random";

			case kOrdered:
				return "ordered";

			case kMostlyOrdered:
				return "mostly ordered";

			default:
				return "unknown";
		}
	}

	template <typename ElementType, typename RandomType>
	void Randomize(eastl::vector<ElementType>& v, EA::UnitTest::RandGenT<RandomType>& rng, RandomizationType type)
	{
		typedef RandomType value_type;

		switch(type)
		{
			default:
			case kRandomizationTypeCount: // We specify this only to avoid a compiler warning about not testing for it.
			case kRandom:
			{
				eastl::generate(v.begin(), v.end(), rng);
				break;
			}

			case kOrdered:
			{
				for(eastl_size_t i = 0; i < v.size(); ++i)
					v[i] = value_type((value_type)i);   // Note that value_type may be a struct and not an integer. Thus the casting and construction here.
				break;
			}

			case kMostlyOrdered:
			{
				for(eastl_size_t i = 0; i < v.size(); ++i)
					v[i] = value_type((value_type)i);   // Note that value_type may be a struct and not an integer. Thus the casting and construction here.

				// We order random segments.
				// The algorithm below in practice will make slightly more than kPercentOrdered be ordered.
				const eastl_size_t kPercentOrdered = 80; // In actuality, due to statistics, the actual ordered percent will be about 82-85%.

				for(eastl_size_t n = 0, s = v.size(), nEnd = ((s < (100 - kPercentOrdered)) ? 1 : (s / (100 - kPercentOrdered))); n < nEnd; n++)
				{
					eastl_size_t i = rng.mRand.RandLimit((uint32_t)s);
					eastl_size_t j = rng.mRand.RandLimit((uint32_t)s);

					eastl::swap(v[i], v[j]);
				}

				break;
			}
		}
	}


	char gSlowAssignBuffer1[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 /* ... */};
	char gSlowAssignBuffer2[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 /* ... */};


	// SlowAssign
	// Implements an object which has slow assign performance.
	template <typename T>
	struct SlowAssign
	{
		T x;

		static int nAssignCount;

		SlowAssign()
			{ x = 0; memcpy(gSlowAssignBuffer1, gSlowAssignBuffer2, sizeof(gSlowAssignBuffer1)); }

		SlowAssign(const SlowAssign& sa)
			{ ++nAssignCount; x = sa.x; memcpy(gSlowAssignBuffer1, gSlowAssignBuffer2, sizeof(gSlowAssignBuffer1)); }

		SlowAssign& operator=(const SlowAssign& sa)
			{ ++nAssignCount; x = sa.x; memcpy(gSlowAssignBuffer1, gSlowAssignBuffer2, sizeof(gSlowAssignBuffer1)); return *this; }

		SlowAssign& operator=(int a)
			{ x = (T)a; return *this; }

		static void Reset()
			{ nAssignCount = 0; }
	};

	template<> int SlowAssign<int32_t>::nAssignCount = 0;

	template <typename T>
	bool operator <(const SlowAssign<T>& a, const SlowAssign<T>& b)
		{ return a.x < b.x; }


	// SlowCompare
	// Implements a compare which is N time slower than a simple integer compare.
	template <typename T>
	struct SlowCompare
	{
		static int nCompareCount;

		bool operator()(T a, T b)
		{
			++nCompareCount;

			return (a < b) &&   // It happens that gSlowAssignBuffer1 is always zeroed.
				   (gSlowAssignBuffer1[0] == 0) && (gSlowAssignBuffer1[1] == 0) &&
				   (gSlowAssignBuffer1[1] == 0) && (gSlowAssignBuffer1[2] == 0) &&
				   (gSlowAssignBuffer1[4] == 0) && (gSlowAssignBuffer1[5] == 0);
		}

		static void Reset()
			{ nCompareCount = 0; }
	};

	template<> int SlowCompare<int32_t>::nCompareCount = 0;


	// qsort callback functions
	// qsort compare function returns negative if b > a and positive if a > b.
	template <typename T>
	int CompareInteger(const void* a, const void* b)
	{
		// Even though you see the following in Internet example code, it doesn't work!
		// The reason is that it works only if a and b are both >= 0, otherwise large 
		// values can cause integer register wraparound. A similar kind of problem happens
		// if you try to do the same thing with floating point value compares.
		// See http://www.akalin.cx/2006/06/23/on-the-qsort-comparison-function/
		// Internet exmaple code:
		//     return *(const int32_t*)a - *(const int32_t*)b;

		// This double comparison might seem like it's crippling qsort against the 
		// STL-based sorts which do a single compare. But consider that the returning
		// of -1, 0, +1 gives qsort more information, and its logic takes advantage
		// of that. 
		if(*(const T*)a < *(const T*)b)
			return -1;
		if(*(const T*)a > *(const T*)b)
			return +1;
		return 0;
	}


	int SlowCompareInt32(const void* a, const void* b)
	{
		++SlowCompare<int32_t>::nCompareCount;

		// This code is similar in performance to the C++ SlowCompare template functor above.
		if((gSlowAssignBuffer1[0] == 0) && (gSlowAssignBuffer1[1] == 0) &&
		   (gSlowAssignBuffer1[1] == 0) && (gSlowAssignBuffer1[2] == 0) &&
		   (gSlowAssignBuffer1[4] == 0) && (gSlowAssignBuffer1[5] == 0))
		{
			if(*(const int32_t*)a < *(const int32_t*)b)
				return -1;
			if(*(const int32_t*)a > *(const int32_t*)b)
				return +1;
		}

		return 0;
	}

	// Used in the radix_sort tests below.
	template <typename T>
	struct RadixSortElement
	{
		typedef T radix_type;
		radix_type mKey; // eastl::radix_sort requires an unsigned key type.
	};

} // namespace



// Sizes of arrays to be sorted.
const eastl_size_t kSizesCount = 4;
const eastl_size_t kSizes[kSizesCount] = { 10, 100, 1000, 10000 };


struct BenchmarkResult
{
	uint64_t mTime;
	uint64_t mCompareCount;
	uint64_t mAssignCount;

	BenchmarkResult() : mTime(0), mCompareCount(0), mAssignCount(0){ }
};

BenchmarkResult gResults[kRandomizationTypeCount][kSizesCount][sf_count];




int CompareSortPerformance()
{
	int nErrorCount = 0;

	EA::UnitTest::ReportVerbosity(2, "Sort comparison\n");
	EA::UnitTest::ReportVerbosity(2, "Random seed = %u\n", (unsigned)EA::UnitTest::GetRandSeed());

	EA::UnitTest::RandGenT<int32_t> rng(EA::UnitTest::GetRandSeed());
	EA::StdC::Stopwatch             stopwatch(EA::StdC::Stopwatch::kUnitsCPUCycles);
	EA::StdC::Stopwatch             stopwatchGlobal(EA::StdC::Stopwatch::kUnitsSeconds);
	const eastl_size_t              kArraySizeMax = 50000;
	const int                       kRunCount = 4;

	#if !defined(EA_DEBUG)
		EA::UnitTest::SetHighThreadPriority();
	#endif


	{
		// Regular speed test.
		// In this case we test the sorting of integral values.
		// This is probably the most common type of comparison.
		EA::UnitTest::ReportVerbosity(2, "Sort comparison: Regular speed test\n");

		typedef uint32_t                      ElementType;
		typedef eastl::less<ElementType>      CompareFunction;
		typedef RadixSortElement<ElementType> RadixSortElementType;

		eastl::string sOutput; sOutput.set_capacity(100000);
		ElementType*  pBuffer = new ElementType[kArraySizeMax];

		memset(gResults, 0, sizeof(gResults));

		stopwatchGlobal.Restart();

		for(int c = 0; c < kRunCount; c++)
		{
			for(int i = 0; i < kRandomizationTypeCount; i++)
			{
				for(size_t sizeType = 0; sizeType < EAArrayCount(kSizes); sizeType++)
				{
					const eastl_size_t size = kSizes[sizeType];

					for(int sortFunction = 0; sortFunction < sf_count; sortFunction++)
					{
						eastl::vector<ElementType> v(size);

						rng.SetSeed(EA::UnitTest::GetRandSeed());
						Randomize(v, rng, (RandomizationType)i);

						switch (sortFunction)
						{
							case sf_quick_sort:
								stopwatch.Restart();
								eastl::quick_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_tim_sort:
								stopwatch.Restart();
								eastl::tim_sort_buffer(v.begin(), v.end(), pBuffer, CompareFunction());
								stopwatch.Stop();
								break;

							case sf_insertion_sort:
								stopwatch.Restart();
								eastl::insertion_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_shell_sort:
								stopwatch.Restart();
								eastl::shell_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_heap_sort:
								stopwatch.Restart();
								eastl::heap_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_merge_sort:
								stopwatch.Restart();
								eastl::merge_sort(v.begin(), v.end(), *get_default_allocator((EASTLAllocatorType*)NULL), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_merge_sort_buffer:
								stopwatch.Restart();
								eastl::merge_sort_buffer(v.begin(), v.end(), pBuffer, CompareFunction());
								stopwatch.Stop();
								break;

							case sf_comb_sort:
								stopwatch.Restart();
								eastl::comb_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_bubble_sort:
								stopwatch.Restart();
								eastl::bubble_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_selection_sort:
								stopwatch.Restart();
								eastl::selection_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_shaker_sort:
								stopwatch.Restart();
								eastl::shaker_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_radix_sort:
							{
								eastl::vector<RadixSortElementType> vr(size);
								eastl::vector<RadixSortElementType> vrBuffer(size);
								for(eastl_size_t r = 0; r < size; r++) // Normally you wouldn't need to do this copying, but it allows this benchmark code to be cleaner.
									vr[r].mKey = v[r];

								stopwatch.Restart();
								eastl::radix_sort<RadixSortElementType*, eastl::Internal::extract_radix_key<RadixSortElementType> >(vr.begin(), vr.end(), &vrBuffer[0]);
								stopwatch.Stop();

								for(eastl_size_t r = 0; r < size; r++)  // Normally you wouldn't need to do this copying, but it allows this benchmark code to be cleaner.
									 v[r] = vr[r].mKey;
								break;
							}

							case sf_qsort:
								stopwatch.Restart();
								qsort(&v[0], (size_t)v.size(), sizeof(ElementType), CompareInteger<ElementType>);
								stopwatch.Stop();
								break;

							case sf_std_sort:
								stopwatch.Restart();
								std::sort(v.data(), v.data() + v.size(), std::less<ElementType>());
								stopwatch.Stop();
								break;
						}

						const uint64_t elapsedTime = (uint64_t)stopwatch.GetElapsedTime();

						// If this result was faster than a previously fastest result, record this one instead.
						if((c == 0) || (elapsedTime < gResults[i][sizeType][sortFunction].mTime))
							gResults[i][sizeType][sortFunction].mTime = elapsedTime;

						VERIFY(eastl::is_sorted(v.begin(), v.end()));

					} // for each sort function...

				} // for each size type...

			} // for each randomization type...

		} // for each run

		EA::UnitTest::ReportVerbosity(2, "Total time: %.2f s\n", stopwatchGlobal.GetElapsedTimeFloat());

		delete[] pBuffer;

		// Now print the results.
		for(int i = 0; i < kRandomizationTypeCount; i++)
		{
			for(size_t sizeType = 0; sizeType < EAArrayCount(kSizes); sizeType++)
			{
				const eastl_size_t size = kSizes[sizeType];

				for(int sortFunction = 0; sortFunction < sf_count; sortFunction++)
					sOutput.append_sprintf("%25s, %14s, Size: %6u, Time: %11I64u ticks\n", GetSortFunctionName(sortFunction), GetRandomizationTypeName(i), (unsigned)size, gResults[i][sizeType][sortFunction].mTime);
				sOutput.append("\n");
			}
		}

		EA::UnitTest::ReportVerbosity(2, "%s\n\n", sOutput.c_str());
	}

	{
		// Do a speed test for the case of slow compares.
		// By this we mean to compare sorting speeds when the comparison of elements is slow.
		// Sort functions use element comparison to tell where elements go and use element 
		// movement to get them there. But some sorting functions accomplish sorting performance by 
		// minimizing the amount of movement, some minimize the amount of comparisons, and the
		// best do a good job of minimizing both.
		EA::UnitTest::ReportVerbosity(2, "Sort comparison: Slow compare speed test\n");

		typedef int32_t                  ElementType;
		typedef SlowCompare<ElementType> CompareFunction;

		eastl::string sOutput; sOutput.set_capacity(100000);
		ElementType*  pBuffer = new ElementType[kArraySizeMax];

		memset(gResults, 0, sizeof(gResults));

		stopwatchGlobal.Restart();

		for(int c = 0; c < kRunCount; c++)
		{
			for(int i = 0; i < kRandomizationTypeCount; i++)
			{
				for(size_t sizeType = 0; sizeType < EAArrayCount(kSizes); sizeType++)
				{
					const eastl_size_t size = kSizes[sizeType];

					for(int sortFunction = 0; sortFunction < sf_count; sortFunction++)
					{
						eastl::vector<ElementType> v(size);

						rng.SetSeed(EA::UnitTest::GetRandSeed());
						Randomize(v, rng, (RandomizationType)i);
						CompareFunction::Reset();

						switch (sortFunction)
						{
							case sf_quick_sort:
								stopwatch.Restart();
								eastl::quick_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_tim_sort:
								stopwatch.Restart();
								eastl::tim_sort_buffer(v.begin(), v.end(), pBuffer, CompareFunction());
								stopwatch.Stop();
								break;

							case sf_insertion_sort:
								stopwatch.Restart();
								eastl::insertion_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_shell_sort:
								stopwatch.Restart();
								eastl::shell_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_heap_sort:
								stopwatch.Restart();
								eastl::heap_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_merge_sort:
								stopwatch.Restart();
								eastl::merge_sort(v.begin(), v.end(), *get_default_allocator((EASTLAllocatorType*)NULL), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_merge_sort_buffer:
								stopwatch.Restart();
								eastl::merge_sort_buffer(v.begin(), v.end(), pBuffer, CompareFunction());
								stopwatch.Stop();
								break;

							case sf_comb_sort:
								stopwatch.Restart();
								eastl::comb_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_bubble_sort:
								stopwatch.Restart();
								eastl::bubble_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_selection_sort:
								stopwatch.Restart();
								eastl::selection_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_shaker_sort:
								stopwatch.Restart();
								eastl::shaker_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_radix_sort:
								// We can't test this because what we need isn't exposed.
								break;

							case sf_qsort:
								stopwatch.Restart();
								qsort(&v[0], (size_t)v.size(), sizeof(ElementType), SlowCompareInt32);
								stopwatch.Stop();
								break;

							case sf_std_sort:
								stopwatch.Restart();
								std::sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;
						}

						if(sortFunction != sf_radix_sort)
						{
							const uint64_t elapsedTime = (uint64_t)stopwatch.GetElapsedTime();

							// If this result was faster than a previously fastest result, record this one instead.
							if((c == 0) || (elapsedTime < gResults[i][sizeType][sortFunction].mTime))
								gResults[i][sizeType][sortFunction].mTime = elapsedTime;

							gResults[i][sizeType][sortFunction].mCompareCount = (uint64_t)CompareFunction::nCompareCount;

							VERIFY(eastl::is_sorted(v.begin(), v.end()));
						}

					} // for each sort function...

				} // for each size type...

			} // for each randomization type...

		} // for each run

		EA::UnitTest::ReportVerbosity(2, "Total time: %.2f s\n", stopwatchGlobal.GetElapsedTimeFloat());

		delete[] pBuffer;

		// Now print the results.
		for(int i = 0; i < kRandomizationTypeCount; i++)
		{
			for(size_t sizeType = 0; sizeType < EAArrayCount(kSizes); sizeType++)
			{
				const eastl_size_t size = kSizes[sizeType];

				for(int sortFunction = 0; sortFunction < sf_count; sortFunction++)
				{
					if(sortFunction != sf_radix_sort)
						sOutput.append_sprintf("%25s, %14s, Size: %6u, Time: %11I64u ticks, Compares: %11I64u\n", GetSortFunctionName(sortFunction), GetRandomizationTypeName(i), (unsigned)size, gResults[i][sizeType][sortFunction].mTime, gResults[i][sizeType][sortFunction].mCompareCount);
				}

				sOutput.append("\n");
			}
		}

		EA::UnitTest::ReportVerbosity(2, "%s\n\n", sOutput.c_str());
	}

	{
		// Do a speed test for the case of slow assignment. 
		// By this we mean to compare sorting speeds when the movement of elements is slow.
		// Sort functions use element comparison to tell where elements go and use element 
		// movement to get them there. But some sorting functions accomplish sorting performance by 
		// minimizing the amount of movement, some minimize the amount of comparisons, and the
		// best do a good job of minimizing both.
		EA::UnitTest::ReportVerbosity(2, "Sort comparison: Slow assignment speed test\n");

		typedef SlowAssign<int32_t>      ElementType;
		typedef eastl::less<ElementType> CompareFunction;

		eastl::string sOutput; sOutput.set_capacity(100000);
		ElementType*  pBuffer = new ElementType[kArraySizeMax];

		memset(gResults, 0, sizeof(gResults));

		stopwatchGlobal.Restart();

		for(int c = 0; c < kRunCount; c++)
		{
			for(int i = 0; i < kRandomizationTypeCount; i++)
			{
				for(size_t sizeType = 0; sizeType < EAArrayCount(kSizes); sizeType++)
				{
					const eastl_size_t size = kSizes[sizeType];

					for(int sortFunction = 0; sortFunction < sf_count; sortFunction++)
					{
						eastl::vector<ElementType> v(size);

						Randomize<ElementType>(v, rng, (RandomizationType)i);
						ElementType::Reset();

						switch (sortFunction)
						{
							case sf_quick_sort:
								stopwatch.Restart();
								eastl::quick_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_tim_sort:
								stopwatch.Restart();
								eastl::tim_sort_buffer(v.begin(), v.end(), pBuffer, CompareFunction());
								stopwatch.Stop();
								break;

							case sf_insertion_sort:
								stopwatch.Restart();
								eastl::insertion_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_shell_sort:
								stopwatch.Restart();
								eastl::shell_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_heap_sort:
								stopwatch.Restart();
								eastl::heap_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_merge_sort:
								stopwatch.Restart();
								eastl::merge_sort(v.begin(), v.end(), *get_default_allocator((EASTLAllocatorType*)NULL), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_merge_sort_buffer:
								stopwatch.Restart();
								eastl::merge_sort_buffer(v.begin(), v.end(), pBuffer, CompareFunction());
								stopwatch.Stop();
								break;

							case sf_comb_sort:
								stopwatch.Restart();
								eastl::comb_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_bubble_sort:
								stopwatch.Restart();
								eastl::bubble_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_selection_sort:
								stopwatch.Restart();
								eastl::selection_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_shaker_sort:
								stopwatch.Restart();
								eastl::shaker_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_radix_sort:
								// We can't test this because what we need isn't exposed.
								break;

							case sf_qsort:
							// Can't implement this because the C standard library doesn't expose it. 
							// We could implement it by copying and modifying the source code.
								break;

							case sf_std_sort:
								stopwatch.Restart();
								std::sort(v.begin(), v.end(), std::less<ElementType>());
								stopwatch.Stop();
								break;
						}

						if((sortFunction != sf_radix_sort) && (sortFunction != sf_qsort))
						{
							const uint64_t elapsedTime = (uint64_t)stopwatch.GetElapsedTime();

							// If this result was faster than a previously fastest result, record this one instead.
							if((c == 0) || (elapsedTime < gResults[i][sizeType][sortFunction].mTime))
								gResults[i][sizeType][sortFunction].mTime = elapsedTime;

							gResults[i][sizeType][sortFunction].mAssignCount = (uint64_t)ElementType::nAssignCount;

							VERIFY(eastl::is_sorted(v.begin(), v.end()));
						}

					} // for each sort function...

				} // for each size type...

			} // for each randomization type...

		} // for each run

		EA::UnitTest::ReportVerbosity(2, "Total time: %.2f s\n", stopwatchGlobal.GetElapsedTimeFloat());

		delete[] pBuffer;

		// Now print the results.
		for(int i = 0; i < kRandomizationTypeCount; i++)
		{
			for(size_t sizeType = 0; sizeType < EAArrayCount(kSizes); sizeType++)
			{
				const eastl_size_t size = kSizes[sizeType];

				for(int sortFunction = 0; sortFunction < sf_count; sortFunction++)
				{
					if((sortFunction != sf_radix_sort) && (sortFunction != sf_qsort)) // qsort doesn't let us count assignments.
						sOutput.append_sprintf("%25s, %14s, Size: %6u, Time: %11I64u ticks, Assignments: %11I64u\n", GetSortFunctionName(sortFunction), GetRandomizationTypeName(i), (unsigned)size, gResults[i][sizeType][sortFunction].mTime, gResults[i][sizeType][sortFunction].mAssignCount);
				}

				sOutput.append("\n");
			}
		}
		EA::UnitTest::ReportVerbosity(2, "%s\n", sOutput.c_str());
	}

	#if !defined(EA_DEBUG)
		EA::UnitTest::SetNormalThreadPriority();
	#endif

	return nErrorCount;
}


void BenchmarkSort()
{
	EASTLTest_Printf("Sort\n");

	EA::UnitTest::RandGenT<uint32_t> rng(12345678); // For debugging sort code we should use 12345678, for normal testing use EA::UnitTest::GetRandSeed().
	EA::StdC::Stopwatch              stopwatch1(EA::StdC::Stopwatch::kUnitsCPUCycles);
	EA::StdC::Stopwatch              stopwatch2(EA::StdC::Stopwatch::kUnitsCPUCycles);

	if(EA::UnitTest::GetVerbosity() >= 3)
		CompareSortPerformance();

	{ // Exercise some declarations
		int nErrorCount = 0;

		ValuePair vp1 = { 0, 0 }, vp2 = { 0, 0 };
		VPCompare c1, c2;

		VERIFY(c1.operator()(vp1, vp2) == c2.operator()(vp1, vp2));
		VERIFY((vp1 < vp2) || (vp1 == vp2) || !(vp1 == vp2));
	}

	{
		eastl::vector<uint32_t> intVector(10000);
		eastl::generate(intVector.begin(), intVector.end(), rng);

		for(int i = 0; i < 2; i++)
		{
			///////////////////////////////
			// Test quick_sort/vector/ValuePair
			///////////////////////////////

			StdVectorVP stdVectorVP(intVector.size());
			EaVectorVP  eaVectorVP (intVector.size());

			for(eastl_size_t j = 0, jEnd = intVector.size(); j < jEnd; j++)
			{
				const ValuePair vp = { intVector[j], intVector[j] };
				stdVectorVP[j] = vp;
				eaVectorVP[j]  = vp;
			}

			TestQuickSortStdVP(stopwatch1, stdVectorVP);
			TestQuickSortEaVP (stopwatch2,  eaVectorVP);

			if(i == 1)
				Benchmark::AddResult("sort/q_sort/vector<ValuePair>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			// Benchmark the sorting of something that is already sorted.
			TestQuickSortStdVP(stopwatch1, stdVectorVP);
			TestQuickSortEaVP (stopwatch2,  eaVectorVP);

			if(i == 1)
				Benchmark::AddResult("sort/q_sort/vector<ValuePair>/sorted", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());



			///////////////////////////////
			// Test quick_sort/vector/Int
			///////////////////////////////

			StdVectorInt stdVectorInt(intVector.size());
			EaVectorInt  eaVectorInt (intVector.size());

			for(eastl_size_t j = 0, jEnd = intVector.size(); j < jEnd; j++)
			{
				stdVectorInt[j] = intVector[j];
				eaVectorInt[j]  = intVector[j];
			}

			TestQuickSortStdInt(stopwatch1, stdVectorInt);
			TestQuickSortEaInt (stopwatch2,  eaVectorInt);

			if(i == 1)
				Benchmark::AddResult("sort/q_sort/vector<uint32>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			// Benchmark the sorting of something that is already sorted.
			TestQuickSortStdInt(stopwatch1, stdVectorInt);
			TestQuickSortEaInt (stopwatch2,  eaVectorInt);

			if(i == 1)
				Benchmark::AddResult("sort/q_sort/vector<uint32>/sorted", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());



			///////////////////////////////
			// Test quick_sort/vector/TestObject
			///////////////////////////////

			StdVectorTO stdVectorTO(intVector.size());
			EaVectorTO  eaVectorTO (intVector.size());

			for(eastl_size_t j = 0, jEnd = intVector.size(); j < jEnd; j++)
			{
				stdVectorTO[j] = TestObject(intVector[j]);
				eaVectorTO[j]  = TestObject(intVector[j]);
			}

			TestQuickSortStdTO(stopwatch1, stdVectorTO);
			TestQuickSortEaTO (stopwatch2,  eaVectorTO);

			if(i == 1)
				Benchmark::AddResult("sort/q_sort/vector<TestObject>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			// Benchmark the sorting of something that is already sorted.
			TestQuickSortStdTO(stopwatch1, stdVectorTO);
			TestQuickSortEaTO (stopwatch2,  eaVectorTO);

			if(i == 1)
				Benchmark::AddResult("sort/q_sort/vector<TestObject>/sorted", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());



			///////////////////////////////
			// Test quick_sort/TestObject[]
			///////////////////////////////

			// Reset the values back to the unsorted state.
			for(eastl_size_t j = 0, jEnd = intVector.size(); j < jEnd; j++)
			{
				stdVectorTO[j] = TestObject(intVector[j]);
				eaVectorTO[j]  = TestObject(intVector[j]);
			}

			TestQuickSortStdTO(stopwatch1, stdVectorTO);
			TestQuickSortEaTO (stopwatch2,  eaVectorTO);

			if(i == 1)
				Benchmark::AddResult("sort/q_sort/TestObject[]", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			// Benchmark the sorting of something that is already sorted.
			TestQuickSortStdTO(stopwatch1, stdVectorTO);
			TestQuickSortEaTO (stopwatch2,  eaVectorTO);

			if(i == 1)
				Benchmark::AddResult("sort/q_sort/TestObject[]/sorted", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
		}
	}
}





