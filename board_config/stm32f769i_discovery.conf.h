#include <gen_board_conf.h>
#include <stm32.h>

#include <stm32f7_chip.h>

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
		.status = ENABLED,
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
			},
			.misc = {
				VAL("NAME_IN_CUBE_SDMMC", 1),
			},
		},
	}
		
};


EXPORT_CONFIG(UART(uarts), LED(leds), MMC(mmcs))
