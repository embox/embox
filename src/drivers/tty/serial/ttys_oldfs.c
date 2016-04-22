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
#include "idesc_serial.h"
#include <fs/file_operation.h>
#include <fs/node.h>


static int uart_fsop_open(struct node *node, struct file_desc *file_desc, int flags)  {
	return -1;
}

static int uart_fsop_close(struct file_desc *desc) {
	return 0;
}

static size_t uart_fsop_read(struct file_desc *desc, void *buf, size_t size) {

	return 0;
}

static size_t uart_fsop_write(struct file_desc *desc, void *buf, size_t size) {
	return 0;
}

const struct kfile_operations ttys_fops = {
	.open = uart_fsop_open,
	.close = uart_fsop_close,
	.read = uart_fsop_read,
	.write = uart_fsop_write
};
