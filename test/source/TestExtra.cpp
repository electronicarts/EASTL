/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Test forward delcarations
/////////////////////////////////////////////////////////////////////////////

namespace eastl
{
	class allocator;

	template <typename T, typename Allocator> class basic_string;
	typedef basic_string<char, allocator> local_string8;  // collides with eastl::string8 in bulkbuilds

	template <typename T> struct local_less {};

	static void UseForwardDeclaredString(local_string8*)
	{
	}


	template <typename T, typename Allocator> class vector;
	typedef vector<char, allocator> vector8;

	static void UseForwardDeclaredVector(vector8*)
	{
	}


	template <typename Value, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode> class hash_set;
	typedef hash_set<char, char, local_less<char>, allocator, false> hash_set8;

	static void UseForwardDeclaredHashSet(hash_set8*)
	{
	}


	template <typename Key, typename T, typename Compare, typename Allocator> class map;
	typedef map<char, char, local_less<char>, allocator> map8;

	static void UseForwardDeclaredMap(map8*)
	{
	}
}


#include "EASTLTest.h"
#include <EASTL/functional.h>
#include <EASTL/utility.h>
#include <EASTL/memory.h>
#include <EASTL/allocator.h>
#include <EASTL/allocator_malloc.h>
#include <EASTL/fixed_allocator.h>
#include <EASTL/intrusive_list.h>
#include <EASTL/numeric.h>
#include <EASTL/queue.h>
#include <EASTL/priority_queue.h>
#include <EASTL/stack.h>
#include <EASTL/heap.h>
#include <EASTL/vector.h>
#include <EASTL/deque.h>
#include <EASTL/list.h>
#include <EASTL/map.h>
#include <EASTL/string.h>
#include <EASTL/hash_set.h>
#include <EASTL/random.h>
#include <EASTL/core_allocator_adapter.h>
#include <EASTL/bonus/call_traits.h>
#include <EASTL/bonus/compressed_pair.h>
#include <EASTL/bonus/adaptors.h>
#include <EAStdC/EAAlignment.h>
#include <EAStdC/EAMemory.h>
#include <EAStdC/EAString.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stdio.h>
#include <string.h>

#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	#include <algorithm>
	#include <utility>
	#include <stack>
	#include <queue>
	#include <vector>
	#include <deque>
	#include <math.h>
#endif

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif



using namespace eastl;



namespace
{
	/// IntNode
	///
	/// Test intrusive_list node.
	///
	struct IntNode : public eastl::intrusive_list_node
	{
		int mX;

		IntNode(int x = 0)
			: mX(x) { }

		operator int() const
			{ return mX; }
	};

	bool operator<(const IntNode& a, const IntNode& b)
		{ return a.mX < b.mX; }
}






struct TestClass
{
	mutable int mX;

	TestClass() : mX(37) { }

	void Increment()
	{
		mX++;
	}

	void IncrementConst() const
	{
		mX++;
	}

	int MultiplyBy(int x)
	{
		return mX * x;
	}

	int MultiplyByConst(int x) const
	{
		return mX * x;
	}
};




///////////////////////////////////////////////////////////////////////////////
// TestForwardDeclarations
//
static int TestForwardDeclarations()
{
	int nErrorCount = 0;

	eastl::local_string8 s8;
	UseForwardDeclaredString(&s8);   

	eastl::vector8 v8;
	UseForwardDeclaredVector(&v8);

	eastl::hash_set8 h8;
	UseForwardDeclaredHashSet(&h8);

	eastl::map8 m8;
	UseForwardDeclaredMap(&m8);

	return nErrorCount;
}




///////////////////////////////////////////////////////////////////////////////
// fixed_pool_reference
//
struct fixed_pool_reference
{
public:
	fixed_pool_reference(const char* = NULL)
	{
		mpFixedPool = NULL;
	}

	fixed_pool_reference(eastl::fixed_pool& fixedPool)
	{
		mpFixedPool = &fixedPool;
	}

	fixed_pool_reference(const fixed_pool_reference& x)
	{
		mpFixedPool = x.mpFixedPool;
	}

