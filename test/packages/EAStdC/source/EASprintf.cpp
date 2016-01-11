/////////////////////////////////////////////////////////////////////////////
// EASprintf.h
//
// Copyright (c) Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EAAssert/eaassert.h>
#include <EAStdC/EASprintf.h>


namespace EA {
namespace StdC {

EASTDC_API int Vsnprintf(char8_t* EA_RESTRICT pDestination, size_t n, const char8_t* EA_RESTRICT pFormat, va_list arguments)
	{ return std::vsnprintf(pDestination, n, pFormat, arguments); }

EASTDC_API int Vsnprintf(char16_t* EA_RESTRICT pDestination, size_t n, const char16_t* EA_RESTRICT pFormat, va_list arguments)
	{ EA_FAIL(); return -1; }

EASTDC_API int Vsnprintf(char32_t* EA_RESTRICT pDestination, size_t n, const char32_t* EA_RESTRICT pFormat, va_list arguments)
	{ EA_FAIL(); return -1; }

EASTDC_API int Vsnprintf(wchar_t* EA_RESTRICT pDestination, size_t n, const wchar_t* EA_RESTRICT pFormat, va_list arguments)
	{ return vswprintf(pDestination, n, pFormat, arguments); }

}}
