/**
 * @file
 * @brief Platform-independent interface to access servos
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 13.08.2022
 */

#ifndef DRIVERS_SERVO_H_
#define DRIVERS_SERVO_H_

#include <lib/libds/array.h>

struct servo_dev;

struct servo_ops {
	int (*init)(struct servo_dev *dev);
	int (*set_pos)(struct servo_dev *dev, int pos);
};

struct servo_dev {
	const struct servo_ops *servo_ops;
	void *servo_priv;
};

extern struct servo_dev *servo_dev_by_id(int id);

#define SERVO_DEV_DEF(ops, priv) \
	ARRAY_SPREAD_DECLARE(struct servo_dev, __servo_device_registry); \
	ARRAY_SPREAD_ADD(__servo_device_registry, { ops, priv } )

#endif /* DRIVERS_SERVO_H_ */
