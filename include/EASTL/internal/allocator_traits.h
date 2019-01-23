/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// The code in this file is a modification of the libcxx implementation.  We copy
// the license information here as required.
////////////////////////////////////////////////////////////////////////////////
//===------------------------ functional ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <EABase/eabase.h>
#include <EASTL/internal/config.h>

namespace eastl
{
	namespace Internal
	{
		// has_value_type
		template <class T>
		struct has_value_type
		{
		private:
			template <class U> static eastl::no_type test(...);
			template <class U> static eastl::yes_type test(typename U::value_type* = 0);
		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(eastl::yes_type);
		};

		template <class Alloc, bool = has_value_type<Alloc>::value>
		struct value_type
		{
			typedef typename Alloc::value_type type;
		};

		template <class Alloc>
		struct value_type<Alloc, false>
		{
			typedef char type;
		};


		// has_pointer_type
		namespace has_pointer_type_imp
		{
			template <class U> static eastl::no_type test(...);
			template <class U> static eastl::yes_type test(typename U::pointer* = 0);
		}

		template <class T>
		struct has_pointer_type
			: public integral_constant<bool, sizeof(has_pointer_type_imp::test<T>(0)) == sizeof(eastl::yes_type)>
		{
		};

		namespace PointerTypeInternal
		{
			template <class T, class D, bool = has_pointer_type<D>::value>
			struct pointer_type
			{
				typedef typename D::pointer type;
			};

			template <class T, class D>
			struct pointer_type<T, D, false>
			{
				typedef T* type;
			};
		}

		template <class T, class D>
		struct pointer_type
		{
			typedef typename PointerTypeInternal::pointer_type<T, typename remove_reference<D>::type>::type type;
		};


		// has_const_pointer
		template <class Pointer>
		struct has_const_pointer
		{
		private:
			template <class U> static eastl::no_type test(...);
			template <class U> static eastl::yes_type test(typename U::const_pointer* = 0);
		public:
			static const bool value = sizeof(test<Pointer>(0)) == sizeof(eastl::yes_type);
		};

		template <class T, class Pointer, class Alloc, bool = has_const_pointer<Alloc>::value>
		struct const_pointer
		{
			typedef typename Alloc::const_pointer type;
		};

		template <class T, class Pointer, class Alloc>
		struct const_pointer<T, Pointer, Alloc, false>
		{
		#ifndef EA_COMPILER_NO_TEMPLATE_ALIASES
			typedef typename pointer_traits<Pointer>::template rebind<const T> type;
		#else
			typedef typename pointer_traits<Pointer>::template rebind<const T>::other type;
		#endif
		};


		// has_void_pointer
		template <class Pointer>
		struct has_void_pointer
		{
		private:
			template <class U> static eastl::no_type test(...);
			template <class U> static eastl::yes_type test(typename U::void_pointer* = 0);
		public:
			static const bool value = sizeof(test<Pointer>(0)) == sizeof(eastl::yes_type);
		};

		template <class Pointer, class Alloc, bool = has_void_pointer<Alloc>::value>
		struct void_pointer
		{
			typedef typename Alloc::void_pointer type;
		};

		template <class Pointer, class Alloc>
		struct void_pointer<Pointer, Alloc, false>
		{
		#ifndef EA_COMPILER_NO_TEMPLATE_ALIASES
			typedef typename pointer_traits<Pointer>::template rebind<void> type;
		#else
			typedef typename pointer_traits<Pointer>::template rebind<void>::other type;
		#endif
		};


		// has_const_void_pointer
		template <class Pointer>
		struct has_const_void_pointer
		{
		private:
			template <class U> static eastl::no_type test(...);
			template <class U> static eastl::yes_type test(typename U::const_void_pointer* = 0);
		public:
			static const bool value = sizeof(test<Pointer>(0)) == sizeof(eastl::yes_type);
		};

		template <class Pointer, class Alloc, bool = has_const_void_pointer<Alloc>::value>
		struct const_void_pointer
		{
			typedef typename Alloc::const_void_pointer type;
		};

		template <class Pointer, class Alloc>
		struct const_void_pointer<Pointer, Alloc, false>
		{
		#ifndef EA_COMPILER_NO_TEMPLATE_ALIASES
			typedef typename pointer_traits<Pointer>::template rebind<const void> type;
		#else
			typedef typename pointer_traits<Pointer>::template rebind<const void>::other type;
		#endif
		};


		// alloc_traits_difference_type
		template <class Alloc, class Pointer, bool = has_difference_type<Alloc>::value>
		struct alloc_traits_difference_type
		{
			typedef typename pointer_traits<Pointer>::difference_type type;
		};

		template <class Alloc, class Pointer>
		struct alloc_traits_difference_type<Alloc, Pointer, true>
		{
			typedef typename Alloc::difference_type type;
		};


		// has_size_type
		template <class T>
		struct has_size_type
		{
		private:
			template <class U> static eastl::no_type test(...);
			template <class U> static char test(typename U::size_type* = 0);
		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(eastl::yes_type);
		};

		template <class Alloc, class DiffType, bool = has_size_type<Alloc>::value>
		struct size_type
		{
			typedef typename make_unsigned<DiffType>::type type;
		};

		template <class Alloc, class DiffType>
		struct size_type<Alloc, DiffType, true>
		{
			typedef typename Alloc::size_type type;
		};


		// has_construct
        template <class Alloc, class T, class... Args>
        decltype(eastl::declval<Alloc>().construct(eastl::declval<T*>(), eastl::declval<Args>()...), eastl::true_type())
        has_construct_test(Alloc&& a, T* p, Args&&... args);

        template <class Alloc, class Pointer, class... Args>
        eastl::false_type has_construct_test(const Alloc& a, Pointer&& p, Args&&... args);

