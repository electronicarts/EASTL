/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements a hashtable, much like the C++11 unordered_set/unordered_map.
// proposed classes.
// The primary distinctions between this hashtable and C++11 unordered containers are:
//    - hashtable is savvy to an environment that doesn't have exception handling,
//      as is sometimes the case with console or embedded environments.
//    - hashtable is slightly more space-efficient than a conventional std hashtable 
//      implementation on platforms with 64 bit size_t.  This is 
//      because std STL uses size_t (64 bits) in data structures whereby 32 bits 
//      of data would be fine.
//    - hashtable can contain objects with alignment requirements. TR1 hash tables 
//      cannot do so without a bit of tedious non-portable effort.
//    - hashtable supports debug memory naming natively.
//    - hashtable provides a find function that lets you specify a type that is 
//      different from the hash table key type. This is particularly useful for 
//      the storing of string objects but finding them by char pointers.
//    - hashtable provides a lower level insert function which lets the caller 
//      specify the hash code and optionally the node instance.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_HASHTABLE_H
#define EASTL_INTERNAL_HASHTABLE_H


#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#include <EASTL/internal/config.h>
#include <EASTL/type_traits.h>
#include <EASTL/allocator.h>
#include <EASTL/iterator.h>
#include <EASTL/functional.h>
#include <EASTL/utility.h>
#include <EASTL/algorithm.h>
#include <EASTL/initializer_list.h>
#include <EASTL/tuple.h>
#include <string.h>

EA_DISABLE_ALL_VC_WARNINGS()
	#include <new>
	#include <stddef.h>
EA_RESTORE_ALL_VC_WARNINGS()

// 4512/4626 - 'class' : assignment operator could not be generated.
// 4530 - C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
// 4571 - catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught.
EA_DISABLE_VC_WARNING(4512 4626 4530 4571);


namespace eastl
{

	/// EASTL_HASHTABLE_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_HASHTABLE_DEFAULT_NAME
		#define EASTL_HASHTABLE_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " hashtable" // Unless the user overrides something, this is "EASTL hashtable".
	#endif


	/// EASTL_HASHTABLE_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_HASHTABLE_DEFAULT_ALLOCATOR
		#define EASTL_HASHTABLE_DEFAULT_ALLOCATOR allocator_type(EASTL_HASHTABLE_DEFAULT_NAME)
	#endif

	
	/// kHashtableAllocFlagBuckets
	/// Flag to allocator which indicates that we are allocating buckets and not nodes.
	enum { kHashtableAllocFlagBuckets = 0x00400000 };


	/// gpEmptyBucketArray
	///
	/// A shared representation of an empty hash table. This is present so that
	/// a new empty hashtable allocates no memory. It has two entries, one for 
	/// the first lone empty (NULL) bucket, and one for the non-NULL trailing sentinel.
	/// 
	extern EASTL_API void* gpEmptyBucketArray[2];


	/// EASTL_MACRO_SWAP
	///
	/// Use EASTL_MACRO_SWAP because GCC (at least v4.6-4.8) has a bug where it fails to compile eastl::swap(mpBucketArray, x.mpBucketArray).
	///
	#define EASTL_MACRO_SWAP(Type, a, b) \
		{ Type temp = a; a = b; b = temp; }


	/// hash_node
	///
	/// A hash_node stores an element in a hash table, much like a 
	/// linked list node stores an element in a linked list. 
	/// A hash_node additionally can, via template parameter,
	/// store a hash code in the node to speed up hash calculations 
	/// and comparisons in some cases.
	/// 
	template <typename Value, bool bCacheHashCode>
	struct hash_node;

	EA_DISABLE_VC_WARNING(4625 4626) // "copy constructor / assignment operator could not be generated because a base class copy constructor is inaccessible or deleted"
	#ifdef EA_COMPILER_MSVC_2015
		EA_DISABLE_VC_WARNING(5026) // disable warning: "move constructor was implicitly defined as deleted"
	#endif
		template <typename Value>
		struct hash_node<Value, true>
		{
			hash_node() = default;
			hash_node(const hash_node&) = default;
			hash_node(hash_node&&) = default;

			Value        mValue;
			hash_node*   mpNext;
			eastl_size_t mnHashCode;      // See config.h for the definition of eastl_size_t, which defaults to size_t.
		} EASTL_MAY_ALIAS;

		template <typename Value>
		struct hash_node<Value, false>
		{
			hash_node() = default;
			hash_node(const hash_node&) = default;
			hash_node(hash_node&&) = default;

		    Value      mValue;
			hash_node* mpNext;
		} EASTL_MAY_ALIAS;

	#ifdef EA_COMPILER_MSVC_2015
		EA_RESTORE_VC_WARNING()
	#endif
	EA_RESTORE_VC_WARNING()


	// has_hashcode_member
	//
	// Custom type-trait that checks for the existence of a class data member 'mnHashCode'.  
	//
	// In order to explicitly instantiate the hashtable without error we need to SFINAE away the functions that will
	// fail to compile based on if the 'hash_node' contains a 'mnHashCode' member dictated by the hashtable template
	// parameters. The hashtable support this level of configuration to allow users to choose which between the space vs.
	// time optimization.
	//
	namespace Internal
	{
		template <class T>
		struct has_hashcode_member 
		{
		private:
			template <class U> static eastl::no_type test(...);
			template <class U> static eastl::yes_type test(decltype(U::mnHashCode)* = 0);
		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(eastl::yes_type);
		};
	}
	
	static_assert(Internal::has_hashcode_member<hash_node<int, true>>::value, "contains a mnHashCode member");
	static_assert(!Internal::has_hashcode_member<hash_node<int, false>>::value, "doesn't contain a mnHashCode member");

	// convenience macros to increase the readability of the code paths that must SFINAE on if the 'hash_node'
	// contains the cached hashed value or not. 
	#define ENABLE_IF_HAS_HASHCODE(T, RT) typename eastl::enable_if<Internal::has_hashcode_member<T>::value, RT>::type*
	#define ENABLE_IF_HASHCODE_EASTLSIZET(T, RT) typename eastl::enable_if<eastl::is_convertible<T, eastl_size_t>::value, RT>::type
	#define ENABLE_IF_TRUETYPE(T) typename eastl::enable_if<T::value>::type*
	#define DISABLE_IF_TRUETYPE(T) typename eastl::enable_if<!T::value>::type*


	/// node_iterator_base
	///
	/// Node iterators iterate nodes within a given bucket.
	///
	/// We define a base class here because it is shared by both const and
	/// non-const iterators.
	///
	template <typename Value, bool bCacheHashCode>
	struct node_iterator_base
	{
		typedef hash_node<Value, bCacheHashCode> node_type;

		node_type* mpNode;

		node_iterator_base(node_type* pNode)
			: mpNode(pNode) { }

		void increment()
			{ mpNode = mpNode->mpNext; }
	};



	/// node_iterator
	///
	/// Node iterators iterate nodes within a given bucket.
	///
	/// The bConst parameter defines if the iterator is a const_iterator
	/// or an iterator.
	///
	template <typename Value, bool bConst, bool bCacheHashCode>
	struct node_iterator : public node_iterator_base<Value, bCacheHashCode>
	{
	public:
		typedef node_iterator_base<Value, bCacheHashCode>                base_type;
		typedef node_iterator<Value, bConst, bCacheHashCode>             this_type;
		typedef typename base_type::node_type                            node_type;
		typedef Value                                                    value_type;
		typedef typename conditional<bConst, const Value*, Value*>::type pointer;
		typedef typename conditional<bConst, const Value&, Value&>::type reference;
		typedef ptrdiff_t                                                difference_type;
		typedef EASTL_ITC_NS::forward_iterator_tag                       iterator_category;

	public:
		explicit node_iterator(node_type* pNode = NULL)
			: base_type(pNode) { }

		node_iterator(const node_iterator<Value, true, bCacheHashCode>& x)
			: base_type(x.mpNode) { }

		reference operator*() const
			{ return base_type::mpNode->mValue; }

		pointer operator->() const
			{ return &(base_type::mpNode->mValue); }

		node_iterator& operator++()
			{ base_type::increment(); return *this; }

		node_iterator operator++(int)
			{ node_iterator temp(*this); base_type::increment(); return temp; }

	}; // node_iterator



	/// hashtable_iterator_base
	///
	/// A hashtable_iterator iterates the entire hash table and not just
	/// nodes within a single bucket. Users in general will use a hash
	/// table iterator much more often, as it is much like other container
	/// iterators (e.g. vector::iterator).
	///
	/// We define a base class here because it is shared by both const and
	/// non-const iterators.
	///
	template <typename Value, bool bCacheHashCode>
	struct hashtable_iterator_base
	{
	public:
		typedef hashtable_iterator_base<Value, bCacheHashCode> this_type;
		typedef hash_node<Value, bCacheHashCode>               node_type;

	protected:
		template <typename, typename, typename, typename, typename, typename, typename, typename, typename, bool, bool, bool>
		friend class hashtable;

		template <typename, bool, bool>
		friend struct hashtable_iterator;

		template <typename V, bool b>
		friend bool operator==(const hashtable_iterator_base<V, b>&, const hashtable_iterator_base<V, b>&);

		template <typename V, bool b>
		friend bool operator!=(const hashtable_iterator_base<V, b>&, const hashtable_iterator_base<V, b>&);

		node_type*  mpNode;      // Current node within current bucket.
		node_type** mpBucket;    // Current bucket.

	public:
		hashtable_iterator_base(node_type* pNode, node_type** pBucket)
			: mpNode(pNode), mpBucket(pBucket) { }

		void increment_bucket()
		{
			++mpBucket;
			while(*mpBucket == NULL) // We store an extra bucket with some non-NULL value at the end 
				++mpBucket;          // of the bucket array so that finding the end of the bucket
			mpNode = *mpBucket;      // array is quick and simple.
		}

		void increment()
		{
			mpNode = mpNode->mpNext;

			while(mpNode == NULL)
				mpNode = *++mpBucket;
		}

	}; // hashtable_iterator_base




	/// hashtable_iterator
	///
	/// A hashtable_iterator iterates the entire hash table and not just
	/// nodes within a single bucket. Users in general will use a hash
	/// table iterator much more often, as it is much like other container
	/// iterators (e.g. vector::iterator).
	///
	/// The bConst parameter defines if the iterator is a const_iterator
	/// or an iterator.
	///
	template <typename Value, bool bConst, bool bCacheHashCode>
	struct hashtable_iterator : public hashtable_iterator_base<Value, bCacheHashCode>
	{
	public:
		typedef hashtable_iterator_base<Value, bCacheHashCode>           base_type;
		typedef hashtable_iterator<Value, bConst, bCacheHashCode>        this_type;
		typedef hashtable_iterator<Value, false, bCacheHashCode>         this_type_non_const;
		typedef typename base_type::node_type                            node_type;
		typedef Value                                                    value_type;
		typedef typename conditional<bConst, const Value*, Value*>::type pointer;
		typedef typename conditional<bConst, const Value&, Value&>::type reference;
		typedef ptrdiff_t                                                difference_type;
		typedef EASTL_ITC_NS::forward_iterator_tag                       iterator_category;

	public:
		hashtable_iterator(node_type* pNode = NULL, node_type** pBucket = NULL)
			: base_type(pNode, pBucket) { }

		hashtable_iterator(node_type** pBucket)
			: base_type(*pBucket, pBucket) { }

		template <bool IsConst = bConst, typename enable_if<IsConst, int>::type = 0>
		hashtable_iterator(const this_type_non_const& x)
			: base_type(x.mpNode, x.mpBucket) { }

		hashtable_iterator(const hashtable_iterator&) = default;
		hashtable_iterator(hashtable_iterator&&) = default;
		hashtable_iterator& operator=(const hashtable_iterator&) = default;
		hashtable_iterator& operator=(hashtable_iterator&&) = default;

		reference operator*() const
			{ return base_type::mpNode->mValue; }

		pointer operator->() const
			{ return &(base_type::mpNode->mValue); }

		hashtable_iterator& operator++()
			{ base_type::increment(); return *this; }

		hashtable_iterator operator++(int)
			{ hashtable_iterator temp(*this); base_type::increment(); return temp; }

		const node_type* get_node() const
			{ return base_type::mpNode; }

	}; // hashtable_iterator




	/// ht_distance
	///
	/// This function returns the same thing as distance() for 
	/// forward iterators but returns zero for input iterators.
	/// The reason why is that input iterators can only be read
	/// once, and calling distance() on an input iterator destroys
	/// the ability to read it. This ht_distance is used only for
	/// optimization and so the code will merely work better with
	/// forward iterators that input iterators.
	///
	template <typename Iterator>
	inline typename eastl::iterator_traits<Iterator>::difference_type
	distance_fw_impl(Iterator /*first*/, Iterator /*last*/, EASTL_ITC_NS::input_iterator_tag)
	{
		return 0;
	}

	template <typename Iterator>
	inline typename eastl::iterator_traits<Iterator>::difference_type
	distance_fw_impl(Iterator first, Iterator last, EASTL_ITC_NS::forward_iterator_tag)
		{ return eastl::distance(first, last); }

	template <typename Iterator>
	inline typename eastl::iterator_traits<Iterator>::difference_type
	ht_distance(Iterator first, Iterator last)
	{
		typedef typename eastl::iterator_traits<Iterator>::iterator_category IC;
		return distance_fw_impl(first, last, IC());
	}




