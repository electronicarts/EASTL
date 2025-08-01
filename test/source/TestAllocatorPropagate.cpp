// (c) 2023 Electronic Arts Inc.

#include "EASTLTest.h"
#include <EASTL/list.h>
#include <EASTL/slist.h>
#include <EASTL/vector.h>
#include <EASTL/deque.h>
#include <EASTL/map.h>
#include <EASTL/set.h>
#include <EASTL/string.h>
#include <EASTL/hash_map.h>
#include <EASTL/hash_set.h>
#include <EASTL/segmented_vector.h>
#include <EASTL/fixed_list.h>
#include <EASTL/fixed_slist.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/fixed_map.h>
#include <EASTL/fixed_set.h>
#include <EASTL/fixed_hash_map.h>
#include <EASTL/fixed_hash_set.h>
#include <EASTL/fixed_string.h>

template<typename Container, typename Elem>
int TestPropagateOnContainerCopyAssignment(std::initializer_list<Elem> ilist1, std::initializer_list<Elem> ilist2, const char* containerName)
{
	int nErrorCount = 0;

	{
		Container c1(ilist1, InstanceAllocator(1));
		Container c2(ilist2, InstanceAllocator(2));

		EATEST_VERIFY(get_allocator(c1).mInstanceId == 1);
		EATEST_VERIFY(get_allocator(c2).mInstanceId == 2);
		EATEST_VERIFY(get_allocator(c1) != get_allocator(c2));

		c1 = c2;
		EATEST_VERIFY(c1 == c2);

		// eastl containers have propagate_on_container_copy_assignment behaviour iff EASTL_ALLOCATOR_COPY_ENABLED.
		bool allocatorsEqual = (get_allocator(c1) == get_allocator(c2));
		EATEST_VERIFY(allocatorsEqual == (bool)EASTL_ALLOCATOR_COPY_ENABLED);

		// destroying containers to invoke InstanceAllocator::deallocate() checks
	}

	EATEST_VERIFY_MSG(InstanceAllocator::reset_all(),
	                  "Container elements should be deallocated by the allocator that allocated it.");

	if(nErrorCount > 0)
		EASTLTest_Printf("Propagate on %s copy assignment: failure\n", containerName);

	return nErrorCount;
}

template<typename Container, typename Elem>
int TestPropagateOnContainerMoveAssignment(std::initializer_list<Elem> ilist1, std::initializer_list<Elem> ilist2, const char* containerName)
{
	int nErrorCount = 0;

	{
		Container c1(ilist1, InstanceAllocator(1));
		Container c2(ilist2, InstanceAllocator(2));
		
		EATEST_VERIFY(get_allocator(c1).mInstanceId == 1);
		EATEST_VERIFY(get_allocator(c2).mInstanceId == 2);
		EATEST_VERIFY(get_allocator(c1) != get_allocator(c2));

		c1 = eastl::move(c2);

		// eastl containers have propagate_on_container_move_assignment behaviour.
		EATEST_VERIFY(get_allocator(c1).mInstanceId == 2);

		// destroying containers to invoke InstanceAllocator::deallocate() checks
	}

	EATEST_VERIFY_MSG(InstanceAllocator::reset_all(),
	                  "Container elements should be deallocated by the allocator that allocated it.");

	if (nErrorCount > 0)
		EASTLTest_Printf("Propagate on %s move assignment: failure\n", containerName);

	return nErrorCount;
}

