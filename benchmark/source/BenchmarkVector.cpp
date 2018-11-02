/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLBenchmark.h"
#include "EASTLTest.h"
#include <EAStdC/EAStopwatch.h>
#include <EASTL/algorithm.h>
#include <EASTL/vector.h>
#include <EASTL/sort.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
	#pragma warning(disable: 4350)
#endif
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif


using namespace EA;


typedef std::vector<uint64_t>     StdVectorUint64;
typedef eastl::vector<uint64_t>   EaVectorUint64;


namespace
{


	//////////////////////////////////////////////////////////////////////////////
	// MovableType
	// 
	struct MovableType
	{
		int8_t* mpData;
		enum { kDataSize = 128 };

		MovableType() : mpData(new int8_t[kDataSize])
			{ memset(mpData, 0, kDataSize); }

		MovableType(const MovableType& x) : mpData(new int8_t[kDataSize])
			{ memcpy(mpData, x.mpData, kDataSize); }

		MovableType& operator=(const MovableType& x)
		{
			if(!mpData)
				mpData = new int8_t[kDataSize];
			memcpy(mpData, x.mpData, kDataSize);
			return *this;
		}

		MovableType(MovableType&& x) EA_NOEXCEPT : mpData(x.mpData)
			{ x.mpData = NULL; }

		MovableType& operator=(MovableType&& x)
		{
			eastl::swap(mpData, x.mpData); // In practice it may not be right to do a swap, depending on the case.
			return *this;
		}

	   ~MovableType()
			{ delete[] mpData; }
	};


	//////////////////////////////////////////////////////////////////////////////
	// AutoRefCount
	// 
	// Basic ref-counted object.
	//
	template <typename T>
	class AutoRefCount
	{
	public:
		T* mpObject;

	public:
		AutoRefCount() EA_NOEXCEPT : mpObject(NULL) 
			{}

		AutoRefCount(T* pObject) EA_NOEXCEPT : mpObject(pObject) 
		{ 
			if(mpObject)
				mpObject->AddRef();
		} 

		AutoRefCount(T* pObject, int) EA_NOEXCEPT : mpObject(pObject) 
		{
			// Inherit the existing refcount.
		} 

		AutoRefCount(const AutoRefCount& x) EA_NOEXCEPT : mpObject(x.mpObject) 
		{ 
			if(mpObject)
				mpObject->AddRef();
		}

		AutoRefCount& operator=(const AutoRefCount& x)     
		{         
			return operator=(x.mpObject);
		}

		AutoRefCount& operator=(T* pObject)
		{         
			if(pObject != mpObject)
			{
				T* const pTemp = mpObject; // Create temporary to prevent possible problems with re-entrancy.
				if(pObject)
					pObject->AddRef();
				mpObject = pObject;
				if(pTemp)
					pTemp->Release();
			}
			return *this;
		}

		AutoRefCount(AutoRefCount&& x) EA_NOEXCEPT : mpObject(x.mpObject) 
		{ 
			x.mpObject = NULL;
		}

		AutoRefCount& operator=(AutoRefCount&& x)
		{
			if(mpObject)
				mpObject->Release();
			mpObject = x.mpObject;
			x.mpObject = NULL;
			return *this;
		}

		~AutoRefCount() 
		{
			if(mpObject)
				mpObject->Release();
		}

		T& operator *() const EA_NOEXCEPT
			{ return *mpObject; }

		T* operator ->() const EA_NOEXCEPT
			{ return  mpObject; }

		operator T*() const EA_NOEXCEPT
			{ return  mpObject; }

	}; // class AutoRefCount


	struct RefCounted
	{
		int        mRefCount;
		static int msAddRefCount;
		static int msReleaseCount;

		RefCounted() : mRefCount(1) {}

		int AddRef()
			{ ++msAddRefCount; return ++mRefCount; }

		int Release()
		{
			++msReleaseCount;
			if(mRefCount > 1)
				return --mRefCount;
			delete this;
			return 0;
		}
	};

