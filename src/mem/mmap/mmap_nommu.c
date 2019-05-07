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
#include <mem/mmap.h>
#include <mem/sysmalloc.h>

#define INSIDE(x,a,b)       (((a) <= (x)) && ((x) < (b)))
#define INTERSECT(a,b,c,d)  (INSIDE(a,c,d) || INSIDE(c,a,b))

static const uint32_t mem_start = 0x40000000;
static const uint32_t mem_end = 0xFFFFF000;

void mmap_init(struct emmap *mmap) {
	dlist_init(&mmap->marea_list);
}

void mmap_free(struct emmap *mmap) {
	mmap_clear(mmap);
}

void mmap_clear(struct emmap *mmap) {
	struct marea *marea;

	dlist_foreach_entry(marea, &mmap->marea_list, mmap_link) {
		dlist_del(&marea->mmap_link);

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

	if (!(marea = sysmalloc(sizeof(struct marea)))) {
		return NULL;
	}

	marea->start = start;
	marea->end   = end;
	marea->flags = flags;

	dlist_head_init(&marea->mmap_link);

	dlist_add_prev(&marea->mmap_link, &mmap->marea_list);

	return marea;
}

/* More stubs */
int mmap_kernel_init(void) {
	return 0;
}

int mmap_kernel_inited(void) {
	return 0;
}

struct emmap *mmap_early_emmap(void) {
	static struct emmap early_emmap = {
		NULL,
		0,
		DLIST_INIT(early_emmap.marea_list)
	};

	return &early_emmap;
}

int mmap_do_marea_map(struct emmap *mmap, struct marea *marea) {
	return -1;
}

void mmap_do_marea_unmap(struct emmap *mmap, struct marea *marea) {
}
