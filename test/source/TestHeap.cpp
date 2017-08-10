/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/heap.h>
#include <EASTL/vector.h>
#include <EASTL/algorithm.h>
#include <EASTL/sort.h>
#include <EABase/eabase.h>
#include <algorithm> //std::pop_heap

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	#include <algorithm>
#endif

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


using namespace eastl;




int VerifyHeaps(uint32_t* pArray2, uint32_t* pArray3, uint32_t nArraySize)
{
	int  nErrorCount = 0;
	bool bResult;

	bResult = is_heap(pArray2, pArray2 + nArraySize);
	EATEST_VERIFY(bResult);

	bResult = is_heap(pArray3, pArray3 + nArraySize);
	EATEST_VERIFY(bResult);

	// bResult = (memcmp(pArray2, pArray3, nArraySize * sizeof(uint32_t)) == 0);
	// EATEST_VERIFY(bResult);
	//
	// The above does not work on iOS since CM added -stdlib=libc++ to the linker switch
	// even though it was already used in our compile switches.
	// It would appear that on clang or iOS the heap is actually structured in a unique way,
	// possibly for optimization. Iterating over the array and using pop_heap verifies
	// that the heaps have the same elements and are retrieved in the same manner.
	// The underlying storage may be different.
	uint32_t* pArray2_copy           = new uint32_t[nArraySize];
	uint32_t* pArray3_copy           = new uint32_t[nArraySize];
	
	memcpy(pArray2_copy, pArray2, sizeof(uint32_t) * nArraySize);
	memcpy(pArray3_copy, pArray3, sizeof(uint32_t) * nArraySize);
	
	for(uint32_t i = 0; i < nArraySize; i++)
	{
		EATEST_VERIFY(pArray2_copy[0] == pArray3_copy[0]);
		std::pop_heap(pArray2_copy, pArray2_copy + nArraySize - i);
		pop_heap(pArray3_copy, pArray3_copy + nArraySize - i);
	}
	delete[] pArray2_copy;
	delete[] pArray3_copy;
	return nErrorCount;
}



