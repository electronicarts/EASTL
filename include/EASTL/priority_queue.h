///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements a priority_queue that is just like the C++ 
// std::priority_queue adapter class, except it has a couple extension functions.
// The primary distinctions between this priority_queue and std::priority_queue are:
//    - priority_queue has a couple extension functions that allow you to 
//      use a priority queue in extra ways. See the code for documentation.
//    - priority_queue can contain objects with alignment requirements. 
//      std::priority_queue cannot do so without a bit of tedious non-portable effort.
//    - priority_queue supports debug memory naming natively.
//    - priority_queue is easier to read, debug, and visualize.
//    - priority_queue is savvy to an environment that doesn't have exception handling,
//      as is sometimes the case with console or embedded environments.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_PRIORITY_QUEUE_H
#define EASTL_PRIORITY_QUEUE_H


#include <EASTL/internal/config.h>
#include <EASTL/vector.h>
#include <EASTL/heap.h>
#include <EASTL/functional.h>
#include <EASTL/initializer_list.h>
#include <stddef.h>


#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4530)  // C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
	#pragma warning(disable: 4571)  // catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught.
#endif

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// EASTL_PRIORITY_QUEUE_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_PRIORITY_QUEUE_DEFAULT_NAME
		#define EASTL_PRIORITY_QUEUE_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " priority_queue" // Unless the user overrides something, this is "EASTL priority_queue".
	#endif

	/// EASTL_PRIORITY_QUEUE_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_PRIORITY_QUEUE_DEFAULT_ALLOCATOR
		#define EASTL_PRIORITY_QUEUE_DEFAULT_ALLOCATOR allocator_type(EASTL_PRIORITY_QUEUE_DEFAULT_NAME)
	#endif



	/// priority_queue
	///
	/// The behaviour of this class is just like the std::priority_queue
	/// class and you can refer to std documentation on it.
	///
	/// A priority_queue is an adapter container which implements a
	/// queue-like container whereby pop() returns the item of highest
	/// priority. The entire queue isn't necessarily sorted; merely the 
	/// first item in the queue happens to be of higher priority than 
	/// other items. You can read about priority_queues in many books
	/// on algorithms, such as "Algorithms" by Robert Sedgewick.
	///
	/// The Container type is a container which is random access and 
	/// supports empty(), size(), clear(), insert(), front(), 
	/// push_back(), and pop_back(). You would typically use vector
	/// or deque.
	/// 
	/// Note that we don't provide functions in the priority_queue 
	/// interface for working with allocators or names. The reason for 
	/// this is that priority_queue is an adapter class which can work
	/// with any standard sequence and not necessarily just a sequence 
	/// provided by this library. So what we do is provide a member 
	/// accessor function get_container() which allows the user to 
	/// manipulate the sequence as needed. The user needs to be careful
	/// not to change the container's contents, however.
	///
	/// Classic heaps allow for the concept of removing arbitrary items
	/// and changing the priority of arbitrary items, though the C++
	/// std heap (and thus priority_queue) functions don't support 
	/// these operations. We have extended the heap algorithms and the 
	/// priority_queue implementation to support these operations.
	///
	///////////////////////////////////////////////////////////////////

	template <typename T, typename Container = eastl::vector<T>, typename Compare = eastl::less<typename Container::value_type> >
	class priority_queue
	{
	public:
		typedef priority_queue<T, Container, Compare>        this_type;
		typedef Container                                    container_type;
		typedef Compare                                      compare_type;
	  //typedef typename Container::allocator_type           allocator_type;  // We can't currently declare this because the container may be a type that doesn't have an allocator. 
		typedef typename Container::value_type               value_type;
		typedef typename Container::reference                reference;
		typedef typename Container::const_reference          const_reference;
		typedef typename Container::size_type                size_type;
		typedef typename Container::difference_type          difference_type;

	public:                   // We declare public so that global comparison operators can be implemented without adding an inline level and without tripping up GCC 2.x friend declaration failures. GCC (through at least v4.0) is poor at inlining and performance wins over correctness.
		container_type  c;    // The C++ standard specifies that you declare a protected member variable of type Container called 'c'.
		compare_type    comp; // The C++ standard specifies that you declare a protected member variable of type Compare called 'comp'.

	public:
		priority_queue();

		// Allocator is templated here because we aren't allowed to infer the allocator_type from the Container, as some containers (e.g. array) don't 
		// have allocators. For containers that don't have allocator types, you could use void or char as the Allocator template type.

		template <class Allocator>                      
		explicit priority_queue(const Allocator& allocator, typename eastl::enable_if<eastl::uses_allocator<container_type, Allocator>::value>::type* = NULL)
		   : c(allocator), comp()
		{
		}    

		template <class Allocator>
		priority_queue(const this_type& x, const Allocator& allocator, typename eastl::enable_if<eastl::uses_allocator<container_type, Allocator>::value>::type* = NULL)
			: c(x.c, allocator), comp(x.comp)
		{
			eastl::make_heap(c.begin(), c.end(), comp);
		}

		template <class Allocator>
		priority_queue(this_type&& x, const Allocator& allocator, typename eastl::enable_if<eastl::uses_allocator<container_type, Allocator>::value>::type* = NULL)
			: c(eastl::move(x.c), allocator), comp(x.comp)
		{
			eastl::make_heap(c.begin(), c.end(), comp);
		}

		explicit priority_queue(const compare_type& compare);
		explicit priority_queue(const compare_type& compare, container_type&& x);

		priority_queue(const compare_type& compare, const container_type& x);
		priority_queue(std::initializer_list<value_type> ilist, const compare_type& compare = compare_type()); // C++11 doesn't specify that std::priority_queue has initializer list support.

		template <typename InputIterator>
		priority_queue(InputIterator first, InputIterator last);

		template <typename InputIterator>
		priority_queue(InputIterator first, InputIterator last, const compare_type& compare);

		template <typename InputIterator>
		priority_queue(InputIterator first, InputIterator last, const compare_type& compare, const container_type& x);

		template <class InputIterator>
		priority_queue(InputIterator first, InputIterator last, const compare_type& compare, container_type&& x);

		// Additional C++11 support to consider:
		//
		// template <class Allocator>
		// priority_queue(const Compare&, const Allocator&);
		//
		// template <class Allocator>
		// priority_queue(const Compare&, const container_type&, const Allocator&);
		//
		// template <class Allocator>
		// priority_queue(const Compare&, container_type&&, const Allocator&);

		bool      empty() const;
		size_type size() const;

		const_reference top() const;

		void push(const value_type& value);

		void push(value_type&& x);

		template <class... Args>
		void emplace(Args&&... args);

		void pop();

		void pop(value_type& value);    // Extension to the C++11 Standard that allows popping a move-only type (e.g. unique_ptr).

		void change(size_type n);   /// Moves the item at the given array index to a new location based on its current priority.
		void remove(size_type n);   /// Removes the item at the given array index.

		container_type&       get_container();
		const container_type& get_container() const;

		void swap(this_type& x) EA_NOEXCEPT_IF((eastl::is_nothrow_swappable<this_type::container_type>::value && eastl::is_nothrow_swappable<this_type::compare_type>::value));
		
		bool validate() const;

	}; // class priority_queue




	///////////////////////////////////////////////////////////////////////
	// priority_queue
	///////////////////////////////////////////////////////////////////////


	template <typename T, typename Container, typename Compare>
	inline priority_queue<T, Container, Compare>::priority_queue()
		: c(),  // To consider: use c(EASTL_PRIORITY_QUEUE_DEFAULT_ALLOCATOR) here, though that would add the requirement that the user supplied container support this.
		  comp()
	{
	}


	template <typename T, typename Container, typename Compare>
	inline priority_queue<T, Container, Compare>::priority_queue(const compare_type& compare)
		: c(),  // To consider: use c(EASTL_PRIORITY_QUEUE_DEFAULT_ALLOCATOR) here, though that would add the requirement that the user supplied container support this.
		  comp(compare)
	{
	}


	template <typename T, typename Container, typename Compare>
	inline priority_queue<T, Container, Compare>::priority_queue(const compare_type& compare, const container_type& x)
		: c(x), comp(compare)
	{
		eastl::make_heap(c.begin(), c.end(), comp);
	}


	template <typename T, typename Container, typename Compare>
	inline priority_queue<T, Container, Compare>::priority_queue(const compare_type& compare, container_type&& x)
	  : c(eastl::move(x)), comp(compare)
	{
		eastl::make_heap(c.begin(), c.end(), comp);
	}


	template <typename T, typename Container, typename Compare>
	inline priority_queue<T, Container, Compare>::priority_queue(std::initializer_list<value_type> ilist, const compare_type& compare)
		: c(), comp(compare)
	{
		c.insert(c.end(), ilist.begin(), ilist.end());
		eastl::make_heap(c.begin(), c.end(), comp);
	}



	template <typename T, typename Container, typename Compare>
	template <typename InputIterator>
	inline priority_queue<T, Container, Compare>::priority_queue(InputIterator first, InputIterator last)
		: c(first, last), comp()
	{
		eastl::make_heap(c.begin(), c.end(), comp);
	}


	template <typename T, typename Container, typename Compare>
	template <typename InputIterator>
	inline priority_queue<T, Container, Compare>::priority_queue(InputIterator first, InputIterator last, const compare_type& compare)
		: c(first, last), comp(compare)
	{
		eastl::make_heap(c.begin(), c.end(), comp);
	}


	template <typename T, typename Container, typename Compare>
	template <typename InputIterator>
	inline priority_queue<T, Container, Compare>::priority_queue(InputIterator first, InputIterator last, const compare_type& compare, const container_type& x)
		: c(x), comp(compare)
	{
		c.insert(c.end(), first, last);
		eastl::make_heap(c.begin(), c.end(), comp);
	}


	template <typename T, typename Container, typename Compare>
	template <typename InputIterator>
	inline priority_queue<T, Container, Compare>::priority_queue(InputIterator first, InputIterator last, const compare_type& compare, container_type&& x)
		: c(eastl::move(x)), comp(compare)
	{
		c.insert(c.end(), first, last);
		eastl::make_heap(c.begin(), c.end(), comp);
	}


	template <typename T, typename Container, typename Compare>
	inline bool priority_queue<T, Container, Compare>::empty() const
	{
		return c.empty();
	}


	template <typename T, typename Container, typename Compare>
	inline typename priority_queue<T, Container, Compare>::size_type
	priority_queue<T, Container, Compare>::size() const
	{
		return c.size();
	}


	template <typename T, typename Container, typename Compare>
	inline typename priority_queue<T, Container, Compare>::const_reference
	priority_queue<T, Container, Compare>::top() const
	{
		return c.front();
	}


	template <typename T, typename Container, typename Compare>
	inline void priority_queue<T, Container, Compare>::push(const value_type& value)
	{
		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
				c.push_back(value);
				eastl::push_heap(c.begin(), c.end(), comp);
			}
			catch(...)
			{
				c.clear();
				throw;
			}
		#else
			c.push_back(value);
			eastl::push_heap(c.begin(), c.end(), comp);
		#endif
	}


	template <typename T, typename Container, typename Compare>
	inline void priority_queue<T, Container, Compare>::push(value_type&& value)
	{
		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
				c.push_back(eastl::move(value));
				eastl::push_heap(c.begin(), c.end(), comp);
			}
			catch(...)
			{
				c.clear();
				throw;
			}
		#else
			c.push_back(eastl::move(value));
			eastl::push_heap(c.begin(), c.end(), comp);
		#endif
	}


	template <typename T, typename Container, typename Compare>
	template <class... Args>
	inline void priority_queue<T, Container, Compare>::emplace(Args&&... args)
	{
		push(value_type(eastl::forward<Args>(args)...)); // The C++11 Standard 23.6.4/1 states that c.emplace is used, but also declares that c doesn't need to have an emplace function.
	}


	template <typename T, typename Container, typename Compare>
	inline void priority_queue<T, Container, Compare>::pop()
	{
		#if EASTL_EXCEPTIONS_ENABLED
			try
			{
				eastl::pop_heap(c.begin(), c.end(), comp);
				c.pop_back();
			}
			catch(...)
			{
				c.clear();
				throw;
			}
		#else
			eastl::pop_heap(c.begin(), c.end(), comp);
			c.pop_back();
		#endif
	}


	template <typename T, typename Container, typename Compare>
	inline void priority_queue<T, Container, Compare>::pop(value_type& value)
	{
		value = eastl::move(c.front());  // To consider: value = move_if_noexcept_assignable(c.front());
		pop();
	}


	template <typename T, typename Container, typename Compare>
	inline void priority_queue<T, Container, Compare>::change(size_type n) // This function is not in the STL std::priority_queue.
	{
		eastl::change_heap(c.begin(), c.size(), n, comp);
	}


	template <typename T, typename Container, typename Compare>
	inline void priority_queue<T, Container, Compare>::remove(size_type n) // This function is not in the STL std::priority_queue.
	{
		eastl::remove_heap(c.begin(), c.size(), n, comp);
		c.pop_back();
	}


	template <typename T, typename Container, typename Compare>
	inline typename priority_queue<T, Container, Compare>::container_type&
	priority_queue<T, Container, Compare>::get_container()
	{
		return c;
	}


	template <typename T, typename Container, typename Compare>
	inline const typename priority_queue<T, Container, Compare>::container_type&
	priority_queue<T, Container, Compare>::get_container() const
	{
		return c;
	}


	template <typename T, typename Container, typename Compare>
	inline void priority_queue<T, Container, Compare>::swap(this_type& x) EA_NOEXCEPT_IF((eastl::is_nothrow_swappable<this_type::container_type>::value && 
																						  eastl::is_nothrow_swappable<this_type::compare_type>::value))
	{
		using eastl::swap;
		swap(c, x.c);
		swap(comp, x.comp);
	}


	template <typename T, typename Container, typename Compare>
	inline bool
	priority_queue<T, Container, Compare>::validate() const
	{
		return c.validate() && eastl::is_heap(c.begin(), c.end(), comp);
	}



	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Container, typename Compare>
	bool operator==(const priority_queue<T, Container, Compare>& a, const priority_queue<T, Container, Compare>& b)
	{
		return (a.c == b.c);
	}

	template <typename T, typename Container, typename Compare>
	bool operator<(const priority_queue<T, Container, Compare>& a, const priority_queue<T, Container, Compare>& b)
	{
		return (a.c < b.c);
	}

	template <typename T, typename Container, typename Compare>
	inline bool operator!=(const priority_queue<T, Container, Compare>& a, const priority_queue<T, Container, Compare>& b)
	{
		return !(a.c == b.c);
	}

	template <typename T, typename Container, typename Compare>
	inline bool operator>(const priority_queue<T, Container, Compare>& a, const priority_queue<T, Container, Compare>& b)
	{
		return (b.c < a.c);
	}

	template <typename T, typename Container, typename Compare>
	inline bool operator<=(const priority_queue<T, Container, Compare>& a, const priority_queue<T, Container, Compare>& b)
	{
		return !(b.c < a.c);
	}

	template <typename T, typename Container, typename Compare>
	inline bool operator>=(const priority_queue<T, Container, Compare>& a, const priority_queue<T, Container, Compare>& b)
	{
		return !(a.c < b.c);
	}


	template <class T, class Container, class Compare>
	inline void swap(priority_queue<T, Container, Compare>& a,  priority_queue<T, Container, Compare>& b) EA_NOEXCEPT_IF((eastl::is_nothrow_swappable<typename priority_queue<T, Container, Compare>::container_type>::value && 
																														  eastl::is_nothrow_swappable<typename priority_queue<T, Container, Compare>::compare_type>::value)) // EDG has a bug and won't let us use Container in this noexcept statement
	{
		a.swap(b);
	}


} // namespace eastl


#ifdef _MSC_VER
	#pragma warning(pop)
#endif


#endif // Header include guard













