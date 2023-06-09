///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file is based on the TR1 (technical report 1) reference implementation
// of the unordered_set/unordered_map C++ classes as of about 4/2005. Most likely
// many or all C++ library vendors' implementations of this classes will be
// based off of the reference version and so will look pretty similar to this
// file as well as other vendors' versions.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_HASH_MAP_H
#define EASTL_HASH_MAP_H


#include <EASTL/internal/config.h>
#include <EASTL/internal/hashtable.h>
#include <EASTL/functional.h>
#include <EASTL/utility.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// EASTL_HASH_MAP_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_HASH_MAP_DEFAULT_NAME
		#define EASTL_HASH_MAP_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " hash_map" // Unless the user overrides something, this is "EASTL hash_map".
	#endif


	/// EASTL_HASH_MULTIMAP_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_HASH_MULTIMAP_DEFAULT_NAME
		#define EASTL_HASH_MULTIMAP_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " hash_multimap" // Unless the user overrides something, this is "EASTL hash_multimap".
	#endif


	/// EASTL_HASH_MAP_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_HASH_MAP_DEFAULT_ALLOCATOR
		#define EASTL_HASH_MAP_DEFAULT_ALLOCATOR allocator_type(EASTL_HASH_MAP_DEFAULT_NAME)
	#endif

	/// EASTL_HASH_MULTIMAP_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_HASH_MULTIMAP_DEFAULT_ALLOCATOR
		#define EASTL_HASH_MULTIMAP_DEFAULT_ALLOCATOR allocator_type(EASTL_HASH_MULTIMAP_DEFAULT_NAME)
	#endif



	/// hash_map
	///
	/// Implements a hash_map, which is a hashed associative container.
	/// Lookups are O(1) (that is, they are fast) but the container is
	/// not sorted. Note that lookups are only O(1) if the hash table
	/// is well-distributed (non-colliding). The lookup approaches
	/// O(n) behavior as the table becomes increasingly poorly distributed.
	///
	/// set_max_load_factor
	/// If you want to make a hashtable never increase its bucket usage,
	/// call set_max_load_factor with a very high value such as 100000.f.
	///
	/// bCacheHashCode
	/// We provide the boolean bCacheHashCode template parameter in order
	/// to allow the storing of the hash code of the key within the map.
	/// When this option is disabled, the rehashing of the table will
	/// call the hash function on the key. Setting bCacheHashCode to true
	/// is useful for cases whereby the calculation of the hash value for
	/// a contained object is very expensive.
	///
	/// find_as
	/// In order to support the ability to have a hashtable of strings but
	/// be able to do efficiently lookups via char pointers (i.e. so they
	/// aren't converted to string objects), we provide the find_as
	/// function. This function allows you to do a find with a key of a
	/// type other than the hashtable key type.
	///
	/// Example find_as usage:
	///     hash_map<string, int> hashMap;
	///     i = hashMap.find_as("hello");    // Use default hash and compare.
	///
	/// Example find_as usage (namespaces omitted for brevity):
	///     hash_map<string, int> hashMap;
	///     i = hashMap.find_as("hello", hash<char*>(), equal_to<>());
	///
	template <typename Key, typename T, typename Hash = eastl::hash<Key>, typename Predicate = eastl::equal_to<Key>,
			  typename Allocator = EASTLAllocatorType, bool bCacheHashCode = false>
	class hash_map
		: public hashtable<Key, eastl::pair<const Key, T>, Allocator, eastl::use_first<eastl::pair<const Key, T> >, Predicate,
							Hash, mod_range_hashing, default_ranged_hash, prime_rehash_policy, bCacheHashCode, true, true>
	{
	public:
		typedef hashtable<Key, eastl::pair<const Key, T>, Allocator,
						  eastl::use_first<eastl::pair<const Key, T> >,
						  Predicate, Hash, mod_range_hashing, default_ranged_hash,
						  prime_rehash_policy, bCacheHashCode, true, true>        base_type;
		typedef hash_map<Key, T, Hash, Predicate, Allocator, bCacheHashCode>      this_type;
		typedef typename base_type::size_type                                     size_type;
		typedef typename base_type::key_type                                      key_type;
		typedef T                                                                 mapped_type;
		typedef typename base_type::value_type                                    value_type;     // NOTE: 'value_type = pair<const key_type, mapped_type>'.
		typedef typename base_type::allocator_type                                allocator_type;
		typedef typename base_type::node_type                                     node_type;
		typedef typename base_type::insert_return_type                            insert_return_type;
		typedef typename base_type::iterator                                      iterator;
		typedef typename base_type::const_iterator                                const_iterator;

		using base_type::insert;

	public:
		/// hash_map
		///
		/// Default constructor.
		///
		hash_map()
			: this_type(EASTL_HASH_MAP_DEFAULT_ALLOCATOR)
		{
			// Empty
		}


		/// hash_map
		///
		/// Constructor which creates an empty container with allocator.
		///
		explicit hash_map(const allocator_type& allocator)
			: base_type(0, Hash(), mod_range_hashing(), default_ranged_hash(),
						Predicate(), eastl::use_first<eastl::pair<const Key, T> >(), allocator)
		{
			// Empty
		}


		/// hash_map
		///
		/// Constructor which creates an empty container, but start with nBucketCount buckets.
		/// We default to a small nBucketCount value, though the user really should manually
		/// specify an appropriate value in order to prevent memory from being reallocated.
		///
		explicit hash_map(size_type nBucketCount, const Hash& hashFunction = Hash(),
						  const Predicate& predicate = Predicate(), const allocator_type& allocator = EASTL_HASH_MAP_DEFAULT_ALLOCATOR)
			: base_type(nBucketCount, hashFunction, mod_range_hashing(), default_ranged_hash(),
						predicate, eastl::use_first<eastl::pair<const Key, T> >(), allocator)
		{
			// Empty
		}


		hash_map(const this_type& x)
		  : base_type(x)
		{
		}


		hash_map(this_type&& x)
		  : base_type(eastl::move(x))
		{
		}


		hash_map(this_type&& x, const allocator_type& allocator)
		  : base_type(eastl::move(x), allocator)
		{
		}


		/// hash_map
		///
		/// initializer_list-based constructor.
		/// Allows for initializing with brace values (e.g. hash_map<int, char*> hm = { {3,"c"}, {4,"d"}, {5,"e"} }; )
		///
		hash_map(std::initializer_list<value_type> ilist, size_type nBucketCount = 0, const Hash& hashFunction = Hash(),
				   const Predicate& predicate = Predicate(), const allocator_type& allocator = EASTL_HASH_MAP_DEFAULT_ALLOCATOR)
			: base_type(ilist.begin(), ilist.end(), nBucketCount, hashFunction, mod_range_hashing(), default_ranged_hash(),
						predicate, eastl::use_first<eastl::pair<const Key, T> >(), allocator)
		{
			// Empty
		}


		/// hash_map
		///
		/// An input bucket count of <= 1 causes the bucket count to be equal to the number of
		/// elements in the input range.
		///
		template <typename ForwardIterator>
		hash_map(ForwardIterator first, ForwardIterator last, size_type nBucketCount = 0, const Hash& hashFunction = Hash(),
				 const Predicate& predicate = Predicate(), const allocator_type& allocator = EASTL_HASH_MAP_DEFAULT_ALLOCATOR)
			: base_type(first, last, nBucketCount, hashFunction, mod_range_hashing(), default_ranged_hash(),
						predicate, eastl::use_first<eastl::pair<const Key, T> >(), allocator)
		{
			// Empty
		}


		this_type& operator=(const this_type& x)
		{
			return static_cast<this_type&>(base_type::operator=(x));
		}


		this_type& operator=(std::initializer_list<value_type> ilist)
		{
			return static_cast<this_type&>(base_type::operator=(ilist));
		}


		this_type& operator=(this_type&& x)
		{
			return static_cast<this_type&>(base_type::operator=(eastl::move(x)));
		}


		/// insert
		///
		/// This is an extension to the C++ standard. We insert a default-constructed
		/// element with the given key. The reason for this is that we can avoid the
		/// potentially expensive operation of creating and/or copying a mapped_type
		/// object on the stack.
		insert_return_type insert(const key_type& key)
		{
			return base_type::DoInsertKey(true_type(), key);
		}

		T& at(const key_type& k)
		{
			iterator it = base_type::find(k);

			if (it == base_type::end())
			{
				#if EASTL_EXCEPTIONS_ENABLED
					// throw exeption if exceptions enabled
					throw std::out_of_range("invalid hash_map<K, T> key");
				#else
					// assert false if asserts enabled
					EASTL_ASSERT_MSG(false, "invalid hash_map<K, T> key");
				#endif
			}
			// undefined behaviour if exceptions and asserts are disabled and it == end()
			return it->second;
		}


		const T& at(const key_type& k) const
		{
			const_iterator it = base_type::find(k);

			if (it == base_type::end())
			{
				#if EASTL_EXCEPTIONS_ENABLED
					// throw exeption if exceptions enabled
					throw std::out_of_range("invalid hash_map<K, T> key");
				#else
					// assert false if asserts enabled
					EASTL_ASSERT_MSG(false, "invalid hash_map<K, T> key");
				#endif
			}
			// undefined behaviour if exceptions and asserts are disabled and it == end()
			return it->second;
		}


		insert_return_type insert(key_type&& key)
		{
			return base_type::DoInsertKey(true_type(), eastl::move(key));
		}


		mapped_type& operator[](const key_type& key)
		{
			return (*base_type::DoInsertKey(true_type(), key).first).second;

			// Slower reference version:
			//const typename base_type::iterator it = base_type::find(key);
			//if(it != base_type::end())
			//    return (*it).second;
			//return (*base_type::insert(value_type(key, mapped_type())).first).second;
		}

		mapped_type& operator[](key_type&& key)
		{
			// The Standard states that this function "inserts the value value_type(std::move(key), mapped_type())"
			return (*base_type::DoInsertKey(true_type(), eastl::move(key)).first).second;
		}

		// try_emplace API added in C++17
		template <class... Args>
		inline insert_return_type try_emplace(const key_type& k, Args&&... args)
		{
			return try_emplace_forwarding(k, eastl::forward<Args>(args)...);
		}

		template <class... Args>
		inline insert_return_type try_emplace(key_type&& k, Args&&... args) {
			return try_emplace_forwarding(eastl::move(k), eastl::forward<Args>(args)...);
		}

		template <class... Args>
		inline iterator try_emplace(const_iterator, const key_type& k, Args&&... args) {
			// Currently, the first parameter is ignored.
			insert_return_type result = try_emplace(k, eastl::forward<Args>(args)...);
			return base_type::DoGetResultIterator(true_type(), result);
		}

		template <class... Args>
		inline iterator try_emplace(const_iterator, key_type&& k, Args&&... args) {
			// Currently, the first parameter is ignored.
			insert_return_type result = try_emplace(eastl::move(k), eastl::forward<Args>(args)...);
			return base_type::DoGetResultIterator(true_type(), result);
		}

	private:
		template <class K, class... Args>
		insert_return_type try_emplace_forwarding(K&& k, Args&&... args)
		{
			const auto key_data = base_type::DoFindKeyData(k);
			if (key_data.node)
			{ // Node exists, no insertion needed.
				return eastl::pair<iterator, bool>(
				    iterator(key_data.node, base_type::mpBucketArray + key_data.bucket_index), false);
			}
			else
			{
				node_type* const pNodeNew =
				    base_type::DoAllocateNode(piecewise_construct, eastl::forward_as_tuple(eastl::forward<K>(k)),
				                              forward_as_tuple(eastl::forward<Args>(args)...));
				// the key might have been moved from above, so we can't use `k` anymore.
				const auto& key = base_type::mExtractKey(pNodeNew->mValue);
				return base_type::template DoInsertUniqueNode<true>(key, key_data.code, key_data.bucket_index, pNodeNew);
			}
		}
	}; // hash_map

	/// hash_map erase_if
	///
	/// https://en.cppreference.com/w/cpp/container/unordered_map/erase_if
	template <typename Key, typename T, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode, typename UserPredicate>
	typename eastl::hash_map<Key, T, Hash, Predicate, Allocator, bCacheHashCode>::size_type erase_if(eastl::hash_map<Key, T, Hash, Predicate, Allocator, bCacheHashCode>& c, UserPredicate predicate)
	{
		auto oldSize = c.size();
		// Erases all elements that satisfy the predicate from the container.
		for (auto i = c.begin(), last = c.end(); i != last;)
		{
			if (predicate(*i))
			{
				i = c.erase(i);
			}
			else
			{
				++i;
			}
		}
		return oldSize - c.size();
	}


	/// hash_multimap
	///
	/// Implements a hash_multimap, which is the same thing as a hash_map
	/// except that contained elements need not be unique. See the
	/// documentation for hash_set for details.
	///
	template <typename Key, typename T, typename Hash = eastl::hash<Key>, typename Predicate = eastl::equal_to<Key>,
			  typename Allocator = EASTLAllocatorType, bool bCacheHashCode = false>
	class hash_multimap
		: public hashtable<Key, eastl::pair<const Key, T>, Allocator, eastl::use_first<eastl::pair<const Key, T> >, Predicate,
						   Hash, mod_range_hashing, default_ranged_hash, prime_rehash_policy, bCacheHashCode, true, false>
	{
	public:
		typedef hashtable<Key, eastl::pair<const Key, T>, Allocator,
						  eastl::use_first<eastl::pair<const Key, T> >,
						  Predicate, Hash, mod_range_hashing, default_ranged_hash,
						  prime_rehash_policy, bCacheHashCode, true, false>           base_type;
		typedef hash_multimap<Key, T, Hash, Predicate, Allocator, bCacheHashCode>     this_type;
		typedef typename base_type::size_type                                         size_type;
		typedef typename base_type::key_type                                          key_type;
		typedef T                                                                     mapped_type;
		typedef typename base_type::value_type                                        value_type;     // Note that this is pair<const key_type, mapped_type>.
		typedef typename base_type::allocator_type                                    allocator_type;
		typedef typename base_type::node_type                                         node_type;
		typedef typename base_type::insert_return_type                                insert_return_type;
		typedef typename base_type::iterator                                          iterator;

		using base_type::insert;

	private:
		using base_type::insert_or_assign;

	public:
		/// hash_multimap
		///
		/// Default constructor.
		///
		explicit hash_multimap(const allocator_type& allocator = EASTL_HASH_MULTIMAP_DEFAULT_ALLOCATOR)
			: base_type(0, Hash(), mod_range_hashing(), default_ranged_hash(),
						Predicate(), eastl::use_first<eastl::pair<const Key, T> >(), allocator)
		{
			// Empty
		}


		/// hash_multimap
		///
		/// Constructor which creates an empty container, but start with nBucketCount buckets.
		/// We default to a small nBucketCount value, though the user really should manually
		/// specify an appropriate value in order to prevent memory from being reallocated.
		///
		explicit hash_multimap(size_type nBucketCount, const Hash& hashFunction = Hash(),
							   const Predicate& predicate = Predicate(), const allocator_type& allocator = EASTL_HASH_MULTIMAP_DEFAULT_ALLOCATOR)
			: base_type(nBucketCount, hashFunction, mod_range_hashing(), default_ranged_hash(),
						predicate, eastl::use_first<eastl::pair<const Key, T> >(), allocator)
		{
			// Empty
		}


		hash_multimap(const this_type& x)
		  : base_type(x)
		{
		}


		hash_multimap(this_type&& x)
		  : base_type(eastl::move(x))
		{
		}


		hash_multimap(this_type&& x, const allocator_type& allocator)
		  : base_type(eastl::move(x), allocator)
		{
		}


		/// hash_multimap
		///
		/// initializer_list-based constructor.
		/// Allows for initializing with brace values (e.g. hash_multimap<int, char*> hm = { {3,"c"}, {3,"C"}, {4,"d"} }; )
		///
		hash_multimap(std::initializer_list<value_type> ilist, size_type nBucketCount = 0, const Hash& hashFunction = Hash(),
				   const Predicate& predicate = Predicate(), const allocator_type& allocator = EASTL_HASH_MULTIMAP_DEFAULT_ALLOCATOR)
			: base_type(ilist.begin(), ilist.end(), nBucketCount, hashFunction, mod_range_hashing(), default_ranged_hash(),
						predicate, eastl::use_first<eastl::pair<const Key, T> >(), allocator)
		{
			// Empty
		}


		/// hash_multimap
		///
		/// An input bucket count of <= 1 causes the bucket count to be equal to the number of
		/// elements in the input range.
		///
		template <typename ForwardIterator>
		hash_multimap(ForwardIterator first, ForwardIterator last, size_type nBucketCount = 0, const Hash& hashFunction = Hash(),
					  const Predicate& predicate = Predicate(), const allocator_type& allocator = EASTL_HASH_MULTIMAP_DEFAULT_ALLOCATOR)
			: base_type(first, last, nBucketCount, hashFunction, mod_range_hashing(), default_ranged_hash(),
						predicate, eastl::use_first<eastl::pair<const Key, T> >(), allocator)
		{
			// Empty
		}


		this_type& operator=(const this_type& x)
		{
			return static_cast<this_type&>(base_type::operator=(x));
		}


		this_type& operator=(std::initializer_list<value_type> ilist)
		{
			return static_cast<this_type&>(base_type::operator=(ilist));
		}


		this_type& operator=(this_type&& x)
		{
			return static_cast<this_type&>(base_type::operator=(eastl::move(x)));
		}


		/// insert
		///
		/// This is an extension to the C++ standard. We insert a default-constructed
		/// element with the given key. The reason for this is that we can avoid the
		/// potentially expensive operation of creating and/or copying a mapped_type
		/// object on the stack.
		insert_return_type insert(const key_type& key)
		{
			return base_type::DoInsertKey(false_type(), key);
		}


		insert_return_type insert(key_type&& key)
		{
			return base_type::DoInsertKey(false_type(), eastl::move(key));
		}

	}; // hash_multimap

	/// hash_multimap erase_if
	///
	/// https://en.cppreference.com/w/cpp/container/unordered_multimap/erase_if
	template <typename Key, typename T, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode, typename UserPredicate>
	typename eastl::hash_multimap<Key, T, Hash, Predicate, Allocator, bCacheHashCode>::size_type erase_if(eastl::hash_multimap<Key, T, Hash, Predicate, Allocator, bCacheHashCode>& c, UserPredicate predicate)
	{
		auto oldSize = c.size();
		// Erases all elements that satisfy the predicate from the container.
		for (auto i = c.begin(), last = c.end(); i != last;)
		{
			if (predicate(*i))
			{
				i = c.erase(i);
			}
			else
			{
				++i;
			}
		}
		return oldSize - c.size();
	}



	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename Key, typename T, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
	inline bool operator==(const hash_map<Key, T, Hash, Predicate, Allocator, bCacheHashCode>& a,
						   const hash_map<Key, T, Hash, Predicate, Allocator, bCacheHashCode>& b)
	{
		typedef typename hash_map<Key, T, Hash, Predicate, Allocator, bCacheHashCode>::const_iterator const_iterator;

		// We implement branching with the assumption that the return value is usually false.
		if(a.size() != b.size())
			return false;

		// For map (with its unique keys), we need only test that each element in a can be found in b,
		// as there can be only one such pairing per element. multimap needs to do a something more elaborate.
		for(const_iterator ai = a.begin(), aiEnd = a.end(), biEnd = b.end(); ai != aiEnd; ++ai)
		{
			const_iterator bi = b.find(ai->first);

			if((bi == biEnd) || !(*ai == *bi))  // We have to compare the values, because lookups are done by keys alone but the full value_type of a map is a key/value pair.
				return false;                   // It's possible that two elements in the two containers have identical keys but different values.
		}

		return true;
	}

