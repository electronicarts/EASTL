///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EASTL/internal/config.h>
#include <EASTL/string.h>
#include <EABase/eabase.h>
#include <string.h>


namespace eastl
{
	///////////////////////////////////////////////////////////////////////////////
	// Converters for DecodePart
	//
	// For some decent documentation about conversions, see:
	//     http://tidy.sourceforge.net/cgi-bin/lxr/source/src/utf8.c
	//     
	///////////////////////////////////////////////////////////////////////////////

	// Requires that pDest have a capacity of at least 6 chars.
	// Sets pResult to '\1' in the case that c is an invalid UCS4 char.
	inline bool UCS4ToUTF8(uint32_t c, char*& pResult)
	{
		if(c < 0x00000080)
			*pResult++ = (char)(uint8_t)c;
		else if(c < 0x0800)
		{
			*pResult++ = (char)(uint8_t)(0xC0 | (c >> 6));
			*pResult++ = (char)(uint8_t)(0x80 | (c & 0x3F));
		}
		else if(c <= 0x0000FFFF)
		{
			*pResult++ = (char)(uint8_t)(0xE0 | (c >> 12));
			*pResult++ = (char)(uint8_t)(0x80 | ((c >> 6) & 0x3F));
			*pResult++ = (char)(uint8_t)(0x80 | (c & 0x3F));
		}
		else if(c <= 0x001FFFFF)
		{
			*pResult++ = (char)(uint8_t)(0xF0 | (c >> 18));
			*pResult++ = (char)(uint8_t)(0x80 | ((c >> 12) & 0x3F));
			*pResult++ = (char)(uint8_t)(0x80 | ((c >> 6) & 0x3F));
			*pResult++ = (char)(uint8_t)(0x80 | (c & 0x3F));
		}
		else if(c <= 0x003FFFFFF)
		{
			*pResult++ = (char)(uint8_t)(0xF8 | (c >> 24));
			*pResult++ = (char)(uint8_t)(0x80 | (c >> 18));
			*pResult++ = (char)(uint8_t)(0x80 | ((c >> 12) & 0x3F));
			*pResult++ = (char)(uint8_t)(0x80 | ((c >> 6) & 0x3F));
			*pResult++ = (char)(uint8_t)(0x80 | (c & 0x3F));
		}
		else if(c <= 0x7FFFFFFF)
		{
			*pResult++ = (char)(uint8_t)(0xFC | (c >> 30));
			*pResult++ = (char)(uint8_t)(0x80 | ((c >> 24) & 0x3F));
			*pResult++ = (char)(uint8_t)(0x80 | ((c >> 18) & 0x3F));
			*pResult++ = (char)(uint8_t)(0x80 | ((c >> 12) & 0x3F));
			*pResult++ = (char)(uint8_t)(0x80 | ((c >> 6) & 0x3F));
			*pResult++ = (char)(uint8_t)(0x80 | (c & 0x3F));
		}
		else
		{
			// values >= 0x80000000 can't be converted to UTF8.
			*pResult++ = '\1';
			return false;
		}

		return true;
	}


	// Requires that pResult have a capacity of at least 3 chars.
	// Sets pResult to '\1' in the case that c is an invalid UCS4 char.
	inline bool UCS2ToUTF8(uint16_t c, char*& pResult)
	{
		return UCS4ToUTF8(c, pResult);
	}


