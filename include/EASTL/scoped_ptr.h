///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// *** Note ***
// *** This code is deprecated in favor of the C++11-conforming             *** 
// *** eastl::unique_ptr template class found in <EASTL/unique_ptr.h>       *** 
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_SCOPED_PTR_H
#define EASTL_SCOPED_PTR_H


#include <EASTL/internal/config.h>
#include <EASTL/internal/smart_ptr.h>   // Defines smart_ptr_deleter
#include <stddef.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{
	/// class scoped_ptr
	///
	/// This class is intended to be the same as the C++11 unique_ptr class, 
	/// but was created before there was such a thing. 
	///
	/// This class implements a scoped_ptr template. This is a class which is 
	/// similar to the C++ auto_ptr template, except that it prohibits copying 
	/// of itself, for safety.
	///
	/// More specifically, the scoped_ptr class template stores a pointer to a 
	/// dynamically allocated object.  The object pointed to is automatically 
	/// deleted on destructor of scoped_ptr or can be manually deleted via the 
	/// scopted_ptr::reset function. 
	///
	/// scoped_ptr cannot be used in C++ Standard Library containers; you'll need 
	/// to use the shared_ptr template if you want to do this. The reason you can't
	/// use scoped_ptr is that it prohibits copying. You can't (safely) use auto_ptr
	/// in C++ Standard Library containers because copying of an auto_ptr will 
	/// create a situation whereby objects are multiply freed.
	/// 
	/// scoped_ptr cannot be used with arrays of objects. The reason for this is
	/// that it calls delete on the owned pointer and not delete[]. The latter
	/// allows for the calling of the destructors for the objects of the owned pointer.
	/// If you want to use scoped_ptr with a dynamically allocated array, use the
	/// scoped_array function instead.
	/// 
	template <typename T, typename Deleter = smart_ptr_deleter<T> > 
	class scoped_ptr
	{
	protected:
		/// this_type
		/// This is an alias for scoped_ptr<T>, this class.
		typedef scoped_ptr<T> this_type;

		/// deleter_type
		typedef Deleter deleter_type;

		/// mpValue
		/// The owned pointer.
		T* mpValue;

		/// scoped_ptr
		/// This function is private in order to prevent copying, for safety.
		scoped_ptr(const scoped_ptr&);

		/// scoped_ptr
		/// This function is private in order to prevent copying, for safety.
		scoped_ptr& operator=(const scoped_ptr&);

		/// scoped_ptr
		/// This function is private in order to prevent copying, for safety.
		scoped_ptr& operator=(T* pValue);

	public:
		typedef T element_type;

		/// scoped_ptr
		/// Construct a scoped_ptr from a pointer allocated via new.
		/// Example usage:
		///    scoped_ptr<int> ptr(new int(3));
		explicit scoped_ptr(T* pValue = NULL)
			: mpValue(pValue) {}

		/// ~scoped_ptr
		/// Destroys the owned pointer. The destructor for the object
		/// referred to by the owned pointer will be called.
		~scoped_ptr()
		{
			Deleter del;
			del(mpValue);
		}

		/// reset
		/// Deletes the owned pointer and takes ownership of the 
		/// passed in pointer. If the passed in pointer is the same
		/// as the owned pointer, nothing is done.
		/// Example usage:
		///    scoped_ptr<int> ptr(new int(3));
		///    ptr.reset(new int(4));  // deletes int(3)
		///    ptr.reset(NULL);        // deletes int(4)
		void reset(T* pValue = NULL)
		{
			if(pValue != mpValue)
			{
				Deleter del;
				del(mpValue);
				mpValue = pValue;
			}
		}

		/// detach
		/// This simply forgets the owned pointer. It doesn't 
		/// free it but rather assumes that the user does.
		/// Example usage:
		///    scoped_ptr<int> ptr(new int(3));
		///    int* pInt = ptr.detach();
		///    delete pInt;
		T* detach()
		{
			T* const pTemp = mpValue;
			mpValue = NULL;
			return pTemp;
		}

		/// swap
		/// Exchanges the owned pointer beween two scoped_ptr objects. 
		void swap(this_type& scopedPtr)
		{
			// std::swap(mpValue, scopedPtr.mpValue); // Not used so that we can reduce a dependency.
			T* const pValue   = scopedPtr.mpValue;
			scopedPtr.mpValue = mpValue;
			mpValue           = pValue;
		}

		/// operator*
		/// Returns the owner pointer dereferenced.
		/// Example usage:
		///    scoped_ptr<int> ptr(new int(3));
		///    int x = *ptr;
		typename add_lvalue_reference<T>::type operator*() const
		{
			// assert(mpValue);
			return *mpValue;
		}

		/// operator->
		/// Allows access to the owned pointer via operator->()
		/// Example usage:
		///    struct X{ void DoSomething(); }; 
		///    scoped_ptr<int> ptr(new X);
		///    ptr->DoSomething();
		T* operator->() const
		{
			// assert(mpValue);
			return mpValue;
		}

		/// get
		/// Returns the owned pointer. Note that this class does 
		/// not provide an operator T() function. This is because such
		/// a thing (automatic conversion) is deemed unsafe.
		/// Example usage:
		///    struct X{ void DoSomething(); }; 
		///    scoped_ptr<int> ptr(new X);
		///    X* pX = ptr.get();
		///    pX->DoSomething();
		T* get() const
		{
			return mpValue;
		}

		/// Implicit operator bool
		/// Allows for using a scoped_ptr as a boolean. 
		/// Example usage:
		///    scoped_ptr<int> ptr(new int(3));
		///    if(ptr)
		///        ++*ptr;
		///
		/// Note that below we do not use operator bool(). The reason for this
		/// is that booleans automatically convert up to short, int, float, etc.
		/// The result is that this: if(scopedPtr == 1) would yield true (bad).
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
		///    scoped_ptr<int> ptr(new int(3));
		///    if(!ptr)
		///        assert(false);
		bool operator!() const
		{
			return (mpValue == NULL);
		}

	}; // class scoped_ptr



	/// get_pointer
	/// returns scoped_ptr::get() via the input scoped_ptr. 
	template <typename T, typename D>
	inline T* get_pointer(const scoped_ptr<T, D>& scopedPtr)
	{
		return scopedPtr.get();
	}


	/// swap
	/// Exchanges the owned pointer beween two scoped_ptr objects.
	/// This non-member version is useful for compatibility of scoped_ptr
	/// objects with the C++ Standard Library and other libraries.
	template <typename T, typename D>
	inline void swap(scoped_ptr<T, D>& scopedPtr1, scoped_ptr<T, D>& scopedPtr2)
	{
		scopedPtr1.swap(scopedPtr2);
	}


	/// operator<
	/// Returns which scoped_ptr is 'less' than the other. Useful when storing
	/// sorted containers of scoped_ptr objects.
	template <typename T, typename D>
	inline bool operator<(const scoped_ptr<T, D>& scopedPtr1, const scoped_ptr<T, D>& scopedPtr2)
	{
		return (scopedPtr1.get() < scopedPtr2.get()); // Alternatively use: std::less<T*>(scopedPtr1.get(), scopedPtr2.get());
	}


} // namespace eastl


#endif // Header include guard










