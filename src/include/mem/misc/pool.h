/**
 * @file
 * @brief An allocator's API for fixed-size objects.
 *
 * @author Kirill Tyushev
 * @author Eldar Abusalimov
 * @author Gleb Efimov
 */

#ifndef MEM_MISC_UTIL_POOL_H_
#define MEM_MISC_UTIL_POOL_H_

#include <stddef.h>

#include <lib/libds/bitmap.h>
#include <lib/libds/slist.h>
#include <util/macro.h>

#include <module/embox/mem/pool.h>

/** Representation of the pool*/
struct pool {
	/* Place in memory for allocation */
	void *memory;
	/* List of free block
	 * (this is a block, which was used,
	 * then was non-used and return to pool) */
	struct slist free_blocks;
	/* Size of object in pool (in bytes) */
	size_t obj_size;
	/* Size of pool */
	size_t pool_size;
	/* Boundary, after which begin non-allocated memory */
	void *bound_free;
#ifdef POOL_DEBUG
	BITMAP_DECL(blocks, POOL_MAX_OBJECTS);
#endif
};

#ifdef POOL_DEBUG
#define POOL_BLOCKS_INIT .blocks = {0},
#else
#define POOL_BLOCKS_INIT
#endif

/**
 * Create pool descriptor. The memory for pool is allocated in special section
 * "reserve.pool".
 *
 * @param name of cache
 * @param type of objects in cache
 * @param count of objects in cache
 * @param specific attributes like aligned
 */
#if defined __LCC__
#define POOL_DEF_ATTR(name, object_type, size, attr)             \
	static union {                                               \
		object_type object;                                      \
		struct slist_link free_link;                             \
	} __pool_storage##name[size] attr                            \
	    __attribute__((section(".bss..reserve.pool,\"aw\";#"))); \
	static struct pool name = {                                  \
	    .memory = __pool_storage##name,                          \
	    .bound_free = __pool_storage##name,                      \
	    .free_blocks = SLIST_INIT(&name.free_blocks),            \
	    .obj_size = sizeof(__pool_storage##name[0]),             \
	    .pool_size = sizeof(__pool_storage##name),               \
	    POOL_BLOCKS_INIT,                                        \
	};

#else
#define POOL_DEF_ATTR(name, object_type, size, attr)                     \
	static union {                                                       \
		object_type object;                                              \
		struct slist_link free_link;                                     \
	} __pool_storage##name[size] attr                                    \
	    __attribute__((section(".bss..reserve.pool,\"aw\",%nobits;#"))); \
	static struct pool name = {                                          \
	    .memory = __pool_storage##name,                                  \
	    .bound_free = __pool_storage##name,                              \
	    .free_blocks = SLIST_INIT(&name.free_blocks),                    \
	    .obj_size = sizeof(__pool_storage##name[0]),                     \
	    .pool_size = sizeof(__pool_storage##name),                       \
	    POOL_BLOCKS_INIT                                                 \
	};

#define POOL_DEF_SECTION_ATTR(name, object_type, size, section_name, attr) \
	static union {                                                         \
		object_type object;                                                \
		struct slist_link free_link;                                       \
	} __pool_storage##name[size] attr                                      \
	    __attribute__((section(section_name ",\"aw\",%nobits;#")));        \
	static struct pool name = {                                            \
	    .memory = __pool_storage##name,                                    \
	    .bound_free = __pool_storage##name,                                \
	    .free_blocks = SLIST_INIT(&name.free_blocks),                      \
	    .obj_size = sizeof(__pool_storage##name[0]),                       \
	    .pool_size = sizeof(__pool_storage##name),                         \
	    POOL_BLOCKS_INIT                                                   \
	};
#endif

/**
 * Create pool descriptor. The memory for pool is allocated in special section
 * "reserve.pool".
 *
 * @param name of cache
 * @param type of objects in cache
 * @param count of objects in cache
 */
#define POOL_DEF(name, object_type, size) \
	POOL_DEF_ATTR(name, object_type, size, )

/**
 * allocate single object from the cache and return it to the caller
 * @param cache corresponding to allocating object
 * @return the address of allocated object or NULL if pool is full
 */
extern void *pool_alloc(struct pool *pl);

/**
 * free an object and return it to the cache
 * @param cachep corresponding to freeing object
 * @param objp which will be free
 */
extern void pool_free(struct pool *pl, void *obj);

extern int pool_belong(const struct pool *pl, const void *obj);

#endif /* MEM_MISC_UTIL_POOL_H_ */
