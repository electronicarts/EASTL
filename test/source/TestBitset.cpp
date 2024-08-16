/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/bitset.h>
#include <EABase/eabase.h>
#include <EASTL/bit.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER)
	#pragma warning(pop)
	#pragma warning(disable: 4310) // Cast truncates constant value
#endif


using namespace eastl;


// Template instantations.
// These tell the compiler to compile all the functions for the given class.

#if (EASTL_BITSET_WORD_SIZE_DEFAULT != 1)
	template class eastl::bitset<1,   uint8_t>;
	template class eastl::bitset<33,  uint8_t>;
	template class eastl::bitset<65,  uint8_t>;
	template class eastl::bitset<129, uint8_t>;
#endif

#if (EASTL_BITSET_WORD_SIZE_DEFAULT != 2)
	template class eastl::bitset<1,   uint16_t>;
	template class eastl::bitset<33,  uint16_t>;
	template class eastl::bitset<65,  uint16_t>;
	template class eastl::bitset<129, uint16_t>;
#endif

#if (EASTL_BITSET_WORD_SIZE_DEFAULT != 4) // If not already represented 
	template class eastl::bitset<1,   uint32_t>;
	template class eastl::bitset<33,  uint32_t>;
	template class eastl::bitset<65,  uint32_t>;
	template class eastl::bitset<129, uint32_t>;
#endif

#if (EASTL_BITSET_WORD_SIZE_DEFAULT != 8)
	template class eastl::bitset<1,   uint64_t>;
	template class eastl::bitset<33,  uint64_t>;
	template class eastl::bitset<65,  uint64_t>;
	template class eastl::bitset<129, uint64_t>;
#endif

#if (EASTL_BITSET_WORD_SIZE_DEFAULT != 16)
	#if EASTL_INT128_SUPPORTED
		template class eastl::bitset<1,   eastl_uint128_t>;
		template class eastl::bitset<33,  eastl_uint128_t>;
		template class eastl::bitset<65,  eastl_uint128_t>;
		template class eastl::bitset<129, eastl_uint128_t>;
	#endif
#endif

template<typename UInt>
int TestGetFirstBit()
{
	int nErrorCount = 0;

	EATEST_VERIFY(GetFirstBit((UInt) 0) == sizeof(UInt) * CHAR_BIT);
	// EATEST_VERIFY(countr_zero((UInt) 0) == sizeof(UInt) * CHAR_BIT);

	for (uint32_t i = 0; i < sizeof(UInt) * CHAR_BIT; ++i)
	{
		UInt x = ((UInt) 1 << i) | (UInt(1) << (sizeof(UInt) * CHAR_BIT - 1));
		EATEST_VERIFY(GetFirstBit(x) == i);
		//EATEST_VERIFY(GetFirstBit(x) == countr_zero(x));
	}

	return nErrorCount;
}

template<typename UInt>
int TestGetLastBit()
{
	int nErrorCount = 0;

	EATEST_VERIFY(GetLastBit((UInt) 0) == sizeof(UInt) * CHAR_BIT);
#if defined(EA_COMPILER_CPP20_ENABLED)
	EATEST_VERIFY(bit_width((UInt) 0) == 0);
#endif

	for (uint32_t i = 0; i < sizeof(UInt) * CHAR_BIT; ++i)
	{
		UInt x = ((UInt)1 << i) | UInt(1);
		EATEST_VERIFY(GetLastBit(x) == i);
#if defined(EA_COMPILER_CPP20_ENABLED)
		EATEST_VERIFY(GetLastBit(x) == ((uint32_t) bit_width(x) - 1));
#endif
	}

	return nErrorCount;
}

template<size_t N, typename WordType, typename UInt>
int VerifyBitsetConversionThrows(const bitset<N, WordType>& bs, UInt(*convert)(const bitset<N, WordType>&), UInt truncatedValue) {
	int nErrorCount = 0;

#if EASTL_EXCEPTIONS_ENABLED
	try {
		convert(bs);
		EATEST_VERIFY(false);
	}
	catch (std::overflow_error&)
	{
		EATEST_VERIFY(true);  // This pathway should be taken.
	}
	catch (...)
	{
		EATEST_VERIFY(false);
	}
#else
	EATEST_VERIFY(convert(bs) == truncatedValue);
#endif

	return nErrorCount;
}

