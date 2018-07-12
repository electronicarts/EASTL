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
// * Allocator overloads removed because C++17 removed them due to poor specification in the standard and fixed_function will never allocate from the heap per user requests.
// * Heavily modified to support short string optimization style memory usage.
//
///////////////////////////////////////////////////////////////////////////////

#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#ifndef EASTL_INTERNAL_FIXED_FUNCTION_H
#define EASTL_INTERNAL_FIXED_FUNCTION_H

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


namespace eastl
{

template<int, typename>
class fixed_function;

namespace ff_detail
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// fwd-decls 
	//
	template <int SIZE_IN_BYTES>
	struct total_storage_type;

	struct function_table;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// functor_storage_type<SIZE_IN_BYTES>
	//
	// The fixed_function internal functor storage class thats size is requested by the user.
	//
	template <int SIZE_IN_BYTES>
	struct functor_storage_type
	{
		union
		{
			void* first;  // forces natural alignment
			char padding[SIZE_IN_BYTES];
		};
	};

	template <>
	struct functor_storage_type<0>
	{
		union
		{
			void* first;     // forces natural alignment
		 // char padding[0]; // intentionally removed to avoid warnings about zero lengthed arrays.
		};
	};


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// total_storage_type
	//
	// variable sized local storage for the user provided functor types.
	//
	template<int SIZE_IN_BYTES_T>
    struct total_storage_type
    {
		typedef functor_storage_type<SIZE_IN_BYTES_T> functor_storage_type_with_size;
		static const int SIZE_IN_BYTES = SIZE_IN_BYTES_T;

	    functor_storage_type_with_size functor;
	    const function_table* function_table_ptr;
    };


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  Functor fit into local buffers of eastl::function (ie. total_storage_type) 
	//
	template<int SIZE_IN_BYTES, typename T, typename Enable = void>
	struct function_table_inplace_specialization
	{
		template <typename Result, typename... Arguments>
		EA_FORCE_INLINE static Result call(const functor_storage_type<SIZE_IN_BYTES>& storage, Arguments... arguments)
		{
			// do not call get_functor_ref because I want this function to be fast in debug when nothing gets inlined
			return const_cast<T&>(reinterpret_cast<const T&>(storage))(EASTL_FORWARD(Arguments, arguments)...);
		}

		// functor accessors
		static void store_functor(total_storage_type<SIZE_IN_BYTES>& storage, T to_store)
		{
			new (&get_functor_ref(storage)) T(EASTL_FORWARD(T, to_store));
		}

		static void move_functor(total_storage_type<SIZE_IN_BYTES>& lhs, total_storage_type<SIZE_IN_BYTES>&& rhs) EA_NOEXCEPT
		{
			new (&get_functor_ref(lhs)) T(EASTL_MOVE(get_functor_ref(rhs)));
		}

		static void destroy_functor(total_storage_type<SIZE_IN_BYTES>& storage) EA_NOEXCEPT 
		{ 
			get_functor_ref(storage).~T(); 
		}

