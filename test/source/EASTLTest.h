/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTLTEST_H
#define EASTLTEST_H


#include <EABase/eabase.h>
#include <EATest/EATest.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <vector>   // For the STD_STL_TYPE defines below.
#if EASTL_EXCEPTIONS_ENABLED
	#include <stdexcept>
	#include <new>
#endif
EA_RESTORE_ALL_VC_WARNINGS();


int TestAlgorithm();
int TestAllocator();
int TestAny();
int TestArray();
int TestBitVector();
int TestBitset();
int TestCharTraits();
int TestChrono();
int TestCppCXTypeTraits();
int TestDeque();
int TestExtra();
int TestFinally();
int TestFixedFunction();
int TestFixedHash();
int TestFixedList();
int TestFixedMap();
int TestFixedSList();
int TestFixedSet();
int TestFixedString();
int TestFixedTupleVector();
int TestFixedVector();
int TestFunctional();
int TestHash();
int TestHeap();
int TestIntrusiveHash();
int TestIntrusiveList();
int TestIntrusiveSDList();
int TestIntrusiveSList();
int TestIterator();
int TestList();
int TestListMap();
int TestLruCache();
int TestMap();
int TestMemory();
int TestMeta();
int TestNumericLimits();
int TestOptional();
int TestRandom();
int TestRatio();
int TestRingBuffer();
int TestSList();
int TestSegmentedVector();
int TestSet();
int TestSmartPtr();
int TestSort();
int TestSpan();
int TestString();
int TestStringHashMap();
int TestStringMap();
int TestStringView();
int TestTuple();
int TestTupleVector();
int TestTypeTraits();
int TestUtility();
int TestVariant();
int TestVector();
int TestVectorMap();
int TestVectorSet();
int TestAtomicBasic();
int TestAtomicAsm();
int TestBitcast();


// Now enable warnings as desired.
#ifdef _MSC_VER
	#pragma warning(disable: 4324)      // 'struct_name' : structure was padded due to __declspec(align())
  //#pragma warning(disable: 4512)      // 'class' : assignment operator could not be generated
  //#pragma warning(disable: 4100)      // 'identifier' : unreferenced formal parameter
  //#pragma warning(disable: 4706)      // assignment within conditional expression

	#pragma warning(default: 4056)      // Floating-point constant arithmetic generates a result that exceeds the maximum allowable value
	#pragma warning(default: 4061)      // The enumerate has no associated handler in a switch statement
	#pragma warning(default: 4062)      // The enumerate has no associated handler in a switch statement, and there is no default label
	#pragma warning(default: 4191)      // Calling this function through the result pointer may cause your program to crash
	#pragma warning(default: 4217)      // Member template functions cannot be used for copy-assignment or copy-construction
  //#pragma warning(default: 4242)      // 'variable' : conversion from 'type' to 'type', possible loss of data
	#pragma warning(default: 4254)      // 'operator' : conversion from 'type1' to 'type2', possible loss of data
	#pragma warning(default: 4255)      // 'function' : no function prototype given: converting '()' to '(void)'
	#pragma warning(default: 4263)      // 'function' : member function does not override any base class virtual member function
	#pragma warning(default: 4264)      // 'virtual_function' : no override available for virtual member function from base 'class'; function is hidden
	#pragma warning(default: 4287)      // 'operator' : unsigned/negative constant mismatch
	#pragma warning(default: 4289)      // Nonstandard extension used : 'var' : loop control variable declared in the for-loop is used outside the for-loop scope
	#pragma warning(default: 4296)      // 'operator' : expression is always false
	#pragma warning(default: 4302)      // 'conversion' : truncation from 'type 1' to 'type 2'
	#pragma warning(default: 4339)      // 'type' : use of undefined type detected in CLR meta-data - use of this type may lead to a runtime exception
	#pragma warning(default: 4347)      // Behavior change: 'function template' is called instead of 'function'
  //#pragma warning(default: 4514)      // unreferenced inline/local function has been removed
	#pragma warning(default: 4529)      // 'member_name' : forming a pointer-to-member requires explicit use of the address-of operator ('&') and a qualified name
	#pragma warning(default: 4545)      // Expression before comma evaluates to a function which is missing an argument list
	#pragma warning(default: 4546)      // Function call before comma missing argument list
	#pragma warning(default: 4547)      // 'operator' : operator before comma has no effect; expected operator with side-effect
  //#pragma warning(default: 4548)      // expression before comma has no effect; expected expression with side-effect
	#pragma warning(default: 4549)      // 'operator' : operator before comma has no effect; did you intend 'operator'?
	#pragma warning(default: 4536)      // 'type name' : type-name exceeds meta-data limit of 'limit' characters
	#pragma warning(default: 4555)      // Expression has no effect; expected expression with side-effect
	#pragma warning(default: 4557)      // '__assume' contains effect 'effect'
  //#pragma warning(default: 4619)      // #pragma warning : there is no warning number 'number'
	#pragma warning(default: 4623)      // 'derived class' : default constructor could not be generated because a base class default constructor is inaccessible
  //#pragma warning(default: 4625)      // 'derived class' : copy constructor could not be generated because a base class copy constructor is inaccessible
  //#pragma warning(default: 4626)      // 'derived class' : assignment operator could not be generated because a base class assignment operator is inaccessible
	#pragma warning(default: 4628)      // Digraphs not supported with -Ze. Character sequence 'digraph' not interpreted as alternate token for 'char'
	#pragma warning(default: 4640)      // 'instance' : construction of local static object is not thread-safe
	#pragma warning(default: 4668)      // 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
	#pragma warning(default: 4682)      // 'parameter' : no directional parameter attribute specified, defaulting to [in]
	#pragma warning(default: 4686)      // 'user-defined type' : possible change in behavior, change in UDT return calling convention
  //#pragma warning(default: 4710)      // 'function' : function not inlined
  //#pragma warning(default: 4786)      // 'identifier' : identifier was truncated to 'number' characters in the debug information
	#pragma warning(default: 4793)      // Native code generated for function 'function': 'reason'
  //#pragma warning(default: 4820)      // 'bytes' bytes padding added after member 'member'
	#pragma warning(default: 4905)      // Wide string literal cast to 'LPSTR'
	#pragma warning(default: 4906)      // String literal cast to 'LPWSTR'
	#pragma warning(default: 4917)      // 'declarator' : a GUID cannot only be associated with a class, interface or namespace
	#pragma warning(default: 4928)      // Illegal copy-initialization; more than one user-defined conversion has been implicitly applied
	#pragma warning(default: 4931)      // We are assuming the type library was built for number-bit pointers
	#pragma warning(default: 4946)      // reinterpret_cast used between related classes: 'class1' and 'class2'

#endif


///////////////////////////////////////////////////////////////////////////////
// EASTL includes
//
// Intentionally keep these includes below the warning settings specified above.
//
#include <EASTL/iterator.h>
#include <EASTL/algorithm.h>




/// EASTL_TestLevel
///
/// Defines how extensive our testing is. A low level is for a desktop or 
/// nightly build in which the test can run quickly but still hit the 
/// majority of functionality. High level is for heavy testing and internal
/// validation which may take numerous hours to run.
///
enum EASTL_TestLevel
{
	kEASTL_TestLevelLow  = 1,   /// ~10 seconds for test completion.
	kEASTL_TestLevelHigh = 10   /// Numerous hours for test completion.
};

extern int gEASTL_TestLevel;



/// EASTLTest_CheckMemory
///
/// Does a global memory heap validation check. Returns 0 if OK and 
/// an error count if there is a problem. 
///
/// Example usage:
///    EASTLTest_CheckMemory();
///
int EASTLTest_CheckMemory_Imp(const char* pFile, int nLine);
#define EASTLTest_CheckMemory() EASTLTest_CheckMemory_Imp(__FILE__, __LINE__)



// EASTLTEST_STD_STL_VER
//
#if defined(_STLPORT_VERSION)
	#define EASTLTEST_STD_STL_VER_STLPORT
#elif defined(_RWSTD_VER_STR) || defined(_RWSTD_NAMESPACE_END)
	#define EASTLTEST_STD_STL_VER_APACHE
