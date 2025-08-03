///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements the class template optional that represents optional objects.
//
// An optional object is an object that contains the storage for another object and
// manages the lifetime of this contained object, if any. The contained object may be
// initialized after the optional object has been initialized, and may be destroyed before
// the optional object has been destroyed.
//
// Any instance of optional<T> at any given time either contains a value or does not
// contain a value. When an instance of optional<T> contains a value, it means that an
// object of type T, referred to as the optional object's contained value, is allocated
// within the storage of the optional object. Implementations are not permitted to use
// additional storage, such as dynamic memory, to allocate its contained value.
//
// The contained value is allocated in the optional<T> storage suitably
// aligned for the type T. When an object of type optional<T> is contextually converted to
// bool, the conversion returns true if the object contains a value; otherwise the
// conversion returns false.
//
// T shall be an object type and satisfy the requirements of Destructible.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_OPTIONAL_H
#define EASTL_OPTIONAL_H

#include <EASTL/internal/config.h>
#include <EASTL/initializer_list.h>
#include <EASTL/memory.h> // eastl::addressof
#include <EASTL/internal/concepts.h>
#include <EASTL/internal/in_place_t.h> // eastl::in_place_t
#include <EASTL/internal/special_member_functions_variant_optional.h>

#if EASTL_EXCEPTIONS_ENABLED
	EA_DISABLE_ALL_VC_WARNINGS()
	#include <stdexcept> // std::logic_error.
	EA_RESTORE_ALL_VC_WARNINGS()
#endif

#if defined(EASTL_OPTIONAL_ENABLED) && EASTL_OPTIONAL_ENABLED

EA_DISABLE_VC_WARNING(4582 4583) // constructor/destructor is not implicitly called

namespace eastl
{
	///////////////////////////////////////////////////////////////////////////////
	/// nullopt_t
	///
	/// nullopt_t is class type used to indicate eastl::optional type with uninitialized state.
	///
	struct nullopt_tag_t {};

	struct nullopt_t
	{
		EA_CONSTEXPR nullopt_t(nullopt_tag_t) {}
	};

	EA_CONSTEXPR nullopt_t nullopt{nullopt_tag_t{}};


    ///////////////////////////////////////////////////////////////////////////////
	/// bad_optional_access
	///
	#if EASTL_EXCEPTIONS_ENABLED
		struct bad_optional_access : public std::logic_error
		{
			bad_optional_access() : std::logic_error("eastl::bad_optional_access exception") {}
			virtual ~bad_optional_access() EA_NOEXCEPT {}
		};
	#endif

	template <typename T>
	class optional; // Forward declaration for Internal::is_optional.
		
	namespace Internal
	{
		template <typename T>
		struct is_optional : false_type {};

		template <typename T>
		struct is_optional<optional<T>> : true_type {};

		template <typename T>
		constexpr bool is_optional_v = is_optional<T>::value;
		
		template <typename T>
		constexpr bool is_specialization_of_optional_v = false;

		template <typename T>
		constexpr bool is_specialization_of_optional_v<optional<T>> = true;

		template<typename T, typename W>
		constexpr bool converts_from_any_cvref_v =
			is_constructible_v<T, W&> || is_convertible_v<W&, T> ||
			is_constructible_v<T, W> || is_convertible_v<W, T> ||
			is_constructible_v<T, const W&> || is_convertible_v<const W&, T> ||
			is_constructible_v<T, const W> || is_convertible_v<const W, T>;

		///////////////////////////////////////////////////////////////////////////////
		/// optional_storage
		///
		template<typename T, bool IsTriviallyDestructible = eastl::is_trivially_destructible_v<T>>
		struct optional_storage
		{
			using value_type = T; // non-const, non-volatile

			inline ~optional_storage()
			{
				if (engaged)
					destruct_value();
			}

			inline EA_CPP20_CONSTEXPR void destruct_value() { (*reinterpret_cast<value_type*>(eastl::addressof(val))).~value_type(); }

			eastl::aligned_storage_t<sizeof(value_type), eastl::alignment_of_v<value_type>> val;
			bool engaged = false;
		};


		/// optional_storage<T, true>
		///
		/// Template specialization for trivial types to satisfy the requirement that optional<T> is trivially
		/// destructible when T is trivially destructible.
		///
		template<typename T>
		struct optional_storage<T, true>
		{
			using value_type = T; // non-const, non-volatile

			// Removed to make optional<T> trivially destructible when T is trivially destructible.
			~optional_storage() EA_NOEXCEPT = default;

			inline EA_CPP20_CONSTEXPR void destruct_value() {} // no implementation necessary since T is trivially destructible.

