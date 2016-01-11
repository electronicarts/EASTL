///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Implements the class template eastl::ratio that provides compile-time
// rational arithmetic support. Each instantiation of this template exactly
// represents any finite rational number as long as its numerator Num and
// denominator Denom are representable as compile-time constants of type
// intmax_t. In addition, Denom may not be zero and may not be equal to the most
// negative value. Both numerator and denominator are automatically reduced to
// the lowest terms.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_RATIO_H
#define EASTL_RATIO_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once 
#endif

#include <EABase/eabase.h>


//////////////////////////////////////////////////////////////////////////////
// namespace eastl 
// {
// 	template <intmax_t N, intmax_t D = 1>
// 	class ratio
// 	{
// 	public:
// 		static constexpr intmax_t num;
// 		static constexpr intmax_t den;
// 		typedef ratio<num, den> type;
// 	};
// 
// 	// ratio arithmetic
// 	template <class R1, class R2> using ratio_add      = ...;
// 	template <class R1, class R2> using ratio_subtract = ...;
// 	template <class R1, class R2> using ratio_multiply = ...;
// 	template <class R1, class R2> using ratio_divide   = ...;
// 
// 	// ratio comparison
// 	template <class R1, class R2> struct ratio_equal;
// 	template <class R1, class R2> struct ratio_not_equal;
// 	template <class R1, class R2> struct ratio_less;
// 	template <class R1, class R2> struct ratio_less_equal;
// 	template <class R1, class R2> struct ratio_greater;
// 	template <class R1, class R2> struct ratio_greater_equal;
// 
// 	// convenience SI typedefs
// 	typedef ratio<1, 1000000000000000000000000> yocto;  // not supported
// 	typedef ratio<1,    1000000000000000000000> zepto;  // not supported
// 	typedef ratio<1,       1000000000000000000> atto;
// 	typedef ratio<1,          1000000000000000> femto;
// 	typedef ratio<1,             1000000000000> pico;
// 	typedef ratio<1,                1000000000> nano;
// 	typedef ratio<1,                   1000000> micro;
// 	typedef ratio<1,                      1000> milli;
// 	typedef ratio<1,                       100> centi;
// 	typedef ratio<1,                        10> deci;
// 	typedef ratio<                       10, 1> deca;
// 	typedef ratio<                      100, 1> hecto;
// 	typedef ratio<                     1000, 1> kilo;
// 	typedef ratio<                  1000000, 1> mega;
// 	typedef ratio<               1000000000, 1> giga;
// 	typedef ratio<            1000000000000, 1> tera;
// 	typedef ratio<         1000000000000000, 1> peta;
// 	typedef ratio<      1000000000000000000, 1> exa;
// 	typedef ratio<   1000000000000000000000, 1> zetta;  // not supported
// 	typedef ratio<1000000000000000000000000, 1> yotta;  // not supported
// }
//////////////////////////////////////////////////////////////////////////////


#include <EASTL/internal/config.h>
#include <EASTL/type_traits.h>


namespace eastl 
{ 
	///////////////////////////////////////////////////////////////////////
	// compile-time overflow helpers
	///////////////////////////////////////////////////////////////////////
	#define EASTL_RATIO_ABS(x)	        		((x) < 0 ? -(x) : (x))

	template <intmax_t X, intmax_t Y>
	struct AdditionOverFlow
	{
		static const bool c1 = (X <= 0 && 0 <= Y) || (Y < 0 && 0 < X);   // True if digits do not have the same sign. 
		static const bool c2 = EASTL_RATIO_ABS(Y) <= INTMAX_MAX - EASTL_RATIO_ABS(X);
		static const bool value = c1 || c2;
	};

    template <intmax_t X, intmax_t Y>
    struct MultiplyOverFlow
    {
	    static const bool value = (EASTL_RATIO_ABS(X) <= (INTMAX_MAX / EASTL_RATIO_ABS(Y)));
    };


    ///////////////////////////////////////////////////////////////////////
	// ratio (C++ Standard: 20.11.3)
	///////////////////////////////////////////////////////////////////////
	template <intmax_t N = 0, intmax_t D = 1>
	class ratio 
	{
	public:
		static EA_CONSTEXPR_OR_CONST intmax_t num = N;
		static EA_CONSTEXPR_OR_CONST intmax_t den = D;
		typedef ratio<num, den> type;
	};

