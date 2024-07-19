/**
 * @file
 * 
 * @author Anton Bondarev
 * @date 08.07.2024
 */

#include <util/log.h>

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <drivers/common/memory.h>

#include <framework/mod/options.h>


#define PM_DIV_AHB_OFFSET           0x00
#define PM_DIV_APB_M_OFFSET         0x04
#define PM_DIV_APB_P_OFFSET         0x08
#define PM_CLK_AHB_SET_OFFSET       0x0C
#define PM_CLK_AHB_CLEAR_OFFSET     0x10
#define PM_CLK_APB_M_SET_OFFSET     0x14
#define PM_CLK_APB_M_CLEAR_OFFSET   0x18
#define PM_CLK_APB_P_SET_OFFSET     0x1C
#define PM_CLK_APB_P_CLEAR_OFFSET   0x20
#define PM_AHB_CLK_MUX_OFFSET       0x24
#define PM_WDT_CLK_MUX_OFFSET       0x28
#define PM_CPU_RTC_CLK_MUX_OFFSET   0x2C
#define PM_TIMER_CFG_OFFSET         0x30
#define PM_FREQ_MASK_OFFSET         0x34
#define PM_FREQ_STATUS_OFFSET       0x38
#define PM_SLEEP_MODE_OFFSET        0x3C


#define PM_AHB_CLK_MUX_S             0
#define PM_AHB_CLK_MUX_M            (0x3 << PM_AHB_CLK_MUX_S)
#define PM_AHB_CLK_MUX_OSC32M_M     (0x0 << PM_AHB_CLK_MUX_S)
#define PM_AHB_CLK_MUX_HSI32M_M     (0x1 << PM_AHB_CLK_MUX_S)
#define PM_AHB_CLK_MUX_OSC32K_M     (0x2 << PM_AHB_CLK_MUX_S)
#define PM_AHB_CLK_MUX_LSI32K_M     (0x3 << PM_AHB_CLK_MUX_S)
#define PM_AHB_FORCE_MUX_S           2
#define PM_AHB_FORCE_MUX_M          (1 << PM_AHB_FORCE_MUX_S)
#define PM_AHB_FORCE_MUX_FIXED      (1 << PM_AHB_FORCE_MUX_S)
#define PM_AHB_FORCE_MUX_UNFIXED    (0 << PM_AHB_FORCE_MUX_S)

#define PM_WDT_CLK_MUX_S            0
#define PM_WDT_CLK_MUX_M            (3 << PM_WDT_CLK_MUX_S)
#define PM_WDT_CLK_MUX_OSC32M_M     (0 << PM_WDT_CLK_MUX_S)
#define PM_WDT_CLK_MUX_HSI32M_M     (1 << PM_WDT_CLK_MUX_S)
#define PM_WDT_CLK_MUX_OSC32K_M     (2 << PM_WDT_CLK_MUX_S)
#define PM_WDT_CLK_MUX_LSI32K_M     (3 << PM_WDT_CLK_MUX_S)

#define PM_CPU_RTC_CLK_MUX_S             0
#define PM_CPU_RTC_CLK_MUX_M            (1 << PM_CPU_RTC_CLK_MUX_S)
#define PM_CPU_RTC_CLK_MUX_OSC32K_M     (0 << PM_CPU_RTC_CLK_MUX_S)
#define PM_CPU_RTC_CLK_MUX_LSI32K_M     (1 << PM_CPU_RTC_CLK_MUX_S)

#define PM_TIMER_CFG_MUX_TIMER32_0_S    0
#define PM_TIMER_CFG_MUX_TIMER32_1_S    3
#define PM_TIMER_CFG_MUX_TIMER32_2_S    6
#define PM_TIMER_CFG_MUX_TIMER32_TIM1_SYS_CLK_M(timer_index)  (0b00 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER32_TIM1_HCLK_M(timer_index)     (0b01 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER32_TIM2_OSC32K_M(timer_index)   (0b01 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER32_TIM2_LSI32K_M(timer_index)   (0b10 << (timer_index))

