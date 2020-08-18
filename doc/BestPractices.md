# EASTL Best Practices

In this document we discuss best practices for using EASTL. The primary emphasis is on performance with a secondary emphasis on correctness and maintainability. Some best practices apply only to some situations, and these will be pointed out as we go along. In order to be easily digestible, we present these practices as a list of items in the tone of the Effective C++ series of books.

## Summary

The descriptions here are intentionally terse; this is to make them easier to visually scan.

1. [Consider intrusive containers.](#consider-intrusive-containers)
2. [Consider fixed-size containers.](#consider-fixed-size-containers)
3. [Consider custom allocators.](#consider-custom-allocators)
4. [Consider hash tables instead of maps.](#consider-hash-tables-instead-of-maps)
5. [Consider a vector_map (a.k.a. sorted vector) for unchanging data.](#consider-a-vector_map-aka-sorted-vector-for-unchanging-data)
6. [Consider slist instead of list.](#consider-slist-instead-of-list)
7. [Avoid redundant end() and size() in loops.](#avoid-redundant-end-and-size-in-loops)
8. [Iterate containers instead of using operator\[\].](#iterate-containers-instead-of-using-operator)
9. [Learn to use the string class appropriately.](#learn-to-use-the-string-class-appropriately)
10. [Cache list size if you want size() to be O(1).](#cache-list-size-if-you-want-listsize-to-be-o1)
11. [Use empty() instead of size() when possible.](#use-empty-instead-of-size-when-possible)
12. [Know your container efficiencies.](#know-your-container-efficiencies)
13. [Use vector::reserve.](#use-vectorreserve)
14. [Use vector::set_capacity to trim memory usage.](#use-vectorset_capacity-to-trim-memory-usage)
15. [Use swap() instead of a manually implemented version.](#use-swap-instead-of-a-manually-implemented-version)
16. [Consider storing pointers instead of objects.](#consider-storing-pointers-instead-of-objects)
17. [Consider smart pointers instead of raw pointers.](#consider-smart-pointers-instead-of-raw-pointers)
18. [Use iterator pre-increment instead of post-increment.](#use-iterator-pre-increment-instead-of-post-increment)
19. [Make temporary references so the code can be traced/debugged.](#make-temporary-references-so-the-code-can-be-traceddebugged)
20. [Consider bitvector or bitset instead of vector\<bool>.](#consider-bitvector-or-bitset-instead-of-vector)
21. [Vectors can be treated as contiguous memory.](#vectors-can-be-treated-as-contiguous-memory)
22. [Search hash_map\<string> via find_as() instead of find().](#search-hash_map-via-find_as-instead-of-find)
23. [Take advantage of type_traits (e.g. EASTL_DECLARE_TRIVIAL_RELOCATE).](#take-advantage-of-type_traits-eg-eastl_declare_trivial_relocate)
24. [Name containers to track memory usage.](#name-containers-to-track-memory-usage)
25. [Learn the algorithms.](#learn-the-algorithms)
26. [Pass and return containers by reference instead of value.](#pass-and-return-containers-by-reference-instead-of-value)
27. [Consider using reset() for fast container teardown.](#consider-using-reset-for-fast-container-teardown)
28. [Consider using fixed_substring instead of copying strings.](#consider-using-fixed_substring-instead-of-copying-strings)
29. [Consider using vector::push_back(void).](#consider-using-vectorpush_backvoid)

## Detail

### Consider intrusive containers.

Intrusive containers (such as intrusive_list) differ from regular containers (such as list) in that they use the stored objects to manage the linked list instead of using nodes allocated from a memory heap. The result is better usage of memory. Additionally intrusive_list objects can be removed from their list without knowing what list they belong to. To make an intrusive_list of Widgets, you have Widget inherit from intrusive_list_node or simply have mpPrev/mpNext member variables.

To create an intrusive_list container, you can use the following code:

```cpp
class Widget : public intrusive_list_node

{ };



intrusive_list<Widget> widgetList;

widgetList.push_back(someWidget);
```

### Consider fixed-size containers.

Fixed-size containers (such as fixed_list) are variations of regular containers (such as list) in that they allocate from a fixed block of local memory instead of allocating from a generic heap. The result is better usage of memory due to reduced fragmentation, better cache behavior, and faster allocation/deallocation. The presence of fixed-size containers negate the most common complaint that people have about STL: that it fragments the heap or "allocates all over the place."

EASTL fixed containers include:

* fixed_list
* fixed_slist
* fixed_vector
* fixed_string
* fixed_map
* fixed_multimap
* fixed_set
* fixed_multiset
* fixed_hash_map
* fixed_hash_multimap
* fixed_hash_set
* fixed_hash_multiset

To create a fixed_set, you can use the following code:

```cpp
fixed_set<int, 25> intSet; // Create a set capable of holding 25 elements.

intSet.push_back(37);
```

### Consider custom allocators.

While EASTL provides fixed-size containers in order to control container memory usage, EASTL lets you assign a custom allocator to any container. This lets you define your own memory pool. EASTL has a more flexible and powerful mechanism of doing this that standard STL, as EASTL understands object alignment requirements, allows for debug naming, allows for sharing allocators across containers, and allows dynamic allocator assignment.

To create a list container that uses your custom allocator and uses block naming, you can use the following code:

```cpp
list<int> intList(pSomeAllocator, "graphics/intList");

intList.push_back(37);
```

### Consider hash tables instead of maps.

Hash containers (such as hash_map) provide the same interface as associative containers (such as map) but have faster lookup and use less memory. The primary disadvantage relative to associative containers is that hash containers are not sorted.

To make a hash_map (dictionary) of integers to strings, you can use the following code:
```cpp
hash_map<int, const char*> stringTable;

stringTable[37] = "hello";
```

### Consider a vector_map (a.k.a. sorted vector) for unchanging data.

You can improve speed, memory usage, and cache behavior by using a vector_map instead of a map (or vector_set instead of set, etc.). The primary disadvantage of vector_map is that insertions and removal of elements is O(n) instead of O(1). However, if your associative container is not going to be changing much or at all, you can benefit from using a vector_map. Consider calling reserve on the vector_map in order to set the desired capacity up front.

To make a vector_set, you can use the following code:

```cpp
vector_set<int> intSet(16); // Create a vector_set with an initial capacity of 16.

intSet.insert(37);
```

Note that you can use containers other than vector to implement vector_set. Here's how you do it with deque:

```cpp
vector_set<int, less<int>, EASTLAllocatorType, deque<int> > intSet;

intSet.insert(37);
```

### Consider slist instead of list.

An slist is a singly-linked list; it is much like a list except that it can only be traversed in a forward direction and not a backward direction. The benefit is that each node is 4 bytes instead of 8 bytes. This is a small improvement, but if you don't need reverse iteration then it can be an improvement. There's also intrusive_slist as an option.

To make an slist, you can use the following code:

```cpp
slist<int> intSlist;

intSlist.push_front(37);
```

### Avoid redundant end() and size() in loops.

Instead of writing code like this:

```cpp
for(deque<int>::iterator it = d.begin(); it != d.end(); ++it)

    ...
```

write code like this:

```cpp
for(deque<int>::iterator it = d.begin(), itEnd = d.end(); it != itEnd; ++it)

    ...
```

The latter avoids a function call and return of an object (which in deque's case happens to be more than just a pointer). The above only works when the container is unchanged or for containers that have a constant end value. But "constant end value" we mean containers which can be modified but end always remains the same.

| Constant begin | Non-constant begin | Constant end | Non-constant end |
|------|------|------|------|
| array<sup>1</sup> | string<br> vector<br> deque<br> intrusive_list<br> intrusive_slist<br> vector_map<br> vector_multimap<br> vector_set<br> vector_multiset<br> bit_vector<br> hash_map<br> hash_multimap<br> hash_set<br> hash_multiset<br> intrusive_hash_map<br> intrusive_hash_multimap<br> intrusive_hash_set<br> intrusive_hash_multiset | array<br> list<br> slist<br> intrusive_list<br> intrusive_slist<br> map<br> multimap<br> set<br> multiset<br> hash_map<sup>2</sup><br> hash_multimap<sup>2</sup><br> hash_set<sup>2</sup><br> hash_multiset<sup>2</sup><br> intrusive_hash_map<br> intrusive_hash_multimap<br> intrusive_hash_set<br> intrusive_hash_multiset | string<br> vector<br> deque<br> vector_map<br> vector_multimap<br> vector_set<br> vector_multiset<br> bit_vector |

* <sup>1</sup> Arrays can be neither resized nor reallocated.
* <sup>2</sup> Constant end if the hashtable can't/won't re-hash. Non-constant if it can re-hash.

### Iterate containers instead of using operator[].

It's faster to iterate random access containers via iterators than via operator[], though operator[] usage may look simpler.

Instead of doing this:

```cpp
for(unsigned i = 0, iEnd = intVector.size(); i != iEnd; ++i)

    intVector[i] = 37;
```

you can execute more efficiently by doing this:

```cpp
for(vector<int>::iterator it = intVector.begin(), itEnd = intVector.end(); it != itEnd; ++it)

    *it = 37;
```

### Learn to use the string class appropriately.

Oddly enough, the most mis-used STL container is easily the string class. The tales of string abuse could rival the 1001 Arabian Nights. Most of the abuses involve doing things in a harder way than need be. In examining the historical mis-uses of string, it is clear that many of the problems stem from the user thinking in terms of C-style string operations instead of object-oriented strings. This explains why statements such as strlen(s.c_str()) are so common, whereas the user could just use s.length() instead and be both clearer and more efficient.

Here we provide a table of actual collected examples of things done and how they could have been done instead.

| What was written | What could have been written |
|------|------|
| `s = s.Left(i) + '+' + s.Right(s.length() - i - 1);` | `s[i] = '+';` |
| `string s(""); // This is the most commonly found misuse.` | `string s;` |
| `s = "";` | `s.clear();` |
| `s.c_str()[0] = 'u';` | `s[0] = 'u';` |
| `len = strlen(s.c_str());` | `len = s.length();` |
| `s = string("u");` | `s = "u";` |
| `puts(s + string("u"));` | `puts(s + "u");` |
| `string s(" ");`<br> `puts(s.c_str());` | `puts(" ");` |
| `s.sprintf("u");` | s = "u";` |
| `char array[32];`<br> `sprintf(array, "%d", 10);`<br> `s = string(array);` | `s.sprintf("%d", 10);` |

The chances are that if you want to do something with a string, there is a very basic way to do it. You don't want your code to appear in a future version of the above table.

### Cache list size if you want list::size() to be O(1).

EASTL's list, slist, intrusive_list, and intrusive_slist containers have a size() implementation which is O(n). That is, these containers don't keep a count (cache) of the current list size and when you call the size() function they iterate the list. This is by design and the reasoning behind it has been deeply debated and considered (and is discussed in the FAQ and the list header file). In summary, list doesn't cache its size because the only function that would benefit is the size function while many others would be negatively impacted and the memory footprint would be negatively impacted, yet list::size is not a very frequently called function in well-designed code. At the same time, nothing prevents the user from caching the size himself, though admittedly it adds some tedium and risk to the code writing process.

Here's an example of caching the list size manually:

```cpp
list<int> intList;

  size_t    n = 0;



  intList.push_back(37);

  ++n;

  intList.pop_front();

  --n;
```

### Use empty() instead of size() when possible.

All conventional containers have both an empty function and a size function. For all containers empty() executes with O(1) (constant time) efficiency. However, this is not so for size(), as some containers need to calculate the size and others need to do pointer subtraction (which may involve integer division) to find the size.

### Know your container efficiencies.

The above two practices lead us to this practice, which is a generalization of the above. We present a table of basic information for the conventional EASTL containers. The values are described at the bottom.

| Container | empty() efficiency | size() efficiency | operator[] efficiency | insert() efficiency | erase() efficiency | find() efficiency | sort efficiency |
|------|------|------|------|------|------|------|------|
| slist | 1 | O(n) | - | O(1) | O(1) | O(n) | O(n+) |
| list | 1 | n | - | 1 | 1 | n | n log(n) |
| intrusive_slist | 1 | n | - | 1 | 1 | 1 | n+ |
| intrusive_list | 1 | n | - | 1 | 1 | 1 | n log(n) |
| array | 1 | 1 | 1 | - | - | n | n log(n) |
| vector | 1 | 1<sup>a</sup> | 1 | 	1 at end, else n | 1 at end, else n | n | n log(n) |
| vector_set | 1 | 1<sup>a</sup> | 1 | 1 at end, else n | 1 at end, else n | log(n) | 1 |
| vector_multiset | 1 | 1<sup>a</sup> | 1 | 1 at end, else n | 1 at end, else n | log(n) | 1 |
| vector_map | 1 | 1<sup>a</sup> | 1 | 1 at end, else n | 1 at end, else n | log(n) | 1 |
| vector_multimap | 1 | 1<sup>a</sup> | 1 | 1 at end, else n | 1 at end, else n | log(n) | 1 |
| deque | 1 | 1<sup>a</sup> | 1 | 1 at begin or end, else n / 2 | 1 at begin or end, else n / 2 | n | n log(n) |
| bit_vector | 1 | 1<sup>a</sup> | 1 | 1 at end, else n | 1 at end, else n | n | n log(n) |
| string, cow_string  | 1 | 1<sup>a</sup> | 1 | 1 at end, else n | 1 at end, else n | n | n log(n) |
| set | 1 | 1 | - | log(n) | log(n) | log(n) | 1 |
| multiset | 1 | 1 | - | log(n) | log(n) | log(n) | 1 |
| map | 1 | 1 | log(n) | log(n) | log(n) | log(n) | 1 |
| multimap | 1 | 1 | - | log(n) | log(n) | log(n) | 1 |
| hash_set | 1 | 1 | - | 1 | 1 | 1 | - |
| hash_multiset | 1 | 1 | - | 1 | 1 | 1 | - |
| hash_map | 1 | 1 | - | 1 | 1 | 1 | - |
| hash_multimap | 1 | 1 | - | 1 | 1 | 1 | - |
| intrusive_hash_set | 1 | 1 | - | 1 | 1 | 1 | - |
| intrusive_hash_multiset | 1 | 1 | - | 1 | 1 | 1 | - |
| intrusive_hash_map | 1 | 1 | - | 1 | 1 | 1 | - |
| intrusive_hash_multimap | 1 | 1 | - | 1 | 1 | 1 | - |

Notes:

* \- means that the operation does not exist.
* 1 means amortized constant time. Also known as O(1)
* n means time proportional to the container size. Also known as O(n)
* log(n) means time proportional to the natural logarithm of the container size. Also known as O(log(n))
* n log(n) means time proportional to log(n) times the size of the container. Also known as O(n log(n))
* n+ means that the time is at least n, and possibly higher.
* Inserting at the end of a vector may cause the vector to be resized; resizing a vector is O(n). However, the amortized time complexity for vector insertions at the end is constant.
* Sort assumes the usage of the best possible sort for a large container of random data. Some sort algorithms (e.g. quick_sort) require random access iterators and so the sorting of some containers requires a different sort algorithm. We do not include bucket or radix sorts, as they are always O(n).
* <sup>a</sup> vector, deque, string size is O(1) but involves pointer subtraction and thus integer division and so is not as efficient as containers that store the size directly.

### Use vector::reserve.

You can prevent vectors (and strings) from reallocating as you add items by specifying up front how many items you will be requiring. You can do this in the constructor or by calling the reserve function at any time. The capacity function returns the amount of space which is currently reserved.

Here's how you could specify reserved capacity in a vector:

```cpp
vector<Widget> v(37);   // Reserve space to hold up to 37 items.

    or

vector<Widget> v;       // This empty construction causes to memory to be allocated or reserved.

  v.reserve(37);
```

The EASTL vector (and string) implementation looks like this:

```cpp
template <typename T>

  class vector {

    T* mpBegin;     // Beginning of used element memory.

    T* mpEnd;       // End of used element memory.

    T* mpCapacity;  // End of storage capacity. Is >= mpEnd

   }
```

Another approach to being efficient with vector memory usage is to use fixed_vector.

### Use vector::set_capacity to trim memory usage.

A commonly asked question about vectors and strings is, "How do I reduce the capacity of a vector?" The conventional solution for std STL is to use the somewhat non-obvious trick of using vector<Widget>(v).swap(v). EASTL provides the same functionality via a member function called set_capacity() which is present in both the vector and string classes.

An example of reducing a vector is the following:

```cpp
vector<Widget> v;

...

 v.set_capacity();
```

An example of resizing to zero and completely freeing the memory of a vector is the following:

```cpp
vector<Widget> v;

  ...

   v.set_capacity(0);
```

### Use swap() instead of a manually implemented version.

The generic swap algorithm provides a basic version for any kind of object. However, each EASTL container provides a specialization of swap which is optimized for that container. For example, the list container implements swap by simply swapping the internal member pointers and not by moving individual elements.

### Consider storing pointers instead of objects.

There are times when storing pointers to objects is more efficient or useful than storing objects directly in containers. It can be more efficient to store pointers when the objects are big and the container may need to construct, copy, and destruct objects during sorting or resizing. Moving pointers is usually faster than moving objects. It can be useful to store pointers instead of objects when somebody else owns the objects or the objects are in another container. It might be useful for a Widget to be in a list and in a hash table at the same time.

### Consider smart pointers instead of raw pointers.

If you take the above recommendation and store objects as pointers instead of as objects, you may want to consider storing them as smart pointers instead of as regular pointers. This is particularly useful for when you want to delete the object when it is removed from the container. Smart pointers will automatically delete the pointed-to object when the smart pointer is destroyed. Otherwise, you will have to be careful about how you work with the list so that you don't generate memory leaks. Smart pointers implement a shared reference count on the stored pointer, as so any operation you do on a smart pointer container will do the right thing. Any pointer can be stored in a smart pointer, and custom new/delete mechanisms can work with smart pointers. The primary smart pointer is shared_ptr.

Here is an example of creating and using a shared_ptr:

```cpp
typedef shared_ptr<Widget> WPtr;

  list<WPtr> wList;



  wList.push_back(WPtr(new Widget)); // The user may have operator new/delete overrides.

wList.pop_back();                  // Implicitly deletes the Widget.
```

Here is an example of creating and using a shared_ptr that uses a custom allocation and deallocation mechanism:

```cpp
typedef shared_ptr<Widget, EASTLAllocatorType, WidgetDelete> WPtr; // WidgetDelete is a custom destroyer.

  list<WPtr> wList;



  wList.push_back(WPtr(WidgetCreate(Widget))); // WidgetCreate is a custom allocator.

wList.pop_back();                            // Implicitly calls WidgetDelete.
```

### Use iterator pre-increment instead of post-increment.

Pre-increment (e.g. ++x) of iterators is better than post-increment (x++) when the latter is not specifically needed. It is common to find code that uses post-incrementing when it could instead use pre-incrementing; presumably this is due to post-increment looking a little better visually. The problem is that the latter constructs a temporary object before doing the increment. With built-in types such as pointers and integers, the compiler will recognize that the object is a trivial built-in type and that the temporary is not needed, but the compiler cannot do this for other types, even if the compiler sees that the temporary is not used; this is because the constructor may have important side effects and the compiler would be broken if it didn't construct the temporary object.

EASTL iterators are usually not trivial types and so it's best not to hope the compiler will do the best thing. Thus you should always play it safe an use pre-increment of iterators whenever post-increment is not required.

Here is an example of using iterator pre-increment; for loops like this should always use pre-increment:

```cpp
for(set<int>::iterator it(intSet.begin()), itEnd(intSet.end()); it != itEnd; ++it)

      *it = 37;
```

### Make temporary references so the code can be traced/debugged.

Users want to be able to inspect or modify variables which are referenced by iterators. While EASTL containers and iterators are designed to make this easier than other STL implementations, it makes things very easy if the code explicitly declares a reference to the iterated element. In addition to making the variable easier to debug, it also makes code easier to read and makes the debug (and possibly release) version of the application run more efficiently.

Instead of doing this:

```cpp
for(list<Widget>::iterator it = wl.begin(), itEnd = wl.end(); it != itEnd; ++it) {

      (*it).x = 37;

      (*it).y = 38;

      (*it).z = 39;

  }
```

Consider doing this:

```cpp
for(list<Widget>::iterator it = wl.begin(), itEnd = wl.end(); it != itEnd; ++it) {

      Widget& w = *it; // The user can easily inspect or modify w here.

      w.x = 37;

      w.y = 38;

      w.z = 39;

  }
```

### Consider bitvector or bitset instead of vector<bool>.

In EASTL, a vector of bool is exactly that. It intentionally does not attempt to make a specialization which implements a packed bit array. The bitvector class is specifically designed for this purpose. There are arguments either way, but if vector<bool> were allowed to be something other than an array of bool, it would go against user expectations and prevent users from making a true array of bool. There's a mechanism for specifically getting the bit packing, and it is bitvector.

Additionally there is bitset, which is not a conventional iterateable container but instead acts like bit flags. bitset may better suit your needs than bitvector if you need to do flag/bit operations instead of array operations. bitset does have an operator[], though.

### Vectors can be treated as contiguous memory.

EASTL vectors (and strings) guarantee that elements are present in a linear contiguous array. This means that you can use a vector as you would a C-style array by using the vector data() member function or by using &v[0].

To use a vector as a pointer to an array, you can use the following code:

```cpp
struct Widget {

      uint32_t x;

      uint32_t y;

  };



  vector<Widget> v;



  quick_sort((uint64_t*)v.data(), (uint64_t*)(v.data() + v.size()));
```

### Search hash_map<string> via find_as() instead of find().

EASTL hash tables offer a bonus function called find_as when lets you search a hash table by something other than the container type. This is particularly useful for hash tables of string objects that you want to search for by string literals (e.g. "hello") or char pointers. If you search for a string via the find function, your string literal will necessarily be converted to a temporary string object, which is inefficient.

To use find_as, you can use the following code:

```cpp
hash_map<string, int> hashMap;

  hash_map<string, int>::iterator it = hashMap.find_as("hello"); // Using default hash and compare.
```

### Take advantage of type_traits (e.g. EASTL_DECLARE_TRIVIAL_RELOCATE).

EASTL includes a fairly serious type traits library that is on par with the one found in Boost but offers some additional performance-enhancing help as well. The type_traits library provides information about class *types*, as opposed to class instances. For example, the is_integral type trait tells if a type is one of int, short, long, char, uint64_t, etc.

There are three primary uses of type traits:

* Allowing for optimized operations on some data types.
* Allowing for different logic pathways based on data types.
* Allowing for compile-type assertions about data type expectations.

Most of the type traits are automatically detected and implemented by the compiler. However, EASTL allows for the user to explicitly give the compiler hints about type traits that the compiler cannot know, via the EASTL_DECLARE declarations. If the user has a class that is relocatable (i.e. can safely use memcpy to copy values), the user can use the EASTL_DECLARE_TRIVIAL_RELOCATE declaration to tell the compiler that the class can be copied via memcpy. This will automatically significantly speed up some containers and algorithms that use that class.

Here is an example of using type traits to tell if a value is a floating point value or not:

```cpp
template <typename T>

  DoSomething(T t) {

    assert(is_floating_point<T>::value);

  }
```

Here is an example of declaring a class as relocatable and using it in a vector.

```cpp
EASTL_DECLARE_TRIVIAL_RELOCATE(Widget); // Usually you put this at the Widget class declaration.

  vector<Widget> wVector;

  wVector.erase(wVector.begin());         // This operation will be optimized via using memcpy.
```

The following is a full list of the currently recognized type traits. Most of these are implemented as of this writing, but if there is one that is missing, feel free to contact the maintainer of this library and request that it be completed.

* is_void
* is_integral
* is_floating_point
* is_arithmetic
* is_fundamental
* is_const
* is_volatile
* is_abstract
* is_signed
* is_unsigned
* is_array
* is_pointer
* is_reference
* is_member_object_pointer
* is_member_function_pointer
* is_member_pointer
* is_enum
* is_union
* is_class
* is_polymorphic
* is_function
* is_object
* is_scalar
* is_compound
* is_same
* is_convertible
* is_base_of
* is_empty
* is_pod
* is_aligned
* has_trivial_constructor
* has_trivial_copy
* has_trivial_assign
* has_trivial_destructor
* has_trivial_relocate1
* has_nothrow_constructor
* has_nothrow_copy
* has_nothrow_assign
* has_virtual_destructor
* alignment_of
* rank
* extent
*
<sup>1</sup> has_trivial_relocate is not found in Boost nor the C++ standard update proposal. However, it is very useful in allowing for the generation of optimized object moving operations. It is similar to the is_pod type trait, but goes further and allows non-pod classes to be categorized as relocatable. Such categorization is something that no compiler can do, as only the user can know if it is such. Thus EASTL_DECLARE_TRIVIAL_RELOCATE  is provided to allow the user to give the compiler a hint.

### Name containers to track memory usage.

All EASTL containers which allocate memory have a built-in function called set_name and have a constructor argument that lets you specify the container name. This name is used in memory tracking and allows for the categorization and measurement of memory usage. You merely need to supply a name for your containers to use and it does the rest.

Here is an example of creating a list and naming it "collision list":

`list<CollisionData> collisionList(allocator("collision list"));`

or

```cpp
list<CollisionData> collisionList;

collisionList.get_allocator().set_name("collision list");
```

Note that EASTL containers do not copy the name contents but merely copy the name pointer. This is done for simplicity and efficiency. A user can get around this limitation by creating a persistently present string table. Additionally, the user can get around this by declaring static but non-const strings and modifying them at runtime.

### Learn the algorithms.

EASTL algorithms provide a variety of optimized implementations of fundamental algorithms. Many of the EASTL algorithms are the same as the STL algorithm set, though EASTL adds additional algorithms and additional optimizations not found in STL implementations such as Microsoft's. The copy algorithm, for example, will memcpy data types that have the has_trivial_relocate type trait instead of doing an element-by-element copy.

The classifications we use here are not exactly the same as found in the C++ standard; they have been modified to be a little more intuitive. Not all the functions listed here may be yet available in EASTL as you read this. If you want some function then send a request to the maintainer. Detailed documentation for each algorithm is found in algorithm.h or the otherwise corresponding header file for the algorithm.

**Search**

* find, find_if
* find_end
* find_first_of
* adjacent_find
* binary_search
* search, search_n
* lower_bound
* upper_bound
* equal_range

**Sort**

* is_sorted
* quick_sort
* insertion_sort
* shell_sort
* heap_sort
* merge_sort, merge_sort_buffer
* merge
* inplace_merge
* partial_sort
* stable_sort
* partial_sort_copy
* <other sort functions found in the EASTL bonus directories>

**Modifying**

* fill, fill_n
* generate, generate_n
* random_shuffle
* swap
* iter_swap
* swap_ranges
* remove, remove_if
* remove_copy, remove_copy_if
* replace, replace_if
* replace_copy, replace_copy_if
* reverse
* reverse_copy
* rotate
* rotate_copy
* partition
* stable_partition
* transform
* next_permutation
* prev_permutation
* unique
* unique_copy

**Non-Modifying**

* for_each
* copy
* copy_backward
* count, count_if
* equal
* mismatch
* min
* max
* min_element
* max_element
* lexicographical_compare
* nth_element

**Heap**

* is_heap
* make_heap
* push_heap
* pop_heap
* change_heap
* sort_heap
* remove_heap

**Set**

* includes
* set_difference
* set_symmetric_difference
* set_intersection
* set_union

### Pass and return containers by reference instead of value.

If you aren't paying attention you might accidentally write code like this:

```cpp
void DoSomething(list<Widget> widgetList) {

      ...

}
```

The problem with the above is that widgetList is passed by value and not by reference. Thus the a copy of the container is made and passed instead of a reference of the container being passed. This may seem obvious to some but this happens periodically and the compiler gives no warning and the code will often execute properly, but inefficiently. Of course there are some occasions where you really do want to pass values instead of references.

### Consider using reset() for fast container teardown.

EASTL containers have a reset function which unilaterally resets the container to a newly constructed state. The contents of the container are forgotten; no destructors are called and no memory is freed. This is a risky but power function for the purpose of implementing very fast temporary containers. There are numerous cases in high performance programming when you want to create a temporary container out of a scratch buffer area, use the container, and then just "vaporize" it, as it would be waste of time to go through the trouble of clearing the container and destroying and freeing the objects. Such functionality is often used with hash tables or maps and with a stack allocator (a.k.a. linear allocator).

Here's an example of usage of the reset function and a PPMalloc-like StackAllocator:

```cpp
pStackAllocator->push_bookmark();

  hash_set<Widget, less<Widget>, StackAllocator> wSet(pStackAllocator);

<use wSet>

  wSet.reset();

  pStackAllocator->pop_bookmark();
```

### Consider using fixed_substring instead of copying strings.

EASTL provides a fixed_substring class which uses a reference to a character segment instead of allocating its own string memory. This can be a more efficient way to work with strings under some circumstances.

Here's an example of usage of fixed_substring:

```cpp
basic_string<char> str("hello world");

  fixed_substring<char> sub(str, 6, 5); // sub == "world"

fixed_substring can refer to any character array and not just one that derives from a string object.
```

### Consider using vector::push_back(void).

EASTL provides an alternative way to insert elements into containers that avoids copy construction and/or the creation of temporaries. Consider the following code:

```cpp
vector<Widget> widgetArray;

  widgetArray.push_back(Widget());
```

The standard vector push_back function requires you to supply an object to copy from. This incurs the cost of the creation of a temporary and for some types of classes or situations this cost may be undesirable. It additionally requires that your contained class support copy-construction whereas you may not be able to support copy construction. As an alternative, EASTL provides a push_back(void) function which requires nothing to copy from but instead constructs the object in place in the container. So you can do this:

```cpp
vector<Widget> widgetArray;

  widgetArray.push_back();

widgetArray.back().x = 0; // Example of how to reference the new object.
```

Other containers with such copy-less functions include:

```cpp
vector::push_back()

  deque::push_back()

  deque::push_front()

  list::push_back()

  list::push_front()

  slist::push_front()

  map::insert(const key_type& key)

  multimap::insert(const key_type& key)

  hash_map::insert(const key_type& key)

  hash_multimap::insert(const key_type& key)
```

Note that the map functions above allow you to insert a default value specified by key alone and not a value_type like with the other map insert functions.

----------------------------------------------
End of document
