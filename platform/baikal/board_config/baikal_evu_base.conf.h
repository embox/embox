#include <gen_board_conf.h>

#include <be_u1000_chip.h>


/*
 * Clock configuration:
 *
 * PLL = 200MHz
 * CCLK = 100MHz
 * PCLK0 = 100MHz
 * PCLK1 = 100MHz
 * PCLK2 = 100MHz
 * HCLK = 50MHz
*/

struct clk_conf clks[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "CRU",
			.regs = {
				REGMAP("BASE", (CRU_BASE), 0x100),
			},
			.clocks = {
				VAL("HCLK_FREQ", 5000000UL),
			}
		},
		.type = {
			VAL("PLL", 1),
		},
	},
};

struct gpio_conf gpios[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_A",
			.regs = {
				REGMAP("BASE", (GPIO0_BASE), 0x100),
			},
			.irqs = {
				VAL("", CLIC_GPIO0_IRQn),
			},
			.clocks = {
				VAL("", "CLK_GPIO0"),
			}
		},
		.port_num = 4,
		.port_width = 16,
	},
	[1] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_B",
			.regs = {
				REGMAP("BASE", (GPIO1_BASE), 0x100),
			},
			.irqs = {
				VAL("", CLIC_GPIO1_IRQn),
			},
			.clocks = {
				VAL("", "CLK_GPIO1"),
			}
		},
		.port_num = 4,
		.port_width = 16,
	},
	[2] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_C",
			.regs = {
				REGMAP("BASE", (GPIO2_BASE), 0x100),
			},
			.irqs = {
				VAL("", CLIC_GPIO2_IRQn),
			},
			.clocks = {
				VAL("", "CLK_GPIO2"),
			}
		},
		.port_num = 4,
		.port_width = 16,
	},
	[3] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_D",
			.regs = {
				REGMAP("BASE", (GPIO3_BASE), 0x100),
			},
			.irqs = {
				VAL("", CLIC_GPIO3_IRQn),
			},
			.clocks = {
				VAL("", "CLK_GPIO3"),
			}
		},
		.port_num = 4,
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
				VAL("", CLIC_UART0_IRQn),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 6, 1),
				PIN("RX", GPIO_PORT_A, 7, 1),
			},
			.clocks = {
				VAL("", "CLK_UART0"),
			}
		},
		.baudrate = 115200,
	},

};

/* Using XP6 pins (pin33=PA6 & pin33=PC13) not real LEDs */
struct led_conf leds[] = {
	[0] = {
		.name = "LED1",
		.port = VAL("", GPIO_PORT_B),
		.pin = VAL("", 10),
		.level = VAL("", GPIO_PIN_HIGH),
	},
};

EXPORT_CONFIG(CLK(clks), GPIO(gpios), UART(uarts),  LED(leds));
