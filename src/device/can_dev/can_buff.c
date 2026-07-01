/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.04.26
 */

#include <assert.h>
#include <linux/list.h>
#include <sys/types.h>

#include <drivers/can_buff.h>
#include <mem/misc/pool.h>

static_assert(CAN_MSG_POOL_SIZE >= 1, "");

static struct can_msg *can_msg_alloc(struct can_buff *buff) {
	return (struct can_msg *)pool_alloc(buff->msg_pool);
}

static void can_msg_free(struct can_buff *buff, struct can_msg *msg) {
	pool_free(buff->msg_pool, (void *)msg);
}

static struct can_msg *can_msg_queue_front(struct can_buff *buff) {
	struct can_msg *msg;

	msg = (struct can_msg *)buff->rx_queue.next;
	assert(msg);

	if (msg == (void *)&buff->rx_queue) {
		return NULL;
	}

	return msg;
}

static void can_msg_queue_push(struct can_buff *buff, struct can_msg *msg) {
	list_add_tail((struct list_head *)msg, &buff->rx_queue);
}

struct can_msg *can_msg_queue_pop(struct can_buff *buff) {
	struct can_msg *msg;

	msg = can_msg_queue_front(buff);
	if (msg) {
		list_del_init((struct list_head *)msg);
	}

	return msg;
}

void can_buff_init(struct can_buff *buff) {
	struct can_msg *msg;

	while ((msg = can_msg_queue_pop(buff))) {
		can_msg_free(buff, msg);
	}
}

int can_buff_empty(struct can_buff *buff) {
	return (can_msg_queue_front(buff) == NULL);
}

int can_buff_write(struct can_buff *buff, void *frame, size_t size) {
	struct can_msg *msg;

	msg = can_msg_alloc(buff);
	if (!msg) {
		return -1;
	}

	memcpy(&msg->frame, frame, size);
	can_msg_queue_push(buff, msg);

	return 0;
}

int can_buff_read(struct can_buff *buff, void *frame, size_t size) {
	struct can_msg *msg;

	msg = can_msg_queue_pop(buff);
	if (!msg) {
		return -1;
	}

	memcpy(frame, &msg->frame, size);
	can_msg_free(buff, msg);

	return 0;
}
