#include <gen_board_conf.h>

#define RCU_BASE      (0x3000E000UL)
#define FLASH_BASE    (0x3000D000UL)
#define PMUSYS_BASE   (0x3000F000UL)

#define GPIOA_BASE              0x28000000UL
#define GPIOB_BASE              0x28001000UL
#define GPIOC_BASE              0x28002000UL

#define UART0_BASE              0x30006000UL
#define UART1_BASE              0x30007000UL
#define UART2_BASE              0x30008000UL
#define UART3_BASE              0x30009000UL
#define UART4_BASE              0x3000A000UL


#define PLIC_NUM_VECTORS 32
#define PLIC_WDT_VECTNUM        1
#define PLIC_CAN0_VECTNUM       2
#define PLIC_CAN1_VECTNUM       3
#define PLIC_USB_VECTNUM        4
#define PLIC_GPIO_VECTNUM       5
#define PLIC_TMR32_VECTNUM      6
#define PLIC_TMR0_VECTNUM       7
#define PLIC_TMR1_VECTNUM       8
#define PLIC_TMR2_VECTNUM       9
#define PLIC_QSPI0_VECTNUM      10
#define PLIC_QSPI1_VECTNUM      11
#define PLIC_QSPI2_VECTNUM      12
#define PLIC_DMA0_VECTNUM       13
#define PLIC_DMA1_VECTNUM       14
#define PLIC_DMA2_VECTNUM       15
#define PLIC_DMA3_VECTNUM       16
#define PLIC_DMA4_VECTNUM       17
#define PLIC_DMA5_VECTNUM       18
#define PLIC_DMA6_VECTNUM       19
#define PLIC_DMA7_VECTNUM       20
#define PLIC_I2C_VECTNUM        21
#define PLIC_UART0_VECTNUM      22
#define PLIC_UART1_VECTNUM      23
#define PLIC_UART2_VECTNUM      24
#define PLIC_UART3_VECTNUM      25
#define PLIC_UART4_VECTNUM      26
#define PLIC_CRYPTOHASH_VECTNUM 27
#define PLIC_TRNG_VECTNUM       28
#define PLIC_ADC_SAR_VECTNUM    29
#define PLIC_CMP_VECTNUM        30
#define PLIC_PMURTC_VECTNUM     31

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
		.name = "USART0",
		.dev = {
			.name = "USART0",
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
				//PIN("TX", GPIO_PORT_A, 1, 1),
				//PIN("RX", GPIO_PORT_A, 0, 1),
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
				//PIN("TX", GPIO_PORT_A, 1, 1),
				//PIN("RX", GPIO_PORT_A, 0, 1),
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
				//PIN("TX", GPIO_PORT_A, 1, 1),
				//PIN("RX", GPIO_PORT_A, 0, 1),
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
				//PIN("TX", GPIO_PORT_A, 1, 1),
				//PIN("RX", GPIO_PORT_A, 0, 1),
			},
			.clocks = {
				VAL("", "CLK_UART4"),
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
				// PIN("SCK",  PA, PIN_5, AF5),
				// PIN("MISO", PA, PIN_6, AF5),
				// PIN("MOSI", PA, PIN_7, AF5),
				// PIN("CS",   PD, PIN_14, NOAF),
			},
			.clocks = {
				// VAL("SPI",  "CLK_SPI1"),
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
				VAL("", PLIC_I2C_VECTNUM),
			},
			.pins = {
				// PIN("SCL", GPIO_PORT_B, PIN_6, AF4),
				// PIN("SDA", GPIO_PORT_B, PIN_9, AF4),
			},
			.clocks = {
				//VAL("", "CLK_I2C1"),
			}
		},
	},

};


EXPORT_CONFIG(CLK(clks), GPIO(gpios), UART(uarts))
