/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/string.h>
#include <EASTL/algorithm.h>
#include <EASTL/sort.h>
#include <EASTL/bonus/overloaded.h>

// 4512/4626 - 'class' : assignment operator could not be generated.  // This disabling would best be put elsewhere.
EA_DISABLE_VC_WARNING(4512 4626);

#ifdef EA_COMPILER_CPP14_ENABLED
#include "ConceptImpls.h"
#include <EASTL/variant.h>


#if EASTL_EXCEPTIONS_ENABLED

// Intentionally Non-Trivial.
// There are optimizations we can make in variant if the types are trivial that we don't currently do but can do.
template <typename T>
struct valueless_struct
{
	valueless_struct() {}

	valueless_struct(const valueless_struct&) {}

	~valueless_struct() {}

	struct exception_tag {};

	operator T() const { throw exception_tag{}; }
};

#endif


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

template<class T>
constexpr bool has_default_hash_v = eastl::internal::is_hasher_for_v<eastl::hash<T>, T>;

int TestVariantHash()
{
	using namespace eastl;
	int nErrorCount = 0;

	{ hash<monostate> h; EA_UNUSED(h); }

	// basic hash example - discriminates between values.
	{
		using variant_type = variant<int, unsigned int>;
		hash<variant_type> hasher;
		VERIFY(hasher(variant_type{ in_place_index<0>, 10 }) != hasher(variant_type{ in_place_index<1>, 20 }));
	}

	// hash<variant> should hash the index so that it can distinguish between non-unique alternative types.
	{
		using variant_type = variant<int, int>;
		hash<variant_type> hasher;
		VERIFY(hasher(variant_type{ in_place_index<0>, 10 }) != hasher(variant_type{ in_place_index<1>, 10 }));
	}

	struct no_hash {};

	static_assert(has_default_hash_v<monostate>, "hashable");
	static_assert(!has_default_hash_v<no_hash>, "!hashable");

	static_assert(has_default_hash_v<variant<monostate>>, "hashable");
	static_assert(has_default_hash_v<variant<monostate, monostate>>, "hashable");
	static_assert(has_default_hash_v<variant<const monostate, monostate>>, "hashable");
	static_assert(has_default_hash_v<variant<monostate, const monostate>>, "hashable");

	static_assert(!has_default_hash_v<variant<no_hash>>, "!hashable");
	static_assert(!has_default_hash_v<variant<no_hash, monostate>>, "!hashable");
	static_assert(!has_default_hash_v<variant<monostate, no_hash>>, "!hashable");

	return nErrorCount;
}

