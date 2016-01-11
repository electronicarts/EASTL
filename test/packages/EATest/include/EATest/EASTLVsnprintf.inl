///////////////////////////////////////////////////////////////////////////////
// EASTLVsnprintf.inl
//
// Copyright (c) Electronic Arts. All Rights Reserved.  
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEST_EASTLVSNPRINTF_INL
#define EATEST_EASTLVSNPRINTF_INL


#include <EAStdC/EASprintf.h>


// Until EASTL is switched to using EAStdC vsnprintf natively, we need to provide the following.

int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t*  pFormat, va_list arguments)
{
    return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
}

int Vsnprintf16(char16_t* pDestination, size_t n, const char16_t* pFormat, va_list arguments)
{
    return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
}

#if (EASTDC_VERSION_N >= 10600)
    int Vsnprintf32(char32_t* pDestination, size_t n, const char32_t* pFormat, va_list arguments)
    {
        return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
    }
#endif

#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
    int VsnprintfW(wchar_t* pDestination, size_t n, const wchar_t* pFormat, va_list arguments)
    {
        return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
    }
#endif

#endif // EATEST_EASTLVSNPRINTF_INL


