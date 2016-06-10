///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_UTILITY_H
#define EASTL_UTILITY_H


#include <EASTL/internal/config.h>
#include <EASTL/type_traits.h>
#include <EASTL/iterator.h>
#include <EASTL/functional.h>
#include <EASTL/internal/move_help.h>
#include <EABase/eahave.h>

#include <EASTL/internal/integer_sequence.h>
#include <EASTL/internal/tuple_fwd_decls.h>
#include <EASTL/internal/in_place_t.h>

#ifdef _MSC_VER
	#pragma warning(push)           // VC++ generates a bogus warning that you cannot code away.
	#pragma warning(disable: 4619)  // There is no warning number 'number'.
	#pragma warning(disable: 4217)  // Member template functions cannot be used for copy-assignment or copy-construction.
	#pragma warning(disable: 4512)  // 'class' : assignment operator could not be generated.  // This disabling would best be put elsewhere.
#endif

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// swap
	///
	/// Assigns the contents of a to b and the contents of b to a. 
	/// A temporary instance of type T is created and destroyed
	/// in the process.
	///
	/// This function is used by numerous other algorithms, and as 
	/// such it may in some cases be feasible and useful for the user 
	/// to implement an override version of this function which is 
	/// more efficient in some way. 
	///

	template <typename T> 
	inline void swap(T& a, T& b) EA_NOEXCEPT_IF(eastl::is_nothrow_move_constructible<T>::value && eastl::is_nothrow_move_assignable<T>::value)
	{
		T temp(EASTL_MOVE(a));  // EASTL_MOVE uses EASTL::move when available, else is a no-op.
		a = EASTL_MOVE(b);
		b = EASTL_MOVE(temp);
	}


	/// is_swappable
	///
	/// Determines if two types can be swapped via the swap function. This determines
	/// only if there is a swap function that matches the types and not if the assignments
	/// within the swap implementation are valid.
	/// Returns false for pre-C++11 compilers that don't support decltype.
	///
	/// This is a type trait, but it's not currently found within <type_traits.h>,
	/// as it's dependent on the swap algorithm, which is at a higher level than
	/// type traits.
	///
	/// Example usage:
	///     static_assert(is_swappable<int>::value, "int should be swappable");
	///
	#if defined(EA_COMPILER_NO_DECLTYPE)
		#define EASTL_TYPE_TRAIT_is_swappable_CONFORMANCE 0

		template <typename>
		struct is_swappable
			: public eastl::false_type {};
	#else
		#define EASTL_TYPE_TRAIT_is_swappable_CONFORMANCE 1

		// We declare this version of 'eastl::swap' to make compile-time existance checks for swap functions possible.  
		//
		#if EASTL_VARIADIC_TEMPLATES_ENABLED
			eastl::unused swap(eastl::argument_sink, eastl::argument_sink);
		#else
			// Compilers that do not support variadic templates suffer from a bug with variable arguments list that
			// causes the construction of aligned types in unaligned memory. To prevent the aligned type construction we
			// accept the parameters by reference.
			eastl::unused swap(eastl::argument_sink&, eastl::argument_sink&);
		#endif

		template <typename T>
		struct is_swappable
			: public integral_constant<bool, !eastl::is_same<decltype(swap(eastl::declval<T&>(), eastl::declval<T&>())), eastl::unused>::value> {}; // Don't prefix swap with eastl:: as we want to allow user-defined swaps via argument-dependent lookup.
	#endif
	
	#if EASTL_VARIABLE_TEMPLATES_ENABLED
        template <class T>
        EA_CONSTEXPR bool is_swappable_v = is_swappable<T>::value;
    #endif



	/// is_nothrow_swappable
	///
	/// Evaluates to true if is_swappable, and swap is a nothrow function.
	/// returns false for pre-C++11 compilers that don't support nothrow.
	///
	/// This is a type trait, but it's not currently found within <type_traits.h>,
	/// as it's dependent on the swap algorithm, which is at a higher level than
	/// type traits.
	///
	#if defined(EA_COMPILER_NO_DECLTYPE) || defined(EA_COMPILER_NO_NOEXCEPT) || defined(__EDG_VERSION__) // EDG mis-compiles the conforming code below and so must be placed here.
		#define EASTL_TYPE_TRAIT_is_nothrow_swappable_CONFORMANCE 0

		template <typename>
		struct is_nothrow_swappable
			: public eastl::false_type {};
	#else
		#define EASTL_TYPE_TRAIT_is_nothrow_swappable_CONFORMANCE EASTL_TYPE_TRAIT_is_swappable_CONFORMANCE

		template <typename T>
		struct is_nothrow_swappable_helper_noexcept_wrapper
			{ const static bool value = noexcept(swap(eastl::declval<T&>(), eastl::declval<T&>())); };

		template <typename T, bool>
		struct is_nothrow_swappable_helper
			: public eastl::integral_constant<bool, is_nothrow_swappable_helper_noexcept_wrapper<T>::value> {}; // Don't prefix swap with eastl:: as we want to allow user-defined swaps via argument-dependent lookup.

		template <typename T>
		struct is_nothrow_swappable_helper<T, false>
			: public eastl::false_type {};

		template <typename T>
		struct is_nothrow_swappable
			: public eastl::is_nothrow_swappable_helper<T, eastl::is_swappable<T>::value> {};
	#endif

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
        template <class T>
        EA_CONSTEXPR bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;
    #endif


	
	/// is_swappable_with
	///
	///
	template <typename T, typename U, bool OneTypeIsVoid = (eastl::is_void<T>::value || eastl::is_void<U>::value)>
	struct is_swappable_with_helper
	{
		// Don't prefix swap with eastl:: as we want to allow user-defined swaps via argument-dependent lookup.
	    static const bool value =
	        !eastl::is_same<decltype(swap(eastl::declval<T>(), eastl::declval<U>())), eastl::unused>::value &&
	        !eastl::is_same<decltype(swap(eastl::declval<U>(), eastl::declval<T>())), eastl::unused>::value;
    };

	template <typename T, typename U>
	struct is_swappable_with_helper<T,U, true> { static const bool value = false; };

    template<typename T, typename U>
	struct is_swappable_with 
			: public eastl::bool_constant<is_swappable_with_helper<T, U>::value> {}; 

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
        template <class T, class U>
        EA_CONSTEXPR bool is_swappable_with_v = is_swappable_with<T, U>::value;
    #endif


	
	/// is_nothrow_swappable_with
	///
	///
	#if defined(EA_COMPILER_NO_DECLTYPE) || defined(EA_COMPILER_NO_NOEXCEPT) 
		#define EASTL_TYPE_TRAIT_is_nothrow_swappable_with_CONFORMANCE 0
		template <typename T, typename U>
		struct is_nothrow_swappable_with_helper { static const bool value = false; };
	#else
		#define EASTL_TYPE_TRAIT_is_nothrow_swappable_with_CONFORMANCE 1
		template <typename T, typename U, bool OneTypeIsVoid = (eastl::is_void<T>::value || eastl::is_void<U>::value)>
		struct is_nothrow_swappable_with_helper
		{
	        // Don't prefix swap with eastl:: as we want to allow user-defined swaps via argument-dependent lookup.
	        static const bool value = noexcept(swap(eastl::declval<T>(), eastl::declval<U>())) &&
	                                  noexcept(swap(eastl::declval<U>(), eastl::declval<T>()));
        };

		template <typename T, typename U>
		struct is_nothrow_swappable_with_helper<T,U, true> { static const bool value = false; };
	#endif

    template <typename T, typename U>
    struct is_nothrow_swappable_with : public eastl::bool_constant<is_nothrow_swappable_with_helper<T, U>::value> {};

    #if EASTL_VARIABLE_TEMPLATES_ENABLED
        template <class T, class U>
        EA_CONSTEXPR bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T, U>::value;
    #endif
	


	// iter_swap helper functions
	//
	template <bool bTypesAreEqual>
	struct iter_swap_impl
	{
		// Handles the false case, where *a and *b are different types.
		template <typename ForwardIterator1, typename ForwardIterator2>
		static void iter_swap(ForwardIterator1 a, ForwardIterator2 b)
		{
			typedef typename eastl::iterator_traits<ForwardIterator1>::value_type value_type_a;

			value_type_a temp(EASTL_MOVE(*a)); // EASTL_MOVE uses EASTL::move when available, else is a no-op.
			*a = EASTL_MOVE(*b);
			*b = EASTL_MOVE(temp); 
		}
	};

	template <>
	struct iter_swap_impl<true>
	{
		template <typename ForwardIterator1, typename ForwardIterator2>
		static void iter_swap(ForwardIterator1 a, ForwardIterator2 b)
		{
			swap(*a, *b);  // Don't prefix swap with eastl:: as we want to allow user-defined swaps via argument-dependent lookup.
		}
	};


	/// iter_swap
	///
	/// Swaps the values of the elements the given iterators are pointing to. 
	///
	/// Equivalent to swap(*a, *b), though the user can provide an override to
	/// iter_swap that is independent of an override which may exist for swap.
	///
	/// We provide a version of iter_swap which uses swap when the swapped types 
	/// are equal but a manual implementation otherwise. We do this because the 
	/// C++ standard defect report says that iter_swap(a, b) must be implemented 
	/// as swap(*a, *b) when possible.
	///
	template <typename ForwardIterator1, typename ForwardIterator2>
	inline void iter_swap(ForwardIterator1 a, ForwardIterator2 b)
	{
		typedef typename eastl::iterator_traits<ForwardIterator1>::value_type value_type_a;
		typedef typename eastl::iterator_traits<ForwardIterator2>::value_type value_type_b;
		typedef typename eastl::iterator_traits<ForwardIterator1>::reference  reference_a;
		typedef typename eastl::iterator_traits<ForwardIterator2>::reference  reference_b;

		eastl::iter_swap_impl<eastl::type_and<eastl::is_same<value_type_a, value_type_b>::value, eastl::is_same<value_type_a&, reference_a>::value, eastl::is_same<value_type_b&, reference_b>::value >::value >::iter_swap(a, b);
	}



	/// swap_ranges
	///
	/// Swaps each of the elements in the range [first1, last1) with the 
	/// corresponding element in the range [first2, first2 + (last1 - first1)). 
	///
	/// Effects: For each nonnegative integer n < (last1 - first1),
	/// performs: swap(*(first1 + n), *(first2 + n)).
	///
	/// Requires: The two ranges [first1, last1) and [first2, first2 + (last1 - first1))
	/// shall not overlap.
	///
	/// Returns: first2 + (last1 - first1). That is, returns the end of the second range.
	///
	/// Complexity: Exactly 'last1 - first1' swaps.
	///
	template <typename ForwardIterator1, typename ForwardIterator2>
	inline ForwardIterator2
	swap_ranges(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2)
	{
		for(; first1 != last1; ++first1, ++first2)
			iter_swap(first1, first2); // Don't prefix swap with eastl:: as we want to allow user-defined swaps via argument-dependent lookup.
		return first2;
	}


	/// swap
	///
	/// C++11 array swap
	/// http://en.cppreference.com/w/cpp/algorithm/swap
	///
	template <typename T, size_t N>
	inline void
	swap(T (&a)[N], T (&b)[N]) EA_NOEXCEPT_IF(eastl::is_nothrow_swappable<T>::value)
	{
		eastl::swap_ranges(a, a + N, b);
	}

