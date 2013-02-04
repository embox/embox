/**
 * @file
 *
 * @date Oct 25, 2012
 * @author: Anton Bondarev
 */
#include <types.h>
#include <string.h>

#include <util/ring_buff.h>

#include <fs/file_desc.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/file_operation.h>

#include <drivers/uart_device.h>

#include <kernel/irq.h>

#include <kernel/thread/event.h>
#include <kernel/thread/sched_lock.h>
#include <kernel/thread/sched.h>

static struct uart_device *uart_dev;

static struct uart_device *uart_dev_lookup(char *name) {
	return uart_dev;
}

static int dev_uart_open(struct node *node, struct file_desc *file_desc,
	int flags);
static int dev_uart_close(struct file_desc *desc);
static size_t dev_uart_read(struct file_desc *desc, void *buf, size_t size);
static size_t dev_uart_write(struct file_desc *desc, void *buf, size_t size);
static int dev_uart_ioctl(struct file_desc *desc, int request, va_list args);

kfile_operations_t uart_dev_file_op = {
	.open = dev_uart_open,
	.close = dev_uart_close,
	.read = dev_uart_read,
	.write = dev_uart_write,
	.ioctl = dev_uart_ioctl
};

RING_BUFFER_DEF(dev_buff, int, 0x20);

static struct event rx_happend;

static irq_return_t irq_handler(unsigned int irq_nr, void *data) {
	struct uart_device *dev;
	int ch;

	dev = (struct uart_device *)data;

	while(dev->operations->hasrx(dev)) {
		ch = dev->operations->get(dev);
		ring_buff_enqueue(&dev_buff, &ch, 1);
		event_notify(&rx_happend);
	}

	return 0;
}

/*
 * file_operations
 */
static int dev_uart_open(struct node *node, struct file_desc *desc, int flags) {
	struct uart_device *uart_dev;
	uart_dev = uart_dev_lookup((char *)node->name);

	if(NULL == uart_dev || uart_dev->fops) {
		return -1;
	}

	if(uart_dev->operations->setup) {
		uart_dev->operations->setup(uart_dev, uart_dev->params);
	}

	desc->ops = &uart_dev_file_op;

	irq_attach(uart_dev->irq_num, irq_handler, 0, (void *)uart_dev, uart_dev->dev_name);

	return 0;
}

static int dev_uart_close(struct file_desc *desc) {
	struct uart_device *uart_dev;
	uart_dev = uart_dev_lookup((char *)desc->node->name);


	irq_detach(uart_dev->irq_num, uart_dev);
	return 0;
}

static size_t dev_uart_read(struct file_desc *desc, void *buff, size_t size) {
	size_t cnt = size;

	if(0 == ring_buff_get_cnt(&dev_buff)) {
		sched_lock();
			irq_lock();
				event_init(&rx_happend, "event_rx_happend");
			irq_unlock();

			event_wait_ms(&rx_happend, SCHED_TIMEOUT_INFINITE);
		sched_unlock();
	}

	for(;0 < cnt;--cnt) {
		int tmp;
		char *tmp_char = buff;

		ring_buff_dequeue(&dev_buff, &tmp, 1);
		*tmp_char = (char)tmp;


		buff = (void *)(((uint32_t)buff) + 1);
	}

	return size - cnt;
}

static size_t dev_uart_write(struct file_desc *desc, void *buff, size_t size) {
	struct uart_device *uart_dev = uart_dev_lookup((char*)desc->node->name);
	size_t cnt;
	char *b;

	cnt = 0;
	b = (char*) buff;

	while (cnt != size) {
		uart_dev->operations->put(uart_dev, b[cnt++]);
	}
	return 0;
}

static int dev_uart_ioctl(struct file_desc *desc, int request, va_list args) {
	return 0;
}

int uart_dev_register(struct uart_device *dev) {
	struct node *nod, *devnod;
	struct nas *dev_nas;

	//TODO tmp (we can have only one device)
	uart_dev = dev;

	/* register char device */
	if (NULL == (nod = vfs_lookup(NULL, "/dev"))) {
		return -1;
	}
	if (NULL == (devnod = vfs_add_path(dev->dev_name, nod))) {
		return -1;
	}

	dev_nas = devnod->nas;
	if(NULL == (dev_nas->fs = alloc_filesystem("empty"))) {
		return -1;
	}
	//strncpy((char*)devnod->name, dev->dev_name, sizeof(devnod->name) - 1);

	dev_nas->fs->file_op = &uart_dev_file_op;

	return 0;
}