	// Sets result to 0xffff in the case that the input UTF8 sequence is bad.
	// 32 bit 0xffffffff is an invalid UCS4 code point, so we can't use that as an error return value.
	inline bool UTF8ToUCS4(const char*& p, const char* pEnd, uint32_t& result)
	{
		// This could likely be implemented in a faster-executing way that uses tables.

		bool           success = true;
		uint32_t       c = 0xffff;
		const char* pNext = NULL;

		if(p < pEnd)
		{
			uint8_t cChar0((uint8_t)*p), cChar1, cChar2, cChar3;

			// Asserts are disabled because we don't necessarily want to interrupt runtime execution due to this.
			// EASTL_ASSERT((cChar0 != 0xFE) && (cChar0 != 0xFF));     //  No byte can be 0xFE or 0xFF
			// Code below will effectively catch this error as it goes.

			if(cChar0 < 0x80)
			{
				pNext = p + 1;
				c     = cChar0;
			}
			else
			{
				//EASTL_ASSERT((cChar0 & 0xC0) == 0xC0);              //  The top two bits need to be equal to 1
				if((cChar0 & 0xC0) != 0xC0)
				{
					success = false;
					goto Failure;
				}

				if((cChar0 & 0xE0) == 0xC0)
				{
					pNext = p + 2;

					if(pNext <= pEnd)
					{
						c      = (uint32_t)((cChar0 & 0x1F) << 6);
						cChar1 = static_cast<uint8_t>(p[1]);
						c     |= cChar1 & 0x3F;

						//EASTL_ASSERT((cChar1 & 0xC0) == 0x80);          //  All subsequent code should be b10xxxxxx
						//EASTL_ASSERT(c >= 0x0080 && c < 0x0800);        //  Check that we have the smallest coding
						if(!((cChar1 & 0xC0) == 0x80) ||
						   !(c >= 0x0080 && c < 0x0800))
						{
							success = false;
							goto Failure;
						}
					}
					else
					{
						success = false;
						goto Failure;
					}
				}
				else if((cChar0 & 0xF0) == 0xE0)
				{
					pNext = p + 3;

					if(pNext <= pEnd)
					{
						c      = (uint32_t)((cChar0 & 0xF) << 12);
						cChar1 = static_cast<uint8_t>(p[1]);
						c     |= (cChar1 & 0x3F) << 6;
						cChar2 = static_cast<uint8_t>(p[2]);
						c     |= cChar2 & 0x3F;

						//EASTL_ASSERT((cChar1 & 0xC0) == 0x80);            //  All subsequent code should be b10xxxxxx
						//EASTL_ASSERT((cChar2 & 0xC0) == 0x80);            //  All subsequent code should be b10xxxxxx
						//EASTL_ASSERT(c >= 0x00000800 && c <  0x00010000); //  Check that we have the smallest coding
						if(!((cChar1 & 0xC0) == 0x80) ||
						   !((cChar2 & 0xC0) == 0x80) ||
						   !(c >= 0x00000800 && c <  0x00010000))
						{
							success = false;
							goto Failure;
						}
					}
					else
					{
						success = false;
						goto Failure;
					}
				}
				else if((cChar0 & 0xF8) == 0xF0)
				{
					pNext = p + 4;

					if(pNext <= pEnd)
					{
						c      = (uint32_t)((cChar0 & 0x7) << 18);
						cChar1 = static_cast<uint8_t>(p[1]);
						c     |= (uint32_t)((cChar1 & 0x3F) << 12);
						cChar2 = static_cast<uint8_t>(p[2]);
						c     |= (cChar2 & 0x3F) << 6;
						cChar3 = static_cast<uint8_t>(p[3]);
						c     |= cChar3 & 0x3F;

						//EASTL_ASSERT((cChar0 & 0xf8) == 0xf0);            //  We handle the unicode but not UCS-4
						//EASTL_ASSERT((cChar1 & 0xC0) == 0x80);            //  All subsequent code should be b10xxxxxx
						//EASTL_ASSERT((cChar2 & 0xC0) == 0x80);            //  All subsequent code should be b10xxxxxx
						//EASTL_ASSERT((cChar3 & 0xC0) == 0x80);            //  All subsequent code should be b10xxxxxx
						//EASTL_ASSERT(c >= 0x00010000 && c <= 0x0010FFFF); //  Check that we have the smallest coding, Unicode and not ucs-4
						if(!((cChar0 & 0xf8) == 0xf0) ||
						   !((cChar1 & 0xC0) == 0x80) ||
						   !((cChar2 & 0xC0) == 0x80) ||
						   !(c >= 0x00010000 && c <= 0x0010FFFF))
						{
							success = false;
							goto Failure;
						}
					}
					else
					{
						success = false;
						goto Failure;
					}
				}
				else if((cChar0 & 0xFC) == 0xF8)
				{
					pNext = p + 4;

					if(pNext <= pEnd)
					{
						// To do. We don't currently support extended UCS4 characters.
					}
					else
					{
						success = false;
						goto Failure;
					}
				}
				else if((cChar0 & 0xFE) == 0xFC)
				{
					pNext = p + 5;

					if(pNext <= pEnd)
					{
						// To do. We don't currently support extended UCS4 characters.
					}
					else
					{
						success = false;
						goto Failure;
					}
				}
				else
				{
					success = false;
					goto Failure;
				}
			}
		}
		else
			success = false;

		Failure:
		if(success)
		{
			p = pNext;
			result = c;
		}
		else
		{
			p = p + 1;
			result = 0xffff;
		}

		return success;
	}

