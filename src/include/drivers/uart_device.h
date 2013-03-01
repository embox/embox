/*
 * @file
 *
 * @date Oct 26, 2012
 * @author: Anton Bondarev
 */

#ifndef UART_DEVICE_H_
#define UART_DEVICE_H_

#include <stdint.h>
#include <stdbool.h>
#include <drivers/tty.h>

struct uart_params {
	uint32_t baud_rate;
	bool parity;
	int n_stop;
	int n_bits;
};

struct uart_device;

typedef int (*uart_getchar_ft)(struct uart_device *dev);
typedef int (*uart_putchar_ft)(struct uart_device *dev, int symbol);
typedef int (*uart_hasrx_ft)(struct uart_device *dev);
typedef int (*uart_setup_ft)(struct uart_device *dev, struct uart_params *params);

struct uart_ops {
	uart_getchar_ft get;
	uart_putchar_ft put;
	uart_hasrx_ft hasrx;
	uart_setup_ft setup;
};

struct file_operations;

struct uart_device {
	const char *dev_name;
	int irq_num;
	uint32_t base_addr;
	struct kfile_operations *fops;
	struct uart_params *params;
	struct uart_ops *operations;
	struct tty tty;
};

extern int uart_dev_register(struct uart_device *dev);

extern struct kfile_operations uart_dev_file_op;

#endif /* UART_DEVICE_H_ */
