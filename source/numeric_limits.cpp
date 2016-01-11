///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EASTL/numeric_limits.h>


#if EASTL_CUSTOM_FLOAT_CONSTANTS_REQUIRED
	#include <limits> // See notes below about usage of this header.

	namespace eastl
	{
		namespace Internal
		{
			// For this platformc/compiler combination we fall back to using std::numeric_limits,
			// which is available for with most compilers and platforms, though it doesn't necessarily
			// support the C++11 functionality that we do. However, we need it just for the four 
			// floating point types. Note that this code isn't used for most EA platforms, as 
			// most platforms use GCC, clang, VC++ (yvals), or Dinkumware (yvals).
			// To do: Initialize these values via a means that doesn't depend on std::numeric_limits.

			EASTL_API float       gFloatInfinity      = std::numeric_limits<float>::infinity();
			EASTL_API float       gFloatNaN           = std::numeric_limits<float>::quiet_NaN();
			EASTL_API float       gFloatSNaN          = std::numeric_limits<float>::signaling_NaN();
			EASTL_API float       gFloatDenorm        = std::numeric_limits<float>::denorm_min();

			EASTL_API double      gDoubleInfinity     = std::numeric_limits<double>::infinity();
			EASTL_API double      gDoubleNaN          = std::numeric_limits<double>::quiet_NaN();
			EASTL_API double      gDoubleSNaN         = std::numeric_limits<double>::signaling_NaN();
			EASTL_API double      gDoubleDenorm       = std::numeric_limits<double>::denorm_min();

			EASTL_API long double gLongDoubleInfinity = std::numeric_limits<long double>::infinity();
			EASTL_API long double gLongDoubleNaN      = std::numeric_limits<long double>::quiet_NaN();
			EASTL_API long double gLongDoubleSNaN     = std::numeric_limits<long double>::signaling_NaN();
			EASTL_API long double gLongDoubleDenorm   = std::numeric_limits<long double>::denorm_min();
		}
   } 
#endif


#if defined(_MSC_VER)
	// VC++ has a long-standing bug: it fails to allow the definition of static const member variables
	// outside the declaration within the class. The C++ Standard actually requires that they be defined
	// and some other compilers fail to link if they aren't. So we simply don't define the members for VC++.
	// See the C++ Standard Sec. 9.4.2 paragraph 4, which makes this clear.
	// http://bytes.com/topic/c/answers/710704-const-static-initialization-visual-studio
