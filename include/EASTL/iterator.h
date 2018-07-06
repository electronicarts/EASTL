///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ITERATOR_H
#define EASTL_ITERATOR_H


#include <EASTL/internal/config.h>
#include <EASTL/internal/move_help.h>
#include <EASTL/initializer_list.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stddef.h>

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

// If the user has specified that we use std iterator
// categories instead of EASTL iterator categories,
// then #include <iterator>.
#if EASTL_STD_ITERATOR_CATEGORY_ENABLED
	#ifdef _MSC_VER
		#pragma warning(push, 0)
	#endif
	#include <iterator>                 
	#ifdef _MSC_VER
		#pragma warning(pop)
	#endif
#endif                                  


#ifdef _MSC_VER
	#pragma warning(push)           // VC++ generates a bogus warning that you cannot code away.
	#pragma warning(disable: 4619)  // There is no warning number 'number'.
	#pragma warning(disable: 4217)  // Member template functions cannot be used for copy-assignment or copy-construction.
#endif

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{
	/// iterator_status_flag
	/// 
	/// Defines the validity status of an iterator. This is primarily used for 
	/// iterator validation in debug builds. These are implemented as OR-able 
	/// flags (as opposed to mutually exclusive values) in order to deal with 
	/// the nature of iterator status. In particular, an iterator may be valid
	/// but not dereferencable, as in the case with an iterator to container end().
	/// An iterator may be valid but also dereferencable, as in the case with an
	/// iterator to container begin().
	///
	enum iterator_status_flag
	{
		isf_none            = 0x00, /// This is called none and not called invalid because it is not strictly the opposite of invalid.
		isf_valid           = 0x01, /// The iterator is valid, which means it is in the range of [begin, end].
		isf_current         = 0x02, /// The iterator is valid and points to the same element it did when created. For example, if an iterator points to vector::begin() but an element is inserted at the front, the iterator is valid but not current. Modification of elements in place do not make iterators non-current.
		isf_can_dereference = 0x04  /// The iterator is dereferencable, which means it is in the range of [begin, end). It may or may not be current.
	};



	// The following declarations are taken directly from the C++ standard document.
	//    input_iterator_tag, etc.
	//    iterator
	//    iterator_traits
	//    reverse_iterator

	// Iterator categories
	// Every iterator is defined as belonging to one of the iterator categories that
	// we define here. These categories come directly from the C++ standard.
	#if !EASTL_STD_ITERATOR_CATEGORY_ENABLED // If we are to use our own iterator category definitions...
		struct input_iterator_tag { };
		struct output_iterator_tag { };
		struct forward_iterator_tag       : public input_iterator_tag { };
		struct bidirectional_iterator_tag : public forward_iterator_tag { };
		struct random_access_iterator_tag : public bidirectional_iterator_tag { };
		struct contiguous_iterator_tag    : public random_access_iterator_tag { };  // Extension to the C++ standard. Contiguous ranges are more than random access, they are physically contiguous.
	#endif


	// struct iterator
	template <typename Category, typename T, typename Distance = ptrdiff_t, 
			  typename Pointer = T*, typename Reference = T&>
	struct iterator
	{
		typedef Category  iterator_category;
		typedef T         value_type;
		typedef Distance  difference_type;
		typedef Pointer   pointer;
		typedef Reference reference;
	};


	// struct iterator_traits
	template <typename Iterator>
	struct iterator_traits
	{
		typedef typename Iterator::iterator_category iterator_category;
		typedef typename Iterator::value_type        value_type;
		typedef typename Iterator::difference_type   difference_type;
		typedef typename Iterator::pointer           pointer;
		typedef typename Iterator::reference         reference;
	};

	template <typename T>
	struct iterator_traits<T*>
	{
		typedef EASTL_ITC_NS::random_access_iterator_tag iterator_category;     // To consider: Change this to contiguous_iterator_tag for the case that 
		typedef T                                        value_type;            //              EASTL_ITC_NS is "eastl" instead of "std".
		typedef ptrdiff_t                                difference_type;
		typedef T*                                       pointer;
		typedef T&                                       reference;
	};

	template <typename T>
	struct iterator_traits<const T*>
	{
		typedef EASTL_ITC_NS::random_access_iterator_tag iterator_category;
		typedef T                                        value_type;
		typedef ptrdiff_t                                difference_type;
		typedef const T*                                 pointer;
		typedef const T&                                 reference;
	};




	/// is_iterator_wrapper
	///
	/// Tells if an Iterator type is a wrapper type as opposed to a regular type.
	/// Relies on the class declaring a typedef called wrapped_iterator_type.
	///
	/// Examples of wrapping iterators:
	///     reverse_iterator  
	///     generic_iterator  
	///     move_iterator     
	/// Examples of non-wrapping iterators:
	///     iterator
	///     list::iterator
	///     char*
	/// 
	/// Example behavior:
	///     is_iterator_wrapper(int*)::value                                              => false
	///     is_iterator_wrapper(eastl::array<char>*)::value                               => false
	///     is_iterator_wrapper(eastl::vector<int>::iterator)::value                      => false
	///     is_iterator_wrapper(eastl::generic_iterator<int*>)::value                     => true
	///     is_iterator_wrapper(eastl::move_iterator<eastl::array<int>::iterator>)::value => true
	///
	template<typename Iterator>
	class is_iterator_wrapper
	{
		template<typename>
		static eastl::no_type test(...);

		template<typename U>
		static eastl::yes_type test(typename U::wrapped_iterator_type*, typename eastl::enable_if<eastl::is_class<U>::value>::type* = 0);
	
	public:
		EA_DISABLE_VC_WARNING(6334)
		static const bool value = (sizeof(test<Iterator>(NULL)) == sizeof(eastl::yes_type));
		EA_RESTORE_VC_WARNING()
	};


	/// unwrap_iterator
	///
	/// Takes a wrapper Iterator (e.g. move_iterator, reverse_iterator, generic_iterator) instance 
	/// and returns the wrapped iterator type. If Iterator is not a wrapper (including being a pointer),
	/// or is not an iterator, then this function returns it as-is.
	/// unwrap_iterator unwraps only a single layer of iterator at a time. You need to call it twice, 
	/// for example, to unwrap two layers of iterators. 
	///
	/// Example usage:
	///     int* pInt             = unwrap_iterator(&pIntArray[15]);
	///     int* pInt             = unwrap_iterator(generic_iterator(&pIntArray[15]));
	///     MyVector::iterator it = unwrap_iterator(myVector.begin());
	///     MyVector::iterator it = unwrap_iterator(move_iterator(myVector.begin()));
	///
	template <typename Iterator, bool isWrapper>
	struct is_iterator_wrapper_helper
	{
		typedef Iterator iterator_type;

		static iterator_type get_base(Iterator it)
			{ return it; }
	};


	template <typename Iterator>
	struct is_iterator_wrapper_helper<Iterator, true>
	{
		typedef typename Iterator::iterator_type iterator_type;

		static iterator_type get_base(Iterator it)
			{ return it.base(); }
	};

	template <typename Iterator>
	inline typename is_iterator_wrapper_helper<Iterator, eastl::is_iterator_wrapper<Iterator>::value>::iterator_type unwrap_iterator(Iterator it)
		{ return eastl::is_iterator_wrapper_helper<Iterator, eastl::is_iterator_wrapper<Iterator>::value>::get_base(it); }



	/// reverse_iterator
	///
	/// From the C++ standard:
	/// Bidirectional and random access iterators have corresponding reverse 
	/// iterator adaptors that iterate through the data structure in the 
	/// opposite direction. They have the same signatures as the corresponding 
	/// iterators. The fundamental relation between a reverse iterator and its 
	/// corresponding iterator i is established by the identity:
	///     &*(reverse_iterator(i)) == &*(i - 1).
	/// This mapping is dictated by the fact that while there is always a pointer 
	/// past the end of an array, there might not be a valid pointer before the
	/// beginning of an array.
	///
	template <typename Iterator>
	class reverse_iterator : public iterator<typename eastl::iterator_traits<Iterator>::iterator_category,
											 typename eastl::iterator_traits<Iterator>::value_type,
											 typename eastl::iterator_traits<Iterator>::difference_type,
											 typename eastl::iterator_traits<Iterator>::pointer,
											 typename eastl::iterator_traits<Iterator>::reference>
	{
	public:
		typedef Iterator                                                   iterator_type;
		typedef iterator_type                                              wrapped_iterator_type;   // This is not in the C++ Standard; it's used by use to identify it as a wrapping iterator type.
		typedef typename eastl::iterator_traits<Iterator>::pointer         pointer;
		typedef typename eastl::iterator_traits<Iterator>::reference       reference;
		typedef typename eastl::iterator_traits<Iterator>::difference_type difference_type;

	protected:
		Iterator mIterator;

	public:
		reverse_iterator()      // It's important that we construct mIterator, because if Iterator  
			: mIterator() { }   // is a pointer, there's a difference between doing it and not.

		explicit reverse_iterator(iterator_type i)
			: mIterator(i) { }

		reverse_iterator(const reverse_iterator& ri)
			: mIterator(ri.mIterator) { }

		template <typename U>
		reverse_iterator(const reverse_iterator<U>& ri)
			: mIterator(ri.base()) { }

		// This operator= isn't in the standard, but the the C++ 
		// library working group has tentatively approved it, as it
		// allows const and non-const reverse_iterators to interoperate.
		template <typename U>
		reverse_iterator<Iterator>& operator=(const reverse_iterator<U>& ri)
			{ mIterator = ri.base(); return *this; }

		iterator_type base() const
			{ return mIterator; }

		reference operator*() const
		{
			iterator_type i(mIterator);
			return *--i;
		}

		pointer operator->() const
			{ return &(operator*()); }

		reverse_iterator& operator++()
			{ --mIterator; return *this; }

		reverse_iterator operator++(int)
		{
			reverse_iterator ri(*this);
			--mIterator;
			return ri;
		}

		reverse_iterator& operator--()
			{ ++mIterator; return *this; }

		reverse_iterator operator--(int)
		{
			reverse_iterator ri(*this);
			++mIterator;
			return ri;
		}

		reverse_iterator operator+(difference_type n) const
			{ return reverse_iterator(mIterator - n); }

		reverse_iterator& operator+=(difference_type n)
			{ mIterator -= n; return *this; }

		reverse_iterator operator-(difference_type n) const
			{ return reverse_iterator(mIterator + n); }

		reverse_iterator& operator-=(difference_type n)
			{ mIterator += n; return *this; }

		// http://cplusplus.github.io/LWG/lwg-defects.html#386, 
		// http://llvm.org/bugs/show_bug.cgi?id=17883 
		// random_access_iterator operator[] is merely required to return something convertible to reference. 
		// reverse_iterator operator[] can't necessarily know what to return as the underlying iterator 
		// operator[] may return something other than reference.
		// reference operator[](difference_type n) const
		//     { return mIterator[-n - 1]; }

		reference operator[](difference_type n) const
			{ return *(*this + n); }
	};


	// The C++ library working group has tentatively approved the usage of two
	// template parameters (Iterator1 and Iterator2) in order to allow reverse_iterators
	// and const_reverse iterators to be comparable. This is a similar issue to the 
	// C++ defect report #179 regarding comparison of container iterators and const_iterators.
	//
	// libstdc++ reports that std::relops breaks the usage of two iterator types and if we 
	// want to support relops then we need to also make versions of each of below with
	// a single template parameter to placate std::relops. But relops is hardly used due to 
	// the troubles it causes and so we are avoiding support here until somebody complains about it.
	template <typename Iterator1, typename Iterator2>
	inline bool
	operator==(const reverse_iterator<Iterator1>& a, const reverse_iterator<Iterator2>& b)
		{ return a.base() == b.base(); }


	template <typename Iterator1, typename Iterator2>
	inline bool
	operator<(const reverse_iterator<Iterator1>& a, const reverse_iterator<Iterator2>& b)
		{ return a.base() > b.base(); }


	template <typename Iterator1, typename Iterator2>
	inline bool
	operator!=(const reverse_iterator<Iterator1>& a, const reverse_iterator<Iterator2>& b)
		{ return a.base() != b.base(); }


	template <typename Iterator1, typename Iterator2>
	inline bool
	operator>(const reverse_iterator<Iterator1>& a, const reverse_iterator<Iterator2>& b)
		{ return a.base() < b.base(); }


	template <typename Iterator1, typename Iterator2>
	inline bool
	operator<=(const reverse_iterator<Iterator1>& a, const reverse_iterator<Iterator2>& b)
		{ return a.base() >= b.base(); }


	template <typename Iterator1, typename Iterator2>
	inline bool
	operator>=(const reverse_iterator<Iterator1>& a, const reverse_iterator<Iterator2>& b)
		{ return a.base() <= b.base(); }


	template <typename Iterator1, typename Iterator2>
	inline typename reverse_iterator<Iterator1>::difference_type
	operator-(const reverse_iterator<Iterator1>& a, const reverse_iterator<Iterator2>& b)
		{ return b.base() - a.base(); }


	template <typename Iterator>
	inline reverse_iterator<Iterator>
	operator+(typename reverse_iterator<Iterator>::difference_type n, const reverse_iterator<Iterator>& a)
		{ return reverse_iterator<Iterator>(a.base() - n); }


	/// is_reverse_iterator
	///
	/// This is a type traits extension utility.
	/// Given an iterator, tells if it's a reverse_iterator vs anything else.
	/// If it's a reverse iterator wrapped by another iterator then value is false.
	/// To consider: Detect that if it's a move_iterator<reverse_iterator> and unwrap 
	/// move_iterator so we can detect that underneath it's reverse_iterator.
	/// 
	template <typename T>
	struct is_reverse_iterator
		: public eastl::false_type {};

	template<typename Iterator>
	struct is_reverse_iterator< eastl::reverse_iterator<Iterator> >
		: public eastl::true_type {};



	/// unwrap_reverse_iterator
	///
	/// Returns Iterator::get_base() if it's a reverse_iterator, else returns Iterator as-is.
	///
	/// Example usage:
	///      vector<int> intVector;
	///      eastl::reverse_iterator<vector<int>::iterator> reverseIterator(intVector.begin());
	///      vector<int>::iterator it = unwrap_reverse_iterator(reverseIterator);
	///
	/// Disabled until there is considered a good use for it.
	/// template <typename Iterator>
	/// inline typename eastl::is_iterator_wrapper_helper<Iterator, eastl::is_reverse_iterator<Iterator>::value>::iterator_type unwrap_reverse_iterator(Iterator it)
	///     { return eastl::is_iterator_wrapper_helper<Iterator, eastl::is_reverse_iterator<Iterator>::value>::get_base(it); }



	#if EASTL_MOVE_SEMANTICS_ENABLED
		/// move_iterator
		///
		/// From the C++11 Standard, section 24.5.3.1:
		/// Class template move_iterator is an iterator adaptor with the same behavior as the underlying iterator
		/// except that its dereference operator implicitly converts the value returned by the underlying iterator's
		/// dereference operator to an rvalue reference. Some generic algorithms can be called with move iterators to
		/// replace copying with moving.

		template<typename Iterator>
		class move_iterator // Don't inherit from iterator.
		{
		public:
			typedef Iterator                                iterator_type;
			typedef iterator_type                           wrapped_iterator_type;   // This is not in the C++ Standard; it's used by use to identify it as a wrapping iterator type.
			typedef iterator_traits<Iterator>               traits_type;
			typedef typename traits_type::iterator_category iterator_category;
			typedef typename traits_type::value_type        value_type;
			typedef typename traits_type::difference_type   difference_type;
			typedef Iterator                                pointer;
			typedef value_type&&                            reference;

		protected:
			iterator_type mIterator;

		public:
			move_iterator()
			  : mIterator()
			{
			}

			explicit move_iterator(iterator_type mi)
			  : mIterator(mi) { }

			template<typename U>
			move_iterator(const move_iterator<U>& mi)
			  : mIterator(mi.base())
			{
			}

			iterator_type base() const
				{ return mIterator; }

			reference operator*() const
				{ return eastl::move(*mIterator); }

			pointer operator->() const
				{ return mIterator; }

			move_iterator& operator++()
			{
				++mIterator;
				return *this;
			}

			move_iterator operator++(int)
			{
				move_iterator tempMoveIterator = *this;
				++mIterator;
				return tempMoveIterator;
			}

			move_iterator& operator--()
			{
				--mIterator;
				return *this;
			}

			move_iterator operator--(int)
			{
				move_iterator tempMoveIterator = *this;
				--mIterator;
				return tempMoveIterator;
			}

			move_iterator operator+(difference_type n) const
				{ return move_iterator(mIterator + n); }

			move_iterator& operator+=(difference_type n)
			{
				mIterator += n;
				return *this;
			}

			move_iterator operator-(difference_type n) const
				{ return move_iterator(mIterator - n); }
	
			move_iterator& operator-=(difference_type n)
			{ 
				mIterator -= n;
				return *this;
			}

			reference operator[](difference_type n) const
				{ return eastl::move(mIterator[n]); }
		};

		template<typename Iterator1, typename Iterator2>
		inline bool
		operator==(const move_iterator<Iterator1>& a, const move_iterator<Iterator2>& b)
			{ return a.base() == b.base(); }


		template<typename Iterator1, typename Iterator2>
		inline bool
		operator!=(const move_iterator<Iterator1>& a, const move_iterator<Iterator2>& b)
			{ return !(a == b); }


		template<typename Iterator1, typename Iterator2>
		inline bool
		operator<(const move_iterator<Iterator1>& a, const move_iterator<Iterator2>& b)
			{ return a.base() < b.base(); }


		template<typename Iterator1, typename Iterator2>
		inline bool
		operator<=(const move_iterator<Iterator1>& a, const move_iterator<Iterator2>& b)
			{ return !(b < a); }


		template<typename Iterator1, typename Iterator2>
		inline bool
		operator>(const move_iterator<Iterator1>& a, const move_iterator<Iterator2>& b)
			{ return b < a; }


		template<typename Iterator1, typename Iterator2>
		inline bool
		operator>=(const move_iterator<Iterator1>& a, const move_iterator<Iterator2>& b)
			{ return !(a < b); }


		template<typename Iterator1, typename Iterator2>
		inline auto
		operator-(const move_iterator<Iterator1>& a, const move_iterator<Iterator2>& b) -> decltype(a.base() - b.base())
			{ return a.base() - b.base(); }


		template<typename Iterator>
		inline move_iterator<Iterator>
		operator+(typename move_iterator<Iterator>::difference_type n, const move_iterator<Iterator>& a)
			{ return a + n; }


		template<typename Iterator>
		inline move_iterator<Iterator> make_move_iterator(Iterator i)
			{ return move_iterator<Iterator>(i); }


		// make_move_if_noexcept_iterator returns move_iterator<Iterator> if the Iterator is of a noexcept type;
		// otherwise returns Iterator as-is. The point of this is to be able to avoid moves that can generate exceptions and instead 
		// fall back to copies or whatever the default IteratorType::operator* returns for use by copy/move algorithms.
		// To consider: merge the conditional expression usage here with the one used by move_if_noexcept, as they are the same condition.
		#if EASTL_EXCEPTIONS_ENABLED
			#if defined(EA_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS)
				namespace Internal
				{
					template <typename Iterator, bool is_noexcept>
					struct mminei_helper_1
					{
						typedef eastl::move_iterator<Iterator> ReturnType; 
						static inline ReturnType mminei(Iterator i)
							{ return eastl::move_iterator<Iterator>(i); }
					};
					template <typename Iterator>
					struct mminei_helper_1<Iterator, false>
					{
						typedef Iterator ReturnType; 
						static inline ReturnType mminei(Iterator i)
							{ return eastl::move_iterator<Iterator>(i); }
					};

					template <typename Iterator>
					struct mminei_helper
					{
						static const bool is_noexcept = eastl::is_nothrow_move_constructible<typename eastl::iterator_traits<Iterator>::value_type>::value || !eastl::is_copy_constructible<typename eastl::iterator_traits<Iterator>::value_type>::value;
						typedef typename mminei_helper_1<Iterator, is_noexcept>::ReturnType ReturnType;
					};
				}

				template <typename Iterator>
				inline typename Internal::mminei_helper<Iterator>::ReturnType
				make_move_if_noexcept_iterator(Iterator i)
				{
					return Internal::mminei_helper_1<Iterator, Internal::mminei_helper<Iterator>::is_noexcept>::mminei(i);
				}
			#else
				template <typename Iterator, typename IteratorType = typename eastl::conditional<eastl::is_nothrow_move_constructible<typename eastl::iterator_traits<Iterator>::value_type>::value || 
																								 !eastl::is_copy_constructible<typename eastl::iterator_traits<Iterator>::value_type>::value, 
																								 eastl::move_iterator<Iterator>, Iterator>::type>
				inline IteratorType make_move_if_noexcept_iterator(Iterator i)
					{ return IteratorType(i); }
			#endif
		#else
			// Else there are no exceptions and thus we always return a move_iterator.
			template <typename Iterator>
			inline eastl::move_iterator<Iterator> make_move_if_noexcept_iterator(Iterator i)
				{ return eastl::move_iterator<Iterator>(i); }
		#endif
	#else

		// To consider: Should we make a dummy move_iterator that does nothing new?
		// It could be a subclass of generic_iterator.

	#endif // #if EASTL_MOVE_SEMANTICS_ENABLED



	/// is_move_iterator
	///
	/// This is a type traits extension utility.
	/// Given an iterator, tells if it's a move iterator vs anything else.
	/// Example usage (though somewhat useless):
	///     template <typename T>
	///     bool IsMoveIterator() { return typename eastl::is_move_iterator<T>::value; }
	/// 
	template <typename T>
	struct is_move_iterator
		: public eastl::false_type {};

	#if EASTL_MOVE_SEMANTICS_ENABLED
		template<typename Iterator>
		struct is_move_iterator< eastl::move_iterator<Iterator> >
			: public eastl::true_type {};
	#endif


	/// unwrap_move_iterator
	///
	/// Returns Iterator::get_base() if it's a move_iterator, else returns Iterator as-is.
	///
	/// Example usage:
	///      vector<int> intVector;
	///      eastl::move_iterator<vector<int>::iterator> moveIterator(intVector.begin());
	///      vector<int>::iterator it = unwrap_move_iterator(moveIterator);
	///
	template <typename Iterator>
	inline typename eastl::is_iterator_wrapper_helper<Iterator, eastl::is_move_iterator<Iterator>::value>::iterator_type unwrap_move_iterator(Iterator it)
		{ return eastl::is_iterator_wrapper_helper<Iterator, eastl::is_move_iterator<Iterator>::value>::get_base(it); }




	/// back_insert_iterator
	///
	/// A back_insert_iterator is simply a class that acts like an iterator but when you 
	/// assign a value to it, it calls push_back on the container with the value.
	///
	template <typename Container>
	class back_insert_iterator : public iterator<EASTL_ITC_NS::output_iterator_tag, void, void, void, void>
	{
	public:
		typedef back_insert_iterator<Container>     this_type;
		typedef Container                           container_type;
		typedef typename Container::const_reference const_reference;

	protected:
		Container& container;

	public:
		//back_insert_iterator(); // Not valid. Must construct with a Container.

		//back_insert_iterator(const this_type& x) // Compiler-implemented
		//    : container(x.container) { }

		explicit back_insert_iterator(Container& x)
			: container(x) { }

		back_insert_iterator& operator=(const_reference value)
			{ container.push_back(value); return *this; }

		back_insert_iterator& operator*()
			{ return *this; }

		back_insert_iterator& operator++()
			{ return *this; } // This is by design.

		back_insert_iterator operator++(int)
			{ return *this; } // This is by design.

	protected:
		void operator=(const this_type&){} // Declared to avoid compiler warnings about inability to generate this function.
	};


	/// back_inserter
	///
	/// Creates an instance of a back_insert_iterator.
	///
	template <typename Container>
	inline back_insert_iterator<Container>
	back_inserter(Container& x)
		{ return back_insert_iterator<Container>(x); }




	/// front_insert_iterator
	///
	/// A front_insert_iterator is simply a class that acts like an iterator but when you 
	/// assign a value to it, it calls push_front on the container with the value.
	///
	template <typename Container>
	class front_insert_iterator : public iterator<EASTL_ITC_NS::output_iterator_tag, void, void, void, void>
	{
	public:
		typedef front_insert_iterator<Container>    this_type;
		typedef Container                           container_type;
		typedef typename Container::const_reference const_reference;

	protected:
		Container& container;

	public:
		//front_insert_iterator(); // Not valid. Must construct with a Container.

		//front_insert_iterator(const this_type& x) // Compiler-implemented
		//    : container(x.container) { }

		explicit front_insert_iterator(Container& x)
			: container(x) { }

		front_insert_iterator& operator=(const_reference value)
			{ container.push_front(value); return *this; }

		front_insert_iterator& operator*()
			{ return *this; }

		front_insert_iterator& operator++()
			{ return *this; } // This is by design.

		front_insert_iterator operator++(int)
			{ return *this; } // This is by design.

	protected:
		void operator=(const this_type&){} // Declared to avoid compiler warnings about inability to generate this function.
	};


	/// front_inserter
	///
	/// Creates an instance of a front_insert_iterator.
	///
	template <typename Container>
	inline front_insert_iterator<Container>
	front_inserter(Container& x)
		{ return front_insert_iterator<Container>(x); }




	/// insert_iterator
	///
	/// An insert_iterator is like an iterator except that when you assign a value to it, 
	/// the insert_iterator inserts the value into the container and increments the iterator.
	///
	/// insert_iterator is an iterator adaptor that functions as an OutputIterator: 
	/// assignment through an insert_iterator inserts an object into a container. 
	/// Specifically, if ii is an insert_iterator, then ii keeps track of a container c and 
	/// an insertion point p; the expression *ii = x performs the insertion container.insert(p, x).
	///
	/// If you assign through an insert_iterator several times, then you will be inserting 
	/// several elements into the underlying container. In the case of a sequence, they will 
	/// appear at a particular location in the underlying sequence, in the order in which 
	/// they were inserted: one of the arguments to insert_iterator's constructor is an 
	/// iterator p, and the new range will be inserted immediately before p.
	///
	template <typename Container>
	class insert_iterator : public iterator<EASTL_ITC_NS::output_iterator_tag, void, void, void, void>
	{
	public:
		typedef Container                           container_type;
		typedef typename Container::iterator        iterator_type;
		typedef typename Container::const_reference const_reference;

	protected:
		Container&     container;
		iterator_type  it; 

	public:
		// This assignment operator is defined more to stop compiler warnings (e.g. VC++ C4512)
		// than to be useful. However, it does allow an insert_iterator to be assigned to another 
		// insert iterator provided that they point to the same container.
		insert_iterator& operator=(const insert_iterator& x)
		{
			EASTL_ASSERT(&x.container == &container);
			it = x.it;
			return *this;
		}

		insert_iterator(Container& x, iterator_type itNew)
			: container(x), it(itNew) {}

		insert_iterator& operator=(const_reference value)
		{
			it = container.insert(it, value);
			++it;
			return *this;
		}

		insert_iterator& operator*()
			{ return *this; }

		insert_iterator& operator++()
			{ return *this; } // This is by design.

		insert_iterator& operator++(int)
			{ return *this; } // This is by design.

	}; // insert_iterator 


	/// inserter
	///
	/// Creates an instance of an insert_iterator.
	///
	template <typename Container, typename Iterator>
	inline eastl::insert_iterator<Container>
	inserter(Container& x, Iterator i)
	{
		typedef typename Container::iterator iterator;
		return eastl::insert_iterator<Container>(x, iterator(i));
	}


	/// is_insert_iterator
	///
	/// This is a type traits extension utility.
	/// Given an iterator, tells if it's an insert_iterator vs anything else.
	/// If it's a insert_iterator wrapped by another iterator then value is false.
	/// 
	template <typename T>
	struct is_insert_iterator
		: public eastl::false_type {};

	template<typename Iterator>
	struct is_insert_iterator< eastl::insert_iterator<Iterator> >
		: public eastl::true_type {};




	//////////////////////////////////////////////////////////////////////////////////
	/// distance
	///
	/// Implements the distance() function. There are two versions, one for
	/// random access iterators (e.g. with vector) and one for regular input
	/// iterators (e.g. with list). The former is more efficient.
	///
	template <typename InputIterator>
	inline typename eastl::iterator_traits<InputIterator>::difference_type
	distance_impl(InputIterator first, InputIterator last, EASTL_ITC_NS::input_iterator_tag)
	{
		typename eastl::iterator_traits<InputIterator>::difference_type n = 0;

		while(first != last)
		{
			++first;
			++n;
		}
		return n;
	}

	template <typename RandomAccessIterator>
	inline typename eastl::iterator_traits<RandomAccessIterator>::difference_type
	distance_impl(RandomAccessIterator first, RandomAccessIterator last, EASTL_ITC_NS::random_access_iterator_tag)
	{
		return last - first;
	}

	// Special version defined so that std C++ iterators can be recognized by 
	// this function. Unfortunately, this function treats all foreign iterators
	// as InputIterators and thus can seriously hamper performance in the case
	// of large ranges of bidirectional_iterator_tag iterators.
	//template <typename InputIterator>
	//inline typename eastl::iterator_traits<InputIterator>::difference_type
	//distance_impl(InputIterator first, InputIterator last, ...)
	//{
	//    typename eastl::iterator_traits<InputIterator>::difference_type n = 0;
	//
	//    while(first != last)
	//    {
	//        ++first;
	//        ++n;
	//    }
	//    return n;
	//}

	template <typename InputIterator>
	inline typename eastl::iterator_traits<InputIterator>::difference_type
	distance(InputIterator first, InputIterator last)
	{
		typedef typename eastl::iterator_traits<InputIterator>::iterator_category IC;

		return eastl::distance_impl(first, last, IC());
	}




	//////////////////////////////////////////////////////////////////////////////////
	/// advance
	///
	/// Implements the advance() function. There are three versions, one for
	/// random access iterators (e.g. with vector), one for bidirectional 
	/// iterators (list) and one for regular input iterators (e.g. with slist). 
	///
	template <typename InputIterator, typename Distance>
	inline void
	advance_impl(InputIterator& i, Distance n, EASTL_ITC_NS::input_iterator_tag)
	{
		while(n--)
			++i;
	}

	template <bool signedDistance>
	struct advance_bi_impl
	{
		template <typename BidirectionalIterator, typename Distance>
		static void advance_impl(BidirectionalIterator& i, Distance n) // Specialization for unsigned distance type.
		{
			while(n--)
				++i;
		}
	};

	template <>
	struct advance_bi_impl<true>
	{
		template <typename BidirectionalIterator, typename Distance>
		static void advance_impl(BidirectionalIterator& i, Distance n) // Specialization for signed distance type.
		{
			if(n > 0)
			{
				while(n--)
					++i;
			}
			else
			{
				while(n++)
					--i;
			}
		}
	};

	template <typename BidirectionalIterator, typename Distance>
	inline void
	advance_impl(BidirectionalIterator& i, Distance n, EASTL_ITC_NS::bidirectional_iterator_tag)
	{
		advance_bi_impl<eastl::is_signed<Distance>::value>::advance_impl(i, n);
	}

	template <typename RandomAccessIterator, typename Distance>
	inline void
	advance_impl(RandomAccessIterator& i, Distance n, EASTL_ITC_NS::random_access_iterator_tag)
	{
		i += n;
	}

	// Special version defined so that std C++ iterators can be recognized by 
	// this function. Unfortunately, this function treats all foreign iterators
	// as InputIterators and thus can seriously hamper performance in the case
	// of large ranges of bidirectional_iterator_tag iterators.
	//template <typename InputIterator, typename Distance>
	//inline void
	//advance_impl(InputIterator& i, Distance n, ...)
	//{
	//    while(n--)
	//        ++i;
	//}

	template <typename InputIterator, typename Distance>
	inline void
	advance(InputIterator& i, Distance n)
	{
		typedef typename eastl::iterator_traits<InputIterator>::iterator_category IC;

		eastl::advance_impl(i, n, IC());
	}


	// eastl::next / eastl::prev
	// Return the nth/-nth successor of iterator it.
	//
	// http://en.cppreference.com/w/cpp/iterator/next
	//
	template<typename InputIterator>
	inline InputIterator 
	next(InputIterator it, typename eastl::iterator_traits<InputIterator>::difference_type n = 1)
	{
		eastl::advance(it, n);
		return it;
	}

	template<typename InputIterator>
	inline InputIterator
	prev(InputIterator it, typename eastl::iterator_traits<InputIterator>::difference_type n = 1)
	{
		eastl::advance(it, -n);
		return it;
	}


#if defined(EA_COMPILER_CPP11_ENABLED) && EA_COMPILER_CPP11_ENABLED
	
	// eastl::data
	//
	// http://en.cppreference.com/w/cpp/iterator/data
	//
	template <class Container>
	EA_CONSTEXPR auto data(Container& c) -> decltype(c.data())
		{ return c.data(); }

	template <class Container>
	EA_CONSTEXPR auto data(const Container& c) -> decltype(c.data())
		{ return c.data(); }

	template <class T, std::size_t N>
	EA_CONSTEXPR T* data(T(&array)[N]) EA_NOEXCEPT 
		{ return array; }

	template <class E>
	EA_CONSTEXPR const E* data(std::initializer_list<E> il) EA_NOEXCEPT
		{ return il.begin(); }


	// eastl::size
	//
	// http://en.cppreference.com/w/cpp/iterator/size
	//
	template <class C> 
	EA_CONSTEXPR auto size(const C& c) -> decltype(c.size())
		{ return c.size(); }

	template <class T, std::size_t N>
	EA_CONSTEXPR std::size_t size(const T (&)[N]) EA_NOEXCEPT
		{ return N; }


	// eastl::empty
	// 
	// http://en.cppreference.com/w/cpp/iterator/empty
	//
	template <class Container> 
	EA_CONSTEXPR auto empty(const Container& c) -> decltype(c.empty())
		{ return c.empty(); }

	template <class T, std::size_t N>
	EA_CONSTEXPR bool empty(const T (&)[N]) EA_NOEXCEPT
		{ return false; }

	template <class E> 
	EA_CONSTEXPR bool empty(std::initializer_list<E> il) EA_NOEXCEPT
		{ return il.size() == 0; }

#endif // defined(EA_COMPILER_CPP11_ENABLED) && EA_COMPILER_CPP11_ENABLED


	// eastl::begin / eastl::end
	// http://en.cppreference.com/w/cpp/iterator/begin
	//
	// In order to enable eastl::begin and eastl::end, the compiler needs to have conforming support
	// for argument-dependent lookup if it supports C++11 range-based for loops. The reason for this is
	// that in C++11 range-based for loops result in usage of std::begin/std::end, but allow that to 
	// be overridden by argument-dependent lookup:
	//     C++11 Standard, section 6.5.4, paragraph 1.
	//     "otherwise, begin-expr and end-expr are begin(__range) and end(__range), respectively, 
	//      where begin and end are looked up with argument-dependent lookup (3.4.2). For the 
	//      purposes of this name lookup, namespace std is an associated namespace."
	// It turns out that one compiler has a problem: GCC 4.6. That version added support for 
	// range-based for loops but has broken argument-dependent lookup which was fixed in GCC 4.7.
	//
	#if (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION == 4006))
		#define EASTL_BEGIN_END_ENABLED 0
	#else
		#define EASTL_BEGIN_END_ENABLED 1
	#endif

	#if EASTL_BEGIN_END_ENABLED
		template <typename Container>
		inline auto begin(Container& container) -> decltype(container.begin())
		{    
			return container.begin();
		}

		template <typename Container>
		inline auto begin(const Container& container) -> decltype(container.begin())
		{    
			return container.begin();
		}

		template <typename Container>
		inline auto cbegin(const Container& container) -> decltype(container.begin())
		{    
			return container.begin();
		}

		template <typename Container>
		inline auto end(Container& container) -> decltype(container.end())
		{
			return container.end();
		}

		template <typename Container>
		inline auto end(const Container& container) -> decltype(container.end())
		{
			return container.end();
		}

		template <typename Container>
		inline auto cend(const Container& container) -> decltype(container.end())
		{
			return container.end();
		}

		template <typename Container>
		inline auto rbegin(Container& container) -> decltype(container.rbegin())
		{
			return container.rbegin();
		}

		template <typename Container>
		inline auto rbegin(const Container& container) -> decltype(container.rbegin())
		{
			return container.rbegin();
		}

		template <typename Container>
		inline auto rend(Container& container) -> decltype(container.rend())
		{
			return container.rend();
		}

		template <typename Container>
		inline auto rend(const Container& container) -> decltype(container.rend())
		{
			return container.rend();
		}

		template <typename Container>
		inline auto crbegin(const Container& container) -> decltype(eastl::rbegin(container))
		{
			return container.rbegin();
		}

		template <typename Container>
		inline auto crend(const Container& container) -> decltype(eastl::rend(container))
		{
			return container.rend();
		}

		template<typename T, size_t arraySize>
		inline T* begin(T (&arrayObject)[arraySize])
		{
			return arrayObject;
		}

		template<typename T, size_t arraySize> 
		inline T* end(T (&arrayObject)[arraySize])
		{
			return (arrayObject + arraySize);
		}

		template <typename T, size_t arraySize>
		inline reverse_iterator<T*> rbegin(T (&arrayObject)[arraySize])
		{
			return reverse_iterator<T*>(arrayObject + arraySize);
		}

		template <typename T, size_t arraySize>
		inline reverse_iterator<T*> rend(T (&arrayObject)[arraySize])
		{
			return reverse_iterator<T*>(arrayObject);
		}

		template <typename E>
		inline reverse_iterator<const E*> rbegin(std::initializer_list<E> ilist)
		{
			return eastl::reverse_iterator<const E*>(ilist.end());
		}

		template <typename E>
		inline reverse_iterator<const E*> rend(std::initializer_list<E> ilist)
		{
			return eastl::reverse_iterator<const E*>(ilist.begin());
		}

		template <typename Iterator>
		reverse_iterator<Iterator> make_reverse_iterator(Iterator i)
			{ return reverse_iterator<Iterator>(i); }

	#endif // EASTL_BEGIN_END_ENABLED

} // namespace eastl



// Some compilers (e.g. GCC 4.6) support range-based for loops, but have a bug with 
// respect to argument-dependent lookup which results on them unilaterally using std::begin/end
// with range-based for loops. To work around this we #include <iterator> for this case in 
// order to make std::begin/end visible to users of <eastl/iterator.h>, for portability.
#if !EASTL_BEGIN_END_ENABLED && !defined(EA_COMPILER_NO_RANGE_BASED_FOR_LOOP)
	#include <iterator>
#endif



#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


#endif // Header include guard





