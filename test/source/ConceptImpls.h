/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef CONCEPTSIMPLS_H
#define CONCEPTSIMPLS_H

#include <EASTL/type_traits.h>

#if !defined(EA_COMPILER_NO_DEFAULTED_FUNCTIONS) && !defined(EA_COMPILER_NO_DELETED_FUNCTIONS)

#define EASTL_TEST_CONCEPT_IMPLS

// This header provides a variety of helper classes that have interfaces corresponding to the concepts used to specify
// requirements of many STL containers and algorithms. These helper classes are used in tests to verify that containers
// and algorithms do not impose stricter requirements than specified by the standard on their arguments.

// Destructible - only valid operation on this class is to destroy it.

class Destructible
{
public:
	~Destructible() = default;

	Destructible() = delete;
	Destructible(const Destructible&) = delete;
	Destructible(Destructible&&) = delete;
	Destructible& operator=(const Destructible&) = delete;
	Destructible& operator=(Destructible&&) = delete;
};

// Unfortunately not all compilers handle type_traits reliably correctly currently so we can't straightforwardly
// static_assert everything that should be true of this class
static_assert(eastl::is_destructible<Destructible>::value, "eastl::is_destructible<Destructible>::value");
// static_assert(!eastl::is_default_constructible<Destructible>::value,
// "!eastl::is_default_constructible<Destructible>::value");
// static_assert(!is_copy_constructible<Destructible>::value, "!eastl::is_copy_constructible<Destructible>::value");
static_assert(!eastl::is_copy_assignable<Destructible>::value, "!eastl::is_copy_assignable<Destructible>::value");
// static_assert(!eastl::is_move_constructible<Destructible>::value,
// "!eastl::is_move_constructible<Destructible>::value");
static_assert(!eastl::is_move_assignable<Destructible>::value, "!eastl::is_move_assignable<Destructible>::value");

class DefaultConstructible
{
public:
	static const int defaultValue = 42;

	DefaultConstructible() : value(defaultValue) {}
	~DefaultConstructible() = default;

	DefaultConstructible(const DefaultConstructible&) = delete;
	DefaultConstructible(DefaultConstructible&&) = delete;
	DefaultConstructible& operator=(const DefaultConstructible&) = delete;
	DefaultConstructible& operator=(DefaultConstructible&&) = delete;

	const int value;
};


struct NotDefaultConstructible
{
	NotDefaultConstructible() = delete;
};
static_assert(!eastl::is_default_constructible<NotDefaultConstructible>::value, "'NotDefaultConstructible' is default constructible.");


class CopyConstructible
{
public:
	static const int defaultValue = 42;
	static CopyConstructible Create()
	{
		CopyConstructible x;
		return x;
	}

	CopyConstructible(const CopyConstructible&) = default;
	~CopyConstructible() = default;

	CopyConstructible& operator=(const CopyConstructible&) = delete;
	CopyConstructible& operator=(CopyConstructible&&) = delete;

	const int value;

private:
	CopyConstructible() : value(defaultValue) {}
};

// Unfortunately not all compilers handle type_traits reliably correctly currently so we can't straightforwardly
// static_assert everything that should be true of this class
static_assert(eastl::is_destructible<CopyConstructible>::value, "eastl::is_destructible<CopyConstructible>::value");
// static_assert(!eastl::is_default_constructible<CopyConstructible>::value,
// "!eastl::is_default_constructible<CopyConstructible>::value");
// static_assert(is_copy_constructible<CopyConstructible>::value, "is_copy_constructible<CopyConstructible>::value");
static_assert(eastl::is_copy_constructible<CopyConstructible>::value,
			  "eastl::is_copy_constructible<CopyConstructible>::value");
static_assert(!eastl::is_copy_assignable<CopyConstructible>::value,
			  "!eastl::is_copy_assignable<CopyConstructible>::value");
// static_assert(!eastl::is_move_constructible<CopyConstructible>::value,
// "!eastl::is_move_constructible<CopyConstructible>::value");
static_assert(!eastl::is_move_assignable<CopyConstructible>::value,
			  "!eastl::is_move_assignable<CopyConstructible>::value");

class MoveConstructible
{
public:
	static const int defaultValue = 42;
	static MoveConstructible Create()
	{
		return MoveConstructible{};
	}

	MoveConstructible(MoveConstructible&& x) : value(x.value) {}
	~MoveConstructible() = default;

	MoveConstructible(const MoveConstructible&) = delete;
	MoveConstructible& operator=(const MoveConstructible&) = delete;
	MoveConstructible& operator=(MoveConstructible&&) = delete;

	const int value;

private:
	MoveConstructible() : value(defaultValue) {}
};

class MoveAssignable
{
public:
	static const int defaultValue = 42;
	static MoveAssignable Create()
	{
		return MoveAssignable{};
	}

	MoveAssignable(MoveAssignable&& x) : value(x.value) {}
	MoveAssignable& operator=(MoveAssignable&& x)
	{
		value = x.value;
		return *this;
	}
	~MoveAssignable() = default;

	MoveAssignable(const MoveAssignable&) = delete;
	MoveAssignable& operator=(const MoveAssignable&) = delete;

	int value;

private:
	MoveAssignable() : value(defaultValue) {}
};

struct MoveAndDefaultConstructible
{
	static const int defaultValue = 42;

	MoveAndDefaultConstructible() : value(defaultValue) {}
	MoveAndDefaultConstructible(MoveAndDefaultConstructible&& x) : value(x.value) {}
	~MoveAndDefaultConstructible() = default;

	MoveAndDefaultConstructible(const MoveAndDefaultConstructible&) = delete;
	MoveAndDefaultConstructible& operator=(const MoveAndDefaultConstructible&) = delete;
	MoveAndDefaultConstructible& operator=(MoveAndDefaultConstructible&&) = delete;

	const int value;
};

struct MissingMoveConstructor
{
	MissingMoveConstructor() {}
	MissingMoveConstructor(const MissingMoveConstructor&) {}
	MissingMoveConstructor& operator=(MissingMoveConstructor&&) { return *this; }
	MissingMoveConstructor& operator=(const MissingMoveConstructor&) { return *this; }
	bool operator<(const MissingMoveConstructor&) const { return true; }
};

struct MissingMoveAssignable
{
	MissingMoveAssignable() {}
	MissingMoveAssignable(const MissingMoveAssignable&) {}
	MissingMoveAssignable(MissingMoveAssignable&&) {}
	MissingMoveAssignable& operator=(const MissingMoveAssignable&) { return *this; }
	bool operator<(const MissingMoveAssignable&) const { return true; }
};

struct MissingEquality
{
	MissingEquality& operator==(const MissingEquality&) = delete;
};

#endif  // !defined(EA_COMPILER_NO_DEFAULTED_FUNCTIONS) && !defined(EA_COMPILER_NO_DELETED_FUNCTIONS)

#endif  // CONCEPTSIMPLS_H