	namespace Internal
	{
		// gcd -- implementation based on euclid's algorithm
		template <intmax_t X, intmax_t Y> struct gcd { static const intmax_t value = gcd<Y, X % Y>::value; };
		template <intmax_t X> struct gcd<X, 0>       { static const intmax_t value = X; };
		template <> struct gcd<0, 0>                 { static const intmax_t value = 1; };

		// lcm
		template<intmax_t X, intmax_t Y>
		struct lcm { static const intmax_t value = (X * (Y / gcd<X,Y>::value)); };

		// ct_add
		template <intmax_t X, intmax_t Y>
		struct ct_add 
		{ 
			static_assert(AdditionOverFlow<X,Y>::value, "compile-time addition overflow"); 
			static const intmax_t value = X + Y; 
		};

		// ct_sub
        template <intmax_t X, intmax_t Y>
        struct ct_sub
        {
			static_assert(AdditionOverFlow<X,-Y>::value, "compile-time addition overflow"); 
	        static const intmax_t value = X - Y;
        };

        // ct_multi
        template <intmax_t X, intmax_t Y>
        struct ct_multi
        {
			static_assert(MultiplyOverFlow<X,Y>::value, "compile-time multiply overflow"); 
	        static const intmax_t value = X * Y;
        };

        // ct_simplify
		template <class R1>
		struct ct_simplify
		{ 
			static const intmax_t divisor = Internal::gcd<R1::num, R1::den>::value; 
			static const intmax_t num = R1::num / divisor;
			static const intmax_t den = R1::den / divisor; 

			typedef ratio<num, den> ratio_type;
			typedef ct_simplify<R1> this_type;
		};

		#if EASTL_VARIABLE_TEMPLATES_ENABLED
			template <intmax_t N1, intmax_t N2> intmax_t ct_add_v   = ct_add<N1, N2>::value;
			template <intmax_t N1, intmax_t N2> intmax_t ct_multi_v = ct_multi<N1, N2>::value;
			template <class R1, class R2> R2 ct_simplify_t          = ct_simplify<R1>::ratio_type;
		#else
			template <intmax_t N1, intmax_t N2> struct ct_add_v   : public ct_add<N1, N2>::value {};
			template <intmax_t N1, intmax_t N2> struct ct_multi_v : public ct_multi<N1, N2>::value {};
			template <class R1> struct ct_simplify_t 			  : public ct_simplify<R1>::ratio_type {};
		#endif

		///////////////////////////////////////////////////////////////////////
		// ratio_add
		///////////////////////////////////////////////////////////////////////
		template <class R1, class R2>
		struct ratio_add
		{
			typedef typename ct_simplify
			<
				typename ratio
				<
					ct_add
					<
						ct_multi<R1::num, R2::den>::value, 
						ct_multi<R2::num, R1::den>::value
					>::value,							
					ct_multi<R1::den, R2::den>::value  
				>::type
			>::ratio_type type;
		};

		///////////////////////////////////////////////////////////////////////
		// ratio_subtract
		///////////////////////////////////////////////////////////////////////
		template <class R1, class R2>
		struct ratio_subtract
		{
			typedef typename ct_simplify
			<
				typename ratio
				<
					ct_sub
					<
						ct_multi<R1::num, R2::den>::value, 
						ct_multi<R2::num, R1::den>::value
					>::value,							
					ct_multi<R1::den, R2::den>::value  
				>::type
			>::ratio_type type;
		};

		///////////////////////////////////////////////////////////////////////
		// ratio_multiply
		///////////////////////////////////////////////////////////////////////
		template <class R1, class R2>
		struct ratio_multiply
		{
			typedef typename ct_simplify
			<
				typename ratio
				<
					ct_multi<R1::num, R2::num>::value, 
					ct_multi<R1::den, R2::den>::value  
				>::type
			>::ratio_type type;
		};

		///////////////////////////////////////////////////////////////////////
		// ratio_divide
		///////////////////////////////////////////////////////////////////////
		template <class R1, class R2>
		struct ratio_divide
		{
			typedef typename ct_simplify
			<
				typename ratio
				<
					ct_multi<R1::num, R2::den>::value, 
					ct_multi<R1::den, R2::num>::value  
				>::type
			>::ratio_type type;
		};

