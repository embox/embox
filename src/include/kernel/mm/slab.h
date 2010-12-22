/**
 * @file
 * @brief slab allocator header
 */

#ifndef SLAB_H
#define SLAB_H

#include <lib/list.h>

/**
 * cache descriptor
 */
typedef struct kmem_cache {
	void* cache_begin;
	// object size
	size_t size;
	// the number of objects stored on each slab
	unsigned int num;
	struct list_head obj_ptr;
	int hasinit;
} kmem_cache_t;

#define ALIGN_UP(size, align) \
     (((size) + (align) - 1) & (~((align) - 1)))

#define ADD_CACHE(name,type,count) \
  static char __name_pool[count * ALIGN_UP(sizeof(type), sizeof(struct list_head))]; \
  static kmem_cache_t name = { \
        .num = count, \
        .size = ALIGN_UP(sizeof(type), sizeof(struct list_head)), \
        .cache_begin = __name_pool, \
        .obj_ptr = {NULL, NULL}, \
        .hasinit = 0 };

/**
 * allocate single object from the cache and return it to the caller
 * @param cache corresponding to allocating object
 * @return the address of allocated object
 */
void* kmem_cache_alloc(kmem_cache_t* cachep);

/**
 * free an object and return it to the cache
 * @param cachep corresponding to freeing object
 * @param objp which will be free
 */
void kmem_cache_free(kmem_cache_t* cachep, void* objp);

#endif /* SLAB_H_ */
