/*-----------------------------------------------------------------------------
 * eaint128_t.h
 *
 * Copyright (c) Electronic Arts Inc. All rights reserved.
 *---------------------------------------------------------------------------*/


#ifndef INCLUDED_int128_h
#define INCLUDED_int128_h


///////////////////////////////////////////////////////////////////////////////////////////////////////
// EA_INT128_INTRINSIC_AVAILABLE
//
#if (EA_COMPILER_INTMAX_SIZE >= 16) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
	// __int128_t/__uint128_t is supported
	#define EA_INT128_INTRINSIC_AVAILABLE 1 
#else
	#define EA_INT128_INTRINSIC_AVAILABLE 0
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////
// EA_INT128_ALIGNAS
//
#if EA_INT128_INTRINSIC_AVAILABLE && !defined(EA_COMPILER_NO_ALIGNAS)
	#define EA_INT128_ALIGNAS alignas(unsigned __int128)
#else
	#define EA_INT128_ALIGNAS
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////
// EA_HAVE_INT128
//
// Indicates that EABase implements 128-bit integer types
//
#define EA_HAVE_INT128 1


///////////////////////////////////////////////////////////////////////////////////////////////////////
// uint128_t_base
//
struct EA_INT128_ALIGNAS int128_t_base
{
	// Constructors / destructors
	int128_t_base() = default;
	int128_t_base(uint32_t nPart0, uint32_t nPart1, uint32_t nPart2, uint32_t nPart3);
	int128_t_base(uint64_t nPart0, uint64_t nPart1);
	int128_t_base(uint8_t value);
	int128_t_base(uint16_t value);
	int128_t_base(uint32_t value);
	int128_t_base(uint64_t value);
	int128_t_base(const int128_t_base& value) = default;

	// Assignment operator
	int128_t_base& operator=(const int128_t_base& value) = default;

	// Explicit operators to convert back to basic types
	EA_CONSTEXPR explicit operator bool() const;
	EA_CONSTEXPR explicit operator char() const;
	EA_CONSTEXPR explicit operator int() const;
	EA_CONSTEXPR explicit operator long() const; 
	EA_CONSTEXPR explicit operator long long() const;
	EA_CONSTEXPR explicit operator short() const; 
	EA_CONSTEXPR explicit operator signed char() const;
	EA_CONSTEXPR explicit operator unsigned char() const;
	EA_CONSTEXPR explicit operator unsigned int() const;
	EA_CONSTEXPR explicit operator unsigned long long() const;
	EA_CONSTEXPR explicit operator unsigned long() const;
	EA_CONSTEXPR explicit operator unsigned short() const;
#if EA_WCHAR_UNIQUE
	// EA_CONSTEXPR explicit operator char16_t() const;
	// EA_CONSTEXPR explicit operator char32_t() const;
	// EA_CONSTEXPR explicit operator wchar_t() const;
#endif
	EA_CONSTEXPR explicit operator float() const;
	EA_CONSTEXPR explicit operator double() const;
	EA_CONSTEXPR explicit operator long double() const;
#if EA_INT128_INTRINSIC_AVAILABLE
	EA_CONSTEXPR explicit operator __int128() const;
	EA_CONSTEXPR explicit operator unsigned __int128() const;
#endif

	// Math operators
	static void OperatorPlus (const int128_t_base& value1, const int128_t_base& value2, int128_t_base& result);
	static void OperatorMinus(const int128_t_base& value1, const int128_t_base& value2, int128_t_base& result);
	static void OperatorMul  (const int128_t_base& value1, const int128_t_base& value2, int128_t_base& result);

	// Shift operators
	static void OperatorShiftRight(const int128_t_base& value, int nShift, int128_t_base& result);
	static void OperatorShiftLeft (const int128_t_base& value, int nShift, int128_t_base& result);

	// Unary arithmetic/logic operators
	bool operator!() const;

	// Logical operators
	static void OperatorXOR(const int128_t_base& value1, const int128_t_base& value2, int128_t_base& result);
	static void OperatorOR (const int128_t_base& value1, const int128_t_base& value2, int128_t_base& result);
	static void OperatorAND(const int128_t_base& value1, const int128_t_base& value2, int128_t_base& result);

	bool     IsZero() const;
	void     SetZero();
	void     TwosComplement();
	void     InverseTwosComplement();

	int      GetBit(int nIndex) const;
	void     SetBit(int nIndex, int value);

protected:
	void DoubleToUint128(double value);

	EA_CONSTEXPR uint64_t Low() const
	{
		return mPart0;
	}

	EA_CONSTEXPR uint64_t High() const
	{
		return mPart1;
	}

protected:
	#ifdef EA_SYSTEM_BIG_ENDIAN
		uint64_t mPart1;  // Most significant byte.
		uint64_t mPart0;  // Least significant byte.
	#else
		uint64_t mPart0;  // Most significant byte.
		uint64_t mPart1;  // Least significant byte.
	#endif
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// int128_t
//
// Implements signed 128 bit integer.
//
struct int128_t : public int128_t_base
{
	// Constructors / destructors
	using int128_t_base::int128_t_base;

