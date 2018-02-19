///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file defines numeric algorithms just like the std C++ <numeric> 
// algorithm header does. 
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_NUMERIC_H
#define EASTL_NUMERIC_H


#include <EASTL/internal/config.h>
#include <EASTL/iterator.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// accumulate
	///
	/// Accumulates the values in the range [first, last) using operator+.  
	/// The initial value is init. The values are processed in order.
	///
	template <typename InputIterator, typename T>
	T accumulate(InputIterator first, InputIterator last, T init)
	{
		// The C++ standard specifies that we use (init = init + first).
		// However, for non-built-in types, this is less efficent than 
		// operator +=, as no temporary is created. Until a serious problem 
		// is found with using operator +=, we'll use it.

		for(; first != last; ++first)
			init += *first;
		return init;
	}


	/// accumulate
	///
	/// Accumulates the values in the range [first, last) using binary_op.  
	/// The initial value is init. The values are processed in order.
	///
	template <typename InputIterator, typename T, typename BinaryOperation>
	T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op)
	{
		for(; first != last; ++first)
			init = binary_op(init, *first);
		return init;
	}



	/// iota
	///
	/// Requires: T shall be convertible to ForwardIterator's value type. The expression ++val, 
	/// where val has type T, shall be well formed.
	/// Effects: For each element referred to by the iterator i in the range [first, last), 
	/// assigns *i = value and increments value as if by ++value.
	/// Complexity: Exactly last - first increments and assignments.
	/// Example usage: seeding a deck of cards with values 0-51.
	///
	template <typename ForwardIterator, typename T>
	void iota(ForwardIterator first, ForwardIterator last, T value)
	{
		while(first != last)
		{
			*first++ = value;
			++value;
		}
	}


	/// inner_product
	///
	/// Starting with an initial value of init, multiplies successive
	/// elements from the two ranges and adds each product into the accumulated
	/// value using operator+. The values in the ranges are processed in order.
	/// 
	template <typename InputIterator1, typename InputIterator2, typename T>
	T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init)
	{
		// The C++ standard specifies that we use (init = init + (*first1 * *first2)).
		// However, for non-built-in types, this is less efficent than 
		// operator +=, as no temporary is created. Until a serious problem 
		// is found with using operator +=, we'll use it.

		for(; first1 != last1; ++first1, ++first2)
			init += (*first1 * *first2);
		return init;
	}


	/// inner_product
	///
	/// Starting with an initial value of init, applies binary_op2 to
	/// successive elements from the two ranges and accumulates each result 
	/// into the accumulated value using binary_op1. The values in the 
	/// ranges are processed in order.
	/// 
	template <typename InputIterator1, typename InputIterator2, typename T, typename BinaryOperation1, typename BinaryOperation2>
	T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init, 
					BinaryOperation1 binary_op1, BinaryOperation2 binary_op2)
	{
		for(; first1 != last1; ++first1, ++first2)
			init = binary_op1(init, binary_op2(*first1, *first2));
		return init;
	}





	/// partial_sum
	///
	/// Accumulates the values in the range [first, last) using operator+.
	/// As each successive input value is added into the total, that partial 
	/// sum is written to result. Therefore, the first value in result is the
	/// first value of the input, the second value in result is the sum of the
	/// first and second input values, and so on.
	/// 
	template <typename InputIterator, typename OutputIterator>
	OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result)
	{
		typedef typename iterator_traits<InputIterator>::value_type value_type;

		if(first != last)
		{
			value_type value(*first);
			
			for(*result = value; ++first != last; *++result = value)
				value += *first; // See discussions above on the decision use += instead of +.

			++result;
		}

		return result;
	}


	/// partial_sum
	///
	/// Accumulates the values in the range [first,last) using binary_op.
	/// As each successive input value is added into the total, that partial 
	/// sum is written to result. Therefore, the first value in result is the
	/// first value of the input, the second value in result is the sum of the
	/// first and second input values, and so on.

	template <typename InputIterator, typename OutputIterator, typename BinaryOperation>
	OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result, BinaryOperation binary_op)
	{
		typedef typename iterator_traits<InputIterator>::value_type value_type;

		if(first != last)
		{
			value_type value(*first);

			for(*result = value; ++first != last; *++result = value)
				value = binary_op(value, *first);

			++result;
		}

		return result;
	}





	/// adjacent_difference
	///
	/// Computes the difference between adjacent values in the range
	/// [first, last) using operator- and writes the result to result.
	/// 
	template <typename InputIterator, typename OutputIterator>
	OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result)
	{
		typedef typename iterator_traits<InputIterator>::value_type value_type;

		if(first != last)
		{
			value_type value(*first);

			for(*result = value; ++first != last; )
			{
				const value_type temp(*first);

				*++result = temp - value;
				value = temp;
			}

			++result;
		}

		return result;
	}


	/// adjacent_difference
	///
	/// Computes the difference between adjacent values in the range
	/// [first, last) using binary_op and writes the result to result.
	/// 
	template <typename InputIterator, typename OutputIterator, typename BinaryOperation>
	OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result, BinaryOperation binary_op)
	{
		typedef typename iterator_traits<InputIterator>::value_type value_type;

		if(first != last)
		{
			value_type value(*first);
			
			for(*result = value; ++first != last; )
			{
				const value_type temp(*first);

				*++result = binary_op(temp, value);
				value = temp;
			}

			++result;
		}

		return result;
	}



} // namespace eastl


#endif // Header include guard







