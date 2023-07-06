/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/bonus/ring_buffer.h>
#include <EASTL/bonus/fixed_ring_buffer.h>
#include <EASTL/vector.h>
#include <EASTL/deque.h>
#include <EASTL/string.h>
#include <EASTL/list.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/fixed_string.h>



using namespace eastl;


// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::ring_buffer< int,        eastl::vector<int>        >;
template class eastl::ring_buffer< Align64,    eastl::vector<Align64>    >;
template class eastl::ring_buffer< TestObject, eastl::vector<TestObject> >;

template class eastl::ring_buffer< int,        eastl::deque<int>        >;
template class eastl::ring_buffer< Align64,    eastl::deque<Align64>    >;
template class eastl::ring_buffer< TestObject, eastl::deque<TestObject> >;

template class eastl::ring_buffer< int,        eastl::list<int>        >;
template class eastl::ring_buffer< Align64,    eastl::list<Align64>    >;
template class eastl::ring_buffer< TestObject, eastl::list<TestObject> >;

// TODO(rparolin):  To consider adding support for eastl::array. 
// template class eastl::ring_buffer< int, eastl::array<int, 64>>;

typedef eastl::fixed_string<char, 256, false>          RBFixedString;
typedef eastl::fixed_vector<RBFixedString, 100, false> RBFixedStringVector;
typedef RBFixedStringVector::overflow_allocator_type   RBFixedStringVectorOverflowAllocator;
template class eastl::ring_buffer<RBFixedString, RBFixedStringVector, RBFixedStringVectorOverflowAllocator>;

typedef eastl::fixed_vector<int, 100, false> RBFixedIntVector;
template class eastl::ring_buffer<int, RBFixedIntVector, RBFixedIntVector::overflow_allocator_type>;
// template class eastl::ring_buffer<int, RBFixedIntVector>;  // currently fails to compile

typedef eastl::fixed_vector<int, 100> RBFixedIntVectorWithOverFlow;
template class eastl::ring_buffer<int, RBFixedIntVectorWithOverFlow, RBFixedIntVectorWithOverFlow::overflow_allocator_type>; 
// template class eastl::ring_buffer<int, RBFixedIntVectorWithOverFlow>; // currently fails to compile



