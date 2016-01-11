///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements a vector (array-like container), much like the C++ 
// std::vector class.
// The primary distinctions between this vector and std::vector are:
//    - vector has a couple extension functions that increase performance.
//    - vector can contain objects with alignment requirements. std::vector 
//      cannot do so without a bit of tedious non-portable effort.
//    - vector supports debug memory naming natively.
//    - vector is easier to read, debug, and visualize.
//    - vector is savvy to an environment that doesn't have exception handling,
//      as is sometimes the case with console or embedded environments.
//    - vector has less deeply nested function calls and allows the user to 
//      enable forced inlining in debug builds in order to reduce bloat.
//    - vector<bool> is a vector of boolean values and not a bit vector.
//    - vector guarantees that memory is contiguous and that vector::iterator
//      is nothing more than a pointer to T.
//    - vector has an explicit data() method for obtaining a pointer to storage 
//      which is safe to call even if the block is empty. This avoids the 
//      common &v[0], &v.front(), and &*v.begin() constructs that trigger false 
//      asserts in STL debugging modes.
//    - vector data is guaranteed to be contiguous.
//    - vector has a set_capacity() function which frees excess capacity. 
//      The only way to do this with std::vector is via the cryptic non-obvious 
//      trick of using: vector<SomeClass>(x).swap(x);
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_VECTOR_H
#define EASTL_VECTOR_H


#include <EASTL/internal/config.h>
#include <EASTL/allocator.h>
#include <EASTL/type_traits.h>
#include <EASTL/iterator.h>
#include <EASTL/algorithm.h>
#include <EASTL/initializer_list.h>
#include <EASTL/memory.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
	#include <new>
	#include <stddef.h>
	#pragma warning(pop)
#else
	#include <new>
	#include <stddef.h>
#endif


#if EASTL_EXCEPTIONS_ENABLED
	#ifdef _MSC_VER
		#pragma warning(push, 0)
	#endif
	#include <stdexcept> // std::out_of_range, std::length_error.
	#ifdef _MSC_VER
		#pragma warning(pop)
	#endif
#endif

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4530)  // C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
	#pragma warning(disable: 4345)  // Behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized
	#pragma warning(disable: 4244)  // Argument: conversion from 'int' to 'const eastl::vector<T>::value_type', possible loss of data
	#pragma warning(disable: 4127)  // Conditional expression is constant
	#pragma warning(disable: 4480)  // nonstandard extension used: specifying underlying type for enum
	#pragma warning(disable: 4571)  // catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught.
#endif

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif

#if EASTL_NOMINMAX
	#ifdef min
		#undef min
	#endif
	#ifdef max
		#undef max
	#endif
#endif

namespace eastl
{

	/// EASTL_VECTOR_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_VECTOR_DEFAULT_NAME
		#define EASTL_VECTOR_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " vector" // Unless the user overrides something, this is "EASTL vector".
	#endif


	/// EASTL_VECTOR_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_VECTOR_DEFAULT_ALLOCATOR
		#define EASTL_VECTOR_DEFAULT_ALLOCATOR allocator_type(EASTL_VECTOR_DEFAULT_NAME)
	#endif



	/// VectorBase
	///
	/// The reason we have a VectorBase class is that it makes exception handling
	/// simpler to implement because memory allocation is implemented entirely 
	/// in this class. If a user creates a vector which needs to allocate
	/// memory in the constructor, VectorBase handles it. If an exception is thrown
	/// by the allocator then the exception throw jumps back to the user code and 
	/// no try/catch code need be written in the vector or VectorBase constructor. 
	/// If an exception is thrown in the vector (not VectorBase) constructor, the 
	/// destructor for VectorBase will be called automatically (and free the allocated
	/// memory) before the execution jumps back to the user code.
	/// However, if the vector class were to handle both allocation and initialization
	/// then it would have no choice but to implement an explicit try/catch statement
	/// for all pathways that allocate memory. This increases code size and decreases
	/// performance and makes the code a little harder read and maintain.
	///
	/// The C++ standard (15.2 paragraph 2) states: 
	///    "An object that is partially constructed or partially destroyed will
	///     have destructors executed for all its fully constructed subobjects,
	///     that is, for subobjects for which the constructor has been completed
	///     execution and the destructor has not yet begun execution."
	///
	/// The C++ standard (15.3 paragraph 11) states: 
	///    "The fully constructed base classes and members of an object shall 
	///     be destroyed before entering the handler of a function-try-block
	///     of a constructor or destructor for that block."
	///
	template <typename T, typename Allocator>
	struct VectorBase
	{
		typedef Allocator    allocator_type;
		typedef eastl_size_t size_type;
		typedef ptrdiff_t    difference_type;

		#if defined(_MSC_VER) && (_MSC_VER >= 1400) && (_MSC_VER <= 1600) && !EASTL_STD_CPP_ONLY  // _MSC_VER of 1400 means VS2005, 1600 means VS2010. VS2012 generates errors with usage of enum:size_type.
			enum : size_type {                      // Use Microsoft enum language extension, allowing for smaller debug symbols than using a static const. Users have been affected by this.
				npos     = (size_type)-1,
				kMaxSize = (size_type)-2
			};
		#else
			static const size_type npos     = (size_type)-1;      /// 'npos' means non-valid position or simply non-position.
			static const size_type kMaxSize = (size_type)-2;      /// -1 is reserved for 'npos'. It also happens to be slightly beneficial that kMaxSize is a value less than -1, as it helps us deal with potential integer wraparound issues.
		#endif

	protected:
		T*              mpBegin;
		T*              mpEnd;
		T*              mpCapacity;
		allocator_type  mAllocator;  // To do: Use base class optimization to make this go away.

	public:
		VectorBase();
		VectorBase(const allocator_type& allocator);
		VectorBase(size_type n, const allocator_type& allocator);

	   ~VectorBase();

		const allocator_type& get_allocator() const EA_NOEXCEPT;
		allocator_type&       get_allocator() EA_NOEXCEPT;
		void                  set_allocator(const allocator_type& allocator);

	protected:
		T*        DoAllocate(size_type n);
		void      DoFree(T* p, size_type n);
		size_type GetNewCapacity(size_type currentCapacity);

	}; // VectorBase




	/// vector
	///
	/// Implements a dynamic array.
	///
	template <typename T, typename Allocator = EASTLAllocatorType>
	class vector : public VectorBase<T, Allocator>
	{
		typedef VectorBase<T, Allocator>                      base_type;
		typedef vector<T, Allocator>                          this_type;

	public:
		typedef T                                             value_type;
		typedef T*                                            pointer;
		typedef const T*                                      const_pointer;
		typedef T&                                            reference;
		typedef const T&                                      const_reference;  // Maintainer note: We want to leave iterator defined as T* -- at least in release builds -- as this gives some algorithms an advantage that optimizers cannot get around.
		typedef T*                                            iterator;         // Note: iterator is simply T* right now, but this will likely change in the future, at least for debug builds. 
		typedef const T*                                      const_iterator;   //       Do not write code that relies on iterator being T*. The reason it will 
		typedef eastl::reverse_iterator<iterator>             reverse_iterator; //       change in the future is that a debugging iterator system will be created.
		typedef eastl::reverse_iterator<const_iterator>       const_reverse_iterator;    
		typedef typename base_type::size_type                 size_type;
		typedef typename base_type::difference_type           difference_type;
		typedef typename base_type::allocator_type            allocator_type;

		using base_type::mpBegin;
		using base_type::mpEnd;
		using base_type::mpCapacity;
		using base_type::mAllocator;
		using base_type::npos;
		using base_type::GetNewCapacity;
		using base_type::DoAllocate;
		using base_type::DoFree;

	public:
		vector();
		explicit vector(const allocator_type& allocator);
		explicit vector(size_type n, const allocator_type& allocator = EASTL_VECTOR_DEFAULT_ALLOCATOR);
		vector(size_type n, const value_type& value, const allocator_type& allocator = EASTL_VECTOR_DEFAULT_ALLOCATOR);
		vector(const this_type& x);
		vector(const this_type& x, const allocator_type& allocator);
		#if EASTL_MOVE_SEMANTICS_ENABLED
			vector(this_type&& x);
			vector(this_type&& x, const allocator_type& allocator);
		#endif
		vector(std::initializer_list<value_type> ilist, const allocator_type& allocator = EASTL_VECTOR_DEFAULT_ALLOCATOR);

		template <typename InputIterator>
		vector(InputIterator first, InputIterator last, const allocator_type& allocator = EASTL_VECTOR_DEFAULT_ALLOCATOR);

	   ~vector();

		this_type& operator=(const this_type& x);
		this_type& operator=(std::initializer_list<value_type> ilist);
		#if EASTL_MOVE_SEMANTICS_ENABLED
			this_type& operator=(this_type&& x);
		#endif

		void swap(this_type& x);

		void assign(size_type n, const value_type& value);

		template <typename InputIterator>
		void assign(InputIterator first, InputIterator last);

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
		size_type capacity() const EA_NOEXCEPT;

		void resize(size_type n, const value_type& value);
		void resize(size_type n);
		void reserve(size_type n);
		void set_capacity(size_type n = base_type::npos);   // Revises the capacity to the user-specified value. Resizes the container to match the capacity if the requested capacity n is less than the current size. If n == npos then the capacity is reallocated (if necessary) such that capacity == size.
		void shrink_to_fit();                               // C++11 function which is the same as set_capacity().

		pointer       data() EA_NOEXCEPT;
		const_pointer data() const EA_NOEXCEPT;

		reference       operator[](size_type n);
		const_reference operator[](size_type n) const;

		reference       at(size_type n);
		const_reference at(size_type n) const;

		reference       front();
		const_reference front() const;

