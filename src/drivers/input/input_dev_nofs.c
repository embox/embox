/**
 * @file
 *
 * @date 30.11.20
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <drivers/input/input_dev.h>
#include <embox/unit.h>
#include <kernel/irq.h>
#include <kernel/lthread/lthread.h>
#include <kernel/sched/schedee_priority.h>
#include <util/log.h>

#define INDEV_HND_PRIORITY OPTION_GET(NUMBER, hnd_priority)

static struct lthread indev_handler_lt;

static DLIST_DEFINE(post_indevs);

EMBOX_UNIT_INIT(input_lthread_init);

int input_dev_private_register(struct input_dev *inpdev) {
	dlist_head_init(&inpdev->post_link);

	return 0;
}

int input_dev_private_notify(struct input_dev *inpdev, struct input_event *ev) {
	if (dlist_empty(&inpdev->post_link)) {
		dlist_add_prev(&inpdev->post_link, &post_indevs);
	}

	lthread_launch(&indev_handler_lt);

	return 0;
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

static int input_lthread_init(void) {
	lthread_init(&indev_handler_lt, &indev_handler);
	schedee_priority_set(&indev_handler_lt.schedee, INDEV_HND_PRIORITY);

	return 0;
}
