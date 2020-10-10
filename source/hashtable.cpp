///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EASTL/internal/hashtable.h>
#include <EASTL/utility.h>
#include <math.h>  // Not all compilers support <cmath> and std::ceilf(), which we need below.
#include <stddef.h>


EA_DISABLE_VC_WARNING(4267); // 'argument' : conversion from 'size_t' to 'const uint32_t', possible loss of data. This is a bogus warning resulting from a bug in VC++.


namespace eastl
{

	/// gpEmptyBucketArray
	///
	/// A shared representation of an empty hash table. This is present so that
	/// a new empty hashtable allocates no memory. It has two entries, one for 
	/// the first lone empty (NULL) bucket, and one for the non-NULL trailing sentinel.
	/// 
	EASTL_API void* gpEmptyBucketArray[2] = { NULL, (void*)uintptr_t(~0) };



	/// gPrimeNumberArray
	///
	/// This is an array of prime numbers. This is the same set of prime
	/// numbers suggested by the C++ standard proposal. These are numbers
	/// which are separated by 8% per entry.
	/// 
	/// To consider: Allow the user to specify their own prime number array.
	///
	const uint32_t gPrimeNumberArray[] =
	{
		2u, 3u, 5u, 7u, 11u, 13u, 17u, 19u, 23u, 29u, 31u,
		37u, 41u, 43u, 47u, 53u, 59u, 61u, 67u, 71u, 73u, 79u,
		83u, 89u, 97u, 103u, 109u, 113u, 127u, 137u, 139u, 149u,
		157u, 167u, 179u, 193u, 199u, 211u, 227u, 241u, 257u,
		277u, 293u, 313u, 337u, 359u, 383u, 409u, 439u, 467u,
		503u, 541u, 577u, 619u, 661u, 709u, 761u, 823u, 887u,
		953u, 1031u, 1109u, 1193u, 1289u, 1381u, 1493u, 1613u,
		1741u, 1879u, 2029u, 2179u, 2357u, 2549u, 2753u, 2971u,
		3209u, 3469u, 3739u, 4027u, 4349u, 4703u, 5087u, 5503u,
		5953u, 6427u, 6949u, 7517u, 8123u, 8783u, 9497u, 10273u,
		11113u, 12011u, 12983u, 14033u, 15173u, 16411u, 17749u,
		19183u, 20753u, 22447u, 24281u, 26267u, 28411u, 30727u,
		33223u, 35933u, 38873u, 42043u, 45481u, 49201u, 53201u,
		57557u, 62233u, 67307u, 72817u, 78779u, 85229u, 92203u,
		99733u, 107897u, 116731u, 126271u, 136607u, 147793u,
		159871u, 172933u, 187091u, 202409u, 218971u, 236897u,
		256279u, 277261u, 299951u, 324503u, 351061u, 379787u,
		410857u, 444487u, 480881u, 520241u, 562841u, 608903u,
		658753u, 712697u, 771049u, 834181u, 902483u, 976369u,
		1056323u, 1142821u, 1236397u, 1337629u, 1447153u, 1565659u,
		1693859u, 1832561u, 1982627u, 2144977u, 2320627u, 2510653u,
		2716249u, 2938679u, 3179303u, 3439651u, 3721303u, 4026031u,
		4355707u, 4712381u, 5098259u, 5515729u, 5967347u, 6456007u,
		6984629u, 7556579u, 8175383u, 8844859u, 9569143u, 10352717u,
		11200489u, 12117689u, 13109983u, 14183539u, 15345007u,
		16601593u, 17961079u, 19431899u, 21023161u, 22744717u,
		24607243u, 26622317u, 28802401u, 31160981u, 33712729u,
		36473443u, 39460231u, 42691603u, 46187573u, 49969847u,
		54061849u, 58488943u, 63278561u, 68460391u, 74066549u,
		80131819u, 86693767u, 93793069u, 101473717u, 109783337u,
		118773397u, 128499677u, 139022417u, 150406843u, 162723577u,
		176048909u, 190465427u, 206062531u, 222936881u, 241193053u,
		260944219u, 282312799u, 305431229u, 330442829u, 357502601u,
		386778277u, 418451333u, 452718089u, 489790921u, 529899637u,
		573292817u, 620239453u, 671030513u, 725980837u, 785430967u,
		849749479u, 919334987u, 994618837u, 1076067617u, 1164186217u,
		1259520799u, 1362662261u, 1474249943u, 1594975441u,
		1725587117u, 1866894511u, 2019773507u, 2185171673u,
		2364114217u, 2557710269u, 2767159799u, 2993761039u,
		3238918481u, 3504151727u, 3791104843u, 4101556399u,
		4294967291u,
		4294967291u // Sentinel so we don't have to test result of lower_bound
	};


