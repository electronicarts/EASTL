/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/type_traits.h>
#include <EASTL/vector.h>
#include <EAStdC/EAAlignment.h>
#include "ConceptImpls.h"



using namespace eastl;

EASTL_INTERNAL_DISABLE_DEPRECATED() // *: was declared deprecated


bool GetType(const true_type&)
{
	return true;
}

bool GetType(const false_type&)
{
	return false;
}

int GetType(const integral_constant<size_t, (size_t)4>&)
{
	return 4;
}

int GetType(const integral_constant<size_t, (size_t)8>&)
{
	return 8;
}

int GetType(const integral_constant<size_t, (size_t)16>&)
{
	return 16;
}

int GetType(const integral_constant<size_t, (size_t)32>&)
{
	return 32;
}

#ifdef _MSC_VER
	__declspec(align(32)) class ClassAlign32{ };
#else
	class ClassAlign32{ } __attribute__((aligned(32)));
#endif


struct Struct
{
	// Empty
};

class Class
{
	// Empty
};

class Subclass : public Class
{
	// Empty
};

class ClassEmpty
{
	// Empty
};

class ClassNonEmpty
{
public:
	int x;
};

enum Enum
{
	kValue1
};

union Union
{
	int   x;
	short y;
};

struct FinalStruct final
{
};

class FinalClass final
{
};

#if !EASTL_TYPE_TRAIT_is_union_CONFORMANCE
	EASTL_DECLARE_UNION(Union) // We have to do this because is_union simply cannot work without user help.
#endif



// Used for union_cast tests below.
// C++11 allows for PodA/PodB to have a trivial default (i.e. compiler-generated) constructor,
// but as of this writing (3/2012) most C++ compilers don't have support for this yet.
struct PodA{
	int mX;
};

struct PodB{
	int mX;
};

bool operator ==(const PodA& a1, const PodA& a2) { return (a1.mX == a2.mX); }


// std::tr1::is_volatile<T>::value == true if and only if, for a given type T:
//   * std::tr1::is_scalar<T>::value == true, or
//   * T is a class or struct that has no user-defined copy assignment operator or destructor, 
//     and T has no non-static data members M for which is_pod<M>::value == false, and no members of reference type, or
//   * T is the type of an array of objects E for which is_pod<E>::value == true
// is_pod may only be applied to complete types.

struct Pod1
{
	// Empty
};
#if !EASTL_TYPE_TRAIT_is_pod_CONFORMANCE
	EASTL_DECLARE_POD(Pod1) // We have to do this because is_pod simply cannot work without user help.
#endif
#if !EASTL_TYPE_TRAIT_is_standard_layout_CONFORMANCE
	EASTL_DECLARE_STANDARD_LAYOUT(Pod1) // We have to do this because is_standard_layout simply cannot work without user help.
#endif


struct Pod2
{
	int  mX;
	Pod1 mPod1;
};
#if !EASTL_TYPE_TRAIT_is_pod_CONFORMANCE
	EASTL_DECLARE_POD(Pod2)
#endif
#if !EASTL_TYPE_TRAIT_is_standard_layout_CONFORMANCE
	EASTL_DECLARE_STANDARD_LAYOUT(Pod2)
#endif

struct Pod3
{
	Pod2 mPod2;
	int  mX;
	Pod1 mPod1;
};
#if !EASTL_TYPE_TRAIT_is_pod_CONFORMANCE
	EASTL_DECLARE_POD(Pod3)
#endif
#if !EASTL_TYPE_TRAIT_is_standard_layout_CONFORMANCE
	EASTL_DECLARE_STANDARD_LAYOUT(Pod3)
#endif


struct NonPod1
{
	NonPod1(){}
	virtual ~NonPod1(){}
};

struct NonPod2
{
	virtual ~NonPod2(){}
	virtual void Function(){}
};

struct HasIncrementOperator { HasIncrementOperator& operator++() { return *this; } };

template <class T>
using has_increment_operator_detection = decltype(++eastl::declval<T>());

template<typename, typename = eastl::void_t<>>
struct has_increment_operator_using_void_t : eastl::false_type {};

template <typename T>
struct has_increment_operator_using_void_t<T, eastl::void_t<has_increment_operator_detection<T>>> : eastl::true_type {};


// We use this for the is_copy_constructible test in order to verify that 
// is_copy_constructible in fact returns false for this type and not true.
// std::is_copy_constructible specification: std::is_constructible<T, const T&>::value is true.
// Note that the specification refers to const T& and not T&. So we rig our class to 
// accept T& and not const T&. This situation is significant because as of this
// writing the clang <type_traits> implementation appears to be broken and mis-implements
// the is_copy_constructible type trait to return true for ConstructibleOnlyWithNonConstReference 
// when in fact it should return false.
EA_DISABLE_VC_WARNING(4521) // disable warning : "multiple copy constructors specified"
struct ConstructibleOnlyWithNonConstReference
{
	ConstructibleOnlyWithNonConstReference(ConstructibleOnlyWithNonConstReference&) {}

	#if defined(EA_COMPILER_NO_DELETED_FUNCTIONS)
		private: ConstructibleOnlyWithNonConstReference() {}
		private: ConstructibleOnlyWithNonConstReference(const ConstructibleOnlyWithNonConstReference&) {}
	#else
		ConstructibleOnlyWithNonConstReference() = delete;
		ConstructibleOnlyWithNonConstReference(const ConstructibleOnlyWithNonConstReference&) = delete;
	#endif
};
EA_RESTORE_VC_WARNING()

#if defined(EA_COMPILER_NO_NOEXCEPT)
	//This is needed because VS2013 supports is_nothrow__xxx type traits but doesn't support C++11 noexcept.
	//So we use throw() to allow the is_nothrow_xxxx and similiar tests to work in VS2013
	#define EASTL_TEST_NOEXCEPT throw()
#else
	#define EASTL_TEST_NOEXCEPT EA_NOEXCEPT
#endif

struct ThrowConstructibleTest
{
	ThrowConstructibleTest(const int = 0) EASTL_TEST_NOEXCEPT { }
	ThrowConstructibleTest(const float) EA_NOEXCEPT_IF(false) { }
};



struct NoThrowAssignable { };

struct ThrowAssignableTest
{
	void operator=(const NoThrowAssignable&) EASTL_TEST_NOEXCEPT { }
	void operator=(const ThrowAssignableTest&) { }
};


struct NoThrowDestructible
{
	~NoThrowDestructible() EASTL_TEST_NOEXCEPT {}
};

#if !defined(EA_COMPILER_NO_EXCEPTIONS)
	struct ThrowDestructible
	{
		~ThrowDestructible() noexcept(false) { throw(int()); }
	};

	struct ThrowDestructibleNoexceptFalse
	{
		virtual ~ThrowDestructibleNoexceptFalse() EA_NOEXCEPT_IF(false) {  }
	};
#endif


struct HasTrivialConstructor
{
	int x;
};
#if !EASTL_TYPE_TRAIT_has_trivial_constructor_CONFORMANCE
	EASTL_DECLARE_TRIVIAL_CONSTRUCTOR(HasTrivialConstructor) // We have to do this because has_trivial_constructor simply cannot work without user help.
#endif
#if !EASTL_TYPE_TRAIT_is_standard_layout_CONFORMANCE
	EASTL_DECLARE_STANDARD_LAYOUT(HasTrivialConstructor)
#endif


struct NoTrivialConstructor
{
	NoTrivialConstructor() { px = &x; }
	int  x;
	int* px;
};
#if !EASTL_TYPE_TRAIT_is_standard_layout_CONFORMANCE
	EASTL_DECLARE_STANDARD_LAYOUT(NoTrivialConstructor)
#endif


struct HasTrivialCopy
{
	void Function(){}
	int x;
};
#if !EASTL_TYPE_TRAIT_has_trivial_constructor_CONFORMANCE
	EASTL_DECLARE_TRIVIAL_COPY(HasTrivialCopy) // We have to do this because has_trivial_copy simply cannot work without user help.
#endif


#if defined(EA_COMPILER_MSVC) && (_MSC_VER == 1900)
	// http://blogs.msdn.com/b/vcblog/archive/2014/06/06/c-14-stl-features-fixes-and-breaking-changes-in-visual-studio-14-ctp1.aspx
	// VS2015-preview has a bug regarding C++14 implicit noexcept rules for destructors.  We explicitly define noexcept below for VS2015-preview only.
	//
	// Re-evaluate when VS2015 RTM has been released.
	//
	struct NoTrivialCopy1
	{
		virtual ~NoTrivialCopy1() EASTL_TEST_NOEXCEPT {} 
		virtual void Function(){}
	};
#else
	struct NoTrivialCopy1
	{
		virtual ~NoTrivialCopy1() {}  
		virtual void Function(){}
	};
#endif

struct NoTrivialCopy2
{
	NoTrivialCopy1 ntv;
};

struct NonCopyable
{
	NonCopyable() : mX(0) {}
	NonCopyable(int x) : mX(x) {}

	int mX;

	EA_NON_COPYABLE(NonCopyable)
};

struct HasTrivialAssign
{
	void Function(){}
	int x;
};
#if !EASTL_TYPE_TRAIT_has_trivial_assign_CONFORMANCE
	EASTL_DECLARE_TRIVIAL_ASSIGN(HasTrivialAssign) // We have to do this because has_trivial_assign simply cannot work without user help.
#endif

struct NoTrivialAssign1
{
	virtual ~NoTrivialAssign1(){}
	virtual void Function(){}
};

struct NoTrivialAssign2
{
	NoTrivialAssign1 nta;
};

struct Polymorphic1
{
	virtual ~Polymorphic1(){}
	virtual void Function(){}
};

struct Polymorphic2 : public Polymorphic1
{
	// Empty
};

struct Polymorphic3
{
	virtual ~Polymorphic3(){}
	virtual void Function() = 0;
};

struct NonPolymorphic1
{
	void Function(){}
};

// Disable the following warning:
//     warning: ‘struct Abstract’ has virtual functions and accessible non-virtual destructor [-Wnon-virtual-dtor]
// We explicitly want this class not to have a virtual destructor to test our type traits.
EA_DISABLE_VC_WARNING(4265)
EA_DISABLE_CLANG_WARNING(-Wnon-virtual-dtor)
EA_DISABLE_GCC_WARNING(-Wnon-virtual-dtor)
struct Abstract
{
	virtual void Function() = 0;
};
EA_RESTORE_GCC_WARNING()
EA_RESTORE_CLANG_WARNING()
EA_RESTORE_VC_WARNING()

struct AbstractWithDtor
{
	virtual ~AbstractWithDtor(){}
	virtual void Function() = 0;
};

struct DeletedDtor
{
	#if !defined(EA_COMPILER_NO_DELETED_FUNCTIONS)
		~DeletedDtor() = delete;
	#endif
};

#if (EASTL_TYPE_TRAIT_is_destructible_CONFORMANCE == 0)
	EASTL_DECLARE_IS_DESTRUCTIBLE(DeletedDtor, false)
#endif

struct Assignable
{
	void operator=(const Assignable&){}
	void operator=(const Pod1&){}
};

class HiddenAssign
{
public:
	HiddenAssign();

private:
	HiddenAssign(const HiddenAssign& x);
	HiddenAssign& operator=(const HiddenAssign& x);
};

#if !EASTL_TYPE_TRAIT_has_trivial_assign_CONFORMANCE
	EASTL_DECLARE_TRIVIAL_ASSIGN(HiddenAssign)
#endif



// This class exercises is_convertible for the case that the class has an explicit copy constructor.
struct IsConvertibleTest1
{
	IsConvertibleTest1() {}
	IsConvertibleTest1(int, int) {}
	explicit IsConvertibleTest1(const IsConvertibleTest1&) {}
	~IsConvertibleTest1(){}
};



// Helpers for enable_if tests
template<typename T>
typename eastl::enable_if<eastl::is_floating_point<T>::value, T>::type EnableIfTestFunction(T) 
	{ return 999; }
 
template<typename T>
typename eastl::enable_if<eastl::is_integral<T>::value, T>::type EnableIfTestFunction(T) 
	{ return 888; }

template<typename T>
typename eastl::disable_if<eastl::is_signed<T>::value, T>::type EnableIfTestFunction(T) 
	{ return 777; }
 


// Test that EASTL_DECLARE_TRIVIAL_ASSIGN can be used to get around case whereby 
// the copy constructor and operator= are private. Normally vector requires this.
// ** This is disabled because it turns out that vector in fact requires the 
// constructor for some uses. But we have code below which tests just part of vector.
// template class eastl::vector<HiddenAssign>;


typedef char Array[32];
typedef const char ArrayConst[32];


typedef Class& Reference;
typedef const Class& ConstReference;


typedef const int ConstInt;
typedef int Int;
typedef volatile int VolatileInt;
typedef const volatile int ConstVolatileInt;
typedef int& IntReference;
typedef const int& ConstIntReference;                   // Note here that the int is const, not the reference to the int.
typedef const volatile int& ConstVolatileIntReference;  // Note here that the int is const, not the reference to the int.


typedef void FunctionVoidVoid();
typedef int  FunctionIntVoid();
typedef int  FunctionIntFloat(float);
typedef void (*FunctionVoidVoidPtr)();

namespace
{
	const eastl::string gEmptyStringInstance("");

	const eastl::integral_constant<int*, nullptr> gIntNullptrConstant;
	static_assert(gIntNullptrConstant() == nullptr, "");
}