		reference       back();
		const_reference back() const;

		void      push_back(const value_type& value);
		reference push_back();
		void*     push_back_uninitialized();
		#if EASTL_MOVE_SEMANTICS_ENABLED
			void  push_back(value_type&& value);
		#endif
		void      pop_back();

		#if EASTL_MOVE_SEMANTICS_ENABLED && EASTL_VARIADIC_TEMPLATES_ENABLED
			template<class... Args>
			iterator emplace(const_iterator position, Args&&... args);

			template<class... Args>
			void emplace_back(Args&&... args);
		#else
			#if EASTL_MOVE_SEMANTICS_ENABLED
			  iterator emplace(const_iterator position, value_type&& value);
			  void emplace_back(value_type&& value);
			#endif

			iterator emplace(const_iterator position, const value_type& value);
			void emplace_back(const value_type& value);
		#endif

		iterator insert(const_iterator position, const value_type& value);
		void     insert(const_iterator position, size_type n, const value_type& value);
		#if EASTL_MOVE_SEMANTICS_ENABLED
			iterator insert(const_iterator position, value_type&& value);
		#endif
		iterator insert(const_iterator position, std::initializer_list<value_type> ilist);

		template <typename InputIterator>
		void insert(const_iterator position, InputIterator first, InputIterator last);

		iterator erase(const_iterator position);
		iterator erase(const_iterator first, const_iterator last);
		iterator erase_unsorted(const_iterator position);         // Same as erase, except it doesn't preserve order, but is faster because it simply copies the last item in the vector over the erased position.

		reverse_iterator erase(const_reverse_iterator position);
		reverse_iterator erase(const_reverse_iterator first, const_reverse_iterator last);
		reverse_iterator erase_unsorted(const_reverse_iterator position);

		void clear() EA_NOEXCEPT;
		void reset_lose_memory() EA_NOEXCEPT;                       // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.

		bool validate() const EA_NOEXCEPT;
		int  validate_iterator(const_iterator i) const EA_NOEXCEPT;

		#if EASTL_RESET_ENABLED
			void reset() EA_NOEXCEPT; // This function name is deprecated; use reset_lose_memory instead.
		#endif

	protected:
		// These functions do the real work of maintaining the vector. You will notice
		// that many of them have the same name but are specialized on iterator_tag
		// (iterator categories). This is because in these cases there is an optimized
		// implementation that can be had for some cases relative to others. Functions
		// which aren't referenced are neither compiled nor linked into the application.
		struct should_copy_tag{}; struct should_move_tag : public should_copy_tag{};

		template <typename ForwardIterator> // Allocates a pointer of array count n and copy-constructs it with [first,last).
		pointer DoRealloc(size_type n, ForwardIterator first, ForwardIterator last, should_copy_tag);

		template <typename ForwardIterator> // Allocates a pointer of array count n and copy-constructs it with [first,last).
		pointer DoRealloc(size_type n, ForwardIterator first, ForwardIterator last, should_move_tag);

		template <typename Integer>
		void DoInit(Integer n, Integer value, true_type);

		template <typename InputIterator>
		void DoInit(InputIterator first, InputIterator last, false_type);

		template <typename InputIterator>
		void DoInitFromIterator(InputIterator first, InputIterator last, EASTL_ITC_NS::input_iterator_tag);

		template <typename ForwardIterator>
		void DoInitFromIterator(ForwardIterator first, ForwardIterator last, EASTL_ITC_NS::forward_iterator_tag);

		template <typename Integer, bool bMove>
		void DoAssign(Integer n, Integer value, true_type);

		template <typename InputIterator, bool bMove>
		void DoAssign(InputIterator first, InputIterator last, false_type);

		void DoAssignValues(size_type n, const value_type& value);

		template <typename InputIterator, bool bMove>
		void DoAssignFromIterator(InputIterator first, InputIterator last, EASTL_ITC_NS::input_iterator_tag);

		template <typename RandomAccessIterator, bool bMove>
		void DoAssignFromIterator(RandomAccessIterator first, RandomAccessIterator last, EASTL_ITC_NS::random_access_iterator_tag);

		template <typename Integer>
		void DoInsert(const_iterator position, Integer n, Integer value, true_type);

		template <typename InputIterator>
		void DoInsert(const_iterator position, InputIterator first, InputIterator last, false_type);

		template <typename InputIterator>
		void DoInsertFromIterator(const_iterator position, InputIterator first, InputIterator last, EASTL_ITC_NS::input_iterator_tag);

		template <typename BidirectionalIterator>
		void DoInsertFromIterator(const_iterator position, BidirectionalIterator first, BidirectionalIterator last, EASTL_ITC_NS::bidirectional_iterator_tag);

		void DoInsertValues(const_iterator position, size_type n, const value_type& value);

		void DoInsertValuesEnd(size_type n); // Default constructs n values
		void DoInsertValuesEnd(size_type n, const value_type& value);

		#if EASTL_MOVE_SEMANTICS_ENABLED && EASTL_VARIADIC_TEMPLATES_ENABLED // If we can do variadic arguments...
			template<typename... Args>
			void DoInsertValue(const_iterator position, Args&&... args);
		#else
			#if EASTL_MOVE_SEMANTICS_ENABLED
				void DoInsertValue(const_iterator position, value_type&& value);
			#endif
			void DoInsertValue(const_iterator position, const value_type& value);
		#endif


		#if EASTL_MOVE_SEMANTICS_ENABLED && EASTL_VARIADIC_TEMPLATES_ENABLED
			template<typename... Args>
			void DoInsertValueEnd(Args&&... args);
		#else
			#if EASTL_MOVE_SEMANTICS_ENABLED
				void DoInsertValueEnd(value_type&& value);
			#endif
				void DoInsertValueEnd(const value_type& value);
		#endif

		void DoClearCapacity();

		void DoGrow(size_type n);

		void DoSwap(this_type& x);

	}; // class vector






	///////////////////////////////////////////////////////////////////////
	// VectorBase
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Allocator>
	inline VectorBase<T, Allocator>::VectorBase()
		: mpBegin(NULL), 
		  mpEnd(NULL),
		  mpCapacity(NULL),
		  mAllocator(EASTL_VECTOR_DEFAULT_NAME)
	{
	}


	template <typename T, typename Allocator>
	inline VectorBase<T, Allocator>::VectorBase(const allocator_type& allocator)
		: mpBegin(NULL), 
		  mpEnd(NULL),
		  mpCapacity(NULL),
		  mAllocator(allocator)
	{
	}


	template <typename T, typename Allocator>
	inline VectorBase<T, Allocator>::VectorBase(size_type n, const allocator_type& allocator)
		: mAllocator(allocator)
	{
		mpBegin    = DoAllocate(n);
		mpEnd      = mpBegin;
		mpCapacity = mpBegin + n;
	}


	template <typename T, typename Allocator>
	inline VectorBase<T, Allocator>::~VectorBase()
	{
		if(mpBegin)
			EASTLFree(mAllocator, mpBegin, (mpCapacity - mpBegin) * sizeof(T));
	}


	template <typename T, typename Allocator>
	inline const typename VectorBase<T, Allocator>::allocator_type&
	VectorBase<T, Allocator>::get_allocator() const EA_NOEXCEPT
	{
		return mAllocator;
	}


	template <typename T, typename Allocator>
	inline typename VectorBase<T, Allocator>::allocator_type&
	VectorBase<T, Allocator>::get_allocator() EA_NOEXCEPT
	{
		return mAllocator;
	}


	template <typename T, typename Allocator>
	inline void VectorBase<T, Allocator>::set_allocator(const allocator_type& allocator)
	{
		mAllocator = allocator;
	}


