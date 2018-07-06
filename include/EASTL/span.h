/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements the eastl::span which is part of the C++ standard
// STL library specification.
//
// eastl::span is a non-owning container that refers to a contiguous block of
// memory.  It bundles up the classic pattern of a pointer and a size into a
// single type.  A span can either have a static extent, in which case the
// number of elements in the sequence is known and encoded in the type, or a
// dynamic extent.
//
// http://en.cppreference.com/w/cpp/container/span
// http://eel.is/c++draft/views#span.syn
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_SPAN_H
#define EASTL_SPAN_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once 
#endif

#include <EASTL/internal/config.h>
#include <EASTL/type_traits.h>
#include <EASTL/iterator.h>
#include <EASTL/array.h>

namespace eastl
{
	namespace Internal 
	{
		// HasSizeAndData
		// 
		// custom type trait to determine if eastl::data(Container) and eastl::size(Container) are well-formed.
		//
		template <typename, typename = void>
		struct HasSizeAndData : eastl::false_type {};

		template <typename T>
		struct HasSizeAndData<T, void_t<decltype(eastl::size(eastl::declval<T>())), decltype(eastl::data(eastl::declval<T>()))>> : eastl::true_type {};
	}

	static EA_CONSTEXPR ptrdiff_t dynamic_extent = ptrdiff_t(-1);

	template <typename T, ptrdiff_t Extent = eastl::dynamic_extent>
	class span
	{
	public:
		typedef T                                       element_type;
		typedef remove_cv_t<T>                          value_type;
		typedef ptrdiff_t                               index_type;
		typedef ptrdiff_t                               difference_type;
		typedef T*                                      pointer;
		typedef T&                                      reference;
		typedef T*                                      iterator;
		typedef const T*                                const_iterator;
		typedef eastl::reverse_iterator<iterator>       reverse_iterator;
		typedef eastl::reverse_iterator<const_iterator> const_reverse_iterator;

		static EA_CONSTEXPR ptrdiff_t extent = Extent;

		// constructors / destructor
		EA_CONSTEXPR span() EA_NOEXCEPT = default;
		EA_CONSTEXPR span(const span& other) EA_NOEXCEPT = default;
		EA_CONSTEXPR span(pointer ptr, index_type count);
		EA_CONSTEXPR span(pointer pBegin, pointer pEnd);
		            ~span() EA_NOEXCEPT = default;

		// copy-assignment operator
		EA_CPP14_CONSTEXPR span& operator=(const span& other) EA_NOEXCEPT = default;

		// conversion constructors for c-array and eastl::array
		template <size_t N> EA_CONSTEXPR span(element_type (&arr)[N]) EA_NOEXCEPT;
		template <size_t N> EA_CONSTEXPR span(eastl::array<value_type, N>& arr) EA_NOEXCEPT;
		template <size_t N> EA_CONSTEXPR span(const eastl::array<value_type, N>& arr) EA_NOEXCEPT;

		// SfinaeForGenericContainers
		//
		template <typename Container>
		using SfinaeForGenericContainers =
		    enable_if_t<!is_same_v<Container, span> && !is_same_v<Container, array<value_type>> &&
		                !is_array_v<Container> &&
		                Internal::HasSizeAndData<Container>::value &&
		                is_convertible_v<remove_pointer_t<decltype(eastl::data(eastl::declval<Container>()))> (*)[], element_type (*)[]>>;

		// generic container conversion constructors
		template <typename Container, typename = SfinaeForGenericContainers<Container>>
		EA_CONSTEXPR span(Container& cont);

		template <typename Container, typename = SfinaeForGenericContainers<Container>>
		EA_CONSTEXPR span(const Container& cont);

		template <typename U, ptrdiff_t N, typename = enable_if_t<(Extent == eastl::dynamic_extent || N == Extent) && (is_convertible_v<U(*)[], element_type(*)[]>)>>
		EA_CONSTEXPR span(const span<U, N>& s) EA_NOEXCEPT;

		// subviews
		template<ptrdiff_t Count>
		EA_CPP14_CONSTEXPR span<element_type, Count> first() const;
		EA_CPP14_CONSTEXPR span<element_type, dynamic_extent> first(ptrdiff_t Count) const;

		template<ptrdiff_t Count>
		EA_CPP14_CONSTEXPR span<element_type, Count> last() const;
		EA_CPP14_CONSTEXPR span<element_type, dynamic_extent> last(ptrdiff_t Count) const;

		// template <ptrdiff_t Offset, ptrdiff_t Count = dynamic_extent>
		// EA_CONSTEXPR span<element_type, E [> see below <]> subspan() const;
		// EA_CONSTEXPR span<element_type, dynamic_extent> subspan(ptrdiff_t Offset, ptrdiff_t Count = dynamic_extent) const;

