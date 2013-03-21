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

#include <kernel/thread/event.h>
#include <kernel/thread/sched_lock.h>
#include <kernel/thread/sched.h>

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

//RING_BUFFER_DEF(dev_buff, int, 0x20);

//static struct event rx_happend;

static irq_return_t irq_handler(unsigned int irq_nr, void *data) {
	struct uart_device *dev = data;

	while (dev->operations->hasrx(dev)) {
		tty_rx_putc(&dev->tty, dev->operations->get(dev), 0);
	}

	return 0;
}

static void uart_tx_start(struct tty *tty, char ch) {

}

static void uart_term_setup(struct tty *tty, struct termios *termios) {

}

static struct tty_ops uart_tty_ops = {
		.setup = uart_term_setup,
		.tx_char = uart_tx_start
};

/*
 * file_operations
 */
static int dev_uart_open(struct node *node, struct file_desc *desc, int flags) {
	struct uart_device *uart_dev;
	uart_dev = uart_dev_lookup((char *)node->name);

//	if(NULL == uart_dev || uart_dev->fops) {
//		return -1;
//	}
	tty_init(&uart_dev->tty, &uart_tty_ops);

	if(uart_dev->operations->setup) {
		uart_dev->operations->setup(uart_dev, uart_dev->params);
	}

	desc->ops = &uart_dev_file_op;

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
#if 0
	size_t cnt = size;

	if (0 == ring_buff_get_cnt(&dev_buff)) {
		sched_lock();
			irq_lock();
				event_init(&rx_happend, "event_rx_happend");
			irq_unlock();

			event_wait_ms(&rx_happend, SCHED_TIMEOUT_INFINITE);
		sched_unlock();
	}

	for (; 0 < cnt; --cnt) {
		int tmp;
		char *tmp_char = buff;

		ring_buff_dequeue(&dev_buff, &tmp, 1);
		*tmp_char = (char)tmp;


		buff = (void *)(((uint32_t)buff) + 1);
	}

	return size - cnt;
#endif
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
	struct uart_device *dev = (struct uart_device *)desc->node->nas->fi;
	int speed;
	va_list args;
//	struct termios *term;

	va_start(args, request);

	switch(request) {
#if 0
	case TTY_IOCTL_GETATTR:
		term = va_arg(args, struct termios *);

		memcpy(term, &dev->tty, sizeof(struct termios));
		break;
	case TTY_IOCTL_SETATTR:
		term = va_arg(args, struct termios *);

		memcpy(&dev->tty, term, sizeof(struct termios));

		dev->operations->setup(dev, dev->params);
		break;
#endif
	case TTY_IOCTL_SETBAUD:
		speed = va_arg(args,int);
		dev->params->baud_rate = speed;
		break;
	default:
		return tty_ioctl(&dev->tty, request, NULL);
	}
	va_end(args);
	return ENOERR;
}

int uart_dev_register(struct uart_device *dev) {
	//struct node *node;
	//struct nas *nas;
	//mode_t mode;

	//TODO tmp (we can have only one device)
	uart_dev = dev;


	dev->fops = &uart_dev_file_op;
	serial_register(dev);

#if 0
	mode = S_IFCHR | S_IRALL | S_IWALL;


	/* register char device */
	node = vfs_lookup(NULL, "/dev");
	if (!node) {
		return -1;
	}

	node = vfs_create_child(node, dev->dev_name, mode);
	if (!node) {
		return -1;
	}

	nas = node->nas;
	if (NULL == (nas->fs = filesystem_alloc("empty"))) {
		return -1;
	}

	nas->fs->file_op = &uart_dev_file_op;
	nas->fi = (void *)dev;

	memset(&dev->tty, 0, sizeof(dev->tty));
	dev->tty.termios.c_cflag = CLOCAL | CREAD | CS8;
#endif

	return 0;
}
