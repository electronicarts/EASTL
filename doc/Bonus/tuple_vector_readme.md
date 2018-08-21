## Introduction to tuple_vector

`tuple_vector` is a data container that is designed to abstract and simplify
the handling of a "structure of arrays" layout of data in memory. In
particular, it mimics the interface of `vector`, including functionality to do
inserts, erases, push_backs, and random-access. It also provides a
`RandomAccessIterator` and corresponding functionality, making it compatible
with most STL (and STL-esque) algorithms such as ranged-for loops, `find_if`,
`remove_if`, or `sort`.

When used or applied properly, this container can improve performance of
some algorithms through cache-coherent data accesses or allowing for
sensible SIMD programming, while keeping the structure of a single
container, to permit a developer to continue to use existing algorithms in
STL and the like.

## Review of "Structure of arrays" data layouts

When trying to improve the performance of some code, it can sometimes be
desirable to transform how some data is stored in memory to be laid out not as
an "array of structures", but as a "structure of arrays". That is, instead of
storing a series of objects as a single contiguous chunk of memory, one or 
more data members are instead stored as separate chunks of memory that are 
handled and accessed in parallel to each other.

This can be beneficial in two primary respects:

1) To improve the cache coherency of the data accesses, e.g. by utilizing more
data that is loaded per cache line loaded from memory, and thereby reducing 
the amount of time waiting on memory accesses from off-CPU memory.
This presentation from Mike Acton touches on this, among other things:
https://www.youtube.com/watch?v=rX0ItVEVjHc

2) To allow the data to be more easily loaded and utilized by SIMD kernels, 
by being able to load memory directly into a SIMD register.
This is touched on in this presentation from Andreas Fredriksson for writing
code with SIMD intrinsics:
http://www.gdcvault.com/play/1022249/SIMD-at-Insomniac-Games-How
...and as well in this guide for writing performant ISPC kernels:
https://ispc.github.io/perfguide.html

## How TupleVecImpl works

`tuple_vector` inherits from `TupleVecImpl`, which
provides the bulk of the functionality for those data containers. It manages
the memory allocated, marshals data members to each array of memory, generates
the necessary iterators, and so on. 

When a `tuple_vector` is declared, it is alongside a list of types, or "tuple
elements", indicating what data to store in the container, similar to how `tuple`
operates. `TupleVecImpl` uses this list of tuple elements to then inherit from a series of
`TupleVecLeaf` structures, which each have their own pointer to an array of their
corresponding type in memory. When dereferencing the container, either to fetch a
tuple of references or just fetching pointers to the memory, it is these pointers
that are utilized or fetched. 

While each `TupleVecLeaf` contains a pointer to its own block of memory, they
are not individual memory allocations. When `TupleVecImpl` needs to grow its
capacity, it calculates the total size needed for a single allocation, taking
into account the number of objects for the container, the size of each tuple
element's type, and the alignment requirements for each type. Pointers into the
allocation for each tuple element are also determined at the same time, which
are passed to each `TupleVecLeaf`. From there, many of the interactions with
`TupleVecImpl`, to modify or access members of the container, then reference
each `TupleVecLeaf`'s data pointer in series, using parameter packs to repeat
each operation for each parent `TupleVecLeaf`.

## How tuple_vector's iterator works

`TupleVecImpl` provides a definition to an iterator type, `TupleVecIter`.
As mentioned above, `TupleVecIter` provides all of the functionality to operate
as a `RandomAccessIterator`. When it is dereferenced, it provides a tuple of
references, similar to `at()` or `operator[]` on `TupleVecImpl`, as opposed to
a reference of some other type. As well, a customization of `move_iterator` for
`TupleVecIter` is provided, which will return a tuple of rvalue-references.

The way that `TupleVecIter` operates internally is to track an index into the
container, as well as a copy of all of the `TupleVecImpl`'s `TupleVecLeaf`
pointers at the time of the iterator's construction. As a result, modifying the
iterator involves just changing the index, and dereferencing the iterator into
the tuple of references involves dereferencing each pointer with an offset
specified by that index.

Of the various ways of handling the multitude of references, this tended to
provide the best code-generation. For example, having a tuple of pointers that
are collectively modified with each iterator modification resulted in the compiler
not being able to accurately determine which pointers were relevant to the final
output of some function, creating many redundant operations. Similarly, having
the iterator refer to the source `TupleVecImpl` for the series of pointers
often resulted in extra, unnecessary, data hops to the `TupleVecImpl` to repeatedly
fetch data that was not practically mutable, but theoretically mutable. While this
solution is the heaviest in terms of storage, the resulted assembly tends to be
competitive with traditional structure-of-arrays setups.

