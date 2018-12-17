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
#include <drivers/device.h>
#include <drivers/serial/uart_device.h>
#include "idesc_serial.h"
#include <fs/dvfs.h>

static struct idesc *uart_fsop_open(struct inode *node, struct idesc *desc) {
	struct dev_module *cdev;
	struct idesc *idesc;
	int res;

	cdev = node->i_data;
	idesc = idesc_serial_create(cdev->dev_priv, 0);
	if (err(idesc)) {
		return idesc;
	}
	res = uart_open(cdev->dev_priv);
	if (res) {
		return err_ptr(-res);
	}

	node->flags |= DVFS_NO_LSEEK;

	return idesc;
}

struct file_operations ttys_fops = {
	.open = uart_fsop_open,
};

#define SERIAL_POOL_SIZE OPTION_GET(NUMBER, serial_quantity)
POOL_DEF(cdev_serials_pool, struct dev_module, SERIAL_POOL_SIZE);

extern const struct idesc_ops idesc_serial_ops;

int ttys_open(struct dev_module *mod, void *dev_priv) {
	return uart_open(mod->dev_priv);
}

int ttys_register(const char *name, void *dev_info) {
	struct dev_module *cdev;

	cdev = pool_alloc(&cdev_serials_pool);
	if (!cdev) {
		return -ENOMEM;
	}
	memset(cdev, 0, sizeof(*cdev));
	memcpy(cdev->name, name, sizeof(cdev->name));
	cdev->dev_priv = dev_info;
	cdev->dev_file.f_ops = &ttys_fops;

	return char_dev_register(cdev);
}
