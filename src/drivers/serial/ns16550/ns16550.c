/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    12.10.2012
 */

#include <hal/reg.h>
#include <drivers/common/memory.h>
#include <drivers/diag.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/mmu.h>
#include <mem/vmem.h>
#include <util/binalign.h>
#include <drivers/serial/uart_device.h>
#include <drivers/serial/diag_serial.h>

#define COM0_IRQ_NUM OPTION_GET(NUMBER,irq_num)

#define UART_LSR_DR     0x01            /* Data ready */
#define UART_LSR_THRE   0x20            /* Xmit holding register empty */
#define COM_BASE (OPTION_GET(NUMBER, base_addr))

#define UART_REG(x)                                                     \
        unsigned char x;                                                \
        unsigned char postpad_##x[3];

struct com {
        UART_REG(rbr);          /* 0 */
        UART_REG(ier);          /* 1 */
#define NS16550_IER_RX_IRQ	(1 << 0)
        UART_REG(fcr);          /* 2 */
        UART_REG(lcr);          /* 3 */
        UART_REG(mcr);          /* 4 */
        UART_REG(lsr);          /* 5 */
        UART_REG(msr);          /* 6 */
        UART_REG(spr);          /* 7 */
        UART_REG(mdr1);         /* 8 */
        UART_REG(reg9);         /* 9 */
        UART_REG(regA);         /* A */
        UART_REG(regB);         /* B */
        UART_REG(regC);         /* C */
        UART_REG(regD);         /* D */
        UART_REG(regE);         /* E */
        UART_REG(uasr);         /* F */
        UART_REG(scr);          /* 10*/
        UART_REG(ssr);          /* 11*/
        UART_REG(reg12);        /* 12*/
        UART_REG(osc_12m_sel);  /* 13*/
};

#define COM3 ((volatile struct com *)COM_BASE)
#define COM3_RBR (COM3->rbr)
#define COM3_LSR (COM3->lsr)

EMBOX_UNIT_INIT(ns16550_init);

static int ns16550_setup(struct uart *dev, const struct uart_params *params) {
	COM3->ier |= NS16550_IER_RX_IRQ;
	return 0;
}

static int ns16550_putc(struct uart *dev, int ch) {
	while ((COM3_LSR & UART_LSR_THRE) == 0);

	COM3_RBR = ch;

	return 0;
}

static int ns16550_getc(struct uart *dev) {
	return COM3_RBR;

}

static int ns16550_has_symbol(struct uart *dev) {
	return COM3_LSR & UART_LSR_DR;
}


static const struct uart_ops i8250_uart_ops = {
		.uart_getc = ns16550_getc,
		.uart_putc = ns16550_putc,
		.uart_hasrx = ns16550_has_symbol,
		.uart_setup = ns16550_setup,
};

static struct uart uart0 = {
		.uart_ops = &i8250_uart_ops,
		.irq_num = COM0_IRQ_NUM,
		.base_addr = COM_BASE,
};

static const struct uart_params uart_defparams = {
		.baud_rate = OPTION_GET(NUMBER,baud_rate),
		.parity = 0,
		.n_stop = 1,
		.n_bits = 8,
		.irq = true,
};

static const struct uart_params uart_diag_params = {
		.baud_rate = OPTION_GET(NUMBER,baud_rate),
		.parity = 0,
		.n_stop = 1,
		.n_bits = 8,
		.irq = false,
};

DIAG_SERIAL_DEF(&uart0, &uart_diag_params);

static int ns16550_init(void) {
	return uart_register(&uart0, &uart_defparams);
}

PERIPH_MEMORY_DEFINE(ns16550, COM_BASE, 0x1000);
