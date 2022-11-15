/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/deque.h>
#include <EASTL/list.h>
#include <EASTL/slist.h>
#include <EASTL/algorithm.h>
#include <EASTL/utility.h>
#include <EASTL/allocator_malloc.h>
#include <EASTL/unique_ptr.h>

#include "ConceptImpls.h"


EA_DISABLE_ALL_VC_WARNINGS()
#ifndef EA_COMPILER_NO_STANDARD_CPP_LIBRARY
	#include <vector>
	#include <string>
#endif
EA_RESTORE_ALL_VC_WARNINGS()


// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::vector<bool>;
template class eastl::vector<int>;
template class eastl::vector<Align64>;
template class eastl::vector<TestObject>;


// This tests "uninitialized_fill" usage in vector when T has a user provided
// address-of operator overload.  In these situations, EASTL containers must use
// the standard utility "eastl::addressof(T)" which is designed to by-pass user
// provided address-of operator overloads.
// 
// Previously written as: 
// 	for(; first != last; ++first, ++currentDest)
// 		::new((void*)&*currentDest) value_type(*first); // & not guaranteed to be a pointer
//
// Bypasses user 'addressof' operators:
// 	for(; n > 0; --n, ++currentDest)
// 		::new(eastl::addressof(*currentDest)) value_type(value);  // guaranteed to be a pointer
//
struct AddressOfOperatorResult {};
struct HasAddressOfOperator 
{
	// problematic 'addressof' operator that doesn't return a pointer type
    AddressOfOperatorResult operator&() const { return {}; }
	bool operator==(const HasAddressOfOperator&) const { return false; }
};
template class eastl::vector<HasAddressOfOperator>;  // force compile all functions of vector



// Test compiler issue that appeared in VS2012 relating to kAlignment
struct StructWithContainerOfStructs
{
	eastl::vector<StructWithContainerOfStructs> children;
};

		// This relatively complex test is to prevent a regression on VS2013.  The data types have what may appear to be
		// strange names (for test code) because the code is based on a test case extracted from the Frostbite codebase.
		// This test is actually invalid and should be removed as const data memebers are problematic for STL container
		// implementations. (ie.  they prevent constructors from being generated).
namespace
{
			EA_DISABLE_VC_WARNING(4512) // disable warning : "assignment operator could not be generated"
#if (defined(_MSC_VER) && (_MSC_VER >= 1900))  // VS2015-preview and later.
			EA_DISABLE_VC_WARNING(5025) // disable warning : "move assignment operator could not be generated"
			EA_DISABLE_VC_WARNING(4626) // disable warning : "assignment operator was implicitly defined as deleted"
			EA_DISABLE_VC_WARNING(5027) // disable warning : "move assignment operator was implicitly defined as deleted"
#endif
struct ScenarioRefEntry
{
	ScenarioRefEntry(const eastl::string& contextDatabase) : ContextDatabase(contextDatabase) {}

	struct RowEntry
	{
		RowEntry(int levelId, int sceneId, int actorId, int partId, const eastl::string& controller)
			: LevelId(levelId), SceneId(sceneId), ActorId(actorId), PartId(partId), Controller(controller)
		{
		}

		int LevelId;
		int SceneId;
		int ActorId;
		int PartId;
		const eastl::string& Controller;
	};
	const eastl::string& ContextDatabase;  // note:  const class members prohibits move semantics
	typedef eastl::vector<RowEntry> RowData;
	RowData Rows;
};
typedef eastl::vector<ScenarioRefEntry> ScenarRefData;
struct AntMetaDataRecord
{
	ScenarRefData ScenarioRefs;
};
typedef eastl::vector<AntMetaDataRecord> MetadataRecords;

struct StructWithConstInt
{
	StructWithConstInt(const int& _i) : i(_i) {}
	const int i;
};

struct StructWithConstRefToInt
{
	StructWithConstRefToInt(const int& _i) : i(_i) {}
	const int& i;
};
#if (defined(_MSC_VER) && (_MSC_VER >= 1900))  // VS2015-preview and later.
	EA_RESTORE_VC_WARNING() // disable warning 5025:  "move assignment operator could not be generated"
	EA_RESTORE_VC_WARNING() // disable warning 4626:  "assignment operator was implicitly defined as deleted"
	EA_RESTORE_VC_WARNING() // disable warning 5027:  "move assignment operator was implicitly defined as deleted"
#endif
EA_RESTORE_VC_WARNING()
}

struct ItemWithConst
{
	ItemWithConst& operator=(const ItemWithConst&);

public:
	ItemWithConst(int _i) : i(_i) {}
	ItemWithConst(const ItemWithConst& x) : i(x.i) {}
	const int i;
};

struct testmovable
{
	EA_NON_COPYABLE(testmovable)
public:
	testmovable() EA_NOEXCEPT {}

	testmovable(testmovable&&) EA_NOEXCEPT {}

	testmovable& operator=(testmovable&&) EA_NOEXCEPT { return *this; }
};

struct TestMoveAssignToSelf
{
	TestMoveAssignToSelf() EA_NOEXCEPT : mMovedToSelf(false)      {}
	TestMoveAssignToSelf(const TestMoveAssignToSelf& other)       { mMovedToSelf = other.mMovedToSelf; }
	TestMoveAssignToSelf& operator=(TestMoveAssignToSelf&&)       { mMovedToSelf = true; return *this; }
	TestMoveAssignToSelf& operator=(const TestMoveAssignToSelf&) = delete;

	bool mMovedToSelf;
};

#if EASTL_VARIABLE_TEMPLATES_ENABLED
	/// custom type-trait which checks if a type is comparable via the <operator.
	template <class, class = eastl::void_t<>>
	struct is_less_comparable : eastl::false_type { };
	template <class T>
	struct is_less_comparable<T, eastl::void_t<decltype(eastl::declval<T>() < eastl::declval<T>())>> : eastl::true_type { };
#else
	// bypass the test since the compiler doesn't support variable templates.
	template <class> struct is_less_comparable : eastl::false_type { };
#endif


