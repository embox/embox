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
	CS_WITH_IRQ = 0,
	CS_ANY = 1
};

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

extern ns_t clock_source_read(struct time_event_device *ed, struct time_counter_device *cd);
extern ns_t clock_source_counter_read(struct time_event_device *ed, struct time_counter_device *cd);

#define CLOCK_SOURCE(ts) \
        extern const struct clock_source * __clock_sources[]; \
        ARRAY_SPREAD_ADD(__clock_sources, ts);

#endif /* KERNEL_CLOCK_SOURCE_H_ */
