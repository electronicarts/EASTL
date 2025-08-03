// Copyright (c) Electronic Arts Inc. All rights reserved.

#pragma once

//
// introduction
// --------------
// 
// hello there.
// 
// Flags are usually defined on a per-bit basis, and stored in integrals. This file
// defines a templated type that encapsulates that logic as a thin-wrapper around an
// integral. The benefit here is that [i.e.] enums that are not already shifted can
// be seamlessly used if the corresponding flags type knows they're pre-shifted.
// 
// 
// context
// ---------
// Here's a really common example where everything is _easy_:
// (operators & conversions left out for brevity)
// 
//    enum class Dragons : uint32_t { Fire = 1, Ice = 2, Poison = 4, Golden = 8 };
//    uint32_t flags = Dragons::Fire | Dragons::Poison;
// 
// Here's a really common example where things are _harder_:
// 
//    enum class Knights : uint16_t { Armoured, Cowardly, Shining, Ni };
//    uint32_t flags = (1 << Knights::Armoured) | (1 << Knights::Shining);
// 
// Fun fact: the shift operator's resultant type is the type of the LHS, and
// bitwise or-operator will promote its arguments of uint16_t to int on 64-bit
// systems, meaning there are _many more_ omitted casts then may first appear.
// 
// In the above examples, we would call `Dragons` _post-shifted_, and `Knights`
// _pre-shifted_. Example code perhaps trivialises the issue; in large codebases
// keeping track of which flags are pre- or post-shifted can be annoying, and
// there is genuine need for both mixed within the same code. Awful!
// 
// 
// basic_flags (prefer using bitflags/maskflags)
// -----------------------------------------------
// This structure wraps an integral and provides all common bitwise operations for
// that integral. It will automatically shift pre-shifted flags (kind of, you need
// to tell it that a flag is pre-shifted (bitflags), or post-shifted (maskflags)).
// 
// This example demonstrates pre-shifted flags being used without any shifting
// required on the user's part. If the user had accidentally used eastl::maskflags<>
// instead of bitflags<>, then no shifting would occur and there would be _bugs_:
// 
//    enum class Knights : uint16_t { Armoured, Cowardly, Shining, Ni };
//    using KnightFlags = eastl::bitflags<Knights>;
// 
//    // default construct = all bits zero
//    KnightFlags knight_useless;
// 
//    // takes an initializer_list if so desired
//    KnightFlags knight_in_shining_armour{Knights::Armoured, Knights::Shining};
//    
//    // supports all typical operators
//    if (knight_in_shining_armour & Knights::Shining)
//    {
//       // ...
//    }
// 
//    // being brave is part of the job description
//    knight_useless = Knights::Cowardly;
// 
//    // integrals would support implicit conversion to bool, so we do too
//    if (knight_useless)
//    {
//    }
// 
// 
// 
//
// macros for defining flags
// ---------------------------
// 
// The following macros are shorthands for defining a flag-type based off
// an enum-type. This enum-type can either be known ahead of time, using
// EASTL_DECLARE_BITFLAGS or EASTL_DECLARE_MASKFLAGS, or it can be defined
// directly then and there, using the "_ENUM_CLASS" versions of the macros.
// 
// These macros help by defining certain bitwise operators for enum-classes.
// 
// 
// 
// 
// EASTL_DECLARE_BITFLAGS
// EASTL_DECLARE_MASKFLAGS
// -------------------------
// These macros simply take an existing enum-type and define an alias for a
// flags-type (bitflags or maskflags) of that enum-type:
// 
//    enum class DragonType { Happy, Sad, Drunk };
// 
//    EASTL_DECLARE_BITFLAGS(DragonFlags, DragonType);
// 
//      ^ the above macro has expanded to something akin to:
// 
//        using DragonFlags = ::eastl::bitflags<DragonType>;
//        inline constexpr DragonFlags operator | (DragonType lhs, DragonType rhs) { /* snip */ }
//
// This example shows a post-shifted enum using MASKFLAGS. Note that pre-shifted
// flags can be combined and thus more than the bitwise-or operator has been
// provided (the bitwse and-operator and xor-operator and included free of charge):
//
//    enum class WizardType
//    {
//        Brilliant = 1, Imbecile = 2, Evil = 4, Forgetful = 8,
//        MaybeHarmless = (Evil | Forgetful)
//    };
// 
//    EASTL_DECLARE_MASKFLAGS(WizardFlags, WizardType);
// 
//      ^ macro expansion for the above macro looks like:
// 
//        using WizardFlags = ::eastl::maskflags<WizardType>;
//        inline constexpr WizardFlags operator | (WizardType lhs, WizardType rhs) { /* snip */ }
//        inline constexpr WizardFlags operator & (WizardType lhs, WizardType rhs) { /* snip */ }
//        inline constexpr WizardFlags operator ^ (WizardType lhs, WizardType rhs) { /* snip */ }
// 
// 
// 
// 
// EASTL_DECLARE_BITFLAGS_ENUM_CLASS
// EASTL_DECLARE_MASKFLAGS_ENUM_CLASS
// -----------------------------------
// These macros are convenience macros to both declare the flag-type, and
// in-place define an enum-class (the second parameter). Operators for the
// enum-type are automatically generated too:
// 
// Note that this first example is again using BITFLAGS.
// 
//    EASTL_DECLARE_BITFLAGS_ENUM_CLASS(DragonFlags, DragonType)
//    {
//        Jubilent,
//        Depressed,
//        Smashed
//    };
// 
// This generates code similar to the following:
// 
//    enum class DragonType;
//    using DragonFlags = ::eastl::bitflags<DragonType>;
//    inline constexpr DragonFlags operator | (DragonType lhs, DragonType rhs) { /* snip */ }
//    enum class DragonType
//    {
//        Jubilent,
//        Depressed,
//        Smashed
//    };
// 
// 
// Note: You will still need to pre-shift your enum values yourself when using
// the MASKFLAGS version of these macros, just as if you were writing the enum
// without the macro:
// 
//    EASTL_DECLARE_MASKFLAGS_ENUM_CLASS(SquireFlags, Squires)
//    {
//        Absent = 0,
//        Inexperienced = (1<<0),
//        Keen =          (1<<1),
//        Jaded =         (1<<2),
//        Competant =     (1<<3),
// 
//        FreshOutOfUni = Inexperienced | Keen
//    };
// 
// Macro expansion:
// 
//    enum class Squires;
//    using SquireFlags = ::eastl::bitflags<Squires>;
//    inline constexpr SquireFlags operator | (Squires lhs, Squires rhs) { /* snip */ }
//    inline constexpr SquireFlags operator & (Squires lhs, Squires rhs) { /* snip */ }
//    inline constexpr SquireFlags operator ^ (Squires lhs, Squires rhs) { /* snip */ }
//    enum class Squires
//    {
//        Absent = 0,
//        Inexperienced = (1<<0),
//        Keen =          (1<<1),
//        Jaded =         (1<<2),
//        Competant =     (1<<3),
// 
//        FreshOutOfUni = Inexperienced | Keen
//    };
// 
// Usage:
// 
//    SquireFlags flags{Squires::FreshOutOfUni};
//    if (flags & Squires::Keen) { /* snip */ }
//    if (flags != Squires::Absent) { /* snip */ }
//


