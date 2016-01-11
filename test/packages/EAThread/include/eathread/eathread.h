///////////////////////////////////////////////////////////////////////////////
// eathread_thread.h
//
// Copyright (c) Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EATHREAD_EATHREAD_H
#define EATHREAD_EATHREAD_H

#include <chrono>
#include <thread>

namespace EA {
namespace Thread {

	typedef std::chrono::milliseconds::rep ThreadTime;                               /// Current storage mechanism for time used by thread timeout functions. Units are milliseconds.
	const   ThreadTime kTimeoutImmediate = std::chrono::milliseconds::zero().count();/// Used to specify to functions to return immediately if the operation could not be done.
	const   ThreadTime kTimeoutNone = std::chrono::milliseconds::max().count();      /// Used to specify to functions to block without a timeout (i.e. block forever).
	const   ThreadTime kTimeoutYield = std::chrono::milliseconds::zero().count();    /// This is used with ThreadSleep to minimally yield to threads of equivalent priority.


	inline ThreadTime GetThreadTime()
	{
		using namespace std::chrono;
		auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		return (ThreadTime)ms.count();
	}

	inline void ThreadSleep(const ThreadTime& timeRelative = kTimeoutImmediate)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds((int)timeRelative));
	}

}} // namespace EA::Thread


#endif  // EATHREAD_EATHREAD_H
