/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include "EASTLTestIterators.h"
#include <EABase/eabase.h>
#include <EASTL/deque.h>
#include <EASTL/list.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/algorithm.h>
#include <EASTL/unique_ptr.h>
#include "ConceptImpls.h"


using namespace eastl;



// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::deque<int>;
template class eastl::deque<TestObject>;


// Test compiler issue that appeared in VS2012 relating to deque::kAlignment.
struct StructWithContainerOfStructs
{
	eastl::deque<StructWithContainerOfStructs, EASTLAllocatorType, 16> children;
};

// The following will not compile because the default value of kDequeSubarraySize
// relies on sizeof(T). Thus, a non-default value must be provided, or the full type
// will be required at the time of instantiation, but it is not available.
// struct StructWithContainerOfStructsDefault
// {
//     eastl::deque<StructWithContainerOfStructsDefault> children;
// };


///////////////////////////////////////////////////////////////////////////////
typedef eastl::deque<int>                            IntDeque;
typedef eastl::deque<int, EASTLAllocatorType, 1>     IntDeque1;
typedef eastl::deque<int, EASTLAllocatorType, 32768> IntDeque32768;


typedef eastl::deque<TestObject>                            TestObjectDeque;
typedef eastl::deque<TestObject, EASTLAllocatorType, 1>     TestObjectDeque1;
typedef eastl::deque<TestObject, EASTLAllocatorType, 32768> TestObjectDeque32768;

