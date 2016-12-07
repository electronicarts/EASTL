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
#include <EASTL/internal/in_place_t.h> // eastl::in_place_t

#if defined(EASTL_OPTIONAL_ENABLED) && EASTL_OPTIONAL_ENABLED

EA_DISABLE_VC_WARNING(4583) // destructor is not implicitly called

namespace eastl
{
	#ifdef EASTL_EXCEPTIONS_ENABLED
		#define EASTL_OPTIONAL_NOEXCEPT 
	#else
		#define EASTL_OPTIONAL_NOEXCEPT EA_NOEXCEPT
	#endif

	///////////////////////////////////////////////////////////////////////////////
	/// nullopt_t
	///
	/// nullopt_t is class type used to indicate eastl::optional type with uninitialized state.
	///
	struct nullopt_t
	{
		EA_CONSTEXPR nullopt_t(int) {}
	};
	EA_CONSTEXPR nullopt_t nullopt{0};


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

	namespace Internal
	{
		///////////////////////////////////////////////////////////////////////////////
		/// optional_storage
		///
		template<typename T, bool IsTriviallyDestructible = eastl::is_trivially_destructible<T>::value>
		struct optional_storage
		{
			typedef typename eastl::remove_const<T>::type value_type;

			inline optional_storage() EA_NOEXCEPT : empty_val('\0') {}
			inline optional_storage(const optional_storage& other) : val(other.val), engaged(other.engaged) { }
			inline optional_storage(const value_type& v) : val(v), engaged(true) {}
			inline ~optional_storage()
			{
				if (engaged)
					destruct_value();
			}

			inline optional_storage& operator=(const optional_storage& other) {}

			#if EASTL_VARIADIC_TEMPLATES_ENABLED
				template <class... Args>
				inline explicit optional_storage(in_place_t, Args&&... args)
					: val(eastl::forward<Args>(args)...), engaged(true) {}

				template <typename U,
						  typename... Args,
						  typename = typename eastl::enable_if<
							  eastl::is_constructible<T, std::initializer_list<U>&, Args&&...>::value>::type>
				inline explicit optional_storage(in_place_t, std::initializer_list<U> ilist, Args&&... args)
					: val(ilist, eastl::forward<Args>(args)...), engaged(true) {}
			#endif

			inline void destruct_value() { val.~value_type(); }


			// This union exists to support trivial types that do not require constructors/destructors to be called.
			// The eastl::optional<T> type will set the empty_val in this case to "initialize" its member data. 
			union
			{
				value_type val; // consider making this aligned storage.
				char empty_val;
			};
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
			typedef typename eastl::remove_const<T>::type value_type;

			inline optional_storage() EA_NOEXCEPT : empty_val('\0') {}
			inline optional_storage(const optional_storage& other) : val(other.val), engaged(other.engaged) { }
			inline optional_storage(const value_type& v) : val(v), engaged(true) {}

			// Removed to make optional<T> trivially destructible when T is trivially destructible.
			//
			// inline ~optional_storage()
			// {
			//     if (engaged)
			//         destruct_value();
			// }

			inline optional_storage& operator=(const optional_storage& other) {}

			#if EASTL_VARIADIC_TEMPLATES_ENABLED
				template <class... Args>
				inline explicit optional_storage(in_place_t, Args&&... args)
					: val(eastl::forward<Args>(args)...), engaged(true) {}

				template <typename U,
						  typename... Args,
						  typename = typename eastl::enable_if<
							  eastl::is_constructible<T, std::initializer_list<U>&, Args&&...>::value>::type>
				inline explicit optional_storage(in_place_t, std::initializer_list<U> ilist, Args&&... args)
					: val(ilist, eastl::forward<Args>(args)...), engaged(true) {}
			#endif

			inline void destruct_value() {}  // no implementation necessary since T is trivially destructible.


			// This union exists to support trivial types that do not require constructors/destructors to be called.
			// The eastl::optional<T> type will set the empty_val in this case to "initialize" its member data. 
			union
			{
				value_type val; // consider making this aligned storage.
				char empty_val;
			};
			bool engaged = false;
		};
	}


	///////////////////////////////////////////////////////////////////////////////
	/// optional
	///
    template <typename T>
    class optional : private Internal::optional_storage<T>
    {
		typedef Internal::optional_storage<T> base_type;

		using base_type::destruct_value;
		using base_type::engaged;
		using base_type::val;

	public:
		typedef T value_type;

	    // (ISOCPP 20.6.3) A program that necessitates the instantiation of template optional for a reference type, or
	    // for possibly cv-qualified types in_place_t or nullopt_t is ill-formed.
	    static_assert(!eastl::is_reference<value_type>::value, "eastl::optional of a reference type is ill-formed");
		static_assert(!eastl::is_same<value_type, in_place_t>::value, "eastl::optional of a in_place_t type is ill-formed");
		static_assert(!eastl::is_same<value_type, nullopt_t>::value, "eastl::optional of a nullopt_t type is ill-formed");

