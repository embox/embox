#include <gen_board_conf.h>

#define K210_GPIO_BASE_ADDR              0x50200000UL

#define K210_UART1_BASE_ADDR             0x50210000UL
#define K210_UART2_BASE_ADDR             0x50220000UL
#define K210_UART3_BASE_ADDR             0x50230000UL

#define K210_RST_ROM    0
#define K210_RST_DMA    1
#define K210_RST_AI     2
#define K210_RST_DVP    3
#define K210_RST_FFT    4
#define K210_RST_GPIO   5
#define K210_RST_SPI0   6
#define K210_RST_SPI1   7
#define K210_RST_SPI2   8
#define K210_RST_SPI3   9
#define K210_RST_I2S0   10
#define K210_RST_I2S1   11
#define K210_RST_I2S2   12
#define K210_RST_I2C0   13
#define K210_RST_I2C1   14
#define K210_RST_I2C2   15
#define K210_RST_UART1  16
#define K210_RST_UART2  17
#define K210_RST_UART3  18
#define K210_RST_AES    19
#define K210_RST_FPIOA  20
#define K210_RST_TIMER0 21
#define K210_RST_TIMER1 22
#define K210_RST_TIMER2 23
#define K210_RST_WDT0   24
#define K210_RST_WDT1   25
#define K210_RST_SHA    26
#define K210_RST_RTC    29

struct clk_conf clks[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "RCU",
			.clocks = {
				VAL("HSECLK_VAL", 26000000UL),
			}
		},
		.type = VAL("SYSCLK_PLL", 1),
	},
};

struct gpio_conf gpios[] = {
	[0] = {
		.status = ENABLED,
		.dev = {
			.name = "GPIO_PORT",
			.regs = {
				REGMAP("BASE", (K210_GPIO_BASE_ADDR), 0x80),
			},
			.irqs = {
				VAL("", K210_RST_GPIO),
			},
			.clocks = {
				VAL("", "CLK_GPIO"),
			}
		},
		.port_num = 1,
		.port_width = 8,
	},
};

EXPORT_CONFIG(CLK(clks), GPIO(gpios), 
				)