/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements a list which uses a fixed size memory pool for its nodes. 
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_FIXED_LIST_H
#define EASTL_FIXED_LIST_H


#include <EASTL/list.h>
#include <EASTL/internal/fixed_pool.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{
	/// EASTL_FIXED_LIST_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	/// In the case of fixed-size containers, the allocator name always refers
	/// to overflow allocations. 
	///
	#ifndef EASTL_FIXED_LIST_DEFAULT_NAME
		#define EASTL_FIXED_LIST_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " fixed_list" // Unless the user overrides something, this is "EASTL fixed_list".
	#endif


	/// EASTL_FIXED_LIST_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_FIXED_LIST_DEFAULT_ALLOCATOR
		#define EASTL_FIXED_LIST_DEFAULT_ALLOCATOR overflow_allocator_type(EASTL_FIXED_LIST_DEFAULT_NAME)
	#endif



	/// fixed_list
	///
	/// fixed_list is a list which uses a single block of contiguous memory 
	/// for its nodes. The purpose of this is to reduce memory usage relative
	/// to a conventional memory allocation system (with block headers), to 
	/// increase allocation speed (often due to avoidance of mutex locks),
	/// to increase performance (due to better memory locality), and to decrease
	/// memory fragmentation due to the way that fixed block allocators work.
	///
	/// The primary downside to a fixed_list is that the number of nodes it
	/// can contain is fixed upon its declaration. If you want a fixed_list
	/// that doesn't have this limitation, then you probably don't want a
	/// fixed_list. You can always create your own memory allocator that works
	/// the way you want.
	///
	/// Template parameters:
	///     T                      The type of object the list holds.
	///     nodeCount              The max number of objects to contain.
	///     bEnableOverflow        Whether or not we should use the overflow heap if our object pool is exhausted.
	///     OverflowAllocator      Overflow allocator, which is only used if bEnableOverflow == true. Defaults to the global heap.
	///
	template <typename T, size_t nodeCount, bool bEnableOverflow = true, typename OverflowAllocator = EASTLAllocatorType>
	class fixed_list : public list<T, fixed_node_allocator<sizeof(typename list<T>::node_type), 
								   nodeCount, EASTL_ALIGN_OF(T), 0, bEnableOverflow, OverflowAllocator> >
	{
	public:
		typedef fixed_node_allocator<sizeof(typename list<T>::node_type), nodeCount, 
					 EASTL_ALIGN_OF(T), 0, bEnableOverflow, OverflowAllocator> fixed_allocator_type;
		typedef OverflowAllocator                                              overflow_allocator_type;
		typedef list<T, fixed_allocator_type>                                  base_type;
		typedef fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>   this_type;
		typedef typename base_type::size_type                                  size_type;
		typedef typename base_type::value_type                                 value_type;
		typedef typename base_type::node_type                                  node_type;
		typedef typename base_type::iterator                                   iterator;

		enum { kMaxSize = nodeCount };

		using base_type::assign;
		using base_type::resize;
		using base_type::insert;
		using base_type::size;
		using base_type::get_allocator;

	protected:
		char mBuffer[fixed_allocator_type::kBufferSize]; // kBufferSize will take into account alignment requirements.

		using base_type::mAllocator;

	public:
		fixed_list();
		explicit fixed_list(const overflow_allocator_type& overflowAllocator); // Only applicable if bEnableOverflow is true.
		explicit fixed_list(size_type n);                                      // Currently we don't support overflowAllocator specification for other constructors, for simplicity.
		fixed_list(size_type n, const value_type& value);
		fixed_list(const this_type& x);
		#if EASTL_MOVE_SEMANTICS_ENABLED
			fixed_list(this_type&& x);
			fixed_list(this_type&&, const overflow_allocator_type& overflowAllocator);
		#endif
		fixed_list(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator = EASTL_FIXED_LIST_DEFAULT_ALLOCATOR);

		template <typename InputIterator>
		fixed_list(InputIterator first, InputIterator last);

		this_type& operator=(const this_type& x);
		this_type& operator=(std::initializer_list<value_type> ilist);
		#if EASTL_MOVE_SEMANTICS_ENABLED
			this_type& operator=(this_type&& x);
		#endif

		void      swap(this_type& x);
		void      reset_lose_memory();      // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.
		size_type max_size() const;         // Returns the max fixed size, which is the user-supplied nodeCount parameter.
		bool      full() const;             // Returns true if the fixed space has been fully allocated. Note that if overflow is enabled, the container size can be greater than nodeCount but full() could return true because the fixed space may have a recently freed slot. 
		bool      has_overflowed() const;   // Returns true if the allocations spilled over into the overflow allocator. Meaningful only if overflow is enabled.
		bool      can_overflow() const;     // Returns the value of the bEnableOverflow template parameter.

		// OverflowAllocator
		const overflow_allocator_type& get_overflow_allocator() const EA_NOEXCEPT;
		overflow_allocator_type&       get_overflow_allocator() EA_NOEXCEPT;
		void                           set_overflow_allocator(const overflow_allocator_type& allocator);
	}; // fixed_list



	///////////////////////////////////////////////////////////////////////
	// fixed_list
	///////////////////////////////////////////////////////////////////////

	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_list()
		: base_type(fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_LIST_DEFAULT_NAME);
		#endif
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_list(const overflow_allocator_type& overflowAllocator)
		: base_type(fixed_allocator_type(mBuffer, overflowAllocator))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_LIST_DEFAULT_NAME);
		#endif
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_list(size_type n)
		: base_type(fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_LIST_DEFAULT_NAME);
		#endif

		resize(n);
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_list(size_type n, const value_type& value)
		: base_type(fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_LIST_DEFAULT_NAME);
		#endif

		resize(n, value);
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_list(const this_type& x)
		: base_type(fixed_allocator_type(mBuffer))
	{
		mAllocator.copy_overflow_allocator(x.mAllocator);

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(x.mAllocator.get_name());
		#endif

		assign(x.begin(), x.end());
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
		inline fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_list(this_type&& x)
			: base_type(fixed_allocator_type(mBuffer))
		{
			// Since we are a fixed_list, we can't normally swap pointers unless both this and 
			// x are using using overflow and the overflow allocators are equal. To do:
			//if(has_overflowed() && x.has_overflowed() && (get_overflow_allocator() == x.get_overflow_allocator()))
			//{
			//    We can swap contents and may need to swap the allocators as well.
			//}

			// The following is currently identical to the fixed_vector(const this_type& x) code above. If it stays that
			// way then we may want to make a shared implementation.
			mAllocator.copy_overflow_allocator(x.mAllocator);

			#if EASTL_NAME_ENABLED
				mAllocator.set_name(x.mAllocator.get_name());
			#endif

			assign(x.begin(), x.end());
		}


		template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
		inline fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_list(this_type&& x, const overflow_allocator_type& overflowAllocator)
			: base_type(fixed_allocator_type(mBuffer, overflowAllocator))
		{
			// See comments above.
			mAllocator.copy_overflow_allocator(x.mAllocator);

			#if EASTL_NAME_ENABLED
				mAllocator.set_name(x.mAllocator.get_name());
			#endif

			assign(x.begin(), x.end());
		}
	#endif


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_list(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator)
		: base_type(fixed_allocator_type(mBuffer, overflowAllocator))
	{
		assign(ilist.begin(), ilist.end());
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	template <typename InputIterator>
	fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::fixed_list(InputIterator first, InputIterator last)
		: base_type(fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_LIST_DEFAULT_NAME);
		#endif

		assign(first, last);
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::this_type&
	fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::operator=(const this_type& x)
	{
		if(this != &x)
		{
			base_type::clear();

			#if EASTL_ALLOCATOR_COPY_ENABLED
				mAllocator = x.mAllocator; // The primary effect of this is to copy the overflow allocator.
			#endif

			base_type::assign(x.begin(), x.end()); // It would probably be better to implement this like list::operator=.
		}
		return *this;
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
		inline typename fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::this_type&
		fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::operator=(this_type&& x)
		{
			return operator=(x);
		}
	#endif


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::this_type&
	fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::operator=(std::initializer_list<value_type> ilist)
	{
		base_type::clear();
		base_type::assign(ilist.begin(), ilist.end());
		return *this;
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::swap(this_type& x)
	{
		// Fixed containers use a special swap that can deal with excessively large buffers.
		eastl::fixed_swap(*this, x);
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::reset_lose_memory()
	{
		base_type::reset_lose_memory();
		get_allocator().reset(mBuffer);
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::size_type
	fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::max_size() const
	{
		return kMaxSize;
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline bool fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::full() const
	{
		// Note: This implementation isn't right in the case of bEnableOverflow = true because it will return 
		// false for the case that  there are free nodes from the buffer but also nodes from the dynamic heap. 
		// This can happen if the container exceeds the fixed size and then frees some of the nodes from the fixed buffer.
		// The only simple fix for this is to take on another member variable which tracks whether this overflow
		// has occurred at some point in the past.
		return !mAllocator.can_allocate(); // This is the quickest way of detecting this. has_overflowed uses a different method because it can't use this quick method.
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline bool fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::has_overflowed() const
	{
		#if EASTL_FIXED_SIZE_TRACKING_ENABLED // If we can use this faster pathway (as size() may be slow)...
			return (mAllocator.mPool.mnPeakSize > kMaxSize);
		#else
			return (size() > kMaxSize);
		#endif
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline bool fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::can_overflow() const
	{
		return bEnableOverflow;
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline const typename fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::overflow_allocator_type& 
	fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::get_overflow_allocator() const EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline typename fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::overflow_allocator_type& 
	fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::get_overflow_allocator() EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void 
	fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>::set_overflow_allocator(const overflow_allocator_type& allocator)
	{
		mAllocator.set_overflow_allocator(allocator);
	}


	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename T, size_t nodeCount, bool bEnableOverflow, typename OverflowAllocator>
	inline void swap(fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>& a, 
					 fixed_list<T, nodeCount, bEnableOverflow, OverflowAllocator>& b)
	{
		// Fixed containers use a special swap that can deal with excessively large buffers.
		eastl::fixed_swap(a, b);
	}


} // namespace eastl


#endif // Header include guard












