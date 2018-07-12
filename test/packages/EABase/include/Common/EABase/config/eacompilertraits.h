/*-----------------------------------------------------------------------------
 * config/eacompilertraits.h
 *
 * Copyright (c) Electronic Arts Inc. All rights reserved.
 *-----------------------------------------------------------------------------
 * Currently supported defines include:
 *    EA_PREPROCESSOR_JOIN
 *    
 *    EA_COMPILER_IS_ANSIC
 *    EA_COMPILER_IS_C99
 *    EA_COMPILER_IS_C11
 *    EA_COMPILER_HAS_C99_TYPES
 *    EA_COMPILER_IS_CPLUSPLUS
 *    EA_COMPILER_MANAGED_CPP
 *    EA_COMPILER_INTMAX_SIZE
 *    EA_OFFSETOF
 *    EA_SIZEOF_MEMBER
 *
 *    EA_ALIGN_OF()
 *    EA_ALIGN_MAX_STATIC / EA_ALIGN_MAX_AUTOMATIC
 *    EA_ALIGN() / EA_PREFIX_ALIGN() / EA_POSTFIX_ALIGN()
 *    EA_ALIGNED()
 *    EA_PACKED()
 *
 *    EA_LIKELY()
 *    EA_UNLIKELY()
 *    EA_INIT_PRIORITY()
 *    EA_MAY_ALIAS()
 *    EA_ASSUME()
 *    EA_ANALYSIS_ASSUME()
 *    EA_PURE
 *    EA_WEAK
 *    EA_UNUSED()
 *    EA_EMPTY()
 *
 *    EA_WCHAR_T_NON_NATIVE
 *    EA_WCHAR_SIZE = <n bytes>
 *
 *    EA_RESTRICT
 *    EA_DEPRECATED   / EA_PREFIX_DEPRECATED   / EA_POSTFIX_DEPRECATED
 *    EA_FORCE_INLINE / EA_PREFIX_FORCE_INLINE / EA_POSTFIX_FORCE_INLINE
 *    EA_NO_INLINE    / EA_PREFIX_NO_INLINE    / EA_POSTFIX_NO_INLINE
 *    EA_NO_VTABLE    / EA_CLASS_NO_VTABLE     / EA_STRUCT_NO_VTABLE
 *    EA_PASCAL
 *    EA_PASCAL_FUNC()
 *    EA_SSE = [0 | 1]
 *    EA_IMPORT
 *    EA_EXPORT
 *    EA_PRAGMA_ONCE_SUPPORTED
 *    EA_ONCE
 *    EA_OVERRIDE
 *    EA_INHERITANCE_FINAL
 *    EA_SEALED
 *    EA_ABSTRACT
 *    EA_CONSTEXPR / EA_CONSTEXPR_OR_CONST
 *    EA_EXTERN_TEMPLATE
 *    EA_NOEXCEPT
 *    EA_NORETURN
 *    EA_CARRIES_DEPENDENCY
 *    EA_NON_COPYABLE / struct EANonCopyable
 *    EA_OPTIMIZE_OFF / EA_OPTIMIZE_ON
 *    EA_SIGNED_RIGHT_SHIFT_IS_UNSIGNED
 *
 *    EA_DISABLE_VC_WARNING    / EA_RESTORE_VC_WARNING / EA_DISABLE_ALL_VC_WARNINGS / EA_RESTORE_ALL_VC_WARNINGS
 *    EA_DISABLE_GCC_WARNING   / EA_RESTORE_GCC_WARNING
 *    EA_DISABLE_CLANG_WARNING / EA_RESTORE_CLANG_WARNING
 *    EA_DISABLE_SN_WARNING    / EA_RESTORE_SN_WARNING / EA_DISABLE_ALL_SN_WARNINGS / EA_RESTORE_ALL_SN_WARNINGS
 *    EA_DISABLE_GHS_WARNING   / EA_RESTORE_GHS_WARNING
 *    EA_DISABLE_EDG_WARNING   / EA_RESTORE_EDG_WARNING
 *    EA_DISABLE_CW_WARNING    / EA_RESTORE_CW_WARNING
 *
 *    EA_DISABLE_DEFAULT_CTOR
 *    EA_DISABLE_COPY_CTOR
 *    EA_DISABLE_MOVE_CTOR
 *    EA_DISABLE_ASSIGNMENT_OPERATOR
 *    EA_DISABLE_MOVE_OPERATOR
 *
 *  Todo:
 *    Find a way to reliably detect wchar_t size at preprocessor time and 
 *    implement it below for EA_WCHAR_SIZE.
 *
 *  Todo:
 *    Find out how to support EA_PASCAL and EA_PASCAL_FUNC for systems in 
 *    which it hasn't yet been found out for.
 *---------------------------------------------------------------------------*/


