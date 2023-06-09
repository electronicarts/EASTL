/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
EA_DISABLE_GCC_WARNING(-Warray-bounds)

#include "EASTLTest.h"
#include <EASTL/fixed_string.h>
#include <EASTL/fixed_substring.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <string.h>

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


using namespace eastl;



// Template instantations.
// These tell the compiler to compile all the functions for the given class.
template class eastl::fixed_string<char8_t,    1,  true>;
template class eastl::fixed_string<char16_t,   1,  true>;
template class eastl::fixed_string<char32_t,   1,  true>;

template class eastl::fixed_string<char8_t,  128, false>;
template class eastl::fixed_string<char16_t, 128, false>;
template class eastl::fixed_string<char32_t, 128, false>;

template class eastl::fixed_string<char8_t,  128,  true, MallocAllocator>;
template class eastl::fixed_string<char16_t, 128,  true, MallocAllocator>;
template class eastl::fixed_string<char32_t, 128,  true, MallocAllocator>;

template class eastl::fixed_string<char8_t,  128, false, MallocAllocator>;
template class eastl::fixed_string<char16_t, 128, false, MallocAllocator>;
template class eastl::fixed_string<char32_t, 128, false, MallocAllocator>;

template class eastl::fixed_substring<char8_t>;
template class eastl::fixed_substring<char16_t>;




/*
// This does not compile, since the fixed_string allocator (among other things) is
// templated on sizeof(T), not just T. Thus, the full type is required at the time 
// of instantiation, but it is not available.
// See EATech Core JIRA issue ETCR-1608 for more information.
struct StructWithContainerOfStructs
{
	eastl::fixed_string<StructWithContainerOfStructs,4> children;
};
*/


int TestFixedSubstring()
{
	int nErrorCount = 0;

	{
		const char* pSource1 = "hello world";
		const char* pSource2 = "hola mundo";

		basic_string<char>    str(pSource1);
		fixed_substring<char> sub(str, 2, 5);

		EATEST_VERIFY(sub.size() == 5);
		EATEST_VERIFY(sub[0] == 'l');
		EATEST_VERIFY(sub == "llo w");

		sub.assign(pSource2);
		EATEST_VERIFY(sub.size() == 10);
		EATEST_VERIFY(sub[0] == pSource2[0]);
		EATEST_VERIFY(sub == pSource2);

		fixed_substring<char> sub2(sub);
		EATEST_VERIFY(sub2.size() == 10);
		EATEST_VERIFY(sub2[0] == pSource2[0]);
		EATEST_VERIFY(sub2 == pSource2);

		sub.assign(sub2, 1, 3);
		EATEST_VERIFY(sub.size() == 3);
		EATEST_VERIFY(sub == "ola");

		sub.assign(pSource2, 3);
		EATEST_VERIFY(sub.size() == 3);
		EATEST_VERIFY(sub == "hol");

		sub.assign(pSource2, pSource2 + 4);
		EATEST_VERIFY(sub.size() == 4);
		EATEST_VERIFY(sub == "hola");

		sub = pSource1;
		EATEST_VERIFY(sub.size() == strlen(pSource1));
		EATEST_VERIFY(sub == pSource1);
	}


	{   // Test fixed_substring with a C character array
		char                  pArray[256];
		fixed_substring<char> str(pArray, 255);

		str.resize(5);
		EATEST_VERIFY(str.size() == 5);

		str[0] = 'a';
		EATEST_VERIFY(str[0] == 'a');

		str.sprintf("Hello %s", "world");
		EATEST_VERIFY(str == "Hello world");

		str += " Hola mundo";
		EATEST_VERIFY(str == "Hello world Hola mundo");

		str.pop_back();
		EATEST_VERIFY(str == "Hello world Hola mund");

		str.replace(6, 5, "abcdefghijlk");
		EATEST_VERIFY(str == "Hello abcdefghijlk Hola mund");

		str.clear();
		EATEST_VERIFY(str.empty());
		EATEST_VERIFY(str == "");
	}


	{
		// Check that copies/moves don't become independent strings.
		// They should all point to the same sub-string.
		string str = "hello world";
		fixed_substring<char> sub(str, 2, 5);

		EATEST_VERIFY(sub.size() == 5);
		EATEST_VERIFY(sub[0] == 'l');
		EATEST_VERIFY(sub == "llo w");

		vector<fixed_substring<char>> v;
		for (eastl_size_t i = 0; i < 1000; ++i) {
			v.push_back(sub);
		}

		sub[0] = 'g';
		EATEST_VERIFY(str == "heglo world");
		EATEST_VERIFY(sub == "glo w");

		for (const auto& s : v){
			EATEST_VERIFY(s == "glo w");
		}

		// copy construct
		fixed_substring<char> sub2 = sub;

		// copy assign
		fixed_substring<char> sub3;
		sub3 = sub;

		// move construct
		fixed_substring<char> sub4 = eastl::move(sub);

		// move assign
		fixed_substring<char> sub_again(str, 2, 5);
		fixed_substring<char> sub5;
		sub5 = eastl::move(sub_again);

		EATEST_VERIFY(sub2 == "glo w");
		EATEST_VERIFY(sub3 == "glo w");
		EATEST_VERIFY(sub4 == "glo w");
		EATEST_VERIFY(sub5 == "glo w");

		str[5] = 'g';
		EATEST_VERIFY(sub2 == "glogw");
		EATEST_VERIFY(sub3 == "glogw");
		EATEST_VERIFY(sub4 == "glogw");
		EATEST_VERIFY(sub5 == "glogw");

	}

	return nErrorCount;
}


