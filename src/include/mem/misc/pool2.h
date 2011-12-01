/**
 * @file
 *
 * @brief	Object pool allocator, new implementation
 *
 * @date	10.11.11
 *
 * @author	Gleb Efimov
 */

#ifndef MEM_MISC_POOL_H_
#define MEM_MISC_POOL_H_

#include <types.h>
#include <util/macro.h>
#include <util/slist.h>

/* Representation of the pool*/
struct pool {
	/* Place in memory for allocation */
	void * memory;
	/* List of free block
	 * (this is a block, which was used,
	 * then was non-used and return to pool) */
	struct slist free_blocks;
	/* Size of object in pool (in bytes) */
	size_t obj_size;
	/* Size of pool */
	size_t pool_size;
	/* Boundary, after which begin
	 * non-allocated memory */
	void * bound_free;
};

/* Macros that initializes an empty pool
 * @name - name of pool
 * @object_type - type of elements in the pool
 * @size - size of pool */
#define POOL_DEF(name, object_type, size) \
	static union {                                  \
		typeof(object_type) object;                 \
		struct slist_link free_link;      \
	} __pool_storage ## name[size] __attribute__((section(".reserve.pool"))); \
	static struct pool name = { \
			.memory = __pool_storage ## name, \
			.bound_free = __pool_storage ## name, \
			.free_blocks = SLIST_INIT(&name.free_blocks),\
			.obj_size = sizeof(*__pool_storage ## name), \
			.pool_size = sizeof(*__pool_storage ## name) * size, \
};

/* Function to allocate memory
 * @pool - pointer to the pool, which memory from allocated
 *
 * @return - pointer to the allocated memory */
extern void *pool_alloc(struct pool *pool);

/* Procedure remove object from pool
 * @pool - pointer to the pool
 * @obj - pointer to the delitable object */
extern void pool_free(struct pool *pool, void* obj);

#endif /* MEM_MISC_POOL2_H_ */
