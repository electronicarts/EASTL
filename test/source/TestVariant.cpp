/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/string.h>
#include <EASTL/algorithm.h>
#include <EASTL/sort.h>

#ifdef EA_COMPILER_CPP14_ENABLED
#include "ConceptImpls.h"
#include <EASTL/variant.h>


int TestVariantAlternative()
{
	using namespace eastl;
	int nErrorCount = 0;
	{
		using v_t = variant<int>;
		static_assert(is_same_v<variant_alternative_t<0, v_t>, int>, "error variant_alternative");
	}
	{
		using v_t = variant<int, long, short, char>;

		static_assert(is_same_v<variant_alternative_t<0, v_t>, int>, "error variant_alternative");
		static_assert(is_same_v<variant_alternative_t<1, v_t>, long>, "error variant_alternative");
		static_assert(is_same_v<variant_alternative_t<2, v_t>, short>, "error variant_alternative");
		static_assert(is_same_v<variant_alternative_t<3, v_t>, char>, "error variant_alternative");
	}
	{
		struct custom_type1 {};
		struct custom_type2 {};
		struct custom_type3 {};

		using v_t = variant<int, long, short, char, size_t, unsigned, signed, custom_type1, custom_type2, custom_type3>;

		static_assert(is_same_v<variant_alternative_t<5, v_t>, unsigned>, "error variant_alternative");
		static_assert(is_same_v<variant_alternative_t<6, v_t>, signed>, "error variant_alternative");
		static_assert(is_same_v<variant_alternative_t<7, v_t>, custom_type1>, "error variant_alternative");
		static_assert(is_same_v<variant_alternative_t<8, v_t>, custom_type2>, "error variant_alternative");
		static_assert(is_same_v<variant_alternative_t<9, v_t>, custom_type3>, "error variant_alternative");
	}
	// cv-qualifier tests
	{
		using v_t = variant<int, const int, volatile int, const volatile int>;

		static_assert(is_same_v<variant_alternative_t<0, v_t>, int>, "error variant_alternative");
		static_assert(is_same_v<variant_alternative_t<1, v_t>, const int>, "error variant_alternative");
		static_assert(is_same_v<variant_alternative_t<2, v_t>, volatile int>, "error variant_alternative");
		static_assert(is_same_v<variant_alternative_t<3, v_t>, const volatile int>, "error variant_alternative");
	}
	return nErrorCount;
}

int TestVariantSize()
{
	using namespace eastl;
	int nErrorCount = 0;

	static_assert(variant_size<variant<int>>() == 1, "error variant_size");
	static_assert(variant_size<variant<int, int>>() == 2, "error variant_size");
	static_assert(variant_size<variant<int, int, int, int>>() == 4, "error variant_size");
	static_assert(variant_size<variant<const int>>() == 1, "error variant_size");
	static_assert(variant_size<variant<volatile int>>() == 1, "error variant_size");
	static_assert(variant_size<variant<const volatile int>>() == 1, "error variant_size");

	static_assert(variant_size_v<variant<int>> == 1, "error variant_size");
	static_assert(variant_size_v<variant<int, int>> == 2, "error variant_size");
	static_assert(variant_size_v<variant<int, int, int, int>> == 4, "error variant_size");
	static_assert(variant_size_v<variant<const int>> == 1, "error variant_size");
	static_assert(variant_size_v<variant<volatile int>> == 1, "error variant_size");
	static_assert(variant_size_v<variant<const volatile int>> == 1, "error variant_size");

	static_assert(variant_size_v<variant<int, int>> == 2, "error variant_size_v");
	static_assert(variant_size_v<variant<volatile int, const int>> == 2, "error variant_size_v");
	static_assert(variant_size_v<variant<volatile int, const int, const volatile int>> == 3, "error variant_size_v");

	return nErrorCount;
}

int TestVariantHash()
{
	using namespace eastl;
	int nErrorCount = 0;

	{ hash<monostate> h; EA_UNUSED(h); }

	return nErrorCount;
}

