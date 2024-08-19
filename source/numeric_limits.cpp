///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EASTL/numeric_limits.h>


namespace eastl
{
	// bool
	constexpr bool                  numeric_limits<bool>::is_specialized;
	constexpr int                   numeric_limits<bool>::digits;
	constexpr int                   numeric_limits<bool>::digits10;
	constexpr bool                  numeric_limits<bool>::is_signed;
	constexpr bool                  numeric_limits<bool>::is_integer;
	constexpr bool                  numeric_limits<bool>::is_exact;
	constexpr int                   numeric_limits<bool>::radix;
	constexpr int                   numeric_limits<bool>::min_exponent;
	constexpr int                   numeric_limits<bool>::min_exponent10;
	constexpr int                   numeric_limits<bool>::max_exponent;
	constexpr int                   numeric_limits<bool>::max_exponent10;
	constexpr bool                  numeric_limits<bool>::is_bounded;
	constexpr bool                  numeric_limits<bool>::is_modulo;
	constexpr bool                  numeric_limits<bool>::traps;
	constexpr bool                  numeric_limits<bool>::tinyness_before;
	constexpr float_round_style     numeric_limits<bool>::round_style;
	constexpr bool                  numeric_limits<bool>::has_infinity;
	constexpr bool                  numeric_limits<bool>::has_quiet_NaN;
	constexpr bool                  numeric_limits<bool>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<bool>::has_denorm;
	constexpr bool                  numeric_limits<bool>::has_denorm_loss;
	constexpr bool                  numeric_limits<bool>::is_iec559;

	// char
	constexpr bool                  numeric_limits<char>::is_specialized;
	constexpr int                   numeric_limits<char>::digits;
	constexpr int                   numeric_limits<char>::digits10;
	constexpr bool                  numeric_limits<char>::is_signed;
	constexpr bool                  numeric_limits<char>::is_integer;
	constexpr bool                  numeric_limits<char>::is_exact;
	constexpr int                   numeric_limits<char>::radix;
	constexpr int                   numeric_limits<char>::min_exponent;
	constexpr int                   numeric_limits<char>::min_exponent10;
	constexpr int                   numeric_limits<char>::max_exponent;
	constexpr int                   numeric_limits<char>::max_exponent10;
	constexpr bool                  numeric_limits<char>::is_bounded;
	constexpr bool                  numeric_limits<char>::is_modulo;
	constexpr bool                  numeric_limits<char>::traps;
	constexpr bool                  numeric_limits<char>::tinyness_before;
	constexpr float_round_style     numeric_limits<char>::round_style;
	constexpr bool                  numeric_limits<char>::has_infinity;
	constexpr bool                  numeric_limits<char>::has_quiet_NaN;
	constexpr bool                  numeric_limits<char>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<char>::has_denorm;
	constexpr bool                  numeric_limits<char>::has_denorm_loss;
	constexpr bool                  numeric_limits<char>::is_iec559;

	// unsigned char
	constexpr bool                  numeric_limits<unsigned char>::is_specialized;
	constexpr int                   numeric_limits<unsigned char>::digits;
	constexpr int                   numeric_limits<unsigned char>::digits10;
	constexpr bool                  numeric_limits<unsigned char>::is_signed;
	constexpr bool                  numeric_limits<unsigned char>::is_integer;
	constexpr bool                  numeric_limits<unsigned char>::is_exact;
	constexpr int                   numeric_limits<unsigned char>::radix;
	constexpr int                   numeric_limits<unsigned char>::min_exponent;
	constexpr int                   numeric_limits<unsigned char>::min_exponent10;
	constexpr int                   numeric_limits<unsigned char>::max_exponent;
	constexpr int                   numeric_limits<unsigned char>::max_exponent10;
	constexpr bool                  numeric_limits<unsigned char>::is_bounded;
	constexpr bool                  numeric_limits<unsigned char>::is_modulo;
	constexpr bool                  numeric_limits<unsigned char>::traps;
	constexpr bool                  numeric_limits<unsigned char>::tinyness_before;
	constexpr float_round_style     numeric_limits<unsigned char>::round_style;
	constexpr bool                  numeric_limits<unsigned char>::has_infinity;
	constexpr bool                  numeric_limits<unsigned char>::has_quiet_NaN;
	constexpr bool                  numeric_limits<unsigned char>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<unsigned char>::has_denorm;
	constexpr bool                  numeric_limits<unsigned char>::has_denorm_loss;
	constexpr bool                  numeric_limits<unsigned char>::is_iec559;

