///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// The intrusive list container is similar to a list, with the primary
// different being that intrusive lists allow you to control memory
// allocation.
//
// * Intrusive lists store the nodes directly in the data items. This
//   is done by deriving the object from intrusive_list_node.
//
// * The container does no memory allocation -- it works entirely with
//   the submitted nodes. This does mean that it is the client's job to 
//   free the nodes in an intrusive list, though.
//
// * Valid node pointers can be converted back to iterators in O(1).
//   This is because objects in the list are also nodes in the list.
//
// * intrusive_list does not support copy construction or assignment; 
//   the push, pop, and insert operations take ownership of the 
//   passed object.
//
// Usage notes:
//
// * You can use an intrusive_list directly with the standard nodes
//   if you have some other way of converting the node pointer back
//   to your data pointer.
//
// * Remember that the list destructor doesn't deallocate nodes -- it can't.
//
// * The size is not cached; this makes size() linear time but splice() is
//   constant time. This does mean that you can remove() an element without
//   having to figure out which list it is in, however.
//
// * You can insert a node into multiple intrusive_lists. One way to do so
//   is to (ab)use inheritance:
//
//      struct NodeA : public intrusive_list_node {};
//      struct NodeB : public intrusive_list_node {};
//      struct Object : public NodeA, nodeB {};
//
//      intrusive_list<NodeA> listA;
//      intrusive_list<NodeB> listB;
//
//      listA.push_back(obj);
//      listB.push_back(obj);
//
// * find() vs. locate()
//   The find(v) algorithm returns an iterator p such that *p == v; intrusive_list::locate(v) 
//   returns an iterator p such that &*p == &v. intrusive_list<> doesn't have find() mainly 
//   because list<> doesn't have it either, but there's no reason it couldn't. intrusive_list
//   uses the name 'find' because:
//      - So as not to confuse the member function with the well-defined free function from algorithm.h.
//      - Because it is not API-compatible with eastl::find().
//      - Because it simply locates an object within the list based on its node entry and doesn't perform before any value-based searches or comparisons.
//
// Differences between intrusive_list and std::list:
//
// Issue                            std::list       intrusive_list
// --------------------------------------------------------------
// Automatic node ctor/dtor         Yes             No
// Can memmove() container          Maybe*          No
// Same item in list twice          Yes(copy/byref) No
// Can store non-copyable items     No              Yes
// size()                           O(1) or O(n)    O(n)
// clear()                          O(n)            O(1)
// erase(range)                     O(n)            O(1)
// splice(range)                    O(1) or O(n)    O(1)
// Convert reference to iterator    No              O(1)
// Remove without container         No              O(1)
// Nodes in mixed allocators        No              Yes
//
// *) Not required by standard but can be done with some STL implementations.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTRUSIVE_LIST_H
#define EASTL_INTRUSIVE_LIST_H


