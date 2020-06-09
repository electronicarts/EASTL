/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// deque design
//
// A deque (pronounced "deck") is a double-ended queue, though this is partially 
// of a misnomer. A deque does indeed let you add and remove values from both ends
// of the container, but it's not usually used for such a thing and instead is used
// as a more flexible version of a vector. It provides operator[] (random access) 
// and can insert items anywhere and not just at the front and back.
// 
// While you can implement a double-ended queue via a doubly-linked list, deque is 
// instead implemented as a list of arrays. The benefit of this is that memory usage 
// is lower and that random access can be had with decent efficiency. 
// 
// Our implementation of deque is just like every other implementation of deque,
// as the C++ standard all but dictates that you make it work this way. Below 
// we have a depiction of an array (or vector) of 48 items, with each node being 
// a '+' character and extra capacity being a '-' character. What we have is one 
// contiguous block of memory:
// 
//     ++++++++++++++++++++++++++++++++++++++++++++++++-----------------
//     0                                              47
// 
// With a deque, the same array of 48 items would be implemented as multiple smaller
// arrays of contiguous memory, each of fixed size. We will call these "sub-arrays."
// In the case here, we have six arrays of 8 nodes:
// 
//     ++++++++ ++++++++ ++++++++ ++++++++ ++++++++ ++++++++
// 
// With an vector, item [0] is the first item and item [47] is the last item. With a 
// deque, item [0] is usually not the first item and neither is item [47]. There is 
// extra capacity on both the front side and the back side of the deque. So a deque
// (of 24 items) actually looks like this:
// 
//     -------- -----+++ ++++++++ ++++++++ +++++--- --------
//                   0                         23
// 
// To insert items at the front, you move into the capacity on the left, and to insert
// items at the back, you append items on the right. As you can see, inserting an item
// at the front doesn't require allocating new memory nor does it require moving any 
// items in the container. It merely involves moving the pointer to the [0] item to
// the left by one node.
// 
// We keep track of these sub-arrays by having an array of pointers, with each array 
// entry pointing to each of the sub-arrays. We could alternatively use a linked
// list of pointers, but it turns out we can implement our deque::operator[] more 
// efficiently if we use an array of pointers instead of a list of pointers.
//
// To implement deque::iterator, we could keep a struct which is essentially this:
//     struct iterator {
//        int subArrayIndex;
//        int subArrayOffset;
//     }
//
// In practice, we implement iterators a little differently, but in reality our 
// implementation isn't much different from the above. It turns out that it's most
// simple if we also manage the location of item [0] and item [end] by using these
// same iterators.
//
// To consider: Implement the deque as a circular deque instead of a linear one.
//              This would use a similar subarray layout but iterators would
//              wrap around when they reached the end of the subarray pointer list.
//
//////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_DEQUE_H
#define EASTL_DEQUE_H


#include <EASTL/internal/config.h>
#include <EASTL/allocator.h>
#include <EASTL/algorithm.h>
#include <EASTL/type_traits.h>
#include <EASTL/iterator.h>
#include <EASTL/memory.h>
#include <EASTL/initializer_list.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <new>
#include <stddef.h>
EA_RESTORE_ALL_VC_WARNINGS()

#if EASTL_EXCEPTIONS_ENABLED
	EA_DISABLE_ALL_VC_WARNINGS()
	#include <stdexcept> // std::out_of_range, std::length_error.
	EA_RESTORE_ALL_VC_WARNINGS()
#endif

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4267)  // 'argument' : conversion from 'size_t' to 'const uint32_t', possible loss of data. This is a bogus warning resulting from a bug in VC++.
	#pragma warning(disable: 4345)  // Behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized
	#pragma warning(disable: 4480)  // nonstandard extension used: specifying underlying type for enum
	#pragma warning(disable: 4530)  // C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
	#pragma warning(disable: 4571)  // catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught.
	#if EASTL_EXCEPTIONS_ENABLED
	#pragma warning(disable: 4703)  // potentially uninitialized local pointer variable used.   VC++ is mistakenly analyzing the possibility of uninitialized variables, though it's not easy for it to do so.
	#pragma warning(disable: 4701)  // potentially uninitialized local variable used.
	#endif
#endif

		
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif


namespace eastl
{

	/// EASTL_DEQUE_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_DEQUE_DEFAULT_NAME
		#define EASTL_DEQUE_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " deque" // Unless the user overrides something, this is "EASTL deque".
	#endif


	/// EASTL_DEQUE_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_DEQUE_DEFAULT_ALLOCATOR
		#define EASTL_DEQUE_DEFAULT_ALLOCATOR allocator_type(EASTL_DEQUE_DEFAULT_NAME)
	#endif


	/// DEQUE_DEFAULT_SUBARRAY_SIZE
	///
	/// Defines the default number of items in a subarray.
	/// Note that the user has the option of specifying the subarray size
	/// in the deque template declaration.
	///
	#if !defined(__GNUC__) || (__GNUC__ >= 3) // GCC 2.x can't handle the declaration below.
		#define DEQUE_DEFAULT_SUBARRAY_SIZE(T) ((sizeof(T) <= 4) ? 64 : ((sizeof(T) <= 8) ? 32 : ((sizeof(T) <= 16) ? 16 : ((sizeof(T) <= 32) ? 8 : 4))))
	#else
		#define DEQUE_DEFAULT_SUBARRAY_SIZE(T) 16
	#endif



	/// DequeIterator
	///
	/// The DequeIterator provides both const and non-const iterators for deque. 
	/// It also is used for the tracking of the begin and end for the deque.
	///
	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	struct DequeIterator
	{
		typedef DequeIterator<T, Pointer, Reference, kDequeSubarraySize>  this_type;
		typedef DequeIterator<T, T*, T&, kDequeSubarraySize>              iterator;
		typedef DequeIterator<T, const T*, const T&, kDequeSubarraySize>  const_iterator;
		typedef ptrdiff_t                                                 difference_type;
		typedef EASTL_ITC_NS::random_access_iterator_tag                  iterator_category;
		typedef T                                                         value_type;
		typedef T*                                                        pointer;
		typedef T&                                                        reference;

	public:
		DequeIterator();
		DequeIterator(const iterator& x);

		pointer   operator->() const;
		reference operator*() const;

		this_type& operator++();
		this_type  operator++(int);

		this_type& operator--();
		this_type  operator--(int);

		this_type& operator+=(difference_type n);
		this_type& operator-=(difference_type n);

		this_type operator+(difference_type n) const;
		this_type operator-(difference_type n) const;

	protected:
		template <typename, typename, typename, unsigned>
		friend struct DequeIterator;

		template <typename, typename, unsigned>
		friend struct DequeBase;

		template <typename, typename, unsigned>
		friend class deque;

		template <typename U, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB, unsigned kDequeSubarraySizeU>
		friend bool operator==(const DequeIterator<U, PointerA, ReferenceA, kDequeSubarraySizeU>&, 
							   const DequeIterator<U, PointerB, ReferenceB, kDequeSubarraySizeU>&);

		template <typename U, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB, unsigned kDequeSubarraySizeU>
		friend bool operator!=(const DequeIterator<U, PointerA, ReferenceA, kDequeSubarraySizeU>&, 
							   const DequeIterator<U, PointerB, ReferenceB, kDequeSubarraySizeU>&);

		template <typename U, typename PointerU, typename ReferenceU, unsigned kDequeSubarraySizeU>
		friend bool operator!=(const DequeIterator<U, PointerU, ReferenceU, kDequeSubarraySizeU>& a, 
							   const DequeIterator<U, PointerU, ReferenceU, kDequeSubarraySizeU>& b);

		template <typename U, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB, unsigned kDequeSubarraySizeU>
		friend bool operator< (const DequeIterator<U, PointerA, ReferenceA, kDequeSubarraySizeU>&, 
							   const DequeIterator<U, PointerB, ReferenceB, kDequeSubarraySizeU>&);

		template <typename U, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB, unsigned kDequeSubarraySizeU>
		friend bool operator> (const DequeIterator<U, PointerA, ReferenceA, kDequeSubarraySizeU>&, 
							   const DequeIterator<U, PointerB, ReferenceB, kDequeSubarraySizeU>&);

		template <typename U, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB, unsigned kDequeSubarraySizeU>
		friend bool operator<=(const DequeIterator<U, PointerA, ReferenceA, kDequeSubarraySizeU>&, 
							   const DequeIterator<U, PointerB, ReferenceB, kDequeSubarraySizeU>&);

		template <typename U, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB, unsigned kDequeSubarraySizeU>
		friend bool operator>=(const DequeIterator<U, PointerA, ReferenceA, kDequeSubarraySizeU>&, 
							   const DequeIterator<U, PointerB, ReferenceB, kDequeSubarraySizeU>&);

		template <typename U, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB, unsigned kDequeSubarraySizeU>
		friend typename DequeIterator<U, PointerA, ReferenceA, kDequeSubarraySizeU>::difference_type
		operator-(const DequeIterator<U, PointerA, ReferenceA, kDequeSubarraySizeU>& a,
				  const DequeIterator<U, PointerB, ReferenceB, kDequeSubarraySizeU>& b);

	protected:
		T*  mpCurrent;          // Where we currently point. Declared first because it's used most often.
		T*  mpBegin;            // The beginning of the current subarray.
		T*  mpEnd;              // The end of the current subarray. To consider: remove this member, as it is always equal to 'mpBegin + kDequeSubarraySize'. Given that deque subarrays usually consist of hundreds of bytes, this isn't a massive win. Also, now that we are implementing a zero-allocation new deque policy, mpEnd may in fact not be equal to 'mpBegin + kDequeSubarraySize'.
		T** mpCurrentArrayPtr;  // Pointer to current subarray. We could alternatively implement this as a list node iterator if the deque used a linked list.

		struct Increment {};
		struct Decrement {};
		struct FromConst {};

		DequeIterator(T** pCurrentArrayPtr, T* pCurrent);
		DequeIterator(const const_iterator& x, FromConst) : mpCurrent(x.mpCurrent), mpBegin(x.mpBegin), mpEnd(x.mpEnd), mpCurrentArrayPtr(x.mpCurrentArrayPtr){}
		DequeIterator(const iterator&       x, Increment);
		DequeIterator(const iterator&       x, Decrement);

		this_type copy(const iterator& first, const iterator& last, true_type);  // true means that value_type has the type_trait has_trivial_relocate,
		this_type copy(const iterator& first, const iterator& last, false_type); // false means it does not. 

		void copy_backward(const iterator& first, const iterator& last, true_type);  // true means that value_type has the type_trait has_trivial_relocate,
		void copy_backward(const iterator& first, const iterator& last, false_type); // false means it does not.

		void SetSubarray(T** pCurrentArrayPtr);
	};




	/// DequeBase
	///
	/// The DequeBase implements memory allocation for deque.
	/// See VectorBase (class vector) for an explanation of why we 
	/// create this separate base class.
	///
	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	struct DequeBase
	{
		typedef T                                                        value_type;
		typedef Allocator                                                allocator_type;
		typedef eastl_size_t                                             size_type;     // See config.h for the definition of eastl_size_t, which defaults to size_t.
		typedef ptrdiff_t                                                difference_type;
		typedef DequeIterator<T, T*, T&, kDequeSubarraySize>             iterator;
		typedef DequeIterator<T, const T*, const T&, kDequeSubarraySize> const_iterator;

		static const size_type npos     = (size_type)-1;      /// 'npos' means non-valid position or simply non-position.
		static const size_type kMaxSize = (size_type)-2;      /// -1 is reserved for 'npos'. It also happens to be slightly beneficial that kMaxSize is a value less than -1, as it helps us deal with potential integer wraparound issues.

		enum
		{
			kMinPtrArraySize = 8,                               /// A new empty deque has a ptrArraySize of 0, but any allocated ptrArrays use this min size.
			kSubarraySize    = kDequeSubarraySize               /// 
		  //kNodeSize        = kDequeSubarraySize * sizeof(T)   /// Disabled because it prevents the ability to do this: struct X{ eastl::deque<X, EASTLAllocatorType, 16> mDequeOfSelf; };
		};

		enum Side       /// Defines the side of the deque: front or back.
		{
			kSideFront, /// Identifies the front side of the deque.
			kSideBack   /// Identifies the back side of the deque.
		};

	protected:
		T**             mpPtrArray;         // Array of pointers to subarrays.
		size_type       mnPtrArraySize;     // Possibly we should store this as T** mpArrayEnd.
		iterator        mItBegin;           // Where within the subarrays is our beginning.
		iterator        mItEnd;             // Where within the subarrays is our end.
		allocator_type  mAllocator;         // To do: Use base class optimization to make this go away.

	public:
		DequeBase(const allocator_type& allocator);
		DequeBase(size_type n);
		DequeBase(size_type n, const allocator_type& allocator);
	   ~DequeBase();

		const allocator_type& get_allocator() const EA_NOEXCEPT;
		allocator_type&       get_allocator() EA_NOEXCEPT;
		void                  set_allocator(const allocator_type& allocator);

	protected:
		T*       DoAllocateSubarray();
		void     DoFreeSubarray(T* p);
		void     DoFreeSubarrays(T** pBegin, T** pEnd);

		T**      DoAllocatePtrArray(size_type n);
		void     DoFreePtrArray(T** p, size_t n);

		iterator DoReallocSubarray(size_type nAdditionalCapacity, Side allocationSide);
		void     DoReallocPtrArray(size_type nAdditionalCapacity, Side allocationSide);

		void     DoInit(size_type n);

	}; // DequeBase




