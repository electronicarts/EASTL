/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/any.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/numeric.h>
#include <EAStdC/EAString.h>


// SmallTestObject
//
struct SmallTestObject
{
	static int mCtorCount;

	SmallTestObject() EA_NOEXCEPT { mCtorCount++; }
	SmallTestObject(const SmallTestObject&) EA_NOEXCEPT { mCtorCount++; }
	SmallTestObject(SmallTestObject&&) EA_NOEXCEPT { mCtorCount++; }
	SmallTestObject& operator=(const SmallTestObject&) EA_NOEXCEPT { mCtorCount++; return *this; }
	~SmallTestObject() EA_NOEXCEPT { mCtorCount--; }

	static void Reset() { mCtorCount = 0; }
	static bool IsClear() { return mCtorCount == 0; }
};

int SmallTestObject::mCtorCount = 0;


// RequiresInitList
//
struct RequiresInitList
{
	RequiresInitList(std::initializer_list<int> ilist)
	: sum(eastl::accumulate(begin(ilist), end(ilist), 0)) {}

	int sum;
};


int TestAny()
{
	using namespace eastl;
	int nErrorCount = 0;

	// NOTE(rparolin): Ensure 'any' is at least the size of an eastl::string and an eastl::vector to prevent heap
	// allocation of handle objects (objects that point to a heap allocation).  This will reduce memory pressure since
	// eastl::string will be a commonly used type.  We could also test with a vector.
	{
		static_assert(sizeof(eastl::string) <= sizeof(eastl::any), "ensure that 'any' has enough local memory to store a string");
		static_assert(sizeof(eastl::vector<int>) <= sizeof(eastl::any), "ensure that 'any' has enough local memory to store a vector");
	}

	{
	  // default construct
		any a;
		VERIFY(a.has_value() == false);
	}

	{
		// test object ctors & dtors are called for a large object
		TestObject::Reset();
		{ any a{TestObject()}; }
		VERIFY(TestObject::IsClear());
	}

	{
		// test object ctors & dtors are called for a small object
		SmallTestObject::Reset();
		{ any a{SmallTestObject()}; }
		VERIFY(SmallTestObject::IsClear());
	}

	{
		any a(42);
		VERIFY(a.has_value() == true);

		VERIFY(any_cast<int>(a) == 42);
		VERIFY(any_cast<int>(a) != 1337);
		any_cast<int&>(a) = 10;
		VERIFY(any_cast<int>(a) == 10);

		a = 1.f;
		any_cast<float&>(a) = 1337.f;
		VERIFY(any_cast<float>(a) == 1337.f);

		a = 4343;
		VERIFY(any_cast<int>(a) == 4343);

		a = string("hello world");
		VERIFY(any_cast<string>(a) == "hello world");
		VERIFY(any_cast<string&>(a) == "hello world");
	}

	{
		struct custom_type { int data; };

		any a = custom_type{};
		any_cast<custom_type&>(a).data = 42;
		VERIFY(any_cast<custom_type>(a).data == 42);
	}

	{
		any a = 42;
		VERIFY(any_cast<int>(a) == 42);

		#if EASTL_EXCEPTIONS_ENABLED
			int throwCount = 0;
			try { VERIFY(any_cast<short>(a) == 42); }
			catch (bad_any_cast) { throwCount++;  }
			VERIFY(throwCount != 0);
		#endif
	}

	{
		vector<any> va = {42, 'a', 42.f, 3333u, 4444ul, 5555ull, 6666.0};

		VERIFY(any_cast<int>(va[0]) == 42);
		VERIFY(any_cast<char>(va[1]) == 'a');
		VERIFY(any_cast<float>(va[2]) == 42.f);
		VERIFY(any_cast<unsigned>(va[3]) == 3333u);
		VERIFY(any_cast<unsigned long>(va[4]) == 4444ul);
		VERIFY(any_cast<unsigned long long>(va[5]) == 5555ull);
		VERIFY(any_cast<double>(va[6]) == 6666.0);
	}

	{
		any a(string("test string"));
		VERIFY(a.has_value());
		VERIFY(any_cast<string>(a) == "test string");
	}

	{
		vector<any> va = {42, string("rob"), 'a', 42.f};
		VERIFY(any_cast<int>(va[0]) == 42);
		VERIFY(any_cast<string>(va[1]) == "rob");
		VERIFY(any_cast<char>(va[2]) == 'a');
		VERIFY(any_cast<float>(va[3]) == 42.f);
	}

	{
		vector<any> va;
		va.push_back(42);
		va.push_back(string("rob"));
		va.push_back('a');
		va.push_back(42.f);

		VERIFY(any_cast<int>(va[0]) == 42);
		VERIFY(any_cast<string>(va[1]) == "rob");
		VERIFY(any_cast<char>(va[2]) == 'a');
		VERIFY(any_cast<float>(va[3]) == 42.f);
	}

	// NOTE(rparolin):  Replaces a small 'any' object with a large one and make sure it doesn't corrupt
	// the surrounding memory in the vector.
	{
		TestObject::Reset();
		{
			vector<any> va = {42, 'a', 42.f, 3333u, 4444ul, 5555ull, 6666.0};

			VERIFY(any_cast<int>(va[0]) == 42);
			VERIFY(any_cast<char>(va[1]) == 'a');
			VERIFY(any_cast<float>(va[2]) == 42.f);
			VERIFY(any_cast<unsigned>(va[3]) == 3333u);
			VERIFY(any_cast<unsigned long>(va[4]) == 4444ul);
			VERIFY(any_cast<unsigned long long>(va[5]) == 5555ull);
			VERIFY(any_cast<double>(va[6]) == 6666.0);

			va[3] = TestObject(3333); // replace a small integral with a large heap allocated object.

			VERIFY(any_cast<int>(va[0]) == 42);
			VERIFY(any_cast<char>(va[1]) == 'a');
			VERIFY(any_cast<float>(va[2]) == 42.f);
			VERIFY(any_cast<TestObject>(va[3]).mX == 3333); // not 3333u because TestObject ctor takes a signed type.
			VERIFY(any_cast<unsigned long>(va[4]) == 4444ul);
			VERIFY(any_cast<unsigned long long>(va[5]) == 5555ull);
			VERIFY(any_cast<double>(va[6]) == 6666.0);
		}
		VERIFY(TestObject::IsClear());
	}

	{
		any a(string("test string"));
		VERIFY(a.has_value());
		a.reset();
		VERIFY(!a.has_value());
	}

	{
		any a1 = 42;
		any a2 = a1;

		VERIFY(a1.has_value());
		VERIFY(a2.has_value());
		VERIFY(any_cast<int>(a1) == any_cast<int>(a2));
	}

	{
		any a1;
		VERIFY(!a1.has_value());
		{
			any a2(string("test string"));
			a1 = any_cast<string>(a2);

			VERIFY(a1.has_value());
		}
		VERIFY(any_cast<string>(a1) == "test string");
		VERIFY(a1.has_value());
	}

	{
		any a1;
		VERIFY(!a1.has_value());
		{
			any a2(string("test string"));
			a1 = a2;
			VERIFY(a1.has_value());
		}
		VERIFY(any_cast<string&>(a1) == "test string");
		VERIFY(a1.has_value());
	}

	// swap tests
	{
		{
			any a1 = 42;
			any a2 = 24;
			VERIFY(any_cast<int>(a1) == 42);
			VERIFY(any_cast<int>(a2) == 24);

			a1.swap(a2);
			VERIFY(any_cast<int>(a1) == 24);
			VERIFY(any_cast<int>(a2) == 42);

			eastl::swap(a1, a2);
			VERIFY(any_cast<int>(a1) == 42);
			VERIFY(any_cast<int>(a2) == 24);
		}
		{
			any a1 = string("hello");
			any a2 = string("world");
			VERIFY(any_cast<string>(a1) == "hello");
			VERIFY(any_cast<string>(a2) == "world");

			a1.swap(a2);
			VERIFY(any_cast<string>(a1) == "world");
			VERIFY(any_cast<string>(a2) == "hello");

			eastl::swap(a1, a2);
			VERIFY(any_cast<string>(a1) == "hello");
			VERIFY(any_cast<string>(a2) == "world");
		}
	}

	#if EASTL_RTTI_ENABLED
	{
		#if defined(EA_COMPILER_MSVC)
			VERIFY(EA::StdC::Strcmp(any(42).type().name(), "int") == 0);
			VERIFY(EA::StdC::Strcmp(any(42.f).type().name(), "float") == 0);
			VERIFY(EA::StdC::Strcmp(any(42u).type().name(), "unsigned int") == 0);
			VERIFY(EA::StdC::Strcmp(any(42ul).type().name(), "unsigned long") == 0);
			VERIFY(EA::StdC::Strcmp(any(42l).type().name(), "long") == 0);

		#elif defined(EA_COMPILER_CLANG) || defined(EA_COMPILER_GNUC)
			VERIFY(EA::StdC::Strcmp(any(42).type().name(), "i") == 0);
			VERIFY(EA::StdC::Strcmp(any(42.f).type().name(), "f") == 0);
			VERIFY(EA::StdC::Strcmp(any(42u).type().name(), "j") == 0);
			VERIFY(EA::StdC::Strcmp(any(42ul).type().name(), "m") == 0);
			VERIFY(EA::StdC::Strcmp(any(42l).type().name(), "l") == 0);
		#endif
	}
	#endif

	// emplace, small object tests
	{
		any a;

		a.emplace<int>(42);
		VERIFY(a.has_value());
		VERIFY(any_cast<int>(a) == 42);

		a.emplace<short>((short)8); // no way to define a short literal we must cast here.
		VERIFY(any_cast<short>(a) == 8);
		VERIFY(a.has_value());

		a.reset();
		VERIFY(!a.has_value());
	}

	// emplace, large object tests
	{
		TestObject::Reset();
		{
			any a;
			a.emplace<TestObject>();
			VERIFY(a.has_value());
		}
		VERIFY(TestObject::IsClear());
	}

	// emplace, initializer_list
	{
		{
			any a;
			a.emplace<RequiresInitList>(std::initializer_list<int>{1,2,3,4,5,6});

			VERIFY(a.has_value());
			VERIFY(any_cast<RequiresInitList>(a).sum == 21);
		}
	}

	// equivalence tests
	{
		any a, b;
		VERIFY(!a.has_value() == !b.has_value());

		#if EASTL_EXCEPTIONS_ENABLED
			int bad_any_cast_thrown = 0;
			try
			{
				VERIFY(any_cast<int>(a) == any_cast<int>(b));
			}
			catch (eastl::bad_any_cast)
			{
				bad_any_cast_thrown++;
			}
			VERIFY(bad_any_cast_thrown != 0);
		#endif


		a = 42; b = 24;
		VERIFY(any_cast<int>(a) != any_cast<int>(b));
		VERIFY(a.has_value() == b.has_value());

		a = 42; b = 42;
		VERIFY(any_cast<int>(a) == any_cast<int>(b));
		VERIFY(a.has_value() == b.has_value());
	}

	// move tests
	{
		any a = string("hello world"); 
		VERIFY(any_cast<string&>(a) == "hello world");

		auto s = move(any_cast<string&>(a)); // move string out
		VERIFY(s == "hello world");
		VERIFY(any_cast<string&>(a).empty());

		any_cast<string&>(a) = move(s); // move string in
		VERIFY(any_cast<string&>(a) == "hello world");
	}

	// nullptr tests
	{
		any* a = nullptr;
		VERIFY(any_cast<int>(a) == nullptr);
		VERIFY(any_cast<short>(a) == nullptr);
		VERIFY(any_cast<long>(a) == nullptr);
		VERIFY(any_cast<string>(a) == nullptr);

		any b;
		VERIFY(any_cast<short>(&b) == nullptr);
		VERIFY(any_cast<const short>(&b) == nullptr);
		VERIFY(any_cast<volatile short>(&b) == nullptr);
		VERIFY(any_cast<const volatile short>(&b) == nullptr);

		VERIFY(any_cast<short*>(&b) == nullptr);
		VERIFY(any_cast<const short*>(&b) == nullptr);
		VERIFY(any_cast<volatile short*>(&b) == nullptr);
		VERIFY(any_cast<const volatile short*>(&b) == nullptr);
	}

	// Aligned type tests
	{
		{
		   any a = Align16(1337);
		   VERIFY(any_cast<Align16>(a) == Align16(1337));
		}
		
		{
			any a = Align32(1337);
			VERIFY(any_cast<Align32>(a) == Align32(1337));
		}
		
		{
			any a = Align64(1337);
			VERIFY(any_cast<Align64>(a) == Align64(1337));
		}
	}

	// make_any
	{
		{
			auto a = make_any<int>(42);
			VERIFY(any_cast<int>(a) == 42);
		}

		{
			auto a = make_any<RequiresInitList>(std::initializer_list<int>{1,2,3,4,5,6,7,8});
			VERIFY(any_cast<RequiresInitList&>(a).sum == 36);
		}
	}

	// user reported regression that eastl::any constructor was not decaying the deduced type correctly.
	{
		float f = 42.f;
		eastl::any a(f);
		VERIFY(any_cast<float>(a) == 42.f);
	}

	//testing unsafe operations
	{
		eastl::any a = 1;
		int* i = eastl::any_cast<int>(&a);
		VERIFY((*i) == 1);

		a = 2;
		int *j = (int*)eastl::unsafe_any_cast<void>(&a);
		VERIFY((*j) == 2);

		const eastl::any b = 3;
		const void * p = eastl::unsafe_any_cast<void>(&b);
		void *q = const_cast<void *>(p);
		int *r = static_cast<int *>(q);
		VERIFY((*r) == 3);
	}

	// user regression when calling the assignment operator
	{
		{
			eastl::any a1;
			eastl::any a2;
			VERIFY(a1.has_value() == false);
			VERIFY(a2.has_value() == false);

			a1 = a2;
			VERIFY(a1.has_value() == false);
			VERIFY(a2.has_value() == false);
		}

		{
			eastl::any a1 = 42;
			eastl::any a2;
			VERIFY(a1.has_value() == true);
			VERIFY(a2.has_value() == false);

			a1 = a2;
			VERIFY(a1.has_value() == false);
			VERIFY(a2.has_value() == false);
		}

		{
			eastl::any a1;
			eastl::any a2 = 42;
			VERIFY(a1.has_value() == false);
			VERIFY(a2.has_value() == true);

			a1 = a2;
			VERIFY(a1.has_value() == true);
			VERIFY(a2.has_value() == true);
			VERIFY(any_cast<int>(a1) == 42);
			VERIFY(any_cast<int>(a2) == 42);
		}
	}

	return nErrorCount;
}