template<typename Container, typename Elem>
int TestPropagateOnContainerSwap(std::initializer_list<Elem> ilist1, std::initializer_list<Elem> ilist2, const char* containerName)
{
	int nErrorCount = 0;

	{
		Container c1(ilist1, InstanceAllocator(1));
		Container c2(ilist2, InstanceAllocator(2));
		
		EATEST_VERIFY(get_allocator(c1).mInstanceId == 1);
		EATEST_VERIFY(get_allocator(c2).mInstanceId == 2);
		EATEST_VERIFY(get_allocator(c1) != get_allocator(c2));

		eastl::swap(c1, c2);

		// eastl containers have propagate_on_container_swap behaviour.
		EATEST_VERIFY(get_allocator(c1).mInstanceId == 2);
		EATEST_VERIFY(get_allocator(c2).mInstanceId == 1);
		EATEST_VERIFY(get_allocator(c1) != get_allocator(c2));

		// destroying containers to invoke InstanceAllocator::deallocate() checks
	}

	EATEST_VERIFY_MSG(InstanceAllocator::reset_all(),
	                  "Container elements should be deallocated by the allocator that allocated it.");

	if (nErrorCount > 0)
		EASTLTest_Printf("Propagate on %s swap: failure\n", containerName);

	return nErrorCount;
}

