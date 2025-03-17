#include <gen_board_conf.h>

#define NS_PERIPHERAL_BASE          (0x40000000UL)

#define SECURE_PERIPHERAL_BASE      (0x50000000UL)


#define CPU_BUS_BASE            (PERIPHERAL_BASE + 0x00010000UL)

#define CPU_IDENTITY_BASE       (CPU_BUS_BASE + 0x0000F000UL)

#define APB_PPC3_BASE           (PERIPHERAL_BASE + 0x00030000UL)

#define IOCTR_BASE              (APB_PPC3_BASE + 0x00000000UL)
#define CLKCTR_BASE             (APB_PPC3_BASE + 0x00001000UL)
#define PWRCTR_BASE             (APB_PPC3_BASE + 0x00002000UL)

/* APB_PPC0 */
#define APB_PPC0_BASE           (PERIPHERAL_BASE + 0x00100000UL)

#define UART0_BASE              (APB_PPC0_BASE + 0x00000000UL)
#define UART1_BASE              (APB_PPC0_BASE + 0x00001000UL)
#define UART2_BASE              (APB_PPC0_BASE + 0x00002000UL)
#define UART3_BASE              (APB_PPC0_BASE + 0x00003000UL)

#define SPI0_BASE               (APB_PPC0_BASE + 0x00004000UL)
#define SPI1_BASE               (APB_PPC0_BASE + 0x00005000UL)
#define SPI2_BASE               (APB_PPC0_BASE + 0x00006000UL)

#define I2C0_BASE               (APB_PPC0_BASE + 0x00007000UL)
#define I2C1_BASE               (APB_PPC0_BASE + 0x00008000UL)

#define I2S_BASE                (APB_PPC0_BASE + 0x00009000UL)

#define GPIO0_BASE              (APB_PPC0_BASE + 0x0000A000UL)
#define GPIO1_BASE              (APB_PPC0_BASE + 0x0000B000UL)
#define GPIO2_BASE              (APB_PPC0_BASE + 0x0000C000UL)
#define GPIO3_BASE              (APB_PPC0_BASE + 0x0000D000UL)

#define IRQ_NUM_UART0           32
#define IRQ_NUM_UART1           33
#define IRQ_NUM_UART2           34
#define IRQ_NUM_UART3           35

#define IRQ_NUM_SPI0            36
#define IRQ_NUM_SPI1            37
#define IRQ_NUM_SPI2            38

#define IRQ_NUM_I2C0            39
#define IRQ_NUM_I2C1            40

#define IRQ_NUM_I2S             41

#define IRQ_NUM_GPIO0_S         42
#define IRQ_NUM_GPIO1_S         43
#define IRQ_NUM_GPIO2_S         44
#define IRQ_NUM_GPIO3_S         45


#define AF(x)                   (x)

#define PERIPHERAL_BASE          NS_PERIPHERAL_BASE

struct clk_conf clks[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "CLKCTRL",
			.regs = {
				REGMAP("BASE", (CLKCTR_BASE), 0x100),
			},
			.clocks = {
				VAL("XTI_FREQ", 24000000UL),
				VAL("FCLK_FREQ", 144000000UL),
				VAL("SYSCLK_FREQ", 48000000UL),
				VAL("GNSS_FREQ", 72000000UL),
				VAL("QSPI_FREQ", 24000000UL),
				VAL("HFI_FREQ", 15100000UL),
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
				REGMAP("BASE", (GPIO0_BASE), 0x100),
			},
			.irqs = {
				VAL("", IRQ_NUM_GPIO0_S),
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
				VAL("", IRQ_NUM_GPIO1_S),
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
				VAL("", IRQ_NUM_GPIO1_S),
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
				VAL("", IRQ_NUM_GPIO3_S),
			},
			.clocks = {
				VAL("", "CLK_GPIO3"),
			}
		},
		.port_num = 4,
		.port_width = 16,
	},
	[4] = {
		.status = ENABLED,
		.dev = {
			.name = "IOCTR",
			.regs = {
				REGMAP("BASE", (IOCTR_BASE), 0x100),
			},

			.clocks = {
				VAL("", "CLK_IOCTR"),
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
				VAL("", IRQ_NUM_UART0),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 0, AF(4)),
				PIN("RX", GPIO_PORT_A, 1, AF(4)),
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
				VAL("", IRQ_NUM_UART1),
			},
			.pins = {
				PIN("TX", GPIO_PORT_D, 0, AF(4)),
				PIN("RX", GPIO_PORT_D, 1, AF(4)),
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
				VAL("", IRQ_NUM_UART2),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 5, AF(4)),
				PIN("RX", GPIO_PORT_A, 4, AF(4)),
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
				VAL("", IRQ_NUM_UART3),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 7, AF(4)),
				PIN("RX", GPIO_PORT_A, 6, AF(4)),
			},
			.clocks = {
				VAL("", "CLK_UART3"),
			}
		},
		.baudrate = 115200,
	},
};

