///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_SEGMENTED_VECTOR_H
#define EASTL_SEGMENTED_VECTOR_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once 
#endif

#include <EASTL/internal/config.h>

namespace eastl
{
	template<typename T, size_t Count, typename Allocator = EASTLAllocatorType>
	class segment
	{
	public:
		typedef eastl_size_t					size_type;
		typedef segment<T, Count, Allocator>	this_type;
		typedef T*								iterator;
		typedef const T*						const_iterator;

		const this_type*		next_segment() const;
		this_type*				next_segment();

		const_iterator			begin() const;
		iterator				begin();

		const_iterator			end() const;
		iterator				end();

	private:
		static const uintptr_t	kIsLastSegment = 1 << 0;
		uintptr_t				mPrev;

		union
		{
			this_type*			mNext;
			size_type			mSize;
		};
		T						mData[Count];
		template<typename, size_t, typename> friend class segmented_vector;
		template<typename, size_t, typename> friend struct segmented_vector_iterator;
	};


	template <typename T, size_t Count, typename Allocator = EASTLAllocatorType>
	struct segmented_vector_iterator
	{
	public:
        typedef segmented_vector_iterator<T, Count, Allocator>	this_type;
		typedef segment<T, Count, Allocator>					segment_type;

        T*						operator->() const;
        T&						operator*() const;

        this_type&				operator++();
        this_type				operator++(int);
		
	public:
		T*						mCurrent;
		T*						mEnd;
		segment_type*			mSegment;
	};


	template <typename T, size_t Count, typename Allocator = EASTLAllocatorType>
	class segmented_vector
	{
	public:
		typedef eastl_size_t												size_type;
		typedef segmented_vector<T, Count, Allocator>						this_type;
		typedef segment<T, Count, Allocator>								segment_type;
        typedef Allocator													allocator_type;
		typedef segmented_vector_iterator<const T, Count, Allocator>		const_iterator;
		typedef segmented_vector_iterator<T, Count, Allocator>				iterator;


								segmented_vector(const Allocator& allocator = Allocator());
								~segmented_vector();

		allocator_type&			get_allocator();

		const segment_type*		first_segment() const;
		segment_type*			first_segment();
		const_iterator			begin() const;
		iterator				begin();

		const_iterator			end() const;
		iterator				end();

		size_type				size() const;
		size_type				segment_count() const;
		T&						front();
		T&						back();

		bool					empty() const;
        void					clear();

		T&						push_back();
		T&						push_back(const T& value);
		void*					push_back_uninitialized();

		void					pop_back();

		void					erase_unsorted(segment_type& segment, typename segment_type::iterator it);
		iterator				erase_unsorted(const iterator& i);

		void					swap(this_type& other);

	protected:
		segment_type*			DoAllocSegment(segment_type* prevSegment);
		void*					DoPushBack();

		allocator_type			mAllocator;
		segment_type*			mFirstSegment;
		segment_type*			mLastSegment;
		size_type				mSegmentCount;
	};


	template<typename T, size_t Count, typename Allocator>
	inline const segment<T, Count, Allocator>*
	segment<T, Count, Allocator>::next_segment() const
	{
		if (mPrev & kIsLastSegment)
			return 0;
		else
			return mNext;
	}

	template<typename T, size_t Count, typename Allocator>
	inline segment<T, Count, Allocator>*
	segment<T, Count, Allocator>::next_segment()
	{
		if (mPrev & kIsLastSegment)
			return 0;
		else
			return mNext;
	}

	template<typename T, size_t Count, typename Allocator>
	inline typename segment<T, Count, Allocator>::const_iterator
	segment<T, Count, Allocator>::begin() const
	{
		return mData;
	}

	template<typename T, size_t Count, typename Allocator>
	inline typename segment<T, Count, Allocator>::iterator
	segment<T, Count, Allocator>::begin()
	{
		return mData;
	}

	template<typename T, size_t Count, typename Allocator>
	inline typename segment<T, Count, Allocator>::const_iterator
	segment<T, Count, Allocator>::end() const
	{
		if (mPrev & kIsLastSegment)
			return mData + mSize;
		else
			return mData + Count;
	}

	template<typename T, size_t Count, typename Allocator>
	inline typename segment<T, Count, Allocator>::iterator
	segment<T, Count, Allocator>::end()
	{
		if (mPrev & kIsLastSegment)
			return mData + mSize;
		else
			return mData + Count;
	}

	template<typename T, size_t Count, typename Allocator>
    T*
	segmented_vector_iterator<T, Count, Allocator>::operator->() const
	{
		return mCurrent;
	}

	template<typename T, size_t Count, typename Allocator>
    T&
	segmented_vector_iterator<T, Count, Allocator>::operator*() const
	{
		return *mCurrent;
	}

