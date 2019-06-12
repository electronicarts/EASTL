/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "TestEABase.h"
#include "TestEABase.h" // Intentionally double-include the same header file, to test it.
#include <EABase/eabase.h>
#include <EABase/eahave.h>

#if defined(_MSC_VER)
	#pragma warning(disable: 4296 4310 4255) // expression is always true, cast truncates const value.
#endif

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

#if defined(EA_COMPILER_MSVC) && defined(EA_PLATFORM_MICROSOFT)
	EA_DISABLE_ALL_VC_WARNINGS()
	#if defined(EA_PLATFORM_XENON)
		#define NOD3D
		#define NONET
		#include <Xtl.h>
	#else
		#pragma warning(disable: 28252)
		#pragma warning(disable: 28253)
		#ifndef WIN32_LEAN_AND_MEAN
			#define WIN32_LEAN_AND_MEAN
		#endif
		#include <Windows.h>
	#endif
	EA_RESTORE_ALL_VC_WARNINGS()
#elif defined(EA_PLATFORM_ANDROID)
	#include <android/log.h>
#endif


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



#if defined(__cplusplus)
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
#endif


// Forward declarations
int  Stricmp(const char* pString1, const char* pString2);
int  TestEABase(void);
int  TestEAResult(void);
int  TestEAPlatform(void);
int  TestNU(void);
int  TestEACompiler(void);
int  TestEACompilerTraits(void);
int  Verify(int bTest, const char* pMessage);

#define DoError(nErrorCount, pMessage) DoErrorC(&nErrorCount, pMessage)
void DoErrorC(int* nErrorCount, const char* pMessage);



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

static void Printf(const char8_t* pFormat, ...)
{
	#if defined(_MSC_VER)
		#define vsnprintf _vsnprintf
	#endif

	if(pFormat)
	{
		char pMessage[512];
		int nReturnValue;

		va_list arguments;
		va_start(arguments, pFormat);
		nReturnValue = vsnprintf(pMessage, EAArrayCount(pMessage), pFormat, arguments);
		va_end(arguments);

		if(nReturnValue > 0)
		{
			#if defined(EA_PLATFORM_ANDROID)
				__android_log_write(ANDROID_LOG_INFO, "EABase.Printf", pMessage);
			#else
				fputs(pMessage, stdout);
			#endif

			#if defined(EA_COMPILER_MSVC) && defined(EA_PLATFORM_MICROSOFT)
				OutputDebugStringA(pMessage);
			#endif
		}
	}
}

/* Test EA_DISABLE_WARNING */
EA_RESTORE_VC_WARNING()
EA_RESTORE_GCC_WARNING()
EA_RESTORE_SN_WARNING()
EA_RESTORE_GHS_WARNING()
EA_RESTORE_EDG_WARNING()
EA_RESTORE_CW_WARNING(10317)



int Verify(int bTest, const char* pMessage)
{
	if(!bTest)
	{
		if(pMessage)
			Printf("Test warning: %s\n", pMessage);
	}

	return bTest ? 0 : 1;
}


void DoErrorC(int* nErrorCount, const char* pMessage)
{
	++*nErrorCount;
	if(pMessage)
		Printf("Test error: %s\n", pMessage);
}


int Stricmp(const char* pString1, const char* pString2)
{
	char c1, c2;

	while((c1 = (char)tolower((int)*pString1++)) == (c2 = (char)tolower((int)*pString2++)))
	{
		if(c1 == 0)
			return 0;
	}
	
	return (c1 - c2);
}


// GCC requires that function attributes be declared in the function 
// declaration and not in the function definition.
// RVCT seems to require attributes to be in both the declaration
// and definition otherwise you get a "declaration is incompatible" error
int PureFunction(void) EA_PURE;

int PureFunction(void) 
#if defined(EA_COMPILER_RVCT)
EA_PURE
#endif
{
	return (strlen("abc") == 3);
}


struct InitPriorityTestClass
{
	int mX;
};



// EA_MAY_ALIAS
void* EA_MAY_ALIAS gPtr0 = NULL;

