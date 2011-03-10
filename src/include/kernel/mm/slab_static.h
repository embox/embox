/**
 * @file
 * @brief Static slab allocator
 *
 * @details
 *		Divide up memory into pools of objects, where each pool
 *		contains objects of the same size, and
 *		different pools contain objects of other sizes.
 */

#ifndef SLAB_STATIC_H_
#define SLAB_STATIC_H_

#include <lib/list.h>
#include <impl/kernel/slab_static.h>

/** cache descriptor */
typedef struct static_cache static_cache_t;

/**
 * create cache
 * @param name of cache
 * @param type of objects in cache
 * @param count of objects in cache
 */
#define STATIC_CACHE_CREATE(name, type, count) \
	__STATIC_CACHE_CREATE(name, type, count)

/**
 * allocate single object from the cache and return it to the caller
 * @param cache corresponding to allocating object
 * @return the address of allocated object or NULL if pool is full
 */
extern void* static_cache_alloc(static_cache_t* cachep);

/**
 * free an object and return it to the cache
 * @param cachep corresponding to freeing object
 * @param objp which will be free
 */
extern void static_cache_free(static_cache_t* cachep, void* objp);

#endif /* SLAB_STATIC_H_ */
