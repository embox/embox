#include <gen_board_conf.h>
#include <stm32.h>
#include <stm32f4_chip.h>

#if 0
#if OPTION_GET(BOOLEAN, errata_spi_wrong_last_bit)
	/* One of possible workarounds, as suggested by
	 * STM32F40x and STM32F41x Errata sheet is to decrease
	 * APB clock speed. */
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;
#else
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
#endif
#endif

/**
  * @brief  Switch the PLL source from HSI to HSE, and select the PLL as SYSCLK
  *         source.
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
				VAL("SYSCLK_VAL",  168000000UL),
				VAL("HSECLK_VAL",  8000000UL),
				VAL("AHB_PRESCALER_VAL",  1),
				VAL("ERRATA_SPI_WRONG_LAST_BIT",  1),
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


struct uart_conf uarts[] = {
 	[1] = {
 		.status = ENABLED,
		.name = "USART1",
		.dev = {
			.name = "USART1",
			.irqs = {
				VAL("", 37),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 9, GPIO_MODE_AF | GPIO_MODE_OUT, AF7),
				PIN("RX", GPIO_PORT_A, 10, GPIO_MODE_AF, AF7),
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
			.irqs = {
				VAL("", 38),
			},
			.pins = {
				PIN("TX", GPIO_PORT_A, 2, GPIO_MODE_AF, AF7),
				PIN("RX", GPIO_PORT_A, 3, GPIO_MODE_AF, AF7),
			},
			.clocks = {
				VAL("UART", CLK_USART2),
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
				PIN("TX", GPIO_PORT_C, 6, GPIO_MODE_AF, AF8),
				PIN("RX", GPIO_PORT_C, 7, GPIO_MODE_AF, AF8),
			},
			.clocks = {
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
				PIN("SCK",  GPIO_PORT_B, 3, GPIO_MODE_AF, AF5),
				PIN("MISO", GPIO_PORT_B, 4, GPIO_MODE_AF, AF5),
				PIN("MOSI", GPIO_PORT_B, 5, GPIO_MODE_AF, AF5),
				PIN("CS",   GPIO_PORT_B, 2, GPIO_MODE_OUT, NOAF),
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
					PIN("CS",   GPIO_PORT_B, 2, GPIO_MODE_OUT, NOAF),
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
				PIN("SCK",  GPIO_PORT_B, 13, GPIO_MODE_AF, AF5),
				PIN("MISO", GPIO_PORT_B, 14, GPIO_MODE_AF, AF5),
				PIN("MOSI", GPIO_PORT_B, 15, GPIO_MODE_AF, AF5),
				PIN("CS",   GPIO_PORT_B, 12, GPIO_MODE_OUT, NOAF),
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
					PIN("CS",   GPIO_PORT_B, 12, GPIO_MODE_OUT, NOAF),
				},
			},
		},
	},
	[5] = {
		.status = DISABLED,
		.name = "SPI5",
		.dev = {
			.name = "SPI5",
			.pins = {
				PIN("SCK",  GPIO_PORT_F, 7, GPIO_MODE_AF, AF5),
				PIN("MISO", GPIO_PORT_F, 8, GPIO_MODE_AF, AF5),
				PIN("MOSI", GPIO_PORT_F, 9, GPIO_MODE_AF, AF5),
				PIN("CS",   GPIO_PORT_C, 1, GPIO_MODE_OUT, NOAF),
			},
			.clocks = {
				VAL("SPI",  CLK_SPI5),
			}
		},
		.spi_devs[0] = {
			.status = ENABLED,
			.name = "SPI5_0",
			.bits_per_word = 8,
			.bus_num = 5,
			.idx     = 0,
			.dev = {
				.name = "SPI5_0",
				.pins = {
					PIN("CS",   GPIO_PORT_C, 1, GPIO_MODE_OUT, NOAF),
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
				PIN("SCL", GPIO_PORT_B, 6, GPIO_MODE_AF, AF4),
				PIN("SDA", GPIO_PORT_B, 9, GPIO_MODE_AF, AF4),
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
				PIN("SCL", GPIO_PORT_B, 10, GPIO_MODE_AF, AF4),
				PIN("SDA", GPIO_PORT_B, 11, GPIO_MODE_AF, AF4),
			},
			.clocks = {
				VAL("I2C", CLK_I2C2),
			}
		},
	},

};

struct led_conf leds[] = {
	[0] = {
		.status = ENABLED,
		.name = "LED0",
		.port = VAL("", GPIO_PORT_G),
		.pin = VAL("", 13),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[1] = {
		.status = ENABLED,
		.name = "LED1",
		.port = VAL("", GPIO_PORT_G),
		.pin = VAL("", 14),
		.level = VAL("", GPIO_PIN_HIGH),
	},
};

EXPORT_CONFIG(CLK(clks), UART(uarts), SPI(spis), I2C(i2cs), LED(leds))
