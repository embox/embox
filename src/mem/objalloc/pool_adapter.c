/**
 * @brief Implementation of memory allocation API.
 *        It allocates object have some size in pool.
 *
 * @date   30.06.11
 * @author Alexandr Kalmuk
 */

#include <mem/objalloc.h>
#include <mem/misc/pool.h>

void *objalloc(objalloc_t *allocator) {
	return pool_alloc(allocator);
}

void objfree(objalloc_t *allocator, void *object) {
	pool_free(allocator, object);
}

