///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// We support eastl::numeric_limits for the following types. Sized types such
// as int32_t are covered by these basic types, with the exception of int128_t.
//
//     bool
//     char                 (distinct from signed and unsigned char)
//     unsigned char,
//     signed char, 
//     wchar_t
//     char16_t             (when char16_t is a distict type)
//     char32_t             (when char32_t is a distinct type)
//     unsigned short,
//     signed short
//     unsigned int
//     signed int
//     unsigned long
//     signed long
//     signed long long
//     unsigned long long
//     uint128_t            (when supported natively by the compiler)
//     int128_t             (when supported natively by the compiler)
//     float
//     double
//     long double
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_NUMERIC_LIMITS_H
#define EASTL_NUMERIC_LIMITS_H


#include <EASTL/internal/config.h>
#include <EASTL/type_traits.h>
#include <limits.h>                 // C limits.h header
#include <float.h>
#if defined(_CPPLIB_VER)            // Dinkumware.
	#include <ymath.h>
#endif

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif


// Disable Warnings:
//   4310 - cast truncates constant value
//   4296 - expression is always false
EA_DISABLE_VC_WARNING(4310 4296)



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


///////////////////////////////////////////////////////////////////////////////
// EASTL_LIMITS macros
// These apply to integral types only.
///////////////////////////////////////////////////////////////////////////////

// true or false.
#define EASTL_LIMITS_IS_SIGNED(T)    ((T)(-1) < 0)

// The min possible value of T. 
#define EASTL_LIMITS_MIN_S(T) ((T)((T)1 << EASTL_LIMITS_DIGITS_S(T)))
#define EASTL_LIMITS_MIN_U(T) ((T)0)
#define EASTL_LIMITS_MIN(T)   ((EASTL_LIMITS_IS_SIGNED(T) ? EASTL_LIMITS_MIN_S(T) : EASTL_LIMITS_MIN_U(T)))

// The max possible value of T. 
#define EASTL_LIMITS_MAX_S(T) ((T)(((((T)1 << (EASTL_LIMITS_DIGITS(T) - 1)) - 1) << 1) + 1))
#define EASTL_LIMITS_MAX_U(T) ((T)~(T)0)
#define EASTL_LIMITS_MAX(T)   ((EASTL_LIMITS_IS_SIGNED(T) ? EASTL_LIMITS_MAX_S(T) : EASTL_LIMITS_MAX_U(T)))

// The number of bits in the representation of T.
#define EASTL_LIMITS_DIGITS_S(T) ((sizeof(T) * CHAR_BIT) - 1)
#define EASTL_LIMITS_DIGITS_U(T) ((sizeof(T) * CHAR_BIT))
#define EASTL_LIMITS_DIGITS(T)   ((EASTL_LIMITS_IS_SIGNED(T) ? EASTL_LIMITS_DIGITS_S(T) : EASTL_LIMITS_DIGITS_U(T)))

// The number of decimal digits that can be represented by T.
#define EASTL_LIMITS_DIGITS10_S(T) ((EASTL_LIMITS_DIGITS_S(T) * 643L) / 2136) // (643 / 2136) ~= log10(2).
#define EASTL_LIMITS_DIGITS10_U(T) ((EASTL_LIMITS_DIGITS_U(T) * 643L) / 2136)
#define EASTL_LIMITS_DIGITS10(T)   ((EASTL_LIMITS_IS_SIGNED(T) ? EASTL_LIMITS_DIGITS10_S(T) : EASTL_LIMITS_DIGITS10_U(T)))






namespace eastl
{
	// See C++11 18.3.2.5
	enum float_round_style
	{
		round_indeterminate       = -1,    /// Intermediate.
		round_toward_zero         =  0,    /// To zero.
		round_to_nearest          =  1,    /// To the nearest representable value.
		round_toward_infinity     =  2,    /// To infinity.
		round_toward_neg_infinity =  3     /// To negative infinity.
	};

	// See C++11 18.3.2.6
	enum float_denorm_style
	{
		denorm_indeterminate = -1,          /// It cannot be determined whether or not the type allows denormalized values.
		denorm_absent        =  0,          /// The type does not allow denormalized values.
		denorm_present       =  1           /// The type allows denormalized values.
	};


	// Default numeric_limits.
	// See C++11 18.3.2.3
	template<typename T>
	class numeric_limits;


	// Const/volatile variations of numeric_limits.
	template<typename T>
	class numeric_limits<const T>
		: public numeric_limits<T>
	{};

	template<typename T>
	class numeric_limits<volatile T>
		: public numeric_limits<T>
	{};

	template<typename T>
	class numeric_limits<const volatile T>
		: public numeric_limits<T>
	{};



	// numeric_limits<bool>
	template<>
	class numeric_limits<bool>
	{
	public:
		typedef bool value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = 1;      // In practice bool is stores as a byte, or sometimes an int.
		static constexpr int                digits10          = 0;
		static constexpr int                max_digits10      = 0;
		static constexpr bool               is_signed         = false;  // In practice bool may be implemented as signed char.
		static constexpr bool               is_integer        = true;
		static constexpr bool               is_exact          = true;
		static constexpr int                radix             = 2;
		static constexpr int                min_exponent      = 0;
		static constexpr int                min_exponent10    = 0;
		static constexpr int                max_exponent      = 0;
		static constexpr int                max_exponent10    = 0;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = false;
		static constexpr bool               traps             = true;   // Should this be true or false? Given that it's implemented in hardware as an integer type, we use true.
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_toward_zero;
		static constexpr bool               has_infinity      = false;
		static constexpr bool               has_quiet_NaN     = false;
		static constexpr bool               has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm        = denorm_absent;
		static constexpr bool               has_denorm_loss   = false;
		static constexpr bool               is_iec559         = false;

		static constexpr value_type min() 
			{ return false; }

		static constexpr value_type max()
			{ return true; }

		static constexpr value_type lowest() 
			{ return false; }

		static constexpr value_type epsilon() 
			{ return false; }

		static constexpr value_type round_error() 
			{ return false; }

		static constexpr value_type infinity() 
			{ return value_type(); }

