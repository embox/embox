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
#include <stdbool.h>

#include <drivers/can_buff.h>
#include <drivers/char_dev.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>
#include <lib/libds/array_spread.h>
#include <mem/misc/pool.h>
#include <util/macro.h>

#define __CAN_BUFF_NAME(_name) MACRO_CONCAT(_name, _buff)

#define __CAN_DEVICE_REGISTER(_can_dev)                                     \
	ARRAY_SPREAD_DECLARE(struct can_dev *const, __can_dev_static_registry); \
	ARRAY_SPREAD_ADD(__can_dev_static_registry, _can_dev)

#define __CAN_DEVICE_DEF(_name, _ops, _priv, _dev_id, _mtu)             \
	static struct can_dev _name = {                                     \
	    .cdev = CHAR_DEV_INIT(MACRO_STRING(MACRO_CONCAT(can, _dev_id)), \
	        &__can_char_dev_ops),                                       \
	    .buff = &__CAN_BUFF_NAME(_name),                                \
	    .configured = false,                                            \
	    .priv = _priv,                                                  \
	    .ops = _ops,                                                    \
	    .mtu = _mtu,                                                    \
	};                                                                  \
	CHAR_DEV_REGISTER((struct char_dev *)&_name);                       \
	__CAN_DEVICE_REGISTER(&_name)

#define CAN_DEVICE_DEF(_name, _ops, _priv, _dev_id) \
	CAN_BUFF_DEF(__CAN_BUFF_NAME(_name));           \
	__CAN_DEVICE_DEF(_name, _ops, _priv, _dev_id, CAN_MTU)

#define CANFD_DEVICE_DEF(_name, _ops, _priv, _dev_id) \
	CANFD_BUFF_DEF(__CAN_BUFF_NAME(_name));           \
	__CAN_DEVICE_DEF(_name, _ops, _priv, _dev_id, CANFD_MTU)

struct can_dev_ops;

struct can_dev_conf {
	struct can_filter filter;
	unsigned long bitrate;
	bool loopback;
};

struct can_dev {
	struct char_dev cdev;
	struct can_dev_conf conf;
	const struct can_dev_ops *const ops;
	struct can_buff *const buff;
	void *const priv;
	const size_t mtu;
	bool configured;
};

struct can_dev_ops {
	void (*cdo_config)(struct can_dev *can);
	int (*cdo_open)(struct can_dev *can);
	void (*cdo_close)(struct can_dev *can);
	int (*cdo_send)(struct can_dev *can, const void *frame);
};

extern const struct char_dev_ops __can_char_dev_ops;

extern struct can_dev *can_dev_find(const char *name);
extern int can_dev_receive(struct can_dev *can, void *frame);
extern int can_dev_wait(struct can_dev *can);
extern void can_dev_notify(struct can_dev *can);

#endif /* DEVICE_CAN_DEV_H_ */
