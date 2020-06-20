/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include "EASTLTest.h"
#include "GetTypeName.h"
#include <EAStdC/EAString.h>
#include <EAStdC/EAStopwatch.h>
#include <EASTL/core_allocator_adapter.h>
#include <EASTL/core_allocator.h>
#include <EASTL/intrusive_ptr.h>
#include <EASTL/linked_array.h>
#include <EASTL/linked_ptr.h>
#include <EASTL/safe_ptr.h>
#include <EASTL/scoped_array.h>
#include <EASTL/scoped_ptr.h>
#include <EASTL/shared_array.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/weak_ptr.h>
#include <eathread/eathread_thread.h>
#include <future>

EA_DISABLE_ALL_VC_WARNINGS()
#include <stdio.h>
#include <string.h>
#ifdef EA_PLATFORM_WINDOWS
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <Windows.h>
#elif defined(EA_PLATFORM_ANDROID)
	#include <android/log.h>
#endif
EA_RESTORE_ALL_VC_WARNINGS()

EA_DISABLE_VC_WARNING(4702 4800)  // 4702: unreachable code
								  // 4800: forcing value to bool 'true' or 'false'



namespace SmartPtrTest
{
	/// CustomDeleter
	///
	/// Used for testing unique_ptr deleter overrides. Otherwise acts the same as the default deleter.
	///
	struct CustomDeleter
	{
		template <typename T>
		void operator()(const T* p) const // We use a const argument type in order to be most flexible with what types we accept. 
			{ delete const_cast<T*>(p); }

		CustomDeleter() {}
		CustomDeleter(const CustomDeleter&) {}
		CustomDeleter(CustomDeleter&&) {}
		CustomDeleter& operator=(const CustomDeleter&) { return *this; }
		CustomDeleter& operator=(CustomDeleter&&) { return *this; }
	};


	struct CustomArrayDeleter
	{
		template <typename T>
		void operator()(const T* p) const // We use a const argument type in order to be most flexible with what types we accept. 
			{ delete[] const_cast<T*>(p); }

		CustomArrayDeleter() {}
		CustomArrayDeleter(const CustomArrayDeleter&) {}
		CustomArrayDeleter(CustomArrayDeleter&&) {}
		CustomArrayDeleter& operator=(const CustomArrayDeleter&) { return *this; }
		CustomArrayDeleter& operator=(CustomArrayDeleter&&) { return *this; }
	};


	/// A
	///
	/// This is used for various tests. 
	///
	struct A
	{
		char mc;
		static int mCount;

		A(char c = 0) 
			: mc(c) { ++mCount; }

		A(const A& x) 
			: mc(x.mc) { ++mCount; }

		A& operator=(const A& x) 
			{ mc = x.mc; return *this; }

		virtual ~A() // Virtual because we subclass A below.
			{ --mCount; }
	};


	int A::mCount = 0;


	/// B
	///
	struct B : public A
	{
	};



	/// RefCountTest
	///
	/// This is used for tests involving intrusive_ptr.
	/// 
	struct RefCountTest
	{
		int mRefCount;
		static int mCount;

		RefCountTest()
			: mRefCount(0) { ++mCount; }

		RefCountTest(const RefCountTest&)
			: mRefCount(0) { ++mCount; }

		RefCountTest& operator=(const RefCountTest&)
			{ return *this; }

		virtual ~RefCountTest()
			{ --mCount; }

		virtual int AddRef()
			{ return (int)((mRefCount++) + 1); }

		virtual int Release()
		{
			int rc = (int)((mRefCount--) - 1);
			if(rc)
				return rc;
			mRefCount = 1;
			delete this;
			return 0;
		}
	};

	int RefCountTest::mCount = 0;



	/// Test
	///
	/// This is used for tests involving intrusive_ptr.
	/// 
	struct Test : public RefCountTest
	{
		bool* mpBool;

		Test(bool* pBool)
			: mpBool(pBool) { *pBool = true; }

		Test(const Test& x): 
			RefCountTest(x), mpBool(x.mpBool) { }

		Test& operator=(const Test& x)
			{ mpBool = x.mpBool; return *this; }

	   ~Test()
			{ *mpBool = false; }
	};



	/// IntrusiveParent / IntrusiveChild
	///
	/// This is used for tests involving intrusive_ptr.
	/// 
	struct IntrusiveParent : public RefCountTest
	{
	};

	struct IntrusiveChild : public IntrusiveParent
	{
	};


	/// intrusive_ptr_add_ref / intrusive_ptr_release
	///
	/// This is used for tests involving intrusive_ptr.
	/// 
	struct IntrusiveCustom : public RefCountTest
	{
		static int mAddRefCallCount;
		static int mReleaseCallCount;

		virtual int AddRef()
		{
			++mAddRefCallCount;
			return RefCountTest::AddRef();
		}

		virtual int Release()
		{
			++mReleaseCallCount;
			return RefCountTest::Release();
		}
	};

	int IntrusiveCustom::mAddRefCallCount  = 0;
	int IntrusiveCustom::mReleaseCallCount = 0;

	void intrusive_ptr_add_ref(IntrusiveCustom* p)
	{
		p->AddRef();
	}
	 
	void intrusive_ptr_release(IntrusiveCustom* p)
	{
		p->Release();
	}


	/// ParentClass / ChildClass / GrandChildClass
	///
	/// This is used for tests involving shared_ptr.
	/// 
	struct ParentClass
	{
		virtual ~ParentClass() { }
		virtual void DoNothingParentClass() { }
	};

	struct ChildClass : public ParentClass
	{
		virtual void DoNothingChildClass() { }
	};

	struct GrandChildClass : public ChildClass
	{
		virtual void DoNothingGrandChildClass() { }
	};



	/// NamedClass
	///
	struct NamedClass
	{
		const char* mpName;
		const char* mpName2;
		static int  mnCount;

		NamedClass(const char* pName = NULL)
			: mpName(pName), mpName2(NULL) { ++mnCount; }

		NamedClass(const char* pName, const char* pName2)
			: mpName(pName), mpName2(pName2) { ++mnCount; }

		NamedClass(const NamedClass& x)
			: mpName(x.mpName), mpName2(x.mpName2) { ++mnCount; }

		NamedClass& operator=(const NamedClass& x)
			{ mpName = x.mpName; mpName2 = x.mpName2; return *this; }

	   ~NamedClass()
			{ --mnCount; }
	};

	int NamedClass::mnCount = 0;



	/// Y
	///
	/// This is used for tests involving shared_ptr and enabled_shared_from_this.
	/// 
	struct Y : public eastl::enable_shared_from_this<Y>
	{
		static int mnCount;

		Y() { ++mnCount; }
		Y(const Y&) { ++mnCount; }
		Y& operator=(const Y&) { return *this; }
	   ~Y() { --mnCount; }

		eastl::shared_ptr<Y> f()
			{ return shared_from_this(); }
	};

	int Y::mnCount = 0;



	/// ACLS / BCLS
	///
	/// This is used for tests involving shared_ptr.
	/// 
	class ACLS : public eastl::enable_shared_from_this<ACLS>
	{
	public:
		static int mnCount;
		int a;

		ACLS(int _a_ = 0) : a(_a_) { ++mnCount; }
		ACLS(const ACLS& x) : a(x.a) { ++mnCount; }
		ACLS& operator=(const ACLS& x) { a = x.a; return *this; }
	   ~ACLS() { --mnCount; }
	};

	int ACLS::mnCount = 0;


	class BCLS : public ACLS
	{
	public:
		static int mnCount;
		int b;

		BCLS(int _b_ = 0) : b(_b_) { ++mnCount; }
		BCLS(const BCLS& x) : ACLS(x), b(x.b) { ++mnCount; }
		BCLS& operator=(const BCLS& x) { b = x.b; ACLS::operator=(x); return *this; }
	   ~BCLS() { --mnCount; }
	};

	int BCLS::mnCount = 0;



	/// A1 / B1
	///
	/// This is used for tests involving shared_ptr.
	/// 
	struct A1
	{
		static int mnCount;
		int a;

		A1(int _a_ = 0) : a(_a_) { ++mnCount; }
		A1(const A1& x) : a(x.a) { ++mnCount; }
		A1& operator=(const A1& x) { a = x.a; return *this; }
	   ~A1() { --mnCount; }
	};

	int A1::mnCount = 0;



	struct B1 : public A1
	{
		static int mnCount;
		int b;

		B1(int _b_ = 0) : b(_b_) { ++mnCount; }
		B1(const B1& x) : A1(x), b(x.b) { ++mnCount; }
		B1& operator=(const B1& x) { b = x.b; A1::operator=(x); return *this; }
	   ~B1() { --mnCount; }
	};

	int B1::mnCount = 0;



	class MockObject
	{
	public:
		MockObject(bool* pAlloc) 
		  : mpAlloc(pAlloc){ *mpAlloc = true; }

	   ~MockObject()
			{ *mpAlloc = false; }

		bool IsAllocated() const
			{ return *mpAlloc; }

		bool* GetAllocPtr() const
			{ return mpAlloc; }

	private:
		bool* mpAlloc;
	};

	class DerivedMockObject : public MockObject
	{
	public:
		DerivedMockObject(bool* pAlloc)
		  : MockObject(pAlloc) {}
	};


	struct foo : public eastl::enable_shared_from_this<foo>
	{
		foo() : mX(0){}
		int mX;
	};

	struct CheckUPtrEmptyInDestructor
	{
		~CheckUPtrEmptyInDestructor()
		{
			if(mpUPtr)
				mCheckUPtrEmpty = (*mpUPtr == nullptr);
		}

		eastl::unique_ptr<CheckUPtrEmptyInDestructor>* mpUPtr{};
		static bool mCheckUPtrEmpty;
	};

	bool CheckUPtrEmptyInDestructor::mCheckUPtrEmpty = false;

	struct CheckUPtrArrayEmptyInDestructor
	{
		~CheckUPtrArrayEmptyInDestructor()
		{
			if(mpUPtr)
				mCheckUPtrEmpty = (*mpUPtr == nullptr);
		}

		eastl::unique_ptr<CheckUPtrArrayEmptyInDestructor[]>* mpUPtr{};
		static bool mCheckUPtrEmpty;
	};

	bool CheckUPtrArrayEmptyInDestructor::mCheckUPtrEmpty = false;
} // namespace SmartPtrTest




