/**
 * @file
 *
 * @date Aug 11, 2014
 * @author: Anton Bondarev
 */
#include <stdint.h>
#include <stddef.h>

#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <mem/mapping/marea.h>
#include <mem/mapping/mmap.h>

#define MAREA_NUM OPTION_GET(NUMBER, marea_num)

POOL_DEF(marea_pool, struct marea, MAREA_NUM);

struct marea *marea_alloc(uintptr_t start, size_t size, uint32_t flags) {
	struct marea *marea;

	if (!(marea = pool_alloc(&marea_pool))) {
		return NULL;
	}

	marea->start = start;
	marea->size  = size;
	marea->flags = flags;

	dlist_head_init(&marea->mmap_link);

	return marea;
}

void marea_free(struct marea *marea) {
	pool_free(&marea_pool, marea);
}
