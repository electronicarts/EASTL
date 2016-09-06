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
	
	#if defined(EA_COMPILER_MSVC)
		#include <math.h>       // VS2008 has an acknowledged bug that requires math.h (and possibly also string.h) to be #included before intrin.h.
		#include <intrin.h>
		#pragma intrinsic(_ReturnAddress)
	#endif

	///////////////////////////////////////////////////////////////////////////////
	// gGeneralAllocator
	//
	namespace EA
	{
		namespace Allocator
		{
			#ifdef EA_DEBUG
							   GeneralAllocatorDebug  gGeneralAllocator EA_INIT_PRIORITY(1000);
				extern PPM_API GeneralAllocatorDebug* gpEAGeneralAllocatorDebug;
			#else
							   GeneralAllocator       gGeneralAllocator EA_INIT_PRIORITY(1000);
				extern PPM_API GeneralAllocator*      gpEAGeneralAllocator;
			#endif
		}
	}


	///////////////////////////////////////////////////////////////////////////////
	// gEASTLTest_AllocationCount
	//
	int gEASTLTest_AllocationCount = 0;


	///////////////////////////////////////////////////////////////////////////////
	// EASTLTest_ValidateHeap
	//
	bool EASTLTest_ValidateHeap()
	{
	#ifdef EA_DEBUG
		return EA::Allocator::gpEAGeneralAllocatorDebug->ValidateHeap(EA::Allocator::GeneralAllocator::kHeapValidationLevelBasic);
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


	_Ret_maybenull_ _Post_writable_byte_size_(size) void* operator new(size_t size, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE()
	{
		#ifdef EA_DEBUG
			void* const p = EA::Allocator::gGeneralAllocator.MallocDebug(size, 0, 0, gUnattributedNewTag, UNATTRIBUTED_NEW_FILE, UNATTRIBUTED_NEW_LINE);
		#else
			void* const p = EA::Allocator::gGeneralAllocator.Malloc(size);
		#endif
		return p;
	}


	void operator delete(void* p, const std::nothrow_t&) EA_THROW_SPEC_DELETE_NONE()
	{
		if(p) // The standard specifies that 'delete NULL' is a valid operation.
		{
			gEASTLTest_AllocationCount--;
			EA::Allocator::gGeneralAllocator.Free(p);
		}
	}


	_Ret_maybenull_ _Post_writable_byte_size_(size) void* operator new[](size_t size, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE()
	{
		gEASTLTest_AllocationCount++;

		#ifdef EA_DEBUG
			void* const p = EA::Allocator::gGeneralAllocator.MallocDebug(size, 0, 0, gUnattributedNewTag, UNATTRIBUTED_NEW_FILE, UNATTRIBUTED_NEW_LINE);
		#else
			void* const p = EA::Allocator::gGeneralAllocator.Malloc(size);
		#endif
		return p;
	}


	void operator delete[](void* p, const std::nothrow_t&) EA_THROW_SPEC_DELETE_NONE()
	{
		if(p)
		{
			gEASTLTest_AllocationCount--;
			EA::Allocator::gGeneralAllocator.Free(p);
		}
	}


	_Ret_notnull_ _Post_writable_byte_size_(size) void* operator new(size_t size) EA_THROW_SPEC_NEW(std::bad_alloc)
	{
		gEASTLTest_AllocationCount++;

		// This IsConstructed functionality is needed by some mobile platforms due to some weaknesses in their application startup.
		#if (PPMALLOC_VERSION_N >= 11602)
			const bool bConstructed = EA::Allocator::gGeneralAllocator.IsConstructed();
		#else
			const bool bConstructed = (EA::Allocator::gGeneralAllocator.GetTraceFunction(NULL) != NULL);
		#endif

		void *mem;

		if(bConstructed)
		{
			#ifdef EA_DEBUG
				mem = EA::Allocator::gGeneralAllocator.MallocDebug(size, 0, 0, gUnattributedNewTag, UNATTRIBUTED_NEW_FILE, UNATTRIBUTED_NEW_LINE);
			#else
				mem = EA::Allocator::gGeneralAllocator.Malloc(size);
			#endif
		}
		else
		{
			mem = malloc(size);
		}

	#if !defined(EA_COMPILER_NO_EXCEPTIONS)
		if (mem == NULL)
		{
			throw std::bad_alloc();
		}
	#endif

		return mem;
	}


	_Ret_notnull_ _Post_writable_byte_size_(size) void* operator new[](size_t size) EA_THROW_SPEC_NEW(std::bad_alloc)
	{
		gEASTLTest_AllocationCount++;

		// This IsConstructed functionality is needed by some mobile platforms due to some weaknesses in their application startup.
		#if (PPMALLOC_VERSION_N >= 11602)
			const bool bConstructed = EA::Allocator::gGeneralAllocator.IsConstructed();
		#else
			const bool bConstructed = (EA::Allocator::gGeneralAllocator.GetTraceFunction(NULL) != NULL);
		#endif

		void *mem;

		if(bConstructed)
		{
			#ifdef EA_DEBUG
				mem = EA::Allocator::gGeneralAllocator.MallocDebug(size, 0, 0, gUnattributedNewTag, UNATTRIBUTED_NEW_FILE, UNATTRIBUTED_NEW_LINE);
			#else
				mem = EA::Allocator::gGeneralAllocator.Malloc(size);
			#endif
		}
		else
		{
			mem = malloc(size);
		}

	#if !defined(EA_COMPILER_NO_EXCEPTIONS)
		if (mem == NULL)
		{
			throw std::bad_alloc();
		}
	#endif

		return mem;
	}


	#ifdef EA_DEBUG
	void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
	#else
	void* operator new[](size_t size, const char* /*name*/, int flags, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
	#endif
	{
		gEASTLTest_AllocationCount++;

		#ifdef EA_DEBUG
			return EA::Allocator::gGeneralAllocator.MallocDebug(size, flags, debugFlags, name, file, line);
		#else
			return EA::Allocator::gGeneralAllocator.Malloc(size, flags);
		#endif
	}


	#ifdef EA_DEBUG
	void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line)
	#else
	void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* /*name*/, int flags, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
	#endif
	{
		gEASTLTest_AllocationCount++;

		#ifdef EA_DEBUG
			return EA::Allocator::gGeneralAllocator.MallocAlignedDebug(size, alignment, alignmentOffset, flags, debugFlags, name, file, line);
		#else
			return EA::Allocator::gGeneralAllocator.MallocAligned(size, alignment, alignmentOffset, flags);
		#endif
	}

	// Used by GCC when you make new objects of classes with >= N bit alignment (with N depending on the compiler).
	void* operator new(size_t size, size_t alignment)
	{
		gEASTLTest_AllocationCount++;
		return EA::Allocator::gGeneralAllocator.MallocAligned(size, alignment);
	}

	// Used by GCC when you make new objects of classes with >= N bit alignment (with N depending on the compiler).
	void* operator new(size_t size, size_t alignment, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE()
	{
		gEASTLTest_AllocationCount++;
		return EA::Allocator::gGeneralAllocator.MallocAligned(size, alignment);
	}

	// Used by GCC when you make new objects of classes with >= N bit alignment (with N depending on the compiler).
	void* operator new[](size_t size, size_t alignment)
	{
		gEASTLTest_AllocationCount++;
		return EA::Allocator::gGeneralAllocator.MallocAligned(size, alignment);
	}

	// Used by GCC when you make new objects of classes with >= N bit alignment (with N depending on the compiler).
	void* operator new[](size_t size, size_t alignment, const std::nothrow_t&) EA_THROW_SPEC_NEW_NONE()
	{
		gEASTLTest_AllocationCount++;
		return EA::Allocator::gGeneralAllocator.MallocAligned(size, alignment);
	}

	void operator delete(void* p) EA_THROW_SPEC_DELETE_NONE()
	{
		if(p) // The standard specifies that 'delete NULL' is a valid operation.
		{
			gEASTLTest_AllocationCount--;

			// This IsConstructed functionality is needed by some mobile platforms due to some weaknesses in their application startup.
			#if (PPMALLOC_VERSION_N >= 11602)
				const bool bConstructed = EA::Allocator::gGeneralAllocator.IsConstructed();
			#else
				const bool bConstructed = (EA::Allocator::gGeneralAllocator.GetTraceFunction(NULL) != NULL); // Old hacky way to test this.
			#endif

			if(bConstructed)
			{
				if(EA::Allocator::gGeneralAllocator.ValidateAddress(p, EA::Allocator::GeneralAllocatorDebug::kAddressTypeOwned))
					EA::Allocator::gGeneralAllocator.Free(p);
				else
					free(p);
			}
			else
				free(p);
		}
	}


	void operator delete[](void* p) EA_THROW_SPEC_DELETE_NONE()
	{
		if(p)
		{
			gEASTLTest_AllocationCount--;

			// This IsConstructed functionality is needed by some mobile platforms due to some weaknesses in their application startup.
			#if (PPMALLOC_VERSION_N >= 11602)
				const bool bConstructed = EA::Allocator::gGeneralAllocator.IsConstructed();
			#else
				const bool bConstructed = (EA::Allocator::gGeneralAllocator.GetTraceFunction(NULL) != NULL); // Old hacky way to test this.
			#endif

			if(bConstructed)
			{
				if(EA::Allocator::gGeneralAllocator.ValidateAddress(p, EA::Allocator::GeneralAllocatorDebug::kAddressTypeOwned))
					EA::Allocator::gGeneralAllocator.Free(p);
				else
					free(p);
			}
			else 
			   free(p);
		}
	}

	void EASTLTest_SetGeneralAllocator() 
	{
		EA::Allocator::SetGeneralAllocator(&EA::Allocator::gGeneralAllocator);
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

#endif // ....

#endif // Header include guard







