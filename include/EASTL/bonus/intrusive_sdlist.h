/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// intrusive_sdlist is a special kind of intrusive list which we say is 
// "singly-doubly" linked. Instead of having a typical intrusive list node
// which looks like this:
//
//     struct intrusive_sdlist_node {
//         intrusive_sdlist_node *mpNext;
//         intrusive_sdlist_node *mpPrev;
//     };
//
// We instead have one that looks like this:
//
//     struct intrusive_sdlist_node {
//         intrusive_sdlist_node*  mpNext;
//         intrusive_sdlist_node** mppPrevNext;
//     };
// 
// This may seem to be suboptimal, but it has one specific advantage: it allows
// the intrusive_sdlist class to be the size of only one pointer instead of two.
// This may seem like a minor optimization, but some users have wanted to create
// thousands of empty instances of these.
// This is because while an intrusive_list class looks like this:
//
//     class intrusive_list {
//         intrusive_list_node mBaseNode;
//     };
//     
// an intrusive_sdlist class looks like this:
//
//     class intrusive_sdlist {
//         intrusive_sdlist_node* mpNext;
//     };
//  
// So here we make a list of plusses and minuses of intrusive sdlists
// compared to intrusive_lists and intrusive_slists:
//
//                          |   list   |   slist   |   sdlist
//      ---------------------------------------------------------
//      min size            |    8     |     4     |     4
//      node size           |    8     |     4     |     8
//      anonymous erase     |   yes    |     no    |     yes
//      reverse iteration   |   yes    |     no    |     no
//    
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTRUSIVE_SDLIST_H
#define EASTL_INTRUSIVE_SDLIST_H


