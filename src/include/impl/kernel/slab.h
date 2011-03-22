/**
 * @file
 * @brief Defines constants for slab allocator
 *
 * @date 26.02.2011
 * @author Alexandr Kalmuk
 */

#ifndef SLAB_H_
# error "Do not include this file directly, use <kernel/mm/slab.h> instead!"
#endif /* SLAB_H_ */

#include <lib/list.h>

/** Length of name of any cache */
#define CACHE_NAMELEN 16
/** max slab size in 2^n form */
#define MAX_SLAB_ORDER 3
/** max object size in 2^n form */
#define MAX_OBJ_ORDER 3
/** number for defining acceptable internal fragmentation */
#define MAX_INT_FRAGM_ORDER 8
/** size of kmalloc_cache in pages */
#define CACHE_CHAIN_SIZE 1

/** cache descriptor */
struct cache {
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
	unsigned int slab_order;
	/** lock the cache from being reaped or shrunk */
	bool growing;
};
