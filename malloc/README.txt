This projects implements a partially optimized dynamic memory allocator, emulating the libc provided malloc.

Provided API, relying on the malloc libc memory management functions. Specific documentation on header file.

Each block uses inner 48 bytes sized metadata structures including inner data structures pointers and block sizes.

Projects implements a segregated list-based memory management logic - a 128 sized array of doubly-linked lists of free memory blocks (bins).
Each ith bin holds block of size 1000*i bytes - (1001)*i-1 bytes.

All allocated blocks are kept in another doubly-linked list, for managing and freeing existing used blocks.


Main Optimizations implemented:

*   Allocation functions search relevant bin for any allocation request, moving on to the next if request cannot be satisfied by the current bin.
    These structures help in quick finding of free block in smallest sizes possible to satisfy memory allocation request, in order to minimize
    external fragmentation.

*   Large (over 128kb) allocations are allocated using mmap, to reduce external fragmentation.

*   Implementation cuts syscalls (especially sbrk) to a minimum, to provide better time and space complexity. Usage of free previously allocated block 
    is always preffered, if at all possible.

*   When freed, all blocks are merged with neighboring blocks in relevant bin list if possible. Neighboring block in bin are merged when a block is allocated
    from said bin. All included in order to make sure maximum sized blocks are maintained, so that external fragmentation is minimized.
    Upon allocating, any free remainder of the allocated block, if larger that a predefined threshold, is reinserted to the appropriate list a new free block.

*   If increasing the heap is unavoidable, upper-most allocated block, if free, is taken into account in mmap/sbrk allocation, to minimize heap/mapping size.

*   All blocks are aligned to a size of a multiplication of 8, to make CPU run less machine instructions when using above functions (based on locality)