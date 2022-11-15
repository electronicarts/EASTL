/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EASTL/list.h>
#include <EASTL/sort.h>
#include <EASTL/fixed_allocator.h>

using namespace eastl;

// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::list<bool>;
template class eastl::list<int>;
template class eastl::list<Align64>;
template class eastl::list<TestObject>;
// template class eastl::list<eastl::unique_ptr<int>>;


int TestList()
{
	int nErrorCount = 0;

	// list();
	{
		eastl::list<int> l;
		VERIFY(l.size() == 0);
		VERIFY(l.empty());
		VERIFY(l.validate());
		VERIFY(l.begin() == l.end());
	}

	// list(const allocator_type& allocator);
	{
		MallocAllocator::reset_all();
		MallocAllocator mallocator;
		{
			eastl::list<int, MallocAllocator> l(mallocator);
			VERIFY(l.get_allocator() == mallocator);
			l.push_front(42);
			VERIFY(MallocAllocator::mAllocCountAll != 0);
		}
		VERIFY(MallocAllocator::mAllocCountAll == MallocAllocator::mFreeCountAll);
	}

	// explicit list(size_type n, const allocator_type& allocator = EASTL_LIST_DEFAULT_ALLOCATOR);
	{
		const int test_size = 42;
		eastl::list<int> l(test_size);
		VERIFY(!l.empty());
		VERIFY(l.size() == test_size);
		VERIFY(l.validate());

		VERIFY(eastl::all_of(l.begin(), l.end(), [](int e)
		                     { return e == 0; }));
	}

	// list(size_type n, const value_type& value, const allocator_type& allocator = EASTL_LIST_DEFAULT_ALLOCATOR);
	{
		const int test_size = 42;
		const int test_val = 435;

		eastl::list<int> l(42, test_val);
		VERIFY(!l.empty());
		VERIFY(l.size() == test_size);
		VERIFY(l.validate());

		VERIFY(eastl::all_of(l.begin(), l.end(), [=](int e)
		                     { return e == test_val; }));
	}

	// list(const this_type& x);
	{
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> b(a); 
		VERIFY(a == b);
		VERIFY(a.validate());
		VERIFY(a.size() == b.size());
		VERIFY(b.validate());
	}

	// list(const this_type& x, const allocator_type& allocator);
	{
		MallocAllocator mallocator;
		eastl::list<int, MallocAllocator> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int, MallocAllocator> b(a, mallocator);
		VERIFY(a == b);
		VERIFY(a.validate());
		VERIFY(a.size() == b.size());
		VERIFY(b.validate());
		VERIFY(a.get_allocator() == b.get_allocator());
	}

	// list(this_type&& x);
	// list(this_type&&, const allocator_type&);
	{
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		VERIFY(!a.empty());
		VERIFY(a.size() == 10);
		VERIFY(a.validate());

		eastl::list<int> b(eastl::move(a)); 
		VERIFY(a.empty());
		VERIFY(!b.empty());
		VERIFY(a.size() == 0);
		VERIFY(b.size() == 10);

		VERIFY(a != b);
		VERIFY(a.size() != b.size());
		VERIFY(a.validate());
		VERIFY(b.validate());
	}

	// list(std::initializer_list<value_type> ilist, const allocator_type& allocator = EASTL_LIST_DEFAULT_ALLOCATOR);
	{
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::for_each(a.begin(), a.end(), [&](int e)
		                {
			                static int inc = 0;
			                VERIFY(inc++ == e);
			            });
	}

	// list(InputIterator first, InputIterator last); 
	{
		eastl::list<int> ref = {3, 4, 5, 6, 7};
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

		auto start = a.begin();
		eastl::advance(start, 3);

		auto end = start;
		eastl::advance(end, 5);

		eastl::list<int> b(start, end); 

		VERIFY(b == ref);
		VERIFY(a.validate());
		VERIFY(b.validate());

		VERIFY(a.size() == 10);
		VERIFY(b.size() == 5);		

		VERIFY(!b.empty());
		VERIFY(!a.empty());
	}

	// this_type& operator=(const this_type& x);
	// this_type& operator=(std::initializer_list<value_type> ilist);
	// this_type& operator=(this_type&& x);
	{
		const eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> b = a;
		VERIFY(a.validate());
		VERIFY(b.validate());
		VERIFY(a.size() == 10);
		VERIFY(b.size() == 10);		
		VERIFY(!a.empty());
		VERIFY(!b.empty());
		VERIFY(b == a);

		eastl::list<int> c = eastl::move(b);
		VERIFY(b.empty());

		VERIFY(c == a);
		VERIFY(c.size() == 10);
		VERIFY(c.validate());
	}

	// void swap(this_type& x);
	{
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> b = {};

		VERIFY(a.validate());
		VERIFY(b.validate());
		VERIFY(!a.empty());
		VERIFY(b.empty());

		b.swap(a);

		VERIFY(a.validate());
		VERIFY(b.validate());
		VERIFY(a.empty());
		VERIFY(!b.empty());
	}

	// void assign(size_type n, const value_type& value);
	{
		eastl::list<int> ref = {42, 42, 42, 42};
		eastl::list<int> a = {0, 1, 2, 3};
		a.assign(4, 42);
		VERIFY(a == ref);
		VERIFY(a.validate());
		VERIFY(!a.empty());
		VERIFY(a.size() == 4);
	}

	// void assign(InputIterator first, InputIterator last);  
	{
		eastl::list<int> ref = eastl::list<int>{3, 4, 5, 6, 7};
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> b;

		auto start = a.begin();
		eastl::advance(start, 3);

		auto end = start;
		eastl::advance(end, 5);

		b.assign(start, end); 

		VERIFY(b == ref);
		VERIFY(a.validate());
		VERIFY(b.validate());

		VERIFY(a.size() == 10);
		VERIFY(b.size() == 5);		

		VERIFY(!b.empty());
		VERIFY(!a.empty());
	}
																
	// void assign(std::initializer_list<value_type> ilist);
	{
		eastl::list<int> ref = eastl::list<int>{3, 4, 5, 6, 7};
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> b;

		auto start = a.begin();
		eastl::advance(start, 3);

		auto end = start;
		eastl::advance(end, 5);

		b.assign(start, end); 

		VERIFY(b == ref);
		VERIFY(a.validate());
		VERIFY(b.validate());

		VERIFY(a.size() == 10);
		VERIFY(b.size() == 5);		

		VERIFY(!b.empty());
		VERIFY(!a.empty());
	}

	// iterator       begin() 
	// const_iterator begin() const 
	// const_iterator cbegin() const 
	// iterator       end() 
	// const_iterator end() const 
	// const_iterator cend() const 
	{
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

		{
			static int inc = 0;
			auto iter = a.begin();
			while(iter != a.end())
			{
				VERIFY(*iter++ == inc++);				
			}
		}

		{
			static int inc = 0;
			auto iter = a.cbegin();
			while(iter != a.cend())
			{
				VERIFY(*iter++ == inc++);
			}
		}
	}

	// reverse_iterator       rbegin()
	// const_reverse_iterator rbegin() const
	// const_reverse_iterator crbegin() const
	// reverse_iterator       rend()
	// const_reverse_iterator rend() const
	// const_reverse_iterator crend() const 
	{
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

		{
			static int inc = 9;
			auto iter = a.rbegin();
			while(iter != a.rend())
			{
				VERIFY(*iter == inc--);
				iter++;
			}
		}

		{
			static int inc = 9;
			auto iter = a.crbegin();
			while(iter != a.crend())
			{
				VERIFY(*iter == inc--);
				iter++;
			}
		}
	}

	// bool empty() const 
	{
		{
			eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
			VERIFY(!a.empty());
		}

		{
			eastl::list<int> a = {};
			VERIFY(a.empty());
		}
	}

	// size_type size() const
	{
		{
			eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
			VERIFY(a.size() == 10);
		}

		{
			eastl::list<int> a = {0, 1, 2, 3, 4};
			VERIFY(a.size() == 5);
		}

		{
			eastl::list<int> a = {0, 1};
			VERIFY(a.size() == 2);
		}

		{
			eastl::list<int> a = {};
			VERIFY(a.size() == 0);
		}
	}

	// void resize(size_type n, const value_type& value);
	// void resize(size_type n);
	{
		{
			eastl::list<int> a;
			a.resize(10);
			VERIFY(a.size() == 10);
			VERIFY(!a.empty());
			VERIFY(eastl::all_of(a.begin(), a.end(), [](int i)
			                     { return i == 0; }));
		}

		{
			eastl::list<int> a;
			a.resize(10, 42);
			VERIFY(a.size() == 10);
			VERIFY(!a.empty());
			VERIFY(eastl::all_of(a.begin(), a.end(), [](int i)
			                     { return i == 42; }));
		}
	}

	// reference       front();
	// const_reference front() const;
	{
		{
			eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
			VERIFY(a.front() == 0);

			a.front() = 42;
			VERIFY(a.front() == 42);
		}

		{
			const eastl::list<int> a = {5, 6, 7, 8, 9};
			VERIFY(a.front() == 5);
		}

		{
			eastl::list<int> a = {9};
			VERIFY(a.front() == 9);

			a.front() = 42;
			VERIFY(a.front() == 42);
		}
	}

	// reference       back();
	// const_reference back() const;
	{
		{
			eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
			VERIFY(a.back() == 9);

			a.back() = 42;
			VERIFY(a.back() == 42);
		}

		{
			const eastl::list<int> a = {5, 6, 7, 8, 9};
			VERIFY(a.back() == 9);
		}

		{
			eastl::list<int> a = {9};
			VERIFY(a.back() == 9);

			a.back() = 42;
			VERIFY(a.back() == 42);
		}
	}

	// void emplace_front(Args&&... args);
	// void emplace_front(value_type&& value);
	// void emplace_front(const value_type& value);
	{
		eastl::list<int> ref = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
		eastl::list<int> a;

		for(int i = 0; i < 10; i++)
			a.emplace_front(i);

		VERIFY(a == ref);
	}

	// template <typename... Args>
	// void emplace_back(Args&&... args);
	// void emplace_back(value_type&& value);
	// void emplace_back(const value_type& value);
	{
		{
			eastl::list<int> ref = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
			eastl::list<int> a;

			for(int i = 0; i < 10; i++)
				a.emplace_back(i);

			VERIFY(a == ref);
		}

		{
			struct A
			{
				A() : mValue(0) {}
				A(int in) : mValue(in) {}
				int mValue;
				bool operator==(const A& other) const { return mValue == other.mValue; }
			};

			{
				eastl::list<A> ref = {{1}, {2}, {3}};
				eastl::list<A> a;

				a.emplace_back(1);
				a.emplace_back(2);
				a.emplace_back(3);

				VERIFY(a == ref);
			}

			{
				eastl::list<A> ref = {{1}, {2}, {3}};
				eastl::list<A> a;

				a.emplace_back(A(1));
				a.emplace_back(A(2));
				a.emplace_back(A(3));

				VERIFY(a == ref);
			}


			{
				eastl::list<A> ref = {{1}, {2}, {3}};
				eastl::list<A> a;

				A a1(1);
				A a2(2);
				A a3(3);

				a.emplace_back(a1);
				a.emplace_back(a2);
				a.emplace_back(a3);

				VERIFY(a == ref);
			}
		}
	}

	// void push_front(const value_type& value);
	// void push_front(value_type&& x);
	// reference push_front();
	{
		{
			eastl::list<int> ref = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
			eastl::list<int> a;

			for(int i = 0; i < 10; i++)
				a.push_front(i);

			VERIFY(a == ref);

		}

		{
			eastl::list<int> a;
			auto& front_ref = a.push_front();
			front_ref = 42;
			VERIFY(a.front() == 42);
		}
	}

	// void* push_front_uninitialized();
	{
		eastl::list<int> a;
		for (unsigned i = 0; i < 100; i++)
		{
			VERIFY(a.push_front_uninitialized() != nullptr);
			VERIFY(a.size() == (i + 1));
		}
	}

	// void push_back(const value_type& value);
	// void push_back(value_type&& x);
	{
		{
			eastl::list<int> ref = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
			eastl::list<int> a;

			for(int i = 0; i < 10; i++)
				a.push_back(i);

			VERIFY(a == ref);
		}

		{
			struct A { int mValue; };
			eastl::list<A> a;
			a.push_back(A{42});
			VERIFY(a.back().mValue == 42);
		}
	}

	// reference push_back();
	{
		eastl::list<int> a;
		auto& back_ref = a.push_back();
		back_ref = 42;
		VERIFY(a.back() == 42);
	}

	// void* push_back_uninitialized();
	{
		eastl::list<int> a;
		for (unsigned int i = 0; i < 100; i++)
		{
			VERIFY(a.push_back_uninitialized() != nullptr);
			VERIFY(a.size() == (i + 1));
		}
	}

	// void pop_front();
	{
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		for(unsigned i = 0; i < 10; i++)
		{
			VERIFY(unsigned(a.front()) == i);
			a.pop_front();
		}
	}

	// void pop_back();
	{
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		for(unsigned i = 0; i < 10; i++)
		{
			VERIFY(unsigned(a.back()) == (9 - i));
			a.pop_back();
		}
	}

	// iterator emplace(const_iterator position, Args&&... args);
	// iterator emplace(const_iterator position, value_type&& value);
	// iterator emplace(const_iterator position, const value_type& value);
	{
		eastl::list<int> ref = {0, 1, 2, 3, 4, 42, 5, 6, 7, 8, 9};
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

		auto insert_pos = a.begin();
		eastl::advance(insert_pos, 5);

		a.emplace(insert_pos, 42);
		VERIFY(a == ref);
	}

	// iterator insert(const_iterator position);
	// iterator insert(const_iterator position, const value_type& value);
	// iterator insert(const_iterator position, value_type&& x);
	{
		eastl::list<int> ref = {0, 1, 2, 3, 4, 42, 5, 6, 7, 8, 9};
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

		auto insert_pos = a.begin();
		eastl::advance(insert_pos, 5);

		a.insert(insert_pos, 42);
		VERIFY(a == ref);
	}

	// void insert(const_iterator position, size_type n, const value_type& value);
	{
		eastl::list<int> ref = {0, 1, 2, 3, 4, 42, 42, 42, 42, 5, 6, 7, 8, 9};
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

		auto insert_pos = a.begin();
		eastl::advance(insert_pos, 5);

		auto result = a.insert(insert_pos, 4, 42);
		VERIFY(a == ref);
		VERIFY(*result == 42);
		VERIFY(*(--result) == 4);
	}

	// void insert(const_iterator position, InputIterator first, InputIterator last);
	{
		eastl::list<int> to_insert = {42, 42, 42, 42};
		eastl::list<int> ref = {0, 1, 2, 3, 4, 42, 42, 42, 42, 5, 6, 7, 8, 9};
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

		auto insert_pos = a.begin();
		eastl::advance(insert_pos, 5);

		auto result = a.insert(insert_pos, to_insert.begin(), to_insert.end());
		VERIFY(a == ref);
		VERIFY(*result == 42);
		VERIFY(*(--result) == 4);
	}

	// iterator insert(const_iterator position, std::initializer_list<value_type> ilist);
	{
		eastl::list<int> ref = {0, 1, 2, 3, 4, 42, 42, 42, 42, 5, 6, 7, 8, 9};
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

		auto insert_pos = a.begin();
		eastl::advance(insert_pos, 5);

		a.insert(insert_pos, {42, 42, 42, 42});
		VERIFY(a == ref);
	}

	// iterator erase(const_iterator position);
	{
		eastl::list<int> ref = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> a = {0, 1, 2, 3, 4, 42, 5, 6, 7, 8, 9};

		auto erase_pos = a.begin();
		eastl::advance(erase_pos, 5);

		auto iter_after_removed = a.erase(erase_pos);
		VERIFY(*iter_after_removed == 5);
		VERIFY(a == ref);
	}

	// iterator erase(const_iterator first, const_iterator last);
	{
		eastl::list<int> a = {0, 1, 2, 3, 4, 42, 42, 42, 42, 5, 6, 7, 8, 9};
		eastl::list<int> ref = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

		auto erase_begin = a.begin();
		eastl::advance(erase_begin, 5);

		auto erase_end = erase_begin;
		eastl::advance(erase_end, 4);

		a.erase(erase_begin, erase_end);
		VERIFY(a == ref);
	}

	// reverse_iterator erase(const_reverse_iterator position);
	{
		eastl::list<int> a = {0, 1, 2, 3, 4, 42, 5, 6, 7, 8, 9};
		eastl::list<int> ref = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

		auto erase_rbegin = a.rbegin();
		eastl::advance(erase_rbegin, 5);

		auto iter_after_remove = a.erase(erase_rbegin);
		VERIFY(*iter_after_remove == 4);
		VERIFY(a == ref);

	}

	// reverse_iterator erase(const_reverse_iterator first, const_reverse_iterator last);
	{
		eastl::list<int> a = {0, 1, 2, 3, 4, 42, 42, 42, 42, 5, 6, 7, 8, 9};
		eastl::list<int> ref = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

		auto erase_crbegin = a.crbegin();
		auto erase_crend = a.crbegin();
		eastl::advance(erase_crbegin, 4);
		eastl::advance(erase_crend, 8);

		auto iter_after_removed = a.erase(erase_crbegin, erase_crend);
		VERIFY(*iter_after_removed == 4);
		VERIFY(a == ref);
	}

	// void clear()
	{
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		a.clear();
		VERIFY(a.empty());
		VERIFY(a.size() == 0);
	}

	// void reset_lose_memory()    
	{
		typedef eastl::list<int, fixed_allocator> IntList;
		typedef IntList::node_type                IntListNode;
		const size_t  kBufferCount = 10;
		IntListNode   buffer1[kBufferCount];
		IntList       intList1;
		const size_t  kAlignOfIntListNode = EA_ALIGN_OF(IntListNode);
		intList1.get_allocator().init(buffer1, sizeof(buffer1), sizeof(IntListNode), kAlignOfIntListNode);

		intList1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		VERIFY(!intList1.empty());
		VERIFY(intList1.size() == 10);
		intList1.reset_lose_memory();
		VERIFY(intList1.empty());
		VERIFY(intList1.size() == 0);
	}

	// void remove(const T& x);
	{
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> ref = {0, 1, 2, 3, 5, 6, 7, 8, 9};
		a.remove(4);
		VERIFY(a == ref);
	}

	// void remove_if(Predicate);
	{
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> ref = {0, 1, 2, 3, 5, 6, 7, 8, 9};
		a.remove_if([](int e) { return e == 4; });
		VERIFY(a == ref);
	}

	// void reverse()
	{
		eastl::list<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> ref = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
		a.reverse();
		VERIFY(a == ref);
	}

	// void splice(const_iterator position, this_type& x);
	{
		const eastl::list<int> ref = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> a1 = {0, 1, 2, 3, 4};
		eastl::list<int> a2 = {5, 6, 7, 8, 9};

		eastl::list<int> a;
		a.splice(a.begin(), a2);
		a.splice(a.begin(), a1);

		VERIFY(a == ref);
		VERIFY(a1.empty());
		VERIFY(a2.empty());
	}

	// void splice(const_iterator position, this_type& x, const_iterator i);
	{
		const eastl::list<int> ref = {0, 5};
		eastl::list<int> a1 = {-1, -1, 0};
		eastl::list<int> a2 = {-1, -1, 5};

		auto a1_begin = a1.begin();
		auto a2_begin = a2.begin();

		eastl::advance(a1_begin, 2);
		eastl::advance(a2_begin, 2);

		eastl::list<int> a;
		a.splice(a.begin(), a2, a2_begin);
		a.splice(a.begin(), a1, a1_begin);

		VERIFY(a == ref);
		VERIFY(!a1.empty());
		VERIFY(!a2.empty());
	}

	// void splice(const_iterator position, this_type& x, const_iterator first, const_iterator last);
	{
		const eastl::list<int> ref = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> a1 = {-1, -1, 0, 1, 2, 3, 4, -1, -1};
		eastl::list<int> a2 = {-1, -1, 5, 6, 7, 8, 9, -1, -1};

		auto a1_begin = a1.begin();
		auto a2_begin = a2.begin();
		auto a1_end = a1.end();
		auto a2_end = a2.end();

		eastl::advance(a1_begin, 2);
		eastl::advance(a2_begin, 2);
		eastl::advance(a1_end, -2);
		eastl::advance(a2_end, -2);

		eastl::list<int> a;
		a.splice(a.begin(), a2, a2_begin, a2_end);
		a.splice(a.begin(), a1, a1_begin, a1_end);

		const eastl::list<int> rref = {-1, -1, -1, -1};  // post splice reference list
		VERIFY(a == ref);
		VERIFY(a1 == rref);
		VERIFY(a2 == rref);
	}

	// void splice(const_iterator position, this_type&& x);
	{
		const eastl::list<int> ref = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> a1 = {0, 1, 2, 3, 4};
		eastl::list<int> a2 = {5, 6, 7, 8, 9};

		eastl::list<int> a;
		a.splice(a.begin(), eastl::move(a2));
		a.splice(a.begin(), eastl::move(a1));

		VERIFY(a == ref);
		VERIFY(a1.empty());
		VERIFY(a2.empty());
	}

	// void splice(const_iterator position, this_type&& x, const_iterator i);
	{
		const eastl::list<int> ref = {0, 5};
		eastl::list<int> a1 = {-1, -1, 0};
		eastl::list<int> a2 = {-1, -1, 5};

		auto a1_begin = a1.begin();
		auto a2_begin = a2.begin();

		eastl::advance(a1_begin, 2);
		eastl::advance(a2_begin, 2);

		eastl::list<int> a;
		a.splice(a.begin(), eastl::move(a2), a2_begin);
		a.splice(a.begin(), eastl::move(a1), a1_begin);

		VERIFY(a == ref);
		VERIFY(!a1.empty());
		VERIFY(!a2.empty());
	}

	// void splice(const_iterator position, this_type&& x, const_iterator first, const_iterator last);
	{
		const eastl::list<int> ref = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> a1 = {-1, -1, 0, 1, 2, 3, 4, -1, -1};
		eastl::list<int> a2 = {-1, -1, 5, 6, 7, 8, 9, -1, -1};

		auto a1_begin = a1.begin();
		auto a2_begin = a2.begin();
		auto a1_end = a1.end();
		auto a2_end = a2.end();

		eastl::advance(a1_begin, 2);
		eastl::advance(a2_begin, 2);
		eastl::advance(a1_end, -2);
		eastl::advance(a2_end, -2);

		eastl::list<int> a;
		a.splice(a.begin(), eastl::move(a2), a2_begin, a2_end);
		a.splice(a.begin(), eastl::move(a1), a1_begin, a1_end);

		const eastl::list<int> rref = {-1, -1, -1, -1};  // post splice reference list
		VERIFY(a == ref);
		VERIFY(a1 == rref);
		VERIFY(a2 == rref);
	}


	// void merge(this_type& x);
	// void merge(this_type&& x);
	// void merge(this_type& x, Compare compare);
	{
		eastl::list<int> ref = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> a1 = {0, 1, 2, 3, 4};
		eastl::list<int> a2 = {5, 6, 7, 8, 9};
		a1.merge(a2);
		VERIFY(a1 == ref);
	}

	// void merge(this_type&& x, Compare compare);
	{
		eastl::list<int> ref = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> a1 = {0, 1, 2, 3, 4};
		eastl::list<int> a2 = {5, 6, 7, 8, 9};
		a1.merge(a2, [](int lhs, int rhs) { return lhs < rhs; });
		VERIFY(a1 == ref);
	}

	// void unique();
	{
		eastl::list<int> ref = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> a = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 3, 3, 3,
		                      4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 7, 8, 9, 9, 9, 9, 9, 9, 9, 9};
		a.unique();
		VERIFY(a == ref);
	}

	// void unique(BinaryPredicate);
	{
		static bool bBreakComparison;
		struct A
		{
			int mValue;
			bool operator==(const A& other) const { return bBreakComparison ? false : mValue == other.mValue; }
		};

		eastl::list<A> ref = {{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9}};
		eastl::list<A> a = {{0}, {0}, {0}, {0}, {0}, {0}, {1}, {2}, {2}, {2}, {2}, {3}, {4}, {5},
		                    {5}, {5}, {5}, {5}, {6}, {7}, {7}, {7}, {7}, {8}, {9}, {9}, {9}};

		bBreakComparison = true;
		a.unique(); // noop because broken comparison operator
		VERIFY(a != ref);  

		a.unique([](const A& lhs, const A& rhs) { return lhs.mValue == rhs.mValue; });

		bBreakComparison = false;
		VERIFY(a == ref);
	}

	// void sort();
	{
		eastl::list<int> ref = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::list<int> a =   {9, 4, 5, 3, 1, 0, 6, 2, 7, 8};

		a.sort();
		VERIFY(a == ref);
	}

	// void sort(Compare compare);
	{
		struct A
		{
			int mValue;
			bool operator==(const A& other) const { return mValue == other.mValue; }
		};

		eastl::list<A> ref = {{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9}};
		eastl::list<A> a = {{1}, {0}, {2}, {9}, {4}, {5}, {6}, {7}, {3}, {8}};

		a.sort([](const A& lhs, const A& rhs) { return lhs.mValue < rhs.mValue; });
		VERIFY(a == ref);
	}

	{ // Test empty base-class optimization
		struct UnemptyDummyAllocator : eastl::dummy_allocator
		{
			int foo;
		};

		typedef eastl::list<int, eastl::dummy_allocator> list1;
		typedef eastl::list<int, UnemptyDummyAllocator> list2;

		EATEST_VERIFY(sizeof(list1) < sizeof(list2));
	}

	{ // Test erase / erase_if
		{
			eastl::list<int> l = {1, 2, 3, 4, 5, 6, 7, 8, 9};

			auto numErased = eastl::erase(l, 3);
		    VERIFY(numErased == 1);
			numErased = eastl::erase(l, 5);
		    VERIFY(numErased == 1);
			numErased = eastl::erase(l, 7);
		    VERIFY(numErased == 1);

			VERIFY((l == eastl::list<int>{1, 2, 4, 6, 8, 9}));
		}

		{
			eastl::list<int> l = {1, 2, 3, 4, 5, 6, 7, 8, 9};
			auto numErased = eastl::erase_if(l, [](auto i) { return i % 2 == 0; });
			VERIFY((l == eastl::list<int>{1, 3, 5, 7, 9}));
		    VERIFY(numErased == 4);
		}
	}

	{ // Test global operators
	    {
		    eastl::list<int> list1 = {0, 1, 2, 3, 4, 5};
		    eastl::list<int> list2 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		    eastl::list<int> list3 = {5, 6, 7, 8};

			VERIFY(list1 == list1);
		    VERIFY(!(list1 != list1));

			VERIFY(list1 != list2);
		    VERIFY(list2 != list3);
		    VERIFY(list1 != list3);

			VERIFY(list1 < list2);
		    VERIFY(list1 <= list2);

			VERIFY(list2 > list1);
		    VERIFY(list2 >= list1);
			
			VERIFY(list3 > list1);
		    VERIFY(list3 > list2);
		}

		// three way comparison operator
#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
		{
			eastl::list<int> list1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
			eastl::list<int> list2 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			// Verify equality between list1 and list2
			VERIFY((list1 <=> list2) == 0);
			VERIFY(!((list1 <=> list2) != 0));
			VERIFY((list1 <=> list2) <= 0);
			VERIFY((list1 <=> list2) >= 0);
			VERIFY(!((list1 <=> list2) < 0));
			VERIFY(!((list1 <=> list2) > 0));

			list1.push_back(100); // Make list1 less than list2.
			list2.push_back(101);

			// Verify list1 < list2
			VERIFY(!((list1 <=> list2) == 0));
			VERIFY((list1 <=> list2) != 0);
			VERIFY((list1 <=> list2) <= 0);
			VERIFY(!((list1 <=> list2) >= 0));
			VERIFY(((list1 <=> list2) < 0));
			VERIFY(!((list1 <=> list2) > 0));

			for (int i = 0; i < 3; i++) // Make the length of list2 less than list1
				list2.pop_back();

			// Verify list2.size() < list1.size() and list2 is a subset of list1
			VERIFY(!((list1 <=> list2) == 0));
			VERIFY((list1 <=> list2) != 0);
			VERIFY((list1 <=> list2) >= 0);
			VERIFY(!((list1 <=> list2) <= 0));
			VERIFY(((list1 <=> list2) > 0));
			VERIFY(!((list1 <=> list2) < 0));
		}

		{
			eastl::list<int> list1 = {1, 2, 3, 4, 5, 6, 7};
			eastl::list<int> list2 = {7, 6, 5, 4, 3, 2, 1};
			eastl::list<int> list3 = {1, 2, 3, 4};

			struct weak_ordering_list
			{
				eastl::list<int> list;
				inline std::weak_ordering operator<=>(const weak_ordering_list& b) const { return list <=> b.list; }
			};

			VERIFY(synth_three_way{}(weak_ordering_list{list1}, weak_ordering_list{list2}) == std::weak_ordering::less);
			VERIFY(synth_three_way{}(weak_ordering_list{list3}, weak_ordering_list{list1}) == std::weak_ordering::less);
			VERIFY(synth_three_way{}(weak_ordering_list{list2}, weak_ordering_list{list1}) == std::weak_ordering::greater);
			VERIFY(synth_three_way{}(weak_ordering_list{list2}, weak_ordering_list{list3}) == std::weak_ordering::greater);
			VERIFY(synth_three_way{}(weak_ordering_list{list1}, weak_ordering_list{list1}) == std::weak_ordering::equivalent);

			struct strong_ordering_list
			{
				eastl::list<int> list;
				inline std::strong_ordering operator<=>(const strong_ordering_list& b) const { return list <=> b.list; }
			};

			VERIFY(synth_three_way{}(strong_ordering_list{list1}, strong_ordering_list{list2}) == std::strong_ordering::less);
			VERIFY(synth_three_way{}(strong_ordering_list{list3}, strong_ordering_list{list1}) == std::strong_ordering::less);
			VERIFY(synth_three_way{}(strong_ordering_list{list2}, strong_ordering_list{list1}) == std::strong_ordering::greater);
			VERIFY(synth_three_way{}(strong_ordering_list{list2}, strong_ordering_list{list3}) == std::strong_ordering::greater);
			VERIFY(synth_three_way{}(strong_ordering_list{list1}, strong_ordering_list{list1}) == std::strong_ordering::equal);
		}
#endif
	}
	return nErrorCount;
}


