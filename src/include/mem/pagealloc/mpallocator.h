/**
 * @file
 * @brief multipage allocator header
 * @details
 *
 * @date 28.04.10
 * @author Fedor Burdun
 */

#ifndef INCLUDE_MEM_PAGEALLOC_MPALLOCATOR_H_
#define INCLUDE_MEM_PAGEALLOC_MPALLOCATOR_H_

#include <types.h>

typedef struct block_info {
	struct list_head *next, *prev;
	size_t size;
	bool free;
} block_info_t;

extern void *mpalloc(size_t page_quantity);
extern void mpfree(void *pointer);

extern void mpget_blocks_info(struct list_head* list);

#endif /* INCLUDE_MEM_PAGEALLOC_MPALLOCATOR_H_ */

