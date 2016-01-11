///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EASTL/intrusive_list.h>


namespace eastl
{


	EASTL_API void intrusive_list_base::reverse() EA_NOEXCEPT
	{
		intrusive_list_node* pNode = &mAnchor;
		do {
			intrusive_list_node* const pTemp = pNode->mpNext;
			pNode->mpNext = pNode->mpPrev;
			pNode->mpPrev = pTemp;
			pNode         = pNode->mpPrev;
		} 
		while(pNode != &mAnchor);
	}



	EASTL_API bool intrusive_list_base::validate() const
	{
		const intrusive_list_node *p = &mAnchor;
		const intrusive_list_node *q = p;

		// We do two tests below:
		//
		// 1) Prev and next pointers are symmetric. We check (p->next->prev == p)
		//    for each node, which is enough to verify all links.
		//
		// 2) Loop check. We bump the q pointer at one-half rate compared to the
		//    p pointer; (p == q) if and only if we are at the start (which we
		//    don't check) or if there is a loop somewhere in the list.

		do {
			// validate node (even phase)
			if (p->mpNext->mpPrev != p)
				return false;               // broken linkage detected

			// bump only fast pointer
			p = p->mpNext;
			if (p == &mAnchor)
				break;

			if (p == q)
				return false;               // loop detected

			// validate node (odd phase)
			if (p->mpNext->mpPrev != p)
				return false;               // broken linkage detected

			// bump both pointers
			p = p->mpNext;
			q = q->mpNext;

			if (p == q)
				return false;               // loop detected

		} while(p != &mAnchor);

		return true;
	}


} // namespace eastl

















