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
#include <sys/cdefs.h>

#include <lib/libds/array_spread.h>

#include <drivers/pin_description.h>

struct pwm_device;

struct pwm_ops {
	int  (*pwmo_init)(struct pwm_device *dev);
	int  (*pwmo_set_period)(struct pwm_device *dev, int period_ns);
	int  (*pwmo_set_duty)(struct pwm_device *dev,  int chan_num, int duty_ns);
	int  (*pwmo_enable)(struct pwm_device *dev, uint32_t chan_mask);
	void (*pwmo_disable)(struct pwm_device *dev, uint32_t chan_mask);
};

struct pwm_desc {
	int                           pwmd_id;
	const struct pwm_ops         *pwmd_ops;
	void                         *pwmd_priv;
	const struct pin_description *pwmd_pin; /* per chan */
	uintptr_t                     pwmd_base_addr;
	uint32_t                      pwmd_avail_chan_mask;

};

struct pwm_device {
	const struct pwm_desc        *pwmd_desc;
	int                           pwmd_id;
	const struct pwm_ops         *pwmd_ops;
	void                         *pwmd_priv;

	int                           pwmd_base_freq;
	uint64_t                      pwmd_max_period;
	int                           pwmd_period;
	int                           pwmd_mask;
	int                          *pwmd_duty; /* per chan */
};

__BEGIN_DECLS

extern int pwm_set_period(struct pwm_device *pwm, int period_ns);
extern int pwm_set_frequency(struct pwm_device *pwm, int hz);
extern int pwm_set_duty(struct pwm_device *dev,  int chan_num, int duty_ns);
extern int pwm_enable(struct pwm_device *pwm, uint32_t chan_mask);
extern void pwm_disable(struct pwm_device *pwm, uint32_t chan_mask);

extern struct pwm_device *pwm_dev_by_id(int id);
extern struct pwm_device *pwm_dev_by_idx(int idx);
extern int pwm_dev_num();

static inline int pwm_dev_max_chan(struct pwm_device *pwm) {
	return (int)(pwm->pwmd_desc->pwmd_avail_chan_mask >> 16);
}

__END_DECLS

#define PWM_DESC_GLOBAL_PTR(id) \
					MACRO_CONCAT(ptr_pwm_desc_, id)

#define PWM_DEV_GLOBAL_PTR(id) \
					MACRO_CONCAT(ptr_pwm_dev_, id)

#define PWM_DEV_DEF(id, ops, priv, out_pin, base_addr, avail_mask, max_chan) \
	static const struct pwm_desc MACRO_CONCAT(pwm_desc_, id) = \
						{ id, ops, priv, out_pin, base_addr, \
							(max_chan << 16) | avail_mask }; \
	ARRAY_SPREAD_DECLARE(const struct pwm_desc, __pwm_desc_registry); \
	ARRAY_SPREAD_ADD(__pwm_desc_registry,  MACRO_CONCAT(pwm_desc_, id)); \
	const struct pwm_desc *PWM_DESC_GLOBAL_PTR(id) = &MACRO_CONCAT(pwm_desc_, id); \
	static int MACRO_CONCAT(_pwm_chan_duty_buf_,id)[max_chan] = {0}; \
	static struct pwm_device MACRO_CONCAT(pwm_dev_, id) = \
						{ \
							.pwmd_desc = &MACRO_CONCAT(pwm_desc_, id),  \
							.pwmd_duty = &MACRO_CONCAT(_pwm_chan_duty_buf_,id)[0], \
						} ; \
	ARRAY_SPREAD_DECLARE(const struct pwm_device *, __pwm_device_registry); \
	ARRAY_SPREAD_ADD(__pwm_device_registry,  &MACRO_CONCAT(pwm_dev_, id)); \
	const struct pwm_device *PWM_DEV_GLOBAL_PTR(id) = &MACRO_CONCAT(pwm_dev_, id)

#endif /* DRIVERS_PWM_PWM_H_ */