	// signed char
	constexpr bool                  numeric_limits<signed char>::is_specialized;
	constexpr int                   numeric_limits<signed char>::digits;
	constexpr int                   numeric_limits<signed char>::digits10;
	constexpr bool                  numeric_limits<signed char>::is_signed;
	constexpr bool                  numeric_limits<signed char>::is_integer;
	constexpr bool                  numeric_limits<signed char>::is_exact;
	constexpr int                   numeric_limits<signed char>::radix;
	constexpr int                   numeric_limits<signed char>::min_exponent;
	constexpr int                   numeric_limits<signed char>::min_exponent10;
	constexpr int                   numeric_limits<signed char>::max_exponent;
	constexpr int                   numeric_limits<signed char>::max_exponent10;
	constexpr bool                  numeric_limits<signed char>::is_bounded;
	constexpr bool                  numeric_limits<signed char>::is_modulo;
	constexpr bool                  numeric_limits<signed char>::traps;
	constexpr bool                  numeric_limits<signed char>::tinyness_before;
	constexpr float_round_style     numeric_limits<signed char>::round_style;

	constexpr bool                  numeric_limits<signed char>::has_infinity;
	constexpr bool                  numeric_limits<signed char>::has_quiet_NaN;
	constexpr bool                  numeric_limits<signed char>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<signed char>::has_denorm;
	constexpr bool                  numeric_limits<signed char>::has_denorm_loss;
	constexpr bool                  numeric_limits<signed char>::is_iec559;

	// wchar_t
	constexpr bool                  numeric_limits<wchar_t>::is_specialized;
	constexpr int                   numeric_limits<wchar_t>::digits;
	constexpr int                   numeric_limits<wchar_t>::digits10;
	constexpr bool                  numeric_limits<wchar_t>::is_signed;
	constexpr bool                  numeric_limits<wchar_t>::is_integer;
	constexpr bool                  numeric_limits<wchar_t>::is_exact;
	constexpr int                   numeric_limits<wchar_t>::radix;
	constexpr int                   numeric_limits<wchar_t>::min_exponent;
	constexpr int                   numeric_limits<wchar_t>::min_exponent10;
	constexpr int                   numeric_limits<wchar_t>::max_exponent;
	constexpr int                   numeric_limits<wchar_t>::max_exponent10;
	constexpr bool                  numeric_limits<wchar_t>::is_bounded;
	constexpr bool                  numeric_limits<wchar_t>::is_modulo;
	constexpr bool                  numeric_limits<wchar_t>::traps;
	constexpr bool                  numeric_limits<wchar_t>::tinyness_before;
	constexpr float_round_style     numeric_limits<wchar_t>::round_style;
	constexpr bool                  numeric_limits<wchar_t>::has_infinity;
	constexpr bool                  numeric_limits<wchar_t>::has_quiet_NaN;
	constexpr bool                  numeric_limits<wchar_t>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<wchar_t>::has_denorm;
	constexpr bool                  numeric_limits<wchar_t>::has_denorm_loss;
	constexpr bool                  numeric_limits<wchar_t>::is_iec559;

