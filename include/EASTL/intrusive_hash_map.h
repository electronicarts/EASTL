///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_INTRUSIVE_HASH_MAP_H
#define EASTL_INTRUSIVE_HASH_MAP_H


#include <EASTL/internal/config.h>
#include <EASTL/internal/intrusive_hashtable.h>
#include <EASTL/functional.h>
#include <EASTL/utility.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// intrusive_hash_map
	///
	/// Template parameters:
	///     Key             The key object (key in the key/value pair). T must contain a member of type Key named mKey.
	///     T               The type of object the map holds (a.k.a. value).
	///     bucketCount     The number of buckets to use. Best if it's a prime number.
	///     Hash            Hash function. See functional.h for examples of hash functions.
	///     Equal           Equality testing predicate; tells if two elements are equal.
	///
	template <typename Key, typename T, size_t bucketCount, typename Hash = eastl::hash<Key>, typename Equal = eastl::equal_to<Key> >
	class intrusive_hash_map : public intrusive_hashtable<Key, T, Hash, Equal, bucketCount, false, true>
	{
	public:
		typedef intrusive_hashtable<Key, T, Hash, Equal, bucketCount, false, true>  base_type;
		typedef intrusive_hash_map<Key, T, bucketCount, Hash, Equal>                this_type;

	public:
		explicit intrusive_hash_map(const Hash& h = Hash(), const Equal& eq = Equal())
			: base_type(h, eq)
		{
			// Empty
		}

		// To consider: Is this feasible, given how initializer_list works by creating a temporary array? Even if it is feasible, is it a good idea?
		//intrusive_hash_map(std::initializer_list<value_type> ilist);

	}; // intrusive_hash_map




	/// intrusive_hash_multimap
	///
	/// Implements a intrusive_hash_multimap, which is the same thing as a intrusive_hash_map 
	/// except that contained elements need not be unique. See the documentation 
	/// for intrusive_hash_map for details.
	///
	/// Template parameters:
	///     Key             The key object (key in the key/value pair). T must contain a member of type Key named mKey.
	///     T               The type of object the map holds (a.k.a. value).
	///     bucketCount     The number of buckets to use. Best if it's a prime number.
	///     Hash            Hash function. See functional.h for examples of hash functions.
	///     Equal           Equality testing predicate; tells if two elements are equal.
	///
	template <typename Key, typename T, size_t bucketCount, typename Hash = eastl::hash<Key>, typename Equal = eastl::equal_to<Key> >
	class intrusive_hash_multimap : public intrusive_hashtable<Key, T, Hash, Equal, bucketCount, false, false>
	{
	public:
		typedef intrusive_hashtable<Key, T, Hash, Equal, bucketCount, false, false>  base_type;
		typedef intrusive_hash_multimap<Key, T, bucketCount, Hash, Equal>            this_type;

	public:
		explicit intrusive_hash_multimap(const Hash& h = Hash(), const Equal& eq = Equal())
			: base_type(h, eq)
		{
			// Empty
		}

		// To consider: Is this feasible, given how initializer_list works by creating a temporary array? Even if it is feasible, is it a good idea?
		//intrusive_hash_multimap(std::initializer_list<value_type> ilist);

	}; // intrusive_hash_multimap




} // namespace eastl


#endif // Header include guard






