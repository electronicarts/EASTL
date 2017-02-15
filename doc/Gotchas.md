# EASTL Gotchas

There are some cases where the EASTL design results in "gotchas" or behavior that isn't necessarily what the new user would expect. These are all situations in which this behavior may be undesirable. One might ask, "Why not change EASTL to make these gotchas go away?" The answer is that in each case making the gotchas go away would either be impossible or would compromise the functionality of the library.

## Summary

The descriptions here are intentionally terse; this is to make them easier to visually scan.

1. [map::operator[] can create elements.](#mapoperator-can-create-elements)
2. [char* converts to string silently.](#char-converts-to-string-silently)
3. [char* is compared by ptr and not by contents.](#char-is-compared-by-ptr-and-not-by-contents)
4. [Iterators can be invalidated by container mutations.](#iterators-can-be-invalidated-by-container-mutations)
5. [Vector resizing may cause ctor/dtor cascades.](#vector-resizing-may-cause-ctordtor-cascades)
6. [Vector and string insert/push_back/resize can reallocate.](#vector-and-string-insertpush_backresize-can-reallocate)
7. [Deriving from containers may not work.](#deriving-from-containers-may-not-work)
8. [set::iterator is const_iterator.](#setiterator-is-const_iterator)
9. [Inserting elements means copying by value.](#inserting-elements-means-copying-by-value)
10. [Containers of pointers can leak if you aren't careful.](#containers-of-pointers-can-leak-if-you-arent-careful)
11. [Containers of auto_ptrs can crash.](#containers-of-auto_ptrs-can-crash)
12. [Remove algorithms don't actually remove elements.](#remove-algorithms-dont-actually-remove-elements)
13. [list::size() is O(n).](#listsize-is-on)
14. [vector and deque::size() may incur integer division.](#vector-and-dequesize-may-incur-integer-division)
15. [Be careful making custom Compare functions.](#be-careful-making-custom-compare-functions)
16. [Comparisons involving floating point are dangerous.](#comparisons-involving-floating-point-are-dangerous)
17. [Writing beyond string::size and vector::size is dangerous.](#writing-beyond-stringsize-and-vectorsize-is-dangerous)
18. [Container operator=() doesn't copy allocators.](#container-operator-doesnt-copy-allocators)

## Detail

### map::operator[] can create elements.

By design, map operator[] creates a value for you if it isn't already present. The reason for this is that the alternative behavior would be to throw an exception, and such behavior isn't desirable. The resolution is to simply use the map::find function instead of operator[].

### char* converts to string silently.

The string class has a non-explicit constructor that takes char* as an argument. Thus if you pass char* to a function that takes a string object, a temporary string will be created. In some cases this is undesirable behavior but the user may not notice it right away, as the compiler gives no warnings. The reason that the string constructor from char* is not declared explicit is that doing so would prevent the user from expressions such as: string s = "hello". In this example, no temporary string object is created, but the syntax is not possible if the char* constructor is declared explicit. Thus a decision to make the string char* constructor explicit involves tradeoffs.

There is an EASTL configuration option called EASTL_STRING_EXPLICIT which makes the string char* ctor explicit and avoids the behaviour described above.

### char* is compared by ptr and not by contents.

If you have a set of strings declared as set<char*>, the find function will compare via the pointer value and not the string contents. The workaround is to make a set of string objects or, better, to supply a custom string comparison function to the set. The workaround is not to declare a global operator< for type char*, as that could cause other systems to break.

### Iterators can be invalidated by container mutations

With some containers, modifications of them may invalidate iterators into them. With other containers, modifications of them only an iterator if the modification involves the element that iterator refers to. Containers in the former category include vector, deque, basic_string (string), vector_map, vector_multimap, vector_set, and vector_multiset. Containers in the latter category include list, slist, map, multimap, multiset, all hash containers, and all intrusive containers.

### Vector resizing may cause ctor/dtor cascades.

If elements are inserted into a vector in middle of the sequence, the elements from the insertion point to the end will be copied upward. This will necessarily cause a series of element constructions and destructions as the elements are copied upward. Similarly, if an element is appended to a vector but the vector capacity is exhausted and needs to be reallocated, the entire vector will undergo a construction and destruction pass as the values are copied to the new storage. This issue exists for deque as well, though to a lesser degree. For vector, the resolution is to reserve enough space in your vector to prevent such reallocation. For deque the resolution is to set its subarray size to enough to prevent such reallocation. Another solution that can often be used is to take advantage of the has_trivial_relocate type trait, which can cause such moves to happen via memcpy instead of via ctor/dtor calls. If your class can be safely memcpy'd, you can use EASTL_DECLARE_TRIVIAL_RELOCATE to tell the compiler it can be memcpy'd. Note that built-in scalars (e.g. int) already are automatically memcpy'd by EASTL.

### Vector and string insert/push_back/resize can reallocate.

If you create an empty vector and use push_back to insert 100 elements, the vector will reallocate itself at least three or four times during the operation. This can be an undesirable thing. The best thing to do if possible is to reserve the size you will need up front in the vector constructor or before you add any elements.

### Deriving from containers may not work.

EASTL containers are not designed with the guarantee that they can be arbitrarily subclassed. This is by design and is done for performance reasons, as such guarantees would likely involve making containers use virtual functions. However, some types of subclassing can be successful and EASTL does such subclassing internally to its advantage. The primary problem with subclassing results when a parent class function calls a function that the user wants to override. The parent class cannot see the overridden function and silent unpredictable behavior will likely occur. If your derived container acts strictly as a wrapper for the container then you will likely be able to successfully subclass it.

### set::iterator is const_iterator.

The reason this is so is that a set is an ordered container and changing the value referred to by an iterator could make the set be out of order. Thus, set and multiset iterators are always const_iterators. If you need to change the value and are sure the change will not alter the container order, use const_cast or declare mutable member variables for your contained object. This resolution is the one blessed by the C++ standardization committee. This issue is addressed in more detail in the EASTL FAQ.

### Inserting elements means copying by value.

When you insert an element into a (non-intrusive) container, the container makes a copy of the element. There is no provision to take over ownership of an object from the user. The exception to this is of course when you use a container of pointers instead of a container of values. See the entry below regarding containers of pointers. Intrusive containers (e.g. intrusive_list) do in fact take over the user-provided value, and thus provide another advantage over regular containers in addition to avoiding memory allocation.

### Containers of pointers can leak if you aren't careful.

Containers of points don't know or care about the possibility that the pointer may have been allocated and need to be freed. Thus if you erase such elements from a container they are not freed. The resolution is to manually free the pointers when removing them or to instead use a container of smart pointers (shared smart pointers, in particular). This issue is addressed in more detail in the EASTL FAQ and the auto_ptr-related entry below.

### Containers of auto_ptrs can crash

We suggested above that the user can use a container of smart pointers to automatically manage contained pointers. However, you don't want to use auto_ptr, as auto_ptrs cannot be safely assigned to each other; doing so results in a stale pointer and most likely a crash.

### Remove algorithms don't actually remove elements.

Algorithms such as remove, remove_if, remove_heap, and unique do not erase elements from the sequences they work on. Instead, they return an iterator to the new end of the sequence and the user must call erase with that iterator in order to actually remove the elements from the container. This behavior exists because algorithms work on sequences via iterators and don't know how to work with containers. Only the container can know how to best erase its own elements. In each case, the documentation for the algorithm reminds the user of this behavior. Similarly, the copy algorithm copies elements from one sequence to another and doesn't modify the size of the destination sequence. So the destination must hold at least as many items as the source, and if it holds more items, you may want to erase the items at the end after the copy.

### list::size() is O(n).

By this we mean that calling size() on a list will iterate the list and add the size as it goes. Thus, getting the size of a list is not a fast operation, as it requires traversing the list and counting the nodes. We could make list::size() be fast by having a member mSize variable. There are reasons for having such functionality and reasons for not having such functionality. We currently choose to not have a member mSize variable as it would add four bytes to the class, add processing to functions such as insert and erase, and would only serve to improve the size function, but no other function. The alternative argument is that the C++ standard states that std::list should be an O(1) operation (i.e. have a member size variable), most C++ standard library list implementations do so, the size is but an integer which is quick to update, and many users expect to have a fast size function. All of this applies to slist and intrusive_list as well.

Note that EASTL's config.h file has an option in it to cause list and slist to cache their size with an mSize variable and thus make size() O(1). This option is disabled by default.

### vector and deque::size() may incur integer division.

Some containers (vector and deque in particular) calculate their size by pointer subtraction. For example, the implementation of vector::size() is 'return mpEnd - mpBegin'. This looks like a harmless subtraction, but if the size of the contained object is not an even power of two then the compiler will likely need to do an integer division to calculate the value of the subtracted pointers. One might suggest that vector use mpBegin and mnSize as member variables instead of mpBegin and mpEnd, but that would incur costs in other vector operations. The suggested workaround is to iterate a vector instead of using a for loop and operator[] and for those cases where you do use a for loop and operator[], get the size once at the beginning of the loop instead of repeatedly during the condition test.

### Be careful making custom Compare functions.

A Compare function compares two values and returns true if the first is less than the second. This is easy to understand for integers and strings, but harder to get right for more complex structures. Many a time have people decided to come up with a fancy mechanism for comparing values and made mistakes. The FAQ has a couple entries related to this. See http://blogs.msdn.com/oldnewthing/archive/2003/10/23/55408.aspx for a story about how this can go wrong by being overly clever.

### Comparisons involving floating point are dangerous.

Floating point comparisons between two values that are very nearly equal can result in inconsistent results. Similarly, floating point comparisons between NaN values will always generate inconsistent results, as NaNs by definition always compare as non-equal. You thus need to be careful when using comparison functions that work with floating point values. Conversions to integral values may help the problem, but not necessarily.

### Writing beyond string::size and vector::size is dangerous.

A trick that often comes to mind when working with strings is to set the string capacity to some maximum value, strcpy data into it, and then resize the string when done. This can be done with EASTL, but only if you resize the string to the maximum value and not reserve the string to the maximum value. The reason is that when you resize a string from size (n) to size (n + count), the count characters are zeroed and overwrite the characters that you strcpyd.

The following code is broken:

```cpp
string mDataDir;


  mDataDir.reserve(kMaxPathLength); // reserve
  strcpy(&mDataDir[0], "blah/blah/blah");

mDataDir.resize(strlen(&mDataDir[0])); // Overwrites your blah/... with 00000...
```

This following code is OK:

```cpp
string mDataDir;


  mDataDir.resize(kMaxPathLength); // resize
  strcpy(&mDataDir[0], "blah/blah/blah");
  
mDataDir.resize(strlen(&mDataDir[0]));
```

### Container operator=() doesn't copy allocators.

EASTL container assignment (e.g. vector::operator=(const vector&)) doesn't copy the allocator. There are good and bad reasons for doing this, but that's how it acts. So you need to beware that you need to assign the allocator separately or make a container subclass which overrides opeator=() and does this.

----------------------------------------------
End of document



