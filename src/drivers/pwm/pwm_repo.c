/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#include <stddef.h>

#include <lib/libds/array.h>

#include <drivers/pwm.h>

ARRAY_SPREAD_DEF(const struct pwm_device, __pwm_device_registry);

struct pwm_device *servo_dev_by_id(int id) {
	struct pwm_device *pwm_dev;

	if (id < 0 || id >= ARRAY_SPREAD_SIZE(__pwm_device_registry)) {
		return NULL;
	}

	array_spread_foreach_ptr(pwm_dev, __pwm_device_registry) {
		if (pwm_dev->pwmd_id == id) {
			return pwm_dev;
		}
	}

	return NULL;
}
