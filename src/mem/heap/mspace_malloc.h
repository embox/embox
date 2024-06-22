/**
 * @file
 * @brief

 * @date 07.04.2014
 * @author Alexander Kalmuk
 */

#ifndef MEM_HEAP_MSPACE_MALLOC_H_
#define MEM_HEAP_MSPACE_MALLOC_H_

#include <stddef.h>

#include <lib/libds/dlist.h>

typedef enum heap_type {
	HEAP_RAM = 0,
	HEAP_FAST_RAM = 1,
	HEAP_EXTERN_MEM = 2
} heap_type_t;

extern void *mspace_memalign(size_t boundary, size_t size,
    struct dlist_head *mspace);
extern void *mspace_malloc(size_t size, struct dlist_head *mspace);
extern void *mspace_calloc(size_t nmemb, size_t size,
    struct dlist_head *mspace);
extern void *mspace_realloc(void *ptr, size_t size, struct dlist_head *mspace);
extern int mspace_free(void *ptr, struct dlist_head *mspace);
extern int mspace_set_heap(heap_type_t type, heap_type_t *prev_type);

#endif /* MEM_HEAP_MSPACE_MALLOC_H_ */
