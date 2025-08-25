/**
 * @file
 * 
 * @author Anton Bondarev
 * @date 04.04.2024
 */

#include <util/log.h>

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <drivers/common/memory.h>

//#include <config/board_config.h>

#include <framework/mod/options.h>

#include <drivers/clk/mdr1206_rst_clk.h>

/* FROM board_config.h*/
#define CLK_NAME_GPIO     "CLK_GPIO"
#define CLK_NAME_UART     "CLK_UART"
#define CLK_NAME_SPI      "CLK_SPI"
#define CLK_NAME_I2C      "CLK_I2C"
#define CLK_NAME_TMR      "CLK_TMR"

#define MDR_RST_CLK         ((volatile struct mdr_rst_clk_regs *) 0x50020000)

struct mdr_rst_clk_regs {
    volatile uint32_t CLOCK_STATUS; /*!< 0x00 RST_CLK Clock Status Register */
    volatile uint32_t PLL_CONTROL;  /*!< 0x04 RST_CLK PLL Control Register */
    volatile uint32_t HS_CONTROL;   /*!< 0x08 RST_CLK HS Control Register */
    volatile uint32_t CPU_CLOCK;    /*!< 0x0С RST_CLK CPU Clock Register */
    volatile uint32_t PER1_CLOCK;   /*!< 0x10 RST_CLK PER1 Clock Register */
    volatile uint32_t ADC_CLOCK;    /*!< 0x14 RST_CLK ADC Clock Register */
    volatile uint32_t RTC_CLOCK;    /*!< 0x18 RST_CLK RTC Clock Register */
    volatile uint32_t PER2_CLOCK;   /*!< 0x1С RST_CLK Peripheral Clock Enable Register */

    volatile uint32_t DMA_DONE_STICK; /*!< 0x20 RST_CLK DMA_DONE Fixation Register */

    volatile uint32_t TIM_CLOCK;  /*!< 0x24 RST_CLK Timer Clock Register */
    volatile uint32_t UART_CLOCK; /*!< 0x28 RST_CLK UART Clock Register */
    volatile uint32_t SSP_CLOCK;  /*!< 0x2С RST_CLK SSP Clock Register */

    volatile uint32_t DIV_SYS_TIM; /*!< 0x30 Machine Timer Prescaler Register */
};

#define RST_CLK_REGS          ((volatile struct rcu_reg *) CONF_RCU_REGION_BASE)

#define MDR_RST_PER2_CLOCK_CLK_PORTA_BIT         16
#define MDR_RST_PER2_CLOCK_CLK_PORTB_BIT         17
#define MDR_RST_PER2_CLOCK_CLK_PORTC_BIT         18
#define MDR_RST_PER2_CLOCK_CLK_PORTD_BIT         30

#define MDR_RST_PER2_CLOCK_CLK_PORTA       (1 << MDR_RST_PER2_CLOCK_CLK_PORTA_BIT)
#define MDR_RST_PER2_CLOCK_CLK_PORTB       (1 << MDR_RST_PER2_CLOCK_CLK_PORTB_BIT)
#define MDR_RST_PER2_CLOCK_CLK_PORTC       (1 << MDR_RST_PER2_CLOCK_CLK_PORTC_BIT)
#define MDR_RST_PER2_CLOCK_CLK_PORTD       (1 << MDR_RST_PER2_CLOCK_CLK_PORTD_BIT)

int milandr_gpio_clock_setup(unsigned char port) {
    switch(port) {
        case 0:
            MDR_RST_CLK->PER2_CLOCK |= MDR_RST_PER2_CLOCK_CLK_PORTA;
            break;
        case 1:
            MDR_RST_CLK->PER2_CLOCK |= MDR_RST_PER2_CLOCK_CLK_PORTB;
            break;
        case 2:
            MDR_RST_CLK->PER2_CLOCK |= MDR_RST_PER2_CLOCK_CLK_PORTC;
            break;
        case 3:
            MDR_RST_CLK->PER2_CLOCK |= MDR_RST_PER2_CLOCK_CLK_PORTD;
            break;
        default:
            return -ENOSUPP;

    }

	return 0;
}

void milandr_uart_clock_setup(int uart_num) {

}

int clk_enable(char *clk_name) {
    int num;

    if (0 == strncmp(clk_name, CLK_NAME_GPIO, sizeof(CLK_NAME_GPIO) - 1)) {
        num = clk_name[sizeof(CLK_NAME_GPIO) - 1] - 'A';
        milandr_gpio_clock_setup(num);
        return 0;
    }
    if (0 == strncmp(clk_name, CLK_NAME_UART, sizeof(CLK_NAME_UART) - 1)) {
        num = clk_name[sizeof(CLK_NAME_UART) - 1]  - '0';
        milandr_gpio_clock_setup(num);
        return 0;
    }

    return -ENOSUPP;
}
