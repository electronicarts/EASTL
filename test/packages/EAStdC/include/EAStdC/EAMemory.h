///////////////////////////////////////////////////////////////////////////////
// EAMemory.h
//
// Copyright (c) Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTDC_EASPRINTF_H
#define EASTDC_EASPRINTF_H

#include <EABase/eabase.h>
#include <EAAssert/eaassert.h>

#include <string.h>

#define EASTDC_API

namespace EA {
namespace StdC {

EASTDC_API const void* Memcheck8 (const void* p, uint8_t  c, size_t byteCount);
EASTDC_API const void* Memcheck16(const void* p, uint16_t c, size_t byteCount);
EASTDC_API const void* Memcheck32(const void* p, uint32_t c, size_t byteCount);
EASTDC_API const void* Memcheck64(const void* p, uint64_t c, size_t byteCount);


EASTDC_API inline uint8_t* Memset8(void* pDest, uint8_t c, size_t uint8Count)
{
	return (uint8_t*)memset(pDest, c, uint8Count);
}


EASTDC_API inline uint16_t* Memset16(void* pDest, uint16_t c, size_t count)
{
	EA_ASSERT(((uintptr_t)pDest & 1) == 0);

	uint16_t*             cur = (uint16_t*)pDest;
	const uint16_t* const end = (uint16_t*)pDest + count;
	while(cur < end)
		*cur++ = c;

	return (uint16_t*)pDest;
}


EASTDC_API inline uint32_t* Memset32(void* pDest, uint32_t c, size_t count)
{
	EA_ASSERT(((uintptr_t)pDest & 3) == 0);

	uint32_t*             cur = (uint32_t*)pDest;
	const uint32_t* const end = (uint32_t*)pDest + count;
	while(cur < end)
		*cur++ = c;

	return (uint32_t*)pDest;
}


EASTDC_API inline uint64_t* Memset64(void* pDest, uint64_t c, size_t count)
{
	EA_ASSERT(((uintptr_t)pDest & 7) == 0);

	uint64_t*             cur = (uint64_t*)pDest;
	const uint64_t* const end = (uint64_t*)pDest + count;

	while(cur < end)
		*cur++ = c;

	return (uint64_t*)pDest;
}


EASTDC_API inline int Memcmp(const void* p1, const void* p2, size_t n)
{
	return memcmp(p1, p2, n);
}


}} // namespace EA::StdC

#endif  // EASTDC_EASPRINTF_H
