///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// Implements the class template variant represents a type-safe union. An
// instance of variant at any given time either holds a value of one of its
// alternative types, or it holds no value.
//
// As with unions, if a variant holds a value of some object type T, the object
// representation of T is allocated directly within the object representation of
// the variant itself.
//
// Variant is not allowed to allocate additional (dynamic) memory.
//
// A variant is not permitted to hold references, arrays, or the type void.
// Empty variants are also ill-formed (variant<monostate> can be used instead).
//
// A variant is permitted to hold the same type more than once, and to hold
// differently cv-qualified versions of the same type.  As with unions, the
// default-initialized variant holds a value of its first alternative, unless
// that alternative is not default-constructible (in which case default
// constructor won't compile: the helper class monostate can be used to make
// such variants default-constructible)
//
// Given defect 2901, the eastl::variant implementation does not provide the
// specified allocator-aware functions.  This will be re-evaluated when the LWG
// addresses this issue in future standardization updates.
// LWG Defect 2901: https://cplusplus.github.io/LWG/issue2901
//
// Allocator-extended constructors
// template <class Alloc> variant(allocator_arg_t, const Alloc&);
// template <class Alloc> variant(allocator_arg_t, const Alloc&, const variant&);
// template <class Alloc> variant(allocator_arg_t, const Alloc&, variant&&);
// template <class Alloc, class T> variant(allocator_arg_t, const Alloc&, T&&);
// template <class Alloc, class T, class... Args> variant(allocator_arg_t, const Alloc&, in_place_type_t<T>, Args&&...);
// template <class Alloc, class T, class U, class... Args> variant(allocator_arg_t, const Alloc&, in_place_type_t<T>, initializer_list<U>, Args&&...);
// template <class Alloc, size_t I, class... Args> variant(allocator_arg_t, const Alloc&, in_place_index_t<I>, Args&&...);
// template <class Alloc, size_t I, class U, class... Args> variant(allocator_arg_t, const Alloc&, in_place_index_t<I>, initializer_list<U>, Args&&...);
//
// 20.7.12, allocator-related traits
// template <class T, class Alloc>        struct uses_allocator;
// template <class... Types, class Alloc> struct uses_allocator<variant<Types...>, Alloc>;
//
// eastl::variant doesn't support:
//  * recursive variant support
//  * strong exception guarantees as specified (we punted on the assignment problem).
//    if an exception is thrown during assignment its undefined behaviour in our implementation.
//
// Reference:
// 	* http://en.cppreference.com/w/cpp/utility/variant
// 	* https://thenewcpp.wordpress.com/2012/02/15/variadic-templates-part-3-or-how-i-wrote-a-variant-class/
///////////////////////////////////////////////////////////////////////////

#ifndef EASTL_VARIANT_H
#define EASTL_VARIANT_H

#include <EASTL/internal/config.h>
#include <EASTL/internal/in_place_t.h>
#include <EASTL/internal/integer_sequence.h>
#include <EASTL/internal/special_member_functions_variant_optional.h>
#include <EASTL/meta.h>
#include <EASTL/utility.h>
#include <EASTL/functional.h>
#include <EASTL/initializer_list.h>
#include <EASTL/tuple.h>
#include <EASTL/type_traits.h>
#include <EASTL/array.h>

#if EASTL_EXCEPTIONS_ENABLED
	#include <stdexcept>
	#include <exception>
#endif


#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif

#ifndef EA_COMPILER_CPP14_ENABLED
	static_assert(false, "eastl::variant requires a C++14 compatible compiler (at least) ");
#endif

EA_DISABLE_VC_WARNING(4625) // copy constructor was implicitly defined as deleted

namespace eastl
{
	///////////////////////////////////////////////////////////////////////////
	// 20.7.3, variant_npos
	//
	EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR size_t variant_npos = size_t(-1);


	///////////////////////////////////////////////////////////////////////////
	// 20.7.10, class bad_variant_access
	//
	#if EASTL_EXCEPTIONS_ENABLED
		struct bad_variant_access : public std::logic_error
		{
			bad_variant_access() : std::logic_error("eastl::bad_variant_access exception") {}
			virtual ~bad_variant_access() EA_NOEXCEPT {}
		};
	#endif


	///////////////////////////////////////////////////////////////////////////
	// 20.7.7, class monostate
	//
	// Unit type intended for use as a well-behaved empty alternative in
	// variant. A variant of non-default-constructible types may list monostate
	// as its first alternative: this makes the variant itself default-contructible.
	//
	struct monostate {};

	// 20.7.8, monostate relational operators
#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	EA_CONSTEXPR std::strong_ordering operator<=>(monostate, monostate) EA_NOEXCEPT { return std::strong_ordering::equal; }
#else
	EA_CONSTEXPR bool operator> (monostate, monostate) EA_NOEXCEPT { return false; }
	EA_CONSTEXPR bool operator< (monostate, monostate) EA_NOEXCEPT { return false; }
	EA_CONSTEXPR bool operator!=(monostate, monostate) EA_NOEXCEPT { return false; }
	EA_CONSTEXPR bool operator<=(monostate, monostate) EA_NOEXCEPT { return true; }
	EA_CONSTEXPR bool operator>=(monostate, monostate) EA_NOEXCEPT { return true; }
#endif
	EA_CONSTEXPR bool operator==(monostate, monostate) EA_NOEXCEPT { return true; }

	// 20.7.11, hash support
	template <class T> struct hash;
	template <> struct hash<monostate>
		{ size_t operator()(monostate) const { return static_cast<size_t>(-0x42); } };


	///////////////////////////////////////////////////////////////////////////
	// 20.7.2, forward-declaration for types that depend on the variant
	//
	template <class... Types>
	class variant;


	///////////////////////////////////////////////////////////////////////////
	// 20.7.3, variant_size, variant_size_v helper classes
	//
	template <class T>        struct variant_size;
	template <class T>        struct variant_size<const T>           : integral_constant<size_t, variant_size<T>::value>  {};
	template <class T>        struct variant_size<volatile T>        : integral_constant<size_t, variant_size<T>::value>  {};
	template <class T>        struct variant_size<const volatile T>  : integral_constant<size_t, variant_size<T>::value>  {};
	template <class... Types> struct variant_size<variant<Types...>> : integral_constant<size_t, sizeof...(Types)> {};

	// variant_size_v template alias
	template <typename T>
	EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR size_t variant_size_v = variant_size<T>::value;


	///////////////////////////////////////////////////////////////////////////
	// variant_alternative_helper
	//
	// This helper does the heavy lifting of traversing the variadic type list
	// and retrieving the type at the user provided index.
	//
	template <size_t I, typename... Ts>
	struct variant_alternative_helper;

	template <size_t I, typename Head, typename... Tail>
	struct variant_alternative_helper<I, Head, Tail...>
		{ typedef typename variant_alternative_helper<I - 1, Tail...>::type type; };

	template <typename Head, typename... Tail>
	struct variant_alternative_helper<0, Head, Tail...>
		{ typedef Head type; };


	///////////////////////////////////////////////////////////////////////////
	// 20.7.4, variant_alternative
	//
	template <size_t I, class T> struct variant_alternative;
	template <size_t I, class... Types> struct variant_alternative<I, variant<Types...>> : variant_alternative_helper<I, Types...> {};

	// ISO required cv-qualifer specializations
	template <size_t I, class T> struct variant_alternative<I, const T>          : add_const<typename variant_alternative<I, T>::type> {};
	template <size_t I, class T> struct variant_alternative<I, volatile T>       : add_volatile<typename variant_alternative<I, T>::type> {};
	template <size_t I, class T> struct variant_alternative<I, const volatile T> : add_cv<typename variant_alternative<I, T>::type> {};

	// variant_alternative_t template alias
	template <size_t I, class T> using variant_alternative_t = typename variant_alternative<I, T>::type;


	///////////////////////////////////////////////////////////////////////////
	// 20.7.11, hash support
	//
	namespace internal {
	template<class Variant, bool enabled>
	struct variant_hash {};

	template<class Variant>
	struct variant_hash<Variant, true>
	{
		size_t operator()(const Variant& obj) const
		{
			if (obj.valueless_by_exception())
				return 0;

			auto genericVisitor = [](const auto& thisAlternative)
			{
				using alternative_t = remove_const_t<remove_reference_t<decltype(thisAlternative)>>;
				return hash<alternative_t>{}(thisAlternative);
			};

			const size_t alternativeHash = visit(genericVisitor, obj);

			return (alternativeHash * 16777619) ^ obj.index();
		}
	};

	template<class, class, class = void>
	struct is_hasher_for : public false_type {};

	template<class Hash, class Key>
	struct is_hasher_for <Hash, Key, void_t<
		decltype(Hash()) // default constructible
		, decltype(Hash(declval<Hash>())) // copy constructible
		, decltype(declval<Hash>() = declval<const Hash&>()) // copy assignable
		, decltype(swap(declval<Hash>(), declval<Hash>())) // swappable
		, decltype(size_t(declval<const Hash>()(declval<const Key&>()))) // function object
		>> : public true_type {};

