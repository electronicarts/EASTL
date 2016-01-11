///////////////////////////////////////////////////////////////////////////////
// EASTLNewOperatorGuard.inl
//
// Copyright (c) Electronic Arts. All Rights Reserved.  
///////////////////////////////////////////////////////////////////////////////

#include <EABase/eabase.h>
#include <EAAssert/eaassert.h>

#ifndef EATEST_EASTLNEWOPERATORGUARD_INL
#define EATEST_EASTLNEWOPERATORGUARD_INL

///////////////////////////////////////////////////////////////////////////////
// EASTL requires the following new operators to be defined.  
// They are provided here due to linkage requirements only.
///////////////////////////////////////////////////////////////////////////////

void* operator new[](size_t, const char*, int, unsigned, const char*, int) EA_NOEXCEPT
{
    EA_FAIL_MSG("Unexpected call to global new operator.  Does EASTL have an ICoreAllocator set?");
    return static_cast<void*>(0);  // perform a static_cast to prevent a GCC warning.
}

void* operator new[](size_t, size_t, size_t, const char*, int, unsigned, const char*, int) EA_NOEXCEPT
{
    EA_FAIL_MSG("Unexpected call to global new operator.  Does EASTL have an ICoreAllocator set?");       
    return static_cast<void*>(0);  // perform a static_cast to prevent a GCC warning.
}
 
#endif // header include guard
 