	    inline optional() EA_NOEXCEPT {}
	    inline optional(nullopt_t) EA_NOEXCEPT {}
	    inline optional(const value_type& value) : base_type(value) {}
	    inline optional(value_type&& value) EA_NOEXCEPT_IF(eastl::is_nothrow_move_constructible<T>::value)
	        : base_type(eastl::move(value)) {}

	    optional(const optional& other) = default; 
	    optional(optional&& other) = default; 

		#if EASTL_VARIADIC_TEMPLATES_ENABLED 
			template <typename... Args>
			inline explicit optional(in_place_t, Args&&... args)
				: base_type(in_place, eastl::forward<Args>(args)...) {}

	        template <typename U,
	                  typename... Args,
	                  typename = typename eastl::enable_if<
	                      eastl::is_constructible<T, std::initializer_list<U>&, Args&&...>::value>::type>
	        inline explicit optional(in_place_t, std::initializer_list<U> ilist, Args&&... args)
	            : base_type(in_place, ilist, eastl::forward<Args>(args)...) {}
        #endif

	    inline optional& operator=(nullopt_t)
	    {
		    reset();
		    return *this;
	    }

	    inline optional& operator=(const optional& other) 
		{
			engaged = other.engaged;
			val = other.val;
			return *this;
		}

	    inline optional& operator=(optional&& other)
	        EA_NOEXCEPT_IF(EA_NOEXCEPT(eastl::is_nothrow_move_assignable<value_type>::value &&
	                                       eastl::is_nothrow_move_constructible<value_type>::value))
	    {
			engaged = other.engaged;
			val = eastl::move(other.val);
		    return *this;
	    }

	    template <class U, typename = typename eastl::enable_if<eastl::is_same<eastl::decay_t<U>, T>::value>::type>
	    inline optional& operator=(U&& u)
	    {
			if(engaged)
			{
				val = eastl::forward<U>(u);
			}
			else
			{
				engaged = true;
				construct_value(eastl::forward<U>(u));
			}

		    return *this;
	    }

	    inline explicit operator bool() const { return engaged; }

	    template <class U>
	    inline value_type value_or(U&& default_value) const
			{ return engaged ? val : static_cast<value_type>(eastl::forward<U>(default_value)); }

	    template <class U>
	    inline value_type value_or(U&& default_value)
			{ return engaged ? val : static_cast<value_type>(eastl::forward<U>(default_value)); }

		inline const T& value()&              { return get_value_ref(); }
		inline const T& value() const&        { return get_value_ref(); }
		inline T&& value()&&                  { return get_value_ref(); }
		inline const T&& value() const&&      { return get_value_ref(); }

	    inline T* operator->()                { return get_value_address(); }
	    inline const T* operator->() const    { return get_value_address(); }
	    inline T& operator*()&                { return get_value_ref(); }
		inline T&& operator*()&&              { return get_value_ref(); }
	    inline const T& operator*() const&    { return get_value_ref(); }
		inline const T&& operator*() const&&  { return get_value_ref(); }

		#if EASTL_VARIADIC_TEMPLATES_ENABLED
			template <class... Args>
			void emplace(Args&&... args)
			{
				construct_value(eastl::move(T(eastl::forward<Args>(args)...)));
				engaged = true;
			}

			template <class U, class... Args>
			void emplace(std::initializer_list<U> ilist, Args&&... args)
			{
				construct_value(eastl::move(T(ilist, eastl::forward<Args>(args)...)));
				engaged = true;
			}
        #endif

	    inline void swap(optional& other)
	        EA_NOEXCEPT_IF(eastl::is_nothrow_move_constructible<T>::value&& eastl::is_nothrow_swappable<T>::value)
	    {
		    using eastl::swap;
		    if (engaged == other.engaged)
		    {
			    if (engaged)
					swap(**this, *other);
		    }
		    else
		    {
			    swap(engaged, other.engaged);
			    if (engaged)
			    {
					other.construct_value(eastl::move(val));
					destruct_value();
			    }
			    else
			    {
					construct_value(eastl::move(other.val));
				    other.destruct_value();
			    }
		    }
	    }

		inline void reset()
		{
			if (engaged)
			{
				destruct_value();
				engaged = false;
			}
		}

	private:

	    inline void construct_value(const value_type& v)
			{ ::new (eastl::addressof(val)) value_type(v); }

	    inline void construct_value(value_type&& v)
			{ ::new (eastl::addressof(val)) value_type(eastl::move(v)); }