		static constexpr value_type quiet_NaN() 
			{ return value_type(); }

		static constexpr value_type signaling_NaN()
			{ return value_type(); }

		static constexpr value_type denorm_min() 
			{ return value_type(); }
	};


	// numeric_limits<char>
	template<>
	class numeric_limits<char>
	{
	public:
		typedef char value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = EASTL_LIMITS_DIGITS(value_type);
		static constexpr int                digits10          = EASTL_LIMITS_DIGITS10(value_type);
		static constexpr int                max_digits10      = 0;
		static constexpr bool               is_signed         = EASTL_LIMITS_IS_SIGNED(value_type);
		static constexpr bool               is_integer        = true;
		static constexpr bool               is_exact          = true;
		static constexpr int                radix             = 2;
		static constexpr int                min_exponent      = 0;
		static constexpr int                min_exponent10    = 0;
		static constexpr int                max_exponent      = 0;
		static constexpr int                max_exponent10    = 0;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = true;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_toward_zero;
		static constexpr bool               has_infinity      = false;
		static constexpr bool               has_quiet_NaN     = false;
		static constexpr bool               has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm        = denorm_absent;
		static constexpr bool               has_denorm_loss   = false;
		static constexpr bool               is_iec559         = false;

		static constexpr value_type min() 
			{ return EASTL_LIMITS_MIN(value_type); }

		static constexpr value_type max()
			{ return EASTL_LIMITS_MAX(value_type); }

		static constexpr value_type lowest() 
			{ return EASTL_LIMITS_MIN(value_type); }

		static constexpr value_type epsilon() 
			{ return 0; }

		static constexpr value_type round_error() 
			{ return 0; }

		static constexpr value_type infinity() 
			{ return value_type(); }    // Question: Should we return 0 here or value_type()?

		static constexpr value_type quiet_NaN() 
			{ return value_type(); }

		static constexpr value_type signaling_NaN()
			{ return value_type(); }

		static constexpr value_type denorm_min() 
			{ return (value_type)0; }
	};


	// numeric_limits<unsigned char>
	template<>
	class numeric_limits<unsigned char>
	{
	public:
		typedef unsigned char value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = EASTL_LIMITS_DIGITS_U(value_type);
		static constexpr int                digits10          = EASTL_LIMITS_DIGITS10_U(value_type);
		static constexpr int                max_digits10      = 0;
		static constexpr bool               is_signed         = false;
		static constexpr bool               is_integer        = true;
		static constexpr bool               is_exact          = true;
		static constexpr int                radix             = 2;
		static constexpr int                min_exponent      = 0;
		static constexpr int                min_exponent10    = 0;
		static constexpr int                max_exponent      = 0;
		static constexpr int                max_exponent10    = 0;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = true;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_toward_zero;
		static constexpr bool               has_infinity      = false;
		static constexpr bool               has_quiet_NaN     = false;
		static constexpr bool               has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm        = denorm_absent;
		static constexpr bool               has_denorm_loss   = false;
		static constexpr bool               is_iec559         = false;

		static constexpr value_type min() 
			{ return 0; }

		static constexpr value_type max()
			{ return EASTL_LIMITS_MAX_U(value_type); }

		static constexpr value_type lowest() 
			{ return 0; }

		static constexpr value_type epsilon() 
			{ return 0; }

		static constexpr value_type round_error() 
			{ return 0; }

		static constexpr value_type infinity() 
			{ return value_type(); }

		static constexpr value_type quiet_NaN() 
			{ return value_type(); }

		static constexpr value_type signaling_NaN()
			{ return value_type(); }

		static constexpr value_type denorm_min() 
			{ return (value_type)0; }
	};


	// numeric_limits<signed char>
	template<>
	class numeric_limits<signed char>
	{
	public:
		typedef signed char value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = EASTL_LIMITS_DIGITS_S(value_type);
		static constexpr int                digits10          = EASTL_LIMITS_DIGITS10_S(value_type);
		static constexpr int                max_digits10      = 0;
		static constexpr bool               is_signed         = true;
		static constexpr bool               is_integer        = true;
		static constexpr bool               is_exact          = true;
		static constexpr int                radix             = 2;
		static constexpr int                min_exponent      = 0;
		static constexpr int                min_exponent10    = 0;
		static constexpr int                max_exponent      = 0;
		static constexpr int                max_exponent10    = 0;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = true;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_toward_zero;
		static constexpr bool               has_infinity      = false;
		static constexpr bool               has_quiet_NaN     = false;
		static constexpr bool               has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm        = denorm_absent;
		static constexpr bool               has_denorm_loss   = false;
		static constexpr bool               is_iec559         = false;

		static constexpr value_type min() 
			{ return EASTL_LIMITS_MIN_S(value_type); }

		static constexpr value_type max()
			{ return EASTL_LIMITS_MAX_S(value_type); }

		static constexpr value_type lowest() 
			{ return EASTL_LIMITS_MIN_S(value_type); }

		static constexpr value_type epsilon() 
			{ return 0; }

		static constexpr value_type round_error() 
			{ return 0; }

		static constexpr value_type infinity() 
			{ return value_type(); }

		static constexpr value_type quiet_NaN() 
			{ return value_type(); }

		static constexpr value_type signaling_NaN()
			{ return value_type(); }

		static constexpr value_type denorm_min() 
			{ return (value_type)0; }
	};


	// numeric_limits<wchar_t>
	// VC++ has the option of making wchar_t simply be unsigned short. If that's enabled then  
	// the code below could possibly cause compile failures due to redundancy. The best resolution 
	// may be to use __wchar_t here for VC++ instead of wchar_t, as __wchar_t is always a true 
	// unique type under VC++. http://social.msdn.microsoft.com/Forums/en-US/vclanguage/thread/9059330a-7cce-4d0d-a8e0-e1dcb63322bd/
	template<>
	class numeric_limits<wchar_t>
	{
	public:
		typedef wchar_t value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = EASTL_LIMITS_DIGITS(value_type);
		static constexpr int                digits10          = EASTL_LIMITS_DIGITS10(value_type);
		static constexpr int                max_digits10      = 0;
		static constexpr bool               is_signed         = EASTL_LIMITS_IS_SIGNED(value_type);
		static constexpr bool               is_integer        = true;
		static constexpr bool               is_exact          = true;
		static constexpr int                radix             = 2;
		static constexpr int                min_exponent      = 0;
		static constexpr int                min_exponent10    = 0;
		static constexpr int                max_exponent      = 0;
		static constexpr int                max_exponent10    = 0;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = true;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_toward_zero;
		static constexpr bool               has_infinity      = false;
		static constexpr bool               has_quiet_NaN     = false;
		static constexpr bool               has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm        = denorm_absent;
		static constexpr bool               has_denorm_loss   = false;
		static constexpr bool               is_iec559         = false;