	fixed_pool_reference& operator=(const fixed_pool_reference& x)
	{
		mpFixedPool = x.mpFixedPool;
		return *this;
	}

	void* allocate(size_t /*n*/, int /*flags*/ = 0)
	{
		return mpFixedPool->allocate();
	}

	void* allocate(size_t /*n*/, size_t /*alignment*/, size_t /*offset*/, int /*flags*/ = 0)
	{
		return mpFixedPool->allocate();
	}

	void deallocate(void* p, size_t /*n*/)
	{
		return mpFixedPool->deallocate(p);
	}

	const char* get_name() const
	{
		return "fixed_pool_reference";
	}

	void set_name(const char* /*pName*/)
	{
	}

protected:
	friend bool operator==(const fixed_pool_reference& a, const fixed_pool_reference& b);
	friend bool operator!=(const fixed_pool_reference& a, const fixed_pool_reference& b);

	eastl::fixed_pool* mpFixedPool;
};


inline bool operator==(const fixed_pool_reference& a, const fixed_pool_reference& b)
{
	return (a.mpFixedPool == b.mpFixedPool);
}

inline bool operator!=(const fixed_pool_reference& a, const fixed_pool_reference& b)
{
	return (a.mpFixedPool != b.mpFixedPool);
}





// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::queue<int, deque<int> >;
template class eastl::queue<Align64, deque<Align64> >;
template class eastl::queue<TestObject, list<TestObject> >;
//template class eastl::queue<IntNode, intrusive_list<IntNode> >;// This test has been disabled as of the addition of initializer_list support to eastl::queue. initializer_lists have const nodes, which is incompatible with intrusive_list. You can use eastl::queue<IntNode, intrusive_list<IntNode> > as long as you don't use initializer_list with it. The problem with this line of code is that it forces compilation of the entire class.