#include <EASTL/type_traits.h>
#include <initializer_list>



// forward-declares
namespace eastl
{
	template <typename Flag, typename Marshaller>
	struct basic_flags;

	template <typename TagType, typename FlagType>
	struct flag_marshaller;

	// tags to determine between flag-types
	struct bitflag_tag {};
	struct maskflag_tag {};

	// aliases
	template <typename Flag>
	using bitflags = basic_flags<Flag, flag_marshaller<bitflag_tag, Flag>>;

	template <typename Flag>
	using maskflags = basic_flags<Flag, flag_marshaller<maskflag_tag, Flag>>;
}


// the mask-type for an enum of a given size
namespace eastl::detail
{
	template <size_t N>
	struct flags_mask_type;

	template <> struct flags_mask_type<1> { using type = uint8_t; };
	template <> struct flags_mask_type<2> { using type = uint16_t; };
	template <> struct flags_mask_type<4> { using type = uint32_t; };
	template <> struct flags_mask_type<8> { using type = uint64_t; };

	template <typename T>
	using flags_mask_type_t = typename flags_mask_type<sizeof(T)>::type;
}


namespace eastl
{
	template <typename Flag, typename Marshaller>
	struct basic_flags
	{
		using flag_type = Flag;
		using marshaller_type = Marshaller;
		using mask_type = detail::flags_mask_type_t<Flag>;

