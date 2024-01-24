/*
 * @file
 *
 * @date Oct 26, 2012
 * @author: Anton Bondarev
 */

#ifndef UART_DEVICE_H_
#define UART_DEVICE_H_

#include <stdint.h>
#include <lib/libds/dlist.h>
#include <lib/libds/ring_buff.h>
#include <kernel/irq.h>

#define UART_NAME_MAXLEN  8
#define UART_RX_BUFF_SZ   8

#define UART_STATE_OPEN      (1 << 0)
/*
#define UART_CTS_ENABLE  (1 << 1))
#define UART_DCD_ENABLE  (1 << 2))
#define UART_AUTORTS     (1 << 3))
#define UART_AUTOCTS     (1 << 4))
#define UART_AUTOXOFF    (1 << 5))
#define UART_SYNC_FIFO   (1 << 6))
*/
#define UART_STATE_RX_ACTIVE  (1 << 8)
#define UART_STATE_INITED     (1 << 10)

struct uart;
struct uart_desc;
struct tty;

struct uart_params {
	uint32_t baud_rate;
	uint32_t uart_param_flags;
};


#define UART_PARAM_FLAGS_PARITY(parity)    ((parity & 0x3) << 0)
#define UART_PARAM_FLAGS_PARITY_MASK(fl)   (fl & (0x3 << 0))
#define UART_PARAM_FLAGS_PARITY_NONE          UART_PARAM_FLAGS_PARITY(0x0)
#define UART_PARAM_FLAGS_PARITY_ODD           UART_PARAM_FLAGS_PARITY(0x1)
#define UART_PARAM_FLAGS_PARITY_EVEN          UART_PARAM_FLAGS_PARITY(0x2)

#define UART_PARAM_FLAGS_HWCRTL(type)      ((type & 0x3) << 2)
#define UART_PARAM_FLAGS_HWCTRL_MASK(fl)   (fl & (0x3 << 2))
#define UART_PARAM_FLAGS_HWCRTL_NONE          UART_PARAM_FLAGS_HWCRTL(0x0)
#define UART_PARAM_FLAGS_HWCRTL_CTS           UART_PARAM_FLAGS_HWCRTL(0x1)
#define UART_PARAM_FLAGS_HWCRTL_RTS           UART_PARAM_FLAGS_HWCRTL(0x2)
#define UART_PARAM_FLAGS_HWCRTL_BOTH          UART_PARAM_FLAGS_HWCRTL(0x3)

#define UART_PARAM_FLAGS_USE_IRQ           (0x1 << 4)

#define UART_PARAM_FLAGS_STOPS(len)        (((len) & 0x3) << 5)
#define UART_PARAM_FLAGS_STOPS_MASK(fl)    (fl & (0x3 << 5))
#define UART_PARAM_FLAGS_1_STOP               UART_PARAM_FLAGS_STOPS(0x0)
#define UART_PARAM_FLAGS_2_STOP               UART_PARAM_FLAGS_STOPS(0x1)
#define UART_PARAM_FLAGS_0_5_STOP             UART_PARAM_FLAGS_STOPS(0x2)
#define UART_PARAM_FLAGS_1_5_STOP             UART_PARAM_FLAGS_STOPS(0x3)

#define UART_PARAM_FLAGS_BIT_WORD(len)     (((len - 4) & 0x7) << 8)
#define UART_PARAM_FLAGS_BIT_WORD_MASK(fl) (fl & (0x7 << 8))
#define UART_PARAM_FLAGS_5BIT_WORD            UART_PARAM_FLAGS_BIT_WORD(5)
#define UART_PARAM_FLAGS_6BIT_WORD            UART_PARAM_FLAGS_BIT_WORD(6)
#define UART_PARAM_FLAGS_7BIT_WORD            UART_PARAM_FLAGS_BIT_WORD(7)
#define UART_PARAM_FLAGS_8BIT_WORD            UART_PARAM_FLAGS_BIT_WORD(8)
#define UART_PARAM_FLAGS_9BIT_WORD            UART_PARAM_FLAGS_BIT_WORD(9)
#define UART_PARAM_FLAGS_10BIT_WORD           UART_PARAM_FLAGS_BIT_WORD(10)

#define UART_PARAM_FLAGS_DEV_TYPE(type)    ((type & 0xF) << 16)
#define UART_PARAM_FLAGS_DEV_TYPE_MASK(fl) (fl & (0xF << 16))
#define UART_PARAM_FLAGS_DEV_TYPE_UART        UART_PARAM_FLAGS_DEV_TYPE(0)
#define UART_PARAM_FLAGS_DEV_TYPE_RS485       UART_PARAM_FLAGS_DEV_TYPE(1)
#define UART_PARAM_FLAGS_DEV_TYPE_ISO7816     UART_PARAM_FLAGS_DEV_TYPE(2)
#define UART_PARAM_FLAGS_DEV_TYPE_IR          UART_PARAM_FLAGS_DEV_TYPE(3)


struct uart_ops {
	int (*uart_getc)(struct uart *dev);
	int (*uart_putc)(struct uart *dev, int symbol);
	int (*uart_hasrx)(struct uart *dev);
	int (*uart_setup)(struct uart *dev, const struct uart_params *params);
	int (*uart_irq_en)(struct uart *dev, const struct uart_params *params);
	int (*uart_irq_dis)(struct uart *dev, const struct uart_params *params);
};

struct uart {
	/* declarative */
	const struct uart_ops *uart_ops;
	short irq_num;
	uintptr_t base_addr;
	irq_handler_t irq_handler;

	/* management */
	struct dlist_head uart_lnk;
	char dev_name[UART_NAME_MAXLEN];
	unsigned char idx;

	/* runtime */
	int state;
	struct uart_params params;
	struct tty *tty;
	int uart_rx_buff[UART_RX_BUFF_SZ];
	struct ring_buff uart_rx_ring;
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
extern int uart_register(struct uart *uartd,
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
	return uart->uart_ops->uart_putc(uart, ch);
}

static inline int uart_getc(struct uart *uart) {
	return uart->uart_ops->uart_getc(uart);
}

static inline int uart_hasrx(struct uart *uart) {
	return uart->uart_ops->uart_hasrx(uart);
}


static inline int uart_state_test(struct uart *uart, int mask) {
	return uart->state & mask;
}

static inline void uart_state_set(struct uart *uart, int mask) {
	uart->state |= mask;
}

static inline void uart_state_clear(struct uart *uart, int mask) {
	uart->state &= ~mask;
}

extern struct dlist_head *uart_get_list(void);

#define uart_opened_foreach(uart_dev) \
	dlist_foreach_entry(uart_dev, uart_get_list(), uart_lnk)

#endif /* UART_DEVICE_H_ */
