/**
 * @file
 * @brief
 *
 * @date 16.05.2012
 * @author Alexander Kalmuk
 */

#include <util/array.h>
#include <string.h>
#include <kernel/clock_event.h>

ARRAY_SPREAD_DEF(const struct clock_event_device *, __clock_devices);

const struct clock_event_device *get_timer_by_name(const char *name) {
	const struct clock_event_device *dev;

	array_foreach(dev, __clock_devices, ARRAY_SPREAD_SIZE(__clock_devices)) {
		if (0 == strcmp(dev->name, name)) {
			return dev;
		}
	}

	return NULL;
}