		static_assert(is_integral_v<mask_type>, "mask_type must be integral");
		static_assert(is_unsigned_v<mask_type>, "mask_type must be unsigned");

		// constructors
		static constexpr basic_flags from_mask(mask_type) noexcept;

		constexpr basic_flags() noexcept = default;
		constexpr basic_flags(flag_type) noexcept;
		constexpr basic_flags(const basic_flags&) = default;
		constexpr basic_flags(std::initializer_list<flag_type>) noexcept;

		// explicit conversions
		constexpr operator bool() const noexcept;
		constexpr explicit operator mask_type() const noexcept;

		// standard flags operators
		constexpr basic_flags& operator = (flag_type) noexcept;
		constexpr basic_flags& operator = (const basic_flags&) noexcept = default;

		constexpr basic_flags& operator |= (basic_flags) noexcept;
		constexpr basic_flags& operator &= (basic_flags) noexcept;
		constexpr basic_flags& operator ^= (basic_flags) noexcept;
		
		constexpr basic_flags& operator |= (flag_type) noexcept;
		constexpr basic_flags& operator &= (flag_type) noexcept;
		constexpr basic_flags& operator ^= (flag_type) noexcept;

		constexpr basic_flags operator ~ () const noexcept;

		// named versions of common operations
		constexpr basic_flags& set(flag_type, bool enabled) noexcept;
		constexpr basic_flags& set(flag_type) noexcept;
		constexpr basic_flags& unset(flag_type) noexcept;
		constexpr basic_flags& toggle(flag_type) noexcept;
		constexpr void clear() noexcept;
		constexpr void reset(mask_type = 0u) noexcept;

	private:
		constexpr basic_flags(mask_type mask) noexcept
			: m_mask{mask}
		{}

	private:
		mask_type m_mask{};

	private:
		// friend non-member bitwise operations
		template <typename Y, typename U> friend constexpr basic_flags<Y, U> operator & (basic_flags<Y, U>, basic_flags<Y, U>) noexcept;
		template <typename Y, typename U> friend constexpr basic_flags<Y, U> operator | (basic_flags<Y, U>, basic_flags<Y, U>) noexcept;
		template <typename Y, typename U> friend constexpr basic_flags<Y, U> operator ^ (basic_flags<Y, U>, basic_flags<Y, U>) noexcept;

		template <typename Y, typename U> friend constexpr basic_flags<Y, U> operator & (basic_flags<Y, U>, typename basic_flags<Y, U>::flag_type) noexcept;
		template <typename Y, typename U> friend constexpr basic_flags<Y, U> operator | (basic_flags<Y, U>, typename basic_flags<Y, U>::flag_type) noexcept;
		template <typename Y, typename U> friend constexpr basic_flags<Y, U> operator ^ (basic_flags<Y, U>, typename basic_flags<Y, U>::flag_type) noexcept;

		template <typename Y, typename U> friend constexpr basic_flags<Y, U> operator & (typename basic_flags<Y, U>::flag_type, basic_flags<Y, U>) noexcept;
		template <typename Y, typename U> friend constexpr basic_flags<Y, U> operator | (typename basic_flags<Y, U>::flag_type, basic_flags<Y, U>) noexcept;
		template <typename Y, typename U> friend constexpr basic_flags<Y, U> operator ^ (typename basic_flags<Y, U>::flag_type, basic_flags<Y, U>) noexcept;

		// friend comparison operators
		template <typename Y, typename U> friend constexpr bool operator == (basic_flags<Y, U>, basic_flags<Y, U>) noexcept;
		template <typename Y, typename U> friend constexpr bool operator == (basic_flags<Y, U>, typename basic_flags<Y, U>::flag_type) noexcept;
		template <typename Y, typename U> friend constexpr bool operator == (typename basic_flags<Y, U>::flag_type, basic_flags<Y, U>) noexcept;

		template <typename Y, typename U> friend constexpr bool operator != (basic_flags<Y, U>, basic_flags<Y, U>) noexcept;
		template <typename Y, typename U> friend constexpr bool operator != (basic_flags<Y, U>, typename basic_flags<Y, U>::flag_type) noexcept;
		template <typename Y, typename U> friend constexpr bool operator != (typename basic_flags<Y, U>::flag_type, basic_flags<Y, U>) noexcept;
	};
}







