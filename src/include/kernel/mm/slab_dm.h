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

/* Length of name of any cache */
#define CACHE_NAMELEN 16
/* max slab size in 2^n form */
#define MAX_GFP_ORDER 3
/* max object size in 2^n form */
#define MAX_OBJ_ORDER 3
/* number for defining acceptable internal fragmentation */
#define MAX_INTFRAGM_ORDER 8
/* size of kmalloc_cache in pages */
#define KMALLOC_CACHE_SIZE 1

/* Standard align of something */
#define ALIGN_UP(size, align) \
     (((size) + (align) - 1) & (~((align) - 1)))

/* Align of something by cache size */
#define CACHE_ALIGN(size) \
			ALIGN_UP(size, 4)

/**
 * cache descriptor
 */
typedef struct kmem_cache {
	/* pointer to other caches */
	struct list_head next;
	char name[CACHE_NAMELEN];
	struct list_head slabs_full;
	struct list_head slabs_partial;
	struct list_head slabs_free;
	size_t obj_size;
	/* the number of objects stored on each slab */
	unsigned int num;
	/* The base 2 logarithm (2^n) number of pages to allocate for each slab */
	unsigned int gfporder;
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
 * Remove all free slabs from cache
 * will be used in feature
 * @param cachep is pointer to cache which need to shrink
 * @return number of removed slabs
 */
int kmem_dmcache_shrink(kmem_cache_t *cachep);

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

/**
 * Return list of free and busy blocks per current slab
 * @param list of free and busy blocks(for objects)
 * @param slabp is pointer to slab, in witch blocks are
 */
extern void sget_blocks_info(struct list_head* list, struct list_head *slabp);

/**
 *
 * @param list is list_head struct:
 * it is'nt a doubly linked list
 * _____________
 *|pseudo_list  |
 *|_____________|____________prev_____________
 *     |next                                 |
 * ____|________       ______               _|____
 *|kmalloc_cache|<--->|cache1|<--->...<--->|cacheN|
 *|_____________|     |______|             |______|
 */
extern void make_caches_list(struct list_head* list);

#endif /* SLAB_DM_H */
