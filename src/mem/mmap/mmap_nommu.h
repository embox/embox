/**
 * @file
 * @brief
 *
 * @date 09.11.12
 * @author Anton Bulychev
 */

#ifndef MEM_MMAP_NOMMU_H_
#define MEM_MMAP_NOMMU_H_

#include <stdint.h>
#include <util/dlist.h>

struct marea {
	uint32_t start, end;
	uint32_t flags;

	struct dlist_head mmap_link;
	struct dlist_head glob_link;
};

struct emmap {
	struct marea *stack_marea;
	struct marea *heap_marea;
	void *brk;

	struct dlist_head marea_list;
};

static inline uint32_t marea_get_start(struct marea *marea) {
	return marea->start;
}

static inline void *mmap_get_brk(struct emmap *mmap) {
	return mmap->brk;
}

static inline void mmap_set_brk(struct emmap *mmap, void *new_brk) {
	mmap->brk = new_brk;
}

#endif /* MEM_MMAP_NOMMU_H_ */
