/**
 * @file
 * @brief Hardware timers abstraction.
 *
 * @date 16.05.2012
 * @author Alexander Kalmuk
 */

#ifndef KERNEL_CLOCK_EVENT_H_
#define KERNEL_CLOCK_EVENT_H_

#if 0
#include <kernel/clock_source.h>
#include <util/array.h>
//#include <kernel/ktime.h>
#include <stdint.h>

/* timer's modes */
#define CLOCK_EVT_MODE_ONESHOT  0
#define CLOCK_EVT_MODE_PERIODIC 1

/**
 * Abstraction for hardware clock sources.
 * Every hw timer may have:
 * 		- init function to initialize timer's internals
 * 		  such as clock source, control registers and so on.
 * 		  Suppose that this function called only once.
 * 		- set_mode function to set mode
 */
struct clock_event_device {
	void (*set_mode)(uint32_t mode);
	int (*init)(void);
	uint32_t mode;
	uint32_t resolution; /* jiffies per second */
	struct clock_source *cs; /* back link to clock source */
	//uint32_t (*get_jiffies)(void); /* current count of jiffies */
	const char *name;
};

extern const struct clock_event_device *cedev_get_by_name(const char *name);
extern const struct clock_event_device *cedev_get_best(enum resolution_mode mode);

#define CLOCK_EVENT_DEVICE(cedev) \
	extern const struct clock_event_device * __clock_devices[]; \
	ARRAY_SPREAD_ADD(__clock_devices, cedev);
#endif

#endif /* KERNEL_CLOCK_EVENT_H_ */