static int Test_unique_ptr()
{
	using namespace SmartPtrTest;
	using namespace eastl;

	int nErrorCount(0);

	{
		EATEST_VERIFY(A::mCount == 0);

		// explicit unique_ptr(pointer pValue) noexcept
		unique_ptr<int> pT1(new int(5));
		EATEST_VERIFY(*pT1 == 5);

		// (reference) operator*() const
		*pT1 = 3;
		EATEST_VERIFY(*pT1 == 3);

		// explicit unique_ptr(pointer pValue) noexcept
		unique_ptr<A> pT2(new A(1));
		EATEST_VERIFY(pT2->mc == 1);
		EATEST_VERIFY(A::mCount == 1);

		// Pointers of derived types are allowed (unlike array unique_ptr) 
		unique_ptr<A> pT1B(new B);
		EATEST_VERIFY(pT1B.get() != NULL);
		EATEST_VERIFY(A::mCount == 2);

		A* pA = pT1B.release(); // release simply forgets the owned pointer.
		EATEST_VERIFY(pT1B.get() == NULL);
		EATEST_VERIFY(A::mCount == 2);

		delete pA;
		EATEST_VERIFY(A::mCount == 1);

		// pointer operator->() const noexcept
		pT2->mc = 5;
		EATEST_VERIFY(pT2.get()->mc == 5);

		// void reset(pointer pValue = pointer()) noexcept
		pT2.reset(new A(2));
		EATEST_VERIFY(pT2->mc == 2);
		EATEST_VERIFY(A::mCount == 1);

		pT2.reset(0);
		EATEST_VERIFY(pT2.get() == (A*)0);
		EATEST_VERIFY(A::mCount == 0);

		pT2.reset(new A(3));
		EATEST_VERIFY(pT2->mc == 3);
		EATEST_VERIFY(A::mCount == 1);

		unique_ptr<A> pT3(new A(4));
		EATEST_VERIFY(pT3->mc == 4);
		EATEST_VERIFY(A::mCount == 2);

		// void swap(this_type& scopedPtr) noexcept
		pT2.swap(pT3);
		EATEST_VERIFY(pT2->mc == 4);
		EATEST_VERIFY(pT3->mc == 3);
		EATEST_VERIFY(A::mCount == 2);

		// void swap(unique_ptr<T, D>& scopedPtr1, unique_ptr<T, D>& scopedPtr2) noexcept
		swap(pT2, pT3);
		EATEST_VERIFY(pT2->mc == 3);
		EATEST_VERIFY(pT3->mc == 4);
		EATEST_VERIFY((pT2 < pT3) == (pT2.get() < pT3.get()));
		EATEST_VERIFY(A::mCount == 2);

		// pointer release() noexcept
		unique_ptr<A> pRelease(new A);
		EATEST_VERIFY(A::mCount == 3);
		pA = pRelease.release();
		delete pA;
		EATEST_VERIFY(A::mCount == 2);

		// constexpr unique_ptr() noexcept
		unique_ptr<A> pT4;
		EATEST_VERIFY(pT4.get() == (A*)0);
		if(pT4)
			EATEST_VERIFY(pT4.get()); // Will fail
		if(!(!pT4))
			EATEST_VERIFY(pT4.get()); // Will fail

		pT4.reset(new A(0));
		if(!pT4)
			EATEST_VERIFY(!pT4.get()); // Will fail

		EATEST_VERIFY(A::mCount == 3);

		// unique_ptr(nullptr_t) noexcept
		unique_ptr<A> pT5(nullptr);
		EATEST_VERIFY(pT5.get() == (A*)0);

		// unique_ptr(pointer pValue, deleter) noexcept
		CustomDeleter customADeleter;
		unique_ptr<A, CustomDeleter> pT6(new A(17), customADeleter);
		EATEST_VERIFY(pT6->mc == 17);

		// unique_ptr(pointer pValue, typename eastl::remove_reference<Deleter>::type&& deleter) noexcept
		unique_ptr<A, CustomDeleter> pT7(new A(18), CustomDeleter());
		EATEST_VERIFY(pT7->mc == 18);

		// unique_ptr(this_type&& x) noexcept
		unique_ptr<A, CustomDeleter> pT8(eastl::move(pT7));
		EATEST_VERIFY(pT8->mc == 18);

		// unique_ptr(unique_ptr<U, E>&& u, ...)
		unique_ptr<A, default_delete<A> > pT9(eastl::move(pT2));

		// this_type& operator=(this_type&& u) noexcept
		// operator=(unique_ptr<U, E>&& u) noexcept
		//unique_ptr<void, CustomDeleter> pTVoid;
		//unique_ptr<int, CustomDeleter>  pTInt(new int(1));
		//pTVoid.operator=<int, CustomDeleter>(eastl::move(pTInt));  // This doesn't work because CustomDeleter doesn't know how to delete void*. Need to rework this test.

		// this_type& operator=(nullptr_t) noexcept
		pT6 = nullptr;
		EATEST_VERIFY(pT6.get() == (A*)0);

		// user reported regression
		// ensure a unique_ptr containing nullptr doesn't call the deleter when its destroyed.
		{
			static bool sLocalDeleterCalled;
			sLocalDeleterCalled = false;

			struct LocalDeleter
			{
				void operator()(int* p) const
				{
					sLocalDeleterCalled = true;
					delete p;
				}
			};

			using local_unique_ptr = eastl::unique_ptr<int, LocalDeleter>;

			local_unique_ptr pEmpty{nullptr};

			pEmpty = local_unique_ptr{new int(42), LocalDeleter()};

			EATEST_VERIFY(sLocalDeleterCalled == false);
		}
	}

	{
		// Test that unique_ptr internal pointer is reset before calling the destructor
		CheckUPtrEmptyInDestructor::mCheckUPtrEmpty = false;

		unique_ptr<CheckUPtrEmptyInDestructor> uptr(new CheckUPtrEmptyInDestructor);
		uptr->mpUPtr = &uptr;
		uptr.reset();
		EATEST_VERIFY(CheckUPtrEmptyInDestructor::mCheckUPtrEmpty);
	}

	{
		// Test that unique_ptr<[]> internal pointer is reset before calling the destructor
		CheckUPtrArrayEmptyInDestructor::mCheckUPtrEmpty = false;

		unique_ptr<CheckUPtrArrayEmptyInDestructor[]> uptr(new CheckUPtrArrayEmptyInDestructor[1]);
		uptr[0].mpUPtr = &uptr;
		uptr.reset();
		EATEST_VERIFY(CheckUPtrArrayEmptyInDestructor::mCheckUPtrEmpty);
	}

	{
		#if EASTL_CORE_ALLOCATOR_ENABLED
			// Test EA::Allocator::EASTLICoreDeleter usage within eastl::shared_ptr.
			// http://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr

			// Consider the following for standards compliance.
			// eastl::shared_ptr<A, EASTLCoreDeleterAdapter> foo(pA, EASTLCoreDeleterAdapter());

			const int cacheAllocationCount = gEASTLTest_AllocationCount;

			using namespace EA::Allocator;

			EASTLCoreAllocatorAdapter ta;
			void* pMem = ta.allocate(sizeof(A));

			EATEST_VERIFY(pMem != nullptr);     
			EATEST_VERIFY(gEASTLTest_AllocationCount > cacheAllocationCount);
			{            
				A* pA = new (pMem) A();
				eastl::shared_ptr<A> foo(pA, EASTLCoreDeleterAdapter());  // Not standards complaint code.  Update EASTL implementation to provide the type of the deleter.
			}
			EATEST_VERIFY(gEASTLTest_AllocationCount == cacheAllocationCount);
			EATEST_VERIFY(A::mCount == 0);
		#endif
	}

	{
		// Test array specialization of unique_ptr

		EATEST_VERIFY(A::mCount == 0);

		// template <typename P>
		// explicit unique_ptr(P pValue) noexcept
		unique_ptr<int[]> pT1(new int[5]);
		pT1[0] = 5;
		EATEST_VERIFY(pT1[0] == 5);

		// Arrays of derived types are not allowed (unlike regular unique_ptr) 
		// unique_ptr<A[]> pT1B(new B[5]); // Disabled because it should not compile.

		// (reference) operator[]() const
		pT1[1] = 1;
		EATEST_VERIFY(pT1[1] == 1);

		// explicit unique_ptr(pointer pValue) noexcept
		unique_ptr<A[]> pT2(new A[1]);
		pT2[0].mc = 1;
		EATEST_VERIFY(pT2[0].mc == 1);
		EATEST_VERIFY(A::mCount == 1);

		// pointer operator->() const noexcept
		pT2[0].mc = 5;
		EATEST_VERIFY(pT2[0].mc == 5);

		// void reset(pointer pValue = pointer()) noexcept
		pT2.reset(new A[2]);
		pT2[0].mc = 2;
		EATEST_VERIFY(pT2[0].mc == 2);

		pT2.reset(0);
		EATEST_VERIFY(pT2.get() == (A*)0);

		pT2.reset(new A[3]);
		pT2[0].mc = 3;
		EATEST_VERIFY(pT2[0].mc == 3);

		unique_ptr<A[]> pT3(new A[4]);
		pT3[0].mc = 4;
		EATEST_VERIFY(pT3[0].mc == 4);

		// void swap(this_type& scopedPtr) noexcept
		pT2.swap(pT3);
		EATEST_VERIFY(pT2[0].mc == 4);
		EATEST_VERIFY(pT3[0].mc == 3);

		// void swap(unique_ptr<T, D>& scopedPtr1, unique_ptr<T, D>& scopedPtr2) noexcept
		swap(pT2, pT3);
		EATEST_VERIFY(pT2[0].mc == 3);
		EATEST_VERIFY(pT3[0].mc == 4);
		EATEST_VERIFY((pT2 < pT3) == (pT2.get() < pT3.get()));

		// pointer release() noexcept
		unique_ptr<A[]> pRelease(new A[1]);
		A* pAArray = pRelease.release();
		delete[] pAArray;

		// constexpr unique_ptr() noexcept
		unique_ptr<A[]> pT4;
		EATEST_VERIFY(pT4.get() == (A*)0);
		if(pT4)
			EATEST_VERIFY(pT4.get()); // Will fail
		if(!(!pT4))
			EATEST_VERIFY(pT4.get()); // Will fail

		pT4.reset(new A[1]);
		if(!pT4)
			EATEST_VERIFY(!pT4.get()); // Will fail

		EATEST_VERIFY(A::mCount == 8);  // There were a number of array creations and deletions above that make this so.

		// unique_ptr(nullptr_t) noexcept
		unique_ptr<A[]> pT5(nullptr);
		EATEST_VERIFY(pT5.get() == (A*)0);

		// unique_ptr(pointer pValue, deleter) noexcept
		CustomArrayDeleter customADeleter;
		unique_ptr<A[], CustomArrayDeleter> pT6(new A[17], customADeleter);
		pT6[0].mc = 17;
		EATEST_VERIFY(pT6[0].mc == 17);

		// unique_ptr(pointer pValue, typename eastl::remove_reference<Deleter>::type&& deleter) noexcept
		unique_ptr<A[], CustomArrayDeleter> pT7(new A[18], CustomArrayDeleter());
		pT7[0].mc = 18;
		EATEST_VERIFY(pT7[0].mc == 18);

		// unique_ptr(this_type&& x) noexcept
		unique_ptr<A[], CustomArrayDeleter> pT8(eastl::move(pT7));
		EATEST_VERIFY(pT8[0].mc == 18);

		// unique_ptr(unique_ptr<U, E>&& u, ...)
		unique_ptr<A[], default_delete<A[]> > pT9(eastl::move(pT2));
		EATEST_VERIFY(pT9[0].mc == 3);

		// this_type& operator=(this_type&& u) noexcept
		// operator=(unique_ptr<U, E>&& u) noexcept
		//unique_ptr<void, CustomDeleter> pTVoid;
		//unique_ptr<int, CustomDeleter>  pTInt(new int(1));
		//pTVoid.operator=<int, CustomDeleter>(eastl::move(pTInt));  // This doesn't work because CustomDeleter doesn't know how to delete void*. Need to rework this test.

		// this_type& operator=(nullptr_t) noexcept
		pT6 = nullptr;
		EATEST_VERIFY(pT6.get() == (A*)0);

		// unique_ptr<> make_unique(Args&&... args);
		unique_ptr<NamedClass> p = eastl::make_unique<NamedClass>("test", "test2");
		EATEST_VERIFY(EA::StdC::Strcmp(p->mpName, "test") == 0 && EA::StdC::Strcmp(p->mpName2, "test2") == 0);

		unique_ptr<NamedClass[]> pArray = eastl::make_unique<NamedClass[]>(4);
		pArray[0].mpName = "test";
		EATEST_VERIFY(EA::StdC::Strcmp(p->mpName, "test") == 0);

		#ifdef EASTL_TEST_DISABLED_PENDING_SUPPORT
		{
			const size_t kAlignedStructAlignment = 512;
			struct AlignedStruct {} EA_ALIGN(kAlignedStructAlignment);

			unique_ptr<AlignedStruct> pAlignedStruct = eastl::make_unique<AlignedStruct>();
			EATEST_VERIFY_F(intptr_t(pAlignedStruct.get()) % kAlignedStructAlignment == 0, "pAlignedStruct didn't have proper alignment");
		}
		#endif

		//Expected to not be valid:
		//unique_ptr<NamedClass[4]> p2Array4 = eastl::make_unique<NamedClass[4]>();
		//p2Array4[0].mpName = "test";
		//EATEST_VERIFY(EA::StdC::Strcmp(p2Array4[0].mpName, "test") == 0);
	}

	EATEST_VERIFY(A::mCount == 0); // This check verifies that no A instances were lost, which also verifies that the [] version of the deleter was used in all cases.

	// validate unique_ptr's compressed_pair implementation is working. 
	{
		const int ARBITRARY_SIZE = 256;
		static_assert(sizeof(unique_ptr<short>)     == sizeof(uintptr_t), "");
		static_assert(sizeof(unique_ptr<long>)      == sizeof(uintptr_t), "");

		// unique_ptr should be the same size as a pointer. The deleter object is empty so the
		// eastl::compressed_pair implementation will remove that deleter data member from the unique_ptr.
		{
			auto deleter = [](void* pMem) { free(pMem); };
			unique_ptr<void, decltype(deleter)> sptr(malloc(ARBITRARY_SIZE), deleter);
			static_assert(sizeof(sptr) == (sizeof(uintptr_t)), "unexpected unique_ptr size");
		}

		// unique_ptr should be larger than a pointer when the deleter functor is capturing state.  This state forces
		// the compressed_pair to cached the data in unique_ptr locally. 
		{
			int a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
			auto deleter = [=](void* pMem) { auto result = (a+b+c+d+e+f); EA_UNUSED(result); free(pMem); };
			unique_ptr<void, decltype(deleter)> sptr(malloc(ARBITRARY_SIZE), deleter);
			static_assert(sizeof(sptr) == ((6 * sizeof(int)) + (sizeof(uintptr_t))), "unexpected unique_ptr size");
		}

		// Simply test moving the one unique pointer to another.
		// Exercising operator=(T&&)
		{
			{
				unique_ptr<int> ptr(new int(3));
				EATEST_VERIFY(ptr.get() && *ptr == 3);

				unique_ptr<int> newPtr(new int(4));
				EATEST_VERIFY(newPtr.get() && *newPtr == 4);

				ptr = eastl::move(newPtr);  // Deletes int(3) and assigns mpValue to int(4)
				EATEST_VERIFY(ptr.get() && *ptr == 4);
				EATEST_VERIFY(newPtr.get() == nullptr);
			}

		#if EA_HAVE_CPP11_INITIALIZER_LIST
			{
				unique_ptr<int[]> ptr(new int[3]{ 0, 1, 2 });
				EATEST_VERIFY(ptr.get() && ptr[0] == 0 && ptr[1] == 1 && ptr[2] == 2);

				unique_ptr<int[]> newPtr(new int[3]{ 3, 4, 5 });
				EATEST_VERIFY(newPtr.get() && newPtr[0] == 3 && newPtr[1] == 4 && newPtr[2] == 5);

				ptr = eastl::move(newPtr);  // Deletes int(3) and assigns mpValue to int(4)
				EATEST_VERIFY(ptr.get() && ptr[0] == 3 && ptr[1] == 4 && ptr[2] == 5);
				EATEST_VERIFY(newPtr.get() == nullptr);
			}
		#endif

			// ToDo: Test move assignment between two convertible types with an is_assignable deleter_type
			//{
			//	struct Base {};
			//	struct Child : public Base {};

			//	typedef unique_ptr<Base, CustomDeleter> BaseSPtr;
			//	typedef unique_ptr<Child, CustomDeleter> ChildSPtr;

			//	static_assert(!is_array<BaseSPtr::element_type>::value, "This test requires a non-array type");
			//	static_assert(is_convertible<ChildSPtr::pointer, BaseSPtr::pointer>::value, "UniquePtr ptr types must be convertible for this test");
			//	static_assert(is_assignable<BaseSPtr::deleter_type&, ChildSPtr::deleter_type&&>::value, "Deleter types must be assignable to one another");

			//	BaseSPtr ptr(new Base);
			//	EATEST_VERIFY(ptr.get());

			//	unique_ptr<Child> newPtr(new Child);
			//	EATEST_VERIFY(newPtr.get());

			//	ptr = eastl::move(newPtr);
			//	EATEST_VERIFY(ptr);
			//	EATEST_VERIFY(newPtr.get() == nullptr);
			//}
		}
	}

	return nErrorCount;
}


