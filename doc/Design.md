# EASTL Design

## Introduction

EASTL (EA Standard Template Library) is designed to be a template library which encompasses and extends the functionality of standard C++ STL while improving it in various ways useful to game development. Much of EASTL's design is identical to standard STL, as the large majority of the STL is well-designed for many uses. The primary areas where EASTL deviates from standard STL implementations are essentially the following:

*   EASTL has a simplified and more flexible custom allocation scheme.
*   EASTL has significantly easier to read code.
*   EASTL has extension containers and algorithms.
*   EASTL has optimizations designed for game development.

Of the above items, the only one which is an incompatible difference with STL is the case of memory allocation. The method for defining a custom allocator for EASTL is slightly different than that of standard STL, though they are 90% similar. The 10% difference, however, is what makes EASTL generally easier and more powerful to work with than standard STL. Containers without custom allocators act identically between EASTL and standard STL.

## Motivations

Our motifications for making EASTL drive the design of EASTL. As identified in the EASTL RFC (Request for Comment), the primary reasons for implementing a custom version of the STL are:

*   <span class="458151900-03082005"><font><font>Some STL implementations (especially Microsoft STL) have inferior performance characteristics that make them unsuitable for game development. EASTL is faster than all existing STL implementations.</font></font></span>
*   The STL is sometimes hard to debug, as most STL implementations use cryptic variable names and unusual data structures.
*   STL allocators are sometimes painful to work with, as they have many requirements and cannot be modified once bound to a container.
*   The STL includes excess functionality that can lead to larger code than desirable. It's not very easy to tell programmers they shouldn't use that functionality.
*   The STL is implemented with very deep function calls. This results is unacceptable performance in non-optimized builds and sometimes in optimized builds as well.
*   The STL doesn't support alignment of contained objects.
*   STL containers won't let you insert an entry into a container without supplying an entry to copy from. This can be inefficient.
*   Useful STL extensions (e.g. slist, hash_map, shared_ptr) found in existing STL implementations such as STLPort are not portable because they don't exist in other versions of STL or aren't consistent between STL versions.  

*   The STL lacks useful extensions that game programmers find useful (e.g. intrusive_list) but which could be best optimized in a portable STL environment.
*   The STL has specifications that limit our ability to use it efficiently. For example, STL vectors are not guaranteed to use contiguous memory and so cannot be safely used as an array.
*   The STL puts an emphasis on correctness before performance, whereas sometimes you can get significant performance gains by making things less academcially pure.
*   STL containers have private implementations that don't allow you to work with their data in a portable way, yet sometimes this is an important thing to be able to do (e.g. node pools).
*   All existing versions of STL allocate memory in empty versions of at least some of their containers. This is not ideal and prevents optimizations such as container memory resets that can greatly increase performance in some situations.
*   The STL is slow to compile, as most modern STL implementations are very large.  

*   There are legal issues that make it hard for us to freely use portable STL implementations such as STLPort.
*   We have no say in the design and implementation of the STL and so are unable to change it to work for our needs.

## Prime Directives

The implementation of EASTL is guided foremost by the following directives which are listed in order of importance.

1.  Efficiency (speed and memory usage)
2.  Correctness
3.  Portability
4.  Readability

Note that unlike commercial STL implementations which must put correctness above all, we put a higher value on efficiency. As a result, some functionality may have some usage limitation that is not present in other similar systems but which allows for more efficient operation, especially on the platforms of significance to us.

Portability is significant, but not critical. Yes, EASTL must compile and run on all platforms that we will ship games for. But we don't take that to mean under all compilers that could be conceivably used for such platforms. For example, Microsoft VC6 can be used to compile Windows programs, but VC6's C++ support is too weak for EASTL and so you simply cannot use EASTL under VC6.

Readability is something that EASTL achieves better than many other templated libraries, particularly Microsoft STL and STLPort. We make every attempt to make EASTL code clean and sensible. Sometimes our need to provide optimizations (particularly related to type_traits and iterator types) results in less simple code, but efficiency happens to be our prime directive and so it overrides all other considerations.

## Thread Safety

It's not simple enough to simply say that EASTL is thread-safe or thread-unsafe. However, we can say that with respect to thread safety that EASTL does the right thing.

