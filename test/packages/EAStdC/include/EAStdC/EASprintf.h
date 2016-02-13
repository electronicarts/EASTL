///////////////////////////////////////////////////////////////////////////////
// EASprintf.h
//
// Copyright (c) Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTDC_EASPRINTF_H
#define EASTDC_EASPRINTF_H

#include <EABase/eabase.h>
#include <EABase/eastdarg.h>
#include <cstdio>

#define EASTDC_API

namespace EA {
namespace StdC {

EASTDC_API int Vsnprintf(char8_t* EA_RESTRICT pDestination, size_t n, const char8_t* EA_RESTRICT pFormat, va_list arguments);
EASTDC_API int Vsnprintf(char16_t* EA_RESTRICT pDestination, size_t n, const char16_t* EA_RESTRICT pFormat, va_list arguments);
EASTDC_API int Vsnprintf(char32_t* EA_RESTRICT pDestination, size_t n, const char32_t* EA_RESTRICT pFormat, va_list arguments);
EASTDC_API int Vsnprintf(wchar_t* EA_RESTRICT pDestination, size_t n, const wchar_t* EA_RESTRICT pFormat, va_list arguments);

template<typename T>
EASTDC_API int Sprintf(T* EA_RESTRICT pDestination, const T* EA_RESTRICT pFormat, ...)
{
	va_list arguments;
	va_start(arguments, pFormat);

	auto result = std::vsprintf(pDestination, pFormat, arguments);

	va_end(arguments);

	return result;
}



}} // namespace EA::StdC

#endif  // EASTDC_EASPRINTF_H
