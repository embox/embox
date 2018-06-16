/**
 * @file
 * @brief
 *
 * @author  Denis Deryugin
 * @date    4 Aug 2014
 */
#include <assert.h>
#include <sys/types.h>

#include "fork_copy_addr_space.h"
#include <kernel/task.h>
#include <kernel/task/resource/task_heap.h>
#include <kernel/task/resource/task_fork.h>
#include <mem/page.h>
#include <mem/phymem.h>

extern size_t mspace_deep_copy_size(struct dlist_head *mspace);
extern void mspace_deep_store(struct dlist_head *mspace, struct dlist_head *store_space, void *buf);
extern void mspace_deep_restore(struct dlist_head *mspace, struct dlist_head *store_space, void *buf);

static inline struct dlist_head *task_mspace(struct task *tk) {
	struct task_heap *task_heap;

	task_heap = task_heap_get(tk);
	return &task_heap->mm;
}

void fork_heap_store(struct heap_space *hpspc, struct task *tk) {
	size_t size;

	assert(hpspc);
	assert(tk);

	size = mspace_deep_copy_size(task_mspace(tk));
	if (!size) {
		return;
	}

	if (hpspc->heap_sz != size) {
		if (hpspc->heap) {
			phymem_free(hpspc->heap, hpspc->heap_sz / PAGE_SIZE());
		}

		hpspc->heap = phymem_alloc(size / PAGE_SIZE());
		hpspc->heap_sz = size;
	}
	assert(hpspc->heap);
	mspace_deep_store(task_mspace(tk), &hpspc->store_space, hpspc->heap);
}

void fork_heap_restore(struct heap_space *hpspc) {
	assert(hpspc);

	if (NULL == hpspc->heap) {
		return;
	}

	mspace_deep_restore(task_mspace(task_self()), &hpspc->store_space, hpspc->heap);
}

void fork_heap_cleanup(struct heap_space *hpspc) {
	if (!hpspc->heap) {
		return;
	}

	phymem_free(hpspc->heap, hpspc->heap_sz / PAGE_SIZE());
	hpspc->heap = NULL;
}

