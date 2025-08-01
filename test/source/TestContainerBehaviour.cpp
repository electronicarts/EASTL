// (c) 2025 Electronic Arts Inc.

#include "EASTLTest.h"
#include <EASTL/list.h>
#include <EASTL/bonus/list_map.h>
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

template <typename Container, typename T>
int TestContainerCtorMoveWithAllocator(bool useSameAllocator,
                                       std::initializer_list<T> ilist,
                                       const char* containerName,
                                       bool expectElementsMoved,
                                       bool expectElementsCopied)
{
	int nErrorCount = 0;

	{
		const InstanceAllocator moveAllocator = InstanceAllocator(useSameAllocator ? 1 : 2);
		const Container expected(ilist, InstanceAllocator(1));
		Container src(ilist, InstanceAllocator(1));

		int64_t toCopyCtorCount = TestObject::sTOCopyCtorCount, toMoveCtorCount = TestObject::sTOMoveCtorCount,
		        toCtorCount = TestObject::sTOCtorCount, toAssignCount = TestObject::sTOAssignCount;

		Container dest(eastl::move(src), moveAllocator);

		// required by the standard:
		EATEST_VERIFY(get_allocator(dest) == moveAllocator);
		EATEST_VERIFY(dest.size() == expected.size());
		EATEST_VERIFY(eastl::equal(dest.begin(), dest.end(), expected.begin()));

		// strengthened compared to the standard:
		if (expectElementsMoved)
			EATEST_VERIFY(TestObject::sTOMoveCtorCount - toMoveCtorCount == static_cast<int64_t>(ilist.size()));
		if (expectElementsCopied)
			EATEST_VERIFY(TestObject::sTOCopyCtorCount - toCopyCtorCount == static_cast<int64_t>(ilist.size()));
		EATEST_VERIFY(TestObject::sTOCtorCount - toCtorCount ==
		              ((expectElementsMoved || expectElementsCopied) ? static_cast<int64_t>(ilist.size()) : 0));
		EATEST_VERIFY(TestObject::sTOAssignCount == toAssignCount);
	}

	EATEST_VERIFY_MSG(InstanceAllocator::reset_all(),
	                  "Container elements should be deallocated by the allocator that allocated it.");

	if (nErrorCount > 0)
		EASTLTest_Printf("%s move constructor with allocator: failure\n", containerName);

	return nErrorCount;
}

