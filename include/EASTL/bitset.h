/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements a bitset much like the C++ std::bitset class. 
// The primary distinctions between this list and std::bitset are:
//    - bitset is more efficient than some other std::bitset implementations,
//      notably the bitset that comes with Microsoft and other 1st party platforms.
//    - bitset is savvy to an environment that doesn't have exception handling,
//      as is sometimes the case with console or embedded environments.
//    - bitset is savvy to environments in which 'unsigned long' is not the 
//      most efficient integral data type. std::bitset implementations use
//      unsigned long, even if it is an inefficient integer type.
//    - bitset removes as much function calls as practical, in order to allow
//      debug builds to run closer in speed and code footprint to release builds.
//    - bitset doesn't support string functionality. We can add this if 
//      it is deemed useful.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_BITSET_H
#define EASTL_BITSET_H


#include <EASTL/internal/config.h>
#include <EASTL/algorithm.h>

EA_DISABLE_ALL_VC_WARNINGS();

#include <stddef.h>
#include <string.h>

EA_RESTORE_ALL_VC_WARNINGS();

#if EASTL_EXCEPTIONS_ENABLED
	EA_DISABLE_ALL_VC_WARNINGS();

	#include <stdexcept> // std::out_of_range, std::length_error.

	EA_RESTORE_ALL_VC_WARNINGS();
#endif

