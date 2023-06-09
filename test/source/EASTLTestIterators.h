/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Input concepts (see https://en.cppreference.com/w/cpp/named_req#Iterator):
//  - input, eg. std::istream_iterator, not part of EASTL.
//  - forward, eg. hash_map<K, V>::iterator
//  - bidirectional, eg. list<T>::iterator
//  - random access, eg. deque<T>::iterator
//  - contiguous, eg. vector<T>::iterator
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_TEST_ITERATORS_H
#define EASTL_TEST_ITERATORS_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif

struct EndSentinel {};

// a LegacyInputIterator that increments a referenced Integer.
// this iterator cannot be made a forward iterator or stronger because it is a single pass iterator.
template <typename Integer>
class InputIterator
{
protected:
	Integer* mInt{nullptr};
	Integer mEnd{0};

public:
	typedef ptrdiff_t							difference_type;
	typedef Integer								value_type;
	typedef Integer* pointer;
	typedef Integer& reference;
	typedef EASTL_ITC_NS::input_iterator_tag	iterator_category;

	explicit InputIterator(Integer* i) noexcept
		: mInt(i), mEnd(0) { }

	explicit InputIterator(EndSentinel, Integer end) noexcept
		: mInt(nullptr), mEnd(end) { }

	InputIterator() = default;
	InputIterator(const InputIterator& x) = default;
	InputIterator(InputIterator&& x) = default;
	InputIterator& operator=(const InputIterator& x) = default;
	InputIterator& operator=(InputIterator&& x) = default;

	reference operator*() const
	{
		return *mInt;
	}

	pointer operator->() const
	{
		return mInt;
	}

	InputIterator& operator++()
	{
		++(*mInt);
		return *this;
	}

	InputIterator operator++(int) const
	{
		// seeing as this is a single pass iterator, ie. the iterator modifies the source, pre- and post- increment are equivalent.
		// we iterate the copy, and even though this member function is const, the modification is visible by this object.
		InputIterator temp(*this);
		++temp;
		return temp;
	}

	template<typename Iterator>
	friend inline bool operator==(const InputIterator<Iterator>& a, const InputIterator<Iterator>& b);

}; // class InputIterator

template<typename Iterator>
inline bool operator==(const InputIterator<Iterator>& a, const InputIterator<Iterator>& b)
{
	return /* is iterator == sentinel? */ (a.mInt && !b.mInt && *a.mInt == b.mEnd) ||
		/* are iterators equal? */ (a.mInt && b.mInt && a.mInt == b.mInt) ||
		/* are sentinels equal? */ (!a.mInt && !b.mInt && a.mEnd == b.mEnd);
}

// LegacyInputIterator
template<typename Iterator>
inline bool operator!=(const InputIterator<Iterator>& a, const InputIterator<Iterator>& b)
{
	return !(a == b);
}

#endif // Header include guard
