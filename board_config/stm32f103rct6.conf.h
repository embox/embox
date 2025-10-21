#include <gen_board_conf.h>
#include <stm32.h>
#include <stm32F10xxx_chip.h>

struct clk_conf clks[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "RCC",
			.regs = {
				REGMAP("BASE", (RCC_BASE), 0x100),
			},
			
		},
		.type = {
			VAL("", 0),
		},
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
			.regs = {
				REGMAP("BASE", (USART2_BASE), 0x100),
			},
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
				PIN("SCK",  GPIO_PORT_A, 5, AF5),
				PIN("MISO", GPIO_PORT_A, 6, AF5),
				PIN("MOSI", GPIO_PORT_A, 7, AF5),
				PIN("CS",   GPIO_PORT_A, 4, NOAF),
			},
			.clocks = {
				VAL("SPI",  CLK_SPI1),
			}
		},
	},
};

EXPORT_CONFIG(CLK(clks), UART(uarts), SPI(spis))
