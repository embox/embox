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
#include <util/member.h>

void * pool_alloc(struct pool *pl) {
	void *obj;

	assert(pl != NULL);

	if (!slist_empty(&pl->free_blocks)) {
		return (void *)slist_remove_first_link(&pl->free_blocks);
	}

	if (pl->bound_free != pl->memory + pl->pool_size) {
		obj = pl->bound_free;
		pl->bound_free += pl->obj_size;
		assert(pl->bound_free <= pl->memory + pl->pool_size);
		return obj;
	}

	return NULL;
}

void pool_free(struct pool *pl, void *obj) {
	assert(pl != NULL);
	assert(obj != NULL);
	assert(pool_belong(pl, obj));

	obj = slist_link_init((struct slist_link *)obj);
	slist_add_first_link(obj, &pl->free_blocks);
}

int pool_belong(const struct pool *pl, const void *obj) {
	return (pl->memory <= obj)
			&& (obj + pl->obj_size <= pl->memory + pl->pool_size)
			&& ((obj - pl->memory) % pl->obj_size == 0);
}
