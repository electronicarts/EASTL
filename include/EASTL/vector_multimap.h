///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// This file implements vector_multimap. It acts much like std::multimap, except 
// its underlying representation is a random access container such as vector. 
// These containers are sometimes also known as "sorted vectors."  
// vector_maps have an advantage over conventional maps in that their memory
// is contiguous and node-less. The result is that lookups are faster, more 
// cache friendly (which potentially more so benefits speed), and the container
// uses less memory. The downside is that inserting new items into the container
// is slower if they are inserted in random order instead of in sorted order.
// This tradeoff is well-worth it for many cases. Note that vector_multimap allows
// you to use a deque or other random access container which may perform
// better for you than vector.
//
// Note that with vector_set, vector_multiset, vector_map, vector_multimap
// that the modification of the container potentially invalidates all 
// existing iterators into the container, unlike what happens with conventional
// sets and maps.
//////////////////////////////////////////////////////////////////////////////



#ifndef EASTL_VECTOR_MULTIMAP_H
#define EASTL_VECTOR_MULTIMAP_H



#include <EASTL/internal/config.h>
#include <EASTL/allocator.h>
#include <EASTL/functional.h>
#include <EASTL/vector.h>
#include <EASTL/utility.h>
#include <EASTL/algorithm.h>
#include <EASTL/initializer_list.h>
#include <stddef.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// EASTL_VECTOR_MULTIMAP_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_VECTOR_MULTIMAP_DEFAULT_NAME
		#define EASTL_VECTOR_MULTIMAP_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " vector_multimap" // Unless the user overrides something, this is "EASTL vector_multimap".
	#endif


	/// EASTL_VECTOR_MULTIMAP_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_VECTOR_MULTIMAP_DEFAULT_ALLOCATOR
		#define EASTL_VECTOR_MULTIMAP_DEFAULT_ALLOCATOR allocator_type(EASTL_VECTOR_MULTIMAP_DEFAULT_NAME)
	#endif



	/// multimap_value_compare
	///
	/// Our adapter for the comparison function in the template parameters.
	///
	template <typename Key, typename Value, typename Compare>
	class multimap_value_compare : public binary_function<Value, Value, bool>
	{
	public:
		Compare c;

		multimap_value_compare(const Compare& x)
			: c(x) {}

	public:
		bool operator()(const Value& a, const Value& b) const
			{ return c(a.first, b.first); }

		bool operator()(const Value& a, const Key& b) const
			{ return c(a.first, b); }

		bool operator()(const Key& a, const Value& b) const
			{ return c(a, b.first); }

		bool operator()(const Key& a, const Key& b) const
			{ return c(a, b); }

	}; // multimap_value_compare



	/// vector_multimap
	///
	/// Implements a multimap via a random access container such as a vector.
	///
	/// Note that with vector_set, vector_multiset, vector_map, vector_multimap
	/// that the modification of the container potentially invalidates all 
	/// existing iterators into the container, unlike what happens with conventional
	/// sets and maps.
	///
	/// Note that the erase functions return iterator and not void. This allows for 
	/// more efficient use of the container and is consistent with the C++ language 
	/// defect report #130 (DR 130)
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
	class vector_multimap : public RandomAccessContainer
	{
	public:
		typedef RandomAccessContainer                                              base_type;
		typedef vector_multimap<Key, T, Compare, Allocator, RandomAccessContainer> this_type;
		typedef Allocator                                                          allocator_type;
		typedef Key                                                                key_type;
		typedef T                                                                  mapped_type;
		typedef eastl::pair<Key, T>                                                value_type;
		typedef Compare                                                            key_compare;
		typedef multimap_value_compare<Key, value_type, Compare>                   value_compare;
		typedef value_type*                                                        pointer;
		typedef const value_type*                                                  const_pointer;
		typedef value_type&                                                        reference;
		typedef const value_type&                                                  const_reference;
		typedef typename base_type::size_type                                      size_type;
		typedef typename base_type::difference_type                                difference_type;
		typedef typename base_type::iterator                                       iterator;
		typedef typename base_type::const_iterator                                 const_iterator;
		typedef typename base_type::reverse_iterator                               reverse_iterator;
		typedef typename base_type::const_reverse_iterator                         const_reverse_iterator;

		using base_type::begin;
		using base_type::end;
		using base_type::get_allocator;

	protected:
		value_compare mValueCompare;

	public:
		// We have an empty ctor and a ctor that takes an allocator instead of one for both
		// because this way our RandomAccessContainer wouldn't be required to have an constructor
		// that takes allocator_type. 
		vector_multimap();
		explicit vector_multimap(const allocator_type& allocator);
		explicit vector_multimap(const key_compare& comp, const allocator_type& allocator = EASTL_VECTOR_MULTIMAP_DEFAULT_ALLOCATOR);
		vector_multimap(const this_type& x);
		vector_multimap(this_type&& x);
		vector_multimap(this_type&& x, const allocator_type& allocator);
		vector_multimap(std::initializer_list<value_type> ilist, const key_compare& compare = key_compare(), const allocator_type& allocator = EASTL_VECTOR_MULTIMAP_DEFAULT_ALLOCATOR);

		template <typename InputIterator>
		vector_multimap(InputIterator first, InputIterator last); // allocator arg removed because VC7.1 fails on the default arg. To do: Make a second version of this function without a default arg.

		template <typename InputIterator>
		vector_multimap(InputIterator first, InputIterator last, const key_compare& compare); // allocator arg removed because VC7.1 fails on the default arg. To do: Make a second version of this function without a default arg.

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
		iterator emplace(Args&&... args);

		template <class... Args> 
		iterator emplace_hint(const_iterator position, Args&&... args);

		iterator insert(const value_type& value);   // The signature of this function was change in EASTL v2.05.00 from (the mistaken) pair<iterator, bool> to (the correct) iterator.

		template <typename P, typename = eastl::enable_if_t<eastl::is_constructible_v<value_type, P&&>>>
		iterator insert(P&& otherValue);

		iterator insert(const key_type& otherValue);
		iterator insert(key_type&& otherValue);

		iterator insert(const_iterator position, const value_type& value);
		iterator insert(const_iterator position, value_type&& value);

		void insert(std::initializer_list<value_type> ilist);

		template <typename InputIterator>
		void insert(InputIterator first, InputIterator last);

		iterator  erase(const_iterator position);
		iterator  erase(const_iterator first, const_iterator last);
		size_type erase(const key_type& k);

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

		/// equal_range_small
		/// This is a special version of equal_range which is optimized for the 
		/// case of there being few or no duplicated keys in the tree.
		eastl::pair<iterator, iterator>             equal_range_small(const key_type& k)
		{
			// Defined inline because VC7.1 is broken for when it's defined outside.
			const iterator itLower(lower_bound(k));
			iterator       itUpper(itLower);

			while((itUpper != end()) && !mValueCompare(k, *itUpper))
				++itUpper;

			return eastl::pair<iterator, iterator>(itLower, itUpper);
		}
		eastl::pair<const_iterator, const_iterator> equal_range_small(const key_type& k) const;

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

	}; // vector_multimap




	///////////////////////////////////////////////////////////////////////
	// vector_multimap
	///////////////////////////////////////////////////////////////////////

	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_multimap<K, T, C, A, RAC>::vector_multimap()
		: base_type(), mValueCompare(C())
	{
	#if EASTL_NAME_ENABLED
		get_allocator().set_name(EASTL_VECTOR_MULTIMAP_DEFAULT_NAME);
	#endif
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_multimap<K, T, C, A, RAC>::vector_multimap(const allocator_type& allocator)
		: base_type(allocator), mValueCompare(C())
	{
		// Empty
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_multimap<K, T, C, A, RAC>::vector_multimap(const key_compare& comp, const allocator_type& allocator)
		: base_type(allocator), mValueCompare(comp)
	{
		// Empty
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_multimap<K, T, C, A, RAC>::vector_multimap(const this_type& x)
		: base_type(x), mValueCompare(x.mValueCompare)
	{
		// Empty
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_multimap<K, T, C, A, RAC>::vector_multimap(this_type&& x)
		: base_type(eastl::move(x)), mValueCompare(x.mValueCompare)
	{
		// Empty. Note: x is left with empty contents but its original mValueCompare instead of the default one. 
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_multimap<K, T, C, A, RAC>::vector_multimap(this_type&& x, const allocator_type& allocator)
		: base_type(eastl::move(x), allocator), mValueCompare(x.mValueCompare)
	{
		// Empty. Note: x is left with empty contents but its original mValueCompare instead of the default one. 
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline vector_multimap<K, T, C, A, RAC>::vector_multimap(std::initializer_list<value_type> ilist, const key_compare& compare, const allocator_type& allocator)
		: base_type(allocator), mValueCompare(compare)
	{
		insert(ilist.begin(), ilist.end());
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <typename InputIterator>
	inline vector_multimap<K, T, C, A, RAC>::vector_multimap(InputIterator first, InputIterator last)
		: base_type(EASTL_VECTOR_MULTIMAP_DEFAULT_ALLOCATOR), mValueCompare(key_compare())
	{
		insert(first, last);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <typename InputIterator>
	inline vector_multimap<K, T, C, A, RAC>::vector_multimap(InputIterator first, InputIterator last, const key_compare& compare)
		: base_type(EASTL_VECTOR_MULTIMAP_DEFAULT_ALLOCATOR), mValueCompare(compare)
	{
		insert(first, last);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::this_type&
	vector_multimap<K, T, C, A, RAC>::operator=(const this_type& x)
	{
		base_type::operator=(x);
		mValueCompare = value_compare(x.mValueCompare);
		return *this;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::this_type&
	vector_multimap<K, T, C, A, RAC>::operator=(this_type&& x)
	{
		base_type::operator=(eastl::move(x));
		eastl::swap(mValueCompare, x.mValueCompare);
		return *this;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::this_type&
	vector_multimap<K, T, C, A, RAC>::operator=(std::initializer_list<value_type> ilist)
	{
		base_type::clear();
		insert(ilist.begin(), ilist.end());
		return *this;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline void vector_multimap<K, T, C, A, RAC>::swap(this_type& x)
	{
		base_type::swap(x);
		eastl::swap(mValueCompare, x.mValueCompare);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline const typename vector_multimap<K, T, C, A, RAC>::key_compare&
	vector_multimap<K, T, C, A, RAC>::key_comp() const
	{
		return mValueCompare.c;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::key_compare&
	vector_multimap<K, T, C, A, RAC>::key_comp()
	{
		return mValueCompare.c;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline const typename vector_multimap<K, T, C, A, RAC>::value_compare&
	vector_multimap<K, T, C, A, RAC>::value_comp() const
	{
		return mValueCompare;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::value_compare&
	vector_multimap<K, T, C, A, RAC>::value_comp()
	{
		return mValueCompare;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <class... Args>
	inline typename vector_multimap<K, T, C, A, RAC>::iterator
	vector_multimap<K, T, C, A, RAC>::emplace(Args&&... args)
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
	inline typename vector_multimap<K, T, C, A, RAC>::iterator
	vector_multimap<K, T, C, A, RAC>::emplace_hint(const_iterator position, Args&&... args)
	{
		#if EASTL_USE_FORWARD_WORKAROUND
			auto value = value_type(eastl::forward<Args>(args)...);  // Workaround for compiler bug in VS2013 which results in a compiler internal crash while compiling this code.
		#else
			value_type  value(eastl::forward<Args>(args)...);
		#endif
		return insert(position, eastl::move(value));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::iterator
	vector_multimap<K, T, C, A, RAC>::insert(const value_type& value)
	{
		const iterator itUB(upper_bound(value.first));
		return base_type::insert(itUB, value);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <typename P, typename>
	inline typename vector_multimap<K, T, C, A, RAC>::iterator
	vector_multimap<K, T, C, A, RAC>::insert(P&& otherValue)
	{
		value_type value(eastl::forward<P>(otherValue));
		const iterator itUB(upper_bound(value.first));
		return base_type::insert(itUB, eastl::move(value));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::iterator
	vector_multimap<K, T, C, A, RAC>::insert(const key_type& otherValue)
	{
		value_type value(eastl::pair_first_construct, otherValue);
		const iterator itUB(upper_bound(value.first));
		return base_type::insert(itUB, eastl::move(value));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::iterator
	vector_multimap<K, T, C, A, RAC>::insert(key_type&& otherValue)
	{
		value_type value(eastl::pair_first_construct, eastl::move(otherValue));
		const iterator itUB(upper_bound(value.first));
		return base_type::insert(itUB, eastl::move(value));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::iterator
	vector_multimap<K, T, C, A, RAC>::insert(const_iterator position, const value_type& value)
	{
		// We assume that the user knows what he is doing and has supplied us with
		// a position that is right where value should be inserted (put in front of). 
		// We do a test to see if the position is correct. If so then we insert, 
		// if not then we ignore the input position. However, 

		if((position == end()) || !mValueCompare(*position, value))  // If value is <= the element at position...
		{
			if((position == begin()) || !mValueCompare(value, *(position - 1))) // If value is >= the element before position...
				return base_type::insert(position, value);
		}

		// In this case we have an incorrect position. We fall back to the regular insert function.
		return insert(value);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::iterator
	vector_multimap<K, T, C, A, RAC>::insert(const_iterator position, value_type&& value)
	{
		if((position == end()) || !mValueCompare(*position, value))  // If value is <= the element at position...
		{
			if((position == begin()) || !mValueCompare(value, *(position - 1))) // If value is >= the element before position...
				return base_type::insert(position, eastl::move(value));
		}

		// In this case we have an incorrect position. We fall back to the regular insert function.
		return insert(eastl::move(value));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline void vector_multimap<K, T, C, A, RAC>::insert(std::initializer_list<value_type> ilist)
	{
		insert(ilist.begin(), ilist.end());
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <typename InputIterator>
	inline void vector_multimap<K, T, C, A, RAC>::insert(InputIterator first, InputIterator last)
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
			base_type::insert(upper_bound((*first).first), *first);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::iterator
	vector_multimap<K, T, C, A, RAC>::erase(const_iterator position)           
	{
		// Note that we return iterator and not void. This allows for more efficient use of 
		// the container and is consistent with the C++ language defect report #130 (DR 130)
		return base_type::erase(position);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::iterator
	vector_multimap<K, T, C, A, RAC>::erase(const_iterator first, const_iterator last)
	{
		return base_type::erase(first, last);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::size_type
	vector_multimap<K, T, C, A, RAC>::erase(const key_type& k)
	{
		const eastl::pair<iterator, iterator> pairIts(equal_range(k));

		if(pairIts.first != pairIts.second)
			base_type::erase(pairIts.first, pairIts.second);

		return (size_type)eastl::distance(pairIts.first, pairIts.second); // This can result in any value >= 0.
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::reverse_iterator
	vector_multimap<K, T, C, A, RAC>::erase(const_reverse_iterator position)           
	{
		return reverse_iterator(base_type::erase((++position).base()));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::reverse_iterator
	vector_multimap<K, T, C, A, RAC>::erase(const_reverse_iterator first, const_reverse_iterator last)
	{
		return reverse_iterator(base_type::erase((++last).base(), (++first).base()));
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::iterator
	vector_multimap<K, T, C, A, RAC>::find(const key_type& k)
	{
		const eastl::pair<iterator, iterator> pairIts(equal_range(k));

		if(pairIts.first != pairIts.second)
			return pairIts.first;
		return end();
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::const_iterator
	vector_multimap<K, T, C, A, RAC>::find(const key_type& k) const
	{
		const eastl::pair<const_iterator, const_iterator> pairIts(equal_range(k));

		if(pairIts.first != pairIts.second)
			return pairIts.first;
		return end();
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <typename U, typename BinaryPredicate>
	inline typename vector_multimap<K, T, C, A, RAC>::const_iterator
	vector_multimap<K, T, C, A, RAC>::find_as(const U& u, BinaryPredicate predicate) const
	{
		multimap_value_compare<U, value_type, BinaryPredicate> predicate_cmp(predicate);
		const eastl::pair<const_iterator, const_iterator> pairIts(eastl::equal_range(begin(), end(), u, predicate_cmp));
		return (pairIts.first != pairIts.second) ? pairIts.first : end();
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	template <typename U, typename BinaryPredicate>
	inline typename vector_multimap<K, T, C, A, RAC>::iterator
	vector_multimap<K, T, C, A, RAC>::find_as(const U& u, BinaryPredicate predicate)
	{
		multimap_value_compare<U, value_type, BinaryPredicate> predicate_cmp(predicate);
		const eastl::pair<iterator, iterator> pairIts(eastl::equal_range(begin(), end(), u, predicate_cmp));
		return (pairIts.first != pairIts.second) ? pairIts.first : end();
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::size_type
	vector_multimap<K, T, C, A, RAC>::count(const key_type& k) const
	{
		const eastl::pair<const_iterator, const_iterator> pairIts(equal_range(k));
		return (size_type)eastl::distance(pairIts.first, pairIts.second); 
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::iterator
	vector_multimap<K, T, C, A, RAC>::lower_bound(const key_type& k)
	{
		return eastl::lower_bound(begin(), end(), k, mValueCompare);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::const_iterator
	vector_multimap<K, T, C, A, RAC>::lower_bound(const key_type& k) const
	{
		return eastl::lower_bound(begin(), end(), k, mValueCompare);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::iterator
	vector_multimap<K, T, C, A, RAC>::upper_bound(const key_type& k)
	{
		return eastl::upper_bound(begin(), end(), k, mValueCompare);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline typename vector_multimap<K, T, C, A, RAC>::const_iterator 
	vector_multimap<K, T, C, A, RAC>::upper_bound(const key_type& k) const
	{
		return eastl::upper_bound(begin(), end(), k, mValueCompare);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline eastl::pair<typename vector_multimap<K, T, C, A, RAC>::iterator, typename vector_multimap<K, T, C, A, RAC>::iterator>
	vector_multimap<K, T, C, A, RAC>::equal_range(const key_type& k)
	{
		return eastl::equal_range(begin(), end(), k, mValueCompare);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline eastl::pair<typename vector_multimap<K, T, C, A, RAC>::const_iterator, typename vector_multimap<K, T, C, A, RAC>::const_iterator>
	vector_multimap<K, T, C, A, RAC>::equal_range(const key_type& k) const
	{
		return eastl::equal_range(begin(), end(), k, mValueCompare);
	}


	/*
	// VC++ fails to compile this when defined here, saying the function isn't a member of vector_multimap.
	template <typename K, typename T, typename C, typename A, typename RAC>
	inline eastl::pair<typename vector_multimap<K, T, C, A, RAC>::iterator, typename vector_multimap<K, T, C, A, RAC>::iterator>
	vector_multimap<K, T, C, A, RAC>::equal_range_small(const key_type& k)
	{
		const iterator itLower(lower_bound(k));
		iterator       itUpper(itLower);

		while((itUpper != end()) && !mValueCompare(k, *itUpper))
			++itUpper;

		return eastl::pair<iterator, iterator>(itLower, itUpper);
	}
	*/


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline eastl::pair<typename vector_multimap<K, T, C, A, RAC>::const_iterator, typename vector_multimap<K, T, C, A, RAC>::const_iterator>
	vector_multimap<K, T, C, A, RAC>::equal_range_small(const key_type& k) const
	{
		const const_iterator itLower(lower_bound(k));
		const_iterator       itUpper(itLower);

		while((itUpper != end()) && !mValueCompare(k, *itUpper))
			++itUpper;

		return eastl::pair<const_iterator, const_iterator>(itLower, itUpper);
	}




	///////////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////////

	template <typename K, typename T, typename C, typename A, typename RAC>
	inline bool operator==(const vector_multimap<K, T, C, A, RAC>& a, 
						   const vector_multimap<K, T, C, A, RAC>& b) 
	{
		return (a.size() == b.size()) && eastl::equal(b.begin(), b.end(), a.begin());
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline bool operator<(const vector_multimap<K, T, C, A, RAC>& a,
						  const vector_multimap<K, T, C, A, RAC>& b)
	{
		return eastl::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), a.value_comp());
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline bool operator!=(const vector_multimap<K, T, C, A, RAC>& a,
						   const vector_multimap<K, T, C, A, RAC>& b)
	{
		return !(a == b);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline bool operator>(const vector_multimap<K, T, C, A, RAC>& a,
						  const vector_multimap<K, T, C, A, RAC>& b)
	{
		return b < a;
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline bool operator<=(const vector_multimap<K, T, C, A, RAC>& a,
						   const vector_multimap<K, T, C, A, RAC>& b)
	{
		return !(b < a);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline bool operator>=(const vector_multimap<K, T, C, A, RAC>& a,
						   const vector_multimap<K, T, C, A, RAC>& b)
	{
		return !(a < b);
	}


	template <typename K, typename T, typename C, typename A, typename RAC>
	inline void swap(vector_multimap<K, T, C, A, RAC>& a,
					 vector_multimap<K, T, C, A, RAC>& b)
	{
		a.swap(b);
	}


} // namespace eastl


#endif // Header include guard






















