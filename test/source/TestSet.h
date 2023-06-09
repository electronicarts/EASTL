/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/vector.h>
#include <EASTL/algorithm.h>
#include <EASTL/type_traits.h>
#include <EASTL/scoped_ptr.h>
#include <EASTL/random.h>

EA_DISABLE_ALL_VC_WARNINGS()
#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	#include <algorithm>
#endif
EA_RESTORE_ALL_VC_WARNINGS()

#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY


///////////////////////////////////////////////////////////////////////////////
// TestSetConstruction
//
// This test compares eastl::set/multiset to std::set/multiset. It could possibly
// work for comparing eastl::hash_set to C++11 std::unordered_set, but we would 
// rather move towards making this test be independent of any std comparisons.
//
// Requires a container that can hold at least 1000 items.
//
template <typename T1, typename T2, bool bMultiset>
int TestSetConstruction()
{
	int nErrorCount = 0;

	TestObject::Reset();

	{
		eastl::scoped_ptr<T1> pt1A(new T1); // We use a pointers instead of concrete object because it's size may be huge.
		eastl::scoped_ptr<T2> pt2A(new T2);
		T1& t1A = *pt1A;
		T2& t2A = *pt2A;
		nErrorCount += CompareContainers(t1A, t2A, "Set ctor", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());
		VERIFY(t1A.validate());


		eastl::scoped_ptr<T1> pt1B(new T1);
		eastl::scoped_ptr<T2> pt2B(new T2);
		T1& t1B = *pt1B;
		T2& t2B = *pt2B;
		nErrorCount += CompareContainers(t1B, t2B, "Set ctor", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());


		eastl::scoped_ptr<T1> pt1C(new T1);
		eastl::scoped_ptr<T2> pt2C(new T2);
		T1& t1C = *pt1C;
		T2& t2C = *pt2C;
		for(int i = 0; i < 1000; i++)
		{
			t1C.insert(typename T1::value_type(typename T1::value_type(i)));
			t2C.insert(typename T2::value_type(typename T2::value_type(i)));
			VERIFY(t1C.validate());
			nErrorCount += CompareContainers(t1C, t2C, "Set insert", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());
		}


		eastl::scoped_ptr<T1> pt1D(new T1);
		eastl::scoped_ptr<T2> pt2D(new T2);
		T1& t1D = *pt1D;
		T2& t2D = *pt2D;
		nErrorCount += CompareContainers(t1D, t2D, "Set ctor", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());


		eastl::scoped_ptr<T1> pt1E(new T1(t1C));
		eastl::scoped_ptr<T2> pt2E(new T2(t2C));
		T1& t1E = *pt1E;
		T2& t2E = *pt2E;
		VERIFY(t1E.validate());
		nErrorCount += CompareContainers(t1E, t2E, "Set ctor", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());


		eastl::scoped_ptr<T1> pt1F(new T1(t1C.begin(), t1C.end()));
		eastl::scoped_ptr<T2> pt2F(new T2(t2C.begin(), t2C.end()));
		T1& t1F = *pt1F;
		T2& t2F = *pt2F;
		VERIFY(t1F.validate());
		nErrorCount += CompareContainers(t1F, t2F, "Set ctor", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());


		// operator=
		t1E = t1D;
		t2E = t2D;
		nErrorCount += CompareContainers(t1D, t2D, "Set operator=", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());
		nErrorCount += CompareContainers(t1E, t2E, "Set operator=", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());


		// operator=(set&&)
		// We test just the EASTL container here.
		eastl::scoped_ptr<T1> pT1P(new T1); // We use a pointers instead of concrete object because it's size may be huge.
		eastl::scoped_ptr<T1> pT1Q(new T1);
		T1& t1P = *pT1P;
		T1& t1Q = *pT1Q;

		typename T1::value_type v10(0);
		typename T1::value_type v11(1);
		typename T1::value_type v12(2);
		typename T1::value_type v13(3);
		typename T1::value_type v14(4);
		typename T1::value_type v15(5);

		t1P.insert(v10);
		t1P.insert(v11);
		t1P.insert(v12);

		t1Q.insert(v13);
		t1Q.insert(v14);
		t1Q.insert(v15);

		t1Q = eastl::move(t1P); // We are effectively requesting to swap t1A with t1B.
	  //EATEST_VERIFY((t1P.size() == 3) && (t1P.find(v13) != t1P.end()) && (t1P.find(v14) != t1P.end()) && (t1P.find(v15) != t1P.end()));  // Currently operator=(this_type&& x) clears x instead of swapping with it.


		// swap
		t1E.swap(t1D);
		t2E.swap(t2D);
		VERIFY(t1D.validate());
		VERIFY(t1E.validate());
		nErrorCount += CompareContainers(t1D, t2D, "Set swap", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());
		nErrorCount += CompareContainers(t1E, t2E, "Set swap", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());


		// eastl::swap
		eastl::swap(t1E, t1D);
		  std::swap(t2E, t2D);
		VERIFY(t1D.validate());
		VERIFY(t1E.validate());
		nErrorCount += CompareContainers(t1D, t2D, "Global swap", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());
		nErrorCount += CompareContainers(t1E, t2E, "Global swap", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());


		// clear
		t1A.clear();
		t2A.clear();
		VERIFY(t1A.validate());
		nErrorCount += CompareContainers(t1A, t2A, "Set clear", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());

		t1B.clear();
		t2B.clear();
		VERIFY(t1B.validate());
		nErrorCount += CompareContainers(t1B, t2B, "Set clear", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());


		// global operators (==, !=, <, etc.)
		t1A.clear();
		t1B.clear();
							   // Make t1A equal to t1B
		t1A.insert(typename T1::value_type(0));
		t1A.insert(typename T1::value_type(1));
		t1A.insert(typename T1::value_type(2));

		t1B.insert(typename T1::value_type(0));
		t1B.insert(typename T1::value_type(1));
		t1B.insert(typename T1::value_type(2));

		VERIFY( (t1A == t1B));
		VERIFY(!(t1A != t1B));
		VERIFY( (t1A <= t1B));
		VERIFY( (t1A >= t1B));
		VERIFY(!(t1A  < t1B));
		VERIFY(!(t1A  > t1B));
							   // Make t1A less than t1B
		t1A.insert(typename T1::value_type(3));
		t1B.insert(typename T1::value_type(4));

		VERIFY(!(t1A == t1B));
		VERIFY( (t1A != t1B));
		VERIFY( (t1A <= t1B));
		VERIFY(!(t1A >= t1B));
		VERIFY( (t1A  < t1B));
		VERIFY(!(t1A  > t1B));
	}

	VERIFY(TestObject::IsClear());
	TestObject::Reset();

	return nErrorCount;
}




