/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EABase/eabase.h>
#include <EASTL/ratio.h>


int TestRatio()
{
	using namespace eastl;

	int nErrorCount = 0;
	{
		using namespace eastl::Internal;

		// lcm (least common multiple)
		static_assert(lcm<0,0>::value == 0, "lcm failure");
		static_assert(lcm<10,6>::value == 30, "lcm failure");
		static_assert(lcm<21,6>::value == 42, "lcm failure");
		static_assert(lcm<21,6>::value == lcm<6,21>::value, "lcm failure");
	
		// gcd (greatest common divisor)
		static_assert(gcd<6, 4>::value == 2, "gcd failure");
		static_assert(gcd<54, 24>::value == 6, "gcd failure");
		static_assert(gcd<42, 56>::value == 14, "gcd failure");
		static_assert(gcd<48, 18>::value == 6, "gcd failure");
		static_assert(gcd<50, 40>::value == 10, "gcd failure");
		static_assert(gcd<6, 4>::value != 9, "gcd failure");
		static_assert(gcd<0, 0>::value == 1, "gcd failure");
		static_assert(gcd<1, 0>::value == 1, "gcd failure");
		static_assert(gcd<0, 1>::value == 1, "gcd failure");
		static_assert(gcd<34,7>::value == gcd<7, 34>::value, "gcd failure");
		static_assert(gcd<9223372036854775807, 9223372036854775807>::value == 9223372036854775807, "gcd failure");
	
		// simplify
		typedef ct_simplify<ratio<50, 40>>::ratio_type smp_rt;
		typedef ct_simplify<ratio<50, 40>>::this_type smp_tt;
		static_assert(smp_rt::num == 5 && smp_rt::den == 4, "simplify failure");
		static_assert(smp_tt::divisor == 10, "simplify failure0");
		static_assert(smp_rt::num == 5, "simplify failure1");
		static_assert(smp_rt::den == 4, "simplify failure2");
	}

	{
		// ratio_add
		typedef ratio_add<ratio<2, 3>, ratio<1, 6>> sum;
		static_assert(sum::num == 5 && sum::den == 6, "ratio_add failure");
		typedef ratio_add<ratio<3,4>, ratio<5,10>> sum2;
		static_assert(sum2::num == 5 && sum2::den == 4, "ratio_add failure");

		// ratio_subtract
		typedef ratio_subtract<ratio<10,10>, ratio<1,2>> sum3;
		static_assert(sum3::num == 1 && sum3::den == 2, "ratio_subtract failure");

		// ratio_multiply
		typedef ratio_multiply<ratio<10,10>, ratio<1,2>> sum4;
		static_assert(sum4::num == 1 && sum4::den == 2, "ratio_multiply failure");
		typedef ratio_multiply<ratio<2,5>, ratio<1,2>> sum5;
		static_assert(sum5::num == 1 && sum5::den == 5, "ratio_multiply failure");
		typedef ratio_multiply<ratio<1,3>, ratio<9,16>> sum6;
		static_assert(sum6::num == 3 && sum6::den == 16, "ratio_multiply failure");

		// ratio_divide
		typedef ratio_divide<ratio<1,8>, ratio<1,4>> sum8;
		static_assert(sum8::num == 1 && sum8::den == 2, "ratio_divide failure");
		typedef ratio_divide<ratio<2,3>, ratio<5>> sum9;
		static_assert(sum9::num == 2 && sum9::den == 15, "ratio_divide failure");

		// ratio_equal
		static_assert(ratio_equal<ratio<1>, ratio<1>>::value, "ratio_equal failure");
		static_assert(ratio_equal<ratio<1,1>, ratio<4,4>>::value, "ratio_equal failure");
		static_assert(ratio_equal<ratio<5,10>, ratio<1,2>>::value, "ratio_equal failure");
		static_assert(ratio_equal<ratio<2,3>, ratio<4,6>>::value, "ratio_equal failure");

		// ratio_not_equal
		static_assert(!ratio_not_equal<ratio<5,10>, ratio<1,2>>::value, "ratio_not_equal failure");

		// ratio_less
		static_assert(ratio_less<ratio<2,10>, ratio<1,2>>::value, "ratio_less failure");
		static_assert(ratio_less<ratio<23,37>, ratio<57,90>>::value, "ratio_less failure");

		// ratio_less_equal
		static_assert(ratio_less_equal<ratio<2,10>, ratio<1,2>>::value, "ratio_less_equal failure");
		static_assert(ratio_less_equal<ratio<2,10>, ratio<1,5>>::value, "ratio_less_equal failure");
		static_assert(ratio_less_equal<ratio<1,100>, ratio<1,5>>::value, "ratio_less_equal failure");

		// ratio_greater 
		static_assert(ratio_greater<ratio<1,2>, ratio<1,4>>::value, "ratio_greater failure");

		// ratio_greater_equal
		static_assert(ratio_greater_equal<ratio<3,4>, ratio<1,2>>::value, "ratio_greater_equal failure");
	}

	return nErrorCount;
}









