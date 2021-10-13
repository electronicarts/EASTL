///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements a doubly-linked list, much like the C++ std::list class.
// The primary distinctions between this list and std::list are:
//    - list doesn't implement some of the less-frequently used functions 
//      of std::list. Any required functions can be added at a later time.
//    - list has a couple extension functions that increase performance.
//    - list can contain objects with alignment requirements. std::list cannot
//      do so without a bit of tedious non-portable effort.
//    - list has optimizations that don't exist in the STL implementations 
//      supplied by library vendors for our targeted platforms.
//    - list supports debug memory naming natively.
//    - list::size() by default is not a constant time function, like the list::size 
//      in some std implementations such as STLPort and SGI STL but unlike the 
//      list in Dinkumware and Metrowerks. The EASTL_LIST_SIZE_CACHE option can change this.
//    - list provides a guaranteed portable node definition that allows users
//      to write custom fixed size node allocators that are portable.
//    - list is easier to read, debug, and visualize.
//    - list is savvy to an environment that doesn't have exception handling,
//      as is sometimes the case with console or embedded environments.
//    - list has less deeply nested function calls and allows the user to 
//      enable forced inlining in debug builds in order to reduce bloat.
//    - list doesn't keep a member size variable. This means that list is 
//      smaller than std::list (depends on std::list) and that for most operations
//      it is faster than std::list. However, the list::size function is slower.
//    - list::size_type is defined as eastl_size_t instead of size_t in order to 
//      save memory and run faster on 64 bit systems.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_LIST_H
#define EASTL_LIST_H


#include <EASTL/internal/config.h>
#include <EASTL/allocator.h>
#include <EASTL/type_traits.h>
#include <EASTL/iterator.h>
#include <EASTL/algorithm.h>
#include <EASTL/initializer_list.h>
#include <EASTL/bonus/compressed_pair.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <new>
#include <stddef.h>
EA_RESTORE_ALL_VC_WARNINGS()


// 4530 - C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
// 4345 - Behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized
// 4571 - catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught.
// 4623 - default constructor was implicitly defined as deleted
EA_DISABLE_VC_WARNING(4530 4345 4571 4623);


