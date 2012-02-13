/**
 * @file
 * @brief API for registration of time's device
 *
 * @date 06.07.11
 * @author Ilia Vaprol
 */

#include <kernel/clock_source.h>
#include <lib/list.h>
#include <mem/misc/pool.h>
#include <types.h>
#include <time.h>
#include <assert.h>

#define CLOCK_SOURCE_POOL_SZ 4

POOL_DEF(clock_source_pool, struct clock_source_head, CLOCK_SOURCE_POOL_SZ);
LIST_HEAD(clock_source_list);

int clock_source_register(struct clock_source *cs) {
	struct clock_source_head *csh;

	if (!cs) {
		return -1;
	}
	csh = (struct clock_source_head *) pool_alloc(&clock_source_pool);
	if (!csh) {
		return -1;
	}
	csh->clock_source = cs;
	list_add_tail(&csh->lnk, &clock_source_list);
	return 0;
}

uint32_t clock_source_get_precision(void) {
	struct clock_source_head *csh;

	assert(!list_empty(&clock_source_list));
	csh = (struct clock_source_head *) clock_source_list.next;

	return (uint32_t) csh->clock_source->precision;
}

useconds_t clock_source_clock_to_usec(clock_t cl) {
	struct clock_source_head *csh;

	assert(!list_empty(&clock_source_list));
	csh = (struct clock_source_head *) clock_source_list.next;

	return (useconds_t) (((useconds_t) cl) * csh->clock_source->precision);
}