int TestVariantBasic()
{
	using namespace eastl;
	int nErrorCount = 0;

	{ VERIFY(variant_npos == size_t(-1)); }

	{ variant<int> v;                                                         EA_UNUSED(v); }
	{ variant<int, short> v;                                                  EA_UNUSED(v); }
	{ variant<int, short, float> v;                                           EA_UNUSED(v); }
	{ variant<int, short, float, char> v;                                     EA_UNUSED(v); }
	{ variant<int, short, float, char, long> v;                               EA_UNUSED(v); }
	{ variant<int, short, float, char, long, long long> v;                    EA_UNUSED(v); }
	{ variant<int, short, float, char, long, long long, double> v;            EA_UNUSED(v); }

	{ variant<monostate> v;                                                   EA_UNUSED(v); }
	{ variant<monostate, NotDefaultConstructible> v;                          EA_UNUSED(v); }
	{ variant<int, NotDefaultConstructible> v;                                EA_UNUSED(v); }

	{
		struct MyObj
		{
			MyObj() : i(1337) {}
			~MyObj() {}

			int i;
		};

		struct MyObj2
		{
			MyObj2(int& ii) : i(ii) {}
			~MyObj2() {}

			MyObj2& operator=(const MyObj2&) = delete;

			int& i;
		};

		static_assert(!eastl::is_trivially_destructible_v<MyObj>, "MyObj can't be trivially destructible");
		static_assert(!eastl::is_trivially_destructible_v<MyObj2>, "MyObj2 can't be trivially destructible");

		{
			eastl::variant<MyObj, MyObj2> myVar;
			VERIFY(get<MyObj>(myVar).i == 1337);
		}

		{
			eastl::variant<MyObj, MyObj2> myVar = MyObj();
			VERIFY(get<MyObj>(myVar).i == 1337);
		}

		{
			int i = 42;
			eastl::variant<MyObj, MyObj2> myVar = MyObj2(i);
			VERIFY(get<MyObj2>(myVar).i == 42);
		}

		{
			auto m = MyObj();
			m.i = 2000;

			eastl::variant<MyObj, MyObj2> myVar = m;
			VERIFY(get<MyObj>(myVar).i == 2000);
		}
	}

	{ variant<int, int> v;                                                    EA_UNUSED(v); }
	{ variant<const short, volatile short, const volatile short> v;           EA_UNUSED(v); }
	{ variant<int, int, const short, volatile short, const volatile short> v; EA_UNUSED(v); }

	{
		// verify constructors and destructors are called
		{
			variant<TestObject> v = TestObject(1337);
			VERIFY((get<TestObject>(v)).mX == 1337);

			variant<TestObject> vCopy = v;
			VERIFY((get<TestObject>(vCopy)).mX == 1337);
		}
		VERIFY(TestObject::IsClear());
		TestObject::Reset();
	}

	{
		variant<string> v;
		VERIFY(*(get_if<string>(&v)) == "");
		VERIFY(get_if<string>(&v)->empty());
		VERIFY(get_if<string>(&v)->length() == 0);
		VERIFY(get_if<string>(&v)->size() == 0);

		*(get_if<string>(&v)) += 'a';
		VERIFY(*(get_if<string>(&v)) == "a");
	}

	return nErrorCount;
}

int TestVariantGet()
{
	using namespace eastl;
	int nErrorCount = 0;

	{
		const char* strValue = "canada";
		using v_t = variant<int, string>;
		{
			v_t v;
			v = 42;
			VERIFY(v.index() == 0);
			VERIFY(*get_if<int>(&v) == 42);
			VERIFY(get<int>(v) == 42);
			VERIFY( holds_alternative<int>(v));
			VERIFY(!holds_alternative<string>(v));
		}
		{
			v_t v;
			v = strValue;
			VERIFY(v.index() == 1);
			VERIFY(*get_if<string>(&v) == strValue);
			VERIFY(get<string>(v) == strValue);
			VERIFY(!holds_alternative<int>(v));
			VERIFY(holds_alternative<string>(v));
		}
		{
			v_t v;
			v = 42;
			VERIFY(v.index() == 0);
			VERIFY(*get_if<0>(&v) == 42);
			VERIFY(get<0>(v) == 42);
			VERIFY( holds_alternative<int>(v));
			VERIFY(!holds_alternative<string>(v));
		}
		{
			 v_t v;
			 v = strValue;
			 VERIFY(v.index() == 1);
			 VERIFY(*get_if<1>(&v) == strValue);
			 VERIFY(get<1>(v) == strValue);
			 VERIFY(!holds_alternative<int>(v));
			 VERIFY( holds_alternative<string>(v));
		}
		{
			 v_t v;
			 v = strValue;
			 VERIFY(v.index() == 1);
			 VERIFY(*get_if<1>(&v) == strValue);
			 VERIFY(get_if<0>(&v) == nullptr);
		}
		{
			 VERIFY(get_if<0>((v_t*)nullptr) == nullptr);
			 VERIFY(get_if<1>((v_t*)nullptr) == nullptr);
		}
	}

	return nErrorCount;
}

