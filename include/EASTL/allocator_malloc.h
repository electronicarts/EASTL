/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ALLOCATOR_MALLOC_H
#define EASTL_ALLOCATOR_MALLOC_H


#include <EABase/eahave.h>
#include <EASTL/allocator.h>
#include <stddef.h>


// EASTL_ALIGNED_MALLOC_AVAILABLE
//
// Identifies if the standard library provides a built-in aligned version of malloc.
// Defined as 0 or 1, depending on the standard library or platform availability.
// None of the viable C functions provides for an aligned malloc with offset, so we 
// don't consider that supported in any case.
//
// Options for aligned allocations:
// C11   aligned_alloc   http://linux.die.net/man/3/aligned_alloc
// glibc memalign        http://linux.die.net/man/3/posix_memalign
// Posix posix_memalign  http://pubs.opengroup.org/onlinepubs/000095399/functions/posix_memalign.html
// VC++ _aligned_malloc  http://msdn.microsoft.com/en-us/library/8z34s9c6%28VS.80%29.aspx This is not suitable, since it has a limitation that you need to free via _aligned_free.
//
#if !defined EASTL_ALIGNED_MALLOC_AVAILABLE
	#if defined(EA_PLATFORM_POSIX) && !defined(EA_PLATFORM_APPLE)
		// memalign is more consistently available than posix_memalign, though its location isn't consistent across 
		// platforms and compiler libraries. Typically it's declared in one of three headers: stdlib.h, malloc.h, or malloc/malloc.h
		#include <stdlib.h> // memalign, posix_memalign. 
		#define EASTL_ALIGNED_MALLOC_AVAILABLE 1

		#if EA_HAS_INCLUDE_AVAILABLE
			#if EA_HAS_INCLUDE(<malloc/malloc.h>)
				#include <malloc/malloc.h>
			#elif EA_HAS_INCLUDE(<malloc.h>)
				#include <malloc.h>
			#endif
		#elif defined(EA_PLATFORM_BSD)
			#include <malloc/malloc.h>
		#elif defined(__clang__)
			#if __has_include(<malloc/malloc.h>)
				#include <malloc/malloc.h>
			#elif __has_include(<malloc.h>)
				#include <malloc.h>
			#endif
		#else
			#include <malloc.h>
		#endif
	#else
		#define EASTL_ALIGNED_MALLOC_AVAILABLE 0
	#endif
#endif


namespace eastl
{

	///////////////////////////////////////////////////////////////////////////////
	// allocator_malloc
	//
	// Implements an EASTL allocator that uses malloc/free as opposed to 
	// new/delete or PPMalloc Malloc/Free. 
	//
	// Example usage:
	//      vector<int, allocator_malloc> intVector;
	// 
	class allocator_malloc
	{
	public:
		allocator_malloc(const char* = NULL)
			{ }

		allocator_malloc(const allocator_malloc&)
			{ }

		allocator_malloc(const allocator_malloc&, const char*)
			{ }

		allocator_malloc& operator=(const allocator_malloc&)
			{ return *this; }

		bool operator==(const allocator_malloc&)
			{ return true; }

		bool operator!=(const allocator_malloc&)
			{ return false; }

		void* allocate(size_t n, int /*flags*/ = 0)
			{ return malloc(n); }

		void* allocate(size_t n, size_t alignment, size_t alignmentOffset, int /*flags*/ = 0)
		{ 
			#if EASTL_ALIGNED_MALLOC_AVAILABLE
				if((alignmentOffset % alignment) == 0) // We check for (offset % alignmnent == 0) instead of (offset == 0) because any block which is aligned on e.g. 64 also is aligned at an offset of 64 by definition. 
					return memalign(alignment, n); // memalign is more consistently available than posix_memalign.
			#else
				if((alignment <= EASTL_SYSTEM_ALLOCATOR_MIN_ALIGNMENT) && ((alignmentOffset % alignment) == 0))
					return malloc(n);
			#endif
			return NULL;
		}

		void deallocate(void* p, size_t /*n*/)
			{ free(p); }

		const char* get_name() const
			{ return "allocator_malloc"; }

		void set_name(const char*)
			{ }
	};


} // namespace eastl



#endif // Header include guard









