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

#include <config/board_config.h>

#include <framework/mod/options.h>

#include <drivers/clk/mdr1206_rst_clk.h>

/* FROM board_config.h*/
#define CLK_NAME_GPIO     "CLK_GPIO"
#define CLK_NAME_UART     "CLK_UART"
#define CLK_NAME_SPI      "CLK_SPI"
#define CLK_NAME_I2C      "CLK_I2C"
#define CLK_NAME_TMR      "CLK_TMR"

#define MDR_RST_CLK         ((volatile struct mdr_rst_clk_regs *) CONF_RST_CLK_REGION_BASE)

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

#define RST_CLK_PER1_CLOCK_PER1_C2_SEL_Pos        2

#define RST_CLK_PER1_PRESCALER_DIV_1    0x0
#define RST_CLK_PER1_PRESCALER_DIV_2    0x1
#define RST_CLK_PER1_PRESCALER_DIV_4    0x2
#define RST_CLK_PER1_PRESCALER_DIV_8    0x3
#define RST_CLK_PER1_PRESCALER_DIV_16   0x4
#define RST_CLK_PER1_PRESCALER_DIV_32   0x5
#define RST_CLK_PER1_PRESCALER_DIV_64   0x6
#define RST_CLK_PER1_PRESCALER_DIV_128  0x7

#define RST_CLK_UART_CLOCK_UART_BRG_Mask     0x7

#define RST_CLK_UART_CLOCK_UART1_BRG_Pos     0
#define RST_CLK_UART_CLOCK_UART2_BRG_Pos     8
#define RST_CLK_UART_CLOCK_UART3_BRG_Pos     16
#define RST_CLK_UART_CLOCK_UART4_BRG_Pos     27
#define RST_CLK_UART_CLOCK_UART1_CLK_EN_Pos  24
#define RST_CLK_UART_CLOCK_UART2_CLK_EN_Pos  25
#define RST_CLK_UART_CLOCK_UART3_CLK_EN_Pos  26
#define RST_CLK_UART_CLOCK_UART4_CLK_EN_Pos  30

#define MDR_RST_PER2_CLOCK_CLK_SSP1_BIT           0
#define MDR_RST_PER2_CLOCK_CLK_UART1_BIT          1
#define MDR_RST_PER2_CLOCK_CLK_UART2_BIT          2
#define MDR_RST_PER2_CLOCK_CLK_FLASH_BIT          3
#define MDR_RST_PER2_CLOCK_CLK_RST_CLK_BIT        4
#define MDR_RST_PER2_CLOCK_CLK_DMA_BIT            5
#define MDR_RST_PER2_CLOCK_CLK_I2C_BIT            6
#define MDR_RST_PER2_CLOCK_CLK_UART3_BIT          7
#define MDR_RST_PER2_CLOCK_CLK_ADC_BIT            8
#define MDR_RST_PER2_CLOCK_CLK_WWDG_BIT           9
#define MDR_RST_PER2_CLOCK_CLK_IWDG_BIT          10
#define MDR_RST_PER2_CLOCK_CLK_POWER_BIT         11
#define MDR_RST_PER2_CLOCK_CLK_BKP_BIT           12
#define MDR_RST_PER2_CLOCK_CLK_ADCUI_BIT         13
#define MDR_RST_PER2_CLOCK_CLK_TIMER1_BIT        14
#define MDR_RST_PER2_CLOCK_CLK_TIMER2_BIT        15
#define MDR_RST_PER2_CLOCK_CLK_PORTA_BIT         16
#define MDR_RST_PER2_CLOCK_CLK_PORTB_BIT         17
#define MDR_RST_PER2_CLOCK_CLK_PORTC_BIT         18

#define MDR_RST_PER2_CLOCK_CLK_UART4_BIT         29
#define MDR_RST_PER2_CLOCK_CLK_PORTD_BIT         30

