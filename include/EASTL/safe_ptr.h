///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_SAFEPTR_H
#define EASTL_SAFEPTR_H


#include <EASTL/internal/config.h>
#include <stddef.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{
	class safe_ptr_base;


	/// safe_object
	///
	/// In order for a class to be the template argument for safe_ptr, 
	/// it must derive from safe_object.
	///
	/// Example usage:
	///     class RandomLifetimeObject : public safe_object
	///     {
	///     public:
	///         RandomLifetimeObject();
	///         Method();
	///         ...
	///     };
	///
	class safe_object
	{
	public:
		bool is_unreferenced() const;                 /// Returns true if there are zero references (by a smart_ptr) to this object (mpSafePtrList is NULL).
		bool has_unique_reference() const;            /// Returns true if there is at most one reference (by a smart_ptr) to us.

	protected:
		safe_object();
	   ~safe_object();

		void clear_references();                      /// Forcibly removes any references (by smart_ptrs) to this object. All safe_ptr mpObject values are set to NULL. 

	private:
		friend class safe_ptr_base;

		void add(safe_ptr_base* pBase) const;         /// Link pBase into my list of safe pointers.
		void remove(safe_ptr_base* pBase) const;      /// Unlink pBase from my list of safe pointers.

		mutable safe_ptr_base* mpSafePtrList;

	public:
		// Deprecated, as its name is misleading:
		EASTL_REMOVE_AT_2024_APRIL bool has_references() const;	/// Returns true if there is at most one reference (by a smart_ptr) to us.
	};



	/// safe_ptr_base
	///
	/// This is a non-templated base class for safe_ptr<T>, not for direct use by the user.
	///
	class safe_ptr_base
	{
	public:
		bool unique() const;                    /// Returns true if there are no other smart pointers pointing to our object except us. True if mpObject is NUll
		bool empty() const;                     /// Returns true if mpObject is NULL.
		void reset(const safe_object* pObject); /// Make this point to pObject and enlist.
		void reset();                           /// Make this point to NULL and delist.

	protected:
		// The following are protected and must be overridden in the safe_ptr<T> subclass.
		safe_ptr_base();
		safe_ptr_base(const safe_object* pObject);
		safe_ptr_base(const safe_ptr_base& safePtrBase);
	   ~safe_ptr_base();

	protected:
		const safe_object* mpObject;

	private:
		friend class safe_object;

		safe_ptr_base& operator=(const safe_ptr_base& safePtrBase);

		safe_ptr_base* mpNext;
	};


	/// safe_ptr
	///
	/// safe_ptr is an automatic, lightweight solution to the dangling pointer problem.
	/// This class is an alternative to weak_ptr which has the primary benefit of not 
	/// allocating memory at the primary cost of being a tad slower and thread-unsafe.
	///
	/// During normal usage, safe_ptr<T> behaves exactly as a T*. When the
	/// raw pointer referenced by the safe_ptr is deleted, all of the SafePtrs
	/// for the raw pointer are set to NULL.
	///
	/// This works by making the raw objects derive from the class safe_object,
	/// which maintains a linked-list of the Safe pointers that reference it.
	/// When a safe_object is destroyed, it walks its linked list, setting the
	/// object reference for each of its SafePtrs to NULL.
	///
	/// The overhead for this is light - a single pointer is added to the
	/// size of the pointed to object, and a safePtr is the size of a raw 
	/// pointer plus one list pointer.
	///
	/// This class is not thread-safe. In particular, manipulation of safe_ptr
	/// objects that refer to the same underlying object cannot be done safely
	/// from multiple threads. safe_ptr objects that are unrelated can be used
	/// safely from multiple threads.
	///
	/// Example usage:
	///     class RandomLifetimeObject : public safe_object
	///     {
	///     public:
	///         RandomLifetimeObject();
	///         Method();
	///         ...
	///     };
	///     
	///     safe_ptr<RandomLifetimeObject> pSafePtr(new RandomLifetimeObject);
	///     safe_ptr<RandomLifetimeObject> pSafePtrCopy = pSafePtr;
	///
	///     pSafePtr->Method();
	///     delete pSafePtr;
	///     At this point, pSafePtrCopy evaluates to NULL.
	///
	template<class T>
	class safe_ptr : public safe_ptr_base
	{
	public:
		typedef T           value_type;
		typedef safe_ptr<T> this_type;

	public:
		safe_ptr();                                         /// Default constructor.
		explicit safe_ptr(T* pObject);                      /// Construct a safeptr from a naked pointer.
		safe_ptr(const this_type& safePtr);                 /// Copy constructor.
		//~safe_ptr() {}                                    /// No need to implement this; the compiler-generated destructor is OK.

		this_type& operator=(const this_type& safePtr);     /// Assignment operator.
		this_type& operator=(T* const pObject);             /// Assign this to a naked pointer.

		bool operator==(const this_type& safePtr) const;    /// Returns true if safePtr points to the same object as this.

	public:
		T* get() const;                                     /// Get the naked pointer from this safe ptr.
		operator T*() const;                                /// Implicit safe_ptr<T> -> T* conversion operator.
		T* operator->() const;                              /// Member operator.
		T& operator*() const;                               /// Dereference operator.
		bool operator!() const;                             /// Boolean negation operator.

		typedef T* (this_type::*bool_)() const;             /// Allows for a more portable version of testing an instance of this class as a bool.
		operator bool_() const                              // A bug in the CodeWarrior compiler forces us to implement this inline instead of below.
		{
			if(mpObject)
				return &this_type::get;
			return NULL;
		}
	};

} // namespace eastl






