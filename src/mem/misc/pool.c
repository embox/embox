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
#include <stddef.h>
#include <stdint.h>
#include <hal/ipl.h>
#include <kernel/spinlock.h>
#include <util/member.h>

/* One lock for every pool in the image, rather than one per pool: struct
 * pool is laid down by POOL_DEF() in read-only initialised data all over the
 * tree, and giving each its own lock would change that layout everywhere.
 * Pool operations are short and rare enough that the contention is not
 * measurable. */
static spinlock_t pool_lock = SPIN_STATIC_UNLOCKED;

void * pool_alloc(struct pool *pl) {
	void *obj = NULL;
	ipl_t ipl;

	assert(pl != NULL);

	ipl = ipl_save();
	__spin_lock(&pool_lock);
	{
		if (!slist_empty(&pl->free_blocks)) {
			obj = (void *)slist_remove_first_link(&pl->free_blocks);
		}
		else if (pl->bound_free != pl->memory + pl->pool_size) {
			obj = pl->bound_free;
			pl->bound_free += pl->obj_size;
			assert(pl->bound_free <= pl->memory + pl->pool_size);
		}
	}
	__spin_unlock(&pool_lock);
	ipl_restore(ipl);

	return obj;
}

void pool_free(struct pool *pl, void *obj) {
	ipl_t ipl;

	assert(pl != NULL);
	assert(obj != NULL);
	assert(pool_belong(pl, obj));

	ipl = ipl_save();
	__spin_lock(&pool_lock);
	{
		obj = slist_link_init((struct slist_link *)obj);
		slist_add_first_link(obj, &pl->free_blocks);
	}
	__spin_unlock(&pool_lock);
	ipl_restore(ipl);
}

int pool_belong(const struct pool *pl, const void *obj) {
	return (pl->memory <= obj)
			&& (obj + pl->obj_size <= pl->memory + pl->pool_size)
			&& ((obj - pl->memory) % pl->obj_size == 0);
}