	// Assignment operator
	using int128_t_base::operator=;

	// Unary arithmetic/logic operators
	int128_t  operator-() const;
	int128_t& operator++();
	int128_t& operator--();
	int128_t  operator++(int);
	int128_t  operator--(int);
	int128_t  operator~() const;
	int128_t  operator+() const;

	// Math operators
	int128_t  operator+ (const int128_t& other);
	int128_t  operator- (const int128_t& other);
	int128_t  operator* (const int128_t& other);
	int128_t  operator/ (const int128_t& other);
	int128_t  operator% (const int128_t& other);
	int128_t& operator+=(const int128_t& other);
	int128_t& operator-=(const int128_t& other);
	int128_t& operator*=(const int128_t& other);
	int128_t& operator/=(const int128_t& other);
	int128_t& operator%=(const int128_t& other);

	// Shift operators
	int128_t  operator>> (int nShift) const;
	int128_t  operator<< (int nShift) const;
	int128_t& operator>>=(int nShift);
	int128_t& operator<<=(int nShift);

	// Logical operators
	int128_t  operator^ (const int128_t& other) const;
	int128_t  operator| (const int128_t& other) const;
	int128_t  operator& (const int128_t& other) const;
	int128_t& operator^=(const int128_t& other);
	int128_t& operator|=(const int128_t& other);
	int128_t& operator&=(const int128_t& other);

	// Equality operators
	bool operator==(const int128_t& other) const;
	bool operator!=(const int128_t& other) const;
	bool operator> (const int128_t& other) const;
	bool operator>=(const int128_t& other) const;
	bool operator< (const int128_t& other) const;
	bool operator<=(const int128_t& other) const;

protected:
	int compare(const int128_t& other) const;
	void Negate();
	void Modulus(const int128_t& divisor, int128_t& quotient, int128_t& remainder) const;
	bool IsNegative() const;    // Returns true for value <  0
	bool IsPositive() const;    // Returns true for value >= 0
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
// uint128_t
//
// Implements unsigned 128 bit integer.
//
struct uint128_t : public int128_t_base
{
	// Constructors / destructors
	using int128_t_base::int128_t_base;

	// Assignment operator
	using int128_t_base::operator=;

	// Unary arithmetic/logic operators
	uint128_t  operator-() const;
	uint128_t& operator++();
	uint128_t& operator--();
	uint128_t  operator++(int);
	uint128_t  operator--(int);
	uint128_t  operator~() const;
	uint128_t  operator+() const;

	// Math operators
	uint128_t  operator+ (const uint128_t& other);
	uint128_t  operator- (const uint128_t& other);
	uint128_t  operator* (const uint128_t& other);
	uint128_t  operator/ (const uint128_t& other);
	uint128_t  operator% (const uint128_t& other);
	uint128_t& operator+=(const uint128_t& other);
	uint128_t& operator-=(const uint128_t& other);
	uint128_t& operator*=(const uint128_t& other);
	uint128_t& operator/=(const uint128_t& other);
	uint128_t& operator%=(const uint128_t& other);

	// Shift operators
	uint128_t  operator>> (int nShift) const;
	uint128_t  operator<< (int nShift) const;
	uint128_t& operator>>=(int nShift);
	uint128_t& operator<<=(int nShift);

	// Logical operators
	uint128_t  operator^ (const uint128_t& other) const;
	uint128_t  operator| (const uint128_t& other) const;
	uint128_t  operator& (const uint128_t& other) const;
	uint128_t& operator^=(const uint128_t& other);
	uint128_t& operator|=(const uint128_t& other);
	uint128_t& operator&=(const uint128_t& other);

