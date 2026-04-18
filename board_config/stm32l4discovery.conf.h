#include <gen_board_conf.h>
#include <stm32.h>

#define PERIPH_BASE           (0x40000000UL) /*!< Peripheral base address */
#define APB1PERIPH_BASE        PERIPH_BASE
#define I2C1_BASE             (APB1PERIPH_BASE + 0x5400UL)
#define I2C2_BASE             (APB1PERIPH_BASE + 0x5800UL)

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
				PIN("CS",   GPIO_PORT_A, 15, GPIO_MODE_OUT, NOAF),
			},
			.clocks = {
				VAL("SPI",  CLK_SPI1),
			},
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
					PIN("CS",   GPIO_PORT_A, 15, GPIO_MODE_OUT, NOAF),
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
			},
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
	[3] = {
		.status = DISABLED,
		.name = "SPI3",
		.bits_per_word = 16,
		.clk_div = 8,
		.dev = {
			.name = "SPI3",
			.pins = {
				PIN("SCK",  GPIO_PORT_C, 10, GPIO_MODE_AF, AF6),
				PIN("MISO", GPIO_PORT_C, 11, GPIO_MODE_AF, AF6),
				PIN("MOSI", GPIO_PORT_C, 12, GPIO_MODE_AF, AF6),
				PIN("CS",   GPIO_PORT_E, 0, GPIO_MODE_OUT, NOAF),	   // for WiFi module
				PIN("WAKE", GPIO_PORT_B, 13, 0, NOAF),    // for WiFi module
				PIN("CMDDATA", GPIO_PORT_E, 1, 0, NOAF),  // for WiFi module
				PIN("RESET", GPIO_PORT_E, 8, 0, NOAF),    // for WiFi module
				PIN("WIFI_LED", GPIO_PORT_C, 9, 0, NOAF), // for WiFi module
			},
			.clocks = {
				VAL("SPI",  CLK_SPI3),
			},
		},
		.spi_devs[0] = {
			.status = ENABLED,
			.name = "SPI3_0",
			.bits_per_word = 8,
			.bus_num = 3,
			.idx     = 0,
			.dev = {
				.name = "SPI3_0",
			},
		},
		.spi_devs[1] = {
			.status = ENABLED,
			.name = "ESWIFI",
			.bits_per_word = 16,
			.clk_freq = 8, /* clk_div */
			.bus_num = 3,
			.idx     = 1,
			.dev = {
				.name = "ESWIFI",
				.pins = {
					PIN("CS",   GPIO_PORT_E, 0, 0, NOAF),	   // for WiFi module
					PIN("WAKE", GPIO_PORT_B, 13, 0, NOAF),    // for WiFi module
					PIN("CMDDATA", GPIO_PORT_E, 1, 0, NOAF),  // for WiFi module
					PIN("RESET", GPIO_PORT_E, 8, 0, NOAF),    // for WiFi module
					PIN("WIFI_LED", GPIO_PORT_C, 9, 0, NOAF), // for WiFi module
				},
			},
		},
	},
};

struct i2c_conf i2cs[] = {
	[1] = {
		.status = ENABLED,
		.name = "I2C1",
		.dev = {
			.name = "I2C1",
			.regs = {
				REGMAP("BASE", (I2C1_BASE), 0x100),
			},
			.irqs = {
				VAL("EVENT", 31),
				VAL("ERROR", 32),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_B, 6, GPIO_MODE_AF, AF4),
				PIN("SDA", GPIO_PORT_B, 9, GPIO_MODE_AF, AF4),
			},
			.clocks = {
				VAL("I2C", CLK_I2C1),
			}
		},
	},
	[2] = {
		.status = ENABLED,
		.name = "I2C2",
		.dev = {
			.name = "I2C2",
			.regs = {
				REGMAP("BASE", (I2C2_BASE), 0x100),
			},
			.irqs = {
				VAL("EVENT", 33),
				VAL("ERROR", 34),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_B, 10, GPIO_MODE_AF, AF4),
				PIN("SDA", GPIO_PORT_B, 11, GPIO_MODE_AF, AF4),
			},
			.clocks = {
				VAL("I2C", CLK_I2C2),
			}
		},
	},

};


EXPORT_CONFIG(UART(uarts), SPI(spis), I2C(i2cs))
