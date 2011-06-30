/**
 * @brief Implementation of memory allocation API.
 *        It allocates object have some size in pool.
 *
 * @date   30.06.11
 * @author Alexandr Kalmuk
 */

#include <util/pool.h>
#include <kernel/mm/objalloc.h>

void *objalloc(struct objalloc *uni) {
	return pool_alloc((struct pool*) uni);
}

void objfree(struct objalloc *uni, void* objp) {
	pool_free((struct pool*) uni, objp);
}

void objcache_destroy(struct objalloc *uni) {

}
