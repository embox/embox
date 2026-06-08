/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.04.26
 */

#include <assert.h>
#include <linux/can.h>
#include <linux/list.h>
#include <poll.h>
#include <sys/types.h>

#include <drivers/can_dev.h>
#include <drivers/char_dev.h>
#include <kernel/irq.h>
#include <mem/misc/pool.h>

void can_msg_queue_init(struct can_dev *can) {
	INIT_LIST_HEAD(&can->rx_queue);
}

void can_msg_queue_push(struct can_dev *can, struct can_msg *msg) {
	list_add_tail((struct list_head *)msg, &can->rx_queue);
}

struct can_msg *can_msg_queue_front(struct can_dev *can) {
	struct can_msg *msg;

	msg = (struct can_msg *)can->rx_queue.next;
	assert(msg);

	if (msg == (void *)&can->rx_queue) {
		return NULL;
	}

	return msg;
}

struct can_msg *can_msg_queue_pop(struct can_dev *can) {
	struct can_msg *msg;

	msg = can_msg_queue_front(can);
	if (msg) {
		list_del_init((struct list_head *)msg);
	}

	return msg;
}

struct can_msg *can_msg_alloc(struct can_dev *can) {
	return (struct can_msg *)pool_alloc(can->msg_pool);
}

void can_msg_free(struct can_dev *can, struct can_msg *msg) {
	pool_free(can->msg_pool, (void *)msg);
}

void can_rx_start(struct can_dev *can) {
	if (can->ops->rxint) {
		can->ops->rxint(can, 1);
	}
}

void can_rx_stop(struct can_dev *can) {
	if (can->ops->rxint) {
		can->ops->rxint(can, 0);
	}
}

void can_rx_notify(struct can_dev *can) {
	if (can_msg_queue_front(can)) {
		char_dev_notify((struct char_dev *)can);
	}
}
