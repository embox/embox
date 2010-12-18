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

/* Length of name of any cache */
#define CHACHE_NEMELEN 16
/* Count of objects which can be contained into the slab */
#define NUM_OF_OBJECTS_IN_SLAB 10
/* Standard align of something */
#define ALIGN_UP(size, align) \
     (((size) + (align) - 1) & (~((align) - 1)))

/**
 * cache descriptor
 */
typedef struct kmem_cache {
	/* pointer to other caches */
	struct list_head next;
	char name[CHACHE_NEMELEN];
	struct list_head slabs_full;
	struct list_head slabs_partial;
	struct list_head slabs_free;
	size_t obj_size;
	/* the number of objects stored on each slab */
	unsigned int num;
	/* size of each slab in pages */
	size_t slab_size;
	/* lock the cache from being reaped or shrunk */
	bool growing;
} kmem_cache_t;

/**
 * Create of cache
 * @param name of cache
 * @param obj_size is size of contained objects
 */
kmem_cache_t *kmem_dmcache_create(char *name, size_t obj_size);

/**
 * Destroy of cache
 * @param cache_ptr is pointer to cache which must be deleted
 */
void kmem_dmcache_destroy(kmem_cache_t *cache_ptr);

/**
 * Return pointer to object for which allocate memory
 * @param cachep is pointer to cache which must contain object of this type
 */
void *kmem_dmcache_alloc(kmem_cache_t *cachep);

/**
 * Free memory from something object
 * @param cachep is pointer to cache which must contain object of this type
 * @param objp is object which must be deleted
 */
void kmem_dmcache_free(kmem_cache_t *cachep, void* objp);

/**
 * Return pointer to object for which allocate memory
 * @param size of object
 */
void *smalloc(size_t size);

/**
 * Free memory from something object
 * @param obj is object which must be deleted
 */
void sfree(void* obj);

#endif
