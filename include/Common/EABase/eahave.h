/*-----------------------------------------------------------------------------
 * eahave.h
 *
 * Copyright (c) Electronic Arts Inc. All rights reserved.
 *---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
	This file's functionality is preliminary and won't be considered stable until 
	a future EABase version.
 *---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
	This header identifies if the given facilities are available in the 
	standard build environment the current compiler/linker/standard library/
	operating system combination. This file may in some cases #include standard
	headers in order to make availability determinations, such as to check 
	compiler or SDK version numbers. However, it cannot be perfect.
	This header does not identify compiler features, as those are defined in 
	eacompiler.h and eacompilertraits.h. Rather this header is about library support.
	This header does not identify platform or library conventions either, such
	as whether the file paths use \ or / for directory separators.

	We provide three types of HAVE features here:

		- EA_HAVE_XXX_FEATURE - Have compiler feature. 
		  Identifies if the compiler has or lacks some feature in the 
		  current build. Sometimes you need to check to see if the 
		  compiler is running in some mode in able to write portable code
		  against it. For example, some compilers (e.g. VC++) have a 
		  mode in which all language extensions are disabled. If you want
		  to write code that works with that but still uses the extensions
		  when available then you can check #if defined(EA_HAVE_EXTENSIONS_FEATURE).
		  Features can be forcibly cancelled via EA_NO_HAVE_XXX_FEATURE.
		  EA_NO_HAVE is useful for a build system or user to override the 
		  defaults because it happens to know better.

		- EA_HAVE_XXX_H - Have header file information. 
		  Identifies if a given header file is available to the current 
		  compile configuration. For example, some compilers provide a 
		  malloc.h header, while others don't. For the former we define 
		  EA_HAVE_MALLOC_H, while for the latter it remains undefined.
		  If a header is missing then it may still be that the functions
		  the header usually declares are declared in some other header.
		  EA_HAVE_XXX does not include the possibility that our own code 
		  provides versions of these headers, and in fact a purpose of 
		  EA_HAVE_XXX is to decide if we should be using our own because
		  the system doesn't provide one.
		  Header availability can be forcibly cancelled via EA_NO_HAVE_XXX_H.
		  EA_NO_HAVE is useful for a build system or user to override the 
		  defaults because it happens to know better.

		- EA_HAVE_XXX_DECL - Have function declaration information. 
		  Identifies if a given function declaration is provided by 
		  the current compile configuration. For example, some compiler
		  standard libraries declare a wcslen function, while others
		  don't. For the former we define EA_HAVE_WCSLEN_DECL, while for
		  the latter it remains undefined. If a declaration of a function
		  is missing then we assume the implementation is missing as well.
		  EA_HAVE_XXX_DECL does not include the possibility that our 
		  own code provides versions of these declarations, and in fact a 
		  purpose of EA_HAVE_XXX_DECL is to decide if we should be using 
		  our own because the system doesn't provide one.
		  Declaration availability can be forcibly cancelled via EA_NO_HAVE_XXX_DECL.
		  EA_NO_HAVE is useful for a build system or user to override the 
		  defaults because it happens to know better.

		- EA_HAVE_XXX_IMPL - Have function implementation information. 
		  Identifies if a given function implementation is provided by
		  the current compile and link configuration. For example, it's
		  commonly the case that console platforms declare a getenv function 
		  but don't provide a linkable implementation.
		  In this case the user needs to provide such a function manually
		  as part of the link. If the implementation is available then
		  we define EA_HAVE_GETENV_IMPL, otherwise it remains undefined.
		  Beware that sometimes a function may not seem to be present in 
		  the Standard Library but in reality you need to link some auxiliary
		  provided library for it. An example of this is the Unix real-time
		  functions such as clock_gettime.
		  EA_HAVE_XXX_IMPL does not include the possibility that our 
		  own code provides versions of these implementations, and in fact a 
		  purpose of EA_HAVE_XXX_IMPL is to decide if we should be using 
		  our own because the system doesn't provide one.
		  Implementation availability can be forcibly cancelled via EA_NO_HAVE_XXX_IMPL.
		  EA_NO_HAVE is useful for a build system or user to override the 
		  defaults because it happens to know better.

	It's not practical to define EA_HAVE macros for every possible header,
	declaration, and implementation, and so the user must simply know that
	some headers, declarations, and implementations tend to require EA_HAVE
	checking. Nearly every C Standard Library we've seen has a <string.h> 
	header, a strlen declaration, and a linkable strlen implementation, 
	so there's no need to provide EA_HAVE support for this. On the other hand
	it's commonly the case that the C Standard Library doesn't have a malloc.h
	header or an inet_ntop declaration.

---------------------------------------------------------------------------*/


#ifndef INCLUDED_eahave_H
#define INCLUDED_eahave_H


#include <EABase/eabase.h>


#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif

/* EA_HAVE_XXX_FEATURE */

#if !defined(EA_HAVE_EXTENSIONS_FEATURE) && !defined(EA_NO_HAVE_EXTENSIONS_FEATURE)
	#define EA_HAVE_EXTENSIONS_FEATURE 1
#endif


/* EA_HAVE_XXX_LIBRARY */

