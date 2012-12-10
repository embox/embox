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
	volatile uint32_t jiffies; /* count of jiffies since clock source started */
	uint32_t flags; /**< periodical or not */
	ns_t (*read)(struct clock_source *cs);
};

static inline ns_t clock_source_clock_to_ns(struct clock_source *cs, clock_t ticks) {
	assert(cs && cs->event_device);
	return (ticks * NSEC_PER_SEC) / cs->event_device->resolution;
}

static inline clock_t clock_source_ns_to_clock(struct clock_source *cs, ns_t ns) {
	assert(cs && cs->event_device);
	return (ns * cs->event_device->resolution) / NSEC_PER_SEC;
}

static inline ns_t cycles_to_ns(uint32_t resolution, cycle_t cycles) {
	return (cycles * (cycle_t)1000000000) / (uint64_t)resolution;
}

extern struct clock_source *clock_source_get_best(enum clock_source_property property);

/**
 * Read cycles from clock source since moment when it started. This function may be used exactly
 * for three types of clock sources:
 * 1. Clock source, that generates IRQ on each overflow of internal counter (e.g. PIT).
 * 2. Clock source, that generates IRQ with fixed time interval without counter (e.g. ACPI_PM, RTC).
 * 3. Clock source, that did'nt generate IRQ. It is simple counter (e.g. RDTSC).
 * @param cs - clock source read from
 * @return count of nanoseconds from moment when clock source started
 */
extern ns_t clock_source_read(struct clock_source *cs);
extern ns_t clock_source_counter_read(struct clock_source *cs);

extern int clock_source_register(struct clock_source *cs);
extern int clock_source_unregister(struct clock_source *cs);

struct clock_source_head {
	struct dlist_head lnk;
	struct clock_source *clock_source;
};

#define TIME_EVENT_DEVICE(ted) \
        extern const struct time_event_device * __event_devices[]; \
        ARRAY_SPREAD_ADD(__event_devices, ted);

#define TIME_COUNTER_DEVICE(tcd) \
        extern const struct time_counter_device * __counter_devices[]; \
        ARRAY_SPREAD_ADD(__counter_devices, tcd);

#endif /* KERNEL_CLOCK_SOURCE_H_ */