int TestContainerBehaviour()
{
	using namespace eastl;
	int nErrorCount = 0;

	EATEST_VERIFY(TestObject::Reset());

	{ // move constructor with allocator
		// only correct because it has incorrect propagate on swap behaviour, see TestAllocatorPropagate
		nErrorCount += TestContainerCtorMoveWithAllocator<list<TestObject, InstanceAllocator>>(false, { TestObject{1}, TestObject{2}, TestObject{3} }, "list", false, true);
		EATEST_VERIFY(TestObject::Reset());
		// only correct because it has incorrect propagate on swap behaviour, see TestAllocatorPropagate
		nErrorCount += TestContainerCtorMoveWithAllocator<slist<TestObject, InstanceAllocator>>(false, { TestObject{1}, TestObject{2}, TestObject{3} }, "slist", false, true);
		EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<vector<TestObject, InstanceAllocator>>(false, { TestObject{1}, TestObject{2}, TestObject{3} }, "vector", true, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<deque<TestObject, InstanceAllocator>>(false, { TestObject{1}, TestObject{2}, TestObject{3} }, "deque", true, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<map<TestObject, int, less<TestObject>, InstanceAllocator>>(false, { pair<const TestObject, int>(1, 1) }, "map", false, true);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<multimap<TestObject, int, less<TestObject>, InstanceAllocator>>(false, { pair<const TestObject, int>(1, 1) }, "multimap", false, true);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<set<TestObject, less<TestObject>, InstanceAllocator>>(false, { TestObject{1}, TestObject{2}, TestObject{3} }, "set", true, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<multiset<TestObject, less<TestObject>, InstanceAllocator>>(false, { TestObject{1}, TestObject{2}, TestObject{3} }, "multiset", true, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<hash_map<TestObject, int, hash<TestObject>, equal_to<TestObject>, InstanceAllocator>>(false, { pair<const TestObject, int>(1, 1) }, "hash_map", false, true);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<hash_multimap<TestObject, int, hash<TestObject>, equal_to<TestObject>, InstanceAllocator>>(false, { pair<const TestObject, int>(1, 1) }, "hash_multimap", false, true);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<hash_set<TestObject, hash<TestObject>, equal_to<TestObject>, InstanceAllocator>>(false, { TestObject{1}, TestObject{2}, TestObject{3} }, "hash_set", true, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<hash_multiset<TestObject, hash<TestObject>, equal_to<TestObject>, InstanceAllocator>>(false, { TestObject{1}, TestObject{2}, TestObject{3} }, "hash_multiset", true, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<basic_string<char, InstanceAllocator>>(false, { 'a', 'b', 'c' }, "basic_string", false, false);

		// fixed types with overflow, but all elements fit in the fixed portion of the container:
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_list<TestObject, 8, true, InstanceAllocator>>(false, { TestObject{1}, TestObject{2}, TestObject{3} }, "fixed_list (no overflow)", true, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_slist<TestObject, 8, true, InstanceAllocator>>(false, { TestObject{1}, TestObject{2}, TestObject{3} }, "fixed_slist (no overflow)", true, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_vector<TestObject, 8, true, InstanceAllocator>>(false, { TestObject{1}, TestObject{2}, TestObject{3} }, "fixed_vector (no overflow)", true, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_map<TestObject, int, 8, true, less<TestObject>, InstanceAllocator>>(false, { pair<const TestObject, int>(1, 1) }, "fixed_map (no overflow)", false, true);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_multimap<TestObject, int, 8, true, less<TestObject>, InstanceAllocator>>(false, { pair<const TestObject, int>(1, 1) }, "fixed_multimap (no overflow)", false, true);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_set<TestObject, 8, true, less<TestObject>, InstanceAllocator>>(false, { TestObject{1}, TestObject{2}, TestObject{3} }, "fixed_set (no overflow)", true, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_multiset<TestObject, 8, true, less<TestObject>, InstanceAllocator>>(false, { TestObject{1}, TestObject{2}, TestObject{3} }, "fixed_multiset (no overflow)", true, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_hash_map<TestObject, int, 8, 9, true, hash<TestObject>, equal_to<TestObject>, true, InstanceAllocator>>(false, { pair<const TestObject, int>(1, 1) }, "fixed_hash_map (no overflow)", false, true);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_hash_multimap<TestObject, int, 8, 9, true, hash<TestObject>, equal_to<TestObject>, true, InstanceAllocator>>(false, { pair<const TestObject, int>(1, 1) }, "fixed_hash_multimap (no overflow)", false, true);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_hash_set<TestObject, 8, 9, true, hash<TestObject>, equal_to<TestObject>, true, InstanceAllocator>>(false, { TestObject{1}, TestObject{2}, TestObject{3} }, "fixed_hash_set (no overflow)", true, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_hash_multiset<TestObject, 8, 9, true, hash<TestObject>, equal_to<TestObject>, true, InstanceAllocator>>(false, { TestObject{1}, TestObject{2}, TestObject{3} }, "fixed_hash_multiset (no overflow)", true, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_string<char, 8, true, InstanceAllocator>>(false, { 'a', 'b', 'c' }, "fixed_string (no overflow)", false, true);
		
		// fixed types that will use the overflow allocator and therefore the constructor can elide copying or moving elements, ie. equivalent to a regular move constructor:
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_list<TestObject, 2, true, InstanceAllocator>>(true, { TestObject{1}, TestObject{2}, TestObject{3} }, "fixed_list (use overflow)", false, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_slist<TestObject, 2, true, InstanceAllocator>>(true, { TestObject{1}, TestObject{2}, TestObject{3} }, "fixed_slist (use overflow)", false, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_vector<TestObject, 2, true, InstanceAllocator>>(true, { TestObject{1}, TestObject{2}, TestObject{3} }, "fixed_vector (use overflow)", false, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_map<TestObject, int, 2, true, less<TestObject>, InstanceAllocator>>(true, { pair<const TestObject, int>(1, 1) }, "fixed_map (use overflow)", false, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_multimap<TestObject, int, 2, true, less<TestObject>, InstanceAllocator>>(true, { pair<const TestObject, int>(1, 1) }, "fixed_multimap (use overflow)", false, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_set<TestObject, 2, true, less<TestObject>, InstanceAllocator>>(true, { TestObject{1}, TestObject{2}, TestObject{3} }, "fixed_set (use overflow)", false, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_multiset<TestObject, 2, true, less<TestObject>, InstanceAllocator>>(true, { TestObject{1}, TestObject{2}, TestObject{3} }, "fixed_multiset (use overflow)", false, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_hash_map<TestObject, int, 2, 3, true, hash<TestObject>, equal_to<TestObject>, true, InstanceAllocator>>(true, { pair<const TestObject, int>(1, 1) }, "fixed_hash_map (use overflow)", false, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_hash_multimap<TestObject, int, 2, 3, true, hash<TestObject>, equal_to<TestObject>, true, InstanceAllocator>>(true, { pair<const TestObject, int>(1, 1) }, "fixed_hash_multimap (use overflow)", false, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_hash_set<TestObject, 2, 3, true, hash<TestObject>, equal_to<TestObject>, true, InstanceAllocator>>(true, { TestObject{1}, TestObject{2}, TestObject{3} }, "fixed_hash_set (use overflow)", false, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_hash_multiset<TestObject, 2, 3, true, hash<TestObject>, equal_to<TestObject>, true, InstanceAllocator>>(true, { TestObject{1}, TestObject{2}, TestObject{3} }, "fixed_hash_multiset (use overflow)", false, false);
		//EATEST_VERIFY(TestObject::Reset());
		//nErrorCount += TestContainerCtorMoveWithAllocator<fixed_string<char, 2, true, InstanceAllocator>>(true, { 'a', 'b', 'c' }, "fixed_string (use overflow)", false, false);

		// following types haven't implemented the appropriate constructor:
		// segmented_vector
		// list_map
		// string_map
		// string_hash_map

		// other containers not tested:
		// container adaptors are not tested because their behaviour should be the same as their underlying container.
		// tuple_vector_alloc - what is this type?
	}

	return nErrorCount;
}
