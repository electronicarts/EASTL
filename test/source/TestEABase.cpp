///////////////////////////////////////////////////////////////////////////////
// TestEABase.cpp
//
// Copyright (c) 2003 Electronic Arts, Inc. -- All Rights Reserved.
// Created by Paul Pedriana.
///////////////////////////////////////////////////////////////////////////////

#include "TestEABase.h"
#include "TestEABase.h" // Intentionally double-include the same header file, to test it.
#include <EABase/eabase.h>
#include <EABase/earesult.h>
#include <EABase/eahave.h>
#include <EABase/nullptr.h>
#include <EABase/eaunits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <cmath>
#include <new>
#include <assert.h>
#include <stdarg.h>
#include <EAMain/EAEntryPointMain.inl>
#include <EATest/EASTLVsnprintf.inl>
#include <EATest/EASTLNewOperatorGuard.inl>
#include <EATest/EATest.h>
#include <EASTL/vector.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/string.h>
#include <EASTL/sort.h>
#include <EASTL/numeric_limits.h>
#include <EAStdC/EAString.h>
#if !defined(EA_COMPILER_NO_STANDARD_CPP_LIBRARY)
EA_DISABLE_ALL_VC_WARNINGS()
	#include <iterator>
EA_RESTORE_ALL_VC_WARNINGS()
#endif

#if defined(EA_COMPILER_MSVC) && defined(EA_PLATFORM_MICROSOFT)
	EA_DISABLE_ALL_VC_WARNINGS()
		#define NOMINMAX
		#include <Windows.h>
	EA_RESTORE_ALL_VC_WARNINGS()
#elif defined(EA_PLATFORM_ANDROID)
	#include <android/log.h>
#endif

#if EA_FP16C
	// Include emmintrin.h so that the test code can try to call one of the intrinsics.
	#include "emmintrin.h"
	#if EA_COMPILER_CLANG
		// On some versions of clang immintrin.h needs to be included to pull in f16c operations.
		#include "immintrin.h"
	#endif
#endif

EA_DISABLE_SN_WARNING(1229) // function is deprecated.
EA_DISABLE_VC_WARNING(4265 4296 4310 4350 4481 4530 4625 4626 4996)


// ------------------------------------------------------------------------
// EA_STATIC_WARNING
//
// ** Temporarily here instead of eabase.h **
//
// Unilaterally prints a message during the compilation pre-processing phase.
// No string quotes are required, and no trailing semicolon should be used.
// As of this writing, clang reports this usage like a warning, but using
// -Wno-#pragma-messages causes both the warning and message to go away.
//
// Example usage:
//     EA_STATIC_WARNING(This function is deprecated.)
//
#if defined(_MSC_VER)
	#define EA_PRAGMA_MESSAGE(x) __pragma(message(#x))
	#define EA_STATIC_WARNING(msg) EA_PRAGMA_MESSAGE(msg)
#elif defined(__clang__) || (defined(__GNUC__) && (EA_COMPILER_VERSION >= 4005)) || defined(__SN_VER__)
	#define EA_PRAGMA(x) _Pragma(#x)
	#define EA_STATIC_WARNING(msg) EA_PRAGMA(message(#msg))
#else
	#define EA_STATIC_WARNING(msg)
#endif




///////////////////////////////////////////////////////////////////////////////
// Exercise EA_HAS_INCLUDE
///////////////////////////////////////////////////////////////////////////////
#if EA_HAS_INCLUDE_AVAILABLE
	#if EA_HAS_INCLUDE(<EASTL/map.h>)
		#include <EASTL/map.h>

		eastl::map<int, int> gTestHasIncludeMap;
	#endif
#endif

#if EA_HAS_INCLUDE_AVAILABLE
	#if EA_HAS_INCLUDE(<DefinitelyDoesNotExist.h>)
		#error "Include Does Not EXIST!"
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// Exercise EAHave
///////////////////////////////////////////////////////////////////////////////

// EA_HAVE_EXTENSIONS_FEATURE
// We don't yet have a test for this.

// EA_HAVE_DINKUMWARE_CPP_LIBRARY
// EA_HAVE_LIBSTDCPP_LIBRARY
// EA_HAVE_LIBCPP_LIBRARY

#if defined(EA_HAVE_DINKUMWARE_CPP_LIBRARY)
	#if !defined(_YVALS)
		#error 
	#endif
#elif defined(EA_HAVE_LIBSTDCPP_LIBRARY)
	#if !defined(__GLIBCXX__) && !defined(__GLIBCPP__)
		#error
	#endif
#elif defined(EA_HAVE_LIBCPP_LIBRARY)
	#if !defined(_LIBCPP_VERSION)
		#error
	#endif
#endif

// EA_HAVE_XXX_H
#if defined(EA_HAVE_SYS_TYPES_H)
	#include <sys/types.h>
#endif
#if defined(EA_HAVE_IO_H)
	#include <io.h>
#endif
#if defined(EA_HAVE_INTTYPES_H)
	#include <inttypes.h>
#endif
#if defined(EA_HAVE_UNISTD_H)
	#include <unistd.h>
#endif
#if defined(EA_HAVE_SYS_TIME_H)
	#include <sys/time.h>
#endif
#if defined(EA_HAVE_SYS_PTRACE_H)
	#include <sys/ptrace.h>
#endif
#if defined(EA_HAVE_SYS_STAT_H)
	#include <sys/stat.h>
#endif
#if defined(EA_HAVE_LOCALE_H)
	#include <locale.h>
#endif
#if defined(EA_HAVE_DIRENT_H)
	#include <dirent.h>
#endif
#if defined(EA_HAVE_SIGNAL_H)
	#include <signal.h>
#endif
#if defined(EA_HAVE_SYS_SIGNAL_H)
	#include <sys/signal.h>
#endif
#if defined(EA_HAVE_PTHREAD_H)
	#include <pthread.h>
#endif
#if defined(EA_HAVE_WCHAR_H)
	#include <wchar.h>
#endif
#if defined(EA_HAVE_MALLOC_H)
	#include <malloc.h>
#endif
#if defined(EA_HAVE_ALLOCA_H)
	#include <alloca.h>
#endif
#if defined(EA_HAVE_EXECINFO_H)
	#include <execinfo.h>
#endif
#if defined(EA_HAVE_SEMAPHORE_H)
	#include <semaphore.h>
#endif

#if defined(EA_HAVE_CPP11_CONTAINERS)
	#include <array>
	#include <forward_list>
	#include <unordered_set>
	#include <unordered_map>
#endif
#if defined(EA_HAVE_CPP11_ATOMIC)
	#include <atomic>
#endif
#if defined(EA_HAVE_CPP11_CONDITION_VARIABLE)
	#include <condition_variable>
#endif
#if defined(EA_HAVE_CPP11_MUTEX) 
	#include <mutex>
#endif
#if defined(EA_HAVE_CPP11_THREAD)
	#if defined(_MSC_VER) && defined(EA_COMPILER_NO_EXCEPTIONS) || defined(EA_COMPILER_NO_UNWIND)
		// Skip this #include, as VC++ has a bug: <concrt.h> (included by <future>) fails to compile when exceptions are disabled.
	#else
		#include <thread>
	#endif
#endif
#if defined(EA_HAVE_CPP11_FUTURE)
	#if defined(_MSC_VER) && defined(EA_COMPILER_NO_EXCEPTIONS) || defined(EA_COMPILER_NO_UNWIND)
		// Skip this #include, as VC++ has a bug: <concrt.h> (included by <future>) fails to compile when exceptions are disabled.
	#else
		#include <future>
	#endif
#endif
#if defined(EA_HAVE_CPP11_TYPE_TRAITS)
	#include <type_traits>
#endif
#if defined(EA_HAVE_CPP11_TUPLES)
	#include <tuple>
#endif
#if defined(EA_HAVE_CPP11_REGEX)
	#include <regex>
#endif
#if defined(EA_HAVE_CPP11_RANDOM)
	#include <random>
#endif
#if defined(EA_HAVE_CPP11_CHRONO)
	#include <chrono> 
#endif
#if defined(EA_HAVE_CPP11_SCOPED_ALLOCATOR)
	#include <scoped_allocator> 
#endif
#if defined(EA_HAVE_CPP11_INITIALIZER_LIST)
	#include <initializer_list>
#else
	// If there is no initializer_list support the the following should succeed.
	// The following is disabled because EASTL defines initializer_list itself and that can collide with this:
	// namespace std{ template<class E> class initializer_list{ }; }
#endif
#if defined(EA_HAVE_CPP11_SYSTEM_ERROR)
	#include <system_error> 
#endif
#if defined(EA_HAVE_CPP11_CODECVT)
	#include <codecvt> 
#endif
#if defined(EA_HAVE_CPP11_TYPEINDEX)
	#include <typeindex> 
#endif



// EA_HAVE_XXX_IMPL
#if defined(EA_HAVE_inet_ntop_IMPL)
	#include <arpa/inet.h>
#endif

#if defined(EA_HAVE_time_IMPL)
	#include <time.h>
#endif

#if defined(EA_HAVE_clock_gettime_IMPL)
	#include <time.h>
#endif

#if defined(EA_HAVE_getcwd_IMPL)
	#if defined(EA_PLATFORM_MICROSOFT)
		#include <direct.h>
	#else
		#include <unistd.h>
	#endif
#endif

#if defined(EA_HAVE_std_terminate_IMPL)
	#include <exception>
#endif

#if defined(EA_HAVE_CPP11_ITERATOR_IMPL)
	#include <iterator>
#endif

#if defined(EA_HAVE_CPP11_SMART_POINTER_IMPL)
	 #include <memory>
#endif

#if defined(EA_HAVE_CPP11_FUNCTIONAL_IMPL)
	#include <functional>
	void BindTestFunction(int /*n1*/, int /*n2*/, int /*n3*/, const int& /*n4*/, int /*n5*/)
	{
	}
	 
	struct BindTestStruct
	{
		void Test(int /*n1*/, int /*n2*/) const
		{
		}
	};
#endif

#if defined(EA_HAVE_CPP11_EXCEPTION_IMPL)
	#include <exception>
#endif


EA_DISABLE_SN_WARNING(1229) // function is deprecated.


// Some CPU/Compiler combinations don't support arbitrary alignment declarations.
// In particular some ARM compilers often don't. You can use EAStdC's EAAlignment to 
// achieve arbitrary alignment if EA_ALIGN doesn't work.
#if (EA_ALIGN_MAX_AUTOMATIC < 64)
	#define ALIGNMENT_AMOUNT_64 EA_ALIGN_MAX_AUTOMATIC
#else
	#define ALIGNMENT_AMOUNT_64 64
#endif

#if (EA_ALIGN_MAX_AUTOMATIC < 32)
	#define ALIGNMENT_AMOUNT_32 EA_ALIGN_MAX_AUTOMATIC
#else
	#define ALIGNMENT_AMOUNT_32 32
#endif

#if (EA_ALIGN_MAX_AUTOMATIC < 16)
	#define ALIGNMENT_AMOUNT_16 EA_ALIGN_MAX_AUTOMATIC
#else
	#define ALIGNMENT_AMOUNT_16 16
#endif


// EA_OVERRIDE
struct OverrideBase { virtual ~OverrideBase(){} virtual void f(int){} };
struct OverrideDerived : public OverrideBase { void f(int) EA_OVERRIDE {} };


// EA_INHERITANCE_FINAL
struct FinalBase EA_INHERITANCE_FINAL { virtual ~FinalBase(){} virtual void f() EA_INHERITANCE_FINAL; };


// EA_SEALED
struct SealedBase EA_SEALED { virtual ~SealedBase(){} virtual void f() EA_SEALED; };


// EA_ABSTRACT
struct AbstractBase EA_ABSTRACT {virtual ~AbstractBase(){} virtual void f(){} };


// EA_CONSTEXPR / EA_COMPILER_NO_CONSTEXPR
EA_CONSTEXPR int GetValue(){ return 37; }


// EA_EXTERN_TEMPLATE / EA_COMPILER_NO_EXTERN_TEMPLATE
template struct eabase_template<char>;


// Forward declarations
template<class T>
bool VerifyValue(T v1, T v2);
void DoError(int& nErrorCount, const char* pMessage = NULL);
int  Stricmp(const char* pString1, const char* pString2);
int  TestEABase();
int  TestEAResult();
int  TestEAPlatform();
bool TestNU();
int  TestEACompiler();
int  TestEACompilerTraits();


template<class T>
bool VerifyValue(T v1, T v2)
{
	return (v1 == v2);
}


// Test EA_PLATFORM_XXX support
// We don't do anything with the defined values below. We are just doing basic testing
// of the usage of #if EA_PLATFORM_XXX
#if EA_PLATFORM_WIN64
	#define EA_PLATFORM_WIN64_OK
#elif EA_PLATFORM_WIN32
	#define EA_PLATFORM_WIN64_OK
#elif EA_PLATFORM_WINDOWS
	#define EA_PLATFORM_WINDOWS_OK
#elif EA_PLATFORM_POSIX
	#define EA_PLATFORM_POSIX_OK
#elif EA_PLATFORM_UNIX
	#define EA_PLATFORM_UNIX_OK
#elif EA_PLATFORM_APPLE
	#define EA_PLATFORM_APPLE_OK
#elif EA_PLATFORM_CONSOLE
	#define EA_PLATFORM_CONSOLE_OK
#elif EA_PLATFORM_DESKTOP
	#define EA_PLATFORM_DESKTOP_OK
#else
	#define EA_PLATFORM_OK
#endif



/* Test EA_DISABLE_WARNING */
EA_DISABLE_VC_WARNING(4548 4127)
EA_DISABLE_ALL_VC_WARNINGS()
EA_RESTORE_ALL_VC_WARNINGS()

EA_DISABLE_GCC_WARNING(-Wuninitialized)

EA_DISABLE_SN_WARNING(1787)
EA_DISABLE_ALL_SN_WARNINGS()
EA_RESTORE_ALL_SN_WARNINGS()

EA_DISABLE_GHS_WARNING(123)

EA_DISABLE_EDG_WARNING(193)

EA_DISABLE_CW_WARNING(10317)
EA_DISABLE_ALL_CW_WARNINGS()
EA_RESTORE_ALL_CW_WARNINGS()

/* Test EA_DISABLE_WARNING */
EA_RESTORE_VC_WARNING()
EA_RESTORE_GCC_WARNING()
EA_RESTORE_SN_WARNING()
EA_RESTORE_GHS_WARNING()
EA_RESTORE_EDG_WARNING()
EA_RESTORE_CW_WARNING(10317)


void DoError(int& nErrorCount, const char* pMessage)
{
	++nErrorCount;
	if(pMessage)
		EA::EAMain::Report("Test error: %s\n", pMessage);
}


int Stricmp(const char* pString1, const char* pString2)
{
	char c1, c2;

	while((c1 = (char)tolower(*pString1++)) == (c2 = (char)tolower(*pString2++)))
	{
		if(c1 == 0)
			return 0;
	}
	
	return (c1 - c2);
}


// EA_PURE
static EA_PURE bool PureFunction()
{
	return (strlen("abc") == 3);
}

// EA_WEAK
EA_WEAK int gWeakVariable = 1;


// EA_NO_VTABLE
struct EA_NO_VTABLE NoVTable1
{
	virtual ~NoVTable1(){}
	virtual void InterfaceFunction()
	{
	}
};

EA_STRUCT_NO_VTABLE(NoVTable2)
{
	virtual ~NoVTable2(){}
	virtual void InterfaceFunction()
	{
	}
};

class NoVTable1Subclass : public NoVTable1
{
	virtual void InterfaceFunction()
	{
	}
};

class NoVTable2Subclass : public NoVTable2
{
	virtual void InterfaceFunction()
	{
	}
};



struct ClassWithDefaultCtor
{
	ClassWithDefaultCtor(int x = 0)
		{ char buffer[16]; sprintf(buffer, "%d", x); }
};


struct ClassWithoutDefaultCtor
{
	ClassWithoutDefaultCtor(int x)
		{ char buffer[16]; sprintf(buffer, "%d", x); }
};


struct InitPriorityTestClass
{
	int mX;
	InitPriorityTestClass(int x = 0) { mX = x; }
};


struct OffsetofTestClass // Intentionally a non-pod.
{
	int32_t mX;
	int32_t mY;
	OffsetofTestClass(int32_t x = 0) : mX(x), mY(0) { }
};

struct SizeofMemberTestClass // Intentionally a non-pod.
{
	int32_t mX;
	int32_t mY;
	SizeofMemberTestClass(int32_t x = 0) : mX(x), mY(0) { }
};

// EA_INIT_PRIORITY
InitPriorityTestClass gInitPriorityTestClass0 EA_INIT_PRIORITY(2000);
InitPriorityTestClass gInitPriorityTestClass1 EA_INIT_PRIORITY(2000) (1);

// EA_INIT_SEG
EA_INIT_SEG(compiler) InitPriorityTestClass gInitSegTestSection(2300);


// EA_MAY_ALIAS
void* EA_MAY_ALIAS gPtr0 = NULL;

