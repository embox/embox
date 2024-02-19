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
#include <lib/libds/indexator.h>
#include <lib/libds/dlist.h>
#include <lib/libds/ring_buff.h>
#include <lib/libds/array.h>

#include <kernel/irq.h>
#include <mem/misc/pool.h>

#include <drivers/device.h>
#include <drivers/serial/uart_dev.h>

ARRAY_SPREAD_DECLARE(struct uart *const, __uart_device_registry);

extern int ttys_register(const char *name, void *dev_info);


#define UART_MAX_N OPTION_GET(NUMBER,uart_max_n)


INDEX_DEF(serial_indexator, 0, UART_MAX_N);
static struct uart *__uart_device_dynamic_registry[UART_MAX_N];



int uart_register(struct uart *uart,
		const struct uart_params *uart_defparams) {
	int res;
	size_t allocated_idx;
	struct uart * existed_uart_dev;

	array_spread_foreach(existed_uart_dev, __uart_device_registry)  {
		if (uart == existed_uart_dev) {
			/* uart has been inited statically */
			return 0;
		}
	}

	allocated_idx = index_alloc(&serial_indexator, INDEX_MIN);
	if (allocated_idx == INDEX_NONE) {
		return -EBUSY;
	}
	uart->idx = (unsigned char)allocated_idx;

	snprintf(uart->dev_name, UART_NAME_MAXLEN, "ttyS%d", uart->idx);

	if (uart_defparams) {
		memcpy(&uart->params, uart_defparams, sizeof(struct uart_params));
	} else {
		memset(&uart->params, 0, sizeof(struct uart_params));
	}

	res = ttys_register(uart->dev_name, uart);
	if (0 != res) {
		log_error("Failed to register tty %s", uart->dev_name);
		index_free(&serial_indexator, uart->idx);
		return res;
	}

	__uart_device_dynamic_registry[uart->idx ] = uart;

	return 0;
}

void uart_deregister(struct uart *uart) {

	dlist_del(&uart->uart_lnk);

	index_free(&serial_indexator, uart->idx);
}

struct uart *uart_dev_lookup(const char *name) {
	int i;
	struct uart *uart = NULL;

	array_spread_foreach(uart, __uart_device_registry) {
		if (0 == strcmp(uart->dev_name, name) ) {
			return uart;
		}
	}
	for(i = 0 ; i < ARRAY_SIZE(__uart_device_dynamic_registry); i++) {
		if (NULL == __uart_device_dynamic_registry[i]) {
			continue;
		}
		uart = __uart_device_dynamic_registry[i];
		if (0 == strcmp(uart->dev_name, name) ) {
			return uart;
		}
	}

	return NULL;
 }

