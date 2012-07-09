/*
 * @file
 *
 * @date 19.06.2012
 * @author Alexander Kalmuk
 */

#include <util/array.h>
#include <kernel/time/clock_source.h>

ARRAY_SPREAD_DEF(const struct clock_source *, __clock_sources);

uint32_t clock_source_clock_to_sec(struct clock_source *cs, uint32_t ticks) {
	return ticks / cs->event_device->resolution;
}

extern clock_t clock_sys_ticks(void);

ns_t clock_source_read(struct time_event_device *ed, struct time_counter_device *cd) {
		static cycle_t prev_cycles;
		int old_jiffies;
		cycle_t cycles, cycles_all;
		int cycles_per_jiff = cd->resolution /
				ed->resolution;

		do {
			old_jiffies = clock_sys_ticks();
			cycles = cd->read();
		} while(old_jiffies != clock_sys_ticks());

		cycles_all = cycles + old_jiffies * cycles_per_jiff;

		/* TODO cheat. read() will miss for one jiff sometimes. */
		if (cycles_all < prev_cycles) {
			cycles_all += cycles_per_jiff;
		}

		prev_cycles = cycles_all;

		return cycles_to_ns(cd, cycles_all);
}

ns_t clock_source_counter_read(struct time_event_device *ed, struct time_counter_device *cd) {
	return cycles_to_ns(cd, cd->read());
}

const struct clock_source *clock_source_get_best(enum clock_source_property pr) {
	const struct clock_source *cs, *best;
	uint32_t best_resolution, resolution;

	best_resolution = 0;
	best = NULL;

	if (pr != CS_ANY && pr != CS_WITH_IRQ) {
		return NULL;
	}

	array_foreach(cs, __clock_sources, ARRAY_SPREAD_SIZE(__clock_sources)) {
		if (cs->event_device) {
			resolution = cs->event_device->resolution;
		} else if (pr == CS_ANY && cs->counter_device &&
				cs->counter_device->resolution > resolution) {
			resolution = cs->counter_device->resolution;
		}

		if (resolution > best_resolution) {
			best_resolution = resolution;
			best = cs;
		}
	}

	return best;
}