template<typename WordType>
int TestBitsetWithWord()
{
	int nErrorCount = 0;

	auto verifyToUint32Throws = [&nErrorCount](const auto& bs, uint32_t truncatedValue) {
		uint32_t(*convert)(const decltype(bs)&) = [](const auto& bs) { return bs.to_uint32_no_assert_convertible(); };
		nErrorCount += VerifyBitsetConversionThrows(bs, convert, truncatedValue);
	};

	auto verifyToUint64Throws = [&nErrorCount](const auto& bs, uint64_t truncatedValue) {
		uint64_t(*convert)(const decltype(bs)&) = [](const auto& bs) { return bs.to_uint64_no_assert_convertible(); };
		nErrorCount += VerifyBitsetConversionThrows(bs, convert, truncatedValue);
	};

	auto verifyToUlongThrowsIf32bit = [&nErrorCount](const auto& bs, unsigned long truncatedValue) {
		if constexpr (sizeof(unsigned long) < 8)
		{
			unsigned long(*convert)(const decltype(bs)&) = [](const auto& bs) { return bs.to_ulong_no_assert_convertible(); };
			nErrorCount += VerifyBitsetConversionThrows(bs, convert, truncatedValue);
		}
		else
		{
			EATEST_VERIFY(bs.to_ulong_no_assert_convertible() == truncatedValue);
		}
	};

	{
		// bitset<0, WordType> tests
#if !defined(__GNUC__) || (__GNUC__ >= 3) // GCC before v3.0 can't handle our bitset<0, WordType>.
		bitset<0, WordType> b0(0x10101010);
		EATEST_VERIFY(b0.count() == 0);
		EATEST_VERIFY(b0.to_ulong_assert_convertible() == 0x00000000);
		EATEST_VERIFY(b0.to_uint32_assert_convertible() == 0x00000000);
		EATEST_VERIFY(b0.to_uint64_assert_convertible() == 0x00000000);
		EATEST_VERIFY(b0.to_uint64_assert_convertible() == 0x00000000);
		EATEST_VERIFY(b0.as_uint64() == 0x00000000);
		EATEST_VERIFY(b0.template as_uint<uint64_t>() == 0x00000000);

		b0.flip();
		EATEST_VERIFY(b0.count() == 0);
		EATEST_VERIFY(b0.to_ulong_assert_convertible() == 0x00000000);
		EATEST_VERIFY(b0.to_uint32_assert_convertible() == 0x00000000);
		EATEST_VERIFY(b0.to_uint64_assert_convertible() == 0x00000000);
		EATEST_VERIFY(b0.to_uint64_assert_convertible() == 0x00000000);
		EATEST_VERIFY(b0.as_uint64() == 0x00000000);
		EATEST_VERIFY(b0.template as_uint<uint64_t>() == 0x00000000);

		b0 <<= 1;
		EATEST_VERIFY(b0.count() == 0);
		EATEST_VERIFY(b0.to_ulong_assert_convertible() == 0x00000000);
		EATEST_VERIFY(b0.to_uint32_assert_convertible() == 0x00000000);
		EATEST_VERIFY(b0.to_uint64_assert_convertible() == 0x00000000);
		EATEST_VERIFY(b0.to_uint64_assert_convertible() == 0x00000000);
		EATEST_VERIFY(b0.as_uint64() == 0x00000000);
		EATEST_VERIFY(b0.template as_uint<uint64_t>() == 0x00000000);

		b0 = 0x10101010;
		EATEST_VERIFY(b0.to_uint32_assert_convertible() == 0x00000000);
#if EA_IS_ENABLED(EASTL_DEPRECATIONS_FOR_2024_SEPT)
		b0 = UINT64_C(0x1010101010101010);
#else
		b0.from_uint64(UINT64_C(0x1010101010101010));
#endif
		EATEST_VERIFY(b0.to_uint64_assert_convertible() == UINT64_C(0x0000000000000000));
#endif

// bitset<8, WordType> tests
		bitset<8, WordType> b8(0x10101010);
		EATEST_VERIFY(b8.count() == 1);
		EATEST_VERIFY(b8.to_ulong_assert_convertible()  == 0x00000010);
		EATEST_VERIFY(b8.to_uint32_assert_convertible() == 0x00000010);
		EATEST_VERIFY(b8.to_uint64_assert_convertible() == 0x00000010);
		EATEST_VERIFY(b8.to_uint64_assert_convertible() == 0x00000010);
		EATEST_VERIFY(b8.as_uint64() == 0x00000010);
		EATEST_VERIFY(b8.template as_uint<uint64_t>() == 0x00000010);

		b8.flip();
		EATEST_VERIFY(b8.count() == 7);
		EATEST_VERIFY(b8.to_ulong_assert_convertible()  == 0x000000ef);
		EATEST_VERIFY(b8.to_uint32_assert_convertible() == 0x000000ef);
		EATEST_VERIFY(b8.to_uint64_assert_convertible() == 0x000000ef);
		EATEST_VERIFY(b8.to_uint64_assert_convertible() == 0x000000ef);
		EATEST_VERIFY(b8.as_uint64() == 0x000000ef);
		EATEST_VERIFY(b8.template as_uint<uint64_t>() == 0x000000ef);

		b8 <<= 1;
		EATEST_VERIFY(b8.count() == 6);
		EATEST_VERIFY(b8.to_ulong_assert_convertible()  == 0x000000de);
		EATEST_VERIFY(b8.to_uint32_assert_convertible() == 0x000000de);
		EATEST_VERIFY(b8.to_uint64_assert_convertible() == 0x000000de);
		EATEST_VERIFY(b8.to_uint64_assert_convertible() == 0x000000de);
		EATEST_VERIFY(b8.as_uint64() == 0x000000de);
		EATEST_VERIFY(b8.template as_uint<uint64_t>() == 0x000000de);

		b8.reset();
		b8.flip();
		b8 >>= 33;
		EATEST_VERIFY(b8.count() == 0);

		b8.reset();
		b8.flip();
		b8 >>= 65;
		EATEST_VERIFY(b8.count() == 0);

		b8 = 0x10101010;
		EATEST_VERIFY(b8.to_uint32_assert_convertible() == 0x00000010);
		b8 = UINT64_C(0x0000000000000010);
		EATEST_VERIFY(b8.to_uint64_assert_convertible() == UINT64_C(0x0000000000000010));



		// bitset<16, WordType> tests
		bitset<16, WordType> b16(0x10101010);
		EATEST_VERIFY(b16.count() == 2);
		EATEST_VERIFY(b16.to_ulong_assert_convertible()  == 0x00001010);
		EATEST_VERIFY(b16.to_uint32_assert_convertible() == 0x00001010);
		EATEST_VERIFY(b16.to_uint64_assert_convertible() == 0x00001010);
		EATEST_VERIFY(b16.to_uint64_assert_convertible() == 0x00001010);
		EATEST_VERIFY(b16.as_uint64() == 0x00001010);
		EATEST_VERIFY(b16.template as_uint<uint64_t>() == 0x00001010);

		b16.flip();
		EATEST_VERIFY(b16.count() == 14);
		EATEST_VERIFY(b16.to_ulong_assert_convertible()  == 0x0000efef);
		EATEST_VERIFY(b16.to_uint32_assert_convertible() == 0x0000efef);
		EATEST_VERIFY(b16.to_uint64_assert_convertible() == 0x0000efef);
		EATEST_VERIFY(b16.to_uint64_assert_convertible() == 0x0000efef);
		EATEST_VERIFY(b16.as_uint64() == 0x0000efef);
		EATEST_VERIFY(b16.template as_uint<uint64_t>() == 0x0000efef);

		b16 <<= 1;
		EATEST_VERIFY(b16.count() == 13);
		EATEST_VERIFY(b16.to_ulong_assert_convertible()  == 0x0000dfde);
		EATEST_VERIFY(b16.to_uint32_assert_convertible() == 0x0000dfde);
		EATEST_VERIFY(b16.to_uint64_assert_convertible() == 0x0000dfde);
		EATEST_VERIFY(b16.to_uint64_assert_convertible() == 0x0000dfde);
		EATEST_VERIFY(b16.as_uint64() == 0x0000dfde);
		EATEST_VERIFY(b16.template as_uint<uint64_t>() == 0x0000dfde);

		b16.reset();
		b16.flip();
		b16 >>= 33;
		EATEST_VERIFY(b16.count() == 0);

		b16.reset();
		b16.flip();
		b16 >>= 65;
		EATEST_VERIFY(b16.count() == 0);

		b16 = 0x10101010;
		EATEST_VERIFY(b16.to_uint32_assert_convertible() == 0x00001010);
		b16 = UINT64_C(0x0000000000001010);
		EATEST_VERIFY(b16.to_uint64_assert_convertible() == UINT64_C(0x0000000000001010));



		// bitset<32, WordType> tests
		bitset<32, WordType> b32(0x10101010);
		EATEST_VERIFY(b32.count() == 4);
		EATEST_VERIFY(b32.to_ulong_assert_convertible()  == 0x10101010);
		EATEST_VERIFY(b32.to_uint32_assert_convertible() == 0x10101010);
		EATEST_VERIFY(b32.to_uint64_assert_convertible() == 0x10101010);
		EATEST_VERIFY(b32.to_uint64_assert_convertible() == 0x10101010);
		EATEST_VERIFY(b32.as_uint64() == 0x10101010);
		EATEST_VERIFY(b32.template as_uint<uint64_t>() == 0x10101010);

		b32.flip();
		EATEST_VERIFY(b32.count() == 28);
		EATEST_VERIFY(b32.to_ulong_assert_convertible()  == 0xefefefef);
		EATEST_VERIFY(b32.to_uint32_assert_convertible() == 0xefefefef);
		EATEST_VERIFY(b32.to_uint64_assert_convertible() == 0xefefefef);
		EATEST_VERIFY(b32.to_uint64_assert_convertible() == 0xefefefef);
		EATEST_VERIFY(b32.as_uint64() == 0xefefefef);
		EATEST_VERIFY(b32.template as_uint<uint64_t>() == 0xefefefef);

		b32 <<= 1;
		EATEST_VERIFY(b32.count() == 27);
		EATEST_VERIFY(b32.to_ulong_assert_convertible()  == 0xdfdfdfde);
		EATEST_VERIFY(b32.to_uint32_assert_convertible() == 0xdfdfdfde);
		EATEST_VERIFY(b32.to_uint64_assert_convertible() == 0xdfdfdfde);
		EATEST_VERIFY(b32.to_uint64_assert_convertible() == 0xdfdfdfde);
		EATEST_VERIFY(b32.as_uint64() == 0xdfdfdfde);
		EATEST_VERIFY(b32.template as_uint<uint64_t>() == 0xdfdfdfde);

		b32.reset();
		b32.flip();
		b32 >>= 33;
		EATEST_VERIFY(b32.count() == 0);

		b32.reset();
		b32.flip();
		b32 >>= 65;
		EATEST_VERIFY(b32.count() == 0);

		b32 = 0x10101010;
		EATEST_VERIFY(b32.to_uint32_assert_convertible() == 0x10101010);
		b32 = UINT64_C(0x0000000010101010);
		EATEST_VERIFY(b32.to_uint64_assert_convertible() == UINT64_C(0x0000000010101010));

		// bitset<64, WordType> tests
		bitset<64, WordType> b64(0x10101010);                                 // b64 => 00000000 00000000 00000000 00000000 00010000 00010000 00010000 00010000
		EATEST_VERIFY(b64.count() == 4);
		EATEST_VERIFY(b64.to_ulong_assert_convertible()  == 0x10101010);
		EATEST_VERIFY(b64.to_uint32_assert_convertible() == 0x10101010);
		EATEST_VERIFY(b64.to_uint64_assert_convertible() == 0x10101010);
		EATEST_VERIFY(b64.to_uint64_assert_convertible() == 0x10101010);
		// EATEST_VERIFY(b64.as_uint32() == 0x10101010); // compile error: cannot represent the entire bitset as a uint32_t.
		EATEST_VERIFY(b64.as_uint64() == 0x10101010);
		EATEST_VERIFY(b64.template as_uint<uint64_t>() == 0x10101010);

		b64.flip();                                                 // b64 => 11111111 11111111 11111111 11111111 11101111 11101111 11101111 11101111
		EATEST_VERIFY(b64.count() == 60);
		verifyToUlongThrowsIf32bit(b64, static_cast<unsigned long>(0xffffffffefefefeful));
		verifyToUint32Throws(b64, 0xefefefef);
		EATEST_VERIFY(b64.to_uint64_assert_convertible() == 0xffffffffefefefefull);
		EATEST_VERIFY(b64.as_uint64() == 0xffffffffefefefefull);
		EATEST_VERIFY(b64.template as_uint<uint64_t>() == 0xffffffffefefefefull);

		b64 <<= 1;                                                  // b64 => 11111111 11111111 11111111 11111111 11011111 11011111 11011111 11011110
		EATEST_VERIFY(b64.count() == 59);
		verifyToUlongThrowsIf32bit(b64, static_cast<unsigned long>(0xffffffffdfdfdfdeul));
		verifyToUint32Throws(b64, 0xdfdfdfde);
		EATEST_VERIFY(b64.to_uint64_assert_convertible() == 0xffffffffdfdfdfdeull);
		EATEST_VERIFY(b64.as_uint64() == 0xffffffffdfdfdfdeull);
		EATEST_VERIFY(b64.template as_uint<uint64_t>() == 0xffffffffdfdfdfdeull);

		b64.reset();                                                // b64 => 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
		EATEST_VERIFY(b64.count() == 0);
		EATEST_VERIFY(b64.to_ulong_assert_convertible() == 0);

		b64 <<= 1;                                                  // b64 => 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
		EATEST_VERIFY(b64.count() == 0);
		EATEST_VERIFY(b64.to_ulong_assert_convertible() == 0);

		b64.flip();                                                 // b64 => 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
		EATEST_VERIFY(b64.count() == 64);
		verifyToUlongThrowsIf32bit(b64, static_cast<unsigned long>(0xfffffffffffffffful));

		b64 <<= 1;                                                  // b64 => 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111110
		EATEST_VERIFY(b64.count() == 63);
		verifyToUlongThrowsIf32bit(b64, static_cast<unsigned long>(0xfffffffffffffffeul));

		b64.reset();
		b64.flip();
		b64 >>= 33;
		EATEST_VERIFY(b64.count() == 31);

		b64.reset();
		b64.flip();
		b64 >>= 65;
		EATEST_VERIFY(b64.count() == 0);

		b64 = 0x10101010;
		EATEST_VERIFY(b64.to_uint32_assert_convertible() == 0x10101010);
#if EA_IS_ENABLED(EASTL_DEPRECATIONS_FOR_2024_SEPT)
		b64 = UINT64_C(0x1010101010101010);
#else
		b64.from_uint64(UINT64_C(0x1010101010101010));
#endif
		EATEST_VERIFY(b64.to_uint64_assert_convertible() == UINT64_C(0x1010101010101010));
	}


	{
		bitset<1, WordType> b1;
		bitset<1, WordType> b1A(1);

		EATEST_VERIFY(b1.size()       == 1);
		EATEST_VERIFY(b1.any()        == false);
		EATEST_VERIFY(b1.all()        == false);
		EATEST_VERIFY(b1.none()       == true);
		EATEST_VERIFY(b1.to_ulong_assert_convertible()   == 0);
		EATEST_VERIFY(b1A.any()       == true);
		EATEST_VERIFY(b1A.all()       == true);
		EATEST_VERIFY(b1A.none()      == false);
		EATEST_VERIFY(b1A.to_ulong_assert_convertible()  == 1);
		EATEST_VERIFY(b1A.to_uint32_assert_convertible() == 1);
		EATEST_VERIFY(b1A.to_uint64_assert_convertible() == 1);


		bitset<33, WordType> b33;
		bitset<33, WordType> b33A(1);

		EATEST_VERIFY(b33.size()      == 33);
		EATEST_VERIFY(b33.any()       == false);
		EATEST_VERIFY(b33.all()       == false);
		EATEST_VERIFY(b33.none()      == true);
		EATEST_VERIFY(b33.to_ulong_assert_convertible()  == 0);
		EATEST_VERIFY(b33A.any()      == true);
		EATEST_VERIFY(b33A.all()      == false);
		EATEST_VERIFY(b33A.none()     == false);
		EATEST_VERIFY(b33A.to_ulong_assert_convertible() == 1);


		bitset<65, WordType> b65;
		bitset<65, WordType> b65A(1);

		EATEST_VERIFY(b65.size()      == 65);
		EATEST_VERIFY(b65.any()       == false);
		EATEST_VERIFY(b65.all()       == false);
		EATEST_VERIFY(b65.none()      == true);
		EATEST_VERIFY(b65.to_ulong_assert_convertible()  == 0);
		EATEST_VERIFY(b65A.any()      == true);
		EATEST_VERIFY(b65A.all()      == false);
		EATEST_VERIFY(b65A.none()     == false);
		EATEST_VERIFY(b65A.to_ulong_assert_convertible() == 1);


		bitset<129, WordType> b129;
		bitset<129, WordType> b129A(1);

		EATEST_VERIFY(b129.size()      == 129);
		EATEST_VERIFY(b129.any()       == false);
		EATEST_VERIFY(b129.all()       == false);
		EATEST_VERIFY(b129.none()      == true);
		EATEST_VERIFY(b129.to_ulong_assert_convertible()  == 0);
		EATEST_VERIFY(b129A.any()      == true);
		EATEST_VERIFY(b129A.all()      == false);
		EATEST_VERIFY(b129A.none()     == false);
		EATEST_VERIFY(b129A.to_ulong_assert_convertible() == 1);


		// operator[], data, test, to_ulong, count
		b1[0]   = true;
		EATEST_VERIFY(b1.test(0)   == true);
		EATEST_VERIFY(b1.count()   == 1);

		b33[0]  = true;
		b33[32] = true;
		EATEST_VERIFY(b33.test(0)  == true);
		EATEST_VERIFY(b33.test(15) == false);
		EATEST_VERIFY(b33.test(32) == true);
		EATEST_VERIFY(b33.count()  == 2);

		b65[0]  = true;
		b65[32] = true;
		b65[64] = true;
		EATEST_VERIFY(b65.test(0)  == true);
		EATEST_VERIFY(b65.test(15) == false);
		EATEST_VERIFY(b65.test(32) == true);
		EATEST_VERIFY(b65.test(47) == false);
		EATEST_VERIFY(b65.test(64) == true);
		EATEST_VERIFY(b65.count()  == 3);

		b129[0]   = true;
		b129[32]  = true;
		b129[64]  = true;
		b129[128] = true;
		EATEST_VERIFY(b129.test(0)   == true);
		EATEST_VERIFY(b129.test(15)  == false);
		EATEST_VERIFY(b129.test(32)  == true);
		EATEST_VERIFY(b129.test(47)  == false);
		EATEST_VERIFY(b129.test(64)  == true);
		EATEST_VERIFY(b129.test(91)  == false);
		EATEST_VERIFY(b129.test(128) == true);
		EATEST_VERIFY(b129.count()   == 4);

		typename bitset<1, WordType>::word_type* pWordArray;

		pWordArray = b1.data();
		EATEST_VERIFY(pWordArray != NULL);
		pWordArray = b33.data();
		EATEST_VERIFY(pWordArray != NULL);
		pWordArray = b65.data();
		EATEST_VERIFY(pWordArray != NULL);
		pWordArray = b129.data();
		EATEST_VERIFY(pWordArray != NULL);


		// bitset<1, WordType> set, reset, flip, ~
		b1.reset();
		EATEST_VERIFY(b1.count() == 0);

		b1.set();
		EATEST_VERIFY(b1.count() == b1.size());
		EATEST_VERIFY(b1.all());

		b1.flip();
		EATEST_VERIFY(b1.count() == 0);
		EATEST_VERIFY(!b1.all());
		EATEST_VERIFY(b1.none());

		b1.set(0, true);
		EATEST_VERIFY(b1[0] == true);

		b1.reset(0);
		EATEST_VERIFY(b1[0] == false);

		b1.flip(0);
		EATEST_VERIFY(b1[0] == true);

		bitset<1, WordType> b1Not = ~b1;
		EATEST_VERIFY(b1[0]    == true);
		EATEST_VERIFY(b1Not[0] == false);


		// bitset<33, WordType> set, reset, flip, ~
		b33.reset();
		EATEST_VERIFY(b33.count() == 0);

		b33.set();
		EATEST_VERIFY(b33.count() == b33.size());
		EATEST_VERIFY(b33.all());


		b33.flip();
		EATEST_VERIFY(b33.count() == 0);
		EATEST_VERIFY(!b33.all());

		b33.set(0, true);
		b33.set(32, true);
		EATEST_VERIFY(b33[0]  == true);
		EATEST_VERIFY(b33[15] == false);
		EATEST_VERIFY(b33[32] == true);

		b33.reset(0);
		b33.reset(32);
		EATEST_VERIFY(b33[0]  == false);
		EATEST_VERIFY(b33[32] == false);

		b33.flip(0);
		b33.flip(32);
		EATEST_VERIFY(b33[0]  == true);
		EATEST_VERIFY(b33[32] == true);

		bitset<33, WordType> b33Not(~b33);
		EATEST_VERIFY(b33[0]     == true);
		EATEST_VERIFY(b33[32]    == true);
		EATEST_VERIFY(b33Not[0]  == false);
		EATEST_VERIFY(b33Not[32] == false);


		// bitset<65, WordType> set, reset, flip, ~
		b65.reset();
		EATEST_VERIFY(b65.count() == 0);
		EATEST_VERIFY(!b65.all());
		EATEST_VERIFY(b65.none());

		b65.set();
		EATEST_VERIFY(b65.count() == b65.size());
		EATEST_VERIFY(b65.all());
		EATEST_VERIFY(!b65.none());

		b65.flip();
		EATEST_VERIFY(b65.count() == 0);
		EATEST_VERIFY(!b65.all());
		EATEST_VERIFY(b65.none());


		b65.set(0, true);
		b65.set(32, true);
		b65.set(64, true);
		EATEST_VERIFY(b65[0]  == true);
		EATEST_VERIFY(b65[15] == false);
		EATEST_VERIFY(b65[32] == true);
		EATEST_VERIFY(b65[50] == false);
		EATEST_VERIFY(b65[64] == true);

		b65.reset(0);
		b65.reset(32);
		b65.reset(64);
		EATEST_VERIFY(b65[0]  == false);
		EATEST_VERIFY(b65[32] == false);
		EATEST_VERIFY(b65[64] == false);

		b65.flip(0);
		b65.flip(32);
		b65.flip(64);
		EATEST_VERIFY(b65[0]  == true);
		EATEST_VERIFY(b65[32] == true);
		EATEST_VERIFY(b65[64] == true);

		bitset<65, WordType> b65Not(~b65);
		EATEST_VERIFY(b65[0]     == true);
		EATEST_VERIFY(b65[32]    == true);
		EATEST_VERIFY(b65[64]    == true);
		EATEST_VERIFY(b65Not[0]  == false);
		EATEST_VERIFY(b65Not[32] == false);
		EATEST_VERIFY(b65Not[64] == false);


		// bitset<65, WordType> set, reset, flip, ~
		b129.reset();
		EATEST_VERIFY(b129.count() == 0);

		b129.set();
		EATEST_VERIFY(b129.count() == b129.size());
		EATEST_VERIFY(b129.all());

		b129.flip();
		EATEST_VERIFY(b129.count() == 0);
		EATEST_VERIFY(!b129.all());
		EATEST_VERIFY(b129.none());

		b129.set(0, true);
		b129.set(32, true);
		b129.set(64, true);
		b129.set(128, true);
		EATEST_VERIFY(b129[0]   == true);
		EATEST_VERIFY(b129[15]  == false);
		EATEST_VERIFY(b129[32]  == true);
		EATEST_VERIFY(b129[50]  == false);
		EATEST_VERIFY(b129[64]  == true);
		EATEST_VERIFY(b129[90]  == false);
		EATEST_VERIFY(b129[128] == true);

		b129.reset(0);
		b129.reset(32);
		b129.reset(64);
		b129.reset(128);
		EATEST_VERIFY(b129[0]   == false);
		EATEST_VERIFY(b129[32]  == false);
		EATEST_VERIFY(b129[64]  == false);
		EATEST_VERIFY(b129[128] == false);

		b129.flip(0);
		b129.flip(32);
		b129.flip(64);
		b129.flip(128);
		EATEST_VERIFY(b129[0]   == true);
		EATEST_VERIFY(b129[32]  == true);
		EATEST_VERIFY(b129[64]  == true);
		EATEST_VERIFY(b129[128] == true);

		bitset<129, WordType> b129Not(~b129);
		EATEST_VERIFY(b129[0]      == true);
		EATEST_VERIFY(b129[32]     == true);
		EATEST_VERIFY(b129[64]     == true);
		EATEST_VERIFY(b129[128]    == true);
		EATEST_VERIFY(b129Not[0]   == false);
		EATEST_VERIFY(b129Not[32]  == false);
		EATEST_VERIFY(b129Not[64]  == false);
		EATEST_VERIFY(b129Not[128] == false);


		// operator ==, !=
		bitset<1, WordType> b1Equal(b1);
		EATEST_VERIFY(b1Equal == b1);
		EATEST_VERIFY(b1Equal != b1Not);

		bitset<33, WordType> b33Equal(b33);
		EATEST_VERIFY(b33Equal == b33);
		EATEST_VERIFY(b33Equal != b33Not);

		bitset<65, WordType> b65Equal(b65);
		EATEST_VERIFY(b65Equal == b65);
		EATEST_VERIFY(b65Equal != b65Not);

		bitset<129, WordType> b129Equal(b129);
		EATEST_VERIFY(b129Equal == b129);
		EATEST_VERIFY(b129Equal != b129Not);


		// bitset<1, WordType> operator<<=, operator>>=, operator<<, operator>>
		b1.reset();

		b1[0] = true;
		b1 >>= 0;
		EATEST_VERIFY(b1[0] == true);
		b1 >>= 1;
		EATEST_VERIFY(b1[0] == false);

		b1[0] = true;
		b1 <<= 0;
		EATEST_VERIFY(b1[0] == true);
		b1 <<= 1;
		EATEST_VERIFY(b1[0] == false);

		b1[0] = true;
		b1Equal = b1 >> 0;
		EATEST_VERIFY(b1Equal == b1);
		b1Equal = b1 >> 1;
		EATEST_VERIFY(b1Equal[0] == false);

		b1[0] = true;
		b1Equal = b1 << 0;
		EATEST_VERIFY(b1Equal[0] == true);
		b1Equal = b1 << 1;
		EATEST_VERIFY(b1Equal[0] == false);

		b1.reset();
		b1.flip();
		b1 >>= 33;
		EATEST_VERIFY(b1.count() == 0);
		EATEST_VERIFY(!b1.all());
		EATEST_VERIFY(b1.none());

		b1.reset();
		b1.flip();
		b1 <<= 33;
		EATEST_VERIFY(b1.count() == 0);
		EATEST_VERIFY(!b1.all());
		EATEST_VERIFY(b1.none());

		b1.reset();
		b1.flip();
		b1 >>= 65;
		EATEST_VERIFY(b1.count() == 0);
		EATEST_VERIFY(!b1.all());
		EATEST_VERIFY(b1.none());

		b1.reset();
		b1.flip();
		b1 <<= 65;
		EATEST_VERIFY(b1.count() == 0);
		EATEST_VERIFY(!b1.all());
		EATEST_VERIFY(b1.none());


		// bitset<33, WordType> operator<<=, operator>>=, operator<<, operator>>
		b33.reset();

		b33[0]  = true;
		b33[32] = true;
		b33 >>= 0;
		EATEST_VERIFY(b33[0]  == true);
		EATEST_VERIFY(b33[32] == true);
		b33 >>= 10;
		EATEST_VERIFY(b33[22] == true);

		b33.reset();
		b33[0]  = true;
		b33[32] = true;
		b33 <<= 0;
		EATEST_VERIFY(b33[0] == true);
		EATEST_VERIFY(b33[32] == true);
		b33 <<= 10;
		EATEST_VERIFY(b33[10] == true);

		b33.reset();
		b33[0]  = true;
		b33[32] = true;
		b33Equal = b33 >> 0;
		EATEST_VERIFY(b33Equal == b33);
		b33Equal = b33 >> 10;
		EATEST_VERIFY(b33Equal[22] == true);

		b33.reset();
		b33[0]  = true;
		b33[32] = true;
		b33Equal = b33 << 10;
		EATEST_VERIFY(b33Equal[10] == true);

		b33.reset();
		b33.flip();
		b33 >>= 33;
		EATEST_VERIFY(b33.count() == 0);
		EATEST_VERIFY(!b33.all());
		EATEST_VERIFY(b33.none());

		b33.reset();
		b33.flip();
		b33 <<= 33;
		EATEST_VERIFY(b33.count() == 0);
		EATEST_VERIFY(!b33.all());
		EATEST_VERIFY(b33.none());

		b33.reset();
		b33.flip();
		b33 >>= 65;
		EATEST_VERIFY(b33.count() == 0);
		EATEST_VERIFY(!b33.all());
		EATEST_VERIFY(b33.none());

		b33.reset();
		b33.flip();
		b33 <<= 65;
		EATEST_VERIFY(b33.count() == 0);
		EATEST_VERIFY(!b33.all());
		EATEST_VERIFY(b33.none());


		// bitset<65, WordType> operator<<=, operator>>=, operator<<, operator>>
		b65.reset();

		b65[0]  = true;
		b65[32] = true;
		b65[64] = true;
		b65 >>= 0;
		EATEST_VERIFY(b65[0]  == true);
		EATEST_VERIFY(b65[32] == true);
		EATEST_VERIFY(b65[64] == true);
		b65 >>= 10;
		EATEST_VERIFY(b65[22] == true);
		EATEST_VERIFY(b65[54] == true);

		b65.reset();
		b65[0]  = true;
		b65[32] = true;
		b65[64] = true;
		b65 <<= 0;
		EATEST_VERIFY(b65[0] == true);
		EATEST_VERIFY(b65[32] == true);
		EATEST_VERIFY(b65[64] == true);
		b65 <<= 10;
		EATEST_VERIFY(b65[10] == true);
		EATEST_VERIFY(b65[42] == true);

		b65.reset();
		b65[0]  = true;
		b65[32] = true;
		b65[64] = true;
		b65Equal = b65 >> 0;
		EATEST_VERIFY(b65Equal == b65);
		b65Equal = b65 >> 10;
		EATEST_VERIFY(b65Equal[22] == true);
		EATEST_VERIFY(b65Equal[54] == true);

		b65.reset();
		b65[0]  = true;
		b65[32] = true;
		b65[64] = true;
		b65Equal = b65 << 10;
		EATEST_VERIFY(b65Equal[10] == true);
		EATEST_VERIFY(b65Equal[42] == true);

		b65.reset();
		b65.flip();
		b65 >>= 33;
		EATEST_VERIFY(b65.count() == 32);

		b65.reset();
		b65.flip();
		b65 <<= 33;
		EATEST_VERIFY(b65.count() == 32);

		b65.reset();
		b65.flip();
		b65 >>= 65;
		EATEST_VERIFY(b65.count() == 0);

		b65.reset();
		b65.flip();
		b65 <<= 65;
		EATEST_VERIFY(b65.count() == 0);


		// bitset<129, WordType> operator<<=, operator>>=, operator<<, operator>>
		b129.reset();

		b129[0]  = true;
		b129[32] = true;
		b129[64] = true;
		b129[128] = true;
		b129 >>= 0;
		EATEST_VERIFY(b129[0]  == true);
		EATEST_VERIFY(b129[32] == true);
		EATEST_VERIFY(b129[64] == true);
		EATEST_VERIFY(b129[128] == true);
		b129 >>= 10;
		EATEST_VERIFY(b129[22] == true);
		EATEST_VERIFY(b129[54] == true);
		EATEST_VERIFY(b129[118] == true);

		b129.reset();
		b129[0]  = true;
		b129[32] = true;
		b129[64] = true;
		b129[128] = true;
		b129 <<= 0;
		EATEST_VERIFY(b129[0] == true);
		EATEST_VERIFY(b129[32] == true);
		EATEST_VERIFY(b129[64] == true);
		EATEST_VERIFY(b129[128] == true);
		b129 <<= 10;
		EATEST_VERIFY(b129[10] == true);
		EATEST_VERIFY(b129[42] == true);
		EATEST_VERIFY(b129[74] == true);

		b129.reset();
		b129[0]  = true;
		b129[32] = true;
		b129[64] = true;
		b129[128] = true;
		b129Equal = b129 >> 0;
		EATEST_VERIFY(b129Equal == b129);
		b129Equal = b129 >> 10;
		EATEST_VERIFY(b129Equal[22] == true);
		EATEST_VERIFY(b129Equal[54] == true);
		EATEST_VERIFY(b129Equal[118] == true);

		b129.reset();
		b129[0]  = true;
		b129[32] = true;
		b129[64] = true;
		b129[128] = true;
		b129Equal = b129 << 10;
		EATEST_VERIFY(b129Equal[10] == true);
		EATEST_VERIFY(b129Equal[42] == true);
		EATEST_VERIFY(b129Equal[74] == true);

		b129.reset();
		b129.flip();
		b129 >>= 33;
		EATEST_VERIFY(b129.count() == 96);

		b129.reset();
		b129.flip();
		b129 <<= 33;
		EATEST_VERIFY(b129.count() == 96);

		b129.reset();
		b129.flip();
		b129 >>= 65;
		EATEST_VERIFY(b129.count() == 64);

		b129.reset();
		b129.flip();
		b129 <<= 65;
		EATEST_VERIFY(b129.count() == 64);


		// operator&=(const this_type& x), operator|=(const this_type& x), operator^=(const this_type& x)
		b1.set();
		b1[0]  = false;
		b1A[0] = true;
		b1 &= b1A;
		EATEST_VERIFY(b1[0] == false);
		b1 |= b1A;
		EATEST_VERIFY(b1[0] == true);
		b1 ^= b1A;
		EATEST_VERIFY(b1[0] == false);
		b1 |= b1A;
		EATEST_VERIFY(b1[0] == true);

		b33.set();
		b33[0]   = false;
		b33[32]  = false;
		b33A[0]  = true;
		b33A[32] = true;
		b33 &= b33A;
		EATEST_VERIFY((b33[0] == false) && (b33[32] == false));
		b33 |= b33A;
		EATEST_VERIFY((b33[0] == true) && (b33[32] == true));
		b33 ^= b33A;
		EATEST_VERIFY((b33[0] == false) && (b33[32] == false));
		b33 |= b33A;
		EATEST_VERIFY((b33[0] == true) && (b33[32] == true));

		b65.set();
		b65[0]   = false;
		b65[32]  = false;
		b65[64]  = false;
		b65A[0]  = true;
		b65A[32] = true;
		b65A[64] = true;
		b65 &= b65A;
		EATEST_VERIFY((b65[0] == false) && (b65[32] == false) && (b65[64] == false));
		b65 |= b65A;
		EATEST_VERIFY((b65[0] == true) && (b65[32] == true) && (b65[64] == true));
		b65 ^= b65A;
		EATEST_VERIFY((b65[0] == false) && (b65[32] == false) && (b65[64] == false));
		b65 |= b65A;
		EATEST_VERIFY((b65[0] == true) && (b65[32] == true) && (b65[64] == true));

		b129.set();
		b129[0]    = false;
		b129[32]   = false;
		b129[64]   = false;
		b129[128]  = false;
		b129A[0]   = true;
		b129A[32]  = true;
		b129A[64]  = true;
		b129A[128] = true;
		b129 &= b129A;
		EATEST_VERIFY((b129[0] == false) && (b129[32] == false) && (b129[64] == false) && (b129[128] == false));
		b129 |= b129A;
		EATEST_VERIFY((b129[0] == true) && (b129[32] == true) && (b129[64] == true) && (b129[128] == true));
		b129 ^= b129A;
		EATEST_VERIFY((b129[0] == false) && (b129[32] == false) && (b129[64] == false) && (b129[128] == false));
		b129 |= b129A;
		EATEST_VERIFY((b129[0] == true) && (b129[32] == true) && (b129[64] == true) && (b129[128] == true));
	}

	{ // Test bitset::reference
		bitset<65, WordType> b65;
		typename bitset<65, WordType>::reference r = b65[33];

		r = true;
		EATEST_VERIFY(r == true);
	}

	{ // Test find_first, find_next
		size_t i, j;

		// bitset<1, WordType>
		bitset<1, WordType>   b1;

		i = b1.find_first();
		EATEST_VERIFY(i == b1.kSize);  
		b1.set(0, true);
		i = b1.find_first();
		EATEST_VERIFY(i == 0);
		i = b1.find_next(i);
		EATEST_VERIFY(i ==  b1.kSize);  

		b1.set();
		for(i = 0, j = b1.find_first(); j != b1.kSize; j = b1.find_next(j))
			++i;
		EATEST_VERIFY(i == 1);

		// bitset<7, WordType>
		bitset<7, WordType>   b7;

		i = b7.find_first();
		EATEST_VERIFY(i == b7.kSize);  
		b7.set(0, true);
		b7.set(5, true);
		i = b7.find_first();
		EATEST_VERIFY(i == 0);
		i = b7.find_next(i);
		EATEST_VERIFY(i == 5);
		i = b7.find_next(i);
		EATEST_VERIFY(i ==  b7.kSize);  

		b7.set();
		for(i = 0, j = b7.find_first(); j != b7.kSize; j = b7.find_next(j))
			++i;
		EATEST_VERIFY(i == 7);

		// bitset<32, WordType>
		bitset<32, WordType>  b32;

		i = b32.find_first();
		EATEST_VERIFY(i == b32.kSize);  
		b32.set(0, true);
		b32.set(27, true);
		i = b32.find_first();
		EATEST_VERIFY(i == 0);
		i = b32.find_next(i);
		EATEST_VERIFY(i == 27);
		i = b32.find_next(i);
		EATEST_VERIFY(i == b32.kSize);  

		b32.set();
		for(i = 0, j = b32.find_first(); j != b32.kSize; j = b32.find_next(j))
			++i;
		EATEST_VERIFY(i == 32);

		// bitset<41, WordType>
		bitset<41, WordType>  b41;

		i = b41.find_first();
		EATEST_VERIFY(i == b41.kSize);  
		b41.set(0, true);
		b41.set(27, true);
		b41.set(37, true);
		i = b41.find_first();
		EATEST_VERIFY(i == 0);
		i = b41.find_next(i);
		EATEST_VERIFY(i == 27);
		i = b41.find_next(i);
		EATEST_VERIFY(i == 37);
		i = b41.find_next(i);
		EATEST_VERIFY(i == b41.kSize);  

		b41.set();
		for(i = 0, j = b41.find_first(); j != b41.kSize; j = b41.find_next(j))
			++i;
		EATEST_VERIFY(i == 41);

		// bitset<64, WordType>
		bitset<64, WordType>  b64;

		i = b64.find_first();
		EATEST_VERIFY(i == b64.kSize);  
		b64.set(0, true);
		b64.set(27, true);
		b64.set(37, true);
		i = b64.find_first();
		EATEST_VERIFY(i == 0);
		i = b64.find_next(i);
		EATEST_VERIFY(i == 27);
		i = b64.find_next(i);
		EATEST_VERIFY(i == 37);
		i = b64.find_next(i);
		EATEST_VERIFY(i == b64.kSize);  

		b64.set();
		for(i = 0, j = b64.find_first(); j != b64.kSize; j = b64.find_next(j))
			++i;
		EATEST_VERIFY(i == 64);

		// bitset<79, WordType>
		bitset<79, WordType>  b79;

		i = b79.find_first();
		EATEST_VERIFY(i == b79.kSize);  
		b79.set(0, true);
		b79.set(27, true);
		b79.set(37, true);
		i = b79.find_first();
		EATEST_VERIFY(i == 0);
		i = b79.find_next(i);
		EATEST_VERIFY(i == 27);
		i = b79.find_next(i);
		EATEST_VERIFY(i == 37);
		i = b79.find_next(i);
		EATEST_VERIFY(i == b79.kSize);  

		b79.set();
		for(i = 0, j = b79.find_first(); j != b79.kSize; j = b79.find_next(j))
			++i;
		EATEST_VERIFY(i == 79);

		// bitset<128, WordType>
		bitset<128, WordType> b128;

		i = b128.find_first();
		EATEST_VERIFY(i == b128.kSize);  
		b128.set(0, true);
		b128.set(27, true);
		b128.set(37, true);
		b128.set(77, true);
		i = b128.find_first();
		EATEST_VERIFY(i == 0);
		i = b128.find_next(i);
		EATEST_VERIFY(i == 27);
		i = b128.find_next(i);
		EATEST_VERIFY(i == 37);
		i = b128.find_next(i);
		EATEST_VERIFY(i == 77);
		i = b128.find_next(i);
		EATEST_VERIFY(i == b128.kSize);  

		b128.set();
		for(i = 0, j = b128.find_first(); j != b128.kSize; j = b128.find_next(j))
			++i;
		EATEST_VERIFY(i == 128);

		// bitset<137, WordType>
		bitset<137, WordType> b137;

		i = b137.find_first();
		EATEST_VERIFY(i == b137.kSize);  
		b137.set(0, true);
		b137.set(27, true);
		b137.set(37, true);
		b137.set(77, true);
		b137.set(99, true);
		b137.set(136, true);
		i = b137.find_first();
		EATEST_VERIFY(i == 0);
		i = b137.find_next(i);
		EATEST_VERIFY(i == 27);
		i = b137.find_next(i);
		EATEST_VERIFY(i == 37);
		i = b137.find_next(i);
		EATEST_VERIFY(i == 77);
		i = b137.find_next(i);
		EATEST_VERIFY(i == 99);
		i = b137.find_next(i);
		EATEST_VERIFY(i == 136);
		i = b137.find_next(i);
		EATEST_VERIFY(i == b137.kSize);

		b137.set();
		for(i = 0, j = b137.find_first(); j != b137.kSize; j = b137.find_next(j))
			++i;
		EATEST_VERIFY(i == 137);
	}

	{ // Test find_last, find_prev
		size_t i, j;

		// bitset<1, WordType>
		bitset<1, WordType>   b1;

		i = b1.find_last();
		EATEST_VERIFY(i == b1.kSize);  
		b1.set(0, true);
		i = b1.find_last();
		EATEST_VERIFY(i == 0);
		i = b1.find_prev(i);
		EATEST_VERIFY(i ==  b1.kSize);  

		b1.set();
		for(i = 0, j = b1.find_last(); j != b1.kSize; j = b1.find_prev(j))
			++i;
		EATEST_VERIFY(i == 1);

		// bitset<7, WordType>
		bitset<7, WordType>   b7;

		i = b7.find_last();
		EATEST_VERIFY(i == b7.kSize);  
		b7.set(0, true);
		b7.set(5, true);
		i = b7.find_last();
		EATEST_VERIFY(i == 5);
		i = b7.find_prev(i);
		EATEST_VERIFY(i == 0);
		i = b7.find_prev(i);
		EATEST_VERIFY(i ==  b7.kSize);  

		b7.set();
		for(i = 0, j = b7.find_last(); j != b7.kSize; j = b7.find_prev(j))
			++i;
		EATEST_VERIFY(i == 7);

		// bitset<32, WordType>
		bitset<32, WordType>  b32;

		i = b32.find_last();
		EATEST_VERIFY(i == b32.kSize);  
		b32.set(0, true);
		b32.set(27, true);
		i = b32.find_last();
		EATEST_VERIFY(i == 27);
		i = b32.find_prev(i);
		EATEST_VERIFY(i == 0);
		i = b32.find_prev(i);
		EATEST_VERIFY(i == b32.kSize);  

		b32.set();
		for(i = 0, j = b32.find_last(); j != b32.kSize; j = b32.find_prev(j))
			++i;
		EATEST_VERIFY(i == 32);

		// bitset<41, WordType>
		bitset<41, WordType>  b41;

		i = b41.find_last();
		EATEST_VERIFY(i == b41.kSize);  
		b41.set(0, true);
		b41.set(27, true);
		b41.set(37, true);
		i = b41.find_last();
		EATEST_VERIFY(i == 37);
		i = b41.find_prev(i);
		EATEST_VERIFY(i == 27);
		i = b41.find_prev(i);
		EATEST_VERIFY(i == 0);
		i = b41.find_prev(i);
		EATEST_VERIFY(i == b41.kSize);  

		b41.set();
		for(i = 0, j = b41.find_last(); j != b41.kSize; j = b41.find_prev(j))
			++i;
		EATEST_VERIFY(i == 41);

		// bitset<64, WordType>
		bitset<64, WordType>  b64;

		i = b64.find_last();
		EATEST_VERIFY(i == b64.kSize);  
		b64.set(0, true);
		b64.set(27, true);
		b64.set(37, true);
		i = b64.find_last();
		EATEST_VERIFY(i == 37);
		i = b64.find_prev(i);
		EATEST_VERIFY(i == 27);
		i = b64.find_prev(i);
		EATEST_VERIFY(i == 0);
		i = b64.find_prev(i);
		EATEST_VERIFY(i == b64.kSize);  

		verifyToUint32Throws(b64, 0x08000001);

		b64.set();
		for(i = 0, j = b64.find_last(); j != b64.kSize; j = b64.find_prev(j))
			++i;
		EATEST_VERIFY(i == 64);

		// bitset<79, WordType>
		bitset<79, WordType>  b79;

		i = b79.find_last();
		EATEST_VERIFY(i == b79.kSize);  
		b79.set(0, true);
		b79.set(27, true);
		b79.set(37, true);
		i = b79.find_last();
		EATEST_VERIFY(i == 37);
		i = b79.find_prev(i);
		EATEST_VERIFY(i == 27);
		i = b79.find_prev(i);
		EATEST_VERIFY(i == 0);
		i = b79.find_prev(i);
		EATEST_VERIFY(i == b79.kSize);  

		EATEST_VERIFY(b79.to_uint64_assert_convertible() == 0x0000002008000001);

		b79.set();
		for(i = 0, j = b79.find_last(); j != b79.kSize; j = b79.find_prev(j))
			++i;
		EATEST_VERIFY(i == 79);

		verifyToUint64Throws(b79, 0xffffffffffffffff);

		// bitset<128, WordType>
		bitset<128, WordType> b128;

		i = b128.find_last();
		EATEST_VERIFY(i == b128.kSize);  
		b128.set(0, true);
		b128.set(27, true);
		b128.set(37, true);
		b128.set(77, true);
		i = b128.find_last();
		EATEST_VERIFY(i == 77);
		i = b128.find_prev(i);
		EATEST_VERIFY(i == 37);
		i = b128.find_prev(i);
		EATEST_VERIFY(i == 27);
		i = b128.find_prev(i);
		EATEST_VERIFY(i == 0);
		i = b128.find_prev(i);
		EATEST_VERIFY(i == b128.kSize);  

		verifyToUint64Throws(b128, 0x0000002008000001);

		b128.set();
		for(i = 0, j = b128.find_last(); j != b128.kSize; j = b128.find_prev(j))
			++i;
		EATEST_VERIFY(i == 128);

		// bitset<137, WordType>
		bitset<137, WordType> b137;

		i = b137.find_last();
		EATEST_VERIFY(i == b137.kSize);  
		b137.set(0, true);
		b137.set(27, true);
		b137.set(37, true);
		b137.set(77, true);
		b137.set(99, true);
		b137.set(136, true);
		i = b137.find_last();
		EATEST_VERIFY(i == 136);
		i = b137.find_prev(i);
		EATEST_VERIFY(i == 99);
		i = b137.find_prev(i);
		EATEST_VERIFY(i == 77);
		i = b137.find_prev(i);
		EATEST_VERIFY(i == 37);
		i = b137.find_prev(i);
		EATEST_VERIFY(i == 27);
		i = b137.find_prev(i);
		EATEST_VERIFY(i == 0);
		i = b137.find_prev(i);
		EATEST_VERIFY(i == b137.kSize);

		b137.set();
		for(i = 0, j = b137.find_last(); j != b137.kSize; j = b137.find_prev(j))
			++i;
		EATEST_VERIFY(i == 137);

		// bitset<99, WordType>
		bitset<99, WordType> b99;
		b99.set(63);
		verifyToUint32Throws(b99, 0x0);
		EATEST_VERIFY(b99.to_uint64_assert_convertible() == 0x8000000000000000);
		verifyToUlongThrowsIf32bit(b99, static_cast<unsigned long>(0x8000000000000000ul));
	}

	// GetFirstBit
	{
		nErrorCount += TestGetFirstBit<uint8_t>();
		nErrorCount += TestGetFirstBit<uint16_t>();
		nErrorCount += TestGetFirstBit<uint32_t>();
		nErrorCount += TestGetFirstBit<uint64_t>();
#if EASTL_INT128_SUPPORTED
		nErrorCount += TestGetFirstBit<eastl_uint128_t>();
#endif
	}

	// GetLastBit
	{
		nErrorCount += TestGetLastBit<uint8_t>();
		nErrorCount += TestGetLastBit<uint16_t>();
		nErrorCount += TestGetLastBit<uint32_t>();
		nErrorCount += TestGetLastBit<uint64_t>();
#if EASTL_INT128_SUPPORTED
		nErrorCount += TestGetLastBit<eastl_uint128_t>();
#endif
	}

	return nErrorCount;
}

