///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EASTL/internal/fixed_pool.h>
#include <EASTL/fixed_allocator.h>



namespace eastl
{


	EASTL_API void fixed_pool_base::init(void* pMemory, size_t memorySize, size_t nodeSize,
							   size_t alignment, size_t /*alignmentOffset*/)
	{
		// To do: Support alignmentOffset.

		#if EASTL_FIXED_SIZE_TRACKING_ENABLED
			mnCurrentSize = 0;
			mnPeakSize    = 0;
		#endif

		if(pMemory)
		{
			// Assert that alignment is a power of 2 value (e.g. 1, 2, 4, 8, 16, etc.)
			EASTL_ASSERT((alignment & (alignment - 1)) == 0);

			// Make sure alignment is a valid value.
			if(alignment < 1)
				alignment = 1;

			mpNext      = (Link*)(((uintptr_t)pMemory + (alignment - 1)) & ~(alignment - 1));
			memorySize -= (uintptr_t)mpNext - (uintptr_t)pMemory;
			pMemory     = mpNext;

			// The node size must be at least as big as a Link, which itself is sizeof(void*).
			if(nodeSize < sizeof(Link))
				nodeSize = ((sizeof(Link) + (alignment - 1))) & ~(alignment - 1);

			// If the user passed in a memory size that wasn't a multiple of the node size,
			// we need to chop down the memory size so that the last node is not a whole node.
			memorySize = (memorySize / nodeSize) * nodeSize;

			mpCapacity = (Link*)((uintptr_t)pMemory + memorySize);
			mpHead     = NULL;
			mnNodeSize = nodeSize;
		}
	}


} // namespace eastl

















