/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_CORE_ALLOCATOR_H
#define EASTL_CORE_ALLOCATOR_H

#if EASTL_CORE_ALLOCATOR_ENABLED

#include <coreallocator/icoreallocator.h>

namespace EA
{
	namespace Allocator
	{
		/// EASTLCoreAllocatorImpl
		///
		/// EASTL provides an out of the box implementation of the
		/// ICoreAllocator interface.  This is provided as a convenience for
		/// users who wish to provide ICoreAllocator implementations for EASTL to use.  
		///
		/// EASTL has a dependency on coreallocator so to provide an out of 
		/// the box implementation for EASTLCoreAlloctor and EASTLCoreDeleter 
		/// that can be used and tested.  Historically we could not test 
		/// ICoreAllocator interface because we relied on the code being linked 
		/// in user code.
		///

		class EASTLCoreAllocatorImpl : public ICoreAllocator
		{
		public:
			virtual void* Alloc(size_t size, const char* name, unsigned int flags)
			{
				return ::operator new[](size, name, flags, 0, __FILE__, __LINE__);
			}

			virtual void* Alloc(size_t size, const char* name, unsigned int flags, unsigned int alignment, unsigned int alignOffset = 0)
			{
				return ::operator new[](size, alignment, alignOffset, name, flags, 0, __FILE__, __LINE__);
			}

			virtual void Free(void* ptr, size_t size = 0)
			{
				::operator delete(static_cast<char*>(ptr));
			}

			virtual void* AllocDebug(size_t size, const DebugParams debugParams, unsigned int flags)
			{
				return Alloc(size, debugParams.mName, flags);
			}

			virtual void* AllocDebug(size_t size, const DebugParams debugParams, unsigned int flags, unsigned int align, unsigned int alignOffset = 0)
			{
				return Alloc(size, debugParams.mName, flags, align, alignOffset);
			}

			static EASTLCoreAllocatorImpl* GetDefaultAllocator();
		};

		inline EASTLCoreAllocatorImpl* EASTLCoreAllocatorImpl::GetDefaultAllocator()
		{
			static EASTLCoreAllocatorImpl allocator;
			return &allocator;
		}
	}
}

#endif // EASTL_CORE_ALLOCATOR_ENABLED
#endif // EASTL_CORE_ALLOCATOR_H