///////////////////////////////////////////////////////////////////////////////
// TestQueue
//
static int TestQueue()
{
	int nErrorCount = 0;

	{
		// Exercise IntNode.
		IntNode x, y;
		EATEST_VERIFY((x < y) || !(x < y) || ((int)x < (int)y));
	}

	TestObject::Reset();

	{
		// queue(const Sequence& x = Sequence());
		queue<TestObject, list<TestObject>> toListQueue;
		queue<TestObject, list<TestObject>> toListQueue2;


		// global operators
		EATEST_VERIFY( (toListQueue == toListQueue2));
		EATEST_VERIFY(!(toListQueue != toListQueue2));
		EATEST_VERIFY( (toListQueue <= toListQueue2));
		EATEST_VERIFY( (toListQueue >= toListQueue2));
		EATEST_VERIFY(!(toListQueue <  toListQueue2));
		EATEST_VERIFY(!(toListQueue >  toListQueue2));


		// bool      empty() const;
		// size_type size() const;
		EATEST_VERIFY(toListQueue.empty());
		EATEST_VERIFY(toListQueue.size() == 0);


		// void            push(const value_type& value);
		// reference       front();
		// const_reference front() const;
		// reference       back();
		// const_reference back() const;
		toListQueue.push(TestObject(0));
		EATEST_VERIFY(toListQueue.front() == TestObject(0));
		EATEST_VERIFY(toListQueue.back()  == TestObject(0));

		toListQueue.push(TestObject(1));
		EATEST_VERIFY(toListQueue.front() == TestObject(0));
		EATEST_VERIFY(toListQueue.back()  == TestObject(1));

		toListQueue.push(TestObject(2));
		EATEST_VERIFY(toListQueue.front() == TestObject(0));
		EATEST_VERIFY(toListQueue.back()  == TestObject(2));
		EATEST_VERIFY(!toListQueue.empty());
		EATEST_VERIFY(toListQueue.size() == 3);


		// void pop();
		toListQueue.pop();
		EATEST_VERIFY(toListQueue.front() == TestObject(1));
		EATEST_VERIFY(toListQueue.back()  == TestObject(2));

		toListQueue.pop();
		EATEST_VERIFY(toListQueue.front() == TestObject(2));
		EATEST_VERIFY(toListQueue.back()  == TestObject(2));

		toListQueue.pop();
		EATEST_VERIFY(toListQueue.empty());
		EATEST_VERIFY(toListQueue.size() == 0);


		// decltype(auto) emplace(Args&&... args);
		toListQueue.emplace(1);
		EATEST_VERIFY(!toListQueue.empty());
		EATEST_VERIFY(toListQueue.front() == TestObject(1));
		EATEST_VERIFY(toListQueue.size() == 1);


		// container_type&       get_container();
		// const container_type& get_container() const;
		list<TestObject>& ref = toListQueue.get_container();
		EATEST_VERIFY(ref.size() == toListQueue.size());


		// queue(std::initializer_list<value_type> ilist);
		queue<int> intQueue = { 3, 4, 5 };
		EATEST_VERIFY(intQueue.size() == 3);
		EATEST_VERIFY(intQueue.front() == 3);
		intQueue.pop();
		EATEST_VERIFY(intQueue.front() == 4);
		intQueue.pop();
		EATEST_VERIFY(intQueue.front() == 5);
	}

	{
		vector<TestObject> toVector;
		for(int i = 0; i < 100; i++)
			toVector.push_back(TestObject(i));

		// template <class Allocator>
		// queue(this_type&& x, const Allocator& allocator, typename eastl::enable_if<eastl::uses_allocator<container_type, Allocator>::value>::type* = NULL);
		//
		// explicit queue(container_type&& x);
		//
		// void push(value_type&& x);

		queue<TestObject, vector<TestObject> > toQ_0;
		queue<TestObject, vector<TestObject> > toQ_A(eastl::move(toQ_0), toQ_0.get_container().get_allocator()); // It would be better if we also tested an alternative allocator.
		EATEST_VERIFY(toQ_A.size() == 0);
		toQ_A.push(TestObject(1000));
		EATEST_VERIFY(toQ_A.size() == 1);

		queue<TestObject, vector<TestObject> > toQ_B(eastl::move(toQ_A), toQ_A.get_container().get_allocator()); // It would be better if we also tested an alternative allocator.
		EATEST_VERIFY((toQ_B.size() == 1) && toQ_A.empty());

		eastl::vector<TestObject> toVectorM(toVector);
		queue<TestObject, vector<TestObject> > toQ_C(eastl::move(toVectorM));
		EATEST_VERIFY((toQ_C.size() == toVector.size()) && toVectorM.empty());

		// template <class... Args>
		// void emplace_back(Args&&... args);

		queue<TestObject, vector<TestObject> > toQ_D;
		toQ_D.emplace(0, 1, 2);
		EATEST_VERIFY(toQ_D.size() == 1) && (toQ_D.back() == TestObject(0, 1, 2));
	}


	{ // Test std namespace elements contained in queue
		#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
			eastl::queue< std::pair<int, int> > stlQueue;
			stlQueue.push(std::make_pair(1, 1));
			EATEST_VERIFY(stlQueue.size() == 1);
		#endif
	}


	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();


	return nErrorCount;
}






// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::priority_queue<int, vector<int> >;
template class eastl::priority_queue<Align64, deque<Align64> >;
template class eastl::priority_queue<TestObject, vector<TestObject> >;
template class eastl::priority_queue<float, vector<float>, less<float> >;