Individual EASTL containers are not thread-safe. That is, access to an instance of a container from multiple threads at the same time is unsafe if any of those accesses are modifying operations. A given container can be read from multiple threads simultaneously as well as any other standalone data structure. If a user wants to be able to have modifying access an instance of a container from multiple threads, it is up to the user to ensure that proper thread synchronization occurs. This usually means using a mutex.

EASTL classes other than containers are the same as containers with respect to thread safety. EASTL functions (e.g. algorithms) are inherently thread-safe as they have no instance data and operate entirely on the stack. As of this writing, no EASTL function allocates memory and thus doesn't bring thread safety issues via that means.

The user may well need to be concerned about thread safety with respect to memory allocation. If the user modifies containers from multiple threads, then allocators are going to be accessed from multiple threads. If an allocator is shared across multiple container instances (of the same type of container or not), then mutexes (as discussed above) the user uses to protect access to indivudual instances will not suffice to provide thread safety for allocators used across multiple instances. The conventional solution here is to use a mutex within the allocator if it is exected to be used by multiple threads.

EASTL uses neither static nor global variables and thus there are no inter-instance dependencies that would make thread safety difficult for the user to implement.

## Container Design

All EASTL containers follow a set of consistent conventions. Here we define the prototypical container which has the minimal functionality that all (non-adapter) containers must have. Some containers (e.g. stack) are explicitly adapter containers and thus wrap or inherit the properties of the wrapped container in a way that is implementation specific.  

```cpp
template <class T, class Allocator =
EASTLAllocator>

class container

{

public:

    typedef container<T, Allocator>            this_type;

    typedef
T                     
            value_type;

    typedef T*                   
             pointer;

    typedef const T*                       
   const_pointer;

    typedef
T&                                 reference;


    typedef const
T&                           const_reference;


    typedef
ptrdiff_t                          difference_type;


    typedef
impl_defined                       size_type;


    typedef impl-defined                  
    iterator;

    typedef impl-defined                  
    const_iterator;

    typedef reverse_iterator<iterator>         reverse_iterator;

    typedef reverse_iterator<const_iterator>   reverse_const_iterator;

    typedef Allocator                 
        allocator_type;



public:

    container(const
allocator_type& allocator = allocator_type());

    container(const this_type&
x);



    this_type&
operator=(this_type& x);

    void swap(this_type& x);

    void reset();



    allocator_type& get_allocator();

    void            set_allocator(allocator_type& allocator);



    iterator       begin();

    const_iterator begin() const;

    iterator       end();

    const_iterator end() const;



    bool validate() const;
     int  validate_iterator(const_iterator i)
const;


protected:

    allocator_type mAllocator;

};



template <class T, class
Allocator>

bool operator==(const container<T, Allocator>& a, const container<T, Allocator>& b);



template <class T, class
Allocator>

bool operator!=(const container<T, Allocator>& a, const
container<T, Allocator>&
b);
```

Notes:

*   Swapped containers do not swap their allocators.
*   Newly constructed empty containers do no memory allocation. Some STL and other container libraries allocate an initial node from the class memory allocator. EASTL containers by design never do this. If a container needs an initial node, that node should be made part of the container itself or be a static empty node object.
*   Empty containers (new or otherwise) contain no constructed objects, including those that might be in an 'end' node. Similarly, no user object (e.g. of type T) should be constructed unless required by the design and unless documented in the cotainer/algorithm contract. 
*   The reset function is a special extension function which unilaterally resets the container to an empty state without freeing the memory of the contained objects. This is useful for very quickly tearing down a container built into scratch memory. No memory is allocated by reset, and the container has no allocatedmemory after the reset is executed.
*   The validate and validate_iterator functions provide explicit container and iterator validation. EASTL provides an option to do implicit automatic iterator and container validation, but full validation (which can be potentially extensive) has too much of a performance cost to execute implicitly, even in a debug build. So EASTL provides these explicit functions which can be called by the user at the appropriate time and in optimized builds as well as debug builds.

## Allocator Design

The most significant difference between EASTL and standard C++ STL is that standard STL containers are templated on an allocator class with the interface defined in std::allocator. std::allocator is defined in the C++ standard as this:  

