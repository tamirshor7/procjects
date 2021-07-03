#ifndef MALLOC_H
#define MALLOC_H

#include <unistd.h>

//Memory Management API
void* smalloc (size_t size); //allocated given number of bytes on heap
void* scalloc (size_t num, size_t size); //allocate a sequence of num*size bytes on heap and initialize contents to 0 
void* srealloc(void* oldp, size_t size); //reallocate pre-allocated space on heap. Reuse space if possible
void  sfree(void* p); //free pre-allocated space pointed by p

//Statistics API
size_t _num_free_blocks(); // overall free blocks number
size_t _num_free_bytes();  // overall free bytes number
size_t _num_allocated_blocks(); //number of block in used and free allocated heap
size_t _num_allocated_bytes(); // number of bytes in used and allocated heap
size_t _num_meta_data_bytes(); // overall number of space consumed by metadata
size_t _size_meta_data(); //size of metadata for each block


#endif