// Dinkumware
#if !defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && !defined(EA_NO_HAVE_DINKUMWARE_CPP_LIBRARY)
	#if defined(__cplusplus)
		EA_DISABLE_ALL_VC_WARNINGS()
		#include <cstddef> // Need to trigger the compilation of yvals.h without directly using <yvals.h> because it might not exist.
		EA_RESTORE_ALL_VC_WARNINGS()
	#endif

	#if defined(__cplusplus) && defined(_CPPLIB_VER) /* If using the Dinkumware Standard library... */
		#define EA_HAVE_DINKUMWARE_CPP_LIBRARY 1
	#else
		#define EA_NO_HAVE_DINKUMWARE_CPP_LIBRARY 1
	#endif
#endif

// GCC libstdc++
#if !defined(EA_HAVE_LIBSTDCPP_LIBRARY) && !defined(EA_NO_HAVE_LIBSTDCPP_LIBRARY)
	#if defined(__GLIBCXX__) /* If using libstdc++ ... */
		#define EA_HAVE_LIBSTDCPP_LIBRARY 1
	#else
		#define EA_NO_HAVE_LIBSTDCPP_LIBRARY 1
	#endif
#endif

// Clang libc++
#if !defined(EA_HAVE_LIBCPP_LIBRARY) && !defined(EA_NO_HAVE_LIBCPP_LIBRARY)
	#if EA_HAS_INCLUDE_AVAILABLE
		#if EA_HAS_INCLUDE(<__config>)
			#define EA_HAVE_LIBCPP_LIBRARY 1 // We could also #include <ciso646> and check if defined(_LIBCPP_VERSION).
		#endif
	#endif

	#if !defined(EA_HAVE_LIBCPP_LIBRARY) 
		#define EA_NO_HAVE_LIBCPP_LIBRARY 1
	#endif
#endif


/* EA_HAVE_XXX_H */

// #include <sys/types.h>
#if !defined(EA_HAVE_SYS_TYPES_H) && !defined(EA_NO_HAVE_SYS_TYPES_H)
		#define EA_HAVE_SYS_TYPES_H 1
#endif

// #include <io.h> (and not sys/io.h or asm/io.h)
#if !defined(EA_HAVE_IO_H) && !defined(EA_NO_HAVE_IO_H)
	// Unix doesn't have Microsoft's <io.h> but has the same functionality in <fcntl.h> and <sys/stat.h>.
	#if defined(EA_PLATFORM_MICROSOFT)
		#define EA_HAVE_IO_H 1
	#else
		#define EA_NO_HAVE_IO_H 1
	#endif
#endif

// #include <inttypes.h>
#if !defined(EA_HAVE_INTTYPES_H) && !defined(EA_NO_HAVE_INTTYPES_H)
	#if !defined(EA_PLATFORM_MICROSOFT) 
		#define EA_HAVE_INTTYPES_H 1
	#else
		#define EA_NO_HAVE_INTTYPES_H 1
	#endif
#endif

// #include <unistd.h>
#if !defined(EA_HAVE_UNISTD_H) && !defined(EA_NO_HAVE_UNISTD_H)
	#if defined(EA_PLATFORM_UNIX)
		#define EA_HAVE_UNISTD_H 1
	#else
		#define EA_NO_HAVE_UNISTD_H 1
	#endif
#endif

// #include <sys/time.h>
#if !defined(EA_HAVE_SYS_TIME_H) && !defined(EA_NO_HAVE_SYS_TIME_H)
	#if !defined(EA_PLATFORM_MICROSOFT) && !defined(_CPPLIB_VER) /* _CPPLIB_VER indicates Dinkumware. */
		#define EA_HAVE_SYS_TIME_H 1 /* defines struct timeval */
	#else
		#define EA_NO_HAVE_SYS_TIME_H 1
	#endif
#endif

// #include <ptrace.h>
#if !defined(EA_HAVE_SYS_PTRACE_H) && !defined(EA_NO_HAVE_SYS_PTRACE_H)
	#if defined(EA_PLATFORM_UNIX) && !defined(__CYGWIN__) && (defined(EA_PLATFORM_DESKTOP) || defined(EA_PLATFORM_SERVER))
		#define EA_HAVE_SYS_PTRACE_H 1 /* declares the ptrace function */
	#else
		#define EA_NO_HAVE_SYS_PTRACE_H 1
	#endif
#endif

// #include <sys/stat.h>
#if !defined(EA_HAVE_SYS_STAT_H) && !defined(EA_NO_HAVE_SYS_STAT_H)
	#if (defined(EA_PLATFORM_UNIX) && !(defined(EA_PLATFORM_SONY) && defined(EA_PLATFORM_CONSOLE))) || defined(__APPLE__) || defined(EA_PLATFORM_ANDROID)
		#define EA_HAVE_SYS_STAT_H 1 /* declares the stat struct and function */
	#else
		#define EA_NO_HAVE_SYS_STAT_H 1
	#endif
#endif

// #include <locale.h>
#if !defined(EA_HAVE_LOCALE_H) && !defined(EA_NO_HAVE_LOCALE_H)
		#define EA_HAVE_LOCALE_H 1
#endif

