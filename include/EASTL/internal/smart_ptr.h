/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_SMART_PTR_H
#define EASTL_INTERNAL_SMART_PTR_H


#include <EABase/eabase.h>
#include <EASTL/type_traits.h>
#include <EASTL/memory.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


namespace eastl
{

	namespace Internal
	{
		// Tells if the Deleter type has a typedef for pointer to T. If so then return it, 
		// else return T*. The large majority of the time the pointer type will be T*.
		// The C++11 Standard requires that scoped_ptr let the deleter define the pointer type.
		//
		// Example usage:
		//     typedef typename unique_pointer_type<int, SomeDeleter>::type pointer
		//
		template <typename T, typename Deleter>
		class unique_pointer_type
		{
			template <typename U>
			static typename U::pointer test(typename U::pointer*);

			template <typename U>
			static T* test(...);

		public:
			typedef decltype(test<typename eastl::remove_reference<Deleter>::type>(0)) type;
		};


		///////////////////////////////////////////////////////////////////////
		// is_array_cv_convertible
		//
		// Tells if the array pointer P1 is cv-convertible to array pointer P2.
		// The two types have two be equivalent pointer types and be convertible
		// when you consider const/volatile properties of them.
		//
		// Example usage:
		//     is_array_cv_convertible<int, Base*>::value             => false
		//     is_array_cv_convertible<Base, Base*>::value            => false
		//     is_array_cv_convertible<double*, bool*>::value         => false
		//     is_array_cv_convertible<Subclass*, Base*>::value       => false
		//     is_array_cv_convertible<const Base*, Base*>::value     => false
		//     is_array_cv_convertible<Base*, Base*>::value           => true
		//     is_array_cv_convertible<Base*, const Base*>::value     => true
		//     is_array_cv_convertible<Base*, volatile Base*>::value  => true
		///////////////////////////////////////////////////////////////////////

		#define EASTL_TYPE_TRAIT_is_array_cv_convertible_CONFORMANCE 1

		template <typename P1, typename P2, bool = eastl::is_same_v<eastl::remove_cv_t<typename pointer_traits<P1>::element_type>,
																    eastl::remove_cv_t<typename pointer_traits<P2>::element_type>>>
		struct is_array_cv_convertible_impl 
			: public eastl::is_convertible<P1, P2> {};  // Return true if P1 is convertible to P2.

		template <typename P1, typename P2>
		struct is_array_cv_convertible_impl<P1, P2, false> 
			: public eastl::false_type {};              // P1's underlying type is not the same as P2's, so it can't be converted, even if P2 refers to a subclass of P1. Parent == Child, but Parent[] != Child[]

		template <typename P1, typename P2, bool = eastl::is_scalar_v<P1> && !eastl::is_pointer_v<P1>>
		struct is_array_cv_convertible
			: public is_array_cv_convertible_impl<P1, P2> {};

		template <typename P1, typename P2>
		struct is_array_cv_convertible<P1, P2, true>
			: public eastl::false_type {};              // P1 is scalar not a pointer, so it can't be converted to a pointer.


		///////////////////////////////////////////////////////////////////////
		// is_derived
		//
		// Given two (possibly identical) types Base and Derived, is_base_of<Base, Derived>::value == true 
		// if and only if Base is a direct or indirect base class of Derived. This is like is_base_of<Base, Derived> 
		// but returns false if Derived is the same as Base. So is_derived is true only if Derived is actually a subclass 
		// of Base and not Base itself.
		//
		// is_derived may only be applied to complete types.
		//
		// Example usage:
		//     is_derived<int, int>::value             => false
		//     is_derived<int, bool>::value            => false
		//     is_derived<Parent, Child>::value        => true
		//     is_derived<Child, Parent>::value        => false
		///////////////////////////////////////////////////////////////////////

		#if EASTL_TYPE_TRAIT_is_base_of_CONFORMANCE
			#define EASTL_TYPE_TRAIT_is_derived_CONFORMANCE 1

			template <typename Base, typename Derived>
			struct is_derived : public eastl::integral_constant<bool, eastl::is_base_of<Base, Derived>::value && !eastl::is_same<typename eastl::remove_cv<Base>::type, typename eastl::remove_cv<Derived>::type>::value> {};
		#else
			#define EASTL_TYPE_TRAIT_is_derived_CONFORMANCE 0

			template <typename Base, typename Derived> // This returns true if Derived is unrelated to Base. That's a wrong answer, but is better for us than returning false for compilers that don't support is_base_of.
			struct is_derived : public eastl::integral_constant<bool, !eastl::is_same<typename eastl::remove_cv<Base>::type, typename eastl::remove_cv<Derived>::type>::value> {};
		#endif


