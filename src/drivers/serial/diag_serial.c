/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    19.08.2013
 */

#include <drivers/diag.h>
#include <drivers/serial/uart_dev.h>

#include <drivers/serial/diag_serial.h>

static int uart_diag_init(const struct diag *diag) {
	struct uart_diag *uart_diag = (struct uart_diag *) diag;

	uart_set_params(uart_diag->uart, uart_diag->params);

	/* XXX */
	/* set params */
	uart_open(uart_diag->uart);
	/* allow future `open' */
	uart_close(uart_diag->uart);

	return 0;
};

static void uart_diag_putc(const struct diag *diag, char ch) {
	struct uart_diag *uart_diag = (struct uart_diag *) diag;

	uart_putc(uart_diag->uart, ch);
}

static char uart_diag_getc(const struct diag *diag) {
	struct uart_diag *uart_diag = (struct uart_diag *) diag;

	return uart_getc(uart_diag->uart);
}

static int uart_diag_kbhit(const struct diag *diag) {
	struct uart_diag *uart_diag = (struct uart_diag *) diag;

	return uart_hasrx(uart_diag->uart);
}

const struct diag_ops uart_diag_ops = {
	.init = uart_diag_init,
	.putc = uart_diag_putc,
	.getc = uart_diag_getc,
	.kbhit = uart_diag_kbhit,
};