typedef void* EA_MAY_ALIAS pvoid_may_alias;
pvoid_may_alias gPtr1 = NULL;



// static_asset at global scope
// Should succeed.
static_assert(sizeof(int32_t) == 4, "static_assert failure");
// Should fail.
//static_assert(sizeof(int32_t) == 8, "static_assert failure");



static size_t RestrictTest(char* EA_RESTRICT p)
{
	return sizeof(p);
}


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



int TestEABase(void)
{
	int nErrorCount = 0;


	// Test NULL
	{
		EA_DISABLE_VC_WARNING(6326)
		Verify(NULL == (void*)0, "unspecified test");
		EA_RESTORE_VC_WARNING()
	}

	// Verify sized type sizes
	{
		const ssize_t ss = 1; // Verify that ssize_t is a signed type.

		Verify(sizeof(int8_t)   == 1, "int8_t size test");
		Verify(sizeof(uint8_t)  == 1, "uint8_t size test");
		Verify(sizeof(int16_t)  == 2, "int16_t size test");
		Verify(sizeof(uint16_t) == 2, "uint16_t size test");
		Verify(sizeof(int32_t)  == 4, "int32_t size test");
		Verify(sizeof(uint32_t) == 4, "uint32_t size test");
		Verify(sizeof(int64_t)  == 8, "int64_t size test");
		Verify(sizeof(uint64_t) == 8, "uint64_t size test");

		#if !defined(FLT_EVAL_METHOD)
			#error  EABase should always define FLT_EVAL_METHOD
			Verify(0, "FLT_EVAL_METHOD test: not defined.");
		#else
			#if !defined(__GNUC__) || defined(__USE_ISOC99) // GCC doesn't define float_t/double_t unless __USE_ISOC99 is defined (compiled with -std=c99)
				#if (FLT_EVAL_METHOD == -1)
					// In this case the C99 standard states that the 
					// precision of float_t and double_t is indeterminable.
				#elif (FLT_EVAL_METHOD == 0)
					Verify(sizeof(float_t)  == sizeof(float), "float_t size test");
					Verify(sizeof(double_t) == sizeof(double), "double_t size test");
				#elif (FLT_EVAL_METHOD == 1)
					Verify(sizeof(float_t)  == sizeof(double), "float_t size test");
					Verify(sizeof(double_t) == sizeof(double), "double_t size test");
				#elif (FLT_EVAL_METHOD == 2)
					Verify(sizeof(float_t)  == sizeof(long double), "float_t size test");
					Verify(sizeof(double_t) == sizeof(long double), "double_t size test");
				#else
					DoError(nErrorCount, "FLT_EVAL_METHOD test: invalid value.");
				#endif
			#endif
		#endif

		Verify(sizeof(bool8_t)   == 1, "bool8_t size test");
		Verify(sizeof(intptr_t)  == sizeof(void*), "intptr_t size test");
		Verify(sizeof(uintptr_t) == sizeof(void*), "uintptr_t size test");
		Verify(sizeof(ssize_t)   == sizeof(size_t), "ssize_t size test");
		Verify((ssize_t)((ss ^ ss) - 1) < 0, "ssize_t sign test");
		Verify(sizeof(char8_t) == 1, "char8_t size test");
		Verify(sizeof(char16_t) == 2, "char16_t size test");
		Verify(sizeof(char32_t) == 4, "char32_t test");

		#if (EA_WCHAR_SIZE == 2) || (EA_WCHAR_SIZE == 4)
			Verify(sizeof(wchar_t) == EA_WCHAR_SIZE, "EA_WCHAR_SIZE test");
		#else
			Verify(0, "EA_WCHAR_SIZE test");
		#endif
	}

	// Test char8_t, char16_t, char32_t string literals.
	{
		const char8_t*  p8  = "abc";
		const char8_t   c8  = 'a';

		#ifdef EA_CHAR16
			const char16_t* p16 = EA_CHAR16("abc"); // Under GCC, this assumes compiling with -fshort-wchar
			const char16_t  c16 = EA_CHAR16('\x3001');
		#else
			const char16_t* p16 = NULL;
			const char16_t  c16 = (char16_t)'X';
		#endif

		#if EA_CHAR16_NATIVE
			const char32_t* p32 = EA_CHAR32("abc");
			const char32_t  c32 = EA_CHAR32('\x3001');
		#else
			const char32_t p32[] = { 'a', 'b', 'c', '\0' };   // Microsoft doesn't support 32 bit strings here, and GCC doesn't use them when we compile with -fshort-wchar (which we do).
			#ifdef EA_CHAR16
				const char32_t c32   = EA_CHAR16('\x3001');   // 16 bit should silently convert to 32 bit.
			#else
				const char32_t c32   = (char16_t)'X';         // 16 bit should silently convert to 32 bit.
			#endif
		#endif

		char buffer[128];
		sprintf(buffer, "%p %p %p %p %p %p", &p8, &c8, &p16, &c16, &p32, &c32); // Make possible compiler warnings about unused variables go away.
	}

	#if 0

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

		sprintf(buffer, "%d %d %u %u %d %d %u %u %d %d %u %u %"SCNd64" %"SCNd64" %"SCNu64" %"SCNu64,
				  (int)i8Min,  (int)i8Max,  (unsigned)u8Min,  (unsigned)u8Max, 
				  (int)i16Min, (int)i16Max, (unsigned)u16Min, (unsigned)u16Max, 
				  (int)i32Min, (int)i32Max, (unsigned)u32Min, (unsigned)u32Max, 
				  i64Min, i64Max, u64Min, u64Max);
		if(strcmp(buffer, "-128 127 0 255 -32768 32767 0 65535 -2147483648 2147483647 0 4294967295 -9223372036854775808 9223372036854775807 0 18446744073709551615"))
			DoError(nErrorCount, "INT_C test");

		// Verify the use of hex numbers with INT64_C
		const int64_t i64Hex = INT64_C(0x1111111122222222);
		if(i64Hex != INT64_C(1229782938533634594))
			DoError(nErrorCount, "INT64_C hex error");

		// Verify the use of hex numbers with UINT64_C
		const uint64_t u64Hex = UINT64_C(0xaaaaaaaabbbbbbbb);

		#if defined(__GNUC__) && (__GNUC__ < 4) // If using a broken version of UINT64_C/INT64_C macros...
			const uint64_t temp = 12297829382759365563ULL; 
		#else
			const uint64_t temp = UINT64_C(12297829382759365563);
		#endif

		if(u64Hex != temp)
			DoError(nErrorCount, "UINT64_C hex error");

		// Verify that the compiler both allows division with uint64_t but 
		// also that it allows it via UINT64_MAX. A bad implementation of 
		// UINT64_MAX would cause the code below to mis-execute or not compile.
		const uint64_t resultUint64 = UINT64_MAX / 2;
		if(resultUint64 != UINT64_C(9223372036854775807))
			DoError(nErrorCount, "UINT64_MAX error");
	}

	//Test sized Printf format specifiers
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
		#endif

		void *dPtr = (void*)INT32_MAX, *iPtr = (void*)INT32_MIN, *oPtr = (void*)INT32_MAX, *uPtr = (void*)(uintptr_t)UINT64_MAX, *xPtr = (void*)(uintptr_t)UINT64_MAX, *XPtr = (void*)(uintptr_t)UINT64_MAX;
		sprintf(buffer, "%" PRIdPTR " %" PRIiPTR " %" PRIoPTR " %" PRIuPTR " %" PRIxPTR " %" PRIXPTR, (intptr_t)dPtr, (intptr_t)iPtr, (uintptr_t)oPtr, (uintptr_t)uPtr, (uintptr_t)xPtr, (uintptr_t)XPtr);

		#if (EA_PLATFORM_PTR_SIZE == 4)
			if(Stricmp(buffer, "2147483647 -2147483648 17777777777 4294967295 ffffffff FFFFFFFF"))
				DoError(nErrorCount, "PRIPTR test");
		#else // EA_PLATFORM_PTR_SIZE == 8
			if(Stricmp(buffer, "2147483647 -2147483648 17777777777 18446744073709551615 ffffffffffffffff FFFFFFFFFFFFFFFF"))
				DoError(nErrorCount, "PRIPTR test");
		#endif

		#if defined(_MSC_VER) && (_MSC_VER >= 1300)
			#pragma warning(default: 4313)
		#endif
	}

	//Test sized scanf format specifiers
	{
		#ifdef EA_COMPILER_IS_C99 // Enabled for C99 only because this code will simply crash on many platforms if the format specifiers aren't supported.
			int8_t  d8, i8, o8;
			uint8_t u8, x8;
			sscanf("127 -127 177 255 ff", "%"SCNd8 " %"SCNi8 " %"SCNo8 " %"SCNu8 " %"SCNx8, &d8, &i8, &o8, &u8, &x8);
			if((d8 != 127) || (i8 != -127) || (o8 != 127) || (u8 != 255) || (x8 != 255))
				DoError(nErrorCount, "SCN8 test"); // This is known to fail with compilers such as VC++ which don't support %hh.
		#endif

		int16_t  d16, i16, o16;
		uint16_t u16, x16;
		sscanf("32767 -32768 77777 65535 ffff", "%"SCNd16 " %"SCNi16 " %"SCNo16 " %"SCNu16 " %"SCNx16, &d16, &i16, &o16, &u16, &x16);
		if((d16 != 32767) || (i16 != -32768) || (o16 != 32767) || (u16 != 65535) || (x16 != 65535))
			DoError(nErrorCount, "SCN16 test");

		int32_t  d32, i32, o32;
		uint32_t u32, x32;
		sscanf("2147483647 -2147483648 17777777777 4294967295 ffffffff", "%"SCNd32 " %"SCNi32 " %"SCNo32 " %"SCNu32 " %"SCNx32, &d32, &i32, &o32, &u32, &x32);
		if((d32 != INT32_MAX) || (i32 != INT32_MIN) || (o32 != INT32_MAX) || (u32 != UINT32_MAX) || (x32 != UINT32_MAX))
			DoError(nErrorCount, "SCN32 test");

		int64_t  d64, i64, o64;
		uint64_t u64, x64;
		sscanf("9223372036854775807 -9223372036854775808 777777777777777777777 18446744073709551615 ffffffffffffffff", "%"SCNd64 " %"SCNi64 " %"SCNo64 " %"SCNu64 " %"SCNx64, &d64, &i64, &o64, &u64, &x64);
		if((d64 != INT64_MAX) || (i64 != INT64_MIN) || (o64 != INT64_MAX) || (u64 != UINT64_MAX) || (x64 != UINT64_MAX))
			DoError(nErrorCount, "SCN64 test");

		// Many compilers give warnings for the following code because they 
		// recognize that a pointer is being formatted as an integer.  
		// This is what we want to do and what the C99 standard intends here.
		void *dPtr, *iPtr, *oPtr, *uPtr, *xPtr;

		#if (EA_PLATFORM_PTR_SIZE == 4)
			sscanf("2147483647 -2147483648 17777777777 4294967295 ffffffff", "%"SCNdPTR " %"SCNiPTR " %"SCNoPTR " %"SCNuPTR " %"SCNxPTR, (intptr_t*)&dPtr, (intptr_t*)&iPtr, (uintptr_t*)&oPtr, (uintptr_t*)&uPtr, (uintptr_t*)&xPtr);
		#else // EA_PLATFORM_PTR_SIZE == 8
			sscanf("2147483647 -2147483648 17777777777 18446744073709551615 ffffffffffffffff", "%"SCNdPTR " %"SCNiPTR " %"SCNoPTR " %"SCNuPTR " %"SCNxPTR, (intptr_t*)&dPtr,(intptr_t*)&iPtr, (uintptr_t*)&oPtr, (uintptr_t*)&uPtr, (uintptr_t*)&xPtr);
		#endif

		if((dPtr != (void*)INT32_MAX) || (iPtr != (void*)INT32_MIN) || (oPtr != (void*)INT32_MAX) || (uPtr != (void*)(uintptr_t)UINT64_MAX) || (xPtr != (void*)(uintptr_t)UINT64_MAX))
			DoError(nErrorCount, "SCNPTR test");
	}


	// Test min/max
	{
		// The C standard allows INT8_MIN to be either -127 or -128. So in order to be able
		// to test for this in a portable way, we do the logic below whereby we test for 
		// -127 (which all compiles should support) or -127 - 1 which all compilers should
		// support if INT8_MIN isn't -127.
		if(!Verify(INT8_MIN == INT8_C(-127)) && !Verify(INT8_MIN == INT8_C(-127) - 1))
			DoError(nErrorCount, "INT8_MIN test");
		if(!Verify(INT8_MAX == INT8_C(127)))
			DoError(nErrorCount, "INT8_MAX test");
		if(!Verify(UINT8_MAX == UINT8_C(255)))
			DoError(nErrorCount, "UINT8_MAX test");

		if(!Verify(INT16_MIN == INT16_C(-32767)) && !Verify(INT16_MIN == INT16_C(-32767) - 1))
			DoError(nErrorCount, "INT16_MIN test");
		if(!Verify(INT16_MAX == INT16_C(32767)))
			DoError(nErrorCount, "INT16_MAX test");
		if(!Verify(UINT16_MAX == UINT16_C(65535)))
			DoError(nErrorCount, "UINT16_MAX test");

		if(!Verify(INT32_MIN == INT32_C(-2147483647)) && !Verify(INT32_MIN == INT32_C(-2147483647) - 1))
			DoError(nErrorCount, "INT32_MIN test");
		if(!Verify(INT32_MAX == INT32_C(2147483647)))
			DoError(nErrorCount, "INT32_MAX test");
		if(!Verify(UINT32_MAX == UINT32_C(4294967295)))
			DoError(nErrorCount, "UINT32_MAX test");

		if(!Verify(INT64_MIN == INT64_C(-9223372036854775807)) && !Verify(INT64_MIN == INT64_C(-9223372036854775807) - 1))
			DoError(nErrorCount, "INT64_MIN test");
		if(!Verify(INT64_MAX == INT64_C(9223372036854775807)))
			DoError(nErrorCount, "INT64_MAX test");

		#if defined(__GNUC__) && (__GNUC__ < 4) // If using a broken version of UINT64_C/INT64_C macros...
			const uint64_t temp = 18446744073709551615ULL;
		#else
			const uint64_t temp = UINT64_C(18446744073709551615);
		#endif

		if(!Verify(UINT64_MAX == temp))
			DoError(nErrorCount, "UINT64_MAX test");
	}

	#endif

	return nErrorCount;
}




