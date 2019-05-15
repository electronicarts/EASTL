/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EASTL/bonus/lru_cache.h>
#include <EASTL/unique_ptr.h>

namespace TestLruCacheInternal
{
	struct Foo
	{
		static int count;

		Foo()
			: a(count++)
			, b(count++)
		{ }

		Foo(int x, int y) : a(x), b(y) {}

		int a;
		int b;

		bool operator==(const Foo &other)
		{
			return this->a == other.a && this->b == other.b;
		}
	};

	int Foo::count = 0;

	class FooCreator
	{
	public:
		FooCreator() : mFooCreatedCount(0) {}

		Foo	*Create()
		{
			mFooCreatedCount++;
			return new Foo();
		}

		void Destroy(Foo *f)
		{
			delete f;
			mFooCreatedCount--;
		}

		int mFooCreatedCount;
	};
}


int TestLruCache()
{
	int nErrorCount = 0;

	// Test simple situation
	{
		using namespace TestLruCacheInternal;

		eastl::lru_cache<int, Foo> lruCache(3);

		// Empty state
		EATEST_VERIFY(lruCache.contains(1) == false);
		EATEST_VERIFY(lruCache.size() == 0);
		EATEST_VERIFY(lruCache.empty() == true);
		EATEST_VERIFY(lruCache.capacity() == 3);
		EATEST_VERIFY(lruCache.at(1).has_value() == false);

		// Auto create with get call
		EATEST_VERIFY(lruCache[0].a == 0);
		EATEST_VERIFY(lruCache[0].b == 1);
		EATEST_VERIFY(lruCache.contains(1) == false);
		EATEST_VERIFY(lruCache.contains(0) == true);
		EATEST_VERIFY(lruCache.size() == 1);
		EATEST_VERIFY(lruCache.empty() == false);
		EATEST_VERIFY(lruCache.capacity() == 3);

		// Fill structure up to 2 more entries to fill out, also test at()
		lruCache.insert(1, Foo(2, 3));
		EATEST_VERIFY(lruCache.at(1).value().a == 2);
		EATEST_VERIFY(lruCache.at(1).value().b == 3);
		EATEST_VERIFY(lruCache.contains(0) == true);
		EATEST_VERIFY(lruCache.contains(1) == true);
		EATEST_VERIFY(lruCache.contains(2) == false);
		EATEST_VERIFY(lruCache.contains(3) == false);
		EATEST_VERIFY(lruCache.size() == 2);
		EATEST_VERIFY(lruCache.empty() == false);
		EATEST_VERIFY(lruCache.capacity() == 3);

		lruCache.insert(2, Foo(4, 5));
		EATEST_VERIFY(lruCache[2].a == 4);
		EATEST_VERIFY(lruCache[2].b == 5);
		EATEST_VERIFY(lruCache.contains(0) == true);
		EATEST_VERIFY(lruCache.contains(1) == true);
		EATEST_VERIFY(lruCache.contains(2) == true);
		EATEST_VERIFY(lruCache.contains(3) == false);
		EATEST_VERIFY(lruCache.size() == 3);
		EATEST_VERIFY(lruCache.empty() == false);
		EATEST_VERIFY(lruCache.capacity() == 3);

		// Add another entry, at this point 0 is the oldest, so it should be pulled
		lruCache.insert(3, Foo(6, 7));
		EATEST_VERIFY(lruCache[3].a == 6);
		EATEST_VERIFY(lruCache[3].b == 7);
		EATEST_VERIFY(lruCache.contains(0) == false);
		EATEST_VERIFY(lruCache.contains(1) == true);
		EATEST_VERIFY(lruCache.contains(2) == true);
		EATEST_VERIFY(lruCache.contains(3) == true);
		EATEST_VERIFY(lruCache.size() == 3);
		EATEST_VERIFY(lruCache.empty() == false);
		EATEST_VERIFY(lruCache.capacity() == 3);

		// Touch the now oldest 1 key
		EATEST_VERIFY(lruCache.touch(1) == true);

		// Add another entry, this will be #4 but since 1 was touched, 2 is now the oldest
		lruCache.insert(4, Foo(8, 9));
		EATEST_VERIFY(lruCache[4].a == 8);
		EATEST_VERIFY(lruCache[4].b == 9);
		EATEST_VERIFY(lruCache.contains(0) == false);
		EATEST_VERIFY(lruCache.contains(1) == true);
		EATEST_VERIFY(lruCache.contains(2) == false);
		EATEST_VERIFY(lruCache.contains(3) == true);
		EATEST_VERIFY(lruCache.contains(4) == true);
		EATEST_VERIFY(lruCache.size() == 3);
		EATEST_VERIFY(lruCache.empty() == false);
		EATEST_VERIFY(lruCache.capacity() == 3);

		// Test resize down
		EATEST_VERIFY(lruCache.touch(3) == true);	// Let's make some key in the middle the most recent
		lruCache.resize(1);	// Resize down to 1 entry in the cache
		EATEST_VERIFY(lruCache.contains(0) == false);
		EATEST_VERIFY(lruCache.contains(1) == false);
		EATEST_VERIFY(lruCache.contains(2) == false);
		EATEST_VERIFY(lruCache.contains(3) == true);
		EATEST_VERIFY(lruCache.contains(4) == false);
		EATEST_VERIFY(lruCache.size() == 1);
		EATEST_VERIFY(lruCache.empty() == false);
		EATEST_VERIFY(lruCache.capacity() == 1);

		// Let's resize up to a size of 5 now
		lruCache.resize(5);
		EATEST_VERIFY(lruCache.contains(0) == false);
		EATEST_VERIFY(lruCache.contains(1) == false);
		EATEST_VERIFY(lruCache.contains(2) == false);
		EATEST_VERIFY(lruCache.contains(3) == true);
		EATEST_VERIFY(lruCache.contains(4) == false);
		EATEST_VERIFY(lruCache.size() == 1);
		EATEST_VERIFY(lruCache.empty() == false);
		EATEST_VERIFY(lruCache.capacity() == 5);

		// Let's try updating
		lruCache.assign(3, Foo(0, 0));
		EATEST_VERIFY(lruCache[3] == Foo(0, 0));
		EATEST_VERIFY(lruCache.contains(0) == false);
		EATEST_VERIFY(lruCache.contains(1) == false);
		EATEST_VERIFY(lruCache.contains(2) == false);
		EATEST_VERIFY(lruCache.contains(3) == true);
		EATEST_VERIFY(lruCache.contains(4) == false);
		EATEST_VERIFY(lruCache.size() == 1);
		EATEST_VERIFY(lruCache.empty() == false);
		EATEST_VERIFY(lruCache.capacity() == 5);

		// add or update existing
		lruCache.insert_or_assign(3, Foo(1, 1));
		EATEST_VERIFY(lruCache[3] == Foo(1, 1));
		EATEST_VERIFY(lruCache.contains(0) == false);
		EATEST_VERIFY(lruCache.contains(1) == false);
		EATEST_VERIFY(lruCache.contains(2) == false);
		EATEST_VERIFY(lruCache.contains(3) == true);
		EATEST_VERIFY(lruCache.contains(4) == false);
		EATEST_VERIFY(lruCache.size() == 1);
		EATEST_VERIFY(lruCache.empty() == false);
		EATEST_VERIFY(lruCache.capacity() == 5);
		
		// Add or update a new entry
		lruCache.insert_or_assign(25, Foo(2, 2));
		EATEST_VERIFY(lruCache[3] == Foo(1, 1));
		EATEST_VERIFY(lruCache[25] == Foo(2, 2));
		EATEST_VERIFY(lruCache.contains(0) == false);
		EATEST_VERIFY(lruCache.contains(1) == false);
		EATEST_VERIFY(lruCache.contains(2) == false);
		EATEST_VERIFY(lruCache.contains(3) == true);
		EATEST_VERIFY(lruCache.contains(4) == false);
		EATEST_VERIFY(lruCache.contains(25) == true);
		EATEST_VERIFY(lruCache.size() == 2);
		EATEST_VERIFY(lruCache.empty() == false);
		EATEST_VERIFY(lruCache.capacity() == 5);

		// clear everything
		lruCache.clear();
		EATEST_VERIFY(lruCache.size() == 0);
		EATEST_VERIFY(lruCache.empty() == true);
		EATEST_VERIFY(lruCache.capacity() == 5);
		EATEST_VERIFY(lruCache.contains(3) == false);

		// test unilateral reset
		lruCache[1] = Foo(1, 2);
		lruCache.reset_lose_memory();
		EATEST_VERIFY(lruCache.size() == 0);
	}

	// Test more advanced creation / deletion via callbacks
	{
		using namespace TestLruCacheInternal;

		FooCreator fooCreator;

		auto createCallback = [&fooCreator](int) { return fooCreator.Create(); };
		auto deleteCallback = [&fooCreator](Foo *f) { fooCreator.Destroy(f); };

		eastl::lru_cache<int, Foo*> lruCache(3, EASTLAllocatorType("eastl lru_cache"), createCallback, deleteCallback);

		lruCache[1];
		EATEST_VERIFY(fooCreator.mFooCreatedCount == 1);
		EATEST_VERIFY(lruCache.size() == 1);
		EATEST_VERIFY(lruCache.empty() == false);
		EATEST_VERIFY(lruCache.capacity() == 3);
		EATEST_VERIFY(lruCache.contains(1) == true);
		EATEST_VERIFY(lruCache.contains(2) == false);

		lruCache[2];
		EATEST_VERIFY(fooCreator.mFooCreatedCount == 2);
		EATEST_VERIFY(lruCache.size() == 2);
		EATEST_VERIFY(lruCache.empty() == false);
		EATEST_VERIFY(lruCache.capacity() == 3);
		EATEST_VERIFY(lruCache.contains(1) == true);
		EATEST_VERIFY(lruCache.contains(2) == true);

		// Update 2, which should delete the existing entry
		{
			auto f = fooCreator.Create();
			EATEST_VERIFY(fooCreator.mFooCreatedCount == 3);
			f->a = 20;
			f->b = 21;
			lruCache.assign(2, f);
			EATEST_VERIFY(fooCreator.mFooCreatedCount == 2);
			EATEST_VERIFY(lruCache.size() == 2);
			EATEST_VERIFY(lruCache.empty() == false);
			EATEST_VERIFY(lruCache.capacity() == 3);
			EATEST_VERIFY(lruCache.contains(1) == true);
			EATEST_VERIFY(lruCache.contains(2) == true);
			EATEST_VERIFY(lruCache[2]->a == 20);
			EATEST_VERIFY(lruCache[2]->b == 21);
		}

		lruCache.erase(2);
		EATEST_VERIFY(fooCreator.mFooCreatedCount == 1);
		EATEST_VERIFY(lruCache.size() == 1);
		EATEST_VERIFY(lruCache.empty() == false);
		EATEST_VERIFY(lruCache.capacity() == 3);
		EATEST_VERIFY(lruCache.contains(1) == true);
		EATEST_VERIFY(lruCache.contains(2) == false);

		lruCache.erase(1);
		EATEST_VERIFY(fooCreator.mFooCreatedCount == 0);
		EATEST_VERIFY(lruCache.size() == 0);
		EATEST_VERIFY(lruCache.empty() == true);
		EATEST_VERIFY(lruCache.capacity() == 3);
		EATEST_VERIFY(lruCache.contains(1) == false);
		EATEST_VERIFY(lruCache.contains(2) == false);

		// Test insert_or_assign
		{
			auto f = fooCreator.Create();
			f->a = 22;
			f->b = 30;
			EATEST_VERIFY(fooCreator.mFooCreatedCount == 1);

			lruCache.insert_or_assign(7, f);
			EATEST_VERIFY(lruCache.size() == 1);
			EATEST_VERIFY(lruCache.empty() == false);
			EATEST_VERIFY(lruCache.capacity() == 3);
			EATEST_VERIFY(lruCache.contains(1) == false);
			EATEST_VERIFY(lruCache.contains(2) == false);
			EATEST_VERIFY(lruCache.contains(7) == true);
			EATEST_VERIFY(lruCache.erase(7) == true);
			EATEST_VERIFY(fooCreator.mFooCreatedCount == 0);
		}
	}

	return nErrorCount;
}