int TestHeap()
{
	int nErrorCount = 0;

	// We do a bit of our heap testing by simply doing rng operations and comparing
	// to a standard STL implementation of the heap functions.

	{
		#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY

			EA::UnitTest::Rand rng(EA::UnitTest::GetRandSeed());

			const int32_t kMinArraySize = 2;
			const int32_t kMaxArraySize = 1000;
			const int32_t kMinValue     = 0;
			const int32_t kMaxValue     = 500;

			// To consider, instead of using 25, try conditioning on EA::UnitTest::GetSystemSpeed().
			// I tried this, but even though Caps and PC are the same system speed, Caps was quite slower
			// than PC doing 75 loops
			for(int i = 0; (i < 25) && (nErrorCount == 0); i++)
			{
				//
				// Set up an array of data to work with as a heap.
				uint32_t  nArraySizeInitial = (uint32_t)rng.RandRange(kMinArraySize, kMaxArraySize);
				uint32_t  nArraySize        = nArraySizeInitial;
				uint32_t* pArray1           = new uint32_t[nArraySize + 1]; // Array1 is the original data. // +1 because we append an additional element in the is_heap_until test below.
				uint32_t* pArray2           = new uint32_t[nArraySize + 1]; // Array2 is the data in std::make_heap
				uint32_t* pArray3           = new uint32_t[nArraySize + 1]; // Array3 is the data in eastl::make_heap.

				for(uint32_t j = 0; j < nArraySize; j++)
					pArray1[j] = pArray2[j] = pArray3[j] = (uint32_t)rng.RandRange(kMinValue, kMaxValue);


				// make_heap
				std::make_heap(pArray2, pArray2 + nArraySize);
				make_heap(pArray3, pArray3 + nArraySize);
				VerifyHeaps(pArray2, pArray3, nArraySize);


				// is_heap_until
				{
					pArray3[nArraySize] = kMaxValue + 1;    // Append a value which is guaranteed to break the heap.
					uint32_t* pUntil = is_heap_until(pArray3, pArray3 + (nArraySize + 1));
					EATEST_VERIFY_F(pUntil == (pArray3 + nArraySize), "is_heap_until failure in iteration %d for array size %I32u.", nArraySize);
				}


				// pop_heap
				const int popCount = min<uint32_t>(200, nArraySize);
				for(int k = 0; (k < popCount) && (nErrorCount == 0); k++, nArraySize--)
				{
					std::pop_heap(pArray2, pArray2 + nArraySize);
					pArray2[nArraySize - 1] = 0xffffffff; // Set it to some value so we can recognize it in a debugger.

					pop_heap(pArray3, pArray3 + nArraySize);
					pArray3[nArraySize - 1] = 0xffffffff;

					VerifyHeaps(pArray2, pArray3, nArraySize - 1);
				}


				// push_heap
				const int pushCount = popCount;
				for(int m = 0; (m < pushCount) && (nErrorCount == 0); m++, nArraySize++)
				{
					const uint32_t n = (uint32_t)rng.RandRange(kMinValue, kMaxValue);

					pArray2[nArraySize] = n;
					std::push_heap(pArray2, pArray2 + nArraySize + 1);

					pArray3[nArraySize] = n;
					push_heap(pArray3, pArray3 + nArraySize + 1);

					VerifyHeaps(pArray2, pArray3, nArraySize + 1);
				}

				uint32_t originalSize = nArraySize;
				// remove_heap
				// Because the heap that stdlib on iOS and other platforms differs, different elements
				// will be removed. After calling remove heap, we cannot call VerifyHeaps anymore, but
				// can still check that heap format is retained.
				const int eraseCount = popCount;
				for(int e = 0; (e < eraseCount) && (nErrorCount == 0); e++, nArraySize--)
				{
					const uint32_t position = (uint32_t)rng.RandRange(0, nArraySize);

					remove_heap(pArray2, nArraySize, position);
					pArray2[nArraySize - 1] = 0xffffffff;

					remove_heap(pArray3, nArraySize, position);
					pArray3[nArraySize - 1] = 0xffffffff;

					//use is_heap_until to verify remove_heap is working.
					if(nArraySize > 1) //If we just popped last element, don't use is_heap_until
					{
						uint32_t* pUntil = is_heap_until(pArray2, pArray2 + (nArraySize));
						EATEST_VERIFY_F(pUntil == (pArray2 + nArraySize - 1), "pUntil failure for pArray2 with array size %I32u.", nArraySize);
						
						pUntil = is_heap_until(pArray3, pArray3 + (nArraySize));
						EATEST_VERIFY_F(pUntil == (pArray3 + nArraySize - 1), "failure for pArray3 with array size %I32u.", nArraySize);
					}
				}

				// push_heap -- increase the heap size back to the original size.
				for(int m = 0; (m < pushCount) && (nErrorCount == 0); m++, nArraySize++)
				{
					const uint32_t n = (uint32_t)rng.RandRange(kMinValue, kMaxValue);

					pArray2[nArraySize] = n;
					std::push_heap(pArray2, pArray2 + nArraySize + 1);

					pArray3[nArraySize] = n;
					push_heap(pArray3, pArray3 + nArraySize + 1);
				}
				
				EATEST_VERIFY_F(nArraySize == originalSize, "Array size is %d not original size %d", nArraySize , originalSize);

				uint32_t* pUntil = is_heap_until(pArray2, pArray2 + (nArraySize));
				EATEST_VERIFY_F(pUntil == (pArray2 + nArraySize), "failure for pArray2 with array size %I32u.", nArraySize);
				pUntil = is_heap_until(pArray3, pArray3 + (nArraySize));
				EATEST_VERIFY_F(pUntil == (pArray3 + nArraySize), "failure for pArray3 with array size %I32u.", nArraySize);


				// change_heap
				const int changeCount = popCount;
				for(int r = 0; (r < changeCount) && (nErrorCount == 0); r++, nArraySize--)
				{
					uint32_t position = (uint32_t)rng.RandRange(0, nArraySize);
					uint32_t newValue = (uint32_t)rng.RandRange(kMinValue, kMaxValue);

					if(rng.RandLimit(5) == 0) // One in five chance that we use the heap top position.
						position = 0;
					if(rng.RandLimit(5) != 0) // One in five chance that we do no change.
						pArray2[position] = pArray3[position] = newValue;

					// There is no std::change_heap, so we just use ours for this test.
					change_heap(pArray2, nArraySize, position);
					pArray2[nArraySize - 1] = 0xffffffff;

					change_heap(pArray3, nArraySize, position);
					pArray3[nArraySize - 1] = 0xffffffff;
					
					if(nArraySize > 1) //If we just removed last element, don't use is_heap_until
					{
						uint32_t* pUntilChanged = is_heap_until(pArray2, pArray2 + (nArraySize));
						EATEST_VERIFY_F(pUntilChanged == (pArray2 + nArraySize - 1), "failure for pArray2 with array size %I32u.", nArraySize);
						pUntilChanged = is_heap_until(pArray3, pArray3 + (nArraySize));
						EATEST_VERIFY_F(pUntilChanged == (pArray3 + nArraySize - 1), "failure for pArray3 with array size %I32u.", nArraySize);
					}
				}


				// sort_heap
				std::sort_heap(pArray2, pArray2 + nArraySize);
				sort_heap(pArray3, pArray3 + nArraySize);

				for(uint32_t q = 1; (q < nArraySize)  && (nErrorCount == 0); q++)
				{
					EATEST_VERIFY(pArray2[q-1] <= pArray2[q]);
					EATEST_VERIFY(pArray3[q-1] <= pArray3[q]);
				}
				// Free our heap data.
				delete[] pArray1;
				delete[] pArray2;
				delete[] pArray3;
			}

		#endif // EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	}

	{
		// Test aligned types.

		// Aligned objects should be CustomAllocator instead of the default, because the 
		// EASTL default might be unable to do aligned allocations, but CustomAllocator always can.
		eastl::vector<Align64, CustomAllocator> heap;

		for(int i = 0; i < 16; i++)
			heap.push_back(Align64(i));

		eastl::make_heap(heap.begin(), heap.end());
		EATEST_VERIFY(is_heap(heap.begin(), heap.end()));

		heap.push_back(Align64(7));
		eastl::push_heap(heap.begin(), heap.end());
		EATEST_VERIFY(is_heap(heap.begin(), heap.end()));

		heap.push_back(Align64(7));
		eastl::push_heap(heap.begin(), heap.end());
		heap.pop_back();
		EATEST_VERIFY(is_heap(heap.begin(), heap.end()));

		eastl::remove_heap(heap.begin(), heap.size(), (eastl_size_t)4);
		heap.pop_back();
		EATEST_VERIFY(is_heap(heap.begin(), heap.end()));

		eastl::sort_heap(heap.begin(), heap.end());
		EATEST_VERIFY(is_sorted(heap.begin(), heap.end()));
	}

	{
		Align16 heap[5];

		eastl::make_heap(heap, heap + 5);
		EATEST_VERIFY(is_heap(heap, heap + 5));

		eastl::partial_sort(heap, heap + 3, heap + 5);
	}

	return nErrorCount;
}