// #include <signal.h>
#if !defined(EA_HAVE_SIGNAL_H) && !defined(EA_NO_HAVE_SIGNAL_H)
	#if !defined(EA_PLATFORM_BSD) && !defined(EA_PLATFORM_SONY) && !defined(CS_UNDEFINED_STRING)
		#define EA_HAVE_SIGNAL_H 1
	#else
		#define EA_NO_HAVE_SIGNAL_H 1
	#endif
#endif

// #include <sys/signal.h>
#if !defined(EA_HAVE_SYS_SIGNAL_H) && !defined(EA_NO_HAVE_SYS_SIGNAL_H)
	#if defined(EA_PLATFORM_BSD) || defined(EA_PLATFORM_SONY)
		#define EA_HAVE_SYS_SIGNAL_H 1
	#else
		#define EA_NO_HAVE_SYS_SIGNAL_H 1
	#endif
#endif

// #include <pthread.h>
#if !defined(EA_HAVE_PTHREAD_H) && !defined(EA_NO_HAVE_PTHREAD_H)
	#if defined(EA_PLATFORM_UNIX) || defined(EA_PLATFORM_APPLE) || defined(EA_PLATFORM_POSIX)
		#define EA_HAVE_PTHREAD_H 1 /* It can be had under Microsoft/Windows with the http://sourceware.org/pthreads-win32/ library */
	#else
		#define EA_NO_HAVE_PTHREAD_H 1
	#endif
#endif

// #include <wchar.h>
#if !defined(EA_HAVE_WCHAR_H) && !defined(EA_NO_HAVE_WCHAR_H)
	#if defined(EA_PLATFORM_DESKTOP) && defined(EA_PLATFORM_UNIX) && defined(EA_PLATFORM_SONY) && defined(EA_PLATFORM_APPLE)
		#define EA_HAVE_WCHAR_H 1
	#else
		#define EA_NO_HAVE_WCHAR_H 1
	#endif
#endif

// #include <malloc.h>
#if !defined(EA_HAVE_MALLOC_H) && !defined(EA_NO_HAVE_MALLOC_H)
	#if defined(_MSC_VER) || defined(__MINGW32__)
		#define EA_HAVE_MALLOC_H 1
	#else
		#define EA_NO_HAVE_MALLOC_H 1
	#endif
#endif

// #include <alloca.h>
#if !defined(EA_HAVE_ALLOCA_H) && !defined(EA_NO_HAVE_ALLOCA_H)
	#if !defined(EA_HAVE_MALLOC_H) && !defined(EA_PLATFORM_SONY)
		#define EA_HAVE_ALLOCA_H 1
	#else
		#define EA_NO_HAVE_ALLOCA_H 1
	#endif
#endif

// #include <execinfo.h>
#if !defined(EA_HAVE_EXECINFO_H) && !defined(EA_NO_HAVE_EXECINFO_H)
	#if (defined(EA_PLATFORM_LINUX) || defined(EA_PLATFORM_OSX)) && !defined(EA_PLATFORM_ANDROID)
		#define EA_HAVE_EXECINFO_H 1
	#else
		#define EA_NO_HAVE_EXECINFO_H 1
	#endif
#endif

// #include <semaphore.h> (Unix semaphore support)
#if !defined(EA_HAVE_SEMAPHORE_H) && !defined(EA_NO_HAVE_SEMAPHORE_H)
	#if defined(EA_PLATFORM_UNIX)
		#define EA_HAVE_SEMAPHORE_H 1
	#else
		#define EA_NO_HAVE_SEMAPHORE_H 1
	#endif
#endif

// #include <dirent.h> (Unix semaphore support)
#if !defined(EA_HAVE_DIRENT_H) && !defined(EA_NO_HAVE_DIRENT_H)
	#if defined(EA_PLATFORM_UNIX) && !defined(EA_PLATFORM_CONSOLE)
		#define EA_HAVE_DIRENT_H 1
	#else
		#define EA_NO_HAVE_DIRENT_H 1
	#endif
#endif

// #include <array>, <forward_list>, <ununordered_set>, <unordered_map>
#if !defined(EA_HAVE_CPP11_CONTAINERS) && !defined(EA_NO_HAVE_CPP11_CONTAINERS)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 520) // Dinkumware. VS2010+
		#define EA_HAVE_CPP11_CONTAINERS 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4004) // Actually GCC 4.3 supports array and unordered_
		#define EA_HAVE_CPP11_CONTAINERS 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_CONTAINERS 1
	#else
		#define EA_NO_HAVE_CPP11_CONTAINERS 1
	#endif
#endif

// #include <atomic>
#if !defined(EA_HAVE_CPP11_ATOMIC) && !defined(EA_NO_HAVE_CPP11_ATOMIC)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 540) // Dinkumware. VS2012+
		#define EA_HAVE_CPP11_ATOMIC 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4007)
		#define EA_HAVE_CPP11_ATOMIC 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_ATOMIC 1
	#else
		#define EA_NO_HAVE_CPP11_ATOMIC 1
	#endif
#endif

// #include <condition_variable>
#if !defined(EA_HAVE_CPP11_CONDITION_VARIABLE) && !defined(EA_NO_HAVE_CPP11_CONDITION_VARIABLE)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 540) // Dinkumware. VS2012+
		#define EA_HAVE_CPP11_CONDITION_VARIABLE 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4007)
		#define EA_HAVE_CPP11_CONDITION_VARIABLE 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_CONDITION_VARIABLE 1
	#else
		#define EA_NO_HAVE_CPP11_CONDITION_VARIABLE 1
	#endif