//
// flag_marshaller
// -----------------
// marshalls the flag-type into a mask-type we can perform bitwise operations
// on with our mask member
// 
// 'bitflags' expects the flags to be sequentially numbered, where the
// value of each flag signifies the bit-position within the mask-type that
// will be affected
// 
//      // the bottom four bits (position 0, 1, 2, 3) will be affected
//      enum class MaidenType { Distressed, Defiant, Conniving, Charasmatic };
// 
//      // bits at positions 3, 4, and 8
//      enum class KnightType { Armoured = 3, Lazy = 4, Jousting = 8 };
// 
// 
// 'maskflags' on the other hand, expects the values of the flags to be
// _already_ shifted to the correct position. taking the above examples
// and rewriting them as bitmasks:
// 
//      // the bottom four bits (position 0, 1, 2, 3) will be affected
//      enum class MaidenType { Distressed = 1, Defiant = 2, Conniving = 4, Charasmatic = 8 };
// 
//      // bits at positions 3, 4, and 8 (note that "Absent" contributes nothing)
//      enum class KnightType { Absent = 0, Armoured = (1<<3), Lazy = (1<<4), Jousting = (1<<8) };
//
//
namespace eastl
{
	template <typename TagType, typename FlagType>
	struct flag_marshaller
	{
		static_assert(!std::is_integral_v<FlagType>);

		using tag_type = TagType;
		using flag_type = FlagType;
		using mask_type = detail::flags_mask_type_t<FlagType>;

		// we really want C++ constexpr-if
		template <size_t AdditionalOffset = 0>
		static constexpr mask_type to_mask(flag_type f)
		{
			return to_mask_impl<AdditionalOffset>(tag_type{}, f);
		}

	private:
		template <size_t AdditionalOffset>
		static constexpr mask_type to_mask_impl(bitflag_tag, flag_type f)
		{
			return mask_type{1} << static_cast<mask_type>(f) << AdditionalOffset;
		}

		template <size_t AdditionalOffset>
		static constexpr mask_type to_mask_impl(maskflag_tag, flag_type f)
		{
			return static_cast<mask_type>(f) << AdditionalOffset;
		}
	};
}




//
// basic_flags implementation
//
namespace eastl
{
	template <typename F, typename T>
	inline constexpr basic_flags<F, T> basic_flags<F, T>::from_mask(mask_type mask) noexcept
	{
		return basic_flags<F, T>{mask};
	}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T>::basic_flags(flag_type flag) noexcept
		: m_mask{marshaller_type::to_mask(flag)}
	{}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T>::basic_flags(std::initializer_list<flag_type> flags) noexcept
	{
		for (auto f : flags)
			set(f);
	}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T>::operator bool() const noexcept
	{
		return static_cast<bool>(m_mask);
	}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T>::operator mask_type() const noexcept
	{
		return m_mask;
	}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T>& basic_flags<F, T>::operator = (flag_type flag) noexcept
	{
		m_mask = marshaller_type::to_mask(flag);
		return *this;
	}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T>& basic_flags<F, T>::operator |= (basic_flags<F, T> flag) noexcept
	{
		m_mask |= flag.m_mask;
		return *this;
	}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T>& basic_flags<F, T>::operator &= (basic_flags<F, T> flag) noexcept
	{
		m_mask &= flag.m_mask;
		return *this;
	}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T>& basic_flags<F, T>::operator ^= (basic_flags<F, T> flag) noexcept
	{
		m_mask ^= flag.m_mask;
		return *this;
	}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T>& basic_flags<F, T>::operator |= (flag_type flag) noexcept
	{
		m_mask |= marshaller_type::to_mask(flag);
		return *this;
	}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T>& basic_flags<F, T>::operator &= (flag_type flag) noexcept
	{
		m_mask &= marshaller_type::to_mask(flag);
		return *this;
	}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T>& basic_flags<F, T>::operator ^= (flag_type flag) noexcept
	{
		m_mask ^= marshaller_type::to_mask(flag);
		return *this;
	}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T> basic_flags<F, T>::operator ~ () const noexcept
	{
		return basic_flags<F, T>{~m_mask};
	}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T>& basic_flags<F, T>::set(flag_type flag, bool enabled) noexcept
	{
		mask_type const rhs = marshaller_type::to_mask(flag);
		m_mask = (m_mask & ~rhs) ^ (rhs * static_cast<mask_type>(enabled));
		return *this;
	}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T>& basic_flags<F, T>::set(flag_type flag) noexcept
	{
		m_mask |= marshaller_type::to_mask(flag);
		return *this;
	}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T>& basic_flags<F, T>::unset(flag_type flag) noexcept
	{
		m_mask &= ~marshaller_type::to_mask(flag);
		return *this;
	}

