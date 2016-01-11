///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// *** Note ***
// *** This code is deprecated in favor of the C++11-conforming             *** 
// *** eastl::unique_ptr template class found in <EASTL/unique_ptr.h>       *** 
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_SCOPED_ARRAY_H
#define EASTL_SCOPED_ARRAY_H


#include <EASTL/internal/config.h>
#include <EASTL/internal/smart_ptr.h>   // Defines smart_array_deleter
#include <stddef.h>                     // Definition of ptrdiff_t

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// class scoped_array
	///
	/// A scoped_array is the same as scoped_ptr but for arrays. 
	///
	template <typename T, typename Deleter = smart_array_deleter<T> >
	class scoped_array
	{
	protected:
		/// this_type
		/// This is an alias for scoped_array<T>, this class.
		typedef scoped_array<T> this_type;

		/// deleter_type
		typedef Deleter deleter_type;

		/// mpArray
		/// The owned pointer. Points to an array of T.
		T* mpArray;

		/// scoped_array
		/// This function is private in order to prevent copying, for safety.
		scoped_array(const scoped_array&);

		/// scoped_array
		/// This function is private in order to prevent copying, for safety.
		scoped_array& operator=(const scoped_array&);

		/// scoped_ptr
		/// This function is private in order to prevent copying, for safety.
		scoped_array& operator=(T* pValue);

	public:
		typedef T element_type;

		/// scoped_ptr
		/// Construct a scoped_ptr from a pointer allocated via new.
		/// Example usage:
		///    scoped_array<int> ptr(new int[6]);
		explicit scoped_array(T* pArray = NULL)
			: mpArray(pArray) {}

		/// ~scoped_array
		/// Destroys the owned pointer. The destructors for each of the objects
		/// in the owned array will be called. 
		~scoped_array()
		{
			Deleter del;
			del(mpArray);
		}

		/// reset
		/// Deletes the owned pointer and takes ownership of the 
		/// passed in pointer. If the passed in pointer is the same
		/// as the owned pointer, nothing is done.
		/// Example usage:
		///    scoped_array<int> ptr(new int[6]);
		///    ptr.reset(new int[7]);  // deletes int[6]
		///    ptr.reset(NULL);        // deletes int[7]
		void reset(T* pArray = NULL)
		{
			if(pArray != mpArray)
			{
				Deleter del;
				del(mpArray);
				mpArray = pArray;
			}
		}

		/// detach
		/// This simply forgets the owned pointer. It doesn't 
		/// free it but rather assumes that the user does.
		/// Example usage:
		///    scoped_array<int> ptr(new int[6]);
		///    int* pIntArray = ptr.get();
		///    ptr.detach();
		///    delete[] pIntArray;
		T* detach()
		{
			T* const pTemp = mpArray;
			mpArray = NULL;
			return pTemp;
		}

		/// swap
		/// Exchanges the owned pointer beween two scoped_array objects. 
		void swap(this_type& scopedArray)
		{
			// std::swap(mpArray, scopedArray.mpArray); // Not used so that we can reduce a dependency.
			T* const pArray     = scopedArray.mpArray;
			scopedArray.mpArray = mpArray;
			mpArray             = pArray;
		}

		/// operator[]
		/// Returns a reference to the specified item in the owned pointer
		/// array. 
		/// Example usage:
		///    scoped_array<int> ptr(new int[6]);
		///    int x = ptr[2];
		T& operator[](ptrdiff_t i) const
		{
			// assert(mpArray && (i >= 0));
			return mpArray[i];
		}

		/// get
		/// Returns the owned array pointer. 
		/// Example usage:
		///    struct X{ void DoSomething(); }; 
		///    scoped_array<int> ptr(new X[8]);
		///    X** ppX = ptr.get();
		///    ppX[2]->DoSomething();
		T* get() const
		{
			return mpArray;
		}

		/// Implicit operator bool
		/// Allows for using a scoped_ptr as a boolean. 
		/// Example usage:
		///    scoped_array<int> ptr(new int[8]);
		///    if(ptr)
		///        ++ptr[2];
		///     
		/// Note that below we do not use operator bool(). The reason for this
		/// is that booleans automatically convert up to short, int, float, etc.
		/// The result is that this: if(scopedArray == 1) would yield true (bad).
		typedef T* (this_type::*bool_)() const;
		operator bool_() const
		{
			if(mpArray)
				return &this_type::get;
			return NULL;
		}

		/// operator!
		/// This returns the opposite of operator bool; it returns true if 
		/// the owned pointer is null. Some compilers require this and some don't.
		///    scoped_array<int> ptr(new int(3));
		///    if(!ptr)
		///        assert(false);
		bool operator!() const
		{
			return (mpArray == NULL);
		}

	}; // class scoped_array


	/// unique_array
	///
	/// Example usage:
	///    unique_array<int> uniqueIntArray;
	/// Example usage:
	///    UNIQUE_ARRAY_T(int, eastl::smart_ptr_deleter<int>) uniqueIntArray;
	///
	#if defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		#define UNIQUE_ARRAY_T(T, Deleter) scoped_array<T, Deleter>
	#else
		template <typename T, typename Deleter = smart_ptr_deleter<T> >
		using unique_array = scoped_array<T, Deleter>;
		#define UNIQUE_ARRAY_T(T, Deleter) unique_array<T, Deleter>
	#endif



	/// scoped_array
	/// returns scoped_array::get() via the input scoped_array. 
	template <typename T, typename D>
	inline T* get_pointer(const scoped_array<T, D>& scopedArray)
	{
		return scopedArray.get();
	}


	/// swap
	/// Exchanges the owned pointer beween two scoped_array objects.
	/// This non-member version is useful for compatibility of scoped_array
	/// objects with the C++ Standard Library and other libraries.
	template <typename T, typename D>
	inline void swap(scoped_array<T, D>& scopedArray1, scoped_array<T, D>& scopedArray2)
	{
		scopedArray1.swap(scopedArray2);
	}


	/// operator<
	/// Returns which scoped_array is 'less' than the other. Useful when storing
	/// sorted containers of scoped_array objects.
	template <typename T, typename D>
	inline bool operator<(const scoped_array<T, D>& scopedArray1, const scoped_array<T, D>& scopedArray2)
	{
		return (scopedArray1.get() < scopedArray2.get()); // Alternatively use: std::less<T*>(scopedArray1.get(), scopedArray2.get());
	}


} // namespace eastl


#endif // Header include guard








