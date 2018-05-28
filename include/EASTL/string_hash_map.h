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


template<typename T, typename Hash = hash<string>, typename Predicate = equal_to<string>, typename Allocator = EASTLAllocatorType>
class string_hash_map : public eastl::hash_map<const char*, T, Hash, Predicate, Allocator>
{
public:
	typedef eastl::hash_map<const char*, T, Hash, Predicate, Allocator> base;
	typedef string_hash_map<T, Hash, Predicate, Allocator> this_type;
	typedef typename base::base_type::allocator_type allocator_type;
	typedef typename base::base_type::insert_return_type insert_return_type;
	typedef typename base::base_type::iterator iterator;
	//typedef typename base::base_type::reverse_iterator reverse_iterator;
	typedef typename base::base_type::const_iterator const_iterator;
	typedef typename base::base_type::size_type size_type;
	typedef typename base::base_type::value_type value_type;
	typedef typename base::mapped_type mapped_type;

						string_hash_map(const allocator_type& allocator = allocator_type()) : base(allocator) {}
						string_hash_map(const string_hash_map& src, const allocator_type& allocator = allocator_type());
						~string_hash_map();
	void				clear();
    void				clear(bool clearBuckets);

	this_type&			operator=(const this_type& x);

	insert_return_type	insert(const char* key, const T& value);
	insert_return_type	insert(const char* key);
	iterator			erase(const_iterator position);
	size_type			erase(const char* key);
	mapped_type&		operator[](const char* key);

private:
	char*				strduplicate(const char* str);

	// Not implemented right now
	//insert_return_type	insert(const value_type& value);
	//iterator			insert(iterator position, const value_type& value);
    //reverse_iterator	erase(reverse_iterator position);
    //reverse_iterator	erase(reverse_iterator first, reverse_iterator last);
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
void
string_hash_map<T, Hash, Predicate, Allocator>::clear()
{
	allocator_type& allocator = base::base_type::get_allocator();
	for (const_iterator i=base::base_type::begin(), e=base::base_type::end(); i!=e; ++i)
		EASTLFree(allocator, (void*)i->first, 0);
	base::base_type::clear();
}

template<typename T, typename Hash, typename Predicate, typename Allocator>
void
string_hash_map<T, Hash, Predicate, Allocator>::clear(bool clearBuckets)
{
	allocator_type& allocator = base::base_type::get_allocator();
	for (const_iterator i=base::base_type::begin(), e=base::base_type::end(); i!=e; ++i)
		EASTLFree(allocator, (void*)i->first, 0);
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
typename string_hash_map<T, Hash, Predicate, Allocator>::iterator
string_hash_map<T, Hash, Predicate, Allocator>::erase(const_iterator position)
{
	const char* key = position->first;
	iterator result = base::base_type::erase(position);
	EASTLFree(base::base_type::get_allocator(), (void*)key, 0);
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
	EASTL_ASSERT(key);
	iterator i = base::base_type::find(key);
	if (i != base::base_type::end())
		return i->second;
	return base::base_type::insert(strduplicate(key)).first->second;
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


}

#endif