#endif

// #include <mutex>
#if !defined(EA_HAVE_CPP11_MUTEX) && !defined(EA_NO_HAVE_CPP11_MUTEX)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 540) // Dinkumware. VS2012+
		#define EA_HAVE_CPP11_MUTEX 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4007)
		#define EA_HAVE_CPP11_MUTEX 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_MUTEX 1
	#else
		#define EA_NO_HAVE_CPP11_MUTEX 1
	#endif
#endif

// #include <thread>
#if !defined(EA_HAVE_CPP11_THREAD) && !defined(EA_NO_HAVE_CPP11_THREAD)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 540) // Dinkumware. VS2012+
		#define EA_HAVE_CPP11_THREAD 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4007)
		#define EA_HAVE_CPP11_THREAD 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_THREAD 1
	#else
		#define EA_NO_HAVE_CPP11_THREAD 1
	#endif
#endif

// #include <future>
#if !defined(EA_HAVE_CPP11_FUTURE) && !defined(EA_NO_HAVE_CPP11_FUTURE)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 540) // Dinkumware. VS2012+
		#define EA_HAVE_CPP11_FUTURE 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4005)
		#define EA_HAVE_CPP11_FUTURE 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_FUTURE 1
	#else
		#define EA_NO_HAVE_CPP11_FUTURE 1
	#endif
#endif


// #include <type_traits>
#if !defined(EA_HAVE_CPP11_TYPE_TRAITS) && !defined(EA_NO_HAVE_CPP11_TYPE_TRAITS)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 540) // Dinkumware. VS2012+
		#define EA_HAVE_CPP11_TYPE_TRAITS 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4007) // Prior versions of libstdc++ have incomplete support for C++11 type traits.
		#define EA_HAVE_CPP11_TYPE_TRAITS 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_TYPE_TRAITS 1
	#else
		#define EA_NO_HAVE_CPP11_TYPE_TRAITS 1
	#endif
#endif

// #include <tuple>
#if !defined(EA_HAVE_CPP11_TUPLES) && !defined(EA_NO_HAVE_CPP11_TUPLES)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 520) // Dinkumware. VS2010+
		#define EA_HAVE_CPP11_TUPLES 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4003)
		#define EA_HAVE_CPP11_TUPLES 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_TUPLES 1
	#else
		#define EA_NO_HAVE_CPP11_TUPLES 1
	#endif
#endif

// #include <regex>
#if !defined(EA_HAVE_CPP11_REGEX) && !defined(EA_NO_HAVE_CPP11_REGEX)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 540) && (defined(_HAS_EXCEPTIONS) && _HAS_EXCEPTIONS) // Dinkumware. VS2012+
		#define EA_HAVE_CPP11_REGEX 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4003)
		#define EA_HAVE_CPP11_REGEX 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_REGEX 1
	#else
		#define EA_NO_HAVE_CPP11_REGEX 1
	#endif
#endif

// #include <random>
#if !defined(EA_HAVE_CPP11_RANDOM) && !defined(EA_NO_HAVE_CPP11_RANDOM)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 520) // Dinkumware. VS2010+
		#define EA_HAVE_CPP11_RANDOM 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4005)
		#define EA_HAVE_CPP11_RANDOM 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_RANDOM 1
	#else
		#define EA_NO_HAVE_CPP11_RANDOM 1
	#endif
#endif

// #include <chrono> 
#if !defined(EA_HAVE_CPP11_CHRONO) && !defined(EA_NO_HAVE_CPP11_CHRONO)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 540) // Dinkumware. VS2012+
		#define EA_HAVE_CPP11_CHRONO 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4007) // chrono was broken in glibc prior to 4.7.
		#define EA_HAVE_CPP11_CHRONO 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_CHRONO 1
	#else
		#define EA_NO_HAVE_CPP11_CHRONO 1
	#endif
#endif

// #include <scoped_allocator> 
#if !defined(EA_HAVE_CPP11_SCOPED_ALLOCATOR) && !defined(EA_NO_HAVE_CPP11_SCOPED_ALLOCATOR)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 540) // Dinkumware. VS2012+
		#define EA_HAVE_CPP11_SCOPED_ALLOCATOR 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4007)
		#define EA_HAVE_CPP11_SCOPED_ALLOCATOR 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_SCOPED_ALLOCATOR 1
	#else
		#define EA_NO_HAVE_CPP11_SCOPED_ALLOCATOR 1
	#endif
#endif

