///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
//////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_SET_H
#define EASTL_SET_H


#include <EASTL/internal/config.h>
#include <EASTL/internal/red_black_tree.h>
#include <EASTL/functional.h>
#include <EASTL/utility.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// EASTL_SET_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_SET_DEFAULT_NAME
		#define EASTL_SET_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " set" // Unless the user overrides something, this is "EASTL set".
	#endif


	/// EASTL_MULTISET_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_MULTISET_DEFAULT_NAME
		#define EASTL_MULTISET_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " multiset" // Unless the user overrides something, this is "EASTL multiset".
	#endif


	/// EASTL_SET_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_SET_DEFAULT_ALLOCATOR
		#define EASTL_SET_DEFAULT_ALLOCATOR allocator_type(EASTL_SET_DEFAULT_NAME)
	#endif

	/// EASTL_MULTISET_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_MULTISET_DEFAULT_ALLOCATOR
		#define EASTL_MULTISET_DEFAULT_ALLOCATOR allocator_type(EASTL_MULTISET_DEFAULT_NAME)
	#endif



	/// set
	///
	/// Implements a canonical set. 
	///
	/// The large majority of the implementation of this class is found in the rbtree
	/// base class. We control the behaviour of rbtree via template parameters.
	///
	/// Note that the 'bMutableIterators' template parameter to rbtree is set to false.
	/// This means that set::iterator is const and the same as set::const_iterator.
	/// This is by design and it follows the C++ standard defect report recommendation.
	/// If the user wants to modify a container element, the user needs to either use
	/// mutable data members or use const_cast on the iterator's data member. Both of 
	/// these solutions are recommended by the C++ standard defect report.
	/// To consider: Expose the bMutableIterators template policy here at the set level
	/// so the user can have non-const set iterators via a template parameter.
	///
	/// Pool allocation
	/// If you want to make a custom memory pool for a set container, your pool 
	/// needs to contain items of type set::node_type. So if you have a memory
	/// pool that has a constructor that takes the size of pool items and the
	/// count of pool items, you would do this (assuming that MemoryPool implements
	/// the Allocator interface):
	///     typedef set<Widget, less<Widget>, MemoryPool> WidgetSet;    // Delare your WidgetSet type.
	///     MemoryPool myPool(sizeof(WidgetSet::node_type), 100);       // Make a pool of 100 Widget nodes.
	///     WidgetSet mySet(&myPool);                                   // Create a map that uses the pool.
	///
	template <typename Key, typename Compare = eastl::less<Key>, typename Allocator = EASTLAllocatorType>
	class set
		: public rbtree<Key, Key, Compare, Allocator, eastl::use_self<Key>, false, true>
	{
	public:
		typedef rbtree<Key, Key, Compare, Allocator, eastl::use_self<Key>, false, true> base_type;
		typedef set<Key, Compare, Allocator>                                            this_type;
		typedef typename base_type::size_type                                           size_type;
		typedef typename base_type::value_type                                          value_type;
		typedef typename base_type::iterator                                            iterator;
		typedef typename base_type::const_iterator                                      const_iterator;
		typedef typename base_type::reverse_iterator                                    reverse_iterator;
		typedef typename base_type::const_reverse_iterator                              const_reverse_iterator;
		typedef typename base_type::allocator_type                                      allocator_type;
		typedef Compare                                                                 value_compare;
		// Other types are inherited from the base class.

		using base_type::begin;
		using base_type::end;
		using base_type::find;
		using base_type::lower_bound;
		using base_type::upper_bound;
		using base_type::mCompare;

	public:
		set(const allocator_type& allocator = EASTL_SET_DEFAULT_ALLOCATOR);
		set(const Compare& compare, const allocator_type& allocator = EASTL_SET_DEFAULT_ALLOCATOR);
		set(const this_type& x);
		set(this_type&& x);
		set(this_type&& x, const allocator_type& allocator);
		set(std::initializer_list<value_type> ilist, const Compare& compare = Compare(), const allocator_type& allocator = EASTL_SET_DEFAULT_ALLOCATOR);

		template <typename Iterator>
		set(Iterator itBegin, Iterator itEnd); // allocator arg removed because VC7.1 fails on the default arg. To do: Make a second version of this function without a default arg.

		// The (this_type&& x) ctor above has the side effect of forcing us to make operator= visible in this subclass.
		this_type& operator=(const this_type& x) { return (this_type&)base_type::operator=(x); }
		this_type& operator=(std::initializer_list<value_type> ilist) { return (this_type&)base_type::operator=(ilist); }
		this_type& operator=(this_type&& x) { return (this_type&)base_type::operator=(eastl::move(x)); }

	public:
		value_compare value_comp() const;

		size_type erase(const Key& k);
		iterator  erase(const_iterator position);
		iterator  erase(const_iterator first, const_iterator last);

		reverse_iterator erase(const_reverse_iterator position);
		reverse_iterator erase(const_reverse_iterator first, const_reverse_iterator last);

		size_type count(const Key& k) const;

		eastl::pair<iterator, iterator>             equal_range(const Key& k);
		eastl::pair<const_iterator, const_iterator> equal_range(const Key& k) const;

	}; // set





	/// multiset
	///
	/// Implements a canonical multiset.
	///
	/// The large majority of the implementation of this class is found in the rbtree
	/// base class. We control the behaviour of rbtree via template parameters.
	///
	/// See notes above in 'set' regarding multable iterators.
	///
	/// Pool allocation
	/// If you want to make a custom memory pool for a multiset container, your pool 
	/// needs to contain items of type multiset::node_type. So if you have a memory
	/// pool that has a constructor that takes the size of pool items and the
	/// count of pool items, you would do this (assuming that MemoryPool implements
	/// the Allocator interface):
	///     typedef multiset<Widget, less<Widget>, MemoryPool> WidgetSet;   // Delare your WidgetSet type.
	///     MemoryPool myPool(sizeof(WidgetSet::node_type), 100);           // Make a pool of 100 Widget nodes.
	///     WidgetSet mySet(&myPool);                                       // Create a map that uses the pool.
	///
	template <typename Key, typename Compare = eastl::less<Key>, typename Allocator = EASTLAllocatorType>
	class multiset
		: public rbtree<Key, Key, Compare, Allocator, eastl::use_self<Key>, false, false>
	{
	public:
		typedef rbtree<Key, Key, Compare, Allocator, eastl::use_self<Key>, false, false>    base_type;
		typedef multiset<Key, Compare, Allocator>                                           this_type;
		typedef typename base_type::size_type                                               size_type;
		typedef typename base_type::value_type                                              value_type;
		typedef typename base_type::iterator                                                iterator;
		typedef typename base_type::const_iterator                                          const_iterator;
		typedef typename base_type::reverse_iterator                                        reverse_iterator;
		typedef typename base_type::const_reverse_iterator                                  const_reverse_iterator;
		typedef typename base_type::allocator_type                                          allocator_type;
		typedef Compare                                                                     value_compare;
		// Other types are inherited from the base class.

		using base_type::begin;
		using base_type::end;
		using base_type::find;
		using base_type::lower_bound;
		using base_type::upper_bound;
		using base_type::mCompare;

	public:
		multiset(const allocator_type& allocator = EASTL_MULTISET_DEFAULT_ALLOCATOR);
		multiset(const Compare& compare, const allocator_type& allocator = EASTL_MULTISET_DEFAULT_ALLOCATOR);
		multiset(const this_type& x);
		multiset(this_type&& x);
		multiset(this_type&& x, const allocator_type& allocator);
		multiset(std::initializer_list<value_type> ilist, const Compare& compare = Compare(), const allocator_type& allocator = EASTL_MULTISET_DEFAULT_ALLOCATOR);

		template <typename Iterator>
		multiset(Iterator itBegin, Iterator itEnd); // allocator arg removed because VC7.1 fails on the default arg. To do: Make a second version of this function without a default arg.

		// The (this_type&& x) ctor above has the side effect of forcing us to make operator= visible in this subclass.
		this_type& operator=(const this_type& x) { return (this_type&)base_type::operator=(x); }
		this_type& operator=(std::initializer_list<value_type> ilist) { return (this_type&)base_type::operator=(ilist); }
		this_type& operator=(this_type&& x) { return (this_type&)base_type::operator=(eastl::move(x)); }

	public:
		value_compare value_comp() const;

		size_type erase(const Key& k);
		iterator  erase(const_iterator position);
		iterator  erase(const_iterator first, const_iterator last);

		reverse_iterator erase(const_reverse_iterator position);
		reverse_iterator erase(const_reverse_iterator first, const_reverse_iterator last);

		size_type count(const Key& k) const;

		eastl::pair<iterator, iterator>             equal_range(const Key& k);
		eastl::pair<const_iterator, const_iterator> equal_range(const Key& k) const;

		/// equal_range_small
		/// This is a special version of equal_range which is optimized for the 
		/// case of there being few or no duplicated keys in the tree.
		eastl::pair<iterator, iterator>             equal_range_small(const Key& k);
		eastl::pair<const_iterator, const_iterator> equal_range_small(const Key& k) const;

	}; // multiset





	///////////////////////////////////////////////////////////////////////
	// set
	///////////////////////////////////////////////////////////////////////

	template <typename Key, typename Compare, typename Allocator>
	inline set<Key, Compare, Allocator>::set(const allocator_type& allocator)
		: base_type(allocator)
	{
	}


	template <typename Key, typename Compare, typename Allocator>
	inline set<Key, Compare, Allocator>::set(const Compare& compare, const allocator_type& allocator)
		: base_type(compare, allocator)
	{
	}


	template <typename Key, typename Compare, typename Allocator>
	inline set<Key, Compare, Allocator>::set(const this_type& x)
		: base_type(x)
	{
	}


	template <typename Key, typename Compare, typename Allocator>
	inline set<Key, Compare, Allocator>::set(this_type&& x)
		: base_type(eastl::move(x))
	{
	}

	template <typename Key, typename Compare, typename Allocator>
	inline set<Key, Compare, Allocator>::set(this_type&& x, const allocator_type& allocator)
		: base_type(eastl::move(x), allocator)
	{
	}


	template <typename Key, typename Compare, typename Allocator>
	inline set<Key, Compare, Allocator>::set(std::initializer_list<value_type> ilist, const Compare& compare, const allocator_type& allocator)
		: base_type(ilist.begin(), ilist.end(), compare, allocator)
	{
	}


	template <typename Key, typename Compare, typename Allocator>
	template <typename Iterator>
	inline set<Key, Compare, Allocator>::set(Iterator itBegin, Iterator itEnd)
		: base_type(itBegin, itEnd, Compare(), EASTL_SET_DEFAULT_ALLOCATOR)
	{
	}


	template <typename Key, typename Compare, typename Allocator>
	inline typename set<Key, Compare, Allocator>::value_compare
	set<Key, Compare, Allocator>::value_comp() const
	{
		return mCompare;
	}


	template <typename Key, typename Compare, typename Allocator>
	inline typename set<Key, Compare, Allocator>::size_type
	set<Key, Compare, Allocator>::erase(const Key& k)
	{
		const iterator it(find(k));

		if(it != end()) // If it exists...
		{
			base_type::erase(it);
			return 1;
		}
		return 0;
	}


	template <typename Key, typename Compare, typename Allocator>
	inline typename set<Key, Compare, Allocator>::iterator
	set<Key, Compare, Allocator>::erase(const_iterator position)
	{
		// We need to provide this version because we override another version 
		// and C++ hiding rules would make the base version of this hidden.
		return base_type::erase(position);
	}


	template <typename Key, typename Compare, typename Allocator>
	inline typename set<Key, Compare, Allocator>::iterator
	set<Key, Compare, Allocator>::erase(const_iterator first, const_iterator last)
	{
		// We need to provide this version because we override another version 
		// and C++ hiding rules would make the base version of this hidden.
		return base_type::erase(first, last);
	}


	template <typename Key, typename Compare, typename Allocator>
	inline typename set<Key, Compare, Allocator>::size_type
	set<Key, Compare, Allocator>::count(const Key& k) const
	{
		const const_iterator it(find(k));
		return (it != end()) ? (size_type)1 : (size_type)0;
	}


	template <typename Key, typename Compare, typename Allocator>
	inline typename set<Key, Compare, Allocator>::reverse_iterator
	set<Key, Compare, Allocator>::erase(const_reverse_iterator position)
	{
		return reverse_iterator(erase((++position).base()));
	}


	template <typename Key, typename Compare, typename Allocator>
	inline typename set<Key, Compare, Allocator>::reverse_iterator
	set<Key, Compare, Allocator>::erase(const_reverse_iterator first, const_reverse_iterator last)
	{
		// Version which erases in order from first to last.
		// difference_type i(first.base() - last.base());
		// while(i--)
		//     first = erase(first);
		// return first;

		// Version which erases in order from last to first, but is slightly more efficient:
		return reverse_iterator(erase((++last).base(), (++first).base()));
	}


	template <typename Key, typename Compare, typename Allocator>
	inline eastl::pair<typename set<Key, Compare, Allocator>::iterator,
					   typename set<Key, Compare, Allocator>::iterator>
	set<Key, Compare, Allocator>::equal_range(const Key& k)
	{
		// The resulting range will either be empty or have one element,
		// so instead of doing two tree searches (one for lower_bound and 
		// one for upper_bound), we do just lower_bound and see if the 
		// result is a range of size zero or one.
		const iterator itLower(lower_bound(k));

		if((itLower == end()) || mCompare(k, *itLower)) // If at the end or if (k is < itLower)...
			return eastl::pair<iterator, iterator>(itLower, itLower);

		iterator itUpper(itLower);
		return eastl::pair<iterator, iterator>(itLower, ++itUpper);
	}
	

	template <typename Key, typename Compare, typename Allocator>
	inline eastl::pair<typename set<Key, Compare, Allocator>::const_iterator, 
					   typename set<Key, Compare, Allocator>::const_iterator>
	set<Key, Compare, Allocator>::equal_range(const Key& k) const
	{
		// See equal_range above for comments.
		const const_iterator itLower(lower_bound(k));

		if((itLower == end()) || mCompare(k, *itLower)) // If at the end or if (k is < itLower)...
			return eastl::pair<const_iterator, const_iterator>(itLower, itLower);

		const_iterator itUpper(itLower);
		return eastl::pair<const_iterator, const_iterator>(itLower, ++itUpper);
	}





	///////////////////////////////////////////////////////////////////////
	// multiset
	///////////////////////////////////////////////////////////////////////

	template <typename Key, typename Compare, typename Allocator>
	inline multiset<Key, Compare, Allocator>::multiset(const allocator_type& allocator)
		: base_type(allocator)
	{
	}


	template <typename Key, typename Compare, typename Allocator>
	inline multiset<Key, Compare, Allocator>::multiset(const Compare& compare, const allocator_type& allocator)
		: base_type(compare, allocator)
	{
	}


	template <typename Key, typename Compare, typename Allocator>
	inline multiset<Key, Compare, Allocator>::multiset(const this_type& x)
		: base_type(x)
	{
	}


	template <typename Key, typename Compare, typename Allocator>
	inline multiset<Key, Compare, Allocator>::multiset(this_type&& x)
		: base_type(eastl::move(x))
	{
	}

	template <typename Key, typename Compare, typename Allocator>
	inline multiset<Key, Compare, Allocator>::multiset(this_type&& x, const allocator_type& allocator)
		: base_type(eastl::move(x), allocator)
	{
	}


	template <typename Key, typename Compare, typename Allocator>
	inline multiset<Key, Compare, Allocator>::multiset(std::initializer_list<value_type> ilist, const Compare& compare, const allocator_type& allocator)
		: base_type(ilist.begin(), ilist.end(), compare, allocator)
	{
	}


	template <typename Key, typename Compare, typename Allocator>
	template <typename Iterator>
	inline multiset<Key, Compare, Allocator>::multiset(Iterator itBegin, Iterator itEnd)
		: base_type(itBegin, itEnd, Compare(), EASTL_MULTISET_DEFAULT_ALLOCATOR)
	{
	}


	template <typename Key, typename Compare, typename Allocator>
	inline typename multiset<Key, Compare, Allocator>::value_compare
	multiset<Key, Compare, Allocator>::value_comp() const
	{
		return mCompare;
	}


	template <typename Key, typename Compare, typename Allocator>
	inline typename multiset<Key, Compare, Allocator>::size_type
	multiset<Key, Compare, Allocator>::erase(const Key& k)
	{
		const eastl::pair<iterator, iterator> range(equal_range(k));
		const size_type n = (size_type)eastl::distance(range.first, range.second);
		base_type::erase(range.first, range.second);
		return n;
	}


	template <typename Key, typename Compare, typename Allocator>
	inline typename multiset<Key, Compare, Allocator>::iterator
	multiset<Key, Compare, Allocator>::erase(const_iterator position)
	{
		// We need to provide this version because we override another version 
		// and C++ hiding rules would make the base version of this hidden.
		return base_type::erase(position);
	}


	template <typename Key, typename Compare, typename Allocator>
	inline typename multiset<Key, Compare, Allocator>::iterator
	multiset<Key, Compare, Allocator>::erase(const_iterator first, const_iterator last)
	{
		// We need to provide this version because we override another version 
		// and C++ hiding rules would make the base version of this hidden.
		return base_type::erase(first, last);
	}


	template <typename Key, typename Compare, typename Allocator>
	inline typename multiset<Key, Compare, Allocator>::size_type
	multiset<Key, Compare, Allocator>::count(const Key& k) const
	{
		const eastl::pair<const_iterator, const_iterator> range(equal_range(k));
		return (size_type)eastl::distance(range.first, range.second);
	}


	template <typename Key, typename Compare, typename Allocator>
	inline typename multiset<Key, Compare, Allocator>::reverse_iterator
	multiset<Key, Compare, Allocator>::erase(const_reverse_iterator position)
	{
		return reverse_iterator(erase((++position).base()));
	}


	template <typename Key, typename Compare, typename Allocator>
	inline typename multiset<Key, Compare, Allocator>::reverse_iterator
	multiset<Key, Compare, Allocator>::erase(const_reverse_iterator first, const_reverse_iterator last)
	{
		// Version which erases in order from first to last.
		// difference_type i(first.base() - last.base());
		// while(i--)
		//     first = erase(first);
		// return first;

		// Version which erases in order from last to first, but is slightly more efficient:
		return reverse_iterator(erase((++last).base(), (++first).base()));
	}


	template <typename Key, typename Compare, typename Allocator>
	inline eastl::pair<typename multiset<Key, Compare, Allocator>::iterator,
					   typename multiset<Key, Compare, Allocator>::iterator>
	multiset<Key, Compare, Allocator>::equal_range(const Key& k)
	{
		// There are multiple ways to implement equal_range. The implementation mentioned
		// in the C++ standard and which is used by most (all?) commercial STL implementations
		// is this:
		//    return eastl::pair<iterator, iterator>(lower_bound(k), upper_bound(k));
		//
		// This does two tree searches -- one for the lower bound and one for the 
		// upper bound. This works well for the case whereby you have a large container
		// and there are lots of duplicated values. We provide an alternative version
		// of equal_range called equal_range_small for cases where the user is confident
		// that the number of duplicated items is only a few.

		return eastl::pair<iterator, iterator>(lower_bound(k), upper_bound(k));
	}


	template <typename Key, typename Compare, typename Allocator>
	inline eastl::pair<typename multiset<Key, Compare, Allocator>::const_iterator, 
					   typename multiset<Key, Compare, Allocator>::const_iterator>
	multiset<Key, Compare, Allocator>::equal_range(const Key& k) const
	{
		// See comments above in the non-const version of equal_range.
		return eastl::pair<iterator, iterator>(lower_bound(k), upper_bound(k));
	}


	template <typename Key, typename Compare, typename Allocator>
	inline eastl::pair<typename multiset<Key, Compare, Allocator>::iterator,
					   typename multiset<Key, Compare, Allocator>::iterator>
	multiset<Key, Compare, Allocator>::equal_range_small(const Key& k)
	{
		// We provide alternative version of equal_range here which works faster
		// for the case where there are at most small number of potential duplicated keys.
		const iterator itLower(lower_bound(k));
		iterator       itUpper(itLower);

		while((itUpper != end()) && !mCompare(k, itUpper.mpNode->mValue))
			++itUpper;

		return eastl::pair<iterator, iterator>(itLower, itUpper);
	}


	template <typename Key, typename Compare, typename Allocator>
	inline eastl::pair<typename multiset<Key, Compare, Allocator>::const_iterator, 
					   typename multiset<Key, Compare, Allocator>::const_iterator>
	multiset<Key, Compare, Allocator>::equal_range_small(const Key& k) const
	{
		// We provide alternative version of equal_range here which works faster
		// for the case where there are at most small number of potential duplicated keys.
		const const_iterator itLower(lower_bound(k));
		const_iterator       itUpper(itLower);

		while((itUpper != end()) && !mCompare(k, *itUpper))
			++itUpper;

		return eastl::pair<const_iterator, const_iterator>(itLower, itUpper);
	}



} // namespace eastl


#endif // Header include guard





















