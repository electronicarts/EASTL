///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include "EASTLBenchmark.h"
#include "EASTLTest.h"
#if !EASTL_OPENSOURCE
    #include <PPMalloc/EAGeneralAllocatorDebug.h>
#endif
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAStopwatch.h>
#include <EAStdC/EAString.h>
#include <EASTL/internal/config.h>
#include <string.h>
#include <stdio.h>
EA_DISABLE_VC_WARNING(4946)
#include "EAMain/EAEntryPointMain.inl"
#include "EASTLTestAllocator.h"


///////////////////////////////////////////////////////////////////////////////
// gpEAGeneralAllocator / gpEAGeneralAllocatorDebug
//
#if !EASTL_OPENSOURCE
namespace EA
{
	namespace Allocator
	{
		#ifdef EA_DEBUG
			extern         GeneralAllocatorDebug  gGeneralAllocator;
			extern PPM_API GeneralAllocatorDebug* gpEAGeneralAllocatorDebug;
		#else
			extern         GeneralAllocator       gGeneralAllocator;
			extern PPM_API GeneralAllocator*      gpEAGeneralAllocator;
		#endif
	}
}
#endif


///////////////////////////////////////////////////////////////////////////////
// Required by EASTL.
//
#if !defined(EASTL_EASTDC_VSNPRINTF) || !EASTL_EASTDC_VSNPRINTF
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
#endif


///////////////////////////////////////////////////////////////////////////////
// main
//
int EAMain(int argc, char* argv[])
{
	bool bWaitAtEnd   = false;
	bool bPrintHelp   = false;
	int  nOptionCount = 0;
	int  nErrorCount  = 0;

	EA::EAMain::PlatformStartup();
	EA::EAMain::SetVerbosity(2);    // Default value.

	// Set up debug parameters.
	#ifdef EA_DEBUG
	  // Only enable this temporarily to help find any problems you might find.
	  // EA::Allocator::gpEAGeneralAllocatorDebug->SetAutoHeapValidation(EA::Allocator::GeneralAllocator::kHeapValidationLevelBasic, 16);
	#endif

	// Parse command line arguments
	for(int i = 1; i < argc; i++)
	{
		if(strstr(argv[i], "-w") == argv[i])
		{
			bWaitAtEnd = true;
			nOptionCount++;
		}
		else if(strstr(argv[i], "-v") == argv[i])
		{
			uint32_t verbosity = EA::StdC::AtoU32(argv[i] + 3);
			EA::EAMain::SetVerbosity(verbosity);
			nOptionCount++;
		}
		else if(strstr(argv[i], "-l:") == argv[i])
		{
			gEASTL_TestLevel = atoi(argv[i] + 3);
			if(gEASTL_TestLevel < kEASTL_TestLevelLow)
				gEASTL_TestLevel = kEASTL_TestLevelLow;
			else if(gEASTL_TestLevel > kEASTL_TestLevelHigh)
				gEASTL_TestLevel = kEASTL_TestLevelHigh;
			nOptionCount++;
		}
		else if(strstr(argv[i], "-s:") == argv[i])
		{
			uint32_t seed = (eastl_size_t)atoi(argv[i] + 3);
			EA::UnitTest::SetRandSeed(seed);
			nOptionCount++;
		}
		else if((strstr(argv[i], "-?") == argv[i]) || (strstr(argv[i], "-h") == argv[i]))
		{
			bPrintHelp = true;
			nOptionCount++;
		}
	}

	// Print user help.
	if(!bPrintHelp)
		bPrintHelp = (nOptionCount == 0);

	if(bPrintHelp)
	{
		EASTLTest_Printf("Options\n");
		EASTLTest_Printf("   -w     Wait at end.\n");
		EASTLTest_Printf("   -l:N   Test level in range of [1, 10]. 10 means maximum testing.\n");
		EASTLTest_Printf("   -s:N   Specify a randomization seed. 0 is default and means use clock.\n");
		EASTLTest_Printf("   -?     Show help.\n");
	}


	// Set up test information
	Benchmark::Environment& environment = Benchmark::GetEnvironment();
	environment.msPlatform = EA_PLATFORM_DESCRIPTION;
	environment.msSTLName1 = GetStdSTLName();
	environment.msSTLName2 = "EASTL";


	// Run tests
	#ifndef EA_DEBUG
		EA::UnitTest::SetHighThreadPriority();
	#endif

	EA::StdC::Stopwatch stopwatch(EA::StdC::Stopwatch::kUnitsSeconds, true);     // Measure seconds, start the counting immediately.

	BenchmarkAlgorithm();
	BenchmarkList();
	BenchmarkString();
	BenchmarkVector();
	BenchmarkDeque();
	BenchmarkSet();
	BenchmarkMap();
	BenchmarkHash();
	BenchmarkHeap();
	BenchmarkBitset();
	BenchmarkSort();
	BenchmarkTupleVector();

	stopwatch.Stop();

	#ifndef EA_DEBUG
		EA::UnitTest::SetNormalThreadPriority();
	#endif

	Benchmark::PrintResults();

	eastl::string sClockTime;
	Benchmark::WriteTime(stopwatch.GetElapsedTime(), sClockTime);

	EASTLTest_Printf("Time to complete all tests: %s.\n", sClockTime.c_str());

	// Done
	if(bWaitAtEnd)
	{
		EASTLTest_Printf("\nPress any key to exit.\n");
		getchar(); // Wait for the user and shutdown
	}

	EA::EAMain::PlatformShutdown(nErrorCount);

	return 0;
}