	template <typename T, typename Allocator>
	inline T* VectorBase<T, Allocator>::DoAllocate(size_type n)
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(n >= 0x80000000))
				EASTL_FAIL_MSG("vector::DoAllocate -- improbably large request.");
		#endif

		// If n is zero, then we allocate no memory and just return NULL. 
		// This is fine, as our default ctor initializes with NULL pointers. 
		return n ? (T*)allocate_memory(mAllocator, n * sizeof(T), EASTL_ALIGN_OF(T), 0) : NULL;
	}


	template <typename T, typename Allocator>
	inline void VectorBase<T, Allocator>::DoFree(T* p, size_type n)
	{
		if(p)
			EASTLFree(mAllocator, p, n * sizeof(T)); 
	}


	template <typename T, typename Allocator>
	inline typename VectorBase<T, Allocator>::size_type
	VectorBase<T, Allocator>::GetNewCapacity(size_type currentCapacity)
	{
		// This needs to return a value of at least currentCapacity and at least 1.
		return (currentCapacity > 0) ? (2 * currentCapacity) : 1;
	}




	///////////////////////////////////////////////////////////////////////
	// vector
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Allocator>
	inline vector<T, Allocator>::vector()
		: base_type()
	{
		// Empty
	}


	template <typename T, typename Allocator>
	inline vector<T, Allocator>::vector(const allocator_type& allocator)
		: base_type(allocator)
	{
		// Empty
	}


	template <typename T, typename Allocator>
	inline vector<T, Allocator>::vector(size_type n, const allocator_type& allocator)
		: base_type(n, allocator)
	{
		eastl::uninitialized_default_fill_n(mpBegin, n);
		mpEnd = mpBegin + n;
	}


	template <typename T, typename Allocator>
	inline vector<T, Allocator>::vector(size_type n, const value_type& value, const allocator_type& allocator)
		: base_type(n, allocator)
	{
		eastl::uninitialized_fill_n_ptr(mpBegin, n, value);
		mpEnd = mpBegin + n;
	}


	template <typename T, typename Allocator>
	inline vector<T, Allocator>::vector(const this_type& x)
		: base_type(x.size(), x.mAllocator)
	{
		mpEnd = eastl::uninitialized_copy_ptr(x.mpBegin, x.mpEnd, mpBegin);
	}


	template <typename T, typename Allocator>
	inline vector<T, Allocator>::vector(const this_type& x, const allocator_type& allocator)
		: base_type(x.size(), allocator)
	{
		mpEnd = eastl::uninitialized_copy_ptr(x.mpBegin, x.mpEnd, mpBegin);
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename T, typename Allocator>
		inline vector<T, Allocator>::vector(this_type&& x)
			: base_type(x.mAllocator)
		{
			DoSwap(x);
		}


		template <typename T, typename Allocator>
		inline vector<T, Allocator>::vector(this_type&& x, const allocator_type& allocator)
			: base_type(allocator)
		{
			swap(x); // member swap handles the case that x has a different allocator than our allocator by doing a copy.
		}
	#endif


	template <typename T, typename Allocator>
	inline vector<T, Allocator>::vector(std::initializer_list<value_type> ilist, const allocator_type& allocator)
		: base_type(allocator)
	{
		DoInit(ilist.begin(), ilist.end(), false_type());
	}


	template <typename T, typename Allocator>
	template <typename InputIterator>
	inline vector<T, Allocator>::vector(InputIterator first, InputIterator last, const allocator_type& allocator)
		: base_type(allocator)
	{
		DoInit(first, last, is_integral<InputIterator>());
	}


	template <typename T, typename Allocator>
	inline vector<T, Allocator>::~vector()
	{
		// Call destructor for the values. Parent class will free the memory.
		eastl::destruct(mpBegin, mpEnd);
	}


	template <typename T, typename Allocator>
	typename vector<T, Allocator>::this_type&
	vector<T, Allocator>::operator=(const this_type& x)
	{
		if(this != &x) // If not assigning to self...
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
				DoClearCapacity(); // Must clear the capacity instead of clear because set_capacity frees our memory, unlike clear.

				#if EASTL_ALLOCATOR_COPY_ENABLED
					mAllocator = x.mAllocator;
				#endif
			}

			DoAssign<const_iterator, false>(x.begin(), x.end(), eastl::false_type());
		}

		return *this;
	}


	template <typename T, typename Allocator>
	typename vector<T, Allocator>::this_type&
	vector<T, Allocator>::operator=(std::initializer_list<value_type> ilist)
	{
		typedef typename std::initializer_list<value_type>::iterator InputIterator;
		typedef typename eastl::iterator_traits<InputIterator>::iterator_category IC;
		DoAssignFromIterator<InputIterator, false>(ilist.begin(), ilist.end(), IC()); // initializer_list has const elements and so we can't move from them.
		return *this;
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename T, typename Allocator>
		typename vector<T, Allocator>::this_type&
		vector<T, Allocator>::operator=(this_type&& x)
		{
			if(this != &x)
			{
				DoClearCapacity(); // To consider: Are we really required to clear here? x is going away soon and will clear itself in its dtor.
				swap(x);           // member swap handles the case that x has a different allocator than our allocator by doing a copy.
			}
			return *this; 
		}
	#endif


	template <typename T, typename Allocator>
	inline void vector<T, Allocator>::assign(size_type n, const value_type& value)
	{
		DoAssignValues(n, value);
	}


	template <typename T, typename Allocator>
	template <typename InputIterator>                              
	inline void vector<T, Allocator>::assign(InputIterator first, InputIterator last)
	{
		// It turns out that the C++ std::vector<int, int> specifies a two argument
		// version of assign that takes (int size, int value). These are not iterators, 
		// so we need to do a template compiler trick to do the right thing.
		DoAssign<InputIterator, false>(first, last, is_integral<InputIterator>());
	}


	template <typename T, typename Allocator>
	inline void vector<T, Allocator>::assign(std::initializer_list<value_type> ilist)
	{
		typedef typename std::initializer_list<value_type>::iterator InputIterator;
		typedef typename eastl::iterator_traits<InputIterator>::iterator_category IC;
		DoAssignFromIterator<InputIterator, false>(ilist.begin(), ilist.end(), IC()); // initializer_list has const elements and so we can't move from them.
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::iterator
	vector<T, Allocator>::begin() EA_NOEXCEPT
	{
		return mpBegin;
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::const_iterator
	vector<T, Allocator>::begin() const EA_NOEXCEPT
	{
		return mpBegin;
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::const_iterator
	vector<T, Allocator>::cbegin() const EA_NOEXCEPT
	{
		return mpBegin;
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::iterator
	vector<T, Allocator>::end() EA_NOEXCEPT
	{
		return mpEnd;
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::const_iterator
	vector<T, Allocator>::end() const EA_NOEXCEPT
	{
		return mpEnd;
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::const_iterator
	vector<T, Allocator>::cend() const EA_NOEXCEPT
	{
		return mpEnd;
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::reverse_iterator
	vector<T, Allocator>::rbegin() EA_NOEXCEPT
	{
		return reverse_iterator(mpEnd);
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::const_reverse_iterator
	vector<T, Allocator>::rbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(mpEnd);
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::const_reverse_iterator
	vector<T, Allocator>::crbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(mpEnd);
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::reverse_iterator
	vector<T, Allocator>::rend() EA_NOEXCEPT
	{
		return reverse_iterator(mpBegin);
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::const_reverse_iterator
	vector<T, Allocator>::rend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(mpBegin);
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::const_reverse_iterator
	vector<T, Allocator>::crend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(mpBegin);
	}


	template <typename T, typename Allocator>
	bool vector<T, Allocator>::empty() const EA_NOEXCEPT
	{
		return (mpBegin == mpEnd);
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::size_type
	vector<T, Allocator>::size() const EA_NOEXCEPT
	{
		return (size_type)(mpEnd - mpBegin);
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::size_type
	vector<T, Allocator>::capacity() const EA_NOEXCEPT
	{
		return (size_type)(mpCapacity - mpBegin);
	}


	template <typename T, typename Allocator>
	inline void vector<T, Allocator>::resize(size_type n, const value_type& value)
	{
		if(n > (size_type)(mpEnd - mpBegin))  // We expect that more often than not, resizes will be upsizes.
			DoInsertValuesEnd(n - ((size_type)(mpEnd - mpBegin)), value);
		else
		{
			eastl::destruct(mpBegin + n, mpEnd);
			mpEnd = mpBegin + n;
		}
	}


	template <typename T, typename Allocator>
	inline void vector<T, Allocator>::resize(size_type n)
	{
		// Alternative implementation:
		// resize(n, value_type());

		if(n > (size_type)(mpEnd - mpBegin))  // We expect that more often than not, resizes will be upsizes.
			DoInsertValuesEnd(n - ((size_type)(mpEnd - mpBegin)));
		else
		{
			eastl::destruct(mpBegin + n, mpEnd);
			mpEnd = mpBegin + n;
		}
	}


	template <typename T, typename Allocator>
	void vector<T, Allocator>::reserve(size_type n)
	{
		// If the user wants to reduce the reserved memory, there is the set_capacity function.
		if(n > size_type(mpCapacity - mpBegin)) // If n > capacity ...
			DoGrow(n);
	}


	template <typename T, typename Allocator>
	void vector<T, Allocator>::set_capacity(size_type n)
	{
		if((n == npos) || (n <= (size_type)(mpEnd - mpBegin))) // If new capacity <= size...
		{
			if(n == 0)  // Very often n will be 0, and clear will be faster than resize and use less stack space.
				clear();
			else if(n < (size_type)(mpEnd - mpBegin))
				resize(n);

			this_type temp(*this);  // This is the simplest way to accomplish this, 
			swap(temp);             // and it is as efficient as any other.
		}
		else // Else new capacity > size.
		{
			pointer const pNewData = DoRealloc(n, mpBegin, mpEnd, should_move_tag());
			eastl::destruct(mpBegin, mpEnd);
			DoFree(mpBegin, (size_type)(mpCapacity - mpBegin));

			const ptrdiff_t nPrevSize = mpEnd - mpBegin;
			mpBegin    = pNewData;
			mpEnd      = pNewData + nPrevSize;
			mpCapacity = mpBegin + n;
		}
	}

	template <typename T, typename Allocator>
	inline void vector<T, Allocator>::shrink_to_fit()
	{
		// This is the simplest way to accomplish this, and it is as efficient as any other.

		#if EASTL_MOVE_SEMANTICS_ENABLED
			this_type temp = this_type(move_iterator<iterator>(begin()), move_iterator<iterator>(end()), mAllocator);
		#else
			this_type temp(*this);
		#endif
		// Call DoSwap() rather than swap() as we know our allocators match and we don't want to invoke the code path
		// handling non matching allocators as it imposes additional restrictions on the type of T to be copyable
		DoSwap(temp);
	}

	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::pointer
	vector<T, Allocator>::data() EA_NOEXCEPT
	{
		return mpBegin;
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::const_pointer
	vector<T, Allocator>::data() const EA_NOEXCEPT
	{
		return mpBegin;
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::reference
	vector<T, Allocator>::operator[](size_type n)
	{
		#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED    // We allow the user to use a reference to v[0] of an empty container. But this was merely grandfathered in and ideally we shouldn't allow such access to [0].
			if(EASTL_UNLIKELY((n != 0) && (n >= (static_cast<size_type>(mpEnd - mpBegin)))))
				EASTL_FAIL_MSG("vector::operator[] -- out of range");
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(n >= (static_cast<size_type>(mpEnd - mpBegin))))
				EASTL_FAIL_MSG("vector::operator[] -- out of range");
		#endif

		return *(mpBegin + n);
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::const_reference
	vector<T, Allocator>::operator[](size_type n) const
	{
		#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED    // We allow the user to use a reference to v[0] of an empty container. But this was merely grandfathered in and ideally we shouldn't allow such access to [0].
			if(EASTL_UNLIKELY((n != 0) && (n >= (static_cast<size_type>(mpEnd - mpBegin)))))
				EASTL_FAIL_MSG("vector::operator[] -- out of range");
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(n >= (static_cast<size_type>(mpEnd - mpBegin))))
				EASTL_FAIL_MSG("vector::operator[] -- out of range");
		#endif

		return *(mpBegin + n);
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::reference
	vector<T, Allocator>::at(size_type n)
	{
		// The difference between at and operator[] is that at signals 
		// if the requested position is out of range by throwing an 
		// out_of_range exception.

		#if EASTL_EXCEPTIONS_ENABLED
			if(EASTL_UNLIKELY(n >= (static_cast<size_type>(mpEnd - mpBegin))))
				throw std::out_of_range("vector::at -- out of range");
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(n >= (static_cast<size_type>(mpEnd - mpBegin))))
				EASTL_FAIL_MSG("vector::at -- out of range");
		#endif

		return *(mpBegin + n);
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::const_reference
	vector<T, Allocator>::at(size_type n) const
	{
		#if EASTL_EXCEPTIONS_ENABLED
			if(EASTL_UNLIKELY(n >= (static_cast<size_type>(mpEnd - mpBegin))))
				throw std::out_of_range("vector::at -- out of range");
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(n >= (static_cast<size_type>(mpEnd - mpBegin))))
				EASTL_FAIL_MSG("vector::at -- out of range");
		#endif

		return *(mpBegin + n);
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::reference
	vector<T, Allocator>::front()
	{
		#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			// We allow the user to reference an empty container.
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(mpEnd <= mpBegin)) // We don't allow the user to reference an empty container.
				EASTL_FAIL_MSG("vector::front -- empty vector");
		#endif

		return *mpBegin;
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::const_reference
	vector<T, Allocator>::front() const
	{
		#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			// We allow the user to reference an empty container.
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(mpEnd <= mpBegin)) // We don't allow the user to reference an empty container.
				EASTL_FAIL_MSG("vector::front -- empty vector");
		#endif

		return *mpBegin;
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::reference
	vector<T, Allocator>::back()
	{
		#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			// We allow the user to reference an empty container.
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(mpEnd <= mpBegin)) // We don't allow the user to reference an empty container.
				EASTL_FAIL_MSG("vector::back -- empty vector");
		#endif

		return *(mpEnd - 1);
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::const_reference
	vector<T, Allocator>::back() const
	{
		#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
			// We allow the user to reference an empty container.
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(mpEnd <= mpBegin)) // We don't allow the user to reference an empty container.
				EASTL_FAIL_MSG("vector::back -- empty vector");
		#endif

		return *(mpEnd - 1);
	}


	template <typename T, typename Allocator>
	inline void vector<T, Allocator>::push_back(const value_type& value)
	{
		if(mpEnd < mpCapacity)
			::new((void*)mpEnd++) value_type(value);
		else
			DoInsertValueEnd(value);
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename T, typename Allocator>
		inline void vector<T, Allocator>::push_back(value_type&& value)
		{
			if (mpEnd < mpCapacity)
				::new((void*)mpEnd++) value_type(eastl::move(value));
			else
				DoInsertValueEnd(eastl::move(value));
		}
	#endif


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::reference
	vector<T, Allocator>::push_back()
	{
		if(mpEnd < mpCapacity)
			::new((void*)mpEnd++) value_type();
		else // Note that in this case we create a temporary, which is less desirable.
			DoInsertValueEnd(value_type());

		return *(mpEnd - 1); // Same as return back();
	}


	template <typename T, typename Allocator>
	inline void* vector<T, Allocator>::push_back_uninitialized()
	{
		if(mpEnd == mpCapacity)
		{
			const size_type newSize = (size_type)(mpEnd - mpBegin) + 1;
			reserve(newSize);
		}
 
		return mpEnd++;
	}


	template <typename T, typename Allocator>
	inline void vector<T, Allocator>::pop_back()
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(mpEnd <= mpBegin))
				EASTL_FAIL_MSG("vector::pop_back -- empty vector");
		#endif

		--mpEnd;
		mpEnd->~value_type();
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED && EASTL_VARIADIC_TEMPLATES_ENABLED
		template <typename T, typename Allocator>
		template<class... Args>
		inline typename vector<T, Allocator>::iterator 
		vector<T, Allocator>::emplace(const_iterator position, Args&&... args)
		{
			const ptrdiff_t n = position - mpBegin; // Save this because we might reallocate.

			if((mpEnd == mpCapacity) || (position != mpEnd))
				DoInsertValue(position, eastl::forward<Args>(args)...);
			else
			{
				::new((void*)mpEnd) value_type(eastl::forward<Args>(args)...);
				++mpEnd; // Increment this after the construction above in case the construction throws an exception.
			}

			return mpBegin + n;
		}

		template <typename T, typename Allocator>
		template<class... Args>
		inline void vector<T, Allocator>::emplace_back(Args&&... args)
		{
			if(mpEnd < mpCapacity)
			{
				::new((void*)mpEnd) value_type(eastl::forward<Args>(args)...);  // If value_type has a move constructor, it will use it and this operation may be faster than otherwise.
				++mpEnd; // Increment this after the construction above in case the construction throws an exception.
			}
			else
				DoInsertValueEnd(eastl::forward<Args>(args)...);
		}
	#else
		#if EASTL_MOVE_SEMANTICS_ENABLED
			template <typename T, typename Allocator>
			inline typename vector<T, Allocator>::iterator 
			vector<T, Allocator>::emplace(const_iterator position, value_type&& value)
			{
				const ptrdiff_t n = position - mpBegin; // Save this because we might reallocate.

				if((mpEnd == mpCapacity) || (position != mpEnd))
					DoInsertValue(position, eastl::move(value));
				else
				{
					::new((void*)mpEnd) value_type(eastl::move(value));
					++mpEnd; // Increment this after the construction above in case the construction throws an exception.
				}

			return mpBegin + n;
			}

			template <typename T, typename Allocator>
			inline void vector<T, Allocator>::emplace_back(value_type&& value)
			{
				if(mpEnd < mpCapacity)
				{
					::new((void*)mpEnd) value_type(eastl::move(value));  // If value_type has a move constructor, it will use it and this operation may be faster than otherwise.
					++mpEnd; // Increment this after the construction above in case the construction throws an exception.
				}
				else
					DoInsertValueEnd(eastl::move(value));
			}
		#endif

		template <typename T, typename Allocator>
		inline typename vector<T, Allocator>::iterator 
		vector<T, Allocator>::emplace(const_iterator position, const value_type& value)
		{
			return insert(position, value);
		}

		template <typename T, typename Allocator>
		inline void vector<T, Allocator>::emplace_back(const value_type& value)
		{
			push_back(value);
		}
	#endif


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::iterator
	vector<T, Allocator>::insert(const_iterator position, const value_type& value)
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY((position < mpBegin) || (position > mpEnd)))
				EASTL_FAIL_MSG("vector::insert -- invalid position");
		#endif

		// We implment a quick pathway for the case that the insertion position is at the end and we have free capacity for it.
		const ptrdiff_t n = position - mpBegin; // Save this because we might reallocate.

		if((mpEnd == mpCapacity) || (position != mpEnd))
			DoInsertValue(position, value);
		else
		{
			::new((void*)mpEnd) value_type(value);
			++mpEnd; // Increment this after the construction above in case the construction throws an exception.
		}

		return mpBegin + n;
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename T, typename Allocator>       
		inline typename vector<T, Allocator>::iterator
		vector<T, Allocator>::insert(const_iterator position, value_type&& value)
		{
			return emplace(position, eastl::move(value));
		}
	#endif


	template <typename T, typename Allocator>
	inline void vector<T, Allocator>::insert(const_iterator position, size_type n, const value_type& value)
	{
		DoInsertValues(position, n, value);
	}


	template <typename T, typename Allocator>
	template <typename InputIterator>
	inline void vector<T, Allocator>::insert(const_iterator position, InputIterator first, InputIterator last)
	{
		DoInsert(position, first, last, is_integral<InputIterator>());
	}


	template <typename T, typename Allocator>       
	inline typename vector<T, Allocator>::iterator
	vector<T, Allocator>::insert(const_iterator position, std::initializer_list<value_type> ilist)
	{
		const ptrdiff_t n = position - mpBegin; // Save this because we might reallocate.
		DoInsert(position, ilist.begin(), ilist.end(), false_type());
		return mpBegin + n;
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::iterator
	vector<T, Allocator>::erase(const_iterator position)
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY((position < mpBegin) || (position >= mpEnd)))
				EASTL_FAIL_MSG("vector::erase -- invalid position");
		#endif

		// C++11 stipulates that position is const_iterator, but the return value is iterator.
		iterator destPosition = const_cast<value_type*>(position);        

		if((position + 1) < mpEnd)
			eastl::move(destPosition + 1, mpEnd, destPosition);
		--mpEnd;
		mpEnd->~value_type();
		return destPosition;
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::iterator
	vector<T, Allocator>::erase(const_iterator first, const_iterator last)
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY((first < mpBegin) || (first > mpEnd) || (last < mpBegin) || (last > mpEnd) || (last < first)))
				EASTL_FAIL_MSG("vector::erase -- invalid position");
		#endif
 
		iterator const position = const_cast<value_type*>(eastl::move(const_cast<value_type*>(last), const_cast<value_type*>(mpEnd), const_cast<value_type*>(first)));
		eastl::destruct(position, mpEnd);
		mpEnd -= (last - first);
 
		return const_cast<value_type*>(first);
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::iterator
	vector<T, Allocator>::erase_unsorted(const_iterator position)
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY((position < mpBegin) || (position >= mpEnd)))
				EASTL_FAIL_MSG("vector::erase -- invalid position");
		#endif

		// C++11 stipulates that position is const_iterator, but the return value is iterator.
		iterator destPosition = const_cast<value_type*>(position);
		*destPosition = *(mpEnd - 1);

		// pop_back();
		--mpEnd;
		mpEnd->~value_type();

		return destPosition;
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::reverse_iterator
	vector<T, Allocator>::erase(const_reverse_iterator position)
	{
		return reverse_iterator(erase((++position).base()));
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::reverse_iterator
	vector<T, Allocator>::erase(const_reverse_iterator first, const_reverse_iterator last)
	{
		// Version which erases in order from first to last.
		// difference_type i(first.base() - last.base());
		// while(i--)
		//     first = erase(first);
		// return first;

		// Version which erases in order from last to first, but is slightly more efficient:
		return reverse_iterator(erase(last.base(), first.base()));
	}


	template <typename T, typename Allocator>
	inline typename vector<T, Allocator>::reverse_iterator
	vector<T, Allocator>::erase_unsorted(const_reverse_iterator position)
	{
		return reverse_iterator(erase_unsorted((++position).base()));
	}


	template <typename T, typename Allocator>
	inline void vector<T, Allocator>::clear() EA_NOEXCEPT
	{
		eastl::destruct(mpBegin, mpEnd);
		mpEnd = mpBegin;
	}


	#if EASTL_RESET_ENABLED
		// This function name is deprecated; use reset_lose_memory instead.
		template <typename T, typename Allocator>
		inline void vector<T, Allocator>::reset() EA_NOEXCEPT
		{
			reset_lose_memory();
		}
	#endif


	template <typename T, typename Allocator>
	inline void vector<T, Allocator>::reset_lose_memory() EA_NOEXCEPT
	{
		// The reset function is a special extension function which unilaterally 
		// resets the container to an empty state without freeing the memory of 
		// the contained objects. This is useful for very quickly tearing down a 
		// container built into scratch memory.
		mpBegin = mpEnd = mpCapacity = NULL;
	}


	// swap exchanges the contents of two containers. With respect to the containers allocators,
	// the C11++ Standard (23.2.1/7) states that the behavior of a call to a container's swap function 
	// is undefined unless the objects being swapped have allocators that compare equal or 
	// allocator_traits<allocator_type>::propagate_on_container_swap::value is true (propagate_on_container_swap
	// is false by default). EASTL doesn't have allocator_traits and so this doesn't directly apply,
	// but EASTL has the effective behavior of propagate_on_container_swap = false for all allocators. 
	// So EASTL swap exchanges contents but not allocators, and swap is more efficient if allocators are equivalent.
	template <typename T, typename Allocator>
	inline void vector<T, Allocator>::swap(this_type& x)
	{
		if(mAllocator == x.mAllocator) // If allocators are equivalent...
			DoSwap(x);
		else // else swap the contents.
		{
			const this_type temp(*this); // Can't call eastl::swap because that would
			*this = x;                   // itself call this member swap function.
			x     = temp;
		}
	}


	template <typename T, typename Allocator>
	template <typename ForwardIterator>
	inline typename vector<T, Allocator>::pointer
	vector<T, Allocator>::DoRealloc(size_type n, ForwardIterator first, ForwardIterator last, should_copy_tag)
	{
		T* const p = DoAllocate(n); // p is of type T* but is not constructed. 
		eastl::uninitialized_copy_ptr(first, last, p); // copy-constructs p from [first,last).
		return p;
	}


	template <typename T, typename Allocator>
	template <typename ForwardIterator>
	inline typename vector<T, Allocator>::pointer
	vector<T, Allocator>::DoRealloc(size_type n, ForwardIterator first, ForwardIterator last, should_move_tag)
	{
		T* const p = DoAllocate(n); // p is of type T* but is not constructed. 
		eastl::uninitialized_move_ptr_if_noexcept(first, last, p); // move-constructs p from [first,last).
		return p;
	}


	template <typename T, typename Allocator>
	template <typename Integer>
	inline void vector<T, Allocator>::DoInit(Integer n, Integer value, true_type)
	{
		mpBegin    = DoAllocate((size_type)n);
		mpCapacity = mpBegin + n;
		mpEnd      = mpCapacity;

		typedef typename eastl::remove_const<T>::type non_const_value_type; // If T is a const type (e.g. const int) then we need to initialize it as if it were non-const.
		eastl::uninitialized_fill_n_ptr<value_type, Integer>((non_const_value_type*)mpBegin, n, value);
	}


	template <typename T, typename Allocator>
	template <typename InputIterator>
	inline void vector<T, Allocator>::DoInit(InputIterator first, InputIterator last, false_type)
	{
		typedef typename eastl::iterator_traits<InputIterator>:: iterator_category IC;
		DoInitFromIterator(first, last, IC());
	}


	template <typename T, typename Allocator>
	template <typename InputIterator>
	inline void vector<T, Allocator>::DoInitFromIterator(InputIterator first, InputIterator last, EASTL_ITC_NS::input_iterator_tag)
	{
		// To do: Use emplace_back instead of push_back(). Our emplace_back will work below without any ifdefs.
		for(; first < last; ++first)  // InputIterators by definition actually only allow you to iterate through them once.
			push_back(*first);        // Thus the standard *requires* that we do this (inefficient) implementation.
	}                                 // Luckily, InputIterators are in practice almost never used, so this code will likely never get executed.


	template <typename T, typename Allocator>
	template <typename ForwardIterator>
	inline void vector<T, Allocator>::DoInitFromIterator(ForwardIterator first, ForwardIterator last, EASTL_ITC_NS::forward_iterator_tag)
	{
		const size_type n = (size_type)eastl::distance(first, last);
		mpBegin    = DoAllocate(n);
		mpCapacity = mpBegin + n;
		mpEnd      = mpCapacity;

		typedef typename eastl::remove_const<T>::type non_const_value_type; // If T is a const type (e.g. const int) then we need to initialize it as if it were non-const.
		eastl::uninitialized_copy_ptr(first, last, (non_const_value_type*)mpBegin);
	}


	template <typename T, typename Allocator>
	template <typename Integer, bool bMove>
	inline void vector<T, Allocator>::DoAssign(Integer n, Integer value, true_type)
	{
		DoAssignValues(static_cast<size_type>(n), static_cast<value_type>(value));
	}


	template <typename T, typename Allocator>
	template <typename InputIterator, bool bMove>
	inline void vector<T, Allocator>::DoAssign(InputIterator first, InputIterator last, false_type)
	{
		typedef typename eastl::iterator_traits<InputIterator>::iterator_category IC;
		DoAssignFromIterator<InputIterator, bMove>(first, last, IC());
	}


	template <typename T, typename Allocator>
	void vector<T, Allocator>::DoAssignValues(size_type n, const value_type& value)
	{
		if(n > size_type(mpCapacity - mpBegin)) // If n > capacity ...
		{
			this_type temp(n, value, mAllocator); // We have little choice but to reallocate with new memory.
			swap(temp);
		}
		else if(n > size_type(mpEnd - mpBegin)) // If n > size ...
		{
			eastl::fill(mpBegin, mpEnd, value);
			eastl::uninitialized_fill_n_ptr(mpEnd, n - size_type(mpEnd - mpBegin), value);
			mpEnd += n - size_type(mpEnd - mpBegin);
		}
		else // else 0 <= n <= size
		{
			eastl::fill_n(mpBegin, n, value);
			erase(mpBegin + n, mpEnd);
		}
	}


	template <typename T, typename Allocator>
	template <typename InputIterator, bool bMove>
	void vector<T, Allocator>::DoAssignFromIterator(InputIterator first, InputIterator last, EASTL_ITC_NS::input_iterator_tag)
	{
		iterator position(mpBegin);

		while((position != mpEnd) && (first != last))
		{
			*position = *first;
			++first;
			++position;
		}
		if(first == last)
			erase(position, mpEnd);
		else
			insert(mpEnd, first, last);
	}


	template <typename T, typename Allocator>
	template <typename RandomAccessIterator, bool bMove>
	void vector<T, Allocator>::DoAssignFromIterator(RandomAccessIterator first, RandomAccessIterator last, EASTL_ITC_NS::random_access_iterator_tag)
	{
		const size_type n = (size_type)eastl::distance(first, last);

		if(n > size_type(mpCapacity - mpBegin)) // If n > capacity ...
		{
			pointer const pNewData = DoRealloc(n, first, last, bMove ? should_move_tag() : should_copy_tag());
			eastl::destruct(mpBegin, mpEnd);
			DoFree(mpBegin, (size_type)(mpCapacity - mpBegin));

			mpBegin    = pNewData;
			mpEnd      = mpBegin + n;
			mpCapacity = mpEnd;
		}
		else if(n <= size_type(mpEnd - mpBegin)) // If n <= size ...
		{
			pointer const pNewEnd = eastl::copy(first, last, mpBegin); // Since we are copying to mpBegin, we don't have to worry about needing copy_backward or a memmove-like copy (as opposed to memcpy-like copy).
			eastl::destruct(pNewEnd, mpEnd);
			mpEnd = pNewEnd;
		}
		else // else size < n <= capacity
		{
			RandomAccessIterator position = first + (mpEnd - mpBegin);
			eastl::copy(first, position, mpBegin); // Since we are copying to mpBegin, we don't have to worry about needing copy_backward or a memmove-like copy (as opposed to memcpy-like copy).
			mpEnd = eastl::uninitialized_copy_ptr(position, last, mpEnd);
		}
	}


	template <typename T, typename Allocator>
	template <typename Integer>
	inline void vector<T, Allocator>::DoInsert(const_iterator position, Integer n, Integer value, true_type)
	{
		DoInsertValues(position, static_cast<size_type>(n), static_cast<value_type>(value));
	}


	template <typename T, typename Allocator>
	template <typename InputIterator>
	inline void vector<T, Allocator>::DoInsert(const_iterator position, InputIterator first, InputIterator last, false_type)
	{
		typedef typename eastl::iterator_traits<InputIterator>::iterator_category IC;
		DoInsertFromIterator(position, first, last, IC());
	}


	template <typename T, typename Allocator>
	template <typename InputIterator>
	inline void vector<T, Allocator>::DoInsertFromIterator(const_iterator position, InputIterator first, InputIterator last, EASTL_ITC_NS::input_iterator_tag)
	{
		for(; first != last; ++first, ++position)
			position = insert(position, *first);
	}


	template <typename T, typename Allocator>
	template <typename BidirectionalIterator>
	void vector<T, Allocator>::DoInsertFromIterator(const_iterator position, BidirectionalIterator first, BidirectionalIterator last, EASTL_ITC_NS::bidirectional_iterator_tag)
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY((position < mpBegin) || (position > mpEnd)))
				EASTL_FAIL_MSG("vector::insert -- invalid position");
		#endif

		// C++11 stipulates that position is const_iterator, but the return value is iterator.
		iterator destPosition = const_cast<value_type*>(position);

		if(first != last)
		{
			const size_type n = (size_type)eastl::distance(first, last);  // n is the number of elements we are inserting.

			if(n <= size_type(mpCapacity - mpEnd)) // If n fits within the existing capacity...
			{
				const size_type nExtra = static_cast<size_type>(mpEnd - destPosition);

				if(n < nExtra) // If the inserted values are entirely within initialized memory (i.e. are before mpEnd)...
				{
					eastl::uninitialized_copy_ptr(mpEnd - n, mpEnd, mpEnd);
					eastl::copy_backward(destPosition, mpEnd - n, mpEnd); // We need copy_backward because of potential overlap issues.
					eastl::copy(first, last, destPosition);
				}
				else
				{
					BidirectionalIterator iTemp = first;
					eastl::advance(iTemp, nExtra);
					eastl::uninitialized_copy_ptr(iTemp, last, mpEnd);
					eastl::uninitialized_copy_ptr(destPosition, mpEnd, mpEnd + n - nExtra);
					eastl::copy_backward(first, iTemp, destPosition + nExtra);
				}

				mpEnd += n;
			}
			else // else we need to expand our capacity.
			{
				const size_type nPrevSize = size_type(mpEnd - mpBegin);
				const size_type nGrowSize = GetNewCapacity(nPrevSize);
				const size_type nNewSize  = nGrowSize > (nPrevSize + n) ? nGrowSize : (nPrevSize + n);
				pointer const   pNewData  = DoAllocate(nNewSize);

				#if EASTL_EXCEPTIONS_ENABLED
					pointer pNewEnd = pNewData;
					try
					{
						pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, destPosition, pNewData);
						pNewEnd = eastl::uninitialized_copy_ptr(first, last, pNewEnd);
						pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(destPosition, mpEnd, pNewEnd);
					}
					catch(...)
					{
						eastl::destruct(pNewData, pNewEnd);
						DoFree(pNewData, nNewSize);
						throw;
					}
				#else
					pointer pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, destPosition, pNewData);
					pNewEnd         = eastl::uninitialized_copy_ptr(first, last, pNewEnd);
					pNewEnd         = eastl::uninitialized_move_ptr_if_noexcept(destPosition, mpEnd, pNewEnd);
				#endif

				eastl::destruct(mpBegin, mpEnd);
				DoFree(mpBegin, (size_type)(mpCapacity - mpBegin));

				mpBegin    = pNewData;
				mpEnd      = pNewEnd;
				mpCapacity = pNewData + nNewSize;
			}
		}
	}


	template <typename T, typename Allocator>
	void vector<T, Allocator>::DoInsertValues(const_iterator position, size_type n, const value_type& value)
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY((position < mpBegin) || (position > mpEnd)))
				EASTL_FAIL_MSG("vector::insert -- invalid position");
		#endif

		// C++11 stipulates that position is const_iterator, but the return value is iterator.
		iterator destPosition = const_cast<value_type*>(position);

		if(n <= size_type(mpCapacity - mpEnd)) // If n is <= capacity...
		{
			if(n > 0) // To do: See if there is a way we can eliminate this 'if' statement.
			{
				// To consider: Make this algorithm work more like DoInsertValue whereby a pointer to value is used.
				const value_type temp  = value;
				const size_type nExtra = static_cast<size_type>(mpEnd - destPosition);

				if(n < nExtra)
				{
					eastl::uninitialized_copy_ptr(mpEnd - n, mpEnd, mpEnd);
					eastl::copy_backward(destPosition, mpEnd - n, mpEnd); // We need copy_backward because of potential overlap issues.
					eastl::fill(destPosition, destPosition + n, temp);
				}
				else
				{
					eastl::uninitialized_fill_n_ptr(mpEnd, n - nExtra, temp);
					eastl::uninitialized_copy_ptr(destPosition, mpEnd, mpEnd + n - nExtra);
					eastl::fill(destPosition, mpEnd, temp);
				}

				mpEnd += n;
			}
		}
		else // else n > capacity
		{
			const size_type nPrevSize = size_type(mpEnd - mpBegin);
			const size_type nGrowSize = GetNewCapacity(nPrevSize);
			const size_type nNewSize  = nGrowSize > (nPrevSize + n) ? nGrowSize : (nPrevSize + n);
			pointer const pNewData    = DoAllocate(nNewSize);

			#if EASTL_EXCEPTIONS_ENABLED
				pointer pNewEnd = pNewData;
				try
				{
					pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, destPosition, pNewData);
					eastl::uninitialized_fill_n_ptr(pNewEnd, n, value);
					pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(destPosition, mpEnd, pNewEnd + n);
				}
				catch(...)
				{
					eastl::destruct(pNewData, pNewEnd);
					DoFree(pNewData, nNewSize);
					throw;
				}
			#else
				pointer pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, destPosition, pNewData);
				eastl::uninitialized_fill_n_ptr(pNewEnd, n, value);
				pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(destPosition, mpEnd, pNewEnd + n);
			#endif

			eastl::destruct(mpBegin, mpEnd);
			DoFree(mpBegin, (size_type)(mpCapacity - mpBegin));

			mpBegin    = pNewData;
			mpEnd      = pNewEnd;
			mpCapacity = pNewData + nNewSize;
		}
	}


	template <typename T, typename Allocator>
	void vector<T, Allocator>::DoClearCapacity() // This function exists because set_capacity() currently indirectly requires value_type to be default-constructible, 
	{                                            // and some functions that need to clear our capacity (e.g. operator=) aren't supposed to require default-constructibility. 
		clear();
		this_type temp(*this);  // This is the simplest way to accomplish this, 
		swap(temp);             // and it is as efficient as any other.
	}


	template <typename T, typename Allocator>
	void vector<T, Allocator>::DoGrow(size_type n)
	{
		pointer const pNewData = DoAllocate(n);

		pointer pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, mpEnd, pNewData);

		eastl::destruct(mpBegin, mpEnd);
		DoFree(mpBegin, (size_type)(mpCapacity - mpBegin));

		mpBegin    = pNewData;
		mpEnd      = pNewEnd;
		mpCapacity = pNewData + n;
	}


	template <typename T, typename Allocator>
	inline void vector<T, Allocator>::DoSwap(this_type& x)
	{
		eastl::swap(mpBegin,    x.mpBegin);
		eastl::swap(mpEnd,      x.mpEnd);
		eastl::swap(mpCapacity, x.mpCapacity);
		eastl::swap(mAllocator, x.mAllocator);  // We do this even if EASTL_ALLOCATOR_COPY_ENABLED is 0.
	}

	// The code duplication between this and the version that takes no value argument and default constructs the values
	// is unfortunate but not easily resolved without relying on C++11 perfect forwarding.
	template <typename T, typename Allocator>
	void vector<T, Allocator>::DoInsertValuesEnd(size_type n, const value_type& value)
	{
		if(n > size_type(mpCapacity - mpEnd))
		{
			const size_type nPrevSize = size_type(mpEnd - mpBegin);
			const size_type nGrowSize = GetNewCapacity(nPrevSize);
			const size_type nNewSize = eastl::max(nGrowSize, nPrevSize + n);
			pointer const pNewData = DoAllocate(nNewSize);

			#if EASTL_EXCEPTIONS_ENABLED
				pointer pNewEnd = pNewData; // Assign pNewEnd a value here in case the copy throws.
				try
				{
					pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, mpEnd, pNewData);
				}
				catch(...)
				{
					eastl::destruct(pNewData, pNewEnd);
					DoFree(pNewData, nNewSize);
					throw;
				}
			#else
				pointer pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, mpEnd, pNewData);
			#endif

			eastl::uninitialized_fill_n_ptr(pNewEnd, n, value);
			pNewEnd += n;

			eastl::destruct(mpBegin, mpEnd);
			DoFree(mpBegin, (size_type)(mpCapacity - mpBegin));

			mpBegin    = pNewData;
			mpEnd      = pNewEnd;
			mpCapacity = pNewData + nNewSize;
		}
		else
		{
			eastl::uninitialized_fill_n_ptr(mpEnd, n, value);
			mpEnd += n;
		}
	}

	template <typename T, typename Allocator>
	void vector<T, Allocator>::DoInsertValuesEnd(size_type n)
	{
		if (n > size_type(mpCapacity - mpEnd))
		{
			const size_type nPrevSize = size_type(mpEnd - mpBegin);
			const size_type nGrowSize = GetNewCapacity(nPrevSize);
			const size_type nNewSize = eastl::max(nGrowSize, nPrevSize + n);
			pointer const pNewData = DoAllocate(nNewSize);

#if EASTL_EXCEPTIONS_ENABLED
			pointer pNewEnd = pNewData;  // Assign pNewEnd a value here in case the copy throws.
			try { pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, mpEnd, pNewData); }
			catch (...)
			{
				eastl::destruct(pNewData, pNewEnd);
				DoFree(pNewData, nNewSize);
				throw;
			}
#else
			pointer pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, mpEnd, pNewData);
