/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */
#include <util/log.h>

#include <lib/libds/array.h>

#include <stddef.h>

#include <drivers/pwm.h>

#include <embox/unit.h>

ARRAY_SPREAD_DECLARE(const struct pwm_device, __pwm_device_registry);

EMBOX_UNIT_INIT(pwm_subsystem_init);

static int pwm_subsystem_init(void) {
	struct pwm_device *pwm_dev;

	array_spread_foreach_ptr(pwm_dev, __pwm_device_registry) {
		assert(pwm_dev);
		if (!pwm_dev->pwmd_ops) {
			continue;
		}

		if (pwm_dev->pwmd_ops->pwmo_init) {
			pwm_dev->pwmd_ops->pwmo_init(pwm_dev);
		} else {
			log_warning("PWM%d has no pwmo_init funcion", pwm_dev->pwmd_id);
		}
	}

	return 0;
}

struct pwm_device *pwm_dev_by_id(int id) {
	struct pwm_device *pwm_dev;

	array_spread_foreach_ptr(pwm_dev, __pwm_device_registry) {
		if (id == pwm_dev->pwmd_id) {
			return pwm_dev;
		}
	}

	return 0;
}

int pwm_config(struct pwm_device *pwm, int duty_ns, int period_ns) {
    return 0;
}

int pwm_enable(struct pwm_device *pwm) {
    return 0;
}

void pwm_disable(struct pwm_device *pwm) {
}