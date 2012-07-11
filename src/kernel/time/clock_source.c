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

ns_t clock_source_read(const struct clock_source *cs) {
		static cycle_t prev_cycles, cycles, cycles_all;
		int old_jiffies;
		struct time_event_device *ed = cs->event_device;
		struct time_counter_device *cd = cs->counter_device;

		int cycles_per_jiff = cd->resolution /
				ed->resolution;

		do {
			old_jiffies = clock_sys_ticks();
			cycles = cd->read();
		} while(old_jiffies != clock_sys_ticks());

		if (ed->pending && ed->pending(ed->irq_nr)) {
			old_jiffies++;
		}

		cycles_all = cycles + old_jiffies * cycles_per_jiff;

		/* TODO cheat. read() will miss for one jiff sometimes. */
		if (cycles_all < prev_cycles) {
			cycles_all += cycles_per_jiff;
		}

		prev_cycles = cycles_all;

		return cycles_to_ns(cd, cycles_all);
}

ns_t clock_source_counter_read(const struct clock_source *cs) {
	return cycles_to_ns(cs->counter_device, cs->counter_device->read());
}

const struct clock_source *clock_source_get_best(enum clock_source_property pr) {
	const struct clock_source *cs, *best;
	uint32_t best_resolution = 0;
	uint32_t resolution = 0;

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
