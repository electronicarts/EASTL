/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// This file implements the eastl::any which is part of the C++ standard STL
// library specification.  
//
// eastl::any is a type-safe container for single values of any type.  Our
// implementation makes use of the "small local buffer" optimization to avoid
// unnecessary dynamic memory allocation if the specified type is a eligible to
// be stored in its local buffer.  The user type must satisfy the size
// requirements and must be no-throw move-constructible to qualify for the local
// buffer optimization.
//
// To consider:  Implement a fixed_any<SIZE> variant to allow users to customize
// the size of the "small local buffer" optimization.
//
// http://en.cppreference.com/w/cpp/utility/any
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ANY_H
#define EASTL_ANY_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif

#include <EASTL/internal/config.h>
#include <EASTL/internal/in_place_t.h>
#if EASTL_RTTI_ENABLED
	#include <typeinfo>
#endif
#if EASTL_EXCEPTIONS_ENABLED
	#include <exception>
#endif


namespace eastl
{
	///////////////////////////////////////////////////////////////////////////////
	// bad_any_cast
	// 
	// The type thrown by any_cast on failure.
	//
	// http://en.cppreference.com/w/cpp/utility/any/bad_any_cast
	//
	#if EASTL_EXCEPTIONS_ENABLED
	struct bad_cast : std::exception
	{
		const char* what() const EA_NOEXCEPT EA_OVERRIDE
			{ return "bad cast"; }
	};

	struct bad_any_cast : public bad_cast 
	{
		const char* what() const EA_NOEXCEPT EA_OVERRIDE
			{ return "bad_any_cast"; }
	};
    #endif

	namespace Internal
	{
		// utility to switch between exceptions and asserts
		inline void DoBadAnyCast()
		{
			#if EASTL_EXCEPTIONS_ENABLED
				throw bad_any_cast();
			#else
				EASTL_ASSERT_MSG(false, "bad_any_cast\n");

				// NOTE(rparolin): CRASH!
				// You crashed here because you requested a type that was not contained in the object.
				// We choose to intentionally crash here instead of returning invalid data to the calling 
				// code which could cause hard to track down bugs. 
				*((volatile int*)0) = 0xDEADC0DE;
			#endif
		}
	}


	///////////////////////////////////////////////////////////////////////////////
	// 20.7.3, class any
	//
	class any
	{
		//////////////////////////////////////////////////////////////////////////////////////////
		// storage_operation 
		//
		// operations supported by the storage handler 
		//
		enum class storage_operation
		{
			GET,
			DESTROY,
			COPY,
			MOVE,
			TYPE_INFO
		};


		//////////////////////////////////////////////////////////////////////////////////////////
		// storage 
		//
		// the underlying storage type which enables the switching between objects stored in
		// the heap and objects stored within the any type.
		//
		union storage
		{
			typedef aligned_storage_t<4 * sizeof(void*), alignment_of<void*>::value> internal_storage_t;

			void* external_storage = nullptr;
			internal_storage_t internal_storage;
		};


		//////////////////////////////////////////////////////////////////////////////////////////
		// use_internal_storage 
		//
		// determines when the "local buffer optimization" is used 
		//
		template <typename T>
		using use_internal_storage = bool_constant
		<
			is_nothrow_move_constructible<T>::value 
			&& (sizeof(T) <= sizeof(storage)) &&
			(alignment_of<storage>::value % alignment_of<T>::value == 0)  
		>;


		//////////////////////////////////////////////////////////////////////////////////////////
		// non-member friend functions	
		//
    	template <class ValueType> friend const ValueType* any_cast(const any* pAny) EA_NOEXCEPT;
		template <class ValueType> friend ValueType* any_cast(any* pAny) EA_NOEXCEPT; 
		template <class ValueType> friend ValueType any_cast(const any& operand);
		template <class ValueType> friend ValueType any_cast(any& operand);
		template <class ValueType> friend ValueType any_cast(any&& operand);


		//////////////////////////////////////////////////////////////////////////////////////////
		// internal storage handler
		//
		template <typename T>
		struct storage_handler_internal
		{
			template <typename V>
			static void construct(storage& s, V&& v)
			{
				::new(&s.internal_storage) T(eastl::forward<V>(v));
			}

			template <typename... Args>
			static void construct_inplace(storage& s, Args... args)
			{
				::new(&s.internal_storage) T(eastl::forward<Args>(args)...);
			}

			template <class NT, class U, class... Args>
			static void construct_inplace(storage& s, std::initializer_list<U> il, Args&&... args)
			{
				::new(&s.internal_storage) NT(il, eastl::forward<Args>(args)...);
			}

