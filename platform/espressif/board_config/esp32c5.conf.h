#include <gen_board_conf.h>

#include <soc/esp32c5_gpio.h>
#include <soc/esp32c5_gpio_sig_map.h>
#include <soc/esp32c5_uart.h>

struct gpio_conf gpios[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_A",
			.regs = {
				REGMAP("BASE", GPIO_BASE, 0x1000),
				REGMAP("IO_MUX", IO_MUX_BASE, 0x1000),
			},
		},
		.port_num = 1,
		.port_width = 29,
	},
};

/* Interrupt sources must be routed to CLIC before assigning CPU IRQs. */
struct uart_conf uarts[] = {
	[0] = {
		.status = ENABLED,
		.name = "UART0",
		.dev = {
			.name = "UART0",
			.regs = {
				REGMAP("BASE_ADDR", UART0_BASE, 0x1000),
			},
			/* Direct UART0 connection through IO_MUX. */
			.pins = {
				PIN("TX", GPIO_PORT_A, GPIO11,
					GPIO_MODE_ALT, FUNC_U0TXD_U0TXD),
				PIN("RX", GPIO_PORT_A, GPIO12,
					GPIO_MODE_ALT, FUNC_U0RXD_U0RXD),
			},
		},
		.baudrate = 115200,
	},
	[1] = {
		.status = ENABLED,
		.name = "UART1",
		.dev = {
			.name = "UART1",
			.regs = {
				REGMAP("BASE_ADDR", UART1_BASE, 0x1000),
			},
			/* Example matrix routing; check pin availability on the board. */
			.pins = {
				PIN("TX", GPIO_PORT_A, GPIO23,
					GPIO_MODE_ALT, ESP32_MATRIX_OUT(U1TXD_OUT)),
				PIN("RX", GPIO_PORT_A, GPIO24,
					GPIO_MODE_ALT, ESP32_MATRIX_IN(U1RXD_IN)),
			},
		},
		.baudrate = 115200,
	},
};


EXPORT_CONFIG(GPIO(gpios), UART(uarts));
