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
		EATEST_VERIFY(singleElementVec.size() == 0);
		EATEST_VERIFY(singleElementVec.capacity() == 0);
		singleElementVec.push_back_uninitialized();
		singleElementVec.push_back(5);
		EATEST_VERIFY(singleElementVec.size() == 2);
		EATEST_VERIFY(singleElementVec.get<0>()[1] == 5);
		EATEST_VERIFY(singleElementVec.get<int>()[1] == 5);

		
		
		tuple_vector<int, float, bool> complexVec;
		complexVec.push_back(3, 2.0f, true);
		complexVec.push_back(1, 4.0f, false);
		complexVec.push_back(2, 1.0f, true);
		complexVec.push_back(4, 3.0f, false);
		EATEST_VERIFY(*(complexVec.get<0>()) == 3);
		EATEST_VERIFY(complexVec.get<float>()[1] == 4.0f);
		EATEST_VERIFY(complexVec.get<2>()[2] == complexVec.get<bool>()[2]);

		__declspec(align(16)) struct AlignTestVec4
		{
			float a[4];
			AlignTestVec4() :a{ 1.0f, 2.0f, 3.0f, 4.0f } {}
		};

		struct AlignTestByte3
		{
			char a[3];
			AlignTestByte3() :a{ 1, 2, 3 } {}
		};

		__declspec(align(8)) struct AlignTestFourByte
		{
			int a[5];
			AlignTestFourByte() :a{ -1, -2, -3, -4, -5 } {}
		};

		tuple_vector<bool, AlignTestVec4, AlignTestByte3, AlignTestFourByte> alignElementVec;
		alignElementVec.push_back();
		alignElementVec.push_back();
		alignElementVec.push_back();
		alignElementVec.push_back();
		alignElementVec.push_back();



		//vector<int>& vec = (singleElementVec).get<0>();
		//EATEST_VERIFY(vec.size() == 1);
		//EATEST_VERIFY(singleElementVec.get<0>().size() == 1);
		//EATEST_VERIFY(singleElementVec.get<int>().size() == 1);

		// Demonstration that we can use multiple types if we only use size_t get
		// Changing get<0> or get<1> to get<int> will cause a compiler error.
		//tuple_vector<int, int> doubleElementVec;
		//doubleElementVec.push_back_uninitialized();

		//EATEST_VERIFY(doubleElementVec.get<0>().size() == 1);
		//EATEST_VERIFY(doubleElementVec.get<1>().size() == 1);
		//EATEST_VERIFY((void*)doubleElementVec.get<0>().data() !=
		//	(void*)doubleElementVec.get<1>().data());

		//typedef struct NonPrimitiveStruct
		//{
		//	float a;
		//	int b;
		//	float c;
		//	float d[4];
		//	NonPrimitiveStruct()
		//		:a(1.0f)
		//		,b(2)
		//		,c(3.0f)
		//		, d{ 5.0f, 6.0f, 7.0f, 8.0f }
		//	{}
		//} NonPrimitiveStruct;

		//tuple_vector<int, NonPrimitiveStruct> nonPrimitiveElementVec;
		//nonPrimitiveElementVec.push_back(2, { 1.0f, 2, 3.0f,{ 1.0f, 1.25f, 1.75f, 2.0f } });
		//nonPrimitiveElementVec.push_back();
		//nonPrimitiveElementVec.push_back();
		//nonPrimitiveElementVec.push_back();
		//nonPrimitiveElementVec.push_back(3, { 1.0f, 3, 4.0f,{ 5.0f, 2.25f, 2.75f, 3.0f } });
		//nonPrimitiveElementVec.push_back();
		//nonPrimitiveElementVec.push_back();
		//nonPrimitiveElementVec.push_back();
		//nonPrimitiveElementVec.push_back();
		//EATEST_VERIFY(nonPrimitiveElementVec.size() == 6);
		//EATEST_VERIFY(nonPrimitiveElementVec.get<1>()[4].d[0] == 5.0f);

		//nonPrimitiveElementVec.get<0>()[0] = 0;
		//nonPrimitiveElementVec.get<1>()[0].a = 0.0f;
		//nonPrimitiveElementVec.get<1>()[0].b = 1;

		//EATEST_VERIFY(npsVec[0].b == 1);

		//tuple_vector<int, bool, float> multiTupleVec;
		//multiTupleVec.push_back(-1, true, 1.0f);
		//multiTupleVec.push_back(-2, true, 2.0f);
		//multiTupleVec.push_back(-3, true, 3.0f);
		//multiTupleVec.push_back(-4, true, 4.0f);
		//multiTupleVec.push_back(-5, true, 5.0f);
		//EATEST_VERIFY((size_t)(multiTupleVec.get<2>()) % alignof(float) == 0);
		//EATEST_VERIFY(multiTupleVec.get<2>()[2] == 3.0f);
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
		tuple_vector<int, float, int> tripleElementVec;
		tripleElementVec.push_back(1, 2.0f, 6);
		tripleElementVec.push_back(2, 3.0f, 7);
		tripleElementVec.push_back(3, 4.0f, 8);
		tripleElementVec.push_back(4, 5.0f, 9);
		tripleElementVec.push_back(5, 6.0f, 10);

		float i = 0;
		EATEST_VERIFY(&get<0>(*tripleElementVec.begin()) == tripleElementVec.get<0>());
		EATEST_VERIFY(&get<1>(*tripleElementVec.begin()) == tripleElementVec.get<1>());
		for (auto& iter : tripleElementVec)
		{
			//EATEST_VERIFY(get<0>(iter) == i);
			i += get<1>(iter);
		}
		EATEST_VERIFY(i == 20);
	}
	return nErrorCount;
}


