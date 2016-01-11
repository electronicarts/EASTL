/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Implements a generic iterator from a given iteratable type, such as a pointer.
// We cannot put this file into our own iterator.h file because we need to 
// still be able to use this file when we have our iterator.h disabled.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_GENERIC_ITERATOR_H
#define EASTL_INTERNAL_GENERIC_ITERATOR_H


#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#include <EASTL/internal/config.h>
#include <EASTL/iterator.h>
#include <EASTL/type_traits.h>


#ifdef _MSC_VER
	#pragma warning(push)           // VC++ generates a bogus warning that you cannot code away.
	#pragma warning(disable: 4619)  // There is no warning number 'number'.
	#pragma warning(disable: 4217)  // Member template functions cannot be used for copy-assignment or copy-construction.
#endif


namespace eastl
{

	/// generic_iterator
	///
	/// Converts something which can be iterated into a formal iterator.
	/// While this class' primary purpose is to allow the conversion of 
	/// a pointer to an iterator, you can convert anything else to an 
	/// iterator by defining an iterator_traits<> specialization for that
	/// object type. See EASTL iterator.h for this.
	///
	/// Example usage:
	///     typedef generic_iterator<int*>       IntArrayIterator;
	///     typedef generic_iterator<int*, char> IntArrayIteratorOther;
	///
	template <typename Iterator, typename Container = void>
	class generic_iterator
	{
	protected:
		Iterator mIterator;

	public:
		typedef typename eastl::iterator_traits<Iterator>::iterator_category iterator_category;
		typedef typename eastl::iterator_traits<Iterator>::value_type        value_type;
		typedef typename eastl::iterator_traits<Iterator>::difference_type   difference_type;
		typedef typename eastl::iterator_traits<Iterator>::reference         reference;
		typedef typename eastl::iterator_traits<Iterator>::pointer           pointer;
		typedef Iterator                                                     iterator_type;
		typedef iterator_type                                                wrapped_iterator_type;   // This is not in the C++ Standard; it's used by use to identify it as a wrapping iterator type.
		typedef Container                                                    container_type;
		typedef generic_iterator<Iterator, Container>                        this_type;

		generic_iterator()
			: mIterator(iterator_type()) { }

		explicit generic_iterator(const iterator_type& x)
			: mIterator(x) { }

		this_type& operator=(const iterator_type& x)
			{ mIterator = x; return *this; }

		template <typename Iterator2>
		generic_iterator(const generic_iterator<Iterator2, Container>& x)
			: mIterator(x.base()) { }

		reference operator*() const
			{ return *mIterator; }

		pointer operator->() const
			{ return mIterator; }

		this_type& operator++()
			{ ++mIterator; return *this; }

		this_type operator++(int)
			{ return this_type(mIterator++); }

		this_type& operator--()
			{ --mIterator; return *this; }

		this_type operator--(int)
			{ return this_type(mIterator--); }

		reference operator[](const difference_type& n) const
			{ return mIterator[n]; }

		this_type& operator+=(const difference_type& n)
			{ mIterator += n; return *this; }

		this_type operator+(const difference_type& n) const
			{ return this_type(mIterator + n); }

		this_type& operator-=(const difference_type& n)
			{ mIterator -= n; return *this; }

		this_type operator-(const difference_type& n) const
			{ return this_type(mIterator - n); }

		const iterator_type& base() const
			{ return mIterator; }

	}; // class generic_iterator


	template <typename IteratorL, typename IteratorR, typename Container>
	inline bool operator==(const generic_iterator<IteratorL, Container>& lhs, const generic_iterator<IteratorR, Container>& rhs)
		{ return lhs.base() == rhs.base(); }

	template <typename Iterator, typename Container>
	inline bool operator==(const generic_iterator<Iterator, Container>& lhs, const generic_iterator<Iterator, Container>& rhs)
		{ return lhs.base() == rhs.base(); }

	template <typename IteratorL, typename IteratorR, typename Container>
	inline bool operator!=(const generic_iterator<IteratorL, Container>& lhs, const generic_iterator<IteratorR, Container>& rhs)
		{ return lhs.base() != rhs.base(); }