int TestVariantHoldsAlternative()
{
	using namespace eastl;
	int nErrorCount = 0;

	{
		{
			using v_t = variant<int, short>;  // default construct first type
			v_t v;

			VERIFY(!holds_alternative<long>(v));   // Verify that a query for a T not in the variant typelist returns false.
			VERIFY(!holds_alternative<string>(v)); // Verify that a query for a T not in the variant typelist returns false.
			VERIFY( holds_alternative<int>(v));    // variant does hold an int, because its a default constructible first parameter
			VERIFY(!holds_alternative<short>(v));  // variant does not hold a short
		}

		{
			using v_t = variant<monostate, int, short>;  // default construct monostate
			v_t v;

			VERIFY(!holds_alternative<long>(v));   // Verify that a query for a T not in the variant typelist returns false.
			VERIFY(!holds_alternative<string>(v)); // Verify that a query for a T not in the variant typelist returns false.
			VERIFY(!holds_alternative<int>(v));    // variant does not hold an int 
			VERIFY(!holds_alternative<short>(v));  // variant does not hold a short
		}

		{
			using v_t = variant<monostate, int>;

			{
				v_t v;
				VERIFY(!holds_alternative<int>(v));   // variant does not hold an int

				v = 42;
				VERIFY(holds_alternative<int>(v));	// variant does hold an int
			}

			{
				v_t v1, v2;
				VERIFY(!holds_alternative<int>(v1));
				VERIFY(!holds_alternative<int>(v2));

				v1 = 42;
				VERIFY(holds_alternative<int>(v1));
				VERIFY(!holds_alternative<int>(v2));

				eastl::swap(v1, v2);
				VERIFY(!holds_alternative<int>(v1));
				VERIFY(holds_alternative<int>(v2));
			}
		}
	}

	return nErrorCount;
}

int TestVariantValuelessByException()
{
	using namespace eastl;
	int nErrorCount = 0;

	{
		{
			using v_t = variant<int, short>;
			static_assert(eastl::is_default_constructible_v<v_t>, "valueless_by_exception error");

			v_t v;
			VERIFY(!v.valueless_by_exception());

			v = 42;
			VERIFY(!v.valueless_by_exception());
		}

		{
			using v_t = variant<monostate, int>;
			static_assert(eastl::is_default_constructible_v<v_t>, "valueless_by_exception error");

			v_t v1, v2;
			VERIFY(!v1.valueless_by_exception());
			VERIFY(!v2.valueless_by_exception());

			v1 = 42;
			VERIFY(!v1.valueless_by_exception());
			VERIFY(!v2.valueless_by_exception());

			eastl::swap(v1, v2);
			VERIFY(!v1.valueless_by_exception());
			VERIFY(!v2.valueless_by_exception());

			v1 = v2;
			VERIFY(!v1.valueless_by_exception());
			VERIFY(!v2.valueless_by_exception());
		}

		{
			struct NotDefaultConstructibleButHasConversionCtor
			{
				NotDefaultConstructibleButHasConversionCtor() = delete;
				NotDefaultConstructibleButHasConversionCtor(int) {}
			};
			static_assert(!eastl::is_default_constructible<NotDefaultConstructibleButHasConversionCtor>::value, "valueless_by_exception error");

			using v_t = variant<NotDefaultConstructibleButHasConversionCtor>;
			v_t v(42);
			static_assert(!eastl::is_default_constructible_v<v_t>, "valueless_by_exception error");
			VERIFY(!v.valueless_by_exception());
		}

		// TODO(rparolin):  review exception safety for variant types 
		//
		// {
		// #if EASTL_EXCEPTIONS_ENABLED
		//     struct DefaultConstructibleButThrows
		//     {
		//         DefaultConstructibleButThrows() {}
		//         ~DefaultConstructibleButThrows() {}
		//
		//         DefaultConstructibleButThrows(DefaultConstructibleButThrows&&) { throw 42; }
		//         DefaultConstructibleButThrows(const DefaultConstructibleButThrows&) { throw 42; }
		//         DefaultConstructibleButThrows& operator=(const DefaultConstructibleButThrows&) { throw 42; }
		//         DefaultConstructibleButThrows& operator=(DefaultConstructibleButThrows&&) { throw 42; }
		//     };
		//
		//     using v_t = variant<DefaultConstructibleButThrows>;
		//
		//     v_t v1;
		//     VERIFY(!v1.valueless_by_exception());
		//
		//     try
		//     {
		//         v1 = DefaultConstructibleButThrows();
		//     }
		//     catch (...)
		//     {
		//         VERIFY(v1.valueless_by_exception());
		//     }
		// #endif
		// }
	}

	return nErrorCount;
}