	int RefCounted::msAddRefCount  = 0;
	int RefCounted::msReleaseCount = 0;

} // namespace 


namespace 
{
	template <typename Container> 
	void TestPushBack(EA::StdC::Stopwatch& stopwatch, Container& c, eastl::vector<uint32_t>& intVector)
	{
		stopwatch.Restart();
		for(eastl_size_t j = 0, jEnd = intVector.size(); j < jEnd; j++)
			c.push_back((uint64_t)intVector[j]);
		stopwatch.Stop();
	}


	template <typename Container>
	void TestBracket(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		uint64_t temp = 0;
		stopwatch.Restart();
		for(typename Container::size_type j = 0, jEnd = c.size(); j < jEnd; j++)
			temp += c[j];
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)(temp & 0xffffffff));
	}


	template <typename Container>
	void TestFind(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		stopwatch.Restart();
		typedef typename Container::iterator iterator_t;  // This typedef is required to get this code to compile on RVCT
		iterator_t it = eastl::find(c.begin(), c.end(), UINT64_C(0xffffffffffff));
		stopwatch.Stop();
		if(it != c.end())
			sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)*it);
	}


	template <typename Container>
	void TestSort(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		// Intentionally use eastl sort in order to measure just  
		// vector access speed and not be polluted by sort speed.
		stopwatch.Restart();
		eastl::quick_sort(c.begin(), c.end()); 
		stopwatch.Stop();
		sprintf(Benchmark::gScratchBuffer, "%u", (unsigned)(c[0] & 0xffffffff));
	}


	template <typename Container>
	void TestInsert(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		typename Container::size_type j, jEnd;
		typename Container::iterator it;

		stopwatch.Restart();
		for(j = 0, jEnd = 100, it = c.begin(); j < jEnd; ++j)
		{
			it = c.insert(it, UINT64_C(0xffffffffffff));

			if(it == c.end()) // Try to safely increment the iterator three times.
				it = c.begin();
			if(++it == c.end())
				it = c.begin();
			if(++it == c.end())
				it = c.begin();
		}
		stopwatch.Stop();
	}


	template <typename Container>
	void TestErase(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		typename Container::size_type j, jEnd;
		typename Container::iterator it;

		stopwatch.Restart();
		for(j = 0, jEnd = 100, it = c.begin(); j < jEnd; ++j)
		{
			it = c.erase(it);

			if(it == c.end()) // Try to safely increment the iterator three times.
				it = c.begin();
			if(++it == c.end())
				it = c.begin();
			if(++it == c.end())
				it = c.begin();
		}
		stopwatch.Stop();
	}


	template <typename Container>
	void TestMoveReallocate(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		stopwatch.Restart();
		while(c.size() < 8192)
			c.resize(c.capacity() + 1);
		stopwatch.Stop();
	}


	template <typename Container>
	void TestMoveErase(EA::StdC::Stopwatch& stopwatch, Container& c)
	{
		stopwatch.Restart();
		while(!c.empty())
			c.erase(c.begin());
		stopwatch.Stop();
	}


} // namespace