int TestEAPlatform(void)
{
	int nErrorCount = 0;

	// Test EA_PLATFORM_PTR_SIZE
	{
		#ifdef EA_PLATFORM_PTR_SIZE
			Verify(EA_PLATFORM_PTR_SIZE == sizeof(void*), "EA_PLATFORM_PTR_SIZE test");
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
			Verify(pValue[0] == 0x78, "EA_SYSTEM_ENDIAN test");
		#elif defined(EA_SYSTEM_BIG_ENDIAN)
			Verify(pValue[0] == 0x12, "EA_SYSTEM_ENDIAN test");
		#else
			Verify(0, "EA_SYSTEM_ENDIAN test");
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
	// Todo
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

// Not applicable to C:
//#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
//    #include <vector>
//#endif

#ifndef EA_COMPILER_NO_COVARIANT_RETURN_TYPE
	// Todo
#endif

#ifndef EA_COMPILER_NO_COVARIANT_RETURN_TYPE
	// Todo
#endif

#ifndef EA_COMPILER_NO_VARIADIC_MACROS
	#define MY_PRINTF(format, ...) Printf(format, __VA_ARGS__)
#endif




int TestEACompiler(void)
{
	int nErrorCount = 0;

	#if 0

	// As of this writing, eacompiler.h defines at least the following compilers:
	// EA_COMPILER_GNUC
	// EA_COMPILER_BORLANDC
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
	#ifndef EA_COMPILER_NO_EXCEPTION_STD_NAMESPACE
		TestNESN();
	#endif


	// Test EA_COMPILER_NO_EXCEPTIONS
	#ifndef EA_COMPILER_NO_EXCEPTIONS
		if(!TestNE())
			DoError(nErrorCount, "EA_COMPILER_NO_EXCEPTIONS test");
	#endif


	// Test EA_COMPILER_NO_UNWIND
	if(!TestNU())
		DoError(nErrorCount, "EA_COMPILER_NO_UNWIND test");

	#endif

	return nErrorCount;
}


int TestEACompilerTraits(void)
{
	int nErrorCount = 0;

	// EA_COMPILER_IS_ANSIC
	// EA_COMPILER_IS_C99
	// EA_COMPILER_IS_CPLUSPLUS
	// EA_COMPILER_MANAGED_CPP

	{ // Test EA_ALIGN_OF
		typedef void (*AlignTestFunctionType)(void);
		if(EA_ALIGN_OF(AlignTestFunctionType) != sizeof(void*)) // This may not be a kosher test.
			DoError(nErrorCount, "EA_ALIGN_OF test (AlignTestFunctionType)");

		if(EA_ALIGN_OF(int8_t) != sizeof(int8_t))   // This may not be a kosher test.
			DoError(nErrorCount, "EA_ALIGN_OF test (int16_t)");

		if(EA_ALIGN_OF(int16_t) != sizeof(int16_t)) // This may not be a kosher test.
			DoError(nErrorCount, "EA_ALIGN_OF test (int16_t)");

		if(EA_ALIGN_OF(int32_t) != sizeof(int32_t)) // This may not be a kosher test.
			DoError(nErrorCount, "EA_ALIGN_OF test (int32_t)");

		#if !defined (EA_ABI_ARM_APPLE)
		if(EA_ALIGN_OF(int64_t) != sizeof(int64_t)) // This may not be a kosher test.
			DoError(nErrorCount, "EA_ALIGN_OF test (int64_t)");
		#endif
	}

	{ // Test EA_PREFIX_ALIGN
		#ifdef EA_PREFIX_ALIGN
			char buffer[32];
			EA_PREFIX_ALIGN(64) int x = 0;
			sprintf(buffer, "%d", x);
			if(buffer[0] != '0')
				DoError(nErrorCount, "EA_PREFIX_ALIGN test");

			#if defined(__cplusplus)
				EA_PREFIX_ALIGN(64) ClassWithDefaultCtor    cdcA;
			  //EA_PREFIX_ALIGN(64) ClassWithoutDefaultCtor cwdcA;

				EA_PREFIX_ALIGN(64) ClassWithDefaultCtor    cdcB(3);
				EA_PREFIX_ALIGN(64) ClassWithoutDefaultCtor cwdcB(3);
			#endif

		#else
			DoError(nErrorCount, "EA_PREFIX_ALIGN test");
		#endif
	}


	{ // Test EA_POSTFIX_ALIGN
		#ifdef EA_POSTFIX_ALIGN
			char buffer[32];
			int x EA_POSTFIX_ALIGN(ALIGNMENT_AMOUNT_64) = 0;
			sprintf(buffer, "%d", x);
			if(buffer[0] != '0')
				DoError(nErrorCount, "EA_POSTFIX_ALIGN test");
		#else
			DoError(nErrorCount, "EA_POSTFIX_ALIGN test");
		#endif
	}


	{ // Test EA_ALIGNED
		#ifdef EA_ALIGNED
			char buffer[32];

			// Verify that a simple declaration works.
			EA_ALIGNED(int, xA, ALIGNMENT_AMOUNT_64); 

			// Verify that a declaration with assignment works.
			EA_ALIGNED(int, xB, ALIGNMENT_AMOUNT_64) = 0;

			// Verify that a typedefd declaration works.
			typedef EA_ALIGNED(int, int16, ALIGNMENT_AMOUNT_16);
			int16 n16 = 0;

			// Verify that a declaration with construction works.
			#if defined(__cplusplus)
				EA_ALIGNED(int, xC, ALIGNMENT_AMOUNT_64)(0);
			#endif

			xA = 0;
			sprintf(buffer, "%d", xA);
			sprintf(buffer, "%d", xB);
			sprintf(buffer, "%p", &n16);
			#if defined(__cplusplus)
				sprintf(buffer, "%d", xC);
			#endif

			#if defined(__cplusplus)
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
			#endif
		#else
			DoError(nErrorCount, "EA_ALIGNED test");
		#endif
	}


	{ // Test EA_PACKED
		#if defined(__cplusplus)
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
		#endif
	}


	{ // Test EA_LIKELY

		if(EA_UNLIKELY(nErrorCount > 0))
		{
			if(EA_LIKELY(nErrorCount == 999999)) // Actually this isn't likely, but that's beside the point.
				DoError(nErrorCount, "EA_LIKELY test");
		}
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
		char* p = NULL;
		if(RestrictTest(p) == 0) // This isn't a real test. If there is a failure, it will happen at compile time.
			DoError(nErrorCount, "EA_RESTRICT test");
	}


	{ // Test EA_DEPRECATED
		/* This causes warnings on compilers, so just disable it.
		#if defined(EA_DEPRECATED) && (!defined(__GNUC__) || ((__GNUC__ * 100 + __GNUC_MINOR__) < 402)) // GCC 4.2+ is converting deprecated into an error instead of a warning.
			char buffer[32];
			EA_DEPRECATED int x = 0;
			sprintf(buffer, "%d", x); (void)x;
			if(buffer[0] != '0')
				DoError(nErrorCount, "EA_DEPRECATED test");
		#else
			DoError(nErrorCount, "EA_DEPRECATED test");
		#endif
		*/
	}


	{ // Test EA_PASCAL
		#if defined(__cplusplus)
			#ifdef EA_PASCAL
				struct X{ void EA_PASCAL DoNothing(void){} };
				X x;
				x.DoNothing();
			#else
				DoError(nErrorCount, "EA_PASCAL test");
			#endif
		#endif
	}


	{ // Test EA_PASCAL_FUNC
		#if defined(__cplusplus)
			#ifdef EA_PASCAL_FUNC
				struct X{ void EA_PASCAL_FUNC(DoNothing()){} };
				X x;
				x.DoNothing();
			#else
				DoError(nErrorCount, "EA_PASCAL_FUNC test");
			#endif
		#endif
	}


	// EA_SSE
	// Not sure how to properly test at this time.


	{ // EA_IMPORT
		// Not possible to do this because import means it will come from outside.
		//struct X{ EA_IMPORT void DoNothing(){} };
		//X x;
		//x.DoNothing();
	}


	{ // EA_EXPORT
		#if defined(__cplusplus)
			struct X{ EA_EXPORT void DoNothing(){} };
			X x;
			x.DoNothing();
		#endif
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

		if((arrayCount != 13) || (testArray[0] != 0))
			DoError(nErrorCount, "EAArrayCount test");
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

	return nErrorCount;
}


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


int EAMain(int argc, char** argv)
{
	int nErrorCount = 0, nTotalErrorCount = 0;

	(void)argc;
	(void)argv;

	nErrorCount = TestEABase();
	Printf("EABase test error count: %d\n\n", nErrorCount);
	nTotalErrorCount += nErrorCount;

	nErrorCount = TestEAPlatform();
	Printf("EAPlatform test error count: %d\n\n", nErrorCount);
	nTotalErrorCount += nErrorCount;

	nErrorCount = TestEACompiler();
	Printf("EACompiler test error count: %d\n\n", nErrorCount);
	nTotalErrorCount += nErrorCount;

	nErrorCount = TestEACompilerTraits();
	Printf("EACompilerTraits test error count: %d\n\n", nErrorCount);
	nTotalErrorCount += nErrorCount;

	if (nTotalErrorCount == 0)
		Printf("\nAll tests completed successfully.\n");
	else
		Printf("\nTests failed. Total error count: %d\n", nTotalErrorCount);
 
	return nTotalErrorCount;
}
