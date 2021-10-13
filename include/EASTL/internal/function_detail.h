///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_FUNCTION_DETAIL_H
#define EASTL_FUNCTION_DETAIL_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif

#include <EABase/eabase.h>
#include <EABase/nullptr.h>
#include <EABase/config/eacompilertraits.h>

#include <EASTL/internal/config.h>
#include <EASTL/internal/functional_base.h>
#include <EASTL/internal/move_help.h>
#include <EASTL/internal/function_help.h>

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>
#include <EASTL/allocator.h>

#if EASTL_RTTI_ENABLED
	#include <typeinfo>
#endif

#if EASTL_EXCEPTIONS_ENABLED
	EA_DISABLE_ALL_VC_WARNINGS()
	#include <new>
	#include <exception>
	EA_RESTORE_ALL_VC_WARNINGS()
#endif

namespace eastl
{
	#if EASTL_EXCEPTIONS_ENABLED
		class bad_function_call : public std::exception
		{
		public:
			bad_function_call() EA_NOEXCEPT = default;

			const char* what() const EA_NOEXCEPT EA_OVERRIDE
			{
				return "bad function_detail call";
			}
		};
	#endif

	namespace internal
	{
		class unused_class {};

		union functor_storage_alignment
		{
			void (*unused_func_ptr)(void);
			void (unused_class::*unused_func_mem_ptr)(void);
			void* unused_ptr;
		};

		template <int SIZE_IN_BYTES>
		struct functor_storage
		{
			static_assert(SIZE_IN_BYTES >= 0, "local buffer storage cannot have a negative size!");
			template <typename Ret>
			Ret& GetStorageTypeRef() const
			{
				return *reinterpret_cast<Ret*>(const_cast<char*>(&storage[0]));
			}

			union
			{
				functor_storage_alignment align;
				char storage[SIZE_IN_BYTES];
			};
		};

		template <>
		struct functor_storage<0>
		{
			template <typename Ret>
			Ret& GetStorageTypeRef() const
			{
				return *reinterpret_cast<Ret*>(const_cast<char*>(&storage[0]));
			}

			union
			{
				functor_storage_alignment align;
				char storage[sizeof(functor_storage_alignment)];
			};
		};

		template <typename Functor, int SIZE_IN_BYTES>
		struct is_functor_inplace_allocatable
		{
			static EA_CONSTEXPR bool value =
			    sizeof(Functor) <= sizeof(functor_storage<SIZE_IN_BYTES>) &&
			    (eastl::alignment_of_v<functor_storage<SIZE_IN_BYTES>> % eastl::alignment_of_v<Functor>) == 0;
		};


		/// function_base_detail
		///
		template <int SIZE_IN_BYTES>
		class function_base_detail
		{
		public:
			using FunctorStorageType = functor_storage<SIZE_IN_BYTES>;
			FunctorStorageType mStorage;

			enum ManagerOperations : int
			{
				MGROPS_DESTRUCT_FUNCTOR = 0,
				MGROPS_COPY_FUNCTOR = 1,
				MGROPS_MOVE_FUNCTOR = 2,
			#if EASTL_RTTI_ENABLED
				MGROPS_GET_TYPE_INFO = 3,
				MGROPS_GET_FUNC_PTR = 4,
			#endif
			};

			// Functor can be allocated inplace
			template <typename Functor, typename = void>
			class function_manager_base
			{
			public:

				static Functor* GetFunctorPtr(const FunctorStorageType& storage) EA_NOEXCEPT
				{
					return &(storage.template GetStorageTypeRef<Functor>());
				}

				template <typename T>
				static void CreateFunctor(FunctorStorageType& storage, T&& functor)
				{
					::new (GetFunctorPtr(storage)) Functor(eastl::forward<T>(functor));
				}

				static void DestructFunctor(FunctorStorageType& storage)
				{
					GetFunctorPtr(storage)->~Functor();
				}

