/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/deque.h>
#include <EASTL/iterator.h>
#include <EASTL/vector.h>
#include <EASTL/set.h>
#include <EASTL/array.h>
#include <EASTL/numeric.h>
#include <EASTL/list.h>
#include <EASTL/slist.h>
#include <EASTL/string.h>
#include <EASTL/intrusive_list.h>
#include <EASTL/memory.h>
#include <EASTL/unique_ptr.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <stdio.h>
#include <string.h>
EA_RESTORE_ALL_VC_WARNINGS()

template <class T>
using detect_iterator_traits_reference = typename eastl::iterator_traits<T>::reference;

// This is used below, though is currently disabled as documented below.
struct IListNode : public eastl::intrusive_list_node{};

int TestIterator_advance()
{
	int nErrorCount = 0;

	{
		// void advance(InputIterator& i, Distance n)
		const int num_elements = 10;
		int i;

		eastl::vector<int> v;
		for(i = 0; i < num_elements; i++)
			v.push_back(i);

		// test forward advancement
		eastl::vector<int>::iterator it = v.begin();
		for(i = 0; i < num_elements; i++)
		{
			EATEST_VERIFY(*it == v[i]);
			eastl::advance(it, 1);
		}

		// test backwards advancement
		eastl::vector<int>::iterator it2 = v.end();
		i = num_elements - 1;
		do
		{
			eastl::advance(it2, -1);
			EATEST_VERIFY(*it2 == v[i]);
		}
		while(i-- != 0);
	}

	{
		// void advance(InputIterator& i, Distance n)
		eastl::list<int> intList;
		intList.push_back(0);
		intList.push_back(1);
		intList.push_back(42);
		intList.push_back(2);

		eastl::list<int>::iterator it = intList.begin();
		eastl::advance(it, intList.size());
		EATEST_VERIFY(it == intList.end());

		// Exercise advance with an signed Distance type.
		it = intList.begin();
		eastl::advance(it, (ssize_t)intList.size());
		EATEST_VERIFY(it == intList.end());


		eastl::slist<int> intSlist;
		intSlist.push_front(0);
		intSlist.push_front(1);
		intSlist.push_front(42);
		intSlist.push_front(2);

		eastl::slist<int>::iterator its = intSlist.begin();
		eastl::advance(its, intSlist.size());
		EATEST_VERIFY(its == intSlist.end());

		// Exercise advance with an signed Distance type.
		its = intSlist.begin();
		eastl::advance(its, (ssize_t)intSlist.size());
		EATEST_VERIFY(its == intSlist.end());
	}

	{
		// void next(InputIterator& i, Distance n)
		eastl::vector<int> v;
		v.push_back(0);
		v.push_back(1);
		v.push_back(42);
		v.push_back(2);

		eastl::vector<int>::iterator it = v.begin();
		EATEST_VERIFY(*eastl::next(it, 0) == 0);
		EATEST_VERIFY(*eastl::next(it /*testing the iterator distance default value*/) == 1);
		EATEST_VERIFY(*eastl::next(it, 2) == 42);
	}

	{
		// void prev(InputIterator& i, Distance n)
		eastl::vector<int> v;
		v.push_back(0);
		v.push_back(1);
		v.push_back(42);
		v.push_back(2);

		eastl::vector<int>::iterator it = v.end();
		EATEST_VERIFY(*eastl::prev(it, 2) == 42);
		EATEST_VERIFY(*eastl::prev(it /*testing the iterator distance default value*/) == 2);
	}

	return nErrorCount;
}

