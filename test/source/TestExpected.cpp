/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <EABase/eabase.h>

// We use a few c++17 features in the implementation of eastl::expceted, so we only provide
// it from c++17 onwards.
#if EA_COMPILER_CPP17_ENABLED

#include <EASTL/expected.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/vector.h>

#include "EASTLTest.h"


using namespace eastl;

namespace
{
	enum class TestError
	{
		Error1,
		Error2,
		Error3,
	};

	// TODO: Move some of these "testing types" to a common header, perhaps EASTLTest.h?
	struct AggregateTest
	{
		int mX;
		int mY;
	};

	struct Point
	{
		Point(int x, int y) noexcept : mX{x}, mY{y} {};
		int mX{};
		int mY{};
	};

	struct SomeClass
	{
		int mI{};
	};

	struct ConversionTest
	{
		ConversionTest(const SomeClass& s) : mI{s.mI} {}
		int mI{};
	};

	struct DefaultConstructible
	{
		static constexpr int DefaultID = 10;
		DefaultConstructible() : mId(DefaultID){};
		int mId;
	};

	struct NoDefaultConstructible
	{
		NoDefaultConstructible(int i) : mId(i){};
		int mId;
	};

	struct NoCopyConstructible
	{
		NoCopyConstructible() = default;
		NoCopyConstructible(const NoCopyConstructible&) = delete;
		NoCopyConstructible(NoCopyConstructible&&) = default;
	};

	struct CopyNoMove
	{
		CopyNoMove() = default;
		CopyNoMove(const CopyNoMove&) = default;
		CopyNoMove(CopyNoMove&&) = delete;
	};

	struct MoveNoCopy
	{
		MoveNoCopy() = default;
		MoveNoCopy(const MoveNoCopy&) = delete;
		MoveNoCopy(MoveNoCopy&&) = default;
	};

	struct NoTriviallyCopyable
	{
		NoTriviallyCopyable() = default;
		NoTriviallyCopyable(int i) : mId{i} {}
		NoTriviallyCopyable(const NoTriviallyCopyable& other) { mId = other.mId; }
		int mId{};
	};

	struct NoTriviallyDestructible
	{
		~NoTriviallyDestructible() { ++mId; }

	public:
		int mId{};
	};

	struct NoTriviallyCopyableNoDefaultConstructible
	{
		NoTriviallyCopyableNoDefaultConstructible() = delete;
		NoTriviallyCopyableNoDefaultConstructible(int i) : mId(i) {}
		NoTriviallyCopyableNoDefaultConstructible(const NoTriviallyCopyableNoDefaultConstructible& other)
		{
			mId = other.mId;
		}
		int mId{};
	};

	struct NoImplilcitIntConversion
	{
		explicit NoImplilcitIntConversion(int i) : mId(i) {}
		int mId;
	};

	struct ImplilcitIntConversion
	{
		ImplilcitIntConversion(int i) : mId(i) {}
		int mId;
	};

	struct CopyAssignableNoMoveAssignable
	{
		CopyAssignableNoMoveAssignable() = default;
		CopyAssignableNoMoveAssignable(const CopyAssignableNoMoveAssignable&) = default;
		CopyAssignableNoMoveAssignable(CopyAssignableNoMoveAssignable&&) = delete;
		CopyAssignableNoMoveAssignable& operator=(const CopyAssignableNoMoveAssignable&) = default;
		CopyAssignableNoMoveAssignable& operator=(CopyAssignableNoMoveAssignable&&) = delete;
	};

#if EASTL_EXCEPTIONS_ENABLED
	struct ThrowOnMoveOrCopy
	{
		ThrowOnMoveOrCopy() = default;
		ThrowOnMoveOrCopy(const ThrowOnMoveOrCopy& x)
		{
			if (x.mShouldThrow)
				throw 0;
		}
		ThrowOnMoveOrCopy(ThrowOnMoveOrCopy&& x)
		{
			if (x.mShouldThrow)
				throw 1;
		}
		ThrowOnMoveOrCopy& operator=(const ThrowOnMoveOrCopy& x)
		{
			if (x.mShouldThrow)
				throw 2;
			return *this;
		}
		ThrowOnMoveOrCopy& operator=(ThrowOnMoveOrCopy&& x)
		{
			if (x.mShouldThrow)
				throw 3;
			return *this;
		}
		bool mShouldThrow{};
	};
#endif

	struct ClearOnMove
	{
		ClearOnMove() = default;
		ClearOnMove(int i) : mId{i} {}
		ClearOnMove(const ClearOnMove&) = default;
		ClearOnMove(ClearOnMove&& other) noexcept : mId{other.mId} { other.mId = 0; }
		ClearOnMove& operator=(const ClearOnMove&) = default;
		ClearOnMove& operator=(ClearOnMove&& other) noexcept
		{
			mId = other.mId;
			other.mId = 0;
			return *this;
		};
		int mId;
	};

	template <class T>
	struct InitListTest
	{
		InitListTest(std::initializer_list<T> il) noexcept : mVec(il){};
		eastl::vector<T> mVec;
	};

	template <class T, class Value, class Error, template <class> class... Rest>
	struct ExpectedTypeTraitsChecker : eastl::true_type
	{
	};

	template <class T, class Value, class Error, template <class> class Trait, template <class> class... Rest>
	struct ExpectedTypeTraitsChecker<T, Value, Error, Trait, Rest...>
	{
		static_assert(Trait<T>::value == Trait<eastl::expected<Value, Error>>::value);
		static constexpr bool value = (Trait<T>::value == Trait<eastl::expected<Value, Error>>::value) &&
		                              ExpectedTypeTraitsChecker<T, Value, Error, Rest...>::value;
	};


