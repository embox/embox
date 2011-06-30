/**
 * @brief Implementation of memory allocation API.
 *        It allocates object have some size in cache using slab allocator
 *
 * @date   30.06.11
 * @author Alexandr Kalmuk
 */

#include <kernel/mm/slab.h>
#include <kernel/mm/objalloc.h>

void *objalloc(struct objalloc *uni) {
	return cache_alloc((cache_t*) uni);
}

void objfree(struct objalloc *uni, void* objp) {
	cache_free((cache_t*) uni, objp);
}

void objcache_destroy(struct objalloc *uni) {
	cache_destroy((cache_t*) uni);
}
