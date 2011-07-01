/**
 * @brief Adapt cache structure to objalloc interface.
 *
 * @see mem/objalloc.h
 *
 * @date   30.06.11
 * @author Alexandr Kalmuk
 */

#include <mem/misc/pool.h>

#define __OBJALLOC_DEF(allocator_nm, object_t, objects_nr) \
	POOL_DEF(allocator_nm, object_t, objects_nr)

typedef struct pool objalloc_t;

static inline int objalloc_init(objalloc_t *allocator, size_t object_sz,
		size_t objects_nr) {
	return 0;
}

static inline int objalloc_destroy(objalloc_t *allocator) {
	return 0;
}
