///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EAStdC/EASprintf.h>
#include <EASTL/internal/config.h>

#if defined(_MSC_VER)
	#pragma warning(push, 0)
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "EAMain/EAEntryPointMain.inl"
#include "EASTLTestAllocator.h"



///////////////////////////////////////////////////////////////////////////////
// Required by EASTL.
//
#if !EASTL_EASTDC_VSNPRINTF
	int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t*  pFormat, va_list arguments)
	{
		return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
	}

	int Vsnprintf16(char16_t* pDestination, size_t n, const char16_t* pFormat, va_list arguments)
	{
		return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
	}

	#if (EASTDC_VERSION_N >= 10600)
		int Vsnprintf32(char32_t* pDestination, size_t n, const char32_t* pFormat, va_list arguments)
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

	testSuite.AddTest("StringView",			    TestStringView);
	testSuite.AddTest("CharTraits",			    TestCharTraits);
	testSuite.AddTest("Any",				    TestAny);
	testSuite.AddTest("Optional",				TestOptional);
	testSuite.AddTest("TypeTraits",				TestTypeTraits);
	testSuite.AddTest("TestCppCXTypeTraits",	TestCppCXTypeTraits);
	testSuite.AddTest("Extra",					TestExtra);
	testSuite.AddTest("Functional",				TestFunctional);
	testSuite.AddTest("Utility",				TestUtility);
	testSuite.AddTest("Tuple",					TestTuple);
	testSuite.AddTest("Memory",					TestMemory);
	testSuite.AddTest("Allocator",				TestAllocator);
	testSuite.AddTest("Random",					TestRandom);
	testSuite.AddTest("NumericLimits",			TestNumericLimits);
	testSuite.AddTest("Iterator",				TestIterator);
	testSuite.AddTest("Bitset",					TestBitset);
	testSuite.AddTest("SmartPtr",				TestSmartPtr);
	testSuite.AddTest("List",					TestList);
	testSuite.AddTest("ListMap",				TestListMap);
	testSuite.AddTest("FixedList",				TestFixedList);
	testSuite.AddTest("SList",					TestSList);
	testSuite.AddTest("FixedSList",				TestFixedSList);
	testSuite.AddTest("IntrusiveList",			TestIntrusiveList);
	testSuite.AddTest("IntrusiveSList",			TestIntrusiveSList);
	testSuite.AddTest("String",					TestString);
	testSuite.AddTest("FixedString",			TestFixedString);
	testSuite.AddTest("Array",					TestArray);
	testSuite.AddTest("Vector",					TestVector);
	testSuite.AddTest("FixedVector",			TestFixedVector);
	testSuite.AddTest("SegmentedVector",		TestSegmentedVector);
	testSuite.AddTest("Deque",					TestDeque);
	testSuite.AddTest("Map",					TestMap);
	testSuite.AddTest("FixedMap",				TestFixedMap);
	testSuite.AddTest("StringMap",				TestStringMap);
	testSuite.AddTest("Set",					TestSet);
	testSuite.AddTest("FixedSet",				TestFixedSet);
	testSuite.AddTest("Hash",					TestHash);
	testSuite.AddTest("FixedHash",				TestFixedHash);
	testSuite.AddTest("FixedHash",				TestStringHashMap);
	testSuite.AddTest("IntrusiveHash",			TestIntrusiveHash);
	testSuite.AddTest("VectorMap",				TestVectorMap);
	testSuite.AddTest("VectorSet",				TestVectorSet);
	testSuite.AddTest("Algorithm",				TestAlgorithm);
	testSuite.AddTest("Sort",					TestSort);
	testSuite.AddTest("Heap",					TestHeap);
	testSuite.AddTest("RingBuffer",				TestRingBuffer);
	testSuite.AddTest("SparseMatrix",			TestSparseMatrix);
	testSuite.AddTest("IntrusiveSDList",		TestIntrusiveSDList);
	testSuite.AddTest("BitVector",				TestBitVector);
	testSuite.AddTest("Ratio",					TestRatio);
	testSuite.AddTest("Chrono",					TestChrono);

	nErrorCount += testSuite.Run();

	nErrorCount += EASTLTest_CheckMemory();

	EA::EAMain::PlatformShutdown(nErrorCount);

	return nErrorCount;
}








