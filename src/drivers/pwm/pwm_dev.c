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

#include <kernel/time/time.h>

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

static inline int pwm_hz_to_dev(struct pwm_device *pwm, int hz) {
	return (uint64_t)pwm->pwmd_base_freq / hz ;
}

static inline int pwm_ns_to_dev(struct pwm_device *pwm, int ns) {
	return (uint64_t)pwm->pwmd_base_freq * ns / NSEC_PER_SEC ;
}

int pwm_set_frequency(struct pwm_device *pwm, int hz) {
	int err;

	if (pwm == NULL) {
		return -EINVAL;
	}

	if (hz  < 1) {
		return -EINVAL;
	}

	if (pwm->pwmd_ops == NULL || pwm->pwmd_ops->pwmo_set_period == NULL) {
		return -ENOSUPP;
	}

    err = pwm->pwmd_ops->pwmo_set_period(pwm, pwm_hz_to_dev(pwm, hz));
	//err = pwm->pwmd_ops->pwmo_set_period(pwm, period_ns);
	if (err) {
		return err;
	}

	pwm->pwmd_period = NSEC_PER_SEC / hz;

	return 0;
}

int pwm_set_period(struct pwm_device *pwm, int period_ns) {
	int err;

	if (pwm == NULL) {
		return -EINVAL;
	}

	if (period_ns  < 1) {
		return -EINVAL;
	}

	if (pwm->pwmd_ops == NULL || pwm->pwmd_ops->pwmo_set_period == NULL) {
		return -ENOSUPP;
	}

    err = pwm->pwmd_ops->pwmo_set_period(pwm, pwm_ns_to_dev(pwm, period_ns));
	//err = pwm->pwmd_ops->pwmo_set_period(pwm, period_ns);
	if (err) {
		return err;
	}

	pwm->pwmd_period = period_ns;

	return 0;
}

int pwm_set_duty(struct pwm_device *pwm, int chan_num, int duty_ns)  {
	int err;

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

    err = pwm->pwmd_ops->pwmo_set_duty(pwm, chan_num, pwm_ns_to_dev(pwm, duty_ns));
	//err = pwm->pwmd_ops->pwmo_set_duty(pwm, chan_num, duty_ns);
	if (err) {
		return err;
	}

	pwm->pwmd_duty[chan_num] = duty_ns;

	return 0;
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