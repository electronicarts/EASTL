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
#include <EASTL/bit.h>
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

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	{
		// queue(const Sequence& x = Sequence());
		queue<TestObject, list<TestObject>> toListQueue;
		queue<TestObject, list<TestObject>> toListQueue2;


		// global operators
		EATEST_VERIFY( ((toListQueue <=> toListQueue2) == 0));
		EATEST_VERIFY(!((toListQueue <=> toListQueue2) != 0));
		EATEST_VERIFY( ((toListQueue <=> toListQueue2) <= 0));
		EATEST_VERIFY( ((toListQueue <=> toListQueue2) >= 0));
		EATEST_VERIFY(!((toListQueue <=> toListQueue2) < 0));
		EATEST_VERIFY(!((toListQueue <=> toListQueue2) > 0));

		// bool      empty() const;
		// size_type size() const;
		EATEST_VERIFY(toListQueue.empty());
		EATEST_VERIFY(toListQueue.size() == 0);

		// Verify toListQueue > toListQueue2
		toListQueue.push(TestObject(0));
		toListQueue.push(TestObject(1));
		toListQueue2.push(TestObject(0));

		EATEST_VERIFY(!((toListQueue <=> toListQueue2) == 0));
		EATEST_VERIFY( ((toListQueue <=> toListQueue2) != 0));
		EATEST_VERIFY( ((toListQueue <=> toListQueue2) >= 0));
		EATEST_VERIFY(!((toListQueue <=> toListQueue2) <= 0));
		EATEST_VERIFY( ((toListQueue <=> toListQueue2) > 0));
		EATEST_VERIFY(!((toListQueue <=> toListQueue2) < 0));

		// Verify toListQueue2 > toListQueue by element size
		toListQueue2.push(TestObject(3));
		EATEST_VERIFY(!((toListQueue <=> toListQueue2) == 0));
		EATEST_VERIFY( ((toListQueue <=> toListQueue2) != 0));
		EATEST_VERIFY( ((toListQueue <=> toListQueue2) <= 0));
		EATEST_VERIFY(!((toListQueue <=> toListQueue2) >= 0));
		EATEST_VERIFY( ((toListQueue <=> toListQueue2) < 0));
		EATEST_VERIFY(!((toListQueue <=> toListQueue2) > 0));

		queue<TestObject, list<TestObject>> toListQueue3;
		queue<TestObject, list<TestObject>> toListQueue4;

		for (int i = 0; i < 10; i++)
		{
			toListQueue3.push(TestObject(i));
			if (i < 5)
				toListQueue4.push(TestObject(i));
		}

		// Verify toListQueue4 is a strict subset of toListQueue3
		EATEST_VERIFY(!((toListQueue3 <=> toListQueue4) == 0));
		EATEST_VERIFY( ((toListQueue3 <=> toListQueue4) != 0));
		EATEST_VERIFY( ((toListQueue3 <=> toListQueue4) >= 0));
		EATEST_VERIFY(!((toListQueue3 <=> toListQueue4) <= 0));
		EATEST_VERIFY( ((toListQueue3 <=> toListQueue4) > 0));
		EATEST_VERIFY(!((toListQueue3 <=> toListQueue4) < 0));

		// Verify that even thoughn toListQueue4 has a smaller size, it's lexicographically larger
		toListQueue4.push(TestObject(11));
		EATEST_VERIFY(!((toListQueue3 <=> toListQueue4) == 0));
		EATEST_VERIFY( ((toListQueue3 <=> toListQueue4) != 0));
		EATEST_VERIFY( ((toListQueue3 <=> toListQueue4) <= 0));
		EATEST_VERIFY(!((toListQueue3 <=> toListQueue4) >= 0));
		EATEST_VERIFY( ((toListQueue3 <=> toListQueue4) < 0));
		EATEST_VERIFY(!((toListQueue3 <=> toListQueue4) > 0));
		
	}

	{
		queue<TestObject, list<TestObject>> toListQueue1;
		queue<TestObject, list<TestObject>> toListQueue2;
		queue<TestObject, list<TestObject>> toListQueue3;

		for (int i = 0; i < 10; i++)
		{
			toListQueue1.push(TestObject(i));
			toListQueue2.push(TestObject(9-i));
			if (i < 5)
				toListQueue3.push(TestObject(i));
		}

		struct weak_ordering_queue
		{
			queue<TestObject, list<TestObject>> queue;
		    inline std::weak_ordering operator<=>(const weak_ordering_queue& b) const { return queue <=> b.queue; }
		};

		EATEST_VERIFY(synth_three_way{}(weak_ordering_queue{toListQueue1}, weak_ordering_queue{toListQueue2}) == std::weak_ordering::less);
		EATEST_VERIFY(synth_three_way{}(weak_ordering_queue{toListQueue3}, weak_ordering_queue{toListQueue1}) == std::weak_ordering::less);
		EATEST_VERIFY(synth_three_way{}(weak_ordering_queue{toListQueue2}, weak_ordering_queue{toListQueue1}) == std::weak_ordering::greater);
		EATEST_VERIFY(synth_three_way{}(weak_ordering_queue{toListQueue2}, weak_ordering_queue{toListQueue3}) == std::weak_ordering::greater);
		EATEST_VERIFY(synth_three_way{}(weak_ordering_queue{toListQueue1}, weak_ordering_queue{toListQueue1}) == std::weak_ordering::equivalent);
	}
	#endif

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

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	{
		// stack(const Sequence& x = Sequence());
		stack<TestObject, list<TestObject> > toListStack;
		stack<TestObject, list<TestObject> > toListStack2;

		// bool      empty() const;
		// size_type size() const;
		EATEST_VERIFY(toListStack.empty());
		EATEST_VERIFY(toListStack.size() == 0);


		// global operators
		EATEST_VERIFY( ((toListStack <=> toListStack2) == 0));
		EATEST_VERIFY(!((toListStack <=> toListStack2) != 0));
		EATEST_VERIFY( ((toListStack <=> toListStack2) <= 0));
		EATEST_VERIFY( ((toListStack <=> toListStack2) >= 0));
		EATEST_VERIFY(!((toListStack <=> toListStack2) < 0));
		EATEST_VERIFY(!((toListStack <=> toListStack2) > 0));

		toListStack.push(TestObject(0));
		toListStack.push(TestObject(1));
		toListStack2.push(TestObject(0));

		EATEST_VERIFY(!((toListStack <=> toListStack2) == 0));
		EATEST_VERIFY( ((toListStack <=> toListStack2) != 0));
		EATEST_VERIFY( ((toListStack <=> toListStack2) >= 0));
		EATEST_VERIFY(!((toListStack <=> toListStack2) <= 0));
		EATEST_VERIFY( ((toListStack <=> toListStack2) > 0));
		EATEST_VERIFY(!((toListStack <=> toListStack2) < 0));

		// Verify toListStack2 > toListStack by element size
		toListStack2.push(TestObject(3));
		EATEST_VERIFY(!((toListStack <=> toListStack2) == 0));
		EATEST_VERIFY( ((toListStack <=> toListStack2) != 0));
		EATEST_VERIFY( ((toListStack <=> toListStack2) <= 0));
		EATEST_VERIFY(!((toListStack <=> toListStack2) >= 0));
		EATEST_VERIFY( ((toListStack <=> toListStack2) < 0));
		EATEST_VERIFY(!((toListStack <=> toListStack2) > 0));

		stack<TestObject, list<TestObject> > toListStack3;
		stack<TestObject, list<TestObject> > toListStack4;

		for (int i = 0; i < 10; i++)
		{
			toListStack3.push(TestObject(i));
			if (i < 5)
				toListStack4.push(TestObject(i));
		}

		// Verify toListStack4 is a strict subset of toListStack3
		EATEST_VERIFY(!((toListStack3 <=> toListStack4) == 0));
		EATEST_VERIFY( ((toListStack3 <=> toListStack4) != 0));
		EATEST_VERIFY( ((toListStack3 <=> toListStack4) >= 0));
		EATEST_VERIFY(!((toListStack3 <=> toListStack4) <= 0));
		EATEST_VERIFY( ((toListStack3 <=> toListStack4) > 0));
		EATEST_VERIFY(!((toListStack3 <=> toListStack4) < 0));

		// Verify that even thoughn toListQueue4 has a smaller size, it's lexicographically larger
		toListStack4.push(TestObject(11));
		EATEST_VERIFY(!((toListStack3 <=> toListStack4) == 0));
		EATEST_VERIFY( ((toListStack3 <=> toListStack4) != 0));
		EATEST_VERIFY( ((toListStack3 <=> toListStack4) <= 0));
		EATEST_VERIFY(!((toListStack3 <=> toListStack4) >= 0));
		EATEST_VERIFY( ((toListStack3 <=> toListStack4) < 0));
		EATEST_VERIFY(!((toListStack3 <=> toListStack4) > 0));
	}

	{
		stack<TestObject, list<TestObject> > toListStack1;
		stack<TestObject, list<TestObject> > toListStack2;
		stack<TestObject, list<TestObject> > toListStack3;

		for (int i = 0; i < 10; i++)
		{
			toListStack1.push(TestObject(i));
			toListStack2.push(TestObject(9-i));
			if (i < 5)
				toListStack3.push(TestObject(i));
		}

		struct weak_ordering_stack
		{
			stack<TestObject, list<TestObject> > stack;
		    inline std::weak_ordering operator<=>(const weak_ordering_stack& b) const { return stack <=> b.stack; }
		};

		EATEST_VERIFY(synth_three_way{}(weak_ordering_stack{toListStack1}, weak_ordering_stack{toListStack2}) == std::weak_ordering::less);
		EATEST_VERIFY(synth_three_way{}(weak_ordering_stack{toListStack3}, weak_ordering_stack{toListStack1}) == std::weak_ordering::less);
		EATEST_VERIFY(synth_three_way{}(weak_ordering_stack{toListStack2}, weak_ordering_stack{toListStack1}) == std::weak_ordering::greater);
		EATEST_VERIFY(synth_three_way{}(weak_ordering_stack{toListStack2}, weak_ordering_stack{toListStack3}) == std::weak_ordering::greater);
		EATEST_VERIFY(synth_three_way{}(weak_ordering_stack{toListStack1}, weak_ordering_stack{toListStack1}) == std::weak_ordering::equivalent);
	}
