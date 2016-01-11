/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_FUNCTIONAL_BASE_H
#define EASTL_INTERNAL_FUNCTIONAL_BASE_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#include <EASTL/internal/config.h>

namespace eastl
{
	// foward declaration for swap
	template <typename T> 
	inline void swap(T& a, T& b) EA_NOEXCEPT_IF(eastl::is_nothrow_move_constructible<T>::value &&
	eastl::is_nothrow_move_assignable<T>::value);

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
	#if !defined(EA_COMPILER_NO_CONSTEXPR)
		EA_CONSTEXPR allocator_arg_t allocator_arg = allocator_arg_t();
	#endif


	template <typename Argument, typename Result>
	struct unary_function
	{
		typedef Argument argument_type;
		typedef Result   result_type;
	};


	template <typename Argument1, typename Argument2, typename Result>
	struct binary_function
	{
		typedef Argument1 first_argument_type;
		typedef Argument2 second_argument_type;
		typedef Result    result_type;
	};


	/// less<T>
	template <typename T>
	struct less : public binary_function<T, T, bool>
	{
		EA_CPP14_CONSTEXPR bool operator()(const T& a, const T& b) const
			{ return a < b; }
	};


	/// reference_wrapper
	///
	/// This is currently a placeholder and isn't complete yet.
	/// reference_wrapper is a class that emulates a C++ reference while adding some flexibility.
	///
	template <typename T>
	class reference_wrapper
	{
	public:
		typedef T type;

		reference_wrapper(T&) EA_NOEXCEPT;
		#if !defined(EA_COMPILER_NO_DELETED_FUNCTIONS)
			reference_wrapper(T&&) = delete;
		#endif
		reference_wrapper(const reference_wrapper<T>& x) EA_NOEXCEPT;

		reference_wrapper& operator=(const reference_wrapper<T>& x) EA_NOEXCEPT;

		operator T& () const EA_NOEXCEPT;
		T& get() const EA_NOEXCEPT;

		#if EASTL_VARIADIC_TEMPLATES_ENABLED
			template <typename... ArgTypes>
			typename eastl::result_of<T&(ArgTypes&&...)>::type operator() (ArgTypes&&...) const;
		#endif
	};


	// reference_wrapper-specific utilties
	template <typename T>
	reference_wrapper<T> ref(T& t) EA_NOEXCEPT;

	#if !defined(EA_COMPILER_NO_DELETED_FUNCTIONS)
		template <typename T>
		void ref(const T&&) = delete;
	#endif

	template <typename T>
	reference_wrapper<T> ref(reference_wrapper<T>t) EA_NOEXCEPT;

	template <typename T> 
	reference_wrapper<const T> cref(const T& t) EA_NOEXCEPT;

	#if !defined(EA_COMPILER_NO_DELETED_FUNCTIONS)
		template <typename T> 
		void cref(const T&&) = delete;
	#endif

	template <typename T>
	reference_wrapper<const T> cref(reference_wrapper<T> t) EA_NOEXCEPT;


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


	///////////////////////////////////////////////////////////////////////
	// bind
	///////////////////////////////////////////////////////////////////////

	/// bind1st
	///
	template <typename Operation>
	class binder1st : public unary_function<typename Operation::second_argument_type, typename Operation::result_type>
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


	template <typename Operation, typename T>
	inline binder1st<Operation> bind1st(const Operation& op, const T& x)
	{
		typedef typename Operation::first_argument_type value;
		return binder1st<Operation>(op, value(x));
	}


	/// bind2nd
	///
	template <typename Operation>
	class binder2nd : public unary_function<typename Operation::first_argument_type, typename Operation::result_type>
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


	template <typename Operation, typename T>
	inline binder2nd<Operation> bind2nd(const Operation& op, const T& x)
	{
		typedef typename Operation::second_argument_type value;
		return binder2nd<Operation>(op, value(x));
	}

} // namespace eastl

#endif // Header include guard