			static inline void destroy(any& refAny)
			{
				T& t = *static_cast<T*>(static_cast<void*>(&refAny.m_storage.internal_storage));
				EA_UNUSED(t);
				t.~T();

				refAny.m_handler = nullptr;
			}

			static void* handler_func(storage_operation op, const any* pThis, any* pOther)
			{
				switch (op)
				{
					case storage_operation::GET:
					{
						EASTL_ASSERT(pThis);
						return (void*)(&pThis->m_storage.internal_storage);
					}
					break;

					case storage_operation::DESTROY:
					{
						EASTL_ASSERT(pThis);
						destroy(const_cast<any&>(*pThis));
					}
					break;

					case storage_operation::COPY:
					{
						EASTL_ASSERT(pThis);
						EASTL_ASSERT(pOther);
						construct(pOther->m_storage, *(T*)(&pThis->m_storage.internal_storage));
					}
					break;

					case storage_operation::MOVE:
					{
						EASTL_ASSERT(pThis);
						EASTL_ASSERT(pOther);
						construct(pOther->m_storage, eastl::move(*(T*)(&pThis->m_storage.internal_storage)));
						destroy(const_cast<any&>(*pThis));
					}
					break;

					case storage_operation::TYPE_INFO:
					{
					#if EASTL_RTTI_ENABLED
						return (void*)&typeid(T);
					#endif
					}
					break;

					default:
					{
						EASTL_ASSERT_MSG(false, "unknown storage operation\n");
					}
					break;
				};

				return nullptr;
			}
		};


		//////////////////////////////////////////////////////////////////////////////////////////
		// external storage handler
		//
		template <typename T>
		struct storage_handler_external
		{
			template <typename V>
			static inline void construct(storage& s, V&& v) 
			{
				s.external_storage = ::new T(eastl::forward<V>(v));
			}

			template <typename... Args>
			static inline void construct_inplace(storage& s, Args... args)
			{
				s.external_storage = ::new T(eastl::forward<Args>(args)...);
			}

			template <class NT, class U, class... Args>
			static inline void construct_inplace(storage& s, std::initializer_list<U> il, Args&&... args)
			{
				s.external_storage = ::new NT(il, eastl::forward<Args>(args)...);
			}

			static inline void destroy(any& refAny)
			{
				delete static_cast<T*>(refAny.m_storage.external_storage);
				refAny.m_handler = nullptr;
			}

			static void* handler_func(storage_operation op, const any* pThis, any* pOther)
			{
				switch (op)
				{
					case storage_operation::GET:
					{
						EASTL_ASSERT(pThis);
						EASTL_ASSERT(pThis->m_storage.external_storage);
						return static_cast<void*>(pThis->m_storage.external_storage);
					}
					break;

					case storage_operation::DESTROY:
					{
						EASTL_ASSERT(pThis);
						destroy(*const_cast<any*>(pThis));
					}
					break;

					case storage_operation::COPY:
					{
						EASTL_ASSERT(pThis);
						EASTL_ASSERT(pOther);
						construct(pOther->m_storage, *static_cast<T*>(pThis->m_storage.external_storage));
					}
					break;

					case storage_operation::MOVE:
					{
						EASTL_ASSERT(pThis);
						EASTL_ASSERT(pOther);
						construct(pOther->m_storage, eastl::move(*(T*)(pThis->m_storage.external_storage)));
						destroy(const_cast<any&>(*pThis));
					}
					break;

					case storage_operation::TYPE_INFO:
					{
					#if EASTL_RTTI_ENABLED
						return (void*)&typeid(T);
					#endif
					}
					break;

					default:
					{
						EASTL_ASSERT_MSG(false, "unknown storage operation\n");
					}
					break;
				};

				return nullptr;
			}
		};


		//////////////////////////////////////////////////////////////////////////////////////////
		// storage_handler_ptr 
		//
		// defines the function signature of the storage handler that both the internal and
		// external storage handlers must implement to retrieve the underlying type of the any
		// object.
		//
		using storage_handler_ptr = void* (*)(storage_operation, const any*, any*);


		//////////////////////////////////////////////////////////////////////////////////////////
		// storage_handler 
		//
		// based on the specified type T we select the appropriate underlying storage handler
		// based on the 'use_internal_storage' trait.
		//
		template <typename T>
		using storage_handler = typename conditional<use_internal_storage<T>::value,
		                                             storage_handler_internal<T>,
		                                             storage_handler_external<T>>::type;


		//////////////////////////////////////////////////////////////////////////////////////////
		// data layout
		//
		storage m_storage;
		storage_handler_ptr m_handler;

