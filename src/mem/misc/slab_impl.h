/**
 * @file
 * @brief Implementation of cache structure.
 *
 * @date 26.02.11
 * @author Alexandr Kalmuk
 */

#ifndef MEM_MISC_SLAB_IMPL_H_
#define MEM_MISC_SLAB_IMPL_H_

#include <lib/libds/dlist.h>
#include <framework/mod/self.h>
#include <stddef.h>
#include <stdbool.h>

/** Length of name of any cache */
#define __CACHE_NAMELEN 16

/** max slab size in 2^n form */
#define MAX_SLAB_ORDER 3
/** max object size in 2^n form */
#define MAX_OBJ_ORDER 3
/** number for defining acceptable internal fragmentation */
#define MAX_INT_FRAGM_ORDER 8
/** size of kmalloc_cache in pages */
#define CACHE_CHAIN_SIZE 1
/** use to search a fit cache for object */
#define MAX_OBJECT_ALIGN 0

/** cache descriptor */
struct cache {
	/** pointer to other caches */
	struct dlist_head next;
	/** name of cache*/
	char name[__CACHE_NAMELEN ];
	/** list of busy slabs */
	struct dlist_head slabs_full;
	/** list of partial busy slabs */
	struct dlist_head slabs_partial;
	/** list of free slabs */
	struct dlist_head slabs_free;
	/** size of object in cache */
	size_t obj_size;
	/** the number of objects stored on each slab */
	unsigned int num;
	/** The base 2 logarithm (2^n) number of pages to allocate for each slab */
	unsigned int slab_order;
	/** Indicates weather cache can growing or not. All caches are growing by default */
	bool growing;
};

#define __CACHE_DEF(cache_nm, object_t, objects_nr) \
	static struct cache cache_nm =  {                      \
		.num = (objects_nr),              \
		.obj_size = sizeof(object_t),                  \
	};                                                     \
	extern const struct mod_member_ops __cache_member_ops; \
	MOD_MEMBER_BIND(&__cache_member_ops, &cache_nm)

#endif /* MEM_MISC_SLAB_IMPL_H_ */
