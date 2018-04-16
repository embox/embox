/**
 * @file e2k.c
 * @brief Monocube serial driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 18.12.2017
 */

#include <asm/io.h>
#include <drivers/common/memory.h>
#include <drivers/diag.h>
#include <drivers/serial/uart_device.h>
#include <drivers/serial/diag_serial.h>
#include <embox/unit.h>

#include "am85c30.h"

EMBOX_UNIT_INIT(uart_init);

#define UART_BASE         0x83200040
#define IRQ_NUM 0


#if 0
static unsigned long		am85c30_com_port = 0;

#define	boot_am85c30_com_port		boot_get_vo_value(am85c30_com_port)
#define	boot_am85c30_serial_boot_console		\
		((serial_console_opts_t)	\
			(boot_get_vo_value(am85c30_serial_boot_console)))
#endif
static inline void
am85c30_com_outb(void *iomem_addr, uint8_t byte)
{
	e2k_write8(byte, iomem_addr);
	wmb();	/* waiting for write to serial port completion */
}

static inline uint8_t
am85c30_com_inb(void *iomem_addr)
{
	rmb();	/* waiting for read from serial port completion */
	wmb();	/* waiting for write to serial port completion */
	return e2k_read8(iomem_addr);
}

static inline uint8_t
am85c30_com_inb_command(void* iomem_addr, uint8_t reg_num)
{
	e2k_write8(reg_num, iomem_addr);
	wmb();	/* waiting for write to serial port completion */
	rmb();	/* waiting for read from serial port completion */
	return e2k_read8(iomem_addr);
}

static inline void
am85c30_com_outb_command(void *iomem_addr, uint8_t reg_num, uint8_t val)
{
	e2k_write8(reg_num, iomem_addr);
	wmb();	/* waiting for write to serial port completion */

	e2k_write8(val, iomem_addr);
	wmb();	/* waiting for write to serial port completion */
}

static int am85c30_setup(struct uart *dev, const struct uart_params *params) {
	//unsigned long port;

	//port = UART_BASE;
	//uint8_t val;

	//val = am85c30_com_inb_command(port, AM85C30_RR0);

	//am85c30_com_outb_command(port, 0, 0);

	return 0;
}

int am85c30_putc(struct uart *dev, int ch) {
	unsigned long port;

	port = UART_BASE;
	while ((am85c30_com_inb_command(port, AM85C30_RR0) & AM85C30_D2) == 0)
		;
	am85c30_com_outb(port + 0x01, ch);

	return 0;
}

static int am85c30_has_symbol(struct uart *dev) {
	unsigned long port;

	port = UART_BASE;
	uint8_t val;

	val = am85c30_com_inb_command(port, AM85C30_RR0);

	//e2k_putc(dev, (val &0xF));
	//printk("0x%x\n", val);
	return val & 1;
}

static int am85c30_getc(struct uart *dev) {
	unsigned long port;

	am85c30_putc(dev, '8');
	port = UART_BASE;
	while (((am85c30_com_inb_command(port, AM85C30_RR0)) & AM85C30_D0) == 0)
		;
	return am85c30_com_inb(port + 0x01);
}


static const struct uart_ops am85c30_uart_ops = {
	.uart_getc = am85c30_getc,
	.uart_putc = am85c30_putc,
	.uart_hasrx = am85c30_has_symbol,
	.uart_setup = am85c30_setup,
};

static struct uart uart0 = {
	.uart_ops = &am85c30_uart_ops,
	.irq_num = IRQ_NUM,
	.base_addr = UART_BASE,
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

const struct uart_diag DIAG_IMPL_NAME(__EMBUILD_MOD__) = {
	.diag = {
		.ops = &uart_diag_ops,
	},
	.uart = &uart0,
	.params = &uart_diag_params,
};

static int uart_init(void) {
	return uart_register(&uart0, &uart_defparams);
}