```cpp
// Standard C++ allocator



  template <class T>

class allocator

{

public:

    typedef size_t    size_type;

    typedef ptrdiff_t difference_type;

    typedef T*        pointer;

    typedef const T*  const_pointer;

    typedef T&      
 reference;

    typedef const
T&  const_reference;

    typedef T         value_type;



    template <class U>

    struct rebind { typedef allocator<U> other; };



    allocator() throw();

    allocator(const allocator&) throw();

    template <class U>

    allocator(const allocator<U>&) throw();



    ~allocator()
throw();



     pointer  
    address(reference x) const;

    const_pointer address(const_reference x)
const;

    pointer       allocate(size_type, typename
allocator<void>::const_pointer hint = 0);

    void          deallocate(pointer p,
size_type n);

    size_type     max_size() const
throw();

    void          construct(pointer p,
const T& val);

    void          destroy(pointer
p);

};
```

Each STL container needs to have an allocator templated on container type T associated with it. The problem with this is that allocators for containers are defined at the class level and not the instance level. This makes it painful to define custom allocators for containers and adds to code bloat. Also, it turns out that the containers don't actually use allocator<T> but instead use allocator\<T>::rebind\<U>::other. Lastly, you cannot access this allocator after the container is constructed. There are some good academic reasons why the C++ standard works this way, but it results in a lot of unnecessary pain and makes concepts like memory tracking much harder to implement.

What EASTL does is use a more familiar memory allocation pattern whereby there is only one allocator class interface and it is used by all containers. Additionally EASTL containers let you access their allocators and query them, name them, change them, etc.

EASTL has chosen to make allocators not be copied between containers during container swap and assign operations. This means that if container A swaps its contents with container B, both containers retain their original allocators. Similarly, assigning container A to container B causes container B to retain its original allocator. Containers that are equivalent should report so via operator==; EASTL will do a smart swap if allocators are equal, and a brute-force swap otherwise.  

```cpp
// EASTL allocator  

class allocator  
{  
public:  
    allocator(const char* pName = NULL);  

    void* allocate(size_t n, int flags = 0);  
    void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0);  
    void  deallocate(void* p, size_t n);  

    const char* get_name() const;  
    void        set_name(const char* pName);  
};  

allocator* GetDefaultAllocator();
```

## Fixed Size Container Design

EASTL supplies a set of fixed-size containers that the user can use, though the user can also implement their own versions. So in addition to class list there is class fixed_list. The fixed_list class implements a linked list via a fixed-size pool of contiguous memory which has no space overhead (unlike with a regular heap), doesn't cause fragmentation, and allocates very quickly.

EASTL implements fixed containers via subclasses of regular containers which set the regular container's allocator to point to themselves. Thus the implementation for fixed_list is very tiny and consists of little more than constructor and allocator functions. This design has some advantages but has one small disadvantage. The primary advantages are primarily that code bloat is reduced and that the implementation is simple and the user can easily extend it. The primary disadvantage is that the parent list class ends up with a pointer to itself and thus has 4 bytes that could arguably be saved if system was designed differently. That different design would be to make the list class have a policy template parameter which specifies that it is a fixed pool container. EASTL chose not to follow the policy design because it would complicate the implementation, make it harder for the user to extend the container, and would potentially waste more memory due to code bloat than it would save due to the 4 byte savings it achieves in container instances.

## Algorithm Design

EASTL algorithms very much follow the philosophy of standard C++ algorithms, as this philosophy is sound and efficient. One of the primary aspects of algorithms is that they work on iterators and not containers. You will note for example that the find algorithm takes a first and last iterator as arguments and not a container. This has two primary benefits: it allows the user to specify a subrange of the container to search within and it allows the user to apply the find algorithm to sequences that aren't containers (e.g. a C array).

EASTL algorithms are optimized at least as well as the best STL algorithms found in commercial libraries and are significantly optimized over the algorithms that come with the first-party STLs that come with compilers. Most significantly, EASTL algorithms take advantage of type traits of contained classes and take advantage of iterator types to optimize code generation. For example, if you resize an array of integers (or other "pod" type), EASTL will detect that this can be done with a memcpy instead of a slow object-by-object move as would Micrsoft STL.

The optimizations found in EASTL algorithms and the supporting code in EASTL type traits consists of some fairly tricky advanced C++ and while it is fairly easy to read, it requires a C++ expert (language lawyer, really) to implement confidently. The result of this is that it takes more effort to develop and maintain EASTL than it would to maintain a simpler library. However, the performance advantages have been deemed worth the tradeoff.

