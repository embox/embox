/**
 * @file
 *
 * @date Mar 11, 2023
 * @author Anton Bondarev
 */

#include <stdint.h>

#include <hal/reg.h>

#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>

#include "muart.h"

static int muart_uart_setup(struct uart *dev, const struct uart_params *params) {
	struct muart_regs *regs = (struct muart_regs *)dev->base_addr;

	REG32_STORE(&regs->bdiv, 50000000UL/params->baud_rate);

	REG32_STORE(&regs->ctrl,
			MUART_CTRL_STOP_BITS(MUART_1_STOP_BIT) |
			MUART_CTRL_WORD_LEN(MUART_WORD_LEN_8) |
			MUART_CTRL_POL_MODE |
			MUART_CTRL_APB_FIFO_MODE |
			MUART_CTRL_ENABLE);

	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		REG16_STORE(&regs->fifowm.rx_num , 0); /* 1 byte */
		REG32_STORE(&regs->mask, MUART_IRQ_RX);
	}

	return 0;
}

static int muart_uart_has_symbol(struct uart *dev) {
	struct muart_regs *regs = (struct muart_regs *)dev->base_addr;
	uint16_t rx_state;

	rx_state = REG16_LOAD(&regs->fifo_status.rx_num);

	return (int)rx_state;
}

static int muart_uart_getc(struct uart *dev) {
	struct muart_regs *regs = (struct muart_regs *)dev->base_addr;
	int ch;

	ch = (int)(REG32_LOAD(&regs->drec) & 0xFF);

	if (muart_uart_has_symbol(dev) == 0) {
		/* read status reg to switch off interrupt bit */
		REG32_LOAD(&regs->status);
	}

	return ch;
}

static int muart_uart_putc(struct uart *dev, int ch) {
	struct muart_regs *regs = (struct muart_regs *)dev->base_addr;

	while ( REG16_LOAD(&regs->fifo_status.tx_num)) {
	}

	REG32_STORE(&regs->dtrans, ch);

	return 0;
}

const struct uart_ops muart_uart_uart_ops = {
		.uart_getc = muart_uart_getc,
		.uart_putc = muart_uart_putc,
		.uart_hasrx = muart_uart_has_symbol,
		.uart_setup = muart_uart_setup,
};