int TestFixedString()
{
	int nErrorCount = 0;

	{
		fixed_string<char, 64>::CtorSprintf cs;

		fixed_string<char, 64> s8(cs, "hello world %d.", 1);
		EATEST_VERIFY(s8 == "hello world 1.");
		EATEST_VERIFY(s8.capacity() == 63); // 63 because the 64 includes the terminating 0, but capacity() subtracts the terminating 0 usage.
		EATEST_VERIFY(s8.max_size() == 63);

		s8.append_sprintf(" More hello %d.", 2);
		EATEST_VERIFY(s8 == "hello world 1. More hello 2.");
		EATEST_VERIFY(s8.capacity() == 63);
	}


	{
		fixed_string<wchar_t, 64>::CtorSprintf cs;

		fixed_string<wchar_t, 64> sW(cs, L"hello world %d.", 1);
		EATEST_VERIFY(sW == L"hello world 1.");
		EATEST_VERIFY(sW.capacity() == 63); // 63 because the 64 includes the terminating 0, but capacity() subtracts the terminating 0 usage.

		sW.append_sprintf(L" More hello %d.", 2);
		EATEST_VERIFY(sW == L"hello world 1. More hello 2.");
		EATEST_VERIFY(sW.capacity() == 63); // 63 because the 64 includes the terminating 0, but capacity() subtracts the terminating 0 usage.
	}

	{
		string s("frost");
		size_t sHash = eastl::hash<string>{}(s);

		// char
		fixed_string<char, 64> fsc1("frost");
		fixed_string<char, 64> fsc2("bite");
		fixed_string<char, 64> fsc3("bite");

		size_t fsc1Hash = eastl::hash<fixed_string<char, 64>>{}(fsc1);
		size_t fsc2Hash = eastl::hash<fixed_string<char, 64>>{}(fsc2);
		size_t fsc3Hash = eastl::hash<fixed_string<char, 64>>{}(fsc3);

		EATEST_VERIFY(fsc1Hash == sHash);
		EATEST_VERIFY(fsc1Hash != fsc2Hash);
		EATEST_VERIFY(fsc2Hash == fsc3Hash);

		// wchar_t
		fixed_string<wchar_t, 64> fswc1(L"frost");
		fixed_string<wchar_t, 64> fswc2(L"bite");
		fixed_string<wchar_t, 64> fswc3(L"bite");

		size_t fswc1Hash = eastl::hash<fixed_string<wchar_t, 64>>{}(fswc1);
		size_t fswc2Hash = eastl::hash<fixed_string<wchar_t, 64>>{}(fswc2);
		size_t fswc3Hash = eastl::hash<fixed_string<wchar_t, 64>>{}(fswc3);

		EATEST_VERIFY(fswc1Hash == sHash);
		EATEST_VERIFY(fswc1Hash != fswc2Hash);
		EATEST_VERIFY(fswc2Hash == fswc3Hash);

		// char8_t
		fixed_string<char8_t, 64> fsc81("frost");
		fixed_string<char8_t, 64> fsc82("bite");
		fixed_string<char8_t, 64> fsc83("bite");

		size_t fsc81Hash = eastl::hash<fixed_string<char8_t, 64>>{}(fsc81);
		size_t fsc82Hash = eastl::hash<fixed_string<char8_t, 64>>{}(fsc82);
		size_t fsc83Hash = eastl::hash<fixed_string<char8_t, 64>>{}(fsc83);

		EATEST_VERIFY(fsc81Hash == sHash);
		EATEST_VERIFY(fsc81Hash != fsc82Hash);
		EATEST_VERIFY(fsc82Hash == fsc83Hash);
	}


	{
		typedef fixed_string<char8_t, 64, true>  FixedString64;
		typedef fixed_string<char8_t, 64, false> FixedString64NoOverflow;
		FixedString64::CtorSprintf               cs;
		FixedString64::CtorDoNotInitialize       cdni;

		// fixed_string();
		FixedString64 fs1;
		EATEST_VERIFY(fs1.size() == 0);
		EATEST_VERIFY(fs1.capacity() == 63);

		FixedString64NoOverflow fsNo;
		EATEST_VERIFY(fs1.can_overflow() == true);
		EATEST_VERIFY(fsNo.can_overflow() == false);
		EATEST_VERIFY(fs1.full() == false);
		EATEST_VERIFY(fs1.has_overflowed() == false);

		const char8_t* pCStr = fs1.c_str();
		EATEST_VERIFY(*pCStr == 0);

		// fixed_string(const this_type& x);
		FixedString64 fs2(fs1);
		EATEST_VERIFY(fs2.size() == 0);
		EATEST_VERIFY(fs2.capacity() == 63);

		fs1 = EA_CHAR8("abc");
		FixedString64 fs3(fs1);
		EATEST_VERIFY(fs3.size() == 3);
		EATEST_VERIFY(fs3.capacity() == 63);
		EATEST_VERIFY(fs3 == EA_CHAR8("abc"));

		// fixed_string(const this_type& x, size_type position, size_type n = npos);
		FixedString64 fs4(fs1, 1, 2);
		EATEST_VERIFY(fs4.size() == 2);
		EATEST_VERIFY(fs4.capacity() == 63);
		EATEST_VERIFY(fs4 == EA_CHAR8("bc"));

		// fixed_string(const value_type* p, size_type n);
		FixedString64 fs5(EA_CHAR8("abcdef"), 6);
		EATEST_VERIFY(fs5.size() == 6);
		EATEST_VERIFY(fs5.capacity() == 63);
		EATEST_VERIFY(fs5 == EA_CHAR8("abcdef"));

		// fixed_string(const value_type* p);
		FixedString64 fs6(EA_CHAR8("abcdef"));
		EATEST_VERIFY(fs6.size() == 6);
		EATEST_VERIFY(fs6.capacity() == 63);
		EATEST_VERIFY(fs6 == EA_CHAR8("abcdef"));

		// fixed_string(size_type n, const value_type& value);
		FixedString64 fs7(8, 'a');
		EATEST_VERIFY(fs7.size() == 8);
		EATEST_VERIFY(fs7.capacity() == 63);
		EATEST_VERIFY(fs7 == EA_CHAR8("aaaaaaaa"));

		// fixed_string(const value_type* pBegin, const value_type* pEnd);
		FixedString64 fs8(&fs6[0], &fs6[5]);
		EATEST_VERIFY(fs8.size() == 5);
		EATEST_VERIFY(fs8.capacity() == 63);
		EATEST_VERIFY(fs8 == EA_CHAR8("abcde"));

		// fixed_string(CtorDoNotInitialize, size_type n);
		FixedString64 fs9(cdni, 7);
		EATEST_VERIFY(fs9.size() == 7);
		EATEST_VERIFY(fs9.capacity() == 63);

		// fixed_string(CtorSprintf, const value_type* pFormat, ...);
		FixedString64 fs10(cs, EA_CHAR8("%d"), 37);
		EATEST_VERIFY(fs10.size() == 2);
		EATEST_VERIFY(fs10.capacity() == 63);
		EATEST_VERIFY(fs10 == EA_CHAR8("37"));

		// this_type& operator=(const const value_type* p);
		// this_type& operator=(const this_type& x);
		fs9 = EA_CHAR8("hello");
		EATEST_VERIFY(fs9 == EA_CHAR8("hello"));

		fs9 = fs10;
		EATEST_VERIFY(fs9 == fs10);
		EATEST_VERIFY(fs9 == EA_CHAR8("37"));

		// void swap(this_type& x);
		swap(fs7, fs9);
		EATEST_VERIFY(fs7 == EA_CHAR8("37"));
		EATEST_VERIFY(fs9 == EA_CHAR8("aaaaaaaa"));

		// void set_capacity(size_type n);
		fs9.set_capacity(100);
		EATEST_VERIFY(fs9.size() == 8);
		EATEST_VERIFY(fs9.capacity() == 100);
		EATEST_VERIFY(fs9.full() == true);
		EATEST_VERIFY(fs9.has_overflowed() == true);

		fs9.set_capacity(100); // EATEST_VERIFY that this has no effect.
		EATEST_VERIFY(fs9.size() == 8);
		EATEST_VERIFY(fs9.capacity() == 100);
		EATEST_VERIFY(fs9.full() == true);
		EATEST_VERIFY(fs9.has_overflowed() == true);

		fs9.resize(100);
		fs9.set_capacity(100);
		EATEST_VERIFY(fs9.size() == 100);
		EATEST_VERIFY(fs9.capacity() == 100);
		EATEST_VERIFY(fs9.full() == true);
		EATEST_VERIFY(fs9.has_overflowed() == true);

		fs9.set_capacity(1);
		EATEST_VERIFY(fs9.size() == 1);
		EATEST_VERIFY(fs9.capacity() < fs9.max_size()); // We don't test for capacity == 1, because with fixed_strings, the fixed-size capacity is the lowest it ever gets.
		EATEST_VERIFY(fs9.full() == false);
		EATEST_VERIFY(fs9.has_overflowed() == false);

		fs9.set_capacity(0);
		EATEST_VERIFY(fs9.size() == 0);
		EATEST_VERIFY(fs9.capacity() < fs9.max_size()); // We don't test for capacity == 1, because with fixed_strings, the fixed-size capacity is the lowest it ever gets.
		EATEST_VERIFY(fs9.full() == false);
		EATEST_VERIFY(fs9.has_overflowed() == false);

		// Exercise the freeing of memory in set_capacity.
		fixed_string<char8_t, 64, true> fs88;
		eastl_size_t capacity = fs88.capacity();
		fs88.resize(capacity);
		fs88.set_capacity(capacity * 2);
		EATEST_VERIFY(fs88.capacity() >= (capacity * 2));

		// void reset_lose_memory();
		fs6.reset_lose_memory();
		EATEST_VERIFY(fs6.size() == 0);
		EATEST_VERIFY(fs5.capacity() == 63);

		// size_type max_size() const;
		EATEST_VERIFY(fs7.max_size() == 63);


		// global operator +
		{
			// fixed_string operator+(const fixed_string& a, const fixed_string& b);
			// fixed_string operator+(value_type* p, const fixed_string& b);
			// fixed_string operator+(value_type c, const fixed_string& b);
			// fixed_string operator+(const fixed_string& a, const value_type* p);
			// fixed_string operator+(const fixed_string& a, value_type c);

			typedef fixed_string<char, 8, true> FSTest; // Make it a small size so it's easily overflowed when we want.

			FSTest a("abc");
			FSTest b("def");
			FSTest c(a + b);
			EATEST_VERIFY(c == "abcdef");
			c = a + "ghi";
			EATEST_VERIFY(c == "abcghi");
			c = "ghi" + a;
			EATEST_VERIFY(c == "ghiabc");
			c = a + 'g';
			EATEST_VERIFY(c == "abcg");
			c = 'g' + a;
			EATEST_VERIFY(c == "gabc");

			// fixed_string operator+(fixed_string&& a,    fixed_string&& b);
			// fixed_string operator+(fixed_string&& a,    const fixed_string& b);
			// fixed_string operator+(const value_type* p, fixed_string&& b);
			// fixed_string operator+(fixed_string&& a,    const value_type* p);
			// fixed_string operator+(fixed_string&& a,    value_type b);

			c = eastl::move(a) + eastl::move(b);
			EATEST_VERIFY(c == "abcdef");
			c.clear();

			FSTest a1("abc");
			FSTest b1("def");
			c = eastl::move(a1) + b1;
			EATEST_VERIFY(c == "abcdef");
			c.clear();

			FSTest b2("def");
			c = "abc" + eastl::move(b2);
			EATEST_VERIFY(c == "abcdef");
			c.clear();

			FSTest a3("abc");
			c = eastl::move(a3) + "def";
			EATEST_VERIFY(c == "abcdef");
			c.clear();

			FSTest a4("abc");
			c = eastl::move(a4) + 'd';
			EATEST_VERIFY(c == "abcd");
			c.clear();
		}


		// bool operator==(const fixed_string<& a, const fixed_string& b)
		// bool operator!=(const fixed_string<& a, const fixed_string& b)
		EATEST_VERIFY(  fs7 != fs8);
		EATEST_VERIFY(!(fs7 == fs8));
		fs7 = fs8;
		EATEST_VERIFY(  fs7 == fs8);
		EATEST_VERIFY(!(fs7 != fs8));
	}


	{   // Test overflow allocator specification
		
		typedef fixed_string<char8_t, 64, true, MallocAllocator> FixedString64Malloc;

		FixedString64Malloc fs;

		fs.push_back('a');
		EATEST_VERIFY(fs.size() == 1);
		EATEST_VERIFY(fs[0] == 'a');

		fs.resize(95);
		fs[94] = 'b';
		EATEST_VERIFY(fs[0] == 'a');
		EATEST_VERIFY(fs[94] == 'b');
		EATEST_VERIFY(fs.size() == 95);

		fs.clear();
		EATEST_VERIFY(fs.empty());

		fs.push_back('a');
		EATEST_VERIFY(fs.size() == 1);
		EATEST_VERIFY(fs[0] == 'a');

		fs.resize(195);
		fs[194] = 'b';
		EATEST_VERIFY(fs[0] == 'a');
		EATEST_VERIFY(fs[194] == 'b');
		EATEST_VERIFY(fs.size() == 195);
	}

	{
		// Test construction of a container with an overflow allocator constructor argument.
		MallocAllocator overflowAllocator;
		void* p = overflowAllocator.allocate(1);
		fixed_string<char8_t, 64, true, MallocAllocator> c(overflowAllocator);
		c.resize(65);
		EATEST_VERIFY(c.get_overflow_allocator().mAllocCount == 2); // 1 for above, and 1 for overflowing from 64 to 65.
		overflowAllocator.deallocate(p, 1);
	}

	{
		// Regression for compile failure when EASTL_NO_RVALUE_REFERENCES is 0.
		typedef eastl::fixed_string<char, 32, true, MallocAllocator> TestString;

		TestString ts1;
		TestString ts2(ts1 + "Test");

		EATEST_VERIFY(ts1.empty() && ts2.size() == 4);
	}

	{
		// Test equality tests of differently-sized fixed_strings.

		/* Disabled because this isn't currently supported by fixed_string.
		typedef fixed_string<char8_t, 64, true, MallocAllocator> FixedString64Malloc;
		typedef fixed_string<char8_t, 32>                        FixedString32;

		FixedString64Malloc s64M;
		FixedString32       s32;

		EATEST_VERIFY(s64M == s32);
		*/
	}

	nErrorCount += TestFixedSubstring();

	return nErrorCount;
}












