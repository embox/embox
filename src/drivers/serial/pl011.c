/**
 * @file
 *
 * @data 04 aug 2015
 * @author: Anton Bondarev
 */
#include <stdint.h>

#include <hal/reg.h>
#include <drivers/diag.h>
#include <framework/mod/options.h>

#define UART_BASE OPTION_GET(NUMBER,base_addr)

/* UART Registers */
#define UART_DR		(UART_BASE + 0x00)
#define UART_RSR	(UART_BASE + 0x04)
#define UART_ECR	(UART_BASE + 0x04)
#define UART_FR		(UART_BASE + 0x18)
#define UART_IBRD	(UART_BASE + 0x24)
#define UART_FBRD	(UART_BASE + 0x28)
#define UART_LCRH	(UART_BASE + 0x2c)
#define UART_CR		(UART_BASE + 0x30)
#define UART_IMSC	(UART_BASE + 0x38)
#define UART_MIS	(UART_BASE + 0x40)
#define UART_ICR	(UART_BASE + 0x44)

/* Flag register */
#define FR_RXFE		0x10	/* Receive FIFO empty */
#define FR_TXFF		0x20	/* Transmit FIFO full */

static void pl011_diag_putc(const struct diag *diag, char ch) {
	while (REG_LOAD(UART_FR) & FR_TXFF) ;

	REG_STORE(UART_DR, (uint32_t)ch);
}

static char pl011_diag_getc(const struct diag *diag) {
	return REG_LOAD(UART_DR);
}

static int pl011_diag_kbhit(const struct diag *diag) {
	return !(REG_LOAD(UART_FR) & FR_RXFE);
}

DIAG_OPS_DECLARE(
		.putc = pl011_diag_putc,
		.getc = pl011_diag_getc,
		.kbhit = pl011_diag_kbhit,
);
