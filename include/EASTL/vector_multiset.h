///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// This file implements vector_multiset. It acts much like std::multiset, except 
// its underlying representation is a random access container such as vector. 
// These containers are sometimes also known as "sorted vectors."  
// vector_sets have an advantage over conventional sets in that their memory
// is contiguous and node-less. The result is that lookups are faster, more 
// cache friendly (which potentially more so benefits speed), and the container
// uses less memory. The downside is that inserting new items into the container
// is slower if they are inserted in random order instead of in sorted order.
// This tradeoff is well-worth it for many cases. Note that vector_multiset allows
// you to use a deque or other random access container which may perform
// better for you than vector.
//
// Note that with vector_set, vector_multiset, vector_map, vector_multimap
// that the modification of the container potentially invalidates all 
// existing iterators into the container, unlike what happens with conventional
// sets and maps.
//////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_VECTOR_MULTISET_H
#define EASTL_VECTOR_MULTISET_H


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

	/// EASTL_VECTOR_MULTISET_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_VECTOR_MULTISET_DEFAULT_NAME
		#define EASTL_VECTOR_MULTISET_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " vector_multiset" // Unless the user overrides something, this is "EASTL vector_multiset".
	#endif


	/// EASTL_VECTOR_MULTISET_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_VECTOR_MULTISET_DEFAULT_ALLOCATOR
		#define EASTL_VECTOR_MULTISET_DEFAULT_ALLOCATOR allocator_type(EASTL_VECTOR_MULTISET_DEFAULT_NAME)
	#endif



	/// vector_multiset
	///
	/// Implements a multiset via a random access container such as a vector.
	/// This container is also known as a sorted_vector. We choose to call it 
	/// vector_multiset, as that is a more consistent universally applicable name
	/// for it in this library.
	///
	/// Note that with vector_set, vector_multiset, vector_map, vector_multimap
	/// that the modification of the container potentially invalidates all 
	/// existing iterators into the container, unlike what happens with conventional
	/// sets and maps.
	///
	/// To consider: std::multiset has the limitation that values in the set cannot
	/// be modified, with the idea that modifying them would change their sort
	/// order. We have the opportunity to make it so that values can be modified
	/// via changing iterators to be non-const, with the downside being that 
	/// the container can get screwed up if the user screws up. Alternatively,
	/// we can do what std STL does and require the user to make their stored
	/// classes use 'mutable' as needed. See the C++ standard defect report
	/// #103 (DR 103) for a discussion of this.
	///
	/// Note that the erase functions return iterator and not void. This allows for 
	/// more efficient use of the container and is consistent with the C++ language 
	/// defect report #130 (DR 130)
	///
	template <typename Key, typename Compare = eastl::less<Key>, typename Allocator = EASTLAllocatorType, 
			  typename RandomAccessContainer = eastl::vector<Key, Allocator> >
	class vector_multiset : public RandomAccessContainer
	{
	public:
		typedef RandomAccessContainer                                           base_type;
		typedef vector_multiset<Key, Compare, Allocator, RandomAccessContainer> this_type;
		typedef Allocator                                                       allocator_type;
		typedef Key                                                             key_type;
		typedef Key                                                             value_type;
		typedef Compare                                                         key_compare;
		typedef Compare                                                         value_compare;
		typedef value_type*                                                     pointer;
		typedef const value_type*                                               const_pointer;
		typedef value_type&                                                     reference;
		typedef const value_type&                                               const_reference;
		typedef typename base_type::size_type                                   size_type;
		typedef typename base_type::difference_type                             difference_type;
		typedef typename base_type::iterator                                    iterator;         // **Currently typedefing from iterator instead of const_iterator due to const issues **: Note that we typedef from const_iterator. This is by design, as sets are sorted and values cannot be modified. To consider: allow values to be modified and thus risk changing their sort values.
		typedef typename base_type::const_iterator                              const_iterator;
		typedef typename base_type::reverse_iterator                            reverse_iterator; // See notes directly above regarding const_iterator.
		typedef typename base_type::const_reverse_iterator                      const_reverse_iterator;

		using base_type::begin;
		using base_type::end;
		using base_type::get_allocator;

	protected:
		value_compare mCompare; // To consider: Declare this instead as: 'key_compare mKeyCompare'

	public:
		// We have an empty ctor and a ctor that takes an allocator instead of one for both
		// because this way our RandomAccessContainer wouldn't be required to have an constructor
		// that takes allocator_type. 
		vector_multiset();
		explicit vector_multiset(const allocator_type& allocator);
		explicit vector_multiset(const key_compare& comp, const allocator_type& allocator = EASTL_VECTOR_MULTISET_DEFAULT_ALLOCATOR);
		vector_multiset(const this_type& x);
		vector_multiset(this_type&& x);
		vector_multiset(this_type&& x, const allocator_type& allocator);
		vector_multiset(std::initializer_list<value_type> ilist, const key_compare& compare = key_compare(), const allocator_type& allocator = EASTL_VECTOR_MULTISET_DEFAULT_ALLOCATOR);

		template <typename InputIterator>
		vector_multiset(InputIterator first, InputIterator last); // allocator arg removed because VC7.1 fails on the default arg. To do: Make a second version of this function without a default arg.

		template <typename InputIterator>
		vector_multiset(InputIterator first, InputIterator last, const key_compare& compare); // allocator arg removed because VC7.1 fails on the default arg. To do: Make a second version of this function without a default arg.

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
		iterator insert(const_iterator position, const value_type& value);
		iterator insert(const_iterator position, value_type&& value);
		void     insert(std::initializer_list<value_type> ilist);

		template <typename P>
		iterator insert(P&& otherValue);

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

		/// equal_range_small
		/// This is a special version of equal_range which is optimized for the 
		/// case of there being few or no duplicated keys in the tree.
		eastl::pair<iterator, iterator>             equal_range_small(const key_type& k)
		{
			// Defined inline because VC7.1 is broken for when it's defined outside.
			const iterator itLower(lower_bound(k));
			iterator       itUpper(itLower);

			while((itUpper != end()) && !mCompare(k, *itUpper))
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

	}; // vector_multiset





	///////////////////////////////////////////////////////////////////////
	// vector_multiset
	///////////////////////////////////////////////////////////////////////

	template <typename K, typename C, typename A, typename RAC>
	inline vector_multiset<K, C, A, RAC>::vector_multiset()
		: base_type(), mCompare(C())
	{
		get_allocator().set_name(EASTL_VECTOR_MULTISET_DEFAULT_NAME);
	}


	template <typename K, typename C, typename A, typename RAC>
	inline vector_multiset<K, C, A, RAC>::vector_multiset(const allocator_type& allocator)
		: base_type(allocator), mCompare(C())
	{
		// Empty
	}


	template <typename K, typename C, typename A, typename RAC>
	inline vector_multiset<K, C, A, RAC>::vector_multiset(const key_compare& comp, const allocator_type& allocator)
		: base_type(allocator), mCompare(comp)
	{
		// Empty
	}


	template <typename K, typename C, typename A, typename RAC>
	template <typename InputIterator>
	inline vector_multiset<K, C, A, RAC>::vector_multiset(InputIterator first, InputIterator last)
		: base_type(EASTL_VECTOR_MULTISET_DEFAULT_ALLOCATOR), mCompare(key_compare())
	{
		insert(first, last);
	}


	template <typename K, typename C, typename A, typename RAC>
	template <typename InputIterator>
	inline vector_multiset<K, C, A, RAC>::vector_multiset(InputIterator first, InputIterator last, const key_compare& compare)
		: base_type(EASTL_VECTOR_MULTISET_DEFAULT_ALLOCATOR), mCompare(compare)
	{
		insert(first, last);
	}


	template <typename K, typename C, typename A, typename RAC>
	inline vector_multiset<K, C, A, RAC>::vector_multiset(const this_type& x)
		: base_type(x), mCompare(x.mCompare)
	{
		// Empty
	}


	template <typename K, typename C, typename A, typename RAC>
	inline vector_multiset<K, C, A, RAC>::vector_multiset(this_type&& x)
		: base_type(eastl::move(x)), mCompare(x.mCompare)
	{
		// Empty. Note: x is left with empty contents but its original mValueCompare instead of the default one. 
	}

	template <typename K, typename C, typename A, typename RAC>
	inline vector_multiset<K, C, A, RAC>::vector_multiset(this_type&& x, const allocator_type& allocator)
		: base_type(eastl::move(x), allocator), mCompare(x.mCompare)
	{
		// Empty. Note: x is left with empty contents but its original mValueCompare instead of the default one. 
	}


	template <typename K, typename C, typename A, typename RAC>
	inline vector_multiset<K, C, A, RAC>::vector_multiset(std::initializer_list<value_type> ilist, const key_compare& compare, const allocator_type& allocator)
		: base_type(allocator), mCompare(compare)
	{
		insert(ilist.begin(), ilist.end());
	}


	template <typename K, typename C, typename A, typename RAC>
	inline vector_multiset<K, C, A, RAC>&
	vector_multiset<K, C, A, RAC>::operator=(const this_type& x)
	{
		base_type::operator=(x);
		mCompare = value_compare(x.mCompare);
		return *this;
	}


	template <typename K, typename C, typename A, typename RAC>
	inline vector_multiset<K, C, A, RAC>&
	vector_multiset<K, C, A, RAC>::operator=(this_type&& x)
	{
		base_type::operator=(eastl::move(x));
		eastl::swap(mCompare, x.mCompare);
		return *this;
	}


	template <typename K, typename C, typename A, typename RAC>
	inline vector_multiset<K, C, A, RAC>&
	vector_multiset<K, C, A, RAC>::operator=(std::initializer_list<value_type> ilist)
	{
		base_type::clear();
		insert(ilist.begin(), ilist.end());
		return *this;
	}


	template <typename K, typename C, typename A, typename RAC>
	inline void vector_multiset<K, C, A, RAC>::swap(this_type& x)
	{
		base_type::swap(x);
		eastl::swap(mCompare, x.mCompare);
	}


	template <typename K, typename C, typename A, typename RAC>
	inline const typename vector_multiset<K, C, A, RAC>::key_compare&
	vector_multiset<K, C, A, RAC>::key_comp() const
	{
		return mCompare;
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::key_compare&
	vector_multiset<K, C, A, RAC>::key_comp()
	{
		return mCompare;
	}


	template <typename K, typename C, typename A, typename RAC>
	inline const typename vector_multiset<K, C, A, RAC>::value_compare&
	vector_multiset<K, C, A, RAC>::value_comp() const
	{
		return mCompare;
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::value_compare&
	vector_multiset<K, C, A, RAC>::value_comp()
	{
		return mCompare;
	}


	template <typename K, typename C, typename A, typename RAC>
	template <class... Args>
	typename vector_multiset<K, C, A, RAC>::iterator
	vector_multiset<K, C, A, RAC>::emplace(Args&&... args)
	{
		#if EASTL_USE_FORWARD_WORKAROUND
			auto value = value_type(eastl::forward<Args>(args)...);  // Workaround for compiler bug in VS2013 which results in a compiler internal crash while compiling this code.
		#else
			value_type  value(eastl::forward<Args>(args)...);
		#endif
		return insert(eastl::move(value));
	}

	template <typename K, typename C, typename A, typename RAC>
	template <class... Args> 
	typename vector_multiset<K, C, A, RAC>::iterator
	vector_multiset<K, C, A, RAC>::emplace_hint(const_iterator position, Args&&... args)
	{
		#if EASTL_USE_FORWARD_WORKAROUND
			auto value = value_type(eastl::forward<Args>(args)...);  // Workaround for compiler bug in VS2013 which results in a compiler internal crash while compiling this code.
		#else
			value_type  value(eastl::forward<Args>(args)...);
		#endif
		return insert(position, eastl::move(value));
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::iterator
	vector_multiset<K, C, A, RAC>::insert(const value_type& value)
	{
		const iterator itUB(upper_bound(value));
		return base_type::insert(itUB, value);
	}


	template <typename K, typename C, typename A, typename RAC>
	template <typename P>
	typename vector_multiset<K, C, A, RAC>::iterator
	vector_multiset<K, C, A, RAC>::insert(P&& otherValue)
	{
		value_type value(eastl::forward<P>(otherValue));
		const iterator itUB(upper_bound(value));
		return base_type::insert(itUB, eastl::move(value));
	}


	template <typename K, typename C, typename A, typename RAC>
	inline void vector_multiset<K, C, A, RAC>::insert(std::initializer_list<value_type> ilist)
	{
		insert(ilist.begin(), ilist.end());
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::iterator
	vector_multiset<K, C, A, RAC>::insert(const_iterator position, const value_type& value)
	{
		// We assume that the user knows what he is doing and has supplied us with
		// a position that is right where value should be inserted (put in front of). 
		// We do a test to see if the position is correct. If so then we insert, 
		// if not then we ignore the input position. However, 

		if((position == end()) || !mCompare(*position, value))  // If value is <= the element at position...
		{
			if((position == begin()) || !mCompare(value, *(position - 1))) // If value is >= the element before position...
				return base_type::insert(position, value);
		}

		// In this case we have an incorrect position. We fall back to the regular insert function.
		return insert(value);
	}


	template <typename K, typename C, typename A, typename RAC>
	typename vector_multiset<K, C, A, RAC>::iterator
	vector_multiset<K, C, A, RAC>::insert(const_iterator position, value_type&& value)
	{
		if((position == end()) || !mCompare(*position, value))  // If value is <= the element at position...
		{
			if((position == begin()) || !mCompare(value, *(position - 1))) // If value is >= the element before position...
				return base_type::insert(position, eastl::move(value));
		}

		// In this case we have an incorrect position. We fall back to the regular insert function.
		return insert(eastl::move(value));
	}


	template <typename K, typename C, typename A, typename RAC>
	template <typename InputIterator>
	inline void vector_multiset<K, C, A, RAC>::insert(InputIterator first, InputIterator last)
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
			base_type::insert(upper_bound(*first), *first);
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::iterator 
	vector_multiset<K, C, A, RAC>::erase(const_iterator position)
	{
		// Note that we return iterator and not void. This allows for more efficient use of 
		// the container and is consistent with the C++ language defect report #130 (DR 130)
		return base_type::erase(position);
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::iterator
	vector_multiset<K, C, A, RAC>::erase(const_iterator first, const_iterator last)
	{
		return base_type::erase(first, last);
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::reverse_iterator 
	vector_multiset<K, C, A, RAC>::erase(const_reverse_iterator position)
	{
		return reverse_iterator(base_type::erase((++position).base()));
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::reverse_iterator
	vector_multiset<K, C, A, RAC>::erase(const_reverse_iterator first, const_reverse_iterator last)
	{
		return reverse_iterator(base_type::erase((++last).base(), (++first).base()));
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::size_type
	vector_multiset<K, C, A, RAC>::erase(const key_type& k)
	{
		const eastl::pair<iterator, iterator> pairIts(equal_range(k));

		if(pairIts.first != pairIts.second)
			base_type::erase(pairIts.first, pairIts.second);

		return (size_type)eastl::distance(pairIts.first, pairIts.second); // This can result in any value >= 0.
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::iterator
	vector_multiset<K, C, A, RAC>::find(const key_type& k)
	{
		const eastl::pair<iterator, iterator> pairIts(equal_range(k));
		return (pairIts.first != pairIts.second) ? pairIts.first : end();
	}


	template <typename K, typename C, typename A, typename RAC>
	template <typename U, typename BinaryPredicate>
	inline typename vector_multiset<K, C, A, RAC>::iterator
	vector_multiset<K, C, A, RAC>::find_as(const U& u, BinaryPredicate predicate)
	{
		const eastl::pair<iterator, iterator> pairIts(eastl::equal_range(begin(), end(), u, predicate));
		return (pairIts.first != pairIts.second) ? pairIts.first : end();
	}


	template <typename K, typename C, typename A, typename RAC>
	template <typename U, typename BinaryPredicate>
	inline typename vector_multiset<K, C, A, RAC>::const_iterator
	vector_multiset<K, C, A, RAC>::find_as(const U& u, BinaryPredicate predicate) const
	{
		const eastl::pair<const_iterator, const_iterator> pairIts(eastl::equal_range(begin(), end(), u, predicate));
		return (pairIts.first != pairIts.second) ? pairIts.first : end();
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::const_iterator
	vector_multiset<K, C, A, RAC>::find(const key_type& k) const
	{
		const eastl::pair<const_iterator, const_iterator> pairIts(equal_range(k));
		return (pairIts.first != pairIts.second) ? pairIts.first : end();
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::size_type
	vector_multiset<K, C, A, RAC>::count(const key_type& k) const
	{
		const eastl::pair<const_iterator, const_iterator> pairIts(equal_range(k));
		return (size_type)eastl::distance(pairIts.first, pairIts.second);
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::iterator
	vector_multiset<K, C, A, RAC>::lower_bound(const key_type& k)
	{
		return eastl::lower_bound(begin(), end(), k, mCompare);
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::const_iterator
	vector_multiset<K, C, A, RAC>::lower_bound(const key_type& k) const
	{
		return eastl::lower_bound(begin(), end(), k, mCompare);
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::iterator
	vector_multiset<K, C, A, RAC>::upper_bound(const key_type& k)
	{
		return eastl::upper_bound(begin(), end(), k, mCompare);
	}


	template <typename K, typename C, typename A, typename RAC>
	inline typename vector_multiset<K, C, A, RAC>::const_iterator
	vector_multiset<K, C, A, RAC>::upper_bound(const key_type& k) const
	{
		return eastl::upper_bound(begin(), end(), k, mCompare);
	}


	template <typename K, typename C, typename A, typename RAC>
	inline eastl::pair<typename vector_multiset<K, C, A, RAC>::iterator, typename vector_multiset<K, C, A, RAC>::iterator>
	vector_multiset<K, C, A, RAC>::equal_range(const key_type& k)
	{
		return eastl::equal_range(begin(), end(), k, mCompare);
	}


	template <typename K, typename C, typename A, typename RAC>
	inline eastl::pair<typename vector_multiset<K, C, A, RAC>::const_iterator, typename vector_multiset<K, C, A, RAC>::const_iterator>
	vector_multiset<K, C, A, RAC>::equal_range(const key_type& k) const
	{
		return eastl::equal_range(begin(), end(), k, mCompare);
	}


	/*
	// VC++ fails to compile this when defined here, saying the function isn't a memgber of vector_multimap.
	template <typename K, typename C, typename A, typename RAC>
	inline eastl::pair<typename vector_multiset<K, C, A, RAC>::iterator, typename vector_multiset<K, C, A, RAC>::iterator>
	vector_multiset<K, C, A, RAC>::equal_range_small(const key_type& k)
	{
		const iterator itLower(lower_bound(k));
		iterator       itUpper(itLower);

		while((itUpper != end()) && !mCompare(k, *itUpper))
			++itUpper;

		return eastl::pair<iterator, iterator>(itLower, itUpper);
	}
	*/


	template <typename K, typename C, typename A, typename RAC>
	inline eastl::pair<typename vector_multiset<K, C, A, RAC>::const_iterator, typename vector_multiset<K, C, A, RAC>::const_iterator>
	vector_multiset<K, C, A, RAC>::equal_range_small(const key_type& k) const
	{
		const const_iterator itLower(lower_bound(k));
		const_iterator       itUpper(itLower);

		while((itUpper != end()) && !mCompare(k, *itUpper))
			++itUpper;

		return eastl::pair<const_iterator, const_iterator>(itLower, itUpper);
	}



	///////////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////////

	template <typename Key, typename Compare, typename Allocator, typename RandomAccessContainer>
	inline bool operator==(const vector_multiset<Key, Compare, Allocator, RandomAccessContainer>& a, 
						   const vector_multiset<Key, Compare, Allocator, RandomAccessContainer>& b) 
	{
		return (a.size() == b.size()) && eastl::equal(b.begin(), b.end(), a.begin());
	}


	template <typename Key, typename Compare, typename Allocator, typename RandomAccessContainer>
	inline bool operator<(const vector_multiset<Key, Compare, Allocator, RandomAccessContainer>& a,
						  const vector_multiset<Key, Compare, Allocator, RandomAccessContainer>& b)
	{
		return eastl::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), a.value_comp());
	}


	template <typename Key, typename Compare, typename Allocator, typename RandomAccessContainer>
	inline bool operator!=(const vector_multiset<Key, Compare, Allocator, RandomAccessContainer>& a,
						   const vector_multiset<Key, Compare, Allocator, RandomAccessContainer>& b)
	{
		return !(a == b);
	}


	template <typename Key, typename Compare, typename Allocator, typename RandomAccessContainer>
	inline bool operator>(const vector_multiset<Key, Compare, Allocator, RandomAccessContainer>& a,
						  const vector_multiset<Key, Compare, Allocator, RandomAccessContainer>& b)
	{
		return b < a;
	}


	template <typename Key, typename Compare, typename Allocator, typename RandomAccessContainer>
	inline bool operator<=(const vector_multiset<Key, Compare, Allocator, RandomAccessContainer>& a,
						   const vector_multiset<Key, Compare, Allocator, RandomAccessContainer>& b)
	{
		return !(b < a);
	}


	template <typename Key, typename Compare, typename Allocator, typename RandomAccessContainer>
	inline bool operator>=(const vector_multiset<Key, Compare, Allocator, RandomAccessContainer>& a,
						   const vector_multiset<Key, Compare, Allocator, RandomAccessContainer>& b)
	{
		return !(a < b);
	}


	template <typename Key, typename Compare, typename Allocator, typename RandomAccessContainer>
	inline void swap(vector_multiset<Key, Compare, Allocator, RandomAccessContainer>& a,
					 vector_multiset<Key, Compare, Allocator, RandomAccessContainer>& b)
	{
		a.swap(b);
	}


} // namespace eastl


#endif // Header include guard













