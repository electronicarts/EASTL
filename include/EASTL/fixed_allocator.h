/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements the following
//     fixed_allocator
//     fixed_allocator_with_overflow
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_FIXED_ALLOCATOR_H
#define EASTL_FIXED_ALLOCATOR_H


#include <EASTL/internal/config.h>
#include <EASTL/internal/fixed_pool.h>
#include <EASTL/functional.h>
#include <EASTL/memory.h>
#include <EASTL/allocator.h>
#include <EASTL/type_traits.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
	#include <new>
	#pragma warning(pop)
#else
	#include <new>
#endif

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 4275) // non dll-interface class used as base for DLL-interface classkey 'identifier'
#endif

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	///////////////////////////////////////////////////////////////////////////
	// fixed_allocator
	///////////////////////////////////////////////////////////////////////////

	/// fixed_allocator
	///
	/// Implements an allocator which allocates a single fixed size where 
	/// the size, alignment, and memory used for the pool is defined at 
	/// runtime by the user. This is different from fixed containers 
	/// such as fixed_list whereby the size and alignment are determined
	/// at compile time and the memory is directly built into the container's
	/// member data.
	///
	/// If the pool's memory is exhausted or was never initialized, the 
	/// allocate function returns NULL. Consider the fixed_allocator_with_overflow 
	/// class as an alternative in order to deal with this situation.
	///
	/// This class requires the user to call container.get_allocator().init() 
	/// after constructing the container. There currently isn't a way to 
	/// construct the container with the initialization parameters, though
	/// with some effort such a thing could probably be made possible. 
	/// It's not as simple as it might first seem, due to the non-copyable
	/// nature of fixed allocators. A side effect of this limitation is that
	/// you cannot copy-construct a container using fixed_allocators.
	///
	/// Another side-effect is that you cannot swap two containers using
	/// a fixed_allocator, as a swap requires temporary memory allocated by
	/// an equivalent allocator, and such a thing cannot be done implicitly.
	/// A workaround for the swap limitation is that you can implement your
	/// own swap whereby you provide an explicitly created temporary object.
	///
	/// Note: Be careful to set the allocator's node size to the size of the 
	/// container node and not the size of the contained object. Note that the 
	/// example code below uses IntListNode.
	///
	/// Example usage:
	///     typedef eastl::list<int, fixed_allocator> IntList;
	///     typedef IntList::node_type                IntListNode;
	///
	///     IntListNode buffer[200];
	///     IntList     intList;
	///     intList.get_allocator().init(buffer, sizeof(buffer), sizeof(IntListNode), __alignof(IntListNode));
	///
	class EASTL_API fixed_allocator : public fixed_pool_base
	{
	public:
		/// fixed_allocator
		///
		/// Default constructor. The user usually will need to call init() after  
		/// constructing via this constructor. 
		///
		fixed_allocator(const char* /*pName*/ = EASTL_FIXED_POOL_DEFAULT_NAME)
			: fixed_pool_base(NULL)
		{
		}


		/// fixed_allocator
		///
		/// Copy constructor. The user usually will need to call init() after  
		/// constructing via this constructor. By their nature, fixed-allocators
		/// cannot be copied in any useful way, as by their nature the user
		/// must manually initialize them.
		///
		fixed_allocator(const fixed_allocator&)
			: fixed_pool_base(NULL)
		{
		}


		/// operator=
		///
		/// By their nature, fixed-allocators cannot be copied in any 
		/// useful way, as by their nature the user must manually 
		/// initialize them.
		///
		fixed_allocator& operator=(const fixed_allocator&)
		{
			return *this;
		}


		// init
		//
		// No init here, as the base class version is sufficient.
		//
		//void init(void* pMemory, size_t memorySize, size_t nodeSize,
		//            size_t alignment, size_t alignmentOffset = 0);


		/// allocate
		///
		/// Allocates a new object of the size specified upon class initialization.
		/// Returns NULL if there is no more memory. 
		///
		void* allocate(size_t n, int /*flags*/ = 0)
		{
			// To consider: Verify that 'n' is what the user initialized us with.

			Link* pLink = mpHead;

			if(pLink) // If we have space...
			{
				#if EASTL_FIXED_SIZE_TRACKING_ENABLED
					if(++mnCurrentSize > mnPeakSize)
						mnPeakSize = mnCurrentSize;
				#endif

				mpHead = pLink->mpNext;
				return pLink;
			}
			else
			{
				// If there's no free node in the free list, just
				// allocate another from the reserved memory area

				if(mpNext != mpCapacity)
				{
					pLink = mpNext;
					
					mpNext = reinterpret_cast<Link*>(reinterpret_cast<char8_t*>(mpNext) + n);

					#if EASTL_FIXED_SIZE_TRACKING_ENABLED
						if(++mnCurrentSize > mnPeakSize)
							mnPeakSize = mnCurrentSize;
					#endif

					return pLink;
				}

				// EASTL_ASSERT(false); To consider: enable this assert. However, we intentionally disable it because this isn't necessarily an assertable error.
				return NULL;
			}
		}


		/// allocate
		///
		void* allocate(size_t n, size_t /*alignment*/, size_t /*offset*/, int flags = 0)
		{
			return allocate(n, flags);
		}


		/// deallocate
		///
		/// Frees the given object which was allocated by allocate(). 
		/// If the given node was not allocated by allocate() then the behaviour 
		/// is undefined.
		///
		void deallocate(void* p, size_t)
		{
			#if EASTL_FIXED_SIZE_TRACKING_ENABLED
				--mnCurrentSize;
			#endif

			((Link*)p)->mpNext = mpHead;
			mpHead = ((Link*)p);
		}


		using fixed_pool_base::can_allocate;


		const char* get_name() const
		{
			return EASTL_FIXED_POOL_DEFAULT_NAME;
		}


		void set_name(const char*)
		{
			// Nothing to do. We don't allocate memory.
		}

	}; // fixed_allocator

	bool operator==(const fixed_allocator& a, const fixed_allocator& b);
	bool operator!=(const fixed_allocator& a, const fixed_allocator& b);



	///////////////////////////////////////////////////////////////////////////
	// fixed_allocator_with_overflow
	///////////////////////////////////////////////////////////////////////////

	/// fixed_allocator_with_overflow
	///
	/// Implements an allocator which allocates a single fixed size where 
	/// the size, alignment, and memory used for the pool is defined at 
	/// runtime by the user. This is different from fixed containers 
	/// such as fixed_list whereby the size and alignment are determined
	/// at compile time and the memory is directly built into the container's
	/// member data.
	///
	/// Note: Be careful to set the allocator's node size to the size of the 
	/// container node and not the size of the contained object. Note that the 
	/// example code below uses IntListNode.
	///
	/// This class requires the user to call container.get_allocator().init() 
	/// after constructing the container. There currently isn't a way to 
	/// construct the container with the initialization parameters, though
	/// with some effort such a thing could probably be made possible. 
	/// It's not as simple as it might first seem, due to the non-copyable
	/// nature of fixed allocators. A side effect of this limitation is that
	/// you cannot copy-construct a container using fixed_allocators.
	///
	/// Another side-effect is that you cannot swap two containers using
	/// a fixed_allocator, as a swap requires temporary memory allocated by
	/// an equivalent allocator, and such a thing cannot be done implicitly.
	/// A workaround for the swap limitation is that you can implement your
	/// own swap whereby you provide an explicitly created temporary object.
	///
	/// Example usage:
	///     typedef eastl::list<int, fixed_allocator_with_overflow> IntList;
	///     typedef IntList::node_type                              IntListNode;
	///
	///     IntListNode buffer[200];
	///     IntList     intList;
	///     intList.get_allocator().init(buffer, sizeof(buffer), sizeof(IntListNode), __alignof(IntListNode));
	///
	class EASTL_API fixed_allocator_with_overflow : public fixed_pool_base
	{
	public:
		/// fixed_allocator_with_overflow
		///
		/// Default constructor. The user usually will need to call init() after  
		/// constructing via this constructor. 
		///
		fixed_allocator_with_overflow(const char* pName = EASTL_FIXED_POOL_DEFAULT_NAME)
			: fixed_pool_base(NULL),
			  mOverflowAllocator(pName)
		{
		}


		/// fixed_allocator_with_overflow
		///
		/// Copy constructor. The user usually will need to call init() after  
		/// constructing via this constructor. By their nature, fixed-allocators
		/// cannot be copied in any useful way, as by their nature the user
		/// must manually initialize them.
		///
		fixed_allocator_with_overflow(const fixed_allocator_with_overflow&)
			: fixed_pool_base(NULL)
		{
		}


		/// operator=
		///
		/// By their nature, fixed-allocators cannot be copied in any 
		/// useful way, as by their nature the user must manually 
		/// initialize them.
		///
		fixed_allocator_with_overflow& operator=(const fixed_allocator_with_overflow& x)
		{
			#if EASTL_ALLOCATOR_COPY_ENABLED
				mOverflowAllocator = x.mOverflowAllocator;
			#else
				(void)x;
			#endif

			return *this;
		}


		/// init
		///
		void init(void* pMemory, size_t memorySize, size_t nodeSize,
					size_t alignment, size_t alignmentOffset = 0)
		{
			fixed_pool_base::init(pMemory, memorySize, nodeSize, alignment, alignmentOffset);

			mpPoolBegin = pMemory;
			mpPoolEnd   = (void*)((uintptr_t)pMemory + memorySize);
			mnNodeSize  = (eastl_size_t)nodeSize;
		}


		/// allocate
		///
		/// Allocates a new object of the size specified upon class initialization.
		/// Returns NULL if there is no more memory. 
		///
		void* allocate(size_t /*n*/, int /*flags*/ = 0)
		{
			// To consider: Verify that 'n' is what the user initialized us with.

			void* p;

			if(mpHead) // If we have space...
			{
				p      = mpHead;
				mpHead = mpHead->mpNext;
			}
			else
			{
				// If there's no free node in the free list, just
				// allocate another from the reserved memory area

				if (mpNext != mpCapacity)
				{
					p = mpNext;
					mpNext = reinterpret_cast<Link*>(reinterpret_cast<char8_t*>(mpNext) + mnNodeSize);
				}
				else
					p = mOverflowAllocator.allocate(mnNodeSize);
			}

			#if EASTL_FIXED_SIZE_TRACKING_ENABLED
				if(p && (++mnCurrentSize > mnPeakSize))
					mnPeakSize = mnCurrentSize;
			#endif

			return p;
		}


		/// allocate
		///
		void* allocate(size_t n, size_t /*alignment*/, size_t /*offset*/, int flags = 0)
		{
			return allocate(n, flags);
		}


		/// deallocate
		///
		/// Frees the given object which was allocated by allocate(). 
		/// If the given node was not allocated by allocate() then the behaviour 
		/// is undefined.
		///
		void deallocate(void* p, size_t)
		{
			#if EASTL_FIXED_SIZE_TRACKING_ENABLED
				--mnCurrentSize;
			#endif

			if((p >= mpPoolBegin) && (p < mpPoolEnd))
			{
				((Link*)p)->mpNext = mpHead;
				mpHead = ((Link*)p);
			}
			else
				mOverflowAllocator.deallocate(p, (size_t)mnNodeSize);
		}


		using fixed_pool_base::can_allocate;


		const char* get_name() const
		{
			return mOverflowAllocator.get_name();
		}


		void set_name(const char* pName)
		{
			mOverflowAllocator.set_name(pName);
		}

	protected:
		EASTLAllocatorType mOverflowAllocator;  // To consider: Allow the user to define the type of this, presumably via a template parameter.
		void*              mpPoolBegin;         // To consider: We have these member variables and ideally we shouldn't need them. The problem is that 
		void*              mpPoolEnd;           //              the information about the pool buffer and object size is stored in the owning container 
		eastl_size_t       mnNodeSize;          //              and we can't have access to it without increasing the amount of code we need and by templating 
												//              more code. It may turn out that simply storing data here is smaller in the end.
	}; // fixed_allocator_with_overflow         //              Granted, this class is usually used for debugging purposes, but perhaps there is an elegant solution.

	bool operator==(const fixed_allocator_with_overflow& a, const fixed_allocator_with_overflow& b);
	bool operator!=(const fixed_allocator_with_overflow& a, const fixed_allocator_with_overflow& b);






	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	inline bool operator==(const fixed_allocator&, const fixed_allocator&)
	{
		return false;
	}

	inline bool operator!=(const fixed_allocator&, const fixed_allocator&)
	{
		return false;
	}

	inline bool operator==(const fixed_allocator_with_overflow&, const fixed_allocator_with_overflow&)
	{
		return false;
	}

	inline bool operator!=(const fixed_allocator_with_overflow&, const fixed_allocator_with_overflow&)
	{
		return false;
	}


} // namespace eastl


#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


#endif // Header include guard



