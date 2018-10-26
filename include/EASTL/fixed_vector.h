///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements a vector which uses a fixed size memory pool. 
// The bEnableOverflow template parameter allows the container to resort to
// heap allocations if the memory pool is exhausted.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_FIXED_VECTOR_H
#define EASTL_FIXED_VECTOR_H


#include <EASTL/vector.h>
#include <EASTL/internal/fixed_pool.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{
	/// EASTL_FIXED_VECTOR_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	/// In the case of fixed-size containers, the allocator name always refers
	/// to overflow allocations. 
	///
	#ifndef EASTL_FIXED_VECTOR_DEFAULT_NAME
		#define EASTL_FIXED_VECTOR_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " fixed_vector" // Unless the user overrides something, this is "EASTL fixed_vector".
	#endif


	/// EASTL_FIXED_VECTOR_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_FIXED_VECTOR_DEFAULT_ALLOCATOR
		#define EASTL_FIXED_VECTOR_DEFAULT_ALLOCATOR overflow_allocator_type(EASTL_FIXED_VECTOR_DEFAULT_NAME)
	#endif


	/// fixed_vector
	///
	/// A fixed_vector with bEnableOverflow == true is identical to a regular 
	/// vector in terms of its behavior. All the expectations of regular vector
	/// apply to it and no additional expectations come from it. When bEnableOverflow
	/// is false, fixed_vector behaves like regular vector with the exception that 
	/// its capacity can never increase. All operations you do on such a fixed_vector
	/// which require a capacity increase will result in undefined behavior or an 
	/// C++ allocation exception, depending on the configuration of EASTL.
	///
	/// Template parameters:
	///     T                      The type of object the vector holds.
	///     nodeCount              The max number of objects to contain.
	///     bEnableOverflow        Whether or not we should use the overflow heap if our object pool is exhausted.
	///     OverflowAllocator              Overflow allocator, which is only used if bEnableOverflow == true. Defaults to the global heap.
	///
	/// Note: The nodeCount value must be at least 1.
	///
	/// Example usage:
	///    fixed_vector<Widget, 128, true> fixedVector);
	///
	///    fixedVector.push_back(Widget());
	///    fixedVector.resize(200);
	///    fixedVector.clear();
	///
	template <typename T, size_t nodeCount, bool bEnableOverflow = true, typename OverflowAllocator = typename eastl::type_select<bEnableOverflow, EASTLAllocatorType, EASTLDummyAllocatorType>::type>
	class fixed_vector : public vector<T, fixed_vector_allocator<sizeof(T), nodeCount, EASTL_ALIGN_OF(T), 0, bEnableOverflow, OverflowAllocator> >
	{
	public:
		typedef fixed_vector_allocator<sizeof(T), nodeCount, EASTL_ALIGN_OF(T), 
							0, bEnableOverflow, OverflowAllocator>              fixed_allocator_type;
		typedef OverflowAllocator                                               overflow_allocator_type;
		typedef vector<T, fixed_allocator_type>                                 base_type;
		typedef fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>  this_type;
		typedef typename base_type::size_type                                   size_type;
		typedef typename base_type::value_type                                  value_type;
		typedef typename base_type::reference                                   reference;
		typedef typename base_type::iterator                                    iterator;
		typedef typename base_type::const_iterator                              const_iterator;
		typedef aligned_buffer<nodeCount * sizeof(T), EASTL_ALIGN_OF(T)>        aligned_buffer_type;

		enum { kMaxSize = nodeCount };

		using base_type::get_allocator;
		using base_type::mpBegin;
		using base_type::mpEnd;
		using base_type::internalCapacityPtr;
		using base_type::resize;
		using base_type::clear;
		using base_type::size;
		using base_type::assign;
		using base_type::npos;
		using base_type::DoAllocate;
		using base_type::DoFree;
		using base_type::DoAssign;
		using base_type::DoAssignFromIterator;

	protected:
		aligned_buffer_type mBuffer;

	public:
		fixed_vector();
		explicit fixed_vector(const overflow_allocator_type& overflowAllocator); // Only applicable if bEnableOverflow is true.
		explicit fixed_vector(size_type n);                                      // Currently we don't support overflowAllocator specification for other constructors, for simplicity.
		fixed_vector(size_type n, const value_type& value);
		fixed_vector(const this_type& x);
		fixed_vector(this_type&& x);
		fixed_vector(this_type&& x, const overflow_allocator_type& overflowAllocator);
		fixed_vector(std::initializer_list<T> ilist, const overflow_allocator_type& overflowAllocator = EASTL_FIXED_VECTOR_DEFAULT_ALLOCATOR);

		template <typename InputIterator>
		fixed_vector(InputIterator first, InputIterator last);

		this_type& operator=(const this_type& x);
		this_type& operator=(std::initializer_list<T> ilist);
		this_type& operator=(this_type&& x);

		void swap(this_type& x);

		void      set_capacity(size_type n);
		void      clear(bool freeOverflow);
		void      reset_lose_memory();          // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.
		size_type max_size() const;             // Returns the max fixed size, which is the user-supplied nodeCount parameter.
		bool      full() const;                 // Returns true if the fixed space has been fully allocated. Note that if overflow is enabled, the container size can be greater than nodeCount but full() could return true because the fixed space may have a recently freed slot. 
		bool      has_overflowed() const;       // Returns true if the allocations spilled over into the overflow allocator. Meaningful only if overflow is enabled.
		bool      can_overflow() const;         // Returns the value of the bEnableOverflow template parameter.

		void*     push_back_uninitialized();
		void      push_back(const value_type& value);   // We implement push_back here because we have a specialization that's 
		reference push_back();                          // smaller for the case of overflow being disabled.
		void      push_back(value_type&& value);

		// OverflowAllocator
		const overflow_allocator_type& get_overflow_allocator() const EA_NOEXCEPT;
		overflow_allocator_type&       get_overflow_allocator() EA_NOEXCEPT;
		void                           set_overflow_allocator(const overflow_allocator_type& allocator);

	protected:
		void*     DoPushBackUninitialized(true_type);
		void*     DoPushBackUninitialized(false_type);

		void      DoPushBack(true_type, const value_type& value);
		void      DoPushBack(false_type, const value_type& value);

		void      DoPushBackMove(true_type, value_type&& value);
		void      DoPushBackMove(false_type, value_type&& value);

		reference DoPushBack(false_type);
		reference DoPushBack(true_type);

	}; // fixed_vector




	///////////////////////////////////////////////////////////////////////
	// fixed_vector
	///////////////////////////////////////////////////////////////////////

	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_vector()
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		#if EASTL_NAME_ENABLED
			get_allocator().set_name(EASTL_FIXED_VECTOR_DEFAULT_NAME);
		#endif

		mpBegin = mpEnd = (value_type*)&mBuffer.buffer[0];
		internalCapacityPtr() = mpBegin + nodeCount;
	}

	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_vector(const overflow_allocator_type& overflowAllocator)
		: base_type(fixed_allocator_type(mBuffer.buffer, overflowAllocator))
	{
		#if EASTL_NAME_ENABLED
			get_allocator().set_name(EASTL_FIXED_VECTOR_DEFAULT_NAME);
		#endif

		mpBegin = mpEnd = (value_type*)&mBuffer.buffer[0];
		internalCapacityPtr() = mpBegin + nodeCount;
	}

	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_vector(size_type n)
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		#if EASTL_NAME_ENABLED
			get_allocator().set_name(EASTL_FIXED_VECTOR_DEFAULT_NAME);
		#endif

		mpBegin = mpEnd = (value_type*)&mBuffer.buffer[0];
		internalCapacityPtr() = mpBegin + nodeCount;
		resize(n);
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_vector(size_type n, const value_type& value)
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		#if EASTL_NAME_ENABLED
			get_allocator().set_name(EASTL_FIXED_VECTOR_DEFAULT_NAME);
		#endif

		mpBegin = mpEnd = (value_type*)&mBuffer.buffer[0];
		internalCapacityPtr() = mpBegin + nodeCount;
		resize(n, value);
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_vector(const this_type& x)
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		get_allocator().copy_overflow_allocator(x.get_allocator());

		#if EASTL_NAME_ENABLED
			get_allocator().set_name(x.get_allocator().get_name());
		#endif

		mpBegin = mpEnd = (value_type*)&mBuffer.buffer[0];
		internalCapacityPtr() = mpBegin + nodeCount;
		base_type::template DoAssign<const_iterator, false>(x.begin(), x.end(), false_type());
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_vector(this_type&& x)
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		// Since we are a fixed_vector, we can't swap pointers. We can possibly so something like fixed_swap or
		// we can just do an assignment from x. If we want to do the former then we need to have some complicated
		// code to deal with overflow or no overflow, and whether the memory is in the fixed-size buffer or in 
		// the overflow allocator. 90% of the time the memory should be in the fixed buffer, in which case
		// a simple assignment is no worse than the fancy pathway.

		// Since we are a fixed_list, we can't normally swap pointers unless both this and 
		// x are using using overflow and the overflow allocators are equal. To do:
		//if(has_overflowed() && x.has_overflowed() && (get_overflow_allocator() == x.get_overflow_allocator()))
		//{
		//    We can swap contents and may need to swap the allocators as well.
		//}

		// The following is currently identical to the fixed_vector(const this_type& x) code above. If it stays that
		// way then we may want to make a shared implementation.
		get_allocator().copy_overflow_allocator(x.get_allocator());

		#if EASTL_NAME_ENABLED
			get_allocator().set_name(x.get_allocator().get_name());
		#endif

		mpBegin = mpEnd = (value_type*)&mBuffer.buffer[0];
		internalCapacityPtr() = mpBegin + nodeCount;
		base_type::template DoAssign<move_iterator<iterator>, true>(eastl::make_move_iterator(x.begin()), eastl::make_move_iterator(x.end()), false_type());
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_vector(this_type&& x, const overflow_allocator_type& overflowAllocator)
		: base_type(fixed_allocator_type(mBuffer.buffer, overflowAllocator))
	{
		// See the discussion above.

		// The following is currently identical to the fixed_vector(const this_type& x) code above. If it stays that
		// way then we may want to make a shared implementation.
		get_allocator().copy_overflow_allocator(x.get_allocator());

		#if EASTL_NAME_ENABLED
			get_allocator().set_name(x.get_allocator().get_name());
		#endif

		mpBegin = mpEnd = (value_type*)&mBuffer.buffer[0];
		internalCapacityPtr() = mpBegin + nodeCount;
		base_type::template DoAssign<iterator, true>(x.begin(), x.end(), false_type());
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_vector(std::initializer_list<T> ilist, const overflow_allocator_type& overflowAllocator)
		: base_type(fixed_allocator_type(mBuffer.buffer, overflowAllocator))
	{
		typedef typename std::initializer_list<value_type>::iterator InputIterator;
		typedef typename eastl::iterator_traits<InputIterator>::iterator_category IC;

		mpBegin = mpEnd = (value_type*)&mBuffer.buffer[0];
		internalCapacityPtr() = mpBegin + nodeCount;
		base_type::template DoAssignFromIterator<InputIterator, false>(ilist.begin(), ilist.end(), IC());
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	template <typename InputIterator>
	fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_vector(InputIterator first, InputIterator last)
		: base_type(fixed_allocator_type(mBuffer.buffer))
	{
		#if EASTL_NAME_ENABLED
			get_allocator().set_name(EASTL_FIXED_VECTOR_DEFAULT_NAME);
		#endif

		mpBegin = mpEnd = (value_type*)&mBuffer.buffer[0];
		internalCapacityPtr() = mpBegin + nodeCount;
		base_type::template DoAssign<InputIterator, false>(first, last, is_integral<InputIterator>());
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::this_type& 
	fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::operator=(const this_type& x)
	{
		if(this != &x)
		{
			clear();

			#if EASTL_ALLOCATOR_COPY_ENABLED
				get_allocator() = x.get_allocator(); // The primary effect of this is to copy the overflow allocator.
			#endif

			base_type::template DoAssign<const_iterator, false>(x.begin(), x.end(), false_type()); // Shorter route.
		}
		return *this;
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::this_type& 
	fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::operator=(std::initializer_list<T> ilist)
	{
		typedef typename std::initializer_list<value_type>::iterator InputIterator;
		typedef typename eastl::iterator_traits<InputIterator>::iterator_category IC;

		clear();
		base_type::template DoAssignFromIterator<InputIterator, false>(ilist.begin(), ilist.end(), IC());
		return *this;
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::this_type& 
	fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::operator=(this_type&& x)
	{
		// Since we are a fixed_vector, we can't swap pointers. We can possibly do something like fixed_swap or
		// we can just do an assignment from x. If we want to do the former then we need to have some complicated
		// code to deal with overflow or no overflow, and whether the memory is in the fixed-size buffer or in 
		// the overflow allocator. 90% of the time the memory should be in the fixed buffer, in which case
		// a simple assignment is no worse than the fancy pathway.
		if (this != &x)
		{
			clear();

			#if EASTL_ALLOCATOR_COPY_ENABLED
				get_allocator() = x.get_allocator(); // The primary effect of this is to copy the overflow allocator.
			#endif

			base_type::template DoAssign<move_iterator<iterator>, true>(make_move_iterator(x.begin()), make_move_iterator(x.end()), false_type()); // Shorter route.
		}
		return *this;
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::swap(this_type& x)
	{
		if((has_overflowed() && x.has_overflowed()) && (get_overflow_allocator() == x.get_overflow_allocator())) // If both containers are using the heap instead of local memory 
		{                                                                                                        // then we can do a fast pointer swap instead of content swap.
			eastl::swap(mpBegin,    x.mpBegin);
			eastl::swap(mpEnd,      x.mpEnd);
			eastl::swap(internalCapacityPtr(), x.internalCapacityPtr());
		}
		else
		{
			// Fixed containers use a special swap that can deal with excessively large buffers.
			eastl::fixed_swap(*this, x);
		}
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::set_capacity(size_type n)
	{
		const size_type nPrevSize     = (size_type)(mpEnd - mpBegin);
		const size_type nPrevCapacity = (size_type)(internalCapacityPtr() - mpBegin);

		if(n == npos)       // If the user means to set the capacity so that it equals the size (i.e. free excess capacity)...
			n = nPrevSize;

		if(n != nPrevCapacity)  // If the request results in a capacity change...
		{
			if(can_overflow() && (((uintptr_t)mpBegin != (uintptr_t)mBuffer.buffer) || (n > kMaxSize))) // If we are or would be using dynamically allocated memory instead of our fixed-size member buffer...
			{
				T* const pNewData = (n <= kMaxSize) ? (T*)&mBuffer.buffer[0] : DoAllocate(n);
				T* const pCopyEnd = (n < nPrevSize) ? (mpBegin + n) : mpEnd;
				eastl::uninitialized_move_ptr(mpBegin, pCopyEnd, pNewData); // Move [mpBegin, pCopyEnd) to p.
				eastl::destruct(mpBegin, mpEnd);
				if((uintptr_t)mpBegin != (uintptr_t)mBuffer.buffer)
					DoFree(mpBegin, (size_type)(internalCapacityPtr() - mpBegin));

				mpEnd      = pNewData + (pCopyEnd - mpBegin);
				mpBegin    = pNewData;
				internalCapacityPtr() = mpBegin + n;
			} // Else the new capacity would be within our fixed buffer.
			else if(n < nPrevSize) // If the newly requested capacity is less than our size, we do what vector::set_capacity does and resize, even though we actually aren't reducing the capacity.
				resize(n);
		}
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename Allocator>
	inline void fixed_vector<T, nodeCount, bEnableOverflow, Allocator>::clear(bool freeOverflow)
	{
		base_type::clear();
		if (freeOverflow && mpBegin != (value_type*)&mBuffer.buffer[0])
		{
			EASTLFree(get_allocator(), mpBegin, (internalCapacityPtr() - mpBegin) * sizeof(T));
			mpBegin = mpEnd = (value_type*)&mBuffer.buffer[0];
			internalCapacityPtr() = mpBegin + nodeCount;
		}
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::reset_lose_memory()
	{
		mpBegin = mpEnd = (value_type*)&mBuffer.buffer[0];
		internalCapacityPtr() = mpBegin + nodeCount;
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::size_type
	fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::max_size() const
	{
		return kMaxSize;
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline bool fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::full() const
	{
		// If size >= capacity, then we are definitely full. 
		// Also, if our size is smaller but we've switched away from mBuffer due to a previous overflow, then we are considered full.
		return ((size_t)(mpEnd - mpBegin) >= kMaxSize) || ((void*)mpBegin != (void*)mBuffer.buffer);
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline bool fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::has_overflowed() const
	{
		// This will be incorrect for the case that bOverflowEnabled is true and the container was resized
		// down to a small size where the fixed buffer could take over ownership of the data again.
		// The only simple fix for this is to take on another member variable which tracks whether this overflow
		// has occurred at some point in the past.
		return ((void*)mpBegin != (void*)mBuffer.buffer);
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline bool fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::can_overflow() const
	{
		return bEnableOverflow;
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void* fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::push_back_uninitialized()
	{
		return DoPushBackUninitialized(typename type_select<bEnableOverflow, true_type, false_type>::type());
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void* fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::DoPushBackUninitialized(true_type)
	{
		return base_type::push_back_uninitialized();
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void* fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::DoPushBackUninitialized(false_type)
	{
		EASTL_ASSERT(mpEnd < internalCapacityPtr());

		return mpEnd++;
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::push_back(const value_type& value)
	{
		DoPushBack(typename type_select<bEnableOverflow, true_type, false_type>::type(), value);
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::DoPushBack(true_type, const value_type& value)
	{
		base_type::push_back(value);
	}


	// This template specializes for overflow NOT enabled.
	// In this configuration, there is no need for the heavy weight push_back() which tests to see if the container should grow (it never will)
	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::DoPushBack(false_type, const value_type& value)
	{
		EASTL_ASSERT(mpEnd < internalCapacityPtr());

		::new((void*)mpEnd++) value_type(value);
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::reference fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::push_back()
	{
		return DoPushBack(typename type_select<bEnableOverflow, true_type, false_type>::type());
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::reference fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::DoPushBack(true_type)
	{
		return base_type::push_back();
	}


	// This template specializes for overflow NOT enabled.
	// In this configuration, there is no need for the heavy weight push_back() which tests to see if the container should grow (it never will)
	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::reference fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::DoPushBack(false_type)
	{
		EASTL_ASSERT(mpEnd < internalCapacityPtr());

		::new((void*)mpEnd++) value_type;    // Note that this isn't value_type() as that syntax doesn't work on all compilers for POD types.

		return *(mpEnd - 1);        // Same as return back();
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::push_back(value_type&& value)
	{
		DoPushBackMove(typename type_select<bEnableOverflow, true_type, false_type>::type(), eastl::move(value));
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::DoPushBackMove(true_type, value_type&& value)
	{
		base_type::push_back(eastl::move(value)); // This will call vector::push_back(value_type &&), and possibly swap value with *mpEnd.
	}


	// This template specializes for overflow NOT enabled.
	// In this configuration, there is no need for the heavy weight push_back() which tests to see if the container should grow (it never will)
	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::DoPushBackMove(false_type, value_type&& value)
	{
		EASTL_ASSERT(mpEnd < internalCapacityPtr());

		::new((void*)mpEnd++) value_type(eastl::move(value)); // This will call the value_type(value_type&&) constructor, and possibly swap value with *mpEnd.
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline const typename fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::overflow_allocator_type& 
	fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::get_overflow_allocator() const EA_NOEXCEPT
	{
		return get_allocator().get_overflow_allocator();
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::overflow_allocator_type& 
	fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::get_overflow_allocator() EA_NOEXCEPT
	{
		return get_allocator().get_overflow_allocator();
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void 
	fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>::set_overflow_allocator(const overflow_allocator_type& allocator)
	{
		get_allocator().set_overflow_allocator(allocator);
	}



	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	// operator ==, !=, <, >, <=, >= come from the vector implementations.

	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void swap(fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>& a, 
					 fixed_vector<T, nodeCount, bEnableOverflow, OverflowAllocator>& b)
	{
		// Fixed containers use a special swap that can deal with excessively large buffers.
		eastl::fixed_swap(a, b);
	}



} // namespace eastl



#endif // Header include guard












