/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#if defined(_MSC_VER)
	// We have little choice but to disable this warning. See the FAQ for why.
	#pragma warning(disable: 4244) // conversion from '___' to '___', possible loss of data
#endif


#include <EASTL/algorithm.h>
#include <EASTL/functional.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/vector.h>
#include <EASTL/array.h>
#include <EASTL/deque.h>
#include <EASTL/list.h>
#include <EASTL/slist.h>
#include <EASTL/string.h>
#include <EASTL/set.h>
#include <EASTL/sort.h>
#include "ConceptImpls.h"
#include <EAStdC/EAMemory.h>
#include "EASTLTest.h"  // Put this after the above so that it doesn't block any warnings from the includes above.

namespace eastl
{
	#if 0
	// These are some tests of altermative implementations of branch-free min/max functions.
	/*
	union FloatInt32Union
	{
		float   f;
		int32_t i;
	};

	inline float min_alt2(float a, float b)
	{
		FloatInt32Union uc;
		uc.f = a - b;

		const float choices[2] = { a, b };
		return (choices + 1)[uc.i >> 31];
	}

	inline float min_alt3(float a, float b)
	{
		FloatInt32Union uc, ua, ub, ur;

		uc.f   = a - b;
		uc.i >>= 31;
		ua.f   = a;
		ub.f   = b;
		ur.i   = (ua.i & uc.i) | (ub.i & ~uc.i);

		return ur.f;
	}
	*/
	#endif
}


namespace
{
	struct A{
		A(int n) : a(n){}
		int a;
	};
	struct LessStruct{ bool operator()(const A& a1, const A& a2){ return a1.a < a2.a; } };


	struct B{
		B(int n) : b(n){}
		int b;
	};
	inline bool LessFunction(const B& b1, const B& b2){ return b1.b < b2.b; }
}

enum TestMinMaxEnum
{
	teX = 0,
	teY = 3
};


///////////////////////////////////////////////////////////////////////////////
// Greater
//
// A version of greater that uses operator < instead of operator >.
//
template <typename T>
struct Greater
{
	bool operator()(const T& a, const T& b) const
		{ return (b < a); }
};


///////////////////////////////////////////////////////////////////////////////
// DivisibleBy
//
struct DivisibleBy
{
	int d;
	DivisibleBy(int n = 1) : d(n) {}
	bool operator()(int n) const { return ((n % d) == 0); }
};


///////////////////////////////////////////////////////////////////////////////
// TestObjectNegate
//
struct TestObjectNegate
{
	TestObject operator()(const TestObject& a) const
		{ return TestObject(-a.mX); }
};

static int TestMinMax()
{
	using namespace eastl;

	int nErrorCount = 0;

	EA::UnitTest::Rand rng(EA::UnitTest::GetRandSeed());

	{
		// NOTE(rparolin): This compiles but it should not.  We provide explicit eastl::max overloads for float, double,
		// and long double which enable this behaviour.  It is not standards compliant and it will be removed in a
		// future release.
		{
			struct Foo
			{
				operator float() const { return 0; }
			};

			Foo f1;
			float f2{};
			eastl::max(f1, f2);
		}

		// NOTE(rparolin): This will not compile because we lack explicit eastl::max overloads for 'int'.
		// {
		//	 struct Foo
		//	 {
		//		 operator int() const { return 0; }
		//	 };

		//	 Foo f1;
		//	 int f2{};
		//	 eastl::max(f1, f2);
		// }
	}

	{
		// const T& min(const T& a, const T& b);
		// const T& min(const T& a, const T& b, Compare compare)
		// const T& max(const T& a, const T& b);
		// const T& max(const T& a, const T& b, Compare compare)

		A a1(1), a2(2), a3(3);
		a3 = min(a1, a2, LessStruct());
		EATEST_VERIFY(a3.a == 1);
		a3 = max(a1, a2, LessStruct());
		EATEST_VERIFY(a3.a == 2);

		B b1(1), b2(2), b3(3);
		b3 = min(b2, b1, LessFunction);
		EATEST_VERIFY(b3.b == 1);
		b3 = max(b2, b1, LessFunction);
		EATEST_VERIFY(b3.b == 2);


		TestObject t1(1), t2(2), t3(3);
		t3 = min(t2, t1);
		EATEST_VERIFY(t3.mX == 1);
		t3 = max(t2, t1);
		EATEST_VERIFY(t3.mX == 2);


		int i1, i2(-1), i3(1);
		i1 = min(i2, i3);
		EATEST_VERIFY(i1 == -1);
		i1 = min(i3, i2);
		EATEST_VERIFY(i1 == -1);
		i1 = max(i2, i3);
		EATEST_VERIFY(i1 == 1);
		i1 = max(i3, i2);
		EATEST_VERIFY(i1 == 1);

		const volatile int i2cv(-1), i3cv(1);
		i1 = min(i2cv, i3cv);
		EATEST_VERIFY(i1 == -1);
		i1 = min(i3cv, i2cv);
		EATEST_VERIFY(i1 == -1);
		i1 = max(i2cv, i3cv);
		EATEST_VERIFY(i1 == 1);
		i1 = max(i3cv, i2cv);
		EATEST_VERIFY(i1 == 1);

		float f1, f2(-1), f3(1);
		f1 = min(f2, f3);
		EATEST_VERIFY(f1 == -1);
		f1 = min(f3, f2);
		EATEST_VERIFY(f1 == -1);
		f1 = max(f2, f3);
		EATEST_VERIFY(f1 == 1);
		f1 = max(f3, f2);
		EATEST_VERIFY(f1 == 1);

		double d1, d2(-1), d3(1);
		d1 = min(d2, d3);
		EATEST_VERIFY(d1 == -1);
		d1 = min(d3, d2);
		EATEST_VERIFY(d1 == -1);
		d1 = max(d2, d3);
		EATEST_VERIFY(d1 == 1);
		d1 = max(d3, d2);
		EATEST_VERIFY(d1 == 1);

		void* p1, *p2 = &d2, *p3 = &d3;
		p1 = min(p2, p3);
		EATEST_VERIFY((uintptr_t)p1 == min((uintptr_t)p2, (uintptr_t)p3));

		double* pd1, *pd2 = &d2, *pd3 = &d3;
		pd1 = min(pd2, pd3);
		EATEST_VERIFY((uintptr_t)pd1 == min((uintptr_t)pd2, (uintptr_t)pd3));


		// initializer_list tests
		#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
			EATEST_VERIFY(min({ 3, 1, 2}) == 1);
			EATEST_VERIFY(max({ 3, 1, 2}) == 3);
		#endif


		// Test scalar specializations
		EATEST_VERIFY(min((char)1, (char)1) == 1);
		EATEST_VERIFY(min((char)1, (char)2) == 1);
		EATEST_VERIFY(min((char)2, (char)1) == 1);

		EATEST_VERIFY(min((signed char)1, (signed char)1) == 1);
		EATEST_VERIFY(min((signed char)1, (signed char)2) == 1);
		EATEST_VERIFY(min((signed char)2, (signed char)1) == 1);

		EATEST_VERIFY(min((unsigned char)1, (unsigned char)1) == 1);
		EATEST_VERIFY(min((unsigned char)1, (unsigned char)2) == 1);
		EATEST_VERIFY(min((unsigned char)2, (unsigned char)1) == 1);

		EATEST_VERIFY(min((signed short)1, (signed short)1) == 1);
		EATEST_VERIFY(min((signed short)1, (signed short)2) == 1);
		EATEST_VERIFY(min((signed short)2, (signed short)1) == 1);

		EATEST_VERIFY(min((unsigned short)1, (unsigned short)1) == 1);
		EATEST_VERIFY(min((unsigned short)1, (unsigned short)2) == 1);
		EATEST_VERIFY(min((unsigned short)2, (unsigned short)1) == 1);

		EATEST_VERIFY(min((signed int)1, (signed int)1) == 1);
		EATEST_VERIFY(min((signed int)1, (signed int)2) == 1);
		EATEST_VERIFY(min((signed int)2, (signed int)1) == 1);

		EATEST_VERIFY(min((unsigned int)1, (unsigned int)1) == 1);
		EATEST_VERIFY(min((unsigned int)1, (unsigned int)2) == 1);
		EATEST_VERIFY(min((unsigned int)2, (unsigned int)1) == 1);

		EATEST_VERIFY(min((signed long)1, (signed long)1) == 1);
		EATEST_VERIFY(min((signed long)1, (signed long)2) == 1);
		EATEST_VERIFY(min((signed long)2, (signed long)1) == 1);

		EATEST_VERIFY(min((unsigned long)1, (unsigned long)1) == 1);
		EATEST_VERIFY(min((unsigned long)1, (unsigned long)2) == 1);
		EATEST_VERIFY(min((unsigned long)2, (unsigned long)1) == 1);

		EATEST_VERIFY(min((signed long long)1, (signed long long)1) == 1);
		EATEST_VERIFY(min((signed long long)1, (signed long long)2) == 1);
		EATEST_VERIFY(min((signed long long)2, (signed long long)1) == 1);

		EATEST_VERIFY(min((unsigned long long)1, (unsigned long long)1) == 1);
		EATEST_VERIFY(min((unsigned long long)1, (unsigned long long)2) == 1);
		EATEST_VERIFY(min((unsigned long long)2, (unsigned long long)1) == 1);

		EATEST_VERIFY(min((float)1, (float)1) == 1);
		EATEST_VERIFY(min((float)1, (float)2) == 1);
		EATEST_VERIFY(min((float)2, (float)1) == 1);

		EATEST_VERIFY(min((double)1, (double)1) == 1);
		EATEST_VERIFY(min((double)1, (double)2) == 1);
		EATEST_VERIFY(min((double)2, (double)1) == 1);

		EATEST_VERIFY(min((long double)1, (long double)1) == 1);
		EATEST_VERIFY(min((long double)1, (long double)2) == 1);
		EATEST_VERIFY(min((long double)2, (long double)1) == 1);


		// Test max specializations
		EATEST_VERIFY(max((char)1, (char)1) == 1);
		EATEST_VERIFY(max((char)1, (char)2) == 2);
		EATEST_VERIFY(max((char)2, (char)1) == 2);

		EATEST_VERIFY(max((signed char)1, (signed char)1) == 1);
		EATEST_VERIFY(max((signed char)1, (signed char)2) == 2);
		EATEST_VERIFY(max((signed char)2, (signed char)1) == 2);

		EATEST_VERIFY(max((unsigned char)1, (unsigned char)1) == 1);
		EATEST_VERIFY(max((unsigned char)1, (unsigned char)2) == 2);
		EATEST_VERIFY(max((unsigned char)2, (unsigned char)1) == 2);

		EATEST_VERIFY(max((signed short)1, (signed short)1) == 1);
		EATEST_VERIFY(max((signed short)1, (signed short)2) == 2);
		EATEST_VERIFY(max((signed short)2, (signed short)1) == 2);

		EATEST_VERIFY(max((unsigned short)1, (unsigned short)1) == 1);
		EATEST_VERIFY(max((unsigned short)1, (unsigned short)2) == 2);
		EATEST_VERIFY(max((unsigned short)2, (unsigned short)1) == 2);

		EATEST_VERIFY(max((signed int)1, (signed int)1) == 1);
		EATEST_VERIFY(max((signed int)1, (signed int)2) == 2);
		EATEST_VERIFY(max((signed int)2, (signed int)1) == 2);

		EATEST_VERIFY(max((unsigned int)1, (unsigned int)1) == 1);
		EATEST_VERIFY(max((unsigned int)1, (unsigned int)2) == 2);
		EATEST_VERIFY(max((unsigned int)2, (unsigned int)1) == 2);

		EATEST_VERIFY(max((signed long)1, (signed long)1) == 1);
		EATEST_VERIFY(max((signed long)1, (signed long)2) == 2);
		EATEST_VERIFY(max((signed long)2, (signed long)1) == 2);

		EATEST_VERIFY(max((unsigned long)1, (unsigned long)1) == 1);
		EATEST_VERIFY(max((unsigned long)1, (unsigned long)2) == 2);
		EATEST_VERIFY(max((unsigned long)2, (unsigned long)1) == 2);

		EATEST_VERIFY(max((signed long long)1, (signed long long)1) == 1);
		EATEST_VERIFY(max((signed long long)1, (signed long long)2) == 2);
		EATEST_VERIFY(max((signed long long)2, (signed long long)1) == 2);

		EATEST_VERIFY(max((unsigned long long)1, (unsigned long long)1) == 1);
		EATEST_VERIFY(max((unsigned long long)1, (unsigned long long)2) == 2);
		EATEST_VERIFY(max((unsigned long long)2, (unsigned long long)1) == 2);

		EATEST_VERIFY(max((float)1, (float)1) == 1);
		EATEST_VERIFY(max((float)1, (float)2) == 2);
		EATEST_VERIFY(max((float)2, (float)1) == 2);

		EATEST_VERIFY(max((double)1, (double)1) == 1);
		EATEST_VERIFY(max((double)1, (double)2) == 2);
		EATEST_VERIFY(max((double)2, (double)1) == 2);

		EATEST_VERIFY(max((long double)1, (long double)1) == 1);
		EATEST_VERIFY(max((long double)1, (long double)2) == 2);
		EATEST_VERIFY(max((long double)2, (long double)1) == 2);


		// Test min_alt specializations
		EATEST_VERIFY(min_alt((char)1, (char)1) == 1);
		EATEST_VERIFY(min_alt((char)1, (char)2) == 1);
		EATEST_VERIFY(min_alt((char)2, (char)1) == 1);

		EATEST_VERIFY(min_alt((signed char)1, (signed char)1) == 1);
		EATEST_VERIFY(min_alt((signed char)1, (signed char)2) == 1);
		EATEST_VERIFY(min_alt((signed char)2, (signed char)1) == 1);

		EATEST_VERIFY(min_alt((unsigned char)1, (unsigned char)1) == 1);
		EATEST_VERIFY(min_alt((unsigned char)1, (unsigned char)2) == 1);
		EATEST_VERIFY(min_alt((unsigned char)2, (unsigned char)1) == 1);

		EATEST_VERIFY(min_alt((signed short)1, (signed short)1) == 1);
		EATEST_VERIFY(min_alt((signed short)1, (signed short)2) == 1);
		EATEST_VERIFY(min_alt((signed short)2, (signed short)1) == 1);

		EATEST_VERIFY(min_alt((unsigned short)1, (unsigned short)1) == 1);
		EATEST_VERIFY(min_alt((unsigned short)1, (unsigned short)2) == 1);
		EATEST_VERIFY(min_alt((unsigned short)2, (unsigned short)1) == 1);

		EATEST_VERIFY(min_alt((signed int)1, (signed int)1) == 1);
		EATEST_VERIFY(min_alt((signed int)1, (signed int)2) == 1);
		EATEST_VERIFY(min_alt((signed int)2, (signed int)1) == 1);

		EATEST_VERIFY(min_alt((unsigned int)1, (unsigned int)1) == 1);
		EATEST_VERIFY(min_alt((unsigned int)1, (unsigned int)2) == 1);
		EATEST_VERIFY(min_alt((unsigned int)2, (unsigned int)1) == 1);

		EATEST_VERIFY(min_alt((signed long)1, (signed long)1) == 1);
		EATEST_VERIFY(min_alt((signed long)1, (signed long)2) == 1);
		EATEST_VERIFY(min_alt((signed long)2, (signed long)1) == 1);

		EATEST_VERIFY(min_alt((unsigned long)1, (unsigned long)1) == 1);
		EATEST_VERIFY(min_alt((unsigned long)1, (unsigned long)2) == 1);
		EATEST_VERIFY(min_alt((unsigned long)2, (unsigned long)1) == 1);

		EATEST_VERIFY(min_alt((signed long long)1, (signed long long)1) == 1);
		EATEST_VERIFY(min_alt((signed long long)1, (signed long long)2) == 1);
		EATEST_VERIFY(min_alt((signed long long)2, (signed long long)1) == 1);

		EATEST_VERIFY(min_alt((unsigned long long)1, (unsigned long long)1) == 1);
		EATEST_VERIFY(min_alt((unsigned long long)1, (unsigned long long)2) == 1);
		EATEST_VERIFY(min_alt((unsigned long long)2, (unsigned long long)1) == 1);

		EATEST_VERIFY(min_alt((float)1, (float)1) == 1);
		EATEST_VERIFY(min_alt((float)1, (float)2) == 1);
		EATEST_VERIFY(min_alt((float)2, (float)1) == 1);

		EATEST_VERIFY(min_alt((double)1, (double)1) == 1);
		EATEST_VERIFY(min_alt((double)1, (double)2) == 1);
		EATEST_VERIFY(min_alt((double)2, (double)1) == 1);

		EATEST_VERIFY(min_alt((long double)1, (long double)1) == 1);
		EATEST_VERIFY(min_alt((long double)1, (long double)2) == 1);
		EATEST_VERIFY(min_alt((long double)2, (long double)1) == 1);


		// Test max_alt specializations
		EATEST_VERIFY(max_alt((char)1, (char)1) == 1);
		EATEST_VERIFY(max_alt((char)1, (char)2) == 2);
		EATEST_VERIFY(max_alt((char)2, (char)1) == 2);

		EATEST_VERIFY(max_alt((signed char)1, (signed char)1) == 1);
		EATEST_VERIFY(max_alt((signed char)1, (signed char)2) == 2);
		EATEST_VERIFY(max_alt((signed char)2, (signed char)1) == 2);

		EATEST_VERIFY(max_alt((unsigned char)1, (unsigned char)1) == 1);
		EATEST_VERIFY(max_alt((unsigned char)1, (unsigned char)2) == 2);
		EATEST_VERIFY(max_alt((unsigned char)2, (unsigned char)1) == 2);

		EATEST_VERIFY(max_alt((signed short)1, (signed short)1) == 1);
		EATEST_VERIFY(max_alt((signed short)1, (signed short)2) == 2);
		EATEST_VERIFY(max_alt((signed short)2, (signed short)1) == 2);

		EATEST_VERIFY(max_alt((unsigned short)1, (unsigned short)1) == 1);
		EATEST_VERIFY(max_alt((unsigned short)1, (unsigned short)2) == 2);
		EATEST_VERIFY(max_alt((unsigned short)2, (unsigned short)1) == 2);

		EATEST_VERIFY(max_alt((signed int)1, (signed int)1) == 1);
		EATEST_VERIFY(max_alt((signed int)1, (signed int)2) == 2);
		EATEST_VERIFY(max_alt((signed int)2, (signed int)1) == 2);

		EATEST_VERIFY(max_alt((unsigned int)1, (unsigned int)1) == 1);
		EATEST_VERIFY(max_alt((unsigned int)1, (unsigned int)2) == 2);
		EATEST_VERIFY(max_alt((unsigned int)2, (unsigned int)1) == 2);

		EATEST_VERIFY(max_alt((signed long)1, (signed long)1) == 1);
		EATEST_VERIFY(max_alt((signed long)1, (signed long)2) == 2);
		EATEST_VERIFY(max_alt((signed long)2, (signed long)1) == 2);

		EATEST_VERIFY(max_alt((unsigned long)1, (unsigned long)1) == 1);
		EATEST_VERIFY(max_alt((unsigned long)1, (unsigned long)2) == 2);
		EATEST_VERIFY(max_alt((unsigned long)2, (unsigned long)1) == 2);

		EATEST_VERIFY(max_alt((signed long long)1, (signed long long)1) == 1);
		EATEST_VERIFY(max_alt((signed long long)1, (signed long long)2) == 2);
		EATEST_VERIFY(max_alt((signed long long)2, (signed long long)1) == 2);

		EATEST_VERIFY(max_alt((unsigned long long)1, (unsigned long long)1) == 1);
		EATEST_VERIFY(max_alt((unsigned long long)1, (unsigned long long)2) == 2);
		EATEST_VERIFY(max_alt((unsigned long long)2, (unsigned long long)1) == 2);

		EATEST_VERIFY(max_alt((float)1, (float)1) == 1);
		EATEST_VERIFY(max_alt((float)1, (float)2) == 2);
		EATEST_VERIFY(max_alt((float)2, (float)1) == 2);

		EATEST_VERIFY(max_alt((double)1, (double)1) == 1);
		EATEST_VERIFY(max_alt((double)1, (double)2) == 2);
		EATEST_VERIFY(max_alt((double)2, (double)1) == 2);

		EATEST_VERIFY(max_alt((long double)1, (long double)1) == 1);
		EATEST_VERIFY(max_alt((long double)1, (long double)2) == 2);
		EATEST_VERIFY(max_alt((long double)2, (long double)1) == 2);
	}

	{
		// const T& min_alt(const T& a, const T& b);
		// const T& min_alt(const T& a, const T& b, Compare compare)
		// const T& max_alt(const T& a, const T& b);
		// const T& max_alt(const T& a, const T& b, Compare compare)

		A a1(1), a2(2), a3(3);
		a3 = min_alt(a1, a2, LessStruct());
		EATEST_VERIFY(a3.a == 1);
		a3 = max_alt(a1, a2, LessStruct());
		EATEST_VERIFY(a3.a == 2);

		B b1(1), b2(2), b3(3);
		b3 = min_alt(b2, b1, LessFunction);
		EATEST_VERIFY(b3.b == 1);
		b3 = max_alt(b2, b1, LessFunction);
		EATEST_VERIFY(b3.b == 2);


		TestObject t1(1), t2(2), t3(3);
		t3 = min_alt(t2, t1);
		EATEST_VERIFY(t3.mX == 1);
		t3 = max_alt(t2, t1);
		EATEST_VERIFY(t3.mX == 2);


		int i1, i2(-1), i3(1);
		i1 = min_alt(i2, i3);
		EATEST_VERIFY(i1 == -1);
		i1 = min_alt(i3, i2);
		EATEST_VERIFY(i1 == -1);
		i1 = max_alt(i2, i3);
		EATEST_VERIFY(i1 == 1);
		i1 = max_alt(i3, i2);
		EATEST_VERIFY(i1 == 1);

		float f1, f2(-1), f3(1);
		f1 = min_alt(f2, f3);
		EATEST_VERIFY(f1 == -1);
		f1 = min_alt(f3, f2);
		EATEST_VERIFY(f1 == -1);
		f1 = max_alt(f2, f3);
		EATEST_VERIFY(f1 == 1);
		f1 = max_alt(f3, f2);
		EATEST_VERIFY(f1 == 1);

		double d1, d2(-1), d3(1);
		d1 = min_alt(d2, d3);
		EATEST_VERIFY(d1 == -1);
		d1 = min_alt(d3, d2);
		EATEST_VERIFY(d1 == -1);
		d1 = max_alt(d2, d3);
		EATEST_VERIFY(d1 == 1);
		d1 = max_alt(d3, d2);
		EATEST_VERIFY(d1 == 1);

		// Make sure enums work
		static_assert(eastl::is_enum<TestMinMaxEnum>::value, "is_enum failure");
		EATEST_VERIFY(eastl::min(teX, teY) == teX);

		// Make sure pointers work
		TestObject testObjectArray[2];
		EATEST_VERIFY(eastl::min(&testObjectArray[0], &testObjectArray[1]) == &testObjectArray[0]);

		// Regression for Microsoft warning C4347 (http://msdn.microsoft.com/en-us/library/x7wb5te0.aspx)
		int32_t value  = rng.RandRange(17, 18);
		int32_t result = eastl::max_alt<int32_t>(0, value); // warning C4347: behavior change: 'const T &eastl::max_alt<int32_t>(const T &,const T &)' is called instead of 'int eastl::max_alt(int,int)'
		EATEST_VERIFY(result == 17);

		// Regression for Microsoft error C2666 (http://msdn.microsoft.com/en-us/library/dyafzty4%28v=vs.110%29.aspx)
		uint32_t value2a  = 17;
		uint32_t value2b  = 2;
		uint32_t result2 = eastl::min_alt<uint32_t>(value2a - value2b, 4); // error C2666: 'eastl::min_alt' : 12 overloads have similar conversions
		EATEST_VERIFY(result2 == 4);

		// Regression for volatile arguments + literals
		// This test is disabled until we come up with a solution for this. std::min gives the same result as below, so we aren't necessarily obligated to resolve this.
		// volatile uint32_t value3  = 17;
		// uint32_t result3 = eastl::min_alt<uint32_t>(value3, 4); // error C2664: 'const T &eastl::min_alt<unsigned int>(const T &,const T &)' : cannot convert parameter 1 from 'volatile uint32_t' to 'const unsigned int &'
		// EATEST_VERIFY(result3 == 4);
	}


	{
		// ForwardIterator min_element(ForwardIterator first, ForwardIterator last)
		// ForwardIterator min_element(ForwardIterator first, ForwardIterator last, Compare compare)

		int intArray[] = { -5, 2, 1, 5, 4, 5 };
		int* pInt = min_element(intArray, intArray + 6);
		EATEST_VERIFY(pInt && (*pInt == -5));

		pInt = min_element(intArray, intArray + 6, Greater<int>());
		EATEST_VERIFY(pInt && (*pInt == 5));


		TestObject toArray[] = { TestObject(7), TestObject(2), TestObject(8), TestObject(5), TestObject(4), TestObject(-12) };
		TestObject* pTO = min_element(toArray, toArray + 6);
		EATEST_VERIFY(pTO && (*pTO == TestObject(-12)));

		pTO = min_element(toArray, toArray + 6, Greater<TestObject>());
		EATEST_VERIFY(pTO && (*pTO == TestObject(8)));
	}


	{
		// ForwardIterator max_element(ForwardIterator first, ForwardIterator last)
		// ForwardIterator max_element(ForwardIterator first, ForwardIterator last, Compare compare)

		int intArray[] = { -5, 2, 1, 5, 4, 5 };
		int* pInt = max_element(intArray, intArray + 6);
		EATEST_VERIFY(pInt && (*pInt == 5));

		pInt = max_element(intArray, intArray + 6, less<int>());
		EATEST_VERIFY(pInt && (*pInt == 5));


		TestObject toArray[] = { TestObject(7), TestObject(2), TestObject(8), TestObject(5), TestObject(4), TestObject(-12) };
		TestObject* pTO = max_element(toArray, toArray + 6);
		EATEST_VERIFY(pTO && (*pTO == TestObject(8)));

		pTO = max_element(toArray, toArray + 6, less<TestObject>());
		EATEST_VERIFY(pTO && (*pTO == TestObject(8)));
	}

	{
		// template <class ForwardIterator, class Compare>
		// eastl::pair<ForwardIterator, ForwardIterator>
		// minmax_element(ForwardIterator first, ForwardIterator last)
		//
		// template <class ForwardIterator, class Compare>
		// eastl::pair<ForwardIterator, ForwardIterator>
		// minmax_element(ForwardIterator first, ForwardIterator last, Compare compare)

		int intArray[] = { 5, -2, 1, 5, 6, 5 };

		eastl::pair<int*, int*> result = eastl::minmax_element(intArray, intArray + 6);
		EATEST_VERIFY((*result.first == -2) && (*result.second == 6));


		// template <typename T>
		// eastl::pair<const T&, const T&>
		// minmax(const T& a, const T& b)
		//
		// template <typename T, typename Compare>
		// eastl::pair<const T&, const T&>
		// minmax(const T& a, const T& b, Compare comp)

		// The VC++ compiler is broken in such a way that it can't compile the following without generating a warning:
		//     warning C4413: 'eastl::pair<T1,T2>::first' : reference member is initialized to a temporary that doesn't persist after the constructor exits.
		// The Microsoft standard library definition of minmax doesn't generate this warning... because that minmax is broken and non-conforming. I think they
		// made it the way they did because of the aforementioned compiler bug.
		// Recent versions of clang seem to generate a warning of its own. To do: we need to address this.
		// GCC 4.8 for x86 has a compiler bug in optimized builds for this code, so we currently enable this for non-optimized builds only.
		#if defined(EA_COMPILER_CPP11_ENABLED) && ((defined(EA_COMPILER_CLANG) && EA_COMPILER_VERSION < 302) || (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4007)) && !defined(__OPTIMIZE__))

			int i3(3), i2(2);
			eastl::pair<const int&, const int&> resulti = eastl::minmax(i3, i2);
			EATEST_VERIFY_F((resulti.first == 2) && (resulti.second == 3), "minmax failure. %d %d", resulti.first, resulti.second);

			char c3(3), c2(2);
			eastl::pair<const char&, const char&> resultc = eastl::minmax(c3, c2);
			EATEST_VERIFY_F((resultc.first == 2) && (resultc.second == 3), "minmax failure. %d %d", (int)resultc.first, (int)resultc.second);

			float f3(3), f2(2);
			eastl::pair<const float&, const float&> resultf = eastl::minmax(f3, f2);
			EATEST_VERIFY_F((resultf.first == 2) && (resultf.second == 3), "minmax failure. %f %f", resultf.first, resultf.second);
		#endif


		// template <typename T>
		// eastl::pair<T, T>
		// minmax(std::initializer_list<T> ilist)
		//
		// template <typename T, class Compare>
		// eastl::pair<T, T>
		// minmax(std::initializer_list<T> ilist, Compare compare)
		#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
			eastl::pair<int, int> result3 = eastl::minmax({3, 2});
			EATEST_VERIFY((result3.first == 2) && (result3.second == 3));
		#endif
	}


	return nErrorCount;
}


