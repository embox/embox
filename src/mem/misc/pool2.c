/**
 * @file
 * @brief	object pool allocator implementation
 *
 * @date	17.11.11
 * @author	Gleb Efimov
 *
 */

#include <types.h>
#include <mem/misc/pool2.h>

void *pool2_alloc(struct pool* pool) {
	assert(pool);
	if (pool->bound_free != pool->pool_size) {
		if (!slist_empty(&pool->free_blocks)) {
			++pool->bound_free;
			return (void *)slist_remove_first_link(&pool->free_blocks);
		}
		 ++pool->bound_free;
		return (void *)pool->memory + (pool->bound_free - 1);
	}
	return NULL;
}

void pool2_free(struct pool* pool, void* obj) {
	assert(pool);
	assert(obj);
}
