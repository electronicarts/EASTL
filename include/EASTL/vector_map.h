///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// This file implements vector_map. It acts much like std::map, except its 
// underlying representation is a random access container such as vector. 
// These containers are sometimes also known as "sorted vectors."  
// vector_maps have an advantage over conventional maps in that their memory
// is contiguous and node-less. The result is that lookups are faster, more 
// cache friendly (which potentially more so benefits speed), and the container
// uses less memory. The downside is that inserting new items into the container
// is slower if they are inserted in random order instead of in sorted order.
// This tradeoff is well-worth it for many cases. Note that vector_map allows
// you to use a deque or other random access container which may perform
// better for you than vector.
//
// Note that with vector_set, vector_multiset, vector_map, vector_multimap
// that the modification of the container potentially invalidates all 
// existing iterators into the container, unlike what happens with conventional
// sets and maps.
// 
// This type could conceptually use a eastl::array as its underlying container,
// however the current design requires an allocator aware container.
// Consider using a fixed_vector instead.
//////////////////////////////////////////////////////////////////////////////



#ifndef EASTL_VECTOR_MAP_H
#define EASTL_VECTOR_MAP_H



#include <EASTL/internal/config.h>
#include <EASTL/allocator.h>
#include <EASTL/functional.h>
#include <EASTL/vector.h>
#include <EASTL/utility.h>
#include <EASTL/algorithm.h>
#include <EASTL/initializer_list.h>
#include <stddef.h>
#if EASTL_EXCEPTIONS_ENABLED
#include <stdexcept>
#endif

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// EASTL_VECTOR_MAP_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_VECTOR_MAP_DEFAULT_NAME
		#define EASTL_VECTOR_MAP_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " vector_map" // Unless the user overrides something, this is "EASTL vector_map".
	#endif


	/// EASTL_VECTOR_MAP_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_VECTOR_MAP_DEFAULT_ALLOCATOR
		#define EASTL_VECTOR_MAP_DEFAULT_ALLOCATOR allocator_type(EASTL_VECTOR_MAP_DEFAULT_NAME)
	#endif



	/// map_value_compare
	///
	/// Our adapter for the comparison function in the template parameters.
	///
	template <typename Key, typename Value, typename Compare>
	class map_value_compare : public Compare
	{
	public:
		explicit map_value_compare(const Compare& x)
			: Compare(x) {}

		bool operator()(const Value& a, const Value& b) const
			{ return Compare::operator()(a.first, b.first); }

		bool operator()(const Value& a, const Key& b) const
			{ return Compare::operator()(a.first, b); }

		bool operator()(const Key& a, const Value& b) const
			{ return Compare::operator()(a, b.first); }

		bool operator()(const Key& a, const Key& b) const
			{ return Compare::operator()(a, b); }

	}; // map_value_compare



	/// vector_map
	///
	/// Implements a map via a random access container such as a vector.
	///
	/// Note that with vector_set, vector_multiset, vector_map, vector_multimap
	/// that the modification of the container potentially invalidates all 
	/// existing iterators into the container, unlike what happens with conventional
	/// sets and maps.
	///
	/// This type could conceptually use a eastl::array as its underlying container,
	/// however the current design requires an allocator aware container.
	/// Consider using a fixed_vector instead.
	///
	/// Note that we set the value_type to be pair<Key, T> and not pair<const Key, T>.
	/// This means that the underlying container (e.g vector) is a container of pair<Key, T>.
	/// Our vector and deque implementations are optimized to assign values in-place and 
	/// using a vector of pair<const Key, T> (note the const) would make it hard to use
	/// our existing vector implementation without a lot of headaches. As a result, 
	/// at least for the time being we do away with the const. This means that the 
	/// insertion type varies between map and vector_map in that the latter doesn't take
	/// const. This also means that a certain amount of automatic safety provided by 
	/// the implementation is lost, as the compiler will let the wayward user modify 
	/// a key and thus make the container no longer ordered behind its back.
	///
	template <typename Key, typename T, typename Compare = eastl::less<Key>, 
			  typename Allocator = EASTLAllocatorType, 
			  typename RandomAccessContainer = eastl::vector<eastl::pair<Key, T>, Allocator> >
	class vector_map : protected map_value_compare<Key, eastl::pair<Key, T>, Compare>, public RandomAccessContainer
	{
	public:
		typedef RandomAccessContainer                                         base_type;
		typedef vector_map<Key, T, Compare, Allocator, RandomAccessContainer> this_type;
		typedef Allocator                                                     allocator_type;
		typedef Key                                                           key_type;
		typedef T                                                             mapped_type;
		typedef eastl::pair<Key, T>                                           value_type;
		typedef Compare                                                       key_compare;
		typedef map_value_compare<Key, value_type, Compare>                   value_compare;
		typedef value_type*                                                   pointer;
		typedef const value_type*                                             const_pointer;
		typedef value_type&                                                   reference;
		typedef const value_type&                                             const_reference;
		typedef typename base_type::size_type                                 size_type;
		typedef typename base_type::difference_type                           difference_type;
		typedef typename base_type::iterator                                  iterator;
		typedef typename base_type::const_iterator                            const_iterator;
		typedef typename base_type::reverse_iterator                          reverse_iterator;
		typedef typename base_type::const_reverse_iterator                    const_reverse_iterator;
		typedef eastl::pair<iterator, bool>                                   insert_return_type;

		using base_type::begin;
		using base_type::end;
		using base_type::get_allocator;

	public:
		// We have an empty ctor and a ctor that takes an allocator instead of one for both
		// because this way our RandomAccessContainer wouldn't be required to have an constructor
		// that takes allocator_type. 
		vector_map();
		explicit vector_map(const allocator_type& allocator);
		explicit vector_map(const key_compare& comp, const allocator_type& allocator = EASTL_VECTOR_MAP_DEFAULT_ALLOCATOR);
		vector_map(const this_type& x);
		vector_map(this_type&& x);
		vector_map(this_type&& x, const allocator_type& allocator);
		vector_map(std::initializer_list<value_type> ilist, const key_compare& compare = key_compare(), const allocator_type& allocator = EASTL_VECTOR_MAP_DEFAULT_ALLOCATOR);

		template <typename InputIterator>
		vector_map(InputIterator first, InputIterator last); // allocator arg removed because VC7.1 fails on the default arg. To do: Make a second version of this function without a default arg.

		template <typename InputIterator>
		vector_map(InputIterator first, InputIterator last, const key_compare& compare); // allocator arg removed because VC7.1 fails on the default arg. To do: Make a second version of this function without a default arg.

		this_type& operator=(const this_type& x);
		this_type& operator=(std::initializer_list<value_type> ilist);
		this_type& operator=(this_type&& x);

		void swap(this_type& x);

		const key_compare& key_comp() const;
		key_compare&       key_comp();

		const value_compare& value_comp() const;
		value_compare&       value_comp();

		// Inherited from base class:
		//
		//     allocator_type& get_allocator();
		//     void            set_allocator(const allocator_type& allocator);
		//
		//     iterator       begin();
		//     const_iterator begin() const;
		//     const_iterator cbegin() const;
		//
		//     iterator       end();
		//     const_iterator end() const;
		//     const_iterator cend() const;
		//
		//     reverse_iterator       rbegin();
		//     const_reverse_iterator rbegin() const;
		//     const_reverse_iterator crbegin() const;
		//
		//     reverse_iterator       rend();
		//     const_reverse_iterator rend() const;
		//     const_reverse_iterator crend() const;
		//
		//     size_type size() const;
		//     bool      empty() const;
		//     void      clear();

		template <class... Args>
		eastl::pair<iterator, bool> emplace(Args&&... args);

		template <class... Args> 
		iterator emplace_hint(const_iterator position, Args&&... args);

		template <typename P, typename = eastl::enable_if_t<eastl::is_constructible_v<value_type, P&&>>>
		pair<iterator, bool> insert(P&& otherValue);

		eastl::pair<iterator, bool> insert(const value_type& value);
		pair<iterator, bool>        insert(const key_type& otherValue);
		pair<iterator, bool>        insert(key_type&& otherValue);
		iterator                    insert(const_iterator position, const value_type& value);
		iterator                    insert(const_iterator position, value_type&& value);
		void                        insert(std::initializer_list<value_type> ilist);

		template <typename InputIterator>
		void insert(InputIterator first, InputIterator last);

		iterator         erase(const_iterator position);
		iterator         erase(const_iterator first, const_iterator last);
		size_type        erase(const key_type& k);
		reverse_iterator erase(const_reverse_iterator position);
		reverse_iterator erase(const_reverse_iterator first, const_reverse_iterator last);

		iterator       find(const key_type& k);
		const_iterator find(const key_type& k) const;

		template <typename U, typename BinaryPredicate>
		iterator       find_as(const U& u, BinaryPredicate predicate);

		template <typename U, typename BinaryPredicate>
		const_iterator find_as(const U& u, BinaryPredicate predicate) const;

		size_type count(const key_type& k) const;

		iterator       lower_bound(const key_type& k);
		const_iterator lower_bound(const key_type& k) const;

		iterator       upper_bound(const key_type& k);
		const_iterator upper_bound(const key_type& k) const;

		eastl::pair<iterator, iterator>             equal_range(const key_type& k);
		eastl::pair<const_iterator, const_iterator> equal_range(const key_type& k) const;

		template <typename U, typename BinaryPredicate> 
		eastl::pair<iterator, iterator>             equal_range(const U& u, BinaryPredicate predicate);

		template <typename U, typename BinaryPredicate> 
		eastl::pair<const_iterator, const_iterator> equal_range(const U& u, BinaryPredicate) const;

		// Note: vector_map operator[] returns a reference to the mapped_type, same as map does.
		// But there's an important difference: This reference can be invalidated by -any- changes  
		// to the vector_map that cause it to change capacity. This is unlike map, with which 
		// mapped_type references are invalidated only if that mapped_type element itself is removed
		// from the map. This is because vector is array-based and map is node-based. As a result
		// the following code that is safe for map is unsafe for vector_map for the case that 
		// the vMap[100] doesn't already exist in the vector_map:
		//     vMap[100] = vMap[0]
		mapped_type& operator[](const key_type& k);
		mapped_type& operator[](key_type&& k);

		// non-standard! this was originally inherited from vector with incorrect semantics.
		// this is only defined so that we can deprecate it.
		// use `*(map.begin() + index)` if you want to get an element by index.
		EASTL_REMOVE_AT_2024_SEPT reference at(size_type index);
		EASTL_REMOVE_AT_2024_SEPT const_reference at(size_type index) const;
		// after the deprecation period the above should be replaced with:
		// mapped_type& at(const key_type& k) { return at_key(k); }
		// const mapped_type& at(const key_type& k) const { return at_key(k); }

		// aka. the standard's at() member function.
		mapped_type& at_key(const key_type& k);
		const mapped_type& at_key(const key_type& k) const;

		// Functions which are disallowed due to being unsafe. 
		void      push_back(const value_type& value) = delete;
		reference push_back()                        = delete;
		void*     push_back_uninitialized()          = delete;
		template <class... Args>
		reference emplace_back(Args&&...)            = delete;

		// NOTE(rparolin): It is undefined behaviour if user code fails to ensure the container
		// invariants are respected by performing an explicit call to 'sort' before any other
		// operations on the container are performed that do not clear the elements.
		//
		// 'push_back_unsorted' and 'emplace_back_unsorted' do not satisfy container invariants
		// for being sorted. We provide these overloads explicitly labelled as '_unsorted' as an
		// optimization opportunity when batch inserting elements so users can defer the cost of
		// sorting the container once when all elements are contained. This was done to clarify
		// the intent of code by leaving a trace that a manual call to sort is required.
		// 
		template <typename... Args> decltype(auto) push_back_unsorted(Args&&... args)    
			{ return base_type::push_back(eastl::forward<Args>(args)...); }
		template <typename... Args> decltype(auto) emplace_back_unsorted(Args&&... args) 
			{ return base_type::emplace_back(eastl::forward<Args>(args)...); }

	}; // vector_map





	///////////////////////////////////////////////////////////////////////
	// vector_map
	///////////////////////////////////////////////////////////////////////

	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_map<K, T, C, A, RAC>::vector_map()
		: value_compare(C()), base_type()
	{
		get_allocator().set_name(EASTL_VECTOR_MAP_DEFAULT_NAME);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_map<K, T, C, A, RAC>::vector_map(const allocator_type& allocator)
		: value_compare(C()), base_type(allocator)
	{
		// Empty
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_map<K, T, C, A, RAC>::vector_map(const key_compare& comp, const allocator_type& allocator)
		: value_compare(comp), base_type(allocator)
	{
		// Empty
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_map<K, T, C, A, RAC>::vector_map(const this_type& x)
		: value_compare(x), base_type(x)
	{
		// Empty
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_map<K, T, C, A, RAC>::vector_map(this_type&& x)
		// careful to only copy / move the distinct base sub-objects of x:
		: value_compare(static_cast<value_compare&>(x)), base_type(eastl::move(static_cast<base_type&&>(x)))
	{
		// Empty. Note: x is left with empty contents but its original value_compare instead of the default one. 
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_map<K, T, C, A, RAC>::vector_map(this_type&& x, const allocator_type& allocator)
		// careful to only copy / move the distinct base sub-objects of x:
		: value_compare(static_cast<value_compare&>(x)), base_type(eastl::move(static_cast<base_type&&>(x)), allocator)
	{
		// Empty. Note: x is left with empty contents but its original value_compare instead of the default one. 
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_map<K, T, C, A, RAC>::vector_map(std::initializer_list<value_type> ilist, const key_compare& compare, const allocator_type& allocator)
		: value_compare(compare), base_type(allocator)
	{
		insert(ilist.begin(), ilist.end());
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <typename InputIterator>
	inline vector_map<K, T, C, A, RAC>::vector_map(InputIterator first, InputIterator last)
		: value_compare(key_compare()), base_type(EASTL_VECTOR_MAP_DEFAULT_ALLOCATOR)
	{
		insert(first, last);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <typename InputIterator>
	inline vector_map<K, T, C, A, RAC>::vector_map(InputIterator first, InputIterator last, const key_compare& compare)
		: value_compare(compare), base_type(EASTL_VECTOR_MAP_DEFAULT_ALLOCATOR)
	{
		insert(first, last);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_map<K, T, C, A, RAC>&
	vector_map<K, T, C, A, RAC>::operator=(const this_type& x)
	{
		base_type::operator=(x);
		value_compare::operator=(x);
		return *this;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_map<K, T, C, A, RAC>&
	vector_map<K, T, C, A, RAC>::operator=(this_type&& x)
	{
		base_type::operator=(eastl::move(static_cast<base_type&&>(x)));
		using eastl::swap;
		swap(static_cast<value_compare&>(*this), static_cast<value_compare&>(x));
		return *this;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_map<K, T, C, A, RAC>&
	vector_map<K, T, C, A, RAC>::operator=(std::initializer_list<value_type> ilist)
	{
		base_type::clear();
		insert(ilist.begin(), ilist.end());
		return *this;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline void vector_map<K, T, C, A, RAC>::swap(this_type& x)
	{
		base_type::swap(x);
		using eastl::swap;
		swap(static_cast<value_compare&>(*this), static_cast<value_compare&>(x));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline const typename vector_map<K, T, C, A, RAC>::key_compare&
	vector_map<K, T, C, A, RAC>::key_comp() const
	{
		return static_cast<const key_compare&>(*this);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::key_compare&
	vector_map<K, T, C, A, RAC>::key_comp()
	{
		return static_cast<key_compare&>(*this);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline const typename vector_map<K, T, C, A, RAC>::value_compare&
	vector_map<K, T, C, A, RAC>::value_comp() const
	{
		return static_cast<const value_compare&>(*this);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::value_compare&
	vector_map<K, T, C, A, RAC>::value_comp()
	{
		return static_cast<value_compare&>(*this);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <class... Args>
	inline eastl::pair<typename vector_map<K, T, C, A, RAC>::iterator, bool>
	vector_map<K, T, C, A, RAC>::emplace(Args&&... args)
	{
		#if EASTL_USE_FORWARD_WORKAROUND
			auto value = value_type(eastl::forward<Args>(args)...);  // Workaround for compiler bug in VS2013 which results in a compiler internal crash while compiling this code.
		#else
			value_type  value(eastl::forward<Args>(args)...);
		#endif
		return insert(eastl::move(value));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <class... Args>
	inline typename vector_map<K, T, C, A, RAC>::iterator 
	vector_map<K, T, C, A, RAC>::emplace_hint(const_iterator position, Args&&... args)
	{
		#if EASTL_USE_FORWARD_WORKAROUND
			auto value = value_type(eastl::forward<Args>(args)...);  // Workaround for compiler bug in VS2013 which results in a compiler internal crash while compiling this code.
		#else
			value_type  value(eastl::forward<Args>(args)...);
		#endif

		return insert(position, eastl::move(value));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline eastl::pair<typename vector_map<K, T, C, A, RAC>::iterator, bool>
	vector_map<K, T, C, A, RAC>::insert(const value_type& value)
	{
		const iterator itLB(lower_bound(value.first));

		if((itLB != end()) && !value_compare::operator()(value, *itLB))
			return eastl::pair<iterator, bool>(itLB, false);

		return eastl::pair<iterator, bool>(base_type::insert(itLB, value), true);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <typename P, typename>
	inline eastl::pair<typename vector_map<K, T, C, A, RAC>::iterator, bool>
	vector_map<K, T, C, A, RAC>::insert(P&& otherValue)
	{
		value_type value(eastl::forward<P>(otherValue));
		const iterator itLB(lower_bound(value.first));

		if((itLB != end()) && !value_compare::operator()(value, *itLB))
			return eastl::pair<iterator, bool>(itLB, false);

		return eastl::pair<iterator, bool>(base_type::insert(itLB, eastl::move(value)), true);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline eastl::pair<typename vector_map<K, T, C, A, RAC>::iterator, bool>
	vector_map<K, T, C, A, RAC>::insert(const key_type& otherValue)
	{
		value_type value(eastl::pair_first_construct, otherValue);
		const iterator itLB(lower_bound(value.first));

		if((itLB != end()) && !value_compare::operator()(value, *itLB))
			return eastl::pair<iterator, bool>(itLB, false);

		return eastl::pair<iterator, bool>(base_type::insert(itLB, eastl::move(value)), true);
	}

	template <typename K, typename T, typename C, typename A, typename RAC>
	inline eastl::pair<typename vector_map<K, T, C, A, RAC>::iterator, bool>
	vector_map<K, T, C, A, RAC>::insert(key_type&& otherValue)
	{
		value_type value(eastl::pair_first_construct, eastl::move(otherValue));
		const iterator itLB(lower_bound(value.first));

		if((itLB != end()) && !value_compare::operator()(value, *itLB))
			return eastl::pair<iterator, bool>(itLB, false);

		return eastl::pair<iterator, bool>(base_type::insert(itLB, eastl::move(value)), true);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	typename vector_map<K, T, C, A, RAC>::iterator
	vector_map<K, T, C, A, RAC>::insert(const_iterator position, const value_type& value)
	{
		// We assume that the user knows what he is doing and has supplied us with
		// a position that is right where value should be inserted (put in front of). 
		// We do a test to see if the position is correct. If so then we insert, 
		// if not then we ignore the input position.

		if((position == end()) || value_compare::operator()(value, *position))  // If the element at position is greater than value...
		{
			if((position == begin()) || value_compare::operator()(*(position - 1), value)) // If the element before position is less than value...
				return base_type::insert(position, value);
		}

		// In this case we either have an incorrect position or value is already present.
		// We fall back to the regular insert function. An optimization would be to detect
		// that the element is already present, but that's only useful if the user supplied
		// a good position but a present element.
		const eastl::pair<typename vector_map<K, T, C, A, RAC>::iterator, bool> result = insert(value);

		return result.first;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	typename vector_map<K, T, C, A, RAC>::iterator
	vector_map<K, T, C, A, RAC>::insert(const_iterator position, value_type&& value)
	{
		if((position == end()) || value_compare::operator()(value, *position))  // If the element at position is greater than value...
		{
			if((position == begin()) || value_compare::operator()(*(position - 1), value)) // If the element before position is less than value...
				return base_type::insert(position, eastl::move(value));
		}

		const eastl::pair<typename vector_map<K, T, C, A, RAC>::iterator, bool> result = insert(eastl::move(value));

		return result.first;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline void vector_map<K, T, C, A, RAC>::insert(std::initializer_list<value_type> ilist)
	{
		insert(ilist.begin(), ilist.end());
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <typename InputIterator>
	inline void vector_map<K, T, C, A, RAC>::insert(InputIterator first, InputIterator last)
	{
		// To consider: Improve the speed of this by getting the length of the 
		//              input range and resizing our container to that size
		//              before doing the insertions. We can't use reserve
		//              because we don't know if we are using a vector or not.
		//              Alternatively, force the user to do the reservation.
		// To consider: When inserting values that come from a container
		//              like this container, use the property that they are 
		//              known to be sorted and speed up the inserts here.
		for(; first != last; ++first)
			insert(*first);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::iterator
	vector_map<K, T, C, A, RAC>::erase(const_iterator position)
	{
		// Note that we return iterator and not void. This allows for more efficient use of 
		// the container and is consistent with the C++ language defect report #130 (DR 130)
		return base_type::erase(position);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::iterator
	vector_map<K, T, C, A, RAC>::erase(const_iterator first, const_iterator last)
	{
		return base_type::erase(first, last);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::size_type
	vector_map<K, T, C, A, RAC>::erase(const key_type& k)
	{
		const iterator it(find(k));

		if(it != end()) // If it exists...
		{
			erase(it);
			return 1;
		}
		return 0;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::reverse_iterator
	vector_map<K, T, C, A, RAC>::erase(const_reverse_iterator position)
	{
		return reverse_iterator(base_type::erase((++position).base()));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::reverse_iterator
	vector_map<K, T, C, A, RAC>::erase(const_reverse_iterator first, const_reverse_iterator last)
	{
		return reverse_iterator(base_type::erase((++last).base(), (++first).base()));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::iterator
	vector_map<K, T, C, A, RAC>::find(const key_type& k)
	{
		const eastl::pair<iterator, iterator> pairIts(equal_range(k));
		return (pairIts.first != pairIts.second) ? pairIts.first : end();
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::const_iterator
	vector_map<K, T, C, A, RAC>::find(const key_type& k) const
	{
		const eastl::pair<const_iterator, const_iterator> pairIts(equal_range(k));
		return (pairIts.first != pairIts.second) ? pairIts.first : end();
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <typename U, typename BinaryPredicate>
	inline typename vector_map<K, T, C, A, RAC>::iterator
	vector_map<K, T, C, A, RAC>::find_as(const U& u, BinaryPredicate predicate)
	{
		const eastl::pair<iterator, iterator> pairIts(equal_range(u, predicate));
		return (pairIts.first != pairIts.second) ? pairIts.first : end();
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <typename U, typename BinaryPredicate>
	inline typename vector_map<K, T, C, A, RAC>::const_iterator
	vector_map<K, T, C, A, RAC>::find_as(const U& u, BinaryPredicate predicate) const
	{
		const eastl::pair<const_iterator, const_iterator> pairIts(equal_range(u, predicate));
		return (pairIts.first != pairIts.second) ? pairIts.first : end();
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::size_type
	vector_map<K, T, C, A, RAC>::count(const key_type& k) const
	{
		const const_iterator it(find(k));
		return (it != end()) ? (size_type)1 : (size_type)0;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::iterator
	vector_map<K, T, C, A, RAC>::lower_bound(const key_type& k)
	{
		return eastl::lower_bound(begin(), end(), k, static_cast<value_compare&>(*this));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::const_iterator
	vector_map<K, T, C, A, RAC>::lower_bound(const key_type& k) const
	{
		return eastl::lower_bound(begin(), end(), k, static_cast<const value_compare&>(*this));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::iterator 
	vector_map<K, T, C, A, RAC>::upper_bound(const key_type& k)
	{
		return eastl::upper_bound(begin(), end(), k, static_cast<value_compare&>(*this));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::const_iterator
	vector_map<K, T, C, A, RAC>::upper_bound(const key_type& k) const
	{
		return eastl::upper_bound(begin(), end(), k, static_cast<const value_compare&>(*this));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline eastl::pair<typename vector_map<K, T, C, A, RAC>::iterator, typename vector_map<K, T, C, A, RAC>::iterator>
	vector_map<K, T, C, A, RAC>::equal_range(const key_type& k)
	{
		// The resulting range will either be empty or have one element,
		// so instead of doing two tree searches (one for lower_bound and 
		// one for upper_bound), we do just lower_bound and see if the 
		// result is a range of size zero or one.
		const iterator itLower(lower_bound(k));

		if((itLower == end()) || value_compare::operator()(k, *itLower)) // If at the end or if (k is < itLower)...
			return eastl::pair<iterator, iterator>(itLower, itLower);

		iterator itUpper(itLower);
		return eastl::pair<iterator, iterator>(itLower, ++itUpper);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline eastl::pair<typename vector_map<K, T, C, A, RAC>::const_iterator, typename vector_map<K, T, C, A, RAC>::const_iterator>
	vector_map<K, T, C, A, RAC>::equal_range(const key_type& k) const
	{
		// The resulting range will either be empty or have one element,
		// so instead of doing two tree searches (one for lower_bound and 
		// one for upper_bound), we do just lower_bound and see if the 
		// result is a range of size zero or one.
		const const_iterator itLower(lower_bound(k));

		if((itLower == end()) || value_compare::operator()(k, *itLower)) // If at the end or if (k is < itLower)...
			return eastl::pair<const_iterator, const_iterator>(itLower, itLower);

		const_iterator itUpper(itLower);
		return eastl::pair<const_iterator, const_iterator>(itLower, ++itUpper);
	}

	template <typename K, typename T, typename C, typename A, typename RAC>
	template <typename U, typename BinaryPredicate> 
	inline eastl::pair<typename vector_map<K, T, C, A, RAC>::iterator, typename vector_map<K, T, C, A, RAC>::iterator>
	vector_map<K, T, C, A, RAC>::equal_range(const U& u, BinaryPredicate predicate)
	{
		// The resulting range will either be empty or have one element,
		// so instead of doing two tree searches (one for lower_bound and 
		// one for upper_bound), we do just lower_bound and see if the 
		// result is a range of size zero or one.
		map_value_compare<U, value_type, BinaryPredicate> predicate_cmp(predicate);

		const iterator itLower(eastl::lower_bound(begin(), end(), u, predicate_cmp));

		if((itLower == end()) || predicate_cmp(u, *itLower)) // If at the end or if (k is < itLower)...
			return eastl::pair<iterator, iterator>(itLower, itLower);

		iterator itUpper(itLower);
		return eastl::pair<iterator, iterator>(itLower, ++itUpper);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <typename U, typename BinaryPredicate> 
	inline eastl::pair<typename vector_map<K, T, C, A, RAC>::const_iterator, typename vector_map<K, T, C, A, RAC>::const_iterator>
	vector_map<K, T, C, A, RAC>::equal_range(const U& u, BinaryPredicate predicate) const
	{
		// The resulting range will either be empty or have one element,
		// so instead of doing two tree searches (one for lower_bound and 
		// one for upper_bound), we do just lower_bound and see if the 
		// result is a range of size zero or one.
		map_value_compare<U, value_type, BinaryPredicate> predicate_cmp(predicate);

		const const_iterator itLower(eastl::lower_bound(begin(), end(), u, predicate_cmp));

		if((itLower == end()) || predicate_cmp(u, *itLower)) // If at the end or if (k is < itLower)...
			return eastl::pair<const_iterator, const_iterator>(itLower, itLower);

		const_iterator itUpper(itLower);
		return eastl::pair<const_iterator, const_iterator>(itLower, ++itUpper);
	}



	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::mapped_type&
	vector_map<K, T, C, A, RAC>::operator[](const key_type& k)
	{
		iterator itLB(lower_bound(k));

		if((itLB == end()) || key_comp()(k, (*itLB).first))
			itLB = insert(itLB, value_type(k, mapped_type()));
		return (*itLB).second;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::mapped_type&
	vector_map<K, T, C, A, RAC>::operator[](key_type&& k)
	{
		iterator itLB(lower_bound(k));

		if((itLB == end()) || key_comp()(k, (*itLB).first))
			itLB = insert(itLB, value_type(eastl::move(k), mapped_type()));
		return (*itLB).second;
	}

	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::reference
		vector_map<K, T, C, A, RAC>::at(size_type index)
	{
		return *(begin() + index);
	}

	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::const_reference
		vector_map<K, T, C, A, RAC>::at(size_type index) const
	{
		return *(begin() + index);
	}

	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_map<K, T, C, A, RAC>::mapped_type&
		vector_map<K, T, C, A, RAC>::at_key(const key_type& k)
	{
		// use the use const version of ::at to remove duplication
		return const_cast<mapped_type&>(const_cast<vector_map<K, T, C, A, RAC> const*>(this)->at_key(k));
	}

	template <typename K, typename T, typename C, typename A, typename RAC>
	inline const typename vector_map<K, T, C, A, RAC>::mapped_type&
		vector_map<K, T, C, A, RAC>::at_key(const key_type& k) const
	{
		const_iterator itLB(lower_bound(k));

		if ((itLB == end()) || key_comp()(k, itLB->first))
		{
#if EASTL_EXCEPTIONS_ENABLED
			throw std::out_of_range("vector_map::at key does not exist");
#else
			EASTL_FAIL_MSG("vector_map::at key does not exist");
#endif
		}

		return itLB->second;
	}



	///////////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////////

	template <typename Key, typename T, typename Compare, typename Allocator, typename RandomAccessContainer>
	inline bool operator==(const vector_map<Key, T, Compare, Allocator, RandomAccessContainer>& a, 
						   const vector_map<Key, T, Compare, Allocator, RandomAccessContainer>& b) 
	{
		return (a.size() == b.size()) && eastl::equal(b.begin(), b.end(), a.begin()); 
	}


	template <typename Key, typename T, typename Compare, typename Allocator, typename RandomAccessContainer>
	inline bool operator<(const vector_map<Key, T, Compare, Allocator, RandomAccessContainer>& a,
						  const vector_map<Key, T, Compare, Allocator, RandomAccessContainer>& b)
	{
		return eastl::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), a.value_comp());
	}


	template <typename Key, typename T, typename Compare, typename Allocator, typename RandomAccessContainer>
	inline bool operator!=(const vector_map<Key, T, Compare, Allocator, RandomAccessContainer>& a,
						   const vector_map<Key, T, Compare, Allocator, RandomAccessContainer>& b)
	{
		return !(a == b);
	}


	template <typename Key, typename T, typename Compare, typename Allocator, typename RandomAccessContainer>
	inline bool operator>(const vector_map<Key, T, Compare, Allocator, RandomAccessContainer>& a,
						  const vector_map<Key, T, Compare, Allocator, RandomAccessContainer>& b)
	{
		return b < a;
	}


	template <typename Key, typename T, typename Compare, typename Allocator, typename RandomAccessContainer>
	inline bool operator<=(const vector_map<Key, T, Compare, Allocator, RandomAccessContainer>& a,
						   const vector_map<Key, T, Compare, Allocator, RandomAccessContainer>& b)
	{
		return !(b < a);
	}


	template <typename Key, typename T, typename Compare, typename Allocator, typename RandomAccessContainer>
	inline bool operator>=(const vector_map<Key, T, Compare, Allocator, RandomAccessContainer>& a,
						   const vector_map<Key, T, Compare, Allocator, RandomAccessContainer>& b)
	{
		return !(a < b);
	}


	template <typename Key, typename T, typename Compare, typename Allocator, typename RandomAccessContainer>
	inline void swap(vector_map<Key, T, Compare, Allocator, RandomAccessContainer>& a,
					 vector_map<Key, T, Compare, Allocator, RandomAccessContainer>& b)
	{
		a.swap(b);
	}


} // namespace eastl


#endif // Header include guard

