	/// mod_range_hashing
	///
	/// Implements the algorithm for conversion of a number in the range of
	/// [0, SIZE_T_MAX] to the range of [0, BucketCount).
	///
	struct mod_range_hashing
	{
		uint32_t operator()(size_t r, uint32_t n) const
			{ return r % n; }
	};


	/// default_ranged_hash
	///
	/// Default ranged hash function H. In principle it should be a
	/// function object composed from objects of type H1 and H2 such that
	/// h(k, n) = h2(h1(k), n), but that would mean making extra copies of
	/// h1 and h2. So instead we'll just use a tag to tell class template
	/// hashtable to do that composition.
	///
	struct default_ranged_hash{ };


	/// prime_rehash_policy
	///
	/// Default value for rehash policy. Bucket size is (usually) the
	/// smallest prime that keeps the load factor small enough.
	///
	struct EASTL_API prime_rehash_policy
	{
	public:
		float            mfMaxLoadFactor;
		float            mfGrowthFactor;
		mutable uint32_t mnNextResize;

	public:
		prime_rehash_policy(float fMaxLoadFactor = 1.f)
			: mfMaxLoadFactor(fMaxLoadFactor), mfGrowthFactor(2.f), mnNextResize(0) { }

		float GetMaxLoadFactor() const
			{ return mfMaxLoadFactor; }

		/// Return a bucket count no greater than nBucketCountHint, 
		/// Don't update member variables while at it.
		static uint32_t GetPrevBucketCountOnly(uint32_t nBucketCountHint);

		/// Return a bucket count no greater than nBucketCountHint.
		/// This function has a side effect of updating mnNextResize.
		uint32_t GetPrevBucketCount(uint32_t nBucketCountHint) const;

		/// Return a bucket count no smaller than nBucketCountHint.
		/// This function has a side effect of updating mnNextResize.
		uint32_t GetNextBucketCount(uint32_t nBucketCountHint) const;

		/// Return a bucket count appropriate for nElementCount elements.
		/// This function has a side effect of updating mnNextResize.
		uint32_t GetBucketCount(uint32_t nElementCount) const;

		/// nBucketCount is current bucket count, nElementCount is current element count,
		/// and nElementAdd is number of elements to be inserted. Do we need 
		/// to increase bucket count? If so, return pair(true, n), where 
		/// n is the new bucket count. If not, return pair(false, 0).
		eastl::pair<bool, uint32_t>
		GetRehashRequired(uint32_t nBucketCount, uint32_t nElementCount, uint32_t nElementAdd) const;
	};





	///////////////////////////////////////////////////////////////////////
	// Base classes for hashtable. We define these base classes because 
	// in some cases we want to do different things depending on the 
	// value of a policy class. In some cases the policy class affects
	// which member functions and nested typedefs are defined; we handle that
	// by specializing base class templates. Several of the base class templates
	// need to access other members of class template hashtable, so we use
	// the "curiously recurring template pattern" (parent class is templated 
	// on type of child class) for them.
	///////////////////////////////////////////////////////////////////////


	/// rehash_base
	///
	/// Give hashtable the get_max_load_factor functions if the rehash 
	/// policy is prime_rehash_policy.
	///
	template <typename RehashPolicy, typename Hashtable>
	struct rehash_base { };

	template <typename Hashtable>
	struct rehash_base<prime_rehash_policy, Hashtable>
	{
		// Returns the max load factor, which is the load factor beyond
		// which we rebuild the container with a new bucket count.
		float get_max_load_factor() const
		{
			const Hashtable* const pThis = static_cast<const Hashtable*>(this);
			return pThis->rehash_policy().GetMaxLoadFactor();
		}

		// If you want to make the hashtable never rehash (resize), 
		// set the max load factor to be a very high number (e.g. 100000.f).
		void set_max_load_factor(float fMaxLoadFactor)
		{
			Hashtable* const pThis = static_cast<Hashtable*>(this);
			pThis->rehash_policy(prime_rehash_policy(fMaxLoadFactor));    
		}
	};




	/// hash_code_base
	///
	/// Encapsulates two policy issues that aren't quite orthogonal.
	///   (1) The difference between using a ranged hash function and using
	///       the combination of a hash function and a range-hashing function.
	///       In the former case we don't have such things as hash codes, so
	///       we have a dummy type as placeholder.
	///   (2) Whether or not we cache hash codes. Caching hash codes is
	///       meaningless if we have a ranged hash function. This is because
	///       a ranged hash function converts an object directly to its
	///       bucket index without ostensibly using a hash code.
	/// We also put the key extraction and equality comparison function 
	/// objects here, for convenience.
	///
	template <typename Key, typename Value, typename ExtractKey, typename Equal, 
			  typename H1, typename H2, typename H, bool bCacheHashCode>
	struct hash_code_base;


	/// hash_code_base
	///
	/// Specialization: ranged hash function, no caching hash codes. 
	/// H1 and H2 are provided but ignored. We define a dummy hash code type.
	///
	template <typename Key, typename Value, typename ExtractKey, typename Equal, typename H1, typename H2, typename H>
	struct hash_code_base<Key, Value, ExtractKey, Equal, H1, H2, H, false>
	{
	protected:
		ExtractKey  mExtractKey;    // To do: Make this member go away entirely, as it never has any data.
		Equal       mEqual;         // To do: Make this instance use zero space when it is zero size.
		H           mRangedHash;    // To do: Make this instance use zero space when it is zero size

	public:
		H1 hash_function() const
			{ return H1(); }

		EASTL_REMOVE_AT_2024_APRIL Equal equal_function() const // Deprecated. Use key_eq() instead, as key_eq is what the new C++ standard 
			{ return mEqual; }					   // has specified in its hashtable (unordered_*) proposal.

		const Equal& key_eq() const
			{ return mEqual; }

		Equal& key_eq()
			{ return mEqual; }

	protected:
		typedef void*    hash_code_t;
		typedef uint32_t bucket_index_t;

		hash_code_base(const ExtractKey& extractKey, const Equal& eq, const H1&, const H2&, const H& h)
			: mExtractKey(extractKey), mEqual(eq), mRangedHash(h) { }

		hash_code_t get_hash_code(const Key& key) const
		{
			EA_UNUSED(key);
			return NULL;
		}

		bucket_index_t bucket_index(hash_code_t, uint32_t) const
			{ return (bucket_index_t)0; }

		bucket_index_t bucket_index(const Key& key, hash_code_t, uint32_t nBucketCount) const
			{ return (bucket_index_t)mRangedHash(key, nBucketCount); }

		bucket_index_t bucket_index(const hash_node<Value, false>* pNode, uint32_t nBucketCount) const
			{ return (bucket_index_t)mRangedHash(mExtractKey(pNode->mValue), nBucketCount); }

		bool compare(const Key& key, hash_code_t, hash_node<Value, false>* pNode) const
			{ return mEqual(key, mExtractKey(pNode->mValue)); }

		void copy_code(hash_node<Value, false>*, const hash_node<Value, false>*) const
			{ } // Nothing to do.

		void set_code(hash_node<Value, false>* pDest, hash_code_t c) const
		{
			EA_UNUSED(pDest);
			EA_UNUSED(c);
		}

		void base_swap(hash_code_base& x)
		{
			eastl::swap(mExtractKey, x.mExtractKey);
			eastl::swap(mEqual,      x.mEqual);
			eastl::swap(mRangedHash, x.mRangedHash);
		}

	}; // hash_code_base



	// No specialization for ranged hash function while caching hash codes.
	// That combination is meaningless, and trying to do it is an error.


	/// hash_code_base
	///
	/// Specialization: ranged hash function, cache hash codes. 
	/// This combination is meaningless, so we provide only a declaration
	/// and no definition.
	///
	template <typename Key, typename Value, typename ExtractKey, typename Equal, typename H1, typename H2, typename H>
	struct hash_code_base<Key, Value, ExtractKey, Equal, H1, H2, H, true>;



	/// hash_code_base
	///
	/// Specialization: hash function and range-hashing function, 
	/// no caching of hash codes. H is provided but ignored. 
	/// Provides typedef and accessor required by TR1.
	///
	template <typename Key, typename Value, typename ExtractKey, typename Equal, typename H1, typename H2>
	struct hash_code_base<Key, Value, ExtractKey, Equal, H1, H2, default_ranged_hash, false>
	{
	protected:
		ExtractKey  mExtractKey;
		Equal       mEqual;
		H1          m_h1;
		H2          m_h2;

	public:
		typedef H1 hasher;

		H1 hash_function() const
			{ return m_h1; }

		EASTL_REMOVE_AT_2024_APRIL Equal equal_function() const // Deprecated. Use key_eq() instead, as key_eq is what the new C++ standard 
			{ return mEqual; }					   // has specified in its hashtable (unordered_*) proposal.

		const Equal& key_eq() const
			{ return mEqual; }

		Equal& key_eq()
			{ return mEqual; }

	protected:
		typedef size_t hash_code_t;
		typedef uint32_t bucket_index_t;
		typedef hash_node<Value, false> node_type;

		hash_code_base(const ExtractKey& ex, const Equal& eq, const H1& h1, const H2& h2, const default_ranged_hash&)
			: mExtractKey(ex), mEqual(eq), m_h1(h1), m_h2(h2) { }

		hash_code_t get_hash_code(const Key& key) const
			{ return (hash_code_t)m_h1(key); }

		bucket_index_t bucket_index(hash_code_t c, uint32_t nBucketCount) const
			{ return (bucket_index_t)m_h2(c, nBucketCount); }

		bucket_index_t bucket_index(const Key&, hash_code_t c, uint32_t nBucketCount) const
			{ return (bucket_index_t)m_h2(c, nBucketCount); }

		bucket_index_t bucket_index(const node_type* pNode, uint32_t nBucketCount) const
			{ return (bucket_index_t)m_h2((hash_code_t)m_h1(mExtractKey(pNode->mValue)), nBucketCount); }

		bool compare(const Key& key, hash_code_t, node_type* pNode) const
			{ return mEqual(key, mExtractKey(pNode->mValue)); }

		void copy_code(node_type*, const node_type*) const
			{ } // Nothing to do.

		void set_code(node_type*, hash_code_t) const
			{ } // Nothing to do.

		void base_swap(hash_code_base& x)
		{
			eastl::swap(mExtractKey, x.mExtractKey);
			eastl::swap(mEqual,      x.mEqual);
			eastl::swap(m_h1,        x.m_h1);
			eastl::swap(m_h2,        x.m_h2);
		}

	}; // hash_code_base



	/// hash_code_base
	///
	/// Specialization: hash function and range-hashing function, 
	/// caching hash codes. H is provided but ignored. 
	/// Provides typedef and accessor required by TR1.
	///
	template <typename Key, typename Value, typename ExtractKey, typename Equal, typename H1, typename H2>
	struct hash_code_base<Key, Value, ExtractKey, Equal, H1, H2, default_ranged_hash, true>
	{
	protected:
		ExtractKey  mExtractKey;
		Equal       mEqual;
		H1          m_h1;
		H2          m_h2;

	public:
		typedef H1 hasher;

		H1 hash_function() const
			{ return m_h1; }

		EASTL_REMOVE_AT_2024_APRIL Equal equal_function() const // Deprecated. Use key_eq() instead, as key_eq is what the new C++ standard 
			{ return mEqual; }					   // has specified in its hashtable (unordered_*) proposal.

		const Equal& key_eq() const
			{ return mEqual; }

		Equal& key_eq()
			{ return mEqual; }

	protected:
		typedef uint32_t hash_code_t;
		typedef uint32_t bucket_index_t;
		typedef hash_node<Value, true> node_type;

		hash_code_base(const ExtractKey& ex, const Equal& eq, const H1& h1, const H2& h2, const default_ranged_hash&)
			: mExtractKey(ex), mEqual(eq), m_h1(h1), m_h2(h2) { }

		hash_code_t get_hash_code(const Key& key) const
			{ return (hash_code_t)m_h1(key); }

		bucket_index_t bucket_index(hash_code_t c, uint32_t nBucketCount) const
			{ return (bucket_index_t)m_h2(c, nBucketCount); }

		bucket_index_t bucket_index(const Key&, hash_code_t c, uint32_t nBucketCount) const
			{ return (bucket_index_t)m_h2(c, nBucketCount); }

		bucket_index_t bucket_index(const node_type* pNode, uint32_t nBucketCount) const
			{ return (bucket_index_t)m_h2((uint32_t)pNode->mnHashCode, nBucketCount); }

		bool compare(const Key& key, hash_code_t c, node_type* pNode) const
			{ return (pNode->mnHashCode == c) && mEqual(key, mExtractKey(pNode->mValue)); }

		void copy_code(node_type* pDest, const node_type* pSource) const
			{ pDest->mnHashCode = pSource->mnHashCode; }

		void set_code(node_type* pDest, hash_code_t c) const
			{ pDest->mnHashCode = c; }

		void base_swap(hash_code_base& x)
		{
			eastl::swap(mExtractKey, x.mExtractKey);
			eastl::swap(mEqual,      x.mEqual);
			eastl::swap(m_h1,        x.m_h1);
			eastl::swap(m_h2,        x.m_h2);
		}

	}; // hash_code_base





