///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
#pragma once

// We use a few c++17 features in the implementation of eastl::expceted, so we only provide
// it from c++17 onwards.
#if EA_COMPILER_CPP17_ENABLED

#include <EABase/eabase.h>

EA_DISABLE_VC_WARNING(4623) // warning C4623: default constructor was implicitly defined as deleted
EA_DISABLE_VC_WARNING(4625) // warning C4625: copy constructor was implicitly defined as deleted
EA_DISABLE_VC_WARNING(4510) // warning C4510: default constructor could not be generated

#include <EASTL/internal/special_member_functions_expected.h>
#include <EASTL/internal/in_place_t.h>
#include <EASTL/memory.h>
#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <initializer_list> // for std::initializer_list

#if EASTL_EXCEPTIONS_ENABLED
#include <exception> // for std::exception in bad_exception_access.
#endif

namespace eastl
{
	template <class T, class E>
	class expected;

	template <class E>
	class unexpected;

	// Some helper type traits:
	namespace internal
	{
		// TODO: move this somewhere else? It doesn't handle
		// templates with non-type template parameters so it isn't
		// really generic...
		template <class T, template <class...> class Template>
		struct is_specialization : eastl::false_type
		{
		};

		template <template <class...> class Template, class... Args>
		struct is_specialization<Template<Args...>, Template> : eastl::true_type
		{
		};

		// Used in the SFINAE expression for a constructor in the expected class.
		template <class T, class U>
		static constexpr bool converts_from_any_cvref_v =
		    is_constructible_v<T, U&> || is_convertible_v<U&, T> || is_constructible_v<T, U> ||
		    is_convertible_v<U, T> || is_constructible_v<T, const U> || is_convertible_v<const U, T> ||
		    is_constructible_v<T, const U&> || is_convertible_v<const U&, T>;

		template <class T, class E, class U>
		static constexpr bool generic_constructor_constraint_v =
		    !is_same_v<remove_cvref_t<U>, in_place_t> && !is_same_v<expected<T, E>, remove_cvref_t<U>> &&
		    !internal::is_specialization<remove_cvref_t<U>, unexpected>::value && is_constructible_v<T, U> &&
		    (!is_same_v<remove_cv<T>, bool> || !internal::is_specialization<remove_cvref_t<U>, expected>::value);

		template <class T, class E, class U, class G, class UF, class GF>
		static constexpr bool expected_to_expected_ctor_constraint_v =
		    is_constructible_v<T, UF> && is_constructible_v<E, GF> &&
		    !is_constructible_v<unexpected<E>, expected<U, G>&> && !is_constructible_v<unexpected<E>, expected<U, G>> &&
		    !is_constructible_v<unexpected<E>, const expected<U, G>&> &&
		    !is_constructible_v<unexpected<E>, const expected<U, G>> &&
		    (!is_same_v<remove_cv<T>, bool> || !internal::converts_from_any_cvref_v<T, expected<U, G>>);

	} // namespace internal

	template <class E>
	class unexpected
	{
	public:
		// constructors
		constexpr unexpected(const unexpected&) = default;
		constexpr unexpected(unexpected&&) = default;

		template <class Err,
		          typename = enable_if_t<!is_same_v<remove_cvref_t<Err>, unexpected<E>> &&
		                                 !is_same_v<remove_cvref_t<Err>, in_place_t> && is_constructible_v<E, Err>>>
		constexpr explicit unexpected(Err&& e) : mError(eastl::forward<Err>(e)){};

		template <class... Args, enable_if_t<is_constructible_v<E, Args...>, int> = 0>
		constexpr explicit unexpected(in_place_t, Args&&... args) : mError(eastl::forward<Args>(args)...){};

		template <class U,
		          class... Args,
		          enable_if_t<is_constructible_v<E, std::initializer_list<U>&, Args...>, int> = 0>
		constexpr explicit unexpected(in_place_t, std::initializer_list<U> il, Args&&... args)
		    : mError(il, eastl::forward<Args>(args)...){};

		constexpr unexpected& operator=(const unexpected&) = default;
		constexpr unexpected& operator=(unexpected&&) = default;

		constexpr const E& error() const& noexcept { return mError; };
		constexpr E& error() & noexcept { return mError; };
		constexpr const E&& error() const&& noexcept { return eastl::move(mError); };
		constexpr E&& error() && noexcept { return eastl::move(mError); };

		constexpr void swap(unexpected& other) noexcept(is_nothrow_swappable_v<E>)
		{
			static_assert(is_swappable_v<E>, "unexpected<E> swap requires E to be swappable");
			using eastl::swap;
			swap(mError, other.mError);
		};

		friend constexpr void swap(unexpected& x, unexpected& y) noexcept(noexcept(x.swap(y)))
		{
			static_assert(is_swappable_v<E>, "unexpected<E> swap requires E to be swappable");
			x.swap(y);
		}

		// equality operator
		template <class E2>
		friend constexpr bool operator==(const unexpected& x, const unexpected<E2>& y)
		{
			return x.mError == y.mError;
		}

	private:
		E mError;

		// The standard specifies these are ill-formed.
		static_assert(is_object_v<E> && !is_array_v<E> && !is_const_v<E> && !is_volatile_v<E> &&
		                  !internal::is_specialization<E, unexpected>::value,
		              "This type is not supported by a conforming implementation of unexpected.");
	};

	template <class E>
	unexpected(E) -> unexpected<E>;


#if EASTL_EXCEPTIONS_ENABLED
	template <class E>
	class bad_expected_access;

	template <>
	class bad_expected_access<void> : public std::exception
	{
	public:
		const char* what() const noexcept override { return "Bad expected access."; };

	protected:
		bad_expected_access() noexcept = default;
		bad_expected_access(const bad_expected_access&) = default;
		bad_expected_access(bad_expected_access&&) = default;
		bad_expected_access& operator=(const bad_expected_access&) = default;
		bad_expected_access& operator=(bad_expected_access&&) = default;
		~bad_expected_access() = default;
	};

	template <class E>
	class bad_expected_access : public bad_expected_access<void>
	{
	public:
		explicit bad_expected_access(E e) : mError(eastl::move(e)){};

		// just use the base class' what(), no need to override this.
		// const char* what() const noexcept override;

		E& error() & noexcept { return mError; };
		const E& error() const& noexcept { return mError; };
		E&& error() && noexcept { return eastl::move(mError); };
		const E&& error() const&& noexcept { return eastl::move(mError); };

	private:
		E mError;
	};
#endif // EASTL_EXCEPTIONS_ENABLED

	// in-place construction of unexpected values
	struct unexpect_t
	{
		explicit unexpect_t() = default;
	};
	inline constexpr unexpect_t unexpect{};

	namespace internal
	{
		///////
		// ExpectedDestructLayer handles the triviality of the destructor
		//
		// The general case when both T and E are trivially destructible.
		template <class T, class E, bool = is_trivially_destructible_v<T> && is_trivially_destructible_v<E>>
		struct ExpectedDestructLayer
		{
			// Note: we deliberately don't initialize anything here, member initailization for
			// the default conxtructoris done in the `expected` class.
			constexpr ExpectedDestructLayer(){};