#include <EASTL/internal/config.h>
#include <EASTL/iterator.h>
#include <EASTL/algorithm.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	template<class T>
	class intrusive_list;

	/// intrusive_list_node
	///
	/// By design this must be a POD, as user structs will be inheriting from 
	/// it and they may wish to remain POD themselves. However, if the 
	/// EASTL_VALIDATE_INTRUSIVE_LIST option is enabled
	/// 
	struct intrusive_list_node
	{
		intrusive_list_node* mpNext;
		intrusive_list_node* mpPrev;

		#if EASTL_VALIDATE_INTRUSIVE_LIST
			intrusive_list_node()       // Implemented inline because GCC can't deal with member functions
			{                           // of may-alias classes being defined outside the declaration.
				mpNext = mpPrev = nullptr;
			}

			~intrusive_list_node()
			{
				#if EASTL_ASSERT_ENABLED
					if(mpNext || mpPrev)
						EASTL_FAIL_MSG("~intrusive_list_node(): List is non-empty.");
				#endif
			}
		#endif
	} EASTL_MAY_ALIAS;  // It's not clear if this really should be needed. An old GCC compatible compiler is generating some crashing optimized code when strict aliasing is enabled, but analysis of it seems to blame the compiler. However, this topic can be tricky.



	/// intrusive_list_iterator
	///
	template <typename T, typename Pointer, typename Reference>
	class intrusive_list_iterator
	{
	public:
		typedef intrusive_list_iterator<T, Pointer, Reference>   this_type;
		typedef intrusive_list_iterator<T, T*, T&>               iterator;
		typedef intrusive_list_iterator<T, const T*, const T&>   const_iterator;
		typedef T                                                value_type;
		typedef T                                                node_type;
		typedef intrusive_list_node                              base_node_type;
		typedef ptrdiff_t                                        difference_type;
		typedef Pointer                                          pointer;
		typedef Reference                                        reference;
		typedef EASTL_ITC_NS::bidirectional_iterator_tag         iterator_category;
#if EA_IS_ENABLED(EASTL_DEPRECATIONS_FOR_2024_APRIL)
	private:
		base_node_type* mpNode;
#else
	public:
		pointer mpNode;
#endif

	public:
		intrusive_list_iterator();

		// Note: you can also construct an iterator from T* via this, since T should inherit from
		// intrusive_list_node.
		explicit intrusive_list_iterator(const base_node_type* pNode);
		// Note: this isn't always a copy constructor, iterator is not always equal to this_type
		intrusive_list_iterator(const iterator& x);
		// Note: this isn't always a copy assignment operator, iterator is not always equal to this_type
		intrusive_list_iterator& operator=(const iterator& x);

		// Calling these on the end() of a list invokes undefined behavior.
		reference operator*() const;
		pointer   operator->() const;

		// Returns a pointer to the fully typed node (the same as operator->) this is useful when
		// iterating a list to destroy all the nodes, calling this on the end() of a list results in
		// undefined behavior.
		pointer nodePtr() const;

		intrusive_list_iterator& operator++();
		intrusive_list_iterator& operator--();

		intrusive_list_iterator operator++(int);
		intrusive_list_iterator operator--(int);

		// The C++ defect report #179 requires that we support comparisons between const and non-const iterators.
		// Thus we provide additional template paremeters here to support this. The defect report does not
		// require us to support comparisons between reverse_iterators and const_reverse_iterators.
		template <class PointerB, class ReferenceB>
		bool operator==(const intrusive_list_iterator<T, PointerB, ReferenceB>& other) const
		{
			return mpNode == other.mpNode;
		}

		template <typename PointerB, typename ReferenceB>
		inline bool operator!=(const intrusive_list_iterator<T, PointerB, ReferenceB>& other) const
		{
			return mpNode != other.mpNode;
		}

		// We provide a version of operator!= for the case where the iterators are of the
		// same type. This helps prevent ambiguity errors in the presence of rel_ops.
		inline bool operator!=(const intrusive_list_iterator other) const { return mpNode != other.mpNode; }

	private:

		// This is a temp helper function for the deprecation.
		// It should be removed when the deprecation window ends.
#if EA_IS_ENABLED(EASTL_DEPRECATIONS_FOR_2024_APRIL)
		base_node_type* toInternalNodeType(base_node_type* node) { return node; }
#else
		pointer toInternalNodeType(base_node_type* node) { return static_cast<pointer>(node); }
#endif

		// for the "copy" constructor, which uses non-const iterator even in the
		// const_iterator case.  Also, some of the internal member functions in
		// intrusive_list<T> want to use mpNode.
		friend const_iterator;
		friend intrusive_list<T>;

		// for the comparison operators.
		template<class U, class Pointer1, class Reference1>
		friend class intrusive_list_iterator;
	}; // class intrusive_list_iterator



	/// intrusive_list_base
	///
	class intrusive_list_base
	{
	public:
		typedef eastl_size_t size_type;     // See config.h for the definition of this, which defaults to size_t.
		typedef ptrdiff_t    difference_type;

	protected:
		intrusive_list_node mAnchor;          ///< Sentinel node (end). All data nodes are linked in a ring from this node.

	public:
		intrusive_list_base();
	   ~intrusive_list_base();

		bool            empty() const EA_NOEXCEPT;
		eastl_size_t    size() const EA_NOEXCEPT;  ///< Returns the number of elements in the list; O(n).
		void            clear() EA_NOEXCEPT;       ///< Clears the list; O(1). No deallocation occurs.
		void            pop_front();               ///< Removes an element from the front of the list; O(1). The element must exist, but is not deallocated.
		void            pop_back();                ///< Removes an element from the back of the list; O(1). The element must exist, but is not deallocated.
		EASTL_API void  reverse() EA_NOEXCEPT;     ///< Reverses a list so that front and back are swapped; O(n).

		EASTL_API bool  validate() const;          ///< Scans a list for linkage inconsistencies; O(n) time, O(1) space. Returns false if errors are detected, such as loops or branching.

	}; // class intrusive_list_base



	/// intrusive_list
	///
	/// Example usage:
	///    struct IntNode : public eastl::intrusive_list_node {
	///        int mX;
	///        IntNode(int x) : mX(x) { }
	///    };
	///    
	///    IntNode nodeA(0);
	///    IntNode nodeB(1);
	///    
	///    intrusive_list<IntNode> intList;
	///    intList.push_back(nodeA);
	///    intList.push_back(nodeB);
	///    intList.remove(nodeA);
	///    
	template <typename T = intrusive_list_node>
	class intrusive_list : public intrusive_list_base
	{
	public:
		typedef intrusive_list<T>                               this_type;
		typedef intrusive_list_base                             base_type;
		typedef T                                               node_type;
		typedef T                                               value_type;
		typedef typename base_type::size_type                   size_type;
		typedef typename base_type::difference_type             difference_type;
		typedef T&                                              reference;
		typedef const T&                                        const_reference;
		typedef T*                                              pointer;
		typedef const T*                                        const_pointer;
		typedef intrusive_list_iterator<T, T*, T&>              iterator;
		typedef intrusive_list_iterator<T, const T*, const T&>  const_iterator;
		typedef eastl::reverse_iterator<iterator>               reverse_iterator;
		typedef eastl::reverse_iterator<const_iterator>         const_reverse_iterator;

	public:
		intrusive_list();                                ///< Creates an empty list.
		intrusive_list(const this_type& x);              ///< Creates an empty list; ignores the argument.
	  //intrusive_list(std::initializer_list<value_type> ilist); To consider: Is this feasible, given how initializer_list works by creating a temporary array? Even if it is feasible, is it a good idea?

		this_type&  operator=(const this_type& x);       ///< Clears the list; ignores the argument.
		void        swap(this_type&);                    ///< Swaps the contents of two intrusive lists; O(1).

		iterator                begin() EA_NOEXCEPT;                 ///< Returns an iterator pointing to the first element in the list.
		const_iterator          begin() const EA_NOEXCEPT;           ///< Returns a const_iterator pointing to the first element in the list.
		const_iterator          cbegin() const EA_NOEXCEPT;          ///< Returns a const_iterator pointing to the first element in the list.

		iterator                end() EA_NOEXCEPT;                   ///< Returns an iterator pointing one-after the last element in the list.
		const_iterator          end() const EA_NOEXCEPT;             ///< Returns a const_iterator pointing one-after the last element in the list.
		const_iterator          cend() const EA_NOEXCEPT;            ///< Returns a const_iterator pointing one-after the last element in the list.

		reverse_iterator        rbegin() EA_NOEXCEPT;                ///< Returns a reverse_iterator pointing at the end of the list (start of the reverse sequence).
		const_reverse_iterator  rbegin() const EA_NOEXCEPT;          ///< Returns a const_reverse_iterator pointing at the end of the list (start of the reverse sequence).
		const_reverse_iterator  crbegin() const EA_NOEXCEPT;         ///< Returns a const_reverse_iterator pointing at the end of the list (start of the reverse sequence).

		reverse_iterator        rend() EA_NOEXCEPT;                  ///< Returns a reverse_iterator pointing at the start of the list (end of the reverse sequence).
		const_reverse_iterator  rend() const EA_NOEXCEPT;            ///< Returns a const_reverse_iterator pointing at the start of the list (end of the reverse sequence).
		const_reverse_iterator  crend() const EA_NOEXCEPT;           ///< Returns a const_reverse_iterator pointing at the start of the list (end of the reverse sequence).
		
		reference               front();                 ///< Returns a reference to the first element. The list must be non-empty.
		const_reference         front() const;           ///< Returns a const reference to the first element. The list must be non-empty.
		reference               back();                  ///< Returns a reference to the last element. The list must be non-empty.
		const_reference         back() const;            ///< Returns a const reference to the last element. The list must be non-empty.

		void        push_front(value_type& x);             ///< Adds an element to the front of the list; O(1). The element is not copied. The element must not be in any other list.
		void        push_back(value_type& x);              ///< Adds an element to the back of the list; O(1). The element is not copied. The element must not be in any other list.

		bool        contains(const value_type& x) const;   ///< Returns true if the given element is in the list; O(n). Equivalent to (locate(x) != end()).

		iterator        locate(value_type& x);             ///< Converts a reference to an object in the list back to an iterator, or returns end() if it is not part of the list. O(n)
		const_iterator  locate(const value_type& x) const; ///< Converts a const reference to an object in the list back to a const iterator, or returns end() if it is not part of the list. O(n)

		iterator    insert(const_iterator pos, value_type& x);   ///< Inserts an element before the element pointed to by the iterator. O(1)
		iterator    erase(const_iterator pos);                   ///< Erases the element pointed to by the iterator. O(1)
		iterator    erase(const_iterator pos, const_iterator last);    ///< Erases elements within the iterator range [pos, last). O(1)

		reverse_iterator erase(const_reverse_iterator pos);
		reverse_iterator erase(const_reverse_iterator pos, const_reverse_iterator last);

		static void remove(value_type& value);                    ///< Erases an element from a list; O(1). Note that this is static so you don't need to know which list the element, although it must be in some list.

		void               splice(const_iterator pos, value_type& x);
				///< Moves the given element into this list before the element pointed to by pos; O(1).
				///< Required: x must be in some list or have first/next pointers that point it itself.

		void               splice(const_iterator pos, intrusive_list& x);
				///< Moves the contents of a list into this list before the element pointed to by pos; O(1).
				///< Required: &x != this (same as std::list).

		void               splice(const_iterator pos, intrusive_list& x, const_iterator i);
				///< Moves the given element pointed to i within the list x into the current list before
				///< the element pointed to by pos; O(1).

		void               splice(const_iterator pos, intrusive_list& x, const_iterator first, const_iterator last);
				///< Moves the range of elements [first, last) from list x into the current list before
				///< the element pointed to by pos; O(1).
				///< Required: pos must not be in [first, last). (same as std::list).

	public:
		// Sorting functionality
		// This is independent of the global sort algorithms, as lists are 
		// linked nodes and can be sorted more efficiently by moving nodes
		// around in ways that global sort algorithms aren't privy to.

		void merge(this_type& x);

		template <typename Compare>
		void merge(this_type& x, Compare compare);

		void unique();

		template <typename BinaryPredicate>
		void unique(BinaryPredicate);

		void sort();

		template<typename Compare>
		void sort(Compare compare);

	public:
		// bool validate() const; // Inherited from parent.
		int     validate_iterator(const_iterator i) const;

	private:
		// This is a helper function to assist with the deprecation,
		// it should be removed after the deprecation window ends.
#if EA_IS_ENABLED(EASTL_DEPRECATIONS_FOR_2024_APRIL)
		intrusive_list_node* toListNode(intrusive_list_node* node) { return node; }
#else
		intrusive_list_node* toListNode(const node_type* node)
		{
			return static_cast<intrusive_list_node*>(const_cast<node_type*>(node));
		}
#endif
	}; // intrusive_list




	///////////////////////////////////////////////////////////////////////
	// intrusive_list_node
	///////////////////////////////////////////////////////////////////////

	// Moved to be inline within the class because the may-alias attribute is
	// triggering what appears to be a bug in GCC that effectively requires 
	// may-alias structs to implement inline member functions within the class
	// declaration. We don't have a .cpp file for 
	// #if EASTL_VALIDATE_INTRUSIVE_LIST
	//     inline intrusive_list_node::intrusive_list_node()
	//     {
	//         mpNext = mpPrev = nullptr;
	//     }
	//
	//     inline intrusive_list_node::~intrusive_list_node()
	//     {
	//         #if EASTL_ASSERT_ENABLED
	//             if(mpNext || mpPrev)
	//                 EASTL_FAIL_MSG("~intrusive_list_node(): List is non-empty.");
	//         #endif
	//     }
	// #endif


	///////////////////////////////////////////////////////////////////////
	// intrusive_list_iterator
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Pointer, typename Reference>
	inline intrusive_list_iterator<T, Pointer, Reference>::intrusive_list_iterator()
	{
		#if EASTL_DEBUG
			mpNode = nullptr;
		#endif
	}


	template <typename T, typename Pointer, typename Reference>
	inline intrusive_list_iterator<T, Pointer, Reference>::intrusive_list_iterator(const base_node_type* pNode)
		: mpNode(toInternalNodeType(const_cast<base_node_type*>(pNode)))
	{
		// Empty
	}


	template <typename T, typename Pointer, typename Reference>
	inline intrusive_list_iterator<T, Pointer, Reference>::intrusive_list_iterator(const iterator& x)
		: mpNode(x.mpNode)
	{
		// Empty
	}

	template <typename T, typename Pointer, typename Reference>
	inline typename intrusive_list_iterator<T, Pointer, Reference>::this_type&
	intrusive_list_iterator<T, Pointer, Reference>::operator=(const iterator& x)
	{
	    mpNode = x.mpNode;
	    return *this;
	}

	template <typename T, typename Pointer, typename Reference>
	inline typename intrusive_list_iterator<T, Pointer, Reference>::reference
	intrusive_list_iterator<T, Pointer, Reference>::operator*() const
	{
		return *static_cast<pointer>(mpNode);
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename intrusive_list_iterator<T, Pointer, Reference>::pointer
	intrusive_list_iterator<T, Pointer, Reference>::operator->() const
	{
		return static_cast<pointer>(mpNode);
	}

	template <typename T, typename Pointer, typename Reference>
	inline typename intrusive_list_iterator<T, Pointer, Reference>::pointer
	intrusive_list_iterator<T, Pointer, Reference>::nodePtr() const
	{
		return static_cast<pointer>(mpNode);
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename intrusive_list_iterator<T, Pointer, Reference>::this_type&
	intrusive_list_iterator<T, Pointer, Reference>::operator++()
	{
		mpNode = toInternalNodeType(mpNode->mpNext);
		return *this;
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename intrusive_list_iterator<T, Pointer, Reference>::this_type
	intrusive_list_iterator<T, Pointer, Reference>::operator++(int)
	{
		intrusive_list_iterator it(*this);
		mpNode = toInternalNodeType(mpNode->mpNext);
		return it;
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename intrusive_list_iterator<T, Pointer, Reference>::this_type&
	intrusive_list_iterator<T, Pointer, Reference>::operator--()
	{
		mpNode = toInternalNodeType(mpNode->mpPrev);
		return *this;
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename intrusive_list_iterator<T, Pointer, Reference>::this_type
	intrusive_list_iterator<T, Pointer, Reference>::operator--(int)
	{
		intrusive_list_iterator it(*this);
		mpNode = toInternalNodeType(mpNode->mpPrev);
		return it;
	}

	///////////////////////////////////////////////////////////////////////
	// intrusive_list_base
	///////////////////////////////////////////////////////////////////////

	inline intrusive_list_base::intrusive_list_base() 
	{
		mAnchor.mpNext = mAnchor.mpPrev = &mAnchor;
	}

	inline intrusive_list_base::~intrusive_list_base()
	{
		#if EASTL_VALIDATE_INTRUSIVE_LIST
			clear();
			mAnchor.mpNext = mAnchor.mpPrev = nullptr;
		#endif
	}


	inline bool intrusive_list_base::empty() const EA_NOEXCEPT
	{
		return mAnchor.mpPrev == &mAnchor;
	}


	inline intrusive_list_base::size_type intrusive_list_base::size() const EA_NOEXCEPT
	{
		const intrusive_list_node* p = &mAnchor;
		size_type n = (size_type)-1;

		do {
			++n;
			p = p->mpNext;
		} while(p != &mAnchor);

		return n;
	}


	inline void intrusive_list_base::clear() EA_NOEXCEPT
	{
		#if EASTL_VALIDATE_INTRUSIVE_LIST
			// Need to clear out all the next/prev pointers in the elements;
			// this makes this operation O(n) instead of O(1).
			intrusive_list_node* pNode = mAnchor.mpNext;

			while(pNode != &mAnchor)
			{
				intrusive_list_node* const pNextNode = pNode->mpNext;
				pNode->mpNext = pNode->mpPrev = nullptr;
				pNode = pNextNode;
			}
		#endif

		mAnchor.mpNext = mAnchor.mpPrev = &mAnchor;
	}


	inline void intrusive_list_base::pop_front()
	{
		#if EASTL_VALIDATE_INTRUSIVE_LIST
			intrusive_list_node* const pNode = mAnchor.mpNext;
		#endif

		mAnchor.mpNext->mpNext->mpPrev = &mAnchor;
		mAnchor.mpNext = mAnchor.mpNext->mpNext;

		#if EASTL_VALIDATE_INTRUSIVE_LIST
			if(pNode != &mAnchor)
				pNode->mpNext = pNode->mpPrev = nullptr;
			#if EASTL_ASSERT_ENABLED
			else
				EASTL_FAIL_MSG("intrusive_list::pop_front(): empty list.");
			#endif
		#endif
	}


	inline void intrusive_list_base::pop_back()
	{
		#if EASTL_VALIDATE_INTRUSIVE_LIST
			intrusive_list_node* const pNode = mAnchor.mpPrev;
		#endif

		mAnchor.mpPrev->mpPrev->mpNext = &mAnchor;
		mAnchor.mpPrev = mAnchor.mpPrev->mpPrev;

		#if EASTL_VALIDATE_INTRUSIVE_LIST
			if(pNode != &mAnchor)
				pNode->mpNext = pNode->mpPrev = nullptr;
			#if EASTL_ASSERT_ENABLED
			else
				EASTL_FAIL_MSG("intrusive_list::pop_back(): empty list.");
			#endif
		#endif
	}




	///////////////////////////////////////////////////////////////////////
	// intrusive_list
	///////////////////////////////////////////////////////////////////////

	template <typename T>
	inline intrusive_list<T>::intrusive_list()
	{
	}


	template <typename T>
	inline intrusive_list<T>::intrusive_list(const this_type& /*x*/)
	  : intrusive_list_base()
	{
		// We intentionally ignore argument x.
		// To consider: Shouldn't this function simply not exist? Is there a useful purpose for having this function?
		// There should be a comment here about it, though my first guess is that this exists to quell VC++ level 4/-Wall compiler warnings.
	}


	template <typename T>
	inline typename intrusive_list<T>::this_type& intrusive_list<T>::operator=(const this_type& /*x*/)
	{ 
		// We intentionally ignore argument x.
		// See notes above in the copy constructor about questioning the existence of this function.
		return *this;
	}


	template <typename T>
	inline typename intrusive_list<T>::iterator intrusive_list<T>::begin() EA_NOEXCEPT
	{
		return iterator(mAnchor.mpNext);
	}


	template <typename T>
	inline typename intrusive_list<T>::const_iterator intrusive_list<T>::begin() const EA_NOEXCEPT
	{
		return const_iterator(mAnchor.mpNext);
	}


	template <typename T>
	inline typename intrusive_list<T>::const_iterator intrusive_list<T>::cbegin() const EA_NOEXCEPT
	{
		return const_iterator(mAnchor.mpNext);
	}


	template <typename T>
	inline typename intrusive_list<T>::iterator intrusive_list<T>::end() EA_NOEXCEPT
	{
		return iterator(&mAnchor);
	}


	template <typename T>
	inline typename intrusive_list<T>::const_iterator intrusive_list<T>::end() const EA_NOEXCEPT
	{
		return const_iterator(&mAnchor);
	}


	template <typename T>
	inline typename intrusive_list<T>::const_iterator intrusive_list<T>::cend() const EA_NOEXCEPT
	{
		return const_iterator(&mAnchor);
	}


	template <typename T>
	inline typename intrusive_list<T>::reverse_iterator intrusive_list<T>::rbegin() EA_NOEXCEPT
	{
		return reverse_iterator(iterator(&mAnchor));
	}


	template <typename T>
	inline typename intrusive_list<T>::const_reverse_iterator intrusive_list<T>::rbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(const_iterator(&mAnchor));
	}


	template <typename T>
	inline typename intrusive_list<T>::const_reverse_iterator intrusive_list<T>::crbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(const_iterator(&mAnchor));
	}


	template <typename T>
	inline typename intrusive_list<T>::reverse_iterator intrusive_list<T>::rend() EA_NOEXCEPT
	{
		return reverse_iterator(iterator(mAnchor.mpNext));
	}


	template <typename T>
	inline typename intrusive_list<T>::const_reverse_iterator intrusive_list<T>::rend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(const_iterator(mAnchor.mpNext));
	}


	template <typename T>
	inline typename intrusive_list<T>::const_reverse_iterator intrusive_list<T>::crend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(const_iterator(mAnchor.mpNext));
	}


	template <typename T>
	inline typename intrusive_list<T>::reference intrusive_list<T>::front()
	{
		#if EASTL_VALIDATE_INTRUSIVE_LIST && EASTL_ASSERT_ENABLED
			if(mAnchor.mpNext == &mAnchor)
				EASTL_FAIL_MSG("intrusive_list::front(): empty list.");
		#endif

		return *static_cast<T*>(mAnchor.mpNext);
	}


	template <typename T>
	inline typename intrusive_list<T>::const_reference intrusive_list<T>::front() const
	{
		#if EASTL_VALIDATE_INTRUSIVE_LIST && EASTL_ASSERT_ENABLED
			if(mAnchor.mpNext == &mAnchor)
				EASTL_FAIL_MSG("intrusive_list::front(): empty list.");
		#endif

		return *static_cast<const T*>(mAnchor.mpNext);
	}


	template <typename T>
	inline typename intrusive_list<T>::reference intrusive_list<T>::back()
	{
		#if EASTL_VALIDATE_INTRUSIVE_LIST && EASTL_ASSERT_ENABLED
			if(mAnchor.mpNext == &mAnchor)
				EASTL_FAIL_MSG("intrusive_list::back(): empty list.");
		#endif

		return *static_cast<T*>(mAnchor.mpPrev);
	}


	template <typename T>
	inline typename intrusive_list<T>::const_reference intrusive_list<T>::back() const
	{
		#if EASTL_VALIDATE_INTRUSIVE_LIST && EASTL_ASSERT_ENABLED
			if(mAnchor.mpNext == &mAnchor)
				EASTL_FAIL_MSG("intrusive_list::back(): empty list.");
		#endif

		return *static_cast<const T*>(mAnchor.mpPrev);
	}


	template <typename T>
	inline void intrusive_list<T>::push_front(value_type& x)
	{
		#if EASTL_VALIDATE_INTRUSIVE_LIST && EASTL_ASSERT_ENABLED
			if(x.mpNext || x.mpPrev)
				EASTL_FAIL_MSG("intrusive_list::push_front(): element already on a list.");
		#endif

		x.mpNext = mAnchor.mpNext;
		x.mpPrev = &mAnchor;
		mAnchor.mpNext = &x;
		x.mpNext->mpPrev = &x;
	}


	template <typename T>
	inline void intrusive_list<T>::push_back(value_type& x)
	{
		#if EASTL_VALIDATE_INTRUSIVE_LIST && EASTL_ASSERT_ENABLED
			if(x.mpNext || x.mpPrev)
				EASTL_FAIL_MSG("intrusive_list::push_back(): element already on a list.");
		#endif

		x.mpPrev = mAnchor.mpPrev;
		x.mpNext = &mAnchor;
		mAnchor.mpPrev = &x;
		x.mpPrev->mpNext = &x;
	}


	template <typename T>
	inline bool intrusive_list<T>::contains(const value_type& x) const
	{
		for(const intrusive_list_node* p = mAnchor.mpNext; p != &mAnchor; p = p->mpNext)
		{
			if(p == &x)
				return true;
		}

		return false;
	}


	template <typename T>
	inline typename intrusive_list<T>::iterator intrusive_list<T>::locate(value_type& x)
	{
		for(intrusive_list_node* p = (T*)mAnchor.mpNext; p != &mAnchor; p = p->mpNext)
		{
			if(p == &x)
				return iterator(p);
		}

		return iterator(&mAnchor);
	}


	template <typename T>
	inline typename intrusive_list<T>::const_iterator intrusive_list<T>::locate(const value_type& x) const
	{
		for(const intrusive_list_node* p = mAnchor.mpNext; p != &mAnchor; p = p->mpNext)
		{
			if(p == &x)
				return const_iterator(p);
		}

		return const_iterator(&mAnchor);
	}


	template <typename T>
	inline typename intrusive_list<T>::iterator intrusive_list<T>::insert(const_iterator pos, value_type& x)
	{
		#if EASTL_VALIDATE_INTRUSIVE_LIST && EASTL_ASSERT_ENABLED
			if(x.mpNext || x.mpPrev)
				EASTL_FAIL_MSG("intrusive_list::insert(): element already on a list.");
		#endif

		intrusive_list_node& next = *toListNode(pos.mpNode);
		intrusive_list_node& prev = *next.mpPrev;

		prev.mpNext = next.mpPrev = &x;
		x.mpPrev    = &prev;
		x.mpNext    = &next;

		return iterator(&x);
	}


	template <typename T>
	inline typename intrusive_list<T>::iterator
	intrusive_list<T>::erase(const_iterator pos)
	{
		intrusive_list_node& prev = *pos.mpNode->mpPrev;
		intrusive_list_node& next = *pos.mpNode->mpNext;
		prev.mpNext = &next;
		next.mpPrev = &prev;

		#if EASTL_VALIDATE_INTRUSIVE_LIST
			iterator ii(pos.mpNode);
			ii.mpNode->mpPrev = ii.mpNode->mpNext = nullptr;
		#endif

		return iterator(&next);
	}


	template <typename T>
	inline typename intrusive_list<T>::iterator
	intrusive_list<T>::erase(const_iterator first, const_iterator last)
	{
		intrusive_list_node& prev = *(first.mpNode->mpPrev);
		intrusive_list_node& next = *toListNode(last.mpNode);

		#if EASTL_VALIDATE_INTRUSIVE_LIST
			// need to clear out all the next/prev pointers in the elements;
			// this makes this operation O(n) instead of O(1), sadly, although
			// it's technically amortized O(1) since you could count yourself
			// as paying this cost with each insert.
			intrusive_list_node* pCur = toListNode(first.mpNode);

			while(pCur != &next)
			{
				intrusive_list_node* const pCurNext = pCur->mpNext;
				pCur->mpPrev = pCur->mpNext = nullptr;
				pCur = pCurNext;
			}
		#endif

		prev.mpNext = &next;
		next.mpPrev = &prev;

		return iterator(last.mpNode);
	}


	template <typename T>
	inline typename intrusive_list<T>::reverse_iterator
	intrusive_list<T>::erase(const_reverse_iterator position)
	{
		return reverse_iterator(erase((++position).base()));
	}


	template <typename T>
	inline typename intrusive_list<T>::reverse_iterator
	intrusive_list<T>::erase(const_reverse_iterator first, const_reverse_iterator last)
	{
		// Version which erases in order from first to last.
		// difference_type i(first.base() - last.base());
		// while(i--)
		//     first = erase(first);
		// return first;

		// Version which erases in order from last to first, but is slightly more efficient:
		return reverse_iterator(erase((++last).base(), (++first).base()));
	}


	template <typename T>
	void intrusive_list<T>::swap(intrusive_list& x)
	{
		// swap anchors
		intrusive_list_node temp(mAnchor);
		mAnchor   = x.mAnchor;
		x.mAnchor = temp;

		// Fixup node pointers into the anchor, since the addresses of 
		// the anchors must stay the same with each list.
		if(mAnchor.mpNext == &x.mAnchor)
			mAnchor.mpNext = mAnchor.mpPrev = &mAnchor;
		else
			mAnchor.mpNext->mpPrev = mAnchor.mpPrev->mpNext = &mAnchor;

		if(x.mAnchor.mpNext == &mAnchor)
			x.mAnchor.mpNext = x.mAnchor.mpPrev = &x.mAnchor;
		else
			x.mAnchor.mpNext->mpPrev = x.mAnchor.mpPrev->mpNext = &x.mAnchor;

		#if EASTL_VALIDATE_INTRUSIVE_LIST
			temp.mpPrev = temp.mpNext = nullptr;
		#endif
	}


	template <typename T>
	void intrusive_list<T>::splice(const_iterator pos, value_type& value)
	{
		// Note that splice(pos, x, pos) and splice(pos+1, x, pos)
		// are valid and need to be handled correctly.

		if(pos.mpNode != &value)
		{
			// Unlink item from old list.
			intrusive_list_node& oldNext = *value.mpNext;
			intrusive_list_node& oldPrev = *value.mpPrev;
			oldNext.mpPrev = &oldPrev;
			oldPrev.mpNext = &oldNext;

			// Relink item into new list.
			intrusive_list_node& newNext = *toListNode(pos.mpNode);
			intrusive_list_node& newPrev = *newNext.mpPrev;

			newPrev.mpNext = &value;
			newNext.mpPrev = &value;
			value.mpPrev = &newPrev;
			value.mpNext = &newNext;
		}
	}


	template <typename T>
	void intrusive_list<T>::splice(const_iterator pos, intrusive_list& x)
	{
		// Note: &x == this is prohibited, so self-insertion is not a problem.
		if(x.mAnchor.mpNext != &x.mAnchor) // If the list 'x' isn't empty...
		{
			intrusive_list_node& next       = *toListNode(pos.mpNode);
			intrusive_list_node& prev       = *next.mpPrev;
			intrusive_list_node& insertPrev = *x.mAnchor.mpNext;
			intrusive_list_node& insertNext = *x.mAnchor.mpPrev;

			prev.mpNext       = &insertPrev;
			insertPrev.mpPrev = &prev;
			insertNext.mpNext = &next;
			next.mpPrev       = &insertNext;
			x.mAnchor.mpPrev  = x.mAnchor.mpNext = &x.mAnchor;
		}
	}


	template <typename T>
	void intrusive_list<T>::splice(const_iterator pos, intrusive_list& /*x*/, const_iterator i)
	{
		// Note: &x == this is prohibited, so self-insertion is not a problem.

		// Note that splice(pos, x, pos) and splice(pos + 1, x, pos)
		// are valid and need to be handled correctly.

		// We don't need to check if the source list is empty, because 
		// this function expects a valid iterator from the source list,
		// and thus the list cannot be empty in such a situation.

		iterator ii(i.mpNode); // Make a temporary non-const version.

		if(pos != ii)
		{
			// Unlink item from old list.
			intrusive_list_node& oldNext = *ii.mpNode->mpNext;
			intrusive_list_node& oldPrev = *ii.mpNode->mpPrev;
			oldNext.mpPrev = &oldPrev;
			oldPrev.mpNext = &oldNext;

			// Relink item into new list.
			intrusive_list_node& newNext = *toListNode(pos.mpNode);
			intrusive_list_node& newPrev = *newNext.mpPrev;

			newPrev.mpNext = ii.mpNode;
			newNext.mpPrev = ii.mpNode;
			ii.mpNode->mpPrev = &newPrev;
			ii.mpNode->mpNext = &newNext;
		}
	}


	template <typename T>
	void intrusive_list<T>::splice(const_iterator pos, intrusive_list& /*x*/, const_iterator first, const_iterator last)
	{
		// Note: &x == this is prohibited, so self-insertion is not a problem.
		if(first != last)
		{
			intrusive_list_node& insertPrev = *toListNode(first.mpNode);
			intrusive_list_node& insertNext = *last.mpNode->mpPrev;

			// remove from old list
			insertNext.mpNext->mpPrev = insertPrev.mpPrev;
			insertPrev.mpPrev->mpNext = insertNext.mpNext;

			// insert into this list
			intrusive_list_node& next = *toListNode(pos.mpNode);
			intrusive_list_node& prev = *next.mpPrev;

			prev.mpNext       = &insertPrev;
			insertPrev.mpPrev = &prev;
			insertNext.mpNext = &next;
			next.mpPrev       = &insertNext;
		}
	}


	template <typename T>
	inline void intrusive_list<T>::remove(value_type& value)
	{
		intrusive_list_node& prev = *value.mpPrev;
		intrusive_list_node& next = *value.mpNext;
		prev.mpNext = &next;
		next.mpPrev = &prev;

		#if EASTL_VALIDATE_INTRUSIVE_LIST
			value.mpPrev = value.mpNext = nullptr;
		#endif
	}


	template <typename T>
	void intrusive_list<T>::merge(this_type& x)
	{
		if(this != &x)
		{
			iterator       first(begin());
			iterator       firstX(x.begin());
			const iterator last(end());
			const iterator lastX(x.end());

			while((first != last) && (firstX != lastX))
			{
				if(*firstX < *first)
				{
					iterator next(firstX);

					splice(first, x, firstX, ++next);
					firstX = next;
				}
				else
					++first;
			}

			if(firstX != lastX)
				splice(last, x, firstX, lastX);
		}
	}


	template <typename T>
	template <typename Compare>
	void intrusive_list<T>::merge(this_type& x, Compare compare)
	{
		if(this != &x)
		{
			iterator       first(begin());
			iterator       firstX(x.begin());
			const iterator last(end());
			const iterator lastX(x.end());

			while((first != last) && (firstX != lastX))
			{
				if(compare(*firstX, *first))
				{
					iterator next(firstX);

					splice(first, x, firstX, ++next);
					firstX = next;
				}
				else
					++first;
			}

			if(firstX != lastX)
				splice(last, x, firstX, lastX);
		}
	}


	template <typename T>
	void intrusive_list<T>::unique()
	{
		iterator       first(begin());
		const iterator last(end());

		if(first != last)
		{
			iterator next(first);

			while(++next != last)
			{
				if(*first == *next)
					erase(next);
				else
					first = next;
				next = first;
			}
		}
	}


	template <typename T>
	template <typename BinaryPredicate>
	void intrusive_list<T>::unique(BinaryPredicate predicate)
	{
		iterator       first(begin());
		const iterator last(end());

		if(first != last)
		{
			iterator next(first);

			while(++next != last)
			{
				if(predicate(*first, *next))
					erase(next);
				else
					first = next;
				next = first;
			}
		}
	}


	template <typename T>
	void intrusive_list<T>::sort()
	{
		// We implement the algorithm employed by Chris Caulfield whereby we use recursive
		// function calls to sort the list. The sorting of a very large list may fail due to stack overflow
		// if the stack is exhausted. The limit depends on the platform and the avaialble stack space.

		// Easier-to-understand version of the 'if' statement:
		// iterator i(begin());
		// if((i != end()) && (++i != end())) // If the size is >= 2 (without calling the more expensive size() function)...

		// Faster, more inlinable version of the 'if' statement:
		if ((mAnchor.mpNext != &mAnchor) && (mAnchor.mpNext != mAnchor.mpPrev))
		{
			// Split the array into 2 roughly equal halves.
			this_type leftList;     // This should cause no memory allocation.
			this_type rightList;

			// We find an iterator which is in the middle of the list. The fastest way to do 
			// this is to iterate from the base node both forwards and backwards with two 
			// iterators and stop when they meet each other. Recall that our size() function 
			// is not O(1) but is instead O(n), at least when EASTL_LIST_SIZE_CACHE is disabled.
			#if EASTL_LIST_SIZE_CACHE
				iterator mid(begin());
				eastl::advance(mid, size() / 2);
			#else
				iterator mid(begin()), tail(end());

				while((mid != tail) && (++mid != tail))
					--tail;
			#endif

			// Move the left half of this into leftList and the right half into rightList.
			leftList.splice(leftList.begin(), *this, begin(), mid);
			rightList.splice(rightList.begin(), *this);

			// Sort the sub-lists.
			leftList.sort();
			rightList.sort();

			// Merge the two halves into this list.
			splice(begin(), leftList);
			merge(rightList);
		}
	}


	template <typename T>
	template<typename Compare>
	void intrusive_list<T>::sort(Compare compare)
	{
		// We implement the algorithm employed by Chris Caulfield whereby we use recursive
		// function calls to sort the list. The sorting of a very large list may fail due to stack overflow
		// if the stack is exhausted. The limit depends on the platform and the avaialble stack space.

		// Easier-to-understand version of the 'if' statement:
		// iterator i(begin());
		// if((i != end()) && (++i != end())) // If the size is >= 2 (without calling the more expensive size() function)...

		// Faster, more inlinable version of the 'if' statement:
		if ((mAnchor.mpNext != &mAnchor) && (mAnchor.mpNext != mAnchor.mpPrev))
		{
			// Split the array into 2 roughly equal halves.
			this_type leftList;     // This should cause no memory allocation.
			this_type rightList;

			// We find an iterator which is in the middle of the list. The fastest way to do 
			// this is to iterate from the base node both forwards and backwards with two 
			// iterators and stop when they meet each other. Recall that our size() function 
			// is not O(1) but is instead O(n), at least when EASTL_LIST_SIZE_CACHE is disabled.
			#if EASTL_LIST_SIZE_CACHE
				iterator mid(begin());
				eastl::advance(mid, size() / 2);
			#else
				iterator mid(begin()), tail(end());

				while((mid != tail) && (++mid != tail))
					--tail;
			#endif

			// Move the left half of this into leftList and the right half into rightList.
			leftList.splice(leftList.begin(), *this, begin(), mid);
			rightList.splice(rightList.begin(), *this);

			// Sort the sub-lists.
			leftList.sort(compare);
			rightList.sort(compare);

			// Merge the two halves into this list.
			splice(begin(), leftList);
			merge(rightList, compare);
		}
	}


	template <typename T>
	inline int intrusive_list<T>::validate_iterator(const_iterator i) const
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
	bool operator==(const intrusive_list<T>& a, const intrusive_list<T>& b)
	{
		// If we store an mSize member for intrusive_list, we want to take advantage of it here.
		typename intrusive_list<T>::const_iterator ia   = a.begin();
		typename intrusive_list<T>::const_iterator ib   = b.begin();
		typename intrusive_list<T>::const_iterator enda = a.end();
		typename intrusive_list<T>::const_iterator endb = b.end();

		while((ia != enda) && (ib != endb) && (*ia == *ib))
		{
			++ia;
			++ib;
		}
		return (ia == enda) && (ib == endb);
	}

	template <typename T>
	bool operator!=(const intrusive_list<T>& a, const intrusive_list<T>& b)
	{
		return !(a == b);
	}

	template <typename T>
	bool operator<(const intrusive_list<T>& a, const intrusive_list<T>& b)
	{
		return eastl::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
	}

	template <typename T>
	bool operator>(const intrusive_list<T>& a, const intrusive_list<T>& b)
	{
		return b < a;
	}

	template <typename T>
	bool operator<=(const intrusive_list<T>& a, const intrusive_list<T>& b)
	{
		return !(b < a);
	}

	template <typename T>
	bool operator>=(const intrusive_list<T>& a, const intrusive_list<T>& b)
	{
		return !(a < b);
	}

	template <typename T>
	void swap(intrusive_list<T>& a, intrusive_list<T>& b)
	{
		a.swap(b);
	}


} // namespace eastl


#endif // Header include guard















