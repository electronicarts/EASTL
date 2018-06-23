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
	class tuple_size;

	template <size_t I, typename Tuple>
	class tuple_element;

	template <size_t I, typename Tuple>
	using tuple_element_t = typename tuple_element<I, Tuple>::type;

	// const typename for tuple_element_t, for when tuple or TupleImpl cannot itself be const
	template <size_t I, typename Tuple>
	using const_tuple_element_t = typename conditional<
						is_lvalue_reference<tuple_element_t<I, Tuple>>::value,
							 add_lvalue_reference_t<const remove_reference_t<tuple_element_t<I, Tuple>>>,
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
