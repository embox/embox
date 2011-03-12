/**
 * @file
 * @brief Internal implementation of static slab allocator
 *
 * @date 07.03.2011
 * @author Kirill Tyushev
 */

#ifndef SLAB_STATIC_H_
# error "Do not include this file directly, use <kernel/mm/slab_static.h> instead!"
#endif /* SLAB_STATIC_H_ */

#include <util/binalign.h>

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

/** create cache */
#define __STATIC_CACHE_CREATE(name, type, count) \
  static char __name_pool[count * binalign_bound(sizeof(type), sizeof(struct list_head))]; \
  static static_cache_t name = { \
        .num = count, \
        .size = binalign_bound(sizeof(type), sizeof(struct list_head)), \
        .cache_begin = __name_pool, \
        .obj_ptr = {NULL, NULL}, \
        .hasinit = 0 }
