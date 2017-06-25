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
