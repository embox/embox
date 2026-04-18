#include <gen_board_conf.h>

#include <soc/niiet_k1921vg1t_chip.h>

//#define HSICLK_VAL 4000000UL
#define HSECLK_VAL 27000000UL
//#define EXTCLK_VAL 10000000UL

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
				PIN("TX", GPIO_PORT_B, 1, GPIO_MODE_AF, 3),
				PIN("RX", GPIO_PORT_B, 0, GPIO_MODE_AF, 3),
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

EXPORT_CONFIG(CLK(clks), GPIO(gpios), UART(uarts), LED(leds))
