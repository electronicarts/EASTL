///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_STRING_HASH_MAP_H
#define EASTL_STRING_HASH_MAP_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once 
#endif

#include <EASTL/hash_map.h>
#include <EASTL/string.h>

namespace eastl
{


// Note: this class creates a copy of the key on insertion and manages it in its own internal
// buffer this has side effects like:
//
//     const char* p = "X";
//     string_hash_map<int> map;
//     auto x = map.insert(p, 1);
//     const char* the_key = x.first->first;
//     the_key != p; <<<< This is true, since we copied the key internally.
//
//
// TODO: This publically inherits from hash_map but really shouldn't since it deliberately uses
// name hiding to change the behaviour of some of its methods in a way that's incompatible with
// what hash_map does. e.g:
//
//     void foo(hash_map<const char*, int, hash<const char*>, str_equal_to<const char*>>& map)
//     {
//         map["a"] = 1;
//     }
//
//     string_hash_map<int> strMap;
//
//     // This is very bad! foo() will use hash_map's implementation of operator[], which
//     // doesn't copy the char* but strMap assumes it owns all its key pointers, this
//     // will cause a crash when strMap is destructed.
//     foo(strMap);
//
template<typename T, typename Hash = hash<const char*>, typename Predicate = str_equal_to<const char*>, typename Allocator = EASTLAllocatorType>
class string_hash_map : public eastl::hash_map<const char*, T, Hash, Predicate, Allocator>
{
public:
	typedef eastl::hash_map<const char*, T, Hash, Predicate, Allocator> base;
	typedef string_hash_map<T, Hash, Predicate, Allocator> this_type;
	typedef typename base::base_type::allocator_type allocator_type;
	typedef typename base::base_type::insert_return_type insert_return_type;
	typedef typename base::base_type::iterator iterator;
//  typedef typename base::base_type::reverse_iterator reverse_iterator;
	typedef typename base::base_type::const_iterator const_iterator;
	typedef typename base::base_type::size_type size_type;
	typedef typename base::base_type::value_type value_type;
	typedef typename base::mapped_type mapped_type;

	string_hash_map(const allocator_type& allocator = allocator_type()) : base(allocator) {}

	// Note/warning: the copy constructor does not copy the underlying allocator instance. This
	// is different from what hash_map does.
	string_hash_map(const string_hash_map& src, const allocator_type& allocator = allocator_type());
						~string_hash_map();

	void				clear();
    void				clear(bool clearBuckets);

	this_type&			operator=(const this_type& x);

	insert_return_type	 insert(const char* key, const T& value);
	insert_return_type	 insert(const char* key);
	pair<iterator, bool> insert_or_assign(const char* key, const T& value);
	iterator			 erase(const_iterator position);
	size_type			 erase(const char* key);
	mapped_type&		 operator[](const char* key);

	// Note: the `emplace` methods are restricted to take the key directly as a first
	// parameter, and the value will be constructed from the template arguments in-place.
	// This choice makes emplace literally equivalent to `try_emplace`.
	template <class... Args>
	insert_return_type emplace(const char* key, Args&&... valArgs);

	template <class... Args>
	iterator emplace_hint(const_iterator position, const char* key, Args&&... valArgs);

	template <class... Args>
	inline insert_return_type try_emplace(const char* k, Args&&... valArgs);

	template <class... Args>
	inline iterator try_emplace(const_iterator, const char* k, Args&&... valArgs);

private:
	char*				strduplicate(const char* str);
	void				free(const char* str);