			eastl::aligned_storage_t<sizeof(value_type), eastl::alignment_of_v<value_type>> val;
			bool engaged = false;
		};

		template <typename T>
		class optional_base : private Internal::optional_storage<remove_cv_t<T>>
		{
			using base_type = Internal::optional_storage<remove_cv_t<T>>;

		public:
			using base_type::destruct_value;
			using base_type::val;
			using base_type::engaged;

			using value_type = T;

#if !EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
			optional_base() = default;
#endif

#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
			void ConstructFrom(const optional_base& other)
#else
			optional_base(const optional_base& other)
#endif
			{
				engaged = other.engaged;

				if (engaged)
				{
					construct_value(*other.get_value_address());
				}
			}

#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
			void ConstructFrom(optional_base&& other)
				EA_NOEXCEPT_IF(eastl::is_nothrow_move_constructible<value_type>::value)
#else
			optional_base(optional_base&& other)
#endif
			{
				engaged = other.engaged;

				if (engaged)
				{
					construct_value(eastl::move(*other.get_value_address()));
				}
			}

#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
			void AssignFrom(const optional_base& other)
#else
			optional_base& operator=(const optional_base& other)
#endif
			{
				if (engaged && !other.engaged)
				{
					destruct_value();
					engaged = false;
				}
				else if (engaged && other.engaged)
				{
					*get_value_address() = *other.get_value_address();
				}
				else if (!engaged && other.engaged)
				{
					construct_value(*other.get_value_address());
					engaged = true;
				}
				// else if (!engaged && !other.engaged)
				//	no op

#if !EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
				return *this;
#endif
			}

#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
			void AssignFrom(optional_base&& other)
				EA_NOEXCEPT_IF(eastl::is_nothrow_move_assignable<value_type>::value&&
					eastl::is_nothrow_move_constructible<value_type>::value)
#else
			optional_base& operator=(optional_base&& other)
#endif
			{
				if (engaged && !other.engaged)
				{
					destruct_value();
					engaged = false;
				}
				else if (engaged && other.engaged)
				{
					*get_value_address() = eastl::move(*other.get_value_address());
					// note, standard specifies: other.has_value() is unchanged.
				}
				else if (!engaged && other.engaged)
				{
					construct_value(eastl::move(*other.get_value_address()));
					engaged = true;
				}
				// else if (!engaged && !other.engaged)
				//	no op

#if !EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
				return *this;
#endif
			}

			template <class... Args>
			inline void construct_value(Args&&... args)
			{
				::new (eastl::addressof(val)) value_type(eastl::forward<Args>(args)...);
			}

			inline EA_CONSTEXPR T* get_value_address() EA_NOEXCEPT
			{
				return reinterpret_cast<T*>(eastl::addressof(val));
			}

			inline EA_CONSTEXPR const T* get_value_address() const EA_NOEXCEPT
			{
				return reinterpret_cast<const T*>(eastl::addressof(val));
			}

		};

	} // namespace Internal