void BenchmarkVector()
{
	EASTLTest_Printf("Vector\n");

	EA::UnitTest::RandGenT<uint32_t> rng(EA::UnitTest::GetRandSeed());
	EA::StdC::Stopwatch              stopwatch1(EA::StdC::Stopwatch::kUnitsCPUCycles);
	EA::StdC::Stopwatch              stopwatch2(EA::StdC::Stopwatch::kUnitsCPUCycles);

	{
		eastl::vector<uint32_t> intVector(100000);
		eastl::generate(intVector.begin(), intVector.end(), rng);

		for(int i = 0; i < 2; i++)
		{
			StdVectorUint64 stdVectorUint64;
			EaVectorUint64  eaVectorUint64;


			///////////////////////////////
			// Test push_back
			///////////////////////////////

			TestPushBack(stopwatch1, stdVectorUint64, intVector);
			TestPushBack(stopwatch2, eaVectorUint64,  intVector);

			if(i == 1)
				Benchmark::AddResult("vector<uint64>/push_back", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test operator[].
			///////////////////////////////

			TestBracket(stopwatch1, stdVectorUint64);
			TestBracket(stopwatch2, eaVectorUint64);

			if(i == 1)
				Benchmark::AddResult("vector<uint64>/operator[]", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test iteration via find().
			///////////////////////////////

			TestFind(stopwatch1, stdVectorUint64);
			TestFind(stopwatch2, eaVectorUint64);
			TestFind(stopwatch1, stdVectorUint64);
			TestFind(stopwatch2, eaVectorUint64);

			if(i == 1)
				Benchmark::AddResult("vector<uint64>/iteration", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test sort
			///////////////////////////////

			// Currently VC++ complains about our sort function decrementing std::iterator that is already at begin(). In the strictest sense,
			// that's a valid complaint, but we aren't testing std STL here. We will want to revise our sort function eventually.
			#if !defined(_MSC_VER) || !defined(_ITERATOR_DEBUG_LEVEL) || (_ITERATOR_DEBUG_LEVEL < 2)
				TestSort(stopwatch1, stdVectorUint64);
				TestSort(stopwatch2, eaVectorUint64);

				if(i == 1)
					Benchmark::AddResult("vector<uint64>/sort", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
			#endif

			///////////////////////////////
			// Test insert
			///////////////////////////////

			TestInsert(stopwatch1, stdVectorUint64);
			TestInsert(stopwatch2, eaVectorUint64);

			if(i == 1)
				Benchmark::AddResult("vector<uint64>/insert", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////
			// Test erase
			///////////////////////////////

			TestErase(stopwatch1, stdVectorUint64);
			TestErase(stopwatch2, eaVectorUint64);

			if(i == 1)
				Benchmark::AddResult("vector<uint64>/erase", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////////////////
			// Test move of MovableType
			// Should be much faster with C++11 move.
			///////////////////////////////////////////

			std::vector<MovableType>   stdVectorMovableType;
			eastl::vector<MovableType> eaVectorMovableType;

			TestMoveReallocate(stopwatch1, stdVectorMovableType);
			TestMoveReallocate(stopwatch2, eaVectorMovableType);

			if(i == 1)
				Benchmark::AddResult("vector<MovableType>/reallocate", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			TestMoveErase(stopwatch1, stdVectorMovableType);
			TestMoveErase(stopwatch2, eaVectorMovableType);

			if(i == 1)
				Benchmark::AddResult("vector<MovableType>/erase", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());


			///////////////////////////////////////////
			// Test move of AutoRefCount
			// Should be much faster with C++11 move.
			///////////////////////////////////////////

			std::vector<AutoRefCount<RefCounted> >   stdVectorAutoRefCount;
			eastl::vector<AutoRefCount<RefCounted> > eaVectorAutoRefCount;

			for(size_t a = 0; a < 2048; a++)
			{
				stdVectorAutoRefCount.push_back(AutoRefCount<RefCounted>(new RefCounted));
				eaVectorAutoRefCount.push_back(AutoRefCount<RefCounted>(new RefCounted));
			}

			RefCounted::msAddRefCount  = 0;
			RefCounted::msReleaseCount = 0;
			TestMoveErase(stopwatch1, stdVectorAutoRefCount);
			EASTLTest_Printf("vector<AutoRefCount>/erase std counts: %d %d\n", RefCounted::msAddRefCount, RefCounted::msReleaseCount);

			RefCounted::msAddRefCount  = 0;
			RefCounted::msReleaseCount = 0;
			TestMoveErase(stopwatch2, eaVectorAutoRefCount);
			EASTLTest_Printf("vector<AutoRefCount>/erase EA counts: %d %d\n", RefCounted::msAddRefCount, RefCounted::msReleaseCount);

			if(i == 1)
				Benchmark::AddResult("vector<AutoRefCount>/erase", stopwatch1.GetUnits(), stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime());
		}
	}
}












