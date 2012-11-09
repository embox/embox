/**
 * @file
 * @brief
 *
 * @date 09.11.12
 * @author Anton Bulychev
 */

#ifndef MEM_MMAP_MMU_H_
#define MEM_MMAP_MMU_H_

#include <types.h>
#include <util/dlist.h>

struct marea {
	uint32_t start, end;
	uint32_t flags;

	struct dlist_head mmap_link;
};

struct mmap {
	struct marea *stack_marea;

	struct dlist_head marea_list;
};


#endif /* MEM_MMAP_MMU_H_ */
