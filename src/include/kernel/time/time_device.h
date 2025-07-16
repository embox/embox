/**
 * @file
 * @brief Abstraction layer for time device.
 *
 * @date 13.06.2012
 * @author Alexander Kalmuk
 */

#ifndef KERNEL_TIME_TIME_DEVICE_H_
#define KERNEL_TIME_TIME_DEVICE_H_

#include <stdint.h>

#include <kernel/time/time.h>

struct clock_source;

/**
 * Time device, that generate interrupts.
 *
 * @param init - init function. Return 0 on success.
 * @param event_handler - handler called on each event occur.
 * @param set_mode - set mode function.
 * @resolution - number of events per second.
 * @name - name of device
 */
struct time_event_device {
	void (*event_handler)(void);
	int (*set_oneshot)(struct clock_source *cs);
	int (*set_periodic)(struct clock_source *cs);
	int (*set_next_event)(struct clock_source *cs, uint32_t next_event);

	uint32_t flags; /**< periodical or not */
#define CLOCK_EVENT_ONESHOT_MODE  (1 << 0)
#define CLOCK_EVENT_PERIODIC_MODE (1 << 1)
#define CLOCK_EVENT_MODE_MASK \
	(CLOCK_EVENT_ONESHOT_MODE | CLOCK_EVENT_PERIODIC_MODE)

	volatile clock_t jiffies; /**< count of jiffies since event device started */

	uint32_t event_hz;
	uint32_t irq_nr;
	const char *name;
};

struct time_counter_device {
	cycle_t (*get_cycles)(struct clock_source *cs); /* number of cycles since last event */
	uint64_t (*get_time)(struct clock_source *cs); /* time from start of timer */
	uint64_t mask;     /* Maximum value that can be loaded */
	uint32_t cycle_hz; /* number of cycles per second */
};

#endif /* KERNEL_TIME_TIME_DEVICE_H_ */