		static constexpr value_type min() 
			{ return EASTL_LIMITS_MIN(value_type); }

		static constexpr value_type max()
			{ return EASTL_LIMITS_MAX(value_type); }

		static constexpr value_type lowest() 
			{ return EASTL_LIMITS_MIN(value_type); }

		static constexpr value_type epsilon() 
			{ return 0; }

		static constexpr value_type round_error() 
			{ return 0; }

		static constexpr value_type infinity() 
			{ return value_type(); }

		static constexpr value_type quiet_NaN() 
			{ return value_type(); }

		static constexpr value_type signaling_NaN()
			{ return value_type(); }

		static constexpr value_type denorm_min() 
			{ return (value_type)0; }
	};


	#if defined(EA_CHAR8_UNIQUE) && EA_CHAR8_UNIQUE
	template<>
	class numeric_limits<char8_t>
	{
	public:
		typedef char8_t value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = EASTL_LIMITS_DIGITS(value_type);
		static constexpr int                digits10          = EASTL_LIMITS_DIGITS10(value_type);
		static constexpr int                max_digits10      = 0;
		static constexpr bool               is_signed         = EASTL_LIMITS_IS_SIGNED(value_type);
		static constexpr bool               is_integer        = true;
		static constexpr bool               is_exact          = true;
		static constexpr int                radix             = 2;
		static constexpr int                min_exponent      = 0;
		static constexpr int                min_exponent10    = 0;
		static constexpr int                max_exponent      = 0;
		static constexpr int                max_exponent10    = 0;
 		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = true;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_toward_zero;
		static constexpr bool               has_infinity      = false;
		static constexpr bool               has_quiet_NaN     = false;
		static constexpr bool               has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm        = denorm_absent;
		static constexpr bool               has_denorm_loss   = false;
		static constexpr bool               is_iec559         = false;

		static constexpr value_type min()
			{ return EASTL_LIMITS_MIN(value_type); }

		static constexpr value_type max()
			{ return EASTL_LIMITS_MAX(value_type); }

		static constexpr value_type lowest()
			{ return EASTL_LIMITS_MIN(value_type); }

		static constexpr value_type epsilon()
			{ return 0; }

		static constexpr value_type round_error()
			{ return 0; }

		static constexpr value_type infinity()
			{ return 0; }

		static constexpr value_type quiet_NaN()
			{ return 0; }

		static constexpr value_type signaling_NaN()
			{ return 0; }

		static constexpr value_type denorm_min()
			{ return (value_type)0; }
	};
	#endif

	#if EA_CHAR16_NATIVE // If char16_t is a true unique type (as called for by the C++11 Standard)...

		// numeric_limits<char16_t>
		template<>
		class numeric_limits<char16_t>
		{
		public:
			typedef char16_t value_type;

			static constexpr bool               is_specialized    = true;
			static constexpr int                digits            = EASTL_LIMITS_DIGITS(value_type);
			static constexpr int                digits10          = EASTL_LIMITS_DIGITS10(value_type);
			static constexpr int                max_digits10      = 0;
			static constexpr bool               is_signed         = EASTL_LIMITS_IS_SIGNED(value_type);
			static constexpr bool               is_integer        = true;
			static constexpr bool               is_exact          = true;
			static constexpr int                radix             = 2;
			static constexpr int                min_exponent      = 0;
			static constexpr int                min_exponent10    = 0;
			static constexpr int                max_exponent      = 0;
			static constexpr int                max_exponent10    = 0;
			static constexpr bool               is_bounded        = true;
			static constexpr bool               is_modulo         = true;
			static constexpr bool               traps             = true;
			static constexpr bool               tinyness_before   = false;
			static constexpr float_round_style  round_style       = round_toward_zero;
			static constexpr bool               has_infinity      = false;
			static constexpr bool               has_quiet_NaN     = false;
			static constexpr bool               has_signaling_NaN = false;
			static constexpr float_denorm_style has_denorm        = denorm_absent;
			static constexpr bool               has_denorm_loss   = false;
			static constexpr bool               is_iec559         = false;

			static constexpr value_type min() 
				{ return EASTL_LIMITS_MIN(value_type); }

			static constexpr value_type max()
				{ return EASTL_LIMITS_MAX(value_type); }

			static constexpr value_type lowest() 
				{ return EASTL_LIMITS_MIN(value_type); }

			static constexpr value_type epsilon() 
				{ return 0; }

			static constexpr value_type round_error() 
				{ return 0; }

			static constexpr value_type infinity() 
				{ return value_type(); }

			static constexpr value_type quiet_NaN() 
				{ return value_type(); }

			static constexpr value_type signaling_NaN()
				{ return value_type(); }

			static constexpr value_type denorm_min() 
				{ return (value_type)0; }
		};

	#endif


	#if EA_CHAR32_NATIVE // If char32_t is a true unique type (as called for by the C++11 Standard)...

		// numeric_limits<char32_t>
		template<>
		class numeric_limits<char32_t>
		{
		public:
			typedef char32_t value_type;

