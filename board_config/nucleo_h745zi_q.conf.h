#include <gen_board_conf.h>
#include <stm32.h>

struct uart_conf uarts[] = {
	[2] = {
		.status = DISABLED,
		.name = "USART2",
		.dev = {
			.name = "USART2",
			.irqs = {
				VAL("", 38),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 2, GPIO_MODE_AF, AF7),
				PIN("RX", GPIO_PORT_A, 3, GPIO_MODE_AF, AF7),
			},
			.clocks = {
				VAL("UART", CLK_USART2),
			}
		},
		.baudrate = 115200,
	},
	[3] = {
		.status = ENABLED,
		.name = "USART3",
		.dev = {
			.name = "USART3",
			.irqs = {
				VAL("", 39),
			},
			.pins = {
				PIN("TX", GPIO_PORT_D, 8, GPIO_MODE_AF, AF7),
				PIN("RX", GPIO_PORT_D, 9, GPIO_MODE_AF, AF7),
			},
			.clocks = {
				VAL("UART", CLK_USART3),
			}
		},
		.baudrate = 115200,
	},
	[6] = {
		.status = ENABLED,
		.name = "USART6",
		.dev = {
			.name = "USART6",
			.irqs = {
				VAL("", 71),
			},
			.pins = {
				PIN("TX", GPIO_PORT_C, 6, GPIO_MODE_AF, AF8),
				PIN("RX", GPIO_PORT_C, 7, GPIO_MODE_AF, AF8),
			},
			.clocks = {
				VAL("UART", CLK_USART6),
			}
		},
		.baudrate = 115200,
	},
};

EXPORT_CONFIG(UART(uarts))