	// char8_t
	#if defined(EA_CHAR8_UNIQUE) && EA_CHAR8_UNIQUE // If char8_t is a true unique type (as called for by the C++20 Standard)
		constexpr bool                  numeric_limits<char8_t>::is_specialized;
		constexpr int                   numeric_limits<char8_t>::digits;
		constexpr int                   numeric_limits<char8_t>::digits10;
		constexpr bool                  numeric_limits<char8_t>::is_signed;
		constexpr bool                  numeric_limits<char8_t>::is_integer;
		constexpr bool                  numeric_limits<char8_t>::is_exact;
		constexpr int                   numeric_limits<char8_t>::radix;
		constexpr int                   numeric_limits<char8_t>::min_exponent;
		constexpr int                   numeric_limits<char8_t>::min_exponent10;
		constexpr int                   numeric_limits<char8_t>::max_exponent;
		constexpr int                   numeric_limits<char8_t>::max_exponent10;
		constexpr bool                  numeric_limits<char8_t>::is_bounded;
		constexpr bool                  numeric_limits<char8_t>::is_modulo;
		constexpr bool                  numeric_limits<char8_t>::traps;
		constexpr bool                  numeric_limits<char8_t>::tinyness_before;
		constexpr float_round_style     numeric_limits<char8_t>::round_style;
		constexpr bool                  numeric_limits<char8_t>::has_infinity;
		constexpr bool                  numeric_limits<char8_t>::has_quiet_NaN;
		constexpr bool                  numeric_limits<char8_t>::has_signaling_NaN;
		constexpr float_denorm_style    numeric_limits<char8_t>::has_denorm;
		constexpr bool                  numeric_limits<char8_t>::has_denorm_loss;
		constexpr bool                  numeric_limits<char8_t>::is_iec559;
	#endif

	// char16_t
	#if EA_CHAR16_NATIVE // If char16_t is a true unique type (as called for by the C++11 Standard)...
		constexpr bool                  numeric_limits<char16_t>::is_specialized;
		constexpr int                   numeric_limits<char16_t>::digits;
		constexpr int                   numeric_limits<char16_t>::digits10;
		constexpr bool                  numeric_limits<char16_t>::is_signed;
		constexpr bool                  numeric_limits<char16_t>::is_integer;
		constexpr bool                  numeric_limits<char16_t>::is_exact;
		constexpr int                   numeric_limits<char16_t>::radix;
		constexpr int                   numeric_limits<char16_t>::min_exponent;
		constexpr int                   numeric_limits<char16_t>::min_exponent10;
		constexpr int                   numeric_limits<char16_t>::max_exponent;
		constexpr int                   numeric_limits<char16_t>::max_exponent10;
		constexpr bool                  numeric_limits<char16_t>::is_bounded;
		constexpr bool                  numeric_limits<char16_t>::is_modulo;
		constexpr bool                  numeric_limits<char16_t>::traps;
		constexpr bool                  numeric_limits<char16_t>::tinyness_before;
		constexpr float_round_style     numeric_limits<char16_t>::round_style;
		constexpr bool                  numeric_limits<char16_t>::has_infinity;
		constexpr bool                  numeric_limits<char16_t>::has_quiet_NaN;
		constexpr bool                  numeric_limits<char16_t>::has_signaling_NaN;
		constexpr float_denorm_style    numeric_limits<char16_t>::has_denorm;
		constexpr bool                  numeric_limits<char16_t>::has_denorm_loss;
		constexpr bool                  numeric_limits<char16_t>::is_iec559;
	#endif

	// char32_t
	#if EA_CHAR32_NATIVE // If char32_t is a true unique type (as called for by the C++11 Standard)...
		constexpr bool                  numeric_limits<char32_t>::is_specialized;
		constexpr int                   numeric_limits<char32_t>::digits;
		constexpr int                   numeric_limits<char32_t>::digits10;
		constexpr bool                  numeric_limits<char32_t>::is_signed;
		constexpr bool                  numeric_limits<char32_t>::is_integer;
		constexpr bool                  numeric_limits<char32_t>::is_exact;
		constexpr int                   numeric_limits<char32_t>::radix;
		constexpr int                   numeric_limits<char32_t>::min_exponent;
		constexpr int                   numeric_limits<char32_t>::min_exponent10;
		constexpr int                   numeric_limits<char32_t>::max_exponent;
		constexpr int                   numeric_limits<char32_t>::max_exponent10;
		constexpr bool                  numeric_limits<char32_t>::is_bounded;
		constexpr bool                  numeric_limits<char32_t>::is_modulo;
		constexpr bool                  numeric_limits<char32_t>::traps;
		constexpr bool                  numeric_limits<char32_t>::tinyness_before;
		constexpr float_round_style     numeric_limits<char32_t>::round_style;
		constexpr bool                  numeric_limits<char32_t>::has_infinity;
		constexpr bool                  numeric_limits<char32_t>::has_quiet_NaN;
		constexpr bool                  numeric_limits<char32_t>::has_signaling_NaN;
		constexpr float_denorm_style    numeric_limits<char32_t>::has_denorm;
		constexpr bool                  numeric_limits<char32_t>::has_denorm_loss;
		constexpr bool                  numeric_limits<char32_t>::is_iec559;
	#endif
	
