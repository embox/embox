/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.04.26
 */

#include <assert.h>
#include <errno.h>
#include <linux/can.h>
#include <poll.h>
#include <string.h>
#include <sys/types.h>

#include <drivers/can_dev.h>
#include <drivers/char_dev.h>
#include <kernel/sched/waitq.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/thread/thread_sched_wait.h>

static int can_dev_open(struct char_dev *cdev, struct idesc *idesc) {
	struct can_dev *can;
	int err;

	can = (struct can_dev *)cdev;

	err = can->ops->co_open(can);
	if (err) {
		return err;
	}

	can_rx_start(can);

	return 0;
}

static void can_dev_close(struct char_dev *cdev) {
	struct can_dev *can;
	struct can_msg *msg;

	can = (struct can_dev *)cdev;

	can_rx_stop(can);

	can->ops->co_close(can);

	while ((msg = can_msg_queue_pop(can))) {
		can_msg_free(can, msg);
	}
}

static ssize_t can_dev_read(struct char_dev *cdev, void *buf, size_t nbyte,
    int flags) {
	struct can_dev *can;
	struct can_msg *msg;

	can = (struct can_dev *)cdev;
	msg = NULL;

	if ((nbyte != CANFD_MTU) && (nbyte != can->mtu)) {
		return -EINVAL;
	}

	while (1) {
		msg = can_msg_queue_pop(can);
		if (msg) {
			break;
		}

		can_rx_start(can);
		char_dev_wait(cdev, POLLIN);
	}

	memset(buf, 0, nbyte);
	memcpy(buf, &msg->frame, can->mtu);

	can_msg_free(can, msg);

	return nbyte;
}

static ssize_t can_dev_write(struct char_dev *cdev, const void *buf,
    size_t nbyte, int flags) {
	struct can_dev *can;
	int err;

	can = (struct can_dev *)cdev;

	if ((nbyte != CAN_MTU) && (nbyte != can->mtu)) {
		return -EINVAL;
	}

	err = can->ops->co_send(can, buf);
	if (err) {
		return err;
	}

	return nbyte;
}

static int can_dev_ioctl(struct char_dev *cdev, int request, void *data) {
	return 0;
}

static int can_dev_status(struct char_dev *cdev, int mask) {
	struct can_dev *can;
	int res;

	can = (struct can_dev *)cdev;
	res = 0;

	if (mask & POLLIN) {
		if (can_msg_queue_front(can)) {
			res += can->mtu;
		}
	}

	if (mask & POLLOUT) {
		res += can->mtu;
	}

	return res;
}

const struct char_dev_ops __can_char_dev_ops = {
    .open = can_dev_open,
    .close = can_dev_close,
    .read = can_dev_read,
    .write = can_dev_write,
    .ioctl = can_dev_ioctl,
    .status = can_dev_status,
};
