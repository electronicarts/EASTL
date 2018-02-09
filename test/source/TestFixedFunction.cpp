/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <EAAssert/eaassert.h>
#include "EASTLTest.h"
#include <EASTL/fixed_function.h>
#include <EASTL/numeric.h>

///////////////////////////////////////////////////////////////////////////////
// TestFixedFunctionCaptureless
//
// Tests calling a captureless (eg. function pointer) callable with variable 
// eastl::fixed_function size types.
//
template<class FixedFunctionT>
int TestFixedFunctionCaptureless()
{
	int nErrorCount = 0;

	FixedFunctionT fn; 

	EATEST_VERIFY(!fn);

	fn =  [](int in) { return in; };

	EATEST_VERIFY(fn);

	EATEST_VERIFY(fn(42) == 42);

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestFunctional
//
int TestFixedFunction()
{
	using namespace eastl;

	int nErrorCount = 0;

	{
		using ff_0 = eastl::fixed_function<0, int(int)>;
		using ff_1 = eastl::fixed_function<1, int(int)>;
		using ff_4 = eastl::fixed_function<4, int(int)>;
		using ff_8 = eastl::fixed_function<8, int(int)>;
		using ff_64 = eastl::fixed_function<64, int(int)>;
		using ff_128 = eastl::fixed_function<128, int(int)>;
		using ff_4096 = eastl::fixed_function<4096, int(int)>;

		static_assert(sizeof(ff_0) >= sizeof(void*), "error");
		static_assert(sizeof(ff_1) >= sizeof(void*), "error");
		static_assert(sizeof(ff_4) >= sizeof(void*), "error");
		static_assert(sizeof(ff_8) >= 8, "error");
		static_assert(sizeof(ff_64) >= 64, "error");
		static_assert(sizeof(ff_128) >= 128, "error");
		static_assert(sizeof(ff_4096) >= 4096, "error");

		nErrorCount += TestFixedFunctionCaptureless<ff_0>();
		nErrorCount += TestFixedFunctionCaptureless<ff_1>();
		nErrorCount += TestFixedFunctionCaptureless<ff_4>();
		nErrorCount += TestFixedFunctionCaptureless<ff_8>();
		nErrorCount += TestFixedFunctionCaptureless<ff_64>();
		nErrorCount += TestFixedFunctionCaptureless<ff_128>();
		nErrorCount += TestFixedFunctionCaptureless<ff_4096>();
	}

	{
		struct Functor { int operator()() { return 42; } };
		fixed_function<0, int(void)> fn = Functor();
		EATEST_VERIFY(fn() == 42);
	}

	{
		struct Functor { int operator()(int in) { return in; } };
		fixed_function<0, int(int)> fn = Functor();
		EATEST_VERIFY(fn(24) == 24);
	}

	{
		eastl::fixed_function<0, void(void)> fn; 

		EATEST_VERIFY(!fn);
		fn =  [] {};
		EATEST_VERIFY(fn);
	}

	{
		eastl::fixed_function<0, int(int)> fn = [](int param) { return param; };
		EATEST_VERIFY(fn(42) == 42);
	}

	{
		eastl::fixed_function<0, int(int)> fn = ReturnVal;
		EATEST_VERIFY(fn(42) == 42);
	}

	{
		eastl::fixed_function<0, int()> fn0 = ReturnZero;
		eastl::fixed_function<0, int()> fn1 = ReturnOne;

		EATEST_VERIFY(fn0() == 0 && fn1() == 1);
		swap(fn0, fn1);
		EATEST_VERIFY(fn0() == 1 && fn1() == 0);
	}

	{
		eastl::fixed_function<0, int()> fn0 = ReturnZero;
		eastl::fixed_function<0, int()> fn1 = ReturnOne;

		EATEST_VERIFY(fn0() == 0 && fn1() == 1);
		fn0 = fn1;
		EATEST_VERIFY(fn0() == 1 && fn1() == 1);
	}

	{
		eastl::fixed_function<0, int()> fn0 = ReturnZero;
		eastl::fixed_function<0, int()> fn1 = ReturnOne;

		EATEST_VERIFY(fn0() == 0 && fn1() == 1);
		fn0 = eastl::move(fn1);
		EATEST_VERIFY(fn0() == 1 && fn1 == nullptr);
	}

	{
		eastl::fixed_function<0, int(int)> f1(nullptr);
		EATEST_VERIFY(!f1);

		eastl::fixed_function<0, int(int)> f2 = nullptr;
		EATEST_VERIFY(!f2);
	}

	{
		// test using a large lambda capture
		uint64_t a = 1, b = 2, c = 3, d = 4, e = 5, f = 6;
		auto large_add = [=] { return a + b + c + d + e + f; };

		{
			eastl::fixed_function<48, uint64_t(void)> fn = large_add;
			auto result = fn();
			EATEST_VERIFY(result == 21);
		}

		{
			eastl::fixed_function<sizeof(large_add), uint64_t(void)> fn = large_add;
			auto result = fn();
			EATEST_VERIFY(result == 21);
		}
	}

	return nErrorCount;
}


