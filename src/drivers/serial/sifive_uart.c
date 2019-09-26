/**
 * @file
 *
 * @date 09 may 2019
 * @author: Dmitry Kurbatov
 */

#include <drivers/diag.h>

#include <framework/mod/options.h>

#include <embox/unit.h>

#define UART_BASE 	OPTION_GET(NUMBER,base_addr)
#define UART_CLOCK_FREQ OPTION_GET(NUMBER,clock_freq)
#define UART_BAUD_RATE	OPTION_GET(NUMBER,baud_rate)

/* TXCTRL register */
#define UART_TXEN       1
#define UART_TXSTOP     2

/* RXCTRL register */
#define UART_RXEN       1

#define UART_REG(x) 			\
        unsigned char x;		\
        unsigned char postpad_##x[3];

#define UART_REG_INT32(x)		\
	unsigned int x;

struct com {
        UART_REG(txfifo);          /* 0 */
        UART_REG(rxfifo);          /* 1 */
        UART_REG(txctrl);          /* 2 */
        UART_REG(rxctrl);          /* 3 */
        UART_REG(ie);  	           /* 4 */
        UART_REG(ip);              /* 5 */
        UART_REG_INT32(div);       /* 6 */
};

#define COM ((volatile struct com *)UART_BASE)
#define COM_TXF (COM->txfifo)
#define COM_RXF (COM->rxfifo)
#define COM_DIV (COM->div)
#define COM_TXC (COM->txctrl)
#define COM_RXC (COM->rxctrl)
#define COM_IE (COM->ie)

static int sifive_uart_setup(const struct diag *dev) {
	COM_DIV = UART_CLOCK_FREQ / UART_BAUD_RATE - 1;
	COM_TXC = UART_TXEN;
	COM_RXC = UART_RXEN;
	COM_IE = 0;

	return 0;
}

static char sifive_uart_getc(const struct diag *dev) {
	int ch = COM_RXF;
	if (ch < 0) return -1;

	return ch;
}

static void sifive_uart_putc(const struct diag *dev, char ch) {
	while (COM_TXF < 0);
	COM_TXF = ch & 0xff;
}

DIAG_OPS_DEF(
	.init = sifive_uart_setup,
	.putc = sifive_uart_putc,
	.getc = sifive_uart_getc,
);

