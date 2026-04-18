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
				PIN("TX", GPIO_PORT_B, 6, GPIO_MODE_AF, AF7),
				PIN("RX", GPIO_PORT_B, 7, GPIO_MODE_AF, AF7),
			},
			.clocks = {
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
				PIN("TX", GPIO_PORT_A, 2, GPIO_MODE_AF, AF7),
				PIN("RX", GPIO_PORT_A, 3, GPIO_MODE_AF, AF7),
			},
			.clocks = {
				VAL("UART", CLK_USART2),
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
				PIN("SCK",  GPIO_PORT_A, 5, GPIO_MODE_AF, AF5),
				PIN("MISO", GPIO_PORT_A, 6, GPIO_MODE_AF, AF5),
				PIN("MOSI", GPIO_PORT_A, 7, GPIO_MODE_AF, AF5),
				PIN("CS",   GPIO_PORT_A, 4, GPIO_MODE_OUT, NOAF),
			},
			.clocks = {
				VAL("SPI",  CLK_SPI1),
			}
		},
		.spi_devs[0] = {
			.status = ENABLED,
			.name = "SPI1_0",
			.bits_per_word = 8,
			.bus_num = 1,
			.idx     = 0,
			.dev = {
				.name = "SPI1_0",
				.pins = {
					PIN("CS",   GPIO_PORT_A, 4, GPIO_MODE_OUT, NOAF),
				},
			},
		},
	},
	[2] = {
		.status = DISABLED,
		.name = "SPI2",
		.dev = {
			.name = "SPI2",
			.pins = {
				PIN("SCK",  GPIO_PORT_B, 10, GPIO_MODE_AF, AF5),
				PIN("MISO", GPIO_PORT_B, 14, GPIO_MODE_AF, AF5),
				PIN("MOSI", GPIO_PORT_B, 15, GPIO_MODE_AF, AF5),
				PIN("CS",   GPIO_PORT_B, 12, GPIO_MODE_OUT, NOAF),
			},
			.clocks = {
				VAL("SPI",  CLK_SPI2),
			}
		},
		.spi_devs[0] = {
			.status = ENABLED,
			.name = "SPI2_0",
			.bits_per_word = 8,
			.bus_num = 2,
			.idx     = 0,
			.dev = {
				.name = "SPI2_0",
				.pins = {
					PIN("CS",   GPIO_PORT_B, 12, GPIO_MODE_OUT, NOAF),
				},
			},
		},
	},
};

EXPORT_CONFIG(UART(uarts), SPI(spis))