static int TestClamp()
{
	using namespace eastl;

	int nErrorCount = 0;

	EATEST_VERIFY(eastl::clamp(42, 1, 100) == 42);
	EATEST_VERIFY(eastl::clamp(-42, 1, 100) == 1);
	EATEST_VERIFY(eastl::clamp(420, 1, 100) == 100);
	EATEST_VERIFY(eastl::clamp(1, 1, 100) == 1);
	EATEST_VERIFY(eastl::clamp(100, 1, 100) == 100);

	EATEST_VERIFY(eastl::clamp(42.f, 1.f, 100.f, less<float>()) == 42.f);
	EATEST_VERIFY(eastl::clamp(-42.f, 1.f, 100.f, less<float>()) == 1.f);
	EATEST_VERIFY(eastl::clamp(420.f, 1.f, 100.f, less<float>()) == 100.f);
	EATEST_VERIFY(eastl::clamp(1.f, 1.f, 100.f, less<float>()) == 1.f);
	EATEST_VERIFY(eastl::clamp(100.f, 1.f, 100.f, less<float>()) == 100.f);

	EATEST_VERIFY(eastl::clamp(42., 1., 100., less<double>()) == 42.);
	EATEST_VERIFY(eastl::clamp(-42., 1., 100., less<double>()) == 1.);
	EATEST_VERIFY(eastl::clamp(420., 1., 100., less<double>()) == 100.);
	EATEST_VERIFY(eastl::clamp(1., 1., 100., less<double>()) == 1.);
	EATEST_VERIFY(eastl::clamp(100., 1., 100., less<double>()) == 100.);

	EATEST_VERIFY(eastl::clamp(A(42), A(1), A(100), LessStruct()).a == A(42).a);
	EATEST_VERIFY(eastl::clamp(A(-42), A(1), A(100), LessStruct()).a == A(1).a);
	EATEST_VERIFY(eastl::clamp(A(420), A(1), A(100), LessStruct()).a == A(100).a);
	EATEST_VERIFY(eastl::clamp(A(1), A(1), A(100), LessStruct()).a == A(1).a);
	EATEST_VERIFY(eastl::clamp(A(100), A(1), A(100), LessStruct()).a == A(100).a);

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestAlgorithm
//
int TestAlgorithm()
{
	using namespace eastl;

	int nErrorCount = 0;

	EA::UnitTest::Rand rng(EA::UnitTest::GetRandSeed());

	TestObject::Reset();

	nErrorCount += TestMinMax();
	nErrorCount += TestClamp();


	// bool all_of (InputIterator first, InputIterator last, Predicate p);
	// bool any_of (InputIterator first, InputIterator last, Predicate p);
	// bool none_of(InputIterator first, InputIterator last, Predicate p);
	{

		eastl::vector<int> v;
		v.push_back(2);
		v.push_back(4);
		v.push_back(6);
		v.push_back(8);

		EATEST_VERIFY(eastl::all_of( v.begin(), v.end(), DivisibleBy(2)));
		EATEST_VERIFY(eastl::any_of( v.begin(), v.end(), DivisibleBy(3)));
		EATEST_VERIFY(eastl::none_of(v.begin(), v.end(), DivisibleBy(5)));
	}


	{
		// pair mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
		// pair mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, Predicate predicate)

		int intArray1[] = { -5, 2, 1, 5, 4, 8888 };
		int intArray2[] = { -5, 2, 1, 5, 4, 9999 };
		int intArray3[] = { -5, 2, 1, 5, 4, 9999 };

		eastl::pair<int*, int*> pairInt = mismatch(intArray1, intArray1, intArray2);
		EATEST_VERIFY(pairInt.first  == intArray1 + 0);
		EATEST_VERIFY(pairInt.second == intArray2 + 0);

		pairInt = mismatch(intArray1, intArray1 + 6, intArray2);
		EATEST_VERIFY(pairInt.first  == intArray1 + 5);
		EATEST_VERIFY(pairInt.second == intArray2 + 5);
		pairInt = mismatch(intArray2, intArray2 + 6, intArray3);

		EATEST_VERIFY(pairInt.first  == intArray2 + 6);
		EATEST_VERIFY(pairInt.second == intArray3 + 6);


		pairInt = mismatch(intArray1, intArray1, intArray2, equal_to<int>());
		EATEST_VERIFY(pairInt.first  == intArray1 + 0);
		EATEST_VERIFY(pairInt.second == intArray2 + 0);

		pairInt = mismatch(intArray1, intArray1 + 6, intArray2, equal_to<int>());
		EATEST_VERIFY(pairInt.first  == intArray1 + 5);
		EATEST_VERIFY(pairInt.second == intArray2 + 5);

		pairInt = mismatch(intArray2, intArray2 + 6, intArray3, equal_to<int>());
		EATEST_VERIFY(pairInt.first  == intArray2 + 6);
		EATEST_VERIFY(pairInt.second == intArray3 + 6);
	}


	{
		// void swap(T& a, T& b)
		// void iter_swap(ForwardIterator1 a, ForwardIterator2 b)

		int intArray[] = { -5, 2, 1, 5, 4, 5 };

		swap(intArray[0], intArray[4]);
		EATEST_VERIFY(VerifySequence(intArray, intArray + 6, int(), "swap", 4, 2, 1, 5, -5, 5, -1));

		iter_swap(intArray + 2, intArray + 3);
		EATEST_VERIFY(VerifySequence(intArray, intArray + 6, int(), "iter_swap", 4, 2, 5, 1, -5, 5, -1));


		TestObject toArray[] = { TestObject(-5), TestObject(2), TestObject(1), TestObject(5), TestObject(4), TestObject(5) };

		swap(toArray[0], toArray[4]);
		EATEST_VERIFY(toArray[0] == TestObject(4));
		EATEST_VERIFY(toArray[4] == TestObject(-5));

		iter_swap(toArray + 2, toArray + 3);
		EATEST_VERIFY(toArray[2] == TestObject(5));
		EATEST_VERIFY(toArray[3] == TestObject(1));
	}


	{
		// ForwardIterator2 swap_ranges(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2)

		int intArray1[] = { 3, 2, 6, 5, 4, 1 };
		int intArray2[] = { 0, 0, 0, 0, 0, 0 };

		swap_ranges(intArray1, intArray1 + 6, intArray2);
		EATEST_VERIFY(VerifySequence(intArray1, intArray1 + 6, int(), "swap_ranges", 0, 0, 0, 0, 0, 0, -1));
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 6, int(), "swap_ranges", 3, 2, 6, 5, 4, 1, -1));


		TestObject toArray1[] = { TestObject(3), TestObject(2), TestObject(6), TestObject(5), TestObject(4), TestObject(1) };
		TestObject toArray2[] = { TestObject(0), TestObject(0), TestObject(0), TestObject(0), TestObject(0), TestObject(0) };

		swap_ranges(toArray1, toArray1 + 6, toArray2);
		EATEST_VERIFY(toArray1[0] == TestObject(0));
		EATEST_VERIFY(toArray1[5] == TestObject(0));
		EATEST_VERIFY(toArray2[0] == TestObject(3));
		EATEST_VERIFY(toArray2[5] == TestObject(1));
	}


	{
		// ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last)
		// ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last, BinaryPredicate predicate)

		int intArray[] = { 3, 2, 5, 5, 4, 1 };

		int* pInt = adjacent_find(intArray + 0, intArray + 6);
		EATEST_VERIFY(pInt == (intArray + 2));

		pInt = adjacent_find(intArray + 3, intArray + 6);
		EATEST_VERIFY(pInt == (intArray + 6)); // Verify not found


		TestObject toArray[] = { TestObject(3), TestObject(2), TestObject(5), TestObject(5), TestObject(4), TestObject(1) };

		TestObject* pTO = adjacent_find(toArray + 0, toArray + 6);
		EATEST_VERIFY(pTO == (toArray + 2));

		pTO = adjacent_find(toArray + 3, toArray + 6);
		EATEST_VERIFY(pTO == (toArray + 6)); // Verify not found
	}


	{
		// OutputIterator move(InputIterator first, InputIterator last, OutputIterator result)

		int intArray1[] = { 3, 2, 6, 5, 4, 1 };
		int intArray2[] = { 0, 0, 0, 0, 0, 0 };

		move(intArray1, intArray1 + 0, intArray2);
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 6, int(), "move", 0, 0, 0, 0, 0, 0, -1));

		move(intArray1, intArray1 + 6, intArray2);
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 6, int(), "move", 3, 2, 6, 5, 4, 1, -1));

		move(intArray1 + 1, intArray1 + 6, intArray1 + 0); // Copy over self.
		EATEST_VERIFY(VerifySequence(intArray1, intArray1 + 6, int(), "move", 2, 6, 5, 4, 1, 1, -1));
	}


	{
		// OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)

		int intArray1[] = { 3, 2, 6, 5, 4, 1 };
		int intArray2[] = { 0, 0, 0, 0, 0, 0 };

		copy(intArray1, intArray1 + 0, intArray2);
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 6, int(), "copy", 0, 0, 0, 0, 0, 0, -1));

		copy(intArray1, intArray1 + 6, intArray2);
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 6, int(), "copy", 3, 2, 6, 5, 4, 1, -1));

		copy(intArray1 + 1, intArray1 + 6, intArray1 + 0); // Copy over self.
		EATEST_VERIFY(VerifySequence(intArray1, intArray1 + 6, int(), "copy", 2, 6, 5, 4, 1, 1, -1));
	}


	{
		// OutputIterator copy_if(InputIterator first, InputIterator last, OutputIterator result, Predicate predicate)

		int intArray1[] = { 9, 1, 9, 9, 9, 9, 1, 1, 9, 9 };
		int intArray2[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

		auto equal_to_1 = [](int i) { return i == 1; };
		copy_if(intArray1, intArray1 + 0, intArray2, equal_to_1);
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 10, int(), "copy_if", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1));

		copy_if(intArray1, intArray1 + 9, intArray2, equal_to_1);
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 10, int(), "copy_if", 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, -1));

		copy_if(intArray1 + 1, intArray1 + 9, intArray1 + 0, equal_to_1); // Copy over self.
		EATEST_VERIFY(VerifySequence(intArray1, intArray1 + 10, int(), "copy_if", 1, 1, 1, 9, 9, 9, 1, 1, 9, 9, -1));
	}


	{
		// OutputIterator copy_n(InputIterator first, Size count, OutputIterator result)

		eastl::string in = "123456";
		eastl::string out;

		eastl::copy_n(in.begin(), 4, eastl::back_inserter(out));
		EATEST_VERIFY(out == "1234");
	}


	{
		// BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result)

		int intArray1[] = { 3, 2, 6, 5, 4, 1 };
		int intArray2[] = { 0, 0, 0, 0, 0, 0 };

		copy_backward(intArray1, intArray1 + 0, (int*) nullptr);
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 6, int(), "copy_backward", 0, 0, 0, 0, 0, 0, -1));

		copy_backward(intArray1, intArray1 + 0, intArray2 + 0);
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 6, int(), "copy_backward", 0, 0, 0, 0, 0, 0, -1));

		copy_backward(intArray1, intArray1 + 6, intArray2 + 6);
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 6, int(), "copy_backward", 3, 2, 6, 5, 4, 1, -1));

		copy_backward(intArray1, intArray1 + 5, intArray1 + 6); // Copy over self.
		EATEST_VERIFY(VerifySequence(intArray1, intArray1 + 6, int(), "copy_backward", 3, 3, 2, 6, 5, 4, -1));
	}


	{
		// OutputIterator move(InputIterator first, InputIterator last, OutputIterator result)
		{
			eastl::vector<eastl::string> src;
			for(eastl_size_t i = 0; i < 4; i++)
				src.push_back(eastl::string(1, (char8_t)('0' + i)));
			eastl::vector<eastl::string> dest(src.size());

			eastl::move(src.begin(), src.end(), dest.begin());
			EATEST_VERIFY((dest[0] == "0") && (dest[3] == "3"));
			EATEST_VERIFY(src[0].empty() && src[3].empty());
		}

		{
			// BidirectionalIterator2 move_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result)
			eastl::vector<eastl::string> src;
			for(eastl_size_t i = 0; i < 4; i++)
				src.push_back(eastl::string(1, (char8_t)('0' + i)));
			eastl::vector<eastl::string> dest(src.size());

			eastl::move_backward(src.begin(), src.end(), dest.end());
			EATEST_VERIFY((dest[0] == "0") && (dest[3] == "3"));
			EATEST_VERIFY(src[0].empty() && src[3].empty());
		}
	}


	{
		// difference_type count(InputIterator first, InputIterator last, const T& value)

		int intArray[] = { 1, 2, 1, 5, 4, 1 };
		ptrdiff_t n = count(intArray, intArray + 6, 1);
		EATEST_VERIFY(n == 3);

		TestObject toArray[] = { TestObject(1), TestObject(2), TestObject(1), TestObject(5), TestObject(4), TestObject(1) };
		n = count(toArray, toArray + 6, TestObject(1));
		EATEST_VERIFY(n == 3);
	}


	{
		// difference_type count_if(InputIterator first, InputIterator last, Predicate predicate)

		int intArray[] = { 3, 2, 6, 5, 4, 1, 2, 4, 5, 4, 1, 2 };

		// Count all items whose value is less than three.
		auto less_than_3 = [](int i) { return i < 3; };
		ptrdiff_t n = count_if(intArray, intArray, less_than_3); // No-op
		EATEST_VERIFY(n == 0);
		n = count_if(intArray, intArray + 12, less_than_3);
		EATEST_VERIFY(n == 5);


		// Count all items whose value is less than three.
		TestObject toArray[] = { TestObject(1), TestObject(3), TestObject(1), TestObject(4), TestObject(2), TestObject(5) };

		auto less_than_testobject_3 = [](const TestObject& lhs) { return lhs < TestObject(3); };
		n = count_if(toArray, toArray, less_than_testobject_3); // No-op
		EATEST_VERIFY(n == 0);
		n = count_if(toArray, toArray + 6, less_than_testobject_3);
		EATEST_VERIFY(n == 3);


		// Count all items whose value is less than three.
		slist<int> intList;
		intList.push_front(1);
		intList.push_front(3);
		intList.push_front(1);
		intList.push_front(4);
		intList.push_front(2);
		intList.push_front(5);

		n = count_if(intList.begin(), intList.begin(), less_than_3); // No-op
		EATEST_VERIFY(n == 0);
		n = count_if(intList.begin(), intList.end(), less_than_3);
		EATEST_VERIFY(n == 3);
	}


	{
		// void fill(ForwardIterator first, ForwardIterator last, const T& value)

		vector<int> intArray(10);

		EATEST_VERIFY(VerifySequence(intArray.begin(), intArray.end(), int(), "fill", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1));
		fill(intArray.begin() + 3, intArray.begin() + 7, 4);
		EATEST_VERIFY(VerifySequence(intArray.begin(), intArray.end(), int(), "fill", 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, -1));


		slist<int> intList(10);
		slist<int>::iterator first = intList.begin();
		slist<int>::iterator last = intList.begin();

		advance(first, 3);
		advance(last, 7);
		EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "fill", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1));
		fill(first, last, 4);
		EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "fill", 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, -1));


		// Exercise specializations we have for some platform/compiler combinations
		// void fill(uint64_t* first, uint64_t* last, uint64_t c);
		// void fill( int64_t* first,  int64_t* last,  int64_t c);
		// void fill(uint32_t* first, uint32_t* last, uint32_t c);
		// void fill( int32_t* first,  int32_t* last,  int32_t c);
		// void fill(uint16_t* first, uint16_t* last, uint16_t c);
		// void fill( int16_t* first,  int16_t* last,  int16_t c);
		const eastl_size_t kMaxSize = 300;
		eastl::vector<uint64_t> vU64(kMaxSize, 0);
		eastl::vector< int64_t> vI64(kMaxSize, 0);
		eastl::vector<uint32_t> vU32(kMaxSize, 0);
		eastl::vector< int32_t> vI32(kMaxSize, 0);
		eastl::vector<uint16_t> vU16(kMaxSize, 0);
		eastl::vector< int16_t> vI16(kMaxSize, 0);

		for(eastl_size_t i = 0; i < kMaxSize; ++i)
		{
			eastl::fill(vU64.begin(), vU64.begin() + i, UINT64_C(0x0123456789abcdef));
			EATEST_VERIFY(EA::StdC::Memcheck64(&vU64[0], UINT64_C(0x0123456789abcdef), i) == NULL);
			EA::StdC::Memset64(&vU64[0], 0, i);

			eastl::fill(vI64.begin(), vI64.begin() + i, UINT64_C(0x0123456789abcdef));
			EATEST_VERIFY(EA::StdC::Memcheck64(&vI64[0], UINT64_C(0x0123456789abcdef), i) == NULL);
			EA::StdC::Memset64(&vI64[0], 0, i);

			eastl::fill(vU32.begin(), vU32.begin() + i, UINT32_C(0x01234567));
			EATEST_VERIFY(EA::StdC::Memcheck32(&vU32[0], UINT32_C(0x01234567), i) == NULL);
			EA::StdC::Memset32(&vU32[0], 0, i);

			eastl::fill(vI32.begin(), vI32.begin() + i, UINT32_C(0x01234567));
			EATEST_VERIFY(EA::StdC::Memcheck32(&vI32[0], UINT32_C(0x01234567), i) == NULL);
			EA::StdC::Memset32(&vI32[0], 0, i);

			eastl::fill(vU16.begin(), vU16.begin() + i, UINT16_C(0x0123));
			EATEST_VERIFY(EA::StdC::Memcheck16(&vU16[0], UINT16_C(0x0123), i) == NULL);
			EA::StdC::Memset16(&vU16[0], 0, i);

			eastl::fill(vI16.begin(), vI16.begin() + i, UINT16_C(0x0123));
			EATEST_VERIFY(EA::StdC::Memcheck16(&vI16[0], UINT16_C(0x0123), i) == NULL);
			EA::StdC::Memset16(&vI16[0], 0, i);
		}

		{   // Regression for user-reported compile failure.
			enum TestEnum { eTestValue = -1 };
			eastl::vector<int32_t> intArrayEnum;

			eastl::fill<eastl::vector<int32_t>::iterator, int32_t>(intArrayEnum.begin(), intArrayEnum.end(), eTestValue);
			EATEST_VERIFY(intArrayEnum.size() == 0);
		}
	}


	{
		// OutputIterator fill_n(OutputIterator first, Size n, const T& value)

		vector<int> intArray(10);

		EATEST_VERIFY(VerifySequence(intArray.begin(), intArray.end(), int(), "fill_n", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1));
		fill_n(intArray.begin() + 3, 4, 4);
		EATEST_VERIFY(VerifySequence(intArray.begin(), intArray.end(), int(), "fill_n", 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, -1));


		list<int> intList(10);
		list<int>::iterator first = intList.begin();

		advance(first, 3);
		EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "fill_n", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1));
		fill_n(first, 4, 4);
		EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "fill_n", 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, -1));

		// Exercise specializations we have for some platform/compiler combinations
		// template<typename Size>
		// uint64_t* fill_n(uint64_t* first, Size n, uint64_t c);
		//  int64_t* fill_n( int64_t* first, Size n,  int64_t c);
		// uint32_t* fill_n(uint32_t* first, Size n, uint32_t c);
		//  int32_t* fill_n( int32_t* first, Size n,  int32_t c);
		// uint16_t* fill_n(uint16_t* first, Size n, uint16_t c);
		//  int16_t* fill_n( int16_t* first, Size n,  int16_t c);
		const eastl_size_t kMaxSize = 17;
		eastl::vector<uint64_t> vU64(kMaxSize, 0);
		eastl::vector< int64_t> vI64(kMaxSize, 0);
		eastl::vector<uint32_t> vU32(kMaxSize, 0);
		eastl::vector< int32_t> vI32(kMaxSize, 0);
		eastl::vector<uint16_t> vU16(kMaxSize, 0);
		eastl::vector< int16_t> vI16(kMaxSize, 0);

		eastl::vector<uint64_t>::iterator itU64 = eastl::fill_n(vU64.begin(), kMaxSize, UINT64_C(0x0123456789abcdef));
		EATEST_VERIFY(EA::StdC::Memcheck64(&vU64[0], UINT64_C(0x0123456789abcdef), kMaxSize) == NULL);
		EATEST_VERIFY(itU64 == (vU64.begin() + kMaxSize));
		EA::StdC::Memset64(&vU64[0], 0, kMaxSize);

		eastl::vector<int64_t>::iterator itI64 = eastl::fill_n(vI64.begin(), kMaxSize, UINT64_C(0x0123456789abcdef));
		EATEST_VERIFY(EA::StdC::Memcheck64(&vI64[0], UINT64_C(0x0123456789abcdef), kMaxSize) == NULL);
		EATEST_VERIFY(itI64 == (vI64.begin() + kMaxSize));
		EA::StdC::Memset64(&vI64[0], 0, kMaxSize);

		eastl::vector<uint32_t>::iterator itU32 = eastl::fill_n(vU32.begin(), kMaxSize, UINT32_C(0x01234567));
		EATEST_VERIFY(EA::StdC::Memcheck32(&vU32[0], UINT32_C(0x01234567), kMaxSize) == NULL);
		EATEST_VERIFY(itU32 == (vU32.begin() + kMaxSize));
		EA::StdC::Memset32(&vU32[0], 0, kMaxSize);

		eastl::vector<int32_t>::iterator itI32 = eastl::fill_n(vI32.begin(), kMaxSize, UINT32_C(0x01234567));
		EATEST_VERIFY(EA::StdC::Memcheck32(&vI32[0], UINT32_C(0x01234567), kMaxSize) == NULL);
		EATEST_VERIFY(itI32 == (vI32.begin() + kMaxSize));
		EA::StdC::Memset32(&vI32[0], 0, kMaxSize);

		eastl::vector<uint16_t>::iterator itU16 = eastl::fill_n(vU16.begin(), kMaxSize, UINT16_C(0x0123));
		EATEST_VERIFY(EA::StdC::Memcheck16(&vU16[0], UINT16_C(0x0123), kMaxSize) == NULL);
		EATEST_VERIFY(itU16 == (vU16.begin() + kMaxSize));
		EA::StdC::Memset16(&vU16[0], 0, kMaxSize);

		eastl::vector<int16_t>::iterator itI16 = eastl::fill_n(vI16.begin(), kMaxSize, UINT16_C(0x0123));
		EATEST_VERIFY(EA::StdC::Memcheck16(&vI16[0], UINT16_C(0x0123), kMaxSize) == NULL);
		EATEST_VERIFY(itI16 == (vI16.begin() + kMaxSize));
		EA::StdC::Memset16(&vI16[0], 0, kMaxSize);
	}


	{
		// InputIterator find(InputIterator first, InputIterator last, const T& value)
		vector<int> intArray;
		intArray.push_back(0);
		intArray.push_back(1);
		intArray.push_back(2);
		intArray.push_back(3);

		vector<int>::iterator it = find(intArray.begin(), intArray.end(), 2);
		EATEST_VERIFY(it == (intArray.begin() + 2));
		EATEST_VERIFY(*it == 2);

		it = find(intArray.begin(), intArray.end(), 7);
		EATEST_VERIFY(it == intArray.end());
	}


	{
		// InputIterator find_if(InputIterator first, InputIterator last, Predicate predicate)
		// InputIterator find_if_not(InputIterator first, InputIterator last, Predicate predicate)

		int intArray[] = { 3, 2, 6, 5, 4, 1, 2, 4, 5, 4, 1, 2 };

		// Find an item which is equal to 1.
		int* pInt = find_if(intArray, intArray, [](int i) { return i == 1; }); // No-op
		EATEST_VERIFY(pInt == (intArray));
		pInt = find_if(intArray, intArray + 12, [](int i) { return i == 1; });
		EATEST_VERIFY(pInt == (intArray + 5));
		pInt = find_if(intArray, intArray + 12, [](int i) { return i == 99; });
		EATEST_VERIFY(pInt == (intArray + 12));

		pInt = find_if_not(intArray, intArray + 12, [](int i) { return i == 3; });
		EATEST_VERIFY(pInt == (intArray + 1));

		// Find an item which is equal to 1.
		TestObject toArray[] = { TestObject(4), TestObject(3), TestObject(2), TestObject(1), TestObject(2), TestObject(5) };

		TestObject* pTO = find_if(toArray, toArray, [](const TestObject& lhs) { return lhs == TestObject(1); }); // No-op
		EATEST_VERIFY(pTO == (toArray));
		pTO = find_if(toArray, toArray + 6, [](const TestObject& lhs) { return lhs == TestObject(1); });
		EATEST_VERIFY(pTO == (toArray + 3));
		pTO = find_if(toArray, toArray + 6, [](const TestObject& lhs) { return lhs == TestObject(99); });
		EATEST_VERIFY(pTO == (toArray + 6));

		pTO = find_if_not(toArray, toArray + 6, [](const TestObject& lhs) { return lhs == TestObject(4); });
		EATEST_VERIFY(pTO == (toArray + 1));

		// Find an item which is equal to 1.
		slist<int> intList;
		intList.push_front(4);
		intList.push_front(3);
		intList.push_front(2);
		intList.push_front(1);
		intList.push_front(2);
		intList.push_front(5);

		// The list is now: { 5, 2, 1, 2, 3, 4 }
		slist<int>::iterator it = find_if(intList.begin(), intList.begin(), [](int i) { return i == 1; }); // No-op
		EATEST_VERIFY(it == intList.begin());
		it = find_if(intList.begin(), intList.end(), [](int i) { return i == 1; });
		EATEST_VERIFY(*it == 1);
		it = find_if(intList.begin(), intList.end(), [](int i) { return i == 99; });
		EATEST_VERIFY(it == intList.end());

		it = find_if_not(intList.begin(), intList.end(), [](int i) { return i == 5; });
		EATEST_VERIFY(*it == 2);
	}


	{
		// ForwardIterator1 find_first_of(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2)
		// ForwardIterator1 find_first_of(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate predicate)

		int intArray1[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		int intArray2[3]  = { 7, 6, 5 };

		int* pInt = find_first_of(intArray1, intArray1, intArray2, intArray2 + 3);
		EATEST_VERIFY(pInt == intArray1);
		pInt = find_first_of(intArray1, intArray1 + 10, intArray2, intArray2);
		EATEST_VERIFY(pInt == intArray1 + 10);
		pInt = find_first_of(intArray1, intArray1 + 10, intArray2, intArray2 + 3);
		EATEST_VERIFY(pInt == intArray1 + 5);

		pInt = find_first_of(intArray1, intArray1, intArray2, intArray2 + 3, equal_to<int>());
		EATEST_VERIFY(pInt == intArray1);
		pInt = find_first_of(intArray1, intArray1 + 10, intArray2, intArray2, equal_to<int>());
		EATEST_VERIFY(pInt == intArray1 + 10);
		pInt = find_first_of(intArray1, intArray1 + 10, intArray2, intArray2 + 3, equal_to<int>());
		EATEST_VERIFY(pInt == intArray1 + 5);
	}


	{
		// ForwardIterator1 find_first_not_of(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2)
		// ForwardIterator1 find_first_not_of(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2), BinaryPredicate predicate)

		int intArray1[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		int intArray2[3]  = { 0, 1, 2 };

		int* pInt = find_first_not_of(intArray1, intArray1, intArray2, intArray2 + 3);
		EATEST_VERIFY(pInt == intArray1);
		pInt = find_first_not_of(intArray1, intArray1 + 10, intArray2, intArray2);
		EATEST_VERIFY(pInt == intArray1 + 0);
		pInt = find_first_not_of(intArray1, intArray1 + 10, intArray2, intArray2 + 3);
		EATEST_VERIFY(pInt == intArray1 + 3);

		pInt = find_first_not_of(intArray1, intArray1, intArray2, intArray2 + 3, equal_to<int>());
		EATEST_VERIFY(pInt == intArray1);
		pInt = find_first_not_of(intArray1, intArray1 + 10, intArray2, intArray2, equal_to<int>());
		EATEST_VERIFY(pInt == intArray1 + 0);
		pInt = find_first_not_of(intArray1, intArray1 + 10, intArray2, intArray2 + 3, equal_to<int>());
		EATEST_VERIFY(pInt == intArray1 + 3);
	}


	{
		// ForwardIterator1 find_last_of(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2)
		// ForwardIterator1 find_last_of(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate predicate)

		int intArray1[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		int intArray2[3]  = { 3, 4, 5 };

		int* pInt = find_last_of(intArray1, intArray1, intArray2, intArray2 + 3);
		EATEST_VERIFY(pInt == intArray1);
		pInt = find_last_of(intArray1, intArray1 + 10, intArray2, intArray2);
		EATEST_VERIFY(pInt == intArray1 + 10);
		pInt = find_last_of(intArray1, intArray1 + 10, intArray2, intArray2 + 3);
		EATEST_VERIFY(pInt == intArray1 + 5);

		pInt = find_last_of(intArray1, intArray1, intArray2, intArray2 + 3, equal_to<int>());
		EATEST_VERIFY(pInt == intArray1);
		pInt = find_last_of(intArray1, intArray1 + 10, intArray2, intArray2, equal_to<int>());
		EATEST_VERIFY(pInt == intArray1 + 10);
		pInt = find_last_of(intArray1, intArray1 + 10, intArray2, intArray2 + 3, equal_to<int>());
		EATEST_VERIFY(pInt == intArray1 + 5);
	}


	{
		// ForwardIterator1 find_last_not_of(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2)
		// ForwardIterator1 find_last_not_of(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2), BinaryPredicate predicate)

		int intArray1[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		int intArray2[3]  = { 7, 8, 9 };

		int* pInt = find_last_not_of(intArray1, intArray1, intArray2, intArray2 + 3);
		EATEST_VERIFY(pInt == intArray1);
		pInt = find_last_not_of(intArray1, intArray1 + 10, intArray2, intArray2);
		EATEST_VERIFY(pInt == intArray1 + 10);
		pInt = find_last_not_of(intArray1, intArray1 + 10, intArray2, intArray2 + 3);
		EATEST_VERIFY(pInt == intArray1 + 6);

		pInt = find_last_not_of(intArray1, intArray1, intArray2, intArray2 + 3, equal_to<int>());
		EATEST_VERIFY(pInt == intArray1);
		pInt = find_last_not_of(intArray1, intArray1 + 10, intArray2, intArray2, equal_to<int>());
		EATEST_VERIFY(pInt == intArray1 + 10);
		pInt = find_last_not_of(intArray1, intArray1 + 10, intArray2, intArray2 + 3, equal_to<int>());
		EATEST_VERIFY(pInt == intArray1 + 6);
	}


	{
		// Function for_each(InputIterator first, InputIterator last, Function function)

		deque<int> intDeque(1000);
		SetIncrementalIntegers<int> sii; // We define this class at the top of this file.
		eastl_size_t i;

		sii = for_each(intDeque.begin(), intDeque.end(), sii);
		EATEST_VERIFY(sii.mX == 1000);
		for(i = 0; i < 1000; i++)
		{
			if(intDeque[i] != (int)i)
				break;
		}
		EATEST_VERIFY(i == 1000);


		array<int, 1000> intArray;
		sii.reset();

		sii = for_each(intArray.begin(), intArray.end(), sii);
		EATEST_VERIFY(sii.mX == 1000);
		for(i = 0; i < 1000; i++)
		{
			if(intArray[i] != (int)i)
				break;
		}
		EATEST_VERIFY(i == 1000);
	}

	// for_each_n
	{
		{
			vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
			for_each_n(v.begin(), 5, [](auto& e) { e += 10; });

			vector<int> expected = {10, 11, 12, 13, 14, 5, 6, 7, 8, 9};
			EATEST_VERIFY(v == expected);
		}

		// verify lambda can return a result that is ignored.
		{
			vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
			for_each_n(v.begin(), 5, [](auto& e) { e += 10; return 42; });

			vector<int> expected = {10, 11, 12, 13, 14, 5, 6, 7, 8, 9};
			EATEST_VERIFY(v == expected);
		}
	}

	{
		// void generate(ForwardIterator first, ForwardIterator last, Generator generator)
		// OutputIterator generate_n(OutputIterator first, Size n, Generator generator)

		deque<int> intDeque((eastl_size_t)rng.RandRange(100, 1000));
		GenerateIncrementalIntegers<int> gii(0); // We define this class at the top of this file.
		int i, iEnd;

		generate(intDeque.begin(), intDeque.end(), gii);
		for(i = 0, iEnd = (int)intDeque.size(); i < iEnd; i++)
		{
			if(intDeque[(eastl_size_t)i] != i)
				break;
		}
		EATEST_VERIFY(i == iEnd);


		array<int, 1000> intArray;
		gii.reset(0);

		generate(intArray.begin(), intArray.end(), gii);
		for(i = 0; i < 1000; i++)
		{
			if(intArray[(eastl_size_t)i] != i)
				break;
		}
		EATEST_VERIFY(i == 1000);
	}


	{
		// OutputIterator transform(InputIterator first, InputIterator last, OutputIterator result, UnaryOperation unaryOperation)

		deque<int> intDeque((eastl_size_t)rng.RandRange(1, 1000));
		int i, iEnd;

		for(i = 0, iEnd = (int)intDeque.size(); i < iEnd; i++)
			intDeque[(eastl_size_t)i] = 1;
		transform(intDeque.begin(), intDeque.begin(), intDeque.begin(), negate<int>()); // No-op
		EATEST_VERIFY(intDeque[0] == 1); // Verify nothing happened
		transform(intDeque.begin(), intDeque.end(), intDeque.begin(), negate<int>());
		for(i = 0, iEnd = (int)intDeque.size(); i < iEnd; i++)
		{
			if(intDeque[(eastl_size_t)i] != -1)
				break;
		}
		EATEST_VERIFY(i == iEnd);


		slist<TestObject> sList;
		for(i = 0, iEnd = rng.RandRange(1, 100); i < iEnd; i++)
			sList.push_front(TestObject(1));
		transform(sList.begin(), sList.begin(), sList.begin(), TestObjectNegate()); // No-op
		EATEST_VERIFY(sList.front() == TestObject(1));
		transform(sList.begin(), sList.end(), sList.begin(), TestObjectNegate()); // TestObjectNegate is a custom function we define for this test.
		slist<TestObject>::iterator it = sList.begin();
		for(; it != sList.end(); it++)
		{
			if(!(*it == TestObject(-1)))
				break;
		}
		EATEST_VERIFY(it == sList.end());
	}


	{
		// OutputIterator transform(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, OutputIterator result, BinaryOperation binaryOperation)

		int intArray1[12] = { 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1 };
		int intArray2[12] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };

		int* pInt = transform(intArray1, intArray1, intArray2, intArray2, plus<int>());
		EATEST_VERIFY(pInt == intArray2);
		EATEST_VERIFY(VerifySequence(intArray1, intArray1 + 12, int(), "transform", 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, -1));
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 12, int(), "transform", 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, -1));

		pInt = transform(intArray1, intArray1 + 12, intArray2, intArray2, plus<int>());
		EATEST_VERIFY(pInt == intArray2 + 12);
		EATEST_VERIFY(VerifySequence(intArray1, intArray1 + 12, int(), "transform", 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, -1));
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 12, int(), "transform", 3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 4, 4, -1));
	}


	{
		// bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
		// bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryPredicate predicate)

		vector<eastl_size_t> intArray(100);
		list<eastl_size_t>   intList(100);
		generate(intArray.begin(), intArray.end(), rng);
		copy(intArray.begin(), intArray.end(), intList.begin());

		bool b = equal(intArray.begin(), intArray.begin(), (eastl_size_t*)NULL);
		EATEST_VERIFY(b);
		b = equal(intArray.begin(), intArray.end(), intList.begin());
		EATEST_VERIFY(b);
		intArray[50] += 1;
		b = equal(intArray.begin(), intArray.end(), intList.begin());
		EATEST_VERIFY(!b);

		intArray[50] -= 1; // resulttore its original value so the containers are equal again.
		b = equal(intArray.begin(), intArray.begin(), (eastl_size_t*)NULL, equal_to<eastl_size_t>());
		EATEST_VERIFY(b);
		b = equal(intArray.begin(), intArray.end(), intList.begin(), equal_to<eastl_size_t>());
		EATEST_VERIFY(b);
		intArray[50] += 1;
		b = equal(intArray.begin(), intArray.end(), intList.begin(), equal_to<eastl_size_t>());
		EATEST_VERIFY(!b);
	}


	{
		// bool identical(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
		// bool identical(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, BinaryPredicate predicate)

		vector<eastl_size_t> intArray(100);
		list<eastl_size_t>   intList(100);
		generate(intArray.begin(), intArray.end(), rng);
		copy(intArray.begin(), intArray.end(), intList.begin());


		bool b = identical(intArray.begin(), intArray.begin(), (eastl_size_t*)NULL, (eastl_size_t*)NULL);
		EATEST_VERIFY(b);
		b = identical(intArray.begin(), intArray.end(), intList.begin(), intList.end());
		EATEST_VERIFY(b);
		b = identical(intArray.begin(), intArray.end() - 10, intList.begin(), intList.end());
		EATEST_VERIFY(!b);
		b = identical(intList.begin(), intList.end(), intArray.begin() + 10, intArray.end());
		EATEST_VERIFY(!b);
		intArray[50] += 1;
		b = identical(intArray.begin(), intArray.end(), intList.begin(), intList.end());
		EATEST_VERIFY(!b);


		intArray[50] -= 1; // resulttore its original value so the containers are equal again.
		b = identical(intArray.begin(), intArray.begin(), (eastl_size_t*)NULL, (eastl_size_t*)NULL, equal_to<eastl_size_t>());
		EATEST_VERIFY(b);
		b = identical(intArray.begin(), intArray.end(), intList.begin(), intList.end(), equal_to<eastl_size_t>());
		EATEST_VERIFY(b);
		b = identical(intArray.begin(), intArray.end() - 10, intList.begin(), intList.end(), equal_to<eastl_size_t>());
		EATEST_VERIFY(!b);
		b = identical(intList.begin(), intList.end(), intArray.begin() + 10, intArray.end(), equal_to<eastl_size_t>());
		EATEST_VERIFY(!b);
		intArray[50] += 1;
		b = identical(intArray.begin(), intArray.end(), intList.begin(), intList.end(), equal_to<eastl_size_t>());
		EATEST_VERIFY(!b);
	}


	{
		// bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
		// bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, Compare compare)

		int intArray1[6] = { 0, 1, 2, 3, 4, 5 };
		int intArray2[6] = { 0, 1, 2, 3, 4, 6 };
		int intArray3[5] = { 0, 1, 2, 3, 4    };

		bool b = lexicographical_compare(intArray1, intArray1, intArray2, intArray2); // Test empty range.
		EATEST_VERIFY(!b);
		b = lexicographical_compare(intArray1, intArray1 + 6, intArray2, intArray2 + 6);
		EATEST_VERIFY( b);
		b = lexicographical_compare(intArray2, intArray2 + 6, intArray1, intArray1 + 6);
		EATEST_VERIFY(!b);
		b = lexicographical_compare(intArray1, intArray1 + 6, intArray3, intArray3 + 5);
		EATEST_VERIFY(!b);

		b = lexicographical_compare(intArray1, intArray1, intArray2, intArray2, greater<int>()); // Test empty range.
		EATEST_VERIFY(!b);
		b = lexicographical_compare(intArray1, intArray1 + 6, intArray2, intArray2 + 6, greater<int>());
		EATEST_VERIFY(!b);
		b = lexicographical_compare(intArray2, intArray2 + 6, intArray1, intArray1 + 6, greater<int>());
		EATEST_VERIFY( b);
		b = lexicographical_compare(intArray3, intArray3 + 5, intArray1, intArray1 + 6, less<int>());
		EATEST_VERIFY( b);
	}

	{
		// bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
		// bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, Compare compare)

		char* cstr = nullptr;

		bool b = lexicographical_compare(cstr, cstr, cstr, cstr);
		EATEST_VERIFY(!b);
	}

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	{
		// <compairison_category> lexicographical_compare_three_way(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, Compare compare)

		int intArray1[6] = {0, 1, 2, 3, 4, 5};
		int intArray2[6] = {0, 1, 2, 3, 4, 6};
		int intArray3[5] = {0, 1, 2, 3, 4};
		int intArray4[5] = {4, 3, 2, 1, 0};

		// strong ordering
		auto compare_strong = [](int first, int second)
		{
			return (first < second) ? std::strong_ordering::less :
				(first > second) ? std::strong_ordering::greater :
				std::strong_ordering::equal;
		};

		auto b = lexicographical_compare_three_way(intArray1, intArray1 + 6, intArray2, intArray2 + 6, compare_strong);
		EATEST_VERIFY(b == std::strong_ordering::less);
		b = lexicographical_compare_three_way(intArray3, intArray3 + 5, intArray2, intArray2 + 6, compare_strong);
		EATEST_VERIFY(b == std::strong_ordering::less);
		b = lexicographical_compare_three_way(intArray3, intArray3 + 5, intArray2, intArray2 + 6, synth_three_way{});
		EATEST_VERIFY(b == std::strong_ordering::less);

		b = lexicographical_compare_three_way(intArray2, intArray2 + 6, intArray1, intArray1 + 6, compare_strong);
		EATEST_VERIFY(b == std::strong_ordering::greater);
		b = lexicographical_compare_three_way(intArray2, intArray2 + 6, intArray1, intArray1 + 6, synth_three_way{});
		EATEST_VERIFY(b == std::strong_ordering::greater);

		b = lexicographical_compare_three_way(intArray1, intArray1 + 6, intArray3, intArray3 + 5, compare_strong);
		EATEST_VERIFY(b == std::strong_ordering::greater);
		b = lexicographical_compare_three_way(intArray1, intArray1 + 6, intArray3, intArray3 + 5, synth_three_way{});
		EATEST_VERIFY(b == std::strong_ordering::greater);

		b = lexicographical_compare_three_way(intArray1, intArray1, intArray2, intArray2, compare_strong); // Test empty range.
		EATEST_VERIFY(b == std::strong_ordering::equal);
		b = lexicographical_compare_three_way(intArray1, intArray1, intArray2, intArray2, synth_three_way{}); // Test empty range.
		EATEST_VERIFY(b == std::strong_ordering::equal);

		// weak ordering
		auto compare_weak = [](int first, int second)
		{
		    return (first < second) ? std::weak_ordering::less :
			    (first > second) ? std::weak_ordering::greater :
			    std::weak_ordering::equivalent;
		};

		auto c = lexicographical_compare_three_way(intArray3, intArray3 + 5, intArray4, intArray4 + 5, compare_weak);
		EATEST_VERIFY(c == std::weak_ordering::less);
		c = lexicographical_compare_three_way(intArray4, intArray4 + 5, intArray3, intArray3 + 5, compare_weak);
		EATEST_VERIFY(c == std::weak_ordering::greater);
		c = lexicographical_compare_three_way(intArray3, intArray3 + 5, intArray4, intArray4 + 5, synth_three_way{});
		EATEST_VERIFY(c == std::weak_ordering::less);
		c = lexicographical_compare_three_way(intArray4, intArray4 + 5, intArray3, intArray3 + 5, synth_three_way{});
		EATEST_VERIFY(c == std::weak_ordering::greater);
	}

	{
		EATEST_VERIFY(synth_three_way{}(1, 1) == std::strong_ordering::equal);
		EATEST_VERIFY(synth_three_way{}(2, 1) == std::strong_ordering::greater);
		EATEST_VERIFY(synth_three_way{}(1, 2) == std::strong_ordering::less);

		struct weak_struct
		{
			int val;
			inline std::weak_ordering operator<=>(const weak_struct& b) const
			{
				return val <=> b.val;
			}
		};

		EATEST_VERIFY(synth_three_way{}(weak_struct{1}, weak_struct{2}) == std::weak_ordering::less);
		EATEST_VERIFY(synth_three_way{}(weak_struct{2}, weak_struct{1}) == std::weak_ordering::greater);
		EATEST_VERIFY(synth_three_way{}(weak_struct{1}, weak_struct{1}) == std::weak_ordering::equivalent);
	}
