/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EASTL/segmented_vector.h>
#include <EASTL/list.h>

// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::segmented_vector<bool, 16>;
template class eastl::segmented_vector<int, 16>;
template class eastl::segmented_vector<Align64, 16>;
template class eastl::segmented_vector<TestObject, 16>;


int TestSegmentedVector()
{
	int nErrorCount = 0;

	TestObject::Reset();

	{
		eastl::segmented_vector<int, 8> sv;
		sv.push_back(0);
		sv.push_back(1);
		sv.push_back(2);
		sv.push_back(3);

		{
			auto i = sv.begin();
			EATEST_VERIFY(*i == 0);
			EATEST_VERIFY(*i++ == 0);
			EATEST_VERIFY(*i++ == 1);
			EATEST_VERIFY(*i++ == 2);
			EATEST_VERIFY(*i++ == 3);
		}

		{
			auto i = sv.begin();
			EATEST_VERIFY(*i == 0);
			EATEST_VERIFY(*(++i) == 1);
			EATEST_VERIFY(*(++i) == 2);
			EATEST_VERIFY(*(++i) == 3);
		}


		{
			for (int i = 4; i < 100; ++i)
			{
				sv.push_back(i);
			}

			EATEST_VERIFY(sv.size() == 100);
			// the capacity is always a multiple of the segment count,
			// in this case it should be the smallest multiple of 8
			// which is >= 100, that number is 104.
			EATEST_VERIFY(sv.capacity() == 104);

			// multi-segment iteration.
			int i = 0;
			for (auto it = sv.begin(); it != sv.end(); ++it, ++i)
			{
				EATEST_VERIFY(*it == i);
			}
		}

		{
			// pop_back() 25 items.
			for (int i = 0; i < 25; ++i)
			{
				sv.pop_back();
			}

			// started with 100 and then popped 25.
			EATEST_VERIFY(sv.size() == 75);

			int i = 0;
			for (auto it = sv.begin(); it != sv.end(); ++it, ++i)
			{
				EATEST_VERIFY(*it == i);
			}

			// capacity is unchanged, this is the same as a normal vector.
			EATEST_VERIFY(sv.capacity() == 104);
		}

		{
			sv.shrink_to_fit();
			// the smallest multiple of 8 which is  >= 75 is 80.
			EATEST_VERIFY(sv.capacity() == 80);
		}

		// Copy/move assignment/construction:
		{
			// copy construction:
			eastl::segmented_vector<int, 8> other = sv;

			// We'll use this down below.
			eastl::segmented_vector<int, 8> other2 = sv;

			// check other has the right things in it.
			int i = 0;
			for (auto it = other.begin(); it != other.end(); ++it, ++i)
			{
				EATEST_VERIFY(*it == i);
			}
			EATEST_VERIFY(other.size() == 75);

			// check that sv is unchanged
			i = 0;
			for (auto it = sv.begin(); it != sv.end(); ++it, ++i)
			{
				EATEST_VERIFY(*it == i);
			}
			EATEST_VERIFY(sv.size() == 75);

			// fill other back to 100
			for (int j = 75; j < 100; ++j)
			{
				other.push_back(j);
			}

			// test copy assignment (growing)
			sv = other;
			i = 0;
			for (auto it = sv.begin(); it != sv.end(); ++it, ++i)
			{
				EATEST_VERIFY(*it == i);
			}
			EATEST_VERIFY(sv.size() == 100);

			// check other is unchanged.
			i = 0;
			for (auto it = other.begin(); it != other.end(); ++it, ++i)
			{
				EATEST_VERIFY(*it == i);
			}
			EATEST_VERIFY(other.size() == 100);

			// make other smaller.
			other.clear();

			// fill it with 10 multiples of 10
			for (int j = 0; j < 10; ++j)
			{
				other.push_back(10 * j);
			}

			// copy assignment (shrinking)
			sv = other;
			i = 0;
			for (auto it = sv.begin(); it != sv.end(); ++it, ++i)
			{
				EATEST_VERIFY(*it == i * 10);
			}
			EATEST_VERIFY(sv.size() == 10);


			// move assignment (growing)
			sv = eastl::move(other2);
			i = 0;
			for (auto it = sv.begin(); it != sv.end(); ++it, ++i)
			{
				EATEST_VERIFY(*it == i);
			}
			EATEST_VERIFY(sv.size() == 75);

			// this behavior need not be set in stone, we could decide
			// the moved from object is in a different state, this is
			// just the current behavior.
			EATEST_VERIFY(other2.empty());

			// move assignment (shrinking)
			sv = eastl::move(other);
			i = 0;
			for (auto it = sv.begin(); it != sv.end(); ++it, ++i)
			{
				EATEST_VERIFY(*it == i * 10);
			}
			EATEST_VERIFY(sv.size() == 10);

			// this behavior need not be set in stone, we could decide
			// the moved from object is in a different state, this is
			// just the current behavior.
			EATEST_VERIFY(other.empty());

			// move construct
			eastl::segmented_vector<int, 8> other3 = eastl::move(sv);
			i = 0;
			for (auto it = other3.begin(); it != other3.end(); ++it, ++i)
			{
				EATEST_VERIFY(*it == i * 10);
			}
			EATEST_VERIFY(other3.size() == 10);

			// this behavior need not be set in stone, we could decide
			// the moved from object is in a different state, this is
			// just the current behavior.
			EATEST_VERIFY(sv.empty());

			// TODO: test move/copy assign with stateful allocators
			// which compare different.
		}

		{
			EATEST_VERIFY(sv.empty());
			// fill up sv with 100 items.
			for (int i = 0; i < 100; ++i)
			{
				sv.push_back(i);
			}
			EATEST_VERIFY(sv.capacity() == 104);

			// check we can clear() and retain capacity.
			sv.clear();
			EATEST_VERIFY(sv.size() == 0);
			EATEST_VERIFY(sv.capacity() == 104);
		}
	}

	{
		// resize/reserve
		eastl::segmented_vector<int, 10> sv;
		sv.reserve(27);
		EATEST_VERIFY(sv.capacity() >= 27);

		sv.resize(55);
		EATEST_VERIFY(sv.size() == 55);
		for (auto i : sv)
		{
			EATEST_VERIFY(i == 0);
		}

		sv.resize(100, 5);
		EATEST_VERIFY(sv.size() == 100);
		int i = 0;
		for (auto it = sv.begin(); it != sv.end(); ++it, ++i)
		{
			EATEST_VERIFY(*it == (i < 55) ? 0 : 5);
		}

		sv.resize(10);
		EATEST_VERIFY(sv.size() == 10);
	}

	{
		// emplace_back
		TestObject::Reset();
		eastl::segmented_vector<TestObject, 8> sv;
		for (int i = 0; i < 25; ++i)
		{
			sv.emplace_back(i);
		}

		{
			int i = 0;
			for (auto it = sv.begin(); it != sv.end(); ++it, ++i)
			{
				EATEST_VERIFY(it->mX == i);
			}
		}

		// no copies are made when we emplace_back
		EATEST_VERIFY(sv.size() == 25);
		EATEST_VERIFY(TestObject::sTOCopyCtorCount == 0);
		EATEST_VERIFY(TestObject::sTOCopyAssignCount == 0);

		// move construction should make no copies.
		eastl::segmented_vector<TestObject, 8> sv2 = eastl::move(sv);
		EATEST_VERIFY(sv2.size() == 25);
		EATEST_VERIFY(TestObject::sTOCopyCtorCount == 0);
		EATEST_VERIFY(TestObject::sTOCopyAssignCount == 0);

		// no copies on move assignment either
		sv = eastl::move(sv2);
		EATEST_VERIFY(sv.size() == 25);
		EATEST_VERIFY(TestObject::sTOCopyCtorCount == 0);
		EATEST_VERIFY(TestObject::sTOCopyAssignCount == 0);
	}

	{
		// Construct segmented_vectors of different types.
		eastl::segmented_vector<int, 8> vectorOfInt;
		eastl::segmented_vector<TestObject, 8> vectorOfTO;
		eastl::segmented_vector<eastl::list<TestObject>, 8> vectorOfListOfTO;

		EATEST_VERIFY(vectorOfInt.empty());
		EATEST_VERIFY(vectorOfTO.empty());
		EATEST_VERIFY(vectorOfListOfTO.empty());
	}

	{
		// Support for non-default constructible types.
		struct S
		{
			int mX;
			explicit S(int i) : mX{i} {}
		};
		eastl::segmented_vector<S, 4> sv;

		sv.push_back(S{0});
		sv.push_back(S{1});
		sv.push_back(S{2});
		sv.push_back(S{3});
		sv.push_back(S{4});
		sv.emplace_back(S{5});
		sv.emplace_back(S{6});
		sv.emplace_back(S{7});
		sv.emplace_back(S{8});
		sv.emplace_back(S{9});


		EATEST_VERIFY(sv.size() == 10);

		int i = 0;
		for (auto it = sv.begin(); it != sv.end(); ++it, ++i)
		{
			EATEST_VERIFY(it->mX == i);
		}
	}

	{
		// Test basic segmented_vector operations.
		eastl::segmented_vector<int, 4> vectorOfInt;

		vectorOfInt.push_back(42);
		EATEST_VERIFY(vectorOfInt.size() == 1);
		EATEST_VERIFY(vectorOfInt.segment_count() == 1);
		EATEST_VERIFY(vectorOfInt.empty() == false);

		vectorOfInt.push_back(43);
		vectorOfInt.push_back(44);
		vectorOfInt.push_back(45);
		vectorOfInt.push_back(46);
		EATEST_VERIFY(vectorOfInt.size() == 5);
		EATEST_VERIFY(vectorOfInt.segment_count() == 2);

		EATEST_VERIFY(vectorOfInt.front() == 42);
		EATEST_VERIFY(vectorOfInt.back() == 46);

		vectorOfInt.pop_back();
		EATEST_VERIFY(vectorOfInt.size() == 4);
		EATEST_VERIFY(vectorOfInt.segment_count() == 1);

		vectorOfInt.clear();
		EATEST_VERIFY(vectorOfInt.empty());
		EATEST_VERIFY(vectorOfInt.size() == 0);
		EATEST_VERIFY(vectorOfInt.segment_count() == 0);
	}

	{
		// global operators (==, !=, <, etc.)
		eastl::segmented_vector<int, 4> sv1;
		eastl::segmented_vector<int, 4> sv2;

		for (auto i = 0; i < 10; ++i)
		{
			sv1.push_back(i);
			sv2.push_back(i);
		}

		EATEST_VERIFY((sv1 == sv2));
		EATEST_VERIFY(!(sv1 != sv2));
		EATEST_VERIFY((sv1 <= sv2));
		EATEST_VERIFY((sv1 >= sv2));
		EATEST_VERIFY(!(sv1 < sv2));
		EATEST_VERIFY(!(sv1 > sv2));

		sv1.push_back(100);  // Make sv1 less than sv2.
		sv2.push_back(101);

		EATEST_VERIFY(!(sv1 == sv2));
		EATEST_VERIFY((sv1 != sv2));
		EATEST_VERIFY((sv1 <= sv2));
		EATEST_VERIFY(!(sv1 >= sv2));
		EATEST_VERIFY((sv1 < sv2));
		EATEST_VERIFY(!(sv1 > sv2));
	}

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)

	{ // Test <=>
		eastl::segmented_vector<int, 4> sv1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		eastl::segmented_vector<int, 4> sv2 = { 9, 8, 7, 6, 5, 4, 3, 2, 1 };
		eastl::segmented_vector<int, 4> sv3 = { 1, 2, 3, 4, 5 };
		eastl::segmented_vector<int, 4> sv4 = { 10 };

		EATEST_VERIFY(sv1 != sv2);
		EATEST_VERIFY(sv1 < sv2);
		EATEST_VERIFY(sv1 != sv3);
		EATEST_VERIFY(sv1 > sv3);
		EATEST_VERIFY(sv4 > sv1);
		EATEST_VERIFY(sv4 > sv2);
		EATEST_VERIFY(sv4 > sv3);

		EATEST_VERIFY((sv1 <=> sv2) != 0);
		EATEST_VERIFY((sv1 <=> sv2) < 0);
		EATEST_VERIFY((sv1 <=> sv3) != 0);
		EATEST_VERIFY((sv1 <=> sv3) > 0);
		EATEST_VERIFY((sv4 <=> sv1) > 0);
		EATEST_VERIFY((sv4 <=> sv2) > 0);
		EATEST_VERIFY((sv4 <=> sv3) > 0);
	}
#endif

	return nErrorCount;
}
