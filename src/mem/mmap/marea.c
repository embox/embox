/**
 * @file
 *
 * @date Aug 11, 2014
 * @author: Anton Bondarev
 */

#include <mem/misc/pool.h>
#include <mem/mapping/marea.h>
#include <module/embox/mem/mmap_api.h>

//TODO const number of struct marea
POOL_DEF(marea_pool, struct marea, 0x400)

POOL_DEF(phy_page_pool, struct phy_page, 0xFFFF)

struct marea *marea_create(uint32_t start, uint32_t end, uint32_t flags, bool is_allocated) {
	struct marea *marea;

	if (!(marea = pool_alloc(&marea_pool))) {
		return NULL;
	}

	marea->start = start;
	marea->end   = end;
	marea->flags = flags;
	marea->is_allocated = is_allocated;

	dlist_head_init(&marea->mmap_link);

	return marea;
}


void marea_destroy(struct marea *marea) {
	pool_free(&marea_pool, marea);
}

struct phy_page *phy_page_create(void *page, size_t page_number) {
	struct phy_page *phy_page;

	if (!(phy_page = pool_alloc(&phy_page_pool))) {
		return NULL;
	}

	phy_page->page = page;
	phy_page->page_number = page_number;

	dlist_head_init(&phy_page->page_link);

	return phy_page;
}

void phy_page_destroy(struct phy_page *phy_page) {
	pool_free(&phy_page_pool, phy_page);
}
