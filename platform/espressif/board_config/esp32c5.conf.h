#include <gen_board_conf.h>

#include <soc/esp32c5_gpio.h>
#include <soc/esp32c5_gpio_sig_map.h>
#include <soc/esp32c5_clk.h>
#include <soc/esp32c5_uart.h>

struct clk_conf clks[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "PCR",
			.regs = {
				REGMAP("BASE", 0x60096000, 0x1000),
			},
			.clocks = {
				VAL("XTAL_FREQ", 48000000),
			},
		},
	},
};

struct gpio_conf gpios[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT_A",
			.regs = {
				REGMAP("BASE", GPIO_BASE, 0x1000),
				REGMAP("IO_MUX", IO_MUX_BASE, 0x1000),
			},
		},
		.port_num = 1,
		.port_width = 29,
	},
};

/* Interrupt sources must be routed to CLIC before assigning CPU IRQs. */
struct uart_conf uarts[] = {
	[0] = {
		.status = ENABLED,
		.name = "UART0",
		.dev = {
			.name = "UART0",
			.regs = {
				REGMAP("BASE_ADDR", UART0_BASE, 0x1000),
			},
			.irqs = {
				/* Full CLIC ID; external interrupt inputs are 16..47. */
				VAL("", 17),
			},
			/* Direct UART0 connection through IO_MUX. */
			.pins = {
				PIN("TX", GPIO_PORT_A, GPIO11,
					GPIO_MODE_ALT, FUNC_U0TXD_U0TXD),
				PIN("RX", GPIO_PORT_A, GPIO12,
					GPIO_MODE_ALT, FUNC_U0RXD_U0RXD),
			},
			.clocks = {
				VAL("ID", ESP32_CLK_UART0),
				/* The standalone loader does not initialize the PLL. */
				VAL("SOURCE", ESP32_CLK_SRC_XTAL),
			},
		},
		.baudrate = 115200,
	},
	[1] = {
		.status = ENABLED,
		.name = "UART1",
		.dev = {
			.name = "UART1",
			.regs = {
				REGMAP("BASE_ADDR", UART1_BASE, 0x1000),
			},
			/* Example matrix routing; check pin availability on the board. */
			.pins = {
				PIN("TX", GPIO_PORT_A, GPIO23,
					GPIO_MODE_ALT, ESP32_MATRIX_OUT(U1TXD_OUT)),
				PIN("RX", GPIO_PORT_A, GPIO24,
					GPIO_MODE_ALT, ESP32_MATRIX_IN(U1RXD_IN)),
			},
			.clocks = {
				VAL("ID", ESP32_CLK_UART1),
				VAL("SOURCE", ESP32_CLK_SRC_PLL80M),
			},
		},
		.baudrate = 115200,
	},
};

/* Hardware GP-SPI2 is exposed as Embox SPI bus 0. */
struct spi_conf spis[] = {
	[0] = {
		.status = ENABLED,
		.name = "SPI0",
		.dev = {
			.name = "SPI0",
			.regs = {
				REGMAP("BASE", 0x60081000, 0x1000),
			},
			/* SPI2 through GPIO matrix; hardware CS0. */
			.pins = {
				PIN("CLK", GPIO_PORT_A, GPIO5,
					GPIO_MODE_ALT | GPIO_MODE_OUT, ESP32_MATRIX_OUT(FSPICLK_OUT)),
				PIN("TX", GPIO_PORT_A, GPIO7,
					GPIO_MODE_ALT | GPIO_MODE_OUT, ESP32_MATRIX_OUT(FSPID_OUT)),
				PIN("RX", GPIO_PORT_A, GPIO2,
					GPIO_MODE_ALT | GPIO_MODE_IN, ESP32_MATRIX_IN(FSPIQ_IN)),
				PIN("CS", GPIO_PORT_A, GPIO10,
					GPIO_MODE_ALT | GPIO_MODE_OUT, ESP32_MATRIX_OUT(FSPICS0_OUT)),
			},
			.clocks = {
				VAL("ID", ESP32_CLK_SPI2),
				VAL("SOURCE", ESP32_CLK_SRC_PLL160M),
				VAL("SOURCE_DIV", 2),
			},
		},
		/* PLL 160 MHz / 2 / 80 = fixed 1 MHz SCLK. */
		.clk_div = 80,
	},
};

struct i2c_conf i2cs[] = {
	[0] = {
		.status = ENABLED,
		.name = "I2C0",
		.dev = {
			.name = "I2C0",
			.regs = {
				REGMAP("BASE", 0x60004000, 0x1000),
			},
			/* AF holds the matrix signal ID, shared by input/output.
			 * Route both directions for each pin in the I2C driver.
			 * External pull-ups to 3.3 V are required.
			 */
			.pins = {
				PIN("SCL", GPIO_PORT_A, GPIO4,
					GPIO_MODE_IN | GPIO_MODE_OUT
						| GPIO_MODE_OUT_OPEN_DRAIN, I2CEXT0_SCL_OUT),
				PIN("SDA", GPIO_PORT_A, GPIO3,
					GPIO_MODE_IN | GPIO_MODE_OUT
						| GPIO_MODE_OUT_OPEN_DRAIN, I2CEXT0_SDA_OUT),
			},
			.clocks = {
				VAL("ID", ESP32_CLK_I2C0),
				VAL("SOURCE", ESP32_CLK_SRC_XTAL),
				VAL("SOURCE_DIV", 1),
			},
		},
	},
};

EXPORT_CONFIG(CLK(clks), GPIO(gpios), UART(uarts), SPI(spis), I2C(i2cs));