int TestVector()
{
	int nErrorCount = 0;
	eastl_size_t i;

	TestObject::Reset();

	{
		MetadataRecords mMetadataRecords;
		AntMetaDataRecord r, s;
		mMetadataRecords.push_back(r);
		mMetadataRecords.push_back(s);
	}

	{
		using namespace eastl;

		// explicit vector();
		vector<int> intArray1;
		vector<TestObject> toArray1;
		vector<list<TestObject> > toListArray1;

		EATEST_VERIFY(intArray1.validate());
		EATEST_VERIFY(intArray1.empty());
		EATEST_VERIFY(toArray1.validate());
		EATEST_VERIFY(toArray1.empty());
		EATEST_VERIFY(toListArray1.validate());
		EATEST_VERIFY(toListArray1.empty());

		// explicit vector(const allocator_type& allocator);
		MallocAllocator::reset_all();
		MallocAllocator ma;
		vector<int, MallocAllocator> intArray6(ma);
		vector<TestObject, MallocAllocator> toArray6(ma);
		vector<list<TestObject>, MallocAllocator> toListArray6(ma);
		intArray6.resize(1);
		toArray6.resize(1);
		toListArray6.resize(1);
		EATEST_VERIFY(MallocAllocator::mAllocCountAll == 3);

		// explicit vector(size_type n, const allocator_type& allocator = EASTL_VECTOR_DEFAULT_ALLOCATOR)
		vector<int> intArray2(10);
		vector<TestObject> toArray2(10);
		vector<list<TestObject> > toListArray2(10);

		EATEST_VERIFY(intArray2.validate());
		EATEST_VERIFY(intArray2.size() == 10);
		EATEST_VERIFY(toArray2.validate());
		EATEST_VERIFY(toArray2.size() == 10);
		EATEST_VERIFY(toListArray2.validate());
		EATEST_VERIFY(toListArray2.size() == 10);

		// vector(size_type n, const value_type& value, const allocator_type& allocator =
		// EASTL_VECTOR_DEFAULT_ALLOCATOR)
		vector<int> intArray3(10, 7);
		vector<TestObject> toArray3(10, TestObject(7));
		vector<list<TestObject> > toListArray3(10, list<TestObject>(7));

		EATEST_VERIFY(intArray3.validate());
		EATEST_VERIFY(intArray3.size() == 10);
		EATEST_VERIFY(intArray3[5] == 7);
		EATEST_VERIFY(toArray3.validate());
		EATEST_VERIFY(toArray3[5] == TestObject(7));
		EATEST_VERIFY(toListArray3.validate());
		EATEST_VERIFY(toListArray3[5] == list<TestObject>(7));

		// vector(const vector& x)
		vector<int> intArray4(intArray2);
		vector<TestObject> toArray4(toArray2);
		vector<list<TestObject> > toListArray4(toListArray2);

		EATEST_VERIFY(intArray4.validate());
		EATEST_VERIFY(intArray4 == intArray2);
		EATEST_VERIFY(toArray4.validate());
		EATEST_VERIFY(toArray4 == toArray2);
		EATEST_VERIFY(intArray4.validate());
		EATEST_VERIFY(toListArray4 == toListArray2);

		// vector(const this_type& x, const allocator_type& allocator)
		MallocAllocator::reset_all();
		vector<int, MallocAllocator> intArray7(intArray6, ma);
		vector<TestObject, MallocAllocator> toArray7(toArray6, ma);
		vector<list<TestObject>, MallocAllocator> toListArray7(toListArray6, ma);
		EATEST_VERIFY(MallocAllocator::mAllocCountAll == 3);

		// vector(InputIterator first, InputIterator last)
		deque<int> intDeque(3);
		deque<TestObject> toDeque(3);
		deque<list<TestObject> > toListDeque(3);

		vector<int> intArray5(intDeque.begin(), intDeque.end());
		vector<TestObject> toArray5(toDeque.begin(), toDeque.end());
		vector<list<TestObject> > toListArray5(toListDeque.begin(), toListDeque.end());

		// vector(std::initializer_list<T> ilist, const Allocator& allocator = EASTL_VECTOR_DEFAULT_ALLOCATOR);
		{
#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
			eastl::vector<float> floatVector{0, 1, 2, 3};

			EATEST_VERIFY(floatVector.size() == 4);
			EATEST_VERIFY((floatVector[0] == 0) && (floatVector[3] == 3));
#endif
		}

		// vector& operator=(const vector& x);
		intArray3 = intArray4;
		toArray3 = toArray4;
		toListArray3 = toListArray4;

		EATEST_VERIFY(intArray3.validate());
		EATEST_VERIFY(intArray3 == intArray4);
		EATEST_VERIFY(toArray3.validate());
		EATEST_VERIFY(toArray3 == toArray4);
		EATEST_VERIFY(intArray3.validate());
		EATEST_VERIFY(toListArray3 == toListArray4);

// this_type& operator=(std::initializer_list<T> ilist);
#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
		intArray3 = {0, 1, 2, 3};
		EATEST_VERIFY((intArray3.size() == 4) && (intArray3[0] == 0) && (intArray3[3] == 3));
#endif
	}

	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();

	{
		using namespace eastl;

		// vector(this_type&& x)
		// vector(this_type&& x, const Allocator& allocator)
		// this_type& operator=(this_type&& x)

		vector<TestObject> vector3TO33(3, TestObject(33));
		vector<TestObject> toVectorA(eastl::move(vector3TO33));
		EATEST_VERIFY((toVectorA.size() == 3) && (toVectorA.front().mX == 33) && (vector3TO33.size() == 0));

		// The following is not as strong a test of this ctor as it could be. A stronger test would be to use
		// IntanceAllocator with different instances.
		vector<TestObject, MallocAllocator> vector4TO44(4, TestObject(44));
		vector<TestObject, MallocAllocator> toVectorB(eastl::move(vector4TO44), MallocAllocator());
		EATEST_VERIFY((toVectorB.size() == 4) && (toVectorB.front().mX == 44) && (vector4TO44.size() == 0));

		vector<TestObject, MallocAllocator> vector5TO55(5, TestObject(55));
		toVectorB = eastl::move(vector5TO55);
		EATEST_VERIFY((toVectorB.size() == 5) && (toVectorB.front().mX == 55) && (vector5TO55.size() == 0));

		// Should be able to emplace_back an item with const members (non-copyable)
		eastl::vector<ItemWithConst> myVec2;
		ItemWithConst& ref = myVec2.emplace_back(42);
		EATEST_VERIFY(myVec2.back().i == 42);
		EATEST_VERIFY(ref.i == 42);
	}

	{
		using namespace eastl;

		// pointer         data();
		// const_pointer   data() const;
		// reference       front();
		// const_reference front() const;
		// reference       back();
		// const_reference back() const;

		vector<int> intArray(10, 7);
		intArray[0] = 10;
		intArray[1] = 11;
		intArray[2] = 12;

		EATEST_VERIFY(intArray.data() == &intArray[0]);
		EATEST_VERIFY(*intArray.data() == 10);
		EATEST_VERIFY(intArray.front() == 10);
		EATEST_VERIFY(intArray.back() == 7);

		const vector<TestObject> toArrayC(10, TestObject(7));

		EATEST_VERIFY(toArrayC.data() == &toArrayC[0]);
		EATEST_VERIFY(*toArrayC.data() == TestObject(7));
		EATEST_VERIFY(toArrayC.front() == TestObject(7));
		EATEST_VERIFY(toArrayC.back() == TestObject(7));
	}

	{
		using namespace eastl;

		// iterator               begin();
		// const_iterator         begin() const;
		// iterator               end();
		// const_iterator         end() const;
		// reverse_iterator       rbegin();
		// const_reverse_iterator rbegin() const;
		// reverse_iterator       rend();
		// const_reverse_iterator rend() const;

		vector<int> intArray(20);
		for (i = 0; i < 20; i++)
			intArray[i] = (int)i;

		i = 0;
		for (vector<int>::iterator it = intArray.begin(); it != intArray.end(); ++it, ++i)
			EATEST_VERIFY(*it == (int)i);

		i = intArray.size() - 1;
		for (vector<int>::reverse_iterator itr = intArray.rbegin(); itr != intArray.rend(); ++itr, --i)
			EATEST_VERIFY(*itr == (int)i);
	}

	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();

	{
		using namespace eastl;

		// void    swap(vector& x);
		// void    assign(size_type n, const value_type& value);
		// void    assign(InputIterator first, InputIterator last);

		const int A[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
		const int B[] = {99, 99, 99, 99, 99};
		const size_t N = sizeof(A) / sizeof(int);
		const size_t M = sizeof(B) / sizeof(int);

		// assign from pointer range
		vector<int> v3;
		v3.assign(A, A + N);
		EATEST_VERIFY(equal(v3.begin(), v3.end(), A));
		EATEST_VERIFY(v3.size() == N);

		// assign from iterator range
		vector<int> v4;
		v4.assign(v3.begin(), v3.end());
		EATEST_VERIFY(equal(v4.begin(), v4.end(), A));
		EATEST_VERIFY(equal(A, A + N, v4.begin()));

		// assign from initializer range with resize
		v4.assign(M, 99);
		EATEST_VERIFY(equal(v4.begin(), v4.end(), B));
		EATEST_VERIFY(equal(B, B + M, v4.begin()));
		EATEST_VERIFY((v4.size() == M) && (M != N));

#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
		// void assign(std::initializer_list<T> ilist);
		v4.assign({0, 1, 2, 3});
		EATEST_VERIFY(v4.size() == 4);
		EATEST_VERIFY((v4[0] == 0) && (v4[3] == 3));
#endif
	}

	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();

	{
		using namespace eastl;

		// reference       operator[](size_type n);
		// const_reference operator[](size_type n) const;
		// reference       at(size_type n);
		// const_reference at(size_type n) const;

		vector<int> intArray(5);
		EATEST_VERIFY(intArray[3] == 0);
		EATEST_VERIFY(intArray.at(3) == 0);

		vector<TestObject> toArray(5);
		EATEST_VERIFY(toArray[3] == TestObject(0));
		EATEST_VERIFY(toArray.at(3) == TestObject(0));

#if EASTL_EXCEPTIONS_ENABLED
		vector<TestObject> vec01(5);

		try
		{
			TestObject& r01 = vec01.at(6);
			EATEST_VERIFY(!(r01 == TestObject(0)));  // Should not get here, as exception thrown.
		}
		catch (std::out_of_range&) { EATEST_VERIFY(true); }
		catch (...) { EATEST_VERIFY(false); }
#endif
	}

	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();

	{
		using namespace eastl;

		// void push_back(const value_type& value);
		// void push_back();
		// void pop_back();
		// void push_back(T&& value);

		vector<int> intArray(6);
		for (i = 0; i < 6; i++)
			intArray[i] = (int)i;

		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 6);
		EATEST_VERIFY(intArray[5] == 5);

		for (i = 0; i < 40; i++)
		{
			int& ref = intArray.push_back();
			EATEST_VERIFY(&ref == &intArray.back());
			ref = 98;
		}

		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 46);
		EATEST_VERIFY(intArray[45] == 98);

		for (i = 0; i < 40; i++)
			intArray.push_back(99);

		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 86);
		EATEST_VERIFY(intArray[85] == 99);

		for (i = 0; i < 30; i++)
			intArray.pop_back();

		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 56);
		EATEST_VERIFY(intArray[5] == 5);
	}

	{
		using namespace eastl;

		// void* push_back_uninitialized();

		int64_t toCount0 = TestObject::sTOCount;

		vector<TestObject> vTO;
		EATEST_VERIFY(TestObject::sTOCount == toCount0);

		for (i = 0; i < 25; i++)
		{
			void* pTO = vTO.push_back_uninitialized();
			EATEST_VERIFY(TestObject::sTOCount == (toCount0 + static_cast<int64_t>(i)));

			new (pTO) TestObject((int)i);
			EATEST_VERIFY(TestObject::sTOCount == (toCount0 + static_cast<int64_t>(i) + 1));
			EATEST_VERIFY(vTO.back().mX == (int)i);
			EATEST_VERIFY(vTO.validate());
		}
	}

	{
		using namespace eastl;

		// template<class... Args>
		// iterator emplace(const_iterator position, Args&&... args);

		// template<class... Args>
		// void emplace_back(Args&&... args);

		// iterator insert(const_iterator position, value_type&& value);
		// void push_back(value_type&& value);

		TestObject::Reset();

		vector<TestObject> toVectorA;

		TestObject& ref = toVectorA.emplace_back(2, 3, 4);
		EATEST_VERIFY((toVectorA.size() == 1) && (toVectorA.back().mX == (2 + 3 + 4)) &&
					  (TestObject::sTOCtorCount == 1));
		EATEST_VERIFY(ref.mX == (2 + 3 + 4));

		toVectorA.emplace(toVectorA.begin(), 3, 4, 5);
		EATEST_VERIFY((toVectorA.size() == 2) && (toVectorA.front().mX == (3 + 4 + 5)) &&
					  (TestObject::sTOCtorCount == 3));  // 3 because the original count of 1, plus the existing vector
														 // element will be moved, plus the one being emplaced.

		TestObject::Reset();

		// void push_back(T&& x);
		// iterator insert(const_iterator position, T&& x);

		vector<TestObject> toVectorC;

		toVectorC.push_back(TestObject(2, 3, 4));
		EATEST_VERIFY((toVectorC.size() == 1) && (toVectorC.back().mX == (2 + 3 + 4)) &&
					  (TestObject::sTOMoveCtorCount == 1));

		toVectorC.insert(toVectorC.begin(), TestObject(3, 4, 5));
		EATEST_VERIFY((toVectorC.size() == 2) && (toVectorC.front().mX == (3 + 4 + 5)) &&
					  (TestObject::sTOMoveCtorCount == 3));  // 3 because the original count of 1, plus the existing
															 // vector element will be moved, plus the one being
															 // emplaced.
	}

	// We don't check for TestObject::IsClear because we messed with state above and don't currently have a matching set
	// of ctors and dtors.
	TestObject::Reset();

	{
		using namespace eastl;

		// iterator erase(iterator position);
		// iterator erase(iterator first, iterator last);
		// iterator erase_unsorted(iterator position);
		// iterator erase_first(const T& pos);
		// iterator erase_first_unsorted(const T& pos);
		// iterator erase_last(const T& pos);
		// iterator erase_last_unsorted(const T& pos);
		// void     clear();

		vector<int> intArray(20);
		for (i = 0; i < 20; i++)
			intArray[i] = (int)i;

		// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19

		intArray.erase(intArray.begin() +
					   10);  // Becomes: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 19);
		EATEST_VERIFY(intArray[0] == 0);
		EATEST_VERIFY(intArray[10] == 11);
		EATEST_VERIFY(intArray[18] == 19);

		intArray.erase(intArray.begin() + 10,
					   intArray.begin() + 15);  // Becomes: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 16, 17, 18, 19
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 14);
		EATEST_VERIFY(intArray[9] == 9);
		EATEST_VERIFY(intArray[13] == 19);

		intArray.erase(intArray.begin() + 1, intArray.begin() + 5);  // Becomes: 0, 5, 6, 7, 8, 9, 16, 17, 18, 19
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 10);
		EATEST_VERIFY(intArray[0] == 0);
		EATEST_VERIFY(intArray[1] == 5);
		EATEST_VERIFY(intArray[9] == 19);

		intArray.erase(intArray.begin() + 7, intArray.begin() + 10);  // Becomes: 0, 5, 6, 7, 8, 9, 16
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 7);
		EATEST_VERIFY(intArray[0] == 0);
		EATEST_VERIFY(intArray[1] == 5);
		EATEST_VERIFY(intArray[6] == 16);

		intArray.clear();
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.empty());
		EATEST_VERIFY(intArray.size() == 0);

		vector<TestObject> toArray(20);
		for (i = 0; i < 20; i++)
			toArray[i] = TestObject((int)i);

		toArray.erase(toArray.begin() + 10);
		EATEST_VERIFY(toArray.validate());
		EATEST_VERIFY(toArray.size() == 19);
		EATEST_VERIFY(toArray[10] == TestObject(11));

		toArray.erase(toArray.begin() + 10, toArray.begin() + 15);
		EATEST_VERIFY(toArray.validate());
		EATEST_VERIFY(toArray.size() == 14);
		EATEST_VERIFY(toArray[10] == TestObject(16));

		toArray.clear();
		EATEST_VERIFY(toArray.validate());
		EATEST_VERIFY(toArray.empty());
		EATEST_VERIFY(toArray.size() == 0);

		// iterator erase_unsorted(iterator position);
		intArray.resize(20);
		for (i = 0; i < 20; i++)
			intArray[i] = (int)i;

		intArray.erase_unsorted(intArray.begin() + 0);
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 19);
		EATEST_VERIFY(intArray[0] == 19);
		EATEST_VERIFY(intArray[1] == 1);
		EATEST_VERIFY(intArray[18] == 18);

		intArray.erase_unsorted(intArray.begin() + 10);
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 18);
		EATEST_VERIFY(intArray[0] == 19);
		EATEST_VERIFY(intArray[10] == 18);
		EATEST_VERIFY(intArray[17] == 17);

		intArray.erase_unsorted(intArray.begin() + 17);
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 17);
		EATEST_VERIFY(intArray[0] == 19);
		EATEST_VERIFY(intArray[10] == 18);
		EATEST_VERIFY(intArray[16] == 16);

		// iterator erase_first(iterator position);
		intArray.resize(20);
		for (i = 0; i < 20; i++)
			intArray[i] = (int)i % 3; // (i.e. 0,1,2,0,1,2...)

		intArray.erase_first(1);
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 19);
		EATEST_VERIFY(intArray[0] == 0);
		EATEST_VERIFY(intArray[1] == 2);
		EATEST_VERIFY(intArray[2] == 0);
		EATEST_VERIFY(intArray[3] == 1);
		EATEST_VERIFY(intArray[18] == 1);

		intArray.erase_first(1);
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 18);
		EATEST_VERIFY(intArray[0] == 0);
		EATEST_VERIFY(intArray[1] == 2);
		EATEST_VERIFY(intArray[2] == 0);
		EATEST_VERIFY(intArray[3] == 2);
		EATEST_VERIFY(intArray[17] == 1);

		intArray.erase_first(0);
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 17);
		EATEST_VERIFY(intArray[0] == 2);
		EATEST_VERIFY(intArray[1] == 0);
		EATEST_VERIFY(intArray[2] == 2);
		EATEST_VERIFY(intArray[3] == 0);
		EATEST_VERIFY(intArray[16] == 1);

		// iterator erase_first_unsorted(const T& val);
		intArray.resize(20);
		for (i = 0; i < 20; i++)
			intArray[i] = (int) i/2; // every two values are the same (i.e. 0,0,1,1,2,2,3,3...)

		intArray.erase_first_unsorted(1);
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 19);
		EATEST_VERIFY(intArray[0] == 0);
		EATEST_VERIFY(intArray[1] == 0);
		EATEST_VERIFY(intArray[2] == 9);
		EATEST_VERIFY(intArray[3] == 1);
		EATEST_VERIFY(intArray[18] == 9);

		intArray.erase_first_unsorted(1);
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 18);
		EATEST_VERIFY(intArray[0] == 0);
		EATEST_VERIFY(intArray[1] == 0);
		EATEST_VERIFY(intArray[2] == 9);
		EATEST_VERIFY(intArray[3] == 9);
		EATEST_VERIFY(intArray[17] == 8);

		intArray.erase_first_unsorted(0);
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 17);
		EATEST_VERIFY(intArray[0] == 8);
		EATEST_VERIFY(intArray[1] == 0);
		EATEST_VERIFY(intArray[2] == 9);
		EATEST_VERIFY(intArray[3] == 9);
		EATEST_VERIFY(intArray[16] == 8);

		// iterator erase_last(const T& val);
		intArray.resize(20);
		for (i = 0; i < 20; i++)
			intArray[i] = (int)i % 3; // (i.e. 0,1,2,0,1,2...)

		intArray.erase_last(1);
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 19);
		EATEST_VERIFY(intArray[0] == 0);
		EATEST_VERIFY(intArray[1] == 1);
		EATEST_VERIFY(intArray[2] == 2);
		EATEST_VERIFY(intArray[3] == 0);
		EATEST_VERIFY(intArray[15] == 0);
		EATEST_VERIFY(intArray[16] == 1);
		EATEST_VERIFY(intArray[17] == 2);
		EATEST_VERIFY(intArray[18] == 0);

		intArray.erase_last(1);
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 18);
		EATEST_VERIFY(intArray[0] == 0);
		EATEST_VERIFY(intArray[1] == 1);
		EATEST_VERIFY(intArray[2] == 2);
		EATEST_VERIFY(intArray[3] == 0);
		EATEST_VERIFY(intArray[14] == 2);
		EATEST_VERIFY(intArray[15] == 0);
		EATEST_VERIFY(intArray[16] == 2);
		EATEST_VERIFY(intArray[17] == 0);

		intArray.erase_last(0);
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 17);
		EATEST_VERIFY(intArray[0] == 0);
		EATEST_VERIFY(intArray[1] == 1);
		EATEST_VERIFY(intArray[2] == 2);
		EATEST_VERIFY(intArray[3] == 0);
		EATEST_VERIFY(intArray[13] == 1);
		EATEST_VERIFY(intArray[14] == 2);
		EATEST_VERIFY(intArray[15] == 0);
		EATEST_VERIFY(intArray[16] == 2);

		// iterator erase_last_unsorted(const T& val);
		intArray.resize(20);
		for (i = 0; i < 20; i++)
			intArray[i] = (int)i / 2; // every two values are the same (i.e. 0,0,1,1,2,2,3,3...)

		intArray.erase_last_unsorted(1);
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 19);
		EATEST_VERIFY(intArray[0] == 0);
		EATEST_VERIFY(intArray[1] == 0);
		EATEST_VERIFY(intArray[2] == 1);
		EATEST_VERIFY(intArray[3] == 9);
		EATEST_VERIFY(intArray[18] == 9);

		intArray.erase_last_unsorted(1);
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 18);
		EATEST_VERIFY(intArray[0] == 0);
		EATEST_VERIFY(intArray[1] == 0);
		EATEST_VERIFY(intArray[2] == 9);
		EATEST_VERIFY(intArray[3] == 9);
		EATEST_VERIFY(intArray[17] == 8);

		intArray.erase_last_unsorted(0);
		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY(intArray.size() == 17);
		EATEST_VERIFY(intArray[0] == 0);
		EATEST_VERIFY(intArray[1] == 8);
		EATEST_VERIFY(intArray[2] == 9);
		EATEST_VERIFY(intArray[3] == 9);
		EATEST_VERIFY(intArray[16] == 8);
	}

	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();

	{
		using namespace eastl;

		// iterator erase(reverse_iterator position);
		// iterator erase(reverse_iterator first, reverse_iterator last);
		// iterator erase_unsorted(reverse_iterator position);

		vector<int> intVector;

		for (i = 0; i < 20; i++)
			intVector.push_back((int)i);
		EATEST_VERIFY((intVector.size() == 20) && (intVector[0] == 0) && (intVector[19] == 19));

		vector<int>::reverse_iterator r2A = intVector.rbegin();
		vector<int>::reverse_iterator r2B = r2A + 3;
		intVector.erase(r2A, r2B);
		EATEST_VERIFY((intVector.size() == 17));
		EATEST_VERIFY((intVector[0] == 0));
		EATEST_VERIFY((intVector[16] == 16));

		r2B = intVector.rend();
		r2A = r2B - 3;
		intVector.erase(r2A, r2B);
		EATEST_VERIFY((intVector.size() == 14));
		EATEST_VERIFY((intVector[0] == 3));
		EATEST_VERIFY((intVector[13] == 16));

		r2B = intVector.rend() - 1;
		intVector.erase(r2B);
		EATEST_VERIFY((intVector.size() == 13));
		EATEST_VERIFY((intVector[0] == 4));
		EATEST_VERIFY((intVector[12] == 16));

		r2B = intVector.rbegin();
		intVector.erase(r2B);
		EATEST_VERIFY((intVector.size() == 12));
		EATEST_VERIFY((intVector[0] == 4));
		EATEST_VERIFY((intVector[11] == 15));

		r2A = intVector.rbegin();
		r2B = intVector.rend();
		intVector.erase(r2A, r2B);
		EATEST_VERIFY(intVector.size() == 0);

		// iterator erase_unsorted(iterator position);
		intVector.resize(20);
		for (i = 0; i < 20; i++)
			intVector[i] = (int)i;

		intVector.erase_unsorted(intVector.rbegin() + 0);
		EATEST_VERIFY(intVector.validate());
		EATEST_VERIFY(intVector.size() == 19);
		EATEST_VERIFY(intVector[0] == 0);
		EATEST_VERIFY(intVector[10] == 10);
		EATEST_VERIFY(intVector[18] == 18);

		intVector.erase_unsorted(intVector.rbegin() + 10);
		EATEST_VERIFY(intVector.validate());
		EATEST_VERIFY(intVector.size() == 18);
		EATEST_VERIFY(intVector[0] == 0);
		EATEST_VERIFY(intVector[8] == 18);
		EATEST_VERIFY(intVector[17] == 17);

		intVector.erase_unsorted(intVector.rbegin() + 17);
		EATEST_VERIFY(intVector.validate());
		EATEST_VERIFY(intVector.size() == 17);
		EATEST_VERIFY(intVector[0] == 17);
		EATEST_VERIFY(intVector[8] == 18);
		EATEST_VERIFY(intVector[16] == 16);
	}

	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();

	{
		const int valueToRemove = 44;
		int testValues[] = {42, 43, 44, 45, 46, 47};

		eastl::vector<eastl::unique_ptr<int>> v; 
		
		for(auto& te : testValues)
			v.push_back(eastl::make_unique<int>(te));

		// remove 'valueToRemove' from the container
		auto iterToRemove = eastl::find_if(v.begin(), v.end(), [&](eastl::unique_ptr<int>& e)
		                                   { return *e == valueToRemove; });
		v.erase_unsorted(iterToRemove); 
		EATEST_VERIFY(v.size() == 5);

		// verify 'valueToRemove' is no longer in the container
		EATEST_VERIFY(eastl::find_if(v.begin(), v.end(), [&](eastl::unique_ptr<int>& e)
		                             { return *e == valueToRemove; }) == v.end());

		// verify all other expected values are in the container
		for (auto& te : testValues)
		{
			if (te == valueToRemove)
				continue;

			EATEST_VERIFY(eastl::find_if(v.begin(), v.end(), [&](eastl::unique_ptr<int>& e)
			                             { return *e == te; }) != v.end());
		}
	}

	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();

	{
		using namespace eastl;

		// iterator insert(iterator position, const value_type& value);
		// iterator insert(iterator position, size_type n, const value_type& value);
		// iterator insert(iterator position, InputIterator first, InputIterator last);
		// iterator insert(const_iterator position, std::initializer_list<T> ilist);

		vector<int> v(7, 13);
		EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "vector", 13, 13, 13, 13, 13, 13, 13, -1));

		// insert at end of size and capacity.
		v.insert(v.end(), 99);
		EATEST_VERIFY(v.validate());
		EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "vector.insert", 13, 13, 13, 13, 13, 13, 13, 99, -1));

		// insert at end of size.
		v.reserve(30);
		v.insert(v.end(), 999);
		EATEST_VERIFY(v.validate());
		EATEST_VERIFY(
			VerifySequence(v.begin(), v.end(), int(), "vector.insert", 13, 13, 13, 13, 13, 13, 13, 99, 999, -1));

		// Insert in middle.
		vector<int>::iterator it = v.begin() + 7;
		it = v.insert(it, 49);
		EATEST_VERIFY(v.validate());
		EATEST_VERIFY(
			VerifySequence(v.begin(), v.end(), int(), "vector.insert", 13, 13, 13, 13, 13, 13, 13, 49, 99, 999, -1));

		// Insert multiple copies
		it = v.insert(v.begin() + 5, 3, 42);
        EATEST_VERIFY(it == v.begin() + 5);
		EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "vector.insert", 13, 13, 13, 13, 13, 42, 42, 42, 13, 13,
									 49, 99, 999, -1));

        // Insert multiple copies with count == 0
        vector<int>::iterator at = v.end();
        it = v.insert(at, 0, 666);
        EATEST_VERIFY(it == at);
        EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "vector.insert", 13, 13, 13, 13, 13, 42, 42, 42, 13, 13,
                                     49, 99, 999, -1));
		// Insert iterator range
		const int data[] = {2, 3, 4, 5};
		it = v.insert(v.begin() + 1, data, data + 4);
        EATEST_VERIFY(it == v.begin() + 1);
		EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "vector.insert", 13, 2, 3, 4, 5, 13, 13, 13, 13, 42, 42,
									 42, 13, 13, 49, 99, 999, -1));

        // Insert empty iterator range
        at = v.begin() + 1;
		it = v.insert(at, data + 4, data + 4);
        EATEST_VERIFY(it == at);
		EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "vector.insert", 13, 2, 3, 4, 5, 13, 13, 13, 13, 42, 42,
									 42, 13, 13, 49, 99, 999, -1));

		// Insert with reallocation
		it = v.insert(v.end() - 3, 6, 17);
        EATEST_VERIFY(it == v.end() - (3 + 6));
		EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "vector.insert", 13, 2, 3, 4, 5, 13, 13, 13, 13, 42, 42,
									 42, 13, 13, 17, 17, 17, 17, 17, 17, 49, 99, 999, -1));

		// Single insert with reallocation
		vector<int> v2;
		v2.reserve(100);
		v2.insert(v2.begin(), 100, 17);
		EATEST_VERIFY(v2.size() == 100);
		EATEST_VERIFY(v2[0] == 17);
		v2.insert(v2.begin() + 50, 42);
		EATEST_VERIFY(v2.size() == 101);
		EATEST_VERIFY(v2[50] == 42);

		// Test insertion of values that come from within the vector.
		v.insert(v.end() - 3, v.end() - 5, v.end());
		EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "vector.insert", 13, 2, 3, 4, 5, 13, 13, 13, 13, 42, 42,
									 42, 13, 13, 17, 17, 17, 17, 17, 17, 17, 17, 49, 99, 999, 49, 99, 999, -1));

		v.insert(v.end() - 3, v.back());
		EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "vector.insert", 13, 2, 3, 4, 5, 13, 13, 13, 13, 42, 42,
									 42, 13, 13, 17, 17, 17, 17, 17, 17, 17, 17, 49, 99, 999, 999, 49, 99, 999, -1));

		v.insert(v.end() - 3, 2, v[v.size() - 3]);
		EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "vector.insert", 13, 2, 3, 4, 5, 13, 13, 13, 13, 42, 42,
									 42, 13, 13, 17, 17, 17, 17, 17, 17, 17, 17, 49, 99, 999, 999, 49, 49, 49, 99, 999,
									 -1));

