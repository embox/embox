/**
 *   Memory allocation API.
 */

#include <stddef.h>

#include <util/macro.h>

#include __impl_x(CONFIG_RTALLOC_IMPL_H)

#define OBJALLOC_DEF(allocator_nm, object_t, objects_nr) \
	  __OBJALLOC_DEF(allocator_nm, object_t, objects_nr)

typedef __objalloc_t objalloc_t;

/**
 *
 * @param allocator
 * @param object_sz
 * @param objects_nr
 * @return
 */
extern int objalloc_init(objalloc_t *allocator, size_t object_sz,
		size_t objects_nr);

/**
 *
 * @param allocator
 * @return
 */
extern int objalloc_destroy(objalloc_t *allocator);

/**
 * Return pointer to object for which allocate memory
 *
 * @param size requested memory size
 *
 * @return pointer to the memory of the requested size.
 * @retval 0 if there are no memory
 */
extern void *objalloc(objalloc_t *uni);

/**
 * Free memory function.
 *
 * @param ptr pointer at the memory, that must be free
 */
extern void objfree(objalloc_t *uni, void* objp);
