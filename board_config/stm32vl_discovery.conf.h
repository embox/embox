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
				PIN("TX", PA, PIN_9, AF7),
				PIN("RX", PA, PIN_10, AF7),
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
				PIN("TX", PB, PIN_10, AF7),
				PIN("RX", PB, PIN_11, AF7),
			},
			.clocks = {
				VAL("TX",   CLK_GPIOB),
				VAL("RX",   CLK_GPIOB),
				VAL("UART", CLK_USART3),
			}
		},
		.baudrate = 115200,
	},
};

EXPORT_CONFIG(UART(uarts))
