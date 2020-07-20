///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Note (March 2014): shared_array is not a full implementation of an array version 
// of C++11 shared_ptr, and there currently are no plans to make it so. A future
// version of shared_ptr would likely take on the ability to store arrays,
// same as unique_ptr has array support. This class isn't deprecated, but it
// is frozen until some future decision is made on what to do about arrays.
//
///////////////////////////////////////////////////////////////////////////////
// This class implements a shared_array template. This is a class which is 
// similar to the C++ shared_ptr template, except that it works with arrays
// instead of individual objects.
//
// Important notice:
// As of this writing (9/2003), the implementation provided here has 
// limitations that you should be aware of. These limitations will be shortly
// rectified. Most significantly, this implementation has the following 
// weaknesses:
//     - It cannot safely deal with exceptions that occur during the 
//       construction of shared_ptr objects. 
//     - It cannot safely deal with recursive shared_ptr objects. 
//       If a shared_ptr<A> holds a pointer to an instance of A and 
//       class A owns an instance of shared_ptr<A> that refers to,
//       the original instance, the memory will leak.
//     - A template of type shared_ptr<void> will not call the destructor
//       for an object that it stores. You thus must declare a shared_ptr
//       template specifically for the class type.
//     - It doesn't safely handle multiple instances of shared_ptr
//       which own the same pointer accessed from multiple threads.
//       This weakness is by design, for performance reasons. You should
//       use shared_ptr_mt for multi-thread safe access.
//
// The rectification of the above issues are discussed in the C++ standardization
// documents for the next C++ standard (as of 2003):
//     http://std.dkuug.dk/jtc1/sc22/wg21/docs/papers/2003/n1450.html#Implementation-difficulty
// 
// This current implementation will be eventually (hopefully by 1/2004) rectified 
// to be in line with the second generation C++ standard proposal.
//
// The intended design of this class is based somewhat on the design of the Boost
// shared_array template. This design is also being considered for the next C++ 
// standard (as of 2003). The C++ standard update proposal is currently available at:
//     http://std.dkuug.dk/jtc1/sc22/wg21/docs/papers/2003/n1450.html
// Boost smart pointers, including shared_array are documented at:
//     http://www.boost.org/libs/smart_ptr/
//
// As of this writing (10/2003), this class has received approval from EA legal
// for use. The potential issue is the similarity of the class name and class 
// interface to existing open source code.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_SHARED_ARRAY_H
#define EASTL_SHARED_ARRAY_H


#include <EASTL/internal/config.h>
#include <EASTL/internal/smart_ptr.h>   // Defines smart_array_deleter


EA_DISABLE_ALL_VC_WARNINGS();

	#include <new>
	#include <stddef.h>

