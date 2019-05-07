/**
 * @file
 * @brief
 *
 * @date 09.11.12
 * @author Anton Bulychev
 */

#include <util/log.h>

#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <hal/mmu.h>

#include <mem/mmap.h>
#include <mem/phymem.h>
#include <mem/mapping/marea.h>

#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/mmap.h>
#include <kernel/panic.h>

#define INSIDE(x,a,b)       (((a) <= (x)) && ((x) < (b)))
#define INTERSECT(a,b,c,d)  (INSIDE(a,c,d) || INSIDE(c,a,b))

static const uint32_t mem_start = 0x40000000;

#define mmu_size_align(size) (((size) + MMU_PAGE_SIZE - 1) & ~(MMU_PAGE_SIZE - 1))

static inline int mmap_active(struct emmap *mmap) {
	return mmap_kernel_inited() && mmap == task_resource_mmap(task_self());
}

struct marea *mmap_find_marea(struct emmap *mmap, mmu_vaddr_t vaddr) {
	struct marea *marea;
	dlist_foreach_entry(marea, &mmap->marea_list, mmap_link) {
		if (INSIDE(vaddr, marea->start, marea->end)) {
			return marea;
		}
	}
	return NULL;
}

struct marea *mmap_find_marea_next(struct emmap *mmap, mmu_vaddr_t vaddr,
		struct marea *prev) {
	struct marea *marea;
	int found_prev = prev == NULL ? 1 : 0;

	dlist_foreach_entry(marea, &mmap->marea_list, mmap_link) {
		if (found_prev && INSIDE(vaddr, marea->start, marea->end)) {
			return marea;
		}

		if (marea == prev) {
			found_prev = 1;
		}
	}

	return NULL;
}

void mmap_add_marea(struct emmap *mmap, struct marea *marea) {
	log_debug("add to %p start %p end %p",marea, marea->start, marea->end);
	dlist_add_prev(&marea->mmap_link, &mmap->marea_list);
}

void mmap_del_marea(struct marea *marea) {
	dlist_del(&marea->mmap_link);
}

struct marea *mmap_alloc_marea(struct emmap *mmap, size_t size, uint32_t flags) {
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
		s_ptr = MAREA_ALIGN_UP(marea->end);
	} while(1);

	return NULL;
}

static void mmap_unmap_on_error(struct emmap *emmap, struct marea *err_ma) {
	struct marea *marea;
	dlist_foreach_entry(marea, &emmap->marea_list, mmap_link) {
		if (marea == err_ma) {
			break;
		}
		mmap_do_marea_unmap(emmap, marea);
	}
}

int mmap_mapping(struct emmap *emmap) {
	struct marea *marea;
	int err;

	dlist_foreach_entry(marea, &emmap->marea_list, mmap_link) {
		log_debug("do_map: to %p start %p end %p", marea, marea->start, marea->end);
		err = mmap_do_marea_map(emmap, marea);
		if (err) {
			goto out_err;
		}
	}

	return 0;

out_err:
	mmap_unmap_on_error(emmap, marea);
	return err;
}

int mmap_inherit(struct emmap *mmap, struct emmap *p_mmap) {
	struct marea *marea, *new_marea;

	dlist_foreach_entry(marea, &p_mmap->marea_list, mmap_link) {
		if (!(new_marea = marea_create(marea->start, marea->end, marea->flags, marea->is_allocated))) {
			return -ENOMEM;
		}
		mmap_add_marea(mmap, new_marea);
	}

	return mmap_mapping(mmap);
}

#include <kernel/task/resource/mmap.h>

mmu_ctx_t mmap_get_current_context(void) {
	struct emmap *emmap;

	emmap = task_self_resource_mmap();

	return emmap->ctx;
}