///////////////////////////////////////////////////////////////////////////////
// TestDequeConstruction
//
template <typename Deque>
int TestDequeConstruction()
{
	typedef typename Deque::value_type value_type;
	typedef typename Deque::size_type size_type;

	int nErrorCount = 0;

	{
		// constructors
		Deque dA;
		EATEST_VERIFY(dA.size() == 0);

		Deque dB((size_type)0);
		EATEST_VERIFY(dB.size() == 0);

		Deque dC(1000);
		EATEST_VERIFY(dC.size() == 1000);
		for (const auto& elem : dC)
			EATEST_VERIFY(elem == value_type());

		Deque dD(2000, value_type(1));
		EATEST_VERIFY(dD.size() == 2000);
		for (const auto& elem : dD)
			EATEST_VERIFY(elem == value_type(1));

		Deque dE(dC);
		EATEST_VERIFY(dE.size() == 1000);
		for (const auto& elem : dE)
			EATEST_VERIFY(elem == value_type());

		Deque dF(dC.begin(), dC.end());
		EATEST_VERIFY(dF.size() == 1000);
		for (const auto& elem : dF)
			EATEST_VERIFY(elem == value_type());

		// operator=
		dE = dD;
		EATEST_VERIFY(dE.size() == 2000);
		for (const auto& elem : dE)
			EATEST_VERIFY(elem == value_type(1));

		// swap
		dE.swap(dC);
		EATEST_VERIFY(dE.size() == 1000);
		for (const auto& elem : dE)
			EATEST_VERIFY(elem == value_type());
		EATEST_VERIFY(dC.size() == 2000);
		for (const auto& elem : dC)
			EATEST_VERIFY(elem == value_type(1));

		// clear
		dA.clear();
		EATEST_VERIFY(dA.size() == 0);
		EATEST_VERIFY(dA.empty());

		dB.clear();
		EATEST_VERIFY(dB.size() == 0);
		EATEST_VERIFY(dB.empty());
	}

	EATEST_VERIFY(TestObject::sTOCount == 0);
	EATEST_VERIFY(TestObject::sMagicErrorCount == 0);

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestDequeSimpleMutation
//
template <typename Deque>
int TestDequeSimpleMutation()
{
	typedef typename Deque::value_type value_type;
	typedef typename Deque::size_type size_type;

	int nErrorCount = 0;

	{
		Deque d;

		// push_back(value_type&)
		// front
		// back
		for (int i = 0; i < 1000; i++)
		{
			d.push_back(value_type(i));
			EATEST_VERIFY(d.back() == value_type(i));
		}
		EATEST_VERIFY(d.front() == value_type(0));

		// operator[]
		// at()
		for (unsigned int i = 0; i < d.size(); ++i)
		{
			EATEST_VERIFY(d[i] == value_type(i));
			EATEST_VERIFY(d.at(i) == value_type(i));
		}
	}

	{
		Deque d;

		// push_back()
		// this overloads is an EASTL extension.
		for (int i = 0; i < 1000; i++)
		{
			value_type& ref = d.push_back();
			EATEST_VERIFY(&ref == &d.back());
			EATEST_VERIFY(d.back() == value_type());
		}
		EATEST_VERIFY(d.front() == value_type());

		// operator[]
		// at()
		for (size_type i = 0, iEnd = d.size(); i < iEnd; i++)
		{
			EATEST_VERIFY(d[i] == value_type());
			EATEST_VERIFY(d.at(i) == value_type());
		}
	}

	{
		Deque d;

		// push_front(value_type&)
		for (int i = 0; i < 1000; i++)
		{
			d.push_front(value_type(i));
			EATEST_VERIFY(d.front() == value_type(i));
		}

		// operator[]
		// at()
		EATEST_VERIFY(d.size() == 1000);
		for (int i = 0; i < 1000; i++)
		{
			EATEST_VERIFY(d[1000 - 1 - i] == value_type(i));
			EATEST_VERIFY(d.at(1000 - 1 - i) == value_type(i));
		}
	}

	{
		Deque d;

		// push_front()
		// this overloads is an EASTL extension.
		for (int i = 0; i < 1000; i++)
		{
			value_type& ref = d.push_front();
			EATEST_VERIFY(&ref == &d.front());
			EATEST_VERIFY(d.front() == value_type());
		}
		EATEST_VERIFY(d.back() == value_type());

		// operator[]
		// at()
		for (size_type i = 0, iEnd = d.size(); i < iEnd; i++)
		{
			EATEST_VERIFY(d[i] == value_type());
			EATEST_VERIFY(d.at(i) == value_type());
		}

		// pop_back()
		for (int i = 0; i < 500; i++)
		{
			d.pop_back();
		}
		EATEST_VERIFY(d.size() == 500);
		for (const auto& elem : d)
			EATEST_VERIFY(elem == value_type());

		// pop_front()
		for (int i = 0; i < 500; i++)
		{
			d.pop_front();
		}
		EATEST_VERIFY(d.size() == 0);
	}

	{
		Deque d;

		// resize(value_type&)
		for(int i = 0; i < 500; i++)
		{
			d.resize(d.size() + 3, value_type(i));
			EATEST_VERIFY(d.size() == size_type((i + 1) * 3));
		}
		
		EATEST_VERIFY(d.size() == 1500);

		for (int i = 0; i < 500; ++i)
		{
			EATEST_VERIFY(d[i * 3 + 0] == value_type(i));
			EATEST_VERIFY(d[i * 3 + 1] == value_type(i));
			EATEST_VERIFY(d[i * 3 + 2] == value_type(i));
		}

		// resize()
		for(int i = 0; i < 500; i++)
		{
			d.resize(d.size() - 2);
			EATEST_VERIFY(d.size() == size_type(1500 - ((i + 1) * 2)));
		}
		EATEST_VERIFY(d.size() == 500);
	}

	EATEST_VERIFY(TestObject::sTOCount == 0);
	EATEST_VERIFY(TestObject::sMagicErrorCount == 0);

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestDequeComplexMutation
//
template <typename Deque>
int TestDequeComplexMutation()
{
	typedef typename Deque::value_type value_type;
	typedef typename Deque::size_type size_type;
	typedef typename Deque::iterator iterator;
	typedef typename Deque::reverse_iterator reverse_iterator;

	int nErrorCount = 0;

	{
		Deque d;


		//////////////////////////////////////////////////////////////////
		// void assign(size_type n, const value_type& value);
		//////////////////////////////////////////////////////////////////

		d.assign(100, 1);
		EATEST_VERIFY(d.size() == 100);
		for (const auto& elem : d)
			EATEST_VERIFY(elem == value_type(1));

		d.assign(50, 2);
		EATEST_VERIFY(d.size() == 50);
		for (const auto& elem : d)
			EATEST_VERIFY(elem == value_type(2));

		d.assign(150, 3);
		EATEST_VERIFY(d.size() == 150);
		for (const auto& elem : d)
			EATEST_VERIFY(elem == value_type(3));



		//////////////////////////////////////////////////////////////////
		// template <typename InputIterator>
		// void assign(InputIterator first, InputIterator last);
		//////////////////////////////////////////////////////////////////

		list<value_type> valueList;
		for(int i = 0; i < 100; i++)
			valueList.push_back(value_type(i));

		d.assign(valueList.begin(), valueList.end());
		EATEST_VERIFY(d.size() == 100);
		for (int i = 0; i < 100; i++)
			EATEST_VERIFY(d[i] == value_type(i));


		//////////////////////////////////////////////////////////////////
		// iterator insert(iterator position, const value_type& value);
		//////////////////////////////////////////////////////////////////

		iterator itFirstInserted = d.insert(d.begin(), d[1]);
		EATEST_VERIFY(itFirstInserted == d.begin());
		EATEST_VERIFY(d[0] == value_type(1));

		value_type value = d[d.size() - 2];
		itFirstInserted = d.insert(d.end(), value);
		EATEST_VERIFY(itFirstInserted == d.end() - 1);
		EATEST_VERIFY(*(d.end() - 1) == value);

		iterator itNearBegin = d.begin();
		advance(itNearBegin, 1);

		value = d[3];
		itFirstInserted = d.insert(itNearBegin, value);
		EATEST_VERIFY(itFirstInserted == d.begin() + 1);
		EATEST_VERIFY(d[1] == value);

		iterator itNearEnd = d.begin();
		advance(itNearEnd, d.size() - 1);

		value = d[d.size() - 2];
		itFirstInserted = d.insert(itNearEnd, value);
		EATEST_VERIFY(itFirstInserted == d.end() - 2);
		EATEST_VERIFY(d[d.size() - 2] == value);


		//////////////////////////////////////////////////////////////////
		// iterator insert(iterator position, size_type n, const value_type& value);
		//////////////////////////////////////////////////////////////////

		itFirstInserted = d.insert(d.begin(), d.size() * 2, value_type(3)); // Insert a large number of items at the front.
		EATEST_VERIFY(itFirstInserted == d.begin());
		for (size_type i = 0; i < d.size() / 2; i++)
			EATEST_VERIFY(d[i] == value_type(3));

		itFirstInserted = d.insert(d.end(), d.size() * 2, value_type(3)); // Insert a large number of items at the end.
		EATEST_VERIFY(itFirstInserted == d.begin() + (d.size() / 3));
		for (size_type i = 0; i < d.size() / 2; i++)
			EATEST_VERIFY(d[d.size() - 1 - i] == value_type(3));

		itNearBegin = d.begin();
		advance(itNearBegin, 3);

		itFirstInserted = d.insert(itNearBegin, 3, value_type(4));
		EATEST_VERIFY(itFirstInserted == d.begin() + 3);
		EATEST_VERIFY(VerifySequence(d.begin() + 3, d.begin() + 6, { value_type(4), value_type(4), value_type(4) }, "insert()"));

		itNearEnd = d.begin();
		advance(itNearEnd, d.size() - 1);

		itFirstInserted = d.insert(d.end(), 5, value_type(6));
		EATEST_VERIFY(itFirstInserted == d.end() - 5);
		EATEST_VERIFY(VerifySequence(d.end() - 5, d.end(), { value_type(6), value_type(6), value_type(6), value_type(6), value_type(6) }, "insert()"));


		EATEST_VERIFY(d.begin() == d.insert(d.begin(), 0, value_type(9)));


		//////////////////////////////////////////////////////////////////
		// template <typename InputIterator>
		// iterator insert(iterator position, InputIterator first, InputIterator last);
		//////////////////////////////////////////////////////////////////

		itNearBegin = d.begin();
		advance(itNearBegin, 3);

		itFirstInserted = d.insert(itNearBegin, valueList.begin(), valueList.end());
		for (int i = 0; i < 100; ++i, ++itFirstInserted)
			EATEST_VERIFY(*itFirstInserted == value_type(i));

		value_type x(0);
		itFirstInserted = d.insert(d.begin(), InputIterator<value_type>(&x), InputIterator<value_type>(EndSentinel(), value_type(5)));
		EATEST_VERIFY(itFirstInserted == d.begin());
		EATEST_VERIFY(VerifySequence(d.begin(), d.begin() + 5, { value_type(0), value_type(1), value_type(2), value_type(3), value_type(4) }, "deque::insert() with input iterators"));

		auto inputEnd = InputIterator<value_type>(EndSentinel(), value_type(5));
		EATEST_VERIFY(d.begin() == d.insert(d.begin(), inputEnd, inputEnd));

		value_type* itContiguous = nullptr;
		EATEST_VERIFY(d.begin() == d.insert(d.begin(), itContiguous, itContiguous));

		//////////////////////////////////////////////////////////////////
		// iterator erase(iterator position);
		//////////////////////////////////////////////////////////////////

		itNearBegin = d.begin();

		size_type sizeBeforeErase = d.size();
		while(itNearBegin != d.end()) // Run a loop whereby we erase every fourth element.
		{
			for(int i = 0; (i < 3) && (itNearBegin != d.end()); ++i)
			{
				++itNearBegin;
			}

			if(itNearBegin != d.end())
			{
				itNearBegin = d.erase(itNearBegin);
			}
		}
		EATEST_VERIFY(sizeBeforeErase - (sizeBeforeErase / 4) == d.size());


		//////////////////////////////////////////////////////////////////
		// iterator erase(iterator first, iterator last);
		//////////////////////////////////////////////////////////////////

		itNearBegin = d.begin();

		while(itNearBegin != d.end()) // Run a loop whereby we erase spans of elements.
		{
			iterator itSaved = itNearBegin;

			size_type numElementsToErase = 0;
			for(; (numElementsToErase < 22) && (itNearBegin != d.end()); ++numElementsToErase)
			{
				++itNearBegin;
			}

			if(itNearBegin != d.end())
			{
				size_type numElementsPrior = d.size();
				itNearBegin = d.erase(itSaved, itNearBegin);
				EATEST_VERIFY(d.size() == numElementsPrior - numElementsToErase);
			}

			for(int i = 0; (i < 17) && (itNearBegin != d.end()); ++i)
			{
				++itNearBegin;
			}
		}
	}


	{
		//////////////////////////////////////////////////////////////////
		// reverse_iterator erase(reverse_iterator position);
		// reverse_iterator erase(reverse_iterator first, reverse_iterator last);
		//////////////////////////////////////////////////////////////////

		Deque dErase;

		for(int i = 0; i < 20; i++)
		{
			value_type val(i);
			dErase.push_back(val);
		}
		EATEST_VERIFY((dErase.size() == 20) && (dErase[0] == value_type(0)) && (dErase[19] == value_type(19)));


		reverse_iterator rA = dErase.rbegin();
		reverse_iterator rB = rA + 3;
		dErase.erase(rA, rB);
		EATEST_VERIFY((dErase.size() == 17));
		EATEST_VERIFY((dErase[0] == value_type(0)));
		EATEST_VERIFY((dErase[16] == value_type(16)));


		rB = dErase.rend();
		rA = rB - 3;
		dErase.erase(rA, rB);
		EATEST_VERIFY((dErase.size() == 14));
		EATEST_VERIFY((dErase[0] == value_type(3)));
		EATEST_VERIFY((dErase[13] == value_type(16)));


		rB = dErase.rend() - 1;
		dErase.erase(rB);
		EATEST_VERIFY((dErase.size() == 13));
		EATEST_VERIFY((dErase[0] == value_type(4)));
		EATEST_VERIFY((dErase[12] == value_type(16)));


		rB = dErase.rbegin();
		dErase.erase(rB);
		EATEST_VERIFY((dErase.size() == 12));
		EATEST_VERIFY((dErase[0] == value_type(4)));
		EATEST_VERIFY((dErase[11] == value_type(15)));


		rA = dErase.rbegin();
		rB = dErase.rend();
		dErase.erase(rA, rB);
		EATEST_VERIFY(dErase.size() == 0);
	}


	EATEST_VERIFY(TestObject::sTOCount == 0);
	EATEST_VERIFY(TestObject::sMagicErrorCount == 0);

	return nErrorCount;
}


int TestDeque()
{
	int nErrorCount = 0;

	{   // Test construction
		nErrorCount += TestDequeConstruction<IntDeque>();
		nErrorCount += TestDequeConstruction<IntDeque1>();
		nErrorCount += TestDequeConstruction<IntDeque32768>();

		nErrorCount += TestDequeConstruction<TestObjectDeque>();
		nErrorCount += TestDequeConstruction<TestObjectDeque1>();
		nErrorCount += TestDequeConstruction<TestObjectDeque32768>();
	}

	{   // Test simple mutating functionality.
		nErrorCount += TestDequeSimpleMutation<IntDeque>();
		nErrorCount += TestDequeSimpleMutation<IntDeque1>();
		nErrorCount += TestDequeSimpleMutation<IntDeque32768>();

		nErrorCount += TestDequeSimpleMutation<TestObjectDeque>();
		nErrorCount += TestDequeSimpleMutation<TestObjectDeque1>();
		nErrorCount += TestDequeSimpleMutation<TestObjectDeque32768>();
	}

	{   // Test complex mutating functionality.
		nErrorCount += TestDequeComplexMutation<IntDeque>();
		nErrorCount += TestDequeComplexMutation<IntDeque1>();
		nErrorCount += TestDequeComplexMutation<IntDeque32768>();

		nErrorCount += TestDequeComplexMutation<TestObjectDeque>();
		nErrorCount += TestDequeComplexMutation<TestObjectDeque1>();
		nErrorCount += TestDequeComplexMutation<TestObjectDeque32768>();
	}

	// test deque support of move-only types
	{
		{
			eastl::deque<MoveAssignable> d;
			d.emplace_back(MoveAssignable::Create());
			d.emplace_front(MoveAssignable::Create());

			auto cd = eastl::move(d);
			EATEST_VERIFY( d.size() == 0);
			EATEST_VERIFY(cd.size() == 2);
		}

		{
			// User regression but passing end() to deque::erase is not valid.  
			// Iterator passed to deque::erase but must valid and dereferencable.
			//
			// eastl::deque<MoveAssignable> d;  // empty deque
			// d.erase(d.begin());
			// EATEST_VERIFY(d.size() == 0);
		}

		// simply test the basic api of deque with a move-only type
		{
			eastl::deque<MoveAssignable> d;

			// emplace_back
			d.emplace_back(MoveAssignable::Create());
			d.emplace_back(MoveAssignable::Create());
			d.emplace_back(MoveAssignable::Create());

			// erase
			d.erase(d.begin());
			EATEST_VERIFY(d.size() == 2);

			// at / front / back / operator[]
			EATEST_VERIFY(d[0].value == 42);
			EATEST_VERIFY(d.at(0).value == 42);
			EATEST_VERIFY(d.front().value == 42);
			EATEST_VERIFY(d.back().value == 42);

			// clear
			d.clear();
			EATEST_VERIFY(d.size() == 0);

			// emplace
			d.emplace(d.begin(), MoveAssignable::Create());
			d.emplace(d.begin(), MoveAssignable::Create());
			EATEST_VERIFY(d.size() == 2);

			// pop_back
			d.pop_back();
			EATEST_VERIFY(d.size() == 1);

			// push_back / push_front / resize requires T be 'CopyConstructible' 

			{
				eastl::deque<MoveAssignable> swapped_d;

				// emplace_front
				swapped_d.emplace_front(MoveAssignable::Create());
				swapped_d.emplace_front(MoveAssignable::Create());
				swapped_d.emplace_front(MoveAssignable::Create());

				// swap
				swapped_d.swap(d);
				EATEST_VERIFY(swapped_d.size() == 1);
				EATEST_VERIFY(d.size() == 3);
			}

			// pop_front
			d.pop_front();
			EATEST_VERIFY(d.size() == 2);

			// insert
			d.insert(d.end(), MoveAssignable::Create());
			EATEST_VERIFY(d.size() == 3);
		}
	}

	{
		// deque(std::initializer_list<value_type> ilist, const allocator_type& allocator = EASTL_DEQUE_DEFAULT_ALLOCATOR);
		// this_type& operator=(std::initializer_list<value_type> ilist);
		// void assign(std::initializer_list<value_type> ilist);
		// iterator insert(iterator position, std::initializer_list<value_type> ilist);
		#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
			eastl::deque<int> intDeque = { 0, 1, 2 };
			EATEST_VERIFY(VerifySequence(intDeque, { 0, 1, 2 }, "deque std::initializer_list"));

			intDeque = { 13, 14, 15 };
			EATEST_VERIFY(VerifySequence(intDeque, { 13, 14, 15 }, "deque std::initializer_list"));

			intDeque.assign({ 16, 17, 18 });
			EATEST_VERIFY(VerifySequence(intDeque, { 16, 17, 18 }, "deque std::initializer_list"));

			eastl::deque<int>::iterator it = intDeque.insert(intDeque.begin(), { 14, 15 });
			EATEST_VERIFY(VerifySequence(intDeque, { 14, 15, 16, 17, 18 }, "deque std::initializer_list"));
			EATEST_VERIFY(*it == 14);
		#endif
	}


	{   // C++11 functionality
		// deque(this_type&& x);
		// deque(this_type&& x, const allocator_type& allocator);
		// this_type& operator=(this_type&& x);
		// void push_front(value_type&& value);
		// void push_back(value_type&& value);
		// iterator insert(const_iterator position, value_type&& value);

		using namespace eastl;

		deque<TestObject> deque3TO33(3, TestObject(33));
		deque<TestObject> toDequeA(eastl::move(deque3TO33));
		EATEST_VERIFY((toDequeA.size() == 3) && (toDequeA.front().mX == 33) && (deque3TO33.size() == 0));

		// The following is not as strong a test of this ctor as it could be. A stronger test would be to use IntanceAllocator with different instances.
		deque<TestObject, MallocAllocator> deque4TO44(4, TestObject(44));
		deque<TestObject, MallocAllocator> toDequeB(eastl::move(deque4TO44), MallocAllocator());
		EATEST_VERIFY((toDequeB.size() == 4) && (toDequeB.front().mX == 44) && (deque4TO44.size() == 0));

		deque<TestObject, MallocAllocator> deque5TO55(5, TestObject(55));
		toDequeB = eastl::move(deque5TO55);
		EATEST_VERIFY((toDequeB.size() == 5) && (toDequeB.front().mX == 55) && (deque5TO55.size() == 0));
	}


	{   // C++11 functionality
		// template<class... Args>
		// iterator emplace(const_iterator position, Args&&... args);

		// template<class... Args>
		// void emplace_front(Args&&... args);

		// template<class... Args>
		// void emplace_back(Args&&... args);
		TestObject::Reset();

		deque<TestObject, eastl::allocator, 16> toDequeA;

		toDequeA.emplace_back(2, 3, 4);
		EATEST_VERIFY_F((toDequeA.size() == 1) && (toDequeA.back().mX == (2+3+4)) && (TestObject::sTOCtorCount == 1), "size: %u, mX: %u, count: %d", (unsigned)toDequeA.size(), (unsigned)toDequeA.back().mX, (int)TestObject::sTOCtorCount);

		toDequeA.emplace(toDequeA.begin(), 3, 4, 5);                                                              // This is 3 because of how subarray allocation works.
		EATEST_VERIFY_F((toDequeA.size() == 2) && (toDequeA.front().mX == (3+4+5)) && (TestObject::sTOCtorCount == 3), "size: %u, mX: %u, count: %d", (unsigned)toDequeA.size(), (unsigned)toDequeA.front().mX, (int)TestObject::sTOCtorCount);

		toDequeA.emplace_front(6, 7, 8);
		EATEST_VERIFY_F((toDequeA.size() == 3) && (toDequeA.front().mX == (6+7+8)) && (TestObject::sTOCtorCount == 4), "size: %u, mX: %u, count: %d", (unsigned)toDequeA.size(), (unsigned)toDequeA.front().mX, (int)TestObject::sTOCtorCount);


		// This test is similar to the emplace pathway above. 
		TestObject::Reset();

		//void push_front(T&& x);
		//void push_back(T&& x);
		//iterator insert(const_iterator position, T&& x);

		deque<TestObject, eastl::allocator, 16> toDequeC; // Specify a non-small kSubarrayCount of 16 because the move count tests below assume there is no reallocation.

		toDequeC.push_back(TestObject(2, 3, 4));
		EATEST_VERIFY((toDequeC.size() == 1) && (toDequeC.back().mX == (2+3+4)) && (TestObject::sTOMoveCtorCount == 1));

		toDequeC.insert(toDequeC.begin(), TestObject(3, 4, 5));
		EATEST_VERIFY((toDequeC.size() == 2) && (toDequeC.front().mX == (3+4+5)) && (TestObject::sTOMoveCtorCount == 3));

		toDequeC.push_front(TestObject(6, 7, 8));
		EATEST_VERIFY((toDequeC.size() == 3) && (toDequeC.front().mX == (6+7+8)) && (TestObject::sTOMoveCtorCount == 4));
	}


	{
		// Regression of deque::operator= for the case of EASTL_ALLOCATOR_COPY_ENABLED=1
		// For this test we need to use InstanceAllocator to create two containers of the same
		// type but with different and unequal allocator instances. The bug was that when 
		// EASTL_ALLOCATOR_COPY_ENABLED was enabled operator=(this_type& x) assigned x.mAllocator
		// to this and then proceeded to assign member elements from x to this. That's invalid 
		// because the existing elements of this were allocated by a different allocator and 
		// will be freed in the future with the allocator copied from x. 
		// The test below should work for the case of EASTL_ALLOCATOR_COPY_ENABLED == 0 or 1.
		InstanceAllocator::reset_all();

		InstanceAllocator ia0((uint8_t)0);
		InstanceAllocator ia1((uint8_t)1);

		eastl::deque<int, InstanceAllocator> v0((eastl_size_t)1, (int)0, ia0);
		eastl::deque<int, InstanceAllocator> v1((eastl_size_t)1, (int)1, ia1);

		EATEST_VERIFY((v0.front() == 0) && (v1.front() == 1));
		#if EASTL_ALLOCATOR_COPY_ENABLED
			EATEST_VERIFY(v0.get_allocator() != v1.get_allocator());
		#endif
		v0 = v1;
		EATEST_VERIFY((v0.front() == 1) && (v1.front() == 1));
		EATEST_VERIFY(InstanceAllocator::mMismatchCount == 0);
		EATEST_VERIFY(v0.validate());
		EATEST_VERIFY(v1.validate());
		#if EASTL_ALLOCATOR_COPY_ENABLED
			EATEST_VERIFY(v0.get_allocator() == v1.get_allocator());
		#endif
	}


	{   // Regression of kDequeSubarraySize calculations
		EATEST_VERIFY(IntDeque::kSubarraySize      >= 4);
		EATEST_VERIFY(IntDeque1::kSubarraySize     == 1);
		EATEST_VERIFY(IntDeque32768::kSubarraySize == 32768);

		EATEST_VERIFY(TestObjectDeque::kSubarraySize       >= 2);
		EATEST_VERIFY(TestObjectDeque1::kSubarraySize      == 1);
		EATEST_VERIFY(TestObjectDeque32768::kSubarraySize  == 32768);
	}


	{   // Regression of user-reported bug

		// The following was reported by Nicolas Mercier on April 9, 2008 as causing a crash:
		//     This code breaks on our machines because it overwrites the 
		//     first 4 bytes before the beginning of the memory that was 
		//     allocated for mpPtrArray. So when temp goes out of scope, 
		//     it will free this pointer and the debug allocator will detect 
		//     that these bytes have been changed.

		eastl::deque<eastl::string> testArray;
		eastl::string s("a");

		for(int j = 0; j < 65; j++)
			testArray.push_back(s);

		eastl::deque<eastl::string> temp;
		temp = testArray;                     // This is where the corruption occurred.
	}


	{   // Regression of user-reported bug

		// The problem is that the pointer arrays on the deques are growing without bound. 
		// This is causing our game to crash on a soak test due to its frame event queues 
		// consuming inordinate amounts of memory. It looks like the current version of 
		// eastl::deque is missing logic to recenter the pointer array, so it keeps growing 
		// slowly as blocks are allocated on the tail and removed from the head. 
		// Note: This bug was introduced by the (mistaken) fix for April 9 bug above.

		eastl::deque<int, MallocAllocator> x;
		eastl::deque<int, MallocAllocator> y;

		const MallocAllocator& maX = x.get_allocator();
		const MallocAllocator& maY = y.get_allocator();

		size_t allocVolumeX1 = 0;
		size_t allocVolumeY1 = 0;
		size_t allocVolumeX2 = 0;
		size_t allocVolumeY2 = 0;

		for(int i = 0; i < 1001; ++i)  // With the bug, each time through this loop the containers mistakenly allocate more memory.
		{
			if(i == 100) // Save the allocated volume after 50 iterations.
			{
				allocVolumeX1 = maX.mAllocVolume;
				allocVolumeY1 = maY.mAllocVolume;
			}

			for(int j = 0; j < 5; ++j)
				x.push_back(0);

			x.swap(y);

			while(!x.empty())
				x.pop_front();
		}

		allocVolumeX2 = maX.mAllocVolume; // Save the allocated volume after 1001 iterations.
		allocVolumeY2 = maY.mAllocVolume;

		EATEST_VERIFY((allocVolumeX1 == allocVolumeX2) && (allocVolumeX2 < 350));  // Test that the volume has not changed and is below some nominal value.
		EATEST_VERIFY((allocVolumeY1 == allocVolumeY2) && (allocVolumeY2 < 350));  // This value is somewhat arbitrary and slightly hardware dependent (e.g. 32 vs. 64 bit). I bumped it up from 300 to 350 when Linux64 showed it to be 320, which was ~still OK.
	}


	{ // Regression to verify that const deque works.
		const eastl::deque<int> constIntDeque1;
		EATEST_VERIFY(constIntDeque1.empty());

		int intArray[3] = { 37, 38, 39 };
		const eastl::deque<int> constIntDeque2(intArray, intArray + 3);
		EATEST_VERIFY(constIntDeque2.size() == 3);

		const eastl::deque<int> constIntDeque3(4, 37);
		EATEST_VERIFY(constIntDeque3.size() == 4);

		const eastl::deque<int> constIntDeque4;
		const eastl::deque<int> constIntDeque5 = constIntDeque4;
	}

	{
		// test shrink_to_fit 
		eastl::deque<int, CountingAllocator> d(4096);
		d.erase(d.begin(), d.end());

		auto prev = d.get_allocator().getActiveAllocationSize();
		d.shrink_to_fit();
		EATEST_VERIFY(d.get_allocator().getActiveAllocationSize() < prev);
	}

	{
	#ifndef EASTL_OPENSOURCE
		auto prevAllocCount = gEASTLTest_AllocationCount;
	#endif
		{
			EA_DISABLE_VC_WARNING(4625 4626)
			struct a
			{
				a(int* p)
					: ptr(p) { }

				eastl::unique_ptr<int> ptr;
			};
			EA_RESTORE_VC_WARNING()

			EASTL_INTERNAL_DISABLE_DEPRECATED() // 'eastl::has_trivial_relocate<TestDeque::a>': was declared deprecated
			static_assert(eastl::has_trivial_relocate<a>::value == false, "failure");
			EASTL_INTERNAL_RESTORE_DEPRECATED()
			static_assert(eastl::is_trivial<a>::value == false, "failure");
			static_assert(eastl::is_trivially_constructible<a>::value == false, "failure");
			static_assert(eastl::is_trivially_copyable<a>::value == false, "failure");

			eastl::deque<a> d;

			d.emplace_back(new int(1));
			d.emplace_back(new int(2));
			d.emplace_back(new int(3));

			d.erase(d.begin() + 1);
		}
	#ifndef EASTL_OPENSOURCE
		EATEST_VERIFY(gEASTLTest_AllocationCount == prevAllocCount);
	#endif
	}


	{ // Test erase / erase_if
		{
			eastl::deque<int> d = {1, 2, 3, 4, 5, 6, 7, 8, 9};

			auto numErased = eastl::erase(d, 2);
			EATEST_VERIFY((d == eastl::deque<int>{1, 3, 4, 5, 6, 7, 8, 9}));
		    EATEST_VERIFY(numErased == 1);

			numErased = eastl::erase(d, 7);
			EATEST_VERIFY((d == eastl::deque<int>{1, 3, 4, 5, 6, 8, 9}));
		    EATEST_VERIFY(numErased == 1);

			numErased = eastl::erase(d, 9);
			EATEST_VERIFY((d == eastl::deque<int>{1, 3, 4, 5, 6, 8}));
		    EATEST_VERIFY(numErased == 1);

			numErased = eastl::erase(d, 5);
			EATEST_VERIFY((d == eastl::deque<int>{1, 3, 4, 6, 8}));
		    EATEST_VERIFY(numErased == 1);

			numErased = eastl::erase(d, 3);
			EATEST_VERIFY((d == eastl::deque<int>{1, 4, 6, 8}));
		    EATEST_VERIFY(numErased == 1);
		}

		{
			eastl::deque<int> d = {1, 2, 3, 4, 5, 6, 7, 8, 9};
			auto numErased = eastl::erase_if(d, [](auto i) { return i % 2 == 0; });
			EATEST_VERIFY((d == eastl::deque<int>{1, 3, 5, 7, 9}));
		    EATEST_VERIFY(numErased == 4);
		}
	}

	{
		eastl::deque<TriviallyCopyableWithCopy> d1;
		eastl::deque<TriviallyCopyableWithCopy> d2{d1};
	}

	{
		eastl::deque<TriviallyCopyableWithMove> d1;
		eastl::deque<TriviallyCopyableWithMove> d2{ eastl::move(d1) };
	}

	{
		// unusual type - not well supported: eastl containers implicitly assume that ctor and operator= are both defined.
		eastl::deque<TriviallyCopyableWithCopyCtor> d1;
		eastl::deque<TriviallyCopyableWithCopyCtor> d2{ d1 };
	}

	{
		// unusual type - not well supported: eastl containers implicitly assume that ctor and operator= are both defined.
		eastl::deque<TriviallyCopyableWithMoveCtor> d1;
		eastl::deque<TriviallyCopyableWithMoveCtor> d2{ eastl::move(d1) };
	}

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)

	{ // Test <=>
	    eastl::deque<int> d1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	    eastl::deque<int> d2 = {9, 8, 7, 6, 5, 4, 3, 2, 1};
	    eastl::deque<int> d3 = {1, 2, 3, 4, 5};
	    eastl::deque<int> d4 = {10};

		EATEST_VERIFY(d1 != d2);
		EATEST_VERIFY(d1 < d2);
		EATEST_VERIFY(d1 != d3);
		EATEST_VERIFY(d1 > d3);
		EATEST_VERIFY(d4 > d1);
		EATEST_VERIFY(d4 > d2);
		EATEST_VERIFY(d4 > d3);

		EATEST_VERIFY((d1 <=> d2) != 0);
		EATEST_VERIFY((d1 <=> d2) < 0);
		EATEST_VERIFY((d1 <=> d3) != 0);
		EATEST_VERIFY((d1 <=> d3) > 0);
		EATEST_VERIFY((d4 <=> d1) > 0);
		EATEST_VERIFY((d4 <=> d2) > 0);
		EATEST_VERIFY((d4 <=> d3) > 0);
	}
#endif

	return nErrorCount;
}
