EA_RESTORE_ALL_VC_WARNINGS();

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// EASTL_SHARED_ARRAY_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_SHARED_ARRAY_DEFAULT_NAME
		#define EASTL_SHARED_ARRAY_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " shared_array" // Unless the user overrides something, this is "EASTL shared_array".
	#endif


	/// EASTL_SHARED_ARRAY_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_SHARED_ARRAY_DEFAULT_ALLOCATOR
		#define EASTL_SHARED_ARRAY_DEFAULT_ALLOCATOR allocator_type(EASTL_SHARED_ARRAY_DEFAULT_NAME)
	#endif



	/// class shared_array
	/// A shared_array is the same as shared_ptr but for arrays. 
	template <typename T, typename Allocator = EASTLAllocatorType, typename Deleter = smart_array_deleter<T> >
	class shared_array
	{
	protected:
		/// this_type
		/// This is an alias for shared_array<T>, this class.
		typedef shared_array<T> this_type;

		/// allocator_type
		typedef Allocator allocator_type;

		/// deleter_type
		typedef Deleter deleter_type;

		/// ref_count
		/// An internal reference count type. Must be convertable to int
		/// so that the public use_count function can work.
		typedef int ref_count;

		T*              mpArray;       /// The owned pointer. Points to an array of T.
		ref_count*      mpRefCount;    /// Reference count for owned pointer.
		allocator_type  mAllocator;    /// The allocator used to manage new/delete of mpRefCount.

	public:
		typedef T element_type;

		/// shared_array
		/// Takes ownership of the pointer and sets the reference count
		/// to the pointer to 1. It is OK if the input pointer is null.
		/// The shared reference count is allocated on the heap via operator new.
		/// If an exception occurs during the allocation of the shared 
		/// reference count, the owned pointer is deleted and the exception
		/// is rethrown. A null pointer is given a reference count of 1.
		explicit shared_array(T* pArray = NULL, const allocator_type& allocator = EASTL_SHARED_ARRAY_DEFAULT_ALLOCATOR)
			: mpArray(pArray),
			  mpRefCount(NULL),
			  mAllocator(allocator)
		{
			// Allocate memory for the reference count.
			void* const pMemory = EASTLAlloc(mAllocator, sizeof(ref_count));
			if(pMemory)
				mpRefCount = ::new(pMemory) ref_count(1);
		}


		/// shared_array
		/// Shares ownership of a pointer with another instance of shared_array.
		/// This function increments the shared reference count on the pointer.
		/// If we want a shared_array constructor that is templated on shared_array<U>,
		/// then we need to make it in addition to this function, as otherwise 
		/// the compiler will generate this function and things will go wrong.
		shared_array(const shared_array& sharedArray)
			: mpArray(sharedArray.mpArray),
			  mpRefCount(sharedArray.mpRefCount),
			  mAllocator(sharedArray.mAllocator)
		{
			++*mpRefCount;
		}


		/// ~shared_array
		/// Decrements the reference count for the owned pointer. If the 
		/// reference count goes to zero, the owned pointer is deleted and
		/// the shared reference count is deleted.
		~shared_array()
		{
			const ref_count newRefCount(--*mpRefCount); 
			// assert(newRefCount >= 0);
			if(newRefCount == 0)
			{
				EASTLFree(mAllocator, mpRefCount, sizeof(ref_count));
				Deleter del;
				del(mpArray);
			}
		}


		/// operator=
		/// Copies another shared_array to this object. Note that this object
		/// may already own a shared pointer with another different pointer
		/// (but still of the same type) before this call. In that case,
		/// this function releases the old pointer, decrementing its reference
		/// count and deleting it if zero, takes shared ownership of the new 
		/// pointer and increments its reference count.
		/// If we want a shared_array operator= that is templated on shared_array<U>,
		/// then we need to make it in addition to this function, as otherwise 
		/// the compiler will generate this function and things will go wrong.
		shared_array& operator=(const shared_array& sharedArray)
		{
			if(mpArray != sharedArray.mpArray)
			{
				// The easiest thing to do is to create a temporary and 
				// copy ourselves ourselves into it. This is a standard 
				// method for switching pointer ownership in systems like this.
				shared_array(sharedArray).swap(*this);
			}
			return *this;
		}


		/// operator=
		/// Assigns a new pointer, while decrementing the reference count on
		/// the current pointer. The new pointer can be NULL and the current
		/// pointer can NULL. If the new pointer is equivalent to the current
		/// pointer, then nothing is done.
		shared_array& operator=(T* pValue)
		{
			reset(pValue);
			return *this;
		}


		/// reset
		/// Releases the owned pointer and takes ownership of the 
		/// passed in pointer. If the passed in pointer is the same
		/// as the owned pointer, nothing is done. The passed in pointer
		/// can be null, in which case the use count is set to 1.
		void reset(T* pArray = NULL)
		{
			if(pArray != mpArray)
			{
				// The easiest thing to do is to create a temporary and 
				// copy ourselves ourselves into it. This is a standard 
				// method for switching pointer ownership in systems like this.
				shared_array(pArray, mAllocator).swap(*this);
			}
		}


		/// swap
		/// Exchanges the owned pointer beween two shared_array objects.
		void swap(this_type& sharedArray)
		{
			// We leave mAllocator as-is.

			// eastl::swap(mpArray, sharedArray.mpArray);
			T* const pArray     = sharedArray.mpArray;
			sharedArray.mpArray = mpArray;
			mpArray             = pArray;

			// eastl::swap(mpRefCount, sharedArray.mpRefCount);
			ref_count* const pRefCount = sharedArray.mpRefCount;
			sharedArray.mpRefCount     = mpRefCount;
			mpRefCount                 = pRefCount;
		}


		/// operator[]
		/// Returns a reference to the specified item in the owned pointer
		/// array. 
		/// Example usage:
		///    shared_array<int> ptr = new int[6];
		///    int x = ptr[2];
		T& operator[](ptrdiff_t i) const
		{
			// assert(mpArray && (i >= 0));
			return mpArray[i];
		}

		/// operator*
		/// Returns the owner pointer dereferenced.
		/// Example usage:
		///    shared_array<int> ptr = new int(3);
		///    int x = *ptr;
		T& operator*() const
		{
			// assert(mpArray);
			return *mpArray;
		}

		/// operator->
		/// Allows access to the owned pointer via operator->()
		/// Example usage:
		///    struct X{ void DoSomething(); }; 
		///    shared_array<int> ptr = new X;
		///    ptr->DoSomething();
		T* operator->() const EA_NOEXCEPT
		{
			// assert(mpArray);
			return mpArray;
		}

		/// get
		/// Returns the owned pointer. Note that this class does 
		/// not provide an operator T() function. This is because such
		/// a thing (automatic conversion) is deemed unsafe.
		/// Example usage:
		///    struct X{ void DoSomething(); }; 
		///    shared_array<int> ptr = new X;
		///    X* pX = ptr.get();
		///    pX->DoSomething();
		T* get() const EA_NOEXCEPT
		{
			return mpArray;
		}

		/// use_count
		/// Returns the reference count on the owned pointer.
		/// The return value is one if the owned pointer is null.
		int use_count() const
		{
			// assert(mpRefCount);
			return (int)*mpRefCount;
		}

		/// unique
		/// Returns true if the reference count on the owned pointer is one.
		/// The return value is true if the owned pointer is null.
		bool unique() const
		{
			// assert(mpRefCount);
			return (*mpRefCount == 1);
		}

		/// Implicit operator bool
		/// Allows for using a scoped_ptr as a boolean. 
		/// Example usage:
		///    shared_array<int> ptr = new int(3);
		///    if(ptr)
		///        ++*ptr;
		///     
		/// Note that below we do not use operator bool(). The reason for this
		/// is that booleans automatically convert up to short, int, float, etc.
		/// The result is that this: if(sharedArray == 1) would yield true (bad).
		typedef T* (this_type::*bool_)() const;
		operator bool_() const EA_NOEXCEPT
		{
			if(mpArray)
				return &this_type::get;
			return NULL;
		}

		/// operator!
		/// This returns the opposite of operator bool; it returns true if 
		/// the owned pointer is null. Some compilers require this and some don't.
		///    shared_array<int> ptr = new int(3);
		///    if(!ptr)
		///        assert(false);
		bool operator!() const EA_NOEXCEPT
		{
			return (mpArray == NULL);
		}

		/// get_allocator
		/// Returns the memory allocator associated with this class.
		const allocator_type& get_allocator() const EA_NOEXCEPT
		{
			return mAllocator;
		}
		allocator_type& get_allocator() EA_NOEXCEPT
		{
			return mAllocator;
		}

		/// set_allocator
		/// Sets the memory allocator associated with this class.
		void set_allocator(const allocator_type& allocator)
		{
			mAllocator = allocator;
		}

	}; // class shared_array



	/// get_pointer
	/// returns shared_array::get() via the input shared_array. 
	template <typename T, typename A, typename D>
	inline T* get_pointer(const shared_array<T, A, D>& sharedArray)
	{
		return sharedArray.get();
	}

	/// swap
	/// Exchanges the owned pointer beween two shared_array objects.
	/// This non-member version is useful for compatibility of shared_array
	/// objects with the C++ Standard Library and other libraries.
	template <typename T, typename A, typename D>
	inline void swap(shared_array<T, A, D>& sharedArray1, shared_array<T, A, D>& sharedArray2)
	{
		sharedArray1.swap(sharedArray2);
	}


	/// operator!=
	/// Compares two shared_array objects for equality. Equality is defined as 
	/// being true when the pointer shared between two shared_array objects is equal.
	/// It is debatable what the appropriate definition of equality is between two
	/// shared_array objects, but we follow the current 2nd generation C++ standard proposal.
	template <typename T, typename TA, typename TD, typename U, typename UA, typename UD>
	inline bool operator==(const shared_array<T, TA, TD>& sharedArray1, const shared_array<U, UA, UD>& sharedArray2)
	{
		// assert((sharedArray1.get() != sharedArray2.get()) || (sharedArray1.use_count() == sharedArray2.use_count()));
		return (sharedArray1.get() == sharedArray2.get());
	}


	/// operator!=
	/// Compares two shared_array objects for inequality. Equality is defined as 
	/// being true when the pointer shared between two shared_array objects is equal.
	/// It is debatable what the appropriate definition of equality is between two
	/// shared_array objects, but we follow the current 2nd generation C++ standard proposal.
	template <typename T, typename TA, typename TD, typename U, typename UA, typename UD>
	inline bool operator!=(const shared_array<T, TA, TD>& sharedArray1, const shared_array<U, UA, UD>& sharedArray2)
	{
		// assert((sharedArray1.get() != sharedArray2.get()) || (sharedArray1.use_count() == sharedArray2.use_count()));
		return (sharedArray1.get() != sharedArray2.get());
	}


	/// operator<
	/// Returns which shared_array is 'less' than the other. Useful when storing
	/// sorted containers of scoped_ptr objects.
	template <typename T, typename TA, typename TD, typename U, typename UA, typename UD>
	inline bool operator<(const shared_array<T, TA, TD>& sharedArray1, const shared_array<U, UA, UD>& sharedArray2)
	{
		return (sharedArray1.get() < sharedArray2.get()); // Alternatively use: std::less<T*>(a.get(), b.get());
	}


} // namespace eastl


#endif // Header include guard