	// Sets result to 0xffff in the case that the input UTF8 sequence is bad.
	// The effect of converting UTF8 codepoints > 0xffff to UCS2 (char16_t) is to set all
	// such codepoints to 0xffff. EASTL doesn't have a concept of setting or maintaining 
	// error state for string conversions, though it does have a policy of converting 
	// impossible values to something without generating invalid strings or throwing exceptions.
	inline bool UTF8ToUCS2(const char*& p, const char* pEnd, uint16_t& result)
	{
		uint32_t u32;

		if(UTF8ToUCS4(p, pEnd, u32))
		{
			if(u32 <= 0xffff)
			{
				result = (uint16_t)u32;
				return true;
			}
		}

		result = 0xffff;
		return false;
	}



	///////////////////////////////////////////////////////////////////////////
	// DecodePart
	///////////////////////////////////////////////////////////////////////////

	EASTL_API bool DecodePart(const char*& pSrc, const char* pSrcEnd, char*& pDest, char* pDestEnd)
	{
		size_t sourceSize = (size_t)(pSrcEnd - pSrc);
		size_t destSize   = (size_t)(pDestEnd - pDest);

		if(sourceSize > destSize)
		   sourceSize = destSize;

		memmove(pDest, pSrc, sourceSize * sizeof(*pSrcEnd));

		pSrc  += sourceSize;
		pDest += sourceSize; // Intentionally add sourceSize here.

		return true;
	}

	EASTL_API bool DecodePart(const char*& pSrc, const char* pSrcEnd, char16_t*& pDest, char16_t* pDestEnd)
	{
		bool success = true;

		while(success && (pSrc < pSrcEnd) && (pDest < pDestEnd))
			success = UTF8ToUCS2(pSrc, pSrcEnd, (uint16_t&)*pDest++);

		return success;
	}

	EASTL_API bool DecodePart(const char*& pSrc, const char* pSrcEnd, char32_t*& pDest, char32_t* pDestEnd)
	{
		bool success = true;

		while(success && (pSrc < pSrcEnd) && (pDest < pDestEnd))
			success = UTF8ToUCS4(pSrc, pSrcEnd, (uint32_t&)*pDest++);

		return success;
	}


	EASTL_API bool DecodePart(const char16_t*& pSrc, const char16_t* pSrcEnd, char*& pDest, char* pDestEnd)
	{
		bool success = true;

		EASTL_ASSERT((pDest + 6) < pDestEnd); // The user must provide ample buffer space, preferably 256 chars or more.
		pDestEnd -= 6; // Do this so that we can avoid dest buffer size checking in the loop below and the function it calls.

		while(success && (pSrc < pSrcEnd) && (pDest < pDestEnd))
			success = UCS2ToUTF8(*pSrc++, pDest);

		return success;
	}

	EASTL_API bool DecodePart(const char16_t*& pSrc, const char16_t* pSrcEnd, char16_t*& pDest, char16_t* pDestEnd)
	{
		size_t sourceSize = (size_t)(pSrcEnd - pSrc);
		size_t destSize   = (size_t)(pDestEnd - pDest);

		if(sourceSize > destSize)
		   sourceSize = destSize;

		memmove(pDest, pSrc, sourceSize * sizeof(*pSrcEnd));

		pSrc  += sourceSize;
		pDest += sourceSize; // Intentionally add sourceSize here.

		return true;
	}

