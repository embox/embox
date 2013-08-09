/**
 * @file
 *
 * @date Oct 25, 2012
 * @author: Anton Bondarev
 */


#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include <util/indexator.h>

#include <fs/file_desc.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/file_operation.h>

#include <drivers/uart_device.h>
#include <drivers/tty.h>

#include <kernel/irq.h>

#include <kernel/event.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/sched.h>

#include <mem/misc/pool.h>

#include <embox/device.h>

POOL_DEF(uart_pool, struct uart, 4);
INDEX_DEF(serial_indexator, 0, 32);
static DLIST_DEFINE(uart_list);

static int serial_register(struct uart *dev);
static irq_return_t irq_handler(unsigned int irq_nr, void *data);

static int dev_uart_open(struct node *node, struct file_desc *file_desc,
	int flags);
static int dev_uart_close(struct file_desc *desc);
static size_t dev_uart_read(struct file_desc *desc, void *buf, size_t size);
static size_t dev_uart_write(struct file_desc *desc, void *buf, size_t size);
static int dev_uart_ioctl(struct file_desc *desc, int request, ...);

struct kfile_operations uart_dev_file_op = {
	.open = dev_uart_open,
	.close = dev_uart_close,
	.read = dev_uart_read,
	.write = dev_uart_write,
	.ioctl = dev_uart_ioctl
};

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

static int serial_register(struct uart *dev) {
	int idx;
	char dev_name[] = "ttyS00";

	idx = index_alloc(&serial_indexator, INDEX_ALLOC_MIN);
	if(idx < 0) {
		return -EBUSY;
	}
	sprintf(dev_name, "ttyS%d", idx);

	char_dev_register(dev->uart_desc->dev_name, &uart_dev_file_op);

	return ENOERR;
}

struct uart *uart_register(const struct uart_desc *uartd) {
	struct uart *uart = NULL;

	if (!(uart = uart_alloc(uartd))) {
		return NULL;
	}

	uart->uart_desc = uartd;
	dlist_add_next(&uart->lnk, &uart_list);

	serial_register(uart);

	return uart;
}

void uart_deregister(struct uart *uart) {
	dlist_del(&uart->lnk);
	uart_free(uart);
}

struct uart *uart_dev_lookup(const char *name) {
	struct uart *uart, *nxt;

	dlist_foreach_entry(uart, nxt, &uart_list, lnk) {
		if (!name || !strcmp(name, uart->uart_desc->dev_name)) {
			return uart;
		}
	}

	return NULL;
}

int uart_set_params(struct uart *uart, const struct uart_params *params) {

	if (uart->params.irq) {
		irq_detach(uart->params.irq, uart);
	}

	memcpy(&uart->params, params, sizeof(struct uart_params));

	if(uart->uart_desc->uart_setup) {
		uart->uart_desc->uart_setup(uart->uart_desc, &uart->params);
	}

	if (params->irq) {
		irq_attach(params->irq, irq_handler, 0, uart, uart->uart_desc->dev_name);
	}

	return 0;
}

int uart_get_params(struct uart *uart, struct uart_params *params) {
	memcpy(params, &uart->params, sizeof(struct uart_params));

	return 0;
}

static irq_return_t irq_handler(unsigned int irq_nr, void *data) {
	struct uart *dev = data;

	while (uart_hasrx(dev)) {
		tty_rx_putc(&dev->tty, uart_getc(dev), 0);
	}

	return 0;
}

static void uart_out_wake(struct tty *t) {
	struct uart *uart_dev = member_cast_out(t, struct uart, tty);
	int ich;

	while ((ich = tty_out_getc(t)) != -1)
		uart_putc(uart_dev, (char) ich);
}

static void uart_term_setup(struct tty *tty, struct termios *termios) {
	struct uart *uart = member_cast_out(t, struct uart, tty);

	struct uart_params uparams;

	uart_get_params(uart, &uparams);

	/* TODO baud rate is ospeed. What with ispeed ? */
	uparams.baud_rate = termios->c_ospeed;

	uart_set_params(uart, &uparams);

}

static struct tty_ops uart_tty_ops = {
	.setup = uart_term_setup,
	.out_wake = uart_out_wake,
};

/*
 * file_operations
 */
static int dev_uart_open(struct node *node, struct file_desc *desc, int flags) {
	struct uart *uart_dev = uart_dev_lookup(node->name);

	if (!uart_dev) {
		return -ENOENT;
	}

	tty_init(&uart_dev->tty, &uart_tty_ops);

	assert(desc);
	desc->ops = &uart_dev_file_op;
	desc->file_info = uart_dev;

	return 0;
}

static int dev_uart_close(struct file_desc *desc) {
	struct uart *uart_dev = desc->file_info;
	const struct uart_desc *uartd = uart_dev->uart_desc;

	irq_detach(uartd->irq_num, uart_dev);

	return 0;
}


static size_t dev_uart_read(struct file_desc *desc, void *buff, size_t size) {
	struct uart *uart_dev = desc->file_info;

	return tty_read(&uart_dev->tty, (char *) buff, size);
}

static size_t dev_uart_write(struct file_desc *desc, void *buff, size_t size) {
	struct uart *uart_dev = desc->file_info;
	size_t cnt;
	char *b;

	cnt = 0;
	b = (char*) buff;

	while (cnt != size) {
		uart_putc(uart_dev, b[cnt++]);
	}

	return size;
}

static int dev_uart_ioctl(struct file_desc *desc, int request, ...) {
	va_list va;
	void *data;
	struct uart *uart_dev = desc->file_info;

	va_start(va, request);
	data = va_arg(va, void *);
	va_end(va);

	return tty_ioctl(&uart_dev->tty, request, data);
}