static int Test_scoped_ptr()
{
	using namespace SmartPtrTest;
	using namespace eastl;

	int nErrorCount(0);

	{
		EATEST_VERIFY(A::mCount == 0);

		scoped_ptr<int> pT1(new int(5));
		EATEST_VERIFY(*pT1 == 5);

		*pT1 = 3;
		EATEST_VERIFY(*pT1 == 3);
		EATEST_VERIFY(pT1.get() == get_pointer(pT1));

		scoped_ptr<A> pT2(new A(1));
		EATEST_VERIFY(pT2->mc == 1);
		EATEST_VERIFY(A::mCount == 1);

		pT2.reset(new A(2));
		EATEST_VERIFY(pT2->mc == 2);

		pT2.reset(0);
		EATEST_VERIFY(pT2.get() == (A*)0);
		EATEST_VERIFY(pT2.get() == get_pointer(pT2));

		pT2.reset(new A(3));
		EATEST_VERIFY(pT2->mc == 3);

		scoped_ptr<A> pT3(new A(4));
		EATEST_VERIFY(pT3->mc == 4);

		pT2.swap(pT3);
		EATEST_VERIFY(pT2->mc == 4);
		EATEST_VERIFY(pT3->mc == 3);

		swap(pT2, pT3);
		EATEST_VERIFY(pT2->mc == 3);
		EATEST_VERIFY(pT3->mc == 4);
		EATEST_VERIFY((pT2 < pT3) == (pT2.get() < pT3.get()));

		scoped_ptr<A> pT4;
		EATEST_VERIFY(pT4.get() == (A*)0);
		if(pT4)
			EATEST_VERIFY(pT4.get()); // Will fail
		if(!(!pT4))
			EATEST_VERIFY(pT4.get()); // Will fail

		pT4.reset(new A(0));
		if(!pT4)
			EATEST_VERIFY(!pT4.get()); // Will fail

		EATEST_VERIFY(A::mCount == 3);
	}

	{   // Test the detach function.
		scoped_ptr<A> ptr(new A);
		A* pA = ptr.detach();
		delete pA;
	}

	{
		scoped_ptr<void> ptr(new int);
		(void)ptr;
	}

	EATEST_VERIFY(A::mCount == 0);

	return nErrorCount;
}



static int Test_scoped_array()
{
	using namespace SmartPtrTest;
	using namespace eastl;

	int nErrorCount(0);

	{
		scoped_array<int> pT1(new int[5]);
		pT1[0] = 5;
		EATEST_VERIFY(pT1[0] == 5);
		EATEST_VERIFY(pT1.get()[0] == 5);

		scoped_array<A> pT2(new A[2]);
		EATEST_VERIFY(A::mCount == 2);
		EATEST_VERIFY(pT2[0].mc == 0);
		EATEST_VERIFY(pT2.get()[0].mc == 0);
		EATEST_VERIFY(get_pointer(pT2)[0].mc == 0);

		pT2.reset(new A[4]);
		EATEST_VERIFY(A::mCount == 4);
		if(!pT2)
			EATEST_VERIFY(!pT2.get()); // Will fail

		pT2.reset(0);
		EATEST_VERIFY(A::mCount == 0);
		if(pT2)
			EATEST_VERIFY(pT2.get()); // Will fail
		if(!(!pT2))
			EATEST_VERIFY(pT2.get()); // Will fail

		scoped_array<A> pT3(new A[3]);
		EATEST_VERIFY(A::mCount == 3);

		pT2.swap(pT3);
		EATEST_VERIFY(A::mCount == 3);

		swap(pT2, pT3);
		EATEST_VERIFY(A::mCount == 3);
		EATEST_VERIFY((pT2 < pT3) == (pT2.get() < pT3.get()));

		EATEST_VERIFY(A::mCount == 3);
	}

	{   // Test the detach function.
		scoped_array<A> ptr(new A[6]);
		A* pArray = ptr.detach();
		delete[] pArray;
	}

	{
		scoped_array<void> ptr(new int[6]);
		(void)ptr;
	}

	EATEST_VERIFY(A::mCount == 0);

	return nErrorCount;
}


