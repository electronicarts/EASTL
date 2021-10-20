///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_FUNCTIONAL_H
#define EASTL_FUNCTIONAL_H


#include <EABase/eabase.h>
#include <EASTL/internal/config.h>
#include <EASTL/internal/move_help.h>
#include <EASTL/type_traits.h>
#include <EASTL/internal/functional_base.h>
#include <EASTL/internal/mem_fn.h>


#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{
	///////////////////////////////////////////////////////////////////////
	// Primary C++ functions
	///////////////////////////////////////////////////////////////////////

	template <typename T = void>
	struct plus : public binary_function<T, T, T>
	{
		EA_CPP14_CONSTEXPR T operator()(const T& a, const T& b) const
			{ return a + b; }
	};

	// http://en.cppreference.com/w/cpp/utility/functional/plus_void
	template <>
	struct plus<void> 
	{
		typedef int is_transparent;
		template<typename A, typename B>
		EA_CPP14_CONSTEXPR auto operator()(A&& a, B&& b) const
			-> decltype(eastl::forward<A>(a) + eastl::forward<B>(b))
			{ return eastl::forward<A>(a) + eastl::forward<B>(b); }
	};

	template <typename T = void>
	struct minus : public binary_function<T, T, T>
	{
		EA_CPP14_CONSTEXPR T operator()(const T& a, const T& b) const
			{ return a - b; }
	};

	// http://en.cppreference.com/w/cpp/utility/functional/minus_void
	template <>
	struct minus<void> 
	{
		typedef int is_transparent;
		template<typename A, typename B>
		EA_CPP14_CONSTEXPR auto operator()(A&& a, B&& b) const
			-> decltype(eastl::forward<A>(a) - eastl::forward<B>(b))
			{ return eastl::forward<A>(a) - eastl::forward<B>(b); }
	};

	template <typename T = void>
	struct multiplies : public binary_function<T, T, T>
	{
		EA_CPP14_CONSTEXPR T operator()(const T& a, const T& b) const
			{ return a * b; }
	};

	// http://en.cppreference.com/w/cpp/utility/functional/multiplies_void
	template <>
	struct multiplies<void> 
	{
		typedef int is_transparent;
		template<typename A, typename B>
		EA_CPP14_CONSTEXPR auto operator()(A&& a, B&& b) const
			-> decltype(eastl::forward<A>(a) * eastl::forward<B>(b))
			{ return eastl::forward<A>(a) * eastl::forward<B>(b); }
	};

    template <typename T = void>
    struct divides : public binary_function<T, T, T>
    {
		EA_CPP14_CONSTEXPR T operator()(const T& a, const T& b) const
			{ return a / b; }
	};

	// http://en.cppreference.com/w/cpp/utility/functional/divides_void
	template <>
	struct divides<void> 
	{
		typedef int is_transparent;
		template<typename A, typename B>
		EA_CPP14_CONSTEXPR auto operator()(A&& a, B&& b) const
			-> decltype(eastl::forward<A>(a) / eastl::forward<B>(b))
			{ return eastl::forward<A>(a) / eastl::forward<B>(b); }
	};

    template <typename T = void>
    struct modulus : public binary_function<T, T, T>
    {
		EA_CPP14_CONSTEXPR T operator()(const T& a, const T& b) const
			{ return a % b; }
	};

	// http://en.cppreference.com/w/cpp/utility/functional/modulus_void
	template <>
	struct modulus<void> 
	{
		typedef int is_transparent;
		template<typename A, typename B>
		EA_CPP14_CONSTEXPR auto operator()(A&& a, B&& b) const
			-> decltype(eastl::forward<A>(a) % eastl::forward<B>(b))
			{ return eastl::forward<A>(a) % eastl::forward<B>(b); }
	};

    template <typename T = void>
    struct negate : public unary_function<T, T>
    {
		EA_CPP14_CONSTEXPR T operator()(const T& a) const
			{ return -a; }
	};

	// http://en.cppreference.com/w/cpp/utility/functional/negate_void
	template <>
	struct negate<void> 
	{
		typedef int is_transparent;
		template<typename T>
		EA_CPP14_CONSTEXPR auto operator()(T&& t) const
			-> decltype(-eastl::forward<T>(t))
			{ return -eastl::forward<T>(t); }
	};

	template <typename T = void>
	struct equal_to : public binary_function<T, T, bool>
	{
		EA_CPP14_CONSTEXPR bool operator()(const T& a, const T& b) const
			{ return a == b; }
	};

	// http://en.cppreference.com/w/cpp/utility/functional/equal_to_void
	template <>
	struct equal_to<void> 
	{
		typedef int is_transparent;
		template<typename A, typename B>
		EA_CPP14_CONSTEXPR auto operator()(A&& a, B&& b) const
			-> decltype(eastl::forward<A>(a) == eastl::forward<B>(b))
			{ return eastl::forward<A>(a) == eastl::forward<B>(b); }
	};

	template <typename T, typename Compare>
	bool validate_equal_to(const T& a, const T& b, Compare compare)
	{
		return compare(a, b) == compare(b, a);
	}

    template <typename T = void>
    struct not_equal_to : public binary_function<T, T, bool>
    {
		EA_CPP14_CONSTEXPR bool operator()(const T& a, const T& b) const
			{ return a != b; }
	};

	// http://en.cppreference.com/w/cpp/utility/functional/not_equal_to_void
	template <>
	struct not_equal_to<void> 
	{
		typedef int is_transparent;
		template<typename A, typename B>
		EA_CPP14_CONSTEXPR auto operator()(A&& a, B&& b) const
			-> decltype(eastl::forward<A>(a) != eastl::forward<B>(b))
			{ return eastl::forward<A>(a) != eastl::forward<B>(b); }
	};

	template <typename T, typename Compare>
	bool validate_not_equal_to(const T& a, const T& b, Compare compare)
	{
		return compare(a, b) == compare(b, a); // We want the not equal comparison results to be equal.
	}

	/// str_equal_to
	///
	/// Compares two 0-terminated string types.
	/// The T types are expected to be iterators or act like iterators.
	/// The expected behavior of str_less is the same as (strcmp(p1, p2) == 0).
	///
	/// Example usage:
	///     hash_set<const char*, hash<const char*>, str_equal_to<const char*> > stringHashSet;
	///
	/// Note:
	/// You couldn't use str_equal_to like this:
	///     bool result = equal("hi", "hi" + 2, "ho", str_equal_to<const char*>());
	/// This is because equal tests an array of something, with each element by
	/// the comparison function. But str_equal_to tests an array of something itself.
	///
	/// To consider: Update this code to use existing word-based comparison optimizations, 
	/// such as that used in the EAStdC Strcmp function.
	///
	template <typename T>
	struct str_equal_to : public binary_function<T, T, bool>
	{
		EA_CPP14_CONSTEXPR bool operator()(T a, T b) const
		{
			while(*a && (*a == *b))
			{
				++a;
				++b;
			}
			return (*a == *b);
		}
	};

	template <typename T = void>
	struct greater : public binary_function<T, T, bool>
	{
		EA_CPP14_CONSTEXPR bool operator()(const T& a, const T& b) const
			{ return a > b; }
	};

	// http://en.cppreference.com/w/cpp/utility/functional/greater_void
	template <>
	struct greater<void>
	{
		template<typename A, typename B>
		EA_CPP14_CONSTEXPR auto operator()(A&& a, B&& b) const
			-> decltype(eastl::forward<A>(a) > eastl::forward<B>(b))
			{ return eastl::forward<A>(a) > eastl::forward<B>(b); }
	};

	template <typename T, typename Compare>
	bool validate_greater(const T& a, const T& b, Compare compare)
	{
		return !compare(a, b) || !compare(b, a); // If (a > b), then !(b > a)
	}


	template <typename T, typename Compare>
	bool validate_less(const T& a, const T& b, Compare compare)
	{
		return !compare(a, b) || !compare(b, a); // If (a < b), then !(b < a)
	}

	/// str_less
	///
	/// Compares two 0-terminated string types. 
	/// The T types are expected to be iterators or act like iterators, 
	/// and that includes being a pointer to a C character array.
	/// The expected behavior of str_less is the same as (strcmp(p1, p2) < 0).
	/// This function is not Unicode-correct and it's not guaranteed to work
	/// with all Unicode strings.
	///
	/// Example usage:
	///     set<const char*, str_less<const char*> > stringSet;
	///
	/// To consider: Update this code to use existing word-based comparison optimizations, 
	/// such as that used in the EAStdC Strcmp function.
	///
	template <typename T>
	struct str_less : public binary_function<T, T, bool>
	{
		bool operator()(T a, T b) const
		{
			while(static_cast<typename make_unsigned<typename remove_pointer<T>::type>::type>(*a) == 
				  static_cast<typename make_unsigned<typename remove_pointer<T>::type>::type>(*b))
			{
				if(*a == 0)
					return (*b != 0);
				++a;
				++b;
			}

			char aValue = static_cast<typename remove_pointer<T>::type>(*a);
			char bValue = static_cast<typename remove_pointer<T>::type>(*b);

			typename make_unsigned<char>::type aValueU = static_cast<typename make_unsigned<char>::type>(aValue);
			typename make_unsigned<char>::type bValueU = static_cast<typename make_unsigned<char>::type>(bValue);

			return aValueU < bValueU;

			//return (static_cast<typename make_unsigned<typename remove_pointer<T>::type>::type>(*a) < 
			//        static_cast<typename make_unsigned<typename remove_pointer<T>::type>::type>(*b));
		}
	};

    template <typename T = void>
    struct greater_equal : public binary_function<T, T, bool>
    {
		EA_CPP14_CONSTEXPR bool operator()(const T& a, const T& b) const
			{ return a >= b; }
	};

	// http://en.cppreference.com/w/cpp/utility/functional/greater_equal_void
	template <>
	struct greater_equal<void>
	{
		template<typename A, typename B>
		EA_CPP14_CONSTEXPR auto operator()(A&& a, B&& b) const
			-> decltype(eastl::forward<A>(a) >= eastl::forward<B>(b))
			{ return eastl::forward<A>(a) >= eastl::forward<B>(b); }
	};

	template <typename T, typename Compare>
	bool validate_greater_equal(const T& a, const T& b, Compare compare)
	{
		return !compare(a, b) || !compare(b, a); // If (a >= b), then !(b >= a)
	}

	template <typename T = void>
	struct less_equal : public binary_function<T, T, bool>
	{
		EA_CPP14_CONSTEXPR bool operator()(const T& a, const T& b) const
			{ return a <= b; }
	};

	// http://en.cppreference.com/w/cpp/utility/functional/less_equal_void
	template <>
	struct less_equal<void>
	{
		template<typename A, typename B>
		EA_CPP14_CONSTEXPR auto operator()(A&& a, B&& b) const
			-> decltype(eastl::forward<A>(a) <= eastl::forward<B>(b))
			{ return eastl::forward<A>(a) <= eastl::forward<B>(b); }
	};

	template <typename T, typename Compare>
	bool validate_less_equal(const T& a, const T& b, Compare compare)
	{
		return !compare(a, b) || !compare(b, a); // If (a <= b), then !(b <= a)
	}

	template <typename T = void>
	struct logical_and : public binary_function<T, T, bool>
	{
		EA_CPP14_CONSTEXPR bool operator()(const T& a, const T& b) const
			{ return a && b; }
	};
	
	// http://en.cppreference.com/w/cpp/utility/functional/logical_and_void
	template <>
	struct logical_and<void>
	{
		template<typename A, typename B>
		EA_CPP14_CONSTEXPR auto operator()(A&& a, B&& b) const
			-> decltype(eastl::forward<A>(a) && eastl::forward<B>(b))
			{ return eastl::forward<A>(a) && eastl::forward<B>(b); }
	};

    template <typename T = void>
    struct logical_or : public binary_function<T, T, bool>
    {
		EA_CPP14_CONSTEXPR bool operator()(const T& a, const T& b) const
			{ return a || b; }
	};

	// http://en.cppreference.com/w/cpp/utility/functional/logical_or_void
	template <>
	struct logical_or<void>
	{
		template<typename A, typename B>
		EA_CPP14_CONSTEXPR auto operator()(A&& a, B&& b) const
			-> decltype(eastl::forward<A>(a) || eastl::forward<B>(b))
			{ return eastl::forward<A>(a) || eastl::forward<B>(b); }
	};

    template <typename T = void>
    struct logical_not : public unary_function<T, bool>
    {
		EA_CPP14_CONSTEXPR bool operator()(const T& a) const
			{ return !a; }
	};

	// http://en.cppreference.com/w/cpp/utility/functional/logical_not_void
	template <>
	struct logical_not<void>
	{
		template<typename T>
		EA_CPP14_CONSTEXPR auto operator()(T&& t) const
			-> decltype(!eastl::forward<T>(t))
			{ return !eastl::forward<T>(t); }
	};



	///////////////////////////////////////////////////////////////////////
	// Dual type functions
	///////////////////////////////////////////////////////////////////////


	template <typename T, typename U>
	struct equal_to_2 : public binary_function<T, U, bool>
	{
		EA_CPP14_CONSTEXPR bool operator()(const T& a, const U& b) const
			{ return a == b; }

		template <typename T_ = T, typename U_ = U, typename = eastl::enable_if_t<!eastl::is_same_v<eastl::remove_const_t<T_>, eastl::remove_const_t<U_>>>>
		EA_CPP14_CONSTEXPR bool operator()(const U& b, const T& a) const
			{ return b == a; }
	};

	template <typename T, typename U>
	struct not_equal_to_2 : public binary_function<T, U, bool>
	{
		EA_CPP14_CONSTEXPR bool operator()(const T& a, const U& b) const
			{ return a != b; }

		template <typename T_ = T, typename U_ = U, typename = eastl::enable_if_t<!eastl::is_same_v<eastl::remove_const_t<T_>, eastl::remove_const_t<U_>>>>
		EA_CPP14_CONSTEXPR bool operator()(const U& b, const T& a) const
			{ return b != a; }
	};


	template <typename T, typename U>
	struct less_2 : public binary_function<T, U, bool>
	{
		EA_CPP14_CONSTEXPR bool operator()(const T& a, const U& b) const
			{ return a < b; }

		template <typename T_ = T, typename U_ = U, typename = eastl::enable_if_t<!eastl::is_same_v<eastl::remove_const_t<T_>, eastl::remove_const_t<U_>>>>
		EA_CPP14_CONSTEXPR bool operator()(const U& b, const T& a) const
			{ return b < a; }
	};


	/// unary_negate
	///
	template <typename Predicate>
	class unary_negate : public unary_function<typename Predicate::argument_type, bool>
	{
		protected:
			Predicate mPredicate;
		public:
			explicit unary_negate(const Predicate& a)
				: mPredicate(a) {}
			EA_CPP14_CONSTEXPR bool operator()(const typename Predicate::argument_type& a) const
				{ return !mPredicate(a); }
	};

	template <typename Predicate>
	inline EA_CPP14_CONSTEXPR unary_negate<Predicate> not1(const Predicate& predicate)
		{ return unary_negate<Predicate>(predicate); }



	/// binary_negate
	///
	template <typename Predicate>
	class binary_negate : public binary_function<typename Predicate::first_argument_type, typename Predicate::second_argument_type, bool>
	{
		protected:
			Predicate mPredicate;
		public:
			explicit binary_negate(const Predicate& a)
				: mPredicate(a) { }
			EA_CPP14_CONSTEXPR bool operator()(const typename Predicate::first_argument_type& a, const typename Predicate::second_argument_type& b) const
				{ return !mPredicate(a, b); }
	};

	template <typename Predicate>
	inline EA_CPP14_CONSTEXPR binary_negate<Predicate> not2(const Predicate& predicate)
		{ return binary_negate<Predicate>(predicate); }



	/// unary_compose
	///
	template<typename Operation1, typename Operation2>
	struct unary_compose : public unary_function<typename Operation2::argument_type, typename Operation1::result_type>
	{
	protected:
		Operation1 op1;
		Operation2 op2;

	public:
		unary_compose(const Operation1& x, const Operation2& y)
			: op1(x), op2(y) {}

		typename Operation1::result_type operator()(const typename Operation2::argument_type& x) const
			{ return op1(op2(x)); }

		typename Operation1::result_type operator()(typename Operation2::argument_type& x) const
			{ return op1(op2(x)); }
	};

	template<typename Operation1,typename Operation2>
	inline unary_compose<Operation1,Operation2>
	compose1(const Operation1& op1, const Operation2& op2)
	{
		return unary_compose<Operation1, Operation2>(op1,op2);
	}


	/// binary_compose
	///
	template <class Operation1, class Operation2, class Operation3>
	class binary_compose : public unary_function<typename Operation2::argument_type, typename Operation1::result_type> 
	{
	protected:
		Operation1 op1;
		Operation2 op2;
		Operation3 op3;

	public:
		// Support binary functors too.
		typedef typename Operation2::argument_type first_argument_type;
		typedef typename Operation3::argument_type second_argument_type;

		binary_compose(const Operation1& x, const Operation2& y, const Operation3& z) 
			: op1(x), op2(y), op3(z) { }

		typename Operation1::result_type operator()(const typename Operation2::argument_type& x) const 
			{ return op1(op2(x),op3(x)); }

		typename Operation1::result_type operator()(typename Operation2::argument_type& x) const 
			{ return op1(op2(x),op3(x)); }

		typename Operation1::result_type operator()(const typename Operation2::argument_type& x,const typename Operation3::argument_type& y) const 
			{ return op1(op2(x),op3(y)); }

		typename Operation1::result_type operator()(typename Operation2::argument_type& x, typename Operation3::argument_type& y) const 
			{ return op1(op2(x),op3(y)); }
	};


	template <class Operation1, class Operation2, class Operation3>
	inline binary_compose<Operation1, Operation2, Operation3>
	compose2(const Operation1& op1, const Operation2& op2, const Operation3& op3)
	{
		return binary_compose<Operation1, Operation2, Operation3>(op1, op2, op3);
	}



	///////////////////////////////////////////////////////////////////////
	// pointer_to_unary_function
	///////////////////////////////////////////////////////////////////////

	/// pointer_to_unary_function
	///
	/// This is an adapter template which converts a pointer to a standalone
	/// function to a function object. This allows standalone functions to 
	/// work in many cases where the system requires a function object.
	///
	/// Example usage:
	///     ptrdiff_t Rand(ptrdiff_t n) { return rand() % n; } // Note: The C rand function is poor and slow.
	///     pointer_to_unary_function<ptrdiff_t, ptrdiff_t> randInstance(Rand);
	///     random_shuffle(pArrayBegin, pArrayEnd, randInstance);
	///
	template <typename Arg, typename Result>
	class pointer_to_unary_function : public unary_function<Arg, Result>
	{
	protected:
		Result (*mpFunction)(Arg);

	public:
		pointer_to_unary_function()
			{ }

		explicit pointer_to_unary_function(Result (*pFunction)(Arg))
			: mpFunction(pFunction) { }

		Result operator()(Arg x) const
			{ return mpFunction(x); } 
	};


	/// ptr_fun
	///
	/// This ptr_fun is simply shorthand for usage of pointer_to_unary_function.
	///
	/// Example usage (actually, you don't need to use ptr_fun here, but it works anyway):
	///    int factorial(int x) { return (x > 1) ? (x * factorial(x - 1)) : x; }
	///    transform(pIntArrayBegin, pIntArrayEnd, pIntArrayBegin, ptr_fun(factorial));
	///
	template <typename Arg, typename Result>
	inline pointer_to_unary_function<Arg, Result>
	ptr_fun(Result (*pFunction)(Arg))
		{ return pointer_to_unary_function<Arg, Result>(pFunction); }





	///////////////////////////////////////////////////////////////////////
	// pointer_to_binary_function
	///////////////////////////////////////////////////////////////////////

	/// pointer_to_binary_function
	///
	/// This is an adapter template which converts a pointer to a standalone
	/// function to a function object. This allows standalone functions to 
	/// work in many cases where the system requires a function object.
	///
	template <typename Arg1, typename Arg2, typename Result>
	class pointer_to_binary_function : public binary_function<Arg1, Arg2, Result>
	{
	protected:
		Result (*mpFunction)(Arg1, Arg2);

	public:
		pointer_to_binary_function()
			{ }

		explicit pointer_to_binary_function(Result (*pFunction)(Arg1, Arg2))
			: mpFunction(pFunction) {}

		Result operator()(Arg1 x, Arg2 y) const
			{ return mpFunction(x, y); }
	};


	/// This ptr_fun is simply shorthand for usage of pointer_to_binary_function.
	///
	/// Example usage (actually, you don't need to use ptr_fun here, but it works anyway):
	///    int multiply(int x, int y) { return x * y; }
	///    transform(pIntArray1Begin, pIntArray1End, pIntArray2Begin, pIntArray1Begin, ptr_fun(multiply));
	///
	template <typename Arg1, typename Arg2, typename Result>
	inline pointer_to_binary_function<Arg1, Arg2, Result>
	ptr_fun(Result (*pFunction)(Arg1, Arg2))
		{ return pointer_to_binary_function<Arg1, Arg2, Result>(pFunction); }






	///////////////////////////////////////////////////////////////////////
	// mem_fun
	// mem_fun1
	//
	// Note that mem_fun calls member functions via *pointers* to classes 
	// and not instances of classes. mem_fun_ref is for calling functions
	// via instances of classes or references to classes.
	//
	// NOTE:
	// mem_fun was deprecated in C++11 and removed in C++17, in favor 
	// of the more general mem_fn and bind.
	//
	///////////////////////////////////////////////////////////////////////

	/// mem_fun_t
	///
	/// Member function with no arguments.
	///
	template <typename Result, typename T> 
	class mem_fun_t : public unary_function<T*, Result>
	{
	public:
		typedef Result (T::*MemberFunction)();

		inline explicit mem_fun_t(MemberFunction pMemberFunction)
			: mpMemberFunction(pMemberFunction)
		{
			// Empty
		}

		inline Result operator()(T* pT) const
		{
			return (pT->*mpMemberFunction)();
		}

	protected:
		MemberFunction mpMemberFunction;
	};


	/// mem_fun1_t
	///
	/// Member function with one argument.
	///
	template <typename Result, typename T, typename Argument>
	class mem_fun1_t : public binary_function<T*, Argument, Result>
	{
	public:
		typedef Result (T::*MemberFunction)(Argument);

		inline explicit mem_fun1_t(MemberFunction pMemberFunction)
			: mpMemberFunction(pMemberFunction)
		{
			// Empty
		}

		inline Result operator()(T* pT, Argument arg) const
		{
			return (pT->*mpMemberFunction)(arg);
		}

	protected:
		MemberFunction mpMemberFunction;
	};


	/// const_mem_fun_t
	///
	/// Const member function with no arguments.
	/// Note that we inherit from unary_function<const T*, Result>
	/// instead of what the C++ standard specifies: unary_function<T*, Result>.
	/// The C++ standard is in error and this has been recognized by the defect group.
	///
	template <typename Result, typename T>
	class const_mem_fun_t : public unary_function<const T*, Result>
	{
	public:
		typedef Result (T::*MemberFunction)() const;

		inline explicit const_mem_fun_t(MemberFunction pMemberFunction)
			: mpMemberFunction(pMemberFunction)
		{
			// Empty
		}

		inline Result operator()(const T* pT) const
		{
			return (pT->*mpMemberFunction)();
		}

	protected:
		MemberFunction mpMemberFunction;
	};


	/// const_mem_fun1_t
	///
	/// Const member function with one argument.
	/// Note that we inherit from unary_function<const T*, Result>
	/// instead of what the C++ standard specifies: unary_function<T*, Result>.
	/// The C++ standard is in error and this has been recognized by the defect group.
	///
	template <typename Result, typename T, typename Argument>
	class const_mem_fun1_t : public binary_function<const T*, Argument, Result>
	{
	public:
		typedef Result (T::*MemberFunction)(Argument) const;

		inline explicit const_mem_fun1_t(MemberFunction pMemberFunction)
			: mpMemberFunction(pMemberFunction)
		{
			// Empty
		}

		inline Result operator()(const T* pT, Argument arg) const
		{
			return (pT->*mpMemberFunction)(arg);
		}

	protected:
		MemberFunction mpMemberFunction;
	};


	/// mem_fun
	///
	/// This is the high level interface to the mem_fun_t family.
	///
	/// Example usage:
	///    struct TestClass { void print() { puts("hello"); } }
	///    TestClass* pTestClassArray[3] = { ... };
	///    for_each(pTestClassArray, pTestClassArray + 3, &TestClass::print);
	///
	/// Note: using conventional inlining here to avoid issues on GCC/Linux
	///
	template <typename Result, typename T>
	inline mem_fun_t<Result, T>
	mem_fun(Result (T::*MemberFunction)())
	{
		return eastl::mem_fun_t<Result, T>(MemberFunction);
	}

	template <typename Result, typename T, typename Argument>
	inline mem_fun1_t<Result, T, Argument>
	mem_fun(Result (T::*MemberFunction)(Argument))
	{
		return eastl::mem_fun1_t<Result, T, Argument>(MemberFunction);
	}

	template <typename Result, typename T>
	inline const_mem_fun_t<Result, T>
	mem_fun(Result (T::*MemberFunction)() const)
	{
		return eastl::const_mem_fun_t<Result, T>(MemberFunction);
	}

	template <typename Result, typename T, typename Argument>
	inline const_mem_fun1_t<Result, T, Argument>
	mem_fun(Result (T::*MemberFunction)(Argument) const)
	{
		return eastl::const_mem_fun1_t<Result, T, Argument>(MemberFunction);
	}





	///////////////////////////////////////////////////////////////////////
	// mem_fun_ref
	// mem_fun1_ref
	//
	///////////////////////////////////////////////////////////////////////

	/// mem_fun_ref_t
	///
	template <typename Result, typename T>
	class mem_fun_ref_t : public unary_function<T, Result>
	{
	public:
		typedef Result (T::*MemberFunction)();

		inline explicit mem_fun_ref_t(MemberFunction pMemberFunction)
			: mpMemberFunction(pMemberFunction)
		{
			// Empty
		}

		inline Result operator()(T& t) const
		{
			return (t.*mpMemberFunction)();
		}

	protected:
		MemberFunction mpMemberFunction;
	};


	/// mem_fun1_ref_t
	///
	template <typename Result, typename T, typename Argument>
	class mem_fun1_ref_t : public binary_function<T, Argument, Result>
	{
	public:
		typedef Result (T::*MemberFunction)(Argument);

		inline explicit mem_fun1_ref_t(MemberFunction pMemberFunction)
			: mpMemberFunction(pMemberFunction)
		{
			// Empty
		}

		inline Result operator()(T& t, Argument arg) const
		{
			return (t.*mpMemberFunction)(arg);
		}

	protected:
		MemberFunction mpMemberFunction;
	};


	/// const_mem_fun_ref_t
	///
	template <typename Result, typename T>
	class const_mem_fun_ref_t : public unary_function<T, Result>
	{
	public:
		typedef Result (T::*MemberFunction)() const;

		inline explicit const_mem_fun_ref_t(MemberFunction pMemberFunction)
			: mpMemberFunction(pMemberFunction)
		{
			// Empty
		}

		inline Result operator()(const T& t) const
		{
			return (t.*mpMemberFunction)();
		}

	protected:
		MemberFunction mpMemberFunction;
	};


	/// const_mem_fun1_ref_t
	///
	template <typename Result, typename T, typename Argument>
	class const_mem_fun1_ref_t : public binary_function<T, Argument, Result>
	{
	public:
		typedef Result (T::*MemberFunction)(Argument) const;

		inline explicit const_mem_fun1_ref_t(MemberFunction pMemberFunction)
			: mpMemberFunction(pMemberFunction)
		{
			// Empty
		}

		inline Result operator()(const T& t, Argument arg) const
		{
			return (t.*mpMemberFunction)(arg);
		}

	protected:
		MemberFunction mpMemberFunction;
	};


	/// mem_fun_ref
	/// Example usage:
	///    struct TestClass { void print() { puts("hello"); } }
	///    TestClass testClassArray[3];
	///    for_each(testClassArray, testClassArray + 3, &TestClass::print);
	///
	/// Note: using conventional inlining here to avoid issues on GCC/Linux
	///
	template <typename Result, typename T>
	inline mem_fun_ref_t<Result, T>
	mem_fun_ref(Result (T::*MemberFunction)())
	{
		return eastl::mem_fun_ref_t<Result, T>(MemberFunction);
	}

	template <typename Result, typename T, typename Argument>
	inline mem_fun1_ref_t<Result, T, Argument>
	mem_fun_ref(Result (T::*MemberFunction)(Argument))
	{
		return eastl::mem_fun1_ref_t<Result, T, Argument>(MemberFunction);
	}

	template <typename Result, typename T>
	inline const_mem_fun_ref_t<Result, T>
	mem_fun_ref(Result (T::*MemberFunction)() const)
	{
		return eastl::const_mem_fun_ref_t<Result, T>(MemberFunction);
	}

	template <typename Result, typename T, typename Argument>
	inline const_mem_fun1_ref_t<Result, T, Argument>
	mem_fun_ref(Result (T::*MemberFunction)(Argument) const)
	{
		return eastl::const_mem_fun1_ref_t<Result, T, Argument>(MemberFunction);
	}


	// not_fn_ret
	// not_fn_ret is a implementation specified return type of eastl::not_fn.
	// The type name is not specified but it does have mandated functions that conforming implementations must support.
	//
	// http://en.cppreference.com/w/cpp/utility/functional/not_fn
	//
	template <typename F>
	struct not_fn_ret
	{
		explicit not_fn_ret(F&& f) : mDecayF(eastl::forward<F>(f)) {}
		not_fn_ret(not_fn_ret&& f) = default;
		not_fn_ret(const not_fn_ret& f) = default;

		// overloads for lvalues
		template <class... Args>
		auto operator()(Args&&... args) &
		    -> decltype(!eastl::declval<eastl::invoke_result_t<eastl::decay_t<F>&, Args...>>())
		{ return !eastl::invoke(mDecayF, eastl::forward<Args>(args)...); }

		template <class... Args>
		auto operator()(Args&&... args) const &
		    -> decltype(!eastl::declval<eastl::invoke_result_t<eastl::decay_t<F> const&, Args...>>())
		{ return !eastl::invoke(mDecayF, eastl::forward<Args>(args)...); }

		// overloads for rvalues
		template <class... Args>
		auto operator()(Args&&... args) &&
		    -> decltype(!eastl::declval<eastl::invoke_result_t<eastl::decay_t<F>, Args...>>())
		{ return !eastl::invoke(eastl::move(mDecayF), eastl::forward<Args>(args)...); }

		template <class... Args>
		auto operator()(Args&&... args) const &&
		    -> decltype(!eastl::declval<eastl::invoke_result_t<eastl::decay_t<F> const, Args...>>())
		{ return !eastl::invoke(eastl::move(mDecayF), eastl::forward<Args>(args)...); }

		eastl::decay_t<F> mDecayF;
	};

	/// not_fn
	///
	/// Creates an implementation specified functor that returns the complement of the callable object it was passed.
	/// not_fn is intended to replace the C++03-era negators eastl::not1 and eastl::not2.
	///
	/// http://en.cppreference.com/w/cpp/utility/functional/not_fn
	///
	/// Example usage:
	///
	///		auto nf = eastl::not_fn([]{ return false; });
	///     assert(nf());  // return true
	///
	template <class F>
	inline not_fn_ret<F> not_fn(F&& f)
	{
		return not_fn_ret<F>(eastl::forward<F>(f));
	}


	///////////////////////////////////////////////////////////////////////
	// hash
	///////////////////////////////////////////////////////////////////////
	namespace Internal
	{
		// utility to disable the generic template specialization that is
		// used for enum types only.
		template <typename T, bool Enabled>
		struct EnableHashIf {};

		template <typename T>
		struct EnableHashIf<T, true>
		{
			size_t operator()(T p) const { return size_t(p); }
		};
	} // namespace Internal


	template <typename T> struct hash;

	template <typename T>
	struct hash : Internal::EnableHashIf<T, is_enum_v<T>> {};

	template <typename T> struct hash<T*> // Note that we use the pointer as-is and don't divide by sizeof(T*). This is because the table is of a prime size and this division doesn't benefit distribution.
		{ size_t operator()(T* p) const { return size_t(uintptr_t(p)); } };

	template <> struct hash<bool>
		{ size_t operator()(bool val) const { return static_cast<size_t>(val); } };

	template <> struct hash<char>
		{ size_t operator()(char val) const { return static_cast<size_t>(val); } };

	template <> struct hash<signed char>
		{ size_t operator()(signed char val) const { return static_cast<size_t>(val); } };

	template <> struct hash<unsigned char>
		{ size_t operator()(unsigned char val) const { return static_cast<size_t>(val); } };

	#if defined(EA_CHAR8_UNIQUE) && EA_CHAR8_UNIQUE
		template <> struct hash<char8_t>
			{ size_t operator()(char8_t val) const { return static_cast<size_t>(val); } };
	#endif

	#if defined(EA_CHAR16_NATIVE) && EA_CHAR16_NATIVE
		template <> struct hash<char16_t>
			{ size_t operator()(char16_t val) const { return static_cast<size_t>(val); } };
	#endif

	#if defined(EA_CHAR32_NATIVE) && EA_CHAR32_NATIVE
		template <> struct hash<char32_t>
			{ size_t operator()(char32_t val) const { return static_cast<size_t>(val); } };
	#endif

	// If wchar_t is a native type instead of simply a define to an existing type...
	#if !defined(EA_WCHAR_T_NON_NATIVE)
		template <> struct hash<wchar_t>
			{ size_t operator()(wchar_t val) const { return static_cast<size_t>(val); } };
	#endif

	template <> struct hash<signed short>
		{ size_t operator()(signed short val) const { return static_cast<size_t>(val); } };

	template <> struct hash<unsigned short>
		{ size_t operator()(unsigned short val) const { return static_cast<size_t>(val); } };

	template <> struct hash<signed int>
		{ size_t operator()(signed int val) const { return static_cast<size_t>(val); } };

	template <> struct hash<unsigned int>
		{ size_t operator()(unsigned int val) const { return static_cast<size_t>(val); } };

	template <> struct hash<signed long>
		{ size_t operator()(signed long val) const { return static_cast<size_t>(val); } };

	template <> struct hash<unsigned long>
		{ size_t operator()(unsigned long val) const { return static_cast<size_t>(val); } };

	template <> struct hash<signed long long>
		{ size_t operator()(signed long long val) const { return static_cast<size_t>(val); } };

	template <> struct hash<unsigned long long>
		{ size_t operator()(unsigned long long val) const { return static_cast<size_t>(val); } };

	template <> struct hash<float>
		{ size_t operator()(float val) const { return static_cast<size_t>(val); } };

	template <> struct hash<double>
		{ size_t operator()(double val) const { return static_cast<size_t>(val); } };

	template <> struct hash<long double>
		{ size_t operator()(long double val) const { return static_cast<size_t>(val); } };

	#if defined(EA_HAVE_INT128) && EA_HAVE_INT128
	template <> struct hash<uint128_t>
		{ size_t operator()(uint128_t val) const { return static_cast<size_t>(val); } };
	#endif


	///////////////////////////////////////////////////////////////////////////
	// string hashes
	//
	// Note that our string hashes here intentionally are slow for long strings.
	// The reasoning for this is so:
	//    - The large majority of hashed strings are only a few bytes long.
	//    - The hash function is significantly more efficient if it can make this assumption.
	//    - The user is welcome to make a custom hash for those uncommon cases where
	//      long strings need to be hashed. Indeed, the user can probably make a 
	//      special hash customized for such strings that's better than what we provide.
	///////////////////////////////////////////////////////////////////////////

	template <> struct hash<char*>
	{
		size_t operator()(const char* p) const
		{
			uint32_t c, result = 2166136261U;   // FNV1 hash. Perhaps the best string hash. Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
			while((c = (uint8_t)*p++) != 0)     // Using '!=' disables compiler warnings.
				result = (result * 16777619) ^ c;
			return (size_t)result;
		}
	};

	template <> struct hash<const char*>
	{
		size_t operator()(const char* p) const
		{
			uint32_t c, result = 2166136261U;   // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
			while((c = (uint8_t)*p++) != 0)     // cast to unsigned 8 bit.
				result = (result * 16777619) ^ c;
			return (size_t)result;
		}
	};