				static void CopyFunctor(FunctorStorageType& to, const FunctorStorageType& from)
				{
					::new (GetFunctorPtr(to)) Functor(*GetFunctorPtr(from));
				}

				static void MoveFunctor(FunctorStorageType& to, FunctorStorageType& from) EA_NOEXCEPT
				{
					::new (GetFunctorPtr(to)) Functor(eastl::move(*GetFunctorPtr(from)));
				}

				static void* Manager(void* to, void* from, typename function_base_detail::ManagerOperations ops) EA_NOEXCEPT
				{
					switch (ops)
					{
						case MGROPS_DESTRUCT_FUNCTOR:
						{
							DestructFunctor(*static_cast<FunctorStorageType*>(to));
						}
						break;
						case MGROPS_COPY_FUNCTOR:
						{
							CopyFunctor(*static_cast<FunctorStorageType*>(to),
							            *static_cast<const FunctorStorageType*>(from));
						}
						break;
						case MGROPS_MOVE_FUNCTOR:
						{
							MoveFunctor(*static_cast<FunctorStorageType*>(to), *static_cast<FunctorStorageType*>(from));
							DestructFunctor(*static_cast<FunctorStorageType*>(from));
						}
						break;
						default:
							break;
					}
					return nullptr;
				}
			};

			// Functor is allocated on the heap
			template <typename Functor>
			class function_manager_base<Functor, typename eastl::enable_if<!is_functor_inplace_allocatable<Functor, SIZE_IN_BYTES>::value>::type>
			{
			public:
				static Functor* GetFunctorPtr(const FunctorStorageType& storage) EA_NOEXCEPT
				{
					return storage.template GetStorageTypeRef<Functor*>();
				}

				static Functor*& GetFunctorPtrRef(const FunctorStorageType& storage) EA_NOEXCEPT
				{
					return storage.template GetStorageTypeRef<Functor*>();
				}

				template <typename T>
				static void CreateFunctor(FunctorStorageType& storage, T&& functor)
				{
					auto& allocator = *EASTLAllocatorDefault();
					Functor* func = static_cast<Functor*>(allocator.allocate(sizeof(Functor), alignof(Functor), 0));

				#if EASTL_EXCEPTIONS_ENABLED
					if (!func)
					{
						throw std::bad_alloc();
					}
				#else
					EASTL_ASSERT_MSG(func != nullptr, "Allocation failed!");
				#endif

					::new (static_cast<void*>(func)) Functor(eastl::forward<T>(functor));
					GetFunctorPtrRef(storage) = func;
				}

				static void DestructFunctor(FunctorStorageType& storage)
				{
					Functor* func = GetFunctorPtr(storage);
					if (func)
					{
						auto& allocator = *EASTLAllocatorDefault();
						func->~Functor();
						allocator.deallocate(static_cast<void*>(func), sizeof(Functor));
					}
				}

				static void CopyFunctor(FunctorStorageType& to, const FunctorStorageType& from)
				{
					auto& allocator = *EASTLAllocatorDefault();
					Functor* func = static_cast<Functor*>(allocator.allocate(sizeof(Functor), alignof(Functor), 0));
				#if EASTL_EXCEPTIONS_ENABLED
					if (!func)
					{
						throw std::bad_alloc();
					}
				#else
					EASTL_ASSERT_MSG(func != nullptr, "Allocation failed!");
				#endif
					::new (static_cast<void*>(func)) Functor(*GetFunctorPtr(from));
					GetFunctorPtrRef(to) = func;
				}

				static void MoveFunctor(FunctorStorageType& to, FunctorStorageType& from) EA_NOEXCEPT
				{
					Functor* func = GetFunctorPtr(from);
					GetFunctorPtrRef(to) = func;
					GetFunctorPtrRef(from) = nullptr;
				}