#endif


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

#if defined(EA_COMPILER_CPP20_ENABLED)
template <typename T>
static constexpr int SignedIntMidpoint()
{
	int nErrorCount = 0;

	EATEST_VERIFY(eastl::midpoint(T(0), T(0)) == T(0));
	EATEST_VERIFY(eastl::midpoint(T(0), T(2)) == T(1));
	EATEST_VERIFY(eastl::midpoint(T(0), T(4)) == T(2));
	EATEST_VERIFY(eastl::midpoint(T(0), T(8)) == T(4));
	EATEST_VERIFY(eastl::midpoint(T(2), T(0)) == T(1));
	EATEST_VERIFY(eastl::midpoint(T(4), T(0)) == T(2));
	EATEST_VERIFY(eastl::midpoint(T(8), T(0)) == T(4));

	EATEST_VERIFY(eastl::midpoint(T(1), T(1)) == T(1));
	EATEST_VERIFY(eastl::midpoint(T(1), T(3)) == T(2));
	EATEST_VERIFY(eastl::midpoint(T(3), T(1)) == T(2));
	EATEST_VERIFY(eastl::midpoint(T(2), T(6)) == T(4));
	EATEST_VERIFY(eastl::midpoint(T(6), T(2)) == T(4));

	EATEST_VERIFY(eastl::midpoint(T(-1), T(-1)) == T(-1));
	EATEST_VERIFY(eastl::midpoint(T(-1), T(-3)) == T(-2));
	EATEST_VERIFY(eastl::midpoint(T(-3), T(-1)) == T(-2));
	EATEST_VERIFY(eastl::midpoint(T(-2), T(-6)) == T(-4));
	EATEST_VERIFY(eastl::midpoint(T(-6), T(-2)) == T(-4));

	EATEST_VERIFY(eastl::midpoint(T(-0), T(0)) == T(0));
	EATEST_VERIFY(eastl::midpoint(T(0), T(-0)) == T(0));
	EATEST_VERIFY(eastl::midpoint(T(-0), T(-0)) == T(0));
	EATEST_VERIFY(eastl::midpoint(T(-1), T(1)) == T(0));
	EATEST_VERIFY(eastl::midpoint(T(-10), T(10)) == T(0));
	EATEST_VERIFY(eastl::midpoint(T(-3), T(7)) == T(2));
	EATEST_VERIFY(eastl::midpoint(T(-7), T(3)) == T(-2));
	EATEST_VERIFY(eastl::midpoint(T(-2), T(6)) == T(2));
	EATEST_VERIFY(eastl::midpoint(T(-6), T(2)) == T(-2));
	EATEST_VERIFY(eastl::midpoint(T(2), T(-6)) == T(-2));
	EATEST_VERIFY(eastl::midpoint(T(6), T(-2)) == T(2));

	// If an odd sum, midpoint should round towards the LHS operand.
	EATEST_VERIFY(eastl::midpoint(T(0), T(5)) == T(2));
	EATEST_VERIFY(eastl::midpoint(T(5), T(0)) == T(3));
	EATEST_VERIFY(eastl::midpoint(T(1), T(4)) == T(2));
	EATEST_VERIFY(eastl::midpoint(T(4), T(1)) == T(3));
	EATEST_VERIFY(eastl::midpoint(T(7), T(10)) == T(8));
	EATEST_VERIFY(eastl::midpoint(T(10), T(7)) == T(9));
	EATEST_VERIFY(eastl::midpoint(T(-1), T(2)) == T(0));
	EATEST_VERIFY(eastl::midpoint(T(2), T(-1)) == T(1));
	EATEST_VERIFY(eastl::midpoint(T(-5), T(4)) == T(-1));
	EATEST_VERIFY(eastl::midpoint(T(4), T(-5)) == T(0));

	// Test absolute limits
	constexpr T MIN = eastl::numeric_limits<T>::min();
	constexpr T MAX = eastl::numeric_limits<T>::max();

	EATEST_VERIFY(eastl::midpoint(MIN, MIN) == MIN);
	EATEST_VERIFY(eastl::midpoint(MAX, MAX) == MAX);
	EATEST_VERIFY(eastl::midpoint(MIN, MAX) == T(-1));
	EATEST_VERIFY(eastl::midpoint(MAX, MIN) == T(0));
	EATEST_VERIFY(eastl::midpoint(MIN, T(0)) == MIN / 2);
	EATEST_VERIFY(eastl::midpoint(T(0), MIN) == MIN / 2);
	EATEST_VERIFY(eastl::midpoint(MAX, T(0)) == (MAX / 2) + 1);
	EATEST_VERIFY(eastl::midpoint(T(0), MAX) == (MAX / 2));

	EATEST_VERIFY(eastl::midpoint(MIN, T(10)) == (MIN / 2) + 5);
	EATEST_VERIFY(eastl::midpoint(T(10), MIN) == (MIN / 2) + 5);
	EATEST_VERIFY(eastl::midpoint(MAX, T(10)) == (MAX / 2) + 5 + 1);
	EATEST_VERIFY(eastl::midpoint(T(10), MAX) == (MAX / 2) + 5);
	EATEST_VERIFY(eastl::midpoint(MIN, T(-10)) == (MIN / 2) - 5);
	EATEST_VERIFY(eastl::midpoint(T(-10), MIN) == (MIN / 2) - 5);
	EATEST_VERIFY(eastl::midpoint(MAX, T(-10)) == (MAX / 2) - 5 + 1);
	EATEST_VERIFY(eastl::midpoint(T(-10), MAX) == (MAX / 2) - 5);

	return nErrorCount;
}

