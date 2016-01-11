/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>

// Some versions of GCC generate an array bounds warning in opt builds which 
// doesn't say what line below it comes from and doesn't appear to be a valid 
// warning. In researching this on the Internet it appears that this is a 
// known problem with GCC.
#if defined(EA_DISABLE_GCC_WARNING)
	EA_DISABLE_GCC_WARNING(-Warray-bounds)
#endif

#include "EASTLTest.h"
#include <EASTL/sort.h>
#include <EASTL/bonus/sort_extra.h>
#include <EASTL/vector.h>
#include <EASTL/deque.h>
#include <EASTL/algorithm.h>
#include <EASTL/allocator.h>
#include <EASTL/random.h>
#include <EABase/eahave.h>
#include <cmath>


namespace eastl
{
	namespace Internal
	{
		typedef eastl::vector<int>      IntArray;
		typedef eastl::vector<IntArray> IntArrayArray;


		// IntArrayCompare
		// Used to compare IntArray objects.
		struct IntArrayCompare
		{
			bool operator()(const IntArray& a, const IntArray& b)
				{ return a.front() < b.front(); }
		};


		// SafeFloatCompare
		//
		// Float comparison has a problem for the case that either of the floats are a NaN.
		// If you use a NaN in a sort function that uses default floating point comparison then
		// you will get undefined behavior, as all NaNs compare false. This compare function 
		// class sorts floats such that all negative NaNs sort lower than all integers, and all
		// positive NaNs sort higher than all integers.
		//
		// Example usage:
		//     eastl::sort(floatArray.begin(), floatArray.end(), SafeFloatCompare());
		//
		struct SafeFloatCompare
		{
			union FloatInt32{ float f; int32_t i; };

			bool operator()(float a, float b) const
			{
				#if defined(EA_HAVE_ISNAN)
					bool aNan = (EA_HAVE_ISNAN(a) != 0);
					bool bNan = (EA_HAVE_ISNAN(b) != 0);
				#else
					bool aNan = (a != a); // This works as long as the compiler doesn't do any tricks to optimize it away.
					bool bNan = (b != b);
				#endif

				if(!aNan && !bNan)
					return (a < b);

				FloatInt32 fia = { a };
				FloatInt32 fib = { b };

				if(aNan)
				{
					if(bNan)
						return (fia.i < fib.i); // Both are NaNs, so do a binary compare.
					else
						return (fia.i < 0);  // All negative NaNs are assumed to be less than all non-NaNs.
				}
				else
					return (0 < fib.i); // All negative NaNs are assumed to be less than all non-NaNs.
			}
		};



		// StatefulCompare
		// Used to verify that sort<int, StatefulCompare&>() respects the 
		// fact that StatefulCompare is passed by reference instead of by value.
		// All existing commercial STL implementations fail to do what the user
		// wants and instead pass around the compare object by value, even if
		// the user specifically asks to use it by reference. EASTL doesn't 
		// have this problem. 
		struct StatefulCompare
		{
			static int nCtorCount;
			static int nDtorCount;
			static int nCopyCount;

			StatefulCompare()
				{ nCtorCount++; }

			StatefulCompare(StatefulCompare&)
				{ nCopyCount++; }

		   ~StatefulCompare()
				{ nDtorCount++; }

			StatefulCompare& operator=(const StatefulCompare&)
				{ nCopyCount++; return *this; }

			bool operator()(int a, int b)
				{ return a < b; }

			static void Reset()
				{ nCtorCount = 0; nDtorCount = 0; nCopyCount = 0; }
		};

		int StatefulCompare::nCtorCount = 0;
		int StatefulCompare::nDtorCount = 0;
		int StatefulCompare::nCopyCount = 0;


		// TestObjectPtrCompare
		// Used to compare sorted objects by pointer instead of value.
		struct TestObjectPtrCompare
		{
			bool operator()(TestObject* a, TestObject* b)
			   { return a->mX < b->mX; }
		};


