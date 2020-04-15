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
#include <util/dlist.h>
#include <embox/unit.h>

#include <kernel/lthread/lthread.h>
#include <kernel/sched/schedee_priority.h>

#include <drivers/input/input_dev.h>

#define INDEV_HND_PRIORITY OPTION_GET(NUMBER, hnd_priority)

EMBOX_UNIT_INIT(input_init);

static struct lthread indev_handler_lt;

static DLIST_DEFINE(input_devices);
static DLIST_DEFINE(post_indevs);

void input_dev_report_event(struct input_dev *dev, struct input_event *ev) {
	assert(dev);

	irq_lock();
	{
		ev->devtype = dev->type;

		ring_buff_enqueue(&dev->rbuf, ev, 1);

		if (dlist_empty(&dev->post_link)) {
			/* dev not in queue */
			dlist_add_prev(&dev->post_link, &post_indevs);
		}
		lthread_launch(&indev_handler_lt);
	}
	irq_unlock();
}

static int indev_handler(struct lthread *self) {
	struct input_dev *dev;

	irq_lock();
	{
		dlist_foreach_entry_safe(dev, &post_indevs, post_link) {
			if (dev->event_cb) {
				irq_unlock();
				dev->event_cb(dev);
				irq_lock();
			}
			dlist_del_init(&dev->post_link);
		}
	}
	irq_unlock();

	return 0;
}

int input_dev_register(struct input_dev *dev) {
	if (!dev || !dev->ops) {
		return -EINVAL;
	}
	dev->event_cb = NULL;

	ring_buff_init(&dev->rbuf, sizeof(struct input_event),
			INPUT_DEV_EVENT_QUEUE_LEN, &dev->event_buf);

	dlist_add_prev(dlist_head_init(&dev->dev_link), &input_devices);

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

	dlist_head_init(&dev->post_link);

	if (dev->ops->start) {
		res = dev->ops->start(dev);
		if (res < 0) {
			return res;
		}
	}

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

static int input_init(void) {
	lthread_init(&indev_handler_lt, &indev_handler);
	schedee_priority_set(&indev_handler_lt.schedee, INDEV_HND_PRIORITY);
	return 0;
}