static int Test_shared_ptr()
{
	using namespace SmartPtrTest;
	using namespace eastl;

	int nErrorCount(0);

	// Name test.
	#if EASTLTEST_GETTYPENAME_AVAILABLE
		//eastl::string sTypeName = GetTypeName<typename eastl::unique_ptr<int>::pointer>();
		//EA::UnitTest::Report("type name of (typename shared_ptr<int>::pointer): %s", sTypeName.c_str());

		//sTypeName = GetTypeName<typename eastl::common_type<int*, int*>::type>();
		//EA::UnitTest::Report("type name of (typename eastl::common_type<int*, int*>::type): %s", sTypeName.c_str());
	#endif

	{
		shared_ptr<int> pT1;
		EATEST_VERIFY(pT1.get() == NULL);
	}

	{
		shared_ptr<int> pT1(new int(5));
		EATEST_VERIFY(*pT1 == 5);
		EATEST_VERIFY(pT1.get() == get_pointer(pT1));
		EATEST_VERIFY(pT1.use_count() == 1);
		EATEST_VERIFY(pT1.unique() );

		shared_ptr<int> pT2;
		EATEST_VERIFY(pT1 != pT2);
		EATEST_VERIFY(pT1.use_count() == 1);
		EATEST_VERIFY(pT1.unique());

		pT2 = pT1;
		EATEST_VERIFY(pT1.use_count() == 2);
		EATEST_VERIFY(pT2.use_count() == 2);
		EATEST_VERIFY(!pT1.unique());
		EATEST_VERIFY(!(pT1 < pT2)); // They should be equal
		EATEST_VERIFY(pT1 == pT2);

		*pT1 = 3;
		EATEST_VERIFY(*pT1 == 3);
		EATEST_VERIFY(*pT1 == 3);
		EATEST_VERIFY(*pT2 == 3);

		pT2.reset((int*)NULL);
		EATEST_VERIFY(pT2.unique());
		EATEST_VERIFY(pT2.use_count() == 1);
		EATEST_VERIFY(pT1.unique());
		EATEST_VERIFY(pT1.use_count() == 1);
		EATEST_VERIFY(pT1 != pT2);
	}

	{
		EATEST_VERIFY(A::mCount == 0);

		shared_ptr<A> pT2(new A(0));
		EATEST_VERIFY(A::mCount == 1);
		EATEST_VERIFY(pT2->mc == 0);
		EATEST_VERIFY(pT2.use_count() == 1);
		EATEST_VERIFY(pT2.unique());

		pT2.reset(new A(1));
		EATEST_VERIFY(pT2->mc == 1);
		EATEST_VERIFY(A::mCount == 1);
		EATEST_VERIFY(pT2.use_count() == 1);
		EATEST_VERIFY(pT2.unique());

		shared_ptr<A> pT3(new A(2));
		EATEST_VERIFY(A::mCount == 2);

		pT2.swap(pT3);
		EATEST_VERIFY(pT2->mc == 2);
		EATEST_VERIFY(pT3->mc == 1);
		EATEST_VERIFY(A::mCount == 2);

		swap(pT2, pT3);
		EATEST_VERIFY(pT2->mc == 1);
		EATEST_VERIFY(pT3->mc == 2);
		EATEST_VERIFY(A::mCount == 2);
		if(!pT2)
			EATEST_VERIFY(!pT2.get()); // Will fail

		shared_ptr<A> pT4;
		EATEST_VERIFY(pT2.use_count() == 1);
		EATEST_VERIFY(pT2.unique());
		EATEST_VERIFY(A::mCount == 2);
		if(pT4)
			EATEST_VERIFY(pT4.get()); // Will fail
		if(!(!pT4))
			EATEST_VERIFY(pT4.get()); // Will fail

		pT4 = pT2;
		EATEST_VERIFY(pT2.use_count() == 2);
		EATEST_VERIFY(pT4.use_count() == 2);
		EATEST_VERIFY(!pT2.unique());
		EATEST_VERIFY(!pT4.unique());
		EATEST_VERIFY(A::mCount == 2);
		EATEST_VERIFY(pT2 == pT4);
		EATEST_VERIFY(pT2 != pT3);
		EATEST_VERIFY(!(pT2 < pT4)); // They should be equal

		shared_ptr<A> pT5(pT4);
		EATEST_VERIFY(pT4 == pT5);
		EATEST_VERIFY(pT2.use_count() == 3);
		EATEST_VERIFY(pT4.use_count() == 3);
		EATEST_VERIFY(pT5.use_count() == 3);
		EATEST_VERIFY(!pT5.unique());

		pT4 = shared_ptr<A>((A*)NULL);
		EATEST_VERIFY(pT4.unique());
		EATEST_VERIFY(pT4.use_count() == 1);
		EATEST_VERIFY(pT2.use_count() == 2);

		EATEST_VERIFY(A::mCount == 2);
	}


	// Regression test reported by a user.
	// typename eastl::enable_if<!eastl::is_array<U>::value && eastl::is_convertible<U*, element_type*>::value, this_type&>::type
	// operator=(unique_ptr<U, Deleter> && uniquePtr)
	{
		{
			shared_ptr<A> rT1(new A(42));
			unique_ptr<B> rT2(new B);  // default ctor uses 0
			rT2->mc = 115;

			EATEST_VERIFY(rT1->mc == 42);
			EATEST_VERIFY(rT2->mc == 115);

			rT1 = eastl::move(rT2);

			EATEST_VERIFY(rT1->mc == 115);
			// EATEST_VERIFY(rT2->mc == 115);  // state of object post-move is undefined.
		}

		// test the state of the shared_ptr::operator= return
		{
			shared_ptr<A> rT1(new A(42));
			unique_ptr<B> rT2(new B);  // default ctor uses 0
			rT2->mc = 115;

			shared_ptr<A> operatorReturn = (rT1 = eastl::move(rT2));

			EATEST_VERIFY(operatorReturn == rT1);

			EATEST_VERIFY(operatorReturn->mc == 115);
			// EATEST_VERIFY(rT1->mc == 115); // implied as both are pointing to the same address
		}
	}


	{ // Test member template functions.
		shared_ptr<ChildClass>      pCC(new GrandChildClass);
		shared_ptr<ParentClass>     pPC(pCC);
		shared_ptr<GrandChildClass> pGCC(static_pointer_cast<GrandChildClass>(pPC));
	}


	{ // Test enable_shared_from_this
		shared_ptr<Y> p(new Y);
		shared_ptr<Y> q = p->f();

		EATEST_VERIFY(p == q);
		EATEST_VERIFY(!(p < q || q < p)); // p and q must share ownership

		shared_ptr<BCLS> bctrlp = shared_ptr<BCLS>(new BCLS);
	}


	{ // Test static_pointer_cast, etc.
		shared_ptr<GrandChildClass> pGCC(new GrandChildClass);
		shared_ptr<ParentClass>     pPC = static_pointer_cast<ParentClass>(pGCC);

		EATEST_VERIFY(pPC == pGCC);

		#if EASTL_RTTI_ENABLED
		shared_ptr<ChildClass>      pCC = dynamic_pointer_cast<ChildClass>(pPC);
		EATEST_VERIFY(pCC == pGCC);
		#endif

		#if !defined(__GNUC__) ||  (__GNUC__ >= 3) // If not using old GCC (GCC 2.x is broken)...
			eastl::shared_ptr<const void> pVoidPtr = shared_ptr<ParentClass>(new ParentClass);
			shared_ptr<ParentClass> ap = const_pointer_cast<ParentClass>(static_pointer_cast<const ParentClass>(pVoidPtr));
		#endif

		//typedef shared_ptr<void const> ASPtr;
		//shared_ptr<void const> pVoidPtr = ASPtr(new ParentClass);
		//ASPtr ap = const_pointer_cast<ParentClass>(static_pointer_cast<const ParentClass>(pVoidPtr));
	}


	{ // Test static_shared_pointer_cast, etc.
		shared_ptr<GrandChildClass> pGCC(new GrandChildClass);
		shared_ptr<ParentClass>     pPC = static_shared_pointer_cast<ParentClass /*, EASTLAllocatorType, smart_ptr_deleter<ParentClass>*/ >(pGCC);

		EATEST_VERIFY(pPC == pGCC);

		#if EASTL_RTTI_ENABLED
		shared_ptr<ChildClass>      pCC = dynamic_shared_pointer_cast<ChildClass /*, EASTLAllocatorType, smart_ptr_deleter<ParentClass>*/ >(pPC);
		EATEST_VERIFY(pCC == pGCC);
		#endif
	}


	{ // Test smart_ptr_deleter
		shared_ptr<void> pVoid(new ParentClass, smart_ptr_deleter<ParentClass>());
		EATEST_VERIFY(pVoid.get() != NULL);

		pVoid = shared_ptr<ParentClass>(new ParentClass, smart_ptr_deleter<ParentClass>());
		EATEST_VERIFY(pVoid.get() != NULL);
	}


	{ // Test shared_ptr lambda deleter
		auto deleter = [](int*) {}; 
		eastl::shared_ptr<int> ptr(nullptr, deleter);

		EATEST_VERIFY(!ptr);
		EATEST_VERIFY(ptr.get() == nullptr);
	}


	{ // Test of shared_ptr<void const>
		#if !defined(__GNUC__) ||  (__GNUC__ >= 3) // If not using old GCC (GCC 2.x is broken)...
			shared_ptr<void const> voidPtr = shared_ptr<A1>(new A1);
			shared_ptr<A1>         a1Ptr   = const_pointer_cast<A1>(static_pointer_cast<const A1>(voidPtr));
		#endif
	}


	{ // Test of static_pointer_cast
		shared_ptr<B1> bPtr = shared_ptr<B1>(new B1);
		shared_ptr<A1> aPtr = static_pointer_cast<A1, B1>(bPtr);
	}


	{ // Test shared_ptr<void>
		{
			#if !defined(__GNUC__) ||  (__GNUC__ >= 3) // If not using old GCC (GCC 2.x is broken)...
				const char* const pName = "NamedClassTest";

				NamedClass* const pNamedClass0 = new NamedClass(pName);
				EATEST_VERIFY(pNamedClass0->mpName == pName);

			  //shared_ptr<void const, EASTLAllocatorType, smart_ptr_deleter<NamedClass> > voidPtr(pNamedClass0);
				shared_ptr<void const> voidPtr(pNamedClass0);
				EATEST_VERIFY(voidPtr.get() == pNamedClass0);

				NamedClass* const pNamedClass1 = (NamedClass*)voidPtr.get();
				EATEST_VERIFY(pNamedClass1->mpName == pName);
			#endif
		}

		{
			#if !defined(__GNUC__) ||  (__GNUC__ >= 3) // If not using old GCC (GCC 2.x is broken)...
				const char* const pName = "NamedClassTest";

				NamedClass* const pNamedClass0 = new NamedClass(pName);
				EATEST_VERIFY(pNamedClass0->mpName == pName);

				shared_ptr<void const> voidPtr(pNamedClass0, smart_ptr_deleter<NamedClass>());
				EATEST_VERIFY(voidPtr.get() == pNamedClass0);

				NamedClass* const pNamedClass1 = (NamedClass*)voidPtr.get();
				EATEST_VERIFY(pNamedClass1->mpName == pName);
			#endif
		}
	}


	{
		const char* const pName1 = "NamedClassTest1";
		const char* const pName2 = "NamedClassTest2";

		shared_ptr<NamedClass> sp(new NamedClass(pName1));
		EATEST_VERIFY(!sp == false);
		EATEST_VERIFY(sp.unique());
		EATEST_VERIFY(sp->mpName == pName1);

		shared_ptr<NamedClass> sp2 = sp;
		EATEST_VERIFY(sp2.use_count() == 2);

		sp2.reset(new NamedClass(pName2));
		EATEST_VERIFY(sp2.use_count() == 1);
		EATEST_VERIFY(sp.unique());
		EATEST_VERIFY(sp2->mpName == pName2);

		sp.reset();
		EATEST_VERIFY(!sp == true);
	}

	{
		// Exception handling tests
		#if EASTL_EXCEPTIONS_ENABLED
			try {
				weak_ptr<A>   pWeakA;             // leave uninitalized
				shared_ptr<A> pSharedA(pWeakA);   // This should throw eastl::bad_weak_ptr
				EATEST_VERIFY(false);
			}
			catch(eastl::bad_weak_ptr&)
			{
				EATEST_VERIFY(true);            // This pathway should be taken.
			}
			catch(...)
			{
				EATEST_VERIFY(false);
			}


			ThrowingAllocator<true> throwingAllocator; // Throw on first attempt to allocate.
			shared_ptr<A> pA0;

			try {
				A::mCount = 0;
				pA0 = eastl::allocate_shared<A, ThrowingAllocator<true> >(throwingAllocator, 'a');
				EATEST_VERIFY(false);
			}
			catch(std::bad_alloc&)
			{
				EATEST_VERIFY(true);                // This pathway should be taken.
				EATEST_VERIFY(pA0.get() == NULL);   // The C++11 Standard doesn't seem to require this, but that's how we currently do it until we learn it should be otherwise.
				EATEST_VERIFY(pA0.use_count() == 0);
				EATEST_VERIFY(A::mCount == 0);      // Verify that there were no surviving A instances since the exception.
			}
			catch(...)
			{
				EATEST_VERIFY(false);
			}


			try {
				shared_ptr<A> pA1(new A('a'), default_delete<A>(), throwingAllocator);
				EATEST_VERIFY(false);
			}
			catch(std::bad_alloc&)
			{
				EATEST_VERIFY(true);                // This pathway should be taken.
				EATEST_VERIFY(A::mCount == 0);
			}
			catch(...)
			{
				EATEST_VERIFY(false);
			}

		#endif

	}

	#if EASTL_RTTI_ENABLED
	{
		// template <typename U, typename A, typename D>
		// shared_ptr(const shared_ptr<U, A, D>& sharedPtr, dynamic_cast_tag);
		// To do.

		// template <typename U, typename A, typename D, typename UDeleter>
		// shared_ptr(const shared_ptr<U, A, D>& sharedPtr, dynamic_cast_tag, const UDeleter&);
		// To do.
	}
	#endif

	EATEST_VERIFY(A::mCount == 0);

	return nErrorCount;
}




