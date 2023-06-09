/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/fixed_vector.h>
#include <EASTL/unique_ptr.h>
#include <EAStdC/EAMemory.h>
#include <new>

#if defined(EA_COMPILER_CPP17_ENABLED) && __has_include(<variant>)
#include <variant> //Variant not present in older standards
#endif


using namespace eastl;


// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::fixed_vector<int,        1,  true>;
template class eastl::fixed_vector<Align64,    1,  true>;
template class eastl::fixed_vector<TestObject, 1,  true>;

template class eastl::fixed_vector<int,        1, false>;
template class eastl::fixed_vector<Align64,    1, false>;
template class eastl::fixed_vector<TestObject, 1, false>;

/*
// This does not compile, since the fixed_vector allocator is templated on sizeof(T), 
// not just T. Thus, the full type is required at the time of instantiation, but it
// is not available.
// See EATech Core JIRA issue ETCR-1608 for more information.
struct StructWithContainerOfStructs
{
	eastl::fixed_vector<StructWithContainerOfStructs,4> children;
};
*/


namespace
{
	// Aligned objects should be CustomAllocator instead of the default, because the 
	// EASTL default might be unable to do aligned allocations, but CustomAllocator always can.
	typedef fixed_vector<Align64, 3, true, CustomAllocator> VA64;

	VA64      vA64;
	Align64   a64(5);
	Align64*  pA64 = &a64;
}


