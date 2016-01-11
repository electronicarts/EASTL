/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_INTERNAL_THREAD_SUPPORT_H
#define EASTL_INTERNAL_THREAD_SUPPORT_H


#include <EABase/eabase.h>
#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif
#include <EASTL/internal/config.h>

#if defined(EA_HAVE_CPP11_MUTEX) && !defined(EA_PLATFORM_MICROSOFT) && !defined(EA_PLATFORM_UNIX) // We stick with platform-specific mutex support to the extent possible, as it's currently more reliably available.
	#define EASTL_CPP11_MUTEX_ENABLED 1
#else
	#define EASTL_CPP11_MUTEX_ENABLED 0
#endif

#if EASTL_CPP11_MUTEX_ENABLED
	#include <mutex>
#endif

#if defined(EA_PLATFORM_MICROSOFT)
	// Cannot include Windows headers in our headers, as they kill builds with their #defines.
#elif defined(EA_PLATFORM_POSIX)
	#include <pthread.h>
#endif


#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 4625) // copy constructor could not be generated because a base class copy constructor is inaccessible or deleted.
	#pragma warning(disable: 4626) // assignment operator could not be generated because a base class assignment operator is inaccessible or deleted.
	#pragma warning(disable: 4275) // non dll-interface class used as base for DLL-interface classkey 'identifier'.
#endif

	
#if defined(EA_PLATFORM_MICROSOFT)
	#if defined(EA_PROCESSOR_POWERPC)
		extern "C" long  __stdcall _InterlockedIncrement(long volatile* Addend);
		#pragma intrinsic (_InterlockedIncrement)

		extern "C" long  __stdcall _InterlockedDecrement(long volatile* Addend);
		#pragma intrinsic (_InterlockedDecrement)

		extern "C" long  __stdcall _InterlockedCompareExchange(long volatile* Dest, long Exchange, long Comp);
		#pragma intrinsic (_InterlockedCompareExchange)
	#else
		extern "C" long  _InterlockedIncrement(long volatile* Addend);
		#pragma intrinsic (_InterlockedIncrement)

		extern "C" long _InterlockedDecrement(long volatile* Addend);
		#pragma intrinsic (_InterlockedDecrement)

		extern "C" long _InterlockedCompareExchange(long volatile* Dest, long Exchange, long Comp);
		#pragma intrinsic (_InterlockedCompareExchange)
	#endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EASTL_THREAD_SUPPORT_AVAILABLE
//
// Defined as 0 or 1, based on existing support.
// Identifies if thread support (e.g. atomics, mutexes) is available for use.
// The large majority of EASTL doesn't use thread support, but a few parts 
// of it (e.g. shared_ptr) do.
///////////////////////////////////////////////////////////////////////////////

#if !defined(EASTL_THREAD_SUPPORT_AVAILABLE)
	#if defined(EA_COMPILER_CLANG) || (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4003))
		#define EASTL_THREAD_SUPPORT_AVAILABLE 1
	#elif defined(EA_COMPILER_MSVC)
		#define EASTL_THREAD_SUPPORT_AVAILABLE 1
	#else
		#define EASTL_THREAD_SUPPORT_AVAILABLE 0
	#endif
#endif


namespace eastl
{
	namespace Internal
	{
		/// atomic_increment
		/// Returns the new value.
		inline int32_t atomic_increment(int32_t* p32) EA_NOEXCEPT
		{
			#if defined(EA_COMPILER_CLANG) || (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4003))
				return __sync_add_and_fetch(p32, 1);
			#elif defined(EA_COMPILER_MSVC)
				static_assert(sizeof(long) == sizeof(int32_t), "unexpected size");
				return _InterlockedIncrement((volatile long*)p32);
			#elif defined(EA_COMPILER_GNUC)
				int32_t result;
				__asm__ __volatile__ ("lock; xaddl %0, %1"
									: "=r" (result), "=m" (*p32)
									: "0" (1), "m" (*p32)
									: "memory"
									);
				return result + 1;
			#else
				EASTL_FAIL_MSG("EASTL thread safety is not implemented yet. See EAThread for how to do this for the given platform.");
				return ++*p32;
			#endif
		}