			union
			{
				T mValue;
				E mError;
			};
			bool mHasValue;
		};
		//
		// The case where one of T or E is not trivially destructible.
		template <class T, class E>
		struct ExpectedDestructLayer<T, E, false>
		{
			~ExpectedDestructLayer()
			{
				if (mHasValue)
				{
					eastl::destroy_at(&mValue);
				}
				else
				{
					eastl::destroy_at(&mError);
				}
			}

			// Note: we deliberately don't initialize anything here, member initailization for
			// the default conxtructoris done in the `expected` class.
			constexpr ExpectedDestructLayer(){};

			union
			{
				T mValue;
				E mError;
			};
			bool mHasValue;
		};


		///////
		// ExpectedConstructLayer handles the implemenation of the copy/move constructor/assignment
		//
		template <class T, class E>
		struct ExpectedConstructLayer : ExpectedDestructLayer<T, E>
		{
			using ExpectedDestructLayer<T, E>::ExpectedDestructLayer;

			void ConstructFrom(const ExpectedConstructLayer& other)
			{
				this->mHasValue = other.mHasValue;
				if (this->mHasValue)
				{
					eastl::construct_at(eastl::addressof(this->mValue), other.mValue);
				}
				else
				{
					eastl::construct_at(eastl::addressof(this->mError), other.mError);
				}
			}

			void ConstructFrom(ExpectedConstructLayer&& other)
			{
				this->mHasValue = other.mHasValue;
				if (this->mHasValue)
				{
					eastl::construct_at(eastl::addressof(this->mValue), eastl::move(other.mValue));
				}
				else
				{
					eastl::construct_at(eastl::addressof(this->mError), eastl::move(other.mError));
				}
			}

			void AssignFrom(const ExpectedConstructLayer& other)
			{
				if (this->mHasValue && other.mHasValue)
				{
					this->mValue = other.mValue;
				}
				else if (this->mHasValue)
				{
					ReInit(this->mError, this->mValue, other.mError);
					this->mHasValue = false;
				}
				else if (other.mHasValue)
				{
					ReInit(this->mValue, this->mError, other.mValue);
					this->mHasValue = true;
				}
				else
				{
					this->mError = other.mError;
				}
			}

			void AssignFrom(ExpectedConstructLayer&& other)
			{
				if (this->mHasValue && other.mHasValue)
				{
					this->mValue = eastl::move(other.mValue);
				}
				else if (this->mHasValue)
				{
					ReInit(this->mError, this->mValue, eastl::move(other.mError));
					this->mHasValue = false;
				}
				else if (other.mHasValue)
				{
					ReInit(this->mValue, this->mError, eastl::move(other.mValue));
					this->mHasValue = true;
				}
				else
				{
					this->mError = eastl::move(other.mError);
				}
			}


			template <class NewVal, class OldVal, class... Args>
			void ReInit(NewVal& newval, OldVal& oldval, Args&&... args)
			{
#if EASTL_EXCEPTIONS_ENABLED
				if constexpr (is_nothrow_constructible_v<NewVal, Args...>)
				{
					eastl::destroy_at(&oldval);
					eastl::construct_at(eastl::addressof(newval), eastl::forward<Args>(args)...);
				}
				else if constexpr (is_nothrow_move_constructible_v<NewVal>)
				{
					NewVal tmp(eastl::forward<Args>(args)...);
					eastl::destroy_at(&oldval);
					eastl::construct_at(eastl::addressof(newval), eastl::move(tmp));
				}
				else
				{
					OldVal tmp(eastl::move(oldval));
					eastl::destroy_at(&oldval);
					try
					{
						eastl::construct_at(eastl::addressof(newval), eastl::forward<Args>(args)...);
					}
					catch (...)
					{
						eastl::construct_at(eastl::addressof(oldval), eastl::move(tmp));
						throw;
					}
				}
#else
				eastl::destroy_at(&oldval);
				eastl::construct_at(&newval, eastl::forward<Args>(args)...);
#endif
			}
		};

	} // namespace internal

	// TODO: we've marked member functions and constructors as constexpr when the standard
	// dictates it, but in reality a lot of these functions do now work at constant evaluation
	// time becuase they use facilities like `eastl::addressof` and `eastl::construct_at` which
	// are currently not constexpr.
	template <class T, class E>
	class expected : internal::EnableExpectedSpecialMemberFunctions<internal::ExpectedConstructLayer<T, E>, T, E>
	{
	private:
		using LayeredBase = internal::EnableExpectedSpecialMemberFunctions<internal::ExpectedConstructLayer<T, E>, T, E>;

	public:
		using value_type = T;
		using error_type = E;
		using unexpected_type = unexpected<E>;


		template <class U>
		using rebind = expected<U, error_type>;

		template <bool Requires = is_default_constructible_v<T>, enable_if_t<Requires, int> = 0>
		constexpr expected()
		{
			this->mHasValue = true;
			eastl::construct_at(eastl::addressof(this->mValue));
		};

		// non-explicit version for when is_convertible_v<U, T> is true.
		template <class U,
		          enable_if_t<is_convertible_v<U, T> && internal::generic_constructor_constraint_v<T, E, U>, int> = 0>
		constexpr expected(U&& v)
		{
			this->mHasValue = true;
			eastl::construct_at(eastl::addressof(this->mValue), eastl::forward<U>(v));
		}

		// explicit version for when is_convertible_v<U, T> is false
		template <class U,
		          enable_if_t<!is_convertible_v<U, T> && internal::generic_constructor_constraint_v<T, E, U>, int> = 0>
		constexpr explicit expected(U&& v)
		{
			this->mHasValue = true;
			eastl::construct_at(eastl::addressof(this->mValue), eastl::forward<U>(v));
		}

		template <class T1,
		          class E1,
		          enable_if_t<internal::expected_to_expected_ctor_constraint_v<T, E, T1, E1, const T1&, const E1&> &&
		                          (!is_convertible_v<const T1&, T> || !is_convertible_v<const E1&, E>),
		                      int> = 0>
		constexpr explicit expected(const expected<T1, E1>& other)
		{
			this->mHasValue = other.has_value();
			if (this->mHasValue)
			{
				eastl::construct_at(eastl::addressof(this->mValue), other.value());
			}
			else
			{
				eastl::construct_at(eastl::addressof(this->mError), other.error());
			}
		}

		// Same as above except this is implicit when is_convertible_v<const T1&, T> && is_convertible_v<const E1&, E>.
		template <class T1,
		          class E1,
		          enable_if_t<internal::expected_to_expected_ctor_constraint_v<T, E, T1, E1, const T1&, const E1&> &&
		                          (is_convertible_v<const T1&, T> && is_convertible_v<const E1&, E>),
		                      int> = 0>
		constexpr expected(const expected<T1, E1>& other)
		{
			this->mHasValue = other.has_value();
			if (this->mHasValue)
			{
				eastl::construct_at(eastl::addressof(this->mValue), other.value());
			}
			else
			{
				eastl::construct_at(eastl::addressof(this->mError), other.error());
			}
		}

