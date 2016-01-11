/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// This file implements additional sort algorithms beyond the basic set.
// Included here are:
//    selection_sort        -- Unstable.
//    shaker_sort           -- Stable.
//    bucket_sort           -- Stable. 
//
//////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_SORT_EXTRA_H
#define EASTL_SORT_EXTRA_H


#include <EASTL/internal/config.h>
#include <EASTL/iterator.h>
#include <EASTL/algorithm.h>
#include <EASTL/functional.h>
#include <EASTL/heap.h>
#include <EASTL/sort.h>             // For backwards compatibility due to sorts moved from here to sort.h.
#include <EASTL/allocator.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{
	/// selection_sort
	///
	/// Implements the SelectionSort algorithm.
	///
	template <typename ForwardIterator, typename StrictWeakOrdering>
	void selection_sort(ForwardIterator first, ForwardIterator last, StrictWeakOrdering compare)
	{
		ForwardIterator iCurrent, iMin;

		for(; first != last; ++first)
		{
			iCurrent = first;
			iMin     = iCurrent;

			for(++iCurrent; iCurrent != last; ++iCurrent)
			{
				if(compare(*iCurrent, *iMin))
				{
					EASTL_VALIDATE_COMPARE(!compare(*iMin, *iCurrent)); // Validate that the compare function is sane.
					iMin = iCurrent;
				}
			}

			if(first != iMin)
				eastl::iter_swap(first, iMin);
		}
	} // selection_sort

	template <typename ForwardIterator>
	inline void selection_sort(ForwardIterator first, ForwardIterator last)
	{
		typedef eastl::less<typename eastl::iterator_traits<ForwardIterator>::value_type> Less;

		eastl::selection_sort<ForwardIterator, Less>(first, last, Less());
	}



	/// shaker_sort
	///
	/// Implements the ShakerSort algorithm, which is a sorting algorithm which 
	/// improves on bubble_sort by sweeping both from left to right and right 
	/// to left, resulting in less iteration.
	///
	template <typename BidirectionalIterator, typename StrictWeakOrdering>
	void shaker_sort(BidirectionalIterator first, BidirectionalIterator last, StrictWeakOrdering compare)
	{
		if(first != last)
		{
			BidirectionalIterator iCurrent, iNext, iLastModified;

			--last;

			while(first != last)
			{
				iLastModified = first;

				for(iCurrent = first; iCurrent != last; iCurrent = iNext)
				{
					iNext = iCurrent;
					++iNext;

					if(compare(*iNext, *iCurrent))
					{
						EASTL_VALIDATE_COMPARE(!compare(*iCurrent, *iNext)); // Validate that the compare function is sane.
						iLastModified = iCurrent;
						eastl::iter_swap(iCurrent, iNext);
					}
				}

				last = iLastModified;

				if(first != last)
				{
					for(iCurrent = last; iCurrent != first; iCurrent = iNext)
					{
						iNext = iCurrent;
						--iNext;

						if(compare(*iCurrent, *iNext))
						{
							EASTL_VALIDATE_COMPARE(!compare(*iNext, *iCurrent)); // Validate that the compare function is sane.
							iLastModified = iCurrent;
							eastl::iter_swap(iNext, iCurrent);
						}
					}
					first = iLastModified;
				}
			}
		}
	} // shaker_sort

	template <typename BidirectionalIterator>
	inline void shaker_sort(BidirectionalIterator first, BidirectionalIterator last)
	{
		typedef eastl::less<typename eastl::iterator_traits<BidirectionalIterator>::value_type> Less;

		eastl::shaker_sort<BidirectionalIterator, Less>(first, last, Less());
	}



	/// bucket_sort
	///
	/// Implements the BucketSort algorithm. 
	///
	/// Example usage:
	///  const size_t kElementRange = 32;
	///  vector<int>  intArray(1000);
	///  
	///  for(int i = 0; i < 1000; i++)
	///     intArray[i] = rand() % kElementRange;
	///  
	///  vector< vector<int> > bucketArray(kElementRange);
	///  bucket_sort(intArray.begin(), intArray.end(), bucketArray, eastl::hash_use_self<int>());
	///
	template <typename T>
	struct hash_use_self
	{
		T operator()(const T& x) const
			{ return x; }
	};

	// Requires buckeyArray to be an array of arrays with a size equal to the range of values
	// returned by the hash function. The hash function is required to return a unique value
	// for each uniquely sorted element. Usually the way this is done is the elements are 
	// integers of a limited range (e.g. 0-64) and the hash function returns the element value
	// itself. If you had a case where all elements were always even numbers (e.g. 0-128), 
	// you could use a custom hash function that returns (element value / 2).
	//
	// The user is required to provide an empty bucketArray to this function. This function returns
	// with the bucketArray non-empty. This function doesn't clear the bucketArray because that takes
	// time and the user might not need it to be cleared, at least at that time.
	// 
	template <typename ForwardIterator, typename ContainerArray, typename HashFunction>
	void bucket_sort(ForwardIterator first, ForwardIterator last, ContainerArray& bucketArray, HashFunction hash /*= hash_use_self*/)
	{
		for(ForwardIterator iInput = first; iInput != last; ++iInput)
			bucketArray[hash(*iInput)].push_back(*iInput);

		for(typename ContainerArray::const_iterator iBucket = bucketArray.begin(); iBucket != bucketArray.end(); ++iBucket)
			first = eastl::copy((*iBucket).begin(), (*iBucket).end(), first);
	}



} // namespace eastl


#endif // Header include guard




