	template <class Hash, class Key>
	constexpr bool is_hasher_for_v = is_hasher_for<Hash, Key>::value;
	} // namespace internal

	template <class... Types>
	struct hash<variant<Types...>>
		: public internal::variant_hash<variant<Types...>, conjunction_v<internal::is_hasher_for<hash<remove_const_t<Types>>, Types>...>> {};


	///////////////////////////////////////////////////////////////////////////
	// get_if
	//
	template <size_t I, class... Types, enable_if_t<I < sizeof...(Types), bool> = true>
	EA_CONSTEXPR add_pointer_t<variant_alternative_t<I, variant<Types...>>> get_if(variant<Types...>* pv) EA_NOEXCEPT
	{
		using return_type = add_pointer_t<variant_alternative_t<I, variant<Types...>>>;

		return (!pv || pv->index() != I) ? nullptr : pv->template get_as<return_type>();
	}

	template <size_t I, class... Types, enable_if_t<I < sizeof...(Types), bool> = true>
	EA_CONSTEXPR add_pointer_t<const variant_alternative_t<I, variant<Types...>>> get_if(const variant<Types...>* pv) EA_NOEXCEPT
	{
		using return_type = add_pointer_t<variant_alternative_t<I, variant<Types...>>>;

		return (!pv || pv->index() != I) ? nullptr : pv->template get_as<return_type>();
	}

	template <class T, class... Types, size_t I = meta::get_type_index_v<T, Types...>, enable_if_t<I < sizeof...(Types) && meta::duplicate_type_check_v<T, Types...>, bool> = true>
	EA_CONSTEXPR add_pointer_t<T> get_if(variant<Types...>* pv) EA_NOEXCEPT
	{
		return get_if<I>(pv);
	}

	template <class T, class... Types, size_t I = meta::get_type_index_v<T, Types...>, enable_if_t<I < sizeof...(Types) && meta::duplicate_type_check_v<T, Types...>, bool> = true>
	EA_CONSTEXPR add_pointer_t<const T> get_if(const variant<Types...>* pv) EA_NOEXCEPT
	{
		return get_if<I>(pv);
	}


	///////////////////////////////////////////////////////////////////////////
	// get
	//
	template <size_t I, class... Types, enable_if_t<I < sizeof...(Types), bool> = true>
	EA_CONSTEXPR variant_alternative_t<I, variant<Types...>>& get(variant<Types...>& v)
	{
		using return_type = add_pointer_t<variant_alternative_t<I, variant<Types...>>>;

		if (v.index() != I)
			EASTL_THROW_OR_ASSERT(bad_variant_access, "get -- bad_variant_access");

		return *v.template get_as<return_type>();
	}

	template <size_t I, class... Types, enable_if_t<I < sizeof...(Types), bool> = true>
	EA_CONSTEXPR variant_alternative_t<I, variant<Types...>>&& get(variant<Types...>&& v)
	{
		using return_type = add_pointer_t<variant_alternative_t<I, variant<Types...>>>;

		if (v.index() != I)
			EASTL_THROW_OR_ASSERT(bad_variant_access, "get -- bad_variant_access");

		return eastl::move(*v.template get_as<return_type>());
	}

	template <size_t I, class... Types, enable_if_t<I < sizeof...(Types), bool> = true>
	EA_CONSTEXPR const variant_alternative_t<I, variant<Types...>>& get(const variant<Types...>& v)
	{
		using return_type = add_pointer_t<variant_alternative_t<I, variant<Types...>>>;

		if (v.index() != I)
			EASTL_THROW_OR_ASSERT(bad_variant_access, "get -- bad_variant_access");

		return *v.template get_as<return_type>();
	}

	template <size_t I, class... Types, enable_if_t<I < sizeof...(Types), bool> = true>
	EA_CONSTEXPR const variant_alternative_t<I, variant<Types...>>&& get(const variant<Types...>&& v)
	{
		using return_type = add_pointer_t<variant_alternative_t<I, variant<Types...>>>;

		if (v.index() != I)
			EASTL_THROW_OR_ASSERT(bad_variant_access, "get -- bad_variant_access");

		return eastl::move(*v.template get_as<return_type>());
	}

	template <class T, class... Types, size_t I = meta::get_type_index_v<T, Types...>, enable_if_t<I < sizeof...(Types) && meta::duplicate_type_check_v<T, Types...>, bool> = true>
	EA_CONSTEXPR T& get(variant<Types...>& v)
	{
		return get<I>(v);
	}

	template <class T, class... Types, size_t I = meta::get_type_index_v<T, Types...>, enable_if_t<I < sizeof...(Types) && meta::duplicate_type_check_v<T, Types...>, bool> = true>
	EA_CONSTEXPR T&& get(variant<Types...>&& v)
	{
		return get<I>(eastl::move(v));
	}

	template <class T, class... Types, size_t I = meta::get_type_index_v<T, Types...>, enable_if_t<I < sizeof...(Types) && meta::duplicate_type_check_v<T, Types...>, bool> = true>
	EA_CONSTEXPR const T& get(const variant<Types...>& v)
	{
		return get<I>(v);
	}

	template <class T, class... Types, size_t I = meta::get_type_index_v<T, Types...>, enable_if_t<I < sizeof...(Types) && meta::duplicate_type_check_v<T, Types...>, bool> = true>
	EA_CONSTEXPR const T&& get(const variant<Types...>&& v)
	{
		return get<I>(eastl::move(v));
	}


	///////////////////////////////////////////////////////////////////////////
	// 20.7.4, value access
	//
	template <class T, class... Types, size_t I = meta::get_type_index_v<T, Types...>, enable_if_t<I < sizeof...(Types) && meta::duplicate_type_check_v<T, Types...>, bool> = true>
	EA_CONSTEXPR bool holds_alternative(const variant<Types...>& v) EA_NOEXCEPT
	{
		return I == variant_npos ? false : (v.index() == I);
	}


	namespace internal {
	template<typename T, bool = eastl::is_nothrow_copy_constructible_v<T> || !eastl::is_nothrow_move_constructible_v<T>>
	struct emplace_or_move_assign
	{
		template<size_t I, typename... Types>
		static void call(variant<Types...>& obj, const T& rhs)
		{
			obj.template emplace<I>(rhs);
		}
	};

	template<typename T>
	struct emplace_or_move_assign<T, false>
	{
		template<size_t I, typename... Types>
		static void call(variant<Types...>& obj, const T& rhs)
		{
			obj = variant<Types...>(rhs);
		}
	};

	template<typename T_j, typename T, bool = eastl::is_nothrow_constructible_v<T_j, T> || !eastl::is_nothrow_move_constructible_v<T_j>>
	struct converting_emplace
	{
		template<size_t I, typename... Types>
		static void call(variant<Types...>& obj, T&& rhs)
		{
			obj.template emplace<I>(eastl::forward<T>(rhs));
		}
	};

	template<typename T_j, typename T>
	struct converting_emplace<T_j, T, false>
	{
		template<size_t I, typename... Types>
		static void call(variant<Types...>& obj, T&& rhs)
		{
			obj.template emplace<I>(T_j(eastl::forward<T>(rhs)));
		}
	};

	template <typename Visitor, typename Variant>
	EA_CPP14_CONSTEXPR decltype(auto) visit_with_index(Visitor&& visitor, Variant&& variant);

	///////////////////////////////////////////////////////////////////////////
	// variant_storage
	//
	// This is a utility class to simplify the implementation of a storage type
	// for a distriminted union.  This utility handles the alignment, size
	// requirements, and data access required by the variant type.
	//
	template<bool IsTriviallyDestructible, class... Types>
	struct variant_storage;

	// reinterpret_cast used between related classes: 'variant_storage' and 'variant'
	// MSVC incorrectly raises this warning - the warning can be surpressed by using a static_cast, but Clang correctly catches this as an error;
	// variant_storage is a *private* base class.
	EA_DISABLE_VC_WARNING(4946)

	// variant_storage
	//
	// specialization for non-trivially destructible types
	//
	template<class... Types>
	struct variant_storage<false, Types...>
	{
		using aligned_storage_impl_t = aligned_union_t<1, Types...>;

		size_t mIndex;
		aligned_storage_impl_t mBuffer;

		~variant_storage()
		{
			if (!this->valueless_by_exception())
				destroy();
		}

		// precondition:
		//	!this->valueless_by_exception()
		void destroy()
		{
			auto genericVisitor = [](auto& thisAlternative)
			{
				using alternative_t = remove_reference_t<decltype(thisAlternative)>;
				thisAlternative.~alternative_t();
			};

			visit(genericVisitor, reinterpret_cast<variant<Types...>&>(*this));
		}

		EA_CONSTEXPR size_t index() const EA_NOEXCEPT
		{
			return this->mIndex;
		}

		EA_CONSTEXPR bool valueless_by_exception() const EA_NOEXCEPT
		{
#if EASTL_EXCEPTIONS_ENABLED
			return this->mIndex == variant_npos;
#else
			return false;
#endif
		}
	};


	// variant_storage
	//
	// specialization for trivially destructible types
	//
	template<class... Types>
	struct variant_storage<true, Types...>
	{
		using aligned_storage_impl_t = aligned_union_t<1, Types...>;