	template <typename F, typename T>
	inline constexpr basic_flags<F, T>& basic_flags<F, T>::toggle(flag_type flag) noexcept
	{
		m_mask ^= marshaller_type::to_mask(flag);
		return *this;
	}

	template <typename F, typename T>
	inline constexpr void basic_flags<F, T>::clear() noexcept
	{
		m_mask = 0u;
	}

	template <typename F, typename T>
	inline constexpr void basic_flags<F, T>::reset(mask_type mask) noexcept
	{
		m_mask = mask;
	}
}


//
// BF_MASK_CAST_
// ---------------
//
// bitwise operations will promote to int when available.
//
// that is undesireable, as we will then encounter narrowing conversion warnings
// when assigning to our m_mask of those narrower types. so static-cast.
//
#define BF_MASK_CAST_(expr) static_cast<typename basic_flags<F, M>::mask_type>(expr)


// non-member operators
namespace eastl
{

	template <typename F, typename M>
	inline constexpr basic_flags<F, M> operator & (basic_flags<F, M> lhs, basic_flags<F, M> rhs) noexcept
	{
		return basic_flags<F, M>{BF_MASK_CAST_(lhs.m_mask & rhs.m_mask)};
	}

	template <typename F, typename M>
	inline constexpr basic_flags<F, M> operator | (basic_flags<F, M> lhs, basic_flags<F, M> rhs) noexcept
	{
		return basic_flags<F, M>{BF_MASK_CAST_(lhs.m_mask | rhs.m_mask)};
	}

	template <typename F, typename M>
	inline constexpr basic_flags<F, M> operator ^ (basic_flags<F, M> lhs, basic_flags<F, M> rhs) noexcept
	{
		return basic_flags<F, M>{BF_MASK_CAST_(lhs.m_mask ^ rhs.m_mask)};
	}

	template <typename F, typename M>
	inline constexpr basic_flags<F, M> operator & (basic_flags<F, M> lhs, typename basic_flags<F, M>::flag_type rhs) noexcept
	{
		return basic_flags<F, M>{BF_MASK_CAST_(lhs.m_mask & M::to_mask(rhs))};
	}

	template <typename F, typename M>
	inline constexpr basic_flags<F, M> operator | (basic_flags<F, M> lhs, typename basic_flags<F, M>::flag_type rhs) noexcept
	{
		return basic_flags<F, M>{BF_MASK_CAST_(lhs.m_mask | M::to_mask(rhs))};
	}

	template <typename F, typename M>
	inline constexpr basic_flags<F, M> operator ^ (basic_flags<F, M> lhs, typename basic_flags<F, M>::flag_type rhs) noexcept
	{
		return basic_flags<F, M>{BF_MASK_CAST_(lhs.m_mask ^ M::to_mask(rhs))};
	}
	
	template <typename F, typename M>
	inline constexpr basic_flags<F, M> operator & (typename basic_flags<F, M>::flag_type lhs, basic_flags<F, M> rhs) noexcept
	{
		return basic_flags<F, M>{BF_MASK_CAST_(M::to_mask(lhs) & rhs.m_mask)};
	}

	template <typename F, typename M>
	inline constexpr basic_flags<F, M> operator | (typename basic_flags<F, M>::flag_type lhs, basic_flags<F, M> rhs) noexcept
	{
		return basic_flags<F, M>{BF_MASK_CAST_(M::to_mask(lhs) | rhs.m_mask)};
	}

	template <typename F, typename M>
	inline constexpr basic_flags<F, M> operator ^ (typename basic_flags<F, M>::flag_type lhs, basic_flags<F, M> rhs) noexcept
	{
		return basic_flags<F, M>{BF_MASK_CAST_(M::to_mask(lhs) ^ rhs.m_mask)};
	}

	template <typename F, typename M>
	inline constexpr bool operator == (basic_flags<F, M> lhs, basic_flags<F, M> rhs) noexcept
	{
		return lhs.m_mask == rhs.m_mask;
	}

	template <typename F, typename M>
	inline constexpr bool operator == (basic_flags<F, M> lhs, typename basic_flags<F, M>::flag_type rhs) noexcept
	{
		return lhs.m_mask == M::to_mask(rhs);
	}

