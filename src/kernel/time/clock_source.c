/**
 * @file
 * @brief
 *
 * @date 19.06.2012
 * @author Alexander Kalmuk
 */

#include <util/array.h>
#include <kernel/time/clock_source.h>
#include <mem/misc/pool.h>
#include <util/dlist.h>
#include <errno.h>
#include <embox/unit.h>

ARRAY_SPREAD_DEF(const struct time_event_device *, __event_devices);
ARRAY_SPREAD_DEF(const struct time_counter_device *, __counter_devices);

POOL_DEF(clock_source_pool, struct clock_source_head, OPTION_GET(NUMBER, clocks_quantity));
DLIST_DEFINE(clock_source_list);

ns_t clock_source_clock_to_ns(struct clock_source *cs, clock_t ticks) {
	assert(cs && cs->event_device);
	return (ticks * NSEC_PER_SEC) / cs->event_device->resolution;
}

clock_t clock_source_ns_to_clock(struct clock_source *cs, ns_t ns) {
	assert(cs && cs->event_device);
	return (ns * cs->event_device->resolution) / NSEC_PER_SEC;
}

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

extern clock_t clock_sys_ticks(void);

ns_t clock_source_read(struct clock_source *cs) {
		static cycle_t prev_cycles, cycles, cycles_all;
		int old_jiffies, safe;
		struct time_event_device *ed = cs->event_device;
		struct time_counter_device *cd = cs->counter_device;

		int cycles_per_jiff = cd->resolution /
				ed->resolution;
		safe = 0;

		do {
			old_jiffies = clock_sys_ticks();
			cycles = cd->read();
			safe++;
		} while(old_jiffies != clock_sys_ticks() && safe < 3);

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

ns_t clock_source_counter_read(struct clock_source *cs) {
	return cycles_to_ns(cs->counter_device, cs->counter_device->read());
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