		// observers
		EA_CONSTEXPR pointer    data() const EA_NOEXCEPT;
		EA_CONSTEXPR index_type size() const EA_NOEXCEPT;
		EA_CONSTEXPR index_type size_bytes() const EA_NOEXCEPT;
		EA_CONSTEXPR bool       empty() const EA_NOEXCEPT;

		// subscript operators, element access
		EA_CONSTEXPR reference operator[](index_type idx) const;
		EA_CONSTEXPR reference operator()(index_type idx) const;

		// iterator support
		EA_CONSTEXPR iterator begin() const EA_NOEXCEPT;
		EA_CONSTEXPR iterator end() const EA_NOEXCEPT;
		EA_CONSTEXPR const_iterator cbegin() const EA_NOEXCEPT;
		EA_CONSTEXPR const_iterator cend() const EA_NOEXCEPT;
		EA_CONSTEXPR reverse_iterator rbegin() const EA_NOEXCEPT;
		EA_CONSTEXPR reverse_iterator rend() const EA_NOEXCEPT;
		EA_CONSTEXPR const_reverse_iterator crbegin() const EA_NOEXCEPT;
		EA_CONSTEXPR const_reverse_iterator crend() const EA_NOEXCEPT;

	private:
		pointer mpData = nullptr;
		index_type mnSize = 0;

	private:
		EA_CONSTEXPR bool bounds_check(ptrdiff_t) const;  // utility used in asserts
	};


	///////////////////////////////////////////////////////////////////////////
	// template deduction guides 
	///////////////////////////////////////////////////////////////////////////

	// template<class T, size_t N> span(T (&)[N]) ->           span <T, N>;
	// template<class T, size_t N> span(array<T, N>&) ->       span <T, N>;
	// template<class T, size_t N> span(const array<T, N>&) -> span <const T, N>;
	// template<class Container>   span(Container&) ->         span <typename Container::value_type>;
	// template<class Container>   span(const Container&) ->   span <const typename Container::value_type>;


	///////////////////////////////////////////////////////////////////////////
	// comparison operators
	///////////////////////////////////////////////////////////////////////////

	template <class T, ptrdiff_t X, class U, ptrdiff_t Y>
	EA_CONSTEXPR bool operator==(span<T, X> l, span<U, Y> r)
	{
		return (l.size() == r.size()) && eastl::equal(l.begin(), l.end(), r.begin());
	}

	template <class T, ptrdiff_t X, class U, ptrdiff_t Y>
	EA_CONSTEXPR bool operator<(span<T, X> l, span<U, Y> r)
	{
		return lexicographical_compare(l.begin(), l.end(), r.begin(), r.end());
	}

	template <class T, ptrdiff_t X, class U, ptrdiff_t Y>
	EA_CONSTEXPR bool operator!=(span<T, X> l, span<U, Y> r) { return !(l == r); }

	template <class T, ptrdiff_t X, class U, ptrdiff_t Y>
	EA_CONSTEXPR bool operator<=(span<T, X> l, span<U, Y> r) { return !(r < l); }

	template <class T, ptrdiff_t X, class U, ptrdiff_t Y>
	EA_CONSTEXPR bool operator>(span<T, X> l, span<U, Y> r) { return r < l; }

	template <class T, ptrdiff_t X, class U, ptrdiff_t Y>
	EA_CONSTEXPR bool operator>=(span<T, X> l, span<U, Y> r) { return !(l < r); }


	///////////////////////////////////////////////////////////////////////////
	// ctor implementations
	///////////////////////////////////////////////////////////////////////////

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR span<T, Extent>::span(pointer ptr, index_type size)
	    : mpData(ptr), mnSize(size)
	{
	}

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR span<T, Extent>::span(pointer pBegin, pointer pEnd)
	    : mpData(pBegin), mnSize(pEnd - pBegin)
	{
	}

	template <typename T, ptrdiff_t Extent>
	template <size_t N>
	EA_CONSTEXPR span<T, Extent>::span(element_type(&arr)[N]) EA_NOEXCEPT 
		: span(arr, static_cast<index_type>(N))
	{
	}

	template <typename T, ptrdiff_t Extent>
	template <size_t N>
	EA_CONSTEXPR span<T, Extent>::span(eastl::array<value_type, N> &arr) EA_NOEXCEPT 
		: span(arr.data(), arr.size())
	{
	}

	template <typename T, ptrdiff_t Extent>
	template <size_t N>
	EA_CONSTEXPR span<T, Extent>::span(const eastl::array<value_type, N>& arr) EA_NOEXCEPT
		: span(arr.data(), arr.size())
	{
	}


	template <typename T, ptrdiff_t Extent>
	template <typename Container, typename>
	EA_CONSTEXPR span<T, Extent>::span(Container& cont)
		: span(static_cast<pointer>(eastl::data(cont)), static_cast<index_type>(eastl::size(cont)))
	{
	}