	// unsigned short
	constexpr bool                  numeric_limits<unsigned short>::is_specialized;
	constexpr int                   numeric_limits<unsigned short>::digits;
	constexpr int                   numeric_limits<unsigned short>::digits10;
	constexpr bool                  numeric_limits<unsigned short>::is_signed;
	constexpr bool                  numeric_limits<unsigned short>::is_integer;
	constexpr bool                  numeric_limits<unsigned short>::is_exact;
	constexpr int                   numeric_limits<unsigned short>::radix;
	constexpr int                   numeric_limits<unsigned short>::min_exponent;
	constexpr int                   numeric_limits<unsigned short>::min_exponent10;
	constexpr int                   numeric_limits<unsigned short>::max_exponent;
	constexpr int                   numeric_limits<unsigned short>::max_exponent10;
	constexpr bool                  numeric_limits<unsigned short>::is_bounded;
	constexpr bool                  numeric_limits<unsigned short>::is_modulo;
	constexpr bool                  numeric_limits<unsigned short>::traps;
	constexpr bool                  numeric_limits<unsigned short>::tinyness_before;
	constexpr float_round_style     numeric_limits<unsigned short>::round_style;
	constexpr bool                  numeric_limits<unsigned short>::has_infinity;
	constexpr bool                  numeric_limits<unsigned short>::has_quiet_NaN;
	constexpr bool                  numeric_limits<unsigned short>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<unsigned short>::has_denorm;
	constexpr bool                  numeric_limits<unsigned short>::has_denorm_loss;
	constexpr bool                  numeric_limits<unsigned short>::is_iec559;

	// short
	constexpr bool                  numeric_limits<short>::is_specialized;
	constexpr int                   numeric_limits<short>::digits;
	constexpr int                   numeric_limits<short>::digits10;
	constexpr bool                  numeric_limits<short>::is_signed;
	constexpr bool                  numeric_limits<short>::is_integer;
	constexpr bool                  numeric_limits<short>::is_exact;
	constexpr int                   numeric_limits<short>::radix;
	constexpr int                   numeric_limits<short>::min_exponent;
	constexpr int                   numeric_limits<short>::min_exponent10;
	constexpr int                   numeric_limits<short>::max_exponent;
	constexpr int                   numeric_limits<short>::max_exponent10;
	constexpr bool                  numeric_limits<short>::is_bounded;
	constexpr bool                  numeric_limits<short>::is_modulo;
	constexpr bool                  numeric_limits<short>::traps;
	constexpr bool                  numeric_limits<short>::tinyness_before;
	constexpr float_round_style     numeric_limits<short>::round_style;
	constexpr bool                  numeric_limits<short>::has_infinity;
	constexpr bool                  numeric_limits<short>::has_quiet_NaN;
	constexpr bool                  numeric_limits<short>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<short>::has_denorm;
	constexpr bool                  numeric_limits<short>::has_denorm_loss;
	constexpr bool                  numeric_limits<short>::is_iec559;

