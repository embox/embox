/**
 * @file
 * @brief Hardware timers abstraction.
 *
 * @date 16.05.2012
 * @author Alexander Kalmuk
 */

#ifndef KERNEL_CLOCK_EVENT_H_
#define KERNEL_CLOCK_EVENT_H_

#include <kernel/clock_source.h>
#include <kernel/ktime.h>

/* timer's modes */
#define CLOCK_EVT_MODE_ONESHOT  0
#define CLOCK_EVT_MODE_PERIODIC 1

enum resolution_mode {
	JIFFIES = 0,
	TICKS = 1
};

/**
 * Abstraction for hardware clock sources.
 * Every hw timer may have:
 * 		- init function to initialize timer's internals
 * 		  such as clock source, control registers and so on.
 * 		  Suppose that this function called only once.
 * 		- set_mode function to set mode
 */
struct clock_event_device {
	void (*set_mode)(uint32_t mode); /* XXX may be mode must be enum?? */
	uint32_t mode;
	uint32_t resolution; /* jiffies per second */
	struct clock_source *cs;
	const char *name;
};

static inline void clock_events_calc_mult_shift(const struct clock_event_device *ce,
		uint32_t freq, uint32_t minsec) {
	return clocks_calc_mult_shift(&ce->cs->cc->mult, &ce->cs->cc->shift, freq, NSEC_PER_SEC,
				      minsec);
}

extern const struct clock_event_device *cedev_get_by_name(const char *name);
extern const struct clock_event_device *cedev_get_best(enum resolution_mode mode);

#define CLOCK_EVENT_DEVICE(cedev) \
	extern const struct clock_event_device * __clock_devices[]; \
	ARRAY_SPREAD_ADD(__clock_devices, cedev);

#endif /* KERNEL_CLOCK_EVENT_H_ */
