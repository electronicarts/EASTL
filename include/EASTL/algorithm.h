/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements some of the primary algorithms from the C++ STL
// algorithm library. These versions are just like that STL versions and so
// are redundant. They are provided solely for the purpose of projects that
// either cannot use standard C++ STL or want algorithms that have guaranteed
// identical behaviour across platforms.
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Definitions
//
// You will notice that we are very particular about the templated typenames
// we use here. You will notice that we follow the C++ standard closely in
// these respects. Each of these typenames have a specific meaning;
// this is why we don't just label templated arguments with just letters
// such as T, U, V, A, B. Here we provide a quick reference for the typenames
// we use. See the C++ standard, section 25-8 for more details.
//    --------------------------------------------------------------
//    typename                     Meaning
//    --------------------------------------------------------------
//    T                            The value type.
//    Compare                      A function which takes two arguments and returns the lesser of the two.
//    Predicate                    A function which takes one argument returns true if the argument meets some criteria.
//    BinaryPredicate              A function which takes two arguments and returns true if some criteria is met (e.g. they are equal).
//    StrickWeakOrdering           A BinaryPredicate that compares two objects, returning true if the first precedes the second. Like Compare but has additional requirements. Used for sorting routines.
//    Function                     A function which takes one argument and applies some operation to the target.
//    Size                         A count or size.
//    Generator                    A function which takes no arguments and returns a value (which will usually be assigned to an object).
//    UnaryOperation               A function which takes one argument and returns a value (which will usually be assigned to second object).
//    BinaryOperation              A function which takes two arguments and returns a value (which will usually be assigned to a third object).
//    InputIterator                An input iterator (iterator you read from) which allows reading each element only once and only in a forward direction.
//    ForwardIterator              An input iterator which is like InputIterator except it can be reset back to the beginning.
//    BidirectionalIterator        An input iterator which is like ForwardIterator except it can be read in a backward direction as well.
//    RandomAccessIterator         An input iterator which can be addressed like an array. It is a superset of all other input iterators.
//    OutputIterator               An output iterator (iterator you write to) which allows writing each element only once in only in a forward direction.
//
// Note that with iterators that a function which takes an InputIterator will
// also work with a ForwardIterator, BidirectionalIterator, or RandomAccessIterator.
// The given iterator type is merely the -minimum- supported functionality the
// iterator must support.
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Optimizations
//
// There are a number of opportunities for optimizations that we take here
// in this library. The most obvious kinds are those that subsitute memcpy
// in the place of a conventional loop for data types with which this is
// possible. The algorithms here are optimized to a higher level than currently
// available C++ STL algorithms from vendors such as Microsoft. This is especially
// so for game programming on console devices, as we do things such as reduce
// branching relative to other STL algorithm implementations. However, the
// proper implementation of these algorithm optimizations is a fairly tricky
// thing.
//
// The various things we look to take advantage of in order to implement
// optimizations include:
//    - Taking advantage of random access iterators.
//    - Taking advantage of trivially copyable data types (types for which it is safe to memcpy or memmove).
//    - Taking advantage of type_traits in general.
//    - Reducing branching and taking advantage of likely branch predictions.
//    - Taking advantage of issues related to pointer and reference aliasing.
//    - Improving cache coherency during memory accesses.
//    - Making code more likely to be inlinable by the compiler.
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Supported Algorithms
//
// Algorithms that we implement are listed here. Note that these items are not
// all within this header file, as we split up the header files in order to
// improve compilation performance. Items marked with '+' are items that are
// extensions which don't exist in the C++ standard.
//
//    -------------------------------------------------------------------------------
//      Algorithm                                   Notes
//    -------------------------------------------------------------------------------
//      adjacent_find
//      adjacent_find<Compare>
//      all_of                                      C++11
//      any_of                                      C++11
//      none_of                                     C++11
//      binary_search
//      binary_search<Compare>
//     +binary_search_i
//     +binary_search_i<Compare>
//     +change_heap                                 Found in heap.h
//     +change_heap<Compare>                        Found in heap.h
//      clamp
//      copy
//      copy_if                                     C++11
//      copy_n                                      C++11
//      copy_backward
//      count
//      count_if
//      equal
//      equal<Compare>
//      equal_range
//      equal_range<Compare>
//      fill
//      fill_n
//      find
//      find_end
//      find_end<Compare>
//      find_first_of
//      find_first_of<Compare>
//     +find_first_not_of
//     +find_first_not_of<Compare>
//     +find_last_of
//     +find_last_of<Compare>
//     +find_last_not_of
//     +find_last_not_of<Compare>
//      find_if
//      find_if_not
//      for_each
//      generate
//      generate_n
//     +identical
//     +identical<Compare>
//      iter_swap
//      lexicographical_compare
//      lexicographical_compare<Compare>
//      lexicographical_compare_three_way
//      lower_bound
//      lower_bound<Compare>
//      make_heap                                   Found in heap.h
//      make_heap<Compare>                          Found in heap.h
//      min
//      min<Compare>
//      max
//      max<Compare>
//     +min_alt                                     Exists to work around the problem of conflicts with min/max #defines on some systems.
//     +min_alt<Compare>
//     +max_alt
//     +max_alt<Compare>
//     +median
//     +median<Compare>
//      merge                                       Found in sort.h
//      merge<Compare>                              Found in sort.h
//      min_element
//      min_element<Compare>
//      max_element
//      max_element<Compare>
//      mismatch
//      mismatch<Compare>
//      move
//      move_backward
//      nth_element                                 Found in sort.h
//      nth_element<Compare>                        Found in sort.h
//      partial_sort                                Found in sort.h
//      partial_sort<Compare>                       Found in sort.h
//      push_heap                                   Found in heap.h
//      push_heap<Compare>                          Found in heap.h
//      pop_heap                                    Found in heap.h
//      pop_heap<Compare>                           Found in heap.h
//      random_shuffle<Random>
//      remove
//      remove_if
//     +apply_and_remove
//     +apply_and_remove_if
//      remove_copy
//      remove_copy_if
//     +remove_heap                                 Found in heap.h
//     +remove_heap<Compare>                        Found in heap.h
//      replace
//      replace_if
//      replace_copy
//      replace_copy_if
//      reverse_copy
//      reverse
//      random_shuffle
//      rotate
//      rotate_copy
//      search
//      search<Compare>
//      search_n
//      set_difference
//      set_difference<Compare>
//      set_difference_2
//      set_difference_2<Compare>
//      set_decomposition
//      set_decomposition<Compare>
//      set_intersection
//      set_intersection<Compare>
//      set_symmetric_difference
//      set_symmetric_difference<Compare>
//      set_union
//      set_union<Compare>
//      sort                                        Found in sort.h
//      sort<Compare>                               Found in sort.h
//      sort_heap                                   Found in heap.h
//      sort_heap<Compare>                          Found in heap.h
//      stable_sort                                 Found in sort.h
//      stable_sort<Compare>                        Found in sort.h
//      partition                                   Found in sort.h
//      stable_partition                            Found in sort.h
//      swap
//      swap_ranges
//      transform
//      transform<Operation>
//      unique
//      unique<Compare>
//      upper_bound
//      upper_bound<Compare>
//      is_permutation
//      is_permutation<Predicate>
//      next_permutation
//      next_permutation<Compare>
//      is_partitioned
//      partition_point
//
// Algorithms from the C++ standard that we don't implement are listed here.
// Most of these items are absent because they aren't used very often.
// They also happen to be the more complicated than other algorithms.
// However, we can implement any of these functions for users that might
// need them.
//      includes
//      includes<Compare>
//      inplace_merge
//      inplace_merge<Compare>
//      partial_sort_copy
//      partial_sort_copy<Compare>
//      prev_permutation
//      prev_permutation<Compare>
//      search_n<Compare>
//      unique_copy
//      unique_copy<Compare>
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ALGORITHM_H
#define EASTL_ALGORITHM_H


#include <EASTL/internal/config.h>
#include <EASTL/type_traits.h>
#include <EASTL/internal/move_help.h>
#include <EASTL/internal/copy_help.h>
#include <EASTL/internal/fill_help.h>
#include <EASTL/initializer_list.h>
#include <EASTL/iterator.h>
#include <EASTL/functional.h>
#include <EASTL/utility.h>
#include <EASTL/random.h>
#include <EASTL/compare.h>

EA_DISABLE_ALL_VC_WARNINGS();

	#if defined(EA_COMPILER_MSVC) && (defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64))
		#include <intrin.h>
	#endif

	#include <stddef.h>
	#include <string.h> // memcpy, memcmp, memmove

EA_RESTORE_ALL_VC_WARNINGS();

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



///////////////////////////////////////////////////////////////////////////////
// min/max workaround
//
// MSVC++ has #defines for min/max which collide with the min/max algorithm
// declarations. The following may still not completely resolve some kinds of
// problems with MSVC++ #defines, though it deals with most cases in production
// game code.
//
#if EASTL_NOMINMAX
	#ifdef min
		#undef min
	#endif
	#ifdef max
		#undef max
	#endif
#endif




namespace eastl
{
	/// min_element
	///
	/// min_element finds the smallest element in the range [first, last).
	/// It returns the first iterator i in [first, last) such that no other
	/// iterator in [first, last) points to a value smaller than *i.
	/// The return value is last if and only if [first, last) is an empty range.
	///
	/// Returns: The first iterator i in the range [first, last) such that
	/// for any iterator j in the range [first, last) the following corresponding
	/// condition holds: !(*j < *i).
	///
	/// Complexity: Exactly 'max((last - first) - 1, 0)' applications of the
	/// corresponding comparisons.
	///
	template <typename ForwardIterator>
	ForwardIterator min_element(ForwardIterator first, ForwardIterator last)
	{
		if(first != last)
		{
			ForwardIterator currentMin = first;

			while(++first != last)
			{
				if(*first < *currentMin)
					currentMin = first;
			}
			return currentMin;
		}
		return first;
	}


	/// min_element
	///
	/// min_element finds the smallest element in the range [first, last).
	/// It returns the first iterator i in [first, last) such that no other
	/// iterator in [first, last) points to a value smaller than *i.
	/// The return value is last if and only if [first, last) is an empty range.
	///
	/// Returns: The first iterator i in the range [first, last) such that
	/// for any iterator j in the range [first, last) the following corresponding
	/// conditions hold: compare(*j, *i) == false.
	///
	/// Complexity: Exactly 'max((last - first) - 1, 0)' applications of the
	/// corresponding comparisons.
	///
	template <typename ForwardIterator, typename Compare>
	ForwardIterator min_element(ForwardIterator first, ForwardIterator last, Compare compare)
	{
		if(first != last)
		{
			ForwardIterator currentMin = first;

			while(++first != last)
			{
				if(compare(*first, *currentMin))
					currentMin = first;
			}
			return currentMin;
		}
		return first;
	}


	/// max_element
	///
	/// max_element finds the largest element in the range [first, last).
	/// It returns the first iterator i in [first, last) such that no other
	/// iterator in [first, last) points to a value greater than *i.
	/// The return value is last if and only if [first, last) is an empty range.
	///
	/// Returns: The first iterator i in the range [first, last) such that
	/// for any iterator j in the range [first, last) the following corresponding
	/// condition holds: !(*i < *j).
	///
	/// Complexity: Exactly 'max((last - first) - 1, 0)' applications of the
	/// corresponding comparisons.
	///
	template <typename ForwardIterator>
	ForwardIterator max_element(ForwardIterator first, ForwardIterator last)
	{
		if(first != last)
		{
			ForwardIterator currentMax = first;

			while(++first != last)
			{
				if(*currentMax < *first)
					currentMax = first;
			}
			return currentMax;
		}
		return first;
	}


	/// max_element
	///
	/// max_element finds the largest element in the range [first, last).
	/// It returns the first iterator i in [first, last) such that no other
	/// iterator in [first, last) points to a value greater than *i.
	/// The return value is last if and only if [first, last) is an empty range.
	///
	/// Returns: The first iterator i in the range [first, last) such that
	/// for any iterator j in the range [first, last) the following corresponding
	/// condition holds: compare(*i, *j) == false.
	///
	/// Complexity: Exactly 'max((last - first) - 1, 0)' applications of the
	/// corresponding comparisons.
	///
	template <typename ForwardIterator, typename Compare>
	ForwardIterator max_element(ForwardIterator first, ForwardIterator last, Compare compare)
	{
		if(first != last)
		{
			ForwardIterator currentMax = first;

			while(++first != last)
			{
				if(compare(*currentMax, *first))
					currentMax = first;
			}
			return currentMax;
		}
		return first;
	}


	#if EASTL_MINMAX_ENABLED

		/// min
		///
		/// Min returns the lesser of its two arguments; it returns the first
		/// argument if neither is less than the other. The two arguments are
		/// compared with operator <.
		///
		/// This min and our other min implementations are defined as returning:
		///     b < a ? b : a
		/// which for example may in practice result in something different than:
		///     b <= a ? b : a
		/// in the case where b is different from a (though they compare as equal).
		/// We choose the specific ordering here because that's the ordering
		/// done by other STL implementations.
		///
		/// Some compilers (e.g. VS20003 - VS2013) generate poor code for the case of
		/// scalars returned by reference, so we provide a specialization for those cases.
		/// The specialization returns T by value instead of reference, which is
		/// not that the Standard specifies. The Standard allows you to use
		/// an expression like &max(x, y), which would be impossible in this case.
		/// However, we have found no actual code that uses min or max like this and
		/// this specialization causes no problems in practice. Microsoft has acknowledged
		/// the problem and may fix it for a future VS version.
		///
		template <typename T>
		inline EA_CONSTEXPR typename eastl::enable_if<eastl::is_scalar<T>::value, T>::type
		min(T a, T b)
		{
			return b < a ? b : a;
		}

		template <typename T>
		inline EA_CONSTEXPR typename eastl::enable_if<!eastl::is_scalar<T>::value, const T&>::type
		min(const T& a, const T& b)
		{
			return b < a ? b : a;
		}

		inline EA_CONSTEXPR float       min(float  a, float   b)           { return b < a ? b : a; }
		inline EA_CONSTEXPR double      min(double a, double  b)           { return b < a ? b : a; }
		inline EA_CONSTEXPR long double min(long double a, long double  b) { return b < a ? b : a; }

	#endif // EASTL_MINMAX_ENABLED


	/// min_alt
	///
	/// This is an alternative version of min that avoids any possible
	/// collisions with Microsoft #defines of min and max.
	///
	/// See min(a, b) for detailed specifications.
	///
	template <typename T>
	inline EA_CONSTEXPR typename eastl::enable_if<eastl::is_scalar<T>::value, T>::type
	min_alt(T a, T b)
	{
		return b < a ? b : a;
	}

	template <typename T>
	inline typename eastl::enable_if<!eastl::is_scalar<T>::value, const T&>::type
	min_alt(const T& a, const T& b)
	{
		return b < a ? b : a;
	}

	inline EA_CONSTEXPR float       min_alt(float  a, float   b)           { return b < a ? b : a; }
	inline EA_CONSTEXPR double      min_alt(double a, double  b)           { return b < a ? b : a; }
	inline EA_CONSTEXPR long double min_alt(long double a, long double  b) { return b < a ? b : a; }


	#if EASTL_MINMAX_ENABLED

		/// min
		///
		/// Min returns the lesser of its two arguments; it returns the first
		/// argument if neither is less than the other. The two arguments are
		/// compared with the Compare function (or function object), which
		/// takes two arguments and returns true if the first is less than
		/// the second.
		///
		/// See min(a, b) for detailed specifications.
		///
		/// Example usage:
		///    struct A{ int a; };
		///    struct Struct{ bool operator()(const A& a1, const A& a2){ return a1.a < a2.a; } };
		///
		///    A a1, a2, a3;
		///    a3 = min(a1, a2, Struct());
		///
		/// Example usage:
		///    struct B{ int b; };
		///    inline bool Function(const B& b1, const B& b2){ return b1.b < b2.b; }
		///
		///    B b1, b2, b3;
		///    b3 = min(b1, b2, Function);
		///
		template <typename T, typename Compare>
		inline const T&
		min(const T& a, const T& b, Compare compare)
		{
			return compare(b, a) ? b : a;
		}

	#endif // EASTL_MINMAX_ENABLED


	/// min_alt
	///
	/// This is an alternative version of min that avoids any possible
	/// collisions with Microsoft #defines of min and max.
	///
	/// See min(a, b) for detailed specifications.
	///
	template <typename T, typename Compare>
	inline const T&
	min_alt(const T& a, const T& b, Compare compare)
	{
		return compare(b, a) ? b : a;
	}


	#if EASTL_MINMAX_ENABLED

		/// max
		///
		/// Max returns the greater of its two arguments; it returns the first
		/// argument if neither is greater than the other. The two arguments are
		/// compared with operator < (and not operator >).
		///
		/// This min and our other min implementations are defined as returning:
		///     a < b ? b : a
		/// which for example may in practice result in something different than:
		///     a <= b ? b : a
		/// in the case where b is different from a (though they compare as equal).
		/// We choose the specific ordering here because that's the ordering
		/// done by other STL implementations.
		///
		template <typename T>
		inline EA_CONSTEXPR typename eastl::enable_if<eastl::is_scalar<T>::value, T>::type
		max(T a, T b)
		{
			return a < b ? b : a;
		}

		template <typename T>
		inline EA_CONSTEXPR typename eastl::enable_if<!eastl::is_scalar<T>::value, const T&>::type
		max(const T& a, const T& b)
		{
			return a < b ? b : a;
		}

		inline EA_CONSTEXPR float       max(float       a, float       b) { return a < b ? b : a; }
		inline EA_CONSTEXPR double      max(double      a, double      b) { return a < b ? b : a; }
		inline EA_CONSTEXPR long double max(long double a, long double b) { return a < b ? b : a; }

	#endif // EASTL_MINMAX_ENABLED


	/// max_alt
	///
	/// This is an alternative version of max that avoids any possible
	/// collisions with Microsoft #defines of min and max.
	///
	template <typename T>
	inline EA_CONSTEXPR typename eastl::enable_if<eastl::is_scalar<T>::value, T>::type
	max_alt(T a, T b)
	{
		return a < b ? b : a;
	}

