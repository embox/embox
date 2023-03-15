/**
 * @file
 *
 * @date Mar 11, 2023
 * @author Anton Bondarev
 */

#include <hal/reg.h>

#include <drivers/diag.h>

#include <drivers/common/memory.h>

#include <framework/mod/options.h>

#include "muart.h"

#define BASE_ADDR          OPTION_GET(NUMBER, base_addr)
#define UART_DIAG          ((struct muart_regs *)BASE_ADDR)
#define BAUD_RATE          OPTION_GET(NUMBER, baud_rate)


static void muart_diag_putc(const struct diag *diag, char ch) {
	while ( REG16_LOAD(&UART_DIAG->fifo_status.tx_num)) {
		;
	}

	REG32_STORE(&UART_DIAG->dtrans, ch);
}

static int muart_diag_kbhit(const struct diag *diag) {
	return REG16_LOAD(&UART_DIAG->fifo_status.rx_num);
}

static char muart_diag_getc(const struct diag *diag) {

	return (char)(REG32_LOAD(&UART_DIAG->drec) & 0xFF);
}

static int muart_diag_init(const struct diag *diag) {
	REG32_STORE(&UART_DIAG->bdiv, 50000000UL/BAUD_RATE);

	REG32_STORE(&UART_DIAG->ctrl,
			MUART_CTRL_STOP_BITS(MUART_1_STOP_BIT) |
			MUART_CTRL_WORD_LEN(MUART_WORD_LEN_8) |
			MUART_CTRL_POL_MODE |
			MUART_CTRL_APB_FIFO_MODE |
			MUART_CTRL_ENABLE);

	return 0;
}

DIAG_OPS_DEF(
		.init = muart_diag_init,
		.putc = muart_diag_putc,
		.getc = muart_diag_getc,
		.kbhit = muart_diag_kbhit,
);

PERIPH_MEMORY_DEFINE(muart_uart, BASE_ADDR, 0x1000);
