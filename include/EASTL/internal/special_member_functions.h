///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <EASTL/internal/type_pod.h>
#include <EASTL/type_traits.h>

EA_DISABLE_VC_WARNING(4623) // * was implicitly defined as deleted

namespace eastl
{
namespace internal
{

// Helper types for optional<T>, variant<Ts> and expected<T, E> that need to enable and disable special member functions
// (ie. copy/move constructor & assignment) based on member types (Ts).
//
// Requirements for Base, using C++20 concepts notation:
// ConstructFrom and AssignFrom are the implementation for the constructor and assignment of the type being implemented.
// 
// template<typename SMFBase> concept SpecialMemberFunctions = requires(const SMFBase& base)
// {
//  { SMFBase::ConstructFrom(base) };
//  { SMFBase::ConstructFrom(eastl::move(base)) };
//  { SMFBase::AssignFrom(base) };
//  { SMFBase::AssignFrom(eastl::move(base)) };
// };


template <class Base> // requires SpecialMemberFunctions<Base>
struct NonTrivialCopyCtor : Base
{
	using Base::Base;

	NonTrivialCopyCtor() = default;
	NonTrivialCopyCtor(const NonTrivialCopyCtor& other) noexcept(
		noexcept(eastl::declval<Base>().ConstructFrom(static_cast<const Base&>(other))))
	{
		Base::ConstructFrom(static_cast<const Base&>(other));
	}
	NonTrivialCopyCtor(NonTrivialCopyCtor&&) = default;
	NonTrivialCopyCtor& operator=(const NonTrivialCopyCtor&) = default;
	NonTrivialCopyCtor& operator=(NonTrivialCopyCtor&&) = default;
};

template <class Base> // requires SpecialMemberFunctions<Base>
struct DeletedCopyCtor : Base
{
	using Base::Base;

	DeletedCopyCtor() = default;
	DeletedCopyCtor(const DeletedCopyCtor&) = delete;
	DeletedCopyCtor(DeletedCopyCtor&&) = default;
	DeletedCopyCtor& operator=(const DeletedCopyCtor&) = default;
	DeletedCopyCtor& operator=(DeletedCopyCtor&&) = default;
};

template <class CopyBase> // requires SpecialMemberFunctions<CopyBase>
struct NonTrivialMoveCtor : CopyBase
{
	using SelectedBase = CopyBase;
	using SelectedBase::SelectedBase;

	NonTrivialMoveCtor() = default;
	NonTrivialMoveCtor(const NonTrivialMoveCtor&) = default;
	NonTrivialMoveCtor(NonTrivialMoveCtor&& other) noexcept(
		noexcept(eastl::declval<SelectedBase>().ConstructFrom(static_cast<SelectedBase&&>(other))))
	{
		SelectedBase::ConstructFrom(static_cast<SelectedBase&&>(other));
	}
	NonTrivialMoveCtor& operator=(const NonTrivialMoveCtor&) = default;
	NonTrivialMoveCtor& operator=(NonTrivialMoveCtor&&) = default;
};

template <class CopyBase> // requires SpecialMemberFunctions<CopyBase>
struct DeletedMoveCtor : CopyBase
{
	using SelectedBase = CopyBase;
	using SelectedBase::SelectedBase;

	DeletedMoveCtor() = default;
	DeletedMoveCtor(const DeletedMoveCtor&) = default;
	DeletedMoveCtor(DeletedMoveCtor&& other) = delete;
	DeletedMoveCtor& operator=(const DeletedMoveCtor&) = default;
	DeletedMoveCtor& operator=(DeletedMoveCtor&&) = default;
};

template <class MoveBase> // requires SpecialMemberFunctions<MoveBase>
struct NonTrivialCopyAssign : MoveBase
{
	using SelectedBase = MoveBase;
	using SelectedBase::SelectedBase;

	NonTrivialCopyAssign() = default;
	NonTrivialCopyAssign(const NonTrivialCopyAssign&) = default;
	NonTrivialCopyAssign(NonTrivialCopyAssign&&) = default;

	NonTrivialCopyAssign& operator=(const NonTrivialCopyAssign& other) noexcept(
		noexcept(eastl::declval<SelectedBase>().AssignFrom(static_cast<const SelectedBase&>(other))))
	{
		SelectedBase::AssignFrom(static_cast<const SelectedBase&>(other));
		return *this;
	}
	NonTrivialCopyAssign& operator=(NonTrivialCopyAssign&&) = default;
};

template <class MoveBase> // requires SpecialMemberFunctions<MoveBase>
struct DeletedCopyAssign : MoveBase
{
	using SelectedBase = MoveBase;
	using SelectedBase::SelectedBase;

	DeletedCopyAssign() = default;
	DeletedCopyAssign(const DeletedCopyAssign&) = default;
	DeletedCopyAssign(DeletedCopyAssign&&) = default;
	DeletedCopyAssign& operator=(const DeletedCopyAssign&) = delete;
	DeletedCopyAssign& operator=(DeletedCopyAssign&&) = default;
};

template <class CopyAssignBase> // requires SpecialMemberFunctions<CopyAssignBase>
struct NonTrivialMoveAssign : CopyAssignBase
{
	using SelectedBase = CopyAssignBase;
	using SelectedBase::SelectedBase;

	NonTrivialMoveAssign() = default;
	NonTrivialMoveAssign(const NonTrivialMoveAssign&) = default;
	NonTrivialMoveAssign(NonTrivialMoveAssign&&) = default;
	NonTrivialMoveAssign& operator=(const NonTrivialMoveAssign&) = default;

	NonTrivialMoveAssign& operator=(NonTrivialMoveAssign&& other) noexcept(
		noexcept(eastl::declval<SelectedBase>().AssignFrom(static_cast<SelectedBase&&>(other))))
	{
		SelectedBase::AssignFrom(static_cast<SelectedBase&&>(other));
		return *this;
	}
};

template <class CopyAssignBase> // requires SpecialMemberFunctions<CopyAssignBase>
struct DeletedMoveAssign : CopyAssignBase
{
	using SelectedBase = CopyAssignBase;
	using SelectedBase::SelectedBase;

	DeletedMoveAssign() = default;
	DeletedMoveAssign(const DeletedMoveAssign&) = default;
	DeletedMoveAssign(DeletedMoveAssign&&) = default;
	DeletedMoveAssign& operator=(const DeletedMoveAssign&) = default;
	DeletedMoveAssign& operator=(DeletedMoveAssign&&) = delete;
};

} // namespace internal
} // namespace eastl

EA_RESTORE_VC_WARNING()

