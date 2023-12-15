/**
 * @file
 *
 * @date 29.04.2016
 * @author: Anton Bondarev
 */

#include <drivers/serial/uart_dev.h>
#include <drivers/tty.h>
#include <drivers/ttys.h>

static inline struct uart *tty2uart(struct tty *tty) {
	struct tty_uart *tu;
	tu = member_cast_out(tty, struct tty_uart, tty);
	return tu->uart;
}

static void uart_out_wake(struct tty *t) {
	struct uart *uart_dev = tty2uart(t);
	int ich;

	irq_lock();

	while ((ich = tty_out_getc(t)) != -1)
		uart_putc(uart_dev, (char)ich);

	irq_unlock();
}

static void uart_term_setup(struct tty *tty, struct termios *termios) {
	struct uart *uart_dev = tty2uart(tty);
	struct uart_params params;

	uart_get_params(uart_dev, &params);

	/* TODO baud rate is ospeed. What's with ispeed ? */
	if (params.baud_rate != termios->c_ospeed) {
		params.baud_rate = termios->c_ospeed;
		uart_set_params(uart_dev, &params);
	}
}

struct tty_ops uart_tty_ops = {
    .setup = uart_term_setup,
    .out_wake = uart_out_wake,
};