	template <typename T>
	inline EA_CONSTEXPR typename eastl::enable_if<!eastl::is_scalar<T>::value, const T&>::type
	max_alt(const T& a, const T& b)
	{
		return a < b ? b : a;
	}

	inline EA_CONSTEXPR float       max_alt(float       a, float       b) { return a < b ? b : a; }
	inline EA_CONSTEXPR double      max_alt(double      a, double      b) { return a < b ? b : a; }
	inline EA_CONSTEXPR long double max_alt(long double a, long double b) { return a < b ? b : a; }


	#if EASTL_MINMAX_ENABLED
		/// max
		///
		/// Min returns the lesser of its two arguments; it returns the first
		/// argument if neither is less than the other. The two arguments are
		/// compared with the Compare function (or function object), which
		/// takes two arguments and returns true if the first is less than
		/// the second.
		///
		template <typename T, typename Compare>
		inline const T&
		max(const T& a, const T& b, Compare compare)
		{
			return compare(a, b) ? b : a;
		}
	#endif


	/// max_alt
	///
	/// This is an alternative version of max that avoids any possible
	/// collisions with Microsoft #defines of min and max.
	///
	template <typename T, typename Compare>
	inline const T&
	max_alt(const T& a, const T& b, Compare compare)
	{
		return compare(a, b) ? b : a;
	}


	/// min(std::initializer_list)
	///
	template <typename T >
	T min(std::initializer_list<T> ilist)
	{
		return *eastl::min_element(ilist.begin(), ilist.end());
	}

	/// min(std::initializer_list, Compare)
	///
	template <typename T, typename Compare>
	T min(std::initializer_list<T> ilist, Compare compare)
	{
		return *eastl::min_element(ilist.begin(), ilist.end(), compare);
	}


	/// max(std::initializer_list)
	///
	template <typename T >
	T max(std::initializer_list<T> ilist)
	{
		return *eastl::max_element(ilist.begin(), ilist.end());
	}

	/// max(std::initializer_list, Compare)
	///
	template <typename T, typename Compare>
	T max(std::initializer_list<T> ilist, Compare compare)
	{
		return *eastl::max_element(ilist.begin(), ilist.end(), compare);
	}


	/// minmax_element
	///
	/// Returns: make_pair(first, first) if [first, last) is empty, otherwise make_pair(m, M),
	/// where m is the first iterator in [first,last) such that no iterator in the range
	/// refers to a smaller element, and where M is the last iterator in [first,last) such
	/// that no iterator in the range refers to a larger element.
	///
	/// Complexity: At most max([(3/2)*(N - 1)], 0) applications of the corresponding predicate,
	/// where N is distance(first, last).
	///
	template <typename ForwardIterator, typename Compare>
	eastl::pair<ForwardIterator, ForwardIterator>
	minmax_element(ForwardIterator first, ForwardIterator last, Compare compare)
	{
		eastl::pair<ForwardIterator, ForwardIterator> result(first, first);

		if(!(first == last) && !(++first == last))
		{
			if(compare(*first, *result.first))
			{
				result.second = result.first;
				result.first = first;
			}
			else
				result.second = first;

			while(++first != last)
			{
				ForwardIterator i = first;

				if(++first == last)
				{
					if(compare(*i, *result.first))
						result.first = i;
					else if(!compare(*i, *result.second))
						result.second = i;
					break;
				}
				else
				{
					if(compare(*first, *i))
					{
						if(compare(*first, *result.first))
							result.first = first;

						if(!compare(*i, *result.second))
							result.second = i;
					}
					else
					{
						if(compare(*i, *result.first))
							result.first = i;

						if(!compare(*first, *result.second))
							result.second = first;
					}
				}
			}
		}

		return result;
	}


	template <typename ForwardIterator>
	eastl::pair<ForwardIterator, ForwardIterator>
	minmax_element(ForwardIterator first, ForwardIterator last)
	{
		typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;

		return eastl::minmax_element(first, last, eastl::less<value_type>());
	}



	/// minmax
	///
	/// Requires: Type T shall be LessThanComparable.
	/// Returns: pair<const T&, const T&>(b, a) if b is smaller than a, and pair<const T&, const T&>(a, b) otherwise.
	/// Remarks: Returns pair<const T&, const T&>(a, b) when the arguments are equivalent.
	/// Complexity: Exactly one comparison.
	///

	// The following optimization is a problem because it changes the return value in a way that would break
	// users unless they used auto (e.g. auto result = minmax(17, 33); )
	//
	// template <typename T>
	// inline EA_CONSTEXPR typename eastl::enable_if<eastl::is_scalar<T>::value, eastl::pair<T, T> >::type
	// minmax(T a, T b)
	// {
	//     return (b < a) ? eastl::make_pair(b, a) : eastl::make_pair(a, b);
	// }
	//
	// template <typename T>
	// inline typename eastl::enable_if<!eastl::is_scalar<T>::value, eastl::pair<const T&, const T&> >::type
	// minmax(const T& a, const T& b)
	// {
	//     return (b < a) ? eastl::make_pair(b, a) : eastl::make_pair(a, b);
	// }

	// It turns out that the following conforming definition of minmax generates a warning when used with VC++ up
	// to at least VS2012. The VS2012 version of minmax is a broken and non-conforming definition, and we don't
	// want to do that. We could do it for scalars alone, though we'd have to decide if we are going to do that
	// for all compilers, because it changes the return value from a pair of references to a pair of values.
	template <typename T>
	inline eastl::pair<const T&, const T&>
	minmax(const T& a, const T& b)
	{
		return (b < a) ? eastl::make_pair(b, a) : eastl::make_pair(a, b);
	}


	template <typename T, typename Compare>
	eastl::pair<const T&, const T&>
	minmax(const T& a, const T& b, Compare compare)
	{
		return compare(b, a) ? eastl::make_pair(b, a) : eastl::make_pair(a, b);
	}



	template <typename T>
	eastl::pair<T, T>
	minmax(std::initializer_list<T> ilist)
	{
		typedef typename std::initializer_list<T>::iterator iterator_type;
		eastl::pair<iterator_type, iterator_type> iteratorPair = eastl::minmax_element(ilist.begin(), ilist.end());
		return eastl::make_pair(*iteratorPair.first, *iteratorPair.second);
	}

	template <typename T, class Compare>
	eastl::pair<T, T>
	minmax(std::initializer_list<T> ilist, Compare compare)
	{
		typedef typename std::initializer_list<T>::iterator iterator_type;
		eastl::pair<iterator_type, iterator_type> iteratorPair = eastl::minmax_element(ilist.begin(), ilist.end(), compare);
		return eastl::make_pair(*iteratorPair.first, *iteratorPair.second);
	}

	template <typename T>
	inline T&& median_impl(T&& a, T&& b, T&& c)
	{
		if(a < b)
		{
			if(b < c)
				return eastl::forward<T>(b);
			else if(a < c)
				return eastl::forward<T>(c);
			else
				return eastl::forward<T>(a);
		}
		else if(a < c)
			return eastl::forward<T>(a);
		else if(b < c)
			return eastl::forward<T>(c);
		return eastl::forward<T>(b);
	}

	/// median
	///
	/// median finds which element of three (a, b, d) is in-between the other two.
	/// If two or more elements are equal, the first (e.g. a before b) is chosen.
	///
	/// Complexity: Either two or three comparisons will be required, depending
	/// on the values.
	///
	template <typename T>
	inline const T& median(const T& a, const T& b, const T& c)
	{
		return median_impl(a, b, c);
	}

	/// median
	///
	/// median finds which element of three (a, b, d) is in-between the other two.
	/// If two or more elements are equal, the first (e.g. a before b) is chosen.
	///
	/// Complexity: Either two or three comparisons will be required, depending
	/// on the values.
	///
	template <typename T>
	inline T&& median(T&& a, T&& b, T&& c)
	{
		return eastl::forward<T>(median_impl(eastl::forward<T>(a), eastl::forward<T>(b), eastl::forward<T>(c)));
	}


	template <typename T, typename Compare>
	inline T&& median_impl(T&& a, T&& b, T&& c, Compare compare)
	{
		if(compare(a, b))
		{
			if(compare(b, c))
				return eastl::forward<T>(b);
			else if(compare(a, c))
				return eastl::forward<T>(c);
			else
				return eastl::forward<T>(a);
		}
		else if(compare(a, c))
			return eastl::forward<T>(a);
		else if(compare(b, c))
			return eastl::forward<T>(c);
		return eastl::forward<T>(b);
	}


	/// median
	///
	/// median finds which element of three (a, b, d) is in-between the other two.
	/// If two or more elements are equal, the first (e.g. a before b) is chosen.
	///
	/// Complexity: Either two or three comparisons will be required, depending
	/// on the values.
	///
	template <typename T, typename Compare>
	inline const T& median(const T& a, const T& b, const T& c, Compare compare)
	{
		return median_impl<const T&, Compare>(a, b, c, compare);
	}

	/// median
	///
	/// median finds which element of three (a, b, d) is in-between the other two.
	/// If two or more elements are equal, the first (e.g. a before b) is chosen.
	///
	/// Complexity: Either two or three comparisons will be required, depending
	/// on the values.
	///
	template <typename T, typename Compare>
	inline T&& median(T&& a, T&& b, T&& c, Compare compare)
	{
		return eastl::forward<T>(median_impl<T&&, Compare>(eastl::forward<T>(a), eastl::forward<T>(b), eastl::forward<T>(c), compare));
	}




	/// all_of
	///
	/// Returns: true if the unary predicate p returns true for all elements in the range [first, last)
	///
	template <typename InputIterator, typename Predicate>
	inline bool all_of(InputIterator first, InputIterator last, Predicate p)
	{
		for(; first != last; ++first)
		{
			if(!p(*first))
				return false;
		}
		return true;
	}


	/// any_of
	///
	/// Returns: true if the unary predicate p returns true for any of the elements in the range [first, last)
	///
	template <typename InputIterator, typename Predicate>
	inline bool any_of(InputIterator first, InputIterator last, Predicate p)
	{
		for(; first != last; ++first)
		{
			if(p(*first))
				return true;
		}
		return false;
	}


	/// none_of
	///
	/// Returns: true if the unary predicate p returns true for none of the elements in the range [first, last)
	///
	template <typename InputIterator, typename Predicate>
	inline bool none_of(InputIterator first, InputIterator last, Predicate p)
	{
		for(; first != last; ++first)
		{
			if(p(*first))
				return false;
		}
		return true;
	}


	/// adjacent_find
	///
	/// Returns: The first iterator i such that both i and i + 1 are in the range
	/// [first, last) for which the following corresponding conditions hold: *i == *(i + 1).
	/// Returns last if no such iterator is found.
	///
	/// Complexity: Exactly 'find(first, last, value) - first' applications of the corresponding predicate.
	///
	template <typename ForwardIterator>
	inline ForwardIterator
	adjacent_find(ForwardIterator first, ForwardIterator last)
	{
		if(first != last)
		{
			ForwardIterator i = first;

			for(++i; i != last; ++i)
			{
				if(*first == *i)
					return first;
				first = i;
			}
		}
		return last;
	}



	/// adjacent_find
	///
	/// Returns: The first iterator i such that both i and i + 1 are in the range
	/// [first, last) for which the following corresponding conditions hold: predicate(*i, *(i + 1)) != false.
	/// Returns last if no such iterator is found.
	///
	/// Complexity: Exactly 'find(first, last, value) - first' applications of the corresponding predicate.
	///
	template <typename ForwardIterator, typename BinaryPredicate>
	inline ForwardIterator
	adjacent_find(ForwardIterator first, ForwardIterator last, BinaryPredicate predicate)
	{
		if(first != last)
		{
			ForwardIterator i = first;

			for(++i; i != last; ++i)
			{
				if(predicate(*first, *i))
					return first;
				first = i;
			}
		}
		return last;
	}


	/// shuffle
	///
	/// New for C++11
	/// Randomizes a sequence of values via a user-supplied UniformRandomNumberGenerator.
	/// The difference between this and the original random_shuffle function is that this uses the more
	/// advanced and flexible UniformRandomNumberGenerator interface as opposed to the more
	/// limited RandomNumberGenerator interface of random_shuffle.
	///
	/// Effects: Shuffles the elements in the range [first, last) with uniform distribution.
	///
	/// Complexity: Exactly '(last - first) - 1' swaps.
	///
	/// Example usage:
	///     struct Rand{ eastl_size_t operator()(eastl_size_t n) { return (eastl_size_t)(rand() % n); } }; // Note: The C rand function is poor and slow.
	///     Rand randInstance;
	///     shuffle(pArrayBegin, pArrayEnd, randInstance);
	///
	// See the C++11 Standard, 26.5.1.3, Uniform random number generator requirements.
	// Also http://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution

	template <typename RandomAccessIterator, typename UniformRandomNumberGenerator>
	void shuffle(RandomAccessIterator first, RandomAccessIterator last, UniformRandomNumberGenerator&& urng)
	{
		if(first != last)
		{
			typedef typename eastl::iterator_traits<RandomAccessIterator>::difference_type difference_type;
			typedef typename eastl::make_unsigned<difference_type>::type                   unsigned_difference_type;
			typedef typename eastl::uniform_int_distribution<unsigned_difference_type>     uniform_int_distribution;
			typedef typename uniform_int_distribution::param_type                          uniform_int_distribution_param_type;

			uniform_int_distribution uid;

			for(RandomAccessIterator i = first + 1; i != last; ++i)
				iter_swap(i, first + uid(urng, uniform_int_distribution_param_type(0, i - first)));
		}
	}


	/// random_shuffle
	///
	/// Randomizes a sequence of values.
	///
	/// Effects: Shuffles the elements in the range [first, last) with uniform distribution.
	///
	/// Complexity: Exactly '(last - first) - 1' swaps.
	///
	/// Example usage:
	///     eastl_size_t Rand(eastl_size_t n) { return (eastl_size_t)(rand() % n); } // Note: The C rand function is poor and slow.
	///     pointer_to_unary_function<eastl_size_t, eastl_size_t> randInstance(Rand);
	///     random_shuffle(pArrayBegin, pArrayEnd, randInstance);
	///
	/// Example usage:
	///     struct Rand{ eastl_size_t operator()(eastl_size_t n) { return (eastl_size_t)(rand() % n); } }; // Note: The C rand function is poor and slow.
	///     Rand randInstance;
	///     random_shuffle(pArrayBegin, pArrayEnd, randInstance);
	///
	template <typename RandomAccessIterator, typename RandomNumberGenerator>
	inline void random_shuffle(RandomAccessIterator first, RandomAccessIterator last, RandomNumberGenerator&& rng)
	{
		typedef typename eastl::iterator_traits<RandomAccessIterator>::difference_type difference_type;

		// We must do 'rand((i - first) + 1)' here and cannot do 'rand(last - first)',
		// as it turns out that the latter results in unequal distribution probabilities.
		// http://www.cigital.com/papers/download/developer_gambling.php

		const difference_type swapMax = eastl::distance(first, last);

		// deliberately start at 1.
		for (difference_type swapIter = 1; swapIter < swapMax; ++swapIter)
		{
			RandomAccessIterator i = first + swapIter;
			iter_swap(i, first + (difference_type)rng((eastl_size_t)((i - first) + 1)));
		}
	}


	/// random_shuffle
	///
	/// Randomizes a sequence of values.
	///
	/// Effects: Shuffles the elements in the range [first, last) with uniform distribution.
	///
	/// Complexity: Exactly '(last - first) - 1' swaps.
	///
	/// Example usage:
	///     random_shuffle(pArrayBegin, pArrayEnd);
	///
	/// *** Disabled until we decide if we want to get into the business of writing random number generators. ***
	///
	/// template <typename RandomAccessIterator>
	/// inline void random_shuffle(RandomAccessIterator first, RandomAccessIterator last)
	/// {
	///     for(RandomAccessIterator i = first + 1; i < last; ++i)
	///         iter_swap(i, first + SomeRangedRandomNumberGenerator((i - first) + 1));
	/// }






	/// move_n
	///
	/// Same as move(InputIterator, InputIterator, OutputIterator) except based on count instead of iterator range.
	///
	template <typename InputIterator, typename Size, typename OutputIterator>
	inline OutputIterator
	move_n_impl(InputIterator first, Size n, OutputIterator result, EASTL_ITC_NS::input_iterator_tag)
	{
		for(; n > 0; --n)
			*result++ = eastl::move(*first++);
		return result;
	}

	template <typename RandomAccessIterator, typename Size, typename OutputIterator>
	inline OutputIterator
	move_n_impl(RandomAccessIterator first, Size n, OutputIterator result, EASTL_ITC_NS::random_access_iterator_tag)
	{
		return eastl::move(first, first + n, result); // Take advantage of the optimizations present in the move algorithm.
	}


	template <typename InputIterator, typename Size, typename OutputIterator>
	inline OutputIterator
	move_n(InputIterator first, Size n, OutputIterator result)
	{
		typedef typename eastl::iterator_traits<InputIterator>::iterator_category IC;
		return eastl::move_n_impl(first, n, result, IC());
	}



	/// copy_n
	///
	/// Same as copy(InputIterator, InputIterator, OutputIterator) except based on count instead of iterator range.
	/// Effects: Copies exactly count values from the range beginning at first to the range beginning at result, if count > 0. Does nothing otherwise.
	/// Returns: Iterator in the destination range, pointing past the last element copied if count>0 or first otherwise.
	/// Complexity: Exactly count assignments, if count > 0.
	///
	template <typename InputIterator, typename Size, typename OutputIterator>
	inline OutputIterator
	copy_n_impl(InputIterator first, Size n, OutputIterator result, EASTL_ITC_NS::input_iterator_tag)
	{
		for(; n > 0; --n)
			*result++ = *first++;
		return result;
	}

	template <typename RandomAccessIterator, typename Size, typename OutputIterator>
	inline OutputIterator
	copy_n_impl(RandomAccessIterator first, Size n, OutputIterator result, EASTL_ITC_NS::random_access_iterator_tag)
	{
		return eastl::copy(first, first + n, result); // Take advantage of the optimizations present in the copy algorithm.
	}


