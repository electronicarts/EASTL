/////////////////////////////////////////////////////////////////////////////
// TestTupleVector.cpp
//
// Copyright (c) 2014, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"

#include <EASTL/tuple_vector.h>

using namespace eastl;

int TestTupleVector()
{
	int nErrorCount = 0;

	// Test uninit'ed push-backs
	{
		tuple_vector<int> singleElementVec;
		EATEST_VERIFY(singleElementVec.sizeof_tuple() == sizeof(int));
		EATEST_VERIFY(singleElementVec.size() == 0);
		EATEST_VERIFY(singleElementVec.capacity() == 0);
		singleElementVec.push_back_uninitialized();
		singleElementVec.push_back(5);
		EATEST_VERIFY(singleElementVec.size() == 2);
		EATEST_VERIFY(singleElementVec.get<0>()[1] == 5);

		//vector<int>& vec = (singleElementVec).get<0>();
		//EATEST_VERIFY(vec.size() == 1);
		//EATEST_VERIFY(singleElementVec.get<0>().size() == 1);
		//EATEST_VERIFY(singleElementVec.get<int>().size() == 1);

		// Demonstration that we can use multiple types if we only use size_t get
		// Changing get<0> or get<1> to get<int> will cause a compiler error.
		tuple_vector<int, int> doubleElementVec;
		doubleElementVec.push_back_uninitialized();
		EATEST_VERIFY(doubleElementVec.sizeof_tuple() == sizeof(int) + sizeof(int));

		//EATEST_VERIFY(doubleElementVec.get<0>().size() == 1);
		//EATEST_VERIFY(doubleElementVec.get<1>().size() == 1);
		//EATEST_VERIFY((void*)doubleElementVec.get<0>().data() !=
		//	(void*)doubleElementVec.get<1>().data());

		typedef struct NonPrimitiveStruct
		{
			float a;
			int b;
			float c;
			float d[4];
			//NonPrimitiveStruct()
			//	:a(1.0f)
			//	,b(2)
			//	,c(3.0f)
			//	, d{ 5.0f, 6.0f, 7.0f, 8.0f }
			//{}
		} NonPrimitiveStruct;

		tuple_vector<int, NonPrimitiveStruct> nonPrimitiveElementVec;
		nonPrimitiveElementVec.push_back(2, { 1.0f, 2, 3.0f,{ 1.0f, 1.25f, 1.75f, 2.0f } });
		nonPrimitiveElementVec.push_back();
		nonPrimitiveElementVec.push_back();
		nonPrimitiveElementVec.push_back();
		nonPrimitiveElementVec.push_back(3, { 1.0f, 3, 4.0f,{ 5.0f, 2.25f, 2.75f, 3.0f } });
		nonPrimitiveElementVec.push_back();
		//nonPrimitiveElementVec.push_back();
		//nonPrimitiveElementVec.push_back();
		//nonPrimitiveElementVec.push_back();
		EATEST_VERIFY(nonPrimitiveElementVec.size() == 6);
		EATEST_VERIFY(nonPrimitiveElementVec.get<1>()[4].d[0] == 5.0f);

		//nonPrimitiveElementVec.get<0>()[0] = 0;
		//nonPrimitiveElementVec.get<1>()[0].a = 0.0f;
		//nonPrimitiveElementVec.get<1>()[0].b = 1;

		//EATEST_VERIFY(npsVec[0].b == 1);

		tuple_vector<int, bool, float> multiTupleVec;
		multiTupleVec.push_back(-1, true, 1.0f);
		multiTupleVec.push_back(-2, true, 2.0f);
		multiTupleVec.push_back(-3, true, 3.0f);
		multiTupleVec.push_back(-4, true, 4.0f);
		multiTupleVec.push_back(-5, true, 5.0f);
		multiTupleVec.push_back(-6, true, 6.0f);
		EATEST_VERIFY(multiTupleVec.get<2>()[2] == 3.0f);
		// Test the macro declaration
		//TUPLE_VECTOR_DECL_3(MultiTupleVec
		//	,int, Integers
		//	,float, Reals
		//	,bool, Bools);
		
		//MultiTupleVec multiTupleVec;
		//multiTupleVec.push_back_uninitialized();
		//EATEST_VERIFY(multiTupleVec.Integers().size() == 1);
		//EATEST_VERIFY(multiTupleVec.Reals().size() == 1);
		//EATEST_VERIFY((void*)multiTupleVec.Integers().data() != (void*)multiTupleVec.Reals().data());
	}

	// Test non-empty push-backs
	{
		//tuple_vector<int> singleElementVec;
		//singleElementVec.push_back(2);
		//vector<int>& vec = (singleElementVec).get<0>();
		//EATEST_VERIFY(vec.size() == 1);
		//EATEST_VERIFY(singleElementVec.get<0>().size() == 1);
		//EATEST_VERIFY(vec[0] == 2);

		//tuple_vector<int, float> doubleElementVec;
		//doubleElementVec.push_back(2, 3.0f);
		//EATEST_VERIFY(doubleElementVec.get<0>().size() == 1);
		//EATEST_VERIFY(doubleElementVec.get<1>().size() == 1);
		//EATEST_VERIFY((void*)doubleElementVec.get<0>().data() !=
		//	(void*)doubleElementVec.get<1>().data());
		//EATEST_VERIFY(doubleElementVec.get<0>()[0] == 2);
		//EATEST_VERIFY(doubleElementVec.get<1>()[0] == 3.0f);

		//typedef struct
		//{
		//	float a;
		//	int b;
		//	float c;
		//	float d[4];
		//} NonprimitiveStruct;

		//tuple_vector<int, NonprimitiveStruct> nonPrimitiveElementVec;
		//nonPrimitiveElementVec.push_back(2, { 1.0f, 2, 3.0f, {1.0f, 1.25f, 1.75f, 2.0f} });
		//EATEST_VERIFY(nonPrimitiveElementVec.get<1>()[0].b == 2);
	}

	// Test tuple_vector::Element
	{
		//tuple_vector<int, float> doubleElementVec;
		//auto& doubleElement = doubleElementVec.push_back();
		//doubleElement.get<0>() = 2;
		//doubleElement.get<float>() = 3.0f;
		//EATEST_VERIFY(doubleElementVec.get<0>()[0] == 2);
		//EATEST_VERIFY(doubleElementVec.get<1>()[0] == 3.0f);
		//EATEST_VERIFY(doubleElementVec.size() == 1);
	}

	// Test tuple_Vector in a ranged-for
	{
		//tuple_vector<int, float> doubleElementVec;
		//doubleElementVec.push_back(1, 2.0f);
		//doubleElementVec.push_back(2, 3.0f);
		//doubleElementVec.push_back(3, 4.0f);
		//doubleElementVec.push_back(4, 5.0f);
		//doubleElementVec.push_back(5, 6.0f);

		//int i = 1;
		//for (auto element : doubleElementVec)
		//{
		//	//EATEST_VERIFY(element.get<0>() == i);
		//	++i;
		//}
	}
	return nErrorCount;
}


