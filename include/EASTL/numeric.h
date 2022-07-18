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


	#if defined(EA_COMPILER_CPP20_ENABLED)
	/// midpoint
	///
	/// Computes the midpoint between the LHS and RHS by adding them together, then dividing the sum by 2.
	/// If the operands are of integer type and the sum is odd, the result will be rounded closer to the LHS.
	/// If the operands are floating points, then at most one inexact operation occurs.
	///
	template <typename T>
	constexpr eastl::enable_if_t<eastl::is_arithmetic_v<T> && !eastl::is_same_v<eastl::remove_cv_t<T>, bool>, T> midpoint(const T lhs, const T rhs) EA_NOEXCEPT
	{
		// If T is an integral type...
		if constexpr(eastl::is_integral_v<T>)
		{
			using U = eastl::make_unsigned_t<T>;

			int sign = 1;
			U m = lhs;
			U M = rhs;

			if (lhs > rhs)
			{
				sign = -1;
				m = rhs;
				M = lhs;
			}

			return lhs + static_cast<T>(sign * static_cast<T>((U(M - m)) / 2 ));
		}

		// otherwise if T is a floating point
		else
		{
			const T LO = eastl::numeric_limits<T>::min() * 2;
			const T HI = eastl::numeric_limits<T>::max() / 2;

			const T lhs_abs = (lhs < 0) ? -lhs : lhs;
			const T rhs_abs = (rhs < 0) ? -rhs : rhs;

			if (lhs_abs <= HI && rhs_abs <= HI)
				return (lhs + rhs) / 2;
			if (lhs_abs < LO)
				return lhs + (rhs / 2);
			if (rhs_abs < LO)
				return (lhs / 2) + rhs;
			return (lhs / 2) + (rhs / 2);
		}
	}


	/// midpoint
	///
	/// Computes the midpoint address between pointers LHS and RHS.
	/// The midpoint address closer to the LHS is chosen.
	///
	template <typename T>
	constexpr eastl::enable_if_t<eastl::is_object_v<T>, T*> midpoint(T* lhs, T* rhs)
	{
		return lhs + ((rhs - lhs) / 2);
	}


	template <class T>
	constexpr T shared_lerp(const T a, const T b, const T t) EA_NOEXCEPT
	{
		if ((a <= 0 && b >= 0) || (a >= 0 && b <= 0))
		{
			return t * b + (1 - t) * a;
		}

		if (t == 1)
		{
			return b;
		}

		const T X = a + t * (b - a);

		if ((t > 1) == (b > a))
		{
			return (b > X) ? b : X;
		}
		return (b < X) ? b : X;
	}

	/// lerp
	///
	/// Calculates the linear interpolation of two points A and B expressed A + T * (B - A)
	/// where T is some value in range [0, 1]. If T is outside this range, the linear extrapolation will be computed.
	///
	/// https://en.cppreference.com/w/cpp/numeric/lerp
	///
	/// C++ proposal paper:
	/// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0811r3.html
	///
	constexpr float lerp(float a, float b, float t) EA_NOEXCEPT { return shared_lerp(a, b, t); }
	constexpr double lerp(double a, double b, double t) EA_NOEXCEPT { return shared_lerp(a, b, t); }
	constexpr long double lerp(long double a, long double b, long double t) EA_NOEXCEPT { return shared_lerp(a, b, t); }
	#endif

} // namespace eastl


#endif // Header include guard