#if EASTL_THREAD_SUPPORT_AVAILABLE
	// C++ Standard section 20.7.2.5 -- shared_ptr atomic access
	// shared_ptr thread safety is about safe use of the pointer itself and not about what it points to. shared_ptr thread safety
	// allows you to safely use shared_ptr from different threads, but if the object shared_ptr holds requires thread safety then
	// you need to separately handle that in a thread-safe way. A good way to think about it is this: "shared_ptr is as thread-safe as a raw pointer."
	//
	// Some helper links:
	//     http://stackoverflow.com/questions/9127816/stdshared-ptr-thread-safety-explained
	//     http://stackoverflow.com/questions/14482830/stdshared-ptr-thread-safety
	//     http://cppwisdom.quora.com/shared_ptr-is-almost-thread-safe
	// 

	// Test the ability of Futex to report the callstack of another thread holding a futex.
	struct SharedPtrTestThread : public EA::Thread::IRunnable
	{
		EA::Thread::ThreadParameters    mThreadParams;
		EA::Thread::Thread              mThread;
		volatile bool                   mbShouldContinue;
		int                             mnErrorCount;
		eastl::shared_ptr<TestObject>*  mpSPTO;
		eastl::weak_ptr<TestObject>*    mpWPTO;

		SharedPtrTestThread() : mThreadParams(), mThread(), mbShouldContinue(true), mnErrorCount(0), mpSPTO(NULL), mpWPTO(NULL) {}
		SharedPtrTestThread(const SharedPtrTestThread&){}
		void operator=(const SharedPtrTestThread&){}

		intptr_t Run(void*)
		{
			int& nErrorCount = mnErrorCount; // declare nErrorCount so that EATEST_VERIFY can work, as it depends on it being declared.

			while(mbShouldContinue)
			{
				EA::UnitTest::ThreadSleepRandom(1, 10);

				EATEST_VERIFY(mpSPTO->get()->mX == 99);

				eastl::shared_ptr<TestObject> temp(mpWPTO->lock());
				EATEST_VERIFY(temp->mX == 99);

				eastl::shared_ptr<TestObject> spTO2(*mpSPTO);
				EATEST_VERIFY(spTO2->mX == 99);
				EATEST_VERIFY(spTO2.use_count() >= 2);

				eastl::weak_ptr<TestObject> wpTO2(spTO2);
				temp = mpWPTO->lock();
				EATEST_VERIFY(temp->mX == 99);

				temp = spTO2;
				spTO2.reset();
				EATEST_VERIFY(mpSPTO->get()->mX == 99);
			}

			return nErrorCount;
		}
	};
#endif


template<typename Fct1, typename Fct2>
void Try_to_invoke_2tasks_at_same_time(const Fct1& fct1, const Fct2& fct2)
{
	std::atomic_uint32_t waitThread = true;
	std::atomic_uint32_t waitMainThread = true;

	std::future<void> f1 = std::async(
		[&]
		{
			waitThread = false;
			while(waitMainThread)
			{
			}
			fct1();
		});

	waitMainThread = false;
	while(waitThread)
	{
	}
	fct2();
}