///////////////////////////////////////////////////////////////////////////////
// TestSetMutation
//
// Requires a container that can hold at least 1000 items.
//
EA_DISABLE_VC_WARNING(6262)
template <typename T1, typename T2, bool bMultiset>
int TestSetMutation()
{
	int nErrorCount = 0;

	TestObject::Reset();

	{
		eastl::scoped_ptr<T1> pt1A(new T1); // We use a pointers instead of concrete object because it's size may be huge.
		eastl::scoped_ptr<T2> pt2A(new T2);
		T1& t1A = *pt1A;
		T2& t2A = *pt2A;
		int i, iEnd, p;

		// Set up an array of values to randomize / permute.
		eastl::vector<typename T1::value_type> valueArrayInsert;

		if(gEASTL_TestLevel >= kEASTL_TestLevelLow)
		{
			EASTLTest_Rand rng(EA::UnitTest::GetRandSeed());

			valueArrayInsert.clear();

			for(i = 0; i < 1000; i++)
			{
				valueArrayInsert.push_back(typename T1::value_type(i));

				// Occasionally attempt to duplicate an element, both for set and multiset.
				if(((i + 1) < 1000) && (rng.RandLimit(4) == 0))
				{
					valueArrayInsert.push_back(typename T1::value_type(i));
					i++;
				}
			}

			for(p = 0; p < gEASTL_TestLevel * 100; p++) // For each permutation...
			{
				eastl::random_shuffle(valueArrayInsert.begin(), valueArrayInsert.end(), rng);

				// insert
				for(i = 0, iEnd = (int)valueArrayInsert.size(); i < iEnd; i++)
				{
					typename T1::value_type& k = valueArrayInsert[i];

					t1A.insert(typename T1::value_type(k)); // We expect that both arguments are the same.
					t2A.insert(typename T2::value_type(k));

					VERIFY(t1A.validate());
					nErrorCount += CompareContainers(t1A, t2A, "Set insert", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());
				}


				// reverse iteration
				typename T1::reverse_iterator r1 = t1A.rbegin();
				typename T2::reverse_iterator r2 = t2A.rbegin();

				while(r1 != t1A.rend())
				{
					typename T1::value_type k1 = *r1;
					typename T2::value_type k2 = *r2;
					VERIFY(k1 == k2);
				}


				// erase
				for(i = 0, iEnd = (int)valueArrayInsert.size(); i < iEnd; i++)
				{
					typename T1::value_type& k = valueArrayInsert[i];

					typename T1::size_type n1 = t1A.erase(k);
					typename T2::size_type n2 = t2A.erase(k);

					VERIFY(n1 == n2);
					VERIFY(t1A.validate());
					nErrorCount += CompareContainers(t1A, t2A, "Set erase", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());
				}

				VERIFY((TestObject::sTOCount == 0) || (TestObject::sTOCount == (int64_t)valueArrayInsert.size())); // This test will only have meaning when T1 contains TestObject.
			}
		}


		VERIFY(TestObject::IsClear());
		TestObject::Reset();


		// Possibly do extended testing.
		if(gEASTL_TestLevel > 6)
		{
			valueArrayInsert.clear();

			for(i = 0; i < 9; i++) // Much more than this count would take too long to test all permutations.
				valueArrayInsert.push_back(typename T1::value_type(i));

			// Insert these values into the set in every existing permutation.
			for(p = 0; std::next_permutation(valueArrayInsert.begin(), valueArrayInsert.end()); p++) // For each permutation...
			{
				for(i = 0, iEnd = (int)valueArrayInsert.size(); i < iEnd; i++)
				{
					typename T1::value_type& k = valueArrayInsert[i];

					t1A.insert(typename T1::value_type(k)); // We expect that both arguments are the same.
					t2A.insert(typename T2::value_type(k));

					VERIFY(t1A.validate());
					nErrorCount += CompareContainers(t1A, t2A, "Set insert", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());
				}

				for(i = 0, iEnd = (int)valueArrayInsert.size(); i < iEnd; i++)
				{
					typename T1::value_type& k = valueArrayInsert[i];

					t1A.erase(k);
					t2A.erase(k);

					VERIFY(t1A.validate());
					nErrorCount += CompareContainers(t1A, t2A, "Set erase", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());
				}

				VERIFY((TestObject::sTOCount == 0) || (TestObject::sTOCount == (int64_t)valueArrayInsert.size())); // This test will only have meaning when T1 contains TestObject.
			}
		}
	}


	VERIFY(TestObject::IsClear());
	TestObject::Reset();


	{  // Other insert and erase operations

		eastl::scoped_ptr<T1> pt1A(new T1); // We use a pointers instead of concrete object because it's size may be huge.
		eastl::scoped_ptr<T2> pt2A(new T2);
		T1& t1A = *pt1A;
		T2& t2A = *pt2A;
		int i;

		// Set up an array of values to randomize / permute.
		eastl::vector<typename T1::value_type> valueArrayInsert1;
		eastl::vector<typename T2::value_type> valueArrayInsert2;

		EASTLTest_Rand rng(EA::UnitTest::GetRandSeed());

		for(i = 0; i < 100; i++)
		{
			valueArrayInsert1.push_back(typename T1::value_type(i));
			valueArrayInsert2.push_back(typename T2::value_type(i));

			if(rng.RandLimit(3) == 0)
			{
				valueArrayInsert1.push_back(typename T1::value_type(i));
				valueArrayInsert2.push_back(typename T2::value_type(i));
			}
		}


		// insert(InputIterator first, InputIterator last)
		t1A.insert(valueArrayInsert1.begin(), valueArrayInsert1.end());
		t2A.insert(valueArrayInsert2.begin(), valueArrayInsert2.end());
		VERIFY(t1A.validate());
		nErrorCount += CompareContainers(t1A, t2A, "Set insert", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());


		// iterator insert(iterator position, const value_type& value);
		//
		// If bMultiset == true, then the insertions below should fail due to the 
		// item being present. But they should return the correct iterator value.
		typename T1::iterator it1 = t1A.insert(t1A.find(typename T1::value_type(2)), typename T1::value_type(1));
		typename T2::iterator it2 = t2A.insert(t2A.find(typename T2::value_type(2)), typename T2::value_type(1));
		VERIFY(t1A.validate());
		VERIFY(*it1 == typename T1::value_type(1));
		VERIFY(*it2 == typename T2::value_type(1));
		nErrorCount += CompareContainers(t1A, t2A, "Set insert", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());

		it1 = t1A.insert(t1A.end(), typename T1::value_type(5));
		it2 = t2A.insert(t2A.end(), typename T2::value_type(5));
		VERIFY(t1A.validate());
		VERIFY(*it1 == typename T1::value_type(5));
		VERIFY(*it2 == typename T2::value_type(5));
		nErrorCount += CompareContainers(t1A, t2A, "Set insert", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());

		// Now we remove these items so that the insertions above can succeed.
		t1A.erase(t1A.find(typename T1::value_type(1)));
		t2A.erase(t2A.find(typename T2::value_type(1)));
		it1 = t1A.insert(t1A.find(typename T1::value_type(2)), typename T1::value_type(1));
		it2 = t2A.insert(t2A.find(typename T2::value_type(2)), typename T2::value_type(1));
		VERIFY(t1A.validate());
		VERIFY(*it1 == typename T1::value_type(1));
		VERIFY(*it2 == typename T2::value_type(1));
		nErrorCount += CompareContainers(t1A, t2A, "Set insert", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());

		t1A.erase(t1A.find(typename T1::value_type(5)));
		t2A.erase(t2A.find(typename T2::value_type(5)));
		it1 = t1A.insert(t1A.end(), typename T1::value_type(5));
		it2 = t2A.insert(t2A.end(), typename T2::value_type(5));
		VERIFY(t1A.validate());
		VERIFY(*it1 == typename T1::value_type(5));
		VERIFY(*it2 == typename T2::value_type(5));
		nErrorCount += CompareContainers(t1A, t2A, "Set insert", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());

		// iterator erase(iterator first, iterator last);
		typename T1::iterator it11 = t1A.find(typename T1::value_type(17));
		typename T1::iterator it12 = t1A.find(typename T2::value_type(37));
		t1A.erase(it11, it12);

		typename T2::iterator it21 = t2A.find(typename T1::value_type(17));
		typename T2::iterator it22 = t2A.find(typename T2::value_type(37));
		t2A.erase(it21, it22);

		VERIFY(t1A.validate());
		nErrorCount += CompareContainers(t1A, t2A, "Set erase(first, last)", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());


		// iterator erase(iterator position);
		t1A.erase(t1A.find(typename T1::value_type(60)));
		t2A.erase(t2A.find(typename T1::value_type(60)));
		VERIFY(t1A.validate());
		nErrorCount += CompareContainers(t1A, t2A, "Set erase(first, last)", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());


		// Disabled because this function isn't exposed outside the rbtree yet.
		// void erase(const value_type* first, const value_type* last);
		//typename T1::value_type keyArray1[3] = { typename T1::value_type(70), typename T1::value_type(71), typename T1::value_type(72) };
		//typename T2::value_type keyArray2[3] = { typename T2::value_type(70), typename T2::value_type(71), typename T2::value_type(72) };
		//t1A.erase(keyArray1 + 0, keyArray1 + 3);
		//t2A.erase(keyArray2 + 0, keyArray2 + 3);
		//VERIFY(t1A.validate());
		//nErrorCount += CompareContainers(t1A, t2A, "Set erase(first, last)", eastl::use_self<typename T1::value_type>(), eastl::use_self<typename T2::value_type>());
	}

	{
		// set(std::initializer_list<value_type> ilist, const Compare& compare = Compare(), const allocator_type& allocator = EASTL_MAP_DEFAULT_ALLOCATOR);
		// this_type& operator=(std::initializer_list<T> ilist);
		// void insert(std::initializer_list<value_type> ilist);
		#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
			T1 mySet = { typename T1::value_type(10), typename T1::value_type(11) };
			EATEST_VERIFY(mySet.size() == 2);
			typename T1::iterator it = mySet.begin();
			EATEST_VERIFY(*it == typename T1::value_type(10));
			it = mySet.rbegin().base();
			EATEST_VERIFY(*--it == typename T1::value_type(11));

			mySet = {typename T1::value_type(20), typename T1::value_type(21) };
			EATEST_VERIFY(mySet.size() == 2);
			EATEST_VERIFY(*mySet.begin() == typename T1::value_type(20));
			it = mySet.rbegin().base();
			EATEST_VERIFY(*--it == typename T1::value_type(21));

			mySet.insert({ typename T1::value_type(40), typename T1::value_type(41) });
			EATEST_VERIFY(mySet.size() == 4);
			it = mySet.rbegin().base();
			EATEST_VERIFY(*--it == typename T1::value_type(41));
		#endif
	}

	VERIFY(TestObject::IsClear());
	TestObject::Reset();

	return nErrorCount;
}
EA_RESTORE_VC_WARNING()


