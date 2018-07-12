/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/
// despite that it would be nice if you give credit to Malte Skarupke


/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file has been approved by EA Legal.
// http://easites.ea.com/legal/ViewRequest.aspx?List=0142a10f-ee60-4187-a2cf-766be908dd39&ID=1917&Source=http%3A%2F%2Feasites%2Eea%2Ecom%2Flegal%2FLists%2FOpenSourceDealSheet%2FRequests%2Easpx
//
// Original code has been modified in the following ways:
// * Support compilers missing variadic template support
// * Does not assume eastl::allocator are templated with the Functor type.  Instead requests raw bytes from  eastl::allocator and caches functor types itself in function_table.
// * Minor changes to support older console versions GCC compiler.
//
///////////////////////////////////////////////////////////////////////////////

#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#ifndef EASTL_INTERNAL_FUNCTION_H
#define EASTL_INTERNAL_FUNCTION_H

#include <EABase/nullptr.h>
#include <EABase/config/eacompilertraits.h>
#include <EASTL/type_traits.h>
#include <EASTL/utility.h>
#include <EASTL/allocator.h>
#include <EASTL/internal/move_help.h>
#include <EASTL/internal/mem_fn.h>
#include <EASTL/internal/function_help.h>
#include <EASTL/internal/allocator_traits_fwd_decls.h>

// RTTI currently disabled.  User can re-enable via the define below but note this code path isn't tested.
#define FUNC_NO_RTTI	

#define FUNC_TEMPLATE_NOEXCEPT(FUNCTOR, ALLOCATOR) EA_NOEXCEPT_IF(EA_NOEXCEPT_EXPR((detail::is_inplace_allocated<FUNCTOR, ALLOCATOR>::value)))


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This workaround exists because on MSVC the "is_nothrow_move_constructible" type trait does not function as
// expected.  It incorrectly flags the coping/moving of a pointer to the callable as being able to throw an exception.
// We can remove this workaround when the "is_nothrow_move_constructible" type trait functions for all Microsoft
// platforms and we begin testing it again in EASTLs unit tests.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(EA_PLATFORM_MICROSOFT)
	#define EASTL_INTERNAL_FUNCTION_ARE_TYPETRAITS_FUNCTIONAL 0
#else
	#define EASTL_INTERNAL_FUNCTION_ARE_TYPETRAITS_FUNCTIONAL 1
#endif

namespace eastl
{
	/// EASTL_FUNCTION_DEFAULT_NAME
	///
	/// Defines a default container name in the absence of a user-provided name.
	///
	#ifndef EASTL_FUNCTION_DEFAULT_NAME
		#define EASTL_FUNCTION_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " function" // Unless the user overrides something, this is "EASTL function".
	#endif

	/// EASTL_FUNCTION_DEFAULT_ALLOCATOR
	///
	#ifndef EASTL_FUNCTION_DEFAULT_ALLOCATOR
		#define EASTL_FUNCTION_DEFAULT_ALLOCATOR allocator_type(EASTL_FUNCTION_DEFAULT_NAME)
	#endif


	/// EASTL_FUNCTION_DEFAULT_CAPTURE_SSO_SIZE
	///
	/// Defines the size of the SSO buffer which is used to hold the specified capture state of the callable.
	///
	#ifndef EASTL_FUNCTION_DEFAULT_CAPTURE_SSO_SIZE
		#define EASTL_FUNCTION_DEFAULT_CAPTURE_SSO_SIZE (2 * sizeof(void*))
	#endif

	/// EASTL_FUNCTION_DEFAULT_ALLOCATOR_SSO_SIZE
	///
	/// Defines the size of the SSO buffer which is used to hold the specified allocator type.
	///
	#ifndef EASTL_FUNCTION_DEFAULT_ALLOCATOR_SSO_SIZE
		#define EASTL_FUNCTION_DEFAULT_ALLOCATOR_SSO_SIZE (2 * sizeof(void*))
	#endif


template<typename>
struct force_function_heap_allocation
	: public eastl::false_type {};

template<typename>
class function;

namespace detail
{
	struct total_storage_type;
	struct function_table;