#define PM_TIMER_CFG_MUX_TIMER16_0_S    9
#define PM_TIMER_CFG_MUX_TIMER16_1_S    12
#define PM_TIMER_CFG_MUX_TIMER16_2_S    15
#define PM_TIMER_CFG_MUX_TIMER16_SYS_CLK_M(timer_index)     (0x0 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER16_HCLK_M(timer_index)     (0x1 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER16_OSC32M_M(timer_index)     (0x2 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER16_HSI32M_M(timer_index)     (0x3 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER16_OSC32K_M(timer_index)     (0x4 << (timer_index))
#define PM_TIMER_CFG_MUX_TIMER16_LSI32K_M(timer_index)     (0x5 << (timer_index))

#define PM_TIMER_CFG_MUX_TIMER_M(timer_index)     (0x7 << (timer_index))

#define PM_FREQ_MASK_FORCE_DIV_S        4
#define PM_FREQ_MASK_FORCE_DIV_M        (1 << PM_FREQ_MASK_FORCE_DIV_S)
#define PM_FREQ_MASK_OSC32M_S           3
#define PM_FREQ_MASK_OSC32M_M           (1 << PM_FREQ_MASK_OSC32M_S)
#define PM_FREQ_MASK_HSI32M_S           2
#define PM_FREQ_MASK_HSI32M_M           (1 << PM_FREQ_MASK_HSI32M_S)
#define PM_FREQ_MASK_OSC32K_S           1
#define PM_FREQ_MASK_OSC32K_M           (1 << PM_FREQ_MASK_OSC32K_S)
#define PM_FREQ_MASK_LSI32K_S           0
#define PM_FREQ_MASK_LSI32K_M           (1 << PM_FREQ_MASK_LSI32K_S)

#define PM_FREQ_STATUS_OSC32M_S         3
#define PM_FREQ_STATUS_OSC32M_M         (1 << PM_FREQ_STATUS_OSC32M_S)
#define PM_FREQ_STATUS_HSI32M_S         2
#define PM_FREQ_STATUS_HSI32M_M         (1 << PM_FREQ_STATUS_HSI32M_S)
#define PM_FREQ_STATUS_OSC32K_S         1
#define PM_FREQ_STATUS_OSC32K_M         (1 << PM_FREQ_STATUS_OSC32K_S)
#define PM_FREQ_STATUS_LSI32K_S         0
#define PM_FREQ_STATUS_LSI32K_M         (1 << PM_FREQ_STATUS_LSI32K_S)

#define PM_SLEEP_MODE_S     0
#define PM_SLEEP_MODE_M    (1 << PM_SLEEP_MODE_S)


// Clock gating masks to be used with PM module 
//AHB BUS
#define PM_CLOCK_AHB_CPU_S              0
#define PM_CLOCK_AHB_CPU_M              (1 << PM_CLOCK_AHB_CPU_S)
#define PM_CLOCK_AHB_EEPROM_S           1
#define PM_CLOCK_AHB_EEPROM_M           (1 << PM_CLOCK_AHB_EEPROM_S)
#define PM_CLOCK_AHB_RAM_S              2
#define PM_CLOCK_AHB_RAM_M              (1 << PM_CLOCK_AHB_RAM_S)
#define PM_CLOCK_AHB_SPIFI_S            3
#define PM_CLOCK_AHB_SPIFI_M            (1 << PM_CLOCK_AHB_SPIFI_S)
#define PM_CLOCK_AHB_TCB_S              4
#define PM_CLOCK_AHB_TCB_M              (1 << PM_CLOCK_AHB_TCB_S)
#define PM_CLOCK_AHB_DMA_S              5
#define PM_CLOCK_AHB_DMA_M              (1 << PM_CLOCK_AHB_DMA_S)
#define PM_CLOCK_AHB_CRYPTO_S           6
#define PM_CLOCK_AHB_CRYPTO_M           (1 << PM_CLOCK_AHB_CRYPTO_S)
#define PM_CLOCK_AHB_CRC32_S            7
#define PM_CLOCK_AHB_CRC32_M            (1 << PM_CLOCK_AHB_CRC32_S)


