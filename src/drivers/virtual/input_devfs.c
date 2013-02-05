/**
 * @file
 *
 * @date 20.01.13
 * @author Alexander Kalmuk
 */

#include <types.h>
#include <stdlib.h>
#include <string.h>

#include <fs/file_desc.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/file_operation.h>

#include <mem/objalloc.h>
#include <util/ring_buff.h>

#include <embox/unit.h>

#include <drivers/input/input_dev.h>

EMBOX_UNIT_INIT(init);

#define MAX_OPEN_CNT      64
#define EVENT_SIZE        sizeof(struct input_event)
#define BUF_SIZE          (32 * EVENT_SIZE)

OBJALLOC_DEF(__input_handlers, struct input_handler, MAX_OPEN_CNT);

extern struct dlist_head __input_devices;

static int input_devfs_open(struct node *node, struct file_desc *file_desc, int flags);
static int input_devfs_close(struct file_desc *desc);
static size_t input_devfs_read(struct file_desc *desc, void *buf, size_t size);

kfile_operations_t input_dev_file_op = {
	.open = input_devfs_open,
	.close = input_devfs_close,
	.read = input_devfs_read
};

static struct input_dev *input_devfs_lookup(char *name) {
	struct input_dev *dev, *nxt;

	dlist_foreach_entry(dev, nxt, &__input_devices, input_dev_link) {
		if (0 == strcmp(dev->name, name)) {
			return dev;
		}
	}

	return NULL;
}

static int input_devfs_store_event(struct input_handler *hnd, struct input_event e) {
	assert(hnd);
	ring_buff_enqueue(hnd->storage, &e, 1);
	return 0;
}

static int input_devfs_open(struct node *node, struct file_desc *desc, int flags) {
	struct input_dev *dev;
	struct input_handler *hnd;
	struct ring_buff *buf;

	if (NULL == (dev = input_devfs_lookup((char *)desc->node->name))) {
		return -1;
	}

	if (dlist_empty(&dev->handler_link) && dev->open) {
		dev->open(dev);
	}

	hnd = objalloc(&__input_handlers);
	if (NULL == hnd) {
		return -1;
	}

	hnd->storage = buf = malloc(sizeof(struct ring_buff));
	if (NULL == buf) {
		goto free_hnd;
	}
	buf->storage = malloc(BUF_SIZE);
	if (NULL == buf->storage) {
		goto free_all;
	}
	ring_buff_init(buf, EVENT_SIZE, BUF_SIZE / EVENT_SIZE, buf->storage);

	hnd->store_event = input_devfs_store_event;
	/* store desc as id to search handler in read/close functions */
	hnd->id = (unsigned int)desc;

	dlist_head_init(&hnd->input_dev_link);
	dlist_add_prev(&hnd->input_dev_link, &dev->handler_link);

	return 0;

free_all:
	free(buf);
free_hnd:
	objfree(&__input_handlers, hnd);
	return -1;
}

static int input_devfs_close(struct file_desc *desc) {
	struct input_dev *dev;
	struct input_handler *hnd;

	if (NULL == (dev = input_devfs_lookup((char *)desc->node->name))) {
		return -1;
	}

	if (NULL == (hnd = input_dev_get_handler(dev, (unsigned int)desc))) {
		return -1;
	}

	dlist_del(&hnd->input_dev_link);
	free(((struct ring_buff *)hnd->storage)->storage);
	free(hnd->storage);
	objfree(&__input_handlers, hnd);

	if (dlist_empty(&dev->handler_link) && dev->close) {
		dev->close(dev);
	}

	return 0;
}

static size_t input_devfs_read(struct file_desc *desc, void *buff, size_t size) {
	struct input_dev *dev;
	struct input_handler *hnd;

	if (NULL == (dev = input_devfs_lookup((char *)desc->node->name))) {
		return -1;
	}

	if (NULL == (hnd = input_dev_get_handler(dev, (unsigned int)desc))) {
		return -1;
	}

	return ring_buff_dequeue((struct ring_buff *)hnd->storage, buff, size / EVENT_SIZE);
}

/* from uart.c */
static int input_devfs_register(struct input_dev *dev) {
	struct node *node, *devnode;
	struct nas *dev_nas;

	assert(dev);

	/* register char device */
	if (NULL == (node = vfs_find_node("/dev/input", NULL))) {
		return -1;
	}
	if (NULL == (devnode = vfs_add_path(dev->name, node))) {
		return -1;
	}

	dev_nas = devnode->nas;
	if(NULL == (dev_nas->fs = alloc_filesystem("empty"))) {
		return -1;
	}

	dev_nas->fs->file_op = &input_dev_file_op;

	return 0;
}

static int init(void) {
	struct node *node;
	struct input_dev *dev, *nxt;

	if (NULL == (node = vfs_find_node("/dev", NULL))) {
		return -1;
	}

	if (NULL == vfs_add_path("input", node)) {
		return -1;
	}

	dlist_foreach_entry(dev, nxt, &__input_devices, input_dev_link) {
		if (input_devfs_register(dev) < 0) {
			return -1;
		}
	}

	return 0;
}
