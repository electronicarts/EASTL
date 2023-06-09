/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements similar functionality to char_traits which is part of
// the C++ standard STL library specification. This is intended for internal
// EASTL use only.  Functionality can be accessed through the eastl::string or
// eastl::string_view types.  
//
// http://en.cppreference.com/w/cpp/string/char_traits
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_CHAR_TRAITS_H
#define EASTL_CHAR_TRAITS_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#include <EASTL/internal/config.h>
#include <EASTL/type_traits.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <ctype.h>              // toupper, etc.
#include <string.h>             // memset, etc.
EA_RESTORE_ALL_VC_WARNINGS()

namespace eastl
{
	///////////////////////////////////////////////////////////////////////////////
	/// DecodePart
	///
	/// These implement UTF8/UCS2/UCS4 encoding/decoding.
	///
	EASTL_API bool DecodePart(const char*& pSrc, const char* pSrcEnd, char*&     pDest, char*     pDestEnd);
	EASTL_API bool DecodePart(const char*& pSrc, const char* pSrcEnd, char16_t*& pDest, char16_t* pDestEnd);
	EASTL_API bool DecodePart(const char*& pSrc, const char* pSrcEnd, char32_t*& pDest, char32_t* pDestEnd);

	EASTL_API bool DecodePart(const char16_t*& pSrc, const char16_t* pSrcEnd, char*&     pDest, char*     pDestEnd);
	EASTL_API bool DecodePart(const char16_t*& pSrc, const char16_t* pSrcEnd, char16_t*& pDest, char16_t* pDestEnd);
	EASTL_API bool DecodePart(const char16_t*& pSrc, const char16_t* pSrcEnd, char32_t*& pDest, char32_t* pDestEnd);

	EASTL_API bool DecodePart(const char32_t*& pSrc, const char32_t* pSrcEnd, char*&     pDest, char*     pDestEnd);
	EASTL_API bool DecodePart(const char32_t*& pSrc, const char32_t* pSrcEnd, char16_t*& pDest, char16_t* pDestEnd);
	EASTL_API bool DecodePart(const char32_t*& pSrc, const char32_t* pSrcEnd, char32_t*& pDest, char32_t* pDestEnd);

	EASTL_API bool DecodePart(const int*& pSrc, const int* pSrcEnd, char*&     pDest, char*     pDestEnd);
	EASTL_API bool DecodePart(const int*& pSrc, const int* pSrcEnd, char16_t*& pDest, char16_t* pDestEnd);
	EASTL_API bool DecodePart(const int*& pSrc, const int* pSrcEnd, char32_t*& pDest, char32_t* pDestEnd);

	#if EA_CHAR8_UNIQUE
		bool DecodePart(const char8_t*& pSrc, const char8_t* pSrcEnd, char8_t*&  pDest, char8_t*  pDestEnd);

		bool DecodePart(const char8_t*& pSrc, const char8_t* pSrcEnd, char*&     pDest, char*     pDestEnd);
		bool DecodePart(const char8_t*& pSrc, const char8_t* pSrcEnd, char16_t*& pDest, char16_t* pDestEnd);
		bool DecodePart(const char8_t*& pSrc, const char8_t* pSrcEnd, char32_t*& pDest, char32_t* pDestEnd);

		bool DecodePart(const char*&     pSrc, const char*     pSrcEnd, char8_t*& pDest, char8_t* pDestEnd);
		bool DecodePart(const char16_t*& pSrc, const char16_t* pSrcEnd, char8_t*& pDest, char8_t* pDestEnd);
		bool DecodePart(const char32_t*& pSrc, const char32_t* pSrcEnd, char8_t*& pDest, char8_t* pDestEnd);
	#endif

	#if EA_WCHAR_UNIQUE
		bool DecodePart(const wchar_t*& pSrc, const wchar_t* pSrcEnd, wchar_t*&  pDest, wchar_t*  pDestEnd);

		bool DecodePart(const wchar_t*& pSrc, const wchar_t* pSrcEnd, char*&     pDest, char*     pDestEnd);
		bool DecodePart(const wchar_t*& pSrc, const wchar_t* pSrcEnd, char16_t*& pDest, char16_t* pDestEnd);
		bool DecodePart(const wchar_t*& pSrc, const wchar_t* pSrcEnd, char32_t*& pDest, char32_t* pDestEnd);