#if !defined(EASTL_STD_ITERATOR_CATEGORY_ENABLED) && !defined(EA_COMPILER_NO_STANDARD_CPP_LIBRARY)
		// std::vector / eastl::vector
		std::vector<TestObject> stdV(10);
		eastl::vector<TestObject> eastlV(10);

		eastlV.insert(eastlV.end(), stdV.begin(), stdV.end());
		stdV.insert(stdV.end(), eastlV.begin(), eastlV.end());

		EATEST_VERIFY(eastlV.size() == 20);
		EATEST_VERIFY(stdV.size() == 30);

		// std::string / eastl::vector
		std::string stdString("blah");
		eastl::vector<char8_t> eastlVString;

		eastlVString.assign(stdString.begin(), stdString.end());
#endif

// iterator insert(const_iterator position, std::initializer_list<T> ilist);
#if !defined(EA_COMPILER_NO_INITIALIZER_LISTS)
		// iterator insert(const_iterator position, std::initializer_list<T> ilist);
		eastl::vector<float> floatVector;

		floatVector.insert(floatVector.end(), {0, 1, 2, 3});
		EATEST_VERIFY(floatVector.size() == 4);
		EATEST_VERIFY((floatVector[0] == 0) && (floatVector[3] == 3));
#endif
	}

	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();

	{
		// Test insert move objects
		eastl::vector<TestObject> toVector1;
		toVector1.reserve(20);
		for(int idx = 0; idx < 2; ++idx)
			toVector1.push_back(TestObject(idx));

		eastl::vector<TestObject> toVector2;
		for(int idx = 0; idx < 3; ++idx)
			toVector2.push_back(TestObject(10 + idx));

		// Insert more objects than the existing number using insert with iterator
		TestObject::Reset();
        eastl::vector<TestObject>::iterator it;
		it = toVector1.insert(toVector1.begin(), toVector2.begin(), toVector2.end());
        EATEST_VERIFY(it == toVector1.begin());
		EATEST_VERIFY(VerifySequence(toVector1.begin(), toVector1.end(), int(), "vector.insert", 10, 11, 12, 0, 1, -1));
		EATEST_VERIFY(TestObject::sTOMoveCtorCount + TestObject::sTOMoveAssignCount == 2 &&
					  TestObject::sTOCopyCtorCount + TestObject::sTOCopyAssignCount == 3); // Move 2 existing elements and copy the 3 inserted

		eastl::vector<TestObject> toVector3;
		toVector3.push_back(TestObject(20));

		// Insert less objects than the existing number using insert with iterator
		TestObject::Reset();
		it = toVector1.insert(toVector1.begin(), toVector3.begin(), toVector3.end());
		EATEST_VERIFY(VerifySequence(toVector1.begin(), toVector1.end(), int(), "vector.insert", 20, 10, 11, 12, 0, 1, -1));
        EATEST_VERIFY(it == toVector1.begin());
		EATEST_VERIFY(TestObject::sTOMoveCtorCount + TestObject::sTOMoveAssignCount == 5 &&
					  TestObject::sTOCopyCtorCount + TestObject::sTOCopyAssignCount == 1); // Move 5 existing elements and copy the 1 inserted

		// Insert more objects than the existing number using insert without iterator
		TestObject::Reset();
		it = toVector1.insert(toVector1.begin(), 1, TestObject(17));
        EATEST_VERIFY(it == toVector1.begin());
		EATEST_VERIFY(VerifySequence(toVector1.begin(), toVector1.end(), int(), "vector.insert", 17, 20, 10, 11, 12, 0, 1, -1));
		EATEST_VERIFY(TestObject::sTOMoveCtorCount + TestObject::sTOMoveAssignCount == 6 &&
					  TestObject::sTOCopyCtorCount + TestObject::sTOCopyAssignCount == 2); // Move 6 existing element and copy the 1 inserted +
																						   // the temporary one inside the function

		// Insert less objects than the existing number using insert without iterator
		TestObject::Reset();
		it = toVector1.insert(toVector1.begin(), 10, TestObject(18));
        EATEST_VERIFY(it == toVector1.begin());
		EATEST_VERIFY(VerifySequence(toVector1.begin(), toVector1.end(), int(), "vector.insert", 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 17, 20, 10, 11, 12, 0, 1, -1));
		EATEST_VERIFY(TestObject::sTOMoveCtorCount + TestObject::sTOMoveAssignCount == 7 &&
					  TestObject::sTOCopyCtorCount + TestObject::sTOCopyAssignCount == 11); // Move 7 existing element and copy the 10 inserted +
																							// the temporary one inside the function
	}

	TestObject::Reset();

	{
		using namespace eastl;

		// reserve / resize / capacity / clear
		vector<int> v(10, 17);
		v.reserve(20);
		EATEST_VERIFY(v.validate());
		EATEST_VERIFY(v.size() == 10);
		EATEST_VERIFY(v.capacity() == 20);

		v.resize(7);  // Shrink
		EATEST_VERIFY(v.validate());
		EATEST_VERIFY(v.capacity() == 20);

		v.resize(17);  // Grow without reallocation
		EATEST_VERIFY(v.validate());
		EATEST_VERIFY(v.capacity() == 20);

		v.resize(42);  // Grow with reallocation
		vector<int>::size_type c = v.capacity();
		EATEST_VERIFY(v.validate());
		EATEST_VERIFY(v[41] == 0);
		EATEST_VERIFY(c >= 42);

		v.resize(44, 19);  // Grow with reallocation
		EATEST_VERIFY(v.validate());
		EATEST_VERIFY(v[43] == 19);

		c = v.capacity();
		v.clear();
		EATEST_VERIFY(v.validate());
		EATEST_VERIFY(v.empty());
		EATEST_VERIFY(v.capacity() == c);

		// How to shrink a vector's capacity to be equal to its size.
		vector<int>(v).swap(v);
		EATEST_VERIFY(v.validate());
		EATEST_VERIFY(v.empty());
		EATEST_VERIFY(v.capacity() == v.size());

		// How to completely clear a vector (size = 0, capacity = 0, no allocation).
		vector<int>().swap(v);
		EATEST_VERIFY(v.validate());
		EATEST_VERIFY(v.empty());
		EATEST_VERIFY(v.capacity() == 0);
	}

	{  // set_capacity / reset
		using namespace eastl;

		const int intArray[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
		const size_t kIntArraySize = sizeof(intArray) / sizeof(int);

		vector<int> v(30);
		EATEST_VERIFY(v.capacity() >= 30);

		v.assign(intArray, intArray + kIntArraySize);
		EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "vector.assign", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
									 13, 14, 15, 16, 17, -1));

		// set_capacity
		v.set_capacity();
		EATEST_VERIFY(v.capacity() == v.size());
		EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "vector.set_capacity", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
									 11, 12, 13, 14, 15, 16, 17, -1));

		v.set_capacity(0);
		EATEST_VERIFY(v.size() == 0);
		EATEST_VERIFY(v.data() == NULL);
		EATEST_VERIFY(v.capacity() == v.size());

		// Test set_capacity doing a realloc of non-scalar class types.
		eastl::vector<TestObject> toArray;
		toArray.resize(16);
		toArray.set_capacity(64);
		EATEST_VERIFY(v.validate());

		// reset_lose_memory
		int* const pData = v.data();
		vector<int>::size_type n = v.size();
		vector<int>::allocator_type& allocator = v.get_allocator();
		v.reset_lose_memory();
		allocator.deallocate(pData, n);
		EATEST_VERIFY(v.capacity() == 0);
		EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "vector.reset", -1));

		// Test set_capacity make a move when reducing size
		vector<TestObject> toArray2(10, TestObject(7));
		TestObject::Reset();
		toArray2.set_capacity(5);
		EATEST_VERIFY(TestObject::sTOMoveCtorCount == 5 &&
					  TestObject::sTOCopyCtorCount + TestObject::sTOCopyAssignCount == 0); // Move the 5 existing elements, no copy
		EATEST_VERIFY(VerifySequence(toArray2.begin(), toArray2.end(), int(), "vector.set_capacity", 7, 7, 7, 7, 7, -1));
	}

	TestObject::Reset();

	{
		using namespace eastl;

		// Regression for user-reported possible bug.
		{
			MallocAllocator::reset_all();

			eastl::vector<int, MallocAllocator> v;
			v.reserve(32);  // does allocation

			v.push_back(37);  // may reallocate if we do enough of these to exceed 32
			v.erase(v.begin());

			v.set_capacity(0);

			// Verify that all memory is freed by the set_capacity function.
			EATEST_VERIFY((MallocAllocator::mAllocCountAll > 0) &&
						  (MallocAllocator::mAllocCountAll == MallocAllocator::mFreeCountAll));

			MallocAllocator::reset_all();
		}

		{
			MallocAllocator::reset_all();

			eastl::vector<int, MallocAllocator> v;
			v.reserve(32);  // does allocation

			for (int j = 0; j < 40; j++)
				v.push_back(37);  // may reallocate if we do enough of these to exceed 32
			for (int k = 0; k < 40; k++)
				v.erase(v.begin());

			v.set_capacity(0);

			// Verify that all memory is freed by the set_capacity function.
			EATEST_VERIFY((MallocAllocator::mAllocCountAll > 0) &&
						  (MallocAllocator::mAllocCountAll == MallocAllocator::mFreeCountAll));

			MallocAllocator::reset_all();
		}
	}

	{
		using namespace eastl;

		// bool validate() const;
		// bool validate_iterator(const_iterator i) const;

		vector<int> intArray(20);

		EATEST_VERIFY(intArray.validate());
		EATEST_VERIFY((intArray.validate_iterator(intArray.begin()) & (isf_valid | isf_can_dereference)) != 0);
		EATEST_VERIFY(intArray.validate_iterator(NULL) == isf_none);
	}

	{
		using namespace eastl;

		// global operators (==, !=, <, etc.)
		vector<int> intArray1(10);
		vector<int> intArray2(10);

		for (i = 0; i < intArray1.size(); i++)
		{
			intArray1[i] = (int)i;  // Make intArray1 equal to intArray2.
			intArray2[i] = (int)i;
		}

		EATEST_VERIFY((intArray1 == intArray2));
		EATEST_VERIFY(!(intArray1 != intArray2));
		EATEST_VERIFY((intArray1 <= intArray2));
		EATEST_VERIFY((intArray1 >= intArray2));
		EATEST_VERIFY(!(intArray1 < intArray2));
		EATEST_VERIFY(!(intArray1 > intArray2));

		intArray1.push_back(100);  // Make intArray1 less than intArray2.
		intArray2.push_back(101);

		EATEST_VERIFY(!(intArray1 == intArray2));
		EATEST_VERIFY((intArray1 != intArray2));
		EATEST_VERIFY((intArray1 <= intArray2));
		EATEST_VERIFY(!(intArray1 >= intArray2));
		EATEST_VERIFY((intArray1 < intArray2));
		EATEST_VERIFY(!(intArray1 > intArray2));
	}

	// three way comparison operator
