/**
 * @file
 *
 * @date Dec 18, 2013
 * @author: Anton Bondarev
 */
#include <assert.h>

#include <err.h>
#include <poll.h>

#include <mem/misc/pool.h>
#include <fs/idesc.h>

#include <fs/idesc_serial.h>

#include <framework/mod/options.h>


#define MAX_SERIALS \
	OPTION_GET(NUMBER, serial_quantity)

//POOL_DEF(pool_serials, struct idesc_serial, MAX_SERIALS);

#define idesc_to_uart(desc) \
	(((struct file_desc *)desc)->file_info)

static const struct idesc_ops idesc_serial_ops;

struct idesc *idesc_serial_create(struct file_desc *fdesc, struct uart *uart,
		idesc_access_mode_t mod) {
	//struct idesc_serial *idesc;

	assert(uart);
	assert(mod);

	//idesc = pool_alloc(&pool_serials);
//	if (idesc) {
//		err_ptr(-ENOMEM);
//	}
	//idesc_init(&idesc->idesc, &idesc_serial_ops, mod);
	//idesc->uart = uart;

	//return &idesc->idesc;
	fdesc->idesc.idesc_ops = &idesc_serial_ops;
	return &fdesc->idesc;
}

static int serial_read(struct idesc *idesc, void *buf, size_t nbyte) {
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
	assert(&uart->tty);

	return tty_read(&uart->tty, (char *) buf, nbyte);
}

static int serial_write(struct idesc *idesc, const void *buf, size_t nbyte) {
	int ch;
	struct uart *uart;
	size_t written, left = nbyte;

	assert(buf);
	assert(idesc);
	assert(idesc->idesc_ops == &idesc_serial_ops);
	assert(idesc->idesc_amode & FS_MAY_WRITE);

	uart = idesc_to_uart(idesc);
	assert(uart);
	assert(&uart->tty);

	do {
		written = tty_write(&uart->tty, buf, left);

		while (-1 != (ch = tty_out_getc(&uart->tty))) {
			uart_putc(uart, ch);
		}

		left -= written;
		buf = (void *)((char *)buf + written);
	} while (left != 0);

	return nbyte;
}

static int serial_close(struct idesc *idesc) {
	struct uart *uart;
	int res;

	assert(idesc);
	assert(idesc->idesc_ops == &idesc_serial_ops);

	uart = idesc_to_uart(idesc);
	assert(uart);
	res = uart_close(uart);

	//pool_free(&pool_serials, idesc);
	file_desc_destroy((struct file_desc *)idesc);

	return res;
}

static int serial_ioctl(struct idesc *idesc, int request, void *data) {
	struct uart *uart;

	assert(idesc);
	assert(idesc->idesc_ops == &idesc_serial_ops);

	uart = idesc_to_uart(idesc);
	assert(uart);

	return tty_ioctl(&uart->tty, request, data);
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
		res += tty_status(&uart->tty, POLLIN);
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

static const struct idesc_ops idesc_serial_ops = {
		.read = serial_read,
		.write = serial_write,
		.ioctl = serial_ioctl,
		.close = serial_close,
		.status = serial_status
};
