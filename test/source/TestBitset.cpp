/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EASTLTest.h"
#include <EASTL/bitset.h>
#include <EABase/eabase.h>

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


int TestBitset()
{
	int nErrorCount = 0;

	{
		// bitset<0> tests
		#if !defined(__GNUC__) || (__GNUC__ >= 3) // GCC before v3.0 can't handle our bitset<0>.
			bitset<0> b0(0x10101010);
			EATEST_VERIFY(b0.count() == 0);
			EATEST_VERIFY(b0.to_ulong()  == 0x00000000);
			EATEST_VERIFY(b0.to_uint32() == 0x00000000);
			EATEST_VERIFY(b0.to_uint64() == 0x00000000);

			b0.flip();
			EATEST_VERIFY(b0.count() == 0);
			EATEST_VERIFY(b0.to_ulong()  == 0x00000000);
			EATEST_VERIFY(b0.to_uint32() == 0x00000000);
			EATEST_VERIFY(b0.to_uint64() == 0x00000000);

			b0 <<= 1;
			EATEST_VERIFY(b0.count() == 0);
			EATEST_VERIFY(b0.to_ulong()  == 0x00000000);
			EATEST_VERIFY(b0.to_uint32() == 0x00000000);
			EATEST_VERIFY(b0.to_uint64() == 0x00000000);

			// Disabled because currently bitset<0> instances can't be modified without triggering asserts.
			//b0.from_uint32(0x10101010);
			//EATEST_VERIFY(b0.to_uint32() == 0x00000000);
			//b0.from_uint64(UINT64_C(0x1010101010101010));
			//EATEST_VERIFY(b0.to_uint64() == UINT64_C(0x0000000000000000));
		#endif

		// bitset<8> tests
		bitset<8> b8(0x10101010);
		EATEST_VERIFY(b8.count() == 1);
		EATEST_VERIFY(b8.to_ulong()  == 0x00000010);
		EATEST_VERIFY(b8.to_uint32() == 0x00000010);
		EATEST_VERIFY(b8.to_uint64() == 0x00000010);

		b8.flip();
		EATEST_VERIFY(b8.count() == 7);
		EATEST_VERIFY(b8.to_ulong()  == 0x000000ef);
		EATEST_VERIFY(b8.to_uint32() == 0x000000ef);
		EATEST_VERIFY(b8.to_uint64() == 0x000000ef);

		b8 <<= 1;
		EATEST_VERIFY(b8.count() == 6);
		EATEST_VERIFY(b8.to_ulong()  == 0x000000de);
		EATEST_VERIFY(b8.to_uint32() == 0x000000de);
		EATEST_VERIFY(b8.to_uint64() == 0x000000de);

		b8.reset();
		b8.flip();
		b8 >>= 33;
		EATEST_VERIFY(b8.count() == 0);

		b8.reset();
		b8.flip();
		b8 >>= 65;
		EATEST_VERIFY(b8.count() == 0);

		b8.from_uint32(0x10101010);
		EATEST_VERIFY(b8.to_uint32() == 0x00000010);
		b8.from_uint64(UINT64_C(0x0000000000000010));
		EATEST_VERIFY(b8.to_uint64() == UINT64_C(0x0000000000000010));



		// bitset<16> tests
		bitset<16> b16(0x10101010);
		EATEST_VERIFY(b16.count() == 2);
		EATEST_VERIFY(b16.to_ulong()  == 0x00001010);
		EATEST_VERIFY(b16.to_uint32() == 0x00001010);
		EATEST_VERIFY(b16.to_uint64() == 0x00001010);

		b16.flip();
		EATEST_VERIFY(b16.count() == 14);
		EATEST_VERIFY(b16.to_ulong()  == 0x0000efef);
		EATEST_VERIFY(b16.to_uint32() == 0x0000efef);
		EATEST_VERIFY(b16.to_uint64() == 0x0000efef);

		b16 <<= 1;
		EATEST_VERIFY(b16.count() == 13);
		EATEST_VERIFY(b16.to_ulong()  == 0x0000dfde);
		EATEST_VERIFY(b16.to_uint32() == 0x0000dfde);
		EATEST_VERIFY(b16.to_uint64() == 0x0000dfde);

		b16.reset();
		b16.flip();
		b16 >>= 33;
		EATEST_VERIFY(b16.count() == 0);

		b16.reset();
		b16.flip();
		b16 >>= 65;
		EATEST_VERIFY(b16.count() == 0);

		b16.from_uint32(0x10101010);
		EATEST_VERIFY(b16.to_uint32() == 0x00001010);
		b16.from_uint64(UINT64_C(0x0000000000001010));
		EATEST_VERIFY(b16.to_uint64() == UINT64_C(0x0000000000001010));



		// bitset<32> tests
		bitset<32> b32(0x10101010);
		EATEST_VERIFY(b32.count() == 4);
		EATEST_VERIFY(b32.to_ulong()  == 0x10101010);
		EATEST_VERIFY(b32.to_uint32() == 0x10101010);
		EATEST_VERIFY(b32.to_uint64() == 0x10101010);

		b32.flip();
		EATEST_VERIFY(b32.count() == 28);
		EATEST_VERIFY(b32.to_ulong()  == 0xefefefef);
		EATEST_VERIFY(b32.to_uint32() == 0xefefefef);
		EATEST_VERIFY(b32.to_uint64() == 0xefefefef);

		b32 <<= 1;
		EATEST_VERIFY(b32.count() == 27);
		EATEST_VERIFY(b32.to_ulong()  == 0xdfdfdfde);
		EATEST_VERIFY(b32.to_uint32() == 0xdfdfdfde);
		EATEST_VERIFY(b32.to_uint64() == 0xdfdfdfde);

		b32.reset();
		b32.flip();
		b32 >>= 33;
		EATEST_VERIFY(b32.count() == 0);

		b32.reset();
		b32.flip();
		b32 >>= 65;
		EATEST_VERIFY(b32.count() == 0);

		b32.from_uint32(0x10101010);
		EATEST_VERIFY(b32.to_uint32() == 0x10101010);
		b32.from_uint64(UINT64_C(0x0000000010101010));
		EATEST_VERIFY(b32.to_uint64() == UINT64_C(0x0000000010101010));



		// bitset<64> tests
		bitset<64> b64(0x10101010);                                 // b64 => 00000000 00000000 00000000 00000000 00010000 00010000 00010000 00010000
		EATEST_VERIFY(b64.count() == 4);
		EATEST_VERIFY(b64.to_ulong()  == 0x10101010);
		EATEST_VERIFY(b64.to_uint32() == 0x10101010);
		EATEST_VERIFY(b64.to_uint64() == 0x10101010);

		b64.flip();                                                 // b64 => 11111111 11111111 11111111 11111111 11101111 11101111 11101111 11101111
		EATEST_VERIFY(b64.count() == 60);
		if(sizeof(unsigned long) + nErrorCount - nErrorCount == 4)  // We have this no-op math here in order to avoid compiler warnings about constant expressions.
		{
			#if EASTL_EXCEPTIONS_ENABLED
				try {
					EATEST_VERIFY(b64.to_ulong() == 0xefefefef);
					EATEST_VERIFY(false);
				}
				catch(std::overflow_error&)
				{
					EATEST_VERIFY(true);  // This pathway should be taken.
				}
				catch(...)
				{
					EATEST_VERIFY(false);
				}
			#else
				EATEST_VERIFY(b64.to_ulong() == 0xefefefef);
			#endif
		}
		else
		{
			EATEST_VERIFY(b64.to_ulong() == (unsigned long)UINT64_C(0xffffffffefefefef));
		}

		b64 <<= 1;                                                  // b64 => 11111111 11111111 11111111 11111111 11011111 11011111 11011111 11011110
		EATEST_VERIFY(b64.count() == 59);
		if(sizeof(unsigned long) + nErrorCount - nErrorCount == 4)
		{
			#if !EASTL_EXCEPTIONS_ENABLED
				EATEST_VERIFY(b64.to_ulong() == 0xdfdfdfde);
			#endif
		}
		else
		{
			EATEST_VERIFY(b64.to_ulong() == (unsigned long)UINT64_C(0xffffffffdfdfdfde));
		}

		b64.reset();                                                // b64 => 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
		EATEST_VERIFY(b64.count() == 0);
		EATEST_VERIFY(b64.to_ulong() == 0);

		b64 <<= 1;                                                  // b64 => 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
		EATEST_VERIFY(b64.count() == 0);
		EATEST_VERIFY(b64.to_ulong() == 0);

		b64.flip();                                                 // b64 => 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
		EATEST_VERIFY(b64.count() == 64);
		if(sizeof(unsigned long) + nErrorCount - nErrorCount == 4)
		{
			#if !EASTL_EXCEPTIONS_ENABLED
				EATEST_VERIFY(b64.to_ulong() == 0xffffffff);
			#endif
		}
		else
			EATEST_VERIFY(b64.to_ulong() == (unsigned long)UINT64_C(0xffffffffffffffff));

		b64 <<= 1;                                                  // b64 => 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111110
		EATEST_VERIFY(b64.count() == 63);
		if(sizeof(unsigned long) + nErrorCount - nErrorCount == 4)
		{
			#if !EASTL_EXCEPTIONS_ENABLED
				EATEST_VERIFY(b64.to_ulong() == 0xfffffffe);
			#endif
		}
		else
			EATEST_VERIFY(b64.to_ulong() == (unsigned long)UINT64_C(0xfffffffffffffffe));

		b64.reset();
		b64.flip();
		b64 >>= 33;
		EATEST_VERIFY(b64.count() == 31);

		b64.reset();
		b64.flip();
		b64 >>= 65;
		EATEST_VERIFY(b64.count() == 0);

		b64.from_uint32(0x10101010);
		EATEST_VERIFY(b64.to_uint32() == 0x10101010);
		b64.from_uint64(UINT64_C(0x1010101010101010));
		EATEST_VERIFY(b64.to_uint64() == UINT64_C(0x1010101010101010));
	}


	{
		bitset<1> b1;
		bitset<1> b1A(1);

		EATEST_VERIFY(b1.size()       == 1);
		EATEST_VERIFY(b1.any()        == false);
		EATEST_VERIFY(b1.all()        == false);
		EATEST_VERIFY(b1.none()       == true);
		EATEST_VERIFY(b1.to_ulong()   == 0);
		EATEST_VERIFY(b1A.any()       == true);
		EATEST_VERIFY(b1A.all()       == true);
		EATEST_VERIFY(b1A.none()      == false);
		EATEST_VERIFY(b1A.to_ulong()  == 1);
		EATEST_VERIFY(b1A.to_uint32() == 1);
		EATEST_VERIFY(b1A.to_uint64() == 1);


		bitset<33> b33;
		bitset<33> b33A(1);

		EATEST_VERIFY(b33.size()      == 33);
		EATEST_VERIFY(b33.any()       == false);
		EATEST_VERIFY(b33.all()       == false);
		EATEST_VERIFY(b33.none()      == true);
		EATEST_VERIFY(b33.to_ulong()  == 0);
		EATEST_VERIFY(b33A.any()      == true);
		EATEST_VERIFY(b33A.all()      == false);
		EATEST_VERIFY(b33A.none()     == false);
		EATEST_VERIFY(b33A.to_ulong() == 1);


		bitset<65> b65;
		bitset<65> b65A(1);

		EATEST_VERIFY(b65.size()      == 65);
		EATEST_VERIFY(b65.any()       == false);
		EATEST_VERIFY(b65.all()       == false);
		EATEST_VERIFY(b65.none()      == true);
		EATEST_VERIFY(b65.to_ulong()  == 0);
		EATEST_VERIFY(b65A.any()      == true);
		EATEST_VERIFY(b65A.all()      == false);
		EATEST_VERIFY(b65A.none()     == false);
		EATEST_VERIFY(b65A.to_ulong() == 1);


		bitset<129> b129;
		bitset<129> b129A(1);

		EATEST_VERIFY(b129.size()      == 129);
		EATEST_VERIFY(b129.any()       == false);
		EATEST_VERIFY(b129.all()       == false);
		EATEST_VERIFY(b129.none()      == true);
		EATEST_VERIFY(b129.to_ulong()  == 0);
		EATEST_VERIFY(b129A.any()      == true);
		EATEST_VERIFY(b129A.all()      == false);
		EATEST_VERIFY(b129A.none()     == false);
		EATEST_VERIFY(b129A.to_ulong() == 1);


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

		bitset<1>::word_type* pWordArray;

		pWordArray = b1.data();
		EATEST_VERIFY(pWordArray != NULL);
		pWordArray = b33.data();
		EATEST_VERIFY(pWordArray != NULL);
		pWordArray = b65.data();
		EATEST_VERIFY(pWordArray != NULL);
		pWordArray = b129.data();
		EATEST_VERIFY(pWordArray != NULL);


		// bitset<1> set, reset, flip, ~
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

		bitset<1> b1Not = ~b1;
		EATEST_VERIFY(b1[0]    == true);
		EATEST_VERIFY(b1Not[0] == false);


		// bitset<33> set, reset, flip, ~
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

		bitset<33> b33Not(~b33);
		EATEST_VERIFY(b33[0]     == true);
		EATEST_VERIFY(b33[32]    == true);
		EATEST_VERIFY(b33Not[0]  == false);
		EATEST_VERIFY(b33Not[32] == false);


		// bitset<65> set, reset, flip, ~
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

		bitset<65> b65Not(~b65);
		EATEST_VERIFY(b65[0]     == true);
		EATEST_VERIFY(b65[32]    == true);
		EATEST_VERIFY(b65[64]    == true);
		EATEST_VERIFY(b65Not[0]  == false);
		EATEST_VERIFY(b65Not[32] == false);
		EATEST_VERIFY(b65Not[64] == false);


		// bitset<65> set, reset, flip, ~
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

		bitset<129> b129Not(~b129);
		EATEST_VERIFY(b129[0]      == true);
		EATEST_VERIFY(b129[32]     == true);
		EATEST_VERIFY(b129[64]     == true);
		EATEST_VERIFY(b129[128]    == true);
		EATEST_VERIFY(b129Not[0]   == false);
		EATEST_VERIFY(b129Not[32]  == false);
		EATEST_VERIFY(b129Not[64]  == false);
		EATEST_VERIFY(b129Not[128] == false);


		// operator ==, !=
		bitset<1> b1Equal(b1);
		EATEST_VERIFY(b1Equal == b1);
		EATEST_VERIFY(b1Equal != b1Not);

		bitset<33> b33Equal(b33);
		EATEST_VERIFY(b33Equal == b33);
		EATEST_VERIFY(b33Equal != b33Not);

		bitset<65> b65Equal(b65);
		EATEST_VERIFY(b65Equal == b65);
		EATEST_VERIFY(b65Equal != b65Not);

		bitset<129> b129Equal(b129);
		EATEST_VERIFY(b129Equal == b129);
		EATEST_VERIFY(b129Equal != b129Not);


		// bitset<1> operator<<=, operator>>=, operator<<, operator>>
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


		// bitset<33> operator<<=, operator>>=, operator<<, operator>>
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


		// bitset<65> operator<<=, operator>>=, operator<<, operator>>
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


		// bitset<129> operator<<=, operator>>=, operator<<, operator>>
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
		bitset<65> b65;
		bitset<65>::reference r = b65[33];

		r = true;
		EATEST_VERIFY(r == true);
	}

	{ // Test find_first, find_next
		size_t i, j;

		// bitset<1>
		bitset<1>   b1;

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

		// bitset<7>
		bitset<7>   b7;

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

		// bitset<32>
		bitset<32>  b32;

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

		// bitset<41>
		bitset<41>  b41;

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

		// bitset<64>
		bitset<64>  b64;

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

		// bitset<79>
		bitset<79>  b79;

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

		// bitset<128>
		bitset<128> b128;

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

		// bitset<137>
		bitset<137> b137;

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

		// bitset<1>
		bitset<1>   b1;

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

		// bitset<7>
		bitset<7>   b7;

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

		// bitset<32>
		bitset<32>  b32;

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

		// bitset<41>
		bitset<41>  b41;

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

		// bitset<64>
		bitset<64>  b64;

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

		b64.set();
		for(i = 0, j = b64.find_last(); j != b64.kSize; j = b64.find_prev(j))
			++i;
		EATEST_VERIFY(i == 64);

		// bitset<79>
		bitset<79>  b79;

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

		b79.set();
		for(i = 0, j = b79.find_last(); j != b79.kSize; j = b79.find_prev(j))
			++i;
		EATEST_VERIFY(i == 79);

		// bitset<128>
		bitset<128> b128;

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

		b128.set();
		for(i = 0, j = b128.find_last(); j != b128.kSize; j = b128.find_prev(j))
			++i;
		EATEST_VERIFY(i == 128);

		// bitset<137>
		bitset<137> b137;

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
	}

	// test BITSET_WORD_COUNT macro
	{
		{
			typedef eastl::bitset<32, char> bitset_t;
			static_assert(bitset_t::kWordCount == BITSET_WORD_COUNT(bitset_t::kSize, bitset_t::word_type), "bitset failure");
		}
		{
			typedef eastl::bitset<32, int> bitset_t;
			static_assert(bitset_t::kWordCount == BITSET_WORD_COUNT(bitset_t::kSize, bitset_t::word_type), "bitset failure");
		}
		{
			typedef eastl::bitset<32, int16_t> bitset_t;
			static_assert(bitset_t::kWordCount == BITSET_WORD_COUNT(bitset_t::kSize, bitset_t::word_type), "bitset failure");
		}
		{
			typedef eastl::bitset<32, int32_t> bitset_t;
			static_assert(bitset_t::kWordCount == BITSET_WORD_COUNT(bitset_t::kSize, bitset_t::word_type), "bitset failure");
		}
		{
			typedef eastl::bitset<128, int64_t> bitset_t;
			static_assert(bitset_t::kWordCount == BITSET_WORD_COUNT(bitset_t::kSize, bitset_t::word_type), "bitset failure");
		}
		{
			typedef eastl::bitset<256, int64_t> bitset_t;
			static_assert(bitset_t::kWordCount == BITSET_WORD_COUNT(bitset_t::kSize, bitset_t::word_type), "bitset failure");
		}
	}

	return nErrorCount;
}



















