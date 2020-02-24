/*-----------------------------------------------------------------------------
 * config/eacompiler.h
 *
 * Copyright (c) Electronic Arts Inc. All rights reserved.
 *-----------------------------------------------------------------------------
 * Currently supported defines include:
 *     EA_COMPILER_GNUC
 *     EA_COMPILER_ARM
 *     EA_COMPILER_EDG
 *     EA_COMPILER_SN
 *     EA_COMPILER_MSVC
 *     EA_COMPILER_METROWERKS
 *     EA_COMPILER_INTEL
 *     EA_COMPILER_BORLANDC
 *     EA_COMPILER_IBM
 *     EA_COMPILER_QNX
 *     EA_COMPILER_GREEN_HILLS
 *     EA_COMPILER_CLANG
 *     EA_COMPILER_CLANG_CL
 *     
 *     EA_COMPILER_VERSION = <integer>
 *     EA_COMPILER_NAME = <string>
 *     EA_COMPILER_STRING = <string>
 *     
 *     EA_COMPILER_VA_COPY_REQUIRED
 * 
 *  C++98/03 functionality
 *     EA_COMPILER_NO_STATIC_CONSTANTS
 *     EA_COMPILER_NO_TEMPLATE_SPECIALIZATION
 *     EA_COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION
 *     EA_COMPILER_NO_MEMBER_TEMPLATES
 *     EA_COMPILER_NO_MEMBER_TEMPLATE_SPECIALIZATION
 *     EA_COMPILER_NO_TEMPLATE_TEMPLATES
 *     EA_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS
 *     EA_COMPILER_NO_VOID_RETURNS
 *     EA_COMPILER_NO_COVARIANT_RETURN_TYPE
 *     EA_COMPILER_NO_DEDUCED_TYPENAME
 *     EA_COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP
 *     EA_COMPILER_NO_EXCEPTION_STD_NAMESPACE
 *     EA_COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS
 *     EA_COMPILER_NO_RTTI
 *     EA_COMPILER_NO_EXCEPTIONS
 *     EA_COMPILER_NO_NEW_THROW_SPEC
 *     EA_THROW_SPEC_NEW / EA_THROW_SPEC_DELETE
 *     EA_COMPILER_NO_UNWIND
 *     EA_COMPILER_NO_STANDARD_CPP_LIBRARY
 *     EA_COMPILER_NO_STATIC_VARIABLE_INIT
 *     EA_COMPILER_NO_STATIC_FUNCTION_INIT
 *     EA_COMPILER_NO_VARIADIC_MACROS
 *
 *  C++11 functionality
 *     EA_COMPILER_NO_RVALUE_REFERENCES
 *     EA_COMPILER_NO_EXTERN_TEMPLATE
 *     EA_COMPILER_NO_RANGE_BASED_FOR_LOOP
 *     EA_COMPILER_NO_CONSTEXPR
 *     EA_COMPILER_NO_OVERRIDE
 *     EA_COMPILER_NO_INHERITANCE_FINAL
 *     EA_COMPILER_NO_NULLPTR
 *     EA_COMPILER_NO_AUTO
 *     EA_COMPILER_NO_DECLTYPE
 *     EA_COMPILER_NO_DEFAULTED_FUNCTIONS
 *     EA_COMPILER_NO_DELETED_FUNCTIONS
 *     EA_COMPILER_NO_LAMBDA_EXPRESSIONS
 *     EA_COMPILER_NO_TRAILING_RETURN_TYPES
 *     EA_COMPILER_NO_STRONGLY_TYPED_ENUMS
 *     EA_COMPILER_NO_FORWARD_DECLARED_ENUMS
 *     EA_COMPILER_NO_VARIADIC_TEMPLATES
 *     EA_COMPILER_NO_TEMPLATE_ALIASES
 *     EA_COMPILER_NO_INITIALIZER_LISTS
 *     EA_COMPILER_NO_NORETURN
 *     EA_COMPILER_NO_CARRIES_DEPENDENCY
 *     EA_COMPILER_NO_FALLTHROUGH
 *     EA_COMPILER_NO_NODISCARD
 *     EA_COMPILER_NO_MAYBE_UNUSED
 *     EA_COMPILER_NO_NONSTATIC_MEMBER_INITIALIZERS
 *     EA_COMPILER_NO_RIGHT_ANGLE_BRACKETS
 *     EA_COMPILER_NO_ALIGNOF
 *     EA_COMPILER_NO_ALIGNAS
 *     EA_COMPILER_NO_DELEGATING_CONSTRUCTORS
 *     EA_COMPILER_NO_INHERITING_CONSTRUCTORS
 *     EA_COMPILER_NO_USER_DEFINED_LITERALS
 *     EA_COMPILER_NO_STANDARD_LAYOUT_TYPES
 *     EA_COMPILER_NO_EXTENDED_SIZEOF
 *     EA_COMPILER_NO_INLINE_NAMESPACES
 *     EA_COMPILER_NO_UNRESTRICTED_UNIONS
 *     EA_COMPILER_NO_EXPLICIT_CONVERSION_OPERATORS
 *     EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
 *     EA_COMPILER_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS
 *     EA_COMPILER_NO_NOEXCEPT
 *     EA_COMPILER_NO_RAW_LITERALS
 *     EA_COMPILER_NO_UNICODE_STRING_LITERALS
 *     EA_COMPILER_NO_NEW_CHARACTER_TYPES
 *     EA_COMPILER_NO_UNICODE_CHAR_NAME_LITERALS
 *     EA_COMPILER_NO_UNIFIED_INITIALIZATION_SYNTAX
 *     EA_COMPILER_NO_EXTENDED_FRIEND_DECLARATIONS
 *
 *  C++14 functionality
 *     EA_COMPILER_NO_VARIABLE_TEMPLATES
 * 
 *  C++17 functionality
 *     EA_COMPILER_NO_INLINE_VARIABLES
 *     EA_COMPILER_NO_ALIGNED_NEW
 *
 *  C++20 functionality
 *     EA_COMPILER_NO_DESIGNATED_INITIALIZERS
 *     
 *-----------------------------------------------------------------------------
 *
 * Supplemental documentation
 *     EA_COMPILER_NO_STATIC_CONSTANTS
 *         Code such as this is legal, but some compilers fail to compile it:
 *             struct A{ static const a = 1; };
 *
 *     EA_COMPILER_NO_TEMPLATE_SPECIALIZATION
 *         Some compilers fail to allow template specialization, such as with this:
 *             template<class U> void DoSomething(U u);
 *             void DoSomething(int x);
 *
 *     EA_COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION
 *         Some compilers fail to allow partial template specialization, such as with this:
 *             template <class T, class Allocator> class vector{ };         // Primary templated class.
 *             template <class Allocator> class vector<bool, Allocator>{ }; // Partially specialized version.
 *
 *     EA_COMPILER_NO_MEMBER_TEMPLATES
 *         Some compilers fail to allow member template functions such as this:
 *             struct A{ template<class U> void DoSomething(U u); };
 *
 *     EA_COMPILER_NO_MEMBER_TEMPLATE_SPECIALIZATION
 *         Some compilers fail to allow member template specialization, such as with this:
 *             struct A{ 
 *                 template<class U> void DoSomething(U u);
 *                 void DoSomething(int x);
 *             };
 *
 *     EA_COMPILER_NO_TEMPLATE_TEMPLATES
 *         Code such as this is legal:
 *             template<typename T, template<typename> class U>
 *             U<T> SomeFunction(const U<T> x) { return x.DoSomething(); }
 *
 *     EA_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS
 *         Some compilers fail to compile templated friends, as with this:
 *             struct A{ template<class U> friend class SomeFriend; };
 *         This is described in the C++ Standard at 14.5.3.
 *
 *     EA_COMPILER_NO_VOID_RETURNS
 *          This is legal C++:
 *              void DoNothing1(){ };
 *              void DoNothing2(){ return DoNothing1(); }
 *
 *     EA_COMPILER_NO_COVARIANT_RETURN_TYPE
 *         See the C++ standard sec 10.3,p5.
 *     
 *     EA_COMPILER_NO_DEDUCED_TYPENAME
 *         Some compilers don't support the use of 'typename' for 
 *         dependent types in deduced contexts, as with this:
 *             template <class T> void Function(T, typename T::type);
 *
 *     EA_COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP
 *         Also known as Koenig lookup. Basically, if you have a function
 *         that is a namespace and you call that function without prefixing
 *         it with the namespace the compiler should look at any arguments
 *         you pass to that function call and search their namespace *first* 
 *         to see if the given function exists there.
 *
 *     EA_COMPILER_NO_EXCEPTION_STD_NAMESPACE
 *         <exception> is in namespace std. Some std libraries fail to 
 *         put the contents of <exception> in namespace std. The following 
 *         code should normally be legal:
 *             void Function(){ std::terminate(); }
 *
 *     EA_COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS
 *         Some compilers fail to execute DoSomething() properly, though they 
 *         succeed in compiling it, as with this:
 *             template <int i>
 *             bool DoSomething(int j){ return i == j; };
 *             DoSomething<1>(2);
 *
 *     EA_COMPILER_NO_EXCEPTIONS
 *         The compiler is configured to disallow the use of try/throw/catch
 *         syntax (often to improve performance). Use of such syntax in this 
 *         case will cause a compilation error. 
 *         
 *     EA_COMPILER_NO_UNWIND
 *         The compiler is configured to allow the use of try/throw/catch 
 *         syntax and behaviour but disables the generation of stack unwinding 
 *         code for responding to exceptions (often to improve performance).
 *
 *---------------------------------------------------------------------------*/

