/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_COPY_HELP_H
#define EASTL_INTERNAL_COPY_HELP_H


#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#include <EASTL/internal/config.h>
#include <EASTL/type_traits.h>
#include <EASTL/iterator.h>
#include <string.h> // memcpy, memcmp, memmove


namespace eastl
{
	/// move / move_n / move_backward 
	/// copy / copy_n / copy_backward
	///
	/// We want to optimize move, move_n, move_backward, copy, copy_backward, copy_n to do memmove operations
	/// when possible. 
	///
	/// We could possibly use memcpy, though it has stricter overlap requirements than the move and copy 
	/// algorithms and would require a runtime if/else to choose it over memmove. In particular, memcpy 
	/// allows no range overlap at all, whereas move/copy allow output end overlap and move_backward/copy_backward 
	/// allow output begin overlap. Despite this it might be useful to use memcpy for any platforms where
	/// memcpy is significantly faster than memmove, and since in most cases the copy/move operation in fact
	/// doesn't target overlapping memory and so memcpy would be usable.
	///
	/// We can use memmove/memcpy if the following hold true:
	///     InputIterator and OutputIterator are of the same type.
	///     InputIterator and OutputIterator are of type contiguous_iterator_tag or simply are pointers (the two are virtually synonymous).
	///     is_trivially_copyable<T>::value is true. i.e. the constructor T(const T& t) (or T(T&& t) if present) can be replaced by memmove(this, &t, sizeof(T))
	/// 
	/// copy normally differs from move, but there is a case where copy is the same as move: when copy is 
	/// used with a move_iterator. We handle that case here by detecting that copy is being done with a 
	/// move_iterator and redirect it to move (which can take advantage of memmove/memcpy).
	///
	/// The generic_iterator class is typically used for wrapping raw memory pointers so they can act like 
	/// formal iterators. Since pointers provide an opportunity for memmove/memcpy operations, we can 
	/// detect a generic iterator and use it's wrapped type as a pointer if it happens to be one.

	// Implementation moving copying both trivial and non-trivial data via a lesser iterator than random-access.
	template <typename /*InputIteratorCategory*/, bool /*isMove*/, bool /*canMemmove*/>
	struct move_and_copy_helper
	{
		template <typename InputIterator, typename OutputIterator>
		static OutputIterator move_or_copy(InputIterator first, InputIterator last, OutputIterator result)
		{
			for(; first != last; ++result, ++first)
				*result = *first;
			return result;
		}
	};

	// Specialization for copying non-trivial data via a random-access iterator. It's theoretically faster because the compiler can see the count when its a compile-time const.
	// This specialization converts the random access InputIterator last-first to an integral type. There's simple way for us to take advantage of a random access output iterator,
	// as the range is specified by the input instead of the output, and distance(first, last) for a non-random-access iterator is potentially slow.
	template <> 
	struct move_and_copy_helper<EASTL_ITC_NS::random_access_iterator_tag, false, false>
	{
		template <typename InputIterator, typename OutputIterator>
		static OutputIterator move_or_copy(InputIterator first, InputIterator last, OutputIterator result)
		{
			typedef typename eastl::iterator_traits<InputIterator>::difference_type difference_type;

			for(difference_type n = (last - first); n > 0; --n, ++first, ++result)
				*result = *first;

			return result;
		}
	};

	// Specialization for moving non-trivial data via a lesser iterator than random-access.
	template <typename InputIteratorCategory>
	struct move_and_copy_helper<InputIteratorCategory, true, false>
	{
		template <typename InputIterator, typename OutputIterator>
		static OutputIterator move_or_copy(InputIterator first, InputIterator last, OutputIterator result)
		{
			for(; first != last; ++result, ++first)
				*result = eastl::move(*first);
			return result;
		}
	};
	
	// Specialization for moving non-trivial data via a random-access iterator. It's theoretically faster because the compiler can see the count when its a compile-time const.
	template <>
	struct move_and_copy_helper<EASTL_ITC_NS::random_access_iterator_tag, true, false>
	{
		template <typename InputIterator, typename OutputIterator>
		static OutputIterator move_or_copy(InputIterator first, InputIterator last, OutputIterator result)
		{
			typedef typename eastl::iterator_traits<InputIterator>::difference_type difference_type;

			for(difference_type n = (last - first); n > 0; --n, ++first, ++result)
				*result = eastl::move(*first);

			return result;
		}
	};

