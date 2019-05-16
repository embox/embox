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
#include <kernel/task/resource/mmap.h>
#include <kernel/task/kernel_task.h>

#define INSIDE(x,a,b)       (((a) <= (x)) && ((x) < (b)))
#define INTERSECT(a,b,c,d)  (INSIDE(a,c,d) || INSIDE(c,a,b))

static struct marea *mmap_find_marea(struct emmap *mmap, mmu_vaddr_t vaddr) {
	struct marea *marea;

	assert(mmap);

	dlist_foreach_entry(marea, &mmap->marea_list, mmap_link) {
		if (INSIDE(vaddr, marea->start, marea->start + marea->size)) {
			return marea;
		}
	}
	return NULL;
}

static int mmap_check_marea(struct emmap *mmap, struct marea *marea) {
	assert(mmap);

	if (mmap_find_marea(mmap, marea->start)
		|| mmap_find_marea(mmap, marea->start + marea->size - 1)) {
		return -EEXIST;
	}

	return 0;
}

int mmap_place(struct emmap *mmap, uintptr_t start, size_t size, uint32_t flags) {
	struct marea *marea;

	if (mmap == EMMAP_SELF) {
		mmap = task_self_resource_mmap();
	} else if (mmap == EMMAP_KERNEL) {
		mmap = task_resource_mmap(task_kernel_task());
	}

	assert(mmap);

	if (!(marea = marea_alloc(start, size, flags))) {
		return -ENOMEM;
	}

	if (0 != mmap_check_marea(mmap, marea)) {
		marea_free(marea);
		return -EINVAL;
	}

	dlist_add_prev(&marea->mmap_link, &mmap->marea_list);

	return 0;
}

int mmap_release(struct emmap *mmap, uintptr_t addr) {
	struct marea *m;

	if (mmap == EMMAP_SELF) {
		mmap = task_self_resource_mmap();
	} else if (mmap == EMMAP_KERNEL) {
		mmap = task_resource_mmap(task_kernel_task());
	}

	assert(mmap);

	m = mmap_find_marea(mmap, addr);

	if (m) {
		dlist_del(&m->mmap_link);
		return 0;
	} else {
		return -EINVAL;
	}
}

int mmap_prot(struct emmap *mmap, uintptr_t addr) {
	struct marea *m;

	if (mmap == EMMAP_SELF) {
		mmap = task_self_resource_mmap();
	} else if (mmap == EMMAP_KERNEL) {
		mmap = task_resource_mmap(task_kernel_task());
	}

	assert(mmap);

	m = mmap_find_marea(mmap, addr);

	if (m) {
		return m->flags;
	} else {
		return 0;
	}
}

#define INSIDE(x,a,b)       (((a) <= (x)) && ((x) < (b)))
#define INTERSECT(a,b,c,d)  (INSIDE(a,c,d) || INSIDE(c,a,b))
uintptr_t mmap_alloc(struct emmap *mmap, size_t size) {
	uintptr_t ptr_s = MAREA_ALIGN_UP(0x1);
	uintptr_t ptr_e = MAREA_ALIGN_UP(ptr_s + size);
	struct marea *marea;

	if (mmap == EMMAP_SELF) {
		mmap = task_self_resource_mmap();
	} else if (mmap == EMMAP_KERNEL) {
		mmap = task_resource_mmap(task_kernel_task());
	}

	assert(mmap);

	do {
		dlist_foreach_entry(marea, &mmap->marea_list, mmap_link) {
			if (INTERSECT(ptr_s, ptr_e,
						marea->start, marea->start + marea->size)) {
				ptr_s = MAREA_ALIGN_UP(marea->start + marea->size);
				ptr_e = MAREA_ALIGN_UP(ptr_s + size);

				continue;
			}
		}

		break;
	} while (1);

	return ptr_s;
}