// #include <initializer_list> 
#if !defined(EA_HAVE_CPP11_INITIALIZER_LIST) && !defined(EA_NO_HAVE_CPP11_INITIALIZER_LIST)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 520) && !defined(EA_COMPILER_NO_INITIALIZER_LISTS) // Dinkumware. VS2010+
		#define EA_HAVE_CPP11_INITIALIZER_LIST 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_CLANG) && (EA_COMPILER_VERSION >= 301) && !defined(EA_COMPILER_NO_INITIALIZER_LISTS) && !defined(EA_PLATFORM_APPLE)
		#define EA_HAVE_CPP11_INITIALIZER_LIST 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBCPP_LIBRARY) && defined(EA_COMPILER_CLANG) && (EA_COMPILER_VERSION >= 301) && !defined(EA_COMPILER_NO_INITIALIZER_LISTS) && !defined(EA_PLATFORM_APPLE)
		#define EA_HAVE_CPP11_INITIALIZER_LIST 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4004) && !defined(EA_COMPILER_NO_INITIALIZER_LISTS) && !defined(EA_PLATFORM_APPLE)
		#define EA_HAVE_CPP11_INITIALIZER_LIST 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1) && !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
		#define EA_HAVE_CPP11_INITIALIZER_LIST 1
	#else
		#define EA_NO_HAVE_CPP11_INITIALIZER_LIST 1
	#endif
#endif

// #include <system_error> 
#if !defined(EA_HAVE_CPP11_SYSTEM_ERROR) && !defined(EA_NO_HAVE_CPP11_SYSTEM_ERROR)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 520) && !(defined(_HAS_CPP0X) && _HAS_CPP0X) // Dinkumware. VS2010+
		#define EA_HAVE_CPP11_SYSTEM_ERROR 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_CLANG) && (EA_COMPILER_VERSION >= 301) && !defined(EA_PLATFORM_APPLE)
		#define EA_HAVE_CPP11_SYSTEM_ERROR 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4004) && !defined(EA_PLATFORM_APPLE)
		#define EA_HAVE_CPP11_SYSTEM_ERROR 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_SYSTEM_ERROR 1
	#else
		#define EA_NO_HAVE_CPP11_SYSTEM_ERROR 1
	#endif
#endif

// #include <codecvt> 
#if !defined(EA_HAVE_CPP11_CODECVT) && !defined(EA_NO_HAVE_CPP11_CODECVT)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 520) // Dinkumware. VS2010+
		#define EA_HAVE_CPP11_CODECVT 1
	// Future versions of libc++ may support this header.  However, at the moment there isn't
	// a reliable way of detecting if this header is available.
	//#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4008)
	//    #define EA_HAVE_CPP11_CODECVT 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_CODECVT 1
	#else
		#define EA_NO_HAVE_CPP11_CODECVT 1
	#endif
#endif

// #include <typeindex> 
#if !defined(EA_HAVE_CPP11_TYPEINDEX) && !defined(EA_NO_HAVE_CPP11_TYPEINDEX)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 520) // Dinkumware. VS2010+
		#define EA_HAVE_CPP11_TYPEINDEX 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4006)
		#define EA_HAVE_CPP11_TYPEINDEX 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_TYPEINDEX 1
	#else
		#define EA_NO_HAVE_CPP11_TYPEINDEX 1
	#endif
#endif




/* EA_HAVE_XXX_DECL */

#if !defined(EA_HAVE_mkstemps_DECL) && !defined(EA_NO_HAVE_mkstemps_DECL)
	#if defined(EA_PLATFORM_APPLE) || defined(CS_UNDEFINED_STRING)
		#define EA_HAVE_mkstemps_DECL 1
	#else
		#define EA_NO_HAVE_mkstemps_DECL 1
	#endif
#endif

#if !defined(EA_HAVE_gettimeofday_DECL) && !defined(EA_NO_HAVE_gettimeofday_DECL)
	#if defined(EA_PLATFORM_POSIX) /* Posix means Linux, Unix, and Macintosh OSX, among others (including Linux-based mobile platforms). */
		#define EA_HAVE_gettimeofday_DECL 1
	#else
		#define EA_NO_HAVE_gettimeofday_DECL 1
	#endif
#endif

#if !defined(EA_HAVE_strcasecmp_DECL) && !defined(EA_NO_HAVE_strcasecmp_DECL)
	#if !defined(EA_PLATFORM_MICROSOFT)
		#define EA_HAVE_strcasecmp_DECL  1     /* This is found as stricmp when not found as strcasecmp */
		#define EA_HAVE_strncasecmp_DECL 1
	#else
		#define EA_HAVE_stricmp_DECL  1
		#define EA_HAVE_strnicmp_DECL 1
	#endif
#endif

#if !defined(EA_HAVE_mmap_DECL) && !defined(EA_NO_HAVE_mmap_DECL)
	#if defined(EA_PLATFORM_POSIX)
		#define EA_HAVE_mmap_DECL 1 /* mmap functionality varies significantly between systems. */
	#else
		#define EA_NO_HAVE_mmap_DECL 1
	#endif
#endif

#if !defined(EA_HAVE_fopen_DECL) && !defined(EA_NO_HAVE_fopen_DECL)
		#define EA_HAVE_fopen_DECL 1 /* C FILE functionality such as fopen */
#endif

