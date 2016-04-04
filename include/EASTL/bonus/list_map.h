/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_LIST_MAP_H
#define EASTL_LIST_MAP_H


#include <EASTL/map.h>


namespace eastl
{

	/// EASTL_MAP_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_LIST_MAP_DEFAULT_NAME
		#define EASTL_LIST_MAP_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " list_map" // Unless the user overrides something, this is "EASTL list_map".
	#endif

	/// EASTL_MAP_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_LIST_MAP_DEFAULT_ALLOCATOR
		#define EASTL_LIST_MAP_DEFAULT_ALLOCATOR allocator_type(EASTL_LIST_MAP_DEFAULT_NAME)
	#endif


	/// list_map_data_base
	///
	/// We define a list_map_data_base separately from list_map_data (below), because it 
	/// allows us to have non-templated operations, and it makes it so that the 
	/// list_map anchor node doesn't carry a T with it, which would waste space and 
	/// possibly lead to surprising the user due to extra Ts existing that the user 
	/// didn't explicitly create. The downside to all of this is that it makes debug 
	/// viewing of an list_map harder, given that the node pointers are of type 
	/// list_map_data_base and not list_map_data.
	///
	struct list_map_data_base
	{
		list_map_data_base* mpNext;
		list_map_data_base* mpPrev;
	};


	/// list_map_data
	///
	template <typename Value>
	struct list_map_data : public list_map_data_base
	{
		typedef Value       value_type;

		list_map_data(const value_type& value);

		value_type mValue; // This is a pair of key/value.
	};


	/// list_map_iterator
	///
	template <typename T, typename Pointer, typename Reference>
	struct list_map_iterator
	{
		typedef list_map_iterator<T, Pointer, Reference>    this_type;
		typedef list_map_iterator<T, T*, T&>                iterator;
		typedef list_map_iterator<T, const T*, const T&>    const_iterator;
		typedef eastl_size_t                                size_type;     // See config.h for the definition of eastl_size_t, which defaults to uint32_t.
		typedef ptrdiff_t                                   difference_type;
		typedef T                                           value_type;
		typedef list_map_data_base                          base_node_type;
		typedef list_map_data<T>                            node_type;
		typedef Pointer                                     pointer;
		typedef Reference                                   reference;
		typedef EASTL_ITC_NS::bidirectional_iterator_tag    iterator_category;

	public:
		node_type* mpNode;

	public:
		list_map_iterator();
		list_map_iterator(const base_node_type* pNode);
		list_map_iterator(const iterator& x);

		reference operator*() const;
		pointer   operator->() const;

		this_type& operator++();
		this_type  operator++(int);

		this_type& operator--();
		this_type  operator--(int);

	}; // list_map_iterator


	/// use_value_first
	///
	/// operator()(x) simply returns x.mValue.first. Used in list_map.
	/// This is similar to eastl::use_first, however it assumes that the input type is an object
	/// whose mValue is an eastl::pair, and the first value in the pair is the desired return.
	///
	template <typename Object>
	struct use_value_first
	{
		typedef Object argument_type;
		typedef typename Object::value_type::first_type result_type;

		const result_type& operator()(const Object& x) const
			{ return x.mValue.first; }
	};


