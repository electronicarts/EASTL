/////////////////////////////////////////////////////////////////////////////
// EATextUtil.h
//
// Copyright (c) Electronic Arts. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EASTDC_EATEXTUTIL_H
#define EASTDC_EATEXTUTIL_H

#define EASTDC_API

namespace EA {
namespace StdC {

	// EASTDC_API size_t    UTF8CharSize(const char8_t* p);
	EASTDC_API size_t UTF8CharSize(char16_t c)
	{
		if(c < 0x00000080)
			return 1;
		else if(c < 0x00000800)
			return 2;
		else // if(c < 0x00010000) 
			return 3;

		// The following would be used if the input was 32 bit instead of 16 bit.
		//else if(c < 0x00010000)
		//    return 3;
		//else if(c < 0x00200000)
		//    return 4;
		//else if(c < 0x04000000)
		//    return 5;
		//else if(c <= 0x7fffffff)
		//    return 6;
		//
		//return 1; // Error
	}
	// EASTDC_API size_t    UTF8CharSize(char32_t c);

	EASTDC_API char8_t* UTF8WriteChar(char8_t* p, char16_t c)
	{
		if(c < 0x80)
		{
			*p++ = (char8_t)(uint8_t)c;
		}
		else if(c < 0x0800)
		{
			*p++ = (char8_t)(uint8_t)((c >> 6) | 0xC0);
			*p++ = (char8_t)(uint8_t)((c & 0x3F) | 0x80);
		}
		else // if(c < 0x00010000)
		{
			*p++ = (char8_t)(uint8_t)((c >> 12) | 0xE0);
			*p++ = (char8_t)(uint8_t)(((c >> 6) & 0x3F) | 0x80);
			*p++ = (char8_t)(uint8_t)((c & 0x3F) | 0x80);
		}
		//else
		//{
		//    *p++ = (char8_t)(uint8_t)((c >> 18) | 0xF0);
		//    *p++ = (char8_t)(uint8_t)(((c >> 12) & 0x3F) | 0x80);
		//    *p++ = (char8_t)(uint8_t)(((c >> 6) & 0x3F) | 0x80);
		//    *p++ = (char8_t)(uint8_t)((c & 0x3F) | 0x80);
		//}

		return p;
	}
	// EASTDC_API char8_t*  UTF8WriteChar(char8_t* p, char32_t c);

}} // namespace EA::StdC

#endif  // EASTDC_EATEXTUTIL_H