			static constexpr bool               is_specialized    = true;
			static constexpr int                digits            = EASTL_LIMITS_DIGITS(value_type);
			static constexpr int                digits10          = EASTL_LIMITS_DIGITS10(value_type);
			static constexpr int                max_digits10      = 0;
			static constexpr bool               is_signed         = EASTL_LIMITS_IS_SIGNED(value_type);
			static constexpr bool               is_integer        = true;
			static constexpr bool               is_exact          = true;
			static constexpr int                radix             = 2;
			static constexpr int                min_exponent      = 0;
			static constexpr int                min_exponent10    = 0;
			static constexpr int                max_exponent      = 0;
			static constexpr int                max_exponent10    = 0;
			static constexpr bool               is_bounded        = true;
			static constexpr bool               is_modulo         = true;
			static constexpr bool               traps             = true;
			static constexpr bool               tinyness_before   = false;
			static constexpr float_round_style  round_style       = round_toward_zero;
			static constexpr bool               has_infinity      = false;
			static constexpr bool               has_quiet_NaN     = false;
			static constexpr bool               has_signaling_NaN = false;
			static constexpr float_denorm_style has_denorm        = denorm_absent;
			static constexpr bool               has_denorm_loss   = false;
			static constexpr bool               is_iec559         = false;

			static constexpr value_type min() 
				{ return EASTL_LIMITS_MIN(value_type); }

			static constexpr value_type max()
				{ return EASTL_LIMITS_MAX(value_type); }

			static constexpr value_type lowest() 
				{ return EASTL_LIMITS_MIN(value_type); }

			static constexpr value_type epsilon() 
				{ return 0; }

			static constexpr value_type round_error() 
				{ return 0; }

			static constexpr value_type infinity() 
				{ return value_type(); }

			static constexpr value_type quiet_NaN() 
				{ return value_type(); }

			static constexpr value_type signaling_NaN()
				{ return value_type(); }

			static constexpr value_type denorm_min() 
				{ return (value_type)0; }
		};

	#endif


	// numeric_limits<unsigned short>
	template<>
	class numeric_limits<unsigned short>
	{
	public:
		typedef unsigned short value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = EASTL_LIMITS_DIGITS_U(value_type);
		static constexpr int                digits10          = EASTL_LIMITS_DIGITS10_U(value_type);
		static constexpr int                max_digits10      = 0;
		static constexpr bool               is_signed         = false;
		static constexpr bool               is_integer        = true;
		static constexpr bool               is_exact          = true;
		static constexpr int                radix             = 2;
		static constexpr int                min_exponent      = 0;
		static constexpr int                min_exponent10    = 0;
		static constexpr int                max_exponent      = 0;
		static constexpr int                max_exponent10    = 0;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = true;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_toward_zero;
		static constexpr bool               has_infinity      = false;
		static constexpr bool               has_quiet_NaN     = false;
		static constexpr bool               has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm        = denorm_absent;
		static constexpr bool               has_denorm_loss   = false;
		static constexpr bool               is_iec559         = false;

		static constexpr value_type min() 
			{ return 0; }

		static constexpr value_type max()
			{ return EASTL_LIMITS_MAX_U(value_type); }

		static constexpr value_type lowest() 
			{ return 0; }

		static constexpr value_type epsilon() 
			{ return 0; }

		static constexpr value_type round_error() 
			{ return 0; }

		static constexpr value_type infinity() 
			{ return value_type(); }

		static constexpr value_type quiet_NaN() 
			{ return value_type(); }

		static constexpr value_type signaling_NaN()
			{ return value_type(); }

		static constexpr value_type denorm_min() 
			{ return static_cast<value_type>(0); }
	};


	// numeric_limits<signed short>
	template<>
	class numeric_limits<signed short>
	{
	public:
		typedef signed short value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = EASTL_LIMITS_DIGITS_S(value_type);
		static constexpr int                digits10          = EASTL_LIMITS_DIGITS10_S(value_type);
		static constexpr int                max_digits10      = 0;
		static constexpr bool               is_signed         = true;
		static constexpr bool               is_integer        = true;
		static constexpr bool               is_exact          = true;
		static constexpr int                radix             = 2;
		static constexpr int                min_exponent      = 0;
		static constexpr int                min_exponent10    = 0;
		static constexpr int                max_exponent      = 0;
		static constexpr int                max_exponent10    = 0;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = true;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_toward_zero;
		static constexpr bool               has_infinity      = false;
		static constexpr bool               has_quiet_NaN     = false;
		static constexpr bool               has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm        = denorm_absent;
		static constexpr bool               has_denorm_loss   = false;
		static constexpr bool               is_iec559         = false;

		static constexpr value_type min() 
			{ return EASTL_LIMITS_MIN_S(value_type); }

		static constexpr value_type max()
			{ return EASTL_LIMITS_MAX_S(value_type); }

		static constexpr value_type lowest() 
			{ return EASTL_LIMITS_MIN_S(value_type); }

		static constexpr value_type epsilon() 
			{ return 0; }

		static constexpr value_type round_error() 
			{ return 0; }

		static constexpr value_type infinity() 
			{ return value_type(); }

		static constexpr value_type quiet_NaN() 
			{ return value_type(); }

		static constexpr value_type signaling_NaN()
			{ return value_type(); }

		static constexpr value_type denorm_min() 
			{ return static_cast<value_type>(0); }
	};



	// numeric_limits<unsigned int>
	template<>
	class numeric_limits<unsigned int>
	{
	public:
		typedef unsigned int value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = EASTL_LIMITS_DIGITS_U(value_type);
		static constexpr int                digits10          = EASTL_LIMITS_DIGITS10_U(value_type);
		static constexpr int                max_digits10      = 0;
		static constexpr bool               is_signed         = false;
		static constexpr bool               is_integer        = true;
		static constexpr bool               is_exact          = true;
		static constexpr int                radix             = 2;
		static constexpr int                min_exponent      = 0;
		static constexpr int                min_exponent10    = 0;
		static constexpr int                max_exponent      = 0;
		static constexpr int                max_exponent10    = 0;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = true;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_toward_zero;
		static constexpr bool               has_infinity      = false;
		static constexpr bool               has_quiet_NaN     = false;
		static constexpr bool               has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm        = denorm_absent;
		static constexpr bool               has_denorm_loss   = false;
		static constexpr bool               is_iec559         = false;

		static constexpr value_type min() 
			{ return 0; }

		static constexpr value_type max()
			{ return EASTL_LIMITS_MAX_U(value_type); }

