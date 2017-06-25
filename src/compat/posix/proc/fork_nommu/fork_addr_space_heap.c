/**
 * @file
 * @brief
 *
 * @author  Denis Deryugin
 * @date    4 Aug 2014
 */

#include "fork_copy_addr_space.h"
#include <kernel/task.h>
#include <kernel/task/resource/task_heap.h>
#include <kernel/task/resource/task_fork.h>
#include <mem/page.h>
#include <mem/phymem.h>
#include <sys/types.h>

extern size_t mspace_deep_copy_size(struct dlist_head *mspace);
extern void mspace_deep_store(struct dlist_head *mspace, struct dlist_head *store_space, void *buf);
extern void mspace_deep_restore(struct dlist_head *mspace, struct dlist_head *store_space, void *buf);

static inline struct dlist_head *task_mspace(void) {
	struct task_heap *task_heap;

	task_heap = task_heap_get(task_self());
	return &task_heap->mm;
}

void fork_heap_store(struct heap_space *hpspc) {
	size_t size;

	size = mspace_deep_copy_size(task_mspace());

	if (hpspc->heap_sz != size) {
		if (hpspc->heap) {
			phymem_free(hpspc->heap, hpspc->heap_sz / PAGE_SIZE());
		}

		hpspc->heap = phymem_alloc(size / PAGE_SIZE());
		assert(hpspc->heap);
	}
	mspace_deep_store(task_mspace(), &hpspc->store_space, hpspc->heap);
}

void fork_heap_restore(struct heap_space *hpspc) {
	mspace_deep_restore(task_mspace(), &hpspc->store_space, hpspc->heap);
}

void fork_heap_cleanup(struct heap_space *hpspc) {
	if (!hpspc->heap) {
		return;
	}

	phymem_free(hpspc->heap, hpspc->heap_sz / PAGE_SIZE());
	hpspc->heap = NULL;
}

