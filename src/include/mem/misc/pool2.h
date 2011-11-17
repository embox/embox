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
	void * bound_free;
	struct slist free_blocks;
	size_t obj_size;
	size_t pool_size;
	size_t current_size;
};


#define POOL_DEF(name, object_type, size) \
	static union {                                  \
		typeof(object_type) object;                 \
		struct slist free_link;      \
	} name ## _storage[size] __attribute__((section(".reserve.pool"))); \
	static struct pool name = { \
			.memory = (void*)name ## _storage, \
			.bound_free = (void*)name ## _storage, \
			.obj_size = sizeof(*name ## _storage), \
			.pool_size = sizeof(*name ## _storage) * size, \
			.current_size = 0 \
};


#if 0
#define POOL_DEF(pool_name, elem_type, pool_size) \
			__POOL_DEF(pool_name, elem_type, pool_size, MACRO_GUARD(__pool_##pool_name))

#define __POOL_DEF(pool_name, elem_type, pool_size, storage_name)\
			static  typeof(elem_type) obj \
			storage_name[pool_size] __attribute__((section(".reserve.pool")));\
			static obj_pool pool_name = { \
					.obj_size = sizeof(elem_type) \
			}

#endif

extern void *pool2_alloc(struct pool *pool);

extern void pool2_free(struct pool *pool, void* obj);

#endif /* MEM_MISC_POOL2_H_ */
