/**
 * @file
 * @brief	Fixed-size pool with fixed size objects
 * @details This implementation base on single list structure.
 *     When object is being allocated it first of all try to find one in the
 *     single list or just increasing pointer to free space in the pool.
 *     When freeing object happens the object just added to the head of the list.
 *
 * @date	17.11.11
 * @author	Gleb Efimov
 *
 */

#include <mem/misc/pool.h>

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

void *pool_alloc(struct pool* pool) {
	void * addr;

	assert(pool);

	if (!slist_empty(&pool->free_blocks)) {
		return (void *)slist_remove_first_link(&pool->free_blocks);
	}

	if((size_t)pool->bound_free != ((size_t)pool->memory + pool->pool_size)) {
		addr = (void *)pool->bound_free;
		pool->bound_free += pool->obj_size;
		return addr;
	}

	return NULL;
}

void pool_free(struct pool* pool, void* obj) {
	assert(pool);

	assert(obj);

	obj = slist_link_init((struct slist_link *)obj);
	slist_add_first_link(obj , &pool->free_blocks);
}

int pool_belong(struct pool* pool, void* obj) {

	uint32_t pool_end = (uint32_t)pool->memory + pool->obj_size * pool->pool_size;

	if(((uint32_t)pool->memory <= (uint32_t)obj) &&
			((uint32_t)obj < pool_end)) {
				return 1;
			}
	return 0;
}
