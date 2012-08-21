/*
 * @brief Entry point for time device. Read time in device specified units
 * to pass above.
 *
 * @date 13.06.2012
 * @author Alexander Kalmuk
 */

#ifndef KERNEL_TIME_TIME_SOURCE_H_
#define KERNEL_TIME_TIME_SOURCE_H_

#include <lib/list.h>
#include <kernel/time/ktime.h>
#include <kernel/time/time_device.h>

/**
 * Time source of hardware time - events and cycles.
 * @param read - store count of events and cycles elapsed since last event.
 * @param link - link in time sources list
 */
struct time_source {
	struct time_event_device *event_device;
	struct time_counter_device *counter;
	void (*read)(struct timehw *thw);
	struct list_head *link;
	const char *name;
};

#endif /* KERNEL_TIME_TIME_SOURCE_H_ */