int TestVariantBasic()
{
	using namespace eastl;
	int nErrorCount = 0;

	{ VERIFY(variant_npos == size_t(-1)); }

	// variant used to:
	//  - internally store a pointer.
	//  - use a minimum of 16 bytes for its internal buffer for the alternative storage.
	// both of these overheads have been removed.
	static_assert(sizeof(variant<int>) <= 2 * sizeof(size_t), "size");

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
		static_assert(eastl::is_copy_constructible_v<MyObj>, "MyObj can be copy constructed");
		static_assert(eastl::is_copy_constructible_v<MyObj2>, "MyObj2 can't be copy constructed");
		static_assert(eastl::is_copy_assignable_v<MyObj>, "MyObj can be copy assigned");
		static_assert(!eastl::is_copy_assignable_v<MyObj2>, "MyObj2 can't be copy assigned");

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
			VERIFY(get<string>(move(v)) == strValue);
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
		{
#if EASTL_EXCEPTIONS_ENABLED
			v_t v;
			v = strValue;
			EATEST_VERIFY_THROW(get<0>(v));
			EATEST_VERIFY_THROW(get<int>(v));
			auto* str = get_if<1>(&v);
			VERIFY(str && *str == strValue);
#endif
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

			// no matching overload, type is not an alternative.
			// holds_alternative<long>(v);
			// holds_alternative<string>(v);
			VERIFY( holds_alternative<int>(v));    // variant does hold an int, because its a default constructible first parameter
			VERIFY(!holds_alternative<short>(v));  // variant does not hold a short
		}

		{
			using v_t = variant<monostate, int, short>;  // default construct monostate
			v_t v;

			// no matching overload, type is not an alternative.
			// holds_alternative<long>(v);
			// holds_alternative<string>(v);
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

		{
			struct NotDefaultConstructible
			{
				NotDefaultConstructible() = delete;
				explicit NotDefaultConstructible(int) {}
			};
			static_assert(!eastl::is_default_constructible<NotDefaultConstructible>::value, "is_default_constructible");

			using v_t = variant<float, NotDefaultConstructible>;
			v_t v;
			static_assert(eastl::is_default_constructible_v<v_t>, "is_default_constructible");
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
		using v_t = variant<int, short, char>;

		v_t v1 = 42;
		v_t v2 = v1;

		VERIFY(get<int>(v2) == get<int>(v1));
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

	// emplace<T>() is invalid if T matches more than one of the variant's alternative types,
	// but emplace<I>() is fine.
	{
		variant<int, int> v;

		v.emplace<0>(42);
		VERIFY(get<0>(v) == 42);

		v.emplace<1>(1337);
		VERIFY(get<1>(v) == 1337);
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

	{
		variant<TestObject> v1{ in_place_type<TestObject>, 1337 };
		variant<TestObject> v2{ in_place_type<TestObject>, 9001 };

		swap(v1, v2); // same alternative types; calls swap(TestObject&, TestObject&);

		VERIFY(get<TestObject>(v1).mX == 9001);
		VERIFY(get<TestObject>(v2).mX == 1337);
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_OCT) // This test fails in previous EASTL versions.
		VERIFY(TestObject::sTOSwapCount == 1);
#endif
		VERIFY(TestObject::sTODefaultCtorCount == 2);
		VERIFY(TestObject::sTOCopyCtorCount == 0);
	}
	VERIFY(TestObject::Reset());

	{
		variant<int, TestObject> v1{ in_place_type<int>, 1337 };
		variant<int, TestObject> v2{ in_place_type<TestObject>, 9001 };

		swap(v1, v2); // differing alternative types; swaps by move construction (no assignments).

		VERIFY(get<TestObject>(v1).mX == 9001);
		VERIFY(get<int>(v2) == 1337);
		VERIFY(TestObject::sTOSwapCount == 0);
		VERIFY(TestObject::sTODefaultCtorCount == 1);
		VERIFY(TestObject::sTOCopyCtorCount == 0);
		VERIFY(TestObject::sTOMoveCtorCount >= 1);
		VERIFY(TestObject::sTOCopyAssignCount == 0);
		VERIFY(TestObject::sTOMoveAssignCount == 0);
	}
	VERIFY(TestObject::Reset());

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
		variant<int, int> v(in_place_index<0>, 42);
		VERIFY(get<0>(v) == 42);
		VERIFY(v.index() == 0);
	}

	{
		variant<int, int> v(in_place_index<1>, 42);
		VERIFY(get<1>(v) == 42);
		VERIFY(v.index() == 1);
	}

	{
		variant<int, string> v(in_place_type<int>, 42);
		VERIFY(get<0>(v) == 42);
		VERIFY(v.index() == 0);
	}

	{
		variant<int, string> v(in_place_type<string>, "hello");
		VERIFY(get<1>(v) == "hello");
		VERIFY(v.index() == 1);
	}

	return nErrorCount;
}

// Many Compilers are smart and will fully inline the visitor in our unittests,
// Thereby not actually testing the recursive call.
EA_NO_INLINE int TestVariantVisitNoInline(const eastl::variant<int, bool, unsigned>& v)
{
	int nErrorCount = 0;

	bool bVisited = false;

	struct MyVisitor
	{
		MyVisitor() = delete;
		MyVisitor(bool& visited) : mVisited(visited) {};

		void operator()(int) { mVisited = true; }
		void operator()(bool) { mVisited = true; }
		void operator()(unsigned) { mVisited = true; }

		bool& mVisited;
	};

	eastl::visit(MyVisitor(bVisited), v);

	EATEST_VERIFY(bVisited);

	return nErrorCount;
}

EA_NO_INLINE int TestVariantVisit2NoInline(const eastl::variant<int, bool>& v0, const eastl::variant<int, bool>& v1)
{
	int nErrorCount = 0;

	bool bVisited = false;

	struct MyVisitor
	{
		MyVisitor() = delete;
		MyVisitor(bool& visited) : mVisited(visited) {};

		void operator()(int, int) { mVisited = true; }
		void operator()(bool, int) { mVisited = true; }
		void operator()(int, bool) { mVisited = true; }
		void operator()(bool, bool) { mVisited = true; }

		bool& mVisited;
	};

	eastl::visit(MyVisitor(bVisited), v0, v1);

	EATEST_VERIFY(bVisited);

	return nErrorCount;
}

EA_NO_INLINE int TestVariantVisit3tNoInline(const eastl::variant<int, bool>& v0, const eastl::variant<int, bool>& v1, const eastl::variant<int, bool>& v2)
{
	int nErrorCount = 0;

	bool bVisited = false;

	struct MyVisitor
	{
		MyVisitor() = delete;
		MyVisitor(bool& visited) : mVisited(visited) {};

		void operator()(int, int, int) { mVisited = true; }
		void operator()(bool, int, int) { mVisited = true; }
		void operator()(int, bool, int) { mVisited = true; }
		void operator()(bool, bool, int) { mVisited = true; }

		void operator()(int, int, bool) { mVisited = true; }
		void operator()(bool, int, bool) { mVisited = true; }
		void operator()(int, bool, bool) { mVisited = true; }
		void operator()(bool, bool, bool) { mVisited = true; }

		bool& mVisited;
	};

	eastl::visit(MyVisitor(bVisited), v0, v1, v2);

	EATEST_VERIFY(bVisited);

	return nErrorCount;
}

int TestVariantVisitorOverloaded()
{
	using namespace eastl;
	int nErrorCount = 0;

	using v_t = variant<int, string, double, long>;
	v_t arr[] = {42, "jean", 42.0, 42L};
	v_t v{42.0};


	#ifdef __cpp_deduction_guides
	{
		int count = 0;

		for (auto& e : arr)
		{
			eastl::visit(
				overloaded{
					[&](int)    { count++; },
					[&](string) { count++; },
					[&](double) { count++; },
			   		[&](long)   { count++; }},
				e
			);
		}

		VERIFY(count == EAArrayCount(arr));
	}

	{
		double visitedValue = 0.0f;

		eastl::visit(
			overloaded{
				[](int)    { },
				[](string) { },
				[&](double d) { visitedValue = d; },
				[](long)   { }},
			v
		);

		VERIFY(visitedValue == 42.0f);
	}

	#endif

	{
		int count = 0;

		for (auto& e : arr)
		{
			eastl::visit(
				eastl::make_overloaded(
					[&](int)    { count++; },
					[&](string) { count++; },
					[&](double) { count++; },
					[&](long)   { count++; }), 
				e
			);
		}

		VERIFY(count == EAArrayCount(arr));
	}

	{
		double visitedValue = 0.0f;

		eastl::visit(
			eastl::make_overloaded(
				[](int)    { },
				[](string) { },
				[&](double d) { visitedValue = d; },
				[](long)   { }),
			v
		);

		VERIFY(visitedValue == 42.0f);
	}

	return nErrorCount;
}

int TestVariantVisitor()
{
	using namespace eastl;
	int nErrorCount = 0;

	using v_t = variant<int, string, double, long>;

	{
		v_t arr[] = {42, "hello", 42.0, 42L};

		int count = 0;
		for (auto& e : arr)
		{
			eastl::visit([&](auto){ count++; }, e);
		}

		VERIFY(count == EAArrayCount(arr));

		count = 0;
		for (auto& e : arr)
		{
			eastl::visit<void>([&](auto){ count++; }, e);
		}

		VERIFY(count == EAArrayCount(arr));
	}

	{
		static bool bVisited = false;

		variant<int, long, string> v = 42;

		struct MyVisitor
		{
			void operator()(int)      { bVisited = true; };
			void operator()(long)     { };
			void operator()(string)   { };
			void operator()(unsigned) { }; // not in variant
		};

		visit(MyVisitor{}, v);
		VERIFY(bVisited);

		bVisited = false;

		visit<void>(MyVisitor{}, v);
		VERIFY(bVisited);
	}

	{
		static bool bVisited = false;

		variant<int, bool, unsigned> v = (int)1;

		struct MyVisitor
		{
			bool& operator()(int) { return bVisited; }
			bool& operator()(bool) { return bVisited; }
			bool& operator()(unsigned) { return bVisited; }
		};

		bool& ret = visit(MyVisitor{}, v);
		ret = true;
		VERIFY(bVisited);

		bVisited = false;
		bool& ret2 = visit<bool&>(MyVisitor{}, v);
		ret2 = true;
		VERIFY(bVisited);
	}

	{
		variant<int, bool, unsigned> v = (int)1;

		struct MyVisitor
		{
			void operator()(int& i) { i = 2; }
			void operator()(bool&) {}
			void operator()(unsigned&) {}
		};

		visit(MyVisitor{}, v);
		EATEST_VERIFY(get<0>(v) == (int)2);

		v = (int)1;
		visit<void>(MyVisitor{}, v);
		EATEST_VERIFY(get<0>(v) == (int)2);
	}

	{
		static bool bVisited = false;

		variant<int, bool, unsigned> v =(int)1;

		struct MyVisitor
		{
			void operator()(const int&) { bVisited = true; }
			void operator()(const bool&) {}
			void operator()(const unsigned&) {}
		};

		visit(MyVisitor{}, v);
		EATEST_VERIFY(bVisited);

		bVisited = false;
		visit<void>(MyVisitor{}, v);
		EATEST_VERIFY(bVisited);
	}

	{
		static bool bVisited = false;

		const variant<int, bool, unsigned> v =(int)1;

		struct MyVisitor
		{
			void operator()(const int&) { bVisited = true; }
			void operator()(const bool&) {}
			void operator()(const unsigned&) {}
		};

		visit(MyVisitor{}, v);
		EATEST_VERIFY(bVisited);

		bVisited = false;
		visit<void>(MyVisitor{}, v);
		EATEST_VERIFY(bVisited);
	}

	{
		static bool bVisited = false;

		struct MyVisitor
		{
			void operator()(int&&) { bVisited = true; }
			void operator()(bool&&) {}
			void operator()(unsigned&&) {}
		};

		visit(MyVisitor{}, variant<int, bool, unsigned>{(int)1});
		EATEST_VERIFY(bVisited);

		visit<void>(MyVisitor{}, variant<int, bool, unsigned>{(int)1});
		EATEST_VERIFY(bVisited);
	}

	{
		static bool bVisited = false;

		variant<int, bool, unsigned> v = (int)1;

		struct MyVisitor
		{
			bool&& operator()(int) { return eastl::move(bVisited); }
			bool&& operator()(bool) { return eastl::move(bVisited); }
			bool&& operator()(unsigned) { return eastl::move(bVisited); }
		};

		bool&& ret = visit(MyVisitor{}, v);
		ret = true;
		VERIFY(bVisited);

		bVisited = false;
		bool&& ret2 = visit<bool&&>(MyVisitor{}, v);
		ret2 = true;
		VERIFY(bVisited);
	}

	{
		variant<int, bool, unsigned> v = (int)1;

		TestVariantVisitNoInline(v);
		v = (bool)true;
		TestVariantVisitNoInline(v);
		v = (int)3;
		TestVariantVisitNoInline(v);
	}

	{
		variant<int, bool> v0 = (int)1;
		variant<int, bool> v1 = (bool)true;

		TestVariantVisit2NoInline(v0, v1);
		v0 = (bool)false;
		TestVariantVisit2NoInline(v0, v1);
		v1 = (int)2;
		TestVariantVisit2NoInline(v0, v1);
	}

	{
		variant<int, bool> v0 = (int)1;
		variant<int, bool> v1 = (int)2;
		variant<int, bool> v2 = (int)3;

		TestVariantVisit3tNoInline(v0, v1, v2);
		v2 = (bool)false;
		TestVariantVisit3tNoInline(v0, v1, v2);
		v0 = (bool)true;
		TestVariantVisit3tNoInline(v0, v1, v2);
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

		MultipleVisitor& ret = visit(MultipleVisitor{}, i, s);
		EA_UNUSED(ret);
		VERIFY(bVisited);

		MultipleVisitor& ret2 = visit<MultipleVisitor&>(MultipleVisitor{}, i, s);
		EA_UNUSED(ret2);
		VERIFY(bVisited);
	}

	{
		bool bVisited = false;

		variant<int, bool> v0 = 0;
		variant<int, bool> v1 = 1;

		struct MultipleVisitor
		{
			MultipleVisitor() = delete;
			MultipleVisitor(bool& visited) : mVisited(visited) {};

			void operator()(int, int) { mVisited = true; }
			void operator()(int, bool) {}
			void operator()(bool, int) {}
			void operator()(bool, bool) {}

			bool& mVisited;
		};

		visit(MultipleVisitor(bVisited), v0, v1);
		EATEST_VERIFY(bVisited);

		bVisited = false;
		visit<void>(MultipleVisitor(bVisited), v0, v1);
		EATEST_VERIFY(bVisited);
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

	return nErrorCount;
}

int TestVariantVisitorReturn()
{
	int nErrorCount = 0;

	{
		static bool bVisited = false;

		eastl::variant<int, bool> v = (int)1;

		struct MyVisitor
		{
			bool operator()(int) { bVisited = true; return true; }
			bool operator()(bool) { return false; }
		};

		eastl::visit(MyVisitor{}, v);
		EATEST_VERIFY(bVisited);
		bVisited = false;
		eastl::visit<void>(MyVisitor{}, v); // using the C++20 overload with explicit return type
		EATEST_VERIFY(bVisited);
	}

	{
		static bool bVisited = false;

		eastl::variant<int, bool> v = (int)1;

		struct MyVisitor
		{
			bool operator()(int) { bVisited = true; return true; }
			bool operator()(bool) { return false; }
		};

		eastl::visit(MyVisitor{}, v);
		EATEST_VERIFY(bVisited);
		bVisited = false;
		eastl::visit<const void>(MyVisitor{}, v);
		EATEST_VERIFY(bVisited);
	}

	{
		static bool bVisited = false;

		eastl::variant<int, bool> v = (int)1;

		struct MyVisitor
		{
			bool operator()(int) { bVisited = true; return true; }
			bool operator()(bool) { return false; }
		};

		eastl::visit(MyVisitor{}, v);
		EATEST_VERIFY(bVisited);
		bVisited = false;
		eastl::visit<void>(MyVisitor{}, v);
		EATEST_VERIFY(bVisited);
	}

	{
		static bool bVisited = false;

		eastl::variant<int, bool> v = (int)1;

		struct MyVisitor
		{
			bool operator()(int) { bVisited = true; return true; }
			bool operator()(bool) { return false; }
		};

		eastl::visit(MyVisitor{}, v);
		EATEST_VERIFY(bVisited);
		bVisited = false;
		eastl::visit<const void>(MyVisitor{}, v);
		EATEST_VERIFY(bVisited);
	}

	{
		static bool bVisited = false;

		eastl::variant<int, bool> v = (int)1;

		struct MyVisitor
		{
			bool operator()(int) { bVisited = true; return true; }
			bool operator()(bool) { return false; }
		};

		bool ret = eastl::visit(MyVisitor{}, v);
		EATEST_VERIFY(bVisited);
		EATEST_VERIFY(ret);
		bVisited = false;
		int int_ret = eastl::visit<int>(MyVisitor{}, v);
		EATEST_VERIFY(bVisited);
		EATEST_VERIFY(int_ret);
	}

	{
		static bool bVisited = false;

		struct A {};
		struct B : public A {};
		struct C : public A {};

		eastl::variant<int, bool> v = (int)1;

		struct MyVisitor
		{
			B operator()(int) { bVisited = true; return B{}; }
			C operator()(bool) { return C{}; }
		};

		// ambiguity requires specifying the visit return type:
		A ret = eastl::visit<A>(MyVisitor{}, v);
		EA_UNUSED(ret);
		EATEST_VERIFY(bVisited);
	}

	{
		static bool bVisited = false;

		eastl::variant<int, bool> v = (int)1;

		struct MyVisitor
		{
			MyVisitor operator()(int) { bVisited = true; return MyVisitor{}; }
			MyVisitor operator()(bool) { return MyVisitor{}; }
		};

		// specifying the return type with visit<MyVisitor>() would result in overload ambiguity. could be either:
		// visit<MyVisitor, variant<int, bool>>()					[ie. Visitor, Variants...]
		// visit<MyVisitor, MyVisitor, variant<int, bool>>()		[ie. Return, Visitor, Variants...]
		// calling without explicit template parameters uses the first overload.
		MyVisitor ret = eastl::visit(MyVisitor{}, v);
		EA_UNUSED(ret);
		EATEST_VERIFY(bVisited);
	}

	return nErrorCount;
}

int TestVariantVisitBase()
{
	int nErrorCount = 0;

	struct Base { virtual ~Base() = default; };
	struct Derived : public Base {};

	struct MyVisitor
	{
		int operator()(const Base&) { return 1; }
		int operator()(int) { return 2; }
		int operator()(const eastl::string&) { return 3; }
	};

	eastl::variant<Derived, int, eastl::string> v;

	v.emplace<Derived>();
	EATEST_VERIFY(eastl::visit(MyVisitor{}, v) == 1);

	v.emplace<int>();
	EATEST_VERIFY(eastl::visit(MyVisitor{}, v) == 2);

	v.emplace<eastl::string>("my string");
	EATEST_VERIFY(eastl::visit(MyVisitor{}, v) == 3);

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

	{
		variant<int, ImplicitlyConvertible> v(ImplicitlyConvertible{});
		v = ImplicitlyConvertible::implicit; // converting assignment; calls ImplicitlyConvertible move assignment
		VERIFY(v.index() == 1);
		VERIFY(ImplicitlyConvertible::sDefaultCtorCount == 1);
		VERIFY(ImplicitlyConvertible::sConvertCtorCount == 1);
		VERIFY(ImplicitlyConvertible::sCopyAssignCount == 0);
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_OCT) // This test fails in previous EASTL versions.
		VERIFY(ImplicitlyConvertible::sMoveAssignCount == 1);
#endif
		ImplicitlyConvertible::Reset();
	}

	{
		variant<int, ImplicitlyConvertible> v(0); // starts as an int
		v = ImplicitlyConvertible::implicit; // converting assignment; calls ImplicitlyConvertible converting constructor
		VERIFY(v.index() == 1);
		VERIFY(ImplicitlyConvertible::sDefaultCtorCount == 0);
		VERIFY(ImplicitlyConvertible::sConvertCtorCount == 1);
		VERIFY(ImplicitlyConvertible::sCopyAssignCount == 0); // should be emplaced
		VERIFY(ImplicitlyConvertible::sMoveAssignCount == 0); // should be emplaced
		ImplicitlyConvertible::Reset();
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
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_OCT) // This test fails in previous EASTL versions.
			VERIFY(TestObject::sTOCopyAssignCount == 1);
#endif
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

int TestVariantRelationalOperators()
{
	int nErrorCount = 0;

	using VariantNoThrow = eastl::variant<int, bool, float>;

	// Equality
	{
		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ true };

			EATEST_VERIFY((v1 == v2) == false);
		}

		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ (int)1 };

			EATEST_VERIFY((v1 == v2) == true);
		}

		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ (int)0 };

			EATEST_VERIFY((v1 == v2) == false);
		}
	}

	// Inequality
	{
		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ true };

			EATEST_VERIFY((v1 != v2) == true);
		}

		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ (int)1 };

			EATEST_VERIFY((v1 != v2) == false);
		}

		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ (int)0 };

			EATEST_VERIFY((v1 != v2) == true);
		}
	}

	// Less Than
	{
		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ true };

			EATEST_VERIFY((v1 < v2) == true);
		}

		{
			VariantNoThrow v1{ true };
			VariantNoThrow v2{ (int)1 };

			EATEST_VERIFY((v1 < v2) == false);
		}

		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ (int)1 };

			EATEST_VERIFY((v1 < v2) == false);
		}

		{
			VariantNoThrow v1{ (int)0 };
			VariantNoThrow v2{ (int)1 };

			EATEST_VERIFY((v1 < v2) == true);
		}
	}

	// Greater Than
	{
		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ true };

			EATEST_VERIFY((v1 > v2) == false);
		}

		{
			VariantNoThrow v1{ true };
			VariantNoThrow v2{ (int)1 };

			EATEST_VERIFY((v1 > v2) == true);
		}

		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ (int)1 };

			EATEST_VERIFY((v1 > v2) == false);
		}

		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ (int)0 };

			EATEST_VERIFY((v1 > v2) == true);
		}
	}

	// Less Equal
	{
		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ true };

			EATEST_VERIFY((v1 <= v2) == true);
		}

		{
			VariantNoThrow v1{ true };
			VariantNoThrow v2{ (int)1 };

			EATEST_VERIFY((v1 <= v2) == false);
		}

		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ (int)1 };

			EATEST_VERIFY((v1 <= v2) == true);
		}

		{
			VariantNoThrow v1{ (int)0 };
			VariantNoThrow v2{ (int)1 };

			EATEST_VERIFY((v1 <= v2) == true);
		}

		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ (int)0 };

			EATEST_VERIFY((v1 <= v2) == false);
		}
	}

	// Greater Equal
	{
		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ true };

			EATEST_VERIFY((v1 >= v2) == false);
		}

		{
			VariantNoThrow v1{ true };
			VariantNoThrow v2{ (int)1 };

			EATEST_VERIFY((v1 >= v2) == true);
		}

		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ (int)1 };

			EATEST_VERIFY((v1 >= v2) == true);
		}

		{
			VariantNoThrow v1{ (int)0 };
			VariantNoThrow v2{ (int)1 };

			EATEST_VERIFY((v1 >= v2) == false);
		}

		{
			VariantNoThrow v1{ (int)1 };
			VariantNoThrow v2{ (int)0 };

			EATEST_VERIFY((v1 >= v2) == true);
		}
	}

