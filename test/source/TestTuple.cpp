/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"

EA_DISABLE_VC_WARNING(4623 4625 4413 4510)

#include <EASTL/tuple.h>

#if EASTL_TUPLE_ENABLED

namespace TestTupleInternal
{

struct DefaultConstructibleType
{
	static const int defaultVal = 0x1EE7C0DE;
	DefaultConstructibleType() : mVal(defaultVal) {}
	int mVal;
};

struct MoveOnlyType
{
// tuple should work with non default constructible types but due to a bug in VS2103 is_default_constructible type trait
// it doesn't work there
#if !defined(_MSC_VER) || (_MSC_VER > 1800)
	MoveOnlyType() = delete;
#else
	MoveOnlyType() : mVal() {}
#endif
	MoveOnlyType(int val) : mVal(val) {}
	MoveOnlyType(const MoveOnlyType&) = delete;
	MoveOnlyType(MoveOnlyType&& x) : mVal(x.mVal) { x.mVal = 0; }
	MoveOnlyType& operator=(const MoveOnlyType&) = delete;
	MoveOnlyType& operator=(MoveOnlyType&& x)
	{
		mVal = x.mVal;
		x.mVal = 0;
		return *this;
	}

	int mVal;
};

struct OperationCountingType
{
	OperationCountingType() : mVal() { ++mDefaultConstructorCalls; }
	OperationCountingType(int x) : mVal(x) { ++mIntConstructorCalls; }
	OperationCountingType(const OperationCountingType& x) : mVal(x.mVal) { ++mCopyConstructorCalls; }
	OperationCountingType(OperationCountingType&& x) : mVal(x.mVal)
	{
		++mMoveConstructorCalls;
		x.mVal = 0;
	}
	OperationCountingType& operator=(const OperationCountingType& x)
	{
		mVal = x.mVal;
		++mCopyAssignmentCalls;
		return *this;
	}
	OperationCountingType& operator=(OperationCountingType&& x)
	{
		mVal = x.mVal;
		x.mVal = 0;
		++mMoveAssignmentCalls;
		return *this;
	}
	~OperationCountingType() { ++mDestructorCalls; }

	int mVal;

	static void ResetCounters()
	{
		mDefaultConstructorCalls = 0;
		mIntConstructorCalls = 0;
		mCopyConstructorCalls = 0;
		mMoveConstructorCalls = 0;
		mCopyAssignmentCalls = 0;
		mMoveAssignmentCalls = 0;
		mDestructorCalls = 0;
	}

	static int mDefaultConstructorCalls;
	static int mIntConstructorCalls;
	static int mCopyConstructorCalls;
	static int mMoveConstructorCalls;
	static int mCopyAssignmentCalls;
	static int mMoveAssignmentCalls;
	static int mDestructorCalls;
};

int OperationCountingType::mDefaultConstructorCalls = 0;
int OperationCountingType::mIntConstructorCalls = 0;
int OperationCountingType::mCopyConstructorCalls = 0;
int OperationCountingType::mMoveConstructorCalls = 0;
int OperationCountingType::mCopyAssignmentCalls = 0;
int OperationCountingType::mMoveAssignmentCalls = 0;
int OperationCountingType::mDestructorCalls = 0;

}  // namespace TestTupleInternal

