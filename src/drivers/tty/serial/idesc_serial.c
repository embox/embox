/**
 * @file
 *
 * @date Dec 18, 2013
 * @author: Anton Bondarev
 */
#include <assert.h>

#include <util/err.h>
#include <poll.h>

#include <mem/misc/pool.h>
#include <fs/idesc.h>

#include <framework/mod/options.h>

#include <drivers/tty.h>
#include <fs/file_desc.h>
#include <fs/idesc_serial.h>
#include <drivers/serial/uart_device.h>

#define MAX_SERIALS \
	OPTION_GET(NUMBER, serial_quantity)

//POOL_DEF(pool_serials, struct idesc_serial, MAX_SERIALS);

#define idesc_to_uart(desc) \
	(((struct file_desc *)desc)->file_info)

static const struct idesc_ops idesc_serial_ops;

struct tty_uart {
	struct tty tty;
	struct uart *uart;
};

POOL_DEF(uart_ttys, struct tty_uart, MAX_SERIALS);

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
		uart_putc(uart_dev, (char) ich);

	irq_unlock();
}

static void uart_term_setup(struct tty *tty, struct termios *termios) {
	struct uart *uart_dev = tty2uart(tty);
	struct uart_params params;

	uart_get_params(uart_dev, &params);

	/* TODO baud rate is ospeed. What's with ispeed ? */
	params.baud_rate = termios->c_ospeed;

	uart_set_params(uart_dev, &params);
}

static struct tty_ops uart_tty_ops = {
	.setup = uart_term_setup,
	.out_wake = uart_out_wake,
};

static irq_return_t uart_irq_handler(unsigned int irq_nr, void *data) {
	struct uart *dev = data;

	if (dev->tty) {
		while (uart_hasrx(dev))
			tty_rx_putc(dev->tty, uart_getc(dev), 0);
	}

	return IRQ_HANDLED;
}

static int idesc_uart_bind(struct uart *uart, struct idesc *idesc) {
	struct tty_uart *tu;

	tu = pool_alloc(&uart_ttys);
	if (!tu) {
		return -ENOMEM;
	}

	tty_init(&tu->tty, &uart_tty_ops);

	tu->uart = uart;
	uart->tty = &tu->tty;
	uart->tty->idesc = idesc;
	uart->irq_handler = uart_irq_handler;

	return 0;
}

static void idesc_uart_unbind(struct uart *uart) {
	struct tty_uart *tu;

	uart->tty->idesc = NULL;
	tu = member_cast_out(uart->tty, struct tty_uart, tty);
	pool_free(&uart_ttys, tu);
}

struct idesc *idesc_serial_create(struct file_desc *fdesc, struct uart *uart,
		idesc_access_mode_t mod) {
	//struct idesc_serial *idesc;

	assert(uart);
	assert(mod);

	idesc_init(&fdesc->idesc, &idesc_serial_ops, FS_MAY_READ | FS_MAY_WRITE);

	if (idesc_uart_bind(uart, &fdesc->idesc)) {
		return NULL;
	}

	if (uart_open(uart)) {
		idesc_uart_unbind(uart);
		return NULL;
	}

	return &fdesc->idesc;
}

static ssize_t serial_read(struct idesc *idesc, void *buf, size_t nbyte) {
	struct uart *uart;

	assert(buf);
	assert(idesc);
	assert(idesc->idesc_ops == &idesc_serial_ops);
	assert(idesc->idesc_amode & FS_MAY_READ);

	if (!nbyte) {
		return 0;
	}

	uart = idesc_to_uart(idesc);
	assert(uart);
	assert(uart->tty);

	return tty_read(uart->tty, (char *) buf, nbyte);
}

static ssize_t serial_write(struct idesc *idesc, const void *buf, size_t nbyte) {
	int ch;
	struct uart *uart;
	size_t written, left = nbyte;

	assert(buf);
	assert(idesc);
	assert(idesc->idesc_ops == &idesc_serial_ops);
	assert(idesc->idesc_amode & FS_MAY_WRITE);

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

static void serial_close(struct idesc *idesc) {
	struct uart *uart;
	int res;

	assert(idesc);
	assert(idesc->idesc_ops == &idesc_serial_ops);

	uart = idesc_to_uart(idesc);
	assert(uart);
	res = uart_close(uart);
	assert(res == 0); /* TODO */

	idesc_uart_unbind(uart);

	file_desc_destroy((struct file_desc *)idesc);
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

static int serial_fstat(struct idesc *data, void *buff) {
	struct stat *st = buff;

	st->st_mode = S_IFCHR;

	return 0;

}

static const struct idesc_ops idesc_serial_ops = {
		.read = serial_read,
		.write = serial_write,
		.ioctl = serial_ioctl,
		.close = serial_close,
		.status = serial_status,
		.fstat = serial_fstat,
};
