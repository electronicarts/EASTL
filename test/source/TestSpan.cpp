/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EASTL/array.h>
#include <EASTL/span.h>
#include <EASTL/vector.h>

void TestSpanCtor(int& nErrorCount)
{
	using namespace eastl;

	{
		span<int> s;
		VERIFY(s.empty());
		VERIFY(s.size() == 0);
		VERIFY(s.data() == nullptr);
	}
	{
		span<float> s;
		VERIFY(s.empty());
		VERIFY(s.size() == 0);
		VERIFY(s.data() == nullptr);
	}
	{
		span<TestObject> s;
		VERIFY(s.empty());
		VERIFY(s.size() == 0);
		VERIFY(s.data() == nullptr);
	}

	{
		int arr[5] = {0, 1, 2, 3, 4};
		span<int> s(eastl::begin(arr), 5);
		VERIFY(s.data() == eastl::begin(arr));
		VERIFY(s.size() == 5);
		VERIFY(!s.empty());
	}

	{
		int arr[5] = {0, 1, 2, 3, 4};
		span<int> s(eastl::begin(arr), eastl::end(arr));
		VERIFY(s.data() == eastl::begin(arr));
		VERIFY(s.size() == 5);
		VERIFY(!s.empty());
	}

	{
		int arr[5] = {0, 1, 2, 3, 4};
		span<int> s(arr);
		VERIFY(s.data() == eastl::begin(arr));
		VERIFY(s.size() == 5);
		VERIFY(s.data()[2] == arr[2]);
		VERIFY(!s.empty());
	}

	{
		eastl::array<int, 5> arr = {{0, 1, 2, 3, 4}};
		span<int> s(arr);
		VERIFY(s.data() == eastl::begin(arr));
		VERIFY(s.size() == 5);
		VERIFY(s.data()[2] == arr.data()[2]);
		VERIFY(!s.empty());
	}

	{
		const eastl::array<int, 5> arr = {{0, 1, 2, 3, 4}};
		span<const int> s(arr);
		VERIFY(s.data() == eastl::begin(arr));
		VERIFY(s.size() == 5);
		VERIFY(s.data()[2] == arr.data()[2]);
		VERIFY(!s.empty());
	}

	{
		const eastl::array<int, 5> arr = {{0, 1, 2, 3, 4}};
		const span<const int> s(arr);
		VERIFY(s.data() == eastl::begin(arr));
		VERIFY(s.size() == 5);
		VERIFY(s.data()[2] == arr.data()[2]);
	}

	{
		class foo {};

		foo* pFoo = nullptr;

		auto f = [](eastl::span<const foo*>) {};

		eastl::array<const foo*, 1> foos = {{pFoo}};

		f(foos);
	}
}

void TestSpanSizeBytes(int& nErrorCount)
{
	using namespace eastl;

	{
		int arr[5] = {0, 1, 2, 3, 4};
		span<int> s(arr);
		VERIFY(s.size_bytes() == sizeof(arr));
		VERIFY(s.size_bytes() == (5 * sizeof(int)));
	}

	{
		float arr[8] = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f};
		span<float> s(arr);
		VERIFY(s.size_bytes() == sizeof(arr));
		VERIFY(s.size_bytes() == (8 * sizeof(float)));
	}

	{
		int64_t arr[5] = {0, 1, 2, 3, 4};
		span<int64_t> s(arr);
		VERIFY(s.size_bytes() == sizeof(arr));
		VERIFY(s.size_bytes() == (5 * sizeof(int64_t)));
	}
}

void TestSpanElementAccess(int& nErrorCount)
{
	using namespace eastl;

	{
		int arr[5] = {0, 1, 2, 3, 4};
		span<int> s(arr);

		VERIFY(s.front() == 0);
		VERIFY(s.back() == 4);

		VERIFY(s[0] == 0);
		VERIFY(s[1] == 1);
		VERIFY(s[2] == 2);
		VERIFY(s[3] == 3);
		VERIFY(s[4] == 4);

		VERIFY(s(0) == 0);
		VERIFY(s(1) == 1);
		VERIFY(s(2) == 2);
		VERIFY(s(3) == 3);
		VERIFY(s(4) == 4);
	}
}

