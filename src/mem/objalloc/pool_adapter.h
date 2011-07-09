/**
 * @file
 * @brief Adapt cache structure to objalloc interface.
 *
 * @see include/mem/objalloc.h
 *
 * @date   30.06.11
 * @author Alexandr Kalmuk
 */

#ifndef MEM_MISC_POOL_ADAPTER_H_
#define MEM_MISC_POOL_ADAPTER_H_

#include <mem/misc/pool.h>

#define __OBJALLOC_DEF(allocator_nm, object_t, objects_nr) \
	POOL_DEF(allocator_nm, object_t, objects_nr)

typedef struct pool __objalloc_t;

static inline int objalloc_init(__objalloc_t *allocator, size_t object_sz,
		size_t objects_nr) {
	return 0;
}

static inline int objalloc_destroy(__objalloc_t *allocator) {
	return 0;
}

#endif /* MEM_MISC_POOL_ADAPTER_H_ */
