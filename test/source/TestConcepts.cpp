/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EASTL/internal/concepts.h>
#include <EASTL/memory.h>

#include "EASTL/unique_ptr.h"
#include "EASTLTest.h"

struct NoExceptFalseDestructor
{
	~NoExceptFalseDestructor() noexcept(false) = default;
};

// In GCC prior to version 14, the following static_assert fails:
//
//    static_assert(!noexcept(std::declval<NoExceptFalseDestructor&>().~NoExceptFalseDestructor()), "bad noexcept!");
//
// our implementation of some of these concepts depends on that working correctly so we don't
// do these tests in older versions of GCC. Clang handles this properly and on MSVC we use
// __is_nothrow_destructible instead of rolling our own.
#if defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION <= 14000)
	#define EA_TEST_NoExceptFalseDestructor 0
#else
	#define EA_TEST_NoExceptFalseDestructor 1
#endif

struct From;

struct To
{
	To(const From&)
	{}

	To(const int&) // To can be implicitly constructed from int, but int cannot be explicitly converted to To.
	{}
};

struct From
{
	explicit operator To() const
	{
		return To{*this};
	}
};

struct NotMoveable
{
	NotMoveable() = default;

	NotMoveable(const NotMoveable&) = delete;
	NotMoveable& operator=(const NotMoveable&) = delete;
	NotMoveable(NotMoveable&&) = delete;
	NotMoveable& operator=(NotMoveable&&) = delete;

	~NotMoveable() = default;
};

int TestConcepts()
{
	using namespace eastl;

	int nErrorCount = 0;

	// destructible
	{
		static_assert(internal::concepts::destructible<int>, "destructible concept failure.");
		static_assert(!internal::concepts::destructible<void>, "destructible concept failure.");
#if EA_TEST_NoExceptFalseDestructor
		static_assert(!internal::concepts::destructible<NoExceptFalseDestructor>, "destructible concept failure.");
#endif
	}

	// constructible_from
	{
		static_assert(internal::concepts::constructible_from<int>, "constructible_from concept failure.");
		static_assert(internal::concepts::constructible_from<int, int>, "constructible_from concept failure.");
		static_assert(internal::concepts::constructible_from<int, bool>, "constructible_from concept failure.");
		static_assert(internal::concepts::constructible_from<To, From>, "constructible_from concept failure.");

		static_assert(!internal::concepts::constructible_from<int, int*>, "constructible_from concept failure.");
		static_assert(!internal::concepts::constructible_from<int, bool, bool>, "constructible_from concept failure.");
		static_assert(!internal::concepts::constructible_from<int, void>, "constructible_from concept failure.");
#if EA_TEST_NoExceptFalseDestructor
		static_assert(!internal::concepts::constructible_from<NoExceptFalseDestructor>, "constructible_from concept failure.");
		static_assert(!internal::concepts::constructible_from<NoExceptFalseDestructor, NoExceptFalseDestructor>, "constructible_from concept failure.");
#endif
	}

	// constructible_to
	{
		static_assert(internal::concepts::convertible_to<bool, int>, "convertible_to concept failure.");
		static_assert(internal::concepts::convertible_to<int, int>, "convertible_to concept failure.");
		static_assert(internal::concepts::convertible_to<NoExceptFalseDestructor, NoExceptFalseDestructor>, "convertible_to concept failure.");
		static_assert(internal::concepts::convertible_to<From, To>, "convertible_to concept failure.");

		static_assert(!internal::concepts::convertible_to<From, int>, "convertible_to concept failure."); // No implicit conversion for int.
		static_assert(!internal::concepts::convertible_to<int*, int>, "convertible_to concept failure.");
		static_assert(!internal::concepts::convertible_to<void, int>, "convertible_to concept failure.");
	}

	// move_constructible
	{
		static_assert(internal::concepts::move_constructible<int>, "move_constructible concept failure.");
		static_assert(internal::concepts::move_constructible<unique_ptr<int>>, "move_constructible concept failure.");

#if EA_TEST_NoExceptFalseDestructor
		static_assert(!internal::concepts::move_constructible<NoExceptFalseDestructor>, "move_constructible concept failure.");
#endif
		static_assert(!internal::concepts::move_constructible<NotMoveable>, "move_constructible concept failure.");
	}

	// copy_constructible
	{
		static_assert(internal::concepts::copy_constructible<int>, "copy_constructible concept failure.");
		
		static_assert(!internal::concepts::copy_constructible<unique_ptr<int>>, "copy_constructible concept failure.");
#if EA_TEST_NoExceptFalseDestructor
		static_assert(!internal::concepts::move_constructible<NoExceptFalseDestructor>, "copy_constructible concept failure.");
#endif
		static_assert(!internal::concepts::move_constructible<NotMoveable>, "copy_constructible concept failure.");
	}

	return nErrorCount;
}
