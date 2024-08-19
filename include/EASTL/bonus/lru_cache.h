///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// lru_cache is a container that simplifies caching of objects in a map.
// Basically, you give the container a key, like a string, and the data you want.
// The container provides callback mechanisms to generate data if it's missing
// as well as delete data when it's purged from the cache.  This container
// uses a least recently used method: whatever the oldest item is will be 
// replaced with a new entry.
//
// Algorithmically, the container is a combination of a map and a list.
// The list stores the age of the entries by moving the entry to the head
// of the list on each access, either by a call to get() or to touch().
// The map is just the map as one would expect.
//
// This is useful for caching off data that is expensive to generate, 
// for example text to speech wave files that are dynamically generated,
// but that will need to be reused, as is the case in narration of menu
// entries as a user scrolls through the entries.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_LRUCACHE_H
#define EASTL_LRUCACHE_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
#pragma once
#endif

#include <EASTL/list.h>
#include <EASTL/unordered_map.h>
#include <EASTL/optional.h>
#include <EASTL/utility.h> // for pair
#include <EASTL/functional.h> // for function, hash, equal_to

namespace eastl
{
	/// EASTL_LRUCACHE_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_LRUCACHE_DEFAULT_NAME
	#define EASTL_LRUCACHE_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " lru_cache" // Unless the user overrides something, this is "EASTL lru_cache".
	#endif


	/// EASTL_LRUCACHE_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_LRUCACHE_DEFAULT_ALLOCATOR
	#define EASTL_LRUCACHE_DEFAULT_ALLOCATOR allocator_type(EASTL_LRUCACHE_DEFAULT_NAME)
	#endif

	/// lru_cache
	///
	/// Implements a caching map based off of a key and data.
	/// LRUList parameter is any container that guarantees the validity of its iterator even after a modification (e.g. list)
	/// LRUMap is any associative container that can map a key to some data.  By default, we use unordered_map, but it might be better
	/// to use hash_map or some other structure depending on your key/data combination.  For example, you may want to swap the 
	/// map backing if using strings as keys or if the data objects are small.  In any case, unordered_map is a good default and should
	/// work well enough since the purpose of this class is to cache results of expensive, order of milliseconds, operations
	///
	/// Algorithmic Performance (default data structures):
	///		touch() -> O(1)
	///		insert() / update(), get() / operator[] -> equivalent to unordered_map (O(1) on average, O(n) worst)
	///		size() -> O(1)
	///
	/// All accesses to a given key (insert, update, get) will push that key to most recently used.
	/// If the data objects are shared between threads, it would be best to use a smartptr to manage the lifetime of the data.
	/// as it could be removed from the cache while in use by another thread.
	template <typename Key,
	          typename Value,
	          typename Allocator = EASTLAllocatorType,
	          typename list_type = eastl::list<Key, Allocator>,
	          typename map_type = eastl::unordered_map<Key,
	                                                   eastl::pair<Value, typename list_type::iterator>,
	                                                   eastl::hash<Key>,
	                                                   eastl::equal_to<Key>,
	                                                   Allocator>>
	class lru_cache
	{
	public:
		using key_type = Key;
		using value_type = Value;
		using allocator_type = Allocator;
		using size_type = eastl_size_t;
		using list_iterator = typename list_type::iterator;
		using map_iterator = typename map_type::iterator;
		using data_container_type = eastl::pair<value_type, list_iterator>;
		using iterator = typename map_type::iterator;
		using const_iterator = typename map_type::const_iterator;
		using this_type = lru_cache<key_type, value_type, Allocator, list_type, map_type>;
		using create_callback_type = eastl::function<value_type(key_type)>;
		using delete_callback_type = eastl::function<void(const value_type &)>;

		/// lru_cache constructor
		///
		/// Creates a Key / Value map that only stores size Value objects until it deletes them.
		/// For complex objects or operations, the creator and deletor callbacks can be used.
		/// This works just like a regular map object: on access, the Value will be created if it doesn't exist, returned otherwise.
		explicit lru_cache(size_type size,
		                   const allocator_type& allocator = EASTL_LRUCACHE_DEFAULT_ALLOCATOR,
		                   create_callback_type creator = nullptr,
		                   delete_callback_type deletor = nullptr)
		    : m_list(allocator)
		    , m_map(allocator)
		    , m_capacity(size)
		    , m_create_callback(creator)
		    , m_delete_callback(deletor)
		{
		}