#elif defined(_CPPLIB_VER)
	#define EASTLTEST_STD_STL_VER_DINKUMWARE
#elif defined(__GNUC__) && defined(_CXXCONFIG)
	#define EASTLTEST_STD_STL_VER_GCC
#else
	#define EASTLTEST_STD_STL_VER_UNKNOWN
#endif



/// StdSTLType
///
enum StdSTLType
{
	kSTLUnknown,    // Unknown type
	kSTLPort,       // STLPort. Descendent of the old HP / SGI STL.
	kSTLApache,     // Apache stdcxx (previously RogueWave), which is a descendent of the old HP / SGI STL.
	kSTLClang,      // Clang native. a.k.a. libc++
	kSTLGCC,        // GCC native. a.k.a. libstdc++
	kSTLMS,         // Microsoft. Tweaked version of Dinkumware.
	kSTLDinkumware  // Generic Dinkumware
};

StdSTLType GetStdSTLType();




/// GetStdSTLName
///
/// Returns the name of the std C++ STL available to the current build.
/// The returned value will be one of:
///     "STLPort"
///     "GCC"
///     "VC++"
//      "Apache" // Previously RogueWave
///
const char* GetStdSTLName();


/// gEASTLTest_AllocationCount
///
extern int gEASTLTest_AllocationCount; 
extern int gEASTLTest_TotalAllocationCount; 



// For backwards compatibility:
#define EASTLTest_Printf EA::UnitTest::Report
#define VERIFY           EATEST_VERIFY


///////////////////////////////////////////////////////////////////////////////
/// EASTLTest_Rand
///
/// Implements a basic random number generator for EASTL unit tests. It's not
/// intended to be a robust random number generator (though it is decent), 
/// but rather is present so the unit tests can have a portable random number
/// generator they can rely on being present.
///
/// Example usage:
///    EASTLTest_Rand rng;
///    eastl_size_t   x = rng();                      // Generate value in range of [0, 0xffffffff] (i.e. generate any uint32_t)
///    eastl_ssize_t  y = rng.Rand(1000);             // Generate value in range of [0, 1000)
///    eastl_ssize_t  z = rng.RandRange(-50, +30);    // Generate value in range of [-50, +30)
///
/// Example usage in the random_shuffle algorithm:
///    EASTLTest_Rand rng;
///    random_shuffle(first, last, rnd);
///
class EASTLTest_Rand
{
public:
	EASTLTest_Rand(eastl_size_t nSeed) // The user must supply a seed; we don't provide default values.
		: mnSeed(nSeed) { }

	eastl_size_t Rand()
	{
		// This is not designed to be a high quality random number generator.
		if(mnSeed == 0)
			mnSeed = UINT64_C(0xfefefefefefefefe); // Can't have a seed of zero.

		const uint64_t nResult64A = ((mnSeed     * UINT64_C(6364136223846793005)) + UINT64_C(1442695040888963407));
		const uint64_t nResult64B = ((nResult64A * UINT64_C(6364136223846793005)) + UINT64_C(1442695040888963407));

		mnSeed = (nResult64A >> 32) ^ nResult64B;

		return (eastl_size_t)mnSeed; // For eastl_size_t == uint32_t, this is a chop.
	}

	eastl_size_t operator()() // Returns a pseudorandom value in range of [0, 0xffffffffffffffff)] (i.e. generate any eastl_size_t)
		{ return Rand(); }

	eastl_size_t operator()(eastl_size_t n)  // Returns a pseudorandom value in range of [0, n)
		{ return RandLimit(n); }

	eastl_size_t RandLimit(eastl_size_t nLimit) // Returns a pseudorandom value in range of [0, nLimit)
	{
		// Can't do the following correct solution because we don't have a portable int128_t to work with. 
		// We could implement a 128 bit multiply manually. See EAStdC/int128_t.cpp.
		// return (eastl_size_t)((Rand() * (uint128_t)nLimit) >> 64);

		return (Rand() % nLimit); // This results in an imperfect distribution, especially for the case of nLimit being high relative to eastl_size_t.
	}

	eastl_ssize_t RandRange(eastl_ssize_t nBegin, eastl_ssize_t nEnd)   // Returns a pseudorandom value in range of [nBegin, nEnd)
		{ return nBegin + (eastl_ssize_t)RandLimit((eastl_size_t)(nEnd - nBegin)); }

protected:
	uint64_t mnSeed;
};


///////////////////////////////////////////////////////////////////////////////
/// RandGenT
///
/// A wrapper for EASTLTest_Rand which generates values of the given integral
/// data type. This is mostly useful for clearnly avoiding compiler warnings, 
/// as we intentionally enable the highest warning levels in these tests.
///
template <typename Integer>
struct RandGenT
{
	RandGenT(eastl_size_t nSeed)
		: mRand(nSeed) { }

	Integer operator()()
		{ return (Integer)mRand.Rand(); }

	Integer operator()(eastl_size_t n)
		{ return (Integer)mRand.RandLimit(n); }

	EASTLTest_Rand mRand;
};



///////////////////////////////////////////////////////////////////////////////
/// kMagicValue
///
/// Used as a unique integer. We assign this to TestObject in its constructor
/// and verify in the TestObject destructor that the value is unchanged. 
/// This can be used to tell, for example, if an invalid object is being 
/// destroyed.
///
const uint32_t kMagicValue = 0x01f1cbe8;


///////////////////////////////////////////////////////////////////////////////
/// TestObject
///
/// Implements a generic object that is suitable for use in container tests.
/// Note that we choose a very restricted set of functions that are available
/// for this class. Do not add any additional functions, as that would 
/// compromise the intentions of the unit tests.
///
struct TestObject
{
	int             mX;                  // Value for the TestObject.
	bool            mbThrowOnCopy;       // Throw an exception of this object is copied, moved, or assigned to another.
	int64_t         mId;                 // Unique id for each object, equal to its creation number. This value is not coped from other TestObjects during any operations, including moves.
	uint32_t        mMagicValue;         // Used to verify that an instance is valid and that it is not corrupted. It should always be kMagicValue.
	static int64_t  sTOCount;            // Count of all current existing TestObjects.
	static int64_t  sTOCtorCount;        // Count of times any ctor was called.
	static int64_t  sTODtorCount;        // Count of times dtor was called.
	static int64_t  sTODefaultCtorCount; // Count of times the default ctor was called.
	static int64_t  sTOArgCtorCount;     // Count of times the x0,x1,x2 ctor was called.
	static int64_t  sTOCopyCtorCount;    // Count of times copy ctor was called.
	static int64_t  sTOMoveCtorCount;    // Count of times move ctor was called.
	static int64_t  sTOCopyAssignCount;  // Count of times copy assignment was called.
	static int64_t  sTOMoveAssignCount;  // Count of times move assignment was called.
	static int      sMagicErrorCount;    // Number of magic number mismatch errors.

	explicit TestObject(int x = 0, bool bThrowOnCopy = false)
		: mX(x), mbThrowOnCopy(bThrowOnCopy), mMagicValue(kMagicValue)
	{
		++sTOCount;
		++sTOCtorCount;
		++sTODefaultCtorCount;
		mId = sTOCtorCount;
	}

	// This constructor exists for the purpose of testing variadiac template arguments, such as with the emplace container functions.
	TestObject(int x0, int x1, int x2, bool bThrowOnCopy = false)
		: mX(x0 + x1 + x2), mbThrowOnCopy(bThrowOnCopy), mMagicValue(kMagicValue)
	{
		++sTOCount;
		++sTOCtorCount;
		++sTOArgCtorCount;
		mId = sTOCtorCount;
	}

	TestObject(const TestObject& testObject)
		: mX(testObject.mX), mbThrowOnCopy(testObject.mbThrowOnCopy), mMagicValue(testObject.mMagicValue)
	{
		++sTOCount;
		++sTOCtorCount;
		++sTOCopyCtorCount;
		mId = sTOCtorCount;
		if(mbThrowOnCopy)
		{
			#if EASTL_EXCEPTIONS_ENABLED
				throw "Disallowed TestObject copy";
			#endif
		}
	}

