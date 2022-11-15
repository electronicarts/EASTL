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

/////////////////////////////////////////////////////////////////////////////////////////////////////
// NOTE(rparolin): We need a fallback mutex implementation because the Microsoft implementation 
// of std::mutex can not be included in managed-cpp code.
//
// fatal error C1189: <mutex> is not supported when compiling with /clr or /clr:pure 
/////////////////////////////////////////////////////////////////////////////////////////////////////
#if !defined(EASTL_CPP11_MUTEX_ENABLED)
	#if defined(EA_HAVE_CPP11_MUTEX) && !defined(EA_COMPILER_MANAGED_CPP)
		#define EASTL_CPP11_MUTEX_ENABLED 1
	#else
		#define EASTL_CPP11_MUTEX_ENABLED 0
	#endif
#endif

#if EASTL_CPP11_MUTEX_ENABLED
	EA_DISABLE_ALL_VC_WARNINGS()
	#include <mutex>
	EA_RESTORE_ALL_VC_WARNINGS()
#endif

#if defined(EA_PLATFORM_MICROSOFT)
	// Cannot include Windows headers in our headers, as they kill builds with their #defines.
#elif defined(EA_PLATFORM_POSIX)
	#include <pthread.h>
#endif

// copy constructor could not be generated because a base class copy constructor is inaccessible or deleted.
// assignment operator could not be generated because a base class assignment operator is inaccessible or deleted.
// non dll-interface class used as base for DLL-interface classkey 'identifier'.
EA_DISABLE_VC_WARNING(4625 4626 4275);


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
	#if defined(__clang__) || (defined(EA_COMPILER_GNUC) && (EA_COMPILER_VERSION >= 4003))
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

			auto_mutex(const auto_mutex&) = delete;
			void operator=(const auto_mutex&) = delete;
		};


		// shared_ptr_auto_mutex
		class EASTL_API shared_ptr_auto_mutex : public auto_mutex
		{
		public:
			shared_ptr_auto_mutex(const void* pSharedPtr);

			shared_ptr_auto_mutex(const shared_ptr_auto_mutex&) = delete;
			void operator=(shared_ptr_auto_mutex&&) = delete;
		};


	} // namespace Internal

} // namespace eastl


EA_RESTORE_VC_WARNING();


#endif // Header include guard
