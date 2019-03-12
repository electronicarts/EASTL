/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// *** Note ***
// This implementation is incomplete.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTRUSIVE_SLIST_H
#define EASTL_INTRUSIVE_SLIST_H


#include <EASTL/internal/config.h>
#include <EASTL/iterator.h>
#include <EASTL/algorithm.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// intrusive_slist_node
	///
	struct intrusive_slist_node
	{
		intrusive_slist_node* mpNext;
	};


	/// IntrusiveSListIterator
	///
	template <typename T, typename Pointer, typename Reference>
	struct IntrusiveSListIterator
	{
		typedef IntrusiveSListIterator<T, Pointer, Reference>   this_type;
		typedef IntrusiveSListIterator<T, T*, T&>               iterator;
		typedef IntrusiveSListIterator<T, const T*, const T&>   const_iterator;
		typedef eastl_size_t                                    size_type;     // See config.h for the definition of eastl_size_t, which defaults to size_t.
		typedef ptrdiff_t                                       difference_type;
		typedef T                                               value_type;
		typedef T                                               node_type;
		typedef Pointer                                         pointer;
		typedef Reference                                       reference;
		typedef EASTL_ITC_NS::forward_iterator_tag              iterator_category;

	public:
		node_type* mpNode;

	public:
		IntrusiveSListIterator();
		explicit IntrusiveSListIterator(pointer pNode); // Note that you can also construct an iterator from T via this, since value_type == node_type.
		IntrusiveSListIterator(const iterator& x);

		reference operator*() const;
		pointer   operator->() const;

		this_type& operator++();
		this_type  operator++(int);

	}; // struct IntrusiveSListIterator



	/// intrusive_slist_base
	///
	/// Provides a template-less base class for intrusive_slist.
	///
	class intrusive_slist_base
	{
	public:
		typedef eastl_size_t size_type;     // See config.h for the definition of eastl_size_t, which defaults to size_t.
		typedef ptrdiff_t    difference_type;

	protected:
		intrusive_slist_node* mpNext;

	public:
		intrusive_slist_base();

		bool      empty() const;            ///< Returns true if the container is empty.
		size_type size() const;             ///< Returns the number of elements in the list; O(n).

		void      clear();                  ///< Clears the list; O(1). No deallocation occurs.
		void      pop_front();              ///< Removes an element from the front of the list; O(1). The element must be present, but is not deallocated.
		void      reverse();                ///< Reverses a list so that front and back are swapped; O(n).

		//bool    validate() const;         ///< Scans a list for linkage inconsistencies; O(n) time, O(1) space. Returns false if errors are detected, such as loops or branching.

	}; // class intrusive_slist_base



	/// intrusive_slist
	///
	template <typename T = intrusive_slist_node>
	class intrusive_slist : public intrusive_slist_base
	{
	public:
		typedef intrusive_slist<T>                              this_type;
		typedef intrusive_slist_base                            base_type;
		typedef T                                               node_type;
		typedef T                                               value_type;
		typedef typename base_type::size_type                   size_type;
		typedef typename base_type::difference_type             difference_type;
		typedef T&                                              reference;
		typedef const T&                                        const_reference;
		typedef T*                                              pointer;
		typedef const T*                                        const_pointer;
		typedef IntrusiveSListIterator<T, T*, T&>               iterator;
		typedef IntrusiveSListIterator<T, const T*, const T&>   const_iterator;

	public:
		intrusive_slist();                        ///< Creates an empty list.
	  //intrusive_slist(const this_type& x);      ///< Creates an empty list; ignores the argument. To consider: Is this a useful function?
	  //this_type& operator=(const this_type& x); ///< Clears the list; ignores the argument.       To consider: Is this a useful function?

		iterator         begin();                 ///< Returns an iterator pointing to the first element in the list. O(1).
		const_iterator   begin() const;           ///< Returns a const_iterator pointing to the first element in the list. O(1).
		const_iterator   cbegin() const;          ///< Returns a const_iterator pointing to the first element in the list. O(1).
		iterator         end();                   ///< Returns an iterator pointing one-after the last element in the list. O(1).
		const_iterator   end() const;             ///< Returns a const_iterator pointing one-after the last element in the list. O(1).
		const_iterator   cend() const;            ///< Returns a const_iterator pointing one-after the last element in the list. O(1).
		iterator         before_begin();          ///< Returns iterator to position before begin. O(1).
		const_iterator   before_begin() const;    ///< Returns iterator to previous position. O(1).
		const_iterator   cbefore_begin() const;   ///< Returns iterator to previous position. O(1).

		iterator         previous(const_iterator position);         ///< Returns iterator to previous position. O(n).
		const_iterator   previous(const_iterator position) const;   ///< Returns iterator to previous position. O(n).

		reference        front();                 ///< Returns a reference to the first element. The list must be empty.
		const_reference  front() const;           ///< Returns a const reference to the first element. The list must be empty.

		void             push_front(value_type& value);                 ///< Adds an element to the front of the list; O(1). The element is not copied. The element must not be in any other list.
		void             pop_front();                                   ///< Removes an element from the back of the list; O(n). The element must be present, but is not deallocated.

		bool             contains(const value_type& value) const;       ///< Returns true if the given element is in the list; O(n). Equivalent to (locate(x) != end()).

		iterator         locate(value_type& value);                     ///< Converts a reference to an object in the list back to an iterator, or returns end() if it is not part of the list. O(n)
		const_iterator   locate(const value_type& value) const;         ///< Converts a const reference to an object in the list back to a const iterator, or returns end() if it is not part of the list. O(n)

		iterator insert(iterator position, value_type& value);          ///< Inserts an element before the element pointed to by the iterator. O(n)
		iterator insert_after(iterator position, value_type& value);    ///< Inserts an element after the element pointed to by the iterator. O(1)

		iterator erase(iterator position);                              ///< Erases the element pointed to by the iterator. O(n)
		iterator erase_after(iterator position);                        ///< Erases the element after the element pointed to by the iterator. O(1)

		iterator erase(iterator first, iterator last);                  ///< Erases elements within the iterator range [first, last). O(n).
		iterator erase_after(iterator before_first, iterator last);     ///< Erases elements within the iterator range [before_first, last). O(1).

		void swap(this_type& x);                                        ///< Swaps the contents of two intrusive lists; O(1).


		void splice(iterator position, value_type& value);              ///< Moves the given element into this list before the element pointed to by position; O(n).
																		///< Required: x must be in some list or have first/next pointers that point it itself.

		void splice(iterator position, this_type& x);                   ///< Moves the contents of a list into this list before the element pointed to by position; O(n).
																		///< Required: &x != this (same as std::list).

		void splice(iterator position, this_type& x, iterator xPosition);      ///< Moves the given element pointed to i within the list x into the current list before
																			   ///< the element pointed to by position; O(n).

		void splice(iterator position, this_type& x, iterator first, iterator last);   ///< Moves the range of elements [first, last) from list x into the current list before
																					   ///< the element pointed to by position; O(n).
																					   ///< Required: position must not be in [first, last). (same as std::list).

		void splice_after(iterator position, value_type& value);            ///< Moves the given element into this list after the element pointed to by position; O(1).
																			///< Required: x must be in some list or have first/next pointers that point it itself.

		void splice_after(iterator position, this_type& x);                 ///< Moves the contents of a list into this list after the element pointed to by position; O(n).
																			///< Required: &x != this (same as std::list).

		void splice_after(iterator position, this_type& x, iterator xPrevious); ///< Moves the element after xPrevious to be after position. O(1).
																				///< Required: &x != this (same as std::list).

		void splice_after(iterator position, this_type& x, iterator before_first, iterator before_last);  ///< Moves the elements in the range of [before_first+1, before_last+1) to be after position. O(1).

		bool validate() const;
		int  validate_iterator(const_iterator i) const;

	}; // intrusive_slist




	///////////////////////////////////////////////////////////////////////
	// IntrusiveSListIterator
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Pointer, typename Reference>
	inline IntrusiveSListIterator<T, Pointer, Reference>::IntrusiveSListIterator()
	{
		#if EASTL_DEBUG
			mpNode = NULL;
		#endif
	}

	template <typename T, typename Pointer, typename Reference>
	inline IntrusiveSListIterator<T, Pointer, Reference>::IntrusiveSListIterator(pointer pNode)
		: mpNode(pNode)
	{
	}

	template <typename T, typename Pointer, typename Reference>
	inline IntrusiveSListIterator<T, Pointer, Reference>::IntrusiveSListIterator(const iterator& x)
		: mpNode(x.mpNode)
	{
	}


	///////////////////////////////////////////////////////////////////////
	// intrusive_slist_base
	///////////////////////////////////////////////////////////////////////

	// To do.


	///////////////////////////////////////////////////////////////////////
	// intrusive_slist
	///////////////////////////////////////////////////////////////////////

	// To do.


	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename T>
	bool operator==(const intrusive_slist<T>& a, const intrusive_slist<T>& b)
	{
		// If we store an mSize member for intrusive_slist, we want to take advantage of it here.
		typename intrusive_slist<T>::const_iterator ia   = a.begin();
		typename intrusive_slist<T>::const_iterator ib   = b.begin();
		typename intrusive_slist<T>::const_iterator enda = a.end();
		typename intrusive_slist<T>::const_iterator endb = b.end();

		while((ia != enda) && (ib != endb) && (*ia == *ib))
		{
			++ia;
			++ib;
		}
		return (ia == enda) && (ib == endb);
	}

	template <typename T>
	bool operator<(const intrusive_slist<T>& a, const intrusive_slist<T>& b)
	{
		return eastl::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
	}

	template <typename T>
	bool operator!=(const intrusive_slist<T>& a, const intrusive_slist<T>& b)
	{
		return !(a == b);
	}

	template <typename T>
	bool operator>(const intrusive_slist<T>& a, const intrusive_slist<T>& b)
	{
		return b < a;
	}

	template <typename T>
	bool operator<=(const intrusive_slist<T>& a, const intrusive_slist<T>& b)
	{
		return !(b < a);
	}

	template <typename T>
	bool operator>=(const intrusive_slist<T>& a, const intrusive_slist<T>& b)
	{
		return !(a < b);
	}

	template <typename T>
	void swap(intrusive_slist<T>& a, intrusive_slist<T>& b)
	{
		a.swap(b);
	}

} // namespace eastl


#endif // Header include guard
