#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	{
		using namespace eastl;

		vector<int> intArray1(10);
		vector<int> intArray2(10);

		for (i = 0; i < intArray1.size(); i++)
		{
		    intArray1[i] = (int)i; // Make intArray1 equal to intArray2.
		    intArray2[i] = (int)i;
		}

		// Verify equality between intArray1 and intArray2
		EATEST_VERIFY((intArray1 <=> intArray2) == 0);
		EATEST_VERIFY(!((intArray1 <=> intArray2) != 0));
		EATEST_VERIFY((intArray1 <=> intArray2) <= 0);
		EATEST_VERIFY((intArray1 <=> intArray2) >= 0);
		EATEST_VERIFY(!((intArray1 <=> intArray2) < 0));
		EATEST_VERIFY(!((intArray1 <=> intArray2) > 0));

		intArray1.push_back(100); // Make intArray1 less than intArray2.
		intArray2.push_back(101);

		// Verify intArray1 < intArray2
		EATEST_VERIFY(!((intArray1 <=> intArray2) == 0));
		EATEST_VERIFY((intArray1 <=> intArray2) != 0);
		EATEST_VERIFY((intArray1 <=> intArray2) <= 0);
		EATEST_VERIFY(!((intArray1 <=> intArray2) >= 0));
		EATEST_VERIFY(((intArray1 <=> intArray2) < 0));
		EATEST_VERIFY(!((intArray1 <=> intArray2) > 0));

		for (i = 0; i < 3; i++) // Make the length of intArray2 less than intArray1
			intArray2.pop_back();

		// Verify intArray2.size() < intArray1.size() and intArray2 is a subset of intArray1
		EATEST_VERIFY(!((intArray1 <=> intArray2) == 0));
		EATEST_VERIFY((intArray1 <=> intArray2) != 0);
		EATEST_VERIFY((intArray1 <=> intArray2) >= 0);
		EATEST_VERIFY(!((intArray1 <=> intArray2) <= 0));
		EATEST_VERIFY(((intArray1 <=> intArray2) > 0));
		EATEST_VERIFY(!((intArray1 <=> intArray2) < 0));
	}

	{
		using namespace eastl;

		vector<int> intArray1 = {1, 2, 3, 4, 5, 6, 7};
		vector<int> intArray2 = {7, 6, 5, 4, 3, 2, 1};
		vector<int> intArray3 = {1, 2, 3, 4};

		struct weak_ordering_vector
		{
		    vector<int> vec;
		    inline std::weak_ordering operator<=>(const weak_ordering_vector& b) const { return vec <=> b.vec; }
		};

		EATEST_VERIFY(synth_three_way{}(weak_ordering_vector{intArray1}, weak_ordering_vector{intArray2}) == std::weak_ordering::less);
		EATEST_VERIFY(synth_three_way{}(weak_ordering_vector{intArray3}, weak_ordering_vector{intArray1}) == std::weak_ordering::less);
		EATEST_VERIFY(synth_three_way{}(weak_ordering_vector{intArray2}, weak_ordering_vector{intArray1}) == std::weak_ordering::greater);
		EATEST_VERIFY(synth_three_way{}(weak_ordering_vector{intArray2}, weak_ordering_vector{intArray3}) == std::weak_ordering::greater);
		EATEST_VERIFY(synth_three_way{}(weak_ordering_vector{intArray1}, weak_ordering_vector{intArray1}) == std::weak_ordering::equivalent);

		struct strong_ordering_vector
		{
		    vector<int> vec;
		    inline std::strong_ordering operator<=>(const strong_ordering_vector& b) const { return vec <=> b.vec; }
		};

		EATEST_VERIFY(synth_three_way{}(strong_ordering_vector{intArray1}, strong_ordering_vector{intArray2}) == std::strong_ordering::less);
		EATEST_VERIFY(synth_three_way{}(strong_ordering_vector{intArray3}, strong_ordering_vector{intArray1}) == std::strong_ordering::less);
		EATEST_VERIFY(synth_three_way{}(strong_ordering_vector{intArray2}, strong_ordering_vector{intArray1}) == std::strong_ordering::greater);
		EATEST_VERIFY(synth_three_way{}(strong_ordering_vector{intArray2}, strong_ordering_vector{intArray3}) == std::strong_ordering::greater);
		EATEST_VERIFY(synth_three_way{}(strong_ordering_vector{intArray1}, strong_ordering_vector{intArray1}) == std::strong_ordering::equal);
	}
