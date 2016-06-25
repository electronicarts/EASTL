/////////////////////////////////////////////////////////////////////////////
// TestTuple.cpp
//
// Copyright (c) 2014, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"

#include <EASTL/soavector.h>

//EASTL_SOA_VECTOR_DECL(IntSoaVec
//	int, element
//)

int TestSoaVector()
{
	using namespace eastl;

	int nErrorCount = 0;

	// Test empty push-backs
	{
		soa_vector<int> singleElementVec;
		singleElementVec.push_back();
//		EATEST_VERIFY(get<0>(singleElementVec).size()  == 1);

		soa_vector<int, float> doubleElementVec;
		doubleElementVec.push_back();
		//EATEST_VERIFY(get<0>(doubleElementVec).size() == 1);
		//EATEST_VERIFY(get<1>(doubleElementVec).size() == 1);

		//IntSoaVec macroSingleElementVec;
		//macroSingleElementVec.push_back(1);
		//EATEST_VERIFY(macroSingleElementVec.element[0] == 1);


	}

	{
		//soa_vector<int> singleElementVec;
		//singleElementVec.push_back(1);
		//EATEST_VERIFY(get<0>(singleElementVec)[0]  == 1);

		//soa_vector<int, float> doubleElementVec;
		//doubleElementVec.push_back({ 2, 3.0f });
		//EATEST_VERIFY(get<0>(doubleElementVec)[0] == 2);
		//EATEST_VERIFY(get<1>(doubleElementVec)[0] == 3.0f);

		//IntSoaVec macroSingleElementVec;
		//macroSingleElementVec.push_back(1);
		//EATEST_VERIFY(macroSingleElementVec.element[0] == 1);
	}

	return nErrorCount;
}


