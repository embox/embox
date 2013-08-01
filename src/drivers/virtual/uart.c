/**
 * @file
 *
 * @date Oct 25, 2012
 * @author: Anton Bondarev
 */

#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include <util/ring_buff.h>

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

static struct uart_device *uart_dev;

static struct uart_device *uart_dev_lookup(char *name) {
	return uart_dev;
}

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

static irq_return_t irq_handler(unsigned int irq_nr, void *data) {
	struct uart_device *dev = data;

	while (dev->operations->hasrx(dev)) {
		tty_rx_putc(&dev->tty, dev->operations->get(dev), 0);
	}

	return 0;
}

static void uart_out_wake(struct tty *t) {
	struct uart_device *uart_dev = member_cast_out(t, struct uart_device, tty);
	int ich;

	while ((ich = tty_out_getc(t)) != -1)
		uart_dev->operations->put(uart_dev, (char) ich);
}

static void uart_term_setup(struct tty *tty, struct termios *termios) {

}

static struct tty_ops uart_tty_ops = {
	.setup = uart_term_setup,
	.out_wake = uart_out_wake,
};

/*
 * file_operations
 */
static int dev_uart_open(struct node *node, struct file_desc *desc, int flags) {
	struct uart_device *uart_dev;
	uart_dev = uart_dev_lookup((char *)node->name);

	tty_init(&uart_dev->tty, &uart_tty_ops);

	if(uart_dev->operations->setup) {
		uart_dev->operations->setup(uart_dev, uart_dev->params);
	}

	if (desc) {
		desc->ops = &uart_dev_file_op;
	}

	irq_attach(uart_dev->irq_num, irq_handler, 0, (void *)uart_dev, uart_dev->dev_name);

	return 0;
}

static int dev_uart_close(struct file_desc *desc) {
	struct uart_device *uart_dev;

	uart_dev = uart_dev_lookup((char *)desc->node->name);

	irq_detach(uart_dev->irq_num, uart_dev);
	return 0;
}


static size_t dev_uart_read(struct file_desc *desc, void *buff, size_t size) {
	struct uart_device *dev = (struct uart_device *)desc->node->nas->fi;

	return tty_read(&dev->tty, (char *) buff, size);
}

static size_t dev_uart_write(struct file_desc *desc, void *buff, size_t size) {
	struct uart_device *uart_dev = uart_dev_lookup((char*)desc->node->name);
	size_t cnt;
	char *b;

	cnt = 0;
	b = (char*) buff;

	while (cnt != size) {
		uart_dev->operations->put(uart_dev, b[cnt++]);
	}
	return 0;
}

static int dev_uart_ioctl(struct file_desc *desc, int request, ...) {
	return ENOERR;
}

int uart_dev_register(struct uart_device *dev) {
	struct node node;

	//TODO tmp (we can have only one device)
	extern struct uart_device diag_uart;
	uart_dev = &diag_uart;
	uart_dev->operations = dev->operations;
	uart_dev->base_addr = dev->base_addr;
	uart_dev->params = dev->params;
	uart_dev->irq_num = dev->irq_num;

	uart_dev->fops = &uart_dev_file_op;
	serial_register(uart_dev);

	memcpy(node.name, dev->dev_name, 6);
	dev_uart_open(&node, NULL, 0);

	return 0;
}
