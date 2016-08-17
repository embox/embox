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

#include <mem/page.h>
#include <mem/phymem.h>
#include <mem/mmap.h>
#include <mem/sysmalloc.h>

#define INSIDE(x,a,b)       (((a) <= (x)) && ((x) < (b)))
#define INTERSECT(a,b,c,d)  (INSIDE(a,c,d) || INSIDE(c,a,b))

EMBOX_UNIT(init, fini);

static DLIST_DEFINE(glob_list);

static uint32_t mem_start;
static uint32_t mem_end;
static uint32_t mem_page_count;

void mmap_init(struct emmap *mmap) {
	dlist_init(&mmap->marea_list);
	mmap->stack_marea = NULL;
}

void mmap_free(struct emmap *mmap) {
	mmap_clear(mmap);
}

void mmap_clear(struct emmap *mmap) {
	struct marea *marea;

	dlist_foreach_entry(marea, &mmap->marea_list, mmap_link) {
		dlist_del(&marea->mmap_link);
		dlist_del(&marea->glob_link);

		free(marea);
	}
}

struct marea *mmap_place_marea(struct emmap *mmap, uint32_t start, uint32_t end, uint32_t flags) {
	struct marea *marea;

	start = MAREA_ALIGN_DOWN(start);
	end   = MAREA_ALIGN_UP(end);

	if (!(INSIDE(start, mem_start, mem_end) && INSIDE(end, mem_start, mem_end))) {
		return NULL;
	}

	dlist_foreach_entry(marea, &glob_list, glob_link) {
		if (INTERSECT(start, end, marea->start, marea->end)) {
			return NULL;
		}
	}

	if (!(marea = sysmalloc(sizeof(struct marea)))) {
		return NULL;
	}

	marea->start = start;
	marea->end   = end;
	marea->flags = flags;

	dlist_head_init(&marea->glob_link);
	dlist_head_init(&marea->mmap_link);

	dlist_add_prev(&marea->glob_link, &glob_list);
	dlist_add_prev(&marea->mmap_link, &mmap->marea_list);

	return marea;
}

struct marea *mmap_alloc_marea(struct emmap *mmap, size_t size, uint32_t flags) {
	struct dlist_head *item = &glob_list;
	uint32_t s_ptr = mem_start;
	struct marea *marea;

	size = MAREA_ALIGN_UP(size);

	do {
		if ((marea = mmap_place_marea(mmap, s_ptr, s_ptr + size, flags))) {
			return marea;
		}

		item = item->next;
		if (item == &glob_list) {
			break;
		}

		marea = dlist_entry(item, struct marea, glob_link);
		s_ptr = marea->end;
	} while(1);

	return NULL;
}

int mmap_inherit(struct emmap *mmap, struct emmap *p_mmap) {
	return 0;
}

static int init() {
	/*
 	 * Initializing this module early was lead to all except 32 pages
	 * belong to mmap_nommu, and I see no reason to do it. Anton Kozlov
	 */

	mem_page_count = 1;

	if (!(mem_start = (uint32_t)phymem_alloc(mem_page_count))) {
		return -ENOMEM;
	}

	mem_end = mem_start + mem_page_count * PAGE_SIZE();

	return 0;
}

static int fini() {
	phymem_free((void *) mem_start, mem_page_count);

	return 0;
}

/* More stubs */
int mmap_kernel_init(void) {
	return 0;
}

int mmap_kernel_inited(void) {
	return 0;
}

struct emmap *mmap_early_emmap(void) {
	return NULL;
}

struct marea *marea_create(uint32_t start, uint32_t end, uint32_t flags, bool is_allocated) {
	return NULL;
}

void marea_destroy(struct marea *marea) {
}

void mmap_add_marea(struct emmap *mmap, struct marea *marea) {
}

void mmap_del_marea(struct marea *marea) {
}

int mmap_do_marea_map(struct emmap *mmap, struct marea *marea) {
	return -1;
}