		// TestObjectIndexCompare
		// Used to compare sorted objects by array index instead of value.
		struct TestObjectIndexCompare
		{
			vector<TestObject>* mpArray;

			TestObjectIndexCompare(vector<TestObject>* pArray) : mpArray(pArray) { }
			TestObjectIndexCompare(const TestObjectIndexCompare& x)   : mpArray(x.mpArray){ }
			TestObjectIndexCompare& operator=(const TestObjectIndexCompare& x) { mpArray = x.mpArray; return *this; }

			bool operator()(eastl_size_t a, eastl_size_t b)
			   { return (*mpArray)[a] < (*mpArray)[b]; }
		};


		// Radix sort elements
		struct RadixSortElement8
		{
			typedef uint8_t radix_type;
			uint8_t  mKey;
			uint16_t mData;
		};

		struct RadixSortElement16
		{
			typedef uint16_t radix_type;
			uint16_t mKey;
			uint16_t mData;
		};

		struct RadixSortElement32
		{
			typedef uint32_t radix_type;
			uint32_t mKey;
			uint16_t mData;
		};

	} // namespace Internal

} // namespace eastl


int TestSort()
{
	EASTLTest_Printf("TestSort\n");

	using namespace eastl;
	using namespace Internal;

	int nErrorCount = 0;

	EASTLTest_Rand rng(EA::UnitTest::GetRandSeed());

	{
		// is_sorted
		int array[] = { 0, 1, 2, 2, 2, 3, 4, 5, 6, 7, 8, 9, 9, 8, 7, 6, 5, 4, 3, 2, 2, 2, 1, 0 };
	
		EATEST_VERIFY( is_sorted(array +  0, array +  0));
		EATEST_VERIFY( is_sorted(array +  2, array +  4));
		EATEST_VERIFY( is_sorted(array +  0, array + 10));
		EATEST_VERIFY(!is_sorted(array +  0, array + 14));
		EATEST_VERIFY( is_sorted(array + 11, array + 23, eastl::greater<int>()));
	}

	{
		// is_sorted_until
		int sorted[]    = { 0, 1, 2, 3, 4,  5, 6, 7, 8, 9 };
		int notsorted[] = { 0, 1, 2, 3, 4, 42, 6, 7, 8, 9 };
			
		EATEST_VERIFY( is_sorted_until(sorted + EAArrayCount(sorted),  sorted + EAArrayCount(sorted)) == sorted + EAArrayCount(sorted) );
		EATEST_VERIFY( is_sorted_until(sorted                       ,  sorted + EAArrayCount(sorted)) == sorted + EAArrayCount(sorted) );

		EATEST_VERIFY( is_sorted_until(sorted +  0, sorted +  0) == sorted     );
		EATEST_VERIFY( is_sorted_until(sorted +  2, sorted +  8) == sorted + 8 );

		EATEST_VERIFY( is_sorted_until(notsorted +  2, notsorted +  8) == notsorted + 6 );
		
		// is_sorted_until (with compare function)
		EATEST_VERIFY( is_sorted_until(sorted + EAArrayCount(sorted),  sorted + EAArrayCount(sorted), eastl::less<int>()) == sorted + EAArrayCount(sorted) );
		EATEST_VERIFY( is_sorted_until(notsorted +  2, notsorted +  8, eastl::less<int>()) == notsorted + 6 );
	}

	// Sort arrays of size 0 - N. Sort M random permutations of each.
	{
		vector<int64_t> intArray, intArraySaved;

		for(int i = 0; i < (150 + (gEASTL_TestLevel * 200)); i += (i < 5) ? 1 : 37) // array sizes of 0 to 300 - 2100, depending on test level.
		{
			// intArraySaved.clear(); // Do we want to do this?

			for(int n = 0; n < i; n++)
			{
				intArraySaved.push_back(n);

				if(rng.RandLimit(10) == 0)
				{
					intArraySaved.push_back(n);

					if(rng.RandLimit(5) == 0)
						intArraySaved.push_back(n);
				}
			}

			for(int j = 0; j < 300 + (gEASTL_TestLevel * 50); j++)
			{
				eastl::random_shuffle(intArraySaved.begin(), intArraySaved.end(), rng);

				intArray = intArraySaved;
				bubble_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));

				intArray = intArraySaved;
				shaker_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));

				intArray = intArraySaved;
				insertion_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));

				intArray = intArraySaved;
				selection_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));

				intArray = intArraySaved;
				shell_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));

				intArray = intArraySaved;
				comb_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));

				intArray = intArraySaved;
				heap_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));

				intArray = intArraySaved;
				merge_sort(intArray.begin(), intArray.end(), *get_default_allocator((EASTLAllocatorType*)NULL));
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));

				intArray = intArraySaved;
				vector<int64_t> buffer(intArray.size());
				merge_sort_buffer(intArray.begin(), intArray.end(), &buffer[0]);
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));

				intArray = intArraySaved;
				quick_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));
			}
		}
	}


	// TestObject sorting
	TestObject::Reset();
	{
		vector<TestObject> toArray, toArraySaved;

		for(int i = 0; i < (150 + (gEASTL_TestLevel * 200)); i += (i < 5) ? 1 : 37) // array sizes of 0 to 300 - 2100, depending on test level.
		{
			for(int n = 0; n < i; n++)
			{
				toArraySaved.push_back(TestObject(n));

				if(rng.RandLimit(10) == 0)
				{
					toArraySaved.push_back(TestObject(n));

					if(rng.RandLimit(5) == 0)
						toArraySaved.push_back(TestObject(n));
				}
			}

			for(int j = 0; j < 300 + (gEASTL_TestLevel * 50); j++)
			{
				eastl::random_shuffle(toArraySaved.begin(), toArraySaved.end(), rng);

				toArray = toArraySaved;
				bubble_sort(toArray.begin(), toArray.end());
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end()));

				toArray = toArraySaved;
				shaker_sort(toArray.begin(), toArray.end());
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end()));

				toArray = toArraySaved;
				insertion_sort(toArray.begin(), toArray.end());
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end()));

				toArray = toArraySaved;
				selection_sort(toArray.begin(), toArray.end());
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end()));

				toArray = toArraySaved;
				shell_sort(toArray.begin(), toArray.end());
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end()));

				toArray = toArraySaved;
				comb_sort(toArray.begin(), toArray.end());
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end()));

				toArray = toArraySaved;
				heap_sort(toArray.begin(), toArray.end());
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end()));

				// Not ready yet:
				toArray = toArraySaved;
				merge_sort(toArray.begin(), toArray.end(), *get_default_allocator((EASTLAllocatorType*)NULL));
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end()));

				toArray = toArraySaved;
				quick_sort(toArray.begin(), toArray.end());
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end()));
			}
		}
	}


	{
		// OutputIterator merge(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result)
		// This is tested by merge_sort.
	}


	{
		// void partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last)
		// This is tested by quick_sort.
	}


	{
		// void nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last)
		// void nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last, Compare compare)
		const int intArrayInit[16] = { 4, 2, 8, 6, 9, 1, 1, 4, 0, 5, 5, 7, 8, 9, 3, 3 };
		int intArraySorted[16]; // Same as intArrayInit but sorted
		int intArray[16];
		size_t i, j;

		// We test many combinations of nth_element on the int array.
		for(i = 1; i < 16; i++)
		{
			for(j = 0; j < i; j++)
			{
				eastl::copy(intArrayInit, intArrayInit + i, intArraySorted);
				eastl::sort(intArraySorted, intArraySorted + i);

				eastl::copy(intArrayInit, intArrayInit + i, intArray);
				nth_element(intArray, intArray + j, intArray + i);
				EATEST_VERIFY(intArray[j] == intArraySorted[j]);
			}
		}

		for(i = 1; i < 16; i++)
		{
			for(j = 0; j < i; j++)
			{
				eastl::copy(intArrayInit, intArrayInit + i, intArraySorted);
				eastl::sort(intArraySorted, intArraySorted + i);

				eastl::copy(intArrayInit, intArrayInit + 16, intArray);
				nth_element(intArray, intArray + j, intArray + i, eastl::less<int>());
				EATEST_VERIFY(intArray[j] == intArraySorted[j]);
			}
		}
	}


	{
		// void radix_sort(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator buffer);
		const uint32_t kCount = 100;

		{
			RadixSortElement32* pRadixSortElementArray32     = new RadixSortElement32[kCount];
			RadixSortElement32* pRadixSortElementArrayTemp32 = new RadixSortElement32[kCount];
			for(uint32_t i = 0; i < kCount; i++)
			{
				pRadixSortElementArray32[i].mKey  = (uint16_t)(kCount - i);
				pRadixSortElementArray32[i].mData = (uint16_t)i;
			}
			radix_sort<RadixSortElement32*, extract_radix_key<RadixSortElement32> >(pRadixSortElementArray32, pRadixSortElementArray32 + kCount, pRadixSortElementArrayTemp32);
			// To do: EATEST_VERIFY sort proceeded correctly.
			delete[] pRadixSortElementArray32;
			delete[] pRadixSortElementArrayTemp32;
		}

		{
			RadixSortElement16* pRadixSortElementArray16     = new RadixSortElement16[kCount];
			RadixSortElement16* pRadixSortElementArrayTemp16 = new RadixSortElement16[kCount];
			for(uint32_t i = 0; i < kCount; i++)
			{
				pRadixSortElementArray16[i].mKey  = (uint16_t)(kCount - i);
				pRadixSortElementArray16[i].mData = (uint16_t)i;
			}
			radix_sort<RadixSortElement16*, extract_radix_key<RadixSortElement16> >(pRadixSortElementArray16, pRadixSortElementArray16 + kCount, pRadixSortElementArrayTemp16);
			// To do: EATEST_VERIFY sort proceeded correctly.
			delete[] pRadixSortElementArray16;
			delete[] pRadixSortElementArrayTemp16;
		}

		{
			RadixSortElement8* pRadixSortElementArray8     = new RadixSortElement8[kCount];
			RadixSortElement8* pRadixSortElementArrayTemp8 = new RadixSortElement8[kCount];
			for(uint32_t i = 0; i < kCount; i++)
			{
				pRadixSortElementArray8[i].mKey  = (uint8_t)(kCount - i);
				pRadixSortElementArray8[i].mData = (uint8_t)i;
			}
			radix_sort<RadixSortElement8*, extract_radix_key<RadixSortElement8> >(pRadixSortElementArray8, pRadixSortElementArray8 + kCount, pRadixSortElementArrayTemp8);
			// To do: EATEST_VERIFY sort proceeded correctly.
			delete[] pRadixSortElementArray8;
			delete[] pRadixSortElementArrayTemp8;
		}
	}


	{
		// void bucket_sort(ForwardIterator first, ForwardIterator last, ContainerArray& bucketArray, HashFunction hash)

		const size_t kElementRange = 32;
		vector<int>  intArray(1000);

		for(int i = 0; i < 1000; i++)
			intArray[i] = rng() % kElementRange;

		vector< vector<int> > bucketArray(kElementRange);
		bucket_sort(intArray.begin(), intArray.end(), bucketArray, eastl::hash_use_self<int>());
		EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));
	}


	{ 
		// stable_sort general test
		typedef eastl::less<int> IntCompare;

		int intArray[2] = { 0, 1 };

		stable_sort(intArray, intArray + 2);
		stable_sort(intArray, intArray + 2, IntCompare());
		stable_sort<int*>(intArray, intArray + 2);
		stable_sort<int*, IntCompare>(intArray, intArray + 2, IntCompare());

		MallocAllocator mallocAllocator;

		//stable_sort(intArray, intArray + 2, mallocAllocator);
		stable_sort(intArray, intArray + 2, mallocAllocator, IntCompare());
		//stable_sort<int*, MallocAllocator>(intArray, intArray + 2, mallocAllocator);
		stable_sort<int*, MallocAllocator, IntCompare>(intArray, intArray + 2, mallocAllocator, IntCompare());
	}

	{
		// stable_sort special test
		IntArrayArray   intArrayArray(2);
		IntArrayCompare compare;

		intArrayArray[0].push_back(0);
		intArrayArray[1].push_back(1);
		 
		stable_sort(intArrayArray.begin(), intArrayArray.end(), compare);
	}


	{
		// Test to verify that Compare object references are preserved.
		typedef deque<int>         IntDeque;
		typedef IntDeque::iterator IntDequeIterator;

		IntDeque        intDeque, intDequeSaved;
		StatefulCompare compare;

		// Set up intDequeSaved with random data.
		for(int n = 0; n < 500; n++)
		{
			intDequeSaved.push_back(n);

			if(rng.RandLimit(10) == 0)
			{
				intDequeSaved.push_back(n);

				if(rng.RandLimit(5) == 0)
					intDequeSaved.push_back(n);
			}
		}

		eastl::random_shuffle(intDequeSaved.begin(), intDequeSaved.end(), rng);

		StatefulCompare::Reset();
		intDeque = intDequeSaved;
		bubble_sort<IntDequeIterator, StatefulCompare&>(intDeque.begin(), intDeque.end(), compare);
		EATEST_VERIFY((StatefulCompare::nCtorCount == 0) && (StatefulCompare::nDtorCount == 0) && (StatefulCompare::nCopyCount == 0));

		StatefulCompare::Reset();
		intDeque = intDequeSaved;
		shaker_sort<IntDequeIterator, StatefulCompare&>(intDeque.begin(), intDeque.end(), compare);
		EATEST_VERIFY((StatefulCompare::nCtorCount == 0) && (StatefulCompare::nDtorCount == 0) && (StatefulCompare::nCopyCount == 0));

		StatefulCompare::Reset();
		intDeque = intDequeSaved;
		insertion_sort<IntDequeIterator, StatefulCompare&>(intDeque.begin(), intDeque.end(), compare);
		EATEST_VERIFY((StatefulCompare::nCtorCount == 0) && (StatefulCompare::nDtorCount == 0) && (StatefulCompare::nCopyCount == 0));

		StatefulCompare::Reset();
		intDeque = intDequeSaved;
		selection_sort<IntDequeIterator, StatefulCompare&>(intDeque.begin(), intDeque.end(), compare);
		EATEST_VERIFY((StatefulCompare::nCtorCount == 0) && (StatefulCompare::nDtorCount == 0) && (StatefulCompare::nCopyCount == 0));

		StatefulCompare::Reset();
		intDeque = intDequeSaved;
		shell_sort<IntDequeIterator, StatefulCompare&>(intDeque.begin(), intDeque.end(), compare);
		EATEST_VERIFY((StatefulCompare::nCtorCount == 0) && (StatefulCompare::nDtorCount == 0) && (StatefulCompare::nCopyCount == 0));

		StatefulCompare::Reset();
		intDeque = intDequeSaved;
		comb_sort<IntDequeIterator, StatefulCompare&>(intDeque.begin(), intDeque.end(), compare);
		EATEST_VERIFY((StatefulCompare::nCtorCount == 0) && (StatefulCompare::nDtorCount == 0) && (StatefulCompare::nCopyCount == 0));

		StatefulCompare::Reset();
		intDeque = intDequeSaved;
		heap_sort<IntDequeIterator, StatefulCompare&>(intDeque.begin(), intDeque.end(), compare);
		EATEST_VERIFY((StatefulCompare::nCtorCount == 0) && (StatefulCompare::nDtorCount == 0) && (StatefulCompare::nCopyCount == 0));

		StatefulCompare::Reset();
		intDeque = intDequeSaved;
		merge_sort<IntDequeIterator, EASTLAllocatorType, StatefulCompare&>(intDeque.begin(), intDeque.end(), *get_default_allocator((EASTLAllocatorType*)NULL), compare);
		EATEST_VERIFY((StatefulCompare::nCtorCount == 0) && (StatefulCompare::nDtorCount == 0) && (StatefulCompare::nCopyCount == 0));

		StatefulCompare::Reset();
		intDeque = intDequeSaved;
		quick_sort<IntDequeIterator, StatefulCompare&>(intDeque.begin(), intDeque.end(), compare);
		EATEST_VERIFY((StatefulCompare::nCtorCount == 0) && (StatefulCompare::nDtorCount == 0) && (StatefulCompare::nCopyCount == 0));
	}

	{
		// EATEST_VERIFY deque sorting can compile.
		deque<int>  intDeque;
		vector<int> intVector;

		stable_sort(intDeque.begin(),  intDeque.end());
		stable_sort(intVector.begin(), intVector.end());
	}


	{
		// Test sorting of a container of pointers to objects as opposed to a container of objects themselves.
		vector<TestObject>  toArray;
		vector<TestObject*> topArray;

		for(eastl_size_t i = 0; i < 32; i++)
			toArray.push_back(TestObject((int)rng.RandLimit(20)));
		for(eastl_size_t i = 0; i < 32; i++) // This needs to be a second loop because the addresses might change in the first loop due to container resizing.
			topArray.push_back(&toArray[i]);

		quick_sort(topArray.begin(), topArray.end(), TestObjectPtrCompare());
		EATEST_VERIFY(is_sorted(topArray.begin(), topArray.end(), TestObjectPtrCompare()));
	}


	{
		// Test sorting of a container of array indexes to objects as opposed to a container of objects themselves.

		vector<TestObject>   toArray;
		vector<eastl_size_t> toiArray;

		for(eastl_size_t i = 0; i < 32; i++)
		{
			toArray.push_back(TestObject((int)rng.RandLimit(20)));
			toiArray.push_back(i);
		}

		quick_sort(toiArray.begin(), toiArray.end(), TestObjectIndexCompare(&toArray));
		EATEST_VERIFY(is_sorted(toiArray.begin(), toiArray.end(), TestObjectIndexCompare(&toArray)));
	}


	{
		// Test of special floating point sort in the presence of NaNs.
		vector<float> floatArray;
		union FloatInt32{ float f; int32_t i; } fi;

		for(int i = 0; i < 1000; i++)
		{
			fi.i = (int32_t)rng.Rand();
			floatArray.push_back(fi.f);
		}

		// Without SafeFloatCompare, the following quick_sort will crash, hang, or generate inconsistent results. 
		quick_sort(floatArray.begin(), floatArray.end(), SafeFloatCompare());
		EATEST_VERIFY(is_sorted(floatArray.begin(), floatArray.end(), SafeFloatCompare()));
	}


	#if 0 // Disabled because it takes a long time and thus far seems to show no bug in quick_sort.
	{
		// Regression of Coverity report for Madden 2014 that quick_sort is reading beyond an array bounds within insertion_sort_simple.
		// The Madden code was sorting the 11 players on the field for a team by some criteria. We write 
		vector<int> intArray(11);
		for(eastl_size_t i = 0; i < intArray.size(); i++)
			intArray[i] = i;
		
		do {
			vector<int> intArrayCopy(intArray);

			// We need to verify that intArray[12] is never accessed. We could do that with a stomp allocator, 
			// which we don't currently have set up for the EASTL unit tests, or we could put a breakpoint in 
			// the debugger. Until we get a stomp allocator installed, do the breakpoint solution.
			quick_sort(intArrayCopy.begin(), intArrayCopy.end());
		} while(next_permutation(intArray.begin(), intArray.end()));
	}
	#endif

	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();

	return nErrorCount;
}









