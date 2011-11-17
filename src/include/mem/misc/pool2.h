/**
 * @file	pool2.h
 *
 * @brief	Object pool allocator, new implementation
 *
 * @date	10.11.11
 *
 * @author	Gleb Efimov
 */

#ifndef MEM_MISC_POOL2_H_
#define MEM_MISC_POOL2_H_

#include <types.h>
#include <util/macro.h>
#include <util/slist.h>

struct pool {
	void * memory;
	struct slist free_blocks;
	size_t obj_size;
	size_t pool_size;
	size_t bound_free;
};


#define POOL_DEF(name, object_type, size) \
	static union {                                  \
		typeof(object_type) object;                 \
		struct slist_link free_link;      \
	} name ## _storage[size] __attribute__((section(".reserve.pool"))); \
	struct slist temp_list;\
	static struct pool name = { \
			.memory = name ## _storage, \
			/*.bound_free = (void*)name ## _storage, \*/ \
			.obj_size = sizeof(*name ## _storage), \
			.pool_size = sizeof(*name ## _storage) * size, \
			.bound_free = 0 \
};

extern void *pool2_alloc(struct pool *pool);

extern void pool2_free(struct pool *pool, void* obj);

#endif /* MEM_MISC_POOL2_H_ */