	public:
			#ifndef EA_COMPILER_GNUC
				// TODO(rparolin):  renable constexpr for GCC
				EA_CONSTEXPR 
			#endif
			any() EA_NOEXCEPT 
			: m_storage(), m_handler(nullptr) {}

		any(const any& other) : m_handler(nullptr)
		{
			if (other.m_handler)
			{
				// NOTE(rparolin): You can not simply copy the underlying
				// storage because it could hold a pointer to an object on the
				// heap which breaks the copy semantics of the language. 
				other.m_handler(storage_operation::COPY, &other, this);
				m_handler = other.m_handler;
			}
		}

		any(any&& other) EA_NOEXCEPT : m_handler(nullptr)
		{ 
			if(other.m_handler)
			{
				// NOTE(rparolin): You can not simply move the underlying
				// storage because because the storage class has effectively
				// type erased user type so we have to defer to the handler
				// function to get the type back and pass on the move request.
				m_handler = eastl::move(other.m_handler);
				other.m_handler(storage_operation::MOVE, &other, this);
			}
		}

		~any() { reset(); }

		template <class ValueType>
		any(ValueType&& value,
		    typename eastl::enable_if<!eastl::is_same<typename eastl::decay<ValueType>::type, any>::value>::type* = 0)
		{
			typedef decay_t<ValueType> DecayedValueType;
			static_assert(is_copy_constructible<DecayedValueType>::value, "ValueType must be copy-constructible");
			storage_handler<DecayedValueType>::construct(m_storage, eastl::forward<ValueType>(value));
			m_handler = &storage_handler<DecayedValueType>::handler_func;
		}

		template <class T, class... Args>
		explicit any(in_place_type_t<T>, Args&&... args) 
		{
			typedef storage_handler<decay_t<T>> StorageHandlerT;
			static_assert(eastl::is_constructible<T, Args...>::value, "T must be constructible with Args...");

			StorageHandlerT::construct_inplace(m_storage, eastl::forward<Args>(args)...);
			m_handler = &StorageHandlerT::handler_func;
		}

		template <class T, class U, class... Args>
		explicit any(in_place_type_t<T>,
		             std::initializer_list<U> il,
		             Args&&... args,
		             typename eastl::enable_if<eastl::is_constructible<T, std::initializer_list<U>&, Args...>::value,
		                                       void>::type* = 0)
		{
			typedef storage_handler<decay_t<T>> StorageHandlerT;

			StorageHandlerT::construct_inplace(m_storage, il, eastl::forward<Args>(args)...);
			m_handler = &StorageHandlerT::handler_func;
		}

		// 20.7.3.2, assignments
		template <class ValueType>
		any& operator=(ValueType&& value)
		{
			static_assert(is_copy_constructible<decay_t<ValueType>>::value, "ValueType must be copy-constructible");
			any(eastl::forward<ValueType>(value)).swap(*this);
			return *this;
		}

		any& operator=(const any& other) 
		{ 
			any(other).swap(*this);
			return *this; 
		}

		any& operator=(any&& other) EA_NOEXCEPT 
		{ 
			any(eastl::move(other)).swap(*this);
			return *this; 
		}

        // 20.7.3.3, modifiers
		#if EASTL_VARIADIC_TEMPLATES_ENABLED
			template <class T, class... Args>
			void emplace(Args&&... args)
			{
			    typedef storage_handler<decay_t<T>> StorageHandlerT;
				static_assert(eastl::is_constructible<T, Args...>::value, "T must be constructible with Args...");

			    reset();
				StorageHandlerT::construct_inplace(m_storage, eastl::forward<Args>(args)...);
				m_handler = &StorageHandlerT::handler_func;
			}

			template <class NT, class U, class... Args>
		    typename eastl::enable_if<eastl::is_constructible<NT, std::initializer_list<U>&, Args...>::value, void>::type
			emplace(std::initializer_list<U> il, Args&&... args)
			{
			    typedef storage_handler<decay_t<NT>> StorageHandlerT;

				reset();
				StorageHandlerT::construct_inplace(m_storage, il, eastl::forward<Args>(args)...);
				m_handler = &StorageHandlerT::handler_func;
			}
        #endif

		void reset() EA_NOEXCEPT 
		{
			if(m_handler)
				m_handler(storage_operation::DESTROY, this, nullptr);
		}

