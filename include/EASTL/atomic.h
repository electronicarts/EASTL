/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////////


#ifndef EASTL_ATOMIC_H
#define EASTL_ATOMIC_H

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif


/////////////////////////////////////////////////////////////////////////////////
//
//  Below is the documentation of the API of the eastl::atomic<T> library.
//  This includes class and free functions.
//  Anything marked with a '+' in front of the name is an extension to the std API.
//


/////////////////////////////////////////////////////////////////////////////////
//
// eastl::atomic<T> memory_order API
//
//  See below for full explanations on the memory orders and their guarantees.
//
//  - eastl::memory_order_relaxed
//  - eastl::memory_order_acquire
//  - eastl::memory_order_release
//  - eastl::memory_order_acq_rel
//  - eastl::memory_order_seq_cst
//  - +eastl::memory_order_read_depends
//


/////////////////////////////////////////////////////////////////////////////////
//
// eastl::atomic<T> class API
//
//   All jargon and prerequisite knowledge is explained below.
//
//   Unless otherwise specified all orders except read_depends is a valid order
//   on the given operation.
//   Unless otherwise specified all operations are valid on all types T.
//   If no order is provided, seq_cst memory ordering is used for the operation.
//
//   - atomic()        : Value-initializes the underlying object as T{}.
//
//   - atomic(T)       : Initializes the underlying object with a copy of T.
//
//   - T operator=(T)  : Atomically assigns T as store(T, seq_cst).
//
//   - is_lock_free()  : true if the operations are lockfree. Always true for eastl.
//
//   - store(T, order) : Atomically stores T affecting memory according to order.
//                     : Valid orders are relaxed, release, and seq_cst.
//
//   - T load(order)   : Atomically loads T affecting memory according to order.
//                     : Valid orders are relaxed, acquire, and seq_cst.
//                     : If T is a pointer type, read_depends is another valid order.
//
//   - operator T()    : Atomically loads T as load(T, seq_cst).
//
//   - T exchange(T, order) : Atomically performs a RMW that replaces the current value with T.
//                          : Memory is affected according to order.
//                          : Returns the previous value stored before the RMW operation.
//
//   - bool compare_exchange_weak(T&, T, successOrder, failOrder)
//        : Atomically compares the value stored with that of T& and if equal replaces it with T.
//        : This is a RMW operation.
//        : If the comparison fails, loads the observed value into T&. This is a load operation.
//        : Memory is affected in the RMW operation according to successOrder.
//        : Memory is affected in the load operation according to failOrder.
//        : failOrder cannot be a stronger order than successOrder.
//        : Returns true or false if the comparison succeeded and T was stored into the atomic object.
//        :
//        : The weak variant may fail even if the observed value of the atomic object equals T&.
//        : This can yield performance gains on platforms with ld/str exclusive pair instructions especially
//        : when the compare_exchange operation is done in a loop.
//        : Only the bool return value can be used to determine if the operation was successful.
//
//   - bool compare_exchange_weak(T&, T, order)
//        : Same as the above except that order is used for both the RMW and the load operation.
//        : If order == acq_rel then the order of the load operation equals acquire.
//        : If order == release then the order of the load operation equals relaxed.
//
//   - bool compare_exchange_strong(T&, T, successOrder, failOrder)
//   - bool compare_exchange_strong(T&, T, order)
//        : This operation is the same as the above weak variants
//        : expect that it will not fail spuriously if the value stored equals T&.
//
//   The below operations are only valid for Integral types.
//
//   - T fetch_add(T, order)
//        : Atomically performs a RMW that increments the value stored with T.
//        : Returns the previous value stored before the RMW operation.
//   - T fetch_sub(T, order)
//        : Atomically performs a RMW that decrements the value stored with T.
//        : Returns the previous value stored before the RMW operation.
//   - T fetch_and(T, order)
//        : Atomically performs a RMW that bit-wise and's the value stored with T.
//        : Returns the previous value stored before the RMW operation.
//   - T fetch_or(T, order)
//        : Atomically performs a RMW that bit-wise or's the value stored with T.
//        : Returns the previous value stored before the RMW operation.
//   - T fetch_xor(T, order)
//        : Atomically performs a RMW that bit-wise xor's the value stored with T.
//        : Returns the previous value stored before the RMW operation.
//
//   - +T add_fetch(T, order)
//        : Atomically performs a RMW that increments the value stored with T.
//        : Returns the new updated value after the operation.
//   - +T sub_fetch(T, order)
//        : Atomically performs a RMW that decrements the value stored with T.
//        : Returns the new updated value after the operation.
//   - +T and_fetch(T, order)
//        : Atomically performs a RMW that bit-wise and's the value stored with T.
//        : Returns the new updated value after the operation.
//   - +T or_fetch(T, order)
//        : Atomically performs a RMW that bit-wise or's the value stored with T.
//        : Returns the new updated value after the operation.
//   - +T xor_fetch(T, order)
//        : Atomically performs a RMW that bit-wise xor's the value stored with T.
//        : Returns the new updated value after the operation.
//
//   - T operator++/--()
//        : Atomically increments or decrements the atomic value by one.
//        : Returns the previous value stored before the RMW operation.
//        : Memory is affected according to seq_cst ordering.
//
//   - T ++/--operator()
//        : Atomically increments or decrements the atomic value by one.
//        : Returns the new updated value after the RMW operation.
//        : Memory is affected according to seq_cst ordering.
//
//   - T operator+=/-=/&=/|=/^=(T)
//        : Atomically adds, subtracts, bitwise and/or/xor the atomic object with T.
//        : Returns the new updated value after the operation.
//        : Memory is affected according to seq_cst ordering.
//
//
//   The below operations are only valid for Pointer types
//
//   - T* fetch_add(ptrdiff_t val, order)
//        : Atomically performs a RMW that increments the value store with sizeof(T) * val
//        : Returns the previous value stored before the RMW operation.
//   - T* fetch_sub(ptrdiff_t val, order)
//        : Atomically performs a RMW that decrements the value store with sizeof(T) * val
//        : Returns the previous value stored before the RMW operation.
//
//   - +T* add_fetch(ptrdiff_t val, order)
//        : Atomically performs a RMW that increments the value store with sizeof(T) * val
//        : Returns the new updated value after the operation.
//   - +T* sub_fetch(ptrdiff_t val, order)
//        : Atomically performs a RMW that decrements the value store with sizeof(T) * val
//        : Returns the new updated value after the operation.
//
//   - T* operator++/--()
//        : Atomically increments or decrements the atomic value by sizeof(T) * 1.
//        : Returns the previous value stored before the RMW operation.
//        : Memory is affected according to seq_cst ordering.
//
//   - T* ++/--operator()
//        : Atomically increments or decrements the atomic value by sizeof(T) * 1.
//        : Returns the new updated value after the RMW operation.
//        : Memory is affected according to seq_cst ordering.
//
//
//   - +EASTL_ATOMIC_HAS_[len]BIT Macro Definitions
//        These macros provide the ability to compile-time switch on the availability of support for the specific
//        bit width of an atomic object.
//   Example:
//
//   #if defined(EASTL_ATOMIC_HAS_128BIT)
//   #endif
//
//   Indicates the support for 128-bit atomic operations on an eastl::atomic<T> object.
//


/////////////////////////////////////////////////////////////////////////////////
//
// eastl::atomic_flag class API
//
//   Unless otherwise specified all orders except read_depends is a valid order
//   on the given operation.
//
//   - atomic_flag()     : Initializes the flag to false.
//
//   - clear(order)
//       : Atomically stores the value false to the flag.
//       : Valid orders are relaxed, release, and seq_cst.
//
//   - bool test_and_set(order)
//       : Atomically exchanges flag with true and returns the previous value that was held.
//
//   - bool test(order)
//       : Atomically loads the flag value.
//       : Valid orders are relaxed, acquire, and seq_cst.
//


/////////////////////////////////////////////////////////////////////////////////
//
// eastl::atomic standalone free function API
//
//   All class methods have a standalone free function that takes a pointer to the
//   atomic object as the first argument. These functions just call the correct method
//   on the atomic object for the given operation.
//   These functions come in two variants, a non-explicit and an explicit variant
//   that take on the form atomic_op() and atomic_op_explicit() respectively.
//   The non-explicit variants take no order arguments and thus are all seq_cst.
//   The explicit variants take an order argument.
//   Only the standalone functions that do not have a class method equivalent pair will be
//   documented here which includes all new extensions to the std API.
//
//   - +compiler_barrier()
//        : Read-Write Compiler Barrier.
//   - +compiler_barrier_data_dependency(const T&)
//        : Read-Write Compiler Barrier.
//        : Applies a fake input dependency on const T& so the compiler believes said variable is used.
//        : Useful for example when writing benchmark or testing code with local variables that must not get dead-store eliminated.
//   - +cpu_pause()
//        : Prevents speculative memory order violations in spin-wait loops.
//        : Allows giving up core resources, execution units, to other threads while in spin-wait loops.
//   - atomic_thread_fence(order)
//        : Read docs below.
//   - atomic_signal_fence(order)
//        : Prevents reordering with a signal handler.
//   - +atomic_load_cond(const eastl::atomic<T>*, Predicate)
//        : continuously loads the atomic object until Predicate is true
//        : will properly ensure the spin-wait loop is optimal
//        : very useful when needing to spin-wait for some condition to be true which is common is many lock-free algorithms
//        : Memory is affected according to seq_cst ordering.
//   - +atomic_load_cond_explicit(const eastl::atomic<T>*, Predicate, Order)
//        : Same as above but takes an order for how memory is affected
//


/////////////////////////////////////////////////////////////////////////////////
//
//   Deviations from the standard. This does not include new features added:
//
// 1.
//   Description: Atomics are always lock free
//   Reasoning  : We don't want people to fall into performance traps where implicit locking
//                is done. If your user defined type is large enough to not support atomic
//                instructions then your user code should do the locking.
//
// 2.
//   Description: Atomic objects can not be volatile
//   Reasoning  : Volatile objects do not make sense in the context of eastl::atomic<T>.
//                Use the given memory orders to get the ordering you need.
//                Atomic objects have to become visible on the bus. See below for details.
//
// 3.
//   Description: Consume memory order is not supported
//   Reasoning  : See below for the reasoning.
//
// 4.
//   Description: ATOMIC_INIT() macros and the ATOMIC_LOCK_FREE macros are not implemented
//   Reasoning  : Use the is_lock_free() method instead of the macros.
//                ATOMIC_INIT() macros aren't needed since the default constructor value initializes.
//
// 5.
//   Description: compare_exchange failure memory order cannot be stronger than success memory order
//   Reasoning  : Besides the argument that it ideologically does not make sense that a failure
//                of the atomic operation shouldn't have a stricter ordering guarantee than the
//                success of it; if that is required then just make the whole operation stronger.
//                This ability was added and allowed in C++17 only which makes supporting multiple
//                C++ versions harder when using the compiler provided intrinsics since their behaviour
//                is reliant on the C++ version being compiled. Also makes it harder to reason about code
//                using these atomic ops since C++ versions vary the behaviour. We have also noticed
//                that versions of compilers that say they support C++17 do not properly adhere to this
//                new requirement in their intrinsics. Thus we will not support this.
//
// 6.
//   Description: All memory orders are distinct types instead of enum values
//   Reasoning  : This will not affect how the API is used in user code.
//                It allows us to statically assert on invalid memory orders since they are compile-time types
//                instead of potentially runtime enum values.
//                Allows for more efficient code gen without the use of switch statements or if-else conditionals
//                on the memory order enum values on compilers that do not provide intrinsics that take in a
//                memory order, such as MSVC, especially in debug and debug-opt builds.
//


