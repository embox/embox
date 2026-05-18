/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */
#include <util/log.h>

#include <lib/libds/array.h>

#include <errno.h>
#include <stddef.h>

#include <drivers/pin_description.h>
#include <drivers/pwm.h>

#include <embox/unit.h>

ARRAY_SPREAD_DECLARE(struct pwm_device *, __pwm_device_registry);

EMBOX_UNIT_INIT(pwm_subsystem_init);

static int pwm_subsystem_init(void) {
	struct pwm_device *pwm_dev;

	array_spread_foreach(pwm_dev, __pwm_device_registry) {
		assert(pwm_dev);

		pwm_dev->pwmd_ops = pwm_dev->pwmd_desc->pwmd_ops;
		pwm_dev->pwmd_id = pwm_dev->pwmd_desc->pwmd_id;
		pwm_dev->pwmd_priv = pwm_dev->pwmd_desc->pwmd_priv;

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

struct pwm_device*pwm_dev_by_id(int id) {
	struct pwm_device *pwm_dev;

	array_spread_foreach(pwm_dev, __pwm_device_registry) {
		if (id == pwm_dev->pwmd_id) {
			return pwm_dev;
		}
	}

	return 0;
}

int pwm_set_period(struct pwm_device *pwm, int period_ns) {
	if (pwm == NULL) {
		return -EINVAL;
	}
	if (period_ns  < 1) {
		return -EINVAL;
	}

	if (pwm->pwmd_ops == NULL || pwm->pwmd_ops->pwmo_set_period == NULL) {
		return -ENOSUPP;
	}

    return pwm->pwmd_ops->pwmo_set_period(pwm, period_ns);
}

int pwm_set_duty(struct pwm_device *pwm, int chan_num, int duty_ns)  {
	if (pwm == NULL) {
		return -EINVAL;
	}
	if (duty_ns < 2) {
		return -EINVAL;
	}

	if (pwm->pwmd_ops == NULL || pwm->pwmd_ops->pwmo_set_duty == NULL) {
		return -ENOSUPP;
	}

	if (pwm_dev_max_chan(pwm) < chan_num) {
		return -EINVAL;
	}

    return pwm->pwmd_ops->pwmo_set_duty(pwm, chan_num, duty_ns);
}

int pwm_enable(struct pwm_device *pwm, uint32_t chan_mask) {
	if (pwm == NULL) {
		return -EINVAL;
	}
	if (pwm->pwmd_ops == NULL || pwm->pwmd_ops->pwmo_enable == NULL) {
		return -ENOSUPP;
	}
    return pwm->pwmd_ops->pwmo_enable(pwm, chan_mask);
}

void pwm_disable(struct pwm_device *pwm, uint32_t chan_mask) {
	if (pwm == NULL) {
		return;
	}
	if (pwm->pwmd_ops == NULL || pwm->pwmd_ops->pwmo_disable == NULL) {
		return;
	}
	pwm->pwmd_ops->pwmo_disable(pwm, chan_mask);
}