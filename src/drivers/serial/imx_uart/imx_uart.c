/**
 * @file
 *
 * @data 01.04.2016
 * @author: Anton Bondarev
 */

#include <drivers/diag.h>

#define UART(x) (*(volatile unsigned long *)(0x02020000 + (x)))

#define USR2 0x98
#define USR2_TXFE (1<<14)
#define TXR  0x40
#define UCR1 0x80
#define UCR1_UARTEN 1

static int imxuart_diag_init(const struct diag *diag) {
	return 0;
}

static int imxuart_diag_hasrx(const struct diag *diag) {
	return 0;
}

static char imxuart_diag_getc(const struct diag *diag) {
	return 0;
}

static void imxuart_diag_putc(const struct diag *diag, char ch) {
	while (!(UART(USR2) & USR2_TXFE)) {
	}
	UART(TXR) = ch;
}

DIAG_OPS_DECLARE(
		.init = imxuart_diag_init,
		.putc = imxuart_diag_putc,
		.getc = imxuart_diag_getc,
		.kbhit = imxuart_diag_hasrx,
);
