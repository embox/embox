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

const struct clock_event_device *cedev_get_by_name(const char *name) {
	const struct clock_event_device *dev;

	array_foreach(dev, __clock_devices, ARRAY_SPREAD_SIZE(__clock_devices)) {
		if (0 == strcmp(dev->name, name)) {
			return dev;
		}
	}

	return NULL;
}

const struct clock_event_device *cedev_get_best(void) {
	const struct clock_event_device *dev, *best;
	int best_resolution = 0;

	array_foreach(dev, __clock_devices, ARRAY_SPREAD_SIZE(__clock_devices)) {
		if (dev->resolution > best_resolution) {
			best_resolution = dev->resolution;
			best = dev;
		}
	}

	return best;
}