#if !defined(EA_HAVE_ISNAN) && !defined(EA_NO_HAVE_ISNAN)
	#if defined(EA_PLATFORM_MICROSOFT) && !defined(EA_PLATFORM_MINGW)
		#define EA_HAVE_ISNAN(x)  _isnan(x)          /* declared in <math.h> */
		#define EA_HAVE_ISINF(x)  !_finite(x)
	#elif defined(EA_PLATFORM_APPLE)
		#define EA_HAVE_ISNAN(x)  std::isnan(x)      /* declared in <cmath> */
		#define EA_HAVE_ISINF(x)  std::isinf(x)
	#elif defined(EA_PLATFORM_ANDROID)
		#define EA_HAVE_ISNAN(x)  __builtin_isnan(x) /* There are a number of standard libraries for Android and it's hard to tell them apart, so just go with builtins */
		#define EA_HAVE_ISINF(x)  __builtin_isinf(x)
	#elif defined(__GNUC__) && defined(__CYGWIN__)
		#define EA_HAVE_ISNAN(x)  __isnand(x)        /* declared nowhere, it seems. */
		#define EA_HAVE_ISINF(x)  __isinfd(x)
	#else
		#define EA_HAVE_ISNAN(x)  std::isnan(x)      /* declared in <cmath> */
		#define EA_HAVE_ISINF(x)  std::isinf(x)
	#endif
#endif

#if !defined(EA_HAVE_itoa_DECL) && !defined(EA_NO_HAVE_itoa_DECL)
	#if defined(EA_COMPILER_MSVC)
		#define EA_HAVE_itoa_DECL 1
	#else
		#define EA_NO_HAVE_itoa_DECL 1
	#endif
#endif

#if !defined(EA_HAVE_nanosleep_DECL) && !defined(EA_NO_HAVE_nanosleep_DECL)
	#if (defined(EA_PLATFORM_UNIX) && !defined(EA_PLATFORM_SONY)) || defined(EA_PLATFORM_IPHONE) || defined(EA_PLATFORM_OSX) || defined(EA_PLATFORM_SONY) || defined(CS_UNDEFINED_STRING)
		#define EA_HAVE_nanosleep_DECL 1
	#else
		#define EA_NO_HAVE_nanosleep_DECL 1
	#endif
#endif

#if !defined(EA_HAVE_utime_DECL) && !defined(EA_NO_HAVE_utime_DECL)
	#if defined(EA_PLATFORM_MICROSOFT)
		#define EA_HAVE_utime_DECL _utime
	#elif EA_PLATFORM_UNIX
		#define EA_HAVE_utime_DECL utime
	#else
		#define EA_NO_HAVE_utime_DECL 1
	#endif
#endif

#if !defined(EA_HAVE_ftruncate_DECL) && !defined(EA_NO_HAVE_ftruncate_DECL)
	#if !defined(__MINGW32__)
		#define EA_HAVE_ftruncate_DECL 1
	#else
		#define EA_NO_HAVE_ftruncate_DECL 1
	#endif
#endif

#if !defined(EA_HAVE_localtime_DECL) && !defined(EA_NO_HAVE_localtime_DECL)
		#define EA_HAVE_localtime_DECL 1
#endif

#if !defined(EA_HAVE_pthread_getattr_np_DECL) && !defined(EA_NO_HAVE_pthread_getattr_np_DECL)
	#if defined(EA_PLATFORM_LINUX)
		#define EA_HAVE_pthread_getattr_np_DECL 1
	#else
		#define EA_NO_HAVE_pthread_getattr_np_DECL 1
	#endif
#endif



/* EA_HAVE_XXX_IMPL*/

#if !defined(EA_HAVE_WCHAR_IMPL) && !defined(EA_NO_HAVE_WCHAR_IMPL)
	#if defined(EA_PLATFORM_DESKTOP)
		#define EA_HAVE_WCHAR_IMPL 1      /* Specifies if wchar_t string functions are provided, such as wcslen, wprintf, etc. Implies EA_HAVE_WCHAR_H */
	#else
		#define EA_NO_HAVE_WCHAR_IMPL 1
	#endif
#endif

#if !defined(EA_HAVE_getenv_IMPL) && !defined(EA_NO_HAVE_getenv_IMPL)
	#if (defined(EA_PLATFORM_DESKTOP) || defined(EA_PLATFORM_UNIX)) && !defined(EA_PLATFORM_WINRT)
		#define EA_HAVE_getenv_IMPL 1
	#else
		#define EA_NO_HAVE_getenv_IMPL 1
	#endif
#endif

#if !defined(EA_HAVE_setenv_IMPL) && !defined(EA_NO_HAVE_setenv_IMPL)
	#if defined(EA_PLATFORM_UNIX) && defined(EA_PLATFORM_POSIX)
		#define EA_HAVE_setenv_IMPL 1
	#else
		#define EA_NO_HAVE_setenv_IMPL 1
	#endif
#endif

#if !defined(EA_HAVE_unsetenv_IMPL) && !defined(EA_NO_HAVE_unsetenv_IMPL)
	#if defined(EA_PLATFORM_UNIX) && defined(EA_PLATFORM_POSIX)
		#define EA_HAVE_unsetenv_IMPL 1
	#else
		#define EA_NO_HAVE_unsetenv_IMPL 1
	#endif
#endif

#if !defined(EA_HAVE_putenv_IMPL) && !defined(EA_NO_HAVE_putenv_IMPL)
	#if (defined(EA_PLATFORM_DESKTOP) || defined(EA_PLATFORM_UNIX)) && !defined(EA_PLATFORM_WINRT)
		#define EA_HAVE_putenv_IMPL 1        /* With Microsoft compilers you may need to use _putenv, as they have deprecated putenv. */
	#else
		#define EA_NO_HAVE_putenv_IMPL 1
	#endif
#endif