	// unsigned int
	constexpr bool                  numeric_limits<unsigned int>::is_specialized;
	constexpr int                   numeric_limits<unsigned int>::digits;
	constexpr int                   numeric_limits<unsigned int>::digits10;
	constexpr bool                  numeric_limits<unsigned int>::is_signed;
	constexpr bool                  numeric_limits<unsigned int>::is_integer;
	constexpr bool                  numeric_limits<unsigned int>::is_exact;
	constexpr int                   numeric_limits<unsigned int>::radix;
	constexpr int                   numeric_limits<unsigned int>::min_exponent;
	constexpr int                   numeric_limits<unsigned int>::min_exponent10;
	constexpr int                   numeric_limits<unsigned int>::max_exponent;
	constexpr int                   numeric_limits<unsigned int>::max_exponent10;
	constexpr bool                  numeric_limits<unsigned int>::is_bounded;
	constexpr bool                  numeric_limits<unsigned int>::is_modulo;
	constexpr bool                  numeric_limits<unsigned int>::traps;
	constexpr bool                  numeric_limits<unsigned int>::tinyness_before;
	constexpr float_round_style     numeric_limits<unsigned int>::round_style;
	constexpr bool                  numeric_limits<unsigned int>::has_infinity;
	constexpr bool                  numeric_limits<unsigned int>::has_quiet_NaN;
	constexpr bool                  numeric_limits<unsigned int>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<unsigned int>::has_denorm;
	constexpr bool                  numeric_limits<unsigned int>::has_denorm_loss;
	constexpr bool                  numeric_limits<unsigned int>::is_iec559;

	// int
	constexpr bool                  numeric_limits<int>::is_specialized;
	constexpr int                   numeric_limits<int>::digits;
	constexpr int                   numeric_limits<int>::digits10;
	constexpr bool                  numeric_limits<int>::is_signed;
	constexpr bool                  numeric_limits<int>::is_integer;
	constexpr bool                  numeric_limits<int>::is_exact;
	constexpr int                   numeric_limits<int>::radix;
	constexpr int                   numeric_limits<int>::min_exponent;
	constexpr int                   numeric_limits<int>::min_exponent10;
	constexpr int                   numeric_limits<int>::max_exponent;
	constexpr int                   numeric_limits<int>::max_exponent10;
	constexpr bool                  numeric_limits<int>::is_bounded;
	constexpr bool                  numeric_limits<int>::is_modulo;
	constexpr bool                  numeric_limits<int>::traps;
	constexpr bool                  numeric_limits<int>::tinyness_before;
	constexpr float_round_style     numeric_limits<int>::round_style;
	constexpr bool                  numeric_limits<int>::has_infinity;
	constexpr bool                  numeric_limits<int>::has_quiet_NaN;
	constexpr bool                  numeric_limits<int>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<int>::has_denorm;
	constexpr bool                  numeric_limits<int>::has_denorm_loss;
	constexpr bool                  numeric_limits<int>::is_iec559;

	// unsigned long
	constexpr bool                  numeric_limits<unsigned long>::is_specialized;
	constexpr int                   numeric_limits<unsigned long>::digits;
	constexpr int                   numeric_limits<unsigned long>::digits10;
	constexpr bool                  numeric_limits<unsigned long>::is_signed;
	constexpr bool                  numeric_limits<unsigned long>::is_integer;
	constexpr bool                  numeric_limits<unsigned long>::is_exact;
	constexpr int                   numeric_limits<unsigned long>::radix;
	constexpr int                   numeric_limits<unsigned long>::min_exponent;
	constexpr int                   numeric_limits<unsigned long>::min_exponent10;
	constexpr int                   numeric_limits<unsigned long>::max_exponent;
	constexpr int                   numeric_limits<unsigned long>::max_exponent10;
	constexpr bool                  numeric_limits<unsigned long>::is_bounded;
	constexpr bool                  numeric_limits<unsigned long>::is_modulo;
	constexpr bool                  numeric_limits<unsigned long>::traps;
	constexpr bool                  numeric_limits<unsigned long>::tinyness_before;
	constexpr float_round_style     numeric_limits<unsigned long>::round_style;
	constexpr bool                  numeric_limits<unsigned long>::has_infinity;
	constexpr bool                  numeric_limits<unsigned long>::has_quiet_NaN;
	constexpr bool                  numeric_limits<unsigned long>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<unsigned long>::has_denorm;
	constexpr bool                  numeric_limits<unsigned long>::has_denorm_loss;
	constexpr bool                  numeric_limits<unsigned long>::is_iec559;