		size_t mIndex;
		aligned_storage_impl_t mBuffer;

		void destroy()
		{
			// trivially destructible, no op.
		}

		EA_CONSTEXPR size_t index() const EA_NOEXCEPT
		{
			return this->mIndex;
		}

		EA_CONSTEXPR bool valueless_by_exception() const EA_NOEXCEPT
		{
#if EASTL_EXCEPTIONS_ENABLED
			return this->mIndex == variant_npos;
#else
			return false;
#endif
		}
	};

	template<class... Types>
	struct variant_base : protected variant_storage<conjunction_v<is_trivially_destructible<Types>...>, Types...>
	{
		using variant_storage_t = variant_storage<conjunction_v<is_trivially_destructible<Types>...>, Types...>;
		using aligned_storage_impl_t = typename variant_storage_t::aligned_storage_impl_t;

		EA_DISABLE_VC_WARNING(4702) // unreachable code: suppress warning because construct_as<I>() may always throw
			                        // (because alternative_type(args...) throws).
		template <size_t I, typename... Args>
		void construct_as(Args&&... args)
		{
			using alternative_type = variant_alternative_t<I, variant<Types...>>;

			// NOTE(rparolin): If this assert fires there is an EASTL problem picking the size of the local buffer which
			// variant_storage used to store types. The size selected should be large enough to hold the largest type in
			// the user provided variant type-list.
			static_assert(sizeof(aligned_storage_impl_t) >= sizeof(alternative_type), "alternative_type is larger than local buffer size");

			new (&this->mBuffer) alternative_type(eastl::forward<Args>(args)...);
			this->mIndex = I;
		}

		template <size_t I, typename U, typename... Args>
		void construct_as(std::initializer_list<U> il, Args&&... args)
		{
			using alternative_type = variant_alternative_t<I, variant<Types...>>;

			// NOTE(rparolin): If this assert fires there is an EASTL problem picking the size of the local buffer which
			// variant_storage used to store types. The size selected should be large enough to hold the largest type in
			// the user provided variant type-list.
			static_assert(sizeof(aligned_storage_impl_t) >= sizeof(alternative_type), "alternative_type is larger than local buffer size");

			new (&this->mBuffer) alternative_type(il, eastl::forward<Args>(args)...);
			this->mIndex = I;
		}
		EA_RESTORE_VC_WARNING()

		template<typename T>
		T get_as()
		{
			static_assert(eastl::is_pointer_v<T>, "T must be a pointer type");
			return reinterpret_cast<T>(&this->mBuffer);
		}

		template<typename T>
		const T get_as() const
		{
			static_assert(eastl::is_pointer_v<T>, "T must be a pointer type");
			return reinterpret_cast<const T>(reinterpret_cast<uintptr_t>(&this->mBuffer));
		}

#if !EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		variant_base() = default;
#endif

		// copy constructor
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		void ConstructFrom(const variant_base& rhs)
#else
		variant_base(const variant_base& rhs)
#endif
		{
			this->mIndex = rhs.mIndex;

			auto genericVisitor = [&rhs](auto& thisAlternative)
			{
				using alternative_t = remove_reference_t<decltype(thisAlternative)>;
				new (&thisAlternative) alternative_t(*rhs.template get_as<const alternative_t*>());
			};

			if(!rhs.valueless_by_exception())
				visit(genericVisitor, reinterpret_cast<variant<Types...>&>(*this));
		}

		// move constructor
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		EA_CONSTEXPR void ConstructFrom(variant_base&& rhs) EA_NOEXCEPT(conjunction_v<is_nothrow_move_constructible<Types>...>)
#else
		variant_base(variant_base&& rhs)
#endif
		{
			this->mIndex = rhs.mIndex;

			auto genericVisitor = [&rhs](auto& thisAlternative)
			{
				using alternative_t = remove_reference_t<decltype(thisAlternative)>;
				new (&thisAlternative) alternative_t(eastl::move(*rhs.template get_as<alternative_t*>()));
			};

			if (!rhs.valueless_by_exception())
				visit(genericVisitor, reinterpret_cast<variant<Types...>&>(*this));
		}

		// copy assignment
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		void AssignFrom(const variant_base& rhs)
			EA_NOEXCEPT(conjunction_v<conjunction<is_nothrow_copy_constructible<Types>...>,
				conjunction<is_nothrow_copy_assignable<Types>...>>)
#else
		variant_base& operator=(const variant_base& rhs)
#endif
		{
			if (this->valueless_by_exception() && rhs.valueless_by_exception())
			{
				// no op
			}
			else if (!this->valueless_by_exception() && rhs.valueless_by_exception())
			{
				this->destroy();
				this->mIndex = variant_npos;
			}
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
			else if (!this->valueless_by_exception() && this->mIndex == rhs.mIndex)
			{
				auto genericVisitor = [&rhs](auto& thisAlternative)
				{
					using alternative_t = remove_reference_t<decltype(thisAlternative)>;
					thisAlternative = *rhs.template get_as<alternative_t*>();
				};

				visit(genericVisitor, reinterpret_cast<variant<Types...>&>(*this));
			}
#endif
			else
			{
				auto genericVisitor = [this](auto idxConstant, const auto& rhsAlternative)
				{
					constexpr size_t I = idxConstant();
					using alternative_t = remove_cvref_t<decltype(rhsAlternative)>;
					emplace_or_move_assign<alternative_t>::template call<I>(reinterpret_cast<variant<Types...>&>(*this), rhsAlternative);
				};

				visit_with_index(genericVisitor, reinterpret_cast<const variant<Types...>&>(rhs));
			}

#if !EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
			return *this;
#endif
		}

		// move assignment
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		void AssignFrom(variant_base&& rhs)
			EA_NOEXCEPT(conjunction_v<conjunction<is_nothrow_move_constructible<Types>...>,
				conjunction<is_nothrow_move_assignable<Types>...>>)
#else
		variant_base& operator=(variant_base&& rhs)
#endif
		{
			if (this->valueless_by_exception() && rhs.valueless_by_exception())
			{
				// no op
			}
			else if (!this->valueless_by_exception() && rhs.valueless_by_exception())
			{
				this->destroy();
				this->mIndex = variant_npos;
			}
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
			else if (!this->valueless_by_exception() && this->mIndex == rhs.mIndex)
			{
				auto genericVisitor = [&rhs](auto& thisAlternative)
				{
					using alternative_t = remove_reference_t<decltype(thisAlternative)>;
					thisAlternative = eastl::move(*rhs.template get_as<alternative_t*>());
				};

				visit(genericVisitor, reinterpret_cast<variant<Types...>&>(*this));
			}
#endif
			else
			{
				auto genericVisitor = [this](auto idxConstant, auto& rhsAlternative)
				{
					constexpr size_t I = idxConstant();
					reinterpret_cast<variant<Types...>&>(*this).template emplace<I>(eastl::move(rhsAlternative));
				};

				visit_with_index(genericVisitor, reinterpret_cast<variant<Types...>&>(rhs));
			}

#if !EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
			return *this;
#endif
		}
	};

	EA_RESTORE_VC_WARNING() // 4946

	template<typename>
	constexpr bool is_in_place_type_specialization_v = false;

	template<typename T>
	constexpr bool is_in_place_type_specialization_v<in_place_type_t<T>> = true;

	template<typename>
	constexpr bool is_in_place_index_specialization_v = false;

	template<size_t Idx>
	constexpr bool is_in_place_index_specialization_v<in_place_index_t<Idx>> = true;

	// this could be a generic lambda, except that MSVC and Clang disagree with whether constants need to be in the capture list and MSVC (circa 2024) doesn't
	// recognize constants passed in the capture list as constant in the lambda body.
	template<typename LhsAlternative, size_t LhsAlternativeIndex, typename... Types>
	struct swap_visitor
	{
		variant<Types...>& lhs;
		variant<Types...>& rhs;
		LhsAlternative& lhsAlternative;

		template<typename IntConstant, typename RhsAlternative>
		void operator()(IntConstant rhsIdxConstant, RhsAlternative& rhsAlternative)
		{
			constexpr size_t RhsAlternativeIndex = rhsIdxConstant();

			LhsAlternative temp(eastl::move(lhsAlternative));
			lhs.template emplace<RhsAlternativeIndex>(eastl::move(rhsAlternative));
			rhs.template emplace<LhsAlternativeIndex>(eastl::move(temp));
		};
	};
	} // namespace internal