		static constexpr value_type lowest() 
			{ return 0; }

		static constexpr value_type epsilon() 
			{ return 0; }

		static constexpr value_type round_error() 
			{ return 0; }

		static constexpr value_type infinity() 
			{ return value_type(); }

		static constexpr value_type quiet_NaN() 
			{ return value_type(); }

		static constexpr value_type signaling_NaN()
			{ return value_type(); }

		static constexpr value_type denorm_min() 
			{ return static_cast<value_type>(0); }
	};


	// numeric_limits<signed int>
	template<>
	class numeric_limits<signed int>
	{
	public:
		typedef signed int value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = EASTL_LIMITS_DIGITS_S(value_type);
		static constexpr int                digits10          = EASTL_LIMITS_DIGITS10_S(value_type);
		static constexpr int                max_digits10      = 0;
		static constexpr bool               is_signed         = true;
		static constexpr bool               is_integer        = true;
		static constexpr bool               is_exact          = true;
		static constexpr int                radix             = 2;
		static constexpr int                min_exponent      = 0;
		static constexpr int                min_exponent10    = 0;
		static constexpr int                max_exponent      = 0;
		static constexpr int                max_exponent10    = 0;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = true;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_toward_zero;
		static constexpr bool               has_infinity      = false;
		static constexpr bool               has_quiet_NaN     = false;
		static constexpr bool               has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm        = denorm_absent;
		static constexpr bool               has_denorm_loss   = false;
		static constexpr bool               is_iec559         = false;

		static constexpr value_type min() 
			{ return INT_MIN; } // It's hard to get EASTL_LIMITS_MIN_S to work with all compilers here.

		static constexpr value_type max()
			{ return EASTL_LIMITS_MAX_S(value_type); }

		static constexpr value_type lowest() 
			{ return INT_MIN; }

		static constexpr value_type epsilon() 
			{ return 0; }

		static constexpr value_type round_error() 
			{ return 0; }

		static constexpr value_type infinity() 
			{ return value_type(); }

		static constexpr value_type quiet_NaN() 
			{ return value_type(); }

		static constexpr value_type signaling_NaN()
			{ return value_type(); }

		static constexpr value_type denorm_min() 
			{ return static_cast<value_type>(0); }
	};


	// numeric_limits<unsigned long>
	template<>
	class numeric_limits<unsigned long>
	{
	public:
		typedef unsigned long value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = EASTL_LIMITS_DIGITS_U(value_type);
		static constexpr int                digits10          = EASTL_LIMITS_DIGITS10_U(value_type);
		static constexpr int                max_digits10      = 0;
		static constexpr bool               is_signed         = false;
		static constexpr bool               is_integer        = true;
		static constexpr bool               is_exact          = true;
		static constexpr int                radix             = 2;
		static constexpr int                min_exponent      = 0;
		static constexpr int                min_exponent10    = 0;
		static constexpr int                max_exponent      = 0;
		static constexpr int                max_exponent10    = 0;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = true;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_toward_zero;
		static constexpr bool               has_infinity      = false;
		static constexpr bool               has_quiet_NaN     = false;
		static constexpr bool               has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm        = denorm_absent;
		static constexpr bool               has_denorm_loss   = false;
		static constexpr bool               is_iec559         = false;

		static constexpr value_type min() 
			{ return 0; }

		static constexpr value_type max()
			{ return EASTL_LIMITS_MAX_U(value_type); }

		static constexpr value_type lowest() 
			{ return 0; }

		static constexpr value_type epsilon() 
			{ return 0; }

		static constexpr value_type round_error() 
			{ return 0; }

		static constexpr value_type infinity() 
			{ return value_type(); }

		static constexpr value_type quiet_NaN() 
			{ return value_type(); }

		static constexpr value_type signaling_NaN()
			{ return value_type(); }

		static constexpr value_type denorm_min() 
			{ return static_cast<value_type>(0); }
	};


	// numeric_limits<signed long>
	template<>
	class numeric_limits<signed long>
	{
	public:
		typedef signed long value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = EASTL_LIMITS_DIGITS_S(value_type);
		static constexpr int                digits10          = EASTL_LIMITS_DIGITS10_S(value_type);
		static constexpr int                max_digits10      = 0;
		static constexpr bool               is_signed         = true;
		static constexpr bool               is_integer        = true;
		static constexpr bool               is_exact          = true;
		static constexpr int                radix             = 2;
		static constexpr int                min_exponent      = 0;
		static constexpr int                min_exponent10    = 0;
		static constexpr int                max_exponent      = 0;
		static constexpr int                max_exponent10    = 0;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = true;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_toward_zero;
		static constexpr bool               has_infinity      = false;
		static constexpr bool               has_quiet_NaN     = false;
		static constexpr bool               has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm        = denorm_absent;
		static constexpr bool               has_denorm_loss   = false;
		static constexpr bool               is_iec559         = false;

		static constexpr value_type min() 
			{ return LONG_MIN; }  // It's hard to get EASTL_LIMITS_MIN_S to work with all compilers here.

		static constexpr value_type max()
			{ return EASTL_LIMITS_MAX_S(value_type); }

		static constexpr value_type lowest() 
			{ return LONG_MIN; }

		static constexpr value_type epsilon() 
			{ return 0; }

		static constexpr value_type round_error() 
			{ return 0; }

		static constexpr value_type infinity() 
			{ return value_type(); }

		static constexpr value_type quiet_NaN() 
			{ return value_type(); }

		static constexpr value_type signaling_NaN()
			{ return value_type(); }

		static constexpr value_type denorm_min() 
			{ return static_cast<value_type>(0); }
	};