/////////////////////////////////////////////////////////////////////////////////
//
//   ******** DISCLAIMER ********
//
//   This documentation is not meant to provide rigorous proofs on the memory models
//   of specific architectures or the C++ memory model introduced in C++11. It is not
//   meant to provide formal mathematical definitions and logic that shows that a given
//   implementation adheres to the C++ memory model. This isn't meant to be some infallible
//   oracle on memory models, barriers, observers, and architecture implementation details.
//   What I do hope a reader gets out of this is the following. An understanding of the C++
//   memory model and how that relates to implementations on various architectures. Various
//   phenomena and ways that compilers and architectures can steer away from a sequentially
//   consistent system. To provide examples on how to use this library with common patterns
//   that will be seen in many code bases. Lastly I would like to provide insight and
//   further readings into the lesser known topics that aren't shared outside people
//   who live in this space and why certain things are done the way they are
//   such as cumulativity of memory barriers as one example. Sometimes specifying barriers
//   as LDLD/LDST/STST/STLD doesn't actually cut it, and finer grain semantics are needed
//   to describe cumulativity of memory barriers.
//
//   ******** Layout of the Documentation ********
//
//   This document will first go through a variety of different hardware architectures with examples of the various kinds of
//   reordering that is allowed by these architectures. We will use the memory barriers provided by the hardware to "fix" these
//   examples.
//   Then we will introduce the C++ memory model and revisit the examples using the platform agnostic abstract memory model to "fix"
//   them.
//   The hope here is that we get a sense of the various types of architectures and weak memory consistency provided by them and thus
//   an appreciation for the design of the C++ abstract memory model.
//
//   ******** REFERENCES ********
//   [1] Dekker's mutual exclusion algorithm made RW-safe
//   [2] Handling Memory Ordering in Multithreaded Applications with Oracle Solaris
//   [3] Evaluating the Cost of Atomic Operations on Modern Architectures
//   [4] A Tutorial Introduction to the ARM and POWER Relaxed Memory Models
//   [5] Memory Barriers: a Hardware View for Software Hackers
//   [6] Memory Model = Instruction Reordering + Store Atomicity
//   [7] ArMOR: Defending Against Memory Consistency Model Mismatches in Heterogeneous Architectures
//   [8] Weak Memory Models: Balancing Definitional Simplicity and Implementation Flexibility
//   [9] Repairing Sequential Consistency in C/C++11
//   [10] A high-level operational semantics for hardware weak memory models
//   [11] x86-TSO: A Rigorous and Usable Programmer's Model for x86 Multiprocessors
//   [12] Simplifying ARM Concurrency: Multicopy-Atomic Axiomatic and Operational Models for ARMv8
//   [13] Mixed-size Concurrency: ARM, POWER, C/C++11, and SC
//   [14] P0668R4: Revising the C++ memory model
//   [15] Constructing a Weak Memory Model
//   [16] The Superfluous Load Queue
//   [17] P0190R1: Proposal for New memory_order_consume Definition
//
//   ******** What does it mean to be Atomic? ********
//
//   The word atomic has been overloaded and can mean a lot of different things depending on the context,
//   so let's digest it.
//
//   The first attribute for something to be atomic is that concurrent stores and loads
//   must not tear or shear. This means if two threads write 0x01 and 0x02 at the same time
//   then the only values that should ever be observed is 0x01 or 0x02. We can only see
//   the whole write of 0x01 or 0x02, not 0x03 as an example. Many algorithms rely on
//   this property; only very few such a Dekker's algorithm for mutual exclusion don't.
//   Well actually a recent paper, [1], showed that Dekker's isn't safe without atomic
//   loads and stores so this property is pretty fundamental and also hard to prove that
//   your algorithm is safe without this property on loads and stores.
//
//   We need to ensure the compiler emits a single load instruction.
//   If we are doing 64-bit loads on a 32-bit platform, we need to ensure the load is one
//   instruction instead of 2 32-bit loads into two registers.
//   Another example is if we have this struct, struct { int32_t i; int32_t k; }, even on
//   a 64-bit system we have to ensure the compiler does one 64-bit load and not two
//   32-bit loads for each individual member.
//
//   We also need to ensure the correct instruction is emitted. A general load instruction
//   to do a 64-bit load on a 32-bit platform may perform a 64-bit load but it may not
//   be atomic, it may be turned into two 32-bit loads behind the scenes in the cpu.
//   For example on ARMv7 we would have to use ldrexd not ldrd for 64-bit loads
//   on a 32-bit ARMv7 core.
//
//   An operation may be considered atomic if multiple sub-operations are done as one
//   transactional unit. This is commonly known as a Read-Modify-Write, RMW, operation.
//   Take a simple add operation; it is actually a load from memory into a register,
//   a modification of said register and then a store back to memory. If two threads
//   concurrently execute this add operation on the same memory location; any interleaving
//   of the 3 sub-operations is possible. It is possible that if the initial value is 0,
//   the result may be 1 because each thread executed in lockstep both loading 0, adding 1
//   and then storing 1. A RMW operation may be considered atomic if the whole sequence of
//   sub-operations are serialized as one transactional unit.
//
//   Atomicity may also refer to the order in which memory operations are observed and the
//   dependencies between memory operations to different memory locations. As a quick example
//   into the very thing we will be deep diving into that is not very intuitive. If I do, [STORE(A, 2); STORE(B, 1);],
//   in one thread and another thread does, [r0 = LOAD(B); r1 = LOAD(A);]; if r0 == 1, thus we observed
//   the store to B, will we observe r1 == 2. Our intuition tells us that well A was stored
//   first and then B, so if I read the new value of B then I must also read the new value
//   of A since the store to A happened before B so if I can see B then I must be able to
//   see everything before B which includes A.
//   This highlights the ordering of memory operations and why memory barriers and memory
//   models are so heavily attached to atomic operations because one could classify something
//   is atomic if the dependency highlighted in the above example is allowed to be maintained.
//
//   This is what people mean when you hear that volatile does NOT mean atomicity of the operation.
//   Usually people imply a lot of implicit assumptions when they mark a variable as volatile.
//   All volatile gives us is the ability to tell the compiler it may not assume anything
//   about the state of that memory location. This means the compiler must always emit a load
//   or store instruction, cannot perform constant folding, dead-store elimination, or
//   do any sort of code movement on volatile variables.
//
//   ******** Preliminary Basics ********
//
//   It is expected that the reader understands what a cache is, how it is organized and how data
//   is chunked into cachelines. It is helpful if the reader understands basic cache coherency
//   protocols such as MSI or MESI.
//   It is expected the reader understands alignment, especially natural alignment
//   of the processor and why alignment is important for data access.
//   The reader should have some understanding of how a processor executes instructions,
//   basics of what Out-of-Order execution means and basics of what speculative execution means.
//   It is expected that the reader has an understanding of threading, multi-threaded programming
//   and the use of concurrency primitives such as mutexes.
//   Memory Barrier, Barrier, Memory Fence and Fence are all interchangeable synonyms.
//
//   Independent memory operations can be performed or observed, depending on your perspective,
//   in any order as long as the local cpu thinks its execution is happening in program order.
//   This can be a problem for inter-cpu communications and thus we need some way to enforce
//   that the compiler does not reorder instructions and that the cpu also does not reorder
//   instructions. This is what a barrier is, it is an enforcement of ordering on memory instructions,
//   so as the name suggests a barrier. Barriers can be one-sided or both-sided which means
//   the barrier enforces a partial order above or below or on both sides of said barrier.
//
//   Processors will use tricks such as out-of-order execution, memory instruction buffering and
//   combining, speculative loads and speculative execution, branch prediction and many types of caching even
//   in various interconnects from the cpu to the memory itself. One key thing to note is that cpus
//   do not physically reorder the instruction stream. Instructions are dispatched and retired
//   in-order but executed out-of-order. Memory barriers will prevent these tricks from happening
//   by controlling the interaction of multiple cpus.
//
//   Compilers will morph your code and physically move instructions around as long as the program
//   has the same observed behaviour. This is becoming increasingly true with more optimization techniques
//   such as Link Time Optimization becoming the norm where once people assumed compilers couldn't assume
//   something outside the given TU and now because they have the whole program view they know everything.
//   This means the compiler does indeed alter the instruction stream
//   and compiler barriers are a way to tell them to not move any memory instructions across the barrier.
//   This does not prevent a compiler from doing optimizations such as constant folding, merging of
//   overlapping loads, or even dead store elimination. Compiler barriers are also very cheap and
//   have zero impact on anything that the compiler knows isn't visible in memory such as local variables
//   whose addresses do not escape the function even if their address is taken. You can think of it
//   in terms of a sequence point as used with "volatile" qualified variables to denote a place in code where
//   things must be stable and the compiler doesn't cache any variables in registers or do any reordering.
//
//   Memory Barriers come in many flavours that instill a partial or full ordering on memory operations.
//   Some memory operations themselves have implicit ordering guarantees already, for example
//   Total-Store Order, TSO, architectures like x86 guarantee that a store operation cannot be reordered with a
//   previous store operation thus a memory barrier that only orders stores is not needed
//   on this architecture other than ensuring the compiler doesn't do any shenanigans.
//   Considering we have 4 permutations of memory operations; a common way to describe an ordering
//   is via Load-Load/LDLD, Load-Store/LDST, Store-Store/STST or Store-Load/STLD notation. You read this
//   notation as follows; STLD memory barrier means a load cannot be reordered with a previous store.
//   For example, on TSO architecture we can say all stores provide a STST memory barrier,
//   since a store cannot be reordered with a previous store.
//
//   Memory Barriers in itself are not a magic bullet, they come with caveats that must be known.
//   Each cpu architecture also has its own flavours and guarantees provided by said memory barriers.
//   There is no guarantee that memory instructions specified before a memory barrier will complete,
//   be written to memory or fully propagated throughout the rest of the system, when the memory barrier
//   instruction completes. The memory barrier creates a point in that local cpus queue of memory instructions
//   whereby they must not cross. There is no guarantee that using a memory barrier on one cpu will have
//   any effect at all on another remote cpu's observed view of memory. This also implies that executing
//   a memory barrier does not hinder, incur, stall or enforce any other cpus to serialize with each other cpu.
//   In order for a remote cpu to observe the correct effects it must also use a matching memory barrier.
//   This means code communicating in 2 threads through memory must both be employing the use of memory barriers.
//   For example, a store memory barrier that only orders stores, STST, in one thread must be paired with a load memory barrier
//   that only orders loads, LDLD, in the other thread trying to observe those stores in the correct order.
//
//   ******** Memory Types && Devices ********
//
//   eastl::atomic<T> and accompanying memory barriers ONLY ORDER MEMORY to cpu-to-cpu communication through whatever the
//   processor designates as normal cacheable memory. It does not order memory to devices. It does not provide any DMA ordering guarantees.
//   It does not order memory with other memory types such as Write Combining. It strictly orders memory only to shared memory that is used
//   to communicate between cpus only.
//
//   ******** Sequentially Consistent Machine ********
//
//   The most intuitive as well as the model people naturally expect a concurrent system to have is Sequential Consistency.
//   You may have or definitely have heard this term if you dealt with any type of distributed system. Lamport's definition
//   articulates this consistency model the best.
//   Leslie Lamport: "the result of any execution is the same as if the operations of all the processors were executed in some
//                    sequential order, and the operations of each individual processor appear in this sequence in the order
//                    specified by its program".
//
//   A Sequentially Consistent machine is modelled as follows:
//
//   ------------               ------------
//   | Thread 0 |      ...      | Thread N |
//   ------------               ------------
//       |  |                        |  |
//       |  |                        |  |
//   ----------------------------------------
//   |                                      |
//   |           Shared Memory              |
//   |                                      |
//   ----------------------------------------
//
//   This is a sequentially consistent machine. Each thread is executing instructions in program order which does loads and stores
//   that are serialized in some order to the shared memory. This means all communication is done through the shared memory with one cpu
//   doing one access at a time. This system has a couple key properties.
//
//   1. There is no local cpu memory reordering. Each cpu executes instructions in program order and all loads and stores must complete,
//      be visible in the shared memory or be visible in a register before starting the next instruction.
//   2. Each memory operation becomes visible to all cpus at the same time. If a store hits the shared memory, then all subsequent loads
//      from every other cpu will always see the latest store.
//
//   A Sequentially Consistent machine has, Single-Copy Store Atomicity: All stores must become visible to all cores in the system at the same time.
//
//   ******** Adding Caches ********
//
//   Caches by nature implicitly add the potential for memory reordering. A centralized shared snoopy bus that we all learned in school
//   makes it easy to implement sequential consistency with caches. Writes and reads are all serialized in a total order via the cache bus transaction
//   ordering. Every modern day bus is not inorder, and most certainly not a shared centralized bus. Cache coherency guarantees that all memory operations
//   will be propagated eventually to all parties, but it doesn't guarantee in what order or in what time frame. Once you add
//   caches, various levels of caching and various interconnects between remote cpus, you inevitably run into the issue where
//   some cpus observe the effects of a store before other cpus. Obviously we have weakly-ordered and strongly-ordered cpus with
//   caches so why is that? The short answer is, where is the onus put, is it on the programmer or the hardware. Does the hardware
//   have dependency tracking, is it able to determine when a memory order violation occurs such as rolling back its speculative execution
//   and also how far along the chain of interconnects does the hardware wait before it determines that the memory operation has
//   been acknowledged or is considered to satisfy its memory ordering guarantees. Again this is a very high level view of the system
//   as a whole, but the takeaway is yes; caches do add the potential for reordering but other supporting hardware determines whether
//   that is observable by the programmer. There is also some debate whether weakly-ordered processors are actually more performant
//   than strongly-ordered cpus eluding to the fact that the hardware has a better picture of what is a violation versus the programmer
//   having to emit far more barriers on weakly-ordered architectures in multi-threaded code which may actually not be needed because the
//   hardware didn't commit a violation but it may have and we as the programmer cannot rely on may haves.
//
//   ******** Store Buffers ********
//
//   Obviously having all stores serialize results in unnecessary stalls. Store buffers alleviate this issue.
//   Store buffers are simple fixed size structures that sit between the cpu and the memory hierarchy. This allows
//   each cpu to record its write in the store buffer and then move onto the next instruction. The store buffer will
//   eventually be flushed to the resulting memory hierarchy in FIFO order. How and when this flushing occurs is irrelevant to the
//   understanding of a store buffer. A read from an address will grab the most recent write to the same address in the store buffer.
//
//   The introduction of a store buffer is our first dive into weaker memory consistency. The addition of this hardware turns the consistency model weaker,
//   into one that is commonly known as TSO, Total-Store Order. This is the exact model used by x86 cpus and we will see what this means
//   and what new effects are observed with the addition of the store buffer. Below is a diagram of how the machine may now look.
//   This type of store buffer is known as a FIFO store buffer, FIFO write buffer, or Load/Store Queue in some literature. This type of
//   store buffer introduces STLD reordering but still prevents STST reordering. We will take a look at another type of store buffer later.
//   Even with this store buffer, stores to the same address can still be merged so that only the latest store is written to the cache assuming
//   no other intermediary stores happen. x86 cpus do write merging even for consecutive stores, i.e. storing to A and A+1 can be merged into one two-byte store.
//
//   ------------               ------------
//   | Thread 0 |      ...      | Thread N |
//   ------------               ------------
//       |  |                        |  |
//       |  |                        |  |
//    | Store  |                  | Store  |
//    | Buffer |                  | Buffer |
//       |  |                        |  |
//   ----------------------------------------
//   |                                      |
//   |           Shared Memory              |
//   |                                      |
//   ----------------------------------------
//
//   ---- Store-Buffering / Dekker's Example ----
//   This is a very common litmus test that showcases the introduction of STLD reordering. It is called Store-Buffering example because it is the only weaker
//   behaviour observed under TSO and also called Dekker's Example as it famously breaks Dekker's mutual exclusion algorithm.
//
//   ---------------------------
//   Initial State:
//   x = 0; y = 0;
//   ---------------------------
//   Thread 0     |    Thread 1
//   ---------------------------
//   STORE(x, 1)  | STORE(y, 1)
//   r0 = LOAD(y) | r1 = LOAD(x)
//   ---------------------------
//   Observed: r0 = 0 && r1 = 0
//   ---------------------------
//
//   We would normally assume that any interleaving of the two threads cannot possibly end up with both loads reading 0. We assume that the observed outcome
//   of r0 = 0 && r1 = 0 to be impossible, clearly that is not the case. Let's start by understanding the example with no reordering possible. Both threads
//   run and their first instruction is to write the value 1 into either x or y, the next instruction then loads from the opposite variable. This means no
//   matter the interleaving, one of the loads always executes after the other thread's store to that variable.
//   We could observe r0 = 1 && r1 = 1 if both threads execute in lockstep.
//   We could observe r0 = 0 && r1 = 1 if thread 0 executes and then thread 1 executes.
//   We could observe r0 = 1 && r1 = 0 if thread 1 executes and then thread 0 executes.
//   Since the stores always execute before that load in the other thread, one thread must always at least observe a store, so let's see why store buffers break this.
//
//   What will happen is that STORE(x, 1) is stored to the store buffer but not made globally visible yet.
//   STORE(y, 1) is written to the store buffer and also is not made globally visible yet.
//   Both loads now read the initial state of x and y which is 0. We got the r0 = 0 && r1 = 0 outcome and just observed a Store-Load reordering.
//   It has appeared as if the loads have been reordered with the previous stores and thus executed before the stores.
//   Notice even if we execute the instructions in order, a series of other hardware side effects made it appear as if the instructions have been reordered.
//   We can solve this by placing a Store-Load barrier after the store and before the load as follows.
//
//   ---------------------------
//   Thread 0     |    Thread 1
//   ---------------------------
//   STORE(x, 1)  | STORE(y, 1)
//   STLD BARRIER | STLD BARRIER
//   r0 = LOAD(y) | r1 = LOAD(x)
//   ---------------------------
//
//   This STLD barrier effectively will flush the store buffer into the memory hierarchy ensuring all stores in the buffer are visible to all other cpus at the same time
//   before executing the load instruction. Again nothing prevents a potential hardware from speculatively executing the load even with the STLD barrier, the hardware will have to do
//   a proper rollback if it detected a memory order violation otherwise it can continue on with its speculative load. The barrier just delimits a stability point.
//
//   Most hardware does not provide granular barrier semantics such as STLD. Most provide a write memory barrier which only orders stores, STST, a read memory barrier
//   which only orders loads, LDLD, and then a full memory barrier which is all 4 permutations. So on x86 we will have to use the mfence, memory fence, instruction
//   which is a full memory barrier to get our desired STLD requirements.
//
//   TSO also has the property that we call, Multi-Copy Store Atomicity. This means a cpu sees its own stores before they become visible to other cpus,
//   by forwarding them from the store buffer, but a store becomes visible to all other cpus at the same time when flushed from the store buffer.
//
//
//   Let's look at a non-FIFO store buffer now as seen in ARM cpus as an example and we will use a standard Message Passing example to see how it manifests in even weaker consistency.
//   A store buffer on ARM as an example allows write merging even with adjacent stores, is not a FIFO queue, any stores in the small hardware hash table may be ejected at any point
//   due to a collision eviction or the availability of cachelines in the cache hierarchy meaning that stores may bypass the buffer entirely if that cacheline is already owned by that cpu.
//   There is no guarantee that stores will be completed in order as in the FIFO case.
//
//   ---------------------------
//   Initial State:
//   x = 0; y = 0;
//   ---------------------------
//   Thread 0     |    Thread 1
//   ---------------------------
//   STORE(x, 1)  | while(LOAD(y) == 0);
//   STORE(y, 1)  | r0 = LOAD(x)
//   ---------------------------
//   Observed: r0 = 0
//   ---------------------------
//
//   This is a classic Message Passing example that is very commonly used in production code. We store some values and then set a flag, STORE(y, 1) in this case.
//   The other thread waits until the flag is observed and then reads the value out of x. If we observed the flag then we should obviously see all stores before the flag was set.
//   Given our familiarity with TSO consistency above we know this definitely works on TSO and it is impossible to observe the load of x returning 0 under that consistency model.
//   Let's see how this breaks with a non-FIFO store buffer.
//
//   Thread 0 executes the STORE(x, 1) but the cacheline for x is not in thread 0's cache so we write to the store buffer and wait for the cacheline.
//   Thread 1 executes the LOAD(y) and it also does not have y in its cacheline so it waits before completing the load.
//   Thread 0 moves on to STORE(y, 1). It owns this cacheline, hypothetically, so it may bypass the store buffer and store directly to the cache.
//   Thread 0 receives a message that Thread 1 needs y's cacheline, so it transfers the now modified cacheline to Thread 1.
//   Thread 1 completes the load with the updated value of y = 1 and branches out of the while loop since we saw the new value of y.
//   Thread 1 executes LOAD(x) which will return 0 since Thread 0 still hasn't flushed its store buffer waiting for x's cacheline.
//   Thread 0 receives x's cacheline and now flushes x = 1 to the cache. Thread 1 will also have invalidated its cacheline for x that it brought in via the previous load.
//
//   We have now fallen victim to STST reordering, allowing Thread 1 to observe a load of x returning 0. Not only does this store buffer allow STLD reordering due to the nature of
//   buffering stores, but it also allows another reordering; that of Store-Store reordering. It was observed as if Thread 0 executed STORE(y, 1) before STORE(x, 1) which completely
//   broke our simple message passing scenario.
//
//   ---------------------------
//   Thread 0     |    Thread 1
//   ---------------------------
//   STORE(x, 1)  | while(LOAD(y) == 0);
//   STST BARRIER |
//   STORE(y, 1)  | r0 = LOAD(x)
//   ---------------------------
//
//   The STST memory barrier effectively ensures that the cpu will flush its store buffer before executing any subsequent stores. That is not entirely true, the cpu is still allowed
//   to continue and execute stores to the store buffer as long as it doesn't flush them to the cache before the previous stores are flushed to the cache. If nothing becomes
//   globally visible out of order then we are good.
//   The example above will change how the processor executes due to the STST memory barrier. Thread 0 will execute STORE(y, 1), write to the store buffer and mark all current entries. Even though it owns the cacheline
//   it cannot write the store to the cache until all marked entries, which are all the previous stores, are flushed to the cache. We have now fixed the message passing code by adding
//   a STST or write memory barrier and thus it is no longer possible to observe the load of x returning 0.
//
//   ******** Invalidation Queues ********
//
//   Due to the cache coherency protocol in play, a write to a cacheline will have to send invalidation messages to all other cpus that may have that cacheline as well.
//   Immediately executing and responding to invalidation messages can cause quite a stall especially if the cache is busy at the moment with other requests.
//   The longer we wait to invalidate the cacheline, the longer the remote cpu doing the write is stalled waiting on us. We don't like this very much.
//   Invalidation Queues are just that, we queue up the action of actually invalidating the cacheline but immediately respond to the request saying we did it anyway.
//   Now the remote cpu thinks we invalidated said cacheline but actually it may very well still be in our cache ready to be read from. We just got weaker again, let's
//   see how this manifests in code by starting from the end of our previous example.
//
//   ---------------------------
//   Initial State:
//   x = 0; y = 0;
//   ---------------------------
//   Thread 0     |    Thread 1
//   ---------------------------
//   STORE(x, 1)  | while(LOAD(y) == 0);
//   STST BARRIER |
//   STORE(y, 1)  | r0 = LOAD(x)
//   ---------------------------
//   Observed: r0 = 0
//   ---------------------------
//
//   Thread 1 receives the invalidate x's cacheline message and queues it because it is busy.
//   Thread 1 receives the invalidate y's cacheline message, but we don't have that cacheline so acknowledge immediately.
//   Thread 1 executes LOAD(y), loads in y's cacheline and branches out of the loop.
//   Thread 1 executes LOAD(x), and loads from the cache the old value of x because the invalidation message is still sitting in the invalidation queue.
//
//   We have just again observed the load of x returning 0 but from a different type of reordering now on the reader side.
//   This is a form of LDLD, Load-Load, reordering as it appears as if LOAD(x) was executed before LOAD(y). This can be fixed as follows.
//
//   ---------------------------
//   Thread 0     |    Thread 1
//   ---------------------------
//   STORE(x, 1)  | while(LOAD(y) == 0);
//   STST BARRIER | LDLD BARRIER
//   STORE(y, 1)  | r0 = LOAD(x)
//   ---------------------------
//
//   The LDLD memory barrier essentially marks all entries currently in the invalidation queue. Any subsequent load must wait until all the marked entries have been
//   processed. This ensures once we observe y = 1, we process all entries that came before y and that way we observe all the stores that happened before y.
//   The insertion of the read memory barrier creates the required memory barrier pairing as discussed above and ensures that now our code executes as expected.
//
//   It must be made clear that these are not the only hardware structure additions or ways that can relax STST, STLD and LDLD orderings. These are merely
//   2 structures that are common and ones that I choose to use as examples of how hardware can reduce ordering guarantees. Knowing how the hardware does this
//   isn't always entirely clear but having a model that tells us what operations can be reordered is all we need to be able to reason about our code when executing on that hardware.
//
//   ******** Load Buffering ********
//
//   The analog of the Store Buffering example, this litmus test has two threads read from two different locations and then write to the other locations.
//   The outcome of having LDST reordering is allowed and observable on many processors such as ARM.
//
//   ---------------------------
//   Initial State:
//   x = 0; y = 0;
//   ---------------------------
//   Thread 0     |    Thread 1
//   ---------------------------
//   r0 = LOAD(x) | r1 = LOAD(y)
//   STORE(y, 1)  | STORE(x, 1)
//   ---------------------------
//   Observed: r0 = 1 && r1 = 1
//   ---------------------------
//
//   This is possible because the processor does not have to wait for the other cpu's cacheline to arrive before storing into the cache.
//   Assume Thread 0 owns y's cacheline and Thread 1 owns x's cacheline.
//   The processor may execute the load and thus buffer the load waiting for the cacheline to arrive.
//   The processor may continue onto the store and since each cpu owns their respective cacheline, store the result into the cache.
//   The cpus now receive the cachelines for x and y with the now modified value.
//   We have just observed the loads returning 1 and thus observed LDST reordering.
//
//   To forbid such outcome it suffices to add any full memory barrier to both threads or a local Read-After-Write/Read-To-Write dependency or a control dependency.
//
//   -------------------------------
//   Thread 0       |    Thread 1
//   -------------------------------
//   r0 = LOAD(x)   | r1 = LOAD(y)
//   if (r0 == 1)   | if (r1 == 1)
//     STORE(y, 1)  |   STORE(x, 1)
//   -------------------------------
//
//   -----------------------------------------------------
//   Thread 0                 |    Thread 1
//   -----------------------------------------------------
//   r0 = LOAD(x)             | r1 = LOAD(y)
//   STORE(&(y + r0 - r1), 1) | STORE(&(x + r1 - r1), 1)
//   -----------------------------------------------------
//
//   Both fixes above ensure that both writes cannot be committed, made globally visible, until their program source code order preceding reads have been fully satisfied.
//
//   ******** Compiler Barriers ********
//
//   Compiler barriers are both-sided barriers that prevent loads and stores from moving down past the compiler barrier and
//   loads and stores from moving up above the compiler barrier. Here we will see the various ways our code may be subject
//   to compiler optimizations and why compiler barriers are needed. Note as stated above, compiler barriers may not
//   prevent all compiler optimizations or transformations. Compiler barriers are usually implemented by reloading all
//   variables that are currently cached in registers and flushing all stores in registers back to memory.
//   This list isn't exhaustive but will hopefully try to outline what compiler barriers protect against and what they don't.
//
//   Compiler may reorder loads.
//   LOAD A; LOAD B; -> LOAD B; LOAD A;
//   LOAD A; operation on A; LOAD B; operation on B; -> LOAD A; LOAD B; operation on A; operation on B
//
//   Insert a compiler barrier in between the two loads to guarantee that they are kept in order.
//   LOAD A; COMPILER_BARRIER; LOAD B;
//   LOAD A; operation on A; COMPILER_BARRIER; LOAD B; operation on B;
//
//   The same with stores.
//   STORE(A, 1); STORE(B, 1); -> STORE(B, 1); STORE(A, 1);
//   operations and STORE result into A; operations and STORE result int B; -> all operations; STORE result into B; STORE result into A;
//
//   Insert a compiler barrier in between the two stores to guarantee that they are kept in order.
//   It is not required that the multiple stores to A before the barrier are not merged into one final store.
//   It is not required that the store to B after the barrier be written to memory, it may be cached in a register for some indeterminate
//   amount of time as an example.
//   STORE(A, 1); COMPILER_BARRIER; STORE(B, 1);
//
//   The compiler is allowed to merge overlapping loads and stores.
//   Inserting a compiler barrier here will not prevent the compiler from doing this optimization as doing one wider load/store is
//   technically still abiding by the guarantee that the loads/stores are not reordered with each other.
//   LOAD A[0]; LOAD A[1]; -> A single wider LOAD instruction
//   STORE(A[0], 1); STORE(A[1], 2); -> A single wider STORE instruction
//
//   Compilers do not have to reload the values pointers point to. This is especially common with RISC architectures with lots
//   of general purpose registers or even compiler optimizations such as inlining or Link-Time Optimization.
//   int i = *ptr; Do bunch of operations; if (*ptr) { do more; }
//   It is entirely possible the compiler may remove the last if statement because it can keep the *ptr in a register
//   and it may infer from the operations done on i that i is never 0.
//
//   int i = *ptr; Do bunch of operations; COMPILER_BARRIER; if (*ptr) { do more; }
//   Inserting a compiler barrier at that location will cause the compiler to have reload *ptr thus keeping the if statement assuming
//   no other optimizations take place, such as the compiler knowing that *ptr is always greater than 0.
//
//   The compiler is within its rights to also merge and reload loads as much as it pleases.
//
//   while (int tmp = LOAD(A))
//        process_tmp(tmp)
//
//   Will be merged and transformed to
//
//   if (int tmp = LOAD(A))
//        for (;;) process_tmp(tmp)
//
//   Inserting a compiler barrier will ensure that LOAD(A) is always reloaded and thus the unwanted transformation is avoided.
//
//   while (int tmp = LOAD(A))
//   {
//       process_tmp(tmp)
//       COMPILER_BARRIER
//   }
//
//   Under heavy register pressure scenarios, say the loop body was larger, the compiler may reload A as follows.
//   Compiler barriers cannot prevent this from happening, even if we put it after process_tmp as above;
//   the compiler still kept those loads above the barrier so it satisfied its contract even though it reloaded
//   from A more than once.
//
//   while (int tmp = LOAD(A))
//       process_tmp(LOAD(A))
//
//   In the above transformation it is possible that another cpu stores 0 into A. When we reload A for process_tmp, we pass 0
//   to process_tmp() which it would actually never expect to observe. Because if we observed 0, the while loop condition
//   would never be satisfied. If the compiler under register pressure instead stored and loaded tmp from its stack slot, that is fine
//   because we are just storing and loading the original observed value from A. Obviously that is slower than just reloading from
//   A again so an optimizing compiler may not do the stack slot store. This is an unwanted transformation which eastl::atomic<T> prevents
//   even on relaxed loads.
//
//   The compiler is allowed to do dead-store elimination if it knows that value has already been stored, or that only the last store
//   needs to be stored. The compiler does not assume or know that these variables are shared variables.
//
//   STORE(A, 1);         STORE(A, 1);
//   OPERATIONS;     ->   OPERATIONS;
//   STORE(A, 1);
//
//   The compiler is well within its rights to omit the second store to A. Assuming we are doing some fancy lockfree communication
//   with another cpu and the last store is meant to ensure the ending value is 1 even if another cpu changed A in between; that
//   assumption will not be satisfied. A compiler barrier will not prevent the last store from being dead-store removed.
//
//   STORE(A, 1);
//   OPERATIONS;
//   STORE(A, 2);
//
//   Assuming these stores are meant to denote some state changes to communicate with a remote cpu. The compiler is allowed to
//   transform this as follows without a compiler barrier. Insert a compiler barrier between the two stores to prevent the transformation.
//   Something like this will also require memory barriers, but that is not the point of this section.
//
//   STORE(A, 2);
//   OPERATIONS;
//
//   The compiler is also allowed to invent stores as it may please.
//   First on many RISC architectures storing an immediate value either involves loading the immediate from the .data section
//   or combing a variety of load upper immediate and add or or immediate instructions to get our constant in a register and then
//   doing a single 32-bit store instruction from said register. Some ISAs have 16-bit stores with immediate value so that a store
//   may be broken into 2 16-bit store immediate values causing shearing. To reduce instruction dependencies it may also decide
//   to do two add immediates and then two 16-bit stores again causing shearing.
//
//   lui $t0, 1       # t0 == 0x00010000
//   ori $a0, $t0, 8  # t0 == 0x00010008
//   strw $t0, 0($a1) # store t0 into address at a1
//   ->
//   ori $a0, $t0, 1   # t0 == 0x00000001
//   ori $a0, $t1, 8   # t0 == 0x00000008
//   strhw $t0, 0($a1) # store t0 lower half at a1
//   strhw $t1, 2($a1) # store t1 upper half at a1
//
//   The above shows a potential transformation that a compiler barrier cannot solve for us.
//
//   A compiler may also introduce stores to save on branching. Let's see.
//
//   if (a)
//     STORE(X, 10);
//   else
//     STORE(X, 20);
//
//   STORE(X, 20);
//   if (a)
//     STORE(X, 10);
//
//   This is a very common optimization as it saves a potentially more expensive branch instruction but breaks multi-threaded code.
//   This is also another case where a compiler barrier doesn't give us the granularity we need.
//   The branches may even be completely removed with the compiler instead choosing to use conditional move operations which would
//   actually be compliant since there would be one store only done, an extra store wouldn't have been added.
//
//   You are now probably thinking that compiler barriers are useful and are definitely needed to tell the compiler to calm down
//   and guarantee our hardware guarantees are valid because the code we wrote is the instructions that were emitted.
//   But there are definitely lots of caveats where compiler barriers do not at all provide the guarantees we still need.
//   This where eastl::atomic<T> comes into play, and under the relaxed memory ordering section it will be explained
//   what the standard guarantees and how we achieve those guarantees, like ensuring the compiler never does dead-store elimination or reloads.
//
//   ******** Control Dependencies ********
//
//   Control dependencies are implicit local cpu ordering of memory instructions due to branching instructions, specifically
//   only conditional branches. The problem is compilers do not understand control dependencies, and control dependencies
//   are incredibly hard to understand. This is meant to make the reader aware they exist and to never use them
//   because they shouldn't be needed at all with eastl::atomic<T>. Also control dependencies are categorized as LDLD or LDST,
//   store control dependencies inherently do not make sense since the conditional branch loads and compares two values.
//
//   A LDLD control dependency is an anti-pattern since it is not guaranteed that any architecture will detect the memory-order violation.
//   r0 = LOAD(A);
//   if (r0)
//       r1 = LOAD(B)
//
//   Given those sequence of instructions, it is entirely possible that a cpu attempts to speculatively predict and load the value of B
//   before the branch instruction has finished executing. It is entirely allowed that the cpu loads from B, assume B is in cache and A
//   is not in cache, before A. It is allowed, that even if the cpu was correct in it's prediction that it doesn't reload B and change the
//   fact that it speculatively got lucky.
//
//   This is also what the x86 pause instruction inserted into spin wait loops is meant to solve.
//   LOOP:
//       r0 = LOAD(A);
//       if (!r0) pause; goto LOOP;
//
//   In the above spin loop, after a couple of iterations the processor will fill the pipeline with speculated cmp and load instructions.
//   x86 will catch a memory order violation if it sees that an external store was done to A and thus must flush the entire
//   pipeline of all the speculated load A. Pause instruction tells the cpu to not do speculative loads so that the pipeline is not
//   filled with all said speculative load instructions. This ensures we do not incur the costly pipeline flushes from memory order
//   violations which are likely to occur in tight spin wait loops. This also allows other threads on the same physical core to use the
//   core's resources better since our speculative nature won't be hogging it all.
//
//   A LDST control dependency is a true dependency in which the cpu cannot make a store visible to the system and other cpus until it
//   knows its prediction is correct. Thus a LDST ordering is guaranteed and can be always relied upon as in the following example.
//
//   r0 = LOAD(A);
//   if (r0)
//       STORE(B, 1);
//
//   The fun part comes in with how does the compiler actually break all of this.
//   First is that if the compiler can ensure that the value of A in the LDST example is always not zero, then it is always within its
//   rights to completely remove the if statement which would lend us with no control dependency.
//
//   Things get more fun when we deal with conditionals with else and else if statements where the compiler might be able to employ
//   invariant code motion optimizations. Take this example.
//
//   r0 = LOAD(A);
//   r1 = LOAD(B);
//   if (r0)
//       STORE(B, 1);
//       /* MORE CODE */
//   else if (r1)
//       STORE(B, 1);
//       /* MORE CODE */
//   else
//       STORE(B, 1);
//       /* MORE CODE */
//
//   If we were trying to be smart and entirely rely on the control dependency to ensure order, ya well just don't the compiler
//   is always smarter. The compiler is well within its rights to move all the STORE(B, 1) up and above all the conditionals breaking
//   our reliance on the LDST control dependency.
//
//   Things can get even more complicated especially in C++ when values may come from constexpr, inline, inline constexpr, static const, etc,
//   variables and thus the compiler will do all sorts of transformations to reduce, remove, augment and change all your conditional code since
//   it knows the values of the expressions or even parts of it at compile time. Even more aggressive optimizations like LTO might break code that was being cautious.
//   Even adding simple short circuiting logic or your classic likely/unlikely macros can alter conditionals in ways you didn't expect.
//   In short know enough about control dependencies to know not to ever use them.
//
//   ******** Multi-Copy Store Atomicity && Barrier Cumulativity ********
//
//   Single-Copy Store Atomicity: All stores must become visible to all cores in the system at the same time.
//
//   Multi-Copy Store Atomicity : This means a cpu sees its own stores before they become visible to other cpus, by forwarding them from the store buffer,
//                                but a store becomes visible to all other cpus at the same time when flushed from the store buffer.
//
//   Non-Atomic Store Atomicity : A store becomes visible to different cpus at different times.
//
//   Those are the above variations of Store Atomicity. Most processors have Non-Atomic Store Atomicity and thus you must program to that lowest common denominator.
//   We can use barriers, with some caveats, to restore Multi-Copy Store Atomicity to a Non-Atomic system though we need to define a new granular definition for
//   memory barriers to define this behaviour. Simple LDLD/LDST/STST/STLD definition is not enough to categorize memory barriers at this level. Let's start off
//   with a simple example that breaks under a Non-Atomic Store Atomicity system and what potential hardware features allow this behaviour to be observed.
//
//   NOTE: For all the below examples we assume no compile reordering and that the processor also executes the instructions with no local reorderings to make the examples simpler,
//         to only show off the effects of Multi-Copy Store Atomicity. This is why we don't add any address dependencies, or mark explicit LDLD/LDST memory barriers.
//         Thus you may assume all LDLD and LDST pairs have an address dependency between them, so that they are not reordered by the compiler or the local cpu.
//
//   ---------------------------------------------------------------------------------------------------------
//   Write-To-Read Causality, WRC, Litmus Test
//   ---------------------------------------------------------------------------------------------------------
//   Initial State:
//   X = 0; Y = 0;
//   ---------------------------------------------------------------------------------------------------------
//   Thread 0                 | Thread 1                          | Thread 2
//   ---------------------------------------------------------------------------------------------------------
//   STORE(X, 1)              | r0 = LOAD(X)                      | r1 = LOAD(Y)
//                            | STORE(Y, r0)                      | r2 = LOAD(X)
//   ---------------------------------------------------------------------------------------------------------
//   Observed: r0 = 1 && r1 = 1 && r2 = 0
//   ---------------------------------------------------------------------------------------------------------
//
//   Let's go over this example in detail and whether the outcome shown above can be observed. In this example Thread 0 stores 1 into X. If Thread 1 observes the write to X,
//   it stores the observed value into Y. Thread 2 loads from Y then X. This means if the load from Y returns 1, then we intuitively know the global store order
//   was 1 to X and then 1 to Y. So is it possible then that the load from X in Thread 2 can return 0 in that case? Under a Multi-Copy Store Atomicity system, that would be
//   impossible because once 1 was stored to X all cpus see that store so if Thread 2 saw the store to Y which can only happen after the store to X was observed, then
//   Thread 2 must also have observed the store to X and return 1. As you may well have figured out, it is possible under a Non-Atomic Store Atomicity system to still
//   observe the load from X returning 0 even if the above load from Y returned 1 in Thread 2. This completely breaks our intuition of causality. Let's now understand what hardware may cause this.
//
//   This is possible on cpus that have Simultaneous Multi-Threading, SMT or HyperThreading in Intel parlance, which share resources such as store buffers or L1 cache.
//   We are accustomed to the x86 way of SMT where each logical core shares Execution Units on the physical core but each logical core has their own statically partitioned
//   cache and store buffer that is not visible to the other cpus. It is possible on cpus like ARMv7 or POWER, POWER9 supports 4 and even 8 threads per physical core, so
//   to save on die space though yet enable this large number of threads per physical core it is common for these logical cores to all use the same store buffer or L1 cache
//   per physical core on these processors. Let's take the above example and rerun it with this knowledge to get the observed behaviour outlined above.
//
//   Assume Thread 0, Thread 1, and Thread 2 run on cpu 0, cpu 1, and cpu 2 respectively. Assume that cpu 0 and cpu 1 are two logical cores on the same physical core so this processor
//   has an SMT value of 2. Thread 0 will store 1 into X. This store may be in the store buffer or in the L1 cache that cpu 1 also shares with cpu 0, thus cpu 1 has early access to cpu 0's stores.
//   Thread 1 loads X which it observed as 1 early and then stores 1 into Y. Thread 2 may see the load from Y returning 1 but now the load from X returning 0 all because cpu 1 got early
//   access to cpu 0 store due to sharing a L1 cache or store buffer.
//   We will come back on how to fix this example with the proper memory barriers for the Non-Atomic Store Atomicity systems, but we need to detour first.
//
//   We need to take a deeper dive into memory barriers to understand how to restore Multi-Copy Store Atomicity from a Non-Atomic Store Atomicity system.
//   Let's start with a motivating example and we will be using the POWER architecture throughout this example because it encompasses all the possible observable behaviour.
//   ARMv7 technically allows Non-Atomic Store Atomicity behaviour but no consumer ARMv7 chip actually observes this behaviour.
//   ARMv8 reworked its model to specifically say it is a Multi-Copy Store Atomicity system.
//   POWER is one of the last few popular consumer architectures that are guaranteed to have Non-Atomic Store Atomicity observable behaviour, thus we will be using it for the following examples.
//
//   To preface, POWER has two types of memory barriers called lwsync and sync. The following table lists the guarantees provided by TSO, x86, and the lwsync instruction.
//   The table gives a hint as to why using our previous definition of LDLD/LDST/STST/STLD isn't granular enough to categorize memory barrier instructions.
//
//   TSO:                 |       POWER lwsync memory barrier:
//       LDLD : YES       |                                    LDLD : YES
//       LDST : YES       |                                    LDST : YES
//       STST : YES       |                                    STST : YES
//       STLD : NO        |                                    STLD : NO
//   A cumulative : YES   |                            A cumulative : YES
//   B cumulative : YES   |                            B cumulative : YES
//   IRIW         : YES   |                            IRIW         : NO
//
//   The TSO memory model provided by x86 seems to be exactly the same as POWER if we add lwsync memory barrier instructions in between each of the memory instructions.
//   This provides us the exact same ordering guarantees as the TSO memory model. If we just looked at the 4 permutations of reorderings we would be inclined to assume that
//   TSO has the exact same ordering as sprinkling lwsync in our code in between every pair of memory instructions. That is not the case because memory barrier causality and cumulativity differ in subtle ways.
//   In this case they differ by the implicit guarantees from the TSO memory model versus those provided by the POWER lwsync memory barrier.
//   So the lwsync memory barrier prevents reordering with instructions that have causality but does not prevent reordering with instructions that are completely independent.
//   Let's dive into these concepts a bit more.
//
//   Non-Atomic Store Atomicity architectures are prone to behaviours such as the non-causal outcome of the WRC test above. Architectures such as POWER defines memory barriers to enforce
//   ordering with respect to memory accesses in remote cpus other than the cpu actually issuing the memory barrier. This is known as memory barrier cumulativity.
//   How does the memory barrier issued on my cpu affect the view of memory accesses done by remote cpuss.
//
//   Cumulative memory barriers are defined as follows - Take your time this part is very non-trivial:
//   A-Cumulative: We denote group A as the set of memory instructions in this cpu or other cpus that are ordered before the memory barrier in this cpu.
//                 A-Cumulativity requires that memory instructions from any cpu that have performed prior to a memory load before the memory barrier on this cpu are also members of group A.
//   B-Cumulative: We denote group B as the set of memory instructions in this cpu or other cpus that are ordered after the memory barrier in this cpu.
//                 B-Cumulativity requires that memory instructions from any cpu that perform after a load and including the load in that cpu that returns the value of a store in group B are
//                 also members of group B.
//   IRIW        : enforces a global ordering even for memory instructions that have no causality. The memory instructions are completely independent.
//
//   ---------------------------------------------------------------------------------------------------------
//   WRC Litmus Test
//   ---------------------------------------------------------------------------------------------------------
//   Thread 0                 | Thread 1                          | Thread 2
//   ---------------------------------------------------------------------------------------------------------
//   {i} : STORE(X, 1)        | {ii}  : r0 = LOAD(X)              | {v}  : r1 = LOAD(Y)
//                            | {iii} : lwsync                    |
//                            | {iv}  : STORE(Y, r0)              | {vi} : r2 = LOAD(X)
//   ---------------------------------------------------------------------------------------------------------
//   Outcome: r0 = 1 && r1 = 1 && r2 = 1
//
//   Group A of {iii} : {i} && {ii}
//
//   Group B of {iii} : {iv} && {v} && {vi}
//   ---------------------------------------------------------------------------------------------------------
//
//   Using the WRC test again and inserting a POWER lwsync, don't concern yourself with why the memory barrier was inserted at that spot right now, we now see the distinctions of group A and group B.
//   It demonstrates the A and B Cumulative nature of the lwsync instruction, {iii}. First group A, initially consists of {ii} and group B initially consists of {iv} from the local cpu that issued the lwsync.
//   Since {ii} reads from {i} and assume {i} happens before {ii}, by definition of A-Cumulativity {i} is included in group A.
//   Similarly {v} reads from {iv} and assume {iv} happens before {v}, then {v} is included in group B by definition of B-Cumulativity.
//   {vi} is also included in group B since it happens after {v} by definition of B-Cumulativity.
//
//   WRC litmus test represents a scenario where only a A-Cumulative memory barrier is needed. The lwsync not only provides the needed local LDST memory barrier for the local thread but also ensures
//   that any write Thread 1 has read from before the memory barrier is kept in order with any write Thread 1 does after the memory barrier as far as any other thread observes.
//   In other words it ensures that any write that has propagated to Thread 1 before the memory barrier is propagated to any other thread before the second store after the memory barrier in Thread 1
//   can propagate to other threads in the system. This is exactly the definition of A-Cumulativity and what we need to ensure that causality is maintained in the WRC Litmus Test example.
//   With that lwsync in place it is now impossible to observe r0 = 1 && r1 = 1 && r2 = 0. The lwsync has restored causal ordering. Let's look at an example that requires B-Cumulativity.
//
//   ---------------------------------------------------------------------------------------------------------
//   Example 2 from POWER manual
//   ---------------------------------------------------------------------------------------------------------
//   Initial State:
//   X = 0; Y = 0; Z = 0
//   ---------------------------------------------------------------------------------------------------------
//   Thread 0                 | Thread 1                          | Thread 2
//   ---------------------------------------------------------------------------------------------------------
//   STORE(X, 1)              | r0 = LOAD(Y)                      | r1 = LOAD(Z)
//   STORE(Y, 1)              | STORE(Z, r0)                      | r2 = LOAD(X)
//   ---------------------------------------------------------------------------------------------------------
//   Observed: r0 = 1 && r1 = 1 && r2 = 0
//   ---------------------------------------------------------------------------------------------------------
//
//   This example is very similar to WRC except that we kinda extended the Message Passing through an additional shared variable instead.
//   Think of this as Thread 0 writing some data into X, setting flag Y, Thread 1 waiting for flag Y then writing flag Z, and finally Thread 2 waiting for flag Z before reading the data.
//   Take a minute to digest the above example and think about where a memory barrier, lwsync, should be placed. Don't peek at the solution below.
//
//   ---------------------------------------------------------------------------------------------------------
//   Example 2 from POWER manual
//   ---------------------------------------------------------------------------------------------------------
//   Thread 0                 | Thread 1                          | Thread 2
//   ---------------------------------------------------------------------------------------------------------
//   STORE(X, 1)              | r0 = LOAD(Y)                      | r1 = LOAD(Z)
//   lwsync                   |                                   |
//   STORE(Y, 1)              | STORE(Z, r0)                      | r2 = LOAD(X)
//   ---------------------------------------------------------------------------------------------------------
//
//   First the lwsync provides the needed local STST memory barrier for the local thread, thus the lwsync here ensures that the store to X propagates to Thread 1 before the store to Y.
//   B-Cumulativity applied to all operations after the memory barrier ensure that the store to X is
//   kept in order with respect to the store to Z as far as all other threads participating in the dependency chain are concerned. This is the exact definition of B-Cumulativity.
//   With this one lwsync the outcome outlined above is impossible to observe. If r0 = 1 && r1 = 1 then r2 must be properly observed to be 1.
//
//   We know that lwsync only provides A-Cumulativity and B-Cumulativity. Now we will look at examples that have no causality constraints thus we need to grab heavier memory barriers
//   that ensures in short we will say makes a store become visible to all processors, even those not on the dependency chains. Let's get to the first example.
//
//   ---------------------------------------------------------------------------------------------------------
//   Independent Reads of Independent Writes, IRIW, coined by Doug Lea
//   ---------------------------------------------------------------------------------------------------------
//   Initial State:
//   X = 0; Y = 0;
//   ---------------------------------------------------------------------------------------------------------
//   Thread 0                 | Thread 1                       | Thread 2               | Thread 3
//   ---------------------------------------------------------------------------------------------------------
//   STORE(X, 1)              | r0 = LOAD(X)                   | STORE(Y, 1)            | r2 = LOAD(Y)
//                            | r1 = LOAD(Y)                   |                        | r3 = LOAD(X)
//   ---------------------------------------------------------------------------------------------------------
//   Observed: r0 = 1 && r1 = 0 && r2 = 1 && r3 = 0
//   ---------------------------------------------------------------------------------------------------------
//
//   The IRIW example above clearly shows that writes can be propagated to different cpus in completely different orders.
//   Thread 1 sees the store to X but not the store to Y while Thread 3 sees the store to Y but not the store to X, the complete opposite.
//   Also to the keen eye you may have noticed this example is a slight modification of the Store Buffer example so try to guess where the memory barriers would go.
//
//   ---------------------------------------------------------------------------------------------------------
//   Independent Reads of Independent Writes, IRIW, coined by Doug Lea
//   ---------------------------------------------------------------------------------------------------------
//   Thread 0                 | Thread 1                       | Thread 2               | Thread 3
//   ---------------------------------------------------------------------------------------------------------
//   STORE(X, 1)              | r0 = LOAD(X)                   | STORE(Y, 1)            | r2 = LOAD(Y)
//                            | sync                           |                        | sync
//                            | r1 = LOAD(Y)                   |                        | r3 = LOAD(X)
//   ---------------------------------------------------------------------------------------------------------
//
//   To ensure that the above observation is forbidden we need to add a full sync memory barrier on both the reading threads. Think of sync as restoring sequential consistency.
//   The sync memory barrier ensures that any writes that Thread 1 has read from before the memory barrier are fully propagated to all threads before the reads are satisfied after the memory barrier.
//   The same can be said for Thread 3. This is why the sync memory barrier is needed because there is no partial causal ordering here or anything that can be considered for our A and  B Cumulativity definitions.
//   We must ensure that all writes have been propagated to all cpus before proceeding. This gives way to the difference between sync and lwsync with regards to visibility of writes and cumulativity.
//   sync guarantees that all program-order previous stores must have been propagated to all other cpus before the memory instructions after the memory barrier.
//   lwsync does not ensure that stores before the memory barrier have actually propagated to any other cpu before memory instructions after the memory barrier, but it will keep stores before and after the
//   lwsync in order as far as other cpus are concerned that are within the dependency chain.
//
//   Fun fact while ARMv7 claims to be Non-Atomic Store Atomicity no mainstream ARM implementation that I have seen has shown cases of Non-Atomic Store Atomicity.
//   It's allowed by the ARMv7 memory model and thus you have to program to that. ARMv8 changes this and states that it has Multi-Copy Store Atomicity.
//
//   ******** Release-Acquire Semantics ********
//
//   The most useful and common cases where Release-Acquire Semantics are used in every day code is in message passing and mutexes. Let's get onto some examples and the C++ definition of Release-Acquire.
//
//   ACQUIRE:
//   An Acquire operation is a one-way memory barrier whereby all loads and stores after the acquire operation cannot move up and above the acquire operation.
//   Loads and stores before the acquire operation can move down past the acquire operation. An acquire operation should always be paired with a Release operation on the SAME atomic object.
//
//   RELEASE:
//   A Release operation is a one-way memory barrier whereby all loads and stores before the release operation cannot move down and below the release operation.
//   Loads and stores after the release operation can move up and above the release operation. A release operation should always be paired with an Acquire operation on the SAME atomic object.
//
//   Release-Acquire pair does not create a full memory barrier but it guarantees that all memory instructions before a Release operation on an atomic object M are visible after an Acquire
//   operation on that same atomic object M. Thus these semantics usually are enough to preclude the need for any other memory barriers.
//   The synchronization is established only between the threads Releasing and Acquiring the same atomic object M.
//
//   ---------------------------------------------------
//   Critical Section
//   ---------------------------------------------------
//   Thread 0                 | Thread 1
//   ---------------------------------------------------
//   mtx.lock() - Acquire     | mtx.lock() - Acquire
//   STORE(X, 1)              | r0 = LOAD(X)
//   mtx.unlock() - Release   | mtx.unlock() - Release
//   ---------------------------------------------------
//
//   A mutex only requires Release-Acquire semantics to protect the critical section. We do not care if operations above the lock leak into the critical section or that operations below the unlock leak into the
//   critical section because they are outside the protected region of the lock()/unlock() pair. Release-Acquire semantics does guarantee that everything inside the critical section cannot leak out.
//   Thus all accesses of all previous critical sections for the mutex are guaranteed to have completed and be visible when the mutex is handed off to the next party due to the Release-Acquire chaining.
//   This also means that mutexes do not provide or restore Multi-Copy Store Atomicity to any memory instructions outside the mutex, like the IRIW example since it does not emit full memory barriers.
//
//   ------------------------------------------------------
//   Message Passing
//   ------------------------------------------------------
//   Thread 0                 | Thread 1
//   ------------------------------------------------------
//   STORE(DATA, 1)           | while (!LOAD_ACQUIRE(FLAG))
//                            |
//   STORE_RELEASE(FLAG, 1)   | r0 = LOAD(DATA)
//   ------------------------------------------------------
//
//   This is a common message passing idiom that also shows the use of Release-Acquire semantics. It should be obvious by the definitions outlined above why this works.
//   An Acquire operation attached to a load needs to provide a LDLD and LDST memory barrier according to our definition of acquire. This is provided by default on x86 TSO thus no memory barrier is emitted.
//   A Release operation attached to a store needs to provide a STST and LDST memory barrier according to our definition of release. This is provided by default on x86 TSO thus no memory barrier is emitted.
//
//   A couple of things of note here. One is that by attaching the semantics of a memory model directly to the memory instruction/operation itself we can take advantage of the fact the some processors
//   already provide guarantees between memory instructions and thus we do not have to emit memory barriers. Another thing of note is that the memory model is directly attached to the operation,
//   so you must do the Release-Acquire pairing on the SAME object which in this case is the FLAG variable. Doing an Acquire or Release on a separate object has no guarantee to observe an Acquire or Release on a different object.
//   This better encapsulates the meaning of the code and also allows the processor to potentially do more optimizations since a stand alone memory barrier will order all memory instructions of a given type before and after the barrier.
//   Where as the memory ordering attached to the load or store tells the processor that it only has to order memory instructions in relation to that specific load or store with the given memory order.
//
//
//   ---------------------------------------------------------------------------------------------------------
//   Release Attached to a Store VS. Standalone Fence
//   ---------------------------------------------------------------------------------------------------------
//   STORE(DATA, 1)               |  STORE(DATA, 1)
//                                |  ATOMIC_THREAD_FENCE_RELEASE()
//   STORE_RELEASE(FLAG, 1)       |  STORE_RELAXED(FLAG, 1)
//   STORE_RELAXED(VAR, 2)        |  STORE_RELAXED(VAR, 2)
//   ---------------------------------------------------------------------------------------------------------
//   ARMv8 Assembly
//   ---------------------------------------------------------------------------------------------------------
//   str 1, DATA                  |  str 1, DATA
//                                |  dmb ish
//   stlr 1, FLAG                 |  str 1, FLAG
//   str 2, VAR                   |  str 2, VAR
//   ---------------------------------------------------------------------------------------------------------
//
//   In the above example the release is attached to the FLAG variable, thus synchronization only needs to be guaranteed for that atomic variable.
//   It is entirely possible for the VAR relaxed store to be reordered above the release store.
//   In the fence version, since the fence is standalone, there is no notion where the release is meant to be attached to thus the fence must prevent all subsequent relaxed stores
//   from being reordered above the fence. The fence provides a stronger guarantee whereby now the VAR relaxed store cannot be moved up and above the release operation.
//   Also notice the ARMv8 assembly is different, the release fence must use the stronger dmb ish barrier instead of the dedicated release store instruction.
//   We dive more into fences provided by eastl::atomic<T> below.
//
//   Release-Acquire semantics also have the property that it must chain through multiple dependencies which is where our knowledge from the previous section comes into play.
//   Everything on the Release-Acquire dependency chain must be visible to the next hop in the chain.
//
//   ---------------------------------------------------------------------------------------------------------
//   Example 2 from POWER manual
//   ---------------------------------------------------------------------------------------------------------
//   Thread 0                 | Thread 1                          | Thread 2
//   ---------------------------------------------------------------------------------------------------------
//   STORE(X, 1)              | r0 = LOAD_ACQUIRE(Y)              | r1 = LOAD_ACQUIRE(Z)
//   STORE_RELEASE(Y, 1)      | STORE_RELEASE(Z, r0)              | r2 = LOAD(X)
//   ---------------------------------------------------------------------------------------------------------
//
//   ---------------------------------------------------------------------------------------------------------
//   Write-To-Read Causality, WRC, Litmus Test
//   ---------------------------------------------------------------------------------------------------------
//   Thread 0                 | Thread 1                          | Thread 2
//   ---------------------------------------------------------------------------------------------------------
//   STORE(X, 1)              | r0 = LOAD(X)                      | r1 = LOAD_ACQUIRE(Y)
//                            | STORE_RELEASE(Y, r0)              | r2 = LOAD(X)
//   ---------------------------------------------------------------------------------------------------------
//
//   You may notice both of these examples from the previous section. We replaced the standalone POWER memory barrier instructions with Release-Acquire semantics attached directly to the operations where we want causality preserved.
//   We have transformed those examples to use the eastl::atomic<T> memory model.
//   Take a moment to digest these examples in relation to the definition of Release-Acquire semantics.
//
//   The Acquire chain can be satisfied by reading the value from the store release or any later stored headed by that release operation. The following examples will make this clearer.
//
//   ------------------------------------------------------
//   Release Sequence Headed
//   ------------------------------------------------------
//   Initial State:
//   DATA = 0; FLAG = 0;
//   ------------------------------------------------------
//   Thread 0                 | Thread 1
//   ------------------------------------------------------
//   STORE(DATA, 1)           | r0 = LOAD_ACQUIRE(FLAG)
//                            |
//   STORE_RELEASE(FLAG, 1)   | r1 = LOAD(DATA)
//   STORE_RELAXED(FLAG, 3)   |
//   ------------------------------------------------------
//   Observed: r0 = 3 && r1 = 0
//   ------------------------------------------------------
//
//   In the above example we may read the value 3 from FLAG which was not the release store, but it was headed by that release store. Thus we observed a later store and therefore it is still valid to then observe r1 = 1.
//   The stores to FLAG from the STORE_RELEASE up to but not including the next STORE_RELEASE operation make up the release sequence headed by the first release store operation. Any store on that sequence can be used to enforce
//   causality on the load acquire.
//
//   ******** Consume is currently not useful ********
//
//   Consume is a weaker form of an acquire barrier and creates the Release-Consume barrier pairing.
//   Consume states that a load operation on an atomic object M cannot allow any loads or stores dependent on the value loaded by the operation to be reordered before the operation.
//   To understand consume we must first understand dependent loads.
//   You might encounter this being called a data dependency or an address dependency in some literature.
//
//   --------------------------------------------------------------
//   Address Dependency
//   --------------------------------------------------------------
//   Initial State:
//   DATA = 0; PTR = nullptr;
//   --------------------------------------------------------------
//   Thread 0                 | Thread 1
//   --------------------------------------------------------------
//   STORE(DATA, 1)           | r0 = LOAD(PTR) - typeof(r0) = int*
//                            |
//   STORE(PTR, &DATA)        | r1 = LOAD(r0)  - typeof(r1) = int
//   --------------------------------------------------------------
//
//   There is a clear dependency here where we cannot load from *int until we actually read the int* from memory.
//   Now it is possible for Thread 1's load from *ptr to be observed before the store to DATA, therefore it can lead to r0 = &DATA && r1 = 0.
//   While this is a failure of causality, it is allowed by some cpus such as the DEC Alpha and I believe Blackfin as well.
//   Thus a data dependency memory barrier must be inserted between the data dependent loads in Thread 1. Note that this would equate to a nop on any processor other than the DEC Alpha.
//
//   This can occur for a variety of hardware reasons. We learned about invalidation queues. It is possible that the invalidation for DATA gets buffered in Thread 1. DEC Alpha allows the Thread 1
//   load from PTR to continue without marking the entries in its invalidation queue. Thus the subsequent load is allowed to return the old cached value of DATA instead of waiting for the
//   marked entries in the invalidation queue to be processed. It is a design decision of the processor not to do proper dependency tracking here and instead relying on the programmer to insert memory barriers.
//
//   This data dependent ordering guarantee is useful because in places where we were using an Acquire memory barrier we can reduce it to this Consume memory barrier without any hardware barriers actually emitted on every modern processor.
//   Let's take the above example, translate it to Acquire and Consume memory barriers and then translate it to the ARMv7 assembly and see the difference.
//
//   ---------------------------------------------------------------     ---------------------------------------------------------------
//   Address Dependency - Release-Acquire                                Address Dependency - Release-Acquire - ARMv7 Assembly
//   ---------------------------------------------------------------     ---------------------------------------------------------------
//   Thread 0                  | Thread 1                                Thread 0                  | Thread 1
//   ---------------------------------------------------------------     ---------------------------------------------------------------
//   STORE(DATA, 1)            | r0 = LOAD_ACQUIRE(PTR)                  STORE(DATA, 1)            | r0 = LOAD(PTR)
//                             |                                         dmb ish                   | dmb ish
//   STORE_RELEASE(PTR, &DATA) | r1 = LOAD(r0)                           STORE(PTR, &DATA)         | r1 = LOAD(r0)
//   ---------------------------------------------------------------     ---------------------------------------------------------------
//
//   To get Release-Acquire semantics on ARMv7 we need to emit dmb ish; memory barriers.
//
//   ---------------------------------------------------------------     ---------------------------------------------------------------
//   Address Dependency - Release-Consume                                Address Dependency - Release-Consume - ARMv7 Assembly
//   ---------------------------------------------------------------     ---------------------------------------------------------------
//   Thread 0                  | Thread 1                                Thread 0                  | Thread 1
//   ---------------------------------------------------------------     ---------------------------------------------------------------
//   STORE(DATA, 1)            | r0 = LOAD_CONSUME(PTR)                  STORE(DATA, 1)            | r0 = LOAD(PTR)
//                             |                                         dmb ish                   |
//   STORE_RELEASE(PTR, &DATA) | r1 = LOAD(r0)                           STORE(PTR, &DATA)         | r1 = LOAD(r0)
//   ---------------------------------------------------------------     ---------------------------------------------------------------
//
//   Data Dependencies can not only be created by read-after-write/RAW on registers, but also by RAW on memory locations too. Let's look at some more elaborate examples.
//
//   ---------------------------------------------------------------     ---------------------------------------------------------------
//   Address Dependency on Registers - Release-Consume - ARMv7               Address Dependency on Memory - Release-Consume - ARMv7
//   ---------------------------------------------------------------     ---------------------------------------------------------------
//   Thread 0                  | Thread 1                                Thread 0                  | Thread 1
//   ---------------------------------------------------------------     ---------------------------------------------------------------
//   STORE(DATA, 1)            | r0 = LOAD(PTR)                          STORE(DATA, 1)            | r0 = LOAD(PTR)
//                             | r1 = r0 + 0                                                       | STORE(TEMP, r0)
//   dmb ish                   | r2 = r1 - 0                             dmb ish                   | r1 = LOAD(TEMP)
//   STORE(PTR, &DATA)         | r3 = LOAD(r2)                           STORE(PTR, &DATA)         | r2 = LOAD(r1)
//   ---------------------------------------------------------------     ---------------------------------------------------------------
//
//   The above shows a more elaborate example of how data dependent dependencies flow through RAW chains either through memory or through registers.
//
//   Notice by identifying that this is a data dependent operation and asking for a consume ordering, we can completely eliminate the memory barrier on Thread 1 since we know ARMv7 does not reorder data dependent loads. Neat.
//   Unfortunately every major compiler upgrades a consume to an acquire ordering, because the consume ordering in the standard has a stronger guarantee and requires the compiler to do complicated dependency tracking.
//   Dependency chains in source code must be mapped to dependency chains at the machine instruction level until a std::kill_dependency in the source code.
//
//   ----------------------------------------------------------------
//   Non-Address Dependency && Multiple Chains
//   ----------------------------------------------------------------
//   Initial State:
//   std::atomic<int> FLAG; int DATA[1] = 0;
//   ----------------------------------------------------------------
//   Thread 0                   | Thread 1
//   ----------------------------------------------------------------
//   STORE(DATA[0], 1)          | int f = LOAD_CONSUME(FLAG)
//                              | int x = f
//                              | if (x) return Func(x);
//                              |
//   STORE_RELEASE(FLAG, 1)     | Func(int y) return DATA[y - y]
//   ----------------------------------------------------------------
//
//   This example is really concise but there is a lot going on. Let's digest it.
//   First is that the standard allows consume ordering even on what we will call not true machine level dependencies like a ptr load and then a load from that ptr as shown in the previous examples.
//   Here the dependency is between two ints, and the dependency chain on Thread 1 is as follows. f -> x -> y -> DATA[y - y]. The standard requires that source code dependencies on the loaded value
//   from consume flow thru assignments and even thru function calls. Also notice we added a dependency on the dereference of DATA with the value loaded from consume which while it does nothing actually abides by the standard
//   by enforcing a source code data dependent load on the consume operation. You may see this referred to as artificial data dependencies in other texts.
//   If we assume the compiler is able to track all these dependencies, the question is how do we enforce these dependencies at the machine instruction level. Let's go back to our ptr dependent load example.
//
//   ----------------------------------------------------------------
//   addi r0, pc, offset;
//   ldr r1, 0(r0);
//   ldr r2, 0(r1);
//   ----------------------------------------------------------------
//
//   The above pseudo assembly does a pc relative calculation to find the address of ptr. We then load ptr and then continue the dependency chain by loading the int from the loaded ptr.
//   Thus r0 has type of int**, which we use to load r1 an int* which we use to load our final value of r2 which is the int.
//   The key observation here is that most instructions provided by most architectures only allow moving from a base register + offset into a destination register.
//   This allows for trivial capturing of data dependent loads through pointers. But how do we capture the data dependency of DATA[y - y]. We would need something like this.
//
//   ----------------------------------------------------------------
//   sub r1, r0, r0; // Assume r0 holds y from the Consume Operation
//   add r3, r1, r2; // Assume r2 holds the address of DATA[0]
//   ldr r4, 0(r3);
//   ----------------------------------------------------------------
//
//   We cannot use two registers as both arguments to the load instruction. Thus to accomplish this you noticed we had to add indirect data dependencies through registers to compute the final address from the consume
//   load of y and then load from the final computed address. The compiler would have to recognize all these dependencies and enforce that they be maintained in the generated assembly.
//   The compiler must ensure the entire syntactic, source code, data-dependency chain is enforced in the generated assembly, no matter how long such chain may be.
//   Because of this and other issues, every major compiler unilaterally promotes consume to an acquire operation across the board. Read reference [15] for more information.
//   This completely removes the actual usefulness of consume for the pointer dependent case which is used quite heavily in concurrent read heavy data structures where updates are published via pointer swaps.
//
//   ******** read_depends use case - Release-ReadDepends Semantics ********
//
//   eastl::atomic<T> provides a weaker read_depends operation that only encapsulates the pointer dependency case above. Loading from a pointer and then loading the value from the loaded pointer.
//   The read_depends operation can be used on loads from only an eastl::atomic<T*> type. The return pointer of the load must and can only be used to then further load values. And that is it.
//   If you are unsure, upgrade this load to an acquire operation.
//
//   MyStruct* ptr = gAtomicPtr.load(memory_order_read_depends);
//   int a = ptr->a;
//   int b = ptr->b;
//   return a + b;
//
//   The loads from ptr after the gAtomicPtr load ensure that the correct values of a and b are observed. This pairs with a Release operation on the writer side by releasing gAtomicPtr.
//
//
//   As said above the returned pointer from a .load(memory_order_read_depends) can only be used to then further load values.
//   Dereferencing(*) and Arrow Dereferencing(->) are valid operations on return values from .load(memory_order_read_depends).
//
//   MyStruct* ptr = gAtomicPtr.load(memory_order_read_depends);
//   int a = ptr->a;  - VALID
//   int a = *ptr;    - VALID
//
//   Since dereferencing is just indexing via some offset from some base address, this also means addition and subtraction of constants is ok.
//
//   int* ptr = gAtomicPtr.load(memory_order_read_depends);
//   int a = *(ptr + 1)  - VALID
//   int a = *(ptr - 1)  - VALID
//
//   Casts also work correctly since casting is just offsetting a pointer depending on the inheritance hierarchy or if using intrusive containers.
//
//   ReadDependsIntrusive** intrusivePtr = gAtomicPtr.load(memory_order_read_depends);
//   ReadDependsIntrusive* ptr = ((ReadDependsIntrusive*)(((char*)intrusivePtr) - offsetof(ReadDependsIntrusive, next)));
//
//   Base* basePtr = gAtomicPtr.load(memory_order_read_depends);
//   Dervied* derivedPtr = static_cast<Derived*>(basePtr);
//
//   Both of the above castings from the result of the load are valid for this memory order.
//
//   You can reinterpret_cast the returned pointer value to a uintptr_t to set bits, clear bits, or xor bits but the pointer must be casted back before doing anything else.
//
//   int* ptr = gAtomicPtr.load(memory_order_read_depends);
//   ptr = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(ptr) & ~3);
//
//   Do not use any equality or relational operator (==, !=, >, <, >=, <=) results in the computation of offsets before dereferencing.
//   As we learned above in the Control Dependencies section, CPUs will not order Load-Load Control Dependencies. Relational and equality operators are often compiled using branches.
//   It doesn't have to be compiled to branched, condition instructions could be used. Or some architectures provide comparison instructions such as set less than which do not need
//   branches when using the result of the relational operator in arithmetic statements. Then again short circuiting may need to introduct branches since C++ guarantees the
//   rest of the expression must not be evaluated.
//   The following odd code is forbidden.
//
//   int* ptr = gAtomicPtr.load(memory_order_read_depends);
//   int* ptr2 = ptr + (ptr >= 0);
//   int a = *ptr2;
//
//   Only equality comparisons against nullptr are allowed. This is becase the compiler cannot assume that the address of the loaded value is some known address and substitute our loaded value.
//   int* ptr = gAtomicPtr.load(memory_order_read_depends);
//   if (ptr == nullptr);  - VALID
//   if (ptr != nullptr);  - VALID
//
//   Thus the above sentence that states:
//   The return pointer of the load must and can only be used to then further load values. And that is it.
//   must be respected by the programmer. This memory order is an optimization added for efficient read heavy pointer swapping data structures. IF you are unsure, use memory_order_acquire.
//
//   ******** Relaxed && eastl::atomic<T> guarantees ********
//
//   We saw various ways that compiler barriers do not help us and that we need something more granular to make sure accesses are not mangled by the compiler to be considered atomic.
//   Ensuring these guarantees like preventing dead-store elimination or the splitting of stores into smaller sub stores is where the C/C++11
//   standard comes into play to define what it means to operate on an atomic object.
//   These basic guarantees are provided via new compiler intrinsics on gcc/clang that provide explicit indication to the compiler.
//   Or on msvc by casting the underlying atomic T to a volatile T*, providing stronger compiler guarantees than the standard requires.
//   Essentially volatile turns off all possible optimizations on that variable access and ensures all volatile variables cannot be
//   reordered across sequence points. Again we are not using volatile here to guarantee atomicity, we are using it in its very intended purpose
//   to tell the compiler it cannot assume anything about the contents of that variable. Now let's dive into the base guarantees of eastl::atomic<T>.
//
//   The standard defines the following for all operations on an atomic object M.
//
//   Write-Write Coherence:
//   If an operation A modifies an atomic object M(store), happens before an operation B that modifies M(store), then A shall be earlier than B in the modification order of M.
//
//   Read-Read Coherence:
//   If a value computation A on an atomic object M(load), happens before a value computation B on M(load), and A takes its value from a side effect X on M(from a previous store to M), then the value
//   computed by B shall either be the value stored by X or some later side effect Y on M, where Y follows X in the modification order of M.
//
//   Read-Write Coherence:
//   If a value computation A on an atomic object M(load), happens before an operation B that modifies M(store), then A shall take its value from a side effect X on M, where X precedes B in the modification
//   order of M.
//
//   Write-Read Coherence:
//   If a side effect X on an atomic object M(store), happens before a value computation B on M(load), then the evaluation of B must take its value from X or from some side effect Y that follows X in the
//   modification order of M.
//
//   What does all this mean. This is just a pedantic way of saying that the preceding coherence requirements disallow compiler reordering of atomic operations to a single atomic object.
//   This means all operations must be emitted by the compiler. Stores cannot be dead-store eliminated even if they are the only stores.
//   Loads cannot have common subexpression elimination performed on them even if they are the only loads.
//   Loads and Stores to the same atomic object cannot be reordered by the compiler.
//   Compiler cannot introduce extra loads or stores to the atomic object.
//   Compiler also cannot reload from an atomic object, it must save and store to a stack slot.
//   Essentially this provides all the necessary guarantees needed when treating an object as atomic from the compilers point of view.
//
//   ******** Same Address LoadLoad Reordering ********
//
//   It is expected that same address operations cannot and are not reordered with each other. It is expected that operations to the same address have sequential consistency because
//   they are to the same address. If you picture a cpu executing instructions, how is it possible to reorder instructions to the same address and yet keep program behaviour the same.
//   Same Address LoadLoad Reordering is one weakening that is possible to do and keep observed program behaviour for a single-threaded program.
//   More formally, A and B are two memory instructions onto the same address P, where A is program ordered before B. If A and B are both loads then their order need not be ordered.
//   If B is a store then it cannot retire the store before A instruction completes. If A is a store and B is a load, then B must get its value forwarded from the store buffer or observe a later store
//   from the cache. Thus Same Address LDST, STST, STLD cannot be reordered but Same Address LDLD can be reordered.
//   Intel Itanium and SPARC RMO cpus allow and do Same Address LoadLoad Reordering.
//   Let's look at an example.
//
//   ---------------------------
//   Same Address LoadLoad
//   ---------------------------
//   Initial State:
//   x = 0;
//   ---------------------------
//   Thread 0     |    Thread 1
//   ---------------------------
//   STORE(x, 1)  | r0 = LOAD(x)
//                | r1 = LOAD(x)
//   ---------------------------
//   Observed: r0 = 1 && r0 = 0
//   ---------------------------
//
//   Notice in the above example it has appeared as if the two loads from the same address have been reordered. If we first observed the new store of 1, then the next load should not observe a value in the past.
//   Many programmers, expect same address sequential consistency, all accesses to a single address appear to execute in a sequential order.
//   Notice this violates the Read-Read Coherence for all atomic objects defined by the std and thus provided by eastl::atomic<T>.
//
//   All operations on eastl::atomic<T> irrelevant of the memory ordering of the operation provides Same Address Sequential Consistency since it must abide by the coherence rules above.
//
//   ******** eastl::atomic_thread_fence ********
//
//   eastl::atomic_thread_fence(relaxed) : Provides no ordering guarantees
//   eastl::atomic_thread_fence(acquire) : Prevents all prior loads from being reordered with all later loads and stores, LDLD && LDST memory barrier
//   eastl::atomic_thread_fence(release) : Prevents all prior loads and stores from being reordered with all later stores, STST && LDST memory barrier
//   eastl::atomic_thread_fence(acq_rel) : Union of acquire and release, LDLD && STST && LDST memory barrier
//   eastl::atomic_thread_fence(seq_cst) : Full memory barrier that provides a single total order
//
//   See Reference [9] and Fence-Fence, Atomic-Fence, Fence-Atomic Synchronization, Atomics Order and Consistency in the C++ std.
//
//   ******** Atomic && Fence Synchronization ********
//
//   ---------------------------
//   Fence-Fence Synchronization
//   ---------------------------
//   A release fence A synchronizes-with an acquire fence B if there exist operations X and Y on the same atomic object M, such that fence A is sequenced-before operation X and X modifies M,
//   operation Y is sequenced-before B and Y reads the value written by X.
//   In this case all non-atomic and relaxed atomic stores that are sequenced-before fence A will happen-before all non-atomic and relaxed atomic loads after fence B.
//
//   ----------------------------
//   Atomic-Fence Synchronization
//   ----------------------------
//   An atomic release operation A on atomic object M synchronizes-with an acquire fence B if there exists some atomic operation X on atomic object M, such that X is sequenced-before B and reads
//   the value written by A.
//   In this case all non-atomic and relaxed atomic stores that are sequenced-before atomic release operation A will happen-before all non-atomic and relaxed atomic loads after fence B.
//
//   ----------------------------
//   Fence-Atomic Synchronization
//   ----------------------------
//   A release fence A synchronizes-with an atomic acquire operation B on an atomic object M if there exists an atomic operation X such that A is sequenced-before X, X modifies M and B reads the
//   value written by X.
//   In this case all non-atomic and relaxed atomic stores that are sequenced-before fence A will happen-before all non-atomic and relaxed atomic loads after atomic acquire operation B.
//
//   This can be used to add synchronization to a series of several relaxed atomic operations, as in the following trivial example.
//
//   ----------------------------------------------------------------------------------------
//   Initial State:
//   x = 0;
//   eastl::atomic<int> y = 0;
//   z = 0;
//   eastl::atomic<int> w = 0;
//   ----------------------------------------------------------------------------------------
//   Thread 0                                   | Thread 1
//   ----------------------------------------------------------------------------------------
//   x = 2                                      | r0 = y.load(memory_order_relaxed);
//   z = 2                                      | r1 = w.load(memory_order_relaxed);
//   atomic_thread_fence(memory_order_release); | atomic_thread_fence(memory_order_acquire);
//   y.store(1, memory_order_relaxed);          | r2 = x
//   w.store(1, memory_order_relaxed);          | r3 = z
//   ----------------------------------------------------------------------------------------
//   Observed: r0 = 1 && r1 = 1 && r2 = 0 && r3 = 0
//   ----------------------------------------------------------------------------------------
//
//   ******** Atomic vs Standalone Fence ********
//
//   A sequentially consistent fence is stronger than a sequentially consistent operation because it is not tied to a specific atomic object.
//   An atomic fence must provide synchronization with ANY atomic object whereas the ordering on the atomic object itself must only provide
//   that ordering on that SAME atomic object. Thus this can provide cheaper guarantees on architectures with dependency tracking hardware.
//   Let's look at a concrete example that will make this all clear.
//
//   ----------------------------------------------------------------------------------------
//   Initial State:
//   eastl::atomic<int> y = 0;
//   eastl::atomic<int> z = 0;
//   ----------------------------------------------------------------------------------------
//   Thread 0                                   | Thread 1
//   ----------------------------------------------------------------------------------------
//   z.store(2, memory_order_relaxed);          | r0 = y.load(memory_order_relaxed);
//   atomic_thread_fence(memory_order_seq_cst); | atomic_thread_fence(memory_order_seq_cst);
//   y.store(1, memory_order_relaxed);          | r1 = z.load(memory_order_relaxed);
//   ----------------------------------------------------------------------------------------
//   Observed: r0 = 1 && r1 = 0
//   ----------------------------------------------------------------------------------------
//
//   Here the two sequentially consistent fences synchronize-with each other thus ensuring that if we observe r0 = 1 then we also observe that r1 = 2.
//   In the above example if we observe r0 = 1 it is impossible to observe r1 = 0.
//
//   ----------------------------------------------------------------------------------------
//   Initial State:
//   eastl::atomic<int> x = 0;
//   eastl::atomic<int> y = 0;
//   eastl::atomic<int> z = 0;
//   ----------------------------------------------------------------------------------------
//   Thread 0                                   | Thread 1
//   ----------------------------------------------------------------------------------------
//   z.store(2, memory_order_relaxed);          | r0 = y.load(memory_order_relaxed);
//   x.fetch_add(1, memory_order_seq_cst);      | x.fetch_add(1, memory_order_seq_cst);
//   y.store(1, memory_order_relaxed);          | r1 = z.load(memory_order_relaxed);
//   ----------------------------------------------------------------------------------------
//   Observed: r0 = 1 && r1 = 0
//   ----------------------------------------------------------------------------------------
//
//   Here the two fetch_add sequentially consistent operations on x synchronize-with each other ensuring that if we observe r0 = 1 then we cannot observer r1 = 0;
//   The thing to take note here is that we synchronized on the SAME atomic object, that being the atomic object x.
//   Note that replacing the x.fetch_add() in Thread 1 with a sequentially consistent operation on another atomic object or a sequentially consistent fence can lead to
//   observing r1 = 0 even if we observe r0 = 1. For example the following code may fail.
//
//   ----------------------------------------------------------------------------------------
//   Initial State:
//   eastl::atomic<int> x = 0;
//   eastl::atomic<int> y = 0;
//   eastl::atomic<int> z = 0;
//   ----------------------------------------------------------------------------------------
//   Thread 0                                   | Thread 1
//   ----------------------------------------------------------------------------------------
//   z.store(2, memory_order_relaxed);          | r0 = y.load(memory_order_relaxed);
//                                              | x.fetch_add(1, memory_order_seq_cst);
//   y.fetch_add(1, memory_order_seq_cst);      | r1 = z.load(memory_order_relaxed);
//   ----------------------------------------------------------------------------------------
//   Observed: r0 = 1 && r1 = 0
//   ----------------------------------------------------------------------------------------
//
//   ----------------------------------------------------------------------------------------
//   Initial State:
//   eastl::atomic<int> x = 0;
//   eastl::atomic<int> y = 0;
//   eastl::atomic<int> z = 0;
//   ----------------------------------------------------------------------------------------
//   Thread 0                                   | Thread 1
//   ----------------------------------------------------------------------------------------
//   z.store(2, memory_order_relaxed);          | r0 = y.load(memory_order_relaxed);
//   x.fetch_add(1, memory_order_seq_cst);      | atomic_thread_fence(memory_order_seq_cst);
//   y.store(1, memory_order_relaxed);          | r1 = z.load(memory_order_relaxed);
//   ----------------------------------------------------------------------------------------
//   Observed: r0 = 1 && r1 = 0
//   ----------------------------------------------------------------------------------------
//
//   In this example it is entirely possible that we observe r0 = 1 && r1 = 0 even though we have source code causality and sequentially consistent operations.
//   Observability is tied to the atomic object on which the operation was performed and the thread fence doesn't synchronize-with the fetch_add because
//   there is no load above the fence that reads the value from the fetch_add.
//
//   ******** Sequential Consistency Semantics ********
//
//   See section, Order and consistency, in the C++ std and Reference [9].
//
//   A load with memory_order_seq_cst performs an acquire operation
//   A store with memory_order_seq_cst performs a release operation
//   A RMW with memory_order_seq_cst performs both an acquire and a release operation
//
//   All memory_order_seq_cst operations exhibit the below single total order in which all threads observe all modifications in the same order
//
//   Paraphrasing, there is a single total order on all memory_order_seq_cst operations, S, such that each sequentially consistent operation B that loads a value from
//   atomic object M observes either the result of the last sequentially consistent modification A on M, or some modification on M that isn't memory_order_seq_cst.
//   For atomic modifications A and B on an atomic object M, B occurs after A in the total order of M if:
//   there is a memory_order_seq_cst fence X whereby A is sequenced before X, and X precedes B,
//   there is a memory_order_seq_cst fence Y whereby Y is sequenced before B, and A precedes Y,
//   there are memory_order_seq_cst fences X and Y such that A is sequenced before X, Y is sequenced before B, and X precedes Y.
//
//   Let's look at some examples using memory_order_seq_cst.
//
//   ------------------------------------------------------------
//   Store-Buffer
//   ------------------------------------------------------------
//   Initial State:
//   x = 0; y = 0;
//   ------------------------------------------------------------
//   Thread 0                     |    Thread 1
//   ------------------------------------------------------------
//   STORE_RELAXED(x, 1)          | STORE_RELAXED(y, 1)
//   ATOMIC_THREAD_FENCE(SEQ_CST) | ATOMIC_THREAD_FENCE(SEQ_CST)
//   r0 = LOAD_RELAXED(y)         | r1 = LOAD_RELAXED(x)
//   ------------------------------------------------------------
//   Observed: r0 = 0 && r1 = 0
//   ------------------------------------------------------------
//
//   ------------------------------------------------------------
//   Store-Buffer
//   ------------------------------------------------------------
//   Initial State:
//   x = 0; y = 0;
//   ------------------------------------------------------------
//   Thread 0                     |    Thread 1
//   ------------------------------------------------------------
//   STORE_SEQ_CST(x, 1)          | STORE_SEQ_CST(y, 1)
//   r0 = LOAD_SEQ_CST(y)         | r1 = LOAD_SEQ_CST(x)
//   ------------------------------------------------------------
//   Observed: r0 = 0 && r1 = 0
//   ------------------------------------------------------------
//
//   Both solutions above are correct to ensure that the end results cannot lead to both r0 and r1 returning 0. Notice that the second one requires memory_order_seq_cst on both
//   operations to ensure they are in the total order, S, for all memory_order_seq_cst operations. The other example uses the stronger guarantee provided by a sequentially consistent fence.
//
//   ------------------------------------------------------------------------------------------------
//   Read-To-Write Causality
//   ------------------------------------------------------------------------------------------------
//   Initial State:
//   x = 0; y = 0;
//   ------------------------------------------------------------------------------------------------
//   Thread 0                     |    Thread 1                  |   Thread 2
//   ------------------------------------------------------------------------------------------------
//   STORE_SEQ_CST(x, 1)          | r0 = LOAD_RELAXED(x)         | STORE_RELAXED(y, 1)
//                                | ATOMIC_THREAD_FENCE(SEQ_CST) | ATOMIC_THREAD_FENCE(SEQ_CST)
//                                | r1 = LOAD_RELAXED(y)         | r2 = LOAD_RELAXED(x)
//   ------------------------------------------------------------------------------------------------
//   Observed: r0 = 1 && r1 = 0 && r2 = 0
//   ------------------------------------------------------------------------------------------------
//
//   You'll notice this example is an in between example of the Store-Buffer and IRIW examples we have seen earlier. The store in Thread 0 needs to be sequentially consistent so it synchronizes with the
//   thread fence in Thread 1. C++20 due to Reference [9], increased the strength of sequentially consistent fences has been increased to allow for the following.
//
//   ------------------------------------------------------------------------------------------------
//   Read-To-Write Causality - C++20
//   ------------------------------------------------------------------------------------------------
//   Initial State:
//   x = 0; y = 0;
//   ------------------------------------------------------------------------------------------------
//   Thread 0                     |    Thread 1                  |   Thread 2
//   ------------------------------------------------------------------------------------------------
//   STORE_RELAXED(x, 1)          | r0 = LOAD_RELAXED(x)         | STORE_RELAXED(y, 1)
//                                | ATOMIC_THREAD_FENCE(SEQ_CST) | ATOMIC_THREAD_FENCE(SEQ_CST)
//                                | r1 = LOAD_RELAXED(y)         | r2 = LOAD_RELAXED(x)
//   ------------------------------------------------------------------------------------------------
//   Observed: r0 = 1 && r1 = 0 && r2 = 0
//   ------------------------------------------------------------------------------------------------
//
//   Notice we were able to turn the store in Thread 0 into a relaxed store and still properly observe either r1 or r2 returning 1.
//   Note that all implementations of the C++11 standard for every architecture even now allows the C++20 behaviour.
//   The C++20 standard memory model was brought up to recognize that all current implementations are able to implement them stronger.
//
//   ******** False Sharing ********
//
//   As we know operations work on the granularity of a cacheline. A RMW operation obviously must have some help from the cache to ensure the entire operation
//   is seen as one whole unit. Conceptually we can think of this as the cpu's cache taking a lock on the cacheline, the cpu doing the read-modify-write operation on the
//   locked cacheline, and then releasing the lock on the cacheline. This means during that time any other cpu needing that cacheline must wait for the lock to be released.
//
//   If we have two atomic objects doing RMW operations and they are within the same cacheline, they are unintentionally contending and serializing with each other even
//   though they are two completely separate objects. This gives us the common name to this phenomona called false sharing.
//   You can cacheline align your structure or the eastl::atomic<T> object to prevent false sharing.
//
//   ******** union of eastl::atomic<T> ********
//
//   union { eastl::atomic<uint8_t> atomic8; eastl::atomic<uint32_t> atomic32; };
//
//   While we know that operations operate at the granularity of a processor's cacheline size and so we may expect that storing and loading
//   from different width atomic variables at the same address to not cause weird observable behaviour but it may.
//   Store Buffers allow smaller stores to replace parts of larger loads that are forwarded from a store buffer.
//   This means if there is 2 bytes of modified data in the store buffer that overlaps with a 4 byte load, the 2 bytes will be forwarded
//   from the store buffer. This is even documented behaviour of the x86 store buffer in the x86 architecture manual.
//   This behaviour can cause processors to observe values that have never and will never be visible on the bus to other processors.
//   The use of a union with eastl::atomic<T> is not wrong but your code must be able to withstand these effects.
//
//   Assume everything starts out initially as zero.
//
//   -------------------------------------------------------------------------------------------------------
//   Thread 0                 | Thread 1                          | Thread 2
//   --------------------------------------------------------------------------------------------------------
//   cmpxchg 0 -> 0x11111111  | cmpxchg 0x11111111 -> 0x22222222  | mov byte 0x33; mov 4 bytes into register;
//   ---------------------------------------------------------------------------------------------------------
//
//   After all operations complete, the value in memory at that location is, 0x22222233.
//   It is possible that the 4 byte load in thread 2 actually returns 0x11111133.
//   Now 0x11111133 is an observed value that no other cpu could observe because it was never globally visible on the data bus.
//
//   If the value in memory is 0x22222233 then the first cmpxchg succeeded, then the second cmpxchg succeeded and finally our
//   byte to memory was stored, yet our load returned 0x11111133. This is because store buffer contents can be forwarded to overlapping loads.
//   It is possible that the byte store got put in the store buffer. Our load happened after the first cmpxchg with the byte forwarded.
//   This behaviour is fine as long as your algorithm is able to cope with this kind of store buffer forwarding effects.
//
//   Reference [13] is a great read on more about this topic of mixed-size concurrency.
//


/////////////////////////////////////////////////////////////////////////////////


#include <EASTL/internal/atomic/atomic.h>
#include <EASTL/internal/atomic/atomic_standalone.h>
#include <EASTL/internal/atomic/atomic_flag.h>
#include <EASTL/internal/atomic/atomic_flag_standalone.h>


#endif /* EASTL_ATOMIC_H */
