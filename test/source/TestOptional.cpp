/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EASTL/type_traits.h>
#include <EASTL/sort.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/optional.h>


/////////////////////////////////////////////////////////////////////////////
struct IntStruct
{
	IntStruct(int in) : data(in) {}
	int data;
};

bool operator<(const IntStruct& lhs, const IntStruct& rhs)
	{ return lhs.data < rhs.data; }
bool operator==(const IntStruct& lhs, const IntStruct& rhs)
	{ return lhs.data == rhs.data; }


/////////////////////////////////////////////////////////////////////////////
struct destructor_test
{
	~destructor_test() { destructor_ran = true; }
	static bool destructor_ran;
	static void reset() { destructor_ran = false; }
};
bool destructor_test::destructor_ran = false;


/////////////////////////////////////////////////////////////////////////////
// TestOptional
//
int TestOptional()
{
	using namespace eastl;
	int nErrorCount = 0;
	#if defined(EASTL_OPTIONAL_ENABLED) && EASTL_OPTIONAL_ENABLED
	{
		{
			VERIFY( (is_same<optional<int>::value_type, int>::value));
			VERIFY( (is_same<optional<short>::value_type, short>::value));
			VERIFY(!(is_same<optional<short>::value_type, long>::value));
			VERIFY( (is_same<optional<const short>::value_type, const short>::value));
			VERIFY( (is_same<optional<volatile short>::value_type, volatile short>::value));
			VERIFY( (is_same<optional<const volatile short>::value_type, const volatile short>::value));

			VERIFY(is_empty<nullopt_t>::value);
			#if EASTL_TYPE_TRAIT_is_literal_type_CONFORMANCE
				VERIFY(is_literal_type<nullopt_t>::value);
			#endif

			#if EASTL_TYPE_TRAIT_is_trivially_destructible_CONFORMANCE
				VERIFY(is_trivially_destructible<int>::value);
				VERIFY(is_trivially_destructible<Internal::optional_storage<int>>::value);
				VERIFY(is_trivially_destructible<optional<int>>::value);
				VERIFY(is_trivially_destructible<optional<int>>::value == is_trivially_destructible<int>::value);
			#endif

			{
				struct NotTrivialDestructible { ~NotTrivialDestructible() {} };
				VERIFY(!is_trivially_destructible<NotTrivialDestructible>::value);
				VERIFY(!is_trivially_destructible<optional<NotTrivialDestructible>>::value);
				VERIFY(!is_trivially_destructible<Internal::optional_storage<NotTrivialDestructible>>::value);
				VERIFY(is_trivially_destructible<optional<NotTrivialDestructible>>::value == is_trivially_destructible<NotTrivialDestructible>::value);
			}
		}

		{
			optional<int> o;  
			VERIFY(!o);
			VERIFY(o.value_or(0x8BADF00D) == (int)0x8BADF00D);
			o = 1024;
			VERIFY(static_cast<bool>(o));
			VERIFY(o.value_or(0x8BADF00D) == 1024);
			VERIFY(o.value() == 1024);
			
			// Test reset
			o.reset();
			VERIFY(!o);
			VERIFY(o.value_or(0x8BADF00D) == (int)0x8BADF00D);
		}

		{
			optional<int> o(nullopt);  
			VERIFY(!o);
			VERIFY(o.value_or(0x8BADF00D) == (int)0x8BADF00D);
		}

		{
			optional<int> o = {};  
			VERIFY(!o);
			VERIFY(o.value_or(0x8BADF00D) == (int)0x8BADF00D);
		}

		{
			optional<int> o(42);  
			VERIFY(bool(o));
			VERIFY(o.value_or(0x8BADF00D) == 42);
			o = nullopt;
			VERIFY(!o);
			VERIFY(o.value_or(0x8BADF00D) == (int)0x8BADF00D);
		}

		{
			optional<int> o(42);
			VERIFY(static_cast<bool>(o));
			VERIFY(o.value_or(0x8BADF00D) == 42);
			VERIFY(o.value() == 42);
		}

		{
			auto o = make_optional(42);
			VERIFY((is_same<decltype(o), optional<int>>::value));
			VERIFY(static_cast<bool>(o));
			VERIFY(o.value_or(0x8BADF00D) == 42);
			VERIFY(o.value() == 42);
		}

		{
			// test make_optional stripping refs/cv-qualifers
			int a = 42;
			const volatile int& intRef = a;
			auto o = make_optional(intRef);
			VERIFY((is_same<decltype(o)::value_type, int>::value));
		}

		{
			optional<int> o1(42), o2(24);
			VERIFY(o1.value() == 42);
			VERIFY(o2.value() == 24);
			VERIFY(*o1 == 42);
			VERIFY(*o2 == 24);
			o1 = eastl::move(o2);
			VERIFY(*o2 == 24);
			VERIFY(*o1 == 24);
			VERIFY(o2.value() == 24);
			VERIFY(o1.value() == 24);
			VERIFY(bool(o1));
			VERIFY(bool(o2));
		}

		{
			struct local { int payload; };
			optional<local> o = local{ 42 }; 
			VERIFY(o->payload == 42);
		}

		{
			struct local
			{
				int test() const { return 42; }
			};

			{
				const optional<local> o = local{};
				VERIFY(o->test() == 42);
				VERIFY((*o).test() == 42);
				VERIFY(o.value().test() == 42);
				VERIFY(bool(o));
			}

			{
				optional<local> o = local{};
				VERIFY(bool(o));
				o = nullopt;
				VERIFY(!bool(o));

				VERIFY(o.value_or(local{}).test() == 42);
				VERIFY(!bool(o));
			}
		}
	}

	#if EASTL_VARIADIC_TEMPLATES_ENABLED 
	{
		struct vec3
		{
			vec3(std::initializer_list<float> ilist) { auto* p = ilist.begin();  x = *p++; y = *p++; z = *p++; }
			vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}  // testing variadic template constructor overload 
			float x = 0, y = 0, z = 0;
		};

		{
			optional<vec3> o{ in_place, 4.f, 5.f, 6.f };
			VERIFY(o->x == 4 && o->y == 5 && o->z == 6);
		}

		{
			optional<vec3> o{ in_place, {4.f, 5.f, 6.f} };
			VERIFY(o->x == 4 && o->y == 5 && o->z == 6);
		}

		{
			optional<string> o(in_place, {'a', 'b', 'c'});
			VERIFY(o == string("abc"));
		}

		// http://en.cppreference.com/w/cpp/utility/optional/emplace
		{
			optional<vec3> o;
			o.emplace(42.f, 42.f, 42.f);
			VERIFY(o->x == 42.f && o->y == 42.f && o->z == 42.f);
		}

		{
			optional<vec3> o;
			o.emplace({42.f, 42.f, 42.f});
			VERIFY(o->x == 42.f && o->y == 42.f && o->z == 42.f);
		}

		{
			optional<int> o;
			o.emplace(42);
			VERIFY(*o == 42);
		}
	}
	#endif


	// swap
	{
		{
			optional<int> o1 = 42, o2 = 24;
			VERIFY(*o1 == 42);
			VERIFY(*o2 == 24);
			o1.swap(o2);
			VERIFY(*o1 == 24);
			VERIFY(*o2 == 42);
		}

		{
			optional<int> o1 = 42, o2 = 24;
			VERIFY(*o1 == 42);
			VERIFY(*o2 == 24);
			swap(o1, o2);
			VERIFY(*o1 == 24);
			VERIFY(*o2 == 42);
		}
	}

	{
		optional<IntStruct> o(in_place, 10);
		optional<IntStruct> e;

		VERIFY(o < IntStruct(42));
		VERIFY(!(o < IntStruct(2)));
		VERIFY(!(o < IntStruct(10)));
		VERIFY(e < o);
		VERIFY(e < IntStruct(10));

		VERIFY(o > IntStruct(4));
		VERIFY(!(o > IntStruct(42)));

		VERIFY(o >= IntStruct(4));
		VERIFY(o >= IntStruct(10));
		VERIFY(IntStruct(4) <= o);
		VERIFY(IntStruct(10) <= o);

		VERIFY(o == IntStruct(10));
		VERIFY(o->data == IntStruct(10).data);

		VERIFY(o != IntStruct(11));
		VERIFY(o->data != IntStruct(11).data);
		
		VERIFY(e == nullopt);
		VERIFY(nullopt == e);

		VERIFY(o != nullopt);
		VERIFY(nullopt != o);
		VERIFY(nullopt < o);
		VERIFY(o > nullopt);
		VERIFY(!(nullopt > o));
		VERIFY(!(o < nullopt));
		VERIFY(nullopt <= o);
		VERIFY(o >= nullopt);
	}

	// hash 
	{
		{
			// verify that the hash an empty eastl::optional object is zero.
			typedef hash<optional<int>> hash_optional_t;
			optional<int> e;
			VERIFY(hash_optional_t{}(e) == 0);
		}

		{
			// verify that the hash is the same as the hash of the underlying type
			const char* const pMessage = "Electronic Arts Canada";
			typedef hash<optional<string>> hash_optional_t;
			optional<string> o = string(pMessage);
			VERIFY(hash_optional_t{}(o) == hash<string>{}(pMessage));
		}
	}

	// sorting
	{
		vector<optional<int>> v = {{122}, {115}, nullopt, {223}}; 
		sort(begin(v), end(v));
		vector<optional<int>> sorted = {nullopt, 115, 122, 223};

		VERIFY(v == sorted);
	}

	// test destructors being called.
	{
		destructor_test::reset();
		{
			optional<destructor_test> o = destructor_test{};
		}
		VERIFY(destructor_test::destructor_ran);

		destructor_test::reset();
		{
			optional<destructor_test> o;
		} 
		// destructor shouldn't be called as object wasn't constructed.
		VERIFY(!destructor_test::destructor_ran);


		destructor_test::reset();
		{
			optional<destructor_test> o = {};
		} 
		// destructor shouldn't be called as object wasn't constructed.
		VERIFY(!destructor_test::destructor_ran);

		destructor_test::reset();
		{
			optional<destructor_test> o = nullopt; 
		} 
		// destructor shouldn't be called as object wasn't constructed.
		VERIFY(!destructor_test::destructor_ran);
	}

    #endif // EASTL_OPTIONAL_ENABLED
	return nErrorCount;
}

