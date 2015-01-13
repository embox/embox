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
#include <kernel/softirq.h>
#include <util/dlist.h>
#include <embox/unit.h>

#include <drivers/input/input_dev.h>

#define INPUT_SOFTIRQ 17

EMBOX_UNIT_INIT(input_devfs_init);

#if 0
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <fs/file_desc.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/file_operation.h>

#include <kernel/irq.h>
#include <mem/objalloc.h>
#include <util/ring_buff.h>

#include <kernel/sched/sched_lock.h>
#include <kernel/sched.h>


#define MAX_OPEN_CNT      64
#define EVENT_SIZE        sizeof(struct input_event)
//#define BUF_SIZE          (32 * EVENT_SIZE)

OBJALLOC_DEF(__input_handlers, struct input_subscriber, MAX_OPEN_CNT);

static node_t *input_devfs_root;

static int input_devfs_open(struct node *node, struct file_desc *file_desc, int flags);
static int input_devfs_close(struct file_desc *desc);
static size_t input_devfs_read(struct file_desc *desc, void *buf, size_t size);

static struct kfile_operations input_dev_file_op = {
	.open = input_devfs_open,
	.close = input_devfs_close,
	.read = input_devfs_read
};


static struct input_subscriber *input_dev_find_subscriber(struct input_dev *dev, unsigned int handler_id) {
	struct input_subscriber *cur;

	assert(dev != NULL);

	dlist_foreach_entry(cur, &dev->subscribers, subscribers) {
		if (cur->id == handler_id) {
			return cur;
		}
	}

	return NULL;
}

#endif

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
		if (!dev->curprocessd && !ring_buff_fill_nulls(&dev->rbuf, 1)) {
				ret = 0;
				goto out_unlock;
			}
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
		ret = softirq_raise(INPUT_SOFTIRQ);
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

static void indev_softirqhnd(unsigned int nt, void* data) {
	struct input_dev *dev;

	softirq_lock();

	dlist_foreach_entry(dev, &post_indevs, post_link) {
		dlist_del(&dev->post_link);

		dlist_head_init(&dev->post_link);
		assert(dlist_empty(&dev->post_link));

		assert(dev->event_cb);

		dev->event_cb(dev);
	}

	softirq_unlock();
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


#if 0
static int input_devfs_open(struct node *node, struct file_desc *desc, int flags) {
	struct input_dev *dev;
	struct input_subscriber *hnd;

	if (NULL == (dev = input_devfs_lookup((char *)desc->node->name))) {
		return -1;
	}

	hnd = objalloc(&__input_handlers);
	if (NULL == hnd) {
		objfree(&__input_handlers, hnd);
		return -1;
	}

	/* if we not open before */
	if(dlist_empty(&dev->subscribers)) {
		irq_attach(dev->irq, input_irq_handler, 0, dev, dev->name);
	}


	ring_buff_init(&hnd->rbuff, EVENT_SIZE, sizeof(hnd->inbuff) / EVENT_SIZE, hnd);

	/* store desc as id to search handler in read/close functions */
	hnd->id = (unsigned int)desc;

	dlist_add_prev(dlist_head_init(&hnd->subscribers), &dev->subscribers);

	return 0;

}

static int input_devfs_close(struct file_desc *desc) {
	struct input_dev *dev;
	struct input_subscriber *hnd;

	if (NULL == (dev = input_devfs_lookup((char *)desc->node->name))) {
		return -1;
	}

	if (NULL == (hnd = input_dev_find_subscriber(dev, (unsigned int)desc))) {
		return -1;
	}

	dlist_del(&hnd->subscribers);
	objfree(&__input_handlers, hnd);

	if (dlist_empty(&dev->subscribers)) {
		irq_detach(dev->irq, dev);
	}

	if (dlist_empty(&dev->subscribers) && dev->close) {
		dev->close(dev);
	}

	return 0;
}

static size_t input_devfs_read(struct file_desc *desc, void *buff, size_t size) {
	struct input_dev *dev;
	size_t cnt = size;
	struct input_subscriber *hnd;

	if (NULL == (dev = input_devfs_lookup((char *)desc->node->name))) {
		return -1;
	}

	if (NULL == (hnd = input_dev_find_subscriber(dev, (unsigned int)desc))) {
		return -1;
	}


	if(0 == ring_buff_get_cnt(&hnd->rbuff)) {
		sched_lock();
			irq_lock();
				event_init(&hnd->rx_happend, "event_rx_happend");
			irq_unlock();

			event_wait(&hnd->rx_happend, SCHED_TIMEOUT_INFINITE);
		sched_unlock();
	}

	for(;0 < cnt;--cnt) {
		int tmp;
		char *tmp_char = buff;

		ring_buff_dequeue(&hnd->rbuff, &tmp, 1);
		*tmp_char = (char)tmp;


		buff = (void *)(((uint32_t)buff) + 1);
	}

	return size - cnt;

}


static int input_devfs_register(struct input_dev *dev) {
	struct node *node;
	struct nas *nas;
	mode_t mode;

	assert(dev);

	mode = S_IFCHR | S_IRALL | S_IWALL; // TODO is it a char dev? -- Eldar

	/* register char device */
	node = vfs_create_child(input_devfs_root, dev->name, mode);
	if (!node) {
		return -1;
	}

	nas = node->nas;
	nas->fs = filesystem_create("empty");
	if (!nas->fs) {
		return -1;
	}

	nas->fs->file_op = &input_dev_file_op;

	return 0;
}

#endif

static int input_devfs_init(void) {
	return softirq_install(INPUT_SOFTIRQ, indev_softirqhnd, NULL);
}
