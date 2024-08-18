/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// This one first because it defines EASTL_THREAD_SUPPORT_AVAILABLE
#include <EASTL/internal/thread_support.h>

#if EASTL_THREAD_SUPPORT_AVAILABLE

#include <EASTL/atomic.h>
#include <eathread/eathread_thread.h>

#include "EASTLTest.h"

/**
 * These are some basic tests to test "atomicity" of operations.
 */

namespace
{

	struct Test128BitType
	{
		Test128BitType() = default;
		Test128BitType(uint32_t x) : a{x}, b{x}, c{x}, d{x} {};
		Test128BitType(uint32_t x, uint32_t y, uint32_t z, uint32_t w) : a{x}, b{y}, c{z}, d{w} {};

		bool operator==(const Test128BitType& rhs) const
		{
			return a == rhs.a && b == rhs.b && c == rhs.c && d == rhs.d;
		}

		uint32_t a{};
		uint32_t b{};
		uint32_t c{};
		uint32_t d{};
	};

	bool AreAllMembersEqual(const Test128BitType& x) { return x.a == x.b && x.b == x.c && x.c == x.d; }

// We don't support 128bit atomics on 32-bit x86.
#if !defined(EA_PROCESSOR_X86)
	struct Test128BitLoadStoreData
	{
		eastl::atomic<Test128BitType> mX;
		EA::Thread::Semaphore mStartSem{0};
		eastl::atomic<bool> mShouldStop{};
		eastl::atomic<bool> mFoundTornRead{};
	};

	static intptr_t RelaxedLoadAndCheck(void* v)
	{
		Test128BitLoadStoreData* data = static_cast<Test128BitLoadStoreData*>(v);

		// Wait to be signaled that we can start.
		data->mStartSem.Wait();

		bool foundTornRead = false;
		// Until we're signaled we should stop.
		while (!data->mShouldStop.load(eastl::memory_order_relaxed))
		{
			// Do a relaxed load and make sure we don't ever tear. The
			// writing thread will do atomic stores with values satisfying
			// this condition.
			auto loaded = data->mX.load(eastl::memory_order_relaxed);

			foundTornRead |= !AreAllMembersEqual(loaded);
		}

		data->mFoundTornRead = foundTornRead;

		return 0;
	}

	static intptr_t RelaxedStore(void* v)
	{
		Test128BitLoadStoreData* data = static_cast<Test128BitLoadStoreData*>(v);

		// Wait to be signaled that we can start.
		data->mStartSem.Wait();

		uint32_t counter{};

		// Until we're signaled we should stop.
		while (!data->mShouldStop.load(eastl::memory_order_relaxed))
		{
			// This thread just stores incremental values which have identical entries.
			Test128BitType x{counter};
			data->mX.store(x, eastl::memory_order_relaxed);

			// This could wrap, but that's OK, it's not UB on unsigned types.
			++counter;
		}

		return 0;
	}
#endif

	template <class T>
	struct TestSequentialConsistencyData
	{
		eastl::atomic<T> mA1;
		eastl::atomic<T> mA2;
		eastl::atomic<T> mB1;
		eastl::atomic<T> mB2;
		EA::Thread::Semaphore mStartSem1{0};
		EA::Thread::Semaphore mStartSem2{0};
		EA::Thread::Semaphore mEndSem{0};
		eastl::atomic<uint32_t> mLoopCount{10000};
		eastl::atomic<uint32_t> mThreadCounter{};
	};

	template <class T>
	static intptr_t TestSequentialConsistencyThreadFn(void* v)
	{
		TestSequentialConsistencyData<T>& data = *static_cast<TestSequentialConsistencyData<T>*>(v);

		// We're doing x = 1, r = y in order, but we want different
		// order on different threads, and different r's for the
		// different orders.
		//
		// See the following link for details:
		// https://preshing.com/20120515/memory-reordering-caught-in-the-act/

		const uint32_t threadNumber = ++data.mThreadCounter;
		const bool chooseFirst = threadNumber % 2;

		auto& x = chooseFirst ? data.mA1 : data.mA2;
		auto& y = chooseFirst ? data.mA2 : data.mA1;
		auto& r = chooseFirst ? data.mB1 : data.mB2;

		auto& startSem = chooseFirst ? data.mStartSem1 : data.mStartSem2;

		uint32_t loops = data.mLoopCount;
		while (loops--)
		{
			startSem.Wait();

			// x = 1;
			x.store(T(1), eastl::memory_order_seq_cst);

			// r = y;
			r.store(y.load(eastl::memory_order_seq_cst), eastl::memory_order_seq_cst);

			data.mEndSem.Post();
		}

		return 0;
	}

	template <class T>
	static int TestSequentialConsistencyImpl()
	{
		TestSequentialConsistencyData<T> data;

		int nErrorCount = 0;
		EA::Thread::Thread threads[2];

		threads[0].Begin(TestSequentialConsistencyThreadFn<T>, static_cast<void*>(&data));
		threads[1].Begin(TestSequentialConsistencyThreadFn<T>, static_cast<void*>(&data));

		uint32_t loops = data.mLoopCount;

		while (loops--)
		{
			// Reset the input.
			data.mA1.store(T(0), eastl::memory_order_seq_cst);
			data.mA2.store(T(0), eastl::memory_order_seq_cst);

			// Signal the threads
			data.mStartSem1.Post();
			data.mStartSem2.Post();

			// Wait for both threads;
			data.mEndSem.Wait();
			data.mEndSem.Wait();

			// Check if there was a CPU reorder.
			const auto b1 = data.mB1.load(eastl::memory_order_seq_cst);
			const auto b2 = data.mB2.load(eastl::memory_order_seq_cst);

			const bool reorderHappened = (b1 == T{0} && b2 == T{0});
			EATEST_VERIFY(!reorderHappened);
		}

		return nErrorCount;
	}


} // namespace

int Test128BitLoadStoreMultiThreaded()
{
	int nErrorCount = 0;

// We don't support 128bit atomics on 32-bit x86.
#if !defined(EA_PROCESSOR_X86)
	constexpr int kNumThreads = 4; // 2 readers and 2 writers.
	EA::Thread::Thread threads[kNumThreads];
	Test128BitLoadStoreData data;

	for (int i = 0; (i + 1) < kNumThreads; i += 2)
	{
		threads[i].Begin(RelaxedLoadAndCheck, static_cast<void*>(&data));
		threads[i + 1].Begin(RelaxedStore, static_cast<void*>(&data));
	}

	data.mStartSem.Post(kNumThreads);

	EA::Thread::ThreadSleep(3000);

	data.mShouldStop = true;

	for (auto& t : threads)
	{
		t.WaitForEnd();
	}

	nErrorCount += data.mFoundTornRead;
#endif

	return nErrorCount;
}

int TestSequentialConsistency()
{
	int nErrorCount = 0;

	nErrorCount += TestSequentialConsistencyImpl<uint16_t>();
	nErrorCount += TestSequentialConsistencyImpl<uint32_t>();
	nErrorCount += TestSequentialConsistencyImpl<uint64_t>();

// We don't support 128bit atomics on 32-bit x86.
#if !defined(EA_PROCESSOR_X86)
	nErrorCount += TestSequentialConsistencyImpl<Test128BitType>();
#endif

	return nErrorCount;
}

int TestAtomicMultiThreaded()
{
	int nErrorCount = 0;

	nErrorCount += Test128BitLoadStoreMultiThreaded();
	nErrorCount += TestSequentialConsistency();

	return nErrorCount;
}

#endif
