/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements an intrusive hash table, which is a hash table whereby
// the container nodes are the hash table objects themselves. This has benefits
// primarily in terms of memory management. There are some minor limitations
// that result from this.
//
///////////////////////////////////////////////////////////////////////////////



#ifndef EASTL_INTERNAL_INTRUSIVE_HASHTABLE_H
#define EASTL_INTERNAL_INTRUSIVE_HASHTABLE_H


#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#include <EASTL/internal/config.h>
#include <EASTL/internal/hashtable.h>
#include <EASTL/type_traits.h>
#include <EASTL/iterator.h>
#include <EASTL/functional.h>
#include <EASTL/utility.h>
#include <EASTL/algorithm.h>

EA_DISABLE_ALL_VC_WARNINGS();
#include <new>
#include <stddef.h>
#include <string.h>
EA_RESTORE_ALL_VC_WARNINGS();


namespace eastl
{

	/// intrusive_hash_node
	///
	/// A hash_node stores an element in a hash table, much like a 
	/// linked list node stores an element in a linked list. 
	/// An intrusive_hash_node additionally can, via template parameter,
	/// store a hash code in the node to speed up hash calculations 
	/// and comparisons in some cases.
	///
	/// To consider: Make a version of intrusive_hash_node which is
	/// templated on the container type. This would allow for the 
	/// mpNext pointer to be the container itself and thus allow
	/// for easier debugging. 
	/// 
	/// Example usage:
	///   struct Widget : public intrusive_hash_node{ ... };
	/// 
	///   struct Dagget : public intrusive_hash_node_key<int>{ ... };
	/// 
	struct intrusive_hash_node
	{
		intrusive_hash_node* mpNext;
	};


	template <typename Key>
	struct intrusive_hash_node_key : public intrusive_hash_node
	{
		typedef Key key_type;
		Key mKey;
	};



	/// intrusive_node_iterator
	///
	/// Node iterators iterate nodes within a given bucket.
	///
	/// The bConst parameter defines if the iterator is a const_iterator
	/// or an iterator.
	///
	template <typename Value, bool bConst>
	struct intrusive_node_iterator
	{
	public:
		typedef intrusive_node_iterator<Value, bConst>                   this_type;
		typedef Value                                                    value_type;
		typedef Value                                                    node_type;
		typedef ptrdiff_t                                                difference_type;
		typedef typename type_select<bConst, const Value*, Value*>::type pointer;
		typedef typename type_select<bConst, const Value&, Value&>::type reference;
		typedef EASTL_ITC_NS::forward_iterator_tag                       iterator_category;

	public:
		node_type* mpNode;

	public:
		intrusive_node_iterator()
			: mpNode(NULL) { }

		explicit intrusive_node_iterator(value_type* pNode)
			: mpNode(pNode) { }

		intrusive_node_iterator(const intrusive_node_iterator<Value, true>& x)
			: mpNode(x.mpNode) { }

		reference operator*() const
			{ return *mpNode; }

		pointer operator->() const
			{ return mpNode; }

		this_type& operator++()
			{ mpNode = static_cast<node_type*>(mpNode->mpNext); return *this; }

		this_type operator++(int)
			{ this_type temp(*this); mpNode = static_cast<node_type*>(mpNode->mpNext); return temp; }

	}; // intrusive_node_iterator




	/// intrusive_hashtable_iterator_base
	///
	/// An intrusive_hashtable_iterator_base iterates the entire hash table and 
	/// not just nodes within a single bucket. Users in general will use a hash
	/// table iterator much more often, as it is much like other container
	/// iterators (e.g. vector::iterator).
	///
	/// We define a base class here because it is shared by both const and
	/// non-const iterators.
	///
	template <typename Value>
	struct intrusive_hashtable_iterator_base
	{
	public:
		typedef Value value_type;

	protected:
		template <typename, typename, typename, typename, size_t, bool, bool>
		friend class intrusive_hashtable;

