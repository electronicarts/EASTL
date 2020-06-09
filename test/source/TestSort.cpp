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
#include <EASTL/list.h>
#include <EASTL/deque.h>
#include <EASTL/algorithm.h>
#include <EASTL/allocator.h>
#include <EASTL/numeric.h>
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
		template <typename Key>
		struct RadixSortElement
		{
			typedef Key radix_type;
			Key  mKey;
			uint16_t mData;

			bool operator<(const RadixSortElement<Key> &other) const
			{
				return mKey < other.mKey;
			}
		};

		typedef RadixSortElement<uint8_t> RadixSortElement8;
		typedef RadixSortElement<uint16_t> RadixSortElement16;
		typedef RadixSortElement<uint32_t> RadixSortElement32;

		template <typename integer_type>
		struct identity_extract_radix_key
		{
			typedef integer_type radix_type;

			const radix_type operator()(const integer_type& x) const
			{
				return x;
			}
		};
	} // namespace Internal

} // namespace eastl

int TestSort()
{
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
			const int64_t expectedSum = eastl::accumulate(begin(intArraySaved), end(intArraySaved), int64_t(0));

			for(int j = 0; j < 300 + (gEASTL_TestLevel * 50); j++)
			{
				eastl::random_shuffle(intArraySaved.begin(), intArraySaved.end(), rng);

				intArray = intArraySaved;
				bubble_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));
				EATEST_VERIFY(eastl::accumulate(begin(intArraySaved), end(intArraySaved), int64_t(0)) == expectedSum);

				intArray = intArraySaved;
				shaker_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));
				EATEST_VERIFY(eastl::accumulate(begin(intArraySaved), end(intArraySaved), int64_t(0)) == expectedSum);

				intArray = intArraySaved;
				insertion_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));
				EATEST_VERIFY(eastl::accumulate(begin(intArraySaved), end(intArraySaved), int64_t(0)) == expectedSum);

				intArray = intArraySaved;
				selection_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));
				EATEST_VERIFY(eastl::accumulate(begin(intArraySaved), end(intArraySaved), int64_t(0)) == expectedSum);

				intArray = intArraySaved;
				shell_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));
				EATEST_VERIFY(eastl::accumulate(begin(intArraySaved), end(intArraySaved), int64_t(0)) == expectedSum);

				intArray = intArraySaved;
				comb_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));
				EATEST_VERIFY(eastl::accumulate(begin(intArraySaved), end(intArraySaved), int64_t(0)) == expectedSum);

				intArray = intArraySaved;
				heap_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));
				EATEST_VERIFY(eastl::accumulate(begin(intArraySaved), end(intArraySaved), int64_t(0)) == expectedSum);

				intArray = intArraySaved;
				merge_sort(intArray.begin(), intArray.end(), *get_default_allocator((EASTLAllocatorType*)NULL));
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));
				EATEST_VERIFY(eastl::accumulate(begin(intArraySaved), end(intArraySaved), int64_t(0)) == expectedSum);

				intArray = intArraySaved;
				vector<int64_t> buffer(intArray.size());
				merge_sort_buffer(intArray.begin(), intArray.end(), buffer.data());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));
				EATEST_VERIFY(eastl::accumulate(begin(intArraySaved), end(intArraySaved), int64_t(0)) == expectedSum);

				intArray = intArraySaved;
				quick_sort(intArray.begin(), intArray.end());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));
				EATEST_VERIFY(eastl::accumulate(begin(intArraySaved), end(intArraySaved), int64_t(0)) == expectedSum);

				intArray = intArraySaved;
				buffer.resize(intArray.size()/2);
				tim_sort_buffer(intArray.begin(), intArray.end(), buffer.data());
				EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));
				EATEST_VERIFY(eastl::accumulate(begin(intArraySaved), end(intArraySaved), int64_t(0)) == expectedSum);
			}
		}
	}

	// Test insertion_sort() does not invalidate a BidirectionalIterator by doing --BidirectionalIterator.begin()
	{
		// Test Passes if the Test doesn't crash
		eastl::deque<int> deque;
		deque.push_back(1);

		insertion_sort(deque.begin(), deque.end());

		insertion_sort(deque.begin(), deque.end(), eastl::less<int>{});
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

				toArray = toArraySaved;
				vector<TestObject> buffer(toArray.size()/2);
				tim_sort_buffer(toArray.begin(), toArray.end(), buffer.data());
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end()));
			}
		}
	}

	// Test that stable sorting algorithms are actually stable
	{
		struct StableSortTestObj
		{
			StableSortTestObj()
			{
			}

			StableSortTestObj(int value)
				:value(value)
				,initialPositionIndex(0)
			{
			}

			int value;
			size_t initialPositionIndex;
		};

		// During the test this comparison is used to sort elements based on value.
		struct StableSortCompare
		{
			bool operator()(const StableSortTestObj& a, const StableSortTestObj& b)
			{
				return a.value < b.value;
			}
		};

		// During the test this comparison is used to verify the sort was a stable sort.  i.e. if values are the same then
		// their relative position should be maintained.
		struct StableSortCompareForStability
		{
			bool operator()(const StableSortTestObj& a, const StableSortTestObj& b)
			{
				if (a.value != b.value)
				{
					return a.value < b.value;
				}
				else
				{
					return a.initialPositionIndex < b.initialPositionIndex;
				}
			}
		};

		vector<StableSortTestObj> toArray, toArraySaved;
		StableSortCompare compare;
		StableSortCompareForStability compareForStability;

		for (int i = 0; i < (150 + (gEASTL_TestLevel * 200)); i += (i < 5) ? 1 : 37) // array sizes of 0 to 300 - 2100, depending on test level.
		{
			for (int n = 0; n < i; n++)
			{
				toArraySaved.push_back(StableSortTestObj(n));

				if (rng.RandLimit(10) == 0)
				{
					toArraySaved.push_back(StableSortTestObj(n));

					if (rng.RandLimit(5) == 0)
						toArraySaved.push_back(StableSortTestObj(n));
				}
			}
			vector<StableSortTestObj> tempBuffer;
			tempBuffer.resize(toArraySaved.size());

			for (int j = 0; j < 300 + (gEASTL_TestLevel * 50); j++)
			{
				eastl::random_shuffle(toArraySaved.begin(), toArraySaved.end(), rng);
				// Store the intial position of each element in the array before sorting.  This position can then be used to verify that the sorting operation is stable.
				for (vector<StableSortTestObj>::size_type k = 0; k < toArraySaved.size(); k++)
				{
					toArraySaved[k].initialPositionIndex = k;
				}

				toArray = toArraySaved;
				bubble_sort(toArray.begin(), toArray.end(), compare);
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end(), compareForStability));

				toArray = toArraySaved;
				shaker_sort(toArray.begin(), toArray.end(), compare);
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end(), compareForStability));

				toArray = toArraySaved;
				insertion_sort(toArray.begin(), toArray.end(), compare);
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end(), compareForStability));

				toArray = toArraySaved;
				tim_sort_buffer(toArray.begin(), toArray.end(), tempBuffer.data(), compare);
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end(), compareForStability));

				toArray = toArraySaved;
				merge_sort(toArray.begin(), toArray.end(), *get_default_allocator((EASTLAllocatorType*)NULL), compare);
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end(), compareForStability));

				toArray = toArraySaved;
				merge_sort_buffer(toArray.begin(), toArray.end(), tempBuffer.data(), compare);
				EATEST_VERIFY(is_sorted(toArray.begin(), toArray.end(), compareForStability));
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
			EATEST_VERIFY(is_sorted(pRadixSortElementArray32, pRadixSortElementArray32 + kCount));
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
			EATEST_VERIFY(is_sorted(pRadixSortElementArray16, pRadixSortElementArray16 + kCount));
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
			EATEST_VERIFY(is_sorted(pRadixSortElementArray8, pRadixSortElementArray8 + kCount));
			delete[] pRadixSortElementArray8;
			delete[] pRadixSortElementArrayTemp8;
		}
	}

	{
		// Do some white-box testing of radix sort to verify internal optimizations work properly for some edge cases.

		{
			uint32_t input[] = { 123, 15, 76, 2, 74, 12, 62, 91 };
			uint32_t buffer[EAArrayCount(input)];
			radix_sort<uint32_t*, identity_extract_radix_key<uint32_t>>(begin(input), end(input), buffer);
			EATEST_VERIFY(is_sorted(begin(input), end(input)));
		}
		{
			// Test values where some digit positions have identical values
			uint32_t input[] = { 0x75000017, 0x74000003, 0x73000045, 0x76000024, 0x78000033, 0x76000099, 0x78000043, 0x75000010 };
			uint32_t buffer[EAArrayCount(input)];
			radix_sort<uint32_t*, identity_extract_radix_key<uint32_t>>(begin(input), end(input), buffer);
			EATEST_VERIFY(is_sorted(begin(input), end(input)));
		}
		{
			// Test values where some digit positions have identical values
			uint32_t input[] = { 0x00750017, 0x00740003, 0x00730045, 0x00760024, 0x00780033, 0x00760099, 0x00780043, 0x00750010 };
			uint32_t buffer[EAArrayCount(input)];
			radix_sort<uint32_t*, identity_extract_radix_key<uint32_t>>(begin(input), end(input), buffer);
			EATEST_VERIFY(is_sorted(begin(input), end(input)));
		}
		{
			// Test values where an odd number of scatter operations will be done during sorting (which forces a copy operation to move values back to the input buffer).
			uint32_t input[] = { 0x00000017, 0x00000003, 0x00000045, 0x00000024, 0x00000033, 0x00000099, 0x00000043, 0x00000010 };
			uint32_t buffer[EAArrayCount(input)];
			radix_sort<uint32_t*, identity_extract_radix_key<uint32_t>>(begin(input), end(input), buffer);
			EATEST_VERIFY(is_sorted(begin(input), end(input)));
		}
	}

	{
		// Test different values for DigitBits

		{
			uint32_t input[] = {2514513, 6278225, 2726217, 963245656, 35667326, 2625624562, 3562562562, 1556256252};
			uint32_t buffer[EAArrayCount(input)];
			radix_sort<uint32_t*, identity_extract_radix_key<uint32_t>, 1>(begin(input), end(input), buffer);
			EATEST_VERIFY(is_sorted(begin(input), end(input)));
		}
		{
			uint32_t input[] = { 2514513, 6278225, 2726217, 963245656, 35667326, 2625624562, 3562562562, 1556256252 };
			uint32_t buffer[EAArrayCount(input)];
			radix_sort<uint32_t*, identity_extract_radix_key<uint32_t>, 3>(begin(input), end(input), buffer);
			EATEST_VERIFY(is_sorted(begin(input), end(input)));
		}
		{
			uint32_t input[] = { 2514513, 6278225, 2726217, 963245656, 35667326, 2625624562, 3562562562, 1556256252 };
			uint32_t buffer[EAArrayCount(input)];
			radix_sort<uint32_t*, identity_extract_radix_key<uint32_t>, 6>(begin(input), end(input), buffer);
			EATEST_VERIFY(is_sorted(begin(input), end(input)));
		}
		{
			// Test a value for DigitBits that is more than half the size of the type.
			uint16_t input[] = { 14513, 58225, 26217, 34656, 63326, 24562, 35562, 15652 };
			uint16_t buffer[EAArrayCount(input)];
			radix_sort<uint16_t*, identity_extract_radix_key<uint16_t>, 11>(begin(input), end(input), buffer);
			EATEST_VERIFY(is_sorted(begin(input), end(input)));
		}
		{
			// Test a value for DigitBits that is the size of the type itself.
			uint8_t input[] = { 113, 225, 217, 56, 26, 162, 62, 152 };
			uint8_t buffer[EAArrayCount(input)];
			radix_sort<uint8_t*, identity_extract_radix_key<uint8_t>, 8>(begin(input), end(input), buffer);
			EATEST_VERIFY(is_sorted(begin(input), end(input)));
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

		StatefulCompare::Reset();
		vector<int> buffer(intDeque.size()/2);
		intDeque = intDequeSaved;
		tim_sort_buffer<IntDequeIterator, int, StatefulCompare&>(intDeque.begin(), intDeque.end(), buffer.data(), compare);
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

	{
		auto test_stable_sort = [&](auto testArray, size_t count)
		{
			auto isEven = [](auto val) { return (val % 2) == 0; };
			auto isOdd  = [](auto val) { return (val % 2) != 0; };

			for (size_t i = 0; i < count; i++)
				testArray.push_back((uint16_t)rng.Rand());

			vector<uint16_t> evenArray;
			vector<uint16_t> oddArray;

			eastl::copy_if(testArray.begin(), testArray.end(), eastl::back_inserter(evenArray), isEven);
			eastl::copy_if(testArray.begin(), testArray.end(), eastl::back_inserter(oddArray), isOdd);

			const auto boundary = eastl::stable_partition(testArray.begin(), testArray.end(), isEven);

			const auto evenCount = eastl::distance(testArray.begin(), boundary);
			const auto oddCount = eastl::distance(boundary, testArray.end());

			const auto evenExpectedCount = (ptrdiff_t)evenArray.size();
			const auto oddExpectedCount = (ptrdiff_t)oddArray.size();

			EATEST_VERIFY(evenCount == evenExpectedCount);
			EATEST_VERIFY(oddCount == oddExpectedCount);
			EATEST_VERIFY(eastl::equal(testArray.begin(), boundary, evenArray.begin()));
			EATEST_VERIFY(eastl::equal(boundary, testArray.end(), oddArray.begin()));
		};

		test_stable_sort(vector<uint16_t>(), 1000); // Test stable_partition
		test_stable_sort(vector<uint16_t>(), 0);	// Test stable_partition on empty container
		test_stable_sort(vector<uint16_t>(), 1);	// Test stable_partition on container of one element
		test_stable_sort(vector<uint16_t>(), 2);	// Test stable_partition on container of two element
		test_stable_sort(list<uint16_t>(),   0);	// Test stable_partition on bidirectional iterator (not random access)
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