static int Test_shared_ptr_thread()
{
	using namespace SmartPtrTest;
	using namespace eastl;
	using namespace EA::Thread;

	int nErrorCount(0);

	#if EASTL_THREAD_SUPPORT_AVAILABLE
		{
			SharedPtrTestThread    thread[4];
			shared_ptr<TestObject> spTO(new TestObject(99));
			weak_ptr<TestObject>   wpTO(spTO);

			for(size_t i = 0; i < EAArrayCount(thread); i++)
			{
				thread[i].mpSPTO = &spTO;
				thread[i].mpWPTO = &wpTO;
				thread[i].mThreadParams.mpName = "SharedPtrTestThread";
			}

			for(size_t i = 0; i < EAArrayCount(thread); i++)
				thread[i].mThread.Begin(&thread[0], NULL, &thread[0].mThreadParams);

			EA::UnitTest::ThreadSleep(2000);

			for(size_t i = 0; i < EAArrayCount(thread); i++)
				thread[i].mbShouldContinue = false;

			for(size_t i = 0; i < EAArrayCount(thread); i++)
			{
				thread[i].mThread.WaitForEnd();
				nErrorCount += thread[i].mnErrorCount;
			}
		}
	#endif

	#if EASTL_THREAD_SUPPORT_AVAILABLE
		{
			// We currently do light testing of the atomic functions. It would take a bit of work to fully test
			// the memory behavior of these in a rigorous way. Also, as of this writing we don't have a portable
			// way to use the std::memory_order functionality.

			shared_ptr<TestObject> spTO(new TestObject(55));

			// bool atomic_is_lock_free(const shared_ptr<T>*);
			EATEST_VERIFY(!atomic_is_lock_free(&spTO));

			// shared_ptr<T> atomic_load(const shared_ptr<T>* pSharedPtr);
			// shared_ptr<T> atomic_load_explicit(const shared_ptr<T>* pSharedPtr, ... /*std::memory_order memoryOrder*/);
			shared_ptr<TestObject> spTO2 = atomic_load(&spTO);
			EATEST_VERIFY(spTO->mX == 55);
			EATEST_VERIFY(spTO2->mX == 55);

			// void atomic_store(shared_ptr<T>* pSharedPtrA, shared_ptr<T> sharedPtrB);
			// void atomic_store_explicit(shared_ptr<T>* pSharedPtrA, shared_ptr<T> sharedPtrB, ... /*std::memory_order memoryOrder*/);
			spTO2->mX = 56;
			EATEST_VERIFY(spTO->mX == 56);
			EATEST_VERIFY(spTO2->mX == 56);

			atomic_store(&spTO, shared_ptr<TestObject>(new TestObject(77)));
			EATEST_VERIFY(spTO->mX == 77);
			EATEST_VERIFY(spTO2->mX == 56);

			// shared_ptr<T> atomic_exchange(shared_ptr<T>* pSharedPtrA, shared_ptr<T> sharedPtrB);
			// shared_ptr<T> atomic_exchange_explicit(shared_ptr<T>* pSharedPtrA, shared_ptr<T> sharedPtrB, ... /*std::memory_order memoryOrder*/);
			spTO = atomic_exchange(&spTO2, spTO);
			EATEST_VERIFY(spTO->mX == 56);
			EATEST_VERIFY(spTO2->mX == 77);
			
			spTO = atomic_exchange_explicit(&spTO2, spTO);
			EATEST_VERIFY(spTO->mX == 77);
			EATEST_VERIFY(spTO2->mX == 56);

			// bool atomic_compare_exchange_strong(shared_ptr<T>* pSharedPtr, shared_ptr<T>* pSharedPtrCondition, shared_ptr<T> sharedPtrNew);
			// bool atomic_compare_exchange_weak(shared_ptr<T>* pSharedPtr, shared_ptr<T>* pSharedPtrCondition, shared_ptr<T> sharedPtrNew);
			// bool atomic_compare_exchange_strong_explicit(shared_ptr<T>* pSharedPtr, shared_ptr<T>* pSharedPtrCondition, shared_ptr<T> sharedPtrNew, ... /*memory_order memoryOrderSuccess, memory_order memoryOrderFailure*/);
			// bool atomic_compare_exchange_weak_explicit(shared_ptr<T>* pSharedPtr, shared_ptr<T>* pSharedPtrCondition, shared_ptr<T> sharedPtrNew, ... /*memory_order memoryOrderSuccess, memory_order memoryOrderFailure*/);
			shared_ptr<TestObject> spTO3 = atomic_load(&spTO2);
			bool result = atomic_compare_exchange_strong(&spTO3, &spTO, make_shared<TestObject>(88));   // spTO3 != spTO, so this should do no exchange and return false.
			EATEST_VERIFY(!result);
			EATEST_VERIFY(spTO3->mX == 56);
			EATEST_VERIFY(spTO->mX == 56);

			result = atomic_compare_exchange_strong(&spTO3, &spTO2, make_shared<TestObject>(88));       // spTO3 == spTO2, so this should succeed.
			EATEST_VERIFY(result);
			EATEST_VERIFY(spTO2->mX == 56);
			EATEST_VERIFY(spTO3->mX == 88);
		}
	#endif

	EATEST_VERIFY(A::mCount == 0);
	TestObject::Reset();

	{
		// Check that counter inside shared_ptr<> is thread safe when using reset().
		for(uint32_t counter = 0; counter < 200000; ++counter)
		{
			eastl::shared_ptr<double> valueSPtr1(new double(0.));
			eastl::shared_ptr<double> valueSPtr2(valueSPtr1);

			Try_to_invoke_2tasks_at_same_time(
				[&]
				{
					valueSPtr1.reset();
				},
				[&]
				{
					valueSPtr2.reset();
				});
		}
	}

	{
		// Check that counter inside shared_ptr<> and weak_ptr<> is thread safe when using reset().
		for(uint32_t counter = 0; counter < 200000; ++counter)
		{
			eastl::shared_ptr<double> valueSPtr(new double(0.));
			eastl::weak_ptr<double> valueWPtr(valueSPtr);

			Try_to_invoke_2tasks_at_same_time(
				[&]
				{
					valueSPtr.reset();
				},
				[&]
				{
					valueWPtr.reset();
				});
		}
	}

	{
		// Check that counter inside shared_ptr<> is thread safe when using operator =().
		for(uint32_t counter = 0; counter < 200000; ++counter)
		{
			eastl::shared_ptr<double> valueSPtr(new double(0.));
			eastl::weak_ptr<double> valueWPtr(valueSPtr);
			eastl::shared_ptr<double> otherValueSPtr(new double(0.));

			Try_to_invoke_2tasks_at_same_time(
				[&]
				{
					valueSPtr = otherValueSPtr;
				},
				[&]
				{
					valueWPtr = otherValueSPtr;
				});
		}
	}

	{
		// Check that counter inside shared_ptr<> and weak_ptr<> is thread safe when using operator =().
		for(uint32_t counter = 0; counter < 200000; ++counter)
		{
			eastl::shared_ptr<double> valueSPtr(new double(0.));
			eastl::shared_ptr<double> valueWPtr(valueSPtr);
			eastl::shared_ptr<double> otherValueSPtr(new double(0.));

			Try_to_invoke_2tasks_at_same_time(
				[&]
				{
					valueSPtr = otherValueSPtr;
				},
				[&]
				{
					valueWPtr = otherValueSPtr;
				});
		}
	}

	return nErrorCount;
}


static int Test_weak_ptr()
{
	using namespace SmartPtrTest;
	using namespace eastl;

	int nErrorCount(0);

	{
		weak_ptr<int>   pW0;
		shared_ptr<int> pS0(new int(0));
		shared_ptr<int> pS1(new int(1));
		weak_ptr<int>   pW1(pS1);
		weak_ptr<int>   pW2;
		weak_ptr<int>   pW3(pW2);

		EATEST_VERIFY(pS1.use_count() == 1);
		EATEST_VERIFY(pW1.use_count() == 1);
		EATEST_VERIFY(pW2.use_count() == 0);
		EATEST_VERIFY(pW3.use_count() == 0);
		EATEST_VERIFY(pW1.expired() == false);
		EATEST_VERIFY(pW2.expired() == true);
		EATEST_VERIFY(pW3.expired() == true);
		pS1.reset();
		EATEST_VERIFY(pW1.expired() == true);
		pW1 = pS0;
		EATEST_VERIFY(pW1.expired() == false);
		pW1.swap(pW2);
		EATEST_VERIFY(pW1.expired() == true);
		EATEST_VERIFY(pW2.expired() == false);
		pW1 = pW2;
		EATEST_VERIFY(pW1.expired() == false);
		pW3 = pW1;
		EATEST_VERIFY(pW3.expired() == false);
		EATEST_VERIFY(pS1.use_count() == 0);
		pW3.reset();
		EATEST_VERIFY(pW3.expired() == true);
		pS1.reset(new int(3));
		EATEST_VERIFY(pS1.use_count() == 1);
		pW3 = pS1;
		EATEST_VERIFY(pS1.use_count() == 1);
		EATEST_VERIFY(pS1.use_count() == pW3.use_count());

		shared_ptr<int> pShared2(pW2.lock());
		shared_ptr<int> pShared3(pW3.lock());

		EATEST_VERIFY(pShared2.use_count() == 2);
		EATEST_VERIFY(pShared3.use_count() == 2); 
		swap(pW2, pW3);
		EATEST_VERIFY(pW2.use_count() == 2);
		EATEST_VERIFY(pW3.use_count() == 2);
		pW1 = pW3;
		EATEST_VERIFY(pW3.use_count() == 2);

		EATEST_VERIFY((pW2 < pW3) || (pW3 < pW2));

		EATEST_VERIFY(pS0.use_count() == 2);
		pW0 = pS0; // This tests the deletion of a weak_ptr after its associated shared_ptr has destructed.
		EATEST_VERIFY(pS0.use_count() == 2);
	}


	{
		weak_ptr<NamedClass> wp;

		EATEST_VERIFY(wp.use_count() == 0);
		EATEST_VERIFY(wp.expired() == true);

		{
			shared_ptr<NamedClass> sp(new NamedClass("NamedClass"));
			wp = sp;

			EATEST_VERIFY(wp.use_count() == 1);
			EATEST_VERIFY(wp.expired() == false);
		}

		EATEST_VERIFY(wp.use_count() == 0);
		EATEST_VERIFY(wp.expired() == true);
	}

	{   // shared_from_this
		// This example is taken from the C++11 Standard doc.
		shared_ptr<const foo> pFoo(new foo);
		shared_ptr<const foo> qFoo = pFoo->shared_from_this();

		EATEST_VERIFY(pFoo == qFoo);
		EATEST_VERIFY(!(pFoo < qFoo) && !(qFoo < pFoo)); // p and q share ownership
	}

	{   // weak_from_this const
		shared_ptr<const foo> pFoo(new foo);
		weak_ptr<const foo> qFoo = pFoo->weak_from_this();

		EATEST_VERIFY(pFoo == qFoo.lock());
		EATEST_VERIFY(!(pFoo < qFoo.lock()) && !(qFoo.lock() < pFoo)); // p and q share ownership
	}

	{   // weak_from_this
		shared_ptr<foo> pFoo(new foo);
		weak_ptr<foo> qFoo = pFoo->weak_from_this();

		EATEST_VERIFY(pFoo == qFoo.lock());
		EATEST_VERIFY(!(pFoo < qFoo.lock()) && !(qFoo.lock() < pFoo)); // p and q share ownership
	}
	
	return nErrorCount;
}


