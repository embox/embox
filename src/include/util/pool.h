/**
 * @file
 * @brief Object pool allocator.
 *
 * @author Kirill Tyushev
 * @author Eldar Abusalimov
 */

#ifndef UTIL_POOL_H_
#define UTIL_POOL_H_

#include __impl_x(util/pool_impl.h)

struct pool;

/**
 * create cache
 * @param name of cache
 * @param type of objects in cache
 * @param count of objects in cache
 */
#define POOL_DEF(pool_nm, object_t, objects_nr) \
	__POOL_DEF(pool_nm, object_t, objects_nr)

#if 0
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
#endif

extern void *pool_alloc(struct pool *pool);
extern void pool_free(struct pool *pool, void *object);

#endif /* UTIL_POOL_H_ */
