/////////////////////////////////////////////////////////////////////////////
// EASprintf.h
//
// Copyright (c) Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EABase/eabase.h>
#include <EAAssert/eaassert.h>
#include <EAStdC/EASprintf.h>
#include <cwchar>

namespace EA {
namespace StdC {

EASTDC_API int Vsnprintf(char8_t* EA_RESTRICT pDestination, size_t n, const char8_t* EA_RESTRICT pFormat, va_list arguments)
	{ return std::vsnprintf(pDestination, n, pFormat, arguments); }

#if defined(EA_CHAR16_NATIVE)
EASTDC_API int Vsnprintf(char16_t* EA_RESTRICT pDestination, size_t n, const char16_t* EA_RESTRICT pFormat, va_list arguments)
	{ EA_FAIL(); return -1; }
#endif

#if defined(EA_CHAR32_NATIVE)
EASTDC_API int Vsnprintf(char32_t* EA_RESTRICT pDestination, size_t n, const char32_t* EA_RESTRICT pFormat, va_list arguments)
 	{ EA_FAIL(); return -1; }
#endif

EASTDC_API int Vsnprintf(wchar_t* EA_RESTRICT pDestination, size_t n, const wchar_t* EA_RESTRICT pFormat, va_list arguments)
	{ return vswprintf(pDestination, n, pFormat, arguments); }

}}