	/// list_map
	///
	/// Implements a map like container, which also provides functionality similar to a list.
	/// 
	/// Note: Like a map, keys must still be unique.  As such, push_back() and push_front() operations
	///       return a bool indicating success, or failure if the entry's key is already in use.
	///
	/// list_map is designed to improve performance for situations commonly implemented as:
	///     A map, which must be iterated over to find the oldest entry, or purge expired entries.
	///     A list, which must be iterated over to remove a player's record when they sign off.
	/// 
	/// list_map requires a little more memory per node than either a list or map alone,
	/// and many of list_map's functions have a higher operational cost (CPU time) than their
	/// counterparts in list and map.  However, as the node count increases, list_map quickly outperforms
	/// either a list or a map when find [by-index] and front/back type operations are required.
	/// 
	/// In essence, list_map avoids O(n) iterations at the expense of additional costs to quick (O(1) and O(log n) operations:
	///     push_front(), push_back(), pop_front() and pop_back() have O(log n) operation time, similar to map::insert(), rather than O(1) time like a list,
	///     however, front() and back() maintain O(1) operation time.
	/// 
	/// As a canonical example, consider a large backlog of player group invites, which are removed when either:
	///     The invitation times out - in main loop:  while( !listMap.empty() && listMap.front().IsExpired() ) { listMap.pop_front(); }
	///     The player rejects the outstanding invitation - on rejection:  iter = listMap.find(playerId);  if (iter != listMap.end()) { listMap.erase(iter); }
	/// 
	/// For a similar example, consider a high volume pending request container which must:
	///     Time out old requests (similar to invites timing out above)
	///     Remove requests once they've been handled (similar to rejecting invites above)
	/// 
	/// For such usage patterns, the performance benefits of list_map become dramatic with
	/// common O(n) operations once the node count rises to hundreds or more.
	/// 
	/// When high performance is a priority, Containers with thousands of nodes or more
	/// can quickly result in unacceptable performance when executing even infrequenty O(n) operations.
	/// 
	/// In order to maintain strong performance, avoid iterating over list_map whenever possible.
	/// 
	///////////////////////////////////////////////////////////////////////
	/// find_as
	/// In order to support the ability to have a tree of strings but
	/// be able to do efficiently lookups via char pointers (i.e. so they
	/// aren't converted to string objects), we provide the find_as
	/// function. This function allows you to do a find with a key of a
	/// type other than the tree's key type. See the find_as function
	/// for more documentation on this.
	///
	///////////////////////////////////////////////////////////////////////
	/// Pool allocation
	/// If you want to make a custom memory pool for a list_map container, your pool 
	/// needs to contain items of type list_map::node_type. So if you have a memory
	/// pool that has a constructor that takes the size of pool items and the
	/// count of pool items, you would do this (assuming that MemoryPool implements
	/// the Allocator interface):
	///     typedef list_map<Widget, int, less<Widget>, MemoryPool> WidgetMap;  // Delare your WidgetMap type.
	///     MemoryPool myPool(sizeof(WidgetMap::node_type), 100);               // Make a pool of 100 Widget nodes.
	///     WidgetMap myMap(&myPool);                                           // Create a map that uses the pool.
	///
	template <typename Key, typename T, typename Compare = eastl::less<Key>, typename Allocator = EASTLAllocatorType>
	class list_map
		: protected rbtree<Key, eastl::list_map_data<eastl::pair<const Key, T> >, Compare, Allocator, eastl::use_value_first<eastl::list_map_data<eastl::pair<const Key, T> > >, true, true>
	{
	public:
		typedef rbtree<Key, eastl::list_map_data<eastl::pair<const Key, T> >, Compare, Allocator,
					   eastl::use_value_first<eastl::list_map_data<eastl::pair<const Key, T> > >, true, true>   base_type;
		typedef list_map<Key, T, Compare, Allocator>                                                            this_type;
		typedef typename base_type::size_type                                                                   size_type;
		typedef typename base_type::key_type                                                                    key_type;
		typedef T                                                                                               mapped_type;
		typedef typename eastl::pair<const Key, T>                                                              value_type;          // This is intentionally different from base_type::value_type
		typedef value_type&                                                                                     reference;
		typedef const value_type&                                                                               const_reference;
		typedef typename base_type::node_type                                                                   node_type;           // Despite the internal and external values being different, we're keeping the node type the same as the base
																																	 // in order to allow for pool allocation.  See EASTL/map.h for more information.
		typedef typename eastl::list_map_iterator<value_type, value_type*, value_type&>                         iterator;            // This is intentionally different from base_type::iterator
		typedef typename eastl::list_map_iterator<value_type, const value_type*, const value_type&>             const_iterator;      // This is intentionally different from base_type::const_iterator
		typedef eastl::reverse_iterator<iterator>                                                               reverse_iterator;
		typedef eastl::reverse_iterator<const_iterator>                                                         const_reverse_iterator;
		typedef typename base_type::allocator_type                                                              allocator_type;
		typedef typename eastl::pair<iterator, bool>                                                            insert_return_type;  // This is intentionally removed, as list_map doesn't support insert() functions, in favor of list like push_back and push_front
		typedef typename eastl::use_first<value_type>                                                           extract_key;         // This is intentionally different from base_type::extract_key

		using base_type::get_allocator;
		using base_type::set_allocator;
		using base_type::key_comp;
		using base_type::empty;
		using base_type::size;

	protected:
		typedef typename eastl::list_map_data<eastl::pair<const Key, T> >                                       internal_value_type;

	protected:
		// internal base node, acting as the sentinel for list like behaviors
		list_map_data_base mNode;

	public:
		list_map(const allocator_type& allocator = EASTL_LIST_MAP_DEFAULT_ALLOCATOR);
		list_map(const Compare& compare, const allocator_type& allocator = EASTL_MAP_DEFAULT_ALLOCATOR);

		// To do: Implement the following:

		//list_map(const this_type& x);
		//#if EASTL_MOVE_SEMANTICS_ENABLED
		//    list_map(this_type&& x);
		//    list_map(this_type&& x, const allocator_type& allocator);
		//#endif
		//list_map(std::initializer_list<mapped_type> ilist, const Compare& compare = Compare(), const allocator_type& allocator = EASTL_LIST_MAP_DEFAULT_ALLOCATOR);

		//template <typename Iterator>
		//list_map(Iterator itBegin, Iterator itEnd);

		//this_type& operator=(const this_type& x);
		//this_type& operator=(std::initializer_list<mapped_type> ilist);

		//#if EASTL_MOVE_SEMANTICS_ENABLED
		//    this_type& operator=(this_type&& x);
		//#endif

		//void swap(this_type& x);

	public:
		// iterators
		iterator       begin() EA_NOEXCEPT;
		const_iterator begin() const EA_NOEXCEPT;
		const_iterator cbegin() const EA_NOEXCEPT;

		iterator       end() EA_NOEXCEPT;
		const_iterator end() const EA_NOEXCEPT;
		const_iterator cend() const EA_NOEXCEPT;

		reverse_iterator       rbegin() EA_NOEXCEPT;
		const_reverse_iterator rbegin() const EA_NOEXCEPT;
		const_reverse_iterator crbegin() const EA_NOEXCEPT;

		reverse_iterator       rend() EA_NOEXCEPT;
		const_reverse_iterator rend() const EA_NOEXCEPT;
		const_reverse_iterator crend() const EA_NOEXCEPT;

	public:
		// List like methods
		reference       front();
		const_reference front() const;

		reference       back();
		const_reference back() const;

		// push_front and push_back which takes in a key/value pair
		bool            push_front(const value_type& value);
		bool            push_back(const value_type& value);

		// push_front and push_back which take key and value separately, for convenience
		bool            push_front(const key_type& key, const mapped_type& value);
		bool            push_back(const key_type& key, const mapped_type& value);

		void            pop_front();
		void            pop_back();

	public:
		// Map like methods
		iterator        find(const key_type& key);
		const_iterator  find(const key_type& key) const;

		template <typename U, typename Compare2>
		iterator       find_as(const U& u, Compare2 compare2);
		template <typename U, typename Compare2>
		const_iterator find_as(const U& u, Compare2 compare2) const;

		size_type count(const key_type& key) const;
		size_type erase(const key_type& key);

	public:
		// Shared methods which are common to list and map
		iterator erase(const_iterator position);
		reverse_iterator erase(const_reverse_iterator position);

		void clear();
		void reset_lose_memory();

		bool validate() const;
		int validate_iterator(const_iterator i) const;

	public:
		// list like functionality which is in consideration for implementation:
		// iterator insert(const_iterator position, const value_type& value);
		// void remove(const mapped_type& x); 
		 
	public:
		// list like functionality which may be implemented, but is discouraged from implementation:
		// due to the liklihood that they would require O(n) time to execute.
		// template <typename Predicate>
		// void remove_if(Predicate);
		// void reverse();
		// void sort();
		// template<typename Compare>
		// void sort(Compare compare);          

	public:
		// map like functionality which list_map does not support, due to abmiguity with list like functionality:
		#if !defined(EA_COMPILER_NO_DELETED_FUNCTIONS)
		template <typename InputIterator>
			list_map(InputIterator first, InputIterator last, const Compare& compare, const allocator_type& allocator = EASTL_RBTREE_DEFAULT_ALLOCATOR) = delete;
		 
			insert_return_type insert(const value_type& value) = delete;
			iterator insert(const_iterator position, const value_type& value) = delete;

			template <typename InputIterator>
			void insert(InputIterator first, InputIterator last) = delete;
				  
			insert_return_type insert(const key_type& key) = delete;
		 
			iterator erase(const_iterator first, const_iterator last) = delete;
			reverse_iterator erase(reverse_iterator first, reverse_iterator last) = delete;
		 
			void erase(const key_type* first, const key_type* last) = delete;
		 
			iterator       lower_bound(const key_type& key) = delete;
			const_iterator lower_bound(const key_type& key) const = delete;

			iterator       upper_bound(const key_type& key) = delete;
			const_iterator upper_bound(const key_type& key) const = delete;
		 
			eastl::pair<iterator, iterator>             equal_range(const key_type& key) = delete;
			eastl::pair<const_iterator, const_iterator> equal_range(const key_type& key) const = delete;

			mapped_type& operator[](const key_type& key) = delete; // Of map, multimap, set, and multimap, only map has operator[].
		#endif

	public:
		// list like functionality which list_map does not support, due to ambiguity with map like functionality:
		#if 0
			reference push_front() = delete;
			void*     push_front_uninitialized() = delete;

			reference push_back() = delete;
			void*     push_back_uninitialized() = delete;

			iterator insert(const_iterator position) = delete;
		 
			void insert(const_iterator position, size_type n, const value_type& value) = delete;

		template <typename InputIterator>
			void insert(const_iterator position, InputIterator first, InputIterator last) = delete;
		 
			iterator erase(const_iterator first, const_iterator last) = delete;
			reverse_iterator erase(const_reverse_iterator first, const_reverse_iterator last) = delete;
		 
			void splice(const_iterator position, this_type& x) = delete
			void splice(const_iterator position, this_type& x, const_iterator i) = delete;
			void splice(const_iterator position, this_type& x, const_iterator first, const_iterator last) = delete;

			void merge(this_type& x) = delete;

		template <typename Compare>
			void merge(this_type& x, Compare compare) = delete;
		 
			void unique() = delete;  // Uniqueness is enforced by map functionality

		template <typename BinaryPredicate>
			void unique(BinaryPredicate) = delete;  // Uniqueness is enforced by map functionality
		#endif

	}; // list_map


