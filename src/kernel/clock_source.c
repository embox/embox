/**
 * @file
 * @brief
 *
 * @date 06.07.2011
 * @author Ilia Vaprol
 */

#include <kernel/clock_source.h>
#include <lib/list.h>
#include <util/pool.h>
#include <types.h>
#include <time.h>
#include <assert.h>

struct clock_source {
	struct list_head *next, *prev;
	uint32_t flags; // periodical or not
	uint32_t precision;
//	struct list_head *timers_list;
};

#define CLOCK_SOURCE_POOL_SZ 4

POOL_DEF(clock_source_pool, struct clock_source, CLOCK_SOURCE_POOL_SZ);
static LIST_HEAD(clock_source_list);

void clock_source_register(struct clock_source **pcs, uint32_t flags, uint32_t precision) {
	struct clock_source *new_cs;

	if (pcs) {
		if ((new_cs = (struct clock_source *) pool_alloc(&clock_source_pool))) {
			new_cs->flags      = flags;
			new_cs->precision  = precision;
			list_add_tail((struct list_head *) new_cs, &clock_source_list);
			*pcs = new_cs;
		}
		else {
			*pcs = NULL;
		}
	}
}

uint32_t clock_source_get_HZ(void) {
	assert(!list_empty(&clock_source_list));
	return ((struct clock_source *)clock_source_list.next)->precision;
}

useconds_t clock_source_clock_to_usec(clock_t cl) {
	return ((useconds_t) cl) * ((struct clock_source *)clock_source_list.next)->precision;
}
