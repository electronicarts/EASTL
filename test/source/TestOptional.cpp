/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EASTL/type_traits.h>
#include <EASTL/sort.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/optional.h>
#include <EASTL/unique_ptr.h>


/////////////////////////////////////////////////////////////////////////////
struct IntStruct
{
	IntStruct(int in) : data(in) {}
	int data;
};

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
auto operator<=>(const IntStruct& lhs, const IntStruct& rhs) { return lhs.data <=> rhs.data; }
#else
bool operator<(const IntStruct& lhs, const IntStruct& rhs)
	{ return lhs.data < rhs.data; }
#endif
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
struct copy_test
{
	copy_test() = default;

	copy_test(const copy_test& ct)
	{
		was_copied = true;
		value = ct.value;
	}

	copy_test& operator=(const copy_test& ct)
	{
		was_copied = true;
		value = ct.value;

		return *this;
	}

	// issue a compiler error if container tries to move
	copy_test(copy_test const&&) = delete;
	copy_test& operator=(const copy_test&&) = delete;

	static bool was_copied;

	int value;
};

bool copy_test::was_copied = false;

/////////////////////////////////////////////////////////////////////////////
struct move_test
{
    move_test() = default;

	move_test(move_test&& mt)
	{
		was_moved = true;
		value = mt.value;
	}

	move_test& operator=(move_test&& mt)
	{
		was_moved = true;
		value = mt.value;

		return *this;
	}

	// issue a compiler error if container tries to copy
	move_test(move_test const&) = delete;
	move_test& operator=(const move_test&) = delete;

	static bool was_moved;

	int value;
};

bool move_test::was_moved = false;

/////////////////////////////////////////////////////////////////////////////
template <typename T>
class forwarding_test
{
	eastl::optional<T> m_optional;

public:
	forwarding_test() : m_optional() {}
	forwarding_test(T&& t) : m_optional(t) {}
	~forwarding_test() { m_optional.reset(); }

	template <typename U>
	T GetValueOrDefault(U&& def) const
	{
		return m_optional.value_or(eastl::forward<U>(def));
	}
};

/////////////////////////////////////////////////////////////////////////////
struct assignment_test
{
	assignment_test()                                  { ++num_objects_inited; }
	assignment_test(assignment_test&&)                 { ++num_objects_inited; }
	assignment_test(const assignment_test&)            { ++num_objects_inited; }
	assignment_test& operator=(assignment_test&&)      { return *this; }
	assignment_test& operator=(const assignment_test&) { return *this; }
	~assignment_test()                                 { --num_objects_inited; }

	static int num_objects_inited;
};

