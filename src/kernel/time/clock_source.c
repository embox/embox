/**
 * @file
 * @brief
 *
 * @date 19.06.2012
 * @author Alexander Kalmuk
 * @author Roman Kurbatov
 *         - clock_source_get_list() function.
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <util/dlist.h>
#include <util/math.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>

static DLIST_DEFINE(clock_source_list);

extern int clock_tick_init(void);

int clock_source_register(struct clock_source *cs) {
	if (!cs) {
		return -EINVAL;
	}

	dlist_add_prev(dlist_head_init(&cs->lnk), &clock_source_list);

	clock_tick_init();

	jiffies_init();

	return ENOERR;
}

int clock_source_unregister(struct clock_source *cs) {
	if (!cs) {
		return -EINVAL;
	}

	dlist_del(&cs->lnk);

	return ENOERR;
}

struct timespec clock_source_read(struct clock_source *cs) {
	struct timespec ts;
	struct time_event_device *ed;
	struct time_counter_device *cd;
	uint64_t ns = 0;

	ed = cs->event_device;
	if (ed) {
		ns += ((uint64_t) cs->jiffies * NSEC_PER_SEC) / ed->event_hz;
	}

	cd = cs->counter_device;
	if (cd) {
		ns += ((uint64_t) cd->read() * NSEC_PER_SEC) / cd->cycle_hz;
	}

	ts = ns_to_timespec(ns);

	return ts;
}

struct clock_source *clock_source_get_best(enum clock_source_property pr) {
	struct clock_source *cs, *best;
	uint32_t best_resolution = 0;
	uint32_t resolution = 0;

	best = NULL;

	dlist_foreach_entry(cs, &clock_source_list, lnk) {
		switch (pr) {
			case CS_ANY:
			case CS_WITH_IRQ:
				if (cs->event_device) {
					resolution = cs->event_device->event_hz;
				}

				if (pr == CS_ANY && cs->counter_device) {
					resolution = max(resolution, cs->counter_device->cycle_hz);
				}
				if (resolution > best_resolution) {
					best_resolution = resolution;
					best = cs;
				}
			break;

			case CS_WITHOUT_IRQ:
				if (cs->counter_device) {
					resolution = cs->counter_device->cycle_hz;
				}
				if (resolution > best_resolution) {
					best_resolution = resolution;
					best = cs;
				}
			break;
		}
	}

	return best;
}

struct clock_source *clock_source_get_by_name(const char *name) {
	struct clock_source *cs;

	dlist_foreach_entry(cs, &clock_source_list, lnk) {
		if (!strcmp(cs->name, name)) {
			return cs;
		}
	}

	return NULL;
}

time64_t clock_source_get_hwcycles(struct clock_source *cs) {
	int load;

	/* TODO: support for counter-less and event-less clock sources */
	assert(cs->event_device && cs->counter_device);

	load = cs->counter_device->cycle_hz / cs->event_device->event_hz;
	return ((uint64_t) cs->jiffies) * load + cs->counter_device->read();
}
