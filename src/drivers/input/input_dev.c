/**
 * @file
 *
 * @date 20.01.13
 * @author Alexander Kalmuk
 * @author Anton Kozlov:
 * 	splitted with fs, simplifying
 */

#include <errno.h>
#include <string.h>
#include <kernel/irq.h>
#include <lib/libds/dlist.h>

#include <kernel/lthread/lthread.h>
#include <kernel/sched/schedee_priority.h>

#include <drivers/input/input_dev.h>

static DLIST_DEFINE(input_devices);

void input_dev_report_event(struct input_dev *dev, struct input_event *ev) {
	assert(dev);

	if (!(dev->flags & INPUT_DEV_OPENED)) {
		return;
	}

	irq_lock();
	{
		ev->devtype = dev->type;

		ring_buff_enqueue(&dev->rbuf, ev, 1);

		input_dev_private_notify(dev, ev);
	}
	irq_unlock();
}

int input_dev_register(struct input_dev *dev) {
	if (!dev || !dev->ops) {
		return -EINVAL;
	}
	dev->event_cb = NULL;

	ring_buff_init(&dev->rbuf, sizeof(struct input_event),
			INPUT_DEV_EVENT_QUEUE_LEN, &dev->event_buf);

	dlist_add_prev(dlist_head_init(&dev->dev_link), &input_devices);

	input_dev_private_register(dev);

	return 0;
}

int input_dev_event(struct input_dev *dev, struct input_event *ev) {
	int ret = 0;

	if (dev == NULL) {
		return -EINVAL;
	}

	irq_lock();
	{
		if (0 == ring_buff_get_cnt(&dev->rbuf)) {
			ret = -ENOENT;
			goto out;
		}

		ring_buff_dequeue(&dev->rbuf, ev, 1);
	}
out:
	irq_unlock();

	return ret;
}

int input_dev_open(struct input_dev *dev, indev_event_cb_t *event_cb) {
	int res;

	if (dev == NULL) {
		return -EINVAL;
	}
	dev->event_cb = event_cb;

	if (dev->ops->start) {
		res = dev->ops->start(dev);
		if (res < 0) {
			return res;
		}
	}

	dev->flags |= INPUT_DEV_OPENED;

	return 0;
}

int input_dev_close(struct input_dev *dev) {
	if (dev == NULL) {
		return -EINVAL;
	}

	dev->event_cb = NULL;

	if (dev->ops->stop) {
		dev->ops->stop(dev);
	}

	dev->flags &= ~INPUT_DEV_OPENED;

	return 0;
}

struct input_dev *input_dev_lookup(const char *name) {
	struct input_dev *dev;

	dlist_foreach_entry(dev, &input_devices, dev_link) {
		if (0 == strcmp(dev->name, name)) {
			return dev;
		}
	}

	return NULL;
}

struct input_dev *input_dev_iterate(struct input_dev *dev) {
	struct dlist_head *link;

	link = dev ? dev->dev_link.next : input_devices.next;
	if (link == &input_devices) {
		return NULL;
	}
	return member_cast_out(link, struct input_dev, dev_link);
}
