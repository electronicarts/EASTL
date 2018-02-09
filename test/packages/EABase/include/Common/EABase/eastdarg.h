/*-----------------------------------------------------------------------------
 * eastdarg.h
 *
 * Copyright (c) Electronic Arts Inc. All rights reserved.
 *---------------------------------------------------------------------------*/


#ifndef INCLUDED_eastdarg_H
#define INCLUDED_eastdarg_H


#include <EABase/eabase.h>
#include <stdarg.h>


// VA_ARG_COUNT
//
// Returns the number of arguments passed to a macro's ... argument.
// This applies to macros only and not functions.
//
// Example usage:
//    assert(VA_ARG_COUNT() == 0);
//    assert(VA_ARG_COUNT(a) == 1);
//    assert(VA_ARG_COUNT(a, b) == 2);
//    assert(VA_ARG_COUNT(a, b, c) == 3);
//
#if !defined(VA_ARG_COUNT)
	#define VA_ARG_COUNT(...)                         VA_ARG_COUNT_II((VA_ARG_COUNT_PREFIX_ ## __VA_ARGS__ ## _VA_ARG_COUNT_POSTFIX,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0))
	#define VA_ARG_COUNT_II(__args)                   VA_ARG_COUNT_I __args
	#define VA_ARG_COUNT_PREFIX__VA_ARG_COUNT_POSTFIX ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,0
	#define VA_ARG_COUNT_I(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,N,...) N
#endif


// va_copy
//
// va_copy is required by C++11
// C++11 and C99 require va_copy to be #defined and implemented.
// http://en.cppreference.com/w/cpp/utility/variadic/va_copy
//
// Example usage:
//     void Func(char* p, ...){
//         va_list args, argsCopy;
//         va_start(args, p);
//         va_copy(argsCopy, args);
//           (use args)
//           (use argsCopy, which acts the same as args)
//         va_end(args);
//         va_end(argsCopy);
//     }
//
#ifndef va_copy
	#if defined(__va_copy) // GCC and others define this for non-C99 compatibility.
		#define va_copy(dest, src) __va_copy((dest), (src))
	#else
		// This may not work for some platforms, depending on their ABI.
		// It works for Microsoft x86,x64, and PowerPC-based platforms.
		#define va_copy(dest, src) memcpy(&(dest), &(src), sizeof(va_list))
	#endif
#endif
 
 
 
// va_list_reference
//
// va_list_reference is not part of the C or C++ standards.
// It allows you to pass a va_list by reference to another
// function instead of by value. You cannot simply use va_list&
// as that won't work with many va_list implementations because 
// they are implemented as arrays (which can't be passed by
// reference to a function without decaying to a pointer).
//
// Example usage:
//     void Test(va_list_reference args){
//         printf("%d", va_arg(args, int));
//     }
//     void Func(char* p, ...){
//         va_list args;
//         va_start(args, p);
//         Test(args); // Upon return args will be modified.
//         va_end(args);
//     }
#ifndef va_list_reference
	#if defined(EA_PLATFORM_MICROSOFT) || (EA_PLATFORM_PTR_SIZE == 4) || (defined(EA_PLATFORM_APPLE) && defined(EA_PROCESSOR_ARM64)) ||  defined(CS_UNDEFINED_STRING) || (defined(EA_PLATFORM_ANDROID) && defined(EA_PROCESSOR_ARM64))
		// This is required for platform ABIs in which va_list is a struct or pointer.
		#define va_list_reference va_list&
	#else
		// This is required for platform ABIs in which va_list is defined to be an array.
		#define va_list_reference va_list
	#endif
#endif




#endif /* Header include guard */