		///////////////////////////////////////////////////////////////////////
		// is_safe_array_conversion
		//
		// Say you have two array types: T* t and U* u. You want to assign the u to t but only if 
		// that's a safe thing to do. As shown in the logic below, the array conversion
		// is safe if U* and T* are convertible, if U is an array, and if either U or T is not 
		// a pointer or U is not derived from T.
		//
		// Note: Usage of this class could be replaced with is_array_cv_convertible usage.
		// To do: Do this replacement and test it.
		// 
		///////////////////////////////////////////////////////////////////////

		template <typename T, typename T_pointer, typename U, typename U_pointer>
		struct is_safe_array_conversion : public eastl::integral_constant<bool, eastl::is_convertible<U_pointer, T_pointer>::value &&
																				eastl::is_array<U>::value && 
																				(!eastl::is_pointer<U_pointer>::value || !is_pointer<T_pointer>::value || !Internal::is_derived<T, typename eastl::remove_extent<U>::type>::value)> {};

	} // namespace Internal



	



	/// default_delete
	///
	/// C++11 smart pointer default delete function class.
	///
	/// Provides a default way to delete an object. This default is simply to call delete on the 
	/// object pointer. You can provide an alternative to this class or you can override this on 
	/// a class-by-class basis like the following:
	///     template <>
	///     struct smart_ptr_deleter<MyClass>
	///     {
	///         void operator()(MyClass* p) const
	///            { SomeCustomFunction(p); }
	///     };
	///
	template <typename T>
	struct default_delete
	{
		#if defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION <= 4006) // GCC prior to 4.7 has a bug with noexcept here.
			EA_CONSTEXPR default_delete() = default;
		#else
			EA_CONSTEXPR default_delete() EA_NOEXCEPT = default;
		#endif

		template <typename U>  // Enable if T* can be constructed with U* (i.e. U* is convertible to T*).
		default_delete(const default_delete<U>&, typename eastl::enable_if<is_convertible<U*, T*>::value>::type* = 0) EA_NOEXCEPT {}

		void operator()(T* p) const EA_NOEXCEPT
		{
			static_assert(eastl::internal::is_complete_type_v<T>, "Attempting to call the destructor of an incomplete type");
			delete p;
		}
	};


	template <typename T>
	struct default_delete<T[]> // Specialization for arrays.
	{
		#if defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION <= 4006) // GCC prior to 4.7 has a bug with noexcept here.
			EA_CONSTEXPR default_delete() = default;
		#else
			EA_CONSTEXPR default_delete() EA_NOEXCEPT = default;
		#endif

		template <typename U> // This ctor is enabled if T is equal to or a base of U, and if U is less or equal const/volatile-qualified than T.
		default_delete(const default_delete<U[]>&, typename eastl::enable_if<Internal::is_array_cv_convertible<U*, T*>::value>::type* = 0) EA_NOEXCEPT {}

		void operator()(T* p) const EA_NOEXCEPT
			{ delete[] p; }
	};




	/// smart_ptr_deleter
	///
	/// Deprecated in favor of the C++11 name: default_delete
	///
	template <typename T>
	struct smart_ptr_deleter
	{
		typedef T value_type;

		void operator()(const value_type* p) const // We use a const argument type in order to be most flexible with what types we accept. 
			{ delete const_cast<value_type*>(p); }
	};

	template <>
	struct smart_ptr_deleter<void>
	{
		typedef void value_type;

		void operator()(const void* p) const
			{ delete[] (char*)p; } // We don't seem to have much choice but to cast to a scalar type.
	};

	template <>
	struct smart_ptr_deleter<const void>
	{
		typedef void value_type;

		void operator()(const void* p) const
			{ delete[] (char*)p; } // We don't seem to have much choice but to cast to a scalar type.
	};



	/// smart_array_deleter
	///
	/// Deprecated in favor of the C++11 name: default_delete
	///
	template <typename T>
	struct smart_array_deleter
	{
		typedef T value_type;

		void operator()(const value_type* p) const // We use a const argument type in order to be most flexible with what types we accept. 
			{ delete[] const_cast<value_type*>(p); }
	};

	template <>
	struct smart_array_deleter<void>
	{
		typedef void value_type;

		void operator()(const void* p) const
			{ delete[] (char*)p; } // We don't seem to have much choice but to cast to a scalar type.
	};


} // namespace eastl


#endif // Header include guard

