template <typename T>
static constexpr int UnsignedIntMidpoint()
{
	int nErrorCount = 0;

	EATEST_VERIFY(eastl::midpoint(T(0), T(0)) == T(0));
	EATEST_VERIFY(eastl::midpoint(T(0), T(2)) == T(1));
	EATEST_VERIFY(eastl::midpoint(T(0), T(4)) == T(2));
	EATEST_VERIFY(eastl::midpoint(T(0), T(8)) == T(4));
	EATEST_VERIFY(eastl::midpoint(T(2), T(0)) == T(1));
	EATEST_VERIFY(eastl::midpoint(T(4), T(0)) == T(2));
	EATEST_VERIFY(eastl::midpoint(T(8), T(0)) == T(4));

	EATEST_VERIFY(eastl::midpoint(T(1), T(1)) == T(1));
	EATEST_VERIFY(eastl::midpoint(T(1), T(3)) == T(2));
	EATEST_VERIFY(eastl::midpoint(T(3), T(1)) == T(2));
	EATEST_VERIFY(eastl::midpoint(T(2), T(6)) == T(4));
	EATEST_VERIFY(eastl::midpoint(T(6), T(2)) == T(4));

	// If an odd sum, midpoint should round towards the LHS operand.
	EATEST_VERIFY(eastl::midpoint(T(0), T(5)) == T(2));
	EATEST_VERIFY(eastl::midpoint(T(5), T(0)) == T(3));
	EATEST_VERIFY(eastl::midpoint(T(1), T(4)) == T(2));
	EATEST_VERIFY(eastl::midpoint(T(4), T(1)) == T(3));
	EATEST_VERIFY(eastl::midpoint(T(7), T(10)) == T(8));
	EATEST_VERIFY(eastl::midpoint(T(10), T(7)) == T(9));

	// Test absolute limits
	constexpr T MIN = eastl::numeric_limits<T>::min();
	constexpr T MAX = eastl::numeric_limits<T>::max();

	EATEST_VERIFY(eastl::midpoint(MIN, MIN) == MIN);
	EATEST_VERIFY(eastl::midpoint(MAX, MAX) == MAX);
	EATEST_VERIFY(eastl::midpoint(MIN, MAX) == MAX / 2);
	EATEST_VERIFY(eastl::midpoint(MAX, MIN) == (MAX / 2) + 1);
	EATEST_VERIFY(eastl::midpoint(MIN, T(0)) == T(0));
	EATEST_VERIFY(eastl::midpoint(T(0), MIN) == T(0));

	EATEST_VERIFY(eastl::midpoint(MIN, T(10)) == (MIN / 2) + 5);
	EATEST_VERIFY(eastl::midpoint(T(10), MIN) == (MIN / 2) + 5);
	EATEST_VERIFY(eastl::midpoint(MAX, T(10)) == (MAX / 2) + 5 + 1);
	EATEST_VERIFY(eastl::midpoint(T(10), MAX) == (MAX / 2) + 5);

	return nErrorCount;
}

