/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EABase/eabase.h>
#include <EASTL/deque.h>
#include <EASTL/list.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/algorithm.h>
#include <EASTL/unique_ptr.h>
#include "ConceptImpls.h"

#if !defined(EA_COMPILER_NO_STANDARD_CPP_LIBRARY)
	EA_DISABLE_ALL_VC_WARNINGS()
	#include <deque>
	#include <list>
	#include <vector>
	#include <algorithm>
	#include <stdio.h>
	EA_RESTORE_ALL_VC_WARNINGS()
#endif


using namespace eastl;




///////////////////////////////////////////////////////////////////////////////
// DequeObject
// 
struct DequeObject
{
	int mX;                         // Value for the DequeObject.
	uint32_t mMagicValue;           // 
	static int sDOCount;            // Count of all current existing DequeObjects.
	static int sMagicErrorCount;    // Number of magic number mismatch errors.

	DequeObject(int x = 0) : mX(x), mMagicValue(kMagicValue)
		{ ++sDOCount; }

	DequeObject(const DequeObject& dequeObject) : mX(dequeObject.mX), mMagicValue(kMagicValue)
		{ ++sDOCount; }

	DequeObject& operator=(const DequeObject& dequeObject)
	{
		mX = dequeObject.mX;
		return *this;
	}

   ~DequeObject()
	{
		if(mMagicValue != kMagicValue)
			++sMagicErrorCount;
		mMagicValue = 0;
		--sDOCount;
	}
};

int DequeObject::sDOCount = 0;
int DequeObject::sMagicErrorCount = 0;


bool operator==(const DequeObject& de1, const DequeObject& de2)
	{ return de1.mX == de2.mX; }

bool operator<(const DequeObject& de1, const DequeObject& de2)
	{ return de1.mX < de2.mX; }
///////////////////////////////////////////////////////////////////////////////



// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::deque<int>;
template class eastl::deque<DequeObject>;


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
typedef eastl::deque<int>                            EIntDeque;
typedef eastl::deque<int, EASTLAllocatorType, 1>     EIntDeque1;
typedef eastl::deque<int, EASTLAllocatorType, 32768> EIntDeque32768;


typedef eastl::deque<DequeObject>                            EDODeque;
typedef eastl::deque<DequeObject, EASTLAllocatorType, 1>     EDODeque1;
typedef eastl::deque<DequeObject, EASTLAllocatorType, 32768> EDODeque32768;


#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	typedef std::deque<int>         SIntDeque;
	typedef std::deque<DequeObject> SDODeque;
#endif
///////////////////////////////////////////////////////////////////////////////



#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY


