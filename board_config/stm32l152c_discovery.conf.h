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
				PIN("TX", PB, PIN_10, AF7),
				PIN("RX", PB, PIN_11, AF7),
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

struct spi_conf spis[] = {
    [1] = {
   	 .status = DISABLED,
   	 .name = "SPI1",
   	 .dev = {
		.name = "SPI1",
   		 .pins = {
   			 PIN("SCK",  GPIO_PORT_A, PIN_5, AF5),
   			 PIN("MISO", GPIO_PORT_A, PIN_11, AF5),
   			 PIN("MOSI", GPIO_PORT_A, PIN_12, AF5),
   			 PIN("CS",   GPIO_PORT_C, PIN_12, NOAF),
   		 },
   		 .clocks = {
   			 VAL("SPI",  CLK_SPI1),
   		 }
   	 },
    },
    [2] = {
   	 .status = DISABLED,
   	 .name = "SPI2",
   	 .dev = {
		.name = "SPI2",
   		 .pins = {
   			 PIN("SCK",  GPIO_PORT_B, PIN_13, AF5),
   			 PIN("MISO", GPIO_PORT_B, PIN_14, AF5),
   			 PIN("MOSI", GPIO_PORT_B, PIN_15, AF5),
   			 PIN("CS",   GPIO_PORT_B, PIN_12, NOAF),
   		 },
   		 .clocks = {
   			 VAL("SPI",  CLK_SPI2),
   		 }
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
   			 PIN("SCL", GPIO_PORT_B, PIN_10, AF4),
   			 PIN("SDA", GPIO_PORT_B, PIN_11, AF4),
   		 },
   		 .clocks = {
   			 VAL("I2C", CLK_I2C2),
   		 }
   	 },
    },
};

struct led_conf leds[] = {
	[0] = {
		.name = "LED1",
		.port = VAL("", GPIO_PORT_B),
		.pin = VAL("", 7),
	},
	[1] = {
		.name = "LED2",
		.port = VAL("", GPIO_PORT_B),
		.pin = VAL("", 6),
	},
};

EXPORT_CONFIG(UART(uarts), LED(leds), SPI(spis), I2C(i2cs))
