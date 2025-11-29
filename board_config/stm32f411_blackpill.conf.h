#include <gen_board_conf.h>
#include <stm32.h>

struct uart_conf uarts[] = {
 	[1] = {
 		.status = DISABLED,
		.name = "USART1",
		.dev = {
			.name = "USART1",
			.irqs = {
				VAL("", 37),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 9, AF7),
				PIN("RX", GPIO_PORT_A, 10, AF7),
			},
			.clocks = {
				VAL("TX",   CLK_GPIOA),
				VAL("RX",   CLK_GPIOA),
				VAL("UART", CLK_USART1),
			}
		},
		.baudrate = 115200,
	},
	[2] = {
		.status = ENABLED,
		.name = "USART2",
		.dev = {
			.name = "USART2",
			.irqs = {
				VAL("", 38),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 2, AF7),
				PIN("RX", GPIO_PORT_A, 3, AF7),
			},
			.clocks = {
				VAL("TX",   CLK_GPIOA),
				VAL("RX",   CLK_GPIOA),
				VAL("UART", CLK_USART2),
			}
		},
		.baudrate = 115200,
	},
};

struct led_conf leds[] = {
	[0] = {
		.status = ENABLED,
		.name = "LED",
		.port = VAL("", GPIO_PORT_C),
		.pin = VAL("", 13),
		.level = VAL("", GPIO_PIN_HIGH),
	},
};

EXPORT_CONFIG(UART(uarts), LED(leds))