## How to work with tuple_vector, and where to use it
	
Put simply, `tuple_vector` can be used as a replacement for `vector`. For example,
instead of declaring a structure and vector as:

```
struct Entity
{
	bool active;
	float lifetime;
	Vec3 position;
}
vector<Entity> entityVec;
```

...the `tuple_vector` equivalent of this can be defined as:

```
tuple_vector<bool, float, Vec3> entityVec;
```

In terms of how `tuple_vector` is modified and accessed, it has a similar
featureset as `vector`, except where `vector` would accept or return a single
value, it instead accepts or returns a tuple of values or unstructured series
of equivalent arguments.

For example, the following functions can be used to access the data, either by
fetching a tuple of references to a series of specific values, or the data
pointers to the tuple elements:

```
tuple<bool&, float&, Vec3&> operator[](size_type)
tuple<bool&, float&, Vec3&> at(size_type)
tuple<bool&, float&, Vec3&> iterator::operator*()
tuple<bool&&, float&&, Vec3&&> move_iterator::operator*()
tuple<bool*, float*, Vec3*> data()

// extract the Ith tuple element pointer from the tuple_vector
template<size_type I>
T* get<I>()
// e.g. bool* get<0>(), float* get<1>(), and Vec3* get<2>()

// extract the tuple element pointer of type T from the tuple_vector
// note that this function can only be used if there is one instance
// of type T in the tuple_vector's elements
template<typename T>
T* get<T>()
// e.g. bool* get<bool>(), float* get<float>(), and Vec3* get<Vec3>()
```

And `push_back(...)` has the following overloads, accepting either values or tuples as needed.

```
tuple<bool&, float&, Vec3&> push_back()
push_back(const bool&, const float&, const Vec3&)
push_back(tuple<const bool&, const float&,const  Vec3&>)
push_back(bool&&, float&&, Vec3&&)
push_back(tuple<bool&&, float&&, Vec3&&>)		
```
...and so on, and so forth, for others like the constructor, `insert(...)`,
`emplace(...)`, `emplace_back(...)`, `assign(...)`, and `resize(...)`.

As well, note that the tuple types that are accepted or returned for 
`tuple_vector<Ts...>` have typedefs available in the case of not wanting to use
automatic type deduction:
```
typedef eastl::tuple<Ts...> value_tuple;
typedef eastl::tuple<Ts&...> reference_tuple;
typedef eastl::tuple<const Ts&...> const_reference_tuple;
typedef eastl::tuple<Ts*...> ptr_tuple;
typedef eastl::tuple<const Ts*...> const_ptr_tuple;
typedef eastl::tuple<Ts&&...> rvalue_tuple;
```
With this, and the fact that the iterator type satisfies
the `RandomAccessIterator` requirements, it is possible to use `tuple_vector` in
most ways and manners that `vector` was previously used, with few structural
differences.

However, even if not using it strictly as a replacement for `vector`, it is
still useful as a tool for simplifying management of a traditional structure of
arrays. That is, it is possible to use `tuple_vector` to just perform a single
large memory allocation instead of a series of smaller memory allocations,
by sizing the `tuple_vector` as needed, fetching the necessary pointers with
`data()` or `get<...>()`, and carrying on normally.

One example where this can be utilized is with ISPC integration. Given the
following ISPC function definition:

	export void simple(uniform float vin[], uniform float vfactors[], uniform float vout[], uniform int size);

...which generates the following function prototype for C/C++ usage:

	extern void simple(float* vin, float* vfactors, float* vout, int32_t size);
		
...this can be utilized with some raw float arrays:
```
float* vin = new float[NumElements];
float* vfactors = new float[NumElements];
float* vout = new float[NumElements];

// Initialize input buffer
for (int i = 0; i < NumElements; ++i)
{
	vin[i] = (float)i;
	vfactors[i] = (float)i / 2.0f;
}

// Call simple() function from simple.ispc file
simple(vin, vfactors, vout, NumElements);

delete vin;
delete vfactors;
delete vout;
```
or, with `tuple_vector`:

```
tuple_vector<float, float, float> simpleData(NumElements);
float* vin = simpleData.get<0>();
float* vfactors = simpleData.get<1>();
float* vout = simpleData.get<2>();

// Initialize input buffer
for (int i = 0; i < NumElements; ++i)
{
	vin[i] = (float)i;
	vfactors[i] = (float)i / 2.0f;
}

// Call simple() function from simple.ispc file
simple(vin, vfactors, vout, NumElements);
```
		