///////////////////////////////////////////////////////////////////////////////
// TestPriorityQueue
//
static int TestPriorityQueue()
{
	int nErrorCount = 0;

	EASTLTest_Rand rng(EA::UnitTest::GetRandSeed());

	TestObject::Reset();

	{
		less<TestObject> toLess;

		vector<TestObject> toVector;
		for(int i = 0; i < 100; i++)
			toVector.push_back(TestObject(i));
		random_shuffle(toVector.begin(), toVector.end(), rng);

		list<TestObject> toList;
		for(eastl_size_t j = 0; j < 100; j++)
			toList.push_back(toVector[j]);


		// priority_queue(const Compare& compare = Compare(), const Sequence& x = Sequence());
		// template <typename InputIterator>
		// priority_queue(InputIterator first, InputIterator last, const Compare& compare = Compare(), const Sequence& x = Sequence());
		priority_queue<TestObject, vector<TestObject> > toPQ;
		priority_queue<TestObject, vector<TestObject> > toPQV(toLess, toVector);
		priority_queue<TestObject, vector<TestObject> > toPQL(toList.begin(), toList.end());

		EATEST_VERIFY(toPQ.empty());
		EATEST_VERIFY(toPQ.size() == 0);

		EATEST_VERIFY(!toPQV.empty());
		EATEST_VERIFY( toPQV.size() == toVector.size());

		EATEST_VERIFY(!toPQL.empty());
		EATEST_VERIFY( toPQL.size() == toList.size());


		// global operators
		EATEST_VERIFY( (toPQ  != toPQL));
		EATEST_VERIFY( (toPQV == toPQL));
		EATEST_VERIFY(!(toPQV != toPQL));
		EATEST_VERIFY( (toPQV <= toPQL));
		EATEST_VERIFY( (toPQV >= toPQL));
		EATEST_VERIFY(!(toPQV <  toPQL));
		EATEST_VERIFY(!(toPQV >  toPQL));


		// container_type&       get_container();
		// const container_type& get_container() const;
		vector<TestObject>& ref = toPQL.get_container();
		EATEST_VERIFY(ref.size() == toPQL.size());
		EATEST_VERIFY(is_heap(ref.begin(), ref.end()));

		// bool validate() const;
		EATEST_VERIFY(toPQL.validate());
		// To consider: Verify that validate detects an invalid heap. 
		// Testing this might be an issue if the validation function actively complains in some way.


		// const_reference top() const;
		// void pop();
		const TestObject& to1 = toPQL.top();
		EATEST_VERIFY(to1 == TestObject(99));

		toPQL.pop();
		EATEST_VERIFY(!toPQL.empty());
		EATEST_VERIFY( toPQL.size() == toList.size() - 1);
		EATEST_VERIFY(to1 == TestObject(98));
		EATEST_VERIFY(is_heap(ref.begin(), ref.end()));


		// void push(const value_type& value);
		toPQL.push(TestObject(1000));
		EATEST_VERIFY(toPQL.size() == toList.size());
		const TestObject& to2 = toPQL.top();
		EATEST_VERIFY(to2 == TestObject(1000));
		toPQL.pop();
		const TestObject& to3 = toPQL.top();
		EATEST_VERIFY(to3 == TestObject(98));
		EATEST_VERIFY(is_heap(ref.begin(), ref.end()));


		// void change(size_type n);
		TestObject& to4 = ref[50];
		to4 = TestObject(2000);
		toPQL.change(50);
		const TestObject& to5 = toPQL.top();
		EATEST_VERIFY(to5 == TestObject(2000));
		EATEST_VERIFY(is_heap(ref.begin(), ref.end()));


		// void remove(size_type n);
		TestObject to6 = ref[20];
		toPQL.remove(20);
		EATEST_VERIFY( toPQL.size() == toList.size() - 2);
		TestObject& to7 = ref[20];
		EATEST_VERIFY(!(to6 == to7));
		EATEST_VERIFY(is_heap(ref.begin(), ref.end()));


		// priority_queue(std::initializer_list<value_type> ilist, const compare_type& compare = compare_type());
		#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
			priority_queue<int, vector<int> > intPQ = { 3, 4, 5 };
			EATEST_VERIFY(intPQ.size() == 3);
			EATEST_VERIFY(intPQ.top() == 5);
			intPQ.pop();
			EATEST_VERIFY(intPQ.top() == 4);
			intPQ.pop();
			EATEST_VERIFY(intPQ.top() == 3);
		#endif
	}

	{
		vector<TestObject> toVector;
		for(int i = 0; i < 100; i++)
			toVector.push_back(TestObject(i));

		// template <class Allocator>
		// priority_queue(this_type&& x, const Allocator& allocator, typename eastl::enable_if<eastl::uses_allocator<container_type, Allocator>::value>::type* = NULL);
		//
		// explicit priority_queue(const compare_type& compare, container_type&& x);
		//
		// template <class InputIterator>
		// priority_queue(InputIterator first, InputIterator last, const compare_type& compare, container_type&& x);
		//
		// void push(value_type&& x);

		priority_queue<TestObject, vector<TestObject> > toPQ_0;
		priority_queue<TestObject, vector<TestObject> > toPQ_A(toPQ_0.get_container().begin(), toPQ_0.get_container().begin(), eastl::less<TestObject>(), toPQ_0.get_container());
		EATEST_VERIFY(toPQ_A.size() == 0);
		toPQ_A.push(TestObject(1000));
		EATEST_VERIFY(toPQ_A.size() == 1);

		priority_queue<TestObject, vector<TestObject> > toPQ_B(eastl::move(toPQ_A), toPQ_A.get_container().get_allocator()); // It would be better if we also tested an alternative allocator.
		EATEST_VERIFY((toPQ_B.size() == 1) && toPQ_A.empty());

		eastl::vector<TestObject> toVectorM(toVector);
		priority_queue<TestObject, vector<TestObject> > toPQ_C(eastl::less<TestObject>(), eastl::move(toVectorM));
		EATEST_VERIFY((toPQ_C.size() == toVector.size()) && toVectorM.empty());

		// template <class... Args>
		// void emplace(Args&&... args);
		priority_queue<TestObject, vector<TestObject> > toPQ_D;
		toPQ_D.emplace(0, 1, 2);
		EATEST_VERIFY(toPQ_D.size() == 1) && (toPQ_D.top() == TestObject(0, 1, 2));
	}


	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();


	return nErrorCount;
}






// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::stack<int, vector<int> >;
template class eastl::stack<Align64, list<Align64> >;
template class eastl::stack<TestObject, vector<TestObject> >;
//template class eastl::stack<IntNode, intrusive_list<IntNode> >; // This test has been disabled as of the addition of initializer_list support to eastl::stack. initializer_lists have const nodes, which is incompatible with intrusive_list. You can use eastl::stack<IntNode, intrusive_list<IntNode> > as long as you don't use initializer_list with it. The problem with this line of code is that it forces compilation of the entire class.


///////////////////////////////////////////////////////////////////////////////
// TestStack
//
static int TestStack()
{
	int nErrorCount = 0;

	TestObject::Reset();

	{
		// stack(const Sequence& x = Sequence());
		stack<TestObject, list<TestObject> > toListStack;
		stack<TestObject, list<TestObject> > toListStack2;


		// bool      empty() const;
		// size_type size() const;
		EATEST_VERIFY(toListStack.empty());
		EATEST_VERIFY(toListStack.size() == 0);


		// global operators
		EATEST_VERIFY( (toListStack == toListStack2));
		EATEST_VERIFY(!(toListStack != toListStack2));
		EATEST_VERIFY( (toListStack <= toListStack2));
		EATEST_VERIFY( (toListStack >= toListStack2));
		EATEST_VERIFY(!(toListStack <  toListStack2));
		EATEST_VERIFY(!(toListStack >  toListStack2));


		// void push(const value_type& value);
		// reference       top();
		// const_reference top() const;
		toListStack.push(TestObject(0));
		EATEST_VERIFY(toListStack.top() == TestObject(0));

		toListStack.push(TestObject(1));
		EATEST_VERIFY(toListStack.top() == TestObject(1));

		toListStack.push(TestObject(2));
		EATEST_VERIFY( toListStack.top() == TestObject(2));
		EATEST_VERIFY(!toListStack.empty());
		EATEST_VERIFY( toListStack.size() == 3);

		// void pop();
		toListStack.pop();
		EATEST_VERIFY(toListStack.top() == TestObject(1));

		toListStack.pop();
		EATEST_VERIFY(toListStack.top() == TestObject(0));

		toListStack.pop();
		EATEST_VERIFY(toListStack.empty());
		EATEST_VERIFY(toListStack.size() == 0);


		// container_type&       get_container();
		// const container_type& get_container() const;
		list<TestObject>& ref = toListStack.get_container();
		EATEST_VERIFY(ref.size() == toListStack.size());


		// stack(std::initializer_list<value_type> ilist);
		#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
			stack<int> intStack = { 3, 4, 5 };
			EATEST_VERIFY(intStack.size() == 3);
			EATEST_VERIFY(intStack.top() == 5);
			intStack.pop();
			EATEST_VERIFY(intStack.top() == 4);
			intStack.pop();
			EATEST_VERIFY(intStack.top() == 3);
		#endif
	}


	{
		vector<TestObject> toVector;
		for(int i = 0; i < 100; i++)
			toVector.push_back(TestObject(i));

		// template <class Allocator>
		// stack(this_type&& x, const Allocator& allocator, typename eastl::enable_if<eastl::uses_allocator<container_type, Allocator>::value>::type* = NULL);
		//
		// explicit stack(container_type&& x);
		//
		// void push(value_type&& x);
		stack<TestObject, vector<TestObject> > toS_0;
		stack<TestObject, vector<TestObject> > toS_A(eastl::move(toS_0), toS_0.get_container().get_allocator()); // It would be better if we also tested an alternative allocator.
		EATEST_VERIFY(toS_A.size() == 0);
		toS_A.push(TestObject(1000));
		EATEST_VERIFY(toS_A.size() == 1);

		stack<TestObject, vector<TestObject> > toS_B(eastl::move(toS_A), toS_A.get_container().get_allocator()); // It would be better if we also tested an alternative allocator.
		EATEST_VERIFY((toS_B.size() == 1) && toS_A.empty());

		eastl::vector<TestObject> toVectorM(toVector);
		stack<TestObject, vector<TestObject> > toS_C(eastl::move(toVectorM));
		EATEST_VERIFY((toS_C.size() == toVector.size()) && toVectorM.empty());

		{
			// template <class... Args>
			// void emplace_back(Args&&... args);
			stack<TestObject, vector<TestObject>> toS_D;
			toS_D.emplace_back(0, 1, 2);
			EATEST_VERIFY(toS_D.size() == 1) && (toS_D.top() == TestObject(0, 1, 2));
		}

		{
			// template <class... Args>
			// decltype(auto) emplace(Args&&... args);
			stack<TestObject, vector<TestObject>> toS_D;
			auto it = toS_D.emplace(0, 1, 2);
			EATEST_VERIFY(toS_D.size() == 1) && (toS_D.top() == TestObject(0, 1, 2));
			EATEST_VERIFY(it == TestObject(0, 1, 2));
		}
	}


	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();


	return nErrorCount;
}





