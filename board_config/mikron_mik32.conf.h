#include <gen_board_conf.h>

#include <mikron_mik32_chip.h>

struct clk_conf clks[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "PM",
			.regs = {
				REGMAP("BASE", (PM_BASE_ADDRESS), 0x100),
			},
			.clocks = {
				VAL("CLK_VAL", 32000000UL),
			}
		},
		.type = {
			VAL("SYSCLK_PLL", 1),
		},
	},
	[1] = {
		.status = ENABLED,
		.dev = {
			.name = "WU",
			.regs = {
				REGMAP("BASE", (WU_BASE_ADDRESS), 0x100),
			},
			.clocks = {
				VAL("HSECLK_VAL", 3200000UL),
			},
		},
	},
};

struct gpio_conf gpios[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_0",
			.regs = {
				REGMAP("BASE", (GPIO_0_BASE_ADDRESS), 0x100),
			},
			.irqs = {
				VAL("", EPIC_GPIO_IRQ_INDEX),
			},
			.clocks = {
				VAL("", "CLK_GPIO0"),
			}
		},
		.port_num = 3,
		.port_width = 16,
	},
	[1] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_1",
			.regs = {
				REGMAP("BASE", (GPIO_1_BASE_ADDRESS), 0x100),
			},
			.irqs = {
				VAL("", EPIC_GPIO_IRQ_INDEX),
			},
			.clocks = {
				VAL("", "CLK_GPIO0"),
			}
		},
		.port_num = 3,
		.port_width = 16,
	},
	[2] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_2",
			.regs = {
				REGMAP("BASE", (GPIO_2_BASE_ADDRESS), 0x100),
			},
			.irqs = {
				VAL("", EPIC_GPIO_IRQ_INDEX),
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
				REGMAP("BASE_ADDR", (UART_0_BASE_ADDRESS), 0x100),
			},
			.irqs = {
				VAL("", EPIC_UART_0_INDEX),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 5, 1),
				PIN("RX", GPIO_PORT_A, 6, 1),
			},
			.clocks = {
				VAL("", "CLK_UART0"),
			}
		},
		.baudrate = 115200,
	},
	[1] = {
		.status = ENABLED,
		.name = "UART1",
		.dev = {
			.name = "UART1",
			.regs = {
				REGMAP("BASE_ADDR", (UART_1_BASE_ADDRESS), 0x100),
			},
			.irqs = {
				VAL("", EPIC_UART_1_INDEX),
			},
			.pins = {
				PIN("TX", GPIO_PORT_B, 8, 1),
				PIN("RX", GPIO_PORT_B, 9, 1),
			},
			.clocks = {
				VAL("", "CLK_UART1"),
			}
		},
		.baudrate = 115200,
	},

};

struct led_conf leds[] = {
	[0] = {
		.name = "LED_GREEN",
		.port = VAL("", GPIO_PORT_A),
		.pin = VAL("", 9),
		.level = VAL("", GPIO_PIN_HIGH), // or GPIO_PIN_LOW ?
	},
	[1] = {
		.name = "LED_RED",
		.port = VAL("", GPIO_PORT_A),
		.pin = VAL("", 10),
		.level = VAL("", GPIO_PIN_HIGH), // or GPIO_PIN_LOW ?
	},
};

EXPORT_CONFIG(CLK(clks), GPIO(gpios), UART(uarts), LED(leds))
