#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "mem_alloc_standard_pool.h"
#include "syscall.h"

/* Returns 1 if the block is used, or 0 if the block is free */
int is_block_used(mem_standard_block_header_footer_t *m)
{
	return (((m->flag_and_size) >> 31) & 1UL);
}

/* Returns 1 the if block is free, or 1 if the block is free */
int is_block_free(mem_standard_block_header_footer_t *m)
{
	return (is_block_used(m) == 0);
}

/* Modifies a block header (or footer) to mark it as used */
void set_block_used(mem_standard_block_header_footer_t *m)
{
	m->flag_and_size = ((m->flag_and_size) | (1UL << 31));
}

/* Modifies a block header (or footer) to mark it as free */
void set_block_free(mem_standard_block_header_footer_t *m)
{
	m->flag_and_size = ((m->flag_and_size) & ~(1UL << 31));
}

/* Returns the size of a block (as stored in the header/footer) */
size_t get_block_size(mem_standard_block_header_footer_t *m)
{
	uint64_t res = ((m->flag_and_size) & ~(1UL << 31));
	return (size_t)res;
}

/* Modifies a block header (or footer) to update the size of the block */
void set_block_size(mem_standard_block_header_footer_t *m, size_t size)
{
	uint64_t s = (uint64_t)size;
	uint64_t flag = (m->flag_and_size) & (1UL << 31);
	m->flag_and_size = flag | s;
}

// FIXME first_free could be used instead, but then continuing the search at the beginning would require backtracking
// by repeatedly accessing free_block->prev which has a bad complexity (especially if there are a lot of small free blocks)
// NOTE: This is not multi-standard-pool safe as it's a global. It should be put in the pool structure to make it safe.
void *next_lookup = NULL;

const int MEM_ALIGNMENT = 4;
const size_t PADDED_HEADER_SIZE = sizeof(mem_standard_block_header_footer_t);

size_t align_size(size_t size)
{
	if (size % MEM_ALIGNMENT)
		return size + MEM_ALIGNMENT - size % MEM_ALIGNMENT;
	else
		return size;
}

void *align_addr(void *addr)
{
	if ((uintptr_t)addr % MEM_ALIGNMENT)
		return addr + MEM_ALIGNMENT - (uintptr_t)addr % MEM_ALIGNMENT;
	else
		return addr;
}

void init_standard_pool(mem_pool_t *p, size_t size, size_t min_request_size, size_t max_request_size)
{
	void *block = (void *)Sbrk(25);

	p->start = block;
	p->end = block + size;
	p->first_free = block;

	mem_standard_free_block_t *first_free = p->first_free;
	set_block_free(&first_free->header);
	set_block_size(&first_free->header, size - 2 * PADDED_HEADER_SIZE);

	first_free->prev = NULL;
	first_free->next = NULL;
}

void *mem_alloc_standard_pool(mem_pool_t *pool, size_t size)
{
	mem_standard_free_block_t *suitable_block;
	suitable_block = pool->first_free;

	size = align_size(size);

	size_t suitable_block_size;

	while (suitable_block && (suitable_block_size = get_block_size(&suitable_block->header)) < size)
		suitable_block = suitable_block->next;

	if (!suitable_block)
		return NULL;

	mem_standard_free_block_t *prev_block = suitable_block->prev;
	mem_standard_free_block_t *next_block = suitable_block->next;

	mem_standard_block_header_footer_t *header = &suitable_block->header;

	set_block_size(header, size);
	set_block_used(header);

	// Not enough room for a block with header + prec + succ + footer
	if (suitable_block_size - size < 2 * PADDED_HEADER_SIZE + 2 * sizeof(mem_standard_free_block_t *)) {

		if (prev_block)
			prev_block->next = next_block;
		else
			pool->first_free = next_block;

		if (next_block)
			next_block->prev = prev_block;

		size = suitable_block_size;

		mem_standard_block_header_footer_t *footer = (void *)suitable_block + PADDED_HEADER_SIZE + size;
		set_block_size(footer, size);
		set_block_used(footer);
	} else {
		mem_standard_block_header_footer_t *left_over_block_header = (void *)suitable_block + PADDED_HEADER_SIZE + size + PADDED_HEADER_SIZE;

		size_t left_over_size = suitable_block_size - size - 2 * PADDED_HEADER_SIZE;
		mem_standard_block_header_footer_t *left_over_block_footer = (void *)left_over_block_header + PADDED_HEADER_SIZE + left_over_size;

		if (prev_block)
			prev_block->next = (mem_standard_free_block_t *)left_over_block_header;
		else
			pool->first_free = (mem_standard_free_block_t *)left_over_block_header;

		if (next_block)
			next_block->prev = (mem_standard_free_block_t *)left_over_block_header;

		mem_standard_free_block_t *left_over_block_free = (mem_standard_free_block_t *)left_over_block_header;
		left_over_block_free->prev = prev_block;
		left_over_block_free->next = next_block;

		set_block_size(left_over_block_header, left_over_size);
		set_block_size(left_over_block_footer, left_over_size);

		set_block_free(left_over_block_header);
		set_block_free(left_over_block_footer);

		mem_standard_block_header_footer_t *footer = (void *)suitable_block + PADDED_HEADER_SIZE + size;
		set_block_size(footer, size);
		set_block_used(footer);
	}

	return (void *)suitable_block + PADDED_HEADER_SIZE;
}