template <typename D1, typename D2>
int CompareDeques(const D1& d1, const D2& d2, const char* pTestName)
{
	int nErrorCount = 0;

	// Compare emptiness.
	VERIFY(d1.empty() == d2.empty());

	// Compare sizes.
	const size_t nSize1 = d1.size();
	const size_t nSize2 = d2.size();

	VERIFY(nSize1 == nSize2);
	if(nSize1 != nSize2)
		EASTLTest_Printf("%s: Deque size difference: %u, %u", pTestName, (unsigned)nSize1, (unsigned)nSize2);

	// Compare values.
	if(nSize1 == nSize2)
	{
		// Test operator[]
		for(unsigned i = 0; i < nSize1; i++)
		{
			const typename D1::value_type& t1 = d1[i];
			const typename D2::value_type& t2 = d2[i];

			VERIFY(t1 == t2);
			if(!(t1 == t2))
			{
				EASTLTest_Printf("%s: Deque index difference at index %d", pTestName, i);
				break;  
			}
		}

		// Test iteration
		typename D1::const_iterator it1 = d1.begin();
		typename D2::const_iterator it2 = d2.begin();

		for(unsigned j = 0; it1 != d1.end(); ++it1, ++it2, ++j)
		{
			const typename D1::value_type& t1 = *it1;
			const typename D2::value_type& t2 = *it2;

			VERIFY(t1 == t2);
			if(!(t1 == t2))
			{
				EASTLTest_Printf("%s: Deque iterator difference at index %d", pTestName, j);
				break;  
			}
		}

		// Test reverse iteration
		typename D1::const_reverse_iterator itr1 = d1.rbegin();
		typename D2::const_reverse_iterator itr2 = d2.rbegin();

		for(typename D1::size_type j = d1.size() - 1; itr1 != d1.rend(); ++itr1, ++itr2, --j)
		{
			const typename D1::value_type& t1 = *itr1;
			const typename D2::value_type& t2 = *itr2;

			VERIFY(t1 == t2);
			if(!(t1 == t2))
			{
				EASTLTest_Printf("%s: Deque reverse iterator difference at index %u", pTestName, (unsigned)j);
				break;  
			}
		}
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestDequeConstruction
//
template <typename D1, typename D2>
int TestDequeConstruction()
{
	int nErrorCount = 0;

	{
		D1 d1A;
		D2 d2A;
		nErrorCount += CompareDeques(d1A, d2A, "Deque ctor");

		D1 d1B((typename D1::size_type)0);
		D2 d2B((typename D2::size_type)0);
		nErrorCount += CompareDeques(d1B, d2B, "Deque ctor");

		D1 d1C(1000);
		D2 d2C(1000);
		nErrorCount += CompareDeques(d1C, d2C, "Deque ctor");

		D1 d1D(2000, 1);
		D2 d2D(2000, 1);
		nErrorCount += CompareDeques(d1D, d2D, "Deque ctor");

		D1 d1E(d1C);
		D2 d2E(d2C);
		nErrorCount += CompareDeques(d1E, d2E, "Deque ctor");

		D1 d1F(d1C.begin(), d1C.end());
		D2 d2F(d2C.begin(), d2C.end());
		nErrorCount += CompareDeques(d1F, d2F, "Deque ctor");

		// operator=
		d1E = d1D;
		d2E = d2D;
		nErrorCount += CompareDeques(d1D, d2D, "Deque operator=");
		nErrorCount += CompareDeques(d1E, d2E, "Deque operator=");

		// swap
		d1E.swap(d1D);
		d2E.swap(d2D);
		nErrorCount += CompareDeques(d1D, d2D, "Deque swap");
		nErrorCount += CompareDeques(d1E, d2E, "Deque swap");

		// clear
		d1A.clear();
		d2A.clear();
		nErrorCount += CompareDeques(d1A, d2A, "Deque clear");

		d1B.clear();
		d2B.clear();
		nErrorCount += CompareDeques(d1B, d2B, "Deque clear");
	}

	VERIFY(DequeObject::sDOCount == 0);
	VERIFY(DequeObject::sMagicErrorCount == 0);

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestDequeSimpleMutation
//
template <typename D1, typename D2>
int TestDequeSimpleMutation()
{
	int nErrorCount = 0;

	{
		D1 d1;
		D2 d2;

		// push_back(value_type&)
		// front
		// back
		for(int i = 0; i < 1000; i++)
		{
			d1.push_back(i);
			d2.push_back(i);
			VERIFY(d1.front() == d2.front()); 
			VERIFY(d1.back()  == d2.back()); 
		}
		nErrorCount += CompareDeques(d1, d2, "Deque push_back(value_type&)");

		// operator[]
		// at()
		for(typename D1::size_type i = 0, iEnd = d1.size(); i < iEnd; i++)
		{
			VERIFY(d1[(unsigned)i]    == d2[(unsigned)i]); 
			VERIFY(d1.at((unsigned)i) == d2.at((unsigned)i)); 
		}

		// push_back()
		for(int i = 0; i < 1000; i++)
		{
			d1.push_back(int());
			typename D2::value_type& ref = d2.push_back();       // d2 here must be the EASTL version.
			VERIFY(d1.front() == d2.front()); 
			VERIFY(d1.back()  == d2.back()); 
			VERIFY(&ref       == &d2.back()); 
		}
		nErrorCount += CompareDeques(d1, d2, "Deque push_back()");

		// operator[]
		// at()
		for(typename D1::size_type i = 0, iEnd = d1.size(); i < iEnd; i++)
		{
			VERIFY(d1[(unsigned)i]    == d2[(unsigned)i]); 
			VERIFY(d1.at((unsigned)i) == d2.at((unsigned)i)); 
		}

		// push_front(value_type&)
		for(int i = 0; i < 1000; i++)
		{
			d1.push_front(i);
			d2.push_front(i);
			VERIFY(d1.front() == d2.front()); 
			VERIFY(d1.back()  == d2.back()); 
		}
		nErrorCount += CompareDeques(d1, d2, "Deque push_front(value_type&)");

		// operator[]
		// at()
		for(typename D1::size_type i = 0, iEnd = d1.size(); i < iEnd; i++)
		{
			VERIFY(d1[(unsigned)i]    == d2[(unsigned)i]); 
			VERIFY(d1.at((unsigned)i) == d2.at((unsigned)i)); 
		}

		// push_front()
		for(int i = 0; i < 1000; i++)
		{
			d1.push_front(int());
			typename D2::value_type& ref = d2.push_front();
			VERIFY(d1.front() == d2.front()); 
			VERIFY(d1.back()  == d2.back()); 
			VERIFY(&ref == &d2.front()); 
		}
		nErrorCount += CompareDeques(d1, d2, "Deque push_front()");

		// operator[]
		// at()
		for(typename D1::size_type i = 0, iEnd = d1.size(); i < iEnd; i++)
		{
			VERIFY(d1[(unsigned)i]    == d2[(unsigned)i]); 
			VERIFY(d1.at((unsigned)i) == d2.at((unsigned)i)); 
		}

		// pop_back()
		for(int i = 0; i < 500; i++)
		{
			d1.pop_back();
			d2.pop_back();
			VERIFY(d1.front() == d2.front()); 
			VERIFY(d1.back()  == d2.back()); 
		}
		nErrorCount += CompareDeques(d1, d2, "Deque pop_back()");

		// operator[]
		// at()
		for(typename D1::size_type i = 0, iEnd = d1.size(); i < iEnd; i++)
		{
			VERIFY(d1[(unsigned)i]    == d2[(unsigned)i]); 
			VERIFY(d1.at((unsigned)i) == d2.at((unsigned)i)); 
		}

		// pop_front()
		for(int i = 0; i < 500; i++)
		{
			d1.pop_front();
			d2.pop_front();
			VERIFY(d1.front() == d2.front()); 
			VERIFY(d1.back()  == d2.back()); 
		}
		nErrorCount += CompareDeques(d1, d2, "Deque pop_front()");

		// operator[]
		// at()
		for(typename D1::size_type i = 0, iEnd = d1.size(); i < iEnd; i++)
		{
			VERIFY(d1[(unsigned)i]    == d2[(unsigned)i]); 
			VERIFY(d1.at((unsigned)i) == d2.at((unsigned)i)); 
		}

		// resize(value_type&)
		for(int i = 0; i < 500; i++)
		{
			d1.resize(d1.size() + 3, i);
			d2.resize(d2.size() + 3, i);
			VERIFY(d1.front() == d2.front()); 
			VERIFY(d1.back()  == d2.back()); 
		}
		nErrorCount += CompareDeques(d1, d2, "Deque resize(value_type&)");

		// operator[]
		// at()
		for(typename D1::size_type i = 0, iEnd = d1.size(); i < iEnd; i++)
		{
			VERIFY(d1[(unsigned)i]    == d2[(unsigned)i]); 
			VERIFY(d1.at((unsigned)i) == d2.at((unsigned)i)); 
		}

		// resize()
		for(int i = 0; i < 500; i++)
		{
			d1.resize(d1.size() - 2);
			d2.resize(d2.size() - 2);
			VERIFY(d1.front() == d2.front()); 
			VERIFY(d1.back()  == d2.back()); 
		}
		nErrorCount += CompareDeques(d1, d2, "Deque resize()");

		// operator[]
		// at()
		for(typename D1::size_type i = 0, iEnd = d1.size(); i < iEnd; i++)
		{
			VERIFY(d1[(unsigned)i]    == d2[(unsigned)i]); 
			VERIFY(d1.at((unsigned)i) == d2.at((unsigned)i)); 
		}
	}

	VERIFY(DequeObject::sDOCount == 0);
	VERIFY(DequeObject::sMagicErrorCount == 0);

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestDequeComplexMutation
//
template <typename D1, typename D2>
int TestDequeComplexMutation()
{
	int nErrorCount = 0;

	{
		D1 d1;
		D2 d2;


		//////////////////////////////////////////////////////////////////
		// void assign(size_type n, const value_type& value);
		//////////////////////////////////////////////////////////////////

		d1.assign(100, 1);
		d2.assign(100, 1);
		nErrorCount += CompareDeques(d1, d2, "Deque assign(size_type n, const value_type& value)");

		d1.assign(50, 2);
		d2.assign(50, 2);
		nErrorCount += CompareDeques(d1, d2, "Deque assign(size_type n, const value_type& value)");

		d1.assign(150, 2);
		d2.assign(150, 2);
		nErrorCount += CompareDeques(d1, d2, "Deque assign(size_type n, const value_type& value)");



		//////////////////////////////////////////////////////////////////
		// template <typename InputIterator>
		// void assign(InputIterator first, InputIterator last);
		//////////////////////////////////////////////////////////////////

		std::list<int> intList1;
		for(int i = 0; i < 100; i++)
			intList1.push_back(i);

		eastl::list<int> intList2;
		for(int i = 0; i < 100; i++)
			intList2.push_back(i);

		d1.assign(intList1.begin(), intList1.end());
		d2.assign(intList2.begin(), intList2.end());
		nErrorCount += CompareDeques(d1, d2, "Deque assign(InputIterator first, InputIterator last)");



		//////////////////////////////////////////////////////////////////
		// iterator insert(iterator position, const value_type& value);
		//////////////////////////////////////////////////////////////////

		d1.insert(d1.begin(), d1[1]);
		d2.insert(d2.begin(), d2[1]);
		nErrorCount += CompareDeques(d1, d2, "Deque insert(iterator position, const value_type& value)");

		d1.insert(d1.end(), d1[d1.size() - 2]);
		d2.insert(d2.end(), d2[d2.size() - 2]);
		nErrorCount += CompareDeques(d1, d2, "Deque insert(iterator position, const value_type& value)");

		typename D1::iterator itD1NearBegin = d1.begin();
		typename D2::iterator itD2NearBegin = d2.begin();

		std::advance(itD1NearBegin, 1);
		eastl::advance(itD2NearBegin, 1);

		d1.insert(itD1NearBegin, d1[3]);
		d2.insert(itD2NearBegin, d2[3]);
		nErrorCount += CompareDeques(d1, d2, "Deque insert(iterator position, const value_type& value)");

		typename D1::iterator itD1NearEnd = d1.begin();
		typename D2::iterator itD2NearEnd = d2.begin();

		std::advance(itD1NearEnd, d1.size() - 1);
		eastl::advance(itD2NearEnd, d2.size() - 1);

		d1.insert(itD1NearEnd, d1[d1.size() - 2]);
		d2.insert(itD2NearEnd, d2[d2.size() - 2]);
		nErrorCount += CompareDeques(d1, d2, "Deque insert(iterator position, const value_type& value)");


		//////////////////////////////////////////////////////////////////
		// void insert(iterator position, size_type n, const value_type& value);
		//////////////////////////////////////////////////////////////////

		d1.insert(d1.begin(), d1.size() * 2, 3); // Insert a large number of items at the front.
		d2.insert(d2.begin(), d2.size() * 2, 3);
		nErrorCount += CompareDeques(d1, d2, "Deque insert(iterator position, size_type n, const value_type& value)");

		d1.insert(d1.end(), d1.size() * 2, 3); // Insert a large number of items at the end.
		d2.insert(d2.end(), d2.size() * 2, 3);
		nErrorCount += CompareDeques(d1, d2, "Deque insert(iterator position, size_type n, const value_type& value)");

		itD1NearBegin = d1.begin();
		itD2NearBegin = d2.begin();

		std::advance(itD1NearBegin, 3);
		eastl::advance(itD2NearBegin, 3);

		d1.insert(itD1NearBegin, 3, 4);
		d2.insert(itD2NearBegin, 3, 4);
		nErrorCount += CompareDeques(d1, d2, "Deque insert(iterator position, size_type n, const value_type& value)");

		itD1NearEnd = d1.begin();
		itD2NearEnd = d2.begin();

		std::advance(itD1NearEnd, d1.size() - 1);
		eastl::advance(itD2NearEnd, d2.size() - 1);

		d1.insert(d1.end(), 5, 6);
		d2.insert(d2.end(), 5, 6);
		nErrorCount += CompareDeques(d1, d2, "Deque insert(iterator position, size_type n, const value_type& value)");



		//////////////////////////////////////////////////////////////////
		// template <typename InputIterator>
		// void insert(iterator position, InputIterator first, InputIterator last);
		//////////////////////////////////////////////////////////////////

		itD1NearBegin = d1.begin();
		itD2NearBegin = d2.begin();

		std::advance(itD1NearBegin, 3);
		eastl::advance(itD2NearBegin, 3);

		d1.insert(itD1NearBegin, intList1.begin(), intList1.end());
		d2.insert(itD2NearBegin, intList2.begin(), intList2.end());
		nErrorCount += CompareDeques(d1, d2, "Deque insert(iterator position, InputIterator first, InputIterator last)");



		//////////////////////////////////////////////////////////////////
		// iterator erase(iterator position);
		//////////////////////////////////////////////////////////////////

		itD1NearBegin = d1.begin();
		itD2NearBegin = d2.begin();

		while(itD1NearBegin != d1.end()) // Run a loop whereby we erase every third element.
		{
			for(int i = 0; (i < 3) && (itD1NearBegin != d1.end()); ++i)
			{
				++itD1NearBegin;
				++itD2NearBegin;
			}

			if(itD1NearBegin != d1.end())
			{
				itD1NearBegin = d1.erase(itD1NearBegin);
				itD2NearBegin = d2.erase(itD2NearBegin);
				nErrorCount += CompareDeques(d1, d2, "Deque erase(iterator position)");
			}
		}


		//////////////////////////////////////////////////////////////////
		// iterator erase(iterator first, iterator last);
		//////////////////////////////////////////////////////////////////

		itD1NearBegin = d1.begin();
		itD2NearBegin = d2.begin();

		while(itD1NearBegin != d1.end()) // Run a loop whereby we erase spans of elements.
		{
			typename D1::iterator itD1Saved = itD1NearBegin;
			typename D2::iterator itD2Saved = itD2NearBegin;

			for(int i = 0; (i < 11) && (itD1NearBegin != d1.end()); ++i)
			{
				++itD1NearBegin;
				++itD2NearBegin;
			}

			if(itD1NearBegin != d1.end())
			{
				itD1NearBegin = d1.erase(itD1Saved, itD1NearBegin);
				itD2NearBegin = d2.erase(itD2Saved, itD2NearBegin);
				nErrorCount += CompareDeques(d1, d2, "Deque erase(iterator position)");
			}

			for(int i = 0; (i < 17) && (itD1NearBegin != d1.end()); ++i)
			{
				++itD1NearBegin;
				++itD2NearBegin;
			}

		}

	}


	{
		//////////////////////////////////////////////////////////////////
		// reverse_iterator erase(reverse_iterator position);
		// reverse_iterator erase(reverse_iterator first, reverse_iterator last);
		//////////////////////////////////////////////////////////////////

		//D1 d1Erase;
		D2 d2Erase;

		for(int i = 0; i < 20; i++)
		{
			typename D2::value_type val(i);
			d2Erase.push_back(val);
		}
		VERIFY((d2Erase.size() == 20) && (d2Erase[0] == 0) && (d2Erase[19] == 19));


		typename D2::reverse_iterator r2A = d2Erase.rbegin();
		typename D2::reverse_iterator r2B = r2A + 3;
		d2Erase.erase(r2A, r2B);
		VERIFY((d2Erase.size() == 17));
		VERIFY((d2Erase[0] == 0));
		VERIFY((d2Erase[16] == 16));


		r2B = d2Erase.rend();
		r2A = r2B - 3;
		d2Erase.erase(r2A, r2B);
		VERIFY((d2Erase.size() == 14));
		VERIFY((d2Erase[0] == 3));
		VERIFY((d2Erase[13] == 16));


		r2B = d2Erase.rend() - 1;
		d2Erase.erase(r2B);
		VERIFY((d2Erase.size() == 13));
		VERIFY((d2Erase[0] == 4));
		VERIFY((d2Erase[12] == 16));


		r2B = d2Erase.rbegin();
		d2Erase.erase(r2B);
		VERIFY((d2Erase.size() == 12));
		VERIFY((d2Erase[0] == 4));
		VERIFY((d2Erase[11] == 15));


		r2A = d2Erase.rbegin();
		r2B = d2Erase.rend();
		d2Erase.erase(r2A, r2B);
		VERIFY(d2Erase.size() == 0);
	}


	VERIFY(DequeObject::sDOCount == 0);
	VERIFY(DequeObject::sMagicErrorCount == 0);

	return nErrorCount;
}

#endif // EA_COMPILER_NO_STANDARD_CPP_LIBRARY


int TestDeque()
{
	int nErrorCount = 0;

	#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
		{   // Test construction
			nErrorCount += TestDequeConstruction<SIntDeque, EIntDeque>();
			nErrorCount += TestDequeConstruction<SIntDeque, EIntDeque1>();
			nErrorCount += TestDequeConstruction<SIntDeque, EIntDeque32768>();

			nErrorCount += TestDequeConstruction<SIntDeque, EDODeque>();
			nErrorCount += TestDequeConstruction<SIntDeque, EDODeque1>();
			nErrorCount += TestDequeConstruction<SIntDeque, EDODeque32768>();
		}


		{   // Test simple mutating functionality.
			nErrorCount += TestDequeSimpleMutation<SIntDeque, EIntDeque>();
			nErrorCount += TestDequeSimpleMutation<SIntDeque, EIntDeque1>();
			nErrorCount += TestDequeSimpleMutation<SIntDeque, EIntDeque32768>();

			nErrorCount += TestDequeSimpleMutation<SIntDeque, EDODeque>();
			nErrorCount += TestDequeSimpleMutation<SIntDeque, EDODeque1>();
			nErrorCount += TestDequeSimpleMutation<SIntDeque, EDODeque32768>();
		}

		{   // Test complex mutating functionality.
			nErrorCount += TestDequeComplexMutation<SIntDeque, EIntDeque>();
			nErrorCount += TestDequeComplexMutation<SIntDeque, EIntDeque1>();
			nErrorCount += TestDequeComplexMutation<SIntDeque, EIntDeque32768>();

			nErrorCount += TestDequeComplexMutation<SIntDeque, EDODeque>();
			nErrorCount += TestDequeComplexMutation<SIntDeque, EDODeque1>();
			nErrorCount += TestDequeComplexMutation<SIntDeque, EDODeque32768>();
		}
	#endif // EA_COMPILER_NO_STANDARD_CPP_LIBRARY

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
			EATEST_VERIFY(VerifySequence(intDeque.begin(), intDeque.end(), int(), "deque std::initializer_list", 0, 1, 2, -1));

			intDeque = { 13, 14, 15 };
			EATEST_VERIFY(VerifySequence(intDeque.begin(), intDeque.end(), int(), "deque std::initializer_list", 13, 14, 15, -1));

			intDeque.assign({ 16, 17, 18 });
			EATEST_VERIFY(VerifySequence(intDeque.begin(), intDeque.end(), int(), "deque std::initializer_list", 16, 17, 18, -1));

			eastl::deque<int>::iterator it = intDeque.insert(intDeque.begin(), { 14, 15 });
			EATEST_VERIFY(VerifySequence(intDeque.begin(), intDeque.end(), int(), "deque std::initializer_list", 14, 15, 16, 17, 18, -1));
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
		VERIFY(EIntDeque::kSubarraySize      >= 4);
		VERIFY(EIntDeque1::kSubarraySize     == 1);
		VERIFY(EIntDeque32768::kSubarraySize == 32768);

		VERIFY(EDODeque::kSubarraySize       >= 2);
		VERIFY(EDODeque1::kSubarraySize      == 1);
		VERIFY(EDODeque32768::kSubarraySize  == 32768);
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

		VERIFY((allocVolumeX1 == allocVolumeX2) && (allocVolumeX2 < 350));  // Test that the volume has not changed and is below some nominal value.
		VERIFY((allocVolumeY1 == allocVolumeY2) && (allocVolumeY2 < 350));  // This value is somewhat arbitrary and slightly hardware dependent (e.g. 32 vs. 64 bit). I bumped it up from 300 to 350 when Linux64 showed it to be 320, which was ~still OK.
	}


	{ // Regression of user error report for the case of deque<const type>.
		eastl::vector<int> ctorValues;

		for(int v = 0; v < 10; v++)
			ctorValues.push_back(v);

		eastl::deque<const ConstType> testStruct(ctorValues.begin(), ctorValues.end());
		eastl::deque<const int>       testInt(ctorValues.begin(), ctorValues.end());
	}


	{ // Regression to verify that const deque works.
		const eastl::deque<int> constIntDeque1;
		VERIFY(constIntDeque1.empty());

		int intArray[3] = { 37, 38, 39 };
		const eastl::deque<int> constIntDeque2(intArray, intArray + 3);
		VERIFY(constIntDeque2.size() == 3);

		const eastl::deque<int> constIntDeque3(4, 37);
		VERIFY(constIntDeque3.size() == 4);

		const eastl::deque<int> constIntDeque4;
		const eastl::deque<int> constIntDeque5 = constIntDeque4;
	}

	{
		// test shrink_to_fit 
		eastl::deque<int, CountingAllocator> d(4096);
		d.erase(d.begin(), d.end());

		auto prev = d.get_allocator().getActiveAllocationSize();
		d.shrink_to_fit();
		VERIFY(d.get_allocator().getActiveAllocationSize() < prev);
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

			static_assert(eastl::has_trivial_relocate<a>::value == false, "failure");

			eastl::deque<a> d;

			d.emplace_back(new int(1));
			d.emplace_back(new int(2));
			d.emplace_back(new int(3));

			d.erase(d.begin() + 1);
		}
	#ifndef EASTL_OPENSOURCE
		VERIFY(gEASTLTest_AllocationCount == prevAllocCount);
	#endif
	}


	{ // Test erase / erase_if
		{
			eastl::deque<int> d = {1, 2, 3, 4, 5, 6, 7, 8, 9};

			eastl::erase(d, 2);
			VERIFY((d == eastl::deque<int>{1, 3, 4, 5, 6, 7, 8, 9}));

			eastl::erase(d, 7);
			VERIFY((d == eastl::deque<int>{1, 3, 4, 5, 6, 8, 9}));

			eastl::erase(d, 9);
			VERIFY((d == eastl::deque<int>{1, 3, 4, 5, 6, 8}));

			eastl::erase(d, 5);
			VERIFY((d == eastl::deque<int>{1, 3, 4, 6, 8}));

			eastl::erase(d, 3);
			VERIFY((d == eastl::deque<int>{1, 4, 6, 8}));
		}

		{
			eastl::deque<int> d = {1, 2, 3, 4, 5, 6, 7, 8, 9};
			eastl::erase_if(d, [](auto i) { return i % 2 == 0; });
			VERIFY((d == eastl::deque<int>{1, 3, 5, 7, 9}));
		}
	}

	return nErrorCount;
}
























