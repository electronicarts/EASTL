/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_CONFIG_H
#define EASTL_INTERNAL_CONFIG_H


///////////////////////////////////////////////////////////////////////////////
// ReadMe
//
// This is the EASTL configuration file. All configurable parameters of EASTL
// are controlled through this file. However, all the settings here can be
// manually overridden by the user. There are three ways for a user to override
// the settings in this file:
//
//     - Simply edit this file.
//     - Define EASTL_USER_CONFIG_HEADER.
//     - Predefine individual defines (e.g. EASTL_ASSERT).
//
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// EASTL_USER_CONFIG_HEADER
//
// This allows the user to define a header file to be #included before the
// EASTL config.h contents are compiled. A primary use of this is to override
// the contents of this config.h file. Note that all the settings below in
// this file are user-overridable.
//
// Example usage:
//     #define EASTL_USER_CONFIG_HEADER "MyConfigOverrides.h"
//     #include <EASTL/vector.h>
//
///////////////////////////////////////////////////////////////////////////////

#ifdef EASTL_USER_CONFIG_HEADER
	#include EASTL_USER_CONFIG_HEADER
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_EABASE_DISABLED
//
// The user can disable EABase usage and manually supply the configuration
// via defining EASTL_EABASE_DISABLED and defining the appropriate entities
// globally or via the above EASTL_USER_CONFIG_HEADER.
//
// Example usage:
//     #define EASTL_EABASE_DISABLED
//     #include <EASTL/vector.h>
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_EABASE_DISABLED
	#include <EABase/eabase.h>
#endif
#include <EABase/eahave.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


///////////////////////////////////////////////////////////////////////////////
// EASTL_VERSION
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
//     printf("EASTL version: %s", EASTL_VERSION);
//     printf("EASTL version: %d.%d.%d", EASTL_VERSION_N / 10000 % 100, EASTL_VERSION_N / 100 % 100, EASTL_VERSION_N % 100);
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_VERSION
	#define EASTL_VERSION   "3.17.06"
	#define EASTL_VERSION_N  31706
#endif


///////////////////////////////////////////////////////////////////////////////
// EA_COMPILER_NO_STANDARD_CPP_LIBRARY
//
// Defined as 1 or undefined.
// Implements support for the definition of EA_COMPILER_NO_STANDARD_CPP_LIBRARY for the case
// of using EABase versions prior to the addition of its EA_COMPILER_NO_STANDARD_CPP_LIBRARY support.
//
#if !defined(EA_COMPILER_NO_STANDARD_CPP_LIBRARY)
	#if defined(EA_PLATFORM_ANDROID)
		// Disabled because EA's eaconfig/android_config/android_sdk packages currently
		// don't support linking STL libraries. Perhaps we can figure out what linker arguments
		// are needed for an app so we can manually specify them and then re-enable this code.
		//
		//#include <android/api-level.h>
		//
		//#if (__ANDROID_API__ < 9) // Earlier versions of Android provide no std C++ STL implementation.
			#define EA_COMPILER_NO_STANDARD_CPP_LIBRARY 1
		//#endif
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EA_NOEXCEPT
//
// Defined as a macro. Provided here for backward compatibility with older
// EABase versions prior to 2.00.40 that don't yet define it themselves.
//
#if !defined(EA_NOEXCEPT)
	#define EA_NOEXCEPT
	#define EA_NOEXCEPT_IF(predicate)
	#define EA_NOEXCEPT_EXPR(expression) false
#endif



///////////////////////////////////////////////////////////////////////////////
// EA_CPP14_CONSTEXPR
//
// Defined as constexpr when a C++14 compiler is present. Defines it as nothing
// when using a C++11 compiler.
// C++14 relaxes the specification for constexpr such that it allows more
// kinds of expressions. Since a C++11 compiler doesn't allow this, we need
// to make a unique define for C++14 constexpr. This macro should be used only
// when you are using it with code that specfically requires C++14 constexpr
// functionality beyond the regular C++11 constexpr functionality.
// http://en.wikipedia.org/wiki/C%2B%2B14#Relaxed_constexpr_restrictions
//
#if !defined(EA_CPP14_CONSTEXPR)
	#if defined(EA_COMPILER_CPP14_ENABLED)
		#define EA_CPP14_CONSTEXPR constexpr
	#else
		#define EA_CPP14_CONSTEXPR  // not supported
		#define EA_NO_CPP14_CONSTEXPR
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EASTL namespace
//
// We define this so that users that #include this config file can reference
// these namespaces without seeing any other files that happen to use them.
///////////////////////////////////////////////////////////////////////////////

/// EA Standard Template Library
namespace eastl
{
	// Intentionally empty.
}




///////////////////////////////////////////////////////////////////////////////
// EASTL_DEBUG
//
// Defined as an integer >= 0. Default is 1 for debug builds and 0 for
// release builds. This define is also a master switch for the default value
// of some other settings.
//
// Example usage:
//    #if EASTL_DEBUG
//       ...
//    #endif
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_DEBUG
	#if defined(EA_DEBUG) || defined(_DEBUG)
		#define EASTL_DEBUG 1
	#else
		#define EASTL_DEBUG 0
	#endif
#endif

// Developer debug. Helps EASTL developers assert EASTL is coded correctly.
// Normally disabled for users since it validates internal things and not user things.
#ifndef EASTL_DEV_DEBUG
	#define EASTL_DEV_DEBUG 0
#endif


///////////////////////////////////////////////////////////////////////////////
// EASTL_DEBUGPARAMS_LEVEL
//
// EASTL_DEBUGPARAMS_LEVEL controls what debug information is passed through to
// the allocator by default.
// This value may be defined by the user ... if not it will default to 1 for
// EA_DEBUG builds, otherwise 0.
//
//  0 - no debug information is passed through to allocator calls.
//  1 - 'name' is passed through to allocator calls.
//  2 - 'name', __FILE__, and __LINE__ are passed through to allocator calls.
//
// This parameter mirrors the equivalent parameter in the CoreAllocator package.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_DEBUGPARAMS_LEVEL
	#if EASTL_DEBUG
		#define EASTL_DEBUGPARAMS_LEVEL 2
	#else
		#define EASTL_DEBUGPARAMS_LEVEL 0
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_DLL
//
// Defined as 0 or 1. The default is dependent on the definition of EA_DLL.
// If EA_DLL is defined, then EASTL_DLL is 1, else EASTL_DLL is 0.
// EA_DLL is a define that controls DLL builds within the EAConfig build system.
// EASTL_DLL controls whether EASTL is built and used as a DLL.
// Normally you wouldn't do such a thing, but there are use cases for such
// a thing, particularly in the case of embedding C++ into C# applications.
//
#ifndef EASTL_DLL
	#if defined(EA_DLL)
		#define EASTL_DLL 1
	#else
		#define EASTL_DLL 0
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EASTL_IF_NOT_DLL
//
// Utility to include expressions only for static builds.
//
#ifndef EASTL_IF_NOT_DLL
	#if EASTL_DLL
		#define EASTL_IF_NOT_DLL(x)
	#else
		#define EASTL_IF_NOT_DLL(x) x
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EASTL_API
//
// This is used to label functions as DLL exports under Microsoft platforms.
// If EA_DLL is defined, then the user is building EASTL as a DLL and EASTL's
// non-templated functions will be exported. EASTL template functions are not
// labelled as EASTL_API (and are thus not exported in a DLL build). This is
// because it's not possible (or at least unsafe) to implement inline templated
// functions in a DLL.
//
// Example usage of EASTL_API:
//    EASTL_API int someVariable = 10;      // Export someVariable in a DLL build.
//
//    struct EASTL_API SomeClass{           // Export SomeClass and its member functions in a DLL build.
//        EASTL_LOCAL void PrivateMethod(); // Not exported.
//    };
//
//    EASTL_API void SomeFunction();        // Export SomeFunction in a DLL build.
//
//
#if defined(EA_DLL) && !defined(EASTL_DLL)
	#define EASTL_DLL 1
#endif

