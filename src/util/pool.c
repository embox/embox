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

#if 0
/**
 * simple cache initialization
 * @param cache which will be initialize
 */
static void static_cache_init(static_cache_t* cache) {
	struct list_head* elem;

	INIT_LIST_HEAD(&(cache->obj_ptr));
	for (int i = 0; i < cache->num; i++) {
		elem = (struct list_head*) (cache->cache_begin + cache->size * i);

		/*add this free block in slab_list*/
		list_add_tail(elem, &(cache->obj_ptr));
	}
}

void* static_cache_alloc(static_cache_t* cachep) {
	void* objp;

	if (!(cachep->hasinit)) {
		static_cache_init(cachep);
		cachep->hasinit = 1;
	}

	if (list_empty(&(cachep->obj_ptr)))
		return NULL;

	objp = cachep->obj_ptr.next;
	list_del_init(cachep->obj_ptr.next);

	return objp;
}

void  static_cache_free(static_cache_t* cachep, void* objp) {
	struct list_head* ptr_begin;

	if (objp == NULL)
		return;

	ptr_begin = (struct list_head*)(objp);
	list_add(ptr_begin, &(cachep->obj_ptr));
}

#endif

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