		bool DecodePart(const char*&     pSrc, const char*     pSrcEnd, wchar_t*& pDest, wchar_t* pDestEnd);
		bool DecodePart(const char16_t*& pSrc, const char16_t* pSrcEnd, wchar_t*& pDest, wchar_t* pDestEnd);
		bool DecodePart(const char32_t*& pSrc, const char32_t* pSrcEnd, wchar_t*& pDest, wchar_t* pDestEnd);
	#endif

	#if EA_CHAR8_UNIQUE && EA_WCHAR_UNIQUE
		bool DecodePart(const char8_t*& pSrc, const char8_t* pSrcEnd, wchar_t*& pDest, wchar_t* pDestEnd);
		bool DecodePart(const wchar_t*& pSrc, const wchar_t* pSrcEnd, char8_t*& pDest, char8_t* pDestEnd);
	#endif


	#if EA_WCHAR_UNIQUE
		inline bool DecodePart(const wchar_t*& pSrc, const wchar_t* pSrcEnd, wchar_t*& pDest, wchar_t* pDestEnd)
		{
			return DecodePart(reinterpret_cast<const char*&>(pSrc), reinterpret_cast<const char*>(pSrcEnd), reinterpret_cast<char*&>(pDest), reinterpret_cast<char*&>(pDestEnd));
		}

		inline bool DecodePart(const wchar_t*& pSrc, const wchar_t* pSrcEnd, char*& pDest, char* pDestEnd)
		{
		#if (EA_WCHAR_SIZE == 2)
			return DecodePart(reinterpret_cast<const char16_t*&>(pSrc), reinterpret_cast<const char16_t*>(pSrcEnd), pDest, pDestEnd);
		#elif (EA_WCHAR_SIZE == 4)
			return DecodePart(reinterpret_cast<const char32_t*&>(pSrc), reinterpret_cast<const char32_t*>(pSrcEnd), pDest, pDestEnd);
		#endif
		}

		inline bool DecodePart(const wchar_t*& pSrc, const wchar_t* pSrcEnd, char16_t*& pDest, char16_t* pDestEnd)
		{
		#if (EA_WCHAR_SIZE == 2)
			return DecodePart(reinterpret_cast<const char16_t*&>(pSrc), reinterpret_cast<const char16_t*>(pSrcEnd), pDest, pDestEnd);
		#elif (EA_WCHAR_SIZE == 4)
			return DecodePart(reinterpret_cast<const char32_t*&>(pSrc), reinterpret_cast<const char32_t*>(pSrcEnd), pDest, pDestEnd);
		#endif
		}

		inline bool DecodePart(const wchar_t*& pSrc, const wchar_t* pSrcEnd, char32_t*& pDest, char32_t* pDestEnd)
		{
		#if (EA_WCHAR_SIZE == 2)
			return DecodePart(reinterpret_cast<const char16_t*&>(pSrc), reinterpret_cast<const char16_t*>(pSrcEnd), pDest, pDestEnd);
		#elif (EA_WCHAR_SIZE == 4)
			return DecodePart(reinterpret_cast<const char32_t*&>(pSrc), reinterpret_cast<const char32_t*>(pSrcEnd), pDest, pDestEnd);
		#endif
		}

		inline bool DecodePart(const char*& pSrc, const char* pSrcEnd, wchar_t*& pDest, wchar_t* pDestEnd)
		{
		#if (EA_WCHAR_SIZE == 2)
			return DecodePart(pSrc, pSrcEnd, reinterpret_cast<char16_t*&>(pDest), reinterpret_cast<char16_t*>(pDestEnd));
		#elif (EA_WCHAR_SIZE == 4)
			return DecodePart(pSrc, pSrcEnd, reinterpret_cast<char32_t*&>(pDest), reinterpret_cast<char32_t*>(pDestEnd));
		#endif
		}

		inline bool DecodePart(const char16_t*& pSrc, const char16_t* pSrcEnd, wchar_t*& pDest, wchar_t* pDestEnd)
		{
		#if (EA_WCHAR_SIZE == 2)
			return DecodePart(pSrc, pSrcEnd, reinterpret_cast<char16_t*&>(pDest), reinterpret_cast<char16_t*>(pDestEnd));
		#elif (EA_WCHAR_SIZE == 4)
			return DecodePart(pSrc, pSrcEnd, reinterpret_cast<char32_t*&>(pDest), reinterpret_cast<char32_t*>(pDestEnd));
		#endif
		}

