///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <EASTL/internal/special_member_functions.h>

EA_DISABLE_VC_WARNING(4623) // * was implicitly defined as deleted

namespace eastl
{
	namespace internal
	{

		// https://eel.is/c++draft/optional#ctor-7
		// https://eel.is/c++draft/variant#ctor-9
		template <class Base, class... Ts>
		using VariantOptionalCopyCtorLayer = conditional_t<
		    conjunction_v<is_copy_constructible<Ts>...>,
		    conditional_t<conjunction_v<is_trivially_copy_constructible<Ts>...>, Base, NonTrivialCopyCtor<Base>>,
		    DeletedCopyCtor<Base>>;

		// https://eel.is/c++draft/optional#ctor-12
		// https://eel.is/c++draft/variant#ctor-13
		template <class Base, class... Ts>
		using VariantOptionalMoveCtorLayer =
		    conditional_t<conjunction_v<is_nothrow_move_constructible<Ts>...>,
		                  conditional_t<conjunction_v<is_trivially_move_constructible<Ts>...>,
		                                VariantOptionalCopyCtorLayer<Base, Ts...>,
		                                NonTrivialMoveCtor<VariantOptionalCopyCtorLayer<Base, Ts...>>>,
		                  DeletedMoveCtor<VariantOptionalCopyCtorLayer<Base, Ts...>>>;

		// https://eel.is/c++draft/optional.assign#7
		// https://eel.is/c++draft/variant.assign#5
		template <class Base, class... Ts>
		using VariantOptionalCopyAssignLayer =
		    conditional_t<conjunction_v<is_copy_constructible<Ts>...> && conjunction_v<is_copy_assignable<Ts>...>,
		                  conditional_t<conjunction_v<is_trivially_copy_constructible<Ts>...> &&
		                                    conjunction_v<is_trivially_copy_assignable<Ts>...> &&
		                                    conjunction_v<is_trivially_destructible<Ts>...>,
		                                VariantOptionalMoveCtorLayer<Base, Ts...>,
		                                NonTrivialCopyAssign<VariantOptionalMoveCtorLayer<Base, Ts...>>>,
		                  DeletedCopyAssign<VariantOptionalMoveCtorLayer<Base, Ts...>>>;

		// https://eel.is/c++draft/optional#assign-8
		// https://eel.is/c++draft/optional#assign-13
		// https://eel.is/c++draft/variant#assign-7
		// https://eel.is/c++draft/variant#assign-10
		template <class Base, class... Ts>
		using VariantOptionalMoveAssignLayer =
		    conditional_t<conjunction_v<is_move_constructible<Ts>...> && conjunction_v<is_move_assignable<Ts>...>,
		                  conditional_t<conjunction_v<is_trivially_move_constructible<Ts>...> &&
		                                    conjunction_v<is_trivially_move_assignable<Ts>...> &&
		                                    conjunction_v<is_trivially_destructible<Ts>...>,
		                                VariantOptionalCopyAssignLayer<Base, Ts...>,
		                                NonTrivialMoveAssign<VariantOptionalCopyAssignLayer<Base, Ts...>>>,
		                  DeletedMoveAssign<VariantOptionalCopyAssignLayer<Base, Ts...>>>;

		// EnableSpecialMemberFunctions<Base, Ts...> is a helper class for optional<T> and variant<T> that need to
		// enable and disable special member functions (ie. copy/move constructor & assignment) based on member types
		// (Ts). The type being implemented (eg. variant) omits defining its special members and instead inherits from
		// EnableSpecialMemberFunctions<Base, Ts...>. The reason this is necessary is because conditionally disabling
		// special member functions using SFINAE (enable_if) allows the special member function to be *implicitly*
		// generated instead, which is undesirable and breaks standard conformance.
		//
		// None of this would be necessary if we could use C++20 requires clauses instead.
		//
		// Using a C++20 requires clause would work as intended instead, but we can't require C++20 (yet).
		//
		template <class Base, class... Ts> // requires SpecialMemberFunctions<base>
		using EnableVariantOptionalSpecialMemberFunctions = VariantOptionalMoveAssignLayer<Base, Ts...>;
	} // namespace internal
} // namespace eastl

EA_RESTORE_VC_WARNING()
