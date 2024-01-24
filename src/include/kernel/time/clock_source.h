/**
 * @file
 * @brief API for registration of time's device
 *
 * @date 06.07.2011
 * @author Ilia Vaprol
 * @author Roman Kurbatov
 *         - Interface for iterating time devices (clock_source_get_list() and
 *           clock_source_foreach()).
 */

#ifndef KERNEL_CLOCK_SOURCE_H_
#define KERNEL_CLOCK_SOURCE_H_

#include <stdint.h>
#include <lib/libds/dlist.h>
#include <embox/unit.h>
#include <kernel/time/time_device.h>
#include <kernel/time/ktime.h>
#include <kernel/time/time.h>

#include <lib/libds/array.h>

enum clock_source_property {
	CS_WITH_IRQ = 0,
	CS_ANY = 1,
	CS_WITHOUT_IRQ = 2
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

	int (*init)(struct clock_source *cs);

	void *driver_priv_data;

	struct dlist_head lnk;
};

extern struct clock_source *clock_source_get_best(enum clock_source_property property);
extern struct clock_source *clock_source_get_by_name(const char *name);

extern int clock_source_set_oneshot(struct clock_source *cs);
extern int clock_source_set_periodic(struct clock_source *cs, uint32_t hz);
extern int clock_source_set_next_event(struct clock_source *cs,
		uint32_t next_event);

static inline clock_t clock_source_get_cycles(struct clock_source *cs) {
	assert(cs && cs->counter_device);

	return cs->counter_device->read(cs);
}

static inline uint64_t clock_source_cycles2ticks(struct clock_source *cs,
	                                             uint64_t cycles) {
	assert(cs);
	assert(cs->event_device && cs->counter_device);

	return (cycles * cs->event_device->event_hz) / cs->counter_device->cycle_hz;
}

static inline uint64_t clock_source_ticks2cycles(struct clock_source *cs,
	                                             uint64_t ticks) {
	assert(cs);
	assert(cs->event_device && cs->counter_device);

	return (ticks * cs->counter_device->cycle_hz) / cs->event_device->event_hz;
}

/**
 * Read cycles from clock source since moment when it started. This function may be used exactly
 * for three types of clock sources:
 * 1. Clock source, that generates IRQ on each overflow of internal counter (e.g. PIT).
 * 2. Clock source, that generates IRQ with fixed time interval without counter (e.g. ACPI_PM, RTC).
 * 3. Clock source, that did'nt generate IRQ. It is simple counter (e.g. RDTSC).
 * @param cs - clock source read from
 * @return count of nanoseconds from moment when clock source started
 */
//extern time64_t clock_source_counter_read(struct clock_source *cs);

/**
 * Returns a new \ref timespec representing the duration of time since
 * the clock source was started.
 */
extern struct timespec clock_source_read(struct clock_source *cs);

extern int clock_source_register(struct clock_source *cs);
extern int clock_source_unregister(struct clock_source *cs);

extern time64_t clock_source_get_hwcycles(struct clock_source *cs);

static inline uint32_t clock_sourcehz2mult(uint32_t hz, uint32_t shift) {
	uint64_t tmp = ((uint64_t)NSEC_PER_SEC) << shift;
	return tmp / hz;
}

#define CLOCK_SOURCE_NAME(name) \
	MACRO_CONCAT(clock_source_, name)

#define CLOCK_SOURCE_DEF(cs_name, cs_init, cs_data, cs_event, cs_counter) \
	struct clock_source CLOCK_SOURCE_NAME(cs_name) = { \
		.name = #cs_name, \
		.init = cs_init, \
		.event_device = cs_event, \
		.counter_device = cs_counter, \
		.driver_priv_data = cs_data, \
	}; \
	__CLOCK_SOURCE_UNIT_INIT(&CLOCK_SOURCE_NAME(cs_name))

/* Private macro */
#define __CLOCK_SOURCE_UNIT_INIT(cs) \
	static int __clock_source_init(void) { \
		return clock_source_register(cs); \
	} \
	EMBOX_UNIT_INIT(__clock_source_init)

#endif /* KERNEL_CLOCK_SOURCE_H_ */