#if !defined(EA_HAVE_time_IMPL) && !defined(EA_NO_HAVE_time_IMPL)
		#define EA_HAVE_time_IMPL 1
		#define EA_HAVE_clock_IMPL 1
#endif

// <cstdio> fopen()
#if !defined(EA_HAVE_fopen_IMPL) && !defined(EA_NO_HAVE_fopen_IMPL)
		#define EA_HAVE_fopen_IMPL 1  /* C FILE functionality such as fopen */
#endif

// <arpa/inet.h> inet_ntop()
#if !defined(EA_HAVE_inet_ntop_IMPL) && !defined(EA_NO_HAVE_inet_ntop_IMPL)
	#if (defined(EA_PLATFORM_UNIX) || defined(EA_PLATFORM_POSIX)) && !defined(EA_PLATFORM_SONY) && !defined(CS_UNDEFINED_STRING) 
		#define EA_HAVE_inet_ntop_IMPL 1  /* This doesn't identify if the platform SDK has some alternative function that does the same thing; */
		#define EA_HAVE_inet_pton_IMPL 1  /* it identifies strictly the <arpa/inet.h> inet_ntop and inet_pton functions. For example, Microsoft has InetNtop in <Ws2tcpip.h> */
	#else
		#define EA_NO_HAVE_inet_ntop_IMPL 1
		#define EA_NO_HAVE_inet_pton_IMPL 1
	#endif
#endif

// <time.h> clock_gettime()
#if !defined(EA_HAVE_clock_gettime_IMPL) && !defined(EA_NO_HAVE_clock_gettime_IMPL)
	#if defined(EA_PLATFORM_LINUX) || defined(__CYGWIN__) || (defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)) || (defined(EA_PLATFORM_POSIX) && defined(_CPPLIB_VER) /*Dinkumware*/)
		#define EA_HAVE_clock_gettime_IMPL 1 /* You need to link the 'rt' library to get this */
	#else
		#define EA_NO_HAVE_clock_gettime_IMPL 1
	#endif
#endif

#if !defined(EA_HAVE_getcwd_IMPL) && !defined(EA_NO_HAVE_getcwd_IMPL)
	#if (defined(EA_PLATFORM_DESKTOP) || defined(EA_PLATFORM_UNIX)) && !defined(EA_PLATFORM_ANDROID) && !defined(EA_PLATFORM_WINRT)
		#define EA_HAVE_getcwd_IMPL 1       /* With Microsoft compilers you may need to use _getcwd, as they have deprecated getcwd. And in any case it's present at <direct.h> */
	#else
		#define EA_NO_HAVE_getcwd_IMPL 1
	#endif
#endif

#if !defined(EA_HAVE_tmpnam_IMPL) && !defined(EA_NO_HAVE_tmpnam_IMPL)
	#if (defined(EA_PLATFORM_DESKTOP) || defined(EA_PLATFORM_UNIX)) && !defined(EA_PLATFORM_ANDROID)
		#define EA_HAVE_tmpnam_IMPL 1
	#else
		#define EA_NO_HAVE_tmpnam_IMPL 1
	#endif
#endif

// nullptr, the built-in C++11 type.
// This EA_HAVE is deprecated, as EA_COMPILER_NO_NULLPTR is more appropriate, given that nullptr is a compiler-level feature and not a library feature.
#if !defined(EA_HAVE_nullptr_IMPL) && !defined(EA_NO_HAVE_nullptr_IMPL)
	#if defined(EA_COMPILER_NO_NULLPTR)
		#define EA_NO_HAVE_nullptr_IMPL 1
	#else
		#define EA_HAVE_nullptr_IMPL 1
	#endif
#endif

// <cstddef> std::nullptr_t
// Note that <EABase/nullptr.h> implements a portable nullptr implementation, but this 
// EA_HAVE specifically refers to std::nullptr_t from the standard libraries.
#if !defined(EA_HAVE_nullptr_t_IMPL) && !defined(EA_NO_HAVE_nullptr_t_IMPL)
	#if defined(EA_COMPILER_CPP11_ENABLED)
		// VS2010+ with its default Dinkumware standard library.
		#if defined(_MSC_VER) && (_MSC_VER >= 1600) && defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY)
			#define EA_HAVE_nullptr_t_IMPL 1

		#elif defined(EA_HAVE_LIBCPP_LIBRARY) // clang/llvm libc++
			#define EA_HAVE_nullptr_t_IMPL 1

		#elif defined(EA_HAVE_LIBSTDCPP_LIBRARY) // GNU libstdc++
			// Unfortunately __GLIBCXX__ date values don't go strictly in version ordering.
			#if (__GLIBCXX__ >= 20110325) && (__GLIBCXX__ != 20120702) && (__GLIBCXX__ != 20110428)
				#define EA_HAVE_nullptr_t_IMPL 1
			#else
				#define EA_NO_HAVE_nullptr_t_IMPL 1
			#endif
			
		// We simply assume that the standard library (e.g. Dinkumware) provides std::nullptr_t.
		#elif defined(__clang__)
			#define EA_HAVE_nullptr_t_IMPL 1

		// With GCC compiler >= 4.6, std::nullptr_t is always defined in <cstddef>, in practice.
		#elif defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4006)
			#define EA_HAVE_nullptr_t_IMPL 1

		// The EDG compiler provides nullptr, but uses an older standard library that doesn't support std::nullptr_t.
		#elif defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403) 
			#define EA_HAVE_nullptr_t_IMPL 1
			
		#else
			#define EA_NO_HAVE_nullptr_t_IMPL 1
		#endif
	#else
		#define EA_NO_HAVE_nullptr_t_IMPL 1
	#endif
