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
#include <kernel/time/time_device.h>
#include <kernel/time/time_types.h>
#include <kernel/time/ktime.h>

#include <util/array.h>

enum clock_source_property {
	JIFFIES = 0,
	CYCLES = 1
};

#if 0
struct clock_source {
	const char *name;
	uint32_t flags; /**< periodical or not */
	uint32_t resolution;
	struct clock_event_device *dev;
	struct cyclecounter *cc;
	uint32_t (*read)(void);
};
#endif

/**
 * Time source of hardware time - events and cycles.
 * @param read - return count of ns
 * @param link - link in time sources list
 */
struct clock_source {
	const char *name;
	struct time_event_device *event_device;
	struct time_counter_device *counter_device;
	uint32_t flags; /**< periodical or not */
	ns_t (*read)(struct time_event_device *ed, struct time_counter_device *cd);
};

extern uint32_t clock_source_clock_to_sec(struct clock_source *cs, uint32_t sys_ticks);

static inline ns_t cycles_to_ns(struct time_counter_device *counter, cycle_t cycles) {
	return (cycles * (cycle_t)1000000000) / (cycle_t)counter->resolution;
}

extern const struct clock_source *clock_source_get_best(enum clock_source_property property);

extern void clock_source_init(const struct clock_source *cs);

extern ns_t clock_source_read(struct time_event_device *ed, struct time_counter_device *cd);
extern ns_t clock_source_counter_read(struct time_event_device *ed, struct time_counter_device *cd);

#define CLOCK_SOURCE(ts) \
        extern const struct clock_source * __clock_devices[]; \
        ARRAY_SPREAD_ADD(__clock_devices, ts);


#if 0
struct clock_source_head {
	struct dlist_head lnk;
	struct clock_source *clock_source;
};

extern struct dlist_head clock_source_list;

/* register new clock_source at the system */
extern int clock_source_register(struct clock_source *cs);

/* unregister clock source at the system */
extern int clock_source_unregister(struct clock_source *cs);

extern void clock_source_init(struct clock_source *cs);

extern useconds_t clock_source_clock_to_usec(struct clock_source *cs, clock_t cl);

/* return driver setting */
extern uint32_t clock_source_get_precision(struct clock_source *cs); */

extern struct clock_source *clock_source_get_default(void);

void clock_source_calc_mult_shift(struct clock_source *cs,
		uint32_t freq, uint32_t minsec);
#endif

#endif /* KERNEL_CLOCK_SOURCE_H_ */
