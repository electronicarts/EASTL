/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTLBENCHMARK_H
#define EASTLBENCHMARK_H


// Intrinsic control
//
// Our benchmark results are being skewed by inconsistent decisions by the 
// VC++ compiler to use intrinsic functions. Additionally, many of our 
// benchmarks work on large blocks of elements, whereas intrinsics often
// are an improvement only over small blocks of elements. As a result, 
// enabling of intrinsics is often resulting in poor benchmark results for
// code that gets an intrinsic enabled for it, even though it will often
// happen in real code to be the opposite case. The disabling of intrinsics
// here often results in EASTL performance being lower than it would be in
// real-world situations.
//
#include <string.h>
#ifdef _MSC_VER
	#pragma function(strlen, strcmp, strcpy, strcat, memcpy, memcmp, memset)
#endif


#include <EASTL/set.h>
#include <EASTL/string.h>
#include <EAStdC/EAStopwatch.h>
#include <stdlib.h>
#include <string.h>


void BenchmarkSort();
void BenchmarkList();
void BenchmarkString();
void BenchmarkVector();
void BenchmarkDeque();
void BenchmarkSet();
void BenchmarkMap();
void BenchmarkHash();
void BenchmarkAlgorithm();
void BenchmarkHeap();
void BenchmarkBitset();
void BenchmarkTupleVector();


namespace Benchmark
{

	// Environment
	//
	// The environment for this benchmark test.
	//
	struct Environment
	{
		eastl::string8 msPlatform;       // Name of test platform (e.g. "Windows")
		eastl::string8 msSTLName1;       // Name of competitor #1 (e.g. "EASTL").
		eastl::string8 msSTLName2;       // Name of competitor #2 (e.g. "MS STL").

		void clear() { msPlatform.set_capacity(0); msSTLName1.set_capacity(0); msSTLName2.set_capacity(0); }
	};

	Environment& GetEnvironment();


	// Result
	//
	// An individual benchmark result.
	//
	struct Result
	{
		eastl::string8       msName;    // Test name (e.g. "vector/insert").
		int                  mUnits;    // Timing units (e.g. EA::StdC::Stopwatch::kUnitsSeconds).
		int64_t              mTime1;    // Time of competitor #1.
		uint64_t             mTime1NS;  // Nanoseconds.
		int64_t              mTime2;    // Time of competitor #2.
		int64_t              mTime2NS;  // Nanoseconds.
		eastl::string8       msNotes;   // Any comments to attach to this result.

		Result() : msName(), mUnits(EA::StdC::Stopwatch::kUnitsCPUCycles), 
					mTime1(0), mTime1NS(0), mTime2(0), mTime2NS(0), msNotes() { }
	};

	inline bool operator<(const Result& r1, const Result& r2)
		{ return r1.msName < r2.msName; } 

	typedef eastl::set<Result> ResultSet;

	ResultSet& GetResultSet();


	// Scratch sprintf buffer
	extern char gScratchBuffer[1024];



	// Utility functions
	//
	void DoNothing(...);
	void AddResult(const char* pName, int units, int64_t nTime1, int64_t nTime2, const char* pNotes = NULL);
	void PrintResults();
	void WriteTime(int64_t timeNS, eastl::string& sTime);
	

} // namespace Benchmark




///////////////////////////////////////////////////////////////////////////////
/// LargePOD
///
/// Implements a structure which is essentially a largish POD. Useful for testing
/// containers and algorithms for their ability to efficiently work with PODs.
/// This class isn't strictly a POD by the definition of the C++ standard, 
/// but it suffices for our interests.
///
struct LargeObject
{
	int32_t mData[2048];
};

struct LargePOD
{
	LargeObject mLargeObject1;
	LargeObject mLargeObject2;
	const char* mpName1;
	const char* mpName2;

	explicit LargePOD(int32_t x = 0) // A true POD doesn't have a non-trivial constructor.
	{
		memset(mLargeObject1.mData, 0, sizeof(mLargeObject1.mData));
		memset(mLargeObject2.mData, 0, sizeof(mLargeObject2.mData));
		mLargeObject1.mData[0] = x;

		mpName1 = "LargePOD1";
		mpName2 = "LargePOD2";
	}

	LargePOD(const LargePOD& largePOD) // A true POD doesn't have a non-trivial copy-constructor.
		: mLargeObject1(largePOD.mLargeObject1), 
		  mLargeObject2(largePOD.mLargeObject2), 
		  mpName1(largePOD.mpName1),
		  mpName2(largePOD.mpName2)
	{
	}

	virtual ~LargePOD() { }

	LargePOD& operator=(const LargePOD& largePOD) // A true POD doesn't have a non-trivial assignment operator.
	{
		if(&largePOD != this)
		{
			mLargeObject1 = largePOD.mLargeObject1; 
			mLargeObject2 = largePOD.mLargeObject2; 
			mpName1       = largePOD.mpName1;
			mpName2       = largePOD.mpName2;
		}
		return *this;
	}

	virtual void DoSomething() // Note that by declaring this virtual, this class is not truly a POD.
	{                          // But it acts like a POD for the purposes of EASTL algorithms.
		mLargeObject1.mData[1]++;
	}

	operator int()
	{
		return (int)mLargeObject1.mData[0];
	}
};

//EASTL_DECLARE_POD(LargePOD);
//EASTL_DECLARE_TRIVIAL_CONSTRUCTOR(LargePOD);
//EASTL_DECLARE_TRIVIAL_COPY(LargePOD);
//EASTL_DECLARE_TRIVIAL_ASSIGN(LargePOD);
//EASTL_DECLARE_TRIVIAL_DESTRUCTOR(LargePOD);
//EASTL_DECLARE_TRIVIAL_RELOCATE(LargePOD);

// Operators
// We specifically define only == and <, in order to verify that 
// our containers and algorithms are not mistakenly expecting other 
// operators for the contained and manipulated classes.
inline bool operator==(const LargePOD& t1, const LargePOD& t2)
{
	return (memcmp(&t1.mLargeObject1, &t2.mLargeObject1, sizeof(t1.mLargeObject1)) == 0) &&
		   (memcmp(&t1.mLargeObject2, &t2.mLargeObject2, sizeof(t1.mLargeObject2)) == 0) &&
		   (strcmp(t1.mpName1, t2.mpName1) == 0)  &&
		   (strcmp(t1.mpName2, t2.mpName2) == 0);
}

inline bool operator<(const LargePOD& t1, const LargePOD& t2)
{
	return (memcmp(&t1.mLargeObject1, &t2.mLargeObject1, sizeof(t1.mLargeObject1)) < 0) &&
		   (memcmp(&t1.mLargeObject2, &t2.mLargeObject2, sizeof(t1.mLargeObject2)) < 0) &&
		   (strcmp(t1.mpName1, t2.mpName1) < 0)  &&
		   (strcmp(t1.mpName2, t2.mpName2) < 0);
}





#endif // Header sentry






