	// Due to the nature of TestObject, there isn't much special for us to 
	// do in our move constructor. A move constructor swaps its contents with 
	// the other object, whhich is often a default-constructed object.
	TestObject(TestObject&& testObject)
		: mX(testObject.mX), mbThrowOnCopy(testObject.mbThrowOnCopy), mMagicValue(testObject.mMagicValue)
	{
		++sTOCount;
		++sTOCtorCount;
		++sTOMoveCtorCount;
		mId = sTOCtorCount;  // testObject keeps its mId, and we assign ours anew.
		testObject.mX = 0;   // We are swapping our contents with the TestObject, so give it our "previous" value.
		if(mbThrowOnCopy)
		{
			#if EASTL_EXCEPTIONS_ENABLED
				throw "Disallowed TestObject copy";
			#endif
		}
	}

	TestObject& operator=(const TestObject& testObject)
	{
		++sTOCopyAssignCount;

		if(&testObject != this)
		{
			mX = testObject.mX;
			// Leave mId alone.
			mMagicValue = testObject.mMagicValue;
			mbThrowOnCopy = testObject.mbThrowOnCopy;
			if(mbThrowOnCopy)
			{
				#if EASTL_EXCEPTIONS_ENABLED
					throw "Disallowed TestObject copy";
				#endif
			}
		}
		return *this;
	}

	TestObject& operator=(TestObject&& testObject)
	{
		++sTOMoveAssignCount;

		if(&testObject != this)
		{
			eastl::swap(mX, testObject.mX);
			// Leave mId alone.
			eastl::swap(mMagicValue, testObject.mMagicValue);
			eastl::swap(mbThrowOnCopy, testObject.mbThrowOnCopy);

			if(mbThrowOnCopy)
			{
				#if EASTL_EXCEPTIONS_ENABLED
					throw "Disallowed TestObject copy";
				#endif
			}
		}
		return *this;
	}

	~TestObject()
	{
		if(mMagicValue != kMagicValue)
			++sMagicErrorCount;
		mMagicValue = 0;
		--sTOCount;
		++sTODtorCount;
	}

	static void Reset()
	{
		sTOCount            = 0;
		sTOCtorCount        = 0;
		sTODtorCount        = 0;
		sTODefaultCtorCount = 0;
		sTOArgCtorCount     = 0;
		sTOCopyCtorCount    = 0;
		sTOMoveCtorCount    = 0;
		sTOCopyAssignCount  = 0;
		sTOMoveAssignCount  = 0;
		sMagicErrorCount    = 0;
	}

	static bool IsClear() // Returns true if there are no existing TestObjects and the sanity checks related to that test OK.
	{
		return (sTOCount == 0) && (sTODtorCount == sTOCtorCount) && (sMagicErrorCount == 0);
	}

	TestObject& operator++()
	{
		++mX;
		return *this;
	}

	TestObject operator++(int) const
	{
		TestObject temp(*this);
		++temp;
		return temp;
	}
};

// Operators
// We specifically define only == and <, in order to verify that 
// our containers and algorithms are not mistakenly expecting other 
// operators for the contained and manipulated classes.
inline bool operator==(const TestObject& t1, const TestObject& t2)
	{ return t1.mX == t2.mX; }

inline bool operator<(const TestObject& t1, const TestObject& t2)
	{ return t1.mX < t2.mX; }


// TestObject hash
// Normally you don't want to put your hash functions in the eastl namespace, as that namespace is owned by EASTL.
// However, these are the EASTL unit tests and we can say that they are also owned by EASTL.
namespace eastl
{
	template <> 
	struct hash<TestObject>
	{
		size_t operator()(const TestObject& a) const 
			{ return static_cast<size_t>(a.mX); }
	};
}


// use_mX
// Used for printing TestObject contents via the PrintSequence function,
// which is defined below. See the PrintSequence function for documentation.
// This function is an analog of the eastl::use_self and use_first functions.
// We declare this all in one line because the user should never need to 
// debug usage of this function.
template <typename T> struct use_mX { int operator()(const T& t) const { return t.mX; } };




///////////////////////////////////////////////////////////////////////////////
/// TestObjectHash
///
/// Implements a manually specified hash function for TestObjects.
///
struct TestObjectHash
{
	size_t operator()(const TestObject& t) const 
	{
		return (size_t)t.mX;
	}
};





///////////////////////////////////////////////////////////////////////////////
/// Align16
///

#if defined(EA_PROCESSOR_ARM)
	#define kEASTLTestAlign16 8 //ARM processors can only align to 8 
#else
	#define kEASTLTestAlign16 16
#endif


EA_PREFIX_ALIGN(kEASTLTestAlign16)
struct Align16
{
	explicit Align16(int x = 0) : mX(x) {}
	int mX;
} EA_POSTFIX_ALIGN(kEASTLTestAlign16);

inline bool operator==(const Align16& a, const Align16& b)
	{ return (a.mX == b.mX); }

inline bool operator<(const Align16& a, const Align16& b)
	{ return (a.mX < b.mX); }



///////////////////////////////////////////////////////////////////////////////
/// Align32
///
#if defined(EA_PROCESSOR_ARM)
	#define kEASTLTestAlign32 8 //ARM processors can only align to 8 
#elif defined(__GNUC__) && (((__GNUC__ * 100) + __GNUC_MINOR__) < 400) // GCC 2.x, 3.x
	#define kEASTLTestAlign32 16 // Some versions of GCC fail to support any alignment beyond 16.
#else
	#define kEASTLTestAlign32 32
#endif

EA_PREFIX_ALIGN(kEASTLTestAlign32)
struct Align32
{
	explicit Align32(int x = 0) : mX(x) {}
	int mX;
} EA_POSTFIX_ALIGN(kEASTLTestAlign32);

inline bool operator==(const Align32& a, const Align32& b)
	{ return (a.mX == b.mX); }

inline bool operator<(const Align32& a, const Align32& b)
	{ return (a.mX < b.mX); }



///////////////////////////////////////////////////////////////////////////////
/// Align64
///
/// Used for testing of alignment.
///
#if defined(EA_PROCESSOR_ARM)
	#define kEASTLTestAlign64 8
#elif defined(__GNUC__) && (((__GNUC__ * 100) + __GNUC_MINOR__) < 400) // GCC 2.x, 3.x
	#define kEASTLTestAlign64 16 // Some versions of GCC fail to support any alignment beyond 16.
#else
	#define kEASTLTestAlign64 64
#endif

EA_PREFIX_ALIGN(kEASTLTestAlign64)
struct Align64
{
	explicit Align64(int x = 0) : mX(x) {}
	int mX;
} EA_POSTFIX_ALIGN(kEASTLTestAlign64);

inline bool operator==(const Align64& a, const Align64& b)
	{ return (a.mX == b.mX); }

inline bool operator<(const Align64& a, const Align64& b)
	{ return (a.mX < b.mX); }

namespace eastl
{
	template <>
	struct hash < Align64 >
	{
		size_t operator()(const Align64& a) const
		{
			return static_cast<size_t>(a.mX);
		}
	};
}





/// test_use_self
///
/// Intentionally avoiding a dependency on eastl::use_self.
///
template <typename T>
struct test_use_self
{
	const T& operator()(const T& x) const
		{ return x; }
};



/// GenerateIncrementalIntegers
///
/// Used to seed containers with incremental values based on integers.
///
/// Example usage:
///     vector<int> v(10, 0);
///     generate(v.begin(), v.end(), GenerateIncrementalIntegers<int>());
///     // v will now have 0, 1, 2, ... 8, 9.
/// 
///     generate_n(intArray.begin(), 10, GenerateIncrementalIntegers<int>());
///     // v will now have 0, 1, 2, ... 8, 9.
///
///     vector<TestObject> vTO(10, 0);
///     generate(vTO.begin(), vTO.end(), GenerateIncrementalIntegers<TestObject>());
///     // vTO will now have 0, 1, 2, ... 8, 9.
/// 
template <typename T>
struct GenerateIncrementalIntegers
{
	int mX;