int TestFixedVector()
{
	int nErrorCount = 0;

	TestObject::Reset();

	{ // Test the aligned_buffer template
		{
			eastl::aligned_buffer<sizeof(TestObject), EASTL_ALIGN_OF(TestObject)> toAlignedBuffer;
			TestObject* const pTO = new(toAlignedBuffer.buffer) TestObject;
			#if !defined(__GNUC__) // GCC complains about strict aliasing here.
				EATEST_VERIFY(pTO->mX == ((TestObject*)&toAlignedBuffer.buffer[0])->mX);
			#endif
			pTO->~TestObject();
		}

		{
			eastl::aligned_buffer<sizeof(Align64), EASTL_ALIGN_OF(Align64)> a64AlignedBuffer;
			Align64* const pAlign64 = new(a64AlignedBuffer.buffer) Align64;
			#if !defined(__GNUC__) // GCC complains about strict aliasing here.
				EATEST_VERIFY(pAlign64->mX == ((Align64*)&a64AlignedBuffer.buffer[0])->mX);
			#endif
			pAlign64->~Align64();
		}
	}

	{
		// fixed_vector();
		// size_type max_size() const;
		fixed_vector<int, 1, true> v;
		EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "fixed_vector", -1));
		EATEST_VERIFY(v.max_size() == 1);

		// fixed_vector();
		typedef fixed_vector<int, 8, false> FixedVectorInt8;
		FixedVectorInt8 fv1;
		EATEST_VERIFY(fv1.size() == 0);
		EATEST_VERIFY(fv1.capacity() == 8);

		// this_type& operator=(const base_type& x);
		FixedVectorInt8 fv2 = fv1;
		EATEST_VERIFY(fv2.size() == 0);
		EATEST_VERIFY(fv2.capacity() == 8);

		// fixed_vector(const base_type& x);
		FixedVectorInt8 fv3(fv1);
		EATEST_VERIFY(fv3.size() == 0);
		EATEST_VERIFY(fv3.capacity() == 8);

		// explicit fixed_vector(size_type n);
		FixedVectorInt8 fv4(5);
		EATEST_VERIFY(fv4.size() == 5);
		EATEST_VERIFY(fv4.capacity() == 8);
		EATEST_VERIFY((fv4[0] == 0) && (fv4[4] == 0));

		// fixed_vector(size_type n, const value_type& value);
		FixedVectorInt8 fv5((eastl_size_t)5, (int)3);
		EATEST_VERIFY(fv5.size() == 5);
		EATEST_VERIFY(fv5.capacity() == 8);
		EATEST_VERIFY((fv5[0] == 3) && (fv5[4] == 3));

		// fixed_vector(InputIterator first, InputIterator last);
		const int intArray[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
		FixedVectorInt8 fv6(intArray, intArray + 8);
		EATEST_VERIFY(fv6.size() == 8);
		EATEST_VERIFY(fv5.capacity() == 8);
		EATEST_VERIFY((fv6[0] == 0) && (fv6[7] == 7));

		// void reset_lose_memory();
		fv6.reset_lose_memory();
		EATEST_VERIFY(fv6.size() == 0);
		EATEST_VERIFY(fv6.capacity() == 8);

		// void set_capacity(size_type);
		fv6.set_capacity(100);  // overflow is disabled, so this should have no effect.
		EATEST_VERIFY(fv6.size() == 0);
		EATEST_VERIFY(fv6.capacity() == 8); // EATEST_VERIFY that the capacity is unchanged.

		fv6.resize(8);
		EATEST_VERIFY(fv6.size() == 8);
		fv6.set_capacity(1);
		EATEST_VERIFY(fv6.size() == 1);
		EATEST_VERIFY(fv6.capacity() == 8);

		// Exercise the freeing of memory in set_capacity.
		fixed_vector<int, 8, true> fv88;
		eastl_size_t capacity = fv88.capacity();
		fv88.resize(capacity);
		fv88.set_capacity(capacity * 2);
		EATEST_VERIFY(fv88.capacity() >= (capacity * 2));

		// void swap(this_type& x);
		// FixedVectorInt8 fv7(5, 3);  // MSVC-ARM64 generated an internal compiler error on this line.
		FixedVectorInt8 fv7 = {3, 3, 3, 3, 3};
		FixedVectorInt8 fv8(intArray, intArray + 8);

		swap(fv7, fv8);
		EATEST_VERIFY(fv7.size() == 8);
		EATEST_VERIFY((fv7[0] == 0) && (fv7[7] == 7));
		EATEST_VERIFY(fv8.size() == 5);
		EATEST_VERIFY((fv8[0] == 3) && (fv8[4] == 3));

		fv7.swap(fv8);
		EATEST_VERIFY(fv8.size() == 8);
		EATEST_VERIFY((fv8[0] == 0) && (fv8[7] == 7));
		EATEST_VERIFY(fv7.size() == 5);
		EATEST_VERIFY((fv7[0] == 3) && (fv7[4] == 3));

		// Test a recent optimization we added, which was to do a pointer swap of the fixed_vector pointers
		// for the case that both fixed_vectors were overflowed and using the heap instead of their fixed buffers.
		fixed_vector<int8_t, 4, true> fvo5;
		fixed_vector<int8_t, 4, true> fvo6;
		fvo5.resize(5, 5);
		EATEST_VERIFY(fvo5.has_overflowed());
		fvo6.resize(6, 6);
		EATEST_VERIFY(fvo6.has_overflowed());
		fvo5.swap(fvo6);
		EATEST_VERIFY(fvo5.size() == 6); // Verify that sizes are swapped.
		EATEST_VERIFY(fvo6.size() == 5);
		EATEST_VERIFY(EA::StdC::Memcheck8(fvo5.data(), 6, fvo5.size()) == NULL); // Verify that contents are swapped.
		EATEST_VERIFY(EA::StdC::Memcheck8(fvo6.data(), 5, fvo6.size()) == NULL);

		// global operators
		EATEST_VERIFY(  fv7 != fv8);
		EATEST_VERIFY(!(fv7 == fv8));
		fv7 = fv8;
		EATEST_VERIFY(  fv7 == fv8);
		EATEST_VERIFY(!(fv7 != fv8));
		EATEST_VERIFY(fv7.validate());
		EATEST_VERIFY(fv8.validate());
	}


	{
		// POD types
		typedef fixed_vector<int, 1, true> vInt;

		vInt    v;
		int     n = 5;
		int*    pN = &n;

		v.insert(v.begin(), pN, pN + 1);
		EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "fixed_vector", 5, -1));
		EATEST_VERIFY(v.validate());
	}


	{
		// non POD types
		typedef fixed_vector<TestObject, 1, true> VTO;

		VTO              v;
		TestObject       to(5);
		TestObject*      pTO = &to;

		v.insert(v.begin(), pTO, pTO + 1);
		EATEST_VERIFY(VerifySequence(v.begin(), v.end(), int(), "fixed_vector", 5, -1));
		EATEST_VERIFY(v.validate());
	}


	{
		// non POD types

		// The variables used here are declared above in the global space.
		vA64.insert(vA64.begin(), pA64, pA64 + 1);
		EATEST_VERIFY(VerifySequence(vA64.begin(), vA64.end(), int(), "fixed_vector", 5, -1));
		EATEST_VERIFY(((uintptr_t)&a64 % kEASTLTestAlign64) == 0);
		EATEST_VERIFY(((uintptr_t)vA64.data() % kEASTLTestAlign64) == 0);
		EATEST_VERIFY(((uintptr_t)&vA64[0] % kEASTLTestAlign64) == 0);
		EATEST_VERIFY(vA64.max_size() == 3);
		EATEST_VERIFY(vA64.validate());
	}


	{
		// Test for potential bug reported Sep. 19, 2006.
		typedef eastl::fixed_vector<void*, 160, false> FixedVector;
		FixedVector v;
		int arr[100] = {};
		int* p = arr;

		for(int i = 0; i < 100; i++, p++)
			v.push_back(p);

		EATEST_VERIFY(v.size() == 100);
		EATEST_VERIFY(eastl::unique(v.begin(), v.end()) == v.end());

		FixedVector::iterator it = eastl::lower_bound(v.begin(), v.end(), p - 30);
		EATEST_VERIFY(v.validate_iterator(it) == (isf_valid | isf_current | isf_can_dereference));
		EATEST_VERIFY((*it) == (p - 30));

		v.erase(it);

		EATEST_VERIFY(v.size() == 99);
		EATEST_VERIFY(eastl::unique(v.begin(), v.end()) == v.end());
	}

	{
		typedef fixed_vector<Align64, 4, true, CustomAllocator> FixedVectorWithAlignment;

		FixedVectorWithAlignment fv;

		Align64 a;

		fv.push_back(a);
		fv.push_back(a);
		fv.push_back(a);
		fv.push_back(a);
		fv.push_back(a);
		for (FixedVectorWithAlignment::const_iterator it = fv.begin(); it != fv.end(); ++it)
		{
			const Align64* ptr = &(*it);
			EATEST_VERIFY((uint64_t)ptr % EASTL_ALIGN_OF(Align64) == 0);
		}
	}

	{   // Test overflow allocator specification
		typedef fixed_vector<char8_t, 64, true, MallocAllocator> FixedString64Malloc;

		FixedString64Malloc fs;

		fs.push_back('a');
		EATEST_VERIFY(fs.size() == 1);
		EATEST_VERIFY(fs[0] == 'a');

		fs.resize(95);
		fs[94] = 'b';
		EATEST_VERIFY(fs[0] == 'a');
		EATEST_VERIFY(fs[94] == 'b');
		EATEST_VERIFY(fs.size() == 95);
		EATEST_VERIFY(fs.validate());

		fs.clear();
		EATEST_VERIFY(fs.empty());

		fs.push_back('a');
		EATEST_VERIFY(fs.size() == 1);
		EATEST_VERIFY(fs[0] == 'a');
		EATEST_VERIFY(fs.validate());

		fs.resize(195);
		fs[194] = 'b';
		EATEST_VERIFY(fs[0] == 'a');
		EATEST_VERIFY(fs[194] == 'b');
		EATEST_VERIFY(fs.size() == 195);
		EATEST_VERIFY(fs.validate());

		// get_overflow_allocator / set_overflow_allocator
		fs.set_capacity(0); // This should free all memory allocated by the existing (overflow) allocator.
		EATEST_VERIFY(fs.validate());
		MallocAllocator a;
		fs.get_allocator().set_overflow_allocator(a);
		EATEST_VERIFY(fs.validate());
		fs.resize(400);
		EATEST_VERIFY(fs.validate());
	}


	{
		//Test clear(bool freeOverflow)
		const size_t nodeCount = 4;
		typedef fixed_vector<int, nodeCount, true> vInt4;
		vInt4 fv;
		for (int i = 0; (unsigned)i < nodeCount+1; i++)
		{
			fv.push_back(i);
		}
		vInt4::size_type capacity = fv.capacity();
		EATEST_VERIFY(capacity >= nodeCount+1);
		fv.clear(false);
		EATEST_VERIFY(fv.size() == 0);
		EATEST_VERIFY(fv.capacity() == capacity);
		fv.push_back(1);
		fv.clear(true);
		EATEST_VERIFY(fv.size() == 0);
		EATEST_VERIFY(fv.capacity() == nodeCount);
	}


	{
		// bool empty() const
		// bool has_overflowed() const
		// size_type size() const;
		// size_type max_size() const

		// Test a vector that has overflow disabled.
		fixed_vector<int, 5, false> vInt5;

		EATEST_VERIFY(vInt5.max_size() == 5);
		EATEST_VERIFY(vInt5.size() == 0);
		EATEST_VERIFY(vInt5.empty());
		EATEST_VERIFY(!vInt5.has_overflowed());

		vInt5.push_back(37);
		vInt5.push_back(37);
		vInt5.push_back(37);

		EATEST_VERIFY(vInt5.size() == 3);
		EATEST_VERIFY(!vInt5.empty());
		EATEST_VERIFY(!vInt5.has_overflowed());

		vInt5.push_back(37);
		vInt5.push_back(37);

		EATEST_VERIFY(vInt5.size() == 5);
		EATEST_VERIFY(!vInt5.empty());
		EATEST_VERIFY(!vInt5.has_overflowed());

		vInt5.pop_back();

		EATEST_VERIFY(vInt5.size() == 4);
		EATEST_VERIFY(!vInt5.empty());
		EATEST_VERIFY(!vInt5.has_overflowed());
		EATEST_VERIFY(vInt5.validate());
	}


	{
		// bool empty() const
		// bool has_overflowed() const
		// size_type size() const;
		// size_type max_size() const

		// Test a list that has overflow enabled.
		fixed_vector<int, 5, true> vInt5;

		EATEST_VERIFY(vInt5.max_size() == 5);
		EATEST_VERIFY(vInt5.size() == 0);
		EATEST_VERIFY(vInt5.empty());
		EATEST_VERIFY(!vInt5.has_overflowed());

		vInt5.push_back(37);
		vInt5.push_back(37);
		vInt5.push_back(37);

		EATEST_VERIFY(vInt5.size() == 3);
		EATEST_VERIFY(!vInt5.empty());
		EATEST_VERIFY(!vInt5.has_overflowed());

		vInt5.push_back(37);
		vInt5.push_back(37);

		EATEST_VERIFY(vInt5.size() == 5);
		EATEST_VERIFY(!vInt5.empty());
		EATEST_VERIFY(!vInt5.has_overflowed());

		vInt5.push_back(37);

		EATEST_VERIFY(vInt5.size() == 6);
		EATEST_VERIFY(!vInt5.empty());
		EATEST_VERIFY(vInt5.has_overflowed());

		vInt5.clear();

		EATEST_VERIFY(vInt5.size() == 0);
		EATEST_VERIFY(vInt5.empty());
		EATEST_VERIFY(vInt5.has_overflowed());           // Note that we declare the container full, as it is no longer using the fixed-capacity.
		EATEST_VERIFY(vInt5.validate());
	}

	{
		// void* push_back_uninitialized();

		int64_t toCount0 = TestObject::sTOCount;

		eastl::fixed_vector<TestObject, 32, false> vTO1;         // <-- bEnableOverflow = false
		EATEST_VERIFY(TestObject::sTOCount == toCount0);

		for(int i = 0; i < 25; i++) // 25 is simply a number that is <= 32.
		{
			void* pTO1 = vTO1.push_back_uninitialized();
			EATEST_VERIFY(TestObject::sTOCount == (toCount0 + i));

			new(pTO1) TestObject(i);
			EATEST_VERIFY(TestObject::sTOCount == (toCount0 + i + 1));
			EATEST_VERIFY(vTO1.back().mX == i);
			EATEST_VERIFY(vTO1.validate());
		}
	}

	{
		// void* push_back_uninitialized();

		int64_t toCount0 = TestObject::sTOCount;

		eastl::fixed_vector<TestObject, 15, true> vTO2;         // <-- bEnableOverflow = true
		EATEST_VERIFY(TestObject::sTOCount == toCount0);

		for(int i = 0; i < 25; i++) // 25 is simply a number that is > 15.
		{
			void* pTO2 = vTO2.push_back_uninitialized();
			EATEST_VERIFY(TestObject::sTOCount == (toCount0 + i));

			new(pTO2) TestObject(i);
			EATEST_VERIFY(TestObject::sTOCount == (toCount0 + i + 1));
			EATEST_VERIFY(vTO2.back().mX == i);
			EATEST_VERIFY(vTO2.validate());
		}
	}

	{ // Try to repro user report that fixed_vector on the stack crashes.
		eastl::fixed_vector<int,        10, false> fvif;
		eastl::fixed_vector<int,        10, true>  fvit;
		eastl::fixed_vector<TestObject, 10, false> fvof;
		eastl::fixed_vector<TestObject, 10, true>  fvot;
		eastl::fixed_vector<int,        10, false, MallocAllocator> fvimf;
		eastl::fixed_vector<int,        10, true,  MallocAllocator> fvimt;
		eastl::fixed_vector<TestObject, 10, false, MallocAllocator> fvomf;
		eastl::fixed_vector<TestObject, 10, true,  MallocAllocator> fvomt;

		fvif.push_back(1);
		fvit.push_back(1);
		fvimf.push_back(1);
		fvimt.push_back(1);

		fvif.clear();
		fvit.clear();
		fvimf.clear();
		fvimt.clear();
	}

	{
		// Test construction of a container with an overflow allocator constructor argument.
		MallocAllocator overflowAllocator;
		void* p = overflowAllocator.allocate(1);
		fixed_vector<int, 64, true, MallocAllocator> c(overflowAllocator);
		c.resize(65);
		EATEST_VERIFY(c.get_overflow_allocator().mAllocCount == 2); // 1 for above, and 1 for overflowing from 64 to 65.
		overflowAllocator.deallocate(p, 1);
	}

	EATEST_VERIFY(TestObject::IsClear());
	TestObject::Reset();


	{   // Test for crash bug reported by Arpit Baldeva.
		eastl::fixed_vector<void*, 1, true> test;

		test.push_back(NULL);
		test.push_back(NULL);
		test.erase(eastl::find(test.begin(), test.end(), (void*)NULL));
		test.erase(eastl::find(test.begin(), test.end(), (void*)NULL));
		EATEST_VERIFY(test.empty());
		EATEST_VERIFY(test.validate());

		test.set_capacity(0);    // "Does nothing currently."
		EATEST_VERIFY(test.capacity() == 0);
		EATEST_VERIFY(test.validate());

	}   // "Crash here."

	{
		const int FV_SIZE = 100;
		fixed_vector<unique_ptr<unsigned int>, FV_SIZE> fvmv1; // to move via move assignment operator
		fixed_vector<unique_ptr<unsigned int>, FV_SIZE> fvmv2; // to move via move copy constructor

		for (unsigned int i = 0; i < FV_SIZE; ++i) // populate fvmv1
			fvmv1.push_back(make_unique<unsigned int>(i));

		fvmv2 = eastl::move(fvmv1); // Test move assignment operator

		for (unsigned int i = 0; i < FV_SIZE; ++i)
		{
			EATEST_VERIFY(!fvmv1[i]);
			EATEST_VERIFY(*fvmv2[i] == i);
		}
		EATEST_VERIFY(fvmv2.validate());
		
		swap(fvmv1, fvmv2); // Test swap with move-only objects
		for (unsigned int i = 0; i < FV_SIZE; ++i)
		{
			EATEST_VERIFY(*fvmv1[i] == i);
			EATEST_VERIFY(!fvmv2[i]);
		}
		EATEST_VERIFY(fvmv1.validate());
		EATEST_VERIFY(fvmv2.validate());

		fixed_vector<unique_ptr<unsigned int>, FV_SIZE> fv = eastl::move(fvmv1); // Test move copy constructor
		for (unsigned int i = 0; i < FV_SIZE; ++i)
		{
			EATEST_VERIFY(!fvmv1[i]);
			EATEST_VERIFY(*fv[i] == i);
		}
		EATEST_VERIFY(fv.validate());
	}

	{ // Test that ensures that move ctor that triggers realloc (e.g. > capacity) does so via move code path
		eastl::fixed_vector<TestObject, 1, true> fv1;
		fv1.push_back(TestObject(0));
		fv1.push_back(TestObject(0));
		int64_t copyCtorCount0 = TestObject::sTOCopyCtorCount, moveCtorCount0 = TestObject::sTOMoveCtorCount;
		decltype(fv1) fv2 = eastl::move(fv1);
		EATEST_VERIFY(TestObject::sTOCopyCtorCount == copyCtorCount0 && TestObject::sTOMoveCtorCount == (moveCtorCount0 + 2));
	}
	{ // Same as above but with custom statefull allocator
		struct MyAlloc : public eastl::allocator
		{
			MyAlloc()=default;
			MyAlloc(int i) : dummy(i) {}
			int dummy;
		};
		eastl::fixed_vector<TestObject, 1, true, MyAlloc> fv1;
		fv1.push_back(TestObject(0));
		fv1.push_back(TestObject(0));
		int64_t copyCtorCount0 = TestObject::sTOCopyCtorCount, moveCtorCount0 = TestObject::sTOMoveCtorCount;
		decltype(fv1) fv2(eastl::move(fv1), MyAlloc(123));
		EATEST_VERIFY(TestObject::sTOCopyCtorCount == copyCtorCount0 && TestObject::sTOMoveCtorCount == (moveCtorCount0 + 2));
	}

	#if defined(EA_COMPILER_CPP17_ENABLED) && __has_include(<variant>)
	//Test pairing of std::variant with fixed_vector
	{
		eastl::fixed_vector<std::variant<int>, 4> v;
		eastl::fixed_vector<std::variant<int>, 4> b = eastl::move(v);
	}
	#endif
	return nErrorCount;     
}