	template <typename InputIterator, typename Size, typename OutputIterator>
	inline OutputIterator
	copy_n(InputIterator first, Size n, OutputIterator result)
	{
		typedef typename eastl::iterator_traits<InputIterator>::iterator_category IC;
		return eastl::copy_n_impl(first, n, result, IC());
	}


	/// copy_if
	///
	/// Effects: Assigns to the result iterator only if the predicate is true.
	///
	template <typename InputIterator, typename OutputIterator, typename Predicate>
	inline OutputIterator
	copy_if(InputIterator first, InputIterator last, OutputIterator result, Predicate predicate)
	{
		// This implementation's performance could be improved by taking a more complicated approach like with the copy algorithm.
		for(; first != last; ++first)
		{
			if(predicate(*first))
				*result++ = *first;
		}

		return result;
	}




	// Implementation moving copying both trivial and non-trivial data via a lesser iterator than random-access.
	template <typename /*BidirectionalIterator1Category*/, bool /*isMove*/, bool /*canMemmove*/>
	struct move_and_copy_backward_helper
	{
		template <typename BidirectionalIterator1, typename BidirectionalIterator2>
		static BidirectionalIterator2 move_or_copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 resultEnd)
		{
			while(first != last)
				*--resultEnd = *--last;
			return resultEnd; // resultEnd now points to the beginning of the destination sequence instead of the end.
		}
	};

	// Specialization for moving non-trivial data via a lesser iterator than random-access.
	template <typename BidirectionalIterator1Category>
	struct move_and_copy_backward_helper<BidirectionalIterator1Category, true, false>
	{
		template <typename BidirectionalIterator1, typename BidirectionalIterator2>
		static BidirectionalIterator2 move_or_copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 resultEnd)
		{
			while(first != last)
				*--resultEnd = eastl::move(*--last);
			return resultEnd; // resultEnd now points to the beginning of the destination sequence instead of the end.
		}
	};

	// Specialization for moving non-trivial data via a random-access iterator. It's theoretically faster because the compiler can see the count when its a compile-time const.
	template<>
	struct move_and_copy_backward_helper<EASTL_ITC_NS::random_access_iterator_tag, true, false>
	{
		template<typename BidirectionalIterator1, typename BidirectionalIterator2>
		static BidirectionalIterator2 move_or_copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 resultEnd)
		{
			typedef typename eastl::iterator_traits<BidirectionalIterator1>::difference_type difference_type;

			for(difference_type n = (last - first); n > 0; --n)
				*--resultEnd = eastl::move(*--last);
			return resultEnd; // resultEnd now points to the beginning of the destination sequence instead of the end.
		}
	};

	// Specialization for copying non-trivial data via a random-access iterator. It's theoretically faster because the compiler can see the count when its a compile-time const.
	// This specialization converts the random access BidirectionalIterator1 last-first to an integral type. There's simple way for us to take advantage of a random access output iterator,
	// as the range is specified by the input instead of the output, and distance(first, last) for a non-random-access iterator is potentially slow.
	template <>
	struct move_and_copy_backward_helper<EASTL_ITC_NS::random_access_iterator_tag, false, false>
	{
		template <typename BidirectionalIterator1, typename BidirectionalIterator2>
		static BidirectionalIterator2 move_or_copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 resultEnd)
		{
			typedef typename eastl::iterator_traits<BidirectionalIterator1>::difference_type difference_type;

			for(difference_type n = (last - first); n > 0; --n)
				*--resultEnd = *--last;
			return resultEnd; // resultEnd now points to the beginning of the destination sequence instead of the end.
		}
	};

	// Specialization for when we can use memmove/memcpy. See the notes above for what conditions allow this.
	template <bool isMove>
	struct move_and_copy_backward_helper<EASTL_ITC_NS::random_access_iterator_tag, isMove, true>
	{
		template <typename T>
		static T* move_or_copy_backward(const T* first, const T* last, T* resultEnd)
		{
			const size_t n = (size_t)((uintptr_t)last - (uintptr_t)first);
			// We could use memcpy here if there's no range overlap, but memcpy is rarely much faster than memmove.
			if (n > 0)
				return (T*)memmove(resultEnd - (last - first), first, n);
			else
				return resultEnd;
		}
	};

	template <bool isMove, typename BidirectionalIterator1, typename BidirectionalIterator2>
	inline BidirectionalIterator2 move_and_copy_backward_chooser(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 resultEnd)
	{
		typedef typename eastl::iterator_traits<BidirectionalIterator1>::iterator_category IIC;

		const bool canBeMemmoved = internal::can_be_memmoved_helper<BidirectionalIterator1, BidirectionalIterator2>::value;

		return eastl::move_and_copy_backward_helper<IIC, isMove, canBeMemmoved>::move_or_copy_backward(first, last, resultEnd); // Need to chose based on the input iterator tag and not the output iterator tag, because containers accept input ranges of iterator types different than self.
	}


	template <bool isMove, typename BidirectionalIterator1, typename BidirectionalIterator2>
	EASTL_REMOVE_AT_2024_SEPT inline BidirectionalIterator2 move_and_copy_backward_unwrapper(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 resultEnd)
	{
		EASTL_INTERNAL_DISABLE_DEPRECATED() // 'unwrap_iterator': was declared deprecated
		return BidirectionalIterator2(eastl::move_and_copy_backward_chooser<isMove>(eastl::unwrap_iterator(first), eastl::unwrap_iterator(last), eastl::unwrap_iterator(resultEnd))); // Have to convert to BidirectionalIterator2 because result.base() could be a T*
		EASTL_INTERNAL_RESTORE_DEPRECATED()
	}


	/// move_backward
	///
	/// The elements are moved in reverse order (the last element is moved first), but their relative order is preserved.
	/// After this operation the elements in the moved-from range will still contain valid values of the
	/// appropriate type, but not necessarily the same values as before the move.
	/// Returns the beginning of the result range.
	/// Note: When moving between containers, the dest range must be valid; this function doesn't resize containers.
	/// Note: If result is within [first, last), move must be used instead of move_backward.
	///
	/// Example usage:
	///     eastl::move_backward(myArray.begin(), myArray.end(), myDestArray.end());
	///
	/// Reference implementation:
	///     template <typename BidirectionalIterator1, typename BidirectionalIterator2>
	///     BidirectionalIterator2 move_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 resultEnd)
	///     {
	///         while(last != first)
	///             *--resultEnd = eastl::move(*--last);
	///         return resultEnd;
	///     }
	///
	template <typename BidirectionalIterator1, typename BidirectionalIterator2>
	inline BidirectionalIterator2 move_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 resultEnd)
	{
		return eastl::move_and_copy_backward_chooser<true>(first, last, resultEnd);
	}


	/// copy_backward
	///
	/// copies memory in the range of [first, last) to the range *ending* with result.
	///
	/// Effects: Copies elements in the range [first, last) into the range
	/// [result - (last - first), result) starting from last 1 and proceeding to first.
	/// For each positive integer n <= (last - first), performs *(result n) = *(last - n).
	///
	/// Requires: result shall not be in the range [first, last).
	///
	/// Returns: result - (last - first). That is, returns the beginning of the result range.
	///
	/// Complexity: Exactly 'last - first' assignments.
	///
	template <typename BidirectionalIterator1, typename BidirectionalIterator2>
	inline BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 resultEnd)
	{
		return eastl::move_and_copy_backward_chooser<false>(first, last, resultEnd);
	}


	/// count
	///
	/// Counts the number of items in the range of [first, last) which equal the input value.
	///
	/// Effects: Returns the number of iterators i in the range [first, last) for which the
	/// following corresponding conditions hold: *i == value.
	///
	/// Complexity: At most 'last - first' applications of the corresponding predicate.
	///
	/// Note: The predicate version of count is count_if and not another variation of count.
	/// This is because both versions would have three parameters and there could be ambiguity.
	///
	template <typename InputIterator, typename T>
	inline typename eastl::iterator_traits<InputIterator>::difference_type
	count(InputIterator first, InputIterator last, const T& value)
	{
		typename eastl::iterator_traits<InputIterator>::difference_type result = 0;

		for(; first != last; ++first)
		{
			if(*first == value)
				++result;
		}
		return result;
	}


	// C++ doesn't define a count with predicate, as it can effectively be synthesized via count_if
	// with an appropriate predicate. However, it's often simpler to just have count with a predicate.
	template <typename InputIterator, typename T, typename Predicate>
	inline typename eastl::iterator_traits<InputIterator>::difference_type
	count(InputIterator first, InputIterator last, const T& value, Predicate predicate)
	{
		typename eastl::iterator_traits<InputIterator>::difference_type result = 0;

		for(; first != last; ++first)
		{
			if(predicate(*first, value))
				++result;
		}
		return result;
	}


	/// count_if
	///
	/// Counts the number of items in the range of [first, last) which match
	/// the input value as defined by the input predicate function.
	///
	/// Effects: Returns the number of iterators i in the range [first, last) for which the
	/// following corresponding conditions hold: predicate(*i) != false.
	///
	/// Complexity: At most 'last - first' applications of the corresponding predicate.
	///
	/// Note: The non-predicate version of count_if is count and not another variation of count_if.
	/// This is because both versions would have three parameters and there could be ambiguity.
	///
	template <typename InputIterator, typename Predicate>
	inline typename eastl::iterator_traits<InputIterator>::difference_type
	count_if(InputIterator first, InputIterator last, Predicate predicate)
	{
		typename eastl::iterator_traits<InputIterator>::difference_type result = 0;

		for(; first != last; ++first)
		{
			if(predicate(*first))
				++result;
		}
		return result;
	}


	/// find
	///
	/// finds the value within the unsorted range of [first, last).
	///
	/// Returns: The first iterator i in the range [first, last) for which
	/// the following corresponding conditions hold: *i == value.
	/// Returns last if no such iterator is found.
	///
	/// Complexity: At most 'last - first' applications of the corresponding predicate.
	/// This is a linear search and not a binary one.
	///
	/// Note: The predicate version of find is find_if and not another variation of find.
	/// This is because both versions would have three parameters and there could be ambiguity.
	///
	template <typename InputIterator, typename T>
	inline InputIterator
	find(InputIterator first, InputIterator last, const T& value)
	{
		while((first != last) && !(*first == value)) // Note that we always express value comparisons in terms of < or ==.
			++first;
		return first;
	}


	// C++ doesn't define a find with predicate, as it can effectively be synthesized via find_if
	// with an appropriate predicate. However, it's often simpler to just have find with a predicate.
	template <typename InputIterator, typename T, typename Predicate>
	inline InputIterator
	find(InputIterator first, InputIterator last, const T& value, Predicate predicate)
	{
		while((first != last) && !predicate(*first, value))
			++first;
		return first;
	}



	/// find_if
	///
	/// finds the value within the unsorted range of [first, last).
	///
	/// Returns: The first iterator i in the range [first, last) for which
	/// the following corresponding conditions hold: pred(*i) != false.
	/// Returns last if no such iterator is found.
	/// If the sequence of elements to search for (i.e. first2 - last2) is empty,
	/// the find always fails and last1 will be returned.
	///
	/// Complexity: At most 'last - first' applications of the corresponding predicate.
	///
	/// Note: The non-predicate version of find_if is find and not another variation of find_if.
	/// This is because both versions would have three parameters and there could be ambiguity.
	///
	template <typename InputIterator, typename Predicate>
	inline InputIterator
	find_if(InputIterator first, InputIterator last, Predicate predicate)
	{
		while((first != last) && !predicate(*first))
			++first;
		return first;
	}



	/// find_if_not
	///
	/// find_if_not works the same as find_if except it tests for if the predicate
	/// returns false for the elements instead of true.
	///
	template <typename InputIterator, typename Predicate>
	inline InputIterator
	find_if_not(InputIterator first, InputIterator last, Predicate predicate)
	{
		for(; first != last; ++first)
		{
			if(!predicate(*first))
				return first;
		}
		return last;
	}




	/// find_first_of
	///
	/// find_first_of is similar to find in that it performs linear search through
	/// a range of ForwardIterators. The difference is that while find searches
	/// for one particular value, find_first_of searches for any of several values.
	/// Specifically, find_first_of searches for the first occurrance in the
	/// range [first1, last1) of any of the elements in [first2, last2).
	/// This function is thus similar to the strpbrk standard C string function.
	/// If the sequence of elements to search for (i.e. first2-last2) is empty,
	/// the find always fails and last1 will be returned.
	///
	/// Effects: Finds an element that matches one of a set of values.
	///
	/// Returns: The first iterator i in the range [first1, last1) such that for some
	/// integer j in the range [first2, last2) the following conditions hold: *i == *j.
	/// Returns last1 if no such iterator is found.
	///
	/// Complexity: At most '(last1 - first1) * (last2 - first2)' applications of the
	/// corresponding predicate.
	///
	template <typename ForwardIterator1, typename ForwardIterator2>
	ForwardIterator1
	find_first_of(ForwardIterator1 first1, ForwardIterator1 last1,
				  ForwardIterator2 first2, ForwardIterator2 last2)
	{
		for(; first1 != last1; ++first1)
		{
			for(ForwardIterator2 i = first2; i != last2; ++i)
			{
				if(*first1 == *i)
					return first1;
			}
		}
		return last1;
	}


	/// find_first_of
	///
	/// find_first_of is similar to find in that it performs linear search through
	/// a range of ForwardIterators. The difference is that while find searches
	/// for one particular value, find_first_of searches for any of several values.
	/// Specifically, find_first_of searches for the first occurrance in the
	/// range [first1, last1) of any of the elements in [first2, last2).
	/// This function is thus similar to the strpbrk standard C string function.
	///
	/// Effects: Finds an element that matches one of a set of values.
	///
	/// Returns: The first iterator i in the range [first1, last1) such that for some
	/// integer j in the range [first2, last2) the following conditions hold: pred(*i, *j) != false.
	/// Returns last1 if no such iterator is found.
	///
	/// Complexity: At most '(last1 - first1) * (last2 - first2)' applications of the
	/// corresponding predicate.
	///
	template <typename ForwardIterator1, typename ForwardIterator2, typename BinaryPredicate>
	ForwardIterator1
	find_first_of(ForwardIterator1 first1, ForwardIterator1 last1,
				  ForwardIterator2 first2, ForwardIterator2 last2,
				  BinaryPredicate predicate)
	{
		for(; first1 != last1; ++first1)
		{
			for(ForwardIterator2 i = first2; i != last2; ++i)
			{
				if(predicate(*first1, *i))
					return first1;
			}
		}
		return last1;
	}


	/// find_first_not_of
	///
	/// Searches through first range for the first element that does not belong the second input range.
	/// This is very much like the C++ string find_first_not_of function.
	///
	/// Returns: The first iterator i in the range [first1, last1) such that for some
	/// integer j in the range [first2, last2) the following conditions hold: !(*i == *j).
	/// Returns last1 if no such iterator is found.
	///
	/// Complexity: At most '(last1 - first1) * (last2 - first2)' applications of the
	/// corresponding predicate.
	///
	template <class ForwardIterator1, class ForwardIterator2>
	ForwardIterator1
	find_first_not_of(ForwardIterator1 first1, ForwardIterator1 last1,
					  ForwardIterator2 first2, ForwardIterator2 last2)
	{
		for(; first1 != last1; ++first1)
		{
			if(eastl::find(first2, last2, *first1) == last2)
				break;
		}

		return first1;
	}



	/// find_first_not_of
	///
	/// Searches through first range for the first element that does not belong the second input range.
	/// This is very much like the C++ string find_first_not_of function.
	///
	/// Returns: The first iterator i in the range [first1, last1) such that for some
	/// integer j in the range [first2, last2) the following conditions hold: pred(*i, *j) == false.
	/// Returns last1 if no such iterator is found.
	///
	/// Complexity: At most '(last1 - first1) * (last2 - first2)' applications of the
	/// corresponding predicate.
	///
	template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
	inline ForwardIterator1
	find_first_not_of(ForwardIterator1 first1, ForwardIterator1 last1,
					  ForwardIterator2 first2, ForwardIterator2 last2,
					  BinaryPredicate predicate)
	{
		typedef typename eastl::iterator_traits<ForwardIterator1>::value_type value_type;

		for(; first1 != last1; ++first1)
		{
			if(eastl::find_if(first2, last2, [&predicate, first1](value_type& rhs) { return predicate(*first1, rhs); }) == last2)
				break;
		}

		return first1;
	}


	template <class BidirectionalIterator1, class ForwardIterator2>
	inline BidirectionalIterator1
	find_last_of(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
				 ForwardIterator2 first2, ForwardIterator2 last2)
	{
		if((first1 != last1) && (first2 != last2))
		{
			BidirectionalIterator1 it1(last1);

			while((--it1 != first1) && (eastl::find(first2, last2, *it1) == last2))
				; // Do nothing

			if((it1 != first1) || (eastl::find(first2, last2, *it1) != last2))
				return it1;
		}

		return last1;
	}


	template <class BidirectionalIterator1, class ForwardIterator2, class BinaryPredicate>
	BidirectionalIterator1
	find_last_of(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
				 ForwardIterator2 first2, ForwardIterator2 last2,
				 BinaryPredicate predicate)
	{
		typedef typename eastl::iterator_traits<BidirectionalIterator1>::value_type value_type;

		if((first1 != last1) && (first2 != last2))
		{
			BidirectionalIterator1 it1(last1);

			while((--it1 != first1) && (eastl::find_if(first2, last2, [&predicate, it1](value_type& rhs) { return predicate(*it1, rhs); }) == last2))
				; // Do nothing

			if((it1 != first1) || (eastl::find_if(first2, last2, [&predicate, it1](value_type& rhs) { return predicate(*it1, rhs); }) != last2))
				return it1;
		}

		return last1;
	}


	template <class BidirectionalIterator1, class ForwardIterator2>
	inline BidirectionalIterator1
	find_last_not_of(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
					 ForwardIterator2 first2, ForwardIterator2 last2)
	{
		if((first1 != last1) && (first2 != last2))
		{
			BidirectionalIterator1 it1(last1);

			while((--it1 != first1) && (eastl::find(first2, last2, *it1) != last2))
				; // Do nothing

			if((it1 != first1) || (eastl::find( first2, last2, *it1) == last2))
				return it1;
		}

		return last1;
	}


	template <class BidirectionalIterator1, class ForwardIterator2, class BinaryPredicate>
	inline BidirectionalIterator1
	find_last_not_of(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
					 ForwardIterator2 first2, ForwardIterator2 last2,
					 BinaryPredicate predicate)
	{
		typedef typename eastl::iterator_traits<BidirectionalIterator1>::value_type value_type;

		if((first1 != last1) && (first2 != last2))
		{
			BidirectionalIterator1 it1(last1);

			while((--it1 != first1) && (eastl::find_if(first2, last2, [&predicate, it1](value_type& rhs) { return predicate(*it1, rhs); }) != last2))
				; // Do nothing

			if((it1 != first1) || (eastl::find_if(first2, last2, [&predicate, it1](value_type& rhs) { return predicate(*it1, rhs); })) != last2)
				return it1;
		}

		return last1;
	}




	/// for_each
	///
	/// Calls the Function function for each value in the range [first, last).
	/// Function takes a single parameter: the current value.
	///
	/// Effects: Applies function to the result of dereferencing every iterator in
	/// the range [first, last), starting from first and proceeding to last 1.
	///
	/// Returns: function.
	///
	/// Complexity: Applies function exactly 'last - first' times.
	///
	/// Note: If function returns a result, the result is ignored.
	///
	template <typename InputIterator, typename Function>
	inline Function
	for_each(InputIterator first, InputIterator last, Function function)
	{
		for(; first != last; ++first)
			function(*first);
		return function;
	}

	/// for_each_n
	///
	/// Calls the Function function for each value in the range [first, first + n).
	/// Function takes a single parameter: the current value.
	///
	/// Effects: Applies function to the result of dereferencing every iterator in
	/// the range [first, first + n), starting from first and proceeding to last 1.
	///
	/// Returns: first + n.
	///
	/// Complexity: Applies function exactly 'first + n' times.
	///
	/// Note:
	////  * If function returns a result, the result is ignored.
	////  * If n < 0, behaviour is undefined.
	///
	template <typename InputIterator, typename Size, typename Function>
	EA_CPP14_CONSTEXPR inline InputIterator
	for_each_n(InputIterator first, Size n, Function function)
	{
		for (Size i = 0; i < n; ++first, i++)
			function(*first);
		return first;
	}


	/// generate
	///
	/// Iterates the range of [first, last) and assigns to each element the
	/// result of the function generator. Generator is a function which takes
	/// no arguments.
	///
	/// Complexity: Exactly 'last - first' invocations of generator and assignments.
	///
	template <typename ForwardIterator, typename Generator>
	inline void
	generate(ForwardIterator first, ForwardIterator last, Generator generator)
	{
		for(; first != last; ++first) // We cannot call generate_n(first, last-first, generator)
			*first = generator();     // because the 'last-first' might not be supported by the
	}                                 // given iterator.


	/// generate_n
	///
	/// Iterates an interator n times and assigns the result of generator
	/// to each succeeding element. Generator is a function which takes
	/// no arguments.
	///
	/// Complexity: Exactly n invocations of generator and assignments.
	///
	template <typename OutputIterator, typename Size, typename Generator>
	inline OutputIterator
	generate_n(OutputIterator first, Size n, Generator generator)
	{
		for(; n > 0; --n, ++first)
			*first = generator();
		return first;
	}


	/// transform
	///
	/// Iterates the input range of [first, last) and the output iterator result
	/// and assigns the result of unaryOperation(input) to result.
	///
	/// Effects: Assigns through every iterator i in the range [result, result + (last1 - first1))
	/// a new corresponding value equal to unaryOperation(*(first1 + (i - result)).
	///
	/// Requires: op shall not have any side effects.
	///
	/// Returns: result + (last1 - first1). That is, returns the end of the output range.
	///
	/// Complexity: Exactly 'last1 - first1' applications of unaryOperation.
	///
	/// Note: result may be equal to first.
	///
	template <typename InputIterator, typename OutputIterator, typename UnaryOperation>
	inline OutputIterator
	transform(InputIterator first, InputIterator last, OutputIterator result, UnaryOperation unaryOperation)
	{
		for(; first != last; ++first, ++result)
			*result = unaryOperation(*first);
		return result;
	}


	/// transform
	///
	/// Iterates the input range of [first, last) and the output iterator result
	/// and assigns the result of binaryOperation(input1, input2) to result.
	///
	/// Effects: Assigns through every iterator i in the range [result, result + (last1 - first1))
	/// a new corresponding value equal to binaryOperation(*(first1 + (i - result), *(first2 + (i - result))).
	///
	/// Requires: binaryOperation shall not have any side effects.
	///
	/// Returns: result + (last1 - first1). That is, returns the end of the output range.
	///
	/// Complexity: Exactly 'last1 - first1' applications of binaryOperation.
	///
	/// Note: result may be equal to first1 or first2.
	///
	template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename BinaryOperation>
	inline OutputIterator
	transform(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, OutputIterator result, BinaryOperation binaryOperation)
	{
		for(; first1 != last1; ++first1, ++first2, ++result)
			*result = binaryOperation(*first1, *first2);
		return result;
	}


	/// equal
	///
	/// Returns: true if for every iterator i in the range [first1, last1) the
	/// following corresponding conditions hold: predicate(*i, *(first2 + (i - first1))) != false.
	/// Otherwise, returns false.
	///
	/// Complexity: At most last1 first1 applications of the corresponding predicate.
	///
	/// To consider: Make specializations of this for scalar types and random access
	/// iterators that uses memcmp or some trick memory comparison function.
	/// We should verify that such a thing results in an improvement.
	///
	template <typename InputIterator1, typename InputIterator2>
	EA_CPP14_CONSTEXPR inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
	{
		for(; first1 != last1; ++first1, ++first2)
		{
			if(!(*first1 == *first2)) // Note that we always express value comparisons in terms of < or ==.
				return false;
		}
		return true;
	}

	/* Enable the following if there was shown to be some benefit. A glance and Microsoft VC++ memcmp
		shows that it is not optimized in any way, much less one that would benefit us here.

	inline bool equal(const bool* first1, const bool* last1, const bool* first2)
		{ return (memcmp(first1, first2, (size_t)((uintptr_t)last1 - (uintptr_t)first1)) == 0); }

	inline bool equal(const char* first1, const char* last1, const char* first2)
		{ return (memcmp(first1, first2, (size_t)((uintptr_t)last1 - (uintptr_t)first1)) == 0); }

	inline bool equal(const unsigned char* first1, const unsigned char* last1, const unsigned char* first2)
		{ return (memcmp(first1, first2, (size_t)((uintptr_t)last1 - (uintptr_t)first1)) == 0); }

	inline bool equal(const signed char* first1, const signed char* last1, const signed char* first2)
		{ return (memcmp(first1, first2, (size_t)((uintptr_t)last1 - (uintptr_t)first1)) == 0); }

	#ifndef EA_WCHAR_T_NON_NATIVE
		inline bool equal(const wchar_t* first1, const wchar_t* last1, const wchar_t* first2)
			{ return (memcmp(first1, first2, (size_t)((uintptr_t)last1 - (uintptr_t)first1)) == 0); }
	#endif

	inline bool equal(const int16_t* first1, const int16_t* last1, const int16_t* first2)
		{ return (memcmp(first1, first2, (size_t)((uintptr_t)last1 - (uintptr_t)first1)) == 0); }

	inline bool equal(const uint16_t* first1, const uint16_t* last1, const uint16_t* first2)
		{ return (memcmp(first1, first2, (size_t)((uintptr_t)last1 - (uintptr_t)first1)) == 0); }

	inline bool equal(const int32_t* first1, const int32_t* last1, const int32_t* first2)
		{ return (memcmp(first1, first2, (size_t)((uintptr_t)last1 - (uintptr_t)first1)) == 0); }

	inline bool equal(const uint32_t* first1, const uint32_t* last1, const uint32_t* first2)
		{ return (memcmp(first1, first2, (size_t)((uintptr_t)last1 - (uintptr_t)first1)) == 0); }

	inline bool equal(const int64_t* first1, const int64_t* last1, const int64_t* first2)
		{ return (memcmp(first1, first2, (size_t)((uintptr_t)last1 - (uintptr_t)first1)) == 0); }

	inline bool equal(const uint64_t* first1, const uint64_t* last1, const uint64_t* first2)
		{ return (memcmp(first1, first2, (size_t)((uintptr_t)last1 - (uintptr_t)first1)) == 0); }
	*/



	/// equal
	///
	/// Returns: true if for every iterator i in the range [first1, last1) the
	/// following corresponding conditions hold: pred(*i, *(first2 + (i first1))) != false.
	/// Otherwise, returns false.
	///
	/// Complexity: At most last1 first1 applications of the corresponding predicate.
	///
	template <typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
	inline bool
	equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryPredicate predicate)
	{
		for(; first1 != last1; ++first1, ++first2)
		{
			if(!predicate(*first1, *first2))
				return false;
		}
		return true;
	}



	/// identical
	///
	/// Returns true if the two input ranges are equivalent.
	/// There is a subtle difference between this algorithm and
	/// the 'equal' algorithm. The equal algorithm assumes the
	/// two ranges are of equal length. This algorithm efficiently
	/// compares two ranges for both length equality and for
	/// element equality. There is no other standard algorithm
	/// that can do this.
	///
	/// Returns: true if the sequence of elements defined by the range
	/// [first1, last1) is of the same length as the sequence of
	/// elements defined by the range of [first2, last2) and if
	/// the elements in these ranges are equal as per the
	/// equal algorithm.
	///
	/// Complexity: At most 'min((last1 - first1), (last2 - first2))' applications
	/// of the corresponding comparison.
	///
	template <typename InputIterator1, typename InputIterator2>
	bool identical(InputIterator1 first1, InputIterator1 last1,
				   InputIterator2 first2, InputIterator2 last2)
	{
		while((first1 != last1) && (first2 != last2) && (*first1 == *first2))
		{
			++first1;
			++first2;
		}
		return (first1 == last1) && (first2 == last2);
	}


	/// identical
	///
	template <typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
	bool identical(InputIterator1 first1, InputIterator1 last1,
				   InputIterator2 first2, InputIterator2 last2, BinaryPredicate predicate)
	{
		while((first1 != last1) && (first2 != last2) && predicate(*first1, *first2))
		{
			++first1;
			++first2;
		}
		return (first1 == last1) && (first2 == last2);
	}



	/// lexicographical_compare
	///
	/// Returns: true if the sequence of elements defined by the range
	/// [first1, last1) is lexicographically less than the sequence of
	/// elements defined by the range [first2, last2). Returns false otherwise.
	///
	/// Complexity: At most 'min((last1 - first1), (last2 - first2))' applications
	/// of the corresponding comparison.
	///
	/// Note: If two sequences have the same number of elements and their
	/// corresponding elements are equivalent, then neither sequence is
	/// lexicographically less than the other. If one sequence is a prefix
	/// of the other, then the shorter sequence is lexicographically less
	/// than the longer sequence. Otherwise, the lexicographical comparison
	/// of the sequences yields the same result as the comparison of the first
	/// corresponding pair of elements that are not equivalent.
	///
	template <typename InputIterator1, typename InputIterator2>
	inline bool
	lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
	{
		for(; (first1 != last1) && (first2 != last2); ++first1, ++first2)
		{
			if(*first1 < *first2)
				return true;
			if(*first2 < *first1)
				return false;
		}
		return (first1 == last1) && (first2 != last2);
	}

	inline bool     // Specialization for const char*.
	lexicographical_compare(const char* first1, const char* last1, const char* first2, const char* last2)
	{
		const ptrdiff_t n1(last1 - first1), n2(last2 - first2);
		const size_t n = (size_t)eastl::min_alt(n1, n2);
		if (n == 0) // don't call memcmp with n == 0
			return false;
		const int result = memcmp(first1, first2, n);
		return result ? (result < 0) : (n1 < n2);
	}

	inline bool     // Specialization for char*.
	lexicographical_compare(char* first1, char* last1, char* first2, char* last2)
	{
		const ptrdiff_t n1(last1 - first1), n2(last2 - first2);
		const size_t n = (size_t)eastl::min_alt(n1, n2);
		if (n == 0) // don't call memcmp with n == 0
			return false;
		const int result = memcmp(first1, first2, n);
		return result ? (result < 0) : (n1 < n2);
	}

	inline bool     // Specialization for const unsigned char*.
	lexicographical_compare(const unsigned char* first1, const unsigned char* last1, const unsigned char* first2, const unsigned char* last2)
	{
		const ptrdiff_t n1(last1 - first1), n2(last2 - first2);
		const size_t n = (size_t)eastl::min_alt(n1, n2);
		if (n == 0) // don't call memcmp with n == 0
			return false;
		const int result = memcmp(first1, first2, n);
		return result ? (result < 0) : (n1 < n2);
	}

	inline bool     // Specialization for unsigned char*.
	lexicographical_compare(unsigned char* first1, unsigned char* last1, unsigned char* first2, unsigned char* last2)
	{
		const ptrdiff_t n1(last1 - first1), n2(last2 - first2);
		const size_t n = (size_t)eastl::min_alt(n1, n2);
		if (n == 0) // don't call memcmp with n == 0
			return false;
		const int result = memcmp(first1, first2, n);
		return result ? (result < 0) : (n1 < n2);
	}

	inline bool     // Specialization for const signed char*.
	lexicographical_compare(const signed char* first1, const signed char* last1, const signed char* first2, const signed char* last2)
	{
		const ptrdiff_t n1(last1 - first1), n2(last2 - first2);
		const size_t n = (size_t)eastl::min_alt(n1, n2);
		if (n == 0) // don't call memcmp with n == 0
			return false;
		const int result = memcmp(first1, first2, n);
		return result ? (result < 0) : (n1 < n2);
	}

	inline bool     // Specialization for signed char*.
	lexicographical_compare(signed char* first1, signed char* last1, signed char* first2, signed char* last2)
	{
		const ptrdiff_t n1(last1 - first1), n2(last2 - first2);
		const size_t n = (size_t)eastl::min_alt(n1, n2);
		if (n == 0) // don't call memcmp with n == 0
			return false;
		const int result = memcmp(first1, first2, n);
		return result ? (result < 0) : (n1 < n2);
	}

	#if defined(_MSC_VER) // If using the VC++ compiler (and thus bool is known to be a single byte)...
		//Not sure if this is a good idea.
		//inline bool     // Specialization for const bool*.
		//lexicographical_compare(const bool* first1, const bool* last1, const bool* first2, const bool* last2)
		//{
		//    const ptrdiff_t n1(last1 - first1), n2(last2 - first2);
		//    const int result = memcmp(first1, first2, (size_t)eastl::min_alt(n1, n2));
		//    return result ? (result < 0) : (n1 < n2);
		//}
		//
		//inline bool     // Specialization for bool*.
		//lexicographical_compare(bool* first1, bool* last1, bool* first2, bool* last2)
		//{
		//    const ptrdiff_t n1(last1 - first1), n2(last2 - first2);
		//    const int result = memcmp(first1, first2, (size_t)eastl::min_alt(n1, n2));
		//    return result ? (result < 0) : (n1 < n2);
		//}
	#endif



	/// lexicographical_compare
	///
	/// Returns: true if the sequence of elements defined by the range
	/// [first1, last1) is lexicographically less than the sequence of
	/// elements defined by the range [first2, last2). Returns false otherwise.
	///
	/// Complexity: At most 'min((last1 -first1), (last2 - first2))' applications
	/// of the corresponding comparison.
	///
	/// Note: If two sequences have the same number of elements and their
	/// corresponding elements are equivalent, then neither sequence is
	/// lexicographically less than the other. If one sequence is a prefix
	/// of the other, then the shorter sequence is lexicographically less
	/// than the longer sequence. Otherwise, the lexicographical comparison
	/// of the sequences yields the same result as the comparison of the first
	/// corresponding pair of elements that are not equivalent.
	///
	/// Note: False is always returned if range 1 is exhausted before range 2.
	/// The result of this is that you can't do a successful reverse compare
	/// (e.g. use greater<> as the comparison instead of less<>) unless the
	/// two sequences are of identical length. What you want to do is reverse
	/// the order of the arguments in order to get the desired effect.
	///
	template <typename InputIterator1, typename InputIterator2, typename Compare>
	inline bool
	lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
							InputIterator2 first2, InputIterator2 last2, Compare compare)
	{
		for(; (first1 != last1) && (first2 != last2); ++first1, ++first2)
		{
			if(compare(*first1, *first2))
				return true;
			if(compare(*first2, *first1))
				return false;
		}
		return (first1 == last1) && (first2 != last2);
	}


