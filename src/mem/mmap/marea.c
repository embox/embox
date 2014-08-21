/**
 * @file
 *
 * @date Aug 11, 2014
 * @author: Anton Bondarev
 */

#include <mem/misc/pool.h>
#include <module/embox/mem/mmap_api.h>

//TODO const number of struct marea
POOL_DEF(marea_pool, struct marea, 0x400)

struct marea *marea_alloc(void) {
	return pool_alloc(&marea_pool);
}

void marea_free(struct marea *marea) {
	pool_free(&marea_pool, marea);
}

struct marea *marea_init(struct marea *marea, uint32_t start, uint32_t end, uint32_t flags) {
	marea->start = start;
	marea->end   = end;
	marea->flags = flags;

	dlist_head_init(&marea->mmap_link);

	return marea;
}

struct marea *marea_create(uint32_t start, uint32_t end, uint32_t flags) {
	struct marea *marea;

	if (!(marea = marea_alloc())) {
		return NULL;
	}

	marea->start = start;
	marea->end   = end;
	marea->flags = flags;

	dlist_head_init(&marea->mmap_link);

	return marea;
}


void marea_destroy(struct marea *marea) {
	marea_free(marea);
}