#endif

	{
		// ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value)
		// ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare compare)

		int i;

		int* pInt = lower_bound((int*)NULL, (int*)NULL, 100);
		EATEST_VERIFY(pInt == NULL);


		for(i = 0; i < 20 + (gEASTL_TestLevel * 20); i++)
		{
			deque<int> intDeque((eastl_size_t)rng.RandRange(1, 500));

			for(int j = 0, jEnd = (int)intDeque.size(); j < jEnd; j++)
				intDeque[(eastl_size_t)j] = (int)rng.RandLimit(jEnd / 2); // This will result in both gaps and duplications.

			for(int k = 0, kEnd = (int)intDeque.size(); k < kEnd; k++)
			{
				deque<int>::iterator it = lower_bound(intDeque.begin(), intDeque.end(), k);

				if(it != intDeque.begin())
					EATEST_VERIFY(*(it - 1) < k);

				if(it != intDeque.end())
					EATEST_VERIFY((k < *it) || !(*it < k)); // Verify tha k <= *it by using only operator<
			}
		}


		for(i = 0; i < 20 + (gEASTL_TestLevel * 20); i++)
		{
			list<TestObject> toList;
			int              nSize = (int)rng.RandRange(1, 500);

			for(int j = 0, jEnd = nSize; j < jEnd; j++)
				toList.push_back(TestObject((int)rng.RandLimit(jEnd / 2))); // This will result in both gaps and duplications.

			for(int k = 0; k < nSize; k++)
			{
				TestObject toK(k);
				list<TestObject>::iterator it = lower_bound(toList.begin(), toList.end(), toK);

				if(it != toList.begin())
				{
					--it;
					EATEST_VERIFY(*it < toK);
					++it;
				}

				if(it != toList.end())
					EATEST_VERIFY((toK < *it) || !(*it < toK)); // Verify tha k <= *it by using only operator<
			}
		}
	}


	{
		// ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value)
		// ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare compare)

		int i;

		int* pInt = upper_bound((int*)NULL, (int*)NULL, 100);
		EATEST_VERIFY(pInt == NULL);


		for(i = 0; i < 20 + (gEASTL_TestLevel * 20); i++)
		{
			deque<int> intDeque((eastl_size_t)rng.RandRange(1, 500));

			for(eastl_size_t j = 0, jEnd = intDeque.size(); j < jEnd; j++)
				intDeque[j] = (int)rng.RandLimit((uint32_t)jEnd / 2); // This will result in both gaps and duplications.

			for(int k = 0, kEnd = (int)intDeque.size(); k < kEnd; k++)
			{
				deque<int>::iterator it = upper_bound(intDeque.begin(), intDeque.end(), k);

				if(it != intDeque.begin())
					EATEST_VERIFY((*(it - 1) < k) || !(k < *(it - 1))); // Verify tha *it <= k by using only operator<

				if(it != intDeque.end())
					EATEST_VERIFY(k < *it);
			}
		}


		for(i = 0; i < 20 + (gEASTL_TestLevel * 20); i++)
		{
			list<TestObject> toList;
			int              nSize = (int)rng.RandRange(1, 500);

			for(int j = 0, jEnd = nSize; j < jEnd; j++)
				toList.push_back(TestObject((int)rng.RandLimit(jEnd / 2))); // This will result in both gaps and duplications.

			for(int k = 0; k < nSize; k++)
			{
				TestObject toK(k);
				list<TestObject>::iterator it = upper_bound(toList.begin(), toList.end(), toK);

				if(it != toList.begin())
				{
					--it;
					EATEST_VERIFY((*it < toK) || !(toK < *it)); // Verify tha *it <= k by using only operator<
					++it;
				}

				if(it != toList.end())
					EATEST_VERIFY(toK < *it);
			}
		}
	}


	{
		// pair<ForwardIterator, ForwardIterator> equal_range(ForwardIterator first, ForwardIterator last, const T& value)
		// pair<ForwardIterator, ForwardIterator> equal_range(ForwardIterator first, ForwardIterator last, const T& value, Compare compare)

		int i;

		pair<int*, int*> pInt = equal_range((int*)NULL, (int*)NULL, 100);
		EATEST_VERIFY(pInt.first  == NULL);
		EATEST_VERIFY(pInt.second == NULL);


		for(i = 0; i < 20 + (gEASTL_TestLevel * 20); i++)
		{
			deque<int> intDeque((eastl_size_t)rng.RandRange(1, 500));

			for(int j = 0, jEnd = (int)intDeque.size(); j < jEnd; j++)
				intDeque[(eastl_size_t)j] = (int)rng.RandLimit(jEnd / 2); // This will result in both gaps and duplications.

			for(int k = 0, kEnd = (int)intDeque.size(); k < kEnd; k++)
			{
				pair<deque<int>::iterator, deque<int>::iterator> it = equal_range(intDeque.begin(), intDeque.end(), k);

				// Test it.first as lower_bound.
				if(it.first != intDeque.begin())
					EATEST_VERIFY(*(it.first - 1) < k);

				if(it.first != intDeque.end())
					EATEST_VERIFY((k < *it.first) || !(*it.first < k)); // Verify tha k <= *it by using only operator<

				// Test it.second as upper_bound.
				if(it.second != intDeque.begin())
					EATEST_VERIFY((*(it.second - 1) < k) || !(k < *(it.second - 1))); // Verify tha *it <= k by using only operator<

				if(it.second != intDeque.end())
					EATEST_VERIFY(k < *it.second);
			}
		}


		for(i = 0; i < 20 + (gEASTL_TestLevel * 20); i++)
		{
			list<TestObject> toList;
			int              nSize = (int)rng.RandRange(1, 500);

			for(int j = 0, jEnd = nSize; j < jEnd; j++)
				toList.push_back(TestObject((int)rng.RandLimit(jEnd / 2))); // This will result in both gaps and duplications.

			for(int k = 0; k < nSize; k++)
			{
				TestObject toK(k);
				pair<list<TestObject>::iterator, list<TestObject>::iterator> it = equal_range(toList.begin(), toList.end(), toK);

				// Test it.first as lower_bound
				if(it.first != toList.begin())
				{
					--it.first;
					EATEST_VERIFY(*it.first < toK);
					++it.first;
				}

				if(it.first != toList.end())
					EATEST_VERIFY((toK < *it.first) || !(*it.first < toK)); // Verify tha k <= *it by using only operator<

				// Test it.second as upper_bound
				if(it.second != toList.begin())
				{
					--it.second;
					EATEST_VERIFY((*it.second < toK) || !(toK < *it.second)); // Verify tha *it <= k by using only operator<
					++it.second;
				}

				if(it.second != toList.end())
					EATEST_VERIFY(toK < *it.second);
			}
		}
	}


	{
		// void replace(ForwardIterator first, ForwardIterator last, const T& old_value, const T& new_value)
		// void replace_if(ForwardIterator first, ForwardIterator last, Predicate predicate, const T& new_value)

		int intArray[8] = { 0, 3, 2, 7, 5, 4, 5, 3, };

		// Convert 3s to 99s.
		replace(intArray, intArray,     3, 99); // No-op
		EATEST_VERIFY((intArray[1] == 3) && (intArray[7] == 3));
		replace(intArray, intArray + 8, 3, 99); // No-op
		EATEST_VERIFY((intArray[1] == 99) && (intArray[7] == 99));

		// Convert 99s to 88s.
		replace_if(intArray, intArray, [](int i) { return i == 99; }, 88); // No-op
		EATEST_VERIFY((intArray[1] == 99) && (intArray[7] == 99));
		replace_if(intArray, intArray + 8, [](int i) { return i == 99; }, 88);
		EATEST_VERIFY((intArray[1] == 88) && (intArray[7] == 88));


		slist<TestObject> toList;
		slist<TestObject>::iterator it;
		toList.push_front(TestObject(3));
		toList.push_front(TestObject(5));
		toList.push_front(TestObject(4));
		toList.push_front(TestObject(5));
		toList.push_front(TestObject(7));
		toList.push_front(TestObject(2));
		toList.push_front(TestObject(3));
		toList.push_front(TestObject(0));

		// Convert 3s to 99s.
		replace(toList.begin(), toList.begin(), TestObject(3), TestObject(99)); // No-op
		it = toList.begin();
		advance(it, 1);
		EATEST_VERIFY(*it == TestObject(3));
		advance(it, 6);
		EATEST_VERIFY(*it == TestObject(3));
		replace(toList.begin(), toList.end(), TestObject(3), TestObject(99));
		it = toList.begin();
		advance(it, 1);
		EATEST_VERIFY(*it == TestObject(99));
		advance(it, 6);
		EATEST_VERIFY(*it == TestObject(99));

		// Convert 99s to 88s.
		replace_if(toList.begin(), toList.begin(), [](const TestObject& lhs) { return lhs == TestObject(99); }, TestObject(88)); // No-op
		it = toList.begin();
		advance(it, 1);
		EATEST_VERIFY(*it == TestObject(99));
		advance(it, 6);
		EATEST_VERIFY(*it == TestObject(99));
		replace_if(toList.begin(), toList.end(), [](const TestObject& lhs) { return lhs == TestObject(99); }, TestObject(88));
		it = toList.begin();
		advance(it, 1);
		EATEST_VERIFY(*it == TestObject(88));
		advance(it, 6);
		EATEST_VERIFY(*it == TestObject(88));
	}


	{
		// OutputIterator remove_copy(InputIterator first, InputIterator last, OutputIterator result, const T& value)
		// OutputIterator remove_copy_if(InputIterator first, InputIterator last, OutputIterator result, Predicate predicate)

		int intArray1[12] = { 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1 };
		int intArray2[12] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };

		int* pInt = remove_copy(intArray1, intArray1, intArray2, 1); // No-op
		EATEST_VERIFY(pInt == intArray2);
		EATEST_VERIFY(VerifySequence(intArray1, intArray1 + 12, int(), "remove_copy", 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, -1));
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 12, int(), "remove_copy", 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, -1));

		pInt = remove_copy(intArray1, intArray1 + 12, intArray2, 1);
		EATEST_VERIFY(pInt == intArray2 + 6);
		EATEST_VERIFY(VerifySequence(intArray1, intArray1 + 12, int(), "remove_copy", 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, -1));
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 12, int(), "remove_copy", 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, -1));


		pInt = remove_copy_if(intArray1, intArray1, intArray2, [](int i) { return i == 0; }); // No-op
		EATEST_VERIFY(pInt == intArray2);
		EATEST_VERIFY(VerifySequence(intArray1, intArray1 + 12, int(), "remove_copy_if", 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, -1));
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 12, int(), "remove_copy_if", 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, -1));

		pInt = remove_copy_if(intArray1, intArray1 + 12, intArray2, [](int i) { return i == 0; });
		EATEST_VERIFY(pInt == intArray2 + 6);
		EATEST_VERIFY(VerifySequence(intArray1, intArray1 + 12, int(), "remove_copy_if", 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, -1));
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 12, int(), "remove_copy_if", 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, -1));
	}


	{
		// ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T& value)
		// ForwardIterator remove_if(ForwardIterator first, ForwardIterator last, Predicate predicate)

		int intArray1[12] = { 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1 };
		int intArray2[12] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };

		int* pInt = remove(intArray1, intArray1, 1);
		EATEST_VERIFY(pInt == intArray1);
		EATEST_VERIFY(VerifySequence(intArray1, intArray1 + 12, int(), "remove", 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, -1));
		pInt = remove(intArray1, intArray1 + 12, 1);
		EATEST_VERIFY(pInt == intArray1 + 6);
		EATEST_VERIFY(VerifySequence(intArray1, intArray1 + 12, int(), "remove", 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, -1));

		pInt = remove(intArray2, intArray2, 1);
		EATEST_VERIFY(pInt == intArray2);
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 12, int(), "remove", 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, -1));
		pInt = remove(intArray2, intArray2 + 12, 1);
		EATEST_VERIFY(pInt == intArray2 + 12);
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 12, int(), "remove", 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, -1));
	}


    {
	    // ForwardIterator apply_and_remove(ForwardIterator first, ForwardIterator last, Function function, const T&
	    // value) ForwardIterator apply_and_remove_if(ForwardIterator first, ForwardIterator last, Function function,
	    // Predicate predicate)

	    // Test for empty range and full container range
	    {
		    int intArray[12] = {0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1};
		    vector<int> output;
		    auto func = [&output](int a) { output.push_back(a); };
		    int* pInt = apply_and_remove(intArray, intArray, func, 1);
		    EATEST_VERIFY(pInt == intArray);
		    EATEST_VERIFY(VerifySequence(intArray, intArray + 12, int(), "apply_and_remove", 0, 0, 1, 1, 0, 0, 1, 1, 0,
		                                 0, 1, 1, -1));
		    EATEST_VERIFY(VerifySequence(output.begin(), output.end(), int(), "apply_and_remove", -1));
		    pInt = apply_and_remove(intArray, intArray + 12, func, 1);
		    EATEST_VERIFY(pInt == intArray + 6);
		    EATEST_VERIFY(VerifySequence(intArray, intArray + 6, int(), "apply_and_remove", 0, 0, 0, 0, 0, 0, -1));
		    EATEST_VERIFY(
		        VerifySequence(output.begin(), output.end(), int(), "apply_and_remove", 1, 1, 1, 1, 1, 1, -1));
	    }

	    // Test for no match on empty range and full container range
	    {
		    int intArray[12] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
		    vector<int> output;
		    auto func = [&output](int a) { output.push_back(a); };
		    int* pInt = apply_and_remove(intArray, intArray, func, 1);
		    EATEST_VERIFY(pInt == intArray);
		    EATEST_VERIFY(VerifySequence(intArray, intArray + 12, int(), "apply_and_remove", 3, 3, 3, 3, 3, 3, 3, 3, 3,
		                                 3, 3, 3, -1));
		    EATEST_VERIFY(VerifySequence(output.begin(), output.end(), int(), "apply_and_remove", -1));
		    pInt = apply_and_remove(intArray, intArray + 12, func, 1);
		    EATEST_VERIFY(pInt == intArray + 12);
		    EATEST_VERIFY(VerifySequence(intArray, intArray + 12, int(), "apply_and_remove", 3, 3, 3, 3, 3, 3, 3, 3, 3,
		                                 3, 3, 3, -1));
		    EATEST_VERIFY(VerifySequence(output.begin(), output.end(), int(), "apply_and_remove", -1));
	    }

	    // Test for empty range and full container range
	    {
		    int intArray[12] = {0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1};
		    vector<int> output;
		    auto func = [&output](int a) { output.push_back(a); };
		    int* pInt = apply_and_remove_if(intArray, intArray, func, [](int i) { return i == 1; });
		    EATEST_VERIFY(pInt == intArray);
		    EATEST_VERIFY(VerifySequence(intArray, intArray + 12, int(), "apply_and_remove_if", 0, 0, 1, 1, 0, 0, 1, 1,
		                                 0, 0, 1, 1, -1));
		    EATEST_VERIFY(VerifySequence(output.begin(), output.end(), int(), "apply_and_remove_if", -1));
		    pInt = apply_and_remove_if(intArray, intArray + 12, func, [](int i) { return i == 1; });
		    EATEST_VERIFY(pInt == intArray + 6);
		    EATEST_VERIFY(VerifySequence(intArray, intArray + 6, int(), "apply_and_remove_if", 0, 0, 0, 0, 0, 0, -1));
		    EATEST_VERIFY(
		        VerifySequence(output.begin(), output.end(), int(), "apply_and_remove_if", 1, 1, 1, 1, 1, 1, -1));
	    }

	    // Test for no match on empty range and full container range
	    {
		    int intArray[12] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
		    vector<int> output;
		    auto func = [&output](int a) { output.push_back(a); };
		    int* pInt = apply_and_remove_if(intArray, intArray, func, [](int i) { return i == 1; });
		    EATEST_VERIFY(pInt == intArray);
		    EATEST_VERIFY(VerifySequence(intArray, intArray + 12, int(), "apply_and_remove_if", 3, 3, 3, 3, 3, 3, 3, 3,
		                                 3, 3, 3, 3, -1));
		    EATEST_VERIFY(VerifySequence(output.begin(), output.end(), int(), "apply_and_remove_if", -1));
		    pInt = apply_and_remove_if(intArray, intArray + 12, func, [](int i) { return i == 1; });
		    EATEST_VERIFY(pInt == intArray + 12);
		    EATEST_VERIFY(VerifySequence(intArray, intArray + 12, int(), "apply_and_remove_if", 3, 3, 3, 3, 3, 3, 3, 3,
		                                 3, 3, 3, 3, -1));
		    EATEST_VERIFY(VerifySequence(output.begin(), output.end(), int(), "apply_and_remove_if", -1));
	    }

	    auto even = [](int a) { return (a % 2) == 0; };
	    // Test to verify that the remaining element have stable ordering
	    {
		    int intArray[12] = {7, 8, 2, 3, 4, 5, 6, 0, 1, 9, 10, 11};
		    vector<int> output;
		    auto func = [&output](int a) { output.push_back(a); };
		    int* pInt = apply_and_remove_if(intArray, intArray + 12, func, even);
		    EATEST_VERIFY(pInt == intArray + 6);
		    EATEST_VERIFY(VerifySequence(intArray, intArray + 6, int(), "apply_and_remove_if", 7, 3, 5, 1, 9, 11, -1));
		    EATEST_VERIFY(
		        VerifySequence(output.begin(), output.end(), int(), "apply_and_remove_if", 8, 2, 4, 6, 0, 10, -1));
	    }
	    {
		    int intArray[12] = {7, 8, 0, 0, 4, 5, 6, 0, 1, 9, 0, 11};
		    vector<int> output;
		    auto func = [&output](int a) { output.push_back(a); };
		    int* pInt = apply_and_remove(intArray, intArray + 12, func, 0);
		    EATEST_VERIFY(pInt == intArray + 8);
		    EATEST_VERIFY(
		        VerifySequence(intArray, intArray + 8, int(), "apply_and_remove", 7, 8, 4, 5, 6, 1, 9, 11, -1));
		    EATEST_VERIFY(VerifySequence(output.begin(), output.end(), int(), "apply_and_remove", 0, 0, 0, 0, -1));
	    }

	    // Tests on a list (i.e. non-contiguous memory container)
	    {
		    list<int> intList = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
		    vector<int> output;
		    auto func = [&output](int a) { output.push_back(a); };
		    auto listIter = apply_and_remove_if(intList.begin(), intList.begin(), func, even);
		    EATEST_VERIFY(listIter == intList.begin());
		    EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "apply_and_remove_if", 0, 1, 2, 3, 4, 5,
		                                 6, 7, 8, 9, 10, 11, -1));
		    EATEST_VERIFY(VerifySequence(output.begin(), output.end(), int(), "apply_and_remove_if", -1));
		    listIter = apply_and_remove_if(intList.begin(), intList.end(), func, even);
		    EATEST_VERIFY(listIter == next(intList.begin(), 6));
		    EATEST_VERIFY(
		        VerifySequence(intList.begin(), listIter, int(), "apply_and_remove_if", 1, 3, 5, 7, 9, 11, -1));
		    EATEST_VERIFY(
		        VerifySequence(output.begin(), output.end(), int(), "apply_and_remove_if", 0, 2, 4, 6, 8, 10, -1));
	    }
	    {
		    list<int> intList = {0, 4, 2, 3, 4, 5, 6, 4, 4, 4, 10, 11};
		    vector<int> output;
		    auto func = [&output](int a) { output.push_back(a); };
		    auto listIter = apply_and_remove(intList.begin(), intList.begin(), func, 4);
		    EATEST_VERIFY(listIter == intList.begin());
		    EATEST_VERIFY(VerifySequence(intList.begin(), intList.end(), int(), "apply_and_remove", 0, 4, 2, 3, 4, 5, 6,
		                                 4, 4, 4, 10, 11, -1));
		    EATEST_VERIFY(VerifySequence(output.begin(), output.end(), int(), "apply_and_remove", -1));
		    listIter = apply_and_remove(intList.begin(), intList.end(), func, 4);
		    EATEST_VERIFY(listIter == next(intList.begin(), 7));
		    EATEST_VERIFY(
		        VerifySequence(intList.begin(), listIter, int(), "apply_and_remove", 0, 2, 3, 5, 6, 10, 11, -1));
		    EATEST_VERIFY(VerifySequence(output.begin(), output.end(), int(), "apply_and_remove", 4, 4, 4, 4, 4, -1));
	    }

	    // Tests on a part of a container
	    {
		    vector<int> intVector = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
		    vector<int> output;
		    auto func = [&output](int a) { output.push_back(a); };
		    auto vectorIter = apply_and_remove_if(next(intVector.begin(), 3), prev(intVector.end(), 2), func, even);
		    EATEST_VERIFY(vectorIter == next(intVector.begin(), 7));
		    EATEST_VERIFY(
		        VerifySequence(intVector.begin(), vectorIter, int(), "apply_and_remove_if", 0, 1, 2, 3, 5, 7, 9, -1));
		    EATEST_VERIFY(
		        VerifySequence(prev(intVector.end(), 2), intVector.end(), int(), "apply_and_remove_if", 10, 11, -1));
		    EATEST_VERIFY(VerifySequence(output.begin(), output.end(), int(), "apply_and_remove_if", 4, 6, 8, -1));
	    }
	    {
		    vector<int> intVector = {5, 1, 5, 3, 4, 5, 5, 7, 8, 5, 10, 5};
		    vector<int> output;
		    auto func = [&output](int a) { output.push_back(a); };
		    auto vectorIter = apply_and_remove(next(intVector.begin(), 2), prev(intVector.end(), 3), func, 5);
		    EATEST_VERIFY(vectorIter == next(intVector.begin(), 6));
		    EATEST_VERIFY(
		        VerifySequence(intVector.begin(), vectorIter, int(), "apply_and_remove", 5, 1, 3, 4, 7, 8, -1));
		    EATEST_VERIFY(
		        VerifySequence(prev(intVector.end(), 3), intVector.end(), int(), "apply_and_remove", 5, 10, 5, -1));
		    EATEST_VERIFY(VerifySequence(output.begin(), output.end(), int(), "apply_and_remove", 5, 5, 5, -1));
	    }
    }


    {
		// OutputIterator replace_copy(InputIterator first, InputIterator last, OutputIterator result, const T& old_value, const T& new_value)
		// OutputIterator replace_copy_if(InputIterator first, InputIterator last, OutputIterator result, Predicate predicate, const T& new_value)

		int intArray1[12] = { 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1 };
		int intArray2[12] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };

		int* pInt = replace_copy(intArray1, intArray1, intArray2, 1, 4);
		EATEST_VERIFY(pInt == intArray2);
		EATEST_VERIFY(VerifySequence(intArray1, intArray1 + 12, int(), "replace_copy", 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, -1));
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 12, int(), "replace_copy", 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, -1));

		pInt = replace_copy(intArray1, intArray1 + 12, intArray2, 1, 4);
		EATEST_VERIFY(pInt == intArray2 + 12);
		EATEST_VERIFY(VerifySequence(intArray1, intArray1 + 12, int(), "replace_copy", 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, -1));
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 12, int(), "replace_copy", 0, 0, 4, 4, 0, 0, 4, 4, 0, 0, 4, 4, -1));
	}


	{
		// void reverse(BidirectionalIterator first, BidirectionalIterator last)

		vector<int> intArray;
		for(int i = 0; i < 10; i++)
			intArray.push_back(i);

		reverse(intArray.begin(), intArray.begin()); // No-op
		EATEST_VERIFY(VerifySequence(intArray.begin(), intArray.end(), int(), "reverse", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1));

		reverse(intArray.begin(), intArray.end());
		EATEST_VERIFY(VerifySequence(intArray.begin(), intArray.end(), int(), "reverse", 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, -1));


		list<TestObject> toList;
		for(int j = 0; j < 10; j++)
			toList.push_back(TestObject(j));

		reverse(toList.begin(), toList.begin()); // No-op
		EATEST_VERIFY(toList.front() == TestObject(0));
		EATEST_VERIFY(toList.back() == TestObject(9));

		reverse(toList.begin(), toList.end());
		EATEST_VERIFY(toList.front() == TestObject(9));
		EATEST_VERIFY(toList.back() == TestObject(0));

		// Verify that reversing an empty range executes without exception.
		reverse(toList.begin(), toList.begin());
	}


	{
		// reverse_copy(BidirectionalIterator first, BidirectionalIterator last, OutputIterator result)

		vector<int> intArray1;
		int         intArray2[10] = { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };

		for(int i = 0; i < 10; i++)
			intArray1.push_back(i);

		int* pInt = reverse_copy(intArray1.begin(), intArray1.begin(), intArray2); // No-op
		EATEST_VERIFY(pInt == intArray2);
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 10, int(), "reverse_copy", 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, -1));

		pInt = reverse_copy(intArray1.begin(), intArray1.end(), intArray2);
		EATEST_VERIFY(pInt == intArray2 + intArray1.size());
		EATEST_VERIFY(VerifySequence(intArray2, intArray2 + 10, int(), "reverse_copy", 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, -1));


		list<TestObject> toList;
		TestObject       toArray2[10];

		for(int j = 0; j < 10; j++)
		{
			toList.push_back(TestObject(j));
			toArray2[j] = TestObject(5);
		}

		TestObject* pTO = reverse_copy(toList.begin(), toList.begin(), toArray2); // No-op
		EATEST_VERIFY(pTO == toArray2);
		EATEST_VERIFY(toArray2[0] == TestObject(5));
		EATEST_VERIFY(toArray2[9] == TestObject(5));

		pTO = reverse_copy(toList.begin(), toList.end(), toArray2);
		EATEST_VERIFY(pTO == toArray2 + 10);
	}


	{
		// ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2)
		// ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate predicate)

		// Test via bidirectional/random_access iterator.
		basic_string<char> sTest("abcdefg abcdefg abcdefg");
		const char* pSubstring1 = " abcd";
		const char* pSubstring2 = "1234";

		basic_string<char>::iterator iString = search(sTest.begin(), sTest.end(), pSubstring1, pSubstring1 + strlen(pSubstring1));
		EATEST_VERIFY(&*iString == &sTest[7]);

		iString = search(sTest.begin(), sTest.end(), pSubstring1, pSubstring1 + 1); // Search for sequence of 1.
		EATEST_VERIFY(&*iString == &sTest[7]);

		iString = search(sTest.begin(), sTest.end(), pSubstring2, pSubstring2 + strlen(pSubstring2));
		EATEST_VERIFY(&*iString == sTest.end());

		iString = search(sTest.begin(), sTest.end(), pSubstring2, pSubstring2); // Search with empty search pattern.
		EATEST_VERIFY(&*iString == sTest.begin());

		// Test via forward iterator.
		slist<char> sListTest;
		for(slist<char>::size_type i = sTest.size(); i > 0; --i)
			sListTest.push_front(sTest[i - 1]);

		slist<char>::iterator iSlist = search(sListTest.begin(), sListTest.end(), pSubstring1, pSubstring1 + 5);
		slist<char>::iterator i7 = sListTest.begin();
		advance(i7, 7);
		EATEST_VERIFY(iSlist == i7);

		iSlist = search(sListTest.begin(), sListTest.end(), pSubstring2, pSubstring2 + strlen(pSubstring2));
		EATEST_VERIFY(iSlist == sListTest.end());

		iSlist = search(sListTest.begin(), sListTest.end(), pSubstring2, pSubstring2); // Search with empty search pattern.
		EATEST_VERIFY(iSlist == sListTest.begin());
	}


	{
		// ForwardIterator search_n(ForwardIterator first, ForwardIterator last, Size count, const T& value)

		const char* pString1 = "Hello wwworld";
		const char* presultult  = search_n(pString1, pString1 + strlen(pString1), 1, 'w');
		EATEST_VERIFY(presultult == pString1 + 6);
	}


	{
		// bool binary_search(ForwardIterator first, ForwardIterator last, const T& value)
		// bool binary_search(ForwardIterator first, ForwardIterator last, const T& value, Compare compare)

		// ForwardIterator binary_search_i(ForwardIterator first, ForwardIterator last, const T& value)
		// ForwardIterator binary_search_i(ForwardIterator first, ForwardIterator last, const T& value, Compare compare)

		vector<int> intArray;
		for(int i = 0; i < 1000; i++)
			intArray.push_back(i);

		bool b = binary_search(intArray.begin(), intArray.begin(), 0);
		EATEST_VERIFY(b == false);

		b = binary_search(intArray.begin(), intArray.begin() + 1, 0);
		EATEST_VERIFY(b == true);

		b = binary_search(intArray.begin(), intArray.end(), 733, less<int>());
		EATEST_VERIFY(b == true);


		vector<int>::iterator it = binary_search_i(intArray.begin(), intArray.begin(), 0);
		EATEST_VERIFY(it == intArray.begin());

		it = binary_search_i(intArray.begin(), intArray.begin() + 1, 0, less<int>());
		EATEST_VERIFY(it == intArray.begin());

		it = binary_search_i(intArray.begin(), intArray.end(), 733);
		EATEST_VERIFY(it == intArray.begin() + 733);


		list<TestObject> toList;
		list<TestObject>::iterator toI;
		for(int j = 0; j < 1000; j++)
			toList.push_back(TestObject(j));

		b = binary_search(toList.begin(), toList.begin(), TestObject(0), less<TestObject>());
		EATEST_VERIFY(b == false);

		toI = toList.begin();
		toI++;
		b = binary_search(toList.begin(), toI, TestObject(0));
		EATEST_VERIFY(b == true);

		b = binary_search(toList.begin(), toList.end(), TestObject(733));
		EATEST_VERIFY(b == true);


		toI = binary_search_i(toList.begin(), toList.begin(), TestObject(0), less<TestObject>()); // No-op
		EATEST_VERIFY(toI == toList.begin());

		toI = toList.begin();
		toI++;
		toI = binary_search_i(toList.begin(), toI, TestObject(0));
		EATEST_VERIFY(*toI == TestObject(0));

		toI = binary_search_i(toList.begin(), toList.end(), TestObject(733));
		EATEST_VERIFY(*toI == TestObject(733));
	}


	{
		// ForwardIterator unique(ForwardIterator first, ForwardIterator last)
		// ForwardIterator unique(ForwardIterator first, ForwardIterator last, BinaryPredicate predicate)

		int intArray[] = { 1, 2, 3, 3, 4, 4 };

		int* pInt = unique(intArray, intArray + 0);
		EATEST_VERIFY(pInt == intArray);
		EATEST_VERIFY(VerifySequence(intArray, intArray + 6, int(), "unique", 1, 2, 3, 3, 4, 4, -1));

		pInt = unique(intArray, intArray + 6, equal_to<int>());
		EATEST_VERIFY(pInt == intArray + 4);
		EATEST_VERIFY(VerifySequence(intArray, intArray + 6, int(), "unique", 1, 2, 3, 4, 4, 4, -1));


		TestObject toArray[] = { TestObject(1), TestObject(2), TestObject(3), TestObject(3), TestObject(4), TestObject(4) };

		TestObject* pTO = unique(toArray, toArray + 6);
		EATEST_VERIFY(pTO == toArray + 4);
		EATEST_VERIFY(toArray[3] == TestObject(4));
	}


	{
		// ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2)
		// ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate predicate)

		// Test via bidirectional/random_access iterator.
		basic_string<char> sTest("abcdefg abcdefg abcdefg");
		const char* pSubstring1 = "abcd";
		const char* pSubstring2 = "1234";

		basic_string<char>::iterator iString = find_end(sTest.begin(), sTest.end(), pSubstring1, pSubstring1 + 4);
		EATEST_VERIFY(&*iString == &sTest[16]);

		iString = find_end(sTest.begin(), sTest.end(), pSubstring1, pSubstring1 + 4, equal_to<char>());
		EATEST_VERIFY(&*iString == &sTest[16]);

		iString = find_end(sTest.begin(), sTest.end(), pSubstring2, pSubstring2 + strlen(pSubstring2));
		EATEST_VERIFY(iString == sTest.end());

		iString = find_end(sTest.begin(), sTest.end(), pSubstring2, pSubstring2 + strlen(pSubstring2), equal_to<char>());
		EATEST_VERIFY(iString == sTest.end());

		// Test via forward iterator.
		slist<char> sListTest;
		for(slist<char>::size_type i = sTest.size(); i > 0; --i)
			sListTest.push_front(sTest[i - 1]);

		slist<char>::iterator iSlist = find_end(sListTest.begin(), sListTest.end(), pSubstring1, pSubstring1 + strlen(pSubstring1));
		slist<char>::iterator i16 = sListTest.begin();
		advance(i16, 16);
		EATEST_VERIFY(iSlist == i16);

		iSlist = find_end(sListTest.begin(), sListTest.end(), pSubstring1, pSubstring1 + strlen(pSubstring1), equal_to<char>());
		i16 = sListTest.begin();
		advance(i16, 16);
		EATEST_VERIFY(iSlist == i16);

		iSlist = find_end(sListTest.begin(), sListTest.end(), pSubstring2, pSubstring2 + strlen(pSubstring2));
		EATEST_VERIFY(iSlist == sListTest.end());

		iSlist = find_end(sListTest.begin(), sListTest.end(), pSubstring2, pSubstring2 + strlen(pSubstring2), equal_to<char>());
		EATEST_VERIFY(iSlist == sListTest.end());
	}


	{
		// OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result)
		// OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2,  OutputIterator result, Compare compare)

		int intArray1[] = { 0, 0, 2, 5, 8, 8, 12, 24, 26, 43 };
		int intArray2[] = { 0, 0, 0, 5, 7, 8, 11, 24, 25, 43 };
		int intArray3[] = { 9, 9, 9, 9, 9, 9,  9,  9,  9,  9 };

		set_difference(intArray1, intArray1 + 0, intArray2, intArray2 + 0, intArray3);
		EATEST_VERIFY(VerifySequence(intArray3, intArray3 + 10, int(), "set_difference", 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, -1));

		set_difference(intArray1, intArray1 + 10, intArray2, intArray2 + 10, intArray3);
		EATEST_VERIFY(VerifySequence(intArray3, intArray3 + 10, int(), "set_difference", 2, 8, 12, 26, 9, 9, 9, 9, 9, 9, -1));

		intArray3[0] = intArray3[1] = intArray3[2] = 9;

		set_difference(intArray1, intArray1 + 10, intArray2, intArray2 + 10, intArray3, less<int>());
		EATEST_VERIFY(VerifySequence(intArray3, intArray3 + 10, int(), "set_difference", 2, 8, 12, 26, 9, 9, 9, 9, 9, 9, -1));
	}


	{
		// OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result)
		// OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2,  OutputIterator result, Compare compare)

		int intArray1[] = { 0, 0, 2, 5, 8, 8, 12, 24, 26, 43 };
		int intArray2[] = { 0, 0, 0, 5, 7, 8, 11, 24, 25, 43 };
		int intArray3[] = { 9, 9, 9, 9, 9, 9,  9,  9,  9,  9 };

		set_symmetric_difference(intArray1, intArray1 + 0, intArray2, intArray2 + 0, intArray3);
		EATEST_VERIFY(VerifySequence(intArray3, intArray3 + 10, int(), "set_symmetric_difference", 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, -1));

		set_symmetric_difference(intArray1, intArray1 + 10, intArray2, intArray2 + 10, intArray3);
		EATEST_VERIFY(VerifySequence(intArray3, intArray3 + 10, int(), "set_symmetric_difference", 0, 2, 7, 8, 11, 12, 25, 26, 9, 9, -1));

		intArray3[0] = intArray3[1] = intArray3[2] = intArray3[4] = intArray3[5] = intArray3[6] = 9;

		set_symmetric_difference(intArray1, intArray1 + 10, intArray2, intArray2 + 10, intArray3, less<int>());
		EATEST_VERIFY(VerifySequence(intArray3, intArray3 + 10, int(), "set_symmetric_difference", 0, 2, 7, 8, 11, 12, 25, 26, 9, 9, -1));
	}


	{
		// OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result)
		// OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare compare)

		int intArray1[] = { 0, 0, 2, 5, 8, 8, 12, 24, 26, 43 };
		int intArray2[] = { 0, 0, 0, 5, 7, 8, 11, 24, 25, 43 };
		int intArray3[] = { 9, 9, 9, 9, 9, 9,  9,  9,  9,  9 };

		set_intersection(intArray1, intArray1 + 0, intArray2, intArray2 + 0, intArray3);
		EATEST_VERIFY(VerifySequence(intArray3, intArray3 + 10, int(), "set_intersection", 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, -1));

		set_intersection(intArray1, intArray1 + 10, intArray2, intArray2 + 10, intArray3);
		EATEST_VERIFY(VerifySequence(intArray3, intArray3 + 10, int(), "set_intersection", 0, 0, 5, 8, 24, 43, 9, 9, 9, 9, -1));

		intArray3[0] = intArray3[1] = intArray3[2] = intArray3[4] = intArray3[5] = intArray3[6] = 9;

		set_intersection(intArray1, intArray1 + 10, intArray2, intArray2 + 10, intArray3, less<int>());
		EATEST_VERIFY(VerifySequence(intArray3, intArray3 + 10, int(), "set_intersection", 0, 0, 5, 8, 24, 43, 9, 9, 9, 9, -1));
	}


	{
		// OutputIterator set_union(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result)
		// OutputIterator set_union(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare compare)

		int intArray1[] = { 0, 0, 2, 5, 8, 8, 12, 24, 26, 43 };
		int intArray2[] = { 0, 0, 0, 5, 7, 8, 11, 24, 25, 43 };
		int intArray3[] = { 9, 9, 9, 9, 9, 9,  9,  9,  9,  9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9 };

		set_union(intArray1, intArray1 + 0, intArray2, intArray2 + 0, intArray3);
		EATEST_VERIFY(VerifySequence(intArray3, intArray3 + 20, int(), "set_union", 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, -1));

		set_union(intArray1, intArray1 + 10, intArray2, intArray2 + 10, intArray3);
		EATEST_VERIFY(VerifySequence(intArray3, intArray3 + 20, int(), "set_union", 0, 0, 0, 2, 5, 7, 8, 8, 11, 12, 24, 25, 26, 43, 9, 9, 9, 9, 9, 9, -1));

		intArray3[0] = intArray3[1] = intArray3[2] = intArray3[3] = intArray3[4] = intArray3[5] = intArray3[6] = intArray3[7] = intArray3[8] = intArray3[9] = intArray3[10] = intArray3[11] = 9;

		set_union(intArray1, intArray1 + 10, intArray2, intArray2 + 10, intArray3, less<int>());
		EATEST_VERIFY(VerifySequence(intArray3, intArray3 + 20, int(), "set_union", 0, 0, 0, 2, 5, 7, 8, 8, 11, 12, 24, 25, 26, 43, 9, 9, 9, 9, 9, 9, -1));
	}


	// set_difference_2
	{
		// template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
		// void set_difference_2(InputIterator1 first1, InputIterator1 last1,
		//                       InputIterator2 first2, InputIterator2 last2,
		//                       OutputIterator result1, OutputIterator result2)
		{
			const eastl::vector<int> v1 = {1, 2, 4, 5,   7, 7, 9};
			const eastl::vector<int> v2 = {   2,       6,      9};
			eastl::vector<int> only_v1, only_v2;

			eastl::set_difference_2(v1.begin(), v1.end(), v2.begin(), v2.end(),
									eastl::inserter(only_v1, only_v1.begin()),
									eastl::inserter(only_v2, only_v2.begin()));

			EATEST_VERIFY((only_v1 == eastl::vector<int>{1, 4, 5, 7, 7}));
			EATEST_VERIFY((only_v2 == eastl::vector<int>{6}));
		}

        // template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename Compare>
        // void set_difference_2(InputIterator1 first1, InputIterator1 last1,
        //                       InputIterator2 first2, InputIterator2 last2,
        //                       OutputIterator result1, OutputIterator result2, Compare compare)
        {
			struct local
			{
				int data = -1;
				bool operator==(const local& other) const
					{ return data == other.data; }
			};

			const eastl::vector<local> v1 = {{1}, {2}, {4}, {5},      {7}, {7}, {9}};
			const eastl::vector<local> v2 = {     {2},           {6},           {9}};
			eastl::vector<local> only_v1, only_v2;

			eastl::set_difference_2(v1.begin(), v1.end(), v2.begin(), v2.end(),
									eastl::inserter(only_v1, only_v1.begin()),
									eastl::inserter(only_v2, only_v2.begin()),
									[](const local& lhs, const local& rhs) { return lhs.data < rhs.data; });

			EATEST_VERIFY((only_v1 == eastl::vector<local>{{1}, {4}, {5}, {7}, {7}}));
			EATEST_VERIFY((only_v2 == eastl::vector<local>{{6}}));
		}
	}


	// set_decomposition
	{
		// OutputIterator3 set_decomposition(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2,
		//                           OutputIterator1 result1, OutputIterator2 result2, OutputIterator3 result3)
		{
			const eastl::vector<int> v1 = {1, 2, 4, 5,   7, 7, 9};
			const eastl::vector<int> v2 = {   2,       6,      9};
			eastl::vector<int> only_v1, only_v2, intersection;

			eastl::set_decomposition(v1.begin(), v1.end(), v2.begin(), v2.end(),
									eastl::inserter(only_v1, only_v1.begin()),
									eastl::inserter(only_v2, only_v2.begin()),
									eastl::inserter(intersection, intersection.begin()));

			EATEST_VERIFY((only_v1 == eastl::vector<int>{1, 4, 5, 7, 7}));
			EATEST_VERIFY((only_v2 == eastl::vector<int>{6}));
			EATEST_VERIFY((intersection == eastl::vector<int>{2, 9}));
		}

		// OutputIterator3 set_decomposition(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2,
		//                           OutputIterator1 result1, OutputIterator2 result2, OutputIterator3 result3, Compare compare)
		{
			struct local
			{
				int data = -1;
				bool operator==(const local& other) const
					{ return data == other.data; }
			};

			const eastl::vector<local> v1 = {{1}, {2}, {4}, {5},      {7}, {7}, {9}};
			const eastl::vector<local> v2 = {     {2},           {6},           {9}};
			eastl::vector<local> only_v1, only_v2, intersection;

			eastl::set_decomposition(v1.begin(), v1.end(), v2.begin(), v2.end(),
									eastl::inserter(only_v1, only_v1.begin()),
									eastl::inserter(only_v2, only_v2.begin()),
									eastl::inserter(intersection, intersection.begin()),
									[](const local& lhs, const local& rhs) { return lhs.data < rhs.data; });

			EATEST_VERIFY((only_v1 == eastl::vector<local>{{1}, {4}, {5}, {7}, {7}}));
			EATEST_VERIFY((only_v2 == eastl::vector<local>{{6}}));
			EATEST_VERIFY((intersection == eastl::vector<local>{{2}, {9}}));
		}
	}

    {
		// template<typename ForwardIterator1, typename ForwardIterator2>
		// bool is_permutation(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2)

		// template<typename ForwardIterator1, typename ForwardIterator2, class BinaryPredicate>
		// bool is_permutation(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, BinaryPredicate predicate)
		EASTLTest_Rand eastlRNG(EA::UnitTest::GetRandSeed());

		{
			int intArray1[] = { 0, 1, 2, 3, 4 };
			int intArray2[] = { 0, 1, 2, 3, 4 };

			// Test an empty set.
			EATEST_VERIFY(eastl::is_permutation(intArray1, intArray1 + 0, intArray2));

			// Test two identical sets.
			EATEST_VERIFY(eastl::is_permutation(intArray1, intArray1 + EAArrayCount(intArray1), intArray2));
			eastl::random_shuffle(intArray1, intArray1 + EAArrayCount(intArray1), eastlRNG);

			// Test order randomization.
			EATEST_VERIFY(eastl::is_permutation(intArray1, intArray1 + EAArrayCount(intArray1), intArray2));
			eastl::random_shuffle(intArray2, intArray2 + EAArrayCount(intArray2), eastlRNG);
			EATEST_VERIFY(eastl::is_permutation(intArray1, intArray1 + EAArrayCount(intArray1), intArray2));

			// Test the case where there's a difference.
			intArray2[4] = intArray2[3]; // This change guarantees is_permutation will return false.
			EATEST_VERIFY(!eastl::is_permutation(intArray1, intArray1 + EAArrayCount(intArray1), intArray2));
		}

		{
			int intArray1[] = { 0, 0, 0, 1, 1 };
			int intArray2[] = { 0, 0, 0, 1, 1 };

			// Test two identical sets.
			EATEST_VERIFY(eastl::is_permutation(intArray1, intArray1 + EAArrayCount(intArray1), intArray2));
			eastl::random_shuffle(intArray1, intArray1 + EAArrayCount(intArray1), eastlRNG);

			// Test order randomization.
			EATEST_VERIFY(eastl::is_permutation(intArray1, intArray1 + EAArrayCount(intArray1), intArray2));
			eastl::random_shuffle(intArray2, intArray2 + EAArrayCount(intArray2), eastlRNG);
			EATEST_VERIFY(eastl::is_permutation(intArray1, intArray1 + EAArrayCount(intArray1), intArray2));

			// Test the case where there's a difference.
			intArray2[4] = (intArray2[4] == 0) ? 1 : 0;
			EATEST_VERIFY(!eastl::is_permutation(intArray1, intArray1 + EAArrayCount(intArray1), intArray2));
		}

		for(int n = 0; n < 100000; n++)
		{
			eastl_size_t intArray1[6];
			eastl_size_t intArray2[6];

			for(size_t i = 0; i < EAArrayCount(intArray1); i++)
			{
				intArray1[i] = eastlRNG.RandLimit(6);
				intArray2[i] = eastlRNG.RandLimit(6);
			}

			bool isPermutation = eastl::is_permutation(intArray1, intArray1 + EAArrayCount(intArray1), intArray2);

			// If is_permutation returned true, then sorted versions of the two arrays should be identical.
			eastl::sort(intArray1, intArray1 + EAArrayCount(intArray1));
			eastl::sort(intArray2, intArray2 + EAArrayCount(intArray2));

			eastl::pair<eastl_size_t*, eastl_size_t*> mismatchResult = eastl::mismatch(intArray1, intArray1 + EAArrayCount(intArray1), intArray2);
			bool isIdentical = (mismatchResult.first == (intArray1 + EAArrayCount(intArray1)));

			EATEST_VERIFY(isPermutation == isIdentical); // With an array size of 6, isPermutation ends up being true about 1 in 400 times here.
		}
	}

    {
	    // template <class InputIterator, class UnaryPredicate>
	    // bool is_partitioned(InputIterator first, InputIterator last, UnaryPredicate predicate)

	    // template <class ForwardIterator, class UnaryPredicate>
	    // ForwardIterator partition_point(ForwardIterator first, ForwardIterator last, UnaryPredicate predicate)

	    const auto isEven = [](int i) { return i % 2 == 0; };

	    // These are all partitioned, even first and then odd.
	    vector<int> v1 = {0, 2, 4, 5, 7, 9, 11};
	    vector<int> v2 = {1, 3, 5, 7, 9};
	    vector<int> v3 = {2, 4, 8, 100, 102};
	    vector<int> v4 = {2, 4, 8, 100, 103};
	    EATEST_VERIFY(is_partitioned(v1.begin(), v1.end(), isEven));
	    EATEST_VERIFY(is_partitioned(v2.begin(), v2.end(), isEven));
	    EATEST_VERIFY(is_partitioned(v3.begin(), v3.end(), isEven));
	    EATEST_VERIFY(is_partitioned(v4.begin(), v4.end(), isEven));

	    EATEST_VERIFY(distance(v1.begin(), partition_point(v1.begin(), v1.end(), isEven)) == 3);
	    EATEST_VERIFY(distance(v2.begin(), partition_point(v2.begin(), v2.end(), isEven)) == 0);
	    EATEST_VERIFY(distance(v3.begin(), partition_point(v3.begin(), v3.end(), isEven)) == 5);
	    EATEST_VERIFY(distance(v4.begin(), partition_point(v4.begin(), v4.end(), isEven)) == 4);

	    // These are all not partitioned:
	    vector<int> v5 = {0, 2, 3, 4, 5, 7, 9, 11};
	    vector<int> v6 = {1, 3, 5, 7, 9, 2};
	    vector<int> v7 = {2, 4, 3, 8, 100, 102};
	    vector<int> v8 = {2, 4, 8, 5, 100, 103};
	    EATEST_VERIFY(!is_partitioned(v5.begin(), v5.end(), isEven));
	    EATEST_VERIFY(!is_partitioned(v6.begin(), v6.end(), isEven));
	    EATEST_VERIFY(!is_partitioned(v7.begin(), v7.end(), isEven));
	    EATEST_VERIFY(!is_partitioned(v8.begin(), v8.end(), isEven));
    }

    {
		//template<typename BidirectionalIterator>
		//bool next_permutation(BidirectionalIterator first, BidirectionalIterator last);

		//template<typename BidirectionalIterator, typename Compare>
		//bool next_permutation(BidirectionalIterator first, BidirectionalIterator last, Compare compare);

		uint64_t    count;
		vector<int> intArray;
		for(int i = 0; i < 8; i++)
			intArray.push_back(i);

		count = 0;
		do {
			++count;
		} while(next_permutation(intArray.begin(), intArray.end()));
		EATEST_VERIFY(count == 40320); // count = n!
		EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));

		count = 0;
		do {
			++count;
		} while(next_permutation(intArray.begin(), intArray.end(), eastl::less<int>()));
		EATEST_VERIFY(count == 40320); // count = n!
		EATEST_VERIFY(is_sorted(intArray.begin(), intArray.end()));
	}


	{
		// template <typename ForwardIterator>
		// ForwardIterator rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last);

		// eastl::array (ContiguousIterator/Pointer)
		const eastl_size_t kRotateArraySize = 10;
		typedef eastl::array<int, kRotateArraySize> IntArray;

		{   // This type is templated, so we can't run a loop over various sizes.
			IntArray intArray;

			for(eastl_size_t i = 0; i < kRotateArraySize; i++)
			{
				eastl::generate_n(intArray.begin(), kRotateArraySize, GenerateIncrementalIntegers<int>());
				IntArray::iterator intArrayItMiddle = eastl::next(intArray.begin(), i);
				IntArray::iterator intArrayIt = eastl::rotate(intArray.begin(), intArrayItMiddle, intArray.end());

				for(eastl_size_t j = 0; j < kRotateArraySize; j++)
				{
					if(intArrayIt == intArray.end())
						intArrayIt = intArray.begin();
					EATEST_VERIFY(*intArrayIt++ == (int)j);
				}
			}
		}

		// eastl::vector (ContiguousIterator)
		typedef eastl::vector<int> IntVector;

		for(eastl_size_t s = 10; s < 500; s += (eastl_size_t)rng.RandRange(50, 100))
		{
			IntVector intVector(s, 0);

			for(eastl_size_t i = 0; i < s; i++)
			{
				eastl::generate_n(intVector.begin(), s, GenerateIncrementalIntegers<int>());
				IntVector::iterator intVectorItMiddle = eastl::next(intVector.begin(), i);
				IntVector::iterator intVectorIt = eastl::rotate(intVector.begin(), intVectorItMiddle, intVector.end());

				for(eastl_size_t j = 0; j < s; j++)
				{
					if(intVectorIt == intVector.end())
						intVectorIt = intVector.begin();
					EATEST_VERIFY(*intVectorIt++ == (int)j);
				}
			}
		}

		// eastl::deque (RandomAccessIterator)
		typedef eastl::deque<int> IntDeque;

		for(eastl_size_t s = 10; s < 500; s += (eastl_size_t)rng.RandRange(50, 100))
		{
			IntDeque intDeque(s, 0);

			for(eastl_size_t i = 0; i < s; i++)
			{
				eastl::generate_n(intDeque.begin(), s, GenerateIncrementalIntegers<int>());
				IntDeque::iterator intDequeItMiddle = eastl::next(intDeque.begin(), i);
				IntDeque::iterator intDequeIt = eastl::rotate(intDeque.begin(), intDequeItMiddle, intDeque.end());

				for(eastl_size_t j = 0; j < s; j++)
				{
					if(intDequeIt == intDeque.end())
						intDequeIt = intDeque.begin();
					EATEST_VERIFY(*intDequeIt++ == (int)j);
				}
			}
		}

		// eastl::list (BidirectionalIterator)
		typedef eastl::list<int> IntList;

		for(eastl_size_t s = 10; s < 500; s += (eastl_size_t)rng.RandRange(50, 100))
		{
			IntList intList(s, 0);

			for(eastl_size_t i = 0; i < s; i++)
			{
				eastl::generate_n(intList.begin(), s, GenerateIncrementalIntegers<int>());
				IntList::iterator intListItMiddle = eastl::next(intList.begin(), i);
				IntList::iterator intListIt = eastl::rotate(intList.begin(), intListItMiddle, intList.end());

				for(eastl_size_t j = 0; j < s; j++)
				{
					if(intListIt == intList.end())
						intListIt = intList.begin();
					EATEST_VERIFY(*intListIt++ == (int)j);
				}
			}
		}

		// eastl::slist (ForwardIterator)
		typedef eastl::slist<int> IntSlist;

		for(eastl_size_t s = 10; s < 500; s += (eastl_size_t)rng.RandRange(50, 100))
		{
			IntSlist intSlist(s, 0);

			for(eastl_size_t i = 0; i < s; i++)
			{
				eastl::generate_n(intSlist.begin(), s, GenerateIncrementalIntegers<int>());
				IntSlist::iterator intSlistItMiddle = eastl::next(intSlist.begin(), i);
				IntSlist::iterator intSlistIt = eastl::rotate(intSlist.begin(), intSlistItMiddle, intSlist.end());

				for(eastl_size_t j = 0; j < s; j++)
				{
					if(intSlistIt == intSlist.end())
						intSlistIt = intSlist.begin();
					EATEST_VERIFY(*intSlistIt++ == (int)j);
				}
			}
		}
	}

	// test eastl::sort with move-only type
	{
		{
			eastl::vector<eastl::unique_ptr<int>> vec;
			eastl::sort(vec.begin(), vec.end(), [](const eastl::unique_ptr<int>& lhs, const eastl::unique_ptr<int>& rhs) { return *lhs < *rhs; });
		}
		{
			eastl::vector<eastl::unique_ptr<int>> vec;
			eastl::sort(vec.begin(), vec.end());
		}
		{
			eastl::vector<MissingMoveConstructor> vec;
			eastl::sort(vec.begin(), vec.end(), [](const MissingMoveConstructor& lhs, const MissingMoveConstructor& rhs) { return lhs < rhs; });
		}
		{
			eastl::vector<MissingMoveConstructor> vec;
			eastl::sort(vec.begin(), vec.end());
		}
		{
			eastl::vector<MissingMoveAssignable> vec;
			eastl::sort(vec.begin(), vec.end(), [](const MissingMoveAssignable& lhs, const MissingMoveAssignable& rhs) { return lhs < rhs; });
		}
		{
			eastl::vector<MissingMoveAssignable> vec;
			eastl::sort(vec.begin(), vec.end());
		}
		{
			eastl::vector<eastl::unique_ptr<int>> vec;
			vec.emplace_back(new int(7));
			vec.emplace_back(new int(-42));
			vec.emplace_back(new int(5));
			eastl::sort(vec.begin(), vec.end(),  [](const eastl::unique_ptr<int>& lhs, const eastl::unique_ptr<int>& rhs) { return *lhs < *rhs; });
			EATEST_VERIFY(*vec[0] == -42);
			EATEST_VERIFY(*vec[1] == 5);
			EATEST_VERIFY(*vec[2] == 7);
		}
		{
			for (unsigned tests = 0; tests < 50; ++tests)
			{
				eastl::vector<eastl::unique_ptr<int>> vec1;

				for (int i = 0; i < 100; ++i)
				{
					int randomNumber = rng();
					vec1.emplace_back(new int(randomNumber));
				}

				auto vec1Cmp = [](const eastl::unique_ptr<int>& lhs, const eastl::unique_ptr<int>& rhs) { return *lhs < *rhs; };
				eastl::sort(vec1.begin(), vec1.end(), vec1Cmp);
				EATEST_VERIFY(eastl::is_sorted(vec1.begin(), vec1.end(), vec1Cmp));
			}
		}
	}

	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();

	return nErrorCount;
}