	///////////////////////////////////////////////////////////////////////////////
	/// optional
	///
    template <typename T>
	class optional
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		: private internal::EnableVariantOptionalSpecialMemberFunctions<Internal::optional_base<T>, T>
	{
		using base_type = internal::EnableVariantOptionalSpecialMemberFunctions<Internal::optional_base<T>, T>;
#else
		: private Internal::optional_base<T>
	{
		using base_type = Internal::optional_base<T>;
#endif

		using base_type::destruct_value;
		using base_type::val;
		using base_type::engaged;
		using base_type::construct_value;
		using base_type::get_value_address;

	public:
		using value_type = T;
		using value_result_type = remove_volatile_t<value_type>;

		// https://eel.is/c++draft/optional#optional.general-3
		// https://eel.is/c++draft/utility.arg.requirements#:Cpp17Destructible
		static_assert(eastl::is_destructible_v<value_type>, "value type must be destructible");
		static_assert(eastl::is_object_v<value_type>, "value type must be destructible (non-object type is ill-formed)");
		static_assert(!eastl::is_array_v<value_type>, "value type must be destructible (array type is ill-formed)");
		static_assert(!eastl::is_same<remove_cv_t<value_type>, in_place_t>::value, "eastl::optional of a in_place_t type is ill-formed");
		static_assert(!eastl::is_same<remove_cv_t<value_type>, nullopt_t>::value, "eastl::optional of a nullopt_t type is ill-formed");

		inline EA_CONSTEXPR optional() EA_NOEXCEPT {}
		inline EA_CONSTEXPR optional(nullopt_t) EA_NOEXCEPT {}

#if !EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		inline EA_CONSTEXPR optional(const value_type& value)
		{
			construct_value(value);
			engaged = true;
		}

		inline EA_CONSTEXPR optional(value_type&& value) EA_NOEXCEPT_IF(eastl::is_nothrow_move_constructible_v<T>)
		{
			construct_value(eastl::move(value));
			engaged = true;
		}
#endif

		// implemented by EnableVariantOptionalSpecialMemberFunctions<...>
		optional(const optional&) = default;
		optional(optional&&) = default;

		// converting copy constructor, explicit version
		template<typename U,
			eastl::enable_if_t<is_constructible_v<T, const U&> && (is_same_v<eastl::remove_cv_t<T>, bool> || !Internal::converts_from_any_cvref_v<T, optional<U>>) &&
			!is_convertible_v<const U&, T>, bool> = true>
		EA_CPP20_CONSTEXPR explicit optional(const optional<U>& other)
		{
			if (other)
			{
				construct_value(*other);
				engaged = true;
			}
		}

		// converting copy constructor, non-explicit version
		template<typename U,
			eastl::enable_if_t<is_constructible_v<T, const U&> && (is_same_v<eastl::remove_cv_t<T>, bool> || !Internal::converts_from_any_cvref_v<T, optional<U>>) &&
			is_convertible_v<const U&, T>, bool> = true>
		EA_CPP20_CONSTEXPR optional(const optional<U>& other)
		{
			if (other)
			{
				construct_value(*other);
				engaged = true;
			}
		}

		// converting move constructor, explicit version
		template<typename U,
			eastl::enable_if_t<is_constructible_v<T, U> && (is_same_v<eastl::remove_cv_t<T>, bool> || !Internal::converts_from_any_cvref_v<T, optional<U>>) &&
			!is_convertible_v<U, T>, bool> = true>
		EA_CPP20_CONSTEXPR explicit optional(optional<U>&& other)
		{
			if (other)
			{
				construct_value(eastl::move(*other));
				engaged = true;
				// note, standard specifies: other.has_value() is unchanged.
			}
		}

		// converting move constructor, non-explicit version
		template<typename U,
			eastl::enable_if_t<is_constructible_v<T, U> && (is_same_v<eastl::remove_cv_t<T>, bool> || !Internal::converts_from_any_cvref_v<T, optional<U>>) &&
			is_convertible_v<U, T>, bool> = true>
		EA_CPP20_CONSTEXPR explicit optional(optional<U>&& other)
		{
			if (other)
			{
				construct_value(eastl::move(*other));
				engaged = true;
				// note, standard specifies: other.has_value() is unchanged.
			}
		}

#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		template <typename... Args,
			eastl::enable_if_t<eastl::is_constructible_v<T, Args...>, bool> = true>
#else
		template <typename... Args>
#endif
		inline EA_CONSTEXPR explicit optional(in_place_t, Args&&... args)
		{
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
			construct_value(eastl::forward<Args>(args)...);
#else
			// value_type{ ... } is direct-list-initialization, which permits aggregate initialization.
			// that's not allowed by the standard; it's meant to be direct-non-list-initialization (which is what construct_value() implements).
			// this block (and omitting the is_constructible_v requirement) is to deprecate incorrect usage.
			::new (eastl::addressof(val)) value_type{ eastl::forward<Args>(args)... };
#endif
			engaged = true;
		}

#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		template <typename U, typename... Args,
			eastl::enable_if_t<eastl::is_constructible_v<T, std::initializer_list<U>&, Args...>, bool> = true>
#else
		template <typename U, typename... Args>
#endif
		inline EA_CONSTEXPR explicit optional(in_place_t, std::initializer_list<U> ilist, Args&&... args)
		{
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
			construct_value(ilist, eastl::forward<Args>(args)...);
#else
			// value_type{ ... } is direct-list-initialization, which permits aggregate initialization.
			// that's not allowed by the standard; it's meant to be direct-non-list-initialization (which is what construct_value() implements).
			// this block (and omitting the is_constructible_v requirement) is to deprecate incorrect usage.
			::new (eastl::addressof(val)) value_type{ ilist, eastl::forward<Args>(args)... };
#endif
			engaged = true;
		}

		// constructs an optional that contains a value, explicit version
		template <typename U = T,
			eastl::enable_if_t<eastl::is_constructible_v<T, U&&> &&
			!eastl::is_same_v<eastl::remove_cvref_t<U>, eastl::in_place_t> &&
			!eastl::is_same_v<eastl::remove_cvref_t<U>, optional<T>> &&
			(!eastl::is_same_v<eastl::remove_cv_t<T>, bool> || !Internal::is_specialization_of_optional_v<eastl::remove_cvref_t<U>>) &&
			!eastl::is_convertible_v<U&&, T>,
			bool> = true>
		inline explicit EA_CONSTEXPR optional(U&& value)
		{
			construct_value(eastl::forward<U>(value));
			engaged = true;
		}

		// constructs an optional that contains a value, non-explicit version
		template <typename U = T,
			eastl::enable_if_t<eastl::is_constructible_v<T, U&&> &&
			!eastl::is_same_v<eastl::remove_cvref_t<U>, eastl::in_place_t> &&
			!eastl::is_same_v<eastl::remove_cvref_t<U>, optional<T>> &&
			(!eastl::is_same_v<eastl::remove_cv_t<T>, bool> || !Internal::is_specialization_of_optional_v<eastl::remove_cvref_t<U>>) &&
			eastl::is_convertible_v<U&&, T>,
			bool> = true>
		inline EA_CONSTEXPR optional(U&& value)
		{
			construct_value(eastl::forward<U>(value));
			engaged = true;
		}

		inline EA_CPP20_CONSTEXPR optional& operator=(nullopt_t) EA_NOEXCEPT
		{
			reset();
			return *this;
		}

		// implemented by EnableVariantOptionalSpecialMemberFunctions<...>
		optional& operator=(const optional&) = default;
		optional& operator=(optional&&) = default;

		// converting copy assignment
		template<typename U,
			eastl::enable_if_t<is_constructible_v<T, const U&> && is_assignable_v<T&, const U&> && !Internal::converts_from_any_cvref_v<T, optional<U>> &&
			!is_assignable_v<T&, optional<U>&> && !is_assignable_v<T&, optional<U>&&> && !is_assignable_v<T&, const optional<U>&> && !is_assignable_v<T&, const optional<U>&&>, bool> = true>
		EA_CPP20_CONSTEXPR optional& operator=(const optional<U>& other)
		{
			if (engaged && other.engaged)
			{
				*get_value_address() = *other;
			}
			else if (engaged && !other.engaged)
			{
				destruct_value();
				engaged = false;
			}
			else if (!engaged && other.engaged)
			{
				construct_value(*other);
				engaged = true;
			}
			// else if (!engaged && !other.engaged)
			//	no op

			return *this;
		}

		// converting move assignment
		template<typename U,
			eastl::enable_if_t<is_constructible_v<T, U> && is_assignable_v<T&, U> && !Internal::converts_from_any_cvref_v<T, optional<U>> &&
			!is_assignable_v<T&, optional<U>&> && !is_assignable_v<T&, optional<U>&&> && !is_assignable_v<T&, const optional<U>&> && !is_assignable_v<T&, const optional<U>&&>, bool> = true>
		EA_CPP20_CONSTEXPR optional& operator=(optional<U>&& other)
		{
			if (engaged && other.engaged)
			{
				*get_value_address() = eastl::move(*other);
				// note, standard specifies: other.has_value() is unchanged.
			}
			else if (engaged && !other.engaged)
			{
				destruct_value();
				engaged = false;
			}
			else if (!engaged && other.engaged)
			{
				construct_value(eastl::move(*other));
				engaged = true;
			}
			//else if (!engaged && !other.engaged)
			//	no op

			return *this;
		}

		template <class U = T, typename eastl::enable_if_t<
			!eastl::is_same_v<eastl::remove_cvref_t<U>, optional<T>>&&
			eastl::is_constructible_v<T, U>&&
			eastl::is_assignable_v<T&, U> && (!eastl::is_scalar_v<T> || !eastl::is_same_v<eastl::decay_t<U>, T>), bool> = true>
		inline EA_CPP20_CONSTEXPR optional& operator=(U&& u)
		{
			if (engaged)
			{
				*get_value_address() = eastl::forward<U>(u);
			}
			else
			{
				construct_value(eastl::forward<U>(u));
				engaged = true;
			}

			return *this;
		}

	    EA_CONSTEXPR inline explicit operator bool() const EA_NOEXCEPT { return engaged; }

		EA_CONSTEXPR inline bool has_value() const EA_NOEXCEPT { return engaged; }

	    template <class U>
	    inline EA_CONSTEXPR value_result_type value_or(U&& default_value) const&
			{ return engaged ? value() : static_cast<value_type>(eastl::forward<U>(default_value)); }

	    template <class U>
	    inline EA_CONSTEXPR value_result_type value_or(U&& default_value) &&
			{ return engaged ? eastl::move(value()) : static_cast<value_type>(eastl::forward<U>(default_value)); }

		inline EA_CONSTEXPR T& value()&
		{
			if (!engaged)
				EASTL_THROW_OR_ASSERT(bad_optional_access, "no value to retrieve");
			return *get_value_address();
		}

		inline EA_CONSTEXPR const T& value() const&
		{
			if (!engaged)
				EASTL_THROW_OR_ASSERT(bad_optional_access, "no value to retrieve");
			return *get_value_address();
		}

		inline EA_CONSTEXPR T&& value()&&
		{
			if (!engaged)
				EASTL_THROW_OR_ASSERT(bad_optional_access, "no value to retrieve");
			return eastl::move(*get_value_address());
		}

		inline EA_CONSTEXPR const T&& value() const&&
		{
			if (!engaged)
				EASTL_THROW_OR_ASSERT(bad_optional_access, "no value to retrieve");
			return eastl::move(*get_value_address());
		}

		// These all assume has_value() is true. Otherwise, calling them is UB (as per the
		// standard).  When asserts are enabled, we've decided to assert the precondition
		// similar to what would be done in a hardened library implementation.
		inline EA_CONSTEXPR T* operator->() EA_NOEXCEPT
		{
			EASTL_ASSERT_MSG(has_value(), "Pre-condition failed! Accessing an optional without a value.");
			return get_value_address();
		}
		inline EA_CONSTEXPR const T* operator->() const EA_NOEXCEPT
		{
			EASTL_ASSERT_MSG(has_value(), "Pre-condition failed! Accessing an optional without a value.");
			return get_value_address();
		}
		inline EA_CONSTEXPR T& operator*() & EA_NOEXCEPT
		{
			EASTL_ASSERT_MSG(has_value(), "Pre-condition failed! Accessing an optional without a value.");
			return *get_value_address();
		}
		inline EA_CONSTEXPR T&& operator*() && EA_NOEXCEPT
		{
			EASTL_ASSERT_MSG(has_value(), "Pre-condition failed! Accessing an optional without a value.");
			return eastl::move(*get_value_address());
		}
		inline EA_CONSTEXPR const T& operator*() const& EA_NOEXCEPT
		{
			EASTL_ASSERT_MSG(has_value(), "Pre-condition failed! Accessing an optional without a value.");
			return *get_value_address();
		}
		inline EA_CONSTEXPR const T&& operator*() const&& EA_NOEXCEPT
		{
			EASTL_ASSERT_MSG(has_value(), "Pre-condition failed! Accessing an optional without a value.");
			return eastl::move(*get_value_address());
		}

		// Monadic operations
		template <typename F>
		constexpr inline auto and_then(F&& f) &
		{
			using U = eastl::invoke_result_t<F, decltype(value())>;
			static_assert(Internal::is_optional_v<eastl::remove_cvref_t<U>>,
						  "The supplied callable isn't returning an optional.");

			if (has_value())
			{
				return eastl::invoke(eastl::forward<F>(f), value());
			}

			return eastl::remove_cvref_t<U>();
		}

		template <typename F>
		constexpr inline auto and_then(F&& f) const&
		{
			using U = eastl::invoke_result_t<F, decltype(value())>;
			static_assert(Internal::is_optional_v<eastl::remove_cvref_t<U>>,
						  "The supplied callable isn't returning an optional.");

			if (has_value())
			{
				return eastl::invoke(eastl::forward<F>(f), value());
			}

			return eastl::remove_cvref_t<U>();
		}

		template <typename F>
		constexpr inline auto and_then(F&& f) &&
		{
			using U = eastl::invoke_result_t<F, decltype(eastl::move(value()))>;
			static_assert(Internal::is_optional_v<eastl::remove_cvref_t<U>>,
						  "The supplied callable isn't returning an optional.");

			if (has_value())
			{
				return eastl::invoke(eastl::forward<F>(f), eastl::move(value()));
			}

			return eastl::remove_cvref_t<U>();
		}

		template <typename F>
		constexpr inline auto and_then(F&& f) const&&
		{
			using U = eastl::invoke_result_t<F, decltype(eastl::move(value()))>;
			static_assert(Internal::is_optional_v<eastl::remove_cvref_t<U>>,
						  "The supplied callable isn't returning an optional.");

			if (has_value())
			{
				return eastl::invoke(eastl::forward<F>(f), eastl::move(value()));
			}

			return eastl::remove_cvref_t<U>();
		}

		template <typename F>
		constexpr inline auto transform(F&& f) &
		{
			using U = eastl::remove_cvref_t<eastl::invoke_result_t<F, decltype(value())>>;

			static_assert(!eastl::is_same_v<U, in_place_t>, "The supplied callable cannot return in_place_t.");
			static_assert(!eastl::is_same_v<U, nullopt_t>, "The supplied callable cannot return nullopt_t.");
			static_assert(eastl::is_object_v<U>, "The supplied callable must return an object type.");
			static_assert(!eastl::is_array_v<U>, "The supplied callable cannot return an array type.");

			if (has_value())
			{
				return eastl::optional<U>(eastl::invoke(eastl::forward<F>(f), value()));
			}

			return eastl::optional<U>();
		}

		template <typename F>
		constexpr inline auto transform(F&& f) const&
		{
			using U = eastl::remove_cvref_t<eastl::invoke_result_t<F, decltype(value())>>;

			static_assert(!eastl::is_same_v<U, in_place_t>, "The supplied callable cannot return in_place_t.");
			static_assert(!eastl::is_same_v<U, nullopt_t>, "The supplied callable cannot return nullopt_t.");
			static_assert(eastl::is_object_v<U>, "The supplied callable must return an object type.");
			static_assert(!eastl::is_array_v<U>, "The supplied callable cannot return an array type.");

			if (has_value())
			{
				return eastl::optional<U>(eastl::invoke(eastl::forward<F>(f), value()));
			}

			return eastl::optional<U>();
		}

		template <typename F>
		constexpr inline auto transform(F&& f) &&
		{
			using U = eastl::remove_cvref_t<eastl::invoke_result_t<F, decltype(eastl::move(value()))>>;

			static_assert(!eastl::is_same_v<U, in_place_t>, "The supplied callable cannot return in_place_t.");
			static_assert(!eastl::is_same_v<U, nullopt_t>, "The supplied callable cannot return nullopt_t.");
			static_assert(eastl::is_object_v<U>, "The supplied callable must return an object type.");
			static_assert(!eastl::is_array_v<U>, "The supplied callable cannot return an array type.");

			if (has_value())
			{
				return eastl::optional<U>(eastl::invoke(eastl::forward<F>(f), eastl::move(value())));
			}

			return eastl::optional<U>();
		}

		template <typename F>
		constexpr inline auto transform(F&& f) const&&
		{
			using U = eastl::remove_cvref_t<eastl::invoke_result_t<F, decltype(eastl::move(value()))>>;

			static_assert(!eastl::is_same_v<U, in_place_t>, "The supplied callable cannot return in_place_t.");
			static_assert(!eastl::is_same_v<U, nullopt_t>, "The supplied callable cannot return nullopt_t.");
			static_assert(eastl::is_object_v<U>, "The supplied callable must return an object type.");
			static_assert(!eastl::is_array_v<U>, "The supplied callable cannot return an array type.");

			if (has_value())
			{
				return eastl::optional<U>(eastl::invoke(eastl::forward<F>(f), eastl::move(value())));
			}

			return eastl::optional<U>();
		}

		template <typename F, enable_if_t<is_invocable_v<F> && internal::concepts::copy_constructible<T>, int> = 0>
		constexpr inline optional or_else(F&& f) const&
		{
			static_assert(eastl::is_same_v<eastl::remove_cvref_t<eastl::invoke_result_t<F>>, optional>,
						  "The supplied callable must return an optional of the same type.");

			if (has_value())
			{
				return *this;
			}

			return eastl::forward<F>(f)();
		}

		template <typename F, enable_if_t<is_invocable_v<F> && internal::concepts::move_constructible<T>, int> = 0>
		constexpr inline optional or_else(F&& f) &&
		{
			static_assert(eastl::is_same_v<eastl::remove_cvref_t<eastl::invoke_result_t<F>>, optional>,
						  "The supplied callable must return an optional of the same type.");

			if (has_value())
			{
				return eastl::move(*this);
			}

			return eastl::forward<F>(f)();
		}

		template <class... Args>
		EA_CPP20_CONSTEXPR T& emplace(Args&&... args)
		{
			if (engaged)
			{
				destruct_value();
				engaged = false;
			}
			construct_value(eastl::forward<Args>(args)...);
			engaged = true;
			return *get_value_address();
		}

		template <class U, class... Args>
		EA_CPP20_CONSTEXPR T& emplace(std::initializer_list<U> ilist, Args&&... args)
		{
			if (engaged)
			{
				destruct_value();
				engaged = false;
			}
			construct_value(ilist, eastl::forward<Args>(args)...);
			engaged = true;
			return *get_value_address();
		}

	    inline EA_CPP20_CONSTEXPR void swap(optional& other)
	        EA_NOEXCEPT_IF(eastl::is_nothrow_move_constructible<T>::value && eastl::is_nothrow_swappable<T>::value)
	    {
			if (!engaged && other.engaged)
			{
				construct_value(eastl::move(*other.get_value_address()));
				other.destruct_value();
				engaged = true;
				other.engaged = false;
			}
			else if (engaged && !other.engaged)
			{
				other.construct_value(eastl::move(*get_value_address()));
				destruct_value();
				engaged = false;
				other.engaged = true;
			}
			else if (engaged && other.engaged)
			{
				using eastl::swap;
				swap(**this, *other);
			}
			//else if (!engaged && !other.engaged)
			//	no op
	    }

		inline EA_CPP20_CONSTEXPR void reset() EA_NOEXCEPT
		{
			if (engaged)
			{
				destruct_value();
				engaged = false;
			}
		}

    }; // class optional


    ///////////////////////////////////////////////////////////////////////////////
	/// global swap
	///
    template <class T>
    void swap(optional<T>& lhs, optional<T>& rhs) EA_NOEXCEPT_IF(EA_NOEXCEPT(lhs.swap(rhs)))
		{ lhs.swap(rhs); }


    ///////////////////////////////////////////////////////////////////////////////
	/// global comparisions
	///
	/// http://en.cppreference.com/w/cpp/utility/optional/operator_cmp
	///

    ///////////////////////////////////////////////////////////////////////////////
	// Compare two optional objects
	//
	template <class T>
	inline EA_CONSTEXPR bool operator==(const optional<T>& lhs, const optional<T>& rhs)
	{
		return (bool(lhs) != bool(rhs)) ? false : (bool(lhs) == false) ? true : *lhs == *rhs;
	}

	template <class T>
	inline EA_CONSTEXPR bool operator<(const optional<T>& lhs, const optional<T>& rhs)
	{
	    return (!bool(rhs)) ? false : (!bool(lhs)) ? true : *lhs < *rhs;
    }

    template <class T>
	inline EA_CONSTEXPR bool operator!=(const optional<T>& lhs, const optional<T>& rhs)
		{ return !(lhs == rhs); }

	template <class T>
	inline EA_CONSTEXPR bool operator<=(const optional<T>& lhs, const optional<T>& rhs)
		{ return !(rhs < lhs); }

	template <class T>
	inline EA_CONSTEXPR bool operator>(const optional<T>& lhs, const optional<T>& rhs)
		{ return rhs < lhs; }

	template <class T>
	inline EA_CONSTEXPR bool operator>=(const optional<T>& lhs, const optional<T>& rhs)
		{ return !(lhs < rhs);	}

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	template <class T, class U=T> requires std::three_way_comparable_with<T, U>
	inline EA_CONSTEXPR std::compare_three_way_result_t<T, U> operator<=>(const optional<T>& lhs, const optional<U>& rhs)
		{
		    if (lhs && rhs)
		    {
		        return *lhs <=> *rhs;
		    }
		    return lhs.has_value() <=> rhs.has_value();
		}
#endif

    ///////////////////////////////////////////////////////////////////////////////
	// Compare an optional object with a nullopt
	//
    template <class T>
    inline EA_CONSTEXPR bool operator==(const optional<T>& opt, eastl::nullopt_t) EA_NOEXCEPT
		{ return !opt; }
#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
    template <class T>
    inline EA_CONSTEXPR std::strong_ordering operator<=>(const optional<T>& opt, eastl::nullopt_t) EA_NOEXCEPT
		{ return opt.has_value() <=> false; }
#else
    template <class T>
    inline EA_CONSTEXPR bool operator==(eastl::nullopt_t, const optional<T>& opt) EA_NOEXCEPT
		{ return !opt; }

    template <class T>
    inline EA_CONSTEXPR bool operator!=(const optional<T>& opt, eastl::nullopt_t) EA_NOEXCEPT
		{ return bool(opt); }

    template <class T>
    inline EA_CONSTEXPR bool operator!=(eastl::nullopt_t, const optional<T>& opt) EA_NOEXCEPT
		{ return bool(opt); }

    template <class T>
    inline EA_CONSTEXPR bool operator<(const optional<T>&, eastl::nullopt_t) EA_NOEXCEPT
		{ return false; }

    template <class T>
    inline EA_CONSTEXPR bool operator<(eastl::nullopt_t, const optional<T>& opt) EA_NOEXCEPT
		{ return bool(opt); }

    template <class T>
    inline EA_CONSTEXPR bool operator<=(const optional<T>& opt, eastl::nullopt_t) EA_NOEXCEPT
		{ return !opt; }

    template <class T>
    inline EA_CONSTEXPR bool operator<=(eastl::nullopt_t, const optional<T>&) EA_NOEXCEPT
		{ return true; }

    template <class T>
    inline EA_CONSTEXPR bool operator>(const optional<T>& opt, eastl::nullopt_t) EA_NOEXCEPT
		{ return bool(opt); }

    template <class T>
    inline EA_CONSTEXPR bool operator>(eastl::nullopt_t, const optional<T>&) EA_NOEXCEPT
		{ return false; }

    template <class T>
    inline EA_CONSTEXPR bool operator>=(const optional<T>&, eastl::nullopt_t) EA_NOEXCEPT
		{ return true; }

    template <class T>
    inline EA_CONSTEXPR bool operator>=(eastl::nullopt_t, const optional<T>& opt) EA_NOEXCEPT
		{ return !opt; }
#endif

    ///////////////////////////////////////////////////////////////////////////////
    // Compare an optional object with a T
	//
    template <class T>
    inline EA_CONSTEXPR bool operator==(const optional<T>& opt, const T& value)
		{ return bool(opt) ? *opt == value : false; }

    template <class T>
    inline EA_CONSTEXPR bool operator==(const T& value, const optional<T>& opt)
		{ return bool(opt) ? value == *opt : false; }

    template <class T>
    inline EA_CONSTEXPR bool operator!=(const optional<T>& opt, const T& value)
		{ return bool(opt) ? !(*opt == value) : true; }

    template <class T>
    inline EA_CONSTEXPR bool operator!=(const T& value, const optional<T>& opt)
		{ return bool(opt) ? !(value == *opt) : true; }

    template <class T>
    inline EA_CONSTEXPR bool operator<(const optional<T>& opt, const T& value)
		{ return bool(opt) ? *opt < value  : true; }

    template <class T>
    inline EA_CONSTEXPR bool operator<(const T& value, const optional<T>& opt)
		{ return bool(opt) ? value < *opt  : false;  }

    template <class T>
    inline EA_CONSTEXPR bool operator<=(const optional<T>& opt, const T& value)
		{ return !(opt > value); }

    template <class T>
    inline EA_CONSTEXPR bool operator<=(const T& value, const optional<T>& opt)
		{ return !(value > opt); }

    template <class T>
    inline EA_CONSTEXPR bool operator>(const optional<T>& opt, const T& value)
		{ return bool(opt) ? value < *opt  : false; }

    template <class T>
    inline EA_CONSTEXPR bool operator>(const T& value, const optional<T>& opt)
		{ return bool(opt) ? *opt < value  : true; }

    template <class T>
    inline EA_CONSTEXPR bool operator>=(const optional<T>& opt, const T& value)
		{ return !(opt < value); }

    template <class T>
    inline EA_CONSTEXPR bool operator>=(const T& value, const optional<T>& opt)
		{ return !(value < opt);  }

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
    template <class T, class U=T> requires std::three_way_comparable_with<T, U>
    inline EA_CONSTEXPR std::compare_three_way_result_t<T, U> operator<=>(const optional<T>& opt, const U& value)
		{ return (opt.has_value()) ? *opt <=> value : std::strong_ordering::less; }
#endif

    ///////////////////////////////////////////////////////////////////////////////
	/// hash
	///
	template <typename T>
	struct hash<eastl::optional<T>>
	{
		size_t operator()(const eastl::optional<T>& opt) const EA_NOEXCEPT
	    {
		    if (opt)
			    return eastl::hash<T>()(*opt);
		    else
			    return 0;  // no value to generate a hash from
	    }
    };


    ///////////////////////////////////////////////////////////////////////////////
	/// make_optional
	///
	template <class T>
	inline EA_CONSTEXPR optional<decay_t<T>> make_optional(T&& value)
	{
		return optional<decay_t<T>>(eastl::forward<T>(value));
	}

#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
	template <class T, class... Args, enable_if_t<is_constructible_v<T, Args...>, bool> = true>
#else
	template <class T, class... Args>
#endif
	inline EA_CONSTEXPR optional<T> make_optional(Args&&... args)
	{
		return optional<T>(eastl::in_place, eastl::forward<Args>(args)...);
	}

#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
	template <class T, class U, class... Args, enable_if_t<is_constructible_v<T, std::initializer_list<U>&, Args...>, bool> = true>
#else
	template <class T, class U, class... Args>
#endif
	inline EA_CONSTEXPR optional<T> make_optional(std::initializer_list<U> il, Args&&... args)
	{
		return eastl::optional<T>(eastl::in_place, il, eastl::forward<Args>(args)...);
	}

#ifdef __cpp_deduction_guides
	template<typename T>
	optional(T) -> optional<T>;
#endif

}  // namespace eastl

EA_RESTORE_VC_WARNING()

#endif  // EASTL_OPTIONAL_ENABLED
#endif  // EASTL_OPTIONAL_H
