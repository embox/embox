/**
 * @file
 * 
 * @author Anton Bondarev
 * @date 10.10.2025
 */

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include <drivers/clk.h>
#include <drivers/common/memory.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <util/log.h>

#include <config/board_config.h>

/* FROM board_config.h*/
#define CLK_NAME_GPIO "CLK_GPIO"
#define CLK_NAME_UART "CLK_UART"
#define CLK_NAME_SPI  "CLK_SPI"
#define CLK_NAME_I2C  "CLK_I2C"
#define CLK_NAME_TMR  "CLK_TMR"

#define CRU ((volatile struct bmcu_cru_regs *)CONF_CRU_REGION_BASE)

/* clang-format off */
struct bmcu_cru_regs {
    volatile uint32_t CLKSEL;        /*!< Clock select control,                Address offset: 0x00 */
    volatile uint32_t PLLSET;        /*!< PLL configuration set,               Address offset: 0x04 */
    volatile uint32_t SYSCLKSEL0;    /*!< Clock domains select control 0,      Address offset: 0x08 */
    volatile uint32_t PCLK0EN;       /*!< APB0 domain clock gating control,    Address offset: 0x0C */
    volatile uint32_t PCLK1EN;       /*!< APB1 domain clock gating control,    Address offset: 0x10 */
    volatile uint32_t PCLK2EN;       /*!< APB2 domain clock gating control,    Address offset: 0x14 */
    volatile uint32_t SYSCTRL0;      /*!< System resource control 0,           Address offset: 0x18 */
    volatile uint32_t SYSCTRL1;      /*!< System resource control 1,           Address offset: 0x1C */
    volatile uint32_t SYSCTRL2;      /*!< System resource control 2,           Address offset: 0x20 */
    volatile uint32_t PRIOR0;        /*!< AXI masters priority control,        Address offset: 0x24 */
    volatile uint32_t PRIOR1;        /*!< AXI slave priority control,          Address offset: 0x28 */
    volatile uint32_t PADPUEN0;      /*!< PullUp control 0,                    Address offset: 0x2C */
    volatile uint32_t PADPUEN1;      /*!< PullUp control 1,                    Address offset: 0x30 */
    volatile uint32_t PADPDEN0;      /*!< PullDown control 0,                  Address offset: 0x34 */
    volatile uint32_t PADPDEN1;      /*!< PullDown control 1,                  Address offset: 0x38 */
    volatile uint32_t PADALT0;       /*!< PAD alternate function control 0,    Address offset: 0x3C */
    volatile uint32_t PADALT1;       /*!< PAD alternate function control 1,    Address offset: 0x40 */
    volatile uint32_t PADALT2;       /*!< PAD alternate function control 2,    Address offset: 0x44 */
    volatile uint32_t PADALT3;       /*!< PAD alternate function control 3,    Address offset: 0x48 */
    volatile uint32_t PADALT4;       /*!< PAD alternate function control 4,    Address offset: 0x4C */
    volatile uint32_t PADALT5;       /*!< PAD alternate function control 5,    Address offset: 0x50 */
    volatile uint32_t PADDS0;        /*!< Port 0 outputs drive strength,       Address offset: 0x54 */
    volatile uint32_t PADDS1;        /*!< Port 1 outputs drive strength,       Address offset: 0x58 */
    volatile uint32_t PADDS2;        /*!< Port 2 outputs drive strength,       Address offset: 0x5C */
    volatile uint32_t LDOCTRL;       /*!< LDO control register,                Address offset: 0x60 */
    uint32_t      RESERVED0[3];      /*!< Reserved,                            Address offset: 0x64 */
    volatile uint32_t SYSSTAT0;      /*!< System resource status,              Address offset: 0x70 */
    volatile uint32_t WDTCLRKEY;     /*!< Secure key for WDTx reset            Address offset: 0x74 */
    uint32_t      RESERVED1[2];      /*!< Reserved,                            Address offset: 0x78 */
    volatile uint32_t INTCTRL0;      /*!< Interrupt route control,             Address offset: 0x80 */
    volatile uint32_t SYSCLKSEL1;    /*!< Clock domains select control 1,      Address offset: 0x84 */
    volatile uint32_t CRPTADDR;      /*!< MAGMA address register,              Address offset: 0x88 */
    volatile uint32_t CRPTDATA;      /*!< MAGMA data register,                 Address offset: 0x8C */
};
/* clang-format on */

/** @brief APB0 peripherals */
#define CRU_APB0_PERIPH_QSPI0  0  /*!< QSPI0 */
#define CRU_APB0_PERIPH_SPI0   1  /*!< SPI0 */
#define CRU_APB0_PERIPH_SPI1   2  /*!< SPI1 */
#define CRU_APB0_PERIPH_UART0  3  /*!< UART0 */
#define CRU_APB0_PERIPH_UART1  4  /*!< UART1 */
#define CRU_APB0_PERIPH_UART2  5  /*!< UART2 */
#define CRU_APB0_PERIPH_I2C0   6  /*!< I2C0 */
#define CRU_APB0_PERIPH_I2C1   7  /*!< I2C1 */
#define CRU_APB0_PERIPH_I2S0   8  /*!< I2S0 */
#define CRU_APB0_PERIPH_TIM0   9  /*!< TIM0 */
#define CRU_APB0_PERIPH_WDT0   10 /*!< WDT0 */
#define CRU_APB0_PERIPH_GPIO0  11 /*!< GPIO0 */
#define CRU_APB0_PERIPH_PWMG0  12 /*!< PWMG0 */
#define CRU_APB0_PERIPH_CANFD0 13 /*!< CANFD0 */