	template <class T>
	constexpr bool CheckExpectedValueTypeTraits = ExpectedTypeTraitsChecker<
	    T,   // This is the type to test against.
	    T,   // This is the value type
	    int, // This is the error type
	    eastl::is_default_constructible,
	    eastl::is_copy_constructible,
	    eastl::is_move_constructible,
	    eastl::is_trivially_copy_constructible,
	    eastl::is_trivially_move_constructible,
	    // clang-format off
	    // in the standard, this one is more strict for expected than for the template type parameters
	    // eastl::is_copy_assignable,
	    // in the standard, this one is more strict for expected than for the template type parameters
	    // eastl::is_move_assignable,
	    // no specification about when assignment is trivial
	    // eastl::is_trivially_copy_assignable,
	    // no specification about when assignment is trivial
	    // eastl::is_trivially_move_assignable,
	    // clang-format on
	    eastl::is_trivially_destructible>::value;

	template <class T>
	constexpr bool CheckExpectedErrorTypeTraits =
	    ExpectedTypeTraitsChecker<
	        T,   // This is the type to test against.
	        int, // This is the value type
	        T,   // This is the error type
	        // clang-format off
			// if the value is default constructible, expected should be as well, this check is below.
	    	// eastl::is_default_constructible,
	        // clang-format on
	        eastl::is_copy_constructible,
	        eastl::is_move_constructible,
	        eastl::is_trivially_copy_constructible,
	        eastl::is_trivially_move_constructible,
	        // clang-format off
	    	// in the standard, this one is more strict for expected than for the template type parameters
	    	// eastl::is_copy_assignable,
	    	// in the standard, this one is more strict for expected than for the template type parameters
	    	// eastl::is_move_assignable,
	    	// no specification about when assignment is trivial
	    	// eastl::is_trivially_copy_assignable,
	    	// no specification about when assignment is trivial
	    	// eastl::is_trivially_move_assignable,
	        // clang-format on
	        eastl::is_trivially_destructible>::value &&
	    // This just checks expected<int, T> is default constructible.
	    ExpectedTypeTraitsChecker<int, int, T, eastl::is_default_constructible>::value;


	template <class T>
	constexpr bool CheckExpectedTypeTraits = CheckExpectedValueTypeTraits<T> && CheckExpectedErrorTypeTraits<T>;

	template <class T>
	constexpr bool CheckExpectedVoidTypeTraits =
	    ExpectedTypeTraitsChecker<T,    // This is the type to test against.
	                              void, // This is the value type
	                              T,    // This is the error type
	                              eastl::is_copy_constructible,
	                              eastl::is_move_constructible,
	                              eastl::is_trivially_copy_constructible,
	                              eastl::is_trivially_move_constructible,
	                              eastl::is_trivially_destructible>::value &&
	    // This just checks expected<void, T> is default constructible.
	    ExpectedTypeTraitsChecker<int, void, T, eastl::is_default_constructible>::value;

} // namespace

int TestUnexpected()
{
	int nErrorCount = 0;
	{
		// simple construction and getter.
		eastl::unexpected<int> u(1);
		EATEST_VERIFY(u.error() == 1);
		eastl::unexpected<float> v(2.0f);
		EATEST_VERIFY(v.error() == 2.0f);
		eastl::unexpected<TestError> w(TestError::Error1);
		EATEST_VERIFY(w.error() == TestError::Error1);
		w.error() = TestError::Error2;
		EATEST_VERIFY(w.error() == TestError::Error2);
		eastl::unexpected<Point> x(Point{1, 2});
		EATEST_VERIFY(x.error().mX == 1 && x.error().mY == 2);
	}

	{
		// in place construction.
		eastl::unexpected<Point> u(eastl::in_place, 1, 2);
		EATEST_VERIFY(u.error().mX == 1 && u.error().mY == 2);
	}

#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
	{
		// initializer_list construction (how is this different than the normal in place construction?)
		eastl::unexpected<InitListTest<int>> u(eastl::in_place, {1, 2, 3, 4});
		eastl::vector<int> v = {1, 2, 3, 4};
		EATEST_VERIFY(u.error().mVec == v);
	}
#endif

	{
		// swap
		eastl::unexpected<int> u(1);
		eastl::unexpected<int> v(2);
		swap(u, v);
		EATEST_VERIFY(u.error() == 2);
		EATEST_VERIFY(v.error() == 1);
	}

	{
		// operator ==
		eastl::unexpected<int> u(1);
		eastl::unexpected<int> v(2);
		eastl::unexpected<int> w(2);
		EATEST_VERIFY(!(u == v));
		EATEST_VERIFY(w == v);
	}

	return nErrorCount;
}

int TestBadExpectedAccess()
{
	int nErrorCount = 0;

#if EASTL_EXCEPTIONS_ENABLED
	{
		expected<int, int> e{unexpect, 1};
		bool exceptionFired = false;
		try
		{
			e.value();
		}
		catch (const bad_expected_access<int>& b)
		{
			exceptionFired = true;
			EATEST_VERIFY(b.error() == 1);
		}
		EATEST_VERIFY(exceptionFired);
	}

	{
		expected<void, int> e{unexpect, 2};
		bool exceptionFired = false;
		try
		{
			e.value();
		}
		catch (const bad_expected_access<int>& b)
		{
			exceptionFired = true;
			EATEST_VERIFY(b.error() == 2);
		}
		EATEST_VERIFY(exceptionFired);
	}
#endif

	return nErrorCount;
}

