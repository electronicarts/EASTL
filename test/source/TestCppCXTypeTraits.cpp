/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/type_traits.h>

using namespace eastl;

#if defined(__cplusplus_winrt)
	ref class Foo
	{

	};
#endif

int TestCppCXTypeTraits()
{
	int nErrorCount = 0;

	// We can only build this code if C++/CX is enabled
#if defined(__cplusplus_winrt)
	{
		Foo^ foo = ref new Foo();
		static_assert(eastl::is_pod<Foo^>::value == false, "Ref types are not POD");
		static_assert(eastl::is_trivially_destructible<Foo^>::value == false, "Ref types cannot be trivially destructible");
		static_assert(eastl::is_trivially_constructible<Foo^>::value == false, "Ref types cannot be trivially constructible");
		static_assert(eastl::is_trivially_copy_constructible<Foo^>::value == false, "Ref types cannot be trivially copyable");
		static_assert(eastl::is_trivially_copy_assignable<Foo^>::value == false, "Ref types cannot be trivially copyable");
	}
#endif

	return nErrorCount;
}
