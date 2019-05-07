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
#include <mem/vmem.h>
#include <mem/mapping/marea.h>

#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/mmap.h>
#include <kernel/panic.h>

#define INSIDE(x,a,b)       (((a) <= (x)) && ((x) < (b)))
#define INTERSECT(a,b,c,d)  (INSIDE(a,c,d) || INSIDE(c,a,b))

static const uint32_t mem_start = 0x40000000;
static const uint32_t mem_end = 0xFFFFF000;

#define mmu_size_align(size) (((size) + MMU_PAGE_SIZE - 1) & ~(MMU_PAGE_SIZE - 1))

static inline int mmap_active(struct emmap *mmap) {
	return mmap_kernel_inited() && mmap == task_resource_mmap(task_self());
}

int mmap_do_marea_map(struct emmap *mmap, struct marea *marea) {
	size_t len = mmu_size_align(marea->end - marea->start);

	return vmem_map_region(mmap->ctx,
			marea->start,
			marea->start,
			len,
			marea_to_vmem_flags(marea->flags));
}

int mmap_do_marea_map_overwrite(struct emmap *mmap, struct marea *marea) {
	size_t len = mmu_size_align(marea->end - marea->start);

	return vmem_map_region_overwrite(mmap->ctx,
			marea->start,
			marea->start,
			len,
			marea_to_vmem_flags(marea->flags));
}

void mmap_do_marea_unmap(struct emmap *mmap, struct marea *marea) {
	size_t len = mmu_size_align(marea->end - marea->start);
	vmem_unmap_region(mmap->ctx, marea->start, len);
}

void mmap_init(struct emmap *mmap) {
	int err;
	dlist_init(&mmap->marea_list);

	if ((err = vmem_init_context(&mmap->ctx))) {
		panic("%s: %s\n", __func__, strerror(-err));
	}
}

void mmap_free(struct emmap *mmap) {
	/*
	 * To unmap this context we should switch to kernel task virtual context.
	 * Actually, we can save this context for the later created tasks.
	 */
	struct emmap *emmap;
	emmap = task_resource_mmap(task_kernel_task());

	mmu_set_context(emmap->ctx);

	mmap_clear(mmap);
	vmem_free_context(mmap->ctx);
}

void mmap_clear(struct emmap *mmap) {
	struct marea *marea;

	dlist_foreach_entry(marea, &mmap->marea_list, mmap_link) {
		vmem_unmap_region(mmap->ctx, marea->start, mmu_size_align(marea->end - marea->start));

		marea_destroy(marea);
	}
}

static int mmap_check_marea(struct emmap *mmap, struct marea *marea) {
	if (mmap_find_marea(mmap, marea->start)
		|| mmap_find_marea(mmap, marea->end)) {
		return -EEXIST;
	}

	return 0;
}

struct marea *mmap_place_marea(struct emmap *mmap, uint32_t start, uint32_t end, uint32_t flags) {
	struct marea *marea;

	start = MAREA_ALIGN_DOWN(start);
	end   = MAREA_ALIGN_UP(end);

	if (!(INSIDE(start, mem_start, mem_end) && INSIDE(end, mem_start, mem_end))) {
		goto error;
	}

	if (!(marea = marea_create(start, end, flags, true))) {
		goto error;
	}

	if (0 != mmap_check_marea(mmap, marea)) {
		goto error_free;
	}

	if (vmem_create_space(mmap->ctx, start, end-start, VMEM_PAGE_WRITABLE | VMEM_PAGE_USERMODE)) {
		goto error_free;
	}

	mmap_add_marea(mmap, marea);

	return marea;

error_free:
	marea_destroy(marea);
error:
	return NULL;
}
