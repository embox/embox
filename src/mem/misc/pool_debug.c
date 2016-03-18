/**
 * @file
 * @brief It is debug version of fixed-size pool with fixed size objects
 *  We add a bitmap of free blocks in struct pool to catch double freeing.
 *
 * @see For more information see pool.c
 *
 * @date	04.03.2016
 * @author	Alex Kalmuk
 *			-- It is derived from pool.c with added double freeing catching
 */

#include <mem/misc/pool.h>

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <util/member.h>

void * pool_alloc(struct pool *pl) {
	void *obj;
	size_t index;

	assert(pl != NULL);

	if (!slist_empty(&pl->free_blocks)) {
		obj = (void *)slist_remove_first_link(&pl->free_blocks);

		index = (obj - pl->memory) / pl->obj_size;
		assert(bitmap_test_bit(pl->blocks, index) == 0);
		bitmap_set_bit(pl->blocks, index);

		return obj;
	}

	if (pl->bound_free != pl->memory + pl->pool_size) {
		obj = pl->bound_free;
		pl->bound_free += pl->obj_size;
		assert(pl->bound_free <= pl->memory + pl->pool_size);

		index = (obj - pl->memory) / pl->obj_size;
		assert(bitmap_test_bit(pl->blocks, index) == 0);
		bitmap_set_bit(pl->blocks, index);

		return obj;
	}

	return NULL;
}

void pool_free(struct pool *pl, void *obj) {
	size_t index;

	assert(pl != NULL);
	assert(obj != NULL);
	assert(pool_belong(pl, obj));

	index = (obj - pl->memory) / pl->obj_size;
	assert(bitmap_test_bit(pl->blocks, index) == 1);
	bitmap_clear_bit(pl->blocks, index);

	obj = slist_link_init((struct slist_link *)obj);
	slist_add_first_link(obj, &pl->free_blocks);
}

int pool_belong(const struct pool *pl, const void *obj) {
	return (pl->memory <= obj)
			&& (obj + pl->obj_size <= pl->memory + pl->pool_size)
			&& ((obj - pl->memory) % pl->obj_size == 0);
}