	/// kPrimeCount
	///
	/// The number of prime numbers in gPrimeNumberArray.
	///
	const uint32_t kPrimeCount = (sizeof(gPrimeNumberArray) / sizeof(gPrimeNumberArray[0]) - 1);


	/// GetPrevBucketCountOnly
	/// Return a bucket count no greater than nBucketCountHint.
	///
	uint32_t prime_rehash_policy::GetPrevBucketCountOnly(uint32_t nBucketCountHint)
	{
		const uint32_t nPrime = *(eastl::upper_bound(gPrimeNumberArray, gPrimeNumberArray + kPrimeCount, nBucketCountHint) - 1);
		return nPrime;
	}


	/// GetPrevBucketCount
	/// Return a bucket count no greater than nBucketCountHint.
	/// This function has a side effect of updating mnNextResize.
	///
	uint32_t prime_rehash_policy::GetPrevBucketCount(uint32_t nBucketCountHint) const
	{
		const uint32_t nPrime = *(eastl::upper_bound(gPrimeNumberArray, gPrimeNumberArray + kPrimeCount, nBucketCountHint) - 1);

		mnNextResize = (uint32_t)ceilf(nPrime * mfMaxLoadFactor);
		return nPrime;
	}


	/// GetNextBucketCount
	/// Return a prime no smaller than nBucketCountHint.
	/// This function has a side effect of updating mnNextResize.
	///
	uint32_t prime_rehash_policy::GetNextBucketCount(uint32_t nBucketCountHint) const
	{
		const uint32_t nPrime = *eastl::lower_bound(gPrimeNumberArray, gPrimeNumberArray + kPrimeCount, nBucketCountHint);

		mnNextResize = (uint32_t)ceilf(nPrime * mfMaxLoadFactor);
		return nPrime;
	}


	/// GetBucketCount
	/// Return the smallest prime p such that alpha p >= nElementCount, where alpha 
	/// is the load factor. This function has a side effect of updating mnNextResize.
	///
	uint32_t prime_rehash_policy::GetBucketCount(uint32_t nElementCount) const
	{
		const uint32_t nMinBucketCount = (uint32_t)(nElementCount / mfMaxLoadFactor);
		const uint32_t nPrime          = *eastl::lower_bound(gPrimeNumberArray, gPrimeNumberArray + kPrimeCount, nMinBucketCount);

		mnNextResize = (uint32_t)ceilf(nPrime * mfMaxLoadFactor);
		return nPrime;
	}


	/// GetRehashRequired
	/// Finds the smallest prime p such that alpha p > nElementCount + nElementAdd.
	/// If p > nBucketCount, return pair<bool, uint32_t>(true, p); otherwise return
	/// pair<bool, uint32_t>(false, 0). In principle this isn't very different from GetBucketCount.
	/// This function has a side effect of updating mnNextResize.
	///
	eastl::pair<bool, uint32_t>
	prime_rehash_policy::GetRehashRequired(uint32_t nBucketCount, uint32_t nElementCount, uint32_t nElementAdd) const
	{
		if((nElementCount + nElementAdd) > mnNextResize) // It is significant that we specify > next resize and not >= next resize.
		{
			if(nBucketCount == 1) // We force rehashing to occur if the bucket count is < 2.
				nBucketCount = 0;

			float fMinBucketCount = (nElementCount + nElementAdd) / mfMaxLoadFactor;

			if(fMinBucketCount > (float)nBucketCount)
			{
				fMinBucketCount       = eastl::max_alt(fMinBucketCount, mfGrowthFactor * nBucketCount);
				const uint32_t nPrime = *eastl::lower_bound(gPrimeNumberArray, gPrimeNumberArray + kPrimeCount, (uint32_t)fMinBucketCount);
				mnNextResize          = (uint32_t)ceilf(nPrime * mfMaxLoadFactor);

				return eastl::pair<bool, uint32_t>(true, nPrime);
			}
			else
			{
				mnNextResize = (uint32_t)ceilf(nBucketCount * mfMaxLoadFactor);
				return eastl::pair<bool, uint32_t>(false, (uint32_t)0);
			}
		}

		return eastl::pair<bool, uint32_t>(false, (uint32_t)0);
	}


} // namespace eastl

EA_RESTORE_VC_WARNING();
