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
	namespace internal
	{
		///////////////////////////////////////////////////////////////////////////
		// default_construct_if_supported<T>
		//
		// Utility class to remove default constructor calls for types that
		// do not support default construction.
		//
		// We can remove these utilities when C++17 'constexpr if' is available.
		//
		template<typename T, bool = eastl::is_default_constructible_v<T>>
		struct default_construct_if_supported
		{
			static void call(T* pThis)
			{
				new (pThis) T();
			}
		};

		template<typename T>
		struct default_construct_if_supported<T, false>
		{
			static void call(T*) {} // intentionally blank
		};

		///////////////////////////////////////////////////////////////////////////
		// destroy_if_supported<T>
		//
		// Utility class to remove default constructor calls for types that
		// do not support default construction.
		//
		// We can remove these utilities when C++17 'constexpr if' is available.
		//
		template<typename T, bool = eastl::is_destructible_v<T>>
		struct destroy_if_supported
		{
			static void call(T* pThis)
			{
				pThis->~T();
			}
		};

		template<typename T>
		struct destroy_if_supported<T, false>
		{
			static void call(T*) {} // intentionally blank
		};

		///////////////////////////////////////////////////////////////////////////
		// copy_if_supported<T>
		//
		// Utility class to remove copy constructor calls for types that
		// do not support copying.
		//
		// We can remove these utilities when C++17 'constexpr if' is available.
		//
		template<typename T, bool = eastl::is_copy_constructible_v<T>>
		struct copy_if_supported
		{
			static void call(T* pThis, T* pOther)
			{
				new (pThis) T(*pOther);
			}
		};

		template<typename T>
		struct copy_if_supported<T, false>
		{
			static void call(T*, T*) {} // intentionally blank
		};

		///////////////////////////////////////////////////////////////////////////
		// move_if_supported<T>
		//
		// Utility class to remove move constructor calls for types that
		// do not support moves.
		//
		// We can remove these utilities when C++17 'constexpr if' is available.
		//
		template<typename T, bool = eastl::is_move_constructible_v<T>>
		struct move_if_supported
		{
			static void call(T* pThis, T* pOther)
			{
				new (pThis) T(eastl::move(*pOther));
			}
		};

		template<typename T>
		struct move_if_supported<T, false>
		{
			static void call(T*, T*) {} // intentionally blank
		};
	} // namespace internal


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
	// TODO(rparolin): JUST COPY/PASTE THIS CODE
	//
	inline void CheckVariantCondition(bool b)
	{
		EA_UNUSED(b);
	#if EASTL_EXCEPTIONS_ENABLED
		if (!b)
			throw bad_variant_access();
	#elif EASTL_ASSERT_ENABLED
		EASTL_ASSERT_MSG(b, "eastl::bad_variant_access assert");
	#else
		EA_UNUSED(b);
	#endif
	}


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
	// variant_storage
	//
	// This is a utility class to simplify the implementation of a storage type
	// for a distriminted union.  This utility handles the alignment, size
	// requirements, and data access required by the variant type.
	//
	template<bool IsTriviallyDestructible, class... Types>
	struct variant_storage;


	// variant_storage
	//
	// specialization for non-trivial types (must call constructors and destructors)
	//
	template<class... Types>
	struct variant_storage<false, Types...>
	{
		enum class StorageOp
		{
			DEFAULT_CONSTRUCT,
			DESTROY,
			COPY,
			MOVE
		};

		// handler function
		using storage_handler_ptr = void(*)(StorageOp, void*, void*);
		using aligned_storage_impl_t = aligned_union_t<16, Types...>;

		aligned_storage_impl_t mBuffer;
		storage_handler_ptr mpHandler = nullptr;

		template<typename VariantStorageT>
		inline void DoOp(StorageOp op, VariantStorageT&& other)  // bind to both rvalue and lvalues
		{
			if(mpHandler)
				DoOp(StorageOp::DESTROY);

			if (other.mpHandler)
				mpHandler = other.mpHandler;

			if(mpHandler)
				mpHandler(op, (void*)&mBuffer, (void*)&other.mBuffer);
		}

		inline void DoOp(StorageOp op)
		{
			if(mpHandler)
				mpHandler(op, &mBuffer, nullptr);
		}

		template<typename T>
		static void DoOpImpl(StorageOp op, T* pThis, T* pOther)
		{
			switch (op)
			{
				case StorageOp::DEFAULT_CONSTRUCT:
				{
					internal::default_construct_if_supported<T>::call(pThis);
				}
				break;

				case StorageOp::DESTROY:
				{
					internal::destroy_if_supported<T>::call(pThis);
				}
				break;

				case StorageOp::COPY:
				{
					internal::copy_if_supported<T>::call(pThis, pOther);
				}
				break;

				case StorageOp::MOVE:
				{
					internal::move_if_supported<T>::call(pThis, pOther);
				}
				break;

				default: {} break;
			};
		}

	public:
		variant_storage()
		{
			DoOp(StorageOp::DEFAULT_CONSTRUCT);
		}

		~variant_storage()
		{
			DoOp(StorageOp::DESTROY);
		}

		variant_storage(const variant_storage& other)
		{
			DoOp(StorageOp::COPY, other);
		}

		variant_storage(variant_storage&& other)
		{
			DoOp(StorageOp::MOVE, other);
		}

		variant_storage& operator=(const variant_storage& other)
		{
			DoOp(StorageOp::COPY, other);
			return *this;
		}

		variant_storage& operator=(variant_storage&& other)
		{
			DoOp(StorageOp::MOVE, eastl::move(other));
			return *this;
		}

		template <typename T, typename... Args>
		void set_as(Args&&... args)
		{
			// NOTE(rparolin): If this assert fires there is an EASTL problem picking the size of the local buffer which
			// variant_storage used to store types. The size selected should be large enough to hold the largest type in
			// the user provided variant type-list.
			static_assert(sizeof(aligned_storage_impl_t) >= sizeof(T), "T is larger than local buffer size");

			using RT = remove_reference_t<T>;

			new (&mBuffer) RT(eastl::forward<Args>(args)...);

			mpHandler = (storage_handler_ptr)&DoOpImpl<RT>;
		}

		template <typename T, typename U, typename... Args>
		void set_as(std::initializer_list<U> il, Args&&... args)
		{
			// NOTE(rparolin): If this assert fires there is an EASTL problem picking the size of the local buffer which
			// variant_storage used to store types. The size selected should be large enough to hold the largest type in
			// the user provided variant type-list.
			static_assert(sizeof(aligned_storage_impl_t) >= sizeof(T), "T is larger than local buffer size");

			using RT = remove_reference_t<T>;

			new (&mBuffer) RT(il, eastl::forward<Args>(args)...);

			mpHandler = (storage_handler_ptr)&DoOpImpl<RT>;
		}

		template<typename T>
		T get_as()
		{
			static_assert(eastl::is_pointer_v<T>, "T must be a pointer type");
			return reinterpret_cast<T>(&mBuffer);
		}

		template<typename T>
		const T get_as() const
		{
			static_assert(eastl::is_pointer_v<T>, "T must be a pointer type");
			return reinterpret_cast<const T>(reinterpret_cast<uintptr_t>(&mBuffer));
		}

		void destroy()
		{
			DoOp(StorageOp::DESTROY);
		}
	};


	// variant_storage
	//
	// specialization for trivial types
	//
	template<class... Types>
	struct variant_storage<true, Types...>
	{
		using aligned_storage_impl_t = aligned_union_t<16, Types...>;
		aligned_storage_impl_t mBuffer;

	public:

		// NOTE(rparolin): Since this is the specialization for trivial types can we potentially remove all the
		// defaulted special constructors.  Consider removing this.
		//
		// variant_storage() = default;
		// ~variant_storage() = default;
		// variant_storage(const variant_storage& other) = default;
		// variant_storage(variant_storage&& other) = default;
		// variant_storage& operator=(const variant_storage& other) = default;
		// variant_storage& operator=(variant_storage&& other) = default;

		template <typename T, typename... Args>
		void set_as(Args&&... args)
		{
			// NOTE(rparolin): If this assert fires there is an EASTL problem picking the size of the local buffer which
			// variant_storage used to store types. The size selected should be large enough to hold the largest type in
			// the user provided variant type-list.
			static_assert(sizeof(aligned_storage_impl_t) >= sizeof(T), "T is larger than local buffer size");
			new (&mBuffer) remove_reference_t<T>(eastl::forward<Args>(args)...);

			// mpHandler = ...; // member does not exist in this template specialization
		}

		template <typename T, typename U, typename... Args>
		void set_as(std::initializer_list<U> il, Args&&... args)
		{
			// NOTE(rparolin): If this assert fires there is an EASTL problem picking the size of the local buffer which
			// variant_storage used to store types. The size selected should be large enough to hold the largest type in
			// the user provided variant type-list.
			static_assert(sizeof(aligned_storage_impl_t) >= sizeof(T), "T is larger than local buffer size");
			new (&mBuffer) remove_reference_t<T>(il, eastl::forward<Args>(args)...);

			// mpHandler = ...; // member does not exist in this template specialization
		}

		template<typename T>
		T get_as()
		{
			static_assert(eastl::is_pointer_v<T>, "T must be a pointer type");
			return reinterpret_cast<T>(&mBuffer);
		}

		template<typename T>
		const T get_as() const
		{
			static_assert(eastl::is_pointer_v<T>, "T must be a pointer type");
			return reinterpret_cast<const T>(reinterpret_cast<uintptr_t>(&mBuffer));
		}

		void destroy() {}
	};


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
	template <class... Types>
	struct hash<variant<Types...> >
		{ size_t operator()(const variant<Types...>&) const { return static_cast<size_t>(-0x42); } };


	///////////////////////////////////////////////////////////////////////////
	// get_if
	//
	template <size_t I, class... Types>
	EA_CONSTEXPR add_pointer_t<variant_alternative_t<I, variant<Types...>>> get_if(variant<Types...>* pv) EA_NOEXCEPT
	{
		static_assert(I < sizeof...(Types), "get_if is ill-formed if I is not a valid index in the variant typelist");
		using return_type = add_pointer_t<variant_alternative_t<I, variant<Types...>>>;

		return (!pv || pv->index() != I) ? nullptr : pv->mStorage.template get_as<return_type>();
	}

	template <size_t I, class... Types>
	EA_CONSTEXPR add_pointer_t<const variant_alternative_t<I, variant<Types...>>> get_if(const variant<Types...>* pv) EA_NOEXCEPT
	{
		static_assert(I < sizeof...(Types), "get_if is ill-formed if I is not a valid index in the variant typelist");
		using return_type = add_pointer_t<variant_alternative_t<I, variant<Types...>>>;

		return (!pv || pv->index() != I) ? nullptr : pv->mStorage.template get_as<return_type>();
	}

	template <class T, class... Types, size_t I = meta::get_type_index_v<T, Types...>>
	EA_CONSTEXPR add_pointer_t<T> get_if(variant<Types...>* pv) EA_NOEXCEPT
	{
		return get_if<I>(pv);
	}

	template <class T, class... Types, size_t I = meta::get_type_index_v<T, Types...>>
	EA_CONSTEXPR add_pointer_t<const T> get_if(const variant<Types...>* pv) EA_NOEXCEPT
	{
		return get_if<I>(pv);
	}


	///////////////////////////////////////////////////////////////////////////
	// get
	//
	template <size_t I, class... Types>
	EA_CONSTEXPR variant_alternative_t<I, variant<Types...>>& get(variant<Types...>& v)
	{
		static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist");
		using return_type = add_pointer_t<variant_alternative_t<I, variant<Types...>>>;

		return *v.mStorage.template get_as<return_type>();
	}

	template <size_t I, class... Types>
	EA_CONSTEXPR variant_alternative_t<I, variant<Types...>>&& get(variant<Types...>&& v)
	{
		static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist");
		using return_type = add_pointer_t<variant_alternative_t<I, variant<Types...>>>;

		return eastl::move(*v.mStorage.template get_as<return_type>());
	}

	template <size_t I, class... Types>
	EA_CONSTEXPR const variant_alternative_t<I, variant<Types...>>& get(const variant<Types...>& v)
	{
		static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist");
		using return_type = add_pointer_t<variant_alternative_t<I, variant<Types...>>>;

		return *v.mStorage.template get_as<return_type>();
	}

	template <size_t I, class... Types>
	EA_CONSTEXPR const variant_alternative_t<I, variant<Types...>>&& get(const variant<Types...>&& v)
	{
		static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist");
		using return_type = add_pointer_t<variant_alternative_t<I, variant<Types...>>>;

		return eastl::move(*v.mStorage.template get_as<return_type>());
	}

	template <class T, class... Types, size_t I = meta::get_type_index_v<T, Types...>>
	EA_CONSTEXPR T& get(variant<Types...>& v)
	{
		static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist");
		return get<I>(v);
	}

	template <class T, class... Types, size_t I = meta::get_type_index_v<T, Types...>>
	EA_CONSTEXPR T&& get(variant<Types...>&& v)
	{
		static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist");
		return get<I>(eastl::move(v));
	}

	template <class T, class... Types, size_t I = meta::get_type_index_v<T, Types...>>
	EA_CONSTEXPR const T& get(const variant<Types...>& v)
	{
		static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist");
		return get<I>(v);
	}

	template <class T, class... Types, size_t I = meta::get_type_index_v<T, Types...>>
	EA_CONSTEXPR const T&& get(const variant<Types...>&& v)
	{
		static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist");
		return get<I>(v);
	}


	///////////////////////////////////////////////////////////////////////////
	// 20.7.4, value access
	//
	template <class T, class... Types, ssize_t I = meta::get_type_index_v<T, Types...>>
	EA_CONSTEXPR bool holds_alternative(const variant<Types...>& v) EA_NOEXCEPT
	{
		// ssize_t template parameter because the value can be negative
		return I == variant_npos ? false : (v.index() == I);
	}


	///////////////////////////////////////////////////////////////////////////
	// 20.7.2, variant
	//
	template <class... Types>
	class variant
	{
		static_assert(sizeof...(Types) > 0,                           "variant must have at least 1 type (empty variants are ill-formed)");
		static_assert(disjunction_v<is_void<Types>...> == false,      "variant does not allow void as an alternative type");
		static_assert(disjunction_v<is_reference<Types>...> == false, "variant does not allow references as an alternative type");
		static_assert(disjunction_v<is_array<Types>...> == false,     "variant does not allow arrays as an alternative type");

		using variant_index_t = size_t;
		using variant_storage_t = variant_storage<conjunction_v<is_trivially_destructible<Types>...>, Types...>;
		using T_0 = variant_alternative_t<0, variant<Types...>>;  // alias for the 1st type in the variadic pack

		///////////////////////////////////////////////////////////////////////////
		// variant data members
		//
		variant_index_t mIndex;
		variant_storage_t mStorage;

	public:
		///////////////////////////////////////////////////////////////////////////
		// 20.7.2.1, constructors
		//

		// Only participates in overload resolution when the first alternative is default constructible
		template <typename TT0 = T_0, typename = enable_if_t<is_default_constructible_v<TT0>>>
		EA_CONSTEXPR variant() EA_NOEXCEPT : mIndex(variant_npos), mStorage()
		{
			mIndex = static_cast<variant_index_t>(0);
			mStorage.template set_as<T_0>();
		}

		// Only participates in overload resolution if is_copy_constructible_v<T_i> is true for all T_i in Types....
		template <bool enable = conjunction_v<is_copy_constructible<Types>...>,
		          typename = enable_if_t<enable>> // add a dependent type to enable sfinae
		variant(const variant& other)
		{
			if (this != &other)
			{
				mIndex = other.mIndex;
				mStorage = other.mStorage;
			}
		}

		// Only participates in overload resolution if is_move_constructible_v<T_i> is true for all T_i in Types...
		template <bool enable = conjunction_v<is_move_constructible<Types>...>, typename = enable_if_t<enable>> // add a dependent type to enable sfinae
		EA_CONSTEXPR variant(variant&& other) EA_NOEXCEPT(conjunction_v<is_move_constructible<Types>...>)
		    : mIndex(variant_npos), mStorage()
		{
			if(this != &other)
			{
				mIndex = other.mIndex;
				mStorage = eastl::move(other.mStorage);
			}
		}

		// Conversion constructor
		template <typename T,
		          typename T_j = meta::overload_resolution_t<T, meta::overload_set<Types...>>,
		          typename = enable_if_t<!is_same_v<decay_t<T>, variant>>,
		          size_t I = meta::get_type_index_v<decay_t<T_j>, Types...>>
		EA_CONSTEXPR variant(T&& t) EA_NOEXCEPT(is_nothrow_constructible_v<T_j, T>)
		   : mIndex(variant_npos), mStorage()
		{
			static_assert(I >= 0, "T not found in type-list.");
			static_assert((meta::type_count_v<T_j, Types...> == 1), "function overload is not unique - duplicate types in type list");

			mIndex = static_cast<variant_index_t>(I);
			mStorage.template set_as<T_j>(eastl::forward<T>(t));
		}


		///////////////////////////////////////////////////////////////////////////
		// 20.7.2.1, in_place_t constructors
		//
		template <
			class T,
			class... Args,
			class = enable_if_t<conjunction_v<meta::duplicate_type_check<T, Types...>, is_constructible<T, Args...>>, T>>
		EA_CPP14_CONSTEXPR explicit variant(in_place_type_t<T>, Args&&... args)
			: variant(in_place<meta::get_type_index_v<T, Types...>>, eastl::forward<Args>(args)...)
		{}

		template <
		    class T,
		    class U,
		    class... Args,
		    class = enable_if_t<conjunction_v<meta::duplicate_type_check<T, Types...>, is_constructible<T, Args...>>, T>>
		EA_CPP14_CONSTEXPR explicit variant(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
		    : variant(in_place<meta::get_type_index_v<T, Types...>>, il, eastl::forward<Args>(args)...)
		{}

		template <size_t I,
		          class... Args,
		          class = enable_if_t<conjunction_v<integral_constant<bool, (I < sizeof...(Types))>,
		                                            is_constructible<meta::get_type_at_t<I, Types...>, Args...>>>>
		EA_CPP14_CONSTEXPR explicit variant(in_place_index_t<I>, Args&&... args)
		    : mIndex(I)
		{
			mStorage.template set_as<meta::get_type_at_t<I, Types...>>(eastl::forward<Args>(args)...);
		}

		template <size_t I,
		          class U,
		          class... Args,
		          class = enable_if_t<conjunction_v<integral_constant<bool, (I < sizeof...(Types))>,
		                                            is_constructible<meta::get_type_at_t<I, Types...>, Args...>>>>
		EA_CPP14_CONSTEXPR explicit variant(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
		    : mIndex(I)
		{
			mStorage.template set_as<meta::get_type_at_t<I, Types...>>(il, eastl::forward<Args>(args)...);
		}


		///////////////////////////////////////////////////////////////////////////
		// 20.7.2.2, destructor
		//
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
		    typename = enable_if_t<conjunction_v<is_constructible<T, Args...>, meta::duplicate_type_check<T, Types...>>>>
		decltype(auto) emplace(Args&&... args)
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
		          typename = enable_if_t<conjunction_v<is_constructible<T, std::initializer_list<U>&, Args...>,
		                                               meta::duplicate_type_check<T, Types...>>>>
		decltype(auto) emplace(std::initializer_list<U> il, Args&&... args)
		{
			return emplace<I>(il, eastl::forward<T>(args)...);
		}

		// First, destroys the currently contained value (if any). Then direct-initializes the contained value as if
		// constructing a value of type T_I with the arguments std::forward<Args>(args).... If an exception is thrown,
		// *this may become valueless_by_exception. This overload only participates in overload resolution if
		// std::is_constructible_v<T_I, Args...> is true. The behavior is undefined if I is not less than
		// sizeof...(Types).
		//
		template <size_t I,
		          class... Args,
		          typename T = meta::get_type_at_t<I, Types...>,
		          typename =
		              enable_if_t<conjunction_v<is_constructible<T, Args...>, meta::duplicate_type_check<T, Types...>>>>
		variant_alternative_t<I, variant>& emplace(Args&&... args)
		{
			if (!valueless_by_exception())
			{
				mStorage.destroy();

				#if EASTL_EXCEPTIONS_ENABLED
					mIndex = static_cast<variant_index_t>(variant_npos);
				#endif
			}

			mStorage.template set_as<T>(eastl::forward<Args>(args)...);
			mIndex = static_cast<variant_index_t>(I);
			return *reinterpret_cast<T*>(&mStorage.mBuffer);
		}

		// First, destroys the currently contained value (if any). Then direct-initializes the contained value as if
		// constructing a value of type T_I with the arguments il, std::forward<Args>(args).... If an exception is
		// thrown, *this may become valueless_by_exception. This overload only participates in overload resolution if
		// std::is_constructible_v<T_I, initializer_list<U>&, Args...> is true. The behavior is undefined if I is not
		// less than sizeof...(Types).
		//
		template <size_t I,
		          class U,
		          class... Args,
		          typename T = meta::get_type_at_t<I, Types...>,
		          typename = enable_if_t<conjunction_v<is_constructible<T, std::initializer_list<U>&, Args...>, meta::duplicate_type_check<T, Types...>>>>
		variant_alternative_t<I, variant>& emplace(std::initializer_list<U> il, Args&&... args)
		{
			if (!valueless_by_exception())
			{
				mStorage.destroy();

				#if EASTL_EXCEPTIONS_ENABLED
					mIndex = static_cast<variant_index_t>(variant_npos);
				#endif
			}

			mStorage.template set_as<T>(il, eastl::forward<Args>(args)...);
			mIndex = static_cast<variant_index_t>(I);
			return *reinterpret_cast<T*>(&mStorage.mBuffer);
		}


		///////////////////////////////////////////////////////////////////////////
		// 20.7.2.3, assignment
		//
		template <class T,
		          typename T_j = meta::overload_resolution_t<T, meta::overload_set<Types...>>,
		          ssize_t I = meta::get_type_index_v<decay_t<T_j>, Types...>,
		          typename = enable_if_t<!eastl::is_same_v<decay_t<T>, variant> && eastl::is_assignable_v<T_j&, T> &&
		                                 eastl::is_constructible_v<T_j, T>>>
		EA_CPP14_CONSTEXPR variant& operator=(T&& t)
		    EA_NOEXCEPT(conjunction_v<is_nothrow_assignable<T_j&, T>, is_nothrow_constructible<T_j, T>>)
		{
			static_assert(I >= 0, "T not found in type-list.");
			static_assert((meta::type_count_v<T_j, Types...> == 1),
			              "function overload is not unique - duplicate types in type list");

			if (!valueless_by_exception())
				mStorage.destroy();

			mIndex = static_cast<variant_index_t>(I);
			mStorage.template set_as<T_j>(eastl::forward<T>(t));
			return *this;
		}


		// Only participates in overload resolution if is_copy_constructible_v<T_i> && is_copy_assignable_v<T_i> is true
		// for all T_i in Types....
		template <bool enable = conjunction_v<conjunction<is_copy_constructible<Types>...>,
		                                      conjunction<is_copy_assignable<Types>...>>,
		          typename = enable_if_t<enable>> // add a dependent type to enable sfinae
		variant& operator=(const variant& other)
		{
			if (this != &other)
			{
				mIndex = other.mIndex;
				mStorage = other.mStorage;
			}
			return *this;
		}

		// Only participates in overload resolution if is_move_constructible_v<T_i> && is_move_assignable_v<T_i> is true for all T_i in Types....
		template <bool enable = conjunction_v<conjunction<is_move_constructible<Types>...>,
		                                      conjunction<is_move_assignable<Types>...>>,
		          typename = enable_if_t<enable>> // add a dependent type to enable sfinae
		variant& operator=(variant&& other)
		    EA_NOEXCEPT(conjunction_v<conjunction<is_nothrow_move_constructible<Types>...>,
		                              conjunction<is_nothrow_move_assignable<Types>...>>)
		{
			if (this != &other)
			{
				mIndex = eastl::move(other.mIndex);
				mStorage = eastl::move(other.mStorage);
			}
			return *this;
		}


		///////////////////////////////////////////////////////////////////////////
		// 20.7.2.5, value status
		//
		EA_CONSTEXPR size_t index() const EA_NOEXCEPT
		{
			#if EASTL_EXCEPTIONS_ENABLED
				return valueless_by_exception() ? variant_npos : mIndex;
			#else
				return mIndex;
			#endif
		}

		EA_CONSTEXPR bool valueless_by_exception() const EA_NOEXCEPT
		{
			#if EASTL_EXCEPTIONS_ENABLED
				return mIndex == variant_npos;
			#else
				return false;
			#endif
		}


		///////////////////////////////////////////////////////////////////////////
		// 20.7.2.6, swap
		//
		void swap(variant& other)
			EA_NOEXCEPT(conjunction_v<is_nothrow_move_constructible<Types>..., is_nothrow_swappable<Types>...>)
		{
			eastl::swap(mIndex, other.mIndex);
			eastl::swap(mStorage, other.mStorage);
		}

	private:
		// NOTE(rparolin): get_if accessors require internal access to the variant storage class
		template <size_t I, class... Types2> friend EA_CONSTEXPR add_pointer_t<      variant_alternative_t<I, variant<Types2...>>> get_if(      variant<Types2...>* pv) EA_NOEXCEPT;
		template <size_t I, class... Types2> friend EA_CONSTEXPR add_pointer_t<const variant_alternative_t<I, variant<Types2...>>> get_if(const variant<Types2...>* pv) EA_NOEXCEPT;

		// NOTE(rparolin): get accessors require internal access to the variant storage class
		template <size_t I, class... Types2> friend EA_CONSTEXPR       variant_alternative_t<I, variant<Types2...>>&  get(variant<Types2...>& v);
		template <size_t I, class... Types2> friend EA_CONSTEXPR       variant_alternative_t<I, variant<Types2...>>&& get(variant<Types2...>&& v);
		template <size_t I, class... Types2> friend EA_CONSTEXPR const variant_alternative_t<I, variant<Types2...>>&  get(const variant<Types2...>& v);
		template <size_t I, class... Types2> friend EA_CONSTEXPR const variant_alternative_t<I, variant<Types2...>>&& get(const variant<Types2...>&& v);
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

	// visit
	//
	template <typename Visitor, typename Variant>
	EA_CPP14_CONSTEXPR decltype(auto) visit(Visitor&& visitor, Variant&& variant)
	{
		visit_static_assert_check(eastl::forward<Variant>(variant));

		visit_throw_bad_variant_access(eastl::forward<Variant>(variant));

		return visitor_caller_one::call(eastl::forward<Visitor>(visitor),
										eastl::forward<Variant>(variant));
	}

	template <typename Visitor, typename... Variants>
	EA_CPP14_CONSTEXPR decltype(auto) visit(Visitor&& visitor, Variants&&... variants)
	{
		visit_static_assert_check(eastl::forward<Variants>(variants)...);

		visit_throw_bad_variant_access(eastl::forward<Variants>(variants)...);

		return call_initial_n(make_index_sequence<variant_size_v<decay_t<meta::get_type_at_t<0, Variants...>>>>{},
							  eastl::forward<Visitor>(visitor),
							  eastl::forward<Variants>(variants)...);

	}

	template <typename R, typename Visitor, typename Variant, eastl::enable_if_t<!eastl::is_same_v<R, Visitor>, int> = 0>
	EA_CPP14_CONSTEXPR R visit(Visitor&& visitor, Variant&& variant)
	{
		visit_static_assert_check(eastl::forward<Variant>(variant));

		visit_throw_bad_variant_access(eastl::forward<Variant>(variant));

		return visitor_caller_one_r::call_r<R>(eastl::forward<Visitor>(visitor),
											   eastl::forward<Variant>(variant));
	}

	template <typename R, typename Visitor, typename... Variants, eastl::enable_if_t<!eastl::is_same_v<R, Visitor>, int> = 0>
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
