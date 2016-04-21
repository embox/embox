/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.08.2013
 */
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <util/err.h>
#include <util/indexator.h>

#include <mem/misc/pool.h>
#include <drivers/char_dev.h>
#include <drivers/serial/uart_device.h>
#include <fs/idesc_serial.h>
#include <fs/dvfs.h>

static struct idesc *uart_fsop_open(struct inode *node, struct idesc *desc) {
	struct device_module *cdev;
	struct idesc *idesc;
	int res;

	cdev = node->i_data;
	idesc = idesc_serial_create(cdev->dev_data, 0);
	if (err(idesc)) {
		return idesc;
	}
	res = uart_open(cdev->dev_data);
	if (res) {
		return err_ptr(-res);
	}

	return idesc;
}

static int uart_fsop_close(struct file *desc) {
	return 0;
}

static size_t uart_fsop_read(struct file *desc, void *buf, size_t size) {
	struct uart *uart;
	int i;
	char *b;
	struct device_module *cdev;

	b = buf;
	cdev = desc->f_inode->i_data;
	uart = cdev->dev_data;

	for(i = 0; i < size; i ++) {
		while(!uart->uart_ops->uart_hasrx(uart)) {
		}
		b[i] = uart->uart_ops->uart_getc(uart);
	}

	return size;
}

static size_t uart_fsop_write(struct file *desc, void *buf, size_t size) {
	struct uart *uart;
	int i;
	char *b;
	struct device_module *cdev;

	b = buf;
	cdev = desc->f_inode->i_data;
	uart = cdev->dev_data;

	for(i = 0; i < size; i ++) {
		uart->uart_ops->uart_putc(uart, b[i]);
	}
	return 0;
}

const struct file_operations ttys_fops = {
	.open = uart_fsop_open,
	.close = uart_fsop_close,
	.read = uart_fsop_read,
	.write = uart_fsop_write
};
