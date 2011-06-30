/**
 * @brief Implementation of memory allocation API.
 *        It allocates object have some size in pool.
 *
 * @date   30.06.11
 * @author Alexandr Kalmuk
 */

#include <util/pool.h>
#include <kernel/mm/objalloc.h>

void *objalloc(struct objalloc *adapter) {
	return pool_alloc(&adapter->pool);
}

void objfree(struct objalloc *adapter, void* objp) {
	pool_free(&adapter->pool, objp);
}

void objcache_destroy(struct objalloc *adapter) {

}
