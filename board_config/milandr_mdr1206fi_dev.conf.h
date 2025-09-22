#include <gen_board_conf.h>

#include <mdr1206fi.h>

/*

    static const RST_CLK_HCLK_InitTypeDef Clk = {
        .RST_CLK_CPU_C1_Source      = RST_CLK_CPU_C1_CLK_SRC_HSE,
        .RST_CLK_PLLCPU_ClockSource = RST_CLK_PLLCPU_CLK_SRC_HSE,
        .RST_CLK_PLLCPU_Multiplier  = RST_CLK_PLLCPU_MUL_4,
        .RST_CLK_CPU_C2_ClockSource = RST_CLK_CPU_C2_CLK_SRC_PLLCPU,
        .RST_CLK_CPU_C3_Prescaler   = RST_CLK_CPU_C3_PRESCALER_DIV_1,
        .RST_CLK_HCLK_ClockSource   = RST_CLK_CPU_HCLK_CLK_SRC_CPU_C3,
    };
*/

#define HSI_CLK_FREQUENCY_MIN_Hz 7500000UL
#define HSI_CLK_FREQUENCY_TYP_Hz 8000000UL
#define HSI_CLK_FREQUENCY_MAX_Hz 8500000UL

/* Clock generators frequencies if the SPL is not used. */
/** HSI clock value [Hz].
    Default: 8000000 (8MHz). */
#define HSI_FREQUENCY_Hz HSI_CLK_FREQUENCY_TYP_Hz
/** HSE clock value [Hz].
    Default: 8000000 (8MHz). */
#define HSE_FREQUENCY_Hz 8000000UL
/** LSI clock value [Hz].
    Default: 31250 (31.25kHz). */
#define LSI_FREQUENCY_Hz (HSI_FREQUENCY_Hz / 256)
/** LSE clock value [Hz].
    Default: 32768 (32.768kHz). */
#define LSE_FREQUENCY_Hz 32768UL

struct clk_conf clks[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "RST_CLK",
			.regs = {
				REGMAP("BASE", (MDR_RST_CLK_BASE), 0x100),
			},
			.clocks = {
				VAL("HSECLK_VAL", HSE_FREQUENCY_Hz),
			}
		},
		.type = {
			VAL("SYSCLK_PLL", 1),
		},
	},
};

struct gpio_conf gpios[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_A",
			.regs = {
				REGMAP("BASE", (MDR_PORTA_BASE), 0x100),
			},
			.irqs = {
				VAL("", EXT_INT1_IRQ),
			},
			.clocks = {
				VAL("", "CLK_GPIOA"),
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
				REGMAP("BASE", (MDR_PORTB_BASE), 0x100),
			},
			.irqs = {
				VAL("", EXT_INT1_IRQ),
			},
			.clocks = {
				VAL("", "CLK_GPIOB"),
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
				REGMAP("BASE", (MDR_PORTA_BASE), 0x100),
			},
			.irqs = {
				VAL("", EXT_INT1_IRQ),
			},
			.clocks = {
				VAL("", "CLK_GPIOC"),
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
				REGMAP("BASE", (MDR_PORTD_BASE), 0x100),
			},
			.irqs = {
				VAL("", EXT_INT1_IRQ),
			},
			.clocks = {
				VAL("", "CLK_GPIOD"),
			}
		},
		.port_num = 4,
		.port_width = 16,
	},
};

struct uart_conf uarts[] = {
	[1] = {
		.status = DISABLED,
		.name = "UART1",
		.dev = {
			.name = "UART1",
			.regs = {
				REGMAP("BASE_ADDR", (MDR_UART1_BASE), 0x100),
			},
			.irqs = {
				VAL("", MDR_UART1_IRQ),
			},
			.pins = {
				PIN("TX", GPIO_PORT_B, 0, 1),
				PIN("RX", GPIO_PORT_B, 1, 1),
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
				REGMAP("BASE_ADDR", (MDR_UART2_BASE), 0x100),
			},
			.irqs = {
				VAL("", MDR_UART2_IRQ),
			},
			.pins = {
				PIN("TX", GPIO_PORT_D, 4, 1),
				PIN("RX", GPIO_PORT_D, 5, 1),
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
				REGMAP("BASE_ADDR", (MDR_UART3_BASE), 0x100),
			},
			.irqs = {
				VAL("", MDR_UART3_IRQ),
			},
			.pins = {
				PIN("TX", GPIO_PORT_D, 7, 1),
				PIN("RX", GPIO_PORT_D, 6, 1),
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
				REGMAP("BASE_ADDR", (MDR_UART4_BASE), 0x100),
			},
			.irqs = {
				VAL("", MDR_UART4_IRQ),
			},
			.pins = {
				PIN("TX", GPIO_PORT_D, 9, 1),
				PIN("RX", GPIO_PORT_D, 8, 1),
			},
			.clocks = {
				VAL("", "CLK_UART4"),
			}
		},
		.baudrate = 115200,
	},
};

struct led_conf leds[] = {
	[0] = {
		.name = "LED1",
		.port = VAL("", GPIO_PORT_D),
		.pin = VAL("", 5),
		.level = VAL("", GPIO_PIN_LOW),
	},
	[1] = {
		.name = "LED2",
		.port = VAL("", GPIO_PORT_D),
		.pin = VAL("", 6),
		.level = VAL("", GPIO_PIN_LOW),
	},
	[2] = {
		.name = "LED3",
		.port = VAL("", GPIO_PORT_D),
		.pin = VAL("", 7),
		.level = VAL("", GPIO_PIN_LOW),
	},
	[3] = {
		.name = "LED4",
		.port = VAL("", GPIO_PORT_D),
		.pin = VAL("PIN", 7),
		.level = VAL("", GPIO_PIN_LOW),
	},
};

struct lcd_conf lcds[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "LCD",
			.pins = {
				PIN("DB0", GPIO_PORT_B, 6, 0),
				PIN("DB1", GPIO_PORT_B, 7, 0),
				PIN("DB2", GPIO_PORT_B, 8, 0),
				PIN("DB3", GPIO_PORT_B, 9, 0),
				PIN("DB4", GPIO_PORT_B, 10, 0),
				PIN("DB5", GPIO_PORT_B, 11, 0),
				PIN("DB6", GPIO_PORT_B, 12, 0),
				PIN("DB7", GPIO_PORT_B, 13, 0),
				PIN("CS1", GPIO_PORT_B, 4, 0),
				PIN("CS2", GPIO_PORT_B, 5, 0),
				PIN("RST", GPIO_PORT_A, 4, 0),
				PIN("RW", GPIO_PORT_B, 14, 0),
				PIN("RS", GPIO_PORT_B, 2, 0),
				PIN("EN", GPIO_PORT_B, 3, 0),	
			},
		},
		.width = 128,
		.height = 64,
		.bits_per_pixel = 1,
	},
};

EXPORT_CONFIG(CLK(clks), GPIO(gpios), UART(uarts), LED(leds), LCD(lcds))
