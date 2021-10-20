///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EAStdC/EASprintf.h>
#include <EASTL/internal/config.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
EA_RESTORE_ALL_VC_WARNINGS()


#include "EAMain/EAEntryPointMain.inl"
#include "EASTLTestAllocator.h"

///////////////////////////////////////////////////////////////////////////////
// Required by EASTL.
//
#if !EASTL_EASTDC_VSNPRINTF
	int Vsnprintf8(char* pDestination, size_t n, const char*  pFormat, va_list arguments)
	{
		return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
	}

	int Vsnprintf16(char16_t* pDestination, size_t n, const char16_t* pFormat, va_list arguments)
	{
		return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
	}

	int Vsnprintf32(char32_t* pDestination, size_t n, const char32_t* pFormat, va_list arguments)
	{
		return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
	}

	#if defined(EA_CHAR8_UNIQUE) && EA_CHAR8_UNIQUE
		int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t*  pFormat, va_list arguments)
		{
			return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
		}
	#endif

	#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
		int VsnprintfW(wchar_t* pDestination, size_t n, const wchar_t* pFormat, va_list arguments)
		{
			return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
		}
	#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EAMain
//
int EAMain(int argc, char* argv[])
{
	using namespace EA::UnitTest;

	int nErrorCount = 0;

	EA::EAMain::PlatformStartup();

	EASTLTest_SetGeneralAllocator();

	nErrorCount += EASTLTest_CheckMemory();

	// Parse command line arguments
	for(int i = 1; i < argc; i++)
	{
		// Example usage: -l:7
		if(strstr(argv[i], "-l:") == argv[i])
		{
			gEASTL_TestLevel = atoi(argv[i] + 3);

			if(gEASTL_TestLevel < kEASTL_TestLevelLow)
			   gEASTL_TestLevel = kEASTL_TestLevelLow;
			if(gEASTL_TestLevel > kEASTL_TestLevelHigh)
			   gEASTL_TestLevel = kEASTL_TestLevelHigh;
		}
	}

	TestApplication testSuite("EASTL Unit Tests", argc, argv);

	testSuite.AddTest("Algorithm",				TestAlgorithm);
	testSuite.AddTest("Allocator",				TestAllocator);
	testSuite.AddTest("Any",				    TestAny);
	testSuite.AddTest("Array",					TestArray);
	testSuite.AddTest("BitVector",				TestBitVector);
	testSuite.AddTest("Bitset",					TestBitset);
	testSuite.AddTest("CharTraits",			    TestCharTraits);
	testSuite.AddTest("Chrono",					TestChrono);
	testSuite.AddTest("Deque",					TestDeque);
	testSuite.AddTest("Extra",					TestExtra);
	testSuite.AddTest("Finally",				TestFinally);
	testSuite.AddTest("FixedFunction",			TestFixedFunction);
	testSuite.AddTest("FixedHash",				TestFixedHash);
	testSuite.AddTest("FixedList",				TestFixedList);
	testSuite.AddTest("FixedMap",				TestFixedMap);
	testSuite.AddTest("FixedSList",				TestFixedSList);
	testSuite.AddTest("FixedSet",				TestFixedSet);
	testSuite.AddTest("FixedString",			TestFixedString);
	testSuite.AddTest("FixedTupleVector",		TestFixedTupleVector);
	testSuite.AddTest("FixedVector",			TestFixedVector);
	testSuite.AddTest("Functional",				TestFunctional);
	testSuite.AddTest("Hash",					TestHash);
	testSuite.AddTest("Heap",					TestHeap);
	testSuite.AddTest("IntrusiveHash",			TestIntrusiveHash);
	testSuite.AddTest("IntrusiveList",			TestIntrusiveList);
	testSuite.AddTest("IntrusiveSDList",		TestIntrusiveSDList);
	testSuite.AddTest("IntrusiveSList",			TestIntrusiveSList);
	testSuite.AddTest("Iterator",				TestIterator);
	testSuite.AddTest("LRUCache",				TestLruCache);
	testSuite.AddTest("List",					TestList);
	testSuite.AddTest("ListMap",				TestListMap);
	testSuite.AddTest("Map",					TestMap);
	testSuite.AddTest("Memory",					TestMemory);
	testSuite.AddTest("Meta",				    TestMeta);
	testSuite.AddTest("NumericLimits",			TestNumericLimits);
	testSuite.AddTest("Optional",				TestOptional);
	testSuite.AddTest("Random",					TestRandom);
	testSuite.AddTest("Ratio",					TestRatio);
	testSuite.AddTest("RingBuffer",				TestRingBuffer);
	testSuite.AddTest("SList",					TestSList);
	testSuite.AddTest("SegmentedVector",		TestSegmentedVector);
	testSuite.AddTest("Set",					TestSet);
	testSuite.AddTest("SmartPtr",				TestSmartPtr);
	testSuite.AddTest("Sort",					TestSort);
	testSuite.AddTest("Span",				    TestSpan);
	testSuite.AddTest("String",					TestString);
	testSuite.AddTest("StringHashMap",			TestStringHashMap);
	testSuite.AddTest("StringMap",				TestStringMap);
	testSuite.AddTest("StringView",			    TestStringView);
	testSuite.AddTest("TestCppCXTypeTraits",	TestCppCXTypeTraits);
	testSuite.AddTest("Tuple",					TestTuple);
	testSuite.AddTest("TupleVector",			TestTupleVector);
	testSuite.AddTest("TypeTraits",				TestTypeTraits);
	testSuite.AddTest("Utility",				TestUtility);
	testSuite.AddTest("Variant",				TestVariant);
	testSuite.AddTest("Vector",					TestVector);
	testSuite.AddTest("VectorMap",				TestVectorMap);
	testSuite.AddTest("VectorSet",				TestVectorSet);
	testSuite.AddTest("AtomicBasic",			TestAtomicBasic);
	testSuite.AddTest("AtomicAsm",			    TestAtomicAsm);
	testSuite.AddTest("TestBitcast",			TestBitcast);


	nErrorCount += testSuite.Run();

	nErrorCount += EASTLTest_CheckMemory();

	EA::EAMain::PlatformShutdown(nErrorCount);

	return nErrorCount;
}








