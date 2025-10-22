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
		.status = ENABLED,
		.name = "USART1",
		.dev = {
			.name = "USART1",
			.irqs = {
				VAL("", 37),
			},
			.pins = {
				PIN("TX", PA, PIN_9, AF7),
				PIN("RX", PA, PIN_10, AF7),
			},
			.clocks = {
				VAL("TX",   CLK_GPIOA),
				VAL("RX",   CLK_GPIOA),
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
				PIN("SCK",  GPIO_PORT_B, 3, AF5),
				PIN("MISO", GPIO_PORT_B, 4, AF5),
				PIN("MOSI", GPIO_PORT_B, 5, AF5),
				PIN("CS",   GPIO_PORT_B, 2, NOAF),
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
					PIN("CS",   GPIO_PORT_B, 2, NOAF),
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
				PIN("CS",   GPIO_PORT_B, 12, NOAF),
			},
			.clocks = {
				VAL("SPI",  CLK_SPI2),
			}
		},
		.spi_devs[0] = {
			.status = ENABLED,
			.name = "SPI2_0",
			.bits_per_word = 8,
			.clk_freq = 8, /* clk_div */
			.bus_num = 2,
			.idx     = 0,
			.dev = {
				.name = "SPI2_0",
				.pins = {
					PIN("CS",   GPIO_PORT_B, 12, NOAF),
				},
			},
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
				PIN("TIM",  PB, PIN_6, AF2),
			},
			.clocks = {
				VAL("GPIO",  CLK_GPIOB),
				VAL("TIM",  CLK_TIM4),
			}
		},
	},
};

struct led_conf leds[] = {
	[0] = {
		.name = "LED1",
		.port = VAL("", GPIO_PORT_A),
		.pin = VAL("", 6),
		.level = VAL("", GPIO_PIN_HIGH),
	},
	[1] = {
		.name = "LED2",
		.port = VAL("", GPIO_PORT_A),
		.pin = VAL("", 7),
		.level = VAL("", GPIO_PIN_HIGH),
	},
};

struct mmc_conf mmcs[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "SDIO1",
			.regs = {
				REGMAP("BASE", (SDIO_BASE), 0x100),
			},
			.irqs = {
				VAL("", 49),
				VAL("DMA_RX", 59), /* DMA2_Stream3 */
				VAL("DMA_TX", 69), /* DMA2_Stream6 */				
			},
			.pins = {
				PIN("D0", GPIO_PORT_C, 8, AF12),
				PIN("D1", GPIO_PORT_C, 9, AF12),
				PIN("D2", GPIO_PORT_C, 10, AF12),
				PIN("D3", GPIO_PORT_C, 11, AF12),
				PIN("CK", GPIO_PORT_C, 12, AF12),
				PIN("CMD", GPIO_PORT_D, 2, AF12),

			},
			.clocks = {
				VAL("", STM32_CLK_ENABLE(SDIO)),
				VAL("TRANSFER_DIV", 4),
				VAL("DMA", STM32_CLK_ENABLE(DMA2)),
			},
			.misc = {
				VAL("NAME_IN_CUBE_SDIO", 1),
			},
			.dmas = {
				VAL("NUM", 2),
				VAL("TX_CHANNEL", 4),
				VAL("RX_CHANNEL", 4),
				VAL("TX_STREAM", 6),
				VAL("RX_STREAM", 3),
			},
		},
	},
};

EXPORT_CONFIG(CLK(clks), GPIO(gpios), UART(uarts), SPI(spis), PWM(pwms), LED(leds), MMC(mmcs))