int TestExpectedGeneric()
{
	static_assert(CheckExpectedTypeTraits<int>);
	static_assert(CheckExpectedTypeTraits<DefaultConstructible>);
	static_assert(CheckExpectedTypeTraits<NoDefaultConstructible>);
	static_assert(CheckExpectedTypeTraits<NoCopyConstructible>);
	static_assert(CheckExpectedTypeTraits<NoTriviallyCopyable>);
	static_assert(CheckExpectedTypeTraits<NoTriviallyDestructible>);
	static_assert(CheckExpectedTypeTraits<NoTriviallyCopyableNoDefaultConstructible>);
	static_assert(CheckExpectedTypeTraits<MoveNoCopy>);
	static_assert(CheckExpectedTypeTraits<eastl::vector<int>>);
	static_assert(CheckExpectedTypeTraits<eastl::unique_ptr<int>>);

	// Note: even if a type has a deleted move constructor, it's
	// corresponding expected type should be both copy and move
	// constructible.
	static_assert(is_copy_constructible_v<expected<CopyNoMove, int>>);
	static_assert(is_move_constructible_v<expected<CopyNoMove, int>>);
	static_assert(is_copy_constructible_v<expected<int, CopyNoMove>>);
	static_assert(is_move_constructible_v<expected<int, CopyNoMove>>);

	// Note: even if a type has a deleted move assignment, it's
	// corresponding expected type should still be both copy and move
	// assignable.
	static_assert(is_copy_assignable_v<expected<CopyAssignableNoMoveAssignable, int>>);
	static_assert(is_move_assignable_v<expected<CopyAssignableNoMoveAssignable, int>>);
	static_assert(is_copy_assignable_v<expected<int, CopyAssignableNoMoveAssignable>>);
	static_assert(is_move_assignable_v<expected<int, CopyAssignableNoMoveAssignable>>);

	int nErrorCount = 0;
	{
		// default construction for default constructible type
		expected<int, TestError> e;
		EATEST_VERIFY(e.has_value());

		// check for value initialization of contained type.
		EATEST_VERIFY(e.value() == 0);

		e.value() = 42;
		EATEST_VERIFY(e.has_value());
		EATEST_VERIFY(e.value() == 42);

		// copy constructor
		expected<int, TestError> e1{e};
		EATEST_VERIFY(e1.has_value());
		EATEST_VERIFY(e1.value() == 42);

		// copy assignement
		expected<int, TestError> e2{unexpect, TestError::Error2};

		// value->no_value
		e1 = e2;
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY((e1.error() == TestError::Error2));

		// no_value->value;
		e2 = e;
		EATEST_VERIFY(e2.has_value());
		EATEST_VERIFY(e2.value() == 42);
	}

	{
		expected<DefaultConstructible, TestError> e;
		EATEST_VERIFY(e.has_value());
		EATEST_VERIFY(e->mId == 10);
	}

	{
		expected<NoTriviallyCopyable, TestError> e{5};
		expected<NoTriviallyCopyable, TestError> e1 = e;
		EATEST_VERIFY(e1.has_value());
		EATEST_VERIFY(e1.value().mId == 5);
	}

	{
		expected<NoTriviallyCopyable, TestError> e{unexpect, TestError::Error3};
		expected<NoTriviallyCopyable, TestError> e1 = e;
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(e1.error() == TestError::Error3);
	}


	{
		expected<NoDefaultConstructible, TestError> e{2};
		expected<NoDefaultConstructible, TestError> e1 = e;
		EATEST_VERIFY(e1.value().mId == 2);
	}

	{
		expected<NoDefaultConstructible, NoDefaultConstructible> e{2};
		EATEST_VERIFY(e.has_value());
		expected<NoDefaultConstructible, NoDefaultConstructible> e1 = e;
		EATEST_VERIFY(e1.has_value());
		EATEST_VERIFY(e1.value().mId == 2);
	}

	{
		expected<NoTriviallyCopyableNoDefaultConstructible, NoTriviallyCopyableNoDefaultConstructible> e{2};
		EATEST_VERIFY(e.has_value());
		expected<NoTriviallyCopyableNoDefaultConstructible, NoTriviallyCopyableNoDefaultConstructible> e1 = e;
		EATEST_VERIFY(e1.has_value());
		EATEST_VERIFY(e1.value().mId == 2);
	}

	{
		expected<NoTriviallyCopyableNoDefaultConstructible, TestError> e{2};
		EATEST_VERIFY(e.has_value());
		expected<NoTriviallyCopyableNoDefaultConstructible, TestError> e1 = e;
		EATEST_VERIFY(e1.has_value());
		EATEST_VERIFY(e1.value().mId == 2);
	}

	{
		expected<NoCopyConstructible, TestError> e;
		EATEST_VERIFY(e.has_value());
	}

	{
		eastl::vector<int> v = {1, 2, 3, 4};
		expected<eastl::vector<int>, TestError> e(v);
		EATEST_VERIFY(e.value() == v);
		expected<eastl::vector<int>, TestError> e1 = e;
		EATEST_VERIFY(e1.value() == v);

		expected<eastl::vector<int>, TestError> e2 = std::move(e);
		EATEST_VERIFY(e2.value() == v);

		eastl::vector<int> v1 = {1, 2, 3, 4, 5, 6};
		expected<eastl::vector<int>, TestError> e3(v1);
		e2 = e3;
		EATEST_VERIFY(e2.value() == v1);

		e2 = std::move(e1);
		EATEST_VERIFY(e2.value() == v);

		// just check that we did in fact move from e1.
		EATEST_VERIFY(e1.value().size() == 0);
	}

	{
		expected<NoTriviallyCopyableNoDefaultConstructible, TestError> e(2);
		EATEST_VERIFY(e.value().mId == 2);
		expected<NoTriviallyCopyableNoDefaultConstructible, TestError> e1 = e;
		EATEST_VERIFY(e1.value().mId == 2);
	}

	{
		// implicit conversion
		expected<ImplilcitIntConversion, int> e = 1;
		EATEST_VERIFY(e.has_value());
		// explicit conversion
		expected<NoImplilcitIntConversion, int> e1(1);
		EATEST_VERIFY(e1.has_value());
		EATEST_VERIFY(e1.value().mId == e.value().mId);
	}

	{
		// check things work with move only types.
		expected<unique_ptr<int>, TestError> e(new int(2));
		expected<unique_ptr<int>, TestError> e1 = eastl::move(e);
		EATEST_VERIFY(e1.has_value());
		EATEST_VERIFY(*e1.value() == 2);
		e1 = eastl::unexpected<TestError>(TestError::Error2);
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(e1.error() == TestError::Error2);
		expected<unique_ptr<int>, TestError> e3(new int(5));
		EATEST_VERIFY(e3.has_value());
		EATEST_VERIFY(*e3.value() == 5);
		e3 = eastl::move(e1);
		EATEST_VERIFY(!e3.has_value());
		EATEST_VERIFY(e3.error() == TestError::Error2);
	}

	{
		// conversion between expected
		expected<unsigned int, unsigned int> e(1u);
		expected<int, int> e1(e);
		EATEST_VERIFY(e1.value() == 1);
	}

	{
		// conversion between expected
		eastl::vector<int> v = {1, 2, 3, 4};
		expected<eastl::vector<int>, unsigned int> e(v);
		expected<eastl::vector<int>, int> e1(eastl::move(e));
		EATEST_VERIFY(e1.value() == v);
	}

	{
		eastl::unexpected<TestError> unex(TestError::Error2);
		expected<int, TestError> e{unex};
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(e.error() == TestError::Error2);
	}

	{
		vector<int> v = {1, 2, 3, 4};
		expected<int, vector<int>> e{eastl::unexpected<vector<int>>(v)};
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(e.error() == v);

		expected<int, vector<int>> e1{eastl::move(e)};
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(e1.error() == v);
	}

	{
		expected<Point, int> e(in_place, 1, 2);
		EATEST_VERIFY(e.has_value());
		EATEST_VERIFY(e.value().mX == 1 && e.value().mY == 2);
	}

	{
		expected<int, Point> e(unexpect, 1, 2);
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(e.error().mX == 1 && e.error().mY == 2);
	}

	{
		expected<vector<int>, int> e(in_place, {1, 2, 3, 4});
		EATEST_VERIFY(e.has_value());
		vector<int> v = {1, 2, 3, 4};
		EATEST_VERIFY(e.value() == v);
	}

	{
		expected<int, vector<int>> e(unexpect, {1, 2, 3, 4});
		EATEST_VERIFY(!e.has_value());
		vector<int> v = {1, 2, 3, 4};
		EATEST_VERIFY(e.error() == v);
	}

	{
		// assignment from has_value -> has_value
		expected<vector<int>, int> e(in_place, {1, 2, 3, 4});
		vector<int> v = {1, 2, 3, 5};
		e = v;
		EATEST_VERIFY(e.has_value());
		EATEST_VERIFY(e.value() == v);
	}

	{
		// assignment from !has_value -> has_value
		expected<vector<int>, int> e(unexpect, 1);
		vector<int> v = {1, 2, 3, 5};
		e = v;
		EATEST_VERIFY(e.has_value());
		EATEST_VERIFY(e.value() == v);
	}

	{
		// assignment from has_value -> !has_value
		expected<vector<int>, int> e(in_place, {1, 2, 3, 4});
		eastl::unexpected<int> u{2};
		e = u;
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(e.error() == u.error());
	}

	{
		// assignment from !has_value -> !has_value
		expected<vector<int>, int> e(unexpect, 1);
		eastl::unexpected<int> u{2};
		e = u;
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(e.error() == u.error());
	}

	{
		expected<int, TestError> e(1);
		e = 3;
		EATEST_VERIFY(e.has_value());
		EATEST_VERIFY(e.value() == 3);
	}

	{
		expected<unsigned int, TestError> e(1u);
		e = 3; // test with conversion.
		EATEST_VERIFY(e.has_value());
		EATEST_VERIFY(e.value() == static_cast<float>(3));

		e = eastl::unexpected<TestError>(TestError::Error3);
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(e.error() == TestError::Error3);

		auto unex = eastl::unexpected<TestError>(TestError::Error2);
		e = unex;
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(e.error() == TestError::Error2);
	}

	{
		expected<Point, TestError> e(in_place, 1, 2);
		e.emplace(3, 4);
		EATEST_VERIFY(e.has_value());
		EATEST_VERIFY(e.value().mX == 3 && e.value().mY == 4);
	}

	{
#if defined EA_COMPILER_CPP20_ENABLED
		// NOTE: This should work in C++20 but not before since
		// C++20 changed how aggregates can be constructed.  See
		// https://godbolt.org/z/TnqWbebac
		expected<AggregateTest, TestError> e(in_place, 1, 2);
		EATEST_VERIFY(e.has_value());
		EATEST_VERIFY(e.value().mX == 1 && e.value().mY == 2);
		e.emplace(3, 4);
		EATEST_VERIFY(e.has_value());
		EATEST_VERIFY(e.value().mX == 3 && e.value().mY == 4);
#endif
	}

	{
		expected<InitListTest<int>, TestError> e(in_place, {1, 2});
		EATEST_VERIFY(e.has_value());
		EATEST_VERIFY(e.value().mVec == vector<int>({1, 2}));
		e.emplace({4, 5, 6});
	}

	{
		expected<int, TestError> e1(1);
		expected<int, TestError> e2(2);
		e1.swap(e2);
		EATEST_VERIFY(e1.has_value());
		EATEST_VERIFY(e1.value() == 2);
		EATEST_VERIFY(e2.has_value());
		EATEST_VERIFY(e2.value() == 1);
	}
#if EASTL_EXCEPTIONS_ENABLED
	{
		expected<ClearOnMove, ThrowOnMoveOrCopy> e1(1);
		expected<ClearOnMove, ThrowOnMoveOrCopy> e2(eastl::unexpected<ThrowOnMoveOrCopy>{ThrowOnMoveOrCopy{}});

		// Make it so we throw when we move or copy, so the swap throws.
		e2.error().mShouldThrow = true;
		bool exceptionThrown = false;
		try
		{
			e1.swap(e2);
		}
		catch (...)
		{
			exceptionThrown = true;
		}
		EATEST_VERIFY(exceptionThrown);
		// Verify that e1 is in a good state:
		EATEST_VERIFY(e1.has_value());
		// Verify that e1 has its old value:
		EATEST_VERIFY(e1.value().mId == 1);

		// Verify that moving from e1.vlaue() would've cleared the value.
		ClearOnMove x = eastl::move(e1.value());
		EATEST_VERIFY(e1.value().mId == 0);
	}

	{
		// Same as above, but now T throws on move construct.
		expected<ThrowOnMoveOrCopy, ClearOnMove> e1;
		expected<ThrowOnMoveOrCopy, ClearOnMove> e2(eastl::unexpected<ClearOnMove>{1});

		// Make it so we throw when we move or copy, so the swap throws.
		e1.value().mShouldThrow = true;
		bool exceptionThrown = false;
		try
		{
			e1.swap(e2);
		}
		catch (...)
		{
			exceptionThrown = true;
		}
		EATEST_VERIFY(exceptionThrown);
		// Verify that e1 is in a good state:
		EATEST_VERIFY(!e2.has_value());
		// Verify that e1 has its old value:
		EATEST_VERIFY(e2.error().mId == 1);

		// Verify that moving from e1.vlaue() would've cleared the value.
		ClearOnMove x = eastl::move(e2.error());
		EATEST_VERIFY(e2.error().mId == 0);
	}
#endif

	{
		expected<int, TestError> e1(1);
		expected<int, TestError> e2{eastl::unexpected<TestError>(TestError::Error1)};
		e1.swap(e2);
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(e1.error() == TestError::Error1);
		EATEST_VERIFY(e2.has_value());
		EATEST_VERIFY(e2.value() == 1);
	}

	{
		expected<int, TestError> e1(1);
		expected<int, TestError> e2(2);

		using eastl::swap;
		swap(e1, e2);
		static_assert(noexcept(swap(e1, e2)));

		EATEST_VERIFY(e1.has_value());
		EATEST_VERIFY(e1.value() == 2);
		EATEST_VERIFY(e2.has_value());
		EATEST_VERIFY(e2.value() == 1);
	}

	{
		expected<int, TestError> e1(1);
		expected<int, TestError> e2{eastl::unexpected<TestError>(TestError::Error1)};
		using eastl::swap;
		swap(e1, e2);
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(e1.error() == TestError::Error1);
		EATEST_VERIFY(e2.has_value());
		EATEST_VERIFY(e2.value() == 1);
	}

	{
		eastl::vector<int> v = {1, 2, 3, 4, 5};
		expected<int, eastl::vector<int>> e1{3};
		expected<int, eastl::vector<int>> e2{unexpect, v};
		using eastl::swap;
		swap(e1, e2);
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(e1.error() == v);
		EATEST_VERIFY(e2.has_value());
		EATEST_VERIFY(e2.value() == 3);
	}

	{
		eastl::vector<int> v = {1, 2, 3, 4, 5};
		expected<eastl::vector<int>, int> e1{v};
		expected<eastl::vector<int>, int> e2{unexpect, 5};
		using eastl::swap;
		swap(e1, e2);
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(e1.error() == 5);
		EATEST_VERIFY(e2.has_value());
		EATEST_VERIFY(e2.value() == v);
	}

	{
		expected<float, TestError> e(2.0f);
		EATEST_VERIFY(e.value_or(10.f) == 2.0f);
		EATEST_VERIFY(e.error_or(TestError::Error2) == TestError::Error2);
		e = eastl::unexpected{TestError::Error3};
		EATEST_VERIFY(e.value_or(10.f) == 10.0f);
		EATEST_VERIFY(e.value_or(1) == static_cast<float>(1));
		EATEST_VERIFY(e.error_or(TestError::Error2) == TestError::Error3);
	}

	{
		expected<int, int> e1(1);
		expected<int, int> e2(1);
		EATEST_VERIFY(e1 == e2);
		e2 = 5;
		EATEST_VERIFY(!(e1 == e2));
		e1 = eastl::unexpected{5};
		EATEST_VERIFY(!(e1 == e2));
		e2 = eastl::unexpected{5};
		EATEST_VERIFY(e1 == e2);
	}

	{
		expected<int, int> e1(1);
		expected<float, float> e2(1.0f);
		EATEST_VERIFY(e1 == e2);
		e2 = 5.0f;
		EATEST_VERIFY(!(e1 == e2));
		e1 = eastl::unexpected{1};
		EATEST_VERIFY(!(e1 == e2));
		e2 = eastl::unexpected{1.0f};
		EATEST_VERIFY(e1 == e2);
	}

	{
		const auto addHalf = [](int val) -> expected<float, TestError> { return static_cast<float>(val) + 0.5f; };
		const auto getVector = [](float val) -> expected<vector<int>, TestError>
		{
			if (val > 10.f)
			{
				return vector<int>{1, 2, 3, 4};
			}
			if (val > 0.0f)
			{
				return vector<int>{1};
			}
			return eastl::unexpected{TestError::Error2};
		};

		const auto isBigVector = [](vector<int> val) -> expected<bool, TestError> { return val.size() > 2; };

		// test and_then...
		{
			expected<int, TestError> e(1); // 1->1.5->{1}->false
			auto r1 = e.and_then(addHalf).and_then(getVector).and_then(isBigVector);
			EATEST_VERIFY(!*r1);
		}

		{
			expected<int, TestError> e(10); // 10->10.5->{1, 2, 3, 4}->true
			auto r1 = e.and_then(addHalf).and_then(getVector).and_then(isBigVector);
			EATEST_VERIFY(*r1);
		}

		{
			expected<int, TestError> e(-5); // -5->-4.5->Error2->Error2
			auto r1 = e.and_then(addHalf).and_then(getVector).and_then(isBigVector);
			EATEST_VERIFY(r1.error() == TestError::Error2);
		}

		// test or_else
		const auto getVectorForError = [](TestError err) -> expected<vector<int>, TestError>
		{
			switch (err)
			{
				case TestError::Error1:
					return vector<int>{1, 1, 1, 1};
				case TestError::Error2:
					return vector<int>{4, 3, 2, 1};
				case TestError::Error3:
					return vector<int>{0, 1, 2};
			}
			return vector<int>{};
		};

		{
			expected<float, TestError> e(1.0f); // 1.0->{1}->{1}
			auto r1 = e.and_then(getVector).or_else(getVectorForError);
			vector v = {1};
			EATEST_VERIFY(*r1 == v);
		}

		{
			expected<float, TestError> e(-5.0f); // -5.0f->TestError2->{4, 3, 2, 1}
			auto r1 = e.and_then(getVector).or_else(getVectorForError);
			vector v = {4, 3, 2, 1};
			EATEST_VERIFY(*r1 == v);
		}

		const auto pushBackTen = [](auto val)
		{
			val.push_back(10);
			return val;
		};

		const auto getSize = [](const vector<int>& val) { return val.size(); };

		{
			expected<float, TestError> e(1.0f); // 1.0f->{1}->{1, 10}->2
			auto r1 = e.and_then(getVector).transform(pushBackTen).transform(getSize);
			EATEST_VERIFY(*r1 == 2);
		}

		{
			expected<float, TestError> e(-5.0f); // -5.0f->TestError2->TesError2->TestError2
			auto r1 = e.and_then(getVector).transform(pushBackTen).transform(getSize);
			EATEST_VERIFY(r1.error() == TestError::Error2);
		}

		{
			expected<float, TestError> e(-5.0f); // -5.0f->TestError2->{4, 3, 2, 1}->4
			auto r1 = e.and_then(getVector).or_else(getVectorForError).transform(getSize);
			EATEST_VERIFY(*r1 == 4);
		}

		{
			expected<float, TestError> e(1.0f); // 1.0f->{1}->{1}->1
			auto r1 = e.and_then(getVector).or_else(getVectorForError).transform(getSize);
			EATEST_VERIFY(*r1 == 1);
		}

		auto cycleError = [](TestError err)
		{
			switch (err)
			{
				case TestError::Error1:
					return TestError::Error2;
				case TestError::Error2:
					return TestError::Error3;
				case TestError::Error3:
					return TestError::Error1;
			}
			return TestError::Error1;
		};

		{
			expected<float, TestError> e(-5.0f); // -5->TestError2->TestError3->{0, 1, 2}->3
			auto r1 = e.and_then(getVector).transform_error(cycleError).or_else(getVectorForError).transform(getSize);
			EATEST_VERIFY(*r1 == 3);
		}

		{
			expected<float, TestError> e(1.0f); // 1->{1}->{1}->{1}->1
			auto r1 = e.and_then(getVector).transform_error(cycleError).or_else(getVectorForError).transform(getSize);
			EATEST_VERIFY(*r1 == 1);
		}
	}

	return nErrorCount;
}

