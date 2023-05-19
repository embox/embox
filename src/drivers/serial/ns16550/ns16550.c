/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    12.10.2012
 */

#include <drivers/common/memory.h>

#include <framework/mod/options.h>

#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>

#define REG_WIDTH     OPTION_GET(NUMBER,reg_width)
#define MEM32_ACCESS  OPTION_GET(NUMBER,mem32_access)

#define UART_LSR_DR     0x01            /* Data ready */
#define UART_LSR_THRE   0x20            /* Xmit holding register empty */


#if MEM32_ACCESS
#define UART_REG(x) uint32_t x
#else
#define UART_REG(x)                                                     \
        unsigned char x;                                                \
        unsigned char postpad_##x[REG_WIDTH - 1];
#endif

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

static int ns16550_setup(struct uart *dev, const struct uart_params *params) {
	volatile struct com *dev_regs = (void *)(uintptr_t)dev->base_addr;

	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		dev_regs->ier |= NS16550_IER_RX_IRQ;
	}
	return 0;
}

static int ns16550_irq_en(struct uart *dev, const struct uart_params *params) {
	volatile struct com *dev_regs = (void *)(uintptr_t)dev->base_addr;

	dev_regs->ier |= NS16550_IER_RX_IRQ;
	return 0;
}

static int ns16550_irq_dis(struct uart *dev, const struct uart_params *params) {
	volatile struct com *dev_regs = (void *)(uintptr_t)dev->base_addr;

	dev_regs->ier &= ~NS16550_IER_RX_IRQ;
	return 0;
}

static int ns16550_putc(struct uart *dev, int ch) {
	volatile struct com *dev_regs = (void *)(uintptr_t)dev->base_addr;

	while ((dev_regs->lsr & UART_LSR_THRE) == 0);

	dev_regs->rbr = ch;

	return 0;
}

static int ns16550_getc(struct uart *dev) {
	volatile struct com *dev_regs = (void *)(uintptr_t)dev->base_addr;

	return dev_regs->rbr;

}

static int ns16550_has_symbol(struct uart *dev) {
	volatile struct com *dev_regs = (void *)(uintptr_t)dev->base_addr;

	return dev_regs->lsr & UART_LSR_DR;
}

const struct uart_ops ns16550_uart_ops = {
		.uart_getc = ns16550_getc,
		.uart_putc = ns16550_putc,
		.uart_hasrx = ns16550_has_symbol,
		.uart_setup = ns16550_setup,
		.uart_irq_en = ns16550_irq_en,
		.uart_irq_dis = ns16550_irq_dis,
};
