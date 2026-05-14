#include <gen_board_conf.h>
#include <stm32.h>
#include <stm32f2_chip.h>


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 120000000
  *            HCLK(Hz)                       = 120000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 240
  *            PLL_P                          = 2
  *            PLL_Q                          = 5
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 3
  * @param  None
  * @retval None
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
				VAL("SYSCLK_VAL",  120000000UL),
				VAL("HSECLK_VAL",  8000000UL),
				VAL("AHB_PRESCALER_VAL",  1),
				VAL("APB1_PRESCALER_VAL", 4),
				VAL("APB2_PRESCALER_VAL", 2),
				VAL("PLL_M_VAL",  8),
				VAL("PLL_N_VAL",  240),
				VAL("PLL_P_VAL",  2),
				VAL("PLL_Q_VAL",  5),
				VAL("FLASH_LATENCY", 3),
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
			.regs = {
				REGMAP("BASE", (USART1_BASE), 0x100),
			},
			.irqs = {
				VAL("", USART1_IRQn),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 9, GPIO_MODE_ALT, AF7),
				PIN("RX", GPIO_PORT_A, 10, GPIO_MODE_ALT, AF7),
			},
			.clocks = {
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
			.regs = {
				REGMAP("BASE", (USART2_BASE), 0x100),
			},
			.irqs = {
				VAL("", USART2_IRQn),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 2, GPIO_MODE_ALT, AF7),
				PIN("RX", GPIO_PORT_A, 3, GPIO_MODE_ALT, AF7),
			},
			.clocks = {
				VAL("UART", CLK_USART2),
			}
		},
		.baudrate = 115200,
	},
	[3] = {
		.status = ENABLED,
		.name = "USART3",
		.dev = {
			.name = "USART3",
			.regs = {
				REGMAP("BASE", (USART3_BASE), 0x100),
			},
			.irqs = {
				VAL("", USART3_IRQn),
			},
			.pins = {
				PIN("TX", GPIO_PORT_D, 8, GPIO_MODE_ALT, AF7),
				PIN("RX", GPIO_PORT_D, 9, GPIO_MODE_ALT, AF7),
			},
			.clocks = {
				VAL("UART", CLK_USART3),
			}
		},
		.baudrate = 115200,
	},
};

struct led_conf leds[] = {
	[0] = {
		.name = "LED1",
		.port = VAL("", GPIO_PORT_B),
		.pin = VAL("", 0),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[1] = {
		.name = "LED2",
		.port = VAL("", GPIO_PORT_B),
		.pin = VAL("", 7),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[2] = {
		.name = "LED3",
		.port = VAL("", GPIO_PORT_B),
		.pin = VAL("", 14),
		.level = VAL("", GPIO_PIN_HIGH),
	},
};

struct spi_conf spis[] = {
	[1] = {
		.status = DISABLED,
		.name = "SPI1",
		.dev = {
			.name = "SPI1",
			.regs = {
				REGMAP("BASE", (SPI1_BASE), 0x100),
			},
			.pins = {
				PIN("SCK",  GPIO_PORT_A, 5, GPIO_MODE_ALT, AF5),
				PIN("MISO", GPIO_PORT_A, 6, GPIO_MODE_ALT, AF5),
				PIN("MOSI", GPIO_PORT_A, 7, GPIO_MODE_ALT, AF5),
				PIN("CS",   GPIO_PORT_D, 14, GPIO_MODE_OUT, NOAF),
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
					PIN("CS",   GPIO_PORT_D, 14, GPIO_MODE_ALT, NOAF),
				},
			},
		},
	},
};

struct i2c_conf i2cs[] = {
	[1] = {
		.status = ENABLED,
		.name = "I2C1",
		.dev = {
			.name = "I2C1",
			.regs = {
				REGMAP("BASE", (I2C1_BASE), 0x100),
			},
			.irqs = {
				VAL("EVENT", 31),
				VAL("ERROR", 32),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_B, 6, GPIO_MODE_ALT, AF4),
				PIN("SDA", GPIO_PORT_B, 9, GPIO_MODE_ALT, AF4),
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
			.regs = {
				REGMAP("BASE", (I2C2_BASE), 0x100),
			},
			.irqs = {
				VAL("EVENT", 33),
				VAL("ERROR", 34),
			},
			.pins = {
				PIN("SCL", GPIO_PORT_B, 10, GPIO_MODE_ALT, AF4),
				PIN("SDA", GPIO_PORT_B, 11, GPIO_MODE_ALT, AF4),
			},
			.clocks = {
				VAL("I2C", CLK_I2C2),
			}
		},
	},

};

/* Ethernet pins configuration ************************************************/
/*
        RMII_REF_CLK ----------------------> PA1
        RMII_MDIO -------------------------> PA2
        RMII_MDC --------------------------> PC1
        RMII_MII_CRS_DV -------------------> PA7
        RMII_MII_RXD0 ---------------------> PC4
        RMII_MII_RXD1 ---------------------> PC5
        RMII_MII_RXER ---------------------> PG2
        RMII_MII_TX_EN --------------------> PG11
        RMII_MII_TXD0 ---------------------> PG13
        RMII_MII_TXD1 ---------------------> PB13
*/
struct eth_conf eths[] = {
	[1] = {
		.status = ENABLED,
		.name = "ETH",
		.dev = {
			.name = "ETH",
			.regs = {
				REGMAP("BASE", (ETH_BASE), 0x100),
			},
			.irqs = {
				VAL("", ETH_IRQn),
			},
			.pins = {
				PIN("RMII_REF_CLK",    GPIO_PORT_A, 1, GPIO_MODE_ALT, AF11),
				PIN("RMII_MDIO",       GPIO_PORT_A, 2, GPIO_MODE_ALT, AF11),
				PIN("RMII_MDC",        GPIO_PORT_C, 1, GPIO_MODE_ALT, AF11),
				PIN("RMII_MII_CRS_DV", GPIO_PORT_A, 7, GPIO_MODE_ALT, AF11),
				PIN("RMII_MII_RXD0",   GPIO_PORT_C, 4, GPIO_MODE_ALT, AF11),
				PIN("RMII_MII_RXD1",   GPIO_PORT_C, 5, GPIO_MODE_ALT, AF11),
				PIN("RMII_MII_RXER",   GPIO_PORT_G, 2, GPIO_MODE_ALT, AF11),
				PIN("RMII_MII_TX_EN",  GPIO_PORT_G, 11, GPIO_MODE_ALT, AF11),
				PIN("RMII_MII_TXD0",   GPIO_PORT_G, 13, GPIO_MODE_ALT, AF11),
				PIN("RMII_MII_TXD1",   GPIO_PORT_B, 13, GPIO_MODE_ALT, AF11),
			},
			.clocks = {
				VAL("ETH", STM32_CLK_ENABLE(ETH)),
			},
			.misc = {
				VAL("PHY_ADDR", 0),
				VAL("MEDIA_TYPE", RMII),
			}
		},
	},
};

EXPORT_CONFIG(CLK(clks), GPIO(gpios),
				UART(uarts), LED(leds), SPI(spis), I2C(i2cs), ETH(eths))
