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
#include <fs/dvfs.h>
#include <util/dlist.h>
#include <kernel/sched/schedee_priority.h>
#include <kernel/lthread/lthread.h>
#include <embox/unit.h>
#include <hal/ipl.h>

#define MAX_SERIALS \
	OPTION_GET(NUMBER, serial_quantity)


#define idesc_to_uart(desc) \
	(((struct  tty_uart*)desc)->uart)

#define UART_DATA_BUFF_SZ 8
#define UART_RX_HND_PRIORITY 128

EMBOX_UNIT_INIT(serial_common_init);

static const struct idesc_ops idesc_serial_ops;

struct tty_uart {
	struct idesc idesc;
	struct tty tty;
	struct uart *uart;
};

struct uart_rx {
	struct uart *uart;
	int data;
	struct dlist_head lnk;
};

POOL_DEF(uart_ttys, struct tty_uart, MAX_SERIALS);
POOL_DEF(uart_rx_buff, struct uart_rx, UART_DATA_BUFF_SZ);

static DLIST_DEFINE(uart_rx_list);

static struct lthread uart_rx_irq_handler;

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

static int uart_rx_buff_put(struct uart *dev, int c) {
	struct uart_rx *rx;

	irq_lock();
	{
		rx = pool_alloc(&uart_rx_buff);
		if (!rx) {
			irq_unlock();
			return -1;
		}

		rx->uart = dev;
		rx->data = c;
		dlist_add_prev(dlist_head_init(&rx->lnk), &uart_rx_list);
	}
	irq_unlock();

	return 0;
}

static int uart_rx_buff_get(struct uart_rx *rx_data) {
	struct uart_rx *rx;

	if (dlist_empty(&uart_rx_list)) {
		return -1;
	}

	irq_lock();
	{
		rx = dlist_next_entry_or_null(&uart_rx_list, struct uart_rx, lnk);
		if (!rx) {
			irq_unlock();
			return -1;
		}

		*rx_data = *rx;
		dlist_del(&rx->lnk);
		pool_free(&uart_rx_buff, rx);
	}
	irq_unlock();

	return 0;
}

static int uart_rx_action(struct lthread *self) {
	struct uart_rx rx;

	while (!uart_rx_buff_get(&rx)) {
		tty_rx_locked(rx.uart->tty, rx.data, 0);
	}

	return 0;
}

static irq_return_t uart_irq_handler(unsigned int irq_nr, void *data) {
	struct uart *dev = data;

	if (dev->tty) {
		while (uart_hasrx(dev)) {
			uart_rx_buff_put(dev, uart_getc(dev));
		}
		lthread_launch(&uart_rx_irq_handler);
	}

	return IRQ_HANDLED;
}

struct idesc *idesc_serial_create(struct uart *uart,
		idesc_access_mode_t mod) {
	struct tty_uart *tu;

	assert(uart);

	tu = pool_alloc(&uart_ttys);
	if (!tu) {
		return err_ptr(ENOMEM);
	}

	tty_init(&tu->tty, &uart_tty_ops);

	tu->uart = uart;
	uart->tty = &tu->tty;
	uart->tty->idesc = &tu->idesc;
	uart->irq_handler = uart_irq_handler;

	idesc_init(&tu->idesc, &idesc_serial_ops, FS_MAY_READ | FS_MAY_WRITE);

	return &tu->idesc;
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

	pool_free(&uart_ttys, idesc);
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

static int serial_common_init(void) {
	lthread_init(&uart_rx_irq_handler, &uart_rx_action);
	schedee_priority_set(&uart_rx_irq_handler.schedee, UART_RX_HND_PRIORITY);
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
