/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// A ring buffer is a FIFO (first-in, first-out) container which acts
// much like a queue. The difference is that a ring buffer is implemented
// via chasing pointers around a given container instead of like queue
// adds to the writes to the end of the container are reads from the begin.
// The benefit of a ring buffer is that memory allocations don't occur
// and new elements are neither added nor removed from the container.
// Elements in the container are simply assigned values in circles around
// the container.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_RING_BUFFER_H
#define EASTL_RING_BUFFER_H


#include <EASTL/internal/config.h>
#include <EASTL/iterator.h>
#include <EASTL/vector.h>
#include <EASTL/initializer_list.h>
#include <stddef.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{
	/// EASTL_RING_BUFFER_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_RING_BUFFER_DEFAULT_NAME
		#define EASTL_RING_BUFFER_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " ring_buffer" // Unless the user overrides something, this is "EASTL ring_buffer".
	#endif

	/// EASTL_RING_BUFFER_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_RING_BUFFER_DEFAULT_ALLOCATOR
		#define EASTL_RING_BUFFER_DEFAULT_ALLOCATOR allocator_type(EASTL_RING_BUFFER_DEFAULT_NAME)
	#endif


	/// ring_buffer_iterator
	///
	/// We force this iterator to act like a random access iterator even if 
	/// the underlying container doesn't support random access iteration.
	/// Any BidirectionalIterator can be a RandomAccessIterator; it just
	/// might be inefficient in some cases.
	///
	template <typename T, typename Pointer, typename Reference, typename Container>
	struct ring_buffer_iterator
	{
	public:
		typedef ring_buffer_iterator<T, Pointer, Reference, Container>   this_type;
		typedef T                                                        value_type;
		typedef Pointer                                                  pointer;
		typedef Reference                                                reference;
		typedef typename Container::size_type                            size_type;
		typedef typename Container::difference_type                      difference_type;
		typedef typename Container::iterator                             container_iterator;
		typedef typename Container::const_iterator                       container_const_iterator;
		typedef ring_buffer_iterator<T, T*, T&, Container>               iterator;
		typedef ring_buffer_iterator<T, const T*, const T&, Container>   const_iterator;
		typedef EASTL_ITC_NS::random_access_iterator_tag                 iterator_category;

	public:
		Container*         mpContainer;
		container_iterator mContainerIterator;

	public:
		ring_buffer_iterator();
		ring_buffer_iterator(Container* pContainer, const container_iterator& containerIterator);
		ring_buffer_iterator(const iterator& x);

		ring_buffer_iterator& operator=(const iterator& x);

		reference operator*() const;
		pointer   operator->() const;

		this_type& operator++();
		this_type  operator++(int);

		this_type& operator--();
		this_type  operator--(int);

		this_type& operator+=(difference_type n);
		this_type& operator-=(difference_type n);

		this_type operator+(difference_type n) const;
		this_type operator-(difference_type n) const;

	protected:
		void increment(difference_type n, EASTL_ITC_NS::input_iterator_tag);
		void increment(difference_type n, EASTL_ITC_NS::random_access_iterator_tag);

	}; // struct ring_buffer_iterator



	/// ring_buffer
	///
	/// Implements a ring buffer via a given container type, which would 
	/// typically be a vector or array, though any container which supports 
	/// bidirectional iteration would work.
	///
	/// A ring buffer is a FIFO (first-in, first-out) container which acts
	/// much like a queue. The difference is that a ring buffer is implemented
	/// via chasing pointers around a container and moving the read and write
	/// positions forward (and possibly wrapping around) as the container is 
	/// read and written via pop_front and push_back.
	///
	/// The benefit of a ring buffer is that memory allocations don't occur
	/// and new elements are neither added nor removed from the container.
	/// Elements in the container are simply assigned values in circles around
	/// the container.
	/// 
	/// ring_buffer is different from other containers -- including adapter
	/// containers -- in how iteration is done. Iteration of a ring buffer
	/// starts at the current begin position, proceeds to the end of the underlying
	/// container, and continues at the begin of the underlying container until
	/// the ring buffer's current end position. Thus a ring_buffer does 
	/// indeed have a begin and an end, though the values of begin and end 
	/// chase each other around the container. An empty ring_buffer is one 
	/// in which end == begin, and a full ring_buffer is one in which 
	/// end + 1 == begin. 
	///
	/// Example of a ring buffer layout, where + indicates queued items:
	///    ++++++++++--------------------------------+++++++++
	///              ^                               ^
	///              end                             begin
	///
	/// Empty ring buffer:
	///    ---------------------------------------------------
	///                              ^
	///                          begin / end
	///
	/// Full ring buffer. Note that one item is necessarily unused; it is 
	/// analagous to a '\0' at the end of a C string:
	///    +++++++++++++++++++++++++++++++++++++++++-+++++++++
	///                                             ^^
	///                                           end begin
	///
	/// A push_back operation on a ring buffer assigns the new value to end.  
	/// If there is no more space in the buffer, this will result in begin
	/// being overwritten and the begin position being moved foward one position.
	/// The user can use the full() function to detect this condition.
	/// Note that elements in a ring buffer are not created or destroyed as 
	/// their are added and removed; they are merely assigned. Only on 
	/// container construction and destruction are any elements created and 
	/// destroyed.
	///
	/// The ring buffer can be used in either direction. By this we mean that 
	/// you can use push_back to add items and pop_front to remove them; or you can
	/// use push_front to add items and pop_back to remove them. You aren't 
	/// limited to these operations; you can push or pop from either side
	/// arbitrarily and you can insert or erase anywhere in the container.
	///
	/// The ring buffer requires the user to specify a Container type, which 
	/// by default is vector. However, any container with bidirectional iterators
	/// will work, such as list, deque, string or any of the fixed_* versions
	/// of these containers, such as fixed_string. Since ring buffer works via copying
	/// elements instead of allocating and freeing nodes, inserting in the middle
	/// of a ring buffer based on list (instead of vector) is no more efficient.
	///
	/// To use the ring buffer, its container must be resized to the desired
	/// ring buffer size. Changing the size of a ring buffer may cause ring
	/// buffer iterators to invalidate.
	///
	/// An alternative to using a ring buffer is to use a list with a user-created 
	/// node pool and custom allocator. There are various tradeoffs that result from this.
	///
	/// Example usage:
	///     ring_buffer< int, list<int> > rb(100);
	///     rb.push_back(1);
	/// 
	/// Example usage:
	///     // Example of creating an on-screen debug log that shows 16 
	///     // strings at a time and scrolls older strings away.
	///
	///     // Create ring buffer of 16 strings.
	///     ring_buffer< string, vector<string> > debugLogText(16);
	///     
	///     // Reserve 128 chars for each line. This can make it so that no 
	///     // runtime memory allocations occur.
	///     for(vector<string>::iterator it = debugLogText.get_container().begin(),
	///         itEnd = debugLogText.get_container().end(); it != itEnd; ++it)
	///     {
	///         (*it).reserve(128);
	///     }
	///     
	///     // Add a new string, using push_front() and front() instead of 
	///     // push_front(str) in order to avoid creating a temporary str.
	///     debugLogText.push_front();
	///     debugLogText.front() = "Player fired weapon";
	///
	template <typename T, typename Container = eastl::vector<T>, typename Allocator = typename Container::allocator_type>
	class ring_buffer
	{
	public:
		typedef ring_buffer<T, Container, Allocator>                   this_type;
		typedef Container                                              container_type;
		typedef Allocator                                              allocator_type;

		typedef typename Container::value_type                         value_type;
		typedef typename Container::reference                          reference;
		typedef typename Container::const_reference                    const_reference;
		typedef typename Container::size_type                          size_type;
		typedef typename Container::difference_type                    difference_type;
		typedef typename Container::iterator                           container_iterator;
		typedef typename Container::const_iterator                     container_const_iterator;
		typedef ring_buffer_iterator<T, T*, T&, Container>             iterator;
		typedef ring_buffer_iterator<T, const T*, const T&, Container> const_iterator;
		typedef eastl::reverse_iterator<iterator>                      reverse_iterator;
		typedef eastl::reverse_iterator<const_iterator>                const_reverse_iterator;    

	public:                             // We declare public so that global comparison operators can be implemented without adding an inline level and without tripping up GCC 2.x friend declaration failures. GCC (through at least v4.0) is poor at inlining and performance wins over correctness.
		Container          c;           // We follow the naming convention established for stack, queue, priority_queue and name this 'c'. This variable must always have a size of at least 1, as even an empty ring_buffer has an unused terminating element.

	protected:
		container_iterator mBegin;      // We keep track of where our begin and end are by using Container iterators. 
		container_iterator mEnd;
		size_type          mSize;

	public:
		// There currently isn't a ring_buffer constructor that specifies an initial size, unlike other containers.
		explicit ring_buffer(size_type cap = 0);                                // Construct with an initial capacity (but size of 0).
		explicit ring_buffer(size_type cap, const allocator_type& allocator); 
		explicit ring_buffer(const Container& x);
		explicit ring_buffer(const allocator_type& allocator);
		ring_buffer(const this_type& x);
		ring_buffer(this_type&& x);
		ring_buffer(this_type&& x, const allocator_type& allocator);
		ring_buffer(std::initializer_list<value_type> ilist, const allocator_type& allocator = EASTL_RING_BUFFER_DEFAULT_ALLOCATOR); // This function sets the capacity to be equal to the size of the initializer list.

		// No destructor necessary. Default will do.

		this_type& operator=(const this_type& x);
		this_type& operator=(std::initializer_list<value_type> ilist);
		this_type& operator=(this_type&& x);

		template <typename InputIterator>
		void assign(InputIterator first, InputIterator last);

		void swap(this_type& x);

		iterator               begin()          EA_NOEXCEPT;
		const_iterator         begin()    const EA_NOEXCEPT;
		const_iterator         cbegin()   const EA_NOEXCEPT;

		iterator               end()            EA_NOEXCEPT;
		const_iterator         end()      const EA_NOEXCEPT;
		const_iterator         cend()     const EA_NOEXCEPT;

		reverse_iterator       rbegin()         EA_NOEXCEPT;
		const_reverse_iterator rbegin()   const EA_NOEXCEPT;
		const_reverse_iterator crbegin()  const EA_NOEXCEPT;

		reverse_iterator       rend()           EA_NOEXCEPT;
		const_reverse_iterator rend()     const EA_NOEXCEPT;
		const_reverse_iterator crend()    const EA_NOEXCEPT;

		bool                   empty()    const EA_NOEXCEPT;
		bool                   full()     const EA_NOEXCEPT;
		size_type              size()     const EA_NOEXCEPT;
		size_type              capacity() const EA_NOEXCEPT;

		void                   resize(size_type n);
		void                   set_capacity(size_type n); // Sets the capacity to the given value, including values less than the current capacity. Adjusts the size downward if n < size, by throwing out the oldest elements in the buffer.
		void                   reserve(size_type n);      // Reserve a given capacity. Doesn't decrease the capacity; it only increases it (for compatibility with other containers' behavior).

		reference       front();
		const_reference front() const;

		reference       back();              
		const_reference back() const;

		void            push_back(const value_type& value);
		reference       push_back();

		void            push_front(const value_type& value);
		reference       push_front();

		void            pop_back();
		void            pop_front();

		reference       operator[](size_type n);
		const_reference operator[](size_type n) const;

		// To consider:
		// size_type read(value_type* pDestination, size_type nCount);
		// size_type read(iterator** pPosition1, iterator** pPosition2, size_type& nCount1, size_type& nCount2);

		/* To do:
			template <class... Args>
			reference emplace_front(Args&&... args);

			template <class... Args>
			reference emplace_back(Args&&... args);

			template <class... Args>
			iterator emplace(const_iterator position, Args&&... args);
		*/

		iterator insert(const_iterator position, const value_type& value);
		void     insert(const_iterator position, size_type n, const value_type& value);
		void     insert(const_iterator position, std::initializer_list<value_type> ilist);

		template <typename InputIterator>
		void insert(const_iterator position, InputIterator first, InputIterator last);

		iterator         erase(const_iterator position);
		iterator         erase(const_iterator first, const_iterator last);
		reverse_iterator erase(const_reverse_iterator position);
		reverse_iterator erase(const_reverse_iterator first, const_reverse_iterator last);

		void clear();

		container_type&       get_container();
		const container_type& get_container() const;

		bool validate() const;
		int  validate_iterator(const_iterator i) const;

	protected:
		//size_type DoGetSize(EASTL_ITC_NS::input_iterator_tag) const;
		//size_type DoGetSize(EASTL_ITC_NS::random_access_iterator_tag) const;

	}; // class ring_buffer




	///////////////////////////////////////////////////////////////////////
	// ring_buffer_iterator
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Pointer, typename Reference, typename Container>
	ring_buffer_iterator<T, Pointer, Reference, Container>::ring_buffer_iterator()
		: mpContainer(NULL), mContainerIterator()
	{
	}


	template <typename T, typename Pointer, typename Reference, typename Container>
	ring_buffer_iterator<T, Pointer, Reference, Container>::ring_buffer_iterator(Container* pContainer, const container_iterator& containerIterator)
		: mpContainer(pContainer), mContainerIterator(containerIterator)
	{
	}


	template <typename T, typename Pointer, typename Reference, typename Container>
	ring_buffer_iterator<T, Pointer, Reference, Container>::ring_buffer_iterator(const iterator& x)
		: mpContainer(x.mpContainer), mContainerIterator(x.mContainerIterator)
	{
	}


	template <typename T, typename Pointer, typename Reference, typename Container>
	ring_buffer_iterator<T, Pointer, Reference, Container>& 
	ring_buffer_iterator<T, Pointer, Reference, Container>::operator=(const iterator& x)
	{
		mpContainer        = x.mpContainer;
		mContainerIterator = x.mContainerIterator;
		return *this;
	}

	template <typename T, typename Pointer, typename Reference, typename Container>
	typename ring_buffer_iterator<T, Pointer, Reference, Container>::reference
	ring_buffer_iterator<T, Pointer, Reference, Container>::operator*() const
	{
		return *mContainerIterator;
	}


	template <typename T, typename Pointer, typename Reference, typename Container>
	typename ring_buffer_iterator<T, Pointer, Reference, Container>::pointer
	ring_buffer_iterator<T, Pointer, Reference, Container>::operator->() const
	{
		return &*mContainerIterator;
	}


	template <typename T, typename Pointer, typename Reference, typename Container>
	typename ring_buffer_iterator<T, Pointer, Reference, Container>::this_type&
	ring_buffer_iterator<T, Pointer, Reference, Container>::operator++()
	{
		if(EASTL_UNLIKELY(++mContainerIterator == mpContainer->end()))
			mContainerIterator = mpContainer->begin();
		return *this;
	}


	template <typename T, typename Pointer, typename Reference, typename Container>
	typename ring_buffer_iterator<T, Pointer, Reference, Container>::this_type
	ring_buffer_iterator<T, Pointer, Reference, Container>::operator++(int)
	{
		const this_type temp(*this);
		if(EASTL_UNLIKELY(++mContainerIterator == mpContainer->end()))
			mContainerIterator = mpContainer->begin();
		return temp;
	}


	template <typename T, typename Pointer, typename Reference, typename Container>
	typename ring_buffer_iterator<T, Pointer, Reference, Container>::this_type&
	ring_buffer_iterator<T, Pointer, Reference, Container>::operator--()
	{
		if(EASTL_UNLIKELY(mContainerIterator == mpContainer->begin()))
			mContainerIterator = mpContainer->end();
		--mContainerIterator;
		return *this;
	}


	template <typename T, typename Pointer, typename Reference, typename Container>
	typename ring_buffer_iterator<T, Pointer, Reference, Container>::this_type
	ring_buffer_iterator<T, Pointer, Reference, Container>::operator--(int)
	{
		const this_type temp(*this);
		if(EASTL_UNLIKELY(mContainerIterator == mpContainer->begin()))
			mContainerIterator = mpContainer->end();
		--mContainerIterator;
		return temp;
	}


	template <typename T, typename Pointer, typename Reference, typename Container>
	typename ring_buffer_iterator<T, Pointer, Reference, Container>::this_type&
	ring_buffer_iterator<T, Pointer, Reference, Container>::operator+=(difference_type n)
	{
		typedef typename eastl::iterator_traits<container_iterator>::iterator_category IC;
		increment(n, IC());
		return *this;
	}


	template <typename T, typename Pointer, typename Reference, typename Container>
	typename ring_buffer_iterator<T, Pointer, Reference, Container>::this_type&
	ring_buffer_iterator<T, Pointer, Reference, Container>::operator-=(difference_type n)
	{
		typedef typename eastl::iterator_traits<container_iterator>::iterator_category IC;
		increment(-n, IC());
		return *this;
	}


	template <typename T, typename Pointer, typename Reference, typename Container>
	typename ring_buffer_iterator<T, Pointer, Reference, Container>::this_type
	ring_buffer_iterator<T, Pointer, Reference, Container>::operator+(difference_type n) const
	{
		return this_type(*this).operator+=(n);
	}


	template <typename T, typename Pointer, typename Reference, typename Container>
	typename ring_buffer_iterator<T, Pointer, Reference, Container>::this_type
	ring_buffer_iterator<T, Pointer, Reference, Container>::operator-(difference_type n) const
	{
		return this_type(*this).operator+=(-n);
	}


	template <typename T, typename Pointer, typename Reference, typename Container>
	void ring_buffer_iterator<T, Pointer, Reference, Container>::increment(difference_type n, EASTL_ITC_NS::input_iterator_tag)
	{
		// n cannot be negative, as input iterators don't support reverse iteration.
		while(n-- > 0)
			operator++();
	}


	template <typename T, typename Pointer, typename Reference, typename Container>
	void ring_buffer_iterator<T, Pointer, Reference, Container>::increment(difference_type n, EASTL_ITC_NS::random_access_iterator_tag)
	{
		// We make the assumption here that the user is incrementing from a valid
		// starting position to a valid ending position. Thus *this + n yields a 
		// valid iterator, including if n happens to be a negative value.

		if(n >= 0)
		{
			const difference_type d = mpContainer->end() - mContainerIterator;

			if(n < d)
				mContainerIterator += n;
			else
				mContainerIterator = mpContainer->begin() + (n - d);
		}
		else
		{
			// Recall that n and d here will be negative and so the logic here works as intended.
			const difference_type d = mpContainer->begin() - mContainerIterator;

			if(n >= d)
				mContainerIterator += n;
			else
				mContainerIterator = mpContainer->end() + (n - d);
		}
	}


	// Random access iterators must support operator + and operator -.
	// You can only add an integer to an iterator, and you cannot add two iterators.
	template <typename T, typename Pointer, typename Reference, typename Container>
	inline ring_buffer_iterator<T, Pointer, Reference, Container>
	operator+(ptrdiff_t n, const ring_buffer_iterator<T, Pointer, Reference, Container>& x)
	{
		return x + n; // Implement (n + x) in terms of (x + n).
	}


	// You can only add an integer to an iterator, but you can subtract two iterators.
	template <typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB, typename Container>
	inline typename ring_buffer_iterator<T, PointerA, ReferenceA, Container>::difference_type
	operator-(const ring_buffer_iterator<T, PointerA, ReferenceA, Container>& a,
			  const ring_buffer_iterator<T, PointerB, ReferenceB, Container>& b)
	{
		typedef typename ring_buffer_iterator<T, PointerA, ReferenceA, Container>::difference_type difference_type;

		// To do: If container_iterator is a random access iterator, then do a simple calculation.
		// Otherwise, we have little choice but to iterate from a to b and count as we go.
		// See the ring_buffer::size function for an implementation of this.

		// Iteration implementation:
		difference_type d = 0;

		for(ring_buffer_iterator<T, PointerA, ReferenceA, Container> temp(b); temp != a; ++temp)
			++d;

		return d;
	}


	// The C++ defect report #179 requires that we support comparisons between const and non-const iterators.
	// Thus we provide additional template paremeters here to support this. The defect report does not
	// require us to support comparisons between reverse_iterators and const_reverse_iterators.
	template <typename T, typename PointerA, typename ReferenceA, typename ContainerA,
						  typename PointerB, typename ReferenceB, typename ContainerB>
	inline bool operator==(const ring_buffer_iterator<T, PointerA, ReferenceA, ContainerA>& a, 
						   const ring_buffer_iterator<T, PointerB, ReferenceB, ContainerB>& b)
	{
		// Perhaps we should compare the container pointer as well.
		// However, for valid iterators this shouldn't be necessary.
		return a.mContainerIterator == b.mContainerIterator;
	}


	template <typename T, typename PointerA, typename ReferenceA, typename ContainerA,
						  typename PointerB, typename ReferenceB, typename ContainerB>
	inline bool operator!=(const ring_buffer_iterator<T, PointerA, ReferenceA, ContainerA>& a, 
						   const ring_buffer_iterator<T, PointerB, ReferenceB, ContainerB>& b)
	{
		// Perhaps we should compare the container pointer as well.
		// However, for valid iterators this shouldn't be necessary.
		return !(a.mContainerIterator == b.mContainerIterator);
	}


	// We provide a version of operator!= for the case where the iterators are of the 
	// same type. This helps prevent ambiguity errors in the presence of rel_ops.
	template <typename T, typename Pointer, typename Reference, typename Container>
	inline bool operator!=(const ring_buffer_iterator<T, Pointer, Reference, Container>& a, 
						   const ring_buffer_iterator<T, Pointer, Reference, Container>& b)
	{
		return !(a.mContainerIterator == b.mContainerIterator);
	}




	///////////////////////////////////////////////////////////////////////
	// ring_buffer
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Container, typename Allocator>
	ring_buffer<T, Container, Allocator>::ring_buffer(size_type cap)
		: c() // Default construction with default allocator for the container.
	{
		// To do: This code needs to be amended to deal with possible exceptions 
		// that could occur during the resize call below.

		// We add one because the element at mEnd is necessarily unused.
		c.resize(cap + 1); // Possibly we could construct 'c' with size, but c may not have such a ctor, though we rely on it having a resize function. 
		mBegin = c.begin();
		mEnd   = mBegin;
		mSize  = 0;
	}


	template <typename T, typename Container, typename Allocator>
	ring_buffer<T, Container, Allocator>::ring_buffer(size_type cap, const allocator_type& allocator)
		: c(allocator)
	{
		// To do: This code needs to be amended to deal with possible exceptions 
		// that could occur during the resize call below.

		// We add one because the element at mEnd is necessarily unused.
		c.resize(cap + 1); // Possibly we could construct 'c' with size, but c may not have such a ctor, though we rely on it having a resize function. 
		mBegin = c.begin();
		mEnd   = mBegin;
		mSize  = 0;
	}


	template <typename T, typename Container, typename Allocator>
	ring_buffer<T, Container, Allocator>::ring_buffer(const Container& x)
		: c(x) // This copies elements from x, but unless the user is doing some tricks, the only thing that matters is that c.size() == x.size().
	{
		// To do: This code needs to be amended to deal with possible exceptions 
		// that could occur during the resize call below.
		if(c.empty())
			c.resize(1);
		mBegin = c.begin();
		mEnd   = mBegin;
		mSize  = 0;
	}


	template <typename T, typename Container, typename Allocator>
	ring_buffer<T, Container, Allocator>::ring_buffer(const allocator_type& allocator)
		: c(allocator)
	{
		// To do: This code needs to be amended to deal with possible exceptions 
		// that could occur during the resize call below.

		// We add one because the element at mEnd is necessarily unused.
		c.resize(1); // Possibly we could construct 'c' with size, but c may not have such a ctor, though we rely on it having a resize function.
		mBegin = c.begin();
		mEnd   = mBegin;
		mSize  = 0;
	}


	template <typename T, typename Container, typename Allocator>
	ring_buffer<T, Container, Allocator>::ring_buffer(const this_type& x)
		: c(x.c)
	{
		mBegin = c.begin();
		mEnd   = mBegin;
		mSize  = x.mSize;

		eastl::advance(mBegin, eastl::distance(const_cast<this_type&>(x).c.begin(), x.mBegin)); // We can do a simple distance algorithm here, as there will be no wraparound.
		eastl::advance(mEnd,   eastl::distance(const_cast<this_type&>(x).c.begin(), x.mEnd));
	}

	template <typename T, typename Container, typename Allocator>
	ring_buffer<T, Container, Allocator>::ring_buffer(this_type&& x)
	: c() // Default construction with default allocator for the container.
	{
		c.resize(1); // Possibly we could construct 'c' with size, but c may not have such a ctor, though we rely on it having a resize function.
		mBegin = c.begin();
		mEnd   = mBegin;
		mSize  = 0;

		swap(x); // We are leaving x in an unusual state by swapping default-initialized members with it, as it won't be usable and can be only destructible.
	}

	template <typename T, typename Container, typename Allocator>
	ring_buffer<T, Container, Allocator>::ring_buffer(this_type&& x, const allocator_type& allocator)
		: c(allocator)
	{
		c.resize(1); // Possibly we could construct 'c' with size, but c may not have such a ctor, though we rely on it having a resize function.
		mBegin = c.begin();
		mEnd   = mBegin;
		mSize  = 0;

		if(c.get_allocator() == x.c.get_allocator())
			swap(x); // We are leaving x in an unusual state by swapping default-initialized members with it, as it won't be usable and can be only destructible.
		else
			operator=(x);
	}


	template <typename T, typename Container, typename Allocator>
	ring_buffer<T, Container, Allocator>::ring_buffer(std::initializer_list<value_type> ilist, const allocator_type& allocator)
		: c(allocator)
	{
		c.resize((eastl_size_t)ilist.size() + 1);
		mBegin = c.begin();
		mEnd   = mBegin;
		mSize  = 0;

		assign(ilist.begin(), ilist.end());
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::this_type&
	ring_buffer<T, Container, Allocator>::operator=(const this_type& x)
	{
		if(&x != this)
		{
			c = x.c;

			mBegin = c.begin();
			mEnd   = mBegin;
			mSize  = x.mSize;

			eastl::advance(mBegin, eastl::distance(const_cast<this_type&>(x).c.begin(), x.mBegin)); // We can do a simple distance algorithm here, as there will be no wraparound.
			eastl::advance(mEnd,   eastl::distance(const_cast<this_type&>(x).c.begin(), x.mEnd));
		}

		return *this;
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::this_type&
	ring_buffer<T, Container, Allocator>::operator=(this_type&& x)
	{
		swap(x);
		return *this;
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::this_type&
	ring_buffer<T, Container, Allocator>::operator=(std::initializer_list<value_type> ilist)
	{
		assign(ilist.begin(), ilist.end());
		return *this;
	}


	template <typename T, typename Container, typename Allocator>
	template <typename InputIterator>
	void ring_buffer<T, Container, Allocator>::assign(InputIterator first, InputIterator last)
	{
		// To consider: We can make specializations of this for pointer-based 
		// iterators to PODs and turn the action into a memcpy.
		clear();

		for(; first != last; ++first)
			push_back(*first);
	}


	template <typename T, typename Container, typename Allocator>
	void ring_buffer<T, Container, Allocator>::swap(this_type& x)
	{
		if(&x != this)
		{
			const difference_type dBegin  = eastl::distance(c.begin(), mBegin); // We can do a simple distance algorithm here, as there will be no wraparound.
			const difference_type dEnd    = eastl::distance(c.begin(), mEnd);

			const difference_type dxBegin = eastl::distance(x.c.begin(), x.mBegin);
			const difference_type dxEnd   = eastl::distance(x.c.begin(), x.mEnd);

			eastl::swap(c, x.c);
			eastl::swap(mSize, x.mSize);

			mBegin = c.begin();
			eastl::advance(mBegin, dxBegin); // We can do a simple advance algorithm here, as there will be no wraparound.

			mEnd = c.begin();
			eastl::advance(mEnd, dxEnd);

			x.mBegin = x.c.begin();
			eastl::advance(x.mBegin, dBegin);

			x.mEnd = x.c.begin();
			eastl::advance(x.mEnd, dEnd);
		}
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::iterator
	ring_buffer<T, Container, Allocator>::begin() EA_NOEXCEPT
	{
		return iterator(&c, mBegin);
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::const_iterator
	ring_buffer<T, Container, Allocator>::begin() const EA_NOEXCEPT
	{
		return const_iterator(const_cast<Container*>(&c), mBegin); // We trust that the const_iterator will respect const-ness.
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::const_iterator
	ring_buffer<T, Container, Allocator>::cbegin() const EA_NOEXCEPT
	{
		return const_iterator(const_cast<Container*>(&c), mBegin); // We trust that the const_iterator will respect const-ness.
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::iterator
	ring_buffer<T, Container, Allocator>::end() EA_NOEXCEPT
	{
		return iterator(&c, mEnd);
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::const_iterator
	ring_buffer<T, Container, Allocator>::end() const EA_NOEXCEPT
	{
		return const_iterator(const_cast<Container*>(&c), mEnd); // We trust that the const_iterator will respect const-ness.
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::const_iterator
	ring_buffer<T, Container, Allocator>::cend() const EA_NOEXCEPT
	{
		return const_iterator(const_cast<Container*>(&c), mEnd); // We trust that the const_iterator will respect const-ness.
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::reverse_iterator 
	ring_buffer<T, Container, Allocator>::rbegin() EA_NOEXCEPT
	{
		return reverse_iterator(iterator(&c, mEnd));
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::const_reverse_iterator
	ring_buffer<T, Container, Allocator>::rbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(const_iterator(const_cast<Container*>(&c), mEnd));
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::const_reverse_iterator
	ring_buffer<T, Container, Allocator>::crbegin() const EA_NOEXCEPT
	{
		return const_reverse_iterator(const_iterator(const_cast<Container*>(&c), mEnd));
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::reverse_iterator
	ring_buffer<T, Container, Allocator>::rend() EA_NOEXCEPT
	{
		return reverse_iterator(iterator(&c, mBegin));
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::const_reverse_iterator
	ring_buffer<T, Container, Allocator>::rend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(const_iterator(const_cast<Container*>(&c), mBegin));
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::const_reverse_iterator
	ring_buffer<T, Container, Allocator>::crend() const EA_NOEXCEPT
	{
		return const_reverse_iterator(const_iterator(const_cast<Container*>(&c), mBegin));
	}


	template <typename T, typename Container, typename Allocator>
	bool ring_buffer<T, Container, Allocator>::empty() const EA_NOEXCEPT
	{
		return mBegin == mEnd;
	}


	template <typename T, typename Container, typename Allocator>
	bool ring_buffer<T, Container, Allocator>::full() const EA_NOEXCEPT
	{
		// Implementation that relies on c.size() being a fast operation:
		// return mSize == (c.size() - 1); // (c.size() - 1) == capacity(); we are attempting to reduce function calls.

		// Version that has constant speed guarantees, but is still pretty fast.
		const_iterator afterEnd(end());
		++afterEnd;
		return afterEnd.mContainerIterator == mBegin;
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::size_type
	ring_buffer<T, Container, Allocator>::size() const EA_NOEXCEPT
	{
		return mSize;

		// Alternatives:
		// return eastl::distance(begin(), end());
		// return end() - begin(); // This is more direct than using distance().
		//typedef typename eastl::iterator_traits<container_iterator>::iterator_category IC;
		//return DoGetSize(IC()); // This is more direct than using iterator math.
	}


	/*
	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::size_type
	ring_buffer<T, Container, Allocator>::DoGetSize(EASTL_ITC_NS::input_iterator_tag) const
	{
		// We could alternatively just use eastl::distance() here, but we happen to
		// know that such code would boil down to what we have here, and we might
		// as well remove function calls where possible. 
		difference_type d = 0;

		for(const_iterator temp(begin()), tempEnd(end()); temp != tempEnd; ++temp)
			++d;

		return (size_type)d;
	}
	*/

	/*
	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::size_type
	ring_buffer<T, Container, Allocator>::DoGetSize(EASTL_ITC_NS::random_access_iterator_tag) const
	{
		// A simpler but less efficient implementation fo this function would be:
		//     return eastl::distance(mBegin, mEnd);
		//
		// The calculation of distance here takes advantage of the fact that random
		// access iterators' distances can be calculated by simple pointer calculation.
		// Thus the code below boils down to a few subtractions when using a vector,
		// string, or array as the Container type. 
		//
		const difference_type dBegin = eastl::distance(const_cast<Container&>(c).begin(), mBegin); // const_cast here solves a little compiler 
		const difference_type dEnd   = eastl::distance(const_cast<Container&>(c).begin(), mEnd);   // argument matching problem.

		if(dEnd >= dBegin)
			return dEnd - dBegin;

		return c.size() - (dBegin - dEnd);
	}
	*/


	namespace Internal 
	{
		///////////////////////////////////////////////////////////////
		// has_overflow_allocator
		//
		// returns true_type when the specified container type is an 
		// eastl::fixed_*  container and therefore has an overflow 
		// allocator type.
		//
		template <typename T, typename = void>
		struct has_overflow_allocator : false_type {};

		template <typename T>
		struct has_overflow_allocator<T, void_t<decltype(declval<T>().get_overflow_allocator())>> : true_type {};


		///////////////////////////////////////////////////////////////
		// GetFixedContainerCtorAllocator
		//
		// eastl::fixed_* containers are only constructible via their 
		// overflow allocator type. This helper select the appropriate 
		// allocator from the specified container.
		//
		template <typename Container, bool UseOverflowAllocator = has_overflow_allocator<Container>()()>
		struct GetFixedContainerCtorAllocator
		{
			auto& operator()(Container& c) { return c.get_overflow_allocator(); }
		};

		template <typename Container>
		struct GetFixedContainerCtorAllocator<Container, false>
		{
			auto& operator()(Container& c) { return c.get_allocator(); }
		};
	} // namespace Internal


	///////////////////////////////////////////////////////////////
	// ContainerTemporary
	// 
	// Helper type which prevents utilizing excessive stack space 
	// when creating temporaries when swapping/copying the underlying 
	// ring_buffer container type. 
	//
	template <typename Container, bool UseHeapTemporary = (sizeof(Container) >= EASTL_MAX_STACK_USAGE)>
	struct ContainerTemporary
	{
		Container mContainer;

		ContainerTemporary(Container& parentContainer)
		    : mContainer(Internal::GetFixedContainerCtorAllocator<Container>{}(parentContainer))
		{
		}

		Container& get() { return mContainer; }
	};

	template <typename Container>
	struct ContainerTemporary<Container, true>
	{
		typename Container::allocator_type* mAllocator;
		Container* mContainer;

		ContainerTemporary(Container& parentContainer)
		    : mAllocator(&parentContainer.get_allocator())
		    , mContainer(new (mAllocator->allocate(sizeof(Container))) Container)
		{
		}

		~ContainerTemporary()
		{
			mContainer->~Container();
			mAllocator->deallocate(mContainer, sizeof(Container));
		}

		Container& get() { return *mContainer; }
	};


	template <typename T, typename Container, typename Allocator>
	void ring_buffer<T, Container, Allocator>::resize(size_type n)
	{
		// Note that if n > size(), we just move the end position out to
		// the begin + n, with the data being the old end and the new end
		// being stale values from the past. This is by design, as the concept
		// of arbitrarily resizing a ring buffer like this is currently deemed
		// to be vague in what it intends to do. We can only assume that the
		// user knows what he is doing and will deal with the stale values.
		EASTL_ASSERT(c.size() >= 1);
		const size_type cap = (c.size() - 1);

		mSize = n;

		if(n > cap) // If we need to grow in capacity...
		{
			// Given that a growing operation will always result in memory allocation, 
			// we currently implement this function via the usage of a temp container.
			// This makes for a simple implementation, but in some cases it is less
			// efficient. In particular, if the container is a node-based container like
			// a (linked) list, this function would be faster if we simply added nodes
			// to ourself. We would do this by inserting the nodes to be after end()
			// and adjusting the begin() position if it was after end().

			// To do: This code needs to be amended to deal with possible exceptions 
			// that could occur during the resize call below.

			ContainerTemporary<Container> cTemp(c);
			cTemp.get().resize(n + 1);
			eastl::copy(begin(), end(), cTemp.get().begin());
			eastl::swap(c, cTemp.get());

			mBegin = c.begin();
			mEnd   = mBegin;
			eastl::advance(mEnd, n); // We can do a simple advance algorithm on this because we know that mEnd will not wrap around.
		}
		else // We could do a check here for n != size(), but that would be costly and people don't usually resize things to their same size.
		{
			mEnd = mBegin;

			// eastl::advance(mEnd, n); // We *cannot* use this because there may be wraparound involved.

			// To consider: Possibly we should implement some more detailed logic to optimize the code here.
			// We'd need to do different behaviour dending on whether the container iterator type is a 
			// random access iterator or otherwise.

			while(n--)
			{
				if(EASTL_UNLIKELY(++mEnd == c.end()))
					mEnd = c.begin();
			}
		}
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::size_type
	ring_buffer<T, Container, Allocator>::capacity() const EA_NOEXCEPT
	{
		EASTL_ASSERT(c.size() >= 1); // This is required because even an empty ring_buffer has one unused termination element, somewhat like a \0 at the end of a C string.

		return (c.size() - 1); // Need to subtract one because the position at mEnd is unused.
	}


	template <typename T, typename Container, typename Allocator>
	void ring_buffer<T, Container, Allocator>::set_capacity(size_type n)
	{
		const size_type capacity = (c.size() - 1);

		if(n != capacity)    // If we need to change capacity...
		{
			ContainerTemporary<Container> cTemp(c);
			cTemp.get().resize(n + 1);

			iterator itCopyBegin = begin();

			if(n < mSize) // If we are shrinking the capacity, to less than our size...
			{
				eastl::advance(itCopyBegin, mSize - n);
				mSize = n;
			}

			eastl::copy(itCopyBegin, end(), cTemp.get().begin());  // The begin-end range may in fact be larger than n, in which case values will be overwritten.
			eastl::swap(c, cTemp.get());

			mBegin = c.begin();
			mEnd   = mBegin;
			eastl::advance(mEnd, mSize); // We can do a simple advance algorithm on this because we know that mEnd will not wrap around.
		}
	}


	template <typename T, typename Container, typename Allocator>
	void ring_buffer<T, Container, Allocator>::reserve(size_type n)
	{
		// We follow the pattern of vector and only do something if n > capacity.
		EASTL_ASSERT(c.size() >= 1);

		if(n > (c.size() - 1))    // If we need to grow in capacity... // (c.size() - 1) == capacity(); we are attempting to reduce function calls.
		{
			ContainerTemporary<Container> cTemp(c);
			cTemp.get().resize(n + 1);
			eastl::copy(begin(), end(), cTemp.get().begin());
			eastl::swap(c, cTemp.get());

			mBegin = c.begin();
			mEnd   = mBegin;
			eastl::advance(mEnd, mSize); // We can do a simple advance algorithm on this because we know that mEnd will not wrap around.
		}
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::reference
	ring_buffer<T, Container, Allocator>::front()
	{
		return *mBegin;
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::const_reference
	ring_buffer<T, Container, Allocator>::front() const
	{
		return *mBegin;
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::reference
	ring_buffer<T, Container, Allocator>::back()
	{
		// return *(end() - 1); // Can't use this because not all iterators support operator-.

		iterator temp(end());   // To do: Find a way to construct this temporary in the return statement.
		return *(--temp);       // We can do it by making all our containers' iterators support operator-.
	}

		
	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::const_reference
	ring_buffer<T, Container, Allocator>::back() const
	{
		// return *(end() - 1); // Can't use this because not all iterators support operator-.

		const_iterator temp(end()); // To do: Find a way to construct this temporary in the return statement.
		return *(--temp);           // We can do it by making all our containers' iterators support operator-.
	}


	/// A push_back operation on a ring buffer assigns the new value to end.  
	/// If there is no more space in the buffer, this will result in begin
	/// being overwritten and the begin position being moved foward one position.
	template <typename T, typename Container, typename Allocator>
	void ring_buffer<T, Container, Allocator>::push_back(const value_type& value)
	{
		*mEnd = value;

		if(++mEnd == c.end())
			mEnd = c.begin();

		if(mEnd == mBegin)
		{
			if(++mBegin == c.end())
				mBegin = c.begin();
		}
		else
			++mSize;
	}


	/// A push_back operation on a ring buffer assigns the new value to end.  
	/// If there is no more space in the buffer, this will result in begin
	/// being overwritten and the begin position being moved foward one position.
	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::reference
	ring_buffer<T, Container, Allocator>::push_back()
	{
		// We don't do the following assignment, as the value at mEnd is already constructed;
		// it is merely possibly not default-constructed. However, the spirit of push_back
		// is that the user intends to do an assignment or data modification after the 
		// push_back call. The user can always execute *back() = value_type() if he wants.
		//*mEnd = value_type();

		if(++mEnd == c.end())
			mEnd = c.begin();

		if(mEnd == mBegin)
		{
			if(++mBegin == c.end())
				mBegin = c.begin();
		}
		else
			++mSize;

		return back();
	}


	template <typename T, typename Container, typename Allocator>
	void ring_buffer<T, Container, Allocator>::pop_back()
	{
		EASTL_ASSERT(mEnd != mBegin); // We assume that size() > 0 and thus that there is something to pop.

		if(EASTL_UNLIKELY(mEnd == c.begin()))
			mEnd = c.end();
		--mEnd;
		--mSize;
	}


	template <typename T, typename Container, typename Allocator>
	void ring_buffer<T, Container, Allocator>::push_front(const value_type& value)
	{
		if(EASTL_UNLIKELY(mBegin == c.begin()))
			mBegin = c.end();

		if(--mBegin == mEnd)
		{
			if(EASTL_UNLIKELY(mEnd == c.begin()))
				mEnd = c.end();
			--mEnd;
		}
		else
			++mSize;

		*mBegin = value;
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::reference
	ring_buffer<T, Container, Allocator>::push_front()
	{
		if(EASTL_UNLIKELY(mBegin == c.begin()))
			mBegin = c.end();

		if(--mBegin == mEnd)
		{
			if(EASTL_UNLIKELY(mEnd == c.begin()))
				mEnd = c.end();
			--mEnd;
		}
		else
			++mSize;

		// See comments above in push_back for why we don't execute this:
		// *mBegin = value_type();

		return *mBegin; // Same as return front();
	}


	template <typename T, typename Container, typename Allocator>
	void ring_buffer<T, Container, Allocator>::pop_front()
	{
		EASTL_ASSERT(mBegin != mEnd); // We assume that mEnd > mBegin and thus that there is something to pop.

		if(++mBegin == c.end())
			mBegin = c.begin();
		--mSize;
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::reference
	ring_buffer<T, Container, Allocator>::operator[](size_type n)
	{
		// return *(begin() + n); // Can't use this because not all iterators support operator+.

		// This should compile to code that is nearly as efficient as that above.
		// The primary difference is the possible generation of a temporary in this case.
		iterator temp(begin());
		eastl::advance(temp, n);
		return *(temp.mContainerIterator);
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::const_reference
	ring_buffer<T, Container, Allocator>::operator[](size_type n) const
	{
		// return *(begin() + n); // Can't use this because not all iterators support operator+.

		// This should compile to code that is nearly as efficient as that above.
		// The primary difference is the possible generation of a temporary in this case.
		const_iterator temp(begin());
		eastl::advance(temp, n);
		return *(temp.mContainerIterator);
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::iterator
	ring_buffer<T, Container, Allocator>::insert(const_iterator position, const value_type& value)
	{
		// To consider: It would be faster if we could tell that position was in the first
		// half of the container and instead of moving things after the position back, 
		// we could move things before the position forward. 

		iterator afterEnd(end());
		iterator beforeEnd(afterEnd);

		++afterEnd;

		if(afterEnd.mContainerIterator == mBegin) // If we are at full capacity...
			--beforeEnd;
		else
			push_back();

		iterator itPosition(position.mpContainer, position.mContainerIterator); // We merely copy from const_iterator to iterator.
		eastl::copy_backward(itPosition, beforeEnd, end());
		*itPosition = value;

		return itPosition;
	}


	template <typename T, typename Container, typename Allocator>
	void ring_buffer<T, Container, Allocator>::insert(const_iterator position, size_type n, const value_type& value)
	{
		// To do: This can be improved with a smarter version. However, 
		// this is a little tricky because we need to deal with the case 
		// whereby n is greater than the size of the container itself. 
		while(n--)
			insert(position, value);
	}


	template <typename T, typename Container, typename Allocator>
	void ring_buffer<T, Container, Allocator>::insert(const_iterator position, std::initializer_list<value_type> ilist)
	{
		insert(position, ilist.begin(), ilist.end());
	}


	template <typename T, typename Container, typename Allocator>
	template <typename InputIterator>
	void ring_buffer<T, Container, Allocator>::insert(const_iterator position, InputIterator first, InputIterator last)
	{
		// To do: This can possibly be improved with a smarter version.
		// However, this can be tricky if distance(first, last) is greater
		// than the size of the container itself.
		for(; first != last; ++first, ++position)
			insert(position, *first);
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::iterator
	ring_buffer<T, Container, Allocator>::erase(const_iterator position)
	{
		iterator itPosition(position.mpContainer, position.mContainerIterator); // We merely copy from const_iterator to iterator.
		iterator iNext(itPosition);

		eastl::copy(++iNext, end(), itPosition);
		pop_back();

		return itPosition;
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::iterator
	ring_buffer<T, Container, Allocator>::erase(const_iterator first, const_iterator last)
	{
		iterator itFirst(first.mpContainer, first.mContainerIterator); // We merely copy from const_iterator to iterator.
		iterator itLast(last.mpContainer, last.mContainerIterator);

		typename iterator::difference_type d = eastl::distance(itFirst, itLast);

		eastl::copy(itLast, end(), itFirst);

		while(d--)      // To do: improve this implementation.
			pop_back();

		return itFirst;
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::reverse_iterator
	ring_buffer<T, Container, Allocator>::erase(const_reverse_iterator position)
	{
		return reverse_iterator(erase((++position).base()));
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::reverse_iterator
	ring_buffer<T, Container, Allocator>::erase(const_reverse_iterator first, const_reverse_iterator last)
	{
		// Version which erases in order from first to last.
		// difference_type i(first.base() - last.base());
		// while(i--)
		//     first = erase(first);
		// return first;

		// Version which erases in order from last to first, but is slightly more efficient:
		return reverse_iterator(erase((++last).base(), (++first).base()));
	}


	template <typename T, typename Container, typename Allocator>
	void ring_buffer<T, Container, Allocator>::clear()
	{
		// Don't clear the container; we use its valid data for our elements.
		mBegin = c.begin();
		mEnd   = c.begin();
		mSize  = 0;
	}


	template <typename T, typename Container, typename Allocator>
	typename ring_buffer<T, Container, Allocator>::container_type&
	ring_buffer<T, Container, Allocator>::get_container()
	{
		return c;
	}


	template <typename T, typename Container, typename Allocator>
	const typename ring_buffer<T, Container, Allocator>::container_type&
	ring_buffer<T, Container, Allocator>::get_container() const
	{
		return c;
	}


	template <typename T, typename Container, typename Allocator>
	inline bool ring_buffer<T, Container, Allocator>::validate() const
	{
		if(!c.validate())  // This requires that the container implement the validate function. That pretty much 
			return false;  // means that the container is an EASTL container and not a std STL container. 

		if(c.empty())      // c must always have a size of at least 1, as even an empty ring_buffer has an unused terminating element.
			return false;

		if(size() > capacity())
			return false;

		if((validate_iterator(begin()) & (isf_valid | isf_current)) != (isf_valid | isf_current))
			return false;

		if((validate_iterator(end()) & (isf_valid | isf_current)) != (isf_valid | isf_current))
			return false;

		// Verify that the size calculation is consistent.
		size_type n = 0;
		for(const_iterator i(begin()), iEnd(end()); i != iEnd; ++i)
			++n;
		if(n != mSize)
			return false;

		return true;
	}


	template <typename T, typename Container, typename Allocator>
	inline int ring_buffer<T, Container, Allocator>::validate_iterator(const_iterator i) const
	{
		// To do: Replace this with a more efficient implementation if possible.

		for(const_iterator temp = begin(), tempEnd = end(); temp != tempEnd; ++temp)
		{
			if(temp == i)
				return (isf_valid | isf_current | isf_can_dereference);
		}

		if(i == end())
			return (isf_valid | isf_current); 

		return isf_none;
	}



	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Container, typename Allocator>
	inline bool operator==(const ring_buffer<T, Container, Allocator>& a, const ring_buffer<T, Container, Allocator>& b)
	{
		return (a.size() == b.size()) && eastl::equal(a.begin(), a.end(), b.begin());
	}


	template <typename T, typename Container, typename Allocator>
	inline bool operator<(const ring_buffer<T, Container, Allocator>& a, const ring_buffer<T, Container, Allocator>& b)
	{
		return eastl::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
	}


	template <typename T, typename Container, typename Allocator>
	inline bool operator!=(const ring_buffer<T, Container, Allocator>& a, const ring_buffer<T, Container, Allocator>& b)
	{
		return !(a == b);
	}


	template <typename T, typename Container, typename Allocator>
	inline bool operator>(const ring_buffer<T, Container, Allocator>& a, const ring_buffer<T, Container, Allocator>& b)
	{
		return (b < a);
	}


	template <typename T, typename Container, typename Allocator>
	inline bool operator<=(const ring_buffer<T, Container, Allocator>& a, const ring_buffer<T, Container, Allocator>& b)
	{
		return !(b < a);
	}


	template <typename T, typename Container, typename Allocator>
	inline bool operator>=(const ring_buffer<T, Container, Allocator>& a, const ring_buffer<T, Container, Allocator>& b)
	{
		return !(a < b);
	}


	template <typename T, typename Container, typename Allocator>
	inline void swap(ring_buffer<T, Container, Allocator>& a, ring_buffer<T, Container, Allocator>& b)
	{
		a.swap(b);
	}


} // namespace eastl


#endif // Header include guard



















