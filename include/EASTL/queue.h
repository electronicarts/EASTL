///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements a queue that is just like the C++ std::queue adapter class.
// There are no significant differences between EASTL/queue and std::queue.
// We provide this class for completeness and where std STL may not be available.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_QUEUE_H
#define EASTL_QUEUE_H


#include <EASTL/internal/config.h>
#include <EASTL/deque.h>
#include <EASTL/initializer_list.h>
#include <stddef.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace eastl
{

	/// EASTL_QUEUE_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_QUEUE_DEFAULT_NAME
		#define EASTL_QUEUE_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " queue" // Unless the user overrides something, this is "EASTL queue".
	#endif

	/// EASTL_QUEUE_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_QUEUE_DEFAULT_ALLOCATOR
		#define EASTL_QUEUE_DEFAULT_ALLOCATOR allocator_type(EASTL_QUEUE_DEFAULT_NAME)
	#endif



	/// queue
	///
	/// queue is an adapter class provides a FIFO (first-in, first-out) interface
	/// via wrapping a sequence that provides at least the following operations:
	///     push_back
	///     pop_front
	///     front
	///     back
	///
	/// In practice this usually means deque, list, intrusive_list. vector and string  
	/// cannot be used because they don't provide pop-front. This is reasonable because
	/// a vector or string pop_front would be inefficient and could lead to 
	/// silently poor performance.
	///
	template <typename T, typename Container = eastl::deque<T, EASTLAllocatorType, DEQUE_DEFAULT_SUBARRAY_SIZE(T)> >
	class queue
	{
	public:
		typedef queue<T, Container>                  this_type;
		typedef          Container                   container_type;
	  //typedef typename Container::allocator_type   allocator_type;  // We can't currently declare this because the container may be a type that doesn't have an allocator. 
		typedef typename Container::value_type       value_type;
		typedef typename Container::reference        reference;
		typedef typename Container::const_reference  const_reference;
		typedef typename Container::size_type        size_type;

	public:               // We declare public so that global comparison operators can be implemented without adding an inline level and without tripping up GCC 2.x friend declaration failures. GCC (through at least v4.0) is poor at inlining and performance wins over correctness.
		container_type c; // The C++ standard specifies that you declare a protected member variable of type Container called 'c'.

	public:
		queue();

		// Allocator is templated here because we aren't allowed to infer the allocator_type from the Container, as some containers (e.g. array) don't 
		// have allocators. For containers that don't have allocator types, you could use void or char as the Allocator template type.

		template <class Allocator>                      
		explicit queue(const Allocator& allocator, typename eastl::enable_if<eastl::uses_allocator<container_type, Allocator>::value>::type* = NULL)
		  : c(allocator)
		{
		}    

		template <class Allocator>
		queue(const this_type& x, const Allocator& allocator, typename eastl::enable_if<eastl::uses_allocator<container_type, Allocator>::value>::type* = NULL)
		  : c(x.c, allocator)
		{
		}

		#if EASTL_MOVE_SEMANTICS_ENABLED
			template <class Allocator>
			queue(this_type&& x, const Allocator& allocator, typename eastl::enable_if<eastl::uses_allocator<container_type, Allocator>::value>::type* = NULL)
			  : c(eastl::move(x.c), allocator)
			{
			}
		#endif

		explicit queue(const container_type& x);

		#if EASTL_MOVE_SEMANTICS_ENABLED
			explicit queue(container_type&& x);
		#endif

		// Additional C++11 support to consider:
		//
		// template <class Allocator>
		// queue(const container_type& x, const Allocator& allocator);
		//
		// template <class Allocator>
		// queue(container_type&& x, const Allocator& allocator);

		queue(std::initializer_list<value_type> ilist); // C++11 doesn't specify that std::queue has initializer list support.

		bool      empty() const;
		size_type size() const;

		reference       front();
		const_reference front() const;

		reference       back();
		const_reference back() const;

		void push(const value_type& value);

		#if EASTL_MOVE_SEMANTICS_ENABLED
			void push(value_type&& x);
		#endif

		#if EASTL_MOVE_SEMANTICS_ENABLED && EASTL_VARIADIC_TEMPLATES_ENABLED
			template <class... Args>
			void emplace_back(Args&&... args);
		#else
			#if EASTL_MOVE_SEMANTICS_ENABLED
				void emplace_back(value_type&& x);
			#endif

			void emplace_back(const value_type& x);
		#endif

		void pop();

		container_type&       get_container();
		const container_type& get_container() const;

		void swap(this_type& x) EA_NOEXCEPT_IF((eastl::is_nothrow_swappable<this_type::container_type>::value));

		bool validate() const;

	}; // class queue




	///////////////////////////////////////////////////////////////////////
	// queue
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Container>
	inline queue<T, Container>::queue()
		: c() // To consider: use c(EASTL_QUEUE_DEFAULT_ALLOCATOR) here, though that would add the requirement that the user supplied container support this.
	{
		// Empty
	}


	template <typename T, typename Container>
	inline queue<T, Container>::queue(const Container& x)
		: c(x)
	{
		// Empty
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename T, typename Container>
		inline queue<T, Container>::queue(Container&& x)
			: c(eastl::move(x))
		{
			// Empty
		}
	#endif


	template <typename T, typename Container>
	inline queue<T, Container>::queue(std::initializer_list<value_type> ilist)
		: c() // We could alternatively use c(ilist) here, but that would require c to have an ilist constructor.
	{
		// Better solution but requires an insert function.
		// c.insert(ilist.begin(), ilist.end());

		// Possibly slower solution but doesn't require an insert function.
		for(typename std::initializer_list<value_type>::iterator it = ilist.begin(); it != ilist.end(); ++it)
		{
			const value_type& value = *it;
			c.push_back(value);
		}
	}


	template <typename T, typename Container>
	inline bool queue<T, Container>::empty() const
	{
		return c.empty();
	}


	template <typename T, typename Container>
	inline typename queue<T, Container>::size_type
	queue<T, Container>::size() const
	{
		return c.size();
	}


	template <typename T, typename Container>
	inline typename queue<T, Container>::reference
	queue<T, Container>::front()
	{
		return c.front();
	}


	template <typename T, typename Container>
	inline typename queue<T, Container>::const_reference
	queue<T, Container>::front() const
	{
		return c.front();
	}


	template <typename T, typename Container>
	inline typename queue<T, Container>::reference
	queue<T, Container>::back()
	{
		return c.back();
	}


	template <typename T, typename Container>
	inline typename queue<T, Container>::const_reference
	queue<T, Container>::back() const
	{
		return c.back();
	}


	template <typename T, typename Container>
	inline void queue<T, Container>::push(const value_type& value)
	{
		c.push_back(const_cast<value_type&>(value)); // const_cast so that intrusive_list can work. We may revisit this.
	}


	#if EASTL_MOVE_SEMANTICS_ENABLED
		template <typename T, typename Container>
		inline void queue<T, Container>::push(value_type&& x) 
		{
			c.push_back(eastl::move(x));
		}
	#endif


	#if EASTL_MOVE_SEMANTICS_ENABLED && EASTL_VARIADIC_TEMPLATES_ENABLED
		template <typename T, typename Container>
		template <class... Args> 
		inline void queue<T, Container>::emplace_back(Args&&... args)
		{
			c.emplace_back(eastl::forward<Args>(args)...);
		}
	#else
		#if EASTL_MOVE_SEMANTICS_ENABLED
			template <typename T, typename Container>
			inline void queue<T, Container>::emplace_back(value_type&& x)
			{
				c.emplace_back(eastl::move(x));
			}
		#endif

		template <typename T, typename Container>
		inline void queue<T, Container>::emplace_back(const value_type& x)
		{
			c.emplace_back(x);
		}
	#endif


	template <typename T, typename Container>
	inline void queue<T, Container>::pop()
	{
		c.pop_front();
	}


	template <typename T, typename Container>
	inline typename queue<T, Container>::container_type&
	queue<T, Container>::get_container()
	{
		return c;
	}


	template <typename T, typename Container>
	inline const typename queue<T, Container>::container_type&
	queue<T, Container>::get_container() const
	{
		return c;
	}


	template <typename T, typename Container>
	void queue<T, Container>::swap(this_type& x) EA_NOEXCEPT_IF((eastl::is_nothrow_swappable<this_type::container_type>::value))
	{
		using eastl::swap;
		swap(c, x.c);
	}


	template <typename T, typename Container>
	bool queue<T, Container>::validate() const
	{
		return c.validate();
	}


	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template <typename T, typename Container>
	inline bool operator==(const queue<T, Container>& a, const queue<T, Container>& b)
	{
		return (a.c == b.c);
	}

	template <typename T, typename Container>
	inline bool operator!=(const queue<T, Container>& a, const queue<T, Container>& b)
	{
		return !(a.c == b.c);
	}

	template <typename T, typename Container>
	inline bool operator<(const queue<T, Container>& a, const queue<T, Container>& b)
	{
		return (a.c < b.c);
	}

	template <typename T, typename Container>
	inline bool operator>(const queue<T, Container>& a, const queue<T, Container>& b)
	{
		return (b.c < a.c);
	}

	template <typename T, typename Container>
	inline bool operator<=(const queue<T, Container>& a, const queue<T, Container>& b)
	{
		return !(b.c < a.c);
	}

	template <typename T, typename Container>
	inline bool operator>=(const queue<T, Container>& a, const queue<T, Container>& b)
	{
		return !(a.c < b.c);
	}


	template <typename T, typename Container>
	inline void swap(queue<T, Container>& a, queue<T, Container>& b) EA_NOEXCEPT_IF((eastl::is_nothrow_swappable<typename queue<T, Container>::container_type>::value)) // EDG has a bug and won't let us use Container in this noexcept statement
	{
		a.swap(b);
	}


} // namespace eastl


#endif // Header include guard













