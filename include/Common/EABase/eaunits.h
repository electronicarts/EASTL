/*-----------------------------------------------------------------------------
 * eaunits.h
 *
 * Copyright (c) Electronic Arts Inc. All rights reserved.
 *---------------------------------------------------------------------------*/


#ifndef INCLUDED_eaunits_h
#define INCLUDED_eaunits_h

#include <EABase/eabase.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif

// Defining common SI unit macros.
//
// The mebibyte is a multiple of the unit byte for digital information. Technically a
// megabyte (MB) is a power of ten, while a mebibyte (MiB) is a power of two,
// appropriate for binary machines. Many Linux distributions use the unit, but it is
// not widely acknowledged within the industry or media.
// Reference: https://en.wikipedia.org/wiki/Mebibyte
//
// Examples:
// 	auto size1 = EA_KILOBYTE(16);
// 	auto size2 = EA_MEGABYTE(128);
// 	auto size3 = EA_MEBIBYTE(8);
// 	auto size4 = EA_GIBIBYTE(8);

// define byte for completeness
#define EA_BYTE(x) (x)

// Decimal SI units
#define EA_KILOBYTE(x) (size_t(x) * 1000)
#define EA_MEGABYTE(x) (size_t(x) * 1000 * 1000)
#define EA_GIGABYTE(x) (size_t(x) * 1000 * 1000 * 1000)
#define EA_TERABYTE(x) (size_t(x) * 1000 * 1000 * 1000 * 1000)
#define EA_PETABYTE(x) (size_t(x) * 1000 * 1000 * 1000 * 1000 * 1000)
#define EA_EXABYTE(x)  (size_t(x) * 1000 * 1000 * 1000 * 1000 * 1000 * 1000)

// Binary SI units
#define EA_KIBIBYTE(x) (size_t(x) * 1024)
#define EA_MEBIBYTE(x) (size_t(x) * 1024 * 1024)
#define EA_GIBIBYTE(x) (size_t(x) * 1024 * 1024 * 1024)
#define EA_TEBIBYTE(x) (size_t(x) * 1024 * 1024 * 1024 * 1024)
#define EA_PEBIBYTE(x) (size_t(x) * 1024 * 1024 * 1024 * 1024 * 1024)
#define EA_EXBIBYTE(x) (size_t(x) * 1024 * 1024 * 1024 * 1024 * 1024 * 1024)

#endif // INCLUDED_earesult_H




