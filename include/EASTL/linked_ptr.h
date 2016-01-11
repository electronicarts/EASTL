///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_LINKED_PTR_H
#define EASTL_LINKED_PTR_H



#include <EASTL/internal/config.h>
#include <EASTL/internal/smart_ptr.h>   // Defines smart_ptr_deleter
#include <EASTL/allocator.h>
#include <stddef.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// linked_ptr_base
	///
	/// This class allows linked_ptr<T> and linked_ptr<U> to share the same 
	/// base nodes and thus be in the same linked list.
	///
	struct linked_ptr_base
	{
		mutable linked_ptr_base* mpPrev;
		mutable linked_ptr_base* mpNext;
	};


	/// linked_ptr
	///
	/// This class implements a linked_ptr template. A linked_ptr is like the C++
	/// Standard Library auto_ptr except that it allows sharing of pointers between
	/// instances of auto_ptr via reference counting. linked_ptr objects can safely
	/// be copied and can safely be used in C++ Standard Library containers such
	/// as std::vector or std::list. This implementation, however, is not thread-safe.
	/// you would need to use a separate linked_ptr_mt (multi-threaded) to get 
	/// thread safety.
	///
	/// linked_ptr is a variation of shared_ptr (a.k.a. counted_ptr) which differs 
	/// in that instead of being implemented by a shared integer stored on the heap, 
	/// it is implemented by linked list stored within the linked_ptr object itself.
	/// The result is that no memory is explicitly allocated from the heap, though
	/// the cost of each linked_ptr object is 12 bytes of memory (32 bit machine)
	/// instead of 4 bytes for the case of shared_ptr (depending on the heap).
	///
	template <typename T, typename Deleter = smart_ptr_deleter<T> >
	class linked_ptr : public linked_ptr_base
	{
	protected:
		template <typename U, typename D> friend class linked_ptr;

		/// this_type
		/// This is an alias for linked_ptr<T>, this class.
		typedef linked_ptr<T> this_type;

		/// deleter_type
		typedef Deleter deleter_type;

		T* mpValue; /// The owned pointer.

		template <typename U, typename D>
		void link(const linked_ptr<U, D>& linkedPtr)
		{   // This code can only be called when we are in a reset state.
			// assert(!mpValue && (mpNext == mpPrev));
			mpNext           = linkedPtr.mpNext;
			mpNext->mpPrev   = this;
			mpPrev           = const_cast<linked_ptr<U, D>*>(&linkedPtr);
			linkedPtr.mpNext = this;
		}

	public:
		/// element_type
		/// Synonym for type T, useful for external code to reference the 
		/// type in a generic way.
		typedef T element_type;


		/// linked_ptr
		/// Default constructor.
		linked_ptr() 
			: mpValue(NULL)
		{
			mpPrev = mpNext = this;
		}


		/// linked_ptr
		/// Takes ownership of the pointer. It is OK if the input pointer is null.
		template <typename U>
		explicit linked_ptr(U* pValue) 
			: mpValue(pValue)
		{
			mpPrev = mpNext = this;
		}


		/// linked_ptr
		/// Construction with self type.
		/// If we want a shared_ptr constructor that is templated on linked_ptr<U>,
		/// then we need to make it in addition to this function, as otherwise 
		/// the compiler will generate this function and things will go wrong.
		linked_ptr(const linked_ptr& linkedPtr)
			: mpValue(linkedPtr.mpValue)
		{
			if(mpValue)
				link(linkedPtr);
			else
				mpPrev = mpNext = this;
		}


		/// linked_ptr
		/// Shares ownership of a pointer with another instance of linked_ptr.
		template <typename U, typename D>
		linked_ptr(const linked_ptr<U, D>& linkedPtr)
			: mpValue(linkedPtr.mpValue)
		{
			if(mpValue)
				link(linkedPtr);
			else
				mpPrev = mpNext = this;
		}


		/// ~linked_ptr
		/// Removes this object from the of objects using the shared pointer.
		/// If this object is the last owner of the shared pointer, the shared 
		/// pointer is deleted.
		~linked_ptr() 
		{
			reset();
		}


		/// operator=
		/// If we want a shared_ptr operator= that is templated on linked_ptr<U>,
		/// then we need to make it in addition to this function, as otherwise 
		/// the compiler will generate this function and things will go wrong.
		linked_ptr& operator=(const linked_ptr& linkedPtr)
		{
			if(linkedPtr.mpValue != mpValue)
			{
				reset(linkedPtr.mpValue);
				if(linkedPtr.mpValue)
					link(linkedPtr);
			}
			return *this;
		}


		/// operator=
		/// Copies another linked_ptr to this object. Note that this object
		/// may already own a shared pointer with another different pointer
		/// (but still of the same type) before this call. In that case,
		/// this function removes ownership of the old pointer and takes shared 
		/// ownership of the new pointer and increments its reference count.
		template <typename U, typename D>
		linked_ptr& operator=(const linked_ptr<U, D>& linkedPtr)
		{
			if(linkedPtr.mpValue != mpValue)
			{
				reset(linkedPtr.mpValue);
				if(linkedPtr.mpValue)
					link(linkedPtr);
			}
			return *this;
		}


		/// operator=
		/// Assigns a new pointer. If the new pointer is equivalent
		/// to the current pointer, nothing is done. Otherwise the 
		/// current pointer is unlinked and possibly destroyed.
		/// The new pointer can be NULL.
		template <typename U>
		linked_ptr& operator=(U* pValue)
		{
			reset(pValue);
			return *this;
		}


		/// reset
		/// Releases the owned pointer and takes ownership of the 
		/// passed in pointer. If the passed in pointer is the same
		/// as the owned pointer, nothing is done. The passed in pointer
		/// can be NULL, in which case the use count is set to 1.
		template <typename U>
		void reset(U* pValue)
		{
			if(pValue != mpValue)
			{
				if(unique())
				{
					deleter_type del;
					del(mpValue);
				}
				else
				{
					mpPrev->mpNext  = mpNext;
					mpNext->mpPrev  = mpPrev;
					mpPrev = mpNext = this;
				}
				mpValue = pValue;
			}
		}


		/// reset
		/// Resets the container with NULL. If the current pointer
		/// is non-NULL, it is unlinked and possibly destroyed.
		void reset()
		{
			reset((T*)NULL);
		}


		/// swap
		/// Exchanges the owned pointer beween two linkedPtr objects.
		///
		/// This function is disabled as it is currently deemed unsafe.
		/// The problem is that the only way to implement this function
		/// is to transfer pointers between the objects; you cannot 
		/// transfer the linked list membership between the objects. 
		/// Thus unless both linked_ptr objects were 'unique()', the 
		/// shared pointers would be duplicated amongst containers, 
		/// resulting in a crash.
		//template <typename U, typename D>
		//void swap(linked_ptr<U, D>& linkedPtr)
		//{
		//    if(linkedPtr.mpValue != mpValue)
		//    {   // This is only safe if both linked_ptrs are unique().
		//        linkedPtr::element_type* const pValueTemp = linkedPtr.mpValue;
		//        linkedPtr.reset(mpValue);
		//        reset(pValueTemp);
		//    }
		//}


		/// operator*
		/// Returns the owner pointer dereferenced.
		T& operator*() const
		{
			return *mpValue;
		}


		/// operator->
		/// Allows access to the owned pointer via operator->()
		T* operator->() const
		{
			return mpValue;
		}


		/// get
		/// Returns the owned pointer. Note that this class does 
		/// not provide an operator T() function. This is because such
		/// a thing (automatic conversion) is deemed unsafe.
		T* get() const
		{
			return mpValue;
		}


		/// use_count
		/// Returns the use count of the shared pointer.
		/// The return value is one if the owned pointer is null.
		/// This function is provided for compatibility with the 
		/// proposed C++ standard and for debugging purposes. It is not
		/// intended for runtime use given that its execution time is
		/// not constant.
		int use_count() const
		{
			int useCount(1);
			
			for(const linked_ptr_base* pCurrent = static_cast<const linked_ptr_base*>(this); 
					pCurrent->mpNext != static_cast<const linked_ptr_base*>(this); pCurrent = pCurrent->mpNext)
				++useCount;

			return useCount;
		}


		/// unique
		/// Returns true if the use count of the owned pointer is one.
		/// The return value is true if the owned pointer is null.
		bool unique() const
		{
			return (mpNext == static_cast<const linked_ptr_base*>(this));
		}


		/// Implicit operator bool
		/// Allows for using a linked_ptr as a boolean. 
		/// Note that below we do not use operator bool(). The reason for this
		/// is that booleans automatically convert up to short, int, float, etc.
		/// The result is that this: if(linkedPtr == 1) would yield true (bad).
		typedef T* (this_type::*bool_)() const;
		operator bool_() const
		{
			if(mpValue)
				return &this_type::get;
			return NULL;
		}


		/// operator!
		/// This returns the opposite of operator bool; it returns true if 
		/// the owned pointer is null. Some compilers require this and some don't.
		bool operator!()
		{
			return (mpValue == NULL);
		}


		/// detach
		/// Returns ownership of the pointer to the caller.  Fixes all
		/// references to the pointer by any other owners to be NULL.
		/// This function can work properly only if all entries in the list 
		/// refer to type T and none refer to any other type (e.g. U).
		T* detach()
		{
			T* const pValue = mpValue;

			linked_ptr_base* p = this;
			do
			{
				linked_ptr_base* const pNext = p->mpNext;
				static_cast<this_type*>(p)->mpValue = NULL;
				p->mpNext = p->mpPrev = p;
				p = pNext;
			}
			while(p != this);

			return pValue;
		}

		/// force_delete
		/// Forces deletion of the shared pointer. Fixes all references to the 
		/// pointer by any other owners to be NULL.
		/// This function can work properly only if all entries in the list 
		/// refer to type T and none refer to any other type (e.g. U).
		void force_delete()
		{
			T* const pValue = detach();
			Deleter del;
			del(pValue);
		}

	}; // class linked_ptr



	/// get_pointer
	/// Returns linked_ptr::get() via the input linked_ptr. Provided for compatibility
	/// with certain well-known libraries that use this functionality.
	template <typename T, typename D>
	inline T* get_pointer(const linked_ptr<T, D>& linkedPtr)
	{
		return linkedPtr.get();
	}


	/// operator==
	/// Compares two linked_ptr objects for equality. Equality is defined as 
	/// being true when the pointer shared between two linked_ptr objects is equal.
	template <typename T, typename TD, typename U, typename UD>
	inline bool operator==(const linked_ptr<T, TD>& linkedPtr1, const linked_ptr<U, UD>& linkedPtr2)
	{
		return (linkedPtr1.get() == linkedPtr2.get());
	}


	/// operator!=
	/// Compares two linked_ptr objects for inequality. Equality is defined as 
	/// being true when the pointer shared between two linked_ptr objects is equal.
	template <typename T, typename TD, typename U, typename UD>
	inline bool operator!=(const linked_ptr<T, TD>& linkedPtr1, const linked_ptr<U, UD>& linkedPtr2)
	{
		return (linkedPtr1.get() != linkedPtr2.get());
	}


	/// operator<
	/// Returns which linked_ptr is 'less' than the other. Useful when storing
	/// sorted containers of linked_ptr objects.
	template <typename T, typename TD, typename U, typename UD>
	inline bool operator<(const linked_ptr<T, TD>& linkedPtr1, const linked_ptr<U, UD>& linkedPtr2)
	{
		return (linkedPtr1.get() < linkedPtr2.get());
	}


} // namespace eastl


#endif // Header include guard




















