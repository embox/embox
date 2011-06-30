/**
 *   Memory allocation API.
 */

#include <util/macro.h>

#include __impl_x(CONFIG_RTALLOC_IMPL_H)

/**
 * wrapper for pool and cache.
 */
struct objalloc;

/**
 * Return pointer to object for which allocate memory
 *
 * @param size requested memory size
 *
 * @return pointer to the memory of the requested size.
 * @retval 0 if there are no memory
 */
extern void *objalloc(struct objalloc *uni);

/**
 * Free memory function.
 *
 * @param ptr pointer at the memory, that must be free
 */
extern void objfree(struct objalloc *uni, void* objp);

/**
 * Destroy of cache
 * @param cache_ptr is pointer to cache which must be deleted
 */
extern void objcache_destroy(struct objalloc *uni);