	// Not implemented right now
	// insert_return_type	insert(const value_type& value);
	// iterator				insert(iterator position, const value_type& value);
    // reverse_iterator		erase(reverse_iterator position);
    // reverse_iterator		erase(reverse_iterator first, reverse_iterator last);
};


template<typename T, typename Hash, typename Predicate, typename Allocator>
string_hash_map<T, Hash, Predicate, Allocator>::string_hash_map(const string_hash_map& src, const allocator_type& allocator) : base(allocator)
{
	for (const_iterator i=src.begin(), e=src.end(); i!=e; ++i)
		base::base_type::insert(eastl::make_pair(strduplicate(i->first), i->second));
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
string_hash_map<T, Hash, Predicate, Allocator>::~string_hash_map()
{
	clear();
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
void string_hash_map<T, Hash, Predicate, Allocator>::clear()
{
	for (const_iterator i=base::base_type::begin(), e=base::base_type::end(); i!=e; ++i)
		free(i->first);
	base::base_type::clear();
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
void string_hash_map<T, Hash, Predicate, Allocator>::clear(bool clearBuckets)
{
	for (const_iterator i=base::base_type::begin(), e=base::base_type::end(); i!=e; ++i)
		free(i->first);
	base::base_type::clear(clearBuckets);
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
typename string_hash_map<T, Hash, Predicate, Allocator>::this_type&
string_hash_map<T, Hash, Predicate, Allocator>::operator=(const this_type& x)
{
	allocator_type allocator = base::base_type::get_allocator();
	this->~this_type();
	new (this) this_type(x, allocator);
	return *this;
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
typename string_hash_map<T, Hash, Predicate, Allocator>::insert_return_type
string_hash_map<T, Hash, Predicate, Allocator>::insert(const char* key)
{
	return insert(key, mapped_type());
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
typename string_hash_map<T, Hash, Predicate, Allocator>::insert_return_type
string_hash_map<T, Hash, Predicate, Allocator>::insert(const char* key, const T& value)
{
	EASTL_ASSERT(key);
	iterator i = base::base_type::find(key);
	if (i != base::base_type::end())
	{
		insert_return_type ret;
		ret.first = i;
		ret.second = false;
		return ret;
	}
	return base::base_type::insert(eastl::make_pair(strduplicate(key), value));
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
eastl::pair<typename string_hash_map<T, Hash, Predicate, Allocator>::iterator, bool>
string_hash_map<T, Hash, Predicate, Allocator>::insert_or_assign(const char* key, const T& value)
{
	iterator i = base::base_type::find(key);
	if (i != base::base_type::end())
	{
		return base::base_type::insert_or_assign(i->first, value);
	}
	else
	{
		return base::base_type::insert_or_assign(strduplicate(key), value);
	}
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
typename string_hash_map<T, Hash, Predicate, Allocator>::iterator
string_hash_map<T, Hash, Predicate, Allocator>::erase(const_iterator position)
{
	const char* key = position->first;
	iterator result = base::base_type::erase(position);
	free(key);
	return result;
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
typename string_hash_map<T, Hash, Predicate, Allocator>::size_type
string_hash_map<T, Hash, Predicate, Allocator>::erase(const char* key)
{
    const iterator it(base::base_type::find(key));

    if(it != base::base_type::end())
    {
        erase(it);
        return 1;
    }
    return 0;
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
typename string_hash_map<T, Hash, Predicate, Allocator>::mapped_type&
string_hash_map<T, Hash, Predicate, Allocator>::operator[](const char* key)
{
	using base_value_type = typename base::base_type::value_type;

	EASTL_ASSERT(key);
	iterator i = base::base_type::find(key);
	if (i != base::base_type::end())
		return i->second;
	return base::base_type::insert(base_value_type(pair_first_construct, strduplicate(key))).first->second;
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
template <class... Args>
typename string_hash_map<T, Hash, Predicate, Allocator>::insert_return_type
string_hash_map<T, Hash, Predicate, Allocator>::emplace(const char* key, Args&&...valArgs)
{
	return try_emplace(key, eastl::forward<Args>(valArgs)...);
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
template <class... Args>
typename string_hash_map<T, Hash, Predicate, Allocator>::iterator
string_hash_map<T, Hash, Predicate, Allocator>::emplace_hint(typename string_hash_map<T, Hash, Predicate, Allocator>::const_iterator hint, const char* key, Args&&...valArgs)
{
	return try_emplace(hint, key, eastl::forward<Args>(valArgs)...);
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
template <class... Args>
typename string_hash_map<T, Hash, Predicate, Allocator>::insert_return_type
string_hash_map<T, Hash, Predicate, Allocator>::try_emplace(const char* k, Args&&...valArgs)
{
	// This is lifted from hash_map::try_emplace_forwarding.  The point is that we don't want to
	// allocate space for a copy of `k` unless we know we're going to insert it.
	using hashtable_type = typename base::base_type;
	const auto key_data = hashtable_type::DoFindKeyData(k);
	if (key_data.node)
	{
		// Node exists, no insertion needed.
		return eastl::pair<iterator, bool>(
			iterator(key_data.node, hashtable_type::mpBucketArray + key_data.bucket_index), false);
	}
	else
	{
		// We're adding a new node, copy the key.
		const char* keyCopy = strduplicate(k);
		typename base::node_type* const pNodeNew = hashtable_type::DoAllocateNode(
			piecewise_construct, eastl::forward_as_tuple(keyCopy), forward_as_tuple(eastl::forward<Args>(valArgs)...));
		return hashtable_type::template DoInsertUniqueNode<true>(keyCopy, key_data.code, key_data.bucket_index,
			                                                     pNodeNew);
	}
}


template<typename T, typename Hash, typename Predicate, typename Allocator>
template <class... Args>
typename string_hash_map<T, Hash, Predicate, Allocator>::iterator
string_hash_map<T, Hash, Predicate, Allocator>::try_emplace(typename string_hash_map<T, Hash, Predicate, Allocator>::const_iterator hint, const char* key, Args&&...valArgs)
{
	EA_UNUSED(hint);
	// The hint is currently ignored in all our implementations :(
	auto ret = try_emplace(key, eastl::forward<Args>(valArgs)...);
	return base::base_type::DoGetResultIterator(true_type(), ret);
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
char*
string_hash_map<T, Hash, Predicate, Allocator>::strduplicate(const char* str)
{
	size_t len = strlen(str);
	char* result = (char*)EASTLAlloc(base::base_type::get_allocator(), (len + 1));
	memcpy(result, str, len+1);
	return result;
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
void
string_hash_map<T, Hash, Predicate, Allocator>::free(const char* str)
{
	EASTLFree(base::base_type::get_allocator(), (void*)str, 0);
}


}

#endif