int TestExpectedVoid()
{

	static_assert(CheckExpectedVoidTypeTraits<int>);
	static_assert(CheckExpectedVoidTypeTraits<NoDefaultConstructible>);
	static_assert(CheckExpectedVoidTypeTraits<NoCopyConstructible>);
	static_assert(CheckExpectedVoidTypeTraits<NoTriviallyCopyable>);
	static_assert(CheckExpectedVoidTypeTraits<NoTriviallyDestructible>);
	static_assert(CheckExpectedVoidTypeTraits<NoTriviallyCopyableNoDefaultConstructible>);
	static_assert(CheckExpectedVoidTypeTraits<MoveNoCopy>);
	static_assert(CheckExpectedVoidTypeTraits<eastl::vector<int>>);
	static_assert(CheckExpectedVoidTypeTraits<eastl::unique_ptr<int>>);

	// Note: even if a type has a deleted move constructor,
	// expected should be both copy and move constructible.
	static_assert(is_copy_constructible_v<expected<void, CopyNoMove>>);
	static_assert(is_move_constructible_v<expected<void, CopyNoMove>>);

	int nErrorCount = 0;
	{
		// default construction
		expected<void, TestError> e;
		EATEST_VERIFY(e.has_value());

		// even if the error is not default constructible.
		expected<void, NoDefaultConstructible> e1;
		EATEST_VERIFY(e1.has_value());

		// copy constructor.
		expected<void, TestError> e2{e};
		EATEST_VERIFY(e2.has_value());
	}

	{
		// unexpected constructor test.
		eastl::unexpected<SomeClass> unex(SomeClass{3});
		expected<void, SomeClass> e{unex};
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(e.error().mI == 3);

		// Test constructor with different types.
		expected<void, ConversionTest> e1{e};
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(e1.error().mI == 3);

		// Test constructor with different types rvalues.
		expected<void, ConversionTest> e2{eastl::move(e)};
		EATEST_VERIFY(!e2.has_value());
		EATEST_VERIFY(e2.error().mI == 3);
	}

	{
		// Test constructor converting from an unexpected.
		eastl::unexpected<SomeClass> unex{SomeClass{4}};
		expected<void, ConversionTest> e{unex};
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(e.error().mI == 4);

		// With r-values
		expected<void, ConversionTest> e1{eastl::move(unex)};
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(e1.error().mI == 4);
	}

	{
		// Non trivial copies work.
		expected<void, NoTriviallyCopyable> e{unexpect, 4};
		expected<void, NoTriviallyCopyable> e1 = e;
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(e1.error().mId == 4);
	}

	{
		eastl::vector<int> v = {1, 2, 3, 4};
		expected<void, eastl::vector<int>> e(unexpect, v);
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(e.error() == v);

		expected<void, eastl::vector<int>> e1 = e;
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(e1.error() == v);

		expected<void, eastl::vector<int>> e2 = std::move(e);
		EATEST_VERIFY(!e2.has_value());
		EATEST_VERIFY(e2.error() == v);

		eastl::vector<int> v1 = {1, 2, 3, 4, 5, 6};
		expected<void, eastl::vector<int>> e3(unexpect, v1);
		e2 = e3;
		EATEST_VERIFY(e2.error() == v1);

		e2 = std::move(e1);
		EATEST_VERIFY(e2.error() == v);

		// just check that we did in fact move from e1.
		EATEST_VERIFY(e1.error().size() == 0);
	}

	{
		expected<void, NoTriviallyCopyableNoDefaultConstructible> e(unexpect, 2);
		EATEST_VERIFY(e.error().mId == 2);
		expected<void, NoTriviallyCopyableNoDefaultConstructible> e1 = e;
		EATEST_VERIFY(e1.error().mId == 2);
	}

	{
		// implicit conversion
		expected<void, ImplilcitIntConversion> e{unexpect, 1};
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(e.error().mId == 1);
	}

	{
		// check things work with move only types.
		expected<void, unique_ptr<int>> e(unexpect, new int(2));
		expected<void, unique_ptr<int>> e1 = eastl::move(e);
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(*e1.error() == 2);

		e = eastl::move(e1);
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(*e.error() == 2);

		expected<void, unique_ptr<int>> e2;
		EATEST_VERIFY(e2.has_value());

		// assignment changes from value -> error
		e2 = eastl::move(e);
		EATEST_VERIFY(!e2.has_value());
		EATEST_VERIFY(*e2.error() == 2);

		expected<void, unique_ptr<int>> e3;
		EATEST_VERIFY(e3.has_value());

		// assignment changes from error -> value
		e2 = eastl::move(e3);
		EATEST_VERIFY(e2.has_value());
	}

	{
		// conversion between expected
		expected<void, unsigned int> e(unexpect, 1u);
		expected<void, int> e1(e);
		EATEST_VERIFY(e1.error() == 1);
	}

	{
		vector<int> v = {1, 2, 3, 4};
		expected<void, vector<int>> e{eastl::unexpected<vector<int>>(v)};
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(e.error() == v);

		expected<void, vector<int>> e1{eastl::move(e)};
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(e1.error() == v);

		EATEST_VERIFY(e.error().size() == 0);
	}

	{
		expected<void, Point> e(unexpect, 1, 2);
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(e.error().mX == 1 && e.error().mY == 2);
	}

	{
		expected<void, vector<int>> e(unexpect, {1, 2, 3, 4});
		EATEST_VERIFY(!e.has_value());
		vector<int> v = {1, 2, 3, 4};
		EATEST_VERIFY(e.error() == v);
	}

	{
		expected<void, TestError> e;
		EATEST_VERIFY(e.has_value());

		e = eastl::unexpected<TestError>(TestError::Error3);
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(e.error() == TestError::Error3);

		auto unex = eastl::unexpected<TestError>(TestError::Error2);
		e = unex;
		EATEST_VERIFY(!e.has_value());
		EATEST_VERIFY(e.error() == TestError::Error2);
	}

	{
		expected<void, TestError> e1;
		expected<void, TestError> e2;
		e1.swap(e2);
		EATEST_VERIFY(e1.has_value());
		EATEST_VERIFY(e2.has_value());
	}

	{
		expected<void, TestError> e1;
		expected<void, TestError> e2{eastl::unexpected<TestError>(TestError::Error1)};
		e1.swap(e2);
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(e1.error() == TestError::Error1);
		EATEST_VERIFY(e2.has_value());
	}

	{
		expected<void, TestError> e1;
		expected<void, TestError> e2;
		using eastl::swap;
		swap(e1, e2);
		EATEST_VERIFY(e1.has_value());
		EATEST_VERIFY(e2.has_value());
	}

	{
		expected<void, TestError> e1;
		expected<void, TestError> e2{unexpect, TestError::Error1};
		using eastl::swap;
		swap(e1, e2);
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(e1.error() == TestError::Error1);
		EATEST_VERIFY(e2.has_value());
	}

	{
		eastl::vector<int> v = {1, 2, 3, 4, 5};
		expected<void, eastl::vector<int>> e1;
		expected<void, eastl::vector<int>> e2{unexpect, v};
		using eastl::swap;
		swap(e1, e2);
		EATEST_VERIFY(!e1.has_value());
		EATEST_VERIFY(e1.error() == v);
		EATEST_VERIFY(e2.has_value());
	}

	{
		expected<void, TestError> e;
		EATEST_VERIFY(e.error_or(TestError::Error2) == TestError::Error2);
		e = eastl::unexpected<TestError>{TestError::Error3};
		EATEST_VERIFY(e.error_or(TestError::Error2) == TestError::Error3);
	}

	{
		expected<void, int> e1;
		expected<void, int> e2;
		EATEST_VERIFY(e1 == e2);
		e2 = eastl::unexpected<int>{5};
		EATEST_VERIFY(!(e1 == e2));
		e1 = eastl::unexpected<int>{4};
		EATEST_VERIFY(!(e1 == e2));
		e2 = eastl::unexpected<int>{4};
		EATEST_VERIFY(e1 == e2);
	}

	{
		expected<void, int> e1;
		expected<void, float> e2;
		EATEST_VERIFY(e1 == e2);
		e2 = eastl::unexpected<float>{5.0f};
		EATEST_VERIFY(!(e1 == e2));
		e1 = eastl::unexpected<int>{1};
		EATEST_VERIFY(!(e1 == e2));
		e2 = eastl::unexpected<float>{1.0f};
		EATEST_VERIFY(e1 == e2);
	}

	{
		int counter{};
		const auto foo = [&counter]() -> expected<void, TestError>
		{
			++counter;
			return {};
		};
		{
			expected<void, TestError> e;
			auto e1 = e.and_then(foo);
			EATEST_VERIFY(counter == 1);
			EATEST_VERIFY(e1.has_value());
			counter = 0;
		}

		{
			expected<void, TestError> e{unexpect, TestError::Error2};
			expected<void, TestError> e1 = e.and_then(foo);
			EATEST_VERIFY(counter == 0);
			EATEST_VERIFY(!e1.has_value());
			EATEST_VERIFY(e1.error() == TestError::Error2);
			counter = 0;
		}

		const auto fooError = [&counter](TestError t) -> expected<void, TestError>
		{
			++counter;
			switch (t)
			{
				case TestError::Error1:
					return eastl::unexpected<TestError>(TestError::Error1);
				case TestError::Error2:
					return {};
				case TestError::Error3:
					return eastl::unexpected<TestError>(TestError::Error1);
			}
			return {};
		};

		{
			expected<void, TestError> e;
			// valid -> valid (pass-through without function call)
			expected<void, TestError> e1 = e.or_else(fooError);
			EATEST_VERIFY(counter == 0);
			EATEST_VERIFY(e1.has_value());
			counter = 0;
		}

		{
			expected<void, TestError> e{unexpect, TestError::Error3};
			// Error3 -> fooError(Error3) = Error1
			expected<void, TestError> e1 = e.or_else(fooError);
			EATEST_VERIFY(counter == 1);
			EATEST_VERIFY(!e1.has_value());
			EATEST_VERIFY(e1.error() == TestError::Error1);
			counter = 0;
		}

		{
			expected<void, TestError> e{unexpect, TestError::Error2};
			// Error2 -> fooError(Error2) = valid
			expected<void, TestError> e1 = e.or_else(fooError);
			EATEST_VERIFY(counter == 1);
			EATEST_VERIFY(e1.has_value());
			counter = 0;
		}


		// transform from void -> vector<int>
		const auto getVector = [&counter]() -> vector<int>
		{
			if (counter > 3)
			{
				return {1, 2, 3, 4};
			}
			return {1, 2};
		};

		{
			expected<void, TestError> e;
			expected<vector<int>, TestError> e1 = e.transform(getVector);
			EATEST_VERIFY(e1.has_value());
			EATEST_VERIFY(e1.value().size() == 2);
			counter = 0;
		}

		{
			counter = 5;
			expected<void, TestError> e;
			expected<vector<int>, TestError> e1 = e.transform(getVector);
			EATEST_VERIFY(e1.has_value());
			EATEST_VERIFY(e1.value().size() == 4);
			counter = 0;
		}

		{
			expected<void, TestError> e{unexpect, TestError::Error3};
			expected<vector<int>, TestError> e1 = e.transform(getVector);
			EATEST_VERIFY(counter == 0);
			EATEST_VERIFY(!e1.has_value());
			EATEST_VERIFY(e1.error() == TestError::Error3);
			counter = 0;
		}

		// transform from int -> void
		const auto setCount = [&counter](int i) -> void { counter = i; };

		{
			expected<int, TestError> e{unexpect, TestError::Error3};
			// Error3 -> Error3
			expected<void, TestError> e1 = e.transform(setCount);
			EATEST_VERIFY(counter == 0);
			EATEST_VERIFY(!e1.has_value());
			EATEST_VERIFY(e1.error() == TestError::Error3);
			counter = 0;
		}

		{
			expected<int, TestError> e{5};
			// 5 -> valid
			expected<void, TestError> e1 = e.transform(setCount);
			EATEST_VERIFY(counter == 5);
			EATEST_VERIFY(e1.has_value());
			counter = 0;
		}

		// transform from int -> void
		const auto numberToError = [&counter](int i) -> TestError
		{
			counter = i;
			if (i <= 1)
			{
				return TestError::Error1;
			}
			if (i == 2)
			{
				return TestError::Error2;
			}
			return TestError::Error3;
		};

		{
			expected<void, int> e{unexpect, 5};
			expected<void, TestError> e1 = e.transform_error(numberToError);
			EATEST_VERIFY(counter == 5);
			EATEST_VERIFY(!e1.has_value());
			EATEST_VERIFY(e1.error() == TestError::Error3);
			counter = 0;
		}

		{
			expected<void, int> e{unexpect, 2};
			expected<void, TestError> e1 = e.transform_error(numberToError);
			EATEST_VERIFY(counter == 2);
			EATEST_VERIFY(!e1.has_value());
			EATEST_VERIFY(e1.error() == TestError::Error2);
			counter = 0;
		}
	}

	return nErrorCount;
}

int TestExpected() { return TestExpectedGeneric() + TestExpectedVoid(); }

#endif
