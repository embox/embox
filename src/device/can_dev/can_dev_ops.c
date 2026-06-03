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

static int can_dev_open(struct char_dev *char_dev, struct idesc *idesc) {
	struct can_dev *dev;
	int err;

	dev = (struct can_dev *)char_dev;

	dev->rx_buff_cnt = 0;
	waitq_init(&dev->waitq);

	err = dev->ops->open(dev);
	if (err) {
		return err;
	}

	dev->ops->irq_en(dev);

	return 0;
}

static void can_dev_close(struct char_dev *char_dev) {
	struct can_dev *dev;

	dev = (struct can_dev *)char_dev;

	dev->ops->irq_dis(dev);
	dev->ops->close(dev);
}

static ssize_t can_dev_read(struct char_dev *char_dev, void *buf, size_t nbyte) {
	struct waitq_link wql;
	struct can_dev *dev;

	dev = (struct can_dev *)char_dev;
	assert(dev->rx_buff_cnt >= 0);

	if (dev->rx_buff_cnt == 0) {
		waitq_link_init(&wql);
		waitq_wait_prepare(&dev->waitq, &wql);

		SCHED_WAIT(dev->rx_buff_cnt > 0);

		waitq_wait_cleanup(&dev->waitq, &wql);

		if (dev->rx_buff_cnt == 0) {
			return 0;
		}
	}

	memcpy(buf, &dev->rx_buff[dev->rx_buff_cnt - 1], nbyte);

	if (dev->rx_buff_cnt == CAN_RX_BUFF_SIZE) {
		dev->ops->irq_en(dev);
	}

	--dev->rx_buff_cnt;

	return nbyte;
}

static ssize_t can_dev_write(struct char_dev *char_dev, const void *buf,
    size_t nbyte) {
	struct can_dev *dev;
	int err;

	dev = (struct can_dev *)char_dev;

	err = dev->ops->send(dev, (can_frame_t *)buf);
	if (err) {
		return err;
	}

	return nbyte;
}

static int can_dev_ioctl(struct char_dev *char_dev, int request, void *data) {
	return 0;
}

static int can_dev_status(struct char_dev *char_dev, int mask) {
	int res = 0;

	if (mask & POLLIN) {
		res += sizeof(can_frame_t);
	}

	if (mask & POLLOUT) {
		res += sizeof(can_frame_t);
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