int TestRingBuffer()
{
	int nErrorCount = 0;

	// GCC prior to 4.1 has a fatal code generation bug in string arrays, which we use below.
	#if !defined(EA_DEBUG) && defined(__GNUC__) && !defined(__EDG__) && (((__GNUC__ * 100) + __GNUC_MINOR__) < 401)
		return nErrorCount;
	#endif

	{ // regression for bug in the capacity() function for the case of capacity == 0.

		vector<int> emptyIntArray;
		ring_buffer<int, vector<int> > intRingBuffer(emptyIntArray);

		EATEST_VERIFY(intRingBuffer.validate());
		EATEST_VERIFY(intRingBuffer.capacity() == 0);

		intRingBuffer.resize(0);
		EATEST_VERIFY(intRingBuffer.validate());
		EATEST_VERIFY(intRingBuffer.size() == 0);

		intRingBuffer.resize(1);
		EATEST_VERIFY(intRingBuffer.validate());
		EATEST_VERIFY(intRingBuffer.size() == 1);
	}

	{
		EA::UnitTest::Rand rng(EA::UnitTest::GetRandSeed());

		typedef ring_buffer< string, vector<string> > RBVectorString;

		int  counter = 0;
		char counterBuffer[32];

		// explicit ring_buffer(size_type size = 0);
		const int kOriginalCapacity = 50;
		RBVectorString rbVectorString(50);

		// bool empty() const;
		// size_type size() const;
		// bool validate() const;
		EATEST_VERIFY(rbVectorString.validate());
		EATEST_VERIFY(rbVectorString.empty());
		EATEST_VERIFY(rbVectorString.size() == 0);
		EATEST_VERIFY(rbVectorString.capacity() == 50);

		// void clear();
		rbVectorString.clear();
		EATEST_VERIFY(rbVectorString.validate());
		EATEST_VERIFY(rbVectorString.empty());
		EATEST_VERIFY(rbVectorString.size() == 0);
		EATEST_VERIFY(rbVectorString.capacity() == 50);

		// container_type& get_container();
		RBVectorString::container_type& c = rbVectorString.get_container();
		EATEST_VERIFY(c.size() == (kOriginalCapacity + 1)); // We need to add one because the ring_buffer mEnd is necessarily an unused element.

		// iterator begin();
		// iterator end();
		// int validate_iterator(const_iterator i) const;
		RBVectorString::iterator it = rbVectorString.begin();
		EATEST_VERIFY(rbVectorString.validate_iterator(it) == (isf_valid | isf_current));

		while(it != rbVectorString.end()) // This loop should do nothing.
		{
			EATEST_VERIFY(rbVectorString.validate_iterator(it) == (isf_valid | isf_current));
			++it;
		}

		// void push_back(const value_type& value);
		sprintf(counterBuffer, "%d", counter++);
		rbVectorString.push_back(string(counterBuffer));
		EATEST_VERIFY(rbVectorString.validate());
		EATEST_VERIFY(!rbVectorString.empty());
		EATEST_VERIFY(rbVectorString.size() == 1);
		EATEST_VERIFY(rbVectorString.capacity() == 50);

		it = rbVectorString.begin();
		EATEST_VERIFY(rbVectorString.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));
		EATEST_VERIFY(*it == "0");

		// reference front();
		// reference back();              
		string& sFront = rbVectorString.front();
		string& sBack  = rbVectorString.back();
		EATEST_VERIFY(&sFront == &sBack);

		// void push_back();
		string& ref = rbVectorString.push_back();
		EATEST_VERIFY(rbVectorString.validate());
		EATEST_VERIFY(rbVectorString.size() == 2);
		EATEST_VERIFY(rbVectorString.capacity() == 50);
		EATEST_VERIFY(&ref == &rbVectorString.back());

		it = rbVectorString.begin();
		++it;
		EATEST_VERIFY(rbVectorString.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));
		EATEST_VERIFY(it->empty());

		sprintf(counterBuffer, "%d", counter++);
		*it = counterBuffer;
		EATEST_VERIFY(*it == "1");

		++it;
		EATEST_VERIFY(it == rbVectorString.end());

		it = rbVectorString.begin();
		while(it != rbVectorString.end())
		{
			EATEST_VERIFY(rbVectorString.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));
			++it;
		}

		// reference operator[](size_type n);
		string& s0 = rbVectorString[0];
		EATEST_VERIFY(s0 == "0");

		string& s1 = rbVectorString[1];
		EATEST_VERIFY(s1 == "1");

		// Now we start hammering the ring buffer with push_back.
		for(eastl_size_t i = 0, iEnd = rbVectorString.capacity() * 5; i != iEnd; i++)
		{
			sprintf(counterBuffer, "%d", counter++);
			rbVectorString.push_back(string(counterBuffer));
			EATEST_VERIFY(rbVectorString.validate());
		}

		int  counterCheck = counter - 1;
		char counterCheckBuffer[32];
		sprintf(counterCheckBuffer, "%d", counterCheck);
		EATEST_VERIFY(rbVectorString.back() == counterCheckBuffer);

		// reverse_iterator rbegin();
		// reverse_iterator rend();
		for(RBVectorString::reverse_iterator ri = rbVectorString.rbegin(); ri != rbVectorString.rend(); ++ri)
		{
			sprintf(counterCheckBuffer, "%d", counterCheck--);
			EATEST_VERIFY(*ri == counterCheckBuffer);
		}

		++counterCheck;

		// iterator begin();
		// iterator end();
		for(RBVectorString::iterator i = rbVectorString.begin(); i != rbVectorString.end(); ++i)
		{
			EATEST_VERIFY(rbVectorString.validate_iterator(i) == (isf_valid | isf_current | isf_can_dereference));
			EATEST_VERIFY(*i == counterCheckBuffer);
			sprintf(counterCheckBuffer, "%d", ++counterCheck);
		}

		// void clear();
		rbVectorString.clear();
		EATEST_VERIFY(rbVectorString.validate());
		EATEST_VERIFY(rbVectorString.empty());
		EATEST_VERIFY(rbVectorString.size() == 0);
		EATEST_VERIFY(rbVectorString.capacity() == 50);

		// Random operations
		// Not easy to test the expected values without some tedium.
		for(int j = 0; j < 10000 + (gEASTL_TestLevel * 10000); j++)
		{
			sprintf(counterBuffer, "%d", counter++);

			const eastl_size_t op = rng.RandLimit(12);
			const eastl_size_t s  = rbVectorString.size();

			if(op == 0)
			{
				// void push_back(const value_type& value);

				rbVectorString.push_back(string(counterBuffer));
				EATEST_VERIFY(rbVectorString.size() == eastl::min(s + 1, rbVectorString.capacity()));
			}
			else if(op == 1)
			{
				// void push_back();

				string& ref2 = rbVectorString.push_back();
				rbVectorString.back() = string(counterBuffer);
				EATEST_VERIFY(rbVectorString.size() == eastl::min(s + 1, rbVectorString.capacity()));
				EATEST_VERIFY(&ref2 == &rbVectorString.back());
			}
			else if(op == 2)
			{
				// void pop_back();

				if(!rbVectorString.empty())
				{
					rbVectorString.pop_back();
					EATEST_VERIFY(rbVectorString.size() == (s - 1));
				}
			}
			else if(op == 3)
			{
				// void push_front(const value_type& value);

				rbVectorString.push_front(string(counterBuffer));
				EATEST_VERIFY(rbVectorString.size() == eastl::min(s + 1, rbVectorString.capacity()));
			}
			else if(op == 4)
			{
				// void push_front();

				string& ref2 = rbVectorString.push_front();
				rbVectorString.front() = string(counterBuffer);
				EATEST_VERIFY(rbVectorString.size() == eastl::min(s + 1, rbVectorString.capacity()));
				EATEST_VERIFY(&ref2 == &rbVectorString.front());
			}
			else if(op == 5)
			{
				// void pop_front();

				if(!rbVectorString.empty())
				{
					rbVectorString.pop_front();
					EATEST_VERIFY(rbVectorString.size() == (s - 1));
				}
			}
			else if(op == 6)
			{
				// iterator insert(iterator position, const value_type& value);

				it = rbVectorString.begin();
				const eastl_size_t dist = rng.RandLimit((uint32_t)s + 1);
				eastl::advance(it, dist);

				if(it == rbVectorString.end())
					EATEST_VERIFY(rbVectorString.validate_iterator(it) == (isf_valid | isf_current));
				else
					EATEST_VERIFY(rbVectorString.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));

				rbVectorString.insert(it, string(counterBuffer));
				EATEST_VERIFY(rbVectorString.size() == eastl::min(s + 1, rbVectorString.capacity()));
			}
			else if(op == 7)
			{
				// void insert(iterator position, size_type n, const value_type& value);

				it = rbVectorString.begin();
				const eastl_size_t dist = rng.RandLimit((uint32_t)s + 1);
				eastl::advance(it, dist);

				if(it == rbVectorString.end())
					EATEST_VERIFY(rbVectorString.validate_iterator(it) == (isf_valid | isf_current));
				else
					EATEST_VERIFY(rbVectorString.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));

				const eastl_size_t count = (eastl_size_t)rng.RandLimit(10);

				rbVectorString.insert(it, count, string(counterBuffer));
				EATEST_VERIFY(rbVectorString.size() == eastl::min(s + count, rbVectorString.capacity()));
			}
			else if(op == 8)
			{
				// template <typename InputIterator>
				// void insert(iterator position, InputIterator first, InputIterator last);

				string stringArray[10];

				it = rbVectorString.begin();
				const eastl_size_t dist = rng.RandLimit((uint32_t)s + 1);
				eastl::advance(it, dist);

				if(it == rbVectorString.end())
					EATEST_VERIFY(rbVectorString.validate_iterator(it) == (isf_valid | isf_current));
				else
					EATEST_VERIFY(rbVectorString.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));

				const eastl_size_t count = (eastl_size_t)rng.RandLimit(10);

				rbVectorString.insert(it, stringArray, stringArray + count);
				EATEST_VERIFY(rbVectorString.size() == eastl::min(s + count, rbVectorString.capacity()));
			}
			else if(op == 9)
			{
				// iterator erase(iterator position);

				if(!rbVectorString.empty())
				{
					it = rbVectorString.begin();
					const eastl_size_t dist = rng.RandLimit((uint32_t)s);
					eastl::advance(it, dist);
					EATEST_VERIFY(rbVectorString.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));
					rbVectorString.erase(it);

					EATEST_VERIFY(rbVectorString.size() == (s  - 1));
				}
			}
			else if(op == 10)
			{
				// iterator erase(iterator first, iterator last);

				if(!rbVectorString.empty())
				{
					RBVectorString::iterator it1 = rbVectorString.begin();
					const eastl_size_t pos = rng.RandLimit((uint32_t)s / 4);
					eastl::advance(it1, pos);
					EATEST_VERIFY(rbVectorString.validate_iterator(it1) == (isf_valid | isf_current | isf_can_dereference));

					RBVectorString::iterator it2 = it1;
					const eastl_size_t dist = rng.RandLimit((uint32_t)s / 4);
					eastl::advance(it2, dist);
					EATEST_VERIFY(rbVectorString.validate_iterator(it2) == (isf_valid | isf_current | isf_can_dereference));

					EATEST_VERIFY(s > (pos + dist));
					rbVectorString.erase(it1, it2);
					EATEST_VERIFY(rbVectorString.size() == (s - dist));
				}
			}
			else if(op == 11)
			{
				// void resize(size_type n);
				const eastl_size_t nSubOp = rng.RandLimit(100);

				if(nSubOp == 1)
				{
					rbVectorString.resize(kOriginalCapacity);
					EATEST_VERIFY(rbVectorString.size() == (RBVectorString::size_type)kOriginalCapacity);
				}
				else if(nSubOp == 2)
				{
					const eastl_size_t newSize = rng.RandLimit((uint32_t)s * 2) + 2;

					rbVectorString.resize(newSize);
					EATEST_VERIFY(rbVectorString.size() == newSize);
				}
				else if(nSubOp == 3)
				{
					rbVectorString.clear();
					EATEST_VERIFY(rbVectorString.size() == 0);
				}
			}

			EATEST_VERIFY(rbVectorString.validate());
		}

		// We make sure that after the above we still have some contents.
		if(rbVectorString.size() < 8)
			rbVectorString.resize(8);

		EATEST_VERIFY(rbVectorString.validate());

		// Test const functions
		// const_iterator begin() const;
		// const_iterator end() const;
		// const_reverse_iterator rbegin() const;
		// const_reverse_iterator rend() const;
		// const_reference front() const;
		// const_reference back() const;
		// const_reference operator[](size_type n) const;
		// const container_type& get_container() const;
		const RBVectorString& rbVSConst = rbVectorString;

		for(RBVectorString::const_iterator ic = rbVSConst.begin(); ic != rbVSConst.end(); ++ic)
		{
			EATEST_VERIFY(rbVectorString.validate_iterator(ic) == (isf_valid | isf_current | isf_can_dereference));
		}

		for(RBVectorString::const_reverse_iterator ric = rbVSConst.rbegin(); ric != rbVSConst.rend(); ++ric)
		{
			if(ric == rbVSConst.rbegin())
				EATEST_VERIFY(rbVectorString.validate_iterator(ric.base()) == (isf_valid | isf_current));
			else
				EATEST_VERIFY(rbVectorString.validate_iterator(ric.base()) == (isf_valid | isf_current | isf_can_dereference));
		}

		EATEST_VERIFY(rbVSConst.front() == rbVectorString.front());
		EATEST_VERIFY(rbVSConst.back() == rbVectorString.back());
		EATEST_VERIFY(rbVSConst[0] == rbVectorString[0]);
		EATEST_VERIFY(&rbVSConst.get_container() == &rbVectorString.get_container());


		// Test additional constructors.
		// ring_buffer(const this_type& x);
		// explicit ring_buffer(const Container& x);
		// this_type& operator=(const this_type& x);
		// void swap(this_type& x);
		RBVectorString rbVectorString2(rbVectorString);
		RBVectorString rbVectorString3(rbVectorString.get_container());
		RBVectorString rbVectorString4(rbVectorString.capacity() / 2);
		RBVectorString rbVectorString5(rbVectorString.capacity() * 2);

		EATEST_VERIFY(rbVectorString.validate());
		EATEST_VERIFY(rbVectorString2.validate());
		EATEST_VERIFY(rbVectorString3.validate());
		EATEST_VERIFY(rbVectorString4.validate());
		EATEST_VERIFY(rbVectorString5.validate());

		EATEST_VERIFY(rbVectorString == rbVectorString2);
		EATEST_VERIFY(rbVectorString3.get_container() == rbVectorString2.get_container());

		rbVectorString3 = rbVectorString4;
		EATEST_VERIFY(rbVectorString3.validate());

		eastl::swap(rbVectorString2, rbVectorString4);
		EATEST_VERIFY(rbVectorString2.validate());
		EATEST_VERIFY(rbVectorString3.validate());
		EATEST_VERIFY(rbVectorString4.validate());
		EATEST_VERIFY(rbVectorString == rbVectorString4);
		EATEST_VERIFY(rbVectorString2 == rbVectorString3);

		// void ring_buffer<T, Container>::reserve(size_type n)
		eastl_size_t cap = rbVectorString2.capacity();
		rbVectorString2.reserve(cap += 2);
		EATEST_VERIFY(rbVectorString2.validate());
		EATEST_VERIFY(rbVectorString2.capacity() == cap);
		rbVectorString2.reserve(cap -= 4); // This should act as a no-op if we are following convention.
		EATEST_VERIFY(rbVectorString2.validate());

		// void ring_buffer<T, Container>::set_capacity(size_type n)
		cap = rbVectorString2.capacity();
		rbVectorString2.resize(cap);
		EATEST_VERIFY(rbVectorString2.size() == cap);
		rbVectorString2.set_capacity(cap += 2);
		EATEST_VERIFY(rbVectorString2.validate());
		EATEST_VERIFY(rbVectorString2.capacity() == cap);
		rbVectorString2.set_capacity(cap -= 4);
		EATEST_VERIFY(rbVectorString2.capacity() == cap);
		EATEST_VERIFY(rbVectorString2.validate());

		// template <typename InputIterator>
		// void assign(InputIterator first, InputIterator last);
		string stringArray[10];
		for(int q = 0; q < 10; q++)
			stringArray[q] = (char)('0' + (char)q);

		rbVectorString5.assign(stringArray, stringArray + 10);
		EATEST_VERIFY(rbVectorString5.validate());
		EATEST_VERIFY(rbVectorString5.size() == 10);
		EATEST_VERIFY(rbVectorString5.front() == "0");
		EATEST_VERIFY(rbVectorString5.back() == "9");
	}


	{
		// Additional testing
		typedef ring_buffer< int, vector<int> > RBVectorInt;

		RBVectorInt rbVectorInt(6);

		rbVectorInt.push_back(0);
		rbVectorInt.push_back(1);
		rbVectorInt.push_back(2);
		rbVectorInt.push_back(3);
		rbVectorInt.push_back(4);
		rbVectorInt.push_back(5);
		EATEST_VERIFY(rbVectorInt[0] == 0);
		EATEST_VERIFY(rbVectorInt[5] == 5);

		// iterator insert(iterator position, const value_type& value);
		rbVectorInt.insert(rbVectorInt.begin(), 999);
		EATEST_VERIFY(rbVectorInt[0] == 999);
		EATEST_VERIFY(rbVectorInt[1] == 0);
		EATEST_VERIFY(rbVectorInt[5] == 4);

		rbVectorInt.clear();
		rbVectorInt.push_back(0);
		rbVectorInt.push_back(1);
		rbVectorInt.push_back(2);
		rbVectorInt.push_back(3);
		rbVectorInt.push_back(4);

		// iterator insert(iterator position, const value_type& value);
		rbVectorInt.insert(rbVectorInt.begin(), 999);
		EATEST_VERIFY(rbVectorInt[0] == 999);
		EATEST_VERIFY(rbVectorInt[1] == 0);
		EATEST_VERIFY(rbVectorInt[5] == 4);

		rbVectorInt.clear();
		rbVectorInt.push_back(0);
		rbVectorInt.push_back(1);
		rbVectorInt.push_back(2);
		rbVectorInt.push_back(3);
		rbVectorInt.push_back(4);
		rbVectorInt.push_back(5);
		rbVectorInt.push_back(6);
		EATEST_VERIFY(rbVectorInt[0] == 1);
		EATEST_VERIFY(rbVectorInt[5] == 6);

		// iterator insert(iterator position, const value_type& value);
		rbVectorInt.insert(rbVectorInt.begin(), 999);
		EATEST_VERIFY(rbVectorInt[0] == 999);
		EATEST_VERIFY(rbVectorInt[1] == 1);
		EATEST_VERIFY(rbVectorInt[5] == 5);

		// iterator insert(iterator position, const value_type& value);
		RBVectorInt::iterator it = rbVectorInt.begin();
		eastl::advance(it, 3);
		rbVectorInt.insert(it, 888);
		EATEST_VERIFY(rbVectorInt[0] == 999);
		EATEST_VERIFY(rbVectorInt[1] == 1);
		EATEST_VERIFY(rbVectorInt[2] == 2);
		EATEST_VERIFY(rbVectorInt[3] == 888);
		EATEST_VERIFY(rbVectorInt[4] == 3);
		EATEST_VERIFY(rbVectorInt[5] == 4);
	}

	{
		// Comparation operator ==, operator <
		// Fix bug mentioned in https://github.com/electronicarts/EASTL/issues/511
		typedef ring_buffer<int, vector<int>> RBVectorInt;

		RBVectorInt rbVectorInt(3);

		rbVectorInt.push_back(0);
		rbVectorInt.push_back(1);
		rbVectorInt.push_back(2);
		rbVectorInt.push_back(3);
		rbVectorInt.push_back(4);

		EATEST_VERIFY(rbVectorInt[0] == 2);
		EATEST_VERIFY(rbVectorInt[1] == 3);
		EATEST_VERIFY(rbVectorInt[2] == 4);

		RBVectorInt rbVectorInt2({3, 4, 2});
		RBVectorInt rbVectorInt3({2, 3, 4});
		RBVectorInt rbVectorInt4({3, 4, 5});

		EATEST_VERIFY(rbVectorInt != rbVectorInt2);
		EATEST_VERIFY(rbVectorInt == rbVectorInt3);
		EATEST_VERIFY(rbVectorInt < rbVectorInt4);

		// Different size
		RBVectorInt rbVectorInt5({1, 2});
		EATEST_VERIFY(rbVectorInt != rbVectorInt5);
		EATEST_VERIFY(rbVectorInt > rbVectorInt5);

		RBVectorInt rbVectorInt6({2, 3});
		EATEST_VERIFY(rbVectorInt != rbVectorInt6);
		EATEST_VERIFY(rbVectorInt > rbVectorInt6);

		RBVectorInt rbVectorInt7({3, 4});
		EATEST_VERIFY(rbVectorInt != rbVectorInt7);
		EATEST_VERIFY(rbVectorInt < rbVectorInt7);

		RBVectorInt rbVectorInt8({1, 2, 3, 4});
		EATEST_VERIFY(rbVectorInt != rbVectorInt8);
		EATEST_VERIFY(rbVectorInt > rbVectorInt8);

		RBVectorInt rbVectorInt9({2, 3, 4, 5});
		EATEST_VERIFY(rbVectorInt != rbVectorInt9);
		EATEST_VERIFY(rbVectorInt < rbVectorInt9);

		RBVectorInt rbVectorInt10({3, 4, 5, 6});
		EATEST_VERIFY(rbVectorInt != rbVectorInt10);
		EATEST_VERIFY(rbVectorInt < rbVectorInt10);
	}

	{
		EA::UnitTest::Rand rng(EA::UnitTest::GetRandSeed());

		typedef ring_buffer< string, list<string> > RBListString;

		int  counter = 0;
		char counterBuffer[32];

		// explicit ring_buffer(size_type size = 0);
		const int kOriginalCapacity = 50;
		RBListString rbListString(50);

		// bool empty() const;
		// size_type size() const;
		// bool validate() const;
		EATEST_VERIFY(rbListString.validate());
		EATEST_VERIFY(rbListString.empty());
		EATEST_VERIFY(rbListString.size() == 0);
		EATEST_VERIFY(rbListString.capacity() == 50);

		// void clear();
		rbListString.clear();
		EATEST_VERIFY(rbListString.validate());
		EATEST_VERIFY(rbListString.empty());
		EATEST_VERIFY(rbListString.size() == 0);
		EATEST_VERIFY(rbListString.capacity() == 50);

		// container_type& get_container();
		RBListString::container_type& c = rbListString.get_container();
		EATEST_VERIFY(c.size() == (kOriginalCapacity + 1)); // We need to add one because the ring_buffer mEnd is necessarily an unused element.

		// iterator begin();
		// iterator end();
		// int validate_iterator(const_iterator i) const;
		RBListString::iterator it = rbListString.begin();
		EATEST_VERIFY(rbListString.validate_iterator(it) == (isf_valid | isf_current));

		while(it != rbListString.end()) // This loop should do nothing.
		{
			EATEST_VERIFY(rbListString.validate_iterator(it) == (isf_valid | isf_current));
			++it;
		}

		// void push_back(const value_type& value);
		sprintf(counterBuffer, "%d", counter++);
		rbListString.push_back(string(counterBuffer));
		EATEST_VERIFY(rbListString.validate());
		EATEST_VERIFY(!rbListString.empty());
		EATEST_VERIFY(rbListString.size() == 1);
		EATEST_VERIFY(rbListString.capacity() == 50);

		it = rbListString.begin();
		EATEST_VERIFY(rbListString.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));
		EATEST_VERIFY(*it == "0");

		// reference front();
		// reference back();              
		string& sFront = rbListString.front();
		string& sBack  = rbListString.back();
		EATEST_VERIFY(&sFront == &sBack);

		// void push_back();
		string& ref = rbListString.push_back();
		EATEST_VERIFY(rbListString.validate());
		EATEST_VERIFY(rbListString.size() == 2);
		EATEST_VERIFY(rbListString.capacity() == 50);
		EATEST_VERIFY(&ref == &rbListString.back());

		it = rbListString.begin();
		++it;
		EATEST_VERIFY(rbListString.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));
		EATEST_VERIFY(it->empty());

		sprintf(counterBuffer, "%d", counter++);
		*it = counterBuffer;
		EATEST_VERIFY(*it == "1");

		++it;
		EATEST_VERIFY(it == rbListString.end());

		it = rbListString.begin();
		while(it != rbListString.end())
		{
			EATEST_VERIFY(rbListString.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));
			++it;
		}

		// reference operator[](size_type n);
		string& s0 = rbListString[0];
		EATEST_VERIFY(s0 == "0");

		string& s1 = rbListString[1];
		EATEST_VERIFY(s1 == "1");

		// Now we start hammering the ring buffer with push_back.
		for(eastl_size_t i = 0, iEnd = rbListString.capacity() * 5; i != iEnd; i++)
		{
			sprintf(counterBuffer, "%d", counter++);
			rbListString.push_back(string(counterBuffer));
			EATEST_VERIFY(rbListString.validate());
		}

		int  counterCheck = counter - 1;
		char counterCheckBuffer[32];
		sprintf(counterCheckBuffer, "%d", counterCheck);
		EATEST_VERIFY(rbListString.back() == counterCheckBuffer);

		// reverse_iterator rbegin();
		// reverse_iterator rend();
		for(RBListString::reverse_iterator ri = rbListString.rbegin(); ri != rbListString.rend(); ++ri)
		{
			sprintf(counterCheckBuffer, "%d", counterCheck--);
			EATEST_VERIFY(*ri == counterCheckBuffer);
		}

		++counterCheck;

		// iterator begin();
		// iterator end();
		for(RBListString::iterator i = rbListString.begin(); i != rbListString.end(); ++i)
		{
			EATEST_VERIFY(rbListString.validate_iterator(i) == (isf_valid | isf_current | isf_can_dereference));
			EATEST_VERIFY(*i == counterCheckBuffer);
			sprintf(counterCheckBuffer, "%d", ++counterCheck);
		}

		// void clear();
		rbListString.clear();
		EATEST_VERIFY(rbListString.validate());
		EATEST_VERIFY(rbListString.empty());
		EATEST_VERIFY(rbListString.size() == 0);
		EATEST_VERIFY(rbListString.capacity() == 50);

		// Random operations
		// Not easy to test the expected values without some tedium.
		for(int j = 0; j < 10000 + (gEASTL_TestLevel * 10000); j++)
		{
			sprintf(counterBuffer, "%d", counter++);

			const eastl_size_t op = rng.RandLimit(12);
			const eastl_size_t s  = rbListString.size();

			if(op == 0)
			{
				// void push_back(const value_type& value);

				rbListString.push_back(string(counterBuffer));
				EATEST_VERIFY(rbListString.size() == eastl::min(s + 1, rbListString.capacity()));
			}
			else if(op == 1)
			{
				// void push_back();

				string& ref2 = rbListString.push_back();
				rbListString.back() = string(counterBuffer);
				EATEST_VERIFY(rbListString.size() == eastl::min(s + 1, rbListString.capacity()));
				EATEST_VERIFY(&ref2 == &rbListString.back());
			}
			else if(op == 2)
			{
				// void pop_back();

				if(!rbListString.empty())
				{
					rbListString.pop_back();
					EATEST_VERIFY(rbListString.size() == (s - 1));
				}
			}
			else if(op == 3)
			{
				// void push_front(const value_type& value);

				rbListString.push_front(string(counterBuffer));
				EATEST_VERIFY(rbListString.size() == eastl::min(s + 1, rbListString.capacity()));
			}
			else if(op == 4)
			{
				// void push_front();

				string& ref2 = rbListString.push_front();
				rbListString.front() = string(counterBuffer);
				EATEST_VERIFY(rbListString.size() == eastl::min(s + 1, rbListString.capacity()));
				EATEST_VERIFY(&ref2 == &rbListString.front());
			}
			else if(op == 5)
			{
				// void pop_front();

				if(!rbListString.empty())
				{
					rbListString.pop_front();
					EATEST_VERIFY(rbListString.size() == (s - 1));
				}
			}
			else if(op == 6)
			{
				// iterator insert(iterator position, const value_type& value);

				it = rbListString.begin();
				const eastl_size_t dist = rng.RandLimit((uint32_t)s + 1);
				eastl::advance(it, dist);

				if(it == rbListString.end())
					EATEST_VERIFY(rbListString.validate_iterator(it) == (isf_valid | isf_current));
				else
					EATEST_VERIFY(rbListString.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));

				rbListString.insert(it, string(counterBuffer));
				EATEST_VERIFY(rbListString.size() == eastl::min(s + 1, rbListString.capacity()));
			}
			else if(op == 7)
			{
				// void insert(iterator position, size_type n, const value_type& value);

				it = rbListString.begin();
				const eastl_size_t dist = rng.RandLimit((uint32_t)s + 1);
				eastl::advance(it, dist);

				if(it == rbListString.end())
					EATEST_VERIFY(rbListString.validate_iterator(it) == (isf_valid | isf_current));
				else
					EATEST_VERIFY(rbListString.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));

				const eastl_size_t count = (eastl_size_t)rng.RandLimit(10);

				rbListString.insert(it, count, string(counterBuffer));
				EATEST_VERIFY(rbListString.size() == eastl::min(s + count, rbListString.capacity()));
			}
			else if(op == 8)
			{
				// template <typename InputIterator>
				// void insert(iterator position, InputIterator first, InputIterator last);

				string stringArray[10];

				it = rbListString.begin();
				const eastl_size_t dist = rng.RandLimit((uint32_t)s + 1);
				eastl::advance(it, dist);

				if(it == rbListString.end())
					EATEST_VERIFY(rbListString.validate_iterator(it) == (isf_valid | isf_current));
				else
					EATEST_VERIFY(rbListString.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));

				const eastl_size_t count = (eastl_size_t)rng.RandLimit(10);

				rbListString.insert(it, stringArray, stringArray + count);
				EATEST_VERIFY(rbListString.size() == eastl::min(s + count, rbListString.capacity()));
			}
			else if(op == 9)
			{
				// iterator erase(iterator position);

				if(!rbListString.empty())
				{
					it = rbListString.begin();
					const eastl_size_t dist = rng.RandLimit((uint32_t)s);
					eastl::advance(it, dist);
					EATEST_VERIFY(rbListString.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));
					rbListString.erase(it);

					EATEST_VERIFY(rbListString.size() == (s  - 1));
				}
			}
			else if(op == 10)
			{
				// iterator erase(iterator first, iterator last);

				if(!rbListString.empty())
				{
					RBListString::iterator it1 = rbListString.begin();
					const eastl_size_t pos = rng.RandLimit((uint32_t)s / 4);
					eastl::advance(it1, pos);
					EATEST_VERIFY(rbListString.validate_iterator(it1) == (isf_valid | isf_current | isf_can_dereference));

					RBListString::iterator it2 = it1;
					const eastl_size_t dist = rng.RandLimit((uint32_t)s / 4);
					eastl::advance(it2, dist);
					EATEST_VERIFY(rbListString.validate_iterator(it2) == (isf_valid | isf_current | isf_can_dereference));

					EATEST_VERIFY(s > (pos + dist));
					rbListString.erase(it1, it2);
					EATEST_VERIFY(rbListString.size() == (s - dist));
				}
			}
			else if(op == 11)
			{
				// void resize(size_type n);
				const eastl_size_t nSubOp = rng.RandLimit(100);

				if(nSubOp == 1)
				{
					rbListString.resize(kOriginalCapacity);
					EATEST_VERIFY(rbListString.size() == (RBListString::size_type)kOriginalCapacity);
				}
				else if(nSubOp == 2)
				{
					const eastl_size_t newSize = rng.RandLimit((uint32_t)s * 2) + 2;

					rbListString.resize(newSize);
					EATEST_VERIFY(rbListString.size() == newSize);
				}
				else if(nSubOp == 3)
				{
					rbListString.clear();
					EATEST_VERIFY(rbListString.size() == 0);
				}
			}

			EATEST_VERIFY(rbListString.validate());
		}

		// We make sure that after the above we still have some contents.
		if(rbListString.size() < 8)
			rbListString.resize(8);

		EATEST_VERIFY(rbListString.validate());

		// Test const functions
		// const_iterator begin() const;
		// const_iterator end() const;
		// const_reverse_iterator rbegin() const;
		// const_reverse_iterator rend() const;
		// const_reference front() const;
		// const_reference back() const;
		// const_reference operator[](size_type n) const;
		// const container_type& get_container() const;
		const RBListString& rbVSConst = rbListString;

		for(RBListString::const_iterator ic = rbVSConst.begin(); ic != rbVSConst.end(); ++ic)
		{
			EATEST_VERIFY(rbListString.validate_iterator(ic) == (isf_valid | isf_current | isf_can_dereference));
		}

		for(RBListString::const_reverse_iterator ric = rbVSConst.rbegin(); ric != rbVSConst.rend(); ++ric)
		{
			if(ric == rbVSConst.rbegin())
				EATEST_VERIFY(rbListString.validate_iterator(ric.base()) == (isf_valid | isf_current));
			else
				EATEST_VERIFY(rbListString.validate_iterator(ric.base()) == (isf_valid | isf_current | isf_can_dereference));
		}

		EATEST_VERIFY(rbVSConst.front() == rbListString.front());
		EATEST_VERIFY(rbVSConst.back() == rbListString.back());
		EATEST_VERIFY(rbVSConst[0] == rbListString[0]);
		EATEST_VERIFY(&rbVSConst.get_container() == &rbListString.get_container());


		// Test additional constructors.
		// ring_buffer(const this_type& x);
		// explicit ring_buffer(const Container& x);
		// this_type& operator=(const this_type& x);
		// void swap(this_type& x);
		RBListString rbListString2(rbListString);
		RBListString rbListString3(rbListString.get_container());
		RBListString rbListString4(rbListString.capacity() / 2);
		RBListString rbListString5(rbListString.capacity() * 2);

		EATEST_VERIFY(rbListString.validate());
		EATEST_VERIFY(rbListString2.validate());
		EATEST_VERIFY(rbListString3.validate());
		EATEST_VERIFY(rbListString4.validate());
		EATEST_VERIFY(rbListString5.validate());

		EATEST_VERIFY(rbListString == rbListString2);
		EATEST_VERIFY(rbListString3.get_container() == rbListString2.get_container());

		rbListString3 = rbListString4;
		EATEST_VERIFY(rbListString3.validate());

		eastl::swap(rbListString2, rbListString4);
		EATEST_VERIFY(rbListString2.validate());
		EATEST_VERIFY(rbListString3.validate());
		EATEST_VERIFY(rbListString4.validate());
		EATEST_VERIFY(rbListString == rbListString4);
		EATEST_VERIFY(rbListString2 == rbListString3);

		// void ring_buffer<T, Container>::reserve(size_type n)
		eastl_size_t cap = rbListString2.capacity();
		rbListString2.reserve(cap += 2);
		EATEST_VERIFY(rbListString2.validate());
		EATEST_VERIFY(rbListString2.capacity() == cap);
		rbListString2.reserve(cap -= 4); // This should act as a no-op if we are following convention.
		EATEST_VERIFY(rbListString2.validate());


		// template <typename InputIterator>
		// void assign(InputIterator first, InputIterator last);
		string stringArray[10];
		for(int q = 0; q < 10; q++)
			stringArray[q] = '0' + (char)q;

		rbListString5.assign(stringArray, stringArray + 10);
		EATEST_VERIFY(rbListString5.validate());
		EATEST_VERIFY(rbListString5.size() == 10);
		EATEST_VERIFY(rbListString5.front() == "0");
		EATEST_VERIFY(rbListString5.back() == "9");


		// ring_buffer(this_type&& x);
		// ring_buffer(this_type&& x, const allocator_type& allocator);
		// this_type& operator=(this_type&& x);

		RBListString rbListStringM1(eastl::move(rbListString5));
		EATEST_VERIFY(rbListStringM1.validate() && rbListString5.validate());
		EATEST_VERIFY((rbListStringM1.size() == 10) && (rbListString5.size() == 0));

		RBListString rbListStringM2(eastl::move(rbListStringM1), RBListString::allocator_type());
		EATEST_VERIFY(rbListStringM2.validate() && rbListStringM1.validate());
		EATEST_VERIFY((rbListStringM2.size() == 10) && (rbListStringM1.size() == 0));

		rbListStringM1 = eastl::move(rbListStringM2);
		EATEST_VERIFY(rbListStringM1.validate() && rbListStringM2.validate());
		EATEST_VERIFY((rbListStringM1.size() == 10) && (rbListStringM2.size() == 0));
	}



	{
		// ring_buffer(std::initializer_list<value_type> ilist, const allocator_type& allocator = allocator_type());
		// this_type& operator=(std::initializer_list<value_type> ilist);
		// void     insert(iterator position, std::initializer_list<value_type> ilist);
		#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
			ring_buffer<int> intBuffer = { 0, 1, 2 };
			EATEST_VERIFY(VerifySequence(intBuffer.begin(), intBuffer.end(), int(), "ring_buffer std::initializer_list", 0, 1, 2, -1));

			intBuffer = { 16, 17, 18 };
			EATEST_VERIFY(VerifySequence(intBuffer.begin(), intBuffer.end(), int(), "ring_buffer std::initializer_list", 16, 17, 18, -1));

			// We need to increase the capacity in order to insert new items because the ctor above set the capacity to be only enough to hold the initial list.
			intBuffer.reserve(intBuffer.capacity() + 2);
			intBuffer.insert(intBuffer.begin(), { 14, 15 });
			EATEST_VERIFY(VerifySequence(intBuffer.begin(), intBuffer.end(), int(), "ring_buffer std::initializer_list", 14, 15, 16, 17, 18, -1));
		#endif
	}

	{
		// Regression for user-reported problem.
		typedef eastl::fixed_vector<float, 8>                     GamepadData_t;
		typedef eastl::ring_buffer<GamepadData_t>                 GamepadDataDelayBuffer_t;
		typedef eastl::fixed_vector<GamepadDataDelayBuffer_t, 32> GamepadDataDelayBufferTable_t;

		GamepadDataDelayBufferTable_t mDelayTable;
		mDelayTable.resize(32);
		for(eastl_size_t i = 0; i < mDelayTable.size(); i++)
			mDelayTable[i].reserve(16);
		
		GamepadData_t data(8, 1.f);
		mDelayTable[0].push_back(data);
		mDelayTable[0].push_back(data);
		mDelayTable[0].push_back(data);
		mDelayTable[0].push_back(data);

		EATEST_VERIFY(mDelayTable[0].size() == 4);
		GamepadData_t dataFront = mDelayTable[0].front();
		EATEST_VERIFY((dataFront.size() == 8) && (dataFront[0] == 1.f));
		mDelayTable[0].pop_front();
	}

	{
		// Regression for bug with iterator subtraction
		typedef eastl::ring_buffer<int>		IntBuffer_t;
		IntBuffer_t intBuffer = { 0, 1, 2, 3, 4, 5, 6, 7 };
		IntBuffer_t::iterator it = intBuffer.begin();

		EATEST_VERIFY(*it == 0);
		it += 4;
		EATEST_VERIFY(*it == 4);
		it--;
		EATEST_VERIFY(*it == 3);
		it -= 2;
		EATEST_VERIFY(*it == 1);

		intBuffer.push_back(8);
		intBuffer.push_back(9);
		intBuffer.push_back(10);
		intBuffer.push_back(11);

		EATEST_VERIFY(*it == 10);
		it -= 3;
		EATEST_VERIFY(*it == 7); // Test looping around the end of the underlying container
		it -= 5;
		EATEST_VERIFY(*it == 11); // Test wrapping around begin to end of the ring_buffer
		it -= 2;
		EATEST_VERIFY(*it == 9); // It is important to test going back to the beginning of the underlying container.

	}

	// fixed_ring_buffer<T,N> tests
	// ring_buffer<T, fixed_vector<T,N>> tests
	{
		{
			// (MAX_ELEMENTS - 1) accommodates the ring_buffer sentinel
			const int MAX_ELEMENTS = 8;
			eastl::ring_buffer<int, eastl::fixed_vector<int, MAX_ELEMENTS, false>> rb(MAX_ELEMENTS - 1);  

			for (int i = 0; i < MAX_ELEMENTS - 1; i++)
				rb.push_back(i);

			auto it = rb.begin();
			for (int i = 0; i < MAX_ELEMENTS - 1; i++)
				{ EATEST_VERIFY(*it++ == i); }
		}

		#if !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		{
			const int MAX_ELEMENTS = 25;
			eastl::fixed_ring_buffer<int, MAX_ELEMENTS> rb(MAX_ELEMENTS);

			for(int i = 0; i < MAX_ELEMENTS; i++)
				rb.push_back(i);

			auto it = rb.begin();
			for(int i = 0; i < MAX_ELEMENTS; i++)
				{ EATEST_VERIFY(*it++ == i); }
		}
		#endif

		#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS) && !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		{
			const int MAX_ELEMENTS = 8;
			eastl::fixed_ring_buffer<int, MAX_ELEMENTS> rb = {0, 1, 2, 3, 4, 5, 6, 7};

			auto it = rb.begin();
			for(int i = 0; i < MAX_ELEMENTS; i++)
				{ EATEST_VERIFY(*it++ == i); }
		}

		{
			struct LocalStruct {};
			fixed_ring_buffer<LocalStruct, 8> rb = {{{}, {}, {}}};
			EATEST_VERIFY(rb.size() == 3);
		}
		#endif
	}

	{
		const auto MAX_ELEMENTS = EASTL_MAX_STACK_USAGE;

		// create a container simulating LARGE state that exceeds
		// our maximum stack size macro. This forces our ring_buffer implementation
		// to allocate the container in the heap instead of holding it on the stack.
		// This test ensures that allocation is NOT serviced by the default global heap.  
		// Instead it is serviced by the allocator of the ring_buffers underlying container.
		struct PaddedVector : public eastl::vector<int, MallocAllocator>
		{
			char mPadding[EASTL_MAX_STACK_USAGE];
		};

		MallocAllocator::reset_all();
		CountingAllocator::resetCount();

		{
			CountingAllocator countingAlloc;
			AutoDefaultAllocator _(&countingAlloc);

			eastl::ring_buffer<int, PaddedVector> intBuffer(1);  
			for (int i = 0; i < MAX_ELEMENTS; i++)
				intBuffer.push_back(i);

		#if !EASTL_OPENSOURCE
			const auto cacheAllocationCount = gEASTLTest_TotalAllocationCount;
		#endif
			const auto cacheMallocatorCount = MallocAllocator::mAllocCountAll;
			const auto forceReAllocSize = intBuffer.size() * 2;

			intBuffer.resize(forceReAllocSize);

		#if !EASTL_OPENSOURCE
			VERIFY(cacheAllocationCount == gEASTLTest_TotalAllocationCount);
		#endif
			VERIFY(cacheMallocatorCount <  MallocAllocator::mAllocCountAll);
			VERIFY(CountingAllocator::neverUsed());
		}
	}

	return nErrorCount;
}