		template <class T1,
		          class E1,
		          enable_if_t<internal::expected_to_expected_ctor_constraint_v<T, E, T1, E1, T1, E1> &&
		                          (!is_convertible_v<T1, T> || !is_convertible_v<E1, E>),
		                      int> = 0>
		constexpr explicit expected(expected<T1, E1>&& other)
		{
			this->mHasValue = other.has_value();
			if (this->mHasValue)
			{
				eastl::construct_at(eastl::addressof(this->mValue), eastl::move(other).value());
			}
			else
			{
				eastl::construct_at(eastl::addressof(this->mError), eastl::move(other).error());
			}
		}

		// Same as above except this is implicit when (is_convertible_v<T1, T> && is_convertible_v<E1, E>)
		template <class T1,
		          class E1,
		          enable_if_t<internal::expected_to_expected_ctor_constraint_v<T, E, T1, E1, T1, E1> &&
		                          (is_convertible_v<T1, T> && is_convertible_v<E1, E>),
		                      int> = 0>
		constexpr expected(expected<T1, E1>&& other)
		{
			this->mHasValue = other.has_value();
			if (this->mHasValue)
			{
				eastl::construct_at(eastl::addressof(this->mValue), eastl::move(other).value());
			}
			else
			{
				eastl::construct_at(eastl::addressof(this->mError), eastl::move(other).error());
			}
		}


		template <class G, enable_if_t<is_constructible_v<E, const G&> && !is_convertible_v<const G&, E>, int> = 0>
		constexpr explicit expected(const unexpected<G>& unex)
		{
			this->mHasValue = false;
			eastl::construct_at(eastl::addressof(this->mError), unex.error());
		}

		template <class G, enable_if_t<is_constructible_v<E, const G&> && is_convertible_v<const G&, E>, int> = 0>
		constexpr expected(const unexpected<G>& unex)
		{
			this->mHasValue = false;
			eastl::construct_at(eastl::addressof(this->mError), unex.error());
		}

		template <class G, enable_if_t<is_constructible_v<E, G> && !is_convertible_v<G, E>, int> = 0>
		constexpr explicit expected(unexpected<G>&& unex)
		{
			this->mHasValue = false;
			eastl::construct_at(eastl::addressof(this->mError), eastl::move(unex.error()));
		}

		template <class G, enable_if_t<is_constructible_v<E, G> && is_convertible_v<G, E>, int> = 0>
		constexpr expected(unexpected<G>&& unex)
		{
			this->mHasValue = false;
			eastl::construct_at(eastl::addressof(this->mError), eastl::move(unex.error()));
		}

		template <class... Args, enable_if_t<is_constructible_v<T, Args...>, int> = 0>
		constexpr explicit expected(in_place_t, Args&&... args)
		{
			this->mHasValue = true;
			eastl::construct_at(eastl::addressof(this->mValue), eastl::forward<Args>(args)...);
		}

		template <class U,
		          class... Args,
		          enable_if_t<is_constructible_v<T, std::initializer_list<U>&, Args...>, int> = 0>
		constexpr explicit expected(in_place_t, std::initializer_list<U> il, Args&&... args)
		{
			this->mHasValue = true;
			eastl::construct_at(eastl::addressof(this->mValue), il, eastl::forward<Args>(args)...);
		}

		template <class... Args, enable_if_t<is_constructible_v<E, Args...>, int> = 0>
		constexpr explicit expected(unexpect_t, Args&&... args)
		{
			this->mHasValue = false;
			eastl::construct_at(eastl::addressof(this->mError), eastl::forward<Args>(args)...);
		}

		template <class U,
		          class... Args,
		          enable_if_t<is_constructible_v<E, std::initializer_list<U>&, Args...>, int> = 0>
		constexpr explicit expected(unexpect_t, std::initializer_list<U> il, Args&&... args)
		{
			this->mHasValue = false;
			eastl::construct_at(eastl::addressof(this->mError), il, eastl::forward<Args>(args)...);
		}

		// copy/move assignments are done by means of ExpectedConstructLayer::AssignFrom, and the
		// special function layers the assignments are deleted when they should be.
		//
		// constexpr expected& operator=(const expected&);
		// constexpr expected& operator=(expected&&) noexcept(/* see description */);
		//
		///////

		// Note: The default template parameter is in the standard, the nothrow constraints are
		// also in the standard.
		template <class U = T,
		          enable_if_t<!is_same_v<expected, remove_cvref_t<U>> &&
		                          !internal::is_specialization<remove_cvref_t<U>, unexpected>::value &&
		                          is_constructible_v<T, U> && is_assignable_v<T&, U> &&
		                          (is_nothrow_constructible_v<T, U> || is_nothrow_move_constructible_v<T> ||
		                           is_nothrow_move_constructible_v<E>),
		                      int> = 0>
		constexpr expected& operator=(U&& x)
		{
			if (this->mHasValue)
			{
				this->mValue = eastl::forward<U>(x);
			}
			else
			{
				this->ReInit(this->mValue, this->mError, eastl::forward<U>(x));
				this->mHasValue = true;
			}
			return *this;
		}

		template <class G,
		          enable_if_t<is_constructible_v<E, const G&> && is_assignable_v<E&, const G&> &&
		                          (is_nothrow_constructible_v<E, const G&> || is_nothrow_move_constructible_v<T> ||
		                           is_nothrow_move_constructible_v<E>),
		                      int> = 0>
		constexpr expected& operator=(const unexpected<G>& unex)
		{
			if (this->mHasValue)
			{
				this->ReInit(this->mError, this->mValue, unex.error());
				this->mHasValue = false;
			}
			else
			{
				this->mError = unex.error();
			}
			return *this;
		}

		template <class G,
		          enable_if_t<is_constructible_v<E, G> && is_assignable_v<E&, G> &&
		                          (is_nothrow_constructible_v<E, G> || is_nothrow_move_constructible_v<T> ||
		                           is_nothrow_move_constructible_v<E>),
		                      int> = 0>
		constexpr expected& operator=(unexpected<G>&& unex)
		{
			if (this->mHasValue)
			{
				this->ReInit(this->mError, this->mValue, eastl::move(unex).error());
				this->mHasValue = false;
			}
			else
			{
				this->mError = eastl::move(unex).error();
			}
			return *this;
		}

		// Note: this only works if the constructor is noexcept, kind of strict but that's what the standard dictates...
		template <class... Args, enable_if_t<is_nothrow_constructible_v<T, Args...>, int> = 0>
		constexpr T& emplace(Args&&... args) noexcept
		{
			if (this->mHasValue)
			{
				eastl::destroy_at(&this->mValue);
			}
			else
			{
				eastl::destroy_at(&this->mError);
				this->mHasValue = true;
			}
			return *eastl::construct_at(eastl::addressof(this->mValue), eastl::forward<Args>(args)...);
		}

