/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.04.26
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/can.h>
#include <poll.h>
#include <string.h>
#include <sys/types.h>

#include <drivers/can_buff.h>
#include <drivers/can_dev.h>
#include <drivers/char_dev.h>
#include <kernel/irq_lock.h>

static int can_dev_open(struct char_dev *cdev, struct idesc *idesc) {
	struct can_dev *can;
	int err;

	can = (struct can_dev *)cdev;

	/* Clear old messages */
	can_buff_init(can->buff);

	err = can->ops->co_open(can);
	if (err) {
		return err;
	}

	return 0;
}

static void can_dev_close(struct char_dev *cdev) {
	struct can_dev *can;

	can = (struct can_dev *)cdev;

	can->ops->co_close(can);
}

static ssize_t can_dev_read(struct char_dev *cdev, void *buf, size_t nbyte,
    int flags) {
	struct can_dev *can;
	int err;

	can = (struct can_dev *)cdev;

	if ((nbyte != CANFD_MTU) && (nbyte != can->mtu)) {
		return -EINVAL;
	}

	while (1) {
		IRQ_LOCKED_DO(err = can_buff_read(can->buff, buf, nbyte));
		if (!err) {
			break;
		}

		if (flags & O_NONBLOCK) {
			return -EAGAIN;
		}

		err = can_dev_wait(can);
		if (err) {
			return err;
		}
	}

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
		if (!can_buff_empty(can->buff)) {
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
