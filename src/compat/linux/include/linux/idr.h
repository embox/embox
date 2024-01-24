/**
 * @file
 *
 * @date Jan 22, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_COMPAT_LINUX_INCLUDE_IDR_H_
#define SRC_COMPAT_LINUX_INCLUDE_IDR_H_

#include <assert.h>
#include <stddef.h>
#include <limits.h>

#include <lib/libds/bitmap.h>

#include <linux/types.h>

struct idr {
	int inited;
	unsigned long id_pool;
	void *object_handlers[sizeof(unsigned long) * LONG_BIT];
};

/**
 * idr_preload - preload for idr_alloc()
 * @gfp_mask: allocation mask to use for preloading
 *
 * Preload per-cpu layer buffer for idr_alloc().  Can only be used from
 * process context and each idr_preload() invocation should be matched with
 * idr_preload_end().  Note that preemption is disabled while preloaded.
 *
 * The first idr_alloc() in the preloaded section can be treated as if it
 * were invoked with @gfp_mask used for preloading.  This allows using more
 * permissive allocation masks for idrs protected by spinlocks.
 *
 * For example, if idr_alloc() below fails, the failure can be treated as
 * if idr_alloc() were called with GFP_KERNEL rather than GFP_NOWAIT.
 *
 *	idr_preload(GFP_KERNEL);
 *	spin_lock(lock);
 *
 *	id = idr_alloc(idr, ptr, start, end, GFP_NOWAIT);
 *
 *	spin_unlock(lock);
 *	idr_preload_end();
 *	if (id < 0)
 *		error;
 */
static inline void idr_preload(gfp_t gfp_mask) { }

/**
 * idr_alloc - allocate new idr entry
 * @idr: the (initialized) idr
 * @ptr: pointer to be associated with the new id
 * @start: the minimum id (inclusive)
 * @end: the maximum id (exclusive, <= 0 for max)
 * @gfp_mask: memory allocation flags
 *
 * Allocate an id in [start, end) and associate it with @ptr.  If no ID is
 * available in the specified range, returns -ENOSPC.  On memory allocation
 * failure, returns -ENOMEM.
 *
 * Note that @end is treated as max when <= 0.  This is to always allow
 * using @start + N as @end as long as N is inside integer range.
 *
 * The user is responsible for exclusively synchronizing all operations
 * which may modify @idr.  However, read-only accesses such as idr_find()
 * or iteration can be performed under RCU read lock provided the user
 * destroys @ptr in RCU-safe way after removal from idr.
 */
static inline int idr_alloc(struct idr *idp, void *ptr, int start, int end, gfp_t gfp_mask) {
	int id;
	assert(idp);

	if (!idp->inited) {
		idp->id_pool = 0;
		idp->inited = 1;
		memset(idp->object_handlers, 0, sizeof(idp->object_handlers));
	}

	if (start > sizeof(unsigned long) * CHAR_BIT) {
		return -ENOSPC;
	}

	id = bitmap_find_zero_bit(&idp->id_pool, 1, start);
	idp->object_handlers[id] = ptr;
	bitmap_set_bit(&idp->id_pool, id);

	return id;
}

/**
 * idr_preload_end - end preload section started with idr_preload()
 *
 * Each idr_preload() should be matched with an invocation of this
 * function.  See idr_preload() for details.
 */
static inline void idr_preload_end(void) { }

/**
 * idr_find - return pointer for given id
 * @idr: idr handle
 * @id: lookup key
 *
 * Return the pointer given the id it has been registered with.  A %NULL
 * return indicates that @id is not valid or you passed %NULL in
 * idr_get_new().
 *
 */
static inline void *idr_find(struct idr *idr, int id)
{
	return idr->object_handlers[id];
}


#endif /* SRC_COMPAT_LINUX_INCLUDE_IDR_H_ */