struct Size0
{
	// Empty
};

struct Size4
{
	uint32_t m32;
};


///////////////////////////////////////////////////////////////////////////////
// TestCompressedPair
//
static int TestCompressedPair()
{
	int nErrorCount = 0;

	compressed_pair<Size0, Size0> cp00;
	compressed_pair<Size0, Size4> cp04;
	compressed_pair<Size4, Size0> cp40;
	compressed_pair<Size4, Size4> cp44;

	EATEST_VERIFY(sizeof(cp00) <= 4);
	EATEST_VERIFY(sizeof(cp04) <= 4);
	EATEST_VERIFY(sizeof(cp40) <= 4);
	EATEST_VERIFY(sizeof(cp44) <= 8);

	return nErrorCount;
}






template <typename T>
struct CallTraitsContainer
{
   typedef typename eastl::call_traits<T>::param_type       param_type;
   typedef typename eastl::call_traits<T>::reference        reference;
   typedef typename eastl::call_traits<T>::const_reference  const_reference;
   typedef typename eastl::call_traits<T>::value_type       result_type;
   typedef T                                                value_type;

public:
   value_type mValue;
   

   CallTraitsContainer() { }
   CallTraitsContainer(param_type p) : mValue(p) { }

   CallTraitsContainer<T>& operator=(const CallTraitsContainer<T>&) { } // Defined simply to prevent possible compiler warnings.

   result_type value() { return mValue; }

   reference       get()             { return mValue; }
   const_reference const_get() const { return mValue; }

   void call(param_type p){ }
};