		/// lru_cache destructor
		///
		/// Iterates across every entry in the map and calls the deletor before calling the standard destructors
		~lru_cache()
		{
			// Destruct everything we have cached
			for (auto& iter : m_map)
			{
				if (m_delete_callback)
					m_delete_callback(iter.second.first);
			}
		}

		lru_cache(std::initializer_list<eastl::pair<Key, Value>> il)
			: lru_cache(static_cast<size_type>(il.size()))
		{
			for(auto& p : il)
				insert_or_assign(p.first, p.second);
		}

		// TODO(rparolin):  Why do we prevent copies? And what about moves?
		lru_cache(const this_type&) = delete;
		this_type &operator=(const this_type&) = delete;

		/// insert
		/// 
		/// insert key k with value v.
		/// If key already exists, no change is made and the return value is false.
		/// If the key doesn't exist, the data is added to the map and the return value is true.
		bool insert(const key_type& k, const value_type& v)
		{
			if (m_map.find(k) == m_map.end())
			{
				make_space();

				m_list.push_front(k);
				m_map[k] = data_container_type(v, m_list.begin());

				return true;
			}
			else
			{
				return false;
			}
		}

		/// emplace
		/// 
		/// Places a new object in place k created with args
		/// If the key already exists, no change is made.
		/// return value is a pair of the iterator to the emplaced or already-existing element and a bool denoting whether insertion took place.
		template <typename... Args>
		eastl::pair<iterator, bool> emplace(const key_type& k, Args&&... args)
		{
			auto it = m_map.find(k);
			if (it == m_map.end())
			{
				make_space();

				m_list.push_front(k);
				return m_map.emplace(k, data_container_type(piecewise_construct, eastl::forward_as_tuple(eastl::forward<Args>(args)...), make_tuple(m_list.begin())));
			}
			else
			{
				return make_pair(it, false);
			}
		}

		/// insert_or_assign
		///
		/// Same as add, but replaces the data at key k, if it exists, with the new entry v
		/// Note that the deletor for the old v will be called before it's replaced with the new value of v
		void insert_or_assign(const key_type& k, const value_type& v)
		{
			auto iter = m_map.find(k);

			if (m_map.find(k) != m_map.end())
			{
				assign(iter, v);
			}
			else
			{
				insert(k, v);
			}
		}

		/// contains
		/// 
		/// Returns true if key k exists in the cache
		bool contains(const key_type& k) const
		{
			return m_map.find(k) != m_map.end();
		}

		/// at
		///
		/// Retrives the data for key k, not valid if k does not exist
		eastl::optional<value_type> at(const key_type& k)
		{
			auto iter = m_map.find(k);

			if (iter != m_map.end())
			{
				return iter->second.first;
			}
			else
			{
				return eastl::nullopt;
			}
		}

		/// get
		///
		/// Retrives the data for key k.  If no data exists, it will be created by calling the
		/// creator.
		value_type& get(const key_type& k)
		{
			auto iter = m_map.find(k);

			// The entry exists in the cache
			if (iter != m_map.end())
			{
				touch(k);
				return iter->second.first;
			}
			else // The entry doesn't exist in the cache, so create one
			{
				// Add the entry to the map
				insert(k, m_create_callback ? m_create_callback(k) : value_type());

				// return the new data
				return m_map[k].first;
			}
		}

		/// Equivalent to get(k)
		value_type& operator[](const key_type& k) { return get(k); }

		/// erase
		///
		/// erases key k from the cache.
		/// If k does not exist, returns false.  If k exists, returns true.
		bool erase(const key_type& k)
		{
			auto iter = m_map.find(k);

			if (iter != m_map.end())
			{
				m_list.erase(iter->second.second);

				// Delete the actual entry
				map_erase(iter);

				return true;
			}

			return false;
		}

		/// erase_oldest
		///
		/// Removes the oldest entry from the cache.
		void erase_oldest()
		{
			auto key = m_list.back();
			m_list.pop_back();

			// Delete the actual entry
			auto iter = m_map.find(key);
			map_erase(iter);
		}

