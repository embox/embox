/**
 * @file
 * @brief
 *
 * @date 09.11.12
 * @author Anton Bulychev
 */

#ifndef MEM_MMAP_MMU_H_
#define MEM_MMAP_MMU_H_

#include <stdint.h>
#include <util/dlist.h>
#include <hal/mmu.h>

struct marea {
	uintptr_t start;
	uintptr_t end;
	uint32_t flags;
	uint32_t is_allocated;

	struct dlist_head mmap_link;
};

struct emmap {
#if 0
	struct marea *stack_marea;
	struct marea *heap_marea;
#endif
	void *brk;

	mmu_ctx_t ctx;

	struct dlist_head marea_list;
};


#endif /* MEM_MMAP_MMU_H_ */