	/// deque
	///
	/// Implements a conventional C++ double-ended queue. The implementation used here
	/// is very much like any other deque implementations you may have seen, as it 
	/// follows the standard algorithm for deque design. 
	///
	/// Note:
	/// As of this writing, deque does not support zero-allocation initial emptiness.
	/// A newly created deque with zero elements will still allocate a subarray
	/// pointer set. We are looking for efficient and clean ways to get around this,
	/// but current efforts have resulted in less efficient and more fragile code.
	/// The logic of this class doesn't lend itself to a clean implementation. 
	/// It turns out that deques are one of the least likely classes you'd want this
	/// behaviour in, so until this functionality becomes very imporantant to somebody,
	/// we will leave it as-is. It can probably be solved by adding some extra code to
	/// the Do* functions and adding good comments explaining the situation.
	/// 
	template <typename T, typename Allocator = EASTLAllocatorType, unsigned kDequeSubarraySize = DEQUE_DEFAULT_SUBARRAY_SIZE(T)>
	class deque : public DequeBase<T, Allocator, kDequeSubarraySize>
	{
	public:
		typedef DequeBase<T, Allocator, kDequeSubarraySize>              base_type;
		typedef deque<T, Allocator, kDequeSubarraySize>                  this_type;
		typedef T                                                        value_type;
		typedef T*                                                       pointer;
		typedef const T*                                                 const_pointer;
		typedef T&                                                       reference;
		typedef const T&                                                 const_reference;
		typedef DequeIterator<T, T*, T&, kDequeSubarraySize>             iterator;
		typedef DequeIterator<T, const T*, const T&, kDequeSubarraySize> const_iterator;
		typedef eastl::reverse_iterator<iterator>                        reverse_iterator;
		typedef eastl::reverse_iterator<const_iterator>                  const_reverse_iterator;
		typedef typename base_type::size_type                            size_type;
		typedef typename base_type::difference_type                      difference_type;
		typedef typename base_type::allocator_type                       allocator_type;

		using base_type::kSideFront;
		using base_type::kSideBack;
		using base_type::mpPtrArray;
		using base_type::mnPtrArraySize;
		using base_type::mItBegin;
		using base_type::mItEnd;
		using base_type::mAllocator;
		using base_type::npos;
		using base_type::DoAllocateSubarray;
		using base_type::DoFreeSubarray;
		using base_type::DoFreeSubarrays;
		using base_type::DoAllocatePtrArray;
		using base_type::DoFreePtrArray;
		using base_type::DoReallocSubarray;
		using base_type::DoReallocPtrArray;

	public:
		deque();
		explicit deque(const allocator_type& allocator);
		explicit deque(size_type n, const allocator_type& allocator = EASTL_DEQUE_DEFAULT_ALLOCATOR);
		deque(size_type n, const value_type& value, const allocator_type& allocator = EASTL_DEQUE_DEFAULT_ALLOCATOR);
		deque(const this_type& x);
		deque(this_type&& x);
		deque(this_type&& x, const allocator_type& allocator);
		deque(std::initializer_list<value_type> ilist, const allocator_type& allocator = EASTL_DEQUE_DEFAULT_ALLOCATOR);

		template <typename InputIterator>
		deque(InputIterator first, InputIterator last); // allocator arg removed because VC7.1 fails on the default arg. To do: Make a second version of this function without a default arg.

	   ~deque();

		this_type& operator=(const this_type& x);
		this_type& operator=(std::initializer_list<value_type> ilist);
		this_type& operator=(this_type&& x);

		void swap(this_type& x);

		void assign(size_type n, const value_type& value);
		void assign(std::initializer_list<value_type> ilist);

		template <typename InputIterator>                       // It turns out that the C++ std::deque<int, int> specifies a two argument
		void assign(InputIterator first, InputIterator last);   // version of assign that takes (int size, int value). These are not 
																// iterators, so we need to do a template compiler trick to do the right thing.

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

		void shrink_to_fit();
		void set_capacity(size_type n = base_type::npos);

		reference       operator[](size_type n);
		const_reference operator[](size_type n) const;

		reference       at(size_type n);
		const_reference at(size_type n) const;

		reference       front();
		const_reference front() const;

		reference       back();
		const_reference back() const;

		void      push_front(const value_type& value);
		reference push_front();
		void      push_front(value_type&& value);

		void      push_back(const value_type& value);
		reference push_back();
		void      push_back(value_type&& value);

		void pop_front();
		void pop_back();

		template<class... Args>
		iterator emplace(const_iterator position, Args&&... args);

		template<class... Args>
		void emplace_front(Args&&... args);

		template<class... Args>
		void emplace_back(Args&&... args);

		iterator insert(const_iterator position, const value_type& value);
		iterator insert(const_iterator position, value_type&& value);
		void     insert(const_iterator position, size_type n, const value_type& value);
		iterator insert(const_iterator position, std::initializer_list<value_type> ilist);

		template <typename InputIterator>
		void insert(const_iterator position, InputIterator first, InputIterator last);

		iterator         erase(const_iterator position);
		iterator         erase(const_iterator first, const_iterator last);
		reverse_iterator erase(reverse_iterator position);
		reverse_iterator erase(reverse_iterator first, reverse_iterator last);

		void clear();
		//void reset_lose_memory(); // Disabled until it can be implemented efficiently and cleanly.  // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.

		bool validate() const;
		int  validate_iterator(const_iterator i) const;

	protected:
		template <typename Integer>
		void DoInit(Integer n, Integer value, true_type);

		template <typename InputIterator>
		void DoInit(InputIterator first, InputIterator last, false_type);

		template <typename InputIterator>
		void DoInitFromIterator(InputIterator first, InputIterator last, EASTL_ITC_NS::input_iterator_tag);

		template <typename ForwardIterator>
		void DoInitFromIterator(ForwardIterator first, ForwardIterator last, EASTL_ITC_NS::forward_iterator_tag);

		void DoFillInit(const value_type& value);

		template <typename Integer>
		void DoAssign(Integer n, Integer value, true_type);

		template <typename InputIterator>
		void DoAssign(InputIterator first, InputIterator last, false_type);

		void DoAssignValues(size_type n, const value_type& value);

		template <typename Integer>
		void DoInsert(const const_iterator& position, Integer n, Integer value, true_type);

		template <typename InputIterator>
		void DoInsert(const const_iterator& position, const InputIterator& first, const InputIterator& last, false_type);

		template <typename InputIterator>
		void DoInsertFromIterator(const_iterator position, const InputIterator& first, const InputIterator& last, EASTL_ITC_NS::forward_iterator_tag);

		void DoInsertValues(const_iterator position, size_type n, const value_type& value);

		void DoSwap(this_type& x);
	}; // class deque