int TestTuple()
{
	using namespace eastl;
	using namespace TestTupleInternal;

	int nErrorCount = 0;

	static_assert(tuple_size<tuple<int>>::value == 1, "tuple_size<tuple<T>> test failed.");
	static_assert(tuple_size<const tuple<int>>::value == 1, "tuple_size<const tuple<T>> test failed.");
	static_assert(tuple_size<const tuple<const int>>::value == 1, "tuple_size<const tuple<const T>> test failed.");
	static_assert(tuple_size<volatile tuple<int>>::value == 1, "tuple_size<volatile tuple<T>> test failed.");
	static_assert(tuple_size<const volatile tuple<int>>::value == 1,
				  "tuple_size<const volatile tuple<T>> test failed.");
	static_assert(tuple_size<tuple<int, float, bool>>::value == 3, "tuple_size<tuple<T, T, T>> test failed.");

	static_assert(is_same<tuple_element_t<0, tuple<int>>, int>::value, "tuple_element<I, T> test failed.");
	static_assert(is_same<tuple_element_t<1, tuple<float, int>>, int>::value, "tuple_element<I, T> test failed.");
	static_assert(is_same<tuple_element_t<1, tuple<float, const int>>, const int>::value,
				  "tuple_element<I, T> test failed.");
	static_assert(is_same<tuple_element_t<1, tuple<float, volatile int>>, volatile int>::value,
				  "tuple_element<I, T> test failed.");
	static_assert(is_same<tuple_element_t<1, tuple<float, const volatile int>>, const volatile int>::value,
				  "tuple_element<I, T> test failed.");
	static_assert(is_same<tuple_element_t<1, tuple<float, int&>>, int&>::value, "tuple_element<I, T> test failed.");

	{
		tuple<int> aSingleElementTuple(1);
		EATEST_VERIFY(get<0>(aSingleElementTuple) == 1);
		get<0>(aSingleElementTuple) = 2;
		EATEST_VERIFY(get<0>(aSingleElementTuple) == 2);
		get<int>(aSingleElementTuple) = 3;
		EATEST_VERIFY(get<int>(aSingleElementTuple) == 3);

		const tuple<int> aConstSingleElementTuple(3);
		EATEST_VERIFY(get<0>(aConstSingleElementTuple) == 3);
		EATEST_VERIFY(get<int>(aConstSingleElementTuple) == 3);

		tuple<DefaultConstructibleType> aDefaultConstructedTuple;
		EATEST_VERIFY(get<0>(aDefaultConstructedTuple).mVal == DefaultConstructibleType::defaultVal);

		OperationCountingType::ResetCounters();
		tuple<OperationCountingType> anOperationCountingTuple;
		EATEST_VERIFY(OperationCountingType::mDefaultConstructorCalls == 1 &&
					  get<0>(anOperationCountingTuple).mVal == 0);
		get<0>(anOperationCountingTuple).mVal = 1;
		tuple<OperationCountingType> anotherOperationCountingTuple(anOperationCountingTuple);
		EATEST_VERIFY(OperationCountingType::mDefaultConstructorCalls == 1 &&
					  OperationCountingType::mCopyConstructorCalls == 1 &&
					  get<0>(anotherOperationCountingTuple).mVal == 1);
		get<0>(anOperationCountingTuple).mVal = 2;
		anotherOperationCountingTuple = anOperationCountingTuple;
		EATEST_VERIFY(
			OperationCountingType::mDefaultConstructorCalls == 1 && OperationCountingType::mCopyConstructorCalls == 1 &&
			OperationCountingType::mCopyAssignmentCalls == 1 && get<0>(anotherOperationCountingTuple).mVal == 2);

		OperationCountingType::ResetCounters();
		tuple<OperationCountingType> yetAnotherOperationCountingTuple(OperationCountingType(5));
		EATEST_VERIFY(
			OperationCountingType::mMoveConstructorCalls == 1 && OperationCountingType::mDefaultConstructorCalls == 0 &&
			OperationCountingType::mCopyConstructorCalls == 0 && get<0>(yetAnotherOperationCountingTuple).mVal == 5);
	}

	EATEST_VERIFY(OperationCountingType::mDestructorCalls == 4);

	{
		// Test constructor
		tuple<int, float, bool> aTuple(1, 1.0f, true);
		EATEST_VERIFY(get<0>(aTuple) == 1);
		EATEST_VERIFY(get<1>(aTuple) == 1.0f);
		EATEST_VERIFY(get<2>(aTuple) == true);
		EATEST_VERIFY(get<int>(aTuple) == 1);
		EATEST_VERIFY(get<float>(aTuple) == 1.0f);
		EATEST_VERIFY(get<bool>(aTuple) == true);

		get<1>(aTuple) = 2.0f;
		EATEST_VERIFY(get<1>(aTuple) == 2.0f);

		// Test copy constructor
		tuple<int, float, bool> anotherTuple(aTuple);
		EATEST_VERIFY(get<0>(anotherTuple) == 1 && get<1>(anotherTuple) == 2.0f && get<2>(anotherTuple) == true);

		// Test copy assignment
		tuple<int, float, bool> yetAnotherTuple(2, 3.0f, true);
		EATEST_VERIFY(get<0>(yetAnotherTuple) == 2 && get<1>(yetAnotherTuple) == 3.0f &&
					  get<2>(yetAnotherTuple) == true);
		yetAnotherTuple = anotherTuple;
		EATEST_VERIFY(get<0>(yetAnotherTuple) == 1 && get<1>(yetAnotherTuple) == 2.0f &&
					  get<2>(yetAnotherTuple) == true);

		// Test converting 'copy' constructor (from a tuple of different type whose members are each convertible)
		tuple<double, double, bool> aDifferentTuple(aTuple);
		EATEST_VERIFY(get<0>(aDifferentTuple) == 1.0 && get<1>(aDifferentTuple) == 2.0 &&
					  get<2>(aDifferentTuple) == true);

		// Test converting assignment operator (from a tuple of different type whose members are each convertible)
		tuple<double, double, bool> anotherDifferentTuple;
		EATEST_VERIFY(get<0>(anotherDifferentTuple) == 0.0 && get<1>(anotherDifferentTuple) == 0.0 &&
					  get<2>(anotherDifferentTuple) == false);
		anotherDifferentTuple = anotherTuple;
		EATEST_VERIFY(get<0>(anotherDifferentTuple) == 1.0 && get<1>(anotherDifferentTuple) == 2.0 &&
					  get<2>(anotherDifferentTuple) == true);

		// Test default initialization (built in types should be value initialized rather than default initialized)
		tuple<int, float, bool> aDefaultInitializedTuple;
		EATEST_VERIFY(get<0>(aDefaultInitializedTuple) == 0 && get<1>(aDefaultInitializedTuple) == 0.0f &&
					  get<2>(aDefaultInitializedTuple) == false);
	}

	{
		// Test some other cases with typed-getter
		tuple<double, double, bool> aTupleWithRepeatedType(1.0f, 2.0f, true);
		EATEST_VERIFY(get<bool>(aTupleWithRepeatedType) == true);

		tuple<double, bool, double> anotherTupleWithRepeatedType(1.0f, true, 2.0f);
		EATEST_VERIFY(get<bool>(anotherTupleWithRepeatedType) == true);

		tuple<bool, double, double> yetAnotherTupleWithRepeatedType(true, 1.0f, 2.0f);
		EATEST_VERIFY(get<bool>(anotherTupleWithRepeatedType) == true);

		struct floatOne { float val; };
		struct floatTwo { float val; };
		tuple<floatOne, floatTwo> aTupleOfStructs({ 1.0f }, { 2.0f } );
		EATEST_VERIFY(get<floatOne>(aTupleOfStructs).val == 1.0f);
		EATEST_VERIFY(get<floatTwo>(aTupleOfStructs).val == 2.0f);
		
		const tuple<double, double, bool> aConstTuple(aTupleWithRepeatedType);
		const bool& constRef = get<bool>(aConstTuple);
		EATEST_VERIFY(constRef == true);

		const bool&& constRval = get<bool>(eastl::move(aTupleWithRepeatedType));
		EATEST_VERIFY(constRval == true);
	}

	{
		tuple<int, float> aTupleWithDefaultInit(1, {});

		// tuple construction from pair
		pair<int, float> aPair(1, 2.0f);
		tuple<int, float> aTuple(aPair);
		EATEST_VERIFY(get<0>(aTuple) == 1 && get<1>(aTuple) == 2.0f);
		tuple<double, double> anotherTuple(aPair);
		EATEST_VERIFY(get<0>(anotherTuple) == 1.0 && get<1>(anotherTuple) == 2.0);
		anotherTuple = make_pair(2, 3);
		EATEST_VERIFY(get<0>(anotherTuple) == 2.0 && get<1>(anotherTuple) == 3.0);

		// operators: ==, !=, <
		anotherTuple = aTuple;
		EATEST_VERIFY(aTuple == anotherTuple);
		EATEST_VERIFY(!(aTuple < anotherTuple) && !(anotherTuple < aTuple));
		tuple<double, double> aDefaultInitTuple;
		EATEST_VERIFY(aTuple != aDefaultInitTuple);
		EATEST_VERIFY(aDefaultInitTuple < aTuple);

		tuple<int, int, int> lesserTuple(1, 2, 3);
		tuple<int, int, int> greaterTuple(1, 2, 4);
		EATEST_VERIFY(lesserTuple < greaterTuple && !(greaterTuple < lesserTuple) && greaterTuple > lesserTuple &&
					  !(lesserTuple > greaterTuple));

		// swap
		swap(lesserTuple, greaterTuple);
		EATEST_VERIFY(get<2>(lesserTuple) == 4 && get<2>(greaterTuple) == 3);
		swap(greaterTuple, lesserTuple);
		EATEST_VERIFY(lesserTuple < greaterTuple);
	}

	{
		// Test construction of tuple containing a move only type
		static_assert(is_constructible<MoveOnlyType, MoveOnlyType>::value,
					  "is_constructible type trait giving confusing answers.");
		static_assert(is_constructible<MoveOnlyType, MoveOnlyType&&>::value,
					  "is_constructible type trait giving wrong answers.");
		// Static assert below fails on clang OSX
		// static_assert(is_constructible<MoveOnlyType&&, MoveOnlyType&&>::value, "is_constructible type trait giving
		// bizarre answers.");
		tuple<MoveOnlyType> aTupleWithMoveOnlyMember(1);
		EATEST_VERIFY(get<0>(aTupleWithMoveOnlyMember).mVal == 1);
		get<0>(aTupleWithMoveOnlyMember) = MoveOnlyType(2);
		EATEST_VERIFY(get<0>(aTupleWithMoveOnlyMember).mVal == 2);
	}

	{
		// Tuple helpers

		// make_tuple
		auto makeTuple = make_tuple(1, 2.0, true);
		EATEST_VERIFY(get<0>(makeTuple) == 1 && get<1>(makeTuple) == 2.0 && get<2>(makeTuple) == true);

		// TODO: reference_wrapper implementation needs to be finished to enable this code
		/*
		int a = 2;
		float b = 3.0f;
		auto makeTuple2 = make_tuple(ref(a), b);
		get<0>(makeTuple2) = 3;
		get<1>(makeTuple2) = 4.0f;
		EATEST_VERIFY(get<0>(makeTuple2) == 3 && get<1>(makeTuple2) == 4.0f && a == 3 && b == 3.0f);
		*/

		// forward_as_tuple
		auto forwardTest = [](tuple < MoveOnlyType &&, MoveOnlyType && > x)->tuple<MoveOnlyType, MoveOnlyType>
		{
			return tuple<MoveOnlyType, MoveOnlyType>(move(x));
		};

		tuple<MoveOnlyType, MoveOnlyType> aMovableTuple(
			forwardTest(forward_as_tuple(MoveOnlyType(1), MoveOnlyType(2))));
		EATEST_VERIFY(get<0>(aMovableTuple).mVal == 1 && get<1>(aMovableTuple).mVal == 2);

		// tie
		int a = 0;
		double b = 0.0f;
		static_assert(is_assignable<const Internal::ignore_t&, int>::value, "ignore_t not assignable");
		static_assert(Internal::TupleAssignable<tuple<const Internal::ignore_t&>, tuple<int>>::value, "Not assignable");
		tie(a, ignore, b) = make_tuple(1, 3, 5);
		EATEST_VERIFY(a == 1 && b == 5.0f);
		// tuple_cat
		auto tcatRes = tuple_cat(make_tuple(1, 2.0f), make_tuple(3.0, true));
		EATEST_VERIFY(get<0>(tcatRes) == 1 && get<1>(tcatRes) == 2.0f && get<2>(tcatRes) == 3.0 &&
					  get<3>(tcatRes) == true);

		auto tcatRes2 = tuple_cat(make_tuple(1, 2.0f), make_tuple(3.0, true), make_tuple(5u, '6'));
		EATEST_VERIFY(get<0>(tcatRes2) == 1 && get<1>(tcatRes2) == 2.0f && get<2>(tcatRes2) == 3.0 &&
					  get<3>(tcatRes2) == true && get<4>(tcatRes2) == 5u && get<5>(tcatRes2) == '6');

		auto aCattedRefTuple = tuple_cat(make_tuple(1), tie(a, ignore, b));
		get<1>(aCattedRefTuple) = 2;
		EATEST_VERIFY(a == 2);

		// Empty tuple
		tuple<> emptyTuple;
		EATEST_VERIFY(tuple_size<decltype(emptyTuple)>::value == 0);
		emptyTuple = make_tuple();
		auto anotherEmptyTuple = make_tuple();
		swap(anotherEmptyTuple, emptyTuple);
	}

	return nErrorCount;
}

#else

int TestTuple() { return 0; }

#endif  // EASTL_TUPLE_ENABLED

EA_RESTORE_VC_WARNING()
