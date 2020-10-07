/**
 * @file
 * @brief Raspberry Pi 16550-like Mini UART driver
 * Mini UART is the default console device on Raspberry Pi models with
 * Bluetooth and WiFi. But some registers are different. For example,
 * we need enable manually the mini UART by writting into base_address
 * + 0x04 Auxiliary Enable Register.
 * There are Extra Control Register and Extra Status Register as well,
 * in purpose to provide a more intelligent way to manage RX/TX. But
 * up to now, the driver adopts a full 16550-like solution.
 * There is a Baudrate Register as a shortcut to get and set Baudrate.
 *
 * @author  Weixuan XIAO
 * @date    09.11.2020
 */

#include <hal/reg.h>
#include <drivers/common/memory.h>
#include <drivers/diag.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/mmu.h>
#include <mem/vmem.h>
#include <util/binalign.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>
#include <kernel/printk.h>

#define REG_WIDTH    OPTION_GET(NUMBER,reg_width)
#define COM0_IRQ_NUM OPTION_GET(NUMBER, irq_num)

#define AUX_ENABLE ((volatile unsigned int*)(OPTION_GET(NUMBER, base_addr) + 0x04))
#define AUX_MU_IO_OFFSET 0x40
#define COM_BASE (OPTION_GET(NUMBER, base_addr) + AUX_MU_IO_OFFSET)

#define MINI_UART_IER_RX_IRQ 0x01

#define UART_REG(x)													\
        unsigned char x;											\
        unsigned char postpad_##x[REG_WIDTH - 1];

struct com {
        UART_REG(io);          	/* Mini Uart I/O Data */
        UART_REG(ier);          /* Mini Uart Interrupt Enable */
        UART_REG(iir);          /* Mini Uart Interrupt Identify */
        UART_REG(lcr);          /* Mini Uart Line Control */
        UART_REG(mcr);          /* Mini Uart Modem Control */
        UART_REG(lsr);          /* Mini Uart Line Status */
        UART_REG(msr);          /* Mini Uart Modem Status */
        UART_REG(scr);          /* Mini Uart Scratch */
        UART_REG(cntl);         /* Mini Uart Extra Control */
        UART_REG(stat);         /* Mini Uart Extra Status */
        UART_REG(baud);         /* Mini Uart Baudrate */
};

#define COM3 ((volatile struct com *)COM_BASE)
#define COM3_LSR (COM3->lsr)
#define COM3_IER (COM3->ier)
#define COM3_IO (COM3->io)
#define COM3_CNTL (COM3->cntl)

EMBOX_UNIT_INIT(rpi_mini_uart_init);

static int rpi_mini_uart_setup(struct uart *dev, const struct uart_params *params) {
	*AUX_ENABLE |= 0x01;	/* enable mini uart */
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		COM3_IER |= MINI_UART_IER_RX_IRQ;
	}
	COM3_CNTL = 0x03;		/* enable tx and rx */
	return 0;
}

static int rpi_mini_uart_irq_en(struct uart *dev, const struct uart_params *params) {
	COM3_IER |= MINI_UART_IER_RX_IRQ;
	return 0;
}

static int rpi_mini_uart_irq_dis(struct uart *dev, const struct uart_params *params) {
	COM3_IER &= ~MINI_UART_IER_RX_IRQ;
	return 0;
}

static int rpi_mini_uart_putc(struct uart *dev, int ch) {
	while ((COM3_LSR & 0x20) == 0);	/* wait until we can send */

	COM3_IO = ch;

	return 0;
}

static int rpi_mini_uart_getc(struct uart *dev) {
	return COM3_IO;
}

static int rpi_mini_uart_has_symbol(struct uart *dev) {
	return COM3_LSR & 0x01;
}


static const struct uart_ops rpi_mini_uart_ops = {
		.uart_getc = rpi_mini_uart_getc,
		.uart_putc = rpi_mini_uart_putc,
		.uart_hasrx = rpi_mini_uart_has_symbol,
		.uart_setup = rpi_mini_uart_setup,
		.uart_irq_en = rpi_mini_uart_irq_en,
		.uart_irq_dis = rpi_mini_uart_irq_dis,
};

static struct uart uart0 = {
		.uart_ops = &rpi_mini_uart_ops,
		.irq_num = COM0_IRQ_NUM,
		.base_addr = COM_BASE,
};

static const struct uart_params uart_defparams = {
		.baud_rate = OPTION_GET(NUMBER,baud_rate),
		.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_USE_IRQ,
};

static const struct uart_params uart_diag_params = {
		.baud_rate = OPTION_GET(NUMBER,baud_rate),
		.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
};

DIAG_SERIAL_DEF(&uart0, &uart_diag_params);

static int rpi_mini_uart_init(void) {
	return uart_register(&uart0, &uart_defparams);
}

PERIPH_MEMORY_DEFINE(rpi_mini_uart, COM_BASE, 0x1000);