	GenerateIncrementalIntegers(int x = 0)
		: mX(x) { }

	void reset(int x = 0)
		{ mX = x; }

	T operator()()
		{ return T(mX++); } 
};



/// SetIncrementalIntegers
///
/// Used to seed containers with incremental values based on integers.
///
/// Example usage:
///     vector<int> v(10, 0);
///     for_each(v.begin(), v.end(), SetIncrementalIntegers<int>());
///     // v will now have 0, 1, 2, ... 8, 9.
/// 
template <typename T>
struct SetIncrementalIntegers
{
	int mX;

	SetIncrementalIntegers(int x = 0)
		: mX(x) { }

	void reset(int x = 0)
		{ mX = x; }

	void operator()(T& t)
		{ t = T(mX++); } 
};



/// CompareContainers
///
/// Does a comparison between the contents of two containers. 
///
/// Specifically tests for the following properties:
///     empty() is the same for both
///     size() is the same for both
///     iteration through both element by element yields equal values.
///
template <typename T1, typename T2, typename ExtractValue1, typename ExtractValue2>
int CompareContainers(const T1& t1, const T2& t2, const char* ppName, 
					  ExtractValue1 ev1 = test_use_self<T1>(), ExtractValue2 ev2 = test_use_self<T2>())
{
	int nErrorCount = 0;

	// Compare emptiness.
	VERIFY(t1.empty() == t2.empty());

	// Compare sizes.
	const size_t nSize1 = t1.size();
	const size_t nSize2 = t2.size();

	VERIFY(nSize1 == nSize2);
	if(nSize1 != nSize2)
		EASTLTest_Printf("%s: Container size difference: %u, %u\n", ppName, (unsigned)nSize1, (unsigned)nSize2);

	// Compare values.
	if(nSize1 == nSize2)
	{
		// Test iteration
		typename T1::const_iterator it1 = t1.begin();
		typename T2::const_iterator it2 = t2.begin();

		for(unsigned j = 0; it1 != t1.end(); ++it1, ++it2, ++j)
		{
			const typename T1::value_type& v1 = *it1;
			const typename T2::value_type& v2 = *it2;

			VERIFY(ev1(v1) == ev2(v2));
			if(!(ev1(v1) == ev2(v2)))
			{
				EASTLTest_Printf("%s: Container iterator difference at index %d\n", ppName, j);
				break;  
			}
		}

		VERIFY(it1 == t1.end());
		VERIFY(it2 == t2.end());
	}

	return nErrorCount;
}


template <typename InputIterator1, typename InputIterator2>
bool VerifySequence(InputIterator1 firstActual, InputIterator1 lastActual, InputIterator2 firstExpected, InputIterator2 lastExpected, const char* pName)
{
	size_t     numMatching = 0;
	
	while ((firstActual != lastActual) && (firstExpected != lastExpected) && (*firstActual == *firstExpected))
	{
		++firstActual;
		++firstExpected;
		++numMatching;
	}

	if (firstActual == lastActual && firstExpected == lastExpected)
	{
		return true;
	}
	else if (firstActual != lastActual && firstExpected == lastExpected)
	{
		size_t numActual = numMatching, numExpected = numMatching;
		for (; firstActual != lastActual; ++firstActual)
			++numActual;
		if (pName)
			EASTLTest_Printf("[%s] Too many elements: expected %u, found %u\n", pName, numExpected, numActual);
		else
			EASTLTest_Printf("Too many elements: expected %u, found %u\n", numExpected, numActual);
		return false;
	}
	else if (firstActual == lastActual && firstExpected != lastExpected)
	{
		size_t numActual = numMatching, numExpected = numMatching;
		for (; firstExpected != lastExpected; ++firstExpected)
			++numExpected;
		if (pName)
			EASTLTest_Printf("[%s] Too few elements: expected %u, found %u\n", pName, numExpected, numActual);
		else
			EASTLTest_Printf("Too few elements: expected %u, found %u\n", numExpected, numActual);
		return false;
	}
	else // if (firstActual != lastActual && firstExpected != lastExpected)
	{
		if (pName)
			EASTLTest_Printf("[%s] Mismatch at index %u\n", pName, numMatching);
		else
			EASTLTest_Printf("Mismatch at index %u\n", numMatching);
		return false;
	}
}

template <typename InputIterator, typename T = typename InputIterator::value_type>
bool VerifySequence(InputIterator firstActual, InputIterator lastActual, std::initializer_list<T> initList, const char* pName)
{
	return VerifySequence(firstActual, lastActual, initList.begin(), initList.end(), pName);
}

template <typename Container, typename T = typename Container::value_type>
bool VerifySequence(const Container& container, std::initializer_list<T> initList, const char* pName)
{
	return VerifySequence(container.begin(), container.end(), initList.begin(), initList.end(), pName);
}


/// VerifySequence
///
/// Allows the user to specify that a container has a given set of values.
/// 
/// Example usage:
///    vector<int> v;
///    v.push_back(1); v.push_back(3); v.push_back(5);
///    VerifySequence(v.begin(), v.end(), int(), "v.push_back", 1, 3, 5, -1);
///
/// Note: The StackValue template argument is a hint to the compiler about what type 
///       the passed vararg sequence is.
///
template <typename InputIterator, typename StackValue>
bool VerifySequence(InputIterator first, InputIterator last, StackValue /*unused*/, const char* pName, ...)
{
	typedef typename eastl::iterator_traits<InputIterator>::value_type value_type;

	int        argIndex = 0;
	int        seqIndex = 0;
	bool       bReturnValue = true;
	StackValue next;

	va_list args;
	va_start(args, pName);

	for( ; first != last; ++first, ++argIndex, ++seqIndex)
	{
		next = va_arg(args, StackValue);

		if((next == StackValue(-1)) || !(value_type(next) == *first))
		{
			if(pName)
				EASTLTest_Printf("[%s] Mismatch at index %d\n", pName, argIndex);
			else
				EASTLTest_Printf("Mismatch at index %d\n", argIndex);
			bReturnValue = false;
		}
	}

	for(; first != last; ++first)
		++seqIndex;

	if(bReturnValue)
	{
		next = va_arg(args, StackValue);

		if(!(next == StackValue(-1)))
		{
			do {
				++argIndex;
				next = va_arg(args, StackValue);
			} while(!(next == StackValue(-1)));

			if(pName)
				EASTLTest_Printf("[%s] Too many elements: expected %d, found %d\n", pName, argIndex, seqIndex);
			else
				EASTLTest_Printf("Too many elements: expected %d, found %d\n", argIndex, seqIndex);
			bReturnValue = false;
		}
	}

	va_end(args);

	return bReturnValue;
}




/// PrintSequence
///
/// Allows the user to print a sequence of values.
/// 
/// Example usage:
///    vector<int> v;
///    PrintSequence(v.begin(), v.end(), use_self<int>(), 100, "vector", 1, 3, 5, -1);
///
/// Example usage:
///    template <typename T> struct use_mX { int operator()(const T& t) const { return t.mX; } };
///    vector<TestObject> v;
///    PrintSequence(v.begin(), v.end(), use_mX<TestObject>(), 100, "vector", 1, 3, 5, -1);
///    
template <typename InputIterator, typename ExtractInt>
void PrintSequence(InputIterator first, InputIterator last, ExtractInt extractInt, int nMaxCount, const char* pName, ...)
{
	if(pName)
		EASTLTest_Printf("[%s]", pName);

	for(int i = 0; (i < nMaxCount) && (first != last); ++i, ++first)
	{
		EASTLTest_Printf("%d ", (int)extractInt(*first));
	}

	EASTLTest_Printf("\n");
}