#if EA_CHAR8_UNIQUE
	template <> struct hash<char8_t*>
	{
		size_t operator()(const char8_t* p) const
		{
			uint32_t c, result = 2166136261U;   // FNV1 hash. Perhaps the best string hash. Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
			while((c = (uint8_t)*p++) != 0)     // Using '!=' disables compiler warnings.
				result = (result * 16777619) ^ c;
			return (size_t)result;
		}
	};

	template <> struct hash<const char8_t*>
	{
		size_t operator()(const char8_t* p) const
		{
			uint32_t c, result = 2166136261U;   // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
			while((c = (uint8_t)*p++) != 0)     // cast to unsigned 8 bit.
				result = (result * 16777619) ^ c;
			return (size_t)result;
		}
	};
#endif


	template <> struct hash<char16_t*>
	{
		size_t operator()(const char16_t* p) const
		{
			uint32_t c, result = 2166136261U;   // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
			while((c = (uint16_t)*p++) != 0)    // cast to unsigned 16 bit.
				result = (result * 16777619) ^ c;
			return (size_t)result;
		}
	};

	template <> struct hash<const char16_t*>
	{
		size_t operator()(const char16_t* p) const
		{
			uint32_t c, result = 2166136261U;   // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
			while((c = (uint16_t)*p++) != 0)    // cast to unsigned 16 bit.
				result = (result * 16777619) ^ c;
			return (size_t)result;
		}
	};

	template <> struct hash<char32_t*>
	{
		size_t operator()(const char32_t* p) const
		{
			uint32_t c, result = 2166136261U;   // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
			while((c = (uint32_t)*p++) != 0)    // cast to unsigned 32 bit.
				result = (result * 16777619) ^ c;
			return (size_t)result;
		}
	};

	template <> struct hash<const char32_t*>
	{
		size_t operator()(const char32_t* p) const
		{
			uint32_t c, result = 2166136261U;   // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
			while((c = (uint32_t)*p++) != 0)    // cast to unsigned 32 bit.
				result = (result * 16777619) ^ c;
			return (size_t)result;
		}
	};

