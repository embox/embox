/**
 * @file
 * @brief
 *
 * @date 19.06.2012
 * @author Alexander Kalmuk
 * @author Roman Kurbatov
 *         - clock_source_get_list() function.
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>
#include <lib/libds/dlist.h>
#include <util/math.h>
#include "kernel/irq_lock.h"

static DLIST_DEFINE(clock_source_list);

__attribute__((weak)) int monotonic_clock_select(void) {
	return 0;
}

__attribute__((weak)) int realtime_clock_select(void) {
	return 0;
}

int clock_source_register(struct clock_source *cs) {
	struct clock_source *tmp;

	assert(cs);

	dlist_foreach_entry(tmp, &clock_source_list, lnk) {
		if (tmp == cs) {
			/* Already registered, do nothing. */
			return 0;
		}
	}

	if (cs->init) {
		cs->init(cs);
	}

	dlist_add_prev(dlist_head_init(&cs->lnk), &clock_source_list);

	monotonic_clock_select();

	realtime_clock_select();

	return 0;
}

int clock_source_unregister(struct clock_source *cs) {
	if (!cs) {
		return -EINVAL;
	}

	dlist_del(&cs->lnk);

	return ENOERR;
}

struct timespec clock_source_read(struct clock_source *cs) {
	struct time_counter_device *cd;
	struct time_event_device *ed;
	struct timespec ts = {0};

	ed = cs->event_device;
	cd = cs->counter_device;

	if (cd && cd->get_time) {
		ts = ns_to_timespec(cd->get_time(cs));
	}
	else if (ed && (ed->flags & CLOCK_EVENT_PERIODIC_MODE)) {
		cycle_t cycle;
		volatile clock_t jiffies;
		struct timespec jiffies_ts;

		cycle = 0;
		jiffies = ed->jiffies;

		if (cd && cd->get_cycles) {
			do {
				jiffies = ed->jiffies;
				cycle = cd->get_cycles(cs);
			} while (jiffies != ed->jiffies);
			ts = cycles64_to_timespec(cd->cycle_hz, cycle);		
		}

		jiffies_ts = jiffies_to_timespec(ed->event_hz, jiffies);
		ts = timespec_add(ts, jiffies_ts);
	}
	else if (cd && cd->get_cycles) {
		ts = cycles64_to_timespec(cd->cycle_hz, cd->get_cycles(cs));
	}

	return ts;
}

int clock_source_set_oneshot(struct clock_source *cs) {
	assert(cs && cs->event_device);

	if (!cs->event_device->set_oneshot) {
		return -1;
	}

	if (!cs->event_device->set_oneshot(cs)) {
		cs->event_device->flags &= ~CLOCK_EVENT_MODE_MASK;
		cs->event_device->flags |= CLOCK_EVENT_ONESHOT_MODE;
		return 0;
	}

	return -1;
}

int clock_source_set_periodic(struct clock_source *cs, uint32_t hz) {
	assert(cs && cs->event_device);

	if (!cs->event_device->set_periodic) {
		return -1;
	}

	if (cs->event_device->set_periodic(cs) != 0) {
		return -1;
	}

	cs->event_device->flags &= ~CLOCK_EVENT_MODE_MASK;
	cs->event_device->flags |= CLOCK_EVENT_PERIODIC_MODE;

	cs->event_device->event_hz = hz;

	/* FIXME Currently not all clock drivers support counter device. */
	if (cs->counter_device) {
		clock_source_set_next_event(cs, clock_source_ticks2cycles(cs, 1));
	}
	else {
		clock_source_set_next_event(cs, 0);
	}

	return 0;
}

int clock_source_set_next_event(struct clock_source *cs, uint32_t next_event) {
	assert(cs && cs->event_device);

	if (!cs->event_device->set_next_event) {
		return -1;
	}

	return cs->event_device->set_next_event(cs, next_event);
}

struct clock_source *clock_source_get_best(enum clock_source_property pr) {
	struct clock_source *cs = NULL;
	struct clock_source *best;
	int32_t cycle_hz, best_hz, hz;

	best_hz = -1;
	best = NULL;

	dlist_foreach_entry(cs, &clock_source_list, lnk) {
		hz = 0;
		cycle_hz = cs->counter_device ? cs->counter_device->cycle_hz : 0;

		switch (pr) {
		case CS_ANY:
			hz = cycle_hz;
			break;
		case CS_WITH_IRQ:
			if (cs->event_device) {
				hz = cycle_hz;
			}
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
	return ((uint64_t)cs->event_device->jiffies) * load
	       + cs->counter_device->get_cycles(cs);
}
