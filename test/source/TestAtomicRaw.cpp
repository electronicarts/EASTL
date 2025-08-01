/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EASTL/atomic.h>
#include <EASTL/atomic_raw.h>
#include <EASTL/meta.h>

#include "EASTLTest.h"

// Minimal tests to make sure that all eastl::atomic_X functions compile with the memory
// orders we expect them to and do what we expect them to (single threaded tests only, for now)

template <class Fn, class... MemOrd>
int TestMemoryOrders(Fn fn, MemOrd... mem_ord)
{
	return (fn(mem_ord) + ...);
}

template <class Fn, class MemOrdSuccess, class MemOrdFailure>
int TestMemoryOrderPair(Fn fn, eastl::meta::type_list<MemOrdSuccess, MemOrdFailure>)
{
	return fn(MemOrdSuccess{}, MemOrdFailure{});
}

template <class... Pairs>
struct TestMemoryOrderPairs
{
	template <class Fn>
	static int Run(Fn fn)
	{
		return (TestMemoryOrderPair(fn, Pairs{}) + ...);
	}
};

template <class... MemOrd>
struct TestAtomicLoadAPIImpl
{
	template <typename T>
	static int Run(T initVal, T otherVal)
	{
		auto test = [=](auto mem_ord)
		{
			int nErrorCount{};
			T a{initVal};
			T x = eastl::atomic_raw_load(&a, mem_ord);
			VERIFY(x == initVal);
			VERIFY(a == initVal);
			a = otherVal;
			T y = eastl::atomic_raw_load(&a, mem_ord);
			VERIFY(otherVal == y);
			VERIFY(otherVal == a);
			return nErrorCount;
		};

		return TestMemoryOrders(test, MemOrd{}...);
	}
};

template <class T>
int TestAtomicLoadAPI(T initVal, T otherVal)
{

	using namespace eastl::internal;
	return TestAtomicLoadAPIImpl<memory_order_relaxed_s, memory_order_acquire_s, memory_order_seq_cst_s>::Run(initVal,
	                                                                                                          otherVal);
}

template <class T>
int TestAtomicStoreAPI(T initVal, T otherVal)
{
	auto test = [=](auto mem_ord)
	{
		int nErrorCount{};
		T a{initVal};
		T b = otherVal;
		eastl::atomic_raw_store(&a, b, mem_ord);
		VERIFY(a == otherVal);
		VERIFY(b == otherVal);
		return nErrorCount;
	};

	return TestMemoryOrders(test, eastl::memory_order_relaxed, eastl::memory_order_release,
	                        eastl::memory_order_seq_cst);
}

template <class T>
int TestAtomicExchangeAPI(T initVal, T exchangeVal)
{
	auto test = [=](auto mem_ord)
	{
		int nErrorCount{};
		T a{initVal};
		T x = eastl::atomic_raw_exchange(&a, exchangeVal, mem_ord);
		VERIFY(initVal == x);
		VERIFY(a == exchangeVal);
		T y = eastl::atomic_raw_exchange(&a, initVal, mem_ord);
		VERIFY(initVal == a);
		VERIFY(y == exchangeVal);
		return nErrorCount;
	};

	return TestMemoryOrders(test, eastl::memory_order_relaxed, eastl::memory_order_acquire, eastl::memory_order_release,
	                        eastl::memory_order_acq_rel, eastl::memory_order_seq_cst);
}

template <class T>
int TestAtomicCmpxchgAPI(T initVal, T newVal)
{
	using namespace eastl::internal;
	using namespace eastl::meta;

	// this can be used to test with one or two memory orders.
	auto test = [=](auto... mem_orders)
	{
		auto testInternal = [=](auto cmpxchg_fn)
		{
			int nErrorCount{};
			T a{initVal};
			T x = newVal;
			T y = initVal;
			VERIFY(cmpxchg_fn(&a, y, x, mem_orders...));
			VERIFY(y == initVal);
			VERIFY(a == newVal);
			VERIFY(!cmpxchg_fn(&a, y, x, mem_orders...));
			VERIFY(y == newVal);
			VERIFY(a == newVal);
			VERIFY(cmpxchg_fn(&a, y, initVal, mem_orders...));
			VERIFY(y == newVal);
			VERIFY(a == initVal);
			return nErrorCount;
		};

		auto cmpxchg_weak = [](auto&&... args)
		{ return eastl::atomic_raw_compare_exchange_weak(eastl::forward<decltype(args)>(args)...); };

		auto cmpxchg_strong = [](auto&&... args)
		{ return eastl::atomic_raw_compare_exchange_strong(eastl::forward<decltype(args)>(args)...); };

		return testInternal(cmpxchg_weak) + testInternal(cmpxchg_strong);
	};

	// Note: the supported pairs are specified by the standard like this:
	// - the failure ordering cannot be stronger than the success.
	// - the failure ordering cannot be one of release or acq_rel
	return TestMemoryOrderPairs<type_list<memory_order_relaxed_s, memory_order_relaxed_s>,
	                            type_list<memory_order_acquire_s, memory_order_relaxed_s>,
	                            type_list<memory_order_acquire_s, memory_order_acquire_s>,
	                            type_list<memory_order_release_s, memory_order_relaxed_s>,
	                            type_list<memory_order_acq_rel_s, memory_order_relaxed_s>,
	                            type_list<memory_order_acq_rel_s, memory_order_acquire_s>,
	                            type_list<memory_order_seq_cst_s, memory_order_relaxed_s>,
	                            type_list<memory_order_seq_cst_s, memory_order_acquire_s>,
	                            type_list<memory_order_seq_cst_s, memory_order_seq_cst_s>>::Run(test) +
	       // There are also versions which only take the order for the RMW operation order "deduce" the order for the
	       // load operation under the following rules:
	       // - if the order for RMW is acq_rel then the order for the load is acquire.
	       // - if the order for RMW is release then the order for the load is relaxed.
	       // - otherwise, if the order for RMW is X then the order for the load is X.
	       TestMemoryOrders(test, eastl::memory_order_relaxed, eastl::memory_order_acquire, eastl::memory_order_release,
	                        eastl::memory_order_acq_rel, eastl::memory_order_seq_cst);
}