/// demoted_iterator
///
/// Converts an iterator into a demoted category. For example, you can convert
/// an iterator of type bidirectional_iterator_tag to forward_iterator_tag.
/// The following is a list of iterator types. A demonted iterator can be demoted
/// only to a lower iterator category (earlier in the following list):
///     input_iterator_tag
///     forward_iterator_tag
///     bidirectional_iterator_tag
///     random_access_iterator_tag
///     contiguous_iterator_tag
///
/// Converts something which can be iterated into a formal input iterator.
/// This class is useful for testing functions and algorithms that expect
/// InputIterators, which are the lowest and 'weakest' form of iterators.
/// 
/// Key traits of InputIterators:
///    Algorithms on input iterators should never attempt to pass
///    through the same iterator twice. They should be single pass 
///    algorithms. value_type T is not required to be an lvalue type.
///
/// Example usage:
///     typedef demoted_iterator<int*, eastl::bidirectional_iterator_tag>         PointerAsBidirectionalIterator;
///     typedef demoted_iterator<MyVector::iterator, eastl::forward_iterator_tag> VectorIteratorAsForwardIterator;
///
/// Example usage:
///     IntVector v;
///     comb_sort(to_forward_iterator(v.begin()), to_forward_iterator(v.end()));
///
template <typename Iterator, typename IteratorCategory>
class demoted_iterator
{
protected:
	Iterator mIterator;

public:
	typedef demoted_iterator<Iterator, IteratorCategory>                 this_type;
	typedef Iterator                                                     iterator_type;
	typedef IteratorCategory                                             iterator_category;
	typedef typename eastl::iterator_traits<Iterator>::value_type        value_type;
	typedef typename eastl::iterator_traits<Iterator>::difference_type   difference_type;
	typedef typename eastl::iterator_traits<Iterator>::reference         reference;
	typedef typename eastl::iterator_traits<Iterator>::pointer           pointer;

	demoted_iterator()
		: mIterator() { }

	explicit demoted_iterator(const Iterator& i)
		: mIterator(i) { }

	demoted_iterator(const this_type& x)
		: mIterator(x.mIterator) { }

	this_type& operator=(const Iterator& i)
		{ mIterator = i; return *this; }

	this_type& operator=(const this_type& x)
		{ mIterator = x.mIterator; return *this; }

	reference operator*() const
		{ return *mIterator; }

	pointer operator->() const
		{ return mIterator; }

	this_type& operator++()
		{ ++mIterator; return *this; }

	this_type operator++(int)
		{ return this_type(mIterator++); }

	this_type& operator--()
		{ --mIterator; return *this; }

	this_type operator--(int)
		{ return this_type(mIterator--); }

	reference operator[](const difference_type& n) const
		{ return mIterator[n]; }

	this_type& operator+=(const difference_type& n)
		{ mIterator += n; return *this; }

	this_type operator+(const difference_type& n) const
		{ return this_type(mIterator + n); }

	this_type& operator-=(const difference_type& n)
		{ mIterator -= n; return *this; }

	this_type operator-(const difference_type& n) const
		{ return this_type(mIterator - n); }

	const iterator_type& base() const
		{ return mIterator; }

}; // class demoted_iterator

template<typename Iterator1, typename IteratorCategory1, typename Iterator2, typename IteratorCategory2>
inline bool
operator==(const demoted_iterator<Iterator1, IteratorCategory1>& a, const demoted_iterator<Iterator2, IteratorCategory2>& b)
	{ return a.base() == b.base(); }

template<typename Iterator1, typename IteratorCategory1, typename Iterator2, typename IteratorCategory2>
inline bool
operator!=(const demoted_iterator<Iterator1, IteratorCategory1>& a, const demoted_iterator<Iterator2, IteratorCategory2>& b)
	{ return !(a == b); }

template<typename Iterator1, typename IteratorCategory1, typename Iterator2, typename IteratorCategory2>
inline bool
operator<(const demoted_iterator<Iterator1, IteratorCategory1>& a, const demoted_iterator<Iterator2, IteratorCategory2>& b)
	{ return a.base() < b.base(); }

template<typename Iterator1, typename IteratorCategory1, typename Iterator2, typename IteratorCategory2>
inline bool
operator<=(const demoted_iterator<Iterator1, IteratorCategory1>& a, const demoted_iterator<Iterator2, IteratorCategory2>& b)
	{ return !(b < a); }

template<typename Iterator1, typename IteratorCategory1, typename Iterator2, typename IteratorCategory2>
inline bool
operator>(const demoted_iterator<Iterator1, IteratorCategory1>& a, const demoted_iterator<Iterator2, IteratorCategory2>& b)
	{ return b < a; }

template<typename Iterator1, typename IteratorCategory1, typename Iterator2, typename IteratorCategory2>
inline bool
operator>=(const demoted_iterator<Iterator1, IteratorCategory1>& a, const demoted_iterator<Iterator2, IteratorCategory2>& b)
	{ return !(a < b); }

template<typename Iterator1, typename IteratorCategory1, typename Iterator2, typename IteratorCategory2>
inline demoted_iterator<Iterator1, IteratorCategory1>
operator-(const demoted_iterator<Iterator1, IteratorCategory1>& a, const demoted_iterator<Iterator2, IteratorCategory2>& b)
	{ return demoted_iterator<Iterator1, IteratorCategory1>(a.base() - b.base()); }

template<typename Iterator1, typename IteratorCategory1>
inline demoted_iterator<Iterator1, IteratorCategory1>
operator+(typename demoted_iterator<Iterator1, IteratorCategory1>::difference_type n, const demoted_iterator<Iterator1, IteratorCategory1>& a)
	{ return a + n; }


// to_xxx_iterator
//
// Returns a demoted iterator
//
template <typename Iterator>
inline demoted_iterator<Iterator, EASTL_ITC_NS::input_iterator_tag>
to_input_iterator(const Iterator& i)
	{ return demoted_iterator<Iterator, EASTL_ITC_NS::input_iterator_tag>(i); }

template <typename Iterator>
inline demoted_iterator<Iterator, EASTL_ITC_NS::forward_iterator_tag>
to_forward_iterator(const Iterator& i)
	{ return demoted_iterator<Iterator, EASTL_ITC_NS::forward_iterator_tag>(i); }

template <typename Iterator>
inline demoted_iterator<Iterator, EASTL_ITC_NS::bidirectional_iterator_tag>
to_bidirectional_iterator(const Iterator& i)
	{ return demoted_iterator<Iterator, EASTL_ITC_NS::bidirectional_iterator_tag>(i); }

template <typename Iterator>
inline demoted_iterator<Iterator, EASTL_ITC_NS::random_access_iterator_tag>
to_random_access_iterator(const Iterator& i)
	{ return demoted_iterator<Iterator, EASTL_ITC_NS::random_access_iterator_tag>(i); }






///////////////////////////////////////////////////////////////////////////////
// MallocAllocator
//
// Implements an EASTL allocator that uses malloc/free as opposed to 
// new/delete or PPMalloc Malloc/Free. This is useful for testing 
// allocator behaviour of code.
//
// Example usage:
//      vector<int, MallocAllocator> intVector;
//
class MallocAllocator
{
public:
	MallocAllocator(const char* = EASTL_NAME_VAL("MallocAllocator"))
		: mAllocCount(0), mFreeCount(0), mAllocVolume(0) {}

	MallocAllocator(const MallocAllocator& x)
		: mAllocCount(x.mAllocCount), mFreeCount(x.mFreeCount), mAllocVolume(x.mAllocVolume) {}

	MallocAllocator(const MallocAllocator& x, const char*) : MallocAllocator(x) {}

	MallocAllocator& operator=(const MallocAllocator& x)
	{
		mAllocCount = x.mAllocCount;
		mFreeCount = x.mFreeCount;
		mAllocVolume = x.mAllocVolume;
		return *this;
	}

	void* allocate(size_t n, int = 0);
	void* allocate(size_t n, size_t, size_t, int = 0); // We don't support alignment, so you can't use this class where alignment is required.
	void deallocate(void* p, size_t n);

	const char* get_name() const { return "MallocAllocator"; }
	void set_name(const char*) {}

	static void reset_all()
	{
		mAllocCountAll = 0;
		mFreeCountAll = 0;
		mAllocVolumeAll = 0;
		mpLastAllocation = NULL;
	}

public:
	int mAllocCount;
	int mFreeCount;
	size_t mAllocVolume;

	static int mAllocCountAll;
	static int mFreeCountAll;
	static size_t mAllocVolumeAll;
	static void* mpLastAllocation;
};