	// numeric_limits<unsigned long long>
	template<>
	class numeric_limits<unsigned long long>
	{
	public:
		typedef unsigned long long value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = EASTL_LIMITS_DIGITS_U(value_type);
		static constexpr int                digits10          = EASTL_LIMITS_DIGITS10_U(value_type);
		static constexpr int                max_digits10      = 0;
		static constexpr bool               is_signed         = false;
		static constexpr bool               is_integer        = true;
		static constexpr bool               is_exact          = true;
		static constexpr int                radix             = 2;
		static constexpr int                min_exponent      = 0;
		static constexpr int                min_exponent10    = 0;
		static constexpr int                max_exponent      = 0;
		static constexpr int                max_exponent10    = 0;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = true;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_toward_zero;
		static constexpr bool               has_infinity      = false;
		static constexpr bool               has_quiet_NaN     = false;
		static constexpr bool               has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm        = denorm_absent;
		static constexpr bool               has_denorm_loss   = false;
		static constexpr bool               is_iec559         = false;

		static constexpr value_type min() 
			{ return 0; }

		static constexpr value_type max()
			{ return EASTL_LIMITS_MAX_U(value_type); }

		static constexpr value_type lowest() 
			{ return 0; }

		static constexpr value_type epsilon() 
			{ return 0; }

		static constexpr value_type round_error() 
			{ return 0; }

		static constexpr value_type infinity() 
			{ return value_type(); }

		static constexpr value_type quiet_NaN() 
			{ return value_type(); }

		static constexpr value_type signaling_NaN()
			{ return value_type(); }

		static constexpr value_type denorm_min() 
			{ return static_cast<value_type>(0); }
	};


	// numeric_limits<signed long long>
	template<>
	class numeric_limits<signed long long>
	{
	public:
		typedef signed long long value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = EASTL_LIMITS_DIGITS_S(value_type);
		static constexpr int                digits10          = EASTL_LIMITS_DIGITS10_S(value_type);
		static constexpr int                max_digits10      = 0;
		static constexpr bool               is_signed         = true;
		static constexpr bool               is_integer        = true;
		static constexpr bool               is_exact          = true;
		static constexpr int                radix             = 2;
		static constexpr int                min_exponent      = 0;
		static constexpr int                min_exponent10    = 0;
		static constexpr int                max_exponent      = 0;
		static constexpr int                max_exponent10    = 0;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = true;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_toward_zero;
		static constexpr bool               has_infinity      = false;
		static constexpr bool               has_quiet_NaN     = false;
		static constexpr bool               has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm        = denorm_absent;
		static constexpr bool               has_denorm_loss   = false;
		static constexpr bool               is_iec559         = false;

		static constexpr value_type min() 
			{ return EASTL_LIMITS_MIN_S(value_type); }

		static constexpr value_type max()
			{ return EASTL_LIMITS_MAX_S(value_type); }

		static constexpr value_type lowest() 
			{ return EASTL_LIMITS_MIN_S(value_type); }

		static constexpr value_type epsilon() 
			{ return 0; }

		static constexpr value_type round_error() 
			{ return 0; }

		static constexpr value_type infinity() 
			{ return value_type(); }

		static constexpr value_type quiet_NaN() 
			{ return value_type(); }

		static constexpr value_type signaling_NaN()
			{ return value_type(); }

		static constexpr value_type denorm_min() 
			{ return static_cast<value_type>(0); }
	};


	#if (EA_COMPILER_INTMAX_SIZE >= 16) && (defined(EA_COMPILER_GNUC) || defined(__clang__)) // If __int128_t/__uint128_t is supported...
		// numeric_limits<__uint128_t>
		template<>
		class numeric_limits<__uint128_t>
		{
		public:
			typedef __uint128_t value_type;

			static constexpr bool               is_specialized    = true;
			static constexpr int                digits            = EASTL_LIMITS_DIGITS_U(value_type);
			static constexpr int                digits10          = EASTL_LIMITS_DIGITS10_U(value_type);
			static constexpr int                max_digits10      = 0;
			static constexpr bool               is_signed         = false;
			static constexpr bool               is_integer        = true;
			static constexpr bool               is_exact          = true;
			static constexpr int                radix             = 2;
			static constexpr int                min_exponent      = 0;
			static constexpr int                min_exponent10    = 0;
			static constexpr int                max_exponent      = 0;
			static constexpr int                max_exponent10    = 0;
			static constexpr bool               is_bounded        = true;
			static constexpr bool               is_modulo         = true;
			static constexpr bool               traps             = true;
			static constexpr bool               tinyness_before   = false;
			static constexpr float_round_style  round_style       = round_toward_zero;
			static constexpr bool               has_infinity      = false;
			static constexpr bool               has_quiet_NaN     = false;
			static constexpr bool               has_signaling_NaN = false;
			static constexpr float_denorm_style has_denorm        = denorm_absent;
			static constexpr bool               has_denorm_loss   = false;
			static constexpr bool               is_iec559         = false;

			static constexpr value_type min() 
				{ return 0; }

			static constexpr value_type max()
				{ return EASTL_LIMITS_MAX_U(value_type); }

			static constexpr value_type lowest() 
				{ return 0; }

			static constexpr value_type epsilon() 
				{ return 0; }

			static constexpr value_type round_error() 
				{ return 0; }

			static constexpr value_type infinity() 
				{ return value_type(); }

			static constexpr value_type quiet_NaN() 
				{ return value_type(); }

			static constexpr value_type signaling_NaN()
				{ return value_type(); }

			static constexpr value_type denorm_min() 
				{ return static_cast<value_type>(0); }
		};


		// numeric_limits<__int128_t>
		template<>
		class numeric_limits<__int128_t>
		{
		public:
			typedef __int128_t value_type;

			static constexpr bool               is_specialized    = true;
			static constexpr int                digits            = EASTL_LIMITS_DIGITS_S(value_type);
			static constexpr int                digits10          = EASTL_LIMITS_DIGITS10_S(value_type);
			static constexpr int                max_digits10      = 0;
			static constexpr bool               is_signed         = true;
			static constexpr bool               is_integer        = true;
			static constexpr bool               is_exact          = true;
			static constexpr int                radix             = 2;
			static constexpr int                min_exponent      = 0;
			static constexpr int                min_exponent10    = 0;
			static constexpr int                max_exponent      = 0;
			static constexpr int                max_exponent10    = 0;
			static constexpr bool               is_bounded        = true;
			static constexpr bool               is_modulo         = true;
			static constexpr bool               traps             = true;
			static constexpr bool               tinyness_before   = false;
			static constexpr float_round_style  round_style       = round_toward_zero;
			static constexpr bool               has_infinity      = false;
			static constexpr bool               has_quiet_NaN     = false;
			static constexpr bool               has_signaling_NaN = false;
			static constexpr float_denorm_style has_denorm        = denorm_absent;
			static constexpr bool               has_denorm_loss   = false;
			static constexpr bool               is_iec559         = false;

