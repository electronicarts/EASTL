# EASTL Modules

## Introduction

We provide here a list of all top-level modules present or planned for future presence in EASTL. In some cases (e.g. algorithm), the module consists of many smaller submodules which are not described in detail here. In those cases you should consult the source code for those modules or consult the detailed documentation for those modules. This document is a high level overview and not a detailed document.

## Module List

| Module | Description |
|------|------|
| config | Configuration header. Allows for changing some compile-time options. |
| slist <br>fixed_slist | Singly-linked list.<br> fixed_slist is a version which is implemented via a fixed block of contiguous memory.|
| list<br> fixed_list | Doubly-linked list. |
| intrusive_list<br> intrusive_slist | List whereby the contained item provides the node implementation. |
| array | Wrapper for a C-style array which extends it to act like an STL container. |
| vector<br> fixed_vector | Resizable array container.
| vector_set<br> vector_multiset | Set implemented via a vector instead of a tree. Speed and memory use is improved but resizing is slower. |
| vector_map<br> vector_multimap | Map implemented via a vector instead of a tree. Speed and memory use is improved but resizing is slower. |
| deque | Double-ended queue, but also with random access. Acts like a vector but insertions and removals are efficient. |
| bit_vector | Implements a vector of bool, but the actual storage is done with one bit per bool. Not the same thing as a bitset. |
| bitset | Implements an efficient arbitrarily-sized bitfield. Note that this is not strictly the same thing as a vector of bool (bit_vector), as it is optimized to act like an arbitrary set of flags and not to be a generic container which can be iterated, inserted, removed, etc. |
| set<br> multiset<br> fixed_set<br> fixed_multiset | A set is a sorted unique collection, multiset is sorted but non-unique collection. |
| map<br> multimap<br> fixed_map<br> fixed_multimap | A map is a sorted associative collection implemented via a tree. It is also known as dictionary. |
| hash_map<br> hash_multimap<br> fixed_hash_map<br> fixed_hash_multimap |  Map implemented via a hash table. |
| intrusive_hash_map<br> intrusive_hash_multimap<br> intrusive_hash_set<br> intrusive_hash_multiset | hash_map whereby the contained item provides the node implementation, much like intrusive_list. |
| hash_set<br> hash_multiset<br> fixed_hash_set<br> fixed_hash_map | Set implemented via a hash table.
| basic_string<br> fixed_string<br> fixed_substring | basic_string is a character string/array.<br> fixed_substring is a string which is a reference to a range within another string or character array.<br> cow_string is a string which implements copy-on-write. |
| algorithm | min/max, find, binary_search, random_shuffle, reverse, etc. |
| sort | Sorting functionality, including functionality not in STL. quick_sort, heap_sort, merge_sort, shell_sort, insertion_sort, etc. |
| numeric | Numeric algorithms: accumulate, inner_product, partial_sum, adjacent_difference, etc. |
| heap | Heap structure functionality: make_heap, push_heap, pop_heap, sort_heap, is_heap, remove_heap, etc. |
| stack | Adapts any container into a stack. |
| queue | Adapts any container into a queue. |
| priority_queue | Implements a conventional priority queue via a heap structure. |
| type_traits | Type information, useful for writing optimized and robust code. Also used for implementing optimized containers and algorithms. |
| utility | pair, make_pair, rel_ops, etc. |
| functional | Function objects. |
| iterator | Iteration for containers and algorithms. |
| smart_ptr | Smart pointers: shared_ptr, shared_array, weak_ptr, scoped_ptr, scoped_array, linked_ptr, linked_array, intrusive_ptr. |
 

## Module Behaviour

The overhead sizes listed here refer to an optimized release build; debug builds may add some additional overhead. Some of the overhead sizes may be off by a little bit (usually at most 4 bytes). This is because the values reported here are those that refer to when EASTL's container optimizations have been complete. These optimizations may not have been completed as you are reading this.

