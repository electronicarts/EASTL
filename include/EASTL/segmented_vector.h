///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_SEGMENTED_VECTOR_H
#define EASTL_SEGMENTED_VECTOR_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once 
#endif

#include <EASTL/internal/config.h>

#include <EASTL/algorithm.h>
#include <EASTL/allocator.h>
#include <EASTL/memory.h>

namespace eastl
{
	// TODO: this really shouldn't be a public class, deprecate it and hide it.
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

		segment() = default;
		// TODO: should we implement copy/move ctor/assign?
		segment(const segment& other) = delete;
		segment& operator=(const segment&) = delete;
		// move ctor/assignment implicitly deleted.

		// The owning segment_vector is in charge of destroying the
		// data before the destructor of the segment runs, so we leave
		// the destructor as trivial. This class can't be copied and
		// it's public API doesn't really let you do anything, only
		// segmented_vector can really use these.
		//
		//~segment();

	private:
		// Destroys all the elements in the segment.
		void                    DestroyData();

		static const uintptr_t	kIsLastSegment = 1 << 0;
		uintptr_t				mPrev;

		union
		{
			// When the segment is not the last one in the segmented
			// vector, mNext is the link to the next segment.
			this_type*			mNext;
			// When the segment is the last one in the segmented
			// vector, mSize is the number elements in this current
			// segment. Note: if a segment is not the last one, it
			// must be completely full i.e. it must contain exactly
			// Count elements so there's no need to store anything in
			// mSize.
			size_type			mSize;
		};

		// Storage for the segment. Type-erased because we want to
		// support types which are not default constructible.
		// TODO: Should we be using std::byte here instead of
		// unsigned char?
		alignas(T) unsigned char mData[Count * sizeof(T)];

