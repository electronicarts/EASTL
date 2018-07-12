///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef EASTL_UNORDERED_MAP_H
#define EASTL_UNORDERED_MAP_H

#include <EASTL/internal/config.h>
#include <EASTL/hash_map.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif

namespace eastl
{
	/// unordered_map 
	///
	/// The original TR1 (technical report 1) used "hash_map" to name a hash
	/// table backed associative container of unique key-value pairs.  When the
	/// container was added to the C++11 standard the committee chose the name
	/// "unordered_map" to clarify that internally the elements are NOT sorted in
	/// any particular order.  We provide a template alias here to ensure feature
	/// parity with the original eastl::hash_map.
	///
	#if !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		template <typename Key,
				  typename T,
				  typename Hash = eastl::hash<Key>,
				  typename Predicate = eastl::equal_to<Key>,
				  typename Allocator = EASTLAllocatorType,
				  bool bCacheHashCode = false>
		using unordered_map = hash_map<Key, T, Hash, Predicate, Allocator, bCacheHashCode>;
    #endif

    /// unordered_multimap
	///
	/// Similar template alias as "unordered_map" except the contained elements
	/// need not be unique. See "hash_multimap" for more details.
	///
	#if !defined(EA_COMPILER_NO_TEMPLATE_ALIASES)
		template <typename Key,
				  typename T,
				  typename Hash = eastl::hash<Key>,
				  typename Predicate = eastl::equal_to<Key>,
				  typename Allocator = EASTLAllocatorType,
				  bool bCacheHashCode = false>
		using unordered_multimap = hash_multimap<Key, T, Hash, Predicate, Allocator, bCacheHashCode>;
    #endif

} // namespace eastl

#endif // Header include guard

