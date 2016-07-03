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

	// Test uninit'ed push-backs
	{
		soa_vector<int> singleElementVec;
		singleElementVec.push_back_uninitialized();
		vector<int>& vec = (singleElementVec).get<0>();
		EATEST_VERIFY(vec.size() == 1);
		EATEST_VERIFY(singleElementVec.get<0>().size() == 1);
		EATEST_VERIFY(singleElementVec.get<int>().size() == 1);

		// Demonstration that we can use multiple types if we only use size_t get
		// Changing get<0> or get<1> to get<int> will cause a compiler error.
		soa_vector<int, int> doubleElementVec;
		doubleElementVec.push_back_uninitialized();
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
		} NonPrimitiveStruct;

		soa_vector<int, NonPrimitiveStruct> nonPrimitiveElementVec;
		vector<NonPrimitiveStruct>& npsVec = nonPrimitiveElementVec.get<NonPrimitiveStruct>();
		nonPrimitiveElementVec.push_back_uninitialized();
		nonPrimitiveElementVec.get<0>()[0] = 0;
		nonPrimitiveElementVec.get<1>()[0].a = 0.0f;
		nonPrimitiveElementVec.get<1>()[0].b = 1;

		EATEST_VERIFY(npsVec[0].b == 1);

		// Test the macro declaration
		SOA_VECTOR_DECL_3(MultiSoaVec
			,int, Integers
			,float, Reals
			,bool, Bools);
		
		MultiSoaVec multiSoaVec;
		multiSoaVec.push_back_uninitialized();
		EATEST_VERIFY(multiSoaVec.Integers().size() == 1);
		EATEST_VERIFY(multiSoaVec.Reals().size() == 1);
		EATEST_VERIFY((void*)multiSoaVec.Integers().data() != (void*)multiSoaVec.Reals().data());
	}

	// Test non-empty push-backs
	{
		soa_vector<int> singleElementVec;
		singleElementVec.push_back(2);
		vector<int>& vec = (singleElementVec).get<0>();
		EATEST_VERIFY(vec.size() == 1);
		EATEST_VERIFY(singleElementVec.get<0>().size() == 1);
		EATEST_VERIFY(vec[0] == 2);

		soa_vector<int, float> doubleElementVec;
		doubleElementVec.push_back(2, 3.0f);
		EATEST_VERIFY(doubleElementVec.get<0>().size() == 1);
		EATEST_VERIFY(doubleElementVec.get<1>().size() == 1);
		EATEST_VERIFY((void*)doubleElementVec.get<0>().data() !=
			(void*)doubleElementVec.get<1>().data());
		EATEST_VERIFY(doubleElementVec.get<0>()[0] == 2);
		EATEST_VERIFY(doubleElementVec.get<1>()[0] == 3.0f);

		typedef struct
		{
			float a;
			int b;
			float c;
			float d[4];
		} NonprimitiveStruct;

		soa_vector<int, NonprimitiveStruct> nonPrimitiveElementVec;
		nonPrimitiveElementVec.push_back(2, { 1.0f, 2, 3.0f, {1.0f, 1.25f, 1.75f, 2.0f} });
		EATEST_VERIFY(nonPrimitiveElementVec.get<1>()[0].b == 2);
	}

	// Test soa_vector::Element
	{
		soa_vector<int, float> doubleElementVec;
		auto& doubleElement = doubleElementVec.push_back();
		doubleElement.get<0>() = 2;
		doubleElement.get<float>() = 3.0f;
		EATEST_VERIFY(doubleElementVec.get<0>()[0] == 2);
		EATEST_VERIFY(doubleElementVec.get<1>()[0] == 3.0f);
		EATEST_VERIFY(doubleElementVec.size() == 1);
	}

	// Test soa_Vector in a ranged-for
	{
		soa_vector<int, float> doubleElementVec;
		doubleElementVec.push_back(1, 2.0f);
		doubleElementVec.push_back(2, 3.0f);
		doubleElementVec.push_back(3, 4.0f);
		doubleElementVec.push_back(4, 5.0f);
		doubleElementVec.push_back(5, 6.0f);

		int i = 1;
		for (auto element : doubleElementVec)
		{
			EATEST_VERIFY(element.get<0>() == i);
			++i;
		}
	}
	return nErrorCount;
}


