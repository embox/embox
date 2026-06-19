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

#include <drivers/char_dev.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>
#include <kernel/thread/sync/mutex.h>
#include <lib/libds/array_spread.h>
#include <mem/misc/pool.h>
#include <util/macro.h>

#include <config/embox/device/can_dev.h>

#define CAN_FRAME_POOL_SIZE \
	OPTION_MODULE_GET(embox__device__can_dev, NUMBER, frame_pool_size)

#define __CAN_DEVICE_DEF(_name, _ops, _priv, _dev_id, _mtu)                \
	static struct can_dev _name = {                                        \
	    .cdev = CHAR_DEV_INIT(_name.cdev,                                  \
	        MACRO_STRING(MACRO_CONCAT(can, _dev_id)), &__can_char_dev_ops, \
	        CHAR_DEV_TYPE_CAN),                                            \
	    .mutex = MUTEX_INIT(_name.mutex),                                  \
	    .rx_queue = LIST_HEAD_INIT(_name.rx_queue),                        \
	    .msg_pool = &MACRO_CONCAT(_name, _pool),                           \
	    .mtu = _mtu,                                                       \
	    .ops = _ops,                                                       \
	    .priv = _priv,                                                     \
	};                                                                     \
	CHAR_DEV_REGISTER((struct char_dev *)&_name)

#define CAN_DEVICE_DEF(_name, _ops, _priv, _dev_id)                            \
	POOL_DEF(MACRO_CONCAT(_name, _pool), struct can_msg, CAN_FRAME_POOL_SIZE); \
	__CAN_DEVICE_DEF(_name, _ops, _priv, _dev_id, CAN_MTU)

#define CANFD_DEVICE_DEF(_name, _ops, _priv, _dev_id)                            \
	POOL_DEF(MACRO_CONCAT(_name, _pool), struct canfd_msg, CAN_FRAME_POOL_SIZE); \
	__CAN_DEVICE_DEF(_name, _ops, _priv, _dev_id, CANFD_MTU)

struct can_ops;

struct can_dev {
	struct char_dev cdev;
	struct mutex mutex;
	struct list_head rx_queue;
	struct pool *msg_pool;
	const struct can_ops *ops;
	void *priv;
	const int mtu;
};

struct can_ops {
	void (*reset)(struct can_dev *can);
	int (*open)(struct can_dev *can);
	void (*close)(struct can_dev *can);
	void (*rxint)(struct can_dev *can, int enable);
	int (*send)(struct can_dev *can, const void *frame);
};

struct can_msg {
	struct list_head lnk;
	struct can_frame frame;
};

struct canfd_msg {
	struct list_head lnk;
	struct canfd_frame frame;
};

extern const struct char_dev_ops __can_char_dev_ops;

extern void can_msg_queue_init(struct can_dev *can);
extern void can_msg_queue_push(struct can_dev *can, struct can_msg *msg);
extern struct can_msg *can_msg_queue_front(struct can_dev *can);
extern struct can_msg *can_msg_queue_pop(struct can_dev *can);

extern struct can_msg *can_msg_alloc(struct can_dev *can);
extern void can_msg_free(struct can_dev *can, struct can_msg *msg);

extern void can_rx_start(struct can_dev *can);
extern void can_rx_stop(struct can_dev *can);
extern void can_rx_notify(struct can_dev *can);

/* clang-format off */
#define can_dev_foreach(dev_ptr) \
	char_dev_foreach((struct char_dev *)dev_ptr) \
		if (((struct char_dev *)dev_ptr)->type == CHAR_DEV_TYPE_CAN)
/* clang-format on */

#endif /* DEVICE_CAN_DEV_H_ */
