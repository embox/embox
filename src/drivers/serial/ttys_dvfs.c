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

#define UART_MAX_N OPTION_GET(NUMBER,uart_max_n)

INDEX_DEF(serial_indexator, 0, UART_MAX_N);

POOL_DEF(cdev_serials_pool, struct device_module, 1);


static int uart_fill_name(struct uart *dev) {

	dev->idx = index_alloc(&serial_indexator, INDEX_MIN);
	if(dev->idx < 0) {
		return -EBUSY;
	}

	snprintf(dev->dev_name, UART_NAME_MAXLEN, "ttyS%d", dev->idx);

	return 0;
}

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

static struct file_operations uart_fops = {
	.open = uart_fsop_open,
	.close = uart_fsop_close,
	.read = uart_fsop_read,
	.write = uart_fsop_write
};

int uart_register(struct uart *uart,
		const struct uart_params *uart_defparams) {
	struct device_module *cdev;

	cdev = pool_alloc(&cdev_serials_pool);
	if (!cdev) {
		return -ENOMEM;
	}

	if (uart_fill_name(uart)) {
		pool_free(&cdev_serials_pool, cdev);
		return -EBUSY;
	}

	if (uart_defparams) {
		memcpy(&uart->params, uart_defparams, sizeof(struct uart_params));
	} else {
		memset(&uart->params, 0, sizeof(struct uart_params));
	}

	memset(cdev, 0, sizeof(*cdev));
	cdev->name = uart->dev_name;
	cdev->fops = &uart_fops;
	cdev->dev_data = uart;

	char_dev_register(cdev);

	return 0;
}

void uart_deregister(struct uart *uart) {

	dlist_del(&uart->lnk);

	index_free(&serial_indexator, uart->idx);
}