	struct functor_storage_type
	{
		union
		{
			void* first;
			char padding[EASTL_FUNCTION_DEFAULT_CAPTURE_SSO_SIZE];
		};
	};
	static_assert(sizeof(functor_storage_type) >= EASTL_FUNCTION_DEFAULT_CAPTURE_SSO_SIZE, "capture storage size mismatch");

	struct allocator_storage_type
	{
		union
		{
			void* first;
			char padding[EASTL_FUNCTION_DEFAULT_ALLOCATOR_SSO_SIZE];
		};
	};
	static_assert(sizeof(allocator_storage_type) >= EASTL_FUNCTION_DEFAULT_ALLOCATOR_SSO_SIZE, "allocator storage size mismatch");



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// type_trait that determines if functor and allocator can fit into the eastl::function local buffer 
	//
	template<typename Callable, typename Allocator>
	struct is_inplace_allocated
	{
		static const bool value
			// so that Callable fits
			= sizeof(Callable) <= sizeof(functor_storage_type)
			// so that Allocator fits 
			&& sizeof(Allocator) <= sizeof(allocator_storage_type)
			// so that it will be aligned
			&& eastl::alignment_of<functor_storage_type>::value % eastl::alignment_of<Callable>::value == 0
		#if EASTL_INTERNAL_FUNCTION_ARE_TYPETRAITS_FUNCTIONAL			
			// so that we can offer noexcept move
			&& eastl::is_nothrow_move_constructible<Callable>::value
		#endif
			// so that the user can override it
			&& !force_function_heap_allocation<Callable>::value;
	};

	// todo:  provide a manual override macro that allows users to set the value of this trait for this specific type.


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// eastl::function local storage for the user provided functor and allocator types.
	//
    typedef const function_table* function_table_storage_type;
    struct total_storage_type
    {
	    allocator_storage_type allocator_storage;
	    functor_storage_type functor;
	    function_table_storage_type function_table;
    };


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocator and Functor fit into local buffers of eastl::function (ie. total_storage_type) 
	//
	template<typename T, typename Allocator, typename Enable = void>
	struct function_table_inplace_specialization
	{
		template<typename Result, typename... Arguments>
		static Result call(const functor_storage_type & storage, Arguments... arguments)
		{
			// do not call get_functor_ref because I want this function to be fast in debug when nothing gets inlined
			return const_cast<T &>(reinterpret_cast<const T &>(storage))(EASTL_FORWARD(Arguments, arguments)...);
		}

		// functor accessors
		static void store_functor(total_storage_type & storage, T to_store)
		{
			new (&get_functor_ref(storage)) T(EASTL_FORWARD(T, to_store));
		}
		static void move_functor(total_storage_type & lhs, total_storage_type && rhs) EA_NOEXCEPT
		{
			new (&get_functor_ref(lhs)) T(EASTL_MOVE(get_functor_ref(rhs)));
		}
		static void destroy_functor(Allocator &, total_storage_type & storage) EA_NOEXCEPT
		{
			get_functor_ref(storage).~T();
		}
		static T & get_functor_ref(const total_storage_type & storage) EA_NOEXCEPT
		{
			return const_cast<T &>(reinterpret_cast<const T &>(storage.functor));
		}

		// allocator accessors
		static void store_allocator(total_storage_type& storage, Allocator&& allocator)
		{
			new (&storage.allocator_storage) Allocator(EASTL_MOVE(allocator));
		}
		
		static void move_allocator(total_storage_type& lhs, total_storage_type&& rhs) EA_NOEXCEPT
		{
			new (&lhs.allocator_storage) Allocator(EASTL_MOVE((get_allocator(rhs))));
		}

		static Allocator& get_allocator(const total_storage_type& storage) EA_NOEXCEPT
		{
			return const_cast<Allocator&>(reinterpret_cast<const Allocator&>(storage.allocator_storage));
		}

		static void destroy_allocator(total_storage_type& storage) EA_NOEXCEPT
		{
			get_allocator(storage).~Allocator();
		}
	};


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocator and Functor must be allocated on the heap
	//
	template<typename T, typename Allocator>
	struct function_table_inplace_specialization<T, Allocator, typename eastl::enable_if<!is_inplace_allocated<T, Allocator>::value>::type>
	{
		typedef T Functor;
		typedef T* FunctorPointer;