#else

	namespace eastl
	{
		namespace Internal
		{
			EA_CONSTEXPR_OR_CONST bool               numeric_limits_base::is_specialized;
			EA_CONSTEXPR_OR_CONST int                numeric_limits_base::digits;
			EA_CONSTEXPR_OR_CONST int                numeric_limits_base::digits10;
			EA_CONSTEXPR_OR_CONST int                numeric_limits_base::max_digits10;
			EA_CONSTEXPR_OR_CONST bool               numeric_limits_base::is_signed;
			EA_CONSTEXPR_OR_CONST bool               numeric_limits_base::is_integer;
			EA_CONSTEXPR_OR_CONST bool               numeric_limits_base::is_exact;
			EA_CONSTEXPR_OR_CONST int                numeric_limits_base::radix;
			EA_CONSTEXPR_OR_CONST int                numeric_limits_base::min_exponent;
			EA_CONSTEXPR_OR_CONST int                numeric_limits_base::min_exponent10;
			EA_CONSTEXPR_OR_CONST int                numeric_limits_base::max_exponent;
			EA_CONSTEXPR_OR_CONST int                numeric_limits_base::max_exponent10;
			EA_CONSTEXPR_OR_CONST bool               numeric_limits_base::is_bounded;
			EA_CONSTEXPR_OR_CONST bool               numeric_limits_base::is_modulo;
			EA_CONSTEXPR_OR_CONST bool               numeric_limits_base::traps;
			EA_CONSTEXPR_OR_CONST bool               numeric_limits_base::tinyness_before;
			EA_CONSTEXPR_OR_CONST float_round_style  numeric_limits_base::round_style;
			EA_CONSTEXPR_OR_CONST bool               numeric_limits_base::has_infinity;
			EA_CONSTEXPR_OR_CONST bool               numeric_limits_base::has_quiet_NaN;
			EA_CONSTEXPR_OR_CONST bool               numeric_limits_base::has_signaling_NaN;
			EA_CONSTEXPR_OR_CONST float_denorm_style numeric_limits_base::has_denorm;
			EA_CONSTEXPR_OR_CONST bool               numeric_limits_base::has_denorm_loss;
			EA_CONSTEXPR_OR_CONST bool               numeric_limits_base::is_iec559;
		}

		// bool
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<bool>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<bool>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<bool>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<bool>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<bool>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<bool>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<bool>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<bool>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<bool>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<bool>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<bool>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<bool>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<bool>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<bool>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<bool>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<bool>::round_style;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<bool>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<bool>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<bool>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<bool>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<bool>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<bool>::is_iec559;

		// char
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<char>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<char>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<char>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<char>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<char>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<char>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<char>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<char>::round_style;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<char>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char>::is_iec559;

		// unsigned char
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned char>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned char>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned char>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned char>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned char>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned char>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned char>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned char>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned char>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned char>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned char>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned char>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned char>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned char>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned char>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<unsigned char>::round_style;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned char>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned char>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned char>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<unsigned char>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned char>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned char>::is_iec559;

		// signed char
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<signed char>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<signed char>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<signed char>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<signed char>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<signed char>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<signed char>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<signed char>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<signed char>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<signed char>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<signed char>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<signed char>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<signed char>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<signed char>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<signed char>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<signed char>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<signed char>::round_style;

		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<signed char>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<signed char>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<signed char>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<signed char>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<signed char>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<signed char>::is_iec559;

		// wchar_t
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<wchar_t>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<wchar_t>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<wchar_t>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<wchar_t>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<wchar_t>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<wchar_t>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<wchar_t>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<wchar_t>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<wchar_t>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<wchar_t>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<wchar_t>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<wchar_t>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<wchar_t>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<wchar_t>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<wchar_t>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<wchar_t>::round_style;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<wchar_t>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<wchar_t>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<wchar_t>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<wchar_t>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<wchar_t>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<wchar_t>::is_iec559;

		// char16_t
		#if EA_CHAR16_NATIVE // If char16_t is a true unique type (as called for by the C++11 Standard)...
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char16_t>::is_specialized;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<char16_t>::digits;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<char16_t>::digits10;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char16_t>::is_signed;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char16_t>::is_integer;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char16_t>::is_exact;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<char16_t>::radix;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<char16_t>::min_exponent;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<char16_t>::min_exponent10;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<char16_t>::max_exponent;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<char16_t>::max_exponent10;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char16_t>::is_bounded;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char16_t>::is_modulo;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char16_t>::traps;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char16_t>::tinyness_before;
			EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<char16_t>::round_style;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char16_t>::has_infinity;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char16_t>::has_quiet_NaN;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char16_t>::has_signaling_NaN;
			EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<char16_t>::has_denorm;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char16_t>::has_denorm_loss;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char16_t>::is_iec559;
		#endif

		// char32_t
		#if EA_CHAR32_NATIVE // If char32_t is a true unique type (as called for by the C++11 Standard)...
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char32_t>::is_specialized;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<char32_t>::digits;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<char32_t>::digits10;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char32_t>::is_signed;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char32_t>::is_integer;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char32_t>::is_exact;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<char32_t>::radix;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<char32_t>::min_exponent;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<char32_t>::min_exponent10;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<char32_t>::max_exponent;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<char32_t>::max_exponent10;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char32_t>::is_bounded;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char32_t>::is_modulo;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char32_t>::traps;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char32_t>::tinyness_before;
			EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<char32_t>::round_style;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char32_t>::has_infinity;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char32_t>::has_quiet_NaN;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char32_t>::has_signaling_NaN;
			EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<char32_t>::has_denorm;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char32_t>::has_denorm_loss;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<char32_t>::is_iec559;
		#endif
	
		// unsigned short
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned short>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned short>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned short>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned short>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned short>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned short>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned short>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned short>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned short>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned short>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned short>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned short>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned short>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned short>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned short>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<unsigned short>::round_style;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned short>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned short>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned short>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<unsigned short>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned short>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned short>::is_iec559;

		// short
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<short>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<short>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<short>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<short>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<short>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<short>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<short>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<short>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<short>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<short>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<short>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<short>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<short>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<short>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<short>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<short>::round_style;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<short>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<short>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<short>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<short>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<short>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<short>::is_iec559;

		// unsigned int
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned int>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned int>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned int>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned int>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned int>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned int>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned int>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned int>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned int>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned int>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned int>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned int>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned int>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned int>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned int>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<unsigned int>::round_style;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned int>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned int>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned int>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<unsigned int>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned int>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned int>::is_iec559;

		// int
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<int>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<int>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<int>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<int>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<int>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<int>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<int>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<int>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<int>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<int>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<int>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<int>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<int>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<int>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<int>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<int>::round_style;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<int>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<int>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<int>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<int>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<int>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<int>::is_iec559;

		// unsigned long
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned long>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned long>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned long>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned long>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned long>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned long>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned long>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<unsigned long>::round_style;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<unsigned long>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long>::is_iec559;

		// long
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<long>::round_style;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<long>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long>::is_iec559;

		// unsigned long long
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long long>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned long long>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned long long>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long long>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long long>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long long>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned long long>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned long long>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned long long>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned long long>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<unsigned long long>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long long>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long long>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long long>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long long>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<unsigned long long>::round_style;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long long>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long long>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long long>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<unsigned long long>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long long>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<unsigned long long>::is_iec559;

		// long long
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long long>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long long>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long long>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long long>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long long>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long long>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long long>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long long>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long long>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long long>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long long>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long long>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long long>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long long>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long long>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<long long>::round_style;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long long>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long long>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long long>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<long long>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long long>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long long>::is_iec559;

		// __uint128_t
		#if (EA_COMPILER_INTMAX_SIZE >= 16) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)) // If __int128_t/__uint128_t is supported...
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__uint128_t>::is_specialized;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<__uint128_t>::digits;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<__uint128_t>::digits10;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__uint128_t>::is_signed;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__uint128_t>::is_integer;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__uint128_t>::is_exact;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<__uint128_t>::radix;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<__uint128_t>::min_exponent;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<__uint128_t>::min_exponent10;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<__uint128_t>::max_exponent;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<__uint128_t>::max_exponent10;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__uint128_t>::is_bounded;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__uint128_t>::is_modulo;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__uint128_t>::traps;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__uint128_t>::tinyness_before;
			EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<__uint128_t>::round_style;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__uint128_t>::has_infinity;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__uint128_t>::has_quiet_NaN;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__uint128_t>::has_signaling_NaN;
			EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<__uint128_t>::has_denorm;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__uint128_t>::has_denorm_loss;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__uint128_t>::is_iec559;
		#endif

		// __int128_t
		#if (EA_COMPILER_INTMAX_SIZE >= 16) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)) // If __int128_t/__uint128_t is supported...
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__int128_t>::is_specialized;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<__int128_t>::digits;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<__int128_t>::digits10;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__int128_t>::is_signed;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__int128_t>::is_integer;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__int128_t>::is_exact;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<__int128_t>::radix;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<__int128_t>::min_exponent;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<__int128_t>::min_exponent10;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<__int128_t>::max_exponent;
			EA_CONSTEXPR_OR_CONST int                   numeric_limits<__int128_t>::max_exponent10;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__int128_t>::is_bounded;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__int128_t>::is_modulo;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__int128_t>::traps;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__int128_t>::tinyness_before;
			EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<__int128_t>::round_style;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__int128_t>::has_infinity;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__int128_t>::has_quiet_NaN;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__int128_t>::has_signaling_NaN;
			EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<__int128_t>::has_denorm;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__int128_t>::has_denorm_loss;
			EA_CONSTEXPR_OR_CONST bool                  numeric_limits<__int128_t>::is_iec559;
		#endif

		// float
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<float>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<float>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<float>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<float>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<float>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<float>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<float>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<float>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<float>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<float>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<float>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<float>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<float>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<float>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<float>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<float>::round_style;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<float>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<float>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<float>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<float>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<float>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<float>::is_iec559;

		// double
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<double>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<double>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<double>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<double>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<double>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<double>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<double>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<double>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<double>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<double>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<double>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<double>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<double>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<double>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<double>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<double>::round_style;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<double>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<double>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<double>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<double>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<double>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<double>::is_iec559;

		// long double
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long double>::is_specialized;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long double>::digits;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long double>::digits10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long double>::is_signed;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long double>::is_integer;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long double>::is_exact;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long double>::radix;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long double>::min_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long double>::min_exponent10;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long double>::max_exponent;
		EA_CONSTEXPR_OR_CONST int                   numeric_limits<long double>::max_exponent10;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long double>::is_bounded;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long double>::is_modulo;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long double>::traps;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long double>::tinyness_before;
		EA_CONSTEXPR_OR_CONST float_round_style     numeric_limits<long double>::round_style;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long double>::has_infinity;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long double>::has_quiet_NaN;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long double>::has_signaling_NaN;
		EA_CONSTEXPR_OR_CONST float_denorm_style    numeric_limits<long double>::has_denorm;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long double>::has_denorm_loss;
		EA_CONSTEXPR_OR_CONST bool                  numeric_limits<long double>::is_iec559;

	} // namespace eastl

#endif // (VC++ 2010 or earlier)