	///////////////////////////////////////////////////////////////////////////
	/// hashtable
	///
	/// Key and Value: arbitrary CopyConstructible types.
	///
	/// ExtractKey: function object that takes a object of type Value
	/// and returns a value of type Key.
	///
	/// Equal: function object that takes two objects of type k and returns
	/// a bool-like value that is true if the two objects are considered equal.
	///
	/// H1: a hash function. A unary function object with argument type
	/// Key and result type size_t. Return values should be distributed
	/// over the entire range [0, numeric_limits<uint32_t>::max()].
	///
	/// H2: a range-hashing function (in the terminology of Tavori and
	/// Dreizin). This is a function which takes the output of H1 and 
	/// converts it to the range of [0, n]. Usually it merely takes the
	/// output of H1 and mods it to n.
	///
	/// H: a ranged hash function (Tavori and Dreizin). This is merely
	/// a class that combines the functionality of H1 and H2 together, 
	/// possibly in some way that is somehow improved over H1 and H2
	/// It is a binary function whose argument types are Key and size_t 
	/// and whose result type is uint32_t. Given arguments k and n, the 
	/// return value is in the range [0, n). Default: h(k, n) = h2(h1(k), n). 
	/// If H is anything other than the default, H1 and H2 are ignored, 
	/// as H is thus overriding H1 and H2.
	///
	/// RehashPolicy: Policy class with three members, all of which govern
	/// the bucket count. nBucket(n) returns a bucket count no smaller
	/// than n. GetBucketCount(n) returns a bucket count appropriate
	/// for an element count of n. GetRehashRequired(nBucketCount, nElementCount, nElementAdd)
	/// determines whether, if the current bucket count is nBucket and the
	/// current element count is nElementCount, we need to increase the bucket
	/// count. If so, returns pair(true, n), where n is the new
	/// bucket count. If not, returns pair(false, <anything>).
	///
	/// Currently it is hard-wired that the number of buckets never
	/// shrinks. Should we allow RehashPolicy to change that?
	///
	/// bCacheHashCode: true if we store the value of the hash
	/// function along with the value. This is a time-space tradeoff.
	/// Storing it may improve lookup speed by reducing the number of 
	/// times we need to call the Equal function.
	///
	/// bMutableIterators: true if hashtable::iterator is a mutable
	/// iterator, false if iterator and const_iterator are both const 
	/// iterators. This is true for hash_map and hash_multimap,
	/// false for hash_set and hash_multiset.
	///
	/// bUniqueKeys: true if the return value of hashtable::count(k)
	/// is always at most one, false if it may be an arbitrary number. 
	/// This is true for hash_set and hash_map and is false for 
	/// hash_multiset and hash_multimap.
	///
	///////////////////////////////////////////////////////////////////////
	/// Note:
	/// If you want to make a hashtable never increase its bucket usage,
	/// call set_max_load_factor with a very high value such as 100000.f.
	///
	/// find_as
	/// In order to support the ability to have a hashtable of strings but
	/// be able to do efficiently lookups via char pointers (i.e. so they 
	/// aren't converted to string objects), we provide the find_as 
	/// function. This function allows you to do a find with a key of a
	/// type other than the hashtable key type. See the find_as function
	/// for more documentation on this.
	///
	/// find_by_hash
	/// In the interest of supporting fast operations wherever possible,
	/// we provide a find_by_hash function which finds a node using its
	/// hash code.  This is useful for cases where the node's hash is
	/// already known, allowing us to avoid a redundant hash operation
	/// in the normal find path.
	/// 
	template <typename Key, typename Value, typename Allocator, typename ExtractKey, 
			  typename Equal, typename H1, typename H2, typename H, 
			  typename RehashPolicy, bool bCacheHashCode, bool bMutableIterators, bool bUniqueKeys>
	class hashtable
		:   public rehash_base<RehashPolicy, hashtable<Key, Value, Allocator, ExtractKey, Equal, H1, H2, H, RehashPolicy, bCacheHashCode, bMutableIterators, bUniqueKeys> >,
			public hash_code_base<Key, Value, ExtractKey, Equal, H1, H2, H, bCacheHashCode>
	{
	public:
		typedef Key                                                                                 key_type;
		typedef Value                                                                               value_type;
		typedef typename ExtractKey::result_type                                                    mapped_type;
		typedef hash_code_base<Key, Value, ExtractKey, Equal, H1, H2, H, bCacheHashCode>            hash_code_base_type;
		typedef typename hash_code_base_type::hash_code_t                                           hash_code_t;
		typedef Allocator                                                                           allocator_type;
		typedef Equal                                                                               key_equal;
		typedef ptrdiff_t                                                                           difference_type;
		typedef eastl_size_t                                                                        size_type;     // See config.h for the definition of eastl_size_t, which defaults to size_t.
		typedef value_type&                                                                         reference;
		typedef const value_type&                                                                   const_reference;
		typedef node_iterator<value_type, !bMutableIterators, bCacheHashCode>                       local_iterator;
		typedef node_iterator<value_type, true,               bCacheHashCode>                       const_local_iterator;
		typedef hashtable_iterator<value_type, !bMutableIterators, bCacheHashCode>                  iterator;
		typedef hashtable_iterator<value_type, true,               bCacheHashCode>                  const_iterator;
		typedef hash_node<value_type, bCacheHashCode>                                               node_type;
		typedef typename conditional<bUniqueKeys, eastl::pair<iterator, bool>, iterator>::type      insert_return_type;
		typedef hashtable<Key, Value, Allocator, ExtractKey, Equal, H1, H2, H, 
							RehashPolicy, bCacheHashCode, bMutableIterators, bUniqueKeys>           this_type;
		typedef RehashPolicy                                                                        rehash_policy_type;
		typedef ExtractKey                                                                          extract_key_type;
		typedef H1                                                                                  h1_type;
		typedef H2                                                                                  h2_type;
		typedef H                                                                                   h_type;
		typedef integral_constant<bool, bUniqueKeys>                                                has_unique_keys_type;

		using hash_code_base_type::key_eq;
		using hash_code_base_type::hash_function;
		using hash_code_base_type::mExtractKey;
		using hash_code_base_type::get_hash_code;
		using hash_code_base_type::bucket_index;
		using hash_code_base_type::compare;
		using hash_code_base_type::set_code;
		using hash_code_base_type::copy_code;

		static const bool kCacheHashCode = bCacheHashCode;

		enum
		{
			// This enumeration is deprecated in favor of eastl::kHashtableAllocFlagBuckets.
			kAllocFlagBuckets EASTL_REMOVE_AT_2024_APRIL = eastl::kHashtableAllocFlagBuckets                  // Flag to allocator which indicates that we are allocating buckets and not nodes.
		};

	protected:
		node_type**     mpBucketArray;
		size_type       mnBucketCount;
		size_type       mnElementCount;
		RehashPolicy    mRehashPolicy;  // To do: Use base class optimization to make this go away.
		allocator_type  mAllocator;     // To do: Use base class optimization to make this go away.

		struct NodeFindKeyData {
			node_type* node;
			hash_code_t code;
			size_type bucket_index;
		};

	public:
		hashtable(size_type nBucketCount, const H1&, const H2&, const H&, const Equal&, const ExtractKey&, 
				  const allocator_type& allocator = EASTL_HASHTABLE_DEFAULT_ALLOCATOR);
		
		// note: standard only requires InputIterator.
		template <typename FowardIterator>
		hashtable(FowardIterator first, FowardIterator last, size_type nBucketCount, 
				  const H1&, const H2&, const H&, const Equal&, const ExtractKey&, 
				  const allocator_type& allocator = EASTL_HASHTABLE_DEFAULT_ALLOCATOR); 
		
		hashtable(const hashtable& x);

		// initializer_list ctor support is implemented in subclasses (e.g. hash_set).
		// hashtable(initializer_list<value_type>, size_type nBucketCount, const H1&, const H2&, const H&, 
		//           const Equal&, const ExtractKey&, const allocator_type& allocator = EASTL_HASHTABLE_DEFAULT_ALLOCATOR);

		hashtable(this_type&& x);
		hashtable(this_type&& x, const allocator_type& allocator);
	   ~hashtable();

		const allocator_type& get_allocator() const EA_NOEXCEPT;
		allocator_type&       get_allocator() EA_NOEXCEPT;
		void                  set_allocator(const allocator_type& allocator);

		this_type& operator=(const this_type& x);
		this_type& operator=(std::initializer_list<value_type> ilist);
		this_type& operator=(this_type&& x);

		void swap(this_type& x);

		iterator begin() EA_NOEXCEPT
		{
			iterator i(mpBucketArray);
			if(!i.mpNode)
				i.increment_bucket();
			return i;
		}

		const_iterator begin() const EA_NOEXCEPT
		{
			const_iterator i(mpBucketArray);
			if(!i.mpNode)
				i.increment_bucket();
			return i;
		}

		const_iterator cbegin() const EA_NOEXCEPT
			{ return begin(); }

		iterator end() EA_NOEXCEPT
			{ return iterator(mpBucketArray + mnBucketCount); }

		const_iterator end() const EA_NOEXCEPT
			{ return const_iterator(mpBucketArray + mnBucketCount); }

		const_iterator cend() const EA_NOEXCEPT
			{ return const_iterator(mpBucketArray + mnBucketCount); }

		// Returns an iterator to the first item in bucket n.
		local_iterator begin(size_type n) EA_NOEXCEPT
			{ return local_iterator(mpBucketArray[n]); }

		const_local_iterator begin(size_type n) const EA_NOEXCEPT
			{ return const_local_iterator(mpBucketArray[n]); }

		const_local_iterator cbegin(size_type n) const EA_NOEXCEPT
			{ return const_local_iterator(mpBucketArray[n]); }

		// Returns an iterator to the last item in a bucket returned by begin(n).
		local_iterator end(size_type) EA_NOEXCEPT
			{ return local_iterator(NULL); }

		const_local_iterator end(size_type) const EA_NOEXCEPT
			{ return const_local_iterator(NULL); }

		const_local_iterator cend(size_type) const EA_NOEXCEPT
			{ return const_local_iterator(NULL); }

		bool empty() const EA_NOEXCEPT
			{ return mnElementCount == 0; }

		size_type size() const EA_NOEXCEPT
			{ return mnElementCount; }

		// size_type max_size() const EA_NOEXCEPT;

		size_type bucket_count() const EA_NOEXCEPT
			{ return mnBucketCount; }

		// size_type max_bucket_count() const;

		size_type bucket_size(size_type n) const EA_NOEXCEPT
			{ return (size_type)eastl::distance(begin(n), end(n)); }

		//size_type bucket(const key_type& k) const EA_NOEXCEPT
		//    { return bucket_index(k, (hash code here), (uint32_t)mnBucketCount); }

		// Returns the ratio of element count to bucket count. A return value of 1 means 
		// there's an optimal 1 bucket for each element.
		float load_factor() const EA_NOEXCEPT
			{ return (float)mnElementCount / (float)mnBucketCount; }

		// float max_load_factor() const;
		// void max_load_factor( float ml );

		// Inherited from the base class.
		// Returns the max load factor, which is the load factor beyond
		// which we rebuild the container with a new bucket count.
		// get_max_load_factor comes from rehash_base.
		//    float get_max_load_factor() const;

		// Inherited from the base class.
		// If you want to make the hashtable never rehash (resize), 
		// set the max load factor to be a very high number (e.g. 100000.f).
		// set_max_load_factor comes from rehash_base.
		//    void set_max_load_factor(float fMaxLoadFactor);

		/// Generalization of get_max_load_factor. This is an extension that's
		/// not present in C++ hash tables (unordered containers).
		const rehash_policy_type& rehash_policy() const EA_NOEXCEPT
			{ return mRehashPolicy; }

		/// Generalization of set_max_load_factor. This is an extension that's
		/// not present in C++ hash tables (unordered containers).
		void rehash_policy(const rehash_policy_type& rehashPolicy);

		template <class... Args>
		insert_return_type emplace(Args&&... args);

		template <class... Args>
		iterator emplace_hint(const_iterator position, Args&&... args);

		insert_return_type                     insert(const value_type& value);
		insert_return_type                     insert(value_type&& otherValue);
		// template<typename P>
		// insert_return_type					insert(P&& value); // sfinae: is_constructible<value_type, P&&>::value
		iterator                               insert(const_iterator hint, const value_type& value);
		iterator                               insert(const_iterator hint, value_type&& value);
		// template<typename P>
		// insert_return_type					insert(const_iterator hint, P&& value); // sfinae: is_constructible<value_type, P&&>::value
		void                                   insert(std::initializer_list<value_type> ilist);
		template <typename InputIterator> void insert(InputIterator first, InputIterator last);
	  //insert_return_type                     insert(node_type&& nh);
	  //iterator                               insert(const_iterator hint, node_type&& nh);

		// This overload attempts to mitigate the overhead associated with mismatched cv-quality elements of
		// the hashtable pair. It can avoid copy overhead because it will perfect forward the user provided pair types
		// until it can constructed in-place in the allocated hashtable node.  
		//
		// Ideally we would remove this overload as it deprecated and removed in C++17 but it currently causes
		// performance regressions for hashtables with complex keys (keys that allocate resources).
		EASTL_INTERNAL_DISABLE_DEPRECATED() // 'is_literal_type_v<P>': was declared deprecated
		template <class P,
		          class = typename eastl::enable_if_t<
					#if EASTL_ENABLE_PAIR_FIRST_ELEMENT_CONSTRUCTOR
		              !eastl::is_same_v<eastl::decay_t<P>, key_type> &&
					#endif
		              !eastl::is_literal_type_v<P> &&
		              eastl::is_constructible_v<value_type, P&&>>>
		insert_return_type insert(P&& otherValue);
		EASTL_INTERNAL_RESTORE_DEPRECATED()

		// Non-standard extension
		template <class P> // See comments below for the const value_type& equivalent to this function.
		insert_return_type insert(hash_code_t c, node_type* pNodeNew, P&& otherValue);

		// We provide a version of insert which lets the caller directly specify the hash value and 
		// a potential node to insert if needed. This allows for less thread contention in the case
		// of a thread-shared hash table that's accessed during a mutex lock, because the hash calculation
		// and node creation is done outside of the lock. If pNodeNew is supplied by the user (i.e. non-NULL) 
		// then it must be freeable via the hash table's allocator. If the return value is true then this function 
		// took over ownership of pNodeNew, else pNodeNew is still owned by the caller to free or to pass 
		// to another call to insert. pNodeNew need not be assigned the value by the caller, as the insert
		// function will assign value to pNodeNew upon insertion into the hash table. pNodeNew may be 
		// created by the user with the allocate_uninitialized_node function, and freed by the free_uninitialized_node function.
		insert_return_type insert(hash_code_t c, node_type* pNodeNew, const value_type& value);

		template <class M> eastl::pair<iterator, bool> insert_or_assign(const key_type& k, M&& obj);
		template <class M> eastl::pair<iterator, bool> insert_or_assign(key_type&& k, M&& obj);
		template <class M> iterator                    insert_or_assign(const_iterator hint, const key_type& k, M&& obj);
		template <class M> iterator                    insert_or_assign(const_iterator hint, key_type&& k, M&& obj);

		// Used to allocate and free memory used by insert(const value_type& value, hash_code_t c, node_type* pNodeNew).
		node_type* allocate_uninitialized_node();
		void       free_uninitialized_node(node_type* pNode);

		iterator         erase(const_iterator position);
		iterator         erase(const_iterator first, const_iterator last);
		size_type        erase(const key_type& k);

		void clear();
		void clear(bool clearBuckets);                  // If clearBuckets is true, we free the bucket memory and set the bucket count back to the newly constructed count.
		void reset_lose_memory() EA_NOEXCEPT;           // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.
		void rehash(size_type nBucketCount);
		void reserve(size_type nElementCount);

		iterator       find(const key_type& key);
		const_iterator find(const key_type& key) const;

		// missing transparent key support:
		// template<typename K>
		// iterator       find(const K& key);
		// template<typename K>
		// const_iterator find(const K& key) const;

		/// Implements a find whereby the user supplies a comparison of a different type
		/// than the hashtable value_type. A useful case of this is one whereby you have
		/// a container of string objects but want to do searches via passing in char pointers.
		/// The problem is that without this kind of find, you need to do the expensive operation
		/// of converting the char pointer to a string so it can be used as the argument to the 
		/// find function.
		///
		/// Example usage (namespaces omitted for brevity):
		///     hash_set<string> hashSet;
		///     hashSet.find_as("hello");    // Use default hash and compare.
		///
		/// Example usage (note that the predicate uses string as first type and char* as second):
		///     hash_set<string> hashSet;
		///     hashSet.find_as("hello", hash<char*>(), equal_to<>());
		///
		template <typename U, typename UHash, typename BinaryPredicate>
		iterator       find_as(const U& u, UHash uhash, BinaryPredicate predicate);

		template <typename U, typename UHash, typename BinaryPredicate>
		const_iterator find_as(const U& u, UHash uhash, BinaryPredicate predicate) const;

		template <typename U>
		iterator       find_as(const U& u);

		template <typename U>
		const_iterator find_as(const U& u) const;

		// Note: find_by_hash and find_range_by_hash both perform a search based on a hash value.
		// It is important to note that multiple hash values may map to the same hash bucket, so
		// it would be incorrect to assume all items returned match the hash value that
		// was searched for.

		/// Implements a find whereby the user supplies the node's hash code.
		/// It returns an iterator to the first element that matches the given hash. However, there may be multiple elements that match the given hash.

		template<typename HashCodeT>
		ENABLE_IF_HASHCODE_EASTLSIZET(HashCodeT, iterator) find_by_hash(HashCodeT c)
		{
			EASTL_CT_ASSERT_MSG(bCacheHashCode,
				"find_by_hash(hash_code_t c) is designed to avoid recomputing hashes, "
				"so it requires cached hash codes.  Consider setting template parameter "
				"bCacheHashCode to true or using find_by_hash(const key_type& k, hash_code_t c) instead.");

			const size_type n = (size_type)bucket_index(c, (uint32_t)mnBucketCount);

			node_type* const pNode = DoFindNode(mpBucketArray[n], c);

			return pNode ? iterator(pNode, mpBucketArray + n) :
						   iterator(mpBucketArray + mnBucketCount); // iterator(mpBucketArray + mnBucketCount) == end()
		}

		template<typename HashCodeT>
		ENABLE_IF_HASHCODE_EASTLSIZET(HashCodeT, const_iterator) find_by_hash(HashCodeT c) const
		{
			EASTL_CT_ASSERT_MSG(bCacheHashCode,
								"find_by_hash(hash_code_t c) is designed to avoid recomputing hashes, "
								"so it requires cached hash codes.  Consider setting template parameter "
								"bCacheHashCode to true or using find_by_hash(const key_type& k, hash_code_t c) instead.");

			const size_type n = (size_type)bucket_index(c, (uint32_t)mnBucketCount);

			node_type* const pNode = DoFindNode(mpBucketArray[n], c);

			return pNode ?
					   const_iterator(pNode, mpBucketArray + n) :
					   const_iterator(mpBucketArray + mnBucketCount); // iterator(mpBucketArray + mnBucketCount) == end()
		}

		iterator find_by_hash(const key_type& k, hash_code_t c)
		{
			const size_type n = (size_type)bucket_index(c, (uint32_t)mnBucketCount);

			node_type* const pNode = DoFindNode(mpBucketArray[n], k, c);
			return pNode ? iterator(pNode, mpBucketArray + n) : iterator(mpBucketArray + mnBucketCount); // iterator(mpBucketArray + mnBucketCount) == end()
		}

		const_iterator find_by_hash(const key_type& k, hash_code_t c) const
		{
			const size_type n = (size_type)bucket_index(c, (uint32_t)mnBucketCount);

			node_type* const pNode = DoFindNode(mpBucketArray[n], k, c);
			return pNode ? const_iterator(pNode, mpBucketArray + n) : const_iterator(mpBucketArray + mnBucketCount); // iterator(mpBucketArray + mnBucketCount) == end()
		}

		// Returns a pair that allows iterating over all nodes in a hash bucket
		//   first in the pair returned holds the iterator for the beginning of the bucket,
		//   second in the pair returned holds the iterator for the end of the bucket,
		// If no bucket is found, both values in the pair are set to end().
		//
		// See also the note above.
		eastl::pair<iterator, iterator> find_range_by_hash(hash_code_t c);
		eastl::pair<const_iterator, const_iterator> find_range_by_hash(hash_code_t c) const;

		size_type count(const key_type& k) const EA_NOEXCEPT;

		// transparent key support:
		// template<typename K>
		// size_type count(const K& k) const;

		eastl::pair<iterator, iterator>             equal_range(const key_type& k);
		eastl::pair<const_iterator, const_iterator> equal_range(const key_type& k) const;

		// transparent key support:
		// template<typename K>
		// eastl::pair<iterator, iterator>             equal_range(const K& k);
		// template<typename K>
		// eastl::pair<const_iterator, const_iterator> equal_range(const K& k) const;

		bool validate() const;
		int  validate_iterator(const_iterator i) const;

	protected:
		// We must remove one of the 'DoGetResultIterator' overloads from the overload-set (via SFINAE) because both can
		// not compile successfully at the same time. The 'bUniqueKeys' template parameter chooses at compile-time the
		// type of 'insert_return_type' between a pair<iterator,bool> and a raw iterator. We must pick between the two
		// overloads that unpacks the iterator from the pair or simply passes the provided iterator to the caller based
		// on the class template parameter.
		template <typename BoolConstantT>
		iterator DoGetResultIterator(BoolConstantT,
		                             const insert_return_type& irt,
		                             ENABLE_IF_TRUETYPE(BoolConstantT) = nullptr) const EA_NOEXCEPT
		{
			return irt.first;
		}

		template <typename BoolConstantT>
		iterator DoGetResultIterator(BoolConstantT,
		                             const insert_return_type& irt,
		                             DISABLE_IF_TRUETYPE(BoolConstantT) = nullptr) const EA_NOEXCEPT
		{
			return irt;
		}

		node_type*  DoAllocateNodeFromKey(const key_type& key);
		node_type*  DoAllocateNodeFromKey(key_type&& key);
		void        DoFreeNode(node_type* pNode);
		void        DoFreeNodes(node_type** pBucketArray, size_type);

		node_type** DoAllocateBuckets(size_type n);
		void        DoFreeBuckets(node_type** pBucketArray, size_type n);

		template <bool bDeleteOnException, typename Enabled = bool_constant<bUniqueKeys>, ENABLE_IF_TRUETYPE(Enabled) = nullptr> // only enabled when keys are unique
		eastl::pair<iterator, bool> DoInsertUniqueNode(const key_type& k, hash_code_t c, size_type n, node_type* pNodeNew);

		template <typename BoolConstantT, class... Args, ENABLE_IF_TRUETYPE(BoolConstantT) = nullptr>
		eastl::pair<iterator, bool> DoInsertValue(BoolConstantT, Args&&... args);

		template <typename BoolConstantT, class... Args, DISABLE_IF_TRUETYPE(BoolConstantT) = nullptr>
		iterator DoInsertValue(BoolConstantT, Args&&... args);


		template <typename BoolConstantT>
		eastl::pair<iterator, bool> DoInsertValueExtra(BoolConstantT,
													   const key_type& k,
													   hash_code_t c,
													   node_type* pNodeNew,
													   value_type&& value,
													   ENABLE_IF_TRUETYPE(BoolConstantT) = nullptr);

		template <typename BoolConstantT>
		eastl::pair<iterator, bool> DoInsertValue(BoolConstantT,
												  value_type&& value,
												  ENABLE_IF_TRUETYPE(BoolConstantT) = nullptr);

		template <typename BoolConstantT>
		iterator DoInsertValueExtra(BoolConstantT,
									const key_type& k,
									hash_code_t c,
									node_type* pNodeNew,
									value_type&& value,
									DISABLE_IF_TRUETYPE(BoolConstantT) = nullptr);

		template <typename BoolConstantT>
		iterator DoInsertValue(BoolConstantT, value_type&& value, DISABLE_IF_TRUETYPE(BoolConstantT) = nullptr);


		template <typename BoolConstantT>
		eastl::pair<iterator, bool> DoInsertValueExtra(BoolConstantT,
													   const key_type& k,
													   hash_code_t c,
													   node_type* pNodeNew,
													   const value_type& value,
													   ENABLE_IF_TRUETYPE(BoolConstantT) = nullptr);

		template <typename BoolConstantT>
		eastl::pair<iterator, bool> DoInsertValue(BoolConstantT,
		                                          const value_type& value,
		                                          ENABLE_IF_TRUETYPE(BoolConstantT) = nullptr);

		template <typename BoolConstantT>
		iterator DoInsertValueExtra(BoolConstantT,
		                            const key_type& k,
		                            hash_code_t c,
		                            node_type* pNodeNew,
		                            const value_type& value,
		                            DISABLE_IF_TRUETYPE(BoolConstantT) = nullptr);

		template <typename BoolConstantT>
		iterator DoInsertValue(BoolConstantT, const value_type& value, DISABLE_IF_TRUETYPE(BoolConstantT) = nullptr);

		template <class... Args>
		node_type* DoAllocateNode(Args&&... args);
		node_type* DoAllocateNode(value_type&& value);
		node_type* DoAllocateNode(const value_type& value);

		// DoInsertKey is supposed to get hash_code_t c  = get_hash_code(key).
		// it is done in case application has it's own hashset/hashmap-like containter, where hash code is for some reason known prior the insert
		// this allows to save some performance, especially with heavy hash functions
		eastl::pair<iterator, bool> DoInsertKey(true_type, const key_type& key, hash_code_t c);
		iterator                    DoInsertKey(false_type, const key_type& key, hash_code_t c);

		// We keep DoInsertKey overload without third parameter, for compatibility with older revisions of EASTL (3.12.07 and earlier)
		// It used to call get_hash_code as a first call inside the DoInsertKey.
		eastl::pair<iterator, bool> DoInsertKey(true_type, const key_type& key)  { return DoInsertKey(true_type(),  key, get_hash_code(key)); }
		iterator                    DoInsertKey(false_type, const key_type& key) { return DoInsertKey(false_type(), key, get_hash_code(key)); }

		void       DoRehash(size_type nBucketCount);
		node_type* DoFindNode(node_type* pNode, const key_type& k, hash_code_t c) const;
		NodeFindKeyData DoFindKeyData(const key_type& k) const;

		template <typename T>
		ENABLE_IF_HAS_HASHCODE(T, node_type) DoFindNode(T* pNode, hash_code_t c) const
		{
			for (; pNode; pNode = pNode->mpNext)
			{
				if (pNode->mnHashCode == c)
					return pNode;
			}
			return NULL;
		}

		template <typename U, typename BinaryPredicate>
		node_type* DoFindNodeT(node_type* pNode, const U& u, BinaryPredicate predicate) const;

	private:
		template <typename V, typename Enabled = bool_constant<bUniqueKeys>, ENABLE_IF_TRUETYPE(Enabled) = nullptr>
		eastl::pair<iterator, bool> DoInsertValueExtraForwarding(const key_type& k,
														hash_code_t c,
														node_type* pNodeNew,
														V&& value);


	}; // class hashtable