template <typename T>
static constexpr int FloatMidpoint()
{
	// for use with floats, double, long doubles.
	int nErrorCount = 0;
	EATEST_VERIFY(eastl::midpoint(T(0.0), T(0.0)) == T(0.0));
	EATEST_VERIFY(eastl::midpoint(T(0.0), T(2.0)) == T(1.0));
	EATEST_VERIFY(eastl::midpoint(T(0.0), T(4.0)) == T(2.0));
	EATEST_VERIFY(eastl::midpoint(T(2.0), T(0.0)) == T(1.0));
	EATEST_VERIFY(eastl::midpoint(T(4.0), T(0.0)) == T(2.0));

	EATEST_VERIFY(eastl::midpoint(T(0.5), T(0.5)) == T(0.5));
	EATEST_VERIFY(eastl::midpoint(T(0.0), T(0.5)) == T(0.25));
	EATEST_VERIFY(eastl::midpoint(T(0.5), T(0.0)) == T(0.25));
	EATEST_VERIFY(eastl::midpoint(T(0.5), T(1.0)) == T(0.75));
	EATEST_VERIFY(eastl::midpoint(T(1.0), T(0.5)) == T(0.75));

	EATEST_VERIFY(eastl::midpoint(T(-0.0), T(0.0)) == T(0.0));
	EATEST_VERIFY(eastl::midpoint(T(0.0), T(-0.0)) == T(0.0));
	EATEST_VERIFY(eastl::midpoint(T(-0.0), T(-0.0)) == T(0.0));
	EATEST_VERIFY(eastl::midpoint(T(-1.0), T(2.0)) == T(0.5));
	EATEST_VERIFY(eastl::midpoint(T(-2.0), T(1)) == T(-0.5));
	EATEST_VERIFY(eastl::midpoint(T(-3.0), T(6.0)) == T(1.5));
	EATEST_VERIFY(eastl::midpoint(T(-6.0), T(3.0)) == T(-1.5));

	// Test absolute limits
	const T MIN = eastl::numeric_limits<T>::min();
	const T MAX = eastl::numeric_limits<T>::max();

	EATEST_VERIFY(eastl::midpoint(MIN, MIN) == MIN);
	EATEST_VERIFY(eastl::midpoint(MAX, MAX) == MAX);
	EATEST_VERIFY(eastl::midpoint(MIN, MAX) == MAX / 2);
	EATEST_VERIFY(eastl::midpoint(MAX, MIN) == MAX / 2);
	EATEST_VERIFY(eastl::midpoint(-MAX, MIN) == -MAX / 2);

	EATEST_VERIFY(eastl::midpoint(MIN, T(9.0)) == T(4.5));
	EATEST_VERIFY(eastl::midpoint(MIN, T(-9.0)) == T(-4.5));
	EATEST_VERIFY(eastl::midpoint(T(9.0), MIN) == T(4.5));
	EATEST_VERIFY(eastl::midpoint(T(-9.0), MIN) == T(-4.5));
	EATEST_VERIFY(eastl::midpoint(MAX, T(9.0)) == MAX / 2 + T(4.5));
	EATEST_VERIFY(eastl::midpoint(MAX, T(-9.0)) == MAX / 2 - T(4.5));
	EATEST_VERIFY(eastl::midpoint(T(9.0), MAX) == MAX / 2 + T(4.5));
	EATEST_VERIFY(eastl::midpoint(T(-9.0), MAX) == MAX / 2 - T(4.5));

	return nErrorCount;
}