				static void* Manager(void* to, void* from, typename function_base_detail::ManagerOperations ops) EA_NOEXCEPT
				{
					switch (ops)
					{
						case MGROPS_DESTRUCT_FUNCTOR:
						{
							DestructFunctor(*static_cast<FunctorStorageType*>(to));
						}
						break;
						case MGROPS_COPY_FUNCTOR:
						{
							CopyFunctor(*static_cast<FunctorStorageType*>(to),
							            *static_cast<const FunctorStorageType*>(from));
						}
						break;
						case MGROPS_MOVE_FUNCTOR:
						{
							MoveFunctor(*static_cast<FunctorStorageType*>(to), *static_cast<FunctorStorageType*>(from));
							// Moved ptr, no need to destruct ourselves
						}
						break;
						default:
							break;
					}
					return nullptr;
				}
			};

			template <typename Functor, typename R, typename... Args>
			class function_manager final : public function_manager_base<Functor>
			{
			public:
				using Base = function_manager_base<Functor>;

			#if EASTL_RTTI_ENABLED
				static void* GetTypeInfo() EA_NOEXCEPT
				{
					return reinterpret_cast<void*>(const_cast<std::type_info*>(&typeid(Functor)));
				}

				static void* Manager(void* to, void* from, typename function_base_detail::ManagerOperations ops) EA_NOEXCEPT
				{
					switch (ops)
					{
						case MGROPS_GET_TYPE_INFO:
						{
							return GetTypeInfo();
						}
						break;
						case MGROPS_GET_FUNC_PTR:
						{
							return static_cast<void*>(Base::GetFunctorPtr(*static_cast<FunctorStorageType*>(to)));
						}
						break;
						default:
						{
							return Base::Manager(to, from, ops);
						}
						break;
					}
				}
			#endif // EASTL_RTTI_ENABLED

				/**
				 * NOTE:
				 *
				 * The order of arguments here is vital to the call optimization. Let's dig into why and look at some asm.
				 * We have two invoker signatures to consider:
				 *   R Invoker(const FunctorStorageType& functor, Args... args)
				 *   R Invoker(Args... args, const FunctorStorageType& functor)
				 *
				 * Assume we are using the Windows x64 Calling Convention where the first 4 arguments are passed into
				 * RCX, RDX, R8, R9. This optimization works for any Calling Convention, we are just using Windows x64 for
				 * this example.
				 *
				 * Given the following member function: void TestMemberFunc(int a, int b)
				 *  RCX == this
				 *  RDX == a
				 *  R8  == b
				 *
				 * All three arguments to the function including the hidden this pointer, which in C++ is always the first argument
				 * are passed into the first three registers.
				 * The function call chain for eastl::function<>() is as follows:
				 *  operator ()(this, Args... args) -> Invoker(Args... args, this->mStorage) -> StoredFunction(Args... arg)
				 *
				 * Let's look at what is happening at the asm level with the different Invoker function signatures and why.
				 *
				 * You will notice that operator ()() and Invoker() have the arguments reversed. operator ()() just directly calls
				 * to Invoker(), it is a tail call, so we force inline the call operator to ensure we directly call to the Invoker().
				 * Most compilers always inline it anyways by default; have been instances where it doesn't even though the asm ends
				 * up being cheaper.
				 * call -> call -> call versus call -> call
				 *
				 * eastl::function<int(int, int)> = FunctionPointer
				 *
				 * Assume we have the above eastl::function object that holds a pointer to a function as the internal callable.
				 *
				 * Invoker(this->mStorage, Args... args) is called with the follow arguments in registers:
				 *  RCX = this  |  RDX = a  |  R8 = b
				 *
				 * Inside Invoker() we use RCX to deference into the eastl::function object and get the function pointer to call.
				 * This function to call has signature Func(int, int) and thus requires its arguments in registers RCX and RDX.
				 * The compiler must shift all the arguments towards the left. The full asm looks something as follows.
				 *
				 * Calling Invoker:                       Inside Invoker:
				 *
				 * mov rcx, this                          mov rax, [rcx]
				 * mov rdx, a                             mov rcx, rdx
				 * mov r8, b                              mov rdx, r8
				 * call [rcx + offset to Invoker]         jmp [rax]
				 *
				 * Notice how the compiler shifts all the arguments before calling the callable and also we only use the this pointer
				 * to access the internal storage inside the eastl::function object.
				 *
				 * Invoker(Args... args, this->mStorage) is called with the following arguments in registers:
				 *  RCX = a  |  RDX = b  |  R8 = this
				 *
				 * You can see we no longer have to shift the arguments down when going to call the internal stored callable.
				 *
				 * Calling Invoker:                      Inside Invoker:
				 *
				 * mov rcx, a                            mov rax, [r8]
				 * mov rdx, b                            jmp [rax]
				 * mov r8, this
				 * call [r8 + offset to Invoker]
				 *
				 * The generated asm does a straight tail jmp to the loaded function pointer. The arguments are already in the correct
				 * registers.
				 *
				 * For Functors or Lambdas with no captures, this gives us another free register to use to pass arguments since the this
				 * is at the end, it can be passed onto the stack if we run out of registers. Since the callable has no captures; inside
				 * the Invoker(), we won't ever need to touch this thus we can just call the operator ()() or let the compiler inline it.
				 *
				 * For a callable with captures there is no perf hit since the callable in the common case is inlined and the pointer to the callable
				 * buffer is passed in a register which the compiler can use to access the captures.
				 *
				 * For eastl::function<void(const T&, int, int)> that a holds a pointer to member function. The this pointers is implicitly
				 * the first argument in the argument list, const T&, and the member function pointer will be called on that object.
				 * This prevents any argument shifting since the this for the member function pointer is already in RCX.
				 *
				 * This is why having this at the end of the argument list is important for generating efficient Invoker() thunks.
				 */
				static R Invoker(Args... args, const FunctorStorageType& functor)
				{
					return eastl::invoke(*Base::GetFunctorPtr(functor), eastl::forward<Args>(args)...);
				}
			};

