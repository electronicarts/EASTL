///////////////////////////////////////////////////////////////////////////////
// Config.h
//
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEST_INTERNAL_CONFIG_H
#define EATEST_INTERNAL_CONFIG_H


#include <EABase/eabase.h>
//#include <EABase/eahave.h> Disabled until eahave.h stabilizes.


///////////////////////////////////////////////////////////////////////////////
// EATEST_VERSION
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
//     printf("EATEST version: %s", EATEST_VERSION);
//     printf("EATEST version: %d.%d.%d", EATEST_VERSION_N / 10000 % 100, EATEST_VERSION_N / 100 % 100, EATEST_VERSION_N % 100);
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EATEST_VERSION
    #define EATEST_VERSION   "2.08.06"
    #define EATEST_VERSION_N  20806
#endif


///////////////////////////////////////////////////////////////////////////////
// EA_HAVE_SYS_PTRACE_H
//
// Defined or not defined, as per <EABase/eahave.h>
//
#if !defined(EA_HAVE_SYS_PTRACE_H) && !defined(EA_NO_HAVE_SYS_PTRACE_H)
    #if defined(EA_PLATFORM_UNIX) && !defined(__CYGWIN__) && (defined(EA_PLATFORM_DESKTOP) || defined(EA_PLATFORM_SERVER))
        #define EA_HAVE_SYS_PTRACE_H 1 /* declares the ptrace function */
    #else
        #define EA_NO_HAVE_SYS_PTRACE_H 1
    #endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EATEST_ALIGNMENT_EXCEPTION_DETECTION_CALLED
//
// Defined as 0 or 1. 
// Specifies whether EnableAlignmentExceptionDetection is automatically called
// upon running tests.
//
#if !defined(EATEST_ALIGNMENT_EXCEPTION_DETECTION_CALLED)
    #define EATEST_ALIGNMENT_EXCEPTION_DETECTION_CALLED 1
#endif


///////////////////////////////////////////////////////////////////////////////
// EATEST_ALLOC_PREFIX
//
// Defined as a string literal. Defaults to this package's name.
// Can be overridden by the user by predefining it or by editing this file.
// This define is used as the default name used by this package for naming
// memory allocations and memory allocators.
//
// All allocations names follow the same naming pattern:
//     <package>/<module>[/<specific usage>]
// 
// Example usage:
//     void* p = pCoreAllocator->Alloc(37, EATEST_ALLOC_PREFIX, 0);
//
// Example usage:
//     gMessageServer.GetMessageQueue().get_allocator().set_name(EATEST_ALLOC_PREFIX "MessageSystem/Queue");
//
#ifndef EATEST_ALLOC_PREFIX
    #define EATEST_ALLOC_PREFIX "EATEST/"
#endif


///////////////////////////////////////////////////////////////////////////////
// EATEST_DLL
//
// Defined as 0 or 1. The default is dependent on the definition of EA_DLL.
// If EA_DLL is defined, then EATEST_DLL is 1, else EATEST_DLL is 0.
// EA_DLL is a define that controls DLL builds within the EAConfig build system. 
// EATEST_DLL controls whether EATEST is built and used as a DLL. 
// Normally you wouldn't do such a thing, but there are use cases for such
// a thing, particularly in the case of embedding C++ into C# applications.
//
#ifndef EATEST_DLL
    #if defined(EA_DLL)
        #define EATEST_DLL 1
    #else
        #define EATEST_DLL 0
    #endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EATEST_API
//
// This is used to label functions as DLL exports under Microsoft platforms.
// If EA_DLL is defined, then the user is building EATest as a DLL and EATest's
// non-templated functions will be exported. EATest template functions are not
// labelled as EATEST_API (and are thus not exported in a DLL build). This is 
// because it's not possible (or at least unsafe) to implement inline templated 
// functions in a DLL.
//
// Example usage of EATEST_API:
//    EATEST_API int someVariable = 10;         // Export someVariable in a DLL build.
//
//    struct EATEST_API SomeClass{              // Export SomeClass and its member functions in a DLL build.
//        EATEST_LOCAL void PrivateMethod();    // Not exported.
//    };
//
//    EATEST_API void SomeFunction();           // Export SomeFunction in a DLL build.
//
// For GCC, see http://gcc.gnu.org/wiki/Visibility
//
#ifndef EATEST_API // If the build file hasn't already defined this to be dllexport...
    #if EATEST_DLL 
        #if defined(_MSC_VER)
            #define EATEST_API      __declspec(dllimport)
            #define EATEST_LOCAL
        #elif defined(__CYGWIN__)
            #define EATEST_API      __attribute__((dllimport))
            #define EATEST_LOCAL
        #elif (defined(__GNUC__) && (__GNUC__ >= 4))
            #define EATEST_API      __attribute__ ((visibility("default")))
            #define EATEST_LOCAL    __attribute__ ((visibility("hidden")))
        #else
            #define EATEST_API
            #define EATEST_LOCAL
        #endif
    #else
        #define EATEST_API
        #define EATEST_LOCAL
    #endif
#endif


#endif // Header include guard

