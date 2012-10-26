/**
 * @file
 *
 * @date Oct 25, 2012
 * @author: Anton Bondarev
 */
#include <types.h>

#include <util/ring_buff.h>

#include <fs/file_desc.h>
#include <fs/node.h>
#include <kernel/file.h>

#include <drivers/uart_device.h>

#include <kernel/irq.h>

#include <kernel/thread/event.h>
#include <kernel/thread/sched_lock.h>
#include <kernel/thread/sched.h>

static struct uart_device *uart_dev;

static struct uart_device *uart_dev_lookup(char *name) {
	return uart_dev;
}

static void *dev_uart_open(struct file_desc *desc, const char *mode);
static int dev_uart_close(struct file_desc *desc);
static size_t dev_uart_read(void *buf, size_t size, size_t count, void *file);
static size_t dev_uart_write(const void *buff, size_t size, size_t count, void *file);
static int dev_uart_ioctl(void *file, int request, va_list args);

file_operations_t uart_dev_file_op = {
	.fread = dev_uart_read,
	.fopen = dev_uart_open,
	.fclose = dev_uart_close,
	.fwrite = dev_uart_write,
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
static void *dev_uart_open(struct file_desc *desc, const char *mode) {
	struct uart_device *uart_dev;
	uart_dev = uart_dev_lookup((char *)desc->node->name);

	if(NULL == uart_dev || uart_dev->fops) {
		return NULL;
	}

	if(uart_dev->operations->setup) {
		uart_dev->operations->setup(uart_dev, uart_dev->params);
	}

	desc->ops = &uart_dev_file_op;

	irq_attach(uart_dev->irq_num, irq_handler, 0, (void *)uart_dev, uart_dev->dev_name);

	return (void *) desc;
}

static int dev_uart_close(struct file_desc *desc) {
	struct uart_device *uart_dev;
	uart_dev = uart_dev_lookup((char *)desc->node->name);


	irq_detach(uart_dev->irq_num, uart_dev);
	return 0;
}

static size_t dev_uart_read(void *buff, size_t size, size_t count, void *file) {
	size_t cnt = count;

	if(0 == ring_buff_get_cnt(&dev_buff)) {
		sched_lock();
			irq_lock();
				event_init(&rx_happend, "event_rx_happend");
			irq_unlock();

			event_wait(&rx_happend, SCHED_TIMEOUT_INFINITE);
		sched_unlock();
	}

	for(;0 < cnt;--cnt) {
		int tmp;
		char *tmp_char = buff;

		ring_buff_dequeue(&dev_buff, &tmp, 1);
		*tmp_char = (char)tmp;


		buff = (void *)(((uint32_t)buff) + size);
	}

	return count - cnt;
}

static size_t dev_uart_write(const void *buff, size_t size, size_t count, void *file) {
	struct file_desc *desc = (struct file_desc *)file;
	struct uart_device *uart_dev = uart_dev_lookup((char*)desc->node->name);
	size_t cnt;
	char *b;

	cnt = 0;
	b = (char*) buff;

	while (cnt != count * size) {
		uart_dev->operations->put(uart_dev, b[cnt++]);
	}
	return 0;
}

static int dev_uart_ioctl(void *file, int request, va_list args) {
	return 0;
}

int uart_dev_register(struct uart_device *dev) {
	uart_dev = dev;
	return 0;
}