			static constexpr value_type min() 
				{ return EASTL_LIMITS_MIN_S(value_type); }

			static constexpr value_type max()
				{ return EASTL_LIMITS_MAX_S(value_type); }

			static constexpr value_type lowest() 
				{ return EASTL_LIMITS_MIN_S(value_type); }

			static constexpr value_type epsilon() 
				{ return 0; }

			static constexpr value_type round_error() 
				{ return 0; }

			static constexpr value_type infinity() 
				{ return value_type(); }

			static constexpr value_type quiet_NaN() 
				{ return value_type(); }

			static constexpr value_type signaling_NaN()
				{ return value_type(); }

			static constexpr value_type denorm_min() 
				{ return static_cast<value_type>(0); }
		};
	#endif


	// numeric_limits<float>
	template<>
	class numeric_limits<float>
	{
	public:
		typedef float value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = FLT_MANT_DIG;
		static constexpr int                digits10          = FLT_DIG;
		static constexpr int                max_digits10      = FLT_MANT_DIG;
		static constexpr bool               is_signed         = true;
		static constexpr bool               is_integer        = false;
		static constexpr bool               is_exact          = false;
		static constexpr int                radix             = FLT_RADIX;
		static constexpr int                min_exponent      = FLT_MIN_EXP;
		static constexpr int                min_exponent10    = FLT_MIN_10_EXP;
		static constexpr int                max_exponent      = FLT_MAX_EXP;
		static constexpr int                max_exponent10    = FLT_MAX_10_EXP;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = false;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_to_nearest;
		static constexpr bool               has_infinity      = true;
		static constexpr bool               has_quiet_NaN     = true;                   // This may be wrong for some platforms.
		static constexpr bool               has_signaling_NaN = true;                   // This may be wrong for some platforms.
		static constexpr float_denorm_style has_denorm        = denorm_present;         // This may be wrong for some platforms.
		static constexpr bool               has_denorm_loss   = false;                  // This may be wrong for some platforms.
		static constexpr bool               is_iec559         = has_infinity && has_quiet_NaN && (has_denorm == denorm_present);

		#if (defined(EA_COMPILER_GNUC) || defined(__clang__)) && defined(__FLT_MIN__)
			static constexpr value_type min()
				{ return __FLT_MIN__; }

			static constexpr value_type max()
				{ return __FLT_MAX__; }

			static constexpr value_type lowest() 
				{ return -__FLT_MAX__; }

			static constexpr value_type epsilon() 
				{ return __FLT_EPSILON__; }

			static constexpr value_type round_error() 
				{ return 0.5f; }

			static constexpr value_type infinity() 
				{ return __builtin_huge_valf(); }

			static constexpr value_type quiet_NaN() 
				{ return __builtin_nanf(""); }

			static constexpr value_type signaling_NaN()
				{ return __builtin_nansf(""); }

			static constexpr value_type denorm_min() 
				{ return __FLT_DENORM_MIN__; }

		#elif defined(_CPPLIB_VER) // If using the Dinkumware Standard library...
			static constexpr value_type min()
				{ return FLT_MIN; }

			static constexpr value_type max()
				{ return FLT_MAX; }

			static constexpr value_type lowest() 
				{ return -FLT_MAX; }

			static constexpr value_type epsilon() 
				{ return FLT_EPSILON; }

			static constexpr value_type round_error() 
				{ return 0.5f; }

			#if defined(_MSVC_STL_UPDATE) && _MSVC_STL_UPDATE >= 202206L // If using a recent version of MSVC's STL...
				static constexpr value_type infinity()
					{ return __builtin_huge_valf(); }

				static constexpr value_type quiet_NaN()
					{ return __builtin_nanf("0"); }

				static constexpr value_type signaling_NaN()
					{ return __builtin_nansf("1"); }

				static constexpr value_type denorm_min()
					{ return FLT_TRUE_MIN; }
			#else
				static constexpr value_type infinity() 
					{ return _CSTD _FInf._Float; }

				static constexpr value_type quiet_NaN() 
					{ return _CSTD _FNan._Float; }

				static constexpr value_type signaling_NaN()
					{ return _CSTD _FSnan._Float; } 

				static constexpr value_type denorm_min() 
					{ return _CSTD _FDenorm._Float; }
			#endif
		#endif
	};


	// numeric_limits<double>
	template<>
	class numeric_limits<double>
	{
	public:
		typedef double value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = DBL_MANT_DIG;
		static constexpr int                digits10          = DBL_DIG;
		static constexpr int                max_digits10      = DBL_MANT_DIG;
		static constexpr bool               is_signed         = true;
		static constexpr bool               is_integer        = false;
		static constexpr bool               is_exact          = false;
		static constexpr int                radix             = FLT_RADIX;              // FLT_RADIX applies to all floating point types.
		static constexpr int                min_exponent      = DBL_MIN_EXP;
		static constexpr int                min_exponent10    = DBL_MIN_10_EXP;
		static constexpr int                max_exponent      = DBL_MAX_EXP;
		static constexpr int                max_exponent10    = DBL_MAX_10_EXP;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = false;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_to_nearest;
		static constexpr bool               has_infinity      = true;
		static constexpr bool               has_quiet_NaN     = true;                   // This may be wrong for some platforms.
		static constexpr bool               has_signaling_NaN = true;                   // This may be wrong for some platforms.
		static constexpr float_denorm_style has_denorm        = denorm_present;         // This may be wrong for some platforms.
		static constexpr bool               has_denorm_loss   = false;                  // This may be wrong for some platforms.
		static constexpr bool               is_iec559         = has_infinity && has_quiet_NaN && (has_denorm == denorm_present);