int TestVariantCopyAndMove()
{
	using namespace eastl;
	int nErrorCount = 0;

	{
		{
			using v_t = variant<int, short, char>;

			v_t v1 = 42;
			v_t v2 = v1;

			VERIFY(get<int>(v2) == get<int>(v1));
		}

	}

	return nErrorCount;
}

int TestVariantEmplace()
{
	using namespace eastl;
	int nErrorCount = 0;

	{
		variant<int> v;
		v.emplace<int>(42);
		VERIFY(get<int>(v) == 42);
	}
	{
		variant<int> v;
		v.emplace<0>(42);
		VERIFY(get<0>(v) == 42);
	}

	{
		variant<int, short, long> v;

		v.emplace<0>(42);
		VERIFY(get<0>(v) == 42);

		v.emplace<1>(short(43));
		VERIFY(get<1>(v) == short(43));

		v.emplace<2>(44L);
		VERIFY(get<2>(v) == 44L);
	}
	{
		variant<int, short, long> v;

		v.emplace<int>(42);
		VERIFY(get<int>(v) == 42);

		v.emplace<short>(short(43));
		VERIFY(get<short>(v) == short(43));

		v.emplace<long>(44L);
		VERIFY(get<long>(v) == 44L);
	}

	{
		{
			variant<TestObject> v;
			v.emplace<0>(1337);
			VERIFY(get<0>(v).mX == 1337);
		}
		VERIFY(TestObject::IsClear());
		TestObject::Reset();
	}

	{
		{
			variant<int, TestObject> v;

			v.emplace<int>(42);
			VERIFY(get<int>(v) == 42);

			v.emplace<TestObject>(1337);
			VERIFY(get<TestObject>(v).mX == 1337);

			v.emplace<TestObject>(1338, 42, 3);
			VERIFY(get<TestObject>(v).mX == 1338 + 42 + 3);
		}
		VERIFY(TestObject::IsClear());
		TestObject::Reset();
	}

	{
		{
			struct r {
				r() = default;
				r(int x) : mX(x) {}
				int mX;
			};

			variant<int, r> v;

			v.emplace<0>(42);
			VERIFY(get<0>(v) == 42);

			v.emplace<1>(1337);
			VERIFY(get<1>(v).mX == 1337);
		}
	}

	{
		struct r {
			r() = default;
			r(int a, int b, int c, int d) : a(a), b(b), c(c), d(d) {}
			r(std::initializer_list<int> l)
			{
				auto it = l.begin();

				a = *it++;
				b = *it++;
				c = *it++;
				d = *it++;
			}
			int a, b, c, d;
		};

		r aa{1,2,3,4};
		VERIFY(aa.a == 1);
		VERIFY(aa.b == 2);
		VERIFY(aa.c == 3);
		VERIFY(aa.d == 4);

		variant<r> v;
		v.emplace<0>(std::initializer_list<int>{1,2,3,4});

		VERIFY(get<r>(v).a == 1);
		VERIFY(get<r>(v).b == 2);
		VERIFY(get<r>(v).c == 3);
		VERIFY(get<r>(v).d == 4);
	}

	return nErrorCount;
}