#if EASTL_MOVE_SEMANTICS_ENABLED

	/// exchange
	///
	/// Replaces the value of the first argument with the new value provided.  
	/// The return value is the previous value of first argument.
	///
	/// http://en.cppreference.com/w/cpp/utility/exchange
	///
	#if defined(EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS)
		template <typename T, typename U>
		inline T exchange(T& obj, U&& new_value, typename eastl::enable_if<eastl::is_convertible<U, T>::value>::type* = 0)
		{
			T old_value = eastl::move(obj);
			obj = eastl::forward<U>(new_value);
			return old_value;
		}
	#else
		template <typename T, typename U = T>
		inline T exchange(T& obj, U&& new_value)
		{
			T old_value = eastl::move(obj);
			obj = eastl::forward<U>(new_value);
			return old_value;
		}
	#endif

#endif // EASTL_MOVE_SEMANTICS_ENABLED

	/// as_const
	///
	/// Converts a 'T&' into a 'const T&' which simplifies calling const functions on non-const objects. 
	///
	/// http://en.cppreference.com/w/cpp/utility/as_const
	///
	/// C++ proposal paper:
	/// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4380.html
	///
	template <class T>
	EA_CONSTEXPR typename eastl::add_const<T>::type& as_const(T& t) EA_NOEXCEPT
		{ return t; }

	// The C++17 forbids 'eastl::as_const' from accepting rvalues.  Passing an rvalue reference to 'eastl::as_const'
	// generates an 'const T&' or const lvalue reference to a temporary object. 
	#if !defined(EA_COMPILER_NO_DELETED_FUNCTIONS)
		template <class T>
		void as_const(const T&&) = delete;
	#endif


    ///////////////////////////////////////////////////////////////////////
	/// rel_ops
	///
	/// rel_ops allow the automatic generation of operators !=, >, <=, >= from
	/// just operators == and <. These are intentionally in the rel_ops namespace
	/// so that they don't conflict with other similar operators. To use these 
	/// operators, add "using namespace std::rel_ops;" to an appropriate place in 
	/// your code, usually right in the function that you need them to work.
	/// In fact, you will very likely have collision problems if you put such
	/// using statements anywhere other than in the .cpp file like so and may 
	/// also have collisions when you do, as the using statement will affect all
	/// code in the module. You need to be careful about use of rel_ops.
	///
	namespace rel_ops
	{
		template <typename T>
		inline bool operator!=(const T& x, const T& y)
			{ return !(x == y); }

		template <typename T>
		inline bool operator>(const T& x, const T& y)
			{ return (y < x); }

		template <typename T>
		inline bool operator<=(const T& x, const T& y)
			{ return !(y < x); }

		template <typename T>
		inline bool operator>=(const T& x, const T& y)
			{ return !(x < y); }
	}



	///////////////////////////////////////////////////////////////////////
	/// pair
	///
	/// Implements a simple pair, just like the C++ std::pair.
	///
	template <typename T1, typename T2>
	struct pair
	{
		typedef T1           first_type;
		typedef T2           second_type;
		typedef pair<T1, T2> this_type;

		T1 first;
		T2 second;

		EA_CONSTEXPR pair()
			: first(), 
			  second() {}

		// To consider: Use type traits to enable this ctor only if T2 (second is_default_constructible<T2>::value ==
		// true.)
		EA_CPP14_CONSTEXPR pair(const T1& x)
			: first(x),
			  second()
		{
		}

		EA_CPP14_CONSTEXPR pair(const T1& x, const T2& y) : first(x), second(y) {}

		template <typename U, typename V> // This would enable_if situation be better solvable with C++14 concepts, but such compilers aren't currently available.
		EA_CPP14_CONSTEXPR pair(const pair<U, V>& p, typename eastl::enable_if<eastl::is_convertible<const U&, T1>::value && eastl::is_convertible<const V&, T2>::value>::type* = 0)
			: first(p.first), 
			  second(p.second) {}

		#if (EABASE_VERSION_N >= 20040) && !defined(EA_COMPILER_NO_DEFAULTED_FUNCTIONS) 
			pair(const pair&) = default;
		#endif

		#if EASTL_MOVE_SEMANTICS_ENABLED
		  #if defined(EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS)
			template <typename U, typename V>
			EA_CPP14_CONSTEXPR pair(U&& u, V&& v, typename eastl::enable_if<eastl::is_convertible<U, T1>::value && eastl::is_convertible<V, T2>::value>::type* = 0)
				: first(eastl::forward<U>(u)), 
				  second(eastl::forward<V>(v)) {}

			template <typename U>
			EA_CPP14_CONSTEXPR pair(U&& x, const T2& y, typename eastl::enable_if<eastl::is_convertible<U, T1>::value>::type* = 0)
				: first(eastl::forward<U>(x)), 
				  second(y) {}

			template <typename V>
			EA_CPP14_CONSTEXPR pair(const T1& x, V&& y, typename eastl::enable_if<eastl::is_convertible<V, T2>::value>::type* = 0)
				: first(x), 
				  second(eastl::forward<V>(y)) {}
		  #else
			template <typename U, typename V, typename = typename eastl::enable_if<eastl::is_convertible<U, T1>::value && eastl::is_convertible<V, T2>::value>::type>
			EA_CPP14_CONSTEXPR pair(U&& u, V&& v)
				: first(eastl::forward<U>(u)), 
				  second(eastl::forward<V>(v)) {}

			template <typename U, typename = typename eastl::enable_if<eastl::is_convertible<U, T1>::value>::type>
			EA_CPP14_CONSTEXPR pair(U&& x, const T2& y)
				: first(eastl::forward<U>(x)), 
				  second(y) {}

			template <typename V, typename = typename eastl::enable_if<eastl::is_convertible<V, T2>::value>::type>
			EA_CPP14_CONSTEXPR pair(const T1& x, V&& y)
				: first(x), 
				  second(eastl::forward<V>(y)) {}
		  #endif

			#if (EABASE_VERSION_N >= 20040) && !defined(EA_COMPILER_NO_DEFAULTED_FUNCTIONS) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1900)
				pair(pair&& p) = default;
			#else
				// The C++11 Standard specifies the following as a defaulted function, but we cannot do that because
				// otherwise we get GCC and MSVC compile errors in the rest of EASTL saying that it's a needed but implicitly deleted function.
				EA_CPP14_CONSTEXPR pair(pair&& p) EA_NOEXCEPT_IF(eastl::is_nothrow_move_constructible<T1>::value && eastl::is_nothrow_move_constructible<T2>::value)
					: first(eastl::forward<T1>(p.first)), 
					  second(eastl::forward<T2>(p.second)) {}
			#endif


		  #if defined(EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS)
			template <typename U, typename V>
			EA_CPP14_CONSTEXPR pair(pair<U, V>&& p, typename eastl::enable_if<eastl::is_convertible<U, T1>::value && eastl::is_convertible<V, T2>::value>::type* = 0)
				: first(eastl::forward<U>(p.first)), 
				  second(eastl::forward<V>(p.second)) {}
		  #else
			template <typename U, typename V, typename = typename eastl::enable_if<eastl::is_convertible<U, T1>::value && eastl::is_convertible<V, T2>::value>::type>
			EA_CPP14_CONSTEXPR pair(pair<U, V>&& p)
				: first(eastl::forward<U>(p.first)), 
				  second(eastl::forward<V>(p.second)) {}
		  #endif
		#endif


		pair& operator=(const pair& p) EA_NOEXCEPT_IF(eastl::is_nothrow_copy_assignable<T1>::value && eastl::is_nothrow_copy_assignable<T2>::value)
		{
			first  = p.first;
			second = p.second;
			return *this;
		}


	  #if defined(EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS)
		template <typename U, typename V>
	  #else
		template <typename U, typename V, typename = typename eastl::enable_if<eastl::is_convertible<U, T1>::value && eastl::is_convertible<V, T2>::value>::type>
	  #endif
		pair& operator=(const pair<U, V>& p)
		{
			first  = p.first;
			second = p.second;
			return *this;
		}


		#if EASTL_MOVE_SEMANTICS_ENABLED
			pair& operator=(pair&& p) EA_NOEXCEPT_IF(eastl::is_nothrow_move_assignable<T1>::value && eastl::is_nothrow_move_assignable<T2>::value)
			{
				first  = eastl::forward<T1>(p.first);
				second = eastl::forward<T2>(p.second);
				return *this;
			}

		  #if defined(EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS)
			template <typename U, typename V>
		  #else
			template <typename U, typename V, typename = typename eastl::enable_if<eastl::is_convertible<U, T1>::value && eastl::is_convertible<V, T2>::value>::type>
		  #endif
			pair& operator=(pair<U, V>&& p)
			{
				first  = eastl::forward<U>(p.first);
				second = eastl::forward<V>(p.second);
				return *this;
			}
		#endif


		void swap(pair& p) EA_NOEXCEPT_IF(eastl::is_nothrow_swappable<T1>::value && eastl::is_nothrow_swappable<T2>::value)
		{
			eastl::iter_swap(&first,  &p.first);
			eastl::iter_swap(&second, &p.second);
		}
	};

	#if defined(EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS) || !EASTL_MOVE_SEMANTICS_ENABLED // We aren't concerned about the noexcept conformance above.
		#define EASTL_PAIR_CONFORMANCE 0 
	#else
		#define EASTL_PAIR_CONFORMANCE 1
	#endif



	/// use_self
	///
	/// operator()(x) simply returns x. Used in sets, as opposed to maps.
	/// This is a template policy implementation; it is an alternative to 
	/// the use_first template implementation.
	///
	/// The existance of use_self may seem odd, given that it does nothing,
	/// but these kinds of things are useful, virtually required, for optimal 
	/// generic programming.
	///
	template <typename T>
	struct use_self             // : public unary_function<T, T> // Perhaps we want to make it a subclass of unary_function.
	{
		typedef T result_type;

		const T& operator()(const T& x) const
			{ return x; }
	};

	/// use_first
	///
	/// operator()(x) simply returns x.first. Used in maps, as opposed to sets.
	/// This is a template policy implementation; it is an alternative to 
	/// the use_self template implementation. This is the same thing as the
	/// SGI SGL select1st utility.
	///
	template <typename Pair>
	struct use_first            // : public unary_function<Pair, typename Pair::first_type> // Perhaps we want to make it a subclass of unary_function.
	{
		typedef Pair argument_type;
		typedef typename Pair::first_type result_type;

		const result_type& operator()(const Pair& x) const
			{ return x.first; }
	};

	/// use_second
	///
	/// operator()(x) simply returns x.second. 
	/// This is the same thing as the SGI SGL select2nd utility
	///
	template <typename Pair>
	struct use_second           // : public unary_function<Pair, typename Pair::second_type> // Perhaps we want to make it a subclass of unary_function.
	{
		typedef Pair argument_type;
		typedef typename Pair::second_type result_type;

		const result_type& operator()(const Pair& x) const
			{ return x.second; }
	};





	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename T1, typename T2>
	EA_CPP14_CONSTEXPR inline bool operator==(const pair<T1, T2>& a, const pair<T1, T2>& b)
	{
		return ((a.first == b.first) && (a.second == b.second));
	}


	template <typename T1, typename T2>
	EA_CPP14_CONSTEXPR inline bool operator<(const pair<T1, T2>& a, const pair<T1, T2>& b)
	{
		// Note that we use only operator < in this expression. Otherwise we could
		// use the simpler: return (a.m1 == b.m1) ? (a.m2 < b.m2) : (a.m1 < b.m1);
		// The user can write a specialization for this operator to get around this
		// in cases where the highest performance is required.
		return ((a.first < b.first) || (!(b.first < a.first) && (a.second < b.second)));
	}


	template <typename T1, typename T2>
	EA_CPP14_CONSTEXPR inline bool operator!=(const pair<T1, T2>& a, const pair<T1, T2>& b)
	{
		return !(a == b);
	}


	template <typename T1, typename T2>
	EA_CPP14_CONSTEXPR inline bool operator>(const pair<T1, T2>& a, const pair<T1, T2>& b)
	{
		return b < a;
	}


	template <typename T1, typename T2>
	EA_CPP14_CONSTEXPR inline bool operator>=(const pair<T1, T2>& a, const pair<T1, T2>& b)
	{
		return !(a < b);
	}


	template <typename T1, typename T2>
	EA_CPP14_CONSTEXPR inline bool operator<=(const pair<T1, T2>& a, const pair<T1, T2>& b)
	{
		return !(b < a);
	}




	///////////////////////////////////////////////////////////////////////
	/// make_pair / make_pair_ref
	///
	/// make_pair is the same as std::make_pair specified by the C++ standard.
	/// If you look at the C++ standard, you'll see that it specifies T& instead of T.
	/// However, it has been determined that the C++ standard is incorrect and has 
	/// flagged it as a defect (http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#181).
	/// In case you feel that you want a more efficient version that uses references,
	/// we provide the make_pair_ref function below, though C++11 move support
	/// makes that no longer necessary.
	/// 
	/// Note: You don't usually need to use make_pair in order to make a pair. 
	/// The following code is equivalent, and the latter avoids one more level of inlining:
	///     return make_pair(charPtr, charPtr);
	///     return pair<char*, char*>(charPtr, charPtr);
	///
	#if EASTL_MOVE_SEMANTICS_ENABLED

		template <typename T1, typename T2>
		EA_CPP14_CONSTEXPR inline pair<typename eastl::remove_reference_wrapper<typename eastl::decay<T1>::type>::type, 
									   typename eastl::remove_reference_wrapper<typename eastl::decay<T2>::type>::type>
		make_pair(T1&& a, T2&& b)
		{
			typedef typename eastl::remove_reference_wrapper<typename eastl::decay<T1>::type>::type T1Type;
			typedef typename eastl::remove_reference_wrapper<typename eastl::decay<T2>::type>::type T2Type;

			return eastl::pair<T1Type, T2Type>(eastl::forward<T1>(a), eastl::forward<T2>(b));
		}


		// Without the following, VC++ fails to compile code like this: pair<const char*, int> p = eastl::make_pair<const char*, int>("hello", 0);
		// We define a const reference version alternative to the above. "hello" is of type char const(&)[6] (array of 6 const chars), 
		// but VC++ decays it to const char* and allows this make_pair to be called with that. VC++ fails below with make_pair("hello", "people") 
		// because you can't assign arrays and until we have a better solution we just disable this make_pair specialization for when T1 or T2 
		// are of type char const(&)[].
		#if defined(_MSC_VER)
			template <typename T1, typename T2>
			EA_CPP14_CONSTEXPR inline pair<T1, T2> make_pair(const T1& a, const T2& b, typename eastl::enable_if<!eastl::is_array<T1>::value && !eastl::is_array<T2>::value>::type* = 0)
			{
				return eastl::pair<T1, T2>(a, b);
			}
		#endif


		// For backwards compatibility
		template <typename T1, typename T2>
		EA_CPP14_CONSTEXPR inline pair<typename eastl::remove_reference_wrapper<typename eastl::decay<T1>::type>::type, 
									   typename eastl::remove_reference_wrapper<typename eastl::decay<T2>::type>::type>
		make_pair_ref(T1&& a, T2&& b)
		{
			typedef typename eastl::remove_reference_wrapper<typename eastl::decay<T1>::type>::type T1Type;
			typedef typename eastl::remove_reference_wrapper<typename eastl::decay<T2>::type>::type T2Type;

			return eastl::pair<T1Type, T2Type>(eastl::forward<T1>(a), eastl::forward<T2>(b));
		}
	#else

		template <typename T1, typename T2>
		inline pair<T1, T2> make_pair(T1 a, T2 b)
		{
			return eastl::pair<T1, T2>(a, b);
		}


		template <typename T1, typename T2>
		inline pair<T1, T2> make_pair_ref(const T1& a, const T2& b)
		{
			return eastl::pair<T1, T2>(a, b);
		}

	#endif

