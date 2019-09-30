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
	int nErrorCount = 0;

	TestObject::Reset();

	{
		eastl::segmented_vector<int, 8> sv;
		sv.push_back(0);
		sv.push_back(1);
		sv.push_back(2);
		sv.push_back(3);

		{
			auto i = sv.begin();
			EATEST_VERIFY(*i  == 0);
			EATEST_VERIFY(*i++ == 0);
			EATEST_VERIFY(*i++ == 1);
			EATEST_VERIFY(*i++ == 2);
			EATEST_VERIFY(*i++ == 3);
		}

		{
			auto i = sv.begin();
			EATEST_VERIFY(*i  == 0);
			EATEST_VERIFY(*(++i) == 1);
			EATEST_VERIFY(*(++i) == 2);
			EATEST_VERIFY(*(++i) == 3);
		}
	}

	{
		// Construct segmented_vectors of different types.
		eastl::segmented_vector<int, 8> vectorOfInt;
		eastl::segmented_vector<TestObject, 8> vectorOfTO;
		eastl::segmented_vector<eastl::list<TestObject>, 8> vectorOfListOfTO;

		EATEST_VERIFY(vectorOfInt.empty());
		EATEST_VERIFY(vectorOfTO.empty());
		EATEST_VERIFY(vectorOfListOfTO.empty());
	}

	{
		// Test basic segmented_vector operations.
		eastl::segmented_vector<int, 4> vectorOfInt;

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