		template<typename Result, typename... Arguments>
		static Result call(const functor_storage_type & storage, Arguments... arguments)
		{
			// do not call get_functor_ptr_ref because I want this function to be fast in debug when nothing gets inlined
			return (*reinterpret_cast<const FunctorPointer&>(storage))(EASTL_FORWARD(Arguments, arguments)...);
		}

		// functor accessors
		//
		static void store_functor(total_storage_type & self, T to_store)
		{
			static_assert(sizeof(FunctorPointer) <= sizeof(self.functor), "The allocator's pointer type is too big");

			Allocator& allocator = get_allocator(self);
			FunctorPointer* ptr = new (&get_functor_ptr_ref(self)) FunctorPointer(reinterpret_cast<FunctorPointer>(eastl::allocator_traits<Allocator>::allocate(allocator, sizeof(Functor))));
			eastl::allocator_traits<Allocator>::construct(allocator, *ptr, EASTL_FORWARD(T, to_store));
		}

		static void move_functor(total_storage_type & lhs, total_storage_type && rhs) EA_NOEXCEPT
		{
		#if EASTL_INTERNAL_FUNCTION_ARE_TYPETRAITS_FUNCTIONAL
			static_assert(eastl::is_nothrow_move_constructible<FunctorPointer>::value, "we can't offer a noexcept swap if the pointer type is not nothrow move constructible");
		#endif
			new (&get_functor_ptr_ref(lhs)) FunctorPointer(EASTL_MOVE(get_functor_ptr_ref(rhs)));
			// this next assignment makes the destroy function easier
			get_functor_ptr_ref(rhs) = nullptr;
		}

		static void destroy_functor(Allocator & allocator, total_storage_type & storage) EA_NOEXCEPT
		{
			FunctorPointer& pointer = get_functor_ptr_ref(storage);
			if (!pointer) return;
			eastl::allocator_traits<Allocator>::destroy(allocator, pointer);
			eastl::allocator_traits<Allocator>::deallocate(allocator, reinterpret_cast<typename eastl::allocator_traits<Allocator>::pointer>(pointer), sizeof(Functor));
		}

		static Functor& get_functor_ref(const total_storage_type & storage) EA_NOEXCEPT
		{
			return *get_functor_ptr_ref(storage);
		}

		static FunctorPointer& get_functor_ptr_ref(total_storage_type & storage) EA_NOEXCEPT
		{
			return const_cast<FunctorPointer&>(reinterpret_cast<const FunctorPointer&>(storage.functor));
		}

		static const FunctorPointer& get_functor_ptr_ref(const total_storage_type & storage) EA_NOEXCEPT
		{
			return const_cast<FunctorPointer&>(reinterpret_cast<const FunctorPointer&>(storage.functor));
		}


		// allocator accessors
		//
		static void store_allocator(total_storage_type& storage, Allocator&& allocator)
		{
			auto pMem = eastl::allocator_traits<Allocator>::allocate(allocator, sizeof(Allocator));
			storage.allocator_storage.first = reinterpret_cast<Allocator*>(pMem);
			eastl::allocator_traits<Allocator>::construct(allocator, (Allocator*)storage.allocator_storage.first, allocator);
		}

		static Allocator& get_allocator(const total_storage_type& storage) EA_NOEXCEPT
		{			
			return *const_cast<Allocator*&>(reinterpret_cast<const Allocator*const&>(storage.allocator_storage));
		}

		static void move_allocator(total_storage_type& lhs, total_storage_type&& rhs) EA_NOEXCEPT
		{			
			lhs.allocator_storage.first = rhs.allocator_storage.first;
		}

