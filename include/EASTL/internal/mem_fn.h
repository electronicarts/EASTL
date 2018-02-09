/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_MEM_FN_H
#define EASTL_INTERNAL_MEM_FN_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
#pragma once
#endif

////////////////////////////////////////////////////////////////////////////////
// The code in this file is a modification of the libcxx implementation.  We copy
// the license information here as required.
//
// We implement only enough of mem_fn to implement eastl::function.
////////////////////////////////////////////////////////////////////////////////

//===------------------------ functional ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//


namespace eastl
{
	//
	// apply_cv
	//
	template <class T, class U,
			  bool = is_const<typename remove_reference<T>::type>::value,
			  bool = is_volatile<typename remove_reference<T>::type>::value>
	struct apply_cv { typedef U type; };

	template <class T, class U> struct apply_cv<T, U, true, false>   { typedef const U type; };
	template <class T, class U> struct apply_cv<T, U, false, true>   { typedef volatile U type; };
	template <class T, class U> struct apply_cv<T, U, true, true>    { typedef const volatile U type; };
	template <class T, class U> struct apply_cv<T&, U, false, false> { typedef U& type; };
	template <class T, class U> struct apply_cv<T&, U, true, false>  { typedef const U& type; };
	template <class T, class U> struct apply_cv<T&, U, false, true>  { typedef volatile U& type; };
	template <class T, class U> struct apply_cv<T&, U, true, true>   { typedef const volatile U& type; };



	//
	// has_result_type
	//
	template <class T>
	struct has_result_type
	{
	private:
		template <class U>
		static eastl::no_type test(...);

		template <class U>
		static eastl::yes_type test(typename U::result_type* = 0);

	public:
		static const bool value = sizeof(test<T>(0)) == sizeof(eastl::yes_type);
	};



	//
	// derives_from_unary_function
	// derives_from_binary_function
	//
	template <class T>
	struct derives_from_unary_function
	{
	private:
		static eastl::no_type test(...);

		template <class A, class R>
		static unary_function<A, R> test(const volatile unary_function<A, R>*);

	public:
		static const bool value = !is_same<decltype(test((T*)0)), eastl::no_type>::value;
		typedef decltype(test((T*)0)) type;
	};

	template <class T>
	struct derives_from_binary_function
	{
	private:
		static eastl::no_type test(...);
		template <class A1, class A2, class R>
		static binary_function<A1, A2, R> test(const volatile binary_function<A1, A2, R>*);

	public:
		static const bool value = !is_same<decltype(test((T*)0)), eastl::no_type>::value;
		typedef decltype(test((T*)0)) type;
	};



	//
	// maybe_derives_from_unary_function
	// maybe_derives_from_binary_function
	//
	template <class T, bool = derives_from_unary_function<T>::value>
	struct maybe_derive_from_unary_function // bool is true
		: public derives_from_unary_function<T>::type { };

	template <class T>
	struct maybe_derive_from_unary_function<T, false> { };

	template <class T, bool = derives_from_binary_function<T>::value>
	struct maybe_derive_from_binary_function // bool is true
		: public derives_from_binary_function<T>::type { };

	template <class T>
	struct maybe_derive_from_binary_function<T, false> { };



	//
	// weak_result_type_imp
	//
	template <class T, bool = has_result_type<T>::value>
	struct weak_result_type_imp // bool is true
		: public maybe_derive_from_unary_function<T>,
		  public maybe_derive_from_binary_function<T>
	{
		typedef typename T::result_type result_type;
	};

	template <class T>
	struct weak_result_type_imp<T, false> : public maybe_derive_from_unary_function<T>,
											public maybe_derive_from_binary_function<T> { };



	//
	// weak_result_type
	//
	template <class T>
	struct weak_result_type : public weak_result_type_imp<T> { };

	// 0 argument case 
	template <class R> struct weak_result_type<R()> { typedef R result_type; }; 
	template <class R> struct weak_result_type<R(&)()> { typedef R result_type; }; 
	template <class R> struct weak_result_type<R (*)()> { typedef R result_type; }; 

