/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EASTL/bonus/sort_extra.h>
#include <EASTL/sort.h>
#include <EASTL/vector.h>
#include <EAStdC/EAStopwatch.h>
#include "EASTLBenchmark.h"
#include "EASTLTest.h"

EA_DISABLE_ALL_VC_WARNINGS()
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <vector>
EA_RESTORE_ALL_VC_WARNINGS()


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
			// return *(const uint64_t*)&vp1 < *(const uint64_t*)&vp2;
			return (vp1.key == vp2.key) ? (vp1.v < vp2.v) : (vp1.key < vp2.key);
		}
	};

	bool operator<(const ValuePair& vp1, const ValuePair& vp2)
	{
		// return *(const uint64_t*)&vp1 < *(const uint64_t*)&vp2;
		return (vp1.key == vp2.key) ? (vp1.v < vp2.v) : (vp1.key < vp2.key);
	}

	bool operator==(const ValuePair& vp1, const ValuePair& vp2)
	{
		// return *(const uint64_t*)&vp1 == *(const uint64_t*)&vp2;
		return (vp1.key == vp2.key) && (vp1.v == vp2.v);
	}
}

// VPCompareC
// Useful for testing the the C qsort function.
int VPCompareC(const void* elem1, const void* elem2)
{
	return (int)(*(const uint64_t*)elem1 - *(const uint64_t*)elem2);
}


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
		sf_qsort,             // C qsort
		sf_shell_sort,        // eastl::shell_sort.
		sf_heap_sort,         // eastl::heap_sort
		sf_merge_sort,        // eastl::merge_sort
		sf_merge_sort_buffer, // eastl::merge_sort_buffer
		sf_comb_sort,         // eastl::comb_sort
		sf_bubble_sort,       // eastl::bubble_sort
		sf_selection_sort,    // eastl::selection_sort
		sf_shaker_sort,       // eastl::shaker_sort
		sf_quick_sort,        // eastl::quick_sort
		sf_tim_sort,          // eastl::tim_sort
		sf_insertion_sort,    // eastl::insertion_sort
		sf_std_sort,          // std::sort
		sf_std_stable_sort,   // std::stable_sort
		sf_radix_sort,        // eastl::radix_sort (unconventional sort)
		sf_count              //
	};

	const char* GetSortFunctionName(int sortFunctionType)
	{
		switch (sortFunctionType)
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

			case sf_std_stable_sort:
				return "std::stable_sort";

			default:
				return "unknown";
		}
	}


	enum RandomizationType
	{
		kRandom,                    // Completely random data.
		kRandomSorted,  // Random values already sorted.
		kOrdered,                   // Already sorted.
		kMostlyOrdered,             // Partly sorted already.
		kRandomizationTypeCount
	};

	const char* GetRandomizationTypeName(int randomizationType)
	{
		switch (randomizationType)
		{
			case kRandom:
				return "random";

			case kRandomSorted:
				return "random sorted";

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

		switch (type)
		{
			default:
			case kRandomizationTypeCount: // We specify this only to avoid a compiler warning about not testing for it.
			case kRandom:
			{
				eastl::generate(v.begin(), v.end(), rng);
				break;
			}

			case kRandomSorted:
			{
				// This randomization type differs from kOrdered because the set of values is random (but sorted), in the kOrdered
				// case the set of values is contiguous (i.e. 0, 1, ..., n) which can have different performance characteristics.
				// For example, radix_sort performs poorly for kOrdered.
				eastl::generate(v.begin(), v.end(), rng);
				eastl::sort(v.begin(), v.end());
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


	char gSlowAssignBuffer1[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0 /* ... */};
	char gSlowAssignBuffer2[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0 /* ... */};


	// SlowAssign
	// Implements an object which has slow assign performance.
	template <typename T>
	struct SlowAssign
	{
		typedef T key_type;
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

	template<> int SlowAssign<uint32_t>::nAssignCount = 0;

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

			return (a < b) && // It happens that gSlowAssignBuffer1 is always zeroed.
			       (gSlowAssignBuffer1[0] == 0) && (gSlowAssignBuffer1[1] == 0) && (gSlowAssignBuffer1[1] == 0) &&
			       (gSlowAssignBuffer1[2] == 0) && (gSlowAssignBuffer1[4] == 0) && (gSlowAssignBuffer1[5] == 0);
		}

		static void Reset() { nCompareCount = 0; }
	};

	template <>
	int SlowCompare<int32_t>::nCompareCount = 0;


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
		if (*(const T*)a < *(const T*)b)
			return -1;
		if (*(const T*)a > *(const T*)b)
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
			if (*(const int32_t*)a < *(const int32_t*)b)
				return -1;
			if (*(const int32_t*)a > *(const int32_t*)b)
				return +1;
		}

		return 0;
	}

	template <typename slow_assign_type>
	struct slow_assign_extract_radix_key
	{
		typedef typename slow_assign_type::key_type radix_type;

		const radix_type operator()(const slow_assign_type& obj) const
		{
			return obj.x;
		}
	};
	
	template <typename integer_type>
	struct identity_extract_radix_key
	{
		typedef integer_type radix_type;

		const radix_type operator()(const integer_type& x) const
		{
			return x;
		}
	};
} // namespace


