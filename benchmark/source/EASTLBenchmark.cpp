/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLBenchmark.h"
#include "EASTLTest.h"
#include <EASTL/string.h>
#include <EAMain/EAMain.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
#include <stdio.h>
#include <math.h>
#include <float.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif



namespace Benchmark
{
	static int64_t ConvertStopwatchUnits(EA::StdC::Stopwatch::Units unitsSource, int64_t valueSource, EA::StdC::Stopwatch::Units unitsDest)
	{
		using namespace EA::StdC;

		int64_t valueDest = valueSource;

		if(unitsSource != unitsDest)
		{
			double sourceMultiplier;

			switch (unitsSource)
			{
				case Stopwatch::kUnitsCPUCycles:
					sourceMultiplier = Stopwatch::GetUnitsPerCPUCycle(unitsDest); // This will typically be a number less than 1.
					valueDest = (int64_t)(valueSource * sourceMultiplier);
					break;

				case Stopwatch::kUnitsCycles:
					sourceMultiplier = Stopwatch::GetUnitsPerStopwatchCycle(unitsDest); // This will typically be a number less than 1.
					valueDest = (int64_t)(valueSource * sourceMultiplier);
					break;

				case Stopwatch::kUnitsNanoseconds:
				case Stopwatch::kUnitsMicroseconds:
				case Stopwatch::kUnitsMilliseconds:
				case Stopwatch::kUnitsSeconds:
				case Stopwatch::kUnitsMinutes:
				case Stopwatch::kUnitsUserDefined:
					// To do. Also, handle the case of unitsDest being Cycles or CPUCycles and unitsSource being a time.
					break;
			}
		}

		return valueDest;
	}

	void WriteTime(int64_t timeNS, eastl::string& sTime)
	{
		if(timeNS > 1000000000)
			sTime.sprintf(" %6.2f s",  (double)timeNS / 1000000000);
		else if(timeNS > 1000000)
			sTime.sprintf("%6.1f ms", (double)timeNS / 1000000);
		else if(timeNS > 1000)
			sTime.sprintf("%6.1f us", (double)timeNS / 1000);
		else
			sTime.sprintf("%6.1f ns", (double)timeNS / 1);
	}



	Environment gEnvironment;

	Environment& GetEnvironment()
	{
		return gEnvironment;
	}



	ResultSet gResultSet;

	ResultSet& GetResultSet()
	{
		return gResultSet;
	}



	// Scratch sprintf buffer
	char gScratchBuffer[1024];


	void DoNothing(...)
	{
		// Intentionally nothing.
	}


	void AddResult(const char* pName, int units, int64_t nTime1, int64_t nTime2, const char* pNotes)
	{
		Result result;

		result.msName   = pName;
		result.mUnits   = units;
		result.mTime1   = nTime1;
		result.mTime1NS = ConvertStopwatchUnits((EA::StdC::Stopwatch::Units)units, nTime1, EA::StdC::Stopwatch::kUnitsNanoseconds);
		result.mTime2   = nTime2;
		result.mTime2NS = ConvertStopwatchUnits((EA::StdC::Stopwatch::Units)units, nTime2, EA::StdC::Stopwatch::kUnitsNanoseconds);

		if(pNotes)
			result.msNotes = pNotes;

		gResultSet.insert(result);
	}


	void PrintResultLine(const Result& result)
	{
		const double fRatio         = (double)result.mTime1 / (double)result.mTime2;
		const double fRatioPrinted  = (fRatio > 100) ? 100 : fRatio;
		const double fPercentChange = fabs(((double)result.mTime1 - (double)result.mTime2) / (((double)result.mTime1 + (double)result.mTime2) / 2));
		const bool   bDifference    = (result.mTime1 > 10) && (result.mTime2 > 10) && (fPercentChange > 0.25);
		const char*  pDifference    = (bDifference ? (result.mTime1 < result.mTime2 ? "-" : "+") : "");

		eastl::string sClockTime1, sClockTime2;

		WriteTime(result.mTime1NS, sClockTime1);  // This converts an integer in nanoseconds (e.g. 23400000) to a string (e.g. "23.4 ms")
		WriteTime(result.mTime2NS, sClockTime2);

		EA::UnitTest::Report("%-43s | %13" PRIu64 " %s | %13" PRIu64 " %s | %10.2f%10s", result.msName.c_str(), result.mTime1, sClockTime1.c_str(), result.mTime2, sClockTime2.c_str(), fRatioPrinted, pDifference);

		if(result.msNotes.length()) // If there are any notes...
			EA::UnitTest::Report("   %s", result.msNotes.c_str());
		EA::UnitTest::Report("\n");
	}


	#if defined(EASTL_BENCHMARK_WRITE_FILE) && EASTL_BENCHMARK_WRITE_FILE

		#if !defined(EASTL_BENCHMARK_WRITE_FILE_PATH)
			#define EASTL_BENCHMARK_WRITE_FILE_PATH "BenchmarkResults.txt"
		#endif

		struct FileWriter
		{
			FILE* mpReportFile;
			EA::EAMain::ReportFunction mpSavedReportFunction;
			static FileWriter* gpFileWriter;

