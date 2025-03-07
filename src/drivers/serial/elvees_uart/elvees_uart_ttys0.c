/**
 * @file
 *
 * @date Feb 11, 2023
 * @author Anton Bondarev
 */

#include <kernel/irq.h>

#include <util/macro.h>

#include <drivers/serial/uart_dev.h>
#include <drivers/ttys.h>

#include <drivers/common/memory.h>

#include <framework/mod/options.h>

#define UART_BASE      OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM        OPTION_GET(NUMBER, irq_num)

#define PINS_INIT      OPTION_GET(NUMBER, pins_init)

#define TTY_NAME    ttyS0

extern int elvees_uart_setup_common(struct uart *dev, const struct uart_params *params);
extern int elvees_uart_has_symbol(struct uart *dev);
extern int elvees_uart_getc(struct uart *dev);
extern int elvees_uart_putc(struct uart *dev, int ch);

#if PINS_INIT == 0

static int elvees_uart_setup(struct uart *dev, const struct uart_params *params) {
	return 0;
}

#else

#include <drivers/gpio/gpio_driver.h>

#define PIN_RX_PORT      OPTION_GET(NUMBER, pin_rx_port)
#define PIN_RX_PIN       OPTION_GET(NUMBER, pin_rx_pin)
#define PIN_TX_PORT      OPTION_GET(NUMBER, pin_tx_port)
#define PIN_TX_PIN       OPTION_GET(NUMBER, pin_tx_pin)

#if 0
static int elvees_uart_setup(struct uart *dev, const struct uart_params *params) {
#define GPIO_ALT_FUNC_UART   (4)

	gpio_setup_mode(PIN_RX_PORT, 1 << PIN_RX_PIN, GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(GPIO_ALT_FUNC_UART));
	gpio_setup_mode(PIN_TX_PORT, 1 << PIN_TX_PIN, GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(GPIO_ALT_FUNC_UART));

	elvees_uart_setup_common(dev, params);

	return 0;
}
#endif
#endif

#if 0
static const struct uart_ops elvees_uart_uart_ops = {
		.uart_getc = elvees_uart_getc,
		.uart_putc = elvees_uart_putc,
		.uart_hasrx = elvees_uart_has_symbol,
		.uart_setup = elvees_uart_setup,
};
#endif

extern const struct uart_ops elvees_uart_uart_ops;

extern irq_return_t uart_irq_handler(unsigned int irq_nr, void *data);

static struct uart uart_ttyS0 = {
		.dev_name = MACRO_STRING(TTY_NAME),
		.uart_ops = &elvees_uart_uart_ops,
		.irq_num = IRQ_NUM,
		.base_addr = UART_BASE,
		.params =  {
			.baud_rate = OPTION_GET(NUMBER,baud_rate),
			.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_USE_IRQ,
		},
};

PERIPH_MEMORY_DEFINE(elvees_uart, UART_BASE, 0x1000);

STATIC_IRQ_ATTACH(IRQ_NUM, uart_irq_handler, &uart_ttyS0);

TTYS_DEF(TTY_NAME, &uart_ttyS0);
