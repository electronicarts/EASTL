

Introduction to tuple_vector

tuple_vector is a data container that is designed to abstract and simplify
the handling of a "structure of arrays" layout of data in memory. In
particular, it mimics the interface of vector, including functionality to do
inserts, erases, push_backs, and random-access. It also provides a
RandomAccessIterator and corresponding functionality, making it compatible
with most STL (and STL-esque) algorithms such as ranged-for loops, find_if,
remove_if, or sort.

When used or applied properly, this container can improve performance of
some algorithms through cache-coherent data accesses or allowing for
sensible SIMD programming, while keeping the structure of a single
container, to permit a developer to continue to use existing algorithms in
STL and the like.



Review of "Structure of arrays" data layouts

When trying to improve the performance of some code, it can sometimes be
desirable to transform how some data is stored in memory to be laid out not as
an "array of structures", but as a "structure of arrays". That is, instead of storing a series of objects as a single contiguous chunk of memory, one or 
more data members are instead stored as separate chunks of member that are 
handled and accessed in a parallel fashion.

This can be beneficial in two primary respects:

1) To improve the cache coherency of the data accesses, e.g. by utilizing more
data that is loaded per cache line loaded from memory, and thereby reducing 
the amount of time waiting on memory accesses from off-CPU memory.
This presentation from Mike Acton touches on this, among other things: https://www.youtube.com/watch?v=rX0ItVEVjHc

2) To allow the data to be more easily loaded and utilized by SIMD kernels, 
allowing for data to be more easily accessed and manipulated by doing single
loads into SIMD registers.
This is touched on in this presentation from Andreas Fredriksson for writing
code with SIMD intrinsics: http://www.gdcvault.com/play/1022249/SIMD-at-Insomniac-Games-How
...and as well in the ISPC performance guide when writing ISPC kernels: https://ispc.github.io/perfguide.html



How TupleVecImpl works

tuple_vector and fixed_tuple_vector both inherit from TupleVecImpl, which provides the bulk of the functionality for those data containers. It manages the memory allocated, marshals data members to each array of memory, generates the necessary iterators, and so on. 

When a tuple_vector is declared, it is alongside a list of types, or "elements", indicating what data to store in the container, similar to tuple. TupleVecImpl then inherits from a series of TupleVecLeaf structures - one for each element - each of which has a pointer to its own block of memory. When dereferencing the container, to fetch a tuple of references or otherwise fetching pointers to the memory the tuple_vector handles, it is these pointers that are utilized or fetched. 

While each TupleVecLeaf contains a pointer to its own block of memory, they are not individual memory allocations. When TupleVecImpl needs to grow its capacity, it calculates the total size needed for a single allocation, given the target number of objects to manage and the alignment requirements for each element's type, as well as pointers into the memory allocation for each element. Each TupleVecLeaf is then provided with these pointers. From there, many of the interactions with TupleVecImpl, to modify or access members of the container, then reference each TupleVecLeaf's data pointer in series, using parameter packs to repeat each operation for each parent TupleVecLeaf.



How tuple_vector's iterator works

TupleVecImpl provides a definition to an iterator type, TupleVecIter, as well was a move_iterator customization for it. As mentioned above, TupleVecIter provides all of the functionality to operate as a RandomAccessIterator. However, when it is dereferenced, it provides a tuple of references, similar to at() or operator[] on TupleVecImpl, as opposed to a reference of some other type. Similarly, there is a customization of move_iterator for TupleVecIter, which will return a tuple of rvalue-references.

The way that TupleVecIter operates internally is to track an index into the container, as well as a copy of all of the TupleVecImpl's TupleVecLeaf pointers at the time of the iterator's construction. Modifying the iterator, then, involves just changing the index, and dereferencing the iterator into the tuple of references involves deferencing each pointer with an offset specified by that index.



How to work with tuple_vector, and where to use it
	
	-As a drop-in replacement for vector
		-Go over list of supported functions
	-As a way to manage a struct of arrays
		-esp. fetching data() or get<...>()
		-push_back, etc, automatically marshalling stuff member-by-member
	-Demonstrate utilization with ispc

		
		
Performance comparisons/discussion

A small benchmark suite for tuple_vector is included when running the EASTLBenchmarks project. It provides the following output on a Core i7 3770k (Skylake) at 3.5GHz, with DDR3 1600 (PC 12800) memory:
	
tuple_vector<AutoRefCount>/erase        |   1.7 ms |   1.7 ms | 1.00 
                                                                     
tuple_vector<MovableType>/erase         | 104.6 ms | 106.3 ms | 0.98 
tuple_vector<MovableType>/reallocate    |   1.3 ms |   1.7 ms | 0.77  -
                                                                     
tuple_vector<uint64>/erase              |   3.4 ms |   3.5 ms | 0.98 
tuple_vector<uint64>/insert             |   3.4 ms |   3.4 ms | 0.99 
tuple_vector<uint64>/iteration          |  56.3 us |  81.4 us | 0.69  -
tuple_vector<uint64>/operator[]         |  67.4 us |  61.8 us | 1.09 
tuple_vector<uint64>/push_back          |   1.3 ms | 818.3 us | 1.53  +
tuple_vector<uint64>/sort               |   5.8 ms |   7.3 ms | 0.80 
                                                                     
