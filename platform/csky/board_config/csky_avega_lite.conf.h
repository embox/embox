#include <gen_board_conf.h>
#include <stm32.h>
#include <stm32f4_chip.h>

/**
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  */
struct clk_conf clks[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "RCC",
			.regs = {
				REGMAP("BASE", (RCC_BASE), 0x100),
			},
			.clocks = {
				VAL("SYSCLK_VAL", 16800000UL),
				VAL("HSECLK_VAL",  8000000UL),
				VAL("AHB_PRESCALER_VAL",  1),
				VAL("APB1_PRESCALER_VAL", 4),
				VAL("APB2_PRESCALER_VAL", 2),
				VAL("PLL_M_VAL",  8),
				VAL("PLL_N_VAL",  336),
				VAL("PLL_P_VAL",  2),
				VAL("PLL_Q_VAL",  7),
				VAL("FLASH_LATENCY", 5),
			}
		},
		.type = {
			VAL("PLL", 1),
			VAL("HSE", 1),
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
			.irqs = {
				VAL("", 37),
			},
			.pins = {
				PIN("TX", PB, PIN_6, AF7),
				PIN("RX", PB, PIN_7, AF7),
			},
			.clocks = {
				VAL("TX",   CLK_GPIOB),
				VAL("RX",   CLK_GPIOB),
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
	[3] = {
		.status = DISABLED,
		.name = "USART3",
		.dev = {
			.name = "USART3",
			.irqs = {
				VAL("", 39),
			},
			.pins = {
				PIN("TX", PC, PIN_10, AF7),
				PIN("RX", PC, PIN_11, AF7),
			},
			.clocks = {
				VAL("TX",   CLK_GPIOC),
				VAL("RX",   CLK_GPIOC),
				VAL("UART", CLK_USART3),
			}
		},
		.baudrate = 115200,
	},
	[4] = {
		.status = DISABLED,
		.name = "UART4",
		.dev = {
			.name = "UART4",
			.irqs = {
				VAL("", 52),
			},
			.pins = {
				PIN("TX", PA, PIN_0, AF8),
				PIN("RX", PA, PIN_1, AF8),
			},
			.clocks = {
				VAL("TX",   CLK_GPIOA),
				VAL("RX",   CLK_GPIOA),
				VAL("UART", CLK_UART4),
			}
		},
		.baudrate = 115200,
	},
	[5] = {
		.status = DISABLED,
		.name = "UART5",
		.dev = {
			.name = "UART5",
			.irqs = {
				VAL("", 53),
			},
			.pins = {
				PIN("TX", PC, PIN_12, AF8),
				PIN("RX", PD, PIN_2, AF8),
			},
			.clocks = {
				VAL("TX",   CLK_GPIOC),
				VAL("RX",   CLK_GPIOD),
				VAL("UART", CLK_UART5),
			}
		},
		.baudrate = 115200,
	},
	[6] = {
		.status = DISABLED,
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
				PIN("SCK", GPIO_PORT_A, 5, AF5),
				PIN("MISO", GPIO_PORT_A, 6, AF5),
				PIN("MOSI", GPIO_PORT_A, 7, AF5),
				PIN("CS",   GPIO_PORT_C, 13, NOAF),
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
					PIN("CS",   GPIO_PORT_C, 13, NOAF),
				},
			},
		},
	},
	[2] = {
		.status = DISABLED,
		.name = "SPI2",
		.dev = {
			.name = "SPI2",
			.pins = {
				PIN("SCK",  GPIO_PORT_B, 13, AF5),
				PIN("MISO", GPIO_PORT_B, 14, AF5),
				PIN("MOSI", GPIO_PORT_B, 15, AF5),
				PIN("CS",   GPIO_PORT_C, 14, NOAF),
			},
			.clocks = {
				VAL("SPI",  CLK_SPI2),
			}
		},
		.spi_devs[0] = {
			.status = ENABLED,
			.name = "SPI2_0",
			.bits_per_word = 8,
			.bus_num = 2,
			.idx     = 0,
			.dev = {
				.name = "SPI2_0",
				.pins = {
					PIN("CS",   GPIO_PORT_C, 14, NOAF),
				},
			},
		},
	},
};

struct i2c_conf i2cs[] = {
	[1] = {
		.status = DISABLED,
		.name = "I2C1",
		.dev = {
			.name = "I2C1",
			.irqs = {
				VAL("EVENT", 31),
				VAL("ERROR", 32),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_B, PIN_8, AF4),
				PIN("SDA", GPIO_PORT_B, PIN_9, AF4),
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
			.irqs = {
				VAL("EVENT", 33),
				VAL("ERROR", 34),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_F, PIN_0, AF4),
				PIN("SDA", GPIO_PORT_F, PIN_1, AF4),
			},
			.clocks = {
				VAL("I2C", CLK_I2C2),
			}
		},
	},
	[3] = {
		.status = DISABLED,
		.name = "I2C3",
		.dev = {
			.name = "I2C3",
			.irqs = {
				VAL("EVENT", 72),
				VAL("ERROR", 73),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_H, PIN_4, AF4),
				PIN("SDA", GPIO_PORT_H, PIN_5, AF4),
			},
			.clocks = {
				VAL("I2C", CLK_I2C3),
			}
		},
	},

};

EXPORT_CONFIG(CLK(clks), GPIO(gpios), UART(uarts), SPI(spis), I2C(i2cs))
