/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_INTEGER_SEQUENCE_H
#define EASTL_INTEGER_SEQUENCE_H

#include <EABase/config/eacompiler.h>
#include <EASTL/internal/config.h>
#include <EASTL/type_traits.h>

namespace eastl
{

#if EASTL_VARIADIC_TEMPLATES_ENABLED && !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)

// integer_sequence
template <typename T, T... Ints>
class integer_sequence
{
public:
	typedef T value_type;
	static_assert(is_integral<T>::value, "eastl::integer_sequence can only be instantiated with an integral type");
	static EA_CONSTEXPR size_t size() EA_NOEXCEPT { return sizeof...(Ints); }
};

template <size_t... Is>
using index_sequence = integer_sequence<size_t, Is...>;

#if (defined(EA_COMPILER_GNUC) && EA_COMPILER_VERSION >= 8001)

template <typename T, T N>
using make_integer_sequence = integer_sequence<T, __integer_pack(N)...>;

#elif (defined(EA_COMPILER_CLANG) && EA_COMPILER_HAS_BUILTIN(__make_integer_seq)) || (defined(EA_COMPILER_MSVC) && (EA_COMPILER_VERSION >= 1910))

template <class T, T N>
using make_integer_sequence = __make_integer_seq<integer_sequence, T, N>;

#else 

template <size_t N, typename IndexSeq>
struct make_index_sequence_impl;

template <size_t N, size_t... Is>
struct make_index_sequence_impl<N, integer_sequence<size_t, Is...>>
{
	typedef typename make_index_sequence_impl<N - 1, integer_sequence<size_t, N - 1, Is...>>::type type;
};

template <size_t... Is>
struct make_index_sequence_impl<0, integer_sequence<size_t, Is...>>
{
	typedef integer_sequence<size_t, Is...> type;
};

template <typename Target, typename Seq>
struct integer_sequence_convert_impl;

template <typename Target, size_t... Is>
struct integer_sequence_convert_impl<Target, integer_sequence<size_t, Is...>>
{
	typedef integer_sequence<Target, Is...> type;
};

template <typename T, T N>
struct make_integer_sequence_impl
{
	typedef typename integer_sequence_convert_impl<T, typename make_index_sequence_impl<N, integer_sequence<size_t>>::type>::type type;
};

template <typename T, T N>
using make_integer_sequence = typename make_integer_sequence_impl<T, N>::type;

#endif

template <size_t N>
using make_index_sequence = make_integer_sequence<size_t, N>;

// Helper alias template that converts any type parameter pack into an index sequence of the same length
template<typename... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;

namespace internal
{

template <typename T>
struct integer_sequence_size_helper;

template <typename T, T... Ints>
struct integer_sequence_size_helper<eastl::integer_sequence<T, Ints...>> : public integral_constant<size_t, sizeof...(Ints)>
{
};

template <typename T>
struct integer_sequence_size : public integer_sequence_size_helper<eastl::remove_cv_t<T>>
{
};

template <typename T>
struct index_sequence_size : public integer_sequence_size_helper<eastl::remove_cv_t<T>>
{
};

template <typename T>
EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR size_t integer_sequence_size_v = integer_sequence_size<T>::value;

template <typename T>
EASTL_CPP17_INLINE_VARIABLE EA_CONSTEXPR size_t index_sequence_size_v = index_sequence_size<T>::value;


} // namespace internal

#endif  // EASTL_VARIADIC_TEMPLATES_ENABLED

}  // namespace eastl

#endif  // EASTL_INTEGER_SEQUENCE_H