	// 1 argument case 
	template <class R, class A1> struct weak_result_type<R(A1)> : public unary_function<A1, R> { }; 
	template <class R, class A1> struct weak_result_type<R(&)(A1)> : public unary_function<A1, R> { }; 
	template <class R, class A1> struct weak_result_type<R (*)(A1)> : public unary_function<A1, R> { }; 
	template <class R, class C> struct weak_result_type<R (C::*)()> : public unary_function<C*, R> { }; 
	template <class R, class C> struct weak_result_type<R (C::*)() const> : public unary_function<const C*, R> { }; 
	template <class R, class C> struct weak_result_type<R (C::*)() volatile> : public unary_function<volatile C*, R> { }; 
	template <class R, class C> struct weak_result_type<R (C::*)() const volatile> : public unary_function<const volatile C*, R> { };

	// 2 argument case 
	template <class R, class A1, class A2> struct weak_result_type<R(A1, A2)> : public binary_function<A1, A2, R> { }; 
	template <class R, class A1, class A2> struct weak_result_type<R (*)(A1, A2)> : public binary_function<A1, A2, R> { }; 
	template <class R, class A1, class A2> struct weak_result_type<R(&)(A1, A2)> : public binary_function<A1, A2, R> { }; 
	template <class R, class C, class A1> struct weak_result_type<R (C::*)(A1)> : public binary_function<C*, A1, R> { }; 
	template <class R, class C, class A1> struct weak_result_type<R (C::*)(A1) const> : public binary_function<const C*, A1, R> { }; 
	template <class R, class C, class A1> struct weak_result_type<R (C::*)(A1) volatile> : public binary_function<volatile C*, A1, R> { }; 
	template <class R, class C, class A1> struct weak_result_type<R (C::*)(A1) const volatile> : public binary_function<const volatile C*, A1, R> { };

	// 3 or more arguments
#if EASTL_VARIADIC_TEMPLATES_ENABLED 
	template <class R, class A1, class A2, class A3, class... A4> struct weak_result_type<R(A1, A2, A3, A4...)> { typedef R result_type; }; 
	template <class R, class A1, class A2, class A3, class... A4> struct weak_result_type<R(&)(A1, A2, A3, A4...)> { typedef R result_type; }; 
	template <class R, class A1, class A2, class A3, class... A4> struct weak_result_type<R (*)(A1, A2, A3, A4...)> { typedef R result_type; }; 
	template <class R, class C, class A1, class A2, class... A3> struct weak_result_type<R (C::*)(A1, A2, A3...)> { typedef R result_type; }; 
	template <class R, class C, class A1, class A2, class... A3> struct weak_result_type<R (C::*)(A1, A2, A3...) const> { typedef R result_type; }; 
	template <class R, class C, class A1, class A2, class... A3> struct weak_result_type<R (C::*)(A1, A2, A3...) volatile> { typedef R result_type; }; 
	template <class R, class C, class A1, class A2, class... A3> struct weak_result_type<R (C::*)(A1, A2, A3...) const volatile> { typedef R result_type; };
#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// mem_fn_impl
	//
	template <class T>
	class mem_fn_impl 
#if defined(_MSC_VER) && (_MSC_VER >= 1900)  // VS2015 or later
		// Due to a (seemingly random) internal compiler error on VS2013 we disable eastl::unary_function and
		// binary_function support for eastl::mem_fn as its not widely (if at all) used.  If you require this support
		// on VS2013 or below please contact us.
		: public weak_result_type<T>
#endif
	{
	public:
		typedef T type;

	private:
		type func;

	public:
		EASTL_FORCE_INLINE mem_fn_impl(type _func) : func(_func) {}

#if EASTL_VARIADIC_TEMPLATES_ENABLED
	    template <class... ArgTypes>
	    typename invoke_result<type, ArgTypes...>::type operator()(ArgTypes&&... args) const
	    {
		    return invoke(func, eastl::forward<ArgTypes>(args)...);
	    }
#else
	    typename invoke_result<type>::type operator()() const { return invoke_impl(func); }

	    template <class A0>
	    typename invoke_result0<type, A0>::type operator()(A0& a0) const
	    {
		    return invoke(func, a0);
	    }

	    template <class A0, class A1>
	    typename invoke_result1<type, A0, A1>::type operator()(A0& a0, A1& a1) const
	    {
		    return invoke(func, a0, a1);
	    }

	    template <class A0, class A1, class A2>
	    typename invoke_result2<type, A0, A1, A2>::type operator()(A0& a0, A1& a1, A2& a2) const
	    {
		    return invoke(func, a0, a1, a2);
	    }
#endif
    };  // mem_fn_impl



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// mem_fn -> mem_fn_impl adapters
	//
	template <class R, class T>
	EASTL_FORCE_INLINE mem_fn_impl<R T::*> mem_fn(R T::*pm)
	{ return mem_fn_impl<R T::*>(pm); }

