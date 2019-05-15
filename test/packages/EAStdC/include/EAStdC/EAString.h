///////////////////////////////////////////////////////////////////////////////
// EAString.h
//
// Copyright (c) Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTDC_EASTRING_H
#define EASTDC_EASTRING_H

#include <cctype>
#include <wchar.h>
#include <stdlib.h>
#include <cstring>

#define EASTDC_API
#define EASTDC_LOCAL

namespace EA {
namespace StdC {


template<typename T>
EASTDC_API size_t Strlen(const T*  pString)
{
	size_t nLength = (size_t)-1;
	
	do{
		++nLength;
	}while(*pString++);
	
	return nLength;
}


template<typename T>
EASTDC_API int Stricmp(const T*  pString1, const T*  pString2)
{
	T c1, c2;

	while((c1 = static_cast<T>(std::tolower(*pString1++))) == (c2 = static_cast<T>(std::tolower(*pString2++))))
	{
		if(c1 == 0)
			return 0;
	}

	return c1 - c2;
}


template<typename T>
EASTDC_API inline int Strcmp(const T*  pString1, const T*  pString2)
{
	T c1, c2;

	while((c1 = *pString1++) == (c2 = *pString2++))
	{
		if(c1 == 0)
			return 0;
	}

	return c1 - c2;
}

EASTDC_API inline int Strcmp(const char*  pString1, const char*  pString2)
{
    return std::strcmp(pString1, pString2);
}


template<typename T>
EASTDC_API uint32_t AtoU32(const T* pString)
{
	return atoi(pString);
}


template<typename T>
EASTDC_API int32_t AtoI32(const T* pString)
{
	return atoi(pString);
}


template<typename T>
EASTDC_API inline size_t Strlcpy(T*  pDestination, const T*  pSource, size_t nDestCapacity)
{
	const T *s = pSource;
	size_t n = nDestCapacity;

	if(n && --n)
	{
		do{
			if((*pDestination++ = *s++) == 0)
				break;
		} while(--n);
	}

	if(!n)
	{
		if(nDestCapacity)
			*pDestination = 0;
		while(*s++)
			{ }
	}

	return (size_t)(s - pSource - 1);
}

EASTDC_API inline size_t Strlcpy(wchar_t*  pDestination, const char*  pSource, size_t nDestCapacity)
{
	return mbstowcs(pDestination, pSource, nDestCapacity);
}

EASTDC_API inline size_t Strlcpy(char*  pDestination, const wchar_t*  pSource, size_t nDestCapacity)
{
	return wcstombs(pDestination, pSource, nDestCapacity);
}


}} // namespace EA::StdC


#endif  // EASTDC_EASTRING_H
