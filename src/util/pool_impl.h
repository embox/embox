/**
 * @file
 * @brief Internal implementation of object pool allocator.
 *
 * @date 07.03.2011
 * @author Kirill Tyushev
 */

#ifndef UTIL_POOL_IMPL_H_
#define UTIL_POOL_IMPL_H_

#include <util/binalign.h>
#include <lib/list.h>

/** cache descriptor */
struct static_cache {
	/** pointer to pool */
	char* cache_begin;
	/** object size */
	size_t size;
	/** the number of objects stored on each slab */
	unsigned int num;
	/** the list of free objects in pool */
	struct list_head obj_ptr;
	/** for initialization */
	int hasinit;
};

#define __POOL_DEF(type, name, sz) \
  static char __##name##_pool[(sz) * binalign_bound(sizeof(type), sizeof(struct list_head))]; \
  static static_cache_t name = { \
        .num = (sz), \
        .size = binalign_bound(sizeof(type), sizeof(struct list_head)), \
        .cache_begin = __##name##_pool, \
        .obj_ptr = {NULL, NULL}, \
        .hasinit = 0 }

#endif /* UTIL_POOL_IMPL_H_ */
