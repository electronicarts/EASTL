///////////////////////////////////////////////////////////////////////////////
// EASprintf.h
//
// Copyright (c) Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTDC_EAALIGNMENT_H
#define EASTDC_EAALIGNMENT_H

#include <EABase/eabase.h>

#define EASTDC_API
#define EASTDC_LOCAL

namespace EA {
namespace StdC {


	/// IsAligned
	/// 
	/// Tells if a given integer is aligned to a given power-of-2 boundary.
	/// Returns true for an input x value of 0, regardless of the value of a.
	/// The template <a> value must be >= 1.
	/// Example:
	///    IsAligned<int, 8>(64)  ->  true
	///    IsAligned<int, 8>(67)  ->  false
	///
	/// To consider: wouldn't it be better if the template arguments were reversed?
	///
	template <typename T, int a>
	inline bool IsAligned(T x)
	{
		return (x & (a - 1)) == 0;
	}

	template <typename T, int a>
	inline bool IsAligned(T* p)
	{
		return ((uintptr_t)p & (a - 1)) == 0;
	}


	/// IsAligned
	/// 
	/// Tells if a given integer is aligned to a given power-of-2 boundary.
	/// Returns true for an input x value of 0, regardless of the value of a.
	/// The alignment value a must be >= 1.
	/// Example:
	///    IsAligned(64, 8)  ->  true
	///    IsAligned(67, 8)  ->  false
	///
	template <typename T>
	inline bool IsAligned(T x, size_t a)
	{
		return (x & (a - 1)) == 0;
	}

	template <typename T>
	inline bool IsAligned(T* p, size_t a)
	{
		return ((uintptr_t)p & (a - 1)) == 0;
	}


	/// GetAlignment
	/// 
	/// Returns the highest power-of-two alignment of the given value x.
	/// x must be a scalar value (integer or pointer), else the results are undefined.
	/// Returns 0 for an input a value of 0.
	/// Beware that GetAlignment returns the highest power-of-two alignment, which 
	/// may result in a return value that is higher than you expect. Consider using
	/// the IsAligned functions to test for a specific alignment.
	/// Example:
	///    GetAlignment(0)  ->  0
	///    GetAlignment(1)  ->  1
	///    GetAlignment(2)  ->  2
	///    GetAlignment(3)  ->  1
	///    GetAlignment(4)  ->  4
	///    GetAlignment(5)  ->  1
	///    GetAlignment(6)  ->  2
	///    GetAlignment(7)  ->  1
	///    GetAlignment(8)  ->  8
	///    GetAlignment(9)  ->  1
	template <typename T>
	inline size_t GetAlignment(T x)
	{
		return (size_t)((x ^ (x - 1)) >> 1) + 1;
	}

	template <typename T>
	inline size_t GetAlignment(T* p)
	{
		return (size_t)(((uintptr_t)p ^ ((uintptr_t)p - 1)) >> 1) + 1;
	}


}} // namespace EA::StdC

#endif  // EASTDC_EAALIGNMENT_H
