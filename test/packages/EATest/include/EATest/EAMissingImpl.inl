///////////////////////////////////////////////////////////////////////////////
// EAMissingImpl.inl
//
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
 
///////////////////////////////////////////////////////////////////////////////
// *** Note ***
//
// You must compile this file into your main executable and not into a .lib
// file, otherwise it will not work with GCC and may not work with VC++.
// The C++ standard supports overriding operator new, in which case it will
// override that provided by the library vendor. However, in practice you need
// to compile your operator new into your main executable and not in a library
// that you make. This is so because the linker doesn't know how to distinguish
// between your .lib file and the C runtime .lib files.
///////////////////////////////////////////////////////////////////////////////
 
 
#ifndef EAMISSINGIMPL_INL
#define EAMISSINGIMPL_INL
 

#include <time.h>



#endif // Header include guard






