/**
 * @file
 * @brief Object pool allocator.
 *
 * @author Kirill Tyushev
 * @author Eldar Abusalimov
 */

#ifndef INCLUDE_MEM_MISC_UTIL_POOL_H_
#define INCLUDE_MEM_MISC_UTIL_POOL_H_

#include __impl_x(mem/misc/pool_impl.h)

struct pool;

/**
 * create cache
 * @param name of cache
 * @param type of objects in cache
 * @param count of objects in cache
 */
#define POOL_DEF(pool_nm, object_t, objects_nr) \
	  __POOL_DEF(pool_nm, object_t, objects_nr)

//extern int pool_init(struct pool *pool, size_t obj_size, size_t obj_num);

/**
 * allocate single object from the cache and return it to the caller
 * @param cache corresponding to allocating object
 * @return the address of allocated object or NULL if pool is full
 */
extern void *pool_alloc(struct pool *pool);

/**
 * free an object and return it to the cache
 * @param cachep corresponding to freeing object
 * @param objp which will be free
 */
extern void pool_free(struct pool *pool, void *object);

#endif /* INCLUDE_MEM_MISC_UTIL_POOL_H_ */
