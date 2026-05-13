#include <gen_board_conf.h>
#include <stm32.h>
#include <stm32f2_chip.h>

struct clk_conf clks[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "RCC",
			.regs = {
				REGMAP("BASE", (RCC_BASE), 0x100),
			},
			
		},
		.type = {
			VAL("", 0),
		},
	},
};

struct gpio_conf gpios[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_A",
			.regs = {
				REGMAP("BASE", (GPIOA_BASE), 0x100),
			},
			.irqs = {
				VAL("", 0),
			},
			.clocks = {
				VAL("",   CLK_GPIOA),
			}
		},
		.port_num = 7,
		.port_width = 16,
	},
};

struct uart_conf uarts[] = {
	[1] = {
		.status = DISABLED,
		.name = "USART1",
		.dev = {
			.name = "USART1",
			.regs = {
				REGMAP("BASE", (USART1_BASE), 0x100),
			},
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
		.status = DISABLED,
		.name = "USART2",
		.dev = {
			.name = "USART2",
			.regs = {
				REGMAP("BASE", (USART2_BASE), 0x100),
			},
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
	[3] = {
		.status = ENABLED,
		.name = "USART3",
		.dev = {
			.name = "USART3",
			.regs = {
				REGMAP("BASE", (USART3_BASE), 0x100),
			},
			.irqs = {
				VAL("", USART3_IRQn),
			},
			.pins = {
				PIN("TX", GPIO_PORT_D, 8, GPIO_MODE_ALT, AF7),
				PIN("RX", GPIO_PORT_D, 9, GPIO_MODE_ALT, AF7),
			},
			.clocks = {
				VAL("UART", CLK_USART3),
			}
		},
		.baudrate = 115200,
	},
};

struct led_conf leds[] = {
	[0] = {
		.name = "LED1",
		.port = VAL("", GPIO_PORT_B),
		.pin = VAL("", 0),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[1] = {
		.name = "LED2",
		.port = VAL("", GPIO_PORT_B),
		.pin = VAL("", 7),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[2] = {
		.name = "LED3",
		.port = VAL("", GPIO_PORT_B),
		.pin = VAL("", 14),
		.level = VAL("", GPIO_PIN_HIGH),
	},
};

struct spi_conf spis[] = {
	[1] = {
		.status = DISABLED,
		.name = "SPI1",
		.dev = {
			.name = "SPI1",
			.regs = {
				REGMAP("BASE", (SPI1_BASE), 0x100),
			},
			.pins = {
				PIN("SCK",  GPIO_PORT_A, 5, GPIO_MODE_ALT, AF5),
				PIN("MISO", GPIO_PORT_A, 6, GPIO_MODE_ALT, AF5),
				PIN("MOSI", GPIO_PORT_A, 7, GPIO_MODE_ALT, AF5),
				PIN("CS",   GPIO_PORT_D, 14, GPIO_MODE_OUT, NOAF),
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
					PIN("CS",   GPIO_PORT_D, 14, GPIO_MODE_ALT, NOAF),
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
				PIN("SCL", GPIO_PORT_B, 6, GPIO_MODE_ALT, AF4),
				PIN("SDA", GPIO_PORT_B, 9, GPIO_MODE_ALT, AF4),
			},
			.clocks = {
				VAL("I2C", CLK_I2C1),
			}
		},
	},
	[2] = {
		.status = DISABLED,
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
				PIN("SCL", GPIO_PORT_B, 10, GPIO_MODE_ALT, AF4),
				PIN("SDA", GPIO_PORT_B, 11, GPIO_MODE_ALT, AF4),
			},
			.clocks = {
				VAL("I2C", CLK_I2C2),
			}
		},
	},

};

EXPORT_CONFIG(CLK(clks), GPIO(gpios),
				UART(uarts), LED(leds), SPI(spis), I2C(i2cs))