#if !defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	template <typename Key, typename T, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
	inline bool operator!=(const hash_map<Key, T, Hash, Predicate, Allocator, bCacheHashCode>& a,
						   const hash_map<Key, T, Hash, Predicate, Allocator, bCacheHashCode>& b)
	{
		return !(a == b);
	}
#endif

	template <typename Key, typename T, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
	inline bool operator==(const hash_multimap<Key, T, Hash, Predicate, Allocator, bCacheHashCode>& a,
						   const hash_multimap<Key, T, Hash, Predicate, Allocator, bCacheHashCode>& b)
	{
		typedef typename hash_multimap<Key, T, Hash, Predicate, Allocator, bCacheHashCode>::const_iterator const_iterator;
		typedef typename eastl::iterator_traits<const_iterator>::difference_type difference_type;

		// We implement branching with the assumption that the return value is usually false.
		if(a.size() != b.size())
			return false;

		// We can't simply search for each element of a in b, as it may be that the bucket for
		// two elements in a has those same two elements in b but in different order (which should
		// still result in equality). Also it's possible that one bucket in a has two elements which
		// both match a solitary element in the equivalent bucket in b (which shouldn't result in equality).
		eastl::pair<const_iterator, const_iterator> aRange;
		eastl::pair<const_iterator, const_iterator> bRange;

		for(const_iterator ai = a.begin(), aiEnd = a.end(); ai != aiEnd; ai = aRange.second) // For each element in a...
		{
			aRange = a.equal_range(ai->first); // Get the range of elements in a that are equal to ai.
			bRange = b.equal_range(ai->first); // Get the range of elements in b that are equal to ai.

			// We need to verify that aRange == bRange. First make sure the range sizes are equivalent...
			const difference_type aDistance = eastl::distance(aRange.first, aRange.second);
			const difference_type bDistance = eastl::distance(bRange.first, bRange.second);

			if(aDistance != bDistance)
				return false;

			// At this point, aDistance > 0 and aDistance == bDistance.
			// Implement a fast pathway for the case that there's just a single element.
			if(aDistance == 1)
			{
				if(!(*aRange.first == *bRange.first)) // We have to compare the values, because lookups are done by keys alone but the full value_type of a map is a key/value pair.
					return false;                     // It's possible that two elements in the two containers have identical keys but different values. Ditto for the permutation case below.
			}
			else
			{
				// Check to see if these aRange and bRange are any permutation of each other.
				// This check gets slower as there are more elements in the range.
				if(!eastl::is_permutation(aRange.first, aRange.second, bRange.first))
					return false;
			}
		}

		return true;
	}

#if !defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	template <typename Key, typename T, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
	inline bool operator!=(const hash_multimap<Key, T, Hash, Predicate, Allocator, bCacheHashCode>& a,
						   const hash_multimap<Key, T, Hash, Predicate, Allocator, bCacheHashCode>& b)
	{
		return !(a == b);
	}
#endif


} // namespace eastl


#endif // Header include guard