struct BenchmarkResult
{
	uint64_t mTime;
	uint64_t mCompareCount;
	uint64_t mAssignCount;

	BenchmarkResult() : mTime(0), mCompareCount(0), mAssignCount(0) {}
};


int CompareSortPerformance()
{
	// Sizes of arrays to be sorted.
	const eastl_size_t kSizes[] = { 10, 100, 1000, 10000 };
	const eastl_size_t kSizesCount = EAArrayCount(kSizes);
	static BenchmarkResult sResults[kRandomizationTypeCount][kSizesCount][sf_count];
	int nErrorCount = 0;

	EA::UnitTest::ReportVerbosity(2, "Sort comparison\n");
	EA::UnitTest::ReportVerbosity(2, "Random seed = %u\n", (unsigned)EA::UnitTest::GetRandSeed());

	EA::UnitTest::RandGenT<int32_t> rng(EA::UnitTest::GetRandSeed());
	EA::StdC::Stopwatch             stopwatch(EA::StdC::Stopwatch::kUnitsCPUCycles);
	EA::StdC::Stopwatch             stopwatchGlobal(EA::StdC::Stopwatch::kUnitsSeconds);
	const eastl_size_t kArraySizeMax = *eastl::max_element(eastl::begin(kSizes), eastl::end(kSizes));
	const int                       kRunCount = 4;

	#if !defined(EA_DEBUG)
		EA::UnitTest::SetHighThreadPriority();
	#endif

	eastl::vector<SortFunctionType> allSortFunctions;
	for (int i = 0; i < sf_count; i++)
	{
		allSortFunctions.push_back(SortFunctionType(i));
	}

	{
		auto& sortFunctions = allSortFunctions;

		// Regular speed test.
		// In this case we test the sorting of integral values.
		// This is probably the most common type of comparison.
		EA::UnitTest::ReportVerbosity(2, "Sort comparison: Regular speed test\n");

		typedef uint32_t                      ElementType;
		typedef eastl::less<ElementType>      CompareFunction;

		eastl::string sOutput;
		sOutput.set_capacity(100000);
		ElementType* pBuffer = new ElementType[kArraySizeMax];

		memset(sResults, 0, sizeof(sResults));

		stopwatchGlobal.Restart();

		for (int c = 0; c < kRunCount; c++)
		{
			for (int i = 0; i < kRandomizationTypeCount; i++)
			{
				for (size_t sizeType = 0; sizeType < EAArrayCount(kSizes); sizeType++)
				{
					const eastl_size_t size = kSizes[sizeType];

					for (SortFunctionType sortFunction : sortFunctions)
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
								stopwatch.Restart();
								eastl::radix_sort<ElementType*, identity_extract_radix_key<ElementType>>(v.begin(), v.end(), pBuffer);
								stopwatch.Stop();
								break;

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

							case sf_std_stable_sort:
								stopwatch.Restart();
								std::stable_sort(v.data(), v.data() + v.size(), std::less<ElementType>());
								stopwatch.Stop();
								break;

							case sf_count:
							default:
								// unsupported
								break;
						}

						const uint64_t elapsedTime = (uint64_t)stopwatch.GetElapsedTime();

						// If this result was faster than a previously fastest result, record this one instead.
						if ((c == 0) || (elapsedTime < sResults[i][sizeType][sortFunction].mTime))
							sResults[i][sizeType][sortFunction].mTime = elapsedTime;

						VERIFY(eastl::is_sorted(v.begin(), v.end()));

					} // for each sort function...

				} // for each size type...

			} // for each randomization type...

		} // for each run

		EA::UnitTest::ReportVerbosity(2, "Total time: %.2f s\n", stopwatchGlobal.GetElapsedTimeFloat());

		delete[] pBuffer;

		// Now print the results.
		for (int i = 0; i < kRandomizationTypeCount; i++)
		{
			for (size_t sizeType = 0; sizeType < EAArrayCount(kSizes); sizeType++)
			{
				const eastl_size_t size = kSizes[sizeType];

				for (SortFunctionType sortFunction : sortFunctions)
				{
					sOutput.append_sprintf("%25s, %14s, Size: %8u, Time: %14" PRIu64 " ticks %0.2f ticks/elem\n",
					                       GetSortFunctionName(sortFunction), GetRandomizationTypeName(i),
					                       (unsigned)size, sResults[i][sizeType][sortFunction].mTime,
					                       float(sResults[i][sizeType][sortFunction].mTime)/float(size));
				}
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
		auto sortFunctions = allSortFunctions;
		// We can't test this radix_sort because what we need isn't exposed.
		sortFunctions.erase(eastl::remove(sortFunctions.begin(), sortFunctions.end(), sf_radix_sort), sortFunctions.end());
		EA::UnitTest::ReportVerbosity(2, "Sort comparison: Slow compare speed test\n");

		typedef int32_t ElementType;
		typedef SlowCompare<ElementType> CompareFunction;

		eastl::string sOutput;
		sOutput.set_capacity(100000);
		ElementType* pBuffer = new ElementType[kArraySizeMax];

		memset(sResults, 0, sizeof(sResults));

		stopwatchGlobal.Restart();

		for (int c = 0; c < kRunCount; c++)
		{
			for (int i = 0; i < kRandomizationTypeCount; i++)
			{
				for (size_t sizeType = 0; sizeType < EAArrayCount(kSizes); sizeType++)
				{
					const eastl_size_t size = kSizes[sizeType];

					for (SortFunctionType sortFunction : sortFunctions)
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

							case sf_std_stable_sort:
								stopwatch.Restart();
								std::stable_sort(v.begin(), v.end(), CompareFunction());
								stopwatch.Stop();
								break;

							case sf_radix_sort:
							case sf_count:
							default:
								// unsupported
								break;
						}

						const uint64_t elapsedTime = (uint64_t)stopwatch.GetElapsedTime();

						// If this result was faster than a previously fastest result, record this one instead.
						if ((c == 0) || (elapsedTime < sResults[i][sizeType][sortFunction].mTime))
						{
							sResults[i][sizeType][sortFunction].mTime = elapsedTime;
							sResults[i][sizeType][sortFunction].mCompareCount = (uint64_t)CompareFunction::nCompareCount;
						}

						VERIFY(eastl::is_sorted(v.begin(), v.end()));
					} // for each sort function...

				} // for each size type...

			} // for each randomization type...

		} // for each run

		EA::UnitTest::ReportVerbosity(2, "Total time: %.2f s\n", stopwatchGlobal.GetElapsedTimeFloat());

		delete[] pBuffer;

		// Now print the results.
		for (int i = 0; i < kRandomizationTypeCount; i++)
		{
			for (size_t sizeType = 0; sizeType < EAArrayCount(kSizes); sizeType++)
			{
				const eastl_size_t size = kSizes[sizeType];

				for (SortFunctionType sortFunction : sortFunctions)
				{
					sOutput.append_sprintf("%25s, %14s, Size: %6u, Time: %11" PRIu64 " ticks, Compares: %11" PRIu64 "\n",
					                       GetSortFunctionName(sortFunction), GetRandomizationTypeName(i),
					                       (unsigned)size, sResults[i][sizeType][sortFunction].mTime,
					                       sResults[i][sizeType][sortFunction].mCompareCount);
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
		auto sortFunctions = allSortFunctions;
		// Can't implement this for qsort because the C standard library doesn't expose it.
		// We could implement it by copying and modifying the source code.
		sortFunctions.erase(eastl::remove(sortFunctions.begin(), sortFunctions.end(), sf_qsort), sortFunctions.end());

		EA::UnitTest::ReportVerbosity(2, "Sort comparison: Slow assignment speed test\n");

		typedef SlowAssign<uint32_t> ElementType;
		typedef eastl::less<ElementType> CompareFunction;

		eastl::string sOutput;
		sOutput.set_capacity(100000);
		ElementType* pBuffer = new ElementType[kArraySizeMax];

		memset(sResults, 0, sizeof(sResults));

		stopwatchGlobal.Restart();

		for (int c = 0; c < kRunCount; c++)
		{
			for (int i = 0; i < kRandomizationTypeCount; i++)
			{
				for (size_t sizeType = 0; sizeType < EAArrayCount(kSizes); sizeType++)
				{
					const eastl_size_t size = kSizes[sizeType];

					for (SortFunctionType sortFunction : sortFunctions)
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
								stopwatch.Restart();
								eastl::radix_sort<ElementType*, slow_assign_extract_radix_key<ElementType>>(v.begin(), v.end(), pBuffer);
								stopwatch.Stop();
								break;

							case sf_std_sort:
								stopwatch.Restart();
								std::sort(v.begin(), v.end(), std::less<ElementType>());
								stopwatch.Stop();
								break;

							case sf_std_stable_sort:
								stopwatch.Restart();
								std::stable_sort(v.begin(), v.end(), std::less<ElementType>());
								stopwatch.Stop();
								break;

							case sf_qsort:
							case sf_count:
							default:
								// unsupported
								break;
						}

						const uint64_t elapsedTime = (uint64_t)stopwatch.GetElapsedTime();

						// If this result was faster than a previously fastest result, record this one instead.
						if ((c == 0) || (elapsedTime < sResults[i][sizeType][sortFunction].mTime))
						{
							sResults[i][sizeType][sortFunction].mTime = elapsedTime;
							sResults[i][sizeType][sortFunction].mAssignCount = (uint64_t)ElementType::nAssignCount;
						}

						VERIFY(eastl::is_sorted(v.begin(), v.end()));

					} // for each sort function...

				} // for each size type...

			} // for each randomization type...

		} // for each run

		EA::UnitTest::ReportVerbosity(2, "Total time: %.2f s\n", stopwatchGlobal.GetElapsedTimeFloat());

		delete[] pBuffer;

		// Now print the results.
		for (int i = 0; i < kRandomizationTypeCount; i++)
		{
			for (size_t sizeType = 0; sizeType < EAArrayCount(kSizes); sizeType++)
			{
				const eastl_size_t size = kSizes[sizeType];

				for (SortFunctionType sortFunction : sortFunctions)
				{
					sOutput.append_sprintf("%25s, %14s, Size: %6u, Time: %11" PRIu64 " ticks, Assignments: %11" PRIu64 "\n",
					                       GetSortFunctionName(sortFunction), GetRandomizationTypeName(i),
					                       (unsigned)size, sResults[i][sizeType][sortFunction].mTime,
					                       sResults[i][sizeType][sortFunction].mAssignCount);
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

typedef eastl::function<void(eastl::string &output, const char* sortFunction, const char* randomizationType, size_t size, size_t numSubArrays, const BenchmarkResult &result)> OutputResultCallback;
typedef eastl::function<void(BenchmarkResult &result)> PostExecuteCallback;
typedef eastl::function<void()> PreExecuteCallback;


template<class ElementType, class CompareFunction>
static int CompareSmallInputSortPerformanceHelper(eastl::vector<eastl_size_t> &arraySizes, eastl::vector<SortFunctionType> &sortFunctions, const PreExecuteCallback &preExecuteCallback, const PostExecuteCallback &postExecuteCallback, const OutputResultCallback &outputResultCallback)
{
	int nErrorCount = 0;

	EA::UnitTest::RandGenT<int32_t> rng(EA::UnitTest::GetRandSeed());
	EA::StdC::Stopwatch stopwatch(EA::StdC::Stopwatch::kUnitsCPUCycles);
	EA::StdC::Stopwatch stopwatchGlobal(EA::StdC::Stopwatch::kUnitsSeconds);
	const eastl_size_t kArraySizeMax = *eastl::max_element(eastl::begin(arraySizes), eastl::end(arraySizes));
	const int kRunCount = 4;
	const int numSubArrays = 128;

	eastl::string sOutput;
	sOutput.set_capacity(100000);
	ElementType* pBuffer = new ElementType[kArraySizeMax];

	stopwatchGlobal.Restart();

	for (int i = 0; i < kRandomizationTypeCount; i++)
	{
		for (size_t size : arraySizes)
		{
			for (SortFunctionType sortFunction : sortFunctions)
			{
				BenchmarkResult bestResult{};

				for (int c = 0; c < kRunCount; c++)
				{
					eastl::vector<ElementType> v(size * numSubArrays);

					rng.SetSeed(EA::UnitTest::GetRandSeed());
					Randomize(v, rng, (RandomizationType)i);
					preExecuteCallback();

					switch (sortFunction)
					{
						case sf_quick_sort:
							stopwatch.Restart();
							for (auto begin = v.begin(); begin != v.end(); begin += size)
							{
								eastl::quick_sort(begin, begin + size, CompareFunction());
							}
							stopwatch.Stop();
							break;

						case sf_tim_sort:
							stopwatch.Restart();
							for (auto begin = v.begin(); begin != v.end(); begin += size)
							{
								eastl::tim_sort_buffer(begin, begin + size, pBuffer, CompareFunction());
							}
							stopwatch.Stop();
							break;

						case sf_insertion_sort:
							stopwatch.Restart();
							for (auto begin = v.begin(); begin != v.end(); begin += size)
							{
								eastl::insertion_sort(begin, begin + size, CompareFunction());
							}
							stopwatch.Stop();
							break;

						case sf_shell_sort:
							stopwatch.Restart();
							for (auto begin = v.begin(); begin != v.end(); begin += size)
							{
								eastl::shell_sort(begin, begin + size, CompareFunction());
							}
							stopwatch.Stop();
							break;

						case sf_heap_sort:
							stopwatch.Restart();
							for (auto begin = v.begin(); begin != v.end(); begin += size)
							{
								eastl::heap_sort(begin, begin + size, CompareFunction());
							}
							stopwatch.Stop();
							break;

						case sf_merge_sort:
							stopwatch.Restart();
							for (auto begin = v.begin(); begin != v.end(); begin += size)
							{
								eastl::merge_sort(begin, begin + size, *get_default_allocator((EASTLAllocatorType*)NULL), CompareFunction());
							}
							stopwatch.Stop();
							break;

						case sf_merge_sort_buffer:
							stopwatch.Restart();
							for (auto begin = v.begin(); begin != v.end(); begin += size)
							{
								eastl::merge_sort_buffer(begin, begin + size, pBuffer, CompareFunction());
							}
							stopwatch.Stop();
							break;

						case sf_comb_sort:
							stopwatch.Restart();
							for (auto begin = v.begin(); begin != v.end(); begin += size)
							{
								eastl::comb_sort(begin, begin + size, CompareFunction());
							}
							stopwatch.Stop();
							break;

						case sf_bubble_sort:
							stopwatch.Restart();
							for (auto begin = v.begin(); begin != v.end(); begin += size)
							{
								eastl::bubble_sort(begin, begin + size, CompareFunction());
							}
							stopwatch.Stop();
							break;

						case sf_selection_sort:
							stopwatch.Restart();
							for (auto begin = v.begin(); begin != v.end(); begin += size)
							{
								eastl::selection_sort(begin, begin + size, CompareFunction());
							}
							stopwatch.Stop();
							break;

						case sf_shaker_sort:
							stopwatch.Restart();
							for (auto begin = v.begin(); begin != v.end(); begin += size)
							{
								eastl::shaker_sort(begin, begin + size, CompareFunction());
							}
							stopwatch.Stop();
							break;

						case sf_std_sort:
							stopwatch.Restart();
							for (auto begin = v.begin(); begin != v.end(); begin += size)
							{
								std::sort(begin, begin + size, CompareFunction());
							}
							stopwatch.Stop();
							break;

						case sf_std_stable_sort:
							stopwatch.Restart();
							for (auto begin = v.begin(); begin != v.end(); begin += size)
							{
								std::stable_sort(begin, begin + size, CompareFunction());
							}
							stopwatch.Stop();
							break;

						case sf_qsort:
						case sf_radix_sort:
						case sf_count:
						default:
							EATEST_VERIFY_F(false, "Missing case statement for sort function %s.", GetSortFunctionName(sortFunction));
							break;
					}

					BenchmarkResult result {};
					result.mTime = (uint64_t)stopwatch.GetElapsedTime();
					postExecuteCallback(result);

					// If this result was faster than a previously fastest result, record this one instead.
					if ((c == 0) || (result.mTime < bestResult.mTime))
						bestResult = result;

					for (auto begin = v.begin(); begin != v.end(); begin += size)
					{
						VERIFY(eastl::is_sorted(begin, begin + size));
					}
				} // for each run

				outputResultCallback(sOutput, GetSortFunctionName(sortFunction), GetRandomizationTypeName(i), size, numSubArrays, bestResult);

			} // for each sort function...
			sOutput.append("\n");

		} // for each size type...

	} // for each randomization type...

	EA::UnitTest::ReportVerbosity(2, "Total time: %.2f s\n", stopwatchGlobal.GetElapsedTimeFloat());
	EA::UnitTest::ReportVerbosity(2, "%s\n", sOutput.c_str());

	delete[] pBuffer;
	return nErrorCount;
}

static int CompareSmallInputSortPerformance()
{
	int nErrorCount = 0;
	eastl::vector<eastl_size_t> arraySizes{1, 2, 3, 4, 7, 8, 15, 16, 31, 32, 64, 128, 256};
	// Test quick sort and merge sort to provide a "base line" for performance.  The other sort algorithms are mostly
	// O(n^2) and they are benchmarked to determine what sorts are ideal for sorting small arrays or sub-arrays.  (i.e.
	// this is useful to determine good algorithms to choose as a base case for some of the recursive sorts).
	eastl::vector<SortFunctionType> sortFunctions{sf_quick_sort,     sf_merge_sort_buffer,     sf_bubble_sort, sf_comb_sort,
	                                              sf_insertion_sort, sf_selection_sort, sf_shell_sort,  sf_shaker_sort};

	EA::UnitTest::ReportVerbosity(2, "Small Sub-array Sort comparison: Regular speed test\n");
	nErrorCount += CompareSmallInputSortPerformanceHelper<uint32_t, eastl::less<uint32_t>>(
	    arraySizes, sortFunctions, PreExecuteCallback([]() {}), PostExecuteCallback([](BenchmarkResult&) {}),
	    OutputResultCallback([](eastl::string& output, const char* sortFunction, const char* randomizationType,
	                            size_t size, size_t numSubArrays, const BenchmarkResult& result) {
		    output.append_sprintf("%25s, %14s, Size: %8u, Time: %0.1f ticks %0.2f ticks/elem\n", sortFunction,
		                          randomizationType, (unsigned)size, float(result.mTime) / float(numSubArrays),
		                          float(result.mTime) / float(size * numSubArrays));
	    }));

	EA::UnitTest::ReportVerbosity(2, "Small Sub-array Sort comparison: Slow compare speed test\n");
	nErrorCount += CompareSmallInputSortPerformanceHelper<int32_t, SlowCompare<int32_t>>(
	    arraySizes, sortFunctions, PreExecuteCallback([]() { SlowCompare<int32_t>::Reset(); }),
	    PostExecuteCallback(
	        [](BenchmarkResult& result) { result.mCompareCount = (uint64_t)SlowCompare<int32_t>::nCompareCount; }),
	    OutputResultCallback([](eastl::string& output, const char* sortFunction, const char* randomizationType,
	                            size_t size, size_t numSubArrays, const BenchmarkResult& result) {
		    output.append_sprintf("%25s, %14s, Size: %6u, Time: %0.2f ticks, Compares: %0.2f\n", sortFunction,
		                          randomizationType, (unsigned)size, float(result.mTime) / float(numSubArrays),
		                          float(result.mCompareCount) / float(numSubArrays));
	    }));

	EA::UnitTest::ReportVerbosity(2, "Small Sub-array Sort comparison: Slow assignment speed test\n");
	nErrorCount += CompareSmallInputSortPerformanceHelper<SlowAssign<uint32_t>, eastl::less<SlowAssign<uint32_t>>>(
	    arraySizes, sortFunctions, PreExecuteCallback([]() { SlowAssign<uint32_t>::Reset(); }),
	    PostExecuteCallback([](BenchmarkResult& result) {
		    result.mCompareCount = (uint64_t)SlowCompare<int32_t>::nCompareCount;
		    result.mAssignCount = (uint64_t)SlowAssign<uint32_t>::nAssignCount;
	    }),
	    OutputResultCallback([](eastl::string& output, const char* sortFunction, const char* randomizationType,
	                            size_t size, size_t numSubArrays, const BenchmarkResult& result) {
		    output.append_sprintf("%25s, %14s, Size: %6u, Time: %0.2f ticks, Assignments: %0.2f\n", sortFunction,
		                          randomizationType, (unsigned)size, float(result.mTime) / float(numSubArrays),
		                          float(result.mAssignCount) / float(numSubArrays));
	    }));

	return nErrorCount;
}


void BenchmarkSort()
{
	EASTLTest_Printf("Sort\n");

	EA::UnitTest::RandGenT<uint32_t> rng(12345678); // For debugging sort code we should use 12345678, for normal testing use EA::UnitTest::GetRandSeed().
	EA::StdC::Stopwatch              stopwatch1(EA::StdC::Stopwatch::kUnitsCPUCycles);
	EA::StdC::Stopwatch              stopwatch2(EA::StdC::Stopwatch::kUnitsCPUCycles);

	if (EA::UnitTest::GetVerbosity() >= 3)
	{
		CompareSortPerformance();
		CompareSmallInputSortPerformance();
	}

	{ // Exercise some declarations
		int nErrorCount = 0;

		ValuePair vp1 = {0, 0}, vp2 = {0, 0};
		VPCompare c1, c2;

		VERIFY(c1.operator()(vp1, vp2) == c2.operator()(vp1, vp2));
		VERIFY((vp1 < vp2) || (vp1 == vp2) || !(vp1 == vp2));
	}

	{
		eastl::vector<uint32_t> intVector(10000);
		eastl::generate(intVector.begin(), intVector.end(), rng);

		for (int i = 0; i < 2; i++)
		{
			///////////////////////////////
			// Test quick_sort/vector/ValuePair
			///////////////////////////////

			StdVectorVP stdVectorVP(intVector.size());
			EaVectorVP eaVectorVP(intVector.size());

			for (eastl_size_t j = 0, jEnd = intVector.size(); j < jEnd; j++)
			{
				const ValuePair vp = {intVector[j], intVector[j]};
				stdVectorVP[j] = vp;
				eaVectorVP[j] = vp;
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
			EaVectorTO eaVectorTO(intVector.size());

			for (eastl_size_t j = 0, jEnd = intVector.size(); j < jEnd; j++)
			{
				stdVectorTO[j] = TestObject(intVector[j]);
				eaVectorTO[j] = TestObject(intVector[j]);
			}

			TestQuickSortStdTO(stopwatch1, stdVectorTO);
			TestQuickSortEaTO(stopwatch2, eaVectorTO);

			if (i == 1)
				Benchmark::AddResult("sort/q_sort/vector<TestObject>", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());

			// Benchmark the sorting of something that is already sorted.
			TestQuickSortStdTO(stopwatch1, stdVectorTO);
			TestQuickSortEaTO(stopwatch2, eaVectorTO);

			if (i == 1)
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





