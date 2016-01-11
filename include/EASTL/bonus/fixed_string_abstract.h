/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements a string which uses a fixed size memory pool.
// The bEnableOverflow template parameter allows the container to resort to
// heap allocations if the memory pool is exhausted.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_FIXED_STRING_ABSTRACT_H
#define EASTL_FIXED_STRING_ABSTRACT_H


#include <EASTL/internal/config.h>
#include <EASTL/string.h>
#include <EASTL/internal/fixed_pool.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{
	/// EASTL_FIXED_STRING_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	/// In the case of fixed-size containers, the allocator name always refers
	/// to overflow allocations. 
	///
	#ifndef EASTL_FIXED_STRING_DEFAULT_NAME
		#define EASTL_FIXED_STRING_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " fixed_string" // Unless the user overrides something, this is "EASTL fixed_string".
	#endif



	/// fixed_string
	///
	/// A fixed_string with bEnableOverflow == true is identical to a regular
	/// string in terms of its behavior. All the expectations of regular string
	/// apply to it and no additional expectations come from it. When bEnableOverflow
	/// is false, fixed_string behaves like regular string with the exception that
	/// its capacity can never increase. All operations you do on such a fixed_string
	/// which require a capacity increase will result in undefined behavior or an
	/// C++ allocation exception, depending on the configuration of EASTL.
	///
	/// Note: The nodeCount value is the amount of characters to allocate, which needs to
	/// take into account a terminating zero. Thus if you want to store strings with a strlen
	/// of 30, the nodeCount value must be at least 31.
	///
	/// Template parameters:
	///     T                      The type of object the string holds (char, wchar_t, char8_t, char16_t, char32_t).
	///     nodeCount              The max number of objects to contain.
	///     bEnableOverflow        Whether or not we should use the overflow heap if our object pool is exhausted.
	///     OverflowAllocator              Overflow allocator, which is only used if bEnableOverflow == true. Defaults to the global heap.
	///
	/// Notes:
	///     The nodeCount value must be at least 2, one for a character and one for a terminating 0.
	///
	///     As of this writing, the string class necessarily reallocates when an insert of
	///     self is done into self. As a result, the fixed_string class doesn't support
	///     inserting self into self unless the bEnableOverflow template parameter is true.
	///
	/// Example usage:
	///    fixed_string<char, 128 + 1, true> fixedString("hello world"); // Can hold up to a strlen of 128.
	///
	///    fixedString = "hola mundo";
	///    fixedString.clear();
	///    fixedString.resize(200);
	///    fixedString.sprintf("%f", 1.5f);
	///
	template <typename T, int nodeCount, bool bEnableOverflow = true, typename OverflowAllocator = EASTLAllocatorType>
	class fixed_string : public basic_string<T, fixed_vector_allocator<sizeof(T), nodeCount, EASTL_ALIGN_OF(T), 0, bEnableOverflow, OverflowAllocator> >
	{
	public:
		typedef fixed_vector_allocator<sizeof(T), nodeCount, EASTL_ALIGN_OF(T),
							0, bEnableOverflow, OverflowAllocator>              fixed_allocator_type;
		typedef typename fixed_allocator_type::overflow_allocator_type          overflow_allocator_type;
		typedef abstract_string<T>                                              abstract_type;
		typedef basic_string<T, fixed_allocator_type>                           base_type;
		typedef fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>  this_type;
		typedef typename base_type::size_type                                   size_type;
		typedef typename base_type::value_type                                  value_type;
		typedef typename base_type::CtorDoNotInitialize                         CtorDoNotInitialize;
		typedef typename base_type::CtorSprintf                                                     CtorSprintf;
		typedef aligned_buffer<nodeCount * sizeof(T), EASTL_ALIGN_OF(T)>                            aligned_buffer_type;

		enum { kMaxSize = nodeCount - 1 }; // -1 because we need to save one element for the silent terminating null.

		using base_type::npos;
		using base_type::mpBegin;
		using base_type::mpEnd;
		using base_type::mpCapacity;
		using base_type::mAllocator;
		using base_type::append;
		using base_type::resize;
		using base_type::clear;
		using base_type::size;
		using base_type::sprintf_va_list;
		using base_type::mpAllocFreeMethod;
		using base_type::DoAllocate;
		using base_type::DoFree;

	protected:
		union // We define a union in order to avoid strict pointer aliasing issues with compilers like GCC.
		{
			value_type          mArray[1];
			aligned_buffer_type mBuffer;     // Question: Why are we doing this aligned_buffer thing? Why not just do an array of value_type, given that we are using just strings of char types.
		};

	public:
		fixed_string();
		explicit fixed_string(const overflow_allocator_type& overflowAllocator);            // Only applicable if bEnableOverflow is true.
		fixed_string(const this_type& x);                                                   // Currently we don't support overflowAllocator specification for other constructors, for simplicity.
		fixed_string(const base_type& x);
		fixed_string(const abstract_type& x);
		fixed_string(const base_type& x, size_type position, size_type n = base_type::npos);
		fixed_string(const value_type* p, size_type n);
		fixed_string(const value_type* p);
		fixed_string(size_type n, const value_type& value);
		fixed_string(const value_type* pBegin, const value_type* pEnd);
		fixed_string(CtorDoNotInitialize, size_type n);
		fixed_string(CtorSprintf, const value_type* pFormat, ...);

		this_type& operator=(const this_type& x);
		this_type& operator=(const base_type& x);
		this_type& operator=(const abstract_type& x);
		this_type& operator=(const value_type* p);
		this_type& operator=(const value_type c);

		void swap(this_type& x);

		void      set_capacity(size_type n);
		void      reset_lose_memory();                       // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.
		size_type max_size() const;
		bool      full() const;                 // Returns true if the fixed space has been fully allocated. Note that if overflow is enabled, the container size can be greater than nodeCount but full() could return true because the fixed space may have a recently freed slot. 
		bool      has_overflowed() const;       // Returns true if the allocations spilled over into the overflow allocator. Meaningful only if overflow is enabled.
		bool      can_overflow() const;         // Returns the value of the bEnableOverflow template parameter.

		// The inherited versions of substr/left/right call the basic_string constructor,
		// which will call the overflow allocator and fail if bEnableOverflow == false
		fixed_string substr(size_type position, size_type n) const;
		fixed_string left(size_type n) const;
		fixed_string right(size_type n) const;

		// OverflowAllocator
		const overflow_allocator_type& get_overflow_allocator() const EA_NOEXCEPT;
		overflow_allocator_type&       get_overflow_allocator() EA_NOEXCEPT;
		void                           set_overflow_allocator(const overflow_allocator_type& allocator);

		static void* AllocFreeMethod(size_t n, void* pBuffer, void* pContext);

		#if EASTL_RESET_ENABLED
			void reset(); // This function name is deprecated; use reset_lose_memory instead.
		#endif

	}; // fixed_string





	///////////////////////////////////////////////////////////////////////
	// fixed_string
	///////////////////////////////////////////////////////////////////////

	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_string()
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_STRING_DEFAULT_NAME);
		#endif

		mpAllocFreeMethod = &fixed_string::AllocFreeMethod;
		mpBegin = mpEnd = mArray;
		mpCapacity = mpBegin + nodeCount;
	   *mpBegin = 0;
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_string(const overflow_allocator_type& overflowAllocator)
		: base_type(fixed_allocator_type(mBuffer.buffer, overflowAllocator))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_STRING_DEFAULT_NAME);
		#endif

		mpAllocFreeMethod = &fixed_string::AllocFreeMethod;
		mpBegin = mpEnd = mArray;
		mpCapacity = mpBegin + nodeCount;
	   *mpBegin = 0;
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_string(const this_type& x)
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		mAllocator.copy_overflow_allocator(x.mAllocator);

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(x.mAllocator.get_name());
		#endif

		mpAllocFreeMethod = &fixed_string::AllocFreeMethod;
		mpBegin = mpEnd = mArray;
		mpCapacity = mpBegin + nodeCount;
	   *mpBegin = 0;

		append(x);
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_string(const base_type& x)
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(x.get_allocator().get_name());
		#endif

		mpAllocFreeMethod = &fixed_string::AllocFreeMethod;
		mpBegin = mpEnd = mArray;
		mpCapacity = mpBegin + nodeCount;
	   *mpBegin = 0;

		append(x);
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_string(const abstract_type& x)
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		#if EASTL_NAME_ENABLED // abstract_string doesn't have a predefined allocator type we can copy from.
			mAllocator.set_name(EASTL_FIXED_STRING_DEFAULT_NAME);
		#endif

		mpAllocFreeMethod = &fixed_string::AllocFreeMethod;
		mpBegin = mpEnd = mArray;
		mpCapacity = mpBegin + nodeCount;
	   *mpBegin = 0;

		append(x.data(), x.length());
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_string(const base_type& x, size_type position, size_type n)
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(x.get_allocator().get_name());
		#endif

		mpAllocFreeMethod = &fixed_string::AllocFreeMethod;
		mpBegin = mpEnd = mArray;
		mpCapacity = mpBegin + nodeCount;
	   *mpBegin = 0;

		append(x, position, n);
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_string(const value_type* p, size_type n)
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_STRING_DEFAULT_NAME);
		#endif

		mpAllocFreeMethod = &fixed_string::AllocFreeMethod;
		mpBegin = mpEnd = mArray;
		mpCapacity = mpBegin + nodeCount;
	   *mpBegin = 0;

		append(p, n);
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_string(const value_type* p)
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_STRING_DEFAULT_NAME);
		#endif

		mpAllocFreeMethod = &fixed_string::AllocFreeMethod;
		mpBegin = mpEnd = mArray;
		mpCapacity = mpBegin + nodeCount;
	   *mpBegin = 0;

		append(p); // There better be enough space to hold the assigned string.
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_string(size_type n, const value_type& value)
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_STRING_DEFAULT_NAME);
		#endif

		mpAllocFreeMethod = &fixed_string::AllocFreeMethod;
		mpBegin = mpEnd = mArray;
		mpCapacity = mpBegin + nodeCount;
	   *mpBegin = 0;

		append(n, value); // There better be enough space to hold the assigned string.
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_string(const value_type* pBegin, const value_type* pEnd)
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_STRING_DEFAULT_NAME);
		#endif

		mpAllocFreeMethod = &fixed_string::AllocFreeMethod;
		mpBegin = mpEnd = mArray;
		mpCapacity = mpBegin + nodeCount;
	   *mpBegin = 0;

		append(pBegin, pEnd);
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_string(CtorDoNotInitialize, size_type n)
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_STRING_DEFAULT_NAME);
		#endif

		mpAllocFreeMethod = &fixed_string::AllocFreeMethod;
		mpBegin = mArray;
		mpCapacity = mpBegin + nodeCount;

		if((mpBegin + n) < mpCapacity)
		{
			mpEnd = mpBegin + n;
		   *mpEnd = 0;
		}
		else
		{
			mpEnd = mArray;
		   *mpEnd = 0;
			resize(n);
		}
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_string(CtorSprintf, const value_type* pFormat, ...)
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_STRING_DEFAULT_NAME);
		#endif

		mpAllocFreeMethod = &fixed_string::AllocFreeMethod;
		mpBegin = mpEnd = mArray;
		mpCapacity = mpBegin + nodeCount;
	   *mpBegin = 0;

		va_list arguments;
		va_start(arguments, pFormat);
		sprintf_va_list(pFormat, arguments);
		va_end(arguments);
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::this_type&
	fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::operator=(const this_type& x)
	{
		if(this != &x)
		{
			clear();
			append(x);
		}
		return *this;
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::
	this_type& fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::operator=(const base_type& x)
	{
		if(static_cast<base_type*>(this) != &x)
		{
			clear();
			append(x);
		}
		return *this;
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::
	this_type& fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::operator=(const abstract_type& x)
	{
		if(static_cast<abstract_type*>(this) != &x)
		{
			clear();
			append(x.data(), x.length());
		}
		return *this;
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::
	this_type& fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::operator=(const value_type* p)
	{
		if(mpBegin != p)
		{
			clear();
			append(p);
		}
		return *this;
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::
	this_type& fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::operator=(const value_type c)
	{
		clear();
		append((size_type)1, c);
		return *this;
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::swap(this_type& x)
	{
		// Fixed containers use a special swap that can deal with excessively large buffers.
		eastl::fixed_swap(*this, x);
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::set_capacity(size_type n)
	{
		const size_type nPrevSize     = (size_type)(mpEnd - mpBegin);
		const size_type nPrevCapacity = (size_type)((mpCapacity - mpBegin) - 1); // -1 because the terminating 0 isn't included in the calculated capacity value.

		if(n == npos)       // If the user means to set the capacity so that it equals the size (i.e. free excess capacity)...
			n = nPrevSize;

		if(n != nPrevCapacity)  // If the request results in a capacity change...
		{
			const size_type allocSize = (n + 1); // +1 because the terminating 0 isn't included in the supplied capacity value. So now n refers the amount of memory we need.

			if(can_overflow() && (((uintptr_t)mpBegin != (uintptr_t)mBuffer.buffer) || (allocSize > kMaxSize))) // If we are or would be using dynamically allocated memory instead of our fixed-size member buffer...
			{
				T* const pNewData = (allocSize <= kMaxSize) ? (T*)&mBuffer.buffer[0] : DoAllocate(allocSize);
				T* const pCopyEnd = (n < nPrevSize) ? (mpBegin + n) : mpEnd;
				CharStringUninitializedCopy(mpBegin, pCopyEnd, pNewData);  // Copy [mpBegin, pCopyEnd) to pNewData.
				if((uintptr_t)mpBegin != (uintptr_t)mBuffer.buffer)
					DoFree(mpBegin, (size_type)(mpCapacity - mpBegin));

				mpEnd      = pNewData + (pCopyEnd - mpBegin);
				mpBegin    = pNewData;
				mpCapacity = mpBegin + allocSize;
			} // Else the new capacity would be within our fixed buffer.
			else if(n < nPrevSize) // If the newly requested capacity is less than our size, we do what vector::set_capacity does and resize, even though we actually aren't reducing the capacity.
				resize(n);
		}
	}


	#if EASTL_RESET_ENABLED
		// This function name is deprecated; use reset_lose_memory instead.
		template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
		inline void fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::reset()
		{
			reset_lose_memory();
		}
	#endif


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::reset_lose_memory()
	{
		mpBegin = mpEnd = mArray;
		mpCapacity = mpBegin + nodeCount;
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::
	size_type fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::max_size() const
	{
		return kMaxSize;
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline bool fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::full() const
	{
		// If size >= capacity, then we are definitely full. 
		// Also, if our size is smaller but we've switched away from mBuffer due to a previous overflow, then we are considered full.
		return ((size_t)(mpEnd - mpBegin) >= kMaxSize) || ((void*)mpBegin != (void*)mBuffer.buffer);
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline bool fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::has_overflowed() const
	{
		// This will be incorrect for the case that bOverflowEnabled is true and the container was resized
		// down to a small size where the fixed buffer could take over ownership of the data again.
		// The only simple fix for this is to take on another member variable which tracks whether this overflow
		// has occurred at some point in the past.
		return ((void*)mpBegin != (void*)mBuffer.buffer);
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline bool fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::can_overflow() const
	{
		return bEnableOverflow;
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::
	this_type fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::substr(size_type position, size_type n) const
	{
		#if EASTL_STRING_OPT_RANGE_ERRORS
			if(position > (size_type)(mpEnd - mpBegin))
				ThrowRangeException();
		#endif

		return fixed_string(mpBegin + position, mpBegin + position + eastl::min_alt(n, (size_type)(mpEnd - mpBegin) - position));
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::
	this_type fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::left(size_type n) const
	{
		const size_type nLength = size();
		if(n < nLength)
			return fixed_string(mpBegin, mpBegin + n);
		return *this;
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::
	this_type fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::right(size_type n) const
	{
		const size_type nLength = size();
		if(n < nLength)
			return fixed_string(mpEnd - n, mpEnd);
		return *this;
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void* fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::AllocFreeMethod(size_t n, void* pBuffer, void* pContext)
	{
		// Use the basic_string method to reuse the same allocator code
		// In reality, we should have a specific implementation
		// We could also handle the overflow code at this level (instead of doing it at the allocator level)
		return base_type::AllocFreeMethod(n, pBuffer, pContext);
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline const typename fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::
	overflow_allocator_type& fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::get_overflow_allocator() const EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::
	overflow_allocator_type& fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::get_overflow_allocator() EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void 
	fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>::set_overflow_allocator(const overflow_allocator_type& allocator)
	{
		mAllocator.set_overflow_allocator(allocator);
	}


	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	// operator ==, !=, <, >, <=, >= come from the string implementations.

	template <typename T, int nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void swap(fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>& a,
					 fixed_string<T, nodeCount, bEnableOverflow, OverflowAllocator>& b)
	{
		// Fixed containers use a special swap that can deal with excessively large buffers.
		eastl::fixed_swap(a, b);
	}


} // namespace eastl



#endif // Header include guard












