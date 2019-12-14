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
#include <EASTL/internal/type_pod.h>
#include <EASTL/internal/in_place_t.h>
#include <EASTL/internal/integer_sequence.h>
#include <EASTL/meta.h>
#include <EASTL/utility.h>
#include <EASTL/functional.h> 
#include <EASTL/initializer_list.h>
#include <EASTL/tuple.h>

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
			static void call(T* pThis) {} // intentionally blank
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
			static void call(T* pThis, T* pOther) {} // intentionally blank
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
			static void call(T* pThis, T* pOther) {} // intentionally blank
		};
	} // namespace internal 


	///////////////////////////////////////////////////////////////////////////
	// 20.7.3, variant_npos
	//
	#ifdef EA_COMPILER_NO_INLINE_VARIABLES
		static EA_CONSTEXPR const size_t variant_npos = size_t(-1);
	#else
		inline EA_CONSTEXPR size_t variant_npos = -1;
	#endif


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
	EA_CONSTEXPR bool operator> (monostate, monostate) EA_NOEXCEPT { return false; }
	EA_CONSTEXPR bool operator< (monostate, monostate) EA_NOEXCEPT { return false; }
	EA_CONSTEXPR bool operator!=(monostate, monostate) EA_NOEXCEPT { return false; }
	EA_CONSTEXPR bool operator<=(monostate, monostate) EA_NOEXCEPT { return true; }
	EA_CONSTEXPR bool operator>=(monostate, monostate) EA_NOEXCEPT { return true; }
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
	template <class T> EA_CONSTEXPR size_t variant_size_v = variant_size<T>::value;


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
	template <size_t I, class T> struct variant_alternative<I, const T>          : add_cv_t<variant_alternative<I, T>> {};
	template <size_t I, class T> struct variant_alternative<I, volatile T>       : add_volatile_t<variant_alternative<I, T>> {};
	template <size_t I, class T> struct variant_alternative<I, const volatile T> : add_cv_t<variant_alternative<I, T>> {};

	// variant_alternative_t template alias
	template <size_t I, class T> using variant_alternative_t = typename variant_alternative<I, T>::type;


	///////////////////////////////////////////////////////////////////////////
	// 20.7.11, hash support
	//
	template <class... Types>
	struct hash<variant<Types...> >
		{ size_t operator()(const variant<Types...>& val) const { return static_cast<size_t>(-0x42); } };


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
		
		EASTL_ASSERT(v.index() == I);
		return *v.mStorage.template get_as<return_type>();
	}

	template <size_t I, class... Types>
	EA_CONSTEXPR variant_alternative_t<I, variant<Types...>>&& get(variant<Types...>&& v)
	{
		static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist");
		using return_type = add_pointer_t<variant_alternative_t<I, variant<Types...>>>;
		
		EASTL_ASSERT(v.index() == I);
		return eastl::move(*v.mStorage.template get_as<return_type>());
	}

	template <size_t I, class... Types>
	EA_CONSTEXPR const variant_alternative_t<I, variant<Types...>>& get(const variant<Types...>& v)
	{
		static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist");
		using return_type = add_pointer_t<variant_alternative_t<I, variant<Types...>>>;
		
		EASTL_ASSERT(v.index() == I);
		return *v.mStorage.template get_as<return_type>();
	}

	template <size_t I, class... Types>
	EA_CONSTEXPR const variant_alternative_t<I, variant<Types...>>&& get(const variant<Types...>&& v)
	{
		static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist");
		using return_type = add_pointer_t<variant_alternative_t<I, variant<Types...>>>;
		
		EASTL_ASSERT(v.index() == I);
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
		return get<I>(v);
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
			: variant(in_place<meta::get_type_index_v<T, Types...>>, forward<Args>(args)...)
		{}

		template <
		    class T,
		    class U,
		    class... Args,
		    class = enable_if_t<conjunction_v<meta::duplicate_type_check<T, Types...>, is_constructible<T, Args...>>, T>>
		EA_CPP14_CONSTEXPR explicit variant(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
		    : variant(in_place<meta::get_type_index_v<T, Types...>>, il, forward<Args>(args)...)
		{}

		template <size_t I,
		          class... Args,
		          class = enable_if_t<conjunction_v<integral_constant<bool, (I < sizeof...(Types))>,
		                                            is_constructible<meta::get_type_at_t<I, Types...>, Args...>>>>
		EA_CPP14_CONSTEXPR explicit variant(in_place_index_t<I>, Args&&... args)
		    : mIndex(I)
		{
			mStorage.template set_as<meta::get_type_at_t<I, Types...>>(forward<Args>(args)...);
		}

		template <size_t I,
		          class U,
		          class... Args,
		          class = enable_if_t<conjunction_v<integral_constant<bool, (I < sizeof...(Types))>,
		                                            is_constructible<meta::get_type_at_t<I, Types...>, Args...>>>>
		EA_CPP14_CONSTEXPR explicit variant(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
		    : mIndex(I)
		{
			mStorage.template set_as<meta::get_type_at_t<I, Types...>>(il, forward<Args>(args)...);
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
				mStorage.destroy();

			mIndex = static_cast<variant_index_t>(I);
			mStorage.template set_as<T>(eastl::forward<Args>(args)...);
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
				mStorage.destroy();

			mIndex = static_cast<variant_index_t>(I);
			mStorage.template set_as<T>(il, eastl::forward<Args>(args)...);
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
		EA_CONSTEXPR size_t index() const EA_NOEXCEPT { return valueless_by_exception() ? variant_npos : mIndex; }
		EA_CONSTEXPR bool valueless_by_exception() const EA_NOEXCEPT { return mIndex == variant_npos; }


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
	//   - It must allow different return types in the visitor, as long as they are all convertible
	//
	// visitor_caller is responsible for the mechanics of visit. Each visitor_caller creates an array of
	// functions which call get<I>() on the variant (where I is the array index), then add the returned reference
	// to a tuple of arguments. The final visitor_caller calls invoke() with the visitor and the unpacked
	// arguments.
	//
	// This allows us to look up each appropriate get() function in constant time using the variant's index.
	template <typename Visitor, typename Variant, typename... Variants>
	struct visitor_caller
	{
		// @visitor, @variant and @variants are all the arguments to the initial visit() function.
		// @args is the tuple of arguments which have been retrieved by any previous visitor_callers.
		//
		// The two unnamed index_sequence parameters let us deduce two different sets of indices
		// as parameter packs - one for the arguments and one for the array of call_next functions.
		// This is necessary so we can create the constexpr array of functions which call
		// get<I>(variant) based on the array index, and so we can unpack the final of arguments by
		// calling get<I>(args) for each index in args.
		template <size_t I, typename ArgsTuple, size_t... ArgsIndices, size_t... ArrayIndices>
		static decltype(auto) EA_CONSTEXPR call_next(Visitor&& visitor,
		                                             index_sequence<ArgsIndices...>,
		                                             index_sequence<ArrayIndices...>,
		                                             ArgsTuple&& args,
		                                             Variant&& variant,
		                                             Variants&&... variants)
		{
			// Call the appropriate get() function on the variant, and pack the result into a new tuple along with
			// all of the previous arguments. Then call the next visitor_caller with the new argument added,
			// and the current variant removed.
			return visitor_caller<Visitor, Variants...>::call(
				forward<Visitor>(visitor),
				index_sequence<ArgsIndices..., sizeof...(ArgsIndices)>(),
				index_sequence<ArrayIndices...>(),
				make_tuple(get<ArgsIndices>(forward<ArgsTuple>(args))..., get<I>(forward<Variant>(variant))),
				forward<Variants>(variants)...
			);
		}

		// Arguments are the same as for call_next (see above).
		template <typename ArgsTuple, size_t... ArgsIndices, size_t... ArrayIndices>
		static decltype(auto) EA_CPP14_CONSTEXPR call(Visitor&& visitor,
		                                              index_sequence<ArgsIndices...>,
		                                              index_sequence<ArrayIndices...>,
		                                              ArgsTuple&& args,
		                                              Variant&& variant,
		                                              Variants&&... variants)
		{
			// Deduce the type of the inner array of call_next functions
			using return_type = decltype(call_next<0>(
				forward<Visitor>(visitor),
				index_sequence<ArgsIndices...>(),
				index_sequence<ArrayIndices...>(),
				forward<ArgsTuple>(args),
				forward<Variant>(variant),
				forward<Variants>(variants)...)
			);

			using next_type = return_type (*)(
				Visitor&&,
				index_sequence<ArgsIndices...>,
				index_sequence<ArrayIndices...>,
				ArgsTuple&&,
				Variant&&,
				Variants&&...
			);

			// Create an array of call_next<0>, call_next<1>, ... , call_next<N - 1>
			// where N = variant_size<Variant>.
			EA_CPP14_CONSTEXPR next_type next[] = { static_cast<next_type>(call_next<ArrayIndices>)... };

			// call_next() with the correct index for the variant.
			return next[variant.index()](
				forward<Visitor>(visitor),
				index_sequence<ArgsIndices...>(),
				index_sequence<ArrayIndices...>(),
				forward<ArgsTuple>(args),
				forward<Variant>(variant),
				forward<Variants>(variants)...
			);
		}
	};

	template <typename Visitor, typename Variant>
	struct visitor_caller<Visitor, Variant>
	{
		// Invoke the correct visitor for a given variant index, and call the correct get() function to retrieve
		// the argument. Unpack any additional arguments from earlier visitor_callers (see above).
		template <typename R, size_t I, typename ArgsTuple, size_t... ArgsIndices>
		static decltype(auto) EA_CONSTEXPR invoke_visitor(Visitor&& visitor, index_sequence<ArgsIndices...>, ArgsTuple&& args, Variant&& variant)
		{
			return static_cast<R>(invoke(
				forward<Visitor>(visitor),
				get<ArgsIndices>(forward<ArgsTuple>(args))...,
				get<I>(forward<Variant>(variant))
			));
		}

		// The final call() in the recursion.
		//
		// By this point, <ArgsIndices...> expands to <0 .. N - 2> where N is the number of arguments to the
		// final invoke() call. This corresponds to each element in @args, so `get<ArgsIndices>(args)...`
		// expands to `get<0>(args), get<1>(args), ... , get<N - 2>(args)`. The final argument is selected
		// based on the final array index, leaving us with a sequence of arguments from 0 .. N - 1.
		//
		// <ArrayIndices...> is the same as in earlier calls - it expands to <0 .. I - 1> where I is the
		// number of alternatives in the variant. This lets us call the correct `get<I>` based on the
		// final variant index, as we did for all earlier calls.
		template <typename ArgsTuple, size_t... ArgsIndices, size_t... ArrayIndices>
		static decltype(auto) EA_CPP14_CONSTEXPR call(Visitor&& visitor, index_sequence<ArgsIndices...>, index_sequence<ArrayIndices...>, ArgsTuple&& args, Variant&& variant)
		{
			// MSVC isn't able to handle the nested pack expansion required here, so we have to just use the
			// return type of the first visitor function instead of the common_type of all possible visitor
			// functions. This means we can't handle the case where visitor functions return different (but
			// compatible) types. This is unlikely to be a common case, but we might be able to get around it
			// if it's a big issue.
			//
			// TODO: we should reevaluate this on future compiler releases
			#if defined(EA_COMPILER_MSVC)
			using return_type = invoke_result_t<Visitor, decltype(get<ArgsIndices>(args))..., decltype(get<0>(variant))>;
			#else
			// If we're on a compiler that can take it, determine the common_type between all possible visitor
			// invocations.
			using return_type = common_type_t<
				invoke_result_t<Visitor, decltype(get<ArgsIndices>(args))..., decltype(get<ArrayIndices>(variant))>...
			>;
			#endif

			using caller_type = return_type (*)(Visitor&&, index_sequence<ArgsIndices...>, ArgsTuple&&, Variant&&);

			// Create the final array of invoke_visitor<0>, invoke_visitor<1>, ... , invoke_visitor<N - 1>
			// where N = variant_size<Variant>
			EA_CPP14_CONSTEXPR caller_type callers[] = { invoke_visitor<return_type, ArrayIndices>... };

			return callers[forward<Variant>(variant).index()](
				forward<Visitor>(visitor),
				index_sequence<ArgsIndices...>(),
				forward<ArgsTuple>(args),
				forward<Variant>(variant)
			);
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

	// visit
	//
	template <class Visitor, class... Variants>
	EA_CONSTEXPR decltype(auto) visit(Visitor&& visitor, Variants&&... variants)
	{
		static_assert(sizeof...(Variants) > 0, "at least one variant instance must be passed as an argument to the visit function");

		using variant_type = remove_reference_t<meta::get_type_at_t<0, Variants...>>;
		static_assert(conjunction_v<is_same<variant_type, remove_reference_t<Variants>>...>,
		              "all variants passed to eastl::visit() must have the same type");

		return visitor_caller<Visitor, Variants...>::call(
			forward<Visitor>(visitor),
			index_sequence<>(),
			make_index_sequence<variant_size_v<variant_type>>(),
			tuple<>(),
			forward<Variants>(variants)...
		);
	}


	///////////////////////////////////////////////////////////////////////////
	// 20.7.5, relational operators
	//
	template <class... Types, class Predicate>
	EA_CPP14_CONSTEXPR bool Compare(const variant<Types...>& lhs, const variant<Types...>& rhs, Predicate predicate)
	{
		return visit(predicate, lhs, rhs);
	}

	// For variant visitation, we need to have a comparison function for all possible combinations of types,
	// eg. for variant<int, string>, our comparator needs:
	//
	//   bool operator()(int, int);
	//   bool operator()(int, string);
	//   bool operator()(string, int);
	//   bool operator()(string, string);
	//
	// Even though we never call the mixed-type versions of these functions when comparing variants, we
	// need them in order to compile visit(). So this struct forwards the good comparisons to the appropriate
	// comparison, and asserts that we never call the bad comparisons.
	template <typename C>
	struct variant_comparison : public C
	{
		template <typename A, typename B>
		auto operator()(A&& a, B&& b) -> decltype(C::operator()(a, b)) { return C::operator()(a, b); }

		template <typename... Args> bool operator()(Args&&...) { EASTL_ASSERT(false); return false; }
	};

	///////////////////////////////////////////////////////////////////////////
	// 20.7.5, relational operators
	//
	template <class... Types>
	EA_CPP14_CONSTEXPR bool operator==(const variant<Types...>& lhs, const variant<Types...>& rhs)
	{
		if(lhs.index() != rhs.index()) return false;
		if(lhs.valueless_by_exception()) return true;
		return Compare(lhs, rhs, variant_comparison<equal_to<>>{});
	}

	template <class... Types>
	EA_CPP14_CONSTEXPR bool operator<(const variant<Types...>& lhs, const variant<Types...>& rhs)
	{
		if(rhs.valueless_by_exception()) return false;
		if(lhs.valueless_by_exception()) return true;
		if(lhs.index() < rhs.index()) return true;
		if(lhs.index() > rhs.index()) return false;
		return Compare(lhs, rhs, variant_comparison<less<>>{});
	}

	template <class... Types>
	EA_CPP14_CONSTEXPR bool operator!=(const variant<Types...>& lhs, const variant<Types...>& rhs)
	{
		if(lhs.index() != rhs.index()) return true;
		if(lhs.valueless_by_exception()) return false;
		return !(lhs == rhs);
	}

	template <class... Types>
	EA_CPP14_CONSTEXPR bool operator>(const variant<Types...>& lhs, const variant<Types...>& rhs)
	{
		if(lhs.valueless_by_exception()) return false;
		if(rhs.valueless_by_exception()) return true;
		if(lhs.index() > rhs.index()) return true;
		if(lhs.index() < rhs.index()) return false;
		return rhs < lhs;
	}

	template <class... Types>
	EA_CPP14_CONSTEXPR bool operator<=(const variant<Types...>& lhs, const variant<Types...>& rhs)
	{
		if(rhs.valueless_by_exception()) return true;
		if(lhs.valueless_by_exception()) return false;
		if(lhs.index() < rhs.index()) return true;
		if(lhs.index() > rhs.index()) return false;
		return !(rhs < lhs);
	}

	template <class... Types>
	EA_CPP14_CONSTEXPR bool operator>=(const variant<Types...>& lhs, const variant<Types...>& rhs)
	{
		if(rhs.valueless_by_exception()) return true;
		if(lhs.valueless_by_exception()) return false;
		if(lhs.index() > rhs.index()) return true;
		if(lhs.index() < rhs.index()) return false;
		return !(lhs < rhs);
	}
} // namespace eastl

EA_RESTORE_VC_WARNING()

#endif // EASTL_VARIANT_H