	// Equality operators
	bool operator==(const uint128_t& other) const;
	bool operator!=(const uint128_t& other) const;
	bool operator> (const uint128_t& other) const;
	bool operator>=(const uint128_t& other) const;
	bool operator< (const uint128_t& other) const;
	bool operator<=(const uint128_t& other) const;

protected:
	int  compare(const uint128_t& other) const;
	void Negate();
	void Modulus(const uint128_t& divisor, uint128_t& quotient, uint128_t& remainder) const;
	bool IsNegative() const;    // Returns true for value <  0
	bool IsPositive() const;    // Returns true for value >= 0
};



///////////////////////////////////////////////////////////////////////////////////////////////////////
// uint128_t_base implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////
EA_CONSTEXPR inline int128_t_base::operator bool() const               { return mPart0 || mPart1; }
EA_CONSTEXPR inline int128_t_base::operator char() const               { return static_cast<char>(Low()); }
#if EA_WCHAR_UNIQUE
// EA_CONSTEXPR inline int128_t_base::operator char16_t() const           { return static_cast<char16_t>(Low()); }
// EA_CONSTEXPR inline int128_t_base::operator char32_t() const           { return static_cast<char32_t>(Low()); }
// EA_CONSTEXPR inline int128_t_base::operator wchar_t() const            { return static_cast<wchar_t>(Low()); }
#endif
EA_CONSTEXPR inline int128_t_base::operator int() const                { return static_cast<int>(Low()); }
EA_CONSTEXPR inline int128_t_base::operator long() const               { return static_cast<long>(Low()); }
EA_CONSTEXPR inline int128_t_base::operator long long() const          { return static_cast<long long>(Low()); }
EA_CONSTEXPR inline int128_t_base::operator short() const              { return static_cast<short>(Low()); }
EA_CONSTEXPR inline int128_t_base::operator signed char() const        { return static_cast<signed char>(Low()); }
EA_CONSTEXPR inline int128_t_base::operator unsigned char() const      { return static_cast<unsigned char>(Low()); }
EA_CONSTEXPR inline int128_t_base::operator unsigned int() const       { return static_cast<unsigned int>(Low()); }
EA_CONSTEXPR inline int128_t_base::operator unsigned long long() const { return static_cast<unsigned long long>(Low()); }
EA_CONSTEXPR inline int128_t_base::operator unsigned long() const	   { return static_cast<unsigned long>(Low()); }
EA_CONSTEXPR inline int128_t_base::operator unsigned short() const     { return static_cast<unsigned short>(Low()); }
EA_CONSTEXPR inline int128_t_base::operator float() const              { return static_cast<float>(Low()); }
EA_CONSTEXPR inline int128_t_base::operator double() const             { return static_cast<double>(Low()); }
EA_CONSTEXPR inline int128_t_base::operator long double() const        { return static_cast<long double>(Low()); }
#if EA_INT128_INTRINSIC_AVAILABLE
EA_CONSTEXPR inline int128_t_base::operator __int128() const           { return static_cast<__int128>(Low()); }
EA_CONSTEXPR inline int128_t_base::operator unsigned __int128() const  { return static_cast<unsigned __int128>(Low()); }
#endif

inline void int128_t_base::SetBit(int nIndex, int value)
{
	// EA_ASSERT((nIndex >= 0) && (nIndex < 128));

	const uint64_t nBitMask = ((uint64_t)1 << (nIndex % 64));

	if(nIndex < 64)
	{
		if(value)
			mPart0 = mPart0 |  nBitMask;
		else
			mPart0 = mPart0 & ~nBitMask;
	}
	else if(nIndex < 128)
	{
		if(value)
			mPart1 = mPart1 |  nBitMask;
		else
			mPart1 = mPart1 & ~nBitMask;
	}
}

inline int int128_t_base::GetBit(int nIndex) const
{
	// EA_ASSERT((nIndex >= 0) && (nIndex < 128));

	const uint64_t nBitMask = ((uint64_t)1 << (nIndex % 64));

	if(nIndex < 64)
		return ((mPart0 & nBitMask) ? 1 : 0);
	else if(nIndex < 128)
		return ((mPart1 & nBitMask) ? 1 : 0);
	return 0;
}

inline int128_t_base::int128_t_base(uint32_t nPart0, uint32_t nPart1, uint32_t nPart2, uint32_t nPart3)
{
	mPart1 = ((uint64_t)nPart3 << 32) + nPart2;
	mPart0 = ((uint64_t)nPart1 << 32) + nPart0;
}

inline int128_t_base::int128_t_base(uint64_t nPart0, uint64_t nPart1)
{
	mPart1 = nPart1;
	mPart0 = nPart0;
}

inline int128_t_base::int128_t_base(uint8_t value)
{
	mPart1 = 0;
	mPart0 = value;
}

inline int128_t_base::int128_t_base(uint16_t value)
{
	mPart1 = 0;
	mPart0 = value;
}

inline int128_t_base::int128_t_base(uint32_t value)
{
	mPart1 = 0;
	mPart0 = value;
}

inline int128_t_base::int128_t_base(uint64_t value)
{
	mPart1 = 0;
	mPart0 = value;
}

///////////////////////////////////////////////////////////////////////////////
// OperatorPlus
//
// Returns: (value1 + value2) into result.
// The output 'result' *is* allowed to point to the same memory as one of the inputs.
// To consider: Fix 'defect' of this function whereby it doesn't implement overflow wraparound.
//
inline void int128_t_base::OperatorPlus(const int128_t_base& value1, const int128_t_base& value2, int128_t_base& result)
{
	uint64_t t      = value1.mPart0 + value2.mPart0;
	uint64_t nCarry = (t < value1.mPart0) && (t < value2.mPart0);
	result.mPart0   = t;
	result.mPart1   = value1.mPart1 + value2.mPart1 + nCarry;
}

///////////////////////////////////////////////////////////////////////////////
// OperatorMinus
//
// Returns: (value1 - value2) into result.
// The output 'result' *is* allowed to point to the same memory as one of the inputs.
// To consider: Fix 'defect' of this function whereby it doesn't implement overflow wraparound.
//
inline void int128_t_base::OperatorMinus(const int128_t_base& value1, const int128_t_base& value2, int128_t_base& result)
{
	uint64_t t      = (value1.mPart0 - value2.mPart0);
	uint64_t nCarry = (value1.mPart0 < value2.mPart0) ? 1u : 0u;
	result.mPart0   = t;
	result.mPart1   = (value1.mPart1 - value2.mPart1) - nCarry;
}

///////////////////////////////////////////////////////////////////////////////
// OperatorMul
//
// 64 bit systems:
//    This is how it would be able to work if we could get a 128 bit result from
//    two 64 bit values. None of the 64 bit systems that we are currently working
//    with have C language support for multiplying two 64 bit numbers and retrieving
//    the 128 bit result. However, many 64 bit platforms have support at the asm
//    level for doing such a thing.
//                                                      Part 1            Part 0
//                                            0000000000000002  0000000000000001
//                                         x  0000000000000002  0000000000000001
//                                   -------------------------------------------
//                                          | 0000000000000002  0000000000000001
//                      +  0000000000000004 | 0000000000000002 (0000000000000000)
//     -------------------------------------------------------------------------
//
inline void int128_t_base::OperatorMul(const int128_t_base& a, const int128_t_base& b, int128_t_base& result)
{
	// To consider: Use compiler or OS-provided custom functionality here, such as
	//              Windows UnsignedMultiply128 and GCC's built-in int128_t.

	#if defined(DISABLED_PLATFORM_WIN64)
		// To do: Implement x86-64 asm here.

	#else
		// Else we are stuck doing something less efficient. In this case we 
		// fall back to doing 32 bit multiplies as with 32 bit platforms.
		result       = (a.mPart0 & 0xffffffff) *  (b.mPart0 & 0xffffffff);
		int128_t v01 = (a.mPart0 & 0xffffffff) * ((b.mPart0 >> 32) & 0xffffffff);
		int128_t v02 = (a.mPart0 & 0xffffffff) *  (b.mPart1 & 0xffffffff);
		int128_t v03 = (a.mPart0 & 0xffffffff) * ((b.mPart1 >> 32) & 0xffffffff);

		int128_t v10 = ((a.mPart0 >> 32) & 0xffffffff) *  (b.mPart0 & 0xffffffff);
		int128_t v11 = ((a.mPart0 >> 32) & 0xffffffff) * ((b.mPart0 >> 32) & 0xffffffff);
		int128_t v12 = ((a.mPart0 >> 32) & 0xffffffff) *  (b.mPart1 & 0xffffffff);

		int128_t v20 = (a.mPart1 & 0xffffffff) *  (b.mPart0 & 0xffffffff);
		int128_t v21 = (a.mPart1 & 0xffffffff) * ((b.mPart0 >> 32) & 0xffffffff);

		int128_t v30 = ((a.mPart1 >> 32) & 0xffffffff) * (b.mPart0 & 0xffffffff);

		// Do row addition, shifting as needed. 
		OperatorPlus(result, v01 << 32, result);
		OperatorPlus(result, v02 << 64, result);
		OperatorPlus(result, v03 << 96, result);

		OperatorPlus(result, v10 << 32, result);
		OperatorPlus(result, v11 << 64, result);
		OperatorPlus(result, v12 << 96, result);

		OperatorPlus(result, v20 << 64, result);
		OperatorPlus(result, v21 << 96, result);

		OperatorPlus(result, v30 << 96, result);
	#endif
}

///////////////////////////////////////////////////////////////////////////////
// OperatorShiftRight
//
// Returns: value >> nShift into result
// The output 'result' may *not* be the same as one the input.
// With rightward shifts of negative numbers, shift in zero from the left side.
//
inline void int128_t_base::OperatorShiftRight(const int128_t_base& value, int nShift, int128_t_base& result)
{
	if(nShift >= 0)
	{
		if(nShift < 64)
		{   // 0 - 63
			result.mPart1 = (value.mPart1 >> nShift);

			if(nShift == 0)
				result.mPart0 = (value.mPart0 >> nShift);
			else
				result.mPart0 = (value.mPart0 >> nShift) | (value.mPart1 << (64 - nShift));
		}
		else
		{   // 64+
			result.mPart1 = 0;
			result.mPart0 = (value.mPart1 >> (nShift - 64));
		}
	}
	else // (nShift < 0)
		OperatorShiftLeft(value, -nShift, result);
}


///////////////////////////////////////////////////////////////////////////////
// OperatorShiftRight
//
// Returns: value << nShift into result
// The output 'result' may *not* be the same as one the input.
// With rightward shifts of negative numbers, shift in zero from the left side.
//
inline void int128_t_base::OperatorShiftLeft(const int128_t_base& value, int nShift, int128_t_base& result)
{
	if(nShift >= 0)
	{
		if(nShift < 64)
		{
			if(nShift) // We need to have a special case because CPUs convert a shift by 64 to a no-op.
			{
				// 1 - 63
				result.mPart0 = (value.mPart0 << nShift);
				result.mPart1 = (value.mPart1 << nShift) | (value.mPart0 >> (64 - nShift));
			}
			else
			{
				result.mPart0 = value.mPart0;
				result.mPart1 = value.mPart1;
			}
		}
		else
		{   // 64+
			result.mPart0 = 0;
			result.mPart1 = (value.mPart0 << (nShift - 64));
		}
	}
	else // (nShift < 0)
		OperatorShiftRight(value, -nShift, result);
}


inline bool int128_t_base::operator!() const
{
	return (mPart0 == 0) && (mPart1 == 0);
}


///////////////////////////////////////////////////////////////////////////////
// OperatorXOR
//
// Returns: value1 ^ value2 into result
// The output 'result' may be the same as one the input.
//
inline void int128_t_base::OperatorXOR(const int128_t_base& value1, const int128_t_base& value2, int128_t_base& result)
{
	result.mPart0 = (value1.mPart0 ^ value2.mPart0);
	result.mPart1 = (value1.mPart1 ^ value2.mPart1);
}


///////////////////////////////////////////////////////////////////////////////
// OperatorOR
//
// Returns: value1 | value2 into result
// The output 'result' may be the same as one the input.
//
inline void int128_t_base::OperatorOR(const int128_t_base& value1, const int128_t_base& value2, int128_t_base& result)
{
	result.mPart0 = (value1.mPart0 | value2.mPart0);
	result.mPart1 = (value1.mPart1 | value2.mPart1);
}


///////////////////////////////////////////////////////////////////////////////
// OperatorAND
//
// Returns: value1 & value2 into result
// The output 'result' may be the same as one the input.
//
inline void int128_t_base::OperatorAND(const int128_t_base& value1, const int128_t_base& value2, int128_t_base& result)
{
	result.mPart0 = (value1.mPart0 & value2.mPart0);
	result.mPart1 = (value1.mPart1 & value2.mPart1);
}


inline bool int128_t_base::IsZero() const
{
	return (mPart0 == 0) && // Check mPart0 first as this will likely yield faster execution.
		   (mPart1 == 0);
}


inline void int128_t_base::SetZero()
{
	mPart1 = 0;
	mPart0 = 0;
}


inline void int128_t_base::TwosComplement()
{
	mPart1 = ~mPart1;
	mPart0 = ~mPart0;

	// What we want to do, but isn't available at this level:
	// operator++();
	// Alternative:
	int128_t_base one((uint32_t)1);
	OperatorPlus(*this, one, *this);
}


inline void int128_t_base::InverseTwosComplement()
{
	// What we want to do, but isn't available at this level:
	// operator--();
	// Alternative:
	int128_t_base one((uint32_t)1);
	OperatorMinus(*this, one, *this);

	mPart1 = ~mPart1;
	mPart0 = ~mPart0;
}


inline void int128_t_base::DoubleToUint128(double value)
{
	// Currently this function is limited to 64 bits of integer input.
	// We need to make a better version of this function. Perhaps we should implement 
	// it via dissecting the IEEE floating point format (sign, exponent, matissa).
	// EA_ASSERT(fabs(value) < 18446744073709551616.0); // Assert that the input is <= 64 bits of integer.

	mPart1 = 0;
	mPart0 = (value >= 0 ? (uint64_t)value : (uint64_t)-value);
}





///////////////////////////////////////////////////////////////////////////////////////////////////////
// uint128_t implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////

inline uint128_t uint128_t::operator^(const uint128_t& other) const
{
	uint128_t temp;
	uint128_t::OperatorXOR(*this, other, temp);
	return temp;
}

inline uint128_t uint128_t::operator|(const uint128_t& other) const
{
	uint128_t temp;
	uint128_t::OperatorOR(*this, other, temp);
	return temp;
}

inline uint128_t uint128_t::operator&(const uint128_t& other) const
{
	uint128_t temp;
	uint128_t::OperatorAND(*this, other, temp);
	return temp;
}

inline uint128_t& uint128_t::operator^=(const uint128_t& value)
{
	OperatorXOR(*this, value, *this);
	return *this;
}

inline uint128_t& uint128_t::operator|=(const uint128_t& value)
{
	OperatorOR(*this, value, *this);
	return *this;
}

inline uint128_t& uint128_t::operator&=(const uint128_t& value)
{
	OperatorAND(*this, value, *this);
	return *this;
}

// With rightward shifts of negative numbers, shift in zero from the left side.
inline uint128_t uint128_t::operator>>(int nShift) const
{
	uint128_t temp;
	OperatorShiftRight(*this, nShift, temp);
	return temp;
}

// With rightward shifts of negative numbers, shift in zero from the left side.
inline uint128_t uint128_t::operator<<(int nShift) const
{
	uint128_t temp;
	OperatorShiftLeft(*this, nShift, temp);
	return temp;
}

inline uint128_t& uint128_t::operator>>=(int nShift)
{
	uint128_t temp;
	OperatorShiftRight(*this, nShift, temp);
	*this = temp;
	return *this;
}

inline uint128_t& uint128_t::operator<<=(int nShift)
{
	uint128_t temp;
	OperatorShiftLeft(*this, nShift, temp);
	*this = temp;
	return *this;
}

inline uint128_t& uint128_t::operator+=(const uint128_t& value)
{
	OperatorPlus(*this, value, *this);
	return *this;
}

inline uint128_t& uint128_t::operator-=(const uint128_t& value)
{
	OperatorMinus(*this, value, *this);
	return *this;
}

inline uint128_t& uint128_t::operator*=(const uint128_t& value)
{
	*this = *this * value;
	return *this;
}

inline uint128_t& uint128_t::operator/=(const uint128_t& value)
{
	*this = *this / value;
	return *this;
}

inline uint128_t& uint128_t::operator%=(const uint128_t& value)
{
	*this = *this % value;
	return *this;
}

inline uint128_t uint128_t::operator+(const uint128_t& other)
{
	uint128_t temp;
	uint128_t::OperatorPlus(*this, other, temp);
	return temp;
}

inline uint128_t uint128_t::operator-(const uint128_t& other)
{
	uint128_t temp;
	uint128_t::OperatorMinus(*this, other, temp);
	return temp;
}

inline uint128_t uint128_t::operator*(const uint128_t& other)
{
	uint128_t returnValue;
	int128_t_base::OperatorMul(*this, other, returnValue);
	return returnValue;
}

inline uint128_t uint128_t::operator/(const uint128_t& other)
{
	uint128_t remainder;
	uint128_t quotient;
	this->Modulus(other, quotient, remainder);
	return quotient;
}

inline uint128_t uint128_t::operator%(const uint128_t& other)
{
	uint128_t remainder;
	uint128_t quotient;
	this->Modulus(other, quotient, remainder);
	return remainder;
}

inline uint128_t uint128_t::operator+() const
{
	return *this;
}

inline uint128_t uint128_t::operator~() const
{
	return uint128_t(~mPart0, ~mPart1);
}

inline uint128_t& uint128_t::operator--()
{
	int128_t_base one((uint32_t)1);
	OperatorMinus(*this, one, *this);
	return *this;
}

inline uint128_t uint128_t::operator--(int)
{
	uint128_t temp((uint32_t)1);
	OperatorMinus(*this, temp, temp);
	return temp;
}

inline uint128_t uint128_t::operator++(int)
{
	uint128_t prev = *this;
	uint128_t temp((uint32_t)1);
	OperatorPlus(*this, temp, *this);
	return prev;
}

inline uint128_t& uint128_t::operator++() 
{
	int128_t_base one((uint32_t)1);
	OperatorPlus(*this, one, *this);
	return *this;
}

inline void uint128_t::Negate()
{
	TwosComplement();
}

inline uint128_t uint128_t::operator-() const
{
	uint128_t returnValue(*this);
	returnValue.Negate();
	return returnValue;
}

// This function forms the basis of all logical comparison functions.
// If value1 <  value2, the return value is -1.
// If value1 == value2, the return value is 0.
// If value1 >  value2, the return value is 1.
inline int uint128_t::compare(const uint128_t& other) const
{
	// Compare individual parts. At this point, the two numbers have the same sign.
	if(mPart1 == other.mPart1)
	{
		if(mPart0 == other.mPart0)
			return 0;
		else if(mPart0 > other.mPart0)
			return 1;
		// return -1; //Just fall through to the end.
	}
	else if(mPart1 > other.mPart1)
		return 1;
	return -1;
}

EA_DISABLE_VC_WARNING(4723) // warning C4723: potential divide by 0
inline void uint128_t::Modulus(const uint128_t& divisor, uint128_t& quotient, uint128_t& remainder) const
{
	uint128_t tempDividend(*this);
	uint128_t tempDivisor(divisor);

	if(tempDivisor.IsZero())
	{
		// Force a divide by zero exception. 
		// We know that tempDivisor.mPart0 is zero.
		quotient.mPart0 /= tempDivisor.mPart0;
	}
	else if(tempDividend.IsZero())
	{
		quotient  = uint128_t((uint32_t)0);
		remainder = uint128_t((uint32_t)0);
	}
	else
	{
		remainder.SetZero();

		for(int i(0); i < 128; i++)
		{
			remainder += (uint32_t)tempDividend.GetBit(127 - i);
			const bool bBit(remainder >= tempDivisor);
			quotient.SetBit(127 - i, bBit);

			if(bBit)
				remainder -= tempDivisor;
		 
			if((i != 127) && !remainder.IsZero())
				remainder <<= 1;
		}
	}
}
EA_RESTORE_VC_WARNING()

inline bool uint128_t::operator==(const uint128_t& other) const
{
	return (mPart0 == other.mPart0) && // Check mPart0 first as this will likely yield faster execution.
		   (mPart1 == other.mPart1);
}

inline bool uint128_t::operator< (const uint128_t& other) const { return (compare(other) < 0); }
inline bool uint128_t::operator!=(const uint128_t& other) const { return !(*this == other); }
inline bool uint128_t::operator> (const uint128_t& other) const { return other < *this; }
inline bool uint128_t::operator>=(const uint128_t& other) const { return !(*this < other); }
inline bool uint128_t::operator<=(const uint128_t& other) const { return !(other < *this); }

inline bool uint128_t::IsNegative() const
{   // True if value < 0
	return false;
}

inline bool uint128_t::IsPositive() const
{
	// True of value >= 0
	return true;
}






///////////////////////////////////////////////////////////////////////////////////////////////////////
// int128_t implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////

inline void int128_t::Negate()
{
	if (IsPositive())
		TwosComplement();
	else
		InverseTwosComplement();
}

inline int128_t int128_t::operator-() const
{
	int128_t returnValue(*this);
	returnValue.Negate();
	return returnValue;
}

inline int128_t& int128_t::operator++()
{
	int128_t_base one((uint32_t)1);
	OperatorPlus(*this, one, *this);
	return *this;
}

inline int128_t& int128_t::operator--()
{
	int128_t_base one((uint32_t)1);
	OperatorMinus(*this, one, *this);
	return *this;
}

inline int128_t int128_t::operator++(int)
{
	int128_t prev = *this;
	int128_t temp((uint32_t)1);
	OperatorPlus(*this, temp, *this);
	return prev;
}

inline int128_t int128_t::operator--(int)
{
	int128_t temp((uint32_t)1);
	OperatorMinus(*this, temp, temp);
	return temp;
}

inline int128_t int128_t::operator+() const
{
	return *this;
}

inline int128_t int128_t::operator~() const
{
	return int128_t(~mPart0, ~mPart1);
}

inline int128_t int128_t::operator+(const int128_t& other)
{
	int128_t temp;
	int128_t::OperatorPlus(*this, other, temp);
	return temp;
}

inline int128_t int128_t::operator-(const int128_t& other)
{
	int128_t temp;
	int128_t::OperatorMinus(*this, other, temp);
	return temp;
}

// This function forms the basis of all logical comparison functions.
// If value1 <  value2, the return value is -1.
// If value1 == value2, the return value is 0.
// If value1 >  value2, the return value is 1.
inline int int128_t::compare(const int128_t& other) const
{
	// Cache some values. Positive means >= 0. Negative means < 0 and thus means '!positive'.
	const bool bValue1IsPositive(      IsPositive());
	const bool bValue2IsPositive(other.IsPositive());

	// Do positive/negative tests.
	if(bValue1IsPositive != bValue2IsPositive)
		return bValue1IsPositive ? 1 : -1;

	// Compare individual parts. At this point, the two numbers have the same sign.
	if(mPart1 == other.mPart1)
	{
		if(mPart0 == other.mPart0)
			return 0;
		else if(mPart0 > other.mPart0)
			return 1;
		// return -1; //Just fall through to the end.
	}
	else if(mPart1 > other.mPart1)
		return 1;
	return -1;
}

inline bool int128_t::operator==(const int128_t& other) const
{
	return (mPart0 == other.mPart0) && // Check mPart0 first as this will likely yield faster execution.
		   (mPart1 == other.mPart1);
}

inline bool int128_t::operator!=(const int128_t& other) const
{
	return (mPart0 != other.mPart0) ||  // Check mPart0 first as this will likely yield faster execution.
		   (mPart1 != other.mPart1);
}

inline bool int128_t::operator>(const int128_t& other) const
{
	return (compare(other) > 0);
}

inline bool int128_t::operator>=(const int128_t& other) const
{
	return (compare(other) >= 0);
}

inline bool int128_t::operator<(const int128_t& other) const
{
	return (compare(other) < 0);
}

inline bool int128_t::operator<=(const int128_t& other) const
{
	return (compare(other) <= 0);
}

inline bool int128_t::IsNegative() const
{   // True if value < 0
	return ((mPart1 & UINT64_C(0x8000000000000000)) != 0);
}

inline bool int128_t::IsPositive() const
{   // True of value >= 0
	return ((mPart1 & UINT64_C(0x8000000000000000)) == 0);
}

inline int128_t int128_t::operator*(const int128_t& other)
{
	int128_t a(*this);
	int128_t b(other);
	int128_t returnValue;

	// Correctly handle negative values
	bool bANegative(false);
	bool bBNegative(false);

	if(a.IsNegative())
	{
		bANegative = true;
		a.Negate();
	}

	if(b.IsNegative())
	{
		bBNegative = true;
		b.Negate();
	}

	int128_t_base::OperatorMul(a, b, returnValue);

	// Do negation as needed.
	if(bANegative != bBNegative)
		returnValue.Negate();

	return returnValue;
}

inline int128_t int128_t::operator/(const int128_t& other)
{
	int128_t remainder;
	int128_t quotient;
	this->Modulus(other, quotient, remainder);
	return quotient;
}

inline int128_t int128_t::operator<<(int nShift) const
{
	int128_t temp;
	OperatorShiftLeft(*this, nShift, temp);
	return temp;
}

inline int128_t& int128_t::operator+=(const int128_t& value)
{
	OperatorPlus(*this, value, *this);
	return *this;
}

inline int128_t& int128_t::operator-=(const int128_t& value)
{
	OperatorMinus(*this, value, *this);
	return *this;
}

inline int128_t& int128_t::operator<<=(int nShift)
{
	int128_t temp;
	OperatorShiftLeft(*this, nShift, temp);
	*this = temp;
	return *this;
}

inline int128_t& int128_t::operator*=(const int128_t& value)
{
	*this = *this * value;
	return *this;
}

inline int128_t& int128_t::operator%=(const int128_t& value)
{
	*this = *this % value;
	return *this;
}

inline int128_t int128_t::operator%(const int128_t& other)
{
	int128_t remainder;
	int128_t quotient;
	this->Modulus(other, quotient, remainder);
	return remainder;
}

inline int128_t& int128_t::operator/=(const int128_t& value)
{
	*this = *this / value;
	return *this;
}

// With rightward shifts of negative numbers, shift in zero from the left side.
inline int128_t int128_t::operator>>(int nShift) const
{
	int128_t temp;
	OperatorShiftRight(*this, nShift, temp);
	return temp;
}

inline int128_t& int128_t::operator>>=(int nShift)
{
	int128_t temp;
	OperatorShiftRight(*this, nShift, temp);
	*this = temp;
	return *this;
}

inline int128_t int128_t::operator^(const int128_t& other) const
{
	int128_t temp;
	int128_t::OperatorXOR(*this, other, temp);
	return temp;
}

inline int128_t int128_t::operator|(const int128_t& other) const
{
	int128_t temp;
	int128_t::OperatorOR(*this, other, temp);
	return temp;
}


inline int128_t int128_t::operator&(const int128_t& other) const
{
	int128_t temp;
	int128_t::OperatorAND(*this, other, temp);
	return temp;
}

inline int128_t& int128_t::operator^=(const int128_t& value)
{
	OperatorXOR(*this, value, *this);
	return *this;
}

inline int128_t& int128_t::operator|=(const int128_t& value)
{
	OperatorOR(*this, value, *this);
	return *this;
}

inline int128_t& int128_t::operator&=(const int128_t& value)
{
	OperatorAND(*this, value, *this);
	return *this;
}

EA_DISABLE_VC_WARNING(4723) // warning C4723: potential divide by 0
inline void int128_t::Modulus(const int128_t& divisor, int128_t& quotient, int128_t& remainder) const
{
	int128_t tempDividend(*this);
	int128_t tempDivisor(divisor);

	bool bDividendNegative = false;
	bool bDivisorNegative = false;

	if(tempDividend.IsNegative())
	{
		bDividendNegative = true;
		tempDividend.Negate();
	}
	if(tempDivisor.IsNegative())
	{
		bDivisorNegative = true;
		tempDivisor.Negate();
	}

	// Handle the special cases
	if(tempDivisor.IsZero())
	{
		// Force a divide by zero exception. 
		// We know that tempDivisor.mPart0 is zero.
		quotient.mPart0 /= tempDivisor.mPart0;
	}
	else if(tempDividend.IsZero())
	{
		quotient  = int128_t((uint32_t)0);
		remainder = int128_t((uint32_t)0);
	}
	else
	{
		remainder.SetZero();

		for(int i(0); i < 128; i++)
		{
			remainder += (uint32_t)tempDividend.GetBit(127 - i);
			const bool bBit(remainder >= tempDivisor);
			quotient.SetBit(127 - i, bBit);

			if(bBit)
				remainder -= tempDivisor;
		 
			if((i != 127) && !remainder.IsZero())
				remainder <<= 1;
		}
	}

	if((bDividendNegative && !bDivisorNegative) || (!bDividendNegative && bDivisorNegative))
	{
		// Ensure the following formula applies for negative dividends
		// dividend = divisor * quotient + remainder
		quotient.Negate();
	}
}
EA_RESTORE_VC_WARNING()






///////////////////////////////////////////////////////////////////////////////////////////////////////
// INT128_C / UINT128_C
//
// The C99 language defines macros for portably defining constants of 
// sized numeric types. For example, there might be:
//     #define UINT64_C(x) x##ULL
// Since our int128 data type is not a built-in type, we can't define a
// UINT128_C macro as something that pastes ULLL at the end of the digits.
// Instead we define it to create a temporary that is constructed from a 
// string of the digits. This will work in most cases that suffix pasting
// would work.
//
/* EA_CONSTEXPR */ inline uint128_t UINT128_C(uint64_t nPart1, uint64_t nPart0) { return uint128_t(nPart0, nPart1); }
/* EA_CONSTEXPR */ inline int128_t INT128_C(int64_t nPart1, int64_t nPart0) { return int128_t(static_cast<uint64_t>(nPart0), static_cast<uint64_t>(nPart1)); }




#endif // INCLUDED_int128_h

