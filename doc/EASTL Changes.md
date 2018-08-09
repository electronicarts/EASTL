EASTL Changes
=============
------------------------------------------------------------------------
**Version 3.12.01** (Aug 08, 2018)

Changes
* eastl::pair first element construct required fix due to a Clang compiler bug determine when a class with NSDMI is default constructible.
	* We have submitted a bug with LLVM: https://bugs.llvm.org/show_bug.cgi?id=38374

------------------------------------------------------------------------
**Version 3.12.00** (Jul 06, 2018)

Changes
* eastl::is_final type trait support added.
* eastl::endian support for C++20 conformance.
* eastl::fixed_pool improving swap performance by enabling move semantics.
* eastl::optional bug fixes to properly destroy held objects when assigning another object.
* eastl::string bug fixes for string_view substr ctor overload.

------------------------------------------------------------------------
**Version 3.11.00** (Jul 06, 2018)

Changes

* eastl::function improved standards conformace - C++17 conformant
	* Can now call functions where return type is implicity converitble to the
      return type specified in the function template
	* Implemented missing functions for operator=, such as operator=(Functor&&)
	* operator bool() is now explicit so function cannot be implicit convertible
      to bool
	* SFINAE away the templated construct/operator= so it is not a candidate in
      the overload set
* eastl::function && eastl::fixed_function now share a common implementation
* eastl::invoke_result && eastl::is_invocable are now C++14 compliant
	* can be used in an unevaluated context
	* typename F is no longer required to be a Callable
* Added EASTL_CPP17_INLINE_VARIABLE macro to declare a variable as inline
* eastl::pair first element constructor overload can not be toggled to give users the opportunity to fix their code before the overloads are removed in a future release.
	* Look at the feature define: EASTL_ENABLE_PAIR_FIRST_ELEMENT_CONSTRUCTOR
* eastl::string_map and eastl::string_hash_map optimizations to avoid implicit eastl::string conversions.
* eastl::ring_buffer fixes when using an underlying container with a large stack memory requirement that it allocates the required heap allocation from the containers allocator -- not the default EASTL allocator instance.

------------------------------------------------------------------------
**Version 3.10.00** (June 26, 2018)

Changes

* eastl::variant get_if bug fix for requesting the incorrect held type or when passing a nullptr.
* eastl::string substr(), left(), right() return a copy of this->get_allocator()
* eastl::string changed SSOSize variable name from mnSize to mnRemainingSize to
  better signify what type of data the variable holds
* Added iterator vector::erase_first_unsorted(const T& val) and the reverse equivalent, iterator
  vector::erase_last_unsorted(const T& val). These allow for erasing the first found value matching element, as a
  convenience function. The container is considered unsorted after the operation.
* Implemented for_each_n
	* http://en.cppreference.com/w/cpp/algorithm/for_each_n
* Implemented string_view::starts_with, string_view_ends_with.
* Implemented eastl::basic_string constructor and assignment operator overloads for eastl::basic_string_view.
* Resolved a bug in the uint8_t specialization of radix_sort_impl that was detected by adding verification to a sorting test.
* eastl::fixed_function fixes to ensure closure destructors run correctly.
* eastl::optional copy-construction fixes for complex types.
* eastl::optional move-only type fixes.
* eastl::span implementation for C++20.

------------------------------------------------------------------------
**Version 3.09.01** (Apr 03, 2018)

Changes

* BITSET_WORD_COUNT bug fix for incorrect macro parameters.
* eastl::has_unique_object_representations implementation
* eastl::is_integral_v implementation & fixes for char16_t and char32_t types.
* eastl::make_optional compiler error bug fix when passing l-values.
* eastl::make_optional implemented C++17 std::initializer_list and constructor argument overloads.
* eastl::optional C++17 additional conversion constructors.
* eastl::nullopt_t bug fix for 'int' conversion ctor which enabled really bad implicit conversions.  Migrated to a custom tag type to prevent any implicit conversions from user code.
* eastl::remove_pointer_t implemented. 
* Resolved compiler error in eastl::optional when using non-trivially destructible types

------------------------------------------------------------------------
**Version 3.09.00** (Mar 29, 2018)

Changes

* eastl::any fix to correctly decay reference types.
* eastl::fixed\_hash\_map/eastl::fixed\_hash\_multimap fixes when a user
 requests to clear the bucket array hashtable rehashing was disabled with
 a default bucket count of 1. The behaviour now for fixed\_hash\_map (and
 fixed\_hash\_multimap) is the clear buckets request resets the
 containter to use the SBO and the default bucket count.
* Added 'eastl::is\_literal\_type\_v' type trait.
* Added 'eastl::is\_convertible\_v' type trait.
* eastl::string SSO capacity is now optimal
	* HeapLayout now uses 1 pointer and 2 size\_t instead of 3 pointers SSO rework to now use the maximum number of bytes possible, 23 bytes of SSO on 64bit, was 14 bytes before.
	* set\_capacity() will now shrink to SSO if the new capacity is within SSO limits
	* shrink\_to\_fit() implemented
	* basic\_string(const basic\_string&, const Allocator&) constructor implemented
	* substr(), left(), right() now properly return a basic\_string with a
	* default constructed allocator
	* when inserting yourself into yourself reduced allocations from 1 to 0 or 1.
	* is\_sso() function implemented that returns true if the current string is an SSO string
	* natvis updated
* eastl::fixed\_substring && eastl::fixed\_string updated due to
* eastl::string changes
* eastl::vector::emplace\_back() now returns a reference to the emplaced element 
* eastl::variant::operator=(T) conversion assignment operator fixed an issue due to a incorrect conjunction expression that caused the operator to be removed via SFINAE.
* eastl::intrusive\_ptr now supports move semantics.
* eastl::is\_trivially\_copyable fixes to use the proper compiler intrinsics.
* eastl::deque::shrink\_to\_fit() implemented.


------------------------------------------------------------------------
**Version 3.08.00** (Mar 16, 2018)

Changes

-   Fully qualifying call to move in algorithms "remove\_copy\_if".
-   Implemented the following new C++17 memory algorithms:
    -   uninitialized\_default\_construct
    -   uninitialized\_default\_construct\_n
    -   uninitialized\_value\_construct
    -   uninitialized\_value\_construct\_n
-   Implemented the eastl::remove\_cvref type trait.
-   eastl::string\_view hash fixes for both char and wchar types
-   eastl::optional compiler fixes when building with exceptions enabled
-   Implemented eastl::unordered\_map/map/hash\_map \"try\_emplace\" and
    \"insert\_or\_assign\".
-   Removing deprecated \"reset\" member function from containers.
-   string\_view string compare fixes.
-   eastl::optional storage now type aligned.
-   eastl::string\_view no longer crashes when passed a nullptr via the
    \'const char\*\' conversion constructor.
-   Added static\_asserts to eastl::variant visit function to improve
    diagonistic message when variant instances are not provided.
-   Added noexcept decorations to eastl::vector and eastl::string.
-   eastl::any::swap fixes to ensure destructors are being called
    correctly.
-   -   eastl::rbtree and eastl::hashtable insert and emplace
    optimizations to reduce the allocation overhead for object that act
    as a handle to a heap allocation (ie. eastl::string). Breaking
    change: rbtree/hashtable backed containers now correct forbid
    inserting pointers with that lose cv-qualifers.
-   Adding eastl::string\_view visualizer to EASTL natvis.


------------------------------------------------------------------------
**Version 3.07.02** (Jan 17, 2018)

Changes

-   Implemented eastl::fixed\_function.
-   Implemented eastl::apply for unpacking tuples into callable
    arguments.
-   Implemented eastl::piecewise\_construct constructor overload for
    eastl::pair.
-   Implemented eastl::reference\_wrapper.
-   Implemented eastl::invoke for pointer-to-member,
    pointer-to-member-function and eastl::reference\_wrapper types.
-   Implemented vector::insert updates that return an interator to the
    beginning of the inserted range.
-   Implemented eastl::tuple\_size\_v alias.
-   Removed non-variadic template code paths in eastl::function.
-   Implemented eastl::is\_invocable.
-   Fixed eastl::common\_type\<void, void\> and updated
    eastl::common\_type to decay result type based on defect \#2141
-   Implemented eastl::variant.


------------------------------------------------------------------------
**Version 3.07.01** (November 11, 2017)

Changes

-   Fixed GetTicks for the NX platform.
-   Fixed issue with SSOString unit test.


------------------------------------------------------------------------
**Version 3.07.00** (November 11, 2017)

Changes

-   Improved eastl::hashtable related hashtable::DoInsertValue SFINAE
    mechanism.
-   Improved move-only type support in eastl::remove / eastl::remove\_if
    algorithms.
-   Deprecated eastl::string::reset\_lose\_memory due to correctness
    issues when used with SSO.
-   Implemented east::string::detach as a replacement for
    reset\_lost\_memory so can handle the SSO case.
-   Removed eastl::abstract\_string and eastl::abstract\_fixed\_string.
    eastl::string\_view is the replacement for a string object not
    templated on an allocator type.
-   Removed EmptyString globally exported symbols defined in the
    string.h file.

------------------------------------------------------------------------
**Version 3.06.00** (Oct 05, 2017)

Changes

-   Added SSO or the \"short string optimization\" support in
    eastl::string.
-   Fixed vector::erase(first, last) to check for empty range to ensure
    it is a noop (behaviour is mandated by the standard).
-   Fixed type\_traits header to improve compatibility with Microsoft
    SAL (Source Annotation Langage).
-   Added support for eastl::string to implicitly convert to
    eastl::string\_view.
-   Avoided macro expansion of \'max()\' in string\_view.h in the
    case where a previously-included file defined it.

------------------------------------------------------------------------
**Version 3.05.09** (September 28th, 2017)

Changes

-   Fixed an issue with \"fixed\_allocator\_with\_overflow\" causing all
    allocation requests to be serviced by the overflow allocator instead
    of using the fixed buffer until its exhausted.
-   Improving eastl::sort support for move-only types.
-   Removing Xenon code path for internal mutex implementation.
-   Fixed a compiler error when the value of an eastl::optional was
    retrieved as an rvalue reference.
-   Fixed EASTL.natvis bug where the wrong type was being used to
    surface the contents of an rbtree\_node.

------------------------------------------------------------------------
**Version 3.05.08** (Aug 14, 2017)

Changes

-   Adding natvis visualizer for eastl::ring\_buffer.
-   Updated natvis visualizer for eastl::string and eastl::vector.
-   Improved eastl::vector by guarding against user types with
    overloaded address-of operators. This was observed in
    \"eastl::vector\>\" usage.
-   Adding support logical operations conjuction, disjunction, and
    negation type traits for C++17 conformance.
-   Implemented find\_as(T, Predicate) for the following vector based
    containers:
    -   vector\_set
    -   vector\_multiset
    -   vector\_map
    -   vector\_multimap
-   Added eastl::not\_fn for C++17 conformance.

------------------------------------------------------------------------
**Version 3.05.07** (Jun 19, 2017)

Changes

-   Adding ARM64 processor support.
-   Fixed move copy constructor and move assignment operator for
    fixed\_vector, and added tests.
-   Removing eastl::any type checking when RTTI is disabled due to cross
    dll pointer issues.
-   Relaxing restrictions on what the type of the find\_by\_hash\' input
    parameter can be. Instead of a requirement of a specific type, any
    type convertible to the underlying type is acceptable.

------------------------------------------------------------------------
**Version 3.05.06** (Jun 05, 2017)

Changes

-   Resolved shared\_ptr conversion operator= with no return expression.
-   Corrected constructor of ref\_count\_sp to match parameter types of
    the declaration.
-   Removing comments suggesting auto\_ptr converstion methods as
    auto\_ptr has been deprecated and removed from the standard.
-   Resolved VS2017 Preview 1 compiler warnings.
-   Disabled single-argument eastl::pair move-constructor (only GCC due
    to a bug) that generates an error for lvalue and rvalue overloads.

------------------------------------------------------------------------
**Version 3.05.05** (May 08, 2017)

Changes

-   Updated EASTL.natvis to support eastl::bitset containers.
-   Improved hash\_map::operator\[\] to work with move-only types.
-   Fixed the \'u8string\_view\' typedef to correctly be backed by
    char8\_t.
-   Added eastl::string user defined literals, but still disabled
    globally due to required eastl::string constexpr constructors.
-   eastl::integral\_constant c++14 updates for value\_type constexpr
    conversion operators.
-   Enabled the empty-base class optimization for eastl::string and
    its fixed sized variants.
-   Fixes required to enable eastl::string, eastl::string\_view and
    eastl::chrono literals.
-   Improving vector::erase\_unsorted to support move-only types.
-   Added at accessor to hash\_map class.

------------------------------------------------------------------------
**Version 3.05.04** (Mar 23, 2017)

Changes

-   Fixed eastl::hashtable functions DoInsertValue & DoInsertValueExtra
    which used a defaulted template parameter after a variadic template.

------------------------------------------------------------------------
**Version 3.05.03** (Mar 22, 2017)

Changes

-   Implemented helper template-alias \'index\_sequence\_for\'.
-   Fixed eastl::chrono::Internal::GetTicks function not scaling ticks
    by the HFC frequency.

------------------------------------------------------------------------
**Version 3.05.02** (Feb 08, 2017)

Changes

-   Fixed move\_iterator operator++(int) / operator\--(int) not
    incrementing its state.
-   Implemented fixed\_ring\_buffer template alias to simplify the
    creation of ring\_buffers backed by fixed\_sized containers.
-   Added templates aliases for new hashtable based container names.
    (eg. unordered\_set and unordered\_map)
-   Improved internal hashtable support for move-only (aka. non-copyable
    types).
-   string\_view fixes:
    -   implemented cbegin/rbegin/crbegin fixes and unit tests
    -   implemented cend/rend/crend fixes and unit tests
    -   Added CharTypeStringRSearch function to char\_traits header and
        fixed unit tests to called string\_view::rfrind.

------------------------------------------------------------------------
**Version 3.05.01** (Jan 10, 2017)

Changes

-   Resolving issue of eastl\_size\_t causing mismatch problems with
    standard C library functions like \'strlen\'
-   Resolved vector\_map/vector\_multimap::count to be called from const
    contexts.
-   Resolving eastl::is\_function returning incorrect results for
    Microsoft specific calling conventions (eg. \_\_stdcall).

------------------------------------------------------------------------
**Version 3.05.00** (December 7, 2016)

Changes

-   Implemented eastl:string\_view, a non-owning string type that isn\'t
    templated on an allocator making it suitable for use at API
    boundaries.
    -   http://en.cppreference.com/w/cpp/string/basic\_string\_view
-   Adding support for typed get for tuple (e.g. get(tuple& t)).
-   CoreAllocatorAdapter::operator= now copies the allocator name.
-   Fixed strict-aliasing issues in the eastl::intrusive\_list
    implementation.
-   Adding move semantic guards around test code requiring that language
    feature.
-   Improving eastl::vector implementation to move elements (instead of
    copying) during a resize.

------------------------------------------------------------------------
**Version 3.04.01** (Oct 26, 2016)

Changes

-   eastl::function the size of the local buffers that hold captures and
    allocators can tuned via the
    EASTL\_FUNCTION\_DEFAULT\_CAPTURE\_SSO\_SIZE, and
    EASTL\_FUNCTION\_DEFAULT\_ALLOCATOR\_SSO\_SIZE defines.

------------------------------------------------------------------------
**Version 3.04.00** (Oct 21, 2016)

Changes

-   Implemented eastl::any, a type-safe container for single values of
    any type that is implemented with the \'local buffer optimization\'
    to avoid heap allocations for small objects.
-   Improving standards conformance of vector::swap and improves use of
    vector with non-copyable or move-only types (e.g. unique\_ptr). This
    includes a subtle but important behaviour change were vector::swap
    will no longer perform a copy of the vectors elements when the
    allocator instances are different. If you rely on copying elements
    from one vector to another when calling vector::swap(), you can
    reintroduce the slower, standard non-conforming behaviour by
    building with
    \'EASTL\_VECTOR\_LEGACY\_SWAP\_BEHAVIOUR\_REQUIRES\_COPY\_CTOR\' as
    a global define.