#include <EASTL/internal/config.h>
#include <EASTL/iterator.h>
#include <EASTL/algorithm.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{


	/// intrusive_sdlist_node
	///
	struct intrusive_sdlist_node
	{
		intrusive_sdlist_node*  mpNext;
		intrusive_sdlist_node** mppPrevNext;
	};


	/// IntrusiveSDListIterator
	///
	template <typename T, typename Pointer, typename Reference>
	struct IntrusiveSDListIterator
	{
		typedef IntrusiveSDListIterator<T, Pointer, Reference>   this_type;
		typedef IntrusiveSDListIterator<T, T*, T&>               iterator;
		typedef IntrusiveSDListIterator<T, const T*, const T&>   const_iterator;
		typedef eastl_size_t                                     size_type;     // See config.h for the definition of eastl_size_t, which defaults to size_t.
		typedef ptrdiff_t                                        difference_type;
		typedef T                                                value_type;
		typedef T                                                node_type;
		typedef Pointer                                          pointer;
		typedef Reference                                        reference;
		typedef EASTL_ITC_NS::forward_iterator_tag               iterator_category;

	public:
		pointer mpNode;

	public:
		IntrusiveSDListIterator();
		explicit IntrusiveSDListIterator(pointer pNode); // Note that you can also construct an iterator from T via this, since value_type == node_type.
		IntrusiveSDListIterator(const iterator& x);

		reference operator*() const;
		pointer   operator->() const;

		this_type& operator++();
		this_type  operator++(int);

	}; // struct IntrusiveSDListIterator




	/// intrusive_sdlist_base
	///
	/// Provides a template-less base class for intrusive_sdlist.
	///
	class intrusive_sdlist_base
	{
	public:
		typedef eastl_size_t size_type;     // See config.h for the definition of eastl_size_t, which defaults to size_t.
		typedef ptrdiff_t    difference_type;

	protected:
		intrusive_sdlist_node* mpNext;

	public:
		intrusive_sdlist_base();

		bool      empty() const;            ///< Returns true if the container is empty.
		size_type size() const;             ///< Returns the number of elements in the list; O(n).

		void      clear();                  ///< Clears the list; O(1). No deallocation occurs.
		void      pop_front();              ///< Removes an element from the front of the list; O(1). The element must be present, but is not deallocated.
		void      reverse();                ///< Reverses a list so that front and back are swapped; O(n).

		//bool    validate() const;         ///< Scans a list for linkage inconsistencies; O(n) time, O(1) space. Returns false if errors are detected, such as loops or branching.

	}; // class intrusive_sdlist_base



	/// intrusive_sdlist
	///
	template <typename T = intrusive_sdlist_node>
	class intrusive_sdlist : public intrusive_sdlist_base
	{
	public:
		typedef intrusive_sdlist<T>                             this_type;
		typedef intrusive_sdlist_base                           base_type;
		typedef T                                               node_type;
		typedef T                                               value_type;
		typedef typename base_type::size_type                   size_type;
		typedef typename base_type::difference_type             difference_type;
		typedef T&                                              reference;
		typedef const T&                                        const_reference;
		typedef T*                                              pointer;
		typedef const T*                                        const_pointer;
		typedef IntrusiveSDListIterator<T, T*, T&>              iterator;
		typedef IntrusiveSDListIterator<T, const T*, const T&>  const_iterator;
		typedef eastl::reverse_iterator<iterator>               reverse_iterator;
		typedef eastl::reverse_iterator<const_iterator>         const_reverse_iterator;

	public:
		intrusive_sdlist();                       ///< Creates an empty list.
		intrusive_sdlist(const this_type& x);     ///< Creates an empty list; ignores the argument.
		this_type& operator=(const this_type& x); ///< Clears the list; ignores the argument.

		iterator         begin();                 ///< Returns an iterator pointing to the first element in the list.
		const_iterator   begin() const;           ///< Returns a const_iterator pointing to the first element in the list.
		const_iterator   cbegin() const;          ///< Returns a const_iterator pointing to the first element in the list.

		iterator         end();                   ///< Returns an iterator pointing one-after the last element in the list.
		const_iterator   end() const;             ///< Returns a const_iterator pointing one-after the last element in the list.
		const_iterator   cend() const;            ///< Returns a const_iterator pointing one-after the last element in the list.

		reference        front();                 ///< Returns a reference to the first element. The list must not be empty.
		const_reference  front() const;           ///< Returns a const reference to the first element. The list must not be empty.

		void             push_front(value_type& value);                  ///< Adds an element to the front of the list; O(1). The element is not copied. The element must not be in any other list.
		void             push_back(value_type& value);                   ///< Adds an element to the back of the list; O(N). The element is not copied. The element must not be in any other list.
		void             pop_back();                                     ///< Removes an element from the back of the list; O(N). The element must be present, but is not deallocated.

		bool             contains(const value_type& value) const;        ///< Returns true if the given element is in the list; O(n). Equivalent to (locate(x) != end()).

		iterator         locate(value_type& value);                      ///< Converts a reference to an object in the list back to an iterator, or returns end() if it is not part of the list. O(n)
		const_iterator   locate(const value_type& value) const;          ///< Converts a const reference to an object in the list back to a const iterator, or returns end() if it is not part of the list. O(n)

		iterator         insert(iterator position, value_type& value);   ///< Inserts an element before the element pointed to by the iterator. O(1)
		iterator         erase(iterator position);                       ///< Erases the element pointed to by the iterator. O(1)
		iterator         erase(iterator first, iterator last);           ///< Erases elements within the iterator range [first, last). O(1).
		void             swap(intrusive_sdlist& x);                      ///< Swaps the contents of two intrusive lists; O(1).

		static void      remove(value_type& value);                      ///< Erases an element from a list; O(1). Note that this is static so you don't need to know which list the element, although it must be in some list.

		void  splice(iterator position, value_type& value);              ///< Moves the given element into this list before the element pointed to by position; O(1).
																		 ///< Required: x must be in some list or have first/next pointers that point it itself.

		void  splice(iterator position, this_type& x);                   ///< Moves the contents of a list into this list before the element pointed to by position; O(1).
																		 ///< Required: &x != this (same as std::list).

		void  splice(iterator position, this_type& x, iterator xPosition);      ///< Moves the given element pointed to i within the list x into the current list before
																				///< the element pointed to by position; O(1).

		void  splice(iterator position, this_type& x, iterator first, iterator last);   ///< Moves the range of elements [first, last) from list x into the current list before
																						///< the element pointed to by position; O(1).
																						///< Required: position must not be in [first, last). (same as std::list).
		bool validate() const;
		int  validate_iterator(const_iterator i) const;

	}; // intrusive_sdlist




	///////////////////////////////////////////////////////////////////////
	// IntrusiveSDListIterator functions
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Pointer, typename Reference>
	inline IntrusiveSDListIterator<T, Pointer, Reference>::IntrusiveSDListIterator()
	{
		#if EASTL_DEBUG
			mpNode = NULL;
		#endif
	}

	template <typename T, typename Pointer, typename Reference>
	inline IntrusiveSDListIterator<T, Pointer, Reference>::IntrusiveSDListIterator(pointer pNode)
		: mpNode(pNode)
	{
	}

	template <typename T, typename Pointer, typename Reference>
	inline IntrusiveSDListIterator<T, Pointer, Reference>::IntrusiveSDListIterator(const iterator& x)
		: mpNode(x.mpNode)
	{
	}

	template <typename T, typename Pointer, typename Reference>
	inline typename IntrusiveSDListIterator<T, Pointer, Reference>::reference
	IntrusiveSDListIterator<T, Pointer, Reference>::operator*() const
	{
		return *mpNode;
	}

	template <typename T, typename Pointer, typename Reference>
	inline typename IntrusiveSDListIterator<T, Pointer, Reference>::pointer
	IntrusiveSDListIterator<T, Pointer, Reference>::operator->() const
	{
		return mpNode;
	}

	template <typename T, typename Pointer, typename Reference>
	inline typename IntrusiveSDListIterator<T, Pointer, Reference>::this_type&
	IntrusiveSDListIterator<T, Pointer, Reference>::operator++()
	{
		mpNode = static_cast<node_type*>(mpNode->mpNext);
		return *this;
	}

	template <typename T, typename Pointer, typename Reference>
	inline typename IntrusiveSDListIterator<T, Pointer, Reference>::this_type
	IntrusiveSDListIterator<T, Pointer, Reference>::operator++(int)
	{
		this_type temp = *this;
		mpNode = static_cast<node_type*>(mpNode->mpNext);
		return temp;
	}

	// The C++ defect report #179 requires that we support comparisons between const and non-const iterators.
	// Thus we provide additional template paremeters here to support this. The defect report does not
	// require us to support comparisons between reverse_iterators and const_reverse_iterators.
	template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
	inline bool operator==(const IntrusiveSDListIterator<T, PointerA, ReferenceA>& a, 
						   const IntrusiveSDListIterator<T, PointerB, ReferenceB>& b)
	{
		return a.mpNode == b.mpNode;
	}


	template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
	inline bool operator!=(const IntrusiveSDListIterator<T, PointerA, ReferenceA>& a, 
						   const IntrusiveSDListIterator<T, PointerB, ReferenceB>& b)
	{
		return a.mpNode != b.mpNode;
	}


	// We provide a version of operator!= for the case where the iterators are of the 
	// same type. This helps prevent ambiguity errors in the presence of rel_ops.
	template <typename T, typename Pointer, typename Reference>
	inline bool operator!=(const IntrusiveSDListIterator<T, Pointer, Reference>& a, 
						   const IntrusiveSDListIterator<T, Pointer, Reference>& b)
	{
		return a.mpNode != b.mpNode;
	}



	///////////////////////////////////////////////////////////////////////
	// intrusive_sdlist_base
	///////////////////////////////////////////////////////////////////////

	inline intrusive_sdlist_base::intrusive_sdlist_base() 
		{ mpNext = NULL; }


	inline bool intrusive_sdlist_base::empty() const
		{ return mpNext == NULL; }


	inline intrusive_sdlist_base::size_type intrusive_sdlist_base::size() const
	{
		size_type n = 0;
		for(const intrusive_sdlist_node* pCurrent = mpNext; pCurrent; pCurrent = pCurrent->mpNext)
			n++;
		return n;
	}


	inline void intrusive_sdlist_base::clear()
		{ mpNext = NULL; } // Note that we don't do anything with the list nodes.


	inline void intrusive_sdlist_base::pop_front()
	{
		// To consider: Set mpNext's pointers to NULL in debug builds.
		mpNext = mpNext->mpNext;
		mpNext->mppPrevNext = &mpNext;
	}



	///////////////////////////////////////////////////////////////////////
	// intrusive_sdlist
	///////////////////////////////////////////////////////////////////////

	template <typename T>
	inline intrusive_sdlist<T>::intrusive_sdlist()
	{
	}


	template <typename T>
	inline intrusive_sdlist<T>::intrusive_sdlist(const this_type& /*x*/)
	  : intrusive_sdlist_base()
	{
		// We intentionally ignore argument x.
	}


	template <typename T>
	inline typename intrusive_sdlist<T>::this_type& intrusive_sdlist<T>::operator=(const this_type& /*x*/)
	{ 
		return *this; // We intentionally ignore argument x.
	}


	template <typename T>
	inline typename intrusive_sdlist<T>::iterator intrusive_sdlist<T>::begin()
		{ return iterator(static_cast<T*>(mpNext)); }


	template <typename T>
	inline typename intrusive_sdlist<T>::const_iterator intrusive_sdlist<T>::begin() const
		{ return const_iterator(static_cast<T*>(const_cast<intrusive_sdlist_node*>(mpNext))); }


	template <typename T>
	inline typename intrusive_sdlist<T>::const_iterator intrusive_sdlist<T>::cbegin() const
		{ return const_iterator(static_cast<T*>(const_cast<intrusive_sdlist_node*>(mpNext))); }


	template <typename T>
	inline typename intrusive_sdlist<T>::iterator intrusive_sdlist<T>::end()
		{ return iterator(static_cast<T*>(NULL)); }


	template <typename T>
	inline typename intrusive_sdlist<T>::const_iterator intrusive_sdlist<T>::end() const
		{ return const_iterator(static_cast<const T*>(NULL)); }


	template <typename T>
	inline typename intrusive_sdlist<T>::const_iterator intrusive_sdlist<T>::cend() const
		{ return const_iterator(static_cast<const T*>(NULL)); }


	template <typename T>
	inline typename intrusive_sdlist<T>::reference intrusive_sdlist<T>::front()
		{ return *static_cast<T*>(mpNext); }


	template <typename T>
	inline typename intrusive_sdlist<T>::const_reference intrusive_sdlist<T>::front() const
		{ return *static_cast<const T*>(mpNext); }


	template <typename T>
	inline void intrusive_sdlist<T>::push_front(value_type& value)
	{
		value.mpNext = mpNext;
		value.mppPrevNext = &mpNext;
		if(mpNext)
			mpNext->mppPrevNext = &value.mpNext;
		mpNext = &value;
	}


	template <typename T>
	inline void intrusive_sdlist<T>::push_back(value_type& value)
	{
		intrusive_sdlist_node*  pNext      =  mpNext;
		intrusive_sdlist_node** ppPrevNext = &mpNext;

		while(pNext)
		{
			ppPrevNext = &pNext->mpNext;
			pNext      =  pNext->mpNext;
		}

		*ppPrevNext       = &value;
		value.mppPrevNext = ppPrevNext;
		value.mpNext      = NULL;
	}


	template <typename T>
	inline void intrusive_sdlist<T>::pop_back()
	{
		node_type* pCurrent = static_cast<node_type*>(mpNext);

		while(pCurrent->mpNext)
			pCurrent = static_cast<node_type*>(pCurrent->mpNext);

		*pCurrent->mppPrevNext = NULL;
	}

	template <typename T>
	inline bool intrusive_sdlist<T>::contains(const value_type& value) const
	{
		const intrusive_sdlist_node* pCurrent;

		for(pCurrent = mpNext; pCurrent; pCurrent = pCurrent->mpNext)
		{
			if(pCurrent == &value)
				break;
		}

		return (pCurrent != NULL);
	}


	template <typename T>
	inline typename intrusive_sdlist<T>::iterator intrusive_sdlist<T>::locate(value_type& value)
	{
		intrusive_sdlist_node* pCurrent;

		for(pCurrent = static_cast<value_type*>(mpNext); pCurrent; pCurrent = pCurrent->mpNext)
		{
			if(pCurrent == &value)
				break;
		}

		return iterator(static_cast<value_type*>(pCurrent));
	}


	template <typename T>
	inline typename intrusive_sdlist<T>::const_iterator intrusive_sdlist<T>::locate(const T& value) const
	{
		const intrusive_sdlist_node* pCurrent;

		for(pCurrent = static_cast<value_type*>(mpNext); pCurrent; pCurrent = pCurrent->mpNext)
		{
			if(pCurrent == &value)
				break;
		}

		return const_iterator(static_cast<value_type*>(const_cast<intrusive_sdlist_node*>(pCurrent)));
	}


	template <typename T>
	inline typename intrusive_sdlist<T>::iterator
	intrusive_sdlist<T>::insert(iterator position, value_type& value)
	{
		value.mppPrevNext            = position.mpNode->mppPrevNext;
		value.mpNext                 = position.mpNode;
	   *value.mppPrevNext            = &value;
		position.mpNode->mppPrevNext = &value.mpNext;

		return iterator(&value);
	}


	template <typename T>
	inline typename intrusive_sdlist<T>::iterator
	intrusive_sdlist<T>::erase(iterator position)
	{
	   *position.mpNode->mppPrevNext         = position.mpNode->mpNext;
		position.mpNode->mpNext->mppPrevNext = position.mpNode->mppPrevNext;

		return iterator(position.mpNode);
	}


	template <typename T>
	inline typename intrusive_sdlist<T>::iterator
	intrusive_sdlist<T>::erase(iterator first, iterator last)
	{
		if(first.mpNode) // If not erasing the end...
		{
			*first.mpNode->mppPrevNext = last.mpNode;

			if(last.mpNode) // If not erasing to the end...
				last.mpNode->mppPrevNext = first.mpNode->mppPrevNext;
		}

		return last;
	}


	template <typename T>
	inline void intrusive_sdlist<T>::remove(value_type& value)
	{
		*value.mppPrevNext = value.mpNext;
		if(value.mpNext)
			value.mpNext->mppPrevNext = value.mppPrevNext;
	}


	template <typename T>
	void intrusive_sdlist<T>::swap(intrusive_sdlist& x)
	{
		// swap anchors
		intrusive_sdlist_node* const temp(mpNext);
		mpNext   = x.mpNext;
		x.mpNext = temp;

		if(x.mpNext)
			x.mpNext->mppPrevNext = &mpNext;

		if(mpNext)
			mpNext->mppPrevNext = &x.mpNext;
	}





	// To do: Complete these splice functions. Might want to look at intrusive_sdlist for help.

	template <typename T>
	void intrusive_sdlist<T>::splice(iterator /*position*/, value_type& /*value*/)
	{
		EASTL_ASSERT(false); // If you need this working, ask Paul Pedriana or submit a working version for inclusion.
	}


	template <typename T>
	void intrusive_sdlist<T>::splice(iterator /*position*/, intrusive_sdlist& /*x*/)
	{
		EASTL_ASSERT(false); // If you need this working, ask Paul Pedriana or submit a working version for inclusion.
	}


	template <typename T>
	void intrusive_sdlist<T>::splice(iterator /*position*/, intrusive_sdlist& /*x*/, iterator /*xPosition*/)
	{
		EASTL_ASSERT(false); // If you need this working, ask Paul Pedriana or submit a working version for inclusion.
	}


	template <typename T>
	void intrusive_sdlist<T>::splice(iterator /*position*/, intrusive_sdlist& /*x*/, iterator /*first*/, iterator /*last*/)
	{
		EASTL_ASSERT(false); // If you need this working, ask Paul Pedriana or submit a working version for inclusion.
	}


	template <typename T>
	inline bool intrusive_sdlist<T>::validate() const
	{
		return true; // To do.
	}


	template <typename T>
	inline int intrusive_sdlist<T>::validate_iterator(const_iterator i) const
	{
		// To do: Come up with a more efficient mechanism of doing this.

		for(const_iterator temp = begin(), tempEnd = end(); temp != tempEnd; ++temp)
		{
			if(temp == i)
				return (isf_valid | isf_current | isf_can_dereference);
		}

		if(i == end())
			return (isf_valid | isf_current); 

		return isf_none;
	}




	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename T>
	bool operator==(const intrusive_sdlist<T>& a, const intrusive_sdlist<T>& b)
	{
		// If we store an mSize member for intrusive_sdlist, we want to take advantage of it here.
		typename intrusive_sdlist<T>::const_iterator ia   = a.begin();
		typename intrusive_sdlist<T>::const_iterator ib   = b.begin();
		typename intrusive_sdlist<T>::const_iterator enda = a.end();
		typename intrusive_sdlist<T>::const_iterator endb = b.end();

		while((ia != enda) && (ib != endb) && (*ia == *ib))
		{
			++ia;
			++ib;
		}
		return (ia == enda) && (ib == endb);
	}

	template <typename T>
	bool operator<(const intrusive_sdlist<T>& a, const intrusive_sdlist<T>& b)
	{
		return eastl::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
	}

	template <typename T>
	bool operator!=(const intrusive_sdlist<T>& a, const intrusive_sdlist<T>& b)
	{
		return !(a == b);
	}

	template <typename T>
	bool operator>(const intrusive_sdlist<T>& a, const intrusive_sdlist<T>& b)
	{
		return b < a;
	}

	template <typename T>
	bool operator<=(const intrusive_sdlist<T>& a, const intrusive_sdlist<T>& b)
	{
		return !(b < a);
	}

	template <typename T>
	bool operator>=(const intrusive_sdlist<T>& a, const intrusive_sdlist<T>& b)
	{
		return !(a < b);
	}

	template <typename T>
	void swap(intrusive_sdlist<T>& a, intrusive_sdlist<T>& b)
	{
		a.swap(b);
	}


} // namespace eastl


#endif // Header include guard
