template <class T, class U, class FetchFn, class VerificationFn>
int TestFetchFunction(T initVal, U increment, FetchFn fetchFn, VerificationFn verificationFn)
{
	auto test = [=](auto mem_ord)
	{
		int nErrorCount{};

		T x1{initVal};
		T x2{initVal};
		T y1 = fetchFn(&x1, increment, mem_ord);
		T y2 = verificationFn(x2, increment);
		VERIFY(x1 == x2);
		VERIFY(y1 == y2);

		return nErrorCount;
	};

	return TestMemoryOrders(test, eastl::memory_order_relaxed, eastl::memory_order_acquire, eastl::memory_order_release,
	                        eastl::memory_order_acq_rel, eastl::memory_order_seq_cst);
}

template <class T, class U>
int TestAtomicFetchCommonAPI(T initVal, U increment)
{
	auto fetchAddAtomicFn = [=](T* x, U y, auto mem_ord) { return eastl::atomic_raw_fetch_add(x, y, mem_ord); };
	auto fetchAddVerifyFn = [=](T& x, U y)
	{
		T ret = x;
		x += y;
		return ret;
	};

	auto addFetchAtomicFn = [=](T* x, U y, auto mem_ord) { return eastl::atomic_raw_add_fetch(x, y, mem_ord); };
	auto addFetchVerifyFn = [=](T& x, U y)
	{
		x += y;
		return x;
	};

	auto fetchSubAtomicFn = [=](T* x, U y, auto mem_ord) { return eastl::atomic_raw_fetch_sub(x, y, mem_ord); };
	auto fetchSubVerifyFn = [=](T& x, U y)
	{
		T ret = x;
		x -= y;
		return ret;
	};

	auto subFetchAtomicFn = [=](T* x, U y, auto mem_ord) { return eastl::atomic_raw_sub_fetch(x, y, mem_ord); };
	auto subFetchVerifyFn = [=](T& x, U y)
	{
		x -= y;
		return x;
	};

	return TestFetchFunction(initVal, increment, fetchAddAtomicFn, fetchAddVerifyFn) +
	       TestFetchFunction(initVal, increment, addFetchAtomicFn, addFetchVerifyFn) +
	       TestFetchFunction(initVal, increment, fetchSubAtomicFn, fetchSubVerifyFn) +
	       TestFetchFunction(initVal, increment, subFetchAtomicFn, subFetchVerifyFn);
}