	template <typename F, typename M>
	inline constexpr bool operator == (typename basic_flags<F, M>::flag_type lhs, basic_flags<F, M> rhs) noexcept
	{
		return M::to_mask(lhs) == rhs.m_mask;
	}

	template <typename F, typename M>
	inline constexpr bool operator != (basic_flags<F, M> lhs, basic_flags<F, M> rhs) noexcept
	{
		return lhs.m_mask != rhs.m_mask;
	}

	template <typename F, typename M>
	inline constexpr bool operator != (basic_flags<F, M> lhs, typename basic_flags<F, M>::flag_type rhs) noexcept
	{
		return lhs.m_mask != M::to_mask(rhs);
	}

	template <typename F, typename M>
	inline constexpr bool operator != (typename basic_flags<F, M>::flag_type lhs, basic_flags<F, M> rhs) noexcept
	{
		return M::to_mask(lhs) != rhs.m_mask;
	}
}

#undef BF_MASK_CAST_


//
// mask_of
// ---------
// returns the mask of a basic_flags. we have made the conversion operator
// explicit, so users would have to first know (or get, as the case may be)
// the mask_type, and perform an explicit cast.
// 
// this is good because it makes people aware of when their flags are being
// interpreted as an integral, but it is kind of wordy. so 'mask_of' has
// been introduced to shorten this.
// 
// 'mask' was not chosen due to the high potential for name clashes.
//
namespace eastl
{
	template <typename F, typename T>
	inline constexpr typename basic_flags<F, T>::mask_type mask_of(basic_flags<F, T> flags)
	{
		return static_cast<typename basic_flags<F, T>::mask_type>(flags);
	}
}


//
// macros
// --------
// 
// see top-of-file for explanation
//
#define EASTL_DECLARE_BITFLAGS(flagstype, enumtype) \
	using flagstype = ::eastl::bitflags<enumtype>; \
	inline constexpr flagstype operator | (enumtype lhs, enumtype rhs) \
	{ \
		using msh = typename flagstype::marshaller_type; \
		return flagstype::from_mask(static_cast<msh::mask_type>(msh::to_mask(lhs) | msh::to_mask(rhs))); \
	}

#define EASTL_DECLARE_BITFLAGS_ENUM_CLASS(flagstype, enumtype) \
	enum class enumtype; \
	EASTL_DECLARE_BITFLAGS(flagstype, enumtype) \
	enum class enumtype

#define EASTL_DECLARE_BITFLAGS_ENUM_CLASS_SIZED(sizetype, flagstype, enumtype) \
	enum class enumtype : sizetype; \
	EASTL_DECLARE_BITFLAGS(flagstype, enumtype) \
	enum class enumtype : sizetype




#define EASTL_DECLARE_MASKFLAGS(flagstype, enumtype) \
	using flagstype = ::eastl::maskflags<enumtype>; \
	inline constexpr flagstype operator | (enumtype lhs, enumtype rhs) \
	{ \
		using msh = typename flagstype::marshaller_type; \
		return flagstype::from_mask(static_cast<msh::mask_type>(msh::to_mask(lhs) | msh::to_mask(rhs))); \
	} \
	inline constexpr flagstype operator & (enumtype lhs, enumtype rhs) \
	{ \
		using msh = typename flagstype::marshaller_type; \
		return flagstype::from_mask(static_cast<msh::mask_type>(msh::to_mask(lhs) & msh::to_mask(rhs))); \
	} \
	inline constexpr flagstype operator ^ (enumtype lhs, enumtype rhs) \
	{ \
		using msh = typename flagstype::marshaller_type; \
		return flagstype::from_mask(static_cast<msh::mask_type>(msh::to_mask(lhs) ^ msh::to_mask(rhs))); \
	}

#define EASTL_DECLARE_MASKFLAGS_ENUM_CLASS(flagstype, enumtype) \
	enum class enumtype; \
	EASTL_DECLARE_MASKFLAGS(flagstype, enumtype) \
	enum class enumtype

#define EASTL_DECLARE_MASKFLAGS_ENUM_CLASS_SIZED(sizetype, flagstype, enumtype) \
	enum class enumtype : sizetype; \
	EASTL_DECLARE_MASKFLAGS(flagstype, enumtype) \
	enum class enumtype : sizetype