/** @brief APB1 peripherals */
typedef enum {
	CRU_APB1_PERIPH_QSPI1 = 0, /*!< QSPI1 */
	CRU_APB1_PERIPH_SPI2,      /*!< SPI2 */
	CRU_APB1_PERIPH_SPI3,      /*!< SPI3 */
	CRU_APB1_PERIPH_UART3,     /*!< UART3 */
	CRU_APB1_PERIPH_UART4,     /*!< UART4 */
	CRU_APB1_PERIPH_UART5,     /*!< UART5 */
	CRU_APB1_PERIPH_I2C2,      /*!< I2C2 */
	CRU_APB1_PERIPH_I2C3,      /*!< I2C3 */
	CRU_APB1_PERIPH_I2S1,      /*!< I2S1 */
	CRU_APB1_PERIPH_TIM1,      /*!< TIM1 */
	CRU_APB1_PERIPH_WDT1,      /*!< WDT1 */
	CRU_APB1_PERIPH_GPIO1,     /*!< GPIO1 */
	CRU_APB1_PERIPH_PWMG1,     /*!< PWMG1 */
	CRU_APB1_PERIPH_CANFD1     /*!< CANFD1 */
} CRU_APB1Periph_t;

/** @brief APB2 peripherals */
typedef enum {
	CRU_APB2_PERIPH_ADC0 = 0, /*!< ADC0 */
	CRU_APB2_PERIPH_ADC1,     /*!< ADC1 */
	CRU_APB2_PERIPH_ADC2,     /*!< ADC2 */
	CRU_APB2_PERIPH_PWMA0,    /*!< PWMA0 */
	CRU_APB2_PERIPH_PWMA1,    /*!< PWMA1 */
	CRU_APB2_PERIPH_PWMA2,    /*!< PWMA2 */
	CRU_APB2_PERIPH_PWMA3,    /*!< PWMA3 */
	CRU_APB2_PERIPH_GPIO2,    /*!< GPIO2 */
	CRU_APB2_PERIPH_UART6,    /*!< UART6 */
	CRU_APB2_PERIPH_UART7     /*!< UART7 */
} CRU_APB2Periph_t;

static void bmcu_cru_gpio_en(int num) {
	switch (num) {
	case 0:
		CRU->PCLK0EN |= (1UL << (CRU_APB0_PERIPH_GPIO0)&0xFFFFUL);
		break;
	case 1:
		CRU->PCLK1EN |= (1UL << (CRU_APB1_PERIPH_GPIO1)&0xFFFFUL);
		break;
	case 2:
		CRU->PCLK2EN |= (1UL << (CRU_APB2_PERIPH_GPIO2)&0xFFFFUL);
		break;
	}
}

static void bmcu_cru_uart_en(int num) {
	int periph = CRU_APB0_PERIPH_UART0 + num;

	CRU->PCLK0EN |= (1UL << (periph)&0xFFFFUL);
}

static void bmcu_cru_spi_en(int num) {
}

int clk_enable(char *clk_name) {
	int num;

	if (0 == strncmp(clk_name, CLK_NAME_GPIO, sizeof(CLK_NAME_GPIO) - 1)) {
		/* '0' or 'A'*/
		if (isdigit(clk_name[sizeof(CLK_NAME_GPIO) - 1])) {
			num = clk_name[sizeof(CLK_NAME_GPIO) - 1] - '0';
		}
		else {
			num = clk_name[sizeof(CLK_NAME_GPIO) - 1] - 'A';
		}

		bmcu_cru_gpio_en(num);

		return 0;
	}

	if (0 == strncmp(clk_name, CLK_NAME_UART, sizeof(CLK_NAME_UART) - 1)) {
		num = clk_name[sizeof(CLK_NAME_UART) - 1] - '0';
		bmcu_cru_uart_en(num);

		return 0;
	}

	if (0 == strncmp(clk_name, CLK_NAME_SPI, sizeof(CLK_NAME_SPI) - 1)) {
		num = clk_name[sizeof(CLK_NAME_SPI) - 1] - '0';
		bmcu_cru_spi_en(num);

		return 0;
	}

	return -ENOSUPP;
}

int clk_altfunc_enable(int port, int pin_num, int func) {
	volatile uint32_t *reg;
	uint32_t val;

	reg = &(CRU->PADALT0) + port * 2;

	if (pin_num > 7) {
		pin_num -= 8;
		reg++;
	}

	val = REG32_LOAD(reg) & ~((uint32_t)(0x7 << (pin_num << 0x2)));
	val |= ((uint32_t)(func << (pin_num << 0x2)));
	REG32_STORE(reg, val);

	return 0;
}