int TestAllocatorPropagate()
{
	using namespace eastl;
	int nErrorCount = 0;

	nErrorCount += TestPropagateOnContainerCopyAssignment<list<int, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "list");
	nErrorCount += TestPropagateOnContainerCopyAssignment<slist<int, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "slist");
	nErrorCount += TestPropagateOnContainerCopyAssignment<vector<int, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "vector");
	nErrorCount += TestPropagateOnContainerCopyAssignment<deque<int, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "deque");
	nErrorCount += TestPropagateOnContainerCopyAssignment<segmented_vector<int, 32, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "segmented_vector");
	nErrorCount += TestPropagateOnContainerCopyAssignment<map<int, int, less<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "map");
	nErrorCount += TestPropagateOnContainerCopyAssignment<multimap<int, int, less<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "multimap");
	nErrorCount += TestPropagateOnContainerCopyAssignment<set<int, less<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "set");
	nErrorCount += TestPropagateOnContainerCopyAssignment<multiset<int, less<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "multiset");
	nErrorCount += TestPropagateOnContainerCopyAssignment<hash_map<int, int, hash<int>, equal_to<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "hash_map");
	nErrorCount += TestPropagateOnContainerCopyAssignment<hash_multimap<int, int, hash<int>, equal_to<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "hash_multimap");
	nErrorCount += TestPropagateOnContainerCopyAssignment<hash_set<int, hash<int>, equal_to<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "hash_set");
	nErrorCount += TestPropagateOnContainerCopyAssignment<hash_multiset<int, hash<int>, equal_to<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "hash_multiset");
	nErrorCount += TestPropagateOnContainerCopyAssignment<basic_string<char, InstanceAllocator>>({ 'a', 'b', 'c' }, { 'd', 'e', 'f' }, "basic_string");
	nErrorCount += TestPropagateOnContainerCopyAssignment<fixed_list<int, 8, true, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_list");
	nErrorCount += TestPropagateOnContainerCopyAssignment<fixed_slist<int, 8, true, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_slist");
	nErrorCount += TestPropagateOnContainerCopyAssignment<fixed_vector<int, 8, true, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_vector");
	nErrorCount += TestPropagateOnContainerCopyAssignment<fixed_map<int, int, 8, true, less<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "fixed_map");
	nErrorCount += TestPropagateOnContainerCopyAssignment<fixed_multimap<int, int, 8, true, less<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "fixed_multimap");
	nErrorCount += TestPropagateOnContainerCopyAssignment<fixed_set<int, 8, true, less<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_set");
	nErrorCount += TestPropagateOnContainerCopyAssignment<fixed_multiset<int, 8, true, less<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_multiset");
	nErrorCount += TestPropagateOnContainerCopyAssignment<fixed_hash_map<int, int, 8, 9, true, hash<int>, equal_to<int>, true, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "fixed_hash_map");
	nErrorCount += TestPropagateOnContainerCopyAssignment<fixed_hash_multimap<int, int, 8, 9, true, hash<int>, equal_to<int>, true, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "fixed_hash_multimap");
	nErrorCount += TestPropagateOnContainerCopyAssignment<fixed_hash_set<int, 8, 9, true, hash<int>, equal_to<int>, true, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_hash_set");
	nErrorCount += TestPropagateOnContainerCopyAssignment<fixed_hash_multiset<int, 8, 9, true, hash<int>, equal_to<int>, true, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_hash_multiset");
	nErrorCount += TestPropagateOnContainerCopyAssignment<fixed_string<char, 8, true, InstanceAllocator>>({ 'a', 'b', 'c' }, { 'd', 'e', 'f' }, "fixed_string");

	// commenting out containers with irregular propagation behaviour:
	// nErrorCount += TestPropagateOnContainerMoveAssignment<list<int, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "list");
	// nErrorCount += TestPropagateOnContainerMoveAssignment<slist<int, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "slist");
	nErrorCount += TestPropagateOnContainerMoveAssignment<vector<int, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "vector");
	nErrorCount += TestPropagateOnContainerMoveAssignment<deque<int, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "deque");
	nErrorCount += TestPropagateOnContainerMoveAssignment<segmented_vector<int, 32, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "segmented_vector");
	nErrorCount += TestPropagateOnContainerMoveAssignment<map<int, int, less<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "map");
	nErrorCount += TestPropagateOnContainerMoveAssignment<multimap<int, int, less<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "multimap");
	nErrorCount += TestPropagateOnContainerMoveAssignment<set<int, less<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "set");
	nErrorCount += TestPropagateOnContainerMoveAssignment<multiset<int, less<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "multiset");
	nErrorCount += TestPropagateOnContainerMoveAssignment<hash_map<int, int, hash<int>, equal_to<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "hash_map");
	nErrorCount += TestPropagateOnContainerMoveAssignment<hash_multimap<int, int, hash<int>, equal_to<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "hash_multimap");
	nErrorCount += TestPropagateOnContainerMoveAssignment<hash_set<int, hash<int>, equal_to<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "hash_set");
	nErrorCount += TestPropagateOnContainerMoveAssignment<hash_multiset<int, hash<int>, equal_to<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "hash_multiset");
	// nErrorCount += TestPropagateOnContainerMoveAssignment<basic_string<char, InstanceAllocator>>({ 'a', 'b', 'c' }, { 'd', 'e', 'f' }, "basic_string");
	//nErrorCount += TestPropagateOnContainerMoveAssignment<fixed_list<int, 8, true, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_list");
	//nErrorCount += TestPropagateOnContainerMoveAssignment<fixed_slist<int, 8, true, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_slist");
	//nErrorCount += TestPropagateOnContainerMoveAssignment<fixed_vector<int, 8, true, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_vector");
	//nErrorCount += TestPropagateOnContainerMoveAssignment<fixed_map<int, int, 8, true, less<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "fixed_map");
	//nErrorCount += TestPropagateOnContainerMoveAssignment<fixed_multimap<int, int, 8, true, less<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "fixed_multimap");
	//nErrorCount += TestPropagateOnContainerMoveAssignment<fixed_set<int, 8, true, less<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_set");
	//nErrorCount += TestPropagateOnContainerMoveAssignment<fixed_multiset<int, 8, true, less<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_multiset");
	//nErrorCount += TestPropagateOnContainerMoveAssignment<fixed_hash_map<int, int, 8, 9, true, hash<int>, equal_to<int>, true, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "fixed_hash_map");
	//nErrorCount += TestPropagateOnContainerMoveAssignment<fixed_hash_multimap<int, int, 8, 9, true, hash<int>, equal_to<int>, true, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "fixed_hash_multimap");
	//nErrorCount += TestPropagateOnContainerMoveAssignment<fixed_hash_set<int, 8, 9, true, hash<int>, equal_to<int>, true, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_hash_set");
	//nErrorCount += TestPropagateOnContainerMoveAssignment<fixed_hash_multiset<int, 8, 9, true, hash<int>, equal_to<int>, true, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_hash_multiset");
	//nErrorCount += TestPropagateOnContainerMoveAssignment<fixed_string<char, 8, true, InstanceAllocator>>({ 'a', 'b', 'c' }, { 'd', 'e', 'f' }, "fixed_string");

	// nErrorCount += TestPropagateOnContainerSwap<list<int, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "list");
	// nErrorCount += TestPropagateOnContainerSwap<slist<int, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "slist");
	nErrorCount += TestPropagateOnContainerSwap<vector<int, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "vector");
	nErrorCount += TestPropagateOnContainerSwap<deque<int, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "deque");
	nErrorCount += TestPropagateOnContainerSwap<segmented_vector<int, 32, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "segmented_vector");
	nErrorCount += TestPropagateOnContainerSwap<map<int, int, less<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "map");
	nErrorCount += TestPropagateOnContainerSwap<multimap<int, int, less<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "multimap");
	nErrorCount += TestPropagateOnContainerSwap<set<int, less<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "set");
	nErrorCount += TestPropagateOnContainerSwap<multiset<int, less<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "multiset");
	nErrorCount += TestPropagateOnContainerSwap<hash_map<int, int, hash<int>, equal_to<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "hash_map");
	nErrorCount += TestPropagateOnContainerSwap<hash_multimap<int, int, hash<int>, equal_to<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "hash_multimap");
	nErrorCount += TestPropagateOnContainerSwap<hash_set<int, hash<int>, equal_to<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "hash_set");
	nErrorCount += TestPropagateOnContainerSwap<hash_multiset<int, hash<int>, equal_to<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "hash_multiset");
	// nErrorCount += TestPropagateOnContainerSwap<basic_string<char, InstanceAllocator>>({ 'a', 'b', 'c' }, { 'd', 'e', 'f' }, "basic_string");
	//nErrorCount += TestPropagateOnContainerSwap<fixed_list<int, 8, true, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_list");
	//nErrorCount += TestPropagateOnContainerSwap<fixed_slist<int, 8, true, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_slist");
	//nErrorCount += TestPropagateOnContainerSwap<fixed_vector<int, 8, true, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_vector");
	//nErrorCount += TestPropagateOnContainerSwap<fixed_map<int, int, 8, true, less<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "fixed_map");
	//nErrorCount += TestPropagateOnContainerSwap<fixed_multimap<int, int, 8, true, less<int>, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "fixed_multimap");
	//nErrorCount += TestPropagateOnContainerSwap<fixed_set<int, 8, true, less<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_set");
	//nErrorCount += TestPropagateOnContainerSwap<fixed_multiset<int, 8, true, less<int>, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_multiset");
	//nErrorCount += TestPropagateOnContainerSwap<fixed_hash_map<int, int, 8, 9, true, hash<int>, equal_to<int>, true, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "fixed_hash_map");
	//nErrorCount += TestPropagateOnContainerSwap<fixed_hash_multimap<int, int, 8, 9, true, hash<int>, equal_to<int>, true, InstanceAllocator>>({ pair<const int, int>(1, 1) }, { pair<const int, int>(2, 2) }, "fixed_hash_multimap");
	//nErrorCount += TestPropagateOnContainerSwap<fixed_hash_set<int, 8, 9, true, hash<int>, equal_to<int>, true, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_hash_set");
	//nErrorCount += TestPropagateOnContainerSwap<fixed_hash_multiset<int, 8, 9, true, hash<int>, equal_to<int>, true, InstanceAllocator>>({ 1, 2, 3 }, { 4, 5, 6 }, "fixed_hash_multiset");
	//nErrorCount += TestPropagateOnContainerSwap<fixed_string<char, 8, true, InstanceAllocator>>({ 'a', 'b', 'c' }, { 'd', 'e', 'f' }, "fixed_string");

	// container adaptors are not tested because their behaviour should be the same as their underlying container:
	// queue
	// priority_queue
	// stack
	// vector_map
	// vector_multimap
	// vector_set
	// vector_multiset
	// ring_buffer
	// lru_cache

	// other containers not tested:
	// string_hash_map - doesn't have appropriate constructor.
	// string_map - doesn't have appropriate constructor.
	// list_map - doesn't have appropriate constructor.
	// tuple_vector_alloc - what is this type?

	return nErrorCount;
}
