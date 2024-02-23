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
#include <termios.h>

#include <drivers/char_dev.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/tty.h>
#include <drivers/ttys.h>
#include <kernel/panic.h>
#include <mem/misc/pool.h>
#include <util/err.h>
#include <util/log.h>

static ssize_t serial_read(struct char_dev *cdev, void *buf, size_t nbyte) {
	struct uart *uart;

	assert(buf);
	assert(cdev);
	assert(cdev->ops == serial_dev_get_ops());

	if (!nbyte) {
		return 0;
	}

	uart = ((struct serial_dev *)cdev)->uart;

	assert(uart);
	assert(uart->tty);

	return tty_read(uart->tty, buf, nbyte);
}

static ssize_t serial_write(struct char_dev *cdev, const void *buf,
    size_t nbyte) {
	struct uart *uart;
	size_t written;
	size_t left;
	int ch;

	assert(buf);
	assert(cdev);
	assert(cdev->ops == serial_dev_get_ops());

	left = nbyte;
	uart = ((struct serial_dev *)cdev)->uart;

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

static int serial_ioctl(struct char_dev *cdev, int request, void *data) {
	struct uart *uart;

	assert(cdev);
	assert(cdev->ops == serial_dev_get_ops());

	uart = ((struct serial_dev *)cdev)->uart;

	assert(uart);

	return tty_ioctl(uart->tty, request, data);
}

static int serial_status(struct char_dev *cdev, int mask) {
	struct uart *uart;
	int res = 0;

	assert(cdev);
	assert(cdev->ops == serial_dev_get_ops());

	if (!mask) {
		return 0;
	}

	uart = ((struct serial_dev *)cdev)->uart;

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

static void serial_close(struct char_dev *cdev) {
	struct uart *uart;
	int err;

	assert(cdev);
	assert(cdev->ops == serial_dev_get_ops());

	uart = ((struct serial_dev *)cdev)->uart;

	assert(uart);

	uart->tty = NULL;

	if ((err = uart_close(uart))) {
		log_crit("failed to close UART");
	}
}

static int serial_open(struct char_dev *cdev, struct idesc *idesc) {
	struct serial_dev *tty_dev;
	struct uart *uart;

	assert(cdev);
	assert(cdev->ops == serial_dev_get_ops());

	tty_dev = ((struct serial_dev *)cdev);
	uart = tty_dev->uart;

	assert(uart);

	tty_init(&tty_dev->tty, uart_tty_get_ops());

	uart->tty = &tty_dev->tty;
	uart->tty->idesc = idesc;

	if (uart->irq_handler == NULL) {
		uart->irq_handler = uart_irq_handler;
	}

	return uart_open(uart);
}

const struct char_dev_ops __serial_cdev_ops = {
    .read = serial_read,
    .write = serial_write,
    .ioctl = serial_ioctl,
    .status = serial_status,
    .close = serial_close,
    .open = serial_open,
};