		void swap(any& other) EA_NOEXCEPT 
		{
			if(this == &other)
				return;

			if(m_handler && other.m_handler)
			{
				any tmp;
				tmp.m_handler = other.m_handler;
				other.m_handler(storage_operation::MOVE, &other, &tmp);

				other.m_handler = m_handler;
				m_handler(storage_operation::MOVE, this, &other);

				m_handler = tmp.m_handler;
				tmp.m_handler(storage_operation::MOVE, &tmp, this);
			}
			else if (m_handler == nullptr)
			{
				eastl::swap(m_handler, other.m_handler);
				m_handler(storage_operation::MOVE, &other, this);
			}
			else if(other.m_handler == nullptr)
			{
				eastl::swap(m_handler, other.m_handler);
				other.m_handler(storage_operation::MOVE, this, &other);
			}
		}

	    // 20.7.3.4, observers
		bool has_value() const EA_NOEXCEPT { return m_handler != nullptr; }

        #if EASTL_RTTI_ENABLED
			inline const std::type_info& type() const EA_NOEXCEPT 
			{
				if(m_handler)
				{
					auto* pTypeInfo = m_handler(storage_operation::TYPE_INFO, this, nullptr);
					return *static_cast<const std::type_info*>(pTypeInfo);
				}
				else
				{
					return typeid(void);
				}
			}
		#endif
	};



	//////////////////////////////////////////////////////////////////////////////////////////
	// 20.7.4, non-member functions
	//
	inline void swap(any& rhs, any& lhs) EA_NOEXCEPT { rhs.swap(lhs); }


	//////////////////////////////////////////////////////////////////////////////////////////
	// 20.7.4, The non-member any_cast functions provide type-safe access to the contained object.
	//
	template <class ValueType>
	inline ValueType any_cast(const any& operand)
	{
		static_assert(eastl::is_reference<ValueType>::value || eastl::is_copy_constructible<ValueType>::value,
		              "ValueType must be a reference or copy constructible");

		auto* p = any_cast<typename add_const<typename remove_reference<ValueType>::type>::type>(&operand);

		if(p == nullptr)
			Internal::DoBadAnyCast();

		return *p;
	}

	template <class ValueType>
    inline ValueType any_cast(any& operand)
    {
		static_assert(eastl::is_reference<ValueType>::value || eastl::is_copy_constructible<ValueType>::value,
		              "ValueType must be a reference or copy constructible");

		auto* p = any_cast<typename remove_reference<ValueType>::type>(&operand);

		if(p == nullptr)
			Internal::DoBadAnyCast();

		return *p;
    }

	template <class ValueType>
	inline ValueType any_cast(any&& operand)
	{
		static_assert(eastl::is_reference<ValueType>::value || eastl::is_copy_constructible<ValueType>::value,
		              "ValueType must be a reference or copy constructible");

		auto* p = any_cast<typename remove_reference<ValueType>::type>(&operand);

		if (p == nullptr)
			Internal::DoBadAnyCast();

		return *p;
	}

	// NOTE(rparolin): The runtime type check was commented out because in DLL builds the templated function pointer
	// value will be different -- completely breaking the validation mechanism.  Due to the fact that eastl::any uses
	// type erasure we can't refesh (on copy/move) the cached function pointer to the internal handler function because
	// we don't statically know the type.
	template <class ValueType>
	inline const ValueType* any_cast(const any* pAny) EA_NOEXCEPT
	{
		return (pAny && pAny->m_handler //== &any::storage_handler<decay_t<ValueType>>::handler_func
				#if EASTL_RTTI_ENABLED
					&& pAny->type() == typeid(typename remove_reference<ValueType>::type)
				#endif
				) ?
		           static_cast<const ValueType*>(pAny->m_handler(any::storage_operation::GET, pAny, nullptr)) :
		           nullptr;
	}

	template <class ValueType>
	inline ValueType* any_cast(any* pAny) EA_NOEXCEPT
	{
		return (pAny && pAny->m_handler //== &any::storage_handler<decay_t<ValueType>>::handler_func
				#if EASTL_RTTI_ENABLED
					&& pAny->type() == typeid(typename remove_reference<ValueType>::type)
				#endif
				) ?
		           static_cast<ValueType*>(pAny->m_handler(any::storage_operation::GET, pAny, nullptr)) :
		           nullptr;
	}

    //////////////////////////////////////////////////////////////////////////////////////////
	// make_any
	//
	#if EASTL_VARIADIC_TEMPLATES_ENABLED
		template <class T, class... Args>
		inline any make_any(Args&&... args)
		{
			return any(eastl::in_place<T>, eastl::forward<Args>(args)...);
		}

		template <class T, class U, class... Args>
		inline any make_any(std::initializer_list<U> il, Args&&... args)
		{
			return any(eastl::in_place<T>, il, eastl::forward<Args>(args)...);
		}
    #endif

} // namespace eastl

#endif // EASTL_ANY_H