		// Note: this only works if the constructor is noexcept, kind of strict but that's what the standard dictates...
		template <class U,
		          class... Args,
		          enable_if_t<is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>, int> = 0>
		constexpr T& emplace(std::initializer_list<U> il, Args&&... args) noexcept
		{
			if (this->mHasValue)
			{
				eastl::destroy_at(&this->mValue);
			}
			else
			{
				eastl::destroy_at(&this->mError);
				this->mHasValue = true;
			}
			return *eastl::construct_at(eastl::addressof(this->mValue), il, eastl::forward<Args>(args)...);
		}

		// swap
		template <bool Requires = is_swappable_v<T> && is_swappable_v<E> && is_move_constructible_v<T> &&
		                          is_move_constructible_v<E> &&
		                          (is_nothrow_move_assignable_v<E> || is_nothrow_move_assignable_v<T>),
		          enable_if_t<Requires, int> = 0,
		          bool NoExcept = is_nothrow_move_constructible_v<T> && is_nothrow_swappable_v<T> &&
		                          is_nothrow_move_constructible_v<E> && is_nothrow_swappable_v<E>>
		EA_CPP20_CONSTEXPR void swap(expected& other) noexcept(NoExcept)
		{
			using eastl::swap;
			if (other.mHasValue)
			{
				if (this->mHasValue)
				{
					swap(this->mValue, other.mValue);
				}
				else
				{
					other.swap(*this);
				}
			}
			else // other.mHasValue is false
			{
				if (!this->mHasValue)
				{
					swap(this->mError, other.mError);
				}
				else // `other` has an error and `this` has a value, we need to swap them around.
				{
#if EASTL_EXCEPTIONS_ENABLED
					if constexpr (NoExcept)
					{
#endif
						// Note that is_nothrow_swappable_v implies the destructors cannot throw.
						// The definition of NoExcept implies the constructions here cannot throw.
						// So notheng here throws.
						E tmp(eastl::move(other.mError));
						eastl::destroy_at(&other.mError);
						eastl::construct_at(eastl::addressof(other.mValue), eastl::move(this->mValue));
						eastl::destroy_at(&this->mValue);
						eastl::construct_at(eastl::addressof(this->mError), eastl::move(tmp));
#if EASTL_EXCEPTIONS_ENABLED
					}
					else if constexpr (is_nothrow_move_constructible_v<E>)
					{
						E tmp(eastl::move(other.mError));
						eastl::destroy_at(&other.mError);
						try
						{
							// this may throw
							eastl::construct_at(eastl::addressof(other.mValue), eastl::move(this->mValue));

							eastl::destroy_at(&this->mValue);
							eastl::construct_at(eastl::addressof(this->mError), eastl::move(tmp));
						}
						catch (...)
						{
							// We need to reconstruct other.mError.
							eastl::construct_at(eastl::addressof(other.mError), eastl::move(tmp));
							throw;
						}
					}
					else // T is nothrow_move_constructible (see sfinae condition for swap)
					{
						T tmp(eastl::move(this->mValue));

						eastl::destroy_at(&this->mValue);
						try
						{
							// this may throw
							eastl::construct_at(eastl::addressof(this->mError), eastl::move(other.mError));

							eastl::destroy_at(&other.mError);
							eastl::construct_at(eastl::addressof(other.mValue), eastl::move(tmp));
						}
						catch (...)
						{
							// We need to reconstruct this->mValue
							eastl::construct_at(eastl::addressof(this->mValue), eastl::move(tmp));
							throw;
						}
					}
#endif
					this->mHasValue = false;
					other.mHasValue = true;
				}
			}
		}

		friend constexpr void swap(expected& x, expected& y) noexcept(noexcept(x.swap(y))) { x.swap(y); }

		// These all assume has_value() is true. Otherwise, calling them is UB (as per the
		// standard).  When asserts are enabled, we've decided to assert the precondition
		// similar to what would be done in a hardened library implementation.
		constexpr const T* operator->() const noexcept
		{
			EASTL_ASSERT_MSG(has_value(),
			                 "Pre-condition failed! Accessing an expected value while containing an error.");
			return eastl::addressof(this->mValue);
		}
		constexpr T* operator->() noexcept
		{
			EASTL_ASSERT_MSG(has_value(),
			                 "Pre-condition failed! Accessing an expected value while containing an error.");
			return eastl::addressof(this->mValue);
		}
		constexpr const T& operator*() const& noexcept
		{
			EASTL_ASSERT_MSG(has_value(),
			                 "Pre-condition failed! Accessing an expected value while containing an error.");
			return this->mValue;
		}
		constexpr T& operator*() & noexcept
		{
			EASTL_ASSERT_MSG(has_value(),
			                 "Pre-condition failed! Accessing an expected value while containing an error.");
			return this->mValue;
		}
		constexpr const T&& operator*() const&& noexcept
		{
			EASTL_ASSERT_MSG(has_value(),
			                 "Pre-condition failed! Accessing an expected value while containing an error.");
			return eastl::move(this->mValue);
		}
		constexpr T&& operator*() && noexcept
		{
			EASTL_ASSERT_MSG(has_value(),
			                 "Pre-condition failed! Accessing an expected value while containing an error.");
			return eastl::move(this->mValue);
		}

		constexpr explicit operator bool() const noexcept { return this->mHasValue; }
		constexpr bool has_value() const noexcept { return this->mHasValue; };

		constexpr const T& value() const&
		{
			if (!has_value())
			{
#if EASTL_EXCEPTIONS_ENABLED
				throw(eastl::bad_expected_access(this->mError));
#else
				EASTL_FAIL_MSG("Calling `value()` when expected contains no value.");
#endif
			}
			return this->mValue;
		}
		constexpr T& value() &
		{
			if (!has_value())
			{
#if EASTL_EXCEPTIONS_ENABLED
				throw(eastl::bad_expected_access(this->mError));
#else
				EASTL_FAIL_MSG("Calling `value()` when expected contains no value.");
#endif
			}
			return this->mValue;
		}
		constexpr const T&& value() const&&
		{
			if (!has_value())
			{
#if EASTL_EXCEPTIONS_ENABLED
				throw(eastl::bad_expected_access(eastl::move(this->mError)));
#else
				EASTL_FAIL_MSG("Calling `value()` when expected contains no value.");
#endif
			}
			return eastl::move(this->mValue);
		}
		constexpr T&& value() &&
		{
			if (!has_value())
			{
#if EASTL_EXCEPTIONS_ENABLED
				throw(eastl::bad_expected_access(eastl::move(this->mError)));
#else
				EASTL_FAIL_MSG("Calling `value()` when expected contains no value.");
#endif
			}
			return eastl::move(this->mValue);
		}

		// These all assume has_value() is false. Otherwise, calling them is UB (as per the
		// standard).  When asserts are enabled, we've decided to assert the precondition
		// similar to what would be done in a hardened library implementation.
		constexpr const E& error() const&
		{
			EASTL_ASSERT_MSG(!has_value(), "Pre-condition failed! Calling error() while containing a value.");
			return this->mError;
		};
		constexpr E& error() &
		{
			EASTL_ASSERT_MSG(!has_value(), "Pre-condition failed! Calling error() while containing a value.");
			return this->mError;
		};
		constexpr const E&& error() const&&
		{
			EASTL_ASSERT_MSG(!has_value(), "Pre-condition failed! Calling error() while containing a value.");
			return eastl::move(this->mError);
		};
		constexpr E&& error() &&
		{
			EASTL_ASSERT_MSG(!has_value(), "Pre-condition failed! Calling error() while containing a value.");
			return eastl::move(this->mError);
		};