## Smart Pointer Design

EASTL implements the following smart pointer types:

*   shared_ptr
*   shared_array
*   weak_ptr
*   instrusive_ptr
*   scoped_ptr
*   scoped_array
*   linked_ptr
*   linked_array

All but linked_ptr/linked_array are well-known smart pointers from the Boost library. The behaviour of these smart pointers is very similar to those from Boost with two exceptions:

*   EASTL smart pointers allow you to assign an allocator to them.
*   EASTL shared_ptr implements deletion via a templated parameter instead of a dynamically allocated virtual member object interface.

With respect to assigning an allocator, this gives EASTL more control over memory allocation and tracking, as Boost smart pointers unilaterally use global operator new to allocate memory from the global heap.

With respect to shared_ptr deletion, EASTL's current design of using a templated parameter is questionable, but does have some reason. The advantage is that EASTL avoids a heap allocation, avoids virtual function calls, and avoids templated class proliferation. The disadvantage is that EASTL shared_ptr containers which hold void pointers can't call the destructors of their contained objects unless the user manually specifies a custom deleter template parameter. This is case whereby EASTL is more efficient but less safe. We can revisit this topic in the future if it becomes an issue.

## list::size is O(n)

As of this writing, EASTL has three linked list classes: list, slist, and intrusive_list. In each of these classes, the size of the list is not cached in a member size variable. The result of this is that getting the size of a list is not a fast operation, as it requires traversing the list and counting the nodes. We could make the list::size function be fast by having a member mSize variable which tracks the size as we insert and delete items. There are reasons for having such functionality and reasons for not having such functionality. We currently choose to not have a member mSize variable as it would add four bytes to the class, add a tiny amount of processing to functions such as insert and erase, and would only serve to improve the size function, but no others. In the case of intrusive_list, it would do additional harm. The alternative argument is that the C++ standard states that std::list should be an O(1) operation (i.e. have a member size variable), that many C++ standard library list implementations do so, that the size is but an integer which is quick to update, and that many users expect to have a fast size function. In the final analysis, we are developing a library for game development and performance is paramount, so we choose to not cache the list size. The user can always implement a size cache himself.

## basic_string doesn't use copy-on-write

The primary benefit of CoW is that it allows for the sharing of string data between two string objects. Thus if you say this:

```cpp
string a("hello");  
string b(a);
```

the "hello" will be shared between a and b. If you then say this:

```cpp
a = "world";
```

then `a` will release its reference to "hello" and leave b with the only reference to it. Normally this functionality is accomplished via reference counting and with atomic operations or mutexes.

The C++ standard does not say anything about basic_string and CoW. However, for a basic_string implementation to be standards-conforming, a number of issues arise which dictate some things about how one would have to implement a CoW string. The discussion of these issues will not be rehashed here, as you can read the references below for better detail than can be provided in the space we have here. However, we can say that the C++ standard is sensible and that anything we try to do here to allow for an efficient CoW implementation would result in a generally unacceptable string interface.

The disadvantages of CoW strings are:

*   A reference count needs to exist with the string, which increases string memory usage.
*   With thread safety, atomic operations and mutex locks are expensive, especially on weaker memory systems such as console gaming platforms.
*   All non-const string accessor functions need to do a sharing check the the first such check needs to detach the string. Similarly, all string assignments need to do a sharing check as well. If you access the string before doing an assignment, the assignment doesn't result in a shared string, because the string has already been detached.
*   String sharing doesn't happen the large majority of the time. In some cases, the total sum of the reference count memory can exceed any memory savings gained by the strings that share representations. 

The addition of a cow_string class is under consideration for EASTL. There are conceivably some systems which have string usage patterns which would benefit from CoW sharing. Such functionality is best saved for a separate string implementation so that the other string uses aren't penalized.

This is a good starting HTML reference on the topic:

> [http://www.gotw.ca/publications/optimizations.htm](http://www.gotw.ca/publications/optimizations.htm)

Here is a well-known Usenet discussion on the topic:

> [http://groups-beta.google.com/group/comp.lang.c++.moderated/browse_thread/thread/3dc6af5198d0bf7/886c8642cb06e03d](http://groups-beta.google.com/group/comp.lang.c++.moderated/browse_thread/thread/3dc6af5198d0bf7/886c8642cb06e03d)

----------------------------------------------
End of document
