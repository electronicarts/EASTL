/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_FUNCTIONAL_BASE_H
#define EASTL_INTERNAL_FUNCTIONAL_BASE_H

#include <EASTL/internal/config.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#include <EASTL/internal/memory_base.h>
#include <EASTL/internal/move_help.h>
#include <EASTL/type_traits.h>


namespace eastl
{
	// foward declaration for swap
	template <typename T>
	inline void swap(T& a, T& b)
	    EA_NOEXCEPT_IF(eastl::is_nothrow_move_constructible<T>::value && eastl::is_nothrow_move_assignable<T>::value);


	/// invoke
	///
	/// invoke is a generalized function-call operator which works on function pointers, member function
	/// pointers, callable objects and member pointers.
	///
	/// For (member/non-member) function pointers and callable objects, it returns the result of calling
	/// the function/object with the specified arguments. For member data pointers, it simply returns
	/// the member.
	///
	/// Note that there are also reference_wrapper specializations of invoke, which need to be defined
	/// later since reference_wrapper uses invoke in its implementation. Those are defined immediately
	/// after the definition of reference_wrapper.
	///
	/// http://en.cppreference.com/w/cpp/utility/functional/invoke
	///
	template <typename R, typename C, typename T, typename... Args>
	EA_CONSTEXPR auto invoke_impl(R C::*func, T&& obj, Args&&... args) EA_NOEXCEPT_IF(EA_NOEXCEPT_EXPR((eastl::forward<T>(obj).*func)(eastl::forward<Args>(args)...)))
		-> typename enable_if<is_base_of<C, decay_t<T>>::value,
	                       decltype((eastl::forward<T>(obj).*func)(eastl::forward<Args>(args)...))>::type
	{
		return (eastl::forward<T>(obj).*func)(eastl::forward<Args>(args)...);
	}

	template <typename F, typename... Args>
	EA_CONSTEXPR auto invoke_impl(F&& func, Args&&... args) EA_NOEXCEPT_IF(EA_NOEXCEPT_EXPR(eastl::forward<F>(func)(eastl::forward<Args>(args)...)))
		-> decltype(eastl::forward<F>(func)(eastl::forward<Args>(args)...))
	{
		return eastl::forward<F>(func)(eastl::forward<Args>(args)...);
	}


	template <typename R, typename C, typename T, typename... Args>
	EA_CONSTEXPR auto invoke_impl(R C::*func, T&& obj, Args&&... args) EA_NOEXCEPT_IF(EA_NOEXCEPT_EXPR(((*eastl::forward<T>(obj)).*func)(eastl::forward<Args>(args)...)))
		-> decltype(((*eastl::forward<T>(obj)).*func)(eastl::forward<Args>(args)...))
	{
		return ((*eastl::forward<T>(obj)).*func)(eastl::forward<Args>(args)...);
	}

	template <typename M, typename C, typename T>
	EA_CONSTEXPR auto invoke_impl(M C::*member, T&& obj) EA_NOEXCEPT_IF(EA_NOEXCEPT_EXPR(eastl::forward<T>(obj).*member))
		-> typename enable_if<
			is_base_of<C, decay_t<T>>::value,
			decltype(eastl::forward<T>(obj).*member)
	>::type
	{
		return eastl::forward<T>(obj).*member;
	}

	template <typename M, typename C, typename T>
	EA_CONSTEXPR auto invoke_impl(M C::*member, T&& obj) EA_NOEXCEPT_IF(EA_NOEXCEPT_EXPR((*eastl::forward<T>(obj)).*member))
		-> decltype((*eastl::forward<T>(obj)).*member)
	{
		return (*eastl::forward<T>(obj)).*member;
	}

	template <typename F, typename... Args>
	EA_CONSTEXPR decltype(auto) invoke(F&& func, Args&&... args) EA_NOEXCEPT_IF(EA_NOEXCEPT_EXPR(invoke_impl(eastl::forward<F>(func), eastl::forward<Args>(args)...)))
	{
		return invoke_impl(eastl::forward<F>(func), eastl::forward<Args>(args)...);
	}

	template <typename F, typename = void, typename... Args>
	struct invoke_result_impl {
	};

