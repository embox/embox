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
	if(!slist_empty(pool->free_blocks)) {

	}
	if (pool->current_size != pool->pool_size) {
		if (0 != slist_empty(&pool->free_blocks)) {
			pool->current_size++;
			return (void *)slist_remove_first_link(&pool->free_blocks);
		}
		return NULL;
	}
	return NULL;
}

void pool2_free(struct pool* pool, void* obj) {
}
