#ifndef _MEM_ALLOC_H_
#define _MEM_ALLOC_H_

#include <stdlib.h>

/* Allocator functions, to be implemented in mem_alloc.c */
void memory_init(void);
void *memory_alloc(size_t size);
void memory_free(void *p);
size_t memory_get_allocated_block_size(void *addr);

/* Function called upon process termination */
void run_at_exit(void);

#endif