typedef void* EA_MAY_ALIAS pvoid_may_alias;
pvoid_may_alias gPtr1 = NULL;


// EA_NO_INLINE
static EA_NO_INLINE void DoNothingInline()
{
}


// EA_PREFIX_NO_INLINE / EA_POSTFIX_NO_INLINE
static void EA_PREFIX_NO_INLINE DoNothingPrefixInline() EA_POSTFIX_NO_INLINE;

static void DoNothingPrefixInline()
{
}


// EA_FORCE_INLINE
static EA_FORCE_INLINE void DoNothingForceInline()
{
}


// EA_PREFIX_FORCE_INLINE / EA_POSTFIX_FORCE_INLINE
static void EA_PREFIX_FORCE_INLINE DoNothingPrefixForceInline() EA_POSTFIX_FORCE_INLINE;

static void DoNothingPrefixForceInline()
{
}


// static_asset at global scope
// Should succeed.
static_assert(sizeof(int32_t) == 4, "static_assert failure");
// Should fail.
//static_assert(sizeof(int32_t) == 8, "static_assert failure");


// EA_STATIC_WARNING
EA_DISABLE_CLANG_WARNING(-W#pragma-messages) // Clang treats messages as warnings.
EA_STATIC_WARNING(EA_STATIC_WARNING test)
EA_RESTORE_CLANG_WARNING()


// EA_OPTIMIZE_OFF / EA_OPTIMIZE_ON
EA_OPTIMIZE_OFF()
static EA_NO_INLINE int DisabledOptimizations(int x)
{
	return x * 37;
}
EA_OPTIMIZE_ON()


// EA_UNUSED
static void FunctionWithUnusedVariables(int x)
{
	int y = 0;
	EA_UNUSED(x);
	EA_UNUSED(y);
}


// EA_NON_COPYABLE / EANonCopyable
struct NonCopyableA
{
	NonCopyableA(){}
	int x;

	EA_NON_COPYABLE(NonCopyableA)
};

EA_DISABLE_VC_WARNING(4625 4626) // C4625: A copy constructor was not accessible in a base class and was therefore not generated for a derived class. C4626: An assignment operator was not accessible in a base class and was therefore not generated for a derived class.
struct NonCopyableB : public EANonCopyable
{
	#if !EA_COMPILER_NO_DELETED_FUNCTIONS
		NonCopyableB& operator=(NonCopyableB&& other) = delete;
	#endif
	int x;
};
EA_RESTORE_VC_WARNING()



// Exercize the case of using EA_NON_COPYABLE when 
struct NonDefaultConstructibleBase
{
	int mX;
	NonDefaultConstructibleBase(int x) : mX(x){}
};

struct NonCopyableSubclass : public NonDefaultConstructibleBase
{
	NonCopyableSubclass(int x) : NonDefaultConstructibleBase(x){}
	EA_NON_COPYABLE(NonCopyableSubclass)
};




// EA_COMPILER_NO_DEFAULTED_FUNCTIONS
// EA_COMPILER_NO_DELETED_FUNCTIONS
//
// We currently test only the ability of the compiler to build the code, 
// and don't test if the compiler built it correctly.
struct DefaultedDeletedTest
{
	#if defined(EA_COMPILER_NO_DEFAULTED_FUNCTIONS)
	   DefaultedDeletedTest(){}
	#else
	   DefaultedDeletedTest() = default;
	#endif

	#if defined(EA_COMPILER_NO_DEFAULTED_FUNCTIONS)
	  ~DefaultedDeletedTest(){}
	#else
	  ~DefaultedDeletedTest() = delete;
	#endif
};


struct EA_FUNCTION_DELETE_Test
{
	int x;
	EA_FUNCTION_DELETE_Test(int xValue) : x(xValue){}

private: // For portability with pre-C++11 compilers, make the function private.
	void foo() EA_FUNCTION_DELETE;
};


#if !defined(EA_COMPILER_NO_USER_DEFINED_LITERALS)
	// Conversion example
	inline long double operator"" _deg(long double degrees)
		{ return (degrees * 3.141592) / 180; }

	// Custom type example
	struct UDLTest
	{
		UDLTest() : mX(0){}
		UDLTest(uint64_t x) : mX(x){}

		uint64_t mX;
	};

	UDLTest operator"" _udl(unsigned long long x) // The type must be unsigned long long and can't be uint64_t, as uint64_t might be unsigned long int.
		{ return UDLTest(x); }
#endif


#if !defined(EA_COMPILER_NO_INLINE_NAMESPACES)
	namespace INSNamespace
	{
		inline namespace INSNamespace_1
		{
			template <typename T>
			class A; 
		}

		template <typename T>
		int g(T){ return 37; }
	}

	struct INSClass{ };

	namespace INSNamespace
	{
		template<>
		class A<INSClass>{ };
	}

#endif


#if !defined(EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS)
	struct FunctionTemplateTest
	{
		template<typename T = int>
		static T AddOne(T value)
			{ return value + 1; }
	};
#endif


#if !defined(EA_COMPILER_NO_NOEXCEPT)
	int NoExceptTestFunction() EA_NOEXCEPT
		{ return 37; }
	
	struct NoExceptTestStruct
	{
		int mX;
		NoExceptTestStruct() : mX(37) {}
	};

	template <class T>
	int NoExceptTestTemplate() EA_NOEXCEPT_IF(EA_NOEXCEPT_EXPR(T()))
		{ T t; return t.mX; }
#endif


// The following function defintions are intended to generate compilation errors if EA_CHAR16_NATIVE or EA_CHAR32_NATIVE is set to 1 when it should be 0.
// i.e. if the types are not actually native then their will be a function redefinition error generated.
void NoopTakingString(const wchar_t *)
{
}
#if EA_WCHAR_UNIQUE
    #if EA_WCHAR_SIZE == 2
        // This definition should not conflict with the wchar_t defintion because char16_t should be unique.
        void NoopTakingString(const char16_t *)
        {
            #if !EA_CHAR16_NATIVE
                #error Expected EA_CHAR16_NATIVE to be 1.
            #endif
        }
    #else
        // This definition should not conflict with the wchar_t defintion because char32_t should be unique.
        void NoopTakingString(const char32_t *)
        {
            #if !EA_CHAR32_NATIVE
                #error Expected EA_CHAR32_NATIVE to be 1.
            #endif
        }

    #endif
#endif


int TestEABase()
{
	int nErrorCount(0);

	DoNothingInline();

	// Test NULL
	{
		if(!VerifyValue<void*>(NULL, (void*)0))
			DoError(nErrorCount, "unspecified test");
	}

	// Verify sized type sizes
	{
		if(!VerifyValue<size_t>(sizeof(int8_t), 1))
			DoError(nErrorCount, "int8_t size test");
		if(!VerifyValue<size_t>(sizeof(uint8_t), 1))
			DoError(nErrorCount, "uint8_t size test");
		if(!VerifyValue<size_t>(sizeof(int16_t), 2))
			DoError(nErrorCount, "int16_t size test");
		if(!VerifyValue<size_t>(sizeof(uint16_t), 2))
			DoError(nErrorCount, "uint16_t size test");
		if(!VerifyValue<size_t>(sizeof(int32_t), 4))
			DoError(nErrorCount, "int32_t size test");
		if(!VerifyValue<size_t>(sizeof(uint32_t), 4))
			DoError(nErrorCount, "uint32_t size test");
		if(!VerifyValue<size_t>(sizeof(int64_t), 8))
			DoError(nErrorCount, "int64_t size test");
		if(!VerifyValue<size_t>(sizeof(uint64_t), 8))
			DoError(nErrorCount, "uint64_t size test");

		#if !defined(FLT_EVAL_METHOD)
			#error  EABase should always define FLT_EVAL_METHOD
			DoError(nErrorCount, "FLT_EVAL_METHOD test: not defined.");
		#else
			#if (FLT_EVAL_METHOD == -1)
				// In this case the C99 standard states that the 
				// precision of float_t and double_t is indeterminable.
			#elif (FLT_EVAL_METHOD == 0)
				if(!VerifyValue<size_t>(sizeof(float_t), sizeof(float)))
					DoError(nErrorCount, "float_t size test");
				if(!VerifyValue<size_t>(sizeof(double_t), sizeof(double)))
					DoError(nErrorCount, "double_t size test");
			#elif (FLT_EVAL_METHOD == 1)
				if(!VerifyValue<size_t>(sizeof(float_t), sizeof(double)))
					DoError(nErrorCount, "float_t size test");
				if(!VerifyValue<size_t>(sizeof(double_t), sizeof(double)))
					DoError(nErrorCount, "double_t size test");
			#elif (FLT_EVAL_METHOD == 2)
				if(!VerifyValue<size_t>(sizeof(float_t), sizeof(long double)))
					DoError(nErrorCount, "float_t size test");
				if(!VerifyValue<size_t>(sizeof(double_t), sizeof(long double)))
					DoError(nErrorCount, "double_t size test");
			#else
				DoError(nErrorCount, "FLT_EVAL_METHOD test: invalid value.");
			#endif
		#endif

		if(sizeof(bool8_t) != 1)
			DoError(nErrorCount, "bool8_t size test");

		if(!VerifyValue<size_t>(sizeof(intptr_t), sizeof(void*)))
			DoError(nErrorCount, "intptr_t size test");
		if(!VerifyValue<size_t>(sizeof(uintptr_t), sizeof(void*)))
			DoError(nErrorCount, "uintptr_t size test");

		if(!VerifyValue<size_t>(sizeof(ssize_t), sizeof(size_t)))
			DoError(nErrorCount, "ssize_t size test");

		EA_DISABLE_VC_WARNING(6326)
		const ssize_t ss(1); // Verify that ssize_t is a signed type.
		if(ssize_t((ss ^ ss) - 1) >= 0)
			DoError(nErrorCount, "ssize_t sign test");
		EA_RESTORE_VC_WARNING()

		if(!VerifyValue<size_t>(sizeof(char8_t), 1))
			DoError(nErrorCount, "char8_t size test");
		if(!VerifyValue<size_t>(sizeof(char16_t), 2))
			DoError(nErrorCount, "char16_t size test");
		if(!VerifyValue<size_t>(sizeof(char32_t), 4))
			DoError(nErrorCount, "char32_t size test");

		#if (EA_WCHAR_SIZE == 2) || (EA_WCHAR_SIZE == 4)
			if(!VerifyValue<size_t>(sizeof(wchar_t), EA_WCHAR_SIZE))
				DoError(nErrorCount, "EA_WCHAR_SIZE test");
		#else
			DoError(nErrorCount, "EA_WCHAR_SIZE test");
		#endif
	}

	// Test CHAR8_MIN, etc.
	{
		// The C standard allows compilers/platforms to use -127 as the min 8 bit value, but we've never seen it in modern systems.
		static_assert(((((CHAR8_MIN  ==                -128)  && (CHAR8_MAX  ==                127)))  || ((CHAR8_MIN  == 0) && (CHAR8_MAX  ==                255))),  "CHAR8_MAX failure");
		static_assert(((((CHAR16_MIN ==              -32768)  && (CHAR16_MAX ==              32767)))  || ((CHAR16_MIN == 0) && (CHAR16_MAX ==              65535))),  "CHAR16_MAX failure");
		static_assert(((((CHAR32_MIN == -INT64_C(2147483648)) && (CHAR32_MAX == INT64_C(2147483647)))) || ((CHAR32_MIN == 0) && (CHAR32_MAX == INT64_C(4294967295)))), "CHAR32_MAX failure");
	}

	// Test char8_t, char16_t, char32_t string literals.
	{
		const char8_t*  p8  = EA_CHAR8("abc");
		const char8_t   c8  = EA_CHAR8('a');

		#ifdef EA_CHAR16
			const char16_t* p16 = EA_CHAR16("abc"); // Under GCC, this assumes compiling with -fshort-wchar
			const char16_t  c16 = EA_CHAR16('\x3001');
		#else
			const char16_t* p16 = NULL;
			const char16_t  c16 = static_cast<char16_t>('X');
		#endif

		#ifdef EA_CHAR32
			const char32_t* p32 = EA_CHAR32("abc");
			const char32_t  c32 = EA_CHAR32('\x3001');
		#else
			const char32_t p32[] = { 'a', 'b', 'c', '\0' }; // Microsoft doesn't support 32 bit strings here, and GCC doesn't use them when we compile with -fshort-wchar (which we do).
			#ifdef EA_CHAR16
				const char32_t c32   = EA_CHAR16('\x3001');         // 16 bit should silently convert to 32 bit.
			#else
				const char32_t c32   = static_cast<char16_t>('X');         // 16 bit should silently convert to 32 bit.
			#endif
		#endif

		const wchar_t* pW = EA_WCHAR("abc");
		const wchar_t  cW = EA_WCHAR('\x3001');

		EA_UNUSED(p8);
		EA_UNUSED(c8);
		EA_UNUSED(p16);
		EA_UNUSED(c16);
		EA_UNUSED(p32);
		EA_UNUSED(c32);
		EA_UNUSED(pW);
		EA_UNUSED(cW);
	}

	// Verify sized type signs
	{
		int8_t i8(1);
		if(int8_t((i8 ^ i8) - 1) >= 0)
			DoError(nErrorCount, "int8_t sign test");

		uint8_t u8(1);
		if(uint8_t((u8 ^ u8) - 1) <= 0)
			DoError(nErrorCount, "uint8_t sign test");

		int16_t i16(1);
		if(int16_t((i16 ^ i16) - 1) >= 0)
			DoError(nErrorCount, "int16_t sign test");

		uint16_t u16(1);
		if(uint16_t((u16 ^ u16) - 1) <= 0)
			DoError(nErrorCount, "uint16_t sign test");

		int32_t i32(1);
		if(int32_t((i32 ^ i32) - 1) >= 0)
			DoError(nErrorCount, "int32_t sign test");

		uint32_t u32(1);
		if(uint32_t((u32 ^ u32) - 1) <= 0)
			DoError(nErrorCount, "uint32_t sign test");

		int64_t i64(1);
		if(int64_t((i64 ^ i64) - 1) >= 0)
			DoError(nErrorCount, "int64_t sign test");

		uint64_t u64(1);
		if(uint64_t((u64 ^ u64) - 1) <= 0)
			DoError(nErrorCount, "uint64_t sign test");



		intptr_t ip(1);
		if(intptr_t((ip ^ ip) - 1) >= 0)
			DoError(nErrorCount, "intptr_t sign test");

		uintptr_t up(1);
		if(uintptr_t((up ^ up) - 1) <= 0)
			DoError(nErrorCount, "uintptr_t sign test");


		// The following sign tests have been disabled, as the determination of 
		// the sign of type char and wchar_t are in the hands of the compiler and
		// the user's configuration of that compiler.

		//char8_t c8(1); // We expect it to be signed, though the need for such a requirement is debateable.
		//if(char8_t((c8 ^ c8) - 1) >= 0)
		//    DoError(nErrorCount, "char8_t sign test");

		//char16_t c16(1); // We expect it to be unsigned
		//if(char16_t((c16 ^ c16) - 1) <= 0)
		//    DoError(nErrorCount, "char16_t sign test");

		//char32_t c32(1); // We expect it to be unsigned
		//if(char32_t((c32 ^ c32) - 1) <= 0)
		//    DoError(nErrorCount, "char32_t sign test");
	}



	//Test Constant macros
	{
		char buffer[256];

		const int8_t i8Min = INT8_C(-128); // Strictly speaking, the C language standard allows this to be -127 as well.
		const int8_t i8Max = INT8_C(127);

		const uint8_t u8Min = UINT8_C(0);
		const uint8_t u8Max = UINT8_C(255);

		const int16_t i16Min = INT16_C(-32767) - 1;
		const int16_t i16Max = INT16_C( 32767);

		const uint16_t u16Min = UINT16_C(0);
		const uint16_t u16Max = UINT16_C(65535);

		const int32_t i32Min = INT32_C(-2147483647) - 1;
		const int32_t i32Max = INT32_C( 2147483647);

		const uint32_t u32Min = UINT32_C(0);
		const uint32_t u32Max = UINT32_C(4294967295);

		#if defined(__GNUC__) && (__GNUC__ < 4) // If using a broken version of UINT64_C/INT64_C macros...
			const int64_t i64Min = -9223372036854775807LL - 1;
			const int64_t i64Max =  9223372036854775807LL;

			const uint64_t u64Min = UINT64_C(0);
			const uint64_t u64Max = 18446744073709551615ULL;
		#else
			const int64_t i64Min = INT64_C(-9223372036854775807) - 1;
			const int64_t i64Max = INT64_C( 9223372036854775807);

			const uint64_t u64Min = UINT64_C(0);
			const uint64_t u64Max = UINT64_C(18446744073709551615);
		#endif

		sprintf(buffer, "%d %d %u %u %d %d %u %u %d %d %u %u %" SCNd64" %" SCNd64" %" SCNu64" %" SCNu64,
				  (int)i8Min,  (int)i8Max,  (unsigned)u8Min,  (unsigned)u8Max, 
				  (int)i16Min, (int)i16Max, (unsigned)u16Min, (unsigned)u16Max, 
				  (int)i32Min, (int)i32Max, (unsigned)u32Min, (unsigned)u32Max, 
				  i64Min, i64Max, u64Min, u64Max);
		if(strcmp(buffer, "-128 127 0 255 -32768 32767 0 65535 -2147483648 2147483647 0 4294967295 -9223372036854775808 9223372036854775807 0 18446744073709551615"))
			DoError(nErrorCount, "INT_C test");

		EA_DISABLE_VC_WARNING(6326)
		// Verify the use of hex numbers with INT64_C
		const int64_t i64Hex = INT64_C(0x1111111122222222);
		if(i64Hex != INT64_C(1229782938533634594))
			DoError(nErrorCount, "INT64_C hex error");
		EA_RESTORE_VC_WARNING()

		// Verify the use of hex numbers with UINT64_C
		const uint64_t u64Hex = UINT64_C(0xaaaaaaaabbbbbbbb);

		#if defined(__GNUC__) && (__GNUC__ < 4) // If using a broken version of UINT64_C/INT64_C macros...
			const uint64_t temp = 12297829382759365563ULL; 
		#else
			const uint64_t temp = UINT64_C(12297829382759365563);
		#endif

		EA_DISABLE_VC_WARNING(6326)
		if(u64Hex != temp)
			DoError(nErrorCount, "UINT64_C hex error");
		EA_RESTORE_VC_WARNING()

		// Verify that the compiler both allows division with uint64_t but 
		// also that it allows it via UINT64_MAX. A bad implementation of 
		// UINT64_MAX would cause the code below to mis-execute or not compile.
		EA_DISABLE_VC_WARNING(6326)
		const uint64_t resultUint64 = UINT64_MAX / 2;
		if(resultUint64 != UINT64_C(9223372036854775807))
			DoError(nErrorCount, "UINT64_MAX error");
		EA_RESTORE_VC_WARNING()
	}

	{
		static_assert(INTPTR_MIN  == eastl::numeric_limits<intptr_t>::min(),  "INTPTR_MIN failure");
		static_assert(INTPTR_MAX  == eastl::numeric_limits<intptr_t>::max(),  "INTPTR_MAX failure");
	  //static_assert(UINTPTR_MIN == eastl::numeric_limits<uintptr_t>::min(), "UINTPTR_MIN failure"); // not specified by the standard
		static_assert(UINTPTR_MAX == eastl::numeric_limits<uintptr_t>::max(), "UINTPTR_MAX failure");
		static_assert(INTMAX_MIN  == eastl::numeric_limits<intmax_t>::min(),  "INTMAX_MIN failure");
		static_assert(INTMAX_MAX  == eastl::numeric_limits<intmax_t>::max(),  "INTMAX_MAX failure");
	  //static_assert(UINTMAX_MIN == eastl::numeric_limits<uintmax_t>::MIN(), "UINTMAX_MIN failure"); // not specified by the standard
		static_assert(UINTMAX_MAX == eastl::numeric_limits<uintmax_t>::max(), "UINTMAX_MAX failure");
	}

	//Test sized printf format specifiers
	{
		char buffer[256];

		int8_t  d8(INT8_MAX), i8(INT8_MIN), o8(INT8_MAX);
		uint8_t u8(UINT8_MAX), x8(UINT8_MAX), X8(UINT8_MAX);        
		sprintf(buffer, "%" PRId8 " %" PRIi8 " %" PRIo8 " %" PRIu8 " %" PRIx8 " %" PRIX8, d8, i8, o8, u8, x8, X8);
		
		#ifdef EA_COMPILER_GNUC
			if(Stricmp(buffer, "127 -128 177 255 ff FF"))
				DoError(nErrorCount, "PRI8 test"); // This is known to fail with compilers such as VC++ which don't support %hh.
		#endif

		int16_t  d16(INT16_MAX), i16(INT16_MIN), o16(INT16_MAX);
		uint16_t u16(UINT16_MAX), x16(UINT16_MAX), X16(UINT16_MAX);        
		sprintf(buffer, "%" PRId16 " %" PRIi16 " %" PRIo16 " %" PRIu16 " %" PRIx16 " %" PRIX16, d16, i16, o16, u16, x16, X16);
		if(Stricmp(buffer, "32767 -32768 77777 65535 ffff FFFF"))
			DoError(nErrorCount, "PRI16 test");

		int32_t  d32(INT32_MAX), i32(INT32_MIN), o32(INT32_MAX);
		uint32_t u32(UINT32_MAX), x32(UINT32_MAX), X32(UINT32_MAX);        
		sprintf(buffer, "%" PRId32 " %" PRIi32 " %" PRIo32 " %" PRIu32 " %" PRIx32 " %" PRIX32, d32, i32, o32, u32, x32, X32);
		if(Stricmp(buffer, "2147483647 -2147483648 17777777777 4294967295 ffffffff FFFFFFFF"))
			DoError(nErrorCount, "PRI32 test");

		int64_t  d64(INT64_MAX), i64(INT64_MIN), o64(INT64_MAX);
		uint64_t u64(UINT64_MAX), x64(UINT64_MAX), X64(UINT64_MAX);        
		sprintf(buffer, "%" PRId64 " %" PRIi64 " %" PRIo64 " %" PRIu64 " %" PRIx64 " %" PRIX64, d64, i64, o64, u64, x64, X64);
		if(Stricmp(buffer, "9223372036854775807 -9223372036854775808 777777777777777777777 18446744073709551615 ffffffffffffffff FFFFFFFFFFFFFFFF"))
			DoError(nErrorCount, "PRI64 test");

		// Many compilers give warnings for the following code because they 
		// recognize that a pointer is being formatted as an integer.  
		// This is what we want to do and what the C99 standard intends here.
		#if defined(_MSC_VER) && (_MSC_VER >= 1300)
			#pragma warning(disable: 4313) // Warning C4313: 'sprintf'  : '%d' in format string conflicts with argument 1 of type 'void *'
			#pragma warning(disable: 4777) // Warning C4777: 'sprintf' : format string '%lld' requires an argument of type '__int64', but variadic argument 1 has type 'intptr_t'
		#endif

		#if !defined(__GNUC__) // GCC generates warnings here which we can't work around.
			void *dPtr = (void*)INT32_MAX, *iPtr = (void*)INT32_MIN, *oPtr = (void*)INT32_MAX, *uPtr = (void*)(uintptr_t)UINT64_MAX, *xPtr = (void*)(uintptr_t)UINT64_MAX, *XPtr = (void*)(uintptr_t)UINT64_MAX;
			sprintf(buffer, "%" PRIdPTR " %" PRIiPTR " %" PRIoPTR " %" PRIuPTR " %" PRIxPTR " %" PRIXPTR, (intptr_t)dPtr, (intptr_t)iPtr, (uintptr_t)oPtr, (uintptr_t)uPtr, (uintptr_t)xPtr, (uintptr_t)XPtr);

			#if (EA_PLATFORM_PTR_SIZE == 4)
				if(Stricmp(buffer, "2147483647 -2147483648 17777777777 4294967295 ffffffff FFFFFFFF"))
					DoError(nErrorCount, "PRIPTR test");
			#else // EA_PLATFORM_PTR_SIZE == 8
				if(Stricmp(buffer, "2147483647 -2147483648 17777777777 18446744073709551615 ffffffffffffffff FFFFFFFFFFFFFFFF"))
					DoError(nErrorCount, "PRIPTR test");
			#endif
		#endif

		#if defined(_MSC_VER) && (_MSC_VER >= 1300)
			#pragma warning(default: 4313)
			#pragma warning(default: 4777)
		#endif
	}

	//Test sized scanf format specifiers
	{
		int numMatched = 0;
		#ifdef EA_COMPILER_IS_C99 // Enabled for C99 only because this code will simply crash on many platforms if the format specifiers aren't supported.
			int8_t  d8, i8, o8;
			uint8_t u8, x8;
			numMatched = sscanf("127 -127 177 255 ff", "%" SCNd8 " %" SCNi8 " %" SCNo8 " %" SCNu8 " %" SCNx8, &d8, &i8, &o8, &u8, &x8);
			if((numMatched != 5) || (d8 != 127) || (i8 != -127) || (o8 != 127) || (u8 != 255) || (x8 != 255))
				DoError(nErrorCount, "SCN8 test"); // This is known to fail with compilers such as VC++ which don't support %hh.
		#endif

		int16_t  d16, i16, o16;
		uint16_t u16, x16;
		numMatched = sscanf("32767 -32768 77777 65535 ffff", "%" SCNd16 " %" SCNi16 " %" SCNo16 " %" SCNu16 " %" SCNx16, &d16, &i16, &o16, &u16, &x16);
		if((numMatched != 5) || (d16 != 32767) || (i16 != -32768) || (o16 != 32767) || (u16 != 65535) || (x16 != 65535))
			DoError(nErrorCount, "SCN16 test");

		int32_t  d32, i32, o32;
		uint32_t u32, x32;
		numMatched = sscanf("2147483647 -2147483648 17777777777 4294967295 ffffffff", "%" SCNd32 " %" SCNi32 " %" SCNo32 " %" SCNu32 " %" SCNx32, &d32, &i32, &o32, &u32, &x32);
		if((numMatched != 5) || (d32 != INT32_MAX) || (i32 != INT32_MIN) || (o32 != INT32_MAX) || (u32 != UINT32_MAX) || (x32 != UINT32_MAX))
			DoError(nErrorCount, "SCN32 test");

		int64_t  d64, i64, o64;
		uint64_t u64, x64;
		numMatched = sscanf("9223372036854775807 -9223372036854775808 777777777777777777777 18446744073709551615 ffffffffffffffff", "%" SCNd64 " %" SCNi64 " %" SCNo64 " %" SCNu64 " %" SCNx64, &d64, &i64, &o64, &u64, &x64);
		if((numMatched != 5) || (d64 != INT64_MAX) || (i64 != INT64_MIN) || (o64 != INT64_MAX) || (u64 != UINT64_MAX) || (x64 != UINT64_MAX))
			DoError(nErrorCount, "SCN64 test");

		// Many compilers give warnings for the following code because they 
		// recognize that a pointer is being formatted as an integer.  
		// This is what we want to do and what the C99 standard intends here.
		#if !defined(__GNUC__) // GCC generates warnings here which we can't work around.
			void *dPtr, *iPtr, *oPtr, *uPtr, *xPtr;
			intptr_t dip, iip;
			uintptr_t ouip, uuip, xuip;

			EA_DISABLE_VC_WARNING(4777) // format string '%lld' requires an argument of type '__int64 *', but variadic argument 1 has type 'intptr_t *'
			#if (EA_PLATFORM_PTR_SIZE == 4)
				numMatched = sscanf("2147483647 -2147483648 17777777777 4294967295 ffffffff", "%" SCNdPTR " %" SCNiPTR " %" SCNoPTR " %" SCNuPTR " %" SCNxPTR, &dip, &iip, &ouip, &uuip, &xuip);
			#else // EA_PLATFORM_PTR_SIZE == 8
				numMatched = sscanf("2147483647 -2147483648 17777777777 18446744073709551615 ffffffffffffffff", "%" SCNdPTR " %" SCNiPTR " %" SCNoPTR " %" SCNuPTR " %" SCNxPTR, &dip, &iip, &ouip, &uuip, &xuip);
			#endif
			EA_RESTORE_VC_WARNING()

			dPtr = (void*)dip;
			iPtr = (void*)iip;
			oPtr = (void*)ouip;
			uPtr = (void*)uuip;
			xPtr = (void*)xuip;

			if((numMatched != 5) || (dPtr != (void*)INT32_MAX) || (iPtr != (void*)INT32_MIN) || (oPtr != (void*)INT32_MAX) || (uPtr != (void*)(uintptr_t)UINT64_MAX) || (xPtr != (void*)(uintptr_t)UINT64_MAX))
				DoError(nErrorCount, "SCNPTR test");
		#endif
	}


	// Test min/max
	{
		// The C standard allows INT8_MIN to be either -127 or -128. So in order to be able
		// to test for this in a portable way, we do the logic below whereby we test for 
		// -127 (which all compiles should support) or -127 - 1 which all compilers should
		// support if INT8_MIN isn't -127.
		if(!VerifyValue<int8_t>(INT8_MIN, INT8_C(-127)) && !VerifyValue<int8_t>(INT8_MIN, INT8_C(-127) - 1))
			DoError(nErrorCount, "INT8_MIN test");
		if(!VerifyValue<int8_t>(INT8_MAX, INT8_C(127)))
			DoError(nErrorCount, "INT8_MAX test");
		if(!VerifyValue<uint8_t>(UINT8_MAX, UINT8_C(255)))
			DoError(nErrorCount, "UINT8_MAX test");

		if(!VerifyValue<int16_t>(INT16_MIN, INT16_C(-32767)) && !VerifyValue<int16_t>(INT16_MIN, INT16_C(-32767) - 1))
			DoError(nErrorCount, "INT16_MIN test");
		if(!VerifyValue<int16_t>(INT16_MAX, INT16_C(32767)))
			DoError(nErrorCount, "INT16_MAX test");
		if(!VerifyValue<uint16_t>(UINT16_MAX, UINT16_C(65535)))
			DoError(nErrorCount, "UINT16_MAX test");

		if(!VerifyValue<int32_t>(INT32_MIN, INT32_C(-2147483647)) && !VerifyValue<int32_t>(INT32_MIN, INT32_C(-2147483647) - 1))
			DoError(nErrorCount, "INT32_MIN test");
		if(!VerifyValue<int32_t>(INT32_MAX, INT32_C(2147483647)))
			DoError(nErrorCount, "INT32_MAX test");
		if(!VerifyValue<uint32_t>(UINT32_MAX, UINT32_C(4294967295)))
			DoError(nErrorCount, "UINT32_MAX test");

		if(!VerifyValue<int64_t>(INT64_MIN, INT64_C(-9223372036854775807)) && !VerifyValue<int64_t>(INT64_MIN, INT64_C(-9223372036854775807) - 1))
			DoError(nErrorCount, "INT64_MIN test");
		if(!VerifyValue<uint64_t>(INT64_MAX, INT64_C(9223372036854775807)))
			DoError(nErrorCount, "INT64_MAX test");

		#if defined(__GNUC__) && (__GNUC__ < 4) // If using a broken version of UINT64_C/INT64_C macros...
			const uint64_t temp = 18446744073709551615ULL;
		#else
			const uint64_t temp = UINT64_C(18446744073709551615);
		#endif

		if(!VerifyValue<uint64_t>(UINT64_MAX, temp))
			DoError(nErrorCount, "UINT64_MAX test");
	}

    {
        NoopTakingString(L"");
        // Compilation errors below indicate that the EA_CHAR16/EA_CHAR32 may be incorrectly defined, or EA_CHAR16_NATIVE/EA_CHAR32_NATIVE is incorrect set to 0.
        #if EA_WCHAR_SIZE == 2 && defined(EA_CHAR16)
            const char16_t *str = EA_CHAR16("");
            NoopTakingString(str);
        #elif EA_WCHAR_SIZE == 4 && defined(EA_CHAR32)
            const char32_t *str = EA_CHAR32("");
            NoopTakingString(str);
        #endif
    }

	return nErrorCount;
}



int TestEAResult()
{
	int nErrorCount(0);

	EA::result_type resultSuccess(EA::SUCCESS);
	EA::result_type resultFailure(EA::FAILURE);
	EA::result_type resultZero(0); // success
	EA::result_type resultNeg(-1); // failure
	EA::result_type resultPos(+1); // success


	if(!EA_SUCCEEDED(resultSuccess))
		DoError(nErrorCount, "EA::SUCCESS test");
	if(EA_FAILED(resultSuccess))
		DoError(nErrorCount, "EA::SUCCESS test");

	if(EA_SUCCEEDED(resultFailure))
		DoError(nErrorCount, "EA::FAILURE test");
	if(!EA_FAILED(resultFailure))
		DoError(nErrorCount, "EA::FAILURE test");

	if(!EA_SUCCEEDED(resultZero))
		DoError(nErrorCount, "EA::SUCCESS test");
	if(EA_FAILED(resultZero))
		DoError(nErrorCount, "EA::SUCCESS test");

	if(EA_SUCCEEDED(resultNeg))
		DoError(nErrorCount, "EA::FAILURE test");
	if(!EA_FAILED(resultNeg))
		DoError(nErrorCount, "EA::FAILURE test");

	if(!EA_SUCCEEDED(resultPos))
		DoError(nErrorCount, "EA::SUCCESS test");
	if(EA_FAILED(resultPos))
		DoError(nErrorCount, "EA::SUCCESS test");

	return nErrorCount;
}



int TestEAPlatform()
{
	int nErrorCount(0);

	// Test EA_PLATFORM_PTR_SIZE
	{
		#ifdef EA_PLATFORM_PTR_SIZE
			if(!VerifyValue<size_t>(EA_PLATFORM_PTR_SIZE, sizeof(void*)))
				DoError(nErrorCount, "EA_PLATFORM_PTR_SIZE test");
		#else
			DoError(nErrorCount, "EA_PLATFORM_PTR_SIZE test");
		#endif
	}


	// Test EA_PLATFORM_NAME
	{
		#ifdef EA_PLATFORM_NAME
			char buffer[256];
			sprintf(buffer, "TestEAPlatform: EA_PLATFORM_NAME: %s\n", EA_PLATFORM_NAME);
		#else
			DoError(nErrorCount, "EA_PLATFORM_NAME test");
		#endif
	}


	// Test EA_PLATFORM_DESCRIPTION
	{
		#ifdef EA_PLATFORM_DESCRIPTION
			char buffer[256];
			sprintf(buffer, "TestEAPlatform: EA_PLATFORM_DESCRIPTION: %s\n", EA_PLATFORM_DESCRIPTION);
		#else
			DoError(nErrorCount, "EA_PLATFORM_DESCRIPTION test");
		#endif
	}


	// Test EA_SYSTEM_LITTLE_ENDIAN / EA_SYSTEM_BIG_ENDIAN
	{
		uint32_t kValue = 0x12345678;
		uint8_t* pValue = (uint8_t*)&kValue;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			if(pValue[0] != 0x78)
				DoError(nErrorCount, "EA_SYSTEM_ENDIAN test");
		#elif defined(EA_SYSTEM_BIG_ENDIAN)
			if(pValue[0] != 0x12)
				DoError(nErrorCount, "EA_SYSTEM_ENDIAN test");
		#else
			DoError(nErrorCount, "EA_SYSTEM_ENDIAN test");
		#endif
	}


	// Test EA_ASM_STYLE
	{
		#if defined(EA_PROCESSOR_X86)
			#if defined(EA_ASM_STYLE_ATT)
				asm volatile ("nop");
			#elif defined(EA_ASM_STYLE_INTEL)
				__asm nop
			#endif
		#else
			// Add other processors here.
		#endif
	}


	return nErrorCount;
}



// Test compiler limitations
// Easiest way to come up with tests for some of the more complicated versions 
// of these is to look at the Boost /libs/config/test/*.cxx files. Many of the 
// Boost compiler limitation defines are similar or match exactly to those 
// defined by EABase. See http://www.boost.org if you want to check this out.

#ifndef EA_COMPILER_NO_STATIC_CONSTANTS // If class member static constants are allowed...
	struct NSC
	{
		static const int x = 10;
	};
#endif

#ifndef EA_COMPILER_NO_TEMPLATE_SPECIALIZATION
	// Todo
#endif

#ifndef EA_COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION
	// Todo
#endif

#ifndef EA_COMPILER_NO_MEMBER_TEMPLATES
	// Todo
#endif

#ifndef EA_COMPILER_NO_MEMBER_TEMPLATE_SPECIALIZATION
	// Todo
#endif

#ifndef EA_COMPILER_NO_TEMPLATE_TEMPLATES
	// Todo
#endif

#ifndef EA_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS
	// Todo
#endif

#ifndef EA_COMPILER_NO_VOID_RETURNS
	void TestNVR1();
	void TestNVR();

	void TestNVR1()
	{
		char buffer[8];
		sprintf(buffer, " ");
	}
	void TestNVR()
	{
		return TestNVR1();
	}
#endif

#ifndef EA_COMPILER_NO_COVARIANT_RETURN_TYPE
	// Todo
#endif

#ifndef EA_COMPILER_NO_DEDUCED_TYPENAME
	// Todo
#endif

#ifndef EA_COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP
	// Todo
#endif

#if !defined(EA_COMPILER_NO_EXCEPTION_STD_NAMESPACE) && !defined(EA_COMPILER_NO_STANDARD_CPP_LIBRARY)
	#include <exception>

	static void TestNESN()
	{
	  // iPhone gives us this error: Undefined symbols for architecture armv6: std::terminate()
	  // Android gives: undefined reference to std::terminate()
	  // We could possibly define our own std::terminate, but that might collide in the future unexpectedly.
	  #if defined(EA_PLATFORM_IPHONE) || defined(EA_PLATFORM_ANDROID) 
		void (*pTerminate)() = NULL;
	  #else
		void (*pTerminate)() = std::terminate;
	  #endif
		char buffer[32];
		sprintf(buffer, "%p", pTerminate);
	}
#endif

#ifndef EA_COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS
	// Todo
#endif

#ifndef EA_COMPILER_NO_EXCEPTIONS
	static bool TestNE()
	{
		EA_DISABLE_VC_WARNING(4571)
		try{
			char buffer1[8];
			sprintf(buffer1, " ");
			throw int(0);
		}
		catch(...){
			char buffer2[8];
			sprintf(buffer2, " "); // If you are tracing this in a debugger and the debugger stops here, then you need to let the app continue.
		}
		return true;
		EA_RESTORE_VC_WARNING()
	}
#endif


struct UnwindTest
{
	static int x;
	enum State{
		kStateNone,
		kStateConstructed,
		kStateDestructed
	};
	UnwindTest()
		{ x = kStateConstructed; };
   ~UnwindTest()
		{ x = kStateDestructed;  };
};
int UnwindTest::x = kStateNone;

#ifndef EA_COMPILER_NO_EXCEPTIONS
	static void TestNU1()
	{
		UnwindTest ut;
		#ifndef EA_COMPILER_NO_EXCEPTIONS
			throw(int(0)); // If you are tracing this in a debugger and the debugger stops here, then you need to let the app continue.
		#endif
	}
#endif

bool TestNU()
{
	bool bReturnValue(false);

	#ifdef EA_COMPILER_NO_EXCEPTIONS
		bReturnValue = true; //Nothing to test, so we just return true.
	#else
		EA_DISABLE_VC_WARNING(4571)
		try
		{
			TestNU1();
		}
		catch(...)
		{
			#ifdef EA_COMPILER_NO_UNWIND
				if(UnwindTest::x == UnwindTest::kStateConstructed)
					bReturnValue = true;
			#else
				if(UnwindTest::x == UnwindTest::kStateDestructed)
					bReturnValue = true;
			#endif
		}
		EA_RESTORE_VC_WARNING()
	#endif

	return bReturnValue;
}

#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	#include <vector> // We need do nothing more than #include this.
#endif

#ifndef EA_COMPILER_NO_COVARIANT_RETURN_TYPE
	// Todo
#endif

#ifndef EA_COMPILER_NO_COVARIANT_RETURN_TYPE
	// Todo
#endif


#if !defined(EA_COMPILER_NO_TRAILING_RETURN_TYPES)
	// This usage assumes that C++11 auto is supported, which in practice is always the case because
	// the case because otherwise trailing return types wouldn't be as useful.
	static auto AddOne(int i)->int
	{ 
		return i + 1;
	}

	template <typename T>
	struct AddTwoClass
	{
		typedef float Local_type;
		Local_type AddTwo(T t);
	};

	template <typename T>
	auto AddTwoClass<T>::AddTwo(T t)->Local_type
	{
		return (t + 2.f); // Assumes that t is a numerical type in this case.
	}
#endif


#if !defined(EA_COMPILER_NO_VARIADIC_TEMPLATES)
	template<typename...>
	struct VariadicTemplateTuple{};

	template<typename T1, typename T2>
	struct VariadicTemplatePair
	{
		T1 x;
		T1 y;
	};

	template<class ... Args1>
	struct VariadicTemplateZip
	{
		template<class ... Args2> struct with
		{
			typedef VariadicTemplateTuple<VariadicTemplatePair<Args1, Args2> ... > type;
		};
	};

	// VariadicTemplateType is Tuple<Pair<short, unsigned short>, Pair<int, unsigned> >
	typedef VariadicTemplateZip<short, int>::with<unsigned short, unsigned>::type VariadicTemplateType;
#endif


#if !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
	template<typename T> 
	using VectorAlias = eastl::vector<T, EASTLAllocatorType>;
#endif

#if !defined(EA_COMPILER_NO_VARIABLE_TEMPLATES)
	template<class T>
	constexpr T pi = T(3.1415926535897932385);
#endif


int TestEACompiler()
{
	int nErrorCount(0);

	// As of this writing, eacompiler.h defines at least the following compilers:
	// EA_COMPILER_GNUC
	// EA_COMPILER_INTEL
	// EA_COMPILER_METROWERKS
	// EA_COMPILER_MSVC, EA_COMPILER_MSVC6, EA_COMPILER_MSVC7, EA_COMPILER_MSVC7_1
	

	// Test EA_COMPILER_NAME
	{
		#ifdef EA_COMPILER_NAME
			char buffer[256];
			sprintf(buffer, "TestEACompiler: EA_COMPILER_NAME: %s\n", EA_COMPILER_NAME);
		#else
			DoError(nErrorCount, "EA_COMPILER_NAME test");
		#endif
	}


	// Test EA_COMPILER_VERSION
	{
		#ifdef EA_COMPILER_VERSION
			char buffer[256];
			sprintf(buffer, "TestEACompiler: EA_COMPILER_VERSION: %d\n", EA_COMPILER_VERSION);
		#else
			DoError(nErrorCount, "EA_COMPILER_VERSION test");
		#endif
	}


	// Test EA_COMPILER_STRING
	{
		#ifdef EA_COMPILER_STRING
			char buffer[256];
			sprintf(buffer, "TestEACompiler: EA_COMPILER_STRING: %s\n", EA_COMPILER_STRING);
		#else
			DoError(nErrorCount, "EA_COMPILER_STRING test");
		#endif
	}


	// Test EA_COMPILER_NO_STATIC_CONSTANTS
	{
		char buffer[256];
		sprintf(buffer, "%d", (int)NSC::x);
		if(buffer[0] != '1')
			DoError(nErrorCount, "EA_COMPILER_NO_STATIC_CONSTANTS test");
	}


	// Test EA_COMPILER_NO_VOID_RETURNS
	#ifndef EA_COMPILER_NO_VOID_RETURNS
		TestNVR1(); // Nothing to test for except successful compilation.
	#endif


	// Test EA_COMPILER_NO_EXCEPTION_STD_NAMESPACE
	#if !defined(EA_COMPILER_NO_EXCEPTION_STD_NAMESPACE) && !defined(EA_COMPILER_NO_STANDARD_CPP_LIBRARY)
		TestNESN();
	#endif

	#if !(defined(EA_PLATFORM_IPHONE) && defined(EA_COMPILER_CLANG)) || defined(__IPHONE_7_0)
	{
		// There was a bug in Apple's exception code in iOS SDK versions
		// prior to 7.0, which is why this test is disabled for versions
		// of the SDK before 7.0.
		// Note that __IPHONE_7_0 will be defined for all future SDKs as
		// well, because Apple simply adds another define with each release
		// and does not remove the old ones.

	// Test EA_COMPILER_NO_EXCEPTIONS
	#ifndef EA_COMPILER_NO_EXCEPTIONS
		if(!TestNE())
			DoError(nErrorCount, "EA_COMPILER_NO_EXCEPTIONS test");
	#endif


	// Test EA_COMPILER_NO_UNWIND
	if(!TestNU())
		DoError(nErrorCount, "EA_COMPILER_NO_UNWIND test");
	}
	#endif


	// Test EA_COMPILER_NO_RVALUE_REFERENCES
	#ifndef EA_COMPILER_NO_RVALUE_REFERENCES
	{
		// Trivial test 
		int&& i = 2;
		FunctionWithUnusedVariables(i);
	}
	#endif


	// Test EA_COMPILER_NO_RANGE_BASED_FOR_LOOP
	#if !defined(EA_COMPILER_NO_RANGE_BASED_FOR_LOOP)
	{
		float floatArray[2] = { 0.0f, 1.0f };

		for(float& f : floatArray)
			f += 1.0;

		EATEST_VERIFY(floatArray[1] == 2.0f);
	}
	#endif


	// Test EA_COMPILER_NO_AUTO
	#if !defined(EA_COMPILER_NO_AUTO)
	{
		auto length = strlen("test");
		EATEST_VERIFY(length == 4);
	}
	#endif


	// Test EA_COMPILER_NO_DECLTYPE
	#if !defined(EA_COMPILER_NO_DECLTYPE)
	{
		struct A { double x; };
		int b = 9;
		A   a; a.x = 7;
		decltype(b)   x2 = 2;
		decltype(a.x) x3 = 3.5;
		EATEST_VERIFY((b + a.x) == 16);
		EATEST_VERIFY((x3 + x2) == 5.5);
	}
	#endif


	// Test EA_COMPILER_NO_LAMBDA_EXPRESSIONS
	#if !defined(EA_COMPILER_NO_LAMBDA_EXPRESSIONS)
	{
		struct LambaTest
		{
			static void SortViaAbs(float* x, size_t n)
			{
				eastl::insertion_sort(x, x + n,
					[](float a, float b)
					  { return (a < b); }
				);
			}
		};

		float floatArray[3] = { 0.f, 1.f, 3.f };
		LambaTest::SortViaAbs(floatArray, EAArrayCount(floatArray));
		EATEST_VERIFY(floatArray[1] == 1.f);
	}
	#endif


	// Test EA_COMPILER_NO_TRAILING_RETURN_TYPES
	#if !defined(EA_COMPILER_NO_TRAILING_RETURN_TYPES)
	{
		int x = AddOne(2);          // AddOne declared above.
		EATEST_VERIFY(x == 3);

		AddTwoClass<float> a;             
		float y = a.AddTwo(2.f);
		EATEST_VERIFY(y == 4.f);
	}
	#endif


	// Test EA_COMPILER_NO_FORWARD_DECLARED_ENUMS
	// Forward declared enum support requires strongly typed enum support.
	#if !defined(EA_COMPILER_NO_FORWARD_DECLARED_ENUMS) && !defined(EA_COMPILER_NO_STRONGLY_TYPED_ENUMS)
	{
		// This happen to be used below in the EA_COMPILER_NO_STRONGLY_TYPED_ENUMS section.
		enum class Color;
		enum class Size    : uint8_t;
		enum       Distance: uint8_t;
	}
	#endif


	// Test EA_COMPILER_NO_STRONGLY_TYPED_ENUMS
	#if !defined(EA_COMPILER_NO_STRONGLY_TYPED_ENUMS)
	{
		enum class Color              { red, blue, green };
		enum class Size     : uint8_t { little = 1, med = 1, large = 2 };
		enum       Distance : uint8_t { close = 1, faraway = 2 };

		Color c = Color::red;
		EATEST_VERIFY(c != Color::blue);

		Size s = Size::med;
		EATEST_VERIFY(s != Size::large);
		static_assert(sizeof(s) == 1, "EA_COMPILER_NO_STRONGLY_TYPED_ENUMS failure");

		Distance d = close;
		EATEST_VERIFY(d != faraway);
		static_assert(sizeof(d) == 1, "EA_COMPILER_NO_STRONGLY_TYPED_ENUMS failure");
	}
	#endif


	// Test EA_COMPILER_NO_VARIADIC_TEMPLATES
	#if !defined(EA_COMPILER_NO_VARIADIC_TEMPLATES)
	{
		// This uses types defined above.
		VariadicTemplateType x;

		static_assert(sizeof(x) > 0, "EA_COMPILER_NO_VARIADIC_TEMPLATES failure");

		char buffer[32];
		sprintf(buffer, "%p", &x);
		// Ignore the result, as we're just verifying that it compiles.
	}
	#endif


	// Test EA_COMPILER_NO_TEMPLATE_ALIASES
	#if !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
	{
		// This uses types defined above.
		// Same as vector<int, EASTLAllocatorType> v;
		VectorAlias<int> v;
		EATEST_VERIFY(v.empty());
	}
	#endif


	// Test EA_COMPILER_NO_VARIABLE_TEMPLATES
	#if !defined(EA_COMPILER_NO_VARIABLE_TEMPLATES)
		static_assert(pi<int>      == 3, "variable template failure");
		static_assert(pi<intmax_t> == 3, "variable template failure");
		static_assert(pi<double>   == 3.1415926535897932385, "variable template failure");
	#endif


	// Test EA_COMPILER_NO_INITIALIZER_LISTS
	#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
	{
		int a = { 1 };
		EATEST_VERIFY(a == 1);

		int* e{};
		EATEST_VERIFY(!e);

		double x = double{1};
		EATEST_VERIFY(x == 1.0);

		//Disabled until we have a compiler and standard library that can exercise this.
		//#include <initializer_list>
		//eastl::vector<eastl::string, int> anim = { {"bear", 4}, {"cassowary", 2}, {"tiger", 7} };
		//EATEST_VERIFY(!anim.empty());

		// Other tests to do.
		//std::complex<double> z{1,2};
		//eastl::vector<int>{1, 2, 3, 4}; 
		//f({"Nicholas","Annemarie"});
		//return { "Norah" };
	}
	#endif


	// Test EA_COMPILER_NO_NORETURN / EA_NORETURN
	#if !defined(EA_COMPILER_NO_NORETURN) && !defined(EA_PLATFORM_PS4) // Kettle SDK up to at least v.915 has a broken definition of the exit() function and fails to compile the valid code below.
	{
		struct NoReturnTest
		{
			EA_NORETURN void DoesNotReturn()
				{ exit(0); }

			int DoesReturn()
				{ return 17; }
		};

		NoReturnTest nrt;
		if(nrt.DoesReturn() == 18)
			nrt.DoesNotReturn();
	}
	#endif


	// Test EA_COMPILER_NO_CARRIES_DEPENDENCY / EA_CARRIES_DEPENDENCY
	#if !defined(EA_COMPILER_NO_CARRIES_DEPENDENCY)
	{
		struct CarriesDependencyTest
		{
			CarriesDependencyTest() : mX(0){}

			EA_CARRIES_DEPENDENCY int* Test1(){ return &mX; }
			void Test2(int* f EA_CARRIES_DEPENDENCY) { char buffer[32]; sprintf(buffer, "%p", f); }

			int mX;
		};

		CarriesDependencyTest cdt;
		cdt.Test2(cdt.Test1());
	}
	#endif


	// Test EA_COMPILER_NO_FALLTHROUGH / EA_FALLTHROUGH
	#if !defined(EA_COMPILER_NO_FALLTHROUGH)
	{
		int i = 1;
		switch (i)
		{
			case 1:
				i++;

			EA_FALLTHROUGH; 
			case 2: { i = 42; }
				break;
		}

		EATEST_VERIFY(i == 42);
	}
	#endif


	// Test EA_COMPILER_NO_NODISCARD / EA_NODISCARD
	#if !defined(EA_COMPILER_NO_NODISCARD)
	{
		struct EA_NODISCARD DoNotDiscardMe {};
		auto result = [](void) -> DoNotDiscardMe { return {}; }();
		(void)result;  // use the result to avoid [[nodiscard]] compiler warnings
	}
	#endif


	// Test EA_COMPILER_NO_MAYBE_UNUSED / EA_MAYBE_UNUSED
	#if !defined(EA_COMPILER_NO_MAYBE_UNUSED)
	{
		{
			EA_MAYBE_UNUSED int notAlwaysUsed = 42;

			// Do not use expressions below. It defeats the purpose of the test.
			// (void)notAlwaysUsed;
			// EA_UNUSED(notAlwaysUsed);
		}

		{
			[](EA_MAYBE_UNUSED bool b1, EA_MAYBE_UNUSED bool b2) { EA_ASSERT(b1 && b2); }(true, true);
		}
	}
	#endif

	#if !defined(EA_COMPILER_NO_NONSTATIC_MEMBER_INITIALIZERS)
	{
		struct NonstaticInitializerTest
		{
			int a = 7;
			int b = a;
		};

		NonstaticInitializerTest nit;
		EATEST_VERIFY((nit.a == 7) && (nit.b == 7));
	}
	#endif

	#if !defined(EA_COMPILER_NO_RIGHT_ANGLE_BRACKETS)
	{
		eastl::vector<eastl::vector<int>> listList;
		EA_UNUSED(listList);
	}
	#endif


	#if !defined(EA_COMPILER_NO_ALIGNOF)
	{
		char buffer[32];
		sprintf(buffer, "%u", (unsigned)alignof(uint64_t));
	}
	#endif


	#if !defined(EA_COMPILER_NO_ALIGNAS)
	{
		struct alignas(32) AlignAsTest1
			{ float mData[4]; };

		struct alignas(uint64_t) AlignAsTest2
			{ float mData[4]; };

		char buffer[32];
		sprintf(buffer, "%u %u", (unsigned)EA_ALIGN_OF(AlignAsTest1), (unsigned)EA_ALIGN_OF(AlignAsTest2));
	}
	#endif


	#if !defined(EA_COMPILER_NO_DELEGATING_CONSTRUCTORS)
	{
		struct DCTest
		{  
			char   mChar;  
			double mDouble;  

			DCTest() : mChar('\0'), mDouble(1.23){ };  
			DCTest(double d, char c): mChar(c), mDouble(d) { }  
			DCTest(char c) : DCTest(1.23, c) { }  
			DCTest(double d): DCTest(d, 'a') { }
			DCTest(char*): DCTest() { }
		};

		DCTest dcTest(1.5);
		EATEST_VERIFY(dcTest.mDouble == 1.5);
	}
	#endif


	#if !defined(EA_COMPILER_NO_INHERITING_CONSTRUCTORS)
	{
		struct B1{
			B1(int x) : mX(x){}
			int mX;
		};

		struct B2{
			B2(int x = 13, int y = 42) : mX(x), mY(y){}
			int mX, mY;
		};

		struct D1 : B1 {
			using B1::B1;
		};

		struct D2 : B2 {
			using B2::B2;
		};

		D1 d1(3);
		D2 d2a(17, 22);
		D2 d2b;

		EATEST_VERIFY((d1.mX == 3) && 
					  (d2a.mX == 17) && (d2a.mY == 22) && 
					  (d2b.mX == 13) && (d2b.mY == 42));
	}
	#endif


	#if !defined(EA_COMPILER_NO_USER_DEFINED_LITERALS)
	{
		// The operators are defined above.

		// Conversion example
		double x = 90.0_deg; // x = 1.570796
		EATEST_VERIFY((x > 1.57) && (x < 1.58));

		// Custom type example
		UDLTest y(123_udl);
		EATEST_VERIFY(y.mX == 123);
	}
	#endif


	#if !defined(EA_COMPILER_NO_STANDARD_LAYOUT_TYPES)
	{
		// We don't currently have a good way of testing this without bringing in <type_traits>.
	}
	#endif


	#if !defined(EA_COMPILER_NO_EXTENDED_SIZEOF)
	{
		struct SizeofTest{
			int32_t mMember;
		};

		const size_t testSize = sizeof(SizeofTest::mMember);
		EATEST_VERIFY(testSize == sizeof(int32_t));
		char buffer[32];
		sprintf(buffer, "%u", (unsigned)testSize);
	}
	#endif


	#if !defined(EA_COMPILER_NO_INLINE_NAMESPACES)
	{
		// The namespaces are defined above.

		INSNamespace::A<INSClass> a;
		int result = g(a);
		EATEST_VERIFY(result == 37);
	}
	#endif


	#if !defined(EA_COMPILER_NO_UNRESTRICTED_UNIONS)
	{
		struct Point {
			int mX, mY;

			Point(int x = 0, int y = 0) : mX(x), mY(y) {}
		};
		 
		union U {
			int    z;
			double w;
			Point  p; // Illegal in C++03; legal in C++11.

			U() { new(&p) Point(); } // Due to the Point member, a constructor definition is now required.
		};
	}
	#endif


	#if !defined(EA_COMPILER_NO_EXPLICIT_CONVERSION_OPERATORS)
	{
		// bool cast test
		struct Testable
		{
			explicit operator bool() const
				{ return false; }

			Testable() : mX(37) { }
			int mX;
		};
		 
		Testable a;

		if(a)
			EATEST_VERIFY(a.mX == 37);

		// Class cast test
		struct Y { 
			int mY;
			Y(int y = 0) : mY(y) { }
		};

		struct Z { 
			int mZ;
			Z(int z = 0) : mZ(z) { }
			explicit operator Y() const { return Y(mZ); }
		};

		Z z(3);
		Y y1(z);     // Direct initialization
		Y y2 = (Y)z; // Cast notation

		EATEST_VERIFY((z.mZ == 3) && (y1.mY == 3) && (y2.mY == 3));
	}
	#endif


	#if !defined(EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS)
	{
		// FunctionTemplateTest is declared above.
		int result = FunctionTemplateTest::AddOne((int)3);
		EATEST_VERIFY(result == 4);
	}
	#endif


	#if !defined(EA_COMPILER_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS)
	{
		struct LocalStruct{};  
		eastl::fixed_vector<LocalStruct, 2, false> localStructArray;
		EATEST_VERIFY(localStructArray.empty());
	}
	#endif


	#if !defined(EA_COMPILER_NO_NOEXCEPT)
	{
		EATEST_VERIFY(NoExceptTestFunction() == 37);
		EATEST_VERIFY(NoExceptTestTemplate<NoExceptTestStruct>() == 37);
	}
	#endif


	#if !defined(EA_COMPILER_NO_RAW_LITERALS)
	{
		// Older versions of GCC are preventing us from using " below in str1. Due to the way the preprocessor
		// works, it encounters what it sees as a string problem before it handles the #if above. No #ifdefs 
		// can make this problem go away.
		const char str1[] = R"(This slash is just a slash: \ This quote is just a quote: ' )";
		const char str2[] = R"delimiter(This slash is just a slash: \ This paren is just a paren: ) )delimiter";
		EA_UNUSED(str1);
		EA_UNUSED(str2);
		
		static_assert(EAArrayCount(str1) == 61, "EA_COMPILER_NO_RAW_LITERALS failure.");
		static_assert(EAArrayCount(str2) == 61, "EA_COMPILER_NO_RAW_LITERALS failure.");
	}
	#endif


	#if !defined(EA_COMPILER_NO_UNICODE_STRING_LITERALS)
	{
		const char8_t  str1[] = u8"Unicode: \u2018.";       // This assumes that \u and \U are supported by the compiler.
		const char16_t str2[] = u"Unicode: \U00002018.";
		const char32_t str3[] = U"Unicode: \U00022018.";

		static_assert(EAArrayCount(str1) == 14, "EA_COMPILER_NO_UNICODE_STRING_LITERALS failure.");
		static_assert(EAArrayCount(str2) == 12, "EA_COMPILER_NO_UNICODE_STRING_LITERALS failure.");
		static_assert(EAArrayCount(str3) == 12, "EA_COMPILER_NO_UNICODE_STRING_LITERALS failure.");
	}
	#endif


	#if !defined(EA_COMPILER_NO_UNICODE_CHAR_NAME_LITERALS)
	{
		const char8_t str1[] = "\u2018\u2019";
		static_assert(EAArrayCount(str1) == 7, "EA_COMPILER_NO_UNICODE_CHAR_NAME_LITERALS failure.");

		#if (EA_WCHAR_SIZE >= 2)
			const wchar_t str2[] = L"\U00002018\U00002019";
			static_assert(EAArrayCount(str2) == 3, "EA_COMPILER_NO_UNICODE_CHAR_NAME_LITERALS failure."); // This test assumes that wchar_t is a 16bit or greater value.
		#endif

		#if defined(EA_CHAR16_NATIVE) && EA_CHAR16_NATIVE
			const char16_t str3[] = u"\U00002018\U00002019";
			static_assert(EAArrayCount(str3) == 3, "EA_COMPILER_NO_UNICODE_CHAR_NAME_LITERALS failure.");
		#endif
	}
	#endif

	#ifndef EA_COMPILER_NO_RVALUE_REFERENCES
	{
		const int MAX_ARR_SIZE = 4096;
		struct StructWithArray { int arr[MAX_ARR_SIZE]; }; 
		static_assert(EAArrayCount(StructWithArray().arr) == MAX_ARR_SIZE, "");
	}
	#endif

	#if !defined(EA_COMPILER_NO_UNIFIED_INITIALIZATION_SYNTAX)
	{
		struct InitTest1
		{
			int    mX;
			double mY;
		};
		 
		struct InitTest2
		{
			InitTest2(int x, double y) : mX{x}, mY{y} {}

			int    mX;
			double mY;
		};
		 
		InitTest1 var1{5, 3.2};
		InitTest2 var2{2, 4.3};

		EATEST_VERIFY(var1.mY == 3.2);
		EATEST_VERIFY(var2.mY == 4.3);
	}
	#endif

	#if !defined(EA_COMPILER_NO_EXTENDED_FRIEND_DECLARATIONS)
	{
		class G;

		class X1 {
			friend G;
		};
	}
	#endif    


	#if !defined(EA_COMPILER_NO_THREAD_LOCAL)
	{
		// We don't yet test this because we don't have a sufficient compiler to test it with.
	}
	#endif

	return nErrorCount;
}


#if defined(EA_COMPILER_MSVC) && EA_COMPILER_VERSION >= 1900	// VS2015+
	EA_DISABLE_VC_WARNING(5029);  // nonstandard extension used: alignment attributes in C++ apply to variables, data members and tag types only
#endif
int TestEACompilerTraits()
{
	int nErrorCount(0);

	// EA_COMPILER_IS_ANSIC
	// EA_COMPILER_IS_C99
	// EA_COMPILER_IS_CPLUSPLUS
	// EA_COMPILER_MANAGED_CPP

	{
		// EA_COMPILER_INTMAX_SIZE

		#if (EA_COMPILER_INTMAX_SIZE == 16)
			/* To do: Test this when we get a machine that supports it (e.g. Linux64)/
			#if defined(__GNUC__)
				#define int128_t  __int128_t
				#define uint128_t __uint128_t
			#endif

			int128_t  x = UINT128_C(0x12345678123456781234567812345678);
			uint128_t y = (x * 2);

			if(x == (int128_t)y)
				DoError(nErrorCount, "EA_COMPILER_INTMAX_SIZE test");
			*/

		#elif (EA_COMPILER_INTMAX_SIZE == 8)
			int64_t  x = UINT64_C(0x1234567812345678);
			uint64_t y = (x * 2);

			if(x == (int64_t)y)
				DoError(nErrorCount, "EA_COMPILER_INTMAX_SIZE test");

		#elif (EA_COMPILER_INTMAX_SIZE == 32)
			int32_t  x = UINT64_C(0x12345678);
			uint32_t y = (x * 2);

			if(x == (int32_t)y)
				DoError(nErrorCount, "EA_COMPILER_INTMAX_SIZE test");

		#else
			int16_t  x = UINT16_C(0x1234);
			uint16_t y = (x * 2);

			if(x == (int16_t)y)
				DoError(nErrorCount, "EA_COMPILER_INTMAX_SIZE test");
		#endif
	}

	{
		// EA_OFFSETOF
		const size_t o = EA_OFFSETOF(OffsetofTestClass, mY);
		EA_DISABLE_VC_WARNING(6326)
		if(o != 4)
			DoError(nErrorCount, "EA_OFFSETOF test");
		EA_RESTORE_VC_WARNING()
	}

	{
		// EA_SIZEOF_MEMBER
		const size_t s = EA_SIZEOF_MEMBER(SizeofMemberTestClass, mY);
		EA_DISABLE_VC_WARNING(6326)
		if(s != 4)
			DoError(nErrorCount, "EA_SIZEOF_MEMBER test");
		EA_RESTORE_VC_WARNING()

		// There have been problems on some platforms (SNC version < 405) where extended sizeof was not properly
		// supported when used within a member function, so we test for that here.

		class TestClass
		{
		public:
			void TestExtendedSizeof(int& nErrorCount)
			{
				EA_DISABLE_VC_WARNING(6326)
				const size_t sizeOfmY = EA_SIZEOF_MEMBER(SizeofMemberTestClass, mY);
				if(sizeOfmY != 4)
					DoError(nErrorCount, "EA_SIZEOF_MEMBER test: within member function");
				EA_RESTORE_VC_WARNING()
			}
		}tc;

		tc.TestExtendedSizeof(nErrorCount);
	}
	
	{   // EA_ALIGN_OF, EA_PREFIX_ALIGN, etc.
		size_t a = EA_ALIGN_OF(int);
		EA_PREFIX_ALIGN(4) int b = 5;
		EA_ALIGN(8) int c;
		int d EA_POSTFIX_ALIGN(8);
		int e EA_POSTFIX_ALIGN(8) = 5;
		int f EA_POSTFIX_ALIGN(8)(5);
		struct EA_ALIGN(8) G { int x; }; 
		struct EA_PREFIX_ALIGN(8) GG { int x; } EA_POSTFIX_ALIGN(8);
		EA_ALIGNED(int, h, 8) = 5;
		EA_ALIGNED(int, i, ALIGNMENT_AMOUNT_16)(5);
		EA_ALIGNED(int, j[3], ALIGNMENT_AMOUNT_16);
		EA_ALIGNED(int, k[3], ALIGNMENT_AMOUNT_16) = { 1, 2, 3 };
		struct EA_ALIGN(8) L { int x; int y; };

		EA_DISABLE_VC_WARNING(4359)  // ARM64: C4359: 'TestEACompilerTraits::X': Alignment specifier is less than actual alignment (4), and will be ignored.
		EA_ALIGN(ALIGNMENT_AMOUNT_32) struct X { int x; int y; } m;
		EA_RESTORE_VC_WARNING()

	  //int N[3] EA_PACKED; // Some compilers (e.g. GCC) don't support this or ignore this and generate a warning.
		struct P { int x EA_PACKED; int y EA_PACKED; };
		struct Q { int x; int y; } EA_PACKED;
		typedef EA_ALIGNED(int, r, ALIGNMENT_AMOUNT_16);
		r rInstance;
		typedef EA_ALIGNED(Q, X16, ALIGNMENT_AMOUNT_16);
		X16 x16Instance;

		char buffer[256];
		sprintf(buffer, "%p %p %p %p %p %p %p %p %p %p %p %p %p", &a, &b, &c, &d, &e, &f, &h, &i, &j, &k, &m, &rInstance, &x16Instance);
	}

	{ // Test EA_ALIGN_OF
		if(EA_ALIGN_OF(int8_t) != sizeof(int8_t))   // This may not be a kosher test.
			DoError(nErrorCount, "EA_ALIGN_OF test (int16_t)");

		if(EA_ALIGN_OF(int16_t) != sizeof(int16_t)) // This may not be a kosher test.
			DoError(nErrorCount, "EA_ALIGN_OF test (int16_t)");

		if(EA_ALIGN_OF(int32_t) != sizeof(int32_t)) // This may not be a kosher test.
			DoError(nErrorCount, "EA_ALIGN_OF test (int32_t)");

		#if !defined(EA_ABI_ARM_APPLE)
		if(EA_ALIGN_OF(int64_t) != sizeof(int64_t)) // This may not be a kosher test.
			DoError(nErrorCount, "EA_ALIGN_OF test (int64_t)");
		#endif

		typedef void (*AlignTestFunctionType)();
		if(EA_ALIGN_OF(AlignTestFunctionType) != sizeof(void*)) // This may not be a kosher test.
			DoError(nErrorCount, "EA_ALIGN_OF test (AlignTestFunctionType)");
	}

	{ // Test EA_ALIGN
		#ifdef EA_ALIGN
			char buffer[32];
			
			EA_ALIGN(ALIGNMENT_AMOUNT_64) int x(0);
			sprintf(buffer, "%d", x);
			if(buffer[0] != '0')
				DoError(nErrorCount, "EA_ALIGN test 1");
			if((intptr_t)&x & (ALIGNMENT_AMOUNT_64 -1))
				DoError(nErrorCount, "EA_ALIGN test 2");

			EA_ALIGN(ALIGNMENT_AMOUNT_64) ClassWithDefaultCtor    cdcA;
		  //EA_ALIGN(64) ClassWithoutDefaultCtor cwdcA;
			if((intptr_t)&cdcA & (ALIGNMENT_AMOUNT_64 -1))
				DoError(nErrorCount, "EA_ALIGN test 3");

			EA_ALIGN(ALIGNMENT_AMOUNT_64) ClassWithDefaultCtor    cdcB(3);
			if((intptr_t)&cdcB & (ALIGNMENT_AMOUNT_64 -1))
				DoError(nErrorCount, "EA_ALIGN test 4");

			EA_ALIGN(ALIGNMENT_AMOUNT_64) ClassWithoutDefaultCtor cwdcB(3);
			if((intptr_t)&cwdcB & (ALIGNMENT_AMOUNT_64 -1))
				DoError(nErrorCount, "EA_ALIGN test 5");
		#else
			DoError(nErrorCount, "EA_ALIGN test 6");
		#endif
	}

	{ // Test EA_PREFIX_ALIGN
		#ifdef EA_PREFIX_ALIGN
			char buffer[32];
			EA_PREFIX_ALIGN(ALIGNMENT_AMOUNT_64) int x(0);
			sprintf(buffer, "%d", x);
			if(buffer[0] != '0')
				DoError(nErrorCount, "EA_PREFIX_ALIGN test 1");

			EA_PREFIX_ALIGN(64) ClassWithDefaultCtor    cdcA;
		  //EA_PREFIX_ALIGN(64) ClassWithoutDefaultCtor cwdcA;

			EA_PREFIX_ALIGN(64) ClassWithDefaultCtor    cdcB(3);
			EA_PREFIX_ALIGN(64) ClassWithoutDefaultCtor cwdcB(3);
		#else
			DoError(nErrorCount, "EA_PREFIX_ALIGN test 2");
		#endif
	}


	{ // Test EA_POSTFIX_ALIGN
		#ifdef EA_POSTFIX_ALIGN
			char buffer[32];
			int x EA_POSTFIX_ALIGN(ALIGNMENT_AMOUNT_64) = 0;
			sprintf(buffer, "%d", x);
			if(buffer[0] != '0')
				DoError(nErrorCount, "EA_POSTFIX_ALIGN test 1");

			ClassWithDefaultCtor    cdcA  EA_POSTFIX_ALIGN(ALIGNMENT_AMOUNT_64);
		  //ClassWithoutDefaultCtor cwdcA EA_POSTFIX_ALIGN(64);

			ClassWithDefaultCtor    cdcB  EA_POSTFIX_ALIGN(ALIGNMENT_AMOUNT_64)(3);
			ClassWithoutDefaultCtor cwdcB EA_POSTFIX_ALIGN(ALIGNMENT_AMOUNT_64)(3);
		#else
			DoError(nErrorCount, "EA_POSTFIX_ALIGN test 2");
		#endif
	}


	{ // Test EA_ALIGNED
		#ifdef EA_ALIGNED
			char buffer[64];

			// Verify that a simple declaration works.
			EA_ALIGNED(int, xA, ALIGNMENT_AMOUNT_64); xA = 0;
			sprintf(buffer, "%d", xA);
			if((intptr_t)&xA &  (ALIGNMENT_AMOUNT_64 -1))
				DoError(nErrorCount, "EA_ALIGNED test 1");

			// Verify that a declaration with assignment works.
			EA_ALIGNED(int, xB, ALIGNMENT_AMOUNT_64) = 0;
			sprintf(buffer, "%d", xB);
			if((intptr_t)&xB &  (ALIGNMENT_AMOUNT_64 -1))
				DoError(nErrorCount, "EA_ALIGNED test 2");

			// Verify that a declaration with construction works.
			EA_ALIGNED(int, xC, ALIGNMENT_AMOUNT_64)(0);
			sprintf(buffer, "%d", xC);
			if((intptr_t)&xC &  (ALIGNMENT_AMOUNT_64 -1))
				DoError(nErrorCount, "EA_ALIGNED test 3");

			// Verify that a typedefd declaration works.
			typedef EA_ALIGNED(int, int16, ALIGNMENT_AMOUNT_16);
			int16 n16 = 0;
			sprintf(buffer, "%p", &n16);
			if((intptr_t)&n16 & (ALIGNMENT_AMOUNT_16 - 1))
				DoError(nErrorCount, "EA_ALIGNED test 4");

			// Verify that the following tests compile. These tests are here
			// because the SN compiler (EDG front-end) has some problems with
			// GCC compatibility related to the 'aligned' __attribute__.
			ClassWithDefaultCtor    cdc;
			ClassWithoutDefaultCtor cwdc(3);
			sprintf(buffer, "%p%p", &cdc, &cwdc);

			// Verify that regular usage of EA_ALIGNED works.
			EA_ALIGNED(ClassWithDefaultCtor, cdc16A, ALIGNMENT_AMOUNT_16);
			//EA_ALIGNED(ClassWithoutDefaultCtor, cwdcA, 16);   // Doesn't have a default ctor, so this can't be done.
			sprintf(buffer, "%p%p", &cdc16A, (void*)NULL);

			// Verify that argument usage of EA_ALIGNED works.
			EA_ALIGNED(ClassWithDefaultCtor, cdcB, ALIGNMENT_AMOUNT_16)(3);
			EA_ALIGNED(ClassWithoutDefaultCtor, cwdcB, ALIGNMENT_AMOUNT_16)(3);
			sprintf(buffer, "%p%p", &cdcB, &cwdcB);

			// Verify that usage of EA_ALIGNED works within a typedef.
			typedef EA_ALIGNED(ClassWithDefaultCtor, ClassWithDefaultCtor16, ALIGNMENT_AMOUNT_16);
			ClassWithDefaultCtor16 cdcC(3);
			typedef EA_ALIGNED(ClassWithoutDefaultCtor, ClassWithoutDefaultCtor16, ALIGNMENT_AMOUNT_16);
			ClassWithoutDefaultCtor16 cwdcC(3);
			sprintf(buffer, "%p%p", &cdcC, &cwdcC);
		#else
			DoError(nErrorCount, "EA_ALIGNED test");
		#endif
	}


	{ // Test EA_NO_INLINE / EA_PREFIX_NO_INLINE / EA_POSTFIX_NO_INLINE
		DoNothingInline();
		DoNothingPrefixInline();
	}


	{ // Test EA_FORCE_INLINE / EA_PREFIX_FORCE_INLINE / EA_POSTFIX_FORCE_INLINE
		DoNothingForceInline();
		DoNothingPrefixForceInline();
	}

	{ // Test EA_FORCE_INLINE_LAMBDA
		auto testLambda = []() EA_FORCE_INLINE_LAMBDA
		{
		};
		testLambda();
	}


	{ // Test EA_PACKED
		#ifdef EA_PACKED
			char buffer[32];
			struct X { int x; } EA_PACKED;
			X x = { 0 };
			sprintf(buffer, "%d", x.x);
			if(buffer[0] != '0')
				DoError(nErrorCount, "EA_PACKED test");
		#else
			DoError(nErrorCount, "EA_PACKED test");
		#endif
	}


	{ // Test EA_LIKELY

		if(EA_UNLIKELY(nErrorCount > 0))
		{
			if(EA_LIKELY(nErrorCount == 999999)) // Actually this isn't likely, but that's beside the point.
				DoError(nErrorCount, "EA_LIKELY test");
		}
	}


	{ // Test EA_INIT_PRIORITY

		// We don't test that the init priority succeeded in modifying the init priority.
		// We merely test that this compiles on all platforms and assume the compiler's 
		// support of this is not broken.
		if(gInitPriorityTestClass0.mX != 0)
			DoError(nErrorCount, "EA_INIT_PRIORITY test.");

		if(gInitPriorityTestClass1.mX != 1)
			DoError(nErrorCount, "EA_INIT_PRIORITY test.");
	}


	{ // Test EA_INIT_SEG
		// We don't test that the init_seg succeeded in modifying the init priority.
		// We merely test that this compiles on all platforms and assume the compiler's 
		// support of this is not broken.
		if(gInitSegTestSection.mX != 2300)
			DoError(nErrorCount, "EA_INIT_SEG test.");
	}


	{ // Test EA_MAY_ALIAS
		// We don't test that the init priority succeeded in modifying the init priority.
		// We merely test that this compiles on all platforms and assume the compiler's 
		// support of this is not broken.
		if(gPtr0 != NULL)
			DoError(nErrorCount, "EA_MAY_ALIAS test.");

		if(gPtr1 != NULL)
			DoError(nErrorCount, "EA_MAY_ALIAS test.");
	}


	{ // Test EA_ASSUME
		switch (nErrorCount / (nErrorCount + 1))
		{
			case 0:
				Stricmp("nop0", "nop0");
				break;
			case 1:
				Stricmp("nop1", "nop1");
				break;
			default:
				EA_ASSUME(0);
		}
	}


	{ // Test EA_PURE
		if(!PureFunction())
			DoError(nErrorCount, "EA_PURE test");
	}


	{ // EA_WEAK
		if(gWeakVariable != 1)
			DoError(nErrorCount, "EA_WEAK test");
	}


	{ // Test EA_NO_VTABLE
		NoVTable1 nvt1;
		NoVTable2 nvt2;
		nvt1.InterfaceFunction();
		nvt2.InterfaceFunction();
	}

	{ // Test EA_WCHAR_SIZE
		EA_DISABLE_VC_WARNING(6326)
		#ifdef EA_WCHAR_SIZE
			if((EA_WCHAR_SIZE != 1) && (EA_WCHAR_SIZE != 2) && (EA_WCHAR_SIZE != 4))
				DoError(nErrorCount, "EA_WCHAR_SIZE test");
		#else
			DoError(nErrorCount, "EA_WCHAR_SIZE test");
		#endif
		EA_RESTORE_VC_WARNING()
	}


	{ // Test EA_RESTRICT
		struct TestRestrict{
			static size_t Test(char* EA_RESTRICT p){ return sizeof(p); }
		};
		char* p = NULL;
		if(TestRestrict::Test(p) == 0) // This isn't a real test. If there is a failure, it will happen at compile time.
			DoError(nErrorCount, "EA_RESTRICT test");
	}


	{ // Test EA_DEPRECATED
		/* This causes warnings on compilers, so just disable it.
		#if defined(EA_DEPRECATED) && (!defined(__GNUC__) || ((__GNUC__ * 100 + __GNUC_MINOR__) < 402)) // GCC 4.2+ is converting deprecated into an error instead of a warning.
			char buffer[32];
			EA_DEPRECATED int x(0);
			sprintf(buffer, "%d", x); (void)x;
			if(buffer[0] != '0')
				DoError(nErrorCount, "EA_DEPRECATED test");
		#elif !defined (EA_DEPRECATED)
			DoError(nErrorCount, "EA_DEPRECATED test");
		#endif
		*/
	}


	{ // Test EA_PASCAL
		#ifdef EA_PASCAL
			struct X{ void EA_PASCAL DoNothing(){} };
			X x;
			x.DoNothing();
		#else
			DoError(nErrorCount, "EA_PASCAL test");
		#endif
	}


	{ // Test EA_PASCAL_FUNC
		#ifdef EA_PASCAL_FUNC
			struct X{ void EA_PASCAL_FUNC(DoNothing()){} };
			X x;
			x.DoNothing();
		#else
			DoError(nErrorCount, "EA_PASCAL_FUNC test");
		#endif
	}


	// EA_SSE
	// Not sure how to properly test at this time.

	{ // EA_FP16C
		#if EA_FP16C
			// For this test just try to call an intrinsic that is only
			// available when FP16C is available.  The test can make sure the
			// platform actually supports FP16C when it claims to support it,
			// but it can't verify a platform doesn't support FP16C.
			_mm_cvtph_ps(_mm_set1_epi32(42));
		#endif
	}

	{ // EA_IMPORT
		// Not possible to do this because import means it will come from outside.
		//struct X{ EA_IMPORT void DoNothing(){} };
		//X x;
		//x.DoNothing();
	}


	{ // EA_EXPORT
		struct X{ EA_EXPORT void DoNothing(){} };
		X x;
		x.DoNothing();
	}


	// EA_PREPROCESSOR_JOIN
	// EA_STRINGIFY
	{
		char      buffer[32];
		char      bufferExpected[32];
		const int line = (__LINE__ + 2);

		sprintf(buffer, "%s %s", EA_STRINGIFY(EA_PREPROCESSOR_JOIN(test_, __LINE__)), EA_STRINGIFY(__LINE__));
		sprintf(bufferExpected, "test_%d %d", line, line);

		if(strcmp(buffer, bufferExpected) != 0)
			DoError(nErrorCount, "EA_PREPROCESSOR_JOIN/EA_STRINGIFY test");
	}


	{ // EAArrayCount
		const int    testArray[13] = { 0 };
		const size_t arrayCount = EAArrayCount(testArray);

		EA_DISABLE_VC_WARNING(6326)
		if((arrayCount != 13) || (testArray[0] != 0))
			DoError(nErrorCount, "EAArrayCount test");
		EA_RESTORE_VC_WARNING()

		const float testArray2[EAArrayCount(testArray)] = {};
		static_assert(EAArrayCount(testArray2) == EAArrayCount(testArray), "Array counts should be equivalent.");
		static_assert(EAArrayCount(testArray2) == 13, "Float array should have 13 elements.");

		EA_DISABLE_VC_WARNING(6326)
		if (EAArrayCount(testArray2) != EAArrayCount(testArray))
			DoError(nErrorCount, "EAArrayCount - Array counts should be equivalent.");
		EA_RESTORE_VC_WARNING()

		EA_DISABLE_VC_WARNING(6326)
		if (EAArrayCount(testArray2) != 13)
			DoError(nErrorCount, "EAArrayCount - Float array should have 13 elements.");
		EA_UNUSED(testArray2);
		EA_RESTORE_VC_WARNING()

		// Regresssion of user bug report that static_assert<member array> fails with some C++11 compilers.
		// We revised the templated definition of EAArrayCount to deal with the failure.
		struct Example
		{
			int32_t mItems[7];
			Example()
				{ static_assert(EAArrayCount(mItems) == 7, "invalid size"); memset(mItems, 0x77, sizeof(mItems)); } // This was failing with the original templated version of EAArrayCount.
		};

		Example example;
		EATEST_VERIFY(example.mItems[0] == 0x77777777);
	}

	{ // static_assert

		// Should succeed.
		static_assert(sizeof(int32_t) == 4, "static_assert failure");

		// Should fail.
		//static_assert(sizeof(int32_t) == 8, "static_assert failure");
	}

	{ // EA_OPTIMIZE_OFF / EA_OPTIMIZE_ON
		int result = DisabledOptimizations(2);

		if(result != 2*37)
			DoError(nErrorCount, "EA_OPTIMIZE_OFF test");
	}

	{ // EA_UNUSED
		FunctionWithUnusedVariables(3);
	}

	{ // EA_EXTERN_TEMPLATE

		eabase_template<char> x;
		x.value = 0;
		if(x.GetValue() != 0)
			DoError(nErrorCount, "EA_EXTERN_TEMPLATE test");
	}

	{ // EA_FUNCTION_DELETE
		EA_FUNCTION_DELETE_Test test(17);
		EATEST_VERIFY(test.x == 17);
	}

	{ // EA_NON_COPYABLE / EANonCopyable
		NonCopyableA ncA1;
		ncA1.x = 1;
		//NonCopyableA ncA2(ncA1);      // Both of these lines should result in 
		//ncA1 = ncA1;                  // compiler errors if enabled.
		EA_UNUSED(ncA1);

		NonCopyableB ncB1;
		ncB1.x = 1;
		//NonCopyableB ncB2(ncB1);      // Both of these lines should result in 
		//ncB1 = ncB1;                  // compiler errors if enabled.
		EA_UNUSED(ncB1);

		NonCopyableSubclass ncs1(3); 
		//NonCopyableSubclass ncs2(ncs1); // Both of these lines should result in 
		//ncs2 = ncs2;                    // compiler errors if enabled.
		EATEST_VERIFY(ncs1.mX == 3);

		struct NonCopyableLocal
		{
			NonCopyableLocal(){}
			int x;

			EA_NON_COPYABLE(NonCopyableLocal)
		};
		NonCopyableLocal ncLocal1;
		ncLocal1.x = 1;
		//NonCopyableLocal ncLocal2(ncLocal1);  // Both of these lines should result in 
		//ncLocal1 = ncLocal1;                  // compiler errors if enabled.
		EA_UNUSED(ncLocal1);
	}

	return nErrorCount;
}
#if defined(EA_COMPILER_MSVC) && EA_COMPILER_VERSION >= 1900	// VS2015+
	EA_RESTORE_VC_WARNING();
#endif


/////////////////////////////////////////////////
// nullptr test
/////////////////////////////////////////////////

#if !defined(EA_HAVE_nullptr_IMPL)
	#define EA_RTTI_ENABLED 0 // This is something that ideally would be defined in EABase.


	int mfCCount = 0;
	struct C
	{
		void mf()
		{
			mfCCount++;
		}
	};


	int fDoubleCount = 0;
	static void f(double*)
	{
		fDoubleCount++;
	}


	int fIntCount = 0;
	static void f(int)
	{
		fIntCount++;
	}


	int gTCount = 0;
	template<typename T>
	void g(T*)
	{
		gTCount++;
	}

	int hTCount = 0;
	template<typename T>
	void h(T)
	{
		hTCount++;
	}
#endif


static int TestNullPtr()
{
	int nErrorCount(0);

	#if defined(EA_HAVE_nullptr_IMPL) // If the compiler provides a native version... 
		// Don't question it. VC++ nullptr -seems- to be not entirely conforming anyway.
	#else
		using namespace std;

		// DoError("TestNullptr\n");
		void* pv = nullptr;                         // OK
		EATEST_VERIFY(pv == 0);
		EATEST_VERIFY(pv == nullptr);
		#ifndef __MWERKS__                          // Currently the Metrowerks compiler crashes on this code.
			EATEST_VERIFY(nullptr == pv);
		#endif

		pv = &pv;                                   // OK
		EATEST_VERIFY(pv != 0);
		#if !defined(__GNUC__) || (__GNUC__ > 3)
			EATEST_VERIFY(pv != nullptr);
		#endif

		const char* pc = nullptr;                   // OK
		EATEST_VERIFY(pc == 0);
		EATEST_VERIFY(pc == nullptr);
		#ifndef __MWERKS__
			EATEST_VERIFY(nullptr == pc);
		#endif

		C* pC = nullptr;                            // OK
		EATEST_VERIFY(pC == 0);
		EATEST_VERIFY(pC == nullptr);
		#ifndef __MWERKS__
			EATEST_VERIFY(nullptr == pC);
		#endif

		f(nullptr);                                 // OK. Calls f(double*).
		EATEST_VERIFY(fDoubleCount == 1);

		f(0);                                       // OK. Calls f(int)
		EATEST_VERIFY(fIntCount == 1);

		//g(nullptr);                               // Not OK. Can't deduce T

		h(0);                                       // OK. Deduces T = int
		EATEST_VERIFY(hTCount == 1);

		h(nullptr);                                 // OK. Deduces T = nullptr_t
		EATEST_VERIFY(hTCount == 2);

		h((float*)nullptr);                         // OK. Deduces T = float*
		EATEST_VERIFY(hTCount == 3);

		void (C::*pmf)() = 0;                       // OK
		EATEST_VERIFY(pmf == 0);

		#if !defined(__GNUC__) || (__GNUC__ > 3)
			void (C::*pmf2)() = nullptr;            // OK
			EA_UNUSED(pmf2);
			#ifndef __MWERKS__                      // CodeWarrior is not following the C++ Standard properly.
				EATEST_VERIFY(pmf2 == 0);
				EATEST_VERIFY(pmf2 == nullptr);
				EATEST_VERIFY(nullptr == pmf2);
			#endif
		#endif

		#if !defined(__GNUC__) || (__GNUC__ > 3)
			void (C::*pmf3)() = &C::mf;
			#ifndef __MWERKS__
				EATEST_VERIFY(pmf3 != nullptr);
			#endif
		#endif

		nullptr_t n1 = nullptr, n2 = nullptr;       // OK
		n1 = n2;                                    // OK
		h(n1);

		//const int const0 = 0;
		//if(const0 == nullptr) {}                  // Not OK.

		//int n = 0;
		//if(n == nullptr) {}                       // Not OK. 

		//nullptr_t* pN = &n1;                      // Not OK. Address can't be taken.

		EATEST_VERIFY(!nullptr);                    // Supposedly OK, but VC++ doesn't accept it.
		if(nullptr)                                 // Supposedly OK, but VC++ doesn't accept it.
			EATEST_VERIFY(false);

		int   val = 0;
		char* ch3 = val ? nullptr : nullptr;        // OK.
		EATEST_VERIFY(ch3 == 0);

		//char* ch4 = val ? 0       : nullptr;      // Not OK. Types are not compatible.
		//int   n3  = val ? nullptr : nullptr;      // Not OK. nullptr can't be converted to int.
		//int   n4  = val ? 0       : nullptr;      // Not OK. Types are not compatible.

		// void* p = 0;
		// reinterpret_cast<nullptr>(p);            // Not OK. But all compilers allow this. A reinterpret_cast cannot be used to convert a value of any type to the type std::nullptr_t.

		//This is supposed to succeed, but we can't make it so, given the conflicting requirements of the C++ and nullptr standards.
		//EATEST_VERIFY(sizeof(nullptr) == sizeof(void*));    // I don't currently have a means to make this work. See the class for why.

		#ifndef __MWERKS__
			nullptr_t n3 = nullptr, n4 = nullptr;
			EATEST_VERIFY(n3 == n4);
			EATEST_VERIFY(!(n3 != n4));
			EATEST_VERIFY(n3 <= n4);
			EATEST_VERIFY(n3 >= n4);
			EATEST_VERIFY(!(n3 < n4));
			EATEST_VERIFY(!(n3 > n4));
		#endif


		#if EA_RTTI_ENABLED
			typeid(nullptr);                        // OK
		#endif

		#ifndef EA_COMPILER_NO_EXCEPTIONS
			try{
				pv = 0;
				throw nullptr;                      // OK
			}
			catch(nullptr_t n)
			{
				EATEST_VERIFY(n == pv);             // OK
				h(n);
			}
		#endif
	#endif // EA_HAVE_nullptr_IMPL

	return nErrorCount;
}


static int TestEAHave()
{
	int nErrorCount(0);

	// EA_HAVE_XXX_DECL
	//
	// We don't have a simple way to test these, as they indicate the presence of 
	// declarations and not necessarily the presence of implementations.
	//
	// EA_HAVE_mkstemps_DECL
	// EA_HAVE_gettimeofday_DECL
	// EA_HAVE_strcasecmp_DECL
	// EA_HAVE_strncasecmp_DECL
	// EA_HAVE_mmap_DECL
	// EA_HAVE_fopen_DECL
	// EA_HAVE_ISNAN(x)
	// EA_HAVE_ISINF(x)
	// EA_HAVE_itoa_DECL
	// EA_HAVE_nanosleep_DECL
	// EA_HAVE_utime_DECL
	// EA_HAVE_ftruncate_DECL
	// EA_HAVE_localtime_DECL
	// EA_HAVE_pthread_getattr_np_DECL

	#if defined(EA_HAVE_ISNAN)
		EATEST_VERIFY(EA_HAVE_ISNAN(1.f) == 0);
	#endif
	#if defined(EA_HAVE_ISINF)
		EATEST_VERIFY(EA_HAVE_ISINF(1.f) == 0);
	#endif


	// EA_HAVE_XXX_IMPL

	#if defined(EA_HAVE_WCHAR_IMPL)
		size_t wlen = wcslen(L"test");
		EATEST_VERIFY(wlen == 4); // Expect success.
	#endif

	#if defined(EA_HAVE_getenv_IMPL)
		char* p = getenv("nonexistent_asdf");
		EATEST_VERIFY(!p); // Expect failure.
	#endif

	#if defined(EA_HAVE_setenv_IMPL)
		// http://pubs.opengroup.org/onlinepubs/009695399/functions/setenv.html
		// int setenv(const char *envname, const char *envval, int overwrite);
		setenv("test_asdf", "value", 0); // We ignore the return value, as we can't tell if the platform allows it.
	#endif

	#if defined(EA_HAVE_unsetenv_IMPL)
		unsetenv("test_asdf"); // Ignore the return value.
	#endif

	#if defined(EA_HAVE_putenv_IMPL)
		// int putenv(char* string);
		char str[] = "a=b";
		#if defined(EA_PLATFORM_MICROSOFT) && defined(EA_COMPILER_MICROSOFT)
			// Microsoft uses _putenv, while others use putenv.
			int putenvSuccess = _putenv(str);
		#else
			int putenvSuccess = putenv(str);
		#endif
		EATEST_VERIFY(putenvSuccess == 0);
	#endif

	#if defined(EA_HAVE_time_IMPL)
		time_t timeResult = time(NULL);
		EATEST_VERIFY(timeResult != 0); // Expect success.
	#endif

	#if defined(EA_HAVE_clock_IMPL)
		// http://www.cplusplus.com/reference/ctime/clock/ 
		clock_t clockResult = clock();
		EATEST_VERIFY(clockResult != (clock_t) -1); // Expect success.
	#endif

	#if defined(EA_HAVE_fopen_IMPL)
		// We don't have a portable way of testing the success of this, as different platforms have different file systems and requirements.
		// since we want this to fail, we will use a normal Windows path as some platforms /require/ a windows-like mount path else they call abort()
		FILE* pFile = fopen("Q:\\nonexistent_pleasedontexist", "r");
		EATEST_VERIFY(pFile == NULL); // Expect failure.
		if(pFile)
			fclose(pFile);
	#endif

	#if defined(EA_HAVE_inet_ntop_IMPL)
		char inetResult[32];
		const char* pInetNtopResult = inet_ntop(0, "", inetResult, (uint16_t)EAArrayCount(inetResult)); // Cast to uint16_t because different libraries declare this arg differently, and this is a lowest common denominator.
		EATEST_VERIFY(pInetNtopResult == NULL); // Expect failure.
	#endif

	#if defined(EA_HAVE_inet_pton_IMPL)
		char inetPtonResult[32];
		int inetResultVal = inet_pton(0, "", inetPtonResult);
		EATEST_VERIFY(inetResultVal <= 0); // Expect failure.
	#endif

	#if defined(EA_HAVE_clock_gettime_IMPL)
		struct timespec tp;
		int clockGettimeResult = clock_gettime(CLOCK_MONOTONIC, &tp);
		EATEST_VERIFY(clockGettimeResult <= 0); // Expect success or error.
	#endif

	#if defined(EA_HAVE_getcwd_IMPL)
		{
			char cwdBuffer[1];
			char *result = getcwd(cwdBuffer, EAArrayCount(cwdBuffer));
			EA_UNUSED(result);
		}
	#endif

	#if defined(EA_HAVE_tmpnam_IMPL)
		{
			char tmpnamBuffer[L_tmpnam];
			char *result = tmpnam(tmpnamBuffer);
			EA_UNUSED(result);
		}
	#endif

	#if defined(EA_HAVE_nullptr_IMPL)
		// This is exercised elsewhere in this test.
	#endif

	#if defined(EA_HAVE_std_terminate_IMPL)
		if(nErrorCount == INT_MIN) // This is impossible.
			std::terminate();
	#endif

	#if defined(EA_HAVE_CPP11_ITERATOR_IMPL)
		// <iterator>: std::begin, std::end, std::prev, std::next, std::move_iterator.
		#if defined(EA_HAVE_CPP11_INITIALIZER_LIST)
			eastl::vector<int> intArray;
			EATEST_VERIFY(std::begin(intArray) == std::end(intArray));
		#endif

		char charArray[16] = { 0 };
		EATEST_VERIFY(std::begin(charArray) != std::end(charArray));
	#endif

	#if defined(EA_HAVE_CPP11_SMART_POINTER_IMPL)
		// std::weak_ptr, std::shared_ptr, std::unique_ptr, std::bad_weak_ptr
		std::shared_ptr<int>   spInt;
		std::weak_ptr<int>     wpInt;
		std::unique_ptr<int>   upInt;
	  //std::bad_weak_ptr<int> bwpInt;
	#endif

	#if defined(EA_HAVE_CPP11_FUNCTIONAL_IMPL) && !defined(EA_PLATFORM_ANDROID) // Our Android build system is failing to link _1, _2, etc.
		// function, mem_fn, bad_function_call, is_bind_expression, is_placeholder, reference_wrapper, hash, bind, ref, cref.
		// It turns out that all compiler/library combinations that support this also support C++11 auto, so we can use it.

		#if !defined(EA_PLATFORM_ANDROID) // Our Android build system is failing to link _1, _2, etc.
			using namespace std::placeholders; //for _1, _2, _3...

			int  n = 7;
			auto f = std::bind(BindTestFunction, _2, _1, 42, std::cref(n), n);
			f(1, 2, 1001); // 1 is bound by _2, 2 is bound by _1, 1001 is unused

			BindTestStruct bts;
			auto f2 = std::bind(&BindTestStruct::Test, bts, 95, _1);
			f2(5);
		#endif

		std::hash<uint32_t> hash32;
		EATEST_VERIFY(hash32(37) == hash32(37));
	#endif

	#if defined(EA_HAVE_CPP11_EXCEPTION_IMPL)
		// current_exception, rethrow_exception, exception_ptr, make_exception_ptr
		#if !defined(EA_COMPILER_NO_EXCEPTIONS)
			EA_DISABLE_VC_WARNING(4571)

			if(nErrorCount == 9999999) // This will never be true.
			{
				std::exception_ptr ep = std::make_exception_ptr(std::logic_error("logic_error"));

				try {
					std::rethrow_exception(ep);
				} 
				catch (...) {
					ep = std::current_exception();
					std::rethrow_exception(ep);
				}
			}
			EA_RESTORE_VC_WARNING()
		#endif
	#endif

	#if defined(EA_HAVE_CPP11_TYPE_TRAITS)
		// Some form of type traits have been supported by compilers since well before C++11. But C++11 introduced
		// a number of type traits that weren't previously supported by compilers. We require that full C++11 type
		// traits be supported. See the C++11 Standard, section 20.9.2.

		// We currently test a sampling of specific traits that didn't exist in preliminary standard library versions.
		bool ttResult = std::is_nothrow_move_constructible<int>::value;
		EATEST_VERIFY(ttResult);

		ttResult = std::is_standard_layout<int>::value;
		EATEST_VERIFY(ttResult);
	#endif

	return nErrorCount;
}


static int TestEAAlignment()
{
	// This test does a couple of allocations and for each allocation it determines
	// the minimal alignment. If this (local) minimum is less than the global minimum
	// then the global minimum is updated. After all the allocation sizes and iterations
	// it checks this minimum to make sure that the EABase EA_PLATFORM_MIN_MALLOC_ALIGNMENT
	// is at least that number, since you would never want to ask for finer grained 
	// allocations as malloc can't give them.

	int nErrorCount(0);

	const size_t MAX_SIZE = 128;
	const size_t NUM_ITERATIONS = 32;

	size_t minAlignment = MAX_SIZE;

	for(size_t size = 1; size <= MAX_SIZE; ++size)
	{
			
		for(size_t iteration = 0; iteration < NUM_ITERATIONS; ++iteration)
		{
			void* ptr = malloc(size);        
			size_t address = static_cast<size_t>(reinterpret_cast<uintptr_t>(ptr));

			size_t alignment = MAX_SIZE;
				
			do
			{
				if((address & (alignment - 1)) == 0)
				{
					break;
				}
				else
				{
					alignment >>= 1;
				}

			} while(alignment > 0);

			if(alignment < minAlignment)
				minAlignment = alignment;

			free(ptr);
		}
	} 

	EATEST_VERIFY_F(EA_PLATFORM_MIN_MALLOC_ALIGNMENT <= minAlignment,
					"'EA_PLATFORM_MIN_MALLOC_ALIGNMENT=%d' <= 'minAlignment=%d' failure on '%s'",
					EA_PLATFORM_MIN_MALLOC_ALIGNMENT, minAlignment, EA_PLATFORM_DESCRIPTION);

	return nErrorCount;
}




#include <EABase/eastdarg.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAString.h>


static void TestEAStdargReferenceHelp(char* p, va_list_reference args)
{
	EA::StdC::Sprintf(p, "%d", va_arg(args, int));
}

static void TestEAStdargReference(char* p1, char* p2, ...) // Must be called with two ints for ...
{
	va_list args;
	va_start(args, p2);
	TestEAStdargReferenceHelp(p1, args);    // We pass args to TestEAStdargReferenceHelp by reference, which results in args being 
	TestEAStdargReferenceHelp(p2, args);    // modified upon return. So upon this second call args should have used the first int arg.
	va_end(args);
}


static void TestEAStdargCopy(char* p1, char* p2, ...) // Must be called with two ints for ...
{
	va_list args, argsCopy;
	va_start(args, p2);
	va_copy(argsCopy, args);
	EA::StdC::Vsprintf(p1, "%d", args);
	EA::StdC::Vsprintf(p2, "%d", argsCopy);
	va_end(args);
	va_end(argsCopy);
}



static int TestEAStdarg()
{
	int nErrorCount(0);

	// VA_ARG_COUNT
	static_assert(VA_ARG_COUNT()  == 0, "VA_ARG_COUNT()");
	static_assert(VA_ARG_COUNT(1) == 1, "VA_ARG_COUNT(1)");
	static_assert(VA_ARG_COUNT(2, 2) == 2, "VA_ARG_COUNT(2)");
	static_assert(VA_ARG_COUNT(3, 3, 3) == 3, "VA_ARG_COUNT(3)");
	static_assert(VA_ARG_COUNT(4, 4, 4, 4) == 4, "VA_ARG_COUNT(4)");
	static_assert(VA_ARG_COUNT(5, 5, 5, 5, 5) == 5, "VA_ARG_COUNT(5)");


	char buffer1[64];
	char buffer2[64];

	// va_copy
	TestEAStdargCopy(buffer1, buffer2, 17, 99);
	EATEST_VERIFY((EA::StdC::AtoI32(buffer1) == 17) && (EA::StdC::AtoI32(buffer2) == 17));

	// va_list_reference
	TestEAStdargReference(buffer1, buffer2, 17, 99);
	EATEST_VERIFY((EA::StdC::AtoI32(buffer1) == 17) && (EA::StdC::AtoI32(buffer2) == 99));

	return nErrorCount;
}



static int TestEAUnits()
{
	int nErrorCount(0);

	static_assert(EA_BYTE(64) == 64, "SI units mismatch");
	static_assert(EA_BYTE(1000) == 1000, "SI units mismatch");

	static_assert(EA_KILOBYTE(1) != EA_KIBIBYTE(1), "SI units mismatch");
	static_assert(EA_MEGABYTE(1) != EA_MEBIBYTE(1), "SI units mismatch");
	static_assert(EA_GIGABYTE(1) != EA_GIBIBYTE(1), "SI units mismatch");

	static_assert((4 % EA_BYTE(4*10)) == 4, "Order of operations error"); //If unit macros aren't enclosed in parentheses, this will cause order of operation problems in this situation.
	static_assert((4 % EA_MEBIBYTE(4)) == 4, "Order of operations error"); //If unit macros aren't enclosed in parentheses, this will cause order of operation problems in this situation.
#ifndef EA_PROCESSOR_X86
	static_assert(EA_TERABYTE(1) != EA_TEBIBYTE(1), "SI units mismatch");
	static_assert(EA_PETABYTE(1) != EA_PEBIBYTE(1), "SI units mismatch");
	static_assert(EA_EXABYTE(1)  != EA_EXBIBYTE(1), "SI units mismatch");
#endif

	static_assert(EA_KILOBYTE(1) == 1000, "SI units mismatch");
	static_assert(EA_MEGABYTE(2) == EA_KILOBYTE(2) * 1000, "SI units mismatch");
	static_assert(EA_GIGABYTE(3) == EA_MEGABYTE(3) * 1000, "SI units mismatch");
#ifndef EA_PROCESSOR_X86
	static_assert(EA_TERABYTE(4) == EA_GIGABYTE(4) * 1000, "SI units mismatch");
	static_assert(EA_PETABYTE(5) == EA_TERABYTE(5) * 1000, "SI units mismatch");
	static_assert(EA_EXABYTE(6)  == EA_PETABYTE(6) * 1000, "SI units mismatch");
#endif

	static_assert(EA_KIBIBYTE(1) == 1024, "SI units mismatch");
	static_assert(EA_MEBIBYTE(2) == EA_KIBIBYTE(2) * 1024, "SI units mismatch");
	static_assert(EA_GIBIBYTE(3) == EA_MEBIBYTE(3) * 1024, "SI units mismatch");
#ifndef EA_PROCESSOR_X86
	static_assert(EA_TEBIBYTE(4) == EA_GIBIBYTE(4) * 1024, "SI units mismatch");
	static_assert(EA_PEBIBYTE(5) == EA_TEBIBYTE(5) * 1024, "SI units mismatch");
	static_assert(EA_EXBIBYTE(6) == EA_PEBIBYTE(6) * 1024, "SI units mismatch");
#endif

	return nErrorCount;
}



template<typename Int128T, typename MakeInt128T>
static void TestInt128T(MakeInt128T MakeInt128, const char* errorMsg, int& nErrorCount) 
{
	auto VERIFY = [&](bool result) { if(!result) DoError(nErrorCount, errorMsg); }; 
	const auto TestValue = MakeInt128(0x1234567812345678, 0x1234567812345678);
	const Int128T zero = MakeInt128(0, 0);
	const Int128T one = MakeInt128(0, 1);
	const Int128T two = MakeInt128(0, 2);
	const Int128T big = MakeInt128(0x1234567812345678, 0x1234567812345678);;
	const Int128T negative_one = MakeInt128(0xffffffffffffffff, 0xffffffffffffffff);
	const Int128T half_range = MakeInt128(0x0, 0xffffffffffffffff);

	{
		// Int128T a1 = 42.f;
		// Int128T a2 = 42.0f;
		// Int128T a3 = 42;
		// Int128T a4 = 42u;
		// Int128T a5 = 42ul;
		// Int128T a6 = 42ull;
	}

	// default ctor
	{
		{ Int128T a; (void)a; }
		{ Int128T a{}; (void)a; }

		static_assert(eastl::is_trivially_default_constructible_v<Int128T>, "128-bit integer failure");
	}

	// operator-
	{
		VERIFY(negative_one == -one);
	}

	// operator~
	{
		auto not_one = ~one;
		VERIFY(not_one == MakeInt128(0xffffffffffffffff, 0xfffffffffffffffe));
	}

	// operator+
	{
		VERIFY(zero == +zero);
		VERIFY(one  == +one);
		VERIFY(big  == +big);
	}

	// operator+ 
	// operator- 
	// operator* 
	// operator/ 
	// operator% 
	{
		auto i = MakeInt128(42, 42);

		i = i + one;
		VERIFY(i == MakeInt128(42, 43));

		i = i - one;
		VERIFY(i == MakeInt128(42, 42));

		i = i * two;
		VERIFY(i == MakeInt128(84, 84));

		i = i / two;
		VERIFY(i == MakeInt128(42, 42));
	}

	// operator== / operator!=
	{
		VERIFY(TestValue == MakeInt128(0x1234567812345678, 0x1234567812345678));
		VERIFY(TestValue == TestValue);
		VERIFY(MakeInt128(0x1, 0x1) != MakeInt128(0x1, 0x2));
	}

	// operator<
	{
		VERIFY(zero < one);
		VERIFY(one  < two);
		VERIFY(zero < two);
		VERIFY(zero < big);
		VERIFY(one  < big);

		VERIFY(MakeInt128(123, 122) < MakeInt128(123, 123));
		VERIFY(MakeInt128(122, 123) < MakeInt128(123, 123));
	}

	// operator> / operator>=
	{
		VERIFY(TestValue >  MakeInt128(0, 0x1234567812345678));
		VERIFY(TestValue >= MakeInt128(0, 0x1234567812345678));
		VERIFY(TestValue >= TestValue);
		VERIFY(TestValue >= TestValue);
	}

	// operator< / operator<=
	{
		VERIFY(MakeInt128(0, 0x1234567812345678) < TestValue);
		VERIFY(MakeInt128(0, 0x1234567812345678) <= TestValue);
		VERIFY(TestValue <= TestValue);
		VERIFY(TestValue <= TestValue);
	}

	// operator++
	{
		auto i      = MakeInt128(0, 0);
		VERIFY(i++ == MakeInt128(0, 0));
		VERIFY(++i == MakeInt128(0, 2));
		VERIFY(++i == MakeInt128(0, 3));
		VERIFY(i++ == MakeInt128(0, 3));

		{
			auto n1 = half_range;
			VERIFY(++n1 == MakeInt128(1, 0));
		}
	}

	// operator--
	{
		auto i      = MakeInt128(0, 5);
		VERIFY(i-- == MakeInt128(0, 4));
		VERIFY(--i == MakeInt128(0, 4));
		VERIFY(--i == MakeInt128(0, 3));
		VERIFY(i-- == MakeInt128(0, 2));

		{
			auto n1 = MakeInt128(1, 0);
			VERIFY(n1-- == half_range);
		}
	}

	// operator+=
	// operator-=
	// operator*=
	// operator/=	
	// operator%=
	{
		auto n = MakeInt128(0, 5);

		n += MakeInt128(0, 15);
		VERIFY(n == MakeInt128(0, 20));

		n -= MakeInt128(0, 18);
		VERIFY(n == MakeInt128(0, 2));

		n *= MakeInt128(0, 2);
		VERIFY(n == MakeInt128(0, 4));

		n /= MakeInt128(0, 2);
		VERIFY(n == MakeInt128(0, 2));

		n %= MakeInt128(0, 2);
		VERIFY(n == MakeInt128(0, 0));
	}

	// operator>> 
	// operator<< 
	// operator>>=
	// operator<<=
	{
		auto n = MakeInt128(0, 0x4);

		{
			auto a = n >> 1;
			VERIFY(a == MakeInt128(0, 0x2));

			a >>= 1;
			VERIFY(a == MakeInt128(0, 0x1));
		}

		{
			auto a = n << 1;
			VERIFY(a == MakeInt128(0, 0x8));

			a <<= 1;
			VERIFY(a == MakeInt128(0, 0x10));
		}

		{
			auto a = half_range;

			a <<= 1;
			VERIFY(a == MakeInt128(0x1, 0xfffffffffffffffe));
		}

		{
			auto a = half_range;
			a >>= 1;
			VERIFY(a == MakeInt128(0x0, 0x7fffffffffffffff));
		}
	}

	// operator^ 
	// operator| 
	// operator& 
	// operator^=
	// operator|=
	// operator&=
	{
		const auto n1 = MakeInt128(0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA);
		const auto n2 = MakeInt128(0x5555555555555555, 0x5555555555555555);

		{
			auto i = n1 ^ n2; 
			VERIFY(i == negative_one);

			auto n3 = n1;
			n3 ^= n2;
			VERIFY(n3 == negative_one);
		}

		{
			auto i = n1 | n2; 
			VERIFY(i == negative_one);

			auto n3 = n1;
			n3 |= n2;
			VERIFY(n3 == negative_one);
		}

		{
			auto i = n1 & n2; 
			VERIFY(i == zero);

			auto n3 = n1;
			n3 &= n2;
			VERIFY(n3 == zero);
		}

	}

	// Test loop counter
	{
		{
			int counter = 0;
			Int128T i = MakeInt128(0,0);

			for (; i < MakeInt128(0,10); i++)
				counter++;

			VERIFY(i == MakeInt128(0, counter));
		}

		{
			// int counter = 0;

			// for (Int128T i = 0; i < 10; i++)
			//     counter++;

			// VERIFY(i == counter);
		}
	}
}



static int TestEAInt128_t()
{
	int nErrorCount(0);

	TestInt128T<uint128_t>(UINT128_C, "uint128_t test failure", nErrorCount);
	TestInt128T<int128_t>(INT128_C, "int128_t test failure", nErrorCount);

	return nErrorCount;
}



// EA_WCHAR_UNIQUE
template <typename T>
struct wchar_unique { enum { value = 1 }; };                        // if wchar_t is unique then wchar_unique<wchar_t>::value should be 1
template <> struct wchar_unique<char8_t> { enum { value = 0 }; };   // if wchar_unique is not unique then it should match one of the specializations and the value will be 0.
template <> struct wchar_unique<char16_t> { enum { value = 0 }; };
template <> struct wchar_unique<char32_t> { enum { value = 0 }; };
#if EA_WCHAR_UNIQUE
	static_assert( wchar_unique<wchar_t>::value == 1, "WCHAR_UNIQUE appears to be incorrectly defined to 1 by EABase" );
#else
	static_assert( wchar_unique<wchar_t>::value == 0, "WCHAR_UNIQUE appears to be incorrectly defined to 0 by EABase" );
#endif


//
// Tests for EA_IS_ENABLED
//
#define EABASE_TEST_FEATURE_A EA_ENABLED
#if EA_IS_ENABLED(EABASE_TEST_FEATURE_A)
	// Feature A is enabled
#else
	#error Error EABASE_TEST_FEATURE_A should be enabled.
#endif
// Make sure it is possible to successfully negate the test.
#if !EA_IS_ENABLED(EABASE_TEST_FEATURE_A)
	#error Error EABASE_TEST_FEATURE_A should be enabled.
#endif

#define EABASE_TEST_FEATURE_B EA_DISABLED
#if EA_IS_ENABLED(EABASE_TEST_FEATURE_B)
	#error Error EABASE_TEST_FEATURE_B should be disabled.
#endif
// Make sure it is possible to successfully negate the test.
#if !EA_IS_ENABLED(EABASE_TEST_FEATURE_B)
	// Feature B is not enabled
#else
	#error Error EABASE_TEST_FEATURE_B should be disabled.
#endif

// The test below should cause compilation to fail if it is uncommented.  However we can't
// obviously enable the test because it will break the build.  It should be tested manually
// if changes to EA_IS_ENABLED are made.
//
//   #if EA_IS_ENABLED(EABASE_TEST_FEATURE_WITH_NO_DEFINE)
//   #endif


int EAMain(int, char**)
{
	int nErrorCount = 0;
	int nTotalErrorCount = 0;


	nErrorCount = TestEABase();
	EA::EAMain::Report("EABase test failure count: %d\n\n", nErrorCount);
	nTotalErrorCount += nErrorCount;

	nErrorCount = TestEAResult();
	EA::EAMain::Report("EAResult test failure count: %d\n\n", nErrorCount);
	nTotalErrorCount += nErrorCount;

	nErrorCount = TestEAPlatform();
	EA::EAMain::Report("EAPlatform test failure count: %d\n\n", nErrorCount);
	nTotalErrorCount += nErrorCount;

	nErrorCount = TestEACompiler();
	EA::EAMain::Report("EACompiler test failure count: %d\n\n", nErrorCount);
	nTotalErrorCount += nErrorCount;

	nErrorCount = TestEACompilerTraits();
	EA::EAMain::Report("EACompilerTraits test failure count: %d\n\n", nErrorCount);
	nTotalErrorCount += nErrorCount;

	nErrorCount = TestNullPtr();
	EA::EAMain::Report("nullptr test failure count: %d\n\n", nErrorCount);
	nTotalErrorCount += nErrorCount;

	nErrorCount = TestEAHave();
	EA::EAMain::Report("EAHave test failure count: %d\n\n", nErrorCount);
	nTotalErrorCount += nErrorCount;

	nErrorCount = TestEAAlignment();
	EA::EAMain::Report("EAAlignment test failure count: %d\n\n", nErrorCount);
	nTotalErrorCount += nErrorCount;

	nErrorCount = TestEAStdarg();
	EA::EAMain::Report("EAStdarg test failure count: %d\n\n", nErrorCount);
	nTotalErrorCount += nErrorCount;

	nErrorCount = TestEAUnits();
	EA::EAMain::Report("EAUnits test failure count: %d\n\n", nErrorCount);
	nTotalErrorCount += nErrorCount;

	nErrorCount = TestEAInt128_t();
	EA::EAMain::Report("EAInt128_t test failure count: %d\n\n", nErrorCount);
	nTotalErrorCount += nErrorCount;


	if (nTotalErrorCount == 0)
		EA::EAMain::Report("\nAll tests completed successfully.\n");
	else
		EA::EAMain::Report("\nTests failed. Total error count: %d\n", nTotalErrorCount);
	
	return nTotalErrorCount;
}

EA_RESTORE_VC_WARNING() // for the following from above: EA_DISABLE_VC_WARNING(4265 4296 4310 4350 4481 4530 4625 4626 4996)
