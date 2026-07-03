/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 01.07.26
 */

#ifndef DEVICE_CAN_BUFF_H_
#define DEVICE_CAN_BUFF_H_

#include <linux/can.h>
#include <linux/list.h>
#include <sys/types.h>

#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <util/macro.h>

#include <config/embox/device/can_buff.h>

#define CAN_MSG_POOL_SIZE \
	OPTION_MODULE_GET(embox__device__can_buff, NUMBER, msg_pool_size)

#define CAN_BUFF_DEF(_name)                                                  \
	POOL_DEF(MACRO_CONCAT(_name, _pool), struct can_msg, CAN_MSG_POOL_SIZE); \
	static struct can_buff _name = {                                         \
	    .rx_queue = LIST_HEAD_INIT(_name.rx_queue),                          \
	    .msg_pool = &MACRO_CONCAT(_name, _pool),                             \
	}

#define CANFD_BUFF_DEF(_name)                                                  \
	POOL_DEF(MACRO_CONCAT(_name, _pool), struct canfd_msg, CAN_MSG_POOL_SIZE); \
	static struct can_buff _name = {                                           \
	    .rx_queue = LIST_HEAD_INIT(_name.rx_queue),                            \
	    .msg_pool = &MACRO_CONCAT(_name, _pool),                               \
	}

struct can_msg {
	struct list_head lnk;
	struct can_frame frame;
};

struct canfd_msg {
	struct list_head lnk;
	struct canfd_frame frame;
};

struct can_buff {
	struct list_head rx_queue;
	struct pool *msg_pool;
};

extern void can_buff_init(struct can_buff *buff);
extern int can_buff_empty(struct can_buff *buff);
extern int can_buff_write(struct can_buff *buff, void *frame, size_t size);
extern int can_buff_read(struct can_buff *buff, void *frame, size_t size);

#endif /* DEVICE_CAN_BUFF_H_ */