#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
	template<> struct hash<wchar_t*>
	{
		size_t operator()(const wchar_t* p) const
		{
			uint32_t c, result = 2166136261U;    // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
			while ((c = (uint32_t)*p++) != 0)    // cast to unsigned 32 bit.
				result = (result * 16777619) ^ c;
			return (size_t)result;
		}
	};

	template<> struct hash<const wchar_t*>
	{
		size_t operator()(const wchar_t* p) const
		{
			uint32_t c, result = 2166136261U;    // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
			while ((c = (uint32_t)*p++) != 0)    // cast to unsigned 32 bit.
				result = (result * 16777619) ^ c;
			return (size_t)result;
		}
	};
#endif

	/// string_hash
	///
	/// Defines a generic string hash for an arbitrary EASTL basic_string container.
	///
	/// Example usage:
	///    eastl::hash_set<MyString, eastl::string_hash<MyString> > hashSet;
	///
	template <typename String>
	struct string_hash
	{
		typedef String                                         string_type;
		typedef typename String::value_type                    value_type;
		typedef typename eastl::add_unsigned<value_type>::type unsigned_value_type;

		size_t operator()(const string_type& s) const
		{
			const unsigned_value_type* p = (const unsigned_value_type*)s.c_str();
			uint32_t c, result = 2166136261U;   // Intentionally uint32_t instead of size_t, so the behavior is the same regardless of size.
			while((c = *p++) != 0)
				result = (result * 16777619) ^ c;
			return (size_t)result;
		}
	};


} // namespace eastl

#include <EASTL/internal/function.h>

#endif // Header include guard