		///////////////////////////////////////////////////////////////////////
		// ratio_equal
		///////////////////////////////////////////////////////////////////////
		template <class R1, class R2>
		struct ratio_equal
		{
			typedef ct_simplify<R1> sr1_t;
			typedef ct_simplify<R2> sr2_t;

	        static const bool value = (sr1_t::num == sr2_t::num) && (sr1_t::den == sr2_t::den);
        };

		///////////////////////////////////////////////////////////////////////
		// ratio_less
		///////////////////////////////////////////////////////////////////////
		template <class R1, class R2>
		struct ratio_less
		{
	        static const bool value = (R1::num * R2::den) < (R2::num * R1::den); 
        };
	} // namespace Internal


	///////////////////////////////////////////////////////////////////////
	// ratio arithmetic (C++ Standard: 20.11.4)
	///////////////////////////////////////////////////////////////////////
	#if defined(EA_COMPILER_NO_TEMPLATE_ALIASES) || (defined(_MSC_VER) && (_MSC_VER < 1900))  // prior to VS2015
		template <class R1, class R2> struct ratio_add      : public Internal::ratio_add<R1, R2>::type {};
		template <class R1, class R2> struct ratio_subtract : public Internal::ratio_subtract<R1, R2>::type {};
		template <class R1, class R2> struct ratio_multiply : public Internal::ratio_multiply<R1, R2>::type {};
		template <class R1, class R2> struct ratio_divide   : public Internal::ratio_divide<R1, R2>::type {};
	#else
		template <class R1, class R2> using ratio_add      = typename Internal::ratio_add<R1, R2>::type;
		template <class R1, class R2> using ratio_subtract = typename Internal::ratio_subtract<R1, R2>::type;
		template <class R1, class R2> using ratio_multiply = typename Internal::ratio_multiply<R1, R2>::type;
		template <class R1, class R2> using ratio_divide   = typename Internal::ratio_divide<R1, R2>::type;
	#endif


	///////////////////////////////////////////////////////////////////////
	// ratio comparison (C++ Standard: 20.11.5)
	///////////////////////////////////////////////////////////////////////
	template <class R1, class R2> struct ratio_equal         : public integral_constant<bool, Internal::ratio_equal<R1, R2>::value> {};
	template <class R1, class R2> struct ratio_not_equal     : public integral_constant<bool, !ratio_equal<R1, R2>::value> {};
	template <class R1, class R2> struct ratio_less          : public integral_constant<bool, Internal::ratio_less<R1, R2>::value> {};
	template <class R1, class R2> struct ratio_less_equal    : public integral_constant<bool, !ratio_less<R2, R1>::value> {};
	template <class R1, class R2> struct ratio_greater       : public integral_constant<bool, ratio_less<R2, R1>::value> {};
	template <class R1, class R2> struct ratio_greater_equal : public integral_constant<bool, !ratio_less<R1, R2>::value> {};


    ///////////////////////////////////////////////////////////////////////
    // convenience SI typedefs (C++ Standard: 20.11.6)
	///////////////////////////////////////////////////////////////////////
	// typedef ratio<1, 1000000000000000000000000> yocto;  // not supported, too big for intmax_t
	// typedef ratio<1, 1000000000000000000000   > zepto;  // not supported, too big for intmax_t
	typedef ratio<1, 1000000000000000000      > atto;
	typedef ratio<1, 1000000000000000         > femto;
	typedef ratio<1, 1000000000000            > pico;
	typedef ratio<1, 1000000000               > nano;
	typedef ratio<1, 1000000                  > micro;
	typedef ratio<1, 1000                     > milli;
	typedef ratio<1, 100                      > centi;
	typedef ratio<1, 10                       > deci;
	typedef ratio<10, 1                       > deca;
	typedef ratio<100, 1                      > hecto;
	typedef ratio<1000, 1                     > kilo;
	typedef ratio<1000000, 1                  > mega;
	typedef ratio<1000000000, 1               > giga;
	typedef ratio<1000000000000, 1            > tera;
	typedef ratio<1000000000000000, 1         > peta;
	typedef ratio<1000000000000000000, 1      > exa;
	// typedef ratio<1000000000000000000000, 1   > zetta;  // not supported, too big for intmax_t
	// typedef ratio<1000000000000000000000000, 1> yotta;  // not supported, too big for intmax_t
}

#endif // EASTL_RATIO_H