inline bool operator==(const MallocAllocator&, const MallocAllocator&) { return true; }
inline bool operator!=(const MallocAllocator&, const MallocAllocator&) { return false; }


///////////////////////////////////////////////////////////////////////////////
// CustomAllocator
//
// Implements an allocator that works just like eastl::allocator but is defined
// within this test as opposed to within EASTL.
//
// Example usage:
//      vector<int, CustomAllocator> intVector;
//
class CustomAllocator
{
public:
	CustomAllocator(const char* = NULL) {}
	CustomAllocator(const CustomAllocator&) {}
	CustomAllocator(const CustomAllocator&, const char*) {}
	CustomAllocator& operator=(const CustomAllocator&) { return *this; }

	void* allocate(size_t n, int flags = 0);
	void* allocate(size_t n, size_t, size_t, int flags = 0);
	void deallocate(void* p, size_t n);

	const char* get_name() const { return "CustomAllocator"; }
	void set_name(const char*) {}
};

inline bool operator==(const CustomAllocator&, const CustomAllocator&) { return true; }
inline bool operator!=(const CustomAllocator&, const CustomAllocator&) { return false; }


///////////////////////////////////////////////////////////////////////////////
/// UnequalAllocator
///
/// Acts the same as eastl::allocator, but always compares as unequal to an 
/// instance of itself.
///
class UnequalAllocator
{
public:
	EASTL_ALLOCATOR_EXPLICIT UnequalAllocator(const char* pName = EASTL_NAME_VAL(EASTL_ALLOCATOR_DEFAULT_NAME))
	    : mAllocator(pName) {}

	UnequalAllocator(const UnequalAllocator& x) : mAllocator(x.mAllocator) {}
	UnequalAllocator(const UnequalAllocator& x, const char* pName) : mAllocator(x.mAllocator) { set_name(pName); }
	UnequalAllocator& operator=(const UnequalAllocator& x)
	{
		mAllocator = x.mAllocator;
		return *this;
	}

	void* allocate(size_t n, int flags = 0) { return mAllocator.allocate(n, flags); }
	void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0) { return mAllocator.allocate(n, alignment, offset, flags); }
	void deallocate(void* p, size_t n) { return mAllocator.deallocate(p, n); }

	const char* get_name() const { return mAllocator.get_name(); }
	void set_name(const char* pName) { mAllocator.set_name(pName); }

protected:
	eastl::allocator mAllocator;
};

inline bool operator==(const UnequalAllocator&, const UnequalAllocator&) { return false; }
inline bool operator!=(const UnequalAllocator&, const UnequalAllocator&) { return true; }


///////////////////////////////////////////////////////////////////////////////
/// CountingAllocator
///
/// Counts allocation events allowing unit tests to validate assumptions.
///
class CountingAllocator : public eastl::allocator
{
public:
	using base_type = eastl::allocator;

	EASTL_ALLOCATOR_EXPLICIT CountingAllocator(const char* pName = EASTL_NAME_VAL(EASTL_ALLOCATOR_DEFAULT_NAME))
	    : base_type(pName)
	{
		totalCtorCount++;
		defaultCtorCount++;
	}

	CountingAllocator(const CountingAllocator& x) : base_type(x)
	{
		totalCtorCount++;
		copyCtorCount++;
	}

	CountingAllocator(const CountingAllocator& x, const char* pName) : base_type(x)
	{
		totalCtorCount++;
		copyCtorCount++;
		set_name(pName);
	}

	CountingAllocator& operator=(const CountingAllocator& x)
	{
		base_type::operator=(x);
		assignOpCount++;
		return *this;
	}

	virtual void* allocate(size_t n, int flags = 0)
	{
		activeAllocCount++;
		totalAllocCount++;
		totalAllocatedMemory += n;
		activeAllocatedMemory += n;
		return base_type::allocate(n, flags);
	}

	virtual void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0)
	{
		activeAllocCount++;
		totalAllocCount++;
		totalAllocatedMemory += n;
		activeAllocatedMemory += n;
		return base_type::allocate(n, alignment, offset, flags);
	}

	void deallocate(void* p, size_t n)
	{
		activeAllocCount--;
		totalDeallocCount--;
		activeAllocatedMemory -= n;
		return base_type::deallocate(p, n);
	}

	const char* get_name() const          { return base_type::get_name(); }
	void set_name(const char* pName)      { base_type::set_name(pName); }

	static auto getTotalAllocationCount()  { return totalAllocCount; }
	static auto getTotalAllocationSize()   { return totalAllocatedMemory; }
	static auto getActiveAllocationSize()  { return activeAllocatedMemory; }
	static auto getActiveAllocationCount() { return activeAllocCount; }
	static auto neverUsed()				   { return totalAllocCount == 0; }

	static void resetCount()
	{
		activeAllocCount      = 0;
		totalAllocCount       = 0;
		totalDeallocCount     = 0;
		totalCtorCount        = 0;
		defaultCtorCount      = 0;
		copyCtorCount         = 0;
		assignOpCount         = 0;
		totalAllocatedMemory  = 0;
		activeAllocatedMemory = 0;
	}

	virtual ~CountingAllocator() = default;

	static uint64_t activeAllocCount;
	static uint64_t totalAllocCount;
	static uint64_t totalDeallocCount;
	static uint64_t totalCtorCount;
	static uint64_t defaultCtorCount;
	static uint64_t copyCtorCount;
	static uint64_t assignOpCount;
	static uint64_t totalAllocatedMemory;  // the total amount of memory allocated
	static uint64_t activeAllocatedMemory; // currently allocated memory by allocator
};

inline bool operator==(const CountingAllocator& rhs, const CountingAllocator& lhs) { return operator==(CountingAllocator::base_type(rhs), CountingAllocator::base_type(lhs)); }
inline bool operator!=(const CountingAllocator& rhs, const CountingAllocator& lhs) { return !(rhs == lhs); }




///////////////////////////////////////////////////////////////////////////////
// InstanceAllocator
//
// Implements an allocator which has a instance id that makes it different
// from other InstanceAllocators of a different id. Allocations between 
// InstanceAllocators of different ids are incompatible. An allocation done 
// by an InstanceAllocator of id=0 cannot be freed by an InstanceAllocator
// of id=1.
//
// Example usage:
//         InstanceAllocator ia0((uint8_t)0);
//         InstanceAllocator ia1((uint8_t)1);
// 
//         eastl::list<int, InstanceAllocator> list0(1, ia0);
//         eastl::list<int, InstanceAllocator> list1(1, ia1);
// 
//         list0 = list1; // list0 cannot free it's current contents with list1's allocator, and InstanceAllocator's purpose is to detect if it mistakenly does so.
//
class InstanceAllocator
{
public:
	enum
	{
		kMultiplier = 16
	}; // Use 16 because it's the highest currently known platform alignment requirement.

	InstanceAllocator(const char* = NULL, uint8_t instanceId = 0) : mInstanceId(instanceId) {}
	InstanceAllocator(uint8_t instanceId) : mInstanceId(instanceId) {}
	InstanceAllocator(const InstanceAllocator& x) : mInstanceId(x.mInstanceId) {}
	InstanceAllocator(const InstanceAllocator& x, const char*) : mInstanceId(x.mInstanceId) {}

	InstanceAllocator& operator=(const InstanceAllocator& x)
	{
		mInstanceId = x.mInstanceId;
		return *this;
	}

	void* allocate(size_t n, int = 0)
	{ // +1 so that we always have space to write mInstanceId.
		uint8_t* p8 =
		    static_cast<uint8_t*>(malloc(n + (kMultiplier * (mInstanceId + 1)))); // We make allocations between
		                                                                          // different instances incompatible by
		                                                                          // tweaking their return values.
		eastl::fill(p8, p8 + kMultiplier, 0xff);
		EA_ANALYSIS_ASSUME(p8 != NULL);
		*p8 = mInstanceId;
		return p8 + (kMultiplier * (mInstanceId + 1));
	}

