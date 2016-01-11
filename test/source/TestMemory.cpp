/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/memory.h>
#include <EASTL/utility.h>
#include <EASTL/vector.h>
#include <EAStdC/EAMemory.h>
#include <EAStdC/EAAlignment.h>


// Regression for user reported operator new problem (12/8/2009):
class AssetHandler
{
public:
	inline static void* operator new(size_t size, const char* /*text*/, unsigned int /*flags*/)
	{
		return ::operator new(size);
	}
	inline static void operator delete(void* p)
	{
		return ::operator delete(p);
	}
};
typedef eastl::vector<AssetHandler> AssetHandlerArray;



// LCTestObject
//
// Helps test the late_constructed utility.
// Has an unusual alignment so we can test that aspect of late_constructed.
//
struct EA_ALIGN(64) LCTestObject
{
	int             mX;                  // 
	static int64_t  sTOCount;            // Count of all current existing objects.
	static int64_t  sTOCtorCount;        // Count of times any ctor was called.
	static int64_t  sTODtorCount;        // Count of times dtor was called.

	explicit LCTestObject(int x = 0)
		: mX(x)
	{
		++sTOCount;
		++sTOCtorCount;
	}

	LCTestObject(int x0, int x1, int x2)
		: mX(x0 + x1 + x2)
	{
		++sTOCount;
		++sTOCtorCount;
	}

	LCTestObject(const LCTestObject& testObject)
		: mX(testObject.mX)
	{
		++sTOCount;
		++sTOCtorCount;
	}

	#if !defined(EA_COMPILER_NO_RVALUE_REFERENCES)
		LCTestObject(TestObject&& testObject)
			: mX(testObject.mX)
		{
			++sTOCount;
			++sTOCtorCount;
		}
	#endif

	LCTestObject& operator=(const LCTestObject& testObject)
	{
		mX = testObject.mX;
		return *this;
	}

	#if !defined(EA_COMPILER_NO_RVALUE_REFERENCES)
		LCTestObject& operator=(LCTestObject&& testObject)
		{
			eastl::swap(mX, testObject.mX);
			return *this;
		}
	#endif

	~LCTestObject()
	{
		--sTOCount;
		++sTODtorCount;
	}
};

int64_t LCTestObject::sTOCount     = 0;
int64_t LCTestObject::sTOCtorCount = 0;
int64_t LCTestObject::sTODtorCount = 0;


eastl::late_constructed<LCTestObject, true>  gLCTestObjectTrue;
eastl::late_constructed<LCTestObject, false> gLCTestObjectFalse;


