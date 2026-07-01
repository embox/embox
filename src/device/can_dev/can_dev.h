/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.04.26
 */

#ifndef DEVICE_CAN_DEV_H_
#define DEVICE_CAN_DEV_H_

#include <linux/can.h>
#include <linux/list.h>

#include <drivers/can_buff.h>
#include <drivers/char_dev.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>
#include <lib/libds/array_spread.h>
#include <mem/misc/pool.h>
#include <util/macro.h>

#define __CAN_BUFF_NAME(_name) MACRO_CONCAT(_name, _buff)

#define __CAN_DEVICE_DEF(_name, _ops, _priv, _dev_id, _mtu)             \
	static struct can_dev _name = {                                     \
	    .cdev = CHAR_DEV_INIT(MACRO_STRING(MACRO_CONCAT(can, _dev_id)), \
	        &__can_char_dev_ops),                                       \
	    .buff = &__CAN_BUFF_NAME(_name),                                \
	    .priv = _priv,                                                  \
	    .ops = _ops,                                                    \
	    .mtu = _mtu,                                                    \
	};                                                                  \
	CHAR_DEV_REGISTER((struct char_dev *)&_name)

#define CAN_DEVICE_DEF(_name, _ops, _priv, _dev_id) \
	CAN_BUFF_DEF(__CAN_BUFF_NAME(_name));           \
	__CAN_DEVICE_DEF(_name, _ops, _priv, _dev_id, CAN_MTU)

#define CANFD_DEVICE_DEF(_name, _ops, _priv, _dev_id) \
	CANFD_BUFF_DEF(__CAN_BUFF_NAME(_name));           \
	__CAN_DEVICE_DEF(_name, _ops, _priv, _dev_id, CANFD_MTU)

struct can_ops;

struct can_dev {
	struct char_dev cdev;
	const struct can_ops *const ops;
	struct can_buff *const buff;
	void *const priv;
	const size_t mtu;
};

struct can_ops {
	void (*co_reset)(struct can_dev *can);
	int (*co_open)(struct can_dev *can);
	void (*co_close)(struct can_dev *can);
	int (*co_send)(struct can_dev *can, const void *frame);
};

extern const struct char_dev_ops __can_char_dev_ops;

extern int can_dev_receive(struct can_dev *can, void *frame);
extern int can_dev_wait(struct can_dev *can);
extern void can_dev_notify(struct can_dev *can);

#endif /* DEVICE_CAN_DEV_H_ */