	template <typename Iterator, typename Container>
	inline bool operator!=(const generic_iterator<Iterator, Container>& lhs, const generic_iterator<Iterator, Container>& rhs)
		{ return lhs.base() != rhs.base(); }

	template <typename IteratorL, typename IteratorR, typename Container>
	inline bool operator<(const generic_iterator<IteratorL, Container>& lhs, const generic_iterator<IteratorR, Container>& rhs)
		{ return lhs.base() < rhs.base(); }

	template <typename Iterator, typename Container>
	inline bool operator<(const generic_iterator<Iterator, Container>& lhs, const generic_iterator<Iterator, Container>& rhs)
		{ return lhs.base() < rhs.base(); }

	template <typename IteratorL, typename IteratorR, typename Container>
	inline bool operator>(const generic_iterator<IteratorL, Container>& lhs, const generic_iterator<IteratorR, Container>& rhs)
		{ return lhs.base() > rhs.base(); }

	template <typename Iterator, typename Container>
	inline bool operator>(const generic_iterator<Iterator, Container>& lhs, const generic_iterator<Iterator, Container>& rhs)
		{ return lhs.base() > rhs.base(); }

	template <typename IteratorL, typename IteratorR, typename Container>
	inline bool operator<=(const generic_iterator<IteratorL, Container>& lhs, const generic_iterator<IteratorR, Container>& rhs)
		{ return lhs.base() <= rhs.base(); }

	template <typename Iterator, typename Container>
	inline bool operator<=(const generic_iterator<Iterator, Container>& lhs, const generic_iterator<Iterator, Container>& rhs)
		{ return lhs.base() <= rhs.base(); }

	template <typename IteratorL, typename IteratorR, typename Container>
	inline bool operator>=(const generic_iterator<IteratorL, Container>& lhs, const generic_iterator<IteratorR, Container>& rhs)
		{ return lhs.base() >= rhs.base(); }

	template <typename Iterator, typename Container>
	inline bool operator>=(const generic_iterator<Iterator, Container>& lhs, const generic_iterator<Iterator, Container>& rhs)
		{ return lhs.base() >= rhs.base(); }

	template <typename IteratorL, typename IteratorR, typename Container>
	inline typename generic_iterator<IteratorL, Container>::difference_type
	operator-(const generic_iterator<IteratorL, Container>& lhs, const generic_iterator<IteratorR, Container>& rhs)
		{ return lhs.base() - rhs.base(); }

	template <typename Iterator, typename Container>
	inline generic_iterator<Iterator, Container>
	operator+(typename generic_iterator<Iterator, Container>::difference_type n, const generic_iterator<Iterator, Container>& x)
		{ return generic_iterator<Iterator, Container>(x.base() + n); }



	/// is_generic_iterator
	///
	/// Tells if an iterator is one of these generic_iterators. This is useful if you want to 
	/// write code that uses miscellaneous iterators but wants to tell if they are generic_iterators.
	/// A primary reason to do so is that you can get at the pointer within the generic_iterator.
	///
	template <typename Iterator>
	struct is_generic_iterator : public false_type { };

	template <typename Iterator, typename Container>
	struct is_generic_iterator<generic_iterator<Iterator, Container> > : public true_type { };


	/// unwrap_generic_iterator
	///
	/// Returns Iterator::get_base() if it's a generic_iterator, else returns Iterator as-is.
	///
	/// Example usage:
	///      vector<int> intVector;
	///      eastl::generic_iterator<vector<int>::iterator> genericIterator(intVector.begin());
	///      vector<int>::iterator it = unwrap_generic_iterator(genericIterator);
	///
	template <typename Iterator>
	inline typename eastl::is_iterator_wrapper_helper<Iterator, eastl::is_generic_iterator<Iterator>::value>::iterator_type unwrap_generic_iterator(Iterator it)
		{ return eastl::is_iterator_wrapper_helper<Iterator, eastl::is_generic_iterator<Iterator>::value>::get_base(it); }


} // namespace eastl


#ifdef _MSC_VER
	#pragma warning(pop)
#endif


#endif // Header include guard
