template <typename T>
static constexpr int PointerMidpoint()
{
	int nErrorCount = 0;

	const T ARR[100] = {};

	EATEST_VERIFY(eastl::midpoint(ARR, ARR) == ARR);
	EATEST_VERIFY(eastl::midpoint(ARR, ARR + 100) == ARR + 50);
	EATEST_VERIFY(eastl::midpoint(ARR + 100, ARR) == ARR + 50);
	EATEST_VERIFY(eastl::midpoint(ARR, ARR + 25) == ARR + 12);
	EATEST_VERIFY(eastl::midpoint(ARR + 25, ARR) == ARR + 13);
	EATEST_VERIFY(eastl::midpoint(ARR, ARR + 13) == ARR + 6);
	EATEST_VERIFY(eastl::midpoint(ARR + 13, ARR) == ARR + 7);
	EATEST_VERIFY(eastl::midpoint(ARR + 50, ARR + 100) == ARR + 75);
	EATEST_VERIFY(eastl::midpoint(ARR + 100, ARR + 50) == ARR + 75);

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestMidpoint
//
static int TestMidpoint()
{
	int nErrorCount = 0;

	// template <typename T>
	// constexpr eastl::enable_if_t<eastl::is_arithmetic_v<T> && !eastl::is_same_v<eastl::remove_cv_t<T>, bool>, T> 
	// midpoint(const T lhs, const T rhs) EA_NOEXCEPT
	nErrorCount += SignedIntMidpoint<int>();
	nErrorCount += SignedIntMidpoint<char>();
	nErrorCount += SignedIntMidpoint<short>();
	nErrorCount += SignedIntMidpoint<long>();
	nErrorCount += SignedIntMidpoint<long long>();

	nErrorCount += UnsignedIntMidpoint<unsigned int>();
	nErrorCount += UnsignedIntMidpoint<unsigned char>();
	nErrorCount += UnsignedIntMidpoint<unsigned short>();
	nErrorCount += UnsignedIntMidpoint<unsigned long>();
	nErrorCount += UnsignedIntMidpoint<unsigned long long>();

	nErrorCount += FloatMidpoint<float>();
	nErrorCount += FloatMidpoint<double>();
	nErrorCount += FloatMidpoint<long double>();

	// template <typename T>
	// constexpr eastl::enable_if_t<eastl::is_object_v<T>, const T*> midpoint(const T* lhs, const T* rhs)
	nErrorCount += PointerMidpoint<int>();
	nErrorCount += PointerMidpoint<char>();
	nErrorCount += PointerMidpoint<short>();
	nErrorCount += PointerMidpoint<float>();
	nErrorCount += PointerMidpoint<double>();
	nErrorCount += PointerMidpoint<long double>();

	return nErrorCount;
}


template <typename T>
static constexpr int FloatLerp()
{
	int nErrorCount = 0;

	EATEST_VERIFY(eastl::lerp(T(0.0), T(0.0), T(0.0)) == T(0.0));
	EATEST_VERIFY(eastl::lerp(T(1.0), T(0.0), T(0.0)) == T(1.0));
	EATEST_VERIFY(eastl::lerp(T(-1.0), T(0.0), T(0.0)) == T(-1.0));
	EATEST_VERIFY(eastl::lerp(T(0.0), T(1.0), T(0.0)) == T(0.0));
	EATEST_VERIFY(eastl::lerp(T(0.0), T(-1.0), T(0.0)) == T(0.0));
	EATEST_VERIFY(eastl::lerp(T(-1.0), T(1.0), T(1.0)) == T(1.0));
	EATEST_VERIFY(eastl::lerp(T(1.0), T(-1.0), T(1.0)) == T(-1.0));
	EATEST_VERIFY(eastl::lerp(T(-1.0), T(1.0), T(0.5)) == T(0.0));
	EATEST_VERIFY(eastl::lerp(T(1.0), T(-1.0), T(0.5)) == T(0.0));
	EATEST_VERIFY(eastl::lerp(T(5.0), T(5.0), T(0.5)) == T(5.0));
	EATEST_VERIFY(eastl::lerp(T(-5.0), T(-5.0), T(0.5)) == T(-5.0));
	EATEST_VERIFY(eastl::lerp(T(1.0), T(2.0), T(1.0)) == T(2.0));
	EATEST_VERIFY(eastl::lerp(T(2.0), T(1.0), T(1.0)) == T(1.0));
	EATEST_VERIFY(eastl::lerp(T(1.0), T(2.0), T(1.0)) == T(2.0));
	EATEST_VERIFY(eastl::lerp(T(1.0), T(2.0), T(2.0)) == T(3.0));
	EATEST_VERIFY(eastl::lerp(T(2.0), T(1.0), T(2.0)) == T(0.0));
	EATEST_VERIFY(eastl::lerp(T(1.0), T(-2.0), T(2.0)) == T(-5.0));
	EATEST_VERIFY(eastl::lerp(T(-1.0), T(2.0), T(2.0)) == T(5.0));
	EATEST_VERIFY(eastl::lerp(T(-1.5), T(1.5), T(0.75)) == T(0.75));
	EATEST_VERIFY(eastl::lerp(T(0.125), T(1.75), T(0.25)) == T(0.53125));
	EATEST_VERIFY(eastl::lerp(T(-0.125), T(-1.75), T(0.5)) == T(-0.9375));
	EATEST_VERIFY(eastl::lerp(T(-0.125), T(1.5), T(2.5)) == T(3.9375));

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestLerp
//
static int TestLerp()
{
	int nErrorCount = 0;

	// template <class T>
	// constexpr T lerp(const T a, const T b, const T t) EA_NOEXCEPT
	nErrorCount += FloatLerp<float>();
	nErrorCount += FloatLerp<double>();
	nErrorCount += FloatLerp<long double>();

	return nErrorCount;
}
#endif


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

#if defined(EA_COMPILER_CPP20_ENABLED)
template <typename T>
int TestHasSingleBit()
{
	int nErrorCount = 0;

	VERIFY(eastl::has_single_bit(T(0)) == false);
	VERIFY(eastl::has_single_bit(T(1)) == true);
	VERIFY(eastl::has_single_bit(T(2)) == true);
	VERIFY(eastl::has_single_bit(T(3)) == false);

	VERIFY(eastl::has_single_bit(eastl::numeric_limits<T>::min()) == false);
	VERIFY(eastl::has_single_bit(eastl::numeric_limits<T>::max()) == false);

	for (int i = 4; i < eastl::numeric_limits<T>::digits; i++)
	{
		T power_of_two = static_cast<T>(T(1U) << i);
		VERIFY(eastl::has_single_bit(power_of_two));
		VERIFY(eastl::has_single_bit(static_cast<T>(power_of_two - 1)) == false);
	}

	return nErrorCount;
}

template <typename T>
static int TestBitCeil()
{
	int nErrorCount = 0;

	VERIFY(eastl::bit_ceil(T(0)) == T(1));
	VERIFY(eastl::bit_ceil(T(1)) == T(1));
	VERIFY(eastl::bit_ceil(T(2)) == T(2));
	VERIFY(eastl::bit_ceil(T(3)) == T(4));

	EA_CONSTEXPR auto DIGITS = eastl::numeric_limits<T>::digits;
	EA_CONSTEXPR auto MIN = eastl::numeric_limits<T>::min();
	EA_CONSTEXPR auto MAX = static_cast<T>(T(1) << (DIGITS - 1));

	VERIFY(eastl::bit_ceil(MAX) == MAX);
	VERIFY(eastl::bit_ceil(static_cast<T>(MAX - 1)) == MAX);
	VERIFY(eastl::bit_ceil(MIN) == T(1));

	for (int i = 4; i < eastl::numeric_limits<T>::digits; i++)
	{
		T power_of_two = static_cast<T>(T(1U) << i);
		VERIFY(eastl::bit_ceil(power_of_two) == power_of_two);
		VERIFY(eastl::bit_ceil(static_cast<T>(power_of_two - 1)) == power_of_two);
	}

	return nErrorCount;
}

template <typename T>
static int TestBitFloor()
{
	int nErrorCount = 0;
	VERIFY(eastl::bit_floor(T(0)) == T(0));
	VERIFY(eastl::bit_floor(T(1)) == T(1));
	VERIFY(eastl::bit_floor(T(2)) == T(2));
	VERIFY(eastl::bit_floor(T(3)) == T(2));

	EA_CONSTEXPR auto DIGITS = eastl::numeric_limits<T>::digits;
	EA_CONSTEXPR auto MIN = eastl::numeric_limits<T>::min();
	EA_CONSTEXPR auto MAX = eastl::numeric_limits<T>::max();

	VERIFY(eastl::bit_floor(MAX) == T(1) << (DIGITS - 1));
	VERIFY(eastl::bit_floor(MIN) == T(0));

	for (int i = 4; i < eastl::numeric_limits<T>::digits; i++)
	{
		T power_of_two = static_cast<T>(T(1U) << i);
		VERIFY(eastl::bit_floor(power_of_two) == power_of_two);
		VERIFY(eastl::bit_floor(static_cast<T>(power_of_two + 1)) == power_of_two);
	}
	return nErrorCount;
}

template <typename T>
static int TestBitWidth()
{
	int nErrorCount = 0;

	VERIFY(eastl::bit_width(T(0)) == T(0));
	VERIFY(eastl::bit_width(T(1)) == T(1));
	VERIFY(eastl::bit_width(T(2)) == T(2));
	VERIFY(eastl::bit_width(T(3)) == T(2));

	EA_CONSTEXPR auto DIGITS = eastl::numeric_limits<T>::digits;
	EA_CONSTEXPR auto MIN = eastl::numeric_limits<T>::min();
	EA_CONSTEXPR auto MAX = eastl::numeric_limits<T>::max();

	VERIFY(eastl::bit_width(MIN) == 0);
	VERIFY(eastl::bit_width(MAX) == DIGITS);

	for (int i = 4; i < eastl::numeric_limits<T>::digits; i++)
	{
		T power_of_two = static_cast<T>(T(1U) << i);
		VERIFY(eastl::bit_width(power_of_two) == static_cast<T>(i + 1));
	}

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestPowerofTwo
//
static int TestPowerOfTwo()
{
	int nErrorCount = 0;
	nErrorCount += TestHasSingleBit<unsigned int>();
	nErrorCount += TestHasSingleBit<unsigned char>();
	nErrorCount += TestHasSingleBit<unsigned short>();
	nErrorCount += TestHasSingleBit<unsigned long>();
	nErrorCount += TestHasSingleBit<unsigned long long>();

	nErrorCount += TestBitCeil<unsigned int>();
	nErrorCount += TestBitCeil<unsigned char>();
	nErrorCount += TestBitCeil<unsigned short>();
	nErrorCount += TestBitCeil<unsigned long>();
	nErrorCount += TestBitCeil<unsigned long long>();

	nErrorCount += TestBitFloor<unsigned int>();
	nErrorCount += TestBitFloor<unsigned char>();
	nErrorCount += TestBitFloor<unsigned short>();
	nErrorCount += TestBitFloor<unsigned long>();
	nErrorCount += TestBitFloor<unsigned long long>();

	nErrorCount += TestBitWidth<unsigned int>();
	nErrorCount += TestBitWidth<unsigned char>();
	nErrorCount += TestBitWidth<unsigned short>();
	nErrorCount += TestBitWidth<unsigned long>();
	nErrorCount += TestBitWidth<unsigned long long>();

	return nErrorCount;
}
#endif

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
#if defined(EA_COMPILER_CPP20_ENABLED)
	nErrorCount += TestMidpoint();
	nErrorCount += TestLerp();
	nErrorCount += TestPowerOfTwo();
#endif

	return nErrorCount;
}