	void* allocate(size_t n, size_t, size_t, int = 0)
	{ // +1 so that we always have space to write mInstanceId.
		uint8_t* p8 =
		    static_cast<uint8_t*>(malloc(n + (kMultiplier * (mInstanceId + 1)))); // We make allocations between
		                                                                          // different instances incompatible by
		                                                                          // tweaking their return values.
		eastl::fill(p8, p8 + kMultiplier, 0xff);
		EA_ANALYSIS_ASSUME(p8 != NULL);
		*p8 = mInstanceId;
		return p8 + (kMultiplier * (mInstanceId + 1));
	}

	void deallocate(void* p, size_t /*n*/)
	{
		uint8_t* p8 = static_cast<uint8_t*>(p) - (kMultiplier * (mInstanceId + 1));
		EASTL_ASSERT(*p8 == mInstanceId); // mInstanceId must match the id used in allocate(), otherwise the behavior is
		                                  // undefined (probably a heap assert).
		if (*p8 == mInstanceId) // It's possible that *p8 coincidentally matches mInstanceId if p8 is offset into memory
		                        // we don't control.
			free(p8);
		else
			++mMismatchCount;
	}

	const char* get_name()
	{
		sprintf(mName, "InstanceAllocator %u", mInstanceId);
		return mName;
	}

	void set_name(const char*) {}

	static void reset_all() { mMismatchCount = 0; }

public:
	uint8_t mInstanceId;
	char mName[32];

	static int mMismatchCount;
};

inline bool operator==(const InstanceAllocator& a, const InstanceAllocator& b) { return (a.mInstanceId == b.mInstanceId); }
inline bool operator!=(const InstanceAllocator& a, const InstanceAllocator& b) { return (a.mInstanceId != b.mInstanceId); }


///////////////////////////////////////////////////////////////////////////////
// ThrowingAllocator
//
// Implements an EASTL allocator that uses malloc/free as opposed to 
// new/delete or PPMalloc Malloc/Free. This is useful for testing 
// allocator behaviour of code.
//
// Example usage:
//      vector<int, ThrowingAllocator< false<> > intVector;
//
template <bool initialShouldThrow = true>
class ThrowingAllocator
{
public:
	ThrowingAllocator(const char* = EASTL_NAME_VAL("ThrowingAllocator")) : mbShouldThrow(initialShouldThrow) {}
	ThrowingAllocator(const ThrowingAllocator& x) : mbShouldThrow(x.mbShouldThrow) {}
	ThrowingAllocator(const ThrowingAllocator& x, const char*) : mbShouldThrow(x.mbShouldThrow) {}

	ThrowingAllocator& operator=(const ThrowingAllocator& x)
	{
		mbShouldThrow = x.mbShouldThrow;
		return *this;
	}

	void* allocate(size_t n, int = 0)
	{
#if EASTL_EXCEPTIONS_ENABLED
		if (mbShouldThrow)
			throw std::bad_alloc();
#endif
		return malloc(n);
	}

	void* allocate(size_t n, size_t, size_t, int = 0)
	{
#if EASTL_EXCEPTIONS_ENABLED
		if (mbShouldThrow)
			throw std::bad_alloc();
#endif
		return malloc(n); // We don't support alignment, so you can't use this class where alignment is required.
	}

	void deallocate(void* p, size_t) { free(p); }

	const char* get_name() const { return "ThrowingAllocator"; }
	void set_name(const char*) {}

	void set_should_throw(bool shouldThrow) { mbShouldThrow = shouldThrow; }
	bool get_should_throw() const { return mbShouldThrow; }

protected:
	bool mbShouldThrow;
};

template <bool initialShouldThrow>
inline bool operator==(const ThrowingAllocator<initialShouldThrow>&, const ThrowingAllocator<initialShouldThrow>&)
{
	return true;
}

template <bool initialShouldThrow>
inline bool operator!=(const ThrowingAllocator<initialShouldThrow>&, const ThrowingAllocator<initialShouldThrow>&)
{
	return false;
}


///////////////////////////////////////////////////////////////////////////////
// Helper utility that does a case insensitive string comparsion with two sets of overloads
//
struct TestStrCmpI_2
{
	bool operator()(const char* pCStr, const eastl::string& str) const { return str.comparei(pCStr) == 0; }
	bool operator()(const eastl::string& str, const char* pCStr) const { return str.comparei(pCStr) == 0; }
};


///////////////////////////////////////////////////////////////////////////////
// StompDetectAllocator
// 
// An allocator that has sentinal values surrounding its allocator in an 
// effort to detected if its internal memory has been stomped.
//
static uint64_t STOMP_MAGIC_V1 = 0x0101DEC1A551F1ED;
static uint64_t STOMP_MAGIC_V2 = 0x12345C1A551F1ED5;

struct StompDetectAllocator
{
	StompDetectAllocator() { Validate(); }
	~StompDetectAllocator() { Validate(); }

	StompDetectAllocator(const char*) { Validate(); }

	void* allocate(size_t n, int = 0) { return mMallocAllocator.allocate(n); }
	void* allocate(size_t n, size_t, size_t, int = 0) { return mMallocAllocator.allocate(n); }
	void deallocate(void* p, size_t n) { mMallocAllocator.deallocate(p, n); }

	const char* get_name() const { return "FatAllocator"; }
	void set_name(const char*) {}

	void Validate() const
	{
		EASTL_ASSERT(mSentinal1 == STOMP_MAGIC_V1);
		EASTL_ASSERT(mSentinal2 == STOMP_MAGIC_V2);
	}

	uint64_t mSentinal1 = STOMP_MAGIC_V1;
	MallocAllocator mMallocAllocator;
	uint64_t mSentinal2 = STOMP_MAGIC_V2;
};

inline bool operator==(const StompDetectAllocator& a, const StompDetectAllocator& b)
{
	a.Validate();
	b.Validate();

	return (a.mMallocAllocator == b.mMallocAllocator);
}

inline bool operator!=(const StompDetectAllocator& a, const StompDetectAllocator& b)
{
	a.Validate();
	b.Validate();

	return (a.mMallocAllocator != b.mMallocAllocator);
}


// Commonly used free-standing functions to test callables
inline int ReturnVal(int param) { return param; }
inline int ReturnZero() { return 0; }
inline int ReturnOne() { return 1; }


// ValueInit
template<class T>
struct ValueInitOf
{
	ValueInitOf() : mV() {}
	~ValueInitOf() = default;

	ValueInitOf(const ValueInitOf&) = default;
	ValueInitOf(ValueInitOf&&) = default;

	ValueInitOf& operator=(const ValueInitOf&) = default;
	ValueInitOf& operator=(ValueInitOf&&) = default;

	T get() { return mV; }

	T mV;
};

// MoveOnlyType - useful for verifying containers that may hold, e.g., unique_ptrs to make sure move ops are implemented
struct MoveOnlyType
{
	MoveOnlyType() = delete;
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
	bool operator==(const MoveOnlyType& o) const { return mVal == o.mVal; }

	int mVal;
};

// MoveOnlyTypeDefaultCtor - useful for verifying containers that may hold, e.g., unique_ptrs to make sure move ops are implemented
struct MoveOnlyTypeDefaultCtor
{
	MoveOnlyTypeDefaultCtor() = default;
	MoveOnlyTypeDefaultCtor(int val) : mVal(val) {}
	MoveOnlyTypeDefaultCtor(const MoveOnlyTypeDefaultCtor&) = delete;
	MoveOnlyTypeDefaultCtor(MoveOnlyTypeDefaultCtor&& x) : mVal(x.mVal) { x.mVal = 0; }
	MoveOnlyTypeDefaultCtor& operator=(const MoveOnlyTypeDefaultCtor&) = delete;
	MoveOnlyTypeDefaultCtor& operator=(MoveOnlyTypeDefaultCtor&& x)
	{
		mVal = x.mVal;
		x.mVal = 0;
		return *this;
	}
	bool operator==(const MoveOnlyTypeDefaultCtor& o) const { return mVal == o.mVal; }

	int mVal;
};

struct TriviallyCopyableWithCopy {
	// non-trivial default ctor
	TriviallyCopyableWithCopy(unsigned int v = 0) noexcept : mValue(v) {}

