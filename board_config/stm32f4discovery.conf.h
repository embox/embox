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

struct spi_conf spis[] = {
	[1] = {
		.status = DISABLED,
		.name = "SPI1",
		.dev = {
			.name = "SPI1",
			.pins = {
				PIN("SCK",  PB, PIN_3, AF5),
				PIN("MISO", PB, PIN_4, AF5),
				PIN("MOSI", PB, PIN_5, AF5),
				PIN("CS",   PB, PIN_2, NOAF),
			},
			.clocks = {
				VAL("SCK",  CLK_GPIOB),
				VAL("MISO", CLK_GPIOB),
				VAL("MOSI", CLK_GPIOB),
				VAL("CS",   CLK_GPIOB),
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
				PIN("SCK",  PB, PIN_13, AF5),
				PIN("MISO", PB, PIN_14, AF5),
				PIN("MOSI", PB, PIN_15, AF5),
				PIN("CS",   PB, PIN_12, NOAF),
			},
			.clocks = {
				VAL("SCK",  CLK_GPIOB),
				VAL("MISO", CLK_GPIOB),
				VAL("MOSI", CLK_GPIOB),
				VAL("CS",   CLK_GPIOB),
				VAL("SPI",  CLK_SPI2),
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
				VAL("SCL", CLK_GPIOB),
				VAL("SDA", CLK_GPIOB),
				VAL("I2C", CLK_I2C1),
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
				VAL("SCL", CLK_GPIOB),
				VAL("SDA", CLK_GPIOB),
				VAL("I2C", CLK_I2C1),
			}
		},
	},

};

struct pwm_conf pwms[] = {
	[0] = {
		.name = "PWM0",
		.channel = VAL("CHANNEL_TIM", CHANNEL_TIM1),
		.instance = VAL("INSTANCE", TIM4),
		.servo_low = VAL("LOW", 200),
		.servo_high = VAL("HIGH", 1350),
		.dev = {
			.name = "PWM0",
			.pins = {
				PIN("TIM",  PB, PIN_6, AF2),
			},
			.clocks = {
				VAL("GPIO",  CLK_GPIOB),
				VAL("TIM",  CLK_TIM4),
			}
		},
	},
};

EXPORT_CONFIG(UART(uarts), SPI(spis), I2C(i2cs), PWM(pwms))