#ifndef EASTL_API // If the build file hasn't already defined this to be dllexport...
	#if EASTL_DLL
		#if defined(_MSC_VER)
			#define EASTL_API      __declspec(dllimport)
			#define EASTL_LOCAL
		#elif defined(__CYGWIN__)
			#define EASTL_API      __attribute__((dllimport))
			#define EASTL_LOCAL
		#elif (defined(__GNUC__) && (__GNUC__ >= 4))
			#define EASTL_API      __attribute__ ((visibility("default")))
			#define EASTL_LOCAL    __attribute__ ((visibility("hidden")))
		#else
			#define EASTL_API
			#define EASTL_LOCAL
		#endif
	#else
		#define EASTL_API
		#define EASTL_LOCAL
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EASTL_EASTDC_API
//
// This is used for importing EAStdC functions into EASTL, possibly via a DLL import.
//
#ifndef EASTL_EASTDC_API
	#if EASTL_DLL
		#if defined(_MSC_VER)
			#define EASTL_EASTDC_API      __declspec(dllimport)
			#define EASTL_EASTDC_LOCAL
		#elif defined(__CYGWIN__)
			#define EASTL_EASTDC_API      __attribute__((dllimport))
			#define EASTL_EASTDC_LOCAL
		#elif (defined(__GNUC__) && (__GNUC__ >= 4))
			#define EASTL_EASTDC_API      __attribute__ ((visibility("default")))
			#define EASTL_EASTDC_LOCAL    __attribute__ ((visibility("hidden")))
		#else
			#define EASTL_EASTDC_API
			#define EASTL_EASTDC_LOCAL
		#endif
	#else
		#define EASTL_EASTDC_API
		#define EASTL_EASTDC_LOCAL
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EASTL_EASTDC_VSNPRINTF
//
// Defined as 0 or 1. By default it is 1.
//
// When enabled EASTL uses EAStdC's Vsnprintf function directly instead of
// having the user provide a global Vsnprintf8/16/32 function. The benefit
// of this is that it will allow EASTL to just link to EAStdC's Vsnprintf
// without the user doing anything. The downside is that any users who aren't
// already using EAStdC will either need to now depend on EAStdC or globally
// define this property to be 0 and simply provide functions that have the same
// names. See the usage of EASTL_EASTDC_VSNPRINTF in string.h for more info.
//
#if !defined(EASTL_EASTDC_VSNPRINTF)
	#define EASTL_EASTDC_VSNPRINTF 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_NAME_ENABLED / EASTL_NAME / EASTL_NAME_VAL
//
// Used to wrap debug string names. In a release build, the definition
// goes away. These are present to avoid release build compiler warnings
// and to make code simpler.
//
// Example usage of EASTL_NAME:
//    // pName will defined away in a release build and thus prevent compiler warnings.
//    void allocator::set_name(const char* EASTL_NAME(pName))
//    {
//        #if EASTL_NAME_ENABLED
//            mpName = pName;
//        #endif
//    }
//
// Example usage of EASTL_NAME_VAL:
//    // "xxx" is defined to NULL in a release build.
//    vector<T, Allocator>::vector(const allocator_type& allocator = allocator_type(EASTL_NAME_VAL("xxx")));
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_NAME_ENABLED
	#define EASTL_NAME_ENABLED EASTL_DEBUG
#endif

#ifndef EASTL_NAME
	#if EASTL_NAME_ENABLED
		#define EASTL_NAME(x)      x
		#define EASTL_NAME_VAL(x)  x
	#else
		#define EASTL_NAME(x)
		#define EASTL_NAME_VAL(x) ((const char*)NULL)
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_DEFAULT_NAME_PREFIX
//
// Defined as a string literal. Defaults to "EASTL".
// This define is used as the default name for EASTL where such a thing is
// referenced in EASTL. For example, if the user doesn't specify an allocator
// name for their deque, it is named "EASTL deque". However, you can override
// this to say "SuperBaseball deque" by changing EASTL_DEFAULT_NAME_PREFIX.
//
// Example usage (which is simply taken from how deque.h uses this define):
//     #ifndef EASTL_DEQUE_DEFAULT_NAME
//         #define EASTL_DEQUE_DEFAULT_NAME   EASTL_DEFAULT_NAME_PREFIX " deque"
//     #endif
//
#ifndef EASTL_DEFAULT_NAME_PREFIX
	#define EASTL_DEFAULT_NAME_PREFIX "EASTL"
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_ASSERT_ENABLED
//
// Defined as 0 or non-zero. Default is same as EASTL_DEBUG.
// If EASTL_ASSERT_ENABLED is non-zero, then asserts will be executed via
// the assertion mechanism.
//
// Example usage:
//     #if EASTL_ASSERT_ENABLED
//         EASTL_ASSERT(v.size() > 17);
//     #endif
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_ASSERT_ENABLED
	#define EASTL_ASSERT_ENABLED EASTL_DEBUG
#endif

// Developer assert. Helps EASTL developers assert EASTL is coded correctly.
// Normally disabled for users since it validates internal things and not user things.
#ifndef EASTL_DEV_ASSERT_ENABLED
	#define EASTL_DEV_ASSERT_ENABLED EASTL_DEV_DEBUG
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
//
// Defined as 0 or non-zero. Default is same as EASTL_ASSERT_ENABLED.
// This is like EASTL_ASSERT_ENABLED, except it is for empty container
// references. Sometime people like to be able to take a reference to
// the front of the container, but not use it if the container is empty.
// In practice it's often easier and more efficient to do this than to write
// extra code to check if the container is empty.
//
// NOTE: If this is enabled, EASTL_ASSERT_ENABLED must also be enabled
//
// Example usage:
//     template <typename T, typename Allocator>
//     inline typename vector<T, Allocator>::reference
//     vector<T, Allocator>::front()
//     {
//         #if EASTL_ASSERT_ENABLED
//             EASTL_ASSERT(mpEnd > mpBegin);
//         #endif
//
//         return *mpBegin;
//     }
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
	#define EASTL_EMPTY_REFERENCE_ASSERT_ENABLED EASTL_ASSERT_ENABLED
#endif