			static void StaticPrintfReportFunction(const char8_t* pText)
			{
				if(gpFileWriter)
					gpFileWriter->PrintfReportFunction(pText);
			}

			void PrintfReportFunction(const char8_t* pText)
			{
				fwrite(pText, strlen(pText), 1, mpReportFile);
				EA::EAMain::ReportFunction gpReportFunction = EA::EAMain::GetDefaultReportFunction();
				gpReportFunction(pText);
			}

			FileWriter() : mpReportFile(NULL), mpSavedReportFunction(NULL)
			{
				mpReportFile = fopen(EASTL_BENCHMARK_WRITE_FILE_PATH, "w+");

				if(mpReportFile)
				{
					gpFileWriter = this;
					mpSavedReportFunction = EA::EAMain::GetDefaultReportFunction();
					EA::EAMain::SetReportFunction(StaticPrintfReportFunction);
				}
			}

		   ~FileWriter()
			{
				if(mpReportFile)
				{
					gpFileWriter = NULL;
					EA::EAMain::SetReportFunction(mpSavedReportFunction);
					fclose(mpReportFile);
				}
			}
		};

		FileWriter* FileWriter::gpFileWriter = NULL;
	#endif


	void PrintResults()
	{
		#if defined(EASTL_BENCHMARK_WRITE_FILE) && EASTL_BENCHMARK_WRITE_FILE
			FileWriter fileWriter; // This will auto-execute.
		#endif

		// Print the results
		EA::UnitTest::Report("\n");
		EA::UnitTest::Report("****************************************************************************************\n");
		EA::UnitTest::Report("EASTL Benchmark test results\n");
		EA::UnitTest::Report("****************************************************************************************\n");
		EA::UnitTest::Report("\n");
		EA::UnitTest::Report("EASTL version: %s\n", EASTL_VERSION);
		EA::UnitTest::Report("Platform: %s\n", gEnvironment.msPlatform.c_str());
		EA::UnitTest::Report("Compiler: %s\n", EA_COMPILER_STRING);
		#if defined(EA_DEBUG) || defined(_DEBUG)
		EA::UnitTest::Report("Allocator: PPMalloc::GeneralAllocatorDebug. Thread safety enabled.\n");
		EA::UnitTest::Report("Build: Debug. Inlining disabled. STL debug features disabled.\n");
		#else
		EA::UnitTest::Report("Allocator: PPMalloc::GeneralAllocator. Thread safety enabled.\n");
		EA::UnitTest::Report("Build: Full optimization. Inlining enabled.\n");
		#endif
		EA::UnitTest::Report("\n");
		EA::UnitTest::Report("Values are ticks and time to complete tests; smaller values are better.\n");
		EA::UnitTest::Report("\n");
		EA::UnitTest::Report("%-43s%26s%26s%13s%13s\n", "Test", gEnvironment.msSTLName1.c_str(), gEnvironment.msSTLName2.c_str(), "Ratio", "Difference?");
		EA::UnitTest::Report("---------------------------------------------------------------------------------------------------------------------\n");

		eastl::string sTestTypeLast;
		eastl::string sTestTypeTemp;

		for(ResultSet::iterator it = gResultSet.begin(); it != gResultSet.end(); ++it)
		{
			const Result& result = *it;

			eastl_size_t n = result.msName.find('/');
			if(n == eastl::string::npos)
				n = result.msName.length();
			sTestTypeTemp.assign(result.msName, 0, n);

			if(sTestTypeTemp != sTestTypeLast) // If it looks like we are changing to a new test type... add an empty line to help readability.
			{
				if(it != gResultSet.begin())
					EA::UnitTest::Report("\n");
				sTestTypeLast = sTestTypeTemp;
			}

			PrintResultLine(result);
		}

		// We will print out a final line that has the sum of the rows printed above.
		Result resultSum;
		resultSum.msName = "sum";

		for(ResultSet::iterator its = gResultSet.begin(); its != gResultSet.end(); ++its)
		{
			const Result& resultTemp = *its;

			EASTL_ASSERT(resultTemp.mUnits == EA::StdC::Stopwatch::kUnitsCPUCycles); // Our ConvertStopwatchUnits call below assumes that every measured time is CPUCycles.
			resultSum.mTime1 += resultTemp.mTime1;
			resultSum.mTime2 += resultTemp.mTime2;
		}

		// We do this convert as a final step instead of the loop in order to avoid loss of precision.
		resultSum.mTime1NS = ConvertStopwatchUnits(EA::StdC::Stopwatch::kUnitsCPUCycles, resultSum.mTime1, EA::StdC::Stopwatch::kUnitsNanoseconds);
		resultSum.mTime2NS = ConvertStopwatchUnits(EA::StdC::Stopwatch::kUnitsCPUCycles, resultSum.mTime2, EA::StdC::Stopwatch::kUnitsNanoseconds);
		EA::UnitTest::Report("\n");
		PrintResultLine(resultSum);

		EA::UnitTest::Report("\n");
		EA::UnitTest::Report("****************************************************************************************\n");
		EA::UnitTest::Report("\n");

		// Clear the results
		gResultSet.clear();
		gEnvironment.clear();
	}

} // namespace Benchmark



















