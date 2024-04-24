#include <gen_board_conf.h>
#include <stm32.h>

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
				PIN("TX", PC, PIN_4, AF7),
				PIN("RX", PC, PIN_5, AF7),
			},
			.clocks = {
				VAL("TX",   CLK_GPIOC),
				VAL("RX",   CLK_GPIOC),
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
				VAL("", 71),
			},
			.pins = {
				PIN("TX", PC, PIN_10, AF8),
				PIN("RX", PC, PIN_10, AF8),
			},
			.clocks = {
				VAL("TX",   CLK_GPIOB),
				VAL("RX",   CLK_GPIOB),
				VAL("UART", CLK_USART3),
			}
		},
		.baudrate = 115200,
	},
};

struct pwm_conf pwms[] = {
	[0] = {
		.name = "PWM0",
		.instance = VAL("INSTANCE", TIM2),
		.channel = VAL("CHANNEL_TIM", CHANNEL_TIM2),
		.servo_low = VAL("LOW", 430),
		.servo_high = VAL("HIGH", 2175),
		.dev = {
			.name = "PWM0",
			.pins = {
				PIN("TIM",  PD, PIN_4, AF2),
			},
			.clocks = {
				VAL("GPIO",  CLK_GPIOD),
				VAL("TIM",  CLK_TIM2),
			}
		},
	},
};

struct led_conf leds[] = {
	[0] = {
		.name = "LED3",
		.port = VAL("", GPIO_PORT_E),
		.pin = VAL("", 9),
	},
	[1] = {
		.name = "LED4",
		.port = VAL("", GPIO_PORT_E),
		.pin = VAL("", 8),
	},
	[2] = {
		.name = "LED5",
		.port = VAL("", GPIO_PORT_E),
		.pin = VAL("", 10),
	},
	[3] = {
		.name = "LED6",
		.port = VAL("", GPIO_PORT_E),
		.pin = VAL("PIN", 15),
	},
	[4] = {
		.name = "LED7",
		.port = VAL("", GPIO_PORT_E),
		.pin = VAL("", 11),
	},
	[5] = {
		.name = "LED8",
		.port = VAL("", GPIO_PORT_E),
		.pin = VAL("", 14),
	},
	[6] = {
		.name = "LED9",
		.port = VAL("", GPIO_PORT_E),
		.pin = VAL("", 12),
	},
	[7] = {
		.name = "LED10",
		.port = VAL("PORT", GPIO_PORT_E),
		.pin = VAL("PIN", 13),
	},
};

struct spi_conf spis[] = {
	[1] = {
		.status = ENABLED,
		.name = "SPI1",
		.dev = {
			.name = "SPI1",
			.pins = {
				PIN("SCK",  GPIO_PORT_B, PIN_5, AF5),
				PIN("MISO", GPIO_PORT_B, PIN_6, AF5),
				PIN("MOSI", GPIO_PORT_B, PIN_7, AF5),
				/* PIN("CS",   GPIO_PORT_B, PIN_2, NOAF), */
			},
			.clocks = {
				VAL("SCK",  CLK_GPIOA),
				VAL("MISO", CLK_GPIOA),
				VAL("MOSI", CLK_GPIOA),
				/* VAL("CS",   CLK_GPIOB), */
				VAL("SPI",  CLK_SPI1),
			}
		},
	},

};

EXPORT_CONFIG(UART(uarts), PWM(pwms), LED(leds), SPI(spis))