	// all eligible (for trivial copyability) copy ctor/move ctor/copy assignment/move assignment are trivial
	TriviallyCopyableWithCopy(const TriviallyCopyableWithCopy&) = default;
	TriviallyCopyableWithCopy& operator=(const TriviallyCopyableWithCopy&) = default;

	// remaining copy ctor/move ctor/copy assignment/move assignment are deleted
	TriviallyCopyableWithCopy(TriviallyCopyableWithCopy&&) = delete;
	TriviallyCopyableWithCopy& operator=(TriviallyCopyableWithCopy&&) = delete;

	friend bool operator==(const TriviallyCopyableWithCopy& lhs, const TriviallyCopyableWithCopy& rhs) { return lhs.mValue == rhs.mValue; }

public:
	unsigned int mValue;

	// intentionally not a standard-layout class:
	// standard-layout requires all non-static data members have the same access control.
private:
	char ch{ 'C' };
};
static_assert(eastl::is_default_constructible<TriviallyCopyableWithCopy>::value, "TriviallyCopyableWithCopy");
static_assert(eastl::is_trivially_copyable<TriviallyCopyableWithCopy>::value, "TriviallyCopyableWithCopy");
static_assert(!eastl::is_standard_layout<TriviallyCopyableWithCopy>::value, "TriviallyCopyableWithCopy");

struct TriviallyCopyableWithMove {
	// non-trivial default ctor
	TriviallyCopyableWithMove(unsigned int v = 0) noexcept : mValue(v) {}

	// all eligible (for trivial copyability) copy ctor/move ctor/copy assignment/move assignment are trivial
	TriviallyCopyableWithMove(TriviallyCopyableWithMove&&) = default;
	TriviallyCopyableWithMove& operator=(TriviallyCopyableWithMove&&) = default;

	// remaining copy ctor/move ctor/copy assignment/move assignment are deleted
	TriviallyCopyableWithMove(const TriviallyCopyableWithMove&) = delete;
	TriviallyCopyableWithMove& operator=(const TriviallyCopyableWithMove&) = delete;

	friend bool operator==(const TriviallyCopyableWithMove& lhs, const TriviallyCopyableWithMove& rhs) { return lhs.mValue == rhs.mValue; }

public:
	unsigned int mValue;

	// intentionally not a standard-layout class:
	// standard-layout requires all non-static data members have the same access control.
private:
	char ch{ 'C' };
};
static_assert(eastl::is_default_constructible<TriviallyCopyableWithMove>::value, "TriviallyCopyableWithMove");
static_assert(eastl::is_trivially_copyable<TriviallyCopyableWithMove>::value, "TriviallyCopyableWithMove");
static_assert(!eastl::is_standard_layout<TriviallyCopyableWithMove>::value, "TriviallyCopyableWithMove");

struct TriviallyCopyableWithCopyCtor {
	TriviallyCopyableWithCopyCtor(unsigned int v) noexcept : mValue(v) {}

	TriviallyCopyableWithCopyCtor(const TriviallyCopyableWithCopyCtor&) = default;

	TriviallyCopyableWithCopyCtor() = delete;
	TriviallyCopyableWithCopyCtor(TriviallyCopyableWithCopyCtor&&) = delete;
	TriviallyCopyableWithCopyCtor& operator=(const TriviallyCopyableWithCopyCtor&) = delete;
	TriviallyCopyableWithCopyCtor& operator=(TriviallyCopyableWithCopyCtor&&) = delete;

	friend bool operator==(const TriviallyCopyableWithCopyCtor& lhs, const TriviallyCopyableWithCopyCtor& rhs) { return lhs.mValue == rhs.mValue; }

	unsigned int mValue;
};
static_assert(eastl::is_trivially_copyable<TriviallyCopyableWithCopyCtor>::value, "TriviallyCopyableWithCopyCtor");
static_assert(eastl::is_standard_layout<TriviallyCopyableWithCopyCtor>::value, "TriviallyCopyableWithCopyCtor");

struct TriviallyCopyableWithCopyAssign {
	TriviallyCopyableWithCopyAssign(unsigned int v) noexcept : mValue(v) {}

	TriviallyCopyableWithCopyAssign& operator=(const TriviallyCopyableWithCopyAssign&) = default;

	TriviallyCopyableWithCopyAssign() = delete;
	TriviallyCopyableWithCopyAssign(const TriviallyCopyableWithCopyAssign&) = delete;
	TriviallyCopyableWithCopyAssign(TriviallyCopyableWithCopyAssign&&) = delete;
	TriviallyCopyableWithCopyAssign& operator=(TriviallyCopyableWithCopyAssign&&) = delete;

	friend bool operator==(const TriviallyCopyableWithCopyAssign& lhs, const TriviallyCopyableWithCopyAssign& rhs) { return lhs.mValue == rhs.mValue; }

	unsigned int mValue;
};
static_assert(eastl::is_trivially_copyable<TriviallyCopyableWithCopyAssign>::value, "TriviallyCopyableWithCopyAssign");
static_assert(eastl::is_standard_layout<TriviallyCopyableWithCopyAssign>::value, "TriviallyCopyableWithCopyAssign");

struct TriviallyCopyableWithMoveCtor {
	TriviallyCopyableWithMoveCtor(unsigned int v) noexcept : mValue(v) {}

	TriviallyCopyableWithMoveCtor(TriviallyCopyableWithMoveCtor&&) = default;

	TriviallyCopyableWithMoveCtor() = delete;
	TriviallyCopyableWithMoveCtor(const TriviallyCopyableWithMoveCtor&) = delete;
	TriviallyCopyableWithMoveCtor& operator=(const TriviallyCopyableWithMoveCtor&) = delete;
	TriviallyCopyableWithMoveCtor& operator=(TriviallyCopyableWithMoveCtor&&) = delete;

	friend bool operator==(const TriviallyCopyableWithMoveCtor& lhs, const TriviallyCopyableWithMoveCtor& rhs) { return lhs.mValue == rhs.mValue; }

	unsigned int mValue;
};
static_assert(eastl::is_trivially_copyable<TriviallyCopyableWithMoveCtor>::value, "TriviallyCopyableWithMoveCtor");
static_assert(eastl::is_standard_layout<TriviallyCopyableWithMoveCtor>::value, "TriviallyCopyableWithMoveCtor");

struct TriviallyCopyableWithMoveAssign {
	TriviallyCopyableWithMoveAssign(unsigned int v) noexcept : mValue(v) {}

	TriviallyCopyableWithMoveAssign& operator=(TriviallyCopyableWithMoveAssign&&) = default;

	TriviallyCopyableWithMoveAssign() = delete;
	TriviallyCopyableWithMoveAssign(const TriviallyCopyableWithMoveAssign&) = delete;
	TriviallyCopyableWithMoveAssign(TriviallyCopyableWithMoveAssign&&) = delete;
	TriviallyCopyableWithMoveAssign& operator=(const TriviallyCopyableWithMoveAssign&) = delete;

	friend bool operator==(const TriviallyCopyableWithMoveAssign& lhs, const TriviallyCopyableWithMoveAssign& rhs) { return lhs.mValue == rhs.mValue; }

	unsigned int mValue;
};
static_assert(eastl::is_trivially_copyable<TriviallyCopyableWithMoveAssign>::value, "TriviallyCopyableWithMoveAssign");
static_assert(eastl::is_standard_layout<TriviallyCopyableWithMoveAssign>::value, "TriviallyCopyableWithMoveAssign");

// useful for testing empty base optimization of types
struct NoDataMembers {};
static_assert(eastl::is_empty<NoDataMembers>::value, "empty");


//////////////////////////////////////////////////////////////////////////////
// Utility RAII class that sets a new default allocator for the scope
//
struct AutoDefaultAllocator
{
	eastl::allocator* mPrevAllocator = nullptr;

	AutoDefaultAllocator(eastl::allocator* nextAllocator) { mPrevAllocator = SetDefaultAllocator(nextAllocator); }
	~AutoDefaultAllocator()                               { SetDefaultAllocator(mPrevAllocator); }
};


#endif // Header include guard







