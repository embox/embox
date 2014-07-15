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

struct time_dev_conf {
	enum {
		HW_TIMER_PERIOD,
		HW_TIMER_ONESHOOT,
	} period_type;
	int counter_period;
	int event_period;
	int mode;
	int irq_nr;
	enum {
		RISING_EDGE,
		FALLING_EDGE,
	} edge_type;
};

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
	int (*config)(struct time_dev_conf *);
	uint32_t event_hz;
	uint32_t irq_nr;
	int (*pending) (unsigned int nr);
	const char *name;
};

/**
 * Time device, that don't generate interrupts. Such device change own state
 * with fixed frequency and we need make request for it state.
 *
 * @param init - init function.
 * @param resolution - number of cycles per second.
 * @param read - return current number of cycles.
 */
struct time_counter_device {
	uint32_t cycle_hz;
	cycle_t (*read)(void);
};

#endif /* KERNEL_TIME_TIME_DEVICE_H_ */
