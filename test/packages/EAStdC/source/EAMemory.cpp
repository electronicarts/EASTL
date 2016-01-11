/////////////////////////////////////////////////////////////////////////////
// EAMemory.cpp
//
// Copyright (c) Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EABase/eabase.h>
#include <EAStdC/EAMemory.h>


namespace EA {
namespace StdC {
	
EASTDC_API const void* Memcheck8(const void* p, uint8_t c, size_t byteCount)
{
	for(const uint8_t* p8 = (const uint8_t*)p; byteCount > 0; ++p8, --byteCount)
	{
		if(*p8 != c)
			return p8;
	}

	return NULL;
}


EASTDC_API const void* Memcheck16(const void* p, uint16_t c, size_t byteCount)
{
	union U16 {
		uint16_t c16;
		uint8_t  c8[2];
	};
	const U16 u = { c };
	size_t    i = (size_t)((uintptr_t)p % 2);

	for(const uint8_t* p8 = (const uint8_t*)p, *p8End = (const uint8_t*)p + byteCount; p8 != p8End; ++p8, i ^= 1)
	{
		if(*p8 != u.c8[i])
			return p8;
	}

	return NULL;
}


EASTDC_API const void* Memcheck32(const void* p, uint32_t c, size_t byteCount)
{
	union U32 {
		uint32_t c32;
		uint8_t  c8[4];
	};
	const U32 u = { c };
	size_t    i = (size_t)((uintptr_t)p % 4);

	// This code could be a little faster if it could work with an aligned 
	// destination and do word compares. There are some pitfalls to be careful
	// of which may make the effort not worth it in practice for typical uses 
	// of this code. In particular we need to make sure that word compares are 
	// done with word-aligned memory, and that may mean using a version of 
	// the c argument which has bytes rotated from their current position.
	for(const uint8_t* p8 = (const uint8_t*)p, *p8End = (const uint8_t*)p + byteCount; p8 != p8End; ++p8, i = (i + 1) % 4)
	{
		if(*p8 != u.c8[i])
			return p8;
	}

	return NULL;
}


EASTDC_API const void* Memcheck64(const void* p, uint64_t c, size_t byteCount)
{
	union U64 {
		uint64_t c64;
		uint8_t  c8[8];
	};
	const U64 u = { c };
	size_t    i = (size_t)((uintptr_t)p % 8);

	for(const uint8_t* p8 = (const uint8_t*)p, *p8End = (const uint8_t*)p + byteCount; p8 != p8End; ++p8, i = (i + 1) % 8)
	{
		if(*p8 != u.c8[i])
			return p8;
	}

	return NULL;
}

}}
