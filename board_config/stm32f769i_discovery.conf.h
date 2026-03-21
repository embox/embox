#include <gen_board_conf.h>
#include <stm32.h>

#include <stm32f7_chip.h>

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            PLL_R                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
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
				VAL("SYSCLK_VAL", 216000000UL),
				VAL("HSECLK_VAL",  25000000UL),
				VAL("AHB_PRESCALER_VAL",  1),
				VAL("APB1_PRESCALER_VAL", 4),
				VAL("APB2_PRESCALER_VAL", 2),
				VAL("PLL_M_VAL",  25),
				VAL("PLL_N_VAL",  432),
				VAL("PLL_P_VAL",  2),
				VAL("PLL_Q_VAL",  9),
				VAL("FLASH_LATENCY", 7),
			}
		},
		.type = {
			VAL("PLL", 1),
			VAL("HSE", 1),
			/* activate the OverDrive to reach the 216 Mhz Frequency */
			VAL("OVERDRIVE", 1),
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
		.port_num = 11,
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
				VAL("", 0),
			},
			.clocks = {
				VAL("",   CLK_GPIOB),
			}
		},
		.port_num = 11,
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
				VAL("", 0),
			},
			.clocks = {
				VAL("",   CLK_GPIOC),
			}
		},
		.port_num = 11,
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
				VAL("", 0),
			},
			.clocks = {
				VAL("",   CLK_GPIOD),
			}
		},
		.port_num = 11,
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
				VAL("", 0),
			},
			.clocks = {
				VAL("",   CLK_GPIOE),
			}
		},
		.port_num = 11,
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
				VAL("", 0),
			},
			.clocks = {
				VAL("",   CLK_GPIOF),
			}
		},
		.port_num = 11,
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
				VAL("", 0),
			},
			.clocks = {
				VAL("",   CLK_GPIOG),
			}
		},
		.port_num = 11,
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
				PIN("TX", GPIO_PORT_A, 9, AF7),
				PIN("RX", GPIO_PORT_A, 10, AF7),
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
				PIN("TX", GPIO_PORT_A, 2, AF7),
				PIN("RX", GPIO_PORT_A, 3, AF7),
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
				VAL("", 71),
			},
			.pins = {
				PIN("TX", GPIO_PORT_C, 6, AF8),
				PIN("RX", GPIO_PORT_C, 7, AF8),
			},
			.clocks = {
				VAL("UART", CLK_USART6),
			}
		},
		.baudrate = 115200,
	},
};

struct led_conf leds[] = {
	[0] = {
		.status = ENABLED,
		.name = "LD1",
		.port = VAL("", GPIO_PORT_J),
		.pin = VAL("", 13),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[1] = {
		.status = ENABLED,
		.name = "LD2",
		.port = VAL("", GPIO_PORT_J),
		.pin = VAL("", 5),
		.level = VAL("", GPIO_PIN_HIGH),
	},
};

struct mmc_conf mmcs[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "SDIO1",
			.regs = {
				REGMAP("BASE", (SDMMC1_BASE), 0x100),
			},
			.irqs = {
				VAL("", 49),
			},
			.pins = {
				PIN("TX", PC, PIN_6, AF8),
				PIN("RX", PC, PIN_7, AF8),
			},
			.clocks = {
				VAL("", CLK_SDMMC1),
			},
			.misc = {
				VAL("NAME_IN_CUBE_SDMMC", 1),
			},
		},
	},
	[1] = {
		.status = ENABLED,
		.dev = {
			.name = "SDIO2",
			.regs = {
				REGMAP("BASE", (SDMMC2_BASE), 0x100),
			},
			.irqs = {
				VAL("", 103),
				VAL("DMA_RX", 56),
				VAL("DMA_TX", 68),
			},
			.pins = {
				PIN("D0", GPIO_PORT_B, 3, AF10),
				PIN("D1", GPIO_PORT_B, 4, AF10),
				PIN("D2", GPIO_PORT_D, 6, AF11),
				PIN("D3", GPIO_PORT_D, 7, AF11),
				PIN("CK", GPIO_PORT_G, 9, AF11),
				PIN("CMD", GPIO_PORT_G, 10, AF11),

				PIN("DETECT", GPIO_PORT_I, 15, 0),

			},
			.clocks = {
				VAL("", STM32_CLK_ENABLE(SDMMC2)),
				VAL("TRANSFER_DIV", 0),
				VAL("DMA", STM32_CLK_ENABLE(DMA2)),
			},
			.misc = {
				VAL("NAME_IN_CUBE_SDMMC", 1),
			},
			.dmas = {
				VAL("NUM", 2),
				VAL("TX_CHANNEL", 11),
				VAL("RX_CHANNEL", 11),
				VAL("TX_STREAM", 5),
				VAL("RX_STREAM", 0),
			},
		},
	}
		
};


EXPORT_CONFIG(CLK(clks), GPIO(gpios), UART(uarts), LED(leds), MMC(mmcs))