		/// atomic_decrement
		/// Returns the new value.
		inline int32_t atomic_decrement(int32_t* p32) EA_NOEXCEPT
		{
			#if defined(EA_COMPILER_CLANG) || (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4003))
				return __sync_add_and_fetch(p32, -1);
			#elif defined(EA_COMPILER_MSVC)
				return _InterlockedDecrement((volatile long*)p32); // volatile long cast is OK because int32_t == long on Microsoft platforms.
			#elif defined(EA_COMPILER_GNUC)
				int32_t result;
				__asm__ __volatile__ ("lock; xaddl %0, %1"
									: "=r" (result), "=m" (*p32)
									: "0" (-1), "m" (*p32)
									: "memory"
									);
				return result - 1;
			#else
				EASTL_FAIL_MSG("EASTL thread safety is not implemented yet. See EAThread for how to do this for the given platform.");
				return --*p32;
			#endif
		}


		/// atomic_compare_and_swap
		/// Safely sets the value to a new value if the original value is equal to
		/// a condition value. Returns true if the condition was met and the
		/// assignment occurred. The comparison and value setting are done as
		/// an atomic operation and thus another thread cannot intervene between
		/// the two as would be the case with simple C code.
		inline bool atomic_compare_and_swap(int32_t* p32, int32_t newValue, int32_t condition)
		{
			#if defined(EA_COMPILER_CLANG) || (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4003))
				return __sync_bool_compare_and_swap(p32, condition, newValue);
			#elif defined(EA_COMPILER_MSVC)
				return ((int32_t)_InterlockedCompareExchange((volatile long*)p32, (long)newValue, (long)condition) == condition);
			#elif defined(EA_COMPILER_GNUC)
				// GCC Inline ASM Constraints     
				// r  <--> Any general purpose register  
				// a  <--> The a register.  
				// 1  <--> The constraint '1' for operand 2 says that it must occupy the same location as operand 1.
				// =a <--> output registers 
				// =r <--> output registers 

				int32_t result;
				__asm__ __volatile__(
					"lock; cmpxchgl %3, (%1) \n"                    // Test *p32 against EAX, if same, then *p32 = newValue 
					: "=a" (result), "=r" (p32)                     // outputs
					: "a" (condition), "r" (newValue), "1" (p32)    // inputs
					: "memory"                                      // clobbered
					);
				return result == condition;
			#else
				EASTL_FAIL_MSG("EASTL thread safety is not implemented yet. See EAThread for how to do this for the given platform.");
				if(*p32 == condition)
				{
					*p32 = newValue;
					return true;
				}
				return false;
			#endif
		}


		// mutex
		#if EASTL_CPP11_MUTEX_ENABLED
			using std::mutex;
		#else
			class EASTL_API mutex
			{
			public:
				mutex();
			   ~mutex();

				void lock();
				void unlock();

			protected:
				#if defined(EA_PLATFORM_MICROSOFT)
					#if defined(_WIN64)
						uint64_t mMutexBuffer[40 / sizeof(uint64_t)]; // CRITICAL_SECTION is 40 bytes on Win64.
					#elif defined(_WIN32)
						uint32_t mMutexBuffer[24 / sizeof(uint32_t)]; // CRITICAL_SECTION is 24 bytes on Win32.
					#endif
				#elif defined(EA_PLATFORM_POSIX)
					pthread_mutex_t mMutex;
				#endif
			};
		#endif


		// auto_mutex
		class EASTL_API auto_mutex
		{
		public:
			EA_FORCE_INLINE auto_mutex(mutex& mutex) : pMutex(&mutex)
				{ pMutex->lock(); }

			EA_FORCE_INLINE ~auto_mutex()
				{ pMutex->unlock(); }

		protected:
			mutex* pMutex;

			#if defined(EA_COMPILER_NO_DELETED_FUNCTIONS)
				auto_mutex(const auto_mutex&) : pMutex(NULL) {}
				void operator=(const auto_mutex&) {}
			#else
				auto_mutex(const auto_mutex&) = delete;
				void operator=(const auto_mutex&) = delete;
			#endif
		};


		// shared_ptr_auto_mutex
		class EASTL_API shared_ptr_auto_mutex : public auto_mutex
		{
		public:
			shared_ptr_auto_mutex(const void* pSharedPtr);

			#if !defined(EA_COMPILER_NO_DELETED_FUNCTIONS)
				shared_ptr_auto_mutex(const shared_ptr_auto_mutex&) = delete;
				void operator=(shared_ptr_auto_mutex&&) = delete;
			#endif
		};


	} // namespace Internal

} // namespace eastl


#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


#endif // Header include guard