///////////////////////////////////////////////////////////////////////////////
// SetAssertionFailureFunction
//
// Allows the user to set a custom assertion failure mechanism.
//
// Example usage:
//     void Assert(const char* pExpression, void* pContext);
//     SetAssertionFailureFunction(Assert, this);
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_ASSERTION_FAILURE_DEFINED
	#define EASTL_ASSERTION_FAILURE_DEFINED

	namespace eastl
	{
		typedef void (*EASTL_AssertionFailureFunction)(const char* pExpression, void* pContext);
		EASTL_API void SetAssertionFailureFunction(EASTL_AssertionFailureFunction pFunction, void* pContext);

		// These are the internal default functions that implement asserts.
		EASTL_API void AssertionFailure(const char* pExpression);
		EASTL_API void AssertionFailureFunctionDefault(const char* pExpression, void* pContext);
	}
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_ASSERT
//
// Assertion macro. Can be overridden by user with a different value.
//
// Example usage:
//    EASTL_ASSERT(intVector.size() < 100);
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_ASSERT
	#if EASTL_ASSERT_ENABLED
		#define EASTL_ASSERT(expression) \
			EA_DISABLE_VC_WARNING(4127) \
			do { \
				EA_ANALYSIS_ASSUME(expression); \
				(void)((expression) || (eastl::AssertionFailure(#expression), 0)); \
			} while (0) \
			EA_RESTORE_VC_WARNING()
	#else
		#define EASTL_ASSERT(expression)
	#endif
#endif

// Developer assert. Helps EASTL developers assert EASTL is coded correctly.
// Normally disabled for users since it validates internal things and not user things.
#ifndef EASTL_DEV_ASSERT
	#if EASTL_DEV_ASSERT_ENABLED
		#define EASTL_DEV_ASSERT(expression) \
			EA_DISABLE_VC_WARNING(4127) \
			do { \
				EA_ANALYSIS_ASSUME(expression); \
				(void)((expression) || (eastl::AssertionFailure(#expression), 0)); \
			} while(0) \
			EA_RESTORE_VC_WARNING()
	#else
		#define EASTL_DEV_ASSERT(expression)
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_ASSERT_MSG
//
// Example usage:
//    EASTL_ASSERT_MSG(false, "detected error condition!");
//
///////////////////////////////////////////////////////////////////////////////
#ifndef EASTL_ASSERT_MSG
	#if EASTL_ASSERT_ENABLED
		#define EASTL_ASSERT_MSG(expression, message) \
			EA_DISABLE_VC_WARNING(4127) \
			do { \
				EA_ANALYSIS_ASSUME(expression); \
				(void)((expression) || (eastl::AssertionFailure(message), 0)); \
			} while (0) \
			EA_RESTORE_VC_WARNING()
	#else
		#define EASTL_ASSERT_MSG(expression, message)
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_FAIL_MSG
//
// Failure macro. Can be overridden by user with a different value.
//
// Example usage:
//    EASTL_FAIL("detected error condition!");
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_FAIL_MSG
	#if EASTL_ASSERT_ENABLED
		#define EASTL_FAIL_MSG(message) (eastl::AssertionFailure(message))
	#else
		#define EASTL_FAIL_MSG(message)
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_CT_ASSERT / EASTL_CT_ASSERT_NAMED
//
// EASTL_CT_ASSERT is a macro for compile time assertion checks, useful for
// validating *constant* expressions. The advantage over using EASTL_ASSERT
// is that errors are caught at compile time instead of runtime.
//
// Example usage:
//     EASTL_CT_ASSERT(sizeof(uint32_t) == 4);
//
///////////////////////////////////////////////////////////////////////////////

#define EASTL_CT_ASSERT(expression) static_assert(expression, #expression)



///////////////////////////////////////////////////////////////////////////////
// EASTL_CT_ASSERT_MSG
//
// EASTL_CT_ASSERT_MSG is a macro for compile time assertion checks, useful for
// validating *constant* expressions. The advantage over using EASTL_ASSERT
// is that errors are caught at compile time instead of runtime.
// The message must be a string literal.
//
// Example usage:
//     EASTL_CT_ASSERT_MSG(sizeof(uint32_t) == 4, "The size of uint32_t must be 4.");
//
///////////////////////////////////////////////////////////////////////////////

#define EASTL_CT_ASSERT_MSG(expression, message) static_assert(expression, message)



///////////////////////////////////////////////////////////////////////////////
// EASTL_DEBUG_BREAK / EASTL_DEBUG_BREAK_OVERRIDE
//
// This function causes an app to immediately stop under the debugger.
// It is implemented as a macro in order to allow stopping at the site
// of the call.
//
// EASTL_DEBUG_BREAK_OVERRIDE allows one to define EASTL_DEBUG_BREAK directly.
// This is useful in cases where you desire to disable EASTL_DEBUG_BREAK
// but do not wish to (or cannot) define a custom void function() to replace
// EASTL_DEBUG_BREAK callsites.
//
// Example usage:
//     EASTL_DEBUG_BREAK();
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_DEBUG_BREAK_OVERRIDE
    #ifndef EASTL_DEBUG_BREAK
        #if defined(_MSC_VER) && (_MSC_VER >= 1300)
            #define EASTL_DEBUG_BREAK() __debugbreak()    // This is a compiler intrinsic which will map to appropriate inlined asm for the platform.
        #elif (defined(EA_PROCESSOR_ARM) && !defined(EA_PROCESSOR_ARM64)) && defined(__APPLE__)
            #define EASTL_DEBUG_BREAK() asm("trap")
        #elif defined(EA_PROCESSOR_ARM64) && defined(__APPLE__)
            #include <signal.h>
            #include <unistd.h>
            #define EASTL_DEBUG_BREAK() kill( getpid(), SIGINT )
		#elif defined(EA_PROCESSOR_ARM64) && defined(__GNUC__)
			#define EASTL_DEBUG_BREAK() asm("brk 10")
		#elif defined(EA_PROCESSOR_ARM) && defined(__GNUC__)
			#define EASTL_DEBUG_BREAK() asm("BKPT 10")     // The 10 is arbitrary. It's just a unique id.
		#elif defined(EA_PROCESSOR_ARM) && defined(__ARMCC_VERSION)
			#define EASTL_DEBUG_BREAK() __breakpoint(10)
		#elif defined(EA_PROCESSOR_POWERPC)               // Generic PowerPC.
			#define EASTL_DEBUG_BREAK() asm(".long 0")    // This triggers an exception by executing opcode 0x00000000.
		#elif (defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64)) && defined(EA_ASM_STYLE_INTEL)
			#define EASTL_DEBUG_BREAK() { __asm int 3 }
		#elif (defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64)) && (defined(EA_ASM_STYLE_ATT) || defined(__GNUC__))
			#define EASTL_DEBUG_BREAK() asm("int3")
		#else
			void EASTL_DEBUG_BREAK(); // User must define this externally.
		#endif
	#else
		void EASTL_DEBUG_BREAK(); // User must define this externally.
	#endif
#else
	#ifndef EASTL_DEBUG_BREAK
		#if EASTL_DEBUG_BREAK_OVERRIDE == 1
			// define an empty callable to satisfy the call site.
			#define EASTL_DEBUG_BREAK ([]{})
		#else
			#define EASTL_DEBUG_BREAK EASTL_DEBUG_BREAK_OVERRIDE
		#endif
	#else
		#error EASTL_DEBUG_BREAK is already defined yet you would like to override it. Please ensure no other headers are already defining EASTL_DEBUG_BREAK before this header (config.h) is included
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_ALLOCATOR_COPY_ENABLED
//
// Defined as 0 or 1. Default is 0 (disabled) until some future date.
// If enabled (1) then container operator= copies the allocator from the
// source container. It ideally should be set to enabled but for backwards
// compatibility with older versions of EASTL it is currently set to 0.
// Regardless of whether this value is 0 or 1, this container copy constructs
// or copy assigns allocators.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_ALLOCATOR_COPY_ENABLED
	#define EASTL_ALLOCATOR_COPY_ENABLED 0
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_FIXED_SIZE_TRACKING_ENABLED
//
// Defined as an integer >= 0. Default is same as EASTL_DEBUG.
// If EASTL_FIXED_SIZE_TRACKING_ENABLED is enabled, then fixed
// containers in debug builds track the max count of objects
// that have been in the container. This allows for the tuning
// of fixed container sizes to their minimum required size.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_FIXED_SIZE_TRACKING_ENABLED
	#define EASTL_FIXED_SIZE_TRACKING_ENABLED EASTL_DEBUG
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_RTTI_ENABLED
//
// Defined as 0 or 1. Default is 1 if RTTI is supported by the compiler.
// This define exists so that we can use some dynamic_cast operations in the
// code without warning. dynamic_cast is only used if the specifically refers
// to it; EASTL won't do dynamic_cast behind your back.
//
// Example usage:
//     #if EASTL_RTTI_ENABLED
//         pChildClass = dynamic_cast<ChildClass*>(pParentClass);
//     #endif
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_RTTI_ENABLED
	// The VC++ default Standard Library (Dinkumware) disables major parts of RTTI
	// (e.g. type_info) if exceptions are disabled, even if RTTI itself is enabled.
	// _HAS_EXCEPTIONS is defined by Dinkumware to 0 or 1 (disabled or enabled).
	#if defined(EA_COMPILER_NO_RTTI) || (defined(_MSC_VER) && defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && !(defined(_HAS_EXCEPTIONS) && _HAS_EXCEPTIONS))
		#define EASTL_RTTI_ENABLED 0
	#else
		#define EASTL_RTTI_ENABLED 1
	#endif
#endif




///////////////////////////////////////////////////////////////////////////////
// EASTL_EXCEPTIONS_ENABLED
//
// Defined as 0 or 1. Default is to follow what the compiler settings are.
// The user can predefine EASTL_EXCEPTIONS_ENABLED to 0 or 1; however, if the
// compiler is set to disable exceptions then EASTL_EXCEPTIONS_ENABLED is
// forced to a value of 0 regardless of the user predefine.
//
// Note that we do not enable EASTL exceptions by default if the compiler
// has exceptions enabled. To enable EASTL_EXCEPTIONS_ENABLED you need to
// manually set it to 1.
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(EASTL_EXCEPTIONS_ENABLED) || ((EASTL_EXCEPTIONS_ENABLED == 1) && defined(EA_COMPILER_NO_EXCEPTIONS))
	#define EASTL_EXCEPTIONS_ENABLED 0
#endif





///////////////////////////////////////////////////////////////////////////////
// EASTL_STRING_OPT_XXXX
//
// Enables some options / optimizations options that cause the string class
// to behave slightly different from the C++ standard basic_string. These are
// options whereby you can improve performance by avoiding operations that
// in practice may never occur for you.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_STRING_OPT_EXPLICIT_CTORS
	// Defined as 0 or 1. Default is 0.
	// Defines if we should implement explicity in constructors where the C++
	// standard string does not. The advantage of enabling explicit constructors
	// is that you can do this: string s = "hello"; in addition to string s("hello");
	// The disadvantage of enabling explicity constructors is that there can be
	// silent conversions done which impede performance if the user isn't paying
	// attention.
	// C++ standard string ctors are not explicit.
	#define EASTL_STRING_OPT_EXPLICIT_CTORS 0
#endif

#ifndef EASTL_STRING_OPT_LENGTH_ERRORS
	// Defined as 0 or 1. Default is equal to EASTL_EXCEPTIONS_ENABLED.
	// Defines if we check for string values going beyond kMaxSize
	// (a very large value) and throw exections if so.
	// C++ standard strings are expected to do such checks.
	#define EASTL_STRING_OPT_LENGTH_ERRORS EASTL_EXCEPTIONS_ENABLED
#endif

#ifndef EASTL_STRING_OPT_RANGE_ERRORS
	// Defined as 0 or 1. Default is equal to EASTL_EXCEPTIONS_ENABLED.
	// Defines if we check for out-of-bounds references to string
	// positions and throw exceptions if so. Well-behaved code shouldn't
	// refence out-of-bounds positions and so shouldn't need these checks.
	// C++ standard strings are expected to do such range checks.
	#define EASTL_STRING_OPT_RANGE_ERRORS EASTL_EXCEPTIONS_ENABLED
#endif

#ifndef EASTL_STRING_OPT_ARGUMENT_ERRORS
	// Defined as 0 or 1. Default is 0.
	// Defines if we check for NULL ptr arguments passed to string
	// functions by the user and throw exceptions if so. Well-behaved code
	// shouldn't pass bad arguments and so shouldn't need these checks.
	// Also, some users believe that strings should check for NULL pointers
	// in all their arguments and do no-ops if so. This is very debatable.
	// C++ standard strings are not required to check for such argument errors.
	#define EASTL_STRING_OPT_ARGUMENT_ERRORS 0
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_BITSET_SIZE_T
//
// Defined as 0 or 1. Default is 1.
// Controls whether bitset uses size_t or eastl_size_t.
//
#ifndef EASTL_BITSET_SIZE_T
	#define EASTL_BITSET_SIZE_T 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_INT128_SUPPORTED
//
// Defined as 0 or 1.
//
#ifndef EASTL_INT128_SUPPORTED
	#if defined(__SIZEOF_INT128__) || (defined(EA_COMPILER_INTMAX_SIZE) && (EA_COMPILER_INTMAX_SIZE >= 16))
		#define EASTL_INT128_SUPPORTED 1
	#else
		#define EASTL_INT128_SUPPORTED 0
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_DEFAULT_ALLOCATOR_ALIGNED_ALLOCATIONS_SUPPORTED
//
// Defined as 0 or 1.
// Tells if you can use the default EASTL allocator to do aligned allocations,
// which for most uses tells if you can store aligned objects in containers
// that use default allocators. It turns out that when built as a DLL for
// some platforms, EASTL doesn't have a way to do aligned allocations, as it
// doesn't have a heap that supports it. There is a way to work around this
// with dynamically defined allocators, but that's currently a to-do.
//
#ifndef EASTL_DEFAULT_ALLOCATOR_ALIGNED_ALLOCATIONS_SUPPORTED
	#if EASTL_DLL
		#define EASTL_DEFAULT_ALLOCATOR_ALIGNED_ALLOCATIONS_SUPPORTED 0
	#else
		#define EASTL_DEFAULT_ALLOCATOR_ALIGNED_ALLOCATIONS_SUPPORTED 1
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EASTL_INT128_DEFINED
//
// Defined as 0 or 1.
// Specifies whether eastl_int128_t/eastl_uint128_t have been typedef'd yet.
//
#ifndef EASTL_INT128_DEFINED
	#if EASTL_INT128_SUPPORTED
		#define EASTL_INT128_DEFINED 1

		#if defined(__SIZEOF_INT128__) || defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)
			typedef __int128_t   eastl_int128_t;
			typedef __uint128_t eastl_uint128_t;
		#else
			typedef  int128_t  eastl_int128_t;  // The EAStdC package defines an EA::StdC::int128_t and uint128_t type,
			typedef uint128_t eastl_uint128_t;  // though they are currently within the EA::StdC namespace.
		#endif
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_BITSET_WORD_TYPE_DEFAULT / EASTL_BITSET_WORD_SIZE_DEFAULT
//
// Defined as an integral power of two type, usually uint32_t or uint64_t.
// Specifies the word type that bitset should use internally to implement
// storage. By default this is the platform register word size, but there
// may be reasons to use a different value.
//
// Defines the integral data type used by bitset by default.
// You can override this default on a bitset-by-bitset case by supplying a
// custom bitset WordType template parameter.
//
// The C++ standard specifies that the std::bitset word type be unsigned long,
// but that isn't necessarily the most efficient data type for the given platform.
// We can follow the standard and be potentially less efficient or we can do what
// is more efficient but less like the C++ std::bitset.
//
#if !defined(EASTL_BITSET_WORD_TYPE_DEFAULT)
	#if defined(EASTL_BITSET_WORD_SIZE)         // EASTL_BITSET_WORD_SIZE is deprecated, but we temporarily support the ability for the user to specify it. Use EASTL_BITSET_WORD_TYPE_DEFAULT instead.
		#if (EASTL_BITSET_WORD_SIZE == 4)
			#define EASTL_BITSET_WORD_TYPE_DEFAULT uint32_t
			#define EASTL_BITSET_WORD_SIZE_DEFAULT 4
		#else
			#define EASTL_BITSET_WORD_TYPE_DEFAULT uint64_t
			#define EASTL_BITSET_WORD_SIZE_DEFAULT 8
		#endif
	#elif (EA_PLATFORM_WORD_SIZE == 16)                     // EA_PLATFORM_WORD_SIZE is defined in EABase.
		#define EASTL_BITSET_WORD_TYPE_DEFAULT uint128_t
		#define EASTL_BITSET_WORD_SIZE_DEFAULT 16
	#elif (EA_PLATFORM_WORD_SIZE == 8)
		#define EASTL_BITSET_WORD_TYPE_DEFAULT uint64_t
		#define EASTL_BITSET_WORD_SIZE_DEFAULT 8
	#elif (EA_PLATFORM_WORD_SIZE == 4)
		#define EASTL_BITSET_WORD_TYPE_DEFAULT uint32_t
		#define EASTL_BITSET_WORD_SIZE_DEFAULT 4
	#else
		#define EASTL_BITSET_WORD_TYPE_DEFAULT uint16_t
		#define EASTL_BITSET_WORD_SIZE_DEFAULT 2
	#endif
#endif




///////////////////////////////////////////////////////////////////////////////
// EASTL_LIST_SIZE_CACHE
//
// Defined as 0 or 1. Default is 1. Changed from 0 in version 1.16.01.
// If defined as 1, the list and slist containers (and possibly any additional
// containers as well) keep a member mSize (or similar) variable which allows
// the size() member function to execute in constant time (a.k.a. O(1)).
// There are debates on both sides as to whether it is better to have this
// cached value or not, as having it entails some cost (memory and code).
// To consider: Make list size caching an optional template parameter.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_LIST_SIZE_CACHE
	#define EASTL_LIST_SIZE_CACHE 1
#endif

#ifndef EASTL_SLIST_SIZE_CACHE
	#define EASTL_SLIST_SIZE_CACHE 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_MAX_STACK_USAGE
//
// Defined as an integer greater than zero. Default is 4000.
// There are some places in EASTL where temporary objects are put on the
// stack. A common example of this is in the implementation of container
// swap functions whereby a temporary copy of the container is made.
// There is a problem, however, if the size of the item created on the stack
// is very large. This can happen with fixed-size containers, for example.
// The EASTL_MAX_STACK_USAGE define specifies the maximum amount of memory
// (in bytes) that the given platform/compiler will safely allow on the stack.
// Platforms such as Windows will generally allow larger values than embedded
// systems or console machines, but it is usually a good idea to stick with
// a max usage value that is portable across all platforms, lest the user be
// surprised when something breaks as it is ported to another platform.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_MAX_STACK_USAGE
	#define EASTL_MAX_STACK_USAGE 4000
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_VA_COPY_ENABLED
//
// Defined as 0 or 1. Default is 1 for compilers that need it, 0 for others.
// Some compilers on some platforms implement va_list whereby its contents
// are destroyed upon usage, even if passed by value to another function.
// With these compilers you can use va_copy to save and restore a va_list.
// Known compiler/platforms that destroy va_list contents upon usage include:
//     CodeWarrior on PowerPC
//     GCC on x86-64
// However, va_copy is part of the C99 standard and not part of earlier C and
// C++ standards. So not all compilers support it. VC++ doesn't support va_copy,
// but it turns out that VC++ doesn't usually need it on the platforms it supports,
// and va_copy can usually be implemented via memcpy(va_list, va_list) with VC++.
//
// Example usage:
//     void Function(va_list arguments)
//     {
//         #if EASTL_VA_COPY_ENABLED
//             va_list argumentsCopy;
//             va_copy(argumentsCopy, arguments);
//         #endif
//         <use arguments or argumentsCopy>
//         #if EASTL_VA_COPY_ENABLED
//             va_end(argumentsCopy);
//         #endif
//     }
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_VA_COPY_ENABLED
	#if   ((defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__clang__)) && (!defined(__i386__) || defined(__x86_64__)) && !defined(__ppc__) && !defined(__PPC__) && !defined(__PPC64__)
		#define EASTL_VA_COPY_ENABLED 1
	#else
		#define EASTL_VA_COPY_ENABLED 0
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EASTL_OPERATOR_EQUALS_OTHER_ENABLED
//
// Defined as 0 or 1. Default is 0 until such day that it's deemed safe.
// When enabled, enables operator= for other char types, e.g. for code
// like this:
//     eastl::string8  s8;
//     eastl::string16 s16;
//     s8 = s16;
// This option is considered experimental, and may exist as such for an
// indefinite amount of time.
//
#if !defined(EASTL_OPERATOR_EQUALS_OTHER_ENABLED)
	#define EASTL_OPERATOR_EQUALS_OTHER_ENABLED 0
#endif
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// EASTL_LIST_PROXY_ENABLED
//
#if !defined(EASTL_LIST_PROXY_ENABLED)
	// GCC with -fstrict-aliasing has bugs (or undocumented functionality in their
	// __may_alias__ implementation. The compiler gets confused about function signatures.
	// VC8 (1400) doesn't need the proxy because it has built-in smart debugging capabilities.
	#if defined(EASTL_DEBUG) && !defined(__GNUC__) && (!defined(_MSC_VER) || (_MSC_VER < 1400))
		#define EASTL_LIST_PROXY_ENABLED 1
		#define EASTL_LIST_PROXY_MAY_ALIAS EASTL_MAY_ALIAS
	#else
		#define EASTL_LIST_PROXY_ENABLED 0
		#define EASTL_LIST_PROXY_MAY_ALIAS
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_STD_ITERATOR_CATEGORY_ENABLED
//
// Defined as 0 or 1. Default is 0.
// If defined as non-zero, EASTL iterator categories (iterator.h's input_iterator_tag,
// forward_iterator_tag, etc.) are defined to be those from std C++ in the std
// namespace. The reason for wanting to enable such a feature is that it allows
// EASTL containers and algorithms to work with std STL containes and algorithms.
// The default value was changed from 1 to 0 in EASL 1.13.03, January 11, 2012.
// The reason for the change was that almost nobody was taking advantage of it and
// it was slowing down compile times for some compilers quite a bit due to them
// having a lot of headers behind <iterator>.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_STD_ITERATOR_CATEGORY_ENABLED
	#define EASTL_STD_ITERATOR_CATEGORY_ENABLED 0
#endif

#if EASTL_STD_ITERATOR_CATEGORY_ENABLED
	#define EASTL_ITC_NS std
#else
	#define EASTL_ITC_NS eastl
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_VALIDATION_ENABLED
//
// Defined as an integer >= 0. Default is to be equal to EASTL_DEBUG.
// If nonzero, then a certain amount of automatic runtime validation is done.
// Runtime validation is not considered the same thing as asserting that user
// input values are valid. Validation refers to internal consistency checking
// of the validity of containers and their iterators. Validation checking is
// something that often involves significantly more than basic assertion
// checking, and it may sometimes be desirable to disable it.
// This macro would generally be used internally by EASTL.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_VALIDATION_ENABLED
	#define EASTL_VALIDATION_ENABLED EASTL_DEBUG
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_VALIDATE_COMPARE
//
// Defined as EASTL_ASSERT or defined away. Default is EASTL_ASSERT if EASTL_VALIDATION_ENABLED is enabled.
// This is used to validate user-supplied comparison functions, particularly for sorting purposes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_VALIDATE_COMPARE_ENABLED
	#define EASTL_VALIDATE_COMPARE_ENABLED EASTL_VALIDATION_ENABLED
#endif

#if EASTL_VALIDATE_COMPARE_ENABLED
	#define EASTL_VALIDATE_COMPARE EASTL_ASSERT
#else
	#define EASTL_VALIDATE_COMPARE(expression)
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_VALIDATE_INTRUSIVE_LIST
//
// Defined as an integral value >= 0. Controls the amount of automatic validation
// done by intrusive_list. A value of 0 means no automatic validation is done.
// As of this writing, EASTL_VALIDATE_INTRUSIVE_LIST defaults to 0, as it makes
// the intrusive_list_node become a non-POD, which may be an issue for some code.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_VALIDATE_INTRUSIVE_LIST
	#define EASTL_VALIDATE_INTRUSIVE_LIST 0
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_FORCE_INLINE
//
// Defined as a "force inline" expression or defined away.
// You generally don't need to use forced inlining with the Microsoft and
// Metrowerks compilers, but you may need it with the GCC compiler (any version).
//
// Example usage:
//     template <typename T, typename Allocator>
//     EASTL_FORCE_INLINE typename vector<T, Allocator>::size_type
//     vector<T, Allocator>::size() const
//        { return mpEnd - mpBegin; }
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_FORCE_INLINE
	#define EASTL_FORCE_INLINE EA_FORCE_INLINE
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_MAY_ALIAS
//
// Defined as a macro that wraps the GCC may_alias attribute. This attribute
// has no significance for VC++ because VC++ doesn't support the concept of
// strict aliasing. Users should avoid writing code that breaks strict
// aliasing rules; EASTL_MAY_ALIAS is for cases with no alternative.
//
// Example usage:
//    uint32_t value EASTL_MAY_ALIAS;
//
// Example usage:
//    typedef uint32_t EASTL_MAY_ALIAS value_type;
//    value_type value;
//
#if defined(__GNUC__) && (((__GNUC__ * 100) + __GNUC_MINOR__) >= 303) && !defined(EA_COMPILER_RVCT)
	#define EASTL_MAY_ALIAS __attribute__((__may_alias__))
#else
	#define EASTL_MAY_ALIAS
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_LIKELY / EASTL_UNLIKELY
//
// Defined as a macro which gives a hint to the compiler for branch
// prediction. GCC gives you the ability to manually give a hint to
// the compiler about the result of a comparison, though it's often
// best to compile shipping code with profiling feedback under both
// GCC (-fprofile-arcs) and VC++ (/LTCG:PGO, etc.). However, there
// are times when you feel very sure that a boolean expression will
// usually evaluate to either true or false and can help the compiler
// by using an explicity directive...
//
// Example usage:
//    if(EASTL_LIKELY(a == 0)) // Tell the compiler that a will usually equal 0.
//       { ... }
//
// Example usage:
//    if(EASTL_UNLIKELY(a == 0)) // Tell the compiler that a will usually not equal 0.
//       { ... }
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_LIKELY
	#if defined(__GNUC__) && (__GNUC__ >= 3)
		#define EASTL_LIKELY(x)   __builtin_expect(!!(x), true)
		#define EASTL_UNLIKELY(x) __builtin_expect(!!(x), false)
	#else
		#define EASTL_LIKELY(x)   (x)
		#define EASTL_UNLIKELY(x) (x)
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EASTL_STD_TYPE_TRAITS_AVAILABLE
//
// Defined as 0 or 1; default is based on auto-detection.
// Specifies whether Standard C++11 <type_traits> support exists.
// Sometimes the auto-detection below fails to work properly and the
// user needs to override it. Does not define whether the compiler provides
// built-in compiler type trait support (e.g. __is_abstract()), as some
// compilers will EASTL_STD_TYPE_TRAITS_AVAILABLE = 0, but have built
// in type trait support.
//
#ifndef EASTL_STD_TYPE_TRAITS_AVAILABLE
	/* Disabled because we don't currently need it.
	#if defined(_MSC_VER) && (_MSC_VER >= 1500)  // VS2008 or later
		#pragma warning(push, 0)
			#include <yvals.h>
		#pragma warning(pop)
		#if ((defined(_HAS_TR1) && _HAS_TR1) || _MSC_VER >= 1700)  // VS2012 (1700) and later has built-in type traits support.
			#define EASTL_STD_TYPE_TRAITS_AVAILABLE 1
			#include <type_traits>
		#else
			#define EASTL_STD_TYPE_TRAITS_AVAILABLE 0
		#endif

	#elif defined(EA_COMPILER_CLANG) || (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4003) && !defined(__GCCXML__)) && !defined(EA_COMPILER_NO_STANDARD_CPP_LIBRARY)
		#include <cstddef> // This will define __GLIBCXX__ if using GNU's libstdc++ and _LIBCPP_VERSION if using clang's libc++.

		#if defined(EA_COMPILER_CLANG) && !defined(EA_PLATFORM_APPLE) // As of v3.0.0, Apple's clang doesn't support type traits.
			// http://clang.llvm.org/docs/LanguageExtensions.html#checking_type_traits
			// Clang has some built-in compiler trait support. This support doesn't currently
			// directly cover all our type_traits, though the C++ Standard Library that's used
			// with clang could fill that in.
			#define EASTL_STD_TYPE_TRAITS_AVAILABLE 1
		#endif

		#if !defined(EASTL_STD_TYPE_TRAITS_AVAILABLE)
			#if defined(_LIBCPP_VERSION) // This is defined by clang's libc++.
				#include <type_traits>

			#elif defined(__GLIBCXX__) && (__GLIBCXX__ >= 20090124) // It's not clear if this is the oldest version that has type traits; probably it isn't.
				#define EASTL_STD_TYPE_TRAITS_AVAILABLE 1

				#if defined(__GXX_EXPERIMENTAL_CXX0X__) // To do: Update this test to include conforming C++11 implementations.
					#include <type_traits>
				#else
					#include <tr1/type_traits>
				#endif
			#else
				#define EASTL_STD_TYPE_TRAITS_AVAILABLE 0
			#endif
		#endif

	#elif defined(__MSL_CPP__) && (__MSL_CPP__ >= 0x8000) // CodeWarrior compiler.
		#define EASTL_STD_TYPE_TRAITS_AVAILABLE 0
		// To do: Implement support for this (via modifying the EASTL type
		//        traits headers, as CodeWarrior provides this.
	#else
		#define EASTL_STD_TYPE_TRAITS_AVAILABLE 0
	#endif
	*/
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE
//
// Defined as 0 or 1; default is based on auto-detection.
// Specifies whether the compiler provides built-in compiler type trait support
// (e.g. __is_abstract()). Does not specify any details about which traits
// are available or what their standards-compliance is. Nevertheless this is a
// useful macro identifier for our type traits implementation.
//
#ifndef EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE
	#if defined(_MSC_VER) && (_MSC_VER >= 1500) // VS2008 or later
		#pragma warning(push, 0)
			#include <yvals.h>
		#pragma warning(pop)
		#if ((defined(_HAS_TR1) && _HAS_TR1) || _MSC_VER >= 1700)  // VS2012 (1700) and later has built-in type traits support.
			#define EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE 1
		#else
			#define EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE 0
		#endif
	#elif defined(EA_COMPILER_CLANG) && defined(__APPLE__) && defined(_CXXCONFIG) // Apple clang but with GCC's libstdc++.
		#define EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE 0
	#elif defined(EA_COMPILER_CLANG)
		#define EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE 1
	#elif defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4003) && !defined(__GCCXML__)
		#define EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE 1
	#elif defined(__MSL_CPP__) && (__MSL_CPP__ >= 0x8000) // CodeWarrior compiler.
		#define EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE 1
	#else
		#define EASTL_COMPILER_INTRINSIC_TYPE_TRAITS_AVAILABLE 0
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_RESET_ENABLED
//
// Defined as 0 or 1; default is 1 for the time being.
// The reset_lose_memory function works the same as reset, as described below.
//
// Specifies whether the container reset functionality is enabled. If enabled
// then <container>::reset forgets its memory, otherwise it acts as the clear
// function. The reset function is potentially dangerous, as it (by design)
// causes containers to not free their memory.
// This option has no applicability to the bitset::reset function, as bitset
// isn't really a container. Also it has no applicability to the smart pointer
// wrappers (e.g. intrusive_ptr).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_RESET_ENABLED
	#define EASTL_RESET_ENABLED 0
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_MINMAX_ENABLED
//
// Defined as 0 or 1; default is 1.
// Specifies whether the min and max algorithms are available.
// It may be useful to disable the min and max algorithms because sometimes
// #defines for min and max exist which would collide with EASTL min and max.
// Note that there are already alternative versions of min and max in EASTL
// with the min_alt and max_alt functions. You can use these without colliding
// with min/max macros that may exist.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef EASTL_MINMAX_ENABLED
	#define EASTL_MINMAX_ENABLED 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_NOMINMAX
//
// Defined as 0 or 1; default is 1.
// MSVC++ has #defines for min/max which collide with the min/max algorithm
// declarations. If EASTL_NOMINMAX is defined as 1, then we undefine min and
// max if they are #defined by an external library. This allows our min and
// max definitions in algorithm.h to work as expected. An alternative to
// the enabling of EASTL_NOMINMAX is to #define NOMINMAX in your project
// settings if you are compiling for Windows.
// Note that this does not control the availability of the EASTL min and max
// algorithms; the EASTL_MINMAX_ENABLED configuration parameter does that.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_NOMINMAX
	#define EASTL_NOMINMAX 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_STD_CPP_ONLY
//
// Defined as 0 or 1; default is 0.
// Disables the use of compiler language extensions. We use compiler language
// extensions only in the case that they provide some benefit that can't be
// had any other practical way. But sometimes the compiler is set to disable
// language extensions or sometimes one compiler's preprocesor is used to generate
// code for another compiler, and so it's necessary to disable language extension usage.
//
// Example usage:
//     #if defined(_MSC_VER) && !EASTL_STD_CPP_ONLY
//         enum : size_type { npos = container_type::npos };    // Microsoft extension which results in significantly smaller debug symbols.
//     #else
//         static const size_type npos = container_type::npos;
//     #endif
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_STD_CPP_ONLY
	#define EASTL_STD_CPP_ONLY 0
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_NO_RVALUE_REFERENCES
//
// Defined as 0 or 1.
// This is the same as EABase EA_COMPILER_NO_RVALUE_REFERENCES except that it
// follows the convention of being always defined, as 0 or 1.
///////////////////////////////////////////////////////////////////////////////
#if !defined(EASTL_NO_RVALUE_REFERENCES)
	#if defined(EA_COMPILER_NO_RVALUE_REFERENCES)
		#define EASTL_NO_RVALUE_REFERENCES 1
	#else
		#define EASTL_NO_RVALUE_REFERENCES 0
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_MOVE_SEMANTICS_ENABLED
//
// Defined as 0 or 1.
// If enabled then C++11-like functionality with rvalue references and move
// operations is enabled.
///////////////////////////////////////////////////////////////////////////////
#if !defined(EASTL_MOVE_SEMANTICS_ENABLED)
	#if EASTL_NO_RVALUE_REFERENCES // If the compiler doesn't support rvalue references or EASTL is configured to disable them...
		#define EASTL_MOVE_SEMANTICS_ENABLED 0
	#else
		#define EASTL_MOVE_SEMANTICS_ENABLED 1
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_VARIADIC_TEMPLATES_ENABLED
//
// Defined as 0 or 1.
// If enabled then C++11-like functionality with variadic templates is enabled.
///////////////////////////////////////////////////////////////////////////////
#if !defined(EASTL_VARIADIC_TEMPLATES_ENABLED)
	#if defined(EA_COMPILER_NO_VARIADIC_TEMPLATES) // If the compiler doesn't support variadic templates
		#define EASTL_VARIADIC_TEMPLATES_ENABLED 0
	#else
		#define EASTL_VARIADIC_TEMPLATES_ENABLED 1
	#endif
#endif

///////////////////////////////////////////////////////////////////////////////
// EASTL_VARIABLE_TEMPLATES_ENABLED
//
// Defined as 0 or 1.
// If enabled then C++11-like functionality with variable templates is enabled.
///////////////////////////////////////////////////////////////////////////////
#if !defined(EASTL_VARIABLE_TEMPLATES_ENABLED)
	#if((EABASE_VERSION_N < 20605) || defined(EA_COMPILER_NO_VARIABLE_TEMPLATES))
		#define EASTL_VARIABLE_TEMPLATES_ENABLED 0
	#else
		#define EASTL_VARIABLE_TEMPLATES_ENABLED 1
	#endif
#endif

///////////////////////////////////////////////////////////////////////////////
// EASTL_INLINE_VARIABLE_ENABLED
//
// Defined as 0 or 1.
// If enabled then C++17-like functionality with inline variable is enabled.
///////////////////////////////////////////////////////////////////////////////
#if !defined(EASTL_INLINE_VARIABLE_ENABLED)
	#if((EABASE_VERSION_N < 20707) || defined(EA_COMPILER_NO_INLINE_VARIABLES))
		#define EASTL_INLINE_VARIABLE_ENABLED 0
	#else
		#define EASTL_INLINE_VARIABLE_ENABLED 1
	#endif
#endif

///////////////////////////////////////////////////////////////////////////////
// EASTL_CPP17_INLINE_VARIABLE
//
// Used to prefix a variable as inline when C++17 inline variables are available
// Usage: EASTL_CPP17_INLINE_VARIABLE constexpr bool type_trait_v = type_trait::value
///////////////////////////////////////////////////////////////////////////////
#if !defined(EASTL_CPP17_INLINE_VARIABLE)
	#if EASTL_INLINE_VARIABLE_ENABLED
		#define EASTL_CPP17_INLINE_VARIABLE inline
	#else
		#define EASTL_CPP17_INLINE_VARIABLE
	#endif
#endif

///////////////////////////////////////////////////////////////////////////////
// EASTL_HAVE_CPP11_TYPE_TRAITS
//
// Defined as 0 or 1.
// This is the same as EABase EA_HAVE_CPP11_TYPE_TRAITS except that it
// follows the convention of being always defined, as 0 or 1. Note that this
// identifies if the Standard Library has C++11 type traits and not if EASTL
// has its equivalents to C++11 type traits.
///////////////////////////////////////////////////////////////////////////////
#if !defined(EASTL_HAVE_CPP11_TYPE_TRAITS)
	// To do: Change this to use the EABase implementation once we have a few months of testing
	// of this and we are sure it works right. Do this at some point after ~January 2014.
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 540) // Dinkumware. VS2012+
		#define EASTL_HAVE_CPP11_TYPE_TRAITS 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4007) // Prior versions of libstdc++ have incomplete support for C++11 type traits.
		#define EASTL_HAVE_CPP11_TYPE_TRAITS 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EASTL_HAVE_CPP11_TYPE_TRAITS 1
	#else
		#define EASTL_HAVE_CPP11_TYPE_TRAITS 0
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS undef
//
// We need revise this macro to be undefined in some cases, in case the user
// isn't using an updated EABase.
///////////////////////////////////////////////////////////////////////////////
#if defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403) // It may in fact be supported by 4.01 or 4.02 but we don't have compilers to test with.
	#if defined(EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS)
		#undef EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_NO_RANGE_BASED_FOR_LOOP
//
// Defined as 0 or 1.
// This is the same as EABase EA_COMPILER_NO_RANGE_BASED_FOR_LOOP except that it
// follows the convention of being always defined, as 0 or 1.
///////////////////////////////////////////////////////////////////////////////
#if !defined(EASTL_NO_RANGE_BASED_FOR_LOOP)
	#if defined(EA_COMPILER_NO_RANGE_BASED_FOR_LOOP)
		#define EASTL_NO_RANGE_BASED_FOR_LOOP 1
	#else
		#define EASTL_NO_RANGE_BASED_FOR_LOOP 0
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_ALIGN_OF
//
// Determines the alignment of a type.
//
// Example usage:
//    size_t alignment = EASTL_ALIGN_OF(int);
//
///////////////////////////////////////////////////////////////////////////////
#ifndef EASTL_ALIGN_OF
	#define EASTL_ALIGN_OF alignof
#endif




///////////////////////////////////////////////////////////////////////////////
// eastl_size_t
//
// Defined as an unsigned integer type, usually either size_t or uint32_t.
// Defaults to size_t to match std STL unless the user specifies to use
// uint32_t explicitly via the EASTL_SIZE_T_32BIT define
//
// Example usage:
//     eastl_size_t n = intVector.size();
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_SIZE_T_32BIT        // Defines whether EASTL_SIZE_T uses uint32_t/int32_t as opposed to size_t/ssize_t.
	#define EASTL_SIZE_T_32BIT 0  // This makes a difference on 64 bit platforms because they use a 64 bit size_t.
#endif                            // By default we do the same thing as std STL and use size_t.

#ifndef EASTL_SIZE_T
	#if (EASTL_SIZE_T_32BIT == 0) || (EA_PLATFORM_WORD_SIZE == 4)
		#include <stddef.h>
		#define EASTL_SIZE_T  size_t
		#define EASTL_SSIZE_T intptr_t
	#else
		#define EASTL_SIZE_T  uint32_t
		#define EASTL_SSIZE_T int32_t
	#endif
#endif

typedef EASTL_SIZE_T  eastl_size_t;  // Same concept as std::size_t.
typedef EASTL_SSIZE_T eastl_ssize_t; // Signed version of eastl_size_t. Concept is similar to Posix's ssize_t.




///////////////////////////////////////////////////////////////////////////////
// AddRef / Release
//
// AddRef and Release are used for "intrusive" reference counting. By the term
// "intrusive", we mean that the reference count is maintained by the object
// and not by the user of the object. Given that an object implements referencing
// counting, the user of the object needs to be able to increment and decrement
// that reference count. We do that via the venerable AddRef and Release functions
// which the object must supply. These defines here allow us to specify the name
// of the functions. They could just as well be defined to addref and delref or
// IncRef and DecRef.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTLAddRef
	#define EASTLAddRef AddRef
#endif

#ifndef EASTLRelease
	#define EASTLRelease Release
#endif




///////////////////////////////////////////////////////////////////////////////
// EASTL_ALLOCATOR_EXPLICIT_ENABLED
//
// Defined as 0 or 1. Default is 0 for now but ideally would be changed to
// 1 some day. It's 0 because setting it to 1 breaks some existing code.
// This option enables the allocator ctor to be explicit, which avoids
// some undesirable silent conversions, especially with the string class.
//
// Example usage:
//     class allocator
//     {
//     public:
//         EASTL_ALLOCATOR_EXPLICIT allocator(const char* pName);
//     };
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_ALLOCATOR_EXPLICIT_ENABLED
	#define EASTL_ALLOCATOR_EXPLICIT_ENABLED 0
#endif

#if EASTL_ALLOCATOR_EXPLICIT_ENABLED
	#define EASTL_ALLOCATOR_EXPLICIT explicit
#else
	#define EASTL_ALLOCATOR_EXPLICIT
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_ALLOCATOR_MIN_ALIGNMENT
//
// Defined as an integral power-of-2 that's >= 1.
// Identifies the minimum alignment that EASTL should assume its allocators
// use. There is code within EASTL that decides whether to do a Malloc or
// MallocAligned call and it's typically better if it can use the Malloc call.
// But this requires knowing what the minimum possible alignment is.
#if !defined(EASTL_ALLOCATOR_MIN_ALIGNMENT)
	#define EASTL_ALLOCATOR_MIN_ALIGNMENT EA_PLATFORM_MIN_MALLOC_ALIGNMENT
#endif


///////////////////////////////////////////////////////////////////////////////
// EASTL_SYSTEM_ALLOCATOR_MIN_ALIGNMENT
//
// Identifies the minimum alignment that EASTL should assume system allocations
// from malloc and new will have.
#if !defined(EASTL_SYSTEM_ALLOCATOR_MIN_ALIGNMENT)
	#if defined(EA_PLATFORM_MICROSOFT) || defined(EA_PLATFORM_APPLE)
		#define EASTL_SYSTEM_ALLOCATOR_MIN_ALIGNMENT 16
	#else
		#define EASTL_SYSTEM_ALLOCATOR_MIN_ALIGNMENT (EA_PLATFORM_PTR_SIZE * 2)
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EASTL allocator
//
// The EASTL allocator system allows you to redefine how memory is allocated
// via some defines that are set up here. In the container code, memory is
// allocated via macros which expand to whatever the user has them set to
// expand to. Given that there are multiple allocator systems available,
// this system allows you to configure it to use whatever system you want,
// provided your system meets the requirements of this library.
// The requirements are:
//
//     - Must be constructable via a const char* (name) parameter.
//       Some uses of allocators won't require this, however.
//     - Allocate a block of memory of size n and debug name string.
//     - Allocate a block of memory of size n, debug name string,
//       alignment a, and offset o.
//     - Free memory allocated via either of the allocation functions above.
//     - Provide a default allocator instance which can be used if the user
//       doesn't provide a specific one.
//
///////////////////////////////////////////////////////////////////////////////

// namespace eastl
// {
//     class allocator
//     {
//         allocator(const char* pName = NULL);
//
//         void* allocate(size_t n, int flags = 0);
//         void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0);
//         void  deallocate(void* p, size_t n);
//
//         const char* get_name() const;
//         void        set_name(const char* pName);
//     };
//
//     allocator* GetDefaultAllocator(); // This is used for anonymous allocations.
// }

#ifndef EASTLAlloc // To consider: Instead of calling through pAllocator, just go directly to operator new, since that's what allocator does.
	#define EASTLAlloc(allocator, n) (allocator).allocate(n);
#endif

#ifndef EASTLAllocFlags // To consider: Instead of calling through pAllocator, just go directly to operator new, since that's what allocator does.
	#define EASTLAllocFlags(allocator, n, flags) (allocator).allocate(n, flags);
#endif

#ifndef EASTLAllocAligned
	#define EASTLAllocAligned(allocator, n, alignment, offset) (allocator).allocate((n), (alignment), (offset))
#endif

#ifndef EASTLAllocAlignedFlags
	#define EASTLAllocAlignedFlags(allocator, n, alignment, offset, flags) (allocator).allocate((n), (alignment), (offset), (flags))
#endif

#ifndef EASTLFree
	#define EASTLFree(allocator, p, size) (allocator).deallocate((void*)(p), (size)) // Important to cast to void* as p may be non-const.
#endif

#ifndef EASTLAllocatorType
	#define EASTLAllocatorType eastl::allocator
#endif

#ifndef EASTLDummyAllocatorType
	#define EASTLDummyAllocatorType eastl::dummy_allocator
#endif

#ifndef EASTLAllocatorDefault
	// EASTLAllocatorDefault returns the default allocator instance. This is not a global
	// allocator which implements all container allocations but is the allocator that is
	// used when EASTL needs to allocate memory internally. There are very few cases where
	// EASTL allocates memory internally, and in each of these it is for a sensible reason
	// that is documented to behave as such.
	#define EASTLAllocatorDefault eastl::GetDefaultAllocator
#endif


/// EASTL_ALLOCATOR_DEFAULT_NAME
///
/// Defines a default allocator name in the absence of a user-provided name.
///
#ifndef EASTL_ALLOCATOR_DEFAULT_NAME
	#define EASTL_ALLOCATOR_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX // Unless the user overrides something, this is "EASTL".
#endif

/// EASTL_USE_FORWARD_WORKAROUND
///
/// This is to workaround a compiler bug that we found in VS2013. Update 1 did not fix it.
/// This should be fixed in a future release of VS2013 http://accentuable4.rssing.com/browser.php?indx=3511740&item=15696
///
#ifndef EASTL_USE_FORWARD_WORKAROUND
	#if defined(_MSC_FULL_VER) && _MSC_FULL_VER == 180021005 || (defined(__EDG_VERSION__) && (__EDG_VERSION__ < 405))// VS2013 initial release
		#define EASTL_USE_FORWARD_WORKAROUND 1
	#else
		#define EASTL_USE_FORWARD_WORKAROUND 0
	#endif
#endif


/// EASTL_TUPLE_ENABLED
/// EASTL tuple implementation depends on variadic template support
#if EASTL_VARIADIC_TEMPLATES_ENABLED && !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
	#define EASTL_TUPLE_ENABLED 1
#else
	#define EASTL_TUPLE_ENABLED 0
#endif


/// EASTL_FUNCTION_ENABLED
///
#ifndef EASTL_FUNCTION_ENABLED
	#define EASTL_FUNCTION_ENABLED 1
#endif


/// EASTL_USER_LITERALS_ENABLED
#ifndef EASTL_USER_LITERALS_ENABLED
	#if defined(EA_COMPILER_CPP14_ENABLED)
		#define EASTL_USER_LITERALS_ENABLED 1

		// Disabling the Clang/GCC/MSVC warning about using user defined literals without a leading '_' as they are
		// reserved for standard libary usage.
		EA_DISABLE_CLANG_WARNING(-Wuser-defined-literals)
		EA_DISABLE_CLANG_WARNING(-Wreserved-user-defined-literal)
		EA_DISABLE_GCC_WARNING(-Wliteral-suffix)
		#ifdef _MSC_VER
			#pragma warning(disable: 4455) // disable warning C4455: literal suffix identifiers that do not start with an underscore are reserved
		#endif

	#else
		#define EASTL_USER_LITERALS_ENABLED 0
	#endif
#endif


/// EASTL_INLINE_NAMESPACES_ENABLED
#ifndef EASTL_INLINE_NAMESPACES_ENABLED
	#if defined(EA_COMPILER_CPP14_ENABLED)
		#define EASTL_INLINE_NAMESPACES_ENABLED 1
	#else
		#define EASTL_INLINE_NAMESPACES_ENABLED 0
	#endif
#endif


/// EASTL_CORE_ALLOCATOR_ENABLED
#ifndef EASTL_CORE_ALLOCATOR_ENABLED
	#define EASTL_CORE_ALLOCATOR_ENABLED 0
#endif

/// EASTL_OPENSOURCE
/// This is enabled when EASTL is building built in an "open source" mode.  Which is a mode that eliminates code
/// dependencies on other technologies that have not been released publically.
/// EASTL_OPENSOURCE = 0, is the default.
/// EASTL_OPENSOURCE = 1, utilizes technologies that not publically available.
///
#ifndef EASTL_OPENSOURCE
	#define EASTL_OPENSOURCE 0
#endif


/// EASTL_OPTIONAL_ENABLED
#if defined(EA_COMPILER_MSVC_2012)
	#define EASTL_OPTIONAL_ENABLED 0
#elif defined(EA_COMPILER_MSVC_2013)
	#define EASTL_OPTIONAL_ENABLED 0
#elif defined(EA_COMPILER_MSVC_2015)
	#define EASTL_OPTIONAL_ENABLED 1
#elif EASTL_VARIADIC_TEMPLATES_ENABLED && !defined(EA_COMPILER_NO_TEMPLATE_ALIASES) && !defined(EA_COMPILER_NO_DEFAULTED_FUNCTIONS) && defined(EA_COMPILER_CPP11_ENABLED)
	#define EASTL_OPTIONAL_ENABLED 1
#else
	#define EASTL_OPTIONAL_ENABLED 0
#endif


/// EASTL_HAS_UNIQUE_OBJECT_REPRESENTATIONS_AVAILABLE
#if defined(_MSC_VER) && (_MSC_VER >= 1913)  // VS2017+
	#define EASTL_HAS_UNIQUE_OBJECT_REPRESENTATIONS_AVAILABLE 1
#elif defined(EA_COMPILER_CLANG)
	#if !__is_identifier(__has_unique_object_representations)
		#define EASTL_HAS_UNIQUE_OBJECT_REPRESENTATIONS_AVAILABLE 1
	#else
		#define EASTL_HAS_UNIQUE_OBJECT_REPRESENTATIONS_AVAILABLE 0
	#endif
#else
	#define EASTL_HAS_UNIQUE_OBJECT_REPRESENTATIONS_AVAILABLE 0
#endif


/// EASTL_ENABLE_PAIR_FIRST_ELEMENT_CONSTRUCTOR
/// This feature define allows users to toggle the problematic eastl::pair implicit
/// single element constructor.
#ifndef EASTL_ENABLE_PAIR_FIRST_ELEMENT_CONSTRUCTOR
	#define EASTL_ENABLE_PAIR_FIRST_ELEMENT_CONSTRUCTOR 0
#endif

/// EASTL_SYSTEM_BIG_ENDIAN_STATEMENT
/// EASTL_SYSTEM_LITTLE_ENDIAN_STATEMENT
/// These macros allow you to write endian specific macros as statements.
/// This allows endian specific code to be macro expanded from within other macros
///
#if defined(EA_SYSTEM_BIG_ENDIAN)
	#define EASTL_SYSTEM_BIG_ENDIAN_STATEMENT(...) __VA_ARGS__
#else
	#define EASTL_SYSTEM_BIG_ENDIAN_STATEMENT(...)
#endif

#if defined(EA_SYSTEM_LITTLE_ENDIAN)
	#define EASTL_SYSTEM_LITTLE_ENDIAN_STATEMENT(...) __VA_ARGS__
#else
	#define EASTL_SYSTEM_LITTLE_ENDIAN_STATEMENT(...)
#endif


#endif // Header include guard
