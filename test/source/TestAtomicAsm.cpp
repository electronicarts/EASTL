/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"

#include <EASTL/atomic.h>

#include <cstddef>


struct UserType128
{
	uint32_t a,b,c,d;

	friend bool operator==(const UserType128& a, const UserType128& b)
	{
		return (a.a == b.a) && (a.b == b.b) && (a.c == b.c) && (a.d == b.d);
	}
};


/**
 * There is no nice way to verify the emitted asm for each of the given operations.
 * This test file is meant to put each operation into its own function so its easy
 * to verify in a disassembler for manual inspection.
 */

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32StoreRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	atomic.store(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(atomic);
}

EA_NO_INLINE static void TestAtomicU32StoreRelease()
{
	eastl::atomic<uint32_t> atomic;

	atomic.store(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(atomic);
}

EA_NO_INLINE static void TestAtomicU32StoreSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	atomic.store(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(atomic);
}

EA_NO_INLINE static void TestAtomicU32Store()
{
	eastl::atomic<uint32_t> atomic;

	atomic.store(1);

	eastl::compiler_barrier_data_dependency(atomic);
}

EA_NO_INLINE static void TestAtomicU32StoreOrders()
{
	TestAtomicU32StoreRelaxed();

	TestAtomicU32StoreRelease();

	TestAtomicU32StoreSeqCst();

	TestAtomicU32Store();
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64StoreRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	atomic.store(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(atomic);
}

EA_NO_INLINE static void TestAtomicU64StoreRelease()
{
	eastl::atomic<uint64_t> atomic;

	atomic.store(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(atomic);
}

EA_NO_INLINE static void TestAtomicU64StoreSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	atomic.store(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(atomic);
}

EA_NO_INLINE static void TestAtomicU64Store()
{
	eastl::atomic<uint64_t> atomic;

	atomic.store(1);

	eastl::compiler_barrier_data_dependency(atomic);
}

EA_NO_INLINE static void TestAtomicU64StoreOrders()
{
	TestAtomicU64StoreRelaxed();

	TestAtomicU64StoreRelease();

	TestAtomicU64StoreSeqCst();

	TestAtomicU64Store();
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT)

EA_NO_INLINE static void TestAtomic128StoreRelaxed()
{
	eastl::atomic<UserType128> atomic;

	atomic.store(UserType128{1, 1, 1, 1}, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(atomic);
}

EA_NO_INLINE static void TestAtomic128StoreRelease()
{
	eastl::atomic<UserType128> atomic;

	atomic.store(UserType128{1, 1, 1, 1}, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(atomic);
}

EA_NO_INLINE static void TestAtomic128StoreSeqCst()
{
	eastl::atomic<UserType128> atomic;

	atomic.store(UserType128{1, 1, 1, 1}, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(atomic);
}

EA_NO_INLINE static void TestAtomic128Store()
{
	eastl::atomic<UserType128> atomic;

	atomic.store(UserType128{1, 1, 1, 1});

	eastl::compiler_barrier_data_dependency(atomic);
}

EA_NO_INLINE static void TestAtomic128StoreOrders()
{
	TestAtomic128StoreRelaxed();

	TestAtomic128StoreRelease();

	TestAtomic128StoreSeqCst();

	TestAtomic128Store();
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32LoadRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t load = atomic.load(eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(load);
}

EA_NO_INLINE static void TestAtomicU32LoadAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t load = atomic.load(eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(load);
}

EA_NO_INLINE static void TestAtomicU32LoadSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t load = atomic.load(eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(load);
}

EA_NO_INLINE static void TestAtomicU32Load()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t load = atomic.load();

	eastl::compiler_barrier_data_dependency(load);
}

EA_NO_INLINE static void TestAtomicU32LoadOrders()
{
	TestAtomicU32LoadRelaxed();

	TestAtomicU32LoadAcquire();

	TestAtomicU32LoadSeqCst();

	TestAtomicU32Load();
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64LoadRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t load = atomic.load(eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(load);
}

EA_NO_INLINE static void TestAtomicU64LoadAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t load = atomic.load(eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(load);
}

EA_NO_INLINE static void TestAtomicU64LoadSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t load = atomic.load(eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(load);
}

EA_NO_INLINE static void TestAtomicU64Load()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t load = atomic.load();

	eastl::compiler_barrier_data_dependency(load);
}

EA_NO_INLINE static void TestAtomicU64LoadOrders()
{
	TestAtomicU64LoadRelaxed();

	TestAtomicU64LoadAcquire();

	TestAtomicU64LoadSeqCst();

	TestAtomicU64Load();
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT)

EA_NO_INLINE static void TestAtomic128LoadRelaxed()
{
	eastl::atomic<UserType128> atomic;

	UserType128 load = atomic.load(eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(load);
}

EA_NO_INLINE static void TestAtomic128LoadAcquire()
{
	eastl::atomic<UserType128> atomic;

	UserType128 load = atomic.load(eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(load);
}

EA_NO_INLINE static void TestAtomic128LoadSeqCst()
{
	eastl::atomic<UserType128> atomic;

	UserType128 load = atomic.load(eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(load);
}

EA_NO_INLINE static void TestAtomic128Load()
{
	eastl::atomic<UserType128> atomic;

	UserType128 load = atomic.load();

	eastl::compiler_barrier_data_dependency(load);
}

EA_NO_INLINE static void TestAtomic128LoadOrders()
{
	TestAtomic128LoadRelaxed();

	TestAtomic128LoadAcquire();

	TestAtomic128LoadSeqCst();

	TestAtomic128Load();
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32ExchangeRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t exchange = atomic.exchange(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomicU32ExchangeAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t exchange = atomic.exchange(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomicU32ExchangeRelease()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t exchange = atomic.exchange(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomicU32ExchangeAcqRel()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t exchange = atomic.exchange(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomicU32ExchangeSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t exchange = atomic.exchange(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomicU32Exchange()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t exchange = atomic.exchange(1);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomicU32ExchangeOrders()
{
	TestAtomicU32ExchangeRelaxed();

	TestAtomicU32ExchangeAcquire();

	TestAtomicU32ExchangeRelease();

	TestAtomicU32ExchangeAcqRel();

	TestAtomicU32ExchangeSeqCst();

	TestAtomicU32Exchange();
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64ExchangeRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t exchange = atomic.exchange(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomicU64ExchangeAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t exchange = atomic.exchange(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomicU64ExchangeRelease()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t exchange = atomic.exchange(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomicU64ExchangeAcqRel()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t exchange = atomic.exchange(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomicU64ExchangeSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t exchange = atomic.exchange(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomicU64Exchange()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t exchange = atomic.exchange(1);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomicU64ExchangeOrders()
{
	TestAtomicU64ExchangeRelaxed();

	TestAtomicU64ExchangeAcquire();

	TestAtomicU64ExchangeRelease();

	TestAtomicU64ExchangeAcqRel();

	TestAtomicU64ExchangeSeqCst();

	TestAtomicU64Exchange();
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT)

EA_NO_INLINE static void TestAtomic128ExchangeRelaxed()
{
	eastl::atomic<UserType128> atomic;

	UserType128 exchange = atomic.exchange(UserType128{1, 1, 1, 1}, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomic128ExchangeAcquire()
{
	eastl::atomic<UserType128> atomic;

	UserType128 exchange = atomic.exchange(UserType128{1, 1, 1, 1}, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomic128ExchangeRelease()
{
	eastl::atomic<UserType128> atomic;

	UserType128 exchange = atomic.exchange(UserType128{1, 1, 1, 1}, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomic128ExchangeAcqRel()
{
	eastl::atomic<UserType128> atomic;

	UserType128 exchange = atomic.exchange(UserType128{1, 1, 1, 1}, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomic128ExchangeSeqCst()
{
	eastl::atomic<UserType128> atomic;

	UserType128 exchange = atomic.exchange(UserType128{1, 1, 1, 1}, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomic128Exchange()
{
	eastl::atomic<UserType128> atomic;

	UserType128 exchange = atomic.exchange(UserType128{1, 1, 1, 1});

	eastl::compiler_barrier_data_dependency(exchange);
}

EA_NO_INLINE static void TestAtomic128ExchangeOrders()
{
	TestAtomic128ExchangeRelaxed();

	TestAtomic128ExchangeAcquire();

	TestAtomic128ExchangeRelease();

	TestAtomic128ExchangeAcqRel();

	TestAtomic128ExchangeSeqCst();

	TestAtomic128Exchange();
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32OperatorT()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t load = atomic;

	eastl::compiler_barrier_data_dependency(load);
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64OperatorT()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t load = atomic;

	eastl::compiler_barrier_data_dependency(load);
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT)

EA_NO_INLINE static void TestAtomic128OperatorT()
{
	eastl::atomic<UserType128> atomic;

	UserType128 load = atomic;

	eastl::compiler_barrier_data_dependency(load);
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32OperatorEqual()
{
	eastl::atomic<uint32_t> atomic;

	atomic = 1;

	eastl::compiler_barrier_data_dependency(atomic);
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64OperatorEqual()
{
	eastl::atomic<uint64_t> atomic;

	atomic = 1;

	eastl::compiler_barrier_data_dependency(atomic);
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT)

EA_NO_INLINE static void TestAtomic128OperatorEqual()
{
	eastl::atomic<UserType128> atomic;

	atomic = UserType128{1, 1, 1, 1};

	eastl::compiler_barrier_data_dependency(atomic);
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrongRelaxedRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_relaxed, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrongAcquireRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_acquire, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrongAcquireAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_acquire, eastl::memory_order_acquire);


	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrongReleaseRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_release, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrongAcqRelRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_acq_rel, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrongAcqRelAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_acq_rel, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrongSeqCstRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_seq_cst, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrongSeqCstAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_seq_cst, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrongSeqCstSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_seq_cst, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrongRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrongAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrongRelease()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrongAcqRel()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrongSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrong()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeStrongOrders()
{
	TestAtomicU32CompareExchangeStrongRelaxedRelaxed();

	TestAtomicU32CompareExchangeStrongAcquireRelaxed();

	TestAtomicU32CompareExchangeStrongAcquireAcquire();

	TestAtomicU32CompareExchangeStrongReleaseRelaxed();

	TestAtomicU32CompareExchangeStrongAcqRelRelaxed();

	TestAtomicU32CompareExchangeStrongAcqRelAcquire();

	TestAtomicU32CompareExchangeStrongSeqCstRelaxed();

	TestAtomicU32CompareExchangeStrongSeqCstAcquire();

	TestAtomicU32CompareExchangeStrongSeqCstSeqCst();

	TestAtomicU32CompareExchangeStrongRelaxed();

	TestAtomicU32CompareExchangeStrongAcquire();

	TestAtomicU32CompareExchangeStrongRelease();

	TestAtomicU32CompareExchangeStrongAcqRel();

	TestAtomicU32CompareExchangeStrongSeqCst();

	TestAtomicU32CompareExchangeStrong();
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrongRelaxedRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_relaxed, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrongAcquireRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_acquire, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrongAcquireAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_acquire, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrongReleaseRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_release, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrongAcqRelRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_acq_rel, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrongAcqRelAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_acq_rel, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrongSeqCstRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_seq_cst, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrongSeqCstAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_seq_cst, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrongSeqCstSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_seq_cst, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrongRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrongAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrongRelease()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrongAcqRel()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrongSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrong()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_strong(expected, 1);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeStrongOrders()
{
	TestAtomicU64CompareExchangeStrongRelaxedRelaxed();

	TestAtomicU64CompareExchangeStrongAcquireRelaxed();

	TestAtomicU64CompareExchangeStrongAcquireAcquire();

	TestAtomicU64CompareExchangeStrongReleaseRelaxed();

	TestAtomicU64CompareExchangeStrongAcqRelRelaxed();

	TestAtomicU64CompareExchangeStrongAcqRelAcquire();

	TestAtomicU64CompareExchangeStrongSeqCstRelaxed();

	TestAtomicU64CompareExchangeStrongSeqCstAcquire();

	TestAtomicU64CompareExchangeStrongSeqCstSeqCst();

	TestAtomicU64CompareExchangeStrongRelaxed();

	TestAtomicU64CompareExchangeStrongAcquire();

	TestAtomicU64CompareExchangeStrongRelease();

	TestAtomicU64CompareExchangeStrongAcqRel();

	TestAtomicU64CompareExchangeStrongSeqCst();

	TestAtomicU64CompareExchangeStrong();
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT)

EA_NO_INLINE static void TestAtomic128CompareExchangeStrongRelaxedRelaxed()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_strong(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_relaxed, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeStrongAcquireRelaxed()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_strong(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_acquire, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeStrongAcquireAcquire()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_strong(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_acquire, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeStrongReleaseRelaxed()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_strong(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_release, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeStrongAcqRelRelaxed()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_strong(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_acq_rel, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeStrongAcqRelAcquire()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_strong(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_acq_rel, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeStrongSeqCstRelaxed()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_strong(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_seq_cst, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeStrongSeqCstAcquire()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_strong(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_seq_cst, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeStrongSeqCstSeqCst()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_strong(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_seq_cst, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeStrongRelaxed()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_strong(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeStrongAcquire()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_strong(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeStrongRelease()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_strong(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeStrongAcqRel()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_strong(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeStrongSeqCst()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_strong(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeStrong()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_strong(expected, UserType128{1, 1, 1, 1});

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeStrongOrders()
{
	TestAtomic128CompareExchangeStrongRelaxedRelaxed();

	TestAtomic128CompareExchangeStrongAcquireRelaxed();

	TestAtomic128CompareExchangeStrongAcquireAcquire();

	TestAtomic128CompareExchangeStrongReleaseRelaxed();

	TestAtomic128CompareExchangeStrongAcqRelRelaxed();

	TestAtomic128CompareExchangeStrongAcqRelAcquire();

	TestAtomic128CompareExchangeStrongSeqCstRelaxed();

	TestAtomic128CompareExchangeStrongSeqCstAcquire();

	TestAtomic128CompareExchangeStrongSeqCstSeqCst();

	TestAtomic128CompareExchangeStrongRelaxed();

	TestAtomic128CompareExchangeStrongAcquire();

	TestAtomic128CompareExchangeStrongRelease();

	TestAtomic128CompareExchangeStrongAcqRel();

	TestAtomic128CompareExchangeStrongSeqCst();

	TestAtomic128CompareExchangeStrong();
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeakRelaxedRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_relaxed, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeakAcquireRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_acquire, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeakAcquireAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_acquire, eastl::memory_order_acquire);


	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeakReleaseRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_release, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeakAcqRelRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_acq_rel, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeakAcqRelAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_acq_rel, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeakSeqCstRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_seq_cst, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeakSeqCstAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_seq_cst, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeakSeqCstSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_seq_cst, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeakRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeakAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeakRelease()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeakAcqRel()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeakSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeak()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU32CompareExchangeWeakOrders()
{
	TestAtomicU32CompareExchangeWeakRelaxedRelaxed();

	TestAtomicU32CompareExchangeWeakAcquireRelaxed();

	TestAtomicU32CompareExchangeWeakAcquireAcquire();

	TestAtomicU32CompareExchangeWeakReleaseRelaxed();

	TestAtomicU32CompareExchangeWeakAcqRelRelaxed();

	TestAtomicU32CompareExchangeWeakAcqRelAcquire();

	TestAtomicU32CompareExchangeWeakSeqCstRelaxed();

	TestAtomicU32CompareExchangeWeakSeqCstAcquire();

	TestAtomicU32CompareExchangeWeakSeqCstSeqCst();

	TestAtomicU32CompareExchangeWeakRelaxed();

	TestAtomicU32CompareExchangeWeakAcquire();

	TestAtomicU32CompareExchangeWeakRelease();

	TestAtomicU32CompareExchangeWeakAcqRel();

	TestAtomicU32CompareExchangeWeakSeqCst();

	TestAtomicU32CompareExchangeWeak();
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeakRelaxedRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_relaxed, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeakAcquireRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_acquire, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeakAcquireAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_acquire, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeakReleaseRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_release, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeakAcqRelRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_acq_rel, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeakAcqRelAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_acq_rel, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeakSeqCstRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_seq_cst, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeakSeqCstAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_seq_cst, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeakSeqCstSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_seq_cst, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeakRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeakAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeakRelease()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeakAcqRel()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeakSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeak()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t expected = 0;
	bool ret = atomic.compare_exchange_weak(expected, 1);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomicU64CompareExchangeWeakOrders()
{
	TestAtomicU64CompareExchangeWeakRelaxedRelaxed();

	TestAtomicU64CompareExchangeWeakAcquireRelaxed();

	TestAtomicU64CompareExchangeWeakAcquireAcquire();

	TestAtomicU64CompareExchangeWeakReleaseRelaxed();

	TestAtomicU64CompareExchangeWeakAcqRelRelaxed();

	TestAtomicU64CompareExchangeWeakAcqRelAcquire();

	TestAtomicU64CompareExchangeWeakSeqCstRelaxed();

	TestAtomicU64CompareExchangeWeakSeqCstAcquire();

	TestAtomicU64CompareExchangeWeakSeqCstSeqCst();

	TestAtomicU64CompareExchangeWeakRelaxed();

	TestAtomicU64CompareExchangeWeakAcquire();

	TestAtomicU64CompareExchangeWeakRelease();

	TestAtomicU64CompareExchangeWeakAcqRel();

	TestAtomicU64CompareExchangeWeakSeqCst();

	TestAtomicU64CompareExchangeWeak();
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT)

EA_NO_INLINE static void TestAtomic128CompareExchangeWeakRelaxedRelaxed()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_weak(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_relaxed, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeWeakAcquireRelaxed()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_weak(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_acquire, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeWeakAcquireAcquire()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_weak(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_acquire, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeWeakReleaseRelaxed()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_weak(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_release, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeWeakAcqRelRelaxed()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_weak(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_acq_rel, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeWeakAcqRelAcquire()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_weak(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_acq_rel, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeWeakSeqCstRelaxed()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_weak(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_seq_cst, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeWeakSeqCstAcquire()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_weak(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_seq_cst, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeWeakSeqCstSeqCst()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_weak(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_seq_cst, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeWeakRelaxed()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_weak(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeWeakAcquire()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_weak(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeWeakRelease()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_weak(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeWeakAcqRel()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_weak(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeWeakSeqCst()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_weak(expected, UserType128{1, 1, 1, 1}, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeWeak()
{
	eastl::atomic<UserType128> atomic;

	UserType128 expected = UserType128{0, 0, 0, 0};
	bool ret = atomic.compare_exchange_weak(expected, UserType128{1, 1, 1, 1});

	eastl::compiler_barrier_data_dependency(ret);
}

EA_NO_INLINE static void TestAtomic128CompareExchangeWeakOrders()
{
	TestAtomic128CompareExchangeWeakRelaxedRelaxed();

	TestAtomic128CompareExchangeWeakAcquireRelaxed();

	TestAtomic128CompareExchangeWeakAcquireAcquire();

	TestAtomic128CompareExchangeWeakReleaseRelaxed();

	TestAtomic128CompareExchangeWeakAcqRelRelaxed();

	TestAtomic128CompareExchangeWeakAcqRelAcquire();

	TestAtomic128CompareExchangeWeakSeqCstRelaxed();

	TestAtomic128CompareExchangeWeakSeqCstAcquire();

	TestAtomic128CompareExchangeWeakSeqCstSeqCst();

	TestAtomic128CompareExchangeWeakRelaxed();

	TestAtomic128CompareExchangeWeakAcquire();

	TestAtomic128CompareExchangeWeakRelease();

	TestAtomic128CompareExchangeWeakAcqRel();

	TestAtomic128CompareExchangeWeakSeqCst();

	TestAtomic128CompareExchangeWeak();
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32FetchAddRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_add(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchAddAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_add(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchAddRelease()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_add(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchAddAcqRel()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_add(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchAddSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_add(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchAdd()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_add(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchAddOrders()
{
	TestAtomicU32FetchAddRelaxed();

	TestAtomicU32FetchAddAcquire();

	TestAtomicU32FetchAddRelease();

	TestAtomicU32FetchAddAcqRel();

	TestAtomicU32FetchAddSeqCst();

	TestAtomicU32FetchAdd();
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64FetchAddRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_add(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchAddAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_add(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchAddRelease()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_add(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchAddAcqRel()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_add(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchAddSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_add(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchAdd()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_add(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchAddOrders()
{
	TestAtomicU64FetchAddRelaxed();

	TestAtomicU64FetchAddAcquire();

	TestAtomicU64FetchAddRelease();

	TestAtomicU64FetchAddAcqRel();

	TestAtomicU64FetchAddSeqCst();

	TestAtomicU64FetchAdd();
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128FetchAddRelaxed()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_add(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchAddAcquire()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_add(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchAddRelease()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_add(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchAddAcqRel()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_add(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchAddSeqCst()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_add(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchAdd()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_add(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchAddOrders()
{
	TestAtomic128FetchAddRelaxed();

	TestAtomic128FetchAddAcquire();

	TestAtomic128FetchAddRelease();

	TestAtomic128FetchAddAcqRel();

	TestAtomic128FetchAddSeqCst();

	TestAtomic128FetchAdd();
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32AddFetchRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.add_fetch(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32AddFetchAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.add_fetch(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32AddFetchRelease()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.add_fetch(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32AddFetchAcqRel()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.add_fetch(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32AddFetchSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.add_fetch(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32AddFetch()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.add_fetch(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32AddFetchOrders()
{
	TestAtomicU32AddFetchRelaxed();

	TestAtomicU32AddFetchAcquire();

	TestAtomicU32AddFetchRelease();

	TestAtomicU32AddFetchAcqRel();

	TestAtomicU32AddFetchSeqCst();

	TestAtomicU32AddFetch();
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64AddFetchRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.add_fetch(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64AddFetchAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.add_fetch(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64AddFetchRelease()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.add_fetch(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64AddFetchAcqRel()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.add_fetch(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64AddFetchSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.add_fetch(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64AddFetch()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.add_fetch(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64AddFetchOrders()
{
	TestAtomicU64AddFetchRelaxed();

	TestAtomicU64AddFetchAcquire();

	TestAtomicU64AddFetchRelease();

	TestAtomicU64AddFetchAcqRel();

	TestAtomicU64AddFetchSeqCst();

	TestAtomicU64AddFetch();
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128AddFetchRelaxed()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.add_fetch(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128AddFetchAcquire()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.add_fetch(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128AddFetchRelease()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.add_fetch(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128AddFetchAcqRel()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.add_fetch(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128AddFetchSeqCst()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.add_fetch(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128AddFetch()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.add_fetch(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128AddFetchOrders()
{
	TestAtomic128AddFetchRelaxed();

	TestAtomic128AddFetchAcquire();

	TestAtomic128AddFetchRelease();

	TestAtomic128AddFetchAcqRel();

	TestAtomic128AddFetchSeqCst();

	TestAtomic128AddFetch();
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32FetchSubRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_sub(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchSubAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_sub(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchSubRelease()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_sub(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchSubAcqRel()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_sub(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchSubSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_sub(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchSub()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_sub(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchSubOrders()
{
	TestAtomicU32FetchSubRelaxed();

	TestAtomicU32FetchSubAcquire();

	TestAtomicU32FetchSubRelease();

	TestAtomicU32FetchSubAcqRel();

	TestAtomicU32FetchSubSeqCst();

	TestAtomicU32FetchSub();
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64FetchSubRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_sub(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchSubAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_sub(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchSubRelease()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_sub(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchSubAcqRel()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_sub(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchSubSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_sub(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchSub()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_sub(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchSubOrders()
{
	TestAtomicU64FetchSubRelaxed();

	TestAtomicU64FetchSubAcquire();

	TestAtomicU64FetchSubRelease();

	TestAtomicU64FetchSubAcqRel();

	TestAtomicU64FetchSubSeqCst();

	TestAtomicU64FetchSub();
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128FetchSubRelaxed()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_sub(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchSubAcquire()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_sub(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchSubRelease()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_sub(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchSubAcqRel()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_sub(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchSubSeqCst()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_sub(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchSub()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_sub(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchSubOrders()
{
	TestAtomic128FetchSubRelaxed();

	TestAtomic128FetchSubAcquire();

	TestAtomic128FetchSubRelease();

	TestAtomic128FetchSubAcqRel();

	TestAtomic128FetchSubSeqCst();

	TestAtomic128FetchSub();
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32SubFetchRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.sub_fetch(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32SubFetchAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.sub_fetch(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32SubFetchRelease()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.sub_fetch(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32SubFetchAcqRel()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.sub_fetch(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32SubFetchSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.sub_fetch(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32SubFetch()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.sub_fetch(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32SubFetchOrders()
{
	TestAtomicU32SubFetchRelaxed();

	TestAtomicU32SubFetchAcquire();

	TestAtomicU32SubFetchRelease();

	TestAtomicU32SubFetchAcqRel();

	TestAtomicU32SubFetchSeqCst();

	TestAtomicU32SubFetch();
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64SubFetchRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.sub_fetch(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64SubFetchAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.sub_fetch(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64SubFetchRelease()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.sub_fetch(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64SubFetchAcqRel()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.sub_fetch(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64SubFetchSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.sub_fetch(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64SubFetch()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.sub_fetch(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64SubFetchOrders()
{
	TestAtomicU64SubFetchRelaxed();

	TestAtomicU64SubFetchAcquire();

	TestAtomicU64SubFetchRelease();

	TestAtomicU64SubFetchAcqRel();

	TestAtomicU64SubFetchSeqCst();

	TestAtomicU64SubFetch();
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128SubFetchRelaxed()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.sub_fetch(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128SubFetchAcquire()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.sub_fetch(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128SubFetchRelease()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.sub_fetch(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128SubFetchAcqRel()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.sub_fetch(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128SubFetchSeqCst()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.sub_fetch(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128SubFetch()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.sub_fetch(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128SubFetchOrders()
{
	TestAtomic128SubFetchRelaxed();

	TestAtomic128SubFetchAcquire();

	TestAtomic128SubFetchRelease();

	TestAtomic128SubFetchAcqRel();

	TestAtomic128SubFetchSeqCst();

	TestAtomic128SubFetch();
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32FetchAndRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_and(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchAndAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_and(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchAndRelease()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_and(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchAndAcqRel()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_and(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchAndSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_and(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchAnd()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_and(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchAndOrders()
{
	TestAtomicU32FetchAndRelaxed();

	TestAtomicU32FetchAndAcquire();

	TestAtomicU32FetchAndRelease();

	TestAtomicU32FetchAndAcqRel();

	TestAtomicU32FetchAndSeqCst();

	TestAtomicU32FetchAnd();
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64FetchAndRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_and(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchAndAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_and(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchAndRelease()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_and(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchAndAcqRel()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_and(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchAndSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_and(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchAnd()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_and(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchAndOrders()
{
	TestAtomicU64FetchAndRelaxed();

	TestAtomicU64FetchAndAcquire();

	TestAtomicU64FetchAndRelease();

	TestAtomicU64FetchAndAcqRel();

	TestAtomicU64FetchAndSeqCst();

	TestAtomicU64FetchAnd();
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128FetchAndRelaxed()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_and(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchAndAcquire()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_and(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchAndRelease()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_and(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchAndAcqRel()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_and(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchAndSeqCst()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_and(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchAnd()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_and(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchAndOrders()
{
	TestAtomic128FetchAndRelaxed();

	TestAtomic128FetchAndAcquire();

	TestAtomic128FetchAndRelease();

	TestAtomic128FetchAndAcqRel();

	TestAtomic128FetchAndSeqCst();

	TestAtomic128FetchAnd();
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32AndFetchRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.and_fetch(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32AndFetchAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.and_fetch(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32AndFetchRelease()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.and_fetch(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32AndFetchAcqRel()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.and_fetch(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32AndFetchSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.and_fetch(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32AndFetch()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.and_fetch(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32AndFetchOrders()
{
	TestAtomicU32AndFetchRelaxed();

	TestAtomicU32AndFetchAcquire();

	TestAtomicU32AndFetchRelease();

	TestAtomicU32AndFetchAcqRel();

	TestAtomicU32AndFetchSeqCst();

	TestAtomicU32AndFetch();
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64AndFetchRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.and_fetch(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64AndFetchAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.and_fetch(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64AndFetchRelease()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.and_fetch(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64AndFetchAcqRel()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.and_fetch(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64AndFetchSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.and_fetch(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64AndFetch()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.and_fetch(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64AndFetchOrders()
{
	TestAtomicU64AndFetchRelaxed();

	TestAtomicU64AndFetchAcquire();

	TestAtomicU64AndFetchRelease();

	TestAtomicU64AndFetchAcqRel();

	TestAtomicU64AndFetchSeqCst();

	TestAtomicU64AndFetch();
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128AndFetchRelaxed()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.and_fetch(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128AndFetchAcquire()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.and_fetch(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128AndFetchRelease()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.and_fetch(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128AndFetchAcqRel()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.and_fetch(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128AndFetchSeqCst()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.and_fetch(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128AndFetch()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.and_fetch(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128AndFetchOrders()
{
	TestAtomic128AndFetchRelaxed();

	TestAtomic128AndFetchAcquire();

	TestAtomic128AndFetchRelease();

	TestAtomic128AndFetchAcqRel();

	TestAtomic128AndFetchSeqCst();

	TestAtomic128AndFetch();
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32OrFetchRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.or_fetch(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32OrFetchAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.or_fetch(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32OrFetchRelease()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.or_fetch(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32OrFetchAcqRel()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.or_fetch(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32OrFetchSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.or_fetch(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32OrFetch()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.or_fetch(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32OrFetchOrders()
{
	TestAtomicU32OrFetchRelaxed();

	TestAtomicU32OrFetchAcquire();

	TestAtomicU32OrFetchRelease();

	TestAtomicU32OrFetchAcqRel();

	TestAtomicU32OrFetchSeqCst();

	TestAtomicU32OrFetch();
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64OrFetchRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.or_fetch(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64OrFetchAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.or_fetch(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64OrFetchRelease()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.or_fetch(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64OrFetchAcqRel()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.or_fetch(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64OrFetchSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.or_fetch(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64OrFetch()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.or_fetch(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64OrFetchOrders()
{
	TestAtomicU64OrFetchRelaxed();

	TestAtomicU64OrFetchAcquire();

	TestAtomicU64OrFetchRelease();

	TestAtomicU64OrFetchAcqRel();

	TestAtomicU64OrFetchSeqCst();

	TestAtomicU64OrFetch();
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128OrFetchRelaxed()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.or_fetch(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128OrFetchAcquire()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.or_fetch(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128OrFetchRelease()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.or_fetch(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128OrFetchAcqRel()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.or_fetch(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128OrFetchSeqCst()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.or_fetch(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128OrFetch()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.or_fetch(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128OrFetchOrders()
{
	TestAtomic128OrFetchRelaxed();

	TestAtomic128OrFetchAcquire();

	TestAtomic128OrFetchRelease();

	TestAtomic128OrFetchAcqRel();

	TestAtomic128OrFetchSeqCst();

	TestAtomic128OrFetch();
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32FetchOrRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_or(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchOrAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_or(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchOrRelease()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_or(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchOrAcqRel()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_or(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchOrSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_or(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchOr()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_or(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchOrOrders()
{
	TestAtomicU32FetchOrRelaxed();

	TestAtomicU32FetchOrAcquire();

	TestAtomicU32FetchOrRelease();

	TestAtomicU32FetchOrAcqRel();

	TestAtomicU32FetchOrSeqCst();

	TestAtomicU32FetchOr();
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64FetchOrRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_or(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchOrAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_or(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchOrRelease()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_or(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchOrAcqRel()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_or(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchOrSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_or(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchOr()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_or(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchOrOrders()
{
	TestAtomicU64FetchOrRelaxed();

	TestAtomicU64FetchOrAcquire();

	TestAtomicU64FetchOrRelease();

	TestAtomicU64FetchOrAcqRel();

	TestAtomicU64FetchOrSeqCst();

	TestAtomicU64FetchOr();
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128FetchOrRelaxed()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_or(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchOrAcquire()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_or(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchOrRelease()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_or(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchOrAcqRel()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_or(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchOrSeqCst()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_or(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchOr()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_or(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchOrOrders()
{
	TestAtomic128FetchOrRelaxed();

	TestAtomic128FetchOrAcquire();

	TestAtomic128FetchOrRelease();

	TestAtomic128FetchOrAcqRel();

	TestAtomic128FetchOrSeqCst();

	TestAtomic128FetchOr();
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32FetchXorRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_xor(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchXorAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_xor(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchXorRelease()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_xor(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchXorAcqRel()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_xor(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchXorSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_xor(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchXor()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.fetch_xor(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32FetchXorOrders()
{
	TestAtomicU32FetchXorRelaxed();

	TestAtomicU32FetchXorAcquire();

	TestAtomicU32FetchXorRelease();

	TestAtomicU32FetchXorAcqRel();

	TestAtomicU32FetchXorSeqCst();

	TestAtomicU32FetchXor();
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64FetchXorRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_xor(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchXorAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_xor(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchXorRelease()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_xor(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchXorAcqRel()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_xor(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchXorSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_add(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchXor()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.fetch_xor(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64FetchXorOrders()
{
	TestAtomicU64FetchXorRelaxed();

	TestAtomicU64FetchXorAcquire();

	TestAtomicU64FetchXorRelease();

	TestAtomicU64FetchXorAcqRel();

	TestAtomicU64FetchXorSeqCst();

	TestAtomicU64FetchXor();
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128FetchXorRelaxed()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_xor(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchXorAcquire()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_xor(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchXorRelease()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_xor(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchXorAcqRel()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_xor(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchXorSeqCst()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_xor(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchXor()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.fetch_xor(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128FetchXorOrders()
{
	TestAtomic128FetchXorRelaxed();

	TestAtomic128FetchXorAcquire();

	TestAtomic128FetchXorRelease();

	TestAtomic128FetchXorAcqRel();

	TestAtomic128FetchXorSeqCst();

	TestAtomic128FetchXor();
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32XorFetchRelaxed()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.xor_fetch(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32XorFetchAcquire()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.xor_fetch(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32XorFetchRelease()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.xor_fetch(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32XorFetchAcqRel()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.xor_fetch(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32XorFetchSeqCst()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.xor_fetch(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32XorFetch()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic.xor_fetch(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU32XorFetchOrders()
{
	TestAtomicU32XorFetchRelaxed();

	TestAtomicU32XorFetchAcquire();

	TestAtomicU32XorFetchRelease();

	TestAtomicU32XorFetchAcqRel();

	TestAtomicU32XorFetchSeqCst();

	TestAtomicU32XorFetch();
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64XorFetchRelaxed()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.xor_fetch(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64XorFetchAcquire()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.xor_fetch(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64XorFetchRelease()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.xor_fetch(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64XorFetchAcqRel()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.xor_fetch(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64XorFetchSeqCst()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.xor_fetch(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64XorFetch()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic.xor_fetch(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomicU64XorFetchOrders()
{
	TestAtomicU64XorFetchRelaxed();

	TestAtomicU64XorFetchAcquire();

	TestAtomicU64XorFetchRelease();

	TestAtomicU64XorFetchAcqRel();

	TestAtomicU64XorFetchSeqCst();

	TestAtomicU64XorFetch();
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128XorFetchRelaxed()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.xor_fetch(1, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128XorFetchAcquire()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.xor_fetch(1, eastl::memory_order_acquire);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128XorFetchRelease()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.xor_fetch(1, eastl::memory_order_release);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128XorFetchAcqRel()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.xor_fetch(1, eastl::memory_order_acq_rel);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128XorFetchSeqCst()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.xor_fetch(1, eastl::memory_order_seq_cst);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128XorFetch()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic.xor_fetch(1);

	eastl::compiler_barrier_data_dependency(val);
}

EA_NO_INLINE static void TestAtomic128XorFetchOrders()
{
	TestAtomic128XorFetchRelaxed();

	TestAtomic128XorFetchAcquire();

	TestAtomic128XorFetchRelease();

	TestAtomic128XorFetchAcqRel();

	TestAtomic128XorFetchSeqCst();

	TestAtomic128XorFetch();
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32OperatorPlusPlus()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic++;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64OperatorPlusPlus()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic++;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128OperatorPlusPlus()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic++;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32PlusPlusOperator()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = ++atomic;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64PlusPlusOperator()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = ++atomic;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128PlusPlusOperator()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = ++atomic;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32OperatorMinusMinus()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic--;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64OperatorMinusMinus()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic--;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128OperatorMinusMinus()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic--;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32MinusMinusOperator()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = --atomic;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64MinusMinusOperator()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = --atomic;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128MinusMinusOperator()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = --atomic;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32OperatorPlusAssignment()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic += 1;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64OperatorPlusAssignment()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic += 1;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128OperatorPlusAssignment()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic += 1;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32OperatorMinusAssignment()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic -= 1;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64OperatorMinusAssignment()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic -= 1;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128OperatorMinusAssignment()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic -= 1;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32OperatorAndAssignment()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic &= 1;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64OperatorAndAssignment()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic &= 1;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128OperatorAndAssignment()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic &= 1;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32OperatorOrAssignment()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic |= 1;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64OperatorOrAssignment()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic |= 1;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128OperatorOrAssignment()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic |= 1;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomicU32OperatorXorAssignment()
{
	eastl::atomic<uint32_t> atomic;

	uint32_t val = atomic ^= 1;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_64BIT)

EA_NO_INLINE static void TestAtomicU64OperatorXorAssignment()
{
	eastl::atomic<uint64_t> atomic;

	uint64_t val = atomic ^= 1;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))

EA_NO_INLINE static void TestAtomic128OperatorXorAssignment()
{
	eastl::atomic<__uint128_t> atomic;

	__uint128_t val = atomic ^= 1;

	eastl::compiler_barrier_data_dependency(val);
}

#endif

EA_NO_INLINE static void TestAtomicSignalFenceRelaxed()
{
	eastl::atomic_signal_fence(eastl::memory_order_relaxed);
}

EA_NO_INLINE static void TestAtomicSignalFenceAcquire()
{
	eastl::atomic_signal_fence(eastl::memory_order_acquire);
}

EA_NO_INLINE static void TestAtomicSignalFenceRelease()
{
	eastl::atomic_signal_fence(eastl::memory_order_release);
}

EA_NO_INLINE static void TestAtomicSignalFenceAcqRel()
{
	eastl::atomic_signal_fence(eastl::memory_order_acq_rel);
}

EA_NO_INLINE static void TestAtomicSignalFenceSeqCst()
{
	eastl::atomic_signal_fence(eastl::memory_order_seq_cst);
}

EA_NO_INLINE static void TestAtomicThreadFenceRelaxed()
{
	eastl::atomic_thread_fence(eastl::memory_order_relaxed);
}

EA_NO_INLINE static void TestAtomicThreadFenceAcquire()
{
	eastl::atomic_thread_fence(eastl::memory_order_acquire);
}

EA_NO_INLINE static void TestAtomicThreadFenceRelease()
{
	eastl::atomic_thread_fence(eastl::memory_order_release);
}

EA_NO_INLINE static void TestAtomicThreadFenceAcqRel()
{
	eastl::atomic_thread_fence(eastl::memory_order_acq_rel);
}

EA_NO_INLINE static void TestAtomicThreadFenceSeqCst()
{
	eastl::atomic_thread_fence(eastl::memory_order_seq_cst);
}

EA_NO_INLINE static void TestAtomicPointerReadDepends()
{
	eastl::atomic<void*> atomic;

	void* p = atomic.load(eastl::memory_order_read_depends);

	eastl::compiler_barrier_data_dependency(p);
}

struct ReadDependsStruct
{
	int a;
	int b;
};

eastl::atomic<ReadDependsStruct*> gAtomicPtr;

EA_NO_INLINE int TestAtomicReadDependsStruct()
{
	ReadDependsStruct* p = gAtomicPtr.load(eastl::memory_order_read_depends);

	int a = p->a;
	int b = p->b;

	return a + b;
}

EA_NO_INLINE static void TestCompilerBarrierDataDependency()
{
	{
		UserType128 t{4, 5, 7, 8};

		eastl::compiler_barrier_data_dependency(t);
	}

	{
		void* p = (void*)0xdeadbeef;

		eastl::compiler_barrier_data_dependency(p);
	}

	{
		bool b = false;

		eastl::compiler_barrier_data_dependency(b);
	}
}

struct ReadDependsIntrusive
{
	int a;
	int b;
	struct ReadDependsIntrusive* next;
	int c;
	int d;
};

eastl::atomic<ReadDependsIntrusive**> gListHead;

EA_NO_INLINE static int TestAtomicReadDependsIntrusive()
{
	ReadDependsIntrusive** intrusivePtr = gListHead.load(eastl::memory_order_read_depends);
	ReadDependsIntrusive* ptr = ((ReadDependsIntrusive*)(((char*)intrusivePtr) - offsetof(ReadDependsIntrusive, next)));

	int a = ptr->a;
	int b = ptr->b;
	int c = ptr->c;
	int d = ptr->d;

	return a + b + c + d;
}

#if defined(EASTL_ATOMIC_HAS_32BIT)

EA_NO_INLINE static void TestAtomic32LoadStoreSameAddressSeqCst()
{
	eastl::atomic<uint32_t> atomic{0};

	uint32_t ret1 = atomic.load(eastl::memory_order_relaxed);

	atomic.store(4, eastl::memory_order_relaxed);

	uint32_t ret2 = atomic.load(eastl::memory_order_relaxed);

	uint32_t ret3 = atomic.load(eastl::memory_order_relaxed);

	atomic.store(5, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret1);
	eastl::compiler_barrier_data_dependency(ret2);
	eastl::compiler_barrier_data_dependency(ret3);
}

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT)

EA_NO_INLINE static void TestAtomic128LoadStoreSameAddressSeqCst()
{
	eastl::atomic<UserType128> atomic{UserType128{0, 0, 0, 0}};

	UserType128 ret1 = atomic.load(eastl::memory_order_relaxed);

	atomic.store(UserType128{1, 0, 2, 4}, eastl::memory_order_relaxed);

	UserType128 ret2 = atomic.load(eastl::memory_order_relaxed);

	UserType128 ret3 = atomic.load(eastl::memory_order_relaxed);

	atomic.store(UserType128{1, 1, 2, 4}, eastl::memory_order_relaxed);

	eastl::compiler_barrier_data_dependency(ret1);
	eastl::compiler_barrier_data_dependency(ret2);
	eastl::compiler_barrier_data_dependency(ret3);
}

#endif

int TestAtomicAsm()
{
	int nErrorCount = 0;

	// Stores
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32StoreOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64StoreOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT)
		TestAtomic128StoreOrders();
	#endif
	}

	// Loads
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32LoadOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64LoadOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT)
		TestAtomic128LoadOrders();
	#endif
	}

	// exchange
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32ExchangeOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64ExchangeOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT)
		TestAtomic128ExchangeOrders();
	#endif
	}

	// operator T
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32OperatorT();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64OperatorT();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT)
		TestAtomic128OperatorT();
	#endif
	}

	// operator=
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32OperatorEqual();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64OperatorEqual();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT)
		TestAtomic128OperatorEqual();
	#endif
	}

	// compare_exchange_weak
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32CompareExchangeWeakOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64CompareExchangeWeakOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT)
		TestAtomic128CompareExchangeWeakOrders();
	#endif
	}

	// compare_exchange_strong
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32CompareExchangeStrongOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64CompareExchangeStrongOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT)
		TestAtomic128CompareExchangeStrongOrders();
	#endif
	}

	// fetch_add
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32FetchAddOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64FetchAddOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128FetchAddOrders();
	#endif
	}

	// add_fetch
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32AddFetchOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64AddFetchOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128AddFetchOrders();
	#endif
	}

	// fetch_sub
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32FetchSubOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64FetchSubOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128FetchSubOrders();
	#endif
	}

	// sub_fetch
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32SubFetchOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64SubFetchOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128SubFetchOrders();
	#endif
	}

	// fetch_and
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32FetchAndOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64FetchAndOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128FetchAndOrders();
	#endif
	}

	// and_fetch
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32AndFetchOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64AndFetchOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128AndFetchOrders();
	#endif
	}

	// fetch_or
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32FetchOrOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64FetchOrOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
			TestAtomic128FetchOrOrders();
	#endif
	}

	// or_fetch
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32OrFetchOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64OrFetchOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128OrFetchOrders();
	#endif
	}

	// fetch_xor
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32FetchXorOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64FetchXorOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128FetchXorOrders();
	#endif
	}

	// xor_fetch
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32XorFetchOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64XorFetchOrders();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128XorFetchOrders();
	#endif
	}

	// operator++/++operator
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32OperatorPlusPlus();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64OperatorPlusPlus();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128OperatorPlusPlus();
	#endif

	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32PlusPlusOperator();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64PlusPlusOperator();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128PlusPlusOperator();
	#endif
	}

	// operator--/--operator
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32OperatorMinusMinus();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64OperatorMinusMinus();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128OperatorMinusMinus();
	#endif

	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32MinusMinusOperator();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64MinusMinusOperator();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128MinusMinusOperator();
	#endif
	}

	// operator+=
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32OperatorPlusAssignment();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64OperatorPlusAssignment();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128OperatorPlusAssignment();
	#endif
	}

	// operator-=
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32OperatorMinusAssignment();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64OperatorMinusAssignment();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128OperatorMinusAssignment();
	#endif
	}

	// operator&=
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32OperatorAndAssignment();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64OperatorAndAssignment();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128OperatorAndAssignment();
	#endif
	}

	// operator|=
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32OperatorOrAssignment();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64OperatorOrAssignment();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128OperatorOrAssignment();
	#endif
	}

	// operator^=
	{
	#if defined(EASTL_ATOMIC_HAS_32BIT)
		TestAtomicU32OperatorXorAssignment();
	#endif

	#if defined(EASTL_ATOMIC_HAS_64BIT)
		TestAtomicU64OperatorXorAssignment();
	#endif

	#if defined(EASTL_ATOMIC_HAS_128BIT) && (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG))
		TestAtomic128OperatorXorAssignment();
	#endif
	}

	// atomic_signal_fence
	{
		TestAtomicSignalFenceRelaxed();

		TestAtomicSignalFenceAcquire();

		TestAtomicSignalFenceRelease();

		TestAtomicSignalFenceAcqRel();

		TestAtomicSignalFenceSeqCst();
	}

	// atomic_thread_fence
	{
		TestAtomicThreadFenceRelaxed();

		TestAtomicThreadFenceAcquire();

		TestAtomicThreadFenceRelease();

		TestAtomicThreadFenceAcqRel();

		TestAtomicThreadFenceSeqCst();
	}

	// atomic pointer read depends
	{
		TestAtomicPointerReadDepends();
	}

	// atomic pointer read depends
	{
		ReadDependsStruct rds {3, 2};

		gAtomicPtr.store(&rds, eastl::memory_order_release);

		int ret = TestAtomicReadDependsStruct();
		eastl::compiler_barrier_data_dependency(ret);
	}

	{
		ReadDependsIntrusive rdi {3, 2, &rdi, 1, 0};

		gListHead.store(&(rdi.next), eastl::memory_order_release);

		int ret = TestAtomicReadDependsIntrusive();
		eastl::compiler_barrier_data_dependency(ret);
	}

	{
		TestCompilerBarrierDataDependency();
	}

#if defined(EASTL_ATOMIC_HAS_32BIT)

	TestAtomic32LoadStoreSameAddressSeqCst();

#endif

#if defined(EASTL_ATOMIC_HAS_128BIT)

	TestAtomic128LoadStoreSameAddressSeqCst();

#endif

	return nErrorCount;
}