	template <typename F, typename... Args>
	struct invoke_result_impl<F, void_t<decltype(invoke_impl(eastl::declval<F>(), eastl::declval<Args>()...))>, Args...>
	{
		typedef decltype(invoke_impl(eastl::declval<F>(), eastl::declval<Args>()...)) type;
	};

	template <typename F, typename... Args>
	struct invoke_result : public invoke_result_impl<F, void, Args...> {};

	#if !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		template <typename F, typename... Args>
		using invoke_result_t = typename invoke_result<F, Args...>::type;
	#endif

	template <typename F, typename = void, typename... Args>
	struct is_invocable_impl : public eastl::false_type {};

	template <typename F, typename... Args>
	struct is_invocable_impl<F, void_t<typename eastl::invoke_result<F, Args...>::type>, Args...> : public eastl::true_type {};

	template <typename F, typename... Args>
	struct is_invocable : public is_invocable_impl<F, void, Args...> {};

	template <typename R, typename F, typename = void, typename... Args>
	struct is_invocable_r_impl : public eastl::false_type {};

	template <typename R, typename F, typename... Args>
	struct is_invocable_r_impl<R, F, void_t<typename invoke_result<F, Args...>::type>, Args...>
		: public disjunction<is_convertible<typename invoke_result<F, Args...>::type, R>,
							 is_same<typename remove_cv<R>::type, void>> {};

	template <typename R, typename F, typename... Args>
	struct is_invocable_r : public is_invocable_r_impl<R, F, void, Args...> {};

	template <typename F, typename... Args>
	EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR bool is_invocable_v = is_invocable<F, Args...>::value;

	template <typename R, typename F, typename... Args>
	EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR bool is_invocable_r_v = is_invocable_r<R, F, Args...>::value;

	template <typename F, typename = void, typename... Args>
	struct is_nothrow_invocable_impl : public eastl::false_type	{};

	template <typename F, typename... Args>
	struct is_nothrow_invocable_impl<F, void_t<typename eastl::invoke_result<F, Args...>::type>, Args...>
	    : public eastl::bool_constant<EA_NOEXCEPT_EXPR(eastl::invoke(eastl::declval<F>(), eastl::declval<Args>()...))>	{};

	template <typename F, typename... Args>
	struct is_nothrow_invocable : public is_nothrow_invocable_impl<F, void, Args...> {};

	template <typename R, typename F, typename = void, typename... Args>
	struct is_nothrow_invocable_r_impl : public eastl::false_type {};

	template <typename R, typename F, typename... Args>
	struct is_nothrow_invocable_r_impl<R, F, void_t<typename eastl::invoke_result<F, Args...>::type>, Args...>
	{
		static EA_CONSTEXPR_OR_CONST bool value = eastl::is_convertible<typename eastl::invoke_result<F, Args...>::type, R>::value
										&& eastl::is_nothrow_invocable<F, Args...>::value;
	};

	template <typename R, typename F, typename... Args>
	struct is_nothrow_invocable_r : public is_nothrow_invocable_r_impl<R, F, void, Args...> {};

	template <typename F, typename... Args>
	EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR bool is_no_throw_invocable_v = is_nothrow_invocable<F, Args...>::value;

	template <typename R, typename F, typename... Args>
	EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR bool is_nothrow_invocable_r_v = is_nothrow_invocable_r<R, F, Args...>::value;

	/// allocator_arg_t
	///
	/// allocator_arg_t is an empty class type used to disambiguate the overloads of
	/// constructors and member functions of allocator-aware objects, including tuple,
	/// function, promise, and packaged_task.
	/// http://en.cppreference.com/w/cpp/memory/allocator_arg_t
	///
	struct allocator_arg_t
	{};


	/// allocator_arg
	///
	/// allocator_arg is a constant of type allocator_arg_t used to disambiguate, at call site,
	/// the overloads of the constructors and member functions of allocator-aware objects,
	/// such as tuple, function, promise, and packaged_task.
	/// http://en.cppreference.com/w/cpp/memory/allocator_arg
	///
	EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR allocator_arg_t allocator_arg = allocator_arg_t();