		template <class U>
		constexpr T value_or(U&& alt) const&
		{
			static_assert(is_copy_constructible_v<T> && is_convertible_v<U, T>);
			return has_value() ? this->mValue : static_cast<T>(eastl::forward<U>(alt));
		}

		template <class U>
		constexpr T value_or(U&& alt) &&
		{
			static_assert(is_move_constructible_v<T> && is_convertible_v<U, T>);
			return has_value() ? eastl::move(this->mValue) : static_cast<T>(eastl::forward<U>(alt));
		}

		template <class U>
		constexpr E error_or(U&& alt) const&
		{
			static_assert(is_copy_constructible_v<E> && is_convertible_v<U, E>);
			if (has_value())
			{
				return eastl::forward<U>(alt);
			}
			return this->mError;
		}

		template <class U>
		constexpr E error_or(U&& alt) &&
		{
			static_assert(is_move_constructible_v<E> && is_convertible_v<U, E>);
			if (has_value())
			{
				return eastl::forward<U>(alt);
			}
			return eastl::move(this->mError);
		}

		// Note: the constraint in the standard is is_constructible_v<E, decltype(error())>
		// here and is_constructible_v<E, decltype(std::move(error()))> in the && qualified
		// versions, we're just explicitly spellig the decltype in our implementations since we
		// can't put the member call in the template argument. declval doesn't really help us
		// much since it always returns an rvalue reference, and `expected` is an incomplete
		// type at this point.
		template <class F, bool Requires = is_constructible_v<E, E&>, enable_if_t<Requires, int> = 0>
		constexpr auto and_then(F&& f) &
		{
			using U = remove_cvref_t<invoke_result_t<F, decltype(value())>>;
			static_assert(is_same_v<typename U::error_type, E> && internal::is_specialization<U, expected>::value);
			if (has_value())
			{
				return eastl::invoke(eastl::forward<F>(f), value());
			}
			return U(unexpect, error());
		}

		// See note about constraint above.
		template <class F, bool Requires = is_constructible_v<E, const E&>, enable_if_t<Requires, int> = 0>
		constexpr auto and_then(F&& f) const&
		{
			using U = remove_cvref_t<invoke_result_t<F, decltype(value())>>;
			static_assert(is_same_v<typename U::error_type, E> && internal::is_specialization<U, expected>::value);
			if (has_value())
			{
				return eastl::invoke(eastl::forward<F>(f), value());
			}
			return U(unexpect, error());
		}

		// See note about constraint above.
		template <class F, bool Requires = is_constructible_v<E, E&&>, enable_if_t<Requires, int> = 0>
		constexpr auto and_then(F&& f) &&
		{
			using U = remove_cvref_t<invoke_result_t<F, decltype(eastl::move(value()))>>;
			static_assert(is_same_v<typename U::error_type, E> && internal::is_specialization<U, expected>::value);
			if (has_value())
			{
				return eastl::invoke(eastl::forward<F>(f), eastl::move(value()));
			}
			return U(unexpect, eastl::move(error()));
		}

		// See note about constraint above.
		template <class F, bool Requires = is_constructible_v<E, const E&&>, enable_if_t<Requires, int> = 0>
		constexpr auto and_then(F&& f) const&&
		{
			using U = remove_cvref_t<invoke_result_t<F, decltype(eastl::move(value()))>>;
			static_assert(is_same_v<typename U::error_type, E> && internal::is_specialization<U, expected>::value);
			if (has_value())
			{
				return eastl::invoke(eastl::forward<F>(f), eastl::move(value()));
			}
			return U(unexpect, eastl::move(error()));
		}

		template <class F, bool Requires = is_copy_constructible_v<T>, enable_if_t<Requires, int> = 0>
		constexpr auto or_else(F&& f) &
		{
			using G = remove_cvref_t<invoke_result_t<F, decltype(error())>>;
			static_assert(is_same_v<typename G::value_type, T> && internal::is_specialization<G, expected>::value);
			if (has_value())
			{
				return G(in_place, value());
			}
			return eastl::invoke(eastl::forward<F>(f), error());
		}

		template <class F, bool Requires = is_copy_constructible_v<T>, enable_if_t<Requires, int> = 0>
		constexpr auto or_else(F&& f) const&
		{
			using G = remove_cvref_t<invoke_result_t<F, decltype(error())>>;
			static_assert(is_same_v<typename G::value_type, T> && internal::is_specialization<G, expected>::value);
			if (has_value())
			{
				return G(in_place, value());
			}
			return eastl::invoke(eastl::forward<F>(f), error());
		}

		template <class F, bool Requires = is_move_constructible_v<T>, enable_if_t<Requires, int> = 0>
		constexpr auto or_else(F&& f) &&
		{
			using G = remove_cvref_t<invoke_result_t<F, decltype(eastl::move(error()))>>;
			static_assert(is_same_v<typename G::value_type, T> && internal::is_specialization<G, expected>::value);
			if (has_value())
			{
				return G(in_place, eastl::move(value()));
			}
			return eastl::invoke(eastl::forward<F>(f), eastl::move(error()));
		}

		template <class F, bool Requires = is_move_constructible_v<T>, enable_if_t<Requires, int> = 0>
		constexpr auto or_else(F&& f) const&&
		{
			using G = remove_cvref_t<invoke_result_t<F, decltype(eastl::move(error()))>>;
			static_assert(is_same_v<typename G::value_type, T> && internal::is_specialization<G, expected>::value);
			if (has_value())
			{
				return G(in_place, eastl::move(value()));
			}
			return eastl::invoke(eastl::forward<F>(f), eastl::move(error()));
		}

		template <class F, bool Requires = is_copy_constructible_v<E>, enable_if_t<Requires, int> = 0>
		constexpr auto transform(F&& f) &
		{
			using U = remove_cv_t<invoke_result_t<F, decltype(value())>>;
			if (!has_value())
			{
				return expected<U, E>(unexpect, error());
			}

			if constexpr (is_void_v<U>)
			{
				eastl::invoke(eastl::forward<F>(f), value());
				return expected<U, E>();
			}
			else
			{
				return expected<U, E>(eastl::invoke(eastl::forward<F>(f), value()));
			}
		}

		template <class F, bool Requires = is_copy_constructible_v<E>, enable_if_t<Requires, int> = 0>
		constexpr auto transform(F&& f) const&
		{
			using U = remove_cv_t<invoke_result_t<F, decltype(value())>>;
			if (!has_value())
			{
				return expected<U, E>(unexpect, error());
			}

			if constexpr (is_void_v<U>)
			{
				eastl::invoke(eastl::forward<F>(f), value());
				return expected<U, E>();
			}
			else
			{
				return expected<U, E>(eastl::invoke(eastl::forward<F>(f), value()));
			}
		}