struct spi_conf spis[] = {
	[0] = {
		.status = DISABLED,
		.name = "SPI0",
		.dev = {
			.name = "SPI0",
			.regs = {
				REGMAP("BASE_ADDR", (SPI0_BASE), 0x100),
			},
			.irqs = {
				VAL("", IRQ_NUM_SPI0),
			},
			.pins = {
				PIN("MOSI", GPIO_PORT_A, 3, AF(3)), /* AF3*/
				PIN("MISO", GPIO_PORT_A, 4, AF(3)), /* AF3*/
				PIN("SCK", GPIO_PORT_A, 2, AF(3)),  /* AF3*/
				PIN("SS", GPIO_PORT_A, 5, AF(3)),   /* AF3*/

				//PIN("MOSI", GPIO_PORT_C, 0, AF(3)), /* AF3*/
				//PIN("MISO", GPIO_PORT_C, 1, AF(3)), /* AF3*/
				//PIN("SCK", GPIO_PORT_C, 2, AF(3)),  /* AF3*/
				//PIN("SS", GPIO_PORT_C, 3, AF(3)),   /* AF3*/
			},
			.clocks = {
				VAL("SPI",  "CLK_SPI0"),
			}
		},
	},
	[1] = {
		.status = DISABLED,
		.name = "SPI1",
		.dev = {
			.name = "SPI1",
			.regs = {
				REGMAP("BASE_ADDR", (SPI1_BASE), 0x100),
			},
			.irqs = {
				VAL("", IRQ_NUM_SPI1),
			},
			.pins = {
				PIN("MOSI", GPIO_PORT_C, 4, AF(3)), /* AF3*/
				PIN("MISO", GPIO_PORT_C, 5, AF(3)), /* AF3*/
				PIN("SCK", GPIO_PORT_B, 11, AF(3)),  /* AF3*/
				PIN("SS", GPIO_PORT_C, 14, AF(3)),   /* AF3*/
			},
			.clocks = {
				VAL("SPI",  "CLK_SPI1"),
			}
		},
	},
	[2] = {
		.status = DISABLED,
		.name = "SPI2",
		.dev = {
			.name = "SPI2",
			.regs = {
				REGMAP("BASE_ADDR", (SPI2_BASE), 0x100),
			},
			.irqs = {
				VAL("", IRQ_NUM_SPI2),
			},
			.pins = {
				PIN("MOSI", GPIO_PORT_C, 1, AF(6)), /* AF6*/
				PIN("MISO", GPIO_PORT_C, 2, AF(6)), /* AF6*/
				PIN("SCK", GPIO_PORT_C, 0, AF(6)),  /* AF6*/
				PIN("SS", GPIO_PORT_C, 53, AF(6)),   /* AF6*/
			},
			.clocks = {
				VAL("SPI",  "CLK_SPI2"),
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
				REGMAP("BASE_ADDR", (I2C0_BASE), 0x100),
			},
			.irqs = {
				VAL("", IRQ_NUM_I2C0),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_A, 7, AF(2)), /* AF2 */
				PIN("SDA", GPIO_PORT_A, 6, AF(2)), /* AF2 */

				//PIN("SCL", GPIO_PORT_C, 5, AF(2)), /* AF2 */
				//PIN("SDA", GPIO_PORT_C, 4, AF(2)), /* AF2 */

				//PIN("SCL", GPIO_PORT_D, 3, AF(2)), /* AF2 */
				//PIN("SDA", GPIO_PORT_D, 2, AF(2)), /* AF2 */
			},
			.clocks = {
				VAL("", "CLK_I2C0"),
			}
		},
	},
	[0] = {
		.status = ENABLED,
		.name = "I2C1",
		.dev = {
			.name = "I2C1",
			.regs = {
				REGMAP("BASE_ADDR", (I2C1_BASE), 0x100),
			},
			.irqs = {
				VAL("", IRQ_NUM_I2C1),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_B, 10, AF(2)),  /* AF2 */
				PIN("SDA", GPIO_PORT_B, 9, AF(2)),   /* AF2 */
				//PIN("SCL", GPIO_PORT_D, 5, AF(2)),  /* AF2 */
				//PIN("SDA", GPIO_PORT_D, 4, AF(2)),   /* AF2 */

			},
			.clocks = {
				VAL("", "CLK_I2C0"),
			}
		},
	},

};

/* Using XP6 pins (pin33=PA6 & pin33=PC13) not real LEDs */
struct led_conf leds[] = {
	[0] = {
		.name = "LED1",
		.port = VAL("", GPIO_PORT_A),
		.pin = VAL("", 6),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[1] = {
		.name = "LED2",
		.port = VAL("", GPIO_PORT_C),
		.pin = VAL("", 13),
		.level = VAL("", GPIO_PIN_HIGH),
	},
};

EXPORT_CONFIG(CLK(clks), GPIO(gpios), UART(uarts),  LED(leds));
