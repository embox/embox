/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 31.08.2026
 */


#include <errno.h>
#include <stdint.h>
#include <stddef.h>

#include <drivers/motor.h>

#include <embox/unit.h>

ARRAY_SPREAD_DECLARE(const struct motor_dev *, __motor_device_registry); 

EMBOX_UNIT_INIT(motor_subsystem_init);

struct motor_dev *motor_dev_by_id(int id) {
	const struct motor_dev *motor_dev;

	array_spread_foreach(motor_dev, __motor_device_registry) {
		if (motor_dev->md_id == id) {
			return (struct motor_dev *)motor_dev;
		}
	}

	return NULL;
}

static int motor_subsystem_init(void) {
	const struct motor_dev *motor_dev;

	array_spread_foreach(motor_dev, __motor_device_registry) {

		if (!motor_dev->md_ops) {
			continue;
		}

		if (motor_dev->md_ops->mo_init) {
			motor_dev->md_ops->mo_init((struct motor_dev *)motor_dev);
		} else {
			log_warning("Motor%d has no mo_init function", motor_dev->md_id);
		}
	}

	return 0;
}

int motor_send_msg(struct motor_dev *dev, struct motor_msg *msg) {
	if (dev == NULL) {
		return -EINVAL;
	}
	if (dev->md_ops->mo_send_msg) {
		return dev->md_ops->mo_send_msg(dev, msg);
	}

	return -ENOSUPP;
}