int TestVariantSwap()
{
	using namespace eastl;
	int nErrorCount = 0;

	{
		variant<int, float> v1 = 42;
		variant<int, float> v2 = 24;

		v1.swap(v2);

		VERIFY(get<int>(v1) == 24); 
		VERIFY(get<int>(v2) == 42);

		v1.swap(v2);

		VERIFY(get<int>(v1) == 42); 
		VERIFY(get<int>(v2) == 24);
	}

	{
		 variant<string> v1 = "Hello";
		 variant<string> v2 = "World";

		 VERIFY(get<string>(v1) == "Hello"); 
		 VERIFY(get<string>(v2) == "World");

		 v1.swap(v2);

		 VERIFY(get<string>(v1) == "World");
		 VERIFY(get<string>(v2) == "Hello"); 
	}

	return nErrorCount;
}

int TestVariantRelOps()
{
	using namespace eastl;
	int nErrorCount = 0;

	{
		variant<int, float> v1 = 42;
		variant<int, float> v2 = 24;
		variant<int, float> v1e = v1;

		VERIFY(v1 == v1e);
		VERIFY(v1 != v2);
		VERIFY(v1 > v2);
		VERIFY(v2 < v1);
	}

	{
		vector<variant<int, string>> v = {{1}, {3}, {7}, {4}, {0}, {5}, {2}, {6}, {8}};
		eastl::sort(v.begin(), v.end());
		VERIFY(eastl::is_sorted(v.begin(), v.end()));
	}

	return nErrorCount;
}


int TestVariantInplaceCtors()
{
	using namespace eastl;
	int nErrorCount = 0;

	{
		variant<int, int> v(in_place<0>, 42);
		VERIFY(get<0>(v) == 42);
		VERIFY(v.index() == 0);
	}

	{
		variant<int, int> v(in_place<1>, 42);
		VERIFY(get<1>(v) == 42);
		VERIFY(v.index() == 1);
	}

	{
		variant<int, string> v(in_place<int>, 42);
		VERIFY(get<0>(v) == 42);
		VERIFY(v.index() == 0);
	}

	{
		variant<int, string> v(in_place<string>, "hello");
		VERIFY(get<1>(v) == "hello");
		VERIFY(v.index() == 1);
	}

	return nErrorCount;
}


int TestVariantVisitor()
{
	using namespace eastl;
	int nErrorCount = 0;

	using v_t = variant<int, string, double, long>;

	// TODO(rparolin):  When we have a C++17 compiler
	//
	// template deduction guides test
	// template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	// template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

	// {
	//     v_t arr[] = {42, "rob", 42.0, 42L};

	//     int count = 0;
	//     for (auto& e : arr)
	//     {
	//         eastl::visit(overloaded{[&](int)    { count++; },
	//                                 [&](string) { count++; },
	//                                 [&](double) { count++; },
	//                                 [&](long)   { count++; }}, e);
	//     }
	// }

	{
		v_t arr[] = {42, "hello", 42.0, 42L};

		int count = 0;
		for (auto& e : arr)
		{
			eastl::visit([&](auto){ count++; }, e);
		}

		VERIFY(count == EAArrayCount(arr));
	}

	{
		static bool bVisited = false;

		variant<int, long, string> v = 42;

		struct MyVisitor
		{
			MyVisitor& operator()(int)      { bVisited = true; return *this; };
			MyVisitor& operator()(long)     { return *this; };
			MyVisitor& operator()(string)   { return *this; };
			MyVisitor& operator()(unsigned) { return *this; }; // not in variant
		};

		visit(MyVisitor{}, v);
		VERIFY(bVisited);
	}

	{
		static bool bVisited = false;

		variant<int, string> i = 42;
		variant<int, string> s = "hello";

		struct MultipleVisitor
		{
			MultipleVisitor& operator()(int, int)       { return *this; }
			MultipleVisitor& operator()(int, string)    { bVisited = true; return *this; }
			MultipleVisitor& operator()(string, int)    { return *this; }
			MultipleVisitor& operator()(string, string) { return *this; }
		};

		visit(MultipleVisitor{}, i, s);
		VERIFY(bVisited);
	}

	{
		variant<int, string> v = 42;

		struct ModifyingVisitor
		{
			void operator()(int &i) { i += 1; }
			void operator()(string &s) { s += "hello"; }
		};

		visit(ModifyingVisitor{}, v);
		VERIFY(get<0>(v) == 43);
	}

	{
		variant<int, string> v = 42;

		struct ReturningVisitor
		{
			int operator()(int i) {return i;}
			int operator()(string s) {return 0;}
		};

		VERIFY(visit(ReturningVisitor{}, v) == 42);
	}

#if !defined(EA_COMPILER_MSVC)
	{
		variant<int, string> v = 42;

		struct ReturningDifferentTypesVisitor
		{
			int    operator()(int i)    {return i;}
			size_t operator()(string s) {return s.size();}
		};

		VERIFY(visit(ReturningDifferentTypesVisitor{}, v) == 42);
	}
#endif

	return nErrorCount;
}


