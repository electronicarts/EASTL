/*-----------------------------------------------------------------------------
 * version.h
 *
 * Copyright (c) Electronic Arts Inc. All rights reserved.
 *---------------------------------------------------------------------------*/

#ifndef INCLUDED_EABASE_VERSION_H
#define INCLUDED_EABASE_VERSION_H

///////////////////////////////////////////////////////////////////////////////
// EABASE_VERSION
//
// We more or less follow the conventional EA packaging approach to versioning 
// here. A primary distinction here is that minor versions are defined as two
// digit entities (e.g. .03") instead of minimal digit entities ".3"). The logic
// here is that the value is a counter and not a floating point fraction.
// Note that the major version doesn't have leading zeros.
//
// Example version strings:
//      "0.91.00"   // Major version 0, minor version 91, patch version 0. 
//      "1.00.00"   // Major version 1, minor and patch version 0.
//      "3.10.02"   // Major version 3, minor version 10, patch version 02.
//     "12.03.01"   // Major version 12, minor version 03, patch version 
//
// Example usage:
//     printf("EABASE version: %s", EABASE_VERSION);
//     printf("EABASE version: %d.%d.%d", EABASE_VERSION_N / 10000 % 100, EABASE_VERSION_N / 100 % 100, EABASE_VERSION_N % 100);
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EABASE_VERSION
    #define EABASE_VERSION "2.09.03"
    #define EABASE_VERSION_N 20903
#endif

#endif