	// Specialization for when we can use memmove/memcpy. See the notes above for what conditions allow this.
	template <bool isMove>
	struct move_and_copy_helper<EASTL_ITC_NS::random_access_iterator_tag, isMove, true>
	{
		template <typename T>
		static T* move_or_copy(const T* first, const T* last, T* result)
		{
			if (EASTL_UNLIKELY(first == last))
				return result;

			// We could use memcpy here if there's no range overlap, but memcpy is rarely much faster than memmove.
			return (T*)memmove(result, first, (size_t)((uintptr_t)last - (uintptr_t)first)) + (last - first);
		}
	};



	template <bool isMove, typename InputIterator, typename OutputIterator>
	inline OutputIterator move_and_copy_chooser(InputIterator first, InputIterator last, OutputIterator result)
	{
		typedef typename eastl::iterator_traits<InputIterator>::iterator_category  IIC;
		typedef typename eastl::iterator_traits<OutputIterator>::iterator_category OIC;
		typedef typename eastl::iterator_traits<InputIterator>::value_type         value_type_input;
		typedef typename eastl::iterator_traits<OutputIterator>::value_type        value_type_output;

		const bool canBeMemmoved = eastl::is_trivially_copyable<value_type_output>::value && 
								   eastl::is_same<value_type_input, value_type_output>::value && 
								  (eastl::is_pointer<InputIterator>::value  || eastl::is_same<IIC, eastl::contiguous_iterator_tag>::value) && 
								  (eastl::is_pointer<OutputIterator>::value || eastl::is_same<OIC, eastl::contiguous_iterator_tag>::value);

		return eastl::move_and_copy_helper<IIC, isMove, canBeMemmoved>::move_or_copy(first, last, result); // Need to chose based on the input iterator tag and not the output iterator tag, because containers accept input ranges of iterator types different than self.
	}


	// We have a second layer of unwrap_iterator calls because the original iterator might be something like move_iterator<generic_iterator<int*> > (i.e. doubly-wrapped).
	template <bool isMove, typename InputIterator, typename OutputIterator>
	inline OutputIterator move_and_copy_unwrapper(InputIterator first, InputIterator last, OutputIterator result)
	{
		return OutputIterator(eastl::move_and_copy_chooser<isMove>(eastl::unwrap_iterator(first), eastl::unwrap_iterator(last), eastl::unwrap_iterator(result))); // Have to convert to OutputIterator because result.base() could be a T*
	}


	/// move
	///
	/// After this operation the elements in the moved-from range will still contain valid values of the 
	/// appropriate type, but not necessarily the same values as before the move. 
	/// Returns the end of the result range.
	/// Note: When moving between containers, the dest range must be valid; this function doesn't resize containers.
	/// Note: if result is within [first, last), move_backward must be used instead of move. 
	///
	/// Example usage:
	///     eastl::move(myArray.begin(), myArray.end(), myDestArray.begin());
	///
	/// Reference implementation:
	///     template <typename InputIterator, typename OutputIterator>
	///     OutputIterator move(InputIterator first, InputIterator last, OutputIterator result)
	///     {
	///         while(first != last)
	///             *result++ = eastl::move(*first++);
	///         return result;
	///     }

	template <typename InputIterator, typename OutputIterator>
	inline OutputIterator move(InputIterator first, InputIterator last, OutputIterator result)
	{
		return eastl::move_and_copy_unwrapper<true>(eastl::unwrap_iterator(first), eastl::unwrap_iterator(last), result);
	}


	/// copy
	///
	/// Effects: Copies elements in the range [first, last) into the range [result, result + (last - first))
	/// starting from first and proceeding to last. For each nonnegative integer n < (last - first),
	/// performs *(result + n) = *(first + n).
	///
	/// Returns: result + (last - first). That is, returns the end of the result. Note that this 
	/// is different from how memmove/memcpy work, as they return the beginning of the result.
	///
	/// Requires: result shall not be in the range [first, last). But the end of the result range
	/// may in fact be within the input rante.
	///
	/// Complexity: Exactly 'last - first' assignments.
	///
	template <typename InputIterator, typename OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
	{
		const bool isMove = eastl::is_move_iterator<InputIterator>::value; EA_UNUSED(isMove);

		return eastl::move_and_copy_unwrapper<isMove>(eastl::unwrap_iterator(first), eastl::unwrap_iterator(last), result);
	}
} // namespace eastl

#endif // Header include guard