		static void destroy_allocator(total_storage_type& storage) EA_NOEXCEPT
		{
			auto& allocator = get_allocator(storage);
			Allocator local_allocator(allocator);			
			eastl::allocator_traits<Allocator>::deallocate(local_allocator, reinterpret_cast<typename eastl::allocator_traits<Allocator>::pointer>(&allocator), sizeof(Allocator));
			eastl::allocator_traits<Allocator>::destroy(local_allocator, reinterpret_cast<typename eastl::allocator_traits<Allocator>::pointer>(&allocator));
		}
	};

	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// get_default_function_table
	//
    template <typename T, typename Allocator>
    static const function_table& get_default_function_table();


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// init_function_table
	//
	template<typename T, typename Allocator>
	static void init_function_table(total_storage_type& storage)
	{
		storage.function_table = &get_default_function_table<T, Allocator>();
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// this struct acts as a vtable. it is an optimization to prevent
	// code-bloat from rtti. see the documentation of boost::function
	struct function_table
	{
	    typedef void (*call_move_and_destroy_t)(total_storage_type& lhs, total_storage_type&& rhs);
	    typedef void (*call_copy_t)(total_storage_type& lhs, const total_storage_type& rhs);
	    typedef void (*call_copy_functor_only_t)(total_storage_type& lhs, const total_storage_type& rhs);
	    typedef void (*call_destroy_t)(total_storage_type& function_table);
	#ifndef FUNC_NO_RTTI
	    typedef const std::type_info& (*call_type_id_t)();
	    typedef void* (*call_target_t)(const total_storage_type& function_table, const std::type_info& type);
	#endif

		// call move/copy callbacks
	    call_move_and_destroy_t call_move_and_destroy;
	    call_copy_t call_copy;
	    call_copy_functor_only_t call_copy_functor_only;
	    call_destroy_t call_destroy;
	#ifndef FUNC_NO_RTTI
	    call_type_id_t call_type_id;
	    call_target_t call_target;
	#endif

	    template<typename T, typename Allocator>
		inline static EA_CONSTEXPR function_table create_default_function_table()
		{
		    return function_table(&templated_call_move_and_destroy<T, Allocator>
		                            , &templated_call_copy<T, Allocator>
		                            , &templated_call_copy_functor_only<T, Allocator>
		                            , &templated_call_destroy<T, Allocator>
									#ifndef FUNC_NO_RTTI
										, &templated_call_type_id<T, Allocator>,
										, &templated_call_target<T, Allocator>
									#endif
									);
		}

		// function_table constructor
	    EA_CONSTEXPR function_table(call_move_and_destroy_t cmad,
	                                  call_copy_t cc,
	                                  call_copy_functor_only_t ccfo,
	                                  call_destroy_t cd
									#ifndef FUNC_NO_RTTI
	                                  call_type_id_t cti,
	                                  call_target_t ct
									#endif
	                                  )
	        : call_move_and_destroy(cmad)
	        , call_copy(cc)
	        , call_copy_functor_only(ccfo)
	        , call_destroy(cd)
		#ifndef FUNC_NO_RTTI
	        , call_type_id(cti)
	        , call_target(ct)
		#endif
	    {
	    }

	    template<typename T, typename Allocator>
		static void templated_call_move_and_destroy(total_storage_type & lhs, total_storage_type && rhs)
		{
			typedef function_table_inplace_specialization<T, Allocator> specialization;
			specialization::move_functor(lhs, EASTL_MOVE(rhs));
			specialization::destroy_functor(specialization::get_allocator(rhs), rhs);
			init_function_table<T, Allocator>(lhs);
			specialization::move_allocator(lhs, EASTL_MOVE(rhs));
		}

		template<typename T, typename Allocator>
		static void templated_call_copy(total_storage_type & lhs, const total_storage_type & rhs)
		{
			typedef function_table_inplace_specialization<T, Allocator> specialization;
			init_function_table<T, Allocator>(lhs);
			specialization::store_allocator(lhs, Allocator(specialization::get_allocator(rhs)));
			specialization::store_functor(lhs, specialization::get_functor_ref(rhs));
		}

		template<typename T, typename Allocator>
		static void templated_call_destroy(total_storage_type & self)
		{
			typedef function_table_inplace_specialization<T, Allocator> specialization;
			specialization::destroy_functor(specialization::get_allocator(self), self);
			specialization::destroy_allocator(self);
		}

		template<typename T, typename Allocator>
		static void templated_call_copy_functor_only(total_storage_type & lhs, const total_storage_type & rhs)
		{
			typedef function_table_inplace_specialization<T, Allocator> specialization;
			specialization::store_functor(lhs, specialization::get_functor_ref(rhs));
		}

		#ifndef FUNC_NO_RTTI
	        template<typename T, typename>
			static const std::type_info & templated_call_type_id()
			{
				return typeid(T);
			}
			template<typename T, typename Allocator>
			static void * templated_call_target(const total_storage_type & self, const std::type_info & type)
			{
				typedef function_table_inplace_specialization<T, Allocator> specialization;
				if (type == typeid(T))
					return &specialization::get_functor_ref(self);
				else
					return nullptr;
			}
		#endif
    };
	template<typename T, typename Allocator>
	inline static const function_table& get_default_function_table()
	{
		static const function_table default_function_table = function_table::create_default_function_table<T, Allocator>();
		return default_function_table;
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// eastl::function 
//
template<typename Result, typename... Arguments>
class function<Result (Arguments...)>
{
public:
	typedef Result result_type;

	function() EA_NOEXCEPT
	{
		initialize_empty();
	}
	function(std::nullptr_t) EA_NOEXCEPT
	{
		initialize_empty();
	}
	function(function && other) EA_NOEXCEPT
	{
		initialize_empty();
		swap(other);
	}
	function(const function & other)
		: call(other.call)
	{
		other.storage.function_table->call_copy(storage, other.storage);
	}

	template <typename T>
	function(T functor,
		     typename eastl::enable_if<internal::is_valid_function_argument<T, Result(Arguments...)>::value, int>::type = 0) FUNC_TEMPLATE_NOEXCEPT(T, EASTLAllocatorType)
	{
		if (internal::is_null(functor))
		{
			initialize_empty();
		}
		else
		{
			initialize(internal::to_functor(EASTL_FORWARD(T, functor)), EASTLAllocatorType());
		}
	}

	template<typename Allocator>
	function(eastl::allocator_arg_t, const Allocator &)
	{
		// ignore the allocator because I don't allocate
		initialize_empty();
	}

	template<typename Allocator>
	function(eastl::allocator_arg_t, const Allocator &, std::nullptr_t)
	{
		// ignore the allocator because I don't allocate
		initialize_empty();
	}

	template<typename Allocator, typename T>
	function(eastl::allocator_arg_t, const Allocator & allocator, T functor,
			typename eastl::enable_if<internal::is_valid_function_argument<T, Result (Arguments...)>::value, int>::type = 0)
			FUNC_TEMPLATE_NOEXCEPT(T, Allocator)
	{
		if (internal::is_null(functor))
		{
			initialize_empty();
		}
		else
		{
			initialize(internal::to_functor(EASTL_FORWARD(T, functor)), Allocator(allocator));
		}
	}

	template<typename Allocator>
	function(eastl::allocator_arg_t, const Allocator& allocator, const function& other)
		: call(other.call)
	{
		typedef typename eastl::allocator_traits<Allocator>::template rebind_alloc<function> MyAllocator;

		// first try to see if the allocator matches the target type
		detail::function_table_storage_type function_table_for_allocator = &detail::get_default_function_table<typename eastl::allocator_traits<Allocator>::value_type, Allocator>();
		if (other.storage.function_table == function_table_for_allocator)
		{
			detail::init_function_table<typename eastl::allocator_traits<Allocator>::value_type, Allocator>(storage);
			detail::function_table_inplace_specialization<typename eastl::allocator_traits<Allocator>::value_type, Allocator>::store_allocator(storage, Allocator(allocator));
			function_table_for_allocator->call_copy_functor_only(storage, other.storage);
		}
		// if it does not, try to see if the target contains my type. this
		// breaks the recursion of the last case. otherwise repeated copies
		// would allocate more and more memory
		else
		{
			detail::function_table_storage_type function_table_for_function = &detail::get_default_function_table<function, MyAllocator>();
			if (other.storage.function_table == function_table_for_function)
			{
				detail::init_function_table<function, MyAllocator>(storage);
				detail::function_table_inplace_specialization<function, Allocator>::store_allocator(storage, MyAllocator(allocator));
				function_table_for_function->call_copy_functor_only(storage, other.storage);
			}
			else
			{
				// else store the other function as my target
				initialize(other, MyAllocator(allocator));
			}
		}
	}
	template<typename Allocator>
	function(eastl::allocator_arg_t, const Allocator&, function&& other) EA_NOEXCEPT
	{
		// ignore the allocator because I don't allocate
		initialize_empty();
		swap(other);
	}

	function & operator=(function other) EA_NOEXCEPT
	{
		swap(other);
		return *this;
	}

	~function() EA_NOEXCEPT
	{
		storage.function_table->call_destroy(storage);
	}

	Result operator()(Arguments... arguments) const
	{
		return call(storage.functor, EASTL_FORWARD(Arguments, arguments)...);
	}

	template<typename T, typename Allocator>
	void assign(T && functor, const Allocator & allocator) FUNC_TEMPLATE_NOEXCEPT(T, Allocator)
	{
		function(eastl::allocator_arg, allocator, functor).swap(*this);
	}
	
	void swap(function & other) EA_NOEXCEPT
	{
		detail::total_storage_type temp_storage;
		other.storage.function_table->call_move_and_destroy(temp_storage, EASTL_MOVE(other.storage));
		storage.function_table->call_move_and_destroy(other.storage, EASTL_MOVE(storage));
		temp_storage.function_table->call_move_and_destroy(storage, EASTL_MOVE(temp_storage));

		eastl::swap(call, other.call);
	}


	#ifndef FUNC_NO_RTTI
	    const std::type_info & target_type() const EA_NOEXCEPT
		{
			return storage.function_table->call_type_id();
		}
		template<typename T>
		T * target() EA_NOEXCEPT
		{
			return static_cast<T *>(storage.function_table->call_target(storage, typeid(T)));
		}
		template<typename T>
		const T * target() const EA_NOEXCEPT
		{
			return static_cast<const T *>(storage.function_table->call_target(storage, typeid(T)));
		}
	#endif

	operator bool() const EA_NOEXCEPT { return call != nullptr; }

private:
	detail::total_storage_type storage;
	Result (*call)(const detail::functor_storage_type &, Arguments...);

	template<typename T, typename Allocator>
	void initialize(T functor, Allocator && allocator)
	{
		call = &detail::function_table_inplace_specialization<T, Allocator>::template call<Result, Arguments...>;
		detail::init_function_table<T, Allocator>(storage);
		detail::function_table_inplace_specialization<T, Allocator>::store_allocator(storage, EASTL_FORWARD(Allocator, allocator));
		detail::function_table_inplace_specialization<T, Allocator>::store_functor(storage, EASTL_FORWARD(T, functor));		
	}

	void initialize_empty() EA_NOEXCEPT
	{
		typedef Result(*Empty_Function_Type)(Arguments...);
		
		typedef EASTLAllocatorType Allocator;
		static_assert(detail::is_inplace_allocated<Empty_Function_Type, Allocator>::value, "The empty function should benefit from small functor optimization");

		detail::init_function_table<Empty_Function_Type, Allocator>(storage);
		detail::function_table_inplace_specialization<Empty_Function_Type, Allocator>::store_allocator(storage, Allocator());
		detail::function_table_inplace_specialization<Empty_Function_Type, Allocator>::store_functor(storage, nullptr);
		call = nullptr;
	}
};


//////////////////////////////////////////////////////////////////////////////////////////////////
// comparison operators
//
template <typename T>
bool operator==(std::nullptr_t, const function<T>& rhs) EA_NOEXCEPT { return !rhs; }

template <typename T>
bool operator==(const function<T>& lhs, std::nullptr_t) EA_NOEXCEPT { return !lhs; }

template <typename T>
bool operator!=(std::nullptr_t, const function<T>& rhs) EA_NOEXCEPT { return rhs; }

template <typename T>
bool operator!=(const function<T>& lhs, std::nullptr_t) EA_NOEXCEPT { return lhs; }


//////////////////////////////////////////////////////////////////////////////////////////////////
// swap 
//
template<typename T>
void swap(function<T> & lhs, function<T> & rhs) { lhs.swap(rhs); }

} // namespace eastl 


#undef FUNC_TEMPLATE_NOEXCEPT
#undef FUNC_NO_RTTI

#endif // EASTL_INTERNAL_FUNCTION_H