void TestSpanIterators(int& nErrorCount)
{
	using namespace eastl;

	int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	span<int> s(arr);

	// ranged-for test
	{
		int* pBegin = arr;
		for(auto& e : arr)
		{
			VERIFY(e == *pBegin++);
		}
	}

	{
		auto testIteratorBegin = [&](auto p)
		{
			VERIFY(*p++ == 0);
			VERIFY(*p++ == 1);
			VERIFY(*p++ == 2);
			VERIFY(*p++ == 3);
			VERIFY(*p++ == 4);
			VERIFY(*p++ == 5);
			VERIFY(*p++ == 6);
			VERIFY(*p++ == 7);
			VERIFY(*p++ == 8);
			VERIFY(*p++ == 9);
		};

		auto testIteratorEnd = [&](auto p)
		{
			p--; // move pointer to a valid element

			VERIFY(*p-- == 9);
			VERIFY(*p-- == 8);
			VERIFY(*p-- == 7);
			VERIFY(*p-- == 6);
			VERIFY(*p-- == 5);
			VERIFY(*p-- == 4);
			VERIFY(*p-- == 3);
			VERIFY(*p-- == 2);
			VERIFY(*p-- == 1);
			VERIFY(*p-- == 0);
		};

		testIteratorBegin(s.begin());
		testIteratorBegin(s.cbegin());
		testIteratorEnd(s.end());
		testIteratorEnd(s.cend());
	}

	{
		auto testReverseIteratorBegin = [&](auto p)
		{
			VERIFY(*p++ == 9);
			VERIFY(*p++ == 8);
			VERIFY(*p++ == 7);
			VERIFY(*p++ == 6);
			VERIFY(*p++ == 5);
			VERIFY(*p++ == 4);
			VERIFY(*p++ == 3);
			VERIFY(*p++ == 2);
			VERIFY(*p++ == 1);
			VERIFY(*p++ == 0);
		};

		auto testReverseIteratorEnd = [&](auto p)
		{
			p--; // move pointer to a valid element

			VERIFY(*p-- == 0);
			VERIFY(*p-- == 1);
			VERIFY(*p-- == 2);
			VERIFY(*p-- == 3);
			VERIFY(*p-- == 4);
			VERIFY(*p-- == 5);
			VERIFY(*p-- == 6);
			VERIFY(*p-- == 7);
			VERIFY(*p-- == 8);
			VERIFY(*p-- == 9);
		};

		testReverseIteratorBegin(s.rbegin());
		testReverseIteratorBegin(s.crbegin());
		testReverseIteratorEnd(s.rend());
		testReverseIteratorEnd(s.crend());
	}
}

void TestSpanCopyAssignment(int& nErrorCount)
{
	using namespace eastl;

	{
		int arr[5] = {0, 1, 2, 3, 4};
		span<int> s(arr);
		span<int> sc = s;

		VERIFY(s[0] == sc[0]);
		VERIFY(s[1] == sc[1]);
		VERIFY(s[2] == sc[2]);
		VERIFY(s[3] == sc[3]);
		VERIFY(s[4] == sc[4]);

		VERIFY(s(0) == sc(0));
		VERIFY(s(1) == sc(1));
		VERIFY(s(2) == sc(2));
		VERIFY(s(3) == sc(3));
		VERIFY(s(4) == sc(4));
	}
}

void TestSpanContainerConversion(int& nErrorCount)
{
	using namespace eastl;

	{
		vector<int> v = {0, 1, 2, 3, 4, 5};
		span<const int> s(v);

		VERIFY(s.size() == static_cast<span<int>::index_type>(eastl::size(v)));
		VERIFY(s.data() == eastl::data(v));

		VERIFY(s[0] == v[0]);
		VERIFY(s[1] == v[1]);
		VERIFY(s[2] == v[2]);
		VERIFY(s[3] == v[3]);
		VERIFY(s[4] == v[4]);
		VERIFY(s[5] == v[5]);
	}

	{
		const vector<int> v = {0, 1, 2, 3, 4, 5};
		span<const int> s(v);

		VERIFY(s.size() == static_cast<span<int>::index_type>(eastl::size(v)));
		VERIFY(s.data() == eastl::data(v));

		VERIFY(s[0] == v[0]);
		VERIFY(s[1] == v[1]);
		VERIFY(s[2] == v[2]);
		VERIFY(s[3] == v[3]);
		VERIFY(s[4] == v[4]);
		VERIFY(s[5] == v[5]);
	}

	{
		vector<int> v = {0, 1, 2, 3, 4, 5};
		span<const int, 3> s1(v);
		span<const int> s2(s1);
		
		VERIFY(s2.size() == (span<const int>::index_type)v.size());
		VERIFY(s2[0] == v[0]);
		VERIFY(s2[1] == v[1]);

		VERIFY(s1.data() ==  v.data());
		VERIFY(s1.data() == s2.data());
	}

	{ // user reported regression for calling non-const span overload with a vector.
		auto f1 = [](span<int> s) { return s.size(); };
		auto f2 = [](span<const int> s) { return s.size(); };

		{
			vector<int> v = {0, 1, 2, 3, 4, 5};

			VERIFY(f1(v) == v.size());
			VERIFY(f2(v) == v.size());
		}

		{
			int a[] = {0, 1, 2, 3, 4, 5};

			VERIFY(f1(a) == EAArrayCount(a));
			VERIFY(f2(a) == EAArrayCount(a));
		}
	}
}