#define MDR_RST_PER2_CLOCK_CLK_SSP1       (1 << MDR_RST_PER2_CLOCK_CLK_SSP1_BIT)
#define MDR_RST_PER2_CLOCK_CLK_UART1      (1 << MDR_RST_PER2_CLOCK_CLK_UART1_BIT)
#define MDR_RST_PER2_CLOCK_CLK_UART2      (1 << MDR_RST_PER2_CLOCK_CLK_UART2_BIT)
#define MDR_RST_PER2_CLOCK_CLK_FLASH      (1 << MDR_RST_PER2_CLOCK_CLK_FLASH_BIT)
#define MDR_RST_PER2_CLOCK_CLK_RST_CLK    (1 << MDR_RST_PER2_CLOCK_CLK_RST_CLK_BIT)
#define MDR_RST_PER2_CLOCK_CLK_DMA        (1 << MDR_RST_PER2_CLOCK_CLK_DMA_BIT)
#define MDR_RST_PER2_CLOCK_CLK_I2C        (1 << MDR_RST_PER2_CLOCK_CLK_I2C_BIT)
#define MDR_RST_PER2_CLOCK_CLK_UART3      (1 << MDR_RST_PER2_CLOCK_CLK_UART3_BIT)
#define MDR_RST_PER2_CLOCK_CLK_ADC        (1 << MDR_RST_PER2_CLOCK_CLK_ADC_BIT)
#define MDR_RST_PER2_CLOCK_CLK_WWDG       (1 << MDR_RST_PER2_CLOCK_CLK_WWDG_BIT)
#define MDR_RST_PER2_CLOCK_CLK_IWDG       (1 << MDR_RST_PER2_CLOCK_CLK_IWDG_BIT)
#define MDR_RST_PER2_CLOCK_CLK_POWER      (1 << MDR_RST_PER2_CLOCK_CLK_POWER_BIT)
#define MDR_RST_PER2_CLOCK_CLK_BKP        (1 << MDR_RST_PER2_CLOCK_CLK_BKP_BIT)
#define MDR_RST_PER2_CLOCK_CLK_ADCUI      (1 << MDR_RST_PER2_CLOCK_CLK_ADCUI_BIT)
#define MDR_RST_PER2_CLOCK_CLK_TIMER1     (1 << MDR_RST_PER2_CLOCK_CLK_TIMER1_BIT)
#define MDR_RST_PER2_CLOCK_CLK_TIMER2     (1 << MDR_RST_PER2_CLOCK_CLK_TIMER2_BIT)
#define MDR_RST_PER2_CLOCK_CLK_PORTA       (1 << MDR_RST_PER2_CLOCK_CLK_PORTA_BIT)
#define MDR_RST_PER2_CLOCK_CLK_PORTB       (1 << MDR_RST_PER2_CLOCK_CLK_PORTB_BIT)
#define MDR_RST_PER2_CLOCK_CLK_PORTC       (1 << MDR_RST_PER2_CLOCK_CLK_PORTC_BIT)

#define MDR_RST_PER2_CLOCK_CLK_UART4      (1 << MDR_RST_PER2_CLOCK_CLK_UART4_BIT)
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

int milandr_uart_clock_setup(int uart_num) {
    /* XXX to the common part (clk_init)*/
    /* PER1_C2_SEL = 00 – CPU_C1 */
    MDR_RST_CLK->PER1_CLOCK &= ~(3 << RST_CLK_PER1_CLOCK_PER1_C2_SEL_Pos);

    switch(uart_num) {
        case 1:
            MDR_RST_CLK->UART_CLOCK &= ~(RST_CLK_UART_CLOCK_UART_BRG_Mask << RST_CLK_UART_CLOCK_UART1_BRG_Pos);
            MDR_RST_CLK->UART_CLOCK |=  ((RST_CLK_PER1_PRESCALER_DIV_1 << RST_CLK_UART_CLOCK_UART1_BRG_Pos)
                                        | (1UL << RST_CLK_UART_CLOCK_UART1_CLK_EN_Pos));
            break;
        case 2:
            MDR_RST_CLK->UART_CLOCK &= ~(RST_CLK_UART_CLOCK_UART_BRG_Mask << RST_CLK_UART_CLOCK_UART2_BRG_Pos);
            MDR_RST_CLK->UART_CLOCK |= ((RST_CLK_PER1_PRESCALER_DIV_1 << RST_CLK_UART_CLOCK_UART2_BRG_Pos)
                                        | (1 << RST_CLK_UART_CLOCK_UART2_CLK_EN_Pos));
            break;
        case 3:
            MDR_RST_CLK->UART_CLOCK &= ~(RST_CLK_UART_CLOCK_UART_BRG_Mask << RST_CLK_UART_CLOCK_UART3_BRG_Pos);
            MDR_RST_CLK->UART_CLOCK |= ((RST_CLK_PER1_PRESCALER_DIV_1 << RST_CLK_UART_CLOCK_UART3_BRG_Pos)
                                        | (1 << RST_CLK_UART_CLOCK_UART3_CLK_EN_Pos));
            break;
        case 4:
            MDR_RST_CLK->UART_CLOCK &= ~(RST_CLK_UART_CLOCK_UART_BRG_Mask << RST_CLK_UART_CLOCK_UART4_BRG_Pos);
            MDR_RST_CLK->UART_CLOCK |= ((RST_CLK_PER1_PRESCALER_DIV_1 << RST_CLK_UART_CLOCK_UART4_BRG_Pos)
                                        | (1 << RST_CLK_UART_CLOCK_UART4_CLK_EN_Pos));
            break;
        default:
            return -ENOSUPP;

    }

    /*enable*/
    switch(uart_num) {
        case 1:
            MDR_RST_CLK->PER2_CLOCK |= MDR_RST_PER2_CLOCK_CLK_UART1;
            break;
        case 2:
            MDR_RST_CLK->PER2_CLOCK |= MDR_RST_PER2_CLOCK_CLK_UART2;
            break;
        case 3:
            MDR_RST_CLK->PER2_CLOCK |= MDR_RST_PER2_CLOCK_CLK_UART3;
            break;
        case 4:
            MDR_RST_CLK->PER2_CLOCK |= MDR_RST_PER2_CLOCK_CLK_UART4;
            break;
        default:
            return -ENOSUPP;


    }

    return 0;
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
        milandr_uart_clock_setup(num);
        return 0;
    }

    return -ENOSUPP;
}
