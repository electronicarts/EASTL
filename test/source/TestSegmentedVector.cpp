/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EASTL/segmented_vector.h>
#include <EASTL/list.h>

// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::segmented_vector<bool, 16>;
template class eastl::segmented_vector<int, 16>;
template class eastl::segmented_vector<Align64, 16>;
template class eastl::segmented_vector<TestObject, 16>;


int TestSegmentedVector()
{
	EASTLTest_Printf("TestSegmentedVector\n");

	int nErrorCount = 0;

	TestObject::Reset();

	{
		using namespace eastl;

		// Construct segmented_vectors of different types.

		segmented_vector<int, 8> vectorOfInt;
		segmented_vector<TestObject, 8> vectorOfTO;
		segmented_vector<list<TestObject>, 8> vectorOfListOfTO;

		EATEST_VERIFY(vectorOfInt.empty());
		EATEST_VERIFY(vectorOfTO.empty());
		EATEST_VERIFY(vectorOfListOfTO.empty());
	}

	{
		using namespace eastl;
		
		// Test basic segmented_vector operations.

		segmented_vector<int, 4> vectorOfInt;

		vectorOfInt.push_back(42);
		EATEST_VERIFY(vectorOfInt.size() == 1);
		EATEST_VERIFY(vectorOfInt.segment_count() == 1);
		EATEST_VERIFY(vectorOfInt.empty() == false);

		vectorOfInt.push_back(43);
		vectorOfInt.push_back(44);
		vectorOfInt.push_back(45);
		vectorOfInt.push_back(46);
		EATEST_VERIFY(vectorOfInt.size() == 5);
		EATEST_VERIFY(vectorOfInt.segment_count() == 2);

		EATEST_VERIFY(vectorOfInt.front() == 42);
		EATEST_VERIFY(vectorOfInt.back() == 46);

		vectorOfInt.pop_back();
		EATEST_VERIFY(vectorOfInt.size() == 4);
		EATEST_VERIFY(vectorOfInt.segment_count() == 1);

		vectorOfInt.clear();
		EATEST_VERIFY(vectorOfInt.empty());
		EATEST_VERIFY(vectorOfInt.size() == 0);
		EATEST_VERIFY(vectorOfInt.segment_count() == 0);
	}

	return nErrorCount;
}
