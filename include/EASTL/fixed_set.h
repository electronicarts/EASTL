/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements a set and multiset which use a fixed size memory 
// pool for their nodes. 
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_FIXED_SET_H
#define EASTL_FIXED_SET_H


#include <EASTL/set.h>
#include <EASTL/internal/fixed_pool.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{
	/// EASTL_FIXED_SET_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	/// In the case of fixed-size containers, the allocator name always refers
	/// to overflow allocations. 
	///
	#ifndef EASTL_FIXED_SET_DEFAULT_NAME
		#define EASTL_FIXED_SET_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " fixed_set" // Unless the user overrides something, this is "EASTL fixed_set".
	#endif

	#ifndef EASTL_FIXED_MULTISET_DEFAULT_NAME
		#define EASTL_FIXED_MULTISET_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " fixed_multiset" // Unless the user overrides something, this is "EASTL fixed_multiset".
	#endif


	/// EASTL_FIXED_SET_DEFAULT_ALLOCATOR
	/// EASTL_FIXED_MULTISET_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_FIXED_SET_DEFAULT_ALLOCATOR
		#define EASTL_FIXED_SET_DEFAULT_ALLOCATOR overflow_allocator_type(EASTL_FIXED_SET_DEFAULT_NAME)
	#endif

	#ifndef EASTL_FIXED_MULTISET_DEFAULT_ALLOCATOR
		#define EASTL_FIXED_MULTISET_DEFAULT_ALLOCATOR overflow_allocator_type(EASTL_FIXED_MULTISET_DEFAULT_NAME)
	#endif



	/// fixed_set
	///
	/// Implements a set with a fixed block of memory identified by the 
	/// nodeCount template parameter. 
	///
	/// Template parameters:
	///     Key                    The type of object the set holds (a.k.a. value).
	///     nodeCount              The max number of objects to contain.
	///     bEnableOverflow        Whether or not we should use the global heap if our object pool is exhausted.
	///     Compare                Compare function/object for set ordering.
	///     OverflowAllocator              Overflow allocator, which is only used if bEnableOverflow == true. Defaults to the global heap.
	///
	template <typename Key, size_t nodeCount, bool bEnableOverflow = true, typename Compare = eastl::less<Key>, typename OverflowAllocator = EASTLAllocatorType>
	class fixed_set : public set<Key, Compare, fixed_node_allocator<sizeof(typename set<Key>::node_type), 
								 nodeCount, EASTL_ALIGN_OF(Key), 0, bEnableOverflow, OverflowAllocator> >
	{
	public:
		typedef fixed_node_allocator<sizeof(typename set<Key>::node_type), nodeCount, 
					EASTL_ALIGN_OF(Key), 0, bEnableOverflow, OverflowAllocator>            fixed_allocator_type;
		typedef typename fixed_allocator_type::overflow_allocator_type                     overflow_allocator_type;
		typedef set<Key, Compare, fixed_allocator_type>                                    base_type;
		typedef fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>     this_type;
		typedef typename base_type::value_type                                             value_type;
		typedef typename base_type::node_type                                              node_type;
		typedef typename base_type::size_type                                              size_type;

		enum { kMaxSize = nodeCount };

		using base_type::insert;

	protected:
		char mBuffer[fixed_allocator_type::kBufferSize]; // kBufferSize will take into account alignment requirements.

		using base_type::mAllocator;

	public:
		fixed_set();
		fixed_set(const overflow_allocator_type& overflowAllocator);
		explicit fixed_set(const Compare& compare);
		fixed_set(const this_type& x);
		fixed_set(this_type&& x);
		fixed_set(this_type&& x, const overflow_allocator_type& overflowAllocator);
		fixed_set(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator = EASTL_FIXED_SET_DEFAULT_ALLOCATOR);

		template <typename InputIterator>
		fixed_set(InputIterator first, InputIterator last);

		this_type& operator=(const this_type& x);
		this_type& operator=(std::initializer_list<value_type> ilist);
		this_type& operator=(this_type&& x);

		void swap(this_type& x);

		void reset_lose_memory(); // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.

		size_type max_size() const;

		const overflow_allocator_type& get_overflow_allocator() const EA_NOEXCEPT;
		overflow_allocator_type&       get_overflow_allocator() EA_NOEXCEPT;
		void                           set_overflow_allocator(const overflow_allocator_type& allocator);
	}; // fixed_set






	/// fixed_multiset
	///
	/// Implements a multiset with a fixed block of memory identified by the 
	/// nodeCount template parameter. 
	///
	///     Key                    The type of object the set holds (a.k.a. value).
	///     nodeCount              The max number of objects to contain.
	///     bEnableOverflow        Whether or not we should use the global heap if our object pool is exhausted.
	///     Compare                Compare function/object for set ordering.
	///     OverflowAllocator              Overflow allocator, which is only used if bEnableOverflow == true. Defaults to the global heap.
	///
	template <typename Key, size_t nodeCount, bool bEnableOverflow = true, typename Compare = eastl::less<Key>, typename OverflowAllocator = EASTLAllocatorType>
	class fixed_multiset : public multiset<Key, Compare, fixed_node_allocator<sizeof(typename multiset<Key>::node_type), 
										   nodeCount, EASTL_ALIGN_OF(Key), 0, bEnableOverflow, OverflowAllocator> >
	{
	public:
		typedef fixed_node_allocator<sizeof(typename multiset<Key>::node_type), nodeCount, 
					 EASTL_ALIGN_OF(Key), 0, bEnableOverflow, OverflowAllocator>                fixed_allocator_type;
		typedef typename fixed_allocator_type::overflow_allocator_type                          overflow_allocator_type;
		typedef multiset<Key, Compare, fixed_allocator_type>                                    base_type;
		typedef fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>     this_type;
		typedef typename base_type::value_type                                                  value_type;
		typedef typename base_type::node_type                                                   node_type;
		typedef typename base_type::size_type                                                   size_type;

		enum { kMaxSize = nodeCount };

		using base_type::insert;

	protected:
		char mBuffer[fixed_allocator_type::kBufferSize]; // kBufferSize will take into account alignment requirements.

		using base_type::mAllocator;

	public:
		fixed_multiset();
		fixed_multiset(const overflow_allocator_type& overflowAllocator);
		explicit fixed_multiset(const Compare& compare);
		fixed_multiset(const this_type& x);
		fixed_multiset(this_type&& x);
		fixed_multiset(this_type&& x, const overflow_allocator_type& overflowAllocator);
		fixed_multiset(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator = EASTL_FIXED_MULTISET_DEFAULT_ALLOCATOR);

		template <typename InputIterator>
		fixed_multiset(InputIterator first, InputIterator last);

		this_type& operator=(const this_type& x);
		this_type& operator=(std::initializer_list<value_type> ilist);
		this_type& operator=(this_type&& x);

		void swap(this_type& x);

		void reset_lose_memory(); // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.

		size_type max_size() const;

		const overflow_allocator_type& get_overflow_allocator() const EA_NOEXCEPT;
		overflow_allocator_type&       get_overflow_allocator() EA_NOEXCEPT;
		void                           set_overflow_allocator(const overflow_allocator_type& allocator);
	}; // fixed_multiset




	///////////////////////////////////////////////////////////////////////
	// fixed_set
	///////////////////////////////////////////////////////////////////////

	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_set()
		: base_type(fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_SET_DEFAULT_NAME);
		#endif
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_set(const overflow_allocator_type& overflowAllocator)
		: base_type(fixed_allocator_type(mBuffer, overflowAllocator))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_SET_DEFAULT_NAME);
		#endif
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_set(const Compare& compare)
		: base_type(compare, fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_SET_DEFAULT_NAME);
		#endif
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_set(const this_type& x)
		: base_type(x.mCompare, fixed_allocator_type(mBuffer))
	{
		mAllocator.copy_overflow_allocator(x.mAllocator);

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(x.mAllocator.get_name());
		#endif

		base_type::operator=(x);
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_set(this_type&& x)
		: base_type(x.mCompare, fixed_allocator_type(mBuffer))
	{
		mAllocator.copy_overflow_allocator(x.mAllocator);

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(x.mAllocator.get_name());
		#endif

		base_type::operator=(x);
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_set(this_type&& x, const overflow_allocator_type& overflowAllocator)
		: base_type(x.mCompare, fixed_allocator_type(mBuffer, overflowAllocator))
	{
		mAllocator.copy_overflow_allocator(x.mAllocator);

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(x.mAllocator.get_name());
		#endif

		base_type::operator=(x);
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_set(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator)
		: base_type(fixed_allocator_type(mBuffer, overflowAllocator))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_SET_DEFAULT_NAME);
		#endif

		insert(ilist.begin(), ilist.end());
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	template <typename InputIterator>
	fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_set(InputIterator first, InputIterator last)
		: base_type(fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_SET_DEFAULT_NAME);
		#endif

		insert(first, last);
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::this_type& 
	fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::operator=(const this_type& x)
	{
		base_type::operator=(x);
		return *this;
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::this_type&
	fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::operator=(std::initializer_list<value_type> ilist)
	{
		base_type::clear();
		insert(ilist.begin(), ilist.end());
		return *this;
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::this_type&
	fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::operator=(this_type&& x)
	{
		base_type::operator=(x);
		return *this;
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::swap(this_type& x)
	{
		// Fixed containers use a special swap that can deal with excessively large buffers.
		eastl::fixed_swap(*this, x);
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::reset_lose_memory()
	{
		base_type::reset_lose_memory();
		base_type::get_allocator().reset(mBuffer);
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::size_type 
	fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::max_size() const
	{
		return kMaxSize;
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline const typename fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::overflow_allocator_type& 
	fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::get_overflow_allocator() const EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::overflow_allocator_type& 
	fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::get_overflow_allocator() EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::set_overflow_allocator(const overflow_allocator_type& allocator)
	{
		mAllocator.set_overflow_allocator(allocator);
	}


	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void swap(fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>& a, 
					 fixed_set<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>& b)
	{
		// Fixed containers use a special swap that can deal with excessively large buffers.
		eastl::fixed_swap(a, b);
	}



	///////////////////////////////////////////////////////////////////////
	// fixed_multiset
	///////////////////////////////////////////////////////////////////////

	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_multiset()
		: base_type(fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_MULTISET_DEFAULT_NAME);
		#endif
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_multiset(const overflow_allocator_type& overflowAllocator)
		: base_type(fixed_allocator_type(mBuffer, overflowAllocator))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_MULTISET_DEFAULT_NAME);
		#endif
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_multiset(const Compare& compare)
		: base_type(compare, fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_MULTISET_DEFAULT_NAME);
		#endif
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_multiset(const this_type& x)
		: base_type(x.mCompare, fixed_allocator_type(mBuffer))
	{
		mAllocator.copy_overflow_allocator(x.mAllocator);

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(x.mAllocator.get_name());
		#endif

		base_type::operator=(x);
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_multiset(this_type&& x)
		: base_type(x.mCompare, fixed_allocator_type(mBuffer))
	{
		mAllocator.copy_overflow_allocator(x.mAllocator);

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(x.mAllocator.get_name());
		#endif

		base_type::operator=(x);
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_multiset(this_type&& x, const overflow_allocator_type& overflowAllocator)
		: base_type(x.mCompare, fixed_allocator_type(mBuffer, overflowAllocator))
	{
		mAllocator.copy_overflow_allocator(x.mAllocator);

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(x.mAllocator.get_name());
		#endif

		base_type::operator=(x);
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_multiset(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator)
		: base_type(fixed_allocator_type(mBuffer, overflowAllocator))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_MULTISET_DEFAULT_NAME);
		#endif

		insert(ilist.begin(), ilist.end());
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	template <typename InputIterator>
	fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_multiset(InputIterator first, InputIterator last)
		: base_type(fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_MULTISET_DEFAULT_NAME);
		#endif

		insert(first, last);
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::this_type& 
	fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::operator=(const this_type& x)
	{
		base_type::operator=(x);
		return *this;
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::this_type&
	fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::operator=(std::initializer_list<value_type> ilist)
	{
		base_type::clear();
		insert(ilist.begin(), ilist.end());
		return *this;
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::this_type&
	fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::operator=(this_type&& x)
	{
		base_type::operator=(x);
		return *this;
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::swap(this_type& x)
	{
		// Fixed containers use a special swap that can deal with excessively large buffers.
		eastl::fixed_swap(*this, x);
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::reset_lose_memory()
	{
		base_type::reset_lose_memory();
		base_type::get_allocator().reset(mBuffer);
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::size_type 
	fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::max_size() const
	{
		return kMaxSize;
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline const typename fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::overflow_allocator_type& 
	fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::get_overflow_allocator() const EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::overflow_allocator_type& 
	fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::get_overflow_allocator() EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::set_overflow_allocator(const overflow_allocator_type& allocator)
	{
		mAllocator.set_overflow_allocator(allocator);
	}


	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename Key, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void swap(fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>& a, 
					 fixed_multiset<Key, nodeCount, bEnableOverflow, Compare, OverflowAllocator>& b)
	{
		// Fixed containers use a special swap that can deal with excessively large buffers.
		eastl::fixed_swap(a, b);
	}



} // namespace eastl


#endif // Header include guard









