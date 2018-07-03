/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"

#ifdef EA_COMPILER_CPP14_ENABLED
#include "ConceptImpls.h"
#include <EASTL/meta.h>


int TestGetTypeIndex()
{
	using namespace eastl;

	int nErrorCount = 0;

	static_assert(meta::get_type_index_v<short, short, char, int> == 0, "error");
	static_assert(meta::get_type_index_v<char, short, char, int> == 1, "error");
	static_assert(meta::get_type_index_v<int, short, char, int> == 2, "error");
	static_assert(meta::get_type_index_v<int, int, int, int> == 0, "error");

	return nErrorCount;
}

int TestGetType()
{
	using namespace eastl;

	int nErrorCount = 0;

	static_assert(is_same_v<meta::get_type_at_t<2, short, short, char, int>, char>, "error");
	static_assert(is_same_v<meta::get_type_at_t<3, char, short, char, int>, int>, "error");
	// static_assert(is_same_v<meta::get_type_at_t<4, int, short, char, int>, int>, "error");
	static_assert(is_same_v<meta::get_type_at_t<1, int, int, int, int>, int>, "error");

	return nErrorCount;
}

int TestTypeCount()
{
	using namespace eastl;

	int nErrorCount = 0;

	static_assert(meta::type_count_v<short, short, char, int> == 1, "error");
	static_assert(meta::type_count_v<char, short, char, int> == 1, "error");
	static_assert(meta::type_count_v<int, short, char, int> == 1, "error");
	static_assert(meta::type_count_v<int, int, int, int> == 3, "error");
	static_assert(meta::type_count_v<int, int, int, int, int, int, int, int, int> == 8, "error");
	static_assert(meta::type_count_v<int, int, int, int, char, int, int, int, int> == 7, "error");
	static_assert(meta::type_count_v<int, int, char, int, char, int, int, int, int> == 6, "error");
	static_assert(meta::type_count_v<int, int, char, int, char, int, int, int, char> == 5, "error");
	static_assert(meta::type_count_v<int, int, char, int, char, int, const int, int, char> == 4, "error");
	static_assert(meta::type_count_v<int, volatile int, char, int, char, int, const int, const volatile int, char> == 2, "error");

	return nErrorCount;
}

int TestDuplicateTypeCheck()
{
	using namespace eastl;

	int nErrorCount = 0;

	static_assert( meta::duplicate_type_check_v<short, short, char, int>, "error");
	static_assert( meta::duplicate_type_check_v<short, short, char, int, long, unsigned, long long>, "error");
	static_assert( meta::duplicate_type_check_v<int, const int, volatile int, const volatile int, int>, "error");
	static_assert(!meta::duplicate_type_check_v<short, short, char, int, long, unsigned, short, long long>, "error");

	return nErrorCount;
}

int TestOverloadResolution()
{
	using namespace eastl;
	using namespace eastl::meta;

	int nErrorCount = 0;

	static_assert(is_same_v<overload_resolution_t<int, overload_set<int>>, int>, "error");
	static_assert(is_same_v<overload_resolution_t<int, overload_set<short>>, short>, "error");
	static_assert(is_same_v<overload_resolution_t<int, overload_set<long>>, long>, "error");
	static_assert(is_same_v<overload_resolution_t<short, overload_set<int>>, int>, "error");
	static_assert(is_same_v<overload_resolution_t<int, overload_set<int, short, long>>, int>, "error");
	static_assert(is_same_v<overload_resolution_t<int, overload_set<short, int, long, float>>, int>, "error");
	static_assert(is_same_v<overload_resolution_t<int, overload_set<short, long, int, float, char>>, int>, "error");

	static_assert(is_same_v<overload_resolution_t<int, overload_set<int>>, int>, "error");
	static_assert(is_same_v<overload_resolution_t<int, overload_set<int, short>>, int>, "error");
	static_assert(is_same_v<overload_resolution_t<int, overload_set<int, short, long>>, int>, "error");

	return nErrorCount;
}


int TestMeta()
{
	int nErrorCount = 0;

	nErrorCount += TestGetTypeIndex();
	nErrorCount += TestGetType();
	nErrorCount += TestTypeCount();
	nErrorCount += TestDuplicateTypeCheck();
	nErrorCount += TestOverloadResolution();

	return nErrorCount;
}

#endif // EA_COMPILER_CPP14_ENABLED









