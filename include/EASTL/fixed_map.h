/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements a map and multimap which use a fixed size memory 
// pool for their nodes. 
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_FIXED_MAP_H
#define EASTL_FIXED_MAP_H


#include <EASTL/map.h>
#include <EASTL/fixed_set.h> // Included because fixed_rbtree_base resides here.

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{
	/// EASTL_FIXED_MAP_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	/// In the case of fixed-size containers, the allocator name always refers
	/// to overflow allocations. 
	///
	#ifndef EASTL_FIXED_MAP_DEFAULT_NAME
		#define EASTL_FIXED_MAP_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " fixed_map" // Unless the user overrides something, this is "EASTL fixed_map".
	#endif

	#ifndef EASTL_FIXED_MULTIMAP_DEFAULT_NAME
		#define EASTL_FIXED_MULTIMAP_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " fixed_multimap" // Unless the user overrides something, this is "EASTL fixed_multimap".
	#endif


	/// EASTL_FIXED_MAP_DEFAULT_ALLOCATOR
	/// EASTL_FIXED_MULTIMAP_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_FIXED_MAP_DEFAULT_ALLOCATOR
		#define EASTL_FIXED_MAP_DEFAULT_ALLOCATOR overflow_allocator_type(EASTL_FIXED_MAP_DEFAULT_NAME)
	#endif

	#ifndef EASTL_FIXED_MULTIMAP_DEFAULT_ALLOCATOR
		#define EASTL_FIXED_MULTIMAP_DEFAULT_ALLOCATOR overflow_allocator_type(EASTL_FIXED_MULTIMAP_DEFAULT_NAME)
	#endif



	/// fixed_map
	///
	/// Implements a map with a fixed block of memory identified by the 
	/// nodeCount template parameter. 
	///
	///     Key                    The key object (key in the key/value pair).
	///     T                      The mapped object (value in the key/value pair).
	///     nodeCount              The max number of objects to contain.
	///     bEnableOverflow        Whether or not we should use the global heap if our object pool is exhausted.
	///     Compare                Compare function/object for set ordering.
	///     OverflowAllocator              Overflow allocator, which is only used if bEnableOverflow == true. Defaults to the global heap.
	///
	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow = true, typename Compare = eastl::less<Key>, typename OverflowAllocator = EASTLAllocatorType>
	class fixed_map : public map<Key, T, Compare, fixed_node_allocator<sizeof(typename map<Key, T>::node_type), 
		nodeCount, EASTL_ALIGN_OF(eastl::pair<Key, T>), 0, bEnableOverflow, OverflowAllocator> >
	{
	public:
		typedef fixed_node_allocator<sizeof(typename map<Key, T>::node_type), nodeCount, 
					 EASTL_ALIGN_OF(eastl::pair<Key, T>), 0, bEnableOverflow, OverflowAllocator>                           fixed_allocator_type;
		typedef typename fixed_allocator_type::overflow_allocator_type                                                     overflow_allocator_type;
		typedef fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>                                  this_type;
		typedef map<Key, T, Compare, fixed_allocator_type>                                                                 base_type;
		typedef typename base_type::value_type                                                                             value_type;
		typedef typename base_type::node_type                                                                              node_type;
		typedef typename base_type::size_type                                                                              size_type;

		enum { kMaxSize = nodeCount };
		   
		using base_type::insert;

	protected:
		char mBuffer[fixed_allocator_type::kBufferSize]; // kBufferSize will take into account alignment requirements.

		using base_type::mAllocator;

	public:
		fixed_map();
		explicit fixed_map(const overflow_allocator_type& overflowAllocator);
		explicit fixed_map(const Compare& compare);
		fixed_map(const this_type& x);
		#if EASTL_MOVE_SEMANTICS_ENABLED
			fixed_map(this_type&& x);
			fixed_map(this_type&& x, const overflow_allocator_type& overflowAllocator);
		#endif
		fixed_map(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator = EASTL_FIXED_MAP_DEFAULT_ALLOCATOR);

		template <typename InputIterator>
		fixed_map(InputIterator first, InputIterator last);

		this_type& operator=(const this_type& x);
		this_type& operator=(std::initializer_list<value_type> ilist);
		#if EASTL_MOVE_SEMANTICS_ENABLED
			this_type& operator=(this_type&& x);
		#endif

		void swap(this_type& x);

		void reset_lose_memory(); // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.

		size_type max_size() const;

		const overflow_allocator_type& get_overflow_allocator() const EA_NOEXCEPT;
		overflow_allocator_type&       get_overflow_allocator() EA_NOEXCEPT;
		void                           set_overflow_allocator(const overflow_allocator_type& allocator);
	}; // fixed_map




	/// fixed_multimap
	///
	/// Implements a multimap with a fixed block of memory identified by the 
	/// nodeCount template parameter. 
	///
	///     Key                    The key object (key in the key/value pair).
	///     T                      The mapped object (value in the key/value pair).
	///     nodeCount              The max number of objects to contain.
	///     bEnableOverflow        Whether or not we should use the global heap if our object pool is exhausted.
	///     Compare                Compare function/object for set ordering.
	///     OverflowAllocator              Overflow allocator, which is only used if bEnableOverflow == true. Defaults to the global heap.
	///
	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow = true, typename Compare = eastl::less<Key>, typename OverflowAllocator = EASTLAllocatorType>
	class fixed_multimap : public multimap<Key, T, Compare, fixed_node_allocator<sizeof(typename multimap<Key, T>::node_type), 
										   nodeCount, EASTL_ALIGN_OF(eastl::pair<Key, T>), 0, bEnableOverflow, OverflowAllocator> >
	{
	public:
		typedef fixed_node_allocator<sizeof(typename multimap<Key, T>::node_type), nodeCount, 
					EASTL_ALIGN_OF(eastl::pair<Key, T>), 0, bEnableOverflow, OverflowAllocator>                                      fixed_allocator_type;
		typedef typename fixed_allocator_type::overflow_allocator_type                                                               overflow_allocator_type;
		typedef multimap<Key, T, Compare, fixed_allocator_type>                                                                      base_type;
		typedef fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>                                       this_type;
		typedef typename base_type::value_type                                                                                       value_type;
		typedef typename base_type::node_type                                                                                        node_type;
		typedef typename base_type::size_type                                                                                        size_type;

		enum { kMaxSize = nodeCount };

		using base_type::insert;

	protected:
		char mBuffer[fixed_allocator_type::kBufferSize]; // kBufferSize will take into account alignment requirements.

		using base_type::mAllocator;

	public:
		fixed_multimap();
		fixed_multimap(const overflow_allocator_type& overflowAllocator);
		explicit fixed_multimap(const Compare& compare);
		fixed_multimap(const this_type& x);
		#if EASTL_MOVE_SEMANTICS_ENABLED
			fixed_multimap(this_type&& x);
			fixed_multimap(this_type&& x, const overflow_allocator_type& overflowAllocator);
		#endif
		fixed_multimap(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator = EASTL_FIXED_MULTIMAP_DEFAULT_ALLOCATOR);

		template <typename InputIterator>
		fixed_multimap(InputIterator first, InputIterator last);

		this_type& operator=(const this_type& x);
		this_type& operator=(std::initializer_list<value_type> ilist);
		#if EASTL_MOVE_SEMANTICS_ENABLED
		this_type& operator=(this_type&& x);
		#endif

		void swap(this_type& x);

		void reset_lose_memory(); // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.

		size_type max_size() const;

		const overflow_allocator_type& get_overflow_allocator() const EA_NOEXCEPT;
		overflow_allocator_type&       get_overflow_allocator() EA_NOEXCEPT;
		void                           set_overflow_allocator(const overflow_allocator_type& allocator);
	}; // fixed_multimap





	///////////////////////////////////////////////////////////////////////
	// fixed_map
	///////////////////////////////////////////////////////////////////////

	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_map()
		: base_type(fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_MAP_DEFAULT_NAME);
		#endif
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_map(const overflow_allocator_type& overflowAllocator)
		: base_type(fixed_allocator_type(mBuffer, overflowAllocator))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_MAP_DEFAULT_NAME);
		#endif
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_map(const Compare& compare)
		: base_type(compare, fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_MAP_DEFAULT_NAME);
		#endif
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_map(const this_type& x)
		: base_type(x.mCompare, fixed_allocator_type(mBuffer))
	{
		mAllocator.copy_overflow_allocator(x.mAllocator);

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(x.mAllocator.get_name());
		#endif

		base_type::operator=(x);
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
		inline fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_map(this_type&& x)
			: base_type(x.mCompare, fixed_allocator_type(mBuffer))
		{
			mAllocator.copy_overflow_allocator(x.mAllocator);

			#if EASTL_NAME_ENABLED
				mAllocator.set_name(x.mAllocator.get_name());
			#endif

			base_type::operator=(x);
		}


		template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
		inline fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_map(this_type&& x, const overflow_allocator_type& overflowAllocator)
			: base_type(x.mCompare, fixed_allocator_type(mBuffer, overflowAllocator))
		{
			mAllocator.copy_overflow_allocator(x.mAllocator);

			#if EASTL_NAME_ENABLED
				mAllocator.set_name(x.mAllocator.get_name());
			#endif

			base_type::operator=(x);
		}
	#endif


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_map(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator)
		: base_type(fixed_allocator_type(mBuffer, overflowAllocator))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_MAP_DEFAULT_NAME);
		#endif

		insert(ilist.begin(), ilist.end());
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	template <typename InputIterator>
	fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_map(InputIterator first, InputIterator last)
		: base_type(fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_MAP_DEFAULT_NAME);
		#endif

		insert(first, last);
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::this_type&
	fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::operator=(const this_type& x)
	{
		base_type::operator=(x);
		return *this;
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::this_type&
	fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::operator=(std::initializer_list<value_type> ilist)
	{
		base_type::clear();
		insert(ilist.begin(), ilist.end());
		return *this;
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
		inline typename fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::this_type&
		fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::operator=(this_type&& x)
		{
			base_type::operator=(x);
			return *this;
		}
	#endif


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::swap(this_type& x)
	{
		// Fixed containers use a special swap that can deal with excessively large buffers.
		eastl::fixed_swap(*this, x);
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::reset_lose_memory()
	{
		base_type::reset_lose_memory();
		base_type::get_allocator().reset(mBuffer);
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::size_type
	fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::max_size() const
	{
		return kMaxSize;
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline const typename fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::overflow_allocator_type&
	fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::get_overflow_allocator() const EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::overflow_allocator_type&
	fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::get_overflow_allocator() EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void
	fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::set_overflow_allocator(const overflow_allocator_type& allocator)
	{
		mAllocator.set_overflow_allocator(allocator);
	}

	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void swap(fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>& a, 
					 fixed_map<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>& b)
	{
		// Fixed containers use a special swap that can deal with excessively large buffers.
		eastl::fixed_swap(a, b);
	}




	///////////////////////////////////////////////////////////////////////
	// fixed_multimap
	///////////////////////////////////////////////////////////////////////

	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_multimap()
		: base_type(fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_MULTIMAP_DEFAULT_NAME);
		#endif
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_multimap(const overflow_allocator_type& overflowAllocator)
		: base_type(fixed_allocator_type(mBuffer, overflowAllocator))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_MULTIMAP_DEFAULT_NAME);
		#endif
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_multimap(const Compare& compare)
		: base_type(compare, fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_MULTIMAP_DEFAULT_NAME);
		#endif
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_multimap(const this_type& x)
		: base_type(x.mCompare, fixed_allocator_type(mBuffer))
	{
		mAllocator.copy_overflow_allocator(x.mAllocator);

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(x.mAllocator.get_name());
		#endif

		base_type::operator=(x);
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
		inline fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_multimap(this_type&& x)
			: base_type(x.mCompare, fixed_allocator_type(mBuffer))
		{
			mAllocator.copy_overflow_allocator(x.mAllocator);

			#if EASTL_NAME_ENABLED
				mAllocator.set_name(x.mAllocator.get_name());
			#endif

			base_type::operator=(x);
		}


		template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
		inline fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_multimap(this_type&& x, const overflow_allocator_type& overflowAllocator)
			: base_type(x.mCompare, fixed_allocator_type(mBuffer, overflowAllocator))
		{
			mAllocator.copy_overflow_allocator(x.mAllocator);

			#if EASTL_NAME_ENABLED
				mAllocator.set_name(x.mAllocator.get_name());
			#endif

			base_type::operator=(x);
		}
	#endif


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::fixed_multimap(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator)
		: base_type(fixed_allocator_type(mBuffer, overflowAllocator))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_MULTIMAP_DEFAULT_NAME);
		#endif

		insert(ilist.begin(), ilist.end());
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	template <typename InputIterator>
	fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::
	fixed_multimap(InputIterator first, InputIterator last)
		: base_type(fixed_allocator_type(mBuffer))
	{
		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_MULTIMAP_DEFAULT_NAME);
		#endif

		insert(first, last);
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::this_type& 
	fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::operator=(const this_type& x)
	{
		base_type::operator=(x);
		return *this;
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::this_type&
	fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::operator=(std::initializer_list<value_type> ilist)
	{
		base_type::clear();
		insert(ilist.begin(), ilist.end());
		return *this;
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
		inline typename fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::this_type&
		fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::operator=(this_type&& x)
		{
			base_type::operator=(x);
			return *this;
		}
	#endif


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::swap(this_type& x)
	{
		// Fixed containers use a special swap that can deal with excessively large buffers.
		eastl::fixed_swap(*this, x);
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::reset_lose_memory()
	{
		base_type::reset_lose_memory();
		base_type::get_allocator().reset(mBuffer);
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::size_type 
	fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::max_size() const
	{
		return kMaxSize;
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline const typename fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::overflow_allocator_type&
	fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::get_overflow_allocator() const EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline typename fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::overflow_allocator_type&
	fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::get_overflow_allocator() EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void
	fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>::set_overflow_allocator(const overflow_allocator_type& allocator)
	{
		mAllocator.set_overflow_allocator(allocator);
	}


	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow, typename Compare, typename OverflowAllocator>
	inline void swap(fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>& a, 
					 fixed_multimap<Key, T, nodeCount, bEnableOverflow, Compare, OverflowAllocator>& b)
	{
		// Fixed containers use a special swap that can deal with excessively large buffers.
		eastl::fixed_swap(a, b);
	}


} // namespace eastl


#endif // Header include guard