	template <class R, class T>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)()> mem_fn(R (T::*pm)())
	{ return mem_fn_impl<R (T::*)()>(pm); }

	template <class R, class T, class A0>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)(A0)> mem_fn(R (T::*pm)(A0))
	{ return mem_fn_impl<R (T::*)(A0)>(pm); } 

	template <class R, class T, class A0, class A1>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)(A0, A1)> mem_fn(R (T::*pm)(A0, A1))
	{ return mem_fn_impl<R (T::*)(A0, A1)>(pm); }

	template <class R, class T, class A0, class A1, class A2>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)(A0, A1, A2)> mem_fn(R (T::*pm)(A0, A1, A2))
	{ return mem_fn_impl<R (T::*)(A0, A1, A2)>(pm); }

	template <class R, class T>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)() const> mem_fn(R (T::*pm)() const)
	{ return mem_fn_impl<R (T::*)() const>(pm); }

	template <class R, class T, class A0>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)(A0) const> mem_fn(R (T::*pm)(A0) const)
	{ return mem_fn_impl<R (T::*)(A0) const>(pm); }

	template <class R, class T, class A0, class A1>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)(A0, A1) const> mem_fn(R (T::*pm)(A0, A1) const)
	{ return mem_fn_impl<R (T::*)(A0, A1) const>(pm); }

	template <class R, class T, class A0, class A1, class A2>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)(A0, A1, A2) const> mem_fn(R (T::*pm)(A0, A1, A2) const)
	{ return mem_fn_impl<R (T::*)(A0, A1, A2) const>(pm); }

	template <class R, class T>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)() volatile> mem_fn(R (T::*pm)() volatile)
	{ return mem_fn_impl<R (T::*)() volatile>(pm); }

	template <class R, class T, class A0>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)(A0) volatile> mem_fn(R (T::*pm)(A0) volatile)
	{ return mem_fn_impl<R (T::*)(A0) volatile>(pm); }

	template <class R, class T, class A0, class A1>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)(A0, A1) volatile> mem_fn(R (T::*pm)(A0, A1) volatile)
	{ return mem_fn_impl<R (T::*)(A0, A1) volatile>(pm); }

	template <class R, class T, class A0, class A1, class A2>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)(A0, A1, A2) volatile> mem_fn(R (T::*pm)(A0, A1, A2) volatile)
	{ return mem_fn_impl<R (T::*)(A0, A1, A2) volatile>(pm); }

	template <class R, class T>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)() const volatile> mem_fn(R (T::*pm)() const volatile)
	{ return mem_fn_impl<R (T::*)() const volatile>(pm); }

	template <class R, class T, class A0>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)(A0) const volatile> mem_fn(R (T::*pm)(A0) const volatile)
	{ return mem_fn_impl<R (T::*)(A0) const volatile>(pm); }

	template <class R, class T, class A0, class A1>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)(A0, A1) const volatile> mem_fn(R (T::*pm)(A0, A1) const volatile)
	{ return mem_fn_impl<R (T::*)(A0, A1) const volatile>(pm); }

	template <class R, class T, class A0, class A1, class A2>
	EASTL_FORCE_INLINE mem_fn_impl<R (T::*)(A0, A1, A2) const volatile> mem_fn(R (T::*pm)(A0, A1, A2) const volatile)
	{ return mem_fn_impl<R (T::*)(A0, A1, A2) const volatile>(pm); }

} // namespace eastl

#endif // EASTL_INTERNAL_MEM_FN_H
