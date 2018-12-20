#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "mem_alloc_standard_pool.h"

static int __mem_alloc_init_flag = 0;

/* Note: the other fields will be setup by the init procedure */
static mem_pool_t standard_pool_1025_and_above = {
    .pool_id = 3, .pool_name = "pool-3-std (1024_Max)", .pool_size = 1024, .min_request_size = 1, .max_request_size = SIZE_MAX, .pool_type = STANDARD_POOL};

void *malloc(size_t size)
{
	void *res;
	if (!__mem_alloc_init_flag) {
		__mem_alloc_init_flag = 1;
		// init_bootstrap_buffers();
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
		// init_bootstrap_buffers();
		init_standard_pool(&standard_pool_1025_and_above, standard_pool_1025_and_above.pool_size, standard_pool_1025_and_above.min_request_size,
		                   standard_pool_1025_and_above.max_request_size);
	}

	res = mem_alloc_standard_pool(&standard_pool_1025_and_above, size * nmemb);
	if (res != NULL) {
		// explicit_bzero(res, size); // FIXME
	}
	return res;
}