//APB M BUS
#define PM_CLOCK_APB_M_PM_S               0
#define PM_CLOCK_APB_M_PM_M               (1 << PM_CLOCK_APB_M_PM_S)
#define PM_CLOCK_APB_M_EPIC_S             1
#define PM_CLOCK_APB_M_EPIC_M             (1 << PM_CLOCK_APB_M_EPIC_S)
#define PM_CLOCK_APB_M_TIMER32_0_S        2
#define PM_CLOCK_APB_M_TIMER32_0_M        (1 << PM_CLOCK_APB_M_TIMER32_0_S)
#define PM_CLOCK_APB_M_PAD_CONFIG_S       3
#define PM_CLOCK_APB_M_PAD_CONFIG_M       (1 << PM_CLOCK_APB_M_PAD_CONFIG_S)
#define PM_CLOCK_APB_M_WDT_BUS_S          4
#define PM_CLOCK_APB_M_WDT_BUS_M          (1 << PM_CLOCK_APB_M_WDT_BUS_S)
#define PM_CLOCK_APB_M_OTP_CONTROLLER_S   5
#define PM_CLOCK_APB_M_OTP_CONTROLLER_M   (1 << PM_CLOCK_APB_M_OTP_CONTROLLER_S)
#define PM_CLOCK_APB_M_PVD_CONTROL_S      6
#define PM_CLOCK_APB_M_PVD_CONTROL_M      (1 << PM_CLOCK_APB_M_PVD_CONTROL_S)
#define PM_CLOCK_APB_M_WU_S               7
#define PM_CLOCK_APB_M_WU_M               (1 << PM_CLOCK_APB_M_WU_S)
#define PM_CLOCK_APB_M_RTC_S              8
#define PM_CLOCK_APB_M_RTC_M              (1 << PM_CLOCK_APB_M_RTC_S)

//APB P BUS
#define PM_CLOCK_APB_P_WDT_S          0
#define PM_CLOCK_APB_P_WDT_M          (1 << PM_CLOCK_APB_P_WDT_S)
#define PM_CLOCK_APB_P_UART_0_S       1
#define PM_CLOCK_APB_P_UART_0_M       (1 << PM_CLOCK_APB_P_UART_0_S)
#define PM_CLOCK_APB_P_UART_1_S       2
#define PM_CLOCK_APB_P_UART_1_M       (1 << PM_CLOCK_APB_P_UART_1_S)
#define PM_CLOCK_APB_P_TIMER16_0_S    3
#define PM_CLOCK_APB_P_TIMER16_0_M    (1 << PM_CLOCK_APB_P_TIMER16_0_S)
#define PM_CLOCK_APB_P_TIMER16_1_S    4
#define PM_CLOCK_APB_P_TIMER16_1_M    (1 << PM_CLOCK_APB_P_TIMER16_1_S)
#define PM_CLOCK_APB_P_TIMER16_2_S    5
#define PM_CLOCK_APB_P_TIMER16_2_M    (1 << PM_CLOCK_APB_P_TIMER16_2_S)
#define PM_CLOCK_APB_P_TIMER32_1_S    6
#define PM_CLOCK_APB_P_TIMER32_1_M    (1 << PM_CLOCK_APB_P_TIMER32_1_S)
#define PM_CLOCK_APB_P_TIMER32_2_S    7
#define PM_CLOCK_APB_P_TIMER32_2_M    (1 << PM_CLOCK_APB_P_TIMER32_2_S)
#define PM_CLOCK_APB_P_SPI_0_S        8
#define PM_CLOCK_APB_P_SPI_0_M        (1 << PM_CLOCK_APB_P_SPI_0_S)
#define PM_CLOCK_APB_P_SPI_1_S        9
#define PM_CLOCK_APB_P_SPI_1_M        (1 << PM_CLOCK_APB_P_SPI_1_S)
#define PM_CLOCK_APB_P_I2C_0_S        10
#define PM_CLOCK_APB_P_I2C_0_M        (1 << PM_CLOCK_APB_P_I2C_0_S)
#define PM_CLOCK_APB_P_I2C_1_S        11
#define PM_CLOCK_APB_P_I2C_1_M        (1 << PM_CLOCK_APB_P_I2C_1_S)
#define PM_CLOCK_APB_P_GPIO_0_S       12
#define PM_CLOCK_APB_P_GPIO_0_M       (1 << PM_CLOCK_APB_P_GPIO_0_S)
#define PM_CLOCK_APB_P_GPIO_1_S       13
#define PM_CLOCK_APB_P_GPIO_1_M       (1 << PM_CLOCK_APB_P_GPIO_1_S)
#define PM_CLOCK_APB_P_GPIO_2_S       14
#define PM_CLOCK_APB_P_GPIO_2_M       (1 << PM_CLOCK_APB_P_GPIO_2_S)
#define PM_CLOCK_APB_P_ANALOG_REGS_S  15
#define PM_CLOCK_APB_P_ANALOG_REGS_M  (1 << PM_CLOCK_APB_P_ANALOG_REGS_S)
#define PM_CLOCK_APB_P_GPIO_IRQ_S     16
#define PM_CLOCK_APB_P_GPIO_IRQ_M     (1 << PM_CLOCK_APB_P_GPIO_IRQ_S)


