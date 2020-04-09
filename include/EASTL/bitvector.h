/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Implements a bit vector, which is essentially a vector of bool but which
// uses bits instead of bytes. It is thus similar to the original std::vector<bool>.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Note: This code is not yet complete: it isn't tested and doesn't yet 
//       support containers other than vector.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_BITVECTOR_H
#define EASTL_BITVECTOR_H


#include <EASTL/internal/config.h>
#include <EASTL/vector.h>
#include <EASTL/algorithm.h>
#include <EASTL/bitset.h>

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4480)  // nonstandard extension used: specifying underlying type for enum
#endif

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// EASTL_BITVECTOR_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_BITVECTOR_DEFAULT_NAME
		#define EASTL_BITVECTOR_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " bitvector" // Unless the user overrides something, this is "EASTL bitvector".
	#endif

	/// EASTL_BITVECTOR_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_BITVECTOR_DEFAULT_ALLOCATOR
		#define EASTL_BITVECTOR_DEFAULT_ALLOCATOR allocator_type(EASTL_BITVECTOR_DEFAULT_NAME)
	#endif



	/// BitvectorWordType
	/// Defines the integral data type used by bitvector.
	typedef EASTL_BITSET_WORD_TYPE_DEFAULT BitvectorWordType;


	template <typename Element>
	class bitvector_const_iterator;


	template <typename Element>
	class bitvector_reference
	{
	public:
		typedef eastl_size_t size_type;
		bitvector_reference(Element* ptr, eastl_size_t i);

		bitvector_reference& operator=(bool value);
		bitvector_reference& operator=(const bitvector_reference& rhs);

		operator bool() const // Defined here because some compilers fail otherwise.
			{ return (*mpBitWord & (Element(1) << mnBitIndex)) != 0; }

	protected:
		friend class bitvector_const_iterator<Element>;

		Element*  mpBitWord;
		size_type mnBitIndex;

		bitvector_reference() {}
		void CopyFrom(const bitvector_reference& rhs);
	};



	template <typename Element>
	class bitvector_const_iterator
	{
	public:
		typedef EASTL_ITC_NS::random_access_iterator_tag iterator_category;
		typedef bitvector_const_iterator<Element>        this_type;
		typedef bool                                     value_type;
		typedef bitvector_reference<Element>             reference_type;
		typedef ptrdiff_t                                difference_type;
		typedef Element                                  element_type;
		typedef element_type*                            pointer;           // This is wrong. It needs to be someting that acts as a pointer to a bit.
		typedef element_type&                            reference;         // This is not right. It needs to be someting that acts as a pointer to a bit.
		typedef eastl_size_t                             size_type;

	protected:
		reference_type mReference;

		enum
		{
			kBitCount = (8 * sizeof(Element))
		};

	public:
		bool operator*() const;
		bool operator[](difference_type n) const;

		bitvector_const_iterator();
		bitvector_const_iterator(const element_type* p, eastl_size_t i);
		bitvector_const_iterator(const reference_type& referenceType);

		bitvector_const_iterator& operator++();
		bitvector_const_iterator  operator++(int);
		bitvector_const_iterator& operator--();
		bitvector_const_iterator  operator--(int);

		bitvector_const_iterator& operator+=(difference_type dist);
		bitvector_const_iterator& operator-=(difference_type dist);
		bitvector_const_iterator  operator+ (difference_type dist) const;
		bitvector_const_iterator  operator- (difference_type dist) const;

		difference_type operator-(const this_type& rhs) const;

		bitvector_const_iterator& operator= (const this_type& rhs);

		bool operator==(const this_type& rhs) const;
		bool operator!=(const this_type& rhs) const;

		bool operator< (const this_type& rhs) const;
		bool operator<=(const this_type& rhs) const;
		bool operator> (const this_type& rhs) const;
		bool operator>=(const this_type& rhs) const;

		int validate(const element_type* pStart, const element_type* pEnd, eastl_size_t nExtraBits) const;

	protected:
		template <typename, typename, typename>
		friend class bitvector;

		reference_type& get_reference_type() { return mReference; }
	};



	template <typename Element>
	class bitvector_iterator : public bitvector_const_iterator<Element>
	{
	public:
		typedef EASTL_ITC_NS::random_access_iterator_tag iterator_category;
		typedef bitvector_iterator                       this_type;
		typedef bitvector_const_iterator<Element>        base_type;
		typedef bool                                     value_type;
		typedef bitvector_reference<Element>             reference_type;
		typedef ptrdiff_t                                difference_type;
		typedef Element                                  element_type;
		typedef element_type*                            pointer;           // This is wrong. It needs to be someting that acts as a pointer to a bit.
		typedef element_type&                            reference;         // This is not right. It needs to be someting that acts as a pointer to a bit.

	public:
		reference_type operator*() const;
		reference_type operator[](difference_type n) const;

		bitvector_iterator();
		bitvector_iterator(element_type* p, eastl_size_t i);
		bitvector_iterator(reference_type& referenceType);

		bitvector_iterator& operator++()    { base_type::operator++(); return *this; }
		bitvector_iterator& operator--()    { base_type::operator--(); return *this; }
		bitvector_iterator  operator++(int);
		bitvector_iterator  operator--(int);

		bitvector_iterator& operator+=(difference_type dist) { base_type::operator+=(dist); return *this; }
		bitvector_iterator& operator-=(difference_type dist) { base_type::operator-=(dist); return *this; }
		bitvector_iterator  operator+ (difference_type dist) const;
		bitvector_iterator  operator- (difference_type dist) const;

		// We need this here because we are overloading operator-, so for some reason the
		// other overload of the function can't be found unless it's explicitly specified.
		difference_type operator-(const base_type& rhs) const { return base_type::operator-(rhs); }
	};



	/// bitvector
	///
	/// Implements an array of bits treated as boolean values.
	/// bitvector is similar to vector<bool> but uses bits instead of bytes and 
	/// allows the user to use other containers such as deque instead of vector.
	/// bitvector is different from bitset in that bitset is less flexible but
	/// uses less memory and has higher performance.
	///
	/// To consider: Rename the Element template parameter to WordType, for 
	/// consistency with bitset.
	///
	template <typename Allocator = EASTLAllocatorType, 
			  typename Element   = BitvectorWordType, 
			  typename Container = eastl::vector<Element, Allocator> >
	class bitvector
	{
	public:
		typedef bitvector<Allocator, Element>               this_type;
		typedef bool                                        value_type;
		typedef bitvector_reference<Element>                reference;
		typedef bool                                        const_reference;
		typedef bitvector_iterator<Element>                 iterator;
		typedef bitvector_const_iterator<Element>           const_iterator;
		typedef eastl::reverse_iterator<iterator>           reverse_iterator;
		typedef eastl::reverse_iterator<const_iterator>     const_reverse_iterator;
		typedef Allocator                                   allocator_type;
		typedef Element                                     element_type;
		typedef Container                                   container_type;
		typedef eastl_size_t                                size_type;
		typedef ptrdiff_t                                   difference_type;

		#if defined(_MSC_VER) && (_MSC_VER >= 1400) && (_MSC_VER <= 1600) && !EASTL_STD_CPP_ONLY  // _MSC_VER of 1400 means VS2005, 1600 means VS2010. VS2012 generates errors with usage of enum:size_type.
			enum : size_type {                      // Use Microsoft enum language extension, allowing for smaller debug symbols than using a static const. Users have been affected by this.
				npos     = container_type::npos,
				kMaxSize = container_type::kMaxSize
			};
		#else
			static const size_type npos     = container_type::npos;      /// 'npos' means non-valid position or simply non-position.
			static const size_type kMaxSize = container_type::kMaxSize;  /// -1 is reserved for 'npos'. It also happens to be slightly beneficial that kMaxSize is a value less than -1, as it helps us deal with potential integer wraparound issues.
		#endif

		enum
		{
			kBitCount = 8 * sizeof(Element)
		};

	protected:
		container_type mContainer;
		size_type      mFreeBitCount;      // Unused bits in the last word of mContainer.

	public:
		bitvector();
		explicit bitvector(const allocator_type& allocator);
		explicit bitvector(size_type n, const allocator_type& allocator = EASTL_BITVECTOR_DEFAULT_ALLOCATOR);
		bitvector(size_type n, value_type value, const allocator_type& allocator = EASTL_BITVECTOR_DEFAULT_ALLOCATOR);
		bitvector(const bitvector& copy);

		template <typename InputIterator>
		bitvector(InputIterator first, InputIterator last);

		bitvector& operator=(const bitvector& x);
		void swap(this_type& x);

		template <typename InputIterator>
		void assign(InputIterator first, InputIterator last);

		iterator       begin() EA_NOEXCEPT;
		const_iterator begin() const EA_NOEXCEPT;
		const_iterator cbegin() const EA_NOEXCEPT;

		iterator       end() EA_NOEXCEPT;
		const_iterator end() const EA_NOEXCEPT;
		const_iterator cend() const EA_NOEXCEPT;

		reverse_iterator       rbegin() EA_NOEXCEPT;
		const_reverse_iterator rbegin() const EA_NOEXCEPT;
		const_reverse_iterator crbegin() const EA_NOEXCEPT;

		reverse_iterator       rend() EA_NOEXCEPT;
		const_reverse_iterator rend() const EA_NOEXCEPT;
		const_reverse_iterator crend() const EA_NOEXCEPT;

		bool      empty() const EA_NOEXCEPT;
		size_type size() const EA_NOEXCEPT;
		size_type capacity() const EA_NOEXCEPT;

		void resize(size_type n, value_type value);
		void resize(size_type n);
		void reserve(size_type n);
		void set_capacity(size_type n = npos);                  // Revises the capacity to the user-specified value. Resizes the container to match the capacity if the requested capacity n is less than the current size. If n == npos then the capacity is reallocated (if necessary) such that capacity == size.

		void push_back();
		void push_back(value_type value);
		void pop_back();

		reference       front();
		const_reference front() const;
		reference       back();
		const_reference back() const;

		bool            test(size_type n, bool defaultValue) const; // Returns true if the bit index is < size() and set. Returns defaultValue if the bit is >= size().
		void            set(size_type n, bool value);               // Resizes the container to accomodate n if necessary. 

		reference       at(size_type n);                    // throws an out_of_range exception if n is invalid.
		const_reference at(size_type n) const;

		reference       operator[](size_type n);            // behavior is undefined if n is invalid.
		const_reference operator[](size_type n) const;

		/*
		Work in progress:
		template <bool value = true> iterator find_first();                                 // Finds the lowest "on" bit.
		template <bool value = true> iterator find_next(const_iterator it);                 // Finds the next lowest "on" bit after it.
		template <bool value = true> iterator find_last();                                  // Finds the index of the last "on" bit, returns size if none are set.
		template <bool value = true> iterator find_prev(const_iterator it);                 // Finds the index of the last "on" bit before last_find, returns size if none are set.

		template <bool value = true> const_iterator find_first() const;                     // Finds the lowest "on" bit.
		template <bool value = true> const_iterator find_next(const_iterator it) const;     // Finds the next lowest "on" bit after it.
		template <bool value = true> const_iterator find_last() const;                      // Finds the index of the last "on" bit, returns size if none are set.
		template <bool value = true> const_iterator find_prev(const_iterator it) const;     // Finds the index of the last "on" bit before last_find, returns size if none are set.
		*/

		element_type*       data() EA_NOEXCEPT;
		const element_type* data() const EA_NOEXCEPT;
		
		iterator insert(const_iterator position, value_type value);
		void     insert(const_iterator position, size_type n, value_type value);

		// template <typename InputIterator> Not yet implemented. See below for disabled definition.
		// void insert(const_iterator position, InputIterator first, InputIterator last);

		iterator erase(const_iterator position);
		iterator erase(const_iterator first, const_iterator last);

		reverse_iterator erase(const_reverse_iterator position);
		reverse_iterator erase(const_reverse_iterator first, const_reverse_iterator last);

		void clear();
		void reset_lose_memory(); // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.

		container_type&       get_container();
		const container_type& get_container() const;

		bool validate() const;
		int  validate_iterator(const_iterator i) const;
	};




	///////////////////////////////////////////////////////////////////////
	// bitvector_reference
	///////////////////////////////////////////////////////////////////////

	template <typename Element>
	bitvector_reference<Element>::bitvector_reference(Element* p, eastl_size_t i)
	  : mpBitWord(p), 
		mnBitIndex(i)
	{
	}


	template <typename Element>
	bitvector_reference<Element>&
	bitvector_reference<Element>::operator=(bool value)
	{
		const Element mask = (Element)(Element(1) << mnBitIndex);

		if(value)
			*mpBitWord |= mask;
		else
			*mpBitWord &= ~mask;

		return *this;
	}


	template <typename Element>
	bitvector_reference<Element>&
	bitvector_reference<Element>::operator=(const bitvector_reference& rhs)
	{
		return (*this = (bool)rhs);
	}


	template <typename Element>
	void bitvector_reference<Element>::CopyFrom(const bitvector_reference& rhs)
	{
		mpBitWord  = rhs.mpBitWord;
		mnBitIndex = rhs.mnBitIndex;
	}




	///////////////////////////////////////////////////////////////////////
	// bitvector_const_iterator
	///////////////////////////////////////////////////////////////////////

	template <typename Element>
	bitvector_const_iterator<Element>::bitvector_const_iterator()
		: mReference(0, 0)
	{
	}


	template <typename Element>
	bitvector_const_iterator<Element>::bitvector_const_iterator(const Element* p, eastl_size_t i)
		: mReference(const_cast<Element*>(p), i) // const_cast is safe here because we never let mReference leak and we don't modify it.
	{
	}


	template <typename Element>
	bitvector_const_iterator<Element>::bitvector_const_iterator(const reference_type& reference)
		: mReference(reference)
	{
	}


	template <typename Element>
	bitvector_const_iterator<Element>&
	bitvector_const_iterator<Element>::operator++()
	{
		++mReference.mnBitIndex;

		if(mReference.mnBitIndex == kBitCount)
		{
			++mReference.mpBitWord;
			mReference.mnBitIndex = 0;
		}

		return *this;
	}


	template <typename Element>
	bitvector_const_iterator<Element>&
	bitvector_const_iterator<Element>::operator--()
	{
		if(mReference.mnBitIndex == 0)
		{
			--mReference.mpBitWord;
			mReference.mnBitIndex = kBitCount;
		}

		--mReference.mnBitIndex;
		return *this;
	}


	template <typename Element>
	bitvector_const_iterator<Element>
	bitvector_const_iterator<Element>::operator++(int)
	{
		bitvector_const_iterator copy(*this);
		++*this;
		return copy;
	}


	template <typename Element>
	bitvector_const_iterator<Element>
	bitvector_const_iterator<Element>::operator--(int)
	{
		bitvector_const_iterator copy(*this);
		--*this;
		return copy;
	}


	template <typename Element>
	bitvector_const_iterator<Element>&
	bitvector_const_iterator<Element>::operator+=(difference_type n)
	{
		n += mReference.mnBitIndex;

		if(n >= difference_type(0))
		{
			mReference.mpBitWord  += n / kBitCount;
			mReference.mnBitIndex  = (size_type)(n % kBitCount);
		}
		else
		{
			// backwards is tricky
			// figure out how many full words backwards we need to move
			// n = [-1..-32] => 1
			// n = [-33..-64] => 2
			const size_type backwards = (size_type)(-n + kBitCount - 1);
			mReference.mpBitWord -= backwards / kBitCount;

			// -1 => 31; backwards = 32; 31 - (backwards % 32) = 31
			// -2 => 30; backwards = 33; 31 - (backwards % 32) = 30
			// -3 => 29; backwards = 34
			// ..
			// -32 => 0; backwards = 63; 31 - (backwards % 32) = 0
			// -33 => 31; backwards = 64; 31 - (backwards % 32) = 31
			mReference.mnBitIndex = (kBitCount - 1) - (backwards % kBitCount);
		}

		return *this;
	}


	template <typename Element>
	bitvector_const_iterator<Element>&
	bitvector_const_iterator<Element>::operator-=(difference_type n)
	{
		return (*this += -n);
	}


	template <typename Element>
	bitvector_const_iterator<Element>
	bitvector_const_iterator<Element>::operator+(difference_type n) const
	{
		bitvector_const_iterator copy(*this);
		copy += n;
		return copy;
	}

	
	template <typename Element>
	bitvector_const_iterator<Element>
	bitvector_const_iterator<Element>::operator-(difference_type n) const
	{
		bitvector_const_iterator copy(*this);
		copy -= n;
		return copy;
	}


	template <typename Element>
	typename bitvector_const_iterator<Element>::difference_type
	bitvector_const_iterator<Element>::operator-(const this_type& rhs) const
	{
		return ((mReference.mpBitWord - rhs.mReference.mpBitWord) * kBitCount) + mReference.mnBitIndex - rhs.mReference.mnBitIndex;
	}


	template <typename Element>
	bool bitvector_const_iterator<Element>::operator==(const this_type& rhs) const
	{
		return (mReference.mpBitWord == rhs.mReference.mpBitWord) && (mReference.mnBitIndex == rhs.mReference.mnBitIndex);
	}


	template <typename Element>
	bool bitvector_const_iterator<Element>::operator!=(const this_type& rhs) const
	{
		return !(*this == rhs);
	}


	template <typename Element>
	bool bitvector_const_iterator<Element>::operator<(const this_type& rhs) const
	{
		return (mReference.mpBitWord < rhs.mReference.mpBitWord) || 
			   ((mReference.mpBitWord == rhs.mReference.mpBitWord) && (mReference.mnBitIndex < rhs.mReference.mnBitIndex));
	}


	template <typename Element>
	bool bitvector_const_iterator<Element>::operator<=(const this_type& rhs) const
	{
		return (mReference.mpBitWord < rhs.mReference.mpBitWord) || 
			   ((mReference.mpBitWord == rhs.mReference.mpBitWord) && (mReference.mnBitIndex <= rhs.mReference.mnBitIndex));
	}


	template <typename Element>
	bool bitvector_const_iterator<Element>::operator>(const this_type& rhs) const
	{
		return !(*this <= rhs);
	}


	template <typename Element>
	bool bitvector_const_iterator<Element>::operator>=(const this_type& rhs) const
	{
		return !(*this < rhs);
	}


	template <typename Element>
	bool bitvector_const_iterator<Element>::operator*() const
	{
		return mReference;
	}


	template <typename Element>
	bool bitvector_const_iterator<Element>::operator[](difference_type n) const
	{
		return *(*this + n);
	}


	template <typename Element>
	bitvector_const_iterator<Element>& bitvector_const_iterator<Element>::operator= (const this_type& rhs)
	{
		mReference.CopyFrom(rhs.mReference);
		return *this;
	}


	template <typename Element>
	int bitvector_const_iterator<Element>::validate(const Element* pStart, const Element* pEnd, eastl_size_t nExtraBits) const
	{
		const Element* const pCurrent = mReference.mpBitWord;

		if(pCurrent >= pStart)
		{
			if(nExtraBits == 0)
			{
				if(pCurrent == pEnd && mReference)
					return eastl::isf_valid | eastl::isf_current;
				else if(pCurrent < pEnd)
					return eastl::isf_valid | eastl::isf_current | eastl::isf_can_dereference;
			}
			else if(pCurrent == (pEnd - 1))
			{
				const size_type bit     = mReference.mnBitIndex;
				const size_type lastbit = kBitCount - nExtraBits;
				
				if(bit == lastbit)
					return eastl::isf_valid | eastl::isf_current;
				else if(bit < lastbit)
					return eastl::isf_valid | eastl::isf_current | eastl::isf_can_dereference;
			}
			else if(pCurrent < pEnd)
			{
				return eastl::isf_valid | eastl::isf_current | eastl::isf_can_dereference;
			}
		}

		return eastl::isf_none;
	}



	///////////////////////////////////////////////////////////////////////
	// bitvector_iterator
	///////////////////////////////////////////////////////////////////////

	template <typename Element>
	bitvector_iterator<Element>::bitvector_iterator()
		: base_type()
	{
	}

	template <typename Element>
	bitvector_iterator<Element>::bitvector_iterator(Element* p, eastl_size_t i)
		: base_type(p, i)
	{
	}


	template <typename Element>
	bitvector_iterator<Element>::bitvector_iterator(reference_type& reference)
		: base_type(reference)
	{
	}


	template <typename Element>
	typename bitvector_iterator<Element>::reference_type
	bitvector_iterator<Element>::operator*() const
	{
		return base_type::mReference;
	}


	template <typename Element>
	typename bitvector_iterator<Element>::reference_type
	bitvector_iterator<Element>::operator[](difference_type n) const
	{
		return *(*this + n);
	}


	template <typename Element>
	void MoveBits(bitvector_iterator<Element> start, 
				  bitvector_iterator<Element> end, 
				  bitvector_iterator<Element> dest)
	{
		// Slow implemenation; could optimize by moving a word at a time.
		if(dest <= start)
		{
			while(start != end)
			{
				*dest = *start;
				++dest;
				++start;
			}
		}
		else
		{
			// Need to move backwards
			dest += (end - start);

			while(start != end)
			{
				--dest;
				--end;
				*dest = *end;
			}
		}
	}


	template <typename Element>
	bitvector_iterator<Element>
	bitvector_iterator<Element>::operator++(int)
	{
		bitvector_iterator copy(*this);
		++*this;
		return copy;
	}


	template <typename Element>
	bitvector_iterator<Element>
	bitvector_iterator<Element>::operator--(int)
	{
		bitvector_iterator copy(*this);
		--*this;
		return copy;
	}


	template <typename Element>
	bitvector_iterator<Element>
	bitvector_iterator<Element>::operator+(difference_type n) const
	{
		bitvector_iterator copy(*this);
		copy += n;
		return copy;
	}

	
	template <typename Element>
	bitvector_iterator<Element>
	bitvector_iterator<Element>::operator-(difference_type n) const
	{
		bitvector_iterator copy(*this);
		copy -= n;
		return copy;
	}




	///////////////////////////////////////////////////////////////////////
	// bitvector
	///////////////////////////////////////////////////////////////////////

	template <typename Allocator, typename Element, typename Container>
	template <typename InputIterator>
	void bitvector<Allocator, Element, Container>::assign(InputIterator first, InputIterator last)
	{
		// To consider: We can maybe specialize this on bitvector_iterator to do a fast bitwise copy.
		// We can also specialize for random access iterators to figure out the size & reserve first.

		clear();

		while(first != last)
		{
			push_back(*first);
			++first;
		}
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::iterator
	bitvector<Allocator, Element, Container>::begin() EA_NOEXCEPT
	{
		return iterator(mContainer.begin(), 0);
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::const_iterator
	bitvector<Allocator, Element, Container>::begin() const EA_NOEXCEPT
	{
		return const_iterator(mContainer.begin(), 0);
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::const_iterator
	bitvector<Allocator, Element, Container>::cbegin() const EA_NOEXCEPT
	{
		return const_iterator(mContainer.begin(), 0);
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::iterator
	bitvector<Allocator, Element, Container>::end() EA_NOEXCEPT
	{
		return iterator(mContainer.end(), 0) - mFreeBitCount;
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::const_iterator
	bitvector<Allocator, Element, Container>::end() const EA_NOEXCEPT
	{
		return const_iterator(mContainer.end(), 0) - mFreeBitCount;
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::const_iterator
	bitvector<Allocator, Element, Container>::cend() const EA_NOEXCEPT
	{
		return const_iterator(mContainer.end(), 0) - mFreeBitCount;
	}


	template <typename Allocator, typename Element, typename Container>
	bool bitvector<Allocator, Element, Container>::empty() const EA_NOEXCEPT
	{
		return mContainer.empty();
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::size_type
	bitvector<Allocator, Element, Container>::size() const EA_NOEXCEPT
	{
		return (mContainer.size() * kBitCount) - mFreeBitCount;
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::size_type
	bitvector<Allocator, Element, Container>::capacity() const EA_NOEXCEPT
	{
		return mContainer.capacity() * kBitCount;
	}


	template <typename Allocator, typename Element, typename Container>
	void bitvector<Allocator, Element, Container>::set_capacity(size_type n)
	{
		if(n == npos)
			mContainer.set_capacity(npos);
		else
			mContainer.set_capacity((n + kBitCount - 1) / kBitCount);
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::reverse_iterator
	bitvector<Allocator, Element, Container>::rbegin() EA_NOEXCEPT
	{
		return reverse_iterator(end());
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::const_reverse_iterator
	bitvector<Allocator, Element, Container>::rbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(end());
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::const_reverse_iterator
	bitvector<Allocator, Element, Container>::crbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(end());
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::reverse_iterator
	bitvector<Allocator, Element, Container>::rend() EA_NOEXCEPT
	{
		return reverse_iterator(begin());
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::const_reverse_iterator
	bitvector<Allocator, Element, Container>::rend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(begin());
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::const_reverse_iterator
	bitvector<Allocator, Element, Container>::crend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(begin());
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::reference
	bitvector<Allocator, Element, Container>::front()
	{
		EASTL_ASSERT(!empty());
		return reference(&mContainer[0], 0);
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::const_reference
	bitvector<Allocator, Element, Container>::front() const
	{
		EASTL_ASSERT(!empty());

		// To consider: make a better solution to this than const_cast.
		return reference(const_cast<Element*>(&mContainer[0]), 0);
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::reference
	bitvector<Allocator, Element, Container>::back()
	{
		EASTL_ASSERT(!empty());
		return *(--end());
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::const_reference
	bitvector<Allocator, Element, Container>::back() const
	{
		EASTL_ASSERT(!empty());
		return *(--end());
	}


	template <typename Allocator, typename Element, typename Container>
	void bitvector<Allocator, Element, Container>::push_back()
	{
		if(!mFreeBitCount)
		{
			mContainer.push_back();
			mFreeBitCount = kBitCount;
		}

		--mFreeBitCount;
	}


	template <typename Allocator, typename Element, typename Container>
	void bitvector<Allocator, Element, Container>::push_back(value_type value)
	{
		push_back();
		*--end() = value;
	}
	

	template <typename Allocator, typename Element, typename Container>
	void bitvector<Allocator, Element, Container>::pop_back()
	{
		EASTL_ASSERT(!empty());

		if(++mFreeBitCount == kBitCount)
		{
			mContainer.pop_back();
			mFreeBitCount = 0;
		}
	}


	template <typename Allocator, typename Element, typename Container>
	void bitvector<Allocator, Element, Container>::reserve(size_type n)
	{
		const size_type wordCount = (n + kBitCount - 1) / kBitCount;
		mContainer.reserve(wordCount);
	}


	template <typename Allocator, typename Element, typename Container>
	void bitvector<Allocator, Element, Container>::resize(size_type n)
	{
		const size_type wordCount = (n + kBitCount - 1) / kBitCount;
		const size_type extra     = (wordCount * kBitCount) - n;

		mContainer.resize(wordCount);
		mFreeBitCount = extra;
	}


	template <typename Allocator, typename Element, typename Container>
	void bitvector<Allocator, Element, Container>::resize(size_type n, value_type value)
	{
		const size_type s = size();
		if(n < s)
			resize(n);

		// Fill up to the end of a word
		size_type newbits = n - s;

		while(mFreeBitCount && newbits)
		{
			push_back(value);
			--newbits;
		}

		// Fill the rest a word at a time
		if(newbits)
		{
			element_type element(0);
			if(value)
				element = ~element;

			const size_type words = (n + kBitCount - 1) / kBitCount;
			const size_type extra = words * kBitCount - n;
			mContainer.resize(words, element);
			mFreeBitCount = extra;
		}
	}


	template <typename Allocator, typename Element, typename Container>
	bool bitvector<Allocator, Element, Container>::test(size_type n, bool defaultValue) const
	{
		if(n < size())
			return *(begin() + (difference_type)n);

		return defaultValue;
	}


	template <typename Allocator, typename Element, typename Container>
	void bitvector<Allocator, Element, Container>::set(size_type n, bool value)
	{
		if(EASTL_UNLIKELY(n >= size()))
			resize(n + 1);

		*(begin() + (difference_type)n) = value;
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::reference
	bitvector<Allocator, Element, Container>::at(size_type n)
	{
		// The difference between at and operator[] is that at signals 
		// if the requested position is out of range by throwing an 
		// out_of_range exception.

		#if EASTL_EXCEPTIONS_ENABLED
			if(EASTL_UNLIKELY(n >= size()))
				throw std::out_of_range("bitvector::at -- out of range");
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(n >= size()))
				EASTL_FAIL_MSG("bitvector::at -- out of range");
		#endif
		
		return *(begin() + (difference_type)n);
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::const_reference
	bitvector<Allocator, Element, Container>::at(size_type n) const
	{
		#if EASTL_EXCEPTIONS_ENABLED
			if(EASTL_UNLIKELY(n >= size()))
				throw std::out_of_range("bitvector::at -- out of range");
		#elif EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(n >= size()))
				EASTL_FAIL_MSG("bitvector::at -- out of range");
		#endif
		
		return *(begin() + (difference_type)n);
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::reference
	bitvector<Allocator, Element, Container>::operator[](size_type n)
	{
		return *(begin() + (difference_type)n);
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::const_reference
	bitvector<Allocator, Element, Container>::operator[](size_type n) const
	{
		return *(begin() + (difference_type)n);
	}


/*
	template <typename Allocator, typename Element, typename Container>
	template <bool value>
	typename bitvector<Allocator, Element, Container>::iterator 
	bitvector<Allocator, Element, Container>::find_first()
	{
		return begin();
	}

	template <bool value> iterator find_next(const_iterator it);
	template <bool value> iterator find_last();
	template <bool value> iterator find_prev(const_iterator it);
						
	template <bool value> const_iterator find_first() const;
	template <bool value> const_iterator find_next(const_iterator it) const;
	template <bool value> const_iterator find_last() const;
	template <bool value> const_iterator find_prev(const_iterator it) const;
*/




	template <typename Allocator, typename Element, typename Container>
	inline typename bitvector<Allocator, Element, Container>::container_type&
	bitvector<Allocator, Element, Container>::get_container()
	{
		return mContainer;
	}


	template <typename Allocator, typename Element, typename Container>
	inline const typename bitvector<Allocator, Element, Container>::container_type&
	bitvector<Allocator, Element, Container>::get_container() const
	{
		return mContainer;
	}


	template <typename Allocator, typename Element, typename Container>
	bool bitvector<Allocator, Element, Container>::validate() const
	{
		if(!mContainer.validate())
			return false;

		if((unsigned)mFreeBitCount >= kBitCount)
			return false;

		return true;
	}


	template <typename Allocator, typename Element, typename Container>
	int bitvector<Allocator, Element, Container>::validate_iterator(const_iterator i) const
	{
		return i.validate(mContainer.begin(), mContainer.end(), mFreeBitCount);
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::element_type*
	bitvector<Allocator, Element, Container>::data() EA_NOEXCEPT
	{
		return mContainer.data();
	}
	

	template <typename Allocator, typename Element, typename Container>
	const typename bitvector<Allocator, Element, Container>::element_type*
	bitvector<Allocator, Element, Container>::data() const EA_NOEXCEPT
	{
		return mContainer.data();
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::iterator
	bitvector<Allocator, Element, Container>::insert(const_iterator position, value_type value)
	{
		iterator iPosition(position.get_reference_type()); // This is just a non-const version of position.

		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(validate_iterator(iPosition) & eastl::isf_valid) == 0)
				EASTL_FAIL_MSG("bitvector::insert -- invalid iterator");
		#endif

		// Save because we might reallocate
		const typename iterator::difference_type n = iPosition - begin();
		push_back();
		iPosition = begin() + n;

		MoveBits(iPosition, --end(), ++iterator(iPosition));
		*iPosition = value;

		return iPosition;
	}


	template <typename Allocator, typename Element, typename Container>
	void bitvector<Allocator, Element, Container>::insert(const_iterator position, size_type n, value_type value)
	{
		iterator iPosition(position.get_reference_type()); // This is just a non-const version of position.

		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(validate_iterator(iPosition) & eastl::isf_valid) == 0)
				EASTL_FAIL_MSG("bitvector::insert -- invalid iterator");
		#endif

		// Save because we might reallocate.
		const typename iterator::difference_type p = iPosition - begin();
		resize(size() + n);
		iPosition = begin() + p;

		iterator insert_end = iPosition + n;
		MoveBits(iPosition, end() - n, insert_end);

		// To do: Optimize this to word-at-a-time for large inserts
		while(iPosition != insert_end)
		{
			*iPosition = value;
			++iPosition;
		}
	}


	/*
	The following is a placeholder for a future implementation. It turns out that a correct implementation of 
	insert(pos, first, last) is a non-trivial exercise that would take a few hours to implement and test. 
	The reasons why involve primarily the problem of handling the case where insertion source comes from 
	within the container itself, and the case that first and last (note they are templated) might not refer 
	to iterators might refer to a value/count pair. The C++ Standard requires you to handle this case and 
	I (Paul Pedriana) believe that it applies even for a bitvector, given that bool is an integral type. 
	So you have to set up a compile-time type traits function chooser. See vector, for example.

	template <typename Allocator, typename Element, typename Container>
	template <typename InputIterator>
	void bitvector<Allocator, Element, Container>::insert(const_iterator position, InputIterator first, InputIterator last)
	{
		iterator iPosition(position.get_reference_type()); // This is just a non-const version of position.

		// This implementation is probably broken due to not handling insertion into self.
		// To do: Make a more efficient version of this.
		difference_type distance = (iPosition - begin());

		while(first != last)
		{
			insert(iPosition, *first);
			iPosition = begin() + ++distance;
			++first;
		}
	}
	*/


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::iterator
	bitvector<Allocator, Element, Container>::erase(const_iterator position)
	{
		iterator iPosition(position.get_reference_type()); // This is just a non-const version of position.

		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(validate_iterator(iPosition) & eastl::isf_can_dereference) == 0)
				EASTL_FAIL_MSG("bitvector::erase -- invalid iterator");
		#endif

		MoveBits(++iterator(iPosition), end(), iPosition);
		resize(size() - 1);

		// Verify that no reallocation occurred.
		EASTL_ASSERT(validate_iterator(iPosition) & eastl::isf_valid);
		return iPosition;
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::iterator
	bitvector<Allocator, Element, Container>::erase(const_iterator first, const_iterator last)
	{
		iterator iFirst(first.get_reference_type()); // This is just a non-const version of first.
		iterator iLast(last.get_reference_type());   // This is just a non-const version of last.

		#if EASTL_ASSERT_ENABLED
			if(EASTL_UNLIKELY(validate_iterator(iLast) & eastl::isf_valid) == 0)
				EASTL_FAIL_MSG("bitvector::erase -- invalid iterator");
		#endif

		if(!(iFirst == iLast))
		{
			#if EASTL_ASSERT_ENABLED
				if(EASTL_UNLIKELY(validate_iterator(iFirst) & eastl::isf_can_dereference) == 0)
					EASTL_FAIL_MSG("bitvector::erase -- invalid iterator");
			#endif

			const size_type eraseCount = (size_type)(iLast - iFirst);
			MoveBits(iLast, end(), iFirst);
			resize(size() - eraseCount);

			// Verify that no reallocation occurred.
			#if EASTL_ASSERT_ENABLED
				if(EASTL_UNLIKELY(validate_iterator(iFirst) & eastl::isf_valid) == 0)
					EASTL_FAIL_MSG("bitvector::erase -- invalid iterator");
			#endif
		}

		return iFirst;
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::reverse_iterator
	bitvector<Allocator, Element, Container>::erase(const_reverse_iterator position)
	{
		return reverse_iterator(erase((++position).base()));
	}


	template <typename Allocator, typename Element, typename Container>
	typename bitvector<Allocator, Element, Container>::reverse_iterator
	bitvector<Allocator, Element, Container>::erase(const_reverse_iterator first, const_reverse_iterator last)
	{
		// Version which erases in order from first to last.
		// difference_type i(first.base() - last.base());
		// while(i--)
		//     first = erase(first);
		// return first;

		// Version which erases in order from last to first, but is slightly more efficient:
		return reverse_iterator(erase(last.base(), first.base()));
	}


	template <typename Allocator, typename Element, typename Container>
	void bitvector<Allocator, Element, Container>::swap(this_type& rhs)
	{
		mContainer.swap(rhs.mContainer);
		eastl::swap(mFreeBitCount, rhs.mFreeBitCount);
	}


	template <typename Allocator, typename Element, typename Container>
	void bitvector<Allocator, Element, Container>::reset_lose_memory()
	{
		mContainer.reset_lose_memory(); // intentional memory leak.
		mFreeBitCount = 0;
	}


	template <typename Allocator, typename Element, typename Container>
	void bitvector<Allocator, Element, Container>::clear()
	{
		mContainer.clear();
		mFreeBitCount = 0;
	}


	template <typename Allocator, typename Element, typename Container>
	bitvector<Allocator, Element, Container>&
	bitvector<Allocator, Element, Container>::operator=(const bitvector& rhs)
	{
		// The following is OK if (&rhs == this)
		mContainer = rhs.mContainer;
		mFreeBitCount = rhs.mFreeBitCount;

		return *this;
	}


	template <typename Allocator, typename Element, typename Container>
	bitvector<Allocator, Element, Container>::bitvector()
	  : mContainer(), 
		mFreeBitCount(0)
	{
	}


	template <typename Allocator, typename Element, typename Container>
	bitvector<Allocator, Element, Container>::bitvector(const allocator_type& allocator)
	  : mContainer(allocator), 
		mFreeBitCount(0)
	{
	}


	template <typename Allocator, typename Element, typename Container>
	bitvector<Allocator, Element, Container>::bitvector(size_type n, const allocator_type& allocator)
	   : mContainer((n + kBitCount - 1) / kBitCount, allocator)
	{
		mFreeBitCount = kBitCount - (n % kBitCount);

		if(mFreeBitCount == kBitCount)
			mFreeBitCount = 0;
	}


	template <typename Allocator, typename Element, typename Container>
	bitvector<Allocator, Element, Container>::bitvector(size_type n, value_type value, const allocator_type& allocator)
	  : mContainer((n + kBitCount - 1) / kBitCount, value ? ~element_type(0) : element_type(0), allocator)
	{
		mFreeBitCount = kBitCount - (n % kBitCount);

		if(mFreeBitCount == kBitCount)
			mFreeBitCount = 0;
	}


	template <typename Allocator, typename Element, typename Container>
	bitvector<Allocator, Element, Container>::bitvector(const bitvector& copy)
	  : mContainer(copy.mContainer), 
		mFreeBitCount(copy.mFreeBitCount)
	{
	}


	template <typename Allocator, typename Element, typename Container>
	template <typename InputIterator>
	bitvector<Allocator, Element, Container>::bitvector(InputIterator first, InputIterator last)
	  : mContainer(), 
		mFreeBitCount(0)
	{
		assign(first, last);
	}



	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename Allocator, typename Element, typename Container>
	inline bool operator==(const bitvector<Allocator, Element, Container>& a, 
						   const bitvector<Allocator, Element, Container>& b)
	{
		// To do: Replace this with a smart compare implementation. This is much slower than it needs to be.
		return ((a.size() == b.size()) && eastl::equal(a.begin(), a.end(), b.begin()));
	}


	template <typename Allocator, typename Element, typename Container>
	inline bool operator!=(const bitvector<Allocator, Element, Container>& a, 
						   const bitvector<Allocator, Element, Container>& b)
	{
		return !operator==(a, b);
	}


	template <typename Allocator, typename Element, typename Container>
	inline bool operator<(const bitvector<Allocator, Element, Container>& a, 
						  const bitvector<Allocator, Element, Container>& b)
	{
		// To do: Replace this with a smart compare implementation. This is much slower than it needs to be.
		return eastl::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
	}


	template <typename Allocator, typename Element, typename Container>
	inline bool operator>(const bitvector<Allocator, Element, Container>& a, 
						  const bitvector<Allocator, Element, Container>& b)
	{
		return b < a;
	}


	template <typename Allocator, typename Element, typename Container>
	inline bool operator<=(const bitvector<Allocator, Element, Container>& a, 
						   const bitvector<Allocator, Element, Container>& b)
	{
		return !(b < a);
	}


	template <typename Allocator, typename Element, typename Container>
	inline bool operator>=(const bitvector<Allocator, Element, Container>& a, 
						   const bitvector<Allocator, Element, Container>& b)
	{
		return !(a < b);
	}

	template <typename Allocator, typename Element, typename Container>
	inline void swap(bitvector<Allocator, Element, Container>& a,
					 bitvector<Allocator, Element, Container>& b)
	{
		a.swap(b);
	}


} // namespace eastl


#ifdef _MSC_VER
	#pragma warning(pop)
#endif


#endif // Header include guard












