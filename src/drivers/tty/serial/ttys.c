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
#include <drivers/ttys.h>
#include <framework/mod/options.h>
#include <kernel/task/resource/idesc.h>
#include <lib/libds/indexator.h>
#include <mem/misc/pool.h>
#include <util/err.h>

#define SERIAL_POOL_SIZE OPTION_GET(NUMBER, serial_quantity)

POOL_DEF(serial_dev_pool, struct serial_dev, SERIAL_POOL_SIZE);

int ttys_register(const char *name, struct uart *uart) {
	struct serial_dev *dev;

	dev = pool_alloc(&serial_dev_pool);
	if (!dev) {
		return -ENOMEM;
	}

	char_dev_init(&dev->cdev, name, serial_dev_get_ops());

	dev->uart = uart;

	return char_dev_register((struct char_dev *)dev);
}