		template <typename, bool>
		friend struct intrusive_hashtable_iterator;

		template <typename V>
		friend bool operator==(const intrusive_hashtable_iterator_base<V>&, const intrusive_hashtable_iterator_base<V>&);

		template <typename V>
		friend bool operator!=(const intrusive_hashtable_iterator_base<V>&, const intrusive_hashtable_iterator_base<V>&);

		value_type*  mpNode;      // Current node within current bucket.
		value_type** mpBucket;    // Current bucket.

	public:
		intrusive_hashtable_iterator_base(value_type* pNode, value_type** pBucket)
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
			mpNode = static_cast<value_type*>(mpNode->mpNext);

			while(mpNode == NULL)
				mpNode = *++mpBucket;
		}

	}; // intrusive_hashtable_iterator_base




	/// intrusive_hashtable_iterator
	///
	/// An intrusive_hashtable_iterator iterates the entire hash table and not 
	/// just nodes within a single bucket. Users in general will use a hash
	/// table iterator much more often, as it is much like other container
	/// iterators (e.g. vector::iterator).
	///
	/// The bConst parameter defines if the iterator is a const_iterator
	/// or an iterator.
	///
	template <typename Value, bool bConst>
	struct intrusive_hashtable_iterator : public intrusive_hashtable_iterator_base<Value>
	{
	public:
		typedef intrusive_hashtable_iterator_base<Value>                    base_type;
		typedef intrusive_hashtable_iterator<Value, bConst>                 this_type;
		typedef intrusive_hashtable_iterator<Value, false>                  this_type_non_const;
		typedef typename base_type::value_type                              value_type;
		typedef typename type_select<bConst, const Value*, Value*>::type    pointer;
		typedef typename type_select<bConst, const Value&, Value&>::type    reference;
		typedef ptrdiff_t                                                   difference_type;
		typedef EASTL_ITC_NS::forward_iterator_tag                          iterator_category;

	public:
		intrusive_hashtable_iterator()
			: base_type(NULL, NULL) { }

		explicit intrusive_hashtable_iterator(value_type* pNode, value_type** pBucket)
			: base_type(pNode, pBucket) { }

		explicit intrusive_hashtable_iterator(value_type** pBucket)
			: base_type(*pBucket, pBucket) { }

		intrusive_hashtable_iterator(const this_type_non_const& x)
			: base_type(x.mpNode, x.mpBucket) { }

		reference operator*() const
			{ return *base_type::mpNode; }

		pointer operator->() const
			{ return base_type::mpNode; }

		this_type& operator++()
			{ base_type::increment(); return *this; }

		this_type operator++(int)
			{ this_type temp(*this); base_type::increment(); return temp; }

	}; // intrusive_hashtable_iterator



	/// use_intrusive_key
	///
	/// operator()(x) returns x.mKey. Used in maps, as opposed to sets.
	/// This is a template policy implementation; it is an alternative to 
	/// the use_self template implementation, which is used for sets.
	///
	template <typename Node, typename Key>
	struct use_intrusive_key // : public unary_function<T, T> // Perhaps we want to make it a subclass of unary_function.
	{
		typedef Key result_type;

		const result_type& operator()(const Node& x) const
			{ return x.mKey; }
	};



	///////////////////////////////////////////////////////////////////////////
	/// intrusive_hashtable
	///
	template <typename Key, typename Value, typename Hash, typename Equal, 
			  size_t bucketCount, bool bConstIterators, bool bUniqueKeys>
	class intrusive_hashtable
	{
	public:
		typedef intrusive_hashtable<Key, Value, Hash, Equal, 
									bucketCount, bConstIterators, bUniqueKeys>            this_type;
		typedef Key                                                                       key_type;
		typedef Value                                                                     value_type;
		typedef Value                                                                     mapped_type;
		typedef Value                                                                     node_type;
		typedef uint32_t                                                                  hash_code_t;
		typedef Equal                                                                     key_equal;
		typedef ptrdiff_t                                                                 difference_type;
		typedef eastl_size_t                                                              size_type;     // See config.h for the definition of eastl_size_t, which defaults to size_t.
		typedef value_type&                                                               reference;
		typedef const value_type&                                                         const_reference;
		typedef intrusive_node_iterator<value_type, bConstIterators>                      local_iterator;
		typedef intrusive_node_iterator<value_type, true>                                 const_local_iterator;
		typedef intrusive_hashtable_iterator<value_type, bConstIterators>                 iterator;
		typedef intrusive_hashtable_iterator<value_type, true>                            const_iterator;
		typedef typename type_select<bUniqueKeys, pair<iterator, bool>, iterator>::type   insert_return_type;
		typedef typename type_select<bConstIterators, eastl::use_self<Value>, 
												 eastl::use_intrusive_key<Value, key_type> >::type  extract_key;

		enum
		{
			kBucketCount = bucketCount
		};

	protected:
		node_type* mBucketArray[kBucketCount + 1]; // '+1' because we have an end bucket which is non-NULL so iterators always stop on it.
		size_type  mnElementCount;
		Hash       mHash;           // To do: Use base class optimization to make this go away when it is of zero size.
		Equal      mEqual;          // To do: Use base class optimization to make this go away when it is of zero size.

	public:
		intrusive_hashtable(const Hash&, const Equal&);

		void swap(this_type& x);

		iterator begin() EA_NOEXCEPT
		{
			iterator i(mBucketArray);
			if(!i.mpNode)
				i.increment_bucket();
			return i;
		}

		const_iterator begin() const EA_NOEXCEPT
		{
			const_iterator i(const_cast<node_type**>(mBucketArray));
			if(!i.mpNode)
				i.increment_bucket();
			return i;
		}

		const_iterator cbegin() const EA_NOEXCEPT
		{
			return begin();
		}

		iterator end() EA_NOEXCEPT
			{ return iterator(mBucketArray + kBucketCount); }

		const_iterator end() const EA_NOEXCEPT
			{ return const_iterator(const_cast<node_type**>(mBucketArray) + kBucketCount); }

		const_iterator cend() const EA_NOEXCEPT
			{ return const_iterator(const_cast<node_type**>(mBucketArray) + kBucketCount); }

		local_iterator begin(size_type n) EA_NOEXCEPT
			{ return local_iterator(mBucketArray[n]); }

		const_local_iterator begin(size_type n) const EA_NOEXCEPT
			{ return const_local_iterator(mBucketArray[n]); }

		const_local_iterator cbegin(size_type n) const EA_NOEXCEPT
			{ return const_local_iterator(mBucketArray[n]); }

		local_iterator end(size_type) EA_NOEXCEPT
			{ return local_iterator(NULL); }

		const_local_iterator end(size_type) const EA_NOEXCEPT
			{ return const_local_iterator(NULL); }

		const_local_iterator cend(size_type) const EA_NOEXCEPT
			{ return const_local_iterator(NULL); }

		size_type size() const EA_NOEXCEPT
			{ return mnElementCount; }

		bool empty() const EA_NOEXCEPT
			{ return mnElementCount == 0; }

		size_type bucket_count() const  EA_NOEXCEPT // This function is unnecessary, as the user can directly reference
			{ return kBucketCount; }                // intrusive_hashtable::kBucketCount as a constant.

		size_type bucket_size(size_type n) const EA_NOEXCEPT
			{ return (size_type)eastl::distance(begin(n), end(n)); }

		size_type bucket(const key_type& k) const EA_NOEXCEPT
			{ return (size_type)(mHash(k) % kBucketCount); }

	public:
		float load_factor() const EA_NOEXCEPT
			{ return (float)mnElementCount / (float)kBucketCount; }

	public:
		insert_return_type insert(value_type& value) 
			{ return DoInsertValue(value, integral_constant<bool, bUniqueKeys>()); }

		insert_return_type insert(const_iterator, value_type& value)
			{ return insert(value); } // To consider: We might be able to use the iterator argument to specify a specific insertion location.

		template <typename InputIterator>
		void insert(InputIterator first, InputIterator last);

	public:
		iterator  erase(const_iterator position);
		iterator  erase(const_iterator first, const_iterator last);
		size_type erase(const key_type& k);
		iterator  remove(value_type& value);            // Removes by value instead of by iterator. This is an O(1) operation, due to this hashtable being 'intrusive'.

		void clear();

	public:
		iterator       find(const key_type& k);
		const_iterator find(const key_type& k) const;

		/// Implements a find whereby the user supplies a comparison of a different type
		/// than the hashtable value_type. A useful case of this is one whereby you have
		/// a container of string objects but want to do searches via passing in char pointers.
		/// The problem is that without this kind of find, you need to do the expensive operation
		/// of converting the char pointer to a string so it can be used as the argument to the 
		/// find function.
		///
		/// Example usage:
		///     hash_set<string> hashSet;
		///     hashSet.find_as("hello");    // Use default hash and compare.
		///
		/// Example usage (namespaces omitted for brevity):
		///     hash_set<string> hashSet;
		///     hashSet.find_as("hello", hash<char*>(), equal_to_2<string, char*>());
		///
		template <typename U, typename UHash, typename BinaryPredicate>
		iterator       find_as(const U& u, UHash uhash, BinaryPredicate predicate);

		template <typename U, typename UHash, typename BinaryPredicate>
		const_iterator find_as(const U& u, UHash uhash, BinaryPredicate predicate) const;

		template <typename U>
		iterator       find_as(const U& u);

		template <typename U>
		const_iterator find_as(const U& u) const;

		size_type      count(const key_type& k) const;

		// The use for equal_range in a hash_table seems somewhat questionable.
		// The primary reason for its existence is to replicate the interface of set/map.
		eastl::pair<iterator, iterator>             equal_range(const key_type& k);
		eastl::pair<const_iterator, const_iterator> equal_range(const key_type& k) const;

	public:
		bool validate() const;
		int  validate_iterator(const_iterator i) const;

	public:
		Hash hash_function() const
			{ return mHash; }

		Equal equal_function() const    // Deprecated. Use key_eq() instead, as key_eq is what the new C++ standard 
			{ return mEqual; }          // has specified in its hashtable (unordered_*) proposal.

		const key_equal& key_eq() const 
			{ return mEqual; }

		key_equal& key_eq()
			{ return mEqual; }

	protected:
		eastl::pair<iterator, bool> DoInsertValue(value_type&, true_type);  // true_type means bUniqueKeys is true.
		iterator                    DoInsertValue(value_type&, false_type); // false_type means bUniqueKeys is false.

		node_type* DoFindNode(node_type* pNode, const key_type& k) const;

		template <typename U, typename BinaryPredicate>
		node_type* DoFindNode(node_type* pNode, const U& u, BinaryPredicate predicate) const;

	}; // class intrusive_hashtable





	///////////////////////////////////////////////////////////////////////
	// node_iterator_base
	///////////////////////////////////////////////////////////////////////

	template <typename Value, bool bConst>
	inline bool operator==(const intrusive_node_iterator<Value, bConst>& a, 
						   const intrusive_node_iterator<Value, bConst>& b)
		{ return a.mpNode == b.mpNode; }

	template <typename Value, bool bConst>
	inline bool operator!=(const intrusive_node_iterator<Value, bConst>& a,
						   const intrusive_node_iterator<Value, bConst>& b)
		{ return a.mpNode != b.mpNode; }




	///////////////////////////////////////////////////////////////////////
	// hashtable_iterator_base
	///////////////////////////////////////////////////////////////////////

	template <typename Value>
	inline bool operator==(const intrusive_hashtable_iterator_base<Value>& a,
						   const intrusive_hashtable_iterator_base<Value>& b)
		{ return a.mpNode == b.mpNode; }


	template <typename Value>
	inline bool operator!=(const intrusive_hashtable_iterator_base<Value>& a,
						   const intrusive_hashtable_iterator_base<Value>& b)
		{ return a.mpNode != b.mpNode; }




	///////////////////////////////////////////////////////////////////////
	// intrusive_hashtable
	///////////////////////////////////////////////////////////////////////

	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	inline intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::intrusive_hashtable(const H& h, const Eq& eq)
		: mnElementCount(0),
		  mHash(h),
		  mEqual(eq)
	{
		memset(mBucketArray, 0, kBucketCount * sizeof(mBucketArray[0]));
		mBucketArray[kBucketCount] = reinterpret_cast<node_type*>((uintptr_t)~0);
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	void intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::swap(this_type& x)
	{
		for(size_t i = 0; i < kBucketCount; i++)
			eastl::swap(mBucketArray[i], x.mBucketArray[i]);

		eastl::swap(mnElementCount, x.mnElementCount);
		eastl::swap(mHash,          x.mHash);
		eastl::swap(mEqual,         x.mEqual);
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	inline typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::iterator
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::find(const key_type& k)
	{
		const size_type n = (size_type)(mHash(k) % kBucketCount);
		node_type* const pNode = DoFindNode(mBucketArray[n], k);
		return pNode ? iterator(pNode, mBucketArray + n) : iterator(mBucketArray + kBucketCount);
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	inline typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::const_iterator
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::find(const key_type& k) const
	{
		const size_type n = (size_type)(mHash(k) % kBucketCount);
		node_type* const pNode = DoFindNode(mBucketArray[n], k);
		return pNode ? const_iterator(pNode, const_cast<node_type**>(mBucketArray) + n) : const_iterator(const_cast<node_type**>(mBucketArray) + kBucketCount);
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	template <typename U, typename UHash, typename BinaryPredicate>
	inline typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::iterator
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::find_as(const U& other, UHash uhash, BinaryPredicate predicate)
	{
		const size_type n = (size_type)(uhash(other) % kBucketCount);
		node_type* const pNode = DoFindNode(mBucketArray[n], other, predicate);
		return pNode ? iterator(pNode, mBucketArray + n) : iterator(mBucketArray + kBucketCount);
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	template <typename U, typename UHash, typename BinaryPredicate>
	inline typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::const_iterator
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::find_as(const U& other, UHash uhash, BinaryPredicate predicate) const
	{
		const size_type n = (size_type)(uhash(other) % kBucketCount);
		node_type* const pNode = DoFindNode(mBucketArray[n], other, predicate);
		return pNode ? const_iterator(pNode, const_cast<node_type**>(mBucketArray) + n) : const_iterator(const_cast<node_type**>(mBucketArray) + kBucketCount);
	}


	/// intrusive_hashtable_find
	///
	/// Helper function that defaults to using hash<U> and equal_to_2<T, U>.
	/// This makes it so that by default you don't need to provide these.
	/// Note that the default hash functions may not be what you want, though.
	///
	/// Example usage. Instead of this:
	///     hash_set<string> hashSet;
	///     hashSet.find("hello", hash<char*>(), equal_to_2<string, char*>());
	///
	/// You can use this:
	///     hash_set<string> hashSet;
	///     hashtable_find(hashSet, "hello");
	///
	template <typename H, typename U>
	inline typename H::iterator intrusive_hashtable_find(H& hashTable, const U& u)
		{ return hashTable.find_as(u, eastl::hash<U>(), eastl::equal_to_2<const typename H::key_type, U>()); }

	template <typename H, typename U>
	inline typename H::const_iterator intrusive_hashtable_find(const H& hashTable, const U& u)
		{ return hashTable.find_as(u, eastl::hash<U>(), eastl::equal_to_2<const typename H::key_type, U>()); }



	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	template <typename U>
	inline typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::iterator
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::find_as(const U& other)
		{ return eastl::intrusive_hashtable_find(*this, other); }
		// VC++ doesn't appear to like the following, though it seems correct to me.
		// So we implement the workaround above until we can straighten this out.
		//{ return find_as(other, eastl::hash<U>(), eastl::equal_to_2<const key_type, U>()); }


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	template <typename U>
	inline typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::const_iterator
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::find_as(const U& other) const
		{ return eastl::intrusive_hashtable_find(*this, other); }
		// VC++ doesn't appear to like the following, though it seems correct to me.
		// So we implement the workaround above until we can straighten this out.
		//{ return find_as(other, eastl::hash<U>(), eastl::equal_to_2<const key_type, U>()); }


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::size_type
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::count(const key_type& k) const
	{
		const size_type n = (size_type)(mHash(k) % kBucketCount);
		size_type   result = 0;
		extract_key extractKey; // extract_key is empty and thus this ctor is a no-op.

		// To do: Make a specialization for bU (unique keys) == true and take 
		// advantage of the fact that the count will always be zero or one in that case. 
		for(node_type* pNode = mBucketArray[n]; pNode; pNode = static_cast<node_type*>(pNode->mpNext))
		{
			if(mEqual(k, extractKey(*pNode)))
				++result;
		}
		return result;
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	eastl::pair<typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::iterator,
				typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::iterator>
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::equal_range(const key_type& k)
	{
		const size_type n     = (size_type)(mHash(k) % kBucketCount);
		node_type**     head  = mBucketArray + n;
		node_type*      pNode = DoFindNode(*head, k);
		extract_key     extractKey; // extract_key is empty and thus this ctor is a no-op.

		if(pNode)
		{
			node_type* p1 = static_cast<node_type*>(pNode->mpNext);

			for(; p1; p1 = static_cast<node_type*>(p1->mpNext))
			{
				if(!mEqual(k, extractKey(*p1)))
					break;
			}

			iterator first(pNode, head);
			iterator last(p1, head);

			if(!p1)
				last.increment_bucket();

			return eastl::pair<iterator, iterator>(first, last);
		}

		return eastl::pair<iterator, iterator>(iterator(mBucketArray + kBucketCount),
											   iterator(mBucketArray + kBucketCount));
	}




	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	eastl::pair<typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::const_iterator,
				typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::const_iterator>
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::equal_range(const key_type& k) const
	{
		const size_type n     = (size_type)(mHash(k) % kBucketCount);
		node_type**     head  = const_cast<node_type**>(mBucketArray + n);
		node_type*      pNode = DoFindNode(*head, k);
		extract_key     extractKey; // extract_key is empty and thus this ctor is a no-op.

		if(pNode)
		{
			node_type* p1 = static_cast<node_type*>(pNode->mpNext);

			for(; p1; p1 = static_cast<node_type*>(p1->mpNext))
			{
				if(!mEqual(k, extractKey(*p1)))
					break;
			}

			const_iterator first(pNode, head);
			const_iterator last(p1, head);

			if(!p1)
				last.increment_bucket();

			return eastl::pair<const_iterator, const_iterator>(first, last);
		}

		return eastl::pair<const_iterator, const_iterator>(const_iterator(const_cast<node_type**>(mBucketArray) + kBucketCount),
														   const_iterator(const_cast<node_type**>(mBucketArray) + kBucketCount));
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	inline typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::node_type* 
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::DoFindNode(node_type* pNode, const key_type& k) const
	{
		extract_key extractKey; // extract_key is empty and thus this ctor is a no-op.

		for(; pNode; pNode = static_cast<node_type*>(pNode->mpNext))
		{
			if(mEqual(k, extractKey(*pNode)))
				return pNode;
		}
		return NULL;
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	template <typename U, typename BinaryPredicate>
	inline typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::node_type* 
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::DoFindNode(node_type* pNode, const U& other, BinaryPredicate predicate) const
	{
		extract_key extractKey; // extract_key is empty and thus this ctor is a no-op.

		for(; pNode; pNode = static_cast<node_type*>(pNode->mpNext))
		{
			if(predicate(extractKey(*pNode), other)) // Intentionally compare with key as first arg and other as second arg.
				return pNode;
		}
		return NULL;
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	eastl::pair<typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::iterator, bool>
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::DoInsertValue(value_type& value, true_type) // true_type means bUniqueKeys is true.
	{
		// For sets (as opposed to maps), one could argue that all insertions are successful,
		// as all elements are unique. However, the equal function might not think so.
		extract_key      extractKey; // extract_key is empty and thus this ctor is a no-op.
		const size_type  n     = (size_type)(mHash(extractKey(value)) % kBucketCount);
		node_type* const pNode = DoFindNode(mBucketArray[n], extractKey(value));

		if(pNode == NULL)
		{
			value.mpNext = mBucketArray[n];
			mBucketArray[n] = &value;
			++mnElementCount;

			return eastl::pair<iterator, bool>(iterator(&value, mBucketArray + n), true);
		}

		return eastl::pair<iterator, bool>(iterator(pNode, mBucketArray + n), false);
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::iterator
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::DoInsertValue(value_type& value, false_type) // false_type means bUniqueKeys is false.
	{
		extract_key      extractKey; // extract_key is empty and thus this ctor is a no-op.
		const size_type  n         = (size_type)(mHash(extractKey(value)) % kBucketCount);
		node_type* const pNodePrev = DoFindNode(mBucketArray[n], extractKey(value));

		if(pNodePrev == NULL)
		{
			value.mpNext    = mBucketArray[n];
			mBucketArray[n] = &value;
		}
		else
		{
			value.mpNext      = pNodePrev->mpNext;
			pNodePrev->mpNext = &value;
		}

		++mnElementCount;

		return iterator(&value, mBucketArray + n);
	}



	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	template <typename InputIterator>
	inline void intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::insert(InputIterator first, InputIterator last)
	{
		for(; first != last; ++first)
			insert(*first);
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::iterator
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::erase(const_iterator i)
	{
		iterator iNext(i.mpNode, i.mpBucket);
		++iNext;

		node_type* pNode        =  i.mpNode;
		node_type* pNodeCurrent = *i.mpBucket;

		if(pNodeCurrent == pNode)
			*i.mpBucket = static_cast<node_type*>(pNodeCurrent->mpNext);
		else
		{
			// We have a singly-linked list, so we have no choice but to
			// walk down it till we find the node before the node at 'i'.
			node_type* pNodeNext = static_cast<node_type*>(pNodeCurrent->mpNext);

			while(pNodeNext != pNode)
			{
				pNodeCurrent = pNodeNext;
				pNodeNext    = static_cast<node_type*>(pNodeCurrent->mpNext);
			}

			pNodeCurrent->mpNext = static_cast<node_type*>(pNodeNext->mpNext);
		}

		// To consider: In debug builds set the node mpNext to NULL.
		--mnElementCount;

		return iNext;
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	inline typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::iterator
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::erase(const_iterator first, const_iterator last)
	{
		while(first != last)
			first = erase(first);
		return iterator(first.mpNode, first.mpBucket);
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::size_type 
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::erase(const key_type& k)
	{
		const size_type n = (size_type)(mHash(k) % kBucketCount);
		const size_type nElementCountSaved = mnElementCount;
		node_type*&     pNodeBase = mBucketArray[n];
		extract_key     extractKey; // extract_key is empty and thus this ctor is a no-op.

		// Note by Paul Pedriana:
		// We have two loops here, and I'm not finding any easy way to having just one
		// loop without changing the requirements of the hashtable node definition. 
		// It's a problem of taking an address of a variable and converting it to the 
		// address of another type without knowing what that type is. Perhaps I'm a 
		// little overly tired, so if there is a simple solution I am probably missing it.

		while(pNodeBase && mEqual(k, extractKey(*pNodeBase)))
		{
			pNodeBase = static_cast<node_type*>(pNodeBase->mpNext);
			--mnElementCount;
		}

		node_type* pNodePrev = pNodeBase;

		if(pNodePrev)
		{
			node_type* pNodeCur;

			while((pNodeCur = static_cast<node_type*>(pNodePrev->mpNext)) != NULL)
			{
				if(mEqual(k, extractKey(*pNodeCur)))
				{
					pNodePrev->mpNext = static_cast<node_type*>(pNodeCur->mpNext);
					--mnElementCount; // To consider: In debug builds set the node mpNext to NULL.
				}
				else
					pNodePrev = static_cast<node_type*>(pNodePrev->mpNext);
			}
		}

		return nElementCountSaved - mnElementCount;
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	inline typename intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::iterator
	intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::remove(value_type& value)
	{
		extract_key     extractKey; // extract_key is empty and thus this ctor is a no-op.
		const size_type n = (size_type)(mHash(extractKey(value)) % kBucketCount);

		return erase(iterator(&value, &mBucketArray[n]));
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	inline void intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::clear()
	{
		// To consider: In debug builds set the node mpNext to NULL.
		memset(mBucketArray, 0, kBucketCount * sizeof(mBucketArray[0]));
		mnElementCount = 0;
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	inline bool intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::validate() const
	{
		// Verify that the element count matches mnElementCount. 
		size_type nElementCount = 0;

		for(const_iterator temp = begin(), tempEnd = end(); temp != tempEnd; ++temp)
			++nElementCount;

		if(nElementCount != mnElementCount)
			return false;

		// To do: Verify that individual elements are in the expected buckets.

		return true;
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	int intrusive_hashtable<K, V, H, Eq, bC, bM, bU>::validate_iterator(const_iterator i) const
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

	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	inline bool operator==(const intrusive_hashtable<K, V, H, Eq, bC, bM, bU>& a, 
						   const intrusive_hashtable<K, V, H, Eq, bC, bM, bU>& b)
	{
		return (a.size() == b.size()) && eastl::equal(a.begin(), a.end(), b.begin());
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	inline bool operator!=(const intrusive_hashtable<K, V, H, Eq, bC, bM, bU>& a, 
						   const intrusive_hashtable<K, V, H, Eq, bC, bM, bU>& b)
	{
		return !(a == b);
	}


	// Comparing hash tables for less-ness is an odd thing to do. We provide it for 
	// completeness, though the user is advised to be wary of how they use this.
	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	inline bool operator<(const intrusive_hashtable<K, V, H, Eq, bC, bM, bU>& a, 
						  const intrusive_hashtable<K, V, H, Eq, bC, bM, bU>& b)
	{
		// This requires hash table elements to support operator<. Since the hash table
		// doesn't compare elements via less (it does so via equals), we must use the 
		// globally defined operator less for the elements.
		return eastl::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	inline bool operator>(const intrusive_hashtable<K, V, H, Eq, bC, bM, bU>& a, 
						  const intrusive_hashtable<K, V, H, Eq, bC, bM, bU>& b)
	{
		return b < a;
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	inline bool operator<=(const intrusive_hashtable<K, V, H, Eq, bC, bM, bU>& a, 
						   const intrusive_hashtable<K, V, H, Eq, bC, bM, bU>& b)
	{
		return !(b < a);
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	inline bool operator>=(const intrusive_hashtable<K, V, H, Eq, bC, bM, bU>& a, 
						   const intrusive_hashtable<K, V, H, Eq, bC, bM, bU>& b)
	{
		return !(a < b);
	}


	template <typename K, typename V, typename H, typename Eq, size_t bC, bool bM, bool bU>
	inline void swap(const intrusive_hashtable<K, V, H, Eq, bC, bM, bU>& a, 
					 const intrusive_hashtable<K, V, H, Eq, bC, bM, bU>& b)
	{
		a.swap(b);
	}


} // namespace eastl



#endif // Header include guard
