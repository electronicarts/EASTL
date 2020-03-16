/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EABase/eabase.h>
#include <EAAssert/eaassert.h>

// Included prior to EASTLTest.h to guard against the following bug resurfacing:
// https://github.com/electronicarts/EASTL/issues/275
#include <EASTL/fixed_function.h>

#include "EASTLTest.h"
#include <EASTL/numeric.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <functional>
EA_RESTORE_ALL_VC_WARNINGS()


///////////////////////////////////////////////////////////////////////////////
// TestFixedFunctionDtor
//
int TestFixedFunctionDtor()
{
	using namespace eastl;

	int nErrorCount = 0;

	{
		TestObject to;
		TestObject::Reset();
		{
			eastl::fixed_function<sizeof(TestObject), void(void)> ff = [to] {};
			ff();
		}
		VERIFY(TestObject::IsClear());
	}

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestFixedFunctionStdBind
//
int TestFixedFunctionStdBind()
{
	using namespace eastl;

	int nErrorCount = 0;
	int val = 0;

	{
		TestObject to;
		auto lambda = [to, &val] { ++val; };
		TestObject::Reset();
		{
			eastl::fixed_function<64, void(void)> ff = std::bind(lambda);
			ff();
		}
		VERIFY(TestObject::IsClear());
		VERIFY(val == 1);
	}
	{
		TestObject to;
		auto lambda = [to, &val] { ++val; };
		TestObject::Reset();
		{
			eastl::fixed_function<64, void(void)> ff = nullptr;
		    ff = std::bind(lambda);
			ff();
		}
		VERIFY(TestObject::IsClear());
		VERIFY(val == 2);
	}

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestFixedFunctionReferenceWrapper
//
int TestFixedFunctionReferenceWrapper()
{
	using namespace eastl;

	int nErrorCount = 0;
	int val = 0;

	{
		TestObject to;
		auto lambda = [to, &val] { ++val; };
		TestObject::Reset();
		{
			eastl::fixed_function<sizeof(eastl::reference_wrapper<decltype(lambda)>), void(void)> ff = eastl::reference_wrapper<decltype(lambda)>(lambda);
			ff();
		}
		VERIFY(TestObject::IsClear());
		VERIFY(val == 1);
	}
	{
		TestObject to;
		auto lambda = [to, &val] { ++val; };
		TestObject::Reset();
		{
			eastl::fixed_function<sizeof(eastl::reference_wrapper<decltype(lambda)>), void(void)> ff = nullptr;
		    ff = eastl::reference_wrapper<decltype(lambda)>(lambda);
			ff();
		}
		VERIFY(TestObject::IsClear());
		VERIFY(val == 2);
	}

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestFixedFunctionFunctionPointer
//

static void TestVoidRet(int* p)
{
	*p += 1;
}

static int TestIntRet(int* p)
{
	int ret = *p;
	*p += 1;
	return ret;
}

int TestFixedFunctionFunctionPointer()
{
	using namespace eastl;

	typedef int (*FuncPtrInt)(int*);
	typedef void (*FuncPtrVoid)(int*);

	int nErrorCount = 0;
	int val = 0;

	{
		eastl::fixed_function<sizeof(FuncPtrVoid), void(int*)> ff = &TestVoidRet;
		ff(&val);
		VERIFY(val == 1);
	}
	{
		eastl::fixed_function<sizeof(FuncPtrVoid), void(int*)> ff;
		ff = &TestVoidRet;
		ff(&val);
		VERIFY(val == 2);
	}
	{
		eastl::fixed_function<sizeof(FuncPtrInt), int(int*)> ff = &TestIntRet;
		int ret = ff(&val);
		VERIFY(ret == 2);
		VERIFY(val == 3);
	}
	{
		eastl::fixed_function<sizeof(FuncPtrInt), int(int*)> ff;
		ff = &TestIntRet;
		int ret = ff(&val);
		VERIFY(ret == 3);
		VERIFY(val == 4);
	}

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestFixedFunctionPointerToMemberFunction
//

int TestFixedFunctionPointerToMemberFunction()
{
	using namespace eastl;

	struct TestVoidRet
	{
		TestVoidRet() : x(0) {}
		~TestVoidRet() = default;

		void IncX() const
		{
			++x;
		}

		void IncX()
		{
			++x;
		}

		mutable int x = 0;
	};

	struct TestIntRet
	{
		TestIntRet() : x(0) {}

		int IncX() const
		{
			return x++;
		}

		int IncX()
		{
			return x++;
		}

		mutable int x = 0;
	};

	int nErrorCount = 0;
	TestVoidRet voidRet;
	TestIntRet intRet;
	const TestVoidRet cvoidRet;
	const TestIntRet cintRet;

	typedef void (TestVoidRet::*PTMFSize)(void);

	{
		eastl::fixed_function<sizeof(PTMFSize), void(const TestVoidRet&)> ff = static_cast<void(TestVoidRet::*)() const>(&TestVoidRet::IncX);
		ff(cvoidRet);
		VERIFY(cvoidRet.x == 1);
	}
	{
		eastl::fixed_function<sizeof(PTMFSize), void(const TestVoidRet&)> ff = static_cast<void(TestVoidRet::*)() const>(&TestVoidRet::IncX);
		ff(voidRet);
		VERIFY(voidRet.x == 1);
	}
	{
		eastl::fixed_function<sizeof(PTMFSize), void(TestVoidRet&)> ff = static_cast<void(TestVoidRet::*)()>(&TestVoidRet::IncX);
		ff(voidRet);
		VERIFY(voidRet.x == 2);
	}

	{
		eastl::fixed_function<sizeof(PTMFSize), int(const TestIntRet&)> ff = static_cast<int(TestIntRet::*)() const>(&TestIntRet::IncX);
		int ret = ff(cintRet);
		VERIFY(ret == 0);
		VERIFY(cintRet.x == 1);
	}
	{
		eastl::fixed_function<sizeof(PTMFSize), int(const TestIntRet&)> ff = static_cast<int(TestIntRet::*)() const>(&TestIntRet::IncX);
		int ret = ff(intRet);
		VERIFY(ret == 0);
		VERIFY(intRet.x == 1);
	}
	{
		eastl::fixed_function<sizeof(PTMFSize), int(TestIntRet&)> ff = static_cast<int(TestIntRet::*)()>(&TestIntRet::IncX);
		int ret = ff(intRet);
		VERIFY(ret == 1);
		VERIFY(intRet.x == 2);
	}

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestFixedFunctionPointerToMemberData
//

int TestFixedFunctionPointerToMemberData()
{
	using namespace eastl;

	struct Test
	{
		Test() : x(1) {}
		int x = 1;
	};

	int nErrorCount = 0;

	Test t;
	const Test ct;

	{
		eastl::fixed_function<sizeof(void*), int(const Test&)> ff = &Test::x;
		int ret = ff(t);
		VERIFY(ret == 1);
	}
	{
		eastl::fixed_function<sizeof(void*), int(const Test&)> ff = &Test::x;
		int ret = ff(ct);
		VERIFY(ret == 1);
	}
	{
		eastl::fixed_function<sizeof(void*), int(const Test&)> ff;
		ff = &Test::x;
		int ret = ff(t);
		VERIFY(ret == 1);
	}
	{
		eastl::fixed_function<sizeof(void*), int(const Test&)> ff;
		ff = &Test::x;
		int ret = ff(ct);
		VERIFY(ret == 1);
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestFixedFunctionExistingClosure
//
int TestFixedFunctionExistingClosure()
{
	using namespace eastl;

	int nErrorCount = 0;

	{
		TestObject to;
		{
			using ff_t = eastl::fixed_function<sizeof(TestObject), void(void)>;
			{
				ff_t ff1 = [to] {};
				ff_t  ff3 = [to] {};
				TestObject::Reset();
				{
					ff_t ff2 = ff1;
					ff2 = ff3;  // copy over function that holds existing closure state
				}
				VERIFY(TestObject::IsClear());
			}
			{
				ff_t ff1 = [to] {};
				TestObject::Reset();
				ff_t ff3 = [to] {};
				{
					ff_t ff2 = ff1;
					ff2 = eastl::move(ff3);  // copy over function that holds existing closure state
				}
				VERIFY(TestObject::IsClear());
			}
			{
				ff_t ff1 = [to] {};
				TestObject::Reset();
				{
					ff_t ff2 = ff1;
					ff2 = nullptr;
				}
				VERIFY(TestObject::IsClear());
			}
			{
				TestObject::Reset();
				ff_t ff1 = [to] {};
				{
					ff_t ff2 = eastl::move(ff1);
					ff2 = nullptr;
				}
				VERIFY(TestObject::IsClear());
			}
		}
	}

	return nErrorCount;
}


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

	EATEST_VERIFY(!!fn);

	EATEST_VERIFY(fn(42) == 42);

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestFixedFunctionBasic
//
int TestFixedFunctionBasic()
{
	using namespace eastl;

	int nErrorCount = 0;

	{
		struct Functor { void operator()() { return; } };
		fixed_function<24, void(void)> fn;
		fixed_function<24, void(void)> fn2 = nullptr;
		EATEST_VERIFY(!fn);
		EATEST_VERIFY(!fn2);
		EATEST_VERIFY(fn == nullptr);
		EATEST_VERIFY(fn2 == nullptr);
		EATEST_VERIFY(nullptr == fn);
		EATEST_VERIFY(nullptr == fn2);
		fn = Functor();
		fn2 = Functor();
		EATEST_VERIFY(!!fn);
		EATEST_VERIFY(!!fn2);
		EATEST_VERIFY(fn != nullptr);
		EATEST_VERIFY(fn2 != nullptr);
		EATEST_VERIFY(nullptr != fn);
		EATEST_VERIFY(nullptr != fn2);
		fn = nullptr;
		fn2 = fn;
		EATEST_VERIFY(!fn);
		EATEST_VERIFY(!fn2);
		EATEST_VERIFY(fn == nullptr);
		EATEST_VERIFY(fn2 == nullptr);
		EATEST_VERIFY(nullptr == fn);
		EATEST_VERIFY(nullptr == fn2);
	}

	{
		using eastl::swap;
		struct Functor { int operator()() { return 5; } };
		fixed_function<24, int(void)> fn = Functor();
		fixed_function<24, int(void)> fn2;
		EATEST_VERIFY(fn() == 5);
		EATEST_VERIFY(!fn2);
		fn.swap(fn2);
		EATEST_VERIFY(!fn);
		EATEST_VERIFY(fn2() == 5);
		swap(fn, fn2);
		EATEST_VERIFY(fn() == 5);
		EATEST_VERIFY(!fn2);
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
		EATEST_VERIFY(!!fn);
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

	// Verify conversions to fixed_function<N> for sizes greater or equal to the source size.
	{
		uint32_t v0 = 130480, v1 = 936780302;
		const uint32_t result = v0 + v1;

		eastl::fixed_function<8, uint32_t(void)> ff8 = [v0, v1]
			{ return v0 + v1; };

		{
			eastl::fixed_function<16, uint32_t(void)> ff16(ff8);
			VERIFY(result == ff16());
		}

		{
			eastl::fixed_function<16, uint32_t(void)> ff16 = ff8;
			VERIFY(result == ff16());
		}

		{
			eastl::fixed_function<16, uint32_t(void)> ff16; 
			ff16 = ff8;
			VERIFY(result == ff16());
		}

		{
			auto ff8Copy = ff8;
			eastl::fixed_function<16, uint32_t(void)> ff16(eastl::move(ff8Copy));
			VERIFY(result == ff16());
		}

		{
			auto ff8Copy = ff8;
			eastl::fixed_function<16, uint32_t(void)> ff16 = eastl::move(ff8Copy);
			VERIFY(result == ff16());
		}

		{
			auto ff8Copy = ff8;
			eastl::fixed_function<16, uint32_t(void)> ff16;
			ff16 = eastl::move(ff8Copy);
			VERIFY(result == ff16());
		}
	}

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestFunctional
//
int TestFixedFunction()
{
	using namespace eastl;

	int nErrorCount = 0;

	nErrorCount += TestFixedFunctionBasic();
	nErrorCount += TestFixedFunctionDtor();
	nErrorCount += TestFixedFunctionExistingClosure();
	nErrorCount += TestFixedFunctionReferenceWrapper();
	nErrorCount += TestFixedFunctionFunctionPointer();
	nErrorCount += TestFixedFunctionPointerToMemberFunction();
	nErrorCount += TestFixedFunctionPointerToMemberData();
	nErrorCount += TestFixedFunctionStdBind();

	return nErrorCount;
}