	    inline T* get_value_address() EASTL_OPTIONAL_NOEXCEPT
	    {
            #if EASTL_EXCEPTIONS_ENABLED
				if(!engaged) 
					throw bad_optional_access();
			#elif EASTL_ASSERT_ENABLED
				EASTL_ASSERT_MSG(engaged, "no value to retrieve");
			#endif
			return eastl::addressof(val);
	    }

	    inline const T* get_value_address() const EASTL_OPTIONAL_NOEXCEPT
	    {
            #if EASTL_EXCEPTIONS_ENABLED
				if(!engaged) 
					throw bad_optional_access();
			#elif EASTL_ASSERT_ENABLED
				EASTL_ASSERT_MSG(engaged, "no value to retrieve");
			#endif
			return eastl::addressof(val);
	    }

	    inline value_type& get_value_ref() EASTL_OPTIONAL_NOEXCEPT
	    {
            #if EASTL_EXCEPTIONS_ENABLED
				if(!engaged) 
					throw bad_optional_access();
			#elif EASTL_ASSERT_ENABLED
				EASTL_ASSERT_MSG(engaged, "no value to retrieve");
			#endif
		    return val;
	    }

	    inline const value_type& get_value_ref() const EASTL_OPTIONAL_NOEXCEPT
	    {
            #if EASTL_EXCEPTIONS_ENABLED
				if(!engaged) 
					throw bad_optional_access();
			#elif EASTL_ASSERT_ENABLED
				EASTL_ASSERT_MSG(engaged, "no value to retrieve");
			#endif
		    return val;
	    }
    };


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
		// NOTE: 
		//
		// Code collapsed onto a single line to satisfy requirements for constexpr expressions
		// being a single line return statement.
		//
		// if(bool(lhs) != bool(rhs))
		//     return false;

		// if(bool(lhs) == false)
		//     return true;

		// return *lhs == *rhs;

		return (bool(lhs) != bool(rhs)) ? false : (bool(lhs) == false) ? true : *lhs == *rhs;
	}

	template <class T>
	inline EA_CONSTEXPR bool operator<(const optional<T>& lhs, const optional<T>& rhs)
	{
		// NOTE: 
		//
		// Code collapsed onto a single line to satisify requirements for constexpr expressions
		// being a single line return statement.
		//
		// if (!bool(rhs))
		//     return false;

		// if (!bool(lhs))
		//     return true;

		//  return *lhs < *rhs;

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


    ///////////////////////////////////////////////////////////////////////////////
	// Compare an optional object with a nullopt
	//
    template <class T>
    inline EA_CONSTEXPR bool operator==(const optional<T>& opt, eastl::nullopt_t) EA_NOEXCEPT
		{ return !opt; }

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
    inline EA_CONSTEXPR bool operator<(const optional<T>& opt, eastl::nullopt_t) EA_NOEXCEPT
		{ return false; }

    template <class T>
    inline EA_CONSTEXPR bool operator<(eastl::nullopt_t, const optional<T>& opt) EA_NOEXCEPT
		{ return bool(opt); }

    template <class T>
    inline EA_CONSTEXPR bool operator<=(const optional<T>& opt, eastl::nullopt_t) EA_NOEXCEPT
		{ return !opt; }

    template <class T>
    inline EA_CONSTEXPR bool operator<=(eastl::nullopt_t, const optional<T>& opt) EA_NOEXCEPT
		{ return true; }

    template <class T>
    inline EA_CONSTEXPR bool operator>(const optional<T>& opt, eastl::nullopt_t) EA_NOEXCEPT
		{ return bool(opt); }

    template <class T>
    inline EA_CONSTEXPR bool operator>(eastl::nullopt_t, const optional<T>& opt) EA_NOEXCEPT
		{ return false; }

    template <class T>
    inline EA_CONSTEXPR bool operator>=(const optional<T>& opt, eastl::nullopt_t) EA_NOEXCEPT
		{ return true; }

    template <class T>
    inline EA_CONSTEXPR bool operator>=(eastl::nullopt_t, const optional<T>& opt) EA_NOEXCEPT
		{ return !opt; }


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


    ///////////////////////////////////////////////////////////////////////////////
	/// hash 
	///
	template <typename T>
	struct hash<eastl::optional<T>>
	{
		typedef eastl::optional<T> argument_type;
		typedef size_t result_type;

	    result_type operator()(const argument_type& opt) const EA_NOEXCEPT
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
    inline optional<typename eastl::decay<T>::type> make_optional(T&& value)
    {
	    return optional<typename eastl::decay<T>::type>(eastl::forward<T>(value));
    }

	#undef EASTL_OPTIONAL_NOEXCEPT 

}  // namespace eastl

EA_RESTORE_VC_WARNING()

#endif  // EASTL_OPTIONAL_ENABLED 
#endif  // EASTL_OPTIONAL_H