#endif

			eastl::uninitialized_default_fill_n(pNewEnd, n);
			pNewEnd += n;

			eastl::destruct(mpBegin, mpEnd);
			DoFree(mpBegin, (size_type)(mpCapacity - mpBegin));

			mpBegin = pNewData;
			mpEnd = pNewEnd;
			mpCapacity = pNewData + nNewSize;
		}
		else
		{
			eastl::uninitialized_default_fill_n(mpEnd, n);
			mpEnd += n;
		}
	}

	#if EASTL_MOVE_SEMANTICS_ENABLED && EASTL_VARIADIC_TEMPLATES_ENABLED // If we can do variadic arguments...
		template <typename T, typename Allocator>
		template<typename... Args>
		void vector<T, Allocator>::DoInsertValue(const_iterator position, Args&&... args)
		{
			// To consider: It's feasible that the args is from a value_type comes from within the current sequence itself and 
			// so we need to be sure to handle that case. This is different from insert(position, const value_type&) because in 
			// this case value is potentially being modified.

			#if EASTL_ASSERT_ENABLED
				if(EASTL_UNLIKELY((position < mpBegin) || (position > mpEnd)))
					EASTL_FAIL_MSG("vector::insert/emplace -- invalid position");
			#endif

			// C++11 stipulates that position is const_iterator, but the return value is iterator.
			iterator destPosition = const_cast<value_type*>(position);

			if(mpEnd != mpCapacity) // If size < capacity ...
			{
				// We need to take into account the possibility that args is a value_type that comes from within the vector itself.
				// creating a temporary value on the stack here is not an optimal way to solve this because sizeof(value_type) may be
				// too much for the given platform. An alternative solution may be to specialize this function for the case of the
				// argument being const value_type& or value_type&&.
				EASTL_ASSERT(position < mpEnd);                                 // While insert at end() is valid, our design is such that calling code should handle that case before getting here, as our streamlined logic directly doesn't handle this particular case due to resulting negative ranges.
				#if EASTL_USE_FORWARD_WORKAROUND
					auto value = value_type(eastl::forward<Args>(args)...);     // Workaround for compiler bug in VS2013 which results in a compiler internal crash while compiling this code.
				#else
					value_type  value(eastl::forward<Args>(args)...);           // Need to do this before the move_backward below because maybe args refers to something within the moving range.
				#endif
				::new(static_cast<void*>(mpEnd)) value_type(eastl::move(*(mpEnd - 1)));      // mpEnd is uninitialized memory, so we must construct into it instead of move into it like we do with the other elements below.
				eastl::move_backward(destPosition, mpEnd - 1, mpEnd);           // We need to go backward because of potential overlap issues.
				eastl::destruct(destPosition);
				::new(static_cast<void*>(destPosition)) value_type(eastl::move(value));                             // Move the value argument to the given position.
				++mpEnd;
			}
			else // else (size == capacity)
			{
				const size_type nPosSize  = size_type(destPosition - mpBegin); // Index of the insertion position.
				const size_type nPrevSize = size_type(mpEnd - mpBegin);
				const size_type nNewSize  = GetNewCapacity(nPrevSize);
				pointer const   pNewData  = DoAllocate(nNewSize);

				#if EASTL_EXCEPTIONS_ENABLED
					pointer pNewEnd = pNewData;
					try
					{   // To do: We are not handling exceptions properly below.  In particular we don't want to 
						// call eastl::destruct on the entire range if only the first part of the range was costructed.
						::new((void*)(pNewData + nPosSize)) value_type(eastl::forward<Args>(args)...);              // Because the old data is potentially being moved rather than copied, we need to move.
						pNewEnd = NULL;                                                                             // Set to NULL so that in catch we can tell the exception occurred during the next call.
						pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, destPosition, pNewData);       // the value first, because it might possibly be a reference to the old data being moved.
						pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(destPosition, mpEnd, ++pNewEnd);
					}
					catch(...)
					{
						if(pNewEnd)
							eastl::destruct(pNewData, pNewEnd);                                         // Destroy what has been constructed so far.
						else
							eastl::destruct(pNewData + nPosSize);                                       // The exception occurred during the first unintialized move, so destroy only the value at nPosSize.
						DoFree(pNewData, nNewSize);
						throw;
					}
				#else
					::new((void*)(pNewData + nPosSize)) value_type(eastl::forward<Args>(args)...);                  // Because the old data is potentially being moved rather than copied, we need to move 
					pointer pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, destPosition, pNewData);   // the value first, because it might possibly be a reference to the old data being moved.
					pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(destPosition, mpEnd, ++pNewEnd);            // Question: with exceptions disabled, do we asssume all operations are noexcept and thus there's no need for uninitialized_move_ptr_if_noexcept?
				#endif

				eastl::destruct(mpBegin, mpEnd);
				DoFree(mpBegin, (size_type)(mpCapacity - mpBegin));

				mpBegin    = pNewData;
				mpEnd      = pNewEnd;
				mpCapacity = pNewData + nNewSize;
			}
		}
	#else
		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Note: The following two sets of two functions are nearly copies of the above two functions.
		// We (nearly) duplicate code here instead of trying to fold the all nine of these functions into 
		// three more generic functions because: 1) you can't really make just three functions but rather 
		// would need to break them apart somewhat, and 2) these duplications are eventually going away 
		// because they aren't needed with C++11 compilers, though that may not be until the year 2020.
		////////////////////////////////////////////////////////////////////////////////////////////////////

		#if EASTL_MOVE_SEMANTICS_ENABLED
			// To consider: Is there any practical means to merge the following DoInsertValue functions? 
			// They are 90% the same as each other with the only difference being the use of eastl::move(value) usage.
			// However, it isn't simple to fold that difference because value_type& and value_type&& are treated 
			// significantly differently and constructing objects with them executes different code.

			template <typename T, typename Allocator>
			void vector<T, Allocator>::DoInsertValue(const_iterator position, value_type&& value)
			{
				// To consider: It's feasible that value comes from within the current sequence itself and so we need to be 
				// sure to handle that case. This is different from insert(position, const value_type&) because in this case 
				// value is potentially being modified.

				#if EASTL_ASSERT_ENABLED
					if(EASTL_UNLIKELY((position < mpBegin) || (position > mpEnd)))
						EASTL_FAIL_MSG("vector::insert/emplace -- invalid position");
				#endif

				// C++11 stipulates that position is const_iterator, but the return value is iterator.
				iterator destPosition = const_cast<value_type*>(position);

				if(mpEnd != mpCapacity) // If size < capacity (and we can do this without reallocation)...
				{
					// We need to take into account the possibility that value may come from within the vector itself.
					EASTL_ASSERT(position < mpEnd);                                 // While insert at end() is valid, our design is such that calling code should handle that case before getting here, as our streamlined logic directly doesn't handle this particular case due to resulting negative ranges.
					const T* pValue = &value;
					if((pValue >= destPosition) && (pValue < mpEnd))                // If value comes from within the range to be moved...
						++pValue;                                                   // Set pValue to be where it will be after the copy.
					::new(static_cast<void*>(mpEnd)) value_type(eastl::move(*(mpEnd - 1)));      // mpEnd is uninitialized memory, so we must construct into it instead of move into it like we do with the other elements below.
					eastl::move_backward(destPosition, mpEnd - 1, mpEnd);           // We need to go backward because of potential overlap issues.
					eastl::destruct(destPosition);
					::new(static_cast<void*>(destPosition)) value_type(eastl::move(value));                             // Move the value argument to the given position.
					++mpEnd;
				}
				else // else (size == capacity)
				{
					const size_type nPosSize  = size_type(destPosition - mpBegin); // Index of the insertion position.
					const size_type nPrevSize = size_type(mpEnd - mpBegin);
					const size_type nNewSize  = GetNewCapacity(nPrevSize);
					pointer const   pNewData  = DoAllocate(nNewSize);

					#if EASTL_EXCEPTIONS_ENABLED
						pointer pNewEnd = pNewData;
						try
						{
							::new((void*)(pNewData + nPosSize)) value_type(eastl::move(value));                         // Because the old data is being moved rather than copied, we need to move the value first, 
							pNewEnd = NULL;                                                                             // Set to NULL so that in catch we can tell the exception occurred during the next call.
							pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, destPosition, pNewData);       // because it might possibly be a reference to the old data being moved.
							pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(destPosition, mpEnd, ++pNewEnd);
						}
						catch(...)
						{
							if(pNewEnd)
								eastl::destruct(pNewData, pNewEnd);                                         // Destroy what has been constructed so far.
							else
								eastl::destruct(pNewData + nPosSize);                                       // The exception occurred during the first unintialized move, so destroy only the value at nPosSize.
							DoFree(pNewData, nNewSize);
							throw;
						}
					#else
						::new((void*)(pNewData + nPosSize)) value_type(eastl::move(value));                             // Because the old data is being moved rather than copied, we need to move the value first, 
						pointer pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, destPosition, pNewData);   // because it might possibly be a reference to the old data being moved.
						pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(destPosition, mpEnd, ++pNewEnd);
					#endif

					eastl::destruct(mpBegin, mpEnd);
					DoFree(mpBegin, (size_type)(mpCapacity - mpBegin));

					mpBegin    = pNewData;
					mpEnd      = pNewEnd;
					mpCapacity = pNewData + nNewSize;
				}
			}
		#endif


		template <typename T, typename Allocator>
		void vector<T, Allocator>::DoInsertValue(const_iterator position, const value_type& value)
		{
			#if EASTL_ASSERT_ENABLED
				if(EASTL_UNLIKELY((position < mpBegin) || (position > mpEnd)))
					EASTL_FAIL_MSG("vector::insert/emplace -- invalid position");
			#endif

			// C++11 stipulates that position is const_iterator, but the return value is iterator.
			iterator destPosition = const_cast<value_type*>(position);

			if(mpEnd != mpCapacity) // If size < capacity ...
			{
				// We need to take into account the possibility that value may come from within the vector itself.
				EASTL_ASSERT(position < mpEnd);                                 // While insert at end() is valid, our design is such that calling code should handle that case before getting here, as our streamlined logic directly doesn't handle this particular case due to resulting negative ranges.
				const T* pValue = &value;
				if((pValue >= destPosition) && (pValue < mpEnd))        // If value comes from within the range to be moved...
					++pValue;                                           // Set pValue to be where it will be after the copy.
				::new((void*)mpEnd) value_type(*(mpEnd - 1));           // mpEnd is uninitialized memory, so we must construct into it instead of move into it like we do with the other elements below.
				eastl::move_backward(destPosition, mpEnd - 1, mpEnd);   // We need to go backward because of potential overlap issues.
				*destPosition = *pValue;                                // Copy the value argument to the given position.
				++mpEnd;
			}
			else // else (size == capacity)
			{
				const size_type nPosSize  = size_type(destPosition - mpBegin); // Index of the insertion position.
				const size_type nPrevSize = size_type(mpEnd - mpBegin);
				const size_type nNewSize  = GetNewCapacity(nPrevSize);
				pointer const   pNewData  = DoAllocate(nNewSize);

				#if EASTL_EXCEPTIONS_ENABLED
					pointer pNewEnd = pNewData;
					try
					{
						::new((void*)(pNewData + nPosSize)) value_type(value);                                      // Because the old data is being moved rather than copied, we need to move the value first, 
						pNewEnd = NULL;                                                                             // Set to NULL so that in catch we can tell the exception occurred during the next call.
						pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, destPosition, pNewData);       // because it might possibly be a reference to the old data being moved.
						pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(destPosition, mpEnd, ++pNewEnd);
					}
					catch(...)
					{
						if(pNewEnd)
							eastl::destruct(pNewData, pNewEnd);                                         // Destroy what has been constructed so far.
						else
							eastl::destruct(pNewData + nPosSize);                                       // The exception occurred during the first unintialized move, so destroy only the value at nPosSize.
						DoFree(pNewData, nNewSize);
						throw;
					}
				#else
					::new((void*)(pNewData + nPosSize)) value_type(value);                                          // Because the old data is being moved rather than copied, we need to move the value first, 
					pointer pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, destPosition, pNewData);   // because it might possibly be a reference to the old data being moved.
					pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(destPosition, mpEnd, ++pNewEnd);
				#endif

				eastl::destruct(mpBegin, mpEnd);
				DoFree(mpBegin, (size_type)(mpCapacity - mpBegin));

				mpBegin    = pNewData;
				mpEnd      = pNewEnd;
				mpCapacity = pNewData + nNewSize;
			}
		}

	#endif


	#if EASTL_MOVE_SEMANTICS_ENABLED && EASTL_VARIADIC_TEMPLATES_ENABLED
		template <typename T, typename Allocator>
		template<typename... Args>
		void vector<T, Allocator>::DoInsertValueEnd(Args&&... args)
		{
			const size_type nPrevSize = size_type(mpEnd - mpBegin);
			const size_type nNewSize  = GetNewCapacity(nPrevSize);
			pointer const   pNewData  = DoAllocate(nNewSize);

			#if EASTL_EXCEPTIONS_ENABLED
				pointer pNewEnd = pNewData; // Assign pNewEnd a value here in case the copy throws.
				try
				{
					pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, mpEnd, pNewData);
					::new((void*)pNewEnd) value_type(eastl::forward<Args>(args)...);
					pNewEnd++;
				}
				catch(...)
				{
					eastl::destruct(pNewData, pNewEnd);
					DoFree(pNewData, nNewSize);
					throw;
				}
			#else
				pointer pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, mpEnd, pNewData);
				::new((void*)pNewEnd) value_type(eastl::forward<Args>(args)...);
				pNewEnd++;
			#endif

			eastl::destruct(mpBegin, mpEnd);
			DoFree(mpBegin, (size_type)(mpCapacity - mpBegin));

			mpBegin    = pNewData;
			mpEnd      = pNewEnd;
			mpCapacity = pNewData + nNewSize;
		}
	#else
		#if EASTL_MOVE_SEMANTICS_ENABLED
			template <typename T, typename Allocator>
			void vector<T, Allocator>::DoInsertValueEnd(value_type&& value)
			{
				const size_type nPrevSize = size_type(mpEnd - mpBegin);
				const size_type nNewSize  = GetNewCapacity(nPrevSize);
				pointer const   pNewData  = DoAllocate(nNewSize);

				#if EASTL_EXCEPTIONS_ENABLED
					pointer pNewEnd = pNewData; // Assign pNewEnd a value here in case the copy throws.
					try
					{
						pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, mpEnd, pNewData);
						::new((void*)pNewEnd) value_type(eastl::move(value));
						pNewEnd++;
					}
					catch(...)
					{
						eastl::destruct(pNewData, pNewEnd);
						DoFree(pNewData, nNewSize);
						throw;
					}
				#else
					pointer pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, mpEnd, pNewData);
					::new((void*)pNewEnd) value_type(eastl::move(value));
					pNewEnd++;
				#endif

				eastl::destruct(mpBegin, mpEnd);
				DoFree(mpBegin, (size_type)(mpCapacity - mpBegin));

				mpBegin    = pNewData;
				mpEnd      = pNewEnd;
				mpCapacity = pNewData + nNewSize;
			}
		#endif

		template <typename T, typename Allocator>
		void vector<T, Allocator>::DoInsertValueEnd(const value_type& value)
		{
			const size_type nPrevSize = size_type(mpEnd - mpBegin);
			const size_type nNewSize  = GetNewCapacity(nPrevSize);
			pointer const   pNewData  = DoAllocate(nNewSize);

			#if EASTL_EXCEPTIONS_ENABLED
				pointer pNewEnd = pNewData; // Assign pNewEnd a value here in case the copy throws.
				try
				{
					pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, mpEnd, pNewData);
					::new((void*)pNewEnd) value_type(value);
					pNewEnd++;
				}
				catch(...)
				{
					eastl::destruct(pNewData, pNewEnd);
					DoFree(pNewData, nNewSize);
					throw;
				}
			#else
				pointer pNewEnd = eastl::uninitialized_move_ptr_if_noexcept(mpBegin, mpEnd, pNewData);
				::new((void*)pNewEnd) value_type(value);
				pNewEnd++;
			#endif

			eastl::destruct(mpBegin, mpEnd);
			DoFree(mpBegin, (size_type)(mpCapacity - mpBegin));

			mpBegin    = pNewData;
			mpEnd      = pNewEnd;
			mpCapacity = pNewData + nNewSize;
		}
	#endif


	template <typename T, typename Allocator>
	inline bool vector<T, Allocator>::validate() const EA_NOEXCEPT
	{
		if(mpEnd < mpBegin)
			return false;
		if(mpCapacity < mpEnd)
			return false;
		return true;
	}


	template <typename T, typename Allocator>
	inline int vector<T, Allocator>::validate_iterator(const_iterator i) const EA_NOEXCEPT
	{
		if(i >= mpBegin)
		{
			if(i < mpEnd)
				return (isf_valid | isf_current | isf_can_dereference);

			if(i <= mpEnd)
				return (isf_valid | isf_current);
		}

		return isf_none;
	}



	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Allocator>
	inline bool operator==(const vector<T, Allocator>& a, const vector<T, Allocator>& b)
	{
		return ((a.size() == b.size()) && equal(a.begin(), a.end(), b.begin()));
	}


	template <typename T, typename Allocator>
	inline bool operator!=(const vector<T, Allocator>& a, const vector<T, Allocator>& b)
	{
		return ((a.size() != b.size()) || !equal(a.begin(), a.end(), b.begin()));
	}


	template <typename T, typename Allocator>
	inline bool operator<(const vector<T, Allocator>& a, const vector<T, Allocator>& b)
	{
		return lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
	}


	template <typename T, typename Allocator>
	inline bool operator>(const vector<T, Allocator>& a, const vector<T, Allocator>& b)
	{
		return b < a;
	}


	template <typename T, typename Allocator>
	inline bool operator<=(const vector<T, Allocator>& a, const vector<T, Allocator>& b)
	{
		return !(b < a);
	}


	template <typename T, typename Allocator>
	inline bool operator>=(const vector<T, Allocator>& a, const vector<T, Allocator>& b)
	{
		return !(a < b);
	}


	template <typename T, typename Allocator>
	inline void swap(vector<T, Allocator>& a, vector<T, Allocator>& b)
	{
		a.swap(b);
	}


} // namespace eastl


#ifdef _MSC_VER
	#pragma warning(pop)
#endif


#endif // Header include guard










