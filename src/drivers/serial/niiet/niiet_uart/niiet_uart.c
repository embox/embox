/**
 * @file
 * @brief
 *
 * @author  Andrew Bursian
 * @date    20.02.2023
 */

#include <stdint.h>
#include <hal/reg.h>
#include <drivers/diag.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/gpio/gpio.h>
//#include <drivers/gpio/gpio_driver.h>

#include <framework/mod/options.h>

#define UART_BASE OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM   OPTION_GET(NUMBER, irq_num)
#define UARTCLK   OPTION_GET(NUMBER, uartclk)
#define BAUD_RATE OPTION_GET(NUMBER, baud_rate)
#define BAUD_ICOEF (UARTCLK / (16 * BAUD_RATE))
#define BAUD_FCOEF ((UARTCLK / (16.0f * BAUD_RATE) - BAUD_ICOEF) * 64 + 0.5f)

#define RCU				0x40041000

#define RCU_SYSCLKCFG			0x40041010
#define RCU_SYSCLKCFG_SYSSEL_OSICLK	0x00000000
#define RCU_SYSCLKCFG_SYSSEL_OSECLK	0x00000001
#define RCU_SYSCLKCFG_SYSSEL_PLLCLK	0x00000002
#define RCU_SYSCLKCFG_SYSSEL_PLLDIVCLK	0x00000003

#define RCU_SYSCLKSTAT			0x40041014
#define RCU_SYSCLKSTAT_SYSSTAT_mask	0x00000003

#define RCU_UARTCFG			0x40041060
#define RCU_UART0CFG			0x40041060
#define RCU_UART1CFG			0x40041064
#define RCU_UARTCFG_CLKEN_mask		0x00000001
#define RCU_UARTCFG_RSTDIS_mask		0x00000010
#define RCU_UARTCFG_CLKSEL_OSECLK	0x00000000
#define RCU_UARTCFG_CLKSEL_PLLCLK	0x00000100
#define RCU_UARTCFG_CLKSEL_PLLDIVCLK	0x00000200
#define RCU_UARTCFG_CLKSEL_OSICLK	0x00000300

#define RCU_HCLKCFG			0x40041100
#define RCU_HCLKCFG_GPIOAEN_mask	0x00000001
#define RCU_HCLKCFG_GPIOBEN_mask	0x00000002
#define RCU_HRSTCFG			0x40041104
#define RCU_HRSTCFG_GPIOAEN_mask	0x00000001
#define RCU_HRSTCFG_GPIOBEN_mask	0x00000002

#define UART0				0x40045000
#define UART0_DR			0x40045000
#define UART0_FR			0x40045018
#define UART0_IBRD			0x40045024
#define UART0_FBRD			0x40045028
#define UART0_LCRH			0x4004502C
#define UART0_CR			0x40045030
#define UART1				0x40046000
#define UART1_DR			0x40046000
#define UART1_FR			0x40046018
#define UART1_IBRD			0x40046024
#define UART1_FBRD			0x40046028
#define UART1_LCRH			0x4004602C
#define UART1_CR			0x40046030

#define UART_DR_DATA_mask		0x000000FF
#define UART_FR_RXFE_mask		0x00000010
#define UART_FR_BUSY_mask		0x00000008
#define UART_LCRH_FEN_mask		0x00000010
#define UART_LCRH_WLEN_8bit		0x00000060
#define UART_CR_UARTEN_mask		0x00000001
#define UART_CR_TXE_mask		0x00000100
#define UART_CR_RXE_mask		0x00000200

#define UART0_GPIO			GPIO_PORT_B
#define UART0_GPIO_TX_mask		(1<<10)
#define UART0_GPIO_RX_mask		(1<<11)
#define UART1_GPIO_TX_mask		(1<<8)
#define UART1_GPIO_RX_mask		(1<<9)


static int niiet_uart_setup(struct uart *dev, const struct uart_params *params) {
#if 0
	/* Disable uart. */
	REG_STORE(UART0_CR, 0);
	/* Enable uart0 pins */
	gpio_setup_mode(UART0_GPIO, UART0_GPIO_TX_mask | UART0_GPIO_RX_mask, GPIO_MODE_OUT_ALTERNATE);
	/* Clock for uart0 */
	REG32_STORE(RCU_UART0CFG, RCU_UARTCFG_CLKEN_mask | RCU_UARTCFG_RSTDIS_mask | RCU_UARTCFG_CLKSEL_PLLCLK);
	/* Set baud rate */
	REG32_STORE(UART0_IBRD, BAUD_ICOEF);
	REG32_STORE(UART0_FBRD, BAUD_FCOEF);
	/* FIFO enable, 8 data bits */
	REG32_STORE(UART0_LCRH, UART_LCRH_FEN_mask | UART_LCRH_WLEN_8bit);
	/* Enable UART */
	REG32_STORE(UART0_CR, UART_CR_TXE_mask | UART_CR_RXE_mask | UART_CR_UARTEN_mask);
#endif
	return 0;
}

static int niiet_uart_putc(struct uart *dev, int ch) {
#if 0
	while (REG32_LOAD(UART0_FR) & UART_FR_BUSY_mask);
	REG32_STORE(UART0_DR, (uint32_t) ch);
#endif
	return 0;
}

static int niiet_uart_hasrx(struct uart *dev) {
	return 0;
#if 0
	return !(REG32_LOAD(UART0_FR) & UART_FR_RXFE_mask);
#endif
}

static int niiet_uart_getc(struct uart *dev) {
	return 0;
#if 0
	return (int) (REG32_LOAD(UART0_DR) & UART_DR_DATA_mask);
#endif
}

const struct uart_ops niiet_uart_ops = {
		.uart_setup = niiet_uart_setup,
		.uart_putc  = niiet_uart_putc,
		.uart_hasrx = niiet_uart_hasrx,
		.uart_getc  = niiet_uart_getc,
		.uart_irq_en = NULL,
		.uart_irq_dis = NULL,
};


