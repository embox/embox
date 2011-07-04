/**
 * @file
 * @brief Defines constants for slab allocator
 *
 * @date 26.02.11
 * @author Alexandr Kalmuk
 */

#ifndef MEM_MISC_SLAB_IMPL_H_
#define MEM_MISC_SLAB_IMPL_H_

#include <types.h>
#include <lib/list.h>

#define __CACHE_DEF(cache_nm, object_t, objects_nr) \
	      static struct cache* cache_nm;\
	         cache_init(cache_nm, object_t, objects_nr)

/** Length of name of any cache */
#define __CACHE_NAMELEN 16

/** cache descriptor */
struct cache {
	/** pointer to other caches */
	struct list_head next;
	/** name of cache*/
	char name[__CACHE_NAMELEN ];
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

#endif /* MEM_MISC_SLAB_IMPL_H_ */
