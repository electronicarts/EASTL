/**
 *
 * CitizenFX custom allocator with proper alignment and deallocation.
 * Inspired by EAWebKitAllocator
 *
 */

#include <EASTL/allocator.h>
#include <EASTL/internal/config.h>

#include <new>

#ifdef EASTL_USER_DEFINED_ALLOCATOR
namespace eastl
{
	allocator::allocator(const char* EASTL_NAME(pName)) {}
	allocator::allocator(const allocator& EASTL_NAME(alloc)) {}
	allocator::allocator(const allocator&, const char* EASTL_NAME(pName)) {}
	allocator& allocator::operator=(const allocator& EASTL_NAME(alloc)) { return *this; }
	const char* allocator::get_name() const { return ""; }
	void allocator::set_name(const char* EASTL_NAME(pName)) {}

	void* allocator::allocate(size_t n, int flags)
	{
#if defined(_MSC_VER)
		return _aligned_malloc(n, sizeof(void*) * 2);
#elif defined(__GNUC__)
		return malloc(n);
#endif
	}


	void* allocator::allocate(size_t n, size_t alignment, size_t offset, int flags)
	{
#if defined(_MSC_VER)
		return _aligned_malloc(n, alignment);
#elif defined(__GNUC__)
		return memalign(alignment, n);
#endif
	}


	void allocator::deallocate(void* p, size_t)
	{
#if defined(_MSC_VER)
		_aligned_free(p);
#elif defined(__GNUC__)
		free(p);
#endif
	}

	EASTL_API allocator gDefaultAllocator;
	EASTL_API allocator* gpDefaultAllocator = &gDefaultAllocator;

	EASTL_API allocator* GetDefaultAllocator() { return gpDefaultAllocator; }

	EASTL_API allocator* SetDefaultAllocator(allocator* pAllocator)
	{
		allocator* const pPrevAllocator = gpDefaultAllocator;
		gpDefaultAllocator = pAllocator;
		return pPrevAllocator;
	}
} // namespace eastl

#endif
