// (c) 2024 Electronic Arts Inc.

#pragma once

#include <EASTL/internal/config.h>
#include <EASTL/internal/memory_base.h>
#include <EASTL/tuple.h> // via its transitive includes, needs memory_base.h but *not* memory_uses_allocator.h
#include <EASTL/type_traits.h>

namespace eastl
{


	///////////////////////////////////////////////////////////////////////
	// uses_allocator
	//
	// Determines if the class T has an allocator_type member typedef
	// which Allocator is convertible to.
	//
	// http://en.cppreference.com/w/cpp/memory/uses_allocator
	//
	// A program may specialize this template to derive from true_type for a
	// user-defined type T that does not have a nested allocator_type but
	// nonetheless can be constructed with an allocator where either:
	//    - the first argument of a constructor has type allocator_arg_t and
	//      the second argument has type Allocator.
	//    or
	//    - the last argument of a constructor has type Allocator.
	//
	// Example behavilor:
	//     uses_allocator<vector>::value => true
	//     uses_allocator<int>::value    => false
	//
	// This is useful for writing generic code for containers when you can't
	// know ahead of time that the container has an allocator_type.
	///////////////////////////////////////////////////////////////////////

	template <typename T>
	struct has_allocator_type_helper
	{
	private:
		template <typename>
		static eastl::no_type test(...);

		template <typename U>
		static eastl::yes_type test(typename U::allocator_type* = NULL);

	public:
		static const bool value = sizeof(test<T>(NULL)) == sizeof(eastl::yes_type);
	};


	template <typename T, typename Allocator, bool = has_allocator_type_helper<T>::value>
	struct uses_allocator_impl
	    : public integral_constant<bool, eastl::is_convertible<Allocator, typename T::allocator_type>::value>
	{
	};

	template <typename T, typename Allocator>
	struct uses_allocator_impl<T, Allocator, false> : public eastl::false_type
	{
	};

	template <typename T, typename Allocator>
	struct uses_allocator : public uses_allocator_impl<T, Allocator>
	{
	};

	template <typename T, typename Allocator>
	constexpr bool uses_allocator_v = uses_allocator<T, Allocator>::value;

	namespace detail
	{
		template <typename T, typename... Args>
		struct has_allocator_construct
		{
			template <typename Allocator, typename = void>
			struct inner : eastl::false_type
			{
			};

			template <typename Allocator>
			struct inner<Allocator,
			             eastl::void_t<decltype(eastl::declval<Allocator&>().construct(eastl::declval<T*>(),
			                                                                           eastl::declval<Args&&>()...))>>
			    : eastl::true_type
			{
			};
		};

		// equivalent to std::allocator_traits<Alloc>::construct
		template <typename Allocator, typename T, typename... Args>
		EA_CPP14_CONSTEXPR
		    eastl::enable_if_t<has_allocator_construct<T, Args...>::template inner<Allocator>::value, void>
		    allocator_construct(Allocator& allocator, T* p, Args&&... args)
		{
			allocator.construct(p, eastl::forward<Args>(args)...);
		}

		template <typename Allocator, typename T, typename... Args>
		EA_CPP14_CONSTEXPR
		    eastl::enable_if_t<!has_allocator_construct<T, Args...>::template inner<Allocator>::value, void>
		    allocator_construct(Allocator&, T* p, Args&&... args)
		{
			eastl::construct_at(p, eastl::forward<Args>(args)...);
		}

	} // namespace detail

} // namespace eastl