tuple_vector<uint64,Padding>/erase      |  34.7 ms |  32.9 ms | 1.05 
tuple_vector<uint64,Padding>/insert     |  41.0 ms |  32.6 ms | 1.26 
tuple_vector<uint64,Padding>/iteration  | 247.1 us |  80.5 us | 3.07  +
tuple_vector<uint64,Padding>/operator[] | 695.7 us |  81.1 us | 8.58  +
tuple_vector<uint64,Padding>/push_back  |  10.0 ms |   6.0 ms | 1.67  +
tuple_vector<uint64,Padding>/sort       |   8.2 ms |  10.1 ms | 0.81 
                                                                     
vector<AutoRefCount>/erase              |   1.3 ms |   1.2 ms | 1.05 
                                                                     
vector<MovableType>/erase               | 104.4 ms | 109.4 ms | 0.95 
vector<MovableType>/reallocate          |   1.5 ms |   1.5 ms | 0.95 
                                                                     
vector<uint64>/erase                    |   4.3 ms |   3.6 ms | 1.20 
vector<uint64>/insert                   |   4.8 ms |   4.8 ms | 1.01 
vector<uint64>/iteration                |  71.5 us |  77.3 us | 0.92 
vector<uint64>/operator[]               |  90.7 us |  87.2 us | 1.04 
vector<uint64>/push_back                |   1.6 ms |   1.2 ms | 1.38  +
vector<uint64>/sort                     |   7.7 ms |   8.2 ms | 0.93 
		
First off, tuple_vector's behaviour on single types - which is compared against std::vector behaviour - is comparable, as expected. For a single type, it behaves very smiilarly to eastl::vector, with the major notable exception being the iteration case. This is a consequence of the iterator design, where it works with indices, not pointers, so the code generation suffered slightly, in this compute-bound scenarion. This is worth noting as a demonstration of a case where falling back to pointer-based iteration by fetching the begin and end pointers of that tuple_vector element may be preferable, instead of using the iterator constructs.

The bracket of "tuple_vector<uint64,Padding>" tests are where things get more interesting, though. This is a comparison between a single std::vector with a structure containing a uint64 and 56 bytes of padding, and a tuple_vector with two elements: one for uint64 and one for 56 bytes of padding. The erase, insert, push_back, and sort cases all perform at a similar relative rate as they did in the tuple_vector<uint64> tests - demonstrating that operations that have to touch all of elements do not have a significant change in performance. However, iteration and operator[] are very different, because those only access the uint64 member of both vector and tuple_vector to run some operation. The iteration test now runs 3x fsster whereas before it ran 0.7x as fast, and operator[] runs 8.5x faster, instead of 1.1x. This demonstrates some of the utility of tuple_vector, in that these algorithms end up being limited by the CPU's compute capabilities, as opposed to being limited by how fast they can load memory in from DRAM. 
		
In a series of other tests, generally speaking, tuple_vector tends to perform on par with manual management of multiple arrays in many algorithms and operations, often generating the same code. As well, significant degress of inlining and optimization are required to get the most out of it. Compared to accessing a series of arrays or vectors, tuple_vector does perform a multitude of extra function calls internally in order to manage the various elements, or interact with eastl::tuple through its standard interface, so running in debug configurations can run significantly slower in some cases, i.e. sometimes running at 0.2x the speed compared to vector.
		
		
		
Other notes and drawbacks

This will be experienced shortly after using tuple_vector in most capacities, but it should be noted that the most significant drawback is that there is no way to symbolically reference every element of the tuple_vector - much in the same way as tuple. For example, if translating a struct such as...

struct Entity
{
	float x, y, z;
	float lifetime;
};

...to tuple_vector, it will exist as:

tuple_vector<float, float, float, float> entityVec;

...and can only be accessed in a manner like entityVec.get<3>() to refer to the lifetime. With existing tools, the only good alternatives is to encapsulate each float as a separate struct to give it unique typenames...

struct entityX { float val; };
struct entityY { float val; };
struct entityZ { float val; };
struct entityLifetime { float val; };

tuple_vector<entityX, entityY, entityZ, entityLifetime> entityVec;

...and then access each element by typename like entityVec.get<entityLifetime>(). Or, creating an enumerated value to replace the indices...

enum EntityTypeEnum
{
	x = 0,
	y = 1,
	z = 2,
	lifetime = 3
};

tuple_vector<float, float, float, float> entityVec;

...and then access each element by the enumerated value: entityVec.get<lifetime>().

Either way, there is a fairly significant maintenance and readability issue around this, arguably more severe than with the existing tuple functionality because that is generally not intended for structures with long lifetime.

Ideally, if the language could be mutated to accomodate such a thing, it would be best to have some combination or mix of typenames and symbolic names in the declaration, e.g. someting like

tuple_vector<float x, float y, float z, float lifetime> entityVec;

and even more than accessing elements by a symbol, e.g. entityVec.get<lifetime>(), it may be interesting if the necessary get functions could be even automatically generated through a reflection system, e.g. entityVec.get_lifetime(), but that remains a pipe dream for now.