static int Test_shared_array()
{
	using namespace SmartPtrTest;
	using namespace eastl;

	int nErrorCount(0);

	{
		shared_array<int> pT1(new int[5]);
		pT1[0] = 5;
		EATEST_VERIFY(pT1[0] == 5);
		EATEST_VERIFY(pT1.get() == get_pointer(pT1));
		EATEST_VERIFY(pT1.use_count() == 1);
		EATEST_VERIFY(pT1.unique());

		shared_array<int> pT2;
		EATEST_VERIFY(pT1 != pT2);
		EATEST_VERIFY(pT1.use_count() == 1);
		EATEST_VERIFY(pT1.unique());

		pT2 = pT1;
		EATEST_VERIFY(pT1.use_count() == 2);
		EATEST_VERIFY(pT2.use_count() == 2);
		EATEST_VERIFY(!pT1.unique());
		EATEST_VERIFY(!(pT1 < pT2)); // They should be equal
		EATEST_VERIFY(pT1 == pT2);

		*pT1 = 3;
		EATEST_VERIFY(*pT1 == 3);
		EATEST_VERIFY(*pT1 == 3);
		EATEST_VERIFY(*pT2 == 3);

		pT2.reset(0);
		EATEST_VERIFY(pT2.unique());
		EATEST_VERIFY(pT2.use_count() == 1);
		EATEST_VERIFY(pT1.unique());
		EATEST_VERIFY(pT1.use_count() == 1);
		EATEST_VERIFY(pT1 != pT2);
	}

	{
		EATEST_VERIFY(A::mCount == 0);

		shared_array<A> pT2(new A[5]);
		EATEST_VERIFY(A::mCount == 5);
		EATEST_VERIFY(pT2->mc == 0);
		EATEST_VERIFY(pT2.use_count() == 1);
		EATEST_VERIFY(pT2.unique());

		pT2.reset(new A[1]);
		pT2[0].mc = 1;
		EATEST_VERIFY(pT2->mc == 1);
		EATEST_VERIFY(A::mCount == 1);
		EATEST_VERIFY(pT2.use_count() == 1);
		EATEST_VERIFY(pT2.unique());

		shared_array<A> pT3(new A[2]);
		EATEST_VERIFY(A::mCount == 3);

		pT2.swap(pT3);
		pT2[0].mc = 2;
		EATEST_VERIFY(pT2->mc == 2);
		EATEST_VERIFY(pT3->mc == 1);
		EATEST_VERIFY(A::mCount == 3);

		swap(pT2, pT3);
		EATEST_VERIFY(pT2->mc == 1);
		EATEST_VERIFY(pT3->mc == 2);
		EATEST_VERIFY(A::mCount == 3);
		if(!pT2)
			EATEST_VERIFY(!pT2.get()); // Will fail

		shared_array<A> pT4;
		EATEST_VERIFY(pT2.use_count() == 1);
		EATEST_VERIFY(pT2.unique());
		EATEST_VERIFY(A::mCount == 3);
		if(pT4)
			EATEST_VERIFY(pT4.get()); // Will fail
		if(!(!pT4))
			EATEST_VERIFY(pT4.get()); // Will fail

		pT4 = pT2;
		EATEST_VERIFY(pT2.use_count() == 2);
		EATEST_VERIFY(pT4.use_count() == 2);
		EATEST_VERIFY(!pT2.unique());
		EATEST_VERIFY(!pT4.unique());
		EATEST_VERIFY(A::mCount == 3);
		EATEST_VERIFY(pT2 == pT4);
		EATEST_VERIFY(pT2 != pT3);
		EATEST_VERIFY(!(pT2 < pT4)); // They should be equal

		shared_array<A> pT5(pT4);
		EATEST_VERIFY(pT4 == pT5);
		EATEST_VERIFY(pT2.use_count() == 3);
		EATEST_VERIFY(pT4.use_count() == 3);
		EATEST_VERIFY(pT5.use_count() == 3);
		EATEST_VERIFY(!pT5.unique());

		pT4 = shared_array<A>(0);
		EATEST_VERIFY(pT4.unique());
		EATEST_VERIFY(pT4.use_count() == 1);
		EATEST_VERIFY(pT2.use_count() == 2);

		EATEST_VERIFY(A::mCount == 3);
	}

	EATEST_VERIFY(A::mCount == 0);

	return nErrorCount;
}



static int Test_linked_ptr()
{
	using namespace SmartPtrTest;
	using namespace eastl;

	int nErrorCount(0);

	{
		linked_ptr<int> pT1(new int(5));
		EATEST_VERIFY(*pT1.get() == 5);
		EATEST_VERIFY(pT1.get() == get_pointer(pT1));
		EATEST_VERIFY(pT1.use_count() == 1);
		EATEST_VERIFY(pT1.unique());

		linked_ptr<int> pT2;
		EATEST_VERIFY(pT1 != pT2);
		EATEST_VERIFY(pT1.use_count() == 1);
		EATEST_VERIFY(pT1.unique());

		pT2 = pT1;
		EATEST_VERIFY(pT1.use_count() == 2);
		EATEST_VERIFY(pT2.use_count() == 2);
		EATEST_VERIFY(!pT1.unique());
		EATEST_VERIFY(!(pT1 < pT2)); // They should be equal
		EATEST_VERIFY(pT1 == pT2);

		*pT1 = 3;
		EATEST_VERIFY(*pT1.get() == 3);
		EATEST_VERIFY(*pT1 == 3);
		EATEST_VERIFY(*pT2 == 3);

		pT2.reset((int*)NULL);
		EATEST_VERIFY(pT2.unique());
		EATEST_VERIFY(pT2.use_count() == 1);
		EATEST_VERIFY(pT1.unique());
		EATEST_VERIFY(pT1.use_count() == 1);
		EATEST_VERIFY(pT1 != pT2);
	}

	{
		EATEST_VERIFY(A::mCount == 0);

		linked_ptr<A> pT2(new A(0));
		EATEST_VERIFY(A::mCount == 1);
		EATEST_VERIFY(pT2->mc == 0);
		EATEST_VERIFY(pT2.use_count() == 1);
		EATEST_VERIFY(pT2.unique());

		pT2.reset(new A(1));
		EATEST_VERIFY(pT2->mc == 1);
		EATEST_VERIFY(A::mCount == 1);
		EATEST_VERIFY(pT2.use_count() == 1);
		EATEST_VERIFY(pT2.unique());

		linked_ptr<A> pT3(new A(2));
		EATEST_VERIFY(A::mCount == 2);

		linked_ptr<A> pT4;
		EATEST_VERIFY(pT2.use_count() == 1);
		EATEST_VERIFY(pT2.unique());
		EATEST_VERIFY(A::mCount == 2);
		if(pT4)
			EATEST_VERIFY(pT4.get()); // Will fail
		if(!(!pT4))
			EATEST_VERIFY(pT4.get()); // Will fail

		pT4 = pT2;
		EATEST_VERIFY(pT2.use_count() == 2);
		EATEST_VERIFY(pT4.use_count() == 2);
		EATEST_VERIFY(!pT2.unique());
		EATEST_VERIFY(!pT4.unique());
		EATEST_VERIFY(A::mCount == 2);
		EATEST_VERIFY(pT2 == pT4);
		EATEST_VERIFY(pT2 != pT3);
		EATEST_VERIFY(!(pT2 < pT4)); // They should be equal

		linked_ptr<A> pT5(pT4);
		EATEST_VERIFY(pT4 == pT5);
		EATEST_VERIFY(pT2.use_count() == 3);
		EATEST_VERIFY(pT4.use_count() == 3);
		EATEST_VERIFY(pT5.use_count() == 3);
		EATEST_VERIFY(!pT5.unique());

		pT4 = linked_ptr<A>((A*)NULL);
		EATEST_VERIFY(pT4.unique());
		EATEST_VERIFY(pT4.use_count() == 1);
		EATEST_VERIFY(pT2.use_count() == 2);

		EATEST_VERIFY(A::mCount == 2);
	}
		
	{  // Do some force_delete tests.
		linked_ptr<A> pT2(new A(0));
		linked_ptr<A> pT3(pT2);
		pT2.force_delete();
		pT3.force_delete();
	}

	EATEST_VERIFY(A::mCount == 0);


	{   // Verify that subclasses are usable.
		bool bAlloc = false;

		eastl::linked_ptr<DerivedMockObject> pDMO(new DerivedMockObject(&bAlloc));
		eastl::linked_ptr<MockObject> a1(pDMO);
		eastl::linked_ptr<MockObject> a2;

		a2 = pDMO;
	}

	{ // Test regression for a bug.
		linked_ptr<A> pT2;
		linked_ptr<A> pT3(pT2);     // In the bug linked_ptr::mpPrev and mpNext were not initialized via this ctor.
		pT3.reset(new A);           // In the bug this would crash due to unintialized mpPrev/mpNext.

		linked_ptr<B> pT4;
		linked_ptr<A> pT5(pT4);
		pT5.reset(new A);

		linked_array<A> pT6;
		linked_array<A> pT7(pT6);
		pT7.reset(new A[1]);
	}

	return nErrorCount;
}



static int Test_linked_array()
{
	using namespace SmartPtrTest;
	using namespace eastl;

	int nErrorCount(0);

	{
		// Tests go here.
	}

	{  // Do some force_delete tests.
		linked_array<A> pT2(new A[2]);
		linked_array<A> pT3(pT2);
		pT2.force_delete();
		pT3.force_delete();
	}

	EATEST_VERIFY(A::mCount == 0);


	return nErrorCount;
}



