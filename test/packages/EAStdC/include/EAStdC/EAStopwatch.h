///////////////////////////////////////////////////////////////////////////////
// EAStopwatch.h
//
// Copyright (c) Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTDC_EASTOPWATCH_H
#define EASTDC_EASTOPWATCH_H

#define EASTDC_API
#define EASTDC_LOCAL

namespace EA {
namespace StdC {


	class EASTDC_API Stopwatch
	{
	public:
		/// Units
		/// Defines common timing units plus a user-definable set of units.
		enum Units
		{
			kUnitsCycles       =    0,  /// Stopwatch clock ticks. May or may not match CPU clock ticks 1:1, depending on your hardware and operating system. Some CPUs' low level cycle count instruction counts every 16 cycles instead of every cycle. 
			kUnitsCPUCycles    =    1,  /// CPU clock ticks (or similar equivalent for the platform). Not recommended for use in shipping softare as many systems alter their CPU frequencies at runtime.
			kUnitsNanoseconds  =    2,  /// For a 1GHz processor, 1 nanosecond is the same as 1 clock tick.
			kUnitsMilliseconds =    4,  /// For a 1GHz processor, 1 millisecond is the same as 1,000,000 clock ticks.
		};

	public:
		/// Stopwatch
		/// Constructor for Stopwatch, allows user to specify units. 
		/// If units are kUnitsUserDefined,  you'll need to either subclass 
		/// Stopwatch and implement GetUserDefinedStopwatchCycle or call 
		/// SetUserDefinedUnitsToStopwatchCyclesRatio in order to allow it 
		/// to know how to convert units.
		explicit Stopwatch(int nUnits = kUnitsCycles, bool bStartImmediately = false)
			: mnStartTime(0)
			, mnTotalElapsedTime(0)
			, mnUnits(0)
			, mfStopwatchCyclesToUnitsCoefficient(0.f)
			 { } 

		/// Start
		/// Starts the stopwatch. Continues where it was last stopped. 
		/// Does nothing if the stopwatch is already started.
		void Start();

		/// Stop
		/// Stops the stopwatch it it was running and retaines the elasped time.
		void Stop();

		/// GetElapsedTime
		/// Gets the elapsed time, which properly takes into account any 
		/// intervening stops and starts. Works properly whether the stopwatch 
		/// is running or not.
		uint64_t GetElapsedTime() const; 

		/// GetStopwatchCycle
		/// Gets the current stopwatch cycle on the current machine.
		/// Note that a stopwatch cyle may or may not be the same thing
		/// as a CPU cycle. We provide the distinction between stopwatch
		/// cycles and CPU cycles in order to accomodate platforms (e.g.
		/// desktop platforms) in which CPU cycle counting is unreliable.
		static uint64_t GetStopwatchCycle();

		/// GetCPUCycle
		/// Gets the current CPU-based timer cycle on the current processor 
		/// (if in a multiprocessor system). Note that this doesn't necessarily
		/// get the actual machine CPU clock cycle; rather it returns the 
		/// CPU-based timer cycle. One some platforms the CPU-based timer is
		/// a 1:1 relation to the CPU clock, while on others it is some multiple
		/// of it.
		/// Note that on some systems you can't rely on kUnitsCycles being consistent 
		/// at runtime, especially on x86 PCs with their multiple desynchronized CPUs 
		/// and variable runtime clock speed.
		static uint64_t GetCPUCycle();

	private:
		uint64_t    mnStartTime;                            /// Start time; always in cycles.
		uint64_t    mnTotalElapsedTime;                     /// Elapsed time; always in cycles.
		int         mnUnits;                                /// Stopwatch units. One of enum Units or kUnitsUserDefined+.
		float       mfStopwatchCyclesToUnitsCoefficient;    /// Coefficient is defined seconds per cycle (assuming you want to measure seconds). This is the inverse of the frequency, done this way for speed. Time passed = cycle count * coefficient.                 

	}; // class Stopwatch



	class EASTDC_API LimitStopwatch : public Stopwatch
	{
	public:
		/// LimitStopwatch
		/// Constructor
		LimitStopwatch(int nUnits = kUnitsCycles, uint64_t nLimit = 0, bool bStartImmediately = false) : Stopwatch(), mnEndTime(0) { }

		/// IsTimeUp
		/// Returns true if the limit has been reached. Highly efficient.
		bool IsTimeUp() const; 

	protected:
		uint64_t mnEndTime;     /// The precomputed end time used by limit timing functions.

	}; // class LimitStopwatch

}} // namespace EA::StdC


inline
bool EA::StdC::LimitStopwatch::IsTimeUp() const
{
	const uint64_t nCurrentTime = GetStopwatchCycle();
	return (int64_t)(mnEndTime - nCurrentTime) < 0; // We do this instead of a straight compare to deal with possible integer wraparound issues.
}


inline
void EA::StdC::Stopwatch::Start()
{
	if(!mnStartTime) // If not already started...
	{
		if(mnUnits == kUnitsCPUCycles)
			mnStartTime = GetCPUCycle();
		else
			mnStartTime = GetStopwatchCycle();

	}
}

inline
void EA::StdC::Stopwatch::Stop()
{
	if(mnStartTime) // Check to make sure the stopwatch is actually running
	{
		const uint64_t nCurrentTime(mnUnits == kUnitsCPUCycles ? GetCPUCycle() : GetStopwatchCycle());
		mnTotalElapsedTime += (nCurrentTime - mnStartTime);
		mnStartTime = 0;
	}
}


inline
uint64_t EA::StdC::Stopwatch::GetElapsedTime() const
{
	uint64_t nFinalTotalElapsedTime64(mnTotalElapsedTime);

	if(mnStartTime) // We we are currently running, then take into account time passed since last start.
	{
		uint64_t nCurrentTime;

		// See the 'Stop' function for an explanation of the code below.
		if(mnUnits == kUnitsCPUCycles)
			nCurrentTime = GetCPUCycle();
		else
			nCurrentTime = GetStopwatchCycle();

		uint64_t nElapsed = nCurrentTime - mnStartTime;
		nFinalTotalElapsedTime64 += nElapsed;

	} // Now nFinalTotalElapsedTime64 holds the elapsed time in stopwatch cycles. 

	// We are doing a float to int cast here, which is a relatively slow operation on some CPUs.
	return (uint64_t)((nFinalTotalElapsedTime64 * mfStopwatchCyclesToUnitsCoefficient) + 0.49999f);
}


// Other supported processors have fixed-frequency CPUs and thus can 
// directly use the GetCPUCycle functionality for maximum precision
// and speed.
inline uint64_t EA::StdC::Stopwatch::GetStopwatchCycle()
{
	return GetCPUCycle();
}

#if defined(EA_PLATFORM_MICROSOFT)
	#include <windows.h>

	inline uint64_t EA::StdC::Stopwatch::GetCPUCycle()
	{
		LARGE_INTEGER perfCounter;
		QueryPerformanceCounter(&perfCounter);
		return static_cast<uint64_t>(perfCounter.QuadPart);
	}

#elif (defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64)) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

	inline
	uint64_t EA::StdC::Stopwatch::GetCPUCycle()
	{
		uint32_t eaxLow32, edxHigh32;
		uint64_t result;

		asm volatile("rdtsc" : "=a" (eaxLow32), "=d" (edxHigh32));
		result = ((uint64_t)edxHigh32 << 32) | ((uint64_t)eaxLow32);

		return result;
	}
#endif


#endif  // EASTDC_EASTOPWATCH_H
