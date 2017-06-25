/**
 * @file
 * @brief Slab allocator
 * @details
 *              Divide up memory into pools of objects, where each pool
 *              contains objects of the same size, and
 *              different pools contain objects of other sizes.
 *
 *              Pools are named by slabs.
 *
 *              Cache cantains list of pools.
 *
 *              Slabs that contains objects of the same size adds to one cache.
 *
 *              Slabs that contains objects of the different size adds to different caches.
 *
 *              Cache descriptors alloc in kmalloc_cache and add to list of caches.
 *
 * @date 14.12.10
 * @author Dmitry Zubarvich
 * @author Kirill Tyushev
 */

/**
 *      kmalloc and kfree
 *
 *      When someone requests a block of memory of a given size,
 *      find the first pool large enough to hold a
 *      block that size and hand it over.
 */

#ifndef MEM_MISC_SLAB_H_
#define MEM_MISC_SLAB_H_

#include <module/embox/mem/slab.h>
#include <stdbool.h>

/** cache descriptor */
typedef struct cache cache_t;

/**
 * TODO
 */
#define CACHE_DEF(cache_nm, object_t, objects_nr) \
          __CACHE_DEF(cache_nm, object_t, objects_nr)

/**
 * Create of cache
 * @param name of cache
 * @param obj_size is size of contained objects
 * @param obj_num is number of object reserved at first
 */
extern cache_t *cache_create(char *name, size_t obj_size, size_t obj_num);

extern int cache_init(cache_t *cache, size_t obj_size, size_t obj_num);

/**
 * Destroy of cache
 * @param cache_ptr is pointer to cache which must be deleted
 */
extern int cache_destroy(cache_t *cache_ptr);

/**
 * Return pointer to object for which allocate memory
 * @param cachep is pointer to cache which must contain object of this type
 */
extern void *cache_alloc(cache_t *cachep);

/**
 * Free memory from something object
 * @param cachep is pointer to cache which must contain object of this type
 * @param objp is object which must be deleted
 */
extern void cache_free(cache_t *cachep, void* objp);

/**
 * Remove all free slabs from cache
 * will be used in feature
 * @param cachep is pointer to cache which need to shrink
 * @return number of removed slabs
 */
extern int cache_shrink(cache_t *cachep);

/**
 * Enable/disable cache growing. That means if growing is on, than
 * cache will allocate slabs when no memory. And allocation will be return NULL
 * if no memory and cache is not growing.
 * By default growing is on.
 * */
static inline void cache_growing_on(cache_t *cache) {
	cache->growing = true;
}

static inline void cache_growing_off(cache_t *cache) {
	cache->growing = false;
}

#endif /* MEM_MISC_SLAB_H_ */