		inline bool DecodePart(const char32_t*& pSrc, const char32_t* pSrcEnd, wchar_t*& pDest, wchar_t* pDestEnd)
		{
		#if (EA_WCHAR_SIZE == 2)
			return DecodePart(pSrc, pSrcEnd, reinterpret_cast<char16_t*&>(pDest), reinterpret_cast<char16_t*>(pDestEnd));
		#elif (EA_WCHAR_SIZE == 4)
			return DecodePart(pSrc, pSrcEnd, reinterpret_cast<char32_t*&>(pDest), reinterpret_cast<char32_t*>(pDestEnd));
		#endif
		}
	#endif

	#if EA_CHAR8_UNIQUE
	    inline bool DecodePart(const char8_t*& pSrc, const char8_t* pSrcEnd, char8_t*& pDest, char8_t* pDestEnd)
	    {
		    return DecodePart(reinterpret_cast<const char*&>(pSrc), reinterpret_cast<const char*>(pSrcEnd), reinterpret_cast<char*&>(pDest), reinterpret_cast<char*&>(pDestEnd));
	    }

	    inline bool DecodePart(const char8_t*& pSrc, const char8_t* pSrcEnd, char*& pDest, char* pDestEnd)
	    {
		    return DecodePart(reinterpret_cast<const char*&>(pSrc), reinterpret_cast<const char*>(pSrcEnd), pDest, pDestEnd);
	    }

	    inline bool DecodePart(const char8_t*& pSrc, const char8_t* pSrcEnd, char16_t*& pDest, char16_t* pDestEnd)
	    {
		    return DecodePart(reinterpret_cast<const char*&>(pSrc), reinterpret_cast<const char*>(pSrcEnd), pDest, pDestEnd);
	    }

	    inline bool DecodePart(const char8_t*& pSrc, const char8_t* pSrcEnd, char32_t*& pDest, char32_t* pDestEnd)
	    {
		    return DecodePart(reinterpret_cast<const char*&>(pSrc), reinterpret_cast<const char*>(pSrcEnd), pDest, pDestEnd);
	    }

		inline bool DecodePart(const char*& pSrc, const char* pSrcEnd, char8_t*& pDest, char8_t* pDestEnd)
		{
			return DecodePart(pSrc, pSrcEnd, reinterpret_cast<char*&>(pDest), reinterpret_cast<char*&>(pDestEnd));
		}

		inline bool DecodePart(const char16_t*& pSrc, const char16_t* pSrcEnd, char8_t*& pDest, char8_t* pDestEnd)
		{
			return DecodePart(pSrc, pSrcEnd, reinterpret_cast<char*&>(pDest), reinterpret_cast<char*&>(pDestEnd));
		}

		inline bool DecodePart(const char32_t*& pSrc, const char32_t* pSrcEnd, char8_t*& pDest, char8_t* pDestEnd)
		{
			return DecodePart(pSrc, pSrcEnd, reinterpret_cast<char*&>(pDest), reinterpret_cast<char*&>(pDestEnd));
		}
    #endif

	#if EA_CHAR8_UNIQUE && EA_WCHAR_UNIQUE
		inline bool DecodePart(const char8_t*&  pSrc, const char8_t*  pSrcEnd, wchar_t*&  pDest, wchar_t*  pDestEnd)
		{
		#if (EA_WCHAR_SIZE == 2)
		    return DecodePart(pSrc, pSrcEnd, reinterpret_cast<char16_t*&>(pDest), reinterpret_cast<char16_t*>(pDestEnd));
		#elif (EA_WCHAR_SIZE == 4)
		    return DecodePart(pSrc, pSrcEnd, reinterpret_cast<char32_t*&>(pDest), reinterpret_cast<char32_t*>(pDestEnd));
		#endif
		}