///////////////////////////////////////////////////////////////////////////////
// TestMemory
//
int TestMemory()
{
	using namespace eastl;

	int nErrorCount = 0;

	TestObject::Reset();

	{
		// get_temporary_buffer(ptrdiff_t n, size_t alignment, size_t alignmentOffset, char* pName);

		pair<int*, ptrdiff_t> pr1 = get_temporary_buffer<int>(100, 1, 0, EASTL_NAME_VAL("Temp int array"));
		memset(pr1.first, 0, 100 * sizeof(int));
		return_temporary_buffer(pr1.first);

		// Note that 
		pair<TestObject*, ptrdiff_t> pr2 = get_temporary_buffer<TestObject>(300);
		memset(pr2.first, 0, 300 * sizeof(TestObject));
		return_temporary_buffer(pr2.first, pr2.second);
	}

	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();


	{
		LCTestObject* pLCTO;

		// Verify alignment requirements.
		// We don't verify that gLCTestObjectTrue.get() is aligned for all platforms because some platforms can't do that with global memory.
		static_assert(eastl::alignment_of<typename late_constructed<LCTestObject>::value_type>::value == 64, "late_constructed alignment failure.");
		static_assert(eastl::alignment_of<typename late_constructed<LCTestObject>::storage_type>::value == 64, "late_constructed alignment failure.");
		static_assert(eastl::alignment_of<late_constructed<LCTestObject> >::value >= 64, "late_constructed alignment failure.");


		// late_constructed / gLCTestObjectTrue 
		EATEST_VERIFY((LCTestObject::sTOCount == 0) && (LCTestObject::sTOCtorCount == 0) && (LCTestObject::sTODtorCount == 0));
		EATEST_VERIFY(!gLCTestObjectTrue.is_constructed());

		pLCTO = gLCTestObjectTrue.get(); // This will auto-construct LCTestObject.
		EATEST_VERIFY(pLCTO != NULL);
		EATEST_VERIFY(gLCTestObjectTrue.is_constructed());
		EATEST_VERIFY((LCTestObject::sTOCount == 1) && (LCTestObject::sTOCtorCount == 1) && (LCTestObject::sTODtorCount == 0));

		gLCTestObjectTrue->mX = 17;
		EATEST_VERIFY(gLCTestObjectTrue->mX == 17);
		EATEST_VERIFY((LCTestObject::sTOCount == 1) && (LCTestObject::sTOCtorCount == 1) && (LCTestObject::sTODtorCount == 0));

		gLCTestObjectTrue.destruct();
		EATEST_VERIFY((LCTestObject::sTOCount == 0) && (LCTestObject::sTOCtorCount == 1) && (LCTestObject::sTODtorCount == 1));
		EATEST_VERIFY(!gLCTestObjectTrue.is_constructed());

		gLCTestObjectTrue->mX = 18;
		EATEST_VERIFY(gLCTestObjectTrue->mX == 18);
		EATEST_VERIFY(gLCTestObjectTrue.is_constructed());
		EATEST_VERIFY((LCTestObject::sTOCount == 1) && (LCTestObject::sTOCtorCount == 2) && (LCTestObject::sTODtorCount == 1));

		gLCTestObjectTrue.destruct();
		(*gLCTestObjectTrue).mX = 19;
		EATEST_VERIFY(gLCTestObjectTrue->mX == 19);
		EATEST_VERIFY((LCTestObject::sTOCount == 1) && (LCTestObject::sTOCtorCount == 3) && (LCTestObject::sTODtorCount == 2));

		gLCTestObjectTrue.destruct();
		LCTestObject::sTOCount     = 0;
		LCTestObject::sTOCtorCount = 0;
		LCTestObject::sTODtorCount = 0;

		// late_constructed / gLCTestObjectFalse 
		EATEST_VERIFY((LCTestObject::sTOCount == 0) && (LCTestObject::sTOCtorCount == 0) && (LCTestObject::sTODtorCount == 0));
		EATEST_VERIFY(!gLCTestObjectFalse.is_constructed());

		pLCTO = gLCTestObjectFalse.get(); // This will not auto-construct LCTestObject.
		EATEST_VERIFY(pLCTO == NULL);
		EATEST_VERIFY(!gLCTestObjectFalse.is_constructed());
		EATEST_VERIFY((LCTestObject::sTOCount == 0) && (LCTestObject::sTOCtorCount == 0) && (LCTestObject::sTODtorCount == 0));

		gLCTestObjectFalse.construct();
		pLCTO = gLCTestObjectFalse.get();
		EATEST_VERIFY(pLCTO != NULL);
		EATEST_VERIFY(gLCTestObjectFalse.is_constructed());
		EATEST_VERIFY((LCTestObject::sTOCount == 1) && (LCTestObject::sTOCtorCount == 1) && (LCTestObject::sTODtorCount == 0));

		gLCTestObjectFalse->mX = 17;
		EATEST_VERIFY(gLCTestObjectFalse->mX == 17);
		EATEST_VERIFY((LCTestObject::sTOCount == 1) && (LCTestObject::sTOCtorCount == 1) && (LCTestObject::sTODtorCount == 0));

		gLCTestObjectFalse.destruct();
		EATEST_VERIFY((LCTestObject::sTOCount == 0) && (LCTestObject::sTOCtorCount == 1) && (LCTestObject::sTODtorCount == 1));
		EATEST_VERIFY(!gLCTestObjectFalse.is_constructed());

		gLCTestObjectFalse.construct(14);
		EATEST_VERIFY(gLCTestObjectFalse->mX == 14);
		gLCTestObjectFalse->mX = 18;
		EATEST_VERIFY(gLCTestObjectFalse->mX == 18);
		EATEST_VERIFY(gLCTestObjectFalse.is_constructed());
		EATEST_VERIFY((LCTestObject::sTOCount == 1) && (LCTestObject::sTOCtorCount == 2) && (LCTestObject::sTODtorCount == 1));

		gLCTestObjectFalse.destruct();
		gLCTestObjectFalse.construct(10, 20, 30);
		EATEST_VERIFY(gLCTestObjectFalse->mX == 10+20+30);
		(*gLCTestObjectFalse).mX = 19;
		EATEST_VERIFY(gLCTestObjectFalse->mX == 19);
		EATEST_VERIFY((LCTestObject::sTOCount == 1) && (LCTestObject::sTOCtorCount == 3) && (LCTestObject::sTODtorCount == 2));

		gLCTestObjectFalse.destruct();
	}


	// We use the vector container to supply a RandomAccessIterator.
	// We use the list container to supply a BidirectionalIterator.
	// We use the slist container to supply a ForwardIterator.
	// We use our generic_input_iterator adapter to supply an InputIterator.

	// eastl::vector<int>  intVector;
	// eastl::list<int>    intList;
	// eastl::slist<int>   intSlist;

	// template <typename ForwardIterator, typename ForwardIteratorDest>
	// inline ForwardIteratorDest uninitialized_relocate_start(ForwardIterator first, ForwardIterator last, ForwardIteratorDest dest)

	// template <typename ForwardIterator, typename ForwardIteratorDest>
	// inline ForwardIteratorDest uninitialized_relocate_commit(ForwardIterator first, ForwardIterator last, ForwardIteratorDest dest)

	// template <typename ForwardIterator, typename ForwardIteratorDest>
	// inline ForwardIteratorDest uninitialized_relocate_abort(ForwardIterator first, ForwardIterator last, ForwardIteratorDest dest)

	// template <typename ForwardIterator, typename ForwardIteratorDest>
	// inline ForwardIteratorDest uninitialized_relocate(ForwardIterator first, ForwardIterator last, ForwardIteratorDest dest)

	// This test does little more than verify that the code compiles.
	int* pEnd = eastl::uninitialized_relocate_start<int*, int*>((int*)NULL, (int*)NULL, (int*)NULL);
	EATEST_VERIFY(pEnd == NULL);

	pEnd = eastl::uninitialized_relocate_commit<int*, int*>((int*)NULL, (int*)NULL, (int*)NULL);
	EATEST_VERIFY(pEnd == NULL);

	pEnd = eastl::uninitialized_relocate_abort<int*, int*>((int*)NULL, (int*)NULL, (int*)NULL);
	EATEST_VERIFY(pEnd == NULL);

	pEnd = eastl::uninitialized_relocate<int*, int*>((int*)NULL, (int*)NULL, (int*)NULL);
	EATEST_VERIFY(pEnd == NULL);



	// template <typename InputIterator, typename ForwardIterator>
	// ForwardIterator uninitialized_copy(InputIterator sourceFirst, InputIterator sourceLast, ForwardIterator destination);

	pEnd = eastl::uninitialized_copy<int*, int*>((int*)NULL, (int*)NULL, (int*)NULL);
	EATEST_VERIFY(pEnd == NULL);



	// template <typename First, typename Last, typename Result>
	// Result uninitialized_copy_ptr(First first, Last last, Result result)

	pEnd = eastl::uninitialized_copy_ptr<int*, int*, int*>((int*)NULL, (int*)NULL, (int*)NULL);
	EATEST_VERIFY(pEnd == NULL);



	// template <typename ForwardIterator, typename T>
	// void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value)

	eastl::uninitialized_fill<int*, int>((int*)NULL, (int*)NULL, (int)0);



	// template <typename T>
	// void uninitialized_fill_ptr(T* first, T* last, const T& value)

	eastl::uninitialized_fill_ptr<int>((int*)NULL, (int*)NULL, (int)0);



	// template <typename ForwardIterator, typename Count, typename T>
	// void uninitialized_fill_n(ForwardIterator first, Count n, const T& value)

	eastl::uninitialized_fill_n<int*, int, int>((int*)NULL, (int)0, (int)0);



	// template <typename T, typename Count>
	// void uninitialized_fill_n_ptr(T* first, Count n, const T& value)

	eastl::uninitialized_fill_n_ptr<int, int>((int*)NULL, (int)0, (int)0);




	// template <typename InputIterator, typename ForwardIterator, typename T>
	// void uninitialized_copy_fill(InputIterator first1, InputIterator last1,
	//                                 ForwardIterator first2, ForwardIterator last2, const T& value)

	eastl::uninitialized_copy_fill<int*, int*, int>((int*)NULL, (int*)NULL, (int*)NULL, (int*)NULL, (int)0);



	// template <typename ForwardIterator, typename T, typename InputIterator>
	// ForwardIterator uninitialized_fill_copy(ForwardIterator result, ForwardIterator mid, const T& value, InputIterator first, InputIterator last)

	eastl::uninitialized_fill_copy<int*, int, int*>((int*)NULL, (int*)NULL, (int)0, (int*)NULL, (int*)NULL);



	// template <typename InputIterator1, typename InputIterator2, typename ForwardIterator>
	// ForwardIterator uninitialized_copy_copy(InputIterator1 first1, InputIterator1 last1,
	//                                         InputIterator2 first2, InputIterator2 last2,
	//                                         ForwardIterator result)

	eastl::uninitialized_copy_copy<int*, int*, int*>((int*)NULL, (int*)NULL, (int*)NULL, (int*)NULL, (int*)NULL);



	// template <typename T>
	// void destruct(T* p)

	uint64_t testObjectMemory[((sizeof(TestObject) / sizeof(uint64_t)) + 1) * 2];
	TestObject* pTestObject = new(testObjectMemory) TestObject;
	destruct(pTestObject);



	// template <typename ForwardIterator>
	// void destruct(ForwardIterator first, ForwardIterator last)

	pTestObject = new(testObjectMemory) TestObject[2];
	destruct(pTestObject, pTestObject + 1);


	{
		// Regression for user reported operator new problem (12/8/2009):
		eastl::vector<AssetHandler> ahArray;
		ahArray.push_back(AssetHandler());
	}


	// void* align(size_t alignment, size_t size, void*& ptr, size_t& space);
	// void* align_advance(size_t alignment, size_t size, void*& ptr, size_t& space);
	{
		const  size_t kBufferSize = 256;
		char   buffer[kBufferSize * 2];
		size_t space = sizeof(buffer);
		void*  ptr = buffer;
		void*  ptrSaved;
		void*  ptrAligned;
		size_t i;

		// First get 256 bytes of space aligned to 256.
		// It's a coincidence that we are using eastl::align to set up a buffer for testing eastl::align below.
		ptrSaved = eastl::align(256, 256, ptr, space);

		// At this point we have 256 bytes of memory aligned on 256 bytes, within buffer.
		// We test allocating multiple blocks from this space at various alignments values.
		// We also test that the function sets ptr to the next available location after the 
		// returned allocated block.
		EA::StdC::Memset8(buffer, 0x00, sizeof(buffer));
		EATEST_VERIFY(EA::StdC::IsAligned(ptr, 256));

		// align test
		// Try a number of allocation sizes.
		for(size_t a = 1; a < 64; a *= 2)
		{
			// Do multiple sequental allocations from the storage.
			for(i = 0, space = 256, ptr = ptrSaved; i < kBufferSize; i += a)
			{
				ptrAligned = eastl::align(a, a, ptr, space);

				EATEST_VERIFY((uintptr_t)ptrAligned == ((uintptr_t)ptrSaved + i));
				EATEST_VERIFY(ptr                   == ptrAligned);
				EATEST_VERIFY(space                 == (kBufferSize - i));
				EATEST_VERIFY(EA::StdC::IsAligned(ptrAligned, a));
				EATEST_VERIFY(EA::StdC::Memcheck8(ptrAligned, 0x00, a) == NULL);

				ptr    = (char*)ptr + a;
				space -= a;
				memset(ptrAligned, 0xff, a); // Do this so that next time around we can verify this memory isn't returned.
			}

			EA::StdC::Memset8(buffer, 0x00, sizeof(buffer));
		}

		// align_advance test (similar to but not identical to the align test)
		// Try a number of allocation sizes.
		for(size_t a = 1; a < 64; a *= 2)
		{
			// Do multiple sequental allocations from the storage.
			for(i = 0, space = 256, ptr = ptrSaved; i < kBufferSize; i += a)
			{
				ptrAligned = eastl::align_advance(a, a, ptr, space, &ptr, &space);

				EATEST_VERIFY((uintptr_t)ptrAligned == ((uintptr_t)ptrSaved + i));
				EATEST_VERIFY((uintptr_t)ptr        == (uintptr_t)ptrAligned + a);
				EATEST_VERIFY(space                 == (kBufferSize - i) - a);
				EATEST_VERIFY(EA::StdC::IsAligned(ptrAligned, a));
				EATEST_VERIFY(EA::StdC::Memcheck8(ptrAligned, 0x00, a) == NULL);

				memset(ptrAligned, 0xff, a); // Do this so that next time around we can verify this memory isn't returned.
			}

			EA::StdC::Memset8(buffer, 0x00, sizeof(buffer));
		}
	}

	{
		// Test that align handles integral overflow correctly and returns NULL.
		void*  ptr;
		void*  ptrSaved;
		size_t space;
		void*  pResult;

		space    = 64;
		ptr      = 0;
		ptr      = (char*)ptr - space;
		ptrSaved = ptr;
		pResult  = eastl::align(1, space + 1, ptr, space);             // Possible alignment, impossible size due to wraparound.
		EATEST_VERIFY((pResult == NULL) && (ptr == ptrSaved));

		space    = 64;
		ptr      = 0;
		ptr      = (char*)ptr - space;
		ptrSaved = ptr;
		pResult  = eastl::align(space * 2, 32, ptr, space);            // Impossible alignment due to wraparound, possible size.
		EATEST_VERIFY((pResult == NULL) && (ptr == ptrSaved));
	}

	EATEST_VERIFY(nErrorCount == 0);

	return nErrorCount;
}











