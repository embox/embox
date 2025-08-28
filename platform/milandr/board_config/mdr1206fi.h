#ifndef MILANDR_MDR1206FI_BOARD_CONFIG_H_
#define MILANDR_MDR1206FI_BOARD_CONFIG_H_

#define FLASH_BASE    (0x10000000UL)
#define FLASH_SIZE    (0x00080000UL)

#define RAM_AHB_BASE  (0x20000000UL)
#define RAM_AHB_SIZE  (0x00004000UL)

#define RAM_TCMA_BASE (0x40000000UL)
#define RAM_TCMA_SIZE (0x00010000UL)

#define RAM_TCMB_BASE (0x40010000UL)
#define RAM_TCMB_SIZE (0x00008000UL)

#define PERIPH_BASE   (0x50000000UL)



#define MDR_SSP1_BASE    (PERIPH_BASE + 0x00000000UL)
#define MDR_UART1_BASE   (PERIPH_BASE + 0x00008000UL)
#define MDR_UART2_BASE   (PERIPH_BASE + 0x00010000UL)
#define MDR_FLASH_BASE   (PERIPH_BASE + 0x00018000UL)
#define MDR_RST_CLK_BASE (PERIPH_BASE + 0x00020000UL)
#define MDR_DMA_BASE     (PERIPH_BASE + 0x00028000UL)
#define MDR_I2C_BASE     (PERIPH_BASE + 0x00030000UL)
#define MDR_UART3_BASE   (PERIPH_BASE + 0x00038000UL)
#define MDR_ADC_BASE     (PERIPH_BASE + 0x00040000UL)
#define MDR_WWDG_BASE    (PERIPH_BASE + 0x00048000UL)
#define MDR_IWDG_BASE    (PERIPH_BASE + 0x00050000UL)
#define MDR_POWER_BASE   (PERIPH_BASE + 0x00058000UL)
#define MDR_BKP_BASE     (PERIPH_BASE + 0x00060000UL)
#define MDR_ADCUI_BASE   (PERIPH_BASE + 0x00068000UL)
#define MDR_TIMER1_BASE  (PERIPH_BASE + 0x00070000UL)
#define MDR_TIMER2_BASE  (PERIPH_BASE + 0x00078000UL)
#define MDR_PORTA_BASE   (PERIPH_BASE + 0x00080000UL)
#define MDR_PORTB_BASE   (PERIPH_BASE + 0x00088000UL)
#define MDR_PORTC_BASE   (PERIPH_BASE + 0x00090000UL)
#define MDR_CRC_BASE     (PERIPH_BASE + 0x00098000UL)

#define MDR_RANDOM_BASE (PERIPH_BASE + 0x000B8000UL)
#define MDR_USART_BASE  (PERIPH_BASE + 0x000C0000UL)
#define MDR_SSP2_BASE   (PERIPH_BASE + 0x000C8000UL)
#define MDR_SSP3_BASE   (PERIPH_BASE + 0x000D0000UL)
#define MDR_TIMER3_BASE (PERIPH_BASE + 0x000D8000UL)
#define MDR_TIMER4_BASE (PERIPH_BASE + 0x000E0000UL)
#define MDR_UART4_BASE  (PERIPH_BASE + 0x000E8000UL)
#define MDR_PORTD_BASE  (PERIPH_BASE + 0x000F0000UL)

#define MDR_USIP_IRQ              0 // 0:  User Software Interrupt (software request using pending bit)
#define MDR_MSIP_IRQ              3 // 3:  Machine Software Interrupt
#define MDR_UTIP_IRQ              4 // 4:  User Timer Interrupt
#define MDR_MTIP_IRQ              7 // 7:  Machine Timer Interrupt
#define MDR_CSIP_IRQ             12 // 12: CLIC Software Interrupt (software request using pending bit)
#define MDR_DMA_IRQ              16 // 16: DMA Handler
#define MDR_UART1_IRQ            17 // 17: UART1 Handler
#define MDR_UART2_IRQ            18 // 18: UART2 Handler
#define MDR_SSP1_IRQ             19 // 19: SSP1 Handler
#define MDR_POWER_IRQ            20 // 20: POWER Handler
#define MDR_WWDG_IRQ             21 // 21: WWDG Handler
#define MDR_TIMER1_IRQ           22 // 22: TIMER1 Handler
#define MDR_TIMER2_IRQ           23 // 23: TIMER2 Handler
#define MDR_ADC_IRQ              24 // 24: ADC Handler
#define MDR_I2C_IRQ              25 // 25: I2C Handler
#define MDR_BKP_IRQ              26 // 26: BKP and RTC Handler
#define MDR_EXT_INT1_IRQ         27 // 27: EXT_INT1 Handler
#define MDR_EXT_INT2_IRQ         28 // 28: EXT_INT2 Handler
#define MDR_EXT_INT3_IRQ         29 // 29: EXT_INT3 Handler
#define MDR_ADCUI_F03_IRQ        30 // 30: ADCUI_F03 Handler
#define MDR_ADCUI_F1_IRQ         31 // 31: ADCUI_F1 Handler
#define MDR_ADCUI_F2_IRQ         32 // 32: ADCUI_F2 Handler
#define MDR_RANDOM_IRQ           37 // 37: RANDOM Handler
#define MDR_USART_IRQ            38 // 38: UART ISO7816 (USART) Handler
#define MDR_UART3_IRQ            39 // 39: UART3 Handler
#define MDR_SSP2_IRQ             40 // 40: SSP2 Handler
#define MDR_SSP3_IRQ             41 // 41: SSP3 Handler
#define MDR_TIMER3_IRQ           42 // 42: TIMER3 Handler
#define MDR_TIMER4_IRQ           43 // 43: TIMER4 Handler
#define MDR_UART4_IRQ            44 // 44: UART4 Handler


#endif /* MILANDR_MDR1206FI_BOARD_CONFIG_H_ */