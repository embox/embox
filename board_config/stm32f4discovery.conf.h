#include <gen_board_conf.h>
#include <stm32.h>
#include <stm32f4_chip.h>

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
	[2] = {
		.status = DISABLED,
		.name = "USART2",
		.dev = {
			.name = "USART2",
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
	[6] = {
		.status = ENABLED,
		.name = "USART6",
		.dev = {
			.name = "USART6",
			.irqs = {
				VAL("", USART6_IRQn),
			},
			.pins = {
				PIN("TX", GPIO_PORT_C, 6, GPIO_MODE_ALT, AF8),
				PIN("RX", GPIO_PORT_C, 7, GPIO_MODE_ALT, AF8),
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
				PIN("SCK",  GPIO_PORT_B, 3, GPIO_MODE_ALT, AF5),
				PIN("MISO", GPIO_PORT_B, 4, GPIO_MODE_ALT, AF5),
				PIN("MOSI", GPIO_PORT_B, 5, GPIO_MODE_ALT, AF5),
				PIN("CS",   GPIO_PORT_B, 2, GPIO_MODE_OUT, NOAF),
			},
			.clocks = {
				VAL("SPI",  CLK_SPI1),
			},
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
				PIN("SCK",  GPIO_PORT_B, 13, GPIO_MODE_ALT, AF5),
				PIN("MISO", GPIO_PORT_B, 14, GPIO_MODE_ALT, AF5),
				PIN("MOSI", GPIO_PORT_B, 15, GPIO_MODE_ALT, AF5),
				PIN("CS",   GPIO_PORT_B, 12, GPIO_MODE_OUT, NOAF),
			},
			.clocks = {
				VAL("SPI",  CLK_SPI2),
			},
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

struct pwm_conf pwms[] = {
	[0] = {
		.name = "PWM0",
		.channel = VAL("CHANNEL_TIM", CHANNEL_TIM1),
		.instance = VAL("INSTANCE", TIM4),
		.servo_low = VAL("LOW", 200),
		.servo_high = VAL("HIGH", 1350),
		.dev = {
			.name = "PWM0",
			.pins = {
				PIN("TIM",  GPIO_PORT_B, 6, GPIO_MODE_ALT | GPIO_MODE_OUT | GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP, AF2),
			},
			.clocks = {
				//VAL("GPIO",  CLK_GPIOB),
				VAL("TIM",  CLK_TIM4),
			}
		},
	},
};

struct led_conf leds[] = {
	[0] = {
		.status = ENABLED,
		.name = "LD4",
		.port = VAL("", GPIO_PORT_D),
		.pin = VAL("", 12),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[1] = {
		.status = ENABLED,
		.name = "LD3",
		.port = VAL("", GPIO_PORT_D),
		.pin = VAL("", 13),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[2] = {
		.status = ENABLED,
		.name = "LD5",
		.port = VAL("", GPIO_PORT_D),
		.pin = VAL("", 14),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[3] = {
		.status = ENABLED,
		.name = "LD6",
		.port = VAL("", GPIO_PORT_D),
		.pin = VAL("", 15),
		.level = VAL("", GPIO_PIN_HIGH),
	},
};

/* Ethernet pins configuration ************************************************/
/*
 ETH_MDIO --------------> PA2
 ETH_MDC ---------------> PC1
 ETH_RMII_REF_CLK-------> PA1
 ETH_RMII_CRS_DV -------> PA7
 ETH_MII_RX_ER   -------> PB10
 ETH_RMII_RXD0   -------> PC4
 ETH_RMII_RXD1   -------> PC5
 ETH_RMII_TX_EN  -------> PB11
 ETH_RMII_TXD0   -------> PB12
 ETH_RMII_TXD1   -------> PB13
 ETH_RST_PIN     -------> PE2
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
				PIN("RMII_MII_RXER",   GPIO_PORT_B, 10, GPIO_MODE_ALT, AF11),
				PIN("RMII_MII_TX_EN",  GPIO_PORT_B, 11, GPIO_MODE_ALT, AF11),
				PIN("RMII_MII_TXD0",   GPIO_PORT_B, 12, GPIO_MODE_ALT, AF11),
				PIN("RMII_MII_TXD1",   GPIO_PORT_B, 13, GPIO_MODE_ALT, AF11),
				PIN("PHY_RESET",   GPIO_PORT_E, 2, GPIO_MODE_OUT, -1),
			},
			.clocks = {
				VAL("ETH", STM32_CLK_ENABLE(ETH)),
			},
			.misc = {
				VAL("PHY_ADDR", 0x01),
				VAL("MEDIA_TYPE", RMII),
			}
		},
	},
};

struct usb_conf usbs[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "USB_OTG",
			.regs = {
				REGMAP("BASE", (USB_OTG_FS_PERIPH_BASE), 0x100),
			},
			.irqs = {
				VAL("", 67), /* OTG_FS_IRQn */
			},
			.pins = {
				PIN("VBUS", GPIO_PORT_A,  9, GPIO_MODE_INPUT, 0),
				PIN("DM",  GPIO_PORT_A,  11, GPIO_MODE_ALT, 10),
				PIN("DP",  GPIO_PORT_A,  12, GPIO_MODE_ALT, 10),
				PIN("ID", GPIO_PORT_A, 10, GPIO_MODE_ALT | GPIO_MODE_IN_PULL_UP | GPIO_MODE_OUT_OPEN_DRAIN, 10),
			},
			.clocks = {
				VAL("USB", STM32_CLK_ENABLE(USB_OTG_FS)),			
			},
			.misc = {
				VAL("TYPE_FS", 1),
				VAL("PHY_ITFACE", PCD_PHY_EMBEDDED),
				VAL("SPEED", PCD_SPEED_FULL),
				VAL("EP_MAX", 4),
				VAL("EP_MAX_SIZE", 64),
			},
		},
	},
};

EXPORT_CONFIG(CLK(clks), UART(uarts), SPI(spis), I2C(i2cs), PWM(pwms), LED(leds), ETH(eths), USB(usbs))
