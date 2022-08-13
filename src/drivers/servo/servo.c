/**
 * @file
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 13.08.2022
 */

#include <drivers/servo/servo.h>

ARRAY_SPREAD_DEF(const struct servo_dev, __servo_device_registry);

struct servo_dev *servo_dev_by_id(int id) {
	struct servo_dev *servo_dev;
	int i = 0;

	if (id < 0 || id >= ARRAY_SPREAD_SIZE(__servo_device_registry)) {
		return NULL;
	}

	array_spread_foreach_ptr(servo_dev, __servo_device_registry) {
		if (i++ == id) {
			return servo_dev;
		}
	}

	return NULL;
}