#endif // EA_COMPILER_NO_STANDARD_CPP_LIBRARY




template <typename T1>
int TestSetSpecific(T1& /*t1A*/, eastl::false_type) // false_type means this is a map and not a multimap.
{
	return 0;
}


template <typename T1>
int TestSetSpecific(T1& t1A, eastl::true_type) // true_type means this is a multimap and not a map.
{
	int nErrorCount = 0;

	// equal_range_small (multiset only)
	eastl::pair<typename T1::iterator, typename T1::iterator> er = t1A.equal_range_small(typename T1::value_type(499));
	VERIFY(*er.first  == typename T1::value_type(499));
	VERIFY(*er.second == typename T1::value_type(501));

	er = t1A.equal_range_small(typename T1::value_type(-1));
	VERIFY(er.first == er.second);
	VERIFY(er.first == t1A.begin());

	return nErrorCount;
}


// Just for the purposes of the map::find_as test below, we declare the following.
// The map::find_as function searches a container of X for a type Y, where the user 
// defines the equality of X to Y. The purpose of TSetComparable is to be a generic type Y
// that can be used for any X. We need to make this generic because the whole TestMapSearch
// function below is templated on type T1 and so we don't know what T1 is ahead of time.

template <typename T>
struct TSetComparable
{
	T b;