int TestTypeTraits()
{
	int nErrorCount = 0;


	// static_min / static_max
	#if EASTL_TYPE_TRAIT_static_min_CONFORMANCE
		static_assert((static_min<3, 7, 1, 5>::value == 1),  "static_min failure");
		static_assert((static_max<3, 7, 1, 5>::value == 7),  "static_max failure");
	#else
		static_assert((static_min<7, 1>::value == 1),  "static_min failure");
		static_assert((static_max<7, 1>::value == 7),  "static_max failure");
	#endif

	// enable_if, disable_if.
	EATEST_VERIFY((EnableIfTestFunction((double)1.1) == 999));
	EATEST_VERIFY((EnableIfTestFunction((int)1) == 888));
	EATEST_VERIFY((EnableIfTestFunction((int)-4) == 888));


	// conditional
	static_assert(sizeof(conditional<true,  int8_t, int16_t>::type) == sizeof(int8_t),  "conditional failure");
	static_assert(sizeof(conditional<false, int8_t, int16_t>::type) == sizeof(int16_t), "conditional failure");

	// bool_constant
	static_assert(bool_constant<is_same<int, int>::value>::value == true, "bool_constant failure");
	static_assert(bool_constant<is_same<int, short>::value>::value == false, "bool_constant failure");
	static_assert(is_same<bool_constant<false>::type, integral_constant<bool, false>::type>::value, "bool_constant failure");



	// identity
	static_assert(sizeof(identity<int>::type) == sizeof(int), "identity failure");
	static_assert((is_same<int, identity<int>::type >::value == true), "identity failure");

	// type_identity
	static_assert(sizeof(type_identity<int>::type) == sizeof(int), "type_identity failure");
	static_assert((is_same<int, type_identity<int>::type >::value == true), "type_identity failure");
	static_assert(sizeof(type_identity_t<int>) == sizeof(int), "type_identity failure");
	static_assert((is_same_v<int, type_identity_t<int>> == true), "type_identity failure");



	// is_void
	static_assert(is_void<void>::value == true, "is_void failure");
	static_assert(is_void<const void>::value == true, "is_void failure");
	static_assert(is_void<int>::value == false, "is_void failure");


	// is_null_pointer
	#if defined(EA_COMPILER_CPP11_ENABLED)
		#if !defined(EA_COMPILER_NO_DECLTYPE) && !defined(_MSC_VER) // VS2012 is broken for just the case of decltype(nullptr).
			static_assert(is_null_pointer<decltype(nullptr)>::value == true,  "is_null_pointer failure");
			static_assert(is_null_pointer<decltype(NULL)>::value    == false, "is_null_pointer failure");
		#endif
		#if defined(EA_HAVE_nullptr_t_IMPL)
			static_assert(is_null_pointer<std::nullptr_t>::value == true,  "is_null_pointer failure");  // Can't enable this until we are using an updated <EABase/nullptr.h> that is savvy to C++11 clang (defines nullptr) being used with C++98 GNU libstdc++ (defines std::nullptr_t). 
		#endif
		static_assert(is_null_pointer<void*>::value          == false, "is_null_pointer failure");
		static_assert(is_null_pointer<intptr_t>::value       == false, "is_null_pointer failure");
	#endif

	// is_integral
	static_assert(is_integral<int>::value == true, "is_integral failure");
	EATEST_VERIFY(GetType(is_integral<int>()) == true);

	static_assert(is_integral<const int>::value == true, "is_integral failure");
	EATEST_VERIFY(GetType(is_integral<const int>()) == true);

	static_assert(is_integral<float>::value == false, "is_integral failure");
	EATEST_VERIFY(GetType(is_integral<float>()) == false);

	static_assert(is_integral<bool>::value,               "is_integral failure");
	static_assert(is_integral<char8_t>::value,            "is_integral failure");
	static_assert(is_integral<char16_t>::value,           "is_integral failure");
	static_assert(is_integral<char32_t>::value,           "is_integral failure");
	static_assert(is_integral<char>::value,               "is_integral failure");
	static_assert(is_integral<int>::value,                "is_integral failure");
	static_assert(is_integral<long long>::value,          "is_integral failure");
	static_assert(is_integral<long>::value,               "is_integral failure");
	static_assert(is_integral<short>::value,              "is_integral failure");
	static_assert(is_integral<signed char>::value,        "is_integral failure");
	static_assert(is_integral<unsigned char>::value,      "is_integral failure");
	static_assert(is_integral<unsigned int>::value,       "is_integral failure");
	static_assert(is_integral<unsigned long long>::value, "is_integral failure");
	static_assert(is_integral<unsigned long>::value,      "is_integral failure");
	static_assert(is_integral<unsigned short>::value,     "is_integral failure");
#ifndef EA_WCHAR_T_NON_NATIVE // If wchar_t is a native type instead of simply a define to an existing type which is already handled...
	static_assert(is_integral<wchar_t>::value,            "is_integral failure");
#endif


	// is_floating_point
	static_assert(is_floating_point<double>::value == true, "is_floating_point failure");
	EATEST_VERIFY(GetType(is_floating_point<double>()) == true);

	static_assert(is_floating_point<const double>::value == true, "is_floating_point failure");
	EATEST_VERIFY(GetType(is_floating_point<const double>()) == true);

	static_assert(is_floating_point<int>::value == false, "is_floating_point failure");
	EATEST_VERIFY(GetType(is_floating_point<int>()) == false);


	// is_arithmetic
	static_assert(is_arithmetic<float>::value == true, "is_arithmetic failure");
	static_assert(is_arithmetic_v<float> == true,      "is_arithmetic failure");
	EATEST_VERIFY(GetType(is_arithmetic<float>()) == true);

	static_assert(is_arithmetic<Class>::value == false, "is_arithmetic failure");
	static_assert(is_arithmetic_v<Class> == false,      "is_arithmetic failure");
	EATEST_VERIFY(GetType(is_arithmetic<Class>()) == false);


	// is_fundamental
	static_assert(is_fundamental<void>::value == true, "is_fundamental failure");
	static_assert(is_fundamental_v<void> == true,      "is_fundamental failure");
	EATEST_VERIFY(GetType(is_fundamental<void>()) == true);

	#ifndef EA_WCHAR_T_NON_NATIVE // If wchar_t is a native type instead of simply a define to an existing type which is already handled...
		static_assert(is_fundamental<wchar_t>::value == true, "is_fundamental failure");
		static_assert(is_fundamental_v<wchar_t> == true,      "is_fundamental failure");
		EATEST_VERIFY(GetType(is_fundamental<wchar_t>()) == true);
	#endif

	static_assert(is_fundamental<Class>::value == false, "is_fundamental failure");
	static_assert(is_fundamental_v<Class> == false,      "is_fundamental failure");
	EATEST_VERIFY(GetType(is_fundamental<Class>()) == false);

	static_assert(is_fundamental<std::nullptr_t>::value == true, "is_fundamental failure");
	static_assert(is_fundamental_v<std::nullptr_t> == true,      "is_fundamental failure");


	// is_array
	static_assert(is_array<Array>::value == true, "is_array failure");
	EATEST_VERIFY(GetType(is_array<Array>()) == true);

	static_assert(is_array<ArrayConst>::value == true,   "is_array failure");
	EATEST_VERIFY(GetType(is_array<ArrayConst>()) == true);

	static_assert(is_array<int[]>::value == true,        "is_array failure");

	static_assert(is_array<uint32_t>::value == false,    "is_array failure");
	EATEST_VERIFY(GetType(is_array<uint32_t>()) == false);

	static_assert(is_array<uint32_t*>::value == false,   "is_array failure");
	EATEST_VERIFY(GetType(is_array<uint32_t*>()) == false);


	//is_bounded_array
	static_assert(is_bounded_array<Array>::value == true, "is_bounded_array failure");
	EATEST_VERIFY(GetType(is_bounded_array<Array>()) == true);

	static_assert(is_bounded_array<ArrayConst>::value == true,   "is_bounded_array failure");
	EATEST_VERIFY(GetType(is_bounded_array<ArrayConst>()) == true);

	static_assert(is_bounded_array<int>::value == false,        "is_bounded_array failure");
	static_assert(is_bounded_array<int[32]>::value == true,        "is_bounded_array failure");
	static_assert(is_bounded_array<int[]>::value == false,        "is_bounded_array failure");
	static_assert(is_bounded_array<int[0]>::value == false,		"is_bounded_array failure");
	static_assert(is_bounded_array<void>::value == false,		"is_bounded_array failure");

	static_assert(is_bounded_array<uint32_t>::value == false,    "is_bounded_array failure");
	EATEST_VERIFY(GetType(is_bounded_array<uint32_t>()) == false);

	static_assert(is_bounded_array<uint32_t*>::value == false,   "is_bounded_array failure");
	EATEST_VERIFY(GetType(is_bounded_array<uint32_t*>()) == false);


	//is_unbounded_array
	static_assert(is_unbounded_array<Array>::value == false, "is_unbounded_array failure");
	EATEST_VERIFY(GetType(is_unbounded_array<Array>()) == false);

	static_assert(is_unbounded_array<ArrayConst>::value == false,   "is_unbounded_array failure");
	EATEST_VERIFY(GetType(is_unbounded_array<ArrayConst>()) == false);

	static_assert(is_unbounded_array<int>::value == false,        "is_unbounded_array failure");
	static_assert(is_unbounded_array<int[32]>::value == false,        "is_unbounded_array failure");
	static_assert(is_unbounded_array<int[]>::value == true,        "is_unbounded_array failure");
	static_assert(is_unbounded_array<int[0]>::value == false,		"is_unbounded_array failure");
	static_assert(is_unbounded_array<void>::value == false,			"is_unbounded_array failure");

	static_assert(is_unbounded_array<uint32_t>::value == false,    "is_unbounded_array failure");
	EATEST_VERIFY(GetType(is_unbounded_array<uint32_t>()) == false);

	static_assert(is_unbounded_array<uint32_t*>::value == false,   "is_unbounded_array failure");
	EATEST_VERIFY(GetType(is_unbounded_array<uint32_t*>()) == false);



	// is_reference
	static_assert(is_reference<Class&>::value == true,        "is_reference failure");
	EATEST_VERIFY(GetType(is_reference<Class&>()) == true);

	static_assert(is_reference<Class&&>::value == true,        "is_reference failure");
	EATEST_VERIFY(GetType(is_reference<Class&&>()) == true);

	static_assert(is_reference<const Class&>::value == true,  "is_reference failure");
	EATEST_VERIFY(GetType(is_reference<const Class&>()) == true);

	static_assert(is_reference<const Class&&>::value == true,  "is_reference failure");
	EATEST_VERIFY(GetType(is_reference<const Class&&>()) == true);

	static_assert(is_reference<Class>::value == false,        "is_reference failure");
	EATEST_VERIFY(GetType(is_reference<Class>()) == false);

	static_assert(is_reference<const Class*>::value == false, "is_reference failure");
	EATEST_VERIFY(GetType(is_reference<const Class*>()) == false);


	// is_member_function_pointer
	static_assert(is_member_function_pointer<int>::value == false,            "is_member_function_pointer failure");
	static_assert(is_member_function_pointer<int(Class::*)>::value == false,  "is_member_function_pointer failure");
	static_assert(is_member_function_pointer<int(Class::*)()>::value == true, "is_member_function_pointer failure");
	static_assert(is_member_function_pointer<int(Class::*)(...)>::value == true, "is_member_function_pointer failure");
	static_assert(is_member_function_pointer<int(Class::*)() noexcept>::value == true, "is_member_function_pointer failure");
	static_assert(is_member_function_pointer<int(Class::*)() &>::value == true, "is_member_function_pointer failure");
	static_assert(is_member_function_pointer<int(Class::*)() &&>::value == true, "is_member_function_pointer failure");


	// is_member_object_pointer
	static_assert(is_member_object_pointer<int>::value == false,             "is_member_object_pointer failure");
	static_assert(is_member_object_pointer<int(Class::*)>::value == true,    "is_member_object_pointer failure");
	static_assert(is_member_object_pointer<int(Class::*)()>::value == false, "is_member_object_pointer failure");


	// is_member_pointer
	static_assert(is_member_pointer<int>::value == false,            "is_member_pointer failure");
	static_assert(is_member_pointer<int(Class::*)>::value == true,   "is_member_pointer failure");
	static_assert(is_member_pointer<int(Class::*)()>::value == true, "is_member_pointer failure");
	static_assert(is_member_pointer<int(Class::* const)>::value == true, "is_member_pointer failure");
	static_assert(is_member_pointer<int(Class::* volatile)>::value == true, "is_member_pointer failure");
	static_assert(is_member_pointer<int(Class::* const volatile)>::value == true, "is_member_pointer failure");


	// is_pointer
	static_assert(is_pointer<Class*>::value         == true,  "is_pointer failure");
	static_assert(is_pointer<const Class*>::value   == true,  "is_pointer failure");
	static_assert(is_pointer<Class>::value          == false, "is_pointer failure");
	static_assert(is_pointer<const Class&>::value   == false, "is_pointer failure");
	#if defined(EA_HAVE_nullptr_t_IMPL)
		static_assert(is_pointer<std::nullptr_t>::value == false, "is_pointer failure");
	#endif

	// is_enum
	static_assert(is_enum<Enum>::value == true,            "is_enum failure ");
	static_assert(is_enum_v<Enum> == true,                 "is_enum failure ");
	EATEST_VERIFY(GetType(is_enum<Enum>()) == true);

	static_assert(is_enum<const Enum>::value == true,      "is_enum failure ");
	static_assert(is_enum_v<const Enum> == true,           "is_enum failure ");
	EATEST_VERIFY(GetType(is_enum<const Enum>()) == true);

	static_assert(is_enum<Enum*>::value == false,          "is_enum failure ");
	static_assert(is_enum_v<Enum*> == false,               "is_enum failure ");
	EATEST_VERIFY(GetType(is_enum<Enum*>()) == false);

	static_assert(is_enum<Class>::value == false,          "is_enum failure ");
	static_assert(is_enum_v<Class> == false,               "is_enum failure ");
	EATEST_VERIFY(GetType(is_enum<Class>()) == false);

	static_assert(is_enum<Enum&>::value == false,          "is_enum failure ");
	static_assert(is_enum_v<Enum&> == false,               "is_enum failure ");
	EATEST_VERIFY(GetType(is_enum<Enum&>()) == false);

	static_assert(is_enum<Enum&&>::value == false,          "is_enum failure ");
	static_assert(is_enum_v<Enum&&> == false,               "is_enum failure ");
	EATEST_VERIFY(GetType(is_enum<Enum&&>()) == false);


	// is_union
	static_assert(is_union<Union>::value == true, "is_union failure");
	static_assert(is_union_v<Union> == true,      "is_union failure");
	EATEST_VERIFY(GetType(is_union<Union>()) == true);

	static_assert(is_union<int>::value == false, "is_union failure");
	static_assert(is_union_v<int> == false,      "is_union failure");
	EATEST_VERIFY(GetType(is_union<int>()) == false);


	// is_class
	static_assert(is_class<Class>::value == true, "is_class failure");
	EATEST_VERIFY(GetType(is_class<Class>()) == true);

	static_assert(is_class<Struct>::value == true, "is_class failure");
	EATEST_VERIFY(GetType(is_class<Struct>()) == true);

	static_assert(is_class<Union>::value == false, "is_class failure");
	EATEST_VERIFY(GetType(is_class<Union>()) == false);

	static_assert(is_class<Enum>::value == false, "is_class failure");
	EATEST_VERIFY(GetType(is_class<Enum>()) == false);

	static_assert(is_class<int*>::value == false, "is_class failure");
	EATEST_VERIFY(GetType(is_class<int*>()) == false);


	// is_function
	static_assert(is_function<void>::value == false,                      "is_function failure");
	static_assert(is_function<FunctionVoidVoid>::value == true,           "is_function failure");
	static_assert(is_function<FunctionVoidVoid&>::value == false,		  "is_function failure");
	static_assert(is_function<FunctionIntVoid>::value == true,            "is_function failure");
	static_assert(is_function<FunctionIntFloat>::value == true,           "is_function failure");
	static_assert(is_function<FunctionVoidVoidPtr>::value == false,       "is_function failure");
	static_assert(is_function<int>::value == false,                       "is_function failure");
	static_assert(is_function<int[3]>::value == false,                    "is_function failure");
	static_assert(is_function<int[]>::value == false,                     "is_function failure");
	static_assert(is_function<Class>::value == false,                     "is_function failure");
	#if EASTL_TYPE_TRAIT_is_function_CONFORMANCE
		// typedef int PrintfConst(const char*, ...) const;
		static_assert(is_function<int (const char*, ...)>::value == true, "is_function failure");  // This is the signature of printf.
	#endif
		
	static_assert(is_function<int (float)>::value == true, "is_function failure");
	static_assert(is_function<int (float) const>::value == true, "is_function failure");
	static_assert(is_function<int(float) volatile>::value == true, "is_function failure");
	static_assert(is_function<int(float) const volatile>::value == true, "is_function failure");
	static_assert(is_function<int(float)&>::value == true, "is_function failure");
	static_assert(is_function<int(float)&&>::value == true, "is_function failure");
	static_assert(is_function<int(float) noexcept>::value == true, "is_function failure");
	static_assert(is_function<FunctionIntFloat &>::value == false, "is_function failure"); // reference to function, not a l-value reference qualified function
	static_assert(is_function<FunctionIntFloat &&>::value == false, "is_function failure");

	static_assert(is_function_v<void> == false,                           "is_function failure");
	static_assert(is_function_v<FunctionVoidVoid> == true,                "is_function failure");
	static_assert(is_function_v<FunctionVoidVoid&> == false,              "is_function failure");
	static_assert(is_function_v<FunctionIntVoid> == true,                 "is_function failure");
	static_assert(is_function_v<FunctionIntFloat> == true,                "is_function failure");
	static_assert(is_function_v<FunctionVoidVoidPtr> == false,            "is_function failure");
	static_assert(is_function_v<int> == false,                            "is_function failure");
	static_assert(is_function_v<int[3]> == false,                         "is_function failure");
	static_assert(is_function_v<int[]> == false,                          "is_function failure");
	static_assert(is_function_v<Class> == false,                          "is_function failure");
	#if EASTL_TYPE_TRAIT_is_function_CONFORMANCE
		// typedef int PrintfConst(const char*, ...) const;
		static_assert(is_function_v<int (const char*, ...)> == true,      "is_function failure");  // This is the signature of printf.
	#endif


	// is_object
	static_assert(is_object<int>::value == true,     "is_object failure");
	EATEST_VERIFY(GetType(is_object<int>()) == true);

	static_assert(is_object<Class>::value == true,   "is_object failure");
	EATEST_VERIFY(GetType(is_object<Class>()) == true);

	static_assert(is_object<Class*>::value == true,  "is_object failure");
	EATEST_VERIFY(GetType(is_object<Class*>()) == true);

	static_assert(is_object<Class&>::value == false, "is_object failure");
	EATEST_VERIFY(GetType(is_object<Class&>()) == false);

	static_assert(is_object<Class&&>::value == false, "is_object failure");
	EATEST_VERIFY(GetType(is_object<Class&&>()) == false);


	// is_scalar
	static_assert(is_scalar<int>::value == true,          "is_scalar failure");
	EATEST_VERIFY(GetType(is_scalar<int>()) == true);

	static_assert(is_scalar<double>::value == true,       "is_scalar failure");
	EATEST_VERIFY(GetType(is_scalar<double>()) == true);

	static_assert(is_scalar<Enum>::value == true,         "is_scalar failure");
	EATEST_VERIFY(GetType(is_scalar<Enum>()) == true);

	static_assert(is_scalar<const Class*>::value == true, "is_scalar failure");
	EATEST_VERIFY(GetType(is_scalar<const Class*>()) == true);

	static_assert(is_scalar<std::nullptr_t>::value == true, "is_scalar failure");


	// is_compound
	static_assert(is_compound<Class>::value == true, "is_compound failure");
	EATEST_VERIFY(GetType(is_compound<Class>()) == true);

	static_assert(is_compound<const Class&>::value == true, "is_compound failure");
	EATEST_VERIFY(GetType(is_compound<const Class&>()) == true);

	static_assert(is_compound<int*>::value == true, "is_compound failure");
	EATEST_VERIFY(GetType(is_compound<int*>()) == true);

	static_assert(is_compound<float>::value == false, "is_compound failure");
	EATEST_VERIFY(GetType(is_compound<float>()) == false);

	static_assert(is_compound<bool>::value == false, "is_compound failure");
	EATEST_VERIFY(GetType(is_compound<bool>()) == false);


	// is_const
	static_assert(is_const<Int>::value == false, "is_const failure");
	EATEST_VERIFY(GetType(is_const<Int>()) == false);

	static_assert(is_const<ConstInt>::value == true, "is_const failure");
	EATEST_VERIFY(GetType(is_const<ConstInt>()) == true);

	static_assert(is_const<VolatileInt>::value == false, "is_const failure");
	EATEST_VERIFY(GetType(is_const<VolatileInt>()) == false);

	static_assert(is_const<ConstVolatileInt>::value == true, "is_const failure");
	EATEST_VERIFY(GetType(is_const<ConstVolatileInt>()) == true);

	static_assert(is_const<IntReference>::value == false, "is_const failure");
	EATEST_VERIFY(GetType(is_const<IntReference>()) == false);

	static_assert(is_const<ConstIntReference>::value == false, "is_const failure"); // Note here that the int is const, not the reference to the int.
	EATEST_VERIFY(GetType(is_const<ConstIntReference>()) == false);

	static_assert(is_const<ConstVolatileIntReference>::value == false, "is_const failure"); // Note here that the int is const, not the reference to the int.
	EATEST_VERIFY(GetType(is_const<ConstVolatileIntReference>()) == false);

	static_assert(is_const<void() const>::value == false, "is_const failure");
	EATEST_VERIFY(GetType(is_const<void() const>()) == false);

	// is_volatile
	static_assert(is_volatile<Int>::value == false, "is_volatile failure");
	EATEST_VERIFY(GetType(is_volatile<Int>()) == false);

	static_assert(is_volatile<ConstInt>::value == false, "is_volatile failure");
	EATEST_VERIFY(GetType(is_volatile<ConstInt>()) == false);

	static_assert(is_volatile<VolatileInt>::value == true, "is_volatile failure");
	EATEST_VERIFY(GetType(is_volatile<VolatileInt>()) == true);

	static_assert(is_volatile<ConstVolatileInt>::value == true, "is_volatile failure");
	EATEST_VERIFY(GetType(is_volatile<ConstVolatileInt>()) == true);

	static_assert(is_volatile<IntReference>::value == false, "is_volatile failure");
	EATEST_VERIFY(GetType(is_volatile<IntReference>()) == false);

	static_assert(is_volatile<ConstIntReference>::value == false, "is_volatile failure");
	EATEST_VERIFY(GetType(is_volatile<ConstIntReference>()) == false);

	static_assert(is_volatile<ConstVolatileIntReference>::value == false, "is_volatile failure"); // Note here that the int is volatile, not the reference to the int.
	EATEST_VERIFY(GetType(is_volatile<ConstVolatileIntReference>()) == false);

	static_assert(is_volatile<void() const>::value == false, "is_volatile failure");
	EATEST_VERIFY(GetType(is_volatile<void() const>()) == false);


	// underlying_type and to_underlying
	#if EASTL_TYPE_TRAIT_underlying_type_CONFORMANCE && !defined(EA_COMPILER_NO_STRONGLY_TYPED_ENUMS) // If we can execute this test...
		enum UnderlyingTypeTest : uint16_t { firstVal = 0, secondVal = 1 };
		
		constexpr bool isUnderlyingTypeCorrect = is_same_v<underlying_type_t<UnderlyingTypeTest>, uint16_t>;
		static_assert(isUnderlyingTypeCorrect, "Wrong type for underlying_type_t.");
		EATEST_VERIFY(isUnderlyingTypeCorrect);
		
		auto v1 = to_underlying(UnderlyingTypeTest::firstVal); 
		auto v2 = to_underlying(UnderlyingTypeTest::secondVal); 

		constexpr bool isToUnderlyingReturnTypeCorrect = is_same_v<decltype(v1), uint16_t>;
		static_assert(isToUnderlyingReturnTypeCorrect, "Wrong return type for to_underlying.");
		EATEST_VERIFY(isToUnderlyingReturnTypeCorrect);

		EATEST_VERIFY(v1 == 0 && v2 == 1);
	#endif


	// is_literal_type
	static_assert((is_literal_type<int>::value == true),          "is_literal_type failure");
	static_assert((is_literal_type<Enum>::value == true),         "is_literal_type failure");
	#if EASTL_TYPE_TRAIT_is_literal_type_CONFORMANCE
		static_assert((is_literal_type<PodA>::value == true),     "is_literal_type failure");
		static_assert((is_literal_type<NonPod1>::value == false), "is_literal_type failure");
	#endif


	// is_trivial
	// is_trivially_copyable
	// is_trivially_default_constructible
	#if EASTL_TYPE_TRAIT_is_trivial_CONFORMANCE
		static_assert(is_trivial<Pod1>::value == true, "is_trivial failure");
		static_assert(is_trivial<NonPod1>::value == false, "is_trivial failure");
	#endif


	// is_pod
	static_assert(is_pod<Pod1>::value == true, "is_pod failure");
	EATEST_VERIFY(GetType(is_pod<Pod1>()) == true);

	static_assert(is_pod<Pod2>::value == true, "is_pod failure");
	EATEST_VERIFY(GetType(is_pod<Pod2>()) == true);

	static_assert(is_pod<Pod3>::value == true, "is_pod failure");
	EATEST_VERIFY(GetType(is_pod<Pod3>()) == true);

	static_assert(is_pod<float>::value == true, "is_pod failure");
	EATEST_VERIFY(GetType(is_pod<float>()) == true);

	static_assert(is_pod<Pod1*>::value == true, "is_pod failure");
	EATEST_VERIFY(GetType(is_pod<Pod1*>()) == true);

	static_assert(is_pod<NonPod1>::value == false, "is_pod failure");
	EATEST_VERIFY(GetType(is_pod<NonPod1>()) == false);

	static_assert(is_pod<NonPod2>::value == false, "is_pod failure");
	EATEST_VERIFY(GetType(is_pod<NonPod2>()) == false);


	// is_standard_layout
	static_assert(is_standard_layout<Pod1>::value == true, "is_standard_layout<Pod1> failure");
	static_assert(is_standard_layout_v<Pod1> == true,      "is_standard_layout<Pod1> failure");
	EATEST_VERIFY(GetType(is_standard_layout<Pod1>()) == true);

	static_assert(is_standard_layout<Pod2>::value == true, "is_standard_layout<Pod2> failure");
	static_assert(is_standard_layout_v<Pod2> == true,      "is_standard_layout<Pod2> failure");
	EATEST_VERIFY(GetType(is_standard_layout<Pod2>()) == true);

	static_assert(is_standard_layout<Pod3>::value == true, "is_standard_layout<Pod3> failure");
	static_assert(is_standard_layout_v<Pod3> == true,      "is_standard_layout<Pod3> failure");
	EATEST_VERIFY(GetType(is_standard_layout<Pod3>()) == true);

	static_assert(is_standard_layout<float>::value == true, "is_standard_layout<float> failure");
	static_assert(is_standard_layout_v<float> == true,      "is_standard_layout<float> failure");
	EATEST_VERIFY(GetType(is_standard_layout<float>()) == true);

	static_assert(is_standard_layout<Pod1*>::value == true, "is_standard_layout<Pod1*> failure");
	static_assert(is_standard_layout_v<Pod1*> == true,      "is_standard_layout<Pod1*> failure");
	EATEST_VERIFY(GetType(is_standard_layout<Pod1*>()) == true);

	static_assert(is_standard_layout<NonPod1>::value == false, "is_standard_layout<NonPod1> failure");
	static_assert(is_standard_layout_v<NonPod1> == false,      "is_standard_layout<NonPod1> failure");
	EATEST_VERIFY(GetType(is_standard_layout<NonPod1>()) == false);

	static_assert(is_standard_layout<NonPod2>::value == false, "is_standard_layout<NonPod2> failure");
	static_assert(is_standard_layout_v<NonPod2> == false,      "is_standard_layout<NonPod2> failure");
	EATEST_VERIFY(GetType(is_standard_layout<NonPod2>()) == false);

	static_assert(is_standard_layout<HasTrivialConstructor>::value == true, "is_standard_layout<HasTrivialConstructor> failure");
	static_assert(is_standard_layout_v<HasTrivialConstructor> == true,      "is_standard_layout<HasTrivialConstructor> failure");
	EATEST_VERIFY(GetType(is_standard_layout<HasTrivialConstructor>()) == true);

	static_assert(is_standard_layout<NoTrivialConstructor>::value == true, "is_standard_layout<NoTrivialConstructor> failure");        // A key difference between a POD and Standard Layout is that the latter is true if there is a constructor.
	static_assert(is_standard_layout_v<NoTrivialConstructor> == true, "is_standard_layout<NoTrivialConstructor> failure");        // A key difference between a POD and Standard Layout is that the latter is true if there is a constructor.
	EATEST_VERIFY(GetType(is_standard_layout<NoTrivialConstructor>()) == true);


	// is_empty
	static_assert(is_empty<ClassEmpty>::value == true,  "is_empty failure");
	EATEST_VERIFY(GetType(is_empty<ClassEmpty>()) == true);

	static_assert(is_empty<ClassNonEmpty>::value == false,  "is_empty failure");
	EATEST_VERIFY(GetType(is_empty<ClassNonEmpty>()) == false);

	static_assert(is_empty<int>::value == false,  "is_empty failure");
	EATEST_VERIFY(GetType(is_empty<int>()) == false);

	static_assert(is_empty<Enum>::value == false,  "is_empty failure");
	EATEST_VERIFY(GetType(is_empty<Enum>()) == false);


	// is_polymorphic
	static_assert(is_polymorphic<Polymorphic1>::value == true,  "has_trivial_constructor failure");
	EATEST_VERIFY(GetType(is_polymorphic<Polymorphic1>()) == true);

	static_assert(is_polymorphic<Polymorphic2>::value == true,  "has_trivial_constructor failure");
	EATEST_VERIFY(GetType(is_polymorphic<Polymorphic2>()) == true);

	static_assert(is_polymorphic<Polymorphic3>::value == true,  "has_trivial_constructor failure");
	EATEST_VERIFY(GetType(is_polymorphic<Polymorphic3>()) == true);

	static_assert(is_polymorphic<NonPolymorphic1>::value == false,  "has_trivial_constructor failure");
	EATEST_VERIFY(GetType(is_polymorphic<NonPolymorphic1>()) == false);

	static_assert(is_polymorphic<int>::value == false,  "has_trivial_constructor failure");
	EATEST_VERIFY(GetType(is_polymorphic<int>()) == false);

	static_assert(is_polymorphic<Polymorphic1*>::value == false,  "has_trivial_constructor failure");
	EATEST_VERIFY(GetType(is_polymorphic<Polymorphic1*>()) == false);


	// has_trivial_constructor
	static_assert(has_trivial_constructor<int>::value == true,  "has_trivial_constructor failure");
	EATEST_VERIFY(GetType(has_trivial_constructor<int>()) == true);

	static_assert(has_trivial_constructor<int*>::value == true,  "has_trivial_constructor failure");
	EATEST_VERIFY(GetType(has_trivial_constructor<int*>()) == true);

	static_assert(has_trivial_constructor<HasTrivialConstructor>::value == true,  "has_trivial_constructor failure");
	EATEST_VERIFY(GetType(has_trivial_constructor<HasTrivialConstructor>()) == true);

	static_assert(has_trivial_constructor<NoTrivialConstructor>::value == false,  "has_trivial_constructor failure");
	EATEST_VERIFY(GetType(has_trivial_constructor<NoTrivialConstructor>()) == false);

	static_assert(has_trivial_constructor<int&>::value == false,  "has_trivial_constructor failure");
	EATEST_VERIFY(GetType(has_trivial_constructor<int&>()) == false);


	// has_trivial_copy
	static_assert(has_trivial_copy<int>::value == true,             "has_trivial_copy failure");
	EATEST_VERIFY(GetType(has_trivial_copy<int>()) == true);

	static_assert(has_trivial_copy<int*>::value == true,            "has_trivial_copy failure");
	EATEST_VERIFY(GetType(has_trivial_copy<int*>()) == true);

	static_assert(has_trivial_copy<HasTrivialCopy>::value == true,   "has_trivial_copy failure");
	EATEST_VERIFY(GetType(has_trivial_copy<HasTrivialCopy>()) == true);

	static_assert(has_trivial_copy<NoTrivialCopy1>::value == false,  "has_trivial_copy failure");
	EATEST_VERIFY(GetType(has_trivial_copy<NoTrivialCopy1>()) == false);

	static_assert(has_trivial_copy<NoTrivialCopy2>::value == false,  "has_trivial_copy failure");
	EATEST_VERIFY(GetType(has_trivial_copy<NoTrivialCopy2>()) == false);


	// has_trivial_assign
	static_assert(has_trivial_assign<int>::value == true,               "has_trivial_assign failure");
	EATEST_VERIFY(GetType(has_trivial_assign<int>()) == true);

	static_assert(has_trivial_assign<int*>::value == true,              "has_trivial_assign failure");
	EATEST_VERIFY(GetType(has_trivial_assign<int*>()) == true);

	static_assert(has_trivial_assign<HasTrivialAssign>::value == true,  "has_trivial_assign failure");
	EATEST_VERIFY(GetType(has_trivial_assign<HasTrivialAssign>()) == true);

	static_assert(has_trivial_assign<NoTrivialAssign1>::value == false, "has_trivial_assign failure");
	EATEST_VERIFY(GetType(has_trivial_assign<NoTrivialAssign1>()) == false);

	static_assert(has_trivial_assign<NoTrivialAssign2>::value == false, "has_trivial_assign failure");
	EATEST_VERIFY(GetType(has_trivial_assign<NoTrivialAssign2>()) == false);


	// has_trivial_destructor
	static_assert(has_trivial_assign<int>::value == true,  "has_trivial_relocate failure");
	EATEST_VERIFY(GetType(has_trivial_assign<int>()) == true);

	static_assert(has_trivial_assign<int*>::value == true,  "has_trivial_relocate failure");
	EATEST_VERIFY(GetType(has_trivial_assign<int*>()) == true);


	// has_trivial_relocate
	static_assert(has_trivial_relocate<int>::value == true,  "has_trivial_relocate failure");
	EATEST_VERIFY(GetType(has_trivial_relocate<int>()) == true);

	static_assert(has_trivial_relocate<int*>::value == true,  "has_trivial_relocate failure");
	EATEST_VERIFY(GetType(has_trivial_relocate<int*>()) == true);


	// is_signed
	static_assert(is_signed<int>::value == true,                "is_signed failure ");
	static_assert(is_signed_v<int> == true,                     "is_signed failure ");
	EATEST_VERIFY(GetType(is_signed<int>()) == true);

	static_assert(is_signed<const int64_t>::value == true,      "is_signed failure ");
	static_assert(is_signed_v<const int64_t> == true,           "is_signed failure ");
	EATEST_VERIFY(GetType(is_signed<const int64_t>()) == true);

	static_assert(is_signed<uint32_t>::value == false,          "is_signed failure ");
	static_assert(is_signed_v<uint32_t> == false,               "is_signed failure ");
	EATEST_VERIFY(GetType(is_signed<uint32_t>()) == false);

	static_assert(is_signed<bool>::value == false,              "is_signed failure ");
	static_assert(is_signed_v<bool> == false,                   "is_signed failure ");
	EATEST_VERIFY(GetType(is_signed<bool>()) == false);

	static_assert(is_signed<float>::value == true,              "is_signed failure ");
	static_assert(is_signed_v<float> == true,                   "is_signed failure ");
	EATEST_VERIFY(GetType(is_signed<float>()) == true);

	static_assert(is_signed<double>::value == true,             "is_signed failure ");
	static_assert(is_signed_v<double> == true,                  "is_signed failure ");
	EATEST_VERIFY(GetType(is_signed<double>()) == true);
	
	static_assert(is_signed<char16_t>::value == false,			"is_signed failure ");
	static_assert(is_signed_v<char16_t> == false,				"is_signed failure ");
	EATEST_VERIFY(GetType(is_signed<char16_t>()) == false);

	static_assert(is_signed<char32_t>::value == false,			"is_signed failure ");
	static_assert(is_signed_v<char32_t> == false,				"is_signed failure ");
	EATEST_VERIFY(GetType(is_signed<char32_t>()) == false);

#if EASTL_GCC_STYLE_INT128_SUPPORTED
	static_assert(is_signed<__int128_t>::value == true,			"is_signed failure ");
	static_assert(is_signed_v<__int128_t> == true,				"is_signed failure ");
	EATEST_VERIFY(GetType(is_signed<__int128_t>()) == true);

	static_assert(is_signed<__uint128_t>::value == false,		"is_signed failure ");
	static_assert(is_signed_v<__uint128_t> == false,			"is_signed failure ");
	EATEST_VERIFY(GetType(is_signed<__uint128_t>()) == false);
#endif

	// is_unsigned
	static_assert(is_unsigned<unsigned int>::value == true,        "is_unsigned failure ");
	static_assert(is_unsigned_v<unsigned int> == true,             "is_unsigned failure ");
	EATEST_VERIFY(GetType(is_unsigned<unsigned int>()) == true);

	static_assert(is_unsigned<const uint64_t>::value == true,      "is_unsigned failure ");
	static_assert(is_unsigned_v<const uint64_t> == true,           "is_unsigned failure ");
	EATEST_VERIFY(GetType(is_unsigned<const uint64_t>()) == true);

	static_assert(is_unsigned<int32_t>::value == false,            "is_unsigned failure ");
	static_assert(is_unsigned_v<int32_t> == false,                 "is_unsigned failure ");
	EATEST_VERIFY(GetType(is_unsigned<int32_t>()) == false);

	static_assert(is_unsigned<bool>::value == true,                "is_unsigned failure ");
	static_assert(is_unsigned_v<bool> == true,                     "is_unsigned failure ");
	EATEST_VERIFY(GetType(is_unsigned<bool>()) == true);

	static_assert(is_unsigned<float>::value == false,              "is_unsigned failure ");
	static_assert(is_unsigned_v<float> == false,                   "is_unsigned failure ");
	EATEST_VERIFY(GetType(is_unsigned<float>()) == false);

	static_assert(is_unsigned<double>::value == false,             "is_unsigned failure ");
	static_assert(is_unsigned_v<double> == false,                  "is_unsigned failure ");
	EATEST_VERIFY(GetType(is_unsigned<double>()) == false);
	
	static_assert(is_unsigned<char16_t>::value == true,			   "is_unsigned failure ");
	static_assert(is_unsigned_v<char16_t> == true,				   "is_unsigned failure ");
	EATEST_VERIFY(GetType(is_unsigned<char16_t>()) == true);

	static_assert(is_unsigned<char32_t>::value == true,			   "is_unsigned failure ");
	static_assert(is_unsigned_v<char32_t> == true,				   "is_unsigned failure ");
	EATEST_VERIFY(GetType(is_unsigned<char32_t>()) == true);

#if EASTL_GCC_STYLE_INT128_SUPPORTED
	static_assert(is_unsigned<__int128_t>::value == false,		   "is_unsigned failure ");
	static_assert(is_unsigned_v<__int128_t> == false,			   "is_unsigned failure ");
	EATEST_VERIFY(GetType(is_unsigned<__int128_t>()) == false);

	static_assert(is_unsigned<__uint128_t>::value == true,		   "is_unsigned failure ");
	static_assert(is_unsigned_v<__uint128_t> == true,			   "is_unsigned failure ");
	EATEST_VERIFY(GetType(is_unsigned<__uint128_t>()) == true);
#endif


	// is_lvalue_reference
	static_assert((is_lvalue_reference<Class>::value == false),   "is_lvalue_reference failure");
	static_assert((is_lvalue_reference<Class&>::value == true),   "is_lvalue_reference failure");
	static_assert((is_lvalue_reference<Class&&>::value == false), "is_lvalue_reference failure");
	static_assert((is_lvalue_reference<int>::value == false),     "is_lvalue_reference failure");
	static_assert((is_lvalue_reference<int&>::value == true),     "is_lvalue_reference failure");
	static_assert((is_lvalue_reference<int&&>::value == false),   "is_lvalue_reference failure");

	static_assert((is_lvalue_reference_v<Class> == false),        "is_lvalue_reference failure");
	static_assert((is_lvalue_reference_v<Class&> == true),        "is_lvalue_reference failure");
	static_assert((is_lvalue_reference_v<Class&&> == false),      "is_lvalue_reference failure");
	static_assert((is_lvalue_reference_v<int> == false),          "is_lvalue_reference failure");
	static_assert((is_lvalue_reference_v<int&> == true),          "is_lvalue_reference failure");
	static_assert((is_lvalue_reference_v<int&&> == false),        "is_lvalue_reference failure");


	// is_rvalue_reference
	static_assert((is_rvalue_reference<Class>::value == false),  "is_rvalue_reference failure");
	static_assert((is_rvalue_reference<Class&>::value == false), "is_rvalue_reference failure");
	static_assert((is_rvalue_reference<Class&&>::value == true), "is_rvalue_reference failure");
	static_assert((is_rvalue_reference<int>::value == false),    "is_rvalue_reference failure");
	static_assert((is_rvalue_reference<int&>::value == false),   "is_rvalue_reference failure");
	static_assert((is_rvalue_reference<int&&>::value == true),   "is_rvalue_reference failure");

	static_assert((is_rvalue_reference_v<Class> == false),  "is_rvalue_reference failure");
	static_assert((is_rvalue_reference_v<Class&> == false), "is_rvalue_reference failure");
	static_assert((is_rvalue_reference_v<Class&&> == true), "is_rvalue_reference failure");
	static_assert((is_rvalue_reference_v<int> == false),    "is_rvalue_reference failure");
	static_assert((is_rvalue_reference_v<int&> == false),   "is_rvalue_reference failure");
	static_assert((is_rvalue_reference_v<int&&> == true),   "is_rvalue_reference failure");


	// is_assignable
	// See the documentation for is_assignable to understand the results below are as they are.
	static_assert((eastl::is_assignable<int&, int>::value              == true),   "is_assignable failure");
	static_assert((eastl::is_assignable<const int&, int>::value        == false),  "is_assignable failure");
	static_assert((eastl::is_assignable<char*, int*>::value            == false),  "is_assignable failure");
	static_assert((eastl::is_assignable<char*, const char*>::value     == false),  "is_assignable failure");
	static_assert((eastl::is_assignable<PodA, PodB*>::value            == false),  "is_assignable failure");
	static_assert((eastl::is_assignable<Assignable, Pod2>::value       == false),  "is_assignable failure");

	#if EASTL_TYPE_TRAIT_is_assignable_CONFORMANCE
		// These might not succeed unless the implementation is conforming.
		static_assert((eastl::is_assignable<Assignable, Assignable>::value == true),  "is_assignable failure");
		static_assert((eastl::is_assignable<Assignable, Pod1>::value       == true),  "is_assignable failure");
		static_assert((eastl::is_assignable<PodA&, PodA>::value            == true),  "is_assignable failure");

		// These cannot succeed unless the implementation is conforming.
		static_assert((eastl::is_assignable<void, void>::value             == false),  "is_assignable failure");
		static_assert((eastl::is_assignable<int, int>::value               == false),  "is_assignable failure");
		static_assert((eastl::is_assignable<int, const int>::value         == false),  "is_assignable failure");
		static_assert((eastl::is_assignable<const int, int>::value         == false),  "is_assignable failure");
		static_assert((eastl::is_assignable<int, int&>::value              == false),  "is_assignable failure");
		static_assert((eastl::is_assignable<int64_t, int8_t>::value        == false),  "is_assignable failure");
		static_assert((eastl::is_assignable<bool, bool>::value             == false),  "is_assignable failure");
		static_assert((eastl::is_assignable<char*, char*>::value           == false),  "is_assignable failure");
		static_assert((eastl::is_assignable<int, float>::value             == false),  "is_assignable failure");
		static_assert((eastl::is_assignable<const char*, char*>::value     == false),  "is_assignable failure");
		static_assert((eastl::is_assignable<int[], int[]>::value           == false),  "is_assignable failure");
	#endif


	// is_lvalue_assignable
	static_assert((eastl::is_lvalue_assignable<int&, int>::value              == true),   "is_lvalue_assignable failure");
	static_assert((eastl::is_lvalue_assignable<char*, int*>::value            == false),  "is_lvalue_assignable failure");
	static_assert((eastl::is_lvalue_assignable<char*, const char*>::value     == false),  "is_lvalue_assignable failure");
	static_assert((eastl::is_lvalue_assignable<PodA, PodB*>::value            == false),  "is_lvalue_assignable failure");
	static_assert((eastl::is_lvalue_assignable<Assignable, Pod2>::value       == false),  "is_lvalue_assignable failure");

	#if EASTL_TYPE_TRAIT_is_lvalue_assignable_CONFORMANCE
		// These might not succeed unless the implementation is conforming.
		static_assert((eastl::is_lvalue_assignable<Assignable, Assignable>::value == true),  "is_lvalue_assignable failure");
		static_assert((eastl::is_lvalue_assignable<Assignable, Pod1>::value       == true),  "is_lvalue_assignable failure");

		// These cannot succeed unless the implementation is conforming.
		static_assert((eastl::is_lvalue_assignable<void, void>::value             == false),  "is_lvalue_assignable failure");
		static_assert((eastl::is_lvalue_assignable<int, int>::value               == true),   "is_lvalue_assignable failure");
		static_assert((eastl::is_lvalue_assignable<int, const int>::value         == true),   "is_lvalue_assignable failure");
		static_assert((eastl::is_lvalue_assignable<const int, int>::value         == false),  "is_lvalue_assignable failure");
		static_assert((eastl::is_lvalue_assignable<int, int&>::value              == true),   "is_lvalue_assignable failure");
		static_assert((eastl::is_lvalue_assignable<int64_t, int8_t>::value        == true),   "is_lvalue_assignable failure");
		static_assert((eastl::is_lvalue_assignable<bool, bool>::value             == true),   "is_lvalue_assignable failure");
		static_assert((eastl::is_lvalue_assignable<char*, char*>::value           == true),   "is_lvalue_assignable failure");
		static_assert((eastl::is_lvalue_assignable<const char*, char*>::value     == true),   "is_lvalue_assignable failure");
		static_assert((eastl::is_lvalue_assignable<int[], int[]>::value           == false),  "is_lvalue_assignable failure");
		static_assert((eastl::is_lvalue_assignable<int[3], int[3]>::value         == false),  "is_lvalue_assignable failure"); // Despite that you can memcpy these, C++ syntax doesn't all =-based assignment.

		#if !defined(EA_COMPILER_EDG) // EDG (and only EDG) is issuing int8_t->double conversion warnings from the decltype expression inside this trait. That's probably a compiler bug, though we need to verify.
			static_assert((eastl::is_lvalue_assignable<double, int8_t>::value     == true),   "is_lvalue_assignable failure"); // Sure this might generate a warning, but it's valid syntax.
		#endif
	#endif


	// is_copy_assignable
	static_assert((eastl::is_copy_assignable<int&>::value              == true),   "is_copy_assignable failure");
	static_assert((eastl::is_copy_assignable<char>::value              == true),   "is_copy_assignable failure");

	#if EASTL_TYPE_TRAIT_is_assignable_CONFORMANCE
		// These might not succeed unless the implementation is conforming.
		static_assert((eastl::is_copy_assignable<Assignable>::value    == true),  "is_copy_assignable failure");
		static_assert((eastl::is_copy_assignable<Assignable>::value    == true),  "is_copy_assignable failure");

		// These cannot succeed unless the implementation is conforming.
		static_assert((eastl::is_copy_assignable<char*>::value         == true),   "is_copy_assignable failure");
		static_assert((eastl::is_copy_assignable<PodA>::value          == true),   "is_copy_assignable failure");
		static_assert((eastl::is_copy_assignable<Assignable>::value    == true),   "is_copy_assignable failure");
		static_assert((eastl::is_copy_assignable<void>::value          == false),  "is_copy_assignable failure");
		static_assert((eastl::is_copy_assignable<int>::value           == true),   "is_copy_assignable failure");
		static_assert((eastl::is_copy_assignable<const int>::value     == false),  "is_copy_assignable failure");
		static_assert((eastl::is_copy_assignable<int64_t>::value       == true),   "is_copy_assignable failure");
		static_assert((eastl::is_copy_assignable<bool>::value          == true),   "is_copy_assignable failure");
		static_assert((eastl::is_copy_assignable<char*>::value         == true),   "is_copy_assignable failure");
		static_assert((eastl::is_copy_assignable<const char*>::value   == true),   "is_copy_assignable failure");
		static_assert((eastl::is_copy_assignable<int[3]>::value        == false),  "is_copy_assignable failure");
		static_assert((eastl::is_copy_assignable<int[]>::value         == false),  "is_copy_assignable failure");
	#endif


	// is_trivially_assignable
	static_assert((eastl::is_trivially_assignable<int&, int>::value              == true),   "is_trivially_assignable failure");
	static_assert((eastl::is_trivially_assignable<char*, int*>::value            == false),  "is_trivially_assignable failure");
	static_assert((eastl::is_trivially_assignable<char*, const char*>::value     == false),  "is_trivially_assignable failure");
	static_assert((eastl::is_trivially_assignable<PodA, PodB*>::value            == false),  "is_trivially_assignable failure");
	static_assert((eastl::is_trivially_assignable<Assignable, Assignable>::value == false),  "is_trivially_assignable failure"); // False because not trivial.
	static_assert((eastl::is_trivially_assignable<Assignable, Pod1>::value       == false),  "is_trivially_assignable failure"); // False because not trivial.
	static_assert((eastl::is_trivially_assignable<Assignable, Pod2>::value == false),        "is_trivially_assignable failure");

	// is_nothrow_assignable
	static_assert((is_nothrow_assignable<void, void>::value          == false),  "is_nothrow_assignable failure");
	static_assert((is_nothrow_assignable<int32_t, int32_t>::value    == false),  "is_nothrow_assignable failure");    // See is_assignable for why this is so.
	static_assert((is_nothrow_assignable<int32_t&, int32_t>::value   == true),   "is_nothrow_assignable failure");
	static_assert((is_nothrow_assignable<int32_t, int8_t>::value     == false),  "is_nothrow_assignable failure");
	#if EASTL_TYPE_TRAIT_is_nothrow_assignable_CONFORMANCE
		static_assert((is_nothrow_assignable<int32_t&, int8_t>::value    == true),   "is_nothrow_assignable failure");
		static_assert((is_nothrow_assignable<NoThrowAssignable,   NoThrowAssignable>::value   == true),  "is_nothrow_assignable failure");
		static_assert((is_nothrow_assignable<ThrowAssignableTest, NoThrowAssignable>::value   == true),  "is_nothrow_assignable failure");
		static_assert((is_nothrow_assignable<ThrowAssignableTest, ThrowAssignableTest>::value == false), "is_nothrow_assignable failure");
	#endif


	// is_array_of_known_bounds
	// is_array_of_unknown_bounds
	static_assert(is_array_of_known_bounds<void>::value        == false,  "is_array_of_known_bounds failure");
	static_assert(is_array_of_known_bounds<int>::value         == false,  "is_array_of_known_bounds failure");
	static_assert(is_array_of_known_bounds<PodA>::value        == false,  "is_array_of_known_bounds failure");
	static_assert(is_array_of_known_bounds<int[3]>::value      == true,   "is_array_of_known_bounds failure");
	static_assert(is_array_of_known_bounds<int[]>::value       == false,  "is_array_of_known_bounds failure");
	static_assert(is_array_of_known_bounds<int[0]>::value	   == false,  "is_array_of_known_bounds failure");

	static_assert(is_array_of_unknown_bounds<void>::value      == false,  "is_array_of_unknown_bounds failure");
	static_assert(is_array_of_unknown_bounds<int>::value       == false,  "is_array_of_unknown_bounds failure");
	static_assert(is_array_of_unknown_bounds<PodA>::value      == false,  "is_array_of_unknown_bounds failure");
	static_assert(is_array_of_unknown_bounds<int[3]>::value    == false,  "is_array_of_unknown_bounds failure");
	static_assert(is_array_of_unknown_bounds<int[]>::value     == true,   "is_array_of_unknown_bounds failure");
	static_assert(is_array_of_unknown_bounds<int[0]>::value	   == false,  "is_array_of_unknown_bounds failure");


	// is_trivially_copyable
	static_assert(is_trivially_copyable<void>::value           == false,  "is_trivially_copyable failure");
	EATEST_VERIFY(GetType(is_trivially_copyable<void>())	   == false);
	static_assert(is_trivially_copyable<int>::value            == true,   "is_trivially_copyable failure");
	static_assert(is_trivially_copyable<int*>::value           == true,   "is_trivially_copyable failure");
	static_assert(is_trivially_copyable<int[]>::value          == true,   "is_trivially_copyable failure");
	static_assert(is_trivially_copyable<int[4]>::value         == true,   "is_trivially_copyable failure");
	#if EASTL_TYPE_TRAIT_is_trivially_copyable_CONFORMANCE
		static_assert(is_trivially_copyable<NonPod1>::value        == false,  "is_trivially_copyable failure");
		static_assert(is_trivially_copyable<NoTrivialCopy1>::value == false,  "is_trivially_copyable failure");
		static_assert(is_trivially_copyable<PodA>::value           == true,   "is_trivially_copyable failure");
	#endif

	{  // user reported regression
		struct Foo
		{
			int a;
			Foo(int i) : a(i) {}
			Foo(Foo&& other) : a(other.a) { other.a = 0; }

			Foo(const Foo&) = delete;
			Foo& operator=(const Foo&) = delete;
		};

		static_assert(!eastl::is_trivially_copyable<Foo>::value, "is_trivially_copyable failure");
	}


	// is_trivially_copy_assignable
	{
		static_assert(is_trivially_copy_assignable<int>::value == true, "is_trivially_copy_assignable failure");
		static_assert(is_trivially_copy_assignable<char*>::value == true,       "is_trivially_copy_assignable failure");
		static_assert(is_trivially_copy_assignable<const char*>::value == true, "is_trivially_copy_assignable failure");
		static_assert(is_trivially_copy_assignable<NoTrivialCopy1>::value == false, "is_trivially_copy_assignable failure");

	#ifdef INTENTIONALLY_DISABLED
		// These tests currently fail on clang, but they would pass using the std::is_trivially_copy_assignable trait.  We should
		// determine if our implementation is correct, or if clang is actually incorrect.
		static_assert(is_trivially_copy_assignable<const int>::value == true, "is_trivially_copy_assignable failure");
		static_assert(is_trivially_copy_assignable<const PodA>::value == true, "is_trivially_copy_assignable failure");
		static_assert(is_trivially_copy_assignable<PodA>::value == true, "is_trivially_copy_assignable failure");
	#endif
	}
	// is_trivially_default_constructible
	// To do.


	// is_trivial
	// To do.


	// is_constructible
	static_assert(is_constructible<void>::value           == false,  "is_constructible failure");
	static_assert(is_constructible<const void>::value     == false,  "is_constructible failure");
	static_assert(is_constructible<int>::value            == true,   "is_constructible failure");
	static_assert(is_constructible<int&>::value           == false,  "is_constructible failure");
	static_assert(is_constructible<int&&>::value          == false,  "is_constructible failure");
	static_assert(is_constructible<int*>::value           == true,   "is_constructible failure");
	static_assert(is_constructible<int[]>::value          == false,  "is_constructible failure");
	static_assert(is_constructible<int[4]>::value         == true,   "is_constructible failure");
	static_assert(is_constructible<NonPod1>::value        == true,  " is_constructible failure");
	static_assert(is_constructible<NoTrivialCopy1>::value == true,   "is_constructible failure");
	static_assert(is_constructible<PodA>::value           == true,   "is_constructible failure");
	static_assert(is_constructible<Abstract>::value       == false,  "is_constructible failure");
	static_assert(is_constructible<NonCopyable>::value       == true,  "is_constructible failure");
	#if EASTL_TYPE_TRAIT_is_trivially_constructible_CONFORMANCE
		static_assert((is_constructible<int, const int>::value                   == true),   "is_constructible failure");
		static_assert((is_constructible<char*, const char*>::value               == false),  "is_constructible failure");
		static_assert((is_constructible<char*, char* const>::value               == true),   "is_constructible failure");
		static_assert((is_constructible<ThrowConstructibleTest, int>::value      == true),   "is_constructible failure");
		static_assert((is_constructible<ThrowConstructibleTest, float>::value    == true),   "is_constructible failure");
	#endif


	// is_trivially_constructible
	// Need double parentheses because some older compilers need static_assert implemented as a macro.
	static_assert((is_trivially_constructible<void>::value                     == false),  "is_trivially_constructible failure");
	static_assert((is_trivially_constructible<void, void>::value               == false),  "is_trivially_constructible failure");
	static_assert((is_trivially_constructible<void, int>::value                == false),  "is_trivially_constructible failure");
	static_assert((is_trivially_constructible<int>::value                      == true),   "is_trivially_constructible failure");
	static_assert((is_trivially_constructible<int, int>::value                 == true),   "is_trivially_constructible failure");
	static_assert((is_trivially_constructible<int, Abstract>::value            == false),  "is_trivially_constructible failure");
	static_assert((is_trivially_constructible<int*>::value                     == true),   "is_trivially_constructible failure");
	static_assert((is_trivially_constructible<int[]>::value                    == false),  "is_trivially_constructible failure");
	static_assert((is_trivially_constructible<int[], int[]>::value             == false),  "is_trivially_constructible failure");
	static_assert((is_trivially_constructible<int[4]>::value                   == true),   "is_trivially_constructible failure");
	static_assert((is_trivially_constructible<int[4], int[]>::value            == false),  "is_trivially_constructible failure");
	#if EASTL_TYPE_TRAIT_is_trivially_constructible_CONFORMANCE
		static_assert((is_trivially_constructible<NoTrivialCopy1>::value       == false),  "is_trivially_constructible failure");
		static_assert((is_trivially_constructible<PodA>::value                 == true),   "is_trivially_constructible failure");
		static_assert((is_trivially_constructible<PodA, PodA>::value           == true),   "is_trivially_constructible failure");
		static_assert((is_trivially_constructible<Abstract>::value             == false),  "is_trivially_constructible failure");
		static_assert((is_trivially_constructible<NonPod1>::value              == false),  "is_trivially_constructible failure");
		static_assert((is_trivially_constructible<NoTrivialConstructor>::value == false),  "is_trivially_constructible failure");
	#endif


	// is_nothrow_constructible
	static_assert((is_nothrow_constructible<void>::value           == false),  "is_nothrow_constructible failure");
	static_assert((is_nothrow_constructible<int>::value            == true),   "is_nothrow_constructible failure");
	static_assert((is_nothrow_constructible<int*>::value           == true),   "is_nothrow_constructible failure");
	static_assert((is_nothrow_constructible<int[4]>::value         == true),   "is_nothrow_constructible failure");
	#if EASTL_TYPE_TRAIT_is_nothrow_constructible_CONFORMANCE
		static_assert((is_nothrow_constructible<int[]>::value                            == false),  "is_nothrow_constructible failure");
		static_assert((is_nothrow_constructible<Abstract>::value                         == false),  "is_nothrow_constructible failure");
		static_assert((is_nothrow_constructible<int, const int>::value                   == true),   "is_nothrow_constructible failure");
		static_assert((is_nothrow_constructible<char*, const char*>::value               == false),  "is_nothrow_constructible failure");
		static_assert((is_nothrow_constructible<char*, char* const>::value               == true),   "is_nothrow_constructible failure");
		static_assert((is_nothrow_constructible<NonPod1>::value                          == false),  "is_nothrow_constructible failure");
		static_assert((is_nothrow_constructible<PodA>::value                             == true),   "is_nothrow_constructible failure");
		static_assert((is_nothrow_constructible<ThrowConstructibleTest, int>::value      == true),   "is_nothrow_constructible failure");
		static_assert((is_nothrow_constructible<ThrowConstructibleTest, float>::value    == false),  "is_nothrow_constructible failure");
		static_assert((is_nothrow_constructible<NoTrivialCopy1>::value                   == true),   "is_nothrow_constructible failure");  //True because it's a compiler-generated constructor.
	#endif


	// is_nothrow_move_constructible
#if !defined(EA_PLATFORM_MICROSOFT) 
	static_assert((is_nothrow_move_constructible<void>::value                  == false),    "is_nothrow_move_constructible failure");
	static_assert((is_nothrow_move_constructible<int>::value                   == true),     "is_nothrow_move_constructible failure");
	static_assert((is_nothrow_move_constructible<int*>::value                  == true),     "is_nothrow_move_constructible failure");
	static_assert((is_nothrow_move_constructible<const int*>::value            == true),     "is_nothrow_move_constructible failure");
	static_assert((is_nothrow_move_constructible<int&>::value                  == true),     "is_nothrow_move_constructible failure");
	static_assert((is_nothrow_move_constructible<double>::value                == true),     "is_nothrow_move_constructible failure");
	static_assert((is_nothrow_move_constructible<ClassEmpty>::value            == true),     "is_nothrow_move_constructible failure");
#endif


	// is_copy_constructible
	static_assert((is_copy_constructible<void>::value                                       == false),  "is_copy_constructible failure");
	#if EASTL_TYPE_TRAIT_is_copy_constructible_CONFORMANCE
		static_assert((is_copy_constructible<int>::value                                    == true),   "is_copy_constructible failure");
		static_assert((is_copy_constructible<char*>::value                                  == true),   "is_copy_constructible failure");
		static_assert((is_copy_constructible<int&>::value                                   == true),   "is_copy_constructible failure"); // As of this writing, GCC's libstdc++ reports true for this. I'm trying to find what's correct.
		static_assert((is_copy_constructible<const int>::value                              == true),   "is_copy_constructible failure");
		static_assert((is_copy_constructible<HasTrivialCopy>::value                         == true),   "is_copy_constructible failure");

		#if !defined(EA_COMPILER_EDG) && !defined(EA_COMPILER_MSVC) // EDG (and only EDG) is generating warnings about the decltype expression referencing a deleted constructor. This seems like a bug, though we need to verify.
																	// EA_COMPILER_MSVC is disabled because VS2013 fails this test and it may be that EASTL_TYPE_TRAIT_is_copy_constructible_CONFORMANCE should really be 0 for VS2013.
			static_assert((is_copy_constructible<ConstructibleOnlyWithNonConstReference>::value == false),  "is_copy_constructible failure");
		#endif
	#endif


	// is_destructible
	static_assert(is_destructible<int>::value              == true,  "is_destructible failure");
	static_assert(is_destructible<int&>::value             == true,  "is_destructible failure");
	static_assert(is_destructible<int&&>::value            == true,  "is_destructible failure");
	static_assert(is_destructible<char>::value             == true,  "is_destructible failure");
	static_assert(is_destructible<char*>::value            == true,  "is_destructible failure");
	static_assert(is_destructible<PodA>::value             == true,  "is_destructible failure");
	static_assert(is_destructible<void>::value             == false, "is_destructible failure");
	static_assert(is_destructible<int[3]>::value           == true,  "is_destructible failure");
	static_assert(is_destructible<int[]>::value            == false, "is_destructible failure"); // You can't call operator delete on this class.
	static_assert(is_destructible<Abstract>::value         == true, "is_destructible failure");
	static_assert(is_destructible<AbstractWithDtor>::value == true, "is_destructible failure");
	#if !defined(EA_COMPILER_NO_DELETED_FUNCTIONS)
		static_assert(is_destructible<DeletedDtor>::value  == false, "is_destructible failure"); // You can't call operator delete on this class.
	#endif
	static_assert(is_destructible<NonPod2>::value          == true,  "is_destructible failure");


	// is_trivially_destructible
	static_assert(is_trivially_destructible<int>::value                  == true,  "is_trivially_destructible failure");
	static_assert(is_trivially_destructible<int&>::value                 == true,  "is_trivially_destructible failure");
	static_assert(is_trivially_destructible<int&&>::value                == true,  "is_trivially_destructible failure");
	static_assert(is_trivially_destructible<char>::value                 == true,  "is_trivially_destructible failure");
	static_assert(is_trivially_destructible<char*>::value                == true,  "is_trivially_destructible failure");
	static_assert(is_trivially_destructible<void>::value                 == false, "is_trivially_destructible failure");
	#if EASTL_TYPE_TRAIT_is_trivially_destructible_CONFORMANCE
		static_assert(is_trivially_destructible<PodA>::value             == true,  "is_trivially_destructible failure");
		static_assert(is_trivially_destructible<int[3]>::value           == true,  "is_trivially_destructible failure");
		static_assert(is_trivially_destructible<int[]>::value            == false, "is_trivially_destructible failure");
		static_assert(is_trivially_destructible<Abstract>::value         == true, "is_trivially_destructible failure");
		static_assert(is_trivially_destructible<AbstractWithDtor>::value == false, "is_trivially_destructible failure"); // Having a user-defined destructor make it non-trivial.
	#if !defined(EA_COMPILER_NO_DELETED_FUNCTIONS)
		static_assert(is_trivially_destructible<DeletedDtor>::value      == false, "is_trivially_destructible failure");
	#endif
		static_assert(is_trivially_destructible<NonPod2>::value          == false, "is_trivially_destructible failure");    // This case differs from is_destructible, because we have a declared destructor.
	#endif


	// is_nothrow_destructible
	static_assert(is_nothrow_destructible<int>::value                      == true,  "is_nothrow_destructible failure");
	static_assert(is_nothrow_destructible<int&>::value                     == true,  "is_nothrow_destructible failure");
	static_assert(is_nothrow_destructible<int&&>::value                    == true,  "is_nothrow_destructible failure");
	static_assert(is_nothrow_destructible<void>::value                     == false, "is_nothrow_destructible failure");
	static_assert(is_nothrow_destructible<Abstract>::value         	       == true, "is_nothrow_destructible failure");
	static_assert(is_nothrow_destructible<AbstractWithDtor>::value         == true, "is_nothrow_destructible failure");
	#if !defined(EA_COMPILER_NO_DELETED_FUNCTIONS)
		static_assert(is_nothrow_destructible<DeletedDtor>::value          == false, "is_nothrow_destructible failure"); // You can't call operator delete on this class.
	#endif
	#if EASTL_TYPE_TRAIT_is_nothrow_destructible_CONFORMANCE
		static_assert(is_nothrow_destructible<NonPod2>::value              == true,  "is_nothrow_destructible failure"); // NonPod2 is nothrow destructible because it has an empty destructor (makes no calls) which has no exception specification. Thus its exception specification defaults to noexcept(true) [C++11 Standard, 15.4 paragraph 14]
		static_assert(is_nothrow_destructible<NoThrowDestructible>::value  == true,  "is_nothrow_destructible failure");
	#endif
	#if EASTL_TYPE_TRAIT_is_nothrow_destructible_CONFORMANCE && !defined(EA_COMPILER_NO_EXCEPTIONS)
		static_assert(is_nothrow_destructible<ThrowDestructible>::value               == false, "is_nothrow_destructible failure");
		static_assert(is_nothrow_destructible<ThrowDestructibleNoexceptFalse>::value  == false, "is_nothrow_destructible failure");
	#endif


	// alignment_of
	#if !defined(EA_ABI_ARM_APPLE) // Apple on ARM (i.e. iPhone/iPad) doesn't align 8 byte types on 8 byte boundaries, and the hardware allows it.
		static_assert(alignment_of<uint64_t>::value == 8, "alignment_of failure");
		EATEST_VERIFY(GetType(alignment_of<uint64_t>()) == 8);
	#endif

	static_assert(alignment_of<ClassAlign32>::value == 32, "alignment_of failure");
	EATEST_VERIFY(GetType(alignment_of<ClassAlign32>()) == 32);


	// common_type
	static_assert((is_same<common_type<NonPod2*>::type, NonPod2*>::value), "common_type failure");
	static_assert((is_same<common_type<int>::type, int>::value), "common_type failure");
	static_assert((is_same<common_type<void, void>::type, void>::value), "common_type failure");
	static_assert((is_same<common_type<int, int>::type, int>::value), "common_type failure");


	// rank
	static_assert(rank<int[1][2][3][4][5][6]>::value == 6, "rank failure");
	static_assert(rank<int[][1][2]>::value == 3,           "rank failure");
	static_assert(rank<int>::value == 0,                   "rank failure");
	static_assert(rank<void>::value == 0,                  "rank failure");

	static_assert(rank_v<int[1][2][3][4][5][6]> == 6,      "rank failure");
	static_assert(rank_v<int[][1][2]> == 3,                "rank failure");
	static_assert(rank_v<int> == 0,                        "rank failure");
	static_assert(rank_v<void> == 0,                       "rank failure");



	// extent
	static_assert((extent<int>         ::value == 0), "extent failure");
	static_assert((extent<int[2]>      ::value == 2), "extent failure");
	static_assert((extent<int[2][4]>   ::value == 2), "extent failure");
	static_assert((extent<int[]>       ::value == 0), "extent failure");
	static_assert((extent<int[][4]>    ::value == 0), "extent failure");
	static_assert((extent<int, 1>      ::value == 0), "extent failure");
	static_assert((extent<int[2], 1>   ::value == 0), "extent failure");
	static_assert((extent<int[2][4], 1>::value == 4), "extent failure");
	static_assert((extent<int[][4], 1> ::value == 4), "extent failure");

	static_assert((extent_v<int>          == 0),      "extent failure");
	static_assert((extent_v<int[2]>       == 2),      "extent failure");
	static_assert((extent_v<int[2][4]>    == 2),      "extent failure");
	static_assert((extent_v<int[]>        == 0),      "extent failure");
	static_assert((extent_v<int[][4]>     == 0),      "extent failure");
	static_assert((extent_v<int, 1>       == 0),      "extent failure");
	static_assert((extent_v<int[2], 1>    == 0),      "extent failure");
	static_assert((extent_v<int[2][4], 1> == 4),      "extent failure");
	static_assert((extent_v<int[][4], 1>  == 4),      "extent failure");



	// is_aligned
	static_assert(is_aligned<uint8_t>::value == false,  "is_aligned failure");
	EATEST_VERIFY(GetType(is_aligned<uint8_t>()) == false);

	static_assert(is_aligned<uint16_t>::value == false,  "is_aligned failure");
	EATEST_VERIFY(GetType(is_aligned<uint16_t>()) == false);

	static_assert(is_aligned<uint32_t>::value == false,  "is_aligned failure");
	EATEST_VERIFY(GetType(is_aligned<uint32_t>()) == false);

	static_assert(is_aligned<uint64_t>::value == false,  "is_aligned failure");
	EATEST_VERIFY(GetType(is_aligned<uint64_t>()) == false);

	static_assert(is_aligned<uint64_t>::value == false,  "is_aligned failure");
	EATEST_VERIFY(GetType(is_aligned<uint64_t>()) == false);

	{
		#if (kEASTLTestAlign16 == 16) // To do: Rename kEASTLTestAlign16, as what it really means is "is 16 byte alignment+ supported".
			static_assert(is_aligned<Align16>::value,  "is_aligned failure");
			EATEST_VERIFY(GetType(is_aligned<Align16>()));


			static_assert(is_aligned<Align32>::value,  "is_aligned failure");
			EATEST_VERIFY(GetType(is_aligned<Align32>()));

			static_assert(is_aligned<Align64>::value,  "is_aligned failure");
			EATEST_VERIFY(GetType(is_aligned<Align64>()));
		#endif
	}


	// is_same
	static_assert((is_same<uint32_t, uint32_t>::value  == true),  "is_same failure");
	static_assert((is_same<void, void>::value          == true),  "is_same failure");
	static_assert((is_same<void*, void*>::value        == true),  "is_same failure");
	static_assert((is_same<uint64_t, uint64_t>::value  == true),  "is_same failure");
	static_assert((is_same<Class, Class>::value        == true),  "is_same failure");
	static_assert((is_same<uint64_t, uint32_t>::value  == false), "is_same failure");
	static_assert((is_same<Class, ClassAlign32>::value == false), "is_same failure");

	static_assert((is_same_v<uint32_t, uint32_t>  == true),       "is_same_v failure");
	static_assert((is_same_v<void, void>          == true),       "is_same_v failure");
	static_assert((is_same_v<void*, void*>        == true),       "is_same_v failure");
	static_assert((is_same_v<uint64_t, uint64_t>  == true),       "is_same_v failure");
	static_assert((is_same_v<Class, Class>        == true),       "is_same_v failure");
	static_assert((is_same_v<uint64_t, uint32_t>  == false),      "is_same_v failure");
	static_assert((is_same_v<Class, ClassAlign32> == false),      "is_same_v failure");



	// is_convertible
	static_assert((is_convertible<uint16_t,  uint32_t>::value     == true),     "is_convertible failure");
	static_assert((is_convertible<int32_t,   int16_t>::value      == true),     "is_convertible failure");  // This is a conversion from 32 bits down to 16 bits. All compilers natively report that this is true. However, VC++ generates warnings for actual such conversions.
	static_assert((is_convertible<Subclass,  Class>::value        == true),     "is_convertible failure");
	static_assert((is_convertible<Subclass*, Class*>::value       == true),     "is_convertible failure");
	static_assert((is_convertible<Subclass&, const Class&>::value == true),     "is_convertible failure");
	static_assert((is_convertible<int,       Class>::value        == false),    "is_convertible failure");
	static_assert((is_convertible<NonPod1,   NonPod1>::value      == true),     "is_convertible failure");
	static_assert((is_convertible<NonPod1,   NonPod2>::value      == false),    "is_convertible failure");
	#if EASTL_TYPE_TRAIT_is_convertible_CONFORMANCE // This causes compile failures.
	static_assert((is_convertible<IsConvertibleTest1, IsConvertibleTest1>::value == false),    "is_convertible failure");
	#endif

	// Test EASTL_DECLARE_TRIVIAL_ASSIGN(HiddenAssign);
	eastl::vector<HiddenAssign> v;
	EATEST_VERIFY(v.empty());


	// make_signed
	// make_unsigned
	{
		// Test declarations
		eastl::make_signed<int8_t>::type i8 = -1;
		EATEST_VERIFY(i8 == -1);
		eastl::make_unsigned<uint8_t>::type u8 = 0xff;
		EATEST_VERIFY(u8 == 0xff);

		eastl::make_signed<int16_t>::type i16 = -1;
		EATEST_VERIFY(i16 == -1);
		eastl::make_unsigned<uint16_t>::type u16 = 0xffff;
		EATEST_VERIFY(u16 == 0xffff);

		eastl::make_signed<int32_t>::type i32 = -1;
		EATEST_VERIFY(i32 == -1);
		eastl::make_unsigned<uint32_t>::type u32 = 0xffffffff;
		EATEST_VERIFY(u32 == 0xffffffff);

		eastl::make_signed<int64_t>::type i64 = -1;
		EATEST_VERIFY(i64 == -1);
		eastl::make_unsigned<uint64_t>::type u64 = UINT64_C(0xffffffffffffffff);
		EATEST_VERIFY(u64 == UINT64_C(0xffffffffffffffff));

		// Test conversions via static_cast:
		u8 = static_cast<eastl::make_unsigned<uint8_t>::type>(i8);
		EATEST_VERIFY(u8 == 0xff);
		i8 = static_cast<eastl::make_signed<int8_t>::type>(u8);
		EATEST_VERIFY(i8 == -1);

		u16 = static_cast<eastl::make_unsigned<uint16_t>::type>(i16);
		EATEST_VERIFY(u16 == 0xffff);
		i16 = static_cast<eastl::make_signed<int16_t>::type>(u16);
		EATEST_VERIFY(i16 == -1);

		u32 = static_cast<eastl::make_unsigned<uint32_t>::type>(i32);
		EATEST_VERIFY(u32 == 0xffffffff);
		i32 = static_cast<eastl::make_signed<int32_t>::type>(u32);
		EATEST_VERIFY(i32 == -1);

		u64 = static_cast<eastl::make_unsigned<uint64_t>::type>(i64);
		EATEST_VERIFY(u64 == UINT64_C(0xffffffffffffffff));
		i64 = static_cast<eastl::make_signed<int64_t>::type>(u64);
		EATEST_VERIFY(i64 == -1);


		static_assert(eastl::is_same_v<signed char, eastl::make_signed<unsigned char>::type>);
		static_assert(eastl::is_same_v<short, eastl::make_signed<unsigned short>::type>);
		static_assert(eastl::is_same_v<int, eastl::make_signed<unsigned int>::type>);
		static_assert(eastl::is_same_v<long, eastl::make_signed<unsigned long>::type>);
		static_assert(eastl::is_same_v<long long, eastl::make_signed<unsigned long long>::type>);

		static_assert(eastl::is_same_v<const signed char, eastl::make_signed<const unsigned char>::type>);
		static_assert(eastl::is_same_v<const short, eastl::make_signed<const unsigned short>::type>);
		static_assert(eastl::is_same_v<const int, eastl::make_signed<const unsigned int>::type>);
		static_assert(eastl::is_same_v<const long, eastl::make_signed<const unsigned long>::type>);
		static_assert(eastl::is_same_v<const long long, eastl::make_signed<const unsigned long long>::type>);

		static_assert(eastl::is_same_v<volatile signed char, eastl::make_signed<volatile unsigned char>::type>);
		static_assert(eastl::is_same_v<volatile short, eastl::make_signed<volatile unsigned short>::type>);
		static_assert(eastl::is_same_v<volatile int, eastl::make_signed<volatile unsigned int>::type>);
		static_assert(eastl::is_same_v<volatile long, eastl::make_signed<volatile unsigned long>::type>);
		static_assert(eastl::is_same_v<volatile long long, eastl::make_signed<volatile unsigned long long>::type>);

		static_assert(eastl::is_same_v<const volatile signed char, eastl::make_signed<const volatile unsigned char>::type>);
		static_assert(eastl::is_same_v<const volatile short, eastl::make_signed<const volatile unsigned short>::type>);
		static_assert(eastl::is_same_v<const volatile int, eastl::make_signed<const volatile unsigned int>::type>);
		static_assert(eastl::is_same_v<const volatile long, eastl::make_signed<const volatile unsigned long>::type>);
		static_assert(eastl::is_same_v<const volatile long long, eastl::make_signed<const volatile unsigned long long>::type>);

		static_assert(eastl::is_same_v<unsigned char, eastl::make_unsigned<signed char>::type>);
		static_assert(eastl::is_same_v<unsigned short, eastl::make_unsigned<short>::type>);
		static_assert(eastl::is_same_v<unsigned int, eastl::make_unsigned<int>::type>);
		static_assert(eastl::is_same_v<unsigned long, eastl::make_unsigned<long>::type>);
		static_assert(eastl::is_same_v<unsigned long long, eastl::make_unsigned<long long>::type>);

		static_assert(eastl::is_same_v<const unsigned char, eastl::make_unsigned<const signed char>::type>);
		static_assert(eastl::is_same_v<const unsigned short, eastl::make_unsigned<const short>::type>);
		static_assert(eastl::is_same_v<const unsigned int, eastl::make_unsigned<const int>::type>);
		static_assert(eastl::is_same_v<const unsigned long, eastl::make_unsigned<const long>::type>);
		static_assert(eastl::is_same_v<const unsigned long long, eastl::make_unsigned<const long long>::type>);

		static_assert(eastl::is_same_v<volatile unsigned char, eastl::make_unsigned<volatile signed char>::type>);
		static_assert(eastl::is_same_v<volatile unsigned short, eastl::make_unsigned<volatile short>::type>);
		static_assert(eastl::is_same_v<volatile unsigned int, eastl::make_unsigned<volatile int>::type>);
		static_assert(eastl::is_same_v<volatile unsigned long, eastl::make_unsigned<volatile long>::type>);
		static_assert(eastl::is_same_v<volatile unsigned long long, eastl::make_unsigned<volatile long long>::type>);

		static_assert(eastl::is_same_v<const volatile unsigned char, eastl::make_unsigned<const volatile signed char>::type>);
		static_assert(eastl::is_same_v<const volatile unsigned short, eastl::make_unsigned<const volatile short>::type>);
		static_assert(eastl::is_same_v<const volatile unsigned int, eastl::make_unsigned<const volatile int>::type>);
		static_assert(eastl::is_same_v<const volatile unsigned long, eastl::make_unsigned<const volatile long>::type>);
		static_assert(eastl::is_same_v<const volatile unsigned long long, eastl::make_unsigned<const volatile long long>::type>);

		static_assert(eastl::is_same_v<signed char, eastl::make_signed<signed char>::type>);
		static_assert(eastl::is_same_v<short, eastl::make_signed<signed short>::type>);
		static_assert(eastl::is_same_v<int, eastl::make_signed<signed int>::type>);
		static_assert(eastl::is_same_v<long, eastl::make_signed<signed long>::type>);
		static_assert(eastl::is_same_v<long long, eastl::make_signed<signed long long>::type>);

		static_assert(eastl::is_same_v<unsigned char, eastl::make_unsigned<unsigned char>::type>);
		static_assert(eastl::is_same_v<unsigned short, eastl::make_unsigned<unsigned short>::type>);
		static_assert(eastl::is_same_v<unsigned int, eastl::make_unsigned<unsigned int>::type>);
		static_assert(eastl::is_same_v<unsigned long, eastl::make_unsigned<unsigned long>::type>);
		static_assert(eastl::is_same_v<unsigned long long, eastl::make_unsigned<unsigned long long>::type>);

		#if EASTL_GCC_STYLE_INT128_SUPPORTED
			static_assert(eastl::is_same_v<__uint128_t, eastl::make_unsigned<__int128_t>::type>);
			static_assert(eastl::is_same_v<__uint128_t, eastl::make_unsigned<__uint128_t>::type>);

			static_assert(eastl::is_same_v<__int128_t, eastl::make_signed<__int128_t>::type>);
			static_assert(eastl::is_same_v<__int128_t, eastl::make_signed<__uint128_t>::type>);
		#endif

		// Char tests
		static_assert(sizeof(char) == sizeof(eastl::make_signed<char>::type));
		static_assert(sizeof(wchar_t) == sizeof(eastl::make_signed<wchar_t>::type));
		static_assert(sizeof(char8_t) == sizeof(eastl::make_signed<char8_t>::type));
		static_assert(sizeof(char16_t) == sizeof(eastl::make_signed<char16_t>::type));
		static_assert(sizeof(char32_t) == sizeof(eastl::make_signed<char32_t>::type));
		static_assert(sizeof(char) == sizeof(eastl::make_unsigned<char>::type));
		static_assert(sizeof(wchar_t) == sizeof(eastl::make_unsigned<wchar_t>::type));
		static_assert(sizeof(char8_t) == sizeof(eastl::make_unsigned<char8_t>::type));
		static_assert(sizeof(char16_t) == sizeof(eastl::make_unsigned<char16_t>::type));
		static_assert(sizeof(char32_t) == sizeof(eastl::make_unsigned<char32_t>::type));

		static_assert(eastl::is_same_v<signed char, eastl::make_signed<char8_t>::type>);
		static_assert(eastl::is_same_v<unsigned char, eastl::make_unsigned<char8_t>::type>);

		// Enum tests
		enum EnumUCharSize : unsigned char		{};
		enum EnumUShortSize : unsigned short	{};
		enum EnumUIntSize : unsigned int		{};
		enum EnumULongSize : unsigned long {};
		enum EnumULongLongSize : unsigned long long		{};

		static_assert(eastl::is_signed_v<eastl::make_signed<EnumUCharSize>::type>);
		static_assert(eastl::is_signed_v<eastl::make_signed<EnumUShortSize>::type>);
		static_assert(eastl::is_signed_v<eastl::make_signed<EnumUIntSize>::type>);
		static_assert(eastl::is_signed_v<eastl::make_signed<EnumULongSize>::type>);
		static_assert(eastl::is_signed_v<eastl::make_signed<EnumULongLongSize>::type>);
		static_assert(sizeof(EnumUCharSize) == sizeof(eastl::make_signed<EnumUCharSize>::type));
		static_assert(sizeof(EnumUShortSize) == sizeof(eastl::make_signed<EnumUShortSize>::type));
		static_assert(sizeof(EnumUIntSize) == sizeof(eastl::make_signed<EnumUIntSize>::type));
		static_assert(sizeof(EnumULongSize) == sizeof(eastl::make_signed<EnumULongSize>::type));
		static_assert(sizeof(EnumULongLongSize) == sizeof(eastl::make_signed<EnumULongLongSize>::type));

		enum EnumCharSize : signed char	{};
		enum EnumShortSize : short		{};
		enum EnumIntSize : int			{};
		enum EnumLongSize : long			{};
		enum EnumLongLongSize : long long	{};

		static_assert(eastl::is_unsigned_v<eastl::make_unsigned<EnumCharSize>::type>);
		static_assert(eastl::is_unsigned_v<eastl::make_unsigned<EnumShortSize>::type>);
		static_assert(eastl::is_unsigned_v<eastl::make_unsigned<EnumIntSize>::type>);
		static_assert(eastl::is_unsigned_v<eastl::make_unsigned<EnumLongSize>::type>);
		static_assert(eastl::is_unsigned_v<eastl::make_unsigned<EnumLongLongSize>::type>);
		static_assert(sizeof(EnumCharSize) == sizeof(eastl::make_unsigned<EnumCharSize>::type));
		static_assert(sizeof(EnumShortSize) == sizeof(eastl::make_unsigned<EnumShortSize>::type));
		static_assert(sizeof(EnumIntSize) == sizeof(eastl::make_unsigned<EnumIntSize>::type));
		static_assert(sizeof(EnumLongSize) == sizeof(eastl::make_unsigned<EnumLongSize>::type));
		static_assert(sizeof(EnumLongLongSize) == sizeof(eastl::make_unsigned<EnumLongLongSize>::type));
	}

	// remove_const
	// remove_volatile
	// remove_cv
	{
		// To do: Make more thorough tests verifying this. Such tests will probably involve template metaprogramming.
		remove_const<const int32_t>::type i32 = 47;
		EATEST_VERIFY(++i32 == 48);

		remove_volatile<volatile int16_t>::type i16 = 47;
		EATEST_VERIFY(++i16 == 48);

		remove_cv<const volatile int32_t>::type i64 = 47;
		EATEST_VERIFY(++i64 == 48);

		//static_assert(is_same<std::remove_cv<int (int, ...)>::type , std::remove_cv<int (int, ...) const>::type>::value, "remove_cv failure");
	}

	// remove_cvref
	{
		static_assert(is_same_v<remove_cvref_t<int>, int>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<int&>, int>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<int&&>, int>, "remove_cvref failure");

		static_assert(is_same_v<remove_cvref_t<const int>, int>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<const int&>, int>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<const int&&>, int>, "remove_cvref failure");

		static_assert(is_same_v<remove_cvref_t<volatile int>, int>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<volatile int&>, int>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<volatile int&&>, int>, "remove_cvref failure");

		static_assert(is_same_v<remove_cvref_t<const volatile int>, int>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<const volatile int&>, int>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<const volatile int&&>, int>, "remove_cvref failure");

		// test pointer types
		static_assert(is_same_v<remove_cvref_t<int*>, int*>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<int*&>, int*>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<int*&&>, int*>, "remove_cvref failure");

		static_assert(is_same_v<remove_cvref_t<const int*>, const int*>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<const int*&>, const int*>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<const int*&&>, const int*>, "remove_cvref failure");

		static_assert(is_same_v<remove_cvref_t<int* const>, int*>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<int* const&>, int*>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<int* const&&>, int*>, "remove_cvref failure");

		static_assert(is_same_v<remove_cvref_t<int* const volatile>, int*>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<int* const volatile&>, int*>, "remove_cvref failure");
		static_assert(is_same_v<remove_cvref_t<int* const volatile&&>, int*>, "remove_cvref failure");
	}


	// add_const
	// add_volatile
	// add_cv
	{
		// To do: Make more thorough tests verifying this. Such tests will probably involve template metaprogramming.
		eastl::add_const<int32_t>::type i32 = 47;
		EATEST_VERIFY(i32 == 47);

		eastl::add_volatile<volatile int16_t>::type i16 = 47;
		EATEST_VERIFY(++i16 == 48);

		eastl::add_cv<const volatile int32_t>::type i64 = 47;
		EATEST_VERIFY(i64 == 47);
	}


	// as_const
	{
		{
			int i = 42;	
			static_assert(eastl::is_same<decltype(eastl::as_const(i)), const int&>::value, "expecting a 'const T&' return type");
			EATEST_VERIFY(eastl::as_const(i) == 42);
		}

		{
			eastl::string str = "Electronic Arts";
			static_assert(eastl::is_same<decltype(eastl::as_const(str)), const eastl::string&>::value, "expecting a 'const T&' return type");
			EATEST_VERIFY(eastl::as_const(str) == "Electronic Arts");
		}
	}


	// remove_reference
	// add_reference
	// remove_pointer
	// add_pointer
	// remove_extent
	// remove_all_extents
	{
		int x = 17;
		eastl::add_reference<int>::type xRef = x;
		x++;
		EATEST_VERIFY(xRef == 18);

		eastl::remove_reference<int&>::type xValue;
		xValue = 3;
		EATEST_VERIFY(xValue == 3);

		eastl::add_pointer<int>::type xPtr = &x;
		*xPtr = 19;
		EATEST_VERIFY(x == 19);

		eastl::remove_pointer<int*>::type yValue;
		yValue = 3;
		EATEST_VERIFY(yValue == 3);

		// ref to T
		//   -> T*
		static_assert(is_same_v<add_pointer_t<int&>, int*>, "add_pointer failure");
		static_assert(is_same_v<add_pointer_t<int(&)()>, int(*)()>, "add_pointer failure");

		// object type (a (possibly cv-qualified) type other than function type, reference type or void), or
		// a function type that is not cv- or ref-qualified, or a (possibly cv-qualified) void type
		//   -> T*
		static_assert(is_same_v<add_pointer_t<int>, int*>, "add_pointer failure");
		static_assert(is_same_v<add_pointer_t<int*>, int**>, "add_pointer failure");
		static_assert(is_same_v<add_pointer_t<int()>, int(*)()>, "add_pointer failure");
		static_assert(is_same_v<add_pointer_t<void>, void*>, "add_pointer failure");
		static_assert(is_same_v<add_pointer_t<const void>, const void*>, "add_pointer failure");
		static_assert(is_same_v<add_pointer_t<volatile void>, volatile void*>, "add_pointer failure");
		static_assert(is_same_v<add_pointer_t<const volatile void>, const volatile void*>, "add_pointer failure");

		// otherwise (cv- or ref-qualified function type)
		//   -> T
		static_assert(is_same_v<add_pointer_t<int() const>, int() const>, "add_pointer failure");
		static_assert(is_same_v<add_pointer_t<int() volatile>, int() volatile>, "add_pointer failure");
		static_assert(is_same_v<add_pointer_t<int() const volatile>, int() const volatile>, "add_pointer failure");

		// remove_extent
		// If T is an array of some type X, provides the member typedef type equal to X, otherwise 
		// type is T. Note that if T is a multidimensional array, only the first dimension is removed. 
		typedef int IntArray1[37];
		typedef eastl::remove_extent<IntArray1>::type Int;
		static_assert((eastl::is_same<Int, int>::value == true), "remove_extent/is_same failure");

		// remove_all_extents
		typedef int IntArray2[37][54];
		typedef eastl::remove_all_extents<IntArray2>::type Int2;
		static_assert((eastl::is_same<Int2, int>::value == true), "remove_all_extents/is_same failure");
	}

	// add_lvalue_reference
	{
		// function type with no cv- or ref-qualifier
		//   -> T&
		static_assert(is_same_v<add_lvalue_reference_t<void()>, void(&)()>, "add_lvalue_reference failure");

		// object type (a (possibly cv-qualified) type other than function type, reference type or void)
		//   -> T&
		static_assert(is_same_v<add_lvalue_reference_t<int>, int&>, "add_lvalue_reference failure");
		static_assert(is_same_v<add_lvalue_reference_t<const int>, const int&>, "add_lvalue_reference failure");

		// if T is an rvalue reference (to some type U)
		//   -> U&
		static_assert(is_same_v<add_lvalue_reference_t<int&&>, int&>, "add_lvalue_reference failure");

		// otherwise (cv- or ref-qualified function type, or reference type, or (possibly cv-qualified) void)
		//   -> T
		static_assert(is_same_v<add_lvalue_reference_t<void() const>, void() const>, "add_lvalue_reference failure");
		static_assert(is_same_v<add_lvalue_reference_t<void()&>, void()&>, "add_lvalue_reference failure");
		static_assert(is_same_v<add_lvalue_reference_t<void()&&>, void()&&>, "add_lvalue_reference failure");
		static_assert(is_same_v<add_lvalue_reference_t<int&>, int&>, "add_lvalue_reference failure");
		static_assert(is_same_v<add_lvalue_reference_t<const int&>, const int&>, "add_lvalue_reference failure");
		static_assert(is_same_v<add_lvalue_reference_t<void>, void>, "add_lvalue_reference failure");
		static_assert(is_same_v<add_lvalue_reference_t<const void>, const void>, "add_lvalue_reference failure");
	}

	// add_rvalue_reference
	{
		// function type with no cv- or ref-qualifier
		//   -> T&&
		static_assert(is_same_v<add_rvalue_reference_t<void()>, void(&&)()>, "add_rvalue_reference failure");

		// object type (a (possibly cv-qualified) type other than function type, reference type or void)
		//   -> T&&
		static_assert(is_same_v<add_rvalue_reference_t<int>, int&&>, "add_rvalue_reference failure");
		static_assert(is_same_v<add_rvalue_reference_t<const int>, const int&&>, "add_rvalue_reference failure");

		// otherwise (cv- or ref-qualified function type, or reference type, or (possibly cv-qualified) void)
		//   -> T
		static_assert(is_same_v<add_rvalue_reference_t<void() const>, void() const>, "add_rvalue_reference failure");
		static_assert(is_same_v<add_rvalue_reference_t<void()&>, void()&>, "add_rvalue_reference failure");
		static_assert(is_same_v<add_rvalue_reference_t<void()&&>, void()&&>, "add_rvalue_reference failure");
		static_assert(is_same_v<add_rvalue_reference_t<int&>, int&>, "add_rvalue_reference failure");
		static_assert(is_same_v<add_rvalue_reference_t<int&&>, int&&>, "add_rvalue_reference failure");
		static_assert(is_same_v<add_rvalue_reference_t<const int&>, const int&>, "add_rvalue_reference failure");
		static_assert(is_same_v<add_rvalue_reference_t<void>, void>, "add_rvalue_reference failure");
		static_assert(is_same_v<add_rvalue_reference_t<const void>, const void>, "add_rvalue_reference failure");
	}


	// decay
	{ 
		static_assert((eastl::is_same<uint32_t,          eastl::decay<uint32_t>::type>::value                    == true), "is_same failure");
		static_assert((eastl::is_same<uint32_t,          eastl::decay<const uint32_t>::type>::value              == true), "is_same failure");
		static_assert((eastl::is_same<uint32_t,          eastl::decay<volatile uint32_t>::type>::value           == true), "is_same failure");
		static_assert((eastl::is_same<uint32_t,          eastl::decay<uint32_t&>::type>::value                   == true), "is_same failure");
		static_assert((eastl::is_same<uint32_t,          eastl::decay<const uint32_t&>::type>::value             == true), "is_same failure");
		static_assert((eastl::is_same<uint32_t,          eastl::decay<const volatile uint32_t&>::type>::value    == true), "is_same failure");
	  #if !EASTL_NO_RVALUE_REFERENCES
		static_assert((eastl::is_same<uint32_t,          eastl::decay<uint32_t&&>::type>::value                  == true), "is_same failure");
	  #endif
		static_assert((eastl::is_same<uint32_t*,         eastl::decay<uint32_t[3]>::type>::value                 == true), "is_same failure");
		static_assert((eastl::is_same<uint32_t(*)(char), eastl::decay<uint32_t(char)>::type>::value              == true), "is_same failure");
	}


	// aligned_storage
	// Some compilers don't support or ignore alignment specifications for stack variables, 
	// so we limit our testing to compilers that are known to support it.
	#if (EA_ALIGN_MAX_AUTOMATIC >= 64) && defined(EA_PLATFORM_DESKTOP) // Actually there are additional compilers that support alignment of stack-based variables, most significantly clang, GCC 4.4+, and probably others.
	{
		// Test the creation of a single aligned value.
		const size_t     kArraySize = 100;
		const size_t     kExpectedAlignment = 64;
		typedef uint16_t Type;

		eastl::aligned_storage<sizeof(Type), kExpectedAlignment>::type data;
		Type* value = new(&data) Type;
		*value = 37;
		EATEST_VERIFY_F((EA::StdC::GetAlignment(value) >= kExpectedAlignment) && (*value == 37), 
						 "eastl::aligned_storage failure: Expected: %u, Actual: %u", (unsigned)kExpectedAlignment, (unsigned)EA::StdC::GetAlignment(value));

		// Create an array of 100 values aligned.
		eastl::aligned_storage<sizeof(Type), kExpectedAlignment>::type dataArray[kArraySize];
		Type* valueArray = new(dataArray) Type[kArraySize];
		valueArray[0] = 37;
		EATEST_VERIFY_F((EA::StdC::GetAlignment(valueArray) >= kExpectedAlignment) && (valueArray[0] == 37), 
						 "eastl::aligned_storage failure: Expected: %u, Actual: %u", (unsigned)kExpectedAlignment, (unsigned)EA::StdC::GetAlignment(valueArray));
	}
	{
		// Test the creation of a single aligned value.
		const size_t     kArraySize = 17;
		const size_t     kExpectedAlignment = 128;
		typedef uint8_t  Type;

		eastl::aligned_storage<sizeof(Type), kExpectedAlignment>::type data;
		Type* value = new(&data) Type;
		*value = 37;
		EATEST_VERIFY_F((EA::StdC::GetAlignment(value) >= kExpectedAlignment) && (*value == 37), 
						 "eastl::aligned_storage failure: Expected: %u, Actual: %u", (unsigned)kExpectedAlignment, (unsigned)EA::StdC::GetAlignment(value));

		// Create an array of 100 values aligned.
		eastl::aligned_storage<sizeof(Type), kExpectedAlignment>::type dataArray[kArraySize];
		Type* valueArray = new(dataArray) Type[kArraySize];
		valueArray[0] = 37;
		EATEST_VERIFY_F((EA::StdC::GetAlignment(valueArray) >= kExpectedAlignment) && (valueArray[0] == 37), 
						 "eastl::aligned_storage failure: Expected: %u, Actual: %u", (unsigned)kExpectedAlignment, (unsigned)EA::StdC::GetAlignment(valueArray));
	}
	{
		// Test the creation of a single aligned value.
		const size_t     kArraySize = 27;
		const size_t     kExpectedAlignment = 256;
		typedef uint32_t Type;

		eastl::aligned_storage<sizeof(Type), kExpectedAlignment>::type data;
		Type* value = new(&data) Type;
		*value = 37;
		EATEST_VERIFY_F((EA::StdC::GetAlignment(value) >= kExpectedAlignment) && (*value == 37), 
						 "eastl::aligned_storage failure: Expected: %u, Actual: %u", (unsigned)kExpectedAlignment, (unsigned)EA::StdC::GetAlignment(value));

		// Create an array of 100 values aligned.
		eastl::aligned_storage<sizeof(Type), kExpectedAlignment>::type dataArray[kArraySize];
		Type* valueArray = new(dataArray) Type[kArraySize];
		valueArray[0] = 37;
		EATEST_VERIFY_F((EA::StdC::GetAlignment(valueArray) >= kExpectedAlignment) && (valueArray[0] == 37), 
						 "eastl::aligned_storage failure: Expected: %u, Actual: %u", (unsigned)kExpectedAlignment, (unsigned)EA::StdC::GetAlignment(valueArray));
	}
	#endif


	// aligned_union
	// Some compilers don't support or ignore alignment specifications for stack variables, 
	// so we limit our testing to compilers that are known to support it.
	{
		union AlignedUnion
		{
			char  c;
			int   i;
			float f;
			char  a[32];
	
			AlignedUnion(float fValue) : f(fValue) {}
		};
	
		typedef aligned_union<sizeof(AlignedUnion), char, int, float>::type AlignedUnionStorage;

		static_assert((EA_ALIGN_OF(AlignedUnionStorage) >= EA_ALIGN_OF(float)) && (EA_ALIGN_OF(AlignedUnionStorage) <= EA_ALIGN_OF(double)), "aligned_union failure");
		static_assert(sizeof(AlignedUnionStorage) >= sizeof(AlignedUnion), "aligned_union failure");

		AlignedUnionStorage alignedUnionStorage; // Since we know that our alignment is a simple value <= default alignment, we can just declare an object here and it will work with all compilers, including those that are limited in the stack alignments they support.
		AlignedUnion*       pAlignedUnion = new (&alignedUnionStorage) AlignedUnion(21.4f);
		EATEST_VERIFY(pAlignedUnion->f == 21.4f);
		pAlignedUnion->i = 37;
		EATEST_VERIFY(pAlignedUnion->i == 37);
	}


	// union_cast
	{
		float    f32    = -1234.f;
		uint32_t n32    = union_cast<uint32_t>(f32);
		float    f32New = union_cast<float>(n32);
		EATEST_VERIFY(f32 == f32New);

		double   f64    = -1234.0;
		uint64_t n64    = union_cast<uint64_t>(f64);
		double   f64New = union_cast<double>(n64);
		EATEST_VERIFY(f64 == f64New);

		PodA a    = { -1234 };
		PodB b    = union_cast<PodB>(a);
		PodA aNew = union_cast<PodA>(b);
		EATEST_VERIFY(a == aNew);

		PodA*  pA    = new PodA;
		PodB*  pB    = union_cast<PodB*>(pA);
		PodA*  pANew = union_cast<PodA*>(pB);
		EATEST_VERIFY(pA == pANew);
		delete pA;
	}

	// void_t
	{
		{
			static_assert(is_same<void_t<void>, void>::value, "void_t failure");
			static_assert(is_same<void_t<int>, void>::value, "void_t failure");
			static_assert(is_same<void_t<short>, void>::value, "void_t failure");
			static_assert(is_same<void_t<long>, void>::value, "void_t failure");
			static_assert(is_same<void_t<long long>, void>::value, "void_t failure");
			static_assert(is_same<void_t<ClassEmpty>, void>::value, "void_t failure");
			static_assert(is_same<void_t<ClassNonEmpty>, void>::value, "void_t failure");
			static_assert(is_same<void_t<vector<int>>, void>::value, "void_t failure");
		}

		// new sfinae mechansim test 
		{
			static_assert(has_increment_operator_using_void_t<HasIncrementOperator>::value, "void_t sfinae failure");
			static_assert(!has_increment_operator_using_void_t<ClassEmpty>::value, "void_t sfinae failure");
		}
	}

	// detected idiom
	{
		static_assert(is_detected<has_increment_operator_detection, HasIncrementOperator>::value, "is_detected failure.");
		static_assert(!is_detected<has_increment_operator_detection, ClassEmpty>::value, "is_detected failure.");

		static_assert(is_same<detected_t<has_increment_operator_detection, HasIncrementOperator>, HasIncrementOperator&>::value, "is_detected_t failure.");
		static_assert(is_same<detected_t<has_increment_operator_detection, ClassEmpty>, nonesuch>::value, "is_detected_t failure.");

		using detected_or_positive_result = detected_or<float, has_increment_operator_detection, HasIncrementOperator>;
		using detected_or_negative_result = detected_or<float, has_increment_operator_detection, ClassEmpty>;
		static_assert(detected_or_positive_result::value_t::value, "detected_or failure.");
		static_assert(!detected_or_negative_result::value_t::value, "detected_or failure.");
		static_assert(is_same<detected_or_positive_result::type, HasIncrementOperator&>::value, "detected_or failure.");
		static_assert(is_same<detected_or_negative_result::type, float>::value, "detected_or failure.");

		static_assert(is_same<detected_or_t<float, has_increment_operator_detection, HasIncrementOperator>, HasIncrementOperator&>::value, "detected_or_t failure.");
		static_assert(is_same<detected_or_t<float, has_increment_operator_detection, ClassEmpty>, float>::value, "detected_or_t failure.");

		static_assert(is_detected_exact<HasIncrementOperator&, has_increment_operator_detection, HasIncrementOperator>::value, "is_detected_exact failure.");
		static_assert(!is_detected_exact<float, has_increment_operator_detection, HasIncrementOperator>::value, "is_detected_exact failure.");
		static_assert(is_detected_exact<nonesuch, has_increment_operator_detection, ClassEmpty>::value, "is_detected_exact failure.");
		static_assert(!is_detected_exact<float, has_increment_operator_detection, ClassEmpty>::value, "is_detected_exact failure.");

		static_assert(is_detected_convertible<HasIncrementOperator&, has_increment_operator_detection, HasIncrementOperator>::value, "is_detected_convertible failure.");
		static_assert(is_detected_convertible<HasIncrementOperator, has_increment_operator_detection, HasIncrementOperator>::value, "is_detected_convertible failure.");
		static_assert(!is_detected_convertible<float, has_increment_operator_detection, HasIncrementOperator>::value, "is_detected_convertible failure.");
		static_assert(!is_detected_convertible<nonesuch, has_increment_operator_detection, ClassEmpty>::value, "is_detected_convertible failure.");
		static_assert(!is_detected_convertible<float, has_increment_operator_detection, ClassEmpty>::value, "is_detected_convertible failure.");


	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		static_assert(is_detected_v<has_increment_operator_detection, HasIncrementOperator>, "is_detected_v failure.");
		static_assert(!is_detected_v<has_increment_operator_detection, ClassEmpty>, "is_detected_v failure.");

		static_assert(is_detected_exact_v<HasIncrementOperator&, has_increment_operator_detection, HasIncrementOperator>, "is_detected_exact_v failure.");
		static_assert(!is_detected_exact_v<float, has_increment_operator_detection, HasIncrementOperator>, "is_detected_exact_v failure.");
		static_assert(is_detected_exact_v<nonesuch, has_increment_operator_detection, ClassEmpty>, "is_detected_exact_v failure.");
		static_assert(!is_detected_exact_v<float, has_increment_operator_detection, ClassEmpty>, "is_detected_exact_v failure.");

		static_assert(is_detected_convertible_v<HasIncrementOperator&, has_increment_operator_detection, HasIncrementOperator>, "is_detected_convertible_v failure.");
		static_assert(is_detected_convertible_v<HasIncrementOperator, has_increment_operator_detection, HasIncrementOperator>, "is_detected_convertible_v failure.");
		static_assert(!is_detected_convertible_v<float, has_increment_operator_detection, HasIncrementOperator>, "is_detected_convertible_v failure.");
		static_assert(!is_detected_convertible_v<nonesuch, has_increment_operator_detection, ClassEmpty>, "is_detected_convertible_v failure.");
		static_assert(!is_detected_convertible_v<float, has_increment_operator_detection, ClassEmpty>, "is_detected_convertible_v failure.");
	#endif
	}

	// conjunction
	{
		static_assert( conjunction<>::value, "conjunction failure");
		static_assert(!conjunction<false_type>::value, "conjunction failure");
		static_assert(!conjunction<false_type, false_type>::value, "conjunction failure");
		static_assert(!conjunction<false_type, false_type, false_type>::value, "conjunction failure");
		static_assert(!conjunction<false_type, false_type, false_type, true_type>::value, "conjunction failure");
		static_assert(!conjunction<false_type, false_type, true_type, true_type>::value, "conjunction failure");
		static_assert(!conjunction<false_type, true_type, true_type, true_type>::value, "conjunction failure");
		static_assert(!conjunction<true_type, true_type, true_type, true_type, false_type>::value, "conjunction failure");
		static_assert(!conjunction<true_type, false_type, true_type, true_type, true_type>::value, "conjunction failure");
		static_assert( conjunction<true_type, true_type, true_type, true_type, true_type>::value, "conjunction failure");
		static_assert( conjunction<true_type, true_type, true_type, true_type>::value, "conjunction failure");
		static_assert( conjunction<true_type, true_type, true_type>::value, "conjunction failure");
		static_assert( conjunction<true_type>::value, "conjunction failure");

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		static_assert( conjunction_v<>, "conjunction failure");
		static_assert(!conjunction_v<false_type>, "conjunction failure");
		static_assert(!conjunction_v<false_type, false_type>, "conjunction failure");
		static_assert(!conjunction_v<false_type, false_type, false_type>, "conjunction failure");
		static_assert(!conjunction_v<false_type, false_type, false_type, true_type>, "conjunction failure");
		static_assert(!conjunction_v<false_type, false_type, true_type, true_type>, "conjunction failure");
		static_assert(!conjunction_v<false_type, true_type, true_type, true_type>, "conjunction failure");
		static_assert(!conjunction_v<true_type, true_type, true_type, true_type, false_type>, "conjunction failure");
		static_assert(!conjunction_v<true_type, false_type, true_type, true_type, true_type>, "conjunction failure");
		static_assert( conjunction_v<true_type, true_type, true_type, true_type, true_type>, "conjunction failure");
		static_assert( conjunction_v<true_type, true_type, true_type, true_type>, "conjunction failure");
		static_assert( conjunction_v<true_type, true_type, true_type>, "conjunction failure");
		static_assert( conjunction_v<true_type>, "conjunction failure");
	#endif
	}
	
	// disjunction
	{
		static_assert(!disjunction<>::value, "disjunction failure");
		static_assert(!disjunction<false_type>::value, "disjunction failure");
		static_assert(!disjunction<false_type, false_type>::value, "disjunction failure");
		static_assert(!disjunction<false_type, false_type, false_type>::value, "disjunction failure");
		static_assert( disjunction<false_type, false_type, false_type, true_type>::value, "disjunction failure");
		static_assert( disjunction<false_type, false_type, true_type, true_type>::value, "disjunction failure");
		static_assert( disjunction<false_type, true_type, true_type, true_type>::value, "disjunction failure");
		static_assert( disjunction<true_type, true_type, true_type, true_type, false_type>::value, "disjunction failure");
		static_assert( disjunction<true_type, false_type, true_type, true_type, true_type>::value, "disjunction failure");
		static_assert( disjunction<true_type, true_type, true_type, true_type, true_type>::value, "disjunction failure");
		static_assert( disjunction<true_type, true_type, true_type, true_type>::value, "disjunction failure");
		static_assert( disjunction<true_type, true_type, true_type>::value, "disjunction failure");
		static_assert( disjunction<true_type>::value, "disjunction failure");

	#if EASTL_VARIABLE_TEMPLATES_ENABLED
		static_assert(!disjunction_v<>, "disjunction failure");
		static_assert(!disjunction_v<false_type>, "disjunction failure");
		static_assert(!disjunction_v<false_type, false_type>, "disjunction failure");
		static_assert(!disjunction_v<false_type, false_type, false_type>, "disjunction failure");
		static_assert( disjunction_v<false_type, false_type, false_type, true_type>, "disjunction failure");
		static_assert( disjunction_v<false_type, false_type, true_type, true_type>, "disjunction failure");
		static_assert( disjunction_v<false_type, true_type, true_type, true_type>, "disjunction failure");
		static_assert( disjunction_v<true_type, true_type, true_type, true_type, false_type>, "disjunction failure");
		static_assert( disjunction_v<true_type, false_type, true_type, true_type, true_type>, "disjunction failure");
		static_assert( disjunction_v<true_type, true_type, true_type, true_type, true_type>, "disjunction failure");
		static_assert( disjunction_v<true_type, true_type, true_type, true_type>, "disjunction failure");
		static_assert( disjunction_v<true_type, true_type, true_type>, "disjunction failure");
		static_assert( disjunction_v<true_type>, "disjunction failure");
	#endif
	}

	// negation
	{
		static_assert( negation<false_type>::value, "negation failure");
		static_assert(!negation<true_type>::value, "negation failure");

		#if EASTL_VARIABLE_TEMPLATES_ENABLED
			static_assert( negation_v<false_type>, "negation failure");
			static_assert(!negation_v<true_type>, "negation failure");
		#endif
	}

	// has_unique_object_representations
	{
		static_assert( has_unique_object_representations<bool>::value,               "has_unique_object_representations failure");
		static_assert( has_unique_object_representations<char16_t>::value,           "has_unique_object_representations failure");
		static_assert( has_unique_object_representations<char32_t>::value,           "has_unique_object_representations failure");
		static_assert( has_unique_object_representations<char>::value,               "has_unique_object_representations failure");
		static_assert( has_unique_object_representations<int>::value,                "has_unique_object_representations failure");
		static_assert( has_unique_object_representations<long long>::value,          "has_unique_object_representations failure");
		static_assert( has_unique_object_representations<long>::value,               "has_unique_object_representations failure");
		static_assert( has_unique_object_representations<short>::value,              "has_unique_object_representations failure");
		static_assert( has_unique_object_representations<signed char>::value,        "has_unique_object_representations failure");
		static_assert( has_unique_object_representations<unsigned char>::value,      "has_unique_object_representations failure");
		static_assert( has_unique_object_representations<unsigned int>::value,       "has_unique_object_representations failure");
		static_assert( has_unique_object_representations<unsigned long long>::value, "has_unique_object_representations failure");
		static_assert( has_unique_object_representations<unsigned long>::value,      "has_unique_object_representations failure");
		static_assert( has_unique_object_representations<unsigned short>::value,     "has_unique_object_representations failure");
		static_assert(!has_unique_object_representations<void>::value,               "has_unique_object_representations failure");
#ifndef EA_WCHAR_T_NON_NATIVE // If wchar_t is a native type instead of simply a define to an existing type which is already handled...
		static_assert( has_unique_object_representations<wchar_t>::value,            "has_unique_object_representations failure");
#endif

	#if EASTL_TYPE_TRAIT_has_unique_object_representations_CONFORMANCE
		{
			struct packed_type { int a; };
			static_assert( has_unique_object_representations<packed_type>::value, "has_unique_object_representations failure");

			struct padded_type { int a; char b; int c; };
			static_assert(!has_unique_object_representations<padded_type>::value, "has_unique_object_representations failure");
		}
	#endif
	}
	
	// is_final
	{
	#if (EA_COMPILER_HAS_FEATURE(is_final))
		static_assert(std::is_final<FinalStruct>::value == eastl::is_final<FinalStruct>::value,  "final struct not correctly detected");
		static_assert(std::is_final<FinalClass>::value  == eastl::is_final<FinalClass>::value,   "final class not correctly detected");
		static_assert(std::is_final<Enum>::value        == eastl::is_final<Enum>::value,         "enum not correctly detected");
		static_assert(std::is_final<int>::value         == eastl::is_final<int>::value,          "int not correctly detected");
		static_assert(std::is_final<Struct>::value      == eastl::is_final<Struct>::value,       "non-final struct not correctly detected");
		static_assert(std::is_final<Class>::value       == eastl::is_final<Class>::value,        "non-final class not correctly detected");
    #endif

	// endian (big-endian and little; no mixed-endian/middle-endian)
	static_assert(eastl::endian::big != eastl::endian::little, "little-endian and big-endian are not the same");
	static_assert(eastl::endian::native == eastl::endian::big || eastl::endian::native == eastl::endian::little, "native may be little endian or big endian");
	static_assert(!(eastl::endian::native == eastl::endian::big && eastl::endian::native == eastl::endian::little), "native cannot be both big and little endian");

	#ifdef EA_SYSTEM_LITTLE_ENDIAN
		static_assert(eastl::endian::native == eastl::endian::little,  "must be little endian");
		static_assert(eastl::endian::native != eastl::endian::big,     "must not be big endian");
	#else
		static_assert(eastl::endian::native != eastl::endian::little,  "must not be little endian");
		static_assert(eastl::endian::native == eastl::endian::big,     "must be big endian");
	#endif
	}

	// has_equality
	{
		static_assert( has_equality_v<int>, "has_equality failure");
		static_assert( has_equality_v<short>, "has_equality failure");
		static_assert( has_equality_v<long>, "has_equality failure");
		static_assert( has_equality_v<long long>, "has_equality failure");
		static_assert( has_equality_v<TestObject>, "has_equality failure");
		static_assert(!has_equality_v<MissingEquality>, "has_equality failure");
	}

	// is_aggregate
	#if EASTL_TYPE_TRAIT_is_aggregate_CONFORMANCE
	{
		static_assert(!is_aggregate_v<int>, "is_aggregate failure");
		static_assert( is_aggregate_v<int[]>, "is_aggregate failure");

		{
			struct Aggregrate {};
			static_assert(is_aggregate_v<Aggregrate>, "is_aggregate failure");
		}

		{
			struct NotAggregrate { NotAggregrate() {} }; // user provided ctor
			static_assert(!is_aggregate_v<NotAggregrate>, "is_aggregate failure");
		}

		#if defined(EA_COMPILER_CPP11_ENABLED) && !defined(EA_COMPILER_CPP14_ENABLED)
		// See https://en.cppreference.com/w/cpp/language/aggregate_initialization
		// In C++11 the requirement was added to aggregate types that no default member initializers exist,
		// however this requirement was removed in C++14.
		{
			struct NotAggregrate { int data = 42; }; // default member initializer 
			static_assert(!is_aggregate_v<NotAggregrate>, "is_aggregate failure");
		}
		#endif

		{
			struct NotAggregrate { virtual void foo() {} }; // virtual member function
			static_assert(!is_aggregate_v<NotAggregrate>, "is_aggregate failure");
		}
	}
	#endif

	// is_complete_type
	{
		struct Foo
		{
			int x;
		};

		struct FooEmpty
		{
		};

		struct Bar;

		void FooFunc();

		static_assert(eastl::internal::is_complete_type_v<Foo>, "is_complete_type failure");
		static_assert(eastl::internal::is_complete_type_v<FooEmpty>, "is_complete_type failure");
		static_assert(!eastl::internal::is_complete_type_v<Bar>, "is_complete_type failure");
		static_assert(!eastl::internal::is_complete_type_v<void>, "is_complete_type failure");
		static_assert(!eastl::internal::is_complete_type_v<volatile void>, "is_complete_type failure");
		static_assert(!eastl::internal::is_complete_type_v<const void>, "is_complete_type failure");
		static_assert(!eastl::internal::is_complete_type_v<const volatile void>, "is_complete_type failure");
		static_assert(eastl::internal::is_complete_type_v<decltype(FooFunc)>, "is_complete_type failure");
	}


	return nErrorCount;
}

EASTL_INTERNAL_RESTORE_DEPRECATED() // *: was declared deprecated