			function_base_detail() EA_NOEXCEPT = default;
			~function_base_detail() EA_NOEXCEPT = default;
		};

		#define EASTL_INTERNAL_FUNCTION_VALID_FUNCTION_ARGS(FUNCTOR, RET, ARGS, BASE, MYSELF)  \
			typename eastl::enable_if_t<eastl::is_invocable_r_v<RET, FUNCTOR, ARGS> &&         \
										!eastl::is_base_of_v<BASE, eastl::decay_t<FUNCTOR>> && \
										!eastl::is_same_v<eastl::decay_t<FUNCTOR>, MYSELF>>

		#define EASTL_INTERNAL_FUNCTION_DETAIL_VALID_FUNCTION_ARGS(FUNCTOR, RET, ARGS, MYSELF) \
			EASTL_INTERNAL_FUNCTION_VALID_FUNCTION_ARGS(FUNCTOR, RET, ARGS, MYSELF, MYSELF)


		/// function_detail
		///
		template <int, typename>
		class function_detail;

		template <int SIZE_IN_BYTES, typename R, typename... Args>
		class function_detail<SIZE_IN_BYTES, R(Args...)> : public function_base_detail<SIZE_IN_BYTES>
		{
		public:
			using result_type = R;

		protected:
			using Base = function_base_detail<SIZE_IN_BYTES>;
			using FunctorStorageType = typename function_base_detail<SIZE_IN_BYTES>::FunctorStorageType;
			using Base::mStorage;

		public:
			function_detail() EA_NOEXCEPT = default;
			function_detail(std::nullptr_t) EA_NOEXCEPT {}

			function_detail(const function_detail& other)
			{
				if (this != &other)
				{
					Copy(other);
				}
			}

