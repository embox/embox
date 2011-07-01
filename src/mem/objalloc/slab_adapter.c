/**
 * @brief Implementation of memory allocation API.
 *        It allocates object have some size in cache using slab allocator
 *
 * @date   30.06.11
 * @author Alexandr Kalmuk
 */

#include <mem/slab.h>
#include <mem/objalloc.h>

void *objalloc(struct objalloc *adapter) {
	return cache_alloc(&adapter->cache);
}

void objfree(struct objalloc *adapter, void* objp) {
	cache_free(&adapter->cache, objp);
}

void objcache_destroy(struct objalloc *adapter) {
	cache_destroy(&adapter->cache);
}
