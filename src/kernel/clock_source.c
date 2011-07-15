/**
 * @file
 * @brief
 *
 * @date 06.07.2011
 * @author Ilia Vaprol
 */

#include <kernel/clock_source.h>
#include <lib/list.h>
#include <mem/misc/pool.h>
#include <types.h>
#include <time.h>
#include <assert.h>

struct clock_source_head {
	struct list_head *next, *prev;
	struct clock_source *clock_source;
};

#define CLOCK_SOURCE_POOL_SZ 4

POOL_DEF(clock_source_pool, struct clock_source_head, CLOCK_SOURCE_POOL_SZ);
static LIST_HEAD(clock_source_list);

int clock_source_register(struct clock_source *cs) {
	struct clock_source_head *csh;

	if (!cs) {
		return 1;
	}
	if (!(csh = (struct clock_source_head *) pool_alloc(&clock_source_pool))) {
		return 1;
	}
	csh->clock_source = cs;
	list_add_tail((struct list_head *) csh, &clock_source_list);
	return 0;
}

uint32_t clock_source_get_HZ(void) {
	assert(!list_empty(&clock_source_list));
	return (uint32_t) (((struct clock_source_head *)clock_source_list.next)->clock_source->precision);
}

useconds_t clock_source_clock_to_usec(clock_t cl) {
	assert(!list_empty(&clock_source_list));
	return (useconds_t) (((useconds_t) cl) * ((struct clock_source_head *)clock_source_list.next)->clock_source->precision);
}

struct list_head * clock_source_get_timers_list(void) {
	assert(!list_empty(&clock_source_list));
	return ((struct clock_source_head *)clock_source_list.next)->clock_source->timers_list;
}