int TestVariantAssignment()
{
	using namespace eastl;
	int nErrorCount = 0;

	{
		variant<int, TestObject> v = TestObject(1337);
		VERIFY(get<TestObject>(v).mX == 1337);
		TestObject::Reset();

		v.operator=(42);                         // ensure assignment-operator is called
		VERIFY(TestObject::sTODtorCount == 1);   // verify TestObject dtor is called.
		VERIFY(get<int>(v) == 42);
		TestObject::Reset();
	}

	return nErrorCount;
}


int TestVariantMoveOnly()
{
	using namespace eastl;
	int nErrorCount = 0;

	{
		variant<int, MoveOnlyType> v = MoveOnlyType(1337);
		VERIFY(get<MoveOnlyType>(v).mVal == 1337);
	}

	return nErrorCount;
}


//compilation test related to PR #315: converting constructor and assignment operator compilation error
void TestCompilation(const double e) { eastl::variant<double> v{e}; }



int TestVariantUserRegressionCopyMoveAssignmentOperatorLeak()
{
	using namespace eastl;
	int nErrorCount = 0;

	{
		{
			eastl::variant<TestObject> v = TestObject(1337);
			VERIFY(eastl::get<TestObject>(v).mX == 1337);
			eastl::variant<TestObject> v2 = TestObject(1338);
			VERIFY(eastl::get<TestObject>(v2).mX == 1338);
			v.operator=(v2);
			VERIFY(eastl::get<TestObject>(v).mX == 1338);
			VERIFY(eastl::get<TestObject>(v2).mX == 1338);
		}
		VERIFY(TestObject::IsClear());
		TestObject::Reset();
	}
	{
		{
			eastl::variant<TestObject> v = TestObject(1337);
			VERIFY(eastl::get<TestObject>(v).mX == 1337);
			eastl::variant<TestObject> v2 = TestObject(1338);
			VERIFY(eastl::get<TestObject>(v2).mX == 1338);
			v.operator=(eastl::move(v2));
			VERIFY(eastl::get<TestObject>(v).mX == 1338);
		}
		VERIFY(TestObject::IsClear());
		TestObject::Reset();
	}
	{
		{
			eastl::variant<TestObject> v = TestObject(1337);
			VERIFY(eastl::get<TestObject>(v).mX == 1337);
			v = {};
			VERIFY(eastl::get<TestObject>(v).mX == 0);
		}
		VERIFY(TestObject::IsClear());
		TestObject::Reset();
	}

	return nErrorCount;
}


int TestVariantUserRegressionIncompleteType()
{
	using namespace eastl;
	int nErrorCount = 0;

	{
		struct B;

		struct A
		{
			vector<variant<B>> v;
		};

		struct B
		{
			vector<variant<A>> v;
		};
	}

	return nErrorCount;
}


int TestVariant()
{
	int nErrorCount = 0;

	nErrorCount += TestVariantBasic();
	nErrorCount += TestVariantSize();
	nErrorCount += TestVariantAlternative();
	nErrorCount += TestVariantValuelessByException();
	nErrorCount += TestVariantGet();
	nErrorCount += TestVariantHoldsAlternative();
	nErrorCount += TestVariantHash();
	nErrorCount += TestVariantCopyAndMove();
	nErrorCount += TestVariantSwap();
	nErrorCount += TestVariantEmplace();
	nErrorCount += TestVariantRelOps();
	nErrorCount += TestVariantInplaceCtors();
	nErrorCount += TestVariantVisitor();
	nErrorCount += TestVariantAssignment();
	nErrorCount += TestVariantMoveOnly();
	nErrorCount += TestVariantUserRegressionCopyMoveAssignmentOperatorLeak();
	nErrorCount += TestVariantUserRegressionIncompleteType();

	return nErrorCount;
}
#else
	int TestVariant() { return 0; }
#endif










