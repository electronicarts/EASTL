

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
	-loses on some micro-benchmarks due to index-based iteration, not pointer based-iteration
	-requires a lot of optimization and inlining in order to keep performance up: there are a lot of function calls required as part of the per-element operations.
		-thankfully in every case investigated with full optimizations on, the compiler does generate the code desired with little overhead!
	-Can always just access the data like a bunch of C-style-arrays, and use t_v to create the memory allocation and manage the pointers, at the very least.
	
	
	-in comparison to vectors, performance can vary
		-operations that don't touch all of the data tend to win
		-operations that do touch all of the data tend to lose by a bit (anywhere from 10-30% has been observed)
			e.g. remove_if, sort, etc...
			This is more-or-less expected, though. More work to touch same amount of memory, after all.
		-So know what you're doing!


from Core i7 3770K @ 3.5GHz, w/ DDR3 1600 memory
		
tuple_vector<AutoRefCount>/erase            |       1741814    1.7 ms |       1744732    1.7 ms |       1.00

tuple_vector<MovableType>/erase             |     104555232  104.6 ms |     106312216  106.3 ms |       0.98
tuple_vector<MovableType>/reallocate        |       1294327    1.3 ms |       1684347    1.7 ms |       0.77         -

tuple_vector<uint64,Padding>/erase          |      34672924   34.7 ms |      32875392   32.9 ms |       1.05
tuple_vector<uint64,Padding>/insert         |      40956116   41.0 ms |      32625980   32.6 ms |       1.26
tuple_vector<uint64,Padding>/iteration      |        247081  247.1 us |         80512   80.5 us |       3.07         +
tuple_vector<uint64,Padding>/operator[]     |        695733  695.7 us |         81096   81.1 us |       8.58         +
tuple_vector<uint64,Padding>/push_back      |      10029944   10.0 ms |       5993812    6.0 ms |       1.67         +
tuple_vector<uint64,Padding>/sort           |       8150441    8.2 ms |      10071075   10.1 ms |       0.81

tuple_vector<uint64>/erase                  |       3380071    3.4 ms |       3452124    3.5 ms |       0.98
tuple_vector<uint64>/insert                 |       3385613    3.4 ms |       3413910    3.4 ms |       0.99
tuple_vector<uint64>/iteration              |         56300   56.3 us |         81388   81.4 us |       0.69         -
tuple_vector<uint64>/operator[]             |         67386   67.4 us |         61843   61.8 us |       1.09
tuple_vector<uint64>/push_back              |       1253780    1.3 ms |        818253  818.3 us |       1.53         +
tuple_vector<uint64>/sort                   |       5840372    5.8 ms |       7269471    7.3 ms |       0.80

vector<AutoRefCount>/erase                  |       1270991    1.3 ms |       1207980    1.2 ms |       1.05

vector<MovableType>/erase                   |     104378752  104.4 ms |     109444040  109.4 ms |       0.95
vector<MovableType>/reallocate              |       1461187    1.5 ms |       1541991    1.5 ms |       0.95

vector<uint64>/erase                        |       4273877    4.3 ms |       3553348    3.6 ms |       1.20
vector<uint64>/insert                       |       4827255    4.8 ms |       4788166    4.8 ms |       1.01
vector<uint64>/iteration                    |         71469   71.5 us |         77304   77.3 us |       0.92
vector<uint64>/operator[]                   |         90722   90.7 us |         87221   87.2 us |       1.04
vector<uint64>/push_back                    |       1641465    1.6 ms |       1186102    1.2 ms |       1.38         +
vector<uint64>/sort                         |       7703830    7.7 ms |       8241748    8.2 ms |       0.93
		
Other drawbacks
	-No symbolic reference to members!! Strongest suggestion is to make enumerated values to refer to members.
	