	///////////////////////////////////////////////////////////////////////
	// list_map_data
	///////////////////////////////////////////////////////////////////////

	template <typename Value>
	inline list_map_data<Value>::list_map_data(const Value& value)
	  : mValue(value)
	{
		mpNext = NULL; // GCC 4.8 is generating warnings about referencing these values in list_map::push_front unless we 
		mpPrev = NULL; // initialize them here. The compiler seems to be mistaken, as our code isn't actually using them unintialized.
	}


	///////////////////////////////////////////////////////////////////////
	// list_map_iterator
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Pointer, typename Reference>
	inline list_map_iterator<T, Pointer, Reference>::list_map_iterator()
		: mpNode(NULL)
	{
		// Empty
	}


	template <typename T, typename Pointer, typename Reference>
	inline list_map_iterator<T, Pointer, Reference>::list_map_iterator(const base_node_type* pNode)
		: mpNode(static_cast<node_type*>(const_cast<base_node_type*>(pNode)))
	{
		// Empty
	}


	template <typename T, typename Pointer, typename Reference>
	inline list_map_iterator<T, Pointer, Reference>::list_map_iterator(const iterator& x)
		: mpNode(const_cast<node_type*>(x.mpNode))
	{
		// Empty
	} 


	template <typename T, typename Pointer, typename Reference>
	inline typename list_map_iterator<T, Pointer, Reference>::reference
	list_map_iterator<T, Pointer, Reference>::operator*() const
	{
		return mpNode->mValue;
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename list_map_iterator<T, Pointer, Reference>::pointer
	list_map_iterator<T, Pointer, Reference>::operator->() const
	{
		return &mpNode->mValue;
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename list_map_iterator<T, Pointer, Reference>::this_type&
	list_map_iterator<T, Pointer, Reference>::operator++()
	{
		mpNode = static_cast<node_type*>(mpNode->mpNext);
		return *this;
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename list_map_iterator<T, Pointer, Reference>::this_type
	list_map_iterator<T, Pointer, Reference>::operator++(int)
	{
		this_type temp(*this);
		mpNode = static_cast<node_type*>(mpNode->mpNext);
		return temp;
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename list_map_iterator<T, Pointer, Reference>::this_type&
	list_map_iterator<T, Pointer, Reference>::operator--()
	{
		mpNode = static_cast<node_type*>(mpNode->mpPrev);
		return *this;
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename list_map_iterator<T, Pointer, Reference>::this_type 
	list_map_iterator<T, Pointer, Reference>::operator--(int)
	{
		this_type temp(*this);
		mpNode = static_cast<node_type*>(mpNode->mpPrev);
		return temp;
	}


	// We provide additional template paremeters here to support comparisons between const and non-const iterators.
	// See C++ defect report #179, or EASTL/list.h for more information.
	template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
	inline bool operator==(const list_map_iterator<T, PointerA, ReferenceA>& a, 
						   const list_map_iterator<T, PointerB, ReferenceB>& b)
	{
		return a.mpNode == b.mpNode;
	}


	template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
	inline bool operator!=(const list_map_iterator<T, PointerA, ReferenceA>& a, 
						   const list_map_iterator<T, PointerB, ReferenceB>& b)
	{
		return a.mpNode != b.mpNode;
	}


	// We provide a version of operator!= for the case where the iterators are of the 
	// same type. This helps prevent ambiguity errors in the presence of rel_ops.
	template <typename T, typename Pointer, typename Reference>
	inline bool operator!=(const list_map_iterator<T, Pointer, Reference>& a, 
						   const list_map_iterator<T, Pointer, Reference>& b)
	{
		return a.mpNode != b.mpNode;
	}


	///////////////////////////////////////////////////////////////////////
	// list_map
	///////////////////////////////////////////////////////////////////////

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline list_map<Key, T, Compare, Allocator>::list_map(const allocator_type& allocator)
		: base_type(allocator)
	{
		mNode.mpNext = &mNode;
		mNode.mpPrev = &mNode;
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline list_map<Key, T, Compare, Allocator>::list_map(const Compare& compare, const allocator_type& allocator)
		: base_type(compare, allocator)
	{
		mNode.mpNext = &mNode;
		mNode.mpPrev = &mNode;
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::iterator 
	list_map<Key, T, Compare, Allocator>::begin() EA_NOEXCEPT
	{
		return iterator(mNode.mpNext);
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::const_iterator 
	list_map<Key, T, Compare, Allocator>::begin() const EA_NOEXCEPT
	{
		return const_iterator(mNode.mpNext);
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::const_iterator 
	list_map<Key, T, Compare, Allocator>::cbegin() const EA_NOEXCEPT
	{
		return const_iterator(mNode.mpNext);
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::iterator 
	list_map<Key, T, Compare, Allocator>::end() EA_NOEXCEPT
	{
		return iterator(&mNode);
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::const_iterator 
	list_map<Key, T, Compare, Allocator>::end() const EA_NOEXCEPT
	{
		return const_iterator(&mNode);
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::const_iterator 
	list_map<Key, T, Compare, Allocator>::cend() const EA_NOEXCEPT
	{
		return const_iterator(&mNode);
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::reverse_iterator 
	list_map<Key, T, Compare, Allocator>::rbegin() EA_NOEXCEPT
	{
		return reverse_iterator(&mNode);
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::const_reverse_iterator 
	list_map<Key, T, Compare, Allocator>::rbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(&mNode);
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::const_reverse_iterator 
	list_map<Key, T, Compare, Allocator>::crbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(&mNode);
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::reverse_iterator 
	list_map<Key, T, Compare, Allocator>::rend() EA_NOEXCEPT
	{
		return reverse_iterator(mNode.mpNext);
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::const_reverse_iterator 
	list_map<Key, T, Compare, Allocator>::rend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(mNode.mpNext);
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::const_reverse_iterator 
	list_map<Key, T, Compare, Allocator>::crend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(mNode.mpNext);
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::reference
	list_map<Key, T, Compare, Allocator>::front()
	{
		#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			// We allow the user to reference an empty container.
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(static_cast<internal_value_type*>(mNode.mpNext) == &mNode))
				EASTL_FAIL_MSG("list_map::front -- empty container");
		#endif

		return static_cast<internal_value_type*>(mNode.mpNext)->mValue;
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::const_reference
	list_map<Key, T, Compare, Allocator>::front() const
	{
		#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			// We allow the user to reference an empty container.
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(static_cast<internal_value_type*>(mNode.mpNext) == &mNode))
				EASTL_FAIL_MSG("list_map::front -- empty container");
		#endif

		return static_cast<internal_value_type*>(mNode.mpNext)->mValue;
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::reference
	list_map<Key, T, Compare, Allocator>::back()
	{
		#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			// We allow the user to reference an empty container.
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(static_cast<internal_value_type*>(mNode.mpNext) == &mNode))
				EASTL_FAIL_MSG("list_map::back -- empty container");
		#endif

		return static_cast<internal_value_type*>(mNode.mpPrev)->mValue;
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::const_reference
	list_map<Key, T, Compare, Allocator>::back() const
	{
		#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			// We allow the user to reference an empty container.
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(static_cast<internal_value_type*>(mNode.mpNext) == &mNode))
				EASTL_FAIL_MSG("list_map::back -- empty container");
		#endif

		return static_cast<internal_value_type*>(mNode.mpPrev)->mValue;
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	bool list_map<Key, T, Compare, Allocator>::push_front(const value_type& value)
	{
		internal_value_type tempValue(value);
		typename base_type::insert_return_type baseReturn = base_type::insert(tempValue);

		// Did the insert succeed?
		if (baseReturn.second)
		{
			internal_value_type* pNode = &(*baseReturn.first);

			pNode->mpNext = mNode.mpNext;
			pNode->mpPrev = &mNode;

			mNode.mpNext->mpPrev = pNode;
			mNode.mpNext = pNode;

			return true;
		}
		else
		{
			return false;
		}
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	bool list_map<Key, T, Compare, Allocator>::push_back(const value_type& value)
	{
		internal_value_type tempValue(value);
		typename base_type::insert_return_type baseReturn = base_type::insert(tempValue);

		// Did the insert succeed?
		if (baseReturn.second)
		{
			internal_value_type* pNode = &(*baseReturn.first);

			pNode->mpPrev = mNode.mpPrev;
			pNode->mpNext = &mNode;

			mNode.mpPrev->mpNext = pNode;
			mNode.mpPrev = pNode;

			return true;
		}
		else
		{
			return false;
		}
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	bool list_map<Key, T, Compare, Allocator>::push_front(const key_type& key, const mapped_type& value)
	{
		return push_front(eastl::make_pair(key, value));
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	bool list_map<Key, T, Compare, Allocator>::push_back(const key_type& key, const mapped_type& value)
	{
		return push_back(eastl::make_pair(key, value));
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	void list_map<Key, T, Compare, Allocator>::pop_front()
	{
		#if EASTL_ASSERT_ENABLED
			if (EASTL_UNLIKELY(empty()))
				EASTL_FAIL_MSG("list_map::pop_front -- empty container");
		#endif

		erase(static_cast<internal_value_type*>(mNode.mpNext)->mValue.first);
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	void list_map<Key, T, Compare, Allocator>::pop_back()
	{
		#if EASTL_ASSERT_ENABLED
			if (EASTL_UNLIKELY(empty()))
				EASTL_FAIL_MSG("list_map::pop_back -- empty container");
		#endif

		erase(static_cast<internal_value_type*>(mNode.mpPrev)->mValue.first);
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::iterator 
	list_map<Key, T, Compare, Allocator>::find(const key_type& key)
	{
		typename base_type::iterator baseIter = base_type::find(key);
		if (baseIter != base_type::end())
		{
			return iterator(&(*baseIter));
		}
		else
		{
			return end();
		}
	}
	
	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::const_iterator
	list_map<Key, T, Compare, Allocator>::find(const key_type& key) const
	{
		typename base_type::const_iterator baseIter = base_type::find(key);
		if (baseIter != base_type::end())
		{
			return const_iterator(&(*baseIter));
		}
		else
		{
			return end();
		}
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	template <typename U, typename Compare2>
	inline typename list_map<Key, T, Compare, Allocator>::iterator
	list_map<Key, T, Compare, Allocator>::find_as(const U& u, Compare2 compare2)
	{
		typename base_type::iterator baseIter = base_type::find_as(u, compare2);
		if (baseIter != base_type::end())
		{
			return iterator(&(*baseIter));
		}
		else
		{
			return end();
		}
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	template <typename U, typename Compare2>
	inline typename list_map<Key, T, Compare, Allocator>::const_iterator
	list_map<Key, T, Compare, Allocator>::find_as(const U& u, Compare2 compare2) const
	{
		typename base_type::const_iterator baseIter = base_type::find_as(u, compare2);
		if (baseIter != base_type::end())
		{
			return const_iterator(&(*baseIter));
		}
		else
		{
			return end();
		}
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::size_type
	list_map<Key, T, Compare, Allocator>::count(const key_type& key) const
	{
		const typename base_type::const_iterator it = base_type::find(key);
		return (it != base_type::end()) ? 1 : 0;
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::size_type
	list_map<Key, T, Compare, Allocator>::erase(const key_type& key)
	{
		typename base_type::iterator baseIter = base_type::find(key);
		if (baseIter != base_type::end())
		{
			internal_value_type* node = &(*baseIter);

			node->mpNext->mpPrev = node->mpPrev;
			node->mpPrev->mpNext = node->mpNext;

			base_type::erase(baseIter);

			return 1;
		}
		return 0;
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::iterator
	list_map<Key, T, Compare, Allocator>::erase(const_iterator position)
	{
		iterator posIter(position.mpNode); // Convert from const.
		iterator eraseIter(posIter++);
		erase(eraseIter->first);
		return posIter;
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	inline typename list_map<Key, T, Compare, Allocator>::reverse_iterator
	list_map<Key, T, Compare, Allocator>::erase(const_reverse_iterator position)
	{
		return reverse_iterator(erase((++position).base()));
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	void list_map<Key, T, Compare, Allocator>::clear()
	{
		base_type::clear();

		mNode.mpNext = &mNode;
		mNode.mpPrev = &mNode;
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	void list_map<Key, T, Compare, Allocator>::reset_lose_memory()
	{
		base_type::reset_lose_memory();

		mNode.mpNext = &mNode;
		mNode.mpPrev = &mNode;
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	bool list_map<Key, T, Compare, Allocator>::validate() const
	{
		if (!base_type::validate())
		{
			return false;
		}

		size_type nodeCount(0);
		list_map_data_base* node = mNode.mpNext;
		while (node != &mNode)
		{
			internal_value_type* data = static_cast<internal_value_type*>(node);
			if (base_type::find(data->mValue.first) == base_type::end())
			{
				return false;
			}
			node = node->mpNext;
			++nodeCount;
		}
		if (nodeCount != size())
		{
			return false;
		}
		nodeCount = 0;
		node = mNode.mpPrev;
		while (node != &mNode)
		{
			internal_value_type* data = static_cast<internal_value_type*>(node);
			if (base_type::find(data->mValue.first) == base_type::end())
			{
				return false;
			}
			node = node->mpPrev;
			++nodeCount;
		}
		if (nodeCount != size())
		{
			return false;
		}

		return true;
	}

	template <typename Key, typename T, typename Compare, typename Allocator>
	int list_map<Key, T, Compare, Allocator>::validate_iterator(const_iterator iter) const
	{
		for (const_iterator temp = begin(), tempEnd = end(); temp != tempEnd; ++temp)
		{
			if (temp == iter)
			{
				return (isf_valid | isf_current | isf_can_dereference);
			}
		}

		if (iter == end())
			return (isf_valid | isf_current); 

		return isf_none;
	}


} // namespace eastl


#endif // Header include guard




