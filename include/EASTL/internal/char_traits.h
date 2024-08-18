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
#include <EASTL/algorithm.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <ctype.h>              // toupper, etc.
#include <string.h>             // memset, etc.
EA_RESTORE_ALL_VC_WARNINGS()

namespace eastl
{
	namespace details
	{
#if defined(EA_COMPILER_CPP17_ENABLED)
		// Helper to detect if wchar_t is the native type for the current platform or if -fshort-wchar was used.
		// When that flag is used all string builtins and C Standard Library functions are not usable.
		constexpr bool UseNativeWideChar()
		{
#if defined(EA_COMPILER_MSVC)
			return true; // Irrelevant flag for windows.
#elif defined(EA_PLATFORM_SONY) && defined(EA_PLATFORM_POSIX) && defined(EA_PLATFORM_CONSOLE)
			return true; // Sony consoles use short wchar_t disregarding the flag.
#elif defined(EA_PLATFORM_POSIX) || defined(EA_PLATFORM_UNIX)
			return sizeof(wchar_t) == 4;
#endif
		}
#endif
	}
	
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

#if defined(EA_COMPILER_CPP17_ENABLED)
	// All main compilers offer a constexpr __builtin_memcmp as soon as C++17 was available.
	constexpr int Compare(const char* p1, const char* p2, size_t n) { return __builtin_memcmp(p1, p2, n); }

#if !defined(EA_COMPILER_GNUC)
	// GCC doesn't offer __builtin_wmemcmp.
	constexpr int Compare(const wchar_t* p1, const wchar_t* p2, size_t n)
	{
		if constexpr (details::UseNativeWideChar())
			return __builtin_wmemcmp(p1, p2, n);
		else
			return Compare<wchar_t>(p1, p2, n);
	}
#endif // !defined(EA_COMPILER_GNUC)
#else
	inline int Compare(const char* p1, const char* p2, size_t n)
	{
		if (n > 0)
			return memcmp(p1, p2, n);
		else
			return 0;
	}
#endif

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
	inline EA_CPP14_CONSTEXPR const T* Find(const T* p, T c, size_t n)
	{
		for(; n > 0; --n, ++p)
		{
			if(*p == c)
				return p;
		}

		return nullptr;
	}

#if defined(EA_COMPILER_CPP17_ENABLED) && defined(EA_COMPILER_CLANG)
	// Only clang have __builtin_char_memchr.
	// __builtin_memchr doesn't work in a constexpr context since we need to cast the returned void* to a char*.
	inline constexpr const char* Find(const char* p, char c, size_t n)
	{
		return __builtin_char_memchr(p, c, n);
	}
#else
	inline const char* Find(const char* p, char c, size_t n)
	{
		return (const char*)memchr(p, c, n);
	}
#endif

	template <typename T>
	inline EA_CPP14_CONSTEXPR size_t CharStrlen(const T* p)
	{
		const auto* pCurrent = p;
		while (*pCurrent)
			++pCurrent;
		return (size_t)(pCurrent - p);
	}

#if defined(EA_COMPILER_CPP17_ENABLED) && !defined(EA_COMPILER_GNUC)
	// So far, GCC seems to struggle with builtin_strlen: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70816
	// MSVC and Clang support both builtins as soon as C++17 was available.
	constexpr size_t CharStrlen(const char* p) { return __builtin_strlen(p); }

	constexpr size_t CharStrlen(const wchar_t* p)
	{
		if constexpr (details::UseNativeWideChar())
			return __builtin_wcslen(p);
		else
			return CharStrlen<wchar_t>(p);
	}
#endif

	// If either pDestination or pSource is an invalid or null pointer, the behavior is undefined, even if (pSourceEnd - pSource) is zero.
	template <typename T>
	inline T* CharStringUninitializedCopy(const T* pSource, const T* pSourceEnd, T* pDestination)
	{
		memmove(pDestination, pSource, (size_t)(pSourceEnd - pSource) * sizeof(T));
		return pDestination + (pSourceEnd - pSource);
	}


	// CharTypeStringFindEnd
	// Specialized char version of STL find() from back function.
	// Not the same as RFind because search range is specified as forward iterators.
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


	// CharTypeStringSearch
	// Specialized value_type version of STL search() function.
	// Purpose: find p2 within p1. Return p1End if not found or if either string is zero length.
	template <typename T>
	const T* CharTypeStringSearch(const T* p1Begin, const T* p1End,
								  const T* p2Begin, const T* p2End)
	{
		// Test for zero length strings, in which case we have a match or a failure,
		// but the return value is the same either way.
		if((p1Begin == p1End) || (p2Begin == p2End))
			return p1Begin;

		// Test for a pattern of length 1.
		if((p2Begin + 1) == p2End)
			return eastl::find(p1Begin, p1End, *p2Begin);

		// General case.
		const T* pTemp;
		const T* pTemp1 = (p2Begin + 1);
		const T* pCurrent = p1Begin;

		while(p1Begin != p1End)
		{
			p1Begin = eastl::find(p1Begin, p1End, *p2Begin);
			if(p1Begin == p1End)
				return p1End;

			pTemp = pTemp1;
			pCurrent = p1Begin;
			if(++pCurrent == p1End)
				return p1End;

			while(*pCurrent == *pTemp)
			{
				if(++pTemp == p2End)
					return p1Begin;
				if(++pCurrent == p1End)
					return p1End;
			}

			++p1Begin;
		}

		return p1Begin;
	}


	// CharTypeStringRSearch
	// Specialized value_type version of STL find_end() function (which really is a reverse search function).
	// Purpose: find last instance of p2 within p1. Return p1End if not found or if either string is zero length.
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

		const T* pMatchCandidate;
		while((pMatchCandidate = CharTypeStringFindEnd(p1Begin, pSearchEnd, *p2Begin)) != pSearchEnd)
		{
			// In this case, *pMatchCandidate == *p2Begin. So compare the rest.
			const T* pCurrent1 = pMatchCandidate;
			const T* pCurrent2 = p2Begin;
			while(*pCurrent1++ == *pCurrent2++)
			{
				if(pCurrent2 == p2End)
					return (pCurrent1 - (p2End - p2Begin));
			}

			// This match failed, search again with this new end.
			pSearchEnd = pMatchCandidate;
		}

		return p1End;
	}


	// CharTypeStringFindFirstOf
	// Specialized value_type version of STL find_first_of() function.
	// This function is much like the C runtime strtok function, except the strings aren't null-terminated.
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


	// CharTypeStringRFindFirstNotOf
	// Specialized value_type version of STL find_first_not_of() function in reverse.
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


	// CharTypeStringFindFirstNotOf
	// Specialized value_type version of STL find_first_not_of() function.
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


	// CharTypeStringRFindFirstOf
	// Specialized value_type version of STL find_first_of() function in reverse.
	// This function is much like the C runtime strtok function, except the strings aren't null-terminated.
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


	// CharTypeStringRFind
	// Specialized value_type version of STL find() function in reverse.
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
