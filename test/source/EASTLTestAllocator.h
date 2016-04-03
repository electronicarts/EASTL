/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTLTEST_ALLOCATOR_H
#define EASTLTEST_ALLOCATOR_H

#include <EABase/eabase.h>
#include <new>

    void* operator new(size_t size);
    void* operator new[](size_t size);
    void* operator new[](size_t size, const char* /*name*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/);
    void* operator new[](size_t size, size_t alignment, size_t /*alignmentOffset*/, const char* /*name*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/);
    void* operator new(size_t size, size_t alignment);
    void* operator new(size_t size, size_t alignment, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE();
    void* operator new[](size_t size, size_t alignment);
    void* operator new[](size_t size, size_t alignment, const std::nothrow_t&)EA_THROW_SPEC_NEW_NONE();
    void operator delete(void* p) EA_THROW_SPEC_DELETE_NONE();
    void operator delete[](void* p) EA_THROW_SPEC_DELETE_NONE();
    void EASTLTest_SetGeneralAllocator();
    bool EASTLTest_ValidateHeap();


#endif // Header include guard