		#if (defined(EA_COMPILER_GNUC) || defined(__clang__)) && defined(__DBL_MIN__)
			static constexpr value_type min()
				{ return __DBL_MIN__; }

			static constexpr value_type max()
				{ return __DBL_MAX__; }

			static constexpr value_type lowest() 
				{ return -__DBL_MAX__; }

			static constexpr value_type epsilon() 
				{ return __DBL_EPSILON__; }

			static constexpr value_type round_error() 
				{ return 0.5f; }

			static constexpr value_type infinity() 
				{ return __builtin_huge_val(); }

			static constexpr value_type quiet_NaN() 
				{ return __builtin_nan(""); }

			static constexpr value_type signaling_NaN()
				{ return __builtin_nans(""); }

			static constexpr value_type denorm_min() 
				{ return __DBL_DENORM_MIN__; }

		#elif defined(_CPPLIB_VER) // If using the Dinkumware Standard library...
			static constexpr value_type min()
				{ return DBL_MIN; }

			static constexpr value_type max()
				{ return DBL_MAX; }

			static constexpr value_type lowest() 
				{ return -DBL_MAX; }

			static constexpr value_type epsilon() 
				{ return DBL_EPSILON; }

			static constexpr value_type round_error() 
				{ return 0.5f; }

			#if defined(_MSVC_STL_UPDATE) && _MSVC_STL_UPDATE >= 202206L // If using a recent version of MSVC's STL...
				static constexpr value_type infinity()
					{ return __builtin_huge_val(); }

				static constexpr value_type quiet_NaN()
					{ return __builtin_nan("0"); }

				static constexpr value_type signaling_NaN()
					{ return __builtin_nans("1"); }

				static constexpr value_type denorm_min()
					{ return DBL_TRUE_MIN; }
			#else
				static constexpr value_type infinity() 
					{ return _CSTD _Inf._Double; }

				static constexpr value_type quiet_NaN() 
					{ return _CSTD _Nan._Double; }

				static constexpr value_type signaling_NaN()
					{ return _CSTD _Snan._Double; } 

				static constexpr value_type denorm_min() 
					{ return _CSTD _Denorm._Double; }
			#endif
		#endif
	};


	// numeric_limits<long double>
	template<>
	class numeric_limits<long double>
	{
	public:
		typedef long double value_type;

		static constexpr bool               is_specialized    = true;
		static constexpr int                digits            = LDBL_MANT_DIG;
		static constexpr int                digits10          = LDBL_DIG;
		static constexpr int                max_digits10      = LDBL_MANT_DIG;
		static constexpr bool               is_signed         = true;
		static constexpr bool               is_integer        = false;
		static constexpr bool               is_exact          = false;
		static constexpr int                radix             = FLT_RADIX;              // FLT_RADIX applies to all floating point types.
		static constexpr int                min_exponent      = LDBL_MIN_EXP;
		static constexpr int                min_exponent10    = LDBL_MIN_10_EXP;
		static constexpr int                max_exponent      = LDBL_MAX_EXP;
		static constexpr int                max_exponent10    = LDBL_MAX_10_EXP;
		static constexpr bool               is_bounded        = true;
		static constexpr bool               is_modulo         = false;
		static constexpr bool               traps             = true;
		static constexpr bool               tinyness_before   = false;
		static constexpr float_round_style  round_style       = round_to_nearest;
		static constexpr bool               has_infinity      = true;
		static constexpr bool               has_quiet_NaN     = true;                   // This may be wrong for some platforms.
		static constexpr bool               has_signaling_NaN = true;                   // This may be wrong for some platforms.
		static constexpr float_denorm_style has_denorm        = denorm_present;         // This may be wrong for some platforms.
		static constexpr bool               has_denorm_loss   = false;                  // This may be wrong for some platforms.
		static constexpr bool               is_iec559         = has_infinity && has_quiet_NaN && (has_denorm == denorm_present);

		#if (defined(EA_COMPILER_GNUC) || defined(__clang__)) && defined(__LDBL_MIN__)
			static constexpr value_type min()
				{ return __LDBL_MIN__; }

			static constexpr value_type max()
				{ return __LDBL_MAX__; }

			static constexpr value_type lowest() 
				{ return -__LDBL_MAX__; }

			static constexpr value_type epsilon() 
				{ return __LDBL_EPSILON__; }

			static constexpr value_type round_error() 
				{ return 0.5f; }

			static constexpr value_type infinity() 
				{ return __builtin_huge_val(); }

			static constexpr value_type quiet_NaN() 
				{ return __builtin_nan(""); }

			static constexpr value_type signaling_NaN()
				{ return __builtin_nans(""); }

			static constexpr value_type denorm_min() 
				{ return __LDBL_DENORM_MIN__; }

		#elif defined(_CPPLIB_VER) // If using the Dinkumware Standard library...
			static constexpr value_type min()
				{ return LDBL_MIN; }

			static constexpr value_type max()
				{ return LDBL_MAX; }

			static constexpr value_type lowest() 
				{ return -LDBL_MAX; }

			static constexpr value_type epsilon() 
				{ return LDBL_EPSILON; }

			static constexpr value_type round_error() 
				{ return 0.5f; }

			#if defined(_MSVC_STL_UPDATE) && _MSVC_STL_UPDATE >= 202206L // If using a recent version of MSVC's STL...
				static constexpr value_type infinity()
					{ return __builtin_huge_val(); }

				static constexpr value_type quiet_NaN()
					{ return __builtin_nan("0"); }

				static constexpr value_type signaling_NaN()
					{ return __builtin_nans("1"); }

				static constexpr value_type denorm_min()
					{ return LDBL_TRUE_MIN; }
			#else
				static constexpr value_type infinity() 
					{ return _CSTD _LInf._Long_double; }

				static constexpr value_type quiet_NaN() 
					{ return _CSTD _LNan._Long_double; }

				static constexpr value_type signaling_NaN()
					{ return _CSTD _LSnan._Long_double; } 

				static constexpr value_type denorm_min() 
					{ return _CSTD _LDenorm._Long_double; }
			#endif
		#endif
	};

} // namespace eastl


EA_RESTORE_VC_WARNING()


#endif // Header include guard