int TestIterator_moveIterator()
{
	int nErrorCount = 0;

	{
		eastl::vector<int> v = {0, 1, 42, 2};
		const auto constBeginMoveIter = eastl::make_move_iterator(v.begin());

		// operator++(int)
		auto moveIter = constBeginMoveIter;
		moveIter++; // the result of the expression is the incremented value, we need this test to read the existing state of the iterator.
		EATEST_VERIFY(*moveIter != *constBeginMoveIter);

		// operator--(int)
		moveIter = constBeginMoveIter + 2; // points to '42'
		moveIter--; // the result of the expression is the incremented value, we need this test to read the existing state of the iterator.
		EATEST_VERIFY(*moveIter != *(constBeginMoveIter + 2));
	}

	{
		// Ensure that move_iterator indeed move yielded value whenever possible.
		auto x = eastl::make_unique<int>(42);
		auto* pX = &x;
		auto moveIter = eastl::make_move_iterator(pX);

		constexpr bool isCorrectReferenceType = eastl::is_same_v<decltype(moveIter)::reference, eastl::unique_ptr<int>&&>;
		constexpr bool isCorrectReturnType = eastl::is_same_v<decltype(*moveIter), eastl::unique_ptr<int>&&>;

		static_assert(isCorrectReferenceType, "move_iterator::reference has wrong type.");
		static_assert(isCorrectReturnType, "move_iterator::operator*() has wrong return type.");
		EATEST_VERIFY(isCorrectReferenceType);
		EATEST_VERIFY(isCorrectReturnType);

		auto pMoveX = *moveIter;
		EATEST_VERIFY(*pMoveX == 42);
	}

	// Bellow are regression tests that ensure we are covering the defect LWG 2106: http://cplusplus.github.io/LWG/lwg-defects.html#2106
	{
		// Check that we support iterators yielding const references.
		const int x = 42;
		const int* pX = &x;
		auto moveIter = eastl::make_move_iterator(pX);

		constexpr bool isCorrectReferenceType = eastl::is_same_v<decltype(moveIter)::reference, const int&&>;
		constexpr bool isCorrectReturnType = eastl::is_same_v<decltype(*moveIter), const int&&>;

		static_assert(isCorrectReferenceType, "move_iterator::reference has wrong type.");
		static_assert(isCorrectReturnType, "move_iterator::operator*() has wrong return type.");
		EATEST_VERIFY(isCorrectReferenceType);
		EATEST_VERIFY(isCorrectReturnType);

		auto pCopiedX = *moveIter;
		EATEST_VERIFY(pCopiedX == 42);
	}

	{
		// Check that we support iterators yielding plain value (typically a proxy-iterator).
		struct FakeProxyIterator 
		{
			using iterator_category = EASTL_ITC_NS::forward_iterator_tag;
			using difference_type   = ptrdiff_t;
			using value_type        = int;
			using pointer           = int;  // Note that we are yielding by value.
			using reference         = int;  // Note that we are yielding by value.

			reference operator*() const { return 42; }
			pointer operator->() { return 42; }
			FakeProxyIterator& operator++() { return *this; }  
			FakeProxyIterator operator++(int) { return {}; }

			bool operator==(const FakeProxyIterator& rhs) { return true; };
			bool operator!=(const FakeProxyIterator& rhs) { return false; };  
		};

		FakeProxyIterator it = {};
		auto moveIter = eastl::make_move_iterator(it);

		constexpr bool isCorrectReferenceType = eastl::is_same_v<decltype(moveIter)::reference, int>;
		constexpr bool isCorrectReturnType = eastl::is_same_v<decltype(*moveIter), int>;

		static_assert(isCorrectReferenceType, "move_iterator::reference has wrong type.");
		static_assert(isCorrectReturnType, "move_iterator::operator*() has wrong return type.");
		EATEST_VERIFY(isCorrectReferenceType);
		EATEST_VERIFY(isCorrectReturnType);

		auto pCopiedX = *moveIter;
		EATEST_VERIFY(pCopiedX == 42);
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestIterator
//
int TestIterator()
{
	int nErrorCount = 0;
	nErrorCount += TestIterator_advance();
	nErrorCount += TestIterator_moveIterator();

	{
		// reverse_iterator
		// reverse_iterator<Iterator> make_reverse_iterator(Iterator mi)
		{
			eastl::vector<int> src;
			for(int i = 0; i < 10; i++)
				src.push_back(i); // src should become {0,1,2,3,4,5,6,7,8,9}

			auto itr = eastl::make_reverse_iterator(src.end());
			EATEST_VERIFY(*itr == 9); ++itr;
			EATEST_VERIFY(*itr == 8); ++itr;
			EATEST_VERIFY(*itr == 7); ++itr;
			EATEST_VERIFY(*itr == 6); ++itr;
			EATEST_VERIFY(*itr == 5); ++itr;
			EATEST_VERIFY(*itr == 4); ++itr;
			EATEST_VERIFY(*itr == 3); ++itr;
			EATEST_VERIFY(*itr == 2); ++itr;
			EATEST_VERIFY(*itr == 1); ++itr;
			EATEST_VERIFY(*itr == 0); ++itr;
			EATEST_VERIFY( itr == src.rend());
			EATEST_VERIFY( itr == eastl::make_reverse_iterator(src.begin()));
		}
	}

	{
		// Regression bug with assign/insert combined with reverse iterator.
		eastl::vector<int> a;
		for (int i = 0; i < 10; ++i) {
			a.push_back(i);
		}

		eastl::deque<int> d;
		d.assign(a.rbegin(), a.rend());
		for (int i = 0; i < 10; ++i) {
			EATEST_VERIFY(a[i] == d[a.size() - i - 1]);
		}
		d.insert(d.end(), a.rbegin(), a.rend());
		for (int i = 0; i < 10; ++i) {
			EATEST_VERIFY(a[i] == d[d.size() - i - 1]);
		}

		eastl::vector<int> b;
		b.assign(a.rbegin(), a.rend());
		for (int i = 0; i < 10; ++i) {
			EATEST_VERIFY(a[i] == b[a.size() - i - 1]);
		}
		b.insert(b.end(), a.rbegin(), a.rend());
		for (int i = 0; i < 10; ++i) {
			EATEST_VERIFY(a[i] == b[b.size() - i - 1]);
		}
	}

	{
		// move_iterator
		// move_iterator<Iterator> make_move_iterator(Iterator mi)
		typedef eastl::vector<eastl::string> StringArray;

		StringArray src;
		for(eastl_size_t i = 0; i < 4; i++)
			src.push_back(eastl::string(1, (char8_t)('0' + i))); // v should become {"0", "1", "2", "3"};

		// Moves the values out of the string array and into the result.
		StringArray dst(eastl::make_move_iterator(src.begin()), eastl::make_move_iterator(src.end()));

		EATEST_VERIFY((src.size() == 4) && (src[0] ==  "") && (src[3] ==  ""));
		EATEST_VERIFY((dst.size() == 4) && (dst[0] == "0") && (dst[3] == "3"));
	}

	{
		// back_insert_iterator
		// back_inserter
		EA_CPP14_CONSTEXPR int n = 3;
		eastl::vector<TestObject> v1, v2, v3;
		v1.resize(n); v2.reserve(n); v3.reserve(n);
		{
			int64_t copyCtorCount0 = TestObject::sTOCopyCtorCount, moveCtorCount0 = TestObject::sTOMoveCtorCount;
			eastl::copy(v1.begin(), v1.end(), eastl::back_inserter(v2));
			EATEST_VERIFY(v1.size() == v2.size() && TestObject::sTOCopyCtorCount == (copyCtorCount0 + n) &&
				TestObject::sTOMoveCtorCount == moveCtorCount0);
		}
		{
			int64_t copyCtorCount0 = TestObject::sTOCopyCtorCount, moveCtorCount0 = TestObject::sTOMoveCtorCount;
			eastl::move(v1.begin(), v1.end(), eastl::back_inserter(v3));
			EATEST_VERIFY(v1.size() == v3.size() && TestObject::sTOCopyCtorCount == copyCtorCount0 &&
				TestObject::sTOMoveCtorCount == (moveCtorCount0 + n));
		}
	}

	{
		// front_insert_iterator
		// front_inserter
		// To do.
	}

	{
		// insert_iterator
		// inserter
		// To do.
	}

	{
		// difference_type distance(InputIterator first, InputIterator last)
		eastl::vector<int> intVector = {0, 1, 2, 3, 4, 5, 6, 7};
		EATEST_VERIFY(eastl::distance(intVector.begin(), intVector.end()) == 8);
    }


	{
		#if EASTL_BEGIN_END_ENABLED
			// begin / end
			// auto inline begin(Container& container) -> decltype(container.begin())
			// auto inline end(Container& container) -> decltype(container.end())

			eastl::vector<int> intVector;
			eastl::vector<int>::iterator intVectorIterator = eastl::begin(intVector);
			EATEST_VERIFY(intVectorIterator == eastl::end(intVector));

			eastl::list<int> intList;
			eastl::list<int>::iterator intListIterator = eastl::begin(intList);
			EATEST_VERIFY(intListIterator == eastl::end(intList));

			eastl::set<int> intSet;
			eastl::set<int>::iterator intSetIterator = eastl::begin(intSet);
			EATEST_VERIFY(intSetIterator == eastl::end(intSet));

			eastl::array<int, 0> intArray;
			eastl::array<int, 0>::iterator intArrayIterator = eastl::begin(intArray);
			EATEST_VERIFY(intArrayIterator == eastl::end(intArray));

			eastl::intrusive_list<IListNode> intIList;
			eastl::intrusive_list<IListNode>::iterator intIListIterator = eastl::begin(intIList);
			EATEST_VERIFY(intIListIterator == eastl::end(intIList));

			eastl::string8 str8;
			eastl::string8::iterator string8Iterator = eastl::begin(str8);
			EATEST_VERIFY(string8Iterator == eastl::end(str8));
		#endif
	}

	// eastl::data
	{
		eastl::array<int, 0> intArray;
		int* pIntArrayData = eastl::data(intArray);
		EATEST_VERIFY(pIntArrayData == intArray.data());

		eastl::vector<int> intVector;
		int* pIntVectorData = eastl::data(intVector);
		EATEST_VERIFY(pIntVectorData == intVector.data());

		int intCArray[34];
		int* pIntCArray = eastl::data(intCArray);
		EATEST_VERIFY(pIntCArray == intCArray);

		std::initializer_list<int> intInitList;
		const int* pIntInitList = eastl::data(intInitList);
		EATEST_VERIFY(pIntInitList == intInitList.begin());
	}

	// eastl::size
	{
		eastl::vector<int> intVector;
		intVector.push_back();
		intVector.push_back();
		intVector.push_back();
		EATEST_VERIFY(eastl::size(intVector) == 3);

		int intCArray[34];
		EATEST_VERIFY(eastl::size(intCArray) == 34);
		static_assert(eastl::size(intCArray) == 34, "eastl::size failure");
	}

	// eastl::ssize
	{
		eastl::vector<int> intVector;
		intVector.push_back();
		intVector.push_back();
		intVector.push_back();
		EATEST_VERIFY(eastl::ssize(intVector) == (signed)3);

		int intCArray[34];
		EATEST_VERIFY(eastl::ssize(intCArray) == (signed)34);
		static_assert(eastl::ssize(intCArray) == 34, "eastl::ssize failure");
	}

	// eastl::empty
	{
		eastl::vector<int> intVector;
		EATEST_VERIFY(eastl::empty(intVector));
		intVector.push_back();
		EATEST_VERIFY(!eastl::empty(intVector));

		std::initializer_list<int> intInitListEmpty;
		EATEST_VERIFY(eastl::empty(intInitListEmpty));
		EATEST_VERIFY(!eastl::empty({1, 2, 3, 4, 5, 6}));
	}

	// Range-based for loops
	{
		{
			eastl::vector<int> v;
			int I = 0;

			v.push_back(0);
			v.push_back(1);

			for(int i : v)
				EATEST_VERIFY(i == I++);
		}

		{
			eastl::string s8;
			char C = 'a';

			s8.push_back('a');
			s8.push_back('b');

			for(char c : s8)
				EATEST_VERIFY(c == C++);
		}
	}


	{
		// is_iterator_wrapper
		static_assert((eastl::is_iterator_wrapper<void>::value													== false),  "is_iterator_wrapper failure");
		static_assert((eastl::is_iterator_wrapper<int>::value													== false),  "is_iterator_wrapper failure");
		static_assert((eastl::is_iterator_wrapper<int*>::value													== false),  "is_iterator_wrapper failure");
		static_assert((eastl::is_iterator_wrapper<eastl::array<int, 2>::iterator>::value							== false),   "is_iterator_wrapper failure");
		static_assert((eastl::is_iterator_wrapper<eastl::array<char, 2>*>::value									== false),  "is_iterator_wrapper failure");
		static_assert((eastl::is_iterator_wrapper<eastl::vector<char> >::value									== false),  "is_iterator_wrapper failure");
		static_assert((eastl::is_iterator_wrapper<eastl::generic_iterator<int*> >::value						== true),   "is_iterator_wrapper failure");
		static_assert((eastl::is_iterator_wrapper<eastl::move_iterator<eastl::array<int, 2>::iterator> >::value	== true),   "is_iterator_wrapper failure");
		static_assert((eastl::is_iterator_wrapper<eastl::reverse_iterator<eastl::array<int, 2>::iterator> >::value == false),  "is_iterator_wrapper failure");
		static_assert((eastl::is_iterator_wrapper<eastl::reverse_iterator<int*> >::value						== false),  "is_iterator_wrapper failure");
		static_assert((eastl::is_iterator_wrapper<eastl::reverse_iterator<eastl::move_iterator<int*>> >::value  == true),   "is_iterator_wrapper failure");
	}


	{
		// unwrap_iterator
		int intArray[2];
		int* pInt = eastl::unwrap_iterator(&intArray[0]);
		intArray[0] = 17;
		EATEST_VERIFY(*pInt == 17);
		static_assert((eastl::is_same<decltype(eastl::unwrap_iterator(&intArray[0])), int*>::value == true),  "unwrap_iterator failure");

		eastl::generic_iterator<int*> giIntArray(intArray);
		pInt = eastl::unwrap_iterator(giIntArray);
		intArray[0] = 18;
		EATEST_VERIFY(*pInt == 18);
		static_assert((eastl::is_same<decltype(eastl::unwrap_iterator(giIntArray)), int*>::value == true),  "unwrap_iterator failure");

		eastl::vector<int> intVector(4, 19);
		eastl::vector<int>::iterator itVector = eastl::unwrap_iterator(intVector.begin());
		EATEST_VERIFY(*itVector == 19);
		static_assert((eastl::is_same<decltype(eastl::unwrap_iterator(intVector.begin())), eastl::vector<int>::iterator>::value == true),  "unwrap_iterator failure");

		eastl::move_iterator<eastl::vector<int>::iterator> miIntVector(intVector.begin());
		itVector = eastl::unwrap_iterator(miIntVector);
		intVector[0] = 20;
		EATEST_VERIFY(*itVector == 20);
		static_assert((eastl::is_same<decltype(eastl::unwrap_iterator(miIntVector)), eastl::vector<int>::iterator>::value == true),  "unwrap_iterator failure");

		eastl::reverse_iterator<eastl::vector<int>::iterator> riIntVector = intVector.rbegin();
		eastl::reverse_iterator<eastl::vector<int>::iterator> riUnwrapped = eastl::unwrap_iterator(riIntVector);
		EATEST_VERIFY(*riUnwrapped == 19);
		static_assert((eastl::is_same<decltype(eastl::unwrap_iterator(riIntVector)), eastl::reverse_iterator<eastl::vector<int>::iterator>>::value == true), "unwrap_iterator failure");

		eastl::reverse_iterator<eastl::move_iterator<eastl::vector<int>::iterator>> rimiIntVec(miIntVector);
		static_assert((eastl::is_same<decltype(eastl::unwrap_iterator(rimiIntVec)), eastl::reverse_iterator<eastl::vector<int>::iterator>>::value == true), "unwrap_iterator failure");

		eastl::reverse_iterator<eastl::generic_iterator<int*>> rigiIntArray(giIntArray);
		static_assert((eastl::is_same<decltype(eastl::unwrap_iterator(rigiIntArray)), eastl::reverse_iterator<int*>>::value == true), "unwrap_iterator failure");

		eastl::deque<int> intDeque(3);
		eastl::deque<int>::iterator begin = intDeque.begin();
		eastl::generic_iterator<eastl::deque<int>::iterator> giWrappedBegin(begin);
		static_assert((eastl::is_same<decltype(eastl::unwrap_iterator(giWrappedBegin)), eastl::deque<int>::iterator>::value == true), "unwrap_iterator failure");

		eastl::deque<int>::iterator unwrappedBegin = eastl::unwrap_iterator(giWrappedBegin);
		EATEST_VERIFY(begin == unwrappedBegin);
	}

	{
		// unwrap_generic_iterator
		int intArray[2] = {0, 1};
		eastl::generic_iterator<int*> giIntArray(intArray);
		int* pInt = eastl::unwrap_generic_iterator(giIntArray);
		EATEST_VERIFY(*pInt == 0);
		static_assert((eastl::is_same<decltype(eastl::unwrap_generic_iterator(giIntArray)), int*>::value == true),  "unwrap_iterator failure");

		eastl::move_iterator<int*> miIntArray(intArray);
		static_assert((eastl::is_same<decltype(eastl::unwrap_generic_iterator(miIntArray)), eastl::move_iterator<int*>>::value == true),  "unwrap_iterator failure");

		eastl::vector<int> intVector(1, 1);
		eastl::generic_iterator<eastl::vector<int>::iterator> giVectorInt(intVector.begin());
		eastl::vector<int>::iterator it = unwrap_generic_iterator(giVectorInt);
		EATEST_VERIFY(*it == 1);
		static_assert((eastl::is_same<decltype(eastl::unwrap_generic_iterator(giVectorInt)), eastl::vector<int>::iterator>::value == true),  "unwrap_iterator failure");
	}

	{
		// unwrap_move_iterator
		int intArray[2] = {0, 1};
		eastl::move_iterator<int*> miIntArray(intArray);
		int* pInt = eastl::unwrap_move_iterator(miIntArray);
		EATEST_VERIFY(*pInt == 0);
		static_assert((eastl::is_same<decltype(eastl::unwrap_move_iterator(miIntArray)), int*>::value == true),  "unwrap_iterator failure");

		eastl::generic_iterator<int*> giIntArray(intArray);
		static_assert((eastl::is_same<decltype(eastl::unwrap_move_iterator(giIntArray)), eastl::generic_iterator<int*>>::value == true),  "unwrap_iterator failure");

		eastl::vector<int> intVector(1, 1);
		eastl::move_iterator<eastl::vector<int>::iterator> miVectorInt(intVector.begin());
		eastl::vector<int>::iterator it = unwrap_move_iterator(miVectorInt);
		EATEST_VERIFY(*it == 1);
		static_assert((eastl::is_same<decltype(eastl::unwrap_move_iterator(miVectorInt)), eastl::vector<int>::iterator>::value == true),  "unwrap_iterator failure");
	}

	{
		// array cbegin - cend
		int arr[3]{ 1, 2, 3 };
		auto b = eastl::cbegin(arr);
		auto e = eastl::cend(arr);
		EATEST_VERIFY(*b == 1);
		
		auto dist = eastl::distance(b,e);
		EATEST_VERIFY(dist == 3);
	}

	{
		// Regression test that ensure N3844 is working correctly.
		static_assert(!eastl::is_detected<detect_iterator_traits_reference, int>::value, "detecting iterator_traits<int> should SFINAE gracefully.");
	}

	return nErrorCount;
}












