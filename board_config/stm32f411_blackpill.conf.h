#include <gen_board_conf.h>
#include <stm32.h>
#include <stm32f4_chip.h>

struct uart_conf uarts[] = {
 	[1] = {
 		.status = DISABLED,
		.name = "USART1",
		.dev = {
			.name = "USART1",
			.irqs = {
				VAL("", USART1_IRQn),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 9, GPIO_MODE_ALT, AF7),
				PIN("RX", GPIO_PORT_A, 10, GPIO_MODE_ALT, AF7),
			},
			.clocks = {
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
				VAL("", USART2_IRQn),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 2, GPIO_MODE_ALT, AF7),
				PIN("RX", GPIO_PORT_A, 3, GPIO_MODE_ALT, AF7),
			},
			.clocks = {
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