	template <typename T, ptrdiff_t Extent>
	template <typename Container, typename>
	EA_CONSTEXPR span<T, Extent>::span(const Container& cont)
		: span(static_cast<pointer>(eastl::data(cont)), static_cast<index_type>(eastl::size(cont)))
	{
	}

	template <typename T, ptrdiff_t Extent>
	template <typename U, ptrdiff_t N, typename>
	EA_CONSTEXPR span<T, Extent>::span(const span<U, N>& s) EA_NOEXCEPT
		: span(s.data(), s.size())
	{
	}


	///////////////////////////////////////////////////////////////////////////
	// member function implementations
	///////////////////////////////////////////////////////////////////////////

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR typename span<T, Extent>::pointer span<T, Extent>::data() const EA_NOEXCEPT
	{
		return mpData;
	}

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR typename span<T, Extent>::index_type span<T, Extent>::size() const EA_NOEXCEPT
	{
		return mnSize;
	}

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR typename span<T, Extent>::index_type span<T, Extent>::size_bytes() const EA_NOEXCEPT
	{
		return size() * sizeof(element_type);
	}

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR bool span<T, Extent>::empty() const EA_NOEXCEPT
	{
		return size() == 0; 
	}

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR typename span<T, Extent>::reference span<T, Extent>::operator[](index_type idx) const
	{
		EASTL_ASSERT_MSG(!empty(),          "undefined behavior accessing an empty span");
		EASTL_ASSERT_MSG(bounds_check(idx), "undefined behavior accessing out of bounds");

		return mpData[idx];
	}

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR typename span<T, Extent>::reference span<T, Extent>::operator()(index_type idx) const
	{
		EASTL_ASSERT_MSG(!empty(),          "undefined behavior accessing an empty span");
		EASTL_ASSERT_MSG(bounds_check(idx), "undefined behavior accessing out of bounds");

		return mpData[idx];
	}

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR typename span<T, Extent>::iterator span<T, Extent>::begin() const EA_NOEXCEPT
	{
		return mpData;
	}

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR typename span<T, Extent>::iterator span<T, Extent>::end() const EA_NOEXCEPT
	{
		return mpData + mnSize;
	}

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR typename span<T, Extent>::const_iterator span<T, Extent>::cbegin() const EA_NOEXCEPT
	{
		return mpData;
	}

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR typename span<T, Extent>::const_iterator span<T, Extent>::cend() const EA_NOEXCEPT
	{
		return mpData + mnSize;
	}

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR typename span<T, Extent>::reverse_iterator span<T, Extent>::rbegin() const EA_NOEXCEPT
	{
		return mpData + mnSize;
	}

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR typename span<T, Extent>::reverse_iterator span<T, Extent>::rend() const EA_NOEXCEPT
	{
		return mpData;
	}

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR typename span<T, Extent>::const_reverse_iterator span<T, Extent>::crbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(mpData + mnSize);
	}

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR typename span<T, Extent>::const_reverse_iterator span<T, Extent>::crend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(mpData);
	}

	template <typename T, ptrdiff_t Extent>
	template <ptrdiff_t Count>
	EA_CPP14_CONSTEXPR span<typename span<T, Extent>::element_type, Count> span<T, Extent>::first() const
	{
		EASTL_ASSERT_MSG(bounds_check(Count), "undefined behavior accessing out of bounds");
		return {data(), Count};
	}

	template <typename T, ptrdiff_t Extent>
	EA_CPP14_CONSTEXPR span<typename span<T, Extent>::element_type, dynamic_extent> 
	span<T, Extent>::first(ptrdiff_t sz) const
	{
		EASTL_ASSERT_MSG(bounds_check(sz), "undefined behavior accessing out of bounds");
		return {data(), sz};
	}

	template <typename T, ptrdiff_t Extent>
	template <ptrdiff_t Count>
	EA_CPP14_CONSTEXPR span<typename span<T, Extent>::element_type, Count> span<T, Extent>::last() const
	{
		EASTL_ASSERT_MSG(bounds_check(Count), "undefined behavior accessing out of bounds");
		return {data() + size() - Count, Count};
	}

	template <typename T, ptrdiff_t Extent>
	EA_CPP14_CONSTEXPR span<typename span<T, Extent>::element_type, dynamic_extent> 
	span<T, Extent>::last(ptrdiff_t sz) const
	{
		EASTL_ASSERT_MSG(bounds_check(sz), "undefined behavior accessing out of bounds");
		return {data() + size() - sz, sz};
	}

	template <typename T, ptrdiff_t Extent>
	EA_CONSTEXPR bool span<T, Extent>::bounds_check(ptrdiff_t sz) const
	{
		return (sz >= 0 && sz < size());
	}
}

#endif // EASTL_SPAN_H  
