/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements a bitset much like the C++ std::bitset class. 
// The primary distinctions between this bitset and std::bitset are:
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
	/// Note: for nBitCount == 0, returns 1!
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

	template <size_t N, typename WordType = EASTL_BITSET_WORD_TYPE_DEFAULT>
	class bitset;

	namespace detail
	{
		template<typename T>
		struct is_word_type : std::bool_constant<!is_const_v<T> && !is_volatile_v<T> && !is_same_v<T, bool> && is_integral_v<T> && is_unsigned_v<T>> {};

		template<typename T>
		constexpr bool is_word_type_v = is_word_type<T>::value;

		// slices the min(N, UInt) lowest significant bits from value.
		template<size_t N, typename WordType, typename UInt>
		eastl::enable_if_t<is_word_type_v<UInt>> from_unsigned_integral(bitset<N, WordType>& bs, UInt value)
		{
			constexpr size_t numWords = (N > 0) ? ((N - 1) / (CHAR_BIT * sizeof(WordType)) + 1) : 0; // BITSET_WORD_COUNT(N, WordType) but 0 for N == 0

			WordType* data = bs.data();

			EA_CONSTEXPR_IF (numWords > 0)
			{
				// copy everything from value into our word array:
				constexpr size_t bytes_to_copy = eastl::min_alt(numWords * sizeof(WordType), sizeof(UInt));
				memcpy(data, &value, bytes_to_copy);

				// zero any remaining elements in our array:
				memset(reinterpret_cast<unsigned char*>(data) + bytes_to_copy, 0, numWords * sizeof(WordType) - bytes_to_copy);

				// we may have copied bits into the final element that are unusable (ie. bit positions > N).
				// zero these bits out, as this is an invariant for our implementation.
				EA_CONSTEXPR_IF (N % (CHAR_BIT * sizeof(WordType)) != 0)
				{
					constexpr WordType lastElemUsedBitsMask = (WordType(1) << (N % (CHAR_BIT * sizeof(WordType)))) - 1;
					data[numWords - 1] &= lastElemUsedBitsMask;
				}
			}
			else
			{
				data[0] = 0; // our bitset implementation has a single element even when N == 0.
			}
		}

		template<typename UInt, bool bAssertOnOverflow, size_t N, typename WordType>
		eastl::enable_if_t<is_word_type_v<UInt>, UInt> to_unsigned_integral(const bitset<N, WordType>& bs)
		{
			constexpr size_t numWords = (N > 0) ? ((N - 1) / (CHAR_BIT * sizeof(WordType)) + 1) : 0; // BITSET_WORD_COUNT(N, WordType) but 0 for N == 0

			EA_CONSTEXPR_IF (numWords > 0)
			{
				const WordType* data = bs.data();

				UInt result = 0;

				size_t numWordsCopied;
				EA_CONSTEXPR_IF (sizeof(UInt) < sizeof(WordType))
				{
					constexpr size_t bytes_to_copy = sizeof(UInt);
					memcpy(&result, data, bytes_to_copy);

					// check remaining uncopied bits from the first word are zero:
					constexpr WordType lastElemOverflowBitsMask = static_cast<WordType>(~((WordType(1) << (CHAR_BIT * sizeof(UInt))) - 1));
					if ((data[0] & lastElemOverflowBitsMask) != 0)
					{
#if EASTL_EXCEPTIONS_ENABLED
						throw std::overflow_error("target type cannot represent the full bitset.");
#elif EASTL_ASSERT_ENABLED
						EA_CONSTEXPR_IF(bAssertOnOverflow)
							EASTL_FAIL_MSG("overflow_error");
#endif
					}

					numWordsCopied = 1;
				}
				else
				{
					constexpr size_t bytes_to_copy = eastl::min_alt(numWords * sizeof(WordType), sizeof(UInt));
					memcpy(&result, data, bytes_to_copy);

					numWordsCopied = bytes_to_copy / sizeof(WordType);
				}
				
				// check any remaining uncopied words are zero (don't contain any useful information).
				for (size_t wordIndex = numWordsCopied; wordIndex < numWords; ++wordIndex)
				{
					if (data[wordIndex] != 0)
					{
#if EASTL_EXCEPTIONS_ENABLED
						throw std::overflow_error("target type cannot represent the full bitset.");
#elif EASTL_ASSERT_ENABLED
						EA_CONSTEXPR_IF (bAssertOnOverflow)
							EASTL_FAIL_MSG("overflow_error");
#endif
					}
				}

				return result;
			}
			else
			{
				return 0;
			}
		}
	} // namespace detail

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
		// invariant: we keep any high bits in the last word that are unneeded set to 0
		// so that our to_ulong() conversion can simply copy the words into the target type.
		word_type mWord[NW];

	public:
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
	/// - WordType must be a non-cv qualified unsigned integral other than bool.
	///   By default the WordType is the largest native register type that the
	///   target platform supports.
	///
	template <size_t N, typename WordType>
	class bitset : private BitsetBase<BITSET_WORD_COUNT(N, WordType), WordType>
	{
	public:
		static_assert(detail::is_word_type_v<WordType>, "Word type must be a non-cv qualified, unsigned integral other than bool.");

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

		// internal implementation details. do not use.
		using base_type::mWord;
		using base_type::DoGetWord;
		using base_type::DoFindFirst;
		using base_type::DoFindNext;
		using base_type::DoFindLast;
		using base_type::DoFindPrev;

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

#if EA_IS_ENABLED(EASTL_DEPRECATIONS_FOR_2024_SEPT)
		// note: this constructor will only copy the minimum of N or unsigned long long's size least significant bits.
		bitset(unsigned long long value);
#else
		bitset(uint32_t value);
#endif

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

		// Deprecated: use the bitset(unsigned long long) constructor instead.
		// this was a workaround for when our constructor was defined as bitset(uint32_t) and could cause a narrowing conversion.
		EASTL_REMOVE_AT_2024_SEPT void          from_uint32(uint32_t value);
		EASTL_REMOVE_AT_2024_SEPT void          from_uint64(uint64_t value);

		/// to_xxx()
		/// 
		/// Not recommended: Use one of
		///   as_xxx() which is a compile time error if the target type cannot represent the entire bitset, or
		///   to_xxx_assert_convertible() which is the standard conformant version of this function, or
		///   to_xxx_no_assert_convertible() which has the same behaviour, explicit naming
		/// 
		/// Different from the standard:
		/// Does *NOT* assert that the bitset can be represented as the target integer type (has bits set outside the target type).
		/// However, if exceptions are enabled, it does throw an exception if the bitset cannot be represented as the target integer type.
		unsigned long to_ulong()  const;
		uint32_t      to_uint32() const;
		uint64_t      to_uint64() const;

		/// to_xxx_assert_convertible()
		///
		/// Equivalent to the standard library's to_ulong() / to_ullong().
		/// Asserts / throws an exception if the bitset cannot be represented as the target integer type.
		uint32_t			to_uint32_assert_convertible() const { return detail::to_unsigned_integral<uint32_t, true>(*this); }
		uint64_t			to_uint64_assert_convertible() const { return detail::to_unsigned_integral<uint64_t, true>(*this); }
		unsigned long		to_ulong_assert_convertible()  const { return detail::to_unsigned_integral<unsigned long, true>(*this); }
		unsigned long long	to_ullong_assert_convertible() const { return detail::to_unsigned_integral<unsigned long long, true>(*this); }

		/// to_xxx_no_assert_convertible()
		///
		/// Prefer to_xxx_assert_convertible() instead of these functions.
		/// 
		/// Different from the standard:
		/// Does *NOT* assert that the bitset can be represented as the target integer type (has bits set outside the target type).
		/// However, if exceptions are enabled, it does throw an exception if the bitset cannot be represented as the target integer type.
		uint32_t			to_uint32_no_assert_convertible() const { return detail::to_unsigned_integral<uint32_t, false>(*this); }
		uint64_t			to_uint64_no_assert_convertible() const { return detail::to_unsigned_integral<uint64_t, false>(*this); }
		unsigned long		to_ulong_no_assert_convertible()  const { return detail::to_unsigned_integral<unsigned long, false>(*this); }
		unsigned long long	to_ullong_no_assert_convertible() const { return detail::to_unsigned_integral<unsigned long long, false>(*this); }

		/// as_uint<UInt>() / as_xxx()
		/// 
		/// Extension to the standard: Cast to a unsigned integral that can represent the entire bitset.
		/// If the target type cannot represent the entire bitset, then issue a compile error (overload does not exist).
		/// Never throws / asserts.
		template<typename UInt>
		eastl::enable_if_t<detail::is_word_type_v<UInt> && N <= (CHAR_BIT * sizeof(UInt)), UInt>	as_uint() const noexcept { return detail::to_unsigned_integral<UInt, true>(*this); }

		template<size_t NumBits = N>
		eastl::enable_if_t<NumBits <= (CHAR_BIT * sizeof(uint32_t)), uint32_t>						as_uint32() const noexcept { return to_uint32_assert_convertible(); }
		template<size_t NumBits = N>
		eastl::enable_if_t<NumBits <= (CHAR_BIT * sizeof(uint64_t)), uint64_t>						as_uint64() const noexcept { return to_uint64_assert_convertible(); }
		template<size_t NumBits = N>
		eastl::enable_if_t<NumBits <= (CHAR_BIT * sizeof(unsigned long)), unsigned long>			as_ulong() const noexcept { return to_ulong_assert_convertible(); }
		template<size_t NumBits = N>
		eastl::enable_if_t<NumBits <= (CHAR_BIT * sizeof(unsigned long long)), unsigned long long>	as_ullong() const noexcept { return to_ullong_assert_convertible(); }

	  //size_type count() const;            // We inherit this from the base class.
		size_type size() const;

		bool operator==(const this_type& x) const;
#if !defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
		bool operator!=(const this_type& x) const;
#endif

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
	template<typename UInt64>
	eastl::enable_if_t<detail::is_word_type_v<UInt64> && sizeof(UInt64) == 8, uint32_t> BitsetCountBits(UInt64 x)
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

	template<typename UInt32>
	eastl::enable_if_t<detail::is_word_type_v<UInt32> && sizeof(UInt32) == 4, uint32_t> BitsetCountBits(UInt32 x)
	{
		x = x - ((x >> 1) & 0x55555555);
		x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
		x = (x + (x >> 4)) & 0x0F0F0F0F;
		return (uint32_t)((x * 0x01010101) >> 24);
	}

	template<typename SmallUInt>
	eastl::enable_if_t< detail::is_word_type_v<SmallUInt> && sizeof(SmallUInt) < 4, uint32_t> BitsetCountBits(SmallUInt x)
	{
		return BitsetCountBits((uint32_t)x);
	}


	// const static char kBitsPerUint16[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
	#define EASTL_BITSET_COUNT_STRING "\0\1\1\2\1\2\2\3\1\2\2\3\2\3\3\4"


	template<typename UInt8>
	eastl::enable_if_t<detail::is_word_type_v<UInt8> && sizeof(UInt8) == 1, uint32_t> GetFirstBit(UInt8 x)
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

	// To do: Update this to use VC++ _BitScanForward, _BitScanForward64;
	// GCC __builtin_ctz, __builtin_ctzl.
	// VC++ __lzcnt16, __lzcnt, __lzcnt64 requires recent CPUs (2013+) and probably can't be used.
	// http://en.wikipedia.org/wiki/Haswell_%28microarchitecture%29#New_features
	template<typename UInt16>
	eastl::enable_if_t<detail::is_word_type_v<UInt16> && sizeof(UInt16) == 2, uint32_t> GetFirstBit(UInt16 x)
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

	template<typename UInt32>
	eastl::enable_if_t<detail::is_word_type_v<UInt32> && sizeof(UInt32) == 4, uint32_t> GetFirstBit(UInt32 x)
	{
#if defined(EA_COMPILER_MSVC) && (defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64))
		// This has been benchmarked as significantly faster than the generic code below.
		unsigned char isNonZero;
		unsigned long index;
		isNonZero = _BitScanForward(&index, x);
		return isNonZero ? (int)index : 32;
#elif (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)) && !defined(EA_COMPILER_EDG)
		if (x)
			return __builtin_ctz(x);
		return 32;
#else
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
#endif
	}

	template<typename UInt64>
	eastl::enable_if_t<detail::is_word_type_v<UInt64> && sizeof(UInt64) == 8, uint32_t> GetFirstBit(UInt64 x)
	{
#if defined(EA_COMPILER_MSVC) && defined(EA_PROCESSOR_X86_64)
		// This has been benchmarked as significantly faster than the generic code below.
		unsigned char isNonZero;
		unsigned long index;
		isNonZero = _BitScanForward64(&index, x);
		return isNonZero ? (int)index : 64;
#elif (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)) && !defined(EA_COMPILER_EDG)
		if (x)
			return __builtin_ctzll(x);
		return 64;
#else
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
#endif
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

	template<typename UInt8>
	eastl::enable_if_t<detail::is_word_type_v<UInt8> && sizeof(UInt8) == 1, uint32_t> GetLastBit(UInt8 x)
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

	template<typename UInt16>
	eastl::enable_if_t<detail::is_word_type_v<UInt16> && sizeof(UInt16) == 2, uint32_t> GetLastBit(UInt16 x)
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

	template<typename UInt32>
	eastl::enable_if_t<detail::is_word_type_v<UInt32> && sizeof(UInt32) == 4, uint32_t> GetLastBit(UInt32 x)
	{
#if defined(EA_COMPILER_MSVC) && (defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64))
		// This has been benchmarked as significantly faster than the generic code below.
		unsigned char isNonZero;
		unsigned long index;
		isNonZero = _BitScanReverse(&index, x);
		return isNonZero ? (int)index : 32;
#elif (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)) && !defined(EA_COMPILER_EDG)
		if (x)
			return 31 - __builtin_clz(x);
		return 32;
#else
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
#endif
	}

	template<typename UInt64>
	eastl::enable_if_t<detail::is_word_type_v<UInt64> && sizeof(UInt64) == 8, uint32_t> GetLastBit(UInt64 x)
	{
#if defined(EA_COMPILER_MSVC) && defined(EA_PROCESSOR_X86_64)
		// This has been benchmarked as significantly faster than the generic code below.
		unsigned char isNonZero;
		unsigned long index;
		isNonZero = _BitScanReverse64(&index, x);
		return isNonZero ? (int)index : 64;
#elif (defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)) && !defined(EA_COMPILER_EDG)
		if (x)
			return 63 - __builtin_clzll(x);
		return 64;
#else
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
#endif
	}

	#if EASTL_INT128_SUPPORTED
		inline uint32_t GetLastBit(eastl_uint128_t x)
		{
			if(x)
			{
				uint32_t n = 0;
				
				eastl_uint128_t mask(UINT64_C(0xFFFFFFFFFFFFFFFF)); // There doesn't seem to exist compiler support for INT128_C() by any compiler. EAStdC's int128_t supports it though.
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
			word_type this_word = mWord[word_index] & (static_cast<word_type>(~0) << bit_index);

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
			// We do two shifts here because it's undefined behaviour to right shift greater than or equal to the number of bits in the integer.
			// 
			// Note: operator~() is an arithmetic operator and performs integral promotions, ie. small integrals are promoted to an int.
			// Because the promotion is before applying operator~() we need to cast back to our word type otherwise we end up with extraneous set bits.
			word_type mask      = (static_cast<word_type>(~static_cast<word_type>(0)) >> (kBitsPerWord - 1 - bit_index)) >> 1;
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
			const word_type this_word = mWord[0] & (static_cast<word_type>(~0) << last_find);

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
			const word_type this_word = mWord[0] & (static_cast<word_type>(~static_cast<word_type>(0)) >> (kBitsPerWord - last_find));

			return GetLastBit(this_word);
		}

		return kBitsPerWord;
	}




	///////////////////////////////////////////////////////////////////////////
	// BitsetBase<2, WordType>
	///////////////////////////////////////////////////////////////////////////


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
		EA_DISABLE_VC_WARNING(4245); // '=': conversion from 'int' to 'unsigned short', signed/unsigned mismatch 
		// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4245?view=msvc-170
		// MSVC incorrectly believes 0 is a negative value.
		mWord[0] = ~static_cast<word_type>(0);
		mWord[1] = ~static_cast<word_type>(0);
		EA_RESTORE_VC_WARNING();
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
			word_type this_word = mWord[0] & (static_cast<word_type>(~0) << last_find);

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
			word_type this_word = mWord[1] & (static_cast<word_type>(~0) << last_find);

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
			word_type this_word = mWord[1] & (static_cast<word_type>(~static_cast<word_type>(0)) >> (kBitsPerWord - last_find));

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
			word_type this_word = mWord[0] & (static_cast<word_type>(~static_cast<word_type>(0)) >> (kBitsPerWord - last_find));

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
	{
		reset();
	}

	EA_DISABLE_VC_WARNING(6313)
#if EA_IS_ENABLED(EASTL_DEPRECATIONS_FOR_2024_SEPT)
	template <size_t N, typename WordType>
	inline bitset<N, WordType>::bitset(unsigned long long value)
	{
		detail::from_unsigned_integral(*this, value);
	}
#else
	template <size_t N, typename WordType>
	inline bitset<N, WordType>::bitset(uint32_t value)
	{
		detail::from_unsigned_integral(*this, value);
	}
#endif
	EA_RESTORE_VC_WARNING()


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
		detail::from_unsigned_integral(*this, value);
	}


	template <size_t N, typename WordType>
	inline void bitset<N, WordType>::from_uint64(uint64_t value)
	{
		detail::from_unsigned_integral(*this, value);
	}


	template <size_t N, typename WordType>
	inline unsigned long bitset<N, WordType>::to_ulong() const
	{
		return detail::to_unsigned_integral<unsigned long, false>(*this);
	}


	template <size_t N, typename WordType>
	inline uint32_t bitset<N, WordType>::to_uint32() const
	{
		return detail::to_unsigned_integral<uint32_t, false>(*this);
	}


	template <size_t N, typename WordType>
	inline uint64_t bitset<N, WordType>::to_uint64() const
	{
		return detail::to_unsigned_integral<uint64_t, false>(*this);
	}


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

#if !defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	template <size_t N, typename WordType>
	inline bool bitset<N, WordType>::operator!=(const this_type& x) const
	{
		return !base_type::operator==(x);
	}
#endif

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
