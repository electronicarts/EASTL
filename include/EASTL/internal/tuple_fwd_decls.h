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
}

#endif  // EASTL_VARIADIC_TEMPLATES_ENABLED

#endif  // EASTL_TUPLE_FWD_DECLS_H
