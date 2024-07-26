# EASTL FAQ

We provide a FAQ (frequently asked questions) list here for a number of commonly asked questions about EASTL and STL in general. Feel free to suggest new FAQ additions based on your own experience.

## Information

1. [What is EASTL?](#info1-what-is-eastl)
2. [What uses are EASTL suitable for?](#info2-what-uses-are-eastl-suitable-for)
3. [How does EASTL differ from standard C++ STL?](#info3-how-does-eastl-differ-from-standard-c-stl)
4. [Is EASTL thread-safe?](#info4-is-eastl-thread-safe)
5. [What platforms/compilers does EASTL support?](#info5-what-platformscompilers-does-eastl-support)
6. [Why is there EASTL when there is the STL?](#info6-why-is-there-eastl-when-there-is-the-stl)
7. [Can I mix EASTL with standard C++ STL?](#info7-can-i-mix-eastl-with-standard-c-stl)
8. [Where can I learn more about STL and EASTL?](#info8-where-can-i-learn-more-about-stl-and-eastl)
9. [What is the legal status of EASTL?](#info9-what-is-the-legal-status-of-eastl)
10. [Does EASTL deal with compiler exception handling settings?](#info10-does-eastl-deal-with-compiler-exception-handling-settings)
11. [What C++ language features does EASTL use (e.g. virtual functions)?](#info11-what-c-language-features-does-eastl-use-eg-virtual-functions)
12. [What compiler warning levels does EASTL support?](#info12-what-compiler-warning-levels-does-eastl-support)
13. [Is EASTL compatible with Lint?](#info13-is-eastl-compatible-with-lint)
14. [What compiler settings do I need to compile EASTL?](#info14-what-compiler-settings-do-i-need-to-compile-eastl)
15. [How hard is it to incorporate EASTL into my project?](#info15-how-hard-is-it-to-incorporate-eastl-into-my-project)
16. [Should I use EASTL instead of std STL or instead of my custom library?](#info16-should-i-use-eastl-instead-of-std-stl-or-instead-of-my-custom-library)
17. [I think I've found a bug. What do I do?](#info17-i-think-ive-found-a-bug-what-do-i-do)
18. [Can EASTL be used by third party EA developers?](#info18-can-eastl-be-used-by-third-party-ea-developers)

## Performance

1. [How efficient is EASTL compared to standard C++ STL implementations?](#perf1-how-efficient-is-eastl-compared-to-standard-c-stl-implementations)
2. [How efficient is EASTL in general?](#perf2-how-efficient-is-eastl-in-general)
3. [Strings don't appear to use the "copy-on-write" optimization. Why not?](#perf3-strings-dont-appear-to-use-the-copy-on-write-cow-optimization-why-not)
4. [Does EASTL cause code bloat, given that it uses templates?](#perf4-does-eastl-cause-code-bloat-given-that-it-uses-templates)
5. [Don't STL and EASTL containers fragment memory?](#perf5-dont-stl-and-eastl-containers-fragment-memory)
6. [I don't see container optimizations for equivalent scalar types such as pointer types. Why?](#perf6-i-dont-see-container-optimizations-for-equivalent-scalar-types-such-as-pointer-types-why)
7. [I've seen some STL's provide a default quick "node allocator" as the default allocator. Why doesn't EASTL do this?](#perf7-ive-seen-some-stls-provide-a-default-quick-node-allocator-as-the-default-allocator-why-doesnt-eastl-do-this)
8. [Templates sometimes seem to take a long time to compile. Why do I do about that?](#perf8-templates-sometimes-seem-to-take-a-long-time-to-compile-why-do-i-do-about-that)
9. [How do I assign a custom allocator to an EASTL container?](#cont8-how-do-i-assign-a-custom-allocator-to-an-eastl-container)
10. [How well does EASTL inline?](#perf10-how-well-does-eastl-inline)
11. [How do I control function inlining?](#perf11-how-do-i-control-function-inlining)
12. [C++ / EASTL seems to bloat my .obj files much more than C does.](#perf12-c--eastl-seems-to-bloat-my-obj-files-much-more-than-c-does)
13. [What are the best compiler settings for EASTL?](#perf13-what-are-the-best-compiler-settings-for-eastl)

## Problems

1. [I'm getting screwy behavior in sorting algorithms or sorted containers. What's wrong?](#prob1-im-getting-screwy-behavior-in-sorting-algorithms-or-sorted-containers-whats-wrong)
2. [I am getting compiler warnings (e.g. C4244, C4242 or C4267) that make no sense. Why?](#prob2-i-am-getting-compiler-warnings-eg-c4244-c4242-or-c4267-that-make-no-sense-why)
3. [I am getting compiler warning C4530, which complains about exception handling and "unwind semantics." What gives?](#prob3-i-am-getting-compiler-warning-c4530-which-complains-about-exception-handling-and-unwind-semantics-what-gives)
4. [Why are tree-based containers hard to read with a debugger?](#prob4-why-are-tree-based-eastl-containers-hard-to-read-with-a-debugger)
5. [The EASTL source code is sometimes rather complicated looking. Why is that?](#prob5-the-eastl-source-code-is-sometimes-rather-complicated-looking-why-is-that)
6. [When I get compilation errors, they are very long and complicated looking. What do I do?](#prob6-when-i-get-compilation-errors-they-are-very-long-and-complicated-looking-what-do-i-do)
7. [Templates sometimes seem to take a long time to compile. Why do I do about that?](#prob7-templates-sometimes-seem-to-take-a-long-time-to-compile-why-do-i-do-about-that)
8. [I get the compiler error: "template instantiation depth exceeds maximum of 17. use -ftemplate-depth-NN to increase the maximum"](#prob8-i-get-the-compiler-error-template-instantiation-depth-exceeds-maximum-of-17-use--ftemplate-depth-nn-to-increase-the-maximum)
9. [I'm getting errors about min and max while compiling.](#prob9-im-getting-errors-about-min-and-max-while-compiling)
10. [C++ / EASTL seems to bloat my .obj files much more than C does.](#prob10-c--eastl-seems-to-bloat-my-obj-files-much-more-than-c-does)
11. [I'm getting compiler errors regarding operator new being previously defined.](#prob11-im-getting-compiler-errors-regarding-placement-operator-new-being-previously-defined)
12. [I'm getting errors related to wchar_t string  functions such as wcslen().](#prob12-im-getting-errors-related-to-wchar_t-string--functions-such-as-wcslen)
13. [I'm getting compiler warning C4619: there is no warning number Cxxxx (e.g. C4217).](#prob13-im-getting-compiler-warning-c4619-there-is-no-warning-number-cxxxx-eg-c4217)
14. [My stack-based fixed_vector is not respecting the object alignment requirements.](#prob14-my-stack-based-fixed_vector-is-not-respecting-the-object-alignment-requirements)
15. [I am getting compiler errors when using GCC under XCode (Macintosh/iphone).](#prob15-i-am-getting-compiler-errors-when-using-gcc-under-xcode-macintoshiphone)
16. [I am getting linker errors about Vsnprintf8 or Vsnprintf16.](#prob16-i-am-getting-linker-errors-about-vsnprintf8-or-vsnprintf16)
17. [I am getting compiler errors about UINT64_C or UINT32_C.](#prob17-i-am-getting-compiler-errors-about-uint64_c-or-uint32_c)
18. [I am getting a crash with a global EASTL container.](#prob18-i-am-getting-a-crash-with-a-global-eastl-container)
19. [Why doesn't EASTL support passing NULL to functions with pointer arguments?](#prob19-why-doesnt-eastl-support-passing-null-string-functions)

## Debug

1. [How do I get VC++ mouse-overs to view templated data?](#debug1-how-do-i-set-the-vc-debugger-to-display-eastl-container-data-with-tooltips)
2. [How do I view containers if the visualizer/tooltip support is not present?](#debug2-how-do-i-view-containers-if-the-visualizertooltip-support-is-not-present)
3. [The EASTL source code is sometimes rather complicated looking. Why is that?](#debug3-the-eastl-source-code-is-sometimes-rather-complicated-looking-why-is-that)
4. [When I get compilation errors, they are very long and complicated looking. What do I do?](#debug4-when-i-get-compilation-errors-they-are-very-long-and-complicated-looking-what-do-i-do)
5. [How do I measure hash table balancing?](#debug5-how-do-i-measure-hash-table-balancing)

## Containers

1. [Why do some containers have "fixed" versions (e.g. fixed_list) but others(e.g. deque) don't have fixed versions?](#cont1-why-do-some-containers-have-fixed-versions-eg-fixed_list-but-otherseg-deque-dont-have-fixed-versions)
2. [Can I mix EASTL with standard C++ STL?](#cont2-can-i-mix-eastl-with-standard-c-stl)
3. [Why are there so many containers?](#cont3-why-are-there-so-many-containers)
4. [Don't STL and EASTL containers fragment memory?](#cont4-dont-stl-and-eastl-containers-fragment-memory)
5. [I don't see container optimizations for equivalent scalar types such as pointer types. Why?](#cont5-i-dont-see-container-optimizations-for-equivalent-scalar-types-such-as-pointer-types-why)
6. [What about alternative container and algorithm implementations (e.g. treaps, skip lists, avl trees)?](#cont6-what-about-alternative-container-and-algorithm-implementations-eg-treaps-skip-lists-avl-trees)
7. [Why are containers hard to read with a debugger?](#cont7-why-are-tree-based-eastl-containers-hard-to-read-with-a-debugger)
8. [How do I assign a custom allocator to an EASTL container?](#cont8-how-do-i-assign-a-custom-allocator-to-an-eastl-container)
9. [How do I set the VC++ debugger to display EASTL container data with tooltips?](#cont9-how-do-i-set-the-vc-debugger-to-display-eastl-container-data-with-tooltips)
10. [How do I use a memory pool with a container?](#cont10-how-do-i-use-a-memory-pool-with-a-container)
11. [How do I write a comparison (operator<()) for a struct that contains two or more members?](#cont11-how-do-i-write-a-comparison-operator-for-a-struct-that-contains-two-or-more-members)
12. [Why doesn't container X have member function Y?](#cont12-why-doesnt-container-x-have-member-function-y)
13. [How do I search a hash_map of strings via a char pointer efficiently? If I use map.find("hello") it creates a temporary string, which is inefficient.](#cont13-how-do-i-search-a-hash_map-of-strings-via-a-char-pointer-efficiently-if-i-use-mapfindhello-it-creates-a-temporary-string-which-is-inefficient)
14. [Why are set and hash_set iterators const (i.e. const_iterator)?](#cont14-why-are-set-and-hash_set-iterators-const-ie-const_iterator)
15. [How do I prevent my hash container from re-hashing?](#cont15-how-do-i-prevent-my-hash-container-from-re-hashing)
16. [Which uses less memory, a map or a hash_map?](#cont16-which-uses-less-memory-a-map-or-a-hash_map)
17. [How do I write a custom hash function?](#cont17-how-do-i-write-a-custom-hash-function)
18. [How do I write a custom compare function for a map or set?](#cont18-how-do-i-write-a-custom-compare-function-for-a-map-or-set)
19. [How do I force my vector or string capacity down to the size of the container?](#cont19-how-do-i-force-my-vector-or-string-capacity-down-to-the-size-of-the-container)
20. [How do I iterate a container while (selectively) removing items from it?](#cont20-how-do-i-iterate-a-container-while-selectively-removing-items-from-it)
21. [How do I store a pointer in a container?](#cont21-how-do-i-store-a-pointer-in-a-container)
22. [How do I make a union of two containers? difference? intersection?](#cont22-how-do-i-make-a-union-of-two-containers-difference-intersection)
23. [How do I override the default global allocator?](#cont23-how-do-i-override-the-default-global-allocator)
24. [How do I do trick X with the string container?](#cont24-how-do-i-do-trick-x-with-the-string-container)
25. [How do EASTL smart pointers compare to Boost smart pointers?](#cont25-how-do-eastl-smart-pointers-compare-to-boost-smart-pointers)
26. [How do your forward-declare an EASTL container?](#cont26-how-do-your-forward-declare-an-eastl-container)
27. [How do I make two containers share a memory pool?](#cont27-how-do-i-make-two-containers-share-a-memory-pool)
28. [Can I use a std (STL) allocator with EASTL?](#cont28-can-i-use-a-std-stl-allocator-with-eastl)
29. [What are the requirements of classes stored in containers?](#what-are-the-requirements-of-classes-stored-in-containers)

## Algorithms

1. [I'm getting screwy behavior in sorting algorithms or sorted containers. What's wrong?](#algo1-im-getting-screwy-behavior-in-sorting-algorithms-or-sorted-containers-whats-wrong)
2. [How do I write a comparison (operator<()) for a struct that contains two or more members?](#algo2-how-do-i-write-a-comparison-operator-for-a-struct-that-contains-two-or-more-members)
3. [How do I sort something in reverse order?](#algo3-how-do-i-sort-something-in-reverse-order)
4. [I'm getting errors about min and max while compiling.](#algo4-im-getting-errors-about-min-and-max-while-compiling)
5. [Why don't algorithms take a container as an argument instead of iterators? A container would be more convenient.](#algo5-why-dont-algorithms-take-a-container-as-an-argument-instead-of-iterators-a-container-would-be-more-convenient)
6. [Given a container of pointers, how do I find an element by value (instead of by pointer)?](#algo6-given-a-container-of-pointers-how-do-i-find-an-element-by-value-instead-of-by-pointer)
7. [When do stored objects need to support opertor < vs. when do they need to support operator ==?](#algo7-when-do-stored-objects-need-to-support-operator--vs-when-do-they-need-to-support-operator-)
8. [How do I sort via pointers or array indexes instead of objects directly?](#algo8-how-do-i-sort-via-pointers-or-array-indexes-instead-of-objects-directly)

## Iterators

1. [What's the difference between iterator, const iterator, and const_iterator?](#iter1-whats-the-difference-between-iterator-const-iterator-and-const_iterator)
2. [How do I tell from an iterator what type of thing it is iterating?](#iter2-how-do-i-tell-from-an-iterator-what-type-of-thing-it-is-iterating)
3. [How do I iterate a container while (selectively) removing items from it?](#iter3-how-do-i-iterate-a-container-while-selectively-removing-items-from-it)
4. [What is an insert_iterator?](#iter4-what-is-an-insert_iterator)

## Information

### Info.1 What is EASTL?

EASTL refers to "EA Standard Template Library." It is a C++ template library that is analogous to the template facilities of the C++ standard library, which are often referred to as the STL. EASTL consists of the following systems:

* Containers
* Iterators
* Algorithms
* Utilities
* Smart pointers
* Type traits

Of these, the last two (smart pointers and type traits) do not have analogs in standard C++. With respect to the other items, EASTL provides extensions and optimizations over the equivalents in standard C++ STL.

EASTL is a professional-level implementation which outperforms commercial implementations (where functionality overlaps) and is significantly easier to read and debug.

### Info.2 What uses are EASTL suitable for?

EASTL is suitable for any place where templated containers and algorithms would be appropriate. Thus any C++ tools could use it and many C++ game runtimes could use it, especially 2005+ generation game platforms. EASTL has optimizations that make it more suited to the CPUs and memory systems found on console platforms. Additionally, EASTL has some type-traits and iterator-traits-derived template optimizations that make it generally more efficient than home-brew templated containers.

### Info.3 How does EASTL differ from standard C++ STL?

There are three kinds of ways that EASTL differs from standard STL:

* EASTL equivalents to STL sometimes differ.
* EASTL implementations sometimes differ from STL implementations of the same thing.
* EASTL has functionality that doesn't exist in STL.

With respect to item #1, the changes are such that they benefit game development and and not the type that could silently hurt you if you were more familiar with STL interfaces.

With respect to item #2, where EASTL implementations differ from STL implementations it is almost always due to improvements being made in the EASTL versions or tradeoffs being made which are considered better for game development.

With respect to item #3, there are a number of facilities that EASTL has that STL doesn't have, such as intrusive_list and slist containers, smart pointers, and type traits. All of these are facilities that assist in making more efficient game code and data.

Ways in which EASTL is better than standard STL:

* Has higher performance in release builds, sometimes dramatically so.
* Has significantly higher performance in debug builds, due to less call overhead.
* Has extended per-container functionality, particularly for game development.
* Has additional containers that are useful for high performance game development.
* Is easier to read, trace, and debug.
* Memory allocation is much simpler and more controllable.
* Has higher portability, as there is a single implementation for all platforms.
* Has support of object alignment, whereas such functionality is not natively supported by STL.
* We have control over it, so we can modify it as we like.
* Has stricter standards for container design and behavior, particularly as this benefits game development.

Ways in which EASTL is worse than standard STL:

* Standard STL implementations are currently very reliable and weather-worn, whereas EASTL is less tested.
* Standard STL is automatically available with just about every C++ compiler vendor's library.
* Standard STL is supported by the compiler vendor and somewhat by the Internet community.

#### EASTL coverage of std STL

* list
* vector
* deque
* string
* set
* multiset
* map
* multimap
* bitset
* queue
* stack
* priority_queue
* memory
* numeric
* algorithm (all but inplace_merge, prev_permutation, next_permutation, nth_element, includes, unique_copy)
* utility
* functional
* iterator

EASTL additions/amendments to std STL

* allocators work in a simpler way.
* exception handling can be disabled.
* all containers expose/declare their node size, so you can make a node allocator for them.
* all containers have reset(), which unilaterally forgets their contents.
* all containers have validate() and validate_iterator() functions.
* all containers understand and respect object alignment requirements.
* all containers guarantee no memory allocation upon being newly created as empty.
* all containers and their iterators can be viewed in a debugger (no other STL does this, believe it or not).
* linear containers guarantee linear memory.
* vector has push_back(void).
* vector has a data() function.
* vector<bool> is actually a vector of type bool.
* vector and string have set_capacity().
* string has sprintf(), append_sprintf(), trim(), compare_i(), make_lower(), make_upper().
* deque allows you to specify the subarray size.
* list has a push_back(void) and push_back(void) function.
* hash_map, hash_set, etc. have find_as().

EASTL coverage of TR1 (tr1 refers to proposed additions for the next C++ standard library, ~2008)

* array
* type_traits (there are about 30 of these)
* unordered_set (EASTL calls it hash_set)
* unordered_multiset
* unordered_map
* unordered_multimap
* shared_ptr, shared_array, weak_ptr, scoped_ptr, scoped_array, intrusive_ptr

EASTL additional functionality (not found elsewhere)

* fixed_list
* fixed_slist
* fixed_vector
* fixed_string
* fixed_substring
* fixed_set
* fixed_multiset
* fixed_map
* fixed_multimap
* fixed_hash_set
* fixed_hash_multiset
* fixed_hash_map
* fixed_hash_multimap
* vector_set
* vector_multiset
* vector_map
* vector_multimap
* intrusive_list
* intrusive_slist
* intrusive_sdlist
* intrusive_hash_set
* intrusive_hash_multiset
* intrusive_hash_map
* intrusive_hash_multimap
* slist (STLPort's STL has this)
* heap
* linked_ptr, linked_array
* sparse_matrix (this is not complete as of this writing)
* ring_buffer
* compressed_pair
* call_traits
* binary_search_i, change_heap, find_first_not_of, find_last_of, find_last_not_of, identical
* comb_sort, bubble_sort, selection_sort, shaker_sort, bucket_sort
* equal_to_2, not_equal_to_2, str_equal_to, str_equal_to_i

### Info.4 Is EASTL thread-safe?

It's not simple enough to simply say that EASTL is thread-safe or thread-unsafe. However, we can say that with respect to thread safety that EASTL does the right thing.

Individual EASTL containers are not thread-safe. That is, access to an instance of a container from multiple threads at the same time is unsafe if any of those accesses are modifying operations. A given container can be read from multiple threads simultaneously as well as any other standalone data structure. If a user wants to be able to have modifying access an instance of a container from multiple threads, it is up to the user to ensure that proper thread synchronization occurs. This usually means using a mutex.

EASTL classes other than containers are the same as containers with respect to thread safety. EASTL functions (e.g. algorithms) are inherently thread-safe as they have no instance data and operate entirely on the stack. As of this writing, no EASTL function allocates memory and thus doesn't bring thread safety issues via that means.

The user may well need to be concerned about thread safety with respect to memory allocation. If the user modifies containers from multiple threads, then allocators are going to be accessed from multiple threads. If an allocator is shared across multiple container instances (of the same type of container or not), then mutexes (as discussed above) the user uses to protect access to individual instances will not suffice to provide thread safety for allocators used across multiple instances. The conventional solution here is to use a mutex within the allocator if it is expected to be used by multiple threads.

EASTL uses neither static nor global variables and thus there are no inter-instance dependencies that would make thread safety difficult for the user to implement.

### Info.5 What platforms/compilers does EASTL support?

EASTL's support depends entirely on the compiler and not on the platform. EASTL works on any C++ compiler that completely conforms the C++ language standard. Additionally, EASTL is 32 bit and 64 bit compatible. Since EASTL does not use the C or C++ standard library (with a couple small exceptions), it doesn't matter what kind of libraries are provided (or not provided) by the compiler vendor. However, given that we need to work with some compilers that aren't 100% conforming to the language standard, it will be useful to make a list here of these that are supported and those that are not:

| Compiler | Status | Notes |
|---------|--------|-------|
| GCC 2.9x | Supported | However, GCC 2.9x has some issues that you may run into that cause you to use EASTL facilities differently than a fully compliant compiler would allow. |
| GCC 3.x+ | Supported | This compiler is used by the Mac OSX, and Linux platforms. |
| MSVC 6.0 | Not supported | This compiler is too weak in the area of template and namespace support. |
| MSVC 7.0+ | Supported | This compiler is used by the PC and Win CE platforms |
| Borland 5.5+ | Not supported | Borland can successfully compile many parts of EASTL, but not all parts. |
| EDG | Supported | This is the compiler front end to some other compilers, such as Intel, and Comeau C++. |
| IBM XL 5.0+ | Supported | This compiler is sometimes used by PowerPC platforms such as Mac OSX and possibly future console platforms. |

### Info.6 Why is there EASTL when there is the STL?

The STL is largely a fine library for general purpose C++. However, we can improve upon it for our uses and gain other advantages as well. The primary motivations for the existence of EASTL are the following:

* Some STL implementations (especially Microsoft STL) have inferior performance characteristics that make them unsuitable for game development. EASTL is faster than all existing STL implementations.
* The STL is sometimes hard to debug, as most STL implementations use cryptic variable names and unusual data structures.
* STL allocators are sometimes painful to work with, as they have many requirements and cannot be modified once bound to a container.
* The STL includes excess functionality that can lead to larger code than desirable. It's not very easy to tell programmers they shouldn't use that functionality.
* The STL is implemented with very deep function calls. This results is unacceptable performance in non-optimized builds and sometimes in optimized builds as well.
* The STL doesn't support alignment of contained objects.
* STL containers won't let you insert an entry into a container without supplying an entry to copy from. This can be inefficient.
* Useful STL extensions (e.g. slist, hash_map, shared_ptr) found in existing STL implementations such as STLPort are not portable because they don't exist in other versions of STL or aren't consistent between STL versions.
* The STL lacks useful extensions that game programmers find useful (e.g. intrusive_list) but which could be best optimized in a portable STL environment.
* The STL has specifications that limit our ability to use it efficiently. For example, STL vectors are not guaranteed to use contiguous memory and so cannot be safely used as an array.
* The STL puts an emphasis on correctness before performance, whereas sometimes you can get significant performance gains by making things less academcially pure.
* STL containers have private implementations that don't allow you to work with their data in a portable way, yet sometimes this is an important thing to be able to do (e.g. node pools).
* All existing versions of STL allocate memory in empty versions of at least some of their containers. This is not ideal and prevents optimizations such as container memory resets that can greatly increase performance in some situations.
* The STL is slow to compile, as most modern STL implementations are very large.
* There are legal issues that make it hard for us to freely use portable STL implementations such as STLPort.
* We have no say in the design and implementation of the STL and so are unable to change it to work for our needs.
* Note that there isn't actually anything in the C++ standard called "STL." STL is a term that merely refers to the templated portion of the C++ standard library.

### Info.7 Can I mix EASTL with standard C++ STL?

This is possible to some degree, though the extent depends on the implementation of C++ STL. One of things that makes interoperability is something called iterator categories. Containers and algorithms recognize iterator types via their category and STL iterator categories are not recognized by EASTL and vice versa.

Things that you definitely can do:

* #include both EASTL and standard STL headers from the same .cpp file.
* Use EASTL containers to hold STL containers.
* Construct an STL reverse_iterator from an EASTL iterator.
* Construct an EASTL reverse_iterator from an STL iterator.

Things that you probably will be able to do, though a given std STL implementation may prevent it:

* Use STL containers in EASTL algorithms.
* Use EASTL containers in STL algorithms.
* Construct or assign to an STL container via iterators into an EASTL container.
* Construct or assign to an EASTL container via iterators into an STL container.
*
Things that you would be able to do if the given std STL implementation is bug-free:

* Use STL containers to hold EASTL containers. Unfortunately, VC7.x STL has a confirmed bug that prevents this. Similarly, STLPort versions prior to v5 have a similar but.

Things that you definitely can't do:

* Use an STL allocator directly with an EASTL container (though you can use one indirectly).
* Use an EASTL allocator directly with an STL container (though you can use one indirectly).

### Info.8 Where can I learn more about STL and EASTL?

EASTL is close enough in philosophy and functionality to standard C++ STL that most of what you read about STL applies to EASTL. This is particularly useful with respect to container specifications. It would take a lot of work to document EASTL containers and algorithms in fine detail, whereas most standard STL documentation applies as-is to EASTL. We won't cover the differences here, as that's found in another FAQ entry.

That being said, we provide a list of sources for STL documentation that may be useful to you, especially if you are less familiar with the concepts of STL and template programming in general.

* The SGI STL web site. Includes a good STL reference.
* CodeProject STL introduction.
* Scott Meyers Effective STL book.
* The Microsoft online STL documentation. Microsoft links go bad every couple months, so try searching for STL at the * Microsoft MSDN site.
* The Dinkumware online STL documentation.
* The C++ standard, which is fairly readable. You can buy an electronic version for about $18 and in the meantime you can make do with draft revisions of it off the Internet by searching for "c++ draft standard".
* STL performance tips, by Pete Isensee
* STL algorithms vs. hand-written loops, by Scott Meyers.

### Info.9 What is the legal status of EASTL?

EASTL is usable for all uses within Electronic Arts, both for internal usage and for shipping products for all platforms. All source code was written by a single EA engineer. Any externally derived code would be explicitly stated as such and approved by the legal department if such code ever gets introduced. As of EASTL v1.0, the red_black_tree.cpp file contains two functions derived from the original HP STL and have received EA legal approval for usage in any product.

### Info.10 Does EASTL deal with compiler exception handling settings?

EASTL has automatic knowledge of the compiler's enabling/disabling of exceptions. If your compiler is set to disable exceptions, EASTL automatically detects so and executes without them. Also, you can force-enable or force-disable that setting to override the automatic behavior by #defining EASTL_EXCEPTIONS_ENABLED to 0 or 1. See EASTL's config.h for more information.

### Info.11 What C++ language features does EASTL use (e.g. virtual functions)?

EASTL uses the following C++ language features:

* Template functions, classes, member functions.
* Multiple inheritance.
* Namespaces.
* Operator overloading.

EASTL does not use the following C++ language features:

* Virtual functions / interfaces.
* RTTI (dynamic_cast).
* Global and static variables. There are a couple class static const variables, but they act much like enums.
* Volatile declarations
* Template export.
* Virtual inheritance.

EASTL may use the following C++ language features:

* Try/catch. This is an option that the user can enable and it defaults to whatever the compiler is set to use.
* Floating point math. Hash containers have one floating point calculation, but otherwise floating point is not used.

Notes:

* EASTL uses rather little of the standard C or C++ library and uses none of the C++ template library (STL) and iostream library. The memcpy family of functions is one example EASTL C++ library usage.
* EASTL never uses global new / delete / malloc / free. All allocations are done via user-specified allocators, though a default allocator definition is available.


### Info.12 What compiler warning levels does EASTL support?

For VC++ EASTL should compile without warnings on level 4, and should compile without warnings for "warnings disabled by default" except C4242, C4514, C4710, C4786, and C4820. These latter warnings are somewhat draconian and most EA projects have little choice but to leave them disabled.

For GCC, EASTL should compile without warnings with -Wall. Extensive testing beyond that hasn't been done.

However, due to the nature of templated code generation and due to the way compilers compile templates, unforeseen warnings may occur in user code that may or may not be addressible by modifying EASTL.

### Info.13 Is EASTL compatible with Lint?

As of EASTL 1.0, minimal lint testing has occurred. Testing with the November 2005 release of Lint (8.00t) demonstrated bugs in Lint that made its analysisnot very useful. For example, Lint seems to get confused about the C++ typename keyword and spews many errors with code that uses it. We will work with the makers of Lint to get this resolved so that Lint can provide useful information about EASTL.

### Info.14 What compiler settings do I need to compile EASTL?

EASTL consists mostly of header files with templated C++ code, but there are also a few .cpp files that need to be compiled and linked in order to use some of the modules. EASTL will compile in just about any environment. As mentioned elsewhere in this FAQ, EASTL can be compiled at the highest warning level of most compilers, transparently deals with compiler exception handling settings, is savvy to most or all compilation language options (e.g. wchar_t is built-in or not, for loop variables are local or not), and has almost no platform-specific or compiler-specific code. For the most part, you can just drop it in and it will work. The primary thing that needs to be in place is that EASTL .cpp files need to be compiled with the same struct padding/alignment settings as other code in the project. This of course is the same for just about any C++ source code library.

See the Performance section of this FAQ for a discussion of the optimal compiler settings for EASTL performance.

### Info.15 How hard is it to incorporate EASTL into my project?

It's probably trivial.

EASTL has only one dependency: EABase. And EASTL auto-configures itself for most compiler environments and for the most typical configuration choices. Since it is fairly highly warning-free, you won't likely need to modify your compiler warning settings, even if they're pretty stict. EASTL has a few .cpp files which need to be compiled if you want to use the modules associated with those files. You can just compile those files with your regular compiler settings. Alternatively, you can use one of the EASTL project files.

In its default configuration, the only thing you need to provide to make EASTL work is to define implementations of the following operator new functions:

```cpp
#include <new>

void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line);
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line);
```
The flags and debugFlags arguments correspond to PPMalloc/RenderWare GeneralAllocator/GeneralAllocatorDebug Malloc equivalents.

### Info.16 Should I use EASTL instead of std STL or instead of my custom library?

There are reasons you may want to use EASTL; there are reasons you may not want to use it. Ditto for std STL or any other library. Here we present a list of reasons (+ and -) for why you might want to use one or another. However, it should be noted that while EASTL contains functionality found in std STL, it has another ~40% of functionality not found in std STL, so EASTL and std STL (and whatever other template library you may have) are not mutually exclusive.

**EASTL**
* \+ Has higher performance than any commercial STL, especially on console platforms.
* \+ Has extended functionality tailored for game development.
* \+ Is highly configurable, and we own it so it can be amended at will. Std STL is owned by a third party committee.
* \+ Is much easier to read and debug than other similar libraries, especiallly std STL.


* \- Is highly unit tested, but does not have the same level as std STL.
* \- Is more complicated than many users' lite template libraries, and may put off some beginners.
* \- EASTL  

**Std STL**

* \+ Is highly portable; your STL code will likely compile and run anywhere.
* \+ Works without the need to install or download any package to use it. It just works.
* \+ Is highly reliable and supported by the compiler vendor. You can have confidence in it.
* \+ Some std STL versions (e.g. STLPort, VC8 STL) have better runtime debug checking than EASTL.


* \- Has (sometimes greatly) variable implementations, behavior, and performance between implementations.
* \- Is usually hard to read and debug.
* \- Doesn't support some of the needs of game development, such as aligned allocations, named allocations, intrusive containers, etc.
* \- Is not as efficient as EASTL, especially on console platforms.

**Your own library**
(please forgive us for implying there may be weaknesses in your libraries)

* \+ You have control over it and can make it work however you want.
* \+ You can fix bugs in it on the spot and have the fix in your codebase immediately.
* \+ Your own library can be highly integrated into your application code or development environment.


* \- Many custom libraries don't have the same level of testing as libraries such as std STL or EASTL.
* \- Many custom libraries don't have the same breadth or depth as std STL or especially EASTL.
* \- Many custom libraries don't have the level of performance tuning that std STL or especially EASTL has.

### Info.17 I think I've found a bug. What do I do?

**Verify that you indeed have a bug**

There are various levels of bugs that can occur, which include the following:

* Compiler warnings generated by EASTL.
* Compiler errors generated by EASTL (failure to compile well-formed code).
* Runtime misbehavior by EASTL (function does the wrong thing).
* Runtime crash or data corruption by EASTL.
* Mismatch between EASTL documentation and behavior.
* Mismatch between EASTL behavior and user's expections (mis-design).

Any of the above items can be the fault of EASTL. However, the first four can also be the fault of the user. Your primary goal in verifying a potential bug is to determine if it is an EASTL bug or a user bug. Template errors can sometimes be hard to diagnose. It's probably best if you first show the problem to somebody you know to make sure you are not missing something obvious. Creating a reproducible case may be useful in helping convince yourself, but as is mentioned below, this is not required in order to report the bug.

**Report the bug**

The first place to try is the standard EA centralized tech support site. As of this writing (10/2005), that tech site is http://eatech/. Due to the frequent technology churn that seems to occur within Electronic Arts, the bug reporting system in place when you read this may not be the one that was in place when this FAQ entry was written. If the tech site route fails, consider directly contacting the maintainer of the EASTL package.

In reporting a bug, it is nice if there is a simple reproducible case that can be presented. However, such a case requires time to create, and so you are welcome to initially simply state what you think the bug is without producing a simple reproducible case. It may be that this is a known bug or it may be possible to diagnose the bug without a reproducible case. If more information is needed then the step of trying to produce a reproducible case may be necessary.

### Info.18 Can EASTL be used by third party EA developers?

EASTL and other core technologies authored by EA (and not licensed from other companies) can be used in source and binary form by designated 3rd parties. The primary case where there is an issue is if the library contains platform specific code for a platform that the 3rd party is not licensed for. In that case the platform-specific code would need to be removed. This doesn’t apply to EASTL, nor many of the other core tech packages.

## Performance

### Perf.1 How efficient is EASTL compared to standard C++ STL implementations?

With respect to the functionality that is equivalent between EASTL and standard STL, the short answer to this is that EASTL is as at least as efficient as othe STL implementations and in a number of aspects is more so. EASTL has functionality such as intrusive_list and linked_ptr that don't exist in standard STL but are explicitly present to provide significant optimizations over standard STL.

The medium length answer is that EASTL is significantly more efficient than Dinkumware STL, and Microsoft Windows STL. EASTL is generally more efficient than Metrowerks STL, but Metrowerks has a few tricks up its sleeve which EASTL doesn't currently implement. EASTL is roughly equal in efficiency to STLPort and GCC 3.x+ STL, though EASTL has some optimizations that these do not.

The long answer requires a breakdown of the functionality between various versions of the STL.

### Perf.2 How efficient is EASTL in general?

This question is related to the question, "How efficient are templates?" If you understand the effects of templates then you can more or less see the answer for EASTL. Templates are more efficient than the alternative when they are used appropriately, but can be less efficient than the alternative when used under circumstances that don't call for them. The strength of templates is that the compiler sees all the code and data types at compile time and can often reduce statements to smaller and faster code than with conventional non-templated code. The weakness of templates is that the sometimes produce more code and can result in what is often called "code bloat". However, it's important to note that unused template functions result in no generated nor linked code, so if you have a templated class with 100 functions but you only use one, only that one function will be compiled.

EASTL is a rather efficient implementation of a template library and pulls many tricks of the trade in terms of squeezing optimal performance out of the compiler. The only way to beat it is to write custom code for the data types you are working with, and even then people are sometimes surprised to find that their hand-implemented algorithm works no better or even worse than the EASTL equivalent. But certainly there are ways to beat templates, especially if you resort to assembly language programming and some kinds of other non-generic tricks.

### Perf.3 Strings don't appear to use the "copy-on-write" (CoW) optimization. Why not?

**Short answer**
CoW provides a benefit for a small percentage of uses but provides a disadvantage for the large majority of uses.

**Long answer**
The primary benefit of CoW is that it allows for the sharing of string data between two string objects. Thus if you say this:

```cpp
string a("hello");
string b(a);
```

the "hello" will be shared between a and b. If you then say this:

```cpp
a = "world";
```

then *a*  will release its reference to "hello" and leave b with the only reference to it. Normally this functionality is accomplished via reference counting and with atomic operations or mutexes.

The C++ standard does not say anything about basic_string and CoW. However, for a basic_string implementation to be standards-conforming, a number of issues arise which dictate some things about how one would have to implement a CoW string. The discussion of these issues will not be rehashed here, as you can read the references below for better detail than can be provided in the space we have here. However, we can say that the C++ standard is sensible and that anything we try to do here to allow for an efficient CoW implementation would result in a generally unacceptable string interface.

The disadvantages of CoW strings are:

* A reference count needs to exist with the string, which increases string memory usage.
* With thread safety, atomic operations and mutex locks are expensive, especially on weaker memory systems such as console gaming platforms.
* All non-const string accessor functions need to do a sharing check the the first such check needs to detach the string. Similarly, all string assignments need to do a sharing check as well. If you access the string before doing an assignment, the assignment doesn't result in a shared string, because the string has already been detached.
* String sharing doesn't happen the large majority of the time. In some cases, the total sum of the reference count memory can exceed any memory savings gained by the strings that share representations.

The addition of a cow_string class is under consideration for EASTL. There are conceivably some systems which have string usage patterns which would benefit from CoW sharing. Such functionality is best saved for a separate string implementation so that the other string uses aren't penalized.

References

This is a good starting HTML reference on the topic:
    http://www.gotw.ca/publications/optimizations.htm

Here is a well-known Usenet discussion on the topic:
    http://groups-beta.google.com/group/comp.lang.c++.moderated/browse_thread/thread/3dc6af5198d0bf7/886c8642cb06e03d

### Perf.4 Does EASTL cause code bloat, given that it uses templates?

The reason that templated functions and classes might cause an increase in code size because each template instantiation theoretically creates a unique piece of code. For example, when you compile this code:

```cpp
template <typename T>
const T min(const T a, const T b)
    { return b < a ? b : a; }

int    i = min<int>(3, 4);
double d = min<double>(3.0, 4.0);
```

the compiler treats it as if you wrote this:

```cpp
int min(const int a, const int b)
    { return b < a ? b : a; }

double min(const double a, const double b)
    { return b < a ? b : a; }
```

Imagine this same effect happening with containers such as list and map and you can see how it is that templates can cause code proliferation.

A couple things offset the possibility of code proliferation: inlining and folding. In practice the above 'min' function would be converted to inlined functions by the compiler which occupy only a few CPU instructions. In many of the simplest cases the inlined version actually occupies less code than the code required to push parameters on the stack and execute a function call. And they will execute much faster as well.

Code folding (a.k.a. "COMDAT folding", "duplicate stripping", "ICF" / "identical code folding") is a compiler optimization whereby the compiler realizes that two independent functions have compiled to the same code and thus can be reduced to a single function. The Microsoft VC++ compiler (Since VS2005), and GCC (v 4.5+) can do these kinds of optimizations on all platforms. This can result, for example, in all templated containers of pointers (e.g. vector<char*>, vector<Widget*>, etc.) to be linked as a single implementation. This folding occurs at a function level and so individual member functions can be folded while other member functions are not. A side effect of this optimization is that you aren't likely to gain much much declaring containers of void* instead of the pointer type actually contained.

The above two features reduce the extent of code proliferation, but certainly don't eliminate it. What you need to think about is how much code might be generated vs. what your alternatives are. Containers like vector can often inline completely away, whereas more complicated containers such as map can only partially be inlined. In the case of map, if you need an such a container for your Widgets, what alternatives do you have that would be more efficient than instantiating a map? This is up to you to answer.

It's important to note that C++ compilers will throw away any templated functions that aren't used, including unused member functions of templated classes. However, some argue that by having many functions available to the user that users will choose to use that larger function set rather than stick with a more restricted set.

Also, don't be confused by syntax bloat vs. code bloat. In looking at templated libraries such as EASTL you will notice that there is sometimes a lot of text in the definition of a template implementation. But the actual underlying code is what you need to be concerned about.

There is a good Usenet discussion on this topic at: http://groups.google.com/group/comp.lang.c++.moderated/browse_frm/thread/2b00649a935997f5

### Perf.5 Don't STL and EASTL containers fragment memory?

They only fragment memory if you use them in a way that does so. This is no different from any other type of container used in a dynamic way. There are various solutions to this problem, and EASTL provides additional help as well:

* For vectors, use the reserve function (or the equivalent constructor) to set aside a block of memory for the container. The container will not reallocate memory unless you try grow beyond the capacity you reserve.
* EASTL has "fixed" variations of containers which allow you to specify a fixed block of memory which the container uses for its memory. The container will not allocate any memory with these types of containers and all memory will be cache-friendly due to its locality.
* You can assign custom allocators to containers instead of using the default global allocator. You would typically use an allocator that has its own private pool of memory.
* Where possible, add all a container's elements to it at once up front instead of adding them over time. This avoids memory fragmentation and increase cache coherency.

### Perf.6 I don't see container optimizations for equivalent scalar types such as pointer types. Why?

Metrowerks (and no other, as of this writing) STL has some container specializations for type T* which maps them to type void*. The idea is that a user who declares a list of Widget* and a list of Gadget* will generate only one container: a list of void*. As a result, code generation will be smaller. Often this is done only in optimized builds, as such containers are harder to view in debug builds due to type information being lost.

The addition of this optimization is under consideration for EASTL, though it might be noted that optimizing compilers such as VC++ are already capable of recognizing duplicate generated code and folding it automatically as part of link-time code generation (LTCG) (a.k.a. "whole program optimization"). This has been verified with VC++, as the following code and resulting disassembly demonstrate:

```cpp
eastl::list<int*>        intPtrList;
eastl::list<TestObject*> toPtrList;

eastl_size_t n1 = intPtrList.size();
eastl_size_t n2 = toPtrList.size();

0042D288  lea         edx,[esp+14h]
0042D28C  call        eastl::list<TestObject>::size (414180h)
0042D291  push        eax
0042D292  lea         edx,[esp+24h]
0042D296  call        eastl::list<TestObject>::size (414180h)
```

Note that in the above case the compiler folded the two implementations of size() into a single implementation.

### Perf.7 I've seen some STL's provide a default quick "node allocator" as the default allocator. Why doesn't EASTL do this?

**Short answer**

This is a bad, misguided idea.

**Long answer**

These node allocators implement a heap for all of STL with buckets for various sizes of allocations and implemented fixed-size pools for each of these buckets. These pools are attractive at first because they do well in STL comparison benchmarks, especially when thread safety is disabled. Such benchmarks make it impossible to truly compare STL implementations because you have two different allocators in use and in some cases allocator performance can dominate the benchmark. However, the real problem with these node allocators is that they badly fragment and waste memory. The technical discussion of this topic is outside the scope of this FAQ, but you can learn more about it by researching memory management on the Internet. Unfortunately, the people who implement STL libraries are generally not experts on the topic of memory management. A better approach, especially for game development, is for the user to decide when fixed-size pools are appropriate and use them via custom allocator assignment to containers.

### Perf.8 Templates sometimes seem to take a long time to compile. Why do I do about that?

C++ compilers are generally slower than C compilers, and C++ templates are generally slower to compile than regular C++ code. EASTL has some extra functionality (such as type_traits and algorithm specializations) that is not found in most other template libraries and significantly improves performance and usefulness but adds to the amount of code that needs to be compiled. Ironically, we have a case where more source code generates faster and smaller object code.

The best solution to the problem is to use pre-compiled headers, which are available on all modern ~2002+) compilers, such as VC6.0+, GCC 3.2+, and Metrowerks 7.0+. In terms of platforms this means all 2002+ platforms.

Some users have been speeding up build times by creating project files that put all the source code in one large .cpp file. This has an effect similar to pre-compiled headers. It can go even faster than pre-compiled headers but has downsides in the way of convenience and portability.

### Perf.10 How well does EASTL inline?

EASTL is written in such as way as to be easier to inline than typical templated libraries such as STL. How is this so? It is so because EASTL reduces the inlining depth of many functions, particularly the simple ones. In doing so it makes the implementation less "academic" but entirely correct. An example of this is the vector operator[] function, which is implemented like so with Microsoft STL:

```cpp
reference operator[](size_type n) {
   return *(begin() + n);
}
```

EASTL implements the function directly, like so:

```cpp
reference operator[](size_type n) {
    return *(mpBegin + n);
}
```

Both implementations are correct, but hte EASTL implementation will run faster in debug builds, be easier to debug, and will be more likely to be inlined when the usage of this function is within a hierarchy of other functions being inlined. It is not so simple to say that the Microsoft version will always inline in an optimized build, as it could be part of a chain and cause the max depth to be exceeded.

That being said, EASTL appears to inline fairly well under most circumstances, including with GCC, which is the poorest of the compilers in its ability to inline well.

### Perf.11 How do I control function inlining?

Inlining is an important topic for templated code, as such code often relies on the compiler being able to do good function inlining for maximum performance. GCC, VC++, and Metrowerks are discussed here. We discuss compilation-level inlining and function-level inling here, though the latter is likely to be of more use to the user of EASTL, as it can externally control how EASTL is inlined. A related topic is GCC's template expansion depth, discussed elsewhere in this FAQ. We provide descriptions of inlining options here but don't currently have any advice on how to best use these with EASTL.

Compilation-Level Inlining -- VC++

VC++ has some basic functionality to control inlining, and the compiler is pretty good at doing aggressive inlining when optimizing on for all platforms.

> **#pragma inline_depth( [0... 255] )**
>
> Controls the number of times inline expansion can occur by controlling the number of times that a series of function calls can be expanded (from 0 to 255 times). This pragma controls the inlining of functions marked inline and or inlined automatically under the /Ob2 option. The inline_depth pragma controls the number of times a series of function calls can be expanded. For example, if the inline depth is 4, and if A calls B and B then calls C, all three calls will be expanded inline. However, if the closest inline expansion is 2, only A and B are expanded, and C remains as a function call.

> **#pragma inline_recursion( [{on | off}] )**
>
> Controls the inline expansion of direct or mutually recursive function calls. Use this pragma to control functions marked as inline and or functions that the compiler automatically expands under the /Ob2 option. Use of this pragma requires an /Ob compiler option setting of either 1 or 2. The default state for inline_recursion is off. The inline_recursion pragma controls how recursive functions are expanded. If inline_recursion is off, and if an inline function calls itself (either directly or indirectly), the function is expanded only once. If inline_recursion is on, the function is expanded multiple times until it reaches the value set by inline_depth, the default value of 8, or a capacity limit.

Compilation-Level Inlining -- GCC

GCC has a large set of options to control function inlining. Some options are available only  in GCC 3.0 and later and thus not present on older platforms.


> **-fno-default-inline**
>
> Do not make member functions inline by default merely because they are defined inside the class scope (C++ only). Otherwise, when you specify -O, member functions defined inside class scope are compiled inline by default; i.e., you don't need to add 'inline' in front of the member function name.
>
> **-fno-inline**
>
> Don't pay attention to the inline keyword. Normally this option is used to keep the compiler from expanding any functions inline. Note that if you are not optimizing, no functions can be expanded inline.
>
> **-finline-functions**
>
> Integrate all simple functions into their callers. The compiler heuristically decides which functions are simple enough to be worth integrating in this way. If all calls to a given function are integrated, and the function is declared static, then the function is normally not output as assembler code in its own right. Enabled at level -O3.
>
> **-finline-limit=n**
>
> By default, GCC limits the size of functions that can be inlined. This flag allows the control of this limit for functions that are explicitly marked as inline (i.e., marked with the inline keyword or defined within the class definition in c++). n is the size of functions that can be inlined in number of pseudo instructions (not counting parameter handling). pseudo-instructions are an internal representation of function size. The default value of n is 600. Increasing this value can result in more inlined code at the cost of compilation time and memory consumption. Decreasing usually makes the compilation faster and less code will be inlined (which presumably means slower programs). This option is particularly useful for programs that use inlining heavily such as those based on recursive templates with C++.
>
> Inlining is actually controlled by a number of parameters, which may be specified individually by using --param name=value. The -finline-limit=n option sets some of these parameters as follows:
>
> ```
> max-inline-insns-single
>    is set to n/2.
> max-inline-insns-auto
>    is set to n/2.
> min-inline-insns
>    is set to 130 or n/4, whichever is smaller.
> max-inline-insns-rtl
>     is set to n.
> ```
>
> See --param below for a documentation of the individual parameters controlling inlining.
>
> **-fkeep-inline-functions**
>
> Emit all inline functions into the object file, even if they are inlined where used.
>
> **--param name=value**
>
> In some places, GCC uses various constants to control the amount of optimization that is done. For example, GCC will not inline functions that contain more that a certain number of instructions. You can control some of these constants on the command-line using the --param option.
>
> max-inline-insns-single
> Several parameters control the tree inliner used in gcc. This number sets the maximum number of instructions (counted in GCC's internal representation) in a single function that the tree inliner will consider for inlining. This only affects functions declared inline and methods implemented in a class declaration (C++). The default value is 450.
>
> max-inline-insns-auto
> When you use -finline-functions (included in -O3), a lot of functions that would otherwise not be considered for inlining by the compiler will be investigated. To those functions, a different (more restrictive) limit compared to functions declared inline can be applied. The default value is 90.
>
>large-function-insns
> The limit specifying really large functions. For functions larger than this limit after inlining inlining is constrained by --param large-function-growth. This parameter is useful primarily to avoid extreme compilation time caused by non-linear algorithms used by the backend. This parameter is ignored when -funit-at-a-time is not used. The default value is 2700.
>
> large-function-growth
> Specifies maximal growth of large function caused by inlining in percents. This parameter is ignored when -funit-at-a-time is not used. The default value is 100 which limits large function growth to 2.0 times the original size.
>
> inline-unit-growth
> Specifies maximal overall growth of the compilation unit caused by inlining. This parameter is ignored when -funit-at-a-time is not used. The default value is 50 which limits unit growth to 1.5 times the original size.
>
> max-inline-insns-recursive
> max-inline-insns-recursive-auto
> Specifies maximum number of instructions out-of-line copy of self recursive inline function can grow into by performing recursive inlining. For functions declared inline --param max-inline-insns-recursive is taken into acount. For function not declared inline, recursive inlining happens only when -finline-functions (included in -O3) is enabled and --param max-inline-insns-recursive-auto is used. The default value is 450.
>
> max-inline-recursive-depth
> max-inline-recursive-depth-auto
> Specifies maximum recursion depth used by the recursive inlining. For functions declared inline --param max-inline-recursive-depth is taken into acount. For function not declared inline, recursive inlining happens only when -finline-functions (included in -O3) is enabled and --param max-inline-recursive-depth-auto is used. The default value is 450.
>
> inline-call-cost
> Specify cost of call instruction relative to simple arithmetics operations (having cost of 1). Increasing this cost disqualify inlinining of non-leaf functions and at same time increase size of leaf function that is believed to reduce function size by being inlined. In effect it increase amount of inlining for code having large abstraction penalty (many functions that just pass the argumetns to other functions) and decrease inlining for code with low abstraction penalty. Default value is 16.
>
> **-finline-limit=n**
>
> By default, GCC limits the size of functions that can be inlined. This flag allows the control of this limit for functions that are explicitly marked as inline (i.e., marked with the inline keyword or defined within the class definition in c++). n is the size of functions that can be inlined in number of pseudo instructions (not counting parameter handling). The default value of n is 600. Increasing this value can result in more inlined code at the cost of compilation time and memory consumption. Decreasing usually makes the compilation faster and less code will be inlined (which presumably means slower programs). This option is particularly useful for programs that use inlining heavily such as those based on recursive templates with C++.

Inlining is actually controlled by a number of parameters, which may be specified individually by using --param name=value. The -finline-limit=n option sets some of these parameters as follows:

```
max-inline-insns-single
   is set to n/2.
max-inline-insns-auto
   is set to n/2.
min-inline-insns
   is set to 130 or n/4, whichever is smaller.
max-inline-insns-rtl
   is set to n.
```

See below for a documentation of the individual parameters controlling inlining.

Note: pseudo instruction represents, in this particular context, an abstract measurement of function's size. In no way, it represents a count of assembly instructions and as such its exact meaning might change from one release to an another.

GCC additionally has the -Winline compiler warning, which emits a warning whenever a function declared as inline was not inlined.

Compilation-Level Inlining -- Metrowerks

Metrowerks has a number of pragmas (and corresponding compiler settings) to control inlining. These include always_inline, inline_depth, inline_max_size, and inline max_total_size.

> ```
> #pragma always_inline on | off | reset
> ```
>
> Controls the use of inlined functions. If you enable this pragma, the compiler ignores all inlining limits and attempts to inline all functions where it is legal to do so. This pragma is deprecated. Use the inline_depth pragma instead.
>
> ```
> #pragma inline_depth(n)
> #pragma inline_depth(smart)
> ```
>
> Controls how many passes are used to expand inline function. Sets the number of passes used to expand inline function calls. The number n is an integer from 0 to 1024 or the smart specifier. It also represents the distance allowed in the call chain from the last function up. For example, if d is the total depth of a call chain, then functions below (d-n) are inlined if they do not exceed the inline_max_size and inline_max_total_size settings which are discussed directly below.
>
> ```
> #pragma inline_max_size(n);
> #pragma inline_max_total_size(n);
> ```
>
> The first pragma sets the maximum function size to be considered for inlining; the second sets the maximum size to which a function is allowed to grow after the functions it calls are inlined. Here, n is the number of statements, operands, and operators in the function, which turns out to be roughly twice the number of instructions generated by the function. However, this number can vary from function to function. For the inline_max_size pragma, the default value of n is 256; for the inline_max_total_size pragma, the default value of n is 10000. The smart specifier is the default mode, with four passes where the passes 2-4 are limited to small inline functions. All inlineable functions are expanded if inline_depth is set to 1-1024.

Function-Level Inlining -- VC++

> To force inline usage under VC++, you use this:
>
> ```
> __forceinline void foo(){ ... }
> ```
>
> It should be noted that __forceinline has no effect if the compiler is set to disable inlining. It merely tells the compiler that when inlining is enabled that it shouldn't use its judgment to decide if the function should be inlined but instead to always inline it.
>
> To disable inline usage under VC++, you need to use this:
>
> ```
>    #pragma inline_depth(0) // Disable inlining.
>    void foo() { ... }
>    #pragma inline_depth()  // Restore default.
> ```
>
> The above is essentially specifying compiler-level inlining control within the code for a specific function.

**Function-Level Inlining -- GCC / Metrowerks**

> To force inline usage under GCC 3.1+, you use this:
>
> `inline void foo() __attribute__((always_inline)) { ... }`
>
> or
>
> `inline __attribute__((always_inline)) void foo() { ... }`
>
> To disable inline usage under GCC 3+, you use this:
>
> `void foo() __attribute__((noinline)) { ... }`
>
> or
>
> `inline __attribute__((noinline)) void foo() { ... }`

EABase has some wrappers for this, such as EA_FORCE_INLINE.

### Perf.12 C++ / EASTL seems to bloat my .obj files much more than C does.

There is no need to worry. The way most C++ compilers compile templates, they compile all seen template code into the current .obj module, which results in larger .obj files and duplicated template code in multiple .obj files. However, the linker will (and in fact must) select only a single version of any given function for the application, and these linked functions will usually be located contiguously.

Additionally, the debug information for template definitions is usually larger than that for non-templated C++ definitions, which itself is sometimes larger than C defintions due to name decoration.

### Perf.13 What are the best compiler settings for EASTL?

We will discuss various aspects of this topic here. As of this writing, more EASTL research on this topic has been done on Microsoft compiler platforms (e.g. Win32) than GCC platforms. Thus currently this discussion focuses on VC++ optimization. Some of the concepts are applicable to GCC, though. EASTL has been sucessfully compiled and tested (the EASTL unit test) on our major development platforms with the highest optimization settings enabled, including GCC's infamous -O3 level.

**Optimization Topics**

* Function inlining.
* Optimization for speed vs. optimization for size.
* Link-time code generation (LTCG).
* Profile-guided optimization (PGO).

**Function inlining**

EASTL is a template library and inlining is important for optimal speed. Compilers have various options for enabling inlining and those options are discussed in this FAQ in detail. Most users will want to enable some form of inlining when compiling EASTL and other templated libraries. For users that are most concerned about the compiler's inlining increasing code size may want to try the 'inline only functions marked as inline' compiler option. Here is a table of normalized results from the benchmark project (Win32 platform):
| Inlining Disabled | Inline only 'inline' | Inline any |
|------|------|------|------|
| **Application size** | 100K | 86K | 86K |
| **Execution time** | 100 | 75 | 75 |

The above execution times are highly simplified versions of the actual benchmark data but convey a sense of the general average behaviour that can be expected. In practice, simple functions such as vector::operator[] will execute much faster with inlining enabled but complex functions such as map::insert may execute no faster within inlining enabled.

**Optimization for Speed / Size**

Optimization for speed results in the compiler inlining more code than it would otherwise. This results in the inlined code executing faster than if it was not inlined. As mentioned above, basic function inlining can result in smaller code as well as faster code, but after a certain point highly inlined code becomes greater in size than less inlined code and the performance advantages of inlining start to lessen. The EASTL Benchmark project is a medium sized application that is about 80% templated and thus acts as a decent measure of the practical tradeoff between speed and size. Here is a table of normalized results from the benchmark project (Windows platform):
| Size | Speed | Speed + LTCG | Speed + LTCG + PGO |
|------|------|------|------|
| **Application size** | 80K | 100K | 98K | 98K |
| **Execution time** | 100 | 90 | 83 | 75 |

What the above table is saying is that if you are willing to have your EASTL code be 20% larger, it will be 10% faster. Note that it doesn't mean that your app will be 20% larger, only the templated code in it like EASTL will be 20% larger.

**Link-time code generation (LTCG)**

LTCG is a mechanism whereby the compiler compiles the application as if it was all in one big .cpp file instead of separate .cpp files that don't see each other. Enabling LTCG optimizations is done by simply setting some compiler and linker settings and results in slower link times. The benchmark results are presented above and for the EASTL Benchmark project show some worthwhile improvement.

**Profile-guided optimization (PGO)**

PGO is a mechanism whereby the compiler uses profiling information from one or more runs to optimize the compilation and linking of an application. Enabling PGO optimizations is done by setting some linker settings and doing some test runs of the application, then linking the app with the test run results. Doing PGO optimizations is a somewhat time-consuming task but the benchmark results above demonstrate that for the EASTL Benchmark project that PGO is worth the effort.

## Problems

### Prob.1 I'm getting screwy behavior in sorting algorithms or sorted containers. What's wrong?

It may possible that you are seeing floating point roundoff problems. Many STL algorithms require object comparisons to act consistently. However, floating point values sometimes compare differently between uses because in one situation a value might be in 32 bit form in system memory, whereas in anther situation that value might be in an FPU register with a different precision. These are difficult problems to track down and aren't the fault of EASTL or whatever similar library you might be using. There are various solutions to the problem, but the important thing is to find a way to force the comparisons to be consistent.

The code below was an example of this happening, whereby the object pA->mPos was stored in system memory while pB->mPos was stored in a register and comparisons were inconsistent and a crash ensued.

```cpp
class SortByDistance : public binary_function<WorldTreeObject*, WorldTreeObject*, bool>
{
private:
    Vector3 mOrigin;

public:
    SortByDistance(Vector3 origin) {
        mOrigin = origin;
    }

    bool operator()(WorldTreeObject* pA, WorldTreeObject* pB) const {
         return ((WorldObject*)pA)->mPos - mOrigin).GetLength()
              < ((WorldObject*)pB)->mPos - mOrigin).GetLength();
    }
};
```

Another thing to watch out for is the following mistake:

```cpp
struct ValuePair
{
    uint32_t a;
    uint32_t b;
};

// Improve speed by casting the struct to uint64_t
bool operator<(const ValuePair& vp1, const ValuePair& vp2)
    { return *(uint64_t*)&vp1 < *(uint64_t*)&vp2; }
```

The problem is that the ValuePair struct has 32 bit alignment but the comparison assumes 64 bit alignment. The code above has been observed to crash on the PowerPC 64-based machines. The resolution is to declare ValuePair as having 64 bit alignment.

### Prob.2 I am getting compiler warnings (e.g. C4244, C4242 or C4267) that make no sense. Why?

One cause of this occurs with VC++ when you have code compiled with the /Wp64 (detect 64 bit portability issues) option. This causes pointer types to have a hidden flag called __w64 attached to them by the compiler. So 'ptrdiff_t' is actually known by the compiler as '__w64 int', while 'int' is known by the compilers as simply 'int'. A problem occurs here when you use templates. For example, let's say we have this templated function

``` cpp
template <typename T>
T min(const T a, const T b) {
    return b < a ? b : a;
}
```

If you compile this code:

```cpp
ptrdiff_t a = min(ptrdiff_t(0), ptrdiff_t(1));
int       b = min((int)0, (int)1);
```

You will get the following warning for the second line, which is somewhat nonsensical:

`warning C4244: 'initializing' : conversion from 'const ptrdiff_t' to 'int', possible loss of data`

This could probably be considered a VC++ bug, but in the meantime you have little choice but to ignore the warning or disable it.

### Prob.3 I am getting compiler warning C4530, which complains about exception handling and "unwind semantics." What gives?

VC++ has a compiler option (/EHsc) that allows you to enable/disable exception handling stack unwinding but still enable try/catch. This is useful because it can save a lot in the way of code generation for your application. Disabling stack unwinding will decrease the size of your executable on at least the Win32 platform by 10-12%.

If you have stack unwinding disabled, but you have try/catch statements, VC++ will generate the following warning:

`warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc`

As of EASTL v1.0, this warning has been disabled within EASTL for EASTL code. However, non-EASTL code such as std STL code may still cause this warning to be triggered. In this case there is not much you can do about this other than to disable the warning.

### Prob.4 Why are tree-based EASTL containers hard to read with a debugger?

**Short answer**

Maximum performance and design mandates.

**Long answer**

You may notice that when you have a tree-based container (e.g. set, map)  in the debugger that it isn't automatically able to recognize the tree nodes as containing instances of your contained object. You can get the debugger to do what you want with casting statements in the debug watch window, but this is not an ideal solution. The reason this is happening is that node-based containers always use an anonymous node type as the base class for container nodes. This is primarily done for performance, as it allows the node manipulation code to exist as a single non-templated library of functions and it saves memory because containers will have one or two base nodes as container 'anchors' and you don't want to allocate a node of the size of the user data when you can just use a base node. See list.h for an example of this and some additional in-code documentation on this.

Additionally, EASTL has the design mandate that an empty container constructs no user objects. This is both for performance reasons and because it doing so would skew the user's tracking of object counts and might possibly break some expectation the user has about object lifetimes.

Currently this debug issue exists only with tree-based containers. Other node-based containers such as list and slist use a trick to get around this problem in debug builds.

See [Debug.2](#debug2-how-do-i-view-containers-if-the-visualizertooltip-support-is-not-present) for more.

### Prob.5 The EASTL source code is sometimes rather complicated looking. Why is that?

**Short answer**

Maximum performance.

**Long answer**
EASTL uses templates, type_traits, iterator categories, redundancy reduction, and branch reduction in order to achieve optimal performance. A side effect of this is that there are sometimes a lot of template parameters and multiple levels of function calls due to template specialization. The ironic thing about this is that this makes the code (an optimized build, at least) go faster, not slower. In an optimized build the compiler will see through the calls and template parameters and generate a direct optimized inline version.

As an example of this, take a look at the implementation of the copy implementation in algorithm.h. If you are copying an array of scalar values or other trivially copyable values, the compiler will see how the code directs this to the memcpy function and will generate nothing but a memcpy in the final code. For non-memcpyable data types the compiler will automatically understand that in do the right thing.

EASTL's primary objective is maximal performance, and it has been deemed worthwhile to make the code a little less obvious in order to achieve this goal. Every case where EASTL does something in an indirect way is by design and usually this is for the purpose of achieving the highest possible performance.

### Prob.6 When I get compilation errors, they are very long and complicated looking. What do I do?

Assuming the bugs are all worked out of EASTL, these errors really do indicate that you have something wrong. EASTL is intentionally very strict about types, as it tries to minimize the chance of users errors. Unfortunately, there is no simple resolution to the problem of long compiler errors other than to deal with them. On the other hand, once you've dealt with them a few times, you tend to realize that most of time they are the same kinds of errors and

Top five approaches to dealing with long compilation errors:

1. Look at the line where the compilation error occurred and ignore the text of the error and just look at obvious things that might be wrong.
2. Consider the most common typical causes of templated compilation errors and consider if any of these might be your problem. Usually one of them are.
3. Either read through the error (it's not as hard as it may look on the surface) or copy the error to a text file and remove the extraneous
4. Compile the code under GCC instead of MSVC, as GCC warnings and errors tend to be more helpful than MSVC's. Possibly also consider compiling an isolated version under Comeau C++'s free online compiler at www.comeaucomputing.com or the Dinkumware online compiler at http://dinkumware.com/exam/.
5. Try using an STL filter (http://www.bdsoft.com/tools/stlfilt.html) which automatically boils down template errors to simpler forms. We haven't tried this yet with EASTL. Also there is the more generic TextFilt (http://textfilt.sourceforge.net/).

Top five causes of EASTL compilation errors:

1. const-correctness. Perhaps a quarter of container template errors are due to the user not specifying const correctly.
2. Missing hash function. hash_map, hash_set, etc. require that you either specify a hash function or one exists for your class. See functional.h for examples of declarations of hash functions for common data types.
3. Missing operators. Various containers and algorithms require that certain operators exist for your contained classes. For example, list requires that you can test contained objects for equivalence (i.e. operator==), while map requires that you can test contained objects for "less-ness" (operator <). If you define a Widget class and don't have a way to compare two Widgets, you will get errors when trying to put them into a map.
4. Specifying the wrong data type. For example, it is a common mistake to forget that when you insert into a map, you need to insert a pair of objects and not just your key or value type.
5. Incorrect template parameters. When declaring a template instantiation (e.g. map<int, int, less<int> >) you simply need to get the template parameters correct. Also note that when you have ">>" next to each other that you need to separate them by one space (e.g. "> >").

### Prob.7 Templates sometimes seem to take a long time to compile. Why do I do about that?

C++ compilers are generally slower than C compilers, and C++ templates are generally slower to compile than regular C++ code. EASTL has some extra functionality (such as type_traits and algorithm specializations) that is not found in most other template libraries and significantly improves performance and usefulness but adds to the amount of code that needs to be compiled. Ironically, we have a case where more source code generates faster and smaller object code.

The best solution to the problem is to use pre-compiled headers, which are available on all modern ~2002+) compilers, such as VC6.0+, GCC 3.2+, and Metrowerks 7.0+. In terms of platforms this means all 2002+ platforms.

Some users have been speeding up build times by creating project files that put all the source code in one large .cpp file. This has an effect similar to pre-compiled headers. It can go even faster than pre-compiled headers but has downsides in the way of convenience and portability.

### Prob.8 I get the compiler error: "template instantiation depth exceeds maximum of 17. use -ftemplate-depth-NN to increase the maximum".

This is a GCC error that occurs when a templated function calls a templated function which calls a templated function, etc. past a depth of 17. You can use the GCC command line argument -ftemplate-depth-40 (or some other high number) to get around this. As note below, the syntax starting with GCC 4.5 has changed slightly.

The primary reason you would encounter this with EASTL is type traits that are used by algorithms. The type traits library is a (necessarily) highly templated set of types and functions which adds at most about nine levels of inlining. The copy and copy_backward algorithms have optimized pathways that add about four levels of inlining. If you have just a few more layers on top of that in container or user code then the default limit of 17 can be exceeded. We are investigating ways to reduce the template depth in the type traits library, but only so much can be done, as most compilers don't support type traits natively. Metrowerks is the current exception.

From the GCC documentation:

```
-ftemplate-depth-n

Set the maximum instantiation depth for template classes to n.
A limit on the template instantiation depth is needed to detect
endless recursions during template class instantiation ANSI/ISO
C++ conforming programs must not rely on a maximum depth greater than 17.
Note that starting with GCC 4.5 the syntax is -ftemplate-depth=N instead of -ftemplate-depth-n.
```

### Prob.9 I'm getting errors about min and max while compiling.

You need to define NOMINMAX under VC++ when this occurs, as it otherwise defines min and max macros that interfere. There may be equivalent issues with other compilers. Also, VC++ has a specific <minmax.h> header file which defines min and max macros but which doesn't pay attention to NOMINMAX and so in that case there is nothing to do but not include that file or to undefine min and max. minmax.h is not a standard file and its min and max macros are not standard C or C++ macros or functions.

### Prob.10 C++ / EASTL seems to bloat my .obj files much more than C does.

There is no need to worry. The way most C++ compilers compile templates, they compile all seen template code into the current .obj module, which results in larger .obj files and duplicated template code in multiple .obj files. However, the linker will (and must) select only a single version of any given function for the application, and these linked functions will usually be located contiguously.

### Prob.11 I'm getting compiler errors regarding placement operator new being previously defined.

This can happen if you are attempting to define your own versions of placement new/delete. The C++ language standard does not allow the user to override these functions. Section 18.4.3 of the standard states:

> Placement forms
> 1. These functions are reserved, a C++ program may not define functions that displace the versions in the Standard C++ library.

You may find that #defining __PLACEMENT_NEW_INLINE seems to fix your problems under VC++, but it can fail under some circumstances and is not portable and fails with other compilers, which don't have an equivalent workaround.

### Prob.12 I'm getting errors related to wchar_t string  functions such as wcslen().

EASTL requires EABase-related items that the following be so. If not, then EASTL gets confused about what types it can pass to wchar_t related functions.

* The #define EA_WCHAR_SIZE is equal to sizeof(wchar_t).
* If sizeof(wchar_t) == 2, then char16_t is typedef'd to wchar_t.
* If sizeof(wchar_t) == 4, then char32_t is typedef'd to wchar_t.

EABase v2.08 and later automatically does this for most current generation and all next generation platforms. With GCC 2.x, the user may need to predefine EA_WCHAR_SIZE to the appropriate value, due to limitations with the GCC compiler. Note that GCC defaults to sizeof(wchar_t) ==4, but it can be changed to 2 with the -fshort_wchar compiler command line argument. If you are using EASTL without EABase, you will need to make sure the above items are correctly defined.

### Prob.13 I'm getting compiler warning C4619: there is no warning number Cxxxx (e.g. C4217).

Compiler warning C4619 is a VC++ warning which is saying that the user is attempting to enable or disable a warning which the compiler doesn't recognize. This warning only occurs if the user has the compiler set to enable warnings that are normally disabled, regardless of the warning level. The problem, however, is that there is no easy way for user code to tell what compiler warnings any given compiler version will recognize. That's why Microsoft normally disables this warning.

The only practical solution we have for this is for the user to disable warning 4619 globally or an a case-by-case basis. EA build systems such as nant/framework 2's eaconfig will usually disable 4619. In general, global enabling of 'warnings that are disabled by default' often result in quandrys such as this.

### Prob.14 My stack-based fixed_vector is not respecting the object alignment requirements.

EASTL fixed_* containers rely on the compiler-supplied alignment directives, such as that implemented by EA_PREFIX_ALIGN. This is normally a good thing because it allows the memory to be local with the container. However, as documented by Microsoft at http://msdn2.microsoft.com/en-us/library/83ythb65(VS.71).aspx, this doesn't work for stack variables. The two primary means of working around this are:

* Use something like AlignedObject<> from the EAStdC package's EAAllocator.h file.
* Use eastl::vector with a custom allocator and have it provide aligned memory. EASTL automatically recognizes that the objects are aligned and will call the aligned version of your allocator allocate() function. You can get this aligned memory from the stack, if you need it, somewhat like how AlignedObject<> works.

### Prob.15 I am getting compiler errors when using GCC under XCode (Macintosh/iphone).

The XCode environment has a compiler option which causes it to evaluate include directories recursively. So if you specify /a/b/c as an include directory, it will consider all directories underneath c to also be include directories. This option is enabled by default, though many XCode users disable it, as it is a somewhat dangerous option. The result of enabling this option with EASTL is that <EASTL/string.h> is used by the compiler when you say #include <string.h>. The solution is to disable this compiler option. It's probably a good idea to disable this option anyway, as it typically causes problems for users yet provides minimal benefits.

### Prob.16 I am getting linker errors about Vsnprintf8 or Vsnprintf16.

EASTL requires the user to provide a function called Vsnprintf8 if the string::sprintf function is used. vsnprintf is not a standard C function, but most C standard libraries provide some form of it, though in some ways their implementations differ, especially in what the return value means. Also, most implementations of vsnprintf are slow, mostly due to mutexes related to locale functionality. And you can't really use vendor vsnprintf on an SPU due to the heavy standard library size. EASTL is stuck because it doesn't want to depend on something with these problems. EAStdC provides a single consistent fast lightweight, yet standards-conforming, implementation in the form of Vsnprintf(char8_t*, ...), but EASTL can't have a dependency on EAStdC. So the user must provide an implementation, even if all it does is call EAStdC's Vsnprintf or the vendor vsnprintf for that matter.

Example of providing Vsnprintf8 via EAStdC:

```cpp
#include <EAStdC/EASprintf.h>

int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
{
    return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
}

int Vsnprintf16(char16_t* pDestination, size_t n, const char16_t* pFormat, va_list arguments)
{
    return EA::StdC::Vsnprintf(pDestination, n, pFormat, arguments);
}
```

Example of providing Vsnprintf8 via C libraries:

```cpp
#include <stdio.h>

int Vsnprintf8(char8_t* p, size_t n, const char8_t* pFormat, va_list arguments)
{
    #ifdef _MSC_VER
        return vsnprintf_s(p, n, _TRUNCATE, pFormat, arguments);
    #else
        return vsnprintf(p, n, pFormat, arguments);
    #endif
}

int Vsnprintf16(char16_t* p, size_t n, const char16_t* pFormat, va_list arguments)
{
    #ifdef _MSC_VER
        return vsnwprintf_s(p, n, _TRUNCATE, pFormat, arguments);
    #else
        return vsnwprintf(p, n, pFormat, arguments); // Won't work on Unix because its libraries implement wchar_t as int32_t.
    #endif
}
```

### Prob.17 I am getting compiler errors about UINT64_C or UINT32_C.

This is usually an order-of-include problem that comes about due to the implementation of __STDC_CONSTANT_MACROS in C++ Standard libraries. The C++ <stdint.h> header file defineds UINT64_C only if __STDC_CONSTANT_MACROS has been defined by the user or the build system; the compiler doesn't automatically define it. The failure you are seeing occurs because user code is #including a system header before #including EABase and without defining __STDC_CONSTANT_MACROS itself or globally. EABase defines __STDC_CONSTANT_MACROS and #includes the appropriate system header. But if the system header was already previously #included and __STDC_CONSTANT_MACROS was not defined, then UINT64_C doesn't get defined by anybody.

The real solution that the C++ compiler and standard library wants is for the app to globally define __STDC_CONSTANT_MACROS itself in the build.

### Prob.18 I am getting a crash with a global EASTL container.

This usually due to compiler's lack of support for global (and static) C++ class instances. The crash is happening because the global variable exists but its constructor was not called on application startup and it's member data is zeroed bytes. To handle this you need to manually initialize such variables. There are two primary ways:

Failing code:

```cpp
eastl::list<int> gIntList; // Global variable.

void DoSomething()
{
    gIntList.push_back(1); // Crash. gIntList was never constructed.
}
```

Declaring a pointer solution:

```cpp
eastl::list<int>* gIntList = NULL;

void DoSomething()
{
    if(!gIntList) // Or move this to an init function.
        gIntList = new eastl::list<int>;

    gIntList->push_back(1); // Success
}
```

Manual constructor call solution:

```cpp
eastl::list<int> gIntList;

void InitSystem()
{
    new(&gIntList) eastl::list<int>;
}

void DoSomething()
{
    gIntList.push_back(1); // Success
}
```

### Prob.19 Why doesn't EASTL support passing NULL string functions?

The primary argument is to make functions safer for use. Why crash on NULL pointer access when you can make the code safe? That's a good argument. The counter argument, which EASTL currently makes, is:

> It breaks consistency with the C++ STL library and C libraries, which require strings to be valid.
>
> It makes the coder slower and bigger for all users, though few need NULL checks.
The specification for how to handle NULL is simple for some cases but not simple for others. Operator < below a case where the proper handling of it in a consistent way is not simple, as all comparison code (<, >, ==, !=, >=, <=) in EASTL must universally and consistently handle the case where either or both sides are NULL. A NULL string seems similar to an empty string, but doesn't always work out so simply.
>
> What about other invalid string pointers? NULL is merely one invalid value of many, with its only distinction being that sometimes it's intentionally NULL (as opposed to being NULL due to not being initialized).
How and where to implement the NULL checks in such a way as to do it efficiently is not always simple, given that public functions call public functions.
>
> It's arguable (and in fact the the intent of the C++ standard library) that using pointers that are NULL is a user/app mistake. If we really want to be safe then we should be using string objects for everything. You may not entirely buy this argument in practice, but on the other hand one might ask why is the caller of EASTL using a NULL pointer in the first place? The answer of course is that somebody gave it to him.

## Debug

### Debug.1 How do I set the VC++ debugger to display EASTL container data with tooltips?

See [Cont.9](#cont9-how-do-i-set-the-vc-debugger-to-display-eastl-container-data-with-tooltips)

### Debug.2 How do I view containers if the visualizer/tooltip support is not present?

Here is a table of answers about how to manually inspect containers in the debugger.

| Container | Approach |
|------|------|
| slist<br>fixed_slist | slist is a singly-linked list. Look at the slist mNode variable. You can walk the list by looking at mNode.mpNext, etc. |
| list<br>fixed_list | list is a doubly-linked list. Look at the list mNode variable. You can walk the list forward by looking at mNode.mpNext, etc. and backward by looking at mpPrev, etc. |
| intrusive_list<br>intrusive_slist† | Look at the list mAnchor node. This lets you walk forward and backward in the list via mpNext and mpPrev. |
| array | View the array mValue member in the debugger. It's simply a C style array. |
| vector<br>fixed_vector | View the vector mpBegin value in the debugger. If the string is long, use ", N" to limit the view length, as with someVector.mpBegin, 32 |
| vector_set<br>vector_multiset<br>vector_map<br>vector_multimap | These are containers that are implemented as a sorted vector, deque, or array. They are searched via a standard binary search. You can view them the same way you view a vector or deque. |
| deque | deque is implemented as an array of arrays, where the arrays implement successive equally-sized segments of the deque. The mItBegin deque member points the deque begin() position. |
| bitvector | Look at the bitvector mContainer variable. If it's a vector, then see vector above. |
| bitset | Look at the bitset mWord variable. The bitset is nothing but one or more uint32_t mWord items. |
| set<br>multiset<br>fixed_set<br>fixed_multiset | The set containers are implemented as a tree of elements. The set mAnchor.mpNodeParent points to the top of the tree; the mAnchor.mpNodeLeft points to the far left node of the tree (set begin()); the mAnchor.mpNodeRight points to the right of the tree (set end()). |
| map<br>multimap<br>fixed_map<br>fixed_multimap | The map containers are implemented as a tree of pairs, where pair.first is the map key and pair.second is the map value. The map mAnchor.mpNodeParent points to the top of the tree; the mAnchor.mpNodeLeft points to the far left node of the tree (map begin()); the mAnchor.mpNodeRight points to the right of the tree (map end()). |
| hash_map<br>hash_multimap<br>fixed_hash_map<br>fixed_hash_multimap | hash tables in EASTL are implemented as an array of singly-linked lists. The array is the mpBucketArray member. Each element in the list is a pair, where the first element of the pair is the map key and the second is the map value. |
| intrusive_hash_map<br>intrusive_hash_multimap<br>intrusive_hash_set<br>intrusive_hash_multiset | intrusive hash tables in EASTL are implemented very similarly to regular hash tables. See the hash_map and hash_set entries for more info. |
| hash_set<br>hash_multiset<br>fixed_hash_set<br>fixed_hash_map | hash tables in EASTL are implemented as an array of singly-linked lists. The array is the mpBucketArray member. |
| basic_string<br>fixed_string<br>fixed_substring | View the string mpBegin value in the debugger. If the string is long, use ", N" to limit the view length, as with someString.mpBegin, 32 |
| heap | A heap is an array of data (e.g. EASTL vector) which is organized in a tree whereby the highest priority item is array[0], The next two highest priority items are array[1] and [2]. Underneath [1] in priority are items [3] and [4], and underneath item [2] in priority are items [5] and [6]. etc. |
| stack | View the stack member c value in the debugger. That member will typically be a list or deque. |
| queue | View the queue member c value in the debugger. That member will typically be a list or deque. |
| priority_queue | View the priority_queue member c value in the debugger. That member will typically be a vector or deque which is organized as a heap. See the heap section above for how to view a heap. |
| smart_ptr | View the mpValue member. |

### Debug.3 The EASTL source code is sometimes rather complicated looking. Why is that?

**Short answer**

Maximum performance.

**Long answer**

EASTL uses templates, type_traits, iterator categories, redundancy reduction, and branch reduction in order to achieve optimal performance. A side effect of this is that there are sometimes a lot of template parameters and multiple levels of function calls due to template specialization. The ironic thing about this is that this makes the code (an optimized build, at least) go faster, not slower. In an optimized build the compiler will see through the calls and template parameters and generate a direct optimized inline version.

As an example of this, take a look at the implementation of the copy implementation in algorithm.h. If you are copying an array of scalar values or other trivially copyable values, the compiler will see how the code directs this to the memcpy function and will generate nothing but a memcpy in the final code. For non-memcpyable data types the compiler will automatically understand that in do the right thing.

EASTL's primary objective is maximal performance, and it has been deemed worthwhile to make the code a little less obvious in order to achieve this goal. Every case where EASTL does something in an indirect way is by design and usually this is for the purpose of achieving the highest possible performance.

### Debug.4 When I get compilation errors, they are very long and complicated looking. What do I do?

Assuming the bugs are all worked out of EASTL, these errors really do indicate that you have something wrong. EASTL is intentionally very strict about types, as it tries to minimize the chance of users errors. Unfortunately, there is no simple resolution to the problem of long compiler errors other than to deal with them. On the other hand, once you've dealt with them a few times, you tend to realize that most of time they are the same kinds of errors and

Top five approaches to dealing with long compilation errors:

1.Look at the line where the compilation error occurred and ignore the text of the error and just look at obvious things that might be wrong.
2. Consider the most common typical causes of templated compilation errors and consider if any of these might be your problem. Usually one of them are.
3. Either read through the error (it's not as hard as it may look on the surface) or copy the error to a text file and remove the extraneous
4. Compile the code under GCC instead of MSVC, as GCC warnings and errors tend to be more helpful than MSVC's. Possibly also consider compiling an isolated version under Comeau C++'s free online compiler at www.comeaucomputing.com or the Dinkumware online compiler at http://dinkumware.com/exam/.
5. Try using an STL filter (http://www.bdsoft.com/tools/stlfilt.html) which automatically boils down template errors to simpler forms. We haven't tried this yet with EASTL. Also there is the more generic TextFilt (http://textfilt.sourceforge.net/).

Top five causes of EASTL compilation errors:

1. const-correctness. Perhaps a quarter of container template errors are due to the user not specifying const correctly.
2. Missing hash function. hash_map, hash_set, etc. require that you either specify a hash function or one exists for your class. See functional.h for examples of declarations of hash functions for common data types.
3. Missing operators. Various containers and algorithms require that certain operators exist for your contained classes. For example, list requires that you can test contained objects for equivalence (i.e. operator==), while map requires that you can test contained objects for "less-ness" (operator <). If you define a Widget class and don't have a way to compare two Widgets, you will get errors when trying to put them into a map.
4. Specifying the wrong data type. For example, it is a common mistake to forget that when you insert into a map, you need to insert a pair of objects and not just your key or value type.
5. Incorrect template parameters. When declaring a template instantiation (e.g. map<int, int, less<int> >) you simply need to get the template parameters correct. Also note that when you have ">>" next to each other that you need to separate them by one space (e.g. "> >").

### Debug.5 How do I measure hash table balancing?

The following functionality lets you spelunk hash container layout.

* There is the load_factor function which tells you the overall hashtable load, but doesn't tell you if a load is unevenly distributed.
* You can control the load factor and thus the automated bucket redistribution with set_load_factor.
* The local_iterator begin(size_type n) and local_iterator end(size_type) functions lets you iterate each bucket individually. You can use this to examine the elements in a bucket.
* You can use the above to get the size of any bucket, but there is also simply the bucket_size(size_type n) function.
* The bucket_count function tells you the count of buckets. So with this you can completely visualize the layout of the hash table.
* There is also iterator find_by_hash(hash_code_t c), for what it's worth.

The following function draws an ASCII bar graph of the hash table for easy visualization of bucket distribution:

```cpp
#include <EASTL/hash_map.h>
#include <EASTL/algorithm.h>
#include <stdio.h>

template <typename HashTable>
void VisualizeHashTableBuckets(const HashTable& h)
{
    eastl_size_t bucketCount       = h.bucket_count();
    eastl_size_t largestBucketSize = 0;

    for(eastl_size_t i = 0; i < bucketCount; i++)
        largestBucketSize = eastl::max_alt(largestBucketSize, h.bucket_size(i));

    YourPrintFunction("\n --------------------------------------------------------------------------------\n");

    for(eastl_size_t i = 0; i < bucketCount; i++)
    {
        const eastl_size_t k = h.bucket_size(i) * 80 / largestBucketSize;

        char buffer[16];
        sprintf(buffer, "%3u|", (unsigned)i);
        YourPrintFunction(buffer);

        for(eastl_size_t j = 0; j < k; j++)
            YourPrintFunction("*");

        YourPrintFunction("\n");
    }

    YourPrintFunction(" --------------------------------------------------------------------------------\n");
}
```

This results in a graph that looks like the following (with one horizontal bar per bucket). This hashtable has a large number of collisions in each of its 10 buckets.

```
   ------------------------------------------------------
 0|********************************************
 1|************************************************
 2|***************************************
 3|********************************************
 4|*****************************************************
 5|*************************************************
 6|****************************************
 7|***********************************************
 8|********************************************
 9|**************************************
10|********************************************
   -----------------------------------------------------
```

## Containers

### Cont.1 Why do some containers have "fixed" versions (e.g. fixed_list) but others(e.g. deque) don't have fixed versions?

Recall that fixed containers are those that are implemented via a single contiguous block of memory and don't use a general purpose heap to allocate memory from. For example, fixed_list is a list container that implements its list by a user-configurable fixed block of memory. Such containers have an upper limit to how many items they can hold, but have the advantage of being more efficient with memory use and memory access coherency.

The reason why some containers don't have fixed versions is that such functionality doesn't make sense with these containers. Containers which don't have fixed versions include:

```
array, deque, bitset, stack, queue, priority_queue,
intrusive_list, intrusive_hash_map, intrusive_hash_set,
intrusive_hash_multimap, intrusive_hash_multimap,
vector_map, vector_multimap, vector_set, vector_multiset.
```

Some of these containers are adapters which wrap other containers and thus there is no need for a fixed version because you can just wrap a fixed container. In the case of intrusive containers, the user is doing the allocation and so there are no memory allocations. In the case of array, the container is a primitive type which doesn't allocate memory. In the case of deque, it's primary purpose for being is to dynamically resize and thus the user would likely be better of using a fixed_vector.

### Cont.2 Can I mix EASTL with standard C++ STL?

This is possible to some degree, though the extent depends on the implementation of C++ STL. One of things that makes interoperability is something called iterator categories. Containers and algorithms recognize iterator types via their category and STL iterator categories are not recognized by EASTL and vice versa.

Things that you definitely can do:

* #include both EASTL and standard STL headers from the same .cpp file.
* Use EASTL containers to hold STL containers.
* Construct an STL reverse_iterator from an EASTL iterator.
* Construct an EASTL reverse_iterator from an STL iterator.

Things that you probably will be able to do, though a given std STL implementation may prevent it:

* Use STL containers in EASTL algorithms.
* Use EASTL containers in STL algorithms.
* Construct or assign to an STL container via iterators into an EASTL container.
* Construct or assign to an EASTL container via iterators into an STL container.

Things that you would be able to do if the given std STL implementation is bug-free:

* Use STL containers to hold EASTL containers. Unfortunately, VC7.x STL has a confirmed bug that prevents this. Similarly, STLPort versions prior to v5 have a similar but.

Things that you definitely can't do:

* Use an STL allocator directly with an EASTL container (though you can use one indirectly).
* Use an EASTL allocator directly with an STL container (though you can use one indirectly).

### Cont.3 Why are there so many containers?

EASTL has a large number of container types (e.g vector, list, set) and often has a number of variations of given types (list, slist, intrusive_list, fixed_list). The reason for this is that each container is tuned and to a specific need and there is no single container that works for all needs. The more the user is concerned about squeezing the most performance out of their system, the more the individual container variations become significant. It's important to note that having additional container types generally does not mean generating additional code or code bloat. Templates result in generated code regardless of what templated class they come from, and so for the most part you get optimal performance by choosing the optimal container for your needs.

### Cont.4 Don't STL and EASTL containers fragment memory?

They only fragment memory if you use them in a way that does so. This is no different from any other type of container used in a dynamic way. There are various solutions to this problem, and EASTL provides additional help as well:

For vectors, use the reserve function (or the equivalent constructor) to set aside a block of memory for the container. The container will not reallocate memory unless you try grow beyond the capacity you reserve.
EASTL has "fixed" variations of containers which allow you to specify a fixed block of memory which the container uses for its memory. The container will not allocate any memory with these types of containers and all memory will be cache-friendly due to its locality.
You can assign custom allocators to containers instead of using the default global allocator. You would typically use an allocator that has its own private pool of memory.
Where possible, add all a container's elements to it at once up front instead of adding them over time. This avoids memory fragmentation and increase cache coherency.

### Cont.5 I don't see container optimizations for equivalent scalar types such as pointer types. Why?

Metrowerks (and no other, as of this writing) STL has some container specializations for type T* which maps them to type void*. The idea is that a user who declares a list of Widget* and a list of Gadget* will generate only one container: a list of void*. As a result, code generation will be smaller. Often this is done only in optimized builds, as such containers are harder to view in debug builds due to type information being lost.

The addition of this optimization is under consideration for EASTL, though it might be noted that optimizing compilers such as VC++ are already capable of recognizing duplicate generated code and folding it automatically as part of link-time code generation (LTCG) (a.k.a. "whole program optimization"). This has been verified with VC++, as the following code and resulting disassembly demonstrate:

```cpp
eastl::list<int*>        intPtrList;
eastl::list<TestObject*> toPtrList;

eastl_size_t n1 = intPtrList.size();
eastl_size_t n2 = toPtrList.size();

0042D288  lea         edx,[esp+14h]
0042D28C  call        eastl::list<TestObject>::size (414180h)
0042D291  push        eax
0042D292  lea         edx,[esp+24h]
0042D296  call        eastl::list<TestObject>::size (414180h)
```

Note that in the above case the compiler folded the two implementations of size() into a single implementation.

### Cont.6 What about alternative container and algorithm implementations (e.g. treaps, skip lists, avl trees)?

EASTL chooses to implement some alternative containers and algorithms and not others. It's a matter of whether or not the alternative provides truly complementary or improved functionality over existing containers. The following is a list of some implemented and non-implemented alternatives and the rationale behind each:

Implemented:

* intrusive_list, etc. -- Saves memory and improves cache locality.
* vector_map, etc. -- Saves memory and improves cache locality.
* ring_buffer -- Useful for some types of operations and has no alternative.
* shell_sort -- Useful sorting algorithm.
* sparse_matrix -- Useful for some types of operations and has no alternative.

Not implemented:

* skip lists (alternative to red-black tree) -- These use more memory and usually perform worse than rbtrees.
* treap (alternative to red-black tree) -- These are easier and smaller than rbtrees, but perform worse.
* avl tree (alternative to red-black tree) -- These have slightly better search performance than rbtrees, but significantly worse * * insert/remove performance.
* btree (alternative to red-black tree) --  These are no better than rbtrees.

If you have an idea of something that should be implemented, please suggest it or even provide at least a prototypical implementation.

### Cont.7 Why are tree-based EASTL containers hard to read with a debugger?

**Short answer**

Maximum performance and design mandates.

**Long answer**

You may notice that when you have a tree-based container (e.g. set, map)  in the debugger that it isn't automatically able to recognize the tree nodes as containing instances of your contained object. You can get the debugger to do what you want with casting statements in the debug watch window, but this is not an ideal solution. The reason this is happening is that node-based containers always use an anonymous node type as the base class for container nodes. This is primarily done for performance, as it allows the node manipulation code to exist as a single non-templated library of functions and it saves memory because containers will have one or two base nodes as container 'anchors' and you don't want to allocate a node of the size of the user data when you can just use a base node. See list.h for an example of this and some additional in-code documentation on this.

Additionally, EASTL has the design mandate that an empty container constructs no user objects. This is both for performance reasons and because it doing so would skew the user's tracking of object counts and might possibly break some expectation the user has about object lifetimes.

Currently this debug issue exists only with tree-based containers. Other node-based containers such as list and slist use a trick to get around this problem in debug builds.

### Cont.8 How do I assign a custom allocator to an EASTL container?

There are two ways of doing this:

1. Use the set_allocator function that is present in each container.
2. Specify a new allocator type via the Allocator template parameter that is present in each container.

For item #1, EASTL expects that you provide an instance of an allocator of the type that EASTL recognizes. This is simple but has the disadvantage that all such allocators must be of the same class. The class would need to have C++ virtual functions in order to allow a given instance to act differently from another instance.

For item #2, you specify that the container use your own allocator class. The advantage of this is that your class can be implemented any way you want and doesn't require virtual functions for differentiation from other instances. Due to the way C++ works your class would necessarily have to use the same member function names as the default allocator class type. In order to make things easier, we provide a skeleton allocator here which you can copy and fill in with your own implementation.

```cpp
class custom_allocator
{
public:
    custom_allocator(const char* pName = EASTL_NAME_VAL("custom allocator"))
    {
        #if EASTL_NAME_ENABLED
            mpName = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
        #endif

        // Possibly do something here.
    }

    custom_allocator(const allocator& x, const char* pName = EASTL_NAME_VAL("custom allocator"));
    {
        #if EASTL_NAME_ENABLED
            mpName = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
        #endif

        // Possibly copy from x here.
    }

    ~custom_allocator();
    {
        // Possibly do something here.
    }

    custom_allocator& operator=(const custom_allocator& x)
    {
        // Possibly copy from x here.
        return *this;
    }

    void* allocate(size_t n, int flags = 0)
    {
        // Implement the allocation here.
    }

    void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0)
    {
        // Implement the allocation here.
    }

    void deallocate(void* p, size_t n)
    {
        // Implement the deallocation here.
    }

    const char* get_name() const
    {
        #if EASTL_NAME_ENABLED
            return mpName;
        #else
            return "custom allocator";
        #endif
    }

    void set_name(const char* pName)
    {
        #if EASTL_NAME_ENABLED
            mpName = pName;
        #endif
    }

protected:
    // Possibly place instance data here.

    #if EASTL_NAME_ENABLED
        const char* mpName; // Debug name, used to track memory.
    #endif
};


inline bool operator==(const allocator& a, const allocator& b)
{
    // Provide a comparison here.
}

inline bool operator!=(const allocator& a, const allocator& b)
{
    // Provide a negative comparison here.
}
```

Here's an example of how to use the above custom allocator:

```cpp
// Declare a Widget list and have it default construct.
list<Widget, custom_allocator> widgetList;

// Declare a Widget list and have it construct with a copy of some global allocator.
list<Widget, custom_allocator> widgetList2(gSomeGlobalAllocator);

// Declare a Widget list and have it default construct, but assign
// an underlying implementation after construction.
list<Widget, custom_allocator> widgetList;
widgetList.get_allocator().mpIAllocator = new WidgetAllocatorImpl;
```

### Cont.9 How do I set the VC++ debugger to display EASTL container data with tooltips?

Visual Studio supports this via the AutoExp.dat file, an example of which is [present](./html/AutoExp.dat) with this documentation.

Sometimes the AutoExp.dat doesn't seem to work. Avery Lee's explanation:

> If I had to take a guess, the problem is most likely in the cast to the concrete node type. These are always tricky because, for some strange reason, the debugger is whitespace sensitive with regard to specifying template types. You might try manually checking one of the routines of the specific map instantiation and checking that the placement of whitespace and const within the template expression still matches exactly. In some cases the compiler uses different whitespace rules depending on the value type which makes it impossible to correctly specify a single visualizer – this was the case for eastl::list<>, for which I was forced to include sections for both cases. The downside is that you have a bunch of (error) entries either way.

### Cont.10 How do I use a memory pool with a container?

Using custom memory pools is a common technique for decreasing memory fragmentation and increasing memory cache locality. EASTL gives you the flexibility of defining your own memory pool systems for containers. There are two primary ways of doing this:

* Assign a custom allocator to a container. eastl::fixed_pool provides an implementation.
* Use one of the EASTL fixed containers, such as fixed_list.

**Custom Allocator**

In the custom allocator case, you will want to create a memory pool and assign it to the container. For purely node-based containers such as list, slist, map, set, multimap, and multiset, your pool simply needs to be able to allocate list nodes. Each of these containers has a member typedef called node_type which defines the type of node allocated by the container. So if you have a memory pool that has a constructor that takes the size of pool items and the count of pool items, you would do this (assuming that MemoryPool implements the Allocator interface):

```cpp
typedef list<Widget, MemoryPool> WidgetList;           // Declare your WidgetList type.

MemoryPool myPool(sizeof(WidgetList::node_type), 100); // Make a pool of 100 Widget nodes.
WidgetList myList(&myPool);                            // Create a list that uses the pool.
```

In the case of containers that are array-based, such as vector and basic_string, memory pools don't work very well as these containers work on a realloc-basis instead of by adding incremental nodes. What what want to do with these containers is assign a sufficient block of memory to them and reserve() the container's capacity to the size of the memory.

In the case of mixed containers which are partly array-based and partly node based, such as hash containers and deque, you can use a memory pool for the nodes but will need a single array block to supply for the buckets (hash containers and deque both use a bucket-like system).

You might consider using eastl::fixed_pool as such an allocator, as it provides such functionality and allows the user to provide the actual memory used for the pool. Here is some example code:

```cpp
char buffer[256];

list<Widget, fixed_pool> myList;
myList.get_allocator().init(buffer, 256);
Fixed Container
In the fixed container case, the container does all the work for you. To use a list which implements a private pool of memory, just declare it like so:

fixed_list<Widget, 100> fixedList; // Declare a fixed_list that can hold 100 Widgets
```

### Cont.11 How do I write a comparison (operator<()) for a struct that contains two or more members?

See [Algo.2](#algo2-how-do-i-write-a-comparison-operator-for-a-struct-that-contains-two-or-more-members).

### Cont.12 Why doesn't container X have member function Y?

Why don't the list or vector containers have a find() function? Why doesn't the vector container have a sort() function? Why doesn't the string container have a mid() function? These are common examples of such questions.

The answer usually boils down to two reasons:

* The functionality exists in a more centralized location elsewhere, such as the algorithms.
* The functionality can be had by using other member functions.

In the case of find and sort functions not being part of containers, the find algorithm and sort algorithm are centralized versions that apply to any container. Additionally, the algorithms allow you to specify a sub-range of the container on which to apply the algorithm. So in order to find an element in a list, you would do this:

`list<int>::iterator i = find(list.begin(), list.end(), 3);`

And in order to sort a vector, you would do this:

```cpp
quick_sort(v.begin(), v.end());   // Sort the entire array.

quick_sort(&v[3], &v[8]);         // Sort the items at the indexes in the range of [3, 8).
```

In the case of functionality that can be had by using other member functions, note that EASTL follows the philosophy that duplicated functionality should not exist in a container, with exceptions being made for cases where mistakes and unsafe practices commonly happen if the given function isn't present. In the case of string not having a mid function, this is because there is a string constructor that takes a sub-range of another string. So to make a string out of the middle of another, you would do this:

`string strMid(str, 3, 5); // Make a new string of the characters from the source range of [3, 3+5).`

It might be noted that the EASTL string class is unique among EASTL containers in that it sometimes violates the minimum functionality rule. This is so because the std C++ string class similarly does so and EASTL aims to be compatible.

### Cont.13 How do I search a hash_map of strings via a char pointer efficiently? If I use map.find("hello") it creates a temporary string, which is inefficient.

The problem is illustrated with this example:

```cpp
map<string, Widget> swMap;
  ...
map<string, Widget>::iterator it = swMap.find("blue"); // A temporary string object is created here.
```

In this example, the find function expects a string object and not a string literal and so (silently!) creates a temporary string object for the duration of the find. There are two solutions to this problem:

* Make the map a map of char pointers instead of string objects. Don't forget to write a custom compare or else the default comparison function will compare pointer values instead of string contents.
* Use the EASTL hash_map::find_as function, which allows you to find an item in a hash container via an alternative key than the one the hash table uses.

### Cont.14 Why are set and hash_set iterators const (i.e. const_iterator)?

The situation is illustrated with this example:

```cpp
set<int> intSet;

intSet.insert(1);
set<int>::iterator i = intSet.begin();
*i = 2; // Error: iterator i is const.
```

In this example, the iterator is a regular iterator and not a const_iterator, yet the compiler gives an error when trying to change the iterator value. The reason this is so is that a set is an ordered container and changing the value would make it out of order. Thus, set and multiset iterators are always const_iterators. If you need to change the value and are sure the change will not alter the container order, use const_cast or declare mutable member variables for your contained object. This resolution is the one blessed by the C++ standardization committee.

### Cont.15 How do I prevent my hash container from re-hashing?

If you want to make a hashtable never re-hash (i.e. increase/reallocate its bucket count), call set_max_load_factor with a very high value such as 100000.f.

Similarly, you can control the bucket growth factor with the rehash_policy function. By default, when buckets reallocate, they reallocate to about twice their previous count. You can control that value as with the example code here:

```cpp
hash_set<int> hashSet;
hashSet.rehash_policy().mfGrowthFactor = 1.5f
```

### Cont.16 Which uses less memory, a map or a hash_map?

A hash_map will virtually always use less memory. A hash_map will use an average of two pointers per stored element, while a map uses three pointers per stored element.

### Cont.17 How do I write a custom hash function?

You can look at the existing hash functions in functional.h, but we provide a couple examples here.

To write a specific hash function for a Widget class, you would do this:

```cpp
struct WidgetHash {
    size_t operator()(const Widget& w) const
        { return w.id; }
};

hash_set<Widget, WidgetHash> widgetHashSet;
```

To write a generic (templated) hash function for a set of similar classes (in this case that have an id member), you would do this:

```cpp
template <typename T>
struct GeneralHash {
    size_t operator()(const T& t) const
        { return t.id; }
};

hash_set<Widget, GeneralHash<Widget> > widgetHashSet;
hash_set<Dogget, GeneralHash<Dogget> > doggetHashSet;
```

### Cont.18 How do I write a custom compare function for a map or set?

The sorted containers require that an operator< exist for the stored values or that the user provide a suitable custom comparison function. A custom can be implemented like so:

```cpp
struct WidgetLess {
    bool operator()(const Widget& w1, const Widget& w2) const
        { return w.id < w2.id; }
};

set<Widget, WidgetLess> wSet;
```

It's important that your comparison function must be consistent in its behaviour, else the container will either be unsorted or a crash will occur. This concept is called "strict weak ordering."

### Cont.19 How do I force my vector or string capacity down to the size of the container?

You can simply use the set_capacity() member function which is present in both vector and string. This is a function that is not present in std STL vector and string functions.

```cpp
eastl::vector<Widget> x;
x.set_capacity();   // Shrink x's capacity to be equal to its size.

eastl::vector<Widget> x;
x.set_capacity(0);  // Completely clear x.
```

To compact your vector or string in a way that would also work with std STL you need to do the following.

How to shrink a vector's capacity to be equal to its size:

```cpp
std::vector<Widget> x;
std::vector<Widget>(x).swap(x); // Shrink x's capacity.
```

How to completely clear a std::vector (size = 0, capacity = 0, no allocation):

```cpp
std::vector<Widget> x;
std::vector<Widget>().swap(x); // Completely clear x.
```

### Cont.20 How do I iterate a container while (selectively) removing items from it?

All EASTL containers have an erase function which takes an iterator as an argument and returns an iterator to the next item. Thus, you can erase items from a container while iterating it like so:

```cpp
set<int> intSet;

set<int>::iterator i = intSet.begin();

while(i != intSet.end())
{
    if(*i & 1)  // Erase all odd integers from the container.
        i = intSet.erase(i);
    else
        ++i;
}
```

### Cont.21 How do I store a pointer in a container?

The problem with storing pointers in containers is that clearing the container will not free the pointers automatically. There are two conventional resolutions to this problem:

Manually free pointers when removing them from containers.
Store the pointer as a smart pointer instead of a "raw"pointer.
The advantage of the former is that it makes the user's intent obvious and prevents the possibility of smart pointer "thrashing" with some containers. The disadvantage of the former is that it is more tedicous and error-prone.

The advantage of the latter is that your code will be cleaner and will always be error-free. The disadvantage is that it is perhaps slightly obfuscating and with some uses of some containers it can cause smart pointer thrashing, whereby a resize of a linear container (e.g. vector) can cause shared pointers to be repeatedly incremented and decremented with no net effect.

It's important that you use a shared smart pointer and not an unshared one such as C++ auto_ptr, as the latter will result in crashes upon linear container resizes. Here we provide an example of how to create a list of smart pointers:

```cpp
list< shared_ptr<Widget> > wList;

wList.push_back(shared_ptr<Widget>(new Widget));
wList.pop_back(); // The Widget will be freed.
```

### Cont.22 How do I make a union of two containers? difference? intersection?

The best way to accomplish this is to sort your container (or use a sorted container such as set) and then apply the set_union, set_difference, or set_intersection algorithms.

### Cont.23 How do I override the default global allocator?

There are multiple ways to accomplish this. The allocation mechanism is defined in EASTL/internal/config.h and in allocator.h/cpp. Overriding the default global allocator means overriding these files, overriding what these files refer to, or changing these files outright. Here is a list of things you can do, starting with the simplest:

* Simply provide the following versions of operator new (which EASTL requires, actually):
```cpp
void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line);
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line);
```
* Predefine the config.h macros for EASTLAlloc, EASTLFree, etc. See config.h for this.
* Override config.h entirely via EASTL_USER_CONFIG_HEADER. See config.h for this.
* Provide your own version of allocator.h/cpp
* Provide your own version of config.h.

If you redefine the allocator class, you can make it work however you want.

Note that config.h defines EASTLAllocatorDefault, which returns the default allocator instance. As documented in config.h, this is not a global allocator which implements all container allocations but is the allocator that is used when EASTL needs to allocate memory internally. There are very few cases where EASTL allocates memory internally, and in each of these it is for a sensible reason that is documented to behave as such.

### Cont.24 How do I do trick X with the string container?

There seem to be many things users want to do with strings. Perhaps the most commonly requested EASTL container extensions are string class shortcut functions. While some of these requests are being considered, we provide some shortcut functions here.

**find_and_replace**

```cpp
template <typename String>
void find_and_replace(String& s, const typename String::value_type* pFind, const typename String::value_type* pReplace)    
{
    for(size_t i; (i = source.find(pFind)) != T::npos; )
        s.replace(i, eastl::CharStrlen(pFind), pReplace);
}

Example:
    find_and_replace(s, "hello", "hola");
```

**trim front (multiple chars)**

```cpp
template <typename String>
void trim_front(String& s, const typename String::value_type* pValues)
{
    s.erase(0, s.find_first_not_of(pValues));
}

Example:
    trim_front(s, " \t\n\r");
```

**trim back (multiple chars)**

```cpp
template <typename String>
void trim_front(String& s, const typename String::value_type* pValues)
{
    s.resize(s.find_last_not_of(pValues) + 1);
}

Example:
    trim_back(s, " \t\n\r");
```

**prepend**

```cpp
template <typename String>
void prepend(String& s, const typename String::value_type* p)
{
    s.insert(0, p);
}

Example:
    prepend(s, "log: ");
```

**begins_with**

```cpp
template <typename String>
bool begins_with(const String& s, const typename String::value_type* p)
{
    return s.compare(0, eastl::CharStrlen(p), p) == 0;
}

Example:
    if(begins_with(s, "log: ")) ...
```

**ends_with**

```cpp
template <typename String>
bool ends_with(const String& s, const typename String::value_type* p)
{
    const typename String::size_type n1 = s.size();
    const typename String::size_type n2 = eastl::CharStrlen(p);
    return ((n1 >= n2) && s.compare(n1 - n2, n2, p) == 0);
}

Example:
    if(ends_with(s, "test.")) ...
```

**tokenize**

Here is a simple tokenization function that acts very much like the C strtok function.

```cpp
template <typename String>
size_t tokenize(const String& s, const typename String::value_type* pDelimiters,
                String* resultArray, size_t resultArraySize)
{
    size_t n = 0;
    typename String::size_type lastPos = s.find_first_not_of(pDelimiters, 0);
    typename String::size_type pos     = s.find_first_of(pDelimiters, lastPos);

    while((n < resultArraySize) && (pos != String::npos) || (lastPos != String::npos))
    {
        resultArray[n++].assign(s, lastPos, pos - lastPos);
        lastPos = s.find_first_not_of(pDelimiters, pos);
        pos     = s.find_first_of(pDelimiters, lastPos);
    }

    return n;
}

Example:
   string resultArray[32];
tokenize(s, " \t", resultArray, 32));
```

### Cont.25 How do EASTL smart pointers compare to Boost smart pointers?

EASTL's smart pointers are nearly identical to Boost (including all that crazy member template and dynamic cast functionality in shared_ptr), but are not using the Boost source code. EA legal has already stated that it is fine to have smart pointer classes with the same names and functionality as those present in Boost. EA legal specifically looked at the smart pointer classes in EASTL for this. There are two differences between EASTL smart pointers and Boost smart pointers:

* EASTL smart pointers don't have thread safety built-in. It was deemed that this is too much overhead and that thread safety is something best done at a higher level. By coincidence the C++ library proposal to add shared_ptr also omits the thread safety feature. FWIW, I put a thread-safe shared_ptr in EAThread, though it doesn't attempt to do all the fancy member template things that Boost shared_ptr does. Maybe I'll add that some day if people care.
* EASTL shared_ptr object deletion goes through a deletion object instead of through a virtual function interface. 95% of the time this makes no difference (aside from being more efficient), but the primary case where it matters is when you have shared_ptr<void> and assign to is something like "new Widget". The problem is that shared_ptr<void> doesn't know what destructor to call and so doesn't call a destructor unless you specify a custom destructor object as part of the template specification. I don't know what to say about this one, as it is less safe, but forcing everybody to have the overhead of additional templated classes and virtual destruction functions doesn't seem to be in the spirit of high performance or lean game development.

There is the possibility of making a shared_ptr_boost which is completely identical to Boost shared_ptr. So perhaps that will be done some day.

### Cont.26 How do your forward-declare an EASTL container?

Here is are some examples of how to do this:

```cpp
namespace eastl
{
    template <typename T, typename Allocator> class basic_string;
    typedef basic_string<char, allocator> string8;   // Forward declare EASTL's string8 type.

    template <typename T, typename Allocator> class vector;
    typedef vector<char, allocator> CharArray;

    template <typename Value, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode> class hash_set;

    template <typename Key, typename T, typename Compare, typename Allocator> class map;
}
```

The forward declaration can be used to declare a pointer or reference to such a class. It cannot be used to declare an instance of a class or refer to class data, static or otherwise. Nevertheless, forward declarations for pointers and references are useful for reducing the number of header files a header file needs to include.

### Cont.27 How do I make two containers share a memory pool?

EASTL (and std STL) allocators are specified by value semantics and not reference semantics. Value semantics is more powerful (because a value can also be a reference, but not the other way around), but is not always what people expects if they're used to writing things the other way.

Here is some example code:

```cpp
struct fixed_pool_reference
{
public:
    fixed_pool_reference()
    {
        mpFixedPool = NULL;
    }

    fixed_pool_reference(eastl::fixed_pool& fixedPool)
    {
        mpFixedPool = &fixedPool;
    }

    fixed_pool_reference(const fixed_pool_reference& x)
    {
        mpFixedPool = x.mpFixedPool;
    }

    fixed_pool_reference& operator=(const fixed_pool_reference& x)
    {
        mpFixedPool = x.mpFixedPool;
        return *this;
    }

    void* allocate(size_t /*n*/, int /*flags*/ = 0)
    {
        return mpFixedPool->allocate();
    }

    void* allocate(size_t /*n*/, size_t /*alignment*/, size_t /*offset*/, int /*flags*/ = 0)
    {
        return mpFixedPool->allocate();
    }

    void deallocate(void* p, size_t /*n*/)
    {
        return mpFixedPool->deallocate(p);
    }

    const char* get_name() const
    {
        return "fixed_pool_reference";
    }

    void set_name(const char* /*pName*/)
    {
    }

protected:
    friend bool operator==(const fixed_pool_reference& a, const fixed_pool_reference& b);
    friend bool operator!=(const fixed_pool_reference& a, const fixed_pool_reference& b);

    eastl::fixed_pool* mpFixedPool;
};

inline bool operator==(const fixed_pool_reference& a, const fixed_pool_reference& b)
{
    return (a.mpFixedPool == b.mpFixedPool);
}

inline bool operator!=(const fixed_pool_reference& a, const fixed_pool_reference& b)
{
    return (a.mpFixedPool != b.mpFixedPool);
}
```

Example usage of the above:

```cpp
typedef eastl::list<int, fixed_pool_reference> IntList;

IntList::node_type buffer[2];
eastl::fixed_pool  myPool(buffer, sizeof(buffer), sizeof(Int::node_type), 2);

IntList myList1(myPool);
IntList myList2(myPool);

myList1.push_back(37);
myList2.push_back(39);
```

### Cont.28 Can I use a std (STL) allocator with EASTL?

No. EASTL allocators are similar in interface to std STL allocators, but not 100% compatible. If it was possible to make them compatible with std STL allocators but also match the design of EASTL then compatibility would exist. The primary reasons for lack of compatibility are:

* EASTL allocators have a different allocate function signature.
* EASTL allocators have as many as four extra required functions: ctor(name), get_name(), set_name(), allocate(size, align, offset).
* EASTL allocators have an additional allocate function specifically for aligned allocations, as listed directly above.

### What are the requirements of classes stored in containers?

Class types stored in containers must have:

* a public copy constructor
* a public assignment operator
* a public destructor
* an operator < that compares two such classes (sorted containers only).
* an operator == that compares two such classes (hash containers only).

Recall that the compiler generates basic versions these functions for you when you don't implement them yourself, so you can omit any of the above if the compiler-generated version is sufficient.

For example, the following code will act incorrectly, because the user forgot to implement an assignment operator. The compiler-generated assignment operator will assign the refCount value, which the user doesn't want, and which will be called by the vector during resizing.

```cpp
struct NotAPod
{
   NotAPod(const NotAPod&) {} // Intentionally don't copy the refCount

   int refCount; // refCounts should not be copied between NotAPod instances.
};

eastl::vector<NotAPod> v;
```

## Algorithms

### Algo.1 I'm getting screwy behavior in sorting algorithms or sorted containers. What's wrong?

It may possible that you are seeing floating point roundoff problems. Many STL algorithms require object comparisons to act consistently. However, floating point values sometimes compare differently between uses because in one situation a value might be in 32 bit form in system memory, whereas in anther situation that value might be in an FPU register with a different precision. These are difficult problems to track down and aren't the fault of EASTL or whatever similar library you might be using. There are various solutions to the problem, but the important thing is to find a way to force the comparisons to be consistent.

The code below was an example of this happening, whereby the object pA->mPos was stored in system memory while pB->mPos was stored in a register and comparisons were inconsistent and a crash ensued.

```cpp
class SortByDistance : public binary_function<WorldTreeObject*, WorldTreeObject*, bool>
{
private:
    Vector3 mOrigin;

public:
    SortByDistance(Vector3 origin) {
        mOrigin = origin;
    }

    bool operator()(WorldTreeObject* pA, WorldTreeObject* pB) const {
        return ((WorldObject*)pA)->mPos - mOrigin).GetLength()
             < ((WorldObject*)pB)->mPos - mOrigin).GetLength();
    }
};
```

### Algo.2 How do I write a comparison (operator<()) for a struct that contains two or more members?

For a struct with two members such as the following:

```cpp
struct X {
    Blah m1;
    Blah m2;
};
```

You would write the comparison function like this:

```cpp
bool operator<(const X& a, const X& b) {
    return (a.m1 == b.m1) ? (a.m2 < b.m2) : (a.m1 < b.m1);
}
```

or, using only operator < but more instructions:

```cpp
bool operator<(const X& a, const X& b) {
    return (a.m1 < b.m1) || (!(b.m1 < a.m1) && (a.m2 < b.m2));
}
```

For a struct with three members, you would have:

```cpp
bool operator<(const X& a, const X& b) {
    if(a.m1 != b.m1)
        return (a.m1 < b.m1);
    if(a.m2 != b.m2)
        return (a.m2 < b.m2);
    return (a.mType < b.mType);
}
```

And a somewhat messy implementation if you wanted to use only operator <.

Note also that you can use the above technique to implement operator < for spatial types such as vectors, points, and rectangles. You would simply treat the members of the stuct as an array of values and ignore the fact that they have spatial meaning. All operator < cares about is that things order consistently.

```cpp
bool operator<(const Point2D& a, const Point2D& b) {
    return (a.x == b.x) ? (a.y < b.y) : (a.x < b.x);
}
```

### Algo.3 How do I sort something in reverse order?

Normally sorting puts the lowest value items first in the sorted range. You can change this by simply reversing the comparison. For example:

`sort(intVector.begin(), intVector.end(), greater<int>());`

It's important that you use operator > instead of >=. The comparison function must return false for every case where values are equal.

### Algo.4 I'm getting errors about min and max while compiling.

You need to define NOMINMAX under VC++ when this occurs, as it otherwise defines min and max macros that interfere. There may be equivalent issues with other compilers. Also, VC++ has a specific <minmax.h> header file which defines min and max macros but which doesn't pay attention to NOMINMAX and so in that case there is nothing to do but not include that file or to undefine min and max. minmax.h is not a standard file and its min and max macros are not standard C or C++ macros or functions.

### Algo.5 Why don't algorithms take a container as an argument instead of iterators? A container would be more convenient.

Having algorithms that use containers instead of algorithms would reduce reduce functionality with no increase in performance. This is because the use of iterators allows for the application of algorithms to sub-ranges of containers and allows for the application of algorithms to containers aren't formal C++ objects, such as C-style arrays.

Providing additional algorithms that use containers would introduce redundancy with respect to the existing algorithms that use iterators.

### Algo.6 Given a container of pointers, how do I find an element by value (instead of by pointer)?

Functions such as find_if help you find a T element in a container of Ts. But if you have a container of pointers such as vector<Widget*>, these functions will enable you to find an element that matches a given Widget* pointer, but they don't let you find an element that matches a given Widget object.

You can write your own iterating 'for' loop and compare values, or you can use a generic function object to do the work if this is a common task:

```cpp
template<typename T>
struct dereferenced_equal
{
    const T& mValue;

    dereferenced_equal(const T& value) : mValue(value) { }     
    bool operator==(const T* pValue) const { return *pValue == mValue; }
};

...

find_if(container.begin(), container.end(), dereferenced_equal<Widget>(someWidget));
```

### Algo.7 When do stored objects need to support operator < vs. when do they need to support operator ==?

Any object which is sorted needs to have operator < defined for it, implicitly via operator < or explicitly via a user-supplied Compare function. Sets and map containers require operator <, while sort, binary search, and min/max algorithms require operator <.

Any object which is compareed for equality needs to have operator == defined for it, implicitly via operator == or explicitly via a user-supplied BinaryPredicate function. Hash containers required operator ==, while many of the algorithms other than those mentioned above for operator < require operator ==.

Some algorithms and containers require neither < nor ==. Interestingly, no algorithm or container requires both < and ==.

### Algo.8 How do I sort via pointers or array indexes instead of objects directly?

Pointers

```cpp
vector<TestObject>  toArray;
vector<TestObject*> topArray;

for(eastl_size_t i = 0; i < 32; i++)
   toArray.push_back(TestObject(rng.RandLimit(20)));
for(eastl_size_t i = 0; i < 32; i++) // This needs to be a second loop because the addresses might change in the first loop due to container resizing.
   topArray.push_back(&toArray[i]);

struct TestObjectPtrCompare
{
    bool operator()(TestObject* a, TestObject* b)
        { return a->mX < a->mX; }
};

quick_sort(topArray.begin(), topArray.end(), TestObjectPtrCompare());
```

Array indexes

```cpp
vector<TestObject>   toArray;
vector<eastl_size_t> toiArray;

for(eastl_size_t i = 0; i < 32; i++)
{
    toArray.push_back(TestObject(rng.RandLimit(20)));
    toiArray.push_back(i);
}

struct TestObjectIndexCompare
{
    vector* mpArray;

    TestObjectIndexCompare(vector<TestObject>* pArray) : mpArray(pArray) { }
    TestObjectIndexCompare(const TestObjectIndexCompare& x) : mpArray(x.mpArray){ }
    TestObjectIndexCompare& operator=(const TestObjectIndexCompare& x) { mpArray = x.mpArray; return *this; }

    bool operator()(eastl_size_t a, eastl_size_t b)
       { return (*mpArray)[a] < (*mpArray)[b]; }
};

quick_sort(toiArray.begin(), toiArray.end(), TestObjectIndexCompare(&toArray));
```

Array indexes (simpler version using toArray as a global variable)

```cpp
vector<TestObject>   toArray;
vector<eastl_size_t> toiArray;

for(eastl_size_t i = 0; i < 32; i++)
{
    toArray.push_back(TestObject(rng.RandLimit(20)));
    toiArray.push_back(i);
}

struct TestObjectIndexCompare
{
    bool operator()(eastl_size_t a, eastl_size_t b)
       { return toArray[a] < toArray[b]; }
};

quick_sort(toiArray.begin(), toiArray.end(), TestObjectIndexCompare(&toArray));
```

## Iterators

### Iter.1 What's the difference between iterator, const iterator, and const_iterator?

An iterator can be modified and item it points to can be modified.
A const iterator cannot be modified, but the items it points to can be modified.
A const_iterator can be modified, but the items it points to cannot be modified.
A const const_iterator cannot be modified, nor can the items it points to.

This situation is much like with char pointers:

| Iterator type | Pointer equivalent |
|------|------|
| iterator | char* |
| const iterator | char* const |
| const_iterator | const char* |
| const const_iterator | const char* const |

### Iter.2 How do I tell from an iterator what type of thing it is iterating?

Use the value_type typedef from iterator_traits, as in this example

```cpp
template <typename Iterator>
void DoSomething(Iterator first, Iterator last)
{
    typedef typename iterator_traits<Iterator>::value_type;

    // use value_type
}
```

### Iter.3 How do I iterate a container while (selectively) removing items from it?

All EASTL containers have an erase function which takes an iterator as an argument and returns an iterator to the next item. Thus, you can erase items from a container while iterating it like so:

```cpp
set<int> intSet;
set<int>::iterator i = intSet.begin();

while(i != intSet.end())
{
    if(*i & 1) // Erase all odd integers from the container.
        i = intSet.erase(i);
    else
        ++i;
}
```

### Iter.4 What is an insert_iterator?

An insert_iterator is a utility class which is like an iterator except that when you assign a value to it, the insert_iterator inserts the value into the container (via insert()) and increments the iterator. Similarly, there are front_insert_iterator and back_insert_iterator, which are similar to insert_iterator except that assigning a value to them causes then to call push_front and push_back, respectively, on the container. These utilities may seem a slightly abstract, but they have uses in generic programming.

----------------------------------------------
End of document