	TSetComparable() : b() { }
	TSetComparable(const T& a) : b(a){ }
	const TSetComparable& operator=(const T& a) { b = a; return *this; }
	const TSetComparable& operator=(const TSetComparable& x) { b = x.b; return *this; }
	operator const T&() const { return b; }
};


///////////////////////////////////////////////////////////////////////////////
// TestSetSearch
//
// This function is designed to work with set, fixed_set (and not hash containers).
// Requires a container that can hold at least 1000 items.
//
template <typename T1, bool bMultimap>
int TestSetSearch()
{
	int nErrorCount = 0;

	TestObject::Reset();

	{  // Test find, lower_bound, upper_bound, etc..
		eastl::scoped_ptr<T1> pt1A(new T1); // We use a pointers instead of concrete object because it's size may be huge.
		T1& t1A = *pt1A;
		int i, iEnd;
		typename T1::iterator it;

		// Set up an array of values to randomize / permute.
		eastl::vector<typename T1::value_type> valueArrayInsert;

		for(i = 0; i < 1000; i++)
			valueArrayInsert.push_back(typename T1::value_type(i));

		EASTLTest_Rand rng(EA::UnitTest::GetRandSeed());
		eastl::random_shuffle(valueArrayInsert.begin(), valueArrayInsert.end(), rng);


		// insert
		for(i = 0, iEnd = (int)valueArrayInsert.size(); i < iEnd; i++)
		{
			typename T1::value_type k(i);
			t1A.insert(typename T1::value_type(k));

			it = t1A.find(k);
			VERIFY(it != t1A.end());
		}


		// find
		for(i = 0; i < 1000; i++)
		{
			typename T1::value_type k(i);
			it = t1A.find(k);

			VERIFY(it != t1A.end());
			VERIFY(*it == k);
		}

		it = t1A.find(typename T1::value_type(-1));
		VERIFY(it == t1A.end());

		it = t1A.find(typename T1::value_type(1001));
		VERIFY(it == t1A.end());


		// find_as
		typedef TSetComparable<typename T1::key_type> TC;

		// Normally we use find_as to find via a different type, but we can test it here like this.
		for(i = 0; i < 1000; i++)
		{
			TC k = typename T1::key_type(i);
			it = t1A.find_as(k, eastl::less<>());

			VERIFY(it != t1A.end());
			VERIFY(*it == k);
		}

		it = t1A.find_as(TC(typename T1::key_type(-1)), eastl::less<>());
		VERIFY(it == t1A.end());

		it = t1A.find_as(TC(typename T1::key_type(1001)), eastl::less<>());
		VERIFY(it == t1A.end());


		// lower_bound
		it = t1A.lower_bound(typename T1::value_type(0));
		VERIFY(it == t1A.begin());

		it = t1A.lower_bound(typename T1::value_type(-1));
		VERIFY(it == t1A.begin());

		it = t1A.lower_bound(typename T1::value_type(1001));
		VERIFY(it == t1A.end());

		t1A.erase(typename T1::value_type(500));
		it = t1A.lower_bound(typename T1::value_type(500));
		VERIFY(*it  == typename T1::value_type(501));


		// upper_bound
		it = t1A.upper_bound(typename T1::value_type(-1));
		VERIFY(it == t1A.begin());

		it = t1A.upper_bound(typename T1::value_type(499));
		VERIFY(*it  == typename T1::value_type(501));

		it = t1A.upper_bound(typename T1::value_type(-1));
		VERIFY(*it  == typename T1::value_type(0));

		it = t1A.upper_bound(typename T1::value_type(1000));
		VERIFY(it == t1A.end());


		// count
		typename T1::size_type n = t1A.count(typename T1::value_type(-1));
		VERIFY(n == 0);

		n = t1A.count(typename T1::value_type(0));
		VERIFY(n == 1);

		n = t1A.count(typename T1::value_type(500)); // We removed 500 above.
		VERIFY(n == 0);

		n = t1A.count(typename T1::value_type(1001));
		VERIFY(n == 0);


		// equal_range
		eastl::pair<typename T1::iterator, typename T1::iterator> er = t1A.equal_range(typename T1::value_type(200));
		VERIFY(*er.first == typename T1::value_type(200));

		er = t1A.equal_range(typename T1::value_type(499));
		VERIFY(*er.first  == typename T1::value_type(499));
		VERIFY(*er.second == typename T1::value_type(501));

		er = t1A.equal_range(typename T1::value_type(-1));
		VERIFY(er.first == er.second);
		VERIFY(er.first == t1A.begin());


		// Some tests need to be differently between map and multimap.
		nErrorCount += TestSetSpecific(t1A, eastl::integral_constant<bool, bMultimap>());
	}

	VERIFY(TestObject::IsClear());
	TestObject::Reset();

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestSetCpp11
//
// This function is designed to work with set, fixed_set, hash_set, fixed_hash_set
//
template <typename T1>
int TestSetCpp11()
{
	int nErrorCount = 0;

	// template <class... Args>
	// insert_return_type emplace(Args&&... args);
	//
	// template <class... Args> 
	// iterator emplace_hint(const_iterator position, Args&&... args);
	//
	// insert_return_type insert(value_type&& value);
	// iterator insert(const_iterator position, value_type&& value);
	TestObject::Reset();

	typedef T1 TOSet;
	typename TOSet::insert_return_type toSetInsertResult;
	typename TOSet::iterator toSetIterator;

	TOSet      toSet;
	TestObject to0(0);
	TestObject to1(1);

	toSetInsertResult = toSet.emplace(to0);
	EATEST_VERIFY(toSetInsertResult.second == true);
	//EATEST_VERIFY((TestObject::sTOCopyCtorCount == 2) && (TestObject::sTOMoveCtorCount == 1));  // Disabled until we can guarantee its behavior and deal with how it's different between compilers of differing C++11 support.

	toSetInsertResult = toSet.emplace(eastl::move(to1));
	EATEST_VERIFY(toSetInsertResult.second == true);

	// insert_return_type t1A.emplace(value_type&& value);
	TestObject to40(4);
	EATEST_VERIFY(toSet.find(to40) == toSet.end());
	EATEST_VERIFY(to40.mX == 4); // It should change to 0 below during the move swap.
	toSetInsertResult = toSet.emplace(eastl::move(to40));
	EATEST_VERIFY(toSetInsertResult.second == true);
	EATEST_VERIFY(toSet.find(to40) != toSet.end());
	EATEST_VERIFY(to40.mX == 0);

	TestObject to41(4);
	toSetInsertResult = toSet.emplace(eastl::move(to41));
	EATEST_VERIFY(toSetInsertResult.second == false);
	EATEST_VERIFY(toSet.find(to41) != toSet.end());

	// iterator t1A.emplace_hint(const_iterator position, value_type&& value);
	TestObject to50(5);
	toSetInsertResult = toSet.emplace(eastl::move(to50));
	EATEST_VERIFY(toSetInsertResult.second == true);
	EATEST_VERIFY(toSet.find(to50) != toSet.end());

	TestObject to51(5);
	toSetIterator = toSet.emplace_hint(toSetInsertResult.first, eastl::move(to51));
	EATEST_VERIFY(*toSetIterator == TestObject(5));
	EATEST_VERIFY(toSet.find(to51) != toSet.end());

	TestObject to6(6);
	toSetIterator = toSet.emplace_hint(toSet.begin(), eastl::move(to6)); // specify a bad hint. Insertion should still work.
	EATEST_VERIFY(*toSetIterator == TestObject(6));
	EATEST_VERIFY(toSet.find(to6) != toSet.end());
		
	TestObject to2(2);
	EATEST_VERIFY(toSet.find(to2) == toSet.end());
	toSetInsertResult = toSet.emplace(to2);
	EATEST_VERIFY(toSetInsertResult.second == true);
	EATEST_VERIFY(toSet.find(to2) != toSet.end());
	toSetInsertResult = toSet.emplace(to2);
	EATEST_VERIFY(toSetInsertResult.second == false);
	EATEST_VERIFY(toSet.find(to2) != toSet.end());

	// iterator t1A.emplace_hint(const_iterator position, const value_type& value);
	TestObject to70(7);
	toSetInsertResult = toSet.emplace(to70);
	EATEST_VERIFY(toSetInsertResult.second == true);
	EATEST_VERIFY(toSet.find(to70) != toSet.end());

	TestObject to71(7);
	toSetIterator = toSet.emplace_hint(toSetInsertResult.first, to71);
	EATEST_VERIFY(*toSetIterator == to71);
	EATEST_VERIFY(toSet.find(to71) != toSet.end());

	TestObject to8(8);
	toSetIterator = toSet.emplace_hint(toSet.begin(), to8); // specify a bad hint. Insertion should still work.
	EATEST_VERIFY(*toSetIterator == to8);
	EATEST_VERIFY(toSet.find(to8) != toSet.end());

	//pair<iterator,bool> t1A.insert(value_type&& value);
	TestObject to3(3);
	EATEST_VERIFY(toSet.find(to3) == toSet.end());
	toSetInsertResult = toSet.insert(TestObject(to3));
	EATEST_VERIFY(toSetInsertResult.second == true);
	EATEST_VERIFY(toSet.find(to3) != toSet.end());
	toSetInsertResult = toSet.insert(TestObject(to3));
	EATEST_VERIFY(toSetInsertResult.second == false);
	EATEST_VERIFY(toSet.find(to3) != toSet.end());


	// iterator t1A.insert(const_iterator position, value_type&& value);
	TestObject to90(9);
	toSetInsertResult = toSet.emplace(eastl::move(to90));
	EATEST_VERIFY(toSetInsertResult.second == true);
	EATEST_VERIFY(toSet.find(to90) != toSet.end());

	TestObject to91(9);
	toSetIterator = toSet.emplace_hint(toSetInsertResult.first, eastl::move(to91));
	EATEST_VERIFY(*toSetIterator == TestObject(9));
	EATEST_VERIFY(toSet.find(to91) != toSet.end());

	TestObject to10(10);
	toSetIterator = toSet.emplace_hint(toSet.begin(), eastl::move(to10)); // specify a bad hint. Insertion should still work.
	EATEST_VERIFY(*toSetIterator == TestObject(10));
	EATEST_VERIFY(toSet.find(to10) != toSet.end());

	return nErrorCount;
}






///////////////////////////////////////////////////////////////////////////////
// TestMultisetCpp11
//
// This function is designed to work with multiset, fixed_multiset, hash_multiset, fixed_hash_multiset
//
// This is similar to the TestSetCpp11 function, with some differences related 
// to handling of duplicate entries.
//
template <typename T1>
int TestMultisetCpp11()
{
	int nErrorCount = 0;

	// template <class... Args>
	// insert_return_type emplace(Args&&... args);
	//
	// template <class... Args> 
	// iterator emplace_hint(const_iterator position, Args&&... args);
	//
	// insert_return_type insert(value_type&& value);
	// iterator insert(const_iterator position, value_type&& value);
	TestObject::Reset();

	typedef T1 TOSet;
	typename TOSet::iterator toSetIterator;

	TOSet      toSet;
	TestObject to0(0);
	TestObject to1(1);

	toSetIterator = toSet.emplace(to0);
	EATEST_VERIFY(*toSetIterator == TestObject(0));
	//EATEST_VERIFY((TestObject::sTOCopyCtorCount == 2) && (TestObject::sTOMoveCtorCount == 1));  // Disabled until we can guarantee its behavior and deal with how it's different between compilers of differing C++11 support.

	toSetIterator = toSet.emplace(eastl::move(to1));
	EATEST_VERIFY(*toSetIterator == TestObject(1));

	// insert_return_type t1A.emplace(value_type&& value);
	TestObject to40(4);
	EATEST_VERIFY(toSet.find(to40) == toSet.end());
	EATEST_VERIFY(to40.mX == 4); // It should change to 0 below during the move swap.
	toSetIterator = toSet.emplace(eastl::move(to40));
	EATEST_VERIFY(*toSetIterator == TestObject(4));
	EATEST_VERIFY(toSet.find(to40) != toSet.end());
	EATEST_VERIFY(to40.mX == 0);

	TestObject to41(4);
	toSetIterator = toSet.emplace(eastl::move(to41));  // multiset can insert another of these.
	EATEST_VERIFY(*toSetIterator == TestObject(4));
	EATEST_VERIFY(toSet.find(to41) != toSet.end());

	// iterator t1A.emplace_hint(const_iterator position, value_type&& value);
	TestObject to50(5);
	toSetIterator = toSet.emplace(eastl::move(to50));
	EATEST_VERIFY(*toSetIterator == TestObject(5));
	EATEST_VERIFY(toSet.find(to50) != toSet.end());

	TestObject to51(5);
	toSetIterator = toSet.emplace_hint(toSetIterator, eastl::move(to51));
	EATEST_VERIFY(*toSetIterator == TestObject(5));
	EATEST_VERIFY(toSet.find(to51) != toSet.end());

	TestObject to6(6);
	toSetIterator = toSet.emplace_hint(toSet.begin(), eastl::move(to6)); // specify a bad hint. Insertion should still work.
	EATEST_VERIFY(*toSetIterator == TestObject(6));
	EATEST_VERIFY(toSet.find(to6) != toSet.end());
		
	TestObject to2(2);
	EATEST_VERIFY(toSet.find(to2) == toSet.end());
	toSetIterator = toSet.emplace(to2);
	EATEST_VERIFY(*toSetIterator == TestObject(2));
	EATEST_VERIFY(toSet.find(to2) != toSet.end());
	toSetIterator = toSet.emplace(to2);
	EATEST_VERIFY(*toSetIterator == TestObject(2));
	EATEST_VERIFY(toSet.find(to2) != toSet.end());

	// iterator t1A.emplace_hint(const_iterator position, const value_type& value);
	TestObject to70(7);
	toSetIterator = toSet.emplace(to70);
	EATEST_VERIFY(*toSetIterator == TestObject(7));
	EATEST_VERIFY(toSet.find(to70) != toSet.end());

	TestObject to71(7);
	toSetIterator = toSet.emplace_hint(toSetIterator, to71);
	EATEST_VERIFY(*toSetIterator == to71);
	EATEST_VERIFY(toSet.find(to71) != toSet.end());

	TestObject to8(8);
	toSetIterator = toSet.emplace_hint(toSet.begin(), to8); // specify a bad hint. Insertion should still work.
	EATEST_VERIFY(*toSetIterator == to8);
	EATEST_VERIFY(toSet.find(to8) != toSet.end());

	// insert_return_type t1A.insert(value_type&& value);
	TestObject to3(3);
	EATEST_VERIFY(toSet.find(to3) == toSet.end());
	toSetIterator = toSet.insert(TestObject(to3));
	EATEST_VERIFY(*toSetIterator == TestObject(3));
	EATEST_VERIFY(toSet.find(to3) != toSet.end());
	toSetIterator = toSet.insert(TestObject(to3));
	EATEST_VERIFY(*toSetIterator == TestObject(3));
	EATEST_VERIFY(toSet.find(to3) != toSet.end());

	// iterator t1A.insert(const_iterator position, value_type&& value);
	TestObject to90(9);
	toSetIterator = toSet.emplace(eastl::move(to90));
	EATEST_VERIFY(*toSetIterator == TestObject(9));
	EATEST_VERIFY(toSet.find(to90) != toSet.end());

	TestObject to91(9);
	toSetIterator = toSet.emplace_hint(toSetIterator, eastl::move(to91));
	EATEST_VERIFY(*toSetIterator == TestObject(9));
	EATEST_VERIFY(toSet.find(to91) != toSet.end());

	TestObject to10(10);
	toSetIterator = toSet.emplace_hint(toSet.begin(), eastl::move(to10)); // specify a bad hint. Insertion should still work.
	EATEST_VERIFY(*toSetIterator == TestObject(10));
	EATEST_VERIFY(toSet.find(to10) != toSet.end());

	return nErrorCount;
}







