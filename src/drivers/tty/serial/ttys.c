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

#include <drivers/char_dev.h>
#include <drivers/device.h>
#include <drivers/serial/uart_dev.h>
#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <util/err.h>
#include <util/indexator.h>

#include "idesc_serial.h"
#include "ttys.h"

#define SERIAL_POOL_SIZE OPTION_GET(NUMBER, serial_quantity)

POOL_DEF(cdev_serials_pool, struct dev_module, SERIAL_POOL_SIZE);

static struct idesc *uart_cdev_open(struct dev_module *cdev, void *priv) {
	struct idesc *idesc;

	idesc = idesc_serial_open(cdev->dev_priv, (uintptr_t)priv);
	if (err(idesc)) {
		return NULL;
	}

	return idesc;
}

int ttys_register(const char *name, void *dev_info) {
	struct dev_module *cdev;

	cdev = pool_alloc(&cdev_serials_pool);
	if (!cdev) {
		return -ENOMEM;
	}
	memset(cdev, 0, sizeof(*cdev));
	memcpy(cdev->name, name, sizeof(cdev->name));
	cdev->dev_ops = &uart_cdev_ops;
	cdev->dev_iops = idesc_serial_get_ops();
	cdev->dev_priv = dev_info;

	return char_dev_register(cdev);
}

const struct dev_module_ops uart_cdev_ops = {
    .dev_open = uart_cdev_open,
};