| Container |Stores | Container Overhead (32 bit) | Container Overhead (64 bit) | Node Overhead (32 bit) | Node Overhead (64 bit) | Iterator category | size() efficiency | operator[] efficiency | Insert efficiency | Erase via Iterator efficiency | Find efficiency | Sort efficiency |
|------|------|------|------|------|------|------|------|------|------|------|------|------|
| slist | T | 8 | 16 | 4 | 8 | f | n | - | 1 | 1 | n | n+ |
| list | T | 12 | 24 | 8 | 16 | b | n | - | 1 | 1 | n | n log(n) |
| intrusive_slist | T | 4 | 8 | 4 | 8 | f | n | - | 1 | 1 | 1 | n+ |
| intrusive_list | T | 8 | 16 | 8 | 16 | b | n | - | 1 | 1 | 1 | n log(n) |
| array | T | 0 | 0 | 0 | 0 | r | 1 | 1 | - | - | n | n log(n) |
| vector | T | 16 | 32 | 0 | 0 | r | 1 | 1 | 1 at end, else n | 1 at end, else n | n | n log(n) |
| vector_set | T | 16 | 32 | 0 | 0 | r | 1 | 1 | 1 at end, else n | 1 at end, else n | log(n) | 1 |
| vector_multiset | T | 16 | 32 | 0 | 0 | r | 1 | 1 | 1 at end, else n | 1 at end, else n | log(n) | 1 |
| vector_map | Key, T | 16 | 32 | 0 | 0 | r | 1 | 1 | 1 at end, else n | 1 at end, else n | log(n) | 1 |
| vector_multimap | Key, T | 16 | 32 | 0 | 0 | r | 1 | 1 | 1 at end, else n | 1 at end, else n | log(n) | 1 |
| deque | T | 44 | 84 | 0 | 0 | r | 1 | 1 | 1 at begin or end, else n / 2 | 1 at begin or end, else n / 2 | n | n log(n) |
| bit_vector | bool | 8 | 16 | 0 | 0 | r | 1 | 1 | 1 at end, else n | 1 at end, else n | n | n log(n) |
| string (all types) | T | 16 | 32 | 0 | 0 | r | 1 | 1 | 1 at end, else n | 1 at end, else n | n | n log(n) |
| set | T | 24 | 44 | 16 | 28 | b | 1 | - | log(n) | log(n) | log(n) | 1 |
| multiset | T | 24 | 44 | 16 | 28 | b | 1 | - | log(n) | log(n) | log(n) | 1 |
| map | Key, T | 24 | 44 | 16 | 28 | b | 1 | log(n) | log(n) | log(n) | log(n) | 1 |
| multimap | Key, T | 24 | 44 | 16 | 28 | b | 1 | - | log(n) | log(n) | log(n) | 1 |
| hash_set | T | 16 | 20 | 4 | 8 | b | 1 | - | 1 | 1 | 1 | - |
| hash_multiset | T | 16 | 20 | 4 | 8 | b | 1 | - | 1 | 1 | 1 | - |
| hash_map | Key, T | 16 | 20 | 4 | 8 | b | 1 | - | 1 | 1 | 1 | - |
| hash_multimap | Key, T | 16 | 20 | 4 | 8 | b | 1 | - | 1 | 1 | 1 | - |
| intrusive_hash_set | T | 16 | 20 | 4 | 8 | b | 1 | - | 1 | 1 | 1 | - |
| intrusive_hash_multiset | T | 16 | 20 | 4 | 8 | b | 1 | - | 1 | 1 | 1 | - |
| intrusive_hash_map | T <small>(Key == T)</small> | 16 | 20 | 4 | 8 | b | 1 | - | 1 | 1 | 1 | - |
| intrusive_hash_multimap | T <small>(Key == T) </small> | 16 | 20 | 4 | 8 | b | 1 | - | 1 | 1 | 1 | - |

* \- means that the operation does not exist.
* 1 means amortized constant time. Also known as O(1)
* n means time proportional to the container size. Also known as O(n)
* log(n) means time proportional to the natural logarithm of the container size. Also known as O(log(n))
* n log(n) means time proportional to log(n) times the size of the container. Also known as O(n log(n))
* n+ means that the time is at least n, and possibly higher.
* Iterator meanings are: f = forward iterator; b = bidirectional iterator, r = random iterator.
* Overhead indicates approximate per-element overhead memory required in bytes. Overhead doesn't include possible additional overhead that may be imposed by the memory heap used to allocate nodes. General heaps tend to have between 4 and 16 bytes of overhead per allocation, depending on the heap.
* Some overhead values are dependent on the structure alignment characteristics in effect. The values reported here are those that would be in effect for a system that requires pointers to be aligned on boundaries of their size and allocations with a minimum of 4 bytes (thus one byte values get rounded up to 4).
* Some overhead values are dependent on the size_type used by containers. We assume a size_type of 4 bytes, even for 64 bit machines, as this is the EASTL default.
* Inserting at the end of a vector may cause the vector to be resized; resizing a vector is O(n). However, the amortized time complexity for vector insertions at the end is constant.
* Sort assumes the usage of the best possible sort for a large container of random data. Some sort algorithms (e.g. quick_sort) require random access iterators and so the sorting of some containers requires a different sort algorithm. We do not include bucket or radix sorts, as they are always O(n).
* Some containers (e.g. deque, hash*) have unusual data structures that make per-container and per-node overhead calculations not quite account for all memory.

----------------------------------------------
End of document
