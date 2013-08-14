/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.08.2013
 */

#include <string.h>
#include <stdio.h>
#include <kernel/irq.h>
#include <mem/misc/pool.h>
#include <util/indexator.h>
#include <drivers/serial/fsnode.h>

#include <drivers/uart_device.h>

#define UART_MAX_N OPTION_GET(NUMBER,uart_max_n)

POOL_DEF(uart_pool, struct uart, UART_MAX_N);
INDEX_DEF(serial_indexator, 0, UART_MAX_N);

static DLIST_DEFINE(uart_list);

static irq_return_t irq_handler(unsigned int irq_nr, void *data);

static void uart_out_wake(struct tty *t);
static void uart_term_setup(struct tty *tty, struct termios *termios);

static struct tty_ops uart_tty_ops = {
	.setup = uart_term_setup,
	.out_wake = uart_out_wake,
};

static inline int uart_state_test(struct uart *uart, int mask) {
	return uart->state & mask;
}

static inline void uart_state_set(struct uart *uart, int mask) {
	uart->state |= mask;
}

static inline void uart_state_clear(struct uart *uart, int mask) {
	uart->state &= ~mask;
}

static int uart_fill_name(struct uart *dev) {

	dev->idx = index_alloc(&serial_indexator, INDEX_ALLOC_MIN);
	if(dev->idx < 0) {
		return -EBUSY;
	}

	snprintf(dev->dev_name, UART_NAME_MAXLEN, "ttyS%d", dev->idx);

	return 0;
}


static struct uart *uart_alloc(const struct uart_desc *uartd) {
	struct uart *uart = pool_alloc(&uart_pool);

	if (!uart) {
		return NULL;
	}

	uart->uart_desc = uartd;
	dlist_head_init(&uart->lnk);
	memset(&uart->tty, 0, sizeof(struct tty));
	memset(&uart->params, 0, sizeof(struct uart_params));

	return uart;
}

static void uart_free(struct uart *uart) {
	pool_free(&uart_pool, uart);
}

static int uart_attach_irq(struct uart *uart) {

	if (uart->params.irq) {
		return irq_attach(uart->uart_desc->irq_num, irq_handler, 0, uart,
				uart->dev_name);
	}

	return 0;
}

static int uart_detach_irq(struct uart *uart) {

	if (uart->params.irq) {
		return irq_detach(uart->uart_desc->irq_num, uart);
	}

	return 0;
}

static int uart_setup(struct uart *uart) {
	const struct uart_ops *uops = uart->uart_desc->uart_ops;

	if (uops->uart_setup) {
		return uops->uart_setup(uart->uart_desc, &uart->params);
	}

	return 0;
}

static irq_return_t irq_handler(unsigned int irq_nr, void *data) {
	struct uart *dev = data;

	while (uart_hasrx(dev)) {
		tty_rx_putc(&dev->tty, uart_getc(dev), 0);
	}

	return IRQ_HANDLED;
}

static void uart_out_wake(struct tty *t) {
	struct uart *uart_dev = member_cast_out(t, struct uart, tty);
	int ich;

	while ((ich = tty_out_getc(t)) != -1)
		uart_putc(uart_dev, (char) ich);
}

static void uart_term_setup(struct tty *tty, struct termios *termios) {
	struct uart *uart = member_cast_out(tty, struct uart, tty);

	/* TODO baud rate is ospeed. What's with ispeed ? */
	uart->params.baud_rate = termios->c_ospeed;

	uart_setup(uart);
}

struct uart *uart_register(const struct uart_desc *uartd,
		const struct uart_params *uart_defparams) {
	struct uart *uart = NULL;

	if (!(uart = uart_alloc(uartd))) {
		return NULL;
	}

	if (uart_defparams) {
		memcpy(&uart->params, uart_defparams, sizeof(struct uart_params));
	}

	if (uart_fill_name(uart)) {
		goto out_err;
	}

	uart->uart_desc = uartd;
	dlist_add_next(&uart->lnk, &uart_list);

	serial_register_devfs(uart);

	return uart;

out_err:
	uart_free(uart);
	return NULL;
}

void uart_deregister(struct uart *uart) {

	dlist_del(&uart->lnk);

	index_free(&serial_indexator, uart->idx);

	uart_free(uart);
}

struct uart *uart_dev_lookup(const char *name) {
	struct uart *uart, *nxt;

	dlist_foreach_entry(uart, nxt, &uart_list, lnk) {
		if (!name || !strcmp(name, uart->dev_name)) {
			return uart;
		}
	}

	return NULL;
}

int uart_open(struct uart *uart) {
	if (uart_state_test(uart, UART_STATE_OPEN)) {
		return -EINVAL;
	}

	uart_state_set(uart, UART_STATE_OPEN);

	tty_init(&uart->tty, &uart_tty_ops);

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

