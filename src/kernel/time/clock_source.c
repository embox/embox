/**
 * @file
 * @brief
 *
 * @date 19.06.2012
 * @author Alexander Kalmuk
 */

#include <errno.h>

#include <util/array.h>
#include <util/dlist.h>
#include <util/math.h>

#include <mem/misc/pool.h>

#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>

#include <embox/unit.h>

ARRAY_SPREAD_DEF(const struct time_event_device *, __event_devices);
ARRAY_SPREAD_DEF(const struct time_counter_device *, __counter_devices);

POOL_DEF(clock_source_pool, struct clock_source_head,
						OPTION_GET(NUMBER, clocks_quantity));

DLIST_DEFINE(clock_source_list);

static time64_t cs_full_read(struct clock_source *cs);
static time64_t cs_event_read(struct clock_source *cs);
static time64_t cs_counter_read(struct clock_source *cs);

static struct clock_source_head *clock_source_find(struct clock_source *cs) {
	struct clock_source_head *csh;

	struct dlist_head *tmp, *csh_lnk;

	dlist_foreach(csh_lnk,tmp,&clock_source_list) {
		csh = dlist_entry(csh_lnk, struct clock_source_head, lnk);
		if (cs == csh->clock_source) {
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
	if (NULL == (csh = clock_source_find(cs))) {
		return -EBADF;
	}

	return ENOERR;
}

time64_t clock_source_read(struct clock_source *cs) {
	assert(cs);

	/* See comment to clock_source_read in clock_source.h */
	if (cs->event_device && cs->counter_device) {
		return cs_full_read(cs);
	} else if (cs->event_device) {
		return cs_event_read(cs);
	} else if (cs->counter_device) {
		return cs_counter_read(cs);
	}

	/* All clock sources must have at least one device (event device or counter ddevice).
	 * So if we are here, it is error */
	assert(0);

	return 0;
}

static time64_t cs_full_read(struct clock_source *cs) {
	static cycle_t prev_cycles, cycles, cycles_all;
	int old_jiffies, safe;
	struct time_event_device *ed = cs->event_device;
	struct time_counter_device *cd = cs->counter_device;

	int cycles_per_jiff = cd->resolution / ed->resolution;
	safe = 0;

	do {
		old_jiffies = cs->jiffies;
		cycles = cd->read();
		safe++;
	} while (old_jiffies != cs->jiffies && safe < 3);

	if (ed->pending && ed->pending(ed->irq_nr)) {
		old_jiffies++;
	}

	cycles_all = cycles + (time64_t)old_jiffies * cycles_per_jiff;

	/* TODO cheat. read() will miss for one jiff sometimes. */
	if (cycles_all < prev_cycles) {
		cycles_all += cycles_per_jiff;
	}

	prev_cycles = cycles_all;

	return cycles_to_ns(cd->resolution, cycles_all);
}

static time64_t cs_event_read(struct clock_source *cs) {
	return cycles_to_ns(cs->event_device->resolution, (cycle_t)cs->jiffies);
}

static time64_t cs_counter_read(struct clock_source *cs) {
	return cycles_to_ns(cs->counter_device->resolution, cs->counter_device->read());
}

struct clock_source *clock_source_get_best(enum clock_source_property pr) {
	struct clock_source *cs, *best;
	struct clock_source_head *csh;
	struct dlist_head *tmp, *csh_lnk;
	uint32_t best_resolution = 0;
	uint32_t resolution = 0;

	best = NULL;

	dlist_foreach(csh_lnk,tmp,&clock_source_list) {
		csh = dlist_entry(csh_lnk, struct clock_source_head, lnk);
		cs = csh->clock_source;

		if (cs->event_device) {
			resolution = cs->event_device->resolution;
		}

		if (pr == CS_ANY && cs->counter_device) {
			resolution = max(resolution, cs->counter_device->resolution);
		}

		if (resolution > best_resolution) {
			best_resolution = resolution;
			best = cs;
		}
	}

	return best;
}
