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

#define UART_NAME_MAXLEN 8

#define UART_STATE_OPEN (1 << 0)

struct uart;
struct uart_desc;

struct uart_params {
	uint32_t baud_rate;
	bool parity;
	int n_stop;
	int n_bits;
	bool irq;
};

struct uart_ops {
	int (*uart_getc)(const struct uart_desc *dev);
	int (*uart_putc)(const struct uart_desc *dev, int symbol);
	int (*uart_hasrx)(const struct uart_desc *dev);
	int (*uart_setup)(const struct uart_desc *dev,
			const struct uart_params *params);
};

struct uart_desc {
	const struct uart_ops *uart_ops;
	short irq_num;
	uint32_t base_addr;
};

struct uart {
	struct dlist_head lnk;
	char dev_name[UART_NAME_MAXLEN];
	int idx;
	const struct uart_desc *uart_desc;
	int state;
	struct uart_params params;
	struct tty tty;
};

/**
 * @brief Register uart in kernel
 *
 * @param uart Uart descriptor
 * @param uart_params Pointer to uart params. If not null, used to assign
 * 	default params
 *
 * @return uart instance pointer on success
 * @return NULL on error
 */
extern struct uart *uart_register(const struct uart_desc *uartd,
		const struct uart_params *uart_defparams);

/**
 * @brief Deregister in kernel
 *
 * @param uart
 */
extern void uart_deregister(struct uart *uart);

/**
 * @brief Look uart by name
 *
 * @param name
 *
 * @return uart instance pointer if found
 * @return NULL if not found
 */
extern struct uart *uart_dev_lookup(const char *name);

/**
 * @brief Initialize UART: init tty and assing irq
 *
 * @param uart UART to initilize
 *
 * @return 0 on succes
 * @return error code otherwise
 */
extern int uart_open(struct uart *uart);

/**
 * @brief Deinitialize UART
 *
 * @param uart
 *
 * @return
 */
extern int uart_close(struct uart *uart);

/**
 * @brief Get uart parameters
 *
 * @param uart
 * @param params Pointer to store parameters
 *
 * @return 0 on succes
 */
extern int uart_get_params(struct uart *uart, struct uart_params *params);

/**
 * @brief Set parameters of opened UART. This will perform additional actions
 * to maintain irq parameter and other options.
 *
 * @param uart
 * @param params
 *
 * @return 0 on succes
 */
extern int uart_set_params(struct uart *uart, const struct uart_params *params);

static inline int uart_putc(struct uart *uart, int ch) {
	return uart->uart_desc->uart_ops->uart_putc(uart->uart_desc, ch);
}

static inline int uart_getc(struct uart *uart) {
	return uart->uart_desc->uart_ops->uart_getc(uart->uart_desc);
}

static inline int uart_hasrx(struct uart *uart) {
	return uart->uart_desc->uart_ops->uart_hasrx(uart->uart_desc);
}

#endif /* UART_DEVICE_H_ */
