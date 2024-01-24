/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.08.2013
 */

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <util/err.h>
#include <util/log.h>
#include <lib/libds/array.h>

#include <drivers/device.h>
#include <drivers/char_dev.h>
#include <drivers/serial/uart_dev.h>

ARRAY_SPREAD_DECLARE(struct uart *const, __uart_device_registry);

int uart_register(struct uart *uart,
		const struct uart_params *uart_defparams) {
	struct uart * existed_uart_dev;

	array_spread_foreach(existed_uart_dev, __uart_device_registry)  {
		if (uart == existed_uart_dev) {
			/* uart has been inited statically */
			return 0;
		}
	}

	log_error(" The only static uart repo available. Use TTYS_DEF macro");

	return -ENOTSUP;
}

void uart_deregister(struct uart *uart) {
}

struct uart *uart_dev_lookup(const char *name) {
	struct uart *uart = NULL;

	array_spread_foreach(uart, __uart_device_registry) {
		if (0 == strcmp(uart->dev_name, name) ) {
			return uart;
		}
	}

	return NULL;
 }
