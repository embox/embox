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
#include <kernel/lthread/lthread_priority.h>

#include <drivers/input/input_dev.h>

#define INDEV_HND_PRIORITY OPTION_GET(NUMBER, hnd_priority)

EMBOX_UNIT_INIT(input_devfs_init);

static struct lthread indev_handler_lt;

/*POOL_DEF(input_event_pool, struct input_eventq, 16);*/

static DLIST_DEFINE(input_devices);
static DLIST_DEFINE(post_indevs);

int input_dev_input(struct input_dev *dev) {
	int ret = 0;

	assert(dev);
	assert(dev->ops);

	if (dev->event_cb == NULL) {
		return 0;
	}

	irq_lock();
	{
		if (!dev->curprocessd && !ring_buff_alloc(&dev->rbuf, 1,
			(void **) &dev->curprocessd)) {
			ret = 0;
			goto out_unlock;
		}

		if (0 > (ret = dev->ops->event_get(dev, dev->curprocessd))) {
			ret = 0;
			goto out_unlock;
		}

		dev->curprocessd->devtype = dev->type;
		dev->curprocessd = NULL;

		if (dlist_empty(&dev->post_link)) {
			/* dev not in queue */
			dlist_add_prev(&dev->post_link, &post_indevs);
		}
		lthread_launch(&indev_handler_lt);
	}
out_unlock:
	irq_unlock();

	return 0;
}

static irq_return_t indev_irqhnd(unsigned int irq_nr, void *data) {
	struct input_dev *dev = (struct input_dev *) data;

	input_dev_input(dev);

	return IRQ_HANDLED;
}

static int indev_handler(struct lthread *self) {
	struct input_dev *dev;

	sched_lock();

	dlist_foreach_entry(dev, &post_indevs, post_link) {
		dlist_del(&dev->post_link);

		dlist_head_init(&dev->post_link);
		assert(dlist_empty(&dev->post_link));

		assert(dev->event_cb);

		dev->event_cb(dev);
	}

	sched_unlock();

	return 0;
}

static int evnt_noact(struct input_dev *dev) {

	return 0;
}

int input_dev_register(struct input_dev *dev) {
	if (dev == NULL) {
		return -EINVAL;
	}


	if (!dev->ops || !dev->ops->event_get) {
		return -EINVAL;
	}

	dev->event_cb = NULL;

	ring_buff_init(&dev->rbuf, sizeof(struct input_event),
			INPUT_DEV_EVENT_QUEUE_LEN, &dev->event_buf);

	dev->curprocessd = NULL;

	dlist_add_prev(dlist_head_init(&dev->global_indev_list), &input_devices);

	return 0;
}

int input_dev_event(struct input_dev *dev, struct input_event *ev) {
	int cnt;

	if (dev == NULL) {
		return -EINVAL;
	}

	cnt = ring_buff_get_cnt(&dev->rbuf);
	if ((cnt == 0)
			|| ((dev->curprocessd != NULL) && (cnt == 1))) {
		return -ENOENT;
	}

	ring_buff_dequeue(&dev->rbuf, ev, 1);

	return 0;

}

int input_dev_open(struct input_dev *dev, indev_event_cb_t *event) {
	int res;

	if (dev == NULL) {
		return -EINVAL;
	}

	if (event) {
		dev->event_cb = event;
	} else {
		dev->event_cb = evnt_noact;
	}

	dlist_head_init(&dev->post_link);

	if (dev->irq > 0) {

		res = irq_attach(dev->irq, indev_irqhnd, 0, dev, "input_dev hnd");
		if (res < 0) {
			return res;
		}
	}

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

	if (dev->irq > 0) {
		irq_detach(dev->irq, dev);
	}

	dev->event_cb = NULL;

	if (dev->ops->stop) {
		dev->ops->stop(dev);
	}

	return 0;
}

struct input_dev *input_dev_lookup(const char *name) {
	struct input_dev *dev;

	dlist_foreach_entry(dev,  &input_devices, global_indev_list) {
		if (0 == strcmp(dev->name, name)) {
			return dev;
		}
	}

	return NULL;
}

static int input_devfs_init(void) {
	lthread_init(&indev_handler_lt, &indev_handler);
	lthread_priority_set(&indev_handler_lt, INDEV_HND_PRIORITY);
	return 0;
}