#if EASTL_EXCEPTIONS_ENABLED

	using VariantThrow = eastl::variant<int, bool, float>;

	auto make_variant_valueless = [](VariantThrow& v)
								  {
									  try
									  {
										  v.emplace<0>(valueless_struct<int>{});
									  }
									  catch(const typename valueless_struct<int>::exception_tag &)
									  {
									  }
								  };

	// Equality
	{
		{
			VariantThrow v0{ (int)0 };
			VariantThrow v1{ (int)1 };

			make_variant_valueless(v0);
			make_variant_valueless(v1);

			EATEST_VERIFY((v0 == v1) == true);
		}
	}

	// Inequality
	{
		{
			VariantThrow v0{ (int)0 };
			VariantThrow v1{ (int)1 };

			make_variant_valueless(v0);
			make_variant_valueless(v1);

			EATEST_VERIFY((v0 != v1) == false);
		}
	}

	// Less Than
	{
		{
			VariantThrow v0{ (int)0 };
			VariantThrow v1{ (int)1 };

			make_variant_valueless(v0);

			EATEST_VERIFY((v0 < v1) == true);
		}

		{
			VariantThrow v0{ (int)0 };
			VariantThrow v1{ (int)1 };

			make_variant_valueless(v1);

			EATEST_VERIFY((v0 < v1) == false);
		}
	}

	// Greater Than
	{
		{
			VariantThrow v0{ (int)1 };
			VariantThrow v1{ (int)0 };

			make_variant_valueless(v0);

			EATEST_VERIFY((v0 > v1) == false);
		}

		{
			VariantThrow v0{ (int)1 };
			VariantThrow v1{ (int)0 };

			make_variant_valueless(v1);

			EATEST_VERIFY((v0 > v1) == true);
		}
	}

	// Less Equal
	{
		{
			VariantThrow v0{ (int)1 };
			VariantThrow v1{ (int)1 };

			make_variant_valueless(v0);

			EATEST_VERIFY((v0 <= v1) == true);
		}

		{
			VariantThrow v0{ (int)1 };
			VariantThrow v1{ (int)0 };

			make_variant_valueless(v1);

			EATEST_VERIFY((v0 <= v1) == false);
		}
	}

	// Greater Equal
	{
		{
			VariantThrow v0{ (int)1 };
			VariantThrow v1{ (int)1 };

			make_variant_valueless(v0);

			EATEST_VERIFY((v0 >= v1) == false);
		}

		{
			VariantThrow v0{ (int)1 };
			VariantThrow v1{ (int)0 };

			make_variant_valueless(v1);

			EATEST_VERIFY((v0 >= v1) == true);
		}
	}

	// get()
	{
		VariantThrow v1{ (int)1 };
		EATEST_VERIFY_THROW(eastl::get<bool>(v1));

		VariantThrow v2{ (int)1 };
		EATEST_VERIFY_THROW(eastl::get<1>(v2));
	}