	// long
	constexpr bool                  numeric_limits<long>::is_specialized;
	constexpr int                   numeric_limits<long>::digits;
	constexpr int                   numeric_limits<long>::digits10;
	constexpr bool                  numeric_limits<long>::is_signed;
	constexpr bool                  numeric_limits<long>::is_integer;
	constexpr bool                  numeric_limits<long>::is_exact;
	constexpr int                   numeric_limits<long>::radix;
	constexpr int                   numeric_limits<long>::min_exponent;
	constexpr int                   numeric_limits<long>::min_exponent10;
	constexpr int                   numeric_limits<long>::max_exponent;
	constexpr int                   numeric_limits<long>::max_exponent10;
	constexpr bool                  numeric_limits<long>::is_bounded;
	constexpr bool                  numeric_limits<long>::is_modulo;
	constexpr bool                  numeric_limits<long>::traps;
	constexpr bool                  numeric_limits<long>::tinyness_before;
	constexpr float_round_style     numeric_limits<long>::round_style;
	constexpr bool                  numeric_limits<long>::has_infinity;
	constexpr bool                  numeric_limits<long>::has_quiet_NaN;
	constexpr bool                  numeric_limits<long>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<long>::has_denorm;
	constexpr bool                  numeric_limits<long>::has_denorm_loss;
	constexpr bool                  numeric_limits<long>::is_iec559;

	// unsigned long long
	constexpr bool                  numeric_limits<unsigned long long>::is_specialized;
	constexpr int                   numeric_limits<unsigned long long>::digits;
	constexpr int                   numeric_limits<unsigned long long>::digits10;
	constexpr bool                  numeric_limits<unsigned long long>::is_signed;
	constexpr bool                  numeric_limits<unsigned long long>::is_integer;
	constexpr bool                  numeric_limits<unsigned long long>::is_exact;
	constexpr int                   numeric_limits<unsigned long long>::radix;
	constexpr int                   numeric_limits<unsigned long long>::min_exponent;
	constexpr int                   numeric_limits<unsigned long long>::min_exponent10;
	constexpr int                   numeric_limits<unsigned long long>::max_exponent;
	constexpr int                   numeric_limits<unsigned long long>::max_exponent10;
	constexpr bool                  numeric_limits<unsigned long long>::is_bounded;
	constexpr bool                  numeric_limits<unsigned long long>::is_modulo;
	constexpr bool                  numeric_limits<unsigned long long>::traps;
	constexpr bool                  numeric_limits<unsigned long long>::tinyness_before;
	constexpr float_round_style     numeric_limits<unsigned long long>::round_style;
	constexpr bool                  numeric_limits<unsigned long long>::has_infinity;
	constexpr bool                  numeric_limits<unsigned long long>::has_quiet_NaN;
	constexpr bool                  numeric_limits<unsigned long long>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<unsigned long long>::has_denorm;
	constexpr bool                  numeric_limits<unsigned long long>::has_denorm_loss;
	constexpr bool                  numeric_limits<unsigned long long>::is_iec559;

	// long long
	constexpr bool                  numeric_limits<long long>::is_specialized;
	constexpr int                   numeric_limits<long long>::digits;
	constexpr int                   numeric_limits<long long>::digits10;
	constexpr bool                  numeric_limits<long long>::is_signed;
	constexpr bool                  numeric_limits<long long>::is_integer;
	constexpr bool                  numeric_limits<long long>::is_exact;
	constexpr int                   numeric_limits<long long>::radix;
	constexpr int                   numeric_limits<long long>::min_exponent;
	constexpr int                   numeric_limits<long long>::min_exponent10;
	constexpr int                   numeric_limits<long long>::max_exponent;
	constexpr int                   numeric_limits<long long>::max_exponent10;
	constexpr bool                  numeric_limits<long long>::is_bounded;
	constexpr bool                  numeric_limits<long long>::is_modulo;
	constexpr bool                  numeric_limits<long long>::traps;
	constexpr bool                  numeric_limits<long long>::tinyness_before;
	constexpr float_round_style     numeric_limits<long long>::round_style;
	constexpr bool                  numeric_limits<long long>::has_infinity;
	constexpr bool                  numeric_limits<long long>::has_quiet_NaN;
	constexpr bool                  numeric_limits<long long>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<long long>::has_denorm;
	constexpr bool                  numeric_limits<long long>::has_denorm_loss;
	constexpr bool                  numeric_limits<long long>::is_iec559;

