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
#include <fs/dvfs.h>

static struct idesc * uart_fsop_open(struct inode *node, struct idesc *desc) {
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

const struct file_operations ttys_fops = {
	.open = uart_fsop_open,
};

POOL_DEF(cdev_serials_pool, struct device_module, 1);

int ttys_register(const char *name, void *dev_info) {
	struct device_module *cdev;

	cdev = pool_alloc(&cdev_serials_pool);
	if (!cdev) {
		return -ENOMEM;
	}
	memset(cdev, 0, sizeof(*cdev));
	cdev->name = name;
	cdev->fops = (struct file_operations *)&ttys_fops;
	cdev->dev_data = dev_info;

	return char_dev_register(cdev);
}
