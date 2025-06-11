/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#ifndef DRIVERS_PWM_PWM_H_
#define DRIVERS_PWM_PWM_H_

#include <stdint.h>

#include <lib/libds/array.h>

struct pwm_device;

struct pwm_ops {
	int (*pwmo_init)(struct pwm_device *dev);
	int (*pwmo_config)(struct pwm_device *dev,  int duty_ns, int period_ns);
	int (*pwmo_enable)(struct pwm_device *dev);
	void (*pwmo_disable)(struct pwm_device *dev);
};

struct pin_description {
	int pd_port;
	int pd_pin;
	int pd_func;
};

struct pwm_device {
	int pwmd_id;
	const struct pwm_ops   *pwmd_ops;
	void                   *pwmd_priv;
	const struct pin_description *pwmd_pin;
	uintptr_t               pwmd_base_addr;
};


extern int pwm_config(struct pwm_device *pwm, int duty_ns, int period_ns);
extern int pwm_enable(struct pwm_device *pwm);
extern void pwm_disable(struct pwm_device *pwm);

extern struct pwm_device *pwm_dev_by_id(int id);

#define PWM_DEV_DEF(id, ops, priv, out_pin, base_addr) \
	ARRAY_SPREAD_DECLARE(const struct pwm_device, __pwm_device_registry); \
	ARRAY_SPREAD_ADD(__pwm_device_registry, { id, ops, priv, out_pin, base_addr } )

#endif /* DRIVERS_PWM_PWM_H_ */
