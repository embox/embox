/**
 * @brief Implementation of memory allocation API.
 *        It allocates object have some size in cache using slab allocator
 *
 * @date   30.06.11
 * @author Alexandr Kalmuk
 */

#include <mem/misc/slab.h>
#include <mem/objalloc.h>

void *objalloc(objalloc_t *allocator) {
	return cache_alloc(allocator);
}

void objfree(objalloc_t *allocator, void *object) {
	cache_free(allocator, object);
}

void objcache_destroy(objalloc_t *allocator) {
	cache_destroy(allocator);
}
