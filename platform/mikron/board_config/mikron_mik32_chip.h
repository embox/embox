
#define DMA_CONFIG_BASE_ADDRESS             0x000040000

#define APB_DOM_0_BASE_ADDRESS              0x000050000
#define PM_BASE_ADDRESS                     0x000050000
#define EPIC_BASE_ADDRESS                   0x000050400
#define TIMER32_0_BASE_ADDRESS              0x000050800
#define PAD_CONFIG_BASE_ADDRESS             0x000050c00
#define WDT_BUS_BASE_ADDRESS                0x000051000
#define OTP_BASE_ADDRESS                    0x000051400
#define PVD_CONTROL_BASE_ADDRESS            0x000051800

#define WU_BASE_ADDRESS                     0x00060000
#define RTC_BASE_ADDRESS                    0x00060400
#define BOOT_MANAGER_BASE_ADDRESS           0x00060800

#define SPIFI_CONFIG_BASE_ADDRESS           0x00070000
#define EEPROM_REGS_BASE_ADDRESS            0x00070400
#define CRYPTO_BASE_ADDRESS                 0x00080000
#define CRC_BASE_ADDRESS                    0x00080400

#define APB_DOM_3_BASE_ADDRESS              0x00081000

#define WDT_BASE_ADDRESS                    0x00081000
#define UART_0_BASE_ADDRESS                 0x00081400
#define UART_1_BASE_ADDRESS                 0x00081800
#define TIMER16_0_BASE_ADDRESS              0x00081C00
#define TIMER16_1_BASE_ADDRESS              0x00082000
#define TIMER16_2_BASE_ADDRESS              0x00082400
#define TIMER32_1_BASE_ADDRESS              0x00082800
#define TIMER32_2_BASE_ADDRESS              0x00082C00
#define SPI_0_BASE_ADDRESS                  0x00083000
#define SPI_1_BASE_ADDRESS                  0x00083400
#define I2C_0_BASE_ADDRESS                  0x00083800
#define I2C_1_BASE_ADDRESS                  0x00083C00
#define GPIO_0_BASE_ADDRESS                 0x00084000
#define GPIO_1_BASE_ADDRESS                 0x00084400
#define GPIO_2_BASE_ADDRESS                 0x00084800
#define GPIO_IRQ_BASE_ADDRESS               0x00084C00
#define ANALOG_REG_BASE_ADDRESS             0x00085000


#define SCR1_TIMER_BASE_ADDRESS             0x00490000


/* Interrupt lines to be used with EPIC module */
#define EPIC_TIMER32_0_INDEX        0
#define EPIC_UART_0_INDEX           1
#define EPIC_UART_1_INDEX           2
#define EPIC_SPI_0_INDEX            3
#define EPIC_SPI_1_INDEX            4
#define EPIC_GPIO_IRQ_INDEX         5
#define EPIC_I2C_0_INDEX            6
#define EPIC_I2C_1_INDEX            7
#define EPIC_WDT_INDEX              8
#define EPIC_TIMER16_0_INDEX        9
#define EPIC_TIMER16_1_INDEX        10
#define EPIC_TIMER16_2_INDEX        11
#define EPIC_TIMER32_1_INDEX        12
#define EPIC_TIMER32_2_INDEX        13
#define EPIC_SPIFI_INDEX            14
#define EPIC_RTC_INDEX              15
#define EPIC_EEPROM_INDEX           16
#define EPIC_WDT_DOM3_INDEX         17
#define EPIC_WDT_SPIFI_INDEX        18
#define EPIC_WDT_EEPROM_INDEX       19
#define EPIC_DMA_INDEX              20
#define EPIC_FREQ_MON_INDEX         21
#define EPIC_PVD_AVCC_UNDER         22
#define EPIC_PVD_AVCC_OVER          23
#define EPIC_PVD_VCC_UNDER          24
#define EPIC_PVD_VCC_OVER           25
#define EPIC_BATTERY_NON_GOOD       26
#define EPIC_BOR_INDEX              27
#define EPIC_TSENS_INDEX            28
#define EPIC_ADC_INDEX              29
#define EPIC_DAC0_INDEX             30
#define EPIC_DAC1_INDEX             31