#endif

	{
		using namespace eastl;

		// Test vector<Align64>

		// Aligned objects should be CustomAllocator instead of the default, because the
		// EASTL default might be unable to do aligned allocations, but CustomAllocator always can.
		vector<Align64, CustomAllocator> vA64(10);

		vA64.resize(2);
		EATEST_VERIFY(vA64.size() == 2);

		vA64.push_back(Align64());
		EATEST_VERIFY(vA64.size() == 3);

		vA64.resize(0);
		EATEST_VERIFY(vA64.size() == 0);

		vA64.insert(vA64.begin(), Align64());
		EATEST_VERIFY(vA64.size() == 1);

		vA64.resize(20);
		EATEST_VERIFY(vA64.size() == 20);
	}

	{
		// Misc additional tests

		eastl::vector<int> empty1;
		EATEST_VERIFY(empty1.data() == NULL);
		EATEST_VERIFY(empty1.size() == 0);
		EATEST_VERIFY(empty1.capacity() == 0);

		eastl::vector<int> empty2 = empty1;
		EATEST_VERIFY(empty2.data() == NULL);
		EATEST_VERIFY(empty2.size() == 0);
		EATEST_VERIFY(empty2.capacity() == 0);
	}

	{  // Test whose purpose is to see if calling vector::size() in a const loop results in the compiler optimizing the
		// size() call to outside the loop.
		eastl::vector<TestObject> toArray;

		toArray.resize(7);

		for (i = 0; i < toArray.size(); i++)
		{
			TestObject& to = toArray[i];

			if (to.mX == 99999)
				to.mX++;
		}
	}

	{  // Test assign from iterator type.
		TestObject to;
		eastl::vector<TestObject> toTest;

		// InputIterator
		demoted_iterator<TestObject*, EASTL_ITC_NS::forward_iterator_tag> toInput(&to);
		toTest.assign(toInput, toInput);

		// ForwardIterator
		eastl::slist<TestObject> toSList;
		toTest.assign(toSList.begin(), toSList.end());

		// BidirectionalIterator
		eastl::list<TestObject> toList;
		toTest.assign(toList.begin(), toList.end());

		// RandomAccessIterator
		eastl::deque<TestObject> toDeque;
		toTest.assign(toDeque.begin(), toDeque.end());

		// ContiguousIterator    (note: as of this writing, vector doesn't actually use contiguous_iterator_tag)
		eastl::vector<TestObject> toArray;
		toTest.assign(toArray.begin(), toArray.end());
	}

	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();

	{  // Test user report that they think they saw code like this leak memory.
		eastl::vector<int> intTest;

		intTest.push_back(1);
		intTest = eastl::vector<int>();

		eastl::vector<TestObject> toTest;

		toTest.push_back(TestObject(1));
		toTest = eastl::vector<TestObject>();
	}

	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();

	{  // Regression of user error report for the case of vector<const type>.
		eastl::vector<int> ctorValues;

		for (int v = 0; v < 10; v++)
			ctorValues.push_back(v);

		eastl::vector<const ConstType> testStruct(ctorValues.begin(), ctorValues.end());
		eastl::vector<const int> testInt(ctorValues.begin(), ctorValues.end());
	}

	{  // Regression to verify that const vector works.
		const eastl::vector<int> constIntVector1;
		EATEST_VERIFY(constIntVector1.empty());

		int intArray[3] = {37, 38, 39};
		const eastl::vector<int> constIntVector2(intArray, intArray + 3);
		EATEST_VERIFY(constIntVector2.size() == 3);

		const eastl::vector<int> constIntVector3(4, 37);
		EATEST_VERIFY(constIntVector3.size() == 4);

		const eastl::vector<int> constIntVector4;
		const eastl::vector<int> constIntVector5 = constIntVector4;
	}

	{  // Regression to verify that a bug fix for a vector optimization works.
		eastl::vector<int> intVector1;
		intVector1.reserve(128);
		intVector1.resize(128, 37);
		intVector1.push_back(intVector1.front());
		EATEST_VERIFY(intVector1.back() == 37);

		eastl::vector<int> intVector2;
		intVector2.reserve(1024);
		intVector2.resize(1024, 37);
		intVector2.resize(2048, intVector2.front());
		EATEST_VERIFY(intVector2.back() == 37);
	}

	{  // C++11 Range
// EABase 2.00.34+ has EA_COMPILER_NO_RANGE_BASED_FOR_LOOP, which we can check instead.
#if (defined(_MSC_VER) && (EA_COMPILER_VERSION >= 1700)) ||                                        \
	(defined(__clang__) && (EA_COMPILER_VERSION >= 300) && (__cplusplus >= 201103L)) ||            \
	(defined(__GNUC__) && (EA_COMPILER_VERSION >= 4006) && defined(__GXX_EXPERIMENTAL_CXX0X__)) || \
	(__cplusplus >= 201103L)

		eastl::vector<float> floatVector;

		floatVector.push_back(0.0);
		floatVector.push_back(1.0);

		for (auto& f : floatVector)
			f += 1.0;

		EATEST_VERIFY(floatVector.back() == 2.0);
#endif
	}

	{
// C++11 cbegin, cend, crbegin, crend
#if !defined(EA_COMPILER_NO_AUTO)
		// float vector
		eastl::vector<float> floatVector;

		auto cb = floatVector.cbegin();
		auto ce = floatVector.cend();
		auto crb = floatVector.crbegin();
		auto cre = floatVector.crend();

		EATEST_VERIFY(eastl::distance(cb, ce) == 0);
		EATEST_VERIFY(eastl::distance(crb, cre) == 0);

		// const float vector
		const eastl::vector<float> cFloatVector;

		auto ccb = cFloatVector.cbegin();
		auto cce = cFloatVector.cend();
		auto ccrb = cFloatVector.crbegin();
		auto ccre = cFloatVector.crend();

		EATEST_VERIFY(eastl::distance(ccb, cce) == 0);
		EATEST_VERIFY(eastl::distance(ccrb, ccre) == 0);

#endif
	}

	{
		// Regression for failure in DoRealloc's use of uninitialize_move.
		using namespace eastl;

		const eastl::string str0 = "TestString0";
		vector<eastl::string> v(1, str0);
		vector<eastl::string> v_copy;

		// Test operator=
		v_copy = v;
		EATEST_VERIFY_MSG(v_copy.size() == 1, "vector string8 copy size");
		EATEST_VERIFY_MSG(eastl::find(v_copy.begin(), v_copy.end(), str0) != v_copy.end(), "vector copy string8");
		EATEST_VERIFY_MSG(v.size() == 1, "vector string8 copy size");
		EATEST_VERIFY_MSG(eastl::find(v.begin(), v.end(), str0) != v.end(), "vector copy string8");

		// Test assign.
		v.clear();
		v.push_back(str0);
		v_copy.assign(v.begin(), v.end());
		EATEST_VERIFY_MSG(v_copy.size() == 1, "vector string8 copy size");
		EATEST_VERIFY_MSG(eastl::find(v_copy.begin(), v_copy.end(), str0) != v_copy.end(), "vector copy string8");
		EATEST_VERIFY_MSG(v.size() == 1, "vector string8 copy size");
		EATEST_VERIFY_MSG(eastl::find(v.begin(), v.end(), str0) != v.end(), "vector copy string8");
	}

	{
		// Regression of vector::operator= for the case of EASTL_ALLOCATOR_COPY_ENABLED=1
		// For this test we need to use InstanceAllocator to create two containers of the same
		// type but with different and unequal allocator instances. The bug was that when
		// EASTL_ALLOCATOR_COPY_ENABLED was enabled operator=(this_type& x) assigned x.mAllocator
		// to this and then proceeded to assign member elements from x to this. That's invalid
		// because the existing elements of this were allocated by a different allocator and
		// will be freed in the future with the allocator copied from x.
		// The test below should work for the case of EASTL_ALLOCATOR_COPY_ENABLED == 0 or 1.
		InstanceAllocator::reset_all();

		InstanceAllocator ia0((uint8_t)0);
		InstanceAllocator ia1((uint8_t)1);

		eastl::vector<int, InstanceAllocator> v0((eastl_size_t)1, (int)0, ia0);
		eastl::vector<int, InstanceAllocator> v1((eastl_size_t)1, (int)1, ia1);

		EATEST_VERIFY((v0.front() == 0) && (v1.front() == 1));
#if EASTL_ALLOCATOR_COPY_ENABLED
		EATEST_VERIFY(v0.get_allocator() != v1.get_allocator());
#endif
		v0 = v1;
		EATEST_VERIFY((v0.front() == 1) && (v1.front() == 1));
		EATEST_VERIFY(InstanceAllocator::mMismatchCount == 0);
		EATEST_VERIFY(v0.validate());
		EATEST_VERIFY(v1.validate());
#if EASTL_ALLOCATOR_COPY_ENABLED
		EATEST_VERIFY(v0.get_allocator() == v1.get_allocator());
#endif
	}

	{
		// Test shrink_to_fit
		eastl::vector<int> v;
		EATEST_VERIFY(v.capacity() == 0);
		v.resize(100);
		EATEST_VERIFY(v.capacity() == 100);
		v.clear();
		EATEST_VERIFY(v.capacity() == 100);
		v.shrink_to_fit();
		EATEST_VERIFY(v.capacity() == 0);
	}

	{
		// Regression for compilation errors found and fixed when integrating into Frostbite.
		int j = 7;

		eastl::vector<StructWithConstInt> v1;
		v1.push_back(StructWithConstInt(j));

		eastl::vector<StructWithConstRefToInt> v2;
		v2.push_back(StructWithConstRefToInt(j));
	}

	{
		// Regression for issue with vector containing non-copyable values reported by user
		eastl::vector<testmovable> moveablevec;
		testmovable moveable;
		moveablevec.insert(moveablevec.end(), eastl::move(moveable));
	}

	{
		// Calling erase of empty range should not call a move assignment to self
		eastl::vector<TestMoveAssignToSelf> v1;
		v1.push_back(TestMoveAssignToSelf());
		EATEST_VERIFY(!v1[0].mMovedToSelf);
		v1.erase(v1.begin(), v1.begin());
		EATEST_VERIFY(!v1[0].mMovedToSelf);
	}

