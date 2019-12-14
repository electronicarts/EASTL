/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTLTEST_ALLOCATOR_H
#define EASTLTEST_ALLOCATOR_H

#include <EABase/eabase.h>
#include <EASTL/internal/config.h>
#include <new>
#include <stdio.h>

#if !EASTL_OPENSOURCE

	#include <PPMalloc/EAGeneralAllocator.h>
	#include <PPMalloc/EAGeneralAllocatorDebug.h>

	#include <coreallocator/icoreallocator_interface.h>

	#if defined(EA_COMPILER_MSVC)
		#include <math.h>       // VS2008 has an acknowledged bug that requires math.h (and possibly also string.h) to be #included before intrin.h.
		#include <intrin.h>
		#pragma intrinsic(_ReturnAddress)
	#endif

	///////////////////////////////////////////////////////////////////////////////
	// EASTLTest_GetGeneralAllocator()
	//
	namespace EA
	{
		namespace Allocator
		{
			#ifdef EA_DEBUG
				extern PPM_API GeneralAllocatorDebug* gpEAGeneralAllocatorDebug;
			#else
				extern PPM_API GeneralAllocator*      gpEAGeneralAllocator;
			#endif

			static inline auto& EASTLTest_GetGeneralAllocator()
			{
			#ifdef EA_DEBUG
				using GeneralAllocatorType = GeneralAllocatorDebug;
			#else
				using GeneralAllocatorType = GeneralAllocator;
			#endif

				static GeneralAllocatorType sGeneralAllocator;
				return sGeneralAllocator;
			}
		}
	}


	///////////////////////////////////////////////////////////////////////////////
	// allocator counts for debugging purposes
	//
	int gEASTLTest_AllocationCount = 0;
	int gEASTLTest_TotalAllocationCount = 0;


	///////////////////////////////////////////////////////////////////////////////
	// EASTLTest_ValidateHeap
	//
	bool EASTLTest_ValidateHeap()
	{
	#ifdef EA_DEBUG
		return EA::Allocator::EASTLTest_GetGeneralAllocator().ValidateHeap(EA::Allocator::GeneralAllocator::kHeapValidationLevelBasic);
	#else
		return true;
	#endif
	}


	///////////////////////////////////////////////////////////////////////////////
	// Microsoft function parameter annotations
	// https://msdn.microsoft.com/en-CA/library/hh916382.aspx
	//
	#ifndef _Ret_maybenull_
	#define _Ret_maybenull_
	#endif

	#ifndef _Post_writable_byte_size_
	#define _Post_writable_byte_size_(x)
	#endif

	#ifndef _Ret_notnull_
	#define _Ret_notnull_
	#endif


	///////////////////////////////////////////////////////////////////////////////
	// operator new extensions
	//
	namespace
	{
		#ifdef EA_DEBUG
			const char gUnattributedNewTag[] = "Anonymous new";
		#endif

		#if defined(EA_COMPILER_MSVC)
			#define UNATTRIBUTED_NEW_FILE   "raw_return_address"
			#define UNATTRIBUTED_NEW_LINE   ((int)(uintptr_t)_ReturnAddress())
		#else
			#define UNATTRIBUTED_NEW_FILE   NULL
			#define UNATTRIBUTED_NEW_LINE   0
		#endif
	}

	///////////////////////////////////////////////////////////////////////////////
	// system memory allocation helpers
	//
	namespace
	{
		void* PlatformMalloc(size_t size, size_t alignment = 16)
		{
		#ifdef EA_PLATFORM_MICROSOFT
			return _aligned_malloc(size, alignment);
		#else
			void *p = nullptr;
			alignment = alignment < sizeof( void *) ? sizeof( void *) : alignment;
			posix_memalign(&p, alignment, size);
			return p;
		#endif
		}

		void PlatformFree(void* p)
		{
		#ifdef EA_PLATFORM_MICROSOFT
			_aligned_free(p);
		#else
			free(p);
		#endif
		}

		void* InternalMalloc(size_t size)
		{
		    void* mem = nullptr;

			auto& allocator = EA::Allocator::EASTLTest_GetGeneralAllocator();

		#ifdef EA_DEBUG
			mem = allocator.MallocDebug(size, 0, 0, gUnattributedNewTag, UNATTRIBUTED_NEW_FILE, UNATTRIBUTED_NEW_LINE);
		#else
			mem = allocator.Malloc(size);
		#endif

			if(mem == nullptr)
				mem = PlatformMalloc(size);

			return mem;
		}

		void* InternalMalloc(size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
		{
		    void* mem = nullptr;

			auto& allocator = EA::Allocator::EASTLTest_GetGeneralAllocator();

		#ifdef EA_DEBUG
			mem = allocator.MallocDebug(size, flags, debugFlags, name, file, line);
		#else
			mem = allocator.Malloc(size, flags);
			EA_UNUSED(debugFlags);
			EA_UNUSED(file);
			EA_UNUSED(line);
			EA_UNUSED(name);
		#endif

			if(mem == nullptr)
				mem = PlatformMalloc(size);

			return mem;
		}

		void* InternalMalloc(size_t size, size_t alignment, const char* name, int flags, unsigned debugFlags, const char* file, int line)
		{
		    void* mem = nullptr;

			auto& allocator = EA::Allocator::EASTLTest_GetGeneralAllocator();

		#ifdef EA_DEBUG
			mem = allocator.MallocAlignedDebug(size, alignment, 0, flags, debugFlags, name, file, line);
		#else
			mem = allocator.MallocAligned(size, alignment, flags);
			EA_UNUSED(debugFlags);
			EA_UNUSED(file);
			EA_UNUSED(line);
			EA_UNUSED(name);
		#endif

			if(mem == nullptr)
				mem = PlatformMalloc(size, alignment);

			return mem;
		}

		void* InternalMalloc(size_t size, size_t alignment)
		{
			void* mem = nullptr;

			auto& allocator = EA::Allocator::EASTLTest_GetGeneralAllocator();

		#ifdef EA_DEBUG
			mem = allocator.MallocAlignedDebug(size, alignment, 0, 0, 0, gUnattributedNewTag, UNATTRIBUTED_NEW_FILE, UNATTRIBUTED_NEW_LINE);
		#else
			mem = allocator.MallocAligned(size, alignment);
		#endif

			if(mem == nullptr)
				mem = PlatformMalloc(size, alignment);

			return mem;
		}

		void InternalFree(void* p)
		{
			auto& allocator = EA::Allocator::EASTLTest_GetGeneralAllocator();

			if(allocator.ValidateAddress(p, EA::Allocator::GeneralAllocator::kAddressTypeOwned) == p)
			{
				allocator.Free(p);
			}
			else
			{
				PlatformFree(p);
			}
		}
	}

	class EASTLTestICA : public EA::Allocator::ICoreAllocator
	{
	public:
		EASTLTestICA()
		{
		}

		virtual ~EASTLTestICA()
		{
		}

		virtual void* Alloc(size_t size, const char* name, unsigned int flags)
		{
			return ::InternalMalloc(size, name, (int)flags, 0, NULL, 0);
		}

		virtual void* Alloc(size_t size, const char* name, unsigned int flags,
							unsigned int align, unsigned int)
		{
			return ::InternalMalloc(size, (size_t)align, name, (int)flags, 0, NULL, 0);
		}

		virtual void Free(void* pData, size_t /*size*/)
		{
			return ::InternalFree(pData);
		}
	};

	EA::Allocator::ICoreAllocator* EA::Allocator::ICoreAllocator::GetDefaultAllocator()
	{
		static EASTLTestICA sEASTLTestICA;

		return &sEASTLTestICA;
	}

	///////////////////////////////////////////////////////////////////////////
	// operator new/delete implementations
	//
	_Ret_maybenull_ _Post_writable_byte_size_(size) void* operator new(size_t size, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE()
	{
		return InternalMalloc(size);
	}


	void operator delete(void* p, const std::nothrow_t&) EA_THROW_SPEC_DELETE_NONE()
	{
		if(p) // The standard specifies that 'delete NULL' is a valid operation.
		{
			gEASTLTest_AllocationCount--;
			InternalFree(p);
		}
	}


	_Ret_maybenull_ _Post_writable_byte_size_(size) void* operator new[](size_t size, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE()
	{
		gEASTLTest_AllocationCount++;
		gEASTLTest_TotalAllocationCount++;

		void* p = InternalMalloc(size);
		return p;
	}


	void operator delete[](void* p, const std::nothrow_t&) EA_THROW_SPEC_DELETE_NONE()
	{
		if(p)
		{
			gEASTLTest_AllocationCount--;
			InternalFree(p);
		}
	}


	_Ret_notnull_ _Post_writable_byte_size_(size) void* operator new(size_t size) 
	{
		gEASTLTest_AllocationCount++;
		gEASTLTest_TotalAllocationCount++;

		void* mem = InternalMalloc(size);

	#if !defined(EA_COMPILER_NO_EXCEPTIONS)
		if (mem == NULL)
		{
			throw std::bad_alloc();
		}
	#endif

		return mem;
	}


	_Ret_notnull_ _Post_writable_byte_size_(size) void* operator new[](size_t size) 
	{
		gEASTLTest_AllocationCount++;
		gEASTLTest_TotalAllocationCount++;

		void* mem = InternalMalloc(size);

	#if !defined(EA_COMPILER_NO_EXCEPTIONS)
		if (mem == NULL)
		{
			throw std::bad_alloc();
		}
	#endif

		return mem;
	}


	void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
	{
		gEASTLTest_AllocationCount++;
		gEASTLTest_TotalAllocationCount++;

		return InternalMalloc(size, name, flags, debugFlags, file, line);
	}


	void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line)
	{
		gEASTLTest_AllocationCount++;
		gEASTLTest_TotalAllocationCount++;

		return InternalMalloc(size, alignment, name, flags, debugFlags, file, line);
	}

	// Used by GCC when you make new objects of classes with >= N bit alignment (with N depending on the compiler).
	void* operator new(size_t size, size_t alignment)
	{
		gEASTLTest_AllocationCount++;
		gEASTLTest_TotalAllocationCount++;

		return InternalMalloc(size, alignment);
	}

	// Used by GCC when you make new objects of classes with >= N bit alignment (with N depending on the compiler).
	void* operator new(size_t size, size_t alignment, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE()
	{
		gEASTLTest_AllocationCount++;
		gEASTLTest_TotalAllocationCount++;

		return InternalMalloc(size, alignment);
	}

	// Used by GCC when you make new objects of classes with >= N bit alignment (with N depending on the compiler).
	void* operator new[](size_t size, size_t alignment)
	{
		gEASTLTest_AllocationCount++;
		gEASTLTest_TotalAllocationCount++;

		return InternalMalloc(size, alignment);
	}

	// Used by GCC when you make new objects of classes with >= N bit alignment (with N depending on the compiler).
	void* operator new[](size_t size, size_t alignment, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE()
	{
		gEASTLTest_AllocationCount++;
		gEASTLTest_TotalAllocationCount++;

		return InternalMalloc(size, alignment);
	}

	void operator delete(void* p) EA_THROW_SPEC_DELETE_NONE()
	{
		if(p) // The standard specifies that 'delete NULL' is a valid operation.
		{
			gEASTLTest_AllocationCount--;
			InternalFree(p);
		}
	}


	void operator delete[](void* p) EA_THROW_SPEC_DELETE_NONE()
	{
		if(p)
		{
			gEASTLTest_AllocationCount--;
			InternalFree(p);
		}
	}

	void EASTLTest_SetGeneralAllocator()
	{
		EA::Allocator::SetGeneralAllocator(&EA::Allocator::EASTLTest_GetGeneralAllocator());
		#ifdef EA_DEBUG
			EA::Allocator::gpEAGeneralAllocatorDebug->SetDefaultDebugDataFlag(EA::Allocator::GeneralAllocatorDebug::kDebugDataIdGuard);
		#endif
	}

#else
	#if !defined(EA_PLATFORM_MICROSOFT) || defined(EA_PLATFORM_MINGW)
		#include <stdlib.h>
	#endif

	namespace Internal
	{
		void* EASTLAlignedAlloc(size_t size, size_t alignment)
		{
		#ifdef EA_PLATFORM_MICROSOFT
			return _aligned_malloc(size, alignment);
		#else
			void *p = nullptr;
			alignment = alignment < sizeof( void *) ? sizeof( void *) : alignment;
			posix_memalign(&p, alignment, size);
			return p;
		#endif
		}

		void EASTLAlignedFree(void* p)
		{
		#ifdef EA_PLATFORM_MICROSOFT
			_aligned_free(p);
		#else
			free(p);
		#endif
		}
	}

	void* operator new(size_t size)
		{ return Internal::EASTLAlignedAlloc(size, 16); }

	void* operator new[](size_t size)
		{ return Internal::EASTLAlignedAlloc(size, 16); }

	void* operator new[](size_t size, const char* /*name*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
		{ return Internal::EASTLAlignedAlloc(size, 16); }

	void* operator new[](size_t size, size_t alignment, size_t /*alignmentOffset*/, const char* /*name*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
		{ return Internal::EASTLAlignedAlloc(size, alignment); }

	void* operator new(size_t size, size_t alignment)
		{ return Internal::EASTLAlignedAlloc(size, alignment); }

	void* operator new(size_t size, size_t alignment, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE()
		{ return Internal::EASTLAlignedAlloc(size, alignment); }

	void* operator new[](size_t size, size_t alignment)
		{ return Internal::EASTLAlignedAlloc(size, alignment); }

	void* operator new[](size_t size, size_t alignment, const std::nothrow_t&)EA_THROW_SPEC_NEW_NONE()
		{ return Internal::EASTLAlignedAlloc(size, alignment); }

	// C++14 deleter
	void operator delete(void* p, std::size_t sz ) EA_THROW_SPEC_DELETE_NONE()
		{ Internal::EASTLAlignedFree(p); EA_UNUSED(sz); }

	void operator delete[](void* p, std::size_t sz ) EA_THROW_SPEC_DELETE_NONE()
		{ Internal::EASTLAlignedFree(p); EA_UNUSED(sz); }

	void operator delete(void* p) EA_THROW_SPEC_DELETE_NONE()
		{ Internal::EASTLAlignedFree(p); }

	void operator delete[](void* p) EA_THROW_SPEC_DELETE_NONE()
		{ Internal::EASTLAlignedFree(p); }

	void EASTLTest_SetGeneralAllocator() { /* intentionally blank */ }
	bool EASTLTest_ValidateHeap() { return true; }

#endif // !EASTL_OPENSOURCE

#endif // Header include guard