	///////////////////////////////////////////////////////////////////////
	// node_iterator_base
	///////////////////////////////////////////////////////////////////////

	template <typename Value, bool bCacheHashCode>
	inline bool operator==(const node_iterator_base<Value, bCacheHashCode>& a, const node_iterator_base<Value, bCacheHashCode>& b)
		{ return a.mpNode == b.mpNode; }

	template <typename Value, bool bCacheHashCode>
	inline bool operator!=(const node_iterator_base<Value, bCacheHashCode>& a, const node_iterator_base<Value, bCacheHashCode>& b)
		{ return a.mpNode != b.mpNode; }




	///////////////////////////////////////////////////////////////////////
	// hashtable_iterator_base
	///////////////////////////////////////////////////////////////////////

	template <typename Value, bool bCacheHashCode>
	inline bool operator==(const hashtable_iterator_base<Value, bCacheHashCode>& a, const hashtable_iterator_base<Value, bCacheHashCode>& b)
		{ return a.mpNode == b.mpNode; }

	template <typename Value, bool bCacheHashCode>
	inline bool operator!=(const hashtable_iterator_base<Value, bCacheHashCode>& a, const hashtable_iterator_base<Value, bCacheHashCode>& b)
		{ return a.mpNode != b.mpNode; }




	///////////////////////////////////////////////////////////////////////
	// hashtable
	///////////////////////////////////////////////////////////////////////

	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>
	::hashtable(size_type nBucketCount, const H1& h1, const H2& h2, const H& h,
				const Eq& eq, const EK& ek, const allocator_type& allocator)
		:   rehash_base<RP, hashtable>(),
			hash_code_base<K, V, EK, Eq, H1, H2, H, bC>(ek, eq, h1, h2, h),
			mnBucketCount(0),
			mnElementCount(0),
			mRehashPolicy(),
			mAllocator(allocator)
	{
		if(nBucketCount < 2)  // If we are starting in an initially empty state, with no memory allocation done.
			reset_lose_memory();
		else // Else we are creating a potentially non-empty hashtable...
		{
			EASTL_ASSERT(nBucketCount < 10000000);
			mnBucketCount = (size_type)mRehashPolicy.GetNextBucketCount((uint32_t)nBucketCount);
			mpBucketArray = DoAllocateBuckets(mnBucketCount); // mnBucketCount will always be at least 2.
		}
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename FowardIterator>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::hashtable(FowardIterator first, FowardIterator last, size_type nBucketCount, 
																	 const H1& h1, const H2& h2, const H& h, 
																	 const Eq& eq, const EK& ek, const allocator_type& allocator)
		:   rehash_base<rehash_policy_type, hashtable>(),
			hash_code_base<key_type, value_type, extract_key_type, key_equal, h1_type, h2_type, h_type, kCacheHashCode>(ek, eq, h1, h2, h),
		  //mnBucketCount(0), // This gets re-assigned below.
			mnElementCount(0),
			mRehashPolicy(),
			mAllocator(allocator)
	{
		if(nBucketCount < 2)
		{
			const size_type nElementCount = (size_type)eastl::ht_distance(first, last);
			mnBucketCount = (size_type)mRehashPolicy.GetBucketCount((uint32_t)nElementCount);
		}
		else
		{
			EASTL_ASSERT(nBucketCount < 10000000);
			mnBucketCount = nBucketCount;
		}

		mpBucketArray = DoAllocateBuckets(mnBucketCount); // mnBucketCount will always be at least 2.

		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
		#endif
				for(; first != last; ++first)
					insert(*first);
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
				clear();
				DoFreeBuckets(mpBucketArray, mnBucketCount);
				throw;
			}
		#endif
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::hashtable(const this_type& x)
		:   rehash_base<RP, hashtable>(x),
			hash_code_base<K, V, EK, Eq, H1, H2, H, bC>(x),
			mnBucketCount(x.mnBucketCount),
			mnElementCount(x.mnElementCount),
			mRehashPolicy(x.mRehashPolicy),
			mAllocator(x.mAllocator)
	{
		if(mnElementCount) // If there is anything to copy...
		{
			mpBucketArray = DoAllocateBuckets(mnBucketCount); // mnBucketCount will be at least 2.

			#if EASTL_EXCEPTIONS_ENABLED
				try
				{
			#endif
					for(size_type i = 0; i < x.mnBucketCount; ++i)
					{
						node_type*  pNodeSource = x.mpBucketArray[i];
						node_type** ppNodeDest  = mpBucketArray + i;

						while(pNodeSource)
						{
							*ppNodeDest = DoAllocateNode(pNodeSource->mValue);
							copy_code(*ppNodeDest, pNodeSource);
							ppNodeDest = &(*ppNodeDest)->mpNext;
							pNodeSource = pNodeSource->mpNext;
						}
					}
			#if EASTL_EXCEPTIONS_ENABLED
				}
				catch(...)
				{
					clear();
					DoFreeBuckets(mpBucketArray, mnBucketCount);
					throw;
				}
			#endif
		}
		else
		{
			// In this case, instead of allocate memory and copy nothing from x, 
			// we reset ourselves to a zero allocation state.
			reset_lose_memory();
		}
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::hashtable(this_type&& x)
		:   rehash_base<RP, hashtable>(x),
			hash_code_base<K, V, EK, Eq, H1, H2, H, bC>(x),
			mnBucketCount(0),
			mnElementCount(0),
			mRehashPolicy(x.mRehashPolicy),
			mAllocator(x.mAllocator)
	{
		reset_lose_memory(); // We do this here the same as we do it in the default ctor because it puts the container in a proper initial empty state. This code would be cleaner if we could rely on being able to use C++11 delegating constructors and just call the default ctor here.
		swap(x);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::hashtable(this_type&& x, const allocator_type& allocator)
		:   rehash_base<RP, hashtable>(x),
			hash_code_base<K, V, EK, Eq, H1, H2, H, bC>(x),
			mnBucketCount(0),
			mnElementCount(0),
			mRehashPolicy(x.mRehashPolicy),
			mAllocator(allocator)
	{
		reset_lose_memory(); // We do this here the same as we do it in the default ctor because it puts the container in a proper initial empty state. This code would be cleaner if we could rely on being able to use C++11 delegating constructors and just call the default ctor here.
		swap(x); // swap will directly or indirectly handle the possibility that mAllocator != x.mAllocator.
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline const typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::allocator_type&
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::get_allocator() const EA_NOEXCEPT
	{
		return mAllocator;
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::allocator_type&
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::get_allocator() EA_NOEXCEPT
	{
		return mAllocator;
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline void hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::set_allocator(const allocator_type& allocator)
	{
		mAllocator = allocator;
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::this_type&
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::operator=(const this_type& x)
	{
		if(this != &x)
		{
			clear();

			#if EASTL_ALLOCATOR_COPY_ENABLED
				mAllocator = x.mAllocator;
			#endif

			insert(x.begin(), x.end());
		}
		return *this;
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::this_type&
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::operator=(this_type&& x)
	{
		if(this != &x)
		{
			clear();        // To consider: Are we really required to clear here? x is going away soon and will clear itself in its dtor.
			swap(x);        // member swap handles the case that x has a different allocator than our allocator by doing a copy.
		}
		return *this;
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::this_type&
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::operator=(std::initializer_list<value_type> ilist)
	{
		// The simplest means of doing this is to clear and insert. There probably isn't a generic
		// solution that's any more efficient without having prior knowledge of the ilist contents.
		clear();
		insert(ilist.begin(), ilist.end());
		return *this;
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::~hashtable()
	{
		clear();
		DoFreeBuckets(mpBucketArray, mnBucketCount);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::node_type*
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoAllocateNodeFromKey(const key_type& key)
	{
		node_type* const pNode = (node_type*)allocate_memory(mAllocator, sizeof(node_type), EASTL_ALIGN_OF(node_type), 0);
		EASTL_ASSERT_MSG(pNode != nullptr, "the behaviour of eastl::allocators that return nullptr is not defined.");

		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
		#endif
				::new(eastl::addressof(pNode->mValue)) value_type(pair_first_construct, key);
				pNode->mpNext = NULL;
				return pNode;
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
				EASTLFree(mAllocator, pNode, sizeof(node_type));
				throw;
			}
		#endif
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
				typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::node_type*
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoAllocateNodeFromKey(key_type&& key)
	{
		node_type* const pNode = (node_type*)allocate_memory(mAllocator, sizeof(node_type), EASTL_ALIGN_OF(node_type), 0);
		EASTL_ASSERT_MSG(pNode != nullptr, "the behaviour of eastl::allocators that return nullptr is not defined.");

		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
		#endif
				::new(eastl::addressof(pNode->mValue)) value_type(pair_first_construct, eastl::move(key));
				pNode->mpNext = NULL;
				return pNode;
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
				EASTLFree(mAllocator, pNode, sizeof(node_type));
				throw;
			}
		#endif
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline void hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoFreeNode(node_type* pNode)
	{
		pNode->~node_type();
		EASTLFree(mAllocator, pNode, sizeof(node_type));
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	void hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoFreeNodes(node_type** pNodeArray, size_type n)
	{
		for(size_type i = 0; i < n; ++i)
		{
			node_type* pNode = pNodeArray[i];
			while(pNode)
			{
				node_type* const pTempNode = pNode;
				pNode = pNode->mpNext;
				DoFreeNode(pTempNode);
			}
			pNodeArray[i] = NULL;
		}
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::node_type**
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoAllocateBuckets(size_type n)
	{
		// We allocate one extra bucket to hold a sentinel, an arbitrary
		// non-null pointer. Iterator increment relies on this.
		EASTL_ASSERT(n > 1); // We reserve an mnBucketCount of 1 for the shared gpEmptyBucketArray.
		EASTL_CT_ASSERT(kHashtableAllocFlagBuckets == 0x00400000); // Currently we expect this to be so, because the allocator has a copy of this enum.
		node_type** const pBucketArray = (node_type**)EASTLAllocAlignedFlags(mAllocator, (n + 1) * sizeof(node_type*), EASTL_ALIGN_OF(node_type*), 0, kHashtableAllocFlagBuckets);
		//eastl::fill(pBucketArray, pBucketArray + n, (node_type*)NULL);
		memset(pBucketArray, 0, n * sizeof(node_type*));
		pBucketArray[n] = reinterpret_cast<node_type*>((uintptr_t)~0);
		return pBucketArray;
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline void hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoFreeBuckets(node_type** pBucketArray, size_type n)
	{
		// If n <= 1, then pBucketArray is from the shared gpEmptyBucketArray. We don't test 
		// for pBucketArray == &gpEmptyBucketArray because one library have a different gpEmptyBucketArray
		// than another but pass a hashtable to another. So we go by the size.
		if(n > 1)
			EASTLFree(mAllocator, pBucketArray, (n + 1) * sizeof(node_type*)); // '+1' because DoAllocateBuckets allocates nBucketCount + 1 buckets in order to have a NULL sentinel at the end.
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	void hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::swap(this_type& x)
	{
		hash_code_base<K, V, EK, Eq, H1, H2, H, bC>::base_swap(x); // hash_code_base has multiple implementations, so we let them handle the swap.
		eastl::swap(mRehashPolicy, x.mRehashPolicy);
		EASTL_MACRO_SWAP(node_type**, mpBucketArray, x.mpBucketArray);
		eastl::swap(mnBucketCount, x.mnBucketCount);
		eastl::swap(mnElementCount, x.mnElementCount);

		if (mAllocator != x.mAllocator) // If allocators are not equivalent...
		{
			eastl::swap(mAllocator, x.mAllocator);
		}
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline void hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::rehash_policy(const rehash_policy_type& rehashPolicy)
	{
		mRehashPolicy = rehashPolicy;

		const size_type nBuckets = rehashPolicy.GetBucketCount((uint32_t)mnElementCount);

		if(nBuckets > mnBucketCount)
			DoRehash(nBuckets);
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::find(const key_type& k)
	{
		const hash_code_t c = get_hash_code(k);
		const size_type   n = (size_type)bucket_index(k, c, (uint32_t)mnBucketCount);

		node_type* const pNode = DoFindNode(mpBucketArray[n], k, c);
		return pNode ? iterator(pNode, mpBucketArray + n) : iterator(mpBucketArray + mnBucketCount); // iterator(mpBucketArray + mnBucketCount) == end()
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::const_iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::find(const key_type& k) const
	{
		const hash_code_t c = get_hash_code(k);
		const size_type   n = (size_type)bucket_index(k, c, (uint32_t)mnBucketCount);

		node_type* const pNode = DoFindNode(mpBucketArray[n], k, c);
		return pNode ? const_iterator(pNode, mpBucketArray + n) : const_iterator(mpBucketArray + mnBucketCount); // iterator(mpBucketArray + mnBucketCount) == end()
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename U, typename UHash, typename BinaryPredicate>
	inline typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::find_as(const U& other, UHash uhash, BinaryPredicate predicate)
	{
		const hash_code_t c = (hash_code_t)uhash(other);
		const size_type   n = (size_type)(c % mnBucketCount); // This assumes we are using the mod range policy.

		node_type* const pNode = DoFindNodeT(mpBucketArray[n], other, predicate);
		return pNode ? iterator(pNode, mpBucketArray + n) : iterator(mpBucketArray + mnBucketCount); // iterator(mpBucketArray + mnBucketCount) == end()
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename U, typename UHash, typename BinaryPredicate>
	inline typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::const_iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::find_as(const U& other, UHash uhash, BinaryPredicate predicate) const
	{
		const hash_code_t c = (hash_code_t)uhash(other);
		const size_type   n = (size_type)(c % mnBucketCount); // This assumes we are using the mod range policy.

		node_type* const pNode = DoFindNodeT(mpBucketArray[n], other, predicate);
		return pNode ? const_iterator(pNode, mpBucketArray + n) : const_iterator(mpBucketArray + mnBucketCount); // iterator(mpBucketArray + mnBucketCount) == end()
	}


	/// hashtable_find
	///
	/// Helper function that defaults to using hash<U> and equal_to<>.
	/// This makes it so that by default you don't need to provide these.
	/// Note that the default hash functions may not be what you want, though.
	///
	/// Example usage. Instead of this:
	///     hash_set<string> hashSet;
	///     hashSet.find("hello", hash<char*>(), equal_to<>());
	///
	/// You can use this:
	///     hash_set<string> hashSet;
	///     hashtable_find(hashSet, "hello");
	///
	template <typename H, typename U>
	inline typename H::iterator hashtable_find(H& hashTable, U u)
		{ return hashTable.find_as(u, eastl::hash<U>(), eastl::equal_to<>()); }

	template <typename H, typename U>
	inline typename H::const_iterator hashtable_find(const H& hashTable, U u)
		{ return hashTable.find_as(u, eastl::hash<U>(), eastl::equal_to<>()); }



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename U>
	inline typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::find_as(const U& other)
		{ return eastl::hashtable_find(*this, other); }
		// VC++ doesn't appear to like the following, though it seems correct to me.
		// So we implement the workaround above until we can straighten this out.
		//{ return find_as(other, eastl::hash<U>(), eastl::equal_to<>()); }


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename U>
	inline typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::const_iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::find_as(const U& other) const
		{ return eastl::hashtable_find(*this, other); }
		// VC++ doesn't appear to like the following, though it seems correct to me.
		// So we implement the workaround above until we can straighten this out.
		//{ return find_as(other, eastl::hash<U>(), eastl::equal_to<>()); }



	template <typename K, typename V, typename A, typename EK, typename Eq, 
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	eastl::pair<typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::const_iterator,
				typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::const_iterator>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::find_range_by_hash(hash_code_t c) const
	{
		const size_type start = (size_type)bucket_index(c, (uint32_t)mnBucketCount);
		node_type* const pNodeStart = mpBucketArray[start];

		if (pNodeStart)
		{
			eastl::pair<const_iterator, const_iterator> pair(const_iterator(pNodeStart, mpBucketArray + start), 
															 const_iterator(pNodeStart, mpBucketArray + start));
			pair.second.increment_bucket();
			return pair;
		}

		return eastl::pair<const_iterator, const_iterator>(const_iterator(mpBucketArray + mnBucketCount),
														   const_iterator(mpBucketArray + mnBucketCount));
	}



	template <typename K, typename V, typename A, typename EK, typename Eq, 
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	eastl::pair<typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator,
				typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::find_range_by_hash(hash_code_t c)
	{
		const size_type start = (size_type)bucket_index(c, (uint32_t)mnBucketCount);
		node_type* const pNodeStart = mpBucketArray[start];

		if (pNodeStart)
		{
			eastl::pair<iterator, iterator> pair(iterator(pNodeStart, mpBucketArray + start), 
												 iterator(pNodeStart, mpBucketArray + start));
			pair.second.increment_bucket();
			return pair;

		}

		return eastl::pair<iterator, iterator>(iterator(mpBucketArray + mnBucketCount),
											   iterator(mpBucketArray + mnBucketCount));
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::size_type
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::count(const key_type& k) const EA_NOEXCEPT
	{
		const hash_code_t c      = get_hash_code(k);
		const size_type   n      = (size_type)bucket_index(k, c, (uint32_t)mnBucketCount);
		size_type         result = 0;

		// To do: Make a specialization for bU (unique keys) == true and take 
		// advantage of the fact that the count will always be zero or one in that case. 
		for(node_type* pNode = mpBucketArray[n]; pNode; pNode = pNode->mpNext)
		{
			if(compare(k, c, pNode))
				++result;
		}
		return result;
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	eastl::pair<typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator,
				typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::equal_range(const key_type& k)
	{
		const hash_code_t c     = get_hash_code(k);
		const size_type   n     = (size_type)bucket_index(k, c, (uint32_t)mnBucketCount);
		node_type**       head  = mpBucketArray + n;
		node_type*        pNode = DoFindNode(*head, k, c);

		if(pNode)
		{
			node_type* p1 = pNode->mpNext;

			for(; p1; p1 = p1->mpNext)
			{
				if(!compare(k, c, p1))
					break;
			}

			iterator first(pNode, head);
			iterator last(p1, head);

			if(!p1)
				last.increment_bucket();

			return eastl::pair<iterator, iterator>(first, last);
		}

		return eastl::pair<iterator, iterator>(iterator(mpBucketArray + mnBucketCount),  // iterator(mpBucketArray + mnBucketCount) == end()
											   iterator(mpBucketArray + mnBucketCount));
	}




	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	eastl::pair<typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::const_iterator,
				typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::const_iterator>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::equal_range(const key_type& k) const
	{
		const hash_code_t c     = get_hash_code(k);
		const size_type   n     = (size_type)bucket_index(k, c, (uint32_t)mnBucketCount);
		node_type**       head  = mpBucketArray + n;
		node_type*        pNode = DoFindNode(*head, k, c);

		if(pNode)
		{
			node_type* p1 = pNode->mpNext;

			for(; p1; p1 = p1->mpNext)
			{
				if(!compare(k, c, p1))
					break;
			}

			const_iterator first(pNode, head);
			const_iterator last(p1, head);

			if(!p1)
				last.increment_bucket();

			return eastl::pair<const_iterator, const_iterator>(first, last);
		}

		return eastl::pair<const_iterator, const_iterator>(const_iterator(mpBucketArray + mnBucketCount),  // iterator(mpBucketArray + mnBucketCount) == end()
														   const_iterator(mpBucketArray + mnBucketCount));
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::NodeFindKeyData
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoFindKeyData(const key_type& k) const {
		NodeFindKeyData d;
		d.code		   = get_hash_code(k);
		d.bucket_index = (size_type)bucket_index(k, d.code, (uint32_t)mnBucketCount);
		d.node		   = DoFindNode(mpBucketArray[d.bucket_index], k, d.code);
		return d;
	}

	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::node_type* 
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoFindNode(node_type* pNode, const key_type& k, hash_code_t c) const
	{
		for(; pNode; pNode = pNode->mpNext)
		{
			if(compare(k, c, pNode))
				return pNode;
		}
		return NULL;
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename U, typename BinaryPredicate>
	inline typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::node_type* 
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoFindNodeT(node_type* pNode, const U& other, BinaryPredicate predicate) const
	{
		for(; pNode; pNode = pNode->mpNext)
		{
			if(predicate(mExtractKey(pNode->mValue), other)) // Intentionally compare with key as first arg and other as second arg.
				return pNode;
		}
		return NULL;
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <bool bDeleteOnException, typename Enabled, ENABLE_IF_TRUETYPE(Enabled)> // only enabled when keys are unique
	eastl::pair<typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator, bool>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoInsertUniqueNode(const key_type& k, hash_code_t c, size_type n, node_type* pNodeNew)
	{
		const eastl::pair<bool, uint32_t> bRehash = mRehashPolicy.GetRehashRequired((uint32_t)mnBucketCount, (uint32_t)mnElementCount, (uint32_t)1);

		set_code(pNodeNew, c); // This is a no-op for most hashtables.

		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
		#endif
				if(bRehash.first)
				{
					n = (size_type)bucket_index(k, c, (uint32_t)bRehash.second);
					DoRehash(bRehash.second);
				}

				EASTL_ASSERT((uintptr_t)mpBucketArray != (uintptr_t)&gpEmptyBucketArray[0]);
				pNodeNew->mpNext = mpBucketArray[n];
				mpBucketArray[n] = pNodeNew;
				++mnElementCount;

				return eastl::pair<iterator, bool>(iterator(pNodeNew, mpBucketArray + n), true);
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
			    EA_CONSTEXPR_IF(bDeleteOnException) { DoFreeNode(pNodeNew); }
			    throw;
		    }
		#endif
	}

	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename BoolConstantT, class... Args, ENABLE_IF_TRUETYPE(BoolConstantT)>
	eastl::pair<typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator, bool>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoInsertValue(BoolConstantT, Args&&... args) // true_type means bUniqueKeys is true.
	{
		// Adds the value to the hash table if not already present. 
		// If already present then the existing value is returned via an iterator/bool pair.

		// We have a chicken-and-egg problem here. In order to know if and where to insert the value, we need to get the 
		// hashtable key for the value. But we don't explicitly have a value argument, we have a templated Args&&... argument.
		// We need the value_type in order to proceed, but that entails getting an instance of a value_type from the args.
		// And it may turn out that the value is already present in the hashtable and we need to cancel the insertion, 
		// despite having obtained a value_type to put into the hashtable. We have mitigated this problem somewhat by providing
		// specializations of the insert function for const value_type& and value_type&&, and so the only time this function
		// should get called is when args refers to arguments to construct a value_type.

		node_type* const  pNodeNew = DoAllocateNode(eastl::forward<Args>(args)...);
		const key_type&   k        = mExtractKey(pNodeNew->mValue);
		const hash_code_t c        = get_hash_code(k);
		size_type         n        = (size_type)bucket_index(k, c, (uint32_t)mnBucketCount);
		node_type* const  pNode    = DoFindNode(mpBucketArray[n], k, c);

		if(pNode == NULL) // If value is not present... add it.
		{
			return DoInsertUniqueNode<true>(k, c, n, pNodeNew);
		}
		else
		{
			// To do: We have an inefficiency to deal with here. We allocated a node above but we are freeing it here because
			// it turned out it wasn't needed. But we needed to create the node in order to get the hashtable key for
			// the node. One possible resolution is to create specializations: DoInsertValue(true_type, value_type&&) and 
			// DoInsertValue(true_type, const value_type&) which don't need to create a node up front in order to get the 
			// hashtable key. Probably most users would end up using these pathways instead of this Args... pathway.
			// While we should considering handling this to-do item, a lot of the performance limitations of maps and sets 
			// in practice is with finding elements rather than adding (potentially redundant) new elements.
			DoFreeNode(pNodeNew);
		}

		return eastl::pair<iterator, bool>(iterator(pNode, mpBucketArray + n), false);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename BoolConstantT, class... Args, DISABLE_IF_TRUETYPE(BoolConstantT)>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoInsertValue(BoolConstantT, Args&&... args) // false_type means bUniqueKeys is false.
	{
		const eastl::pair<bool, uint32_t> bRehash = mRehashPolicy.GetRehashRequired((uint32_t)mnBucketCount, (uint32_t)mnElementCount, (uint32_t)1);

		if(bRehash.first)
			DoRehash(bRehash.second);

		node_type*        pNodeNew = DoAllocateNode(eastl::forward<Args>(args)...);
		const key_type&   k        = mExtractKey(pNodeNew->mValue);
		const hash_code_t c        = get_hash_code(k);
		const size_type   n        = (size_type)bucket_index(k, c, (uint32_t)mnBucketCount);

		set_code(pNodeNew, c); // This is a no-op for most hashtables.

		// To consider: Possibly make this insertion not make equal elements contiguous.
		// As it stands now, we insert equal values contiguously in the hashtable.
		// The benefit is that equal_range can work in a sensible manner and that
		// erase(value) can more quickly find equal values. The downside is that
		// this insertion operation taking some extra time. How important is it to
		// us that equal_range span all equal items? 
		node_type* const pNodePrev = DoFindNode(mpBucketArray[n], k, c);

		if(pNodePrev == NULL)
		{
			EASTL_ASSERT((void**)mpBucketArray != &gpEmptyBucketArray[0]);
			pNodeNew->mpNext = mpBucketArray[n];
			mpBucketArray[n] = pNodeNew;
		}
		else
		{
			pNodeNew->mpNext  = pNodePrev->mpNext;
			pNodePrev->mpNext = pNodeNew;
		}

		++mnElementCount;

		return iterator(pNodeNew, mpBucketArray + n);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <class... Args>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::node_type*
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoAllocateNode(Args&&... args)
	{
		node_type* const pNode = (node_type*)allocate_memory(mAllocator, sizeof(node_type), EASTL_ALIGN_OF(node_type), 0);
		EASTL_ASSERT_MSG(pNode != nullptr, "the behaviour of eastl::allocators that return nullptr is not defined.");

		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
		#endif
				::new(eastl::addressof(pNode->mValue)) value_type(eastl::forward<Args>(args)...);
				pNode->mpNext = NULL;
				return pNode;
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
				EASTLFree(mAllocator, pNode, sizeof(node_type));
				throw;
			}
		#endif
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Note: The following insertion-related functions are nearly copies of the above three functions,
	// but are for value_type&& and const value_type& arguments. It's useful for us to have the functions
	// below, even when using a fully compliant C++11 compiler that supports the above functions. 
	// The reason is because the specializations below are slightly more efficient because they can delay
	// the creation of a node until it's known that it will be needed.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename BoolConstantT>
	inline eastl::pair<typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator, bool>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoInsertValueExtra(BoolConstantT, const key_type& k,
		hash_code_t c, node_type* pNodeNew, value_type&& value, ENABLE_IF_TRUETYPE(BoolConstantT)) // true_type means bUniqueKeys is true.
	{
		return DoInsertValueExtraForwarding(k, c, pNodeNew, eastl::move(value));
	}

	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename BoolConstantT>
	inline eastl::pair<typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator, bool>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoInsertValueExtra(BoolConstantT, const key_type& k,
		hash_code_t c, node_type* pNodeNew, const value_type& value, ENABLE_IF_TRUETYPE(BoolConstantT)) // true_type means bUniqueKeys is true.
	{
		return DoInsertValueExtraForwarding(k, c, pNodeNew, value);
	}

	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename VFwd, typename Enabled, ENABLE_IF_TRUETYPE(Enabled)> // true_type means bUniqueKeys is true.
	eastl::pair<typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator, bool>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoInsertValueExtraForwarding(const key_type& k,
		hash_code_t c, node_type* pNodeNew, VFwd&& value)
	{
		// Adds the value to the hash table if not already present. 
		// If already present then the existing value is returned via an iterator/bool pair.
		size_type         n     = (size_type)bucket_index(k, c, (uint32_t)mnBucketCount);
		node_type* const  pNode = DoFindNode(mpBucketArray[n], k, c);

		if(pNode == NULL) // If value is not present... add it.
		{
			// Allocate the new node before doing the rehash so that we don't
			// do a rehash if the allocation throws.
			if(pNodeNew)
			{
				::new(eastl::addressof(pNodeNew->mValue)) value_type(eastl::forward<VFwd>(value)); // It's expected that pNodeNew was allocated with allocate_uninitialized_node.
				return DoInsertUniqueNode<false>(k, c, n, pNodeNew);
			}
			else
			{
				pNodeNew = DoAllocateNode(eastl::move(value));
				return DoInsertUniqueNode<true>(k, c, n, pNodeNew);
			}
		}
		// Else the value is already present, so don't add a new node. And don't free pNodeNew.

		return eastl::pair<iterator, bool>(iterator(pNode, mpBucketArray + n), false);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename BoolConstantT>
	eastl::pair<typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator, bool>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoInsertValue(BoolConstantT, value_type&& value, ENABLE_IF_TRUETYPE(BoolConstantT)) // true_type means bUniqueKeys is true.
	{
		const key_type&   k = mExtractKey(value);
		const hash_code_t c = get_hash_code(k);

		return DoInsertValueExtra(true_type(), k, c, NULL, eastl::move(value));
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename BoolConstantT>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoInsertValueExtra(BoolConstantT, const key_type& k, hash_code_t c, node_type* pNodeNew, value_type&& value, 
			DISABLE_IF_TRUETYPE(BoolConstantT)) // false_type means bUniqueKeys is false.
	{
		const eastl::pair<bool, uint32_t> bRehash = mRehashPolicy.GetRehashRequired((uint32_t)mnBucketCount, (uint32_t)mnElementCount, (uint32_t)1);

		if(bRehash.first)
			DoRehash(bRehash.second); // Note: We don't need to wrap this call with try/catch because there's nothing we would need to do in the catch.

		const size_type n = (size_type)bucket_index(k, c, (uint32_t)mnBucketCount);

		if(pNodeNew)
			::new(eastl::addressof(pNodeNew->mValue)) value_type(eastl::move(value)); // It's expected that pNodeNew was allocated with allocate_uninitialized_node.
		else
			pNodeNew = DoAllocateNode(eastl::move(value));

		set_code(pNodeNew, c); // This is a no-op for most hashtables.

		// To consider: Possibly make this insertion not make equal elements contiguous.
		// As it stands now, we insert equal values contiguously in the hashtable.
		// The benefit is that equal_range can work in a sensible manner and that
		// erase(value) can more quickly find equal values. The downside is that
		// this insertion operation taking some extra time. How important is it to
		// us that equal_range span all equal items? 
		node_type* const pNodePrev = DoFindNode(mpBucketArray[n], k, c);

		if(pNodePrev == NULL)
		{
			EASTL_ASSERT((void**)mpBucketArray != &gpEmptyBucketArray[0]);
			pNodeNew->mpNext = mpBucketArray[n];
			mpBucketArray[n] = pNodeNew;
		}
		else
		{
			pNodeNew->mpNext  = pNodePrev->mpNext;
			pNodePrev->mpNext = pNodeNew;
		}

		++mnElementCount;

		return iterator(pNodeNew, mpBucketArray + n);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template<typename BoolConstantT>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoInsertValue(BoolConstantT, value_type&& value, DISABLE_IF_TRUETYPE(BoolConstantT)) // false_type means bUniqueKeys is false.
	{
		const key_type&   k = mExtractKey(value);
		const hash_code_t c = get_hash_code(k);

		return DoInsertValueExtra(false_type(), k, c, NULL, eastl::move(value));
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::node_type*
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoAllocateNode(value_type&& value)
	{
		node_type* const pNode = (node_type*)allocate_memory(mAllocator, sizeof(node_type), EASTL_ALIGN_OF(node_type), 0);
		EASTL_ASSERT_MSG(pNode != nullptr, "the behaviour of eastl::allocators that return nullptr is not defined.");

		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
		#endif
				::new(eastl::addressof(pNode->mValue)) value_type(eastl::move(value));
				pNode->mpNext = NULL;
				return pNode;
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
				EASTLFree(mAllocator, pNode, sizeof(node_type));
				throw;
			}
		#endif
	}

	template <typename K, typename V, typename A, typename EK, typename Eq,
				typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template<typename BoolConstantT>
	eastl::pair<typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator, bool>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoInsertValue(BoolConstantT, const value_type& value, ENABLE_IF_TRUETYPE(BoolConstantT)) // true_type means bUniqueKeys is true.
	{
		const key_type&   k = mExtractKey(value);
		const hash_code_t c = get_hash_code(k);

		return DoInsertValueExtra(true_type(), k, c, NULL, value);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
				typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename BoolConstantT>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoInsertValueExtra(BoolConstantT, const key_type& k, hash_code_t c, node_type* pNodeNew, const value_type& value,
			DISABLE_IF_TRUETYPE(BoolConstantT)) // false_type means bUniqueKeys is false.
	{
		const eastl::pair<bool, uint32_t> bRehash = mRehashPolicy.GetRehashRequired((uint32_t)mnBucketCount, (uint32_t)mnElementCount, (uint32_t)1);

		if(bRehash.first)
			DoRehash(bRehash.second); // Note: We don't need to wrap this call with try/catch because there's nothing we would need to do in the catch.

		const size_type n = (size_type)bucket_index(k, c, (uint32_t)mnBucketCount);

		if(pNodeNew)
			::new(eastl::addressof(pNodeNew->mValue)) value_type(value); // It's expected that pNodeNew was allocated with allocate_uninitialized_node.
		else
			pNodeNew = DoAllocateNode(value);

		set_code(pNodeNew, c); // This is a no-op for most hashtables.

		// To consider: Possibly make this insertion not make equal elements contiguous.
		// As it stands now, we insert equal values contiguously in the hashtable.
		// The benefit is that equal_range can work in a sensible manner and that
		// erase(value) can more quickly find equal values. The downside is that
		// this insertion operation taking some extra time. How important is it to
		// us that equal_range span all equal items? 
		node_type* const pNodePrev = DoFindNode(mpBucketArray[n], k, c);

		if(pNodePrev == NULL)
		{
			EASTL_ASSERT((void**)mpBucketArray != &gpEmptyBucketArray[0]);
			pNodeNew->mpNext = mpBucketArray[n];
			mpBucketArray[n] = pNodeNew;
		}
		else
		{
			pNodeNew->mpNext  = pNodePrev->mpNext;
			pNodePrev->mpNext = pNodeNew;
		}

		++mnElementCount;

		return iterator(pNodeNew, mpBucketArray + n);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
				typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template<typename BoolConstantT>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoInsertValue(BoolConstantT, const value_type& value, DISABLE_IF_TRUETYPE(BoolConstantT)) // false_type means bUniqueKeys is false.
	{
		const key_type&   k = mExtractKey(value);
		const hash_code_t c = get_hash_code(k);

		return DoInsertValueExtra(false_type(), k, c, NULL, value);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::node_type*
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoAllocateNode(const value_type& value)
	{
		node_type* const pNode = (node_type*)allocate_memory(mAllocator, sizeof(node_type), EASTL_ALIGN_OF(node_type), 0);
		EASTL_ASSERT_MSG(pNode != nullptr, "the behaviour of eastl::allocators that return nullptr is not defined.");

		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
		#endif
				::new(eastl::addressof(pNode->mValue)) value_type(value);
				pNode->mpNext = NULL;
				return pNode;
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
				EASTLFree(mAllocator, pNode, sizeof(node_type));
				throw;
			}
		#endif
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::node_type*
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::allocate_uninitialized_node()
	{
		// We don't wrap this in try/catch because users of this function are expected to do that themselves as needed.
		node_type* const pNode = (node_type*)allocate_memory(mAllocator, sizeof(node_type), EASTL_ALIGN_OF(node_type), 0);
		EASTL_ASSERT_MSG(pNode != nullptr, "the behaviour of eastl::allocators that return nullptr is not defined.");
		// Leave pNode->mValue uninitialized.
		pNode->mpNext = NULL;
		return pNode;
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	void hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::free_uninitialized_node(node_type* pNode)
	{
		// pNode->mValue is expected to be uninitialized.
		EASTLFree(mAllocator, pNode, sizeof(node_type));
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	eastl::pair<typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator, bool>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoInsertKey(true_type, const key_type& key, const hash_code_t c) // true_type means bUniqueKeys is true.
	{
		size_type         n     = (size_type)bucket_index(key, c, (uint32_t)mnBucketCount);
		node_type* const  pNode = DoFindNode(mpBucketArray[n], key, c);

		if(pNode == NULL)
		{
			const eastl::pair<bool, uint32_t> bRehash = mRehashPolicy.GetRehashRequired((uint32_t)mnBucketCount, (uint32_t)mnElementCount, (uint32_t)1);

			// Allocate the new node before doing the rehash so that we don't
			// do a rehash if the allocation throws.
			node_type* const pNodeNew = DoAllocateNodeFromKey(key);
			set_code(pNodeNew, c); // This is a no-op for most hashtables.

			#if EASTL_EXCEPTIONS_ENABLED
				try
				{
			#endif
					if(bRehash.first)
					{
						n = (size_type)bucket_index(key, c, (uint32_t)bRehash.second);
						DoRehash(bRehash.second);
					}

					EASTL_ASSERT((void**)mpBucketArray != &gpEmptyBucketArray[0]);
					pNodeNew->mpNext = mpBucketArray[n];
					mpBucketArray[n] = pNodeNew;
					++mnElementCount;

					return eastl::pair<iterator, bool>(iterator(pNodeNew, mpBucketArray + n), true);
			#if EASTL_EXCEPTIONS_ENABLED
				}
				catch(...)
				{
					DoFreeNode(pNodeNew);
					throw;
				}
			#endif
		}

		return eastl::pair<iterator, bool>(iterator(pNode, mpBucketArray + n), false);
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoInsertKey(false_type, const key_type& key, const hash_code_t c) // false_type means bUniqueKeys is false.
	{
		const eastl::pair<bool, uint32_t> bRehash = mRehashPolicy.GetRehashRequired((uint32_t)mnBucketCount, (uint32_t)mnElementCount, (uint32_t)1);

		if(bRehash.first)
			DoRehash(bRehash.second);

		const size_type   n = (size_type)bucket_index(key, c, (uint32_t)mnBucketCount);

		node_type* const pNodeNew = DoAllocateNodeFromKey(key);
		set_code(pNodeNew, c); // This is a no-op for most hashtables.

		// To consider: Possibly make this insertion not make equal elements contiguous.
		// As it stands now, we insert equal values contiguously in the hashtable.
		// The benefit is that equal_range can work in a sensible manner and that
		// erase(value) can more quickly find equal values. The downside is that
		// this insertion operation taking some extra time. How important is it to
		// us that equal_range span all equal items? 
		node_type* const pNodePrev = DoFindNode(mpBucketArray[n], key, c);

		if(pNodePrev == NULL)
		{
			EASTL_ASSERT((void**)mpBucketArray != &gpEmptyBucketArray[0]);
			pNodeNew->mpNext = mpBucketArray[n];
			mpBucketArray[n] = pNodeNew;
		}
		else
		{
			pNodeNew->mpNext  = pNodePrev->mpNext;
			pNodePrev->mpNext = pNodeNew;
		}

		++mnElementCount;

		return iterator(pNodeNew, mpBucketArray + n);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
				typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <class... Args>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert_return_type
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::emplace(Args&&... args)
	{
		return DoInsertValue(has_unique_keys_type(), eastl::forward<Args>(args)...); // Need to use forward instead of move because Args&& is a "universal reference" instead of an rvalue reference.
	}

	template <typename K, typename V, typename A, typename EK, typename Eq,
				typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <class... Args>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::emplace_hint(const_iterator, Args&&... args)
	{
		// We currently ignore the iterator argument as a hint.
		insert_return_type result = DoInsertValue(has_unique_keys_type(), eastl::forward<Args>(args)...);
		return DoGetResultIterator(has_unique_keys_type(), result);
	}

	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert_return_type
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert(value_type&& otherValue)
	{
		return DoInsertValue(has_unique_keys_type(), eastl::move(otherValue));
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <class P>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert_return_type
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert(hash_code_t c, node_type* pNodeNew, P&& otherValue)
	{
		// pNodeNew->mValue is expected to be uninitialized.
		value_type value(eastl::forward<P>(otherValue)); // Need to use forward instead of move because P&& is a "universal reference" instead of an rvalue reference.
		const key_type& k = mExtractKey(value);
		return DoInsertValueExtra(has_unique_keys_type(), k, c, pNodeNew, eastl::move(value));
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert(const_iterator, value_type&& value)
	{
		// We currently ignore the iterator argument as a hint.
		insert_return_type result = DoInsertValue(has_unique_keys_type(), value_type(eastl::move(value)));
		return DoGetResultIterator(has_unique_keys_type(), result);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert_return_type
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert(const value_type& value) 
	{
		return DoInsertValue(has_unique_keys_type(), value);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert_return_type
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert(hash_code_t c, node_type* pNodeNew, const value_type& value) 
	{
		// pNodeNew->mValue is expected to be uninitialized.
		const key_type& k = mExtractKey(value);
		return DoInsertValueExtra(has_unique_keys_type(), k, c, pNodeNew, value);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
	          typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename P, class>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert_return_type
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert(P&& otherValue)
	{
		return emplace(eastl::forward<P>(otherValue));
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert(const_iterator, const value_type& value)
	{
		// We ignore the first argument (hint iterator). It's not likely to be useful for hashtable containers.
		insert_return_type result = DoInsertValue(has_unique_keys_type(), value);
		return DoGetResultIterator(has_unique_keys_type(), result);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	void hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert(std::initializer_list<value_type> ilist)
	{
		insert(ilist.begin(), ilist.end());
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <typename InputIterator>
	void
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert(InputIterator first, InputIterator last)
	{
		const uint32_t nElementAdd = (uint32_t)eastl::ht_distance(first, last);
		const eastl::pair<bool, uint32_t> bRehash = mRehashPolicy.GetRehashRequired((uint32_t)mnBucketCount, (uint32_t)mnElementCount, nElementAdd);

		if(bRehash.first)
			DoRehash(bRehash.second);

		for(; first != last; ++first)
			DoInsertValue(has_unique_keys_type(), *first);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
	          typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <class M>
	eastl::pair<typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator, bool>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert_or_assign(const key_type& k, M&& obj)
	{
		auto iter = find(k);
		if(iter == end())
		{
			return insert(value_type(piecewise_construct, eastl::forward_as_tuple(k), eastl::forward_as_tuple(eastl::forward<M>(obj))));
		}
		else
		{
			iter->second = eastl::forward<M>(obj);
			return {iter, false};
		}
	}

	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <class M>
	eastl::pair<typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator, bool>
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert_or_assign(key_type&& k, M&& obj)
	{
		auto iter = find(k);
		if(iter == end())
		{
			return insert(value_type(piecewise_construct, eastl::forward_as_tuple(eastl::move(k)), eastl::forward_as_tuple(eastl::forward<M>(obj))));
		}
		else
		{
			iter->second = eastl::forward<M>(obj);
			return {iter, false};
		}
	}

	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <class M>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator 
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert_or_assign(const_iterator, const key_type& k, M&& obj)
	{
		return insert_or_assign(k, eastl::forward<M>(obj)).first; // we ignore the iterator hint
	}

	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	template <class M>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator 
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::insert_or_assign(const_iterator, key_type&& k, M&& obj)
	{
		return insert_or_assign(eastl::move(k), eastl::forward<M>(obj)).first; // we ignore the iterator hint
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::erase(const_iterator i)
	{
		iterator iNext(i.mpNode, i.mpBucket); // Convert from const_iterator to iterator while constructing.
		++iNext;

		node_type* pNode        =  i.mpNode;
		node_type* pNodeCurrent = *i.mpBucket;

		if(pNodeCurrent == pNode)
			*i.mpBucket = pNodeCurrent->mpNext;
		else
		{
			// We have a singly-linked list, so we have no choice but to
			// walk down it till we find the node before the node at 'i'.
			node_type* pNodeNext = pNodeCurrent->mpNext;

			while(pNodeNext != pNode)
			{
				pNodeCurrent = pNodeNext;
				pNodeNext    = pNodeCurrent->mpNext;
			}

			pNodeCurrent->mpNext = pNodeNext->mpNext;
		}

		DoFreeNode(pNode);
		--mnElementCount;

		return iNext;
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::iterator
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::erase(const_iterator first, const_iterator last)
	{
		while(first != last)
			first = erase(first);
		return iterator(first.mpNode, first.mpBucket);
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	typename hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::size_type 
	hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::erase(const key_type& k)
	{
		// To do: Reimplement this function to do a single loop and not try to be 
		// smart about element contiguity. The mechanism here is only a benefit if the 
		// buckets are heavily overloaded; otherwise this mechanism may be slightly slower.

		const hash_code_t c = get_hash_code(k);
		const size_type   n = (size_type)bucket_index(k, c, (uint32_t)mnBucketCount);
		const size_type   nElementCountSaved = mnElementCount;

		node_type** pBucketArray = mpBucketArray + n;

		while(*pBucketArray && !compare(k, c, *pBucketArray))
			pBucketArray = &(*pBucketArray)->mpNext;

		node_type* pDeleteList = nullptr;
		while(*pBucketArray && compare(k, c, *pBucketArray))
		{
			node_type* const pNode = *pBucketArray;
			*pBucketArray = pNode->mpNext;
			// Don't free the node here, k might be a reference to the key inside this node,
			// and we're re-using it when we compare to the following nodes.
			// Instead, add it to the list of things to be deleted.
			pNode->mpNext = pDeleteList;
			pDeleteList = pNode;
			--mnElementCount;
		}

		while (pDeleteList) {
			node_type* const pToDelete = pDeleteList;
			pDeleteList = pDeleteList->mpNext;
			DoFreeNode(pToDelete);
		}

		return nElementCountSaved - mnElementCount;
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline void hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::clear()
	{
		DoFreeNodes(mpBucketArray, mnBucketCount);
		mnElementCount = 0;
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline void hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::clear(bool clearBuckets)
	{
		DoFreeNodes(mpBucketArray, mnBucketCount);
		if(clearBuckets)
		{
			DoFreeBuckets(mpBucketArray, mnBucketCount);
			reset_lose_memory();
		}
		mnElementCount = 0;
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline void hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::reset_lose_memory() EA_NOEXCEPT
	{
		// The reset function is a special extension function which unilaterally 
		// resets the container to an empty state without freeing the memory of 
		// the contained objects. This is useful for very quickly tearing down a 
		// container built into scratch memory.
		mnBucketCount  = 1;

		#ifdef _MSC_VER
			mpBucketArray = (node_type**)&gpEmptyBucketArray[0];
		#else
			void* p = &gpEmptyBucketArray[0];
			memcpy(&mpBucketArray, &p, sizeof(mpBucketArray)); // Other compilers implement strict aliasing and casting is thus unsafe.
		#endif

		mnElementCount = 0;
		mRehashPolicy.mnNextResize = 0;
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline void hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::reserve(size_type nElementCount)
	{
		rehash(mRehashPolicy.GetBucketCount(uint32_t(nElementCount)));
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline void hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::rehash(size_type nBucketCount)
	{
		// Note that we unilaterally use the passed in bucket count; we do not attempt migrate it
		// up to the next prime number. We leave it at the user's discretion to do such a thing.
		DoRehash(nBucketCount);
	}



	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	void hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::DoRehash(size_type nNewBucketCount)
	{
		node_type** const pBucketArray = DoAllocateBuckets(nNewBucketCount); // nNewBucketCount should always be >= 2.

		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
		#endif
				node_type* pNode;

				for(size_type i = 0; i < mnBucketCount; ++i)
				{
					while((pNode = mpBucketArray[i]) != NULL) // Using '!=' disables compiler warnings.
					{
						const size_type nNewBucketIndex = (size_type)bucket_index(pNode, (uint32_t)nNewBucketCount);

						mpBucketArray[i] = pNode->mpNext;
						pNode->mpNext    = pBucketArray[nNewBucketIndex];
						pBucketArray[nNewBucketIndex] = pNode;
					}
				}

				DoFreeBuckets(mpBucketArray, mnBucketCount);
				mnBucketCount = nNewBucketCount;
				mpBucketArray = pBucketArray;
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
				// A failure here means that a hash function threw an exception.
				// We can't restore the previous state without calling the hash
				// function again, so the only sensible recovery is to delete everything.
				DoFreeNodes(pBucketArray, nNewBucketCount);
				DoFreeBuckets(pBucketArray, nNewBucketCount);
				DoFreeNodes(mpBucketArray, mnBucketCount);
				mnElementCount = 0;
				throw;
			}
		#endif
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline bool hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::validate() const
	{
		// Verify our empty bucket array is unmodified.
		if(gpEmptyBucketArray[0] != NULL)
			return false;

		if(gpEmptyBucketArray[1] != (void*)uintptr_t(~0))
			return false;

		// Verify that we have at least one bucket. Calculations can  
		// trigger division by zero exceptions otherwise.
		if(mnBucketCount == 0)
			return false;

		// Verify that gpEmptyBucketArray is used correctly.
		// gpEmptyBucketArray is only used when initially empty.
		if((void**)mpBucketArray == &gpEmptyBucketArray[0])
		{
			if(mnElementCount) // gpEmptyBucketArray is used only for empty hash tables.
				return false;

			if(mnBucketCount != 1) // gpEmptyBucketArray is used exactly an only for mnBucketCount == 1.
				return false;
		}
		else
		{
			if(mnBucketCount < 2) // Small bucket counts *must* use gpEmptyBucketArray.
				return false;
		}

		// Verify that the element count matches mnElementCount. 
		size_type nElementCount = 0;

		for(const_iterator temp = begin(), tempEnd = end(); temp != tempEnd; ++temp)
			++nElementCount;

		if(nElementCount != mnElementCount)
			return false;

		// To do: Verify that individual elements are in the expected buckets.

		return true;
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	int hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>::validate_iterator(const_iterator i) const
	{
		// To do: Come up with a more efficient mechanism of doing this.

		for(const_iterator temp = begin(), tempEnd = end(); temp != tempEnd; ++temp)
		{
			if(temp == i)
				return (isf_valid | isf_current | isf_can_dereference);
		}

		if(i == end())
			return (isf_valid | isf_current); 

		return isf_none;
	}



	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	// operator==, != have been moved to the specific container subclasses (e.g. hash_map).

	// The following comparison operators are deprecated and will likely be removed in a  
	// future version of this package.
	//
	// Comparing hash tables for less-ness is an odd thing to do. We provide it for 
	// completeness, though the user is advised to be wary of how they use this.
	//
	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	EASTL_REMOVE_AT_2024_APRIL inline bool operator<(const hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>& a,
						  const hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>& b)
	{
		// This requires hash table elements to support operator<. Since the hash table
		// doesn't compare elements via less (it does so via equals), we must use the 
		// globally defined operator less for the elements.
		return eastl::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	EASTL_REMOVE_AT_2024_APRIL inline bool operator>(const hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>& a,
						  const hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>& b)
	{
		return b < a;
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	EASTL_REMOVE_AT_2024_APRIL inline bool operator<=(const hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>& a,
						   const hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>& b)
	{
		return !(b < a);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	EASTL_REMOVE_AT_2024_APRIL inline bool operator>=(const hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>& a,
						   const hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>& b)
	{
		return !(a < b);
	}


	template <typename K, typename V, typename A, typename EK, typename Eq,
			  typename H1, typename H2, typename H, typename RP, bool bC, bool bM, bool bU>
	inline void swap(const hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>& a, 
					 const hashtable<K, V, A, EK, Eq, H1, H2, H, RP, bC, bM, bU>& b)
	{
		a.swap(b);
	}


} // namespace eastl


EA_RESTORE_VC_WARNING();


#endif // Header include guard
