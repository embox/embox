/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.04.26
 */

#ifndef NET_CAN_H_
#define NET_CAN_H_

#include <linux/can.h>

#include <module/embox/net/can.h>

#ifndef CAN_IMPL_SOCKETCAN
#define CAN_IMPL_SOCKETCAN 0
#endif

#ifndef CAN_IMPL_CHARDEV
#define CAN_IMPL_CHARDEV 0
#endif

#define CAN_CONTROLLER_REGISTER(_can_controller) \
	__CAN_CONTROLLER_REGISTER(_can_controller)

struct can_controller;

struct can_ops {
	int (*start)(struct can_controller *can);
	int (*stop)(struct can_controller *can);
	void (*eoi)(struct can_controller *can);
	int (*hasrx)(struct can_controller *can);
	int (*send)(struct can_controller *can, struct can_frame *frame);
	int (*receive)(struct can_controller *can, struct can_frame *frame);
	// int (*set_filter)(struct can_controller *can, struct can_filter *filter);
};

struct can_controller {
	const struct can_ops *ops;
	int irq;
	void *priv;
};

#endif /* NET_CAN_H_ */