#if defined(EASTL_TEST_CONCEPT_IMPLS)
	{
		// vector default constructor should require no more than Destructible
		eastl::vector<Destructible> v1;
		EATEST_VERIFY(v1.empty());

		// some basic vector operations (data(), capacity(), size(), empty(), clear(), erase()) should impose no
		// requirements beyond Destructible
		EATEST_VERIFY(v1.empty());
		EATEST_VERIFY(v1.size() == 0);
		EATEST_VERIFY(v1.capacity() == 0);
		EATEST_VERIFY(eastl::distance(v1.data(), v1.data() + v1.size()) == 0);
		v1.clear();
	}

	{
		// vector default constructor should work with DefaultConstructible T
		eastl::vector<DefaultConstructible> v1;
		EATEST_VERIFY(v1.empty());
	}

	{
		// vector constructor that takes an initial size should only require DefaultConstructible T
		eastl::vector<DefaultConstructible> v2(2);
		EATEST_VERIFY(v2.size() == 2 && v2[0].value == v2[1].value &&
					  v2[0].value == DefaultConstructible::defaultValue);
	}

	{
		// vector constructor taking an initial size and a value should only require CopyConstructible
		eastl::vector<CopyConstructible> v3(2, CopyConstructible::Create());
		EATEST_VERIFY(v3.size() == 2 && v3[0].value == v3[1].value && v3[0].value == CopyConstructible::defaultValue);

		// vector constructor taking a pair of iterators should work for CopyConstructible
		eastl::vector<CopyConstructible> v4(cbegin(v3), cend(v3));
		EATEST_VERIFY(v4.size() == 2 && v4[0].value == v4[1].value && v4[0].value == CopyConstructible::defaultValue);
	}

	{
		// vector::reserve() should only require MoveInsertible
		eastl::vector<MoveConstructible> v5;
		v5.reserve(2);
		v5.push_back(MoveConstructible::Create());
		v5.push_back(MoveConstructible::Create());
		EATEST_VERIFY(v5.size() == 2 && v5[0].value == v5[1].value && v5[0].value == MoveConstructible::defaultValue);
		v5.pop_back();

		// vector::shrink_to_fit() should only require MoveInsertible
		v5.shrink_to_fit();
		EATEST_VERIFY(v5.size() == 1 && v5.capacity() == 1 && v5[0].value == MoveConstructible::defaultValue);
	}

	{
		// vector constructor taking a pair of iterators should only require MoveConstructible
		MoveConstructible moveConstructibleArray[] = {MoveConstructible::Create()};
		eastl::vector<MoveConstructible> v7(
			eastl::move_iterator<MoveConstructible*>(eastl::begin(moveConstructibleArray)),
			eastl::move_iterator<MoveConstructible*>(eastl::end(moveConstructibleArray)));
		EATEST_VERIFY(v7.size() == 1 && v7[0].value == MoveConstructible::defaultValue);
	}

	{
		// vector::swap() should only require Destructible. We also test with DefaultConstructible as it gives us a
		// testable result.

		eastl::vector<Destructible> v4, v5;
		eastl::swap(v4, v5);
		EATEST_VERIFY(v4.empty() && v5.empty());

		eastl::vector<DefaultConstructible> v6(1), v7(2);
		eastl::swap(v6, v7);
		EATEST_VERIFY(v6.size() == 2 && v7.size() == 1);
	}

	{
		// vector::resize() should only require MoveInsertable and DefaultInsertable
		eastl::vector<MoveAndDefaultConstructible> v8;
		v8.resize(2);
		EATEST_VERIFY(v8.size() == 2 && v8[0].value == v8[1].value && v8[0].value ==
		MoveAndDefaultConstructible::defaultValue);
	}

	{
		eastl::vector<MoveAssignable> v1;
		// vector::insert(pos, rv) should only require MoveAssignable
		v1.insert(begin(v1), MoveAssignable::Create());
		EATEST_VERIFY(v1.size() == 1 && v1.front().value == MoveAssignable::defaultValue);
		// vector::erase(pos) should only require MoveAssignable
		v1.erase(begin(v1));
		EATEST_VERIFY(v1.empty());
	}