#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// EASTL_LIST_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_LIST_DEFAULT_NAME
		#define EASTL_LIST_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " list" // Unless the user overrides something, this is "EASTL list".
	#endif


	/// EASTL_LIST_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_LIST_DEFAULT_ALLOCATOR
		#define EASTL_LIST_DEFAULT_ALLOCATOR allocator_type(EASTL_LIST_DEFAULT_NAME)
	#endif



	/// ListNodeBase
	///
	/// We define a ListNodeBase separately from ListNode (below), because it allows
	/// us to have non-templated operations such as insert, remove (below), and it 
	/// makes it so that the list anchor node doesn't carry a T with it, which would
	/// waste space and possibly lead to surprising the user due to extra Ts existing
	/// that the user didn't explicitly create. The downside to all of this is that 
	/// it makes debug viewing of a list harder, given that the node pointers are of 
	/// type ListNodeBase and not ListNode. However, see ListNodeBaseProxy below.
	///
	struct ListNodeBase
	{
		ListNodeBase* mpNext;
		ListNodeBase* mpPrev;

		void        insert(ListNodeBase* pNext) EA_NOEXCEPT;                                // Inserts this standalone node before the node pNext in pNext's list. 
		void        remove() EA_NOEXCEPT;                                                   // Removes this node from the list it's in. Leaves this node's mpNext/mpPrev invalid.
		void        splice(ListNodeBase* pFirst, ListNodeBase* pLast) EA_NOEXCEPT;          // Removes [pFirst,pLast) from the list it's in and inserts it before this in this node's list.
		void        reverse() EA_NOEXCEPT;                                                  // Reverses the order of nodes in the circular list this node is a part of.
		static void swap(ListNodeBase& a, ListNodeBase& b) EA_NOEXCEPT;                     // Swaps the nodes a and b in the lists to which they belong.

		void        insert_range(ListNodeBase* pFirst, ListNodeBase* pFinal) EA_NOEXCEPT;   // Differs from splice in that first/final aren't in another list.
		static void remove_range(ListNodeBase* pFirst, ListNodeBase* pFinal) EA_NOEXCEPT;   // 
	} EASTL_LIST_PROXY_MAY_ALIAS;


	#if EASTL_LIST_PROXY_ENABLED

		/// ListNodeBaseProxy
		///
		/// In debug builds, we define ListNodeBaseProxy to be the same thing as
		/// ListNodeBase, except it is templated on the parent ListNode class.
		/// We do this because we want users in debug builds to be able to easily
		/// view the list's contents in a debugger GUI. We do this only in a debug
		/// build for the reasons described above: that ListNodeBase needs to be
		/// as efficient as possible and not cause code bloat or extra function 
		/// calls (inlined or not).
		///
		/// ListNodeBaseProxy *must* be separate from its parent class ListNode 
		/// because the list class must have a member node which contains no T value.
		/// It is thus incorrect for us to have one single ListNode class which
		/// has mpNext, mpPrev, and mValue. So we do a recursive template trick in 
		/// the definition and use of SListNodeBaseProxy.
		///
		template <typename LN>
		struct ListNodeBaseProxy
		{
			LN* mpNext;
			LN* mpPrev;
		};

		template <typename T>
		struct ListNode : public ListNodeBaseProxy< ListNode<T> >
		{
			T mValue;
		};

	#else

		EA_DISABLE_VC_WARNING(4625 4626)
		template <typename T>
		struct ListNode : public ListNodeBase
		{
			T mValue;
		};
		EA_RESTORE_VC_WARNING()

	#endif




	/// ListIterator
	///
	template <typename T, typename Pointer, typename Reference>
	struct ListIterator
	{
		typedef ListIterator<T, Pointer, Reference>         this_type;
		typedef ListIterator<T, T*, T&>                     iterator;
		typedef ListIterator<T, const T*, const T&>         const_iterator;
		typedef eastl_size_t                                size_type;     // See config.h for the definition of eastl_size_t, which defaults to size_t.
		typedef ptrdiff_t                                   difference_type;
		typedef T                                           value_type;
		typedef ListNode<T>                                 node_type;
		typedef Pointer                                     pointer;
		typedef Reference                                   reference;
		typedef EASTL_ITC_NS::bidirectional_iterator_tag    iterator_category;

	public:
		node_type* mpNode;

	public:
		ListIterator() EA_NOEXCEPT;
		ListIterator(const ListNodeBase* pNode) EA_NOEXCEPT;
		ListIterator(const iterator& x) EA_NOEXCEPT;

		this_type next() const EA_NOEXCEPT;
		this_type prev() const EA_NOEXCEPT;

		reference operator*() const EA_NOEXCEPT;
		pointer   operator->() const EA_NOEXCEPT;

		this_type& operator++() EA_NOEXCEPT;
		this_type  operator++(int) EA_NOEXCEPT;

		this_type& operator--() EA_NOEXCEPT;
		this_type  operator--(int) EA_NOEXCEPT;

	}; // ListIterator




	/// ListBase
	///
	/// See VectorBase (class vector) for an explanation of why we 
	/// create this separate base class.
	///
	template <typename T, typename Allocator>
	class ListBase
	{
	public:
		typedef T                                    value_type;
		typedef Allocator                            allocator_type;
		typedef ListNode<T>                          node_type;
		typedef eastl_size_t                         size_type;     // See config.h for the definition of eastl_size_t, which defaults to size_t.
		typedef ptrdiff_t                            difference_type;
		#if EASTL_LIST_PROXY_ENABLED
			typedef ListNodeBaseProxy< ListNode<T> > base_node_type;
		#else
			typedef ListNodeBase                     base_node_type; // We use ListNodeBase instead of ListNode<T> because we don't want to create a T.
		#endif

	protected:
		eastl::compressed_pair<base_node_type, allocator_type>  mNodeAllocator;
		#if EASTL_LIST_SIZE_CACHE
			size_type  mSize;
		#endif

		base_node_type& internalNode() EA_NOEXCEPT { return mNodeAllocator.first(); }
		base_node_type const& internalNode() const EA_NOEXCEPT { return mNodeAllocator.first(); }
		allocator_type& internalAllocator() EA_NOEXCEPT { return mNodeAllocator.second(); }
		const allocator_type& internalAllocator() const EA_NOEXCEPT { return mNodeAllocator.second(); }

	public:
		const allocator_type& get_allocator() const EA_NOEXCEPT;
		allocator_type&       get_allocator() EA_NOEXCEPT;
		void                  set_allocator(const allocator_type& allocator);

	protected:
		ListBase();
		ListBase(const allocator_type& a);
	   ~ListBase();

		node_type* DoAllocateNode();
		void       DoFreeNode(node_type* pNode);

		void DoInit() EA_NOEXCEPT;
		void DoClear();

	}; // ListBase




	/// list
	///
	/// -- size() is O(n) --
	/// Note that as of this writing, list::size() is an O(n) operation when EASTL_LIST_SIZE_CACHE is disabled. 
	/// That is, getting the size of the list is not a fast operation, as it requires traversing the list and 
	/// counting the nodes. We could make list::size() be fast by having a member mSize variable. There are reasons 
	/// for having such functionality and reasons for not having such functionality. We currently choose
	/// to not have a member mSize variable as it would add four bytes to the class, add a tiny amount
	/// of processing to functions such as insert and erase, and would only serve to improve the size
	/// function, but no others. The alternative argument is that the C++ standard states that std::list
	/// should be an O(1) operation (i.e. have a member size variable), most C++ standard library list
	/// implementations do so, the size is but an integer which is quick to update, and many users 
	/// expect to have a fast size function. The EASTL_LIST_SIZE_CACHE option changes this.
	/// To consider: Make size caching an optional template parameter.
	///
	/// Pool allocation
	/// If you want to make a custom memory pool for a list container, your pool 
	/// needs to contain items of type list::node_type. So if you have a memory
	/// pool that has a constructor that takes the size of pool items and the
	/// count of pool items, you would do this (assuming that MemoryPool implements
	/// the Allocator interface):
	///     typedef list<Widget, MemoryPool> WidgetList;           // Delare your WidgetList type.
	///     MemoryPool myPool(sizeof(WidgetList::node_type), 100); // Make a pool of 100 Widget nodes.
	///     WidgetList myList(&myPool);                            // Create a list that uses the pool.
	///
	template <typename T, typename Allocator = EASTLAllocatorType>
	class list : public ListBase<T, Allocator>
	{
		typedef ListBase<T, Allocator>                  base_type;
		typedef list<T, Allocator>                      this_type;

	public:
		typedef T                                       value_type;
		typedef T*                                      pointer;
		typedef const T*                                const_pointer;
		typedef T&                                      reference;
		typedef const T&                                const_reference;
		typedef ListIterator<T, T*, T&>                 iterator;
		typedef ListIterator<T, const T*, const T&>     const_iterator;
		typedef eastl::reverse_iterator<iterator>       reverse_iterator;
		typedef eastl::reverse_iterator<const_iterator> const_reverse_iterator;
		typedef typename base_type::size_type           size_type;
		typedef typename base_type::difference_type     difference_type;
		typedef typename base_type::allocator_type      allocator_type;
		typedef typename base_type::node_type           node_type;
		typedef typename base_type::base_node_type      base_node_type;

		using base_type::mNodeAllocator;
		using base_type::DoAllocateNode;
		using base_type::DoFreeNode;
		using base_type::DoClear;
		using base_type::DoInit;
		using base_type::get_allocator;
		#if EASTL_LIST_SIZE_CACHE
			using base_type::mSize;
		#endif
		using base_type::internalNode;
		using base_type::internalAllocator;

	public:
		list();
		list(const allocator_type& allocator);
		explicit list(size_type n, const allocator_type& allocator = EASTL_LIST_DEFAULT_ALLOCATOR);
		list(size_type n, const value_type& value, const allocator_type& allocator = EASTL_LIST_DEFAULT_ALLOCATOR);
		list(const this_type& x);
		list(const this_type& x, const allocator_type& allocator);
		list(this_type&& x);
		list(this_type&&, const allocator_type&);
		list(std::initializer_list<value_type> ilist, const allocator_type& allocator = EASTL_LIST_DEFAULT_ALLOCATOR);

		template <typename InputIterator>
		list(InputIterator first, InputIterator last); // allocator arg removed because VC7.1 fails on the default arg. To do: Make a second version of this function without a default arg.

		this_type& operator=(const this_type& x);
		this_type& operator=(std::initializer_list<value_type> ilist);
		this_type& operator=(this_type&& x);

		// In the case that the two containers' allocators are unequal, swap copies elements instead
		// of replacing them in place. In this case swap is an O(n) operation instead of O(1).
		void swap(this_type& x);

		void assign(size_type n, const value_type& value);

		template <typename InputIterator>                       // It turns out that the C++ std::list specifies a two argument
		void assign(InputIterator first, InputIterator last);   // version of assign that takes (int size, int value). These are not 
																// iterators, so we need to do a template compiler trick to do the right thing.
		void assign(std::initializer_list<value_type> ilist);

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

		bool      empty() const EA_NOEXCEPT;
		size_type size() const EA_NOEXCEPT;

		void resize(size_type n, const value_type& value);
		void resize(size_type n);

		reference       front();
		const_reference front() const;

		reference       back();
		const_reference back() const;

		template <typename... Args>
		void emplace_front(Args&&... args);

		template <typename... Args>
		void emplace_back(Args&&... args);

		void      push_front(const value_type& value);
		void      push_front(value_type&& x);
		reference push_front();
		void*     push_front_uninitialized();

		void      push_back(const value_type& value);
		void      push_back(value_type&& x);
		reference push_back();
		void*     push_back_uninitialized();

		void pop_front();
		void pop_back();

		template <typename... Args>
		iterator emplace(const_iterator position, Args&&... args);

		iterator insert(const_iterator position);
		iterator insert(const_iterator position, const value_type& value);
		iterator insert(const_iterator position, value_type&& x);
		iterator insert(const_iterator position, std::initializer_list<value_type> ilist);
		iterator insert(const_iterator position, size_type n, const value_type& value);

		template <typename InputIterator>
		iterator insert(const_iterator position, InputIterator first, InputIterator last);

		iterator erase(const_iterator position);
		iterator erase(const_iterator first, const_iterator last);

		reverse_iterator erase(const_reverse_iterator position);
		reverse_iterator erase(const_reverse_iterator first, const_reverse_iterator last);

		void clear() EA_NOEXCEPT;
		void reset_lose_memory() EA_NOEXCEPT;    // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.

		void remove(const T& x);

		template <typename Predicate>
		void remove_if(Predicate);

		void reverse() EA_NOEXCEPT;

		// splice inserts elements in the range [first,last) before position and removes the elements from x.
		// In the case that the two containers' allocators are unequal, splice copies elements 
		// instead of splicing them. In this case elements are not removed from x, and iterators 
		// into the spliced elements from x continue to point to the original values in x.
		void splice(const_iterator position, this_type& x);
		void splice(const_iterator position, this_type& x, const_iterator i);
		void splice(const_iterator position, this_type& x, const_iterator first, const_iterator last);
		void splice(const_iterator position, this_type&& x);
		void splice(const_iterator position, this_type&& x, const_iterator i);
		void splice(const_iterator position, this_type&& x, const_iterator first, const_iterator last);

	public:
		// For merge, see notes for splice regarding the handling of unequal allocators.
		void merge(this_type& x);
		void merge(this_type&& x);

		template <typename Compare>
		void merge(this_type& x, Compare compare);

		template <typename Compare>
		void merge(this_type&& x, Compare compare);

		void unique();

		template <typename BinaryPredicate>
		void unique(BinaryPredicate);

		// Sorting functionality
		// This is independent of the global sort algorithms, as lists are 
		// linked nodes and can be sorted more efficiently by moving nodes
		// around in ways that global sort algorithms aren't privy to.
		void sort();

		template<typename Compare>
		void sort(Compare compare);

	public:
		bool validate() const;
		int  validate_iterator(const_iterator i) const;

	protected:
		node_type* DoCreateNode();

		template<typename... Args>
		node_type* DoCreateNode(Args&&... args);

		template <typename Integer>
		void DoAssign(Integer n, Integer value, true_type);

		template <typename InputIterator>
		void DoAssign(InputIterator first, InputIterator last, false_type);

		void DoAssignValues(size_type n, const value_type& value);

		template <typename Integer>
		void DoInsert(ListNodeBase* pNode, Integer n, Integer value, true_type);

		template <typename InputIterator>
		void DoInsert(ListNodeBase* pNode, InputIterator first, InputIterator last, false_type);

		void DoInsertValues(ListNodeBase* pNode, size_type n, const value_type& value);
	   
		template<typename... Args>
		void DoInsertValue(ListNodeBase* pNode, Args&&... args);

		void DoErase(ListNodeBase* pNode);

		void DoSwap(this_type& x);

		template <typename Compare>
		iterator DoSort(iterator i1, iterator end2, size_type n, Compare& compare);

	}; // class list





	///////////////////////////////////////////////////////////////////////
	// ListNodeBase
	///////////////////////////////////////////////////////////////////////

	// Swaps the nodes a and b in the lists to which they belong. This is similar to 
	// splicing a into b's list and b into a's list at the same time.
	// Works by swapping the members of a and b, and fixes up the lists that a and b 
	// were part of to point to the new members.
	inline void ListNodeBase::swap(ListNodeBase& a, ListNodeBase& b) EA_NOEXCEPT
	{
		const ListNodeBase temp(a);
		a = b;
		b = temp;

		if(a.mpNext == &b)
			a.mpNext = a.mpPrev = &a;
		else
			a.mpNext->mpPrev = a.mpPrev->mpNext = &a;

		if(b.mpNext == &a)
			b.mpNext = b.mpPrev = &b;
		else
			b.mpNext->mpPrev = b.mpPrev->mpNext = &b;
	}


	// splices the [first,last) range from its current list into our list before this node.
	inline void ListNodeBase::splice(ListNodeBase* first, ListNodeBase* last) EA_NOEXCEPT
	{
		// We assume that [first, last] are not within our list.
		last->mpPrev->mpNext  = this;
		first->mpPrev->mpNext = last;
		this->mpPrev->mpNext  = first;

		ListNodeBase* const pTemp = this->mpPrev;
		this->mpPrev  = last->mpPrev;
		last->mpPrev  = first->mpPrev;
		first->mpPrev = pTemp;
	}


	inline void ListNodeBase::reverse() EA_NOEXCEPT
	{
		ListNodeBase* pNode = this;
		do
		{
			EA_ANALYSIS_ASSUME(pNode != NULL);
			ListNodeBase* const pTemp = pNode->mpNext;
			pNode->mpNext = pNode->mpPrev;
			pNode->mpPrev = pTemp;
			pNode         = pNode->mpPrev;
		} 
		while(pNode != this);
	}


	inline void ListNodeBase::insert(ListNodeBase* pNext) EA_NOEXCEPT
	{
		mpNext = pNext;
		mpPrev = pNext->mpPrev;
		pNext->mpPrev->mpNext = this;
		pNext->mpPrev = this;
	}


	// Removes this node from the list that it's in. Assumes that the 
	// node is within a list and thus that its prev/next pointers are valid.
	inline void ListNodeBase::remove() EA_NOEXCEPT
	{
		mpNext->mpPrev = mpPrev;
		mpPrev->mpNext = mpNext;
	}


	// Inserts the standalone range [pFirst, pFinal] before pPosition. Assumes that the
	// range is not within a list and thus that it's prev/next pointers are not valid.
	// Assumes that this node is within a list and thus that its prev/next pointers are valid.
	inline void ListNodeBase::insert_range(ListNodeBase* pFirst, ListNodeBase* pFinal) EA_NOEXCEPT
	{
		mpPrev->mpNext = pFirst;
		pFirst->mpPrev = mpPrev;
		mpPrev         = pFinal;
		pFinal->mpNext = this;
	}


	// Removes the range [pFirst, pFinal] from the list that it's in. Assumes that the 
	// range is within a list and thus that its prev/next pointers are valid.
	inline void ListNodeBase::remove_range(ListNodeBase* pFirst, ListNodeBase* pFinal) EA_NOEXCEPT
	{
		pFinal->mpNext->mpPrev = pFirst->mpPrev;
		pFirst->mpPrev->mpNext = pFinal->mpNext;
	}


	///////////////////////////////////////////////////////////////////////
	// ListIterator
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Pointer, typename Reference>
	inline ListIterator<T, Pointer, Reference>::ListIterator() EA_NOEXCEPT
		: mpNode() // To consider: Do we really need to intialize mpNode?
	{
		// Empty
	}


	template <typename T, typename Pointer, typename Reference>
	inline ListIterator<T, Pointer, Reference>::ListIterator(const ListNodeBase* pNode) EA_NOEXCEPT
		: mpNode(static_cast<node_type*>((ListNode<T>*)const_cast<ListNodeBase*>(pNode))) // All this casting is in the name of making runtime debugging much easier on the user.
	{
		// Empty
	}


	template <typename T, typename Pointer, typename Reference>
	inline ListIterator<T, Pointer, Reference>::ListIterator(const iterator& x) EA_NOEXCEPT
		: mpNode(const_cast<node_type*>(x.mpNode))
	{
		// Empty
	} 


	template <typename T, typename Pointer, typename Reference>
	inline typename ListIterator<T, Pointer, Reference>::this_type
	ListIterator<T, Pointer, Reference>::next() const EA_NOEXCEPT
	{
		return ListIterator(mpNode->mpNext);
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename ListIterator<T, Pointer, Reference>::this_type
	ListIterator<T, Pointer, Reference>::prev() const EA_NOEXCEPT
	{
		return ListIterator(mpNode->mpPrev);
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename ListIterator<T, Pointer, Reference>::reference
	ListIterator<T, Pointer, Reference>::operator*() const EA_NOEXCEPT
	{
		return mpNode->mValue;
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename ListIterator<T, Pointer, Reference>::pointer
	ListIterator<T, Pointer, Reference>::operator->() const EA_NOEXCEPT
	{
		return &mpNode->mValue;
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename ListIterator<T, Pointer, Reference>::this_type&
	ListIterator<T, Pointer, Reference>::operator++() EA_NOEXCEPT
	{
		mpNode = static_cast<node_type*>(mpNode->mpNext);
		return *this;
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename ListIterator<T, Pointer, Reference>::this_type
	ListIterator<T, Pointer, Reference>::operator++(int) EA_NOEXCEPT
	{
		this_type temp(*this);
		mpNode = static_cast<node_type*>(mpNode->mpNext);
		return temp;
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename ListIterator<T, Pointer, Reference>::this_type&
	ListIterator<T, Pointer, Reference>::operator--() EA_NOEXCEPT
	{
		mpNode = static_cast<node_type*>(mpNode->mpPrev);
		return *this;
	}


	template <typename T, typename Pointer, typename Reference>
	inline typename ListIterator<T, Pointer, Reference>::this_type 
	ListIterator<T, Pointer, Reference>::operator--(int) EA_NOEXCEPT
	{
		this_type temp(*this);
		mpNode = static_cast<node_type*>(mpNode->mpPrev);
		return temp;
	}


	// The C++ defect report #179 requires that we support comparisons between const and non-const iterators.
	// Thus we provide additional template paremeters here to support this. The defect report does not
	// require us to support comparisons between reverse_iterators and const_reverse_iterators.
	template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
	inline bool operator==(const ListIterator<T, PointerA, ReferenceA>& a, 
						   const ListIterator<T, PointerB, ReferenceB>& b) EA_NOEXCEPT
	{
		return a.mpNode == b.mpNode;
	}


	template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
	inline bool operator!=(const ListIterator<T, PointerA, ReferenceA>& a, 
						   const ListIterator<T, PointerB, ReferenceB>& b) EA_NOEXCEPT
	{
		return a.mpNode != b.mpNode;
	}


	// We provide a version of operator!= for the case where the iterators are of the 
	// same type. This helps prevent ambiguity errors in the presence of rel_ops.
	template <typename T, typename Pointer, typename Reference>
	inline bool operator!=(const ListIterator<T, Pointer, Reference>& a, 
						   const ListIterator<T, Pointer, Reference>& b) EA_NOEXCEPT
	{
		return a.mpNode != b.mpNode;
	}



	///////////////////////////////////////////////////////////////////////
	// ListBase
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Allocator>
	inline ListBase<T, Allocator>::ListBase()
		: mNodeAllocator(base_node_type(), allocator_type(EASTL_LIST_DEFAULT_NAME))
		  #if EASTL_LIST_SIZE_CACHE
		  , mSize(0)
		  #endif
	{
		DoInit();
	}

	template <typename T, typename Allocator>
	inline ListBase<T, Allocator>::ListBase(const allocator_type& allocator)
		: mNodeAllocator(base_node_type(), allocator)
		  #if EASTL_LIST_SIZE_CACHE
		  , mSize(0)
		  #endif
	{
		DoInit();
	}


	template <typename T, typename Allocator>
	inline ListBase<T, Allocator>::~ListBase()
	{
		DoClear();
	}


	template <typename T, typename Allocator>
	const typename ListBase<T, Allocator>::allocator_type&
	ListBase<T, Allocator>::get_allocator() const EA_NOEXCEPT
	{
		return internalAllocator();
	}


	template <typename T, typename Allocator>
	typename ListBase<T, Allocator>::allocator_type&
	ListBase<T, Allocator>::get_allocator() EA_NOEXCEPT
	{
		return internalAllocator();
	}


	template <typename T, typename Allocator>
	inline void ListBase<T, Allocator>::set_allocator(const allocator_type& allocator)
	{
		EASTL_ASSERT((internalAllocator() == allocator) || (static_cast<node_type*>(internalNode().mpNext) == &internalNode())); // We can only assign a different allocator if we are empty of elements.
		internalAllocator() = allocator;
	}


	template <typename T, typename Allocator>
	inline typename ListBase<T, Allocator>::node_type*
	ListBase<T, Allocator>::DoAllocateNode()
	{
		node_type* pNode = (node_type*)allocate_memory(internalAllocator(), sizeof(node_type), EASTL_ALIGN_OF(node_type), 0);
		EASTL_ASSERT(pNode != nullptr);
		return pNode;
	}


	template <typename T, typename Allocator>
	inline void ListBase<T, Allocator>::DoFreeNode(node_type* p)
	{
		EASTLFree(internalAllocator(), p, sizeof(node_type));
	}


	template <typename T, typename Allocator>
	inline void ListBase<T, Allocator>::DoInit() EA_NOEXCEPT
	{
		internalNode().mpNext = (ListNode<T>*)&internalNode();
		internalNode().mpPrev = (ListNode<T>*)&internalNode();
	}


	template <typename T, typename Allocator>
	inline void ListBase<T, Allocator>::DoClear()
	{
		node_type* p = static_cast<node_type*>(internalNode().mpNext);

		while(p != &internalNode())
		{
			node_type* const pTemp = p;
			p = static_cast<node_type*>(p->mpNext);
			pTemp->~node_type();
			EASTLFree(internalAllocator(), pTemp, sizeof(node_type));
		}
	}



	///////////////////////////////////////////////////////////////////////
	// list
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Allocator>
	inline list<T, Allocator>::list()
		: base_type()
	{
		// Empty
	}


	template <typename T, typename Allocator>
	inline list<T, Allocator>::list(const allocator_type& allocator)
		: base_type(allocator)
	{
		// Empty
	}


	template <typename T, typename Allocator>
	inline list<T, Allocator>::list(size_type n, const allocator_type& allocator)
		: base_type(allocator)
	{
		DoInsertValues((ListNodeBase*)&internalNode(), n, value_type());
	}


	template <typename T, typename Allocator>
	inline list<T, Allocator>::list(size_type n, const value_type& value, const allocator_type& allocator)
		: base_type(allocator) 
	{
		DoInsertValues((ListNodeBase*)&internalNode(), n, value);
	}


	template <typename T, typename Allocator>
	inline list<T, Allocator>::list(const this_type& x)
		: base_type(x.internalAllocator())
	{
		DoInsert((ListNodeBase*)&internalNode(), const_iterator((ListNodeBase*)x.internalNode().mpNext), const_iterator((ListNodeBase*)&x.internalNode()), false_type());
	}


	template <typename T, typename Allocator>
	inline list<T, Allocator>::list(const this_type& x, const allocator_type& allocator)
		: base_type(allocator)
	{
		DoInsert((ListNodeBase*)&internalNode(), const_iterator((ListNodeBase*)x.internalNode().mpNext), const_iterator((ListNodeBase*)&x.internalNode()), false_type());
	}


	template <typename T, typename Allocator>
	inline list<T, Allocator>::list(this_type&& x)
		: base_type(eastl::move(x.internalAllocator()))
	{
		swap(x);
	}


	template <typename T, typename Allocator>
	inline list<T, Allocator>::list(this_type&& x, const allocator_type& allocator)
		: base_type(allocator)
	{
		swap(x); // member swap handles the case that x has a different allocator than our allocator by doing a copy.
	}


	template <typename T, typename Allocator>
	inline list<T, Allocator>::list(std::initializer_list<value_type> ilist, const allocator_type& allocator)
		: base_type(allocator)
	{
		DoInsert((ListNodeBase*)&internalNode(), ilist.begin(), ilist.end(), false_type());
	}


	template <typename T, typename Allocator>
	template <typename InputIterator>
	list<T, Allocator>::list(InputIterator first, InputIterator last)
		: base_type(EASTL_LIST_DEFAULT_ALLOCATOR)
	{
		//insert(const_iterator((ListNodeBase*)&internalNode()), first, last);
		DoInsert((ListNodeBase*)&internalNode(), first, last, is_integral<InputIterator>());
	}


	template <typename T, typename Allocator>
	typename list<T, Allocator>::iterator
	inline list<T, Allocator>::begin() EA_NOEXCEPT
	{
		return iterator((ListNodeBase*)internalNode().mpNext);
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::const_iterator
	list<T, Allocator>::begin() const EA_NOEXCEPT
	{
		return const_iterator((ListNodeBase*)internalNode().mpNext);
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::const_iterator
	list<T, Allocator>::cbegin() const EA_NOEXCEPT
	{
		return const_iterator((ListNodeBase*)internalNode().mpNext);
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::iterator
	list<T, Allocator>::end() EA_NOEXCEPT
	{
		return iterator((ListNodeBase*)&internalNode());
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::const_iterator
	list<T, Allocator>::end() const EA_NOEXCEPT
	{
		return const_iterator((ListNodeBase*)&internalNode());
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::const_iterator
	list<T, Allocator>::cend() const EA_NOEXCEPT
	{
		return const_iterator((ListNodeBase*)&internalNode());
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::reverse_iterator
	list<T, Allocator>::rbegin() EA_NOEXCEPT
	{
		return reverse_iterator((ListNodeBase*)&internalNode());
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::const_reverse_iterator
	list<T, Allocator>::rbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator((ListNodeBase*)&internalNode());
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::const_reverse_iterator
	list<T, Allocator>::crbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator((ListNodeBase*)&internalNode());
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::reverse_iterator
	list<T, Allocator>::rend() EA_NOEXCEPT
	{
		return reverse_iterator((ListNodeBase*)internalNode().mpNext);
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::const_reverse_iterator
	list<T, Allocator>::rend() const EA_NOEXCEPT
	{
		return const_reverse_iterator((ListNodeBase*)internalNode().mpNext);
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::const_reverse_iterator
	list<T, Allocator>::crend() const EA_NOEXCEPT
	{
		return const_reverse_iterator((ListNodeBase*)internalNode().mpNext);
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::reference
	list<T, Allocator>::front()
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY(static_cast<node_type*>(internalNode().mpNext) == &internalNode()))
				EASTL_FAIL_MSG("list::front -- empty container");
		#else
			// We allow the user to reference an empty container.
		#endif

		return static_cast<node_type*>(internalNode().mpNext)->mValue;
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::const_reference
	list<T, Allocator>::front() const
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY(static_cast<node_type*>(internalNode().mpNext) == &internalNode()))
				EASTL_FAIL_MSG("list::front -- empty container");
		#else
			// We allow the user to reference an empty container.
		#endif

		return static_cast<node_type*>(internalNode().mpNext)->mValue;
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::reference
	list<T, Allocator>::back()
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY(static_cast<node_type*>(internalNode().mpNext) == &internalNode()))
				EASTL_FAIL_MSG("list::back -- empty container");
		#else
			// We allow the user to reference an empty container.
		#endif

		return static_cast<node_type*>(internalNode().mpPrev)->mValue;
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::const_reference
	list<T, Allocator>::back() const
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY(static_cast<node_type*>(internalNode().mpNext) == &internalNode()))
				EASTL_FAIL_MSG("list::back -- empty container");
		#else
			// We allow the user to reference an empty container.
		#endif

		return static_cast<node_type*>(internalNode().mpPrev)->mValue;
	}


	template <typename T, typename Allocator>
	inline bool list<T, Allocator>::empty() const EA_NOEXCEPT
	{
		#if EASTL_LIST_SIZE_CACHE
			return (mSize == 0);
		#else
			return static_cast<node_type*>(internalNode().mpNext) == &internalNode();
		#endif
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::size_type
	list<T, Allocator>::size() const EA_NOEXCEPT
	{
		#if EASTL_LIST_SIZE_CACHE
			return mSize;
		#else
			#if EASTL_DEBUG
				const ListNodeBase* p = (ListNodeBase*)internalNode().mpNext;
				size_type n = 0;
				while(p != (ListNodeBase*)&internalNode())
				{
					++n;
					p = (ListNodeBase*)p->mpNext;
				}
				return n;
			#else
				// The following optimizes to slightly better code than the code above.
				return (size_type)eastl::distance(const_iterator((ListNodeBase*)internalNode().mpNext), const_iterator((ListNodeBase*)&internalNode()));
			#endif
		#endif
	}


	template <typename T, typename Allocator>
	typename list<T, Allocator>::this_type&
	list<T, Allocator>::operator=(const this_type& x)
	{
		if(this != &x) // If not assigning to self...
		{
			// If (EASTL_ALLOCATOR_COPY_ENABLED == 1) and the current contents are allocated by an 
			// allocator that's unequal to x's allocator, we need to reallocate our elements with 
			// our current allocator and reallocate it with x's allocator. If the allocators are 
			// equal then we can use a more optimal algorithm that doesn't reallocate our elements
			// but instead can copy them in place.

			#if EASTL_ALLOCATOR_COPY_ENABLED
				bool bSlowerPathwayRequired = (internalAllocator() != x.internalAllocator());
			#else
				bool bSlowerPathwayRequired = false;
			#endif

			if(bSlowerPathwayRequired)
			{
				clear();

				#if EASTL_ALLOCATOR_COPY_ENABLED
					internalAllocator() = x.internalAllocator();
				#endif
			}

			DoAssign(x.begin(), x.end(), eastl::false_type());
		}

		return *this;
	}


	template <typename T, typename Allocator>
	typename list<T, Allocator>::this_type&
	list<T, Allocator>::operator=(this_type&& x)
	{
		if(this != &x)
		{
			clear();        // To consider: Are we really required to clear here? x is going away soon and will clear itself in its dtor.
			swap(x);        // member swap handles the case that x has a different allocator than our allocator by doing a copy.
		}
		return *this;
	}


	template <typename T, typename Allocator>
	typename list<T, Allocator>::this_type&
	list<T, Allocator>::operator=(std::initializer_list<value_type> ilist)
	{
		DoAssign(ilist.begin(), ilist.end(), false_type());
		return *this;
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::assign(size_type n, const value_type& value)
	{
		DoAssignValues(n, value);
	}


	// It turns out that the C++ std::list specifies a two argument
	// version of assign that takes (int size, int value). These are not 
	// iterators, so we need to do a template compiler trick to do the right thing.
	template <typename T, typename Allocator>
	template <typename InputIterator>
	inline void list<T, Allocator>::assign(InputIterator first, InputIterator last)
	{
		DoAssign(first, last, is_integral<InputIterator>());
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::assign(std::initializer_list<value_type> ilist)
	{
		DoAssign(ilist.begin(), ilist.end(), false_type());
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::clear() EA_NOEXCEPT
	{
		DoClear();
		DoInit();
		#if EASTL_LIST_SIZE_CACHE
			mSize = 0;
		#endif
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::reset_lose_memory() EA_NOEXCEPT
	{
		// The reset_lose_memory function is a special extension function which unilaterally 
		// resets the container to an empty state without freeing the memory of 
		// the contained objects. This is useful for very quickly tearing down a 
		// container built into scratch memory.
		DoInit();
		#if EASTL_LIST_SIZE_CACHE
			mSize = 0;
		#endif
	}


	template <typename T, typename Allocator>
	void list<T, Allocator>::resize(size_type n, const value_type& value)
	{
		iterator current((ListNodeBase*)internalNode().mpNext);
		size_type i = 0;

		while((current.mpNode != &internalNode()) && (i < n))
		{
			++current;  
			++i;
		}
		if(i == n)
			erase(current, (ListNodeBase*)&internalNode());
		else
			insert((ListNodeBase*)&internalNode(), n - i, value);
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::resize(size_type n)
	{
		resize(n, value_type());
	}


	template <typename T, typename Allocator>
	template <typename... Args>
	void list<T, Allocator>::emplace_front(Args&&... args)
	{
		DoInsertValue((ListNodeBase*)internalNode().mpNext, eastl::forward<Args>(args)...);
	}

	template <typename T, typename Allocator>
	template <typename... Args>
	void list<T, Allocator>::emplace_back(Args&&... args)
	{
		DoInsertValue((ListNodeBase*)&internalNode(), eastl::forward<Args>(args)...);
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::push_front(const value_type& value)
	{
		DoInsertValue((ListNodeBase*)internalNode().mpNext, value);
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::push_front(value_type&& value)
	{
		emplace(begin(), eastl::move(value));
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::reference
	list<T, Allocator>::push_front()
	{
		node_type* const pNode = DoCreateNode();
		((ListNodeBase*)pNode)->insert((ListNodeBase*)internalNode().mpNext);
		#if EASTL_LIST_SIZE_CACHE
			++mSize;
		#endif
		return static_cast<node_type*>(internalNode().mpNext)->mValue; // Same as return front();
	}


	template <typename T, typename Allocator>
	inline void* list<T, Allocator>::push_front_uninitialized()
	{
		node_type* const pNode = DoAllocateNode();
		((ListNodeBase*)pNode)->insert((ListNodeBase*)internalNode().mpNext);
		#if EASTL_LIST_SIZE_CACHE
			++mSize;
		#endif
		return &pNode->mValue;
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::pop_front()
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(static_cast<node_type*>(internalNode().mpNext) == &internalNode()))
				EASTL_FAIL_MSG("list::pop_front -- empty container");
		#endif

		DoErase((ListNodeBase*)internalNode().mpNext);
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::push_back(const value_type& value)
	{
		DoInsertValue((ListNodeBase*)&internalNode(), value);
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::push_back(value_type&& value)
	{
		emplace(end(), eastl::move(value));
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::reference
	list<T, Allocator>::push_back()
	{
		node_type* const pNode = DoCreateNode();
		((ListNodeBase*)pNode)->insert((ListNodeBase*)&internalNode());
		#if EASTL_LIST_SIZE_CACHE
			++mSize;
		#endif
		return static_cast<node_type*>(internalNode().mpPrev)->mValue;  // Same as return back();
	}


	template <typename T, typename Allocator>
	inline void* list<T, Allocator>::push_back_uninitialized()
	{
		node_type* const pNode = DoAllocateNode();
		((ListNodeBase*)pNode)->insert((ListNodeBase*)&internalNode());
		#if EASTL_LIST_SIZE_CACHE
			++mSize;
		#endif
		return &pNode->mValue;
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::pop_back()
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(static_cast<node_type*>(internalNode().mpNext) == &internalNode()))
				EASTL_FAIL_MSG("list::pop_back -- empty container");
		#endif

		DoErase((ListNodeBase*)internalNode().mpPrev);
	}


	template <typename T, typename Allocator>
	template <typename... Args>
	inline typename list<T, Allocator>::iterator
	list<T, Allocator>::emplace(const_iterator position, Args&&... args)
	{
		DoInsertValue(position.mpNode, eastl::forward<Args>(args)...);
		return iterator(position.mpNode->mpPrev);
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::iterator
	list<T, Allocator>::insert(const_iterator position)
	{
		node_type* const pNode = DoCreateNode(value_type());
		((ListNodeBase*)pNode)->insert((ListNodeBase*)position.mpNode);
		#if EASTL_LIST_SIZE_CACHE
			++mSize;
		#endif
		return (ListNodeBase*)pNode;
	}

	
	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::iterator
	list<T, Allocator>::insert(const_iterator position, const value_type& value)
	{
		node_type* const pNode = DoCreateNode(value);
		((ListNodeBase*)pNode)->insert((ListNodeBase*)position.mpNode);
		#if EASTL_LIST_SIZE_CACHE
			++mSize;
		#endif
		return (ListNodeBase*)pNode;
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::iterator
	list<T, Allocator>::insert(const_iterator position, value_type&& value)
	{
		return emplace(position, eastl::move(value));
	}

	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::iterator
	list<T, Allocator>::insert(const_iterator position, size_type n, const value_type& value)
	{
		iterator itPrev(position.mpNode);
		--itPrev;
		DoInsertValues((ListNodeBase*)position.mpNode, n, value);
		return ++itPrev; // Inserts in front of position, returns iterator to new elements. 
	}


	template <typename T, typename Allocator>
	template <typename InputIterator>
	inline typename list<T, Allocator>::iterator
	list<T, Allocator>::insert(const_iterator position, InputIterator first, InputIterator last)
	{
		iterator itPrev(position.mpNode);
		--itPrev;
		DoInsert((ListNodeBase*)position.mpNode, first, last, is_integral<InputIterator>());
		return ++itPrev; // Inserts in front of position, returns iterator to new elements. 
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::iterator 
	list<T, Allocator>::insert(const_iterator position, std::initializer_list<value_type> ilist)
	{
		iterator itPrev(position.mpNode);
		--itPrev;
		DoInsert((ListNodeBase*)position.mpNode, ilist.begin(), ilist.end(), false_type());
		return ++itPrev; // Inserts in front of position, returns iterator to new elements. 
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::iterator
	list<T, Allocator>::erase(const_iterator position)
	{
		++position;
		DoErase((ListNodeBase*)position.mpNode->mpPrev);
		return iterator(position.mpNode);
	}


	template <typename T, typename Allocator>
	typename list<T, Allocator>::iterator
	list<T, Allocator>::erase(const_iterator first, const_iterator last)
	{
		while(first != last)
			first = erase(first);
		return iterator(last.mpNode);
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::reverse_iterator
	list<T, Allocator>::erase(const_reverse_iterator position)
	{
		return reverse_iterator(erase((++position).base()));
	}


	template <typename T, typename Allocator>
	typename list<T, Allocator>::reverse_iterator
	list<T, Allocator>::erase(const_reverse_iterator first, const_reverse_iterator last)
	{
		// Version which erases in order from first to last.
		// difference_type i(first.base() - last.base());
		// while(i--)
		//     first = erase(first);
		// return first;

		// Version which erases in order from last to first, but is slightly more efficient:
		const_iterator itLastBase((++last).base());
		const_iterator itFirstBase((++first).base());

		return reverse_iterator(erase(itLastBase, itFirstBase));
	}


	template <typename T, typename Allocator>
	void list<T, Allocator>::remove(const value_type& value)
	{
		iterator current((ListNodeBase*)internalNode().mpNext);

		while(current.mpNode != &internalNode())
		{
			if(EASTL_LIKELY(!(*current == value)))
				++current; // We have duplicate '++current' statements here and below, but the logic here forces this.
			else
			{
				++current;
				DoErase((ListNodeBase*)current.mpNode->mpPrev);
			}
		}
	}


	template <typename T, typename Allocator>
	template <typename Predicate>
	inline void list<T, Allocator>::remove_if(Predicate predicate)
	{
		for(iterator first((ListNodeBase*)internalNode().mpNext), last((ListNodeBase*)&internalNode()); first != last; )
		{
			iterator temp(first);
			++temp;
			if(predicate(first.mpNode->mValue))
				DoErase((ListNodeBase*)first.mpNode);
			first = temp;
		}
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::reverse() EA_NOEXCEPT
	{
		((ListNodeBase&)internalNode()).reverse();
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::splice(const_iterator position, this_type& x)
	{
		// Splicing operations cannot succeed if the two containers use unequal allocators.
		// This issue is not addressed in the C++ 1998 standard but is discussed in the 
		// LWG defect reports, such as #431. There is no simple solution to this problem.
		// One option is to throw an exception. Another option which probably captures the
		// user intent most of the time is to copy the range from the source to the dest and 
		// remove it from the source. 

		if(internalAllocator() == x.internalAllocator())
		{
			#if EASTL_LIST_SIZE_CACHE
				if(x.mSize)
				{
					((ListNodeBase*)position.mpNode)->splice((ListNodeBase*)x.internalNode().mpNext, (ListNodeBase*)&x.internalNode());
					mSize += x.mSize;
					x.mSize = 0;
				}
			#else
				if(!x.empty())
					((ListNodeBase*)position.mpNode)->splice((ListNodeBase*)x.internalNode().mpNext, (ListNodeBase*)&x.internalNode());
			#endif
		}
		else
		{
			insert(position, x.begin(), x.end());
			x.clear();
		}
	}

	template <typename T, typename Allocator>
	inline void list<T, Allocator>::splice(const_iterator position, this_type&& x)
	{
		return splice(position, x); // This will call splice(const_iterator, const this_type&);
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::splice(const_iterator position, list& x, const_iterator i)
	{
		if(internalAllocator() == x.internalAllocator())
		{
			iterator i2(i.mpNode);
			++i2;
			if((position != i) && (position != i2))
			{
				((ListNodeBase*)position.mpNode)->splice((ListNodeBase*)i.mpNode, (ListNodeBase*)i2.mpNode);

				#if EASTL_LIST_SIZE_CACHE
					++mSize;
					--x.mSize;
				#endif
			}
		}
		else
		{
			insert(position, *i);
			x.erase(i);
		}
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::splice(const_iterator position, list<T,Allocator>&& x, const_iterator i)
	{
		return splice(position, x, i); // This will call splice(const_iterator, const this_type&, const_iterator);
	}
		

	template <typename T, typename Allocator>
	inline void list<T, Allocator>::splice(const_iterator position, this_type& x, const_iterator first, const_iterator last)
	{
		if(internalAllocator() == x.internalAllocator())
		{
			#if EASTL_LIST_SIZE_CACHE
				const size_type n = (size_type)eastl::distance(first, last);

				if(n)
				{
					((ListNodeBase*)position.mpNode)->splice((ListNodeBase*)first.mpNode, (ListNodeBase*)last.mpNode);
					mSize += n;
					x.mSize -= n;
				}
			#else
				if(first != last)
					((ListNodeBase*)position.mpNode)->splice((ListNodeBase*)first.mpNode, (ListNodeBase*)last.mpNode);
			#endif
		}
		else
		{
			insert(position, first, last);
			x.erase(first, last);
		}
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::splice(const_iterator position, list<T,Allocator>&& x, const_iterator first, const_iterator last)
	{
		return splice(position, x, first, last); // This will call splice(const_iterator, const this_type&, const_iterator, const_iterator);
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::swap(this_type& x)
	{
		if(internalAllocator() == x.internalAllocator()) // If allocators are equivalent...
			DoSwap(x);
		else // else swap the contents.
		{
			const this_type temp(*this); // Can't call eastl::swap because that would
			*this = x;                   // itself call this member swap function.
			x     = temp;
		}
	}


	template <typename T, typename Allocator>
	void list<T, Allocator>::merge(this_type& x)
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


	template <typename T, typename Allocator>
	void list<T, Allocator>::merge(this_type&& x)
	{
		return merge(x); // This will call merge(this_type&)
	}


	template <typename T, typename Allocator>
	template <typename Compare>
	void list<T, Allocator>::merge(this_type& x, Compare compare)
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


	template <typename T, typename Allocator>
	template <typename Compare>
	void list<T, Allocator>::merge(this_type&& x, Compare compare)
	{
		return merge(x, compare); // This will call merge(this_type&, Compare)
	}


	template <typename T, typename Allocator>
	void list<T, Allocator>::unique()
	{
		iterator       first(begin());
		const iterator last(end());

		if(first != last)
		{
			iterator next(first);

			while(++next != last)
			{
				if(*first == *next)
					DoErase((ListNodeBase*)next.mpNode);
				else
					first = next;
				next = first;
			}
		}
	}


	template <typename T, typename Allocator>
	template <typename BinaryPredicate>
	void list<T, Allocator>::unique(BinaryPredicate predicate)
	{
		iterator       first(begin());
		const iterator last(end());

		if(first != last)
		{
			iterator next(first);

			while(++next != last)
			{
				if(predicate(*first, *next))
					DoErase((ListNodeBase*)next.mpNode);
				else
					first = next;
				next = first;
			}
		}
	}


	template <typename T, typename Allocator>
	void list<T, Allocator>::sort()
	{
		eastl::less<value_type> compare;
		DoSort(begin(), end(), size(), compare);
	}


	template <typename T, typename Allocator>
	template <typename Compare>
	void list<T, Allocator>::sort(Compare compare)
	{
		DoSort(begin(), end(), size(), compare);
	}


	template <typename T, typename Allocator>
	template <typename Compare>
	typename list<T, Allocator>::iterator
	list<T, Allocator>::DoSort(iterator i1, iterator end2, size_type n, Compare& compare)
	{
		// A previous version of this function did this by creating temporary lists, 
		// but that was incompatible with fixed_list because the sizes could be too big.
		// We sort subsegments by recursive descent. Then merge as we ascend.
		// Return an iterator to the beginning of the sorted subsegment.
		// Start with a special case for small node counts.
		switch (n)
		{
			case 0:
			case 1:
				return i1;

			case 2:
				// Potentialy swap these two nodes and return the resulting first of them.
				if(compare(*--end2, *i1))
				{
					end2.mpNode->remove();
					end2.mpNode->insert(i1.mpNode);
					return end2;
				}
				return i1;

			case 3:
			{
				// We do a list insertion sort. Measurements showed this improved performance 3-12%.
				iterator lowest = i1;

				for(iterator current = i1.next(); current != end2; ++current)
				{
					if(compare(*current, *lowest))
						lowest = current;
				}

				if(lowest == i1)
					++i1;
				else
				{
					lowest.mpNode->remove();
					lowest.mpNode->insert(i1.mpNode);
				}

				if(compare(*--end2, *i1)) // At this point, i1 refers to the second element in this three element segment.
				{
					end2.mpNode->remove();
					end2.mpNode->insert(i1.mpNode);
				}

				return lowest;
			}
		 }

		// Divide the range into two parts are recursively sort each part. Upon return we will have
		// two halves that are each sorted but we'll need to merge the two together before returning.
		iterator  result;
		size_type nMid = (n / 2);
		iterator  end1 = eastl::next(i1, (difference_type)nMid);
				  i1   = DoSort(i1, end1, nMid, compare);        // Return the new beginning of the first sorted sub-range.
		iterator  i2   = DoSort(end1, end2, n - nMid, compare);  // Return the new beginning of the second sorted sub-range.

		// If the start of the second list is before the start of the first list, insert the first list 
		// into the second at an appropriate starting place. 
		if(compare(*i2, *i1))
		{
			// Find the position to insert the first list into the second list. 
			iterator ix = i2.next();
			while((ix != end2) && compare(*ix, *i1))
				++ix;

			// Cut out the initial segment of the second list and move it to be in front of the first list. 
			ListNodeBase* i2Cut     = i2.mpNode;
			ListNodeBase* i2CutLast = ix.mpNode->mpPrev;
			result = i2;
			end1   = i2 = ix;
			ListNodeBase::remove_range(i2Cut, i2CutLast);
			i1.mpNode->insert_range(i2Cut, i2CutLast);
		}
		else
		{
			result = i1;
			end1   = i2;
		}

		// Merge the two segments. We do this by merging the second sub-segment into the first, by walking forward in each of the two sub-segments.
		for(++i1; (i1 != end1) && (i2 != end2); ++i1) // while still working on either segment...
		{
			if(compare(*i2, *i1)) // If i2 is less than i1 and it needs to be merged in front of i1...
			{
				// Find the position to insert the i2 list into the i1 list. 
				iterator ix = i2.next();
				while((ix != end2) && compare(*ix, *i1))
					++ix;

				// Cut this section of the i2 sub-segment out and merge into the appropriate place in the i1 list.
				ListNodeBase* i2Cut     = i2.mpNode;
				ListNodeBase* i2CutLast = ix.mpNode->mpPrev;
				if(end1 == i2)
					end1 = ix;
				i2 = ix;
				ListNodeBase::remove_range(i2Cut, i2CutLast);
				i1.mpNode->insert_range(i2Cut, i2CutLast);
			}
		}

		return result;
	}


	template <typename T, typename Allocator>
	template<typename... Args>
	inline typename list<T, Allocator>::node_type*
	list<T, Allocator>::DoCreateNode(Args&&... args)
	{
		node_type* const pNode = DoAllocateNode();  // pNode is of type node_type, but it's uninitialized memory.

		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
				::new((void*)&pNode->mValue) value_type(eastl::forward<Args>(args)...);
			}
			catch(...)
			{
				DoFreeNode(pNode);
				throw;
			}
		#else
			::new((void*)&pNode->mValue) value_type(eastl::forward<Args>(args)...);
		#endif

		return pNode;
	}


	template <typename T, typename Allocator>
	inline typename list<T, Allocator>::node_type*
	list<T, Allocator>::DoCreateNode()
	{
		node_type* const pNode = DoAllocateNode();

		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
				::new((void*)&pNode->mValue) value_type();
			}
			catch(...)
			{
				DoFreeNode(pNode);
				throw;
			}
		#else
			::new((void*)&pNode->mValue) value_type;
		#endif

		return pNode;
	}


	template <typename T, typename Allocator>
	template <typename Integer>
	inline void list<T, Allocator>::DoAssign(Integer n, Integer value, true_type)
	{
		DoAssignValues(static_cast<size_type>(n), static_cast<value_type>(value));
	}


	template <typename T, typename Allocator>
	template <typename InputIterator>
	void list<T, Allocator>::DoAssign(InputIterator first, InputIterator last, false_type)
	{
		node_type* pNode = static_cast<node_type*>(internalNode().mpNext);

		for(; (pNode != &internalNode()) && (first != last); ++first)
		{
			pNode->mValue = *first;
			pNode         = static_cast<node_type*>(pNode->mpNext);
		}

		if(first == last)
			erase(const_iterator((ListNodeBase*)pNode), (ListNodeBase*)&internalNode());
		else
			DoInsert((ListNodeBase*)&internalNode(), first, last, false_type());
	}


	template <typename T, typename Allocator>
	void list<T, Allocator>::DoAssignValues(size_type n, const value_type& value)
	{
		node_type* pNode  = static_cast<node_type*>(internalNode().mpNext);

		for(; (pNode != &internalNode()) && (n > 0); --n)
		{
			pNode->mValue = value;
			pNode         = static_cast<node_type*>(pNode->mpNext);
		}

		if(n)
			DoInsertValues((ListNodeBase*)&internalNode(), n, value);
		else
			erase(const_iterator((ListNodeBase*)pNode), (ListNodeBase*)&internalNode());
	}


	template <typename T, typename Allocator>
	template <typename Integer>
	inline void list<T, Allocator>::DoInsert(ListNodeBase* pNode, Integer n, Integer value, true_type)
	{
		DoInsertValues(pNode, static_cast<size_type>(n), static_cast<value_type>(value));
	}


	template <typename T, typename Allocator>
	template <typename InputIterator>
	inline void list<T, Allocator>::DoInsert(ListNodeBase* pNode, InputIterator first, InputIterator last, false_type)
	{
		for(; first != last; ++first)
			DoInsertValue(pNode, *first);
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::DoInsertValues(ListNodeBase* pNode, size_type n, const value_type& value)
	{
		for(; n > 0; --n)
			DoInsertValue(pNode, value);
	}


	template <typename T, typename Allocator>
	template<typename... Args>
	inline void list<T, Allocator>::DoInsertValue(ListNodeBase* pNode, Args&&... args)
	{
		node_type* const pNodeNew = DoCreateNode(eastl::forward<Args>(args)...);
		((ListNodeBase*)pNodeNew)->insert(pNode);
		#if EASTL_LIST_SIZE_CACHE
			++mSize;
		#endif
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::DoErase(ListNodeBase* pNode)
	{
		pNode->remove();
		((node_type*)pNode)->~node_type();
		DoFreeNode(((node_type*)pNode));
		#if EASTL_LIST_SIZE_CACHE
			--mSize;
		#endif

		/* Test version that uses union intermediates
		union
		{
			ListNodeBase* mpBase;
			node_type*    mpNode;
		} node = { pNode };

		node.mpNode->~node_type();
		node.mpBase->remove();
		DoFreeNode(node.mpNode);
		#if EASTL_LIST_SIZE_CACHE
			--mSize;
		#endif
		*/
	}


	template <typename T, typename Allocator>
	inline void list<T, Allocator>::DoSwap(this_type& x)
	{
		ListNodeBase::swap((ListNodeBase&)internalNode(), (ListNodeBase&)x.internalNode()); // We need to implement a special swap because we can't do a shallow swap.
		eastl::swap(internalAllocator(), x.internalAllocator()); // We do this even if EASTL_ALLOCATOR_COPY_ENABLED is 0.
		#if EASTL_LIST_SIZE_CACHE
			eastl::swap(mSize, x.mSize);
		#endif
	}


	template <typename T, typename Allocator>
	inline bool list<T, Allocator>::validate() const
	{
		#if EASTL_LIST_SIZE_CACHE
			size_type n = 0;

			for(const_iterator i(begin()), iEnd(end()); i != iEnd; ++i)
				++n;

			if(n != mSize)
				return false;
		#endif

		// To do: More validation.
		return true;
	}


	template <typename T, typename Allocator>
	inline int list<T, Allocator>::validate_iterator(const_iterator i) const
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

	template <typename T, typename Allocator>
	bool operator==(const list<T, Allocator>& a, const list<T, Allocator>& b)
	{
		typename list<T, Allocator>::const_iterator ia   = a.begin();
		typename list<T, Allocator>::const_iterator ib   = b.begin();
		typename list<T, Allocator>::const_iterator enda = a.end();

		#if EASTL_LIST_SIZE_CACHE
			if(a.size() == b.size())
			{
				while((ia != enda) && (*ia == *ib))
				{
					++ia;
					++ib;
				}
				return (ia == enda);
			}
			return false;
		#else
			typename list<T, Allocator>::const_iterator endb = b.end();

			while((ia != enda) && (ib != endb) && (*ia == *ib))
			{
				++ia;
				++ib;
			}
			return (ia == enda) && (ib == endb);
		#endif
	}

	template <typename T, typename Allocator>
	bool operator<(const list<T, Allocator>& a, const list<T, Allocator>& b)
	{
		return eastl::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
	}

	template <typename T, typename Allocator>
	bool operator!=(const list<T, Allocator>& a, const list<T, Allocator>& b)
	{
		return !(a == b);
	}

	template <typename T, typename Allocator>
	bool operator>(const list<T, Allocator>& a, const list<T, Allocator>& b)
	{
		return b < a;
	}

	template <typename T, typename Allocator>
	bool operator<=(const list<T, Allocator>& a, const list<T, Allocator>& b)
	{
		return !(b < a);
	}

	template <typename T, typename Allocator>
	bool operator>=(const list<T, Allocator>& a, const list<T, Allocator>& b)
	{
		return !(a < b);
	}

	template <typename T, typename Allocator>
	void swap(list<T, Allocator>& a, list<T, Allocator>& b)
	{
		a.swap(b);
	}


	///////////////////////////////////////////////////////////////////////
	// erase / erase_if
	//
	// https://en.cppreference.com/w/cpp/container/list/erase2
	///////////////////////////////////////////////////////////////////////
	template <class T, class Allocator, class U>
	void erase(list<T, Allocator>& c, const U& value)
	{
		// Erases all elements that compare equal to value from the container.
		c.remove_if([&](auto& elem) { return elem == value; });
	}

	template <class T, class Allocator, class Predicate>
	void erase_if(list<T, Allocator>& c, Predicate predicate)
	{
		// Erases all elements that satisfy the predicate pred from the container.
		c.remove_if(predicate);
	}


} // namespace eastl


EA_RESTORE_SN_WARNING()

EA_RESTORE_VC_WARNING();


#endif // Header include guard
