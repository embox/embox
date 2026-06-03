#include <gen_board_conf.h>

#include <soc/niiet_k1921vg1t_chip.h>

#define HSECLK_VAL 27000000UL


struct clk_conf clks[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "RCU",
			.regs = {
				REGMAP("BASE", (RCU_BASE), 0x100),
			},
			.clocks = {
				VAL("HSECLK_VAL", HSECLK_VAL),
			}
		},
		.type = {
			VAL("SYSCLK_PLL", 1),
		},
	},
	[1] = {
		.status = ENABLED,
		.dev = {
			.name = "SIU",
			.regs = {
				REGMAP("BASE", (SIU_BASE), 0x100),
			},
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
				VAL("", PLIC_IRQ_GPIOA),
			},
			.clocks = {
				VAL("", "CLK_GPIOA"),
			}
		},
		.port_num = 8,
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
				VAL("", PLIC_IRQ_GPIOB),
			},
			.clocks = {
				VAL("", "CLK_GPIOB"),
			}
		},
		.port_num = 8,
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
				VAL("", PLIC_IRQ_GPIOC),
			},
			.clocks = {
				VAL("", "CLK_GPIOC"),
			}
		},
		.port_num = 8,
		.port_width = 16,
	},
	[3] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_D",
			.regs = {
				REGMAP("BASE", (GPIOD_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_GPIOD),
			},
			.clocks = {
				VAL("", "CLK_GPIOD"),
			}
		},
		.port_num = 8,
		.port_width = 16,
	},
	[4] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_E",
			.regs = {
				REGMAP("BASE", (GPIOE_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_GPIOE),
			},
			.clocks = {
				VAL("", "CLK_GPIOE"),
			}
		},
		.port_num = 8,
		.port_width = 16,
	},
	[5] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_F",
			.regs = {
				REGMAP("BASE", (GPIOF_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_GPIOF),
			},
			.clocks = {
				VAL("", "CLK_GPIOF"),
			}
		},
		.port_num = 8,
		.port_width = 16,
	},
	[6] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_G",
			.regs = {
				REGMAP("BASE", (GPIOG_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_GPIOG),
			},
			.clocks = {
				VAL("", "CLK_GPIOG"),
			}
		},
		.port_num = 8,
		.port_width = 16,
	},
	[7] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_H",
			.regs = {
				REGMAP("BASE", (GPIOH_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_IRQ_GPIOH),
			},
			.clocks = {
				VAL("", "CLK_GPIOH"),
			}
		},
		.port_num = 8,
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
				VAL("", PLIC_IRQ_UART0),
			},
			.pins = {
				PIN("TX", GPIO_PORT_B, 1, GPIO_MODE_ALT, 3),
				PIN("RX", GPIO_PORT_B, 0, GPIO_MODE_ALT, 3),
			},
			.clocks = {
				VAL("", "CLK_UART0"),
			}
		},
		.baudrate = 115200,
	},

};

