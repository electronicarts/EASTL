///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTRUSIVE_PTR_H
#define EASTL_INTRUSIVE_PTR_H


#include <EASTL/internal/config.h>
#include <stddef.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{
	// We provide default implementations of AddRef and Release in the eastl namespace.
	// The user can override these on a per-class basis by defining their own specialized
	// intrusive_ptr_add_ref and intrusive_ptr_release functions. User-defined specializations
	// do not need to exist in the eastl namespace, but should preferably be in the namespace 
	// of the templated class T.
	template <typename T>
	void intrusive_ptr_add_ref(T* p)
	{
		p->AddRef();
	}
	 
	template <typename T>
	void intrusive_ptr_release(T* p)
	{
		p->Release();
	}


	//////////////////////////////////////////////////////////////////////////////
	/// intrusive_ptr
	/// 
	/// This is a class that acts like the C++ auto_ptr class except that instead
	/// of deleting its member data when it goes out of scope, it releases its
	/// member data when it goes out of scope. This class thus requires that the 
	/// templated data type have an AddRef and Release function (or whatever is
	/// configured to be the two refcount functions).
	/// 
	/// This class is useful for automatically releasing an object when this 
	/// class goes out of scope. See below for some usage.
	/// You should be careful about putting instances of this class as members of 
	/// another class. If you do so, then the intrusive_ptr destructor will only
	/// be called if the object that owns it is destructed. This creates a potential
	/// chicken-and-egg situation. What if the intrusive_ptr member contains a 
	/// pointer to an object that has a reference on the object that owns the 
	/// intrusive_ptr member? The answer is that the neither object can ever be
	/// destructed. The solution is to:
	///    1) Be very careful about what objects you put into member intrusive_ptr objects.
	///    2) Clear out your intrusive_ptr members in your shutdown function.
	///    3) Simply don't use intrusive_ptr objects as class members.
	///
	/// Example usage:
	///    intrusive_ptr<IWidget> pWidget = new Widget;
	///    pWidget = new Widget;
	///    pWidget->Reset();
	///
	template <typename T>
	class intrusive_ptr
	{
	protected:
		// Friend declarations.
		template <typename U> friend class intrusive_ptr;
		typedef intrusive_ptr<T> this_type;

		T* mpObject;

	public:
		/// element_type
		/// This typedef is present for consistency with the C++ standard library 
		/// auto_ptr template. It allows users to refer to the templated type via
		/// a typedef. This is sometimes useful to be able to do.
		/// 
		/// Example usage:
		///    intrusive_ptr<IWidget> ip;
		///    void DoSomething(intrusive_ptr<IWidget>::element_type someType);
		///
		typedef T element_type;

		/// intrusive_ptr
		/// Default constructor. The member object is set to NULL.
		intrusive_ptr()
			: mpObject(NULL)
		{
			// Empty
		}

		/// intrusive_ptr
		/// Provides a constructor which takes ownership of a pointer.
		/// The incoming pointer is AddRefd.
		///
		/// Example usage:
		///    intrusive_ptr<Widget> pWidget(new Widget);
		intrusive_ptr(T* p, bool bAddRef = true)
			: mpObject(p) 
		{
			if(mpObject && bAddRef)
				intrusive_ptr_add_ref(mpObject);  // Intentionally do not prefix the call with eastl:: but instead allow namespace lookup to resolve the namespace.
		} 

		/// intrusive_ptr
		/// Construction from self type.
		intrusive_ptr(const intrusive_ptr& ip) 
			: mpObject(ip.mpObject) 
		{
			if(mpObject)
				intrusive_ptr_add_ref(mpObject);
		}


		/// intrusive_ptr
		/// move constructor
		intrusive_ptr(intrusive_ptr&& ip) 
			: mpObject(nullptr)
		{
			swap(ip);
		}

		/// intrusive_ptr
		/// Provides a constructor which copies a pointer from another intrusive_ptr.
		/// The incoming pointer is AddRefd. The source intrusive_ptr object maintains
		/// its AddRef on the pointer.
		///
		/// Example usage:
		///    intrusive_ptr<Widget> pWidget1;
		///    intrusive_ptr<Widget> pWidget2(pWidget1);
		template <typename U>
		intrusive_ptr(const intrusive_ptr<U>& ip) 
			: mpObject(ip.mpObject) 
		{
			if(mpObject)
				intrusive_ptr_add_ref(mpObject);
		}

		/// intrusive_ptr
		/// Releases the owned pointer.
		~intrusive_ptr() 
		{
			if(mpObject)
				intrusive_ptr_release(mpObject);
		}


		/// operator=
		/// Assignment to self type.
		intrusive_ptr& operator=(const intrusive_ptr& ip)
		{
			return operator=(ip.mpObject);
		}


		/// operator=
		/// Move assignment operator 
		intrusive_ptr& operator=(intrusive_ptr&& ip)
		{
			swap(ip);
			return *this;
		}


		/// operator =
		/// Assigns an intrusive_ptr object to this intrusive_ptr object.
		/// The incoming pointer is AddRefd. The source intrusive_ptr object 
		/// maintains its AddRef on the pointer. If there is an existing member
		/// pointer, it is Released before the incoming pointer is assigned.
		/// If the incoming pointer is equal to the existing pointer, no    
		/// action is taken. The incoming pointer is AddRefd before any 
		/// member pointer is Released.
		template <typename U>
		intrusive_ptr& operator=(const intrusive_ptr<U>& ip)       
		{
			return operator=(ip.mpObject);
		}

		/// operator=
		/// Assigns an intrusive_ptr object to this intrusive_ptr object.
		/// The incoming pointer is AddRefd. If there is an existing member
		/// pointer, it is Released before the incoming pointer is assigned.
		/// If the incoming pointer is equal to the existing pointer, no    
		/// action is taken. The incoming pointer is AddRefd before any 
		/// member pointer is Released.
		intrusive_ptr& operator=(T* pObject)
		{
			if(pObject != mpObject)
			{
				T* const pTemp = mpObject; // Create temporary to prevent possible problems with re-entrancy.
				if(pObject)
					intrusive_ptr_add_ref(pObject);
				mpObject = pObject;
				if(pTemp)
					intrusive_ptr_release(pTemp);
			}
			return *this;
		}

		/// operator *
		/// Returns a reference to the contained object.
		T& operator *() const 
		{
			return *mpObject;
		}

		/// operator *
		/// Returns a pointer to the contained object, allowing the 
		/// user to use this container as if it were contained pointer itself. 
		T* operator ->() const
		{
			return mpObject;
		}

		/// get()
		/// Returns a pointer to the contained object. 
		T* get() const
		{
			return mpObject;
		}

		/// reset
		/// Releases the owned object and clears our reference to it.
		void reset() 
		{
			T* const pTemp = mpObject;
			mpObject = NULL;
			if(pTemp)
				intrusive_ptr_release(pTemp);
		}

		/// swap
		/// Exchanges the owned pointer beween two intrusive_ptr objects.
		void swap(this_type& ip)
		{
			T* const pTemp = mpObject;
			mpObject = ip.mpObject;
			ip.mpObject = pTemp;
		}

		/// attach
		/// Sets an intrusive_ptr pointer without calling AddRef() on
		/// the pointed object. The intrusive_ptr thus eventually only does a
		/// Release() on the object. This is useful for assuming a reference
		/// that someone else has handed you and making sure it is always
		/// released, even if you return in the middle of a function or an
		/// exception is thrown.
		///
		void attach(T* pObject)
		{
			T* const pTemp = mpObject;
			mpObject = pObject;
			if(pTemp)
				intrusive_ptr_release(pTemp);
		}

		/// detach
		/// Surrenders the reference held by an intrusive_ptr pointer -- 
		/// it returns the current reference and nulls the pointer. If the returned
		/// pointer is non-null it must be released. This is useful in functions
		/// that must return a reference while possibly being aborted by a return
		/// or thrown exception:
		///
		/// bool GetFoo(T** pp){
		///    intrusive_ptr<T> p(PrivateGetFoo());
		///    if(p->Method())
		///        return false;
		///    *pp = p.detach();
		///    return true;
		/// }
		T* detach()
		{
			T* const pTemp = mpObject;
			mpObject = NULL;
			return pTemp;
		}

		/// Implicit operator bool
		/// Allows for using a intrusive_ptr as a boolean. 
		/// Example usage:
		///    intrusive_ptr<Widget> ptr = new Widget;
		///    if(ptr)
		///        ++*ptr;
		///     
		/// Note that below we do not use operator bool(). The reason for this
		/// is that booleans automatically convert up to short, int, float, etc.
		/// The result is that this: if(intrusivePtr == 1) would yield true (bad).
		typedef T* (this_type::*bool_)() const;
		operator bool_() const
		{
			if(mpObject)
				return &this_type::get;
			return NULL;
		}

		/// operator!
		/// This returns the opposite of operator bool; it returns true if 
		/// the owned pointer is null. Some compilers require this and some don't.
		///    intrusive_ptr<Widget> ptr = new Widget;
		///    if(!ptr)
		///        assert(false);
		bool operator!() const
		{
			return (mpObject == NULL);
		}

	}; // class intrusive_ptr


	/// get_pointer
	/// returns intrusive_ptr::get() via the input intrusive_ptr. 
	template <typename T>
	inline T* get_pointer(const intrusive_ptr<T>& intrusivePtr)
	{
		return intrusivePtr.get();
	}

	/// swap
	/// Exchanges the owned pointer beween two intrusive_ptr objects.
	/// This non-member version is useful for compatibility of intrusive_ptr
	/// objects with the C++ Standard Library and other libraries.
	template <typename T>
	inline void swap(intrusive_ptr<T>& intrusivePtr1, intrusive_ptr<T>& intrusivePtr2)
	{
		intrusivePtr1.swap(intrusivePtr2);
	}


	template <typename T, typename U>
	bool operator==(intrusive_ptr<T> const& iPtr1, intrusive_ptr<U> const& iPtr2)
	{
		return (iPtr1.get() == iPtr2.get());
	}

	template <typename T, typename U>
	bool operator!=(intrusive_ptr<T> const& iPtr1, intrusive_ptr<U> const& iPtr2)
	{
		return (iPtr1.get() != iPtr2.get());
	}

	template <typename T>
	bool operator==(intrusive_ptr<T> const& iPtr1, T* p)
	{
		return (iPtr1.get() == p);
	}

	template <typename T>
	bool operator!=(intrusive_ptr<T> const& iPtr1, T* p)
	{
		return (iPtr1.get() != p);
	}

	template <typename T>
	bool operator==(T* p, intrusive_ptr<T> const& iPtr2)
	{
		return (p == iPtr2.get());
	}

	template <typename T>
	bool operator!=(T* p, intrusive_ptr<T> const& iPtr2)
	{
		return (p != iPtr2.get());
	}

	template <typename T, typename U>
	bool operator<(intrusive_ptr<T> const& iPtr1, intrusive_ptr<U> const& iPtr2)
	{
		return ((uintptr_t)iPtr1.get() < (uintptr_t)iPtr2.get());
	}


	/// static_pointer_cast
	/// Returns an intrusive_ptr<T> static-casted from a intrusive_ptr<U>.
	template <class T, class U>
	intrusive_ptr<T> static_pointer_cast(const intrusive_ptr<U>& intrusivePtr)
	{
		return static_cast<T*>(intrusivePtr.get());
	}


	#if EASTL_RTTI_ENABLED

	/// dynamic_pointer_cast
	/// Returns an intrusive_ptr<T> dynamic-casted from a intrusive_ptr<U>.
	template <class T, class U>
	intrusive_ptr<T> dynamic_pointer_cast(const intrusive_ptr<U>& intrusivePtr)
	{
		return dynamic_cast<T*>(intrusivePtr.get());
	}

	#endif


} // namespace eastl


#endif // Header include guard

























