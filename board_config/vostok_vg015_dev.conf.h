#include <gen_board_conf.h>


struct clk_conf clks[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "RCU",
			.regs = {
				REGMAP("BASE", (RCU_BASE), 0x100),
			},
			.clocks = {
				VAL("HSECLK_VAL", 16000000UL),
			}
		},
		.type = VAL("SYSCLK_PLL", 1),
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
				VAL("", "CLK_GPIOA"),
			}
		},
		.port_num = 3,
		.port_width = 16,
	},
	[1] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_B",
			.regs = {
				REGMAP("BASE", (GPIOB_BASE), 0x100),
			},
			.irqs = {
				VAL("", 0),
			},
			.clocks = {
				VAL("", "CLK_GPIOB"),
			}
		},
		.port_num = 3,
		.port_width = 16,
	},
	[2] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_C",
			.regs = {
				REGMAP("BASE", (GPIOC_BASE), 0x100),
			},
			.irqs = {
				VAL("", 0),
			},
			.clocks = {
				VAL("", "CLK_GPIOC"),
			}
		},
		.port_num = 3,
		.port_width = 16,
	},
};

struct uart_conf uarts[] = {
	[0] = {
		.status = ENABLED,
		.name = "USART0",
		.dev = {
			.name = "USART0",
			.regs = {
				REGMAP("BASE_ADDR", UART0_BASE, 0x100),
			},
			.irqs = {
				//VAL("", 37),
			},
			.pins = {
				PIN("TX", PGPIO_PORT_A, 1, 1),
				PIN("RX", GPIO_PORT_A, 0, 1),
			},
			.clocks = {
				VAL("", "CLK_USART1"),
			}
		},
		.baudrate = 115200,
	},
	[1] = {
		.status = DISABLED,
		.name = "USART1",
		.dev = {
			.name = "USART1",
			.regs = {
				REGMAP("BASE_ADDR", UART1_BASE, 0x100),
			},
			.irqs = {
				//VAL("", 37),
			},
			.pins = {
				//PIN("TX", PGPIO_PORT_A, 1, 1),
				//PIN("RX", GPIO_PORT_A, 0, 1),
			},
			.clocks = {
				VAL("", "CLK_USART1"),
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
				//VAL("", 38),
			},
			.pins = {
				//PIN("TX", PGPIO_PORT_A, 1, 1),
				//PIN("RX", GPIO_PORT_A, 0, 1),
			},
			.clocks = {
				VAL("", "CLK_USART2"),
			}
		},
		.baudrate = 115200,
	},
	[3] = {
		.status = DISABLED,
		.name = "USART3",
		.dev = {
			.irqs = {
				//VAL("", 39),
			},
			.pins = {
				//PIN("TX", PGPIO_PORT_A, 1, 1),
				//PIN("RX", GPIO_PORT_A, 0, 1),
			},
			.clocks = {
				VAL("", "CLK_USART3"),
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
				PIN("SCK",  PA, PIN_5, AF5),
				PIN("MISO", PA, PIN_6, AF5),
				PIN("MOSI", PA, PIN_7, AF5),
				PIN("CS",   PD, PIN_14, NOAF),
			},
			.clocks = {
				VAL("SPI",  "CLK_SPI1"),
			}
		},
	},

};

struct i2c_conf i2cs[] = {
	[1] = {
		.status = ENABLED,
		.name = "I2C1",
		.dev = {
			.name = "I2C1",
			.irqs = {
				VAL("EVENT_IRQ", 31),
				VAL("ERROR_IRQ", 32),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_B, PIN_6, AF4),
				PIN("SDA", GPIO_PORT_B, PIN_9, AF4),
			},
			.clocks = {
				VAL("", "CLK_I2C1"),
			}
		},
	},
	[2] = {
		.status = DISABLED,
		.name = "I2C2",
		.dev = {
			.name = "I2C2",
			.irqs = {
				VAL("EVENT_IRQ", 33),
				VAL("ERROR_IRQ", 34),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_B, PIN_10, AF4),
				PIN("SDA", GPIO_PORT_B, PIN_11, AF4),
			},
			.clocks = {
				VAL("", "CLK_I2C2"),
			}
		},
	},
};


EXPORT_CONFIG(CLK(clks), GPIO(gpios), UART(uarts))