#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)

	/// lexicographical_compare_three_way
	///
	/// Returns: The comparison category ordering between both ranges. For the first non-equivalent pair in the ranges,
	/// the comparison will be returned. Else if the first range is a subset (superset) of the second range, then the
	/// less (greater) ordering will be returned.
	///
	/// Complexity: At most N iterations, where N = min(last1-first1, last2-first2) of the applications
	/// of the corresponding comparison.
	///
	/// Note: If two sequences have the same number of elements and their
	/// corresponding elements are equivalent, then neither sequence is
	/// lexicographically less than the other. If one sequence is a prefix
	/// of the other, then the shorter sequence is lexicographically less
	/// than the longer sequence. Otherwise, the lexicographical comparison
	/// of the sequences yields the same result as the comparison of the first
	/// corresponding pair of elements that are not equivalent.
	///
	template <typename InputIterator1, typename InputIterator2, typename Compare>
	constexpr auto lexicographical_compare_three_way(InputIterator1 first1, InputIterator1 last1,
	                                                 InputIterator2 first2, InputIterator2 last2,
	                                                 Compare compare) -> decltype(compare(*first1, *first2))
	{
		for (; (first1 != last1) && (first2 != last2); ++first1, ++first2)
		{
			if (auto c = compare(*first1, *first2); c != 0)
				return c;
		}

		return (first1 != last1) ? std::strong_ordering::greater :
		       (first2 != last2) ? std::strong_ordering::less :
				std::strong_ordering::equal;
	}
