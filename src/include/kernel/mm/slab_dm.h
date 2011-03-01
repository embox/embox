/**
 * @file
 * @brief Dynamic slab allocator
 *
 * @date 14.12.2010
 * @author Dmitry Zubarvich
 * @author Kirill Tyushev
 */

#ifndef SLAB_DM_H
#define SLAB_DM_H

#include <lib/list.h>
#include <impl/kernel/slab_config.h>

/**
 * cache descriptor
 */
typedef struct kmem_cache {
	/** pointer to other caches */
	struct list_head next;
	/** name of cache*/
	char name[CACHE_NAMELEN];
	/** list of busy slabs */
	struct list_head slabs_full;
	/** list of partial busy slabs */
	struct list_head slabs_partial;
	/** list of free slabs */
	struct list_head slabs_free;
	/** size of object in cache */
	size_t obj_size;
	/** the number of objects stored on each slab */
	unsigned int num;
	/** The base 2 logarithm (2^n) number of pages to allocate for each slab */
	unsigned int gfporder;
	/** lock the cache from being reaped or shrunk */
	bool growing;
} kmem_cache_t;

/**
 * Create of cache
 * @param name of cache
 * @param obj_size is size of contained objects
 */
extern kmem_cache_t *kmem_dmcache_create(char *name, size_t obj_size);

/**
 * Destroy of cache
 * @param cache_ptr is pointer to cache which must be deleted
 */
extern void kmem_dmcache_destroy(kmem_cache_t *cache_ptr);

/**
 * Return pointer to object for which allocate memory
 * @param cachep is pointer to cache which must contain object of this type
 */
extern void *kmem_dmcache_alloc(kmem_cache_t *cachep);

/**
 * Free memory from something object
 * @param cachep is pointer to cache which must contain object of this type
 * @param objp is object which must be deleted
 */
extern void kmem_dmcache_free(kmem_cache_t *cachep, void* objp);

/**
 * Remove all free slabs from cache
 * will be used in feature
 * @param cachep is pointer to cache which need to shrink
 * @return number of removed slabs
 */
extern int kmem_dmcache_shrink(kmem_cache_t *cachep);

/**
 * Return pointer to object for which allocate memory
 * @param size of object
 */
extern void *smalloc(size_t size);

/**
 * Free memory from something object
 * @param obj is object which must be deleted
 */
extern void sfree(void* obj);

#endif /* SLAB_DM_H */