static int Test_intrusive_ptr()
{
	using namespace SmartPtrTest;
	using namespace eastl;

	int nErrorCount = 0;

	{   // Test ctor/dtor
		intrusive_ptr<RefCountTest> ip1;
		intrusive_ptr<RefCountTest> ip2(NULL, false);
		intrusive_ptr<RefCountTest> ip3(NULL, true);
		intrusive_ptr<RefCountTest> ip4(new RefCountTest, true);
		intrusive_ptr<RefCountTest> ip5(new RefCountTest, false);
		intrusive_ptr<RefCountTest> ip6(ip1);
		intrusive_ptr<RefCountTest> ip7(ip4);

		EATEST_VERIFY(ip1.get() == NULL);
		EATEST_VERIFY(!ip1);

		EATEST_VERIFY(ip2.get() == NULL);
		EATEST_VERIFY(!ip2);

		EATEST_VERIFY(ip3.get() == NULL);
		EATEST_VERIFY(!ip3);

		EATEST_VERIFY(ip4.get() != NULL);
		EATEST_VERIFY(ip4.get()->mRefCount == 2);
		EATEST_VERIFY(ip4);

		EATEST_VERIFY(ip5.get() != NULL);
		EATEST_VERIFY(ip5.get()->mRefCount == 0);
		ip5.get()->AddRef();
		EATEST_VERIFY(ip5.get()->mRefCount == 1);
		EATEST_VERIFY(ip5);

		EATEST_VERIFY(ip6.get() == NULL);
		EATEST_VERIFY(!ip6);

		EATEST_VERIFY(ip7.get() != NULL);
		EATEST_VERIFY(ip7.get()->mRefCount == 2);
		EATEST_VERIFY(ip7);
	}

	{ 
		// Test move-ctor
		{
			VERIFY(RefCountTest::mCount == 0);
			intrusive_ptr<RefCountTest> ip1(new RefCountTest);
			VERIFY(RefCountTest::mCount == 1);
			VERIFY(ip1->mRefCount == 1);
			{
				intrusive_ptr<RefCountTest> ip2(eastl::move(ip1));
				VERIFY(ip1.get() != ip2.get());
				VERIFY(ip2->mRefCount == 1);
				VERIFY(RefCountTest::mCount == 1);
			}
			VERIFY(ip1.get() == nullptr);
			VERIFY(RefCountTest::mCount == 0);
		}

		// Test move-assignment
		{
			VERIFY(RefCountTest::mCount == 0);
			intrusive_ptr<RefCountTest> ip1(new RefCountTest);
			VERIFY(RefCountTest::mCount == 1);
			VERIFY(ip1->mRefCount == 1);
			{
				intrusive_ptr<RefCountTest> ip2; 
				ip2 = eastl::move(ip1);
				VERIFY(ip1.get() != ip2.get());
				VERIFY(ip2->mRefCount == 1);
				VERIFY(RefCountTest::mCount == 1);
			}
			VERIFY(ip1.get() == nullptr);
			VERIFY(RefCountTest::mCount == 0);
		}
	}

	{   // Test modifiers (assign, attach, detach, reset, swap)
		RefCountTest* const p1 = new RefCountTest;
		RefCountTest* const p2 = new RefCountTest;
		intrusive_ptr<RefCountTest> ip1;
		intrusive_ptr<RefCountTest> ip2;

		ip1 = p1;
		ip2 = p2;
		EATEST_VERIFY(ip1.get() == p1);
		EATEST_VERIFY((*ip1).mRefCount == 1);
		EATEST_VERIFY(ip1->mRefCount == 1);
		ip1.detach();
		EATEST_VERIFY(ip1.get() == NULL);
		ip1.attach(p1);
		EATEST_VERIFY(ip1.get() == p1);
		EATEST_VERIFY(ip1->mRefCount == 1);
		ip1.swap(ip2);
		EATEST_VERIFY(ip1.get() == p2);
		EATEST_VERIFY(ip2.get() == p1);
		ip1.swap(ip2);
		ip1 = ip2;
		EATEST_VERIFY(ip1 == p2);
		ip1.reset();
		EATEST_VERIFY(ip1.get() == NULL);
		EATEST_VERIFY(ip2.get() == p2);
		ip2.reset();
		EATEST_VERIFY(ip2.get() == NULL);
	}

	{   // Test external functions
		intrusive_ptr<RefCountTest> ip1;
		intrusive_ptr<RefCountTest> ip2(new RefCountTest);
		intrusive_ptr<RefCountTest> ip3(ip1);
		intrusive_ptr<RefCountTest> ip4(ip2);

		// The VC++ code scanner crashes when it scans this code. 
		EATEST_VERIFY(get_pointer(ip1) == NULL);
		EATEST_VERIFY(get_pointer(ip2) != NULL);
		EATEST_VERIFY(get_pointer(ip3) == get_pointer(ip1));
		EATEST_VERIFY(get_pointer(ip4) == get_pointer(ip2));

		EATEST_VERIFY(ip3 == ip1);
		EATEST_VERIFY(ip4 == ip2);
		EATEST_VERIFY(ip1 == ip3);
		EATEST_VERIFY(ip2 == ip4);

		EATEST_VERIFY(ip1 != ip2);
		EATEST_VERIFY(ip3 != ip4);
		EATEST_VERIFY(ip2 != ip1);
		EATEST_VERIFY(ip4 != ip3);

		EATEST_VERIFY(ip3 == ip1.get());
		EATEST_VERIFY(ip4 == ip2.get());
		EATEST_VERIFY(ip1 == ip3.get());
		EATEST_VERIFY(ip2 == ip4.get());

		EATEST_VERIFY(ip1 != ip2.get());
		EATEST_VERIFY(ip3 != ip4.get());
		EATEST_VERIFY(ip2 != ip1.get());
		EATEST_VERIFY(ip4 != ip3.get());

		EATEST_VERIFY(ip3.get() == ip1);
		EATEST_VERIFY(ip4.get() == ip2);
		EATEST_VERIFY(ip1.get() == ip3);
		EATEST_VERIFY(ip2.get() == ip4);

		EATEST_VERIFY(ip1.get() != ip2);
		EATEST_VERIFY(ip3.get() != ip4);
		EATEST_VERIFY(ip2.get() != ip1);
		EATEST_VERIFY(ip4.get() != ip3);

		EATEST_VERIFY((ip4 < ip3) || (ip3 < ip4));

		swap(ip1, ip3);
		EATEST_VERIFY(get_pointer(ip3) == get_pointer(ip1));

		swap(ip2, ip4);
		EATEST_VERIFY(get_pointer(ip2) == get_pointer(ip4));

		swap(ip1, ip2);
		EATEST_VERIFY(get_pointer(ip1) != NULL);
		EATEST_VERIFY(get_pointer(ip2) == NULL);
		EATEST_VERIFY(get_pointer(ip1) == get_pointer(ip4));
		EATEST_VERIFY(get_pointer(ip2) == get_pointer(ip3));
	}

	{   // Misc tests.
		intrusive_ptr<Test> ip;
		EATEST_VERIFY(ip.get() == NULL);

		ip.reset();
		EATEST_VERIFY(ip.get() == NULL);

		intrusive_ptr<Test> ip2(NULL, false);
		EATEST_VERIFY(ip.get() == NULL);

		bool boolValue = false;
		Test* pTest = new Test(&boolValue);
		EATEST_VERIFY(boolValue);
		pTest->AddRef();
		intrusive_ptr<Test> ip3(pTest, false);
		EATEST_VERIFY(ip3.get() == pTest);
		ip3.reset();
		EATEST_VERIFY(!boolValue);
	}

	{   // Misc tests.
		bool boolArray[3];
		memset(boolArray, 0, sizeof(boolArray));

		Test* p1 = new Test(boolArray + 0);
		EATEST_VERIFY(boolArray[0] && !boolArray[1] && !boolArray[2]);
		intrusive_ptr<Test> arc1(p1);
		EATEST_VERIFY(boolArray[0] && !boolArray[1] && !boolArray[2]);

		Test* p2 = new Test(boolArray + 1);
		EATEST_VERIFY(boolArray[0] && boolArray[1] && !boolArray[2]);
		arc1 = p2;
		EATEST_VERIFY(!boolArray[0] && boolArray[1] && !boolArray[2]);

		Test* p3 = new Test(boolArray + 2);
		EATEST_VERIFY(!boolArray[0] && boolArray[1] && boolArray[2]);
		arc1 = p3;
		EATEST_VERIFY(!boolArray[0] && !boolArray[1] && boolArray[2]);
		arc1 = NULL;
		
		EATEST_VERIFY(!boolArray[0] && !boolArray[1] && !boolArray[2]);
	}

	{ // Test intrusive_ptr_add_ref() / intrusive_ptr_release()
		IntrusiveCustom* const pIC = new IntrusiveCustom;

		{
			intrusive_ptr<IntrusiveCustom> bp = intrusive_ptr<IntrusiveCustom>(pIC);
			intrusive_ptr<IntrusiveCustom> ap = bp;
		}
		
		EATEST_VERIFY((IntrusiveCustom::mAddRefCallCount > 0) && (IntrusiveCustom::mReleaseCallCount == IntrusiveCustom::mAddRefCallCount));
	}

	{ // Regression
		intrusive_ptr<IntrusiveChild>  bp = intrusive_ptr<IntrusiveChild>(new IntrusiveChild);
		intrusive_ptr<IntrusiveParent> ap = bp;
	}

	return nErrorCount;
}


struct RandomLifetimeObject : public eastl::safe_object
{
	void DoSomething() const { }
};



static int Test_safe_ptr()
{
	using namespace SmartPtrTest;
	using namespace eastl;

	int nErrorCount = 0;

	{   // non-const RandomLifetimeObject
		RandomLifetimeObject* pObject = new RandomLifetimeObject;
		eastl::safe_ptr<RandomLifetimeObject> pSafePtr(pObject);

		eastl::safe_ptr<RandomLifetimeObject> pSafePtrCopy1 = pSafePtr;
		eastl::safe_ptr<RandomLifetimeObject> pSafePtrCopy2(pSafePtr);

		pSafePtr->DoSomething();

		eastl::safe_ptr<RandomLifetimeObject>* pSafePtrCopy3 = new eastl::safe_ptr<RandomLifetimeObject>(pSafePtr);
		eastl::safe_ptr<RandomLifetimeObject>* pSafePtrCopy4 = new eastl::safe_ptr<RandomLifetimeObject>(pSafePtr);
		EATEST_VERIFY(pSafePtrCopy3->get() == pObject);
		EATEST_VERIFY(pSafePtrCopy4->get() == pObject);
		delete pSafePtrCopy3;
		delete pSafePtrCopy4;

		delete pSafePtr;

		EATEST_VERIFY(pSafePtrCopy1.get() == NULL);
		EATEST_VERIFY(pSafePtrCopy2.get() == NULL);
	}

	{   // const RandomLifetimeObject
		RandomLifetimeObject* pObject = new RandomLifetimeObject;
		eastl::safe_ptr<const RandomLifetimeObject> pSafePtr(pObject);

		eastl::safe_ptr<const RandomLifetimeObject> pSafePtrCopy1(pSafePtr);
		eastl::safe_ptr<const RandomLifetimeObject> pSafePtrCopy2 = pSafePtr;

		pSafePtr->DoSomething();

		eastl::safe_ptr<const RandomLifetimeObject>* pSafePtrCopy3 = new eastl::safe_ptr<const RandomLifetimeObject>(pSafePtr);
		eastl::safe_ptr<const RandomLifetimeObject>* pSafePtrCopy4 = new eastl::safe_ptr<const RandomLifetimeObject>(pSafePtr);
		EATEST_VERIFY(pSafePtrCopy3->get() == pObject);
		EATEST_VERIFY(pSafePtrCopy4->get() == pObject);
		delete pSafePtrCopy3;
		delete pSafePtrCopy4;

		delete pSafePtr;

		EATEST_VERIFY(pSafePtrCopy1.get() == NULL);
		EATEST_VERIFY(pSafePtrCopy2.get() == NULL);
	}

	return nErrorCount;
}


int TestSmartPtr()
{
	using namespace SmartPtrTest;
	using namespace eastl;

	int nErrorCount = 0;

	nErrorCount += Test_unique_ptr();
	nErrorCount += Test_scoped_ptr();
	nErrorCount += Test_scoped_array();
	nErrorCount += Test_shared_ptr();
	nErrorCount += Test_shared_ptr_thread();
	nErrorCount += Test_weak_ptr();
	nErrorCount += Test_shared_array();
	nErrorCount += Test_linked_ptr();
	nErrorCount += Test_linked_array();
	nErrorCount += Test_intrusive_ptr();
	nErrorCount += Test_safe_ptr();

	EATEST_VERIFY(A::mCount == 0);
	EATEST_VERIFY(RefCountTest::mCount == 0);
	EATEST_VERIFY(NamedClass::mnCount == 0);
	EATEST_VERIFY(Y::mnCount == 0);
	EATEST_VERIFY(ACLS::mnCount == 0);
	EATEST_VERIFY(BCLS::mnCount == 0);
	EATEST_VERIFY(A1::mnCount == 0);
	EATEST_VERIFY(B1::mnCount == 0);

	return nErrorCount;
}

EA_RESTORE_VC_WARNING()  // 4702