#endif // EASTL_TEST_CONCEPT_IMPLS

	{
		// validates our vector implementation does not use 'operator<' on input iterators during vector construction.
		//
		struct container_value_type { int data; };
		struct container_with_custom_iterator
		{
			struct iterator
			{
				typedef EASTL_ITC_NS::input_iterator_tag iterator_category;
				typedef int value_type;
				typedef ptrdiff_t difference_type;
				typedef int* pointer;
				typedef int& reference;

				bool operator!=(const iterator&) const { return false; }
				iterator& operator++()                 { return *this; }
				iterator operator++(int)               { return *this; }
				container_value_type operator*()       { return {};    }
			};

			container_with_custom_iterator() EA_NOEXCEPT {}

			iterator begin() const { return {}; }
			iterator end() const   { return {}; }
			bool empty() const     { return false; }

		private:
			eastl::vector<container_value_type> m_vector;
		};

		static_assert(!is_less_comparable<container_with_custom_iterator::iterator>::value, "type cannot support comparison by '<' for this test");
		container_with_custom_iterator ci;
		eastl::vector<container_value_type> v2(ci.begin(), ci.end()); 
	}

	// If the legacy code path is enabled we cannot handle non-copyable types
	#ifndef EASTL_VECTOR_LEGACY_SWAP_BEHAVIOUR_REQUIRES_COPY_CTOR 
		// unique_ptr tests
		{
			// Simple move-assignment test to prevent regressions where eastl::vector utilizes operations on T that are not necessary.
			{
				eastl::vector<eastl::unique_ptr<int>> v1;
				eastl::vector<eastl::unique_ptr<int>> v2;
				v2 = eastl::move(v1);
			}

			{
				// This test verifies that eastl::vector can handle the move-assignment case where its utilizes two
				// different allocator instances that do not compare equal.  An example of an allocator that compares equal
				// but isn't the same object instance is an allocator that shares the same memory allocation mechanism (eg.
				// malloc).  The memory allocated from one instance can be freed by another instance in the case where
				// allocators compare equal.  This test is verifying functionality in the opposite case where allocators
				// instances do not compare equal and must clean up its own allocated memory.
				InstanceAllocator::reset_all();
				{
					InstanceAllocator a1(uint8_t(0)), a2(uint8_t(1));
					eastl::vector<eastl::unique_ptr<int>, InstanceAllocator> v1(a1);
					eastl::vector<eastl::unique_ptr<int>, InstanceAllocator> v2(a2);

					VERIFY(v1.get_allocator() != v2.get_allocator());

					// add some data in the vector so we can move it to the other vector.
					v1.push_back(nullptr);
					v1.push_back(nullptr);
					v1.push_back(nullptr);
					v1.push_back(nullptr);

					VERIFY(!v1.empty() && v2.empty());
					v2 = eastl::move(v1);
					VERIFY(v1.empty() && !v2.empty());
					v1.swap(v2); 
					VERIFY(!v1.empty() && v2.empty());
				}
				VERIFY(InstanceAllocator::mMismatchCount == 0);
			}
		}
	#endif

	{
		// CustomAllocator has no data members which reduces the size of an eastl::vector via the empty base class optimization.
		typedef eastl::vector<int, CustomAllocator> EboVector;
		static_assert(sizeof(EboVector) == 3 * sizeof(void*), "");
	}

	// eastl::erase / eastl::erase_if tests
	{
		{
			eastl::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9};

			auto numErased = eastl::erase(v, 5);
			VERIFY((v == eastl::vector<int> {1, 2, 3, 4, 6, 7, 8, 9}));
			VERIFY(numErased == 1);

			numErased = eastl::erase(v, 2);
			VERIFY((v == eastl::vector<int> {1, 3, 4, 6, 7, 8, 9}));
			VERIFY(numErased == 1);

			numErased = eastl::erase(v, 9);
			VERIFY((v == eastl::vector<int> {1, 3, 4, 6, 7, 8}));
			VERIFY(numErased == 1);
		}

		{
			eastl::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9};
			auto numErased = eastl::erase_if(v, [](auto i) { return i % 2 == 0; });
			VERIFY((v == eastl::vector<int>{1, 3, 5, 7, 9}));
			VERIFY(numErased == 4);
		}
	}

	return nErrorCount;
}
