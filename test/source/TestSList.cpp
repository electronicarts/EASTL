/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EASTL/slist.h>
#include <EABase/eabase.h>
#include <EASTL/fixed_allocator.h>

using namespace eastl;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct TestObj
{
	TestObj() : mI(0), mMoveCtor(0), mCopyCtor(0) {} 
	explicit TestObj(int i) : mI(i), mMoveCtor(0), mCopyCtor(0) {}
	explicit TestObj(int a, int b, int c, int d) : mI(a+b+c+d), mMoveCtor(0), mCopyCtor(0) {}

	TestObj(TestObj&& other)
	{
		mI = other.mI;
		mMoveCtor = other.mMoveCtor;
		mCopyCtor = other.mCopyCtor;
		mMoveCtor++;
	}

	TestObj(const TestObj& other)
	{
		mI = other.mI;
		mMoveCtor = other.mMoveCtor;
		mCopyCtor = other.mCopyCtor;
		mCopyCtor++;
	}

	TestObj& operator=(const TestObj& other) 
	{
		mI = other.mI;
		mMoveCtor = other.mMoveCtor;
		mCopyCtor = other.mCopyCtor;
		return *this;
	}

	int mI;
	int mMoveCtor;
	int mCopyCtor;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TestSList
int TestSList()
{
	int nErrorCount = 0;

	// slist();
	{
		slist<int> list;
		VERIFY(list.empty());
		VERIFY(list.size() == 0);
	}

	// slist(const allocator_type& allocator);
	{ 
		MallocAllocator::reset_all();

		VERIFY(MallocAllocator::mAllocCountAll == 0);
		slist<int, MallocAllocator> list;
		list.resize(100, 42);
		VERIFY(MallocAllocator::mAllocCountAll == 100);
	}

	// explicit slist(size_type n, const allocator_type& allocator = EASTL_SLIST_DEFAULT_ALLOCATOR);
	{
		slist<int> list(100);
		VERIFY(list.size() == 100);
		VERIFY(!list.empty());
	}

	// slist(size_type n, const value_type& value, const allocator_type& allocator = EASTL_SLIST_DEFAULT_ALLOCATOR);
	{
		slist<int> list(32, 42);
		VERIFY(list.size() == 32);
		VERIFY(list.front() == 42);
		VERIFY(!list.empty());
	}

	// slist(const this_type& x);
	{
		slist<int> list1;
		list1.resize(100, 42);

		VERIFY(!list1.empty());
		slist<int> list2(list1);
		VERIFY(!list2.empty());
		VERIFY(list1 == list2);
	}
	
	// slist(std::initializer_list<value_type> ilist, const allocator_type& allocator = EASTL_SLIST_DEFAULT_ALLOCATOR);
	{
	#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
		slist<int> list1({1,2,3,4,5,6,7,8});
		VERIFY(!list1.empty());
		VERIFY(list1.size() == 8);
	#endif
	}

	// slist(this_type&& x);
	{
		slist<int> list1; 
		list1.resize(100,42);

		slist<int> list2(eastl::move(list1));

		VERIFY(list1.empty());
		VERIFY(!list2.empty());
		VERIFY(list1 != list2);
	}
	
	// slist(this_type&& x, const allocator_type& allocator);
	{ } 

	// slist(InputIterator first, InputIterator last); 
	{
		slist<int> list1; 
		list1.resize(100, 42);
		VERIFY(!list1.empty());

		slist<int> list2(list1.begin(), list1.end());
		VERIFY(!list2.empty());
		VERIFY(list1 == list2);
	}

	// this_type& operator=(const this_type& x);
	{
		slist<int> list1;
		list1.resize(100, 42);
		VERIFY(!list1.empty());

		slist<int> list2 = list1;
		VERIFY(!list2.empty());
		VERIFY(list1 == list2);
	}

	// this_type& operator=(std::initializer_list<value_type>);
	{
		slist<int> list1 = {1,2,3,4,5,6,7,8};
		VERIFY(!list1.empty());
	}

	// this_type& operator=(this_type&& x);
	{
		slist<int> list1;
		list1.resize(100, 42);
		slist<int> list2 = eastl::move(list1);

		VERIFY(list1.empty());
		VERIFY(!list2.empty());
		VERIFY(list1 != list2);
	}

	// void swap(this_type& x);
	{
		slist<int> list1;
		list1.resize(8, 37);

		slist<int> list2; 
		VERIFY(!list1.empty());
		VERIFY(list1.size() == 8);
		VERIFY(list2.empty());

		list2.swap(list1);

		VERIFY(list1.empty());
		VERIFY(!list2.empty());
	}

	// void assign(size_type n, const value_type& value);
	{
		slist<int> list1;
		list1.assign(100, 42);

		VERIFY(!list1.empty());
		VERIFY(list1.size() == 100);

		for(auto& e : list1)
			VERIFY(e == 42);
	}

	// void assign(std::initializer_list<value_type> ilist);
	{
	#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
		slist<int> list1;
		list1.assign({1,2,3,4,5,6,7,8});

		VERIFY(!list1.empty());
		VERIFY(list1.size() == 8);

		auto i = eastl::begin(list1);
		VERIFY(*i == 1); i++;
		VERIFY(*i == 2); i++;
		VERIFY(*i == 3); i++;
		VERIFY(*i == 4); i++;
		VERIFY(*i == 5); i++;
		VERIFY(*i == 6); i++;
		VERIFY(*i == 7); i++;
		VERIFY(*i == 8); i++;
		VERIFY(i == eastl::end(list1));
	#endif
	}

	// void assign(InputIterator first, InputIterator last);
	{
		slist<int> list1;
		list1.resize(100, 42);
		VERIFY(!list1.empty());

		slist<int> list2;
		list2.assign(list1.begin(), list1.end());
		VERIFY(!list2.empty());
		VERIFY(list1 == list2);
	}

	// iterator       begin() EA_NOEXCEPT;
	// const_iterator begin() const EA_NOEXCEPT;
	// const_iterator cbegin() const EA_NOEXCEPT;
	{
		slist<int> list1;
		list1.resize(100, 1);
		VERIFY(!list1.empty());

		const auto ci = list1.begin();
		auto i        = list1.begin();
		auto ci2      = list1.cbegin();

		VERIFY(*i   == 1);
		VERIFY(*ci  == 1);
		VERIFY(*ci2 == 1);
	}

	// iterator       end() EA_NOEXCEPT;
	// const_iterator end() const EA_NOEXCEPT;
	// const_iterator cend() const EA_NOEXCEPT;
	{
		slist<int> list1;
		list1.resize(100, 42);
		VERIFY(!list1.empty());

		const auto ci = list1.end();
		auto i = list1.end();
		auto ci2 = list1.cend();

		VERIFY(i == eastl::end(list1));
		VERIFY(ci == eastl::end(list1));
		VERIFY(ci2 == eastl::end(list1));
	}

	// iterator       before_begin() EA_NOEXCEPT;
	// const_iterator before_begin() const EA_NOEXCEPT;
	// const_iterator cbefore_begin() const EA_NOEXCEPT;
	// iterator        previous(const_iterator position);
	// const_iterator  previous(const_iterator position) const;
	{
		slist<int> list1;

		auto b = list1.begin();
		auto prev = list1.previous(b);

		VERIFY(prev == list1.before_begin());
	}

	// reference       front();
	// const_reference front() const;
	{
		slist<int> list1;
		list1.resize(100, 1);

		VERIFY(list1.begin() == eastl::begin(list1));
		VERIFY(list1.front() == 1);

		const slist<int> clist1(list1); 
		VERIFY(clist1.front() == 1);
		VERIFY(list1.validate());
		VERIFY(clist1.validate());
	}


	//     void emplace_front(Args&&... args);
	//     void emplace_front(value_type&& value);
	//     void emplace_front(const value_type& value);
	{
		slist<TestObj> list1;
		list1.emplace_front(42);
		VERIFY(list1.front().mI == 42);
		VERIFY(list1.front().mCopyCtor == 0);
		VERIFY(list1.front().mMoveCtor == 0);
		VERIFY(list1.size() == 1);
		VERIFY(list1.validate());

		list1.emplace_front(1,2,3,4);
		VERIFY(list1.front().mCopyCtor == 0);
		VERIFY(list1.front().mMoveCtor == 0);
		VERIFY(list1.front().mI == (1+2+3+4));
		VERIFY(list1.size() == 2);
		VERIFY(list1.validate());
	}

	// void      push_front(const value_type& value);
	// reference push_front();
	// void      push_front(value_type&& value);
	{
		slist<TestObj> list1;
		list1.push_front(TestObj(42));
		VERIFY(list1.front().mI == 42);
		VERIFY(list1.front().mCopyCtor == 0);
		VERIFY(list1.front().mMoveCtor == 1);
		VERIFY(list1.size() == 1);

		list1.push_front();
		VERIFY(list1.front().mCopyCtor == 0);
		VERIFY(list1.front().mMoveCtor == 0);
		VERIFY(list1.front().mI == 0);
		VERIFY(list1.size() == 2);

		list1.push_front().mI = 1492;
		VERIFY(list1.front().mI == 1492);
		VERIFY(list1.validate());
	}

	// void pop_front();
	{
		slist<int> list1;
		list1.push_front(4);
		list1.push_front(3);
		list1.push_front(2);
		list1.push_front(1);

		list1.pop_front();
		VERIFY(list1.front() == 2);
		VERIFY(list1.size()  == 3);
		VERIFY(list1.validate());

		list1.pop_front();
		VERIFY(list1.front() == 3);
		VERIFY(list1.size()  == 2);
		VERIFY(list1.validate());

		list1.pop_front();
		VERIFY(list1.front() == 4);
		VERIFY(list1.size()  == 1);
		VERIFY(list1.validate());
	}

	// bool empty() const EA_NOEXCEPT;
	// size_type size() const EA_NOEXCEPT;
	{
		slist<int> list1;
		VERIFY(list1.empty());
		VERIFY(list1.size() == 0);
		VERIFY(list1.validate());

		list1.push_front(42);
		VERIFY(!list1.empty());
		VERIFY(list1.size() == 1);
		VERIFY(list1.validate());

		list1.pop_front();
		VERIFY(list1.empty());
		VERIFY(list1.size() == 0);
		VERIFY(list1.validate());
	}


	// void resize(size_type n, const value_type& value);
	// void resize(size_type n);
	{
		slist<int> list1;
		VERIFY(list1.empty());
		list1.resize(100, 42);
		VERIFY(list1.front() == 42);
		VERIFY(!list1.empty());
		VERIFY(list1.size() == 100);
		VERIFY(list1.validate());

		slist<int> list2;
		VERIFY(list2.empty());
		list2.resize(100);
		VERIFY(!list2.empty());
		VERIFY(list2.size() == 100);
		VERIFY(list2.validate());
	}

	// iterator insert(const_iterator position);
	// iterator insert(const_iterator position, const value_type& value);
	// void     insert(const_iterator position, size_type n, const value_type& value);
	{
		static const int MAGIC_VALUE = 4242;
		struct TestVal
		{
			TestVal() : mV(MAGIC_VALUE) {}
			TestVal(int v) : mV(v) {}
			operator int() { return mV; }
			int mV;
		};

		slist<TestVal> list1;
		VERIFY(list1.empty());

		auto insert_iter = eastl::begin(list1);
		list1.insert(insert_iter);
		VERIFY(list1.size() == 1);
		VERIFY(!list1.empty());
		VERIFY(list1.validate());

		list1.insert(insert_iter, 42);
		VERIFY(list1.size() == 2);
		VERIFY(!list1.empty());
		VERIFY(list1.front() == MAGIC_VALUE);
		VERIFY(list1.validate());


		list1.insert(insert_iter, 43);
		VERIFY(list1.size() == 3);
		VERIFY(!list1.empty());
		VERIFY(list1.front() == MAGIC_VALUE);
		VERIFY(list1.validate());
	}

	// template <typename InputIterator>
	// void insert(const_iterator position, InputIterator first, InputIterator last);
	{
		slist<int> list1;
		VERIFY(list1.empty());
		list1.resize(100, 42);
		VERIFY(list1.size() == 100);
		VERIFY(!list1.empty());
		VERIFY(list1.validate());

		slist<int> list2;
		list2.resize(400, 24);
		VERIFY(list2.size() == 400);
		VERIFY(!list2.empty());
		VERIFY(list1.validate());

		list1.insert(eastl::end(list1), eastl::begin(list2), eastl::end(list2)); // [42,42,42,...,42, | 24,24,24,24...]
		VERIFY(!list1.empty());
		VERIFY(list1.size() == 500);
		VERIFY(list1.front() == 42);
		VERIFY(list1.validate());

		auto boundary_iter = list1.begin();
		eastl::advance(boundary_iter, 100); // move to insertation point
		VERIFY(*boundary_iter == 24);
		VERIFY(list1.validate());
	}


	// Returns an iterator pointing to the last inserted element, or position if insertion count is zero.
	// iterator insert_after(const_iterator position);
	// iterator insert_after(const_iterator position, const value_type& value);
	// iterator insert_after(const_iterator position, size_type n, const value_type& value);
	// iterator insert_after(const_iterator position, std::initializer_list<value_type> ilist);
	{
		slist<int> list1;
		VERIFY(list1.empty());
		list1.push_front();

		list1.insert_after(list1.begin());
		VERIFY(!list1.empty());
		VERIFY(list1.size() == 2);
		VERIFY(list1.validate());

		list1.insert_after(list1.begin(), 43);
		VERIFY(list1.size() == 3);
		VERIFY(list1.validate());

		list1.insert_after(list1.begin(), 10, 42);
		VERIFY(list1.size() == 13);
		VERIFY(eastl::count_if(list1.begin(), list1.end(), [](int i) { return i == 42; }) == 10);
		VERIFY(list1.validate());

		list1.insert_after(list1.begin(), {1,2,3,4,5,6,7,8,9,0});
		VERIFY(list1.size() == 23);
		VERIFY(list1.validate());
	}

	// iterator insert_after(const_iterator position, value_type&& value);
	{
		slist<TestObj> list1;
		VERIFY(list1.empty());
		list1.push_front();

		auto inserted = list1.insert_after(list1.begin(), TestObj(42));
		VERIFY(!list1.empty());
		VERIFY((*inserted).mCopyCtor == 0);
		VERIFY((*inserted).mMoveCtor == 1);
	}

	// iterator insert_after(const_iterator position, InputIterator first, InputIterator last);
	{
		slist<int> list1 = {0,1,2,3,4};
		slist<int> list2 = {9,8,7,6,5};
		list1.insert_after(list1.begin(), list2.begin(), list2.end());
		VERIFY(list1 == slist<int>({0,9,8,7,6,5,1,2,3,4}));
	}

	// iterator emplace_after(const_iterator position, Args&&... args);
	// iterator emplace_after(const_iterator position, value_type&& value);
	// iterator emplace_after(const_iterator position, const value_type& value);
	{
		slist<TestObj> list1;
		list1.emplace_after(list1.before_begin(), 42);
		VERIFY(list1.front().mI == 42);
		VERIFY(list1.front().mCopyCtor == 0);
		VERIFY(list1.front().mMoveCtor == 0);
		VERIFY(list1.size() == 1);
		VERIFY(list1.validate());

		list1.emplace_after(list1.before_begin(),1,2,3,4);
		VERIFY(list1.front().mCopyCtor == 0);
		VERIFY(list1.front().mMoveCtor == 0);
		VERIFY(list1.front().mI == (1+2+3+4));
		VERIFY(list1.size() == 2);
		VERIFY(list1.validate());
	}

	// iterator erase(const_iterator position);
	// iterator erase(const_iterator first, const_iterator last);
	{
		slist<int> list1 = {0,1,2,3,4,5,6,7};

		auto p = list1.begin();
		p++; p++; p++;

		list1.erase(p);
		VERIFY(list1 == slist<int>({0,1,2,4,5,6,7}));

		list1.erase(list1.begin(), list1.end());
		VERIFY(list1 == slist<int>({}));
		VERIFY(list1.size() == 0);
		VERIFY(list1.empty());
	}

	// iterator erase_after(const_iterator position);
	// iterator erase_after(const_iterator before_first, const_iterator last);
	{
		slist<int> list1 = {0,1,2,3,4,5,6,7};
		auto p = list1.begin();

		list1.erase_after(p);
		VERIFY(list1 == slist<int>({0,2,3,4,5,6,7}));
		VERIFY(list1.validate());

		list1.erase_after(p);
		VERIFY(list1 == slist<int>({0,3,4,5,6,7}));
		VERIFY(list1.validate());

		list1.erase_after(p);
		VERIFY(list1 == slist<int>({0,4,5,6,7}));
		VERIFY(list1.validate());

		list1.erase_after(p, list1.end());
		VERIFY(list1 == slist<int>({0}));
		VERIFY(list1.validate());
	}

	// void clear();
	{
		slist<int> list1;
		list1.resize(100, 42);
		VERIFY(!list1.empty());
		VERIFY(list1.size() == 100);
		VERIFY(list1.validate());

		list1.clear();
		VERIFY(list1.empty());
		VERIFY(list1.size() == 0);
		VERIFY(list1.validate());
	}

	// void reset_lose_memory();    
	{
		typedef eastl::slist<int, fixed_allocator> SIntList;
		typedef SIntList::node_type                SIntListNode;
		const size_t  kBufferCount = 100;
		SIntListNode  buffer1[kBufferCount];
		SIntList      list1;
		const size_t  kAlignOfSIntListNode = EA_ALIGN_OF(SIntListNode);
		list1.get_allocator().init(buffer1, sizeof(buffer1), sizeof(SIntListNode), kAlignOfSIntListNode);

		VERIFY(list1.empty());
		VERIFY(list1.size() == 0);
		VERIFY(list1.validate());

		list1.resize(kBufferCount, 42);
		VERIFY(!list1.empty());
		VERIFY(list1.size() == kBufferCount);
		VERIFY(list1.validate());

		list1.reset_lose_memory();
		VERIFY(list1.empty());
		VERIFY(list1.size() == 0);
		VERIFY(list1.validate());
	}

	// void remove(const value_type& value);
	{
		slist<int> list1 = {0,1,2,3,4};
		slist<int> list2 = {0,1,3,4};

		list1.remove(2);

		VERIFY(list1 == list2);
		VERIFY(list1.validate());
		VERIFY(list2.validate());
	}

	// void remove_if(Predicate predicate);
	{
		slist<int> list1;
		list1.resize(100, 42);
		VERIFY(list1.size() == 100);
		VERIFY(list1.validate());

		list1.remove_if([](int i) { return i == 1234; }); // intentionally remove nothing.
		VERIFY(list1.size() == 100);
		VERIFY(list1.validate());

		list1.remove_if([](int i) { return i == 42; });
		VERIFY(list1.size() == 0);
		VERIFY(list1.validate());
	}

	// void reverse() EA_NOEXCEPT;
	{
		slist<int> list1 = {0,1,2,3,4};
		slist<int> list2 = {4,3,2,1,0};
		VERIFY(list1 != list2);

		list1.reverse();
		VERIFY(list1 == list2);
	}

	// void splice(const_iterator position, this_type& x);
	// void splice(const_iterator position, this_type& x, const_iterator i);
	// void splice(const_iterator position, this_type& x, const_iterator first, const_iterator last);
	{
		slist<int> valid = {0,1,2,3,4,5,6,7};	
		{
			slist<int> list1 = {0,1,2,3};
			slist<int> list2 = {4,5,6,7};
			list1.splice(list1.end(), list2);

			VERIFY(list1 == valid);
			VERIFY(list1.validate());
		}
		{
			slist<int> list1 = {0,1,2,3};
			slist<int> list2 = {4,5,6,7};

			list1.splice(list1.begin(), list2, list2.begin());
			VERIFY(list1 == slist<int>({4,0,1,2,3}));
			VERIFY(list2 == slist<int>({5,6,7}));

			list1.splice(list1.begin(), list2, list2.begin());
			VERIFY(list1 == slist<int>({5,4,0,1,2,3}));
			VERIFY(list2 == slist<int>({6,7}));

			list1.splice(list1.begin(), list2, list2.begin());
			VERIFY(list1 == slist<int>({6,5,4,0,1,2,3}));
			VERIFY(list2 == slist<int>({7}));

			list1.splice(list1.begin(), list2, list2.begin());
			VERIFY(list1 == slist<int>({7,6,5,4,0,1,2,3}));
			VERIFY(list2 == slist<int>({}));

			VERIFY(list1.validate());
			VERIFY(list2.validate());
		}
	}

	// void splice(const_iterator position, this_type&& x);
	// void splice(const_iterator position, this_type&& x, const_iterator i);
	// void splice(const_iterator position, this_type&& x, const_iterator first, const_iterator last);
	{
		{
			slist<int> list1 = {0,1,2,3};
			slist<int> list2 = {4,5,6,7};	

			list1.splice(list1.begin(), eastl::move(list2));
			VERIFY(list1 == slist<int>({4,5,6,7,0,1,2,3}));
		}
		{
			slist<int> list1 = {0,1,2,3};
			slist<int> list2 = {4,5,6,7};	

			list1.splice(list1.begin(), eastl::move(list2), list2.begin());
			VERIFY(list1 == slist<int>({4,0,1,2,3}));
		}
		{
			slist<int> list1 = {0,1,2,3};
			slist<int> list2 = {4,5,6,7};	

			auto b = list2.begin();
			auto e = list2.end();
			e = list2.previous(e);
			e = list2.previous(e);

			list1.splice(list1.begin(), eastl::move(list2), b, e);
			VERIFY(list1 == slist<int>({4,5,0,1,2,3}));
		}
	}

	// void splice_after(const_iterator position, this_type& x);
	// void splice_after(const_iterator position, this_type& x, const_iterator i);
	// void splice_after(const_iterator position, this_type& x, const_iterator first, const_iterator last);
	{
		slist<int> list1 = {0,1,2,3};	
		slist<int> list2 = {4,5,6,7};	

		list1.splice_after(list1.begin(), list2);
		VERIFY(list1 == slist<int>({0,4,5,6,7,1,2,3}));
		VERIFY(list1.validate());
		VERIFY(list2.validate());
	}

	// void splice_after(const_iterator position, this_type&& x);
	// void splice_after(const_iterator position, this_type&& x, const_iterator i);
	// void splice_after(const_iterator position, this_type&& x, const_iterator first, const_iterator last);
	{
		{
			slist<int> list1 = {0,1,2,3};
			slist<int> list2 = {4,5,6,7};	

			list1.splice_after(list1.begin(), eastl::move(list2));
			VERIFY(list1 == slist<int>({0,4,5,6,7,1,2,3}));
		}
		{
			slist<int> list1 = {0,1,2,3};
			slist<int> list2 = {4,5,6,7};	

			list1.splice_after(list1.begin(), eastl::move(list2), list2.begin());
			VERIFY(list1 == slist<int>({0,5,6,7,1,2,3}));
		}
		{
			slist<int> list1 = {0,1,2,3};
			slist<int> list2 = {4,5,6,7};	

			auto b = list2.begin();
			auto e = list2.end();
			e = list2.previous(e);
			e = list2.previous(e);

			list1.splice_after(list1.begin(), eastl::move(list2), b, e);
			VERIFY(list1 == slist<int>({0,5,6,1,2,3}));
		}
	}

	// void sort();
	{
		slist<int> list1 = {0, 1, 2, 2, 2, 3, 4, 5, 6, 7, 8, 9, 9, 8, 7, 6, 5, 4, 3, 2, 2, 2, 1, 0};
		VERIFY(!eastl::is_sorted(eastl::begin(list1), eastl::end(list1)));
		VERIFY(list1.validate());

		list1.sort();

		VERIFY(eastl::is_sorted(eastl::begin(list1), eastl::end(list1)));
		VERIFY(list1.validate());
	}

	// template <class Compare>
	// void sort(Compare compare);
	{
		auto compare = [](int a, int b) { return a > b;};

		slist<int> list1 = {0, 1, 2, 2, 2, 3, 4, 5, 6, 7, 8, 9, 9, 8, 7, 6, 5, 4, 3, 2, 2, 2, 1, 0};
		VERIFY(!eastl::is_sorted(eastl::begin(list1), eastl::end(list1), compare));
		list1.sort(compare);
		VERIFY(eastl::is_sorted(eastl::begin(list1), eastl::end(list1), compare));
	}

	{ // Test empty base-class optimization
		struct UnemptyDummyAllocator : eastl::dummy_allocator
		{
			int foo;
		};

		typedef eastl::slist<int, eastl::dummy_allocator> list1;
		typedef eastl::slist<int, UnemptyDummyAllocator> list2;

		EATEST_VERIFY(sizeof(list1) < sizeof(list2));
	}

	{ // Test erase / erase_if
		{
			slist<int> l = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			eastl::erase(l, 5);
			VERIFY((l == slist<int>{0, 1, 2, 3, 4, 6, 7, 8, 9}));

			eastl::erase(l, 7);
			VERIFY((l == slist<int>{0, 1, 2, 3, 4, 6, 8, 9}));

			eastl::erase(l, 2);
			VERIFY((l == slist<int>{0, 1, 3, 4, 6, 8, 9}));

			eastl::erase(l, 0);
			VERIFY((l == slist<int>{1, 3, 4, 6, 8, 9}));

			eastl::erase(l, 4);
			VERIFY((l == slist<int>{1, 3, 6, 8, 9}));
		}

		{
			slist<int> l = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			eastl::erase_if(l, [](auto e) { return e % 2 == 0; });
			VERIFY((l == slist<int>{1, 3, 5, 7, 9}));

			eastl::erase_if(l, [](auto e) { return e == 5; });
			VERIFY((l == slist<int>{1, 3, 7, 9}));

			eastl::erase_if(l, [](auto e) { return e % 3 == 0; });
			VERIFY((l == slist<int>{1, 7}));
		}
	}

	return nErrorCount;
}