void TestSpanComparison(int& nErrorCount)
{
	using namespace eastl;

	int arr1[5] = {0, 1, 2, 3, 4};
	int arr2[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	{
		span<int> s1 = arr1;
		span<int> s2 = arr2;
		span<int> s3 = arr2;
		VERIFY(s2 == s3);
		VERIFY(s1 != s2);
		VERIFY(s1 < s2);
		VERIFY(s1 <= s2);
		VERIFY(s2 > s1);
		VERIFY(s2 >= s1);
	}
}

void TestSpanSubViews(int& nErrorCount)
{
	using namespace eastl;

	int arr1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	{
		span<int> s =  arr1;
		auto first_span = s.first<4>();
		VERIFY(first_span.size() == 4);
		VERIFY(first_span[0] == 0);
		VERIFY(first_span[1] == 1);
		VERIFY(first_span[2] == 2);
		VERIFY(first_span[3] == 3);
	}

	{
		span<int> s =  arr1;
		auto first_span = s.first(4);
		VERIFY(first_span.size() == 4);
		VERIFY(first_span[0] == 0);
		VERIFY(first_span[1] == 1);
		VERIFY(first_span[2] == 2);
		VERIFY(first_span[3] == 3);
	}

	{
		span<int> s =  arr1;
		auto first_span = s.last<4>();
		VERIFY(first_span.size() == 4);
		VERIFY(first_span[0] == 6);
		VERIFY(first_span[1] == 7);
		VERIFY(first_span[2] == 8);
		VERIFY(first_span[3] == 9);
	}

	{
		span<int> s =  arr1;
		auto first_span = s.last(4);
		VERIFY(first_span.size() == 4);
		VERIFY(first_span[0] == 6);
		VERIFY(first_span[1] == 7);
		VERIFY(first_span[2] == 8);
		VERIFY(first_span[3] == 9);
	}

	{ // subspan: full range
		span<int, 10> s =  arr1;

		auto fixed_span = s.subspan<0, 10>();
		VERIFY(fixed_span.size() == 10);
		VERIFY(fixed_span[0] == 0);
		VERIFY(fixed_span[1] == 1);
		VERIFY(fixed_span[8] == 8);
		VERIFY(fixed_span[9] == 9);

		auto dynamic_span = s.subspan(0, s.size());
		VERIFY(dynamic_span.size() == 10);
		VERIFY(dynamic_span[0] == 0);
		VERIFY(dynamic_span[1] == 1);
		VERIFY(dynamic_span[8] == 8);
		VERIFY(dynamic_span[9] == 9);
	}

	{ // subspan: subrange
		span<int, 10> s =  arr1;

		auto fixed_span = s.subspan<3, 4>();
		VERIFY(fixed_span.size() == 4);
		VERIFY(fixed_span[0] == 3);
		VERIFY(fixed_span[1] == 4);
		VERIFY(fixed_span[2] == 5);
		VERIFY(fixed_span[3] == 6);

		auto dynamic_span = s.subspan(3, 4);
		VERIFY(dynamic_span.size() == 4);
		VERIFY(dynamic_span[0] == 3);
		VERIFY(dynamic_span[1] == 4);
		VERIFY(dynamic_span[2] == 5);
		VERIFY(dynamic_span[3] == 6);
	}

	{ // subspan: default count
		span<int, 10> s =  arr1;

		auto fixed_span = s.subspan<3>();
		VERIFY(fixed_span.size() == 7);
		VERIFY(fixed_span[0] == 3);
		VERIFY(fixed_span[1] == 4);
		VERIFY(fixed_span[5] == 8);
		VERIFY(fixed_span[6] == 9);

		auto dynamic_span = s.subspan(3);
		VERIFY(dynamic_span.size() == 7);
		VERIFY(dynamic_span[0] == 3);
		VERIFY(dynamic_span[1] == 4);
		VERIFY(dynamic_span[5] == 8);
		VERIFY(dynamic_span[6] == 9);
	}
}

int TestSpan()
{
	int nErrorCount = 0;

	TestSpanCtor(nErrorCount);
	TestSpanSizeBytes(nErrorCount);
	TestSpanElementAccess(nErrorCount);
	TestSpanIterators(nErrorCount);
	TestSpanCopyAssignment(nErrorCount);
	TestSpanContainerConversion(nErrorCount);
	TestSpanComparison(nErrorCount);
	TestSpanSubViews(nErrorCount);

	return nErrorCount;
}
