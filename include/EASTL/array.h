/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Implements a templated array class as per the C++ standard TR1 (technical
// report 1, which is a list of proposed C++ library amendments).
// The primary distinctions between this array and TR1 array are:
//    - array::size_type is defined as eastl_size_t instead of size_t in order
//      to save memory and run faster on 64 bit systems.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ARRAY_H
#define EASTL_ARRAY_H


#include <EASTL/internal/config.h>
#include <EASTL/iterator.h>
#include <EASTL/algorithm.h>
#include <EASTL/utility.h>
#include <stddef.h>

#if EASTL_EXCEPTIONS_ENABLED
	EA_DISABLE_ALL_VC_WARNINGS()
	#include <stdexcept> // std::out_of_range, std::length_error.
	EA_RESTORE_ALL_VC_WARNINGS()
#endif

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	///////////////////////////////////////////////////////////////////////
	/// array
	///
	/// Implements a templated array class as per the C++ standard TR1.
	/// This class allows you to use a built-in C style array like an STL vector.
	/// It does not let you change its size, as it is just like a C built-in array.
	/// Our implementation here strives to remove function call nesting, as that 
	/// makes it hard for us to profile debug builds due to function call overhead.
	/// Note that this is intentionally a struct with public data, as per the 
	/// C++ standard update proposal requirements.
	///
	/// Example usage:
	///    array<int, 5> a = { { 0, 1, 2, 3, 4 } }; // Strict compilers such as GCC require the double brackets.
	///    a[2] = 4;
	///    for(array<int, 5>::iterator i = a.begin(); i < a.end(); ++i)
	///       *i = 0;
	///
	template <typename T, size_t N = 1>
	struct array
	{
	public:
		typedef array<T, N>                                   this_type;
		typedef T                                             value_type;
		typedef value_type&                                   reference;
		typedef const value_type&                             const_reference;
		typedef value_type*                                   iterator;
		typedef const value_type*                             const_iterator;
		typedef eastl::reverse_iterator<iterator>             reverse_iterator;
		typedef eastl::reverse_iterator<const_iterator>       const_reverse_iterator;
		typedef eastl_size_t                                  size_type;        // See config.h for the definition of eastl_size_t, which defaults to size_t.
		typedef ptrdiff_t                                     difference_type;

	public:
		enum
		{
			count = N
		};

		// Note that the member data is intentionally public. 
		// This allows for aggregate initialization of the 
		// object (e.g. array<int, 5> a = { 0, 3, 2, 4 }; ) 
		value_type mValue[N ? N : 1];

	public:
		// We intentionally provide no constructor, destructor, or assignment operator.

		void fill(const value_type& value);

		// Unlike the swap function for other containers, array::swap takes linear time, 
		// may exit via an exception, and does not cause iterators to become associated with the other container.
		void swap(this_type& x) EA_NOEXCEPT_IF(eastl::is_nothrow_swappable<value_type>::value); 

		EA_CPP14_CONSTEXPR iterator       begin() EA_NOEXCEPT;
		EA_CPP14_CONSTEXPR const_iterator begin() const EA_NOEXCEPT;
		EA_CPP14_CONSTEXPR const_iterator cbegin() const EA_NOEXCEPT;

		EA_CPP14_CONSTEXPR iterator       end() EA_NOEXCEPT;
		EA_CPP14_CONSTEXPR const_iterator end() const EA_NOEXCEPT;
		EA_CPP14_CONSTEXPR const_iterator cend() const EA_NOEXCEPT;

		EA_CPP14_CONSTEXPR reverse_iterator       rbegin() EA_NOEXCEPT;
		EA_CPP14_CONSTEXPR const_reverse_iterator rbegin() const EA_NOEXCEPT;
		EA_CPP14_CONSTEXPR const_reverse_iterator crbegin() const EA_NOEXCEPT;

		EA_CPP14_CONSTEXPR reverse_iterator       rend() EA_NOEXCEPT;
		EA_CPP14_CONSTEXPR const_reverse_iterator rend() const EA_NOEXCEPT;
		EA_CPP14_CONSTEXPR const_reverse_iterator crend() const EA_NOEXCEPT;

		EA_CPP14_CONSTEXPR bool empty() const EA_NOEXCEPT;
		EA_CPP14_CONSTEXPR size_type size() const EA_NOEXCEPT;
		EA_CPP14_CONSTEXPR size_type max_size() const EA_NOEXCEPT;

		EA_CPP14_CONSTEXPR T*       data() EA_NOEXCEPT;
		EA_CPP14_CONSTEXPR const T* data() const EA_NOEXCEPT;

		EA_CPP14_CONSTEXPR reference       operator[](size_type i);
		EA_CPP14_CONSTEXPR const_reference operator[](size_type i) const;
		EA_CPP14_CONSTEXPR const_reference at(size_type i) const;
		EA_CPP14_CONSTEXPR reference       at(size_type i);

		EA_CPP14_CONSTEXPR reference       front();
		EA_CPP14_CONSTEXPR const_reference front() const;

		EA_CPP14_CONSTEXPR reference       back();
		EA_CPP14_CONSTEXPR const_reference back() const;

		bool validate() const;
		int  validate_iterator(const_iterator i) const;

	}; // class array


	///////////////////////////////////////////////////////////////////////////
	// template deduction guides
	///////////////////////////////////////////////////////////////////////////
	#ifdef __cpp_deduction_guides
		template <class T, class... U> array(T, U...) -> array<T, 1 + sizeof...(U)>;
	#endif


	///////////////////////////////////////////////////////////////////////
	// array
	///////////////////////////////////////////////////////////////////////


	template <typename T, size_t N>
	inline void array<T, N>::fill(const value_type& value)
	{
		eastl::fill_n(&mValue[0], N, value);
	}


	template <typename T, size_t N>
	inline void array<T, N>::swap(this_type& x) EA_NOEXCEPT_IF(eastl::is_nothrow_swappable<value_type>::value)
	{
		eastl::swap_ranges(&mValue[0], &mValue[N], &x.mValue[0]);
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::iterator
	array<T, N>::begin() EA_NOEXCEPT
	{
		return &mValue[0];
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::const_iterator
	array<T, N>::begin() const EA_NOEXCEPT
	{
		return &mValue[0];
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::const_iterator
	array<T, N>::cbegin() const EA_NOEXCEPT
	{
		return &mValue[0];
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::iterator
	array<T, N>::end() EA_NOEXCEPT
	{
		return &mValue[N];
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::const_iterator
	array<T, N>::end() const EA_NOEXCEPT
	{
		return &mValue[N];
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::const_iterator
	array<T, N>::cend() const EA_NOEXCEPT
	{
		return &mValue[N];
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::reverse_iterator
	array<T, N>::rbegin() EA_NOEXCEPT
	{
		return reverse_iterator(&mValue[N]);
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::const_reverse_iterator
	array<T, N>::rbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(&mValue[N]);
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::const_reverse_iterator
	array<T, N>::crbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(&mValue[N]);
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::reverse_iterator
	array<T, N>::rend() EA_NOEXCEPT
	{
		return reverse_iterator(&mValue[0]);
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::const_reverse_iterator
	array<T, N>::rend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(static_cast<const_iterator>(&mValue[0]));
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::const_reverse_iterator
	array<T, N>::crend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(static_cast<const_iterator>(&mValue[0]));
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::size_type
	array<T, N>::size() const EA_NOEXCEPT
	{
		return (size_type)N;
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::size_type
	array<T, N>::max_size() const EA_NOEXCEPT
	{
		return (size_type)N;
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline bool array<T, N>::empty() const EA_NOEXCEPT
	{
		return (N == 0);
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::reference
	array<T, N>::operator[](size_type i)
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(i >= N))
				EASTL_FAIL_MSG("array::operator[] -- out of range");
		#endif

		EA_ANALYSIS_ASSUME(i < N);
		return mValue[i];
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::const_reference
	array<T, N>::operator[](size_type i) const
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(i >= N))
				EASTL_FAIL_MSG("array::operator[] -- out of range");

		#endif

		EA_ANALYSIS_ASSUME(i < N);
		return mValue[i];
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::reference
	array<T, N>::front()
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(empty())) // We don't allow the user to reference an empty container.
				EASTL_FAIL_MSG("array::front -- empty array");
		#endif

		return mValue[0];
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::const_reference  
	array<T, N>::front() const
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(empty())) // We don't allow the user to reference an empty container.
				EASTL_FAIL_MSG("array::front -- empty array");
		#endif

		return mValue[0];
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::reference
	array<T, N>::back()
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(empty())) // We don't allow the user to reference an empty container.
				EASTL_FAIL_MSG("array::back -- empty array");
		#endif

		return mValue[N - 1];
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::const_reference
	array<T, N>::back() const
	{
		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(empty())) // We don't allow the user to reference an empty container.
				EASTL_FAIL_MSG("array::back -- empty array");
		#endif

		return mValue[N - 1];
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline T* array<T, N>::data() EA_NOEXCEPT
	{
		return mValue;
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline const T* array<T, N>::data() const EA_NOEXCEPT
	{
		return mValue;
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::const_reference array<T, N>::at(size_type i) const
	{
		#if EASTL_EXCEPTIONS_ENABLED
			if(EASTL_UNLIKELY(i >= N))
				throw std::out_of_range("array::at -- out of range");
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(i >= N))
				EASTL_FAIL_MSG("array::at -- out of range");
		#endif

		EA_ANALYSIS_ASSUME(i < N);
		return static_cast<const_reference>(mValue[i]); 
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline typename array<T, N>::reference array<T, N>::at(size_type i)
	{
		#if EASTL_EXCEPTIONS_ENABLED
			if(EASTL_UNLIKELY(i >= N))
				throw std::out_of_range("array::at -- out of range");
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(i >= N))
				EASTL_FAIL_MSG("array::at -- out of range");
		#endif

		EA_ANALYSIS_ASSUME(i < N);
		return static_cast<reference>(mValue[i]);
	}


	template <typename T, size_t N>
	inline bool array<T, N>::validate() const
	{
		return true; // There is nothing to do.
	}


	template <typename T, size_t N>
	inline int array<T, N>::validate_iterator(const_iterator i) const
	{
		if(i >= mValue)
		{
			if(i < (mValue + N))
				return (isf_valid | isf_current | isf_can_dereference);

			if(i <= (mValue + N))
				return (isf_valid | isf_current);
		}

		return isf_none;
	}



	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline bool operator==(const array<T, N>& a, const array<T, N>& b)
	{
		return eastl::equal(&a.mValue[0], &a.mValue[N], &b.mValue[0]);
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline bool operator<(const array<T, N>& a, const array<T, N>& b)
	{
		return eastl::lexicographical_compare(&a.mValue[0], &a.mValue[N], &b.mValue[0], &b.mValue[N]);
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline bool operator!=(const array<T, N>& a, const array<T, N>& b)
	{
		return !eastl::equal(&a.mValue[0], &a.mValue[N], &b.mValue[0]);
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline bool operator>(const array<T, N>& a, const array<T, N>& b)
	{
		return eastl::lexicographical_compare(&b.mValue[0], &b.mValue[N], &a.mValue[0], &a.mValue[N]);
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline bool operator<=(const array<T, N>& a, const array<T, N>& b)
	{
		return !eastl::lexicographical_compare(&b.mValue[0], &b.mValue[N], &a.mValue[0], &a.mValue[N]);
	}


	template <typename T, size_t N>
	EA_CPP14_CONSTEXPR inline bool operator>=(const array<T, N>& a, const array<T, N>& b)
	{
		return !eastl::lexicographical_compare(&a.mValue[0], &a.mValue[N], &b.mValue[0], &b.mValue[N]);
	}


	template <typename T, size_t N>
	inline void swap(array<T, N>& a, array<T, N>& b)
	{
		eastl::swap_ranges(&a.mValue[0], &a.mValue[N], &b.mValue[0]);
	}


} // namespace eastl


#endif // Header include guard










