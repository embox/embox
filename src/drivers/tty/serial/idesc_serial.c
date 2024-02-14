/**
 * @file
 *
 * @date Dec 18, 2013
 * @author: Anton Bondarev
 */
#include <assert.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include <drivers/char_dev.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/tty.h>
#include <drivers/ttys.h>
#include <framework/mod/options.h>
#include <kernel/panic.h>
#include <kernel/task/resource/idesc.h>
#include <mem/misc/pool.h>
#include <util/err.h>

#include "idesc_serial.h"

#define MAX_IDESC_SERIALS OPTION_GET(NUMBER, idesc_serial_quantity)

POOL_DEF(uart_ttys, struct tty_uart, MAX_IDESC_SERIALS);

#define idesc_to_uart(idesc) (((struct tty_uart *)(idesc->idesc_priv))->uart)

const struct idesc_ops idesc_serial_ops;

static ssize_t serial_read(struct idesc *idesc, const struct iovec *iov,
    int cnt) {
	void *buf;
	size_t nbyte;
	struct uart *uart;

	assert(iov);
	buf = iov->iov_base;
	assert(cnt == 1);
	nbyte = iov->iov_len;

	assert(buf);
	assert(idesc);
	assert(idesc->idesc_ops == &idesc_serial_ops);
	assert(((idesc->idesc_flags & O_ACCESS_MASK) != O_WRONLY));

	if (!nbyte) {
		return 0;
	}

	uart = idesc_to_uart(idesc);
	assert(uart);
	assert(uart->tty);

	return tty_read(uart->tty, (char *)buf, nbyte);
}

static ssize_t serial_write(struct idesc *idesc, const struct iovec *iov,
    int cnt) {
	void *buf;
	size_t nbyte;
	int ch;
	struct uart *uart;
	size_t written, left;

	assert(iov);
	buf = iov->iov_base;
	assert(cnt == 1);
	nbyte = iov->iov_len;

	assert(buf);
	assert(idesc);
	assert(idesc->idesc_ops == &idesc_serial_ops);
	assert(((idesc->idesc_flags & O_ACCESS_MASK) != O_RDONLY));

	left = nbyte;
	uart = idesc_to_uart(idesc);
	assert(uart);
	assert(uart->tty);

	do {
		written = tty_write(uart->tty, buf, left);

		while (-1 != (ch = tty_out_getc(uart->tty))) {
			uart_putc(uart, ch);
		}

		left -= written;
		buf = (void *)((char *)buf + written);
	} while (left != 0);

	return (ssize_t)nbyte;
}

static void idesc_serial_close(struct idesc *idesc);
static void serial_close(struct idesc *idesc) {
	struct uart *uart;
	int res;

	assert(idesc);
	assert(idesc->idesc_ops == &idesc_serial_ops);

	uart = idesc_to_uart(idesc);
	assert(uart);
	res = uart_close(uart);
	if (res) {
		panic("Failed to close UART");
	}

	idesc_serial_close(idesc);
}

static int serial_ioctl(struct idesc *idesc, int request, void *data) {
	struct uart *uart;

	assert(idesc);
	assert(idesc->idesc_ops == &idesc_serial_ops);

	uart = idesc_to_uart(idesc);
	assert(uart);

	return tty_ioctl(uart->tty, request, data);
}

static int serial_status(struct idesc *idesc, int mask) {
	struct uart *uart;
	int res = 0;

	assert(idesc);

	if (!mask) {
		return 0;
	}
	uart = idesc_to_uart(idesc);
	assert(uart);

	if (mask & POLLIN) {
		/* how many we can read */
		res += tty_status(uart->tty, POLLIN);
	}

	if (mask & POLLOUT) {
		/* how many we can write */
		res += INT_MAX;
	}

	if (mask & POLLERR) {
		/* is there any exeptions */
		res += 0; //TODO Where is errors counter
	}

	return res;
}

static int serial_open(struct idesc *idesc, void *source) {
	extern irq_return_t uart_irq_handler(unsigned int irq_nr, void *data);
	extern struct tty_ops uart_tty_ops;

	struct dev_module *cdev;
	struct tty_uart *tu;
	struct uart *uart;
	int err;

	assert(source);

	cdev = (struct dev_module *)source;
	uart = (struct uart *)(cdev->dev_priv);

	if (uart->tty) {
		tu = member_cast_out(uart->tty, struct tty_uart, tty);
		if (pool_belong(&uart_ttys, tu)) {
			return 0;
		}
	}

	tu = pool_alloc(&uart_ttys);
	if (!tu) {
		return ENOMEM;
	}

	idesc->idesc_priv = tu;

	tty_init(&tu->tty, &uart_tty_ops);
	tu->tty.termios.c_ispeed = uart->params.baud_rate;
	tu->tty.termios.c_ospeed = uart->params.baud_rate;

	tu->uart = uart;
	uart->tty = &tu->tty;
	uart->tty->idesc = idesc;

	if (uart->irq_handler == NULL) {
		uart->irq_handler = uart_irq_handler;
	}

	err = uart_open(uart);
	if (err) {
		pool_free(&uart_ttys, tu);
	}

	return err;
}

static void idesc_serial_close(struct idesc *idesc) {
	struct tty_uart *tu;
	struct uart *uart;

	uart = idesc_to_uart(idesc);
	uart->tty->idesc = NULL;

	tu = member_cast_out(uart->tty, struct tty_uart, tty);
	uart->tty = NULL;

	pool_free(&uart_ttys, tu);
}

const struct idesc_ops idesc_serial_ops = {
    .id_readv = serial_read,
    .id_writev = serial_write,
    .ioctl = serial_ioctl,
    .open = serial_open,
    .close = serial_close,
    .status = serial_status,
    .fstat = char_dev_default_fstat,
};

const struct idesc_ops *idesc_serial_get_ops(void) {
	return &idesc_serial_ops;
}