/////////////////////////////////////////////////////////////////////////
// Inlines
/////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// safe_object
///////////////////////////////////////////////////////////////////////////////

inline eastl::safe_object::safe_object()
  :  mpSafePtrList(0)
{
}


inline bool eastl::safe_object::is_unreferenced() const
{
	return (mpSafePtrList == NULL);
}


inline void eastl::safe_object::clear_references()
{
	while(mpSafePtrList != NULL)
	{
		safe_ptr_base* const pNext = mpSafePtrList->mpNext;
		mpSafePtrList->mpNext = NULL;
		mpSafePtrList->mpObject = NULL;
		mpSafePtrList = pNext;
	}
}


inline eastl::safe_object::~safe_object()
{
	safe_ptr_base* pIter = mpSafePtrList;

	while(pIter)
	{
		safe_ptr_base* const pNext = pIter->mpNext;
		pIter->mpNext = NULL;
		pIter->mpObject = NULL;
		pIter = pNext;
	}
}


inline void eastl::safe_object::add(safe_ptr_base* pBase) const
{
	pBase->mpNext = mpSafePtrList;
	mpSafePtrList = pBase;
}


inline void eastl::safe_object::remove(safe_ptr_base* pBase) const
{
	// We have a singly-linked list (starting with mpSafePtrList) and need to 
	// remove an element from within it.
	if(pBase == mpSafePtrList)
		mpSafePtrList = mpSafePtrList->mpNext;
	else
	{
		for(safe_ptr_base *pPrev = mpSafePtrList, *pCurrent = mpSafePtrList->mpNext; 
			pCurrent;
			pPrev = pCurrent, pCurrent = pCurrent->mpNext)
		{
			if(pCurrent == pBase)
			{
				pPrev->mpNext = pCurrent->mpNext;
				break;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// safe_ptr_base
///////////////////////////////////////////////////////////////////////////////

inline eastl::safe_ptr_base::safe_ptr_base(const safe_ptr_base& safePtrBase)
  : mpObject(safePtrBase.mpObject),
	mpNext(NULL)    
{
	EASTL_ASSERT(this != &safePtrBase);

	if(mpObject)
		mpObject->add(this);
}


inline eastl::safe_ptr_base::safe_ptr_base()
  : mpObject(NULL),
	mpNext(NULL)
{
}


inline eastl::safe_ptr_base::safe_ptr_base(const safe_object* pObject)
  : mpObject(pObject),
	mpNext(NULL)
{
	if(mpObject)
		mpObject->add(this);
}


inline eastl::safe_ptr_base::~safe_ptr_base()
{
	if(mpObject)
		mpObject->remove(this);
}


inline void eastl::safe_ptr_base::reset()
{
	if(mpObject)
	{
		mpObject->remove(this);
		mpObject = NULL;
	}
}


inline bool eastl::safe_ptr_base::empty() const
{
	return (mpObject == NULL);
}


inline void eastl::safe_ptr_base::reset(const safe_object* pNewObject)
{
	if(mpObject != pNewObject)
	{
		if(mpObject)
			mpObject->remove(this);

		mpObject = pNewObject;

		if(mpObject)
			mpObject->add(this);
	}
}


inline bool eastl::safe_ptr_base::unique() const
{
	return (mpNext == NULL) && ((mpObject == NULL) || (mpObject->mpSafePtrList == this));
}



///////////////////////////////////////////////////////////////////////////////
// safe_object
///////////////////////////////////////////////////////////////////////////////


// This function is defined here below safe_ptr_base because some compilers 
// (GCC in particular) generate warnings about inline functions (e.g. unique below) 
// being used before their inline implementations. 
inline bool eastl::safe_object::has_unique_reference() const
{
	return mpSafePtrList ? mpSafePtrList->unique() : false;
}

// Deprecated:
inline bool eastl::safe_object::has_references() const
{
	return mpSafePtrList ? mpSafePtrList->unique() : false;
}


///////////////////////////////////////////////////////////////////////////////
// safe_ptr<T>
///////////////////////////////////////////////////////////////////////////////

template<class T>
inline eastl::safe_ptr<T>::safe_ptr() 
  : safe_ptr_base()
{
}


template<class T>
inline eastl::safe_ptr<T>::safe_ptr(T* pObject)
  : safe_ptr_base(pObject)
{
}


template<class T>
inline eastl::safe_ptr<T>::safe_ptr(const this_type& safePtr)
  : safe_ptr_base(safePtr)
{
}


template<class T>
inline typename eastl::safe_ptr<T>::this_type& eastl::safe_ptr<T>::operator=(const this_type& safePtr)
{
	if(this != &safePtr)
		reset(safePtr.mpObject);
	return *this;
}


template<class T>
inline typename eastl::safe_ptr<T>::this_type& eastl::safe_ptr<T>::operator=(T* const pObject)
{
	reset(pObject);
	return *this;
}


template<class T>
inline bool eastl::safe_ptr<T>::operator==(const this_type& rhs) const
{
	return (mpObject == rhs.mpObject);
}


template<class T>
inline T* eastl::safe_ptr<T>::get() const
{
	return static_cast<T*>(const_cast<safe_object*>(mpObject));
}


template<class T>
inline eastl::safe_ptr<T>::operator T*() const
{
	return static_cast<T*>(const_cast<safe_object*>(mpObject));
}


template<class T>
inline T* eastl::safe_ptr<T>::operator->() const
{
	return static_cast<T*>(const_cast<safe_object*>(mpObject));
}


template<class T>
inline T& eastl::safe_ptr<T>::operator*() const
{
	return *static_cast<T*>(const_cast<safe_object*>(mpObject));
}


template<class T>
inline bool eastl::safe_ptr<T>::operator!() const
{
	return (mpObject == NULL);
}

// A bug in the CodeWarrior compiler forces us to implement this inline in the class instead of here.
// template<class T>
// inline eastl::safe_ptr<T>::operator bool_() const
// {
//     if(mpObject)
//         return &this_type::get;
//     return NULL;
// }



///////////////////////////////////////////////////////////////////////////////
// global operators
///////////////////////////////////////////////////////////////////////////////

template<class T>
inline bool operator==(const eastl::safe_ptr<T>& safePtr, const T* pObject)
{
	return (safePtr.get() == pObject);
}


template<class T>
inline bool operator!=(const eastl::safe_ptr<T>& safePtr, const T* pObject)
{
	return (safePtr.get() != pObject);
}


template<class T>
inline bool operator<(const eastl::safe_ptr<T>& safePtrA, const eastl::safe_ptr<T>& safePtrB)
{
	return (safePtrA.get() < safePtrB.get());
}




#endif // Header include guard