		template <class F, bool Requires = is_move_constructible_v<E>, enable_if_t<Requires, int> = 0>
		constexpr auto transform(F&& f) &&
		{
			using U = remove_cv_t<invoke_result_t<F, decltype(eastl::move(value()))>>;
			if (!has_value())
			{
				return expected<U, E>(unexpect, eastl::move(error()));
			}

			if constexpr (is_void_v<U>)
			{
				eastl::invoke(eastl::forward<F>(f), eastl::move(value()));
				return expected<U, E>();
			}
			else
			{
				return expected<U, E>(eastl::invoke(eastl::forward<F>(f), eastl::move(value())));
			}
		}

		template <class F, bool Requires = is_move_constructible_v<E>, enable_if_t<Requires, int> = 0>
		constexpr auto transform(F&& f) const&&
		{
			using U = remove_cv_t<invoke_result_t<F, decltype(eastl::move(value()))>>;
			if (!has_value())
			{
				return expected<U, E>(unexpect, eastl::move(error()));
			}

			if constexpr (is_void_v<U>)
			{
				eastl::invoke(eastl::forward<F>(f), eastl::move(value()));
				return expected<U, E>();
			}
			else
			{
				return expected<U, E>(eastl::invoke(eastl::forward<F>(f), eastl::move(value())));
			}
		}

		template <class F, bool Requires = is_copy_constructible_v<T>, enable_if_t<Requires, int> = 0>
		constexpr auto transform_error(F&& f) &
		{
			using G = remove_cv_t<invoke_result_t<F, decltype(error())>>;
			if (has_value())
			{
				return expected<T, G>(in_place, value());
			}
			else
			{
				return expected<T, G>(unexpect, eastl::invoke(eastl::forward<F>(f), error()));
			}
		}

		template <class F, bool Requires = is_copy_constructible_v<T>, enable_if_t<Requires, int> = 0>
		constexpr auto transform_error(F&& f) const&
		{
			using G = remove_cv_t<invoke_result_t<F, decltype(error())>>;
			if (has_value())
			{
				return expected<T, G>(in_place, value());
			}
			else
			{
				return expected<T, G>(unexpect, eastl::invoke(eastl::forward<F>(f), error()));
			}
		}

		template <class F, bool Requires = is_move_constructible_v<T>, enable_if_t<Requires, int> = 0>
		constexpr auto transform_error(F&& f) &&
		{
			using G = remove_cv_t<invoke_result_t<F, decltype(eastl::move(error()))>>;
			if (has_value())
			{
				return expected<T, G>(in_place, eastl::move(value()));
			}
			else
			{
				return expected<T, G>(unexpect, eastl::invoke(eastl::forward<F>(f), eastl::move(error())));
			}
		}

		template <class F, bool Requires = is_move_constructible_v<T>, enable_if_t<Requires, int> = 0>
		constexpr auto transform_error(F&& f) const&&
		{
			using G = remove_cv_t<invoke_result_t<F, decltype(eastl::move(error()))>>;
			if (has_value())
			{
				return expected<T, G>(in_place, eastl::move(value()));
			}
			else
			{
				return expected<T, G>(unexpect, eastl::invoke(eastl::forward<F>(f), eastl::move(error())));
			}
		}

		// equality operators
		template <class T2, class E2>
		friend constexpr bool operator==(const expected& x, const expected<T2, E2>& y)
		{
			if (x.has_value() != y.has_value())
			{
				return false;
			}
			if (x.has_value())
			{
				return *x == *y;
			}
			return x.error() == y.error();
		}

		template <class T2>
		friend constexpr bool operator==(const expected& x, const T2& y)
		{
			return x.has_value() && static_cast<bool>(*x == y);
		}

		template <class E2>
		friend constexpr bool operator==(const expected& x, const unexpected<E2>& y)
		{
			return !x.has_value() && static_cast<bool>(x.error() == y.error());
		}

	private:
		static_assert(is_same_v<remove_cv_t<T>, void> ||
		                  (is_object_v<remove_cv_t<T>> && !is_array_v<remove_cv_t<T>> &&
		                   !is_same_v<remove_cv_t<T>, in_place_t> && !is_same_v<remove_cv_t<T>, unexpect_t> &&
		                   !internal::is_specialization<remove_cv_t<T>, unexpected>::value),
		              "Invalid type for eastl::expected.");


		// TODO: When T is not cv void, it shall meet the
		// Cpp17Destructible requirements. E shall meet the
		// Cpp17Destructible requirements. Can we statically assert this?
	};

	namespace internal
	{
		// Used as a fake "Value" type in the void specialization of expected so it can be default
		// constructible and so we can use all the other machinery we have for value/error pairs.
		struct ExpectedEmptyUnionMember
		{
			constexpr ExpectedEmptyUnionMember() noexcept {
			    // Provide default constructor to avoid zero-initialization when objects are value-initialized.
			};
		};
	} // namespace internal