	// __uint128_t
	#if (EA_COMPILER_INTMAX_SIZE >= 16) && (defined(EA_COMPILER_GNUC) || defined(__clang__)) // If __int128_t/__uint128_t is supported...
		constexpr bool                  numeric_limits<__uint128_t>::is_specialized;
		constexpr int                   numeric_limits<__uint128_t>::digits;
		constexpr int                   numeric_limits<__uint128_t>::digits10;
		constexpr bool                  numeric_limits<__uint128_t>::is_signed;
		constexpr bool                  numeric_limits<__uint128_t>::is_integer;
		constexpr bool                  numeric_limits<__uint128_t>::is_exact;
		constexpr int                   numeric_limits<__uint128_t>::radix;
		constexpr int                   numeric_limits<__uint128_t>::min_exponent;
		constexpr int                   numeric_limits<__uint128_t>::min_exponent10;
		constexpr int                   numeric_limits<__uint128_t>::max_exponent;
		constexpr int                   numeric_limits<__uint128_t>::max_exponent10;
		constexpr bool                  numeric_limits<__uint128_t>::is_bounded;
		constexpr bool                  numeric_limits<__uint128_t>::is_modulo;
		constexpr bool                  numeric_limits<__uint128_t>::traps;
		constexpr bool                  numeric_limits<__uint128_t>::tinyness_before;
		constexpr float_round_style     numeric_limits<__uint128_t>::round_style;
		constexpr bool                  numeric_limits<__uint128_t>::has_infinity;
		constexpr bool                  numeric_limits<__uint128_t>::has_quiet_NaN;
		constexpr bool                  numeric_limits<__uint128_t>::has_signaling_NaN;
		constexpr float_denorm_style    numeric_limits<__uint128_t>::has_denorm;
		constexpr bool                  numeric_limits<__uint128_t>::has_denorm_loss;
		constexpr bool                  numeric_limits<__uint128_t>::is_iec559;
	#endif

	// __int128_t
	#if (EA_COMPILER_INTMAX_SIZE >= 16) && (defined(EA_COMPILER_GNUC) || defined(__clang__)) // If __int128_t/__uint128_t is supported...
		constexpr bool                  numeric_limits<__int128_t>::is_specialized;
		constexpr int                   numeric_limits<__int128_t>::digits;
		constexpr int                   numeric_limits<__int128_t>::digits10;
		constexpr bool                  numeric_limits<__int128_t>::is_signed;
		constexpr bool                  numeric_limits<__int128_t>::is_integer;
		constexpr bool                  numeric_limits<__int128_t>::is_exact;
		constexpr int                   numeric_limits<__int128_t>::radix;
		constexpr int                   numeric_limits<__int128_t>::min_exponent;
		constexpr int                   numeric_limits<__int128_t>::min_exponent10;
		constexpr int                   numeric_limits<__int128_t>::max_exponent;
		constexpr int                   numeric_limits<__int128_t>::max_exponent10;
		constexpr bool                  numeric_limits<__int128_t>::is_bounded;
		constexpr bool                  numeric_limits<__int128_t>::is_modulo;
		constexpr bool                  numeric_limits<__int128_t>::traps;
		constexpr bool                  numeric_limits<__int128_t>::tinyness_before;
		constexpr float_round_style     numeric_limits<__int128_t>::round_style;
		constexpr bool                  numeric_limits<__int128_t>::has_infinity;
		constexpr bool                  numeric_limits<__int128_t>::has_quiet_NaN;
		constexpr bool                  numeric_limits<__int128_t>::has_signaling_NaN;
		constexpr float_denorm_style    numeric_limits<__int128_t>::has_denorm;
		constexpr bool                  numeric_limits<__int128_t>::has_denorm_loss;
		constexpr bool                  numeric_limits<__int128_t>::is_iec559;
	#endif