#if EASTL_TUPLE_ENABLED

		template <typename T1, typename T2>
		class tuple_size<pair<T1, T2>> : public integral_constant<size_t, 2>
		{
		};

		template <typename T1, typename T2>
		class tuple_size<const pair<T1, T2>> : public integral_constant<size_t, 2>
		{
		};

		template <typename T1, typename T2>
		class tuple_element<0, pair<T1, T2>>
		{
		public:
			typedef T1 type;
		};

		template <typename T1, typename T2>
		class tuple_element<1, pair<T1, T2>>
		{
		public:
			typedef T2 type;
		};

		template <typename T1, typename T2>
		class tuple_element<0, const pair<T1, T2>>
		{
		public:
			typedef const T1 type;
		};

		template <typename T1, typename T2>
		class tuple_element<1, const pair<T1, T2>>
		{
		public:
			typedef const T2 type;
		};

		template <size_t I>
		struct GetPair;

		template <>
		struct GetPair<0>
		{
			template <typename T1, typename T2>
			static EA_CONSTEXPR T1& getInternal(pair<T1, T2>& p)
			{
				return p.first;
			}

			template <typename T1, typename T2>
			static EA_CONSTEXPR const T1& getInternal(const pair<T1, T2>& p)
			{
				return p.first;
			}

			template <typename T1, typename T2>
			static EA_CONSTEXPR T1&& getInternal(pair<T1, T2>&& p)
			{
				return forward<T1>(p.first);
			}
		};

		template <>
		struct GetPair<1>
		{
			template <typename T1, typename T2>
			static EA_CONSTEXPR T2& getInternal(pair<T1, T2>& p)
			{
				return p.second;
			}

			template <typename T1, typename T2>
			static EA_CONSTEXPR const T2& getInternal(const pair<T1, T2>& p)
			{
				return p.second;
			}

			template <typename T1, typename T2>
			static EA_CONSTEXPR T2&& getInternal(pair<T1, T2>&& p)
			{
				return forward<T2>(p.second);
			}
		};

		template <size_t I, typename T1, typename T2>
		tuple_element_t<I, pair<T1, T2>>& get(pair<T1, T2>& p)
		{
			return GetPair<I>::getInternal(p);
		}

		template <size_t I, typename T1, typename T2>
		const tuple_element_t<I, pair<T1, T2>>& get(const pair<T1, T2>& p)
		{
			return GetPair<I>::getInternal(p);
		}

		template <size_t I, typename T1, typename T2>
		tuple_element_t<I, pair<T1, T2>>&& get(pair<T1, T2>&& p)
		{
			return GetPair<I>::getInternal(move(p));
		}

#endif  // EASTL_TUPLE_ENABLED


}  // namespace eastl

#ifdef _MSC_VER
	#pragma warning(pop)
#endif


#endif // Header include guard