        template <class Alloc, class Pointer, class... Args>
        struct has_construct
            : public eastl::integral_constant< bool,
                  eastl::is_same<decltype(has_construct_test(eastl::declval<Alloc>(), eastl::declval<Pointer>(),
				  eastl::declval<Args>()...)),
                          eastl::true_type>::value>
        {
        };


		// has_destroy
        template <class Alloc, class Pointer>
        auto has_destroy_test(Alloc&& a, Pointer&& p) -> decltype(a.destroy(p), eastl::true_type());

        template <class Alloc, class Pointer>
        auto has_destroy_test(const Alloc& a, Pointer&& p) -> eastl::false_type;

        template <class Alloc, class Pointer>
        struct has_destroy
            : public eastl::integral_constant< bool,
                  is_same<decltype(has_destroy_test(eastl::declval<Alloc>(), eastl::declval<Pointer>())), eastl::true_type>::value>
        {
        };


		// has_max_size
        template <class Alloc>
        auto has_max_size_test(Alloc&& a) -> decltype(a.max_size(), eastl::true_type());

        template <class Alloc>
        auto has_max_size_test(const volatile Alloc& a) -> eastl::false_type;

        template <class Alloc>
        struct has_max_size
            : public eastl::integral_constant<bool,
                                is_same<decltype(has_max_size_test(eastl::declval<Alloc&>())), eastl::true_type>::value>
        {
        };

    } // namespace Internal


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// allocator_traits
	//
	// C++11 Standard section 20.7.8
	// This Internal namespace holds the utility functions required for allocator_traits to do compile-time type
	// inspection inorder to determine if needs to provide a default implementation or utilize the users allocator
	// implementation.
	//
	// Reference: http://en.cppreference.com/w/cpp/memory/allocator_traits
	//
	// eastl::allocator_traits supplies a uniform interface to all allocator types.
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // eastl::allocator_traits is not a standards conforming implementation. Enough of the standard was implemented to
    // make the eastl::function implementation possible.  We must revisit this implementation before rolling out its
    // usage fully in eastl::containers.
	//
	// NOTE: We do not recommend users directly code against eastl::allocator_traits until we have completed a full standards comforming implementation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Alloc>
	struct allocator_traits
	{
	    typedef Alloc allocator_type;

		typedef typename Internal::value_type<allocator_type>::type value_type;
	    typedef typename Internal::pointer_type<value_type, allocator_type>::type pointer;
	    typedef typename Internal::const_pointer<value_type, pointer, allocator_type>::type const_pointer;
	    typedef typename Internal::void_pointer<pointer, allocator_type>::type void_pointer;
	    typedef typename Internal::const_void_pointer<pointer, allocator_type>::type const_void_pointer;
	    typedef typename Internal::alloc_traits_difference_type<allocator_type, pointer>::type difference_type;
	    typedef typename Internal::size_type<allocator_type, difference_type>::type size_type;

		//
		// TODO: for full standards compliance implement the following:
		//
		// typedef typename Internal::propagate_on_container_copy_assignment<allocator_type>::type propagate_on_container_copy_assignment;
		// typedef typename Internal::propagate_on_container_move_assignment<allocator_type>::type propagate_on_container_move_assignment;
		// typedef typename Internal::propagate_on_container_swap<allocator_type>::type propagate_on_container_swap;
		// template <class T> using rebind_alloc  = Alloc::rebind<U>::other | Alloc<T, Args...>;
		// template <class T> using rebind_traits = allocator_traits<rebind_alloc<T>>;
		// static allocator_type select_on_container_copy_construction(const allocator_type& a);

	    static size_type internal_max_size(true_type, const allocator_type& a) { return a.max_size(); }
	    static size_type internal_max_size(false_type, const allocator_type&) { return (eastl::numeric_limits<size_type>::max)(); }  // additional parenthesis disables the windows max macro from expanding.
	    static size_type max_size(const allocator_type& a) EA_NOEXCEPT
	    {
		    return internal_max_size(Internal::has_max_size<const allocator_type>(), a);
	    }

	    static pointer allocate(allocator_type& a, size_type n) { return static_cast<pointer>(a.allocate(n)); }

		static pointer allocate(allocator_type& a, size_type n, const_void_pointer)
		{
			// return allocate(a, n, hint, Internal::has_allocate_hint<allocator_type, size_type, const_void_pointer>());
			return allocate(a, n);
		}

		static void deallocate(allocator_type& a, pointer p, size_type n) EA_NOEXCEPT { a.deallocate(p, n); }

	    template <class T, class... Args>
	    static void internal_construct(eastl::true_type, allocator_type& a, T* p, Args&&... args)
	    {
		    a.construct(p, eastl::forward<Args>(args)...);
	    }

	    template <class T, class... Args>
	    static void internal_construct(false_type, allocator_type&, T* p, Args&&... args)
	    {
		    ::new ((void*)p) T(eastl::forward<Args>(args)...);
	    }

	    template <class T, class... Args>
		static void construct(allocator_type& a, T* p, Args&&... args)
		{
			internal_construct(Internal::has_construct<allocator_type, T*, Args...>(), a, p, eastl::forward<Args>(args)...);
		}

	    template <class T>
	    static void internal_destroy(eastl::true_type, allocator_type& a, T* p) { a.destroy(p); }

	    template <class T>
	    static void internal_destroy(eastl::false_type, allocator_type&, T* p) { EA_UNUSED(p); p->~T(); }

	    template <class T>
	    static void destroy(allocator_type& a, T* p)
	    {
			internal_destroy(Internal::has_destroy<allocator_type, T*>(), a, p);
	    }
    };
} // namespace eastl
