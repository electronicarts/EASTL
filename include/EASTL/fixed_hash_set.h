/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements a hash_set which uses a fixed size memory pool for 
// its buckets and nodes. 
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_FIXED_HASH_SET_H
#define EASTL_FIXED_HASH_SET_H


#include <EASTL/hash_set.h>
#include <EASTL/internal/fixed_pool.h>

EA_DISABLE_VC_WARNING(4127) // Conditional expression is constant

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{
	/// EASTL_FIXED_HASH_SET_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	/// In the case of fixed-size containers, the allocator name always refers
	/// to overflow allocations. 
	///
	#ifndef EASTL_FIXED_HASH_SET_DEFAULT_NAME
		#define EASTL_FIXED_HASH_SET_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " fixed_hash_set" // Unless the user overrides something, this is "EASTL fixed_hash_set".
	#endif

	#ifndef EASTL_FIXED_HASH_MULTISET_DEFAULT_NAME
		#define EASTL_FIXED_HASH_MULTISET_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " fixed_hash_multiset" // Unless the user overrides something, this is "EASTL fixed_hash_multiset".
	#endif


	/// EASTL_FIXED_HASH_SET_DEFAULT_ALLOCATOR
	/// EASTL_FIXED_HASH_MULTISET_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_FIXED_HASH_SET_DEFAULT_ALLOCATOR
		#define EASTL_FIXED_HASH_SET_DEFAULT_ALLOCATOR overflow_allocator_type(EASTL_FIXED_HASH_SET_DEFAULT_NAME)
	#endif

	#ifndef EASTL_FIXED_HASH_MULTISET_DEFAULT_ALLOCATOR
		#define EASTL_FIXED_HASH_MULTISET_DEFAULT_ALLOCATOR overflow_allocator_type(EASTL_FIXED_HASH_MULTISET_DEFAULT_NAME)
	#endif



	/// fixed_hash_set
	///
	/// Implements a hash_set with a fixed block of memory identified by the nodeCount and bucketCount
	/// template parameters. 
	///
	/// Template parameters:
	///     Value                  The type of object the hash_set holds.
	///     nodeCount              The max number of objects to contain. This value must be >= 1.
	///     bucketCount            The number of buckets to use. This value must be >= 2.
	///     bEnableOverflow        Whether or not we should use the global heap if our object pool is exhausted.
	///     Hash                   hash_set hash function. See hash_set.
	///     Predicate              hash_set equality testing function. See hash_set.
	///
	template <typename Value, size_t nodeCount, size_t bucketCount = nodeCount + 1, bool bEnableOverflow = true,
			  typename Hash = eastl::hash<Value>, typename Predicate = eastl::equal_to<Value>, bool bCacheHashCode = false, typename OverflowAllocator = EASTLAllocatorType>
	class fixed_hash_set : public hash_set<Value,
										   Hash,
										   Predicate,
										   fixed_hashtable_allocator<
												bucketCount + 1, 
												sizeof(typename hash_set<Value, Hash, Predicate, OverflowAllocator, bCacheHashCode>::node_type), 
												nodeCount, 
												EASTL_ALIGN_OF(Value), 
												0,
												bEnableOverflow,
												OverflowAllocator>, 
										   bCacheHashCode>
	{
	public:
		typedef fixed_hashtable_allocator<bucketCount + 1, sizeof(typename hash_set<Value, Hash, Predicate, 
						OverflowAllocator, bCacheHashCode>::node_type), nodeCount, EASTL_ALIGN_OF(Value), 0,
						bEnableOverflow, OverflowAllocator>                                                                        fixed_allocator_type;
		typedef typename fixed_allocator_type::overflow_allocator_type                                                             overflow_allocator_type;
		typedef fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator> this_type;
		typedef hash_set<Value, Hash, Predicate, fixed_allocator_type, bCacheHashCode>                                             base_type;
		typedef typename base_type::value_type                                                                                     value_type;
		typedef typename base_type::node_type                                                                                      node_type;
		typedef typename base_type::size_type                                                                                      size_type;

		enum { kMaxSize = nodeCount };

		using base_type::mAllocator;

	protected:
		node_type** mBucketBuffer[bucketCount + 1]; // '+1' because the hash table needs a null terminating bucket.
		char        mNodeBuffer[fixed_allocator_type::kBufferSize]; // kBufferSize will take into account alignment requirements.

	public:
		explicit fixed_hash_set(const overflow_allocator_type& overflowAllocator);

		explicit fixed_hash_set(const Hash& hashFunction = Hash(), 
								const Predicate& predicate = Predicate());

		fixed_hash_set(const Hash& hashFunction, 
					   const Predicate& predicate,
					   const overflow_allocator_type& overflowAllocator);

		template <typename InputIterator>
		fixed_hash_set(InputIterator first, InputIterator last,
					   const Hash& hashFunction = Hash(),
					   const Predicate& predicate = Predicate());

		fixed_hash_set(const this_type& x);
		#if EASTL_MOVE_SEMANTICS_ENABLED
			fixed_hash_set(this_type&& x);
			fixed_hash_set(this_type&& x, const overflow_allocator_type& overflowAllocator);
		#endif

		fixed_hash_set(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator = EASTL_FIXED_HASH_SET_DEFAULT_ALLOCATOR);

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
	}; // fixed_hash_set






	/// fixed_hash_multiset
	///
	/// Implements a hash_multiset with a fixed block of memory identified by the nodeCount and bucketCount
	/// template parameters. 
	///
	///     Value                  The type of object the hash_set holds.
	///     nodeCount              The max number of objects to contain. This value must be >= 1.
	///     bucketCount            The number of buckets to use. This value must be >= 2.
	///     bEnableOverflow        Whether or not we should use the global heap if our object pool is exhausted.
	///     Hash                   hash_set hash function. See hash_set.
	///     Predicate              hash_set equality testing function. See hash_set.
	///
	template <typename Value, size_t nodeCount, size_t bucketCount = nodeCount + 1, bool bEnableOverflow = true,
			  typename Hash = eastl::hash<Value>, typename Predicate = eastl::equal_to<Value>, bool bCacheHashCode = false, typename OverflowAllocator = EASTLAllocatorType>
	class fixed_hash_multiset : public hash_multiset<Value,
													 Hash,
													 Predicate,
													 fixed_hashtable_allocator<
														bucketCount + 1, 
														sizeof(typename hash_multiset<Value, Hash, Predicate, OverflowAllocator, bCacheHashCode>::node_type),
														nodeCount,
														EASTL_ALIGN_OF(Value), 
														0, 
														bEnableOverflow,
														OverflowAllocator>,
													 bCacheHashCode>
	{
	public:
		typedef fixed_hashtable_allocator<bucketCount + 1, sizeof(typename hash_multiset<Value, Hash, Predicate, 
					OverflowAllocator, bCacheHashCode>::node_type), nodeCount, EASTL_ALIGN_OF(Value), 0,
					bEnableOverflow, OverflowAllocator>                                                                                 fixed_allocator_type;
		typedef typename fixed_allocator_type::overflow_allocator_type                                                                  overflow_allocator_type;
		typedef hash_multiset<Value, Hash, Predicate, fixed_allocator_type, bCacheHashCode>                                             base_type;
		typedef fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator> this_type;
		typedef typename base_type::value_type                                                                                          value_type;
		typedef typename base_type::node_type                                                                                           node_type;
		typedef typename base_type::size_type                                                                                           size_type;

		enum { kMaxSize = nodeCount };

		using base_type::mAllocator;

	protected:
		node_type** mBucketBuffer[bucketCount + 1]; // '+1' because the hash table needs a null terminating bucket.
		char        mNodeBuffer[fixed_allocator_type::kBufferSize]; // kBufferSize will take into account alignment requirements.

	public:
		explicit fixed_hash_multiset(const overflow_allocator_type& overflowAllocator);

		explicit fixed_hash_multiset(const Hash& hashFunction = Hash(), 
									 const Predicate& predicate = Predicate());

		fixed_hash_multiset(const Hash& hashFunction, 
							const Predicate& predicate,
							const overflow_allocator_type& overflowAllocator);

		template <typename InputIterator>
		fixed_hash_multiset(InputIterator first, InputIterator last, 
							const Hash& hashFunction = Hash(), 
							const Predicate& predicate = Predicate());

		fixed_hash_multiset(const this_type& x);
		#if EASTL_MOVE_SEMANTICS_ENABLED
			fixed_hash_multiset(this_type&& x);
			fixed_hash_multiset(this_type&& x, const overflow_allocator_type& overflowAllocator);
		#endif
		fixed_hash_multiset(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator = EASTL_FIXED_HASH_MULTISET_DEFAULT_ALLOCATOR);

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
	}; // fixed_hash_multiset





	///////////////////////////////////////////////////////////////////////
	// fixed_hash_set
	///////////////////////////////////////////////////////////////////////

	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	fixed_hash_set(const overflow_allocator_type& overflowAllocator)
		: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), 
					Hash(), Predicate(), fixed_allocator_type(NULL, mBucketBuffer, overflowAllocator))
	{
		EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

		if(!bEnableOverflow)
			base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_HASH_SET_DEFAULT_NAME);
		#endif

		mAllocator.reset(mNodeBuffer);
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	fixed_hash_set(const Hash& hashFunction, 
				   const Predicate& predicate)
		: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), 
					hashFunction, predicate, fixed_allocator_type(NULL, mBucketBuffer))
	{
		EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

		if(!bEnableOverflow)
			base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_HASH_SET_DEFAULT_NAME);
		#endif

		mAllocator.reset(mNodeBuffer);
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	fixed_hash_set(const Hash& hashFunction, 
				   const Predicate& predicate,
				   const overflow_allocator_type& overflowAllocator)
		: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), 
					hashFunction, predicate, fixed_allocator_type(NULL, mBucketBuffer, overflowAllocator))
	{
		EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

		if(!bEnableOverflow)
			base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_HASH_SET_DEFAULT_NAME);
		#endif

		mAllocator.reset(mNodeBuffer);
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	template <typename InputIterator>
	fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	fixed_hash_set(InputIterator first, InputIterator last,
				   const Hash& hashFunction,
				   const Predicate& predicate)
		: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), hashFunction, 
					predicate, fixed_allocator_type(NULL, mBucketBuffer))
	{
		EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

		if(!bEnableOverflow)
			base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_HASH_SET_DEFAULT_NAME);
		#endif

		mAllocator.reset(mNodeBuffer);
		base_type::insert(first, last);
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	fixed_hash_set(const this_type& x)
		: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), x.hash_function(),
					x.equal_function(), fixed_allocator_type(NULL, mBucketBuffer))
	{
		mAllocator.copy_overflow_allocator(x.mAllocator);

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(x.mAllocator.get_name());
		#endif

		EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

		if(!bEnableOverflow)
			base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

		mAllocator.reset(mNodeBuffer);
		base_type::insert(x.begin(), x.end());
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename Key, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
		inline fixed_hash_set<Key, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::fixed_hash_set(this_type&& x)
		: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), x.hash_function(),
						x.equal_function(), fixed_allocator_type(NULL, mBucketBuffer))
		{
			// This implementation is the same as above. If we could rely on using C++11 delegating constructor support then we could just call that here.
			mAllocator.copy_overflow_allocator(x.mAllocator);

			#if EASTL_NAME_ENABLED
				mAllocator.set_name(x.mAllocator.get_name());
			#endif

			EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

			if(!bEnableOverflow)
				base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

			mAllocator.reset(mNodeBuffer);
			base_type::insert(x.begin(), x.end());
		}


		template <typename Key, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
		inline fixed_hash_set<Key, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::fixed_hash_set(this_type&& x, const overflow_allocator_type& overflowAllocator)
			: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), 
						x.hash_function(), x.equal_function(), fixed_allocator_type(NULL, mBucketBuffer, overflowAllocator))
		{
			// This implementation is the same as above. If we could rely on using C++11 delegating constructor support then we could just call that here.
			mAllocator.copy_overflow_allocator(x.mAllocator);

			#if EASTL_NAME_ENABLED
				mAllocator.set_name(x.mAllocator.get_name());
			#endif

			EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

			if(!bEnableOverflow)
				base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

			mAllocator.reset(mNodeBuffer);
			base_type::insert(x.begin(), x.end());
		}
	#endif


	template <typename Key, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline fixed_hash_set<Key, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	fixed_hash_set(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator)
		: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), Hash(), 
					Predicate(), fixed_allocator_type(NULL, mBucketBuffer, overflowAllocator))
	{
		EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

		if(!bEnableOverflow)
			base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_HASH_SET_DEFAULT_NAME);
		#endif

		mAllocator.reset(mNodeBuffer);
		base_type::insert(ilist.begin(), ilist.end());
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	typename fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::this_type& 
	fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::operator=(const this_type& x)
	{
		base_type::operator=(x);
		return *this;
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename Key, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
		inline typename fixed_hash_set<Key, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::this_type& 
		fixed_hash_set<Key, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::operator=(this_type&& x)
		{
			operator=(x);
			return *this;
		}
	#endif


	template <typename Key, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline typename fixed_hash_set<Key, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::this_type& 
	fixed_hash_set<Key, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::operator=(std::initializer_list<value_type> ilist)
	{
		base_type::clear();
		base_type::insert(ilist.begin(), ilist.end());
		return *this;
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline void fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	swap(this_type& x)
	{
		// We must do a brute-force swap, because fixed containers cannot share memory allocations.
		// Note that we create a temp value on the stack. This approach may fail if the size of the 
		// container is too large. We have a rule against allocating memory from the heap, and so 
		// if the user wants to swap two large objects of this class, the user will currently need 
		// to implement it manually. To consider: add code to allocate a temporary buffer if the 
		// size of the container is too large for the stack.
		EASTL_ASSERT(sizeof(x) < EASTL_MAX_STACK_USAGE); // It is dangerous to try to create objects that are too big for the stack.

		const this_type temp(*this); // Can't call eastl::swap because that would
		*this = x;                   // itself call this member swap function.
		x     = temp;
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	void fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	reset_lose_memory()
	{
		base_type::reset_lose_memory();
		base_type::get_allocator().reset(mNodeBuffer);
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline typename fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::size_type 
	fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::max_size() const
	{
		return kMaxSize;
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline const typename fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::overflow_allocator_type& 
	fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::get_overflow_allocator() const EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline typename fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::overflow_allocator_type& 
	fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::get_overflow_allocator() EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline void fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	set_overflow_allocator(const overflow_allocator_type& allocator)
	{
		mAllocator.set_overflow_allocator(allocator);
	}

	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode>
	inline void swap(fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode>& a, 
					 fixed_hash_set<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode>& b)
	{
		a.swap(b);
	}




	///////////////////////////////////////////////////////////////////////
	// fixed_hash_multiset
	///////////////////////////////////////////////////////////////////////

	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	fixed_hash_multiset(const overflow_allocator_type& overflowAllocator)
		: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), Hash(), 
					Predicate(), fixed_allocator_type(NULL, mBucketBuffer, overflowAllocator))
	{
		EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

		if(!bEnableOverflow)
			base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_HASH_MULTISET_DEFAULT_NAME);
		#endif

		mAllocator.reset(mNodeBuffer);
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	fixed_hash_multiset(const Hash& hashFunction, 
						const Predicate& predicate)
		: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), hashFunction, 
					predicate, fixed_allocator_type(NULL, mBucketBuffer))
	{
		EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

		if(!bEnableOverflow)
			base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_HASH_MULTISET_DEFAULT_NAME);
		#endif

		mAllocator.reset(mNodeBuffer);
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	fixed_hash_multiset(const Hash& hashFunction, 
						const Predicate& predicate,
						const overflow_allocator_type& overflowAllocator)
		: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), hashFunction, 
					predicate, fixed_allocator_type(NULL, mBucketBuffer, overflowAllocator))
	{
		EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

		if(!bEnableOverflow)
			base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_HASH_MULTISET_DEFAULT_NAME);
		#endif

		mAllocator.reset(mNodeBuffer);
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	template <typename InputIterator>
	inline fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	fixed_hash_multiset(InputIterator first, InputIterator last, 
						const Hash& hashFunction, 
						const Predicate& predicate)
		: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), hashFunction, 
					predicate, fixed_allocator_type(NULL, mBucketBuffer))
	{
		EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

		if(!bEnableOverflow)
			base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_HASH_MULTISET_DEFAULT_NAME);
		#endif

		mAllocator.reset(mNodeBuffer);
		base_type::insert(first, last);
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	fixed_hash_multiset(const this_type& x)
		: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), x.hash_function(), 
					x.equal_function(), fixed_allocator_type(NULL, mBucketBuffer))
	{
		mAllocator.copy_overflow_allocator(x.mAllocator);

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(x.mAllocator.get_name());
		#endif

		EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

		if(!bEnableOverflow)
			base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

		mAllocator.reset(mNodeBuffer);
		base_type::insert(x.begin(), x.end());
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename Key, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
		inline fixed_hash_multiset<Key, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::fixed_hash_multiset(this_type&& x)
			: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), x.hash_function(),
							x.equal_function(), fixed_allocator_type(NULL, mBucketBuffer))
		{
			// This implementation is the same as above. If we could rely on using C++11 delegating constructor support then we could just call that here.
			mAllocator.copy_overflow_allocator(x.mAllocator);

			#if EASTL_NAME_ENABLED
				mAllocator.set_name(x.mAllocator.get_name());
			#endif

			EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

			if(!bEnableOverflow)
				base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

			mAllocator.reset(mNodeBuffer);
			base_type::insert(x.begin(), x.end());
		}


		template <typename Key, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
		inline fixed_hash_multiset<Key, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::fixed_hash_multiset(this_type&& x, const overflow_allocator_type& overflowAllocator)
			: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), 
						x.hash_function(), x.equal_function(), fixed_allocator_type(NULL, mBucketBuffer, overflowAllocator))
		{
			// This implementation is the same as above. If we could rely on using C++11 delegating constructor support then we could just call that here.
			mAllocator.copy_overflow_allocator(x.mAllocator);

			#if EASTL_NAME_ENABLED
				mAllocator.set_name(x.mAllocator.get_name());
			#endif

			EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

			if(!bEnableOverflow)
				base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

			mAllocator.reset(mNodeBuffer);
			base_type::insert(x.begin(), x.end());
		}
	#endif


	template <typename Key, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline fixed_hash_multiset<Key, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	fixed_hash_multiset(std::initializer_list<value_type> ilist, const overflow_allocator_type& overflowAllocator)
		: base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), Hash(), 
					Predicate(), fixed_allocator_type(NULL, mBucketBuffer, overflowAllocator))
	{
		EASTL_CT_ASSERT((nodeCount >= 1) && (bucketCount >= 2));

		if(!bEnableOverflow)
			base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

		#if EASTL_NAME_ENABLED
			mAllocator.set_name(EASTL_FIXED_HASH_MULTISET_DEFAULT_NAME);
		#endif

		mAllocator.reset(mNodeBuffer);
		base_type::insert(ilist.begin(), ilist.end());
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline typename fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::this_type& 
	fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::operator=(const this_type& x)
	{
		base_type::operator=(x);
		return *this;
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename Key, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
		inline typename fixed_hash_multiset<Key, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::this_type& 
		fixed_hash_multiset<Key, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::operator=(this_type&& x)
		{
			base_type::operator=(x);
			return *this;
		}
	#endif


	template <typename Key, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline typename fixed_hash_multiset<Key, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::this_type& 
	fixed_hash_multiset<Key, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::operator=(std::initializer_list<value_type> ilist)
	{
		base_type::clear();
		base_type::insert(ilist.begin(), ilist.end());
		return *this;
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline void fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	swap(this_type& x)
	{
		// Fixed containers use a special swap that can deal with excessively large buffers.
		eastl::fixed_swap(*this, x);
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline void fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	reset_lose_memory()
	{
		base_type::reset_lose_memory();
		base_type::get_allocator().reset(mNodeBuffer);
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline typename fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::size_type 
	fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::max_size() const
	{
		return kMaxSize;
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline const typename fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::overflow_allocator_type& 
	fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::get_overflow_allocator() const EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline typename fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::overflow_allocator_type& 
	fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::get_overflow_allocator() EA_NOEXCEPT
	{
		return mAllocator.get_overflow_allocator();
	}


	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode, typename OverflowAllocator>
	inline void fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode, OverflowAllocator>::
	set_overflow_allocator(const overflow_allocator_type& allocator)
	{
		mAllocator.set_overflow_allocator(allocator);
	}


	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename Value, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode>
	inline void swap(fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode>& a, 
					 fixed_hash_multiset<Value, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode>& b)
	{
		// Fixed containers use a special swap that can deal with excessively large buffers.
		eastl::fixed_swap(a, b);
	}


} // namespace eastl

EA_RESTORE_VC_WARNING()

#endif // Header include guard