int is_block_from_standard_pool(mem_pool_t *pool, void *b)
{
	return pool->start <= b && b <= pool->end;
}

int is_block_size_matching(mem_pool_t *pool, void *b)
{
	mem_standard_block_header_footer_t *header = b;
	size_t size1 = get_block_size(header);

	mem_standard_block_header_footer_t *footer = b + PADDED_HEADER_SIZE + size1;
	size_t size2 = get_block_size(footer);

	return size1 == size2;
}

int is_block_probably_used(mem_pool_t *pool, void *b)
{
	mem_standard_block_header_footer_t *header = b;
	size_t size = get_block_size(header);
	mem_standard_block_header_footer_t *footer = b + PADDED_HEADER_SIZE + size;

	return is_block_used(header) && is_block_used(footer);
}

void mem_free_standard_pool(mem_pool_t *pool, void *addr)
{
	mem_standard_free_block_t *addr_free = (void *)addr - PADDED_HEADER_SIZE;

	// Security: out-of-pool free, fraction of allocated zone, double-free
	if (!is_block_from_standard_pool(pool, addr_free) || !is_block_size_matching(pool, addr_free) || !is_block_probably_used(pool, addr_free)) {
		puts("ERROR: no block to be freed\n");
		exit(-1);
	}

	size_t block_size = get_block_size(&addr_free->header);
	void *block_footer = (void *)addr_free + PADDED_HEADER_SIZE + block_size;

	void *next_header = (void *)block_footer + PADDED_HEADER_SIZE;

	void *prev_footer = (void *)addr_free - PADDED_HEADER_SIZE;

	mem_standard_free_block_t *free_block_before = pool->first_free;
	mem_standard_free_block_t *free_block_next = NULL;

	if (pool->first_free > (void *)addr_free) {
		free_block_before = NULL;
		free_block_next = pool->first_free;
	} else if (free_block_before) {
		while ((free_block_next = free_block_before->next) && (void *)free_block_next < addr)
			free_block_before = free_block_next;
	}

	addr_free->prev = free_block_before;
	addr_free->next = free_block_next;

	if (free_block_next) {
		if (next_header == free_block_next) // Coalescing
		{
			free_block_next = free_block_next->next;
			addr_free->next = free_block_next;
			block_size += get_block_size(next_header) + 2 * PADDED_HEADER_SIZE;
		}
	}

	if (free_block_before) {
		free_block_before->next = addr_free;

		void *free_block_before_footer = (void *)free_block_before + PADDED_HEADER_SIZE + get_block_size(&free_block_before->header);
		if (free_block_before_footer == prev_footer) // Coalescing
		{
			free_block_before->next = addr_free->next;
			addr_free = free_block_before;
			block_size += get_block_size(&free_block_before->header) + 2 * PADDED_HEADER_SIZE;
		}
	} else
		pool->first_free = addr_free;

	if (free_block_next)
		free_block_next->prev = addr_free;

	block_footer = (void *)addr_free + PADDED_HEADER_SIZE + block_size;

	set_block_free(&addr_free->header);
	set_block_free(block_footer);

	set_block_size(&addr_free->header, block_size);
	set_block_size(block_footer, block_size);
}

size_t mem_get_allocated_block_size_standard_pool(mem_pool_t *pool, void *addr)
{
	return get_block_size((mem_standard_block_header_footer_t *)(addr - PADDED_HEADER_SIZE));
}

int at_exit_standard_pool(mem_pool_t *pool)
{
	mem_standard_free_block_t *first_free = pool->first_free;

	if (first_free != pool->start || get_block_size(&first_free->header) != (void *)pool->end - (void *)pool->start - 2 * PADDED_HEADER_SIZE)
		return 0;

	return 1;
}
