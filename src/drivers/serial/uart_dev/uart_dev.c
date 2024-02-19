/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.08.2013
 */

#include <string.h>
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

ARRAY_SPREAD_DEF(struct uart *const, __uart_device_registry);

DLIST_DEFINE(uart_list);

struct dlist_head *uart_get_list(void) {
	return &uart_list;
}

static int uart_attach_irq(struct uart *uart) {
	int r;

	if (!(uart->params.uart_param_flags & UART_PARAM_FLAGS_USE_IRQ)) {
		return 0;
	}

	if (!uart->irq_handler) {
		return -EINVAL;
	}

	r = irq_attach(uart->irq_num, uart->irq_handler, 0, uart, uart->dev_name);

	log_debug("setup tty %s irq num %d result %d", uart->dev_name , uart->irq_num, r);

	return r;
}

static int uart_detach_irq(struct uart *uart) {

	if (uart->params.uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		return irq_detach(uart->irq_num, uart);
	}

	return 0;
}

static int uart_setup(struct uart *uart) {
	const struct uart_ops *uops = uart->uart_ops;

	if (uops->uart_setup) {
		log_debug("setup tty %s", uart->dev_name);
		return uops->uart_setup(uart, &uart->params);
	}

	return 0;
}

static void uart_internal_init(struct uart *uart) {
	if (uart_state_test(uart, UART_STATE_INITED)) {
		return;
	}

	uart_state_set(uart, UART_STATE_INITED);

	ring_buff_init(&uart->uart_rx_ring, sizeof(uart->uart_rx_buff[0]),
			UART_RX_BUFF_SZ, uart->uart_rx_buff);

	dlist_add_next(&uart->uart_lnk, &uart_list);
}

int uart_open(struct uart *uart) {
	if (uart_state_test(uart, UART_STATE_OPEN)) {
		return -EINVAL;
	}
	uart_state_set(uart, UART_STATE_OPEN);

	uart_internal_init(uart);

	uart_setup(uart);

	return uart_attach_irq(uart);
}

int uart_close(struct uart *uart) {
	if (!uart_state_test(uart, UART_STATE_OPEN)) {
		return -EINVAL;
	}

	uart_state_clear(uart, UART_STATE_OPEN);

	return uart_detach_irq(uart);
}

int uart_set_params(struct uart *uart, const struct uart_params *params) {

	if (uart_state_test(uart, UART_STATE_OPEN)) {
		uart_detach_irq(uart);
	}

	memcpy(&uart->params, params, sizeof(struct uart_params));

	if (uart_state_test(uart, UART_STATE_OPEN)) {
		uart_attach_irq(uart);
		uart_setup(uart);
	}

	return 0;
}

int uart_get_params(struct uart *uart, struct uart_params *params) {

	memcpy(params, &uart->params, sizeof(struct uart_params));

	return 0;
}