#endif

// <exception> std::terminate
#if !defined(EA_HAVE_std_terminate_IMPL) && !defined(EA_NO_HAVE_std_terminate_IMPL)
	#if !defined(EA_PLATFORM_IPHONE) && !defined(EA_PLATFORM_ANDROID)
		#define EA_HAVE_std_terminate_IMPL 1 /* iOS doesn't appear to provide an implementation for std::terminate under the armv6 target. */
	#else
		#define EA_NO_HAVE_std_terminate_IMPL 1
	#endif
#endif

// <iterator>: std::begin, std::end, std::prev, std::next, std::move_iterator.
#if !defined(EA_HAVE_CPP11_ITERATOR_IMPL) && !defined(EA_NO_HAVE_CPP11_ITERATOR_IMPL)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 520) && !(defined(_HAS_CPP0X) && _HAS_CPP0X) // Dinkumware. VS2010+
		#define EA_HAVE_CPP11_ITERATOR_IMPL 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4006)
		#define EA_HAVE_CPP11_ITERATOR_IMPL 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_ITERATOR_IMPL 1
	#else
		#define EA_NO_HAVE_CPP11_ITERATOR_IMPL 1
	#endif
#endif

// <memory>: std::weak_ptr, std::shared_ptr, std::unique_ptr, std::bad_weak_ptr, std::owner_less
#if !defined(EA_HAVE_CPP11_SMART_POINTER_IMPL) && !defined(EA_NO_HAVE_CPP11_SMART_POINTER_IMPL)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 520) && !(defined(_HAS_CPP0X) && _HAS_CPP0X) // Dinkumware. VS2010+
		#define EA_HAVE_CPP11_SMART_POINTER_IMPL 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4004)
		#define EA_HAVE_CPP11_SMART_POINTER_IMPL 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_SMART_POINTER_IMPL 1
	#else
		#define EA_NO_HAVE_CPP11_SMART_POINTER_IMPL 1
	#endif
#endif

// <functional>: std::function, std::mem_fn, std::bad_function_call, std::is_bind_expression, std::is_placeholder, std::reference_wrapper, std::hash, std::bind, std::ref, std::cref.
#if !defined(EA_HAVE_CPP11_FUNCTIONAL_IMPL) && !defined(EA_NO_HAVE_CPP11_FUNCTIONAL_IMPL)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 520) && !(defined(_HAS_CPP0X) && _HAS_CPP0X) // Dinkumware. VS2010+
		#define EA_HAVE_CPP11_FUNCTIONAL_IMPL 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4004)
		#define EA_HAVE_CPP11_FUNCTIONAL_IMPL 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_FUNCTIONAL_IMPL 1
	#else
		#define EA_NO_HAVE_CPP11_FUNCTIONAL_IMPL 1
	#endif
#endif

// <exception> std::current_exception, std::rethrow_exception, std::exception_ptr, std::make_exception_ptr
#if !defined(EA_HAVE_CPP11_EXCEPTION_IMPL) && !defined(EA_NO_HAVE_CPP11_EXCEPTION_IMPL)
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY) && (_CPPLIB_VER >= 520) && !(defined(_HAS_CPP0X) && _HAS_CPP0X) // Dinkumware. VS2010+
		#define EA_HAVE_CPP11_EXCEPTION_IMPL 1
	#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(EA_HAVE_LIBSTDCPP_LIBRARY) && defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4004)
		#define EA_HAVE_CPP11_EXCEPTION_IMPL 1
	#elif defined(EA_HAVE_LIBCPP_LIBRARY) && (_LIBCPP_VERSION >= 1)
		#define EA_HAVE_CPP11_EXCEPTION_IMPL 1
	#else
		#define EA_NO_HAVE_CPP11_EXCEPTION_IMPL 1
	#endif
#endif




/* Implementations that all platforms seem to have: */
/*
	alloca
	malloc
	calloc
	strtoll
	strtoull
	vsprintf
	vsnprintf
*/

/* Implementations that we don't care about: */
/*
	bcopy   -- Just use memmove or some customized equivalent. bcopy offers no practical benefit.
	strlcpy -- So few platforms have this built-in that we get no benefit from using it. Use EA::StdC::Strlcpy instead.
	strlcat -- "
*/



/*-----------------------------------------------------------------------------
	EABASE_USER_HAVE_HEADER
	
	This allows the user to define a header file to be #included after the 
	eahave.h's contents are compiled. A primary use of this is to override
	the contents of this header file. You can define the overhead header 
	file name in-code or define it globally as part of your build file.
	
	Example usage:
	   #define EABASE_USER_HAVE_HEADER "MyHaveOverrides.h" 
	   #include <EABase/eahave.h>
---------------------------------------------------------------------------*/

#ifdef EABASE_USER_HAVE_HEADER
	#include EABASE_USER_HAVE_HEADER
#endif


#endif /* Header include guard */



