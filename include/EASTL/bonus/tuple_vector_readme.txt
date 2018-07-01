

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
	-in comparison to structure of vectors, performance is fair
	-loses on some micro-benchmarks due to index-based iteration
	


Other drawbacks
	-No symbolic reference to members 
	