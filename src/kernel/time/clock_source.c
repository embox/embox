/**
 * @file
 * @brief API for registration of time's device
 *
 * @date 06.07.11
 * @author Ilia Vaprol
 * @author Alexander Kalmuk
 *         - timecounter
 */

#include <types.h>
#include <time.h> // posix
#include <assert.h>
#include <errno.h>

#include <embox/unit.h>

#include <util/dlist.h>
#include <mem/misc/pool.h>

#include <kernel/time.h>

#include <kernel/clock_source.h>
#include <kernel/ktime.h>
#include <kernel/time/timecounter.h>

POOL_DEF(clock_source_pool, struct clock_source_head, OPTION_GET(NUMBER,clocks_quantity));
DLIST_DEFINE(clock_source_list);

static struct clock_source_head *clock_source_find(struct clock_source *cs) {
	struct clock_source_head *csh;

	struct dlist_head *tmp, *csh_lnk;

	dlist_foreach(csh_lnk,tmp,&clock_source_list) {
		csh = dlist_entry(csh_lnk, struct clock_source_head, lnk);
		if(cs == csh->clock_source) {
			return csh;
		}
	}

	return NULL;
}

int clock_source_register(struct clock_source *cs) {
	struct clock_source_head *csh;

	if (!cs) {
		return -EINVAL;
	}
	csh = (struct clock_source_head *) pool_alloc(&clock_source_pool);
	if (!csh) {
		return -EBUSY;
	}
	csh->clock_source = cs;

	dlist_add_prev(dlist_head_init(&csh->lnk), &clock_source_list);

	return ENOERR;
}

int clock_source_unregister(struct clock_source *cs) {
	struct clock_source_head *csh;

	if (!cs) {
		return -EINVAL;
	}
	if(NULL == (csh = clock_source_find(cs))) {
		return -EBADF;
	}

	return ENOERR;
}


uint32_t clock_source_get_precision(struct clock_source *cs) {
	return (uint32_t) cs->resolution;
}

struct clock_source *clock_source_get_default(void) {
	assert(!dlist_empty(&clock_source_list));

	return ((struct clock_source_head *)clock_source_list.next)->clock_source;
}

useconds_t clock_source_clock_to_usec(struct clock_source *cs, clock_t cl) {
	return (useconds_t) (((useconds_t) cl) * cs->resolution);
}

void clocks_calc_mult_shift(uint32_t *mult, uint32_t *shift, uint32_t from,
		uint32_t to, uint32_t maxsec) {
	*mult = to / from;
	*shift = 0;
}

static useconds_t time_usec(void) {
	return clock_source_clock_to_usec(clock_source_get_default(), clock());
}

struct ktimeval * get_timeval(struct ktimeval *tv) {
	useconds_t usec;

	usec = time_usec();
	tv->tv_sec = usec / USEC_PER_SEC;
	tv->tv_usec = usec % USEC_PER_SEC;
	return tv;
}