struct mikron_pm_regs { 
    volatile uint32_t DIV_AHB;         
    volatile uint32_t DIV_APB_M;         
    volatile uint32_t DIV_APB_P;         
    volatile uint32_t CLK_AHB_SET;
    volatile uint32_t CLK_AHB_CLEAR;         
    volatile uint32_t CLK_APB_M_SET;
    volatile uint32_t CLK_APB_M_CLEAR;         
    volatile uint32_t CLK_APB_P_SET;
    volatile uint32_t CLK_APB_P_CLEAR;         
    volatile uint32_t AHB_CLK_MUX;
    volatile uint32_t WDT_CLK_MUX;    
    volatile uint32_t CPU_RTC_CLK_MUX;   
    volatile uint32_t TIMER_CFG;  
    volatile uint32_t FREQ_MASK;  
    volatile uint32_t FREQ_STATUS;  
    volatile uint32_t SLEEP_MODE;
};

#define BASE_ADDR  OPTION_GET(NUMBER, base_addr)

#define PM    ((volatile struct mikron_pm_regs *) BASE_ADDR)

void mikron_pm_init(void) {

    PM->CLK_APB_P_SET = 0;
	PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_GPIO_0_M;
	PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_GPIO_1_M;
	PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_GPIO_2_M;
	PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_GPIO_IRQ_M;

	PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_TIMER32_1_M;

    PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_UART_0_M;
    PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_UART_1_M;

	PM->CLK_APB_M_SET =   PM_CLOCK_APB_M_PAD_CONFIG_M
						| PM_CLOCK_APB_M_WU_M
						| PM_CLOCK_APB_M_PM_M
						| PM_CLOCK_APB_M_EPIC_M
						| PM_CLOCK_APB_M_RTC_M
						| PM_CLOCK_APB_M_OTP_CONTROLLER_M;

	PM->CLK_AHB_SET |= PM_CLOCK_AHB_SPIFI_M;
}

void mikron_pm_set_ahb_div(uint32_t div) {
    PM->DIV_AHB = div;
}

void mikron_pm_set_apbm_div(uint32_t div) {
    PM->DIV_APB_M = div;
}

void mikron_pm_set_apbp_div(uint32_t div) {
    PM->DIV_APB_P = div;
}

int clk_enable(char *clk_name) {
    return 0;
}
