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

int clock_source_set_oneshot(struct clock_source *cs) {
	assert(cs && cs->event_device);

	if (!cs->event_device->set_oneshot) {
		return -1;
	}

	if (!cs->event_device->set_oneshot(cs)) {
		cs->flags &= ~CLOCK_SOURCE_MODE_MASK;
		cs->flags |= CLOCK_SOURCE_ONESHOT_MODE;
		return 0;
	}

	return -1;
}

int clock_source_set_periodic(struct clock_source *cs) {
	assert(cs && cs->event_device);

	if (!cs->event_device->set_periodic) {
		return -1;
	}

	if (!cs->event_device->set_periodic(cs)) {
		cs->flags &= ~CLOCK_SOURCE_MODE_MASK;
		cs->flags |= CLOCK_SOURCE_PERIODIC_MODE;
		return 0;
	}

	return -1;
}

int clock_source_set_next_event(struct clock_source *cs,
		uint32_t next_event) {
	assert(cs && cs->event_device);

	if (!cs->event_device->set_next_event) {
		return -1;
	}

	return cs->event_device->set_next_event(cs, next_event);
}

struct clock_source *clock_source_get_best(enum clock_source_property pr) {
	struct clock_source *cs, *best;
	uint32_t event_hz, cycle_hz, best_hz, hz;

	best_hz = 0;
	best = NULL;

	dlist_foreach_entry(cs, &clock_source_list, lnk) {
		hz = 0;
		event_hz = cs->event_device ? cs->event_device->event_hz : 0;
		cycle_hz = cs->counter_device ? cs->counter_device->cycle_hz : 0;

		switch (pr) {
			case CS_ANY:
				hz = max(event_hz, cycle_hz);
				break;
			case CS_WITH_IRQ:
				hz = event_hz;
				break;
			case CS_WITHOUT_IRQ:
				if (!cs->event_device) {
					hz = cycle_hz;
				}
				break;
		}

		if (hz > best_hz) {
			best_hz = hz;
			best = cs;
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
