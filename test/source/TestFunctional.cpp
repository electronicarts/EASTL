/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <EAAssert/eaassert.h>
#include "EASTLTest.h"
#include <EASTL/memory.h>
#include <EASTL/functional.h>
#include <EASTL/hash_set.h>
#include <EASTL/set.h>
#include <EASTL/list.h>
#include <EAStdC/EAString.h>


namespace
{
	// Used for str_less tests below.
	template <typename T>
	struct Results
	{
		const T* p1;
		const T* p2;
		bool     expectedResult; // The expected result of the expression (p1 < p2)
	};


	// Used for const_mem_fun_t below.
	struct X
	{
		X() { }
		void DoNothing() const { }
	};

	template <typename T>
	void foo(typename T::argument_type arg)
	{
		typename T::result_type (T::*pFunction)(typename T::argument_type) const = &T::operator();
		T t(&X::DoNothing);
		(t.*pFunction)(arg);
	}


	// Used for equal_to_2 tests below.
	struct N1{
		N1(int x) : mX(x) { }
		int mX;
	};

	struct N2{
		N2(int x) : mX(x) { }
		int mX;
	};

	bool operator==(const N1& n1, const N1& n1a){ return (n1.mX == n1a.mX); }
	bool operator==(const N1& n1, const N2& n2) { return (n1.mX == n2.mX); }
	bool operator==(const N2& n2, const N1& n1) { return (n2.mX == n1.mX); }

	bool operator!=(const N1& n1, const N1& n1a){ return (n1.mX != n1a.mX); }
	bool operator!=(const N1& n1, const N2& n2) { return (n1.mX != n2.mX); }
	bool operator!=(const N2& n2, const N1& n1) { return (n2.mX != n1.mX); }

	bool operator< (const N1& n1, const N1& n1a){ return (n1.mX  < n1a.mX); }
	bool operator< (const N1& n1, const N2& n2) { return (n1.mX  < n2.mX); }
	bool operator< (const N2& n2, const N1& n1) { return (n2.mX  < n1.mX); }


	// Used for mem_fun tests below.
	struct TestClass
	{
		mutable int mX;

		TestClass() : mX(37) { }

		void Increment()
		{
			mX++;
		}

		void IncrementConst() const
		{
			mX++;
		}

		int MultiplyBy(int x)
		{
			return mX * x;
		}

		int MultiplyByConst(int x) const
		{
			return mX * x;
		}
	};
}


// Template instantations.
// These tell the compiler to compile all the functions for the given class.
typedef eastl::basic_string<char8_t,  MallocAllocator> String8MA;
typedef eastl::basic_string<char16_t, MallocAllocator> String16MA;

template struct eastl::string_hash<String8MA>;
template struct eastl::string_hash<String16MA>;

template class eastl::hash_set<String8MA,  eastl::string_hash<String8MA> >;
template class eastl::hash_set<String16MA, eastl::string_hash<String16MA> >;


// Helper function for testing our default hash implementations for pod types which
// simply returns the static_cast<size_t> of the val passed in
template<typename T>
int TestHashHelper(T val)
{
	int nErrorCount = 0;

	EATEST_VERIFY(eastl::hash<T>()(val) == static_cast<size_t>(val));

	return nErrorCount;
}

int ReturnVal(int param) { return param; }
int ReturnZero() { return 0; }
int ReturnOne() { return 1; }