struct led_conf leds[] = {
	[0] = {
		.status = ENABLED,
		.name = "LED1",
		.port = VAL("", GPIO_PORT_C),
		.pin = VAL("", 12),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[1] = {
		.status = ENABLED,	
		.name = "LED2",
		.port = VAL("", GPIO_PORT_C),
		.pin = VAL("", 13),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[2] = {
		.status = ENABLED,		
		.name = "LED3",
		.port = VAL("", GPIO_PORT_C),
		.pin = VAL("", 14),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[3] = {
		.status = ENABLED,		
		.name = "LED4",
		.port = VAL("", GPIO_PORT_C),
		.pin = VAL("PIN", 15),
		.level = VAL("", GPIO_PIN_HIGH),
	},
};

struct pwm_conf pwms[] = {
	[0] = {
		.name = "PWM0",
		.dev = {
			.name = "PWM0",
			.regs = {
				REGMAP("BASE_ADDR", (TMR0_BASE), 0x100),
			},
			.pins = {
				// PIN("OUT0", GPIO_PORT_A, 0, GPIO_MODE_ALT, 5),
				// PIN("OUT1", GPIO_PORT_A, 1, GPIO_MODE_ALT, 5),
				// PIN("OUT2", GPIO_PORT_A, 2, GPIO_MODE_ALT, 5),
				// PIN("OUT3", GPIO_PORT_A, 3, GPIO_MODE_ALT, 5),
			},
			.clocks = {
				VAL("",  "CLK_TMR0"),
			},
			.misc = {
				VAL("COMP_MASK",  0xFFFFFFFF),
				VAL("TMR_NAME", "TMR0"),
				VAL("TMR_IDX", 0),
			},
		},
	},
	[1] = {
		.name = "PWM1",
		.dev = {
			.name = "PWM1",
			.regs = {
				REGMAP("BASE_ADDR", (TMR1_BASE), 0x100),
			},
			.pins = {
				PIN("OUT0", GPIO_PORT_A, 4, GPIO_MODE_ALT, 5),
				PIN("OUT1", GPIO_PORT_A, 5, GPIO_MODE_ALT, 5),
				PIN("OUT2", GPIO_PORT_A, 6, GPIO_MODE_ALT, 5),
				PIN("OUT3", GPIO_PORT_A, 7, GPIO_MODE_ALT, 5),
			},
			.clocks = {
				VAL("",  "CLK_TMR1"),
			},
			.misc = {
				VAL("COMP_MASK",  0xFFFFFFFF),
				VAL("TMR_NAME", "TMR1"),
				VAL("TMR_IDX", 1),
			},
		},
	},
	[2] = {
		.name = "PWM2",
		.dev = {
			.name = "PWM2",
			.regs = {
				REGMAP("BASE_ADDR", (TMR2_BASE), 0x100),
			},
			.pins = {
				PIN("OUT0", GPIO_PORT_A, 8, GPIO_MODE_ALT, 5),
				PIN("OUT1", GPIO_PORT_A, 9, GPIO_MODE_ALT, 5),
				PIN("OUT2", GPIO_PORT_A, 10, GPIO_MODE_ALT, 5),
				PIN("OUT3", GPIO_PORT_A, 11, GPIO_MODE_ALT, 5),
			},
			.clocks = {
				VAL("",  "CLK_TMR2"),
			},
			.misc = {
				VAL("COMP_MASK",  0xFFFFFFFF),
				VAL("TMR_NAME", "TMR2"),
				VAL("TMR_IDX", 2),
			},
		},
	},
	[3] = {
		.name = "PWM3",
		.dev = {
			.name = "PWM3",
			.regs = {
				REGMAP("BASE_ADDR", (TMR3_BASE), 0x100),
			},
			.pins = {
				PIN("OUT0", GPIO_PORT_A, 12, GPIO_MODE_ALT, 5),
				PIN("OUT1", GPIO_PORT_A, 13, GPIO_MODE_ALT, 5),
				PIN("OUT2", GPIO_PORT_A, 14, GPIO_MODE_ALT, 5),
				PIN("OUT3", GPIO_PORT_A, 15, GPIO_MODE_ALT, 5),
			},
			.clocks = {
				VAL("",  "CLK_TMR3"),
			},
			.misc = {
				VAL("COMP_MASK",  0xFFFFFFFF),
				VAL("TMR_NAME", "TMR3"),
				VAL("TMR_IDX", 3),
			},
		},
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
				PIN("CLK", GPIO_PORT_A, 0, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 1, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 1, GPIO_MODE_OUT, -1),
				PIN("RX", GPIO_PORT_A, 2, GPIO_MODE_ALT, 1),
				PIN("TX", GPIO_PORT_A, 3, GPIO_MODE_ALT, 1),
			},
			.clocks = {
				VAL("SPI",  "CLK_SPI0"),
			}
		},
		.spi_devs[0] = {
			.status = ENABLED,
			.name = "SPI0_0",
			.bits_per_word = 8,
			.bus_num = 0,
			.idx     = 0,
			.dev = {
				.name = "SPI0_0",
				.pins = {
					PIN("FSS", GPIO_PORT_A, 1, GPIO_MODE_ALT, 1),
					// PIN("FSS", GPIO_PORT_B, 1, GPIO_MODE_OUT, -1),
				},
			},
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
				PIN("CLK", GPIO_PORT_A, 4, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 5, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 5, GPIO_MODE_OUT, -1),
				PIN("RX", GPIO_PORT_A, 6, GPIO_MODE_ALT, 1),
				PIN("TX", GPIO_PORT_A, 7, GPIO_MODE_ALT, 1),
			},
			.clocks = {
				VAL("SPI",  "CLK_SPI1"),
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
					PIN("FSS", GPIO_PORT_A, 5, GPIO_MODE_ALT, 1),
					// PIN("FSS", GPIO_PORT_B, 5, GPIO_MODE_OUT, -1),
				},
			},
		},
	},
	[2] = {
		.status = ENABLED,
		.name = "SPI2",
		.dev = {
			.name = "SPI2",
			.regs = {
				REGMAP("BASE_ADDR", (SPI2_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_SPI2_VECTNUM),
			},
			.pins = {
				PIN("CLK", GPIO_PORT_A, 8, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 9, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 9, GPIO_MODE_OUT, -1),
				PIN("RX", GPIO_PORT_A, 10, GPIO_MODE_ALT, 1),
				PIN("TX", GPIO_PORT_A, 11, GPIO_MODE_ALT, 1),
			},
			.clocks = {
				VAL("SPI",  "CLK_SPI2"),
			}
		},
		.spi_devs[0] = {
			.status = ENABLED,
			.name = "SPI2_0",
			.bits_per_word = 8,
			.bus_num = 1,
			.idx     = 0,
			.dev = {
				.name = "SPI2_0",
				.pins = {
					PIN("FSS", GPIO_PORT_A, 9, GPIO_MODE_ALT, 1),
					// PIN("FSS", GPIO_PORT_B, 5, GPIO_MODE_OUT, -1),
				},
			},
		},
	},
	[3] = {
		.status = ENABLED,
		.name = "SPI3",
		.dev = {
			.name = "SPI3",
			.regs = {
				REGMAP("BASE_ADDR", (SPI3_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_SPI3_VECTNUM),
			},
			.pins = {
				PIN("CLK", GPIO_PORT_A, 12, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 13, GPIO_MODE_ALT, 1),
				// PIN("FSS", GPIO_PORT_A, 13, GPIO_MODE_OUT, -1),
				PIN("RX", GPIO_PORT_A, 14, GPIO_MODE_ALT, 1),
				PIN("TX", GPIO_PORT_A, 15, GPIO_MODE_ALT, 1),
			},
			.clocks = {
				VAL("SPI",  "CLK_SPI3"),
			}
		},
		.spi_devs[0] = {
			.status = ENABLED,
			.name = "SPI3_0",
			.bits_per_word = 8,
			.bus_num = 1,
			.idx     = 0,
			.dev = {
				.name = "SPI3_0",
				.pins = {
					PIN("FSS", GPIO_PORT_A, 13, GPIO_MODE_ALT, 1),
					// PIN("FSS", GPIO_PORT_B, 13, GPIO_MODE_OUT, -1),
				},
			},
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
				REGMAP("BASE_ADDR", (I2C0_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_I2C0_VECTNUM),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_D, 12, GPIO_MODE_ALT, 1),
				PIN("SDA", GPIO_PORT_D, 13, GPIO_MODE_ALT, 1),
			},
			.clocks = {
				VAL("", "CLK_I2C0"),
			}
		},
	},
	[1] = {
		.status = ENABLED,
		.name = "I2C1",
		.dev = {
			.name = "I2C1",
			.regs = {
				REGMAP("BASE_ADDR", (I2C1_BASE), 0x100),
			},
			.irqs = {
				VAL("", PLIC_I2C1_VECTNUM),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_D, 1, GPIO_MODE_ALT, 1),
				PIN("SDA", GPIO_PORT_D, 0, GPIO_MODE_ALT, 1),
			},
			.clocks = {
				VAL("", "CLK_I2C1"),
			}
		},
	},

};

EXPORT_CONFIG(CLK(clks), GPIO(gpios), UART(uarts), LED(leds), PWM(pwms), SPI(spis), I2C(i2cs))
