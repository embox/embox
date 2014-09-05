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

#define INSIDE(x,a,b)       (((a) <= (x)) && ((x) < (b)))
#define INTERSECT(a,b,c,d)  (INSIDE(a,c,d) || INSIDE(c,a,b))

//static const uint32_t mem_start = 0x04000000;
static const uint32_t mem_start = 0x40000000;
static const uint32_t mem_end = 0xFFFFF000;


#define mmu_size_align(size) (((size) + MMU_PAGE_SIZE) & ~(MMU_PAGE_SIZE - 1))

void mmap_add_marea(struct emmap *mmap, struct marea *marea) {
	dlist_add_prev(&marea->mmap_link, &mmap->marea_list);
}

void mmap_init(struct emmap *mmap) {
	dlist_init(&mmap->marea_list);
	//mmap->stack_marea = NULL;
	//mmap->heap_marea = NULL;

	assert(!vmem_init_context(&mmap->ctx));
}

void mmap_free(struct emmap *mmap) {
	//XXX: Bad code
	mmu_set_context(1);
	mmap_clear(mmap);
	//vmem_free_context(mmap->ctx);
}

void mmap_clear(struct emmap *mmap) {
	struct marea *marea;

	dlist_foreach_entry(marea, &mmap->marea_list, mmap_link) {
		vmem_unmap_region(mmap->ctx, marea->start, mmu_size_align(marea->end - marea->start), 0);

		marea_destroy(marea);
	}
}

struct marea *mmap_place_marea(struct emmap *mmap, uint32_t start, uint32_t end, uint32_t flags) {
	struct marea *marea;

	start = MAREA_ALIGN_DOWN(start);
	end   = MAREA_ALIGN_UP(end);

	if (!(INSIDE(start, mem_start, mem_end) && INSIDE(end, mem_start, mem_end))) {
		return NULL;
	}

	dlist_foreach_entry(marea, &mmap->marea_list, mmap_link) {
		if (INTERSECT(start, end, marea->start, marea->end)) {
			return NULL;
		}
	}

	if (!(marea = marea_create(start, end, flags))) {
		return NULL;
	}

	if (vmem_create_space(mmap->ctx, start, end-start, VMEM_PAGE_WRITABLE | VMEM_PAGE_USERMODE)) {
		free(marea);
		return NULL;
	}

	mmap_add_marea(mmap, marea);

	return marea;
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
		s_ptr = marea->end;
	} while(1);

	return NULL;
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


int mmap_mapping(struct emmap *emmap) {
	struct marea *marea;

	dlist_foreach_entry(marea, &emmap->marea_list, mmap_link) {
		size_t len = mmu_size_align(marea->end - marea->start);

		vmem_map_region(emmap->ctx, marea->start, marea->start, len,  marea_to_vmem_flags(marea->flags));
	}

	return 0;
}

int mmap_inherit(struct emmap *mmap, struct emmap *p_mmap) {
	struct marea *marea, *new_marea;

	dlist_foreach_entry(marea, &p_mmap->marea_list, mmap_link) {
		if (!(new_marea = marea_create(marea->start, marea->end, marea->flags))) {
			return -ENOMEM;
		}
		mmap_add_marea(mmap, new_marea);
	}
	mmap_mapping(mmap);

	return 0;
}

#include <kernel/task/resource/mmap.h>

mmu_ctx_t mmap_get_current_context(void) {
	struct emmap *emmap;

	emmap = task_self_resource_mmap();

	return emmap->ctx;
}