///////////////////////////////////////////////////////////////////////////////
// TestCallTraits
//
static int TestCallTraits()
{
	int nErrorCount = 0;

	CallTraitsContainer<int>    ctcInt;
	CallTraitsContainer<int*>   ctcIntPtr;
	CallTraitsContainer<int&>   ctcVoid(nErrorCount);
	CallTraitsContainer<int[3]> ctcIntArray;

	char buffer[128];
	sprintf(buffer, "%p %p %p %p", &ctcInt, &ctcIntPtr, &ctcVoid, &ctcIntArray);

	return nErrorCount;
}


static int AccumulateMultiply(int x, int y)
{
	return (x * y);
}

static eastl::string AccumulateString(eastl::string s, int x)
{
	s += '0' + static_cast<char>(x);
	return s;
}
 


///////////////////////////////////////////////////////////////////////////////
// TestNumeric
//
static int TestNumeric()
{
	int nErrorCount = 0;

	//template <typename InputIterator, typename T>
	//T accumulate(InputIterator first, InputIterator last, T init);
	eastl::vector<int> v(5, 0);
	eastl::generate(v.begin(), v.end(), GenerateIncrementalIntegers<int>(1));

	int sum = eastl::accumulate(v.begin(), v.end(), 100);
	EATEST_VERIFY(sum == (100 + 1 + 2 + 3 + 4 + 5));


	// template <typename InputIterator, typename T, typename BinaryOperation>
	//T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op);

	eastl::generate(v.begin(), v.end(), GenerateIncrementalIntegers<int>(1));
	int product = eastl::accumulate(v.begin(), v.end(), 100, AccumulateMultiply);
	EATEST_VERIFY(product == (100 * 1 * 2 * 3 * 4 * 5));

	eastl::generate(v.begin(), v.end(), GenerateIncrementalIntegers<int>(1));
	eastl::string s = eastl::accumulate(v.begin(), v.end(), eastl::string("0"), AccumulateString);
	EATEST_VERIFY(s == "012345");


	//template <typename InputIterator1, typename InputIterator2, typename T>
	//T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init);
	// To do.

	//template <typename InputIterator1, typename InputIterator2, typename T, typename BinaryOperation1, typename BinaryOperation2>
	//T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init, BinaryOperation1 binary_op1, BinaryOperation2 binary_op2)
	// To do.

	//template <typename InputIterator, typename OutputIterator>
	//OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result);
	// To do.

	//template <typename InputIterator, typename OutputIterator, typename BinaryOperation>
	//OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result, BinaryOperation binary_op);
	// To do.

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestAdaptors
//
static int TestAdaptors()
{
	int nErrorCount = 0;

	// reverse lvalue container
	{
		int int_data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::vector<int> original(begin(int_data), end(int_data));

		eastl::vector<int> reversed;
		for(auto& e : eastl::reverse(original))
			reversed.push_back(e);

		eastl::reverse(begin(original), end(original));
		EATEST_VERIFY(reversed == original);
	}

	// reverse const lvalue container
	{
		int int_data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
		const eastl::vector<int> original(begin(int_data), end(int_data));

		eastl::vector<int> reversed;
		for(auto& e : eastl::reverse(original))
			reversed.push_back(e);

		eastl::vector<int> reversed_original(original);
		eastl::reverse(begin(reversed_original), end(reversed_original));
		EATEST_VERIFY(reversed == reversed_original);
	}

	// reverse rvalue container
	{
		int int_data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
		eastl::vector<int> original(begin(int_data), end(int_data));

		eastl::vector<int> reversed;
		for (auto& e : eastl::reverse(eastl::vector<int>(original)))
			reversed.push_back(e);

		eastl::reverse(begin(original), end(original));
		EATEST_VERIFY(reversed == original);
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestExtra
//
int TestExtra()
{
	int nErrorCount = 0;
	
	nErrorCount += TestForwardDeclarations();
	nErrorCount += TestQueue();
	nErrorCount += TestPriorityQueue();
	nErrorCount += TestStack();
	nErrorCount += TestCompressedPair();
	nErrorCount += TestCallTraits();
	nErrorCount += TestNumeric();
	nErrorCount += TestAdaptors();

	return nErrorCount;
}












