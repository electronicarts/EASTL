/////////////////////////////////////////////////////////////////////////////
// TestFixedTupleVector.cpp
//
// Copyright (c) 2016, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"

#include <EASTL/fixed_tuple_vector.h>

using namespace eastl;

int TestFixedTupleVector()
{
	int nErrorCount = 0;

	// Test uninit'ed push-backs
	{
		fixed_tuple_vector<4, false, int> singleElementVec;
		EATEST_VERIFY(singleElementVec.size() == 0);
		EATEST_VERIFY(singleElementVec.capacity() == 0);
		singleElementVec.push_back_uninitialized();
		singleElementVec.push_back(5);
		EATEST_VERIFY(singleElementVec.size() == 2);
		EATEST_VERIFY(
			singleElementVec.get<0>()[1] == 5
		);
		EATEST_VERIFY(singleElementVec.get<int>()[1] == 5);
	}

	return nErrorCount;
}


