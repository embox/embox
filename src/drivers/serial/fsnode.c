/**
 * @file
 *
 * @date Oct 25, 2012
 * @author: Anton Bondarev
 */

#include <stdio.h>
#include <errno.h>

#include <kernel/irq.h>
#include <drivers/uart_device.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>
#include <fs/idesc_serial.h>
#include <fs/node.h>
#include <fs/path.h>

#include <embox/device.h> //XXX

#include <drivers/serial/fsnode.h>

static int dev_uart_open(struct node *node, struct file_desc *file_desc,
	int flags);
//static int dev_uart_close(struct file_desc *desc);
//static size_t dev_uart_read(struct file_desc *desc, void *buf, size_t size);
//static size_t dev_uart_write(struct file_desc *desc, void *buf, size_t size);
//static int dev_uart_ioctl(struct file_desc *desc, int request, ...);

static struct kfile_operations uart_dev_file_op = {
	.open = dev_uart_open,
	//.close = dev_uart_close,
	//.read = dev_uart_read,
	//.write = dev_uart_write,
	//.ioctl = dev_uart_ioctl
};

int serial_register_devfs(struct uart *dev) {

	char_dev_register(dev->dev_name, &uart_dev_file_op);

	return ENOERR;
}

/*
 * file_operations
 */
static int dev_uart_open(struct node *node, struct file_desc *desc, int flags) {
	struct uart *uart_dev = uart_dev_lookup(node->name);

	if (!uart_dev) {
		return -ENOENT;
	}

	assert(desc);

	desc->file_info = uart_dev;
	idesc_serial_create(desc, uart_dev, flags);

	return 0;
}
#if 0
static int dev_uart_close(struct file_desc *desc) {
	struct uart *uart_dev = desc->file_info;

	return uart_close(uart_dev);
}

static size_t dev_uart_read(struct file_desc *desc, void *buff, size_t size) {
	struct uart *uart_dev = desc->file_info;

	return tty_read(&uart_dev->tty, (char *) buff, size);
}

static size_t dev_uart_write(struct file_desc *desc, void *buff, size_t size) {
	struct uart *uart_dev = desc->file_info;
	struct tty *tty = &uart_dev->tty;
	size_t written, left = size;

	do {
		written = tty_write(tty, buff, left);

		left -= written;
		buff = (char *) buff + written;
	} while (left != 0);

	return size;
}

static int dev_uart_ioctl(struct file_desc *desc, int request, ...) {
	va_list va;
	void *data;
	struct uart *uart_dev = desc->file_info;

	va_start(va, request);
	data = va_arg(va, void *);
	va_end(va);

	return tty_ioctl(&uart_dev->tty, request, data);
}
#endif
