/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 31.08.2026
 */

#ifndef DRIVERS_MOTOR_H_
#define DRIVERS_MOTOR_H_

#include <stdint.h>

#include <lib/libds/array_spread.h>

struct motor_dev;
struct pwm_device;

struct motor_msg {
	void     *mm_buf;
	int       mm_len;
	uint32_t  mm_flags;
};

struct motor_ops {
	int (*mo_init)(struct motor_dev *dev);
	int (*mo_send_msg)(struct motor_dev *dev, struct motor_msg *msg);
};

struct motor_dev {
	const struct motor_ops *md_ops;
	void                   *md_priv;
	int                     md_id;

	struct pwm_device      *md_pwm_dev;
	int                     md_pwm_id;
	int                     md_pwm_chan;
	int                     md_period;
};

extern struct motor_dev *motor_dev_by_id(int id);

extern int motor_send_msg(struct motor_dev *dev, struct motor_msg *msg);

#define MOTOR_DEV_DEF(id, ops, priv, pwm_id, pwm_chan) \
	ARRAY_SPREAD_DECLARE(const struct motor_dev *, __motor_device_registry); \
	static struct motor_dev MACRO_CONCAT(motor_dev_, id) = \
						{ .md_id = id, \
							.md_ops = ops, \
							.md_priv = priv, \
							.md_pwm_id = pwm_id, \
							.md_pwm_chan = pwm_chan, \
						}; \
	ARRAY_SPREAD_ADD(__motor_device_registry, &MACRO_CONCAT(motor_dev_, id) )

#endif /* DRIVERS_MOTOR_H_ */
