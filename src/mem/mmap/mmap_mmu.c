/**
 * @file
 * @brief
 *
 * @date 09.11.12
 * @author Anton Bulychev
 */

#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <mem/mmap.h>
#include <mem/vmem.h>

#include <mem/mapping/marea.h>
#include <kernel/task/resource/mmap.h>
#include <kernel/panic.h>

#define INSIDE(x,a,b)       (((a) <= (x)) && ((x) < (b)))
#define INTERSECT(a,b,c,d)  (INSIDE(a,c,d) || INSIDE(c,a,b))

//static const uint32_t mem_start = 0x04000000;
static const uint32_t mem_start = 0x40000000;
static const uint32_t mem_end = 0xFFFFF000;

#define mmu_size_align(size) (((size) + MMU_PAGE_SIZE - 1) & ~(MMU_PAGE_SIZE - 1))

static inline int mmap_active(struct emmap *mmap) {
#if 0 //TODO this simple version should work
	return mmap == task_resource_mmap(task_self());
#else
	extern int mmap_kernel_inited(void);
	return mmap_kernel_inited() && mmap == task_resource_mmap(task_self());
#endif
}

static vmem_page_flags_t marea_to_vmem_flags(uint32_t flags) {
	vmem_page_flags_t vmem_page_flags = 0;
	if (flags & PROT_WRITE) {
		vmem_page_flags |= VMEM_PAGE_WRITABLE;
	}
	if (flags & PROT_EXEC) {
		vmem_page_flags |= VMEM_PAGE_EXECUTABLE;
	}
	if (!(flags & PROT_NOCACHE)) {
		vmem_page_flags |= VMEM_PAGE_CACHEABLE;
	}
	return vmem_page_flags;
}

int mmap_do_marea_map(struct emmap *mmap, struct marea *marea) {
	size_t len = mmu_size_align(marea->end - marea->start);

	return vmem_map_region(mmap->ctx, marea->start, marea->start, len,  marea_to_vmem_flags(marea->flags));
}

void mmap_do_marea_unmap(struct emmap *mmap, struct marea *marea) {
	size_t len = mmu_size_align(marea->end - marea->start);
	vmem_unmap_region(mmap->ctx, marea->start, len, marea->is_allocated);
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

static int mmap_check_marea(struct emmap *mmap, struct marea *marea) {
	if (mmap_find_marea(mmap, marea->start)
			|| mmap_find_marea(mmap, marea->end)) {
		return -EEXIST;
	}
	return 0;
}

void mmap_add_marea(struct emmap *mmap, struct marea *marea) {
	struct marea *ma_err;

	if ((ma_err = mmap_find_marea(mmap, marea->start))
			|| (ma_err = mmap_find_marea(mmap, marea->end))) {
		panic("%s: intersect existing=%p(%p):%p\n new=%p(%p):%p\n", __func__,
				(void *) ma_err->start, /* XXX */ (void *) ma_err->start, (void *) ma_err->end,
				(void *) marea->start, /* XXX */ (void *) marea->start, (void *) marea->end);
	}

	dlist_add_prev(&marea->mmap_link, &mmap->marea_list);
}

void mmap_del_marea(struct marea *marea) {
	dlist_del(&marea->mmap_link);
}

void mmap_init(struct emmap *mmap) {
	int err;
	dlist_init(&mmap->marea_list);
	//mmap->stack_marea = NULL;
	//mmap->heap_marea = NULL;

	if ((err = vmem_init_context(&mmap->ctx))) {
		panic("%s: %s\n", __func__, strerror(-err));
	}
}

void mmap_free(struct emmap *mmap) {
	/*
	 * To unmap this context we should switch to kernel task virtual context.
	 * Actually, we can save this context for the later created tasks.
	 */

	//XXX: Bad code
	mmu_set_context(1);
	mmap_clear(mmap);
	vmem_free_context(mmap->ctx);
}

void mmap_clear(struct emmap *mmap) {
	struct marea *marea;

	dlist_foreach_entry(marea, &mmap->marea_list, mmap_link) {
		vmem_unmap_region(mmap->ctx, marea->start, mmu_size_align(marea->end - marea->start), marea->is_allocated);

		marea_destroy(marea);
	}
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