	template <typename Argument, typename Result>
	struct EASTL_REMOVE_AT_2024_APRIL unary_function
	{
		EASTL_REMOVE_AT_2024_APRIL typedef Argument argument_type;
		EASTL_REMOVE_AT_2024_APRIL typedef Result   result_type;
	};


	template <typename Argument1, typename Argument2, typename Result>
	struct EASTL_REMOVE_AT_2024_APRIL binary_function
	{
		EASTL_REMOVE_AT_2024_APRIL typedef Argument1 first_argument_type;
		EASTL_REMOVE_AT_2024_APRIL typedef Argument2 second_argument_type;
		EASTL_REMOVE_AT_2024_APRIL typedef Result    result_type;
	};


	/// less<T>
	template <typename T = void>
	struct less
	{
		EA_CPP14_CONSTEXPR bool operator()(const T& a, const T& b) const
			{ return a < b; }
	};

	// http://en.cppreference.com/w/cpp/utility/functional/less_void
	template <>
	struct less<void>
	{
		template<typename A, typename B>
		EA_CPP14_CONSTEXPR auto operator()(A&& a, B&& b) const
			-> decltype(eastl::forward<A>(a) < eastl::forward<B>(b))
			{ return eastl::forward<A>(a) < eastl::forward<B>(b); }
	};


	/// reference_wrapper
	template <typename T>
	class reference_wrapper
	{
	public:
		typedef T type;

		reference_wrapper(T&) EA_NOEXCEPT;
		reference_wrapper(T&&) = delete;
		reference_wrapper(const reference_wrapper<T>& x) EA_NOEXCEPT;

		reference_wrapper& operator=(const reference_wrapper<T>& x) EA_NOEXCEPT;

		operator T& () const EA_NOEXCEPT;
		T& get() const EA_NOEXCEPT;

		template <typename... ArgTypes>
		typename eastl::invoke_result<T&, ArgTypes...>::type operator() (ArgTypes&&...) const;

	private:
		T* val;
	};

	template <typename T>
	reference_wrapper<T>::reference_wrapper(T &v) EA_NOEXCEPT
		: val(eastl::addressof(v))
	{}

	template <typename T>
	reference_wrapper<T>::reference_wrapper(const reference_wrapper<T>& other) EA_NOEXCEPT
		: val(other.val)
	{}

	template <typename T>
	reference_wrapper<T>& reference_wrapper<T>::operator=(const reference_wrapper<T>& other) EA_NOEXCEPT
	{
		val = other.val;
		return *this;
	}

	template <typename T>
	reference_wrapper<T>::operator T&() const EA_NOEXCEPT
	{
		return *val;
	}

	template <typename T>
	T& reference_wrapper<T>::get() const EA_NOEXCEPT
	{
		return *val;
	}

	template <typename T>
	template <typename... ArgTypes>
	typename eastl::invoke_result<T&, ArgTypes...>::type reference_wrapper<T>::operator() (ArgTypes&&... args) const
	{
		return eastl::invoke(*val, eastl::forward<ArgTypes>(args)...);
	}

	// reference_wrapper-specific utilties
	template <typename T>
	reference_wrapper<T> ref(T& t) EA_NOEXCEPT
	{
		return eastl::reference_wrapper<T>(t);
	}

	template <typename T>
	void ref(const T&&) = delete;

	template <typename T>
	reference_wrapper<T> ref(reference_wrapper<T>t) EA_NOEXCEPT
	{
		return eastl::ref(t.get());
	}

	template <typename T>
	reference_wrapper<const T> cref(const T& t) EA_NOEXCEPT
	{
		return eastl::reference_wrapper<const T>(t);
	}

	template <typename T>
	void cref(const T&&) = delete;

	template <typename T>
	reference_wrapper<const T> cref(reference_wrapper<T> t) EA_NOEXCEPT
	{
		return eastl::cref(t.get());
	}


	// reference_wrapper-specific type traits
	template <typename T>
	struct is_reference_wrapper_helper
		: public eastl::false_type {};

	template <typename T>
	struct is_reference_wrapper_helper<eastl::reference_wrapper<T> >
		: public eastl::true_type {};