	///////////////////////////////////////////////////////////////////////////
	// 20.7.2, variant
	//
	template <class... Types>
	class variant
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
		: private internal::EnableVariantOptionalSpecialMemberFunctions<internal::variant_base<Types...>, Types...>
	{
		using base_type = internal::EnableVariantOptionalSpecialMemberFunctions<internal::variant_base<Types...>, Types...>;
#else
		: private internal::variant_base<Types...>
	{
		using base_type = internal::variant_base<Types...>;
#endif

		using T_0 = variant_alternative_t<0, variant>;  // alias for the 1st type in the variadic pack

		static_assert(sizeof...(Types) > 0,                           "variant must have at least 1 type (empty variants are ill-formed)");
		static_assert(disjunction_v<is_reference<Types>...> == false, "variant alternative types must be destructible (non-reference)");
		static_assert(disjunction_v<is_array<Types>...> == false,     "variant alternative types must be destructible (non-array)");
		static_assert(conjunction_v<is_destructible<Types>...>,		  "variant alternative types must be destructible");

		using variant_storage_t = typename base_type::variant_storage_t;

	public:

		///////////////////////////////////////////////////////////////////////////
		// 20.7.2.1, constructors
		//

		// Only participates in overload resolution when the first alternative is default constructible
		template <typename TT0 = T_0, enable_if_t<is_default_constructible_v<TT0>, bool> = true>
		EA_CONSTEXPR variant() EA_NOEXCEPT(eastl::is_nothrow_default_constructible_v<TT0>)
		{
			this->template construct_as<0>();
		}

		// implemented by EnableVariantOptionalSpecialMemberFunctions<...>
		variant(const variant&) = default;
		variant(variant&&) = default;

		// Conversion constructor
		template <typename T,
			typename T_j = meta::overload_resolution_t<T, meta::overload_set<Types...>>,
			size_t I = meta::get_type_index_v<remove_cvref_t<T_j>, Types...>,
			enable_if_t<(sizeof...(Types) > 0) &&
				I < sizeof...(Types) &&
				!is_same_v<remove_cvref_t<T>, variant> &&
				!internal::is_in_place_type_specialization_v<remove_cvref_t<T>> &&
				!internal::is_in_place_index_specialization_v<remove_cvref_t<T>> &&
				eastl::is_constructible_v<T_j, T>, bool> = true>
			EA_CONSTEXPR variant(T&& t) EA_NOEXCEPT(is_nothrow_constructible_v<T_j, T>)
		{
			this->template construct_as<I>(eastl::forward<T>(t));
		}

		///////////////////////////////////////////////////////////////////////////
		// 20.7.2.1, in_place_t constructors
		//
		template <
			class T,
			class... Args,
			enable_if_t<conjunction_v<meta::duplicate_type_check<T, Types...>, is_constructible<T, Args...>>, bool> = true>
			EA_CPP14_CONSTEXPR explicit variant(in_place_type_t<T>, Args&&... args)
			: variant(in_place_index<meta::get_type_index_v<T, Types...>>, eastl::forward<Args>(args)...)
		{}

		template <
			class T,
			class U,
			class... Args,
			enable_if_t<conjunction_v<meta::duplicate_type_check<T, Types...>, is_constructible<T, Args...>>, bool> = true>
			EA_CPP14_CONSTEXPR explicit variant(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
			: variant(in_place_index<meta::get_type_index_v<T, Types...>>, il, eastl::forward<Args>(args)...)
		{}

		template <size_t I,
			class... Args,
			enable_if_t<conjunction_v<integral_constant<bool, (I < sizeof...(Types))>,
			is_constructible<meta::get_type_at_t<I, Types...>, Args...>>, bool> = true>
			EA_CPP14_CONSTEXPR explicit variant(in_place_index_t<I>, Args&&... args)
		{
			this->template construct_as<I>(eastl::forward<Args>(args)...);
		}

		template <size_t I,
			class U,
			class... Args,
			enable_if_t<conjunction_v<integral_constant<bool, (I < sizeof...(Types))>,
			is_constructible<meta::get_type_at_t<I, Types...>, Args...>>, bool> = true>
			EA_CPP14_CONSTEXPR explicit variant(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
		{
			this->template construct_as<I>(il, eastl::forward<Args>(args)...);
		}

		///////////////////////////////////////////////////////////////////////////
		// 20.7.2.2, destructor
		//
		// implemented by EnableVariantOptionalSpecialMemberFunctions<...>
		~variant() = default;


		///////////////////////////////////////////////////////////////////////////
		// 20.7.2.4, modifiers
		//

		// Equivalent to emplace<I>(std::forward<Args>(args)...), where I is the zero-based index of T in Types....
		// This overload only participates in overload resolution if std::is_constructible_v<T, Args...> is true, and T
		// occurs exactly once in Types...
		template <
		    class T,
		    class... Args,
		    size_t I = meta::get_type_index_v<T, Types...>,
		    enable_if_t<conjunction_v<is_constructible<T, Args...>, meta::duplicate_type_check<T, Types...>>, bool> = true>
		EA_CPP20_CONSTEXPR decltype(auto) emplace(Args&&... args)
		{
			return emplace<I>(eastl::forward<Args>(args)...);
		}

		// Equivalent to emplace<I>(il, std::forward<Args>(args)...), where I is the zero-based index of T in Types....
		// This overload only participates in overload resolution if std::is_constructible_v<T,
		// std::initializer_list<U>&, Args...> is true, and T occurs exactly once in Types...
		template <class T,
		          class U,
		          class... Args,
		          size_t I = meta::get_type_index_v<T, Types...>,
		          enable_if_t<conjunction_v<is_constructible<T, std::initializer_list<U>&, Args...>,
		                                               meta::duplicate_type_check<T, Types...>>, bool> = true>
		EA_CPP20_CONSTEXPR decltype(auto) emplace(std::initializer_list<U> il, Args&&... args)
		{
			return emplace<I>(il, eastl::forward<T>(args)...);
		}

		// First, destroys the currently contained value (if any). Then direct-initializes the contained value as if
		// constructing a value of type T_I with the arguments std::forward<Args>(args).... If an exception is thrown,
		// *this may become valueless_by_exception. This overload only participates in overload resolution if
		// std::is_constructible_v<T_I, Args...> is true. The behavior is undefined if I is not less than
		// sizeof...(Types).
		//
		EA_DISABLE_VC_WARNING(4702) // unreachable code: suppress warning because construct_as<I>() may always throws (because T(args...) throws).
		template <size_t I,
		          class... Args,
		          typename T = meta::get_type_at_t<I, Types...>,
		          enable_if_t<is_constructible_v<T, Args...>, bool> = true>
		EA_CPP20_CONSTEXPR variant_alternative_t<I, variant>& emplace(Args&&... args)
		{
			if (!valueless_by_exception())
			{
				this->destroy();

				#if EASTL_EXCEPTIONS_ENABLED
					this->mIndex = static_cast<size_t>(variant_npos);
				#endif
			}

			this->template construct_as<I>(eastl::forward<Args>(args)...);
			return *reinterpret_cast<T*>(&this->mBuffer);
		}
		EA_RESTORE_VC_WARNING()

		// First, destroys the currently contained value (if any). Then direct-initializes the contained value as if
		// constructing a value of type T_I with the arguments il, std::forward<Args>(args).... If an exception is
		// thrown, *this may become valueless_by_exception. This overload only participates in overload resolution if
		// std::is_constructible_v<T_I, initializer_list<U>&, Args...> is true. The behavior is undefined if I is not
		// less than sizeof...(Types).
		//
		EA_DISABLE_VC_WARNING(4702) // unreachable code: suppress warning because construct_as<I>() may always throws (because T(args...) throws).
		template <size_t I,
		          class U,
		          class... Args,
		          typename T = meta::get_type_at_t<I, Types...>,
		          enable_if_t<is_constructible_v<T, std::initializer_list<U>&, Args...>, bool> = true>
		EA_CPP20_CONSTEXPR variant_alternative_t<I, variant>& emplace(std::initializer_list<U> il, Args&&... args)
		{
			if (!valueless_by_exception())
			{
				this->destroy();

				#if EASTL_EXCEPTIONS_ENABLED
					this->mIndex = static_cast<size_t>(variant_npos);
				#endif
			}

			this->template construct_as<I>(il, eastl::forward<Args>(args)...);
			return *reinterpret_cast<T*>(&this->mBuffer);
		}
		EA_RESTORE_VC_WARNING()


		///////////////////////////////////////////////////////////////////////////
		// 20.7.2.3, assignment
		//

		// implemented by EnableVariantOptionalSpecialMemberFunctions<...>
		variant& operator=(const variant&) = default;
		variant& operator=(variant&&) = default;

		template <class T,
		          typename T_j = meta::overload_resolution_t<T, meta::overload_set<Types...>>,
		          size_t I = meta::get_type_index_v<decay_t<T_j>, Types...>,
		          enable_if_t<!eastl::is_same_v<eastl::remove_cvref_t<T>, variant> && eastl::is_assignable_v<T_j&, T> &&
		              eastl::is_constructible_v<T_j, T>, bool> = true>
		EA_CPP14_CONSTEXPR variant& operator=(T&& t)
		    EA_NOEXCEPT(conjunction_v<is_nothrow_assignable<T_j&, T>, is_nothrow_constructible<T_j, T>>)
		{
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
			if (this->mIndex == I) // already holds a T_j
			{
				*this->template get_as<T_j*>() = eastl::forward<T>(t);
			}
			else
			{
				internal::converting_emplace<T_j, T>::template call<I>(*this, eastl::forward<T>(t));
			}
#else
			if (!valueless_by_exception())
				this->destroy();

			this->template construct_as<I>(eastl::forward<T>(t));
#endif
			return *this;
		}


		///////////////////////////////////////////////////////////////////////////
		// 20.7.2.5, value status
		//
		using base_type::index;
		using base_type::valueless_by_exception;


		///////////////////////////////////////////////////////////////////////////
		// 20.7.2.6, swap
		//
		void swap(variant& other)
			EA_NOEXCEPT(conjunction_v<is_nothrow_move_constructible<Types>..., is_nothrow_swappable<Types>...>)
		{
			if (this->valueless_by_exception() && other.valueless_by_exception())
			{
				// no op
			}
#if EA_IS_ENABLED(EA_DEPRECATIONS_FOR_2025_APRIL)
			else if (this->mIndex == other.mIndex)
			{
				auto genericVisitor = [&other](auto& thisAlternative)
				{
					using alternative_t = remove_reference_t<decltype(thisAlternative)>;

					using eastl::swap;
					swap(thisAlternative, *other.template get_as<alternative_t*>());
				};

				visit(genericVisitor, *this);
			}
#endif
			else if (this->valueless_by_exception() && !other.valueless_by_exception())
			{
				auto genericVisitor = [this](auto idxConstant, auto&& otherAlternative)
				{
					constexpr size_t I = idxConstant();
					this->template construct_as<I>(eastl::move(otherAlternative));
				};

				internal::visit_with_index(genericVisitor, eastl::move(other));
				other.destroy(); // moved variant still contains an object.
				other.mIndex = variant_npos;
			}
			else if (!this->valueless_by_exception() && other.valueless_by_exception())
			{
				auto genericVisitor = [&other](auto idxConstant, auto&& thisAlternative)
				{
					constexpr size_t I = idxConstant();
					other.template construct_as<I>(eastl::move(thisAlternative));
				};

				internal::visit_with_index(genericVisitor, eastl::move(*this));
				this->destroy(); // moved variant still contains an object.
				this->mIndex = variant_npos;
			}
			else // if (!this->valueless_by_exception() && !other.valueless_by_exception() && this->mIndex != other.mIndex)
			{
				// the standard specifies swap() only requires that alternative types are swappable and move constructible.
				// therefore we can't use the assignment operator.

				auto thisVisitor = [this, &other](auto thisIdxConstant, auto& thisAlternative)
				{
					constexpr size_t ThisAlternativeIndex = thisIdxConstant();
					using this_alternative_t = remove_reference_t<decltype(thisAlternative)>;

					internal::swap_visitor<this_alternative_t, ThisAlternativeIndex, Types...> swapVisitor{ *this, other, thisAlternative };
					internal::visit_with_index(swapVisitor, other);
				};

				internal::visit_with_index(thisVisitor, *this);
			}
		}

	private:
		// NOTE(rparolin): get_if accessors require internal access to the variant storage class
		template <size_t I, class... Types2, enable_if_t<I < sizeof...(Types2), bool>> friend EA_CONSTEXPR add_pointer_t<      variant_alternative_t<I, variant<Types2...>>> get_if(      variant<Types2...>* pv) EA_NOEXCEPT;
		template <size_t I, class... Types2, enable_if_t<I < sizeof...(Types2), bool>> friend EA_CONSTEXPR add_pointer_t<const variant_alternative_t<I, variant<Types2...>>> get_if(const variant<Types2...>* pv) EA_NOEXCEPT;

		// NOTE(rparolin): get accessors require internal access to the variant storage class
		template <size_t I, class... Types2, enable_if_t<I < sizeof...(Types2), bool>> friend EA_CONSTEXPR       variant_alternative_t<I, variant<Types2...>>&  get(variant<Types2...>& v);
		template <size_t I, class... Types2, enable_if_t<I < sizeof...(Types2), bool>> friend EA_CONSTEXPR       variant_alternative_t<I, variant<Types2...>>&& get(variant<Types2...>&& v);
		template <size_t I, class... Types2, enable_if_t<I < sizeof...(Types2), bool>> friend EA_CONSTEXPR const variant_alternative_t<I, variant<Types2...>>&  get(const variant<Types2...>& v);
		template <size_t I, class... Types2, enable_if_t<I < sizeof...(Types2), bool>> friend EA_CONSTEXPR const variant_alternative_t<I, variant<Types2...>>&& get(const variant<Types2...>&& v);
	};

	///////////////////////////////////////////////////////////////////////////
	// 20.7.9, swap
	//
	template <class... Types>
	void swap(variant<Types...>& lhs, variant<Types...>& rhs)
		EA_NOEXCEPT(EA_NOEXCEPT(lhs.swap(rhs)))
	{
		lhs.swap(rhs);
	}


	// visit is a bit convoluted, in order to fulfill a few requirements:
	//   - It must support visiting multiple variants using a single visitor and a single function call. The
	//     visitor in this case should have one function for each possible combination of types:
	//
	//     struct MyVisitor {
	//         void operator()(int, int);
	//         void operator()(string, string);
	//         void operator()(int, string);
	//         void operator()(string, int);
	//     };
	//
	//     variant<int, string> a = 42;
	//     variant<int, string> b = "hello";
	//     visit(MyVisitor{}, a, b); // calls MyVisitor::operator()(int, string)
	//
	//   - It must be declared constexpr
	//   - It must be constant-time for the case of visiting a single variant
	//
	//   - 20.7.7 states that variant visitation requires all combinations of visitors to return the same type.
	//
	// NOTE(mwinkler):
	// Visit creates an N-Dimensional matrix whereby each dimension is M wide.
	// Where N == sizeof...(Variants) and M == variant_size_v<Variant>
	//
	// variant<int, bool, float> v;
	// visit(Visitor{}, v, v);
	//
	// This creates a 3x3 matrix of potential visitors.
	// The argument indices into the variants are as follows.
	// [0, 0], [0, 1], [0, 2]
	// [1, 0], [1, 1], [1, 2]
	// [2, 0], [2, 1], [2, 2]
	//
	// These indices are compile-time constants but the variants have a runtime index.
	// Therefore we must instantiate an NxNxN... matrix of function pointers who are
	// templated on the indices based on their position in the matrix and then
	// at runtime index into the array to call the correct function pointer that can
	// get the correct alternatives in the variants.
	//
	// There are a couple of ways to do this. We can construct the matrix bottom up or top down.
	//
	// Constructing a matrix bottom up would look something as follows.
	//
	// make_visitor_matrix_recurse(eastl::index_sequence<>{}, eastl::make_index_sequence<eastl::variant_size_v<eastl::decay_t<Variants>>>{}...);
	//
	// make_visitor_matrix_recurse(eastl::index_sequence<Is...>) { return templated function pointer on Is... }
	//
	// make_visitor_matrix_recurse(eastl::index_sequence<Is...>, eastl::index_sequence<Js...>, RestIndex... rest)
	//    return make_array(make_visitor_matrix_recurse(eastl::index_sequence<Is..., Js>{}, rest...)...);
	//
	// Essentially we construct the matrix bottom up, row by row of indices and return an array of function pointers.
	// The end result is a NxNxN... array on the stack which can be indexed by each variant in order as follows,
	// array[v0.index()][v1.index()][vn.index()]();
	//
	// The downside with this approach is the massive NxNxN... array that is created on the stack.
	//
	// The other approach is to build the matrix top down and use tail recursion to ensure there is only one
	// N sized array on the stack. The downside here is the extra function calls, but we feel this approach provides
	// a good balance between performance and memory usage.
	//
	// We construct the matrix top down by first creating an N sized array that is indexed by the first variant.
	// This calls a function that recursively creates another N sized array that is indexed by the second variant.
	// The recursion continues until we reach the base case which is the last variant. At this point we know
	// the compile-time value of the N indices needed to get each alternative from each variant to invoke the visitor upon.
	// Essentially we create a tree of function pointers like so.
	//
	//
	//						  +------------------------------------------------------------------+
	//						  |																	 |
	//						  |	   0						   1							 N	 |
	//						  |																	 |
	//						  |																	 |
	//						  +----+---------------------------+---------------------------------+
	//							   |						   |
	//							   |						   |
	//							   |						   |
	//							   |						   |
	//							   |						   |
	//	+--------------------------+-----------------+	  +----+------------------------------------+
	//	|											 |	  |											|
	//	|0,0				  0,1				  0,N|	  |1,0				   1,1				 1,N|
	//	|											 |	  |											|
	//	|											 |	  |											|
	//	+--------------------------------------------+	  +-----------------------------------------+
	//
	// Essentially each call creates a N sized array of function pointers that is the concatention of the indices known so far
	// and the index of itself in the array whereby the leaf function pointer does the final invoke of the visitor.
	//

	// Since decltype() is not one of the contexts where an overloaded function can be used without arguments;
	// We use this function to deduce the function pointer types.
	// We also return an eastl::array<> since we cannot return C-style arrays as value types.
	template <typename T>
	static EA_CONSTEXPR array<decay_t<T>, 1> make_visitor_array(T&& t)
	{
		return { { eastl::forward<T>(t) } };
	}

	template <typename T, typename... Ts>
	static EA_CONSTEXPR array<decay_t<T>, sizeof...(Ts) + 1> make_visitor_array(T&& t, Ts&&... ts)
	{
		static_assert(conjunction_v<is_same<decay_t<T>, decay_t<Ts>>...>, "`visit` variant visitation requires that all visitors have the same return type!");

		return { { eastl::forward<T>(t), eastl::forward<Ts>(ts)... } };
	}


	template <size_t N, typename Variant, typename... Variants, eastl::enable_if_t<N == 0, int> = 0>
	static EA_CONSTEXPR decltype(auto) get_variant_n(Variant&& variant, Variants&&...)
	{
		return eastl::forward<Variant>(variant);
	}

	template <size_t N, typename Variant, typename... Variants, eastl::enable_if_t<N != 0, int> = 0>
	static EA_CONSTEXPR decltype(auto) get_variant_n(Variant&&, Variants&&... variants)
	{
		return get_variant_n<N - 1>(eastl::forward<Variants>(variants)...);
	}


	template <typename Visitor, typename Index, typename Array, typename... Variants>
	static EA_CONSTEXPR decltype(auto) call_visitor_at_index(Array&& array, Index index, Visitor&& visitor, Variants&&... variants)
	{
		return array[static_cast<typename Array::size_type>(index)](eastl::forward<Visitor>(visitor), eastl::forward<Variants>(variants)...);
	}

	template <size_t VariantsIndex, typename Visitor, typename Array, typename... Variants>
	static EA_CONSTEXPR decltype(auto) call_visitor_at(Array&& array, Visitor&& visitor, Variants&&... variants)
	{
		return call_visitor_at_index(eastl::forward<Array>(array),
									 get_variant_n<VariantsIndex>(eastl::forward<Variants>(variants)...).index(),
									 eastl::forward<Visitor>(visitor),
									 eastl::forward<Variants>(variants)...);
	}


	// abstracts calling visit on 2 or more variants
	template <typename VariantIndexSequence, typename Visitor, typename... Variants>
	struct visitor_caller_n;

	template <typename Visitor, typename... Variants, size_t... VariantIndices>
	struct visitor_caller_n<index_sequence<VariantIndices...>, Visitor, Variants...>
	{
		using return_type = invoke_result_t<Visitor, variant_alternative_t<0, remove_reference_t<Variants>>...>;

		template <size_t... VariantArgIndices>
		static EA_CONSTEXPR return_type invoke_visitor_leaf(Visitor&& visitor, Variants&&... variants)
		{
			return eastl::invoke(eastl::forward<Visitor>(visitor),
								 eastl::get<VariantArgIndices>(eastl::forward<Variants>(variants))...);
		}

		template <size_t... VariantArgIndices>
		static EA_CONSTEXPR auto make_invoke_visitor_leaf(index_sequence<VariantArgIndices...>)
		{
			return &invoke_visitor_leaf<VariantArgIndices...>;
		}


		template <size_t... VariantArgIndices>
		static EA_CONSTEXPR return_type invoke_visitor_recurse(Visitor&& visitor, Variants&&... variants)
		{
			return call(index_sequence<VariantArgIndices...>{},
						eastl::forward<Visitor>(visitor),
						eastl::forward<Variants>(variants)...);
		}

		template <size_t... VariantArgIndices>
		static EA_CONSTEXPR auto make_invoke_visitor_recurse(index_sequence<VariantArgIndices...>)
		{
			return &invoke_visitor_recurse<VariantArgIndices...>;
		}


		template <typename VariantArgIndexSequence, enable_if_t<internal::index_sequence_size_v<VariantArgIndexSequence> + 1 == sizeof...(Variants), int> = 0>
		static EA_CPP14_CONSTEXPR decltype(auto) call(VariantArgIndexSequence, Visitor&& visitor, Variants&&... variants)
		{
			EA_CPP14_CONSTEXPR auto callers = make_visitor_array(make_invoke_visitor_leaf(meta::double_pack_expansion_t<VariantArgIndexSequence, VariantIndices>{})...);

			return call_visitor_at<internal::index_sequence_size_v<VariantArgIndexSequence>>(eastl::move(callers),
																							 eastl::forward<Visitor>(visitor),
																							 eastl::forward<Variants>(variants)...);
		}

		template <typename VariantArgIndexSequence, enable_if_t<internal::index_sequence_size_v<VariantArgIndexSequence> + 1 != sizeof...(Variants), int> = 0>
		static EA_CPP14_CONSTEXPR decltype(auto) call(VariantArgIndexSequence, Visitor&& visitor, Variants&&... variants)
		{
			EA_CPP14_CONSTEXPR auto callers = make_visitor_array(make_invoke_visitor_recurse(meta::double_pack_expansion_t<VariantArgIndexSequence, VariantIndices>{})...);

			return call_visitor_at<internal::index_sequence_size_v<VariantArgIndexSequence>>(eastl::move(callers),
																							 eastl::forward<Visitor>(visitor),
																							 eastl::forward<Variants>(variants)...);
		}

	};

	template <typename VariantIndexSequence, typename Visitor, typename... Variants>
	static EA_CONSTEXPR decltype(auto) call_initial_n(VariantIndexSequence, Visitor&& visitor, Variants&&... variants)
	{
		return visitor_caller_n<VariantIndexSequence, Visitor, Variants...>::call(index_sequence<>{}, eastl::forward<Visitor>(visitor), eastl::forward<Variants>(variants)...);
	}


	// abstracts calling visit on 2 or more variants with return types convertible to R
	template <typename R, typename VariantIndexSequence, typename Visitor, typename... Variants>
	struct visitor_caller_n_r;

	template <typename R, size_t... VariantIndices, typename Visitor, typename... Variants>
	struct visitor_caller_n_r<R, index_sequence<VariantIndices...>, Visitor, Variants...>
	{
		template <typename R_, size_t... VariantArgIndices>
		struct visitor_leaf_r
		{
			static EA_CONSTEXPR R_ invoke_visitor_leaf_r(Visitor&& visitor, Variants&&... variants)
			{
				return eastl::invoke(eastl::forward<Visitor>(visitor),
									 eastl::get<VariantArgIndices>(eastl::forward<Variants>(variants))...);
			}
		};

		// void return type must discard the return values of the visitor even if the visitor returns a value.
		template <size_t... VariantArgIndices>
		struct visitor_leaf_r<void, VariantArgIndices...>
		{
			static EA_CONSTEXPR void invoke_visitor_leaf_r(Visitor&& visitor, Variants&&... variants)
			{
				eastl::invoke(eastl::forward<Visitor>(visitor),
							  eastl::get<VariantArgIndices>(eastl::forward<Variants>(variants))...);
			}
		};
		template <size_t... VariantArgIndices> struct visitor_leaf_r<const void, VariantArgIndices...> : public visitor_leaf_r<void, VariantArgIndices...> {};
		template <size_t... VariantArgIndices> struct visitor_leaf_r<volatile void, VariantArgIndices...> : public visitor_leaf_r<void, VariantArgIndices...> {};
		template <size_t... VariantArgIndices> struct visitor_leaf_r<const volatile void, VariantArgIndices...> : public visitor_leaf_r<void, VariantArgIndices...> {};

		template <typename R_, size_t... VariantArgIndices>
		static EA_CONSTEXPR auto make_invoke_visitor_leaf_r(index_sequence<VariantArgIndices...>)
		{
			return &visitor_leaf_r<R_, VariantArgIndices...>::invoke_visitor_leaf_r;
		}


		template <typename R_, size_t... VariantArgIndices>
		struct visitor_recurse_r
		{
			static EA_CONSTEXPR R_ invoke_visitor_recurse_r(Visitor&& visitor, Variants&&... variants)
			{
				return call_r(index_sequence<VariantArgIndices...>{},
							  eastl::forward<Visitor>(visitor),
							  eastl::forward<Variants>(variants)...);
			}
		};

		template <typename R_, size_t... VariantArgIndices>
		static EA_CONSTEXPR auto make_invoke_visitor_recurse_r(index_sequence<VariantArgIndices...>)
		{
			return &visitor_recurse_r<R_, VariantArgIndices...>::invoke_visitor_recurse_r;
		}


		template <typename VariantArgIndexSequence, enable_if_t<internal::index_sequence_size_v<VariantArgIndexSequence> + 1 == sizeof...(Variants), int> = 0>
		static EA_CPP14_CONSTEXPR decltype(auto) call_r(VariantArgIndexSequence, Visitor&& visitor, Variants&&... variants)
		{
			EA_CPP14_CONSTEXPR auto callers = make_visitor_array(make_invoke_visitor_leaf_r<R>(meta::double_pack_expansion_t<VariantArgIndexSequence, VariantIndices>{})...);

			return call_visitor_at<internal::index_sequence_size_v<VariantArgIndexSequence>>(eastl::move(callers),
																							 eastl::forward<Visitor>(visitor),
																							 eastl::forward<Variants>(variants)...);
		}

		template <typename VariantArgIndexSequence, enable_if_t<internal::index_sequence_size_v<VariantArgIndexSequence> + 1 != sizeof...(Variants), int> = 0>
		static EA_CPP14_CONSTEXPR decltype(auto) call_r(VariantArgIndexSequence, Visitor&& visitor, Variants&&... variants)
		{
			EA_CPP14_CONSTEXPR auto callers = make_visitor_array(make_invoke_visitor_recurse_r<R>(meta::double_pack_expansion_t<VariantArgIndexSequence, VariantIndices>{})...);

			return call_visitor_at<internal::index_sequence_size_v<VariantArgIndexSequence>>(eastl::move(callers),
																							 eastl::forward<Visitor>(visitor),
																							 eastl::forward<Variants>(variants)...);
		}

	};

	template <typename R, typename VariantIndexSequence, typename Visitor, typename... Variants>
	static EA_CONSTEXPR decltype(auto) call_initial_n_r(VariantIndexSequence, Visitor&& visitor, Variants&&... variants)
	{
		return visitor_caller_n_r<R, VariantIndexSequence, Visitor, Variants...>::call_r(index_sequence<>{}, eastl::forward<Visitor>(visitor), eastl::forward<Variants>(variants)...);
	}


	// abstracts calling visit on a single variant
	struct visitor_caller_one
	{

		template <typename Visitor, typename Variant, size_t I>
		static EA_CONSTEXPR decltype(auto) invoke_visitor(Visitor&& visitor, Variant&& variant)
		{
			return eastl::invoke(eastl::forward<Visitor>(visitor),
								 eastl::get<I>(eastl::forward<Variant>(variant)));
		}

		template <typename Visitor, typename Variant, size_t... VariantArgIndices>
		static EA_CPP14_CONSTEXPR decltype(auto) call_index(Visitor&& visitor, Variant&& variant, index_sequence<VariantArgIndices...>)
		{
			EA_CPP14_CONSTEXPR auto callers = make_visitor_array((&invoke_visitor<Visitor, Variant, VariantArgIndices>)...);

			return call_visitor_at_index(eastl::move(callers), eastl::forward<Variant>(variant).index(),
										 eastl::forward<Visitor>(visitor), eastl::forward<Variant>(variant));
		}

		template <typename Visitor, typename Variant>
		static EA_CONSTEXPR decltype(auto) call(Visitor&& visitor, Variant&& variant)
		{
			return call_index(eastl::forward<Visitor>(visitor),
							  eastl::forward<Variant>(variant),
							  make_index_sequence<variant_size_v<decay_t<Variant>>>{});
		}

	};


	template <typename R>
	struct visitor_r
	{
		template <typename Visitor, typename Variant, size_t I>
		static EA_CONSTEXPR R invoke_visitor_r(Visitor&& visitor, Variant&& variant)
		{
			return eastl::invoke(eastl::forward<Visitor>(visitor),
								 eastl::get<I>(eastl::forward<Variant>(variant)));
		}
	};

	// void return type must discard the return values of the visitor even if the visitor returns a value.
	template <>
	struct visitor_r<void>
	{
		template <typename Visitor, typename Variant, size_t I>
		static EA_CONSTEXPR void invoke_visitor_r(Visitor&& visitor, Variant&& variant)
		{
			eastl::invoke(eastl::forward<Visitor>(visitor),
						  eastl::get<I>(eastl::forward<Variant>(variant)));
		}
	};
	template<> struct visitor_r<const void> : public visitor_r<void> {};
	template<> struct visitor_r<volatile void> : public visitor_r<void> {};
	template<> struct visitor_r<const volatile void> : public visitor_r<void> {};

	// abstracts calling visit on a single variant with return types convertible to R
	struct visitor_caller_one_r
	{
		template <typename R, typename Visitor, typename Variant, size_t... VariantArgIndices>
		static EA_CPP14_CONSTEXPR decltype(auto) call_index_r(Visitor&& visitor, Variant&& variant, eastl::index_sequence<VariantArgIndices...>)
		{
			EA_CPP14_CONSTEXPR auto callers = make_visitor_array(&visitor_r<R>::template invoke_visitor_r<Visitor, Variant, VariantArgIndices>...);

			return callers[static_cast<typename decltype(callers)::size_type>(eastl::forward<Variant>(variant).index())](eastl::forward<Visitor>(visitor),
																														 eastl::forward<Variant>(variant));
		}

		template <typename R, typename Visitor, typename Variant>
		static EA_CONSTEXPR decltype(auto) call_r(Visitor&& visitor, Variant&& variant)
		{
			return call_index_r<R>(eastl::forward<Visitor>(visitor), eastl::forward<Variant>(variant), eastl::make_index_sequence<eastl::variant_size_v<eastl::decay_t<Variant>>>());
		}

	};


	///////////////////////////////////////////////////////////////////////////
	// 20.7.6, visitation
	//
	// Example:
	//     struct MyVisitor
	//     {
	//         auto operator()(int) {};
	//         auto operator()(long) {};
	//         auto operator()(string) {};
	//     };
	//
	//     variant<int, long, string> v = "Hello, Variant";
	//     visit(MyVisitor{}, v);  // calls MyVisitor::operator()(string) {}
	//
	EA_DISABLE_VC_WARNING(4100) // warning C4100: 't': unreferenced formal parameter
	template <typename... Variants>
	static EA_CPP14_CONSTEXPR void visit_throw_bad_variant_access(Variants&&... variants)
	{
	#if EASTL_EXCEPTIONS_ENABLED
		using bool_array_type = bool[];
		bool badAccess = false;

		(void)bool_array_type{ (badAccess |= eastl::forward<Variants>(variants).valueless_by_exception(), false)... };

		if (badAccess)
		{
			throw bad_variant_access();
		}
	#endif
	}
	EA_RESTORE_VC_WARNING()

	template <typename... Variants>
	static EA_CONSTEXPR void visit_static_assert_check(Variants&&...)
	{
		static_assert(sizeof...(Variants) > 0, "`visit` at least one variant instance must be passed as an argument to the visit function");

		using variant_type = decay_t<meta::get_type_at_t<0, Variants...>>;
		static_assert(conjunction_v<is_same<variant_type, decay_t<Variants>>...>,
					  "`visit` all variants passed to eastl::visit() must have the same type");
	}

	namespace internal
	{
		// visit() overloads participate in overload resolution only if the visitor is callable with every combination of the variants' alternative types.
		// informally, for a call to visit(visitor, variant1, variant2, ...):
		//   visitor(Variant1Alt, Variant2Alt...) must be valid for every possible combination of variant alternatives, where Variant1Alt is an alternative type for Variant1, etc.
		// see https://en.cppreference.com/w/cpp/utility/variant/visit for details.
		template<typename ... Ts> variant<Ts...>& as_variant_impl(variant<Ts...>&);
		template<typename ... Ts> const variant<Ts...>& as_variant_impl(const variant<Ts...>&);
		template<typename ... Ts> variant<Ts...>&& as_variant_impl(variant<Ts...>&&);
		template<typename ... Ts> const variant<Ts...>&& as_variant_impl(const variant<Ts...>&&);
		template<typename T> using as_variant = decltype(as_variant_impl(declval<T>()));

		struct visitor_caller_one_with_index
		{
			template <typename Visitor, typename Variant, size_t I>
			static EA_CONSTEXPR decltype(auto) invoke_visitor(Visitor&& visitor, Variant&& variant)
			{
				return eastl::invoke(eastl::forward<Visitor>(visitor),
					eastl::integral_constant<size_t, I>{},
					eastl::get<I>(eastl::forward<Variant>(variant)));
			}

			template <typename Visitor, typename Variant, size_t... VariantArgIndices>
			static EA_CPP14_CONSTEXPR decltype(auto) call_index(Visitor&& visitor, Variant&& variant, index_sequence<VariantArgIndices...>)
			{
				EA_CPP14_CONSTEXPR auto callers = make_visitor_array((&invoke_visitor<Visitor, Variant, VariantArgIndices>)...);

				return call_visitor_at_index(eastl::move(callers), eastl::forward<Variant>(variant).index(),
					eastl::forward<Visitor>(visitor), eastl::forward<Variant>(variant));
			}

			template <typename Visitor, typename Variant>
			static EA_CONSTEXPR decltype(auto) call(Visitor&& visitor, Variant&& variant)
			{
				return call_index(eastl::forward<Visitor>(visitor),
					eastl::forward<Variant>(variant),
					make_index_sequence<variant_size_v<decay_t<Variant>>>{});
			}

		};

		// internal only, so asserts & validation are omitted.
		template <typename Visitor, typename Variant>
		EA_CPP14_CONSTEXPR decltype(auto) visit_with_index(Visitor&& visitor, Variant&& variant)
		{
			return visitor_caller_one_with_index::call(eastl::forward<Visitor>(visitor), eastl::forward<Variant>(variant));
		}
	} // namespace internal

	// visit
	//
	template <typename Visitor, typename Variant, typename = eastl::void_t<internal::as_variant<Variant>>>
	EA_CPP14_CONSTEXPR decltype(auto) visit(Visitor&& visitor, Variant&& variant)
	{
		visit_static_assert_check(eastl::forward<Variant>(variant));

		visit_throw_bad_variant_access(eastl::forward<Variant>(variant));

		return visitor_caller_one::call(eastl::forward<Visitor>(visitor),
										eastl::forward<Variant>(variant));
	}

	template <typename Visitor, typename... Variants, typename = eastl::void_t<internal::as_variant<Variants>...>>
	EA_CPP14_CONSTEXPR decltype(auto) visit(Visitor&& visitor, Variants&&... variants)
	{
		visit_static_assert_check(eastl::forward<Variants>(variants)...);

		visit_throw_bad_variant_access(eastl::forward<Variants>(variants)...);

		return call_initial_n(make_index_sequence<variant_size_v<decay_t<meta::get_type_at_t<0, Variants...>>>>{},
							  eastl::forward<Visitor>(visitor),
							  eastl::forward<Variants>(variants)...);

	}

	// C++20: overload with a user specifiable return type
	template <typename R, typename Visitor, typename Variant, typename = eastl::void_t<internal::as_variant<Variant>>>
	EA_CPP14_CONSTEXPR R visit(Visitor&& visitor, Variant&& variant)
	{
		visit_static_assert_check(eastl::forward<Variant>(variant));

		visit_throw_bad_variant_access(eastl::forward<Variant>(variant));

		return visitor_caller_one_r::call_r<R>(eastl::forward<Visitor>(visitor),
											   eastl::forward<Variant>(variant));
	}

	// C++20: overload with a user specifiable return type
	template <typename R, typename Visitor, typename... Variants, typename = eastl::void_t<internal::as_variant<Variants>...>>
	EA_CPP14_CONSTEXPR R visit(Visitor&& visitor, Variants&&... variants)
	{
		visit_static_assert_check(eastl::forward<Variants>(variants)...);

		visit_throw_bad_variant_access(eastl::forward<Variants>(variants)...);

		return call_initial_n_r<R>(make_index_sequence<variant_size_v<decay_t<meta::get_type_at_t<0, Variants...>>>>{},
								   eastl::forward<Visitor>(visitor),
								   eastl::forward<Variants>(variants)...);
	}


	///////////////////////////////////////////////////////////////////////////
	// 20.7.5, relational operators
	//
	namespace internal
	{

		// For relational operators we do not need to create the NxN matrix of comparisons since we know already
		// that both the lhs and rhs variants have the same index. We just need to compare the value of the types at that
		// index for equality. Therefore the visitation is simpler than visit() for relational operators.
		//
		struct variant_relational_comparison
		{
			template <typename Compare, size_t I, typename Variant>
			static EA_CONSTEXPR bool invoke_relational_visitor(const Variant& lhs, const Variant& rhs)
			{
				return eastl::invoke(Compare{}, eastl::get<I>(lhs), eastl::get<I>(rhs));
			}

			template <typename Compare, typename Variant, size_t... VariantArgIndices>
			static EA_CPP14_CONSTEXPR bool call_index(const Variant& lhs, const Variant& rhs, eastl::index_sequence<VariantArgIndices...>)
			{
				using invoke_relational_visitor_func_ptr = bool (*)(const Variant&, const Variant&);

				EA_CPP14_CONSTEXPR invoke_relational_visitor_func_ptr visitors[] = { static_cast<invoke_relational_visitor_func_ptr>(&invoke_relational_visitor<Compare, VariantArgIndices, Variant>)... };

				return visitors[lhs.index()](lhs, rhs);
			}

			template <typename Compare, typename Variant>
			static EA_CONSTEXPR bool call(const Variant& lhs, const Variant& rhs)
			{
				return call_index<Compare>(lhs, rhs, eastl::make_index_sequence<eastl::variant_size_v<eastl::decay_t<Variant>>>());
			}

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
			template <typename Compare, size_t I, typename Variant>
			static EA_CONSTEXPR std::compare_three_way_result_t<Variant> invoke_relational_visitor_three_way(const Variant& lhs, const Variant& rhs)
			{
				return eastl::invoke(Compare{}, eastl::get<I>(lhs), eastl::get<I>(rhs));
			}

			template <typename Compare, typename Variant, size_t... VariantArgIndices>
			static EA_CONSTEXPR std::compare_three_way_result_t<Variant> call_index_three_way(const Variant& lhs, const Variant& rhs, eastl::index_sequence<VariantArgIndices...>)
			{
				using invoke_relational_visitor_func_ptr = std::compare_three_way_result_t<Variant> (*)(const Variant&, const Variant&);

				EA_CONSTEXPR invoke_relational_visitor_func_ptr visitors[] = {static_cast<invoke_relational_visitor_func_ptr>(&invoke_relational_visitor_three_way<Compare, VariantArgIndices, Variant>)...};

				return visitors[lhs.index()](lhs, rhs);
			}

			template <typename Compare, typename Variant>
			static EA_CONSTEXPR std::compare_three_way_result_t<Variant> call_three_way(const Variant& lhs, const Variant& rhs)
			{
				return call_index_three_way<Compare>(lhs, rhs, eastl::make_index_sequence<eastl::variant_size_v<eastl::decay_t<Variant>>>());
			}
#endif
		};

		template <typename Compare, typename Variant>
		static EA_CONSTEXPR bool CompareVariantRelational(const Variant& lhs, const Variant& rhs)
		{
			return variant_relational_comparison::call<Compare>(lhs, rhs);
		}

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
		template <typename Compare, typename Variant>
		static EA_CONSTEXPR std::compare_three_way_result_t<Variant> CompareVariantRelationalThreeWay(const Variant& lhs, const Variant& rhs)
		{
			return variant_relational_comparison::call_three_way<Compare>(lhs, rhs);
		}
#endif

	} // namespace internal


	///////////////////////////////////////////////////////////////////////////
	// 20.7.5, relational operators
	//
	template <class... Types>
	EA_CPP14_CONSTEXPR bool operator==(const variant<Types...>& lhs, const variant<Types...>& rhs)
	{
		if (lhs.index() != rhs.index()) return false;
		if (lhs.valueless_by_exception()) return true;

		return internal::CompareVariantRelational<eastl::equal_to<>>(lhs, rhs);
	}

	template <class... Types>
	EA_CPP14_CONSTEXPR bool operator!=(const variant<Types...>& lhs, const variant<Types...>& rhs)
	{
		if (lhs.index() != rhs.index()) return true;
		if (lhs.valueless_by_exception()) return false;

		return internal::CompareVariantRelational<eastl::not_equal_to<>>(lhs, rhs);
	}

	template <class... Types>
	EA_CPP14_CONSTEXPR bool operator<(const variant<Types...>& lhs, const variant<Types...>& rhs)
	{
		if (rhs.valueless_by_exception()) return false;
		if (lhs.valueless_by_exception()) return true;
		if (lhs.index() < rhs.index()) return true;
		if (lhs.index() > rhs.index()) return false;

		return internal::CompareVariantRelational<eastl::less<>>(lhs, rhs);
	}

	template <class... Types>
	EA_CPP14_CONSTEXPR bool operator>(const variant<Types...>& lhs, const variant<Types...>& rhs)
	{
		if (lhs.valueless_by_exception()) return false;
		if (rhs.valueless_by_exception()) return true;
		if (lhs.index() > rhs.index()) return true;
		if (lhs.index() < rhs.index()) return false;

		return internal::CompareVariantRelational<eastl::greater<>>(lhs, rhs);
	}

	template <class... Types>
	EA_CPP14_CONSTEXPR bool operator<=(const variant<Types...>& lhs, const variant<Types...>& rhs)
	{
		if (lhs.valueless_by_exception()) return true;
		if (rhs.valueless_by_exception()) return false;
		if (lhs.index() < rhs.index()) return true;
		if (lhs.index() > rhs.index()) return false;

		return internal::CompareVariantRelational<eastl::less_equal<>>(lhs, rhs);
	}

	template <class... Types>
	EA_CPP14_CONSTEXPR bool operator>=(const variant<Types...>& lhs, const variant<Types...>& rhs)
	{
		if (rhs.valueless_by_exception()) return true;
		if (lhs.valueless_by_exception()) return false;
		if (lhs.index() > rhs.index()) return true;
		if (lhs.index() < rhs.index()) return false;

		return internal::CompareVariantRelational<eastl::greater_equal<>>(lhs, rhs);
	}

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	template <class... Types> requires (std::three_way_comparable<Types> && ...)
	EA_CONSTEXPR std::common_comparison_category_t<std::compare_three_way_result_t<Types>...> operator<=>(const variant<Types...>& lhs, const variant<Types...>& rhs)
	{
		if (lhs.valueless_by_exception() && rhs.valueless_by_exception()) return std::strong_ordering::equal;
		if (lhs.valueless_by_exception()) return std::strong_ordering::less;
		if (rhs.valueless_by_exception()) return std::strong_ordering::greater;
		if (auto result = (lhs.index() <=> rhs.index()); result != 0) return result;

		return internal::CompareVariantRelationalThreeWay<std::compare_three_way>(lhs, rhs);

	}
#endif

} // namespace eastl

EA_RESTORE_VC_WARNING()

#endif // EASTL_VARIANT_H