			function_detail(function_detail&& other)
			{
				if (this != &other)
				{
					Move(eastl::move(other));
				}
			}

			template <typename Functor, typename = EASTL_INTERNAL_FUNCTION_DETAIL_VALID_FUNCTION_ARGS(Functor, R, Args..., function_detail)>
			function_detail(Functor functor)
			{
				CreateForwardFunctor(eastl::move(functor));
			}

			~function_detail() EA_NOEXCEPT
			{
				Destroy();
			}

			function_detail& operator=(const function_detail& other)
			{
				if (this != &other)
				{
					Destroy();
					Copy(other);
				}

				return *this;
			}

			function_detail& operator=(function_detail&& other)
			{
				if(this != &other)
				{
					Destroy();
					Move(eastl::move(other));
				}

				return *this;
			}

			function_detail& operator=(std::nullptr_t) EA_NOEXCEPT
			{
				Destroy();
				mMgrFuncPtr = nullptr;
				mInvokeFuncPtr = &DefaultInvoker;

				return *this;
			}

			template <typename Functor, typename = EASTL_INTERNAL_FUNCTION_DETAIL_VALID_FUNCTION_ARGS(Functor, R, Args..., function_detail)>
			function_detail& operator=(Functor&& functor)
			{
				Destroy();
				CreateForwardFunctor(eastl::forward<Functor>(functor));
				return *this;
			}

			template <typename Functor>
			function_detail& operator=(eastl::reference_wrapper<Functor> f) EA_NOEXCEPT
			{
				Destroy();
				CreateForwardFunctor(f);
				return *this;
			}

			void swap(function_detail& other) EA_NOEXCEPT
			{
				if(this == &other)
					return;

				FunctorStorageType tempStorage;
				if (other.HaveManager())
				{
					(void)(*other.mMgrFuncPtr)(static_cast<void*>(&tempStorage), static_cast<void*>(&other.mStorage),
											   Base::ManagerOperations::MGROPS_MOVE_FUNCTOR);
				}

				if (HaveManager())
				{
					(void)(*mMgrFuncPtr)(static_cast<void*>(&other.mStorage), static_cast<void*>(&mStorage),
										 Base::ManagerOperations::MGROPS_MOVE_FUNCTOR);
				}

				if (other.HaveManager())
				{
					(void)(*other.mMgrFuncPtr)(static_cast<void*>(&mStorage), static_cast<void*>(&tempStorage),
											   Base::ManagerOperations::MGROPS_MOVE_FUNCTOR);
				}

				eastl::swap(mMgrFuncPtr, other.mMgrFuncPtr);
				eastl::swap(mInvokeFuncPtr, other.mInvokeFuncPtr);
			}

			explicit operator bool() const EA_NOEXCEPT
			{
				return HaveManager();
			}

			EASTL_FORCE_INLINE R operator ()(Args... args) const
			{
				return (*mInvokeFuncPtr)(eastl::forward<Args>(args)..., this->mStorage);
			}

			#if EASTL_RTTI_ENABLED
				const std::type_info& target_type() const EA_NOEXCEPT
				{
					if (HaveManager())
					{
						void* ret = (*mMgrFuncPtr)(nullptr, nullptr, Base::ManagerOperations::MGROPS_GET_TYPE_INFO);
						return *(static_cast<const std::type_info*>(ret));
					}
					return typeid(void);
				}

				template <typename Functor>
				Functor* target() EA_NOEXCEPT
				{
					if (HaveManager() && target_type() == typeid(Functor))
					{
						void* ret = (*mMgrFuncPtr)(static_cast<void*>(&mStorage), nullptr,
												   Base::ManagerOperations::MGROPS_GET_FUNC_PTR);
						return ret ? static_cast<Functor*>(ret) : nullptr;
					}
					return nullptr;
				}