	template <typename T>
	struct is_reference_wrapper
		: public eastl::is_reference_wrapper_helper<typename eastl::remove_cv<T>::type> {};


	// Helper which adds a reference to a type when given a reference_wrapper of that type.
	template <typename T>
	struct remove_reference_wrapper
		{ typedef T type; };

	template <typename T>
	struct remove_reference_wrapper< eastl::reference_wrapper<T> >
		{ typedef T& type; };

	template <typename T>
	struct remove_reference_wrapper< const eastl::reference_wrapper<T> >
		{ typedef T& type; };

	// reference_wrapper specializations of invoke
	// These have to come after reference_wrapper is defined, but reference_wrapper needs to have a
	// definition of invoke, so these specializations need to come after everything else has been defined.
	template <typename R, typename C, typename T, typename... Args>
	EA_CONSTEXPR auto invoke_impl(R C::*func, T&& obj, Args&&... args) EA_NOEXCEPT_IF(EA_NOEXCEPT_EXPR((obj.get().*func)(eastl::forward<Args>(args)...)))
	   -> typename enable_if<is_reference_wrapper<eastl::decay_t<T>>::value,
						   decltype((obj.get().*func)(eastl::forward<Args>(args)...))>::type
	{
		return (obj.get().*func)(eastl::forward<Args>(args)...);
	}

	template <typename M, typename C, typename T>
	EA_CONSTEXPR auto invoke_impl(M C::*member, T&& obj) EA_NOEXCEPT_IF(EA_NOEXCEPT_EXPR(obj.get().*member))
	   ->  typename enable_if<is_reference_wrapper<eastl::decay_t<T>>::value,
	                       decltype(obj.get().*member)>::type
	{
		return obj.get().*member;
	}


	///////////////////////////////////////////////////////////////////////
	// bind
	///////////////////////////////////////////////////////////////////////

	/// bind1st
	///
	template <typename Operation>
	class EASTL_REMOVE_AT_2024_APRIL binder1st : public unary_function<typename Operation::second_argument_type, typename Operation::result_type>
	{
		protected:
			typename Operation::first_argument_type value;
			Operation op;

		public:
			binder1st(const Operation& x, const typename Operation::first_argument_type& y)
				: value(y), op(x) { }

			typename Operation::result_type operator()(const typename Operation::second_argument_type& x) const
				{ return op(value, x); }

			typename Operation::result_type operator()(typename Operation::second_argument_type& x) const
				{ return op(value, x); }
	};


	EASTL_INTERNAL_DISABLE_DEPRECATED() // 'eastl::binder1st<Operation>': was declared deprecated
	template <typename Operation, typename T>
	EASTL_REMOVE_AT_2024_APRIL inline binder1st<Operation> bind1st(const Operation& op, const T& x)
	{
		typedef typename Operation::first_argument_type value;
		return binder1st<Operation>(op, value(x));
	}
	EASTL_INTERNAL_RESTORE_DEPRECATED()

	/// bind2nd
	///
	template <typename Operation>
	class EASTL_REMOVE_AT_2024_APRIL binder2nd : public unary_function<typename Operation::first_argument_type, typename Operation::result_type>
	{
		protected:
			Operation op;
			typename Operation::second_argument_type value;

		public:
			binder2nd(const Operation& x, const typename Operation::second_argument_type& y)
				: op(x), value(y) { }

			typename Operation::result_type operator()(const typename Operation::first_argument_type& x) const
				{ return op(x, value); }

			typename Operation::result_type operator()(typename Operation::first_argument_type& x) const
				{ return op(x, value); }
	};


	EASTL_INTERNAL_DISABLE_DEPRECATED() // 'eastl::binder2nd<Operation>': was declared deprecated
	template <typename Operation, typename T>
	EASTL_REMOVE_AT_2024_APRIL inline binder2nd<Operation> bind2nd(const Operation& op, const T& x)
	{
		typedef typename Operation::second_argument_type value;
		return binder2nd<Operation>(op, value(x));
	}
	EASTL_INTERNAL_RESTORE_DEPRECATED()

} // namespace eastl

#endif // EASTL_INTERNAL_FUNCTIONAL_BASE_H