	// float
	constexpr bool                  numeric_limits<float>::is_specialized;
	constexpr int                   numeric_limits<float>::digits;
	constexpr int                   numeric_limits<float>::digits10;
	constexpr bool                  numeric_limits<float>::is_signed;
	constexpr bool                  numeric_limits<float>::is_integer;
	constexpr bool                  numeric_limits<float>::is_exact;
	constexpr int                   numeric_limits<float>::radix;
	constexpr int                   numeric_limits<float>::min_exponent;
	constexpr int                   numeric_limits<float>::min_exponent10;
	constexpr int                   numeric_limits<float>::max_exponent;
	constexpr int                   numeric_limits<float>::max_exponent10;
	constexpr bool                  numeric_limits<float>::is_bounded;
	constexpr bool                  numeric_limits<float>::is_modulo;
	constexpr bool                  numeric_limits<float>::traps;
	constexpr bool                  numeric_limits<float>::tinyness_before;
	constexpr float_round_style     numeric_limits<float>::round_style;
	constexpr bool                  numeric_limits<float>::has_infinity;
	constexpr bool                  numeric_limits<float>::has_quiet_NaN;
	constexpr bool                  numeric_limits<float>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<float>::has_denorm;
	constexpr bool                  numeric_limits<float>::has_denorm_loss;
	constexpr bool                  numeric_limits<float>::is_iec559;

	// double
	constexpr bool                  numeric_limits<double>::is_specialized;
	constexpr int                   numeric_limits<double>::digits;
	constexpr int                   numeric_limits<double>::digits10;
	constexpr bool                  numeric_limits<double>::is_signed;
	constexpr bool                  numeric_limits<double>::is_integer;
	constexpr bool                  numeric_limits<double>::is_exact;
	constexpr int                   numeric_limits<double>::radix;
	constexpr int                   numeric_limits<double>::min_exponent;
	constexpr int                   numeric_limits<double>::min_exponent10;
	constexpr int                   numeric_limits<double>::max_exponent;
	constexpr int                   numeric_limits<double>::max_exponent10;
	constexpr bool                  numeric_limits<double>::is_bounded;
	constexpr bool                  numeric_limits<double>::is_modulo;
	constexpr bool                  numeric_limits<double>::traps;
	constexpr bool                  numeric_limits<double>::tinyness_before;
	constexpr float_round_style     numeric_limits<double>::round_style;
	constexpr bool                  numeric_limits<double>::has_infinity;
	constexpr bool                  numeric_limits<double>::has_quiet_NaN;
	constexpr bool                  numeric_limits<double>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<double>::has_denorm;
	constexpr bool                  numeric_limits<double>::has_denorm_loss;
	constexpr bool                  numeric_limits<double>::is_iec559;

	// long double
	constexpr bool                  numeric_limits<long double>::is_specialized;
	constexpr int                   numeric_limits<long double>::digits;
	constexpr int                   numeric_limits<long double>::digits10;
	constexpr bool                  numeric_limits<long double>::is_signed;
	constexpr bool                  numeric_limits<long double>::is_integer;
	constexpr bool                  numeric_limits<long double>::is_exact;
	constexpr int                   numeric_limits<long double>::radix;
	constexpr int                   numeric_limits<long double>::min_exponent;
	constexpr int                   numeric_limits<long double>::min_exponent10;
	constexpr int                   numeric_limits<long double>::max_exponent;
	constexpr int                   numeric_limits<long double>::max_exponent10;
	constexpr bool                  numeric_limits<long double>::is_bounded;
	constexpr bool                  numeric_limits<long double>::is_modulo;
	constexpr bool                  numeric_limits<long double>::traps;
	constexpr bool                  numeric_limits<long double>::tinyness_before;
	constexpr float_round_style     numeric_limits<long double>::round_style;
	constexpr bool                  numeric_limits<long double>::has_infinity;
	constexpr bool                  numeric_limits<long double>::has_quiet_NaN;
	constexpr bool                  numeric_limits<long double>::has_signaling_NaN;
	constexpr float_denorm_style    numeric_limits<long double>::has_denorm;
	constexpr bool                  numeric_limits<long double>::has_denorm_loss;
	constexpr bool                  numeric_limits<long double>::is_iec559;

} // namespace eastl