-   Implemented hashtable::reserve(element\_count). This rehashes the
    underlying hashtable of all the hash containers to the accomdate the
    element count without exceeding the max load factor.
    -   http://en.cppreference.com/w/cpp/container/unordered\_map/reserve
-   Changed chrono::GetTicks() implementation on PS4 to rely on
    sceKernelGetProcessTimeCounter() rather than sceKernelReadTsc() as
    the former is unaffected by process suspend events.

------------------------------------------------------------------------
**Version 3.03.00** (Sept 2, 2016)

Changes

-   Implemented eastl::is\_swappable\_with /
    eastl::is\_nothrow\_swappable\_with.
-   Implemented eastl::clamp.
-   Brought back new classes from Frostbite:
    -   segmented\_vector: vector that does not invalid
        iterators/references when elements are added.
    -   string\_map: specialized version of a map that uses a const
        char\* as the key.
    -   string\_hash\_map: specialized version of a hash\_map that uses
        a const char\* as the key.
-   Added the capability to specify an extra source file to be compiled
    with the package. This is required for Frostbite\'s Arena Allocator.

------------------------------------------------------------------------
**Version 3.02.00** (July 8, 2016)

Changes

-   Implemented eastl::optional. For more details see:
    http://en.cppreference.com/w/cpp/utility/optional.
-   Implemented \'transparent operator functors\' for the following:
    -   plus
    -   minus
    -   multiplies
    -   divides
    -   modulus
    -   negate
    -   equal\_to
    -   not\_equal\_to
    -   greater
    -   less
    -   greater\_equal
    -   less\_equal
    -   logical\_and
    -   logical\_or
    -   logical\_not
-   Implemented eastl::void\_t.
-   Implemented eastl::is\_same\_v.
-   Implemented eastl::reverse(Iterator) to support reverse iteration in
    ranged-based for-loops.
-   Added allocation failure asserts to containers that do not utilize
    local storage.

------------------------------------------------------------------------
**Version 3.01.01** (May 19, 2016)

Changes

-   Implemented eastl::bool\_constant.
-   Implemented eastl::fixed\_hash\_map::rehash now supports hashtable
    bucket array overflow.
-   Implemented non-member eastl::size/eastl::data/eastl::empty.
-   Removed internal eastl::empty structure due to name collision.

------------------------------------------------------------------------

**Version 3.01.00** (Apr 15, 2016)

Changes

Added the ability to override EASTL\_DEBUG\_BREAK so that it no longer
points to an external function call. This is useful for when the user
simply wants to disable EASTL\_DEBUG\_BREAK and doesn\'t want to define
a custom external function, or it simply isn\'t possible with their
build setup. To override the EASTL\_DEBUG\_BREAK, user must define the
globalproperty EASTL.EASTL\_DEBUG\_BREAK\_OVERRIDE=(new macro
definition)

Implemented eastl::to\_string/eastl::to\_wstring.

Implemented eastl::exchange.

Implemented eastl::as\_const.

Implemented the move subscript operator for eastl::map. eg.
eastl::map::operator\[\](Key&&)

First integration of changes from github:

-   Benchmark fixes for memory allocators that support alignment.
-   License wording fixes.

------------------------------------------------------------------------

**Version 3.00.00** (Dec 02, 2015)

Changes

-   VS2015 Update 1 fixes.
-   Removed PS3 and Xbox360 support.
-   Removed internal RenderWare only header eastl\_rw.h.
-   Minor refactoring of build scripts.

------------------------------------------------------------------------

**Version 2.11.02** (Nov 18, 2015)

Changes

-   Implemented eastl::make\_reverse\_iterator.

------------------------------------------------------------------------

**Version 2.11.01** (Oct 26, 2015)

Changes

-   Fixed a compilation bug in eastl::unique\_ptr\'s move assignment
    operator.

------------------------------------------------------------------------

**Version 2.11.00** (Oct 26, 2015)

Changes

-   eastl::chrono implementation
-   Implemented eastl::unique\_ptr memory optimization using
    eastl::compressed\_pair.
-   Fixed eastl::mem\_fn usage for compilers that support variadic
    templates.
-   Added EASTL.natvis to Visual Studio solutions. VS2015 will
    automatically parse NATVIS files included in your solution removing
    the requirement to manually install NATVIS files.
-   Added runtime asserts in eastl::list that validates the containers
    allocator returns a non-null address.

------------------------------------------------------------------------

**Version 2.10.04** (September 23, 2015)

Changes

-   eastl::function fixes for MSVC allocating memory due to known issue
    with is\_nothrow\_move\_constructible.
-   eastl::function fixes for stateful allocators trashing the locally
    stored functor object.
-   Converted initialize.xml to structured-XML which fixes warnings
    generated by Framework.
-   Fixing signed/unsigned mismatch compiler error on capilano when
    using eastl::deque.
-   Fix function\_table copy assignment being deleted due to constant
    data members (function pointers).
-   Required licensing changes.
-   Fixing VS2015 warnings.

------------------------------------------------------------------------

**Version 2.10.03** (July 3, 2015)

Changes

-   Added ratio.h implementation. Required for a future eastl::chrono
    implementation. For more details see:
    http://en.cppreference.com/w/cpp/numeric/ratio/ratio.
-   Fixed fixed\_pool allocate call to use provided alignment
    parameters.

------------------------------------------------------------------------

**Version 2.10.02** (June 3, 2015)

Changes

-   Fixed eastl::hash\_set insert creating template instantiations for
    types that are not convertible to the eastl::hash\_set value\_type.
-   Added C++11 bitset::all().
-   Disabled warning C4746 on EASTL usage of \_\_is\_pod on Microsoft
    platforms.

------------------------------------------------------------------------

**Version 2.10.01** (May 11, 2015)

Changes

-   Fixed an eastl::ring\_buffer bug when using iterator subtraction to
    insert data.

------------------------------------------------------------------------

**Version 2.10.00** (Apr 23, 2015)

Changes

Added support for eastl::function as requested by Frostbite.

-   Due to the header file reorganization, namespace changes were
    required in code using MEM\_TEMP/MEM\_PERM in rwfilesystem.

------------------------------------------------------------------------

**Version 2.09.01** (March 16, 2015)

Changes

Visual Studio 2015 Preview fixes: Implemented workarounds for:

-   Noexcept operator bugs with variadic template expansions
-   Fixes for functions called with parameters with declspec alignment
    qualifiers
-   Enabled \"pair(pair&&) = default\" consructor for VS2015+ builds

Added a compile-time assert for use of find\_by\_hash when hash codes
are not cached. find\_by\_hash is an optimization designed to take
advantage of the hash being available, so it does not recalculate
hashes.

Fixed a bug with alignment in fixed size containers that can overflow.

Fix for alignment error when the key has greater alignment requirements
than the value, in the fixed\_hash\_map.

Fix for allocation alignment limitation in DLL builds.

Fix for shrink\_to\_fit where move semantics are not available.

------------------------------------------------------------------------

**Version 2.09.00** (October, 2014)

Changes

-   Added a safer version of [find\_by\_hash]{.code}, which takes in the
    key as well as the hash, in order to be sure we are finding the
    right one.
-   Added a [clear]{.code} method to [fixed\_vector]{.code}, which can
    free the overflow memory.

Bug fixes

-   Fixed some bugs in container classes that were being overly
    restrictive about what types they could contain.
-   Fixed a bug in [list::emplace()]{.code} - it should return an
    iterator to the newly inserted element, not the same pos iterator
    that was passed in.
-   Fixed an integer overflow bug in [basic\_string::find()]{.code}.

------------------------------------------------------------------------

**Version 2.08.00** (September, 2014)

Changes

-   Implmented atomic increment, decrement, and compare-and-swap for
    versions of GCC used by CentOS.
-   Added a public version.h header.
-   Resolved compilation warnings in iphone-arm64 configurations.
-   Modified the type of [hash\_code\_t]{.code} in
    [hash\_code\_base]{.code} to match the return type of the
    [hash\<T\>]{.code} function to avoid warnings. Similarly, the
    [mod\_range\_hashing]{.code} operator was modified to accept a
    [size\_t]{.code} as its first argument.
-   Resolved compilation problem when compiling with PS3SDK 460.001.
-   Implemented a default EASTLCoreDeleter implementation. Introduced a
    dependency on coreallocator inorder to test the functionality that
    relays on icoreallocator interfaces.
-   Implemented a default EASTLCoreAllocator implementation.

------------------------------------------------------------------------

**Version 2.07.00** (June, 2014)

Changes

-   Changed the list::sort implementation to avoid creating temporary
    lists. This fixes a problem with fixed\_list::sort whereby it was
    previously using its own implementation which effectively
    invalidated iterators. The fixed\_list problem was reported by
    Henrik Karlsson.
-   Added the following functions:\
    \
    [ // We provide a version of insert which lets the caller directly
    specify the hash value and\
    // a potential node to insert if needed. This allows for less thread
    contention in the case\
    // of a thread-shared hash table that\'s accessed during a mutex
    lock, because the hash calculation\
    // and node creation is done outside of the lock. If pNodeNew is
    supplied by the user (i.e. non-NULL)\
    // then it must be freeable via the hash table\'s allocator. If the
    return value is true then this function\
    // took over ownership of pNodeNew, else pNodeNew is still owned by
    the caller to free or to pass\
    // to another call to insert. pNodeNew need not be assigned the
    value by the caller, as the insert\
    // function will assign value to pNodeNew upon insertion into the
    hash table. pNodeNew may be\
    // created by the user with the allocate\_uninitialized\_node
    function, and freed by the free\_uninitialized\_node function.\
    template \<class P\>\
    insert\_return\_type insert(hash\_code\_t c, node\_type\* pNodeNew,
    P&& otherValue);\
    insert\_return\_type insert(hash\_code\_t c, node\_type\* pNodeNew,
    const value\_type& value);\
    \
    // Used to allocate and free memory used by insert(const
    value\_type& value, hash\_code\_t c, node\_type\* pNodeNew).\
    node\_type\* allocate\_uninitialized\_node();\
    void free\_uninitialized\_node(node\_type\* pNode);]{.code}

------------------------------------------------------------------------

**Version 2.06.00** (May 12th, 2014)

Release notes

-   The new conforming version of [shared\_ptr]{.code} in this release
    may possibly break some existing uses. There are only a couple
    possible relevant changes that could break users, and they are
    probably easily worked around. The breaks would be at compile-time,
    not run-time.
-   There is an outstanding issue with Visual Studio 2013\'s compiler
    when using std::initializer\_list types in operator=().
    https://connect.microsoft.com/VisualStudio/feedback/details/792355/compiler-confused-about-whether-to-use-a-initializer-list-assignment-operator
-   Visual Studio 2013\'s compiler will throw warnings when passing
    aligned structs as value which can occur when use
    eastl::is\_trivial\<T\>. To circumvent this temporarily, aligned
    structs beyond stack pointer alignment will be declared as
    non-trivial.

Changes