	template<typename T, size_t Count, typename Allocator>
    segmented_vector_iterator<T, Count, Allocator>&
	segmented_vector_iterator<T, Count, Allocator>::operator++()
	{
		++mCurrent;
        if(EASTL_UNLIKELY(mCurrent == mEnd))
        {
			if (!(mSegment->mPrev & segment_type::kIsLastSegment))
			{
				mSegment = mSegment->mNext;
				mCurrent = mSegment->begin();
				mEnd = mSegment->end();
			}
			else
				mCurrent = 0;
        }
        return *this;
	}

	template<typename T, size_t Count, typename Allocator>
    segmented_vector_iterator<T, Count, Allocator>
	segmented_vector_iterator<T, Count, Allocator>::operator++(int)
	{
		this_type i(*this);
		return ++i;
	}


	template <typename T, size_t Count, typename Allocator>
	inline segmented_vector<T, Count, Allocator>::segmented_vector(const Allocator& allocator)
	:	mAllocator(allocator)
	,	mFirstSegment(0)
	,	mLastSegment(0)
	,	mSegmentCount(0)
	{
	}

	template <typename T, size_t Count, typename Allocator>
	inline segmented_vector<T, Count, Allocator>::~segmented_vector()
	{
		clear();
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::allocator_type&
	segmented_vector<T, Count, Allocator>::get_allocator()
	{
		return mAllocator;
	}

	template <typename T, size_t Count, typename Allocator>
	inline const typename segmented_vector<T, Count, Allocator>::segment_type*
	segmented_vector<T, Count, Allocator>::first_segment() const
	{
		return mFirstSegment;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::segment_type*
	segmented_vector<T, Count, Allocator>::first_segment()
	{
		return mFirstSegment;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::const_iterator
	segmented_vector<T, Count, Allocator>::begin() const
	{
		iterator i;
		i.mSegment = mFirstSegment;
		if (mFirstSegment)
		{
			i.mCurrent = mFirstSegment->begin();
			i.mEnd = mFirstSegment->end();
		}
		else
			i.mCurrent = 0;
		return (const_iterator&)i;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::iterator
	segmented_vector<T, Count, Allocator>::begin()
	{
		iterator i;
		i.mSegment = mFirstSegment;
		if (mFirstSegment)
		{
			i.mCurrent = mFirstSegment->begin();
			i.mEnd = mFirstSegment->end();
		}
		else
			i.mCurrent = 0;
		return i;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::const_iterator
	segmented_vector<T, Count, Allocator>::end() const
	{
		iterator i;
		i.mCurrent = 0;
		return (const_iterator&)i;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::iterator
	segmented_vector<T, Count, Allocator>::end()
	{
		iterator i;
		i.mCurrent = 0;
		return i;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::size_type
	segmented_vector<T, Count, Allocator>::size() const
	{
		if (segment_type* segment = mLastSegment)
			return (mSegmentCount-1)*Count + segment->mSize;
		return 0;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::size_type
	segmented_vector<T, Count, Allocator>::segment_count() const
	{
		return mSegmentCount;
	}

	template <typename T, size_t Count, typename Allocator>
	inline T&
	segmented_vector<T, Count, Allocator>::front()
	{
		return mFirstSegment->mData[0];
	}

	template <typename T, size_t Count, typename Allocator>
	inline T&
	segmented_vector<T, Count, Allocator>::back()
	{
		segment_type* lastSegment = mLastSegment;
		return lastSegment->mData[lastSegment->mSize-1];
	}

	template <typename T, size_t Count, typename Allocator>
	inline bool
	segmented_vector<T, Count, Allocator>::empty() const
	{
		return mFirstSegment == 0;
	}

	template <typename T, size_t Count, typename Allocator>
	inline void
	segmented_vector<T, Count, Allocator>::clear()
	{
		if (segment_type* segment = mFirstSegment)
		{
			while (segment != mLastSegment)
			{
				segment_type* nextSegment = segment->mNext;
				segment->~segment_type();
				EASTLFree(mAllocator, segment, sizeof(segment_type));
				segment = nextSegment;
			}
			for (T* i = segment->mData, *e = segment->mData + segment->mSize; i!=e; ++i)
				i->~T();
			EASTLFree(mAllocator, segment, sizeof(segment_type));
			mFirstSegment = 0;
			mLastSegment = 0;
			mSegmentCount = 0;
		}
	}

	template <typename T, size_t Count, typename Allocator>
	inline T&
	segmented_vector<T, Count, Allocator>::push_back()
	{
		return *(new (DoPushBack()) T());
	}

	template <typename T, size_t Count, typename Allocator>
	inline T&
	segmented_vector<T, Count, Allocator>::push_back(const T& value)
	{
		return *(new (DoPushBack()) T(value));
	}

	template <typename T, size_t Count, typename Allocator>
	inline void*
	segmented_vector<T, Count, Allocator>::push_back_uninitialized()
	{
		return DoPushBack();
	}

	template <typename T, size_t Count, typename Allocator>
	inline void
	segmented_vector<T, Count, Allocator>::pop_back()
	{
		segment_type* lastSegment = mLastSegment;
        #if EASTL_ASSERT_ENABLED
            if(EASTL_UNLIKELY(!lastSegment))
                EASTL_FAIL_MSG("segmented_vector::pop_back -- segmented vector is empty");
        #endif
		--lastSegment->mSize;
		(lastSegment->mData + lastSegment->mSize)->T::~T();

		if (!lastSegment->mSize)
		{
			--mSegmentCount;
			mLastSegment = (segment_type*)(lastSegment->mPrev & (~segment_type::kIsLastSegment));
			EASTLFree(mAllocator, lastSegment, sizeof(segment_type));
			if (mLastSegment)
			{
				mLastSegment->mPrev |= segment_type::kIsLastSegment;
				mLastSegment->mSize = Count;
			}
			else
				mFirstSegment = 0;
		}
	}

	template <typename T, size_t Count, typename Allocator>
	inline void
	segmented_vector<T, Count, Allocator>::erase_unsorted(segment_type& segment, typename segment_type::iterator it)
	{
		*it = back();
		pop_back();
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::iterator
	segmented_vector<T, Count, Allocator>::erase_unsorted(const iterator& i)
	{
		iterator ret(i);
		*i = back();
		if (i.mSegment == mLastSegment && mLastSegment->mSize == 1)
			ret.mCurrent = 0;
		pop_back();
		return ret;
	}

	template <typename T, size_t Count, typename Allocator>
	void
	segmented_vector<T, Count, Allocator>::swap(this_type& other)
	{
		allocator_type tempAllocator(mAllocator);
		segment_type* tempFirstSegment = mFirstSegment;
		segment_type* tempLastSegment = mLastSegment;
		size_type tempSegmentCount = mSegmentCount;

		mAllocator = other.mAllocator;
		mFirstSegment = other.mFirstSegment;
		mLastSegment = other.mLastSegment;
		mSegmentCount = other.mSegmentCount;

		other.mAllocator = tempAllocator;
		other.mFirstSegment = tempFirstSegment;
		other.mLastSegment = tempLastSegment;
		other.mSegmentCount = tempSegmentCount;
	}

	template <typename T, size_t Count, typename Allocator>
	segment<T, Count, Allocator>*
	segmented_vector<T, Count, Allocator>::DoAllocSegment(segment_type* prevSegment)
	{
		++mSegmentCount;
		segment_type* segment = (segment_type*)allocate_memory(mAllocator, sizeof(segment_type), EASTL_ALIGN_OF(segment_type), 0);
		segment->mPrev = uintptr_t(prevSegment) | segment_type::kIsLastSegment;
		segment->mSize = 1;
		return segment;
	}

	template <typename T, size_t Count, typename Allocator>
	inline void*
	segmented_vector<T, Count, Allocator>::DoPushBack()
	{
		if (segment_type* segment = mLastSegment)
		{
			size_type size = segment->mSize;
			if (size < Count)
			{
				++segment->mSize;
				return segment->mData + size;
			}
			else
			{
				segment_type* lastSegment = mLastSegment;
				segment_type* newSegment = mLastSegment = DoAllocSegment(mLastSegment);
				lastSegment->mPrev &= ~segment_type::kIsLastSegment;
				lastSegment->mNext = newSegment;
				return newSegment->mData;
			}
		}
		else
		{
			segment = mFirstSegment = mLastSegment = DoAllocSegment(0);
			return segment->mData;
		}
	}

	template<typename T, size_t Count, typename Allocator>
    inline bool operator==(const segmented_vector_iterator<const T, Count, Allocator>& a, const segmented_vector_iterator<const T, Count, Allocator>& b)
    {
        return a.mCurrent == b.mCurrent;
    }


	template<typename T, size_t Count, typename Allocator>
    inline bool operator!=(const segmented_vector_iterator<const T, Count, Allocator>& a, const segmented_vector_iterator<const T, Count, Allocator>& b)
    {
        return a.mCurrent != b.mCurrent;
    }

	template<typename T, size_t Count, typename Allocator>
    inline bool operator==(const segmented_vector_iterator<T, Count, Allocator>& a, const segmented_vector_iterator<T, Count, Allocator>& b)
    {
        return a.mCurrent == b.mCurrent;
    }


	template<typename T, size_t Count, typename Allocator>
    inline bool operator!=(const segmented_vector_iterator<T, Count, Allocator>& a, const segmented_vector_iterator<T, Count, Allocator>& b)
    {
        return a.mCurrent != b.mCurrent;
    }
}

#endif