#ifndef INCLUDED_eacompiler_H
#define INCLUDED_eacompiler_H

	#include <EABase/config/eaplatform.h>

	// Note: This is used to generate the EA_COMPILER_STRING macros
	#ifndef INTERNAL_STRINGIZE
		#define INTERNAL_STRINGIZE(x) INTERNAL_PRIMITIVE_STRINGIZE(x)
	#endif
	#ifndef INTERNAL_PRIMITIVE_STRINGIZE
		#define INTERNAL_PRIMITIVE_STRINGIZE(x) #x
	#endif

	// EA_COMPILER_HAS_FEATURE
	#ifndef EA_COMPILER_HAS_FEATURE
		#if defined(__clang__)
			#define EA_COMPILER_HAS_FEATURE(x) __has_feature(x)
		#else
			#define EA_COMPILER_HAS_FEATURE(x) 0
		#endif
	#endif


	// EA_COMPILER_HAS_BUILTIN
	#ifndef EA_COMPILER_HAS_BUILTIN
		#if defined(__clang__)
			#define EA_COMPILER_HAS_BUILTIN(x) __has_builtin(x)
		#else
			#define EA_COMPILER_HAS_BUILTIN(x) 0
		#endif
	#endif


	// EDG (EDG compiler front-end, used by other compilers such as SN)
	#if defined(__EDG_VERSION__)
		#define EA_COMPILER_EDG 1

		#if defined(_MSC_VER)
			#define EA_COMPILER_EDG_VC_MODE 1
		#endif
		#if defined(__GNUC__)
			#define EA_COMPILER_EDG_GCC_MODE 1
		#endif
	#endif

	// EA_COMPILER_WINRTCX_ENABLED
	//
	// Defined as 1 if the compiler has its available C++/CX support enabled, else undefined.
	// This specifically means the corresponding compilation unit has been built with Windows Runtime
	// Components enabled, usually via the '-ZW' compiler flags being used. This option allows for using
	// ref counted hat-type '^' objects and other C++/CX specific keywords like "ref new"
	#if !defined(EA_COMPILER_WINRTCX_ENABLED) && defined(__cplusplus_winrt)
		#define EA_COMPILER_WINRTCX_ENABLED 1
	#endif


	// EA_COMPILER_CPP11_ENABLED
	//
	// Defined as 1 if the compiler has its available C++11 support enabled, else undefined.
	// This does not mean that all of C++11 or any particular feature of C++11 is supported
	// by the compiler. It means that whatever C++11 support the compiler has is enabled.
	// This also includes existing and older compilers that still identify C++11 as C++0x.
	//
	// We cannot use (__cplusplus >= 201103L) alone because some compiler vendors have 
	// decided to not define __cplusplus like thus until they have fully completed their
	// C++11 support.
	//
	#if !defined(EA_COMPILER_CPP11_ENABLED) && defined(__cplusplus)
		#if (__cplusplus >= 201103L)    // Clang and GCC defines this like so in C++11 mode.
			#define EA_COMPILER_CPP11_ENABLED 1
		#elif defined(__GNUC__) && defined(__GXX_EXPERIMENTAL_CXX0X__)
			#define EA_COMPILER_CPP11_ENABLED 1
		#elif defined(_MSC_VER) && _MSC_VER >= 1600         // Microsoft unilaterally enables its C++11 support; there is no way to disable it.
			#define EA_COMPILER_CPP11_ENABLED 1
		#elif defined(__EDG_VERSION__) // && ???
			// To do: Is there a generic way to determine this?
		#endif
	#endif


	// EA_COMPILER_CPP14_ENABLED
	//
	// Defined as 1 if the compiler has its available C++14 support enabled, else undefined.
	// This does not mean that all of C++14 or any particular feature of C++14 is supported
	// by the compiler. It means that whatever C++14 support the compiler has is enabled.
	//
	// We cannot use (__cplusplus >= 201402L) alone because some compiler vendors have 
	// decided to not define __cplusplus like thus until they have fully completed their
	// C++14 support.
	#if !defined(EA_COMPILER_CPP14_ENABLED) && defined(__cplusplus)
		#if (__cplusplus >= 201402L) 								// Clang and GCC defines this like so in C++14 mode.
			#define EA_COMPILER_CPP14_ENABLED 1
		#elif defined(_MSC_VER) && (_MSC_VER >= 1900)  	// VS2015+ 
			#define EA_COMPILER_CPP14_ENABLED 1
		#endif
	#endif


	// EA_COMPILER_CPP17_ENABLED
	//
	// Defined as 1 if the compiler has its available C++17 support enabled, else undefined.
	// This does not mean that all of C++17 or any particular feature of C++17 is supported
	// by the compiler. It means that whatever C++17 support the compiler has is enabled.
 	// 
	// We cannot use (__cplusplus >= 201703L) alone because some compiler vendors have 
	// decided to not define __cplusplus like thus until they have fully completed their
	// C++17 support.
	#if !defined(EA_COMPILER_CPP17_ENABLED) && defined(__cplusplus)
		#if (__cplusplus >= 201703L) 
			#define EA_COMPILER_CPP17_ENABLED 1
		#elif defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L) // C++17+
			#define EA_COMPILER_CPP17_ENABLED 1
		#endif
	#endif


	// EA_COMPILER_CPP20_ENABLED
	//
	// Defined as 1 if the compiler has its available C++20 support enabled, else undefined.
	// This does not mean that all of C++20 or any particular feature of C++20 is supported
	// by the compiler. It means that whatever C++20 support the compiler has is enabled.
 	//
	// We cannot use (__cplusplus >= 202003L) alone because some compiler vendors have
	// decided to not define __cplusplus like thus until they have fully completed their
	// C++20 support.
	#if !defined(EA_COMPILER_CPP20_ENABLED) && defined(__cplusplus)
 		// TODO(rparoin): enable once a C++20 value for the __cplusplus macro has been published
		// #if (__cplusplus >= 202003L)
		//     #define EA_COMPILER_CPP20_ENABLED 1
		// #elif defined(_MSVC_LANG) && (_MSVC_LANG >= 202003L) // C++20+
		//     #define EA_COMPILER_CPP20_ENABLED 1
		// #endif
	#endif



	#if   defined(__ARMCC_VERSION)
		// Note that this refers to the ARM RVCT compiler (armcc or armcpp), but there
		// are other compilers that target ARM processors, such as GCC and Microsoft VC++.
		// If you want to detect compiling for the ARM processor, check for EA_PROCESSOR_ARM
		// being defined.
		// This compiler is also identified by defined(__CC_ARM) || defined(__ARMCC__).
		#define EA_COMPILER_RVCT    1
		#define EA_COMPILER_ARM     1
		#define EA_COMPILER_VERSION __ARMCC_VERSION
		#define EA_COMPILER_NAME    "RVCT"
	  //#define EA_COMPILER_STRING (defined below)

	// Clang's GCC-compatible driver.
	#elif defined(__clang__) && !defined(_MSC_VER)
		#define EA_COMPILER_CLANG   1
		#define EA_COMPILER_VERSION (__clang_major__ * 100 + __clang_minor__)
		#define EA_COMPILER_NAME    "clang"
		#define EA_COMPILER_STRING  EA_COMPILER_NAME __clang_version__

	// GCC (a.k.a. GNUC)
	#elif defined(__GNUC__) // GCC compilers exist for many platforms.
		#define EA_COMPILER_GNUC    1
		#define EA_COMPILER_VERSION (__GNUC__ * 1000 + __GNUC_MINOR__)
		#define EA_COMPILER_NAME    "GCC"
		#define EA_COMPILER_STRING  EA_COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE( __GNUC__ ) "." INTERNAL_STRINGIZE( __GNUC_MINOR__ )

		#if (__GNUC__ == 2) && (__GNUC_MINOR__ < 95) // If GCC < 2.95... 
			#define EA_COMPILER_NO_MEMBER_TEMPLATES 1
		#endif
		#if (__GNUC__ == 2) && (__GNUC_MINOR__ <= 97) // If GCC <= 2.97...
			#define EA_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS 1
		#endif
		#if (__GNUC__ == 3) && ((__GNUC_MINOR__ == 1) || (__GNUC_MINOR__ == 2)) // If GCC 3.1 or 3.2 (but not pre 3.1 or post 3.2)...
			#define EA_COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS 1
		#endif

	// Borland C++
	#elif defined(__BORLANDC__)
		#define EA_COMPILER_BORLANDC 1
		#define EA_COMPILER_VERSION  __BORLANDC__
		#define EA_COMPILER_NAME     "Borland C"
	  //#define EA_COMPILER_STRING (defined below)

		#if (__BORLANDC__ <= 0x0550)      // If Borland C++ Builder 4 and 5...
			#define EA_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS 1
		#endif
		#if (__BORLANDC__ >= 0x561) && (__BORLANDC__ < 0x600)
			#define EA_COMPILER_NO_MEMBER_FUNCTION_SPECIALIZATION 1
		#endif


	// Intel C++
	// The Intel Windows compiler masquerades as VC++ and defines _MSC_VER.
	// The Intel compiler is based on the EDG compiler front-end.
	#elif defined(__ICL) || defined(__ICC)
		#define EA_COMPILER_INTEL 1

		// Should we enable the following? We probably should do so since enabling it does a lot more good than harm
		// for users. The Intel Windows compiler does a pretty good job of emulating VC++ and so the user would likely
		// have to handle few special cases where the Intel compiler doesn't emulate VC++ correctly.
		#if defined(_MSC_VER)
			#define EA_COMPILER_MSVC 1
			#define EA_COMPILER_MICROSOFT 1
		#endif

		// Should we enable the following? This isn't as clear because as of this writing we don't know if the Intel 
		// compiler truly emulates GCC well enough that enabling this does more good than harm.
		#if defined(__GNUC__)
			#define EA_COMPILER_GNUC 1
		#endif

		#if defined(__ICL)
			#define EA_COMPILER_VERSION __ICL
		#elif defined(__ICC)
			#define EA_COMPILER_VERSION __ICC
		#endif
		#define EA_COMPILER_NAME "Intel C++"
		#if defined(_MSC_VER)
			#define EA_COMPILER_STRING  EA_COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE( EA_COMPILER_VERSION ) ", EDG version " INTERNAL_STRINGIZE( __EDG_VERSION__ ) ", VC++ version " INTERNAL_STRINGIZE( _MSC_VER )
		#elif defined(__GNUC__)
			#define EA_COMPILER_STRING  EA_COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE( EA_COMPILER_VERSION ) ", EDG version " INTERNAL_STRINGIZE( __EDG_VERSION__ ) ", GCC version " INTERNAL_STRINGIZE( __GNUC__ )
		#else
			#define EA_COMPILER_STRING  EA_COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE( EA_COMPILER_VERSION ) ", EDG version " INTERNAL_STRINGIZE( __EDG_VERSION__ )
		#endif


	#elif defined(_MSC_VER)
		#define EA_COMPILER_MSVC 1
		#define EA_COMPILER_MICROSOFT 1
		#define EA_COMPILER_VERSION _MSC_VER
		#define EA_COMPILER_NAME "Microsoft Visual C++"
	  //#define EA_COMPILER_STRING (defined below)

		#if defined(__clang__)
			// Clang's MSVC-compatible driver.
			#define EA_COMPILER_CLANG_CL 1
		#endif

		#define EA_STANDARD_LIBRARY_MSVC 1
		#define EA_STANDARD_LIBRARY_MICROSOFT 1

		#if (_MSC_VER <= 1200) // If VC6.x and earlier...
			#if (_MSC_VER < 1200)
				#define EA_COMPILER_MSVCOLD 1
			#else
				#define EA_COMPILER_MSVC6 1
			#endif

			#if (_MSC_VER < 1200) // If VC5.x or earlier...
				#define EA_COMPILER_NO_TEMPLATE_SPECIALIZATION 1
			#endif
			#define EA_COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS 1     // The compiler compiles this OK, but executes it wrong. Fixed in VC7.0
			#define EA_COMPILER_NO_VOID_RETURNS 1                             // The compiler fails to compile such cases. Fixed in VC7.0
			#define EA_COMPILER_NO_EXCEPTION_STD_NAMESPACE 1                  // The compiler fails to compile such cases. Fixed in VC7.0
			#define EA_COMPILER_NO_DEDUCED_TYPENAME 1                         // The compiler fails to compile such cases. Fixed in VC7.0
			#define EA_COMPILER_NO_STATIC_CONSTANTS 1                         // The compiler fails to compile such cases. Fixed in VC7.0
			#define EA_COMPILER_NO_COVARIANT_RETURN_TYPE 1                    // The compiler fails to compile such cases. Fixed in VC7.1
			#define EA_COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP 1                // The compiler compiles this OK, but executes it wrong. Fixed in VC7.1
			#define EA_COMPILER_NO_TEMPLATE_TEMPLATES 1                       // The compiler fails to compile such cases. Fixed in VC7.1
			#define EA_COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION 1          // The compiler fails to compile such cases. Fixed in VC7.1
			#define EA_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS 1                  // The compiler fails to compile such cases. Fixed in VC7.1
			//#define EA_COMPILER_NO_MEMBER_TEMPLATES 1                       // VC6.x supports member templates properly 95% of the time. So do we flag the remaining 5%?
			//#define EA_COMPILER_NO_MEMBER_TEMPLATE_SPECIALIZATION 1         // VC6.x supports member templates properly 95% of the time. So do we flag the remaining 5%?

		#elif (_MSC_VER <= 1300) // If VC7.0 and earlier...
			#define EA_COMPILER_MSVC7 1

			#define EA_COMPILER_NO_COVARIANT_RETURN_TYPE 1                    // The compiler fails to compile such cases. Fixed in VC7.1
			#define EA_COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP 1                // The compiler compiles this OK, but executes it wrong. Fixed in VC7.1
			#define EA_COMPILER_NO_TEMPLATE_TEMPLATES 1                       // The compiler fails to compile such cases. Fixed in VC7.1
			#define EA_COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION 1          // The compiler fails to compile such cases. Fixed in VC7.1
			#define EA_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS 1                  // The compiler fails to compile such cases. Fixed in VC7.1
			#define EA_COMPILER_NO_MEMBER_FUNCTION_SPECIALIZATION 1           // This is the case only for VC7.0 and not VC6 or VC7.1+. Fixed in VC7.1
			//#define EA_COMPILER_NO_MEMBER_TEMPLATES 1                       // VC7.0 supports member templates properly 95% of the time. So do we flag the remaining 5%?

		#elif (_MSC_VER < 1400) // VS2003       _MSC_VER of 1300 means VC7 (VS2003)
			// The VC7.1 and later compiler is fairly close to the C++ standard 
			// and thus has no compiler limitations that we are concerned about.
			#define EA_COMPILER_MSVC7_2003 1
			#define EA_COMPILER_MSVC7_1    1

		#elif (_MSC_VER < 1500) // VS2005       _MSC_VER of 1400 means VC8 (VS2005)
			#define EA_COMPILER_MSVC8_2005 1
			#define EA_COMPILER_MSVC8_0    1

		#elif (_MSC_VER < 1600) // VS2008.      _MSC_VER of 1500 means VC9 (VS2008)
			#define EA_COMPILER_MSVC9_2008 1
			#define EA_COMPILER_MSVC9_0    1

		#elif (_MSC_VER < 1700) // VS2010       _MSC_VER of 1600 means VC10 (VS2010)
			#define EA_COMPILER_MSVC_2010 1
			#define EA_COMPILER_MSVC10_0  1

		#elif (_MSC_VER < 1800) // VS2012       _MSC_VER of 1700 means VS2011/VS2012
			#define EA_COMPILER_MSVC_2011 1   // Microsoft changed the name to VS2012 before shipping, despite referring to it as VS2011 up to just a few weeks before shipping.
			#define EA_COMPILER_MSVC11_0  1
			#define EA_COMPILER_MSVC_2012 1
			#define EA_COMPILER_MSVC12_0  1

		#elif (_MSC_VER < 1900) // VS2013       _MSC_VER of 1800 means VS2013
			#define EA_COMPILER_MSVC_2013 1
			#define EA_COMPILER_MSVC13_0  1

		#elif (_MSC_VER < 1910) // VS2015       _MSC_VER of 1900 means VS2015
			#define EA_COMPILER_MSVC_2015 1
			#define EA_COMPILER_MSVC14_0  1

		#elif (_MSC_VER < 1911) // VS2017       _MSC_VER of 1910 means VS2017
			#define EA_COMPILER_MSVC_2017 1
			#define EA_COMPILER_MSVC15_0  1

		#endif


	// IBM
	#elif defined(__xlC__)
		#define EA_COMPILER_IBM     1
		#define EA_COMPILER_NAME    "IBM XL C"
		#define EA_COMPILER_VERSION __xlC__
		#define EA_COMPILER_STRING "IBM XL C compiler, version " INTERNAL_STRINGIZE( __xlC__ )

	// Unknown
	#else // Else the compiler is unknown

		#define EA_COMPILER_VERSION 0
		#define EA_COMPILER_NAME   "Unknown"

	#endif

	#ifndef EA_COMPILER_STRING
		#define EA_COMPILER_STRING EA_COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE(EA_COMPILER_VERSION)
	#endif


	// Deprecated definitions
	// For backwards compatibility, should be supported for at least the life of EABase v2.0.x.
	#ifndef EA_COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION
		#define EA_COMPILER_PARTIAL_TEMPLATE_SPECIALIZATION 1
	#endif
	#ifndef EA_COMPILER_NO_TEMPLATE_SPECIALIZATION
		#define EA_COMPILER_TEMPLATE_SPECIALIZATION 1
	#endif
	#ifndef EA_COMPILER_NO_MEMBER_TEMPLATES
		#define EA_COMPILER_MEMBER_TEMPLATES 1
	#endif
	#ifndef EA_COMPILER_NO_MEMBER_TEMPLATE_SPECIALIZATION
		#define EA_COMPILER_MEMBER_TEMPLATE_SPECIALIZATION 1
	#endif



	///////////////////////////////////////////////////////////////////////////////
	// EA_COMPILER_VA_COPY_REQUIRED
	//
	// Defines whether va_copy must be used to copy or save va_list objects between uses.
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
	///////////////////////////////////////////////////////////////////////////////

	#ifndef EA_COMPILER_VA_COPY_REQUIRED
		#if   ((defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__clang__)) && (!defined(__i386__) || defined(__x86_64__)) && !defined(__ppc__) && !defined(__PPC__) && !defined(__PPC64__)
			#define EA_COMPILER_VA_COPY_REQUIRED 1
		#endif
	#endif


	// EA_COMPILER_NO_RTTI
	//
	// If EA_COMPILER_NO_RTTI is defined, then RTTI (run-time type information)
	// is not available (possibly due to being disabled by the user).
	//
	#if defined(__EDG_VERSION__) && !defined(__RTTI)
		#define EA_COMPILER_NO_RTTI 1
	#elif defined(__clang__) && !EA_COMPILER_HAS_FEATURE(cxx_rtti)
		#define EA_COMPILER_NO_RTTI 1
	#elif defined(__IBMCPP__) && !defined(__RTTI_ALL__)
		#define EA_COMPILER_NO_RTTI 1
	#elif defined(__GXX_ABI_VERSION) && !defined(__GXX_RTTI)
		#define EA_COMPILER_NO_RTTI 1
	#elif defined(_MSC_VER) && !defined(_CPPRTTI)
		#define EA_COMPILER_NO_RTTI 1
	#elif defined(__ARMCC_VERSION) && defined(__TARGET_CPU_MPCORE) && !defined(__RTTI)
		#define EA_COMPILER_NO_RTTI 1
	#endif



	// EA_COMPILER_NO_EXCEPTIONS / EA_COMPILER_NO_UNWIND
	//
	// If EA_COMPILER_NO_EXCEPTIONS is defined, then the compiler is 
	// configured to not recognize C++ exception-handling statements 
	// such as try/catch/throw. Thus, when EA_COMPILER_NO_EXCEPTIONS is 
	// defined, code that attempts to use exception handling statements
	// will usually cause a compilation error. If is often desirable
	// for projects to disable exception handling because exception 
	// handling causes extra code and/or data generation which might
	// not be needed, especially if it is known that exceptions won't
	// be happening. When writing code that is to be portable between
	// systems of which some enable exception handling while others
	// don't, check for EA_COMPILER_NO_EXCEPTIONS being defined. 
	//
	#if !defined(EA_COMPILER_NO_EXCEPTIONS) && !defined(EA_COMPILER_NO_UNWIND)
		#if defined(EA_COMPILER_GNUC) && defined(_NO_EX) // GCC on some platforms defines _NO_EX when exceptions are disabled.
			#define EA_COMPILER_NO_EXCEPTIONS 1

		#elif (defined(EA_COMPILER_CLANG) || defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_INTEL) || defined(EA_COMPILER_RVCT)) && !defined(__EXCEPTIONS) // GCC and most EDG-based compilers define __EXCEPTIONS when exception handling is enabled.
			#define EA_COMPILER_NO_EXCEPTIONS 1

		#elif (defined(EA_COMPILER_MSVC)) && !defined(_CPPUNWIND)
			#define EA_COMPILER_NO_UNWIND 1

		#endif // EA_COMPILER_NO_EXCEPTIONS / EA_COMPILER_NO_UNWIND
	#endif // !defined(EA_COMPILER_NO_EXCEPTIONS) && !defined(EA_COMPILER_NO_UNWIND)


	// ------------------------------------------------------------------------
	// EA_DISABLE_ALL_VC_WARNINGS / EA_RESTORE_ALL_VC_WARNINGS
	// 
	// Disable and re-enable all warning(s) within code.
	//
	// Example usage:
	//     EA_DISABLE_ALL_VC_WARNINGS()
	//     <code>
	//     EA_RESTORE_ALL_VC_WARNINGS()
	//
	//This is duplicated from EABase's eacompilertraits.h
	#ifndef EA_DISABLE_ALL_VC_WARNINGS
		#if defined(_MSC_VER)
			#define EA_DISABLE_ALL_VC_WARNINGS()  \
				__pragma(warning(push, 0)) \
				__pragma(warning(disable: 4244 4265 4267 4350 4472 4509 4548 4623 4710 4985 6320 4755 4625 4626 4702)) // Some warnings need to be explicitly called out.
		#else
			#define EA_DISABLE_ALL_VC_WARNINGS()
		#endif
	#endif

	//This is duplicated from EABase's eacompilertraits.h
	#ifndef EA_RESTORE_ALL_VC_WARNINGS
		#if defined(_MSC_VER)
			#define EA_RESTORE_ALL_VC_WARNINGS()  \
				__pragma(warning(pop))
		#else
			#define EA_RESTORE_ALL_VC_WARNINGS()
		#endif
	#endif

	// Dinkumware
	//This is duplicated from EABase's eahave.h
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


	// EA_COMPILER_NO_ALIGNED_NEW
	//
	//
	#if !defined(EA_COMPILER_NO_ALIGNED_NEW)
		#if defined(_HAS_ALIGNED_NEW) && _HAS_ALIGNED_NEW // VS2017 15.5 Preview 
			// supported.
		#elif defined(EA_COMPILER_CPP17_ENABLED)
			// supported.
		#else
			#define EA_COMPILER_NO_ALIGNED_NEW 1
		#endif
	#endif

	// EA_COMPILER_NO_NEW_THROW_SPEC / EA_THROW_SPEC_NEW / EA_THROW_SPEC_DELETE
	//
	// If defined then the compiler's version of operator new is not decorated
	// with a throw specification. This is useful for us to know because we 
	// often want to write our own overloaded operator new implementations.
	// We need such operator new overrides to be declared identically to the
	// way the compiler is defining operator new itself.
	//
	// Example usage:
	//      void* operator new(std::size_t) EA_THROW_SPEC_NEW(std::bad_alloc);
	//      void* operator new[](std::size_t) EA_THROW_SPEC_NEW(std::bad_alloc);
	//      void* operator new(std::size_t, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE();
	//      void* operator new[](std::size_t, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE();
	//      void  operator delete(void*) EA_THROW_SPEC_DELETE_NONE();
	//      void  operator delete[](void*) EA_THROW_SPEC_DELETE_NONE();
	//      void  operator delete(void*, const std::nothrow_t&) EA_THROW_SPEC_DELETE_NONE();
	//      void  operator delete[](void*, const std::nothrow_t&) EA_THROW_SPEC_DELETE_NONE();
	//
	#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY)
		#if defined(_MSC_VER) && (_MSC_VER >= 1912)  // VS2017 15.3+ 
			#define EA_THROW_SPEC_NEW(x)        noexcept(false)
			#define EA_THROW_SPEC_NEW_NONE()    noexcept 
			#define EA_THROW_SPEC_DELETE_NONE() noexcept 

		#elif defined(_MSC_VER) && (_MSC_VER >= 1910)  // VS2017+
			#define EA_THROW_SPEC_NEW(x)        throw(x)
			#define EA_THROW_SPEC_NEW_NONE()    throw() 
			#define EA_THROW_SPEC_DELETE_NONE() throw() 

		#else
			#if defined(EA_PLATFORM_SONY)
				#define EA_THROW_SPEC_NEW(X)        _THROWS(X)
			#elif defined(_MSC_VER)
				// Disabled warning "nonstandard extension used: 'throw (...)'" as this warning is a W4 warning which is usually off by default
				// and doesn't convey any important information but will still complain when building with /Wall (which most teams do)
				#define EA_THROW_SPEC_NEW(X)        __pragma(warning(push)) __pragma(warning(disable: 4987)) _THROWS(X) __pragma(warning(pop))
			#else
				#define EA_THROW_SPEC_NEW(X)        _THROW1(X)
			#endif
			#define EA_THROW_SPEC_NEW_NONE()    _THROW0()
			#define EA_THROW_SPEC_DELETE_NONE() _THROW0()

		#endif
	#elif defined(EA_COMPILER_NO_EXCEPTIONS) && !defined(EA_COMPILER_RVCT) && !defined(EA_PLATFORM_LINUX) && !defined(EA_PLATFORM_APPLE) && !defined(CS_UNDEFINED_STRING)
		#define EA_COMPILER_NO_NEW_THROW_SPEC 1

		#define EA_THROW_SPEC_NEW(x)
		#define EA_THROW_SPEC_NEW_NONE()
		#define EA_THROW_SPEC_DELETE_NONE()
	#else
		#define EA_THROW_SPEC_NEW(x)        throw(x)
		#define EA_THROW_SPEC_NEW_NONE()    throw()
		#define EA_THROW_SPEC_DELETE_NONE() throw()
	#endif


	// EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	//
	// If defined, then the compiler doesn't provide a Standard C++ library.
	//
	#if defined(EA_PLATFORM_ANDROID)
		// Disabled because EA's eaconfig/android_config/android_sdk packages currently 
		// don't support linking STL libraries. Perhaps we can figure out what linker arguments
		// are needed for an app so we can manually specify them and then re-enable this code.
		//#include <android/api-level.h>
		//
		//#if (__ANDROID_API__ < 9) // Earlier versions of Android provide no std C++ STL implementation.
			#define EA_COMPILER_NO_STANDARD_CPP_LIBRARY 1
		//#endif
	#endif


	// EA_COMPILER_NO_STATIC_VARIABLE_INIT
	//
	// If defined, it means that global or static C++ variables will be 
	// constructed. Not all compiler/platorm combinations support this. 
	// User code that needs to be portable must avoid having C++ variables
	// that construct before main. 
	//
	//#if defined(EA_PLATFORM_MOBILE)
	//    #define EA_COMPILER_NO_STATIC_VARIABLE_INIT 1
	//#endif


	// EA_COMPILER_NO_STATIC_FUNCTION_INIT
	//
	// If defined, it means that functions marked as startup functions
	// (e.g. __attribute__((constructor)) in GCC) are supported. It may
	// be that some compiler/platform combinations don't support this.
	//
	//#if defined(XXX) // So far, all compiler/platforms we use support this.
	//    #define EA_COMPILER_NO_STATIC_VARIABLE_INIT 1
	//#endif

	// EA_COMPILER_NO_VARIADIC_MACROS
	// 
	// If defined, the compiler doesn't support C99/C++11 variadic macros.
	// With a variadic macro, you can do this:
	//     #define MY_PRINTF(format, ...) printf(format, __VA_ARGS__)
	//
	#if !defined(EA_COMPILER_NO_VARIADIC_MACROS)
		#if defined(_MSC_VER) && (_MSC_VER < 1500) // If earlier than VS2008..
			#define EA_COMPILER_NO_VARIADIC_MACROS 1
		#elif defined(__GNUC__) && (((__GNUC__ * 100) + __GNUC_MINOR__)) < 401 // If earlier than GCC 4.1..
			#define EA_COMPILER_NO_VARIADIC_MACROS 1
		#elif defined(EA_COMPILER_EDG) // Includes other compilers
			// variadic macros are supported
		#endif
	#endif


	// EA_COMPILER_NO_RVALUE_REFERENCES
	// 
	// If defined, the compiler doesn't fully support C++11 rvalue reference semantics.
	// This applies to the compiler only and not the Standard Library in use with the compiler,
	// which is required by the Standard to have some support itself.
	//
	#if !defined(EA_COMPILER_NO_RVALUE_REFERENCES)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (_MSC_VER >= 1600)                // VS2010+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403) // EDG 4.3+. 
			// supported. Earlier EDG supported a subset of rvalue references. Implicit move constructors and assignment operators aren't supported until EDG 4.5.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && EA_COMPILER_HAS_FEATURE(cxx_rvalue_references)
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4005)   // GCC 4.5+
			// supported.
		#else
			#define EA_COMPILER_NO_RVALUE_REFERENCES 1
		#endif
	#endif


	// EA_COMPILER_NO_EXTERN_TEMPLATE
	// 
	// If defined, the compiler doesn't support C++11 extern template.
	// With extern templates, you can do this:
	//     extern template void DoSomething(KnownType u);
	//
	#if !defined(EA_COMPILER_NO_EXTERN_TEMPLATE)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (_MSC_VER >= 1700)                 // VS2012+...
			// Extern template is supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)  // EDG 4.1+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && defined(__apple_build_version__) && (EA_COMPILER_VERSION >= 401)
			// Extern template is supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && !defined(__apple_build_version__)             // Clang other than Apple's Clang
			// Extern template is supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4006)    // GCC 4.6+
			// Extern template is supported.
		#else
			#define EA_COMPILER_NO_EXTERN_TEMPLATE 1
		#endif
	#endif


	// EA_COMPILER_NO_RANGE_BASED_FOR_LOOP
	// 
	// If defined, the compiler doesn't support C++11 range-based for loops.
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2930.html
	// You must #include <iterator> for range-based for loops to work.
	// Example usage:
	//    #include <iterator>
	//    #include <vector>
	//    std::vector<float> floatVector;
	//    for(float& f : floatVector)
	//        f += 1.0;
	//
	#if !defined(EA_COMPILER_NO_RANGE_BASED_FOR_LOOP)
		#if defined(EA_COMPILER_CPP11_ENABLED) && (defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1700))      // VS2012+...
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405)  // EDG 4.5+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && (defined(__clang__)  && (EA_COMPILER_VERSION >=  300))  // Clang 3.x+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && (defined(__GNUC__) && (EA_COMPILER_VERSION >= 4006))  // GCC 4.6+
			// supported.
		#else
			#define EA_COMPILER_NO_RANGE_BASED_FOR_LOOP 1
		#endif
	#endif


	// EA_COMPILER_NO_CONSTEXPR
	//
	// Refers to C++11 = constexpr (const expression) declarations.
	//
	#if !defined(EA_COMPILER_NO_CONSTEXPR)
		#if defined(EA_COMPILER_CPP11_ENABLED) && (defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1900)) // VS2015+... Not present in VC++ up to and including VS2013.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 406) // EDG 4.6+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && EA_COMPILER_HAS_FEATURE(cxx_constexpr)
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4006) // GCC 4.6+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1900) // VS 2015+
			// supported.
		#else
			#define EA_COMPILER_NO_CONSTEXPR 1
		#endif
	#endif


	// EA_COMPILER_NO_CONSTEXPR_IF
	//
	// Refers to C++17 = constexpr if(const expression) conditionals.
	//
	#if !defined(EA_COMPILER_NO_CONSTEXPR_IF)
		#if defined(EA_COMPILER_CPP17_ENABLED) && (defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1911)) // VS2017 15.3+
			// supported.
		#elif defined(EA_COMPILER_CPP17_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 309) // Clang 3.9+
			// supported.
		#elif defined(EA_COMPILER_CPP17_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 7000) // GCC 7+
			// supported.
		#else
			#define EA_COMPILER_NO_CONSTEXPR_IF 1
		#endif
	#endif
	

	// EA_COMPILER_NO_OVERRIDE
	// 
	// Refers to the C++11 override specifier.
	//
	#ifndef EA_COMPILER_NO_OVERRIDE
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION > 1600)  // VC++ > VS2010, even without C++11 support. VS2010 does support override, however will generate warnings due to the keyword being 'non-standard'
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 209)    // Clang 2.9+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4007)  // GCC 4.7+
			// supported.
		#else
			#define EA_COMPILER_NO_OVERRIDE 1 
		#endif
	#endif


	// EA_COMPILER_NO_INHERITANCE_FINAL
	// 
	// Refers to the C++11 final specifier.
	//
	#ifndef EA_COMPILER_NO_INHERITANCE_FINAL
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1500)  // VS2008+, even without C++11 support.
			// supported, though you need to use EA_INHERITANCE_FINAL for it to work with VS versions prior to 2012.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >=  209)   // Clang 2.9+
			// supported
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4007)  // GCC 4.7+
			// supported
		#else
			#define EA_COMPILER_NO_INHERITANCE_FINAL 1
		#endif
	#endif


	// EA_COMPILER_NO_AUTO
	//
	// Refers to C++11 auto.
	//
	#if !defined(EA_COMPILER_NO_AUTO)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1600)     // VS2010+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401) // EDG 4.1+.
			// supported with the exception of the usage of braced initializer lists as of EDG 4.3.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 209)   // Clang 2.9+, including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4004)   // GCC 4.4+
			// supported.
		#else
			#define EA_COMPILER_NO_AUTO 1
		#endif
	#endif


	// EA_COMPILER_NO_NULLPTR
	//
	// Refers to C++11 nullptr (which is a built in type). std::nullptr_t is defined in C++11 <cstddef>.
	// Note that <EABase/nullptr.h> implements a portable nullptr implementation.
	//
	#if !defined(EA_COMPILER_NO_NULLPTR)
		#if (defined(_MSC_VER) && (_MSC_VER >= 1600)) && defined(EA_COMPILER_CPP11_ENABLED)
			// supported
		#elif defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4006) && defined(EA_COMPILER_CPP11_ENABLED)
			// supported
		#elif  defined(__clang__) && defined(EA_COMPILER_CPP11_ENABLED)
			// supported
		#elif defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403) && defined(EA_COMPILER_CPP11_ENABLED)
			// supported
		#else
			#define EA_COMPILER_NO_NULLPTR 1
		#endif
	#endif


	// EA_COMPILER_NO_DECLTYPE
	//
	// Refers to C++11 decltype.
	//
	#if !defined(EA_COMPILER_NO_DECLTYPE)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1600)     // VS2010+
			// supported, though VS2010 doesn't support the spec completely as specified in the final standard.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401) // EDG 4.1+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 209)   // Clang 2.9+, including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4003)   // GCC 4.3+
			// supported.
		#else
			#define EA_COMPILER_NO_DECLTYPE 1
		#endif
	#endif



	// EA_COMPILER_NO_DEFAULTED_FUNCTIONS
	// EA_COMPILER_NO_DELETED_FUNCTIONS
	//
	// Refers to C++11 = default and = delete function declarations.
	//
	#if !defined(EA_COMPILER_NO_DEFAULTED_FUNCTIONS)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1800)     // VS2013+
			// supported, but as of VS2013 it isn't supported for defaulted move constructors and move assignment operators.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401) // EDG 4.1+.
			// supported, but as of EDG 4.3 it isn't supported for defaulted move constructors and move assignment operators until EDG 4.5.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >=  300)    // Clang 3.0+, including Apple's Clang
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4004)   // GCC 4.4+
			// supported.
		#else
			// VC++ doesn't support it as of VS2012.
			#define EA_COMPILER_NO_DEFAULTED_FUNCTIONS 1
		#endif
	#endif

	#if !defined(EA_COMPILER_NO_DELETED_FUNCTIONS)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1800)     // VS2013+
			// supported, but as of VS2013 it isn't supported for defaulted move constructors and move assignment operators.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401) // EDG 4.1+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >=  209)    // Clang 2.9+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4004)   // GCC 4.4+
			// supported.
		#else
			// VC++ doesn't support it as of VS2012.
			#define EA_COMPILER_NO_DELETED_FUNCTIONS 1
		#endif
	#endif


	// EA_COMPILER_NO_LAMBDA_EXPRESSIONS
	//
	// Refers to C++11 lambda expressions.
	//
	#if !defined(EA_COMPILER_NO_LAMBDA_EXPRESSIONS)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1600)     // VS2010+
			// supported, though VS2010 doesn't support the spec completely as specified in the final standard.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401) // EDG 4.1+.
			// supported. However, converting lambdas to function pointers is not supported until EDG 4.5.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 301) && !defined(__apple_build_version__)  // Clang 3.1+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4004)   // GCC 4.4+
			// supported.
		#else
			#define EA_COMPILER_NO_LAMBDA_EXPRESSIONS 1
		#endif
	#endif


	// EA_COMPILER_NO_TRAILING_RETURN_TYPES
	//
	// Refers to C++11 trailing-return-type. Also sometimes referred to as "incomplete return type".
	//
	#if !defined(EA_COMPILER_NO_TRAILING_RETURN_TYPES)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1600)     // VS2010+
			// supported, though VS2010 doesn't support the spec completely as specified in the final standard.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 402) // EDG 4.2+.
			// supported. However, use of "this" in trailing return types is not supported untiil EDG 4.4
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 301) && !defined(__apple_build_version__)  // Clang 3.1+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4004)   // GCC 4.4+
			// supported.
		#else
			#define EA_COMPILER_NO_TRAILING_RETURN_TYPES 1
		#endif
	#endif


	// EA_COMPILER_NO_STRONGLY_TYPED_ENUMS
	//
	// Refers to C++11 strongly typed enums, which includes enum classes and sized enums. Doesn't include forward-declared enums.
	//
	#if !defined(EA_COMPILER_NO_STRONGLY_TYPED_ENUMS)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1700)     // VS2012+
			// supported. A subset of this is actually supported by VS2010.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 400) // EDG 4.0+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 209)   // Clang 2.9+, including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4004)   // GCC 4.4+
			// supported.
		#else
			#define EA_COMPILER_NO_STRONGLY_TYPED_ENUMS 1
		#endif
	#endif


	// EA_COMPILER_NO_FORWARD_DECLARED_ENUMS
	//
	// Refers to C++11 forward declared enums.
	//
	#if !defined(EA_COMPILER_NO_FORWARD_DECLARED_ENUMS)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1700)     // VS2012+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405) // EDG 4.5+.
			// supported. EDG 4.3 supports basic forward-declared enums, but not forward-declared strongly typed enums.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 301) && !defined(__apple_build_version__)  // Clang 3.1+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4006)   // GCC 4.6+
			// supported.
		#else
			#define EA_COMPILER_NO_FORWARD_DECLARED_ENUMS 1
		#endif
	#endif


	// EA_COMPILER_NO_VARIADIC_TEMPLATES
	//
	// Refers to C++11 variadic templates.
	//
	#if !defined(EA_COMPILER_NO_VARIADIC_TEMPLATES)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1800)     // VS2013+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (_MSC_FULL_VER == 170051025)    // VS2012 November Preview for Windows only.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403) // EDG 4.3+.
			// supported, though 4.1 has partial support for variadic templates.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 209)   // Clang 2.9+, including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4004)   // GCC 4.4+
			// supported, though GCC 4.3 has partial support for variadic templates.
		#else
			#define EA_COMPILER_NO_VARIADIC_TEMPLATES 1
		#endif
	#endif


	// EA_COMPILER_NO_TEMPLATE_ALIASES
	//
	// Refers to C++11 alias templates.
	// Example alias template usage:
	//     template <typename T>
	//     using Dictionary = eastl::map<eastl::string, T>;
	//
	//     Dictionary<int> StringIntDictionary;
	//
	#if !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1800)     // VS2013+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 402) // EDG 4.2+.
			// supported, though 4.1 has partial support for variadic templates.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 300) && !defined(__apple_build_version__) // Clang 3.0+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4007)   // GCC 4.7+
			// supported, though GCC 4.3 has partial support for variadic templates.
		#else
			#define EA_COMPILER_NO_TEMPLATE_ALIASES 1
		#endif
	#endif


	// EA_COMPILER_NO_VARIABLE_TEMPLATES
	//
	// Refers to C++14 variable templates.
	// Example variable template usage:
	//     template<class T>
	//     constexpr T pi = T(3.1415926535897932385);
	//
	#if !defined(EA_COMPILER_NO_VARIABLE_TEMPLATES)
		#if defined(_MSC_VER) && (_MSC_FULL_VER >= 190023918)    // VS2015 Update 2 and above.
			// supported.
		#elif defined(EA_COMPILER_CPP14_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 304) && !defined(__apple_build_version__)    // Clang 3.4+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP14_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 5000)   // GCC 5+
			// supported.
		#elif !defined(EA_COMPILER_CPP14_ENABLED) 
			#define EA_COMPILER_NO_VARIABLE_TEMPLATES 1
		#endif
	#endif


	// EA_COMPILER_NO_INLINE_VARIABLES
	//
	// Refers to C++17 inline variables that allows the definition of variables in header files
	//
	// Example usage:
	//    struct Foo 
	//    {
	//        static inline constexpr int kConstant = 42;  // no out of class definition
	//    };
	//
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4424.pdf
	// http://en.cppreference.com/w/cpp/language/inline
	//
	#if !defined(EA_COMPILER_NO_INLINE_VARIABLES)
		#define EA_COMPILER_NO_INLINE_VARIABLES 1
	#endif


	// EA_COMPILER_NO_INITIALIZER_LISTS
	//
	// Refers to C++11 initializer lists.
	// This refers to the compiler support for this and not the Standard Library support (std::initializer_list).
	//
	#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1800)     // VS2013+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (_MSC_FULL_VER == 170051025)    // VS2012 November Preview for Windows only.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405) // EDG 4.5+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 301) && !defined(__apple_build_version__) // Clang 3.1+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4004)   // GCC 4.4+
			// supported, though GCC 4.3 has partial support for it.
		#else
			#define EA_COMPILER_NO_INITIALIZER_LISTS 1
		#endif
	#endif


	// EA_COMPILER_NO_NORETURN
	//
	// Refers to C++11 declaration attribute: noreturn.
	// http://en.cppreference.com/w/cpp/language/attributes
	// http://blog.aaronballman.com/2011/09/understanding-attributes/
	//
	#if !defined(EA_COMPILER_NO_NORETURN)
		#if defined(EA_COMPILER_MSVC) && (EA_COMPILER_VERSION >= 1300)                                   // VS2003+
			// supported via __declspec(noreturn). You need to use that or EA_NORETURN. VC++ up to VS2013 doesn't support any C++11 attribute types.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 402) // EDG 4.2+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 300) && !defined(__apple_build_version__) // Clang 3.0+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4008)   // GCC 4.8+
			// supported.
		#else
			#define EA_COMPILER_NO_NORETURN 1
		#endif
	#endif


	// EA_COMPILER_NO_CARRIES_DEPENDENCY
	// 
	// Refers to C++11 declaration attribute: carries_dependency.
	// http://en.cppreference.com/w/cpp/language/attributes
	// http://blog.aaronballman.com/2011/09/understanding-attributes/
	//
	#if !defined(EA_COMPILER_NO_CARRIES_DEPENDENCY)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)    // Apple clang 4.1+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 402) // EDG 4.2+.
			// supported; stricter than other compilers in its usage.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 300) && !defined(__apple_build_version__) // Clang 3.0+, not including Apple's Clang.
			// supported.
		// Currently GNUC doesn't appear to support this attribute.
		//#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4008)                        // GCC 4.8+
		//    // supported.
		#else
			#define EA_COMPILER_NO_CARRIES_DEPENDENCY 1
		#endif
	#endif


	// EA_COMPILER_NO_FALLTHROUGH
	// 
	// Refers to C++17 declaration attribute: fallthrough.
	// http://en.cppreference.com/w/cpp/language/attributes
	//
	#if !defined(EA_COMPILER_NO_FALLTHROUGH)
		#if defined(EA_COMPILER_CPP17_ENABLED) 
			// supported.
		#else
			#define EA_COMPILER_NO_FALLTHROUGH 1
		#endif
	#endif


	// EA_COMPILER_NO_NODISCARD
	// 
	// Refers to C++17 declaration attribute: nodiscard.
	// http://en.cppreference.com/w/cpp/language/attributes
	//
	#if !defined(EA_COMPILER_NO_NODISCARD)
		#if defined(EA_COMPILER_CPP17_ENABLED) 
			// supported.
		#else
			#define EA_COMPILER_NO_NODISCARD 1
		#endif
	#endif


	// EA_COMPILER_NO_MAYBE_UNUSED
	// 
	// Refers to C++17 declaration attribute: maybe_unused.
	// http://en.cppreference.com/w/cpp/language/attributes
	//
	#if !defined(EA_COMPILER_NO_MAYBE_UNUSED)
		#if defined(EA_COMPILER_CPP17_ENABLED) 
			// supported.
		#elif defined(EA_COMPILER_MSVC) && (EA_COMPILER_VERSION >= 1912) // VS2017 15.3+
			// supported.
		#else
			#define EA_COMPILER_NO_MAYBE_UNUSED 1
		#endif
	#endif


	// EA_COMPILER_NO_STRUCTURED_BINDING
	//
	// Indicates if target compiler supports the C++17 "structured binding" language feature.
	// https://en.cppreference.com/w/cpp/language/structured_binding
	//
	//
	#if !defined(EA_COMPILER_NO_STRUCTURED_BINDING)
		#if defined(EA_COMPILER_CPP17_ENABLED) 
			// supported.
		#elif defined(EA_COMPILER_MSVC) && (EA_COMPILER_VERSION >= 1912) // VS2017 15.3+
			// supported.
		#else
			#define EA_COMPILER_NO_STRUCTURED_BINDING 1
		#endif
	#endif


	// EA_COMPILER_NO_DESIGNATED_INITIALIZERS
	//
	// Indicates the target compiler supports the C++20 "designated initializer" language feature.
	// https://en.cppreference.com/w/cpp/language/aggregate_initialization
	//
	// Example:
	//   struct A { int x; int y; };
	//   A a = { .y = 42, .x = 1 };
	//
	#if !defined(EA_COMPILER_NO_DESIGNATED_INITIALIZERS)
		#if defined(EA_COMPILER_CPP20_ENABLED)
			// supported.
		#else
			#define EA_COMPILER_NO_DESIGNATED_INITIALIZERS 1
		#endif
	#endif


	// EA_COMPILER_NO_NONSTATIC_MEMBER_INITIALIZERS
	//
	// Refers to C++11 declaration attribute: carries_dependency.
	// http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2008/n2756.htm
	//
	#if !defined(EA_COMPILER_NO_NONSTATIC_MEMBER_INITIALIZERS)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1800)                          // VS2013+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)  // Apple clang 4.1+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 300) && !defined(__apple_build_version__) // Clang 3.0+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4007)   // GCC 4.7+
			// supported.
		#else
			#define EA_COMPILER_NO_NONSTATIC_MEMBER_INITIALIZERS 1
		#endif
	#endif


	// EA_COMPILER_NO_RIGHT_ANGLE_BRACKETS
	//
	// Defines if the compiler supports >> (as opposed to > >) in template 
	// declarations such as typedef eastl::list<eastl::list<int>> ListList;
	//
	#if !defined(EA_COMPILER_NO_RIGHT_ANGLE_BRACKETS)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1600)     // VS2010+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401) // EDG 4.1+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 209)   // Clang 2.9+, including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4003)   // GCC 4.3+
			// supported.
		#else
			#define EA_COMPILER_NO_RIGHT_ANGLE_BRACKETS 1
		#endif
	#endif


	// EA_COMPILER_NO_ALIGNOF
	//
	// Refers specifically to C++11 alignof and not old compiler extensions such as __alignof__(). 
	// However, EABase provides a portable EA_ALIGN_OF which works for all compilers. 
	//
	#if !defined(EA_COMPILER_NO_ALIGNOF)
		// Not supported by VC++ as of VS2013, though EA_ALIGN_OF is supported on all coompilers as an alternative.
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 209)    // Clang 2.9+, including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4005)  // GCC 4.5+
			// supported.
		#else
			#define EA_COMPILER_NO_ALIGNOF 1
		#endif
	#endif


	// EA_COMPILER_NO_ALIGNAS
	//
	// Refers to C++11 alignas.
	//
	#if !defined(EA_COMPILER_NO_ALIGNAS)
		// Not supported by VC++ as of VS2013.
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)    // Apple clang 4.1+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 300) && !defined(__apple_build_version__) // Clang 3.0+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4008)   // GCC 4.8+
			// supported.
		#else
			#define EA_COMPILER_NO_ALIGNAS 1
		#endif
	#endif


	// EA_COMPILER_NO_DELEGATING_CONSTRUCTORS
	//
	// Refers to C++11 constructor delegation.
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n1986.pdf
	// https://www.ibm.com/developerworks/mydeveloperworks/blogs/5894415f-be62-4bc0-81c5-3956e82276f3/entry/c_0x_delegating_constructors
	//
	#if !defined(EA_COMPILER_NO_DELEGATING_CONSTRUCTORS)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1800)                          // VS2013+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 407) // EDG 4.7+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)  // Apple clang 4.1+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 300) && !defined(__apple_build_version__) // Clang 3.0+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4007)   // GCC 4.7+
			// supported.
		#else
			#define EA_COMPILER_NO_DELEGATING_CONSTRUCTORS 1
		#endif
	#endif


	// EA_COMPILER_NO_INHERITING_CONSTRUCTORS
	//
	// Refers to C++11 constructor inheritance via 'using'.
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2540.htm
	//
	#if !defined(EA_COMPILER_NO_INHERITING_CONSTRUCTORS)
		// Not supported by VC++ as of VS2013.
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && EA_COMPILER_HAS_FEATURE(cxx_inheriting_constructors)    // Clang
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4008)   // GCC 4.8+
			// supported.
		#else
			#define EA_COMPILER_NO_INHERITING_CONSTRUCTORS 1
		#endif
	#endif


	// EA_COMPILER_NO_USER_DEFINED_LITERALS
	//
	// http://en.cppreference.com/w/cpp/language/user_literal
	// http://stackoverflow.com/questions/237804/what-new-capabilities-do-user-defined-literals-add-to-c
	//
	#if !defined(EA_COMPILER_NO_USER_DEFINED_LITERALS)
		// Not supported by VC++ as of VS2013.
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)    // Apple clang 4.1+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 301) && !defined(__apple_build_version__) // Clang 3.1+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4007)   // GCC 4.7+
			// supported.
		#else
			#define EA_COMPILER_NO_USER_DEFINED_LITERALS 1
		#endif
	#endif


	// EA_COMPILER_NO_STANDARD_LAYOUT_TYPES
	//     a.k.a. POD relaxation
	//     http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2342.htm
	//
	#if !defined(EA_COMPILER_NO_STANDARD_LAYOUT_TYPES)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1700)                            // VS2012+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)    // Apple clang 4.1+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 300) && !defined(__apple_build_version__) // Clang 3.0+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4005)   // GCC 4.5+
			// supported.
		#else
			#define EA_COMPILER_NO_STANDARD_LAYOUT_TYPES 1
		#endif
	#endif


	// EA_COMPILER_NO_EXTENDED_SIZEOF
	//
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2253.html
	// Allows you to do this: sizeof(SomeClass::mSomeMember)
	//
	#if !defined(EA_COMPILER_NO_EXTENDED_SIZEOF)
		// Not supported by VC++ as of VS2013.
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)    // Apple clang 4.1+
			// supported.
		// Versions of EDG prior to 4.5 only support extended sizeof in non-member functions. Full support was added in 4.5
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405) // EDG 4.5+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 301) && !defined(__apple_build_version__) // Clang 3.1+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4005)   // GCC 4.5+
			// supported.
		#else
			#define EA_COMPILER_NO_EXTENDED_SIZEOF 1
		#endif
	#endif


	// EA_COMPILER_NO_INLINE_NAMESPACES
	//
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2535.htm
	// http://blog.aaronballman.com/2011/07/inline-namespaces/
	//
	#if !defined(EA_COMPILER_NO_INLINE_NAMESPACES)
		// Not supported by VC++ as of VS2013.
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405) // EDG 4.5+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 209)   // Clang 2.9+, including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4004) // GCC 4.4+
			// supported.
		#else
			#define EA_COMPILER_NO_INLINE_NAMESPACES 1
		#endif
	#endif


	// EA_COMPILER_NO_UNRESTRICTED_UNIONS
	//
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2544.pdf
	//
	#if !defined(EA_COMPILER_NO_UNRESTRICTED_UNIONS)
		// Not supported by VC++ as of VS2013.
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 406) // EDG 4.6+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)  // Apple clang 4.1+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 301) && !defined(__apple_build_version__) // Clang 3.1+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4006)   // GCC 4.6+
			// supported.
		#else
			#define EA_COMPILER_NO_UNRESTRICTED_UNIONS 1
		#endif
	#endif


	// EA_COMPILER_NO_EXPLICIT_CONVERSION_OPERATORS
	//
	// http://en.wikipedia.org/wiki/C%2B%2B11#Explicit_conversion_operators
	//
	#if !defined(EA_COMPILER_NO_EXPLICIT_CONVERSION_OPERATORS)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1800)                          // VS2013+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (_MSC_FULL_VER == 170051025)                         // VS2012 November Preview for Windows only.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 404) // EDG 4.4+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)  // Apple clang 4.1+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 300) && !defined(__apple_build_version__) // Clang 3.0+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4005)   // GCC 4.5+
			// supported.
		#else
			#define EA_COMPILER_NO_EXPLICIT_CONVERSION_OPERATORS 1
		#endif
	#endif


	// EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
	//  
	// The compiler does not support default template arguments for function templates. 
	// http://stackoverflow.com/questions/2447458/default-template-arguments-for-function-templates
	//
	#if !defined(EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1800)        // VS2013+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403)    // EDG 4.4+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 209)      // Clang 2.9+, including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4003) // GCC 4.3+
			// supported.
		#else
			#define EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS 1
		#endif
	#endif


	// EA_COMPILER_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS
	//
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2657.htm
	// http://stackoverflow.com/questions/5751977/local-type-as-template-arguments-in-c
	//
	#if !defined(EA_COMPILER_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1600)     // VS2010+
			// supported.
			#if (EA_COMPILER_VERSION < 1700)    // VS2010 generates a warning, but the C++ language now allows it.
				#pragma warning(disable: 4836) // nonstandard extension used: local types or unnamed types cannot be used as template arguments.
			#endif
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 402) // EDG 4.2+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 209)   // Clang 2.9+, including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4005)   // GCC 4.5+
			// supported.
		#else
			#define EA_COMPILER_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS 1
		#endif
	#endif


	// EA_COMPILER_NO_NOEXCEPT
	//
	// C++11 noexcept
	// http://en.cppreference.com/w/cpp/language/attributes
	// http://en.cppreference.com/w/cpp/language/noexcept
	//
	#if !defined(EA_COMPILER_NO_NOEXCEPT)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1900)     // VS2014+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)    // Apple clang 4.1+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405) // EDG 4.5+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 300) && !defined(__apple_build_version__) // Clang 3.0+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4006)   // GCC 4.6+
			// supported.
		#else
			#define EA_COMPILER_NO_NOEXCEPT 1
		#endif
	#endif


	// EA_COMPILER_NO_RAW_LITERALS
	//
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2442.htm
	// http://en.wikipedia.org/wiki/C%2B%2B11#New_string_literals
	//
	#if !defined(EA_COMPILER_NO_RAW_LITERALS)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1800)                            // VS2013+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 407) // EDG 4.7+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)    // Apple clang 4.1+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 300) && !defined(__apple_build_version__) // Clang 3.0+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4005)   // GCC 4.5+
			// supported.
		#else
			#define EA_COMPILER_NO_RAW_LITERALS 1
		#endif
	#endif


	// EA_COMPILER_NO_UNICODE_STRING_LITERALS
	//
	// http://en.wikipedia.org/wiki/C%2B%2B11#New_string_literals
	//
	#if !defined(EA_COMPILER_NO_UNICODE_STRING_LITERALS)
		// Not supported by VC++ as of VS2013.
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 407) // EDG 4.7+.
			// supported. It's not clear if it's v4.4 or v4.7 that adds this support.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)    // Apple clang 4.1+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 300) && !defined(__apple_build_version__) // Clang 3.0+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4004)   // GCC 4.4+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 407) // EDG 4.7+.
			// supported. It's not clear if it's v4.4 or v4.7 that adds this support.
		#else
			#define EA_COMPILER_NO_UNICODE_STRING_LITERALS 1
		#endif
	#endif


	// EA_COMPILER_NO_NEW_CHARACTER_TYPES
	//
	// Refers to char16_t and char32_t as true native types (and not something simply typedef'd from uint16_t and uint32_t).
	// http://en.cppreference.com/w/cpp/language/types
	//
	#if !defined(EA_COMPILER_NO_NEW_CHARACTER_TYPES)
		#if defined(EA_COMPILER_NO_UNICODE_STRING_LITERALS) // Some compilers have had support for char16_t prior to support for u"", but it's not useful to have the former without the latter.
			#define EA_COMPILER_NO_NEW_CHARACTER_TYPES 1
		#endif
	#endif


	// EA_COMPILER_NO_UNICODE_CHAR_NAME_LITERALS
	//
	// C++ 11 relaxed \u\U sequences in strings.
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2170.html
	//
	#if !defined(EA_COMPILER_NO_UNICODE_CHAR_NAME_LITERALS)
		// VC++ up till at least VS2013 supports \u and \U but supports them wrong with respect to the C++11 Standard.

		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)    // Apple clang 4.1+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 301) && !defined(__apple_build_version__) // Clang 3.1+, not including Apple's Clang.
			// supported. 
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4005)   // GCC 4.5+
			// supported.
		#else
			#define EA_COMPILER_NO_UNICODE_CHAR_NAME_LITERALS 1
		#endif
	#endif


	// EA_COMPILER_NO_UNIFIED_INITIALIZATION_SYNTAX
	//
	// http://en.wikipedia.org/wiki/C%2B%2B11#Uniform_initialization
	//
	#if !defined(EA_COMPILER_NO_UNIFIED_INITIALIZATION_SYNTAX)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1800)                          // VS2013+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 401) && defined(__apple_build_version__)  // Apple clang 4.1+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 301) && !defined(__apple_build_version__) // Clang 3.1+, not including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4004)   // GCC 4.4+
			// supported.
		#else
			#define EA_COMPILER_NO_UNIFIED_INITIALIZATION_SYNTAX 1
		#endif
	#endif


	// EA_COMPILER_NO_EXTENDED_FRIEND_DECLARATIONS
	//
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1791.pdf
	//
	#if !defined(EA_COMPILER_NO_EXTENDED_FRIEND_DECLARATIONS)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1600)     // VS2010+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401) // EDG 4.1+.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && (EA_COMPILER_VERSION >= 209)   // Clang 2.9+, including Apple's Clang.
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4007)   // GCC 4.7+
			// supported.
		#else
			#define EA_COMPILER_NO_EXTENDED_FRIEND_DECLARATIONS 1
		#endif
	#endif


	// EA_COMPILER_NO_THREAD_LOCAL
	//
	// Refers specifically to C++ thread_local, which is like compiler __thread implementations except
	// that it also supports non-trivial classes (e.g. with ctors). EA_COMPILER_NO_THREAD_LOCAL refers
	// specifically to full C++11 thread_local support. The EAThread package provides a wrapper for 
	// __thread via EA_THREAD_LOCAL (which unfortunately sounds like C++ thread_local). 
	//
	// https://en.cppreference.com/w/cpp/keyword/thread_local
	//
	#if !defined(EA_COMPILER_NO_THREAD_LOCAL)
		#if defined(EA_COMPILER_CPP11_ENABLED) && defined(__clang__) && EA_COMPILER_HAS_FEATURE(cxx_thread_local)
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1900)     // VS2015+
			// supported.
		#elif defined(EA_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (EA_COMPILER_VERSION >= 4008)   // GCC 4.8+
			// supported.
		#else
			#define EA_COMPILER_NO_THREAD_LOCAL 1
		#endif
	#endif


#endif // INCLUDED_eacompiler_H