	// TODO: The standard has specializations for all cv-qualified void, but we're only doing
	// it for non-qualified void.
	template <class E>
	class expected<void, E>
	    : internal::EnableExpectedSpecialMemberFunctions<internal::ExpectedConstructLayer<internal::ExpectedEmptyUnionMember, E>,
	                                           E>
	{
	private:
		using LayeredBase = internal::
			EnableExpectedSpecialMemberFunctions<internal::ExpectedConstructLayer<internal::ExpectedEmptyUnionMember, E>, E>;

	public:
		using value_type = void;
		using error_type = E;
		using unexpected_type = unexpected<E>;

		template <class U>
		using rebind = expected<U, error_type>;

		constexpr expected() noexcept { this->mHasValue = true; }
		constexpr expected(in_place_t) noexcept { this->mHasValue = true; }

		// Copy/move constructors and the destructor are handled by the layers.

		template <class U,
		          class G,
		          enable_if_t<is_void_v<U> && is_constructible_v<E, const G&> &&
		                          !is_constructible_v<unexpected<E>, expected<U, G>&> &&
		                          !is_constructible_v<unexpected<E>, expected<U, G>> &&
		                          !is_constructible_v<unexpected<E>, const expected<U, G>&> &&
		                          !is_constructible_v<unexpected<E>, const expected<U, G>>,
		                      int> = 0>
		// TODO: More SFINAE for the explicit vs not explicit version...
		// explicit(!is_convertible_v<const G&, E>)
		constexpr expected(const expected<U, G>& other)
		{
			this->mHasValue = other.has_value();
			if (!this->mHasValue)
			{
				eastl::construct_at(eastl::addressof(this->mError), other.error());
			}
		}

		template <class U,
		          class G,
		          enable_if_t<is_void_v<U> && is_constructible_v<E, G> &&
		                          !is_constructible_v<unexpected<E>, expected<U, G>&> &&
		                          !is_constructible_v<unexpected<E>, expected<U, G>> &&
		                          !is_constructible_v<unexpected<E>, const expected<U, G>&> &&
		                          !is_constructible_v<unexpected<E>, const expected<U, G>>,
		                      int> = 0>
		// TODO: More SFINAE for the explicit vs not explicit version...
		// explicit(!is_convertible_v<T1, T> || !is_convertible_v<E1, E>)
		constexpr expected(expected<U, G>&& other)
		{
			this->mHasValue = other.has_value();
			if (!this->mHasValue)
			{
				eastl::construct_at(eastl::addressof(this->mError), eastl::move(other).error());
			}
		}


		// Conversion from unexpected lvalue explicit version.
		template <class G, enable_if_t<is_constructible_v<E, const G&> && !is_convertible_v<const G&, E>, int> = 0>
		constexpr explicit expected(const unexpected<G>& unex)
		{
			this->mHasValue = false;
			eastl::construct_at(eastl::addressof(this->mError), unex.error());
		}

		// Conversion from unexpected lvalue non-explicit version.
		template <class G, enable_if_t<is_constructible_v<E, const G&> && is_convertible_v<const G&, E>, int> = 0>
		constexpr expected(const unexpected<G>& unex)
		{
			this->mHasValue = false;
			eastl::construct_at(eastl::addressof(this->mError), unex.error());
		}

		// Conversion from unexpected rvalue explicit version.
		template <class G, enable_if_t<is_constructible_v<E, G> && !is_convertible_v<G, E>, int> = 0>
		constexpr explicit expected(unexpected<G>&& unex)
		{
			this->mHasValue = false;
			eastl::construct_at(eastl::addressof(this->mError), eastl::move(unex.error()));
		}

		// Conversion from unexpected rvalue non-explicit version.
		template <class G, enable_if_t<is_constructible_v<E, G> && is_convertible_v<G, E>, int> = 0>
		constexpr expected(unexpected<G>&& unex)
		{
			this->mHasValue = false;
			eastl::construct_at(eastl::addressof(this->mError), eastl::move(unex.error()));
		}

		template <class... Args, enable_if_t<is_constructible_v<E, Args...>, int> = 0>
		constexpr explicit expected(unexpect_t, Args&&... args)
		{
			this->mHasValue = false;
			eastl::construct_at(eastl::addressof(this->mError), eastl::forward<Args>(args)...);
		}

		template <class U,
		          class... Args,
		          enable_if_t<is_constructible_v<E, std::initializer_list<U>&, Args...>, int> = 0>
		constexpr explicit expected(unexpect_t, std::initializer_list<U> il, Args&&... args)
		{
			this->mHasValue = false;
			eastl::construct_at(eastl::addressof(this->mError), il, eastl::forward<Args>(args)...);
		}

		////////
		//
		// copy/move assignments are done by means of ExpectedConstructLayer::AssignFrom, and the
		// special function layers so the assignments are deleted when they should be.
		//
		// constexpr expected& operator=(const expected&);
		// constexpr expected& operator=(expected&&) noexcept(/* see description */);
		//
		///////


		template <class G, enable_if_t<is_constructible_v<E, const G&> && is_assignable_v<E&, const G&>, int> = 0>
		constexpr expected& operator=(const unexpected<G>& unex)
		{
			if (this->mHasValue)
			{
				eastl::construct_at(eastl::addressof(this->mError), unex.error());
				this->mHasValue = false;
			}
			else
			{
				this->mError = unex.error();
			}
			return *this;
		}

		template <class G, enable_if_t<is_constructible_v<E, const G&> && is_assignable_v<E&, G>, int> = 0>
		constexpr expected& operator=(unexpected<G>&& unex)
		{
			if (this->mHasValue)
			{
				eastl::construct_at(eastl::addressof(this->mError), eastl::move(unex.error()));
				this->mHasValue = false;
			}
			else
			{
				this->mError = unex.error();
			}
			return *this;
		}

		template <bool Requires = is_swappable_v<E> && is_move_constructible_v<E>,
		          enable_if_t<Requires, int> = 0,
		          bool NoExcept = is_nothrow_move_constructible_v<E> && is_nothrow_swappable_v<E>>
		EA_CPP20_CONSTEXPR void swap(expected& other) noexcept(NoExcept)
		{
			using eastl::swap;
			if (other.mHasValue)
			{
				if (this->mHasValue)
				{
					return;
				}
				else
				{
					other.swap(*this);
				}
			}
			else // other.mHasValue is false
			{
				if (!this->mHasValue)
				{
					swap(this->mError, other.mError);
				}
				else
				{
					// other has an error and this has a value, we need to swap them around.
					eastl::construct_at(eastl::addressof(this->mError), eastl::move(other.mError));
					eastl::destroy_at(&other.mError);

					this->mHasValue = false;
					other.mHasValue = true;
				}
			}
		}

		friend constexpr void swap(expected& x, expected& y) noexcept(noexcept(x.swap(y))) { x.swap(y); }

		constexpr explicit operator bool() const noexcept { return this->mHasValue; }

		constexpr bool has_value() const noexcept { return this->mHasValue; };

		constexpr void operator*() const noexcept {}

		constexpr void value() const&
		{
			if (!has_value())
			{
#if EASTL_EXCEPTIONS_ENABLED
				throw(eastl::bad_expected_access(eastl::move(this->mError)));
#else
				EASTL_FAIL_MSG("Calling `value()` when expected contains no value.");
#endif
			}
		}

		constexpr void value() &&
		{
			if (!has_value())
			{
#if EASTL_EXCEPTIONS_ENABLED
				throw(eastl::bad_expected_access(eastl::move(this->mError)));
#else
				EASTL_FAIL_MSG("Calling `value()` when expected contains no value.");
#endif
			}
		}

		// These assume has_value() is false, otherwise this is UB, as per the standard.
		constexpr const E& error() const& noexcept { return this->mError; };
		constexpr E& error() & noexcept { return this->mError; };
		constexpr const E&& error() const&& noexcept { return eastl::move(this->mError); };
		constexpr E&& error() && noexcept { return eastl::move(this->mError); };

		template <class U = E>
		constexpr E error_or(U&& alt) const&
		{
			static_assert(is_copy_constructible_v<E> && is_convertible_v<U, E>);
			if (has_value())
			{
				return eastl::forward<U>(alt);
			}
			return this->mError;
		}

		template <class U = E>
		constexpr E error_or(U&& alt) &&
		{
			static_assert(is_move_constructible_v<E> && is_convertible_v<U, E>);
			if (has_value())
			{
				return eastl::forward<U>(alt);
			}
			return eastl::move(this->mError);
		}

		///////////////////////
		// Monadic operations
		///////////////////////
		template <class F, bool Requires = is_copy_constructible_v<E>, enable_if_t<Requires, int> = 0>
		constexpr auto and_then(F&& f) &
		{
			using U = remove_cvref_t<invoke_result_t<F>>;
			static_assert(is_same_v<typename U::error_type, E> && internal::is_specialization<U, expected>::value);
			if (has_value())
			{
				return eastl::invoke(eastl::forward<F>(f));
			}
			return U(unexpect, error());
		}

		template <class F, bool Requires = is_copy_constructible_v<E>, enable_if_t<Requires, int> = 0>
		constexpr auto and_then(F&& f) const&
		{
			using U = remove_cvref_t<invoke_result_t<F>>;
			static_assert(is_same_v<typename U::error_type, E> && internal::is_specialization<U, expected>::value);
			if (has_value())
			{
				return eastl::invoke(eastl::forward<F>(f));
			}
			return U(unexpect, error());
		}

		template <class F, bool Requires = is_move_constructible_v<E>, enable_if_t<Requires, int> = 0>
		constexpr auto and_then(F&& f) &&
		{
			using U = remove_cvref_t<invoke_result_t<F>>;
			static_assert(is_same_v<typename U::error_type, E> && internal::is_specialization<U, expected>::value);
			if (has_value())
			{
				return eastl::invoke(eastl::forward<F>(f));
			}
			return U(unexpect, eastl::move(error()));
		}

		template <class F, bool Requires = is_move_constructible_v<E>, enable_if_t<Requires, int> = 0>
		constexpr auto and_then(F&& f) const&&
		{
			using U = remove_cvref_t<invoke_result_t<F, decltype(eastl::move(value()))>>;
			static_assert(is_same_v<typename U::error_type, E> && internal::is_specialization<U, expected>::value);
			if (has_value())
			{
				return eastl::invoke(eastl::forward<F>(f));
			}
			return U(unexpect, eastl::move(error()));
		}

		template <class F>
		constexpr auto or_else(F&& f) &
		{
			using G = remove_cvref_t<invoke_result_t<F, decltype(error())>>;
			static_assert(is_same_v<typename G::value_type, void> && internal::is_specialization<G, expected>::value);
			if (has_value())
			{
				return G();
			}
			return eastl::invoke(eastl::forward<F>(f), error());
		}

		template <class F>
		constexpr auto or_else(F&& f) const&
		{
			using G = remove_cvref_t<invoke_result_t<F, decltype(error())>>;
			static_assert(is_same_v<typename G::value_type, void> && internal::is_specialization<G, expected>::value);
			if (has_value())
			{
				return G();
			}
			return eastl::invoke(eastl::forward<F>(f), error());
		}

		template <class F>
		constexpr auto or_else(F&& f) &&
		{
			using G = remove_cvref_t<invoke_result_t<F, decltype(eastl::move(error()))>>;
			static_assert(is_same_v<typename G::value_type, void> && internal::is_specialization<G, expected>::value);
			if (has_value())
			{
				return G();
			}
			return eastl::invoke(eastl::forward<F>(f), eastl::move(error()));
		}

		template <class F>
		constexpr auto or_else(F&& f) const&&
		{
			using G = remove_cvref_t<invoke_result_t<F, decltype(eastl::move(error()))>>;
			static_assert(is_same_v<typename G::value_type, void> && internal::is_specialization<G, expected>::value);
			if (has_value())
			{
				return G();
			}
			return eastl::invoke(eastl::forward<F>(f), eastl::move(error()));
		}

		template <class F, bool Requires = is_copy_constructible_v<E>, enable_if_t<Requires, int> = 0>
		constexpr auto transform(F&& f) &
		{
			using U = remove_cv_t<invoke_result_t<F>>;
			if (!has_value())
			{
				return expected<U, E>(unexpect, error());
			}

			if constexpr (is_void_v<U>)
			{
				eastl::invoke(eastl::forward<F>(f));
				return expected<U, E>();
			}
			else
			{
				return expected<U, E>(eastl::invoke(eastl::forward<F>(f)));
			}
		}

		template <class F, bool Requires = is_copy_constructible_v<E>, enable_if_t<Requires, int> = 0>
		constexpr auto transform(F&& f) const&
		{
			using U = remove_cv_t<invoke_result_t<F>>;
			if (!has_value())
			{
				return expected<U, E>(unexpect, error());
			}

			if constexpr (is_void_v<U>)
			{
				eastl::invoke(eastl::forward<F>(f));
				return expected<U, E>();
			}
			else
			{
				return expected<U, E>(eastl::invoke(eastl::forward<F>(f)));
			}
		}

		template <class F, bool Requires = is_move_constructible_v<E>, enable_if_t<Requires, int> = 0>
		constexpr auto transform(F&& f) &&
		{
			using U = remove_cv_t<invoke_result_t<F>>;
			if (!has_value())
			{
				return expected<U, E>(unexpect, eastl::move(error()));
			}

			if constexpr (is_void_v<U>)
			{
				eastl::invoke(eastl::forward<F>(f));
				return expected<U, E>();
			}
			else
			{
				return expected<U, E>(eastl::invoke(eastl::forward<F>(f), eastl::move(value())));
			}
		}

		template <class F, bool Requires = is_move_constructible_v<E>, enable_if_t<Requires, int> = 0>
		constexpr auto transform(F&& f) const&&
		{
			using U = remove_cv_t<invoke_result_t<F>>;
			if (!has_value())
			{
				return expected<U, E>(unexpect, eastl::move(error()));
			}

			if constexpr (is_void_v<U>)
			{
				eastl::invoke(eastl::forward<F>(f));
				return expected<U, E>();
			}
			else
			{
				return expected<U, E>(eastl::invoke(eastl::forward<F>(f), eastl::move(value())));
			}
		}

		template <class F>
		constexpr auto transform_error(F&& f) &
		{
			using G = remove_cv_t<invoke_result_t<F, decltype(error())>>;
			if (has_value())
			{
				return expected<void, G>();
			}
			else
			{
				return expected<void, G>(unexpect, eastl::invoke(eastl::forward<F>(f), error()));
			}
		}

		template <class F>
		constexpr auto transform_error(F&& f) const&
		{
			using G = remove_cv_t<invoke_result_t<F, decltype(error())>>;
			if (has_value())
			{
				return expected<void, G>();
			}
			else
			{
				return expected<void, G>(unexpect, eastl::invoke(eastl::forward<F>(f), error()));
			}
		}

		template <class F>
		constexpr auto transform_error(F&& f) &&
		{
			using G = remove_cv_t<invoke_result_t<F, decltype(eastl::move(error()))>>;
			if (has_value())
			{
				return expected<void, G>();
			}
			else
			{
				return expected<void, G>(unexpect, eastl::invoke(eastl::forward<F>(f), eastl::move(error())));
			}
		}

		template <class F>
		constexpr auto transform_error(F&& f) const&&
		{
			using G = remove_cv_t<invoke_result_t<F, decltype(eastl::move(error()))>>;
			if (has_value())
			{
				return expected<void, G>();
			}
			else
			{
				return expected<void, G>(unexpect, eastl::invoke(eastl::forward<F>(f), eastl::move(error())));
			}
		}

		// equality operators
		template <class T2, class E2, bool Requires = is_void_v<T2>, enable_if_t<Requires, int> = 0>
		friend constexpr bool operator==(const expected& x, const expected<T2, E2>& y)
		{
			if (x.has_value() != y.has_value())
			{
				return false;
			}
			return x.has_value() || static_cast<bool>(x.error() == y.error());
		}

		template <class E2>
		friend constexpr bool operator==(const expected& x, const unexpected<E2>& y)
		{
			return !x.has_value() && static_cast<bool>(x.error() == y.error());
		}
	};


} // namespace eastl

EA_RESTORE_VC_WARNING() EA_RESTORE_VC_WARNING() EA_RESTORE_VC_WARNING()

#endif