EA_DISABLE_VC_WARNING(4127); // Conditional expression is constant

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{
	// To consider: Enable this for backwards compatibility with any user code that might be using BitsetWordType:
	// #define BitsetWordType EASTL_BITSET_WORD_TYPE_DEFAULT


	/// BITSET_WORD_COUNT
	///
	/// Defines the number of words we use, based on the number of bits.
	/// nBitCount refers to the number of bits in a bitset.
	/// WordType refers to the type of integer word which stores bitet data. By default it is BitsetWordType.
	///
	#if !defined(__GNUC__) || (__GNUC__ >= 3) // GCC 2.x can't handle the simpler declaration below.
		#define BITSET_WORD_COUNT(nBitCount, WordType) (nBitCount == 0 ? 1 : ((nBitCount - 1) / (8 * sizeof(WordType)) + 1))
	#else
		#define BITSET_WORD_COUNT(nBitCount, WordType) ((nBitCount - 1) / (8 * sizeof(WordType)) + 1)
	#endif


	/// EASTL_DISABLE_BITSET_ARRAYBOUNDS_WARNING
	/// Before GCC 4.7 the '-Warray-bounds' buggy and was very likely to issue false positives for loops that are
	/// difficult to evaluate.
	/// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=45978
	///
	#if defined(__GNUC__) && (EA_COMPILER_VERSION > 4007) && defined(EA_PLATFORM_ANDROID) // Earlier than GCC 4.7 
		#define EASTL_DISABLE_BITSET_ARRAYBOUNDS_WARNING 1
	#else
		#define EASTL_DISABLE_BITSET_ARRAYBOUNDS_WARNING 0
	#endif



	/// BitsetBase
	///
	/// This is a default implementation that works for any number of words.
	///
	template <size_t NW, typename WordType> // Templated on the number of words used to hold the bitset and the word type.
	struct BitsetBase
	{
		typedef WordType                 word_type;
		typedef BitsetBase<NW, WordType> this_type;
	  #if EASTL_BITSET_SIZE_T
		typedef size_t                   size_type;
	  #else
		typedef eastl_size_t             size_type;
	  #endif

		enum {
			kBitsPerWord      = (8 * sizeof(word_type)),
			kBitsPerWordMask  = (kBitsPerWord - 1),
			kBitsPerWordShift = ((kBitsPerWord == 8) ? 3 : ((kBitsPerWord == 16) ? 4 : ((kBitsPerWord == 32) ? 5 : (((kBitsPerWord == 64) ? 6 : 7)))))
		};

	public:
		word_type mWord[NW];

	public:
		BitsetBase();
		BitsetBase(uint32_t value); // This exists only for compatibility with std::bitset, which has a 'long' constructor.
	  //BitsetBase(uint64_t value); // Disabled because it causes conflicts with the 32 bit version with existing user code. Use from_uint64 to init from a uint64_t instead.

		void operator&=(const this_type& x);
		void operator|=(const this_type& x);
		void operator^=(const this_type& x);

		void operator<<=(size_type n);
		void operator>>=(size_type n);

		void flip();
		void set();
		void set(size_type i, bool value);
		void reset();

		bool operator==(const this_type& x) const;

		bool      any() const;
		size_type count() const;

		void          from_uint32(uint32_t value);
		void          from_uint64(uint64_t value);

		unsigned long to_ulong() const;
		uint32_t      to_uint32() const;
		uint64_t      to_uint64() const;

		word_type& DoGetWord(size_type i);
		word_type  DoGetWord(size_type i) const;

		size_type DoFindFirst() const;
		size_type DoFindNext(size_type last_find) const;

		size_type DoFindLast() const;                       // Returns NW * kBitsPerWord (the bit count) if no bits are set.
		size_type DoFindPrev(size_type last_find) const;    // Returns NW * kBitsPerWord (the bit count) if no bits are set.

	}; // class BitsetBase



	/// BitsetBase<1, WordType>
	/// 
	/// This is a specialization for a bitset that fits within one word.
	///
	template <typename WordType>
	struct BitsetBase<1, WordType>
	{
		typedef WordType                word_type;
		typedef BitsetBase<1, WordType> this_type;
	  #if EASTL_BITSET_SIZE_T
		typedef size_t                  size_type;
	  #else
		typedef eastl_size_t            size_type;
	  #endif

		enum {
			kBitsPerWord      = (8 * sizeof(word_type)),
			kBitsPerWordMask  = (kBitsPerWord - 1),
			kBitsPerWordShift = ((kBitsPerWord == 8) ? 3 : ((kBitsPerWord == 16) ? 4 : ((kBitsPerWord == 32) ? 5 : (((kBitsPerWord == 64) ? 6 : 7)))))
		};

	public:
		word_type mWord[1]; // Defined as an array of 1 so that bitset can treat this BitsetBase like others.

	public:
		BitsetBase();
		BitsetBase(uint32_t value);
	  //BitsetBase(uint64_t value); // Disabled because it causes conflicts with the 32 bit version with existing user code. Use from_uint64 instead.

		void operator&=(const this_type& x);
		void operator|=(const this_type& x);
		void operator^=(const this_type& x);

		void operator<<=(size_type n);
		void operator>>=(size_type n);

		void flip();
		void set();
		void set(size_type i, bool value);
		void reset();

		bool operator==(const this_type& x) const;

		bool      any() const;
		size_type count() const;

		void          from_uint32(uint32_t value);
		void          from_uint64(uint64_t value);

		unsigned long to_ulong() const;
		uint32_t      to_uint32() const;
		uint64_t      to_uint64() const;

		word_type& DoGetWord(size_type);
		word_type  DoGetWord(size_type) const;

		size_type DoFindFirst() const;
		size_type DoFindNext(size_type last_find) const;

		size_type DoFindLast() const;                       // Returns 1 * kBitsPerWord (the bit count) if no bits are set.
		size_type DoFindPrev(size_type last_find) const;    // Returns 1 * kBitsPerWord (the bit count) if no bits are set.

	}; // BitsetBase<1, WordType>



	/// BitsetBase<2, WordType>
	/// 
	/// This is a specialization for a bitset that fits within two words.
	/// The difference here is that we avoid branching (ifs and loops).
	///
	template <typename WordType>
	struct BitsetBase<2, WordType>
	{
		typedef WordType                 word_type;
		typedef BitsetBase<2, WordType>  this_type;
	  #if EASTL_BITSET_SIZE_T
		typedef size_t                   size_type;
	  #else
		typedef eastl_size_t             size_type;
	  #endif

		enum {
			kBitsPerWord      = (8 * sizeof(word_type)),
			kBitsPerWordMask  = (kBitsPerWord - 1),
			kBitsPerWordShift = ((kBitsPerWord == 8) ? 3 : ((kBitsPerWord == 16) ? 4 : ((kBitsPerWord == 32) ? 5 : (((kBitsPerWord == 64) ? 6 : 7)))))
		};

	public:
		word_type mWord[2];

	public:
		BitsetBase();
		BitsetBase(uint32_t value);
	  //BitsetBase(uint64_t value); // Disabled because it causes conflicts with the 32 bit version with existing user code. Use from_uint64 instead.

		void operator&=(const this_type& x);
		void operator|=(const this_type& x);
		void operator^=(const this_type& x);

		void operator<<=(size_type n);
		void operator>>=(size_type n);

		void flip();
		void set();
		void set(size_type i, bool value);
		void reset();

		bool operator==(const this_type& x) const;

		bool      any() const;
		size_type count() const;

		void          from_uint32(uint32_t value);
		void          from_uint64(uint64_t value);

		unsigned long to_ulong() const;
		uint32_t      to_uint32() const;
		uint64_t      to_uint64() const;

		word_type& DoGetWord(size_type);
		word_type  DoGetWord(size_type) const;

		size_type DoFindFirst() const;
		size_type DoFindNext(size_type last_find) const;

		size_type DoFindLast() const;                       // Returns 2 * kBitsPerWord (the bit count) if no bits are set.
		size_type DoFindPrev(size_type last_find) const;    // Returns 2 * kBitsPerWord (the bit count) if no bits are set.

	}; // BitsetBase<2, WordType>




	/// bitset
	///
	/// Implements a bitset much like the C++ std::bitset.
	///
	/// As of this writing we don't implement a specialization of bitset<0>,
	/// as it is deemed an academic exercise that nobody would actually
	/// use and it would increase code space and provide little practical
	/// benefit. Note that this doesn't mean bitset<0> isn't supported; 
	/// it means that our version of it isn't as efficient as it would be 
	/// if a specialization was made for it.
	///
	/// - N can be any unsigned (non-zero) value, though memory usage is 
	///   linear with respect to N, so large values of N use large amounts of memory.
	/// - WordType must be one of [uint16_t, uint32_t, uint64_t, uint128_t] 
	///   and the compiler must support the type. By default the WordType is
	///   the largest native register type that the target platform supports.
	///
	template <size_t N, typename WordType = EASTL_BITSET_WORD_TYPE_DEFAULT>
	class bitset : private BitsetBase<BITSET_WORD_COUNT(N, WordType), WordType>
	{
	public:
		typedef BitsetBase<BITSET_WORD_COUNT(N, WordType), WordType>  base_type;
		typedef bitset<N, WordType>                                   this_type;
		typedef WordType                                              word_type;
		typedef typename base_type::size_type                         size_type;

		enum
		{
			kBitsPerWord      = (8 * sizeof(word_type)),
			kBitsPerWordMask  = (kBitsPerWord - 1),
			kBitsPerWordShift = ((kBitsPerWord == 8) ? 3 : ((kBitsPerWord == 16) ? 4 : ((kBitsPerWord == 32) ? 5 : (((kBitsPerWord == 64) ? 6 : 7))))),
			kSize             = N,                               // The number of bits the bitset holds
			kWordSize         = sizeof(word_type),               // The size of individual words the bitset uses to hold the bits.
			kWordCount        = BITSET_WORD_COUNT(N, WordType)   // The number of words the bitset uses to hold the bits. sizeof(bitset<N, WordType>) == kWordSize * kWordCount.
		};

		using base_type::mWord;
		using base_type::DoGetWord;
		using base_type::DoFindFirst;
		using base_type::DoFindNext;
		using base_type::DoFindLast;
		using base_type::DoFindPrev;
		using base_type::to_ulong;
		using base_type::to_uint32;
		using base_type::to_uint64;
		using base_type::count;
		using base_type::any;

	public:
		/// reference
		///
		/// A reference is a reference to a specific bit in the bitset.
		/// The C++ standard specifies that this be a nested class, 
		/// though it is not clear if a non-nested reference implementation
		/// would be non-conforming.
		///
		class reference
		{
		protected:
			friend class bitset<N, WordType>;

			word_type* mpBitWord;
			size_type  mnBitIndex;
		
			reference(){} // The C++ standard specifies that this is private.
	
		public:
			reference(const bitset& x, size_type i);

			reference& operator=(bool value);
			reference& operator=(const reference& x);

			bool operator~() const;
			operator bool() const // Defined inline because CodeWarrior fails to be able to compile it outside.
			   { return (*mpBitWord & (static_cast<word_type>(1) << (mnBitIndex & kBitsPerWordMask))) != 0; }

			reference& flip();
		};

	public:
		friend class reference;

		bitset();
		bitset(uint32_t value);
	  //bitset(uint64_t value); // Disabled because it causes conflicts with the 32 bit version with existing user code. Use from_uint64 instead.

		// We don't define copy constructor and operator= because 
		// the compiler-generated versions will suffice.

		this_type& operator&=(const this_type& x);
		this_type& operator|=(const this_type& x);
		this_type& operator^=(const this_type& x);

		this_type& operator<<=(size_type n);
		this_type& operator>>=(size_type n);

		this_type& set();
		this_type& set(size_type i, bool value = true);

		this_type& reset();
		this_type& reset(size_type i);
			
		this_type& flip();
		this_type& flip(size_type i);
		this_type  operator~() const;

		reference operator[](size_type i);
		bool      operator[](size_type i) const;

		const word_type* data() const;
		word_type*       data();

		void          from_uint32(uint32_t value);
		void          from_uint64(uint64_t value);

	  //unsigned long to_ulong()  const;    // We inherit this from the base class.
	  //uint32_t      to_uint32() const;
	  //uint64_t      to_uint64() const;

	  //size_type count() const;            // We inherit this from the base class.
		size_type size() const;

		bool operator==(const this_type& x) const;
		bool operator!=(const this_type& x) const;

		bool test(size_type i) const;
	  //bool any() const;                   // We inherit this from the base class.
		bool all() const;
		bool none() const;

		this_type operator<<(size_type n) const;
		this_type operator>>(size_type n) const;

		// Finds the index of the first "on" bit, returns kSize if none are set.
		size_type find_first() const;

		// Finds the index of the next "on" bit after last_find, returns kSize if none are set.
		size_type find_next(size_type last_find) const;

		// Finds the index of the last "on" bit, returns kSize if none are set.
		size_type find_last() const;

		// Finds the index of the last "on" bit before last_find, returns kSize if none are set.
		size_type find_prev(size_type last_find) const;

	}; // bitset







	/// BitsetCountBits
	///
	/// This is a fast trick way to count bits without branches nor memory accesses.
	///
	inline uint32_t BitsetCountBits(uint64_t x)
	{
		// GCC 3.x's implementation of UINT64_C is broken and fails to deal with 
		// the code below correctly. So we make a workaround for it. Earlier and 
		// later versions of GCC don't have this bug.

		#if defined(__GNUC__) && (__GNUC__ == 3)
			x = x - ((x >> 1) & 0x5555555555555555ULL);
			x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
			x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
			return (uint32_t)((x * 0x0101010101010101ULL) >> 56);
		#else
			x = x - ((x >> 1) & UINT64_C(0x5555555555555555));
			x = (x & UINT64_C(0x3333333333333333)) + ((x >> 2) & UINT64_C(0x3333333333333333));
			x = (x + (x >> 4)) & UINT64_C(0x0F0F0F0F0F0F0F0F);
			return (uint32_t)((x * UINT64_C(0x0101010101010101)) >> 56);
		#endif
	}

	inline uint32_t BitsetCountBits(uint32_t x)
	{
		x = x - ((x >> 1) & 0x55555555);
		x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
		x = (x + (x >> 4)) & 0x0F0F0F0F;
		return (uint32_t)((x * 0x01010101) >> 24);
	}

	inline uint32_t BitsetCountBits(uint16_t x)
	{
		return BitsetCountBits((uint32_t)x);
	}

	inline uint32_t BitsetCountBits(uint8_t x)
	{
		return BitsetCountBits((uint32_t)x);
	}


	// const static char kBitsPerUint16[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
	#define EASTL_BITSET_COUNT_STRING "\0\1\1\2\1\2\2\3\1\2\2\3\2\3\3\4"


	inline uint32_t GetFirstBit(uint8_t x)
	{
		if(x)
		{
			uint32_t n = 1;

			if((x & 0x0000000F) == 0) { n +=  4; x >>=  4; }
			if((x & 0x00000003) == 0) { n +=  2; x >>=  2; }

			return (uint32_t)(n - (x & 1));
		}

		return 8;
	}

	inline uint32_t GetFirstBit(uint16_t x) // To do: Update this to use VC++ _BitScanForward, _BitScanForward64; GCC __builtin_ctz, __builtin_ctzl. VC++ __lzcnt16, __lzcnt, __lzcnt64 requires recent CPUs (2013+) and probably can't be used. http://en.wikipedia.org/wiki/Haswell_%28microarchitecture%29#New_features
	{
		if(x)
		{
			uint32_t n = 1;

			if((x & 0x000000FF) == 0) { n +=  8; x >>=  8; }
			if((x & 0x0000000F) == 0) { n +=  4; x >>=  4; }
			if((x & 0x00000003) == 0) { n +=  2; x >>=  2; }

			return (uint32_t)(n - (x & 1));
		}

		return 16;
	}

	inline uint32_t GetFirstBit(uint32_t x)
	{
		if(x)
		{
			uint32_t n = 1;

			if((x & 0x0000FFFF) == 0) { n += 16; x >>= 16; }
			if((x & 0x000000FF) == 0) { n +=  8; x >>=  8; }
			if((x & 0x0000000F) == 0) { n +=  4; x >>=  4; }
			if((x & 0x00000003) == 0) { n +=  2; x >>=  2; }

			return (n - (x & 1));
		}

		return 32;
	}

	inline uint32_t GetFirstBit(uint64_t x)
	{
		if(x)
		{
			uint32_t n = 1;

			if((x & 0xFFFFFFFF) == 0) { n += 32; x >>= 32; }
			if((x & 0x0000FFFF) == 0) { n += 16; x >>= 16; }
			if((x & 0x000000FF) == 0) { n +=  8; x >>=  8; }
			if((x & 0x0000000F) == 0) { n +=  4; x >>=  4; }
			if((x & 0x00000003) == 0) { n +=  2; x >>=  2; }

			return (n - ((uint32_t)x & 1));
		}

		return 64;
	}


	#if EASTL_INT128_SUPPORTED
		inline uint32_t GetFirstBit(eastl_uint128_t x)
		{
			if(x)
			{
				uint32_t n = 1;

				if((x & UINT64_C(0xFFFFFFFFFFFFFFFF)) == 0) { n += 64; x >>= 64; }
				if((x & 0xFFFFFFFF) == 0)                   { n += 32; x >>= 32; }
				if((x & 0x0000FFFF) == 0)                   { n += 16; x >>= 16; }
				if((x & 0x000000FF) == 0)                   { n +=  8; x >>=  8; }
				if((x & 0x0000000F) == 0)                   { n +=  4; x >>=  4; }
				if((x & 0x00000003) == 0)                   { n +=  2; x >>=  2; }

				return (n - ((uint32_t)x & 1));
			}

			return 128;
		}
	#endif

	inline uint32_t GetLastBit(uint8_t x)
	{
		if(x)
		{
			uint32_t n = 0;

			if(x & 0xFFF0) { n +=  4; x >>=  4; }
			if(x & 0xFFFC) { n +=  2; x >>=  2; }
			if(x & 0xFFFE) { n +=  1;           }

			return n;
		}

		return 8;
	}

	inline uint32_t GetLastBit(uint16_t x)
	{
		if(x)
		{
			uint32_t n = 0;

			if(x & 0xFF00) { n +=  8; x >>=  8; }
			if(x & 0xFFF0) { n +=  4; x >>=  4; }
			if(x & 0xFFFC) { n +=  2; x >>=  2; }
			if(x & 0xFFFE) { n +=  1;           }

			return n;
		}

		return 16;
	}

	inline uint32_t GetLastBit(uint32_t x)
	{
		if(x)
		{
			uint32_t n = 0;

			if(x & 0xFFFF0000) { n += 16; x >>= 16; }
			if(x & 0xFFFFFF00) { n +=  8; x >>=  8; }
			if(x & 0xFFFFFFF0) { n +=  4; x >>=  4; }
			if(x & 0xFFFFFFFC) { n +=  2; x >>=  2; }
			if(x & 0xFFFFFFFE) { n +=  1;           }

			return n;
		}

		return 32;
	}

	inline uint32_t GetLastBit(uint64_t x)
	{
		if(x)
		{
			uint32_t n = 0;

			if(x & UINT64_C(0xFFFFFFFF00000000)) { n += 32; x >>= 32; }
			if(x & 0xFFFF0000)                   { n += 16; x >>= 16; }
			if(x & 0xFFFFFF00)                   { n +=  8; x >>=  8; }
			if(x & 0xFFFFFFF0)                   { n +=  4; x >>=  4; }
			if(x & 0xFFFFFFFC)                   { n +=  2; x >>=  2; }
			if(x & 0xFFFFFFFE)                   { n +=  1;           }

			return n;
		}

		return 64;
	}

	#if EASTL_INT128_SUPPORTED
		inline uint32_t GetLastBit(eastl_uint128_t x)
		{
			if(x)
			{
				uint32_t n = 0;
				
				eastl_uint128_t mask(UINT64_C(0xFFFFFFFF00000000)); // There doesn't seem to exist compiler support for INT128_C() by any compiler. EAStdC's int128_t supports it though.
				mask <<= 64;

				if(x & mask)                         { n += 64; x >>= 64; }
				if(x & UINT64_C(0xFFFFFFFF00000000)) { n += 32; x >>= 32; }
				if(x & UINT64_C(0x00000000FFFF0000)) { n += 16; x >>= 16; }
				if(x & UINT64_C(0x00000000FFFFFF00)) { n +=  8; x >>=  8; }
				if(x & UINT64_C(0x00000000FFFFFFF0)) { n +=  4; x >>=  4; }
				if(x & UINT64_C(0x00000000FFFFFFFC)) { n +=  2; x >>=  2; }
				if(x & UINT64_C(0x00000000FFFFFFFE)) { n +=  1;           }

				return n;
			}

			return 128;
		}
	#endif




	///////////////////////////////////////////////////////////////////////////
	// BitsetBase
	//
	// We tried two forms of array access here:
	//     for(word_type *pWord(mWord), *pWordEnd(mWord + NW); pWord < pWordEnd; ++pWord)
	//         *pWord = ...
	// and
	//     for(size_t i = 0; i < NW; i++)
	//         mWord[i] = ...
	//
	// For our tests (~NW < 16), the latter (using []) access resulted in faster code. 
	///////////////////////////////////////////////////////////////////////////

	template <size_t NW, typename WordType>
	inline BitsetBase<NW, WordType>::BitsetBase()
	{
		reset();
	}


	template <size_t NW, typename WordType>
	inline BitsetBase<NW, WordType>::BitsetBase(uint32_t value)
	{
		// This implementation assumes that sizeof(value) <= sizeof(word_type).
		//EASTL_CT_ASSERT(sizeof(value) <= sizeof(word_type)); Disabled because we now have support for uint8_t and uint16_t word types. It would be nice to have a runtime assert that tested this.

		reset();
		mWord[0] = static_cast<word_type>(value);
	}


	/*
	template <size_t NW, typename WordType>
	inline BitsetBase<NW, WordType>::BitsetBase(uint64_t value)
	{
		reset();

		#if(EA_PLATFORM_WORD_SIZE == 4)
			mWord[0] = static_cast<word_type>(value);

			EASTL_CT_ASSERT(NW > 2); // We can assume this because we have specializations of BitsetBase for <1> and <2>.
			//if(NW > 1) // NW is a template constant, but it would be a little messy to take advantage of it's const-ness.
				mWord[1] = static_cast<word_type>(value >> 32);
		#else
			mWord[0] = static_cast<word_type>(value);
		#endif
	}
	*/


	template <size_t NW, typename WordType>
	inline void BitsetBase<NW, WordType>::operator&=(const this_type& x)
	{
		for(size_t i = 0; i < NW; i++)
			mWord[i] &= x.mWord[i];
	}


	template <size_t NW, typename WordType>
	inline void BitsetBase<NW, WordType>::operator|=(const this_type& x)
	{
		for(size_t i = 0; i < NW; i++)
			mWord[i] |= x.mWord[i];
	}


	template <size_t NW, typename WordType>
	inline void BitsetBase<NW, WordType>::operator^=(const this_type& x)
	{
		for(size_t i = 0; i < NW; i++)
			mWord[i] ^= x.mWord[i];
	}


	template <size_t NW, typename WordType>
	inline void BitsetBase<NW, WordType>::operator<<=(size_type n)
	{
		const size_type nWordShift = (size_type)(n >> kBitsPerWordShift);

		if(nWordShift)
		{
			for(int i = (int)(NW - 1); i >= 0; --i)
				mWord[i] = (nWordShift <= (size_type)i) ? mWord[i - nWordShift] : (word_type)0;
		}

		if(n &= kBitsPerWordMask)
		{
			for(size_t i = (NW - 1); i > 0; --i)
				mWord[i] = (word_type)((mWord[i] << n) | (mWord[i - 1] >> (kBitsPerWord - n)));
			mWord[0] <<= n;
		}

		// We let the parent class turn off any upper bits.
	}


	template <size_t NW, typename WordType>
	inline void BitsetBase<NW, WordType>::operator>>=(size_type n)
	{
		const size_type nWordShift = (size_type)(n >> kBitsPerWordShift);

		if(nWordShift)
		{
			for(size_t i = 0; i < NW; ++i)
				mWord[i] = ((nWordShift < (NW - i)) ? mWord[i + nWordShift] : (word_type)0);
		}

		if(n &= kBitsPerWordMask)
		{
			for(size_t i = 0; i < (NW - 1); ++i)
				mWord[i] = (word_type)((mWord[i] >> n) | (mWord[i + 1] << (kBitsPerWord - n)));
			mWord[NW - 1] >>= n;
		}
	}


	template <size_t NW, typename WordType>
	inline void BitsetBase<NW, WordType>::flip()
	{
		for(size_t i = 0; i < NW; i++)
			mWord[i] = ~mWord[i];
		// We let the parent class turn off any upper bits.
	}


	template <size_t NW, typename WordType>
	inline void BitsetBase<NW, WordType>::set()
	{
		for(size_t i = 0; i < NW; i++)
			mWord[i] = static_cast<word_type>(~static_cast<word_type>(0));
		// We let the parent class turn off any upper bits.
	}


	template <size_t NW, typename WordType>
	inline void BitsetBase<NW, WordType>::set(size_type i, bool value)
	{
		if(value)
			mWord[i >> kBitsPerWordShift] |=  (static_cast<word_type>(1) << (i & kBitsPerWordMask));
		else
			mWord[i >> kBitsPerWordShift] &= ~(static_cast<word_type>(1) << (i & kBitsPerWordMask));
	}


	template <size_t NW, typename WordType>
	inline void BitsetBase<NW, WordType>::reset()
	{
		if(NW > 16) // This is a constant expression and should be optimized away.
		{
			// This will be fastest if compiler intrinsic function optimizations are enabled.
			memset(mWord, 0, sizeof(mWord));
		}
		else
		{
			for(size_t i = 0; i < NW; i++)
				mWord[i] = 0;
		}
	}


	template <size_t NW, typename WordType>
	inline bool BitsetBase<NW, WordType>::operator==(const this_type& x) const
	{
		for(size_t i = 0; i < NW; i++)
		{
			if(mWord[i] != x.mWord[i])
				return false;
		}
		return true;
	}


	template <size_t NW, typename WordType>
	inline bool BitsetBase<NW, WordType>::any() const
	{
		for(size_t i = 0; i < NW; i++)
		{
			if(mWord[i])
				return true;
		}
		return false;
	}


	template <size_t NW, typename WordType>
	inline typename BitsetBase<NW, WordType>::size_type
	BitsetBase<NW, WordType>::count() const
	{
		size_type n = 0;

		for(size_t i = 0; i < NW; i++)
		{
			#if defined(__GNUC__) && (((__GNUC__ * 100) + __GNUC_MINOR__) >= 304) && !defined(EA_PLATFORM_ANDROID) // GCC 3.4 or later
				#if(EA_PLATFORM_WORD_SIZE == 4)
					n += (size_type)__builtin_popcountl(mWord[i]);
				#else
					n += (size_type)__builtin_popcountll(mWord[i]);
				#endif
			#elif defined(__GNUC__) && (__GNUC__ < 3)
				n +=  BitsetCountBits(mWord[i]); // GCC 2.x compiler inexplicably blows up on the code below.
			#else
				// todo: use __popcnt16, __popcnt, __popcnt64 for msvc builds
				// https://msdn.microsoft.com/en-us/library/bb385231(v=vs.140).aspx
				for(word_type w = mWord[i]; w; w >>= 4)
					n += EASTL_BITSET_COUNT_STRING[w & 0xF];

				// Version which seems to run slower in benchmarks:
				// n +=  BitsetCountBits(mWord[i]);
			#endif

		}
		return n;
	}


	template <size_t NW, typename WordType>
	inline void BitsetBase<NW, WordType>::from_uint32(uint32_t value)
	{
		reset();
		mWord[0] = static_cast<word_type>(value);
	}


	template <size_t NW, typename WordType>
	inline void BitsetBase<NW, WordType>::from_uint64(uint64_t value)
	{
		reset();

		#if(EA_PLATFORM_WORD_SIZE == 4)
			mWord[0] = static_cast<word_type>(value);

			EASTL_CT_ASSERT(NW > 2); // We can assume this because we have specializations of BitsetBase for <1> and <2>.
			//if(NW > 1) // NW is a template constant, but it would be a little messy to take advantage of it's const-ness.
				mWord[1] = static_cast<word_type>(value >> 32);
		#else
			mWord[0] = static_cast<word_type>(value);
		#endif
	}


	template <size_t NW, typename WordType>
	inline unsigned long BitsetBase<NW, WordType>::to_ulong() const
	{
		#if EASTL_EXCEPTIONS_ENABLED
			for(size_t i = 1; i < NW; ++i)
			{
				if(mWord[i])
					throw std::overflow_error("BitsetBase::to_ulong");
			}
		#endif
		return (unsigned long)mWord[0]; // Todo: We need to deal with the case whereby sizeof(word_type) < sizeof(unsigned long)
	}


	template <size_t NW, typename WordType>
	inline uint32_t BitsetBase<NW, WordType>::to_uint32() const
	{
		#if EASTL_EXCEPTIONS_ENABLED
			// Verify that high words or bits are not set and thus that to_uint32 doesn't lose information.
			for(size_t i = 1; i < NW; ++i)
			{
				if(mWord[i])
					throw std::overflow_error("BitsetBase::to_uint32");
			}
			
			#if(EA_PLATFORM_WORD_SIZE > 4) // if we have 64 bit words...
				if(mWord[0] >> 32)
					throw std::overflow_error("BitsetBase::to_uint32");
			#endif
		#endif

		return (uint32_t)mWord[0];
	}


	template <size_t NW, typename WordType>
	inline uint64_t BitsetBase<NW, WordType>::to_uint64() const
	{
		#if EASTL_EXCEPTIONS_ENABLED
			// Verify that high words are not set and thus that to_uint64 doesn't lose information.
			
			EASTL_CT_ASSERT(NW > 2); // We can assume this because we have specializations of BitsetBase for <1> and <2>.
			for(size_t i = 2; i < NW; ++i)
			{
				if(mWord[i])
					throw std::overflow_error("BitsetBase::to_uint64");
			}
		#endif

		#if(EA_PLATFORM_WORD_SIZE == 4)
			EASTL_CT_ASSERT(NW > 2); // We can assume this because we have specializations of BitsetBase for <1> and <2>.
			return (mWord[1] << 32) | mWord[0];
		#else
			return (uint64_t)mWord[0];
		#endif
	}


	template <size_t NW, typename WordType>
	inline typename BitsetBase<NW, WordType>::word_type&
	BitsetBase<NW, WordType>::DoGetWord(size_type i)
	{
		return mWord[i >> kBitsPerWordShift];
	}


	template <size_t NW, typename WordType>
	inline typename BitsetBase<NW, WordType>::word_type
	BitsetBase<NW, WordType>::DoGetWord(size_type i) const
	{
		return mWord[i >> kBitsPerWordShift];
	}


	template <size_t NW, typename WordType>
	inline typename BitsetBase<NW, WordType>::size_type 
	BitsetBase<NW, WordType>::DoFindFirst() const
	{
		for(size_type word_index = 0; word_index < NW; ++word_index)
		{
			const size_type fbiw = GetFirstBit(mWord[word_index]);

			if(fbiw != kBitsPerWord)
				return (word_index * kBitsPerWord) + fbiw;
		}

		return (size_type)NW * kBitsPerWord;
	}


#if EASTL_DISABLE_BITSET_ARRAYBOUNDS_WARNING
EA_DISABLE_GCC_WARNING(-Warray-bounds)
#endif

	template <size_t NW, typename WordType>
	inline typename BitsetBase<NW, WordType>::size_type 
	BitsetBase<NW, WordType>::DoFindNext(size_type last_find) const
	{
		// Start looking from the next bit.
		++last_find;

		// Set initial state based on last find.
		size_type word_index = static_cast<size_type>(last_find >> kBitsPerWordShift);
		size_type bit_index  = static_cast<size_type>(last_find  & kBitsPerWordMask);

		// To do: There probably is a more elegant way to write looping below.
		if(word_index < NW)
		{
			// Mask off previous bits of the word so our search becomes a "find first".
			word_type this_word = mWord[word_index] & (~static_cast<word_type>(0) << bit_index);

			for(;;)
			{
				const size_type fbiw = GetFirstBit(this_word);

				if(fbiw != kBitsPerWord)
					return (word_index * kBitsPerWord) + fbiw;

				if(++word_index < NW)
					this_word = mWord[word_index];
				else
					break;
			}
		}

		return (size_type)NW * kBitsPerWord;
	}

#if EASTL_DISABLE_BITSET_ARRAYBOUNDS_WARNING
EA_RESTORE_GCC_WARNING()
#endif



	template <size_t NW, typename WordType>
	inline typename BitsetBase<NW, WordType>::size_type 
	BitsetBase<NW, WordType>::DoFindLast() const
	{
		for(size_type word_index = (size_type)NW; word_index > 0; --word_index)
		{
			const size_type lbiw = GetLastBit(mWord[word_index - 1]);

			if(lbiw != kBitsPerWord)
				return ((word_index - 1) * kBitsPerWord) + lbiw;
		}

		return (size_type)NW * kBitsPerWord;
	}


	template <size_t NW, typename WordType>
	inline typename BitsetBase<NW, WordType>::size_type 
	BitsetBase<NW, WordType>::DoFindPrev(size_type last_find) const
	{
		if(last_find > 0)
		{
			// Set initial state based on last find.
			size_type word_index = static_cast<size_type>(last_find >> kBitsPerWordShift);
			size_type bit_index  = static_cast<size_type>(last_find  & kBitsPerWordMask);

			// Mask off subsequent bits of the word so our search becomes a "find last".
			word_type mask      = (~static_cast<word_type>(0) >> (kBitsPerWord - 1 - bit_index)) >> 1; // We do two shifts here because many CPUs ignore requests to shift 32 bit integers by 32 bits, which could be the case above.
			word_type this_word = mWord[word_index] & mask;

			for(;;)
			{
				const size_type lbiw = GetLastBit(this_word);

				if(lbiw != kBitsPerWord)
					return (word_index * kBitsPerWord) + lbiw;

				if(word_index > 0)
					this_word = mWord[--word_index];
				else
					break;
			}
		}

		return (size_type)NW * kBitsPerWord;
	}



	///////////////////////////////////////////////////////////////////////////
	// BitsetBase<1, WordType>
	///////////////////////////////////////////////////////////////////////////

	template <typename WordType>
	inline BitsetBase<1, WordType>::BitsetBase()
	{
		mWord[0] = 0;
	}


	template <typename WordType>
	inline BitsetBase<1, WordType>::BitsetBase(uint32_t value)
	{
		// This implementation assumes that sizeof(value) <= sizeof(word_type).
		//EASTL_CT_ASSERT(sizeof(value) <= sizeof(word_type)); Disabled because we now have support for uint8_t and uint16_t word types. It would be nice to have a runtime assert that tested this.

		mWord[0] = static_cast<word_type>(value);
	}


	/*
	template <typename WordType>
	inline BitsetBase<1, WordType>::BitsetBase(uint64_t value)
	{
		#if(EA_PLATFORM_WORD_SIZE == 4)
			EASTL_ASSERT(value <= 0xffffffff);
			mWord[0] = static_cast<word_type>(value);   // This potentially loses data, but that's what the user is requesting.
		#else
			mWord[0] = static_cast<word_type>(value);
		#endif
	}
	*/


	template <typename WordType>
	inline void BitsetBase<1, WordType>::operator&=(const this_type& x)
	{
		mWord[0] &= x.mWord[0];
	}


	template <typename WordType>
	inline void BitsetBase<1, WordType>::operator|=(const this_type& x)
	{
		mWord[0] |= x.mWord[0];
	}


	template <typename WordType>
	inline void BitsetBase<1, WordType>::operator^=(const this_type& x)
	{
		mWord[0] ^= x.mWord[0];
	}


	template <typename WordType>
	inline void BitsetBase<1, WordType>::operator<<=(size_type n)
	{
		mWord[0] <<= n;
		// We let the parent class turn off any upper bits.
	}


	template <typename WordType>
	inline void BitsetBase<1, WordType>::operator>>=(size_type n)
	{
		mWord[0] >>= n;
	}


	template <typename WordType>
	inline void BitsetBase<1, WordType>::flip()
	{
		mWord[0] = ~mWord[0];
		// We let the parent class turn off any upper bits.
	}


	template <typename WordType>
	inline void BitsetBase<1, WordType>::set()
	{
		mWord[0] = static_cast<word_type>(~static_cast<word_type>(0));
		// We let the parent class turn off any upper bits.
	}


	template <typename WordType>
	inline void BitsetBase<1, WordType>::set(size_type i, bool value)
	{
		if(value)
			mWord[0] |=  (static_cast<word_type>(1) << i);
		else
			mWord[0] &= ~(static_cast<word_type>(1) << i);
	}


	template <typename WordType>
	inline void BitsetBase<1, WordType>::reset()
	{
		mWord[0] = 0;
	}


	template <typename WordType>
	inline bool BitsetBase<1, WordType>::operator==(const this_type& x) const
	{
		return mWord[0] == x.mWord[0];
	}


	template <typename WordType>
	inline bool BitsetBase<1, WordType>::any() const
	{
		return mWord[0] != 0;
	}


	template <typename WordType>
	inline typename BitsetBase<1, WordType>::size_type
	BitsetBase<1, WordType>::count() const
	{
		#if defined(__GNUC__) && (((__GNUC__ * 100) + __GNUC_MINOR__) >= 304) && !defined(EA_PLATFORM_ANDROID) // GCC 3.4 or later
			#if(EA_PLATFORM_WORD_SIZE == 4)
				return (size_type)__builtin_popcountl(mWord[0]);
			#else
				return (size_type)__builtin_popcountll(mWord[0]);
			#endif
		#elif defined(__GNUC__) && (__GNUC__ < 3)
			return BitsetCountBits(mWord[0]); // GCC 2.x compiler inexplicably blows up on the code below.
		#else
			size_type n = 0;
			for(word_type w = mWord[0]; w; w >>= 4)
				n += EASTL_BITSET_COUNT_STRING[w & 0xF];
			return n;
		#endif
	}


	template <typename WordType>
	inline void BitsetBase<1, WordType>::from_uint32(uint32_t value)
	{
		mWord[0] = static_cast<word_type>(value);
	}


	template <typename WordType>
	inline void BitsetBase<1, WordType>::from_uint64(uint64_t value)
	{
		#if(EA_PLATFORM_WORD_SIZE == 4)
			EASTL_ASSERT(value <= 0xffffffff);
			mWord[0] = static_cast<word_type>(value);   // This potentially loses data, but that's what the user is requesting.
		#else
			mWord[0] = static_cast<word_type>(value);
		#endif
	}


	template <typename WordType>
	inline unsigned long BitsetBase<1, WordType>::to_ulong() const
	{
		#if EASTL_EXCEPTIONS_ENABLED
			#if((EA_PLATFORM_WORD_SIZE > 4) && defined(EA_PLATFORM_MICROSOFT)) // If we are using 64 bit words but ulong is less than 64 bits... Microsoft platforms alone use a 32 bit long under 64 bit platforms.
				// Verify that high bits are not set and thus that to_ulong doesn't lose information.
				if(mWord[0] >> 32)
					throw std::overflow_error("BitsetBase::to_ulong");
			#endif
		#endif

		return static_cast<unsigned long>(mWord[0]);
	}


	template <typename WordType>
	inline uint32_t BitsetBase<1, WordType>::to_uint32() const
	{
		#if EASTL_EXCEPTIONS_ENABLED
			#if(EA_PLATFORM_WORD_SIZE > 4) // If we are using 64 bit words...
				// Verify that high bits are not set and thus that to_uint32 doesn't lose information.
				if(mWord[0] >> 32)
					throw std::overflow_error("BitsetBase::to_uint32");
			#endif
		#endif

		return static_cast<uint32_t>(mWord[0]);
	}


	template <typename WordType>
	inline uint64_t BitsetBase<1, WordType>::to_uint64() const
	{
		// This implementation is the same regardless of the word size, and there is no possibility of overflow_error.
		return static_cast<uint64_t>(mWord[0]);
	}


	template <typename WordType>
	inline typename BitsetBase<1, WordType>::word_type&
	BitsetBase<1, WordType>::DoGetWord(size_type)
	{
		return mWord[0];
	}


	template <typename WordType>
	inline typename BitsetBase<1, WordType>::word_type
	BitsetBase<1, WordType>::DoGetWord(size_type) const
	{
		return mWord[0];
	}


	template <typename WordType>
	inline typename BitsetBase<1, WordType>::size_type
	BitsetBase<1, WordType>::DoFindFirst() const
	{
		return GetFirstBit(mWord[0]);
	}


	template <typename WordType>
	inline typename BitsetBase<1, WordType>::size_type 
	BitsetBase<1, WordType>::DoFindNext(size_type last_find) const
	{
		if(++last_find < kBitsPerWord)
		{
			// Mask off previous bits of word so our search becomes a "find first".
			const word_type this_word = mWord[0] & ((~static_cast<word_type>(0)) << last_find);

			return GetFirstBit(this_word);
		}

		return kBitsPerWord;
	}


	template <typename WordType>
	inline typename BitsetBase<1, WordType>::size_type 
	BitsetBase<1, WordType>::DoFindLast() const
	{
		return GetLastBit(mWord[0]);
	}


	template <typename WordType>
	inline typename BitsetBase<1, WordType>::size_type 
	BitsetBase<1, WordType>::DoFindPrev(size_type last_find) const
	{
		if(last_find > 0)
		{
			// Mask off previous bits of word so our search becomes a "find first".
			const word_type this_word = mWord[0] & ((~static_cast<word_type>(0)) >> (kBitsPerWord - last_find));

			return GetLastBit(this_word);
		}

		return kBitsPerWord;
	}




	///////////////////////////////////////////////////////////////////////////
	// BitsetBase<2, WordType>
	///////////////////////////////////////////////////////////////////////////

	template <typename WordType>
	inline BitsetBase<2, WordType>::BitsetBase()
	{
		mWord[0] = 0;
		mWord[1] = 0;
	}


	template <typename WordType>
	inline BitsetBase<2, WordType>::BitsetBase(uint32_t value)
	{
		// This implementation assumes that sizeof(value) <= sizeof(word_type).
		//EASTL_CT_ASSERT(sizeof(value) <= sizeof(word_type)); Disabled because we now have support for uint8_t and uint16_t word types. It would be nice to have a runtime assert that tested this.

		mWord[0] = static_cast<word_type>(value);
		mWord[1] = 0;
	}


	/*
	template <typename WordType>
	inline BitsetBase<2, WordType>::BitsetBase(uint64_t value)
	{
		#if(EA_PLATFORM_WORD_SIZE == 4)
			mWord[0] = static_cast<word_type>(value);
			mWord[1] = static_cast<word_type>(value >> 32);
		#else
			mWord[0] = static_cast<word_type>(value);
			mWord[1] = 0;
		#endif
	}
	*/


	template <typename WordType>
	inline void BitsetBase<2, WordType>::operator&=(const this_type& x)
	{
		mWord[0] &= x.mWord[0];
		mWord[1] &= x.mWord[1];
	}


	template <typename WordType>
	inline void BitsetBase<2, WordType>::operator|=(const this_type& x)
	{
		mWord[0] |= x.mWord[0];
		mWord[1] |= x.mWord[1];
	}


	template <typename WordType>
	inline void BitsetBase<2, WordType>::operator^=(const this_type& x)
	{
		mWord[0] ^= x.mWord[0];
		mWord[1] ^= x.mWord[1];
	}


	template <typename WordType>
	inline void BitsetBase<2, WordType>::operator<<=(size_type n)
	{
		if(n) // to avoid a shift by kBitsPerWord, which is undefined
		{
			if(EASTL_UNLIKELY(n >= kBitsPerWord))   // parent expected to handle high bits and n >= 64
			{
				mWord[1] = mWord[0];
				mWord[0] = 0;
				n -= kBitsPerWord;
			}

			mWord[1] = (mWord[1] << n) | (mWord[0] >> (kBitsPerWord - n)); // Intentionally use | instead of +.
			mWord[0] <<= n;
			// We let the parent class turn off any upper bits.
		}
	}


	template <typename WordType>
	inline void BitsetBase<2, WordType>::operator>>=(size_type n)
	{
		if(n) // to avoid a shift by kBitsPerWord, which is undefined
		{
			if(EASTL_UNLIKELY(n >= kBitsPerWord))   // parent expected to handle n >= 64
			{
				mWord[0] = mWord[1];
				mWord[1] = 0;
				n -= kBitsPerWord;
			}
			
			mWord[0] = (mWord[0] >> n) | (mWord[1] << (kBitsPerWord - n)); // Intentionally use | instead of +.
			mWord[1] >>= n;
		}
	}


	template <typename WordType>
	inline void BitsetBase<2, WordType>::flip()
	{
		mWord[0] = ~mWord[0];
		mWord[1] = ~mWord[1];
		// We let the parent class turn off any upper bits.
	}


	template <typename WordType>
	inline void BitsetBase<2, WordType>::set()
	{
		mWord[0] = ~static_cast<word_type>(0);
		mWord[1] = ~static_cast<word_type>(0);
		// We let the parent class turn off any upper bits.
	}


	template <typename WordType>
	inline void BitsetBase<2, WordType>::set(size_type i, bool value)
	{
		if(value)
			mWord[i >> kBitsPerWordShift] |=  (static_cast<word_type>(1) << (i & kBitsPerWordMask));
		else
			mWord[i >> kBitsPerWordShift] &= ~(static_cast<word_type>(1) << (i & kBitsPerWordMask));
	}


	template <typename WordType>
	inline void BitsetBase<2, WordType>::reset()
	{
		mWord[0] = 0;
		mWord[1] = 0;
	}


	template <typename WordType>
	inline bool BitsetBase<2, WordType>::operator==(const this_type& x) const
	{
		return (mWord[0] == x.mWord[0]) && (mWord[1] == x.mWord[1]);
	}


	template <typename WordType>
	inline bool BitsetBase<2, WordType>::any() const
	{
		// Or with two branches: { return (mWord[0] != 0) || (mWord[1] != 0); }
		return (mWord[0] | mWord[1]) != 0; 
	}

	template <typename WordType>
	inline typename BitsetBase<2, WordType>::size_type
	BitsetBase<2, WordType>::count() const
	{
		#if (defined(__GNUC__) && (((__GNUC__ * 100) + __GNUC_MINOR__) >= 304)) || defined(__clang__) // GCC 3.4 or later
			#if(EA_PLATFORM_WORD_SIZE == 4)
				return (size_type)__builtin_popcountl(mWord[0])  + (size_type)__builtin_popcountl(mWord[1]);
			#else
				return (size_type)__builtin_popcountll(mWord[0]) + (size_type)__builtin_popcountll(mWord[1]);
			#endif

		#else
			return BitsetCountBits(mWord[0]) + BitsetCountBits(mWord[1]);
		#endif
	}


	template <typename WordType>
	inline void BitsetBase<2, WordType>::from_uint32(uint32_t value)
	{
		mWord[0] = static_cast<word_type>(value);
		mWord[1] = 0;
	}


	template <typename WordType>
	inline void BitsetBase<2, WordType>::from_uint64(uint64_t value)
	{
		#if(EA_PLATFORM_WORD_SIZE == 4)
			mWord[0] = static_cast<word_type>(value);
			mWord[1] = static_cast<word_type>(value >> 32);
		#else
			mWord[0] = static_cast<word_type>(value);
			mWord[1] = 0;
		#endif
	}


	template <typename WordType>
	inline unsigned long BitsetBase<2, WordType>::to_ulong() const
	{
		#if EASTL_EXCEPTIONS_ENABLED
			if(mWord[1])
				throw std::overflow_error("BitsetBase::to_ulong");
		#endif
		return (unsigned long)mWord[0]; // Todo: We need to deal with the case whereby sizeof(word_type) < sizeof(unsigned long)
	}


	template <typename WordType>
	inline uint32_t BitsetBase<2, WordType>::to_uint32() const
	{
		#if EASTL_EXCEPTIONS_ENABLED
			// Verify that high words or bits are not set and thus that to_uint32 doesn't lose information.

			#if(EA_PLATFORM_WORD_SIZE == 4)
				if(mWord[1])
					throw std::overflow_error("BitsetBase::to_uint32");
			#else
				if(mWord[1] || (mWord[0] >> 32))
					throw std::overflow_error("BitsetBase::to_uint32");
			#endif
		#endif

		return (uint32_t)mWord[0];
	}


	template <typename WordType>
	inline uint64_t BitsetBase<2, WordType>::to_uint64() const
	{
		#if(EA_PLATFORM_WORD_SIZE == 4)
			// There can't possibly be an overflow_error here.

			return ((uint64_t)mWord[1] << 32) | mWord[0];
		#else
			#if EASTL_EXCEPTIONS_ENABLED
				if(mWord[1])
					throw std::overflow_error("BitsetBase::to_uint64");
			#endif

			return (uint64_t)mWord[0];
		#endif
	}


	template <typename WordType>
	inline typename BitsetBase<2, WordType>::word_type&
	BitsetBase<2, WordType>::DoGetWord(size_type i)
	{
		return mWord[i >> kBitsPerWordShift];
	}


	template <typename WordType>
	inline typename BitsetBase<2, WordType>::word_type
	BitsetBase<2, WordType>::DoGetWord(size_type i) const
	{
		return mWord[i >> kBitsPerWordShift];
	}


	template <typename WordType>
	inline typename BitsetBase<2, WordType>::size_type 
	BitsetBase<2, WordType>::DoFindFirst() const
	{
		size_type fbiw = GetFirstBit(mWord[0]);

		if(fbiw != kBitsPerWord)
			return fbiw;

		fbiw = GetFirstBit(mWord[1]);

		if(fbiw != kBitsPerWord)
			return kBitsPerWord + fbiw;

		return 2 * kBitsPerWord;
	}


	template <typename WordType>
	inline typename BitsetBase<2, WordType>::size_type 
	BitsetBase<2, WordType>::DoFindNext(size_type last_find) const
	{
		// If the last find was in the first word, we must check it and then possibly the second.
		if(++last_find < (size_type)kBitsPerWord)
		{
			// Mask off previous bits of word so our search becomes a "find first".
			word_type this_word = mWord[0] & ((~static_cast<word_type>(0)) << last_find);

			// Step through words.
			size_type fbiw = GetFirstBit(this_word);

			if(fbiw != kBitsPerWord)
				return fbiw;

			fbiw = GetFirstBit(mWord[1]);

			if(fbiw != kBitsPerWord)
				return kBitsPerWord + fbiw;
		}
		else if(last_find < (size_type)(2 * kBitsPerWord))
		{
			// The last find was in the second word, remove the bit count of the first word from the find.
			last_find -= kBitsPerWord;

			// Mask off previous bits of word so our search becomes a "find first".
			word_type this_word = mWord[1] & ((~static_cast<word_type>(0)) << last_find);

			const size_type fbiw = GetFirstBit(this_word);

			if(fbiw != kBitsPerWord)
				return kBitsPerWord + fbiw;
		}

		return 2 * kBitsPerWord;
	}


	template <typename WordType>
	inline typename BitsetBase<2, WordType>::size_type 
	BitsetBase<2, WordType>::DoFindLast() const
	{
		size_type lbiw = GetLastBit(mWord[1]);

		if(lbiw != kBitsPerWord)
			return kBitsPerWord + lbiw;

		lbiw = GetLastBit(mWord[0]);

		if(lbiw != kBitsPerWord)
			return lbiw;

		return 2 * kBitsPerWord;
	}


	template <typename WordType>
	inline typename BitsetBase<2, WordType>::size_type 
	BitsetBase<2, WordType>::DoFindPrev(size_type last_find) const
	{
		// If the last find was in the second word, we must check it and then possibly the first.
		if(last_find > (size_type)kBitsPerWord)
		{
			// This has the same effect as last_find %= kBitsPerWord in our case.
			last_find -= kBitsPerWord;

			// Mask off previous bits of word so our search becomes a "find first".
			word_type this_word = mWord[1] & ((~static_cast<word_type>(0)) >> (kBitsPerWord - last_find));

			// Step through words.
			size_type lbiw = GetLastBit(this_word);

			if(lbiw != kBitsPerWord)
				return kBitsPerWord + lbiw;

			lbiw = GetLastBit(mWord[0]);

			if(lbiw != kBitsPerWord)
				return lbiw;
		}
		else if(last_find != 0)
		{
			// Mask off previous bits of word so our search becomes a "find first".
			word_type this_word = mWord[0] & ((~static_cast<word_type>(0)) >> (kBitsPerWord - last_find));

			const size_type lbiw = GetLastBit(this_word);

			if(lbiw != kBitsPerWord)
				return lbiw;
		}

		return 2 * kBitsPerWord;
	}



	///////////////////////////////////////////////////////////////////////////
	// bitset::reference
	///////////////////////////////////////////////////////////////////////////

	template <size_t N, typename WordType>
	inline bitset<N, WordType>::reference::reference(const bitset& x, size_type i)
		: mpBitWord(&const_cast<bitset&>(x).DoGetWord(i)),
		  mnBitIndex(i & kBitsPerWordMask)
	{   // We have an issue here because the above is casting away the const-ness of the source bitset.
		// Empty
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::reference&
	bitset<N, WordType>::reference::operator=(bool value)
	{
		if(value)
			*mpBitWord |=  (static_cast<word_type>(1) << (mnBitIndex & kBitsPerWordMask));
		else
			*mpBitWord &= ~(static_cast<word_type>(1) << (mnBitIndex & kBitsPerWordMask));
		return *this;
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::reference&
	bitset<N, WordType>::reference::operator=(const reference& x)
	{
		if(*x.mpBitWord & (static_cast<word_type>(1) << (x.mnBitIndex & kBitsPerWordMask)))
			*mpBitWord |=  (static_cast<word_type>(1) << (mnBitIndex & kBitsPerWordMask));
		else
			*mpBitWord &= ~(static_cast<word_type>(1) << (mnBitIndex & kBitsPerWordMask));
		return *this;
	}


	template <size_t N, typename WordType>
	inline bool bitset<N, WordType>::reference::operator~() const
	{
		return (*mpBitWord & (static_cast<word_type>(1) << (mnBitIndex & kBitsPerWordMask))) == 0;
	}


	//Defined inline in the class because Metrowerks fails to be able to compile it here.
	//template <size_t N, typename WordType>
	//inline bitset<N, WordType>::reference::operator bool() const
	//{
	//    return (*mpBitWord & (static_cast<word_type>(1) << (mnBitIndex & kBitsPerWordMask))) != 0;
	//}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::reference&
	bitset<N, WordType>::reference::flip()
	{
		*mpBitWord ^= static_cast<word_type>(1) << (mnBitIndex & kBitsPerWordMask);
		return *this;
	}




	///////////////////////////////////////////////////////////////////////////
	// bitset
	///////////////////////////////////////////////////////////////////////////

	template <size_t N, typename WordType>
	inline bitset<N, WordType>::bitset()
		: base_type()
	{
		// Empty. The base class will set all bits to zero.
	}

	EA_DISABLE_VC_WARNING(6313)
	template <size_t N, typename WordType>
	inline bitset<N, WordType>::bitset(uint32_t value)
		: base_type(value)
	{
		if((N & kBitsPerWordMask) || (N == 0)) // If there are any high bits to clear... (If we didn't have this check, then the code below would do the wrong thing when N == 32.
			mWord[kWordCount - 1] &= ~(static_cast<word_type>(~static_cast<word_type>(0)) << (N & kBitsPerWordMask)); // This clears any high unused bits.
	}
	EA_RESTORE_VC_WARNING()

	/*
	template <size_t N, typename WordType>
	inline bitset<N, WordType>::bitset(uint64_t value)
		: base_type(value)
	{
		if((N & kBitsPerWordMask) || (N == 0)) // If there are any high bits to clear...
			mWord[kWordCount - 1] &= ~(~static_cast<word_type>(0) << (N & kBitsPerWordMask)); // This clears any high unused bits.
	}
	*/


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::this_type&
	bitset<N, WordType>::operator&=(const this_type& x)
	{
		base_type::operator&=(x);
		return *this;
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::this_type&
	bitset<N, WordType>::operator|=(const this_type& x)
	{
		base_type::operator|=(x);
		return *this;
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::this_type&
	bitset<N, WordType>::operator^=(const this_type& x)
	{
		base_type::operator^=(x);
		return *this;
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::this_type&
	bitset<N, WordType>::operator<<=(size_type n)
	{
		if(EASTL_LIKELY((intptr_t)n < (intptr_t)N))
		{
			EA_DISABLE_VC_WARNING(6313)
			base_type::operator<<=(n);
			if((N & kBitsPerWordMask) || (N == 0)) // If there are any high bits to clear... (If we didn't have this check, then the code below would do the wrong thing when N == 32.
				mWord[kWordCount - 1] &= ~(static_cast<word_type>(~static_cast<word_type>(0)) << (N & kBitsPerWordMask)); // This clears any high unused bits. We need to do this so that shift operations proceed correctly.
			EA_RESTORE_VC_WARNING()
		}
		else
			base_type::reset();
		return *this;
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::this_type&
	bitset<N, WordType>::operator>>=(size_type n)
	{
		if(EASTL_LIKELY(n < N))
			base_type::operator>>=(n);
		else
			base_type::reset();
		return *this;
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::this_type&
	bitset<N, WordType>::set()
	{
		base_type::set(); // This sets all bits.
		if((N & kBitsPerWordMask) || (N == 0)) // If there are any high bits to clear... (If we didn't have this check, then the code below would do the wrong thing when N == 32.
			mWord[kWordCount - 1] &= ~(static_cast<word_type>(~static_cast<word_type>(0)) << (N & kBitsPerWordMask)); // This clears any high unused bits. We need to do this so that shift operations proceed correctly.
		return *this;
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::this_type&
	bitset<N, WordType>::set(size_type i, bool value)
	{
		if(i < N)
			base_type::set(i, value);
		else
		{
			#if EASTL_ASSERT_ENABLED
				if(EASTL_UNLIKELY(!(i < N)))
					EASTL_FAIL_MSG("bitset::set -- out of range");
			#endif

			#if EASTL_EXCEPTIONS_ENABLED
				throw std::out_of_range("bitset::set");
			#endif
		}

		return *this;
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::this_type&
	bitset<N, WordType>::reset()
	{
		base_type::reset();
		return *this;
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::this_type&
	bitset<N, WordType>::reset(size_type i)
	{
		if(EASTL_LIKELY(i < N))
			DoGetWord(i) &= ~(static_cast<word_type>(1) << (i & kBitsPerWordMask));
		else
		{
			#if EASTL_ASSERT_ENABLED
				if(EASTL_UNLIKELY(!(i < N)))
					EASTL_FAIL_MSG("bitset::reset -- out of range");
			#endif

			#if EASTL_EXCEPTIONS_ENABLED
				throw std::out_of_range("bitset::reset");
			#endif
		}

		return *this;
	}

		
	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::this_type&
	bitset<N, WordType>::flip()
	{
		EA_DISABLE_VC_WARNING(6313)
		base_type::flip();
		if((N & kBitsPerWordMask) || (N == 0)) // If there are any high bits to clear... (If we didn't have this check, then the code below would do the wrong thing when N == 32.
			mWord[kWordCount - 1] &= ~(static_cast<word_type>(~static_cast<word_type>(0)) << (N & kBitsPerWordMask)); // This clears any high unused bits. We need to do this so that shift operations proceed correctly.
		return *this;
		EA_RESTORE_VC_WARNING()
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::this_type&
	bitset<N, WordType>::flip(size_type i)
	{
		if(EASTL_LIKELY(i < N))
			DoGetWord(i) ^= (static_cast<word_type>(1) << (i & kBitsPerWordMask));
		else
		{
			#if EASTL_ASSERT_ENABLED
				if(EASTL_UNLIKELY(!(i < N)))
					EASTL_FAIL_MSG("bitset::flip -- out of range");
			#endif

			#if EASTL_EXCEPTIONS_ENABLED
				throw std::out_of_range("bitset::flip");
			#endif
		}
		return *this;
	}
		

	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::this_type
	bitset<N, WordType>::operator~() const
	{
		return this_type(*this).flip();
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::reference
	bitset<N, WordType>::operator[](size_type i)
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(!(i < N)))
				EASTL_FAIL_MSG("bitset::operator[] -- out of range");
		#endif

		return reference(*this, i);
	}


	template <size_t N, typename WordType>
	inline bool bitset<N, WordType>::operator[](size_type i) const
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(!(i < N)))
				EASTL_FAIL_MSG("bitset::operator[] -- out of range");
		#endif

		return (DoGetWord(i) & (static_cast<word_type>(1) << (i & kBitsPerWordMask))) != 0;
	}


	template <size_t N, typename WordType>
	inline const typename bitset<N, WordType>::word_type* bitset<N, WordType>::data() const
	{
		return base_type::mWord;
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::word_type* bitset<N, WordType>::data()
	{
		return base_type::mWord;
	}


	template <size_t N, typename WordType>
	inline void bitset<N, WordType>::from_uint32(uint32_t value)
	{
		base_type::from_uint32(value);

		if((N & kBitsPerWordMask) || (N == 0)) // If there are any high bits to clear... (If we didn't have this check, then the code below would do the wrong thing when N == 32.
			mWord[kWordCount - 1] &= ~(static_cast<word_type>(~static_cast<word_type>(0)) << (N & kBitsPerWordMask)); // This clears any high unused bits. We need to do this so that shift operations proceed correctly.
	}


	template <size_t N, typename WordType>
	inline void bitset<N, WordType>::from_uint64(uint64_t value)
	{
		base_type::from_uint64(value);

		if((N & kBitsPerWordMask) || (N == 0)) // If there are any high bits to clear... (If we didn't have this check, then the code below would do the wrong thing when N == 32.
			mWord[kWordCount - 1] &= ~(static_cast<word_type>(~static_cast<word_type>(0)) << (N & kBitsPerWordMask)); // This clears any high unused bits. We need to do this so that shift operations proceed correctly.
	}


	// template <size_t N, typename WordType>
	// inline unsigned long bitset<N, WordType>::to_ulong() const
	// {
	//     return base_type::to_ulong();
	// }


	// template <size_t N, typename WordType>
	// inline uint32_t bitset<N, WordType>::to_uint32() const
	// {
	//     return base_type::to_uint32();
	// }


	// template <size_t N, typename WordType>
	// inline uint64_t bitset<N, WordType>::to_uint64() const
	// {
	//     return base_type::to_uint64();
	// }


	// template <size_t N, typename WordType>
	// inline typename bitset<N, WordType>::size_type
	// bitset<N, WordType>::count() const
	// {
	//     return base_type::count();
	// }


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::size_type
	bitset<N, WordType>::size() const
	{
		return (size_type)N;
	}


	template <size_t N, typename WordType>
	inline bool bitset<N, WordType>::operator==(const this_type& x) const
	{
		return base_type::operator==(x);
	}


	template <size_t N, typename WordType>
	inline bool bitset<N, WordType>::operator!=(const this_type& x) const
	{
		return !base_type::operator==(x);
	}


	template <size_t N, typename WordType>
	inline bool bitset<N, WordType>::test(size_type i) const
	{
		if(EASTL_UNLIKELY(i < N))
			return (DoGetWord(i) & (static_cast<word_type>(1) << (i & kBitsPerWordMask))) != 0;

		#if EASTL_ASSERT_ENABLED
			EASTL_FAIL_MSG("bitset::test -- out of range");
		#endif

		#if EASTL_EXCEPTIONS_ENABLED
			throw std::out_of_range("bitset::test");
		#else
			return false;
		#endif
	}


	// template <size_t N, typename WordType>
	// inline bool bitset<N, WordType>::any() const
	// {
	//     return base_type::any();
	// }


	template <size_t N, typename WordType>
	inline bool bitset<N, WordType>::all() const
	{
		return count() == size();
	}


	template <size_t N, typename WordType>
	inline bool bitset<N, WordType>::none() const
	{
		return !base_type::any();
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::this_type
	bitset<N, WordType>::operator<<(size_type n) const
	{
		return this_type(*this).operator<<=(n);
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::this_type
	bitset<N, WordType>::operator>>(size_type n) const
	{
		return this_type(*this).operator>>=(n);
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::size_type
	bitset<N, WordType>::find_first() const
	{
		const size_type i = base_type::DoFindFirst();

		if(i < kSize)
			return i;
		// Else i could be the base type bit count, so we clamp it to our size.

		return kSize;
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::size_type
	bitset<N, WordType>::find_next(size_type last_find) const
	{
		const size_type i = base_type::DoFindNext(last_find);

		if(i < kSize)
			return i;
		// Else i could be the base type bit count, so we clamp it to our size.

		return kSize;
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::size_type
	bitset<N, WordType>::find_last() const
	{
		const size_type i = base_type::DoFindLast();

		if(i < kSize)
			return i;
		// Else i could be the base type bit count, so we clamp it to our size.

		return kSize;
	}


	template <size_t N, typename WordType>
	inline typename bitset<N, WordType>::size_type
	bitset<N, WordType>::find_prev(size_type last_find) const
	{
		const size_type i = base_type::DoFindPrev(last_find);

		if(i < kSize)
			return i;
		// Else i could be the base type bit count, so we clamp it to our size.

		return kSize;
	}



	///////////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////////

	template <size_t N, typename WordType>
	inline bitset<N, WordType> operator&(const bitset<N, WordType>& a, const bitset<N, WordType>& b)
	{
		// We get betting inlining when we don't declare temporary variables.
		return bitset<N, WordType>(a).operator&=(b);
	}


	template <size_t N, typename WordType>
	inline bitset<N, WordType> operator|(const bitset<N, WordType>& a, const bitset<N, WordType>& b)
	{
		return bitset<N, WordType>(a).operator|=(b);
	}


	template <size_t N, typename WordType>
	inline bitset<N, WordType> operator^(const bitset<N, WordType>& a, const bitset<N, WordType>& b)
	{
		return bitset<N, WordType>(a).operator^=(b);
	}


} // namespace eastl


EA_RESTORE_VC_WARNING();

#endif // Header include guard
