///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <EABase/eabase.h>

// We use a few c++17 features in the implementation of eastl::expceted, so we only provide
// it from c++17 onwards.
#if EA_COMPILER_CPP17_ENABLED

#include <EASTL/internal/type_pod.h>
#include <EASTL/type_traits.h>
#include <EASTL/internal/special_member_functions.h>

namespace eastl
{
namespace internal
{
// From https://eel.is/c++draft/expected.object.cons#10
//
// This constructor is trivial if
// is_trivially_copy_constructible_v<T> is true and
// is_trivially_copy_constructible_v<E> is true
//
// From https://eel.is/c++draft/expected.object.cons#9
//
// Remarks: This constructor is defined as deleted unless
// is_copy_constructible_v<T> is true and
// is_copy_constructible_v<E> is true.
template <class Base, class... Ts>
using ExpectedCopyLayer =
conditional_t<(is_copy_constructible_v<Ts> && ...),
	conditional_t<(is_trivially_copy_constructible_v<Ts> && ...), Base, NonTrivialCopyCtor<Base>>,
	DeletedCopyCtor<Base>>;

// From https://eel.is/c++draft/expected.object.cons#16
//
// This constructor is trivial if
// is_trivially_move_constructible_v<T> is true and
// is_trivially_move_constructible_v<E> is true.
template <class Base, class... Ts>
using ExpectedMoveLayer = conditional_t<(is_move_constructible_v<Ts> && ...),
	conditional_t<(is_trivially_move_constructible_v<Ts> && ...),
	ExpectedCopyLayer<Base, Ts...>,
	NonTrivialMoveCtor<ExpectedCopyLayer<Base, Ts...>>>,
	DeletedMoveCtor<ExpectedCopyLayer<Base, Ts...>>>;

// NOTE: The fact that we have a nothrow check here might be annoying, but
// it's in the standard, so it'll make the eventual aliasing easier.
//
// From https://eel.is/c++draft/expected.object.assign#4
//
// Remarks: This operator is defined as deleted unless:
// is_copy_assignable_v<T> is true and
// is_copy_constructible_v<T> is true and
// is_copy_assignable_v<E> is true and
// is_copy_constructible_v<E> is true and
// is_nothrow_move_constructible_v<T> || is_nothrow_move_constructible_v<E> is true.
template <class Base, class... Ts>
using ExpectedCopyAssignLayer =
conditional_t<((is_copy_constructible_v<Ts> && ...) && (is_copy_assignable_v<Ts> && ...) &&
	(is_nothrow_move_constructible_v<Ts> || ...)),
	NonTrivialCopyAssign<ExpectedMoveLayer<Base, Ts...>>,
	DeletedCopyAssign<ExpectedMoveLayer<Base, Ts...>>>;

// NOTE: The fact that we have a nothrow check here might be annoying, but
// it's in the standard, so it'll make the eventual aliasing easier.
//
// From https://eel.is/c++draft/expected.object.assign#5
//
// Constraints:
// is_move_constructible_v<T> is true and
// is_move_assignable_v<T> is true and
// is_move_constructible_v<E> is true and
// is_move_assignable_v<E> is true and
// is_nothrow_move_constructible_v<T> || is_nothrow_move_constructible_v<E> is true.
//
// NOTE: in the standard, it doesn't say that the move assignment should be `deleted`
// only that it should be constrained (i.e. should not be declared if the constraints
// are not satisfied), so what we're doing here might seem incorrect. In practice, if
// the constraints aren't satisified, then eastl::expcted (which will inherit from
// this) will have an implicitly deleted move assignment, however, implicitly deleted
// move assigment operators are ignored by overload resolution (see
// https://eel.is/c++draft/class.copy.assign#note-4), which is equivalent to it being
// constrained. This, in particular, means that an expression `x = move(y)` could still
// compile and call the copy assignment operator (if that exists) instead, exactly as
// if the move assignment was constrained instead of deleted.
template <class Base, class... Ts>
using ExpectedMoveAssignLayer =
conditional_t<((is_move_constructible_v<Ts> && ...) && (is_move_assignable_v<Ts> && ...) &&
	(is_nothrow_move_constructible_v<Ts> || ...)),
	NonTrivialMoveAssign<ExpectedCopyAssignLayer<Base, Ts...>>,
	DeletedMoveAssign<ExpectedCopyAssignLayer<Base, Ts...>>>;

// EnableExpectedSpecialMemberFunctions<Base, Ts...> is a helper for expected<T> that need to enable and disable special member functions
// (ie. copy/move constructor & assignment) based on member types (Ts). The type being implemented (expected)
// omits defining its special members and instead inherits from EnableExpectedSpecialMemberFunctions<Base, Ts...>. The reason
// this is necessary is because conditionally disabling special member functions using SFINAE (enable_if) allows the
// special member function to be *implicitly* generated instead, which is undesirable and breaks standard conformance.
//
// None of this would be necessary if we could use C++20 requires clauses instead.
//
// Using a C++20 requires clause would work as intended instead, but we can't require C++20 (yet).
//
template <class Base, class... Ts> // requires SpecialMemberFunctions<base>
using EnableExpectedSpecialMemberFunctions = ExpectedMoveAssignLayer<Base, Ts...>;
} // namespace internal
} // namespace eastl

#endif
