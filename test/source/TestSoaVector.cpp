/////////////////////////////////////////////////////////////////////////////
// TestTuple.cpp
//
// Copyright (c) 2014, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"

#include <EASTL/soavector.h>

using namespace eastl;

int TestSoaVector()
{
	int nErrorCount = 0;

	// Test empty push-backs
	{
		soa_vector<int> singleElementVec;
		singleElementVec.push_back();
		vector<int>& vec = (singleElementVec).get<0>();
		EATEST_VERIFY(vec.size() == 1);
		EATEST_VERIFY(singleElementVec.get<0>().size() == 1);

		soa_vector<int, float> doubleElementVec;
		doubleElementVec.push_back();
		EATEST_VERIFY(doubleElementVec.get<0>().size() == 1);
		EATEST_VERIFY(doubleElementVec.get<1>().size() == 1);
		EATEST_VERIFY((void*)doubleElementVec.get<0>().data() !=
			(void*)doubleElementVec.get<1>().data());

		typedef struct
		{
			float a;
			int b;
			float c;
			float d[4];
		} NonprimitiveStruct;

		soa_vector<int, NonprimitiveStruct> nonPrimitveElementVec;
		vector<NonprimitiveStruct>& npsVec = nonPrimitveElementVec.get<1>();
		nonPrimitveElementVec.push_back();
		nonPrimitveElementVec.get<0>()[0] = 0;
		nonPrimitveElementVec.get<1>()[0].a = 0.0f;
		nonPrimitveElementVec.get<1>()[0].b = 1;

		EATEST_VERIFY(npsVec[0].b == 1);

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


