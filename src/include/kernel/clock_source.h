/**
 * @file
 * @brief API for registration of time's device
 *
 * @date 06.07.2011
 * @author Ilia Vaprol
 */

#ifndef KERNEL_CLOCK_SOURCE_H_
#define KERNEL_CLOCK_SOURCE_H_

#include <types.h>
#include <util/dlist.h>
//#include <kernel/ktime.h>
#include <kernel/clock_event.h>

typedef uint64_t cycle_t;

/*
* Hardware abstraction for a free running counter.
* @param read  - returns the current cycle value
* @param mult  - cycle to nanosecond multiplier
* @param shift - cycle to nanosecond divisor (power of two)
*/
struct cyclecounter {
	cycle_t (*read)(const struct cyclecounter *cc);
#if 0
	cycle_t mask;
#endif
	uint32_t mult;
	uint32_t shift;
};

struct clock_source {
	const char *name;
	uint32_t flags; /**< periodical or not */
	uint32_t resolution;
	struct clock_event_device *dev;
	struct cyclecounter *cc;
	uint32_t (*read)(void);
};

struct clock_source_head {
	struct dlist_head lnk;
	struct clock_source *clock_source;
};

extern struct dlist_head clock_source_list;

/* register new clock_source at the system */
extern int clock_source_register(struct clock_source *cs);

/* unregister clock source at the system */
extern int clock_source_unregister(struct clock_source *cs);

/* return driver setting */
extern uint32_t clock_source_get_precision(struct clock_source *cs);

/* convert clocks to microseconds */
extern useconds_t clock_source_clock_to_usec(struct clock_source *cs, clock_t cl);

/* convert clocks to seconds */
extern int clock_source_clock_to_sec(struct clock_source *cs, clock_t cl);

extern struct clock_source *clock_source_get_default(void);

void clock_source_calc_mult_shift(struct clock_source *cs,
		uint32_t freq, uint32_t minsec);

#endif /* KERNEL_CLOCK_SOURCE_H_ */