		inline bool DecodePart(const wchar_t*&  pSrc, const wchar_t*  pSrcEnd, char8_t*&  pDest, char8_t*  pDestEnd)
		{
		#if (EA_WCHAR_SIZE == 2)
			return DecodePart(reinterpret_cast<const char16_t*&>(pSrc), reinterpret_cast<const char16_t*>(pSrcEnd), reinterpret_cast<char*&>(pDest), reinterpret_cast<char*>(pDestEnd));
		#elif (EA_WCHAR_SIZE == 4)
			return DecodePart(reinterpret_cast<const char32_t*&>(pSrc), reinterpret_cast<const char32_t*>(pSrcEnd), reinterpret_cast<char*&>(pDest), reinterpret_cast<char*>(pDestEnd));
		#endif
		}
	#endif

	///////////////////////////////////////////////////////////////////////////////
	// 'char traits' functionality
	//
	inline char CharToLower(char c)
		{ return (char)tolower((uint8_t)c); }

	template<typename T>
	inline T CharToLower(T c)
		{ if((unsigned)c <= 0xff) return (T)tolower((uint8_t)c); return c; }


	inline char CharToUpper(char c)
		{ return (char)toupper((uint8_t)c); }

	template<typename T>
	inline T CharToUpper(T c)
		{ if((unsigned)c <= 0xff) return (T)toupper((uint8_t)c); return c; }


	template <typename T>
	int Compare(const T* p1, const T* p2, size_t n)
	{
		for(; n > 0; ++p1, ++p2, --n)
		{
			if(*p1 != *p2)
				return (static_cast<typename make_unsigned<T>::type>(*p1) < 
						static_cast<typename make_unsigned<T>::type>(*p2)) ? -1 : 1;
		}
		return 0;
	}

	inline int Compare(const char* p1, const char* p2, size_t n)
	{
		if (n > 0)
			return memcmp(p1, p2, n);
		else
			return 0;
	}


	template <typename T>
	inline int CompareI(const T* p1, const T* p2, size_t n)
	{
		for(; n > 0; ++p1, ++p2, --n)
		{
			const T c1 = CharToLower(*p1);
			const T c2 = CharToLower(*p2);

			if(c1 != c2)
				return (static_cast<typename make_unsigned<T>::type>(c1) < 
						static_cast<typename make_unsigned<T>::type>(c2)) ? -1 : 1;
		}
		return 0;
	}


	template<typename T>
	inline const T* Find(const T* p, T c, size_t n)
	{
		for(; n > 0; --n, ++p)
		{
			if(*p == c)
				return p;
		}

		return NULL;
	}

	inline const char* Find(const char* p, char c, size_t n)
	{
		return (const char*)memchr(p, c, n);
	}


	template<typename T>
	inline EA_CPP14_CONSTEXPR size_t CharStrlen(const T* p)
	{
		const auto* pCurrent = p;
		while(*pCurrent)
			++pCurrent;
		return (size_t)(pCurrent - p);
	}


	// If either pDestination or pSource is an invalid or null pointer, the behavior is undefined, even if (pSourceEnd - pSource) is zero.
	template <typename T>
	inline T* CharStringUninitializedCopy(const T* pSource, const T* pSourceEnd, T* pDestination)
	{
		memmove(pDestination, pSource, (size_t)(pSourceEnd - pSource) * sizeof(T));
		return pDestination + (pSourceEnd - pSource);
	}


	template <typename T>
	const T* CharTypeStringFindEnd(const T* pBegin, const T* pEnd, T c)
	{
		const T* pTemp = pEnd;
		while(--pTemp >= pBegin)
		{
			if(*pTemp == c)
				return pTemp;
		}

		return pEnd;
	}

    
	template <typename T>
	const T* CharTypeStringRSearch(const T* p1Begin, const T* p1End, 
								   const T* p2Begin, const T* p2End)
	{
		// Test for zero length strings, in which case we have a match or a failure, 
		// but the return value is the same either way.
		if((p1Begin == p1End) || (p2Begin == p2End))
			return p1Begin;

		// Test for a pattern of length 1.
		if((p2Begin + 1) == p2End)
			return CharTypeStringFindEnd(p1Begin, p1End, *p2Begin);

		// Test for search string length being longer than string length.
		if((p2End - p2Begin) > (p1End - p1Begin))
			return p1End;

		// General case.
		const T* pSearchEnd = (p1End - (p2End - p2Begin) + 1);
		const T* pCurrent1;
		const T* pCurrent2;

		while(pSearchEnd != p1Begin)
		{
			// Search for the last occurrence of *p2Begin.
			pCurrent1 = CharTypeStringFindEnd(p1Begin, pSearchEnd, *p2Begin);
			if(pCurrent1 == pSearchEnd) // If the first char of p2 wasn't found, 
				return p1End;           // then we immediately have failure.

			// In this case, *pTemp == *p2Begin. So compare the rest.
			pCurrent2 = p2Begin;
			while(*pCurrent1++ == *pCurrent2++)
			{
				if(pCurrent2 == p2End)
					return (pCurrent1 - (p2End - p2Begin));
			}

			// A smarter algorithm might know to subtract more than just one,
			// but in most cases it won't make much difference anyway.
			--pSearchEnd;
		}

		return p1End;
	}