`simpleData` here only has a single memory allocation during its construction,
instead of the three in the first example, and also automatically releases the
memory when it falls out of scope.

It is possible to also skip a memory allocation entirely, in some circumstances.
EASTL provides "fixed" counterparts of many data containers which allows for a 
data container to have an inlined buffer of memory. For example, 
`eastl::vector<typename T>` has the following counterpart:

	eastl::fixed_vector<typename T, size_type nodeCount, bool enableOverflow = true>

This buffer allows for enough space to hold a `nodeCount` number of `T` objects,
skipping any memory allocation at all, until the requested size becomes
greater than `nodeCount` - assuming `enableOverflow` is True.

There is a similar counterpart to `eastl::tuple_vector<typename... Ts>` available as well:

	eastl::fixed_tuple_vector<size_type nodeCount, bool enableOverflow, typename... Ts>

This does the similar legwork in creating an inlined buffer, and all of the
functionality of `tuple_vector` otherwise is supported. Note the slight
difference in declaration, though: `nodeCount` and `enableOverflow` are defined
first, and `enableOverflow` is not a default parameter. This change arises out
of restrictions surrounding variadic templates, in that they must be declared
last, and cannot be mixed with default template parameters.

Lastly, `eastl::vector` and other EASTL data containers support custom Memory Allocator
types, through their template parameters. For example, `eastl::vector`'s full declaration
is actually:

	eastl::vector<typename T, typename AllocatorType = EASTLAllocatorType>

However, because such a default template parameter cannot be used with
variadic templates, a separate type for `tuple_vector` is required for such a
definition:

	eastl::tuple_vector_alloc<typename AllocatorType, typename... Ts>

Note that `tuple_vector` uses EASTLAllocatorType as the allocator.

## Performance comparisons/discussion

A small benchmark suite for `tuple_vector` is included when running the
EASTLBenchmarks project. It provides the following output on a Core i7 3770k
(Skylake) at 3.5GHz, with DDR3-1600 memory.

The `tuple_vector` benchmark cases compare total execution time of similar
algorithms run against `eastl::tuple_vector` and `std::vector`, such as
erasing or inserting elements, iterating through the array to find a specific
element, sum all of the elements together via operator[] access, or just
running `eastl::sort` on the data containers. More information about the
EASTLBenchmarks suite can be found in EASTL/doc/EASTL Benchmarks.html
	
Benchmark | STD execution time | EASTL execution time | Ratio
--------- | -------- | ---------- | -----
`tuple_vector<AutoRefCount>/erase       ` |   1.7 ms |   1.7 ms | 1.00 
`tuple_vector<MovableType>/erase        ` | 104.6 ms | 106.3 ms | 0.98 
`tuple_vector<MovableType>/reallocate   ` |   1.3 ms |   1.7 ms | 0.77  -
 |  |  | 
`tuple_vector<uint64>/erase             ` |   3.4 ms |   3.5 ms | 0.98 
`tuple_vector<uint64>/insert            ` |   3.4 ms |   3.4 ms | 0.99 
`tuple_vector<uint64>/iteration         ` |  56.3 us |  81.4 us | 0.69  -
`tuple_vector<uint64>/operator[]        ` |  67.4 us |  61.8 us | 1.09 
`tuple_vector<uint64>/push_back         ` |   1.3 ms | 818.3 us | 1.53  +
`tuple_vector<uint64>/sort              ` |   5.8 ms |   7.3 ms | 0.80 
 |  |  | 
`tuple_vector<uint64,Padding>/erase     ` |  34.7 ms |  32.9 ms | 1.05 
`tuple_vector<uint64,Padding>/insert    ` |  41.0 ms |  32.6 ms | 1.26 
`tuple_vector<uint64,Padding>/iteration ` | 247.1 us |  80.5 us | 3.07  +
`tuple_vector<uint64,Padding>/operator[]` | 695.7 us |  81.1 us | 8.58  +
`tuple_vector<uint64,Padding>/push_back ` |  10.0 ms |   6.0 ms | 1.67  +
`tuple_vector<uint64,Padding>/sort      ` |   8.2 ms |  10.1 ms | 0.81 
 |  |  | 
`vector<AutoRefCount>/erase             ` |   1.3 ms |   1.2 ms | 1.05 
`vector<MovableType>/erase              ` | 104.4 ms | 109.4 ms | 0.95 
`vector<MovableType>/reallocate         ` |   1.5 ms |   1.5 ms | 0.95 
 |  |  | 