#ifndef INCLUDED_eacompilertraits_H
#define INCLUDED_eacompilertraits_H

	#include <EABase/config/eaplatform.h>
	#include <EABase/config/eacompiler.h>


	// Metrowerks uses #defines in its core C header files to define 
	// the kind of information we need below (e.g. C99 compatibility)



	// Determine if this compiler is ANSI C compliant and if it is C99 compliant.
	#if defined(__STDC__)
		#define EA_COMPILER_IS_ANSIC 1    // The compiler claims to be ANSI C

		// Is the compiler a C99 compiler or equivalent?
		// From ISO/IEC 9899:1999:
		//    6.10.8 Predefined macro names
		//    __STDC_VERSION__ The integer constant 199901L. (150)
		//
		//    150) This macro was not specified in ISO/IEC 9899:1990 and was 
		//    specified as 199409L in ISO/IEC 9899/AMD1:1995. The intention 
		//    is that this will remain an integer constant of type long int 
		//    that is increased with each revision of this International Standard.
		//
		#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
			#define EA_COMPILER_IS_C99 1
		#endif

 		// Is the compiler a C11 compiler?
 		// From ISO/IEC 9899:2011:
		//   Page 176, 6.10.8.1 (Predefined macro names) :
 		//   __STDC_VERSION__ The integer constant 201112L. (178)
		//
		#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
			#define EA_COMPILER_IS_C11 1
		#endif
	#endif

	// Some compilers (e.g. GCC) define __USE_ISOC99 if they are not 
	// strictly C99 compilers (or are simply C++ compilers) but are set 
	// to use C99 functionality. Metrowerks defines _MSL_C99 as 1 in 
	// this case, but 0 otherwise.
	#if (defined(__USE_ISOC99) || (defined(_MSL_C99) && (_MSL_C99 == 1))) && !defined(EA_COMPILER_IS_C99)
		#define EA_COMPILER_IS_C99 1
	#endif
 
	// Metrowerks defines C99 types (e.g. intptr_t) instrinsically when in C99 mode (-lang C99 on the command line).
	#if (defined(_MSL_C99) && (_MSL_C99 == 1))
		#define EA_COMPILER_HAS_C99_TYPES 1
	#endif

	#if defined(__GNUC__) 
		#if (((__GNUC__ * 100) + __GNUC_MINOR__) >= 302) // Also, GCC defines _HAS_C9X.
			#define EA_COMPILER_HAS_C99_TYPES 1 // The compiler is not necessarily a C99 compiler, but it defines C99 types.
			
			#ifndef __STDC_LIMIT_MACROS
				#define __STDC_LIMIT_MACROS 1
			#endif
			
			#ifndef __STDC_CONSTANT_MACROS
				#define __STDC_CONSTANT_MACROS 1    // This tells the GCC compiler that we want it to use its native C99 types.
			#endif
		#endif
	#endif

	#if defined(_MSC_VER) && (_MSC_VER >= 1600)
		#define EA_COMPILER_HAS_C99_TYPES 1
	#endif

	#ifdef  __cplusplus
		#define EA_COMPILER_IS_CPLUSPLUS 1
	#endif


	// ------------------------------------------------------------------------
	// EA_PREPROCESSOR_JOIN
	//
	// This macro joins the two arguments together, even when one of  
	// the arguments is itself a macro (see 16.3.1 in C++98 standard). 
	// This is often used to create a unique name with __LINE__.
	//
	// For example, this declaration:
	//    char EA_PREPROCESSOR_JOIN(unique_, __LINE__);
	// expands to this:
	//    char unique_73;
	//
	// Note that all versions of MSVC++ up to at least version 7.1 
	// fail to properly compile macros that use __LINE__ in them
	// when the "program database for edit and continue" option
	// is enabled. The result is that __LINE__ gets converted to 
	// something like __LINE__(Var+37).
	//
	#ifndef EA_PREPROCESSOR_JOIN
		#define EA_PREPROCESSOR_JOIN(a, b)  EA_PREPROCESSOR_JOIN1(a, b)
		#define EA_PREPROCESSOR_JOIN1(a, b) EA_PREPROCESSOR_JOIN2(a, b)
		#define EA_PREPROCESSOR_JOIN2(a, b) a##b
	#endif


	// ------------------------------------------------------------------------
	// EA_STRINGIFY
	//
	// Example usage:
	//     printf("Line: %s", EA_STRINGIFY(__LINE__));
	//
	#ifndef EA_STRINGIFY
		#define EA_STRINGIFY(x)     EA_STRINGIFYIMPL(x)
		#define EA_STRINGIFYIMPL(x) #x
	#endif


	// ------------------------------------------------------------------------
	// EA_IDENTITY
	//
	#ifndef EA_IDENTITY
		#define EA_IDENTITY(x) x
	#endif


	// ------------------------------------------------------------------------
	// EA_COMPILER_MANAGED_CPP
	// Defined if this is being compiled with Managed C++ extensions
	#ifdef EA_COMPILER_MSVC
		#if EA_COMPILER_VERSION >= 1300
			#ifdef _MANAGED
				#define EA_COMPILER_MANAGED_CPP 1
			#endif
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_COMPILER_INTMAX_SIZE
	//
	// This is related to the concept of intmax_t uintmax_t, but is available 
	// in preprocessor form as opposed to compile-time form. At compile-time
	// you can use intmax_t and uintmax_t to use the actual types.
	//
	#if defined(__GNUC__) && defined(__x86_64__)
		#define EA_COMPILER_INTMAX_SIZE 16  // intmax_t is __int128_t (GCC extension) and is 16 bytes.
	#else
		#define EA_COMPILER_INTMAX_SIZE 8   // intmax_t is int64_t and is 8 bytes.
	#endif



	// ------------------------------------------------------------------------
	// EA_LPAREN / EA_RPAREN / EA_COMMA / EA_SEMI
	//
	// These are used for using special characters in macro-using expressions.
	// Note that this macro intentionally uses (), as in some cases it can't 
	// work unless it does.
	//
	// Example usage:
	//     int x = SOME_MACRO(SomeTemplate<int EA_COMMA() int EACOMMA() char>);
	//
	#ifndef EA_LPAREN 
		#define EA_LPAREN() (
	#endif
	#ifndef EA_RPAREN 
		#define EA_RPAREN() )
	#endif
	#ifndef EA_COMMA 
		#define EA_COMMA()  ,
	#endif
	#ifndef EA_SEMI 
		#define EA_SEMI()   ;
	#endif




	// ------------------------------------------------------------------------
	// EA_OFFSETOF
	// Implements a portable version of the non-standard offsetof macro.
	//
	// The offsetof macro is guaranteed to only work with POD types. However, we wish to use
	// it for non-POD types but where we know that offsetof will still work for the cases 
	// in which we use it. GCC unilaterally gives a warning when using offsetof with a non-POD,
	// even if the given usage happens to work. So we make a workaround version of offsetof
	// here for GCC which has the same effect but tricks the compiler into not issuing the warning.
	// The 65536 does the compiler fooling; the reinterpret_cast prevents the possibility of
	// an overloaded operator& for the class getting in the way.
	//
	// Example usage:
	//     struct A{ int x; int y; };
	//     size_t n = EA_OFFSETOF(A, y);
	//
	#if defined(__GNUC__)                       // We can't use GCC 4's __builtin_offsetof because it mistakenly complains about non-PODs that are really PODs.
		#define EA_OFFSETOF(struct_, member_)  ((size_t)(((uintptr_t)&reinterpret_cast<const volatile char&>((((struct_*)65536)->member_))) - 65536))
	#else
		#define EA_OFFSETOF(struct_, member_)  offsetof(struct_, member_)
	#endif

	// ------------------------------------------------------------------------
	// EA_SIZEOF_MEMBER
	// Implements a portable way to determine the size of a member. 
	//
	// The EA_SIZEOF_MEMBER simply returns the size of a member within a class or struct; member
	// access rules still apply. We offer two approaches depending on the compiler's support for non-static member 
	// initializers although most C++11 compilers support this.
	//
	// Example usage:
	//     struct A{ int x; int y; };
	//     size_t n = EA_SIZEOF_MEMBER(A, y);
	//
	#ifndef EA_COMPILER_NO_EXTENDED_SIZEOF
		#define EA_SIZEOF_MEMBER(struct_, member_) (sizeof(struct_::member_))
	#else
		#define EA_SIZEOF_MEMBER(struct_, member_) (sizeof(((struct_*)0)->member_))
	#endif

	// ------------------------------------------------------------------------
	// alignment expressions
	//
	// Here we define
	//    EA_ALIGN_OF(type)         // Returns size_t.
	//    EA_ALIGN_MAX_STATIC       // The max align value that the compiler will respect for EA_ALIGN for static data (global and static variables). Some compilers allow high values, some allow no more than 8. EA_ALIGN_MIN is assumed to be 1.
	//    EA_ALIGN_MAX_AUTOMATIC    // The max align value for automatic variables (variables declared as local to a function).
	//    EA_ALIGN(n)               // Used as a prefix. n is byte alignment, with being a power of two. Most of the time you can use this and avoid using EA_PREFIX_ALIGN/EA_POSTFIX_ALIGN.
	//    EA_ALIGNED(t, v, n)       // Type, variable, alignment. Used to align an instance. You should need this only for unusual compilers.
	//    EA_PACKED                 // Specifies that the given structure be packed (and not have its members aligned).
	//
	// Also we define the following for rare cases that it's needed.
	//    EA_PREFIX_ALIGN(n)        // n is byte alignment, with being a power of two. You should need this only for unusual compilers.
	//    EA_POSTFIX_ALIGN(n)       // Valid values for n are 1, 2, 4, 8, etc. You should need this only for unusual compilers.
	//
	// Example usage:
	//    size_t x = EA_ALIGN_OF(int);                                  Non-aligned equivalents.        Meaning
	//    EA_PREFIX_ALIGN(8) int x = 5;                                 int x = 5;                      Align x on 8 for compilers that require prefix attributes. Can just use EA_ALIGN instead.
	//    EA_ALIGN(8) int x;                                            int x;                          Align x on 8 for compilers that allow prefix attributes.
	//    int x EA_POSTFIX_ALIGN(8);                                    int x;                          Align x on 8 for compilers that require postfix attributes.
	//    int x EA_POSTFIX_ALIGN(8) = 5;                                int x = 5;                      Align x on 8 for compilers that require postfix attributes.
	//    int x EA_POSTFIX_ALIGN(8)(5);                                 int x(5);                       Align x on 8 for compilers that require postfix attributes.
	//    struct EA_PREFIX_ALIGN(8) X { int x; } EA_POSTFIX_ALIGN(8);   struct X { int x; };            Define X as a struct which is aligned on 8 when used.
	//    EA_ALIGNED(int, x, 8) = 5;                                    int x = 5;                      Align x on 8.
	//    EA_ALIGNED(int, x, 16)(5);                                    int x(5);                       Align x on 16.
	//    EA_ALIGNED(int, x[3], 16);                                    int x[3];                       Align x array on 16.
	//    EA_ALIGNED(int, x[3], 16) = { 1, 2, 3 };                      int x[3] = { 1, 2, 3 };         Align x array on 16.
	//    int x[3] EA_PACKED;                                           int x[3];                       Pack the 3 ints of the x array. GCC doesn't seem to support packing of int arrays.
	//    struct EA_ALIGN(32) X { int x; int y; };                      struct X { int x; };            Define A as a struct which is aligned on 32 when used.
	//    EA_ALIGN(32) struct X { int x; int y; } Z;                    struct X { int x; } Z;          Define A as a struct, and align the instance Z on 32.
	//    struct X { int x EA_PACKED; int y EA_PACKED; };               struct X { int x; int y; };     Pack the x and y members of struct X.
	//    struct X { int x; int y; } EA_PACKED;                         struct X { int x; int y; };     Pack the members of struct X.
	//    typedef EA_ALIGNED(int, int16, 16); int16 n16;                typedef int int16; int16 n16;   Define int16 as an int which is aligned on 16.
	//    typedef EA_ALIGNED(X, X16, 16); X16 x16;                      typedef X X16; X16 x16;         Define X16 as an X which is aligned on 16.

	#if !defined(EA_ALIGN_MAX)          // If the user hasn't globally set an alternative value...
		#if defined(EA_PROCESSOR_ARM)                       // ARM compilers in general tend to limit automatic variables to 8 or less.
			#define EA_ALIGN_MAX_STATIC    1048576
			#define EA_ALIGN_MAX_AUTOMATIC       1          // Typically they support only built-in natural aligment types (both arm-eabi and apple-abi).
		#elif defined(EA_PLATFORM_APPLE)
			#define EA_ALIGN_MAX_STATIC    1048576
			#define EA_ALIGN_MAX_AUTOMATIC      16
		#else
			#define EA_ALIGN_MAX_STATIC    1048576          // Arbitrarily high value. What is the actual max?
			#define EA_ALIGN_MAX_AUTOMATIC 1048576
		#endif
	#endif

	// EDG intends to be compatible with GCC but has a bug whereby it 
	// fails to support calling a constructor in an aligned declaration when 
	// using postfix alignment attributes. Prefix works for alignment, but does not align
	// the size like postfix does.  Prefix also fails on templates.  So gcc style post fix
	// is still used, but the user will need to use EA_POSTFIX_ALIGN before the constructor parameters.
	#if   defined(__GNUC__) && (__GNUC__ < 3)
		#define EA_ALIGN_OF(type) ((size_t)__alignof__(type))
		#define EA_ALIGN(n)
		#define EA_PREFIX_ALIGN(n)
		#define EA_POSTFIX_ALIGN(n) __attribute__((aligned(n)))
		#define EA_ALIGNED(variable_type, variable, n) variable_type variable __attribute__((aligned(n)))
		#define EA_PACKED __attribute__((packed))

	// GCC 3.x+, IBM, and clang support prefix attributes.
	#elif (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__xlC__) || defined(__clang__)
		#define EA_ALIGN_OF(type) ((size_t)__alignof__(type))
		#define EA_ALIGN(n) __attribute__((aligned(n)))
		#define EA_PREFIX_ALIGN(n)
		#define EA_POSTFIX_ALIGN(n) __attribute__((aligned(n)))
		#define EA_ALIGNED(variable_type, variable, n) variable_type variable __attribute__((aligned(n)))
		#define EA_PACKED __attribute__((packed))

	// Metrowerks supports prefix attributes.
	// Metrowerks does not support packed alignment attributes.
	#elif defined(EA_COMPILER_INTEL) || defined(CS_UNDEFINED_STRING) || (defined(EA_COMPILER_MSVC) && (EA_COMPILER_VERSION >= 1300))
		#define EA_ALIGN_OF(type) ((size_t)__alignof(type))
		#define EA_ALIGN(n) __declspec(align(n))
		#define EA_PREFIX_ALIGN(n) EA_ALIGN(n)
		#define EA_POSTFIX_ALIGN(n)
		#define EA_ALIGNED(variable_type, variable, n) EA_ALIGN(n) variable_type variable
		#define EA_PACKED // See EA_PRAGMA_PACK_VC for an alternative.

	// Arm brand compiler
	#elif defined(EA_COMPILER_ARM)
		#define EA_ALIGN_OF(type) ((size_t)__ALIGNOF__(type))
		#define EA_ALIGN(n) __align(n)
		#define EA_PREFIX_ALIGN(n) __align(n)
		#define EA_POSTFIX_ALIGN(n)
		#define EA_ALIGNED(variable_type, variable, n) __align(n) variable_type variable
		#define EA_PACKED __packed

	#else // Unusual compilers
		// There is nothing we can do about some of these. This is not as bad a problem as it seems.
		// If the given platform/compiler doesn't support alignment specifications, then it's somewhat
		// likely that alignment doesn't matter for that platform. Otherwise they would have defined 
		// functionality to manipulate alignment.
		#define EA_ALIGN(n)
		#define EA_PREFIX_ALIGN(n)
		#define EA_POSTFIX_ALIGN(n)
		#define EA_ALIGNED(variable_type, variable, n) variable_type variable
		#define EA_PACKED

		#ifdef __cplusplus
			template <typename T> struct EAAlignOf1 { enum { s = sizeof (T), value = s ^ (s & (s - 1)) }; };
			template <typename T> struct EAAlignOf2;
			template <int size_diff> struct helper { template <typename T> struct Val { enum { value = size_diff }; }; };
			template <> struct helper<0> { template <typename T> struct Val { enum { value = EAAlignOf2<T>::value }; }; };
			template <typename T> struct EAAlignOf2 { struct Big { T x; char c; };
			enum { diff = sizeof (Big) - sizeof (T), value = helper<diff>::template Val<Big>::value }; };
			template <typename T> struct EAAlignof3 { enum { x = EAAlignOf2<T>::value, y = EAAlignOf1<T>::value, value = x < y ? x : y }; };
			#define EA_ALIGN_OF(type) ((size_t)EAAlignof3<type>::value)

		#else
			// C implementation of EA_ALIGN_OF
			// This implementation works for most cases, but doesn't directly work 
			// for types such as function pointer declarations. To work with those
			// types you need to typedef the type and then use the typedef in EA_ALIGN_OF.
			#define EA_ALIGN_OF(type) ((size_t)offsetof(struct { char c; type m; }, m))
		#endif
	#endif

	// EA_PRAGMA_PACK_VC
	//
	// Wraps #pragma pack in a way that allows for cleaner code.
	// 
	// Example usage:
	//    EA_PRAGMA_PACK_VC(push, 1)
	//    struct X{ char c; int i; };
	//    EA_PRAGMA_PACK_VC(pop)
	//
	#if !defined(EA_PRAGMA_PACK_VC)
		#if defined(EA_COMPILER_MSVC)
			#define EA_PRAGMA_PACK_VC(...) __pragma(pack(__VA_ARGS__))
		#elif !defined(EA_COMPILER_NO_VARIADIC_MACROS)
			#define EA_PRAGMA_PACK_VC(...)
		#else
			// No support. However, all compilers of significance to us support variadic macros.
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_LIKELY / EA_UNLIKELY
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
	//    if(EA_LIKELY(a == 0)) // Tell the compiler that a will usually equal 0.
	//       { ... }
	//
	// Example usage:
	//    if(EA_UNLIKELY(a == 0)) // Tell the compiler that a will usually not equal 0.
	//       { ... }
	//
	#ifndef EA_LIKELY
		#if (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__clang__)
			#if defined(__cplusplus)
				#define EA_LIKELY(x)   __builtin_expect(!!(x), true)
				#define EA_UNLIKELY(x) __builtin_expect(!!(x), false) 
			#else
				#define EA_LIKELY(x)   __builtin_expect(!!(x), 1)
				#define EA_UNLIKELY(x) __builtin_expect(!!(x), 0) 
			#endif
		#else
			#define EA_LIKELY(x)   (x)
			#define EA_UNLIKELY(x) (x)
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_INIT_PRIORITY_AVAILABLE
	//
	// This value is either not defined, or defined to 1.
	// Defines if the GCC attribute init_priority is supported by the compiler.
	//
	#if !defined(EA_INIT_PRIORITY_AVAILABLE)
		#if   defined(__GNUC__) && !defined(__EDG__) // EDG typically #defines __GNUC__ but doesn't implement init_priority.
			#define EA_INIT_PRIORITY_AVAILABLE 1 
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_INIT_PRIORITY
	//
	// This is simply a wrapper for the GCC init_priority attribute that allows 
	// multiplatform code to be easier to read. This attribute doesn't apply
	// to VC++ because VC++ uses file-level pragmas to control init ordering.
	//
	// Example usage:
	//     SomeClass gSomeClass EA_INIT_PRIORITY(2000);
	//
	#if !defined(EA_INIT_PRIORITY)
		#if defined(EA_INIT_PRIORITY_AVAILABLE)
			#define EA_INIT_PRIORITY(x)  __attribute__ ((init_priority (x)))
		#else
			#define EA_INIT_PRIORITY(x)
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_MAY_ALIAS_AVAILABLE
	//
	// Defined as 0, 1, or 2.
	// Defines if the GCC attribute may_alias is supported by the compiler.
	// Consists of a value 0 (unsupported, shouldn't be used), 1 (some support), 
	// or 2 (full proper support). 
	//
	#ifndef EA_MAY_ALIAS_AVAILABLE
		#if defined(__GNUC__) && (((__GNUC__ * 100) + __GNUC_MINOR__) >= 303)
			#if   !defined(__EDG__)                 // define it as 1 while defining GCC's support as 2.
				#define EA_MAY_ALIAS_AVAILABLE 2
			#else
				#define EA_MAY_ALIAS_AVAILABLE 0    
			#endif                                  
		#else 
			#define EA_MAY_ALIAS_AVAILABLE 0
		#endif
	#endif


	// EA_MAY_ALIAS
	//
	// Defined as a macro that wraps the GCC may_alias attribute. This attribute
	// has no significance for VC++ because VC++ doesn't support the concept of 
	// strict aliasing. Users should avoid writing code that breaks strict 
	// aliasing rules; EA_MAY_ALIAS is for cases with no alternative.
	//
	// Example usage:
	//    void* EA_MAY_ALIAS gPtr = NULL;
	//
	// Example usage:
	//    typedef void* EA_MAY_ALIAS pvoid_may_alias;
	//    pvoid_may_alias gPtr = NULL;
	//
	#if EA_MAY_ALIAS_AVAILABLE
		#define EA_MAY_ALIAS __attribute__((__may_alias__))
	#else
		#define EA_MAY_ALIAS
	#endif


	// ------------------------------------------------------------------------
	// EA_ASSUME
	//
	// This acts the same as the VC++ __assume directive and is implemented 
	// simply as a wrapper around it to allow portable usage of it and to take
	// advantage of it if and when it appears in other compilers.
	//
	// Example usage:
	//    void Function(int a) {
	//       switch(a) {
	//          case 1:
	//             DoSomething(1);
	//             break;
	//          case 2:
	//             DoSomething(-1);
	//             break;
	//          default:
	//             EA_ASSUME(0); // This tells the optimizer that the default cannot be reached.
	//       }
	//    }
	//
	#ifndef EA_ASSUME
		#if defined(_MSC_VER) && (_MSC_VER >= 1300) // If VC7.0 and later
			#define EA_ASSUME(x) __assume(x)
		#else
			#define EA_ASSUME(x)
		#endif
	#endif



	// ------------------------------------------------------------------------
	// EA_ANALYSIS_ASSUME
	//
	// This acts the same as the VC++ __analysis_assume directive and is implemented 
	// simply as a wrapper around it to allow portable usage of it and to take
	// advantage of it if and when it appears in other compilers.
	//
	// Example usage:
	//    char Function(char* p) {
	//       EA_ANALYSIS_ASSUME(p != NULL);
	//       return *p;
	//    }
	//
	#ifndef EA_ANALYSIS_ASSUME
		#if defined(_MSC_VER) && (_MSC_VER >= 1300) // If VC7.0 and later 
			#define EA_ANALYSIS_ASSUME(x) __analysis_assume(!!(x)) // !! because that allows for convertible-to-bool in addition to bool.
		#else
			#define EA_ANALYSIS_ASSUME(x)
		#endif
	#endif



	// ------------------------------------------------------------------------
	// EA_DISABLE_VC_WARNING / EA_RESTORE_VC_WARNING
	// 
	// Disable and re-enable warning(s) within code.
	// This is simply a wrapper for VC++ #pragma warning(disable: nnnn) for the
	// purpose of making code easier to read due to avoiding nested compiler ifdefs
	// directly in code.
	//
	// Example usage:
	//     EA_DISABLE_VC_WARNING(4127 3244)
	//     <code>
	//     EA_RESTORE_VC_WARNING()
	//
	#ifndef EA_DISABLE_VC_WARNING
		#if defined(_MSC_VER)
			#define EA_DISABLE_VC_WARNING(w)  \
				__pragma(warning(push))       \
				__pragma(warning(disable:w))
		#else
			#define EA_DISABLE_VC_WARNING(w)
		#endif
	#endif

	#ifndef EA_RESTORE_VC_WARNING
		#if defined(_MSC_VER)
			#define EA_RESTORE_VC_WARNING()   \
				__pragma(warning(pop))
		#else
			#define EA_RESTORE_VC_WARNING()
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_ENABLE_VC_WARNING_AS_ERROR / EA_DISABLE_VC_WARNING_AS_ERROR
	//
	// Disable and re-enable treating a warning as error within code.
	// This is simply a wrapper for VC++ #pragma warning(error: nnnn) for the
	// purpose of making code easier to read due to avoiding nested compiler ifdefs
	// directly in code.
	//
	// Example usage:
	//     EA_ENABLE_VC_WARNING_AS_ERROR(4996)
	//     <code>
	//     EA_DISABLE_VC_WARNING_AS_ERROR()
	//
	#ifndef EA_ENABLE_VC_WARNING_AS_ERROR
		#if defined(_MSC_VER)
			#define EA_ENABLE_VC_WARNING_AS_ERROR(w) \
					__pragma(warning(push)) \
					__pragma(warning(error:w))
		#else
			#define EA_ENABLE_VC_WARNING_AS_ERROR(w)
		#endif
	#endif

	#ifndef EA_DISABLE_VC_WARNING_AS_ERROR
		#if defined(_MSC_VER)
			#define EA_DISABLE_VC_WARNING_AS_ERROR() \
				__pragma(warning(pop))
		#else
			#define EA_DISABLE_VC_WARNING_AS_ERROR()
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_DISABLE_GCC_WARNING / EA_RESTORE_GCC_WARNING
	//
	// Example usage:
	//     // Only one warning can be ignored per statement, due to how GCC works.
	//     EA_DISABLE_GCC_WARNING(-Wuninitialized)
	//     EA_DISABLE_GCC_WARNING(-Wunused)
	//     <code>
	//     EA_RESTORE_GCC_WARNING()
	//     EA_RESTORE_GCC_WARNING()
	//
	#ifndef EA_DISABLE_GCC_WARNING
		#if defined(EA_COMPILER_GNUC)
			#define EAGCCWHELP0(x) #x
			#define EAGCCWHELP1(x) EAGCCWHELP0(GCC diagnostic ignored x)
			#define EAGCCWHELP2(x) EAGCCWHELP1(#x)
		#endif

		#if defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4006) // Can't test directly for __GNUC__ because some compilers lie.
			#define EA_DISABLE_GCC_WARNING(w)   \
				_Pragma("GCC diagnostic push")  \
				_Pragma(EAGCCWHELP2(w))
		#elif defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4004)
			#define EA_DISABLE_GCC_WARNING(w)   \
				_Pragma(EAGCCWHELP2(w))
		#else
			#define EA_DISABLE_GCC_WARNING(w)
		#endif
	#endif

	#ifndef EA_RESTORE_GCC_WARNING
		#if defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4006)
			#define EA_RESTORE_GCC_WARNING()    \
				_Pragma("GCC diagnostic pop")
		#else
			#define EA_RESTORE_GCC_WARNING()
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_DISABLE_ALL_GCC_WARNINGS / EA_RESTORE_ALL_GCC_WARNINGS
	//
	// This isn't possible except via using _Pragma("GCC system_header"), though
	// that has some limitations in how it works. Another means is to manually
	// disable individual warnings within a GCC diagnostic push statement.
	// GCC doesn't have as many warnings as VC++ and EDG and so this may be feasible.
	// ------------------------------------------------------------------------


	// ------------------------------------------------------------------------
	// EA_ENABLE_GCC_WARNING_AS_ERROR / EA_DISABLE_GCC_WARNING_AS_ERROR
	//
	// Example usage:
	//     // Only one warning can be treated as an error per statement, due to how GCC works.
	//     EA_ENABLE_GCC_WARNING_AS_ERROR(-Wuninitialized)
	//     EA_ENABLE_GCC_WARNING_AS_ERROR(-Wunused)
	//     <code>
	//     EA_DISABLE_GCC_WARNING_AS_ERROR()
	//     EA_DISABLE_GCC_WARNING_AS_ERROR()
	//
	#ifndef EA_ENABLE_GCC_WARNING_AS_ERROR
		#if defined(EA_COMPILER_GNUC)
			#define EAGCCWERRORHELP0(x) #x
			#define EAGCCWERRORHELP1(x) EAGCCWERRORHELP0(GCC diagnostic error x)
			#define EAGCCWERRORHELP2(x) EAGCCWERRORHELP1(#x)
		#endif

		#if defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4006) // Can't test directly for __GNUC__ because some compilers lie.
			#define EA_ENABLE_GCC_WARNING_AS_ERROR(w)   \
				_Pragma("GCC diagnostic push")  \
				_Pragma(EAGCCWERRORHELP2(w))
		#elif defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4004)
			#define EA_DISABLE_GCC_WARNING(w)   \
				_Pragma(EAGCCWERRORHELP2(w))
		#else
			#define EA_DISABLE_GCC_WARNING(w)
		#endif
	#endif

	#ifndef EA_DISABLE_GCC_WARNING_AS_ERROR
		#if defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4006)
			#define EA_DISABLE_GCC_WARNING_AS_ERROR()    \
				_Pragma("GCC diagnostic pop")
		#else
			#define EA_DISABLE_GCC_WARNING_AS_ERROR()
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_DISABLE_CLANG_WARNING / EA_RESTORE_CLANG_WARNING
	//
	// Example usage:
	//     // Only one warning can be ignored per statement, due to how clang works.
	//     EA_DISABLE_CLANG_WARNING(-Wuninitialized)
	//     EA_DISABLE_CLANG_WARNING(-Wunused)
	//     <code>
	//     EA_RESTORE_CLANG_WARNING()
	//     EA_RESTORE_CLANG_WARNING()
	//
	#ifndef EA_DISABLE_CLANG_WARNING
		#if defined(EA_COMPILER_CLANG)
			#define EACLANGWHELP0(x) #x
			#define EACLANGWHELP1(x) EACLANGWHELP0(clang diagnostic ignored x)
			#define EACLANGWHELP2(x) EACLANGWHELP1(#x)

			#define EA_DISABLE_CLANG_WARNING(w)   \
				_Pragma("clang diagnostic push")  \
				_Pragma(EACLANGWHELP2(w))
		#else
			#define EA_DISABLE_CLANG_WARNING(w)
		#endif
	#endif

	#ifndef EA_RESTORE_CLANG_WARNING
		#if defined(EA_COMPILER_CLANG)
			#define EA_RESTORE_CLANG_WARNING()    \
				_Pragma("clang diagnostic pop")
		#else
			#define EA_RESTORE_CLANG_WARNING()
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_DISABLE_ALL_CLANG_WARNINGS / EA_RESTORE_ALL_CLANG_WARNINGS
	//
	// The situation for clang is the same as for GCC. See above.
	// ------------------------------------------------------------------------


	// ------------------------------------------------------------------------
	// EA_ENABLE_CLANG_WARNING_AS_ERROR / EA_DISABLE_CLANG_WARNING_AS_ERROR
	//
	// Example usage:
	//     // Only one warning can be treated as an error per statement, due to how clang works.
	//     EA_ENABLE_CLANG_WARNING_AS_ERROR(-Wuninitialized)
	//     EA_ENABLE_CLANG_WARNING_AS_ERROR(-Wunused)
	//     <code>
	//     EA_DISABLE_CLANG_WARNING_AS_ERROR()
	//     EA_DISABLE_CLANG_WARNING_AS_ERROR()
	//
	#ifndef EA_ENABLE_CLANG_WARNING_AS_ERROR
		#if defined(EA_COMPILER_CLANG)
			#define EACLANGWERRORHELP0(x) #x
			#define EACLANGWERRORHELP1(x) EACLANGWERRORHELP0(clang diagnostic error x)
			#define EACLANGWERRORHELP2(x) EACLANGWERRORHELP1(#x)

			#define EA_ENABLE_CLANG_WARNING_AS_ERROR(w)   \
				_Pragma("clang diagnostic push")  \
				_Pragma(EACLANGWERRORHELP2(w))
		#else
			#define EA_DISABLE_CLANG_WARNING(w)
		#endif
	#endif

	#ifndef EA_DISABLE_CLANG_WARNING_AS_ERROR
		#if defined(EA_COMPILER_CLANG)
			#define EA_DISABLE_CLANG_WARNING_AS_ERROR()    \
				_Pragma("clang diagnostic pop")
		#else
			#define EA_DISABLE_CLANG_WARNING_AS_ERROR()
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_DISABLE_SN_WARNING / EA_RESTORE_SN_WARNING
	//
	// Note that we define this macro specifically for the SN compiler instead of
	// having a generic one for EDG-based compilers. The reason for this is that
	// while SN is indeed based on EDG, SN has different warning value mappings
	// and thus warning 1234 for SN is not the same as 1234 for all other EDG compilers.
	//
	// Example usage:
	//     // Currently we are limited to one warning per line.
	//     EA_DISABLE_SN_WARNING(1787)
	//     EA_DISABLE_SN_WARNING(552)
	//     <code>
	//     EA_RESTORE_SN_WARNING()
	//     EA_RESTORE_SN_WARNING()
	//
	#ifndef EA_DISABLE_SN_WARNING
			#define EA_DISABLE_SN_WARNING(w)
	#endif

	#ifndef EA_RESTORE_SN_WARNING
			#define EA_RESTORE_SN_WARNING()
	#endif


	// ------------------------------------------------------------------------
	// EA_DISABLE_ALL_SN_WARNINGS / EA_RESTORE_ALL_SN_WARNINGS
	//
	// Example usage:
	//     EA_DISABLE_ALL_SN_WARNINGS()
	//     <code>
	//     EA_RESTORE_ALL_SN_WARNINGS()
	//
	#ifndef EA_DISABLE_ALL_SN_WARNINGS
			#define EA_DISABLE_ALL_SN_WARNINGS()
	#endif

	#ifndef EA_RESTORE_ALL_SN_WARNINGS
			#define EA_RESTORE_ALL_SN_WARNINGS()
	#endif



	// ------------------------------------------------------------------------
	// EA_DISABLE_GHS_WARNING / EA_RESTORE_GHS_WARNING
	//
	// Disable warnings from the Green Hills compiler.
	//
	// Example usage:
	//     EA_DISABLE_GHS_WARNING(193)
	//     EA_DISABLE_GHS_WARNING(236, 5323)
	//     <code>
	//     EA_RESTORE_GHS_WARNING()
	//     EA_RESTORE_GHS_WARNING()
	//
	#ifndef EA_DISABLE_GHS_WARNING
			#define EA_DISABLE_GHS_WARNING(w)
	#endif

	#ifndef EA_RESTORE_GHS_WARNING
			#define EA_RESTORE_GHS_WARNING()
	#endif


	// ------------------------------------------------------------------------
	// EA_DISABLE_ALL_GHS_WARNINGS / EA_RESTORE_ALL_GHS_WARNINGS
	//
	// #ifndef EA_DISABLE_ALL_GHS_WARNINGS
	//     #if defined(EA_COMPILER_GREEN_HILLS)
	//         #define EA_DISABLE_ALL_GHS_WARNINGS(w)  \_ 
	//             _Pragma("_________")
	//     #else
	//         #define EA_DISABLE_ALL_GHS_WARNINGS(w)
	//     #endif
	// #endif
	// 
	// #ifndef EA_RESTORE_ALL_GHS_WARNINGS
	//     #if defined(EA_COMPILER_GREEN_HILLS)
	//         #define EA_RESTORE_ALL_GHS_WARNINGS()   \_
	//             _Pragma("_________")
	//     #else
	//         #define EA_RESTORE_ALL_GHS_WARNINGS()
	//     #endif
	// #endif



	// ------------------------------------------------------------------------
	// EA_DISABLE_EDG_WARNING / EA_RESTORE_EDG_WARNING
	//
	// Example usage:
	//     // Currently we are limited to one warning per line.
	//     EA_DISABLE_EDG_WARNING(193)
	//     EA_DISABLE_EDG_WARNING(236)
	//     <code>
	//     EA_RESTORE_EDG_WARNING()
	//     EA_RESTORE_EDG_WARNING()
	//
	#ifndef EA_DISABLE_EDG_WARNING
		// EDG-based compilers are inconsistent in how the implement warning pragmas.
		#if defined(EA_COMPILER_EDG) && !defined(EA_COMPILER_INTEL) && !defined(EA_COMPILER_RVCT)
			#define EAEDGWHELP0(x) #x
			#define EAEDGWHELP1(x) EAEDGWHELP0(diag_suppress x)

			#define EA_DISABLE_EDG_WARNING(w)   \
				_Pragma("control %push diag")   \
				_Pragma(EAEDGWHELP1(w))
		#else
			#define EA_DISABLE_EDG_WARNING(w)
		#endif
	#endif

	#ifndef EA_RESTORE_EDG_WARNING
		#if defined(EA_COMPILER_EDG) && !defined(EA_COMPILER_INTEL) && !defined(EA_COMPILER_RVCT)
			#define EA_RESTORE_EDG_WARNING()   \
				_Pragma("control %pop diag")
		#else
			#define EA_RESTORE_EDG_WARNING()
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_DISABLE_ALL_EDG_WARNINGS / EA_RESTORE_ALL_EDG_WARNINGS
	//
	//#ifndef EA_DISABLE_ALL_EDG_WARNINGS
	//    #if defined(EA_COMPILER_EDG) && !defined(EA_COMPILER_SN)
	//        #define EA_DISABLE_ALL_EDG_WARNINGS(w)  \_ 
	//            _Pragma("_________")
	//    #else
	//        #define EA_DISABLE_ALL_EDG_WARNINGS(w)
	//    #endif
	//#endif
	//
	//#ifndef EA_RESTORE_ALL_EDG_WARNINGS
	//    #if defined(EA_COMPILER_EDG) && !defined(EA_COMPILER_SN)
	//        #define EA_RESTORE_ALL_EDG_WARNINGS()   \_
	//            _Pragma("_________")
	//    #else
	//        #define EA_RESTORE_ALL_EDG_WARNINGS()
	//    #endif
	//#endif



	// ------------------------------------------------------------------------
	// EA_DISABLE_CW_WARNING / EA_RESTORE_CW_WARNING
	//
	// Note that this macro can only control warnings via numbers and not by 
	// names. The reason for this is that the compiler's syntax for such 
	// warnings is not the same as for numbers.
	// 
	// Example usage:
	//     // Currently we are limited to one warning per line and must also specify the warning in the restore macro.
	//     EA_DISABLE_CW_WARNING(10317)
	//     EA_DISABLE_CW_WARNING(10324)
	//     <code>
	//     EA_RESTORE_CW_WARNING(10317)
	//     EA_RESTORE_CW_WARNING(10324)
	//
	#ifndef EA_DISABLE_CW_WARNING
		#define EA_DISABLE_CW_WARNING(w)
	#endif

	#ifndef EA_RESTORE_CW_WARNING

		#define EA_RESTORE_CW_WARNING(w)

	#endif


	// ------------------------------------------------------------------------
	// EA_DISABLE_ALL_CW_WARNINGS / EA_RESTORE_ALL_CW_WARNINGS
	//
	#ifndef EA_DISABLE_ALL_CW_WARNINGS
		#define EA_DISABLE_ALL_CW_WARNINGS()

	#endif
	
	#ifndef EA_RESTORE_ALL_CW_WARNINGS
		#define EA_RESTORE_ALL_CW_WARNINGS()
	#endif



	// ------------------------------------------------------------------------
	// EA_PURE
	// 
	// This acts the same as the GCC __attribute__ ((pure)) directive and is
	// implemented simply as a wrapper around it to allow portable usage of 
	// it and to take advantage of it if and when it appears in other compilers.
	//
	// A "pure" function is one that has no effects except its return value and 
	// its return value is a function of only the function's parameters or 
	// non-volatile global variables. Any parameter or global variable access 
	// must be read-only. Loop optimization and subexpression elimination can be 
	// applied to such functions. A common example is strlen(): Given identical 
	// inputs, the function's return value (its only effect) is invariant across 
	// multiple invocations and thus can be pulled out of a loop and called but once.
	//
	// Example usage:
	//    EA_PURE void Function();
	//
	#ifndef EA_PURE
		#if defined(EA_COMPILER_GNUC)
			#define EA_PURE __attribute__((pure))
		#elif defined(EA_COMPILER_ARM)  // Arm brand compiler for ARM CPU
			#define EA_PURE __pure
		#else
			#define EA_PURE
		#endif
	#endif



	// ------------------------------------------------------------------------
	// EA_WEAK
	// EA_WEAK_SUPPORTED -- defined as 0 or 1.
	// 
	// GCC
	// The weak attribute causes the declaration to be emitted as a weak
	// symbol rather than a global. This is primarily useful in defining
	// library functions which can be overridden in user code, though it
	// can also be used with non-function declarations.
	//
	// VC++
	// At link time, if multiple definitions of a COMDAT are seen, the linker 
	// picks one and discards the rest. If the linker option /OPT:REF 
	// is selected, then COMDAT elimination will occur to remove all the 
	// unreferenced data items in the linker output.
	//
	// Example usage:
	//    EA_WEAK void Function();
	//
	#ifndef EA_WEAK
		#if defined(_MSC_VER) && (_MSC_VER >= 1300) // If VC7.0 and later 
			#define EA_WEAK __declspec(selectany)
			#define EA_WEAK_SUPPORTED 1
		#elif defined(_MSC_VER) || (defined(__GNUC__) && defined(__CYGWIN__))
			#define EA_WEAK
			#define EA_WEAK_SUPPORTED 0
		#elif defined(EA_COMPILER_ARM)  // Arm brand compiler for ARM CPU
			#define EA_WEAK __weak
			#define EA_WEAK_SUPPORTED 1
		#else                           // GCC and IBM compilers, others.
			#define EA_WEAK __attribute__((weak))
			#define EA_WEAK_SUPPORTED 1
		#endif
	#endif



	// ------------------------------------------------------------------------
	// EA_UNUSED
	// 
	// Makes compiler warnings about unused variables go away.
	//
	// Example usage:
	//    void Function(int x)
	//    {
	//        int y;
	//        EA_UNUSED(x);
	//        EA_UNUSED(y);
	//    }
	//
	#ifndef EA_UNUSED
		// The EDG solution below is pretty weak and needs to be augmented or replaced.
		// It can't handle the C language, is limited to places where template declarations
		// can be used, and requires the type x to be usable as a functions reference argument. 
		#if defined(__cplusplus) && defined(__EDG__)
			template <typename T>
			inline void EABaseUnused(T const volatile & x) { (void)x; }
			#define EA_UNUSED(x) EABaseUnused(x)
		#else
			#define EA_UNUSED(x) (void)x
		#endif
	#endif



	// ------------------------------------------------------------------------
	// EA_EMPTY
	// 
	// Allows for a null statement, usually for the purpose of avoiding compiler warnings.
	//
	// Example usage:
	//    #ifdef EA_DEBUG
	//        #define MyDebugPrintf(x, y) printf(x, y)
	//    #else
	//        #define MyDebugPrintf(x, y)  EA_EMPTY
	//    #endif
	//
	#ifndef EA_EMPTY
		#define EA_EMPTY (void)0
	#endif



	// ------------------------------------------------------------------------
	// wchar_t
	// Here we define:
	//    EA_WCHAR_T_NON_NATIVE
	//    EA_WCHAR_SIZE = <sizeof(wchar_t)>
	//
	#ifndef EA_WCHAR_T_NON_NATIVE
		// Compilers that always implement wchar_t as native include:
		//     COMEAU, new SN, and other EDG-based compilers.
		//     GCC
		//     Borland
		//     SunPro
		//     IBM Visual Age
		#if defined(EA_COMPILER_INTEL)
			#if (EA_COMPILER_VERSION < 700)
				#define EA_WCHAR_T_NON_NATIVE 1
			#else
				#if (!defined(_WCHAR_T_DEFINED) && !defined(_WCHAR_T))
					#define EA_WCHAR_T_NON_NATIVE 1
				#endif
			#endif
		#elif defined(EA_COMPILER_MSVC) || defined(EA_COMPILER_BORLAND) || (defined(EA_COMPILER_CLANG) && defined(EA_PLATFORM_WINDOWS))
			#ifndef _NATIVE_WCHAR_T_DEFINED
				#define EA_WCHAR_T_NON_NATIVE 1
			#endif
		#elif defined(__EDG_VERSION__) && (!defined(_WCHAR_T) && (__EDG_VERSION__ < 400)) // EDG prior to v4 uses _WCHAR_T to indicate if wchar_t is native. v4+ may define something else, but we're not currently aware of it.
			#define EA_WCHAR_T_NON_NATIVE 1
		#endif
	#endif

	#ifndef EA_WCHAR_SIZE // If the user hasn't specified that it is a given size...
		#if defined(__WCHAR_MAX__) // GCC defines this for most platforms.
			#if (__WCHAR_MAX__ == 2147483647) || (__WCHAR_MAX__ == 4294967295)
				#define EA_WCHAR_SIZE 4
			#elif (__WCHAR_MAX__ == 32767) || (__WCHAR_MAX__ == 65535)
				#define EA_WCHAR_SIZE 2
			#elif (__WCHAR_MAX__ == 127) || (__WCHAR_MAX__ == 255)
				#define EA_WCHAR_SIZE 1
			#else
				#define EA_WCHAR_SIZE 4
			#endif
		#elif defined(WCHAR_MAX) // The SN and Arm compilers define this.
			#if (WCHAR_MAX == 2147483647) || (WCHAR_MAX == 4294967295)
				#define EA_WCHAR_SIZE 4
			#elif (WCHAR_MAX == 32767) || (WCHAR_MAX == 65535)
				#define EA_WCHAR_SIZE 2
			#elif (WCHAR_MAX == 127) || (WCHAR_MAX == 255)
				#define EA_WCHAR_SIZE 1
			#else
				#define EA_WCHAR_SIZE 4
			#endif
		#elif defined(__WCHAR_BIT) // Green Hills (and other versions of EDG?) uses this.
			#if (__WCHAR_BIT == 16)
				#define EA_WCHAR_SIZE 2
			#elif (__WCHAR_BIT == 32)
				#define EA_WCHAR_SIZE 4
			#elif (__WCHAR_BIT == 8)
				#define EA_WCHAR_SIZE 1
			#else
				#define EA_WCHAR_SIZE 4
			#endif
		#elif defined(_WCMAX) // The SN and Arm compilers define this.
			#if (_WCMAX == 2147483647) || (_WCMAX == 4294967295)
				#define EA_WCHAR_SIZE 4
			#elif (_WCMAX == 32767) || (_WCMAX == 65535)
				#define EA_WCHAR_SIZE 2
			#elif (_WCMAX == 127) || (_WCMAX == 255)
				#define EA_WCHAR_SIZE 1
			#else
				#define EA_WCHAR_SIZE 4
			#endif
		#elif defined(EA_PLATFORM_UNIX)
			// It is standard on Unix to have wchar_t be int32_t or uint32_t.
			// All versions of GNUC default to a 32 bit wchar_t, but EA has used 
			// the -fshort-wchar GCC command line option to force it to 16 bit.
			// If you know that the compiler is set to use a wchar_t of other than 
			// the default, you need to manually define EA_WCHAR_SIZE for the build.
			#define EA_WCHAR_SIZE 4
		#else
			// It is standard on Windows to have wchar_t be uint16_t.  GCC
			// defines wchar_t as int by default.  Electronic Arts has
			// standardized on wchar_t being an unsigned 16 bit value on all
			// console platforms. Given that there is currently no known way to
			// tell at preprocessor time what the size of wchar_t is, we declare
			// it to be 2, as this is the Electronic Arts standard. If you have
			// EA_WCHAR_SIZE != sizeof(wchar_t), then your code might not be
			// broken, but it also won't work with wchar libraries and data from
			// other parts of EA. Under GCC, you can force wchar_t to two bytes
			// with the -fshort-wchar compiler argument.
			#define EA_WCHAR_SIZE 2
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_RESTRICT
	// 
	// The C99 standard defines a new keyword, restrict, which allows for the 
	// improvement of code generation regarding memory usage. Compilers can
	// generate significantly faster code when you are able to use restrict.
	// 
	// Example usage:
	//    void DoSomething(char* EA_RESTRICT p1, char* EA_RESTRICT p2);
	//
	#ifndef EA_RESTRICT
		#if defined(EA_COMPILER_MSVC) && (EA_COMPILER_VERSION >= 1400) // If VC8 (VS2005) or later...
			#define EA_RESTRICT __restrict
		#elif defined(EA_COMPILER_CLANG)
			#define EA_RESTRICT __restrict
		#elif defined(EA_COMPILER_GNUC)     // Includes GCC and other compilers emulating GCC.
			#define EA_RESTRICT __restrict  // GCC defines 'restrict' (as opposed to __restrict) in C99 mode only.
		#elif defined(EA_COMPILER_ARM)
			#define EA_RESTRICT __restrict
		#elif defined(EA_COMPILER_IS_C99)
			#define EA_RESTRICT restrict
		#else
			// If the compiler didn't support restricted pointers, defining EA_RESTRICT 
			// away would result in compiling and running fine but you just wouldn't 
			// the same level of optimization. On the other hand, all the major compilers 
			// support restricted pointers.
			#define EA_RESTRICT
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_DEPRECATED            // Used as a prefix.
	// EA_PREFIX_DEPRECATED     // You should need this only for unusual compilers.
	// EA_POSTFIX_DEPRECATED    // You should need this only for unusual compilers.
	// EA_DEPRECATED_MESSAGE    // Used as a prefix and provides a deprecation message.
	// 
	// Example usage:
	//    EA_DEPRECATED void Function();
	//    EA_DEPRECATED_MESSAGE("Use 1.0v API instead") void Function();
	//
	// or for maximum portability:
	//    EA_PREFIX_DEPRECATED void Function() EA_POSTFIX_DEPRECATED;
	//

	#ifndef EA_DEPRECATED
		#if defined(EA_COMPILER_CPP14_ENABLED)
			#define EA_DEPRECATED [[deprecated]]
		#elif defined(EA_COMPILER_MSVC) && (EA_COMPILER_VERSION > 1300) // If VC7 (VS2003) or later...
			#define EA_DEPRECATED __declspec(deprecated)
		#elif defined(EA_COMPILER_MSVC)
			#define EA_DEPRECATED 
		#else
			#define EA_DEPRECATED __attribute__((deprecated))
		#endif
	#endif

	#ifndef EA_PREFIX_DEPRECATED
		#if defined(EA_COMPILER_CPP14_ENABLED)
			#define EA_PREFIX_DEPRECATED [[deprecated]]
			#define EA_POSTFIX_DEPRECATED
		#elif defined(EA_COMPILER_MSVC) && (EA_COMPILER_VERSION > 1300) // If VC7 (VS2003) or later...
			#define EA_PREFIX_DEPRECATED __declspec(deprecated)
			#define EA_POSTFIX_DEPRECATED
		#elif defined(EA_COMPILER_MSVC)
			#define EA_PREFIX_DEPRECATED
			#define EA_POSTFIX_DEPRECATED
		#else
			#define EA_PREFIX_DEPRECATED
			#define EA_POSTFIX_DEPRECATED __attribute__((deprecated))
		#endif
	#endif

	#ifndef EA_DEPRECATED_MESSAGE
		#if defined(EA_COMPILER_CPP14_ENABLED)
			#define EA_DEPRECATED_MESSAGE(msg) [[deprecated(#msg)]]
		#else
			// Compiler does not support depreaction messages, explicitly drop the msg but still mark the function as deprecated
			#define EA_DEPRECATED_MESSAGE(msg) EA_DEPRECATED
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_FORCE_INLINE              // Used as a prefix.
	// EA_PREFIX_FORCE_INLINE       // You should need this only for unusual compilers.
	// EA_POSTFIX_FORCE_INLINE      // You should need this only for unusual compilers.
	//
	// Example usage:
	//     EA_FORCE_INLINE void Foo();                                // Implementation elsewhere.
	//     EA_PREFIX_FORCE_INLINE void Foo() EA_POSTFIX_FORCE_INLINE; // Implementation elsewhere.
	//
	// Note that when the prefix version of this function is used, it replaces
	// the regular C++ 'inline' statement. Thus you should not use both the 
	// C++ inline statement and this macro with the same function declaration.
	//
	// To force inline usage under GCC 3.1+, you use this:
	//    inline void Foo() __attribute__((always_inline));
	//       or
	//    inline __attribute__((always_inline)) void Foo();
	//
	// The CodeWarrior compiler doesn't have the concept of forcing inlining per function.
	// 
	#ifndef EA_FORCE_INLINE
		#if defined(EA_COMPILER_MSVC)
			#define EA_FORCE_INLINE __forceinline
		#elif defined(EA_COMPILER_GNUC) && (((__GNUC__ * 100) + __GNUC_MINOR__) >= 301) || defined(EA_COMPILER_CLANG)
			#if defined(__cplusplus)
				#define EA_FORCE_INLINE inline __attribute__((always_inline))
			#else
				#define EA_FORCE_INLINE __inline__ __attribute__((always_inline))
			#endif
		#else
			#if defined(__cplusplus)
				#define EA_FORCE_INLINE inline
			#else
				#define EA_FORCE_INLINE __inline
			#endif
		#endif
	#endif

	#if   defined(EA_COMPILER_GNUC) && (((__GNUC__ * 100) + __GNUC_MINOR__) >= 301) || defined(EA_COMPILER_CLANG)
		#define EA_PREFIX_FORCE_INLINE  inline
		#define EA_POSTFIX_FORCE_INLINE __attribute__((always_inline))
	#else
		#define EA_PREFIX_FORCE_INLINE  inline
		#define EA_POSTFIX_FORCE_INLINE 
	#endif


	// ------------------------------------------------------------------------
	// EA_NO_INLINE             // Used as a prefix. 
	// EA_PREFIX_NO_INLINE      // You should need this only for unusual compilers.
	// EA_POSTFIX_NO_INLINE     // You should need this only for unusual compilers.
	//
	// Example usage:
	//     EA_NO_INLINE        void Foo();                       // Implementation elsewhere.
	//     EA_PREFIX_NO_INLINE void Foo() EA_POSTFIX_NO_INLINE;  // Implementation elsewhere.
	//
	// That this declaration is incompatbile with C++ 'inline' and any
	// variant of EA_FORCE_INLINE.
	//
	// To disable inline usage under VC++ priof to VS2005, you need to use this:
	//    #pragma inline_depth(0) // Disable inlining.
	//    void Foo() { ... }
	//    #pragma inline_depth()  // Restore to default.
	//
	// Since there is no easy way to disable inlining on a function-by-function
	// basis in VC++ prior to VS2005, the best strategy is to write platform-specific 
	// #ifdefs in the code or to disable inlining for a given module and enable 
	// functions individually with EA_FORCE_INLINE. 
	// 
	#ifndef EA_NO_INLINE
		#if defined(EA_COMPILER_MSVC) && (EA_COMPILER_VERSION >= 1400) // If VC8 (VS2005) or later...
			#define EA_NO_INLINE __declspec(noinline)
		#elif defined(EA_COMPILER_MSVC)
			#define EA_NO_INLINE
		#else
			#define EA_NO_INLINE __attribute__((noinline))
		#endif
	#endif

	#if defined(EA_COMPILER_MSVC) && (EA_COMPILER_VERSION >= 1400) // If VC8 (VS2005) or later...
		#define EA_PREFIX_NO_INLINE  __declspec(noinline)
		#define EA_POSTFIX_NO_INLINE
	#elif defined(EA_COMPILER_MSVC)
		#define EA_PREFIX_NO_INLINE
		#define EA_POSTFIX_NO_INLINE
	#else
		#define EA_PREFIX_NO_INLINE
		#define EA_POSTFIX_NO_INLINE __attribute__((noinline))
	#endif


	// ------------------------------------------------------------------------
	// EA_NO_VTABLE
	//
	// Example usage:
	//     class EA_NO_VTABLE X {
	//        virtual void InterfaceFunction();
	//     };
	// 
	//     EA_CLASS_NO_VTABLE(X) {
	//        virtual void InterfaceFunction();
	//     };
	//
	#ifdef EA_COMPILER_MSVC
		#define EA_NO_VTABLE           __declspec(novtable)
		#define EA_CLASS_NO_VTABLE(x)  class __declspec(novtable) x
		#define EA_STRUCT_NO_VTABLE(x) struct __declspec(novtable) x
	#else
		#define EA_NO_VTABLE
		#define EA_CLASS_NO_VTABLE(x)  class x
		#define EA_STRUCT_NO_VTABLE(x) struct x
	#endif


	// ------------------------------------------------------------------------
	// EA_PASCAL
	//
	// Also known on PC platforms as stdcall.
	// This convention causes the compiler to assume that the called function 
	// will pop off the stack space used to pass arguments, unless it takes a 
	// variable number of arguments. 
	//
	// Example usage:
	//    this:
	//       void DoNothing(int x);
	//       void DoNothing(int x){}
	//    would be written as this:
	//       void EA_PASCAL_FUNC(DoNothing(int x));
	//       void EA_PASCAL_FUNC(DoNothing(int x)){}
	// 
	#ifndef EA_PASCAL
		#if defined(EA_COMPILER_MSVC)
			#define EA_PASCAL __stdcall
		#elif defined(EA_COMPILER_GNUC) && defined(EA_PROCESSOR_X86)
			#define EA_PASCAL __attribute__((stdcall))
		#else
			// Some compilers simply don't support pascal calling convention.
			// As a result, there isn't an issue here, since the specification of 
			// pascal calling convention is for the purpose of disambiguating the
			// calling convention that is applied.
			#define EA_PASCAL
		#endif
	#endif

	#ifndef EA_PASCAL_FUNC
		#if defined(EA_COMPILER_MSVC)
			#define EA_PASCAL_FUNC(funcname_and_paramlist)    __stdcall funcname_and_paramlist
		#elif defined(EA_COMPILER_GNUC) && defined(EA_PROCESSOR_X86)
			#define EA_PASCAL_FUNC(funcname_and_paramlist)    __attribute__((stdcall)) funcname_and_paramlist
		#else
			#define EA_PASCAL_FUNC(funcname_and_paramlist)    funcname_and_paramlist
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_SSE
	// Visual C Processor Packs define _MSC_FULL_VER and are needed for SSE
	// Intel C also has SSE support.
	// EA_SSE is used to select FPU or SSE versions in hw_select.inl
	//
	// EA_SSE defines the level of SSE support:
	//  0 indicates no SSE support
	//  1 indicates SSE1 is supported
	//  2 indicates SSE2 is supported
	//  3 indicates SSE3 (or greater) is supported
	//
	// Note: SSE support beyond SSE3 can't be properly represented as a single
	// version number.  Instead users should use specific SSE defines (e.g.
	// EA_SSE4_2) to detect what specific support is available.  EA_SSE being
	// equal to 3 really only indicates that SSE3 or greater is supported.
	#ifndef EA_SSE
		#if defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)
			#if defined(__SSE3__)
				#define EA_SSE 3
			#elif defined(__SSE2__)
				#define EA_SSE 2
			#elif defined(__SSE__) && __SSE__
				#define EA_SSE 1
			#else
				#define EA_SSE 0
			#endif
		#elif (defined(EA_SSE3) && EA_SSE3) || defined EA_PLATFORM_CAPILANO
			#define EA_SSE 3
		#elif defined(EA_SSE2) && EA_SSE2
			#define EA_SSE 2
		#elif defined(EA_PROCESSOR_X86) && defined(_MSC_FULL_VER) && !defined(__NOSSE__) && defined(_M_IX86_FP)
			#define EA_SSE _M_IX86_FP 
		#elif defined(EA_PROCESSOR_X86) && defined(EA_COMPILER_INTEL) && !defined(__NOSSE__)
			#define EA_SSE 1
		#elif defined(EA_PROCESSOR_X86_64)
			// All x64 processors support SSE2 or higher
			#define EA_SSE 2
		#else
			#define EA_SSE 0
		#endif
	#endif

	// ------------------------------------------------------------------------
	// We define separate defines for SSE support beyond SSE1.  These defines
	// are particularly useful for detecting SSE4.x features since there isn't
	// a single concept of SSE4.
	//
	// The following SSE defines are always defined.  0 indicates the
	// feature/level of SSE is not supported, and 1 indicates support is
	// available.
	#ifndef EA_SSE2
		#if EA_SSE >= 2
			#define EA_SSE2 1
		#else
			#define EA_SSE2 0
		#endif
	#endif
	#ifndef EA_SSE3
		#if EA_SSE >= 3
			#define EA_SSE3 1
		#else
			#define EA_SSE3 0
		#endif
	#endif
	#ifndef EA_SSSE3
		#if defined __SSSE3__ || defined EA_PLATFORM_CAPILANO
			#define EA_SSSE3 1
		#else
			#define EA_SSSE3 0
		#endif
	#endif
	#ifndef EA_SSE4_1
		#if defined __SSE4_1__ || defined EA_PLATFORM_CAPILANO
			#define EA_SSE4_1 1
		#else
			#define EA_SSE4_1 0
		#endif
	#endif
	#ifndef EA_SSE4_2
		#if defined __SSE4_2__ || defined EA_PLATFORM_CAPILANO
			#define EA_SSE4_2 1
		#else
			#define EA_SSE4_2 0
		#endif
	#endif
	#ifndef EA_SSE4A
		#if defined __SSE4A__ || defined EA_PLATFORM_CAPILANO
			#define EA_SSE4A 1
		#else
			#define EA_SSE4A 0
		#endif
	#endif

	// ------------------------------------------------------------------------
	// EA_AVX
	// EA_AVX may be used to determine if Advanced Vector Extensions are available for the target architecture
	//
	// EA_AVX defines the level of AVX support:
	//  0 indicates no AVX support
	//  1 indicates AVX1 is supported
	//  2 indicates AVX2 is supported
	#ifndef EA_AVX
		#if defined __AVX2__
			#define EA_AVX 2
		#elif defined __AVX__ || defined EA_PLATFORM_CAPILANO
			#define EA_AVX 1
		#else
			#define EA_AVX 0
		#endif
	#endif
	#ifndef EA_AVX2
		#if EA_AVX >= 2
			#define EA_AVX2 1
		#else
			#define EA_AVX2 0
		#endif
	#endif

	// EA_FP16C may be used to determine the existence of float <-> half conversion operations on an x86 CPU.
	// (For example to determine if _mm_cvtph_ps or _mm_cvtps_ph could be used.)
	#ifndef EA_FP16C
		#if defined __F16C__ || defined EA_PLATFORM_CAPILANO
			#define EA_FP16C 1
		#else
			#define EA_FP16C 0
		#endif
	#endif

	// EA_FP128 may be used to determine if __float128 is a supported type for use. This type is enabled by a GCC extension (_GLIBCXX_USE_FLOAT128)
	// but has support by some implementations of clang (__FLOAT128__)
	// PS4 does not support __float128 as of SDK 5.500 https://ps4.siedev.net/resources/documents/SDK/5.500/CPU_Compiler_ABI-Overview/0003.html
	#ifndef EA_FP128
		#if (defined __FLOAT128__ || defined _GLIBCXX_USE_FLOAT128) && !defined(EA_PLATFORM_KETTLE)
			#define EA_FP128 1
		#else
			#define EA_FP128 0
		#endif
	#endif

	// ------------------------------------------------------------------------
	// EA_ABM
	// EA_ABM may be used to determine if Advanced Bit Manipulation sets are available for the target architecture (POPCNT, LZCNT)
	#ifndef EA_ABM
		#if defined(__ABM__) || defined(EA_PLATFORM_CAPILANO)
			#define EA_ABM 1
		#else
			#define EA_ABM 0
		#endif
	#endif

	// ------------------------------------------------------------------------
	// EA_NEON
	// EA_NEON may be used to determine if NEON is supported.
	#ifndef EA_NEON
		#if defined(__ARM_NEON__) || defined(__ARM_NEON)
			#define EA_NEON 1
		#else
			#define EA_NEON 0
		#endif
	#endif

	// ------------------------------------------------------------------------
	// EA_BMI
	// EA_BMI may be used to determine if Bit Manipulation Instruction sets are available for the target architecture
	//
	// EA_BMI defines the level of BMI support:
	//  0 indicates no BMI support
	//  1 indicates BMI1 is supported
	//  2 indicates BMI2 is supported
	#ifndef EA_BMI
		#if defined(__BMI2__)
			#define EA_BMI 2
		#elif defined(__BMI__) || defined(EA_PLATFORM_CAPILANO)
			#define EA_BMI 1
		#else
			#define EA_BMI 0
		#endif
	#endif
	#ifndef EA_BMI2
		#if EA_BMI >= 2
			#define EA_BMI2 1
		#else
			#define EA_BMI2 0
		#endif
	#endif

	// ------------------------------------------------------------------------
	// EA_FMA3
	// EA_FMA3 may be used to determine if Fused Multiply Add operations are available for the target architecture
	// __FMA__ is defined only by GCC, Clang, and ICC; MSVC only defines __AVX__ and __AVX2__
	// FMA3 was introduced alongside AVX2 on Intel Haswell
	// All AMD processors support FMA3 if AVX2 is also supported
	//
	// EA_FMA3 defines the level of FMA3 support:
	//  0 indicates no FMA3 support
	//  1 indicates FMA3 is supported
	#ifndef EA_FMA3
		#if defined(__FMA__) || EA_AVX2 >= 1
			#define EA_FMA3 1
		#else
			#define EA_FMA3 0
		#endif
	#endif

	// ------------------------------------------------------------------------
	// EA_TBM
	// EA_TBM may be used to determine if Trailing Bit Manipulation instructions are available for the target architecture
	#ifndef EA_TBM
		#if defined(__TBM__)
			#define EA_TBM 1
		#else
			#define EA_TBM 0
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_IMPORT
	// import declaration specification
	// specifies that the declared symbol is imported from another dynamic library.
	#ifndef EA_IMPORT
		#if defined(EA_COMPILER_MSVC)
			#define EA_IMPORT __declspec(dllimport)
		#else
			#define EA_IMPORT
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_EXPORT
	// export declaration specification
	// specifies that the declared symbol is exported from the current dynamic library.
	// this is not the same as the C++ export keyword. The C++ export keyword has been
	// removed from the language as of C++11.
	#ifndef EA_EXPORT
		#if defined(EA_COMPILER_MSVC)
			#define EA_EXPORT __declspec(dllexport)
		#else
			#define EA_EXPORT
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_PRAGMA_ONCE_SUPPORTED
	// 
	// This is a wrapper for the #pragma once preprocessor directive.
	// It allows for some compilers (in particular VC++) to implement signifcantly
	// faster include file preprocessing. #pragma once can be used to replace 
	// header include guards or to augment them. However, #pragma once isn't 
	// necessarily supported by all compilers and isn't guaranteed to be so in
	// the future, so using #pragma once to replace traditional include guards 
	// is not strictly portable. Note that a direct #define for #pragma once is
	// impossible with VC++, due to limitations, but can be done with other 
	// compilers/preprocessors via _Pragma("once").
	// 
	// Example usage (which includes traditional header guards for portability):
	//    #ifndef SOMEPACKAGE_SOMEHEADER_H
	//    #define SOMEPACKAGE_SOMEHEADER_H
	//
	//    #if defined(EA_PRAGMA_ONCE_SUPPORTED)
	//        #pragma once
	//    #endif
	//
	//    <user code> 
	//
	//    #endif
	//
	#if defined(_MSC_VER) || defined(__GNUC__) || defined(__EDG__) || defined(__APPLE__)
		#define EA_PRAGMA_ONCE_SUPPORTED 1
	#endif



	// ------------------------------------------------------------------------
	// EA_ONCE
	// 
	// Example usage (which includes traditional header guards for portability):
	//    #ifndef SOMEPACKAGE_SOMEHEADER_H
	//    #define SOMEPACKAGE_SOMEHEADER_H
	//
	//    EA_ONCE()
	//
	//    <user code> 
	//
	//    #endif
	//
	#if defined(EA_PRAGMA_ONCE_SUPPORTED)
		#if defined(_MSC_VER)
			#define EA_ONCE() __pragma(once)
		#else
			#define EA_ONCE() // _Pragma("once")   It turns out that _Pragma("once") isn't supported by many compilers.
		#endif
	#endif



	// ------------------------------------------------------------------------
	// EA_OVERRIDE
	// 
	// C++11 override
	// See http://msdn.microsoft.com/en-us/library/jj678987.aspx for more information.
	// You can use EA_FINAL_OVERRIDE to combine usage of EA_OVERRIDE and EA_INHERITANCE_FINAL in a single statement.
	//
	// Example usage: 
	//        struct B     { virtual void f(int); };
	//        struct D : B { void f(int) EA_OVERRIDE; };
	// 
	#ifndef EA_OVERRIDE
		#if defined(EA_COMPILER_NO_OVERRIDE)
			#define EA_OVERRIDE 
		#else
			#define EA_OVERRIDE override
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_INHERITANCE_FINAL
	// 
	// Portably wraps the C++11 final specifier.
	// See http://msdn.microsoft.com/en-us/library/jj678985.aspx for more information.
	// You can use EA_FINAL_OVERRIDE to combine usage of EA_OVERRIDE and EA_INHERITANCE_FINAL in a single statement.
	// This is not called EA_FINAL because that term is used within EA to denote debug/release/final builds.
	//
	// Example usage:
	//     struct B { virtual void f() EA_INHERITANCE_FINAL; };
	// 
	#ifndef EA_INHERITANCE_FINAL
		#if defined(EA_COMPILER_NO_INHERITANCE_FINAL)
			#define EA_INHERITANCE_FINAL
		#elif (defined(_MSC_VER) && (EA_COMPILER_VERSION < 1700))  // Pre-VS2012
			#define EA_INHERITANCE_FINAL sealed
		#else
			#define EA_INHERITANCE_FINAL final
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_FINAL_OVERRIDE
	// 
	// Portably wraps the C++11 override final specifiers combined.
	//
	// Example usage:
	//     struct A            { virtual void f(); };
	//     struct B : public A { virtual void f() EA_FINAL_OVERRIDE; };
	// 
	#ifndef EA_FINAL_OVERRIDE
		#define EA_FINAL_OVERRIDE EA_OVERRIDE EA_INHERITANCE_FINAL
	#endif


	// ------------------------------------------------------------------------
	// EA_SEALED
	// 
	// This is deprecated, as the C++11 Standard has final (EA_INHERITANCE_FINAL) instead.
	// See http://msdn.microsoft.com/en-us/library/0w2w91tf.aspx for more information.
	// Example usage:
	//     struct B { virtual void f() EA_SEALED; };
	// 
	#ifndef EA_SEALED
		#if defined(EA_COMPILER_MSVC) && (EA_COMPILER_VERSION >= 1400) // VS2005 (VC8) and later
			#define EA_SEALED sealed
		#else
			#define EA_SEALED 
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_ABSTRACT
	// 
	// This is a Microsoft language extension.
	// See http://msdn.microsoft.com/en-us/library/b0z6b513.aspx for more information.
	// Example usage:
	//     struct X EA_ABSTRACT { virtual void f(){} };
	// 
	#ifndef EA_ABSTRACT
		#if defined(EA_COMPILER_MSVC) && (EA_COMPILER_VERSION >= 1400) // VS2005 (VC8) and later
			#define EA_ABSTRACT abstract
		#else
			#define EA_ABSTRACT 
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_CONSTEXPR
	// EA_CONSTEXPR_OR_CONST
	// 
	// Portable wrapper for C++11's 'constexpr' support.
	//
	// See http://www.cprogramming.com/c++11/c++11-compile-time-processing-with-constexpr.html for more information.
	// Example usage:
	//     EA_CONSTEXPR int GetValue() { return 37; }
	//     EA_CONSTEXPR_OR_CONST double gValue = std::sin(kTwoPi);
	// 
	#if !defined(EA_CONSTEXPR)
	#if defined(EA_COMPILER_NO_CONSTEXPR)
		#define EA_CONSTEXPR
	#else
		#define EA_CONSTEXPR constexpr
	#endif
	#endif

	#if !defined(EA_CONSTEXPR_OR_CONST)
		#if defined(EA_COMPILER_NO_CONSTEXPR)
			#define EA_CONSTEXPR_OR_CONST const
		#else
			#define EA_CONSTEXPR_OR_CONST constexpr
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_EXTERN_TEMPLATE
	// 
	// Portable wrapper for C++11's 'extern template' support.
	//
	// Example usage:
	//     EA_EXTERN_TEMPLATE(class basic_string<char>);
	//
	#if !defined(EA_EXTERN_TEMPLATE)
	#if defined(EA_COMPILER_NO_EXTERN_TEMPLATE)
		#define EA_EXTERN_TEMPLATE(declaration)
	#else
		#define EA_EXTERN_TEMPLATE(declaration) extern template declaration
	#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_NOEXCEPT
	// EA_NOEXCEPT_IF(predicate)
	// EA_NOEXCEPT_EXPR(expression)
	//
	// Portable wrapper for C++11 noexcept
	// http://en.cppreference.com/w/cpp/language/noexcept
	// http://en.cppreference.com/w/cpp/language/noexcept_spec
	//
	// Example usage:
	//     EA_NOEXCEPT
	//     EA_NOEXCEPT_IF(predicate)
	//     EA_NOEXCEPT_EXPR(expression)
	//
	//     This function never throws an exception.
	//     void DoNothing() EA_NOEXCEPT
	//         { }
	//
	//     This function throws an exception of T::T() throws an exception.
	//     template <class T>
	//     void DoNothing() EA_NOEXCEPT_IF(EA_NOEXCEPT_EXPR(T()))
	//         { T t; }
	//
	#if !defined(EA_NOEXCEPT)
		#if defined(EA_COMPILER_NO_NOEXCEPT)
			#define EA_NOEXCEPT
			#define EA_NOEXCEPT_IF(predicate)
			#define EA_NOEXCEPT_EXPR(expression) false
		#else
			#define EA_NOEXCEPT noexcept
			#define EA_NOEXCEPT_IF(predicate) noexcept((predicate))
			#define EA_NOEXCEPT_EXPR(expression) noexcept((expression))
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_NORETURN
	// 
	// Wraps the C++11 noreturn attribute. See EA_COMPILER_NO_NORETURN
	// http://en.cppreference.com/w/cpp/language/attributes
	// http://msdn.microsoft.com/en-us/library/k6ktzx3s%28v=vs.80%29.aspx
	// http://blog.aaronballman.com/2011/09/understanding-attributes/
	//
	// Example usage:
	//     EA_NORETURN void SomeFunction()
	//         { throw "error"; }
	// 
	#if !defined(EA_NORETURN)
		#if defined(EA_COMPILER_MSVC) && (EA_COMPILER_VERSION >= 1300) // VS2003 (VC7) and later
			#define EA_NORETURN __declspec(noreturn)
		#elif defined(EA_COMPILER_NO_NORETURN)
			#define EA_NORETURN
		#else
			#define EA_NORETURN [[noreturn]]
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_CARRIES_DEPENDENCY
	// 
	// Wraps the C++11 carries_dependency attribute
	// http://en.cppreference.com/w/cpp/language/attributes
	// http://blog.aaronballman.com/2011/09/understanding-attributes/
	//
	// Example usage:
	//     EA_CARRIES_DEPENDENCY int* SomeFunction()
	//         { return &mX; }
	// 
	//
	#if !defined(EA_CARRIES_DEPENDENCY)
		#if defined(EA_COMPILER_NO_CARRIES_DEPENDENCY)
			#define EA_CARRIES_DEPENDENCY
		#else
			#define EA_CARRIES_DEPENDENCY [[carries_dependency]]
		#endif
	#endif

	
	// ------------------------------------------------------------------------
	// EA_FALLTHROUGH
	// 
	// [[fallthrough] is a C++17 standard attribute that appears in switch
	// statements to indicate that the fallthrough from the previous case in the
	// switch statement is intentially and not a bug.
	// 
	// http://en.cppreference.com/w/cpp/language/attributes
	//
	// Example usage:
	// 		void f(int n)
	// 		{
	// 			switch(n)
	// 			{
	// 				case 1:
	// 				DoCase1();
	// 				// Compiler may generate a warning for fallthrough behaviour
	// 		 
	// 				case 2: 
	// 				DoCase2();
	//
	// 				EA_FALLTHROUGH;
	// 				case 3:
	// 				DoCase3();
	// 			}
	// 		}
	//
	#if !defined(EA_FALLTHROUGH)
		#if defined(EA_COMPILER_NO_FALLTHROUGH)
			#define EA_FALLTHROUGH
		#else
			#define EA_FALLTHROUGH [[fallthrough]]
		#endif
	#endif



	// ------------------------------------------------------------------------
	// EA_NODISCARD
	// 
	// [[nodiscard]] is a C++17 standard attribute that can be applied to a
	// function declaration, enum, or class declaration.  If a any of the list
	// previously are returned from a function (without the user explicitly
	// casting to void) the addition of the [[nodiscard]] attribute encourages
	// the compiler to generate a warning about the user discarding the return
	// value. This is a useful practice to encourage client code to check API
	// error codes. 
	//
	// http://en.cppreference.com/w/cpp/language/attributes
	//
	// Example usage:
	// 
	//     EA_NODISCARD int baz() { return 42; }
	//     
	//     void foo()
	//     {
	//         baz(); // warning: ignoring return value of function declared with 'nodiscard' attribute 
	//     }
	//
	#if !defined(EA_NODISCARD)
		#if defined(EA_COMPILER_NO_NODISCARD)
			#define EA_NODISCARD
		#else
			#define EA_NODISCARD [[nodiscard]]
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_MAYBE_UNUSED
	// 
	// [[maybe_unused]] is a C++17 standard attribute that suppresses warnings
	// on unused entities that are declared as maybe_unused.
	//
	// http://en.cppreference.com/w/cpp/language/attributes
	//
	// Example usage:
	//    void foo(EA_MAYBE_UNUSED int i)
	//    {
	//        assert(i == 42);  // warning suppressed when asserts disabled.
	//    }
	//
	#if !defined(EA_MAYBE_UNUSED)
		#if defined(EA_COMPILER_NO_MAYBE_UNUSED)
			#define EA_MAYBE_UNUSED
		#else
			#define EA_MAYBE_UNUSED [[maybe_unused]]
		#endif
	#endif

	
	// ------------------------------------------------------------------------
	// EA_NO_UBSAN
	// 
	// The LLVM/Clang undefined behaviour sanitizer will not analyse a function tagged with the following attribute.
	//
	// https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html#disabling-instrumentation-with-attribute-no-sanitize-undefined
	//
	// Example usage:
	//     EA_NO_UBSAN int SomeFunction() { ... }
	//
	#ifndef EA_NO_UBSAN
		#if defined(EA_COMPILER_CLANG)
			#define EA_NO_UBSAN __attribute__((no_sanitize("undefined")))
		#else
			#define EA_NO_UBSAN
		#endif
	#endif
	

	// ------------------------------------------------------------------------
	// EA_NO_ASAN
	// 
	// The LLVM/Clang address sanitizer will not analyse a function tagged with the following attribute.
	//
	// https://clang.llvm.org/docs/AddressSanitizer.html#disabling-instrumentation-with-attribute-no-sanitize-address
	//
	// Example usage:
	//     EA_NO_ASAN int SomeFunction() { ... }
	//
	#ifndef EA_NO_ASAN
		#if defined(EA_COMPILER_CLANG)
			#define EA_NO_ASAN __attribute__((no_sanitize("address")))
		#else
			#define EA_NO_ASAN
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_ASAN_ENABLED
	//
	// Defined as 0 or 1. It's value depends on the compile environment.
	// Specifies whether the code is being built with Clang's Address Sanitizer.
	//
	#if defined(__has_feature)
		#if __has_feature(address_sanitizer)
			#define EA_ASAN_ENABLED 1
		#else
			#define EA_ASAN_ENABLED 0
		#endif
	#else
		#define EA_ASAN_ENABLED 0
	#endif


	// ------------------------------------------------------------------------
	// EA_NON_COPYABLE
	//
	// This macro defines as a class as not being copy-constructable
	// or assignable. This is useful for preventing class instances 
	// from being passed to functions by value, is useful for preventing
	// compiler warnings by some compilers about the inability to 
	// auto-generate a copy constructor and assignment, and is useful 
	// for simply declaring in the interface that copy semantics are
	// not supported by the class. Your class needs to have at least a
	// default constructor when using this macro.
	//
	// Beware that this class works by declaring a private: section of 
	// the class in the case of compilers that don't support C++11 deleted
	// functions. 
	//
	// Note: With some pre-C++11 compilers (e.g. Green Hills), you may need 
	//       to manually define an instances of the hidden functions, even 
	//       though they are not used.
	//
	// Example usage:
	//    class Widget {
	//       Widget();
	//       . . .
	//       EA_NON_COPYABLE(Widget)
	//    };
	//
	#if !defined(EA_NON_COPYABLE)
		#if defined(EA_COMPILER_NO_DELETED_FUNCTIONS)
			#define EA_NON_COPYABLE(EAClass_)               \
			  private:                                      \
				EA_DISABLE_VC_WARNING(4822);	/* local class member function does not have a body	*/		\
				EAClass_(const EAClass_&);                  \
				void operator=(const EAClass_&);			\
				EA_RESTORE_VC_WARNING();
		#else
			#define EA_NON_COPYABLE(EAClass_)               \
				EA_DISABLE_VC_WARNING(4822);	/* local class member function does not have a body	*/		\
				EAClass_(const EAClass_&) = delete;         \
				void operator=(const EAClass_&) = delete;	\
				EA_RESTORE_VC_WARNING();
		#endif
	#endif


	// ------------------------------------------------------------------------
	// EA_FUNCTION_DELETE
	//
	// Semi-portable way of specifying a deleted function which allows for 
	// cleaner code in class declarations. 
	//
	// Example usage:
	//
	//  class Example
	//  {
	//  private: // For portability with pre-C++11 compilers, make the function private.
	//      void foo() EA_FUNCTION_DELETE;
	//  };
	//
	// Note: EA_FUNCTION_DELETE'd functions should be private to prevent the 
	// functions from being called even when the compiler does not support 
	// deleted functions. Some compilers (e.g. Green Hills) that don't support 
	// C++11 deleted functions can require that you define the function,
	// which you can do in the associated source file for the class.
	//
	#if defined(EA_COMPILER_NO_DELETED_FUNCTIONS)
		#define EA_FUNCTION_DELETE
	#else
		#define EA_FUNCTION_DELETE = delete
	#endif

	// ------------------------------------------------------------------------
	// EA_DISABLE_DEFAULT_CTOR
	//
	// Disables the compiler generated default constructor. This macro is
	// provided to improve portability and clarify intent of code.
	//
	// Example usage:
	//
	//  class Example
	//  {
	//  private:
	//      EA_DISABLE_DEFAULT_CTOR(Example);
	//  };
	//
	#define EA_DISABLE_DEFAULT_CTOR(ClassName) ClassName() EA_FUNCTION_DELETE

	// ------------------------------------------------------------------------
	// EA_DISABLE_COPY_CTOR
	//
	// Disables the compiler generated copy constructor. This macro is
	// provided to improve portability and clarify intent of code.
	//
	// Example usage:
	//
	//  class Example
	//  {
	//  private:
	//      EA_DISABLE_COPY_CTOR(Example);
	//  };
	//
	#define EA_DISABLE_COPY_CTOR(ClassName) ClassName(const ClassName &) EA_FUNCTION_DELETE

	// ------------------------------------------------------------------------
	// EA_DISABLE_MOVE_CTOR
	//
	// Disables the compiler generated move constructor. This macro is
	// provided to improve portability and clarify intent of code.
	//
	// Example usage:
	//
	//  class Example
	//  {
	//  private:
	//      EA_DISABLE_MOVE_CTOR(Example);
	//  };
	//
	#define EA_DISABLE_MOVE_CTOR(ClassName) ClassName(ClassName&&) EA_FUNCTION_DELETE

	// ------------------------------------------------------------------------
	// EA_DISABLE_ASSIGNMENT_OPERATOR
	//
	// Disables the compiler generated assignment operator. This macro is
	// provided to improve portability and clarify intent of code.
	//
	// Example usage:
	//
	//  class Example
	//  {
	//  private:
	//      EA_DISABLE_ASSIGNMENT_OPERATOR(Example);
	//  };
	//
	#define EA_DISABLE_ASSIGNMENT_OPERATOR(ClassName) ClassName & operator=(const ClassName &) EA_FUNCTION_DELETE

	// ------------------------------------------------------------------------
	// EA_DISABLE_MOVE_OPERATOR
	//
	// Disables the compiler generated move operator. This macro is
	// provided to improve portability and clarify intent of code.
	//
	// Example usage:
	//
	//  class Example
	//  {
	//  private:
	//      EA_DISABLE_MOVE_OPERATOR(Example);
	//  };
	//
	#define EA_DISABLE_MOVE_OPERATOR(ClassName) ClassName & operator=(ClassName&&) EA_FUNCTION_DELETE

	// ------------------------------------------------------------------------
	// EANonCopyable
	//
	// Declares a class as not supporting copy construction or assignment.
	// May be more reliable with some situations that EA_NON_COPYABLE alone,
	// though it may result in more code generation.
	//
	// Note that VC++ will generate warning C4625 and C4626 if you use EANonCopyable
	// and you are compiling with /W4 and /Wall. There is no resolution but
	// to redelare EA_NON_COPYABLE in your subclass or disable the warnings with
	// code like this:
	//     EA_DISABLE_VC_WARNING(4625 4626)
	//     ...
	//     EA_RESTORE_VC_WARNING()
	//
	// Example usage:
	//     struct Widget : EANonCopyable {
	//        . . .
	//     };
	//
	#ifdef __cplusplus
		struct EANonCopyable
		{
			#if defined(EA_COMPILER_NO_DEFAULTED_FUNCTIONS) || defined(__EDG__) // EDG doesn't appear to behave properly for the case of defaulted constructors; it generates a mistaken warning about missing default constructors.
				EANonCopyable(){} // Putting {} here has the downside that it allows a class to create itself, 
			   ~EANonCopyable(){} // but avoids linker errors that can occur with some compilers (e.g. Green Hills).
			#else
				EANonCopyable() = default;
			   ~EANonCopyable() = default;
			#endif
			EA_NON_COPYABLE(EANonCopyable)
		};
	#endif


	// ------------------------------------------------------------------------
	// EA_OPTIMIZE_OFF / EA_OPTIMIZE_ON
	//
	// Implements portable inline optimization enabling/disabling.
	// Usage of these macros must be in order OFF then ON. This is 
	// because the OFF macro pushes a set of settings and the ON
	// macro pops them. The nesting of OFF/ON sets (e.g. OFF, OFF, ON, ON)
	// is not guaranteed to work on all platforms.
	//
	// This is often used to allow debugging of some code that's 
	// otherwise compiled with undebuggable optimizations. It's also
	// useful for working around compiler code generation problems
	// that occur in optimized builds.
	//
	// Some compilers (e.g. VC++) don't allow doing this within a function and 
	// so the usage must be outside a function, as with the example below.
	// GCC on x86 appears to have some problem with argument passing when 
	// using EA_OPTIMIZE_OFF in optimized builds.
	//
	// Example usage:
	//     // Disable optimizations for SomeFunction.
	//     EA_OPTIMIZE_OFF()
	//     void SomeFunction()
	//     {
	//         ...
	//     }
	//     EA_OPTIMIZE_ON()
	//
	#if !defined(EA_OPTIMIZE_OFF)
		#if   defined(EA_COMPILER_MSVC)
			#define EA_OPTIMIZE_OFF() __pragma(optimize("", off))
		#elif defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION > 4004) && (defined(__i386__) || defined(__x86_64__)) // GCC 4.4+ - Seems to work only on x86/Linux so far. However, GCC 4.4 itself appears broken and screws up parameter passing conventions.
			#define EA_OPTIMIZE_OFF()            \
				_Pragma("GCC push_options")      \
				_Pragma("GCC optimize 0")
        #elif defined(EA_COMPILER_CLANG) && (!defined(EA_PLATFORM_ANDROID) || (EA_COMPILER_VERSION >= 380))
            #define EA_OPTIMIZE_OFF() \
				EA_DISABLE_CLANG_WARNING(-Wunknown-pragmas) \
				_Pragma("clang optimize off") \
				EA_RESTORE_CLANG_WARNING()
		#else
			#define EA_OPTIMIZE_OFF()
		#endif
	#endif

	#if !defined(EA_OPTIMIZE_ON)
		#if   defined(EA_COMPILER_MSVC)
			#define EA_OPTIMIZE_ON() __pragma(optimize("", on))
		#elif defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION > 4004) && (defined(__i386__) || defined(__x86_64__)) // GCC 4.4+ - Seems to work only on x86/Linux so far. However, GCC 4.4 itself appears broken and screws up parameter passing conventions.
			#define EA_OPTIMIZE_ON() _Pragma("GCC pop_options")
        #elif defined(EA_COMPILER_CLANG) && (!defined(EA_PLATFORM_ANDROID) || (EA_COMPILER_VERSION >= 380))
            #define EA_OPTIMIZE_ON() \
				EA_DISABLE_CLANG_WARNING(-Wunknown-pragmas) \
				_Pragma("clang optimize on") \
				EA_RESTORE_CLANG_WARNING()
		#else
			#define EA_OPTIMIZE_ON()
		#endif
	#endif



	// ------------------------------------------------------------------------
	// EA_SIGNED_RIGHT_SHIFT_IS_UNSIGNED
	//
	// Defined if right shifts of signed integers (i.e. arithmetic shifts) fail 
	// to propogate the high bit downward, and thus preserve sign. Most hardware 
	// and their corresponding compilers do this.
	//
	// <No current platform fails to propogate sign bits on right signed shifts>

#endif // Header include guard