				template <typename Functor>
				const Functor* target() const EA_NOEXCEPT
				{
					if (HaveManager() && target_type() == typeid(Functor))
					{
						void* ret = (*mMgrFuncPtr)(static_cast<void*>(&mStorage), nullptr,
												   Base::ManagerOperations::MGROPS_GET_FUNC_PTR);
						return ret ? static_cast<const Functor*>(ret) : nullptr;
					}
					return nullptr;
				}
			#endif // EASTL_RTTI_ENABLED

		private:
			bool HaveManager() const EA_NOEXCEPT
			{
				return (mMgrFuncPtr != nullptr);
			}

			void Destroy() EA_NOEXCEPT
			{
				if (HaveManager())
				{
					(void)(*mMgrFuncPtr)(static_cast<void*>(&mStorage), nullptr,
					                     Base::ManagerOperations::MGROPS_DESTRUCT_FUNCTOR);
				}
			}

			void Copy(const function_detail& other)
			{
				if (other.HaveManager())
				{
					(void)(*other.mMgrFuncPtr)(static_cast<void*>(&mStorage),
					                           const_cast<void*>(static_cast<const void*>(&other.mStorage)),
					                           Base::ManagerOperations::MGROPS_COPY_FUNCTOR);
				}

				mMgrFuncPtr = other.mMgrFuncPtr;
				mInvokeFuncPtr = other.mInvokeFuncPtr;
			}

			void Move(function_detail&& other)
			{
				if (other.HaveManager())
				{
					(void)(*other.mMgrFuncPtr)(static_cast<void*>(&mStorage), static_cast<void*>(&other.mStorage),
					                           Base::ManagerOperations::MGROPS_MOVE_FUNCTOR);
				}

				mMgrFuncPtr = other.mMgrFuncPtr;
				mInvokeFuncPtr = other.mInvokeFuncPtr;
				other.mMgrFuncPtr = nullptr;
				other.mInvokeFuncPtr = &DefaultInvoker;
			}

			template <typename Functor>
			void CreateForwardFunctor(Functor&& functor)
			{
				using DecayedFunctorType = typename eastl::decay<Functor>::type;
				using FunctionManagerType = typename Base::template function_manager<DecayedFunctorType, R, Args...>;

				if (internal::is_null(functor))
				{
					mMgrFuncPtr = nullptr;
					mInvokeFuncPtr = &DefaultInvoker;
				}
				else
				{
					mMgrFuncPtr = &FunctionManagerType::Manager;
					mInvokeFuncPtr = &FunctionManagerType::Invoker;
					FunctionManagerType::CreateFunctor(mStorage, eastl::forward<Functor>(functor));
				}
			}

		private:
			typedef void* (*ManagerFuncPtr)(void*, void*, typename Base::ManagerOperations);
			typedef R (*InvokeFuncPtr)(Args..., const FunctorStorageType&);

			EA_DISABLE_GCC_WARNING(-Wreturn-type);
			EA_DISABLE_CLANG_WARNING(-Wreturn-type);
			EA_DISABLE_VC_WARNING(4716); // 'function' must return a value
			// We cannot assume that R is default constructible.
			// This function is called only when the function object CANNOT be called because it is empty,
			// it will always throw or assert so we never use the return value anyways and neither should the caller.
			static R DefaultInvoker(Args... /*args*/, const FunctorStorageType& /*functor*/)
			{
				#if EASTL_EXCEPTIONS_ENABLED
					throw eastl::bad_function_call();
				#else
					EASTL_ASSERT_MSG(false, "function_detail call on an empty function_detail<R(Args..)>");
				#endif
			};
			EA_RESTORE_VC_WARNING();
			EA_RESTORE_CLANG_WARNING();
			EA_RESTORE_GCC_WARNING();


			ManagerFuncPtr mMgrFuncPtr = nullptr;
			InvokeFuncPtr mInvokeFuncPtr = &DefaultInvoker;
		};

	} // namespace internal

} // namespace eastl

#endif // EASTL_FUNCTION_DETAIL_H
