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
				PIN("TX", PA, PIN_2, AF7),
				PIN("RX", PA, PIN_3, AF7),
			},
			.clocks = {
				VAL("TX",   CLK_GPIOA),
				VAL("RX",   CLK_GPIOA),
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
				PIN("TX", PD, PIN_8, AF7),
				PIN("RX", PD, PIN_9, AF7),
			},
			.clocks = {
				VAL("TX",   CLK_GPIOD),
				VAL("RX",   CLK_GPIOD),
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
				PIN("TX", PC, PIN_6, AF8),
				PIN("RX", PC, PIN_7, AF8),
			},
			.clocks = {
				VAL("TX",   CLK_GPIOC),
				VAL("RX",   CLK_GPIOC),
				VAL("UART", CLK_USART6),
			}
		},
		.baudrate = 115200,
	},
};

EXPORT_CONFIG(UART(uarts))