		static T& get_functor_ref(const total_storage_type<SIZE_IN_BYTES>& storage) EA_NOEXCEPT
		{
			return const_cast<T&>(reinterpret_cast<const T&>(storage.functor));
		}
	};

	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// get_default_function_table
	//
    template <typename T, typename TotalStorageType>
    static const function_table& get_default_function_table();


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// init_function_table
	//
	template<typename T, typename TotalStorageType>
	static void init_function_table(TotalStorageType& storage)
	{
		storage.function_table_ptr = &get_default_function_table<T, TotalStorageType>();
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// this struct acts as a vtable. it is an optimization to prevent
	// code-bloat from rtti. see the documentation of boost::function
	struct function_table
	{
	    typedef void (*call_move_and_destroy_t)(void* lhs, void* rhs);
	    typedef void (*call_copy_t)(void* lhs, const void* rhs);
	    typedef void (*call_copy_functor_only_t)(void* lhs, void* rhs);
	    typedef void (*call_destroy_t)(void* self);
	#ifndef FUNC_NO_RTTI
	    typedef const std::type_info& (*call_type_id_t)();
	    typedef void* (*call_target_t)(void* self, const std::type_info& type);
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

		template <typename T, typename TotalStorageType>
		inline static EA_CONSTEXPR function_table create_default_function_table()
		{
		    return function_table(&templated_call_move_and_destroy<T, TotalStorageType>
		                            , &templated_call_copy<T, TotalStorageType>
		                            , &templated_call_copy_functor_only<T, TotalStorageType>
		                            , &templated_call_destroy<T, TotalStorageType>
									#ifndef FUNC_NO_RTTI
										, &templated_call_type_id<T, TotalStorageType>,
										, &templated_call_target<T, TotalStorageType>
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

		template <typename T, typename TotalStorageType>
		static void templated_call_move_and_destroy(void* in_lhs, void* in_rhs)
		{
			TotalStorageType& lhs = *(TotalStorageType*)in_lhs;
			TotalStorageType& rhs = *(TotalStorageType*)in_rhs;

			typedef function_table_inplace_specialization<TotalStorageType::SIZE_IN_BYTES, T> specialization;
			specialization::move_functor(lhs, EASTL_MOVE(rhs));
			specialization::destroy_functor(rhs);
			init_function_table<T>(lhs);
		}

		template <typename T, typename TotalStorageType>
		static void templated_call_copy(void* in_lhs, const void* in_rhs)
		{
			TotalStorageType& lhs = *(TotalStorageType*)in_lhs;
			const TotalStorageType& rhs = *(const TotalStorageType*)in_rhs;

			typedef function_table_inplace_specialization<TotalStorageType::SIZE_IN_BYTES, T> specialization;
			init_function_table<T>(lhs);
			specialization::store_functor(lhs, specialization::get_functor_ref(rhs));
		}

		template <typename T, typename TotalStorageType>
		static void templated_call_destroy(void* in_self)
		{
			TotalStorageType& self = *(TotalStorageType*)in_self;

			typedef function_table_inplace_specialization<TotalStorageType::SIZE_IN_BYTES, T> specialization;
			specialization::destroy_functor(self);
		}

		template <typename T, typename TotalStorageType>
		static void templated_call_copy_functor_only(void* in_lhs, void* in_rhs)
		{
			TotalStorageType& lhs = *(TotalStorageType*)in_lhs;
			const TotalStorageType& rhs = *(const TotalStorageType*)in_rhs;

			typedef function_table_inplace_specialization<TotalStorageType::SIZE_IN_BYTES, T> specialization;
			specialization::store_functor(lhs, specialization::get_functor_ref(rhs));
		}

        #ifndef FUNC_NO_RTTI
	        template<typename T, typename>
			static const std::type_info & templated_call_type_id()
			{
				return typeid(T);
			}

			template <typename T, typename TotalStorageType>
			static void* templated_call_target(void* in_self, const std::type_info& type)
			{
				TotalStorageType& self = *(TotalStorageType*)in_self;

				typedef function_table_inplace_specialization<T> specialization;
				if (type == typeid(T))
					return &specialization::get_functor_ref(self);
				else
					return nullptr;
			}
        #endif
    };

    template <typename T, typename TotalStorageType>
	inline static const function_table& get_default_function_table()
	{
		static const function_table default_function_table = function_table::create_default_function_table<T, TotalStorageType>();
		return default_function_table;
	}
} // namespace ff_detail



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// eastl::fixed_function 
//
// TODO(rparolin):  Consider adding explicit memory alignment controls.
//
template<int SIZE_IN_BYTES, typename Result, typename... Arguments>
class fixed_function<SIZE_IN_BYTES, Result (Arguments...)>
{
public:
	typedef Result result_type;

	fixed_function() EA_NOEXCEPT
	{
		initialize_empty();
	}

	fixed_function(std::nullptr_t) EA_NOEXCEPT
	{
		initialize_empty();
	}

	fixed_function(fixed_function&& other) EA_NOEXCEPT
	{
		initialize_empty();
		swap(other);
	}

	fixed_function(const fixed_function& other) 
		: call(other.call)
	{
		other.storage.function_table_ptr->call_copy(eastl::addressof(storage), eastl::addressof(other.storage));
	}

	template <typename T>
	fixed_function(
		T functor,
		typename eastl::enable_if<internal::is_valid_function_argument<T, Result(Arguments...)>::value, int>::type = 0)
	{
		if (internal::is_null(functor))
		{
			initialize_empty();
		}
		else
		{
			initialize(internal::to_functor(EASTL_FORWARD(T, functor)));
		}
	}

	fixed_function& operator=(const fixed_function& other) EA_NOEXCEPT
	{
		storage.function_table_ptr->call_destroy(eastl::addressof(storage));

		call = other.call;
		other.storage.function_table_ptr->call_copy(eastl::addressof(storage), eastl::addressof(other.storage));
		return *this;
	}

	fixed_function& operator=(fixed_function&& other) EA_NOEXCEPT
	{
		swap(other);
		other.initialize_empty();
		return *this;
	}

	fixed_function& operator=(std::nullptr_t)
	{
		if(call != nullptr)
			storage.function_table_ptr->call_destroy(eastl::addressof(storage));

		initialize_empty();
		return *this;
	}

	// TODO(rparolin):  Implement these APIs
	//
	// template< class F >
	// function& operator=( F&& f );

	// template< class F >
	// function& operator=( std::reference_wrapper<F> f );

	~fixed_function() EA_NOEXCEPT
	{
		storage.function_table_ptr->call_destroy(eastl::addressof(storage));
	}

	Result operator()(Arguments... arguments) const
	{
		return call(storage.functor, EASTL_FORWARD(Arguments, arguments)...);
	}

	void swap(fixed_function& other) EA_NOEXCEPT
	{
		total_storage_type_with_size temp_storage;
		other.storage.function_table_ptr->call_move_and_destroy(eastl::addressof(temp_storage), eastl::addressof(other.storage));
		storage.function_table_ptr->call_move_and_destroy(eastl::addressof(other.storage), eastl::addressof(storage));
		temp_storage.function_table_ptr->call_move_and_destroy(eastl::addressof(storage), eastl::addressof(temp_storage));

		eastl::swap(call, other.call);
	}

	#ifndef FUNC_NO_RTTI
	    const std::type_info& target_type() const EA_NOEXCEPT
		{
			return storage.function_table_ptr->call_type_id();
		}

		template<typename T>
		T* target() EA_NOEXCEPT
		{
			return static_cast<T*>(storage.function_table_ptr->call_target(eastl::addressof(storage), typeid(T)));
		}

		template<typename T>
		const T* target() const EA_NOEXCEPT
		{
			return static_cast<const T*>(storage.function_table_ptr->call_target(eastl::addressof(storage), typeid(T)));
		}
	#endif

	operator bool() const EA_NOEXCEPT { return call != nullptr; }

private:
	typedef ff_detail::total_storage_type<SIZE_IN_BYTES> total_storage_type_with_size;

	total_storage_type_with_size storage;
	Result (*call)(const ff_detail::functor_storage_type<SIZE_IN_BYTES>&, Arguments...);

	template<typename T>
	void initialize(T&& functor)
	{
		static_assert(sizeof(T) <= sizeof(typename total_storage_type_with_size::functor_storage_type_with_size), "fixed_function local buffer is not large enough to hold the callable.");

		call = &ff_detail::function_table_inplace_specialization<SIZE_IN_BYTES, T>::template call<Result, Arguments...>;
		ff_detail::init_function_table<T, total_storage_type_with_size>(storage);
		ff_detail::function_table_inplace_specialization<SIZE_IN_BYTES, T>::store_functor(storage, EASTL_FORWARD(T, functor));
	}

	void initialize_empty() EA_NOEXCEPT
	{
		typedef Result(*Empty_Function_Type)(Arguments...);
		ff_detail::init_function_table<Empty_Function_Type, total_storage_type_with_size>(storage);
		ff_detail::function_table_inplace_specialization<SIZE_IN_BYTES, Empty_Function_Type>::store_functor(storage, nullptr);
		call = nullptr;
	}
};


//////////////////////////////////////////////////////////////////////////////////////////////////
// comparison operators
//
template<int S, typename T>
bool operator==(std::nullptr_t, const fixed_function<S, T> & rhs) EA_NOEXCEPT { return !rhs; }

template<int S, typename T>
bool operator==(const fixed_function<S, T> & lhs, std::nullptr_t) EA_NOEXCEPT { return !lhs; }

template<int S, typename T>
bool operator!=(std::nullptr_t, const fixed_function<S, T> & rhs) EA_NOEXCEPT { return rhs; }

template<int S, typename T>
bool operator!=(const fixed_function<S, T> & lhs, std::nullptr_t) EA_NOEXCEPT { return lhs; }


//////////////////////////////////////////////////////////////////////////////////////////////////
// swap 
//
template<int S, typename T>
void swap(fixed_function<S, T> & lhs, fixed_function<S, T> & rhs) { lhs.swap(rhs); }

} // namespace eastl 

#undef FUNC_NO_RTTI

#endif // EASTL_INTERNAL_FIXED_FUNCTION_H
