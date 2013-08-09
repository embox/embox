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


/* `irq' field has special meaning. If zero, irq is disabled. If not,
 * irq handler assigned to specified irq */
struct uart_params {
	uint32_t baud_rate;
	bool parity;
	int n_stop;
	int n_bits;
	int irq;
};

struct uart;

struct file_operations;

struct uart_desc {
	const char *dev_name;
	int irq_num;
	uint32_t base_addr;
	int (*uart_getc)(const struct uart_desc *dev);
	int (*uart_putc)(const struct uart_desc *dev, int symbol);
	int (*uart_hasrx)(const struct uart_desc *dev);
	int (*uart_setup)(const struct uart_desc *dev, struct uart_params *params);
};

struct uart {
	struct dlist_head lnk;
	const struct uart_desc *uart_desc;
	struct uart_params params;
	struct tty tty;
};

/**
 * @brief Register uart in kernel
 *
 * @param uart
 *
 * @return
 */
extern struct uart *uart_register(const struct uart_desc *uart);
/**
 * @brief Deregister in kernel
 *
 * @param uart
 */
extern void uart_deregister(struct uart *uart);

/**
 * @brief Look uart by uart_desc's name. User names (i.e. ttyS0) not matched.
 *
 * @param name
 *
 * @return
 */
extern struct uart *uart_dev_lookup(const char *name);

/**
 * @brief Get uart parameters
 *
 * @param uart
 * @param params Pointer to store parameters
 *
 * @return
 */
extern int uart_get_params(struct uart *uart, struct uart_params *params);

/**
 * @brief Set uart parameters
 *
 * @param uart
 * @param params
 *
 * @return
 */
extern int uart_set_params(struct uart *uart, const struct uart_params *params);

static inline int uart_putc(struct uart *uart, int ch) {
	return uart->uart_desc->uart_putc(uart->uart_desc, ch);
}

static inline int uart_getc(struct uart *uart) {
	return uart->uart_desc->uart_getc(uart->uart_desc);
}

static inline int uart_hasrx(struct uart *uart) {
	return uart->uart_desc->uart_hasrx(uart->uart_desc);
}

#endif /* UART_DEVICE_H_ */