	///////////////////////////////////////////////////////////////////////
	// DequeBase
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	DequeBase<T, Allocator, kDequeSubarraySize>::DequeBase(const allocator_type& allocator)
		: mpPtrArray(NULL),
		  mnPtrArraySize(0),
		  mItBegin(),
		  mItEnd(),
		  mAllocator(allocator)
	{
		// It is assumed here that the deque subclass will init us when/as needed.
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	DequeBase<T, Allocator, kDequeSubarraySize>::DequeBase(size_type n)
		: mpPtrArray(NULL),
		  mnPtrArraySize(0),
		  mItBegin(),
		  mItEnd(),
		  mAllocator(EASTL_DEQUE_DEFAULT_NAME)
	{
		// It's important to note that DoInit creates space for elements and assigns 
		// mItBegin/mItEnd to point to them, but these elements are not constructed. 
		// You need to immediately follow this constructor with code that constructs the values.
		DoInit(n); 
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	DequeBase<T, Allocator, kDequeSubarraySize>::DequeBase(size_type n, const allocator_type& allocator)
		: mpPtrArray(NULL),
		  mnPtrArraySize(0),
		  mItBegin(),
		  mItEnd(),
		  mAllocator(allocator)
	{
		// It's important to note that DoInit creates space for elements and assigns 
		// mItBegin/mItEnd to point to them, but these elements are not constructed. 
		// You need to immediately follow this constructor with code that constructs the values.
		DoInit(n); 
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	DequeBase<T, Allocator, kDequeSubarraySize>::~DequeBase()
	{
		if(mpPtrArray)
		{
			DoFreeSubarrays(mItBegin.mpCurrentArrayPtr, mItEnd.mpCurrentArrayPtr + 1);
			DoFreePtrArray(mpPtrArray, mnPtrArraySize);
		}
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	const typename DequeBase<T, Allocator, kDequeSubarraySize>::allocator_type&
	DequeBase<T, Allocator, kDequeSubarraySize>::get_allocator() const EA_NOEXCEPT
	{
		return mAllocator;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename DequeBase<T, Allocator, kDequeSubarraySize>::allocator_type&
	DequeBase<T, Allocator, kDequeSubarraySize>::get_allocator() EA_NOEXCEPT
	{
		return mAllocator;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void DequeBase<T, Allocator, kDequeSubarraySize>::set_allocator(const allocator_type& allocator)
	{
		// The only time you can set an allocator is with an empty unused container, such as right after construction.
		if(EASTL_LIKELY(mAllocator != allocator))
		{
			if(EASTL_LIKELY(mpPtrArray && (mItBegin.mpCurrentArrayPtr == mItEnd.mpCurrentArrayPtr))) // If we are empty and so can safely deallocate the existing memory... We could also test for empty(), but that's a more expensive calculation and more involved clearing, though it would be more flexible.
			{
				DoFreeSubarrays(mItBegin.mpCurrentArrayPtr, mItEnd.mpCurrentArrayPtr + 1);
				DoFreePtrArray(mpPtrArray, mnPtrArraySize);

				mAllocator = allocator;
				DoInit(0);
			}
			else
			{
				EASTL_FAIL_MSG("DequeBase::set_allocator -- atempt to change allocator after allocating elements.");
			}
		}
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	T* DequeBase<T, Allocator, kDequeSubarraySize>::DoAllocateSubarray()
	{
		T* p = (T*)allocate_memory(mAllocator, kDequeSubarraySize * sizeof(T), EASTL_ALIGN_OF(T), 0);
		EASTL_ASSERT_MSG(p != nullptr, "the behaviour of eastl::allocators that return nullptr is not defined.");

		#if EASTL_DEBUG
			memset((void*)p, 0, kDequeSubarraySize * sizeof(T));
		#endif

		return (T*)p;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void DequeBase<T, Allocator, kDequeSubarraySize>::DoFreeSubarray(T* p)
	{
		if(p)
			EASTLFree(mAllocator, p, kDequeSubarraySize * sizeof(T)); 
	}

	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void DequeBase<T, Allocator, kDequeSubarraySize>::DoFreeSubarrays(T** pBegin, T** pEnd)
	{
		while(pBegin < pEnd)
			DoFreeSubarray(*pBegin++);
	}

	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	T** DequeBase<T, Allocator, kDequeSubarraySize>::DoAllocatePtrArray(size_type n)
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(n >= 0x80000000))
				EASTL_FAIL_MSG("deque::DoAllocatePtrArray -- improbably large request.");
		#endif

		T** pp = (T**)allocate_memory(mAllocator, n * sizeof(T*), EASTL_ALIGN_OF(T), 0);
		EASTL_ASSERT_MSG(pp != nullptr, "the behaviour of eastl::allocators that return nullptr is not defined.");

		#if EASTL_DEBUG
			memset((void*)pp, 0, n * sizeof(T*));
		#endif

		return pp;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void DequeBase<T, Allocator, kDequeSubarraySize>::DoFreePtrArray(T** pp, size_t n)
	{
		if(pp)
			EASTLFree(mAllocator, pp, n * sizeof(T*)); 
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename DequeBase<T, Allocator, kDequeSubarraySize>::iterator
	DequeBase<T, Allocator, kDequeSubarraySize>::DoReallocSubarray(size_type nAdditionalCapacity, Side allocationSide)
	{
		// nAdditionalCapacity refers to the amount of additional space we need to be 
		// able to store in this deque. Typically this function is called as part of 
		// an insert or append operation. This is the function that makes sure there
		// is enough capacity for the new elements to be copied into the deque.
		// The new capacity here is always at the front or back of the deque.
		// This function returns an iterator to that points to the new begin or
		// the new end of the deque space, depending on allocationSide.

		if(allocationSide == kSideFront)
		{
			// There might be some free space (nCurrentAdditionalCapacity) at the front of the existing subarray.
			const size_type nCurrentAdditionalCapacity = (size_type)(mItBegin.mpCurrent - mItBegin.mpBegin);

			if(EASTL_UNLIKELY(nCurrentAdditionalCapacity < nAdditionalCapacity)) // If we need to grow downward into a new subarray...
			{
				const difference_type nSubarrayIncrease = (difference_type)(((nAdditionalCapacity - nCurrentAdditionalCapacity) + kDequeSubarraySize - 1) / kDequeSubarraySize);
				difference_type i;

				if(nSubarrayIncrease > (mItBegin.mpCurrentArrayPtr - mpPtrArray)) // If there are not enough pointers in front of the current (first) one...
					DoReallocPtrArray((size_type)(nSubarrayIncrease - (mItBegin.mpCurrentArrayPtr - mpPtrArray)), kSideFront);

				#if EASTL_EXCEPTIONS_ENABLED
					try
					{
				#endif
						for(i = 1; i <= nSubarrayIncrease; ++i)
							mItBegin.mpCurrentArrayPtr[-i] = DoAllocateSubarray();
				#if EASTL_EXCEPTIONS_ENABLED
					}
					catch(...)
					{
						for(difference_type j = 1; j < i; ++j)
							DoFreeSubarray(mItBegin.mpCurrentArrayPtr[-j]);
						throw;
					}
				#endif
			}

			return mItBegin - (difference_type)nAdditionalCapacity;
		}
		else // else kSideBack
		{
			const size_type nCurrentAdditionalCapacity = (size_type)((mItEnd.mpEnd - 1) - mItEnd.mpCurrent);

			if(EASTL_UNLIKELY(nCurrentAdditionalCapacity < nAdditionalCapacity)) // If we need to grow forward into a new subarray...
			{
				const difference_type nSubarrayIncrease = (difference_type)(((nAdditionalCapacity - nCurrentAdditionalCapacity) + kDequeSubarraySize - 1) / kDequeSubarraySize);
				difference_type i;

				if(nSubarrayIncrease > ((mpPtrArray + mnPtrArraySize) - mItEnd.mpCurrentArrayPtr) - 1)  // If there are not enough pointers after the current (last) one...
					DoReallocPtrArray((size_type)(nSubarrayIncrease - (((mpPtrArray + mnPtrArraySize) - mItEnd.mpCurrentArrayPtr) - 1)), kSideBack);

				#if EASTL_EXCEPTIONS_ENABLED
					try
					{
				#endif
						for(i = 1; i <= nSubarrayIncrease; ++i)
							mItEnd.mpCurrentArrayPtr[i] = DoAllocateSubarray();
				#if EASTL_EXCEPTIONS_ENABLED
					}
					catch(...)
					{
						for(difference_type j = 1; j < i; ++j)
							DoFreeSubarray(mItEnd.mpCurrentArrayPtr[j]);
						throw;
					}
				#endif
			}

			return mItEnd + (difference_type)nAdditionalCapacity;
		}
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void DequeBase<T, Allocator, kDequeSubarraySize>::DoReallocPtrArray(size_type nAdditionalCapacity, Side allocationSide)
	{
		// This function is not called unless the capacity is known to require a resize.
		//
		// We have an array of pointers (mpPtrArray), of which a segment of them are in use and 
		// at either end of the array are zero or more unused pointers. This function is being
		// called because we need to extend the capacity on either side of this array by 
		// nAdditionalCapacity pointers. However, it's possible that if the user is continually
		// using push_back and pop_front then the pointer array will continue to be extended 
		// on the back side and unused on the front side. So while we are doing this resizing 
		// here we also take the opportunity to recenter the pointers and thus be balanced.
		// It man turn out that we don't even need to reallocate the pointer array in order
		// to increase capacity on one side, as simply moving the pointers to the center may
		// be enough to open up the requires space.
		//
		// Balanced pointer array     Unbalanced pointer array (unused space at front, no free space at back)
		// ----++++++++++++----        ---------+++++++++++

		const size_type nUnusedPtrCountAtFront = (size_type)(mItBegin.mpCurrentArrayPtr - mpPtrArray);
		const size_type nUsedPtrCount          = (size_type)(mItEnd.mpCurrentArrayPtr - mItBegin.mpCurrentArrayPtr) + 1;
		const size_type nUsedPtrSpace          = nUsedPtrCount * sizeof(void*);
		const size_type nUnusedPtrCountAtBack  = (mnPtrArraySize - nUnusedPtrCountAtFront) - nUsedPtrCount;
		value_type**    pPtrArrayBegin;

		if((allocationSide == kSideBack) && (nAdditionalCapacity <= nUnusedPtrCountAtFront))  // If we can take advantage of unused pointers at the front without doing any reallocation...
		{
			if(nAdditionalCapacity < (nUnusedPtrCountAtFront / 2))  // Possibly use more space than required, if there's a lot of extra space.
				nAdditionalCapacity = (nUnusedPtrCountAtFront / 2);

			pPtrArrayBegin = mpPtrArray + (nUnusedPtrCountAtFront - nAdditionalCapacity);
			memmove(pPtrArrayBegin, mItBegin.mpCurrentArrayPtr, nUsedPtrSpace);

			#if EASTL_DEBUG
				memset(pPtrArrayBegin + nUsedPtrCount, 0, (size_t)(mpPtrArray + mnPtrArraySize) - (size_t)(pPtrArrayBegin + nUsedPtrCount));
			#endif
		}
		else if((allocationSide == kSideFront) && (nAdditionalCapacity <= nUnusedPtrCountAtBack)) // If we can take advantage of unused pointers at the back without doing any reallocation...
		{
			if(nAdditionalCapacity < (nUnusedPtrCountAtBack / 2))  // Possibly use more space than required, if there's a lot of extra space.
				nAdditionalCapacity = (nUnusedPtrCountAtBack / 2);

			pPtrArrayBegin = mItBegin.mpCurrentArrayPtr + nAdditionalCapacity;
			memmove(pPtrArrayBegin, mItBegin.mpCurrentArrayPtr, nUsedPtrSpace);

			#if EASTL_DEBUG
				memset(mpPtrArray, 0, (size_t)((uintptr_t)pPtrArrayBegin - (uintptr_t)mpPtrArray));
			#endif
		}
		else
		{
			// In this case we will have to do a reallocation.
			const size_type    nNewPtrArraySize = mnPtrArraySize + eastl::max_alt(mnPtrArraySize, nAdditionalCapacity) + 2;  // Allocate extra capacity.
			value_type** const pNewPtrArray     = DoAllocatePtrArray(nNewPtrArraySize);

			pPtrArrayBegin = pNewPtrArray + (mItBegin.mpCurrentArrayPtr - mpPtrArray) + ((allocationSide == kSideFront) ? nAdditionalCapacity : 0);

			// The following is equivalent to: eastl::copy(mItBegin.mpCurrentArrayPtr, mItEnd.mpCurrentArrayPtr + 1, pPtrArrayBegin);
			// It's OK to use memcpy instead of memmove because the destination is guaranteed to non-overlap the source.
			if(mpPtrArray) // Could also say: 'if(mItBegin.mpCurrentArrayPtr)' 
				memcpy(pPtrArrayBegin, mItBegin.mpCurrentArrayPtr, nUsedPtrSpace);

			DoFreePtrArray(mpPtrArray, mnPtrArraySize);

			mpPtrArray     = pNewPtrArray;
			mnPtrArraySize = nNewPtrArraySize;
		}

		// We need to reset the begin and end iterators, as code that calls this expects them to *not* be invalidated.
		mItBegin.SetSubarray(pPtrArrayBegin);
		mItEnd.SetSubarray((pPtrArrayBegin + nUsedPtrCount) - 1);
	}
	

	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void DequeBase<T, Allocator, kDequeSubarraySize>::DoInit(size_type n)
	{
		// This code is disabled because it doesn't currently work properly.
		// We are trying to make it so that a deque can have a zero allocation 
		// initial empty state, but we (OK, I) am having a hard time making
		// this elegant and efficient. 
		//if(n)
		//{
			const size_type nNewPtrArraySize = (size_type)((n / kDequeSubarraySize) + 1); // Always have at least one, even if n is zero.
			const size_type kMinPtrArraySize_ = kMinPtrArraySize;

			mnPtrArraySize = eastl::max_alt(kMinPtrArraySize_, (nNewPtrArraySize + 2)); 
			mpPtrArray     = DoAllocatePtrArray(mnPtrArraySize);

			value_type** const pPtrArrayBegin   = (mpPtrArray + ((mnPtrArraySize - nNewPtrArraySize) / 2)); // Try to place it in the middle.
			value_type** const pPtrArrayEnd     = pPtrArrayBegin + nNewPtrArraySize;
			value_type**       pPtrArrayCurrent = pPtrArrayBegin;

			#if EASTL_EXCEPTIONS_ENABLED
				try
				{
					try
					{
			#endif
						while(pPtrArrayCurrent < pPtrArrayEnd)
							*pPtrArrayCurrent++ = DoAllocateSubarray();
			#if EASTL_EXCEPTIONS_ENABLED
					}
					catch(...)
					{
						DoFreeSubarrays(pPtrArrayBegin, pPtrArrayCurrent);
						throw;
					}
				}
				catch(...)
				{
					DoFreePtrArray(mpPtrArray, mnPtrArraySize);
					mpPtrArray     = NULL;
					mnPtrArraySize = 0;
					throw;
				}
			#endif

			mItBegin.SetSubarray(pPtrArrayBegin);
			mItBegin.mpCurrent = mItBegin.mpBegin;

			mItEnd.SetSubarray(pPtrArrayEnd - 1);
			mItEnd.mpCurrent = mItEnd.mpBegin + (difference_type)(n % kDequeSubarraySize);
		//}
		//else // Else we do a zero-allocation initialization.
		//{
		//    mpPtrArray     = NULL;
		//    mnPtrArraySize = 0;
		//
		//    mItBegin.mpCurrentArrayPtr = NULL;
		//    mItBegin.mpBegin           = NULL;
		//    mItBegin.mpEnd             = NULL; // We intentionally create a situation whereby the subarray that has no capacity.
		//    mItBegin.mpCurrent         = NULL;
		//
		//    mItEnd = mItBegin;
		//}
	}



	///////////////////////////////////////////////////////////////////////
	// DequeIterator
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::DequeIterator()
		: mpCurrent(NULL), mpBegin(NULL), mpEnd(NULL), mpCurrentArrayPtr(NULL)
	{
		// Empty
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::DequeIterator(T** pCurrentArrayPtr, T* pCurrent)
		: mpCurrent(pCurrent), mpBegin(*pCurrentArrayPtr), mpEnd(pCurrent + kDequeSubarraySize), mpCurrentArrayPtr(pCurrentArrayPtr)
	{
		// Empty
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::DequeIterator(const iterator& x)
		: mpCurrent(x.mpCurrent), mpBegin(x.mpBegin), mpEnd(x.mpEnd), mpCurrentArrayPtr(x.mpCurrentArrayPtr)
	{
		// Empty
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::DequeIterator(const iterator& x, Increment)
		: mpCurrent(x.mpCurrent), mpBegin(x.mpBegin), mpEnd(x.mpEnd), mpCurrentArrayPtr(x.mpCurrentArrayPtr)
	{
		operator++();
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::DequeIterator(const iterator& x, Decrement)
		: mpCurrent(x.mpCurrent), mpBegin(x.mpBegin), mpEnd(x.mpEnd), mpCurrentArrayPtr(x.mpCurrentArrayPtr)
	{
		operator--();
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	typename DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::pointer
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::operator->() const
	{
		return mpCurrent;
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	typename DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::reference
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::operator*() const
	{
		return *mpCurrent;
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	typename DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::this_type&
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::operator++()
	{
		if(EASTL_UNLIKELY(++mpCurrent == mpEnd))
		{
			mpBegin   = *++mpCurrentArrayPtr;
			mpEnd     = mpBegin + kDequeSubarraySize;
			mpCurrent = mpBegin;
		}
		return *this;
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	typename DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::this_type
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::operator++(int)
	{
		const this_type temp(*this);
		operator++();
		return temp;
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	typename DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::this_type&
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::operator--()
	{
		if(EASTL_UNLIKELY(mpCurrent == mpBegin))
		{
			mpBegin   = *--mpCurrentArrayPtr;
			mpEnd     = mpBegin + kDequeSubarraySize;
			mpCurrent = mpEnd; // fall through...
		}
		--mpCurrent;
		return *this;
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	typename DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::this_type
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::operator--(int)
	{
		const this_type temp(*this);
		operator--();
		return temp;
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	typename DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::this_type&
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::operator+=(difference_type n)
	{
		const difference_type subarrayPosition = (mpCurrent - mpBegin) + n;

		// Cast from signed to unsigned (size_t) in order to obviate the need to compare to < 0. 
		if((size_t)subarrayPosition < (size_t)kDequeSubarraySize) // If the new position is within the current subarray (i.e. >= 0 && < kSubArraySize)...
			mpCurrent += n;
		else
		{
			// This implementation is a branchless version which works by offsetting 
			// the math to always be in the positive range. Much of the values here
			// reduce to constants and both the multiplication and division are of 
			// power of two sizes and so this calculation ends up compiling down to 
			// just one addition, one shift and one subtraction. This algorithm has 
			// a theoretical weakness in that on 32 bit systems it will fail if the 
			// value of n is >= (2^32 - 2^24) or 4,278,190,080 of if kDequeSubarraySize
			// is >= 2^24 or 16,777,216.
			EASTL_CT_ASSERT((kDequeSubarraySize & (kDequeSubarraySize - 1)) == 0); // Verify that it is a power of 2.
			const difference_type subarrayIndex = (((16777216 + subarrayPosition) / (difference_type)kDequeSubarraySize)) - (16777216 / (difference_type)kDequeSubarraySize);

			SetSubarray(mpCurrentArrayPtr + subarrayIndex);
			mpCurrent = mpBegin + (subarrayPosition - (subarrayIndex * (difference_type)kDequeSubarraySize));
		}
		return *this;
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	typename DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::this_type&
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::operator-=(difference_type n)
	{
		return (*this).operator+=(-n);
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	typename DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::this_type
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::operator+(difference_type n) const
	{
		return this_type(*this).operator+=(n);
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	typename DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::this_type
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::operator-(difference_type n) const
	{
		return this_type(*this).operator+=(-n);
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	typename DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::this_type
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::copy(const iterator& first, const iterator& last, true_type)
	{
		// To do: Implement this as a loop which does memcpys between subarrays appropriately.
		//        Currently we only do memcpy if the entire operation occurs within a single subarray.
		if((first.mpBegin == last.mpBegin) && (first.mpBegin == mpBegin)) // If all operations are within the same subarray, implement the operation as a memmove.
		{
			memmove(mpCurrent, first.mpCurrent, (size_t)((uintptr_t)last.mpCurrent - (uintptr_t)first.mpCurrent));
			return *this + (last.mpCurrent - first.mpCurrent);
		}
		return eastl::copy(eastl::make_move_iterator(first), eastl::make_move_iterator(last), eastl::make_move_iterator(*this)).base();
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	typename DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::this_type
	DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::copy(const iterator& first, const iterator& last, false_type)
	{
		return eastl::copy(eastl::make_move_iterator(first), eastl::make_move_iterator(last), eastl::make_move_iterator(*this)).base();
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	void DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::copy_backward(const iterator& first, const iterator& last, true_type)
	{
		// To do: Implement this as a loop which does memmoves between subarrays appropriately.
		//        Currently we only do memcpy if the entire operation occurs within a single subarray.
		if((first.mpBegin == last.mpBegin) && (first.mpBegin == mpBegin)) // If all operations are within the same subarray, implement the operation as a memcpy.
			memmove(mpCurrent - (last.mpCurrent - first.mpCurrent), first.mpCurrent, (size_t)((uintptr_t)last.mpCurrent - (uintptr_t)first.mpCurrent));
		else
			eastl::copy_backward(eastl::make_move_iterator(first), eastl::make_move_iterator(last), eastl::make_move_iterator(*this));
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	void DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::copy_backward(const iterator& first, const iterator& last, false_type)
	{
		eastl::copy_backward(eastl::make_move_iterator(first), eastl::make_move_iterator(last), eastl::make_move_iterator(*this)).base();
	}


	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	void DequeIterator<T, Pointer, Reference, kDequeSubarraySize>::SetSubarray(T** pCurrentArrayPtr)
	{
		mpCurrentArrayPtr = pCurrentArrayPtr;
		mpBegin           = *pCurrentArrayPtr;
		mpEnd             = mpBegin + kDequeSubarraySize;
	}


	// The C++ defect report #179 requires that we support comparisons between const and non-const iterators.
	// Thus we provide additional template paremeters here to support this. The defect report does not
	// require us to support comparisons between reverse_iterators and const_reverse_iterators.
	template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB, unsigned kDequeSubarraySize>
	inline bool operator==(const DequeIterator<T, PointerA, ReferenceA, kDequeSubarraySize>& a, 
						   const DequeIterator<T, PointerB, ReferenceB, kDequeSubarraySize>& b)
	{
		return a.mpCurrent == b.mpCurrent;
	}


	template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB, unsigned kDequeSubarraySize>
	inline bool operator!=(const DequeIterator<T, PointerA, ReferenceA, kDequeSubarraySize>& a, 
						   const DequeIterator<T, PointerB, ReferenceB, kDequeSubarraySize>& b)
	{
		return a.mpCurrent != b.mpCurrent;
	}


	// We provide a version of operator!= for the case where the iterators are of the 
	// same type. This helps prevent ambiguity errors in the presence of rel_ops.
	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	inline bool operator!=(const DequeIterator<T, Pointer, Reference, kDequeSubarraySize>& a, 
						   const DequeIterator<T, Pointer, Reference, kDequeSubarraySize>& b)
	{
		return a.mpCurrent != b.mpCurrent;
	}


	template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB, unsigned kDequeSubarraySize>
	inline bool operator<(const DequeIterator<T, PointerA, ReferenceA, kDequeSubarraySize>& a, 
						  const DequeIterator<T, PointerB, ReferenceB, kDequeSubarraySize>& b)
	{
		return (a.mpCurrentArrayPtr == b.mpCurrentArrayPtr) ? (a.mpCurrent < b.mpCurrent) : (a.mpCurrentArrayPtr < b.mpCurrentArrayPtr);
	}


	template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB, unsigned kDequeSubarraySize>
	inline bool operator>(const DequeIterator<T, PointerA, ReferenceA, kDequeSubarraySize>& a, 
						  const DequeIterator<T, PointerB, ReferenceB, kDequeSubarraySize>& b)
	{
		return (a.mpCurrentArrayPtr == b.mpCurrentArrayPtr) ? (a.mpCurrent > b.mpCurrent) : (a.mpCurrentArrayPtr > b.mpCurrentArrayPtr);
	}


	template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB, unsigned kDequeSubarraySize>
	inline bool operator<=(const DequeIterator<T, PointerA, ReferenceA, kDequeSubarraySize>& a, 
						   const DequeIterator<T, PointerB, ReferenceB, kDequeSubarraySize>& b)
	{
		return (a.mpCurrentArrayPtr == b.mpCurrentArrayPtr) ? (a.mpCurrent <= b.mpCurrent) : (a.mpCurrentArrayPtr <= b.mpCurrentArrayPtr);
	}


	template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB, unsigned kDequeSubarraySize>
	inline bool operator>=(const DequeIterator<T, PointerA, ReferenceA, kDequeSubarraySize>& a, 
						   const DequeIterator<T, PointerB, ReferenceB, kDequeSubarraySize>& b)
	{
		return (a.mpCurrentArrayPtr == b.mpCurrentArrayPtr) ? (a.mpCurrent >= b.mpCurrent) : (a.mpCurrentArrayPtr >= b.mpCurrentArrayPtr);
	}


	// Random access iterators must support operator + and operator -.
	// You can only add an integer to an iterator, and you cannot add two iterators.
	template <typename T, typename Pointer, typename Reference, unsigned kDequeSubarraySize>
	inline DequeIterator<T, Pointer, Reference, kDequeSubarraySize>
	operator+(ptrdiff_t n, const DequeIterator<T, Pointer, Reference, kDequeSubarraySize>& x)
	{
		return x + n; // Implement (n + x) in terms of (x + n).
	}


	// You can only add an integer to an iterator, but you can subtract two iterators.
	// The C++ defect report #179 mentioned above specifically refers to 
	// operator - and states that we support the subtraction of const and non-const iterators.
	template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB, unsigned kDequeSubarraySize>
	inline typename DequeIterator<T, PointerA, ReferenceA, kDequeSubarraySize>::difference_type
	operator-(const DequeIterator<T, PointerA, ReferenceA, kDequeSubarraySize>& a,
			  const DequeIterator<T, PointerB, ReferenceB, kDequeSubarraySize>& b)
	{
		// This is a fairly clever algorithm that has been used in STL deque implementations since the original HP STL:
		typedef typename DequeIterator<T, PointerA, ReferenceA, kDequeSubarraySize>::difference_type difference_type;

		return ((difference_type)kDequeSubarraySize * ((a.mpCurrentArrayPtr - b.mpCurrentArrayPtr) - 1)) + (a.mpCurrent - a.mpBegin) + (b.mpEnd - b.mpCurrent);
	}




	///////////////////////////////////////////////////////////////////////
	// deque
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline deque<T, Allocator, kDequeSubarraySize>::deque()
		: base_type((size_type)0)
	{
		// Empty
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline deque<T, Allocator, kDequeSubarraySize>::deque(const allocator_type& allocator)
		: base_type((size_type)0, allocator)
	{
		// Empty
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline deque<T, Allocator, kDequeSubarraySize>::deque(size_type n, const allocator_type& allocator)
		: base_type(n, allocator)
	{
		DoFillInit(value_type());
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline deque<T, Allocator, kDequeSubarraySize>::deque(size_type n, const value_type& value, const allocator_type& allocator)
		: base_type(n, allocator)
	{
		DoFillInit(value);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline deque<T, Allocator, kDequeSubarraySize>::deque(const this_type& x)
		: base_type(x.size(), x.mAllocator)
	{
		eastl::uninitialized_copy(x.mItBegin, x.mItEnd, mItBegin);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline deque<T, Allocator, kDequeSubarraySize>::deque(this_type&& x)
	  : base_type((size_type)0, x.mAllocator)
	{
		swap(x);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline deque<T, Allocator, kDequeSubarraySize>::deque(this_type&& x, const allocator_type& allocator)
	  : base_type((size_type)0, allocator)
	{
		swap(x); // member swap handles the case that x has a different allocator than our allocator by doing a copy.
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline deque<T, Allocator, kDequeSubarraySize>::deque(std::initializer_list<value_type> ilist, const allocator_type& allocator)
		: base_type(allocator)
	{
		DoInit(ilist.begin(), ilist.end(), false_type());
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	template <typename InputIterator>
	inline deque<T, Allocator, kDequeSubarraySize>::deque(InputIterator first, InputIterator last)
		: base_type(EASTL_DEQUE_DEFAULT_ALLOCATOR) // Call the empty base constructor, which does nothing. We need to do all the work in our own DoInit.
	{
		DoInit(first, last, is_integral<InputIterator>());
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline deque<T, Allocator, kDequeSubarraySize>::~deque()
	{
		// Call destructors. Parent class will free the memory.
		for(iterator itCurrent(mItBegin); itCurrent != mItEnd; ++itCurrent)
			itCurrent.mpCurrent->~value_type();
	} 


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::this_type& 
	deque<T, Allocator, kDequeSubarraySize>::operator=(const this_type& x)
	{
		if(&x != this) // If not assigning to ourselves...
		{
			// If (EASTL_ALLOCATOR_COPY_ENABLED == 1) and the current contents are allocated by an 
			// allocator that's unequal to x's allocator, we need to reallocate our elements with 
			// our current allocator and reallocate it with x's allocator. If the allocators are 
			// equal then we can use a more optimal algorithm that doesn't reallocate our elements
			// but instead can copy them in place.

			#if EASTL_ALLOCATOR_COPY_ENABLED
				bool bSlowerPathwayRequired = (mAllocator != x.mAllocator);
			#else
				bool bSlowerPathwayRequired = false;
			#endif

			if(bSlowerPathwayRequired)
			{
				// We can't currently use set_capacity(0) or shrink_to_fit, because they 
				// leave a remaining allocation with our old allocator. So we do a similar 
				// thing but set our allocator to x.mAllocator while doing so.
				this_type temp(x.mAllocator);
				DoSwap(temp);
				// Now we have an empty container with an allocator equal to x.mAllocator, ready to assign from x.
			}

			DoAssign(x.begin(), x.end(), eastl::false_type());
		}

		return *this;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline typename deque<T, Allocator, kDequeSubarraySize>::this_type& 
	deque<T, Allocator, kDequeSubarraySize>::operator=(this_type&& x)
	{
		if(this != &x)
		{
			set_capacity(0); // To consider: Are we really required to clear here? x is going away soon and will clear itself in its dtor.
			swap(x);         // member swap handles the case that x has a different allocator than our allocator by doing a copy.
		}
		return *this; 
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline typename deque<T, Allocator, kDequeSubarraySize>::this_type& 
	deque<T, Allocator, kDequeSubarraySize>::operator=(std::initializer_list<value_type> ilist)
	{
		DoAssign(ilist.begin(), ilist.end(), false_type());
		return *this;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline void deque<T, Allocator, kDequeSubarraySize>::assign(size_type n, const value_type& value)
	{
		DoAssignValues(n, value);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline void deque<T, Allocator, kDequeSubarraySize>::assign(std::initializer_list<value_type> ilist)
	{
		DoAssign(ilist.begin(), ilist.end(), false_type());
	}


	// It turns out that the C++ std::deque specifies a two argument
	// version of assign that takes (int size, int value). These are not
	// iterators, so we need to do a template compiler trick to do the right thing. 
	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	template <typename InputIterator>
	inline void deque<T, Allocator, kDequeSubarraySize>::assign(InputIterator first, InputIterator last)
	{
		DoAssign(first, last, is_integral<InputIterator>());
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline typename deque<T, Allocator, kDequeSubarraySize>::iterator 
	deque<T, Allocator, kDequeSubarraySize>::begin() EA_NOEXCEPT
	{
		return mItBegin;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline typename deque<T, Allocator, kDequeSubarraySize>::const_iterator 
	deque<T, Allocator, kDequeSubarraySize>::begin() const EA_NOEXCEPT
	{
		return mItBegin;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline typename deque<T, Allocator, kDequeSubarraySize>::const_iterator 
	deque<T, Allocator, kDequeSubarraySize>::cbegin() const EA_NOEXCEPT
	{
		return mItBegin;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline typename deque<T, Allocator, kDequeSubarraySize>::iterator 
	deque<T, Allocator, kDequeSubarraySize>::end() EA_NOEXCEPT
	{
		return mItEnd;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::const_iterator
	deque<T, Allocator, kDequeSubarraySize>::end() const EA_NOEXCEPT
	{
		return mItEnd;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline typename deque<T, Allocator, kDequeSubarraySize>::const_iterator
	deque<T, Allocator, kDequeSubarraySize>::cend() const EA_NOEXCEPT
	{
		return mItEnd;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline typename deque<T, Allocator, kDequeSubarraySize>::reverse_iterator
	deque<T, Allocator, kDequeSubarraySize>::rbegin() EA_NOEXCEPT
	{
		return reverse_iterator(mItEnd);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline typename deque<T, Allocator, kDequeSubarraySize>::const_reverse_iterator
	deque<T, Allocator, kDequeSubarraySize>::rbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(mItEnd);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline typename deque<T, Allocator, kDequeSubarraySize>::const_reverse_iterator
	deque<T, Allocator, kDequeSubarraySize>::crbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(mItEnd);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline typename deque<T, Allocator, kDequeSubarraySize>::reverse_iterator
	deque<T, Allocator, kDequeSubarraySize>::rend() EA_NOEXCEPT
	{
		return reverse_iterator(mItBegin);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline typename deque<T, Allocator, kDequeSubarraySize>::const_reverse_iterator
	deque<T, Allocator, kDequeSubarraySize>::rend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(mItBegin);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline typename deque<T, Allocator, kDequeSubarraySize>::const_reverse_iterator
	deque<T, Allocator, kDequeSubarraySize>::crend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(mItBegin);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline bool deque<T, Allocator, kDequeSubarraySize>::empty() const EA_NOEXCEPT
	{
		return mItBegin.mpCurrent == mItEnd.mpCurrent;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::size_type
	inline deque<T, Allocator, kDequeSubarraySize>::size() const EA_NOEXCEPT
	{
		return (size_type)(mItEnd - mItBegin);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline void deque<T, Allocator, kDequeSubarraySize>::resize(size_type n, const value_type& value)
	{
		const size_type nSizeCurrent = size();

		if(n > nSizeCurrent) // We expect that more often than not, resizes will be upsizes.
			insert(mItEnd, n - nSizeCurrent, value);
		else
			erase(mItBegin + (difference_type)n, mItEnd);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline void deque<T, Allocator, kDequeSubarraySize>::resize(size_type n)
	{
		resize(n, value_type());
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline void deque<T, Allocator, kDequeSubarraySize>::shrink_to_fit()
	{
		this_type x(eastl::make_move_iterator(begin()), eastl::make_move_iterator(end()));
		swap(x);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline void deque<T, Allocator, kDequeSubarraySize>::set_capacity(size_type n)
	{
		// Currently there isn't a way to remove all allocations from a deque, as it 
		// requires a single starting allocation for the subarrays. So we can't just
		// free all memory without leaving it in a bad state. So the best means of 
		// implementing set_capacity() is to do what we do below.

		if(n == 0)
		{
			this_type temp(mAllocator);
			DoSwap(temp);
		}
		else if(n < size())
		{
			// We currently ignore the request to reduce capacity. To do: Implement this
			// and do it in a way that doesn't result in temporarily ~doubling our memory usage.
			// That might involve trimming unused subarrays from the front or back of 
			// the container.
			resize(n);
		}        
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::reference
	deque<T, Allocator, kDequeSubarraySize>::operator[](size_type n)
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY(n >= (size_type)(mItEnd - mItBegin)))
				EASTL_FAIL_MSG("deque::operator[] -- out of range");
		#elif EASTL_ASSERT_ENABLED
			// We allow taking a reference to deque[0]
			if (EASTL_UNLIKELY((n != 0) && n >= (size_type)(mItEnd - mItBegin)))
				EASTL_FAIL_MSG("deque::operator[] -- out of range");
		#endif

		// See DequeIterator::operator+=() for an explanation of the code below.
		iterator it(mItBegin);

		const difference_type subarrayPosition = (difference_type)((it.mpCurrent - it.mpBegin) + (difference_type)n);
		const difference_type subarrayIndex    = (((16777216 + subarrayPosition) / (difference_type)kDequeSubarraySize)) - (16777216 / (difference_type)kDequeSubarraySize);

		return *(*(it.mpCurrentArrayPtr + subarrayIndex) + (subarrayPosition - (subarrayIndex * (difference_type)kDequeSubarraySize)));
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::const_reference
	deque<T, Allocator, kDequeSubarraySize>::operator[](size_type n) const
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY(n >= (size_type)(mItEnd - mItBegin)))
				EASTL_FAIL_MSG("deque::operator[] -- out of range");
		#elif EASTL_ASSERT_ENABLED
			// We allow the user to use a reference to deque[0] of an empty container.
			if (EASTL_UNLIKELY((n != 0) && n >= (size_type)(mItEnd - mItBegin)))
				EASTL_FAIL_MSG("deque::operator[] -- out of range");
		#endif

		// See DequeIterator::operator+=() for an explanation of the code below.
		iterator it(mItBegin);

		const difference_type subarrayPosition = (it.mpCurrent - it.mpBegin) + (difference_type)n;
		const difference_type subarrayIndex    = (((16777216 + subarrayPosition) / (difference_type)kDequeSubarraySize)) - (16777216 / (difference_type)kDequeSubarraySize);

		return *(*(it.mpCurrentArrayPtr + subarrayIndex) + (subarrayPosition - (subarrayIndex * (difference_type)kDequeSubarraySize)));
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::reference
	deque<T, Allocator, kDequeSubarraySize>::at(size_type n)
	{
		#if EASTL_EXCEPTIONS_ENABLED
			if(n >= (size_type)(mItEnd - mItBegin))
				throw std::out_of_range("deque::at -- out of range");
		#elif EASTL_ASSERT_ENABLED
			if(n >= (size_type)(mItEnd - mItBegin))
				EASTL_FAIL_MSG("deque::at -- out of range");
		#endif
		return *(mItBegin.operator+((difference_type)n));
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::const_reference
	deque<T, Allocator, kDequeSubarraySize>::at(size_type n) const
	{
		#if EASTL_EXCEPTIONS_ENABLED
			if(n >= (size_type)(mItEnd - mItBegin))
				throw std::out_of_range("deque::at -- out of range");
		#elif EASTL_ASSERT_ENABLED
			if(n >= (size_type)(mItEnd - mItBegin))
				EASTL_FAIL_MSG("deque::at -- out of range");
		#endif
		return *(mItBegin.operator+((difference_type)n));
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::reference
	deque<T, Allocator, kDequeSubarraySize>::front()
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY((size_type)(mItEnd == mItBegin)))
				EASTL_FAIL_MSG("deque::front -- empty deque");
		#else
			// We allow the user to reference an empty container.
		#endif

		return *mItBegin;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::const_reference
	deque<T, Allocator, kDequeSubarraySize>::front() const
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY((size_type)(mItEnd == mItBegin)))
				EASTL_FAIL_MSG("deque::front -- empty deque");
		#else
			// We allow the user to reference an empty container.
		#endif

		return *mItBegin;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::reference
	deque<T, Allocator, kDequeSubarraySize>::back()
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY((size_type)(mItEnd == mItBegin)))
				EASTL_FAIL_MSG("deque::back -- empty deque");
		#else
			// We allow the user to reference an empty container.
		#endif

		return *iterator(mItEnd, typename iterator::Decrement());
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::const_reference
	deque<T, Allocator, kDequeSubarraySize>::back() const
	{
		#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			if (EASTL_UNLIKELY((size_type)(mItEnd == mItBegin)))
				EASTL_FAIL_MSG("deque::back -- empty deque");
		#else
			// We allow the user to reference an empty container.
		#endif

		return *iterator(mItEnd, typename iterator::Decrement());
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void deque<T, Allocator, kDequeSubarraySize>::push_front(const value_type& value)
	{
		emplace_front(value);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void deque<T, Allocator, kDequeSubarraySize>::push_front(value_type&& value)
	{
		emplace_front(eastl::move(value));
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::reference
	deque<T, Allocator, kDequeSubarraySize>::push_front()
	{
		emplace_front(value_type());
		return *mItBegin;   // Same as return front();
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void deque<T, Allocator, kDequeSubarraySize>::push_back(const value_type& value)
	{
		emplace_back(value);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void deque<T, Allocator, kDequeSubarraySize>::push_back(value_type&& value)
	{
		emplace_back(eastl::move(value));
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::reference
	deque<T, Allocator, kDequeSubarraySize>::push_back()
	{
		emplace_back(value_type());
		return *iterator(mItEnd, typename iterator::Decrement()); // Same thing as return back();
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void deque<T, Allocator, kDequeSubarraySize>::pop_front()
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY((size_type)(mItEnd == mItBegin)))
				EASTL_FAIL_MSG("deque::pop_front -- empty deque");
		#endif

		if((mItBegin.mpCurrent + 1) != mItBegin.mpEnd) // If the operation is very simple...
			(mItBegin.mpCurrent++)->~value_type();
		else
		{
			// This is executed only when we are popping the end (last) item off the front-most subarray.
			// In this case we need to free the subarray and point mItBegin to the next subarray.
			#ifdef EA_DEBUG
				value_type** pp = mItBegin.mpCurrentArrayPtr;
			#endif

			mItBegin.mpCurrent->~value_type(); // mpCurrent == mpEnd - 1
			DoFreeSubarray(mItBegin.mpBegin);
			mItBegin.SetSubarray(mItBegin.mpCurrentArrayPtr + 1);
			mItBegin.mpCurrent = mItBegin.mpBegin;

			#ifdef EA_DEBUG
				*pp = NULL;
			#endif
		}
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void deque<T, Allocator, kDequeSubarraySize>::pop_back()
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY((size_type)(mItEnd == mItBegin)))
				EASTL_FAIL_MSG("deque::pop_back -- empty deque");
		#endif

		if(mItEnd.mpCurrent != mItEnd.mpBegin) // If the operation is very simple...
			(--mItEnd.mpCurrent)->~value_type();
		else
		{
			// This is executed only when we are popping the first item off the last subarray.
			// In this case we need to free the subarray and point mItEnd to the previous subarray.
			#ifdef EA_DEBUG
				value_type** pp = mItEnd.mpCurrentArrayPtr;
			#endif

			DoFreeSubarray(mItEnd.mpBegin);
			mItEnd.SetSubarray(mItEnd.mpCurrentArrayPtr - 1);
			mItEnd.mpCurrent = mItEnd.mpEnd - 1;        // Recall that mItEnd points to one-past the last item in the container.
			mItEnd.mpCurrent->~value_type();            // Thus we need to call the destructor on the item *before* that last item.

			#ifdef EA_DEBUG
				*pp = NULL;
			#endif
		}
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	template<class... Args>
	typename deque<T, Allocator, kDequeSubarraySize>::iterator
	deque<T, Allocator, kDequeSubarraySize>::emplace(const_iterator position, Args&&... args)
	{
		if(EASTL_UNLIKELY(position.mpCurrent == mItEnd.mpCurrent)) // If we are doing the same thing as push_back...
		{
			emplace_back(eastl::forward<Args>(args)...);
			return iterator(mItEnd, typename iterator::Decrement()); // Unfortunately, we need to make an iterator here, as the above push_back is an operation that can invalidate existing iterators.
		}
		else if(EASTL_UNLIKELY(position.mpCurrent == mItBegin.mpCurrent)) // If we are doing the same thing as push_front...
		{
			emplace_front(eastl::forward<Args>(args)...);
			return mItBegin;
		}

		iterator              itPosition(position, typename iterator::FromConst());
		value_type  valueSaved(eastl::forward<Args>(args)...); // We need to save this because value may come from within our container. It would be somewhat tedious to make a workaround that could avoid this.
		const difference_type i(itPosition - mItBegin);

		#if EASTL_ASSERT_ENABLED
			EASTL_ASSERT(!empty()); // The push_front and push_back calls below assume that we are non-empty. It turns out this is never called unless so.

			if(EASTL_UNLIKELY(!(validate_iterator(itPosition) & isf_valid)))
				EASTL_FAIL_MSG("deque::emplace -- invalid iterator");
		#endif

		if(i < (difference_type)(size() / 2)) // Should we insert at the front or at the back? We divide the range in half.
		{
			emplace_front(eastl::move(*mItBegin)); // This operation potentially invalidates all existing iterators and so we need to assign them anew relative to mItBegin below.

			itPosition = mItBegin + i;

			const iterator newPosition  (itPosition, typename iterator::Increment());
				  iterator oldBegin     (mItBegin,   typename iterator::Increment());
			const iterator oldBeginPlus1(oldBegin,   typename iterator::Increment());

			oldBegin.copy(oldBeginPlus1, newPosition, eastl::has_trivial_relocate<value_type>());
		}
		else
		{
			emplace_back(eastl::move(*iterator(mItEnd, typename iterator::Decrement())));

			itPosition = mItBegin + i;

				  iterator oldBack      (mItEnd,  typename iterator::Decrement());
			const iterator oldBackMinus1(oldBack, typename iterator::Decrement());

			oldBack.copy_backward(itPosition, oldBackMinus1, eastl::has_trivial_relocate<value_type>());
		}

		*itPosition = eastl::move(valueSaved);

		return itPosition;
	}

	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	template<class... Args>
	void deque<T, Allocator, kDequeSubarraySize>::emplace_front(Args&&... args)
	{
		if(mItBegin.mpCurrent != mItBegin.mpBegin)                                         // If we have room in the first subarray... we hope that usually this 'new' pathway gets executed, as it is slightly faster.
			::new((void*)--mItBegin.mpCurrent) value_type(eastl::forward<Args>(args)...);  // Construct in place. If args is a single arg of type value_type&& then it this will be a move construction.
		else
		{
			// To consider: Detect if value isn't coming from within this container and handle that efficiently.
			value_type  valueSaved(eastl::forward<Args>(args)...);                          // We need to make a temporary, because args may be a value_type that comes from within our container and the operations below may change the container. But we can use move instead of copy.

			if(mItBegin.mpCurrentArrayPtr == mpPtrArray)                                   // If there are no more pointers in front of the current (first) one...
				DoReallocPtrArray(1, kSideFront);

			mItBegin.mpCurrentArrayPtr[-1] = DoAllocateSubarray();

			#if EASTL_EXCEPTIONS_ENABLED
				try
				{
			#endif
					mItBegin.SetSubarray(mItBegin.mpCurrentArrayPtr - 1);
					mItBegin.mpCurrent = mItBegin.mpEnd - 1;
					::new((void*)mItBegin.mpCurrent) value_type(eastl::move(valueSaved));
			#if EASTL_EXCEPTIONS_ENABLED
				}
				catch(...)
				{
					++mItBegin; // The exception could only occur in the new operation above, after we have incremented mItBegin. So we need to undo it.
					DoFreeSubarray(mItBegin.mpCurrentArrayPtr[-1]);
					throw;
				}
			#endif
		}
	}

	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	template<class... Args>
	void deque<T, Allocator, kDequeSubarraySize>::emplace_back(Args&&... args)
	{
		if((mItEnd.mpCurrent + 1) != mItEnd.mpEnd)                                       // If we have room in the last subarray... we hope that usually this 'new' pathway gets executed, as it is slightly faster.
			::new((void*)mItEnd.mpCurrent++) value_type(eastl::forward<Args>(args)...);  // Construct in place. If args is a single arg of type value_type&& then it this will be a move construction.
		else
		{
			// To consider: Detect if value isn't coming from within this container and handle that efficiently.
			value_type  valueSaved(eastl::forward<Args>(args)...);                          // We need to make a temporary, because args may be a value_type that comes from within our container and the operations below may change the container. But we can use move instead of copy.
			if(((mItEnd.mpCurrentArrayPtr - mpPtrArray) + 1) >= (difference_type)mnPtrArraySize) // If there are no more pointers after the current (last) one.
				DoReallocPtrArray(1, kSideBack);

			mItEnd.mpCurrentArrayPtr[1] = DoAllocateSubarray();

			#if EASTL_EXCEPTIONS_ENABLED
				try
				{
			#endif
					::new((void*)mItEnd.mpCurrent) value_type(eastl::move(valueSaved)); // We can move valueSaved into position.
					mItEnd.SetSubarray(mItEnd.mpCurrentArrayPtr + 1);
					mItEnd.mpCurrent = mItEnd.mpBegin;
			#if EASTL_EXCEPTIONS_ENABLED
				}
				catch(...)
				{
					// No need to execute '--mItEnd', as the exception could only occur in the new operation above before we set mItEnd.
					DoFreeSubarray(mItEnd.mpCurrentArrayPtr[1]);
					throw;
				}
			#endif
		}
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::iterator
	deque<T, Allocator, kDequeSubarraySize>::insert(const_iterator position, const value_type& value)
	{
		return emplace(position, value);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::iterator
	deque<T, Allocator, kDequeSubarraySize>::insert(const_iterator position, value_type&& value)
	{
		return emplace(position, eastl::move(value));
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void deque<T, Allocator, kDequeSubarraySize>::insert(const_iterator position, size_type n, const value_type& value)
	{
		DoInsertValues(position, n, value);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	template <typename InputIterator>
	void deque<T, Allocator, kDequeSubarraySize>::insert(const_iterator position, InputIterator first, InputIterator last)
	{
		DoInsert(position, first, last, is_integral<InputIterator>()); // The C++ standard requires this sort of behaviour, as InputIterator might actually be Integer and 'first' is really 'count' and 'last' is really 'value'.
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::iterator
	deque<T, Allocator, kDequeSubarraySize>::insert(const_iterator position, std::initializer_list<value_type> ilist)
	{
		const difference_type i(position - mItBegin);
		DoInsert(position, ilist.begin(), ilist.end(), false_type());
		return (mItBegin + i);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::iterator
	deque<T, Allocator, kDequeSubarraySize>::erase(const_iterator position)
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(!(validate_iterator(position) & isf_valid)))
				EASTL_FAIL_MSG("deque::erase -- invalid iterator");

			if(EASTL_UNLIKELY(position == end()))
				EASTL_FAIL_MSG("deque::erase -- end() iterator is an invalid iterator for erase");
		#endif

		iterator itPosition(position, typename iterator::FromConst());
		iterator itNext(itPosition, typename iterator::Increment());
		const difference_type i(itPosition - mItBegin);

		if(i < (difference_type)(size() / 2)) // Should we move the front entries forward or the back entries backward? We divide the range in half.
		{
			itNext.copy_backward(mItBegin, itPosition, eastl::has_trivial_relocate<value_type>());
			pop_front();
		}
		else
		{
			itPosition.copy(itNext, mItEnd, eastl::has_trivial_relocate<value_type>());
			pop_back();
		}

		return mItBegin + i;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::iterator
	deque<T, Allocator, kDequeSubarraySize>::erase(const_iterator first, const_iterator last)
	{
		iterator itFirst(first, typename iterator::FromConst());
		iterator itLast(last, typename iterator::FromConst());

		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(!(validate_iterator(itFirst) & isf_valid)))
				EASTL_FAIL_MSG("deque::erase -- invalid iterator");
			if(EASTL_UNLIKELY(!(validate_iterator(itLast) & isf_valid)))
				EASTL_FAIL_MSG("deque::erase -- invalid iterator");
		#endif

		if((itFirst != mItBegin) || (itLast != mItEnd)) // If not erasing everything... (We expect that the user won't call erase(begin, end) because instead the user would just call clear.)
		{
			const difference_type n(itLast - itFirst);
			const difference_type i(itFirst - mItBegin);

			if(i < (difference_type)((size() - n) / 2)) // Should we move the front entries forward or the back entries backward? We divide the range in half.
			{
				const iterator itNewBegin(mItBegin + n);
				value_type** const pPtrArrayBegin = mItBegin.mpCurrentArrayPtr;

				itLast.copy_backward(mItBegin, itFirst, eastl::has_trivial_relocate<value_type>());

				for(; mItBegin != itNewBegin; ++mItBegin) // Question: If value_type is a POD type, will the compiler generate this loop at all?
					mItBegin.mpCurrent->~value_type();    //           If so, then we need to make a specialization for destructing PODs.

				DoFreeSubarrays(pPtrArrayBegin, itNewBegin.mpCurrentArrayPtr);

				// mItBegin = itNewBegin; <-- Not necessary, as the above loop makes it so already.
			}
			else // Else we will be moving back entries backward.
			{
				iterator itNewEnd(mItEnd - n);
				value_type** const pPtrArrayEnd = itNewEnd.mpCurrentArrayPtr + 1;

				itFirst.copy(itLast, mItEnd, eastl::has_trivial_relocate<value_type>());

				for(iterator itTemp(itNewEnd); itTemp != mItEnd; ++itTemp)
					itTemp.mpCurrent->~value_type();

				DoFreeSubarrays(pPtrArrayEnd, mItEnd.mpCurrentArrayPtr + 1);

				mItEnd = itNewEnd;
			}

			return mItBegin + i;
		}

		clear();
		return mItEnd;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::reverse_iterator
	deque<T, Allocator, kDequeSubarraySize>::erase(reverse_iterator position)
	{
		return reverse_iterator(erase((++position).base()));
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	typename deque<T, Allocator, kDequeSubarraySize>::reverse_iterator
	deque<T, Allocator, kDequeSubarraySize>::erase(reverse_iterator first, reverse_iterator last)
	{
		// Version which erases in order from first to last.
		// difference_type i(first.base() - last.base());
		// while(i--)
		//     first = erase(first);
		// return first;

		// Version which erases in order from last to first, but is slightly more efficient:
		return reverse_iterator(erase(last.base(), first.base()));
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void deque<T, Allocator, kDequeSubarraySize>::clear()
	{
		// Destroy all values and all subarrays they belong to, except for the first one, 
		// as we need to reserve some space for a valid mItBegin/mItEnd.
		if(mItBegin.mpCurrentArrayPtr != mItEnd.mpCurrentArrayPtr) // If there are multiple subarrays (more often than not, this will be so)...
		{
			for(value_type* p1 = mItBegin.mpCurrent; p1 < mItBegin.mpEnd; ++p1)
				p1->~value_type();
			for(value_type* p2 = mItEnd.mpBegin; p2 < mItEnd.mpCurrent; ++p2)
				p2->~value_type();
			DoFreeSubarray(mItEnd.mpBegin); // Leave mItBegin with a valid subarray.
		}
		else
		{
			for(value_type* p = mItBegin.mpCurrent; p < mItEnd.mpCurrent; ++p)
				p->~value_type();
			// Don't free the one existing subarray, as we need it for mItBegin/mItEnd.
		}

		for(value_type** pPtrArray = mItBegin.mpCurrentArrayPtr + 1; pPtrArray < mItEnd.mpCurrentArrayPtr; ++pPtrArray)
		{
			for(value_type* p = *pPtrArray, *pEnd = *pPtrArray + kDequeSubarraySize; p < pEnd; ++p)
				p->~value_type();
			DoFreeSubarray(*pPtrArray);
		}

		mItEnd = mItBegin; // mItBegin/mItEnd will not be dereferencable.
	}


	//template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	//void deque<T, Allocator, kDequeSubarraySize>::reset_lose_memory()
	//{
	//    // The reset_lose_memory function is a special extension function which unilaterally 
	//    // resets the container to an empty state without freeing the memory of 
	//    // the contained objects. This is useful for very quickly tearing down a 
	//    // container built into scratch memory.
	//
	//    // Currently we are unable to get this reset_lose_memory operation to work correctly 
	//    // as we haven't been able to find a good way to have a deque initialize
	//    // without allocating memory. We can lose the old memory, but DoInit 
	//    // would necessarily do a ptrArray allocation. And this is not within
	//    // our definition of how reset_lose_memory works.
	//    base_type::DoInit(0);
	//
	//}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void deque<T, Allocator, kDequeSubarraySize>::swap(deque& x)
	{
	#if defined(EASTL_DEQUE_LEGACY_SWAP_BEHAVIOUR_REQUIRES_COPY_CTOR) && EASTL_DEQUE_LEGACY_SWAP_BEHAVIOUR_REQUIRES_COPY_CTOR
		if(mAllocator == x.mAllocator) // If allocators are equivalent...
			DoSwap(x);
		else // else swap the contents.
		{
			const this_type temp(*this); // Can't call eastl::swap because that would
			*this = x;                   // itself call this member swap function.
			x     = temp;
		}
	#else
		// NOTE(rparolin): The previous implementation required T to be copy-constructible in the fall-back case where
		// allocators with unique instances copied elements.  This was an unnecessary restriction and prevented the common
		// usage of deque with non-copyable types (eg. eastl::deque<non_copyable> or eastl::deque<unique_ptr>). 
		// 
		// The previous implementation violated the following requirements of deque::swap so the fall-back code has
		// been removed.  EASTL implicitly defines 'propagate_on_container_swap = false' therefore the fall-back case is
		// undefined behaviour.  We simply swap the contents and the allocator as that is the common expectation of
		// users and does not put the container into an invalid state since it can not free its memory via its current
		// allocator instance.
		//
		DoSwap(x);
	#endif
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	template <typename Integer>
	void deque<T, Allocator, kDequeSubarraySize>::DoInit(Integer n, Integer value, true_type)
	{
		base_type::DoInit(n);  // Call the base uninitialized init function. 
		DoFillInit(value);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	template <typename InputIterator>
	void deque<T, Allocator, kDequeSubarraySize>::DoInit(InputIterator first, InputIterator last, false_type)
	{
		typedef typename eastl::iterator_traits<InputIterator>::iterator_category IC;
		DoInitFromIterator(first, last, IC());
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	template <typename InputIterator>
	void deque<T, Allocator, kDequeSubarraySize>::DoInitFromIterator(InputIterator first, InputIterator last, EASTL_ITC_NS::input_iterator_tag)
	{
		base_type::DoInit(0); // Call the base uninitialized init function, but don't actually allocate any values.

		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
		#endif
				// We have little choice but to turn through the source iterator and call 
				// push_back for each item. It can be slow because it will keep reallocating the 
				// container memory as we go. We are not allowed to use distance() on an InputIterator.
				for(; first != last; ++first)   // InputIterators by definition actually only allow you to iterate through them once.
				{                               // Thus the standard *requires* that we do this (inefficient) implementation.  
					push_back(*first);          // Luckily, InputIterators are in practice almost never used, so this code will likely never get executed.
				}
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
				clear();
				throw;
			}
		#endif
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	template <typename ForwardIterator>
	void deque<T, Allocator, kDequeSubarraySize>::DoInitFromIterator(ForwardIterator first, ForwardIterator last, EASTL_ITC_NS::forward_iterator_tag)
	{
		typedef typename eastl::remove_const<ForwardIterator>::type non_const_iterator_type; // If T is a const type (e.g. const int) then we need to initialize it as if it were non-const.
		typedef typename eastl::remove_const<value_type>::type      non_const_value_type;

		const size_type n = (size_type)eastl::distance(first, last);
		value_type** pPtrArrayCurrent;

		base_type::DoInit(n); // Call the base uninitialized init function.

		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
		#endif
				for(pPtrArrayCurrent = mItBegin.mpCurrentArrayPtr; pPtrArrayCurrent < mItEnd.mpCurrentArrayPtr; ++pPtrArrayCurrent) // Copy to the known-to-be-completely-used subarrays.
				{
					// We implment an algorithm here whereby we use uninitialized_copy() and advance() instead of just iterating from first to last and constructing as we go. The reason for this is that we can take advantage of POD data types and implement construction as memcpy operations.
					ForwardIterator current(first); // To do: Implement a specialization of this algorithm for non-PODs which eliminates the need for 'current'.

					eastl::advance(current, kDequeSubarraySize);
					eastl::uninitialized_copy((non_const_iterator_type)first, (non_const_iterator_type)current, (non_const_value_type*)*pPtrArrayCurrent);
					first = current;
				}

				eastl::uninitialized_copy((non_const_iterator_type)first, (non_const_iterator_type)last, (non_const_value_type*)mItEnd.mpBegin);
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
				for(iterator itCurrent(mItBegin), itEnd(pPtrArrayCurrent, *pPtrArrayCurrent); itCurrent != itEnd; ++itCurrent)
					itCurrent.mpCurrent->~value_type();
				throw;
			}
		#endif
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void deque<T, Allocator, kDequeSubarraySize>::DoFillInit(const value_type& value)
	{
		value_type** pPtrArrayCurrent = mItBegin.mpCurrentArrayPtr;

		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
		#endif
				while(pPtrArrayCurrent < mItEnd.mpCurrentArrayPtr)
				{
					eastl::uninitialized_fill(*pPtrArrayCurrent, *pPtrArrayCurrent + kDequeSubarraySize, value);
					++pPtrArrayCurrent;
				}
				eastl::uninitialized_fill(mItEnd.mpBegin, mItEnd.mpCurrent, value);
		#if EASTL_EXCEPTIONS_ENABLED
			}
			catch(...)
			{
				for(iterator itCurrent(mItBegin), itEnd(pPtrArrayCurrent, *pPtrArrayCurrent); itCurrent != itEnd; ++itCurrent)
					itCurrent.mpCurrent->~value_type();
				throw;
			}
		#endif
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	template <typename Integer>
	void deque<T, Allocator, kDequeSubarraySize>::DoAssign(Integer n, Integer value, true_type) // false_type means this is the integer version instead of iterator version.
	{
		DoAssignValues(static_cast<size_type>(n), static_cast<value_type>(value));
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	template <typename InputIterator>
	void deque<T, Allocator, kDequeSubarraySize>::DoAssign(InputIterator first, InputIterator last, false_type) // false_type means this is the iterator version instead of integer version.
	{
		// Actually, the implementation below requires first/last to be a ForwardIterator and not just an InputIterator.
		// But Paul Pedriana if you somehow need to work with an InputIterator and we can deal with it.
		const size_type n     = (size_type)eastl::distance(first, last);
		const size_type nSize = size();

		if(n > nSize) // If we are increasing the size...
		{
			InputIterator atEnd(first);

			eastl::advance(atEnd, (difference_type)nSize);
			eastl::copy(first, atEnd, mItBegin);
			insert(mItEnd, atEnd, last);
		}
		else // n is <= size.
		{
			iterator itEnd(eastl::copy(first, last, mItBegin));

			if(n < nSize) // If we need to erase any trailing elements...
				erase(itEnd, mItEnd);
		}
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void deque<T, Allocator, kDequeSubarraySize>::DoAssignValues(size_type n, const value_type& value)
	{
		const size_type nSize = size();

		if(n > nSize) // If we are increasing the size...
		{
			eastl::fill(mItBegin, mItEnd, value);
			insert(mItEnd, n - nSize, value);
		}
		else
		{
			erase(mItBegin + (difference_type)n, mItEnd);
			eastl::fill(mItBegin, mItEnd, value);
		}
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	template <typename Integer>
	void deque<T, Allocator, kDequeSubarraySize>::DoInsert(const const_iterator& position, Integer n, Integer value, true_type)
	{
		DoInsertValues(position, (size_type)n, (value_type)value);
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	template <typename InputIterator>
	void deque<T, Allocator, kDequeSubarraySize>::DoInsert(const const_iterator& position, const InputIterator& first, const InputIterator& last, false_type)
	{
		typedef typename eastl::iterator_traits<InputIterator>::iterator_category IC;
		DoInsertFromIterator(position, first, last, IC());
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	template <typename InputIterator>
	void deque<T, Allocator, kDequeSubarraySize>::DoInsertFromIterator(const_iterator position, const InputIterator& first, const InputIterator& last, EASTL_ITC_NS::forward_iterator_tag)
	{
		const size_type n = (size_type)eastl::distance(first, last);

		// This implementation is nearly identical to DoInsertValues below. 
		// If you make a bug fix to one, you will likely want to fix the other.
		if(position.mpCurrent == mItBegin.mpCurrent) // If inserting at the beginning or into an empty container...
		{
			iterator itNewBegin(DoReallocSubarray(n, kSideFront)); // itNewBegin to mItBegin refers to memory that isn't initialized yet; so it's not truly a valid iterator. Or at least not a dereferencable one.

			#if EASTL_EXCEPTIONS_ENABLED
				try
				{
			#endif
					// We would like to use move here instead of copy when possible, which would be useful for 
					// when inserting from a std::initializer_list, for example.
					// To do: solve this by having a template or runtime parameter which specifies move vs copy.
					eastl::uninitialized_copy(first, last, itNewBegin);
					mItBegin = itNewBegin;
			#if EASTL_EXCEPTIONS_ENABLED
				}
				catch(...)
				{
					DoFreeSubarrays(itNewBegin.mpCurrentArrayPtr, mItBegin.mpCurrentArrayPtr);
					throw;
				}
			#endif
		}
		else if(EASTL_UNLIKELY(position.mpCurrent == mItEnd.mpCurrent)) // If inserting at the end (i.e. appending)...
		{
			const iterator itNewEnd(DoReallocSubarray(n, kSideBack)); // mItEnd to itNewEnd refers to memory that isn't initialized yet; so it's not truly a valid iterator. Or at least not a dereferencable one.

			#if EASTL_EXCEPTIONS_ENABLED
				try
				{
			#endif
					// We would like to use move here instead of copy when possible, which would be useful for 
					// when inserting from a std::initializer_list, for example.
					// To do: solve this by having a template or runtime parameter which specifies move vs copy.
					eastl::uninitialized_copy(first, last, mItEnd);
					mItEnd = itNewEnd;
			#if EASTL_EXCEPTIONS_ENABLED
				}
				catch(...)
				{
					DoFreeSubarrays(mItEnd.mpCurrentArrayPtr + 1, itNewEnd.mpCurrentArrayPtr + 1);
					throw;
				}
			#endif
		}
		else
		{
			const difference_type nInsertionIndex = position - mItBegin;
			const size_type       nSize           = size();

			if(nInsertionIndex < (difference_type)(nSize / 2)) // If the insertion index is in the front half of the deque... grow the deque at the front.
			{
				const iterator itNewBegin(DoReallocSubarray(n, kSideFront)); // itNewBegin to mItBegin refers to memory that isn't initialized yet; so it's not truly a valid iterator. Or at least not a dereferencable one.
				const iterator itOldBegin(mItBegin);
				const iterator itPosition(mItBegin + nInsertionIndex); // We need to reset this value because the reallocation above can invalidate iterators.

				#if EASTL_EXCEPTIONS_ENABLED
					try
					{
				#endif
						// We have a problem here: we would like to use move instead of copy, but it may be that the range to be inserted comes from
						// this container and comes from the segment we need to move. So we can't use move operations unless we are careful to handle
						// that situation. The newly inserted contents must be contents that were moved to and not moved from. To do: solve this.
						if(nInsertionIndex >= (difference_type)n) // If the newly inserted items will be entirely within the old area...
						{
							iterator itUCopyEnd(mItBegin + (difference_type)n);

							eastl::uninitialized_copy(mItBegin, itUCopyEnd, itNewBegin); // This can throw.
							itUCopyEnd = eastl::copy(itUCopyEnd, itPosition, itOldBegin); // Recycle 'itUCopyEnd' to mean something else.
							eastl::copy(first, last, itUCopyEnd);
						}
						else // Else the newly inserted items are going within the newly allocated area at the front.
						{
							InputIterator mid(first);

							eastl::advance(mid, (difference_type)n - nInsertionIndex);
							eastl::uninitialized_copy_copy(mItBegin, itPosition, first, mid, itNewBegin); // This can throw.
							eastl::copy(mid, last, itOldBegin);
						}
						mItBegin = itNewBegin;
				#if EASTL_EXCEPTIONS_ENABLED
					}
					catch(...)
					{
						DoFreeSubarrays(itNewBegin.mpCurrentArrayPtr, mItBegin.mpCurrentArrayPtr);
						throw;
					}
				#endif
			}
			else
			{
				const iterator        itNewEnd(DoReallocSubarray(n, kSideBack));
				const iterator        itOldEnd(mItEnd);
				const difference_type nPushedCount = (difference_type)nSize - nInsertionIndex;
				const iterator        itPosition(mItEnd - nPushedCount); // We need to reset this value because the reallocation above can invalidate iterators.

				#if EASTL_EXCEPTIONS_ENABLED
					try
					{
				#endif
						// We have a problem here: we would like to use move instead of copy, but it may be that the range to be inserted comes from
						// this container and comes from the segment we need to move. So we can't use move operations unless we are careful to handle
						// that situation. The newly inserted contents must be contents that were moved to and not moved from. To do: solve this.
						if(nPushedCount > (difference_type)n)
						{
							const iterator itUCopyEnd(mItEnd - (difference_type)n);

							eastl::uninitialized_copy(itUCopyEnd, mItEnd, mItEnd);
							eastl::copy_backward(itPosition, itUCopyEnd, itOldEnd);
							eastl::copy(first, last, itPosition);
						}
						else
						{
							InputIterator mid(first);

							eastl::advance(mid, nPushedCount);
							eastl::uninitialized_copy_copy(mid, last, itPosition, mItEnd, mItEnd);
							eastl::copy(first, mid, itPosition);
						}
						mItEnd = itNewEnd;
				#if EASTL_EXCEPTIONS_ENABLED
					}
					catch(...)
					{
						DoFreeSubarrays(mItEnd.mpCurrentArrayPtr + 1, itNewEnd.mpCurrentArrayPtr + 1);
						throw;
					}
				#endif
			}
		}
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	void deque<T, Allocator, kDequeSubarraySize>::DoInsertValues(const_iterator position, size_type n, const value_type& value)
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(!(validate_iterator(position) & isf_valid)))
				EASTL_FAIL_MSG("deque::insert -- invalid iterator");
		#endif

		// This implementation is nearly identical to DoInsertFromIterator above. 
		// If you make a bug fix to one, you will likely want to fix the other.
		if(position.mpCurrent == mItBegin.mpCurrent) // If inserting at the beginning...
		{
			const iterator itNewBegin(DoReallocSubarray(n, kSideFront));

			#if EASTL_EXCEPTIONS_ENABLED
				try
				{
			#endif
					// Note that we don't make a temp copy of 'value' here. This is because in a 
					// deque, insertion at either the front or back doesn't cause a reallocation
					// or move of data in the middle. That's a key feature of deques, in fact.
					eastl::uninitialized_fill(itNewBegin, mItBegin, value);
					mItBegin = itNewBegin;
			#if EASTL_EXCEPTIONS_ENABLED
				}
				catch(...)
				{
					DoFreeSubarrays(itNewBegin.mpCurrentArrayPtr, mItBegin.mpCurrentArrayPtr);
					throw;
				}
			#endif
		}
		else if(EASTL_UNLIKELY(position.mpCurrent == mItEnd.mpCurrent)) // If inserting at the end (i.e. appending)...
		{
			const iterator itNewEnd(DoReallocSubarray(n, kSideBack));

			#if EASTL_EXCEPTIONS_ENABLED
				try
				{
			#endif
					// Note that we don't make a temp copy of 'value' here. This is because in a 
					// deque, insertion at either the front or back doesn't cause a reallocation
					// or move of data in the middle. That's a key feature of deques, in fact.
					eastl::uninitialized_fill(mItEnd, itNewEnd, value);
					mItEnd = itNewEnd;
			#if EASTL_EXCEPTIONS_ENABLED
				}
				catch(...)
				{
					DoFreeSubarrays(mItEnd.mpCurrentArrayPtr + 1, itNewEnd.mpCurrentArrayPtr + 1);
					throw;
				}
			#endif
		}
		else
		{
			// A key purpose of a deque is to implement insertions and removals more efficiently 
			// than with a vector. We are inserting into the middle of the deque here. A quick and 
			// dirty implementation of this would be to reallocate the subarrays and simply push 
			// all values in the middle upward like you would do with a vector. Instead we implement
			// the minimum amount of reallocations needed but may need to do some value moving, 
			// as the subarray sizes need to remain constant and can have no holes in them.
			const difference_type nInsertionIndex = position - mItBegin;
			const size_type       nSize = size();
			const value_type      valueSaved(value);

			if(nInsertionIndex < (difference_type)(nSize / 2)) // If the insertion index is in the front half of the deque... grow the deque at the front.
			{
				const iterator itNewBegin(DoReallocSubarray(n, kSideFront));
				const iterator itOldBegin(mItBegin);
				const iterator itPosition(mItBegin + nInsertionIndex); // We need to reset this value because the reallocation above can invalidate iterators.

				#if EASTL_EXCEPTIONS_ENABLED
					try
					{
				#endif
						if(nInsertionIndex >= (difference_type)n) // If the newly inserted items will be entirely within the old area...
						{
							iterator itUCopyEnd(mItBegin + (difference_type)n);

							eastl::uninitialized_move_if_noexcept(mItBegin, itUCopyEnd, itNewBegin); // This can throw.
							itUCopyEnd = eastl::move(itUCopyEnd, itPosition, itOldBegin); // Recycle 'itUCopyEnd' to mean something else.
							eastl::fill(itUCopyEnd, itPosition, valueSaved);
						}
						else // Else the newly inserted items are going within the newly allocated area at the front.
						{
							eastl::uninitialized_move_fill(mItBegin, itPosition, itNewBegin, mItBegin, valueSaved); // This can throw.
							eastl::fill(itOldBegin, itPosition, valueSaved);
						}
						mItBegin = itNewBegin;
				#if EASTL_EXCEPTIONS_ENABLED
					}
					catch(...)
					{
						DoFreeSubarrays(itNewBegin.mpCurrentArrayPtr, mItBegin.mpCurrentArrayPtr);
						throw;
					}
				#endif
			}
			else // Else the insertion index is in the back half of the deque, so grow the deque at the back.
			{
				const iterator        itNewEnd(DoReallocSubarray(n, kSideBack));
				const iterator        itOldEnd(mItEnd);
				const difference_type nPushedCount = (difference_type)nSize - nInsertionIndex;
				const iterator        itPosition(mItEnd - nPushedCount); // We need to reset this value because the reallocation above can invalidate iterators.

				#if EASTL_EXCEPTIONS_ENABLED
					try
					{
				#endif
						if(nPushedCount > (difference_type)n) // If the newly inserted items will be entirely within the old area...
						{
							iterator itUCopyEnd(mItEnd - (difference_type)n);

							eastl::uninitialized_move_if_noexcept(itUCopyEnd, mItEnd, mItEnd); // This can throw.
							itUCopyEnd = eastl::move_backward(itPosition, itUCopyEnd, itOldEnd); // Recycle 'itUCopyEnd' to mean something else.
							eastl::fill(itPosition, itUCopyEnd, valueSaved);
						}
						else // Else the newly inserted items are going within the newly allocated area at the back.
						{
							eastl::uninitialized_fill_move(mItEnd, itPosition + (difference_type)n, valueSaved, itPosition, mItEnd); // This can throw.
							eastl::fill(itPosition, itOldEnd, valueSaved);
						}
						mItEnd = itNewEnd;
				#if EASTL_EXCEPTIONS_ENABLED
					}
					catch(...)
					{
						DoFreeSubarrays(mItEnd.mpCurrentArrayPtr + 1, itNewEnd.mpCurrentArrayPtr + 1);
						throw;
					}
				#endif
			}
		}
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline void deque<T, Allocator, kDequeSubarraySize>::DoSwap(this_type& x)
	{
		eastl::swap(mpPtrArray,     x.mpPtrArray);
		eastl::swap(mnPtrArraySize, x.mnPtrArraySize);
		eastl::swap(mItBegin,       x.mItBegin);
		eastl::swap(mItEnd,         x.mItEnd);
		eastl::swap(mAllocator,     x.mAllocator);  // We do this even if EASTL_ALLOCATOR_COPY_ENABLED is 0.

	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline bool deque<T, Allocator, kDequeSubarraySize>::validate() const
	{
		// To do: More detailed validation.
		// To do: Try to make the validation resistant to crashes if the data is invalid.
		if((end() - begin()) < 0)
			return false;
		return true;
	}


	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline int deque<T, Allocator, kDequeSubarraySize>::validate_iterator(const_iterator i) const
	{
		// To do: We don't currently track isf_current, will need to make it do so.
		// To do: Fix the validation below, as it will not catch all invalid iterators.
		if((i - begin()) < 0)
			return isf_none;

		if((end() - i) < 0)
			return isf_none;

		if(i == end())
			return (isf_valid | isf_current);

		return (isf_valid | isf_current | isf_can_dereference);
	}



	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline bool operator==(const deque<T, Allocator, kDequeSubarraySize>& a, const deque<T, Allocator, kDequeSubarraySize>& b)
	{
		return ((a.size() == b.size()) && eastl::equal(a.begin(), a.end(), b.begin()));
	}

	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline bool operator!=(const deque<T, Allocator, kDequeSubarraySize>& a, const deque<T, Allocator, kDequeSubarraySize>& b)
	{
		return ((a.size() != b.size()) || !eastl::equal(a.begin(), a.end(), b.begin()));
	}

	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline bool operator<(const deque<T, Allocator, kDequeSubarraySize>& a, const deque<T, Allocator, kDequeSubarraySize>& b)
	{
		return eastl::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
	}

	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline bool operator>(const deque<T, Allocator, kDequeSubarraySize>& a, const deque<T, Allocator, kDequeSubarraySize>& b)
	{
		return b < a;
	}

	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline bool operator<=(const deque<T, Allocator, kDequeSubarraySize>& a, const deque<T, Allocator, kDequeSubarraySize>& b)
	{
		return !(b < a);
	}

	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline bool operator>=(const deque<T, Allocator, kDequeSubarraySize>& a, const deque<T, Allocator, kDequeSubarraySize>& b)
	{
		return !(a < b);
	}

	template <typename T, typename Allocator, unsigned kDequeSubarraySize>
	inline void swap(deque<T, Allocator, kDequeSubarraySize>& a, deque<T, Allocator, kDequeSubarraySize>& b)
	{
		a.swap(b);
	}

	///////////////////////////////////////////////////////////////////////
	// erase / erase_if
	//
	// https://en.cppreference.com/w/cpp/container/deque/erase2
	///////////////////////////////////////////////////////////////////////
	template <class T, class Allocator, class U>
	void erase(deque<T, Allocator>& c, const U& value)
	{
		// Erases all elements that compare equal to value from the container.
		c.erase(eastl::remove(c.begin(), c.end(), value), c.end());
	}

	template <class T, class Allocator, class Predicate>
	void erase_if(deque<T, Allocator>& c, Predicate predicate)
	{
		// Erases all elements that satisfy the predicate pred from the container.
		c.erase(eastl::remove_if(c.begin(), c.end(), predicate), c.end());
	}


} // namespace eastl


#ifdef _MSC_VER
	#pragma warning(pop)
#endif


#endif // Header include guard