#endif

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

#define EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(Type, VarName)			\
	bool operator==(const Type & rhs) const { return VarName == rhs.VarName; } \
	bool operator!=(const Type & rhs) const { return VarName != rhs.VarName; } \
	bool operator<(const Type & rhs) const { return VarName < rhs.VarName; } \
	bool operator>(const Type & rhs) const { return VarName > rhs.VarName; } \
	bool operator<=(const Type & rhs) const { return VarName <= rhs.VarName; } \
	bool operator>=(const Type & rhs) const { return VarName >= rhs.VarName; }

int TestBigVariantComparison()
{
	int nErrorCount = 0;

	struct A;
	struct B;
	struct C;
	struct D;
	struct E;
	struct F;
	struct G;
	struct H;
	struct I;
	struct J;
	struct K;
	struct L;
	struct M;
	struct N;
	struct O;
	struct P;
	struct Q;
	struct R;
	struct S;
	struct T;
	struct U;
	struct V;
	struct W;
	struct X;
	struct Y;
	struct Z;

	using BigVariant = eastl::variant<A, B, C, D, E, F, G, H, I, J, K, L, M, N,
									  O, P, Q, R, S, T, U, V, W, X, Y, Z>;

	struct A { int a; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(A, a) };
	struct B { int b; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(B, b) };
	struct C { int c; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(C, c) };
	struct D { int d; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(D, d) };
	struct E { int e; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(E, e) };
	struct F { int f; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(F, f) };
	struct G { int g; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(G, g) };
	struct H { int h; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(H, h) };
	struct I { int i; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(I, i) };
	struct J { int j; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(J, j) };
	struct K { int k; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(K, k) };
	struct L { int l; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(L, l) };
	struct M { int m; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(M, m) };
	struct N { int n; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(N, n) };
	struct O { int o; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(O, o) };
	struct P { int p; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(P, p) };
	struct Q { int q; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(Q, q) };
	struct R { int r; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(R, r) };
	struct S { int s; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(S, s) };
	struct T { int t; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(T, t) };
	struct U { int u; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(U, u) };
	struct V { int v; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(V, v) };
	struct W { int w; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(W, w) };
	struct X { int x; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(X, x) };
	struct Y { int y; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(Y, y) };
	struct Z { int z; EASTL_TEST_BIG_VARIANT_RELATIONAL_OPS(Z, z) };

	{
		BigVariant v0{ A{0} };
		BigVariant v1{ A{1} };

		VERIFY(v0 != v1);
	}

	{
		BigVariant v0{ A{0} };
		BigVariant v1{ A{1} };

		VERIFY(v0 < v1);
	}

	{
		BigVariant v0{ A{0} };
		BigVariant v1{ A{0} };

		VERIFY(v0 == v1);
	}

	{
		BigVariant v0{ A{1} };
		BigVariant v1{ A{0} };

		VERIFY(v0 > v1);
	}

	{
		BigVariant v0{ A{0} };
		BigVariant v1{ A{1} };

		VERIFY(v0 <= v1);
	}

	{
		BigVariant v0{ A{0} };
		BigVariant v1{ A{0} };

		VERIFY(v0 <= v1);
	}

	{
		BigVariant v0{ A{0} };
		BigVariant v1{ A{0} };

		VERIFY(v0 >= v1);
	}

	{
		BigVariant v0{ A{1} };
		BigVariant v1{ A{0} };

		VERIFY(v0 >= v1);
	}

	{
		BigVariant v0{ A{0} };
		BigVariant v1{ B{0} };

		VERIFY(v0 != v1);
	}

	{
		BigVariant v0{ A{0} };
		BigVariant v1{ B{0} };

		VERIFY(v0 < v1);
	}

	{
		BigVariant v0{ A{0} };
		BigVariant v1{ B{0} };

		VERIFY(v1 > v0);
	}

	return nErrorCount;
}

