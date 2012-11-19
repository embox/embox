/**
 * @file
 * @brief
 *
 * @date 09.11.12
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <errno.h>
#include <stdlib.h>

#include <mem/mmap.h>
#include <mem/vmem.h>

#include "../kernel/task/common.h"
#include "../kernel/thread/types.h"

#define INSIDE(x,a,b)       (((a) <= (x)) && ((x) < (b)))
#define INTERSECT(a,b,c,d)  (INSIDE(a,c,d) || INSIDE(c,a,b))

EMBOX_UNIT(init, fini);

static uint32_t mem_start;
static uint32_t mem_end;

static int initialized = 0;

void mmap_init(struct mmap *mmap) {
	dlist_init(&mmap->marea_list);
	mmap->stack_marea = NULL;

	assert(!vmem_create_context(&mmap->ctx));

	if (!initialized) {
		/* It's kernel task. Set virtual context for him. */
		vmem_set_context(mmap->ctx);
		initialized = 1;
	}
}

void mmap_free(struct mmap *mmap) {
	struct dlist_head *item, *next;
	struct marea *marea;

	dlist_foreach(item, next, &mmap->marea_list) {
		marea = dlist_entry(item, struct marea, mmap_link);
		dlist_del(&marea->mmap_link);

		vmem_unmap_region(mmap->ctx, marea->start, marea->end - marea->start, 1);

		free(marea);
	}
}

struct marea *mmap_place_marea(struct mmap *mmap, uint32_t start, uint32_t end, uint32_t flags) {
	struct dlist_head *item, *next;
	struct marea *marea;

	start = MAREA_ALIGN_DOWN(start);
	end   = MAREA_ALIGN_UP(end);

	if (!(INSIDE(start, mem_start, mem_end) && INSIDE(end, mem_start, mem_end))) {
		return NULL;
	}

	dlist_foreach(item, next, &mmap->marea_list) {
		marea = dlist_entry(item, struct marea, mmap_link);

		if (INTERSECT(start, end, marea->start, marea->end)) {
			return NULL;
		}
	}

	if (!(marea = malloc(sizeof(struct marea)))) {
		return NULL;
	}

	marea->start = start;
	marea->end   = end;
	marea->flags = flags;

	dlist_head_init(&marea->mmap_link);
	dlist_add_prev(&marea->mmap_link, &mmap->marea_list);

	vmem_create_space(mmap->ctx, start, end-start, VMEM_PAGE_WRITABLE);

	return marea;
}

struct marea *mmap_alloc_marea(struct mmap *mmap, size_t size, uint32_t flags) {
	struct dlist_head *item = &mmap->marea_list;
	uint32_t s_ptr = mem_start;
	struct marea *marea;

	size = MAREA_ALIGN_UP(size);

	do {
		if ((marea = mmap_place_marea(mmap, s_ptr, s_ptr + size, flags))) {
			return marea;
		}

		item = item->next;
		if (item == &mmap->marea_list) {
			break;
		}

		marea = dlist_entry(item, struct marea, mmap_link);
		s_ptr = marea->end;
	} while(1);

	return NULL;
}

uint32_t mmap_create_stack(struct mmap *mmap) {
	mmap->stack_marea = mmap_alloc_marea(mmap, 1024, 0);

	if (!mmap->stack_marea) {
		return 0;
	}

	return mmap->stack_marea->end;
}

static int init() {
	mem_start = 0x02000000;
	mem_end   = 0xFFFFF000;

	return 0;
}

static int fini() {

	return 0;
}

static int task_switch_handler(struct thread *prev, struct thread *next) {
	mmu_set_context(next->task->mmap->ctx);
	return 0;
}

TASK_RESOURCE_NOTIFY(task_switch_handler);

