/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.04.26
 */

#ifndef NET_CAN_CHARDEV_H_
#define NET_CAN_CHARDEV_H_

#include <drivers/char_dev.h>
#include <lib/libds/array_spread.h>
#include <net/can.h>

#define CAN_IMPL_CHARDEV 1

#define __CAN_CONTROLLER_REGISTER(_can_controller)                           \
	static struct can_chardev _can_controller##_chardev = {                  \
	    .can = &_can_controller,                                             \
	};                                                                       \
	ARRAY_SPREAD_DECLARE(struct can_chardev *const, __can_chardev_registry); \
	ARRAY_SPREAD_ADD(__can_chardev_registry, &_can_controller##_chardev)

struct can_chardev {
	struct char_dev cdev;
	struct can_controller *can;
};

#endif /* NET_CAN_CHARDEV_H_ */