int TestBitset()
{
	int nErrorCount = 0;

	nErrorCount += TestBitsetWithWord<EASTL_BITSET_WORD_TYPE_DEFAULT>();

	nErrorCount += TestBitsetWithWord<uint8_t>();
	nErrorCount += TestBitsetWithWord<uint16_t>();
	nErrorCount += TestBitsetWithWord<uint32_t>();
	nErrorCount += TestBitsetWithWord<uint64_t>();

	nErrorCount += TestBitsetWithWord<unsigned char>();
	nErrorCount += TestBitsetWithWord<unsigned short>();
	nErrorCount += TestBitsetWithWord<unsigned int>();
	nErrorCount += TestBitsetWithWord<unsigned long>();
	nErrorCount += TestBitsetWithWord<unsigned long long>();

	// test BITSET_WORD_COUNT macro
	{
		{
			typedef eastl::bitset<32, unsigned char> bitset_t;
			static_assert(bitset_t::kWordCount == BITSET_WORD_COUNT(bitset_t::kSize, bitset_t::word_type), "bitset failure");
		}
		{
			typedef eastl::bitset<32, unsigned int> bitset_t;
			static_assert(bitset_t::kWordCount == BITSET_WORD_COUNT(bitset_t::kSize, bitset_t::word_type), "bitset failure");
		}
		{
			typedef eastl::bitset<32, uint16_t> bitset_t;
			static_assert(bitset_t::kWordCount == BITSET_WORD_COUNT(bitset_t::kSize, bitset_t::word_type), "bitset failure");
		}
		{
			typedef eastl::bitset<32, uint32_t> bitset_t;
			static_assert(bitset_t::kWordCount == BITSET_WORD_COUNT(bitset_t::kSize, bitset_t::word_type), "bitset failure");
		}
		{
			typedef eastl::bitset<128, uint64_t> bitset_t;
			static_assert(bitset_t::kWordCount == BITSET_WORD_COUNT(bitset_t::kSize, bitset_t::word_type), "bitset failure");
		}
		{
			typedef eastl::bitset<256, uint64_t> bitset_t;
			static_assert(bitset_t::kWordCount == BITSET_WORD_COUNT(bitset_t::kSize, bitset_t::word_type), "bitset failure");
		}
	}

	return nErrorCount;
}