int TestVariantGeneratingComparisonOverloads();

// test special member functions are enabled/disabled based on the contained types.
int TestSpecialMembersDisabled()
{
	int nErrorCount = 0;

	using namespace eastl;

	// test copy construction
	// copy constructor is not enabled unless Ts are copy constructible.
	// copy constructor is trivial if Ts are trivially copy constructible.
	{
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		static_assert(!is_copy_constructible_v<variant<NoCopyMove>>, "!copy constructible");
		static_assert(!is_copy_constructible_v<variant<MoveOnlyType>>, "!copy constructible");
		static_assert(!is_copy_constructible_v<variant<MoveOnlyTypeDefaultCtor>>, "!copy constructible");
#endif

		static_assert(is_copy_constructible_v<variant<NonTriviallyCopyable>>, "copy constructible");
		static_assert(!is_trivially_copy_constructible_v<variant<NonTriviallyCopyable>>, "!trivially copy constructible");
		static_assert(is_copy_constructible_v<variant<NonTriviallyCopyable, NonTriviallyCopyable>>, "copy constructible");
		static_assert(!is_trivially_copy_constructible_v<variant<NonTriviallyCopyable, NonTriviallyCopyable>>, "!trivially copy constructible");

#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		static_assert(is_trivially_copy_constructible_v<variant<TriviallyCopyableWithCopy>>, "trivially copy constructible");
		static_assert(is_trivially_copy_constructible_v<variant<TriviallyCopyableWithCopy, TriviallyCopyableWithCopy>>, "trivially copy constructible");
		static_assert(is_trivially_copy_constructible_v<variant<TriviallyCopyableWithCopyCtor>>, "trivially copy constructible");
		static_assert(is_trivially_copy_constructible_v<variant<TriviallyCopyableWithCopyCtor, TriviallyCopyableWithCopyCtor>>, "trivially copy constructible");

		// trivially copy constructible types, but with a copy constructor deleted
		static_assert(!is_copy_constructible_v<variant<TriviallyCopyableWithMove>>, "!copy constructible");
		static_assert(!is_copy_constructible_v<variant<TriviallyCopyableWithCopyAssign>>, "!copy constructible");
		static_assert(!is_copy_constructible_v<variant<TriviallyCopyableWithMoveCtor>>, "!copy constructible");
		static_assert(!is_copy_constructible_v<variant<TriviallyCopyableWithMoveAssign>>, "!copy constructible");
#endif
	}

	// test move construction
	// copy constructor is not enabled unless Ts are *nothrow* move constructible.
	// move constructor is trivial if Ts are trivially move constructible.
	{
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		static_assert(!is_move_constructible_v<variant<NoCopyMove>>, "!move constructible");
#endif

		static_assert(is_move_constructible_v<variant<MoveOnlyType>>, "move constructible");
		static_assert(!is_trivially_move_constructible_v<variant<MoveOnlyType>>, "!trivially move constructible");
		static_assert(is_move_constructible_v<variant<MoveOnlyTypeDefaultCtor>>, "move constructible");
		static_assert(!is_trivially_move_constructible_v<variant<MoveOnlyTypeDefaultCtor>>, "!trivially move constructible");

		static_assert(is_move_constructible_v<variant<NonTriviallyCopyable>>, "move constructible");
		static_assert(!is_trivially_move_constructible_v<variant<NonTriviallyCopyable>>, "!trivially move constructible");
		static_assert(is_move_constructible_v<variant<NonTriviallyCopyable, NonTriviallyCopyable>>, "move constructible");
		static_assert(!is_trivially_move_constructible_v<variant<NonTriviallyCopyable, NonTriviallyCopyable>>, "!trivially move constructible");

#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		static_assert(is_trivially_move_constructible_v<variant<TriviallyCopyableWithMove>>, "trivially move constructible");
		static_assert(is_trivially_move_constructible_v<variant<TriviallyCopyableWithMove, TriviallyCopyableWithMove>>, "trivially move constructible");
		static_assert(is_trivially_move_constructible_v<variant<TriviallyCopyableWithMoveCtor>>, "trivially move constructible");
		static_assert(is_trivially_move_constructible_v<variant<TriviallyCopyableWithMoveCtor, TriviallyCopyableWithMoveCtor>>, "trivially move constructible");

		// trivially move constructible types, but with a move constructor deleted
		static_assert(!is_move_constructible_v<variant<TriviallyCopyableWithCopyAssign>>, "!move constructible");
		static_assert(!is_move_constructible_v<variant<TriviallyCopyableWithMoveAssign>>, "!move constructible");

		// move constructor is deleted, but type trait passes because copy constructor exists
		static_assert(is_trivially_copy_constructible_v<variant<TriviallyCopyableWithCopy>>, "trivially copy constructible");
		static_assert(is_trivially_move_constructible_v<variant<TriviallyCopyableWithCopy>>, "trivially move constructible");
		static_assert(is_trivially_copy_constructible_v<variant<TriviallyCopyableWithCopyCtor>>, "trivially copy constructible");
		static_assert(is_trivially_move_constructible_v<variant<TriviallyCopyableWithCopyCtor>>, "trivially move constructible");
#endif
	}

	// test copy assignment
	// copy assignment is not enabled unless Ts are both copy constructible and assignable.
	{
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		static_assert(!is_copy_assignable_v<variant<NoCopyMove>>, "!copy assignable");
		static_assert(!is_copy_assignable_v<variant<MoveOnlyType>>, "!copy assignable");
		static_assert(!is_copy_assignable_v<variant<MoveOnlyTypeDefaultCtor>>, "!copy assignable");
#endif

		static_assert(is_copy_assignable_v<variant<NonTriviallyCopyable>>, "copy assignable");
		static_assert(!is_trivially_copy_assignable_v<variant<NonTriviallyCopyable>>, "!trivially copy assignable");
		static_assert(is_copy_assignable_v<variant<NonTriviallyCopyable, NonTriviallyCopyable>>, "copy assignable");
		static_assert(!is_trivially_copy_assignable_v<variant<NonTriviallyCopyable, NonTriviallyCopyable>>, "!trivially copy assignable");

#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		static_assert(is_trivially_copy_assignable_v<variant<TriviallyCopyableWithCopy>>, "trivially copy assignable");
		static_assert(is_trivially_copy_assignable_v<variant<TriviallyCopyableWithCopy, TriviallyCopyableWithCopy>>, "trivially copy assignable");

		// types that are missing either a copy constructor or copy assignment.
		static_assert(!is_copy_assignable_v<variant<TriviallyCopyableWithCopyCtor>>, "!copy assignable");
		static_assert(!is_copy_assignable_v<variant<TriviallyCopyableWithCopyAssign>>, "!copy assignable");
		static_assert(!is_copy_assignable_v<variant<TriviallyCopyableWithMove>>, "!copy assignable");
		static_assert(!is_copy_assignable_v<variant<TriviallyCopyableWithMoveCtor>>, "!copy assignable");
		static_assert(!is_copy_assignable_v<variant<TriviallyCopyableWithMoveAssign>>, "!copy assignable");
#endif
	}

	// test move assignment
	// move assignment is not enabled unless Ts are both move constructible and assignable.
	{
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		static_assert(!is_move_assignable_v<variant<NoCopyMove>>, "!move assignable");
#endif

		static_assert(is_move_assignable_v<variant<MoveOnlyType>>, "move assignable");
		static_assert(!is_trivially_move_assignable_v<variant<MoveOnlyType>>, "!trivially move assignable");
		static_assert(is_move_assignable_v<variant<MoveOnlyTypeDefaultCtor>>, "move assignable");
		static_assert(!is_trivially_move_assignable_v<variant<MoveOnlyTypeDefaultCtor>>, "!trivially move assignable");

		static_assert(is_move_assignable_v<variant<NonTriviallyCopyable>>, "move assignable"); // invokes copy assignment. therefore true.
		static_assert(!is_trivially_move_assignable_v<variant<NonTriviallyCopyable>>, "!trivially move assignable");
		static_assert(is_move_assignable_v<variant<NonTriviallyCopyable, NonTriviallyCopyable>>, "move assignable");
		static_assert(!is_trivially_move_assignable_v<variant<NonTriviallyCopyable, NonTriviallyCopyable>>, "!trivially move assignable");

#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		static_assert(is_trivially_move_assignable_v<variant<TriviallyCopyableWithCopy>>, "trivially move assignable");
		static_assert(is_trivially_move_assignable_v<variant<TriviallyCopyableWithCopy, TriviallyCopyableWithCopy>>, "trivially move assignable");
		static_assert(is_trivially_move_assignable_v<variant<TriviallyCopyableWithMove>>, "move assignable");
		static_assert(is_trivially_move_assignable_v<variant<TriviallyCopyableWithMove, TriviallyCopyableWithMove>>, "trivially move assignable");
		
		// types that are missing either a move constructor or move assignment.
		static_assert(!is_move_assignable_v<variant<TriviallyCopyableWithCopyCtor>>, "!move assignable");
		static_assert(!is_move_assignable_v<variant<TriviallyCopyableWithCopyAssign>>, "!move assignable");
		static_assert(!is_move_assignable_v<variant<TriviallyCopyableWithMoveCtor>>, "!move assignable");
		static_assert(!is_move_assignable_v<variant<TriviallyCopyableWithMoveAssign>>, "!move assignable");
#endif
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
	nErrorCount += TestVariantVisitorOverloaded();
	nErrorCount += TestVariantVisitor();
	nErrorCount += TestVariantVisitorReturn();
	nErrorCount += TestVariantVisitBase();
	nErrorCount += TestVariantAssignment();
	nErrorCount += TestVariantMoveOnly();
	nErrorCount += TestVariantUserRegressionCopyMoveAssignmentOperatorLeak();
	nErrorCount += TestVariantUserRegressionIncompleteType();
	nErrorCount += TestVariantGeneratingComparisonOverloads();
	nErrorCount += TestBigVariantComparison();
	nErrorCount += TestVariantRelationalOperators();
	nErrorCount += TestSpecialMembersDisabled();

	return nErrorCount;
}
#else
	int TestVariant() { return 0; }
#endif

EA_RESTORE_VC_WARNING();
