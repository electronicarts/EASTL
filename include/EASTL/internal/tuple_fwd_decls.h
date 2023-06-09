/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_TUPLE_FWD_DECLS_H
#define EASTL_TUPLE_FWD_DECLS_H

#include <EASTL/internal/config.h>

#if EASTL_TUPLE_ENABLED

namespace eastl
{
	template <typename... T>
	class tuple;

	template <typename Tuple>
	struct tuple_size;

#if EASTL_VARIABLE_TEMPLATES_ENABLED
	template <class T>
	EA_CONSTEXPR size_t tuple_size_v = tuple_size<T>::value;
#endif

	template <size_t I, typename Tuple>
	struct tuple_element;

	template <size_t I, typename Tuple>
	using tuple_element_t = typename tuple_element<I, Tuple>::type;

	template<typename T> struct is_lvalue_reference;

	template<bool B, typename T, typename F>
	struct conditional;

	template <typename T> struct add_lvalue_reference;

	template <typename T> struct remove_reference;

	// const typename for tuple_element_t, for when tuple or TupleImpl cannot itself be const
	template <size_t I, typename Tuple>
	using const_tuple_element_t = typename conditional<
						is_lvalue_reference<tuple_element_t<I, Tuple>>::value,
							 typename add_lvalue_reference<const typename remove_reference<tuple_element_t<I, Tuple>>::type>::type,
							 const tuple_element_t<I, Tuple>
						>::type;

	// get
	template <size_t I, typename... Ts_>
	tuple_element_t<I, tuple<Ts_...>>& get(tuple<Ts_...>& t);

	template <size_t I, typename... Ts_>
	const_tuple_element_t<I, tuple<Ts_...>>& get(const tuple<Ts_...>& t);

	template <size_t I, typename... Ts_>
	tuple_element_t<I, tuple<Ts_...>>&& get(tuple<Ts_...>&& t);

	template <typename T, typename... ts_>
	T& get(tuple<ts_...>& t);

	template <typename T, typename... ts_>
	const T& get(const tuple<ts_...>& t);

	template <typename T, typename... ts_>
	T&& get(tuple<ts_...>&& t);
}

#endif  // EASTL_VARIADIC_TEMPLATES_ENABLED

#endif  // EASTL_TUPLE_FWD_DECLS_H
