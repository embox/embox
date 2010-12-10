/**
 * @file
 * @brief slab allocator header
 */

#ifndef SLAB_H
#define SLAB_H

/* slabs sizes*/
#define SLAB_SIZE1 0x100
#define SLAB_SIZE2 0x100
#define SLAB_SIZE3 0x100

/* objects sizes allocating in corresponding slabs */
#define OBJECT_SIZE1 0x8
#define OBJECT_SIZE2 0x8
#define OBJECT_SIZE3 0x8

/* max length of cache name */
#define CACHE_NAMELEN 0x10

/**
 * cache descriptor
 */
typedef struct kmem_cache {
	void* cache_begin;
	// object size
	size_t size;
	// the number of objects stored on each slab
	unsigned int num;
	char name[CACHE_NAMELEN];
	int hasinit;
} kmem_cache_t;

/**
 * slab descriptor
 */
typedef struct slab_s {
	// pointer to where the objects start
	struct list_head obj_ptr;
} slab_t;

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
void  kmem_cache_free(kmem_cache_t* cachep, void* objp);

/**
 * return the cache descriptor
 * @param cache_name of returning cache
 */
kmem_cache_t* get_cache(char* cache_name);

#endif /* SLAB_H_ */
