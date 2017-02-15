///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
//////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_MAP_H
#define EASTL_MAP_H


#include <EASTL/internal/config.h>
#include <EASTL/internal/red_black_tree.h>
#include <EASTL/functional.h>
#include <EASTL/utility.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// EASTL_MAP_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_MAP_DEFAULT_NAME
		#define EASTL_MAP_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " map" // Unless the user overrides something, this is "EASTL map".
	#endif


	/// EASTL_MULTIMAP_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_MULTIMAP_DEFAULT_NAME
		#define EASTL_MULTIMAP_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " multimap" // Unless the user overrides something, this is "EASTL multimap".
	#endif


	/// EASTL_MAP_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_MAP_DEFAULT_ALLOCATOR
		#define EASTL_MAP_DEFAULT_ALLOCATOR allocator_type(EASTL_MAP_DEFAULT_NAME)
	#endif

	/// EASTL_MULTIMAP_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_MULTIMAP_DEFAULT_ALLOCATOR
		#define EASTL_MULTIMAP_DEFAULT_ALLOCATOR allocator_type(EASTL_MULTIMAP_DEFAULT_NAME)
	#endif



	/// map
	///
	/// Implements a canonical map. 
	///
	/// The large majority of the implementation of this class is found in the rbtree
	/// base class. We control the behaviour of rbtree via template parameters.
	///
	/// Pool allocation
	/// If you want to make a custom memory pool for a map container, your pool 
	/// needs to contain items of type map::node_type. So if you have a memory
	/// pool that has a constructor that takes the size of pool items and the
	/// count of pool items, you would do this (assuming that MemoryPool implements
	/// the Allocator interface):
	///     typedef map<Widget, int, less<Widget>, MemoryPool> WidgetMap;  // Delare your WidgetMap type.
	///     MemoryPool myPool(sizeof(WidgetMap::node_type), 100);          // Make a pool of 100 Widget nodes.
	///     WidgetMap myMap(&myPool);                                      // Create a map that uses the pool.
	///
	template <typename Key, typename T, typename Compare = eastl::less<Key>, typename Allocator = EASTLAllocatorType>
	class map
		: public rbtree<Key, eastl::pair<const Key, T>, Compare, Allocator, eastl::use_first<eastl::pair<const Key, T> >, true, true>
	{
	public:
		typedef rbtree<Key, eastl::pair<const Key, T>, Compare, Allocator,
						eastl::use_first<eastl::pair<const Key, T> >, true, true>   base_type;
		typedef map<Key, T, Compare, Allocator>                                     this_type;
		typedef typename base_type::size_type                                       size_type;
		typedef typename base_type::key_type                                        key_type;
		typedef T                                                                   mapped_type;
		typedef typename base_type::value_type                                      value_type;
		typedef typename base_type::node_type                                       node_type;
		typedef typename base_type::iterator                                        iterator;
		typedef typename base_type::const_iterator                                  const_iterator;
		typedef typename base_type::allocator_type                                  allocator_type;
		typedef typename base_type::insert_return_type                              insert_return_type;
		typedef typename base_type::extract_key                                     extract_key;
		// Other types are inherited from the base class.

		using base_type::begin;
		using base_type::end;
		using base_type::find;
		using base_type::lower_bound;
		using base_type::upper_bound;
		using base_type::mCompare;
		using base_type::insert;
		using base_type::erase;

		class value_compare 
		{
		protected:
			friend class map;
			Compare compare;
			value_compare(Compare c) : compare(c) {}

		public:
			typedef bool       result_type;
			typedef value_type first_argument_type;
			typedef value_type second_argument_type;

			bool operator()(const value_type& x, const value_type& y) const 
				{ return compare(x.first, y.first); }
		};

	public:
		map(const allocator_type& allocator = EASTL_MAP_DEFAULT_ALLOCATOR);
		map(const Compare& compare, const allocator_type& allocator = EASTL_MAP_DEFAULT_ALLOCATOR);
		map(const this_type& x);
		#if EASTL_MOVE_SEMANTICS_ENABLED
			map(this_type&& x);
			map(this_type&& x, const allocator_type& allocator);
		#endif
		map(std::initializer_list<value_type> ilist, const Compare& compare = Compare(), const allocator_type& allocator = EASTL_MAP_DEFAULT_ALLOCATOR);

		template <typename Iterator>
		map(Iterator itBegin, Iterator itEnd); // allocator arg removed because VC7.1 fails on the default arg. To consider: Make a second version of this function without a default arg.

		#if EASTL_MOVE_SEMANTICS_ENABLED // The (this_type&& x) ctor above has the side effect of forcing us to make operator= visible in this subclass.
			this_type& operator=(const this_type& x) { return (this_type&)base_type::operator=(x); }
			this_type& operator=(std::initializer_list<value_type> ilist) { return (this_type&)base_type::operator=(ilist); }
			this_type& operator=(this_type&& x) { return (this_type&)base_type::operator=(eastl::move(x)); }
		#endif

	public:
		/// This is an extension to the C++ standard. We insert a default-constructed 
		/// element with the given key. The reason for this is that we can avoid the 
		/// potentially expensive operation of creating and/or copying a mapped_type
		/// object on the stack. Note that C++11 move insertions and variadic emplace
		/// support make this extension mostly no longer necessary.
		insert_return_type insert(const Key& key);

		value_compare value_comp() const;

		size_type erase(const Key& key);
		size_type count(const Key& key) const;

		eastl::pair<iterator, iterator>             equal_range(const Key& key);
		eastl::pair<const_iterator, const_iterator> equal_range(const Key& key) const;

		T& operator[](const Key& key); // Of map, multimap, set, and multimap, only map has operator[].
		#if EASTL_MOVE_SEMANTICS_ENABLED
			T& operator[](Key&& key); 
		#endif

		T& at(const Key& key);
		const T& at(const Key& key) const;

	}; // map






	/// multimap
	///
	/// Implements a canonical multimap.
	///
	/// The large majority of the implementation of this class is found in the rbtree
	/// base class. We control the behaviour of rbtree via template parameters.
	///
	/// Pool allocation
	/// If you want to make a custom memory pool for a multimap container, your pool 
	/// needs to contain items of type multimap::node_type. So if you have a memory
	/// pool that has a constructor that takes the size of pool items and the
	/// count of pool items, you would do this (assuming that MemoryPool implements
	/// the Allocator interface):
	///     typedef multimap<Widget, int, less<Widget>, MemoryPool> WidgetMap;  // Delare your WidgetMap type.
	///     MemoryPool myPool(sizeof(WidgetMap::node_type), 100);               // Make a pool of 100 Widget nodes.
	///     WidgetMap myMap(&myPool);                                           // Create a map that uses the pool.
	///
	template <typename Key, typename T, typename Compare = eastl::less<Key>, typename Allocator = EASTLAllocatorType>
	class multimap
		: public rbtree<Key, eastl::pair<const Key, T>, Compare, Allocator, eastl::use_first<eastl::pair<const Key, T> >, true, false>
	{
	public:
		typedef rbtree<Key, eastl::pair<const Key, T>, Compare, Allocator, 
						eastl::use_first<eastl::pair<const Key, T> >, true, false>  base_type;
		typedef multimap<Key, T, Compare, Allocator>                                this_type;
		typedef typename base_type::size_type                                       size_type;
		typedef typename base_type::key_type                                        key_type;
		typedef T                                                                   mapped_type;
		typedef typename base_type::value_type                                      value_type;
		typedef typename base_type::node_type                                       node_type;
		typedef typename base_type::iterator                                        iterator;
		typedef typename base_type::const_iterator                                  const_iterator;
		typedef typename base_type::allocator_type                                  allocator_type;
		typedef typename base_type::insert_return_type                              insert_return_type;
		typedef typename base_type::extract_key                                     extract_key;
		// Other types are inherited from the base class.

		using base_type::begin;
		using base_type::end;
		using base_type::find;
		using base_type::lower_bound;
		using base_type::upper_bound;
		using base_type::mCompare;
		using base_type::insert;
		using base_type::erase;

		class value_compare 
		{
		protected:
			friend class multimap;
			Compare compare;
			value_compare(Compare c) : compare(c) {}

		public:
			typedef bool       result_type;
			typedef value_type first_argument_type;
			typedef value_type second_argument_type;

			bool operator()(const value_type& x, const value_type& y) const 
				{ return compare(x.first, y.first); }
		};

	public:
		multimap(const allocator_type& allocator = EASTL_MULTIMAP_DEFAULT_ALLOCATOR);
		multimap(const Compare& compare, const allocator_type& allocator = EASTL_MULTIMAP_DEFAULT_ALLOCATOR);
		multimap(const this_type& x);
		#if EASTL_MOVE_SEMANTICS_ENABLED
		multimap(this_type&& x);
		multimap(this_type&& x, const allocator_type& allocator);
		#endif
		multimap(std::initializer_list<value_type> ilist, const Compare& compare = Compare(), const allocator_type& allocator = EASTL_MULTIMAP_DEFAULT_ALLOCATOR);

		template <typename Iterator>
		multimap(Iterator itBegin, Iterator itEnd); // allocator arg removed because VC7.1 fails on the default arg. To consider: Make a second version of this function without a default arg.

		#if EASTL_MOVE_SEMANTICS_ENABLED // The (this_type&& x) ctor above has the side effect of forcing us to make operator= visible in this subclass.
			this_type& operator=(const this_type& x) { return (this_type&)base_type::operator=(x); }
			this_type& operator=(std::initializer_list<value_type> ilist) { return (this_type&)base_type::operator=(ilist); }
			this_type& operator=(this_type&& x) { return (this_type&)base_type::operator=(eastl::move(x)); }
		#endif

	public:
		/// This is an extension to the C++ standard. We insert a default-constructed 
		/// element with the given key. The reason for this is that we can avoid the 
		/// potentially expensive operation of creating and/or copying a mapped_type
		/// object on the stack. Note that C++11 move insertions and variadic emplace
		/// support make this extension mostly no longer necessary.
		insert_return_type insert(const Key& key);

		value_compare value_comp() const;

		size_type erase(const Key& key);
		size_type count(const Key& key) const;

		eastl::pair<iterator, iterator>             equal_range(const Key& key);
		eastl::pair<const_iterator, const_iterator> equal_range(const Key& key) const;

		/// equal_range_small
		/// This is a special version of equal_range which is optimized for the 
		/// case of there being few or no duplicated keys in the tree.
		eastl::pair<iterator, iterator>             equal_range_small(const Key& key);
		eastl::pair<const_iterator, const_iterator> equal_range_small(const Key& key) const;

	}; // multimap





	///////////////////////////////////////////////////////////////////////
	// map
	///////////////////////////////////////////////////////////////////////

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline map<Key, T, Compare, Allocator>::map(const allocator_type& allocator)
		: base_type(allocator)
	{
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline map<Key, T, Compare, Allocator>::map(const Compare& compare, const allocator_type& allocator)
		: base_type(compare, allocator)
	{
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline map<Key, T, Compare, Allocator>::map(const this_type& x)
		: base_type(x)
	{
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename Key, typename T, typename Compare, typename Allocator>
		inline map<Key, T, Compare, Allocator>::map(this_type&& x)
			: base_type(eastl::move(x))
		{
		}

		template <typename Key, typename T, typename Compare, typename Allocator>
		inline map<Key, T, Compare, Allocator>::map(this_type&& x, const allocator_type& allocator)
			: base_type(eastl::move(x), allocator)
		{
		}
	#endif


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline map<Key, T, Compare, Allocator>::map(std::initializer_list<value_type> ilist, const Compare& compare, const allocator_type& allocator)
		: base_type(ilist.begin(), ilist.end(), compare, allocator)
	{
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	template <typename Iterator>
	inline map<Key, T, Compare, Allocator>::map(Iterator itBegin, Iterator itEnd)
		: base_type(itBegin, itEnd, Compare(), EASTL_MAP_DEFAULT_ALLOCATOR)
	{
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename map<Key, T, Compare, Allocator>::insert_return_type
	map<Key, T, Compare, Allocator>::insert(const Key& key)
	{
		return base_type::DoInsertKey(true_type(), key);
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename map<Key, T, Compare, Allocator>::value_compare 
	map<Key, T, Compare, Allocator>::value_comp() const
	{
		return value_compare(mCompare);
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename map<Key, T, Compare, Allocator>::size_type
	map<Key, T, Compare, Allocator>::erase(const Key& key)
	{
		const iterator it(find(key));

		if(it != end()) // If it exists...
		{
			base_type::erase(it);
			return 1;
		}
		return 0;
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename map<Key, T, Compare, Allocator>::size_type
	map<Key, T, Compare, Allocator>::count(const Key& key) const
	{
		const const_iterator it(find(key));
		return (it != end()) ? 1 : 0;
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline eastl::pair<typename map<Key, T, Compare, Allocator>::iterator,
					   typename map<Key, T, Compare, Allocator>::iterator>
	map<Key, T, Compare, Allocator>::equal_range(const Key& key)
	{
		// The resulting range will either be empty or have one element,
		// so instead of doing two tree searches (one for lower_bound and 
		// one for upper_bound), we do just lower_bound and see if the 
		// result is a range of size zero or one.
		const iterator itLower(lower_bound(key));

		if((itLower == end()) || mCompare(key, itLower.mpNode->mValue.first)) // If at the end or if (key is < itLower)...
			return eastl::pair<iterator, iterator>(itLower, itLower);

		iterator itUpper(itLower);
		return eastl::pair<iterator, iterator>(itLower, ++itUpper);
	}
	

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline eastl::pair<typename map<Key, T, Compare, Allocator>::const_iterator, 
					   typename map<Key, T, Compare, Allocator>::const_iterator>
	map<Key, T, Compare, Allocator>::equal_range(const Key& key) const
	{
		// See equal_range above for comments.
		const const_iterator itLower(lower_bound(key));

		if((itLower == end()) || mCompare(key, itLower.mpNode->mValue.first)) // If at the end or if (key is < itLower)...
			return eastl::pair<const_iterator, const_iterator>(itLower, itLower);

		const_iterator itUpper(itLower);
		return eastl::pair<const_iterator, const_iterator>(itLower, ++itUpper);
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline T& map<Key, T, Compare, Allocator>::operator[](const Key& key)
	{
		iterator itLower(lower_bound(key)); // itLower->first is >= key.

		if((itLower == end()) || mCompare(key, (*itLower).first))
		{
			itLower = base_type::DoInsertKey(true_type(), itLower, key);
		}

		return (*itLower).second;

		// Reference implementation of this function, which may not be as fast:
		//iterator it(base_type::insert(eastl::pair<iterator, iterator>(key, T())).first);
		//return it->second;
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename Key, typename T, typename Compare, typename Allocator>
		inline T& map<Key, T, Compare, Allocator>::operator[](Key&& key)
		{
			iterator itLower(lower_bound(key)); // itLower->first is >= key.

			if((itLower == end()) || mCompare(key, (*itLower).first))
			{
				itLower = base_type::DoInsertKey(true_type(), itLower, eastl::move(key));
			}

			return (*itLower).second;

			// Reference implementation of this function, which may not be as fast:
			//iterator it(base_type::insert(eastl::pair<iterator, iterator>(key, T())).first);
			//return it->second;
		}
	#endif


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline T& map<Key, T, Compare, Allocator>::at(const Key& key)
	{
		iterator itLower(lower_bound(key)); // itLower->first is >= key.

		if(itLower == end())
		{
			#if EASTL_EXCEPTIONS_ENABLED
				throw std::out_of_range("map::at key does not exist");
			#else
				EASTL_FAIL_MSG("map::at key does not exist");
			#endif
		}

		return (*itLower).second;
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline const T& map<Key, T, Compare, Allocator>::at(const Key& key) const
	{
		const_iterator itLower(lower_bound(key)); // itLower->first is >= key.

		if(itLower == end())
		{
			#if EASTL_EXCEPTIONS_ENABLED
				throw std::out_of_range("map::at key does not exist");
			#else
				EASTL_FAIL_MSG("map::at key does not exist");
			#endif
		}

		return (*itLower).second;
	}





	///////////////////////////////////////////////////////////////////////
	// multimap
	///////////////////////////////////////////////////////////////////////

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline multimap<Key, T, Compare, Allocator>::multimap(const allocator_type& allocator)
		: base_type(allocator)
	{
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline multimap<Key, T, Compare, Allocator>::multimap(const Compare& compare, const allocator_type& allocator)
		: base_type(compare, allocator)
	{
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline multimap<Key, T, Compare, Allocator>::multimap(const this_type& x)
		: base_type(x)
	{
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename Key, typename T, typename Compare, typename Allocator>
		inline multimap<Key, T, Compare, Allocator>::multimap(this_type&& x)
			: base_type(eastl::move(x))
		{
		}

		template <typename Key, typename T, typename Compare, typename Allocator>
		inline multimap<Key, T, Compare, Allocator>::multimap(this_type&& x, const allocator_type& allocator)
			: base_type(eastl::move(x), allocator)
		{
		}
	#endif


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline multimap<Key, T, Compare, Allocator>::multimap(std::initializer_list<value_type> ilist, const Compare& compare, const allocator_type& allocator)
		: base_type(ilist.begin(), ilist.end(), compare, allocator)
	{
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	template <typename Iterator>
	inline multimap<Key, T, Compare, Allocator>::multimap(Iterator itBegin, Iterator itEnd)
		: base_type(itBegin, itEnd, Compare(), EASTL_MULTIMAP_DEFAULT_ALLOCATOR)
	{
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename multimap<Key, T, Compare, Allocator>::insert_return_type
	multimap<Key, T, Compare, Allocator>::insert(const Key& key)
	{
		return base_type::DoInsertKey(false_type(), key);
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename multimap<Key, T, Compare, Allocator>::value_compare 
	multimap<Key, T, Compare, Allocator>::value_comp() const
	{
		return value_compare(mCompare);
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename multimap<Key, T, Compare, Allocator>::size_type
	multimap<Key, T, Compare, Allocator>::erase(const Key& key)
	{
		const eastl::pair<iterator, iterator> range(equal_range(key));
		const size_type n = (size_type)eastl::distance(range.first, range.second);
		base_type::erase(range.first, range.second);
		return n;
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename multimap<Key, T, Compare, Allocator>::size_type
	multimap<Key, T, Compare, Allocator>::count(const Key& key) const
	{
		const eastl::pair<const_iterator, const_iterator> range(equal_range(key));
		return (size_type)eastl::distance(range.first, range.second);
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline eastl::pair<typename multimap<Key, T, Compare, Allocator>::iterator,
					   typename multimap<Key, T, Compare, Allocator>::iterator>
	multimap<Key, T, Compare, Allocator>::equal_range(const Key& key)
	{
		// There are multiple ways to implement equal_range. The implementation mentioned
		// in the C++ standard and which is used by most (all?) commercial STL implementations
		// is this:
		//    return eastl::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
		//
		// This does two tree searches -- one for the lower bound and one for the 
		// upper bound. This works well for the case whereby you have a large container
		// and there are lots of duplicated values. We provide an alternative version
		// of equal_range called equal_range_small for cases where the user is confident
		// that the number of duplicated items is only a few.

		return eastl::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline eastl::pair<typename multimap<Key, T, Compare, Allocator>::const_iterator, 
					   typename multimap<Key, T, Compare, Allocator>::const_iterator>
	multimap<Key, T, Compare, Allocator>::equal_range(const Key& key) const
	{
		// See comments above in the non-const version of equal_range.
		return eastl::pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key));
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline eastl::pair<typename multimap<Key, T, Compare, Allocator>::iterator,
					   typename multimap<Key, T, Compare, Allocator>::iterator>
	multimap<Key, T, Compare, Allocator>::equal_range_small(const Key& key)
	{
		// We provide alternative version of equal_range here which works faster
		// for the case where there are at most small number of potential duplicated keys.
		const iterator itLower(lower_bound(key));
		iterator       itUpper(itLower);

		while((itUpper != end()) && !mCompare(key, itUpper.mpNode->mValue.first))
			++itUpper;

		return eastl::pair<iterator, iterator>(itLower, itUpper);
	}


	template <typename Key, typename T, typename Compare, typename Allocator>
	inline eastl::pair<typename multimap<Key, T, Compare, Allocator>::const_iterator, 
					   typename multimap<Key, T, Compare, Allocator>::const_iterator>
	multimap<Key, T, Compare, Allocator>::equal_range_small(const Key& key) const
	{
		// We provide alternative version of equal_range here which works faster
		// for the case where there are at most small number of potential duplicated keys.
		const const_iterator itLower(lower_bound(key));
		const_iterator       itUpper(itLower);

		while((itUpper != end()) && !mCompare(key, itUpper.mpNode->mValue.first))
			++itUpper;

		return eastl::pair<const_iterator, const_iterator>(itLower, itUpper);
	}




} // namespace eastl


#endif // Header include guard




