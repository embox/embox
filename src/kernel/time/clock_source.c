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

#include <util/array.h>
#include <util/dlist.h>
#include <util/math.h>

#include <mem/misc/pool.h>

#include <kernel/panic.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>
#include <module/embox/kernel/time/slowdown.h>

/* XXX used by x86/test/packetdrill */
#define SLOWDOWN_SHIFT OPTION_MODULE_GET(embox__kernel__time__slowdown, NUMBER, shift)

#include <embox/unit.h>

ARRAY_SPREAD_DEF(const struct time_event_device *const, __event_devices);
ARRAY_SPREAD_DEF(const struct time_counter_device *const, __counter_devices);

POOL_DEF(clock_source_pool, struct clock_source_head,
						OPTION_GET(NUMBER, clocks_quantity));

DLIST_DEFINE(clock_source_list);

static struct timespec cs_full_read(struct clock_source *cs);
static struct timespec cs_event_read(struct clock_source *cs);
static struct timespec cs_counter_read(struct clock_source *cs);

static inline cycle_t clock_source_get_jiffies(struct clock_source *cs) {
	return (((cycle_t) cs->jiffies) << SLOWDOWN_SHIFT) + (cycle_t) cs->jiffies_cnt;
}

static struct clock_source_head *clock_source_find(struct clock_source *cs) {
	struct clock_source_head *csh;

	dlist_foreach_entry(csh, &clock_source_list, lnk) {
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

	/* TODO move it to arch dependent code */
	if (cs->counter_device) {
		cs->counter_shift = CS_SHIFT_CONSTANT;
		cs->counter_mult = clock_sourcehz2mult(cs->counter_device->cycle_hz,
				cs->counter_shift);
	}

	dlist_add_prev(dlist_head_init(&csh->lnk), &clock_source_list);

	jiffies_init();

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

struct timespec clock_source_read(struct clock_source *cs) {
	struct timespec ret;
	assert(cs);

	/* See comment to clock_source_read in clock_source.h */
	if (cs->event_device && cs->counter_device) {
		ret = cs_full_read(cs);
	} else if (cs->event_device) {
		ret = cs_event_read(cs);
	} else if (cs->counter_device) {
		ret = cs_counter_read(cs);
	} else {
		panic("all clock sources must have at least one device (event or counter)\n");
	}

	/* Divide ret by (1 << SLOWDOWN_SHIFT) */
	if (SLOWDOWN_SHIFT != 0) {
		uint32_t t;
		struct timespec tmp = {0, 0};

		t = ret.tv_sec % (1 << SLOWDOWN_SHIFT);
		tmp.tv_nsec = ((uint64_t)t * NSEC_PER_SEC) >> SLOWDOWN_SHIFT;
		ret.tv_sec >>= SLOWDOWN_SHIFT;
		ret.tv_nsec >>= SLOWDOWN_SHIFT;
		ret = timespec_add(tmp, ret);
	}

	return ret;
}

static struct timespec cs_full_read(struct clock_source *cs) {
	static cycle_t prev_cycles, cycles, cycles_all;
	int old_jiffies, cycles_per_jiff, safe;
	struct time_event_device *ed;
	struct time_counter_device *cd;
	struct timespec ts;

	assert(cs);

	ed = cs->event_device;
	assert(ed);
	assert(ed->event_hz != 0);

	cd = cs->counter_device;
	assert(cd);
	assert(cd->read);
	assert(cd->cycle_hz != 0);

	cycles_per_jiff = cd->cycle_hz / ed->event_hz;
	safe = 0;

	do {
		old_jiffies = clock_source_get_jiffies(cs);
		cycles = cd->read();
		safe++;
	} while (old_jiffies != clock_source_get_jiffies(cs) && safe < 3);

	if (ed->pending && ed->pending(ed->irq_nr)) {
		old_jiffies++;
	}

	cycles_all = cycles + (time64_t)old_jiffies * cycles_per_jiff;

	/* TODO cheat. read() will miss for one jiff sometimes. */
	if (cycles_all < prev_cycles) {
		cycles_all += cycles_per_jiff;
	}

	prev_cycles = cycles_all;

	ts = cycles32_to_timespec(cycles, cs->counter_mult, cs->counter_shift);
	ts = timespec_add(ts, jiffies_to_timespec(ed->event_hz, old_jiffies));

	return ts;
}

static struct timespec cs_event_read(struct clock_source *cs) {
	return jiffies_to_timespec(cs->event_device->event_hz, clock_source_get_jiffies(cs));
}

static struct timespec cs_counter_read(struct clock_source *cs) {
	return cycles64_to_timespec(cs->counter_device->cycle_hz, cs->counter_device->read());
}

struct clock_source *clock_source_get_best(enum clock_source_property pr) {
	struct clock_source *cs, *best;
	struct clock_source_head *csh;
	uint32_t best_resolution = 0;
	uint32_t resolution = 0;

	best = NULL;

	dlist_foreach_entry(csh, &clock_source_list, lnk) {
		cs = csh->clock_source;

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

struct dlist_head *clock_source_get_list(void) {
	return &clock_source_list;
}

time64_t clock_source_get_hwcycles(struct clock_source *cs) {
	int load;

	/* TODO: support for counter-less and event-less clock sources */
	assert(cs->event_device && cs->counter_device);

	load = cs->counter_device->cycle_hz / cs->event_device->event_hz;
	return ((uint64_t) cs->jiffies) * load + cs->counter_device->read();
}
