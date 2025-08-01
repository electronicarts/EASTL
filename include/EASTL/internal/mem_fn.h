/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_MEM_FN_H
#define EASTL_INTERNAL_MEM_FN_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
#pragma once
#endif

#include <EABase/eadeprecated.h>

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
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// mem_fn_impl
	//
	template <class T>
	class mem_fn_impl
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
