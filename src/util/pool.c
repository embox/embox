/**
 * @file
 * @brief Object pool allocator implementation.
 *
 * @date 27.11.2010
 * @author Alexandr Kalmuk
 * @author Kirill Tyushev
 * @author Dmitry Zubarevich
 * @author Eldar Abusalimov
 *          - Rewrite lazy pool initialization to be O(1)
 */

#include <assert.h>

#include <util/pool.h>
#include <lib/list.h>


static struct pool *pool_init(struct pool *pool) {
	struct __pool_free_block *block;

	assert(pool);

	block = (struct __pool_free_block *) pool->storage;

	INIT_LIST_HEAD(&pool->free_blocks);
	INIT_LIST_HEAD(&block->link);
	list_add(&block->link, &pool->free_blocks);

	block->bytes_free = pool->objects_free * pool->object_sz;

	return pool;
}

static inline void check_init(struct pool *pool) {
	// TODO making an assumption about list internals. -- Eldar
	if (!pool->free_blocks.next) {
		pool_init(pool);
	}
}

void *pool_alloc(struct pool *pool) {
	struct __pool_free_block *block;
	size_t bytes_left;

	assert(pool);

	check_init(pool);

	if (list_empty(&pool->free_blocks)) {
		return NULL;
	}

	block = list_entry(pool->free_blocks.next, struct __pool_free_block, link);

	if ((bytes_left = block->bytes_free - pool->object_sz)) {
		block->bytes_free = bytes_left;
	} else {
		list_del(&block->link);
	}

	--pool->objects_free;

	return (char *) block + bytes_left;
}

void pool_free(struct pool *pool, void *object) {
	struct __pool_free_block *block;

	assert(pool);
	assert(object);

	block = (struct __pool_free_block *) object;

	list_add(&block->link, &pool->free_blocks);
	block->bytes_free = pool->object_sz;

	++pool->objects_free;
}

