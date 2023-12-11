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


#define UART0_GPIO			GPIO_PORT_B
#define UART0_GPIO_TX_mask		(1<<10)
#define UART0_GPIO_RX_mask		(1<<11)
#define UART1_GPIO_TX_mask		(1<<8)
#define UART1_GPIO_RX_mask		(1<<9)

extern const struct uart_ops niiet_uart_ops;

static struct uart uart0 = {
		.uart_ops = &niiet_uart_ops,
		.irq_num = IRQ_NUM,
		.base_addr = UART_BASE,
};

static const struct uart_params uart_diag_params = {
		.baud_rate = BAUD_RATE,
		.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_DEV_TYPE_UART,
};

DIAG_SERIAL_DEF(&uart0, &uart_diag_params);