int assignment_test::num_objects_inited = 0;


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
			EASTL_INTERNAL_DISABLE_DEPRECATED() // 'is_literal_type<nullopt_t>': was declared deprecated
				VERIFY(is_literal_type<nullopt_t>::value);
			EASTL_INTERNAL_RESTORE_DEPRECATED()
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
			int a = 42;
			auto o = make_optional(a);
			VERIFY((is_same<decltype(o)::value_type, int>::value));
			VERIFY(o.value() == 42);
		}

		{
			// test make_optional stripping refs/cv-qualifers
			int a = 42;
			const volatile int& intRef = a;
			auto o = make_optional(intRef);
			VERIFY((is_same<decltype(o)::value_type, int>::value));
			VERIFY(o.value() == 42);
		}

		{
			int a = 10;
			const volatile int& aRef = a;
			auto o = eastl::make_optional(aRef);
			VERIFY(o.value() == 10);
		}

		{
			{
				struct local { int payload1; };
				auto o = eastl::make_optional<local>(42);
				VERIFY(o.value().payload1 == 42);
			}
			{
				struct local { int payload1; int payload2; };
				auto o = eastl::make_optional<local>(42, 43);
				VERIFY(o.value().payload1 == 42);
				VERIFY(o.value().payload2 == 43);
			}

			{
				struct local
				{
					local(std::initializer_list<int> ilist)
					{
						payload1 = ilist.begin()[0];
						payload2 = ilist.begin()[1];
					}

					int payload1;
					int payload2;
				};

				auto o = eastl::make_optional<local>({42, 43});
				VERIFY(o.value().payload1 == 42);
				VERIFY(o.value().payload2 == 43);
			}
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

	{
		copy_test c;
		c.value = 42;

		optional<copy_test> o1(c);
		VERIFY(copy_test::was_copied);

		copy_test::was_copied = false;

		optional<copy_test> o2(o1);
		VERIFY(copy_test::was_copied);
		VERIFY(o2->value == 42);
	}

	{
		move_test t;
		t.value = 42;

		optional<move_test> o1(eastl::move(t));
		VERIFY(move_test::was_moved);

		move_test::was_moved = false;

		optional<move_test> o2(eastl::move(o1));
		VERIFY(move_test::was_moved);
		VERIFY(o2->value == 42);
	}

	{
        forwarding_test<float>ft(1.f);
        float val = ft.GetValueOrDefault(0.f);
        VERIFY(val == 1.f);
	}

	{
		assignment_test::num_objects_inited = 0;
		{
			optional<assignment_test> o1;
			optional<assignment_test> o2 = assignment_test();
			optional<assignment_test> o3(o2);
			VERIFY(assignment_test::num_objects_inited == 2);
			o1 = nullopt;
			VERIFY(assignment_test::num_objects_inited == 2);
			o1 = o2;
			VERIFY(assignment_test::num_objects_inited == 3);
			o1 = o2;
			VERIFY(assignment_test::num_objects_inited == 3);
			o1 = nullopt;
			VERIFY(assignment_test::num_objects_inited == 2);
			o2 = o1;
			VERIFY(assignment_test::num_objects_inited == 1);
			o1 = o2;
			VERIFY(assignment_test::num_objects_inited == 1);
		}
		VERIFY(assignment_test::num_objects_inited == 0);

		{
			optional<assignment_test> o1;
			VERIFY(assignment_test::num_objects_inited == 0);
			o1 = nullopt;
			VERIFY(assignment_test::num_objects_inited == 0);
			o1 = optional<assignment_test>(assignment_test());
			VERIFY(assignment_test::num_objects_inited == 1);
			o1 = optional<assignment_test>(assignment_test());
			VERIFY(assignment_test::num_objects_inited == 1);
			optional<assignment_test> o2(eastl::move(o1));
			VERIFY(assignment_test::num_objects_inited == 2);
			o1 = nullopt;
			VERIFY(assignment_test::num_objects_inited == 1);
		}
		VERIFY(assignment_test::num_objects_inited == 0);
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

		struct nonCopyableNonMovable
		{
			nonCopyableNonMovable(int v) : val(v) {}

			nonCopyableNonMovable(const nonCopyableNonMovable&) = delete;
			nonCopyableNonMovable(nonCopyableNonMovable&&) = delete;
			nonCopyableNonMovable& operator=(const nonCopyableNonMovable&) = delete;

			int val = 0;
		};

		{
			optional<nonCopyableNonMovable> o;
			o.emplace(42);
			VERIFY(o->val == 42);
		}

		{
			// Verify emplace will destruct object if it has been engaged.
			destructor_test::reset();
			optional<destructor_test> o;
			o.emplace();
			VERIFY(!destructor_test::destructor_ran);

			destructor_test::reset();
			o.emplace();
			VERIFY(destructor_test::destructor_ran);
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

		{
			optional<int> o1 = 42, o2;
			VERIFY(*o1 == 42);
			VERIFY(o2.has_value() == false);
			swap(o1, o2);
			VERIFY(o1.has_value() == false);
			VERIFY(*o2 == 42);
		}

		{
			optional<int> o1 = nullopt, o2 = 42;
			VERIFY(o1.has_value() == false);
			VERIFY(*o2 == 42);
			swap(o1, o2);
			VERIFY(*o1 == 42);
			VERIFY(o2.has_value() == false);
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

	#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	{
		optional<IntStruct> o(in_place, 10);
		optional<IntStruct> e;

		VERIFY((o <=> IntStruct(42)) < 0);
		VERIFY((o <=> IntStruct(2)) >= 0);
		VERIFY((o <=> IntStruct(10)) >= 0);
		VERIFY((e <=> o) < 0);
		VERIFY((e <=> IntStruct(10)) < 0);

		VERIFY((o <=> IntStruct(4)) > 0);
		VERIFY(o <=> IntStruct(42) <= 0);

		VERIFY((o <=> IntStruct(4)) >= 0);
		VERIFY((o <=> IntStruct(10)) >= 0);
		VERIFY((IntStruct(4) <=> o) <= 0);
		VERIFY((IntStruct(10) <=> o) <= 0);

		VERIFY((o <=> IntStruct(10)) == 0);
		VERIFY((o->data <=> IntStruct(10).data) == 0);

		VERIFY((o <=> IntStruct(11)) != 0);
		VERIFY((o->data <=> IntStruct(11).data) != 0);

		VERIFY((e <=> nullopt) == 0);
		VERIFY((nullopt <=> e) == 0);

		VERIFY((o <=> nullopt) != 0);
		VERIFY((nullopt <=> o) != 0);
		VERIFY((nullopt <=> o) < 0);
		VERIFY((o <=> nullopt) > 0);
		VERIFY((nullopt <=> o) <= 0);
		VERIFY((o <=> nullopt) >= 0);
	}
	#endif

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

	// optional rvalue tests
	{
		VERIFY(*optional<uint32_t>(1u)						== 1u);
	    VERIFY(optional<uint32_t>(1u).value()				== 1u);
	    VERIFY(optional<uint32_t>(1u).value_or(0xdeadf00d)	== 1u);
	    VERIFY(optional<uint32_t>().value_or(0xdeadf00d)	== 0xdeadf00d);
	    VERIFY(optional<uint32_t>(1u).has_value() == true);
	    VERIFY(optional<uint32_t>().has_value() == false);
		VERIFY( optional<IntStruct>(in_place, 10)->data		== 10);

	}

	// alignment type tests
	{
		static_assert(alignof(optional<Align16>) == alignof(Align16), "optional alignment failure");
		static_assert(alignof(optional<Align32>) == alignof(Align32), "optional alignment failure");
		static_assert(alignof(optional<Align64>) == alignof(Align64), "optional alignment failure");
	}

	{
		// user reported regression that failed to compile
		struct local_struct
		{
			local_struct() {}
			~local_struct() {}
		};
		static_assert(!eastl::is_trivially_destructible_v<local_struct>, "");

		{
			local_struct ls;
			eastl::optional<local_struct> o{ls};
		}
		{
			const local_struct ls;
			eastl::optional<local_struct> o{ls};
		}
	}

	{
		{
			// user regression
			eastl::optional<eastl::string> o = eastl::string("Hello World");
			eastl::optional<eastl::string> co;

			co = o; // force copy-assignment

			VERIFY( o.value().data() != co.value().data());
			VERIFY( o.value().data() == eastl::string("Hello World"));
			VERIFY(co.value().data() == eastl::string("Hello World"));
		}
		{
			// user regression
			EA_DISABLE_VC_WARNING(4625 4626) // copy/assignment operator constructor was implicitly defined as deleted
			struct local
			{
				eastl::unique_ptr<int> ptr;
			};
			EA_RESTORE_VC_WARNING()

			eastl::optional<local> o1 = local{eastl::make_unique<int>(42)};
			eastl::optional<local> o2;

			o2 = eastl::move(o1);

			VERIFY(!!o1 == true);
			VERIFY(!!o2 == true);
			VERIFY(!!o1->ptr == false);
			VERIFY(!!o2->ptr == true);
			VERIFY(o2->ptr.get() != nullptr);
		}
		{
			// user regression
			static bool copyCtorCalledWithUninitializedValue;
			static bool moveCtorCalledWithUninitializedValue;
			copyCtorCalledWithUninitializedValue = moveCtorCalledWithUninitializedValue = false;
			struct local
			{
				uint32_t val;
				local()
					: val(0xabcdabcd)
				{}
				local(const local& other)
					: val(other.val)
				{
					if (other.val != 0xabcdabcd)
						copyCtorCalledWithUninitializedValue = true;
				}
				local(local&& other)
					: val(eastl::move(other.val))
				{
					if (other.val != 0xabcdabcd)
						moveCtorCalledWithUninitializedValue = true;
				}
				local& operator=(const local&) = delete;
			};
			eastl::optional<local> n;
			eastl::optional<local> o1(n);
			VERIFY(!copyCtorCalledWithUninitializedValue);
			eastl::optional<local> o2(eastl::move(n));
			VERIFY(!moveCtorCalledWithUninitializedValue);
		}
	}

	{
		auto testFn = []() -> optional<int>
		{
			return eastl::nullopt;
		};

		auto o = testFn();
		VERIFY(!!o == false);
	}

    #endif // EASTL_OPTIONAL_ENABLED
	return nErrorCount;
}