-   Fixed a compile failure with[ make\_pair(\"abc\", \"abcd\")]{.code}
    under VC++ (two string literals of different lengths).
-   Fixed a bug in the rbtree (map, multimap, set, multiset) move
    constructor which would result in a subsequent crash.
-   Changed two [vector::erase]{.code} calls from [eastl::copy]{.code}
    to [eastl::move]{.code}, which can sigificantly improve performance
    for movable types. This was missed during the update of vector to
    support C++11 move.
-   Implemented a conforming C++11 [unique\_ptr]{.code},
    [shared\_ptr]{.code}, [weak\_ptr]{.code}. This includes
    thread-safety as specified by the C++11 Standard.
-   [shared\_array]{.code} has been updated to be more consistent with
    the new [shared\_ptr]{.code}. However, in the future
    [shared\_array]{.code} will likely be deprecated and the
    functionality handled by [shared\_ptr]{.code}.
-   Fixed a case where bitset::to\_ulong was not throwing a
    [std::overflow\_error]{.code} exception when it should have been,
    only if EASTL exception support is enabled.
-   Added [move\_if\_noexcept]{.code} support to containers. This allows
    them to handle move vs. copy properly for when EASTL exception
    support is enabled.
-   Fixed [make\_move\_if\_noexcept\_iterator]{.code} to work with
    compilers that have weak C++11 support (e.g. VS2012 and earlier).
-   Added C++11 [allocator\_arg\_t]{.code} and C++14
    [allocator\_arg]{.code} to \<memory.h\>.

------------------------------------------------------------------------

**Version 2.05.00** (April, 2014)

Release notes

-   A class privacy change was made which shouldn\'t affect users unless
    they have custom containers that inherit directly from the internal
    [hashtable]{.code} and [rbtree]{.code} classes.
-   This release completes the support for C++11 container
    const\_iterators, rvalue move, and emplace.
-   Most container usage has the capability of being faster now under
    C++11 compilers due to move functionality. There\'s an uncommon case
    (classes that are slow to copy) where move-related emplace
    functionality can possibly be slower than the corresponding insert
    for some container types, but this will be addressed in a future
    release.

Bug fixes

-   Implemented VC++ [make\_pair]{.code} workaround for the case of
    calling [make\_pair(\"a\", \"b\")]{.code}.
    -   This problem was introduced in EASTL 2.02.00 with its C++11
        [pair]{.code} support. Reported by Christopher Vienneau.
-   Fixed a bug in some containers\'[ operator=]{.code} for the case of
    [EASTL\_ALLOCATOR\_COPY\_ENABLED]{.small} which could have caused a
    crash when allocators were unequal.
    -   It\'s unlikely users would have seen this bug manifested because
        it requires assignment between containers of unequal allocators,
        because people rarely (if ever) do this.
-   Fixed some C++ exception handling support which got stale from
    disuse over the last few years of changes.
    -   Full conformance of behavior in the presence of arbitrary
        exceptions hasn\'t been well-validated, though if you need this
        done let the package maintainers know.
-   Fixed a potential crash bug in the string class when assigned to
    another string via the move constructor.

Changes

Changes for [list]{.code}, [fixed\_list]{.code}, [slist]{.code},
[fixed\_slist]{.code}:

-   C++11 rvalue move support.
-   Added [const\_iterator]{.code} arguments (C++11 changed
    [insert(iterator, \...)]{.code} to [insert(const\_iterator,
    \...)]{.code}.
-   C++11 emplace support ([emplace]{.code}, [emplace\_back]{.code},
    [emplace\_front]{.code}). You can use emplace with compilers that
    don\'t support variadiac template arguments, but can you use only
    single value\_type arguments.
-   C++11 noexcept for all appropriate functions.
-   Fixed a bug in [list]{.code} and [slist]{.code} [operator=]{.code}
    for the case of [EASTL\_ALLOCATOR\_COPY\_ENABLED]{.small} which
    could have caused a crash when allocators were unequal.
-   Fixed [list::splice]{.code} and [slist::splice]{.code} so it can
    handle containers with unqual allocators.
    -   This change allows [slist::splice]{.code} to work and allows
        splicing between [list]{.code} and [fixed\_list]{.code},
        [slist]{.code} and [fixed\_slist]{.code}.
-   [slist]{.code} (the analogue of C++11 [forward\_list]{.code}) now
    supports all C++11 functionality except the [merge]{.code}.
-   Deprecated container-less [slist]{.code} [splice]{.code} functions,
    as they don\'t allow for recognizing the allocator, cannot maintain
    the source mSize, and are not in the C++11 Standard definition of
    [std::forward\_list]{.code}.

Changes for [ring\_buffer:]{.code}

-   Added C++11 rvalue move ctor and assigment support.
-   Added C++11 [const\_iterator]{.code} support.
-   [ring\_buffer]{.code} is still missing C++11 element rvalue
    insertion and emplace and support.

Changes for [vector]{.code}, [fixed\_vector]{.code}:

-   Improved [fixed\_vector::set\_capacity]{.code} to use move
    operations when possible.
-   Added C++11 rvalue move support for [insert]{.code},
    [push\_back]{.code}, [push\_front]{.code}, and added
    [emplace]{.code} support.
-   Fixed a bug in[ operator=]{.code} for the case of
    [EASTL\_ALLOCATOR\_COPY\_ENABLED]{.small} which could have caused a
    crash when allocators were unequal.

Changes for [deque]{.code}:

-   Added C++11 rvalue move ctor and assigment support.
-   Added C++11 [const\_iterator]{.code} support.
-   Added C++11 rvalue move support for [insert]{.code},
    [push\_back]{.code}, [push\_front]{.code}, and added
    [emplace]{.code} support.
-   Added [shrink\_to\_fit]{.code}, and implemented
    [set\_capacity]{.code}, which was previously a no-op.
-   Fixed a bug in[ operator=]{.code} for the case of
    [EASTL\_ALLOCATOR\_COPY\_ENABLED]{.small} which could have caused a
    crash when allocators were unequal.

Changes for tree containers ([set]{.code}, [map]{.code},
[fixed\_set]{.code}, [vector\_set]{.code}, etc):

-   Added C++11 rvalue move ctor and assigment support.
-   Added C++11 [const\_iterator]{.code} support.
-   Added C++11 rvalue move support for [insert]{.code}, and added
    [emplace]{.code} and [emplace\_hint]{.code} support.
-   Added support for the C++11 [value\_comp]{.code} function.
-   Added support for C++ [insert(P&& otherValue)]{.code}.

Changes for [hashtable]{.code} containers ([hash\_set]{.code},
[fixed\_hash\_set]{.code}, etc.):

-   Added C++11 rvalue move ctor and assigment support.
-   Added C++11 rvalue move [push, insert ]{.code} and [emplace]{.code}
    support (and [operator\[\]]{.code} for [hash\_map]{.code}).
-   Added C++11 [const\_iterator]{.code} support.

Changes for [array]{.code}:

-   Added the [fill]{.code} member function.
-   Added C++11 [const\_iterator]{.code} support.
-   Added noexcept to swap, as required by the C++11 Standard.

Changes for [basic\_string/fixed\_substring]{.code}[]{.code}:

-   Fixed a bug in[ basic\_string::operator=]{.code} for the case of
    [EASTL\_ALLOCATOR\_COPY\_ENABLED]{.small} which could have caused a
    crash when allocators were unequal.
-   Added C++11 [const\_iterator]{.code} support to
    [fixed\_substring]{.code}.

Changes for [intrusive\_list]{.code}, [intrusive\_hashtable]{.code}:

-   Added C++11 const\_iterator support.

Changes for [stack]{.code}, [queue]{.code}, [priority\_queue]{.code}:

-   Added C++11 rvalue move ctor and assigment support.
-   Added C++11 rvalue move [push]{.code} and [emplace]{.code} support.
-   Added other C++11 constructors.
-   Added noexcept to swap, as required by the C++11 Standard.
-   Added [priority\_queue::pop(value\_type& value)]{.code}, which is an
    extension to the C++11 Standard that allows popping move-only types
    (e.g. [unique\_ptr]{.code}). This situation is an acknowledged
    problem with the C++11 Standard [priority\_queue]{.code}.
-   Added [queue::validate]{.code} and [stack::validate]{.code}.

Changes for [list\_map]{.code}:

-   Added C++11 [const\_iterator]{.code} support.
-   Used C++11 deleted function support for enforce the non-use of
    unsafe inherited functions.
-   Added [reset\_lose\_memory]{.code}, to be consistent with other
    containers.

Changes to smart pointers:

-   Implemented C++11-conforming [unique\_ptr]{.code} and
    [default\_delete]{.code}.
-   Deprecated [scoped\_ptr]{.code} and [scoped\_array]{.code}, as they
    are superceded by [unique\_ptr]{.code}.

Changes to \<iterator.h\>:

-   Changed [reverse\_iterator::operator\[\]]{.code} implementation to
    be more flexible.
    -   <http://cplusplus.github.io/LWG/lwg-defects.html#386>,
        <http://llvm.org/bugs/show_bug.cgi?id=17883>
-   Added [make\_move\_if\_noexcept\_iterator]{.code}.
    -   [make\_move\_if\_noexcept\_iterator]{.code} returns
        [move\_iterator\<Iterator\>]{.code} if the Iterator is of a
        noexcept type; otherwise returns [Iterator]{.code} as-is. The
        point of this is to be able to avoid moves that can generate
        exceptions and instead fall back to copies or whatever the
        default [IteratorType::operator\*]{.code} returns for use by
        copy/move algorithms.

Changes to \<memory.h\>

-   Added the following C++14 iterator functions:
    -   rbegin(Container& container)
    -   rbegin(const Container& container)
    -   rend(Container& container)
    -   rend(const Container& container)
    -   crbegin(const Container& container)
    -   crend(const Container& container)
    -   rbegin(T (&arrayObject)\[arraySize\])
    -   rend(T (&arrayObject)\[arraySize\])
    -   rbegin(std::initializer\_list\<E\> ilist)
    -   rend(std::initializer\_list\<E\> ilist)
-   Added the following C++11-related functions:
    -   uninitialized\_copy\_n
    -   uninitialized\_move
    -   uninitialized\_move\_n
    -   uninitialized\_move\_fill
    -   uninitialized\_fill\_move
-   Added [late\_constructed]{.code}, which mplements a smart pointer
    type which separates the memory allocation of an object from the
    object\'s construction. The primary use case is to declare a global
    variable of the [late\_construction]{.code} type, which allows the
    memory to be global but the constructor executes at some point after
    main() begins as opposed to before main, which is often dangerous
    for non-trivial types.
-   Added C++11 [uses\_allocator\<\>]{.code}.
-   Added C++11 [pointer\_traits]{.code}.

Changes to \<type\_traits.h\>:

-   Fixed [is\_scalar]{.code} and [is\_fundamental]{.code} type traits
    for [nullptr\_t]{.code}. Previously they were false instead of true.
-   Improved type\_traits under VS2010 to be more conforming.
-   Added C++14 [aligned\_storage\_t/EASTL\_ALIGNED\_STORAGE\_T]{.code}.
-   Worked around a VS2010 bug affecting the implementation of
    [add\_reference]{.code}.

Changes to \<algorithm.h\>:

-   Implemented the [rotate]{.code} and [rotate\_copy]{.code}
    algorithms.
    -   The implementations benchmark as 1.1x - 4x faster than
        Microsoft/Dinkumware, gcc, and llvm [std::rotate]{.code},
        depending on the circumstances.
-   Added some missing [EASTL\_ITC\_NS]{.code} prefixes. This almost
    certainly will affect no users.

Privatized some previously public data in [deque]{.code},
[hashtable]{.code}, [intrusive\_hashtable]{.code}, and [rbtree]{.code}.
This shouldn\'t affect users unless they have custom containers that
inherit directly from the internal [hashtable]{.code} and
[rbtree]{.code} classes.

Added [EASTL\_MOVE\_INLINE]{.code} and [EASTL\_FORWARD\_INLINE]{.code}
macros. These allow for the same behavior as move() and forward() but
without a function call. This is useful for speeding up debug code by
avoid potentially very many function calls to these functions. These are
usable by users in addition to being used internally to EASTL.

------------------------------------------------------------------------

**Version 2.04.00** (March, 2014)

Changes

-   Added default hash operators for [char16\_t]{.code} and
    [char32\_t]{.code} types if the type is natively supported by the
    compiler (as opposed to being a typedef or \#define).
-   Made [\<eastl/iterator.h\>]{.code} [\#include \<iterator\>]{.code}
    for cases where the compiler has a bug and uses
    [std::begin/end]{.code} with range-based for loops instead of
    [eastl::/begin/end]{.code}. Currently there is only one compiler
    version affected by this: GCC 4.6.
-   Added the [next\_permutation]{.code} algorithm.
-   Added minor optimization to [iterator::advance]{.code} for the case
    of signed vs. unsigned distances. Has the side effect of making
    analysis tools like Coverity be more quiet.
-   Made [sort]{.code} and [stable\_sort]{.code} configurable.
-   Moved [radix\_sort]{.code}, [comb\_sort]{.code}, and
    [bubble\_sort]{.code} from sort\_extra.h to sort.h, as they are
    relevant sorts. Backwards compatibility is maintained.
-   Made [vector]{.code} reallocations use move construction when
    possible. Suggested by Yee Cheng Chin.
-   Worked around a GCC failure to compile a swap of two pointers in
    [\<hashtable.h\>]{.code}.

------------------------------------------------------------------------

**Version 2.03.00** (February 11, 2014)

Changes

-   Added VS2013 support.
    -   VS2013 supports more C++11 than previous versions, though there
        are some flaws in its implementation of the new functionality.
-   Fixed the uninitialized family of functions in [\<memory.h\>]{.code}
    to use C++11 [is\_trivially\_copy\_assignable]{.code} instead of
    (pre-C++11) [has\_trivial\_assign]{.code}. This was necessary
    because C++11 introduced deleted functions, which
    [has\_trivial\_assign]{.code} has no knowledge of.
-   EASTL now expects the user to also depend on EAStdC unless
    [EASTL\_EASTDC\_VSNPRINTF]{.small}=0 is defined and, if so, the user
    must then provided their own implementation to Vsnprintf8/16/32. See
    config.h for details.
-   Revised the [is\_convertible]{.code} type trait to be more portable.
-   Fixed some varargs usage to be strictly standards-compliant.
    Discovered by Coverity.

-   Fixed a compiler warning in [\<random.h\>]{.code} regarding
    shadowing of \'a\' and \'b\'.
-   Fixed [hashtable]{.code} [operator==]{.code} and [!=]{.code} to work
    as per the C++11 Standard, along with a new unit test.
-   Added a C++11 [is\_permutation]{.code} algorithm implementation and
    unit test.
-   Added [find]{.code} and [count]{.code} with predicate algorithm
    implementations.

------------------------------------------------------------------------

**Version 2.02.00** (December 2, 2013)

Release notes:

-   This release completes the support for C++11 type traits, and
    includes some parts of C++14 type traits.\
    It retains the pre-C++11 type traits as they are. Many of the traits
    are supported fully by pre-C++11 compilers, while some can traits
    can be supported only partially by pre-C++11 compilers. Not all
    parts of EASTL have been converted to use these C++11 traits, though
    over time they will, as the C++11 traits\' behavior are standardized
    and better tailored towards the needs of practical algorithms and
    containers.
-   This release completes the support for C++11
    cbegin/cend/crbegin/crend. C++14 functionality was added in EASTL
    2.05.00.\
    See
    http://stackoverflow.com/questions/12001410/what-is-the-reason-behind-cbegin-cend
    for an explanation of these.
-   This release completes the support for C++11 algorithms.
-   This release removes support for GCC 2.x, as its usage is unheard of
    for EA (and mostly elsewhere too).

Changes:

-   Added C++11/C++14 type traits:
    -   decay, conditional, add\_pointer, aligned\_union, rank, extent,
        is\_member\_object\_pointer, is\_literal\_type,
        underlying\_type, enable\_if, disable\_if,
        is\_lvalue\_reference, is\_rvalue\_reference,
        add\_lvalue\_reference, add\_rvalue\_reference, common\_type,
        result\_of, is\_trivially\_copyable,
        is\_trivially\_default\_constructible, is\_trivial,
        is\_constructible, is\_trivially\_constructible,
        is\_nothrow\_constructible, is\_default\_constructible,
        is\_nothrow\_default\_constructible, is\_copy\_constructible,
        is\_trivially\_copy\_constructible,
        is\_nothrow\_copy\_constructible, is\_move\_constructible,
        is\_trivially\_move\_constructible,
        is\_nothrow\_move\_constructible, is\_assignable,
        is\_trivially\_assignable, is\_nothrow\_assignable,
        is\_copy\_assignable, is\_trivially\_copy\_assignable,
        is\_nothrow\_copy\_assignable, is\_move\_assignable,
        is\_trivially\_move\_assignable, is\_nothrow\_move\_assignable,
        is\_destructible, is\_trivially\_destructible,
        is\_nothrow\_destructible, is\_null\_pointer.
-   Added some extension type traits:
    -   is\_array\_of\_known\_bounds, is\_array\_of\_unknown\_bounds,
        identity, static\_min, static\_max, is\_reference\_wrapper,
        remove\_reference\_wrapper, is\_swappable,
        is\_nothrow\_swappable, is\_reverse\_iterator,
        is\_move\_iterator, is\_insert\_iterator, is\_iterator\_wrapper,
        unwrap\_iterator.
-   Added volatile support to some type traits that were lacking
    recognition of it.
-   Added C++11 is\_heap\_until to \<heap.h\>.
-   Updated pair\<\> to support C++11 pair functionality, with the
    exception of piecewise\_construct.
-   Added unique\_ptr and unique\_array, which are the C++11 version of
    scoped\_ptr and scoped\_array (though C++11 has no unique\_array).
-   Added C++11 the move, move\_backward, and move\_n algorithms.
-   Re-wrote the copy and copy\_backward algorithms. They should now
    follow the C++11 standard and be at least as performant as
    previously, and in many cases more so due to better detection of
    memmove-able types.
-   Made eastl::forward(T), eastl::move(T), and
    eastl::move\_if\_noexcept(T) work on pre-C++11 compilers, though
    they work by simply returning T as-is.
-   Added a complete C++11 reference\_wrapper declaration, though with
    no definition.
-   Removed support for GCC 2.x, as its usage is unheard of for EA (and
    mostly elsewhere too).
-   Added the C++11 minmax, minmax\_element, and iota algorithms.
-   Added clang support to a couple type traits that weren\'t using it
    and thus were using less-compliant implementations of the trait.
-   Fixed the is\_array type trait to include unbounded arrays (e.g.
    int\[\]).
-   Fixed the definition of
    [EASTL\_TYPE\_TRAIT\_remove\_all\_extents\_CONFORMANCE]{.code}.
-   Added min/max specializations for common scalars.
-   Added support for initializer lists in the min/max algorithms. So
    now you can say this: [i = eastl::min({3, i, j});]{.code}
-   Added cbegin, cend, crbegin, crend support for all remaining
    containers that didn\'t previously have it. Also added the global
    cbegin and cend functions (C++14).
-   Added type traits helpers: [EASTL\_DECLARE\_INTEGRAL(T)]{.code} and
    [EASTL\_DECLARE\_FLOATING\_POINT(T)]{.code}. These will be used by
    EAStdC\'s int128\_t/uint128\_t in the future.
-   Removed reverse\_iterator related typedef\'s and methods from
    [hashtable(T)]{.code}. This change shouldn\'t break any code because
    the related code didn\'t work (or in many cases compile) in the
    past.
-   Added GCC 4.8.1 support, which mostly amounted to working around
    some warnings.

------------------------------------------------------------------------

**Version 2.01.00** (November 15, 2013)

Release notes

-   This version added inline asm optimized [fill]{.code} and
    [fill\_n]{.code} implementations for gcc/clang x86/x64 targets.
    While the unit tests pass, it may be that something was missed and
    affects code generation in other projects.

Additions

-   Added a new hashtable method called [find\_range\_by\_hash]{.code}
    that lets users iterate over all nodes in a hash bucket given a hash
    value.
-   Implemented small improvement to
    [vector::set\_capacity(size\_type n)]{.code} to optimize for the
    common case that n is 0. Aside from being faster, this results in
    such cases not calling resize(n), which currently creates a
    value\_type temporary on the stack, which may be a problem if
    value\_type is large.
-   Added inline asm and intrinsic specializations for the [fill]{.code}
    and [fill\_n]{.code} algorithms.
-   Added [ring\_buffer::set\_capacity]{.code}. It works like other
    containers\' set\_capacity.
-    Added C++11 is\_sorted\_until function, with associated unit
    tests:\
    [   template\<typename ForwardIterator\>\
       ForwardIterator is\_sorted\_until(ForwardIterator first,
    ForwardIterator last)]{.code}\
    \
    [   template\<typename ForwardIterator, typename Compare\>\
       ForwardIterator is\_sorted\_until(ForwardIterator first,
    ForwardIterator last, Compare compare)]{.code}
-   Added C++11 prev and next iterator functions, plus associated unit
    tests:\
    [     template\<typename InputIterator\>\
        InputIterator next(InputIterator it, difference\_type n = 1);\
    \
    ]{.code}[    template\<typename InputIterator\>\
        InputIterator prev(InputIterator it, difference\_type n =
    1);]{.code}\
-   Made it so [slist::insert\_after(position, first, last)]{.code}
    returns iterator instead of void. This was to match C++11\'s
    behavior in the equivalent class, forward\_list.
-   Added full C++ initialization list support for the following
    classes:
    -   deque
    -   map, set, multimap, multiset
    -   list
    -   slist
    -   hash\_map, hash\_set, hash\_multimap, hash\_multiset
    -   fixed\_hash\_map, fixed\_hash\_set, fixed\_hash\_multimap,
        fixed\_hash\_multiset.
    -   fixed\_map, fixed\_set, fixed\_multimap, fixed\_multiset
    -   fixed\_slist
    -   vector\_map, vector\_set, vector\_multimap, vector\_multiset.
    -   ring\_buffer
    -   stack, queue, priority\_queue (C++11 doesn\'t define initializer
        list support for these.

> The initialization\_list class is supported under all C++ compilers,
> but brace initialization syntax is supported only by sufficient C++11
> compilers. Example brace initialization usage: [eastl::map\<int, const
> char\*\> myMap = { {0,\"a\"}, {1,\"b\"} };]{.code}

------------------------------------------------------------------------

**Version 2.00.00** (July 22nd, 2013)

This release continues adding C++11 functionality. There is a lot of
functionality added here, and it may possibly introduce compatibility
problems, so please let us know if you run into any problems. If you are
using containers with custom allocators of your own construction, you
need to make sure that operator== is defined for the allocator or else
you can get compile errors which you didn\'t get with previous EASTL
versions.

Additions

Added the C++11 align function and an extension align\_advance function
to \<memory.h\>, with a unit test.

Added allocator\_malloc, which is an allocator that uses malloc.

Added the [EASTL\_MOVE\_SEMANTICS\_ENABLED]{.small} config define.

Added \<initializer\_list.h\>, which defines std::initializer\_list if
not already provided by the Standard Library.

Added C++11 move\<\> and forward\<\> to \<EASTL/utility.h\> .

Added C++11 move\_iterator to \<EASTL/iterator.h\>.

Added C++11 addressof (20.6.12.1).

Added initial C++11 move and move\_backward algorithms to
\<EASTL/algorithm.h\>.

-   These are initial versions which don\'t take advantage of memmove
    when possible.

Added some C++11 basic\_string and fixed\_string functionality, along
with unit tests:

-   move constuctor and functions:
    -   basic\_string(this\_type&& x);
    -   basic\_string(this\_type&& x, const allocator\_type& allocator);
    -   this\_type& operator=(this\_type&& x);
    -   this\_type& assign(this\_type&& x);
-   shrink\_to\_fit(), though it\'s identical in behavior to the
    existing vector::set\_capacity(string::npos);

<!-- -->

-   cbegin, cend, crbegin, crend.
-   initializer list support:
    -   basic\_string(std::initializer\_list\<T\> init, const
        allocator\_type& allocator =
        [EASTL\_BASIC\_STRING\_DEFAULT\_ALLOCATOR]{.small});
    -   this\_type& operator=(std::initializer\_list\<T\> ilist);
    -   this\_type& assign(std::initializer\_list\<T\>);
    -   iterator insert(const\_iterator p, std::initializer\_list\<T\>);
-   functions that took iterator in C++03 but now take const\_iterator
    in C++11 (e.g. insert(const\_iterator, value\_type c)).
-   Added noexcept declarators as applicable to member functions.

Added some C++11 vector functionality, along with unit tests:

-   move constuctor and functions:
    -   vector(this\_type&& x);
    -   vector(this\_type&& x, const allocator\_type& allocator);
    -   this\_type& operator=(this\_type&& x);
    -   void push\_back(T&& value);
    -   Not yet supported: insert(const\_iterator position, T&& value);
-   shrink\_to\_fit(), though it\'s identical in behavior to the
    existing vector::set\_capacity(string::npos);

<!-- -->

-   cbegin, cend, crbegin, crend.
-   initializer list support:
    -   vector(std::initializer\_list\<T\> ilist, const allocator\_type&
        allocator = [EASTL\_VECTOR\_DEFAULT\_ALLOCATOR]{.small});
    -   this\_type& operator=(std::initializer\_list\<T\> ilist);
    -   void assign(std::initializer\_list\<T\> ilist);
    -   iterator insert(const\_iterator position,
        std::initializer\_list\<T\> ilist);
-   vector(const this\_type& x, const allocator\_type& allocator)

<!-- -->

-   functions that took iterator in C++03 but now take const\_iterator
    in C++11 (e.g. insert(const\_iterator, value\_type c)).
-   Added noexcept declarators as applicable to member functions.
-   Not yet supported:
    -   iterator emplace(const\_iterator position, Args&&\... args);
    -   void emplace\_back(Args&&\... args);

Added fixed\_string global operator+ in various incarnations, including
rvalue reference versions, and unit tests to go with it.

-   This was done in order to support C++11 move operations in
    fixed\_string and basic\_string, but it turns out that it also fixes
    a crash bug that would occur if used an expression like this:
    (someFixedString1 + someFixedString2).swap(someString). Previously
    the + operator would result in a string and not a fixed\_string and
    the wrong swap would be called. Nobody has ever reported this,
    either because they didn\'t try such an obscure thing or they tried
    it and it crashed and they simply did something alternative.

Added inline declarations to class allocator member function
declarations; possibly to avoid warnings that occur with some compilers
in some situations.

Added is\_standard\_layout type traits support. This is a C++11 type
trait that is like is\_pod but is more relaxed and better identifies
structs that you can do things like memset with. As with is\_pod,
is\_standard\_layout mistakenly returns false in older compiler versions
(see [EASTL\_TYPE\_TRAIT\_is\_standard\_layout\_CONFORMANCE]{.small}),
due to compiler limitations. You can use
[EASTL\_DECLARE\_STANDARD\_LAYOUT]{.small} to work around these
limitations.

Added eastl::begin, eastl::end to \<eastl/iterator.h\>, to support C++11
functionality. Due to a bug in GCC 4.6, this iterator is not defined
when compiling with GCC 4.6 (and 4.6 only). If you need to use GCC 4.6
then you can use std::begin/std::end if that\'s available to you. As of
this writing (June 2013), the most commonly used Android compiler is
4.6-based, though it provides an option to build with 4.7 (e.g. by
having a [package.AndroidNDK.toolchain-version=4.7]{.code} global
property in Framework/nant).

Added enum value kNodeSize to deque. Suggested by Frostbite.

Added a ring\_buffer additional template parameter: Allocator. It
defaults to the allocator of the container.

Added ring\_buffer constructors: explicit ring\_buffer(const
allocator\_type& allocator) and ring\_buffer(size\_type cap, const
allocator\_type& allocator).

Added new config define: [EASTL\_ALLOCATOR\_MIN\_ALIGNMENT]{.small}.
This identifies what the eastl::allocator lowest possible min returned
alignment is. Updated allocator.h and allocator\_malloc.h to use this.

Added kHashtableAllocFlagBuckets to hashtable.h, which replaces class
hashtable\<\>::kAllocFlagBuckets. The latter is exists but is
deprecated. This change is to make it easier for users to use the
enumeration.

Added const get\_allocator() and const get\_overflow\_allocator
functions to various containers that didn\'t have it previously.

Bug Fixes:

-   Fixed a potential VC++ warning in string.cpp: warning C4365:
    \'initializing\' : conversion from \'\_\_int64\' to \'size\_t\',
    signed/unsigned mismatch.
-   Fixed eastl.natvis file (VC++ debug file) problem with map/set
    visualization. Reported by Mark Haines.

Changes

-   Updated hashtable.cpp to use ceilf instead of ceil, for a minor
    performance improvement. Suggested by Frostbite.
-   Incremented eastl.natvis file support for vector and array objects
    to display up to six instead of the previous three.
-   Improved fixed\_list, fixed\_slist, fixed\_map, fixed\_set
    constructor efficiency by making the fixed buffer base type be
    initialized only once. Suggested by Henrik Karlsson.
-   uninitialized\_move, uninitialized\_relocate\_start,
    uninitialized\_move\_abort, and uninitialized\_move\_commit from
    \<EASTL/memory.h\> have been renamed to uninitialized\_relocate,
    etc. This is because it collides with C++11 concepts regarding move.
    C++11 doesn\'t actually define an uninitialized\_move function,
    though it implies the meaning of such a thing, and we may implement
    such a function in the future. No users are likely to be affected by
    this change, as it was code that was mostly or entirely internally
    used.

------------------------------------------------------------------------

**Version 1.18.00** (June 4, 2013)

Release notes

-   The random\_shuffle algorithm has moved from \<algorithm.h\> to
    \<random.h\>, though for the time being \<algorithm.h\> \#includes
    \<random.h\>.

Changes

-   Added the following C++11 algorithms:
    -   all\_of
    -   any\_of
    -   none\_of
    -   copy\_if
    -   copy\_n
    -   find\_if\_not
    -   shuffle (see below)
-   Updated random\_shuffle to support C++11 rvalue arguments when
    possible, as per the C++11 Standard for random\_shuffle.
-   Added a [EASTL\_ASSERT(mpEnd \< mpCapacity)]{.small} assert to
    fixed\_vector::push\_back.
-   Added \<random.h\>, which hosts a small subset of C++11 random
    functionality for the purpose of supporting shuffle algorithms.
-   Added \<numeric\_limits.h\>, with C++11-level support for numeric
    limits.
-   Added str\_less\<\> to functional, as a complement to the existing
    str\_equal\<\>.
-   Made the wide basic\_string implementation (e.g. wstring, string32,
    etc.) do unsigned comparisons where the character type is signed. In
    practice this won\'t likely affect any users because all 16 bit
    character types we use are already unsigned, and there are no
    negative 32 bit signed unicode characters.
-   Added the following new type\_traits, all of which are present in
    C++11:
    -   make\_signed
    -   make\_unsigned
    -   add\_const
    -   add\_volatile
    -   add\_cv
    -   add\_reference
    -   remove\_reference
    -   remove\_pointer
    -   remove\_extent
    -   remove\_all\_extents
    -   remove\_const
    -   remove\_volatile
    -   remove\_cv
    -   aligned\_storage

    There are additional C++11 type traits that will be added on an
    as-needed, or by-request basis.

------------------------------------------------------------------------

**Version 1.17.01** (Mar 27, 2013)

Changes

-   Providing support to eaconfig to suppress valgrind errors/warning.
    In addition, adding a valgrind suppressions file to EASTL to
    suppress warnings about va\_list using unitialized variables.
-   Resolved unix64-clang issues that define global new operators.
-   Submission certification changes required by EAMT.

------------------------------------------------------------------------

**Version 1.17.00** (Mar 4, 2013)

Changes

-   Revised core\_allocator\_adapter.h to allow redefining
    EASTLDefaultAllocator to be CoreAllocatorAdapter. Suggested by Ryan
    Butterfoss.
-   This release formally supports the Samsung TV platform
    ([EA\_PLATFORM\_SAMSUNG\_TV]{.small}), though previous versions
    support this platform in practice already.
-   Changed [EASTL\_LIST\_SIZE\_CACHE]{.small} from 0 to 1. It was
    originally 0 due to memory-conscious platforms.
-   Made deque::set\_allocator able to safely install a new entirely
    different allocator, though only right after construction.
    Previously this wasn\'t possible. Submitted by Jeff Duncan as part
    of an effort to allow the queue class to be able to work with deque
    containers with custom-assigned allocators.
-   Improved fixed\_string::set\_capacity to truly free allocated memory
    when needed.
-   Added fixed\_string::full, has\_overflowed, can\_overflow. These
    match the existing functionality in vector and other fixed-size
    containers.
-   Added functions which implement UTF8 ↔ UCS encoding conversion:
    CtorConvert, append\_convert, assign\_convert.
-   Revised [EASTL\_LIST\_PROXY\_ENABLED]{.small} to be disabled under
    the SN compiler, as it can generate aliasing warnings with it
    starting with SN compiler v430+.
-   Added EASTL\_OPERATOR\_EQUALS\_OTHER\_ENABLED to enable previously
    disabled code.
-   Changed EASTL\_LIST\_SIZE\_CACHE from 0 to 1. It was originally 0
    due to memory-conscious platforms.
-   Some documentation updates, additions, and revisions.

------------------------------------------------------------------------

**Version 1.16.00** (December 10th, 2012)

Changes

-   Fixed eastl::safe\_object::has\_unique\_reference() bug, in which it
    was usually returning the wrong answer. Reported by Matt Miner.
-   Added the ability to override the default allocator used by
    CoreAllocatorAdapter \<core\_allocator\_adapter.h\>, through
    [EASTL\_CORE\_ALLOCATOR\_ADAPTER\_GET\_DEFAULT\_CORE\_ALLOCATOR]{.small}.
    Submitted by Ryan Butterfoss.
-   Added WiiU support.
-   Added VS2012 \"natvis\" debug helpers via the doc/EASTL.natvis file.
    Natvis files are the successor to Microsoft\'s AutoExp.dat files for
    viewing debug information for user-defined types.

------------------------------------------------------------------------

**Version 1.15.04** (November 5th, 2012)

Changes

-   Removed an \'if\' in selection\_sort which was unnecessary. Reported
    by James Park.
-   Added hash\<string16\> and hash\<string32\> for the purpose of
    supporting C++11 compilers that treat wchar\_t as being different
    from char16\_t and char32\_t. Suggested by Frederic Lauzon.
-   Fixed VC++ type traits support for VS2012+. VS2012 enables type
    traits support by default and no longer uses the \_HAS\_TR1 define
    to identify it. Reported by Henrik Karlsson.

------------------------------------------------------------------------

**Version 1.15.03** (August 7, 2012)

Changes

-   Revised string hashes (functional.h) to use uint32\_t instead of
    size\_t for calculations, so that the result is the same on 32 and
    64 bit machines. The strength of the hash is weakened on 64 bit
    machines but some amount. Users can override string hashes to
    provide their own custom versions if desired. Suggested by Darryl
    Gates.

------------------------------------------------------------------------

**Version 1.15.02** (July 3, 2012)

Changes

-   Improved platform portability.

------------------------------------------------------------------------

**Version 1.15.01** (June 4, 2012)

Changes

-   Removed kAlignmentOffset, kKeyAlignment, and kValueAlignment enum
    values from container definitions, to avoid evaluating the alignment
    of an incomplete type.
-   Added FreeBSD and WinRT support.
-   Fixed a bug in vector for push\_back(value\_type&) and resize(size,
    n, value\_type&) for the case of the value\_type coming from within
    the vector. This bug was introduced in v1.15.00.

------------------------------------------------------------------------

**Version 1.15.00** (May 7, 2012)

Changes

-   The base type of eastl\_size\_t now defaults to size\_t instead of
    uint32\_t. For backwards compatibility the symbol
    EASTL\_SIZE\_T\_32BIT can be defined to 1 if this causes an issue in
    64-bit configurations.
-   Implemented a unit test for bitvector, which was previously
    unimplemented.
-   Fixed broken \'bitvector::end() const\' implementation, found during
    unit test implementation.

-   Fixed broken \' bitvector::erase(reverse\_iterator,
    reverse\_iterator)\' implementation, found during unit test
    implementation.

-   Fixed broken bitvector global operator (e.g. ==, \<) declarations,
    found during unit test implementation.

-   Added bitvector::test(), set(), and get\_container(). test allows
    you to directly test for a bit being set and specify a default value
    if the bit position is outside the bitvector\'s range. bitvector
    differs from bitset in that the bitvector size is dynamic. set is
    the complement of test. get\_container is needed both for
    consistency with other adapter containers and so you can manipulate
    the container allocator.
-   Significantly improved performance of hash\_map::operator\[\]. Based
    on an idea from Strati Zerbinis.
-   Fixed a possible compiler warning in hashtable\'s
    distance\_fw\_impl.
-   Added union\_cast\<\>, which allows for easy-to-read casting between
    types that are unrelated but have binary equivalence. The classic
    use case is converting between float and int bit representations.\
    [     template \<typename DestType, typename SourceType\> DestType
    union\_cast(SourceType)]{.code}
-   [EASTL\_CT\_ASSERT]{.small} now uses C++ static\_assert when
    available, through EABase.
-   Revised usage of \'enum : size\_type\' is disabled for VS2011+, as
    VS2011 has a bug which results in mistaken compiler errors when
    using it.
-   Changed bitset::reference friend declaration to be more compliant
    with the C++ Standard.
-   Removed the definition of enumerated value kAlignment from vector,
    list, slist, deque, basic\_string, and string\_abstract, as well as
    kNodeSize from deque. This is so that recursively defined structs
    containing containers of themselves, such as [struct Test {
    eastl::vector\<Test\> children; };]{.code}, can be compiled. The
    definition of kAlignment required the evaluation of the alignment of
    an incomplete type.
-   Revised the implementation of eastl::vector to reduce the size of
    user binaries in practice.
-   Support for the clang compiler in unix64 configurations has been
    added.

------------------------------------------------------------------------

**Version 1.14.00** (February 28, 2012)

Changes

-   Added PSVita platform support (EA\_PLATFORM\_PSP2).
-   Added linked\_ptr::detach. Implemented by Ryan Ingram.
-   Fixed EASTL bitset operator \<\< and \>\> to not have a shift
    integer wraparound error for the case of shifting by 0, on some
    platforms.
-   Fixed merge\_sort\_buffer possible error due to integer rollover for
    the case of huge sort sets. Such a bug would only have been seen
    with cases of sorting 2 billion+ elements on a 32 bit computer. This
    was done in response to reading
    <http://googleresearch.blogspot.com/2006/06/extra-extra-read-all-about-it-nearly.html>,
    and it seems on first examination to be the only case within the
    EASTL codebase.
-   Fixed but in shared\_ptr dynamic\_cast constructor for the case that
    [EASTL\_RTTI\_ENABLED]{.small} is on. Reported by Alexander Efimov.
-   Removed [EA\_FORCE\_INLINE]{.small} usage from functional.h. It
    sometimes caused problems under GCC.
-   Fixed an EDG compiler warning related to type aliasing in
    hashtable.h.

------------------------------------------------------------------------

**Version 1.13.03** (January 11, 2012)

Release notes

-   This release may in some cases trigger a compile error under VC++
    due to a bug in VC++\'s crtdbg.h:\
         crtdbg.h(628) : warning C4005: \'\_malloca\' : macro
    redefinition\
    The only fixes for this are to make sure \<crtdbg.h\> is included
    before \<malloc.h\> or to get a fixed version of the Microsoft
    compiler or standard libraries. The bug appears to exist in at least
    VS2005 and VS2008.

Changes

-   Changed [EASTL\_STD\_ITERATOR\_CATEGORY\_ENABLED]{.small} default
    value from 1 to 0. This could break some existing users, but was
    ovewhelmingly approved by users.
-   Made fixed\_vector::set\_capacity fully support the desired
    behavior. Previously the full behavior was left as a to-do.
-   Revised fixed container has\_overflowed to report only if it has
    overflowed into the overflow allocator and not return true if it
    merely used all (but no more) of the space in the fixed allocator.
    Left the old full() function to behave like it used to (i.e. report
    true if the fixed allocator was full but no overflow).
-   Added fixed container can\_overflow().
-   Fixed safe\_ptr\'s use of the undeclared *InitSafePtr* function to
    instead use *reset*. Reported by Hector Yu.
-   Fixed safe\_object::remove infinite loop. Reported by Hector Yu.
-   bitset now supports 8 and 16 bit words in addition to the 32, 64,
    and 128 bit words it previously supported. These are useful for the
    case that you need to export the data to some other system that uses
    8 or 16 bit word bit arrays.
-   This version explicitly supports the Freebox platform.

------------------------------------------------------------------------

**Version 1.13.02** (October 21, 2011)

Release notes:

-   This version includes a change that may break some users. The
    container reset function has been renamed to reset\_lose\_memory.
    This was done because the name \"reset\" appears to have been too
    simple of a name and users unfamiliar with STL were thinking it
    meant the same thing as clear(). The change is intentionally
    breaking because it forces users of reset() (most of which appear to
    be mistaken) to explicitly choose to use reset\_lose\_memory. For
    the time being, you can set the [EASTL\_RESET\_ENABLED]{.small}
    option to enabled (if not already) in order to allow reset to work
    (though it just calls reset\_lose\_memory).
-   This version introduces the [EASTL\_SIZE\_T\_32BIT]{.small} config
    option, as described below. It is set to 1 by default in this
    release for backward compatibility, but a future EASTL release will
    set it to 0. This will result in compiler warnings in user code that
    assumes eastl\_size\_t is 32 bits all the time (even on 64 bit
    platforms). 64 bit users are encouraged to fix their code now so
    they will be prepared for this in the future.

Additions

-   Added support for the CLang compiler in both 32 and 64 bit modes,
    tested under iOS and OS X.
-   Added CTR (Nintendo 3DS) support.
-   Added a *partition* algorithm implementation. Provided by James
    Smith.

Bug Fixes

-   Fixed a possible compile error for the case of
    [EASTL\_ALLOCATOR\_COPY\_ENABLED]{.small} being enabled.
-   Fixed an intrusive\_list compile failure for the case that
    [EASTL\_VALIDATE\_INTRUSIVE\_LIST]{.small} is enabled under some
    versions of GCC. GCC versions prior to \~v4.4 have problems with
    may\_alias structs that have member functions.
-   Fixed a VC++ type\_traits error for the case that the user\'s build
    file explicitly disables TR1 support via [\#define \_TR1 0]{.code}.
    Reported by Arpit Baldeva.
-   Fixed a hash container bug where the copy\_code function usage might
    generate a compile error.

Changes

-   Renamed the container reset() function to reset\_lose\_memory(). For
    the time being, you can set the [EASTL\_RESET\_ENABLED]{.small}
    option to enabled in order to allow reset to work (though it just
    calls reset\_lose\_memory).
-   Implemented the fixed\_list::splice functions. Previously they were
    not supported.
-   Added the [EASTL\_SIZE\_T\_32BIT]{.small} config option, which
    controls whether eastl\_size\_t is size\_t or is uint32\_t. These
    two are different on 64 bit platforms. Currently it is set to 0 for
    backward compatibility, which means that eastl\_size\_t is
    uint32\_t. You can force it to size\_t by setting
    [EASTL\_SIZE\_T\_32BIT]{.small} to 0 in your build. A future EASTL
    release will set it to 0, as by default [EASTL]{.small} intends to
    be compatible with std [STL]{.small} to the extent possible. In nant
    builds you can configure [EASTL\_SIZE\_T\_32BIT]{.small} via the
    masterconfig.xml file globalproperties with
    [EASTL]{.small}.[EASTL\_SIZE\_T\_32BIT=1]{.small}.

------------------------------------------------------------------------

**Version 1.13.01** (June 6, 2011)

Additions

-   Added ability to specify the bitset word type as an optional
    template parameter to bitset. This is in addition to the
    [EASTL\_BITSET\_WORD\_SIZE]{.small} option that was added to EASTL
    1.13.00, with the primary difference being that this is an option
    that\'s usable per-bitset as opposed to being a global default.

Bug Fixes

-   Fixed a compile error under the Airplay platform (mobile middleware)
    due to \#including \<yvals.h\>. Airplay pretends it\'s VC++ but
    isn\'t, and so fails when compiling code that\'s VC++-specific.

Changes

-   Updated vector::DoInsertFromIterator and vector::DoInsertValues to
    be more efficient. Provided by Jean-Philippe Flouret.
-   Added a specialization to equal\_to\_2/not\_equal\_to\_2/less\_2 to
    allow using the same two types.

------------------------------------------------------------------------

**Version 1.13.00** (May 5, 2011)

Additions

-   Added provisional tim\_sort algorithm. Measurements suggest that
    this is the fastest of the stable sort algorithms. quick\_sort is
    still faster in general for random data but is not stable
    (http://en.wikipedia.org/wiki/Sorting\_algorithm\#Stability). Wrote
    a sort benchmarking system to go with this as well as a \"Sorting
    Algorithm Shootout\" paper.
-   Added [EASTL\_TYPE\_TRAIT\_]{.small}xxx[\_CONFORMANCE]{.small} for
    all type traits xxx. This indicates whether the type trait is fully
    conforming as opposed to having weaknesses.
-   Added VC++ type\_traits specializations that take advantage of
    native VC++ type trait support.
-   Added dual-type operator() to binary\_compose. Implemented by James
    Smith.
-   Added copy\_if algorithm. Implemented by James Smith.
-   Added the scoped\_ptr::detach function which forgets the owned
    pointer. This is different from the scoped\_ptr::reset function
    which deletes the pointer. Same for scoped\_array. Suggested by
    Andrew Willmott.
-   Added a codestripper script for the package.

Bug Fixes

-   Fixed bug in the [reverse]{.code} algorithm which could crash on
    reversing an empty range.
-   Fixed a bug in hashtable.h that could result in a compile error on
    recent GCC versions such as \"error: \'predicate\' cannot be used as
    a function\".
-   Fixed a bugs in memory.h\'s usage of operator new that were
    preventing the following from working: vector\<const
    SomeClassWithCtorThatTakesAnInt\> v(intArray.begin(),
    intArray.end()); Submitted by Dave Wall.
-   Fixed safe\_ptr to be buildable, and added a unit test for it.
    Submitted by Dejan Ivkovic.
-   Fixed an SNC strict aliasing problem with intrusive\_list. Reported
    by Toan Pham.

Changes

-   Implemented workaround for SN failure to compile priority\_queue.h.
-   Added a FAQ entry on how to sort by pointers or array indexes to
    objects instead of directly by objects.
-   Added some new developer debug options: [EASTL\_DEV\_DEBUG]{.small}
    and [EASTL\_DEV\_ASSERT]{.small}. These allow for developer
    debugging of EASTL, but aren\'t enabled for regular debug users by
    default since they are for internal code validation and not user
    input validation.
-   Added [EASTL\_BITSET\_WORD\_SIZE]{.small} config option, and set it
    to be 8 for SPU instead of the default 4 for SPU. Suggested by
    Malcolm Liu.
-   Updated code for various containers to better support const types
    (e.g. vector\<const int\>). Support of const types is only partially
    implemented at this point, but is intended to be fully implemented
    in the future. Note that not all functionality can be possible with
    const types; for example vector::insert(iterator) can\'t be used in
    a const type container.

------------------------------------------------------------------------

**Version 1.12.01** (January 19, 2011)

Additions

-   Added [iterator vector::erase\_unsorted(iterator position)]{.code}
    and the reverse\_iterator equivalent. These allow for significantly
    faster performance (due to less copying), but at the cost of not
    preserving container order.

Bug Fixes

-   Fixed bug which resulted in the wrong return value from int
    [basic\_string::compare(size\_type pos1, size\_type n1, const
    basic\_string& x, size\_type pos2, size\_type n2) const]{.code}.
    Reported by Boris Kropivinitsky.

Changes

-   Added [EASTL\_STD\_CPP\_ONLY]{.small} config option to allow
    disabling of C++ language extensions where they might otherwise be
    useful. [There are a couple cases in EASTL where compiler extensions
    are used to significant advantage where there is no alternative
    means of achieving that
    advantage.]{style="font-family:Times New Roman,serif;font-size:12.0pt;"}
-   Added [EASTL\_RESET\_ENABLED]{.small} config option to allow
    disabling of the container reset functionality. When disabled, the
    reset function acts as clear.
-   Added a print [EXIT]{.small}(n) at the end of the unit tests on wii
    to support automated running/testing on Wii.
-   Made it so that unit test printf calls get routed through a custom
    EASTLTest\_Printf function, which does platform-specific things as
    needed.
-   Made the assert function on Android use the Android log print
    function, as printf doesn\'t work on Android.
-   Implemented a specialization of the push\_back function for
    fixed\_vector which results in significantly less code generation
    for the case of overflow being disabled. Implemented by Jonathan
    Lawlor and Andreas Fredriksson.
-   fixed\_ containers now have a constructor that accepts an overflow
    allocator argument.
-   fixed\_ containers now have a get\_overflow\_allocator function.
    Previously you had to use
    container.get\_allocator().get\_overflow\_allocator(), which was
    tedious and not obvious.

------------------------------------------------------------------------

**Version 1.12.00** (November 5, 2010)

Additions

-   Added unary\_compose and binary\_compose to functional.h. Provided
    by James Smith.
-   Added bitset::from\_uint32, from\_uint64, to\_uint32, and to\_uint64
    functions. The addition of a uint64\_t constructor was skipped
    because it caused conflicts with existing code and portability
    problems for existing std STL code.

Bug Fixes

-   Fixed bug in vector::erase(reverse\_iterator, reverse\_iterator) and
    deque equivalent. Reported by David Benson.
-   Changed smart\_ptr.h to use const\_cast in order to get around
    potential compiler errors with some types. Reported by James
    Irizarry.

Changes

-   Improved efficiency of intrusive\_hashtable\_find by removing an
    unnecessary copy. Reported by Avery Lee.
-   use\_first and use\_second now have a argument\_type member typedef.

------------------------------------------------------------------------

**Version 1.11.03** (May 27, 2010)

Release notes

-   This version has been updated to support PS3 SN 330, CodeWarrior
    1.3, and Android platform.

Additions

-   Added PS3 debugger AutoExp.dat file, which implements
    Microsoft-style visualizers for the PS3 debugger. Implemented by
    Avery Lee.

Bug Fixes

-   \<none\>

Changes

-   Made some functions in functional.h use forced inlines in order to
    improve code generation. Implemented by Doug Hayes.

------------------------------------------------------------------------

**Version 1.11.02** (April 5, 2010)

Additions

-   Added Mobile support: iPhone, Palm Pre, and Android. More mobile
    platforms to follow.
-   Added list::push\_front\_uninitialized, to match
    push\_back\_uninitialized. Suggested by Henrik Karlsson.
-   Added intrusive\_hashtable::remove(const value\_type&). Implemented
    by Jon Parise.
-   Added the [EASTL\_ALLOCATOR\_COPY\_ENABLED]{.small} config option,
    which currently defaults to 0 (disabled). This option causes
    allocators to be copied in container operator= and swap.
-   Added key\_comp() to map, set, etc. containers and added key\_eq to
    hash\_\* containers.
-   Added ctor to CoreAllocatorAdapter which specifies flags. Suggested
    by Brian Hixon.

Bug Fixes

-   Switched usage of placment new to explicitly use the global version
    ::new, and opposed to plain new. Without the :: prefix, the compiler
    looks for operator new as a member of the allocated class before
    using the global version. We want to always use the global version.
    In the analagous case in C++ Standard STL, global new is expected to
    be explicitly used. Reported by Scott Penso.
-   Removed some GCC 4.4 Linux warnings related to strict aliasing.

Changes

-   Added PS3 SN compiler v320 and later compatibility. The new compiler
    was generating some new warnings.
-   Changed intrusive\_hashtable\_iterator(value\_type\* pNode,
    value\_type\*\* pBucket = NULL) to no longer have a NULL default
    argument, as using NULL unilaterally would result in a crash.
-   Moved insertion\_sort\_simple into an internal namespace, as it\'s
    not intended to be a public function and doesn\'t act identically to
    the other sort functions.

------------------------------------------------------------------------

**Version 1.11.01** (November 18, 2009)

Additions

-   \<none\>

Bug Fixes

-   Fixed linked\_ptr and linked\_array ctor bug whereby in some cases
    mpPrev and mpNext would be uninitialized. Reported by Chris Brown.
-   Fixed bug in merge\_sort\_buffer (used by merge\_sort and
    stable\_sort) which caused it to not compile when sorting a deque.
    Reported by Ian Barkley-Yeung.
-   Fixed Linux64 GCC compiler warning with that could occur with the
    iterator advance function. Reported by Chen Yang.
-   Fixed a bug in ring\_buffer::ring\_buffer(const Container& x) which
    could cause a crash if x was empty.

Changes

-   get\_overflow\_allocator / set\_overflow\_allocator are available
    for all fixed\_ container allocators. Example usage:
    someFixedList.get\_allocator().set\_overflow\_allocator(x);
-   allocator::operator=(const allocator&) now copies the name from the
    source allocator, same as the allocator copy ctor was already doing.
-   Made it so push\_back can\'t be called on a vector\_set,
    vector\_map, vector\_multiset, vector\_multimap, due to being
    inherently unsafe. Suggested by Mikael Hedberg and Johan Torp.
-   Updated unit test char16\_t support to use EACHAR16(\"\") instead of
    L\"\". This allows for C++0x compatibility.

------------------------------------------------------------------------

**Version 1.11.00** (October 5, 2009)

Additions

-   Added [vector::]{.code} and [list::push\_back\_unintialized]{.code}
    after some discussion with users regarding the appropriateness and
    usefulness of it. This allows the user to push\_back an element
    which is unintialized and the user manually initializes it with
    placement new. Example usage:
    [new(widgetVector.push\_back\_uninitialized()) Widget(37);]{.code}
-   Added support for the PSP SN compiler. This amounted to using
    \#defines to work around a couple compiler limitations.
-   Added [EASTL\_DEBUGPARAMS\_LEVEL]{.small} option to allow disabling
    of debug names.
-   Added a shared\_ptr constructor which allows specifying a custom
    deleter type for constructor argument U. This addition isn\'t
    entirely satisfactory, but there isn\'t a good solution to this
    problem. Suggested by Olivier Nallet.
-   Added void hashtable::clear(bool clearBuckets), which allows
    clearing buckets as well as elements. Using clear(true) returns a
    hash table to its newly constructed state.

Bug Fixes

-   Fixed bug in bitset for the case of shifting some bitsets by amounts
    greater than the machine word size. Submitted by Andrew Willmott.
-   Fixed priority\_queue::validate to use the custom Compare type when
    present. Previously it was always using the default. Submitted by
    Tom Bui.
-   Fixed bug in the change\_heap function whereby it wasn\'t using a
    temp variable like the other heap functions. Submitted by Andrew
    Willmott and Tom Bui.
-   Fixed small bug in string::reserve whereby it could reserve one byte
    more than the user asks for. The bug was introduced in EASTL 1.07.00
    while fixing a set\_capacity bug.
-   Fixed potential PS3 compile error in vector\_multiset::insert.
    Reported by Henrik Karlsson.

Changes

-   Made fixed-size containers delay their memory pool initialization
    until needed. This results in an extra four bytes per fixed-size
    container but allows for faster construction and faster use of
    containers whose memory pools aren\'t fully used. This is especially
    useful for temporary containers on the application stack.
    Implemented by Stefan Boberg.
-   [push\_back(void)]{.code} now returns [value\_type&]{.code} instead
    of void. Affects vector, deque, list, and ring\_buffer.
-   [push\_front(void)]{.code} now returns [value\_type&]{.code} instead
    of void. Affects deque, list, and slist.
-   Converted some remaining usage of \'static const size\_type\' to
    \'enum\' in order to reduce debug symbols.
-   Updated unit tests to work with PS3 SDK 270 and later, which
    introduce a new aligned global operator new.
-   Added some bounds checking assertions to array.h.
-   Revised the implementation of array::at to not fix bounds for the
    user.
-   Fixed some incorrectly failing bitset unit tests on Linux64.

------------------------------------------------------------------------

**Version 1.10.05** (January 19, 2009)

Additions

-   \<none\>

Bug Fixes

-   \<none\>

Changes

-   Changed how core\_allocator\_adapter.h deals with Microsoft warning
    C4396.

------------------------------------------------------------------------

**Version 1.10.04** (December 2, 2008)

Additions

-   \<none\>

Bug Fixes

-   Fixed bug in deque introduced in v 1.10.01 whereby memory usage
    could slowly increase without bound. With this bug fix, memory usage
    fixed and is slightly improved over what it was prior to v 1.10.01.
    Reported by Avery Lee.

Changes

-   Improved performance of deque insertion operations by avoiding some
    gratuitous iterator copying.
-   Added the EASTL\_ALLOCATOR\_EXPLICIT\_ENABLED option, whihc causes
    the allocator constructor (allocator(const char\*)) to be explicit.
    Without it being explicit code like wstring str(\"hello\"); would
    compile but not likely be doing what the user wanted, as the string
    here is the implicitly used allocator\'s name and not the string
    value. For backwards compatibility, this option currently defaults
    to disabled. Suggested by Tim Smith.
-   Re-enabled EASTL\_VALIDATE\_COMPARE in the equal\_range algorithm.

------------------------------------------------------------------------

\
**Version 1.10.03** (October 28, 2008)

Additions

-   Added EASTL Quick Reference.pdf. This is an API reference created by
    Luc Isaak.

Bug Fixes

-   Revised type\_traits is\_array implementation to deal with recent
    versions of GCC on Linux that didn\'t like the existing version.
-   Fixed bug in fixed\_multiset and fixed\_multimap related to
    mAllocator initialization. Reported by Chris Stott.
-   Fixed bug in vector::assign(InputIterator first, InputIterator last)
    whereby it wouldn\'t compile with an InputIterator or
    ForwardIterator but instead only with a RandomAccessIterator.
    Reported by Avery Lee.

Changes

-   Exposed fixed\_node\_pool\'s pool so the user can read tracking data
    programmatically.
-   The EASTL benchmark test has been updated to successfully compile
    under VS2008+ and EABase 2008+.

------------------------------------------------------------------------

**Version 1.10.02** (May 19, 2008)

Additions

-   Added version of stable\_sort which accepts a user-supplied
    allocator. Suggested by Avery Lee.
-   Added list::insert with no value\_type to match slist and
    vector::push\_back(void). Implemented by Luc Isaak.
-   Added specialized global swap for basic\_string to be consistent
    with rest of EASTL.

Bug Fixes

-   Fixed longstanding bug in fixed\_hash\_map/set whereby an assertion
    or crash would occur on construction if the bucket block size was
    identical to the node (value\_type) size. Previously this bug was
    documented as \"don\'t do that\" since all you had to do to fix it
    was change your bucket count to work around it. But this fix removes
    that limitation. Previously you would always find out immediately
    that you had the problem, so this bug fix isn\'t going fix anything
    that was previously silent.

Changes

-   Revised the ordering of some function declarations and usage of
    \"this\_type\" to be more consistent across the package. Implemented
    by Luc Isaak.

**Version 1.10.01** (May 13, 2008)

Additions

-   Added bitset::data() function. Suggested by Matthew Boelter and
    Jeremy Paulding.
-   Added const version of get\_allocator to basic\_string.
-   Added basic\_string::force\_size(), which which unilaterally moves
    the string end position (mpEnd) to make the string be a given size.
    Useful for when the user writes into the string via some extenal
    means such as C strcpy or sprintf. Note that you cannot achieve this
    goal via the resize() function, as resize fills in zeroes when it
    increases the size, as per C++ std::basic\_string convention.

Bug Fixes

-   Node-based fixed\_\* containers now init their node buffers only
    once instead of twice upon container construction. This is a
    performance improvement.
-   Fixed bug in deque whereby it could erroneously expand its storage
    upon assignment to a significantly larger size than current. This
    could result in a crash. Discovered and reported by Nicolas Mercier.
-   Fixed compile warning in slist for case of
    EASTL\_SLIST\_SIZE\_CACHE=1. Reported by Nate Jones.
-   Fixed crash bug in slist copy constructor. Reported by Nate Jones.
-   Fixed intrusive\_list::sort compiler error for case of
    EASTL\_LIST\_SIZE\_CACHE=1.

Changes

-   Modified fixed\_\* container copy constructors to copy the allocator
    name in addition to the container contents. Suggested by Jon Parise.
-   Converted most static const integral members to be enums instead. It
    turns out that in Microsoft debug information is significantly
    larger for static const members than for enums.
-   Removed support for EASTL\_VIRTUAL\_ALLOCATE, which was an
    experimental feature that went unused. This feature allowed you to
    subclass containers and override their allocation mechanism. This
    feature has been replaced by a new experimental feature called
    EASTL\_ENABLE\_VIRTUAL\_ALLOCATOR.

------------------------------------------------------------------------

**Version 1.10.00** (February 26, 2008)

Additions

-   Added SPU support. Submitted by Ashton Mason.
-   Added override allocator specification to fixed\_set,
    fixed\_multiset, fixed\_map, fixed\_multimap. Submitted by Ben
    Talbot.
-   Added EASTL\_MINMAX\_ENABLED, which allows min/max to go away.
    Suggested by Ben Talbot and Jon Lawlor.
-   Added get\_overflow\_allocator/set\_overflow\_allocator to
    fixed\_string. Soon to follow for other fixed containers.

Bug Fixes

-   Fixed x64/Linux/GCC vararg bug in the string.h sprintf function
    under whereby a string resize could crash. Submitted by Dave
    O\'Neill.
-   Fixed definition of EA\_LIKELY whereby it wouldn\'t be used under
    GCC.
-   Fixed bug in bitset::count for old GCC versions (2.x).
-   Fixed bug in string\_abstract whereby not having operator
    +=(base\_type&) could cause an intented temporary to be created.
    Reported by Patrick Moore.

Changes

-   Updated the FAQ with a new entry regarding the
    \_\_STDC\_CONSTANT\_MACROS define and stdint.h.
-   EASTL\_LIST\_PROXY\_ENABLED extended to slist in addition to list.
    EASTL\_LIST\_PROXY\_ENABLED disabled under VC8, since that compiler
    already supports a certain amount of smart container debugging.
-   Fixed the debugger visualizer view for the list container in the
    FAQ. Provided by Jeremy Paulding.
-   Did tweak to dllexport behavior (Windows platform) which allows apps
    to use intrusive\_list without compiling an EASTL library.
-   The assertion macro for SN compiler on PS3 was changed from
    \_\_builtin\_trap to \_\_builtin\_snpause. The latter is more
    friendly to debugging.
-   Added printf(\"\\32\") to end of unit test under PS3 and PS2. This
    indicates to the SN tools that the app is done.

------------------------------------------------------------------------

**Version 1.09.02** (December 20, 2007)

Additions

-   \<none\>

Bug Fixes

-   Fixed bogus compiler warning in allocator.h caused by a compiler
    bug. Reported and tested by Grace Lo.
-   Changed GCC may\_alias attribute use due to warnings generated for
    some users. Submitted by Jeff Litz.
-   Made fixes to newly generated warnings in latest PS3 SDK 210, GCC
    4.1.1, and SNC.

Changes

-   Revised workaround for Xenon standard library header bug recently
    introduced by Microsoft which affected the EASTL benchmark test app.

------------------------------------------------------------------------

**Version 1.09.01** (December 5, 2007)

Additions

-   Added initial support for the SN PS3 compiler. This involved one
    tiny container code change and one debug assertion define change.
    This support likely require more extensive testing before it is
    known to be good, as the SN compiler for PS3 is a fairly new entity
    for EA.

Bug Fixes

-   Fixed compiler warning in deque.h and recent Microsoft compilers
    regarding a signed/unsigned mismatch in the DoReallocSubarray
    function. Reported by [Samir
    Sinha.]{#SupportWebConversation1_lblProblemDescription .query_text}
-   Fixed compiler warning in eastl::bitvector regarding a difference
    between the declaration and the definition of the validate function.

Changes

-   intrusive\_ptr now uses intrusive\_ptr\_add\_ref /
    intrusive\_ptr\_release calls instead of direct calls. This allows
    the user to easily override the refernce counting mechanism on a
    per-class basis. Suggested by Magnus Ryme.
-   Made workaround for Xenon standard library header bug recently
    introduced by Microsoft which affected the EASTL benchmark test app.

------------------------------------------------------------------------

**Version 1.09.00** (November 5, 2007)

Additions

-   Added additional debug validation to deque to match that of vector.
-   Added the full() function to fixed size containers. This function
    efficiently tells you if all of the fixed space has been used. For
    fixed-size containers with overflow disabled, full() == (size() ==
    capacity()), but it is useful because some containers execute size
    in O(n) instead of O(1), whereas full() executes in O(1). So far
    this is implemented for list, slist, and vector. Suggested by Jon
    Lawlor.

Bug Fixes

-   Fixed compile error in shared\_ptr on Wii/CodeWarrior/Radix.
    Submitted by Ben Smith.
-   Fixed bug in linked\_ptr and linked\_array whereby subclasses
    couldn\'t be used with parent classes. Submitted by Harvey Thompson.

Changes

-   Made a minor change to vector and deque to work around what appears
    to be code misgeneration by the Wii compiler (CodeWarrior). Code
    misgeneration found by Ben Smith.
-   The heap algorithms now support aligned data types with all
    compilers. Previously the implementation didn\'t allow aligned types
    due to limitations in the VC++ compiler. Submitted by Jon Parise.
-   Made bitvector closer to working. It now compiles but is only
    partially tested and doesn\'t support containers other than deque.

------------------------------------------------------------------------

**Version 1.08.02** (September 26, 2007)

Bug Fixes

-   Fixed bug in vector::erase(first, last) that was introduced in
    1.08.00. It was intended to be a minor optimization, but it wasn\'t
    checked well enough.
-   PS2 / GCC 2.x compatibility has been reinstated. EASTL will continue
    to support PS2 as long as it is needed.
-   Fixed some potential CodeWarrior compiler errors in hashtable.h
    regarding template using statement requirements.

Changes

-   Disabled Compare validation for lower\_bound, upper\_bound, and
    equal\_range. The C++ standard STL specifications for these
    functions don\'t specify that the user is required to support the
    functionality that the validation code requires. So we align EASTL
    with the STL standard. This was brought up by Avery Lee.
-   Made EASTL\_VALIDATE\_INTRUSIVE\_LIST be disabled by default. This
    is because its presence requires that intrusive\_list\_node be a
    POD, whereas normally this isn\'t a requirement. Thus
    EASTL\_VALIDATE\_INTRUSIVE\_LIST might break some existing code when
    enabled.

------------------------------------------------------------------------

**Version 1.08.01** (September 12, 2007)

Release Notes

-   This release fixes a compile bug introduced in the previous version.

Bug Fixes

-   Fixed a compile bug in map.h under GCC that was introduced in
    v1.08.00.

------------------------------------------------------------------------

**Version 1.08.00** (September 10, 2007)

Release Notes

-   Feel free to try out the new string implementation (based on the
    abstract\_string class), as we would like to make it the default in
    after it is sufficiently tested. This release fixes some problems
    with the initial version present in the previous release of this
    package.
-   EASTL 1.07.00 added Compare function validation, and this may result
    in debug build compiler failures for the case whereby the user has
    made a custom compare function between two classes A and B and
    (incorrectly) assumed that all comparisons would only be A \< B and
    not B \< A. If you make a compare function between two different
    classes, you should be prepared that the compare could execute in
    either order, at least when EASTL\_DEBUG is defined. This is a
    belated warning about this behavior.

Additions

-   Added FAQ entry regarding hash container metrics.
-   Added initial draft of bitvector based on work by Ryan Ingram.
-   Added string\_hash to functional.h. This is a generic hash function
    that could be applied to any basic\_string type.
-   Added add\_signed and add\_unsigned type traits. These convert any
    type into its signed and unsigned equivalent, respectively.
-   Added default allocation names to fixed-size container (e.g.
    fixed\_vector) overflow allocators in debug builds.
-   Added ability to override default allocator definition. See, for
    example, EASTL\_VECTOR\_DEFAULT\_ALLOCATOR.

Bug Fixes

-   Fixed bug in allocator.h whereby it was using eastl::allocator
    instead of the wrapping macro EASTLAllocatorType. The latter allows
    you to redefine the EASTL allocator. This bug only could affect
    people that are redefining the EASTL allocator type. Submitted by
    Sergey Klibanov.
-   Disabled compiler warning in list::splice that would fire during
    list::sort when in practice there is no problem. This is not an
    ideal solution this problem, but the best solution probably involves
    revising or augmenting the list::sort and list::merge functions to
    be able to work on merely node pointers. This is in response to a
    report by Ryan Petrie.
-   Added more using statements to vector, such as using
    base\_type::GetNewCapacity. This fixes some compilation problems
    with the CodeWarrior compiler found by Michael Bagley.
-   Fixed potential compile error in red\_black\_tree for the
    CodeWarrior compiler regarding a using statement of mCompare and
    similarly with mExtractKey in hashtable.h.
-   Fixed bug in basic\_string substr and operator+ whereby the
    allocator wasn\'t being passed to newly constructed temporary
    strings. Submitted by Dan Christensen and Jon Parise.
-   Made workaround for bug in PS2 compiler (GCC 2.95) in map.h. Found
    by Paul Hudak and Martin Griffiths.

Changes

-   type traits improved to reduce symbol generation and debug info
    size. This work was done by Henry Goffin.
-   EASTL\_NAME\_VAL macro changed from \#define NULL to \#define
    ((const char\*)NULL). Based on a suggestion by Eugenio Panero.
-   Clarified the documentation for the allocator specification, based
    on a discussion with Sergey Klibanov.
-   get\_temporary\_buffer modified to take const char\* instead of just
    char\* as the pName parameter. This is arguably a bug fix.

------------------------------------------------------------------------

**Version 1.07.00** (July 23, 2007)

Additions

-   Added priority\_queue::validate(). At this point the validate
    function is no more than calling container::validate() and
    is\_heap(), which are already available. This makes the priority
    queue consistent with other containers.
-   Added an initial test implementation of abstract\_string to the
    bonus directory via two files: string\_abstract.h and
    fixed\_string\_abstract.h. These are intended to possibly replace
    string.h and fixed\_string.h if it works out OK. To test these,
    replace the original string.h and fixed\_string.h files with these.
    It is probably best if you don\'t become dependent on the unique
    functionality in abstract\_string unless and until it gets promoted
    to mainline functionality.
-   Added validate\_xxx functions (e.g. validate\_less()) which verify
    that the comparison functions are valid. Added such function
    validation to algorithms and containers. If you supply an invalid
    comparison function to some algorithms or containers you can get
    crashes, sometimes in rare and unexpected ways. There are some
    unusual cases whereby these validation functions might succeed but
    the comparison is still invalid, such as the case with some kinds of
    floating point comparisons. An EASTL\_VALIDATE\_COMPARE macro has
    been created which wraps debug testing of this validation.

Bug Fixes

-   Fixed bug in string sprintf for the case whereby the user is using
    the Microsoft \_vsnprintf to do the work instead of a
    standards-conforming vsnprintf.
-   Fixed bug in basic\_string::set\_capacity whereby an increased
    capacity could result in an off-by-one error.

Changes

-   Made vector::clear more efficient by directly implementing the
    operations instead of calling erase(begin, end). This was suggested
    by Jon Lawlor.
-   Made vector::erase potentially more efficient for the case of
    erasing simple types (e.g. int, char, float, void\*). This change
    isn\'t necessarily a win, and it may be removed in the future.
-   Changed the internal EASTLAlloc and EASTLFree macros to support
    expressions as arguments instead of just individual symbols.

------------------------------------------------------------------------

**Version 1.06.01** (June 1, 2007)

Additions

-   Added the ability to have a custom overflow allocator assigned to
    fixed\_hash containers instead of just the default global allocator.
    This completes the similar functionality introduced in 1.05.00 for
    other fixed size containers.
-   Added the unintialized\_move function, which aids in implementing
    faster and simpler container reallocation for elements of type
    has\_trivial\_relocate. The typical use case is where you have
    vector\<AutoRefCount\> and you want to avoid a ref-count storm when
    reallocating the vector. This function was implemented by Ryan
    Ingram and reviewed by Henry Goffin.

Bug Fixes

-   Fixed Linux/GCC 4.1.1+ compiler error for set and map due to the
    compiler not seeing the lower\_bound function in the parent class.

Changes

-   Augmented the benchmark to accomodate comparison with stdcxx (a.k.a
    Apache STL, a.k.a. RogueWave STL).
-   The unit test now supports Win64 under Framework2/nant/eaconfig.

------------------------------------------------------------------------

**Version 1.06.00** (May 4, 2007)

Additions

-   Added bitset::find\_first and find\_next, plus an associated unit
    test.

Bug Fixes

-   Fixed Linux compiler error for when using GCC with 32 bit wchar\_t.
-   Fixed CodeWarrior compiler warnings for the benchmark.
-   Fixed CodeWarrior vararg problem in basic\_string::append\_sprintf.
    CodeWarrior va\_lists are destroyed even when passed by value. So
    code was added to save va\_list objects between usage under
    CodeWarrior.
-   Fixed DLL build for fixed-size containers.

Changes

-   Modified the default ctor for vector, list, slist, deque, and string
    to not create and copy a temporary allocator. This is more efficient
    for the case where the user has an expensive-to-construct custom
    allocator. Otherwise nobody is likely to see any difference with
    this change.
-   Made the default ctor for vector, list, slist, deque, string not
    create a temporary. This should improve performance for some cases
    of non-trivial custom allocators.
-   Changed the unit test application to use an external copy of EABase
    and PPMalloc instead of using an internal copy of them.

------------------------------------------------------------------------

[Version ]{style="font-weight: bold;"}**1.05.01** (April 11, 2007)\
\
Release Notes

-   This release is a followup to 1.05.00, which was released one day
    earlier. Thus users picking up this version are likely picking up
    1.05.00 as well.

Additions

-   \<none\>

Bug Fixes

-   \<none\>

Changes

-   Changed masterconfig.xml to use eaconfig 1.20.00 instead of
    1.20.00-cygwin.
-   Changed benchmark to use EASTL\_LIST\_SIZE\_CACHE=1 when the
    comparison STL was also caching the list size.
-   Changed the list benchmark in order to avoid some quirky behaviour
    it was showing.
-   Changed PS3 assertion failure to use \"tw 31,1,1\" instead of
    \"trap\". The former is more friendly to debugging.

------------------------------------------------------------------------

[Version ]{style="font-weight: bold;"}**1.05.00** (April 10, 2007)\
\
Additions

-   Added the ability to have a custom overflow allocator assigned to
    fixed\_string, fixed\_vector, fixed\_list, and fixed\_slist instead
    of just the default global allocator. This has not yet been extended
    to the fixed\_hash, fixed\_set, and fixed\_map classes.
-   Added sndbs and IncrediBuild entries to the masterconfig.xml to
    enable distributed builds.

Bug Fixes

-   Fixed some compiler warnings and errors under GCC 3.4/Linux.
-   Fixed a bug under VC++/x86 in the min/max asm code that was causing
    an fp stack underflow (fcomp was being used to pop an entry, but it
    reads two items). This exception is normally masked, so execution
    continues but you get a \~30 clock stall. Note that this fix is in
    currently-disabled code.

Changes

-   The Vsnprintf function referred to in string.h was moved into the
    eastl namespace in order to reduce the possibility of ambiguity.
-   Modified the FAQ.html to fix some typographical errors.
-   The unit test and benchmark application now uses the native PPMalloc
    package instead of having a private copy of it.
-   The benchmark source code was revised to remove some compiler
    warnings under GCC 3.4/Linux and VC8/Windows.
-   Simplified the build files down to a single EASTL.build file.
-   Removed test and benchmark vcproj files in favor of simply always
    using .build files.

------------------------------------------------------------------------

[Version ]{style="font-weight: bold;"}**1.04.01** (Mar 19, 2007)\
\
Additions

-   \<none\>

Bug Fixes

-   Fixed swap / assignment operator for case whereby two hash tables of
    the same type but unequal allocators are used in the swap. An
    infinite recursion could have occurred.
-   Fixed bug in algorithm.h with the copy algorithm whereby it was
    using memcpy whereas instead it theoretically needs to use memmove
    in order to be safe. It is not known if this bug would result in
    problematic behavior in practice, but a problem related to this was
    in fact seen with the PS3 memcpy function.
-   Fixed compilation error in list::sort for when
    EASTL\_LIST\_SIZE\_CACHE is enabled.

Changes

-   Converted bitset to use shift and mask operations instead of
    division and modulus operations. The result is that debug builds go
    faster, as compilers were not converting the divisions and mods to
    shifts and masks in debug builds.

------------------------------------------------------------------------

[Version ]{style="font-weight: bold;"}[1.04.00
]{style="font-weight: bold;"}[]{style="font-weight: bold;"} (Feb 8,
2007)\
\
Additions

-   Added find\_as to set/multiset/map/multimap. This complements the
    find\_as functionality previously present in hash and
    intrusive\_hash containers. Recall that find\_as is an extension
    function which allows for faster associative container lookups when
    the key type is \"heavy\", such as a string class. This extension
    was implemented and submitted by Jon Parise.
-   Added less\_2 to functional.h to complement equal\_to\_2 and to be
    used with the aforementioned find\_as function.
-   Added core\_allocator\_adapter.h, which allows you to use an
    ICoreAllocator in EASTL (e.g. with a container).

Bug Fixes

-   Fixed definition of eastl\_ssize\_t. It was mistakenly set to be the
    same as eastl\_size\_t.
-   Fixed bug in list::reset for the case where
    EASTL\_SLIST\_SIZE\_CACHE is enabled (it is disabled by default).
    Fix submitted by Jon Lawlor.
-   Fixed bogus assertion failure in list::sort for the case whereby the
    list being sorted has an allocator member that isn\'t the default
    for that allocator type. The code executed correctly.
-   Fixed bug in rbtree::swap which could cause a crash. The bug was
    detected and fixed by James Grieve.
-   Fixed a documentation bug in algorithm.h regarding an example of how
    to use the unique algorithm.
-   Implemented somewhat hacky way to work around CodeWarrior\'s way of
    doing header files. Basically CodeWarrior unilaterally includes the
    current directory as part of the include path even if you don\'t
    specify so and even if you \#include files with \<\> instead of
    \"\". This means that it becomes effectively impossible for you to
    have any header file that is the same name as any other in the
    project.

Changes

-   The list class has the GCC-specific compiler directives removed.
    They should no longer be necessary, as the bug which necessitated
    them was fixed in v1.01.00.
-   Documentation has been modified to more precisely define allocator
    expectations.
-   Made XBox 360 specific functions for a few string member search
    functions. Metrics showed that a memchr/memcmp-based search was
    faster than a compiler-generated algorithm. Note that on other
    platforms/compilers, this was not so.
-   Did some minor bitset optimization of the set(size\_t, bool)
    function. Some compilers weren\'t doing the optimal thing.
-   Removed allocator swap in deque::swap, for consistency with other
    containers.
-   Disabled the Win32/x86 optimized pathways for the min and max
    algorithms. They didn\'t always act optimally.

------------------------------------------------------------------------

[Version 1.03.00
]{style="font-weight: bold;"}[]{style="font-weight: bold;"} (Nov 29,
2006)\
\
Release Notes

-   This version of EASTL adds argument validation to the vector and
    string classes (and their variants such as fixed\_vector). This may
    result in users getting assertion failures in these modules where
    they weren\'t previously. These assertion failures are indicating
    that a misuse of the given vector or string has occurred. Efforts
    have been made to make sure errors are not mis-reported, but please
    notify the EASTL maintainer if you believe a failure report to be in
    error.

Additions

-   Added argument validation to the vector and string classes. For
    example, if you use an invalid index for operator\[\], an assertion
    failure will be generated.
-   Added a provisional radix\_sort to sort\_extra.h.
-   Added a provisional insert(const Key& key) function to
    map/multimap/hash\_map/hash\_multimap. This function is like
    vector::push\_back(void) in that it doesn\'t require a value\_type
    but instead creates one by default in the correct place. Then you
    can reference the object directly from its position in the map. The
    advantage of this is that it avoids the creation of value\_type
    temporaries and copy construction.

Bug Fixes

-   Implemented for a workaround for the VC++ compiler generating
    warning C4548 due to a bug in VC++\'s own header files.

Changes

-   \<none\>

------------------------------------------------------------------------

[Version
1.02.01]{style="font-weight: bold;"}[]{style="font-weight: bold;"} (Nov
15, 2006)\
\
Additions

-   \<none\>

Bug Fixes

-   Fixed a compiler warning under VS2005 that was missed in v1.02.00.
    It was missed because EASTL is composed of templates and sometimes
    you don\'t see a warning unless something is compiled in a specific
    way.

Changes

-   \<none\>

------------------------------------------------------------------------

[Version 1.02.00
]{style="font-weight: bold;"}[]{style="font-weight: bold;"} (Nov 14,
2006)\
\
Additions

-   Added definition of eastl\_ssize\_t to go with eastl\_size\_t. This
    is similar to the Posix/Unix convention (but not C standard)
    ssize\_t.
-   Added a constructor for pair which takes just the first parameter
    but not the second. This allows for more efficient usage of map
    value\_type creation.
-   Added contiguous\_iterator\_tag to extend
    random\_access\_iterator\_tag. Contiguous ranges are more than
    random access, they are physically contiguous. This allows for
    optimizations that aren\'t possible with random-access. For example,
    a deque is random-access, but it is not contiguous and thus you
    cannot memcpy its contents. Future optimizations in EASTL may be
    able to take advantage of this iterator tag.
-   Added VS2005 support. This mostly was a matter of working around or
    disabling warnings generated by this compiler.
-   Added documentation regarding VC8 autoexpand to the FAQ.

Bug Fixes

-   Fixed compiler warning in ring\_buffer related to a function
    argument called \"capacity\".
-   Fixed compiler warning that appeared under VC8 when warnings \"that
    are normally disabled\" are enabled. It turns out that VC++\'s
    malloc.h header file generates such warnings. EASTL was modified to
    disable compiler warnings whenever \#including malloc.h directly or
    indirectly.
-   Removed incorrect const usage from functions in numeric.h which
    result in compiler errors.
-   Fixed vector\_set, vector\_multiset, vector\_map, vector\_multimap
    versions of insert(iterator position, const value\_type& value).
    This is the rarely-used iterator-hint version of insert. There were
    cases where an insertion operation could incorrectly fail for some
    values of a hint iterator.
-   Fixed eastl::hashtable so that the return value of insert(iterator
    position, const value\_type& value) is iterator, and not pair. There
    is no C++ standard related to this, but it is certainly more
    consistent with other associative containers if it returns iterator
    instead of pair.
-   Removed a potential compiler warning in insert\_iterator for VC++
    warning C4512 (level 4).

Changes

-   \<none\>

------------------------------------------------------------------------

[]{style="font-weight: bold;"}

[Version 1.01.00
]{style="font-weight: bold;"}[]{style="font-weight: bold;"} (Sep 12,
2006)\
\
Release Notes

-   As noted below, specializations of the min and max algorithms have
    been made for floating point types. Due to the nature of the
    implementations, the specializations are done as non-templated
    functions that use value semantics instead of reference semantics.
    So they aren\'t really template specializations at all but function
    overrides. A significant result of this is that using
    eastl::min\<float\>(1.f, 2f) is not the same as using
    eastl::min(1.f, 2.f), as the former specifically uses the generic
    templated version while the latter uses the new optimized
    non-templated version. So you usually want to avoid using
    eastl::min\<float\>(); in fact VC++ has a level 4 warning that will
    trigger if you use eastl::min\<float\>().

Additions

-   Added a provisional safe\_ptr class, which is essentially an
    intrusive weak pointer. It is somewhat like weak\_ptr, with the
    primary difference that it uses an intrusive list to implement the
    weak connection instead of using a struct allocated on the heap.
    This has the benefit of avoiding memory allocation at the cost of
    operating slower.
-   Added fixed\_allocator and fixed\_allocator\_with\_overflow in a new
    header file called fixed\_allocator.h. These allow you to implement
    a fixed-size container where by the allocated memory is provided by
    the user dynamically instead of via a compile-time template
    paramter.
-   Added specializations of min/min\_alt/max/max\_alt for floating
    point types which implement hardware-assisted branch-free
    operations.

Bug Fixes

-   Fixed bug in bitset whereby bitset\<32\>::bitset(uint32\_t) would
    unilaterally set the bitset to all zeroes instead of the supplied
    uint32\_t.
-   Fixed bug in the unit test for fixed\_vector whereby it was testing
    alignment incorrectly.
-   Fixed unused parameter warning in memory.h.
-   Fixed bug in merge\_sort in which memory it allocated via
    allocate\_memory wasn\'t constructed to the appropriate value\_type.
    This would lead to a crash for cases where value\_type is no-POD.
    Since stable\_sort calls merge\_sort, stable\_sort is affected by
    this as well.
-   Fixed bug in fixed\_hash containers whereby a users-specified bucket
    count that was equal to a prime number resulted in an off-by-one.
-   Fixed is\_class type trait to work correctly under GCC 4.
-   Fixed bitset for GCC 4 when 64 bit words are in use. Some functions
    such as flip() wouldn\'t work, due to the usage of
    \_\_builtin\_popcountl instead of \_\_builtin\_popcountll.
-   Fixed compiler error that would result if you declared
    vector\<short\> v(100, 0); and didn\'t cast the 0 to short. The 0
    caused a short/int ambiguity.
-   Fixed bug in fixed\_string whereby a newly constructed string mignt
    not have been 0-terminated.
-   Added some GCC compiler barrier statements to get around inlining or
    pointer aliasing issues with the list class (particularly
    ListNodeBase) with full optimization settings. I (Paul Pedriana)
    have looked at the code and can\'t immediately see any mistakes its
    making with respect to pointer aliasing assumptions nor
    inlinability. But disabling inlining, using -fno-strict-aliasing, or
    using these compiler barrier statements nevertheless make the
    problems go away. Note that the bug fix (which occurred after this
    change) is most likely the cause of this behaviour.
-   Fixed bug in list and slist whereby they were mistakenly using debug
    code in a release build instead of the intended release build code.
    The debug code wasn\'t slower or doing anything differently;
    however, it was breaking strict pointer aliasing rules.
-   Fixed bug in string and hashtable with regard to pointer aliasing.
    This wasn\'t known to be causing problems but is an appropriate
    change anyway.

Changes

-   All algorithms now preserve reference semantics of user-supplied
    functors (e.g. sort Compare objects). This means you can now pass a
    Compare object by reference to a sort function and it will treat it
    as expected and use it internally as a reference and never copy it.
    No existing commercial STL lets you do this, though there is no good
    reason why not.
-   Modified the way some containers destroy their value\_types in order
    to get around compilation errors with VC++ when managed extensions
    (/clr) are enabled. This appears to be a bug in VC7, but it\'s not
    entirely clear.
-   The merge\_sort\_buffer algorithm has been modified to include an
    optimization.

------------------------------------------------------------------------

[Version
1.00.00]{style="font-weight: bold;"}[]{style="font-weight: bold;"} (Jun
30, 2006)\
\
Additions

-   Added support for DLL builds via Framework2/Nant/EAConfig.
-   Added some extra constructors and functions to fixed\_string to
    allow it to do some operations it previously was disallowed from
    doing. This change was proposed and implemented by Henry Goffin and
    Alex Bilyk.

Bug Fixes

-   Fixed default constructor of value\_types in slist to explicitly use
    the compiler-generated default constructor. In other words, we say
    \"new value\_type()\" instead of \"new value\_type\". There is a
    subtle difference between the two for built-in types such as int.
    Other containers were already doing this properly and only slist is
    affected by this change.
-   Removed possible generation of warning 4267. There is a bug in VC++
    that causes some warnings to be erroneously reported. This change is
    internal protection against that VC++ bug. See the FAQ for more
    information regarding these warnings.
-   Fixed by in fixed\_string\<char16\_t\> whereby the default
    constructor would not null-terminate the string. This bug was
    introduced in v0.99.00.

Changes

-   Disabled VC++ warning C4345 for the convenience of the user when
    calling container.push\_back(). This is one of those compiler
    warnings that isn\'t actually a warning but rather a message.
    Actually, this warning is odd in that it is telling the user that
    Microsoft fixed a bug in their compiler.
-   Modified min(a,b,compare) and min\_alt(a,b,compare) to reverse the
    comparison argument order to be the same order as min(a,b) and
    min\_alt(a,b). The previous version gave correct results but was
    syntactically different from the other version of min and min\_alt.
    So to be consistent they have been changed to be identical
    syntactically.
-   Documentation was added to the min and max algorithm implementations
    to precisely define that their syntax as(b \< a ? b : a) and (a \< b
    ? b : a) respectively.

------------------------------------------------------------------------

[Version
0.99.00]{style="font-weight: bold;"}[]{style="font-weight: bold;"} (May
21, 2006)\
\
Additions

-   Added min\_alt and max\_alt algorithms, which are identical to min
    and max but avoid any possible conflicts with \#defines of min and
    max that occur with the VC++ Windows compiler. Switched EASTL
    internal code to use these functions instead of regular min and max.

Bug Fixes

-   Fixed bug in fixed\_string default constructor whereby it was
    neglecting to set the first char to 0.
-   Fixed bug in intrusive\_hash\_map and intrusive\_hash\_multimap
    whereby the Equal template parameter was defaulting to using T
    instead of Key.\'
-   A big DEQUE\_DEFAULT\_SUBARRAY\_SIZE was fixed. It was a minor bug
    that simply choose a different subarray size than intended.

Changes

-   Made workaround for GCC 3.x bug in its implementation of UINT64\_C.
-   Made constructors for queue, stack, and priority\_queue be broken
    out into two constructors without default arguments instead of one
    with default arguments. The former gives a little more
    flexilibility.

------------------------------------------------------------------------

[Version
0.98.00]{style="font-weight: bold;"}[]{style="font-weight: bold;"} (Feb
18, 2006)\
\
Additions

-   []{style="font-family: Courier New;"}Added additional testing code
    to the unit tests that help detect if an object being destructed is
    invalid or was corrupted.
-   []{style="font-family: Courier New;"}Added nth\_element algorithm to
    sort.h.
-   Added the following to list and intrusive\_list: merge(), unique(),
    sort().
-   Added support for the Nintendo Revolution platform
    (EA\_PLATFORM\_REVOLUTION).

Bug Fixes

-   Fixed bug in deque::pop\_back() whereby it would call the destructor
    on the wrong object.
-   Fixed bogus assert in the TestHash unit test in release builds. It
    was reporting a failure, whereas the code was acting properly.
-   Fixed bug in binary\_search\_i\<\..., Compare\> whereby it was
    ignoring the user-supplied compare function.
-   Fixed problem with \_\_alignof not working in GCC for PS2; reverted
    to C-style alignof calculation in this case.
-   Fixed bug in fixed\_vector::reset(), fixed\_vector::operator=(),
    fixed\_string::reset(), fixed\_string::operator=().
-   Fixed bug in list::remove\_if whereby a cast was wrong and if you
    used the function it simply wouldn\'t compile.
-   Fixed bug in intrusive\_hash\_map and intrusive\_hash\_multimap
    whereby they wanted you to create a hash for the value\_type instead
    of the key\_type.

Changes

-   Added a check for invalid alignment values in fixed\_pool::init().
    GCC 2.x seems to mess up \_\_alignof() in some cases.
-   Moved some of allocator.cpp\'s implementation into allocator.h.
-   Moved the median algorithm from sort.h to algorithm.h so that it is
    more accessible for general use.
-   Made intrusive\_list, intrusive\_slist, and intrusive\_sdlist have a
    copy constructor and operator=. These functions simply set the
    container to be empty and ignore the copy source. These containers
    no longer are PODs.
-   Removed basic\_string::append(size\_type n), as it too easily allows
    the user to do the following mistake: s.append(\'\\n\').

------------------------------------------------------------------------

[Version 0.97.00]{style="font-weight: bold;"} (Jan 10, 2006)

Thanks go to various users around EA for identifying problems fixed in
this version of EASTL. These include Wes Potter of EAC, Henry Goffin of
Maxis, and Jonathan Lawlor of EAX.\
\
Additions

-   Added some algorithms (of which the first three are not standard C++
    STL algorithms):
-   -   find\_first\_not\_of
    -   find\_last\_of
    -   find\_last\_not\_of
    -   mismatch
    -   set\_difference
    -   set\_symmetric\_difference
    -   set\_intersection
    -   set\_union

-   [ring\_buffer]{style="font-family: Courier New;"} is now complete
    and unit tested.
-   Added [max\_size()]{style="font-family: Courier New;"} to fixed-size
    containers (e.g.
    [fixed\_vector]{style="font-family: Courier New;"}).
-   Added additional documentation to type traits.
-   Added [inserter]{style="font-family: Courier New;"} helper function
    to [iterator.h]{style="font-family: Courier New;"}.
-   Added [sort]{style="font-family: Courier New;"} and
    [stable\_sort]{style="font-family: Courier New;"} for std STL
    compatibility. The implementations map to
    [quick\_sort]{style="font-family: Courier New;"} and
    [merge\_sort]{style="font-family: Courier New;"}.

Bug Fixes

-   Fixed bug in [fixed\_vector]{style="font-family: Courier New;"} and
    [fixed\_string]{style="font-family: Courier New;"} whereby copy
    construction or [operator=]{style="font-family: Courier New;"} would
    do the wrong thing.
-   [deque]{style="font-family: Courier New;"} subarray size calculation
    fixed to force power-of-2 sizes. Previously this could cause a crash
    if the size was not a power of 2.
-   [intrusive\_list::splice(iterator,
    this\_type&)]{style="font-family: Courier New;"} fixed to deal with
    case of x being empty.
-   [intrusive\_hash::erase(value\_type)]{style="font-family: Courier New;"}
    fixed to rectify bug whereby the return value is mistaken in some
    cases.
-   Fixed bug in [vector]{style="font-family: Courier New;"} whereby it
    was telling the allocator the wrong amount of memory being freed.
-   Fixed bug in the [remove\_if]{style="font-family: Courier New;"}
    algorithm whereby it would return the wrong value.
-   Added workaround for GCC 2.x deficiency to
    [deque]{style="font-family: Courier New;"} subarray size
    calculation.

Changes

-   [vector]{style="font-family: Courier New;"} assignment from an empty
    [vector]{style="font-family: Courier New;"} previously resulted in a
    an allocation of zero bytes. Now it detects this situation and
    allocates no memory. This is an efficiency improvement.
-   [vector\_map]{style="font-family: Courier New;"},
    [vector\_multimap]{style="font-family: Courier New;"},
    [vector\_set]{style="font-family: Courier New;"},
    [vector\_multiset]{style="font-family: Courier New;"} modified to
    allow usage of a
    [fixed\_vector]{style="font-family: Courier New;"} or any other
    container that doesn\'t use allocators as their underlying
    container. 
-   Modified calculation for default
    [deque]{style="font-family: Courier New;"} subarray size to be a
    little more finely tuned.
-   Modified [iterator]{style="font-family: Courier New;"}
    [operator==/!=]{style="font-family: Courier New;"} definitions to be
    more flexible in the presence of questionable or illegal use of
    [rel\_ops]{style="font-family: Courier New;"}.
-   Redefined how
    [has\_trivial\_relocate]{style="font-family: Courier New;"} works
    and modified code to be consistent with it.

\

------------------------------------------------------------------------

[Version 0.96.00]{style="font-weight: bold;"} (Nov 23, 2005)\

Additions

-   Added the [identical]{style="font-family: Courier New;"} algorithm
    to [algorithm.h]{style="font-family: Courier New;"}. This algorithm
    is like equal but also tests for sequence equality as well; such a
    test cannot be done as efficiently otherwise.
-   Added [fixed\_substring]{style="font-family: Courier New;"}. This
    implements a string which is merely a reference to a character array
    segment. It allocates no memory and on construction doesn\'t even
    copy memory. It is usually used for efficiently working on a
    substring of another string as if that substring itself were a
    string object.
-   Added [numeric.h]{style="font-family: Courier New;"} to complete std
    [\<numeric\>]{style="font-family: Courier New;"}.

Bug Fixes

-   The [basic\_string(CtorSprintf, allocator\_type, const
    value\_type\*, \...)]{style="font-family: Courier New;"} constructor
    has been removed, as it collides with the
    [basic\_string(CtorSprintf, const value\_type\*,
    \...)]{style="font-family: Courier New;"} constructor.
-   Fixed a bug in BenchmarkSort.cpp which caused a crash in the
    benchmark test on PS3 in an optimized build. It was an alignment
    problem.
-   Fixed a bug in
    [internal/fixed\_allocator.h]{style="font-family: Courier New;"}
    which resulted in [fixed\_string]{style="font-family: Courier New;"}
    and [fixed\_vector]{style="font-family: Courier New;"} compiler
    errors when their
    [bEnableOverflow]{style="font-family: Courier New;"} template
    parameter was false.
-   Added an \"optimization barrier\" to
    [list.h]{style="font-family: Courier New;"} which fixes a code
    generation problem with GCC when used with -O2 or higher
    optimizations. This seems to be due to a bug in GCC, but more
    investigation would be required to verify so.
-   Added an initialize.xml file for NAnt / Framework 2 builds and which
    defines variables such as
    [package.EASTL.includedirs]{style="font-family: Courier New;"}.

Changes

-   Removed some silly [explicit]{style="font-family: Courier New;"}
    declarations that somehow got onto functions that didn\'t need them.
-   Modified the [list]{style="font-family: Courier New;"} and
    [slist]{style="font-family: Courier New;"} implementations to allow
    the user to more easily view the list data in a debugger.
-   Revised [shared\_ptr]{style="font-family: Courier New;"} in order to
    make it more like Boost
    [shared\_ptr]{style="font-family: Courier New;"} but with striving
    to avoid overhead to the extent possible.
-   [vector::compact]{style="font-family: Courier New;"} changed to be
    [vector::set\_capacity]{style="font-family: Courier New;"} and make
    it more generalized in functionality.

\

------------------------------------------------------------------------

[Version 0.95.00]{style="font-weight: bold;"} (Oct 20, 2005)\

Additions

-   Added this Changes.html file.
-   fixed-size containers now have a (template) option to resort to the
    main heap if memory is exhausted.
-   Added some new FAQ entries.
-   Added front() and back() to basic\_string, so now it is like vector.
-   Added a nant/Framework 2 build file. It will need some massaging
    before considered finished.
-   Added intrusive\_hash\_set, \_multiset, \_map, \_multimap. 

Bug Fixes

-   list and slist::get\_allocator() were previously protected member
    data; they have been made public.
-   fixed\_vector was using a cache of T values instead of void memory.
    This has been fixed.
-   hashtable bug fixed whereby a hashtable created with zero buckets
    would crash if elements were added to the hashtable.

Changes

-   Disabed some VC++ warnings.
-   The fixed container buffer mechanism has been reworked to better
    support alignment.
-   All uses of \#include \"\" were changed to \#include \<\>. It seems
    there are cases where the latter can result in more efficient
    development procedures within teams, as the latter indicates a
    \"system\" header that isn\'t likely to change for a given
    application.

\

------------------------------------------------------------------------

[Version 0.94.00]{style="font-weight: bold;"} (Oct 3, 2005)

Additions

-   Defined eastl\_size\_t as the universal size\_type for containers.
-   Added explicit support for overriding config.h.
-   Added FAQ entries.

Bug Fixes

-   Fixed swap implementation for some fixed fixed containers.
-   Warning fixed in vector.h.

Changes

-   Allocator mechanism changed to hopefully final format.
-   Revised documentation to be in line with new allocator mechanism.

\

------------------------------------------------------------------------

[Earlier versions]{style="font-weight: bold;"}\
\
Earlier versions were too volatile to make change lists worthwhile, and
earlier versions were mostly unsupported anyway.\
\
\

------------------------------------------------------------------------

End of document\
\
\
\
\
\
\
\
\
\
\
\
\
\
\
\