		template<typename, size_t, typename> friend class segmented_vector;
		template<typename, size_t, typename> friend struct segmented_vector_iterator;
	};


	template <typename T, size_t Count, typename Allocator = EASTLAllocatorType>
	struct segmented_vector_iterator
	{
	public:
        typedef segmented_vector_iterator<T, Count, Allocator>	this_type;
		typedef segment<T, Count, Allocator>					segment_type;

		// does not conform to any Iterator concept. could be a bidirectional iterator, but not random access iterator because segment is a double-linked list.

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
		typedef T															value_type;


		segmented_vector(const Allocator& allocator = Allocator());
		segmented_vector(const segmented_vector& other);
		segmented_vector(segmented_vector&& other);
		segmented_vector& operator=(const segmented_vector& other);
		segmented_vector& operator=(segmented_vector&& other);
		~segmented_vector();

		// constructors that initialize from a range, etc. missing

		segmented_vector(std::initializer_list<value_type> ilist, const Allocator& allocator = Allocator());

		allocator_type& get_allocator() noexcept;

		// TODO: deprecate these? what's the point of having them in
		// the API? We don't want people messing around with the
		// segments directly.
		const segment_type* first_segment() const noexcept;
		segment_type* first_segment() noexcept;

		const_iterator begin() const noexcept;
		iterator begin() noexcept;

		const_iterator end() const noexcept;
		iterator end() noexcept;

		// The number of elements in the container.
		size_type size() const noexcept;

		// The total capacity of the container.
		size_type capacity() const noexcept;

		// TODO: Deprecate this?
		// The number of "active" segments (does not include segments
		// in the free list).
		size_type segment_count() const noexcept;

		// These are UB if the container is empty.
		T& front() noexcept;
		T& back() noexcept;

		// Return true if the container has no elements and false
		// otherwise.
		bool empty() const noexcept;

		// Destroys the elements in the container, but does not
		// relinquish any memory (i.e. capacity() is unchanged)
		void clear();

		// Increase the capacity so it fits at least `n` elements.
		// This is less useful than in normal vectors since this will
		// allocate multiple segments, resulting in the same number of
		// allocations as if you'd pushed the elements one by one
		// without reserving.
		void reserve(size_type n);

		// Resizes the container to contain exactly `n` elements.
		//   - If `size() > n` it destroys the last `size() - n` elements.
		//   - If `size() < n` it inserts `n - size()` copies of `v`.
		void resize(size_type n, const value_type& v);

		// Equivalent to resize(n, value_type());
		void resize(size_type n);

		// Frees all the segments not currently in use, i.e makes it
		// so capacity() is the lowest multiple of Count greater than
		// or equal to size().
		void shrink_to_fit() noexcept;

		// missing (could be implemented):
		//		assign()
		//		set_allocator()
		//		set_capacity()
		//		validate()
		//		validate_iterator()

		// segmented_vector is almost a deque, but cannot provide:
		//		operator[]
		//		at()
		//		insert()
		//		push_front()
		//		emplace()
		//		emplace_front()
		//		erase()
		// because it is not a random access container and can only push/pop elements from the back.

		T& push_back();
		T& push_back(const T& value);
		T& push_back(T&& value);
		void* push_back_uninitialized();

		template <class... Args>
		T& emplace_back(Args&&... args);

		void pop_back();

		void erase_unsorted(segment_type& segment, typename segment_type::iterator it);
		iterator erase_unsorted(const iterator& i);

		void swap(this_type& other);

	protected:
		// This does not initialize the entry, it just makes returns a
		// pointer to an address where one could initialize one.
		void* DoPushBack();

		// Moves the segment into the free list
		void AddToFreeList(segment_type* segment);

		// Get a segment from the free list, or allocate a segment if
		// there are none.  Sets up the segment's "last segment" flag.
		segment_type* GetUnusedSegmentForLastSegment(segment_type* prevSegment);

		// Allocate a new segment.
		segment_type* AllocateNewSegment();

		// Destroys the elements in the container. Optionally also
		// frees all the memory.
		template <bool bFreeMemory>
		void Clear();

		// If the last segment is empty, this function will move it to
		// the free list and update mLastSegment with the previous
		// segment.
		void UpdateLastSegment();

		// Remove the last `n` elements in the container.
		void EraseFromBack(size_type n);

		// Pushes `n` copies of `v`
		void PushBack(size_type n, const value_type& v);

		template<bool bDoMove, typename ForwardIt>
		void InsertRange(ForwardIt begin, ForwardIt end);

		allocator_type mAllocator;
		segment_type* mFirstSegment{};
		segment_type* mLastSegment{};
		segment_type* mFreeList{};
		size_type mInUseSegmentCount{};

		// TODO: consider storing this in the first free segment (if
		// there is one), it'd save the memory for this member but
		// would make things like `capacity()` less cache coherent and
		// branchy (we need to check if there's a free segment)
		size_type mFreeListSegmentCount{};
	};


	/////////////////////////////////////
	//   segment
	/////////////////////////////////////

	template<typename T, size_t Count, typename Allocator>
	inline const segment<T, Count, Allocator>*
	segment<T, Count, Allocator>::next_segment() const
	{
		if (mPrev & kIsLastSegment)
			return nullptr;
		else
			return mNext;
	}

	template<typename T, size_t Count, typename Allocator>
	inline segment<T, Count, Allocator>*
	segment<T, Count, Allocator>::next_segment()
	{
		if (mPrev & kIsLastSegment)
			return nullptr;
		else
			return mNext;
	}

	template<typename T, size_t Count, typename Allocator>
	inline typename segment<T, Count, Allocator>::const_iterator
	segment<T, Count, Allocator>::begin() const
	{
		return reinterpret_cast<const T*>(&mData);
	}

	template<typename T, size_t Count, typename Allocator>
	inline typename segment<T, Count, Allocator>::iterator
	segment<T, Count, Allocator>::begin()
	{
		return reinterpret_cast<T*>(&mData);
	}

	template<typename T, size_t Count, typename Allocator>
	inline typename segment<T, Count, Allocator>::const_iterator
	segment<T, Count, Allocator>::end() const
	{
		if (mPrev & kIsLastSegment)
			return begin() + mSize;
		else
			return begin() + Count;
	}

	template<typename T, size_t Count, typename Allocator>
	inline typename segment<T, Count, Allocator>::iterator
	segment<T, Count, Allocator>::end()
	{
		if (mPrev & kIsLastSegment)
			return begin() + mSize;
		else
			return begin() + Count;
	}

	template <typename T, size_t Count, typename Allocator>
	inline void segment<T, Count, Allocator>::DestroyData()
	{
		// TODO: Our current call sites know the value of
		// (mPrev & kIsLastSegment), consider having 2 implementations
		// of this which don't branch or something like that.
		T* ptr = begin();
		const size_type count = (mPrev & kIsLastSegment) ? mSize : Count;
		for (size_type i = 0; i < count; ++i)
		{
			eastl::destroy_at(ptr);
			ptr++;
		}
		mSize = 0;
	}


	/////////////////////////////////////
	//   segmented_vector_iterator
	/////////////////////////////////////

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
				mCurrent = nullptr;
        }
        return *this;
	}

	template<typename T, size_t Count, typename Allocator>
    segmented_vector_iterator<T, Count, Allocator>
	segmented_vector_iterator<T, Count, Allocator>::operator++(int)
	{
		this_type i(*this);
		operator++();
		return i;
	}


	/////////////////////////////////////
	//   segmented_vector
	/////////////////////////////////////

	template <typename T, size_t Count, typename Allocator>
	inline segmented_vector<T, Count, Allocator>::segmented_vector(const Allocator& allocator)
	:	mAllocator(allocator)
	{
	}

	template <typename T, size_t Count, typename Allocator>
	inline segmented_vector<T, Count, Allocator>::segmented_vector(const segmented_vector& other)
	    : mAllocator(other.mAllocator)
	{
		InsertRange<false>(other.begin(), other.end());
	}

	template <typename T, size_t Count, typename Allocator>
	inline segmented_vector<T, Count, Allocator>::segmented_vector(segmented_vector&& other)
	    : mAllocator(other.mAllocator)
	{
		swap(other);
	}

	template <typename T, size_t Count, typename Allocator>
	inline segmented_vector<T, Count, Allocator>::segmented_vector(std::initializer_list<value_type> ilist, const Allocator& allocator)
		: mAllocator(allocator)
	{
		InsertRange<false>(ilist.begin(), ilist.end());
	}

	template <typename T, size_t Count, typename Allocator>
	inline segmented_vector<T, Count, Allocator>& segmented_vector<T, Count, Allocator>::operator=(
	    const segmented_vector& other)
	{
		if (EA_UNLIKELY(this == &other))
		{
			return *this;
		}
		// EASTL behaves as if propagate_on_container_copy_assignment
		// is globally false, so we don't propagate the allocator
		// here.
		if (size() > other.size())
		{
			EraseFromBack(size() - other.size());
		}

		// At this point size() <= other.size()
		// copy-assign elements which are already initialized.
		auto fromIt = other.begin();
		// TODO: consider doing segment copies, which do memcpy on
		// trivially copyable types.
		for (auto toIt = begin(); toIt != end(); ++toIt, ++fromIt)
		{
			*toIt = *fromIt;
		}

		// Now we insert (copy construct) all the missing elements
		// in-place
		InsertRange<false>(fromIt, other.end());

		return *this;
	}

	template <typename T, size_t Count, typename Allocator>
	inline segmented_vector<T, Count, Allocator>& segmented_vector<T, Count, Allocator>::operator=(
	    segmented_vector&& other)
	{
		if (EA_UNLIKELY(this == &other))
		{
			return *this;
		}

		// EASTL behaves as if propagate_on_container_move_assignment
		// and propagate_on_container_swap are both globally true, so
		// we just swap and clear the one we're returning.
		swap(other);
		other.Clear<true>();
		return *this;
	}

	template <typename T, size_t Count, typename Allocator>
	inline segmented_vector<T, Count, Allocator>::~segmented_vector()
	{
		Clear<true>();
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::allocator_type&
	segmented_vector<T, Count, Allocator>::get_allocator() noexcept
	{
		return mAllocator;
	}

	template <typename T, size_t Count, typename Allocator>
	inline const typename segmented_vector<T, Count, Allocator>::segment_type*
	segmented_vector<T, Count, Allocator>::first_segment() const noexcept
	{
		return mFirstSegment;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::segment_type*
	segmented_vector<T, Count, Allocator>::first_segment() noexcept
	{
		return mFirstSegment;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::const_iterator
	segmented_vector<T, Count, Allocator>::begin() const noexcept
	{
		iterator i;
		i.mSegment = mFirstSegment;
		if (mFirstSegment)
		{
			i.mCurrent = mFirstSegment->begin();
			i.mEnd = mFirstSegment->end();
		}
		else
		{
			i.mCurrent = nullptr;
		}
		return (const_iterator&)i;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::iterator
	segmented_vector<T, Count, Allocator>::begin() noexcept
	{
		iterator i;
		i.mSegment = mFirstSegment;
		if (mFirstSegment)
		{
			i.mCurrent = mFirstSegment->begin();
			i.mEnd = mFirstSegment->end();
		}
		else
			i.mCurrent = nullptr;
		return i;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::const_iterator
	segmented_vector<T, Count, Allocator>::end() const noexcept
	{
		iterator i;
		i.mCurrent = nullptr;
		return (const_iterator&)i;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::iterator
	segmented_vector<T, Count, Allocator>::end() noexcept
	{
		iterator i;
		i.mCurrent = nullptr;
		return i;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::size_type
	segmented_vector<T, Count, Allocator>::size() const noexcept
	{
		if (segment_type* segment = mLastSegment)
			return (mInUseSegmentCount-1)*Count + segment->mSize;
		return 0;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::size_type
	segmented_vector<T, Count, Allocator>::capacity() const noexcept
	{
		return (mInUseSegmentCount + mFreeListSegmentCount) * Count;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::size_type
	segmented_vector<T, Count, Allocator>::segment_count() const noexcept
	{
		return mInUseSegmentCount;
	}

	template <typename T, size_t Count, typename Allocator>
	inline T&
	segmented_vector<T, Count, Allocator>::front() noexcept
	{
#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
		if (EASTL_UNLIKELY(mFirstSegment == nullptr)) // We don't allow the user to reference an empty container.
			EASTL_FAIL_MSG("segmented_vector::front -- empty container");
#endif

		return mFirstSegment->begin()[0];
	}

	template <typename T, size_t Count, typename Allocator>
	inline T&
	segmented_vector<T, Count, Allocator>::back() noexcept
	{
#if EASTL_ASSERT_ENABLED && EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
		if (EASTL_UNLIKELY(mLastSegment == nullptr)) // We don't allow the user to reference an empty container.
			EASTL_FAIL_MSG("segmented_vector::back -- empty container");
#endif

		segment_type* lastSegment = mLastSegment;
		return lastSegment->begin()[lastSegment->mSize-1];
	}

	template <typename T, size_t Count, typename Allocator>
	inline bool
	segmented_vector<T, Count, Allocator>::empty() const noexcept
	{
		return mFirstSegment == nullptr;
	}

	template <typename T, size_t Count, typename Allocator>
	inline void
	segmented_vector<T, Count, Allocator>::clear()
	{
		Clear<false>();
	}

	template <typename T, size_t Count, typename Allocator>
	inline void segmented_vector<T, Count, Allocator>::reserve(size_type n)
	{
		while (capacity() < n)
		{
			segment_type* segment = AllocateNewSegment();
			AddToFreeList(segment);
		}
	}

	template <typename T, size_t Count, typename Allocator>
	inline void segmented_vector<T, Count, Allocator>::resize(size_type n, const value_type& v)
	{
		if (size() < n)
		{
			PushBack(n - size(), v);
		}
		else
		{
			EraseFromBack(size() - n);
		}
	}

	template <typename T, size_t Count, typename Allocator>
	inline void
	segmented_vector<T, Count, Allocator>::resize(size_type n)
	{
		resize(n, value_type());
	}

	template <typename T, size_t Count, typename Allocator>
	inline void segmented_vector<T, Count, Allocator>::shrink_to_fit() noexcept
	{
		static_assert(eastl::is_trivially_destructible_v<segment_type>,
		              "segment_type doesn't call it's destructor here!");
		while (mFreeList)
		{
			segment_type* toFree = mFreeList;
			mFreeList = reinterpret_cast<segment_type*>(toFree->mPrev);
			EASTLFree(mAllocator, toFree, sizeof(segment_type));
		}

		mFreeListSegmentCount = 0;
	}

	template <typename T, size_t Count, typename Allocator>
	inline T&
	segmented_vector<T, Count, Allocator>::push_back()
	{
		return *(::new (DoPushBack()) T());
	}

	template <typename T, size_t Count, typename Allocator>
	inline T&
	segmented_vector<T, Count, Allocator>::push_back(const T& value)
	{
		return *(::new (DoPushBack()) T(value));
	}

	template <typename T, size_t Count, typename Allocator>
	inline T&
	segmented_vector<T, Count, Allocator>::push_back(T&& value)
	{
		return *(::new (DoPushBack()) T(eastl::move(value)));
	}

	template <typename T, size_t Count, typename Allocator>
	inline void*
	segmented_vector<T, Count, Allocator>::push_back_uninitialized()
	{
		return DoPushBack();
	}

	template <typename T, size_t Count, typename Allocator>
	template <class... Args>
	T& segmented_vector<T, Count, Allocator>::emplace_back(Args&&... args)
	{
		return *(::new (DoPushBack()) T(eastl::forward<Args>(args)... ));
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
		T* const toDestroy = lastSegment->begin() + lastSegment->mSize;
		eastl::destroy_at(toDestroy);

		UpdateLastSegment();
	}

	template <typename T, size_t Count, typename Allocator>
	inline void
	segmented_vector<T, Count, Allocator>::erase_unsorted(segment_type& segment, typename segment_type::iterator it)
	{
		EA_UNUSED(segment);

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
			ret.mCurrent = nullptr;
		pop_back();
		return ret;
	}

	template <typename T, size_t Count, typename Allocator>
	void
	segmented_vector<T, Count, Allocator>::swap(this_type& other)
	{
		using eastl::swap;
		//
		// EASTL doesn't have allocator_traits it has the effective
		// behavior of propagate_on_container_swap = true for all
		// allocators.
		swap(mAllocator, other.mAllocator);
		swap(mFirstSegment, other.mFirstSegment);
		swap(mLastSegment, other.mLastSegment);
		swap(mFreeList, other.mFreeList);
		swap(mInUseSegmentCount, other.mInUseSegmentCount);
		swap(mFreeListSegmentCount, other.mFreeListSegmentCount);
	}

	template <typename T, size_t Count, typename Allocator>
	inline void*
	segmented_vector<T, Count, Allocator>::DoPushBack()
	{
		// This does not initialize the entry, it just makes room for it.
		if (segment_type* segment = mLastSegment)
		{
			size_type size = segment->mSize;
			if (size < Count)
			{
				++segment->mSize;
				return segment->begin() + size;
			}
			else
			{
				segment_type* lastSegment = mLastSegment;
				segment_type* newSegment = mLastSegment = GetUnusedSegmentForLastSegment(mLastSegment);
				lastSegment->mPrev &= ~segment_type::kIsLastSegment;
				lastSegment->mNext = newSegment;
				newSegment->mSize = 1;
				return newSegment->begin();
			}
		}
		else
		{
			segment = mFirstSegment = mLastSegment = GetUnusedSegmentForLastSegment(nullptr);
			segment->mSize = 1;
			return segment->begin();
		}
	}

	template <typename T, size_t Count, typename Allocator>
	inline void
	segmented_vector<T, Count, Allocator>::AddToFreeList(segment_type* segment)
	{
		segment->mPrev = reinterpret_cast<uintptr_t>(mFreeList);
		mFreeList = segment;
		mFreeListSegmentCount++;
	}

	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::segment_type*
	segmented_vector<T, Count, Allocator>::GetUnusedSegmentForLastSegment(segment_type* prevSegment)
	{
		segment_type* const newSegment = [&]
		{
			if (mFreeList)
			{
				mFreeListSegmentCount--;
				segment_type* const freeSegment = mFreeList;
				mFreeList = reinterpret_cast<segment_type*>(freeSegment->mPrev);
				return freeSegment;
			}

			return AllocateNewSegment();
		}();

		mInUseSegmentCount++;
		newSegment->mPrev = uintptr_t(prevSegment) | segment_type::kIsLastSegment;
		newSegment->mSize = 0;
		return newSegment;
	}


	template <typename T, size_t Count, typename Allocator>
	inline typename segmented_vector<T, Count, Allocator>::segment_type*
	segmented_vector<T, Count, Allocator>::AllocateNewSegment()
	{
		static_assert(eastl::is_trivially_constructible_v<segment_type>, "We're not initializing segment_type here");
		return (segment_type*)allocate_memory(mAllocator, sizeof(segment_type), EASTL_ALIGN_OF(segment_type), 0);
	}

	template <typename T, size_t Count, typename Allocator>
	template <bool bFreeMemory>
	inline void segmented_vector<T, Count, Allocator>::Clear()
	{
		if (bFreeMemory)
		{
			// Delete what was already in the free list before this
			// call.
			shrink_to_fit();
		}

		segment_type* segment = mFirstSegment;
		if (segment == nullptr)
		{
			return;
		}

		const auto& wrapUp = [this](segment_type* s)
		{
			s->DestroyData();
			if (!bFreeMemory)
			{
				AddToFreeList(s);
			}
			else
			{
				EASTLFree(mAllocator, s, sizeof(segment_type));
			}
		};

		// Note: the last segment is special because its active member
		// is mSize not mNext, so we need to deal with it separately.
		while (segment != mLastSegment)
		{
			segment_type* nextSegment = segment->mNext;
			wrapUp(segment);
			segment = nextSegment;
		}

		wrapUp(segment);

		mFirstSegment = nullptr;
		mLastSegment = nullptr;
		mInUseSegmentCount = 0;
	}

	template <typename T, size_t Count, typename Allocator>
	inline void segmented_vector<T, Count, Allocator>::UpdateLastSegment()
	{
		// Assumes there is a last segment.
		segment_type* lastSegment = mLastSegment;
		if (lastSegment->mSize == 0)
		{
			--mInUseSegmentCount;
			mLastSegment = (segment_type*)(lastSegment->mPrev & (~segment_type::kIsLastSegment));
			AddToFreeList(lastSegment);
			if (mLastSegment)
			{
				mLastSegment->mPrev |= segment_type::kIsLastSegment;
				mLastSegment->mSize = Count;
			}
			else
			{
				mFirstSegment = nullptr;
			}
		}
	}

	template <typename T, size_t Count, typename Allocator>
	inline void segmented_vector<T, Count, Allocator>::EraseFromBack(size_type toRemoveCount)
	{
		// This is only marginally better than doing consecutive pop_back() calls, is it worth it?

		// No bounds checking for this, we assume we have at least `toRemoveCount` elements.

		// Drop whole segments while we can.
		while (toRemoveCount >= mLastSegment->mSize)
		{
			const size_type removed = mLastSegment->mSize;
			mLastSegment->DestroyData();
			UpdateLastSegment();
			toRemoveCount -= removed;
		}

		// There's a chance we completely emptied the container here.
		if (mLastSegment == nullptr)
		{
			return;
		}

		// at this point we know there's more entries in the last
		// segment that there are elements left to remove.
		for (size_type i = 1; i <= toRemoveCount; ++i)
		{
			const size_type toRemoveIndex = mLastSegment->mSize - i;
			T* const toRemove = mLastSegment->begin() + toRemoveIndex;
			eastl::destroy_at(toRemove);
		}
		mLastSegment->mSize -= toRemoveCount;
	}

	template <typename T, size_t Count, typename Allocator>
	inline void segmented_vector<T, Count, Allocator>::PushBack(size_type toAddCount, const value_type& v)
	{
		// This is only marginally better than doing consecutive push_back(v) calls, is it worth it?

		if (!mLastSegment && (toAddCount > 0))
		{
			mFirstSegment = mLastSegment = GetUnusedSegmentForLastSegment(nullptr);
		}

		const auto& fillLastSegment = [&]()
		{
			const size_type spaceInSegment = Count - mLastSegment->mSize;
			const size_type addedThisLoop = eastl::min(toAddCount, spaceInSegment);
			for (size_type i = 0; i < addedThisLoop; ++i)
			{
				T* slot = mLastSegment->begin() + mLastSegment->mSize + i;
				new (slot) T(v);
			}
			mLastSegment->mSize += addedThisLoop;
			toAddCount -= addedThisLoop;
		};

		// fill the current last segment.
		fillLastSegment();

		// if there's still stuff to add, we need to add new segments as we go.
		while (toAddCount > 0)
		{
			mLastSegment = GetUnusedSegmentForLastSegment(mLastSegment);
			fillLastSegment();
		}
	}

	template <typename T, size_t Count, typename Allocator>
	template<bool bDoMove, typename ForwardIt>
	inline void segmented_vector<T, Count, Allocator>::InsertRange(ForwardIt begin, ForwardIt end)
	{
		// TODO: this can be greatly improved, e.g. memcpy entire
		// segments when the element types are trivially_copyable and
		// things like that, for now just do the trivial thing.
		for (auto it = begin; it != end; ++it)
		{
			if (bDoMove)
			{
				push_back(eastl::move(*it));
			}
			else
			{
				push_back(*it);
			}
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

	///////////////////////////////////////////////////////////////////////
	// global operators
	///////////////////////////////////////////////////////////////////////

	template<typename T, size_t Count, typename Allocator>
	inline bool operator==(const segmented_vector<T, Count, Allocator>& a, const segmented_vector<T, Count, Allocator>& b)
	{
		return (a.size() == b.size()) && eastl::equal(a.begin(), a.end(), b.begin());
	}

#if defined(EA_COMPILER_HAS_THREE_WAY_COMPARISON)
	template<typename T, size_t Count, typename Allocator>
	inline synth_three_way_result<T> operator<=>(const segmented_vector<T, Count, Allocator>& a, const segmented_vector<T, Count, Allocator>& b)
	{
		return eastl::lexicographical_compare_three_way(a.begin(), a.end(), b.begin(), b.end(), synth_three_way{});
	}
#endif

	template<typename T, size_t Count, typename Allocator>
	inline bool operator!=(const segmented_vector<T, Count, Allocator>& a, const segmented_vector<T, Count, Allocator>& b)
	{
		return !(a == b);
	}

	template<typename T, size_t Count, typename Allocator>
	inline bool operator<(const segmented_vector<T, Count, Allocator>& a, const segmented_vector<T, Count, Allocator>& b)
	{
		return eastl::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
	}

	template<typename T, size_t Count, typename Allocator>
	inline bool operator>(const segmented_vector<T, Count, Allocator>& a, const segmented_vector<T, Count, Allocator>& b)
	{
		return b < a;
	}

	template<typename T, size_t Count, typename Allocator>
	inline bool operator<=(const segmented_vector<T, Count, Allocator>& a, const segmented_vector<T, Count, Allocator>& b)
	{
		return !(b < a);
	}

	template<typename T, size_t Count, typename Allocator>
	inline bool operator>=(const segmented_vector<T, Count, Allocator>& a, const segmented_vector<T, Count, Allocator>& b)
	{
		return !(a < b);
	}

	template<typename T, size_t Count, typename Allocator>
	inline void swap(segmented_vector<T, Count, Allocator>& a, segmented_vector<T, Count, Allocator>& b)
	{
		a.swap(b);
	}
}

#endif
