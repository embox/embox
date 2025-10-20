#include <gen_board_conf.h>

#include <niiet_k1921vg015_chip.h>

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
		.type = {
			VAL("SYSCLK_PLL", 1),
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
				VAL("", PLIC_GPIO_VECTNUM),
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
				VAL("", PLIC_GPIO_VECTNUM),
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
				VAL("", PLIC_GPIO_VECTNUM),
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
		.name = "UART0",
		.dev = {
			.name = "UART0",
			.regs = {
				REGMAP("BASE_ADDR", (UART0_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_UART0_VECTNUM),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 1, 1),
				PIN("RX", GPIO_PORT_A, 0, 1),
			},
			.clocks = {
				VAL("", "CLK_UART0"),
			}
		},
		.baudrate = 115200,
	},
	[1] = {
		.status = DISABLED,
		.name = "UART1",
		.dev = {
			.name = "UART1",
			.regs = {
				REGMAP("BASE_ADDR", (UART1_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_UART1_VECTNUM),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 11, 3),
				PIN("RX", GPIO_PORT_A, 10, 3),
			},
			.clocks = {
				VAL("", "CLK_UART1"),
			}
		},
		.baudrate = 115200,
	},
	[2] = {
		.status = DISABLED,
		.name = "UART2",
		.dev = {
			.name = "UART2",
			.regs = {
				REGMAP("BASE_ADDR", (UART2_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_UART2_VECTNUM),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 5, 1),
				PIN("RX", GPIO_PORT_A, 4, 1),
			},
			.clocks = {
				VAL("", "CLK_UART2"),
			}
		},
		.baudrate = 115200,
	},
	[3] = {
		.status = DISABLED,
		.name = "UART3",
		.dev = {
			.name = "UART3",
			.regs = {
				REGMAP("BASE_ADDR", (UART3_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_UART3_VECTNUM),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 7, 1),
				PIN("RX", GPIO_PORT_A, 6, 1),
			},
			.clocks = {
				VAL("", "CLK_UART3"),
			}
		},
		.baudrate = 115200,
	},
	[4] = {
		.status = DISABLED,
		.name = "UART4",
		.dev = {
			.name = "UART4",
			.regs = {
				REGMAP("BASE_ADDR", (UART4_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_UART4_VECTNUM),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 9, 1),
				PIN("RX", GPIO_PORT_A, 8, 1),
			},
			.clocks = {
				VAL("", "CLK_UART4"),
			}
		},
		.baudrate = 115200,
	},
};

struct spi_conf spis[] = {
	[0] = {
		.status = ENABLED,
		.name = "SPI0",
		.dev = {
			.name = "SPI0",
			.regs = {
				REGMAP("BASE_ADDR", (SPI0_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_SPI0_VECTNUM),
			},
			.pins = {
				PIN("CLK", GPIO_PORT_B, 0, 1),
				PIN("FSS", GPIO_PORT_B, 1, 1),
				PIN("RX", GPIO_PORT_B, 2, 1),
				PIN("TX", GPIO_PORT_B, 3, 1),
			},
			.clocks = {
				VAL("SPI",  "CLK_SPI0"),
			}
		},
	},
	[1] = {
		.status = ENABLED,
		.name = "SPI1",
		.dev = {
			.name = "SPI1",
			.regs = {
				REGMAP("BASE_ADDR", (SPI1_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_SPI1_VECTNUM),
			},
			.pins = {
				PIN("CLK", GPIO_PORT_B, 4, 1),
				PIN("FSS", GPIO_PORT_B, 5, 1),
				PIN("RX", GPIO_PORT_B, 6, 1),
				PIN("TX", GPIO_PORT_B, 7, 1),
			},
			.clocks = {
				VAL("SPI",  "CLK_SPI1"),
			}
		},
	},
};

struct i2c_conf i2cs[] = {
	[0] = {
		.status = ENABLED,
		.name = "I2C0",
		.dev = {
			.name = "I2C0",
			.regs = {
				REGMAP("BASE_ADDR", (I2C_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_I2C_VECTNUM),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_C, 12, 1),
				PIN("SDA", GPIO_PORT_C, 13, 1),
			},
			.clocks = {
				VAL("", "CLK_I2C0"),
			}
		},
	},

};

struct led_conf leds[] = {
	[0] = {
		.name = "LED1",
		.port = VAL("", GPIO_PORT_A),
		.pin = VAL("", 12),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[1] = {
		.name = "LED2",
		.port = VAL("", GPIO_PORT_A),
		.pin = VAL("", 13),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[2] = {
		.name = "LED3",
		.port = VAL("", GPIO_PORT_A),
		.pin = VAL("", 14),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[3] = {
		.name = "LED4",
		.port = VAL("", GPIO_PORT_A),
		.pin = VAL("PIN", 15),
		.level = VAL("", GPIO_PIN_HIGH),
	},
};

struct pwm_conf pwms[] = {
	[0] = {
		.name = "PWM0",
		.channel = VAL("", 1),
		.dev = {
			.name = "PWM0",
			.regs = {
				REGMAP("BASE_ADDR", (TMR0_BASE), 0x100),
			},
			.pins = {
				PIN("OUT", GPIO_PORT_C, 7, 1),
			},
			.clocks = {
				VAL("",  "CLK_TMR0"),
			}
		},
	},
	[1] = {
		.name = "PWM1",
		.channel = VAL("", 2),
		.dev = {
			.name = "PWM1",
			.regs = {
				REGMAP("BASE_ADDR", (TMR1_BASE), 0x100),
			},
			.pins = {
				PIN("OUT", GPIO_PORT_A, 8, 2),
			},
			.clocks = {
				VAL("",  "CLK_TMR1"),
			}
		},
	},
	[2] = {
		.name = "PWM2",
		.channel = VAL("", 2),
		.dev = {
			.name = "PWM2",
			.regs = {
				REGMAP("BASE_ADDR", (TMR2_BASE), 0x100),
			},
			.pins = {
				PIN("OUT", GPIO_PORT_A, 2, 2),
			},
			.clocks = {
				VAL("",  "CLK_TMR2"),
			}
		},
	},
};

EXPORT_CONFIG(CLK(clks), GPIO(gpios), UART(uarts), LED(leds), SPI(spis), I2C(i2cs), PWM(pwms))
