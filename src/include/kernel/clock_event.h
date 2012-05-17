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

#define CLOCK_EVT_MODE_DEFAULT 0

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
	struct clock_source *cs;
	const char *name;
};

static inline void clock_events_calc_mult_shift(const struct clock_event_device *ce,
		uint32_t freq, uint32_t minsec) {
	return clocks_calc_mult_shift(&ce->cs->cc->mult, &ce->cs->cc->shift, NSEC_PER_SEC,
				      freq, minsec);
}

extern const struct clock_event_device *get_timer_by_name(const char *name);

#endif /* KERNEL_CLOCK_EVENT_H_ */