	template <typename T>
	inline const T* CharTypeStringFindFirstOf(const T* p1Begin, const T* p1End, const T* p2Begin, const T* p2End)
	{
		for (; p1Begin != p1End; ++p1Begin)
		{
			for (const T* pTemp = p2Begin; pTemp != p2End; ++pTemp)
			{
				if (*p1Begin == *pTemp)
					return p1Begin;
			}
		}
		return p1End;
	}


	template <typename T>
	inline const T* CharTypeStringRFindFirstNotOf(const T* p1RBegin, const T* p1REnd, const T* p2Begin, const T* p2End)
	{
		for (; p1RBegin != p1REnd; --p1RBegin)
		{
			const T* pTemp;
			for (pTemp = p2Begin; pTemp != p2End; ++pTemp)
			{
				if (*(p1RBegin - 1) == *pTemp)
					break;
			}
			if (pTemp == p2End)
				return p1RBegin;
		}
		return p1REnd;
	}


	template <typename T>
	inline const T* CharTypeStringFindFirstNotOf(const T* p1Begin, const T* p1End, const T* p2Begin, const T* p2End)
	{
		for (; p1Begin != p1End; ++p1Begin)
		{
			const T* pTemp;
			for (pTemp = p2Begin; pTemp != p2End; ++pTemp)
			{
				if (*p1Begin == *pTemp)
					break;
			}
			if (pTemp == p2End)
				return p1Begin;
		}
		return p1End;
	}


	template <typename T>
	inline const T* CharTypeStringRFindFirstOf(const T* p1RBegin, const T* p1REnd, const T* p2Begin, const T* p2End)
	{
		for (; p1RBegin != p1REnd; --p1RBegin)
		{
			for (const T* pTemp = p2Begin; pTemp != p2End; ++pTemp)
			{
				if (*(p1RBegin - 1) == *pTemp)
					return p1RBegin;
			}
		}
		return p1REnd;
	}


	template <typename T>
	inline const T* CharTypeStringRFind(const T* pRBegin, const T* pREnd, const T c)
	{
		while (pRBegin > pREnd)
		{
			if (*(pRBegin - 1) == c)
				return pRBegin;
			--pRBegin;
		}
		return pREnd;
	}


	inline char* CharStringUninitializedFillN(char* pDestination, size_t n, const char c)
	{
		if(n) // Some compilers (e.g. GCC 4.3+) generate a warning (which can't be disabled) if you call memset with a size of 0.
			memset(pDestination, (uint8_t)c, (size_t)n);
		return pDestination + n;
	}

	template<typename T>
	inline T* CharStringUninitializedFillN(T* pDestination, size_t n, const T c)
	{
		T * pDest           = pDestination;
		const T* const pEnd = pDestination + n;
		while(pDest < pEnd)
			*pDest++ = c;
		return pDestination + n;
	}


	inline char* CharTypeAssignN(char* pDestination, size_t n, char c)
	{
		if(n) // Some compilers (e.g. GCC 4.3+) generate a warning (which can't be disabled) if you call memset with a size of 0.
			return (char*)memset(pDestination, c, (size_t)n);
		return pDestination;
	}

	template<typename T>
	inline T* CharTypeAssignN(T* pDestination, size_t n, T c)
	{
		T* pDest            = pDestination;
		const T* const pEnd = pDestination + n;
		while(pDest < pEnd)
			*pDest++ = c;
		return pDestination;
	}
} // namespace eastl

#endif // EASTL_CHAR_TRAITS_H
