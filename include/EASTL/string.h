///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Implements a basic_string class, much like the C++ std::basic_string.
// The primary distinctions between basic_string and std::basic_string are:
//    - basic_string has a few extension functions that allow for increased performance.
//    - basic_string has a few extension functions that make use easier,
//      such as a member sprintf function and member tolower/toupper functions.
//    - basic_string supports debug memory naming natively.
//    - basic_string is easier to read, debug, and visualize.
//    - basic_string internally manually expands basic functions such as begin(),
//      size(), etc. in order to improve debug performance and optimizer success.
//    - basic_string is savvy to an environment that doesn't have exception handling,
//      as is sometimes the case with console or embedded environments.
//    - basic_string has less deeply nested function calls and allows the user to
//      enable forced inlining in debug builds in order to reduce bloat.
//    - basic_string doesn't use char traits. As a result, EASTL assumes that
//      strings will hold characters and not exotic things like widgets. At the
//      very least, basic_string assumes that the value_type is a POD.
//    - basic_string::size_type is defined as eastl_size_t instead of size_t in
//      order to save memory and run faster on 64 bit systems.
//    - basic_string data is guaranteed to be contiguous.
//    - basic_string data is guaranteed to be 0-terminated, and the c_str() function
//      is guaranteed to return the same pointer as the data() which is guaranteed
//      to be the same value as &string[0].
//    - basic_string has a set_capacity() function which frees excess capacity.
//      The only way to do this with std::basic_string is via the cryptic non-obvious
//      trick of using: basic_string<char>(x).swap(x);
//    - basic_string has a force_size() function, which unilaterally moves the string
//      end position (mpEnd) to the given location. Useful for when the user writes
//      into the string via some external means such as C strcpy or sprintf.
//    - basic_string substr() deviates from the standard and returns a string with
//		a copy of this->get_allocator()
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Copy on Write (cow)
//
// This string implementation does not do copy on write (cow). This is by design,
// as cow penalizes 95% of string uses for the benefit of only 5% of the uses
// (these percentages are qualitative, not quantitative). The primary benefit of
// cow is that it allows for the sharing of string data between two string objects.
// Thus if you say this:
//    string a("hello");
//    string b(a);
// the "hello" will be shared between a and b. If you then say this:
//    a = "world";
// then a will release its reference to "hello" and leave b with the only reference
// to it. Normally this functionality is accomplished via reference counting and
// with atomic operations or mutexes.
//
// The C++ standard does not say anything about basic_string and cow. However,
// for a basic_string implementation to be standards-conforming, a number of
// issues arise which dictate some things about how one would have to implement
// a cow string. The discussion of these issues will not be rehashed here, as you
// can read the references below for better detail than can be provided in the
// space we have here. However, we can say that the C++ standard is sensible and
// that anything we try to do here to allow for an efficient cow implementation
// would result in a generally unacceptable string interface.
//
// The disadvantages of cow strings are:
//    - A reference count needs to exist with the string, which increases string memory usage.
//    - With thread safety, atomic operations and mutex locks are expensive, especially
//      on weaker memory systems such as console gaming platforms.
//    - All non-const string accessor functions need to do a sharing check then the
//      first such check needs to detach the string. Similarly, all string assignments
//      need to do a sharing check as well. If you access the string before doing an
//      assignment, the assignment doesn't result in a shared string, because the string
//      has already been detached.
//    - String sharing doesn't happen the large majority of the time. In some cases,
//      the total sum of the reference count memory can exceed any memory savings
//      gained by the strings that share representations.
//
// The addition of a string_cow class is under consideration for this library.
// There are conceivably some systems which have string usage patterns which would
// benefit from cow sharing. Such functionality is best saved for a separate string
// implementation so that the other string uses aren't penalized.
//
// References:
//    This is a good starting HTML reference on the topic:
//       http://www.gotw.ca/publications/optimizations.htm
//    Here is a Usenet discussion on the topic:
//       http://groups-beta.google.com/group/comp.lang.c++.moderated/browse_thread/thread/3dc6af5198d0bf7/886c8642cb06e03d
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_STRING_H
#define EASTL_STRING_H

#include <EASTL/internal/config.h>
#include <EASTL/allocator.h>
#include <EASTL/iterator.h>
#include <EASTL/algorithm.h>
#include <EASTL/initializer_list.h>
#include <EASTL/bonus/compressed_pair.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <stddef.h>             // size_t, ptrdiff_t, etc.
#include <stdarg.h>             // vararg functionality.

#include <stdlib.h>             // malloc, free.
#include <stdio.h>              // snprintf, etc.
#include <ctype.h>              // toupper, etc.

EA_DISABLE_GCC_WARNING(-Wtype-limits)
#include <wchar.h>
EA_RESTORE_GCC_WARNING()

#include <string.h> // strlen, etc.

#if EASTL_EXCEPTIONS_ENABLED
	#include <stdexcept> // std::out_of_range, std::length_error.
#endif
EA_RESTORE_ALL_VC_WARNINGS()


// 4530 - C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
// 4480 - nonstandard extension used: specifying underlying type for enum
// 4571 - catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught.
// 4267 - 'argument' : conversion from 'size_t' to 'const uint32_t', possible loss of data. This is a bogus warning resulting from a bug in VC++.
// 4702 - unreachable code
EA_DISABLE_VC_WARNING(4530 4480 4571 4267 4702);


#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif


#include <EASTL/internal/char_traits.h>
#include <EASTL/string_view.h>

///////////////////////////////////////////////////////////////////////////////
// EASTL_STRING_EXPLICIT
//
// See EASTL_STRING_OPT_EXPLICIT_CTORS for documentation.
//
#if EASTL_STRING_OPT_EXPLICIT_CTORS
	#define EASTL_STRING_EXPLICIT explicit
#else
	#define EASTL_STRING_EXPLICIT
#endif
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Vsnprintf
//
// The user is expected to supply these functions one way or another. Note that
// these functions are expected to accept parameters as per the C99 standard.
// These functions can deal with C99 standard return values or Microsoft non-standard
// return values but act more efficiently if implemented via the C99 style.
//
// In the case of EASTL_EASTDC_VSNPRINTF == 1, the user is expected to either
// link EAStdC or provide the functions below that act the same. In the case of
// EASTL_EASTDC_VSNPRINTF == 0, the user is expected to provide the function
// implementations, and may simply use C vsnprintf if desired, though it's not
// completely portable between compilers.
//
#if EASTL_EASTDC_VSNPRINTF
	namespace EA
	{
		namespace StdC
		{
			// Provided by the EAStdC package or by the user.
			EASTL_EASTDC_API int Vsnprintf(char*  EA_RESTRICT pDestination, size_t n, const char*  EA_RESTRICT pFormat, va_list arguments);
			EASTL_EASTDC_API int Vsnprintf(char16_t* EA_RESTRICT pDestination, size_t n, const char16_t* EA_RESTRICT pFormat, va_list arguments);
			EASTL_EASTDC_API int Vsnprintf(char32_t* EA_RESTRICT pDestination, size_t n, const char32_t* EA_RESTRICT pFormat, va_list arguments);
			#if EA_CHAR8_UNIQUE
				EASTL_EASTDC_API int Vsnprintf(char8_t*  EA_RESTRICT pDestination, size_t n, const char8_t*  EA_RESTRICT pFormat, va_list arguments);
			#endif
			#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
				EASTL_EASTDC_API int Vsnprintf(wchar_t* EA_RESTRICT pDestination, size_t n, const wchar_t* EA_RESTRICT pFormat, va_list arguments);
			#endif
		}
	}

	namespace eastl
	{
		inline int Vsnprintf(char* EA_RESTRICT pDestination, size_t n, const char* EA_RESTRICT pFormat, va_list arguments)
			{ return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments); }

		inline int Vsnprintf(char16_t* EA_RESTRICT pDestination, size_t n, const char16_t* EA_RESTRICT pFormat, va_list arguments)
			{ return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments); }

		inline int Vsnprintf(char32_t* EA_RESTRICT pDestination, size_t n, const char32_t* EA_RESTRICT pFormat, va_list arguments)
			{ return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments); }

		#if EA_CHAR8_UNIQUE
			inline int Vsnprintf(char8_t* EA_RESTRICT pDestination, size_t n, const char8_t* EA_RESTRICT pFormat, va_list arguments)
				{ return EA::StdC::Vsnprintf((char*)pDestination, n, (const char*)pFormat, arguments); }
		#endif

		#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
			inline int Vsnprintf(wchar_t* EA_RESTRICT pDestination, size_t n, const wchar_t* EA_RESTRICT pFormat, va_list arguments)
			{ return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments); }
		#endif
	}