	EASTL_API bool DecodePart(const char16_t*& pSrc, const char16_t* pSrcEnd, char32_t*& pDest, char32_t* pDestEnd)
	{
		size_t sourceSize = (size_t)(pSrcEnd - pSrc);
		size_t destSize   = (size_t)(pDestEnd - pDest);

		if(sourceSize > destSize)
		   pSrcEnd = pSrc + destSize;

		while(pSrc != pSrcEnd) // To consider: Improve this by unrolling this loop. Other tricks can improve its speed as well.
			*pDest++ = (char32_t)*pSrc++;

		return true;
	}


	EASTL_API bool DecodePart(const char32_t*& pSrc, const char32_t* pSrcEnd, char*& pDest, char* pDestEnd)
	{
		bool success = true;

		EASTL_ASSERT((pDest + 6) < pDestEnd); // The user must provide ample buffer space, preferably 256 chars or more.
		pDestEnd -= 6; // Do this so that we can avoid dest buffer size checking in the loop below and the function it calls.

		while(success && (pSrc < pSrcEnd) && (pDest < pDestEnd))
			success = UCS4ToUTF8(*pSrc++, pDest);

		return success;
	}

	EASTL_API bool DecodePart(const char32_t*& pSrc, const char32_t* pSrcEnd, char16_t*& pDest, char16_t* pDestEnd)
	{
		size_t sourceSize = (size_t)(pSrcEnd - pSrc);
		size_t destSize   = (size_t)(pDestEnd - pDest);

		if(sourceSize > destSize)
		   pSrcEnd = pSrc + destSize;

		while(pSrc != pSrcEnd) // To consider: Improve this by unrolling this loop. Other tricks can improve its speed as well.
			*pDest++ = (char16_t)*pSrc++;   // This is potentially losing data. We are not converting to UTF16; we are converting to UCS2.

		return true;
	}

	EASTL_API bool DecodePart(const char32_t*& pSrc, const char32_t* pSrcEnd, char32_t*& pDest, char32_t* pDestEnd)
	{
		size_t sourceSize = (size_t)(pSrcEnd - pSrc);
		size_t destSize   = (size_t)(pDestEnd - pDest);

		if(sourceSize > destSize)
		   sourceSize = destSize;

		memmove(pDest, pSrc, sourceSize * sizeof(*pSrcEnd));

		pSrc  += sourceSize;
		pDest += sourceSize; // Intentionally add sourceSize here.

		return true;
	}

	EASTL_API bool DecodePart(const int*& pSrc, const int* pSrcEnd, char*&  pDest, char* pDestEnd)
	{
		bool success = true;

		EASTL_ASSERT((pDest + 6) < pDestEnd); // The user must provide ample buffer space, preferably 256 chars or more.
		pDestEnd -= 6; // Do this so that we can avoid dest buffer size checking in the loop below and the function it calls.

		while(success && (pSrc < pSrcEnd) && (pDest < pDestEnd))
			success = UCS4ToUTF8((uint32_t)(unsigned)*pSrc++, pDest);

		return success;
	}

	EASTL_API bool DecodePart(const int*& pSrc, const int* pSrcEnd, char16_t*& pDest, char16_t* pDestEnd)
	{
		size_t sourceSize = (size_t)(pSrcEnd - pSrc);
		size_t destSize   = (size_t)(pDestEnd - pDest);

		if(sourceSize > destSize)
		   pSrcEnd = pSrc + destSize;

		while(pSrc != pSrcEnd) // To consider: Improve this by unrolling this loop. Other tricks can improve its speed as well.
			*pDest++ = (char16_t)*pSrc++;   // This is potentially losing data. We are not converting to UTF16; we are converting to UCS2.

		return true;
	}

	EASTL_API bool DecodePart(const int*& pSrc, const int* pSrcEnd, char32_t*& pDest, char32_t* pDestEnd)
	{
		size_t sourceSize = (size_t)(pSrcEnd - pSrc);
		size_t destSize   = (size_t)(pDestEnd - pDest);

		if(sourceSize > destSize)
		   pSrcEnd = pSrc + destSize;

		while(pSrc != pSrcEnd) // To consider: Improve this by unrolling this loop. Other tricks can improve its speed as well.
			*pDest++ = (char32_t)*pSrc++;   // This is potentially losing data. We are not converting to UTF16; we are converting to UCS2.

		return true;
	}



} // namespace eastl