`vector<uint64>/erase                   ` |   4.3 ms |   3.6 ms | 1.20 
`vector<uint64>/insert                  ` |   4.8 ms |   4.8 ms | 1.01 
`vector<uint64>/iteration               ` |  71.5 us |  77.3 us | 0.92 
`vector<uint64>/operator[]              ` |  90.7 us |  87.2 us | 1.04 
`vector<uint64>/push_back               ` |   1.6 ms |   1.2 ms | 1.38  +
`vector<uint64>/sort                    ` |   7.7 ms |   8.2 ms | 0.93 
		
First off, `tuple_vector<uint64>`'s performance versus `std::vector<uint64>` is
comparable, as expected, as the `tuple_vector`'s management for one type
becomes very similar to just a regular vector. The major notable exception is
the iteration case, which runs `eastl::find_if`. This
performance differences is a consequence of the iterator design, and how
it works with indices, not a direct pointer, so the code generation suffers slightly
in this compute-bound scenario. This is worth noting as a demonstration of a
case where falling back to pointer-based iteration by fetching the `begin` and
`end` pointers of that tuple element may be preferable, instead of using the
iterator constructs.

The set of `tuple_vector<uint64,Padding>` tests are more interesting. 
This is a comparison between a single `std::vector` with a
structure containing a `uint64` and 56 bytes of padding, and a `tuple_vector` with
two elements: one for `uint64` and one for 56 bytes of padding. The erase,
insert, push_back, and sort cases all perform at a similar relative rate as
they did in the `tuple_vector<uint64>` tests - demonstrating that operations
that have to touch all of elements do not have a significant change in
performance.

However, iteration and operator[] are very different, because
those only access the `uint64` member of both `vector` and `tuple_vector` to run
some operation. The iteration test now runs 3x faster whereas before it ran
0.7x as fast, and operator[] runs 8.5x faster, instead of 1.1x. This
demonstrates some of the utility of `tuple_vector`, in that these algorithms end
up being limited by the CPU's compute capabilities, as opposed to being
limited by how fast they can load memory in from DRAM. 
		
In a series of other tests, generally speaking, `tuple_vector` tends to perform
on par with manual management of multiple arrays in many algorithms and
operations, often even generating the same code. It should be noted that
significant degrees of inlining and optimization are required to get the most out
of `tuple_vector`. Compared to accessing a series of arrays or vectors,
`tuple_vector` does perform a multitude of extra trivial function calls internally
in order to manage the various elements, or interact with `eastl::tuple` through
its interface, so running in debug configurations can run significantly slower
in some cases, e.g. sometimes running at 0.2x the speed compared to vector.
		
## The problem of referencing tuple elements

This will be experienced shortly after using `tuple_vector` in most capacities,
but it should be noted that the most significant drawback is that there is no
way to **symbolically** reference each tuple element of the `tuple_vector` - much
in the same way as `tuple`. For example, if translating a struct such as...

```
struct Entity
{
	float x, y, z;
	float lifetime;
};
```
...to `tuple_vector`, it will exist as:

```
tuple_vector<float, float, float, float> entityVec;
```

...and can only be accessed in a manner like `entityVec.get<3>()` to refer to
the `lifetime` member. With existing tools, the only good alternatives are to
encapsulate each float as a separate struct to give it unique typenames...

```
struct entityX { float val; };
struct entityY { float val; };
struct entityZ { float val; };
struct entityLifetime { float val; };

tuple_vector<entityX, entityY, entityZ, entityLifetime> entityVec;
```
...and then access each tuple element by typename like
`entityVec.get<entityLifetime>()`; or, creating an enumerated value to replace
the indices...

```
enum EntityTypeEnum
{
	entityX = 0,
	entityY = 1,
	entityZ = 2,
	entityLifetime = 3
};

tuple_vector<float, float, float, float> entityVec;
```

...and then access each tuple element by the enumerated value:
`entityVec.get<entityLifetime>()`.

Either way, there is a fairly significant maintenance and readability issue
around this. This is arguably more severe than with `tuple` on its own
because that is generally not intended for structures with long lifetime.

Ideally, if the language could be mutated to accommodate such a thing, it would
be good to have some combination of typenames and symbolic names in the
declaration, e.g. something like

```
tuple_vector<float x, float y, float z, float lifetime> entityVec;
```
and be able to reference the tuple elements not just by typename or index, but
through their corresponding symbol, like `entityVec.get<lifetime>()`. Or, it may
be interesting if the necessary `get` functions could be even automatically
generated through a reflection system, e.g. `entityVec.get_lifetime()`.
All of this remains a pipe dream for now.