///////////////////////////////////////////////////////////////////////////////
// TestFunctional
//
int TestFunctional()
{
	using namespace eastl;

	EASTLTest_Printf("TestFunctional\n");

	int nErrorCount = 0;

	{
		// str_equal_to
		char p0[] = "";
		char p1[] = "hello";
		char p2[] = "world";
		char p3[] = "helllllo";
		char p4[] = "hello"; // Intentionally the same value as p1.

		// str_equal_to
		typedef hash_set<const char*, hash<const char*>, str_equal_to<const char*> > StringHashSet;
		StringHashSet shs;

		shs.insert(p1);
		shs.insert(p2);
		shs.insert(p3);

		StringHashSet::iterator it = shs.find(p0);
		EATEST_VERIFY(it == shs.end());

		it = shs.find(p1);
		EATEST_VERIFY(it != shs.end());

		it = shs.find(p2);
		EATEST_VERIFY(it != shs.end());

		it = shs.find(p4);
		EATEST_VERIFY(it != shs.end());
	}

	{
		// str_less<const char8_t*>
		Results<char8_t> results8[] = 
		{
			{      "",          "", false },
			{      "",         "a",  true },
			{      "a",         "", false },
			{      "a",        "a", false },
			{      "a",        "b",  true },
			{  "____a",    "____a", false },
			{  "____a",    "____b",  true },
			{  "____b",    "____a", false },
			{  "_\xff",       "_a", false },    // Test high values, which exercises the signed/unsiged comparison behavior.
			{     "_a",    "_\xff",  true }
		};

		str_less<const char8_t*> sl8;
		for(size_t i = 0; i < EAArrayCount(results8); i++)
		{
			// Verify that our test is in line with the strcmp function.
			bool bResult = (EA::StdC::Strcmp(results8[i].p1, results8[i].p2) < 0);
			EATEST_VERIFY_F(bResult == results8[i].expectedResult, "Strcmp failure, test %zu. Expected \"%s\" to be %sless than \"%s\"", i, results8[i].p1, results8[i].expectedResult ? "" : "not ", results8[i].p2);

			// Verify that str_less achieves the expected results.
			bResult = sl8(results8[i].p1, results8[i].p2);
			EATEST_VERIFY_F(bResult == results8[i].expectedResult, "str_less test failure, test %zu. Expected \"%s\" to be %sless than \"%s\"", i, results8[i].p1, results8[i].expectedResult ? "" : "not ", results8[i].p2);            
		}

		// str_less<const wchar_t*>
		Results<wchar_t> resultsW[] = 
		{
			{        L"",            L"", false },
			{        L"",           L"a",  true },
			{        L"a",           L"", false },
			{        L"a",          L"a", false },
			{        L"a",          L"b",  true },
			{    L"____a",      L"____a", false },
			{    L"____a",      L"____b",  true },
			{    L"____b",      L"____a", false },
			{  L"_\xffff",         L"_a", false },   // Test high values, which exercises the signed/unsiged comparison behavior.
			{       L"_a",    L"_\xffff",  true }
		};

		str_less<const wchar_t*> slW;
		for(size_t i = 0; i < EAArrayCount(resultsW); i++)
		{
			// Verify that our test is in line with the strcmp function.
			bool bResult = (EA::StdC::Strcmp(resultsW[i].p1, resultsW[i].p2) < 0);
			EATEST_VERIFY_F(bResult == resultsW[i].expectedResult, "Strcmp failure, test %zu. Expected \"%s\" to be %sless than \"%s\"", i, results8[i].p1, results8[i].expectedResult ? "" : "not ", results8[i].p2);

			// Verify that str_less achieves the expected results.
			bResult = slW(resultsW[i].p1, resultsW[i].p2);
			EATEST_VERIFY_F(bResult == resultsW[i].expectedResult, "str_less test failure, test %zu. Expected \"%ls\" to be %sless than \"%ls\"", i, resultsW[i].p1, resultsW[i].expectedResult ? "" : "not ", resultsW[i].p2);
		}
	}

	{
		// str_less
		char p0[] = "";
		char p1[] = "hello";
		char p2[] = "world";
		char p3[] = "helllllo";
		char p4[] = "hello"; // Intentionally the same value as p1.

		typedef set<const char*, str_less<const char*> > StringSet;
		StringSet ss;

		ss.insert(p1);
		ss.insert(p2);
		ss.insert(p3);

		StringSet::iterator it = ss.find(p0);
		EATEST_VERIFY(it == ss.end());

		it = ss.find(p1);
		EATEST_VERIFY(it != ss.end());

		it = ss.find(p2);
		EATEST_VERIFY(it != ss.end());

		it = ss.find(p4);
		EATEST_VERIFY(it != ss.end());
	}

	{
		// equal_to_2
		N1 n11(1);
		N1 n13(3);
		N2 n21(1);
		N2 n22(2);
		//const N1 cn11(1);
		//const N1 cn13(3);

		equal_to_2<N1, N2> e;
		EATEST_VERIFY(e(n11, n21));
		EATEST_VERIFY(e(n21, n11));

		equal_to_2<N1, N1> es;
		EATEST_VERIFY(es(n11, n11));

		//equal_to_2<const N1, N1> ec; // To do: Make this case work.
		//EATEST_VERIFY(e(cn11, n11));

		// not_equal_to_2
		not_equal_to_2<N1, N2> n;
		EATEST_VERIFY(n(n11, n22));
		EATEST_VERIFY(n(n22, n11));

		not_equal_to_2<N1, N1> ns;
		EATEST_VERIFY(ns(n11, n13));

		// less_2
		less_2<N1, N2> le;
		EATEST_VERIFY(le(n11, n22));
		EATEST_VERIFY(le(n22, n13));

		less_2<N1, N1> les;
		EATEST_VERIFY(les(n11, n13));
	}


	{
		// Test defect report entry #297.
		const X x;
		foo< const_mem_fun_t<void, X> >(&x);
	}


	{
		// mem_fun (no argument version)
		TestClass  tc0, tc1, tc2;
		TestClass* tcArray[3] = { &tc0, &tc1, &tc2 };

		for_each(tcArray, tcArray + 3, mem_fun(&TestClass::Increment));
		EATEST_VERIFY((tc0.mX == 38) && (tc1.mX == 38) && (tc2.mX == 38));

		for_each(tcArray, tcArray + 3, mem_fun(&TestClass::IncrementConst));
		EATEST_VERIFY((tc0.mX == 39) && (tc1.mX == 39) && (tc2.mX == 39));
	}


	{
		// mem_fun (one argument version)
		TestClass  tc0, tc1, tc2;
		TestClass* tcArray[3]  = { &tc0, &tc1, &tc2 };
		int        intArray1[3] = { -1,  0,  2 };
		int        intArray2[3] = { -9, -9, -9 };

		transform(tcArray, tcArray + 3, intArray1, intArray2, mem_fun(&TestClass::MultiplyBy));
		EATEST_VERIFY((intArray2[0] == -37) && (intArray2[1] == 0) && (intArray2[2] == 74));

		intArray2[0] = intArray2[1] = intArray2[2] = -9;
		transform(tcArray, tcArray + 3, intArray1, intArray2, mem_fun(&TestClass::MultiplyByConst));
		EATEST_VERIFY((intArray2[0] == -37) && (intArray2[1] == 0) && (intArray2[2] == 74));
	}


	{
		// mem_fun_ref (no argument version)
		TestClass tcArray[3];

		for_each(tcArray, tcArray + 3, mem_fun_ref(&TestClass::Increment));
		EATEST_VERIFY((tcArray[0].mX == 38) && (tcArray[1].mX == 38) && (tcArray[2].mX == 38));

		for_each(tcArray, tcArray + 3, mem_fun_ref(&TestClass::IncrementConst));
		EATEST_VERIFY((tcArray[0].mX == 39) && (tcArray[1].mX == 39) && (tcArray[2].mX == 39));
	}


	{
		// mem_fun_ref (one argument version)
		TestClass tcArray[3];
		int       intArray1[3] = { -1,  0,  2 };
		int       intArray2[3] = { -9, -9, -9 };

		transform(tcArray, tcArray + 3, intArray1, intArray2, mem_fun_ref(&TestClass::MultiplyBy));
		EATEST_VERIFY((intArray2[0] == -37) && (intArray2[1] == 0) && (intArray2[2] == 74));

		intArray2[0] = intArray2[1] = intArray2[2] = -9;
		transform(tcArray, tcArray + 3, intArray1, intArray2, mem_fun_ref(&TestClass::MultiplyByConst));
		EATEST_VERIFY((intArray2[0] == -37) && (intArray2[1] == 0) && (intArray2[2] == 74));
	}


	{
		// Template instantations.
		// These tell the compiler to compile all the functions for the given class.
		eastl::hash_set<String8MA,  eastl::string_hash<String8MA> >  hs8;
		eastl::hash_set<String16MA, eastl::string_hash<String16MA> > hs16;

		EATEST_VERIFY(hs8.empty());
		EATEST_VERIFY(hs16.empty());
	}

	{
		// unary_compose
		/*
		eastl::vector<double> angles;
		eastl::vector<double> sines;

		eastl::transform(angles.begin(), angles.end(), sines.begin(),
				  eastl::compose1(eastl::negate<double>(),
						   eastl::compose1(eastl::ptr_fun(sin),
									eastl::bind2nd(eastl::multiplies<double>(), 3.14159 / 180.0))));
		*/

		// binary_compose
		list<int> L;

		eastl::list<int>::iterator in_range = 
			 eastl::find_if(L.begin(), L.end(),
					 eastl::compose2(eastl::logical_and<bool>(),
							  eastl::bind2nd(eastl::greater_equal<int>(), 1),
							  eastl::bind2nd(eastl::less_equal<int>(), 10)));
		EATEST_VERIFY(in_range == L.end());
	}

	{
		nErrorCount += TestHashHelper<int>(4330);
		nErrorCount += TestHashHelper<bool>(true);
		nErrorCount += TestHashHelper<char>('E');
		nErrorCount += TestHashHelper<signed char>('E');
		nErrorCount += TestHashHelper<unsigned char>('E');
		nErrorCount += TestHashHelper<char8_t>('E');
		nErrorCount += TestHashHelper<char16_t>(0xEAEA);
		nErrorCount += TestHashHelper<char32_t>(0x00EA4330);
		#if !defined(EA_WCHAR_T_NON_NATIVE)
			nErrorCount += TestHashHelper<wchar_t>(L'E');
		#endif
		nErrorCount += TestHashHelper<signed short>(4330);
		nErrorCount += TestHashHelper<unsigned short>(4330u);
		nErrorCount += TestHashHelper<signed int>(4330);
		nErrorCount += TestHashHelper<unsigned int>(4330u);
		nErrorCount += TestHashHelper<signed long>(4330l);
		nErrorCount += TestHashHelper<unsigned long>(4330ul);
		nErrorCount += TestHashHelper<signed long long>(4330ll);
		nErrorCount += TestHashHelper<unsigned long long>(4330ll);
		nErrorCount += TestHashHelper<float>(4330.099999f);
		nErrorCount += TestHashHelper<double>(4330.055);
		nErrorCount += TestHashHelper<long double>(4330.0654l);
	}


#if defined(EA_COMPILER_CPP11_ENABLED) && EASTL_VARIADIC_TEMPLATES_ENABLED
	// On platforms do not support variadic templates the eastl::invoke (eastl::mem_fn is built on eastl::invoke)
	// implementation is extremely basic and does not hold up.  A significant amount of code would have to be written
	// and I don't believe the investment is justified at this point.  If you require this functionality on older
	// compilers please contact us.
	//

	// eastl::invoke
	{
		struct TestStruct
		{
			TestStruct(int inValue) : value(inValue) {}
			void Add(int addAmount) { value += addAmount; }
			// void Add(int addAmount1, int addAmount2) { value += (addAmount1 + addAmount2); }	// error
			// void Add() { value += 10; } 													   	// error
			int value;
		};
		TestStruct a(42);
		eastl::invoke(&TestStruct::Add, a, 6);
		// eastl::invoke(&TestStruct::Add, a);  		// error:  design does not support overloading.  member function must be known at construction time.
		// eastl::invoke(&TestStruct::Add, a, 6,10);    // error:  design does not support overloading.  member function must be known at construction time.
	}

	// eastl::mem_fn
	{
		struct AddingStruct 
		{
			AddingStruct(int inValue) : value(inValue) {}
			void Add(int addAmount) { value += addAmount; }
			void Add2(int add1, int add2) { value += (add1 + add2); }
			int value;
		};

		{
			AddingStruct a(42);
			eastl::mem_fn(&AddingStruct::Add)(a, 6);
			EATEST_VERIFY(a.value == 48);
		}
		{
			AddingStruct a(42);
			eastl::mem_fn(&AddingStruct::Add2)(a, 3, 3);
			EATEST_VERIFY(a.value == 48);
		}
		{
			AddingStruct a(42);
			auto fStructAdd = eastl::mem_fn(&AddingStruct::Add);
			fStructAdd(a,6);
			EATEST_VERIFY(a.value == 48);
		}
	}
#endif

#if EASTL_FUNCTION_ENABLED
	// eastl::function
	{
		{
			{
				struct Functor { int operator()() { return 42; } };
				eastl::function<int(void)> fn = Functor();
				EATEST_VERIFY(fn() == 42);
			}
			
			{
				struct Functor { int operator()(int in) { return in; } };
				eastl::function<int(int)> fn = Functor();
				EATEST_VERIFY(fn(24) == 24);
			}
		}

		{
			{ 
				auto lambda = []{};
				EA_UNUSED(lambda);
				static_assert(detail::is_inplace_allocated<decltype(lambda), eastl::allocator>::value == true, "lambda equivalent to function pointer does not fit in eastl::function local memory.");
			}

			{
				eastl::function<void(void)> fn; 

				EATEST_VERIFY(!fn);
				fn =  [] {};
				EATEST_VERIFY(fn);
			}

			{
				eastl::function<int(int)> fn = [](int param) { return param; };
				EATEST_VERIFY(fn(42) == 42);
			}

			{
				eastl::function<int(int)> fn = ReturnVal;
				EATEST_VERIFY(fn(42) == 42);
			}

			{
				eastl::function<int()> fn0 = ReturnZero;
				eastl::function<int()> fn1 = ReturnOne;

				EATEST_VERIFY(fn0() == 0 && fn1() == 1);
				swap(fn0, fn1);
				EATEST_VERIFY(fn0() == 1 && fn1() == 0);
			}

			{
				eastl::function<int()> fn0 = ReturnZero;
				eastl::function<int()> fn1 = ReturnOne;

				EATEST_VERIFY(fn0() == 0 && fn1() == 1);
				fn0 = fn1;
				EATEST_VERIFY(fn0() == 1 && fn1() == 1);
			}

			#if !EASTL_NO_RVALUE_REFERENCES 
			{
				eastl::function<int()> fn0 = ReturnZero;
				eastl::function<int()> fn1 = ReturnOne;

				EATEST_VERIFY(fn0() == 0 && fn1() == 1);
				fn0 = eastl::move(fn1);
				EATEST_VERIFY(fn0() == 1 && fn1 == nullptr);
			}
			#endif

			{
				eastl::function<int(int)> f1(nullptr);
				EATEST_VERIFY(!f1);

				eastl::function<int(int)> f2 = nullptr;
				EATEST_VERIFY(!f2);
			}
		}

		{
			// test the default allocator path by using a lambda capture too large to fit into the eastl::function local
			// storage.
			uint64_t a = 1, b = 2, c = 3, d = 4, e = 5, f = 6;
			eastl::function<uint64_t(void)> fn = [=] { return a + b + c + d + e + f; };
			auto result = fn();
			EATEST_VERIFY(result == 21);
		}

		{
			uint64_t a = 1, b = 2, c = 3, d = 4, e = 5, f = 6;
			eastl::function<uint64_t(void)> fn(eastl::allocator_arg_t(), eastl::allocator(), [=] { return a + b + c + d + e + f; });

			auto result = fn();
			EATEST_VERIFY(result == 21);
		}

		{
			int allocatorErrorCount = 0;

			// test a custom partial stateful allocator
			static const unsigned int kSentinelValue = 0xdeadbeef;
			struct Mallocator
			{
				Mallocator(int *errorCount)
					: mErrorCount(errorCount)
					, mSentinel(kSentinelValue) {}

				Mallocator(const Mallocator& other)
					: mErrorCount(other.mErrorCount)
					, mSentinel(other.mSentinel) {}

				Mallocator& operator=(const Mallocator &other)
				{
					mSentinel = other.mSentinel;
					mErrorCount = other.mErrorCount;
					return *this;
				}

				~Mallocator()
				{
					if (mSentinel != kSentinelValue)
					{
						(*mErrorCount)++;
					}

					mSentinel = 0x4B1D;  // clear sentinel to catch illegal uses
				}

				void* allocate(size_t n)
				{
					// Verify the sentinel value
					if (mSentinel != kSentinelValue)
					{
						(*mErrorCount)++;
					}
					return malloc(n);
				}

				void deallocate(void* p, size_t n)
				{
					// Verify the sentinel value
					if (mSentinel != kSentinelValue)
					{
						(*mErrorCount)++;
					}

					memset(p, 0xcd, n);  // memset the memory to catch illegal accesses
					free(p);
				}

			private:

				int *mErrorCount;
				unsigned int mSentinel;
			} mallocator(&allocatorErrorCount);

			uint64_t a = 1, b = 2, c = 3, d = 4, e = 5, f = 6;
			eastl::function<uint64_t(void)> fn(eastl::allocator_arg_t(), mallocator, [=] { return a + b + c + d + e + f; });

			auto result = fn();
			EATEST_VERIFY(result == 21);
			EATEST_VERIFY(allocatorErrorCount == 0);
		}


		// Ensure no allocations are made in this case
		{
			struct Failocator
			{
				void* allocate(size_t n) { EA_FAIL(); return malloc(n); }
				void deallocate(void* p, size_t) { EA_FAIL(); free(p); }
			} failocator;

			typedef eastl::function<int*(int&)> failocator_function_t;

			eastl::vector<failocator_function_t> funcs;
			funcs.push_back(failocator_function_t(eastl::allocator_arg_t(), failocator, [](int&){return (int*)42;}));
		}


		// Verify that all allocations made by the user allocator are cleaned up at scope exit
		{
			int allocCount = 0;
			{
				eastl::function<uint64_t(void)> keeper;
				{
					struct LargeStateAllocator
					{
						LargeStateAllocator(int* pAllocCount)							{ mpAllocCount = pAllocCount; large_state_block[0] = 0; }  // set an element to make the compiler happy.
						LargeStateAllocator(const LargeStateAllocator& other)			{ mpAllocCount = other.mpAllocCount; }
						LargeStateAllocator(LargeStateAllocator&& other)				 { mpAllocCount = other.mpAllocCount; }
						LargeStateAllocator& operator=(const LargeStateAllocator &other) { mpAllocCount = other.mpAllocCount; return *this;}					

						void* allocate(size_t n)		 { (*mpAllocCount)++; return malloc(n); }
						void deallocate(void* p, size_t) { (*mpAllocCount)--; free(p); }

						private:
						char large_state_block[4096];  // forces eastl::function to allocate memory on the heap for the allocator instance
						int* mpAllocCount;
					} largeStateAllocator(&allocCount);


					auto lambda = [] { };
					static_assert(detail::is_inplace_allocated<decltype(lambda), LargeStateAllocator>::value == false, "large stateful allocator should not fit into eastl::function local buffers");

					eastl::vector<eastl::function<void(void)>> funcs;				
					funcs.push_back(eastl::function<void(void)>(eastl::allocator_arg_t(), largeStateAllocator, lambda));

					uint64_t a = 1, b = 2, c = 3, d = 4, e = 5, f = 6;					
					keeper = eastl::function<uint64_t(void)>(eastl::allocator_arg_t(), largeStateAllocator, [=] { return a + b + c + d + e + f; });
				}

				// verify we copy the allocator internal to eastl::function
				uint64_t result = keeper();
				EATEST_VERIFY(result == 21);
			}
			EATEST_VERIFY(allocCount == 0);
		}
	}

	// Checking _MSC_EXTENSIONS is required because the Microsoft calling convention classifiers are only available when
	// compiler specific C/C++ language extensions are enabled.
	#if defined(EA_PLATFORM_MICROSOFT) && defined(_MSC_EXTENSIONS)
	{
		// no arguments
		typedef void(__stdcall * StdCallFunction)();
		typedef void(__cdecl * CDeclFunction)();
		
		// only varargs
		typedef void(__stdcall * StdCallFunctionWithVarargs)(...);
		typedef void(__cdecl * CDeclFunctionWithVarargs)(...);

		// arguments and varargs
		typedef void(__stdcall * StdCallFunctionWithVarargsAtEnd)(int, int, int, ...);
		typedef void(__cdecl * CDeclFunctionWithVarargsAtEnd)(int, short, long, ...);

		static_assert(!eastl::is_function<StdCallFunction>::value, "is_function failure");
		static_assert(!eastl::is_function<CDeclFunction>::value, "is_function failure");
		static_assert(eastl::is_function<typename eastl::remove_pointer<StdCallFunction>::type>::value, "is_function failure");
		static_assert(eastl::is_function<typename eastl::remove_pointer<CDeclFunction>::type>::value, "is_function failure");
		static_assert(eastl::is_function<typename eastl::remove_pointer<StdCallFunctionWithVarargs>::type>::value, "is_function failure");
		static_assert(eastl::is_function<typename eastl::remove_pointer<CDeclFunctionWithVarargs>::type>::value, "is_function failure");
		static_assert(eastl::is_function<typename eastl::remove_pointer<StdCallFunctionWithVarargsAtEnd>::type>::value, "is_function failure");
		static_assert(eastl::is_function<typename eastl::remove_pointer<CDeclFunctionWithVarargsAtEnd>::type>::value, "is_function failure");
	}
	#endif
#endif // EASTL_FUNCTION_ENABLED

	// Test Function Objects
	#if defined(EA_COMPILER_CPP14_ENABLED)
	{
		// eastl::plus<void>
		{
			{
				auto result = eastl::plus<>{}(40, 2);
				EA_UNUSED(result);
				EATEST_VERIFY(result == 42);
			}

			{
				auto result = eastl::plus<>{}(40.0, 2.0);
				EA_UNUSED(result);
				EATEST_VERIFY(result == 42.0);
			}

			{
				auto result = eastl::plus<>{}(eastl::string("4"), "2");
				EA_UNUSED(result);
				EATEST_VERIFY(result == "42");
			}
		}

		// eastl::minus<void>
		{
			{
				auto result = eastl::minus<>{}(6, 2);
				EA_UNUSED(result);
				EATEST_VERIFY(result == 4);
			}

			{
				auto result = eastl::minus<>{}(6.0, 2.0);
				EA_UNUSED(result);
				EATEST_VERIFY(result == 4.0);
			}
		}

		// eastl::multiplies
		{
			{
				auto result = eastl::multiplies<>{}(6, 2);
				EA_UNUSED(result);
				EATEST_VERIFY(result == 12);
			}

			{
				auto result = eastl::multiplies<>{}(6.0, 2.0);
				EA_UNUSED(result);
				EATEST_VERIFY(result == 12.0);
			}
		}


		// eastl::divides
		{
			{
				auto result = eastl::divides<>{}(6, 2);
				EA_UNUSED(result);
				EATEST_VERIFY(result == 3);
			}

			{
				auto result = eastl::divides<>{}(6.0, 2.0);
				EA_UNUSED(result);
				EATEST_VERIFY(result == 3.0);
			}
		}

		// eastl::modulus
		{
			{
				auto result = eastl::modulus<>{}(6, 2);
				EA_UNUSED(result);
				EATEST_VERIFY(result == 0);
			}

			{
				auto result = eastl::modulus<>{}(7, 2);
				EA_UNUSED(result);
				EATEST_VERIFY(result == 1);
			}
		}

		// eastl::negate
		{
			{
				auto result = eastl::negate<>{}(42);
				EA_UNUSED(result);
				EATEST_VERIFY(result == -42);
			}

			{
				auto result = eastl::negate<>{}(42.0);
				EA_UNUSED(result);
				EATEST_VERIFY(result == -42.0);
			}
		}

		// eastl::equal_to
		{
			{
				auto result = eastl::equal_to<>{}(40, 2);
				EA_UNUSED(result);
				EATEST_VERIFY(!result);
			}

			{
				auto result = eastl::equal_to<>{}(40, 40);
				EA_UNUSED(result);
				EATEST_VERIFY(result);
			}
		}

		// eastl::not_equal_to
		{
			{
				auto result = eastl::not_equal_to<>{}(40, 2);
				EA_UNUSED(result);
				EATEST_VERIFY(result);
			}

			{
				auto result = eastl::not_equal_to<>{}(40, 40);
				EA_UNUSED(result);
				EATEST_VERIFY(!result);
			}
		}

		// eastl::greater<void>
		{
			{
				auto result = eastl::greater<>{}(40, 2);
				EA_UNUSED(result);
				EATEST_VERIFY(result);
			}

			{
				auto result = eastl::greater<>{}(1, 2);
				EA_UNUSED(result);
				EATEST_VERIFY(!result);
			}

			{
				auto result = eastl::greater<>{}(eastl::string("4"), "2");
				EA_UNUSED(result);
				EATEST_VERIFY(result);
			}
		}

		// eastl::less<void>
		{
			{
				auto result = eastl::less<>{}(40, 2);
				EA_UNUSED(result);
				EATEST_VERIFY(!result);
			}

			{
				auto result = eastl::less<>{}(1, 2);
				EA_UNUSED(result);
				EATEST_VERIFY(result);
			}

			{
				auto result = eastl::less<>{}(eastl::string("4"), "2");
				EA_UNUSED(result);
				EATEST_VERIFY(!result);
			}
		}

		// eastl::greater_equal<void>
		{
			{
				auto result = eastl::greater_equal<>{}(40, 2);
				EA_UNUSED(result);
				EATEST_VERIFY(result);
			}

			{
				auto result = eastl::greater_equal<>{}(40, 40);
				EA_UNUSED(result);
				EATEST_VERIFY(result);
			}

			{
				auto result = eastl::greater_equal<>{}(40, 43);
				EA_UNUSED(result);
				EATEST_VERIFY(!result);
			}
		}

		// eastl::less_equal<void>
		{
			{
				auto result = eastl::less_equal<>{}(40, 2);
				EA_UNUSED(result);
				EATEST_VERIFY(!result);
			}

			{
				auto result = eastl::less_equal<>{}(40, 40);
				EA_UNUSED(result);
				EATEST_VERIFY(result);
			}

			{
				auto result = eastl::less_equal<>{}(40, 43);
				EA_UNUSED(result);
				EATEST_VERIFY(result);
			}
		}

		// eastl::logical_and
		{
			auto result = eastl::logical_and<>{}(true, true);
			EATEST_VERIFY(result);
			result = eastl::logical_and<>{}(true, false);
			EATEST_VERIFY(!result);
			result = eastl::logical_and<>{}(false, true);
			EATEST_VERIFY(!result);
			result = eastl::logical_and<>{}(false, false);
			EATEST_VERIFY(!result);

			bool b = false;
			result = eastl::logical_and<>{}(b, false);
			EATEST_VERIFY(!result);
		}

		// eastl::logical_or
		{
			auto result = eastl::logical_or<>{}(true, true);
			EATEST_VERIFY(result);
			result = eastl::logical_or<>{}(true, false);
			EATEST_VERIFY(result);
			result = eastl::logical_or<>{}(false, true);
			EATEST_VERIFY(result);
			result = eastl::logical_or<>{}(false, false);
			EATEST_VERIFY(!result);

			bool b = false;
			result = eastl::logical_or<>{}(b, false);
			EATEST_VERIFY(!result);
			result = eastl::logical_or<>{}(b, true);
			EATEST_VERIFY(result);
		}

		// eastl::logical_not
		{
			auto result = eastl::logical_not<>{}(true);
			EATEST_VERIFY(!result);
			result = eastl::logical_not<>{}(result);
			EATEST_VERIFY(result);
			result = eastl::logical_not<>{}(false);
			EATEST_VERIFY(result);
		}
	}
	#endif

	return nErrorCount;
}


