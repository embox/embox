/*
 * @file time_source.c
 *
 * @date 19.06.2012
 * @author Alexander Kalmuk
 */

#include <util/array.h>
#include <kernel/clock_source.h>

ARRAY_SPREAD_DEF(const struct clock_source *, __clock_devices);

uint32_t clock_source_clock_to_sec(struct clock_source *cs, uint32_t ticks) {
	return ticks / cs->event_device->resolution;
}

void clock_source_init(const struct clock_source *cs) {
	if (cs->event_device && cs->event_device->init) {
		cs->event_device->init();
	}

	if (cs->event_device && cs->event_device->init) {
		cs->event_device->init();
	}
}

ns_t clock_source_read(struct time_event_device *ed, struct time_counter_device *cd) {
		int old_jiffies;
		cycle_t cycles, cycles_all;
		int cycles_per_jiff = cd->resolution /
				ed->resolution;

		do {
			old_jiffies = ed->jiffies;
			cycles = cd->read();
		} while(old_jiffies != ed->jiffies);

		cycles_all = cycles + old_jiffies * cycles_per_jiff;

		return cycles_to_ns(cd, cycles_all);
}

ns_t clock_source_counter_read(struct time_event_device *ed, struct time_counter_device *cd) {
	return cycles_to_ns(cd, cd->read());
}

const struct clock_source *clock_source_get_best(enum clock_source_property pr) {
	const struct clock_source *cs, *best;
	int best_resolution;

	best_resolution = 0;
	best = NULL;

	switch(pr) {
	case CYCLES:
		array_foreach(cs, __clock_devices, ARRAY_SPREAD_SIZE(__clock_devices)) {
			if (cs->counter_device && cs->counter_device->resolution > best_resolution) {
				best_resolution = cs->counter_device->resolution;
				best = cs;
			}
		}
		break;
	case JIFFIES:
		array_foreach(cs, __clock_devices, ARRAY_SPREAD_SIZE(__clock_devices)) {
			if (cs->event_device && cs->event_device->resolution > best_resolution) {
				best_resolution = cs->event_device->resolution;
				best = cs;
			}
		}
		break;
	default:
		break;
	}

	return best;
}
