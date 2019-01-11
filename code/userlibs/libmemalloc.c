#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "mem_alloc_standard_pool.h"
#include "syscall.h"

static int __mem_alloc_init_flag = 0;

/* Note: the other fields will be setup by the init procedure */
static mem_pool_t standard_pool_1025_and_above = {
    .pool_id = 3, .pool_name = "pool-3-std (1024_Max)", .pool_size = 1024, .min_request_size = 1, .max_request_size = SIZE_MAX, .pool_type = STANDARD_POOL};

void *malloc(size_t size)
{
	void *res;

	if (!__mem_alloc_init_flag) {
		__mem_alloc_init_flag = 1;

		init_standard_pool(&standard_pool_1025_and_above, standard_pool_1025_and_above.pool_size, standard_pool_1025_and_above.min_request_size,
		                   standard_pool_1025_and_above.max_request_size);
	}

	res = mem_alloc_standard_pool(&standard_pool_1025_and_above, size);

	return res;
}

void free(void *p)
{
	if (p == NULL)
		return;

	mem_free_standard_pool(&standard_pool_1025_and_above, p);
}

void *calloc(size_t nmemb, size_t size)
{
	void *res;

	if (!__mem_alloc_init_flag) {
		__mem_alloc_init_flag = 1;

		init_standard_pool(&standard_pool_1025_and_above, standard_pool_1025_and_above.pool_size, standard_pool_1025_and_above.min_request_size,
		                   standard_pool_1025_and_above.max_request_size);
	}

	res = mem_alloc_standard_pool(&standard_pool_1025_and_above, size * nmemb);

	if (res != NULL) {
		memset(res, 0, size * nmemb);
	}

	return res;
}

// void *realloc(void *ptr, size_t size) // FIXME Make it more intelligent?
// {
// 	size_t oldSize = get_block_size(ptr - sizeof(mem_standard_block_header_footer_t));
// 	void* nptr = malloc(size);
// 	memcpy(nptr, ptr, oldSize);
// 	free(ptr);

// 	return nptr;
// }

void *memcpy(void *dest, const void *src, size_t n)
{
	char *destc = dest;
	const char *srcc = src;

	for (n -= 1; n >= 0; --n) {
		destc[n] = srcc[n];
	}

	return dest;
}