template <class T, class U>
int TestAtomicFetchIntegralAPI(T initVal, U increment)
{
	auto fetchAndAtomicFn = [=](T* x, U y, auto mem_ord) { return eastl::atomic_raw_fetch_and(x, y, mem_ord); };
	auto fetchAndVerifyFn = [=](T& x, U y)
	{
		T ret = x;
		x &= y;
		return ret;
	};

	auto andFetchAtomicFn = [=](T* x, U y, auto mem_ord) { return eastl::atomic_raw_and_fetch(x, y, mem_ord); };
	auto andFetchVerifyFn = [=](T& x, U y)
	{
		x &= y;
		return x;
	};

	auto fetchOrAtomicFn = [=](T* x, U y, auto mem_ord) { return eastl::atomic_raw_fetch_or(x, y, mem_ord); };
	auto fetchOrVerifyFn = [=](T& x, U y)
	{
		T ret = x;
		x |= y;
		return ret;
	};

	auto orFetchAtomicFn = [=](T* x, U y, auto mem_ord) { return eastl::atomic_raw_or_fetch(x, y, mem_ord); };
	auto orFetchVerifyFn = [=](T& x, U y)
	{
		x |= y;
		return x;
	};

	auto fetchXorAtomicFn = [=](T* x, U y, auto mem_ord) { return eastl::atomic_raw_fetch_xor(x, y, mem_ord); };
	auto fetchXorVerifyFn = [=](T& x, U y)
	{
		T ret = x;
		x ^= y;
		return ret;
	};

	auto xorFetchAtomicFn = [=](T* x, U y, auto mem_ord) { return eastl::atomic_raw_xor_fetch(x, y, mem_ord); };
	auto xorFetchVerifyFn = [=](T& x, U y)
	{
		x ^= y;
		return x;
	};


	return TestFetchFunction(initVal, increment, fetchAndAtomicFn, fetchAndVerifyFn) +
	       TestFetchFunction(initVal, increment, andFetchAtomicFn, andFetchVerifyFn) +
	       TestFetchFunction(initVal, increment, fetchOrAtomicFn, fetchOrVerifyFn) +
	       TestFetchFunction(initVal, increment, orFetchAtomicFn, orFetchVerifyFn) +
	       TestFetchFunction(initVal, increment, fetchXorAtomicFn, fetchXorVerifyFn) +
	       TestFetchFunction(initVal, increment, xorFetchAtomicFn, xorFetchVerifyFn);
}

template <class T>
int TestAtomicIntAPI()
{
	int nError{};

	nError += TestAtomicLoadAPI(static_cast<T>(10), static_cast<T>(5));
	nError += TestAtomicStoreAPI(static_cast<T>(10), static_cast<T>(5));
	nError += TestAtomicExchangeAPI(static_cast<T>(10), static_cast<T>(5));
	nError += TestAtomicCmpxchgAPI(static_cast<T>(10), static_cast<T>(5));
	nError += TestAtomicFetchCommonAPI(static_cast<T>(10), static_cast<T>(3));
	nError += TestAtomicFetchIntegralAPI(static_cast<T>(10), static_cast<T>(7));

	return nError;
}


int TestAtomicAPIForBool()
{
	int nError{};

	nError += TestAtomicLoadAPI(true, false);
	nError += TestAtomicStoreAPI(true, false);
	nError += TestAtomicExchangeAPI(true, false);
	nError += TestAtomicCmpxchgAPI(true, false);

	return nError;
}

int TestAtomicAPIForInts()
{
	int nError{};

	nError += TestAtomicIntAPI<uint8_t>();
	nError += TestAtomicIntAPI<uint16_t>();
	nError += TestAtomicIntAPI<uint32_t>();
	nError += TestAtomicIntAPI<uint64_t>();

	nError += TestAtomicIntAPI<int8_t>();
	nError += TestAtomicIntAPI<int16_t>();
	nError += TestAtomicIntAPI<int32_t>();
	nError += TestAtomicIntAPI<int64_t>();

	nError += TestAtomicIntAPI<unsigned char>();
	nError += TestAtomicIntAPI<signed char>();
	nError += TestAtomicIntAPI<char>();

	nError += TestAtomicIntAPI<unsigned int>();
	nError += TestAtomicIntAPI<int>();

	nError += TestAtomicIntAPI<unsigned long>();
	nError += TestAtomicIntAPI<long>();

	nError += TestAtomicIntAPI<unsigned long long>();
	nError += TestAtomicIntAPI<long long>();

	nError += TestAtomicIntAPI<size_t>();
	nError += TestAtomicIntAPI<eastl_size_t>();

	nError += TestAtomicIntAPI<ssize_t>();
	nError += TestAtomicIntAPI<eastl_ssize_t>();

	nError += TestAtomicIntAPI<uintptr_t>();

	return nError;
}

template <class T>
int TestAtomicPtrAPI()
{
	T array[15]{};

	int nError{};

	nError += TestAtomicLoadAPI(array + 5, array + 3);
	nError += TestAtomicStoreAPI(array + 5, array + 3);
	nError += TestAtomicExchangeAPI(array + 5, array + 3);
	nError += TestAtomicCmpxchgAPI(array + 5, array + 3);
	nError += TestAtomicFetchCommonAPI(array + 5, 3);

	nError += TestAtomicLoadAPIImpl<eastl::internal::memory_order_read_depends_s>::Run(array + 5, array + 3);

	return nError;
}

int TestAtomicAPIForPtrs()
{

	int nError{};

	struct S
	{
		uint64_t x;
		uint64_t y;
	};

	nError += TestAtomicPtrAPI<int32_t>();
	nError += TestAtomicPtrAPI<int64_t>();
	nError += TestAtomicPtrAPI<S>();

	// Also test pointer to pointer:
	nError += TestAtomicPtrAPI<S*>();

	return nError;
}


int TestAtomicRaw()
{
	int nError{};

	nError += TestAtomicAPIForBool();
	nError += TestAtomicAPIForInts();
	nError += TestAtomicAPIForPtrs();

	return nError;
}