#endif

	/// mismatch
	///
	/// Finds the first position where the two ranges [first1, last1) and
	/// [first2, first2 + (last1 - first1)) differ. The two versions of
	/// mismatch use different tests for whether elements differ.
	///
	/// Returns: A pair of iterators i and j such that j == first2 + (i - first1)
	/// and i is the first iterator in the range [first1, last1) for which the
	/// following corresponding condition holds: !(*i == *(first2 + (i - first1))).
	/// Returns the pair last1 and first2 + (last1 - first1) if such an iterator
	/// i is not found.
	///
	/// Complexity: At most last1 first1 applications of the corresponding predicate.
	///
	template <class InputIterator1, class InputIterator2>
	inline eastl::pair<InputIterator1, InputIterator2>
	mismatch(InputIterator1 first1, InputIterator1 last1,
			 InputIterator2 first2) // , InputIterator2 last2)
	{
		while((first1 != last1) && (*first1 == *first2)) // && (first2 != last2) <- C++ standard mismatch function doesn't check first2/last2.
		{
			++first1;
			++first2;
		}

		return eastl::pair<InputIterator1, InputIterator2>(first1, first2);
	}


	/// mismatch
	///
	/// Finds the first position where the two ranges [first1, last1) and
	/// [first2, first2 + (last1 - first1)) differ. The two versions of
	/// mismatch use different tests for whether elements differ.
	///
	/// Returns: A pair of iterators i and j such that j == first2 + (i - first1)
	/// and i is the first iterator in the range [first1, last1) for which the
	/// following corresponding condition holds: pred(*i, *(first2 + (i - first1))) == false.
	/// Returns the pair last1 and first2 + (last1 - first1) if such an iterator
	/// i is not found.
	///
	/// Complexity: At most last1 first1 applications of the corresponding predicate.
	///
	template <class InputIterator1, class InputIterator2, class BinaryPredicate>
	inline eastl::pair<InputIterator1, InputIterator2>
	mismatch(InputIterator1 first1, InputIterator1 last1,
			 InputIterator2 first2, // InputIterator2 last2,
			 BinaryPredicate predicate)
	{
		while((first1 != last1) && predicate(*first1, *first2)) // && (first2 != last2) <- C++ standard mismatch function doesn't check first2/last2.
		{
			++first1;
			++first2;
		}

		return eastl::pair<InputIterator1, InputIterator2>(first1, first2);
	}


	/// lower_bound
	///
	/// Finds the position of the first element in a sorted range that has a value
	/// greater than or equivalent to a specified value.
	///
	/// Effects: Finds the first position into which value can be inserted without
	/// violating the ordering.
	///
	/// Returns: The furthermost iterator i in the range [first, last) such that
	/// for any iterator j in the range [first, i) the following corresponding
	/// condition holds: *j < value.
	///
	/// Complexity: At most 'log(last - first) + 1' comparisons.
	///
	/// Optimizations: We have no need to specialize this implementation for random
	/// access iterators (e.g. contiguous array), as the code below will already
	/// take advantage of them.
	///
	template <typename ForwardIterator, typename T>
	ForwardIterator
	lower_bound(ForwardIterator first, ForwardIterator last, const T& value)
	{
		typedef typename eastl::iterator_traits<ForwardIterator>::difference_type DifferenceType;

		DifferenceType d = eastl::distance(first, last); // This will be efficient for a random access iterator such as an array.

		while(d > 0)
		{
			ForwardIterator i  = first;
			DifferenceType  d2 = d >> 1; // We use '>>1' here instead of '/2' because MSVC++ for some reason generates significantly worse code for '/2'. Go figure.

			eastl::advance(i, d2); // This will be efficient for a random access iterator such as an array.

			if(*i < value)
			{
				// Disabled because std::lower_bound doesn't specify (23.3.3.3, p3) this can be done: EASTL_VALIDATE_COMPARE(!(value < *i)); // Validate that the compare function is sane.
				first = ++i;
				d    -= d2 + 1;
			}
			else
				d = d2;
		}
		return first;
	}


	/// lower_bound
	///
	/// Finds the position of the first element in a sorted range that has a value
	/// greater than or equivalent to a specified value. The input Compare function
	/// takes two arguments and returns true if the first argument is less than
	/// the second argument.
	///
	/// Effects: Finds the first position into which value can be inserted without
	/// violating the ordering.
	///
	/// Returns: The furthermost iterator i in the range [first, last) such that
	/// for any iterator j in the range [first, i) the following corresponding
	/// condition holds: compare(*j, value) != false.
	///
	/// Complexity: At most 'log(last - first) + 1' comparisons.
	///
	/// Optimizations: We have no need to specialize this implementation for random
	/// access iterators (e.g. contiguous array), as the code below will already
	/// take advantage of them.
	///
	template <typename ForwardIterator, typename T, typename Compare>
	ForwardIterator
	lower_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare compare)
	{
		typedef typename eastl::iterator_traits<ForwardIterator>::difference_type DifferenceType;

		DifferenceType d = eastl::distance(first, last); // This will be efficient for a random access iterator such as an array.

		while(d > 0)
		{
			ForwardIterator i  = first;
			DifferenceType  d2 = d >> 1; // We use '>>1' here instead of '/2' because MSVC++ for some reason generates significantly worse code for '/2'. Go figure.

			eastl::advance(i, d2); // This will be efficient for a random access iterator such as an array.

			if(compare(*i, value))
			{
				// Disabled because std::lower_bound doesn't specify (23.3.3.1, p3) this can be done: EASTL_VALIDATE_COMPARE(!compare(value, *i)); // Validate that the compare function is sane.
				first = ++i;
				d    -= d2 + 1;
			}
			else
				d = d2;
		}
		return first;
	}



	/// upper_bound
	///
	/// Finds the position of the first element in a sorted range that has a
	/// value that is greater than a specified value.
	///
	/// Effects: Finds the furthermost position into which value can be inserted
	/// without violating the ordering.
	///
	/// Returns: The furthermost iterator i in the range [first, last) such that
	/// for any iterator j in the range [first, i) the following corresponding
	/// condition holds: !(value < *j).
	///
	/// Complexity: At most 'log(last - first) + 1' comparisons.
	///
	template <typename ForwardIterator, typename T>
	ForwardIterator
	upper_bound(ForwardIterator first, ForwardIterator last, const T& value)
	{
		typedef typename eastl::iterator_traits<ForwardIterator>::difference_type DifferenceType;

		DifferenceType len = eastl::distance(first, last);

		while(len > 0)
		{
			ForwardIterator i    = first;
			DifferenceType  len2 = len >> 1; // We use '>>1' here instead of '/2' because MSVC++ for some reason generates significantly worse code for '/2'. Go figure.

			eastl::advance(i, len2);

			if(!(value < *i)) // Note that we always express value comparisons in terms of < or ==.
			{
				first = ++i;
				len -= len2 + 1;
			}
			else
			{
				// Disabled because std::upper_bound doesn't specify (23.3.3.2, p3) this can be done: EASTL_VALIDATE_COMPARE(!(*i < value)); // Validate that the compare function is sane.
				len = len2;
			}
		}
		return first;
	}


	/// upper_bound
	///
	/// Finds the position of the first element in a sorted range that has a
	/// value that is greater than a specified value. The input Compare function
	/// takes two arguments and returns true if the first argument is less than
	/// the second argument.
	///
	/// Effects: Finds the furthermost position into which value can be inserted
	/// without violating the ordering.
	///
	/// Returns: The furthermost iterator i in the range [first, last) such that
	/// for any iterator j in the range [first, i) the following corresponding
	/// condition holds: compare(value, *j) == false.
	///
	/// Complexity: At most 'log(last - first) + 1' comparisons.
	///
	template <typename ForwardIterator, typename T, typename Compare>
	ForwardIterator
	upper_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare compare)
	{
		typedef typename eastl::iterator_traits<ForwardIterator>::difference_type DifferenceType;

		DifferenceType len = eastl::distance(first, last);

		while(len > 0)
		{
			ForwardIterator i    = first;
			DifferenceType  len2 = len >> 1; // We use '>>1' here instead of '/2' because MSVC++ for some reason generates significantly worse code for '/2'. Go figure.

			eastl::advance(i, len2);

			if(!compare(value, *i))
			{
				first = ++i;
				len -= len2 + 1;
			}
			else
			{
				// Disabled because std::upper_bound doesn't specify (23.3.3.2, p3) this can be done: EASTL_VALIDATE_COMPARE(!compare(*i, value)); // Validate that the compare function is sane.
				len = len2;
			}
		}
		return first;
	}


	/// equal_range
	///
	/// Effects: Finds the largest subrange [i, j) such that the value can be inserted
	/// at any iterator k in it without violating the ordering. k satisfies the
	/// corresponding conditions: !(*k < value) && !(value < *k).
	///
	/// Complexity: At most '2 * log(last - first) + 1' comparisons.
	///
	template <typename ForwardIterator, typename T>
	pair<ForwardIterator, ForwardIterator>
	equal_range(ForwardIterator first, ForwardIterator last, const T& value)
	{
		typedef pair<ForwardIterator, ForwardIterator> ResultType;
		typedef typename eastl::iterator_traits<ForwardIterator>::difference_type DifferenceType;

		DifferenceType d = eastl::distance(first, last);

		while(d > 0)
		{
			ForwardIterator i(first);
			DifferenceType  d2 = d >> 1; // We use '>>1' here instead of '/2' because MSVC++ for some reason generates significantly worse code for '/2'. Go figure.

			eastl::advance(i, d2);

			if(*i < value)
			{
				EASTL_VALIDATE_COMPARE(!(value < *i)); // Validate that the compare function is sane.
				first = ++i;
				d    -= d2 + 1;
			}
			else if(value < *i)
			{
				EASTL_VALIDATE_COMPARE(!(*i < value)); // Validate that the compare function is sane.
				d    = d2;
				last = i;
			}
			else
			{
				ForwardIterator j(i);

				return ResultType(eastl::lower_bound(first, i, value),
								  eastl::upper_bound(++j, last, value));
			}
		}
		return ResultType(first, first);
	}


	/// equal_range
	///
	/// Effects: Finds the largest subrange [i, j) such that the value can be inserted
	/// at any iterator k in it without violating the ordering. k satisfies the
	/// corresponding conditions: compare(*k, value) == false && compare(value, *k) == false.
	///
	/// Complexity: At most '2 * log(last - first) + 1' comparisons.
	///
	template <typename ForwardIterator, typename T, typename Compare>
	pair<ForwardIterator, ForwardIterator>
	equal_range(ForwardIterator first, ForwardIterator last, const T& value, Compare compare)
	{
		typedef pair<ForwardIterator, ForwardIterator> ResultType;
		typedef typename eastl::iterator_traits<ForwardIterator>::difference_type DifferenceType;

		DifferenceType d = eastl::distance(first, last);

		while(d > 0)
		{
			ForwardIterator i(first);
			DifferenceType  d2 = d >> 1; // We use '>>1' here instead of '/2' because MSVC++ for some reason generates significantly worse code for '/2'. Go figure.

			eastl::advance(i, d2);

			if(compare(*i, value))
			{
				EASTL_VALIDATE_COMPARE(!compare(value, *i)); // Validate that the compare function is sane.
				first = ++i;
				d    -= d2 + 1;
			}
			else if(compare(value, *i))
			{
				EASTL_VALIDATE_COMPARE(!compare(*i, value)); // Validate that the compare function is sane.
				d    = d2;
				last = i;
			}
			else
			{
				ForwardIterator j(i);

				return ResultType(eastl::lower_bound(first, i, value, compare),
								  eastl::upper_bound(++j, last, value, compare));
			}
		}
		return ResultType(first, first);
	}


	/// replace
	///
	/// Effects: Substitutes elements referred by the iterator i in the range [first, last)
	/// with new_value, when the following corresponding conditions hold: *i == old_value.
	///
	/// Complexity: Exactly 'last - first' applications of the corresponding predicate.
	///
	/// Note: The predicate version of replace is replace_if and not another variation of replace.
	/// This is because both versions would have the same parameter count and there could be ambiguity.
	///
	template <typename ForwardIterator, typename T>
	inline void
	replace(ForwardIterator first, ForwardIterator last, const T& old_value, const T& new_value)
	{
		for(; first != last; ++first)
		{
			if(*first == old_value)
				*first = new_value;
		}
	}


	/// replace_if
	///
	/// Effects: Substitutes elements referred by the iterator i in the range [first, last)
	/// with new_value, when the following corresponding conditions hold: predicate(*i) != false.
	///
	/// Complexity: Exactly 'last - first' applications of the corresponding predicate.
	///
	/// Note: The predicate version of replace_if is replace and not another variation of replace_if.
	/// This is because both versions would have the same parameter count and there could be ambiguity.
	///
	template <typename ForwardIterator, typename Predicate, typename T>
	inline void
	replace_if(ForwardIterator first, ForwardIterator last, Predicate predicate, const T& new_value)
	{
		for(; first != last; ++first)
		{
			if(predicate(*first))
				*first = new_value;
		}
	}


	/// remove_copy
	///
	/// Effects: Copies all the elements referred to by the iterator i in the range
	/// [first, last) for which the following corresponding condition does not hold:
	/// *i == value.
	///
	/// Requires: The ranges [first, last) and [result, result + (last - first)) shall not overlap.
	///
	/// Returns: The end of the resulting range.
	///
	/// Complexity: Exactly 'last - first' applications of the corresponding predicate.
	///
	template <typename InputIterator, typename OutputIterator, typename T>
	inline OutputIterator
	remove_copy(InputIterator first, InputIterator last, OutputIterator result, const T& value)
	{
		for(; first != last; ++first)
		{
			if(!(*first == value)) // Note that we always express value comparisons in terms of < or ==.
			{
				*result = eastl::move(*first);
				++result;
			}
		}
		return result;
	}


	/// remove_copy_if
	///
	/// Effects: Copies all the elements referred to by the iterator i in the range
	/// [first, last) for which the following corresponding condition does not hold:
	/// predicate(*i) != false.
	///
	/// Requires: The ranges [first, last) and [result, result + (last - first)) shall not overlap.
	///
	/// Returns: The end of the resulting range.
	///
	/// Complexity: Exactly 'last - first' applications of the corresponding predicate.
	///
	template <typename InputIterator, typename OutputIterator, typename Predicate>
	inline OutputIterator
	remove_copy_if(InputIterator first, InputIterator last, OutputIterator result, Predicate predicate)
	{
		for(; first != last; ++first)
		{
			if(!predicate(*first))
			{
				*result = eastl::move(*first);
				++result;
			}
		}
		return result;
	}


	/// remove
	///
	/// Effects: Eliminates all the elements referred to by iterator i in the
	/// range [first, last) for which the following corresponding condition
	/// holds: *i == value.
	///
	/// Returns: The end of the resulting range.
	///
	/// Complexity: Exactly 'last - first' applications of the corresponding predicate.
	///
	/// Note: The predicate version of remove is remove_if and not another variation of remove.
	/// This is because both versions would have the same parameter count and there could be ambiguity.
	///
	/// Note: Since this function moves the element to the back of the heap and
	/// doesn't actually remove it from the given container, the user must call
	/// the container erase function if the user wants to erase the element
	/// from the container.
	///
	/// Example usage:
	///    vector<int> intArray;
	///    ...
	///    intArray.erase(remove(intArray.begin(), intArray.end(), 4), intArray.end()); // Erase all elements of value 4.
	///
	template <typename ForwardIterator, typename T>
	inline ForwardIterator
	remove(ForwardIterator first, ForwardIterator last, const T& value)
	{
		first = eastl::find(first, last, value);
		if(first != last)
		{
			ForwardIterator i(first);
			return eastl::remove_copy(++i, last, first, value);
		}
		return first;
	}


	/// remove_if
	///
	/// Effects: Eliminates all the elements referred to by iterator i in the
	/// range [first, last) for which the following corresponding condition
	/// holds: predicate(*i) != false.
	///
	/// Returns: The end of the resulting range.
	///
	/// Complexity: Exactly 'last - first' applications of the corresponding predicate.
	///
	/// Note: The predicate version of remove_if is remove and not another variation of remove_if.
	/// This is because both versions would have the same parameter count and there could be ambiguity.
	///
	/// Note: Since this function moves the element to the back of the heap and
	/// doesn't actually remove it from the given container, the user must call
	/// the container erase function if the user wants to erase the element
	/// from the container.
	///
	/// Example usage:
	///    vector<int> intArray;
	///    ...
	///    intArray.erase(remove(intArray.begin(), intArray.end(), bind2nd(less<int>(), (int)3)), intArray.end()); // Erase all elements less than 3.
	///
	template <typename ForwardIterator, typename Predicate>
	inline ForwardIterator
	remove_if(ForwardIterator first, ForwardIterator last, Predicate predicate)
	{
		first = eastl::find_if(first, last, predicate);
		if(first != last)
		{
			ForwardIterator i(first);
			return eastl::remove_copy_if<ForwardIterator, ForwardIterator, Predicate>(++i, last, first, predicate);
		}
		return first;
	}


	/// apply_and_remove_if
	///
	/// Calls the Function function for all elements referred to  my iterator i in the range
	/// [first, last) for which the following corresponding condition holds:
	/// predicate(*i) == true
	/// and then left shift moves potential non-matching elements over it.
	///
	/// Returns: a past-the-end iterator for the new end of the range.
	///
	/// Complexity: Exactly 'last - first' applications of the corresponding predicate + applies
	/// function once for every time the condition holds.
	///
	/// Note: Since removing is done by shifting (by means of copy move assignment) the elements
	/// in the range in such a way that the elements that are not to be removed appear in the
	/// beginning of the range doesn't actually remove it from the given container, the user must call
	/// the container erase function if the user wants to erase the element
	/// from the container. I.e. in the same they as for remove_if the excess elements
	/// are left in a valid but possibly moved from state.
	///
	template <typename ForwardIterator, typename Function, typename Predicate>
	inline ForwardIterator apply_and_remove_if(ForwardIterator first,
	                                           ForwardIterator last,
	                                           Function function,
	                                           Predicate predicate)
	{
		first = eastl::find_if(first, last, predicate);
		if (first != last)
		{
			function(*first);
			for (auto i = next(first); i != last; ++i)
			{
				if (predicate(*i))
				{
					function(*i);
					continue;
				}
				*first = eastl::move(*i);
				++first;
			}
		}
		return first;
	}


	/// apply_and_remove
	///
	/// Calls the Function function for all elements referred to my iterator i in the range
	/// [first, last) for which the following corresponding condition holds:
	/// value == *i
	/// and then left shift moves potential non-matching elements over it.
	///
	/// Returns: a past-the-end iterator for the new end of the range.
	///
	/// Complexity: Exactly 'last - first' applications of the corresponding equality test
	/// + applies function once for every time the condition holds.
	///
	/// Note: Since removing is done by shifting (by means of copy move assignment) the elements
	/// in the range in such a way that the elements that are not to be removed appear in the
	/// beginning of the range doesn't actually remove it from the given container, the user must call
	/// the container erase function if the user wants to erase the element
	/// from the container. I.e. in the same they as for remove_if the excess elements
	/// are left in a valid but possibly moved from state.
	///
	template <typename ForwardIterator, typename Function, typename T>
	inline ForwardIterator apply_and_remove(ForwardIterator first,
	                                        ForwardIterator last,
	                                        Function function,
	                                        const T& value)
	{
		first = eastl::find(first, last, value);
		if (first != last)
		{
			function(*first);
			for (auto i = next(first); i != last; ++i)
			{
				if (value == *i)
				{
					function(*i);
					continue;
				}
				*first = eastl::move(*i);
				++first;
			}
		}
		return first;
	}


	/// replace_copy
	///
	/// Effects: Assigns to every iterator i in the range [result, result + (last - first))
	/// either new_value or *(first + (i - result)) depending on whether the following
	/// corresponding conditions hold: *(first + (i - result)) == old_value.
	///
	/// Requires: The ranges [first, last) and [result, result + (last - first)) shall not overlap.
	///
	/// Returns: result + (last - first).
	///
	/// Complexity: Exactly 'last - first' applications of the corresponding predicate.
	///
	/// Note: The predicate version of replace_copy is replace_copy_if and not another variation of replace_copy.
	/// This is because both versions would have the same parameter count and there could be ambiguity.
	///
	template <typename InputIterator, typename OutputIterator, typename T>
	inline OutputIterator
	replace_copy(InputIterator first, InputIterator last, OutputIterator result, const T& old_value, const T& new_value)
	{
		for(; first != last; ++first, ++result)
			*result = (*first == old_value) ? new_value : *first;
		return result;
	}


	/// replace_copy_if
	///
	/// Effects: Assigns to every iterator i in the range [result, result + (last - first))
	/// either new_value or *(first + (i - result)) depending on whether the following
	/// corresponding conditions hold: predicate(*(first + (i - result))) != false.
	///
	/// Requires: The ranges [first, last) and [result, result+(lastfirst)) shall not overlap.
	///
	/// Returns: result + (last - first).
	///
	/// Complexity: Exactly 'last - first' applications of the corresponding predicate.
	///
	/// Note: The predicate version of replace_copy_if is replace_copy and not another variation of replace_copy_if.
	/// This is because both versions would have the same parameter count and there could be ambiguity.
	///
	template <typename InputIterator, typename OutputIterator, typename Predicate, typename T>
	inline OutputIterator
	replace_copy_if(InputIterator first, InputIterator last, OutputIterator result, Predicate predicate, const T& new_value)
	{
		for(; first != last; ++first, ++result)
			*result = predicate(*first) ? new_value : *first;
		return result;
	}




	// reverse
	//
	// We provide helper functions which allow reverse to be implemented more
	// efficiently for some types of iterators and types.
	//
	template <typename BidirectionalIterator>
	inline void reverse_impl(BidirectionalIterator first, BidirectionalIterator last, EASTL_ITC_NS::bidirectional_iterator_tag)
	{
		for(; (first != last) && (first != --last); ++first) // We are not allowed to use operator <, <=, >, >= with a
			eastl::iter_swap(first, last);                   // generic (bidirectional or otherwise) iterator.
	}

	template <typename RandomAccessIterator>
	inline void reverse_impl(RandomAccessIterator first, RandomAccessIterator last, EASTL_ITC_NS::random_access_iterator_tag)
	{
		if(first != last)
		{
			for(; first < --last; ++first)      // With a random access iterator, we can use operator < to more efficiently implement
				eastl::iter_swap(first, last);  // this algorithm. A generic iterator doesn't necessarily have an operator < defined.
		}
	}

	/// reverse
	///
	/// Reverses the values within the range [first, last).
	///
	/// Effects: For each nonnegative integer i <= (last - first) / 2,
	/// applies swap to all pairs of iterators first + i, (last i) - 1.
	///
	/// Complexity: Exactly '(last - first) / 2' swaps.
	///
	template <typename BidirectionalIterator>
	inline void reverse(BidirectionalIterator first, BidirectionalIterator last)
	{
		typedef typename eastl::iterator_traits<BidirectionalIterator>::iterator_category IC;
		eastl::reverse_impl(first, last, IC());
	}



	/// reverse_copy
	///
	/// Copies the range [first, last) in reverse order to the result.
	///
	/// Effects: Copies the range [first, last) to the range
	/// [result, result + (last - first)) such that for any nonnegative
	/// integer i < (last - first) the following assignment takes place:
	/// *(result + (last - first) - i) = *(first + i)
	///
	/// Requires: The ranges [first, last) and [result, result + (last - first))
	/// shall not overlap.
	///
	/// Returns: result + (last - first). That is, returns the end of the output range.
	///
	/// Complexity: Exactly 'last - first' assignments.
	///
	template <typename BidirectionalIterator, typename OutputIterator>
	inline OutputIterator
	reverse_copy(BidirectionalIterator first, BidirectionalIterator last, OutputIterator result)
	{
		for(; first != last; ++result)
			*result = *--last;
		return result;
	}



	/// search
	///
	/// Search finds a subsequence within the range [first1, last1) that is identical to [first2, last2)
	/// when compared element-by-element. It returns an iterator pointing to the beginning of that
	/// subsequence, or else last1 if no such subsequence exists. As such, it is very much like
	/// the C strstr function, with the primary difference being that strstr uses 0-terminated strings
	/// whereas search uses an end iterator to specify the end of a string.
	///
	/// Returns: The first iterator i in the range [first1, last1 - (last2 - first2)) such that for
	/// any nonnegative integer n less than 'last2 - first2' the following corresponding condition holds:
	/// *(i + n) == *(first2 + n). Returns last1 if no such iterator is found.
	///
	/// Complexity: At most (last1 first1) * (last2 first2) applications of the corresponding predicate.
	///
	template <typename ForwardIterator1, typename ForwardIterator2>
	ForwardIterator1
	search(ForwardIterator1 first1, ForwardIterator1 last1,
		   ForwardIterator2 first2, ForwardIterator2 last2)
	{
		if(first2 != last2) // If there is anything to search for...
		{
			// We need to make a special case for a pattern of one element,
			// as the logic below prevents one element patterns from working.
			ForwardIterator2 temp2(first2);
			++temp2;

			if(temp2 != last2) // If what we are searching for has a length > 1...
			{
				ForwardIterator1 cur1(first1);
				ForwardIterator2 p2;

				while(first1 != last1)
				{
					// The following loop is the equivalent of eastl::find(first1, last1, *first2)
					while((first1 != last1) && !(*first1 == *first2))
						++first1;

					if(first1 != last1)
					{
						p2   = temp2;
						cur1 = first1;

						if(++cur1 != last1)
						{
							while(*cur1 == *p2)
							{
								if(++p2 == last2)
									return first1;

								if(++cur1 == last1)
									return last1;
							}

							++first1;
							continue;
						}
					}
					return last1;
				}

				// Fall through to the end.
			}
			else
				return eastl::find(first1, last1, *first2);
		}

		return first1;


		#if 0
		/*  Another implementation which is a little more simpler but executes a little slower on average.
			typedef typename eastl::iterator_traits<ForwardIterator1>::difference_type difference_type_1;
			typedef typename eastl::iterator_traits<ForwardIterator2>::difference_type difference_type_2;

			const difference_type_2 d2 = eastl::distance(first2, last2);

			for(difference_type_1 d1 = eastl::distance(first1, last1); d1 >= d2; ++first1, --d1)
			{
				ForwardIterator1 temp1 = first1;

				for(ForwardIterator2 temp2 = first2; ; ++temp1, ++temp2)
				{
					if(temp2 == last2)
						return first1;
					if(!(*temp1 == *temp2))
						break;
				}
			}

			return last1;
		*/
		#endif
	}


	/// search
	///
	/// Search finds a subsequence within the range [first1, last1) that is identical to [first2, last2)
	/// when compared element-by-element. It returns an iterator pointing to the beginning of that
	/// subsequence, or else last1 if no such subsequence exists. As such, it is very much like
	/// the C strstr function, with the only difference being that strstr uses 0-terminated strings
	/// whereas search uses an end iterator to specify the end of a string.
	///
	/// Returns: The first iterator i in the range [first1, last1 - (last2 - first2)) such that for
	/// any nonnegative integer n less than 'last2 - first2' the following corresponding condition holds:
	/// predicate(*(i + n), *(first2 + n)) != false. Returns last1 if no such iterator is found.
	///
	/// Complexity: At most (last1 first1) * (last2 first2) applications of the corresponding predicate.
	///
	template <typename ForwardIterator1, typename ForwardIterator2, typename BinaryPredicate>
	ForwardIterator1
	search(ForwardIterator1 first1, ForwardIterator1 last1,
		   ForwardIterator2 first2, ForwardIterator2 last2,
		   BinaryPredicate predicate)
	{
		typedef typename eastl::iterator_traits<ForwardIterator1>::difference_type difference_type_1;
		typedef typename eastl::iterator_traits<ForwardIterator2>::difference_type difference_type_2;

		difference_type_2 d2 = eastl::distance(first2, last2);

		if(d2 != 0)
		{
			ForwardIterator1 i(first1);
			eastl::advance(i, d2);

			for(difference_type_1 d1 = eastl::distance(first1, last1); d1 >= d2; --d1)
			{
				if(eastl::equal<ForwardIterator1, ForwardIterator2, BinaryPredicate>(first1, i, first2, predicate))
					return first1;
				if(d1 > d2) // To do: Find a way to make the algorithm more elegant.
				{
					++first1;
					++i;
				}
			}
			return last1;
		}
		return first1; // Just like with strstr, we return first1 if the match string is empty.
	}



	// search_n helper functions
	//
	template <typename ForwardIterator, typename Size, typename T>
	ForwardIterator     // Generic implementation.
	search_n_impl(ForwardIterator first, ForwardIterator last, Size count, const T& value, EASTL_ITC_NS::forward_iterator_tag)
	{
		if(count <= 0)
			return first;

		Size d1 = (Size)eastl::distance(first, last); // Should d1 be of type Size, ptrdiff_t, or iterator_traits<ForwardIterator>::difference_type?
													  // The problem with using iterator_traits<ForwardIterator>::difference_type is that
		if(count > d1)                                // ForwardIterator may not be a true iterator but instead something like a pointer.
			return last;

		for(; d1 >= count; ++first, --d1)
		{
			ForwardIterator i(first);

			for(Size n = 0; n < count; ++n, ++i, --d1)
			{
				if(!(*i == value)) // Note that we always express value comparisons in terms of < or ==.
					goto not_found;
			}
			return first;

			not_found:
			first = i;
		}
		return last;
	}

	template <typename RandomAccessIterator, typename Size, typename T> inline
	RandomAccessIterator    // Random access iterator implementation. Much faster than generic implementation.
	search_n_impl(RandomAccessIterator first, RandomAccessIterator last, Size count, const T& value, EASTL_ITC_NS::random_access_iterator_tag)
	{
		if(count <= 0)
			return first;
		else if(count == 1)
			return eastl::find(first, last, value);
		else if(last > first)
		{
			RandomAccessIterator lookAhead;
			RandomAccessIterator backTrack;

			Size skipOffset = (count - 1);
			Size tailSize = (Size)(last - first);
			Size remainder;
			Size prevRemainder;

			for(lookAhead = first + skipOffset; tailSize >= count; lookAhead += count)
			{
				tailSize -= count;

				if(*lookAhead == value)
				{
					remainder = skipOffset;

					for(backTrack = lookAhead - 1; *backTrack == value; --backTrack)
					{
						if(--remainder == 0)
							return (lookAhead - skipOffset); // success
					}

					if(remainder <= tailSize)
					{
						prevRemainder = remainder;

						while(*(++lookAhead) == value)
						{
							if(--remainder == 0)
								return (backTrack + 1); // success
						}
						tailSize -= (prevRemainder - remainder);
					}
					else
						return last; // failure
				}

				// lookAhead here is always pointing to the element of the last mismatch.
			}
		}

		return last; // failure
	}


	/// search_n
	///
	/// Returns: The first iterator i in the range [first, last count) such that
	/// for any nonnegative integer n less than count the following corresponding
	/// conditions hold: *(i + n) == value, pred(*(i + n),value) != false.
	/// Returns last if no such iterator is found.
	///
	/// Complexity: At most '(last1 - first1) * count' applications of the corresponding predicate.
	///
	template <typename ForwardIterator, typename Size, typename T>
	ForwardIterator
	search_n(ForwardIterator first, ForwardIterator last, Size count, const T& value)
	{
		typedef typename eastl::iterator_traits<ForwardIterator>::iterator_category IC;
		return eastl::search_n_impl(first, last, count, value, IC());
	}


	/// binary_search
	///
	/// Returns: true if there is an iterator i in the range [first last) that
	/// satisfies the corresponding conditions: !(*i < value) && !(value < *i).
	///
	/// Complexity: At most 'log(last - first) + 2' comparisons.
	///
	/// Note: The reason binary_search returns bool instead of an iterator is
	/// that search_n, lower_bound, or equal_range already return an iterator.
	/// However, there are arguments that binary_search should return an iterator.
	/// Note that we provide binary_search_i (STL extension) to return an iterator.
	///
	/// To use search_n to find an item, do this:
	///     iterator i = search_n(begin, end, 1, value);
	/// To use lower_bound to find an item, do this:
	///     iterator i = lower_bound(begin, end, value);
	///     if((i != last) && !(value < *i))
	///         <use the iterator>
	/// It turns out that the above lower_bound method is as fast as binary_search
	/// would be if it returned an iterator.
	///
	template <typename ForwardIterator, typename T>
	inline bool
	binary_search(ForwardIterator first, ForwardIterator last, const T& value)
	{
		// To do: This can be made slightly faster by not using lower_bound.
		ForwardIterator i(eastl::lower_bound<ForwardIterator, T>(first, last, value));
		return ((i != last) && !(value < *i)); // Note that we always express value comparisons in terms of < or ==.
	}


	/// binary_search
	///
	/// Returns: true if there is an iterator i in the range [first last) that
	/// satisfies the corresponding conditions: compare(*i, value) == false &&
	/// compare(value, *i) == false.
	///
	/// Complexity: At most 'log(last - first) + 2' comparisons.
	///
	/// Note: See comments above regarding the bool return value of binary_search.
	///
	template <typename ForwardIterator, typename T, typename Compare>
	inline bool
	binary_search(ForwardIterator first, ForwardIterator last, const T& value, Compare compare)
	{
		// To do: This can be made slightly faster by not using lower_bound.
		ForwardIterator i(eastl::lower_bound<ForwardIterator, T, Compare>(first, last, value, compare));
		return ((i != last) && !compare(value, *i));
	}


	/// binary_search_i
	///
	/// Returns: iterator if there is an iterator i in the range [first last) that
	/// satisfies the corresponding conditions: !(*i < value) && !(value < *i).
	/// Returns last if the value is not found.
	///
	/// Complexity: At most 'log(last - first) + 2' comparisons.
	///
	template <typename ForwardIterator, typename T>
	inline ForwardIterator
	binary_search_i(ForwardIterator first, ForwardIterator last, const T& value)
	{
		// To do: This can be made slightly faster by not using lower_bound.
		ForwardIterator i(eastl::lower_bound<ForwardIterator, T>(first, last, value));
		if((i != last) && !(value < *i)) // Note that we always express value comparisons in terms of < or ==.
			return i;
		return last;
	}


	/// binary_search_i
	///
	/// Returns: iterator if there is an iterator i in the range [first last) that
	/// satisfies the corresponding conditions: !(*i < value) && !(value < *i).
	/// Returns last if the value is not found.
	///
	/// Complexity: At most 'log(last - first) + 2' comparisons.
	///
	template <typename ForwardIterator, typename T, typename Compare>
	inline ForwardIterator
	binary_search_i(ForwardIterator first, ForwardIterator last, const T& value, Compare compare)
	{
		// To do: This can be made slightly faster by not using lower_bound.
		ForwardIterator i(eastl::lower_bound<ForwardIterator, T, Compare>(first, last, value, compare));
		if((i != last) && !compare(value, *i))
			return i;
		return last;
	}


	/// unique
	///
	/// Given a sorted range, this function removes duplicated items.
	/// Note that if you have a container then you will probably want
	/// to call erase on the container with the return value if your
	/// goal is to remove the duplicated items from the container.
	///
	/// Effects: Eliminates all but the first element from every consecutive
	/// group of equal elements referred to by the iterator i in the range
	/// [first, last) for which the following corresponding condition holds:
	/// *i == *(i - 1).
	///
	/// Returns: The end of the resulting range.
	///
	/// Complexity: If the range (last - first) is not empty, exactly (last - first)
	/// applications of the corresponding predicate, otherwise no applications of the predicate.
	///
	/// Example usage:
	///    vector<int> intArray;
	///    ...
	///    intArray.erase(unique(intArray.begin(), intArray.end()), intArray.end());
	///
	template <typename ForwardIterator>
	ForwardIterator unique(ForwardIterator first, ForwardIterator last)
	{
		first = eastl::adjacent_find<ForwardIterator>(first, last);

		if(first != last) // We expect that there are duplicated items, else the user wouldn't be calling this function.
		{
			ForwardIterator dest(first);

			for(++first; first != last; ++first)
			{
				if(!(*dest == *first)) // Note that we always express value comparisons in terms of < or ==.
					*++dest = *first;
			}
			return ++dest;
		}
		return last;
	}


	/// unique
	///
	/// Given a sorted range, this function removes duplicated items.
	/// Note that if you have a container then you will probably want
	/// to call erase on the container with the return value if your
	/// goal is to remove the duplicated items from the container.
	///
	/// Effects: Eliminates all but the first element from every consecutive
	/// group of equal elements referred to by the iterator i in the range
	/// [first, last) for which the following corresponding condition holds:
	/// predicate(*i, *(i - 1)) != false.
	///
	/// Returns: The end of the resulting range.
	///
	/// Complexity: If the range (last - first) is not empty, exactly (last - first)
	/// applications of the corresponding predicate, otherwise no applications of the predicate.
	///
	template <typename ForwardIterator, typename BinaryPredicate>
	ForwardIterator unique(ForwardIterator first, ForwardIterator last, BinaryPredicate predicate)
	{
		first = eastl::adjacent_find<ForwardIterator, BinaryPredicate>(first, last, predicate);

		if(first != last) // We expect that there are duplicated items, else the user wouldn't be calling this function.
		{
			ForwardIterator dest(first);

			for(++first; first != last; ++first)
			{
				if(!predicate(*dest, *first))
					*++dest = *first;
			}
			return ++dest;
		}
		return last;
	}



	// find_end
	//
	// We provide two versions here, one for a bidirectional iterators and one for
	// regular forward iterators. Given that we are searching backward, it's a bit
	// more efficient if we can use backwards iteration to implement our search,
	// though this requires an iterator that can be reversed.
	//
	template <typename ForwardIterator1, typename ForwardIterator2>
	ForwardIterator1
	find_end_impl(ForwardIterator1 first1, ForwardIterator1 last1,
				  ForwardIterator2 first2, ForwardIterator2 last2,
				  EASTL_ITC_NS::forward_iterator_tag, EASTL_ITC_NS::forward_iterator_tag)
	{
		if(first2 != last2) // We have to do this check because the search algorithm below will return first1 (and not last1) if the first2/last2 range is empty.
		{
			for(ForwardIterator1 result(last1); ; )
			{
				const ForwardIterator1 resultNext(eastl::search(first1, last1, first2, last2));

				if(resultNext != last1) // If another sequence was found...
				{
					first1 = result = resultNext;
					++first1;
				}
				else
					return result;
			}
		}
		return last1;
	}

	template <typename BidirectionalIterator1, typename BidirectionalIterator2>
	BidirectionalIterator1
	find_end_impl(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
				  BidirectionalIterator2 first2, BidirectionalIterator2 last2,
				  EASTL_ITC_NS::bidirectional_iterator_tag, EASTL_ITC_NS::bidirectional_iterator_tag)
	{
		typedef eastl::reverse_iterator<BidirectionalIterator1> reverse_iterator1;
		typedef eastl::reverse_iterator<BidirectionalIterator2> reverse_iterator2;

		reverse_iterator1 rresult(eastl::search(reverse_iterator1(last1), reverse_iterator1(first1),
												reverse_iterator2(last2), reverse_iterator2(first2)));
		if(rresult.base() != first1) // If we found something...
		{
			BidirectionalIterator1 result(rresult.base());

			eastl::advance(result, -eastl::distance(first2, last2)); // We have an opportunity to optimize this, as the
			return result;                                           // search function already calculates this distance.
		}
		return last1;
	}

	/// find_end
	///
	/// Finds the last occurrence of the second sequence in the first sequence.
	/// As such, this function is much like the C string function strrstr and it
	/// is also the same as a reversed version of 'search'. It is called find_end
	/// instead of the possibly more consistent search_end simply because the C++
	/// standard algorithms have such naming.
	///
	/// Returns an iterator between first1 and last1 if the sequence is found.
	/// returns last1 (the end of the first seqence) if the sequence is not found.
	///
	template <typename ForwardIterator1, typename ForwardIterator2>
	inline ForwardIterator1
	find_end(ForwardIterator1 first1, ForwardIterator1 last1,
			 ForwardIterator2 first2, ForwardIterator2 last2)
	{
		typedef typename eastl::iterator_traits<ForwardIterator1>::iterator_category IC1;
		typedef typename eastl::iterator_traits<ForwardIterator2>::iterator_category IC2;

		return eastl::find_end_impl(first1, last1, first2, last2, IC1(), IC2());
	}




	// To consider: Fold the predicate and non-predicate versions of
	//              this algorithm into a single function.
	template <typename ForwardIterator1, typename ForwardIterator2, typename BinaryPredicate>
	ForwardIterator1
	find_end_impl(ForwardIterator1 first1, ForwardIterator1 last1,
				  ForwardIterator2 first2, ForwardIterator2 last2,
				  BinaryPredicate predicate,
				  EASTL_ITC_NS::forward_iterator_tag, EASTL_ITC_NS::forward_iterator_tag)
	{
		if(first2 != last2) // We have to do this check because the search algorithm below will return first1 (and not last1) if the first2/last2 range is empty.
		{
			for(ForwardIterator1 result = last1; ; )
			{
				const ForwardIterator1 resultNext(eastl::search<ForwardIterator1, ForwardIterator2, BinaryPredicate>(first1, last1, first2, last2, predicate));

				if(resultNext != last1) // If another sequence was found...
				{
					first1 = result = resultNext;
					++first1;
				}
				else
					return result;
			}
		}
		return last1;
	}

	template <typename BidirectionalIterator1, typename BidirectionalIterator2, typename BinaryPredicate>
	BidirectionalIterator1
	find_end_impl(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
				  BidirectionalIterator2 first2, BidirectionalIterator2 last2,
				  BinaryPredicate predicate,
				  EASTL_ITC_NS::bidirectional_iterator_tag, EASTL_ITC_NS::bidirectional_iterator_tag)
	{
		typedef eastl::reverse_iterator<BidirectionalIterator1> reverse_iterator1;
		typedef eastl::reverse_iterator<BidirectionalIterator2> reverse_iterator2;

		reverse_iterator1 rresult(eastl::search<reverse_iterator1, reverse_iterator2, BinaryPredicate>
											   (reverse_iterator1(last1), reverse_iterator1(first1),
												reverse_iterator2(last2), reverse_iterator2(first2),
												predicate));
		if(rresult.base() != first1) // If we found something...
		{
			BidirectionalIterator1 result(rresult.base());
			eastl::advance(result, -eastl::distance(first2, last2));
			return result;
		}
		return last1;
	}


	/// find_end
	///
	/// Effects: Finds a subsequence of equal values in a sequence.
	///
	/// Returns: The last iterator i in the range [first1, last1 - (last2 - first2))
	/// such that for any nonnegative integer n < (last2 - first2), the following
	/// corresponding conditions hold: pred(*(i+n),*(first2+n)) != false. Returns
	/// last1 if no such iterator is found.
	///
	/// Complexity: At most (last2 - first2) * (last1 - first1 - (last2 - first2) + 1)
	/// applications of the corresponding predicate.
	///
	template <typename ForwardIterator1, typename ForwardIterator2, typename BinaryPredicate>
	inline ForwardIterator1
	find_end(ForwardIterator1 first1, ForwardIterator1 last1,
			 ForwardIterator2 first2, ForwardIterator2 last2,
			 BinaryPredicate predicate)
	{
		typedef typename eastl::iterator_traits<ForwardIterator1>::iterator_category IC1;
		typedef typename eastl::iterator_traits<ForwardIterator2>::iterator_category IC2;

		return eastl::find_end_impl<ForwardIterator1, ForwardIterator2, BinaryPredicate>
								   (first1, last1, first2, last2, predicate, IC1(), IC2());
	}


	/// set_difference
	///
	/// set_difference iterates over both input ranges and copies elements present
	/// in the first range but not the second to the output range.
	///
	/// Effects: Copies the elements of the range [first1, last1) which are not
	/// present in the range [first2, last2) to the range beginning at result.
	/// The elements in the constructed range are sorted.
	///
	/// Requires: The input ranges must be sorted.
	/// Requires: The output range shall not overlap with either of the original ranges.
	///
	/// Returns: The end of the output range.
	///
	/// Complexity: At most (2 * ((last1 - first1) + (last2 - first2)) - 1) comparisons.
	///
	template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
	OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
								  InputIterator2 first2, InputIterator2 last2,
								  OutputIterator result)
	{
		while((first1 != last1) && (first2 != last2))
		{
			if(*first1 < *first2)
			{
				*result = *first1;
				++first1;
				++result;
			}
			else if(*first2 < *first1)
				++first2;
			else
			{
				++first1;
				++first2;
			}
		}

		return eastl::copy(first1, last1, result);
	}


	template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename Compare>
	OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
								  InputIterator2 first2, InputIterator2 last2,
								  OutputIterator result, Compare compare)
	{
		while((first1 != last1) && (first2 != last2))
		{
			if(compare(*first1, *first2))
			{
				EASTL_VALIDATE_COMPARE(!compare(*first2, *first1)); // Validate that the compare function is sane.
				*result = *first1;
				++first1;
				++result;
			}
			else if(compare(*first2, *first1))
			{
				EASTL_VALIDATE_COMPARE(!compare(*first1, *first2)); // Validate that the compare function is sane.
				++first2;
			}
			else
			{
				++first1;
				++first2;
			}
		}

		return eastl::copy(first1, last1, result);
	}


	/// set_difference_2
	///
	/// set_difference_2 iterates over both input ranges and copies elements present
	/// in the first range but not the second to the first output range and copies
	/// elements present in the second range but not in the first to the second output
	/// range.
	///
	/// Effects: Copies the elements of the range [first1, last1) which are not
	/// present in the range [first2, last2) to the first output range beginning at
	/// result1 AND copies the element of range [first2, last2) which are not present
	/// in the range [first1, last) to the second output range beginning at result2.
	/// The elements in the constructed range are sorted.
	///
	/// Requires: The input ranges must be sorted.
	/// Requires: The output ranges shall not overlap with either of the original ranges.
	///
	/// Returns:  Nothing.
	///
	/// Complexity: At most (2 * ((last1 - first1) + (last2 - first2)) - 1) comparisons.
	///
	template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename Compare>
	void set_difference_2(InputIterator1 first1, InputIterator1 last1,
	                      InputIterator2 first2, InputIterator2 last2,
	                      OutputIterator result1, OutputIterator result2, Compare compare)
	{
		while ((first1 != last1) && (first2 != last2))
		{
			if (compare(*first1, *first2))
			{
				EASTL_VALIDATE_COMPARE(!compare(*first2, *first1)); // Validate that the compare function is sane.
				*result1++ = *first1++;
			}
			else if (compare(*first2, *first1))
			{
				EASTL_VALIDATE_COMPARE(!compare(*first1, *first2)); // Validate that the compare function is sane.
				*result2++ = *first2++;
			}
			else
			{
				++first1;
				++first2;
			}
		}

		eastl::copy(first2, last2, result2);
		eastl::copy(first1, last1, result1);
	}

	/// set_difference_2
	///
	///  set_difference_2 with the default comparison object is eastl::less<>.
	///
	template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
	void set_difference_2(InputIterator1 first1, InputIterator1 last1,
	                      InputIterator2 first2, InputIterator2 last2,
	                      OutputIterator result1, OutputIterator result2)
	{
		eastl::set_difference_2(first1, last1, first2, last2, result1, result2, eastl::less<>{});
	}


	/// set_symmetric_difference
	///
	/// set_difference iterates over both input ranges and copies elements present
	/// in the either range but not the other to the output range.
	///
	/// Effects: Copies the elements of the range [first1, last1) which are not
	/// present in the range [first2, last2), and the elements of the range [first2, last2)
	/// which are not present in the range [first1, last1) to the range beginning at result.
	/// The elements in the constructed range are sorted.
	///
	/// Requires: The input ranges must be sorted.
	/// Requires: The resulting range shall not overlap with either of the original ranges.
	///
	/// Returns: The end of the constructed range.
	///
	/// Complexity: At most (2 * ((last1 - first1) + (last2 - first2)) - 1) comparisons.
	///
	template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
	OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
											InputIterator2 first2, InputIterator2 last2,
											OutputIterator result)
	{
		while((first1 != last1) && (first2 != last2))
		{
			if(*first1 < *first2)
			{
				*result = *first1;
				++first1;
				++result;
			}
			else if(*first2 < *first1)
			{
				*result = *first2;
				++first2;
				++result;
			}
			else
			{
				++first1;
				++first2;
			}
		}

		return eastl::copy(first2, last2, eastl::copy(first1, last1, result));
	}


	template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename Compare>
	OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
											InputIterator2 first2, InputIterator2 last2,
											OutputIterator result, Compare compare)
	{
		while((first1 != last1) && (first2 != last2))
		{
			if(compare(*first1, *first2))
			{
				EASTL_VALIDATE_COMPARE(!compare(*first2, *first1)); // Validate that the compare function is sane.
				*result = *first1;
				++first1;
				++result;
			}
			else if(compare(*first2, *first1))
			{
				EASTL_VALIDATE_COMPARE(!compare(*first1, *first2)); // Validate that the compare function is sane.
				*result = *first2;
				++first2;
				++result;
			}
			else
			{
				++first1;
				++first2;
			}
		}

		return eastl::copy(first2, last2, eastl::copy(first1, last1, result));
	}


	/// set_intersection
	///
	/// set_intersection over both ranges and copies elements present in
	/// both ranges to the output range.
	///
	/// Effects: Constructs a sorted intersection of the elements from the
	/// two ranges; that is, the set of elements that are present in both of the ranges.
	///
	/// Requires: The input ranges must be sorted.
	/// Requires: The resulting range shall not overlap with either of the original ranges.
	///
	/// Returns: The end of the constructed range.
	///
	/// Complexity: At most 2 * ((last1 - first1) + (last2 - first2)) - 1)  comparisons.
	///
	/// Note: The copying operation is stable; if an element is present in both ranges,
	/// the one from the first range is copied.
	///
	template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
	OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
									InputIterator2 first2, InputIterator2 last2,
									OutputIterator result)
	{
		while((first1 != last1) && (first2 != last2))
		{
			if(*first1 < *first2)
				++first1;
			else if(*first2 < *first1)
				++first2;
			else
			{
				*result = *first1;
				++first1;
				++first2;
				++result;
			}
		}

		return result;
	}


	template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename Compare>
	OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
									InputIterator2 first2, InputIterator2 last2,
									OutputIterator result, Compare compare)
	{
		while((first1 != last1) && (first2 != last2))
		{
			if(compare(*first1, *first2))
			{
				EASTL_VALIDATE_COMPARE(!compare(*first2, *first1)); // Validate that the compare function is sane.
				++first1;
			}
			else if(compare(*first2, *first1))
			{
				EASTL_VALIDATE_COMPARE(!compare(*first1, *first2)); // Validate that the compare function is sane.
				++first2;
			}
			else
			{
				*result = *first1;
				++first1;
				++first2;
				++result;
			}
		}

		return result;
	}



	/// set_union
	///
	/// set_union iterates over both ranges and copies elements present in
	/// both ranges to the output range.
	///
	/// Effects: Constructs a sorted union of the elements from the two ranges;
	/// that is, the set of elements that are present in one or both of the ranges.
	///
	/// Requires: The input ranges must be sorted.
	/// Requires: The resulting range shall not overlap with either of the original ranges.
	///
	/// Returns: The end of the constructed range.
	///
	/// Complexity: At most (2 * ((last1 - first1) + (last2 - first2)) - 1) comparisons.
	///
	/// Note: The copying operation is stable; if an element is present in both ranges,
	/// the one from the first range is copied.
	///
	template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
	OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
							 InputIterator2 first2, InputIterator2 last2,
							 OutputIterator result)
	{
		while((first1 != last1) && (first2 != last2))
		{
			if(*first1 < *first2)
			{
				*result = *first1;
				++first1;
			}
			else if(*first2 < *first1)
			{
				*result = *first2;
				++first2;
			}
			else
			{
				*result = *first1;
				++first1;
				++first2;
			}
			++result;
		}

		return eastl::copy(first2, last2, eastl::copy(first1, last1, result));
	}


	template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename Compare>
	OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
							 InputIterator2 first2, InputIterator2 last2,
							 OutputIterator result, Compare compare)
	{
		while((first1 != last1) && (first2 != last2))
		{
			if(compare(*first1, *first2))
			{
				EASTL_VALIDATE_COMPARE(!compare(*first2, *first1)); // Validate that the compare function is sane.
				*result = *first1;
				++first1;
			}
			else if(compare(*first2, *first1))
			{
				EASTL_VALIDATE_COMPARE(!compare(*first1, *first2)); // Validate that the compare function is sane.
				*result = *first2;
				++first2;
			}
			else
			{
				*result = *first1;
				++first1;
				++first2;
			}
			++result;
		}

		return eastl::copy(first2, last2, eastl::copy(first1, last1, result));
	}


	/// set_decomposition
	///
	/// set_decomposition iterates over both ranges and copies elements to one of the three
	/// categories of output ranges.
	///
	/// Effects: Constructs three sorted containers of the elements from the two ranges.
	///             * OutputIterator1 is elements only in Container1.
	///             * OutputIterator2 is elements only in Container2.
	///             * OutputIterator3 is elements that are in both Container1 and Container2.
	///
	/// Requires: The input ranges must be sorted.
	/// Requires: The resulting ranges shall not overlap with either of the original ranges.
	///
	/// Returns: The end of the constructed range of elements in both Container1 and Container2.
	///
	/// Complexity: At most (2 * ((last1 - first1) + (last2 - first2)) - 1) comparisons.
	///
	template <typename InputIterator1, typename InputIterator2,
	          typename OutputIterator1, typename OutputIterator2, typename OutputIterator3, typename Compare>
	OutputIterator3 set_decomposition(InputIterator1 first1, InputIterator1 last1,
	                                  InputIterator2 first2, InputIterator2 last2,
	                                  OutputIterator1 result1, OutputIterator2 result2, OutputIterator3 result3, Compare compare)
	{
		while ((first1 != last1) && (first2 != last2))
		{
			if (compare(*first1, *first2))
			{
				EASTL_VALIDATE_COMPARE(!compare(*first2, *first1)); // Validate that the compare function is sane.
				*result1++ = *first1++;
			}
			else if (compare(*first2, *first1))
			{
				EASTL_VALIDATE_COMPARE(!compare(*first1, *first2)); // Validate that the compare function is sane.
				*result2++ = *first2++;
			}
			else
			{
				*result3++ = *first1++;
				++first2;
			}
		}

		eastl::copy(first1, last1, result1);
		eastl::copy(first2, last2, result2);

		return result3;
	}

	/// set_decomposition
	///
	///  set_decomposition with the default comparison object is eastl::less<>.
	///
	template <typename InputIterator1, typename InputIterator2,
			  typename OutputIterator1, typename OutputIterator2, typename OutputIterator3>
	OutputIterator3 set_decomposition(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2,
							  OutputIterator1 result1, OutputIterator2 result2, OutputIterator3 result3)
	{
		return eastl::set_decomposition(first1, last1, first2, last2, result1, result2, result3, eastl::less<>{});
	}


	/// is_permutation
	///
	template<typename ForwardIterator1, typename ForwardIterator2>
	bool is_permutation(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2)
	{
		typedef typename eastl::iterator_traits<ForwardIterator1>::difference_type difference_type;

		// Skip past any equivalent initial elements.
		while((first1 != last1) && (*first1 == *first2))
		{
			++first1;
			++first2;
		}

		if(first1 != last1)
		{
			const difference_type first1Size = eastl::distance(first1, last1);
			ForwardIterator2 last2 = first2;
			eastl::advance(last2, first1Size);

			for(ForwardIterator1 i = first1; i != last1; ++i)
			{
				if(i == eastl::find(first1, i, *i))
				{
					const difference_type c = eastl::count(first2, last2, *i);

					if((c == 0) || (c != eastl::count(i, last1, *i)))
						return false;
				}
			}
		}

		return true;
	}

	/// is_permutation
	///
	template<typename ForwardIterator1, typename ForwardIterator2, class BinaryPredicate>
	bool is_permutation(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, BinaryPredicate predicate)
	{
		typedef typename eastl::iterator_traits<ForwardIterator1>::difference_type difference_type;

		// Skip past any equivalent initial elements.
		while((first1 != last1) && predicate(*first1, *first2))
		{
			++first1;
			++first2;
		}

		if(first1 != last1)
		{
			const difference_type first1Size = eastl::distance(first1, last1);
			ForwardIterator2 last2 = first2;
			eastl::advance(last2, first1Size);

			for(ForwardIterator1 i = first1; i != last1; ++i)
			{
				if(i == eastl::find(first1, i, *i, predicate))
				{
					const difference_type c = eastl::count(first2, last2, *i, predicate);

					if((c == 0) || (c != eastl::count(i, last1, *i, predicate)))
						return false;
				}
			}
		}

		return true;
	}


	/// next_permutation
	///
	/// mutates the range [first, last) to the next permutation. Returns true if the
	/// new range is not the final permutation (sorted like the starting permutation).
	/// Permutations start with a sorted range, and false is returned when next_permutation
	/// results in the initial sorted range, or if the range has <= 1 element.
	/// Note that elements are compared by operator < (as usual) and that elements deemed
	/// equal via this are not rearranged.
	///
	/// http://marknelson.us/2002/03/01/next-permutation/
	/// Basically we start with an ordered range and reverse it's order one specifically
	/// chosen swap and reverse at a time. It happens that this require going through every
	/// permutation of the range. We use the same variable names as the document above.
	///
	/// To consider: Significantly improved permutation/combination functionality:
	///    http://home.roadrunner.com/~hinnant/combinations.html
	///
	/// Example usage:
	///     vector<int> intArray;
	///     // <populate intArray>
	///     sort(intArray.begin(), intArray.end());
	///     do {
	///         // <do something with intArray>
	///     } while(next_permutation(intArray.begin(), intArray.end()));
	///

	template<typename BidirectionalIterator, typename Compare>
	bool next_permutation(BidirectionalIterator first, BidirectionalIterator last, Compare compare)
	{
		if(first != last) // If there is anything in the range...
		{
			BidirectionalIterator i = last;

			if(first != --i) // If the range has more than one item...
			{
				for(;;)
				{
					BidirectionalIterator ii(i), j;

					if(compare(*--i, *ii)) // Find two consecutive values where the first is less than the second.
					{
						j = last;
						while(!compare(*i, *--j)) // Find the final value that's greater than the first (it may be equal to the second).
							{}
						eastl::iter_swap(i, j);     // Swap the first and the final.
						eastl::reverse(ii, last);   // Reverse the ranget from second to last.
						return true;
					}

					if(i == first) // There are no two consecutive values where the first is less than the second, meaning the range is in reverse order. The reverse ordered range is always the last permutation.
					{
						eastl::reverse(first, last);
						break; // We are done.
					}
				}
			}
		}

		return false;
	}

	template<typename BidirectionalIterator>
	bool next_permutation(BidirectionalIterator first, BidirectionalIterator last)
	{
		typedef typename eastl::iterator_traits<BidirectionalIterator>::value_type value_type;

		return eastl::next_permutation(first, last, eastl::less<value_type>());
	}



	/// rotate
	///
	/// Effects: For each non-negative integer i < (last - first), places the element from the
	/// position first + i into position first + (i + (last - middle)) % (last - first).
	///
	/// Returns: first + (last - middle). That is, returns where first went to.
	///
	/// Remarks: This is a left rotate.
	///
	/// Requires: [first,middle) and [middle,last) shall be valid ranges. ForwardIterator shall
	/// satisfy the requirements of ValueSwappable (17.6.3.2). The type of *first shall satisfy
	/// the requirements of MoveConstructible (Table 20) and the requirements of MoveAssignable.
	///
	/// Complexity: At most last - first swaps.
	///
	/// Note: While rotate works on ForwardIterators (e.g. slist) and BidirectionalIterators (e.g. list),
	/// you can get much better performance (O(1) instead of O(n)) with slist and list rotation by
	/// doing splice operations on those lists instead of calling this rotate function.
	///
	/// http://www.cs.bell-labs.com/cm/cs/pearls/s02b.pdf / http://books.google.com/books?id=kse_7qbWbjsC&pg=PA14&lpg=PA14&dq=Programming+Pearls+flipping+hands
	/// http://books.google.com/books?id=tjOlkl7ecVQC&pg=PA189&lpg=PA189&dq=stepanov+Elements+of+Programming+rotate
	/// http://stackoverflow.com/questions/21160875/why-is-stdrotate-so-fast
	///
	/// Strategy:
	///     - We handle the special case of (middle == first) and (middle == last) no-ops
	///       up front in the main rotate entry point.
	///     - There's a basic ForwardIterator implementation (rotate_general_impl) which is
	///       a fallback implementation that's not as fast as others but works for all cases.
	///     - There's a slightly better BidirectionalIterator implementation.
	///     - We have specialized versions for rotating elements that are trivially copyable.
	///       These versions will use memmove for when we have a RandomAccessIterator.
	///     - We have a specialized version for rotating by only a single position, as that allows us
	///       (with any iterator type) to avoid a lot of logic involved with algorithms like "flipping hands"
	///       and achieve near optimal O(n) behavior. it turns out that rotate-by-one is a common use
	///       case in practice.
	///
	namespace Internal
	{
		template<typename ForwardIterator>
		ForwardIterator rotate_general_impl(ForwardIterator first, ForwardIterator middle, ForwardIterator last)
		{
			using eastl::swap;

			ForwardIterator current = middle;

			do {
				swap(*first++, *current++);

				if(first == middle)
					middle = current;
			} while(current != last);

			ForwardIterator result = first;
			current = middle;

			while(current != last)
			{
				swap(*first++, *current++);

				if(first == middle)
					middle = current;
				else if(current == last)
					current = middle;
			}

			return result; // result points to first + (last - middle).
		}


		template <typename ForwardIterator>
		ForwardIterator move_rotate_left_by_one(ForwardIterator first, ForwardIterator last)
		{
			typedef typename eastl::iterator_traits<ForwardIterator>::value_type value_type;

			value_type temp(eastl::move(*first));
			ForwardIterator result = eastl::move(eastl::next(first), last, first); // Note that while our template type is BidirectionalIterator, if the actual
			*result = eastl::move(temp);                                           // iterator is a RandomAccessIterator then this move will be a memmove for trivially copyable types.

			return result; // result points to the final element in the range.
		}


		template <typename BidirectionalIterator>
		BidirectionalIterator move_rotate_right_by_one(BidirectionalIterator first, BidirectionalIterator last)
		{
			typedef typename eastl::iterator_traits<BidirectionalIterator>::value_type value_type;

			BidirectionalIterator beforeLast = eastl::prev(last);
			value_type temp(eastl::move(*beforeLast));
			BidirectionalIterator result = eastl::move_backward(first, beforeLast, last); // Note that while our template type is BidirectionalIterator, if the actual
			*first = eastl::move(temp);                                                   // iterator is a RandomAccessIterator then this move will be a memmove for trivially copyable types.

			return result; // result points to the first element in the range.
		}

		template <typename /*IteratorCategory*/, bool /*is_trivially_copyable*/>
		struct rotate_helper
		{
			template <typename ForwardIterator>
			static ForwardIterator rotate_impl(ForwardIterator first, ForwardIterator middle, ForwardIterator last)
				{ return Internal::rotate_general_impl(first, middle, last); }
		};

		template <>
		struct rotate_helper<EASTL_ITC_NS::forward_iterator_tag, true>
		{
			template <typename ForwardIterator>
			static ForwardIterator rotate_impl(ForwardIterator first, ForwardIterator middle, ForwardIterator last)
			{
				if(eastl::next(first) == middle) // If moving trivial types by a single element, memcpy is fast for that case.
					return Internal::move_rotate_left_by_one(first, last);
				return Internal::rotate_general_impl(first, middle, last);
			}
		};

		template <>
		struct rotate_helper<EASTL_ITC_NS::bidirectional_iterator_tag, false>
		{
			template <typename BidirectionalIterator>
			static BidirectionalIterator rotate_impl(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last)
				{ return Internal::rotate_general_impl(first, middle, last); } // rotate_general_impl outperforms the flipping hands algorithm.

			/*
			// Simplest "flipping hands" implementation. Disabled because it's slower on average than rotate_general_impl.
			template <typename BidirectionalIterator>
			static BidirectionalIterator rotate_impl(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last)
			{
				eastl::reverse(first, middle);
				eastl::reverse(middle, last);
				eastl::reverse(first, last);
				return first + (last - middle); // This can be slow for large ranges because operator + and - are O(n).
			}

			// Smarter "flipping hands" implementation, but still disabled because benchmarks are showing it to be slower than rotate_general_impl.
			template <typename BidirectionalIterator>
			static BidirectionalIterator rotate_impl(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last)
			{
				// This is the "flipping hands" algorithm.
				eastl::reverse_impl(first,  middle, EASTL_ITC_NS::bidirectional_iterator_tag()); // Reverse the left side.
				eastl::reverse_impl(middle, last,   EASTL_ITC_NS::bidirectional_iterator_tag()); // Reverse the right side.

				// Reverse the entire range.
				while((first != middle) && (middle != last))
				{
					eastl::iter_swap(first, --last);
					++first;
				}

				if(first == middle) // Finish reversing the entire range.
				{
					eastl::reverse_impl(middle, last, bidirectional_iterator_tag());
					return last;
				}
				else
				{
					eastl::reverse_impl(first, middle, bidirectional_iterator_tag());
					return first;
				}
			}
			*/
		};

		template <>
		struct rotate_helper<EASTL_ITC_NS::bidirectional_iterator_tag, true>
		{
			template <typename BidirectionalIterator>
			static BidirectionalIterator rotate_impl(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last)
			{
				if(eastl::next(first) == middle) // If moving trivial types by a single element, memcpy is fast for that case.
					return Internal::move_rotate_left_by_one(first, last);
				if(eastl::next(middle) == last)
					return Internal::move_rotate_right_by_one(first, last);
				return Internal::rotate_general_impl(first, middle, last);
			}
		};

		template <typename Integer>
		inline Integer greatest_common_divisor(Integer x, Integer y)
		{
			do {
				Integer t = (x % y);
				x = y;
				y = t;
			} while(y);

			return x;
		}

		template <>
		struct rotate_helper<EASTL_ITC_NS::random_access_iterator_tag, false>
		{
			// This is the juggling algorithm, using move operations.
			// In practice this implementation is about 25% faster than rotate_general_impl. We may want to
			// consider sticking with just rotate_general_impl and avoid the code generation of this function.
			template <typename RandomAccessIterator>
			static RandomAccessIterator rotate_impl(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last)
			{
				typedef typename iterator_traits<RandomAccessIterator>::difference_type difference_type;
				typedef typename iterator_traits<RandomAccessIterator>::value_type value_type;

				const difference_type m1 = (middle - first);
				const difference_type m2 = (last - middle);
				const difference_type g  = Internal::greatest_common_divisor(m1, m2);
				value_type temp;

				for(RandomAccessIterator p = first + g; p != first;)
				{
					temp = eastl::move(*--p);
					RandomAccessIterator p1 = p;
					RandomAccessIterator p2 = p + m1;
					do
					{
						*p1 = eastl::move(*p2);
						p1 = p2;
						const difference_type d = (last - p2);

						if(m1 < d)
							p2 += m1;
						else
							p2 = first + (m1 - d);
					} while(p2 != p);

					*p1 = eastl::move(temp);
				}

				return first + m2;
			}
		};

		template <>
		struct rotate_helper<EASTL_ITC_NS::random_access_iterator_tag, true>
		{
			// Experiments were done which tested the performance of using an intermediate buffer
			// to do memcpy's to as opposed to executing a swapping algorithm. It turns out this is
			// actually slower than even rotate_general_impl, partly because the average case involves
			// memcpy'ing a quarter of the element range twice. Experiments were done with various kinds
			// of PODs with various element counts.

			template <typename RandomAccessIterator>
			static RandomAccessIterator rotate_impl(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last)
			{
				if(eastl::next(first) == middle) // If moving trivial types by a single element, memcpy is fast for that case.
					return Internal::move_rotate_left_by_one(first, last);
				if(eastl::next(middle) == last)
					return Internal::move_rotate_right_by_one(first, last);
				if((last - first) < 32) // For small ranges rotate_general_impl is faster.
					return Internal::rotate_general_impl(first, middle, last);
				return Internal::rotate_helper<EASTL_ITC_NS::random_access_iterator_tag, false>::rotate_impl(first, middle, last);
			}
		};

	} // namespace Internal


	template <typename ForwardIterator>
	ForwardIterator rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last)
	{
		if(middle != first)
		{
			if(middle != last)
			{
				typedef typename eastl::iterator_traits<ForwardIterator>::iterator_category IC;
				typedef typename eastl::iterator_traits<ForwardIterator>::value_type        value_type;

				// the implementations for is_trivially_copyable types simply check whether we have a single element to rotate and if so,
				// defer to either move_rotate_left_by_one or move_rotate_right_by_one, which are optimized for trivially copyable types.
				// otherwise, use the same implementation as non-trivially copyable types.
				return Internal::rotate_helper<IC, eastl::is_trivially_copyable<value_type>::value>::rotate_impl(first, middle, last);
			}

			return first;
		}

		return last;
	}



	/// rotate_copy
	///
	/// Similar to rotate except writes the output to the OutputIterator and
	/// returns an OutputIterator to the element past the last element copied
	/// (i.e. result + (last - first))
	///
	template <typename ForwardIterator, typename OutputIterator>
	OutputIterator rotate_copy(ForwardIterator first, ForwardIterator middle, ForwardIterator last, OutputIterator result)
	{
		return eastl::copy(first, middle, eastl::copy(middle, last, result));
	}



	/// clamp
	///
	/// Returns a reference to a clamped value within the range of [lo, hi].
	///
	/// http://en.cppreference.com/w/cpp/algorithm/clamp
	///
	template <class T, class Compare>
	EA_CONSTEXPR const T& clamp(const T& v, const T& lo, const T& hi, Compare comp)
	{
		EASTL_ASSERT(!comp(hi, lo));
		return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
	}

	template <class T>
	EA_CONSTEXPR const T& clamp(const T& v, const T& lo, const T& hi)
	{
		return eastl::clamp(v, lo, hi, eastl::less<>());
	}


	/// is_partitioned
	///
	/// Returns true if all the elements in the range [first, last) is empty, or is
	/// partitioned by predicate. Being partitioned means that all elements v for which
	/// predicate(v) evaluates to true appear before any elements for which predicate(v)
	/// is false.
	///
	template <class InputIterator, class UnaryPredicate>
	EA_CONSTEXPR bool is_partitioned(InputIterator first, InputIterator last, UnaryPredicate predicate)
	{
		for (; first != last; ++first)
		{
			if (!predicate(*first))
			{
				// advance the iterator, we don't need to call the predicate on this item
				// again in the "false" loop below.
				++first;
				break;
			}
		}
		for (; first != last; ++first)
		{
			if (predicate(*first))
			{
				return false;
			}
		}
		return true;
	}

	/// partition_point
	///
	/// Precondition: for this function to work correctly the input range [first, last)
	/// must be partitioned by the predicate. i.e. all values for which predicate(v) is
	/// true should precede any value in the range for which predicate(v) is false.
	///
	/// Returns: the iterator past the end of the first partition within [first, last) or
	/// last if all elements satisfy the predicate.
	///
	/// Note: this is a more general version of lower_bound.
	template <class ForwardIterator, class UnaryPredicate>
	EA_CONSTEXPR ForwardIterator partition_point(ForwardIterator first, ForwardIterator last, UnaryPredicate predicate)
	{
		// Just binary chop our way to the first one where predicate(x) is false
		for (auto length = eastl::distance(first, last); 0 < length;)
		{
			const auto half = length / 2;
			const auto middle = eastl::next(first, half);
			if (predicate(*middle))
			{
				first = eastl::next(middle);
				length -= (half + 1);
			}
			else
			{
				length = half;
			}
		}

		return first;
	}

} // namespace eastl


#endif // Header include guard
