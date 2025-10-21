#include <gen_board_conf.h>
#include <stm32.h>

struct uart_conf uarts[] = {
 	[1] = {
 		.status = ENABLED,
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
	[6] = {
		.status = DISABLED,
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

struct spi_conf spis[] = {
	[1] = {
		.status = DISABLED,
		.name = "SPI1",
		.dev = {
			.name = "SPI1",
			.pins = {
				PIN("SCK",  GPIO_PORT_B, 3, AF5),
				PIN("MISO", GPIO_PORT_B, 4, AF5),
				PIN("MOSI", GPIO_PORT_B, 5, AF5),
				PIN("CS",   GPIO_PORT_B, 2, NOAF),
			},
			.clocks = {
				VAL("SPI",  CLK_SPI1),
			}
		},
	},
	[2] = {
		.status = DISABLED,
		.name = "SPI2",
		.dev = {
			.name = "SPI2",
			.pins = {
				PIN("SCK",  GPIO_PORT_B, 13, AF5),
				PIN("MISO", GPIO_PORT_B, 14, AF5),
				PIN("MOSI", GPIO_PORT_B, 15, AF5),
				PIN("CS",   GPIO_PORT_B, 12, NOAF),
			},
			.clocks = {
				VAL("SPI",  CLK_SPI2),
			}
		},
	},
	[5] = {
		.status = DISABLED,
		.name = "SPI5",
		.dev = {
			.name = "SPI5",
			.pins = {
				PIN("SCK",  GPIO_PORT_F, 7, AF5),
				PIN("MISO", GPIO_PORT_F, 8, AF5),
				PIN("MOSI", GPIO_PORT_F, 9, AF5),
				PIN("CS",   GPIO_PORT_C, 1, NOAF),
			},
			.clocks = {
				VAL("SPI",  CLK_SPI5),
			}
		},
	},
};

struct led_conf leds[] = {
	[0] = {
		.status = ENABLED,
		.name = "LED0",
		.port = VAL("", GPIO_PORT_G),
		.pin = VAL("", 13),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[1] = {
		.status = ENABLED,
		.name = "LED1",
		.port = VAL("", GPIO_PORT_G),
		.pin = VAL("", 14),
		.level = VAL("", GPIO_PIN_HIGH),
	},
};

EXPORT_CONFIG(UART(uarts), SPI(spis), LED(leds))