		/// touch
		///
		/// Touches key k, marking it as most recently used.
		/// If k does not exist, returns false.  If the touch was successful, returns true.
		bool touch(const key_type& k)
		{
			auto iter = m_map.find(k);

			if (iter != m_map.end())
			{
				touch(iter);
				return true;
			}

			return false;
		}

		/// touch
		///
		/// Touches key at iterator iter, moving it to most recently used position
		void touch(iterator& iter)
		{
			auto listRef = iter->second.second;

			m_list.erase(listRef);
			m_list.push_front(iter->first);
			iter->second.second = m_list.begin();
		}

		/// assign
		///
		/// Updates key k with data v.
		/// If key k does not exist, returns false and no changes are made.
		/// If key k exists, existing data has its deletor called and key k's data is replaced with new v data
		bool assign(const key_type& k, const value_type& v)
		{
			auto iter = m_map.find(k);

			if (iter != m_map.end())
			{
				assign(iter, v);
				return true;
			}

			return false;
		}

		/// assign
		///
		/// Updates data at spot iter with data v.
		void assign(iterator& iter, const value_type& v)
		{
			if (m_delete_callback)
				m_delete_callback(iter->second.first);
			touch(iter);
			iter->second.first = v;
		}

		// standard container functions
		iterator begin()               EA_NOEXCEPT { return m_map.begin(); }
		iterator end()                 EA_NOEXCEPT { return m_map.end(); }
		iterator rbegin()              EA_NOEXCEPT { return m_map.rbegin(); }
		iterator rend()                EA_NOEXCEPT { return m_map.rend(); }
		const_iterator begin() const   EA_NOEXCEPT { return m_map.begin(); }
		const_iterator cbegin() const  EA_NOEXCEPT { return m_map.cbegin(); }
		const_iterator crbegin() const EA_NOEXCEPT { return m_map.crbegin(); }
		const_iterator end() const     EA_NOEXCEPT { return m_map.end(); }
		const_iterator cend() const    EA_NOEXCEPT { return m_map.cend(); }
		const_iterator crend() const   EA_NOEXCEPT { return m_map.crend(); }

		bool empty() const             EA_NOEXCEPT { return m_map.empty(); }
		size_type size() const         EA_NOEXCEPT { return m_map.size(); }
		size_type capacity() const     EA_NOEXCEPT { return m_capacity; }

		void clear() EA_NOEXCEPT
		{
			// Since we have a delete callback, we want to reuse the trim function by cheating the max
			// size to clear all the entries to avoid duplicating code.
			auto old_max = m_capacity;

			m_capacity = 0;
			trim();
			m_capacity = old_max;
		}

		/// resize
		///
		/// Resizes the cache.  Can be used to either expand or contract the cache.
		/// In the case of a contraction, the oldest entries will be evicted with their respective
		/// deletors called before completing.
		void resize(size_type newSize)	
		{
			m_capacity = newSize;
			trim();
		}
		
		void setCreateCallback(create_callback_type callback) { m_create_callback = callback; }
		void setDeleteCallback(delete_callback_type callback) { m_delete_callback = callback; }

		// EASTL extensions
		const allocator_type& get_allocator() const EA_NOEXCEPT					{ return m_map.get_allocator(); }
		allocator_type&       get_allocator() EA_NOEXCEPT						{ return m_map.get_allocator(); }
		void                  set_allocator(const allocator_type& allocator)	{ m_map.set_allocator(allocator); m_list.set_allocator(allocator); }

		/// Does not reset the callbacks
		void reset_lose_memory() EA_NOEXCEPT									{ m_map.reset_lose_memory(); m_list.reset_lose_memory(); }

	private:
		inline void map_erase(map_iterator pos)
		{
			if (m_delete_callback)
				m_delete_callback(pos->second.first);
			m_map.erase(pos);
		}
		
		bool trim()
		{
			if (size() <= m_capacity)
			{
				return false; // No trim necessary
			}

			// We need to trim
			do
			{
				erase_oldest();
			} while (m_list.size() > m_capacity);

			return true;
		}

		void make_space()
		{
			if (size() == m_capacity)
			{
				erase_oldest();
			}
		}

	private:
		list_type				m_list;
		map_type				m_map;
		size_type				m_capacity;
		create_callback_type	m_create_callback;
		delete_callback_type	m_delete_callback;
	};
}



#endif