#else
	// User-provided functions.
	extern int Vsnprintf8 (char*  pDestination, size_t n, const char*  pFormat, va_list arguments);
	extern int Vsnprintf16(char16_t* pDestination, size_t n, const char16_t* pFormat, va_list arguments);
	extern int Vsnprintf32(char32_t* pDestination, size_t n, const char32_t* pFormat, va_list arguments);
	#if EA_CHAR8_UNIQUE
		extern int Vsnprintf8 (char8_t*  pDestination, size_t n, const char8_t*  pFormat, va_list arguments);
	#endif
	#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
		extern int VsnprintfW(wchar_t* pDestination, size_t n, const wchar_t* pFormat, va_list arguments);
	#endif

	namespace eastl
	{
		inline int Vsnprintf(char* pDestination, size_t n, const char* pFormat, va_list arguments)
			{ return Vsnprintf8(pDestination, n, pFormat, arguments); }

		inline int Vsnprintf(char16_t* pDestination, size_t n, const char16_t* pFormat, va_list arguments)
			{ return Vsnprintf16(pDestination, n, pFormat, arguments); }

		inline int Vsnprintf(char32_t* pDestination, size_t n, const char32_t* pFormat, va_list arguments)
			{ return Vsnprintf32(pDestination, n, pFormat, arguments); }

		#if EA_CHAR8_UNIQUE
			inline int Vsnprintf(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
				{ return Vsnprintf8(pDestination, n, pFormat, arguments); }
		#endif

		#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
			inline int Vsnprintf(wchar_t* pDestination, size_t n, const wchar_t* pFormat, va_list arguments)
				{ return VsnprintfW(pDestination, n, pFormat, arguments); }
		#endif
	}
#endif
///////////////////////////////////////////////////////////////////////////////



namespace eastl
{

	/// EASTL_BASIC_STRING_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_BASIC_STRING_DEFAULT_NAME
		#define EASTL_BASIC_STRING_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " basic_string" // Unless the user overrides something, this is "EASTL basic_string".
	#endif


	/// EASTL_BASIC_STRING_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_BASIC_STRING_DEFAULT_ALLOCATOR
		#define EASTL_BASIC_STRING_DEFAULT_ALLOCATOR allocator_type(EASTL_BASIC_STRING_DEFAULT_NAME)
	#endif


	///////////////////////////////////////////////////////////////////////////////
	/// basic_string
	///
	/// Implements a templated string class, somewhat like C++ std::basic_string.
	///
	/// Notes:
	///     As of this writing, an insert of a string into itself necessarily
	///     triggers a reallocation, even if there is enough capacity in self
	///     to handle the increase in size. This is due to the slightly tricky
	///     nature of the operation of modifying one's self with one's self,
	///     and thus the source and destination are being modified during the
	///     operation. It might be useful to rectify this to the extent possible.
	///
	///     Our usage of noexcept specifiers is a little different from the
	///     requirements specified by std::basic_string in C++11. This is because
	///     our allocators are instances and not types and thus can be non-equal
	///     and result in exceptions during assignments that theoretically can't
	///     occur with std containers.
	///
	template <typename T, typename Allocator = EASTLAllocatorType>
	class basic_string
	{
	public:
		typedef basic_string<T, Allocator>                      this_type;
		typedef basic_string_view<T>                            view_type;
		typedef T                                               value_type;
		typedef T*                                              pointer;
		typedef const T*                                        const_pointer;
		typedef T&                                              reference;
		typedef const T&                                        const_reference;
		typedef T*                                              iterator;           // Maintainer note: We want to leave iterator defined as T* -- at least in release builds -- as this gives some algorithms an advantage that optimizers cannot get around.
		typedef const T*                                        const_iterator;
		typedef eastl::reverse_iterator<iterator>               reverse_iterator;
		typedef eastl::reverse_iterator<const_iterator>         const_reverse_iterator;
		typedef eastl_size_t                                    size_type;          // See config.h for the definition of eastl_size_t, which defaults to size_t.
		typedef ptrdiff_t                                       difference_type;
		typedef Allocator                                       allocator_type;

	static const EA_CONSTEXPR size_type npos     = (size_type)-1;      /// 'npos' means non-valid position or simply non-position.

	public:
		// CtorDoNotInitialize exists so that we can create a constructor that allocates but doesn't
		// initialize and also doesn't collide with any other constructor declaration.
		struct CtorDoNotInitialize{};

		// CtorSprintf exists so that we can create a constructor that accepts printf-style
		// arguments but also doesn't collide with any other constructor declaration.
		#ifdef EA_PLATFORM_MINGW
			// Workaround for MinGW compiler bug: variadic arguments are corrupted if empty object is passed before it
			struct CtorSprintf{ int dummy; };
		#else
			struct CtorSprintf{};
		#endif

		// CtorConvert exists so that we can have a constructor that implements string encoding
		// conversion, such as between UCS2 char16_t and UTF8 char8_t.
		struct CtorConvert{};

	protected:
		// Masks used to determine if we are in SSO or Heap
		#ifdef EA_SYSTEM_BIG_ENDIAN
			// Big Endian use LSB, unless we want to reorder struct layouts on endianness, Bit is set when we are in Heap
			static EA_CONSTEXPR_OR_CONST size_type kHeapMask = 0x1;
			static EA_CONSTEXPR_OR_CONST size_type kSSOMask  = 0x1;
		#else
			// Little Endian use MSB
			static EA_CONSTEXPR_OR_CONST size_type kHeapMask = ~(size_type(~size_type(0)) >> 1);
			static EA_CONSTEXPR_OR_CONST size_type kSSOMask  = 0x80;
		#endif

	public:
		#ifdef EA_SYSTEM_BIG_ENDIAN
			static EA_CONSTEXPR_OR_CONST size_type kMaxSize = (~kHeapMask) >> 1;
		#else
			static EA_CONSTEXPR_OR_CONST size_type kMaxSize = ~kHeapMask;
		#endif

	protected:
		// The view of memory when the string data is obtained from the allocator.
		struct HeapLayout
		{
			value_type* mpBegin;  // Begin of string.
			size_type mnSize;     // Size of the string. Number of characters currently in the string, not including the trailing '0'
			size_type mnCapacity; // Capacity of the string. Number of characters string can hold, not including the trailing '0'
		};

		template <typename CharT, size_t = sizeof(CharT)>
		struct SSOPadding
		{
			char padding[sizeof(CharT) - sizeof(char)];
		};

		template <typename CharT>
		struct SSOPadding<CharT, 1>
		{
			// template specialization to remove the padding structure to avoid warnings on zero length arrays
			// also, this allows us to take advantage of the empty-base-class optimization.
		};

		// The view of memory when the string data is able to store the string data locally (without a heap allocation).
		struct SSOLayout
		{
			static EA_CONSTEXPR_OR_CONST size_type SSO_CAPACITY = (sizeof(HeapLayout) - sizeof(char)) / sizeof(value_type);

			// mnSize must correspond to the last byte of HeapLayout.mnCapacity, so we don't want the compiler to insert
			// padding after mnSize if sizeof(value_type) != 1; Also ensures both layouts are the same size.
			struct SSOSize : SSOPadding<value_type>
			{
				char mnRemainingSize;
			};

			value_type mData[SSO_CAPACITY]; // Local buffer for string data.
			SSOSize mRemainingSizeField;
		};

		// This view of memory is a utility structure for easy copying of the string data.
		struct RawLayout
		{
			char mBuffer[sizeof(HeapLayout)];
		};

		static_assert(sizeof(SSOLayout)  == sizeof(HeapLayout), "heap and sso layout structures must be the same size");
		static_assert(sizeof(HeapLayout) == sizeof(RawLayout),  "heap and raw layout structures must be the same size");

		// This implements the 'short string optimization' or SSO. SSO reuses the existing storage of string class to
		// hold string data short enough to fit therefore avoiding a heap allocation. The number of characters stored in
		// the string SSO buffer is variable and depends on the string character width. This implementation favors a
		// consistent string size than increasing the size of the string local data to accommodate a consistent number
		// of characters despite character width.
		struct Layout
		{
			union
			{
				HeapLayout heap;
				SSOLayout sso;
				RawLayout raw;
			};

			Layout()                                                  { ResetToSSO(); } // start as SSO by default
			Layout(const Layout& other)                               { Copy(*this, other); }
			Layout(Layout&& other)                                    { Move(*this, other); }
			Layout& operator=(const Layout& other)                    { Copy(*this, other); return *this; }
			Layout& operator=(Layout&& other)                         { Move(*this, other); return *this; }

			// We are using Heap when the bit is set, easier to conceptualize checking IsHeap instead of IsSSO
			inline bool IsHeap() const EA_NOEXCEPT                    { return !!(sso.mRemainingSizeField.mnRemainingSize & kSSOMask); }
			inline bool IsSSO() const EA_NOEXCEPT                     { return !IsHeap(); }
			inline value_type* SSOBufferPtr() EA_NOEXCEPT             { return sso.mData; }
			inline const value_type* SSOBufferPtr() const EA_NOEXCEPT { return sso.mData; }

			// Largest value for SSO.mnSize == 23, which has two LSB bits set, but on big-endian (BE)
			// use least significant bit (LSB) to denote heap so shift.
			inline size_type GetSSOSize() const EA_NOEXCEPT
			{
				#ifdef EA_SYSTEM_BIG_ENDIAN
					return SSOLayout::SSO_CAPACITY - (sso.mRemainingSizeField.mnRemainingSize >> 2);
				#else
					return (SSOLayout::SSO_CAPACITY - sso.mRemainingSizeField.mnRemainingSize);
				#endif
			}
			inline size_type GetHeapSize() const EA_NOEXCEPT { return heap.mnSize; }
			inline size_type GetSize() const EA_NOEXCEPT     { return IsHeap() ? GetHeapSize() : GetSSOSize(); }

			inline void SetSSOSize(size_type size) EA_NOEXCEPT
			{
				#ifdef EA_SYSTEM_BIG_ENDIAN
					sso.mRemainingSizeField.mnRemainingSize = (char)((SSOLayout::SSO_CAPACITY - size) << 2);
				#else
					sso.mRemainingSizeField.mnRemainingSize = (char)(SSOLayout::SSO_CAPACITY - size);
				#endif
			}

			inline void SetHeapSize(size_type size) EA_NOEXCEPT          { heap.mnSize = size; }
			inline void SetSize(size_type size) EA_NOEXCEPT              { IsHeap() ? SetHeapSize(size) : SetSSOSize(size); }

			inline size_type GetRemainingCapacity() const EA_NOEXCEPT    { return size_type(CapacityPtr() - EndPtr()); }

			inline value_type* HeapBeginPtr() EA_NOEXCEPT                { return heap.mpBegin; };
			inline const value_type* HeapBeginPtr() const EA_NOEXCEPT    { return heap.mpBegin; };

			inline value_type* SSOBeginPtr() EA_NOEXCEPT                 { return sso.mData; }
			inline const value_type* SSOBeginPtr() const EA_NOEXCEPT     { return sso.mData; }

			inline value_type* BeginPtr() EA_NOEXCEPT                    { return IsHeap() ? HeapBeginPtr() : SSOBeginPtr(); }
			inline const value_type* BeginPtr() const EA_NOEXCEPT        { return IsHeap() ? HeapBeginPtr() : SSOBeginPtr(); }

			inline value_type* HeapEndPtr() EA_NOEXCEPT                  { return heap.mpBegin + heap.mnSize; }
			inline const value_type* HeapEndPtr() const EA_NOEXCEPT      { return heap.mpBegin + heap.mnSize; }

			inline value_type* SSOEndPtr() EA_NOEXCEPT                   { return sso.mData + GetSSOSize(); }
			inline const value_type* SSOEndPtr() const EA_NOEXCEPT       { return sso.mData + GetSSOSize(); }

			// Points to end of character stream, *ptr == '0'
			inline value_type* EndPtr() EA_NOEXCEPT                      { return IsHeap() ? HeapEndPtr() : SSOEndPtr(); }
			inline const value_type* EndPtr() const EA_NOEXCEPT          { return IsHeap() ? HeapEndPtr() : SSOEndPtr(); }

			inline value_type* HeapCapacityPtr() EA_NOEXCEPT             { return heap.mpBegin + GetHeapCapacity(); }
			inline const value_type* HeapCapacityPtr() const EA_NOEXCEPT { return heap.mpBegin + GetHeapCapacity(); }

			inline value_type* SSOCapacityPtr() EA_NOEXCEPT               { return sso.mData + SSOLayout::SSO_CAPACITY; }
			inline const value_type* SSOCapacityPtr() const EA_NOEXCEPT   { return sso.mData + SSOLayout::SSO_CAPACITY; }

			// Points to end of the buffer at the terminating '0', *ptr == '0' <- only true when size() == capacity()
			inline value_type* CapacityPtr() EA_NOEXCEPT                 { return IsHeap() ? HeapCapacityPtr() : SSOCapacityPtr(); }
			inline const value_type* CapacityPtr() const EA_NOEXCEPT     { return IsHeap() ? HeapCapacityPtr() : SSOCapacityPtr(); }

			inline void SetHeapBeginPtr(value_type* pBegin) EA_NOEXCEPT  { heap.mpBegin = pBegin; }

			inline void SetHeapCapacity(size_type cap) EA_NOEXCEPT
			{
			#ifdef EA_SYSTEM_BIG_ENDIAN
				heap.mnCapacity = (cap << 1) | kHeapMask;
			#else
				heap.mnCapacity = (cap | kHeapMask);
			#endif
			}

			inline size_type GetHeapCapacity() const EA_NOEXCEPT
			{
			#ifdef EA_SYSTEM_BIG_ENDIAN
				return (heap.mnCapacity >> 1);
			#else
				return (heap.mnCapacity & ~kHeapMask);
			#endif
			}

			inline void Copy(Layout& dst, const Layout& src) EA_NOEXCEPT { dst.raw = src.raw; }
			inline void Move(Layout& dst, Layout& src) EA_NOEXCEPT       { eastl::swap(dst.raw, src.raw); }
			inline void Swap(Layout& a, Layout& b) EA_NOEXCEPT           { eastl::swap(a.raw, b.raw); }

			inline void ResetToSSO() EA_NOEXCEPT { *SSOBeginPtr() = 0; SetSSOSize(0); }
		};

		eastl::compressed_pair<Layout, allocator_type> mPair;

		inline Layout& internalLayout() EA_NOEXCEPT                        { return mPair.first(); }
		inline const Layout& internalLayout() const EA_NOEXCEPT            { return mPair.first(); }
		inline allocator_type& internalAllocator() EA_NOEXCEPT             { return mPair.second(); }
		inline const allocator_type& internalAllocator() const EA_NOEXCEPT { return mPair.second(); }

	public:
		// Constructor, destructor
		basic_string() EA_NOEXCEPT_IF(EA_NOEXCEPT_EXPR(EASTL_BASIC_STRING_DEFAULT_ALLOCATOR));
		explicit basic_string(const allocator_type& allocator) EA_NOEXCEPT;
		basic_string(const this_type& x, size_type position, size_type n = npos);
		basic_string(const value_type* p, size_type n, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);
		EASTL_STRING_EXPLICIT basic_string(const value_type* p, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);
		basic_string(size_type n, value_type c, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);
		basic_string(const this_type& x);
	    basic_string(const this_type& x, const allocator_type& allocator);
		basic_string(const value_type* pBegin, const value_type* pEnd, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);
		basic_string(CtorDoNotInitialize, size_type n, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);
		basic_string(CtorSprintf, const value_type* pFormat, ...);
		basic_string(std::initializer_list<value_type> init, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);

		basic_string(this_type&& x) EA_NOEXCEPT;
		basic_string(this_type&& x, const allocator_type& allocator);

		explicit basic_string(const view_type& sv, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);
		basic_string(const view_type& sv, size_type position, size_type n, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);

		template <typename OtherCharType>
		basic_string(CtorConvert, const OtherCharType* p, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);

		template <typename OtherCharType>
		basic_string(CtorConvert, const OtherCharType* p, size_type n, const allocator_type& allocator = EASTL_BASIC_STRING_DEFAULT_ALLOCATOR);

		template <typename OtherStringType> // Unfortunately we need the CtorConvert here because otherwise this function would collide with the value_type* constructor.
		basic_string(CtorConvert, const OtherStringType& x);

	   ~basic_string();

		// Allocator
		const allocator_type& get_allocator() const EA_NOEXCEPT;
		allocator_type&       get_allocator() EA_NOEXCEPT;
		void                  set_allocator(const allocator_type& allocator);

		// Implicit conversion operator
		operator basic_string_view<T>() const EA_NOEXCEPT;

		// Operator=
		this_type& operator=(const this_type& x);
		this_type& operator=(const value_type* p);
		this_type& operator=(value_type c);
		this_type& operator=(std::initializer_list<value_type> ilist);
		this_type& operator=(view_type v);
		this_type& operator=(this_type&& x); // TODO(c++17): noexcept(allocator_traits<Allocator>::propagate_on_container_move_assignment::value || allocator_traits<Allocator>::is_always_equal::value);

		#if EASTL_OPERATOR_EQUALS_OTHER_ENABLED
			this_type& operator=(value_type* p) { return operator=((const value_type*)p); } // We need this because otherwise the const value_type* version can collide with the const OtherStringType& version below.

			template <typename OtherCharType>
			this_type& operator=(const OtherCharType* p);

			template <typename OtherStringType>
			this_type& operator=(const OtherStringType& x);
		#endif

		void swap(this_type& x); // TODO(c++17): noexcept(allocator_traits<Allocator>::propagate_on_container_swap::value || allocator_traits<Allocator>::is_always_equal::value);

		// Assignment operations
		this_type& assign(const this_type& x);
		this_type& assign(const this_type& x, size_type position, size_type n = npos);
		this_type& assign(const value_type* p, size_type n);
		this_type& assign(const value_type* p);
		this_type& assign(size_type n, value_type c);
		this_type& assign(const value_type* pBegin, const value_type* pEnd);
		this_type& assign(this_type&& x); // TODO(c++17): noexcept(allocator_traits<Allocator>::propagate_on_container_move_assignment::value || allocator_traits<Allocator>::is_always_equal::value);
		this_type& assign(std::initializer_list<value_type>);

		template <typename OtherCharType>
		this_type& assign_convert(const OtherCharType* p);

		template <typename OtherCharType>
		this_type& assign_convert(const OtherCharType* p, size_type n);

		template <typename OtherStringType>
		this_type& assign_convert(const OtherStringType& x);

		// Iterators.
		iterator       begin() EA_NOEXCEPT;                 // Expanded in source code as: mpBegin
		const_iterator begin() const EA_NOEXCEPT;           // Expanded in source code as: mpBegin
		const_iterator cbegin() const EA_NOEXCEPT;

		iterator       end() EA_NOEXCEPT;                   // Expanded in source code as: mpEnd
		const_iterator end() const EA_NOEXCEPT;             // Expanded in source code as: mpEnd
		const_iterator cend() const EA_NOEXCEPT;

		reverse_iterator       rbegin() EA_NOEXCEPT;
		const_reverse_iterator rbegin() const EA_NOEXCEPT;
		const_reverse_iterator crbegin() const EA_NOEXCEPT;

		reverse_iterator       rend() EA_NOEXCEPT;
		const_reverse_iterator rend() const EA_NOEXCEPT;
		const_reverse_iterator crend() const EA_NOEXCEPT;


		// Size-related functionality
		bool      empty() const EA_NOEXCEPT;
		size_type size() const EA_NOEXCEPT;
		size_type length() const EA_NOEXCEPT;
		size_type max_size() const EA_NOEXCEPT;
		size_type capacity() const EA_NOEXCEPT;
		void      resize(size_type n, value_type c);
		void      resize(size_type n);
		void      reserve(size_type = 0);
		void      set_capacity(size_type n = npos); // Revises the capacity to the user-specified value. Resizes the container to match the capacity if the requested capacity n is less than the current size. If n == npos then the capacity is reallocated (if necessary) such that capacity == size.
		void      force_size(size_type n);          // Unilaterally moves the string end position (mpEnd) to the given location. Useful for when the user writes into the string via some extenal means such as C strcpy or sprintf. This allows for more efficient use than using resize to achieve this.
		void shrink_to_fit();

		// Raw access
		const value_type* data() const  EA_NOEXCEPT;
		      value_type* data()        EA_NOEXCEPT;
		const value_type* c_str() const EA_NOEXCEPT;

		// Element access
		reference       operator[](size_type n);
		const_reference operator[](size_type n) const;
		reference       at(size_type n);
		const_reference at(size_type n) const;
		reference       front();
		const_reference front() const;
		reference       back();
		const_reference back() const;

		// Append operations
		this_type& operator+=(const this_type& x);
		this_type& operator+=(const value_type* p);
		this_type& operator+=(value_type c);

		this_type& append(const this_type& x);
		this_type& append(const this_type& x,  size_type position, size_type n = npos);
		this_type& append(const value_type* p, size_type n);
		this_type& append(const value_type* p);
		this_type& append(size_type n, value_type c);
		this_type& append(const value_type* pBegin, const value_type* pEnd);

		this_type& append_sprintf_va_list(const value_type* pFormat, va_list arguments);
		this_type& append_sprintf(const value_type* pFormat, ...);

		template <typename OtherCharType>
		this_type& append_convert(const OtherCharType* p);

		template <typename OtherCharType>
		this_type& append_convert(const OtherCharType* p, size_type n);

		template <typename OtherStringType>
		this_type& append_convert(const OtherStringType& x);

		void push_back(value_type c);
		void pop_back();

		// Insertion operations
		this_type& insert(size_type position, const this_type& x);
		this_type& insert(size_type position, const this_type& x, size_type beg, size_type n);
		this_type& insert(size_type position, const value_type* p, size_type n);
		this_type& insert(size_type position, const value_type* p);
		this_type& insert(size_type position, size_type n, value_type c);
		iterator   insert(const_iterator p, value_type c);
		iterator   insert(const_iterator p, size_type n, value_type c);
		iterator   insert(const_iterator p, const value_type* pBegin, const value_type* pEnd);
		iterator   insert(const_iterator p, std::initializer_list<value_type>);

		// Erase operations
		this_type&       erase(size_type position = 0, size_type n = npos);
		iterator         erase(const_iterator p);
		iterator         erase(const_iterator pBegin, const_iterator pEnd);
		reverse_iterator erase(reverse_iterator position);
		reverse_iterator erase(reverse_iterator first, reverse_iterator last);
		void             clear() EA_NOEXCEPT;

		// Detach memory
		pointer detach() EA_NOEXCEPT;

		// Replacement operations
		this_type&  replace(size_type position, size_type n,  const this_type& x);
		this_type&  replace(size_type pos1,     size_type n1, const this_type& x,  size_type pos2, size_type n2 = npos);
		this_type&  replace(size_type position, size_type n1, const value_type* p, size_type n2);
		this_type&  replace(size_type position, size_type n1, const value_type* p);
		this_type&  replace(size_type position, size_type n1, size_type n2, value_type c);
		this_type&  replace(const_iterator first, const_iterator last, const this_type& x);
		this_type&  replace(const_iterator first, const_iterator last, const value_type* p, size_type n);
		this_type&  replace(const_iterator first, const_iterator last, const value_type* p);
		this_type&  replace(const_iterator first, const_iterator last, size_type n, value_type c);
		this_type&  replace(const_iterator first, const_iterator last, const value_type* pBegin, const value_type* pEnd);
		size_type   copy(value_type* p, size_type n, size_type position = 0) const;

		// Find operations
		size_type find(const this_type& x,  size_type position = 0) const EA_NOEXCEPT;
		size_type find(const value_type* p, size_type position = 0) const;
		size_type find(const value_type* p, size_type position, size_type n) const;
		size_type find(value_type c, size_type position = 0) const EA_NOEXCEPT;

		// Reverse find operations
		size_type rfind(const this_type& x,  size_type position = npos) const EA_NOEXCEPT;
		size_type rfind(const value_type* p, size_type position = npos) const;
		size_type rfind(const value_type* p, size_type position, size_type n) const;
		size_type rfind(value_type c, size_type position = npos) const EA_NOEXCEPT;

		// Find first-of operations
		size_type find_first_of(const this_type& x, size_type position = 0) const EA_NOEXCEPT;
		size_type find_first_of(const value_type* p, size_type position = 0) const;
		size_type find_first_of(const value_type* p, size_type position, size_type n) const;
		size_type find_first_of(value_type c, size_type position = 0) const EA_NOEXCEPT;

		// Find last-of operations
		size_type find_last_of(const this_type& x, size_type position = npos) const EA_NOEXCEPT;
		size_type find_last_of(const value_type* p, size_type position = npos) const;
		size_type find_last_of(const value_type* p, size_type position, size_type n) const;
		size_type find_last_of(value_type c, size_type position = npos) const EA_NOEXCEPT;

		// Find first not-of operations
		size_type find_first_not_of(const this_type& x, size_type position = 0) const EA_NOEXCEPT;
		size_type find_first_not_of(const value_type* p, size_type position = 0) const;
		size_type find_first_not_of(const value_type* p, size_type position, size_type n) const;
		size_type find_first_not_of(value_type c, size_type position = 0) const EA_NOEXCEPT;

		// Find last not-of operations
		size_type find_last_not_of(const this_type& x,  size_type position = npos) const EA_NOEXCEPT;
		size_type find_last_not_of(const value_type* p, size_type position = npos) const;
		size_type find_last_not_of(const value_type* p, size_type position, size_type n) const;
		size_type find_last_not_of(value_type c, size_type position = npos) const EA_NOEXCEPT;

		// Substring functionality
		this_type substr(size_type position = 0, size_type n = npos) const;

		// Comparison operations
		int        compare(const this_type& x) const EA_NOEXCEPT;
		int        compare(size_type pos1, size_type n1, const this_type& x) const;
		int        compare(size_type pos1, size_type n1, const this_type& x, size_type pos2, size_type n2) const;
		int        compare(const value_type* p) const;
		int        compare(size_type pos1, size_type n1, const value_type* p) const;
		int        compare(size_type pos1, size_type n1, const value_type* p, size_type n2) const;
		static int compare(const value_type* pBegin1, const value_type* pEnd1, const value_type* pBegin2, const value_type* pEnd2);

		// Case-insensitive comparison functions. Not part of C++ this_type. Only ASCII-level locale functionality is supported. Thus this is not suitable for localization purposes.
		int        comparei(const this_type& x) const EA_NOEXCEPT;
		int        comparei(const value_type* p) const;
		static int comparei(const value_type* pBegin1, const value_type* pEnd1, const value_type* pBegin2, const value_type* pEnd2);

		// Misc functionality, not part of C++ this_type.
		void         make_lower();
		void         make_upper();
		void         ltrim();
		void         rtrim();
		void         trim();
		void         ltrim(const value_type* p);
		void         rtrim(const value_type* p);
		void         trim(const value_type* p);
		this_type    left(size_type n) const;
		this_type    right(size_type n) const;
		this_type&   sprintf_va_list(const value_type* pFormat, va_list arguments);
		this_type&   sprintf(const value_type* pFormat, ...);

		bool validate() const EA_NOEXCEPT;
		int  validate_iterator(const_iterator i) const EA_NOEXCEPT;


	protected:
		// Helper functions for initialization/insertion operations.
		value_type* DoAllocate(size_type n);
		void        DoFree(value_type* p, size_type n);
		size_type   GetNewCapacity(size_type currentCapacity);
		size_type   GetNewCapacity(size_type currentCapacity, size_type minimumGrowSize);
		void        AllocateSelf();
		void        AllocateSelf(size_type n);
		void        DeallocateSelf();
		iterator    InsertInternal(const_iterator p, value_type c);
		void        RangeInitialize(const value_type* pBegin, const value_type* pEnd);
		void        RangeInitialize(const value_type* pBegin);
		void        SizeInitialize(size_type n, value_type c);

		bool        IsSSO() const EA_NOEXCEPT;

		void        ThrowLengthException() const;
		void        ThrowRangeException() const;
		void        ThrowInvalidArgumentException() const;

		#if EASTL_OPERATOR_EQUALS_OTHER_ENABLED
			template <typename CharType>
			void DoAssignConvert(CharType c, true_type);

			template <typename StringType>
			void DoAssignConvert(const StringType& x, false_type);
		#endif

		// Replacements for STL template functions.
		static const value_type* CharTypeStringFindEnd(const value_type* pBegin, const value_type* pEnd, value_type c);
		static const value_type* CharTypeStringRFind(const value_type* pRBegin, const value_type* pREnd, const value_type c);
		static const value_type* CharTypeStringSearch(const value_type* p1Begin, const value_type* p1End, const value_type* p2Begin, const value_type* p2End);
		static const value_type* CharTypeStringRSearch(const value_type* p1Begin, const value_type* p1End, const value_type* p2Begin, const value_type* p2End);
		static const value_type* CharTypeStringFindFirstOf(const value_type* p1Begin, const value_type* p1End, const value_type* p2Begin, const value_type* p2End);
		static const value_type* CharTypeStringRFindFirstOf(const value_type* p1RBegin, const value_type* p1REnd, const value_type* p2Begin, const value_type* p2End);
		static const value_type* CharTypeStringFindFirstNotOf(const value_type* p1Begin, const value_type* p1End, const value_type* p2Begin, const value_type* p2End);
		static const value_type* CharTypeStringRFindFirstNotOf(const value_type* p1RBegin, const value_type* p1REnd, const value_type* p2Begin, const value_type* p2End);

	}; // basic_string





	///////////////////////////////////////////////////////////////////////////////
	// basic_string
	///////////////////////////////////////////////////////////////////////////////

	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>::basic_string() EA_NOEXCEPT_IF(EA_NOEXCEPT_EXPR(EASTL_BASIC_STRING_DEFAULT_ALLOCATOR))
	    : mPair(allocator_type(EASTL_BASIC_STRING_DEFAULT_NAME))
	{
		AllocateSelf();
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>::basic_string(const allocator_type& allocator) EA_NOEXCEPT
	    : mPair(allocator)
	{
		AllocateSelf();
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>::basic_string(const this_type& x)
	    : mPair(x.get_allocator())
	{
		RangeInitialize(x.internalLayout().BeginPtr(), x.internalLayout().EndPtr());
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>::basic_string(const this_type& x, const allocator_type& allocator)
		: mPair(allocator)
	{
		RangeInitialize(x.internalLayout().BeginPtr(), x.internalLayout().EndPtr());
	}


	template <typename T, typename Allocator>
	template <typename OtherStringType>
	inline basic_string<T, Allocator>::basic_string(CtorConvert, const OtherStringType& x)
	    : mPair(x.get_allocator())
	{
		AllocateSelf();
		append_convert(x.c_str(), x.length());
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>::basic_string(const this_type& x, size_type position, size_type n)
		: mPair(x.get_allocator())
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if (EASTL_UNLIKELY(position > x.internalLayout().GetSize())) // 21.4.2 p4
			{
				ThrowRangeException();
				AllocateSelf();
			}
			else
				RangeInitialize(
					x.internalLayout().BeginPtr() + position,
					x.internalLayout().BeginPtr() + position + eastl::min_alt(n, x.internalLayout().GetSize() - position));
        #else
			RangeInitialize(
				x.internalLayout().BeginPtr() + position,
				x.internalLayout().BeginPtr() + position + eastl::min_alt(n, x.internalLayout().GetSize() - position));
        #endif
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>::basic_string(const value_type* p, size_type n, const allocator_type& allocator)
		: mPair(allocator)
	{
		RangeInitialize(p, p + n);
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>::basic_string(const view_type& sv, const allocator_type& allocator)
	    : basic_string(sv.data(), sv.size(), allocator)
	{
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>::basic_string(const view_type& sv, size_type position, size_type n, const allocator_type& allocator)
	    : basic_string(sv.substr(position, n), allocator)
	{
	}


	template <typename T, typename Allocator>
	template <typename OtherCharType>
	inline basic_string<T, Allocator>::basic_string(CtorConvert, const OtherCharType* p, const allocator_type& allocator)
		: mPair(allocator)
	{
		AllocateSelf();    // In this case we are converting from one string encoding to another, and we
		append_convert(p); // implement this in the simplest way, by simply default-constructing and calling assign.
	}


	template <typename T, typename Allocator>
	template <typename OtherCharType>
	inline basic_string<T, Allocator>::basic_string(CtorConvert, const OtherCharType* p, size_type n, const allocator_type& allocator)
		: mPair(allocator)
	{
		AllocateSelf();         // In this case we are converting from one string encoding to another, and we
		append_convert(p, n);   // implement this in the simplest way, by simply default-constructing and calling assign.
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>::basic_string(const value_type* p, const allocator_type& allocator)
		: mPair(allocator)
	{
		RangeInitialize(p);
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>::basic_string(size_type n, value_type c, const allocator_type& allocator)
		: mPair(allocator)
	{
		SizeInitialize(n, c);
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>::basic_string(const value_type* pBegin, const value_type* pEnd, const allocator_type& allocator)
		: mPair(allocator)
	{
		RangeInitialize(pBegin, pEnd);
	}


	// CtorDoNotInitialize exists so that we can create a version that allocates but doesn't
	// initialize but also doesn't collide with any other constructor declaration.
	template <typename T, typename Allocator>
	basic_string<T, Allocator>::basic_string(CtorDoNotInitialize /*unused*/, size_type n, const allocator_type& allocator)
		: mPair(allocator)
	{
		// Note that we do not call SizeInitialize here.
		AllocateSelf(n);
		internalLayout().SetSize(0);
		*internalLayout().EndPtr() = 0;
	}


	// CtorSprintf exists so that we can create a version that does a variable argument
	// sprintf but also doesn't collide with any other constructor declaration.
	template <typename T, typename Allocator>
	basic_string<T, Allocator>::basic_string(CtorSprintf /*unused*/, const value_type* pFormat, ...)
		: mPair()
	{
		const size_type n = (size_type)CharStrlen(pFormat);
		AllocateSelf(n);
		internalLayout().SetSize(0);

		va_list arguments;
		va_start(arguments, pFormat);
		append_sprintf_va_list(pFormat, arguments);
		va_end(arguments);
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>::basic_string(std::initializer_list<value_type> init, const allocator_type& allocator)
		: mPair(allocator)
	{
		RangeInitialize(init.begin(), init.end());
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>::basic_string(this_type&& x) EA_NOEXCEPT
		: mPair(x.get_allocator())
	{
		internalLayout() = eastl::move(x.internalLayout());
		x.AllocateSelf();
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>::basic_string(this_type&& x, const allocator_type& allocator)
	: mPair(allocator)
	{
		if(get_allocator() == x.get_allocator()) // If we can borrow from x...
		{
			internalLayout() = eastl::move(x.internalLayout());
			x.AllocateSelf();
		}
		else if(x.internalLayout().BeginPtr())
		{
			RangeInitialize(x.internalLayout().BeginPtr(), x.internalLayout().EndPtr());
			// Let x destruct its own items.
		}
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>::~basic_string()
	{
		DeallocateSelf();
	}


	template <typename T, typename Allocator>
	inline const typename basic_string<T, Allocator>::allocator_type&
	basic_string<T, Allocator>::get_allocator() const EA_NOEXCEPT
	{
		return internalAllocator();
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::allocator_type&
	basic_string<T, Allocator>::get_allocator() EA_NOEXCEPT
	{
		return internalAllocator();
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::set_allocator(const allocator_type& allocator)
	{
		get_allocator() = allocator;
	}


	template <typename T, typename Allocator>
	inline const typename basic_string<T, Allocator>::value_type*
	basic_string<T, Allocator>::data()  const EA_NOEXCEPT
	{
		return internalLayout().BeginPtr();
	}


	template <typename T, typename Allocator>
	inline const typename basic_string<T, Allocator>::value_type*
	basic_string<T, Allocator>::c_str() const EA_NOEXCEPT
	{
		return internalLayout().BeginPtr();
	}

	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::value_type*
	basic_string<T, Allocator>::data() EA_NOEXCEPT
	{
		return internalLayout().BeginPtr();
	}

	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::iterator
	basic_string<T, Allocator>::begin() EA_NOEXCEPT
	{
		return internalLayout().BeginPtr();
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::iterator
	basic_string<T, Allocator>::end() EA_NOEXCEPT
	{
		return internalLayout().EndPtr();
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::const_iterator
	basic_string<T, Allocator>::begin() const EA_NOEXCEPT
	{
		return internalLayout().BeginPtr();
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::const_iterator
	basic_string<T, Allocator>::cbegin() const EA_NOEXCEPT
	{
		return internalLayout().BeginPtr();
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::const_iterator
	basic_string<T, Allocator>::end() const EA_NOEXCEPT
	{
		return internalLayout().EndPtr();
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::const_iterator
	basic_string<T, Allocator>::cend() const EA_NOEXCEPT
	{
		return internalLayout().EndPtr();
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::reverse_iterator
	basic_string<T, Allocator>::rbegin() EA_NOEXCEPT
	{
		return reverse_iterator(internalLayout().EndPtr());
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::reverse_iterator
	basic_string<T, Allocator>::rend() EA_NOEXCEPT
	{
		return reverse_iterator(internalLayout().BeginPtr());
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::const_reverse_iterator
	basic_string<T, Allocator>::rbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(internalLayout().EndPtr());
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::const_reverse_iterator
	basic_string<T, Allocator>::crbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(internalLayout().EndPtr());
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::const_reverse_iterator
	basic_string<T, Allocator>::rend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(internalLayout().BeginPtr());
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::const_reverse_iterator
	basic_string<T, Allocator>::crend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(internalLayout().BeginPtr());
	}


	template <typename T, typename Allocator>
	inline bool basic_string<T, Allocator>::empty() const EA_NOEXCEPT
	{
		return (internalLayout().GetSize() == 0);
	}


	template <typename T, typename Allocator>
	inline bool basic_string<T, Allocator>::IsSSO() const EA_NOEXCEPT
	{
		return internalLayout().IsSSO();
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::size() const EA_NOEXCEPT
	{
		return internalLayout().GetSize();
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::length() const EA_NOEXCEPT
	{
		return internalLayout().GetSize();
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::max_size() const EA_NOEXCEPT
	{
		return kMaxSize;
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::capacity() const EA_NOEXCEPT
	{
		if (internalLayout().IsHeap())
		{
			return internalLayout().GetHeapCapacity();
		}
		return SSOLayout::SSO_CAPACITY;
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::const_reference
	basic_string<T, Allocator>::operator[](size_type n) const
	{
		#if EASTL_ASSERT_ENABLED // We allow the user to reference the trailing 0 char without asserting. Perhaps we shouldn't.
			if(EASTL_UNLIKELY(n > internalLayout().GetSize()))
				EASTL_FAIL_MSG("basic_string::operator[] -- out of range");
		#endif

		return internalLayout().BeginPtr()[n]; // Sometimes done as *(mpBegin + n)
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::reference
	basic_string<T, Allocator>::operator[](size_type n)
	{
		#if EASTL_ASSERT_ENABLED // We allow the user to reference the trailing 0 char without asserting. Perhaps we shouldn't.
			if(EASTL_UNLIKELY(n > internalLayout().GetSize()))
				EASTL_FAIL_MSG("basic_string::operator[] -- out of range");
		#endif

		return internalLayout().BeginPtr()[n]; // Sometimes done as *(mpBegin + n)
	}


	template <typename T, typename Allocator>
	basic_string<T,Allocator>::operator basic_string_view<T>() const EA_NOEXCEPT
	{
		return basic_string_view<T>(data(), size());
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::this_type& basic_string<T, Allocator>::operator=(const this_type& x)
	{
		if(&x != this)
		{
			#if EASTL_ALLOCATOR_COPY_ENABLED
				bool bSlowerPathwayRequired = (get_allocator() != x.get_allocator());
			#else
				bool bSlowerPathwayRequired = false;
			#endif

			if(bSlowerPathwayRequired)
			{
				set_capacity(0); // Must use set_capacity instead of clear because set_capacity frees our memory, unlike clear.

				#if EASTL_ALLOCATOR_COPY_ENABLED
					get_allocator() = x.get_allocator();
				#endif
			}

			assign(x.internalLayout().BeginPtr(), x.internalLayout().EndPtr());
		}
		return *this;
	}


	#if EASTL_OPERATOR_EQUALS_OTHER_ENABLED
		template <typename T, typename Allocator>
		template <typename CharType>
		inline void basic_string<T, Allocator>::DoAssignConvert(CharType c, true_type)
		{
			assign_convert(&c, 1); // Call this version of append because it will result in the encoding-converting append being used.
		}


		template <typename T, typename Allocator>
		template <typename StringType>
		inline void basic_string<T, Allocator>::DoAssignConvert(const StringType& x, false_type)
		{
			//if(&x != this) // Unnecessary because &x cannot possibly equal this.
			{
				#if EASTL_ALLOCATOR_COPY_ENABLED
					get_allocator() = x.get_allocator();
				#endif

				assign_convert(x.c_str(), x.length());
			}
		}


		template <typename T, typename Allocator>
		template <typename OtherStringType>
		inline typename basic_string<T, Allocator>::this_type& basic_string<T, Allocator>::operator=(const OtherStringType& x)
		{
			clear();
			DoAssignConvert(x, is_integral<OtherStringType>());
			return *this;
		}


		template <typename T, typename Allocator>
		template <typename OtherCharType>
		inline typename basic_string<T, Allocator>::this_type& basic_string<T, Allocator>::operator=(const OtherCharType* p)
		{
			return assign_convert(p);
		}
	#endif


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::this_type& basic_string<T, Allocator>::operator=(const value_type* p)
	{
		return assign(p, p + CharStrlen(p));
	}

	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::this_type& basic_string<T, Allocator>::operator=(value_type c)
	{
		return assign((size_type)1, c);
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::this_type& basic_string<T, Allocator>::operator=(this_type&& x)
	{
		return assign(eastl::move(x));
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::this_type& basic_string<T, Allocator>::operator=(std::initializer_list<value_type> ilist)
	{
		return assign(ilist.begin(), ilist.end());
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::this_type& basic_string<T, Allocator>::operator=(view_type v)
	{
		return assign(v.data(), static_cast<this_type::size_type>(v.size()));
	}


	template <typename T, typename Allocator>
	void basic_string<T, Allocator>::resize(size_type n, value_type c)
	{
		const size_type s = internalLayout().GetSize();

		if(n < s)
			erase(internalLayout().BeginPtr() + n, internalLayout().EndPtr());
		else if(n > s)
			append(n - s, c);
	}


	template <typename T, typename Allocator>
	void basic_string<T, Allocator>::resize(size_type n)
	{
		// C++ basic_string specifies that resize(n) is equivalent to resize(n, value_type()).
		// For built-in types, value_type() is the same as zero (value_type(0)).
		// We can improve the efficiency (especially for long strings) of this
		// string class by resizing without assigning to anything.

		const size_type s = internalLayout().GetSize();

		if(n < s)
			erase(internalLayout().BeginPtr() + n, internalLayout().EndPtr());
		else if(n > s)
		{
			append(n - s, value_type());
		}
	}


	template <typename T, typename Allocator>
	void basic_string<T, Allocator>::reserve(size_type n)
	{
		#if EASTL_STRING_OPT_LENGTH_ERRORS
			if(EASTL_UNLIKELY(n > max_size()))
				ThrowLengthException();
		#endif

		// C++20 says if the passed in capacity is less than the current capacity we do not shrink
		// If new_cap is less than or equal to the current capacity(), there is no effect.
		// http://en.cppreference.com/w/cpp/string/basic_string/reserve

		n = eastl::max_alt(n, internalLayout().GetSize()); // Calculate the new capacity, which needs to be >= container size.

		if(n > capacity())
			set_capacity(n);
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::shrink_to_fit()
	{
		set_capacity(internalLayout().GetSize());
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::set_capacity(size_type n)
	{
		if(n == npos)
			// If the user wants to set the capacity to equal the current size...
			// '-1' because we pretend that we didn't allocate memory for the terminating 0.
			n = internalLayout().GetSize();
		else if(n < internalLayout().GetSize())
		{
			internalLayout().SetSize(n);
			*internalLayout().EndPtr() = 0;
		}

		if((n < capacity() && internalLayout().IsHeap()) || (n > capacity()))
		{
			// In here the string is transition from heap->heap, heap->sso or sso->heap

			if(EASTL_LIKELY(n))
			{

				if(n <= SSOLayout::SSO_CAPACITY)
				{
					// heap->sso
					// A heap based layout wants to reduce its size to within sso capacity
					// An sso layout wanting to reduce its capacity will not get in here
					pointer pOldBegin = internalLayout().BeginPtr();
					const size_type nOldCap = internalLayout().GetHeapCapacity();

					CharStringUninitializedCopy(pOldBegin, pOldBegin + n, internalLayout().SSOBeginPtr());
					internalLayout().SetSSOSize(n);
					*internalLayout().SSOEndPtr() = 0;

					DoFree(pOldBegin, nOldCap + 1);

					return;
				}

				pointer pNewBegin = DoAllocate(n + 1); // We need the + 1 to accomodate the trailing 0.
				size_type nSavedSize = internalLayout().GetSize(); // save the size in case we transition from sso->heap

				pointer pNewEnd = CharStringUninitializedCopy(internalLayout().BeginPtr(), internalLayout().EndPtr(), pNewBegin);
				*pNewEnd = 0;

				DeallocateSelf();

				internalLayout().SetHeapBeginPtr(pNewBegin);
				internalLayout().SetHeapCapacity(n);
				internalLayout().SetHeapSize(nSavedSize);
			}
			else
			{
				DeallocateSelf();
				AllocateSelf();
			}
		}
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::force_size(size_type n)
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(n > capacity()))
				ThrowRangeException();
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(n > capacity()))
				EASTL_FAIL_MSG("basic_string::force_size -- out of range");
		#endif

		internalLayout().SetSize(n);
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::clear() EA_NOEXCEPT
	{
		internalLayout().SetSize(0);
		*internalLayout().BeginPtr() = value_type(0);
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::pointer
	basic_string<T, Allocator>::detach() EA_NOEXCEPT
	{
		// The detach function is an extension function which simply forgets the
		// owned pointer. It doesn't free it but rather assumes that the user
		// does. If the string is utilizing the short-string-optimization when a
		// detach is requested, a copy of the string into a seperate memory
		// allocation occurs and the owning pointer is given to the user who is
		// responsible for freeing the memory.

		pointer pDetached = nullptr;

		if (internalLayout().IsSSO())
		{
			const size_type n = internalLayout().GetSize() + 1; // +1' so that we have room for the terminating 0.
			pDetached = DoAllocate(n);
			pointer pNewEnd = CharStringUninitializedCopy(internalLayout().BeginPtr(), internalLayout().EndPtr(), pDetached);
			*pNewEnd = 0;
		}
		else
		{
			pDetached = internalLayout().BeginPtr();
		}

		AllocateSelf(); // reset to string to empty
		return pDetached;
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::const_reference
	basic_string<T, Allocator>::at(size_type n) const
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(n >= internalLayout().GetSize()))
				ThrowRangeException();
		#elif EASTL_ASSERT_ENABLED                  // We assert if the user references the trailing 0 char.
			if(EASTL_UNLIKELY(n >= internalLayout().GetSize()))
				EASTL_FAIL_MSG("basic_string::at -- out of range");
		#endif

		return internalLayout().BeginPtr()[n];
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::reference
	basic_string<T, Allocator>::at(size_type n)
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(n >= internalLayout().GetSize()))
				ThrowRangeException();
		#elif EASTL_ASSERT_ENABLED                  // We assert if the user references the trailing 0 char.
			if(EASTL_UNLIKELY(n >= internalLayout().GetSize()))
				EASTL_FAIL_MSG("basic_string::at -- out of range");
		#endif

		return internalLayout().BeginPtr()[n];
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::reference
	basic_string<T, Allocator>::front()
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY(internalLayout().GetSize() <= 0)) // We assert if the user references the trailing 0 char.
				EASTL_FAIL_MSG("basic_string::front -- empty string");
		#else
			// We allow the user to reference the trailing 0 char without asserting.
		#endif

		return *internalLayout().BeginPtr();
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::const_reference
	basic_string<T, Allocator>::front() const
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY(internalLayout().GetSize() <= 0)) // We assert if the user references the trailing 0 char.
				EASTL_FAIL_MSG("basic_string::front -- empty string");
		#else
			// We allow the user to reference the trailing 0 char without asserting.
		#endif

		return *internalLayout().BeginPtr();
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::reference
	basic_string<T, Allocator>::back()
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY(internalLayout().GetSize() <= 0)) // We assert if the user references the trailing 0 char.
				EASTL_FAIL_MSG("basic_string::back -- empty string");
		#else
			// We allow the user to reference the trailing 0 char without asserting.
		#endif

		return *(internalLayout().EndPtr() - 1);
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::const_reference
	basic_string<T, Allocator>::back() const
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY(internalLayout().GetSize() <= 0)) // We assert if the user references the trailing 0 char.
				EASTL_FAIL_MSG("basic_string::back -- empty string");
		#else
			// We allow the user to reference the trailing 0 char without asserting.
		#endif

		return *(internalLayout().EndPtr() - 1);
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::operator+=(const this_type& x)
	{
		return append(x);
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::operator+=(const value_type* p)
	{
		return append(p);
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::operator+=(value_type c)
	{
		push_back(c);
		return *this;
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::append(const this_type& x)
	{
		return append(x.internalLayout().BeginPtr(), x.internalLayout().EndPtr());
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::append(const this_type& x, size_type position, size_type n)
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(position >= x.internalLayout().GetSize())) // position must be < x.mpEnd, but position + n may be > mpEnd.
				ThrowRangeException();
		#endif

		    return append(x.internalLayout().BeginPtr() + position,
				          x.internalLayout().BeginPtr() + position + eastl::min_alt(n, x.internalLayout().GetSize() - position));
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::append(const value_type* p, size_type n)
	{
		return append(p, p + n);
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::append(const value_type* p)
	{
		return append(p, p + CharStrlen(p));
	}


	template <typename T, typename Allocator>
	template <typename OtherCharType>
	basic_string<T, Allocator>& basic_string<T, Allocator>::append_convert(const OtherCharType* pOther)
	{
		return append_convert(pOther, (size_type)CharStrlen(pOther));
	}


	template <typename T, typename Allocator>
	template <typename OtherStringType>
	basic_string<T, Allocator>& basic_string<T, Allocator>::append_convert(const OtherStringType& x)
	{
		return append_convert(x.c_str(), x.length());
	}


	template <typename T, typename Allocator>
	template <typename OtherCharType>
	basic_string<T, Allocator>& basic_string<T, Allocator>::append_convert(const OtherCharType* pOther, size_type n)
	{
		// Question: What do we do in the case that we have an illegally encoded source string?
		// This can happen with UTF8 strings. Do we throw an exception or do we ignore the input?
		// One argument is that it's not a string class' job to handle the security aspects of a
		// program and the higher level application code should be verifying UTF8 string validity,
		// and thus we should do the friendly thing and ignore the invalid characters as opposed
		// to making the user of this function handle exceptions that are easily forgotten.

		const size_t         kBufferSize = 512;
		value_type           selfBuffer[kBufferSize];   // This assumes that value_type is one of char8_t, char16_t, char32_t, or wchar_t. Or more importantly, a type with a trivial constructor and destructor.
		value_type* const    selfBufferEnd = selfBuffer + kBufferSize;
		const OtherCharType* pOtherEnd = pOther + n;

		while(pOther != pOtherEnd)
		{
			value_type* pSelfBufferCurrent = selfBuffer;
			DecodePart(pOther, pOtherEnd, pSelfBufferCurrent, selfBufferEnd);   // Write pOther to pSelfBuffer, converting encoding as we go. We currently ignore the return value, as we don't yet have a plan for handling encoding errors.
			append(selfBuffer, pSelfBufferCurrent);
		}

		return *this;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::append(size_type n, value_type c)
	{
		if (n > 0)
		{
			const size_type nSize = internalLayout().GetSize();
			const size_type nCapacity = capacity();

			if((nSize + n) > nCapacity)
				reserve(GetNewCapacity(nCapacity, (nSize + n) - nCapacity));

			pointer pNewEnd = CharStringUninitializedFillN(internalLayout().EndPtr(), n, c);
			*pNewEnd = 0;
			internalLayout().SetSize(nSize + n);
		}

		return *this;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::append(const value_type* pBegin, const value_type* pEnd)
	{
		if(pBegin != pEnd)
		{
			const size_type nOldSize  = internalLayout().GetSize();
			const size_type n         = (size_type)(pEnd - pBegin);
			const size_type nCapacity = capacity();
			const size_type nNewSize = nOldSize + n;

			if(nNewSize > nCapacity)
			{
				const size_type nLength = GetNewCapacity(nCapacity, nNewSize - nCapacity);

				pointer pNewBegin = DoAllocate(nLength + 1);

				pointer pNewEnd = CharStringUninitializedCopy(internalLayout().BeginPtr(), internalLayout().EndPtr(), pNewBegin);
				pNewEnd         = CharStringUninitializedCopy(pBegin,  pEnd,  pNewEnd);
			   *pNewEnd         = 0;

				DeallocateSelf();
				internalLayout().SetHeapBeginPtr(pNewBegin);
				internalLayout().SetHeapCapacity(nLength);
				internalLayout().SetHeapSize(nNewSize);
			}
			else
			{
				pointer pNewEnd = CharStringUninitializedCopy(pBegin, pEnd, internalLayout().EndPtr());
				*pNewEnd = 0;
				internalLayout().SetSize(nNewSize);
			}
		}

		return *this;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::append_sprintf_va_list(const value_type* pFormat, va_list arguments)
	{
		// From unofficial C89 extension documentation:
		// The vsnprintf returns the number of characters written into the array,
		// not counting the terminating null character, or a negative value
		// if count or more characters are requested to be generated.
		// An error can occur while converting a value for output.

		// From the C99 standard:
		// The vsnprintf function returns the number of characters that would have
		// been written had n been sufficiently large, not counting the terminating
		// null character, or a negative value if an encoding error occurred.
		// Thus, the null-terminated output has been completely written if and only
		// if the returned value is nonnegative and less than n.

		// https://www.freebsd.org/cgi/man.cgi?query=vswprintf&sektion=3&manpath=freebsd-release-ports
		// https://www.freebsd.org/cgi/man.cgi?query=snprintf&manpath=SuSE+Linux/i386+11.3
		// Well its time to go on an adventure...
		// C99 vsnprintf states that a buffer size of zero returns the number of characters that would
		// be written to the buffer irrelevant of whether the buffer is a nullptr
		// But C99 vswprintf for wchar_t changes the behaviour of the return to instead say that it
		// "will fail if n or more wide characters were requested to be written", so
		// calling vswprintf with a buffer size of zero always returns -1
		// unless... you are MSVC where they deviate from the std and say if the buffer is NULL
		// and the size is zero it will return the number of characters written or if we are using
		// EAStdC which also does the sane behaviour.

#if !EASTL_OPENSOURCE || defined(EA_PLATFORM_MICROSOFT)
		size_type nInitialSize = internalLayout().GetSize();
		int nReturnValue;

		#if EASTL_VA_COPY_ENABLED
			va_list argumentsSaved;
			va_copy(argumentsSaved, arguments);
		#endif

		nReturnValue = eastl::Vsnprintf(nullptr, 0, pFormat, arguments);

		if (nReturnValue > 0)
		{
			resize(nReturnValue + nInitialSize);

		#if EASTL_VA_COPY_ENABLED
			va_end(arguments);
			va_copy(arguments, argumentsSaved);
		#endif

			nReturnValue = eastl::Vsnprintf(internalLayout().BeginPtr() + nInitialSize, static_cast<size_t>(nReturnValue) + 1, pFormat, arguments);
		}

		if (nReturnValue >= 0)
			internalLayout().SetSize(nInitialSize + nReturnValue);

		#if EASTL_VA_COPY_ENABLED
			// va_end for arguments will be called by the caller.
			va_end(argumentsSaved);
		#endif

#else
		size_type nInitialSize = internalLayout().GetSize();
		size_type nInitialRemainingCapacity = internalLayout().GetRemainingCapacity();
		int       nReturnValue;

		#if EASTL_VA_COPY_ENABLED
			va_list argumentsSaved;
			va_copy(argumentsSaved, arguments);
		#endif

		nReturnValue = eastl::Vsnprintf(internalLayout().EndPtr(), (size_t)nInitialRemainingCapacity + 1,
										pFormat, arguments);

		if(nReturnValue >= (int)(nInitialRemainingCapacity + 1))  // If there wasn't enough capacity...
		{
			// In this case we definitely have C99 Vsnprintf behaviour.
		#if EASTL_VA_COPY_ENABLED
			va_end(arguments);
			va_copy(arguments, argumentsSaved);
		#endif
			resize(nInitialSize + nReturnValue);
			nReturnValue = eastl::Vsnprintf(internalLayout().BeginPtr() + nInitialSize, (size_t)(nReturnValue + 1),
											pFormat, arguments);
		}
		else if(nReturnValue < 0) // If vsnprintf is non-C99-standard
		{
			// In this case we either have C89 extension behaviour or C99 behaviour.
			size_type n = eastl::max_alt((size_type)(SSOLayout::SSO_CAPACITY - 1), (size_type)(nInitialSize * 2));

			for(; (nReturnValue < 0) && (n < 1000000); n *= 2)
			{
			#if EASTL_VA_COPY_ENABLED
				va_end(arguments);
				va_copy(arguments, argumentsSaved);
			#endif
				resize(n);

				const size_t nCapacity = (size_t)(n - nInitialSize);
				nReturnValue = eastl::Vsnprintf(internalLayout().BeginPtr() + nInitialSize, nCapacity + 1, pFormat, arguments);

				if(nReturnValue == (int)(unsigned)nCapacity)
				{
					resize(++n);
					nReturnValue = eastl::Vsnprintf(internalLayout().BeginPtr() + nInitialSize, nCapacity + 2, pFormat, arguments);
				}
			}
		}

		if(nReturnValue >= 0)
			internalLayout().SetSize(nInitialSize + nReturnValue);

		#if EASTL_VA_COPY_ENABLED
			// va_end for arguments will be called by the caller.
			va_end(argumentsSaved);
		#endif

#endif // EASTL_OPENSOURCE

		return *this;
	}

	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::append_sprintf(const value_type* pFormat, ...)
	{
		va_list arguments;
		va_start(arguments, pFormat);
		append_sprintf_va_list(pFormat, arguments);
		va_end(arguments);

		return *this;
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::push_back(value_type c)
	{
		append((size_type)1, c);
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::pop_back()
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(internalLayout().GetSize() <= 0))
				EASTL_FAIL_MSG("basic_string::pop_back -- empty string");
		#endif

		internalLayout().EndPtr()[-1] = value_type(0);
		internalLayout().SetSize(internalLayout().GetSize() - 1);
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::assign(const this_type& x)
	{
		// The C++11 Standard 21.4.6.3 p6 specifies that assign from this_type assigns contents only and not the allocator.
		return assign(x.internalLayout().BeginPtr(), x.internalLayout().EndPtr());
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::assign(const this_type& x, size_type position, size_type n)
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(position > x.internalLayout().GetSize()))
				ThrowRangeException();
		#endif

		// The C++11 Standard 21.4.6.3 p6 specifies that assign from this_type assigns contents only and not the allocator.
		    return assign(
		        x.internalLayout().BeginPtr() + position,
		        x.internalLayout().BeginPtr() + position + eastl::min_alt(n, x.internalLayout().GetSize() - position));
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::assign(const value_type* p, size_type n)
	{
		return assign(p, p + n);
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::assign(const value_type* p)
	{
		return assign(p, p + CharStrlen(p));
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::assign(size_type n, value_type c)
	{
		if(n <= internalLayout().GetSize())
		{
			CharTypeAssignN(internalLayout().BeginPtr(), n, c);
			erase(internalLayout().BeginPtr() + n, internalLayout().EndPtr());
		}
		else
		{
			CharTypeAssignN(internalLayout().BeginPtr(), internalLayout().GetSize(), c);
			append(n - internalLayout().GetSize(), c);
		}
		return *this;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::assign(const value_type* pBegin, const value_type* pEnd)
	{
		const size_type n = (size_type)(pEnd - pBegin);
		if(n <= internalLayout().GetSize())
		{
			memmove(internalLayout().BeginPtr(), pBegin, (size_t)n * sizeof(value_type));
			erase(internalLayout().BeginPtr() + n, internalLayout().EndPtr());
		}
		else
		{
			memmove(internalLayout().BeginPtr(), pBegin, (size_t)(internalLayout().GetSize()) * sizeof(value_type));
			append(pBegin + internalLayout().GetSize(), pEnd);
		}
		return *this;
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::assign(std::initializer_list<value_type> ilist)
	{
		return assign(ilist.begin(), ilist.end());
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::assign(this_type&& x)
	{
		if(get_allocator() == x.get_allocator())
		{
			eastl::swap(internalLayout(), x.internalLayout());
		}
		else
			assign(x.internalLayout().BeginPtr(), x.internalLayout().EndPtr());

		return *this;
	}


	template <typename T, typename Allocator>
	template <typename OtherCharType>
	basic_string<T, Allocator>& basic_string<T, Allocator>::assign_convert(const OtherCharType* p)
	{
		clear();
		append_convert(p);
		return *this;
	}


	template <typename T, typename Allocator>
	template <typename OtherCharType>
	basic_string<T, Allocator>& basic_string<T, Allocator>::assign_convert(const OtherCharType* p, size_type n)
	{
		clear();
		append_convert(p, n);
		return *this;
	}


	template <typename T, typename Allocator>
	template <typename OtherStringType>
	basic_string<T, Allocator>& basic_string<T, Allocator>::assign_convert(const OtherStringType& x)
	{
		clear();
		append_convert(x.data(), x.length());
		return *this;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::insert(size_type position, const this_type& x)
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(position > internalLayout().GetSize()))
				ThrowRangeException();
		#endif

		#if EASTL_STRING_OPT_LENGTH_ERRORS
			if(EASTL_UNLIKELY(internalLayout().GetSize() > (max_size() - x.internalLayout().GetSize())))
				ThrowLengthException();
		#endif

		insert(internalLayout().BeginPtr() + position, x.internalLayout().BeginPtr(), x.internalLayout().EndPtr());
		return *this;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::insert(size_type position, const this_type& x, size_type beg, size_type n)
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY((position > internalLayout().GetSize()) || (beg > x.internalLayout().GetSize())))
				ThrowRangeException();
		#endif

		size_type nLength = eastl::min_alt(n, x.internalLayout().GetSize() - beg);

		#if EASTL_STRING_OPT_LENGTH_ERRORS
			if(EASTL_UNLIKELY(internalLayout().GetSize() > (max_size() - nLength)))
				ThrowLengthException();
		#endif

		insert(internalLayout().BeginPtr() + position, x.internalLayout().BeginPtr() + beg, x.internalLayout().BeginPtr() + beg + nLength);
		return *this;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::insert(size_type position, const value_type* p, size_type n)
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(position > internalLayout().GetSize()))
				ThrowRangeException();
		#endif

		#if EASTL_STRING_OPT_LENGTH_ERRORS
			if(EASTL_UNLIKELY(internalLayout().GetSize() > (max_size() - n)))
				ThrowLengthException();
		#endif

		insert(internalLayout().BeginPtr() + position, p, p + n);
		return *this;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::insert(size_type position, const value_type* p)
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(position > internalLayout().GetSize()))
				ThrowRangeException();
		#endif

		size_type nLength = (size_type)CharStrlen(p);

		#if EASTL_STRING_OPT_LENGTH_ERRORS
			if(EASTL_UNLIKELY(internalLayout().GetSize() > (max_size() - nLength)))
				ThrowLengthException();
		#endif

		insert(internalLayout().BeginPtr() + position, p, p + nLength);
		return *this;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::insert(size_type position, size_type n, value_type c)
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(position > internalLayout().GetSize()))
				ThrowRangeException();
		#endif

		#if EASTL_STRING_OPT_LENGTH_ERRORS
			if(EASTL_UNLIKELY(internalLayout().GetSize() > (max_size() - n)))
				ThrowLengthException();
		#endif

		insert(internalLayout().BeginPtr() + position, n, c);
		return *this;
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::iterator
	basic_string<T, Allocator>::insert(const_iterator p, value_type c)
	{
		if(p == internalLayout().EndPtr())
		{
			push_back(c);
			return internalLayout().EndPtr() - 1;
		}
		return InsertInternal(p, c);
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::iterator
	basic_string<T, Allocator>::insert(const_iterator p, size_type n, value_type c)
	{
		const difference_type nPosition = (p - internalLayout().BeginPtr()); // Save this because we might reallocate.

		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY((p < internalLayout().BeginPtr()) || (p > internalLayout().EndPtr())))
				EASTL_FAIL_MSG("basic_string::insert -- invalid position");
		#endif

		if(n) // If there is anything to insert...
		{
			if(internalLayout().GetRemainingCapacity() >= n) // If we have enough capacity...
			{
				const size_type nElementsAfter = (size_type)(internalLayout().EndPtr() - p);

				if(nElementsAfter >= n) // If there's enough space for the new chars between the insert position and the end...
				{
					// Ensure we save the size before we do the copy, as we might overwrite the size field with the NULL
					// terminator in the edge case where we are inserting enough characters to equal our capacity
					const size_type nSavedSize = internalLayout().GetSize();
					CharStringUninitializedCopy((internalLayout().EndPtr() - n) + 1, internalLayout().EndPtr() + 1, internalLayout().EndPtr() + 1);
					internalLayout().SetSize(nSavedSize + n);
					memmove(const_cast<value_type*>(p) + n, p, (size_t)((nElementsAfter - n) + 1) * sizeof(value_type));
					CharTypeAssignN(const_cast<value_type*>(p), n, c);
				}
				else
				{
					pointer pOldEnd = internalLayout().EndPtr();
					#if EASTL_EXCEPTIONS_ENABLED
						const size_type nOldSize = internalLayout().GetSize();
					#endif
					CharStringUninitializedFillN(internalLayout().EndPtr() + 1, n - nElementsAfter - 1, c);
					internalLayout().SetSize(internalLayout().GetSize() + (n - nElementsAfter));

					#if EASTL_EXCEPTIONS_ENABLED
						try
						{
					#endif
							// See comment in if block above
							const size_type nSavedSize = internalLayout().GetSize();
							CharStringUninitializedCopy(p, pOldEnd + 1, internalLayout().EndPtr());
							internalLayout().SetSize(nSavedSize + nElementsAfter);
					#if EASTL_EXCEPTIONS_ENABLED
						}
						catch(...)
						{
							internalLayout().SetSize(nOldSize);
							throw;
						}
					#endif

					CharTypeAssignN(const_cast<value_type*>(p), nElementsAfter + 1, c);
				}
			}
			else
			{
				const size_type nOldSize = internalLayout().GetSize();
				const size_type nOldCap  = capacity();
				const size_type nLength  = GetNewCapacity(nOldCap, (nOldSize + n) - nOldCap);

				iterator pNewBegin = DoAllocate(nLength + 1);

				iterator pNewEnd = CharStringUninitializedCopy(internalLayout().BeginPtr(), p, pNewBegin);
				pNewEnd          = CharStringUninitializedFillN(pNewEnd, n, c);
				pNewEnd          = CharStringUninitializedCopy(p, internalLayout().EndPtr(), pNewEnd);
			   *pNewEnd          = 0;

				DeallocateSelf();
				internalLayout().SetHeapBeginPtr(pNewBegin);
				internalLayout().SetHeapCapacity(nLength);
				internalLayout().SetHeapSize(nOldSize + n);
			}
		}

		return internalLayout().BeginPtr() + nPosition;
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::iterator
	basic_string<T, Allocator>::insert(const_iterator p, const value_type* pBegin, const value_type* pEnd)
	{
		const difference_type nPosition = (p - internalLayout().BeginPtr()); // Save this because we might reallocate.

		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY((p < internalLayout().BeginPtr()) || (p > internalLayout().EndPtr())))
				EASTL_FAIL_MSG("basic_string::insert -- invalid position");
		#endif

		const size_type n = (size_type)(pEnd - pBegin);

		if(n)
		{
			const bool bCapacityIsSufficient = (internalLayout().GetRemainingCapacity() >= n);
			const bool bSourceIsFromSelf     = ((pEnd >= internalLayout().BeginPtr()) && (pBegin <= internalLayout().EndPtr()));

			if(bSourceIsFromSelf && internalLayout().IsSSO())
			{
				// pBegin to pEnd will be <= this->GetSize(), so stackTemp will guaranteed be an SSO String
				// If we are inserting ourself into ourself and we are SSO, then on the recursive call we can
				// guarantee 0 or 1 allocation depending if we need to realloc
				// We don't do this for Heap strings as then this path may do 1 or 2 allocations instead of
				// only 1 allocation when we fall through to the last else case below
				const this_type stackTemp(pBegin, pEnd, get_allocator());
				return insert(p, stackTemp.data(), stackTemp.data() + stackTemp.size());
			}

			// If bSourceIsFromSelf is true, then we reallocate. This is because we are
			// inserting ourself into ourself and thus both the source and destination
			// be modified, making it rather tricky to attempt to do in place. The simplest
			// resolution is to reallocate. To consider: there may be a way to implement this
			// whereby we don't need to reallocate or can often avoid reallocating.
			if(bCapacityIsSufficient && !bSourceIsFromSelf)
			{
				const size_type nElementsAfter = (size_type)(internalLayout().EndPtr() - p);

				if(nElementsAfter >= n) // If there are enough characters between insert pos and end
				{
					// Ensure we save the size before we do the copy, as we might overwrite the size field with the NULL
					// terminator in the edge case where we are inserting enough characters to equal our capacity
					const size_type nSavedSize = internalLayout().GetSize();
					CharStringUninitializedCopy((internalLayout().EndPtr() - n) + 1, internalLayout().EndPtr() + 1, internalLayout().EndPtr() + 1);
					internalLayout().SetSize(nSavedSize + n);
					memmove(const_cast<value_type*>(p) + n, p, (size_t)((nElementsAfter - n) + 1) * sizeof(value_type));
					memmove(const_cast<value_type*>(p), pBegin, (size_t)(n) * sizeof(value_type));
				}
				else
				{
					pointer pOldEnd = internalLayout().EndPtr();
					#if EASTL_EXCEPTIONS_ENABLED
						const size_type nOldSize = internalLayout().GetSize();
					#endif
					const value_type* const pMid = pBegin + (nElementsAfter + 1);

					CharStringUninitializedCopy(pMid, pEnd, internalLayout().EndPtr() + 1);
					internalLayout().SetSize(internalLayout().GetSize() + (n - nElementsAfter));

					#if EASTL_EXCEPTIONS_ENABLED
						try
						{
					#endif
							// See comment in if block above
							const size_type nSavedSize = internalLayout().GetSize();
							CharStringUninitializedCopy(p, pOldEnd + 1, internalLayout().EndPtr());
							internalLayout().SetSize(nSavedSize + nElementsAfter);
					#if EASTL_EXCEPTIONS_ENABLED
						}
						catch(...)
						{
							internalLayout().SetSize(nOldSize);
							throw;
						}
					#endif

					CharStringUninitializedCopy(pBegin, pMid, const_cast<value_type*>(p));
				}
			}
			else // Else we need to reallocate to implement this.
			{
				const size_type nOldSize = internalLayout().GetSize();
				const size_type nOldCap  = capacity();
				size_type nLength;

				if(bCapacityIsSufficient) // If bCapacityIsSufficient is true, then bSourceIsFromSelf must be true.
					nLength = nOldSize + n;
				else
					nLength = GetNewCapacity(nOldCap, (nOldSize + n) - nOldCap);

				pointer pNewBegin = DoAllocate(nLength + 1);

				pointer pNewEnd = CharStringUninitializedCopy(internalLayout().BeginPtr(), p, pNewBegin);
				pNewEnd         = CharStringUninitializedCopy(pBegin, pEnd, pNewEnd);
				pNewEnd         = CharStringUninitializedCopy(p, internalLayout().EndPtr(), pNewEnd);
			   *pNewEnd         = 0;

				DeallocateSelf();
				internalLayout().SetHeapBeginPtr(pNewBegin);
				internalLayout().SetHeapCapacity(nLength);
				internalLayout().SetHeapSize(nOldSize + n);
			}
		}

		return internalLayout().BeginPtr() + nPosition;
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::iterator
	basic_string<T, Allocator>::insert(const_iterator p, std::initializer_list<value_type> ilist)
	{
		return insert(p, ilist.begin(), ilist.end());
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::erase(size_type position, size_type n)
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(position > internalLayout().GetSize()))
				ThrowRangeException();
		#endif

		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(position > internalLayout().GetSize()))
				EASTL_FAIL_MSG("basic_string::erase -- invalid position");
		#endif

		erase(internalLayout().BeginPtr() + position,
			  internalLayout().BeginPtr() + position + eastl::min_alt(n, internalLayout().GetSize() - position));

		return *this;
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::iterator
	basic_string<T, Allocator>::erase(const_iterator p)
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY((p < internalLayout().BeginPtr()) || (p >= internalLayout().EndPtr())))
				EASTL_FAIL_MSG("basic_string::erase -- invalid position");
		#endif

		memmove(const_cast<value_type*>(p), p + 1, (size_t)(internalLayout().EndPtr() - p) * sizeof(value_type));
		internalLayout().SetSize(internalLayout().GetSize() - 1);
		return const_cast<value_type*>(p);
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::iterator
	basic_string<T, Allocator>::erase(const_iterator pBegin, const_iterator pEnd)
	{
		#if EASTL_ASSERT_ENABLED
			if (EASTL_UNLIKELY((pBegin < internalLayout().BeginPtr()) || (pBegin > internalLayout().EndPtr()) ||
							   (pEnd < internalLayout().BeginPtr()) || (pEnd > internalLayout().EndPtr()) || (pEnd < pBegin)))
			    EASTL_FAIL_MSG("basic_string::erase -- invalid position");
		#endif

		if(pBegin != pEnd)
		{
			memmove(const_cast<value_type*>(pBegin), pEnd, (size_t)((internalLayout().EndPtr() - pEnd) + 1) * sizeof(value_type));
			const size_type n = (size_type)(pEnd - pBegin);
			internalLayout().SetSize(internalLayout().GetSize() - n);
		}
		return const_cast<value_type*>(pBegin);
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::reverse_iterator
	basic_string<T, Allocator>::erase(reverse_iterator position)
	{
		return reverse_iterator(erase((++position).base()));
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::reverse_iterator
	basic_string<T, Allocator>::erase(reverse_iterator first, reverse_iterator last)
	{
		return reverse_iterator(erase((++last).base(), (++first).base()));
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::replace(size_type position, size_type n, const this_type& x)
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(position > internalLayout().GetSize()))
				ThrowRangeException();
		#endif

		const size_type nLength = eastl::min_alt(n, internalLayout().GetSize() - position);

		#if EASTL_STRING_OPT_LENGTH_ERRORS
			if(EASTL_UNLIKELY((internalLayout().GetSize() - nLength) >= (max_size() - x.internalLayout().GetSize())))
				ThrowLengthException();
		#endif

		return replace(internalLayout().BeginPtr() + position, internalLayout().BeginPtr() + position + nLength, x.internalLayout().BeginPtr(), x.internalLayout().EndPtr());
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::replace(size_type pos1, size_type n1, const this_type& x, size_type pos2, size_type n2)
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
		if(EASTL_UNLIKELY((pos1 > internalLayout().GetSize()) || (pos2 > x.internalLayout().GetSize())))
				ThrowRangeException();
		#endif

		const size_type nLength1 = eastl::min_alt(n1, internalLayout().GetSize() - pos1);
		const size_type nLength2 = eastl::min_alt(n2, x.internalLayout().GetSize() - pos2);

		#if EASTL_STRING_OPT_LENGTH_ERRORS
			if(EASTL_UNLIKELY((internalLayout().GetSize() - nLength1) >= (max_size() - nLength2)))
				ThrowLengthException();
		#endif

		return replace(internalLayout().BeginPtr() + pos1, internalLayout().BeginPtr() + pos1 + nLength1, x.internalLayout().BeginPtr() + pos2, x.internalLayout().BeginPtr() + pos2 + nLength2);
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::replace(size_type position, size_type n1, const value_type* p, size_type n2)
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(position > internalLayout().GetSize()))
				ThrowRangeException();
		#endif

		const size_type nLength = eastl::min_alt(n1, internalLayout().GetSize() - position);

		#if EASTL_STRING_OPT_LENGTH_ERRORS
			if(EASTL_UNLIKELY((n2 > max_size()) || ((internalLayout().GetSize() - nLength) >= (max_size() - n2))))
				ThrowLengthException();
		#endif

		return replace(internalLayout().BeginPtr() + position, internalLayout().BeginPtr() + position + nLength, p, p + n2);
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::replace(size_type position, size_type n1, const value_type* p)
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(position > internalLayout().GetSize()))
				ThrowRangeException();
		#endif

		const size_type nLength = eastl::min_alt(n1, internalLayout().GetSize() - position);

		#if EASTL_STRING_OPT_LENGTH_ERRORS
			const size_type n2 = (size_type)CharStrlen(p);
			if(EASTL_UNLIKELY((n2 > max_size()) || ((internalLayout().GetSize() - nLength) >= (max_size() - n2))))
				ThrowLengthException();
		#endif

		return replace(internalLayout().BeginPtr() + position, internalLayout().BeginPtr() + position + nLength, p, p + CharStrlen(p));
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::replace(size_type position, size_type n1, size_type n2, value_type c)
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(position > internalLayout().GetSize()))
				ThrowRangeException();
		#endif

		const size_type nLength = eastl::min_alt(n1, internalLayout().GetSize() - position);

		#if EASTL_STRING_OPT_LENGTH_ERRORS
			if(EASTL_UNLIKELY((n2 > max_size()) || (internalLayout().GetSize() - nLength) >= (max_size() - n2)))
				ThrowLengthException();
		#endif

		return replace(internalLayout().BeginPtr() + position, internalLayout().BeginPtr() + position + nLength, n2, c);
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::replace(const_iterator pBegin, const_iterator pEnd, const this_type& x)
	{
		return replace(pBegin, pEnd, x.internalLayout().BeginPtr(), x.internalLayout().EndPtr());
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::replace(const_iterator pBegin, const_iterator pEnd, const value_type* p, size_type n)
	{
		return replace(pBegin, pEnd, p, p + n);
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::replace(const_iterator pBegin, const_iterator pEnd, const value_type* p)
	{
		return replace(pBegin, pEnd, p, p + CharStrlen(p));
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::replace(const_iterator pBegin, const_iterator pEnd, size_type n, value_type c)
	{
		#if EASTL_ASSERT_ENABLED
			if (EASTL_UNLIKELY((pBegin < internalLayout().BeginPtr()) || (pBegin > internalLayout().EndPtr()) ||
							   (pEnd < internalLayout().BeginPtr()) || (pEnd > internalLayout().EndPtr()) || (pEnd < pBegin)))
			    EASTL_FAIL_MSG("basic_string::replace -- invalid position");
		#endif

		const size_type nLength = static_cast<size_type>(pEnd - pBegin);

		if(nLength >= n)
		{
			CharTypeAssignN(const_cast<value_type*>(pBegin), n, c);
			erase(pBegin + n, pEnd);
		}
		else
		{
			CharTypeAssignN(const_cast<value_type*>(pBegin), nLength, c);
			insert(pEnd, n - nLength, c);
		}
		return *this;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::replace(const_iterator pBegin1, const_iterator pEnd1, const value_type* pBegin2, const value_type* pEnd2)
	{
		#if EASTL_ASSERT_ENABLED
			if (EASTL_UNLIKELY((pBegin1 < internalLayout().BeginPtr()) || (pBegin1 > internalLayout().EndPtr()) ||
							   (pEnd1 < internalLayout().BeginPtr()) || (pEnd1 > internalLayout().EndPtr()) || (pEnd1 < pBegin1)))
			    EASTL_FAIL_MSG("basic_string::replace -- invalid position");
		#endif

		const size_type nLength1 = (size_type)(pEnd1 - pBegin1);
		const size_type nLength2 = (size_type)(pEnd2 - pBegin2);

		if(nLength1 >= nLength2) // If we have a non-expanding operation...
		{
			if((pBegin2 > pEnd1) || (pEnd2 <= pBegin1))  // If we have a non-overlapping operation...
				memcpy(const_cast<value_type*>(pBegin1), pBegin2, (size_t)(pEnd2 - pBegin2) * sizeof(value_type));
			else
				memmove(const_cast<value_type*>(pBegin1), pBegin2, (size_t)(pEnd2 - pBegin2) * sizeof(value_type));
			erase(pBegin1 + nLength2, pEnd1);
		}
		else // Else we are expanding.
		{
			if((pBegin2 > pEnd1) || (pEnd2 <= pBegin1)) // If we have a non-overlapping operation...
			{
				const value_type* const pMid2 = pBegin2 + nLength1;

				if((pEnd2 <= pBegin1) || (pBegin2 > pEnd1))
					memcpy(const_cast<value_type*>(pBegin1), pBegin2, (size_t)(pMid2 - pBegin2) * sizeof(value_type));
				else
					memmove(const_cast<value_type*>(pBegin1), pBegin2, (size_t)(pMid2 - pBegin2) * sizeof(value_type));
				insert(pEnd1, pMid2, pEnd2);
			}
			else // else we have an overlapping operation.
			{
				// I can't think of any easy way of doing this without allocating temporary memory.
				const size_type nOldSize     = internalLayout().GetSize();
				const size_type nOldCap      = capacity();
				const size_type nNewCapacity = GetNewCapacity(nOldCap, (nOldSize + (nLength2 - nLength1)) - nOldCap);

				pointer pNewBegin = DoAllocate(nNewCapacity + 1);

				pointer pNewEnd = CharStringUninitializedCopy(internalLayout().BeginPtr(), pBegin1, pNewBegin);
				pNewEnd         = CharStringUninitializedCopy(pBegin2, pEnd2,   pNewEnd);
				pNewEnd         = CharStringUninitializedCopy(pEnd1,   internalLayout().EndPtr(),   pNewEnd);
			   *pNewEnd         = 0;

				DeallocateSelf();
				internalLayout().SetHeapBeginPtr(pNewBegin);
				internalLayout().SetHeapCapacity(nNewCapacity);
				internalLayout().SetHeapSize(nOldSize + (nLength2 - nLength1));
			}
		}
		return *this;
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::copy(value_type* p, size_type n, size_type position) const
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(position > internalLayout().GetSize()))
				ThrowRangeException();
		#endif

		// C++ std says the effects of this function are as if calling char_traits::copy()
		// thus the 'p' must not overlap *this string, so we can use memcpy
		const size_type nLength = eastl::min_alt(n, internalLayout().GetSize() - position);
		CharStringUninitializedCopy(internalLayout().BeginPtr() + position, internalLayout().BeginPtr() + position + nLength, p);
		return nLength;
	}


	template <typename T, typename Allocator>
	void basic_string<T, Allocator>::swap(this_type& x)
	{
		if(get_allocator() == x.get_allocator() || (internalLayout().IsSSO() && x.internalLayout().IsSSO())) // If allocators are equivalent...
		{
			// We leave mAllocator as-is.
			eastl::swap(internalLayout(), x.internalLayout());
		}
		else // else swap the contents.
		{
			const this_type temp(*this); // Can't call eastl::swap because that would
			*this = x;                   // itself call this member swap function.
			x     = temp;
		}
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find(const this_type& x, size_type position) const EA_NOEXCEPT
	{
		return find(x.internalLayout().BeginPtr(), position, x.internalLayout().GetSize());
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find(const value_type* p, size_type position) const
	{
		return find(p, position, (size_type)CharStrlen(p));
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find(const value_type* p, size_type position, size_type n) const
	{
		// It is not clear what the requirements are for position, but since the C++ standard
		// appears to be silent it is assumed for now that position can be any value.
		//#if EASTL_ASSERT_ENABLED
		//    if(EASTL_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
		//        EASTL_FAIL_MSG("basic_string::find -- invalid position");
		//#endif

		if(EASTL_LIKELY(((npos - n) >= position) && (position + n) <= internalLayout().GetSize())) // If the range is valid...
		{
			const value_type* const pTemp = eastl::search(internalLayout().BeginPtr() + position, internalLayout().EndPtr(), p, p + n);

			if((pTemp != internalLayout().EndPtr()) || (n == 0))
				return (size_type)(pTemp - internalLayout().BeginPtr());
		}
		return npos;
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find(value_type c, size_type position) const EA_NOEXCEPT
	{
		// It is not clear what the requirements are for position, but since the C++ standard
		// appears to be silent it is assumed for now that position can be any value.
		//#if EASTL_ASSERT_ENABLED
		//    if(EASTL_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
		//        EASTL_FAIL_MSG("basic_string::find -- invalid position");
		//#endif

		if(EASTL_LIKELY(position < internalLayout().GetSize()))// If the position is valid...
		{
			const const_iterator pResult = eastl::find(internalLayout().BeginPtr() + position, internalLayout().EndPtr(), c);

			if(pResult != internalLayout().EndPtr())
				return (size_type)(pResult - internalLayout().BeginPtr());
		}
		return npos;
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::rfind(const this_type& x, size_type position) const EA_NOEXCEPT
	{
		return rfind(x.internalLayout().BeginPtr(), position, x.internalLayout().GetSize());
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::rfind(const value_type* p, size_type position) const
	{
		return rfind(p, position, (size_type)CharStrlen(p));
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::rfind(const value_type* p, size_type position, size_type n) const
	{
		// Disabled because it's not clear what values are valid for position.
		// It is documented that npos is a valid value, though. We return npos and
		// don't crash if postion is any invalid value.
		//#if EASTL_ASSERT_ENABLED
		//    if(EASTL_UNLIKELY((position != npos) && (position > (size_type)(mpEnd - mpBegin))))
		//        EASTL_FAIL_MSG("basic_string::rfind -- invalid position");
		//#endif

		// Note that a search for a zero length string starting at position = end() returns end() and not npos.
		// Note by Paul Pedriana: I am not sure how this should behave in the case of n == 0 and position > size.
		// The standard seems to suggest that rfind doesn't act exactly the same as find in that input position
		// can be > size and the return value can still be other than npos. Thus, if n == 0 then you can
		// never return npos, unlike the case with find.
		const size_type nLength = internalLayout().GetSize();

		if(EASTL_LIKELY(n <= nLength))
		{
			if(EASTL_LIKELY(n))
			{
				const const_iterator pEnd    = internalLayout().BeginPtr() + eastl::min_alt(nLength - n, position) + n;
				const const_iterator pResult = CharTypeStringRSearch(internalLayout().BeginPtr(), pEnd, p, p + n);

				if(pResult != pEnd)
					return (size_type)(pResult - internalLayout().BeginPtr());
			}
			else
				return eastl::min_alt(nLength, position);
		}
		return npos;
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::rfind(value_type c, size_type position) const EA_NOEXCEPT
	{
		// If n is zero or position is >= size, we return npos.
		const size_type nLength = internalLayout().GetSize();

		if(EASTL_LIKELY(nLength))
		{
			const value_type* const pEnd    = internalLayout().BeginPtr() + eastl::min_alt(nLength - 1, position) + 1;
			const value_type* const pResult = CharTypeStringRFind(pEnd, internalLayout().BeginPtr(), c);

			if(pResult != internalLayout().BeginPtr())
				return (size_type)((pResult - 1) - internalLayout().BeginPtr());
		}
		return npos;
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_first_of(const this_type& x, size_type position) const EA_NOEXCEPT
	{
		return find_first_of(x.internalLayout().BeginPtr(), position, x.internalLayout().GetSize());
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_first_of(const value_type* p, size_type position) const
	{
		return find_first_of(p, position, (size_type)CharStrlen(p));
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_first_of(const value_type* p, size_type position, size_type n) const
	{
		// If position is >= size, we return npos.
		if(EASTL_LIKELY((position < internalLayout().GetSize())))
		{
			const value_type* const pBegin = internalLayout().BeginPtr() + position;
			const const_iterator pResult   = CharTypeStringFindFirstOf(pBegin, internalLayout().EndPtr(), p, p + n);

			if(pResult != internalLayout().EndPtr())
				return (size_type)(pResult - internalLayout().BeginPtr());
		}
		return npos;
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_first_of(value_type c, size_type position) const EA_NOEXCEPT
	{
		return find(c, position);
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_last_of(const this_type& x, size_type position) const EA_NOEXCEPT
	{
		return find_last_of(x.internalLayout().BeginPtr(), position, x.internalLayout().GetSize());
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_last_of(const value_type* p, size_type position) const
	{
		return find_last_of(p, position, (size_type)CharStrlen(p));
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_last_of(const value_type* p, size_type position, size_type n) const
	{
		// If n is zero or position is >= size, we return npos.
		const size_type nLength = internalLayout().GetSize();

		if(EASTL_LIKELY(nLength))
		{
			const value_type* const pEnd    = internalLayout().BeginPtr() + eastl::min_alt(nLength - 1, position) + 1;
			const value_type* const pResult = CharTypeStringRFindFirstOf(pEnd, internalLayout().BeginPtr(), p, p + n);

			if(pResult != internalLayout().BeginPtr())
				return (size_type)((pResult - 1) - internalLayout().BeginPtr());
		}
		return npos;
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_last_of(value_type c, size_type position) const EA_NOEXCEPT
	{
		return rfind(c, position);
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_first_not_of(const this_type& x, size_type position) const EA_NOEXCEPT
	{
		return find_first_not_of(x.internalLayout().BeginPtr(), position, x.internalLayout().GetSize());
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_first_not_of(const value_type* p, size_type position) const
	{
		return find_first_not_of(p, position, (size_type)CharStrlen(p));
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_first_not_of(const value_type* p, size_type position, size_type n) const
	{
		if(EASTL_LIKELY(position <= internalLayout().GetSize()))
		{
			const const_iterator pResult =
			    CharTypeStringFindFirstNotOf(internalLayout().BeginPtr() + position, internalLayout().EndPtr(), p, p + n);

			if(pResult != internalLayout().EndPtr())
				return (size_type)(pResult - internalLayout().BeginPtr());
		}
		return npos;
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_first_not_of(value_type c, size_type position) const EA_NOEXCEPT
	{
		if(EASTL_LIKELY(position <= internalLayout().GetSize()))
		{
			// Todo: Possibly make a specialized version of CharTypeStringFindFirstNotOf(pBegin, pEnd, c).
			const const_iterator pResult =
			    CharTypeStringFindFirstNotOf(internalLayout().BeginPtr() + position, internalLayout().EndPtr(), &c, &c + 1);

			if(pResult != internalLayout().EndPtr())
				return (size_type)(pResult - internalLayout().BeginPtr());
		}
		return npos;
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_last_not_of(const this_type& x, size_type position) const EA_NOEXCEPT
	{
		return find_last_not_of(x.internalLayout().BeginPtr(), position, x.internalLayout().GetSize());
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_last_not_of(const value_type* p, size_type position) const
	{
		return find_last_not_of(p, position, (size_type)CharStrlen(p));
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_last_not_of(const value_type* p, size_type position, size_type n) const
	{
		const size_type nLength = internalLayout().GetSize();

		if(EASTL_LIKELY(nLength))
		{
			const value_type* const pEnd    = internalLayout().BeginPtr() + eastl::min_alt(nLength - 1, position) + 1;
			const value_type* const pResult = CharTypeStringRFindFirstNotOf(pEnd, internalLayout().BeginPtr(), p, p + n);

			if(pResult != internalLayout().BeginPtr())
				return (size_type)((pResult - 1) - internalLayout().BeginPtr());
		}
		return npos;
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::find_last_not_of(value_type c, size_type position) const EA_NOEXCEPT
	{
		const size_type nLength = internalLayout().GetSize();

		if(EASTL_LIKELY(nLength))
		{
			// Todo: Possibly make a specialized version of CharTypeStringRFindFirstNotOf(pBegin, pEnd, c).
			const value_type* const pEnd    = internalLayout().BeginPtr() + eastl::min_alt(nLength - 1, position) + 1;
			const value_type* const pResult = CharTypeStringRFindFirstNotOf(pEnd, internalLayout().BeginPtr(), &c, &c + 1);

			if(pResult != internalLayout().BeginPtr())
				return (size_type)((pResult - 1) - internalLayout().BeginPtr());
		}
		return npos;
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator> basic_string<T, Allocator>::substr(size_type position, size_type n) const
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(position > internalLayout().GetSize()))
				ThrowRangeException();
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(position > internalLayout().GetSize()))
				EASTL_FAIL_MSG("basic_string::substr -- invalid position");
		#endif

			// C++ std says the return string allocator must be default constructed, not a copy of this->get_allocator()
			return basic_string(
				internalLayout().BeginPtr() + position,
				internalLayout().BeginPtr() + position +
					eastl::min_alt(n, internalLayout().GetSize() - position), get_allocator());
	}


	template <typename T, typename Allocator>
	inline int basic_string<T, Allocator>::compare(const this_type& x) const EA_NOEXCEPT
	{
		return compare(internalLayout().BeginPtr(), internalLayout().EndPtr(), x.internalLayout().BeginPtr(), x.internalLayout().EndPtr());
	}


	template <typename T, typename Allocator>
	inline int basic_string<T, Allocator>::compare(size_type pos1, size_type n1, const this_type& x) const
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(pos1 > internalLayout().GetSize()))
				ThrowRangeException();
		#endif

		return compare(
			internalLayout().BeginPtr() + pos1,
			internalLayout().BeginPtr() + pos1 + eastl::min_alt(n1, internalLayout().GetSize() - pos1),
			x.internalLayout().BeginPtr(), x.internalLayout().EndPtr());
	}


	template <typename T, typename Allocator>
	inline int basic_string<T, Allocator>::compare(size_type pos1, size_type n1, const this_type& x, size_type pos2, size_type n2) const
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY((pos1 > (size_type)(internalLayout().EndPtr() - internalLayout().BeginPtr())) ||
			                  (pos2 > (size_type)(x.internalLayout().EndPtr() - x.internalLayout().BeginPtr()))))
				ThrowRangeException();
		#endif

		return compare(internalLayout().BeginPtr() + pos1,
					   internalLayout().BeginPtr() + pos1 + eastl::min_alt(n1, internalLayout().GetSize() - pos1),
					   x.internalLayout().BeginPtr() + pos2,
					   x.internalLayout().BeginPtr() + pos2 + eastl::min_alt(n2, x.internalLayout().GetSize() - pos2));
	}


	template <typename T, typename Allocator>
	inline int basic_string<T, Allocator>::compare(const value_type* p) const
	{
		return compare(internalLayout().BeginPtr(), internalLayout().EndPtr(), p, p + CharStrlen(p));
	}


	template <typename T, typename Allocator>
	inline int basic_string<T, Allocator>::compare(size_type pos1, size_type n1, const value_type* p) const
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(pos1 > internalLayout().GetSize()))
				ThrowRangeException();
		#endif

		return compare(internalLayout().BeginPtr() + pos1,
					   internalLayout().BeginPtr() + pos1 + eastl::min_alt(n1, internalLayout().GetSize() - pos1),
					   p,
					   p + CharStrlen(p));
	}


	template <typename T, typename Allocator>
	inline int basic_string<T, Allocator>::compare(size_type pos1, size_type n1, const value_type* p, size_type n2) const
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(EASTL_UNLIKELY(pos1 > internalLayout().GetSize()))
				ThrowRangeException();
		#endif

		return compare(internalLayout().BeginPtr() + pos1,
					   internalLayout().BeginPtr() + pos1 + eastl::min_alt(n1, internalLayout().GetSize() - pos1),
					   p,
					   p + n2);
	}


	// make_lower
	// This is a very simple ASCII-only case conversion function
	// Anything more complicated should use a more powerful separate library.
	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::make_lower()
	{
		for(pointer p = internalLayout().BeginPtr(); p < internalLayout().EndPtr(); ++p)
			*p = (value_type)CharToLower(*p);
	}


	// make_upper
	// This is a very simple ASCII-only case conversion function
	// Anything more complicated should use a more powerful separate library.
	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::make_upper()
	{
		for(pointer p = internalLayout().BeginPtr(); p < internalLayout().EndPtr(); ++p)
			*p = (value_type)CharToUpper(*p);
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::ltrim()
	{
		const value_type array[] = { ' ', '\t', 0 }; // This is a pretty simplistic view of whitespace.
		erase(0, find_first_not_of(array));
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::rtrim()
	{
		const value_type array[] = { ' ', '\t', 0 }; // This is a pretty simplistic view of whitespace.
		erase(find_last_not_of(array) + 1);
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::trim()
	{
		ltrim();
		rtrim();
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::ltrim(const value_type* p)
	{
		erase(0, find_first_not_of(p));
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::rtrim(const value_type* p)
	{
		erase(find_last_not_of(p) + 1);
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::trim(const value_type* p)
	{
		ltrim(p);
		rtrim(p);
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator> basic_string<T, Allocator>::left(size_type n) const
	{
		const size_type nLength = length();
		if(n < nLength)
			return substr(0, n);
		// C++ std says that substr must return default constructed allocated, but we do not.
		// Instead it is much more practical to provide the copy of the current allocator
		return basic_string(*this, get_allocator());
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator> basic_string<T, Allocator>::right(size_type n) const
	{
		const size_type nLength = length();
		if(n < nLength)
			return substr(nLength - n, n);
		// C++ std says that substr must return default constructed allocated, but we do not.
		// Instead it is much more practical to provide the copy of the current allocator
		return basic_string(*this, get_allocator());
	}


	template <typename T, typename Allocator>
	inline basic_string<T, Allocator>& basic_string<T, Allocator>::sprintf(const value_type* pFormat, ...)
	{
		va_list arguments;
		va_start(arguments, pFormat);
		internalLayout().SetSize(0); // Fast truncate to zero length.
		append_sprintf_va_list(pFormat, arguments);
		va_end(arguments);

		return *this;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator>& basic_string<T, Allocator>::sprintf_va_list(const value_type* pFormat, va_list arguments)
	{
		internalLayout().SetSize(0); // Fast truncate to zero length.

		return append_sprintf_va_list(pFormat, arguments);
	}


	template <typename T, typename Allocator>
	int basic_string<T, Allocator>::compare(const value_type* pBegin1, const value_type* pEnd1,
											const value_type* pBegin2, const value_type* pEnd2)
	{
		const difference_type n1   = pEnd1 - pBegin1;
		const difference_type n2   = pEnd2 - pBegin2;
		const difference_type nMin = eastl::min_alt(n1, n2);
		const int       cmp  = Compare(pBegin1, pBegin2, (size_t)nMin);

		return (cmp != 0 ? cmp : (n1 < n2 ? -1 : (n1 > n2 ? 1 : 0)));
	}


	template <typename T, typename Allocator>
	int basic_string<T, Allocator>::comparei(const value_type* pBegin1, const value_type* pEnd1,
											 const value_type* pBegin2, const value_type* pEnd2)
	{
		const difference_type n1   = pEnd1 - pBegin1;
		const difference_type n2   = pEnd2 - pBegin2;
		const difference_type nMin = eastl::min_alt(n1, n2);
		const int       cmp  = CompareI(pBegin1, pBegin2, (size_t)nMin);

		return (cmp != 0 ? cmp : (n1 < n2 ? -1 : (n1 > n2 ? 1 : 0)));
	}


	template <typename T, typename Allocator>
	inline int basic_string<T, Allocator>::comparei(const this_type& x) const EA_NOEXCEPT
	{
		return comparei(internalLayout().BeginPtr(), internalLayout().EndPtr(), x.internalLayout().BeginPtr(), x.internalLayout().EndPtr());
	}


	template <typename T, typename Allocator>
	inline int basic_string<T, Allocator>::comparei(const value_type* p) const
	{
		return comparei(internalLayout().BeginPtr(), internalLayout().EndPtr(), p, p + CharStrlen(p));
	}


	template <typename T, typename Allocator>
	typename basic_string<T, Allocator>::iterator
	basic_string<T, Allocator>::InsertInternal(const_iterator p, value_type c)
	{
		iterator pNewPosition = const_cast<value_type*>(p);

		if((internalLayout().EndPtr() + 1) <= internalLayout().CapacityPtr())
		{
			const size_type nSavedSize = internalLayout().GetSize();
			memmove(const_cast<value_type*>(p) + 1, p, (size_t)(internalLayout().EndPtr() - p) * sizeof(value_type));
			*(internalLayout().EndPtr() + 1) = 0;
			*pNewPosition = c;
			internalLayout().SetSize(nSavedSize + 1);
		}
		else
		{
			const size_type nOldSize = internalLayout().GetSize();
			const size_type nOldCap  = capacity();
			const size_type nLength = GetNewCapacity(nOldCap, 1);

			iterator pNewBegin = DoAllocate(nLength + 1);

			pNewPosition = CharStringUninitializedCopy(internalLayout().BeginPtr(), p, pNewBegin);
		   *pNewPosition = c;

			iterator pNewEnd = pNewPosition + 1;
			pNewEnd          = CharStringUninitializedCopy(p, internalLayout().EndPtr(), pNewEnd);
		   *pNewEnd          = 0;

			DeallocateSelf();
			internalLayout().SetHeapBeginPtr(pNewBegin);
			internalLayout().SetHeapCapacity(nLength);
			internalLayout().SetHeapSize(nOldSize + 1);
		}
		return pNewPosition;
	}


	template <typename T, typename Allocator>
	void basic_string<T, Allocator>::SizeInitialize(size_type n, value_type c)
	{
		AllocateSelf(n);

		CharStringUninitializedFillN(internalLayout().BeginPtr(), n, c);
		*internalLayout().EndPtr() = 0;
	}


	template <typename T, typename Allocator>
	void basic_string<T, Allocator>::RangeInitialize(const value_type* pBegin, const value_type* pEnd)
	{
		#if EASTL_STRING_OPT_ARGUMENT_ERRORS
			if(EASTL_UNLIKELY(!pBegin && (pEnd < pBegin))) // 21.4.2 p7
				ThrowInvalidArgumentException();
		#endif

		const size_type n = (size_type)(pEnd - pBegin);

		AllocateSelf(n);

		CharStringUninitializedCopy(pBegin, pEnd, internalLayout().BeginPtr());
		*internalLayout().EndPtr() = 0;
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::RangeInitialize(const value_type* pBegin)
	{
		#if EASTL_STRING_OPT_ARGUMENT_ERRORS
			if(EASTL_UNLIKELY(!pBegin))
				ThrowInvalidArgumentException();
		#endif

		RangeInitialize(pBegin, pBegin + CharStrlen(pBegin));
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::value_type*
	basic_string<T, Allocator>::DoAllocate(size_type n)
	{
		return (value_type*)EASTLAlloc(get_allocator(), n * sizeof(value_type));
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::DoFree(value_type* p, size_type n)
	{
		if(p)
			EASTLFree(get_allocator(), p, n * sizeof(value_type));
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::GetNewCapacity(size_type currentCapacity)
	{
		return GetNewCapacity(currentCapacity, 1);
	}


	template <typename T, typename Allocator>
	inline typename basic_string<T, Allocator>::size_type
	basic_string<T, Allocator>::GetNewCapacity(size_type currentCapacity, size_type minimumGrowSize)
	{
		#if EASTL_STRING_OPT_LENGTH_ERRORS
			const size_type nRemainingSize = max_size() - currentCapacity;
			if(EASTL_UNLIKELY((minimumGrowSize > nRemainingSize)))
			{
				ThrowLengthException();
			}
		#endif

		const size_type nNewCapacity = eastl::max_alt(currentCapacity + minimumGrowSize, currentCapacity * 2);

		return nNewCapacity;
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::AllocateSelf()
	{
		internalLayout().ResetToSSO();
	}


	template <typename T, typename Allocator>
	void basic_string<T, Allocator>::AllocateSelf(size_type n)
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(n >= 0x40000000))
				EASTL_FAIL_MSG("basic_string::AllocateSelf -- improbably large request.");
		#endif

		#if EASTL_STRING_OPT_LENGTH_ERRORS
			if(EASTL_UNLIKELY(n > max_size()))
				ThrowLengthException();
		#endif

		if(n > SSOLayout::SSO_CAPACITY)
		{
			pointer pBegin = DoAllocate(n + 1);
			internalLayout().SetHeapBeginPtr(pBegin);
			internalLayout().SetHeapCapacity(n);
			internalLayout().SetHeapSize(n);
		}
		else
			internalLayout().SetSSOSize(n);
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::DeallocateSelf()
	{
		if(internalLayout().IsHeap())
		{
			DoFree(internalLayout().BeginPtr(), internalLayout().GetHeapCapacity() + 1);
		}
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::ThrowLengthException() const
	{
		#if EASTL_EXCEPTIONS_ENABLED
			throw std::length_error("basic_string -- length_error");
		#elif EASTL_ASSERT_ENABLED
			EASTL_FAIL_MSG("basic_string -- length_error");
		#endif
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::ThrowRangeException() const
	{
		#if EASTL_EXCEPTIONS_ENABLED
			throw std::out_of_range("basic_string -- out of range");
		#elif EASTL_ASSERT_ENABLED
			EASTL_FAIL_MSG("basic_string -- out of range");
		#endif
	}


	template <typename T, typename Allocator>
	inline void basic_string<T, Allocator>::ThrowInvalidArgumentException() const
	{
		#if EASTL_EXCEPTIONS_ENABLED
			throw std::invalid_argument("basic_string -- invalid argument");
		#elif EASTL_ASSERT_ENABLED
			EASTL_FAIL_MSG("basic_string -- invalid argument");
		#endif
	}


	// CharTypeStringFindEnd
	// Specialized char version of STL find() from back function.
	// Not the same as RFind because search range is specified as forward iterators.
	template <typename T, typename Allocator>
	const typename basic_string<T, Allocator>::value_type*
	basic_string<T, Allocator>::CharTypeStringFindEnd(const value_type* pBegin, const value_type* pEnd, value_type c)
	{
		const value_type* pTemp = pEnd;
		while(--pTemp >= pBegin)
		{
			if(*pTemp == c)
				return pTemp;
		}

		return pEnd;
	}


	// CharTypeStringRFind
	// Specialized value_type version of STL find() function in reverse.
	template <typename T, typename Allocator>
	const typename basic_string<T, Allocator>::value_type*
	basic_string<T, Allocator>::CharTypeStringRFind(const value_type* pRBegin, const value_type* pREnd, const value_type c)
	{
		while(pRBegin > pREnd)
		{
			if(*(pRBegin - 1) == c)
				return pRBegin;
			--pRBegin;
		}
		return pREnd;
	}


	// CharTypeStringSearch
	// Specialized value_type version of STL search() function.
	// Purpose: find p2 within p1. Return p1End if not found or if either string is zero length.
	template <typename T, typename Allocator>
	const typename basic_string<T, Allocator>::value_type*
	basic_string<T, Allocator>::CharTypeStringSearch(const value_type* p1Begin, const value_type* p1End,
													 const value_type* p2Begin, const value_type* p2End)
	{
		// Test for zero length strings, in which case we have a match or a failure,
		// but the return value is the same either way.
		if((p1Begin == p1End) || (p2Begin == p2End))
			return p1Begin;

		// Test for a pattern of length 1.
		if((p2Begin + 1) == p2End)
			return eastl::find(p1Begin, p1End, *p2Begin);

		// General case.
		const value_type* pTemp;
		const value_type* pTemp1 = (p2Begin + 1);
		const value_type* pCurrent = p1Begin;

		while(p1Begin != p1End)
		{
			p1Begin = eastl::find(p1Begin, p1End, *p2Begin);
			if(p1Begin == p1End)
				return p1End;

			pTemp = pTemp1;
			pCurrent = p1Begin;
			if(++pCurrent == p1End)
				return p1End;

			while(*pCurrent == *pTemp)
			{
				if(++pTemp == p2End)
					return p1Begin;
				if(++pCurrent == p1End)
					return p1End;
			}

			++p1Begin;
		}

		return p1Begin;
	}


	// CharTypeStringRSearch
	// Specialized value_type version of STL find_end() function (which really is a reverse search function).
	// Purpose: find last instance of p2 within p1. Return p1End if not found or if either string is zero length.
	template <typename T, typename Allocator>
	const typename basic_string<T, Allocator>::value_type*
	basic_string<T, Allocator>::CharTypeStringRSearch(const value_type* p1Begin, const value_type* p1End,
													  const value_type* p2Begin, const value_type* p2End)
	{
		// Test for zero length strings, in which case we have a match or a failure,
		// but the return value is the same either way.
		if((p1Begin == p1End) || (p2Begin == p2End))
			return p1Begin;

		// Test for a pattern of length 1.
		if((p2Begin + 1) == p2End)
			return CharTypeStringFindEnd(p1Begin, p1End, *p2Begin);

		// Test for search string length being longer than string length.
		if((p2End - p2Begin) > (p1End - p1Begin))
			return p1End;

		// General case.
		const value_type* pSearchEnd = (p1End - (p2End - p2Begin) + 1);
		const value_type* pCurrent1;
		const value_type* pCurrent2;

		while(pSearchEnd != p1Begin)
		{
			// Search for the last occurrence of *p2Begin.
			pCurrent1 = CharTypeStringFindEnd(p1Begin, pSearchEnd, *p2Begin);
			if(pCurrent1 == pSearchEnd) // If the first char of p2 wasn't found,
				return p1End;           // then we immediately have failure.

			// In this case, *pTemp == *p2Begin. So compare the rest.
			pCurrent2 = p2Begin;
			while(*pCurrent1++ == *pCurrent2++)
			{
				if(pCurrent2 == p2End)
					return (pCurrent1 - (p2End - p2Begin));
			}

			// A smarter algorithm might know to subtract more than just one,
			// but in most cases it won't make much difference anyway.
			--pSearchEnd;
		}

		return p1End;
	}


	// CharTypeStringFindFirstOf
	// Specialized value_type version of STL find_first_of() function.
	// This function is much like the C runtime strtok function, except the strings aren't null-terminated.
	template <typename T, typename Allocator>
	const typename basic_string<T, Allocator>::value_type*
	basic_string<T, Allocator>::CharTypeStringFindFirstOf(const value_type* p1Begin, const value_type* p1End,
														  const value_type* p2Begin, const value_type* p2End)
	{
		for( ; p1Begin != p1End; ++p1Begin)
		{
			for(const value_type* pTemp = p2Begin; pTemp != p2End; ++pTemp)
			{
				if(*p1Begin == *pTemp)
					return p1Begin;
			}
		}
		return p1End;
	}


	// CharTypeStringRFindFirstOf
	// Specialized value_type version of STL find_first_of() function in reverse.
	// This function is much like the C runtime strtok function, except the strings aren't null-terminated.
	template <typename T, typename Allocator>
	const typename basic_string<T, Allocator>::value_type*
	basic_string<T, Allocator>::CharTypeStringRFindFirstOf(const value_type* p1RBegin, const value_type* p1REnd,
														   const value_type* p2Begin,  const value_type* p2End)
	{
		for( ; p1RBegin != p1REnd; --p1RBegin)
		{
			for(const value_type* pTemp = p2Begin; pTemp != p2End; ++pTemp)
			{
				if(*(p1RBegin - 1) == *pTemp)
					return p1RBegin;
			}
		}
		return p1REnd;
	}



	// CharTypeStringFindFirstNotOf
	// Specialized value_type version of STL find_first_not_of() function.
	template <typename T, typename Allocator>
	const typename basic_string<T, Allocator>::value_type*
	basic_string<T, Allocator>::CharTypeStringFindFirstNotOf(const value_type* p1Begin, const value_type* p1End,
															 const value_type* p2Begin, const value_type* p2End)
	{
		for( ; p1Begin != p1End; ++p1Begin)
		{
			const value_type* pTemp;
			for(pTemp = p2Begin; pTemp != p2End; ++pTemp)
			{
				if(*p1Begin == *pTemp)
					break;
			}
			if(pTemp == p2End)
				return p1Begin;
		}
		return p1End;
	}


	// CharTypeStringRFindFirstNotOf
	// Specialized value_type version of STL find_first_not_of() function in reverse.
	template <typename T, typename Allocator>
	const typename basic_string<T, Allocator>::value_type*
	basic_string<T, Allocator>::CharTypeStringRFindFirstNotOf(const value_type* p1RBegin, const value_type* p1REnd,
															  const value_type* p2Begin,  const value_type* p2End)
	{
		for( ; p1RBegin != p1REnd; --p1RBegin)
		{
			const value_type* pTemp;
			for(pTemp = p2Begin; pTemp != p2End; ++pTemp)
			{
				if(*(p1RBegin-1) == *pTemp)
					break;
			}
			if(pTemp == p2End)
				return p1RBegin;
		}
		return p1REnd;
	}




	// iterator operators
	template <typename T, typename Allocator>
	inline bool operator==(const typename basic_string<T, Allocator>::reverse_iterator& r1,
						   const typename basic_string<T, Allocator>::reverse_iterator& r2)
	{
		return r1.mpCurrent == r2.mpCurrent;
	}


	template <typename T, typename Allocator>
	inline bool operator!=(const typename basic_string<T, Allocator>::reverse_iterator& r1,
						   const typename basic_string<T, Allocator>::reverse_iterator& r2)
	{
		return r1.mpCurrent != r2.mpCurrent;
	}


	// Operator +
	template <typename T, typename Allocator>
	basic_string<T, Allocator> operator+(const basic_string<T, Allocator>& a, const basic_string<T, Allocator>& b)
	{
		typedef typename basic_string<T, Allocator>::CtorDoNotInitialize CtorDoNotInitialize;
		CtorDoNotInitialize cDNI; // GCC 2.x forces us to declare a named temporary like this.
		basic_string<T, Allocator> result(cDNI, a.size() + b.size(), const_cast<basic_string<T, Allocator>&>(a).get_allocator()); // Note that we choose to assign a's allocator.
		result.append(a);
		result.append(b);
		return result;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator> operator+(const typename basic_string<T, Allocator>::value_type* p, const basic_string<T, Allocator>& b)
	{
		typedef typename basic_string<T, Allocator>::CtorDoNotInitialize CtorDoNotInitialize;
		CtorDoNotInitialize cDNI; // GCC 2.x forces us to declare a named temporary like this.
		const typename basic_string<T, Allocator>::size_type n = (typename basic_string<T, Allocator>::size_type)CharStrlen(p);
		basic_string<T, Allocator> result(cDNI, n + b.size(), const_cast<basic_string<T, Allocator>&>(b).get_allocator());
		result.append(p, p + n);
		result.append(b);
		return result;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator> operator+(typename basic_string<T, Allocator>::value_type c, const basic_string<T, Allocator>& b)
	{
		typedef typename basic_string<T, Allocator>::CtorDoNotInitialize CtorDoNotInitialize;
		CtorDoNotInitialize cDNI; // GCC 2.x forces us to declare a named temporary like this.
		basic_string<T, Allocator> result(cDNI, 1 + b.size(), const_cast<basic_string<T, Allocator>&>(b).get_allocator());
		result.push_back(c);
		result.append(b);
		return result;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator> operator+(const basic_string<T, Allocator>& a, const typename basic_string<T, Allocator>::value_type* p)
	{
		typedef typename basic_string<T, Allocator>::CtorDoNotInitialize CtorDoNotInitialize;
		CtorDoNotInitialize cDNI; // GCC 2.x forces us to declare a named temporary like this.
		const typename basic_string<T, Allocator>::size_type n = (typename basic_string<T, Allocator>::size_type)CharStrlen(p);
		basic_string<T, Allocator> result(cDNI, a.size() + n, const_cast<basic_string<T, Allocator>&>(a).get_allocator());
		result.append(a);
		result.append(p, p + n);
		return result;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator> operator+(const basic_string<T, Allocator>& a, typename basic_string<T, Allocator>::value_type c)
	{
		typedef typename basic_string<T, Allocator>::CtorDoNotInitialize CtorDoNotInitialize;
		CtorDoNotInitialize cDNI; // GCC 2.x forces us to declare a named temporary like this.
		basic_string<T, Allocator> result(cDNI, a.size() + 1, const_cast<basic_string<T, Allocator>&>(a).get_allocator());
		result.append(a);
		result.push_back(c);
		return result;
	}


	template <typename T, typename Allocator>
	basic_string<T, Allocator> operator+(basic_string<T, Allocator>&& a, basic_string<T, Allocator>&& b)
	{
		a.append(b); // Using an rvalue by name results in it becoming an lvalue.
		return eastl::move(a);
	}

	template <typename T, typename Allocator>
	basic_string<T, Allocator> operator+(basic_string<T, Allocator>&& a, const basic_string<T, Allocator>& b)
	{
		a.append(b);
		return eastl::move(a);
	}

	template <typename T, typename Allocator>
	basic_string<T, Allocator> operator+(const typename basic_string<T, Allocator>::value_type* p, basic_string<T, Allocator>&& b)
	{
		b.insert(0, p);
		return eastl::move(b);
	}

	template <typename T, typename Allocator>
	basic_string<T, Allocator> operator+(basic_string<T, Allocator>&& a, const typename basic_string<T, Allocator>::value_type* p)
	{
		a.append(p);
		return eastl::move(a);
	}

	template <typename T, typename Allocator>
	basic_string<T, Allocator> operator+(basic_string<T, Allocator>&& a, typename basic_string<T, Allocator>::value_type c)
	{
		a.push_back(c);
		return eastl::move(a);
	}


	template <typename T, typename Allocator>
	inline bool basic_string<T, Allocator>::validate() const EA_NOEXCEPT
	{
		if((internalLayout().BeginPtr() == nullptr) || (internalLayout().EndPtr() == nullptr))
			return false;
		if(internalLayout().EndPtr() < internalLayout().BeginPtr())
			return false;
		if(internalLayout().CapacityPtr() < internalLayout().EndPtr())
			return false;
		if(*internalLayout().EndPtr() != 0)
			return false;
		return true;
	}


	template <typename T, typename Allocator>
	inline int basic_string<T, Allocator>::validate_iterator(const_iterator i) const EA_NOEXCEPT
	{
		if(i >= internalLayout().BeginPtr())
		{
			if(i < internalLayout().EndPtr())
				return (isf_valid | isf_current | isf_can_dereference);

			if(i <= internalLayout().EndPtr())
				return (isf_valid | isf_current);
		}

		return isf_none;
	}


	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	// Operator== and operator!=
	template <typename T, typename Allocator>
	inline bool operator==(const basic_string<T, Allocator>& a, const basic_string<T, Allocator>& b)
	{
		return ((a.size() == b.size()) && (memcmp(a.data(), b.data(), (size_t)a.size() * sizeof(typename basic_string<T, Allocator>::value_type)) == 0));
	}


	template <typename T, typename Allocator>
	inline bool operator==(const typename basic_string<T, Allocator>::value_type* p, const basic_string<T, Allocator>& b)
	{
		typedef typename basic_string<T, Allocator>::size_type size_type;
		const size_type n = (size_type)CharStrlen(p);
		return ((n == b.size()) && (memcmp(p, b.data(), (size_t)n * sizeof(*p)) == 0));
	}


	template <typename T, typename Allocator>
	inline bool operator==(const basic_string<T, Allocator>& a, const typename basic_string<T, Allocator>::value_type* p)
	{
		typedef typename basic_string<T, Allocator>::size_type size_type;
		const size_type n = (size_type)CharStrlen(p);
		return ((a.size() == n) && (memcmp(a.data(), p, (size_t)n * sizeof(*p)) == 0));
	}


	template <typename T, typename Allocator>
	inline bool operator!=(const basic_string<T, Allocator>& a, const basic_string<T, Allocator>& b)
	{
		return !(a == b);
	}


	template <typename T, typename Allocator>
	inline bool operator!=(const typename basic_string<T, Allocator>::value_type* p, const basic_string<T, Allocator>& b)
	{
		return !(p == b);
	}


	template <typename T, typename Allocator>
	inline bool operator!=(const basic_string<T, Allocator>& a, const typename basic_string<T, Allocator>::value_type* p)
	{
		return !(a == p);
	}


	// Operator< (and also >, <=, and >=).
	template <typename T, typename Allocator>
	inline bool operator<(const basic_string<T, Allocator>& a, const basic_string<T, Allocator>& b)
	{
		return basic_string<T, Allocator>::compare(a.begin(), a.end(), b.begin(), b.end()) < 0; }


	template <typename T, typename Allocator>
	inline bool operator<(const typename basic_string<T, Allocator>::value_type* p, const basic_string<T, Allocator>& b)
	{
		typedef typename basic_string<T, Allocator>::size_type size_type;
		const size_type n = (size_type)CharStrlen(p);
		return basic_string<T, Allocator>::compare(p, p + n, b.begin(), b.end()) < 0;
	}


	template <typename T, typename Allocator>
	inline bool operator<(const basic_string<T, Allocator>& a, const typename basic_string<T, Allocator>::value_type* p)
	{
		typedef typename basic_string<T, Allocator>::size_type size_type;
		const size_type n = (size_type)CharStrlen(p);
		return basic_string<T, Allocator>::compare(a.begin(), a.end(), p, p + n) < 0;
	}


	template <typename T, typename Allocator>
	inline bool operator>(const basic_string<T, Allocator>& a, const basic_string<T, Allocator>& b)
	{
		return b < a;
	}


	template <typename T, typename Allocator>
	inline bool operator>(const typename basic_string<T, Allocator>::value_type* p, const basic_string<T, Allocator>& b)
	{
		return b < p;
	}


	template <typename T, typename Allocator>
	inline bool operator>(const basic_string<T, Allocator>& a, const typename basic_string<T, Allocator>::value_type* p)
	{
		return p < a;
	}


	template <typename T, typename Allocator>
	inline bool operator<=(const basic_string<T, Allocator>& a, const basic_string<T, Allocator>& b)
	{
		return !(b < a);
	}


	template <typename T, typename Allocator>
	inline bool operator<=(const typename basic_string<T, Allocator>::value_type* p, const basic_string<T, Allocator>& b)
	{
		return !(b < p);
	}


	template <typename T, typename Allocator>
	inline bool operator<=(const basic_string<T, Allocator>& a, const typename basic_string<T, Allocator>::value_type* p)
	{
		return !(p < a);
	}


	template <typename T, typename Allocator>
	inline bool operator>=(const basic_string<T, Allocator>& a, const basic_string<T, Allocator>& b)
	{
		return !(a < b);
	}


	template <typename T, typename Allocator>
	inline bool operator>=(const typename basic_string<T, Allocator>::value_type* p, const basic_string<T, Allocator>& b)
	{
		return !(p < b);
	}


	template <typename T, typename Allocator>
	inline bool operator>=(const basic_string<T, Allocator>& a, const typename basic_string<T, Allocator>::value_type* p)
	{
		return !(a < p);
	}


	template <typename T, typename Allocator>
	inline void swap(basic_string<T, Allocator>& a, basic_string<T, Allocator>& b)
	{
		a.swap(b);
	}


	/// string / wstring
	typedef basic_string<char>    string;
	typedef basic_string<wchar_t> wstring;

	/// custom string8 / string16 / string32
	typedef basic_string<char>     string8;
	typedef basic_string<char16_t> string16;
	typedef basic_string<char32_t> string32;

	/// ISO mandated string types
	typedef basic_string<char8_t>  u8string;    // Actually not a C++11 type, but added for consistency.
	typedef basic_string<char16_t> u16string;
	typedef basic_string<char32_t> u32string;


	/// hash<string>
	///
	/// We provide EASTL hash function objects for use in hash table containers.
	///
	/// Example usage:
	///    #include <EASTL/hash_set.h>
	///    hash_set<string> stringHashSet;
	///
	template <typename T> struct hash;

	template <>
	struct hash<string>
	{
		size_t operator()(const string& x) const
		{
			const unsigned char* p = (const unsigned char*)x.c_str(); // To consider: limit p to at most 256 chars.
			unsigned int c, result = 2166136261U; // We implement an FNV-like string hash.
			while((c = *p++) != 0) // Using '!=' disables compiler warnings.
				result = (result * 16777619) ^ c;
			return (size_t)result;
		}
	};

	#if defined(EA_CHAR8_UNIQUE) && EA_CHAR8_UNIQUE
		template <>
		struct hash<u8string>
		{
			size_t operator()(const u8string& x) const
			{
				const char8_t* p = (const char8_t*)x.c_str();
				unsigned int c, result = 2166136261U;
				while((c = *p++) != 0)
					result = (result * 16777619) ^ c;
				return (size_t)result;
			}
		};
	#endif

	template <>
	struct hash<string16>
	{
		size_t operator()(const string16& x) const
		{
			const char16_t* p = x.c_str();
			unsigned int c, result = 2166136261U;
			while((c = *p++) != 0)
				result = (result * 16777619) ^ c;
			return (size_t)result;
		}
	};

	template <>
	struct hash<string32>
	{
		size_t operator()(const string32& x) const
		{
			const char32_t* p = x.c_str();
			unsigned int c, result = 2166136261U;
			while((c = (unsigned int)*p++) != 0)
				result = (result * 16777619) ^ c;
			return (size_t)result;
		}
	};

	#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
		template <>
		struct hash<wstring>
		{
			size_t operator()(const wstring& x) const
			{
				const wchar_t* p = x.c_str();
				unsigned int c, result = 2166136261U;
				while((c = (unsigned int)*p++) != 0)
					result = (result * 16777619) ^ c;
				return (size_t)result;
			}
		};
	#endif


	/// to_string
	///
	/// Converts integral types to an eastl::string with the same content that sprintf produces.  The following
	/// implementation provides a type safe conversion mechanism which avoids the common bugs associated with sprintf
	/// style format strings.
	///
	/// http://en.cppreference.com/w/cpp/string/basic_string/to_string
	///
	inline string to_string(int value)
		{ return string(string::CtorSprintf(), "%d", value); }
	inline string to_string(long value)
		{ return string(string::CtorSprintf(), "%ld", value); }
	inline string to_string(long long value)
		{ return string(string::CtorSprintf(), "%lld", value); }
	inline string to_string(unsigned value)
		{ return string(string::CtorSprintf(), "%u", value); }
	inline string to_string(unsigned long value)
		{ return string(string::CtorSprintf(), "%lu", value); }
	inline string to_string(unsigned long long value)
		{ return string(string::CtorSprintf(), "%llu", value); }
	inline string to_string(float value)
		{ return string(string::CtorSprintf(), "%f", value); }
	inline string to_string(double value)
		{ return string(string::CtorSprintf(), "%f", value); }
	inline string to_string(long double value)
		{ return string(string::CtorSprintf(), "%Lf", value); }


	/// to_wstring
	///
	/// Converts integral types to an eastl::wstring with the same content that sprintf produces.  The following
	/// implementation provides a type safe conversion mechanism which avoids the common bugs associated with sprintf
	/// style format strings.
	///
	/// http://en.cppreference.com/w/cpp/string/basic_string/to_wstring
	///
	inline wstring to_wstring(int value)
		{ return wstring(wstring::CtorSprintf(), L"%d", value); }
	inline wstring to_wstring(long value)
		{ return wstring(wstring::CtorSprintf(), L"%ld", value); }
	inline wstring to_wstring(long long value)
		{ return wstring(wstring::CtorSprintf(), L"%lld", value); }
	inline wstring to_wstring(unsigned value)
		{ return wstring(wstring::CtorSprintf(), L"%u", value); }
	inline wstring to_wstring(unsigned long value)
		{ return wstring(wstring::CtorSprintf(), L"%lu", value); }
	inline wstring to_wstring(unsigned long long value)
		{ return wstring(wstring::CtorSprintf(), L"%llu", value); }
	inline wstring to_wstring(float value)
		{ return wstring(wstring::CtorSprintf(), L"%f", value); }
	inline wstring to_wstring(double value)
		{ return wstring(wstring::CtorSprintf(), L"%f", value); }
	inline wstring to_wstring(long double value)
		{ return wstring(wstring::CtorSprintf(), L"%Lf", value); }


	/// user defined literals
	///
	/// Converts a character array literal to a basic_string.
	///
	/// Example:
	///   string s = "abcdef"s;
	///
	/// http://en.cppreference.com/w/cpp/string/basic_string/operator%22%22s
	///
	#if EASTL_USER_LITERALS_ENABLED && EASTL_INLINE_NAMESPACES_ENABLED
		EA_DISABLE_VC_WARNING(4455) // disable warning C4455: literal suffix identifiers that do not start with an underscore are reserved
	    inline namespace literals
	    {
		    inline namespace string_literals
		    {
				inline string operator"" s(const char* str, size_t len) EA_NOEXCEPT { return {str, string::size_type(len)}; }
				inline u16string operator"" s(const char16_t* str, size_t len) EA_NOEXCEPT { return {str, u16string::size_type(len)}; }
				inline u32string operator"" s(const char32_t* str, size_t len) EA_NOEXCEPT { return {str, u32string::size_type(len)}; }
				inline wstring operator"" s(const wchar_t* str, size_t len) EA_NOEXCEPT { return {str, wstring::size_type(len)}; }

				// C++20 char8_t support.
				#if EA_CHAR8_UNIQUE
					inline u8string operator"" s(const char8_t* str, size_t len) EA_NOEXCEPT { return {str, u8string::size_type(len)}; }
				#endif
		    }
	    }
		EA_RESTORE_VC_WARNING()  // warning: 4455
	#endif


	/// erase / erase_if
	///
	/// https://en.cppreference.com/w/cpp/string/basic_string/erase2
	template <class CharT, class Allocator, class U>
	void erase(basic_string<CharT, Allocator>& c, const U& value)
	{
		// Erases all elements that compare equal to value from the container.
		c.erase(eastl::remove(c.begin(), c.end(), value), c.end());
	}

	template <class CharT, class Allocator, class Predicate>
	void erase_if(basic_string<CharT, Allocator>& c, Predicate predicate)
	{
		// Erases all elements that satisfy the predicate pred from the container.
		c.erase(eastl::remove_if(c.begin(), c.end(), predicate), c.end());
	}
} // namespace eastl


EA_RESTORE_VC_WARNING();


#endif // Header